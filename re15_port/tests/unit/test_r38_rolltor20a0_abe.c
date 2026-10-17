/* test_r38_rolltor20a0_abe.c — Runde 38 Spur W, Nachbesserung 1 (Abnahme-Mangel M1): halbtransparente Raum-Objekte.
 *
 * ANLASS (Abnahme 0, analysis/befunde_runde38/W_abnahme_0.md M1): der Original-Polygon-Weg der Spur zeichnet in
 * ROOM5060/5061 Cut 4 ein Halbviereck der Glasscheibe obj 4 (Attribut 0x0012), DECKEND dunkel — das Original mischt
 * es zur Haelfte mit dem Hintergrund. Der PC-Prop-Weg kannte kein ABE (vorbestehend, 74 Saetze in 152 RDTs).
 *
 * Code: engine/src/objabe_r38.c, include/re15_objabe_r38.h (Regeln + Adressen), platform/pc/src/objabe_r38_pc.c
 * (zwei Durchgaenge mit STP-Teil-Texturen), Haken in platform/pc/main.c (Prop-Schleife) und render_pc.c (Flush).
 *
 * A1  REGELN gegen den Disassembler: ABE = Attribut & 2 (@0x8002c2b4/@0x8002c2b8); ABR = Texpage-Bits 5-6 der
 *     MD1-Seite ODER (Attribut & 0xC) << 3 (FUN_80025940 @0x800259c4/@0x800259cc/@0x80025a14, FUN_80025a98
 *     @0x80025b24/@0x80025b2c/@0x80025b70); Flach = Attribut & 0x10 (@0x8002c2f4) oder Typ 3 (FUN_8003e64c).
 * A2  ORIGINAL-LAUFZEIT (DuckStation-GDB, Auslieferungs-EXE 0800e003, W_belege/orig_abe_*_log.txt): ROOM5060 obj 2-4
 *     Code 0x3e Texpage 0x0086 (ABR 0), obj 5 Code 0x3e Texpage 0x00a6 (ABR 1); ROOM20A0 obj 4-6 Code 0x3e Texpage
 *     0x00c7 (ABR 2); ROOM4090 obj 0-2 Code 0x3e/0x36 Texpage 0x00e5 (ABR 3). Aus den echten RDTs (Satz-Attribut +
 *     JEDE MD1-Seite der Objekt-Polygone) liefern die Regeln genau diese ABE/ABR.
 * A3  STP-TEILUNG echter Texturen (ROOM11B0 obj 0 = einziges ABE-Objekt mit STP-0- UND STP-1-Texeln, ROOM5060 obj 4):
 *     jeder Texel liegt in genau einem Teil (deckend = STP 0, halb = STP 1, 0000h in keinem), beide zusammen = die
 *     normale Slot-Textur (re15_tim_texel_argb).
 * A4  MISCHUNG: die SDL-Umsetzung der zwei Durchgaenge (ABR 0 BLEND Alpha 128, ABR 1 ADD, ABR 2 B-F, ABR 3 ADD Alpha 64)
 *     weicht fuer alle B, F in 0..255 hoechstens 1 Stufe von der GPU-Formel (psx-spx) ab.
 *
 * Werkzeug-Modus fuer integration_r38_rolltor20a0_abe:
 *   test_r38_rolltor20a0_abe --fall <5060c4|20a0c2|4090c0> <framedump.ppm>
 *   -> Mittelwert RGB der Messflaeche gegen das ORIGINAL-Bild desselben Kamera-Schnitts (W_belege/orig_abe_*.png,
 *      DuckStation-Bildpuffer); exit 0 = innerhalb der Toleranz.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_objabe_r38.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (size_t)sz;
    return b;
}

/* ---- A1 ------------------------------------------------------------------------------------------------------ */
typedef struct { uint16_t attr; int typ; uint16_t page; int abe, abr, flach; const char *was; } regel_t;
static const regel_t k_regel[] = {
    { 0x0012, 0, 0x0080, 1, 0, 1, "Glas 5060/1040 (flach, ABR 0)" },
    { 0x0006, 0, 0x0080, 1, 1, 0, "5060 obj 5 (Licht, ABR 1 aus Attribut-Bit 2)" },
    { 0x001a, 3, 0x0080, 1, 2, 1, "Wasser Stage 2 Typ 3 (flach, ABR 2 aus Attribut-Bit 3)" },
    { 0x000e, 0, 0x0080, 1, 3, 0, "Nebel 4090 (Licht, ABR 3)" },
    { 0x0010, 0, 0x0080, 0, 0, 1, "deckend flach (Rolladen 20A0 obj 1)" },
    { 0x0000, 0, 0x0080, 0, 0, 0, "deckend Licht (Rolltor 1050 obj 0)" },
    { 0x0000, 3, 0x0080, 0, 0, 1, "Typ 3 ohne Flach-Bit -> trotzdem flach (FUN_8003e64c)" },
    { 0x0002, 0, 0x00a0, 1, 1, 0, "ABR aus der MD1-Seite (Bit 5)" },
    { 0x0006, 0, 0x00c0, 1, 3, 0, "Seite ABR 2 ODER Attribut ABR 1 = 3" },
};

/* ---- A2: gemessen am Original (W_belege/orig_abe_5060_log.txt, orig_abe_20a0_log.txt, orig_abe_4090_log.txt) -- */
typedef struct { const char *datei; int obj; unsigned satz; int abr; } orig_t;
static const orig_t k_orig[] = {
    { "STAGE5/ROOM5060.RDT", 2, 0x02D2E, 0 },   /* Pool 2 Code 0x3e Texpage 0x0086 */
    { "STAGE5/ROOM5060.RDT", 3, 0x02D50, 0 },   /* Pool 3 Code 0x3e Texpage 0x0086 */
    { "STAGE5/ROOM5060.RDT", 4, 0x02D72, 0 },   /* Pool 4 Code 0x3e Texpage 0x0086 (der Abnahme-Fall) */
    { "STAGE5/ROOM5060.RDT", 5, 0x02D94, 1 },   /* Pool 5 Code 0x3e Texpage 0x00a6 */
    { "STAGE2/ROOM20A0.RDT", 4, 0x01998, 2 },   /* Pool 4 Typ 3 Code 0x3e Texpage 0x00c7 */
    { "STAGE2/ROOM20A0.RDT", 5, 0x019BA, 2 },   /* Pool 5 Typ 3 Code 0x3e Texpage 0x00c7 */
    { "STAGE2/ROOM20A0.RDT", 6, 0x019DC, 2 },   /* Pool 6 Typ 3 Code 0x3e Texpage 0x00c7 */
    { "STAGE4/ROOM4090.RDT", 0, 0x00AF2, 3 },   /* Pool 0 Code 0x3e/0x36 Texpage 0x00e5 */
    { "STAGE4/ROOM4090.RDT", 1, 0x00B14, 3 },   /* Pool 1 Code 0x3e/0x36 Texpage 0x00e5 */
    { "STAGE4/ROOM4090.RDT", 2, 0x00B36, 3 },   /* Pool 2 Code 0x3e/0x36 Texpage 0x00e5 */
};

static void pruefe_original(void)
{
    for (size_t i = 0; i < sizeof k_orig / sizeof k_orig[0]; i++) {
        const orig_t *o = &k_orig[i];
        char path[512];
        snprintf(path, sizeof path, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), o->datei);
        size_t sz = 0;
        uint8_t *d = read_file(path, &sz);
        CHECK(d != NULL, "A2 %s nicht lesbar", path);
        if (!d) continue;
        re15_rdt_t rdt;
        re15_md1_t md1;
        CHECK(o->satz + 34 <= sz && d[o->satz] == 0x2D && d[o->satz + 1] == o->obj,
              "A2 %s @0x%05X ist kein Obj_model_set obj %d", o->datei, o->satz, o->obj);
        const uint16_t attr = (uint16_t)(d[o->satz + 8] | (d[o->satz + 9] << 8));
        int n_poly = 0, n_ok = 0;
        if (re15_rdt_parse(d, sz, &rdt) == 0 && o->obj < rdt.prop_count && rdt.prop_md1[o->obj] &&
            re15_md1_parse(rdt.prop_md1[o->obj], rdt.prop_md1_size[o->obj], &md1) == 0) {
            for (int m = 0; m < md1.mesh_count; m++) {
                const re15_md1_mesh_t *hm = &md1.meshes[m];
                for (int t = 0; t < hm->triangle_count; t++, n_poly++)
                    n_ok += re15_objabe_r38_abr(attr, hm->triangle_uvs[t].page) == o->abr;
                for (int q = 0; q < hm->quad_count; q++, n_poly++)
                    n_ok += re15_objabe_r38_abr(attr, hm->quad_uvs[q].page) == o->abr;
            }
        }
        CHECK(re15_objabe_r38_abe(attr) == 1, "A2 %s obj %d Attribut 0x%04x: ABE nicht gesetzt (Original Code 0x3e)",
              o->datei, o->obj, attr);
        CHECK(n_poly > 0 && n_ok == n_poly, "A2 %s obj %d: ABR %d nur bei %d von %d Polygonen", o->datei, o->obj,
              o->abr, n_ok, n_poly);
        printf("A2 %s obj %d @0x%05X Attribut 0x%04x: ABE 1, ABR %d bei %d/%d Polygonen = Original\n", o->datei,
               o->obj, o->satz, attr, o->abr, n_ok, n_poly);
        free(d);
    }
}

/* ---- A3 ------------------------------------------------------------------------------------------------------ */
static void pruefe_teilung(const char *datei, int obj, int gemischt)
{
    char path[512];
    snprintf(path, sizeof path, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), datei);
    size_t sz = 0;
    uint8_t *d = read_file(path, &sz);
    CHECK(d != NULL, "A3 %s nicht lesbar", path);
    if (!d) return;
    re15_rdt_t rdt;
    re15_tim_t tim;
    if (re15_rdt_parse(d, sz, &rdt) != 0 || obj >= rdt.prop_count || !rdt.prop_tim[obj] ||
        re15_tim_parse(rdt.prop_tim[obj], rdt.prop_tim_size[obj], &tim) != 0) {
        CHECK(0, "A3 %s obj %d: TIM nicht lesbar", datei, obj);
        free(d);
        return;
    }
    int w = 0, h = 0;
    const int n_cluts = re15_objabe_r38_textur_mass(&tim, &w, &h);
    uint32_t *dk = (uint32_t *)calloc((size_t)w * h, 4), *hb = (uint32_t *)calloc((size_t)w * h, 4);
    const long n_halb = re15_objabe_r38_teilen(&tim, dk, hb);
    long n_deck = 0, n_null = 0, n_falsch = 0;
    const int pro = tim.width * tim.height;
    for (int c = 0; c < n_cluts; c++)
        for (int i = 0; i < pro; i++) {
            uint16_t raw;
            if (tim.bpp == 8)      raw = tim.clut[c * 256 + ((const uint8_t *)tim.pixels)[i]];
            else if (tim.bpp == 4) raw = tim.clut[c * 16 + ((i & 1) ? (((const uint8_t *)tim.pixels)[i / 2] >> 4)
                                                                     : (((const uint8_t *)tim.pixels)[i / 2] & 0xF))];
            else                   raw = tim.pixels[i];
            const uint32_t a = dk[c * pro + i], b = hb[c * pro + i];
            const int k = re15_objabe_r38_texel_klasse(raw);
            if (k == 0) { n_null++; n_falsch += (a != 0 || b != 0); }
            if (k == 1) { n_deck++; n_falsch += (a != re15_tim_texel_argb(raw) || b != 0); }
            if (k == 2) { n_falsch += (b != re15_tim_texel_argb(raw) || a != 0); }
        }
    CHECK(n_falsch == 0, "A3 %s obj %d: %ld Texel falsch geteilt", datei, obj, n_falsch);
    CHECK(n_halb > 0, "A3 %s obj %d: keine STP-1-Texel", datei, obj);
    if (gemischt) CHECK(n_deck > 0 && n_halb > 0, "A3 %s obj %d: nicht gemischt (STP0 %ld, STP1 %ld)", datei, obj,
                        n_deck, n_halb);
    printf("A3 %s obj %d: %dx%d (%d CLUT-Zeilen) -> STP 1 (halb) %ld, STP 0 (deckend) %ld, 0000h %ld, falsch %ld\n",
           datei, obj, w, h, n_cluts, n_halb, n_deck, n_null, n_falsch);
    free(dk); free(hb); free(d);
}

/* ---- A4 ------------------------------------------------------------------------------------------------------ */
static int sdl_blend(int b, int f, int a)        /* SDL_BLENDMODE_BLEND: dst = src*A + dst*(1-A), A = a/255 */
{
    return (int)lround((f * a + b * (255 - a)) / 255.0);
}
static int sdl_add(int b, int f, int a)          /* SDL_BLENDMODE_ADD: dst = src*A + dst, geklemmt */
{
    const int v = b + (int)lround(f * a / 255.0);
    return v > 255 ? 255 : v;
}
static void pruefe_mischung(void)
{
    int dmax[4] = { 0, 0, 0, 0 };
    for (int b = 0; b < 256; b++)
        for (int f = 0; f < 256; f++) {
            const int pc[4] = { sdl_blend(b, f, 128), sdl_add(b, f, 255), (b - f) < 0 ? 0 : (b - f), sdl_add(b, f, 64) };
            for (int abr = 0; abr < 4; abr++) {
                const int dd = abs(pc[abr] - (int)re15_objabe_r38_mische(abr, (uint8_t)b, (uint8_t)f));
                if (dd > dmax[abr]) dmax[abr] = dd;
            }
        }
    for (int abr = 0; abr < 4; abr++)
        CHECK(dmax[abr] <= 1, "A4 ABR %d: SDL weicht bis %d Stufen von der GPU-Formel ab", abr, dmax[abr]);
    printf("A4 Mischung SDL gegen psx-spx: max. Abweichung ABR0 %d, ABR1 %d, ABR2 %d, ABR3 %d Stufen\n",
           dmax[0], dmax[1], dmax[2], dmax[3]);
}

/* ---- Werkzeug-Modus --fall ----------------------------------------------------------------------------------- */
typedef struct {
    const char *name;
    int dreieck;                 /* 1 = Dreieck p[0..5], 0 = Rechteck x0 y0 x1 y1 (x1/y1 exklusiv) */
    int p[6];
    double orig[3];              /* Mittelwert RGB im ORIGINAL-Bild (DuckStation-Bildpuffer) */
    double tol;
    const char *quelle;
} fall_t;
static const fall_t k_fall[] = {
    /* ROOM5060 Cut 4, Halbviereck (1,2,3) von obj 4 (Abnahme 0 §5.2b: (-666,509),(-20,20),(36,142)), Pixelmitten im
     * Dreieck. Original (31.6,15.3,26.1); v0.8.25 ohne Scheibe (47.2,30.5,36.6); deckend (6.3,0.5,6.6). */
    { "5060c4", 1, { -666, 509, -20, 20, 36, 142 }, { 31.6, 15.3, 26.1 }, 6.0,
      "W_belege/orig_abe_5060_cut4_buf0.png (Spieler (-20926,0,-25850), Cut 4)" },
    /* ROOM20A0 Cut 2, Kanal (Wasser obj 4, Typ 3, ABR 2 = B - F). Original (0.4,0.4,0.3); v0.8.25 deckend (11.8,14.3,11.7). */
    { "20a0c2", 0, { 160, 180, 300, 236, 0, 0 }, { 0.4, 0.4, 0.3 }, 4.0,
      "W_belege/orig_abe_20a0_cut2_buf0.png (Spieler (8750,-3600,-22932), Cut 2)" },
    /* ROOM4090 Cut 0, Boden mit Nebel (obj 0-2, ABR 3 = B + F/4). Original (140.5,139.4,147.9); v0.8.25 (56.5,55.7,55.1). */
    { "4090c0", 0, { 20, 160, 300, 236, 0, 0 }, { 140.5, 139.4, 147.9 }, 8.0,
      "W_belege/orig_abe_4090_cut0_buf0.png (Spieler (-21000,0,-24600), Cut 0)" },
};

static uint8_t *lies_ppm(const char *path, int *w, int *h)
{
    size_t sz = 0;
    uint8_t *d = read_file(path, &sz);
    if (!d || sz < 16 || d[0] != 'P' || d[1] != '6') { free(d); return NULL; }
    int vals[3], n = 0;
    size_t i = 2;
    while (n < 3 && i < sz) {
        while (i < sz && (d[i] == ' ' || d[i] == '\n' || d[i] == '\r' || d[i] == '\t')) i++;
        if (i < sz && d[i] == '#') { while (i < sz && d[i] != '\n') i++; continue; }
        int v = 0;
        while (i < sz && d[i] >= '0' && d[i] <= '9') v = v * 10 + (d[i++] - '0');
        vals[n++] = v;
    }
    i++;
    *w = vals[0]; *h = vals[1];
    if (n < 3 || vals[2] != 255 || i + (size_t)(*w) * (*h) * 3 > sz) { free(d); return NULL; }
    uint8_t *px = (uint8_t *)malloc((size_t)(*w) * (*h) * 3);
    memcpy(px, d + i, (size_t)(*w) * (*h) * 3);
    free(d);
    return px;
}

static int in_dreieck(double px, double py, const int *p)
{
    const double ax = p[0], ay = p[1], bx = p[2], by = p[3], cx = p[4], cy = p[5];
    const double d1 = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
    const double d2 = (cx - bx) * (py - by) - (cy - by) * (px - bx);
    const double d3 = (ax - cx) * (py - cy) - (ay - cy) * (px - cx);
    return (d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0);
}

static int werkzeug_fall(const char *name, const char *ppm)
{
    const fall_t *f = NULL;
    for (size_t i = 0; i < sizeof k_fall / sizeof k_fall[0]; i++)
        if (strcmp(k_fall[i].name, name) == 0) f = &k_fall[i];
    if (!f) { printf("FALL %s unbekannt\n", name); return 2; }
    int w = 0, h = 0;
    uint8_t *px = lies_ppm(ppm, &w, &h);
    if (!px || w != 320 || h != 240) { printf("FALL %s: %s kein 320x240-PPM\n", name, ppm); free(px); return 2; }
    double s[3] = { 0, 0, 0 };
    long n = 0;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            const int drin = f->dreieck ? in_dreieck(x + 0.5, y + 0.5, f->p)
                                        : (x >= f->p[0] && y >= f->p[1] && x < f->p[2] && y < f->p[3]);
            if (!drin) continue;
            for (int k = 0; k < 3; k++) s[k] += px[(y * w + x) * 3 + k];
            n++;
        }
    free(px);
    if (n == 0) { printf("FALL %s: leere Messflaeche\n", name); return 2; }
    double dmax = 0;
    for (int k = 0; k < 3; k++) {
        s[k] /= (double)n;
        const double dd = fabs(s[k] - f->orig[k]);
        if (dd > dmax) dmax = dd;
    }
    printf("FALL %s: %ld Pixel, Mittel RGB (%.1f,%.1f,%.1f), Original (%.1f,%.1f,%.1f) [%s], max. Abstand %.1f "
           "(Toleranz %.1f) -> %s\n", name, n, s[0], s[1], s[2], f->orig[0], f->orig[1], f->orig[2], f->quelle, dmax,
           f->tol, dmax <= f->tol ? "OK" : "ABWEICHUNG");
    return dmax <= f->tol ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "--fall") == 0) return werkzeug_fall(argv[2], argv[3]);

    for (size_t i = 0; i < sizeof k_regel / sizeof k_regel[0]; i++) {
        const regel_t *r = &k_regel[i];
        const int abe = re15_objabe_r38_abe(r->attr), abr = re15_objabe_r38_abr(r->attr, r->page),
                  fl = re15_objabe_r38_flach(r->attr, r->typ);
        CHECK(abe == r->abe && abr == r->abr && fl == r->flach,
              "A1 %s: Attribut 0x%04x Typ %d Seite 0x%04x -> ABE %d ABR %d flach %d, erwartet %d/%d/%d", r->was,
              r->attr, r->typ, r->page, abe, abr, fl, r->abe, r->abr, r->flach);
    }
    printf("A1 Regeln: %d Faelle geprueft\n", (int)(sizeof k_regel / sizeof k_regel[0]));
    CHECK(re15_objabe_r38_texel_klasse(0x0000) == 0 && re15_objabe_r38_texel_klasse(0x7FFF) == 1 &&
          re15_objabe_r38_texel_klasse(0x8000) == 2 && re15_objabe_r38_texel_klasse(0x90A1) == 2,
          "A1 Texel-Klassen (psx-spx CLUT: 0000h durchsichtig, 0001h..7FFFh deckend, 8000h..FFFFh halb)");

    pruefe_original();
    pruefe_teilung("STAGE1/ROOM11B0.RDT", 0, 1);
    pruefe_teilung("STAGE5/ROOM5060.RDT", 4, 0);
    pruefe_mischung();

    if (g_fail) {
        printf("test_r38_rolltor20a0_abe: %d FEHLER\n", g_fail);
        return 1;
    }
    printf("test_r38_rolltor20a0_abe: OK\n");
    return 0;
}
