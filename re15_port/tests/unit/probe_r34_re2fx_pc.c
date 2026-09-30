/*
 * probe_r34_re2fx_pc — Runde 34 Spur D, Nachbesserung N5 (Gegenpruefung M7/M8): der PC-Zeichner
 * platform/pc/src/re2fx_pc.c ohne SDL.
 *
 * re2fx_pc.c wird in diese Sonde mituebersetzt; die sechs Zeichen-APIs, die er aus render_pc.c benutzt
 * (upload_tim_slot, dbg_slot_loaded, bind_tim_slot, set_tri_alpha, set_tri_blend, render_textured_tri), sind
 * hier ATTRAPPEN, die mitschreiben. Die Textur-Abbildung des PC-Slots wird wie render_pc.c nachgebildet
 * (re15_render_pc_upload_tim_slot: je CLUT-Zeile eine Kopie der Seite, Zeile = CLUT-y - clut_y, 4 bpp mit
 * Nibble (i & 1) ? hi : lo; re15_render_textured_tri: clut_idx = ((clut >> 6) & 0x1FF) - clut_base_y) und gegen
 * das PSX-VRAM-MODELL geprueft (Lader FUN_80076a40: Bild nach (768,256) @0x80076a64-a8, CLUT nach (x, 480)
 * @0x80076b00-0c; 4-bpp-Abruf aus TPage/CLUT-Wort).
 *
 * Pruefungen (Rueckgabe 0 = gruen, sonst die Nummer):
 *   1  re2fx_pc_lade_tex laedt; Slot RE2FX_TIM_SLOT (52, Integration W1); Masse 512 x 256, 5 CLUT-Zeilen ab 480 -> Texturhoehe 1280 <= 4096 (M8)
 *   2  vor dem Aufschlag zeichnet re2fx_pc_draw nichts (Negativ-Kontrolle)
 *   3  je Bild: Zahl der Dreieckspaare == re2fx_quads (Saeure/Brand lassen nichts aus), Rueckwaerts-Reihenfolge
 *      (FUN_80077ed0 haengt vorn an den OT-Bucket @0x80077f94-fac)
 *   4  Dreiecke = Rechteck des Quads, u + 256 fuer Seite 0x1F, v unveraendert, TPage/CLUT/z durchgereicht,
 *      Farbe 255 (Paketfarbe 0x808080 @0x800783cc-d0), Slot RE2FX_TIM_SLOT gebunden
 *   5  JEDES Texel jedes Quads: PC-Slot-Texel == VRAM-Modell-Texel (roher 16-Bit-CLUT-Wert)
 *   6  Mischmodus je ABR wie pc_draw_effects (main.c:436-448): 0 -> alpha 128, 1 -> blend 1, 2 -> blend 2,
 *      3 -> blend 1 alpha 64; danach blend 0 / alpha 255
 *   7  Negativ-Kontrolle: CLUT-Zeile 485 (ausserhalb 480..484) wird ausgelassen statt mit Zeile 480 gezeichnet
 *   8  Negativ-Kontrolle Texel: ohne u + 256 fuer Seite 0x1F weichen Texel ab
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_fx.h"
#include "re2fx_pc.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_ai_flavor.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fail(int n, const char *w) { printf("FAIL %d: %s\n", n, w); return n; }
static uint16_t le16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }
static uint32_t le32(const uint8_t *b) { return (uint32_t)le16(b) | ((uint32_t)le16(b + 2) << 16); }

/* ---- Attrappen der render_pc.c-API ------------------------------------------------------------- */
static struct {
    int      geladen, slot, breite, hoehe, clut_y, n_cluts;
    uint16_t clut[32 * 16];
    uint8_t  pix[512 * 256 / 2];
} s_slot;
static int s_gebunden = -1, s_blend = 0, s_alpha = 255;
typedef struct { int x[3], y[3], u[3], v[3], tpage, clut, z, r, g, b, blend, alpha, slot; } tri_t;
static tri_t s_tri[4096]; static int s_ntri;

void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot)
{
    /* wie render_pc.c: n_cluts = clut_entries / 16 (4 bpp), Hoehe = height * n_cluts, clut_base_y = clut_y */
    if (!tim || tim->bpp != 4 || !tim->has_clut) return;
    s_slot.slot = slot; s_slot.breite = tim->width; s_slot.clut_y = tim->clut_y;
    s_slot.n_cluts = tim->clut_entries / 16; s_slot.hoehe = tim->height * s_slot.n_cluts;
    if (s_slot.n_cluts > 32 || tim->width * tim->height / 2 > (int)sizeof s_slot.pix) return;
    memcpy(s_slot.clut, tim->clut, (size_t)tim->clut_entries * 2);
    memcpy(s_slot.pix, tim->pixels, (size_t)tim->width * tim->height / 2);
    s_slot.geladen = 1;
}
int  re15_render_pc_dbg_slot_loaded(int slot) { return s_slot.geladen && slot == s_slot.slot; }
void re15_render_pc_bind_tim_slot(int slot) { s_gebunden = slot; }
void re15_render_pc_set_tri_alpha(int a) { s_alpha = a; }
void re15_render_pc_set_tri_blend(int m) { s_blend = m; }
void re15_render_textured_tri(int x0, int y0, int u0, int v0, int x1, int y1, int u1, int v1,
                              int x2, int y2, int u2, int v2, int tpage, int clut, int z,
                              uint8_t r, uint8_t g, uint8_t b)
{
    if (s_ntri >= 4096) return;
    tri_t *t = &s_tri[s_ntri++];
    t->x[0] = x0; t->y[0] = y0; t->u[0] = u0; t->v[0] = v0;
    t->x[1] = x1; t->y[1] = y1; t->u[1] = u1; t->v[1] = v1;
    t->x[2] = x2; t->y[2] = y2; t->u[2] = u2; t->v[2] = v2;
    t->tpage = tpage; t->clut = clut; t->z = z; t->r = r; t->g = g; t->b = b;
    t->blend = s_blend; t->alpha = s_alpha; t->slot = s_gebunden;
}
/* Texel des PC-Slots wie render_pc.c: Zeile clut_idx (ausserhalb -> 0), Pixel i = v*breite + u, Nibble. */
static uint16_t pc_texel(int u, int v, int clut)
{
    int idx = ((clut >> 6) & 0x1FF) - s_slot.clut_y;
    if (idx < 0 || idx >= s_slot.n_cluts) idx = 0;
    const int i = v * s_slot.breite + u;
    const int nib = (i & 1) ? (s_slot.pix[i / 2] >> 4) : (s_slot.pix[i / 2] & 0xF);
    return s_slot.clut[idx * 16 + nib];
}

/* ---- VRAM-Modell (unabhaengig von re2fx_pc.c) ---------------------------------------------------- */
static uint16_t s_vram[512][1024];
static int tim_ins_vram(const uint8_t *t, long n)
{
    if (n < 20 || le32(t) != 0x10u) return -1;
    uint32_t csz = le32(t + 8);
    int cx = le16(t + 12), cy = 480, cw = le16(t + 16), ch = le16(t + 18);   /* CLUT-y := 480 @0x80076b00-0c */
    for (int y = 0; y < ch; y++) for (int x = 0; x < cw; x++) s_vram[cy + y][cx + x] = le16(t + 20 + (size_t)(y * cw + x) * 2);
    const uint8_t *im = t + 8 + csz;
    int ix = 28 * 64 - 1024, iy = 256, iw = le16(im + 8), ih = le16(im + 10);                /* (768,256) @0x80076a64-a8 */
    if ((long)(im - t) + 12 + (long)iw * ih * 2 > n) return -2;
    for (int y = 0; y < ih; y++) for (int x = 0; x < iw; x++) s_vram[iy + y][ix + x] = le16(im + 12 + (size_t)(y * iw + x) * 2);
    return 0;
}
static uint16_t vram_texel(uint16_t tpage, uint16_t clut, int u, int v)
{
    int bx = (tpage & 0xF) * 64, by = ((tpage >> 4) & 1) * 256;
    uint16_t hw = s_vram[(by + v) & 511][(bx + (u >> 2)) & 1023];
    int idx = (hw >> ((u & 3) * 4)) & 0xF;
    return s_vram[(clut >> 6) & 0x1FF][((clut & 0x3F) * 16 + idx) & 1023];
}

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

static re15_camera_view_t s_cam;
static long s_texel_n, s_texel_neg_abw;
static int s_abr_gesehen[4];

/* Ein Bild zeichnen und gegen re2fx_quads pruefen. */
static int bild_pruefen(int *paare)
{
    static re2fx_quad_t q[512];
    const int n = re2fx_quads(&s_cam, 160, 120, 208, 0, NULL, NULL, q, 512);
    s_ntri = 0;
    re2fx_pc_set_ansicht(&s_cam, 160, 120, 208, 0, NULL, NULL);
    re2fx_pc_draw();
    *paare = s_ntri / 2;
    if ((s_ntri & 1) || s_ntri / 2 != n || re2fx_pc_letzte_quads() != n) {
        printf("  %d Dreiecke, %d Quads, letzte_quads %d\n", s_ntri, n, re2fx_pc_letzte_quads());
        return 3;
    }
    for (int k = 0; k < n; k++) {
        const re2fx_quad_t *e = &q[n - 1 - k];                         /* rueckwaerts eingereiht */
        const tri_t *a = &s_tri[2 * k], *b = &s_tri[2 * k + 1];
        const int uo = ((e->tpage & 0x1F) == 0x1F) ? 256 : 0;
        const int u0 = e->u0 + uo, u1 = e->u1 + uo;
        /* Dreieck A: (x0,y0) (x1,y0) (x0,y1); B: (x1,y0) (x1,y1) (x0,y1) */
        if (a->x[0] != e->x0 || a->y[0] != e->y0 || a->x[1] != e->x1 || a->y[1] != e->y0 || a->x[2] != e->x0 || a->y[2] != e->y1 ||
            b->x[0] != e->x1 || b->y[0] != e->y0 || b->x[1] != e->x1 || b->y[1] != e->y1 || b->x[2] != e->x0 || b->y[2] != e->y1)
            return 4;
        if (a->u[0] != u0 || a->v[0] != e->v0 || a->u[1] != u1 || a->v[1] != e->v0 || a->u[2] != u0 || a->v[2] != e->v1 ||
            b->u[0] != u1 || b->v[0] != e->v0 || b->u[1] != u1 || b->v[1] != e->v1 || b->u[2] != u0 || b->v[2] != e->v1)
            return 4;
        if (a->tpage != e->tpage || a->clut != e->clut || a->z != e->vz || b->tpage != e->tpage || b->clut != e->clut)
            return 4;
        if (a->r != 255 || a->g != 255 || a->b != 255 || a->slot != RE2FX_TIM_SLOT) return 4;
        /* 6: Mischmodus je ABR (Code 0x2E = halbtransparent @0x80077a44-50, ABR = TPage-Bits 5-6). */
        const int abr = (e->tpage >> 5) & 3;
        int sb = 0, sa = 255;
        if (e->code & 2) {
            if (abr == 1) { sb = 1; sa = 255; } else if (abr == 2) { sb = 2; sa = 255; }
            else if (abr == 3) { sb = 1; sa = 64; } else { sb = 0; sa = 128; }
        }
        if (a->blend != sb || a->alpha != sa || b->blend != sb || b->alpha != sa) return 6;
        s_abr_gesehen[abr] = 1;
        /* 5: jedes Texel des Quads */
        for (int v = e->v0; v < e->v1; v++)
            for (int u = e->u0; u < e->u1; u++) {
                const uint16_t soll = vram_texel(e->tpage, e->clut, u, v);
                if (pc_texel(u + uo, v, e->clut) != soll) {
                    printf("  Texel (%d,%d) TPage %04X CLUT %04X: PC %04X, VRAM %04X\n", u, v, e->tpage, e->clut,
                           pc_texel(u + uo, v, e->clut), soll);
                    return 5;
                }
                s_texel_n++;
                if (uo && pc_texel(u, v, e->clut) != soll) s_texel_neg_abw++;   /* 8: ohne +256 */
            }
    }
    if (s_blend != 0 || s_alpha != 255) return 6;                      /* Zustand zurueckgesetzt */
    return 0;
}

static int folge(int art, int bilder)
{
    re2fx_reset(); re15_re2z_rng_reset();
    re2fx_boden_hook = NULL; re2fx_applier = NULL; re2fx_se_hook = NULL;
    const int32_t q[3] = { 0, 0, 0 };
    int summe = 0;
    for (int b = 0; b < bilder; b++) {
        if (b == 1) re2fx_aufschlag(art, q, 0);
        re2fx_tick();
        int paare = 0, rc = bild_pruefen(&paare);
        if (rc) { printf("  Art %d Bild %d\n", art, b); return rc; }
        if (b == 0 && paare != 0) return 2;
        summe += paare;
    }
    printf("  Art %d: %d Quads gezeichnet\n", art, summe);
    return summe > 0 ? 0 : 3;
}

int main(void)
{
    long ne = 0, nt = 0;
    uint8_t *esp = lesen("CORE00.ESP", &ne), *tim = lesen("TEX.TIM", &nt);
    if (!esp || !tim || re2fx_register_core(esp, (size_t)ne) != 0 || tim_ins_vram(tim, nt) != 0) return fail(1, "Dateien");
    if (re2fx_pc_lade_tex(tim, (size_t)nt) != 0) return fail(1, "re2fx_pc_lade_tex");
    printf("Slot %d: %d x %d, %d CLUT-Zeilen ab %d\n", s_slot.slot, s_slot.breite, s_slot.hoehe, s_slot.n_cluts, s_slot.clut_y);
    if (s_slot.slot != RE2FX_TIM_SLOT || s_slot.breite != 512 || s_slot.hoehe != 1280 || s_slot.n_cluts != 5 || s_slot.clut_y != 480)
        return fail(1, "Slot-Masse nicht 512 x 1280 / CLUT 480..484");
    if (s_slot.hoehe > 4096) return fail(1, "Textur ueber der 4096er Grenze (M8)");

    re15_camera_cut_t cut; memset(&cut, 0, sizeof cut);
    cut.fov = 26684; cut.pos_x = -1500; cut.pos_y = -1800; cut.pos_z = -5200;
    cut.target_x = 1000; cut.target_y = -400; cut.target_z = 0;
    if (re15_camera_build_view(&cut, &s_cam) != 0) return fail(1, "Kamera");

    int rc;
    if ((rc = folge(2, 16))) return fail(rc, "Saeure-Folge");
    if ((rc = folge(1, 48))) return fail(rc, "Brand-Folge");
    printf("  Texel geprueft %ld; ABR gesehen %d/%d/%d/%d\n", s_texel_n, s_abr_gesehen[0], s_abr_gesehen[1], s_abr_gesehen[2], s_abr_gesehen[3]);
    if (!s_abr_gesehen[1] || !s_abr_gesehen[2] || !s_abr_gesehen[3]) return fail(6, "nicht alle Mischmodi der Aufschlaege durchlaufen");
    /* Seite 0x1F: die Aufschlag-Baenke 3/4/5 liegen alle auf Seite 0x1E (Bankkopf-TPage 0x001E, step[0x14] setzt
     * nur ABR-Bits) - der Zweig u + 256 ist mit echten Daten unerreichbar. Synthetisch: alle lebenden Plaetze auf
     * Seite 0x1F umstellen (ABR bleibt) und dieselbe Texel-Pruefung fahren (VRAM-Modell liest dann x 960). */
    re2fx_reset(); re15_re2z_rng_reset();
    { const int32_t q0[3] = { 0, 0, 0 }; re2fx_aufschlag(2, q0, 0); }
    re2fx_tick(); re2fx_tick();
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        uint8_t *w = re2fx_platz_sonde(i);
        if (le16(w + 0x18) == 0) continue;
        const uint16_t tp = (uint16_t)((le16(w + 0x2A) & ~0x1Fu) | 0x1Fu);
        w[0x2A] = (uint8_t)tp; w[0x2B] = (uint8_t)(tp >> 8);
    }
    { int paare = 0; if ((rc = bild_pruefen(&paare)) != 0 || paare == 0) return fail(rc ? rc : 5, "Seite 0x1F (u + 256)"); }
    printf("  Seite 0x1F: Texel ohne +256 abweichend: %ld\n", s_texel_neg_abw);
    if (s_texel_neg_abw == 0) return fail(8, "Negativ-Kontrolle Texel (ohne +256) ohne Wirkung");

    /* 7: Platz mit CLUT-Zeile 485 (0x7951) wird ausgelassen. */
    re2fx_reset(); re15_re2z_rng_reset();
    { const int32_t q[3] = { 0, 0, 0 }; re2fx_aufschlag(2, q, 0); }
    re2fx_tick(); re2fx_tick();
    static re2fx_quad_t qq[512];
    int n = re2fx_quads(&s_cam, 160, 120, 208, 0, NULL, NULL, qq, 512);
    if (n <= 0) return fail(7, "keine Quads fuer die Negativ-Kontrolle");
    uint8_t *w = re2fx_platz_sonde(qq[0].platz);
    int dieser = 0;                                              /* Quads dieses Platzes */
    for (int k = 0; k < n; k++) if (qq[k].platz == qq[0].platz) dieser++;
    w[0x32] = 0x51; w[0x33] = 0x79;                              /* CLUT 0x7951 = Zeile 485 */
    s_ntri = 0;
    re2fx_pc_set_ansicht(&s_cam, 160, 120, 208, 0, NULL, NULL);
    re2fx_pc_draw();
    if (s_ntri / 2 != n - dieser) { printf("  %d Paare, erwartet %d\n", s_ntri / 2, n - dieser); return fail(7, "CLUT-Zeile 485 nicht ausgelassen"); }
    printf("probe_r34_re2fx_pc: alle Pruefungen gruen\n");
    return 0;
}
