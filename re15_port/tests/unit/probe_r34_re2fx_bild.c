/*
 * probe_r34_re2fx_bild — Runde 34 Spur D, Paket 6 / O9: die Aufschlag-Kinder OFFSCREEN zeichnen.
 *
 * Laeuft die Saeure- und die Brand-Folge der echten RE2-FX-Maschine (engine/src/re2_fx.c) vor einer
 * festen Kamera, baut je Bild die POLY_FT4-Quads mit re2fx_quads() (FUN_80077924/FUN_80077ed0) und
 * rastert sie in einen 320x240-Puffer — Texel-Abruf ueber ein VRAM-MODELL (TEX.TIM-Bild nach
 * (768,256), CLUT nach (256,480), 4-bpp-Seitenadressierung aus dem TPage-Wort, CLUT-Wort -> (x*16, y)),
 * Halbtransparenz je Texel mit STP-Bit nach den PSX-ABR-Regeln (psx-spx "Semi Transparency"). Das ist
 * bewusst UNABHAENGIG vom PC-Zeichner (re2fx_pc.c schneidet die Seiten) und vom Python-Katalog
 * (tools/re2fx_katalog.py dekodiert direkt aus der TIM).
 *
 * Ausgaben (Verzeichnis = argv[1] oder "."):
 *   re2fx_<folge>_<bild>.ppm   Bild je Spielbild (P6, 2x vergroessert)
 *   re2fx_quads.txt            je Quad: Folge Bild Platz Bank Sub TPage CLUT u0 v0 u1 v1 x0 y0 x1 y1 Code
 *   re2fx_crops.txt            je eindeutigem (TPage,CLUT,u0,v0,u1,v1): die RGB555-Texel (Katalog-Vergleich)
 *
 * Pruefungen: 1 Dateien, 10/11 keine Quads vor dem Aufschlag (Negativ-Kontrolle), 12/13 Seiten nur 0x1E/0x1F
 * und CLUT-Spalte 272, 14 UV ohne Byte-Ueberlauf, 15 jedes gezeichnete Sprite (Platz) hat deckende Texel
 * (einzelne leere Ecken-Zellen der 3x3/4x4-Verbuende sind Daten und werden nur gezaehlt), 16/17 Saeure/Brand
 * zeichnen in den Bildern X+1.. sichtbare Pixel. Rueckgabe 0 = gruen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_fx.h"
#include "re15_camera.h"
#include "re15_ai_flavor.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static uint16_t s_vram[512][1024];
static uint8_t  s_rgb[240][320][3];
static const char *s_dir = ".";
static FILE *s_qf, *s_cf;

static uint8_t *lesen(const char *name, long *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/../RE2/%s", RE15_XSTR(RE15_ASSETS_PATH), name);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return NULL; }
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)*n);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f);
    return b;
}
static uint16_t le16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }
static uint32_t le32(const uint8_t *b) { return (uint32_t)le16(b) | ((uint32_t)le16(b + 2) << 16); }

/* TIM ins VRAM-Modell wie der Lader FUN_80076a40 mit 0x800CFBF0 = 28 / 0x800CFBF1 = 0 (Halbwort-Store
 * `addiu v0,zero,28 / sh v0,-1040(at)` @0x8002b8cc-d4): Bild nach (28*64-1024, 256) = (768,256)
 * (@0x80076a64-a8, die Datei selbst sagt (0,0)), CLUT nach (Datei-x, 0 + 480) (@0x80076b00-0c). */
static int tim_ins_vram(const uint8_t *t, long n)
{
    if (n < 20 || le32(t) != 0x10u) return -1;
    uint32_t csz = le32(t + 8);
    int cx = le16(t + 12), cy = 480, cw = le16(t + 16), ch = le16(t + 18);
    for (int y = 0; y < ch; y++) for (int x = 0; x < cw; x++) s_vram[cy + y][cx + x] = le16(t + 20 + (size_t)(y * cw + x) * 2);
    const uint8_t *im = t + 8 + csz;
    int ix = 28 * 64 - 1024, iy = 256, iw = le16(im + 8), ih = le16(im + 10);
    if ((long)(im - t) + 12 + (long)iw * ih * 2 > n) return -2;
    for (int y = 0; y < ih; y++) for (int x = 0; x < iw; x++) s_vram[iy + y][ix + x] = le16(im + 12 + (size_t)(y * iw + x) * 2);
    return 0;
}

/* PSX 4-bpp-Texelabruf: Seite aus TPage (Bits 0-3 x/64, Bit 4 y/256), CLUT (Bits 0-5 x/16, 6-14 y). */
static uint16_t texel(uint16_t tpage, uint16_t clut, int u, int v)
{
    int bx = (tpage & 0xF) * 64, by = ((tpage >> 4) & 1) * 256;
    uint16_t hw = s_vram[(by + v) & 511][(bx + (u >> 2)) & 1023];
    int idx = (hw >> ((u & 3) * 4)) & 0xF;
    return s_vram[(clut >> 6) & 0x1FF][((clut & 0x3F) * 16 + idx) & 1023];
}

static int s_pixel;   /* in diesem Bild gesetzte Pixel */
static void quad_rastern(const re2fx_quad_t *q)
{
    int w = q->x1 - q->x0, h = q->y1 - q->y0;
    if (w <= 0 || h <= 0) return;
    int du = q->u1 - q->u0, dv = q->v1 - q->v0;
    int abr = (q->tpage >> 5) & 3, semi = (q->code & 2) != 0;
    for (int y = q->y0; y < q->y1; y++) {
        if (y < 0 || y >= 240) continue;
        for (int x = q->x0; x < q->x1; x++) {
            if (x < 0 || x >= 320) continue;
            int u = q->u0 + (x - q->x0) * du / w, v = q->v0 + (y - q->y0) * dv / h;
            uint16_t c = texel(q->tpage, q->clut, u, v);
            if (c == 0) continue;                                      /* 0x0000 = durchsichtig */
            int f[3] = { (c & 31), ((c >> 5) & 31), ((c >> 10) & 31) };
            for (int k = 0; k < 3; k++) {
                int b = s_rgb[y][x][k] >> 3, r;
                if (semi && (c & 0x8000)) {
                    switch (abr) {
                    case 0:  r = (b + f[k]) / 2; break;
                    case 1:  r = b + f[k]; break;
                    case 2:  r = b - f[k]; break;
                    default: r = b + f[k] / 4; break;
                    }
                } else r = f[k];
                if (r < 0) r = 0;
                if (r > 31) r = 31;
                s_rgb[y][x][k] = (uint8_t)(r << 3);
            }
            s_pixel++;
        }
    }
}

static void bild_schreiben(const char *folge, int bild)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/re2fx_%s_%02d.ppm", s_dir, folge, bild);
    FILE *f = fopen(p, "wb");
    if (!f) return;
    fprintf(f, "P6\n640 480\n255\n");
    for (int y = 0; y < 480; y++) for (int x = 0; x < 640; x++) fwrite(s_rgb[y / 2][x / 2], 1, 3, f);
    fclose(f);
}

typedef struct { uint16_t tp, cl; uint8_t u0, v0, u1, v1; } crop_t;
static crop_t s_crops[4096]; static int s_ncrops;
static void crop_merken(const re2fx_quad_t *q)
{
    for (int i = 0; i < s_ncrops; i++)
        if (s_crops[i].tp == q->tpage && s_crops[i].cl == q->clut && s_crops[i].u0 == q->u0 && s_crops[i].v0 == q->v0 &&
            s_crops[i].u1 == q->u1 && s_crops[i].v1 == q->v1) return;
    if (s_ncrops >= 4096) return;
    crop_t *c = &s_crops[s_ncrops++];
    c->tp = q->tpage; c->cl = q->clut; c->u0 = q->u0; c->v0 = q->v0; c->u1 = q->u1; c->v1 = q->v1;
    fprintf(s_cf, "CROP %04X %04X %u %u %u %u\n", q->tpage, q->clut, q->u0, q->v0, q->u1, q->v1);
    for (int v = q->v0; v < q->v1; v++) {
        for (int u = q->u0; u < q->u1; u++) fprintf(s_cf, "%04x ", texel(q->tpage, q->clut, u, v));
        fprintf(s_cf, "\n");
    }
}

static re15_camera_view_t s_cam;
static int s_fehler;
static uint8_t s_platz_da[RE2FX_PLAETZE], s_platz_deckend[RE2FX_PLAETZE];
static int s_leere_zellen;
static int folge(const char *name, int art, int bilder, int *pix_nach)
{
    re2fx_reset(); re15_re2z_rng_reset();
    re2fx_boden_hook = NULL; re2fx_applier = NULL; re2fx_se_hook = NULL;
    const int32_t q[3] = { 0, 0, 0 };
    static re2fx_quad_t qs[1024];
    *pix_nach = 0;
    for (int b = 0; b < bilder; b++) {
        if (b == 1) re2fx_aufschlag(art, q, 0);                        /* Aufschlag im Bild 1 = X */
        re2fx_tick();
        int n = re2fx_quads(&s_cam, 160, 120, 208, 0, NULL, NULL, qs, 1024);   /* camf 26684>>7 (ROOM1140) */
        if (b == 0 && n != 0) s_fehler = s_fehler ? s_fehler : 10;     /* vor dem Aufschlag: nichts */
        memset(s_rgb, 64, sizeof s_rgb);
        memset(s_platz_da, 0, sizeof s_platz_da); memset(s_platz_deckend, 0, sizeof s_platz_deckend);
        s_pixel = 0;
        /* Zeichenfolge: fern zuerst (Port-Sortierung wie pc_draw_effects: Schluessel View-Z). */
        for (int pass = 0; pass < n; pass++) {
            int best = -1;
            for (int i = 0; i < n; i++) if (qs[i].platz >= 0 && (best < 0 || qs[i].vz > qs[best].vz)) best = i;
            if (best < 0) break;
            re2fx_quad_t e = qs[best]; qs[best].platz = -1;
            if ((e.tpage & 0x1F) != 0x1E && (e.tpage & 0x1F) != 0x1F) s_fehler = s_fehler ? s_fehler : 12;
            if ((e.clut & 0x3F) != 0x11) s_fehler = s_fehler ? s_fehler : 13;
            if (e.u1 <= e.u0 || e.v1 <= e.v0) s_fehler = s_fehler ? s_fehler : 14;
            int deckend = 0;
            for (int v = e.v0; v < e.v1 && !deckend; v++) for (int u = e.u0; u < e.u1; u++) if (texel(e.tpage, e.clut, u, v)) { deckend = 1; break; }
            if (deckend) s_platz_deckend[e.platz] = 1; else s_leere_zellen++;   /* leere Ecken-Zellen sind Daten */
            s_platz_da[e.platz] = 1;
            const uint8_t *pl = re2fx_platz(e.platz);
            fprintf(s_qf, "%s %d %d %u %02X %04X %04X %u %u %u %u %d %d %d %d %02X\n", name, b, e.platz, pl[0x1C], pl[0x1E],
                    e.tpage, e.clut, e.u0, e.v0, e.u1, e.v1, e.x0, e.y0, e.x1, e.y1, e.code);
            crop_merken(&e);
            quad_rastern(&e);
        }
        for (int i = 0; i < RE2FX_PLAETZE; i++)                        /* jedes gezeichnete Sprite hat Deckung */
            if (s_platz_da[i] && !s_platz_deckend[i]) s_fehler = s_fehler ? s_fehler : 15;
        if (b >= 2) *pix_nach += s_pixel;
        bild_schreiben(name, b);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1) s_dir = argv[1];
    long ne = 0, nt = 0;
    uint8_t *esp = lesen("CORE00.ESP", &ne), *tim = lesen("TEX.TIM", &nt);
    if (!esp || !tim || re2fx_register_core(esp, (size_t)ne) != 0 || tim_ins_vram(tim, nt) != 0) { printf("FAIL 1: Dateien\n"); return 1; }
    char p[1024];
    snprintf(p, sizeof p, "%s/re2fx_quads.txt", s_dir);  s_qf = fopen(p, "w");
    snprintf(p, sizeof p, "%s/re2fx_crops.txt", s_dir);  s_cf = fopen(p, "w");
    if (!s_qf || !s_cf) { printf("FAIL 1: Ausgabe %s\n", s_dir); return 1; }
    /* Kamera: 5 m hinter und 1,5 m ueber dem Aufschlag, Blick auf (1000,-400,0); fov wie ROOM1140. */
    re15_camera_cut_t cut; memset(&cut, 0, sizeof cut);
    cut.fov = 26684; cut.pos_x = -1500; cut.pos_y = -1800; cut.pos_z = -5200;
    cut.target_x = 1000; cut.target_y = -400; cut.target_z = 0;
    if (re15_camera_build_view(&cut, &s_cam) != 0) { printf("FAIL 1: Kamera\n"); return 1; }
    int ps = 0, pb = 0;
    folge("saeure", 2, 16, &ps);
    folge("brand", 1, 48, &pb);
    fclose(s_qf); fclose(s_cf);
    printf("Pixel nach dem Aufschlag: Saeure %d, Brand %d; eindeutige Ausschnitte %d, leere Zellen %d\n",
           ps, pb, s_ncrops, s_leere_zellen);
    if (s_fehler) { printf("FAIL %d\n", s_fehler); return s_fehler; }
    if (ps <= 0) { printf("FAIL 16: Saeure zeichnet nichts\n"); return 16; }
    if (pb <= 0) { printf("FAIL 17: Brand zeichnet nichts\n"); return 17; }
    printf("probe_r34_re2fx_bild: gruen (Ausgabe %s)\n", s_dir);
    return 0;
}
