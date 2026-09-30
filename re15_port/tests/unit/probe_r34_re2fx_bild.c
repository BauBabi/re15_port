/*
 * probe_r34_re2fx_bild — Runde 34 Spur D, Paket 6 / O9 + Nachbesserung N4 (Gegenpruefung M3): die
 * Aufschlag-Kinder OFFSCREEN zeichnen und die Billboard-Geometrie PINNEN.
 *
 * Laeuft die Saeure- und die Brand-Folge der echten RE2-FX-Maschine (engine/src/re2_fx.c) vor einer
 * festen Kamera, baut je Bild die POLY_FT4-Quads mit re2fx_quads() (FUN_80077924/FUN_80077ed0) und
 * rastert sie in einen 320x240-Puffer — Texel-Abruf ueber ein VRAM-MODELL (TEX.TIM-Bild nach
 * (768,256), CLUT nach (256,480), 4-bpp-Seitenadressierung aus dem TPage-Wort, CLUT-Wort -> (x*16, y)),
 * Halbtransparenz je Texel mit STP-Bit nach den PSX-ABR-Regeln (psx-spx "Semi Transparency").
 *
 * N4 (M3): JEDES Quad jedes Bilds wird gegen eine UNABHAENGIGE HANDRECHNUNG verglichen:
 *   - eigener CORE00.ESP-Leser (Bank-Offsets rueckwaerts ab dem letzten Wort wie FUN_8001bca0
 *     @0x8001bcc8-2c; Anim-Tafel Bank+8, UV-Tafel Bank+8+8*n1 wie FUN_8001cbe8 @0x8001cd54-64) — die
 *     Tafel-Zeiger +0x70/+0x74 der Maschine werden NICHT benutzt;
 *   - eigene RTPS (psx-spx GTE RTPS, sf = 1, lm = 0: MAC = (TR<<12 + R*V) >> 12, IR1/IR2 saettigen,
 *     SZ3 = MAC3 auf 0..0xFFFF, SX = (OFX + IR1*n) >> 16, n = UNR-Division re15_gte_divide);
 *   - FUN_80077ed0 von Hand aus der Disasm @0x80077f14-0x8007814c (step = Groesse*Skala*camf / (SZ<<4)
 *     `div` @0x80077fd8, Breite/Hoehe = step*Aspekt @0x8007800c/@0x80078038, Texelschritt `divu`
 *     @0x8007801c/@0x8007805c, Kanten-Trim 0x1FFFF @0x80078070-88, Ecken @0x80078090-100, UV @0x80078104-14c),
 *     Zelle/Anzahl/Groesse aus dem Anim-Eintrag (`lbu s7,0(s1)` / `lbu s3,1(s1)` / `lbu s6,3(s1)`
 *     @0x80077a20/58/5c).
 *   Die ANIM-/UV-FOLGE je Platz kommt aus CORE00.ESP: Start aus Skript-Schritt 0 (Op A 1 -> Anim := step[2]
 *   @0x8001dc40-4c; Op B 27 -> r%3 @0x8001faa4-f4; Op A 30 -> Op 2: step[2] + r%(step[0x16]+1)
 *   @0x8001dd70-a8), dann je Bild der naechste Tafel-Eintrag (Dauer 1), LOOP (Dauer 0xFF -> Zelle)
 *   @0x8001d824-50, ENDE (Dauer 0 und Zelle 0) gibt den Platz frei @0x8001d814-20.
 *
 * Ausgaben (Verzeichnis = argv[1] oder "."):
 *   re2fx_<folge>_<bild>.ppm   Bild je Spielbild (P6, 2x vergroessert)
 *   re2fx_quads.txt            je Quad: Folge Bild Platz Bank Sub TPage CLUT u0 v0 u1 v1 x0 y0 x1 y1 Code
 *   re2fx_crops.txt            je eindeutigem (TPage,CLUT,u0,v0,u1,v1): die RGB555-Texel (Katalog-Vergleich)
 *
 * Pruefungen: 1 Dateien, 10 keine Quads vor dem Aufschlag (Negativ-Kontrolle), 12/13 Seiten nur 0x1E/0x1F
 * und CLUT-Spalte 272, 14 UV ohne Byte-Ueberlauf, 15 jedes gezeichnete Sprite (Platz) hat deckende Texel,
 * 16/17 Saeure/Brand zeichnen in den Bildern X+1.. sichtbare Pixel;
 * N4: 20 Quads == Handrechnung (Zahl, Reihenfolge Platz 95 -> 0 / Zelle aufsteigend, x0 y0 x1 y1 u0 v0 u1 v1,
 * CLUT, TPage, Code), 21 Literal-Pins je Kind-Code, 22 Anim-Start aus der Skript-Startmenge, 23 Anim-Schritt
 * nach der ESP-Tafel, 24 Platz verschwindet nur am ENDE-Eintrag (Aufschlag-Kinder), 25 jede Kind-Folge
 * vollstaendig gezeichnet (Lebensdauer = Folgenlaenge), 26 Negativ-Kontrolle der Handrechnung (SZ<<3 statt
 * SZ<<4 weicht ab), 27/28/29 Datenannahmen (Kind-Code bekannt, Dauer 1, Start-Op bekannt). Rueckgabe 0 = gruen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_fx.h"
#include "re15_camera.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static uint16_t s_vram[512][1024];
static uint8_t  s_rgb[240][320][3];
static const char *s_dir = ".";
static FILE *s_qf, *s_cf;
static int s_fehler;
#define FEHLER(n) do { if (!s_fehler) s_fehler = (n); } while (0)

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

/* 30 (O9 in ctest): Texel direkt aus der TIM-DATEI, ohne VRAM-Modell (wie tools/re2fx_katalog.py Tex.texel):
 * Seite 0x1E/0x1F = Bild-hw-Spalte (Seite & 0xF)*64 - 768 (Bild liegt bei VRAM-x 768), CLUT-Zeile = y - 480,
 * CLUT-Spalte = x*16 - 256 (Datei-CLUT bei (256,480)). */
static const uint8_t *s_tim; static long s_tim_n;
static int s_texel_abw, s_texel_n;
static uint16_t tim_texel(uint16_t tpage, uint16_t clut, int u, int v)
{
    const uint32_t csz = le32(s_tim + 8);
    const int cw = le16(s_tim + 16);
    const uint8_t *im = s_tim + 8 + csz;
    const int iw = le16(im + 8);
    const int col = (tpage & 0xF) * 64 - 768;
    const long off = 12 + ((long)(v & 255) * iw + col + u / 4) * 2;
    if (col < 0 || (long)(im - s_tim) + off + 2 > s_tim_n) return 0xFFFF;
    const uint16_t hw = le16(im + off);
    const int idx = (hw >> ((u & 3) * 4)) & 0xF;
    const int zeile = ((clut >> 6) & 0x1FF) - 480, spalte = (clut & 0x3F) * 16 - 256;
    if (zeile < 0 || spalte < 0) return 0xFFFF;
    return le16(s_tim + 20 + (size_t)(zeile * cw + spalte + idx) * 2);
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
        for (int u = q->u0; u < q->u1; u++) {
            const uint16_t t = texel(q->tpage, q->clut, u, v);
            fprintf(s_cf, "%04x ", t);
            s_texel_n++;
            if (t != tim_texel(q->tpage, q->clut, u, v)) s_texel_abw++;
        }
        fprintf(s_cf, "\n");
    }
}

/* =============================================================================================
 * N4: unabhaengiger CORE00.ESP-Leser + Handrechnung FUN_80077ed0
 * =========================================================================================== */
static const uint8_t *s_esp; static long s_esp_n;
static int32_t bank_off(unsigned bank)
{
    /* FUN_8001bca0: Id-Bytes ab Dateianfang (`lbu v1,0(t1)` @0x8001bccc), Offsets RUECKWAERTS ab dem letzten
     * Wort (`lw v1,0(t2)` / `addiu t2,t2,-4` @0x8001bce0-f0; letztes Wort = align4(Groesse) - 4 @0x8001bb54-80). */
    long ende = ((s_esp_n + 3) & ~3L) - 4;
    for (int i = 0; i < 8; i++) {
        if (s_esp[i] == 0xFF) break;
        if (s_esp[i] == bank) return (int32_t)le32(s_esp + ende - 4 * i);
    }
    return -1;
}
/* Anim-Eintrag a der Bank (Bank+8, 8 Byte: Zelle, Anzahl, Dauer, Groesse; +0x70 = Bank+8 @0x8001cd54). */
static const uint8_t *anim_e(unsigned bank, unsigned a)
{
    int32_t o = bank_off(bank);
    if (o < 0 || a >= le16(s_esp + o)) return NULL;
    return s_esp + o + 8 + 8 * a;
}
/* UV-Eintrag der Zelle (Bank + 8 + 8*n1, 4 Byte: u, v, cx s8, cy s8; +0x74 @0x8001cd5c-64). */
static const uint8_t *uv_e(unsigned bank, unsigned zelle)
{
    int32_t o = bank_off(bank);
    if (o < 0) return NULL;
    return s_esp + o + 8 + 8 * le16(s_esp + o) + 4 * zelle;
}
/* Skript-Schritt 0 des Kind-Codes (Tabelle = Bank + ((w&0xffff)*2 + (w>>16) + 2)*4 @0x8001bd00-20,
 * Wort-Offset[sub&7] @0x8001cbfc-cc18, Skript + 4 = Teil 0, + 4 = Schritt 0 @0x8001cc38/@0x8001cd68-70). */
static const uint8_t *schritt0(unsigned bank, unsigned sub)
{
    int32_t o = bank_off(bank);
    if (o < 0) return NULL;
    uint32_t w = le32(s_esp + o);
    uint32_t tab = (uint32_t)o + ((w & 0xffffu) * 2u + (w >> 16) + 2u) * 4u;
    uint32_t sk = tab + (uint32_t)le16(s_esp + tab + 2 * (sub & 7u)) * 4u;
    return s_esp + sk + 8;
}
/* Startmenge der Anim aus Schritt 0 (s. Kopf). -1 = unbekanntes Start-Op. */
static int start_menge(unsigned bank, unsigned sub, int *lo, int *hi)
{
    const uint8_t *s = schritt0(bank, sub);
    if (!s) return -1;
    if (s[0] == 1)  { *lo = *hi = s[2]; return 0; }                      /* Op 1 */
    if (s[1] == 27) { *lo = 0; *hi = 2; return 0; }                        /* Op 27: r%3 */
    if (s[0] == 30) { *lo = s[2]; *hi = s[2] + le16(s + 0x16); return 0; } /* Op 30 -> Op 2 */
    return -1;
}
/* Naechster Anim-Index nach der Tafel (@0x8001d7c8-864): a+1; Dauer 0xFF = LOOP -> Zelle; Dauer 0 und Zelle 0
 * = ENDE (-1). Nur Dauer 1 ist in diesen Folgen belegt (Pruefung 28). */
static int naechste(unsigned bank, int a)
{
    const uint8_t *e = anim_e(bank, (unsigned)a + 1u);
    if (!e) return -2;
    if (e[2] == 0)    return (e[0] == 0) ? -1 : -3;
    if (e[2] == 0xFF) return e[0];
    return a + 1;
}

typedef struct { int16_t x0, y0, x1, y1; uint8_t u0, v0, u1, v1; uint16_t clut, tpage; uint8_t code; int platz; } hq_t;
static int s_hand_sz_shift = 4;           /* 4 = Original (`sll v0,v0,4` @0x80077fd4); 3 nur Negativ-Kontrolle 26 */
static const int k_cx = 160, k_cy = 120, k_camf = 208;
static re15_camera_view_t s_cam;
static int32_t clamp32(int64_t v, int32_t lo, int32_t hi) { return v < lo ? lo : (v > hi ? hi : (int32_t)v); }
/* RTPS nach psx-spx (Kamera = GTE-Rotation/-Translation, H = fov_screen_dist, OFX/OFY = cx/cy << 16). */
static void rtps(int32_t wx, int32_t wy, int32_t wz, int32_t *sx, int32_t *sy, uint32_t *sz3)
{
    const re15_camera_view_t *c = &s_cam;
    int64_t m1 = (((int64_t)c->trans[0] << 12) + (int64_t)c->rot[0] * wx + (int64_t)c->rot[1] * wy + (int64_t)c->rot[2] * wz) >> 12;
    int64_t m2 = (((int64_t)c->trans[1] << 12) + (int64_t)c->rot[3] * wx + (int64_t)c->rot[4] * wy + (int64_t)c->rot[5] * wz) >> 12;
    int64_t m3 = (((int64_t)c->trans[2] << 12) + (int64_t)c->rot[6] * wx + (int64_t)c->rot[7] * wy + (int64_t)c->rot[8] * wz) >> 12;
    int32_t ir1 = clamp32(m1, -0x8000, 0x7FFF), ir2 = clamp32(m2, -0x8000, 0x7FFF);
    *sz3 = (uint32_t)clamp32(m3, 0, 0xFFFF);
    uint32_t n = re15_gte_divide((uint32_t)(uint16_t)c->fov_screen_dist, *sz3);
    *sx = (int32_t)((((int64_t)k_cx << 16) + (int64_t)ir1 * n) >> 16);
    *sy = (int32_t)((((int64_t)k_cy << 16) + (int64_t)ir2 * n) >> 16);
}
/* FUN_80077924/FUN_80077ed0 von Hand fuer EINEN Platz (Rueckgabe = Zahl der Quads, < 0 = Datenfehler). */
static int hand_quads(int platz, hq_t *out, int max)
{
    const uint8_t *pl = re2fx_platz(platz);
    const uint16_t st = le16(pl + 0x18);
    if ((st & 0xA000u) != 0xA000u) return 0;                   /* `andi v1,s2,0xa000` @0x80077a18-24 */
    const unsigned bank = pl[0x1C];
    const uint8_t *e = anim_e(bank, pl[0x21]);                 /* `lbu v0,33(s0)` / `lw v1,112(s0)` @0x80077a04-14 */
    if (!e) return -1;
    const unsigned zelle = e[0], anzahl = e[1], groesse = e[3];
    if (anzahl == 0) return 0;                                 /* `beq s3,zero` @0x80077a40 */
    if (st & 0x200u) return -2;                                /* Alternativpfad @0x80077a54-60 (nicht belegt) */
    const uint8_t code = (st & 0x1000u) ? 0x2E : 0x2C;         /* `addiu s5,zero,44/46` @0x80077a44-50 */
    int32_t sx, sy; uint32_t sz3;
    rtps((int16_t)le16(pl + 0x34), (int16_t)le16(pl + 0x36), (int16_t)le16(pl + 0x38), &sx, &sy, &sz3);
    if ((sz3 >> 9) == 0) return 0;                             /* `sra v0,v1,9 / beq` @0x80077f58-5c */
    const uint32_t sz = sz3 > 32767u ? 32767u : sz3;           /* @0x80077f64-74 */
    const uint32_t t0 = groesse * (uint32_t)le16(pl + 0x3A);   /* `mult a2,t0 / mflo t0` @0x80077f14-18 */
    const int32_t  t1 = (int32_t)(t0 * (uint32_t)k_camf);      /* `mult t0,a0 / mflo t1` @0x80077f24/48 */
    const int32_t  step = t1 / (int32_t)(sz << s_hand_sz_shift);   /* `sll v0,v0,4 / div t1,v0` @0x80077fd4-d8 */
    const uint32_t w = (uint32_t)step * le16(pl + 0x04);       /* `lhu v1,4(a1) / mult / mflo t3` @0x80078004-10 */
    const uint32_t h = (uint32_t)step * le16(pl + 0x06);       /* `lhu v1,6(a1) / mult / mflo a1` @0x80078030-50 */
    const uint32_t tx = w / groesse, ty = h / groesse;         /* `divu t3,a2` / `divu a1,a2` @0x8007801c/5c */
    const unsigned du = (tx > 0x1FFFFu) ? groesse - 1 : groesse;   /* `sltu v0,a0,t7 / addiu a2,a2,-1` @0x80078070-7c */
    const unsigned dv = (ty > 0x1FFFFu) ? groesse - 1 : groesse;   /* `addiu t5,t5,-256` @0x80078088 */
    int n = 0;
    for (unsigned k = 0; k < anzahl && n < max; k++) {         /* Schleife @0x8007808c-150 */
        const uint8_t *uv = uv_e(bank, zelle + k);
        if (!uv) return -1;
        const uint32_t ax = ((uint32_t)(uint16_t)sx << 16) + (uint32_t)((int64_t)(int8_t)uv[2] * (int64_t)(int32_t)tx);  /* @0x80078090-ac */
        const uint32_t ay = ((uint32_t)(uint16_t)sy << 16) + (uint32_t)((int64_t)(int8_t)uv[3] * (int64_t)(int32_t)ty);  /* @0x800780a0-cc */
        hq_t *q = &out[n++];
        q->x0 = (int16_t)(ax >> 16);        q->x1 = (int16_t)((ax + w) >> 16);   /* `srl` @0x800780b4/c0 */
        q->y0 = (int16_t)(ay >> 16);        q->y1 = (int16_t)((ay + h) >> 16);   /* `and 0xffff0000` @0x800780d4/dc */
        q->u0 = uv[0]; q->v0 = uv[1];                                           /* @0x80078104/14 */
        q->u1 = (uint8_t)(uv[0] + du); q->v1 = (uint8_t)(uv[1] + dv);           /* @0x8007810c/20 */
        q->clut = le16(pl + 0x32); q->tpage = le16(pl + 0x2A); q->code = code;  /* `lhu v0,50 / lhu v1,42` @0x80077f38-3c */
        q->platz = platz;
    }
    return n;
}
static int quad_gleich(const re2fx_quad_t *m, const hq_t *h)
{
    return m->platz == h->platz && m->x0 == h->x0 && m->y0 == h->y0 && m->x1 == h->x1 && m->y1 == h->y1 &&
           m->u0 == h->u0 && m->v0 == h->v0 && m->u1 == h->u1 && m->v1 == h->v1 &&
           m->clut == h->clut && m->tpage == h->tpage && m->code == h->code;
}

/* Kind-Codes der Aufschlaege (Bank<<8 | Sub): Op 49 @0x80021780-0x80021924, Op 48 @0x80021094-0x800210c4,
 * Bodenflamme 0x0505 (@0x80021114), Folgeflamme 0x0504 (@0x8001fdf0). */
static const uint16_t k_codes[9] = { 0x030F, 0x040C, 0x041D, 0x031F, 0x0314, 0x040D, 0x0505, 0x0504, 0x020C };
static int code_index(unsigned bank, unsigned sub)
{
    for (int i = 0; i < 9; i++) if (k_codes[i] == ((bank << 8) | sub)) return i;
    return -1;
}

/* Anim-Verfolgung je Platz ueber die Bilder. */
static int      s_prev_da[96], s_prev_code[96], s_prev_anim[96], s_prev_op1[96];
static int      s_leben[96];
static uint64_t s_gesehen[9];        /* je Kind-Code: gezeichnete Anim-Indizes (Bitmenge) */
static int      s_lebensdauer_fehler;

/* Erstes gezeichnetes Quad je Kind-Code (fuer die Literal-Pins 21) + die Eingaben der Handrechnung. */
typedef struct {
    int folge, bild, platz; hq_t q; int da;
    int32_t welt[3]; uint16_t skala, ax, ay; uint8_t anim[4], uv[4];
} pin_ist_t;
static pin_ist_t s_pin_ist[2][9];

static void anim_pruefen(int folge_nr)
{
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *pl = re2fx_platz(i);
        const uint16_t st = le16(pl + 0x18);
        const int da = ((st & 0xA000u) == 0xA000u);
        const int code = (pl[0x1C] << 8) | pl[0x1E];
        const int a = pl[0x21];
        if (da) {
            const int ci = code_index(pl[0x1C], pl[0x1E]);
            if (ci < 0) { FEHLER(27); continue; }
            if (ci == 8) {                                  /* 0x020C = Aufschlag-Platz selbst (Anim 18, Bank 2) */
                s_prev_da[i] = 1; s_prev_code[i] = code; s_prev_anim[i] = a; s_prev_op1[i] = 0;
                continue;
            }
            const uint8_t *ea = anim_e(pl[0x1C], (unsigned)a);
            if (!ea || ea[2] != 1) FEHLER(28);
            if (!s_prev_da[i] || s_prev_code[i] != code) {  /* neuer Platz-Inhalt */
                int lo, hi;
                if (start_menge(pl[0x1C], pl[0x1E], &lo, &hi) != 0) FEHLER(29);
                else if (a < lo || a > hi) { printf("  Platz %d Code %04X: Start-Anim %d nicht in [%d,%d]\n", i, code, a, lo, hi); FEHLER(22); }
                const uint8_t *s0 = schritt0(pl[0x1C], pl[0x1E]);
                s_prev_op1[i] = (s0 && s0[0] == 1);
                s_leben[i] = 0;
            } else {
                const int soll = naechste(pl[0x1C], s_prev_anim[i]);
                if (soll != a) { printf("  Platz %d Code %04X: Anim %d -> %d, Tafel sagt %d\n", i, code, s_prev_anim[i], a, soll); FEHLER(23); }
            }
            s_gesehen[ci] |= (uint64_t)1 << (a & 63);
            s_leben[i]++;
            s_prev_da[i] = 1; s_prev_code[i] = code; s_prev_anim[i] = a;
        } else {
            if (s_prev_da[i] && s_prev_op1[i]) {            /* Aufschlag-Kind verschwunden: nur am ENDE */
                const int nach = naechste((unsigned)(s_prev_code[i] >> 8), s_prev_anim[i]);
                if (nach != -1) { printf("  Platz %d Code %04X verschwindet bei Anim %d (naechste %d)\n", i, s_prev_code[i], s_prev_anim[i], nach); FEHLER(24); }
                /* Lebensdauer = Laenge der Folge ab dem Start */
                int lo, hi, len = 0;
                if (start_menge((unsigned)(s_prev_code[i] >> 8), (unsigned)(s_prev_code[i] & 0xFF), &lo, &hi) == 0)
                    for (int x = lo; x >= 0 && len < 64; x = naechste((unsigned)(s_prev_code[i] >> 8), x)) len++;
                if (len != s_leben[i]) { printf("  Platz %d Code %04X: %d Bilder gezeichnet, Folge hat %d\n", i, s_prev_code[i], s_leben[i], len); s_lebensdauer_fehler++; }
            }
            s_prev_da[i] = 0; s_prev_op1[i] = 0;
        }
    }
    (void)folge_nr;
}

static uint8_t s_platz_da[RE2FX_PLAETZE], s_platz_deckend[RE2FX_PLAETZE];
static int s_leere_zellen;
static int s_hand_geprueft, s_hand_neg_abweichung;
static int folge(const char *name, int folge_nr, int art, int bilder, int *pix_nach)
{
    re2fx_reset(); re15_re2z_rng_reset();
    re2fx_boden_hook = NULL; re2fx_applier = NULL; re2fx_se_hook = NULL;
    memset(s_prev_da, 0, sizeof s_prev_da); memset(s_prev_op1, 0, sizeof s_prev_op1);
    const int32_t q[3] = { 0, 0, 0 };
    static re2fx_quad_t qs[1024];
    static hq_t hs[1024];
    *pix_nach = 0;
    for (int b = 0; b < bilder; b++) {
        if (b == 1) re2fx_aufschlag(art, q, 0);                        /* Aufschlag im Bild 1 = X */
        re2fx_tick();
        int n = re2fx_quads(&s_cam, k_cx, k_cy, k_camf, 0, NULL, NULL, qs, 1024);   /* camf 26684>>7 (ROOM1140) */
        if (b == 0 && n != 0) FEHLER(10);                              /* vor dem Aufschlag: nichts */

        /* N4: Handrechnung fuer alle Plaetze (Schleife 95 -> 0 @0x800779e8) und Vergleich Quad fuer Quad. */
        int nh = 0;
        for (int i = RE2FX_PLAETZE - 1; i >= 0; i--) {
            int k = hand_quads(i, hs + nh, 1024 - nh);
            if (k < 0) { FEHLER(20); break; }
            nh += k;
        }
        if (nh != n) { printf("  %s Bild %d: %d Quads, Handrechnung %d\n", name, b, n, nh); FEHLER(20); }
        for (int k = 0; k < n && k < nh; k++) {
            if (!quad_gleich(&qs[k], &hs[k])) {
                printf("  %s Bild %d Quad %d Platz %d: (%d,%d)-(%d,%d) uv (%u,%u)-(%u,%u), Hand Platz %d (%d,%d)-(%d,%d) uv (%u,%u)-(%u,%u)\n",
                       name, b, k, qs[k].platz, qs[k].x0, qs[k].y0, qs[k].x1, qs[k].y1, qs[k].u0, qs[k].v0, qs[k].u1, qs[k].v1,
                       hs[k].platz, hs[k].x0, hs[k].y0, hs[k].x1, hs[k].y1, hs[k].u0, hs[k].v0, hs[k].u1, hs[k].v1);
                FEHLER(20); break;
            }
            s_hand_geprueft++;
            /* erstes Quad je Kind-Code merken (Literal-Pins) */
            const uint8_t *pl = re2fx_platz(qs[k].platz);
            int ci = code_index(pl[0x1C], pl[0x1E]);
            if (ci >= 0 && !s_pin_ist[folge_nr][ci].da) {
                pin_ist_t *p = &s_pin_ist[folge_nr][ci];
                p->da = 1; p->folge = folge_nr; p->bild = b; p->platz = qs[k].platz; p->q = hs[k];
                p->welt[0] = (int16_t)le16(pl + 0x34); p->welt[1] = (int16_t)le16(pl + 0x36); p->welt[2] = (int16_t)le16(pl + 0x38);
                p->skala = le16(pl + 0x3A); p->ax = le16(pl + 0x04); p->ay = le16(pl + 0x06);
                const uint8_t *e = anim_e(pl[0x1C], pl[0x21]);
                if (e) { memcpy(p->anim, e, 4); const uint8_t *uv = uv_e(pl[0x1C], e[0]); if (uv) memcpy(p->uv, uv, 4); }
            }
        }
        /* Negativ-Kontrolle 26: dieselbe Handrechnung mit SZ<<3 (doppelte Groesse) muss abweichen. */
        if (n > 0) {
            s_hand_sz_shift = 3;
            int nn = 0; static hq_t hn[1024];
            for (int i = RE2FX_PLAETZE - 1; i >= 0; i--) { int k = hand_quads(i, hn + nn, 1024 - nn); if (k > 0) nn += k; }
            s_hand_sz_shift = 4;
            for (int k = 0; k < n && k < nn; k++) if (!quad_gleich(&qs[k], &hn[k])) { s_hand_neg_abweichung++; break; }
        }
        /* N4: Anim-Folge je Platz nach der ESP-Tafel. */
        anim_pruefen(folge_nr);

        memset(s_rgb, 64, sizeof s_rgb);
        memset(s_platz_da, 0, sizeof s_platz_da); memset(s_platz_deckend, 0, sizeof s_platz_deckend);
        s_pixel = 0;
        /* Zeichenfolge: fern zuerst (Port-Sortierung wie pc_draw_effects: Schluessel View-Z). */
        for (int pass = 0; pass < n; pass++) {
            int best = -1;
            for (int i = 0; i < n; i++) if (qs[i].platz >= 0 && (best < 0 || qs[i].vz > qs[best].vz)) best = i;
            if (best < 0) break;
            re2fx_quad_t e = qs[best]; qs[best].platz = -1;
            if ((e.tpage & 0x1F) != 0x1E && (e.tpage & 0x1F) != 0x1F) FEHLER(12);
            if ((e.clut & 0x3F) != 0x11) FEHLER(13);
            if (e.u1 <= e.u0 || e.v1 <= e.v0) FEHLER(14);
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
            if (s_platz_da[i] && !s_platz_deckend[i]) FEHLER(15);
        if (b >= 2) *pix_nach += s_pixel;
        bild_schreiben(name, b);
    }
    return 0;
}

/* 21 LITERAL-PINS: erstes gezeichnetes Quad je Kind-Code bei fester Kamera (Kamera rot [3706 0 -1781 | -421 3972
 * -875 | 1727 967 3593], trans (-904,480,5618), H 208, camf 208, Mitte (160,120)). Die Pins 030F/0505/040D
 * (Trim-Fall) sind von Hand nachgerechnet: tools/re2fx_billboard_hand.py (eigene RTPS mit UNR-Division nach
 * psx-spx, FUN_80077ed0 aus der Disasm), Dossier bau_d.md §N4. Sie fangen Gleichtakt-Aenderungen, die
 * Maschine UND Handrechnung dieser Sonde treffen (Kamera, UNR-Division, camf). */
typedef struct { int folge; uint16_t code; int bild, platz; int16_t x0, y0, x1, y1; uint8_t u0, v0, u1, v1; uint16_t clut, tpage; } pin_soll_t;
static const pin_soll_t k_pins[10] = {
    { 0, 0x030F, 2, 94,  97, 113, 116, 132,   0,  32,  16,  48, 0x7851, 0x003E },   /* Anim 11 (Zelle 22), Skala 0x2000 */
    { 0, 0x040C, 2, 93, 116, 127, 135, 146, 224, 136, 240, 152, 0x7851, 0x005E },   /* Anim 3, subtraktiv (ABR 2) */
    { 0, 0x041D, 2, 92,  91,  96, 114, 119,   0,  72,  16,  88, 0x78D1, 0x007E },   /* Anim 32, Skala 0x1800 */
    { 0, 0x031F, 3, 91, 106, 112, 124, 131,   0,  32,  16,  48, 0x78D1, 0x003E },   /* Phase 1 */
    { 0, 0x0314, 4, 90, 124, 117, 143, 136, 216,  32, 232,  48, 0x7891, 0x003E },   /* Phase 2, Anim 38 */
    { 0, 0x040D, 5, 89,  96,  72, 132, 108,   0,  72,  15,  87, 0x7851, 0x007E },   /* Phase 3, Kanten-Trim u/v */
    { 1, 0x040C, 2, 94, 114, 125, 137, 148, 224, 136, 240, 152, 0x7851, 0x005E },   /* Brand-Kind, Skala 0x2800 */
    { 1, 0x041D, 2, 93,  69,  71, 107, 108,   0,  72,  15,  87, 0x78D1, 0x007E },   /* Brand-Kind, Trim */
    { 1, 0x0505, 2, 92,  94,  86, 157, 149,   0, 168,  40, 208, 0x7911, 0x003E },   /* Bodenflamme, Groesse 40 */
    { 1, 0x0504, 5, 95, 116, 103, 154, 141,  80, 168, 120, 208, 0x7911, 0x003E },   /* Folgeflamme, Skala x0.8 */
};
static int pins_pruefen(void)
{
    for (int i = 0; i < 10; i++) {
        const pin_soll_t *s = &k_pins[i];
        const int ci = code_index(s->code >> 8, s->code & 0xFF);
        if (ci < 0) return 21;
        const pin_ist_t *p = &s_pin_ist[s->folge][ci];
        if (!p->da || p->bild != s->bild || p->platz != s->platz || p->q.x0 != s->x0 || p->q.y0 != s->y0 ||
            p->q.x1 != s->x1 || p->q.y1 != s->y1 || p->q.u0 != s->u0 || p->q.v0 != s->v0 || p->q.u1 != s->u1 ||
            p->q.v1 != s->v1 || p->q.clut != s->clut || p->q.tpage != s->tpage || p->q.code != 0x2E) {
            printf("  Pin %d (%04X) weicht ab\n", i, s->code);
            return 21;
        }
    }
    return 0;
}

/* Vollstaendigkeit 25: jede aus CORE00.ESP abgeleitete Folge eines Aufschlag-Kinds wurde ganz gezeichnet;
 * die Flammen (0x0505) durchlaufen den ganzen Zyklus 0..9 (Eintrag 10 = LOOP -> 0). */
static int vollstaendig(void)
{
    for (int ci = 0; ci < 6; ci++) {
        int lo, hi;
        if (start_menge(k_codes[ci] >> 8, k_codes[ci] & 0xFF, &lo, &hi) != 0) return 29;
        uint64_t soll = 0;
        for (int x = lo; x >= 0; x = naechste(k_codes[ci] >> 8, x)) soll |= (uint64_t)1 << (x & 63);
        if ((s_gesehen[ci] & soll) != soll || (s_gesehen[ci] & ~soll) != 0) {
            printf("  Code %04X: gezeichnet %016llX, Folge %016llX\n", k_codes[ci],
                   (unsigned long long)s_gesehen[ci], (unsigned long long)soll);
            return 25;
        }
    }
    if ((s_gesehen[6] & 0x3FFull) != 0x3FFull || (s_gesehen[6] & ~0x3FFull) != 0) return 25;
    if (s_lebensdauer_fehler) return 25;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1) s_dir = argv[1];
    long ne = 0, nt = 0;
    uint8_t *esp = lesen("CORE00.ESP", &ne), *tim = lesen("TEX.TIM", &nt);
    if (!esp || !tim || re2fx_register_core(esp, (size_t)ne) != 0 || tim_ins_vram(tim, nt) != 0) { printf("FAIL 1: Dateien\n"); return 1; }
    s_esp = esp; s_esp_n = ne; s_tim = tim; s_tim_n = nt;
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
    folge("saeure", 0, 2, 16, &ps);
    folge("brand", 1, 1, 48, &pb);
    fclose(s_qf); fclose(s_cf);
    printf("Pixel nach dem Aufschlag: Saeure %d, Brand %d; eindeutige Ausschnitte %d, leere Zellen %d\n",
           ps, pb, s_ncrops, s_leere_zellen);
    printf("Handrechnung: %d Quads identisch; Negativ-Kontrolle (SZ<<3) wich in %d Bildern ab\n",
           s_hand_geprueft, s_hand_neg_abweichung);
    printf("Texel (VRAM-Modell gegen TIM-Datei): %d geprueft, %d abweichend\n", s_texel_n, s_texel_abw);
    /* Negativ-Kontrolle 30: ein falsches CLUT-Wort (Zeile + 1) liefert andere Texel. */
    {
        int anders = 0;
        for (int v = 168; v < 208 && !anders; v++) for (int u = 0; u < 40; u++)
            if (tim_texel(0x003E, 0x7911, u, v) != tim_texel(0x003E, 0x7951, u, v)) { anders = 1; break; }
        if (!anders) { printf("FAIL 31: Negativ-Kontrolle Texel ohne Wirkung\n"); return 31; }
    }
    if (s_texel_n < 10000 || s_texel_abw != 0) { printf("FAIL 30: Texel VRAM-Modell != TIM-Datei\n"); return 30; }
    printf("  Kamera rot [%d %d %d | %d %d %d | %d %d %d] trans (%d,%d,%d) H %d\n", s_cam.rot[0], s_cam.rot[1], s_cam.rot[2],
           s_cam.rot[3], s_cam.rot[4], s_cam.rot[5], s_cam.rot[6], s_cam.rot[7], s_cam.rot[8],
           s_cam.trans[0], s_cam.trans[1], s_cam.trans[2], s_cam.fov_screen_dist);
    for (int f = 0; f < 2; f++)
        for (int ci = 0; ci < 9; ci++) {
            const pin_ist_t *pi = &s_pin_ist[f][ci];
            if (!pi->da) continue;
            printf("  PIN %d %04X Bild %2d Platz %2d: (%d,%d)-(%d,%d) uv (%u,%u)-(%u,%u) CLUT %04X TPage %04X Code %02X"
                   " | Welt (%d,%d,%d) Skala %u Aspekt %u/%u Anim (%u,%u,%u,%u) UV (%u,%u,%d,%d)\n",
                   f, k_codes[ci], pi->bild, pi->platz, pi->q.x0, pi->q.y0, pi->q.x1, pi->q.y1,
                   pi->q.u0, pi->q.v0, pi->q.u1, pi->q.v1, pi->q.clut, pi->q.tpage, pi->q.code,
                   pi->welt[0], pi->welt[1], pi->welt[2], pi->skala, pi->ax, pi->ay,
                   pi->anim[0], pi->anim[1], pi->anim[2], pi->anim[3], pi->uv[0], pi->uv[1], (int8_t)pi->uv[2], (int8_t)pi->uv[3]);
        }
    if (s_fehler) { printf("FAIL %d\n", s_fehler); return s_fehler; }
    if (ps <= 0) { printf("FAIL 16: Saeure zeichnet nichts\n"); return 16; }
    if (pb <= 0) { printf("FAIL 17: Brand zeichnet nichts\n"); return 17; }
    if (s_hand_geprueft < 500) { printf("FAIL 20: zu wenige Quads verglichen\n"); return 20; }
    if (s_hand_neg_abweichung == 0) { printf("FAIL 26: Negativ-Kontrolle der Handrechnung ohne Wirkung\n"); return 26; }
    int rc = pins_pruefen();
    if (rc) { printf("FAIL %d: Literal-Pin (Handrechnung tools/re2fx_billboard_hand.py)\n", rc); return rc; }
    rc = vollstaendig();
    if (rc) { printf("FAIL %d: Anim-Folge nicht vollstaendig gezeichnet\n", rc); return rc; }
    printf("probe_r34_re2fx_bild: gruen (Ausgabe %s)\n", s_dir);
    return 0;
}
