/*
 * Runde 35 Spur F, Punkt 2 — "Bei unseren selbsterstellten Dokumenten moechte ich, das der Code
 * darin mit diesen gruen hervorgehoben wird." (AUFTRAG.md Z. 18). Dossier:
 * analysis/befunde_runde35/F_inhalt.md, Punkt 2.
 *
 * Misst ueber den ECHTEN Seitenlader des Ports (re15_re2doc_pixel, re2doc_common.c — dieselbe
 * Routine, die der FILE-Leser zeichnet), nicht ueber das Satz-Werkzeug:
 *   A  Das Soll-Gruen ist das des Textmalers fuer Steuerbyte 0x05 Argument 1: FUN_80028868
 *      `andi v1,v0,0x4` @0x80028974 .. `ori s5,v0,0x10` @0x80028994 -> CLUT-Zeile
 *      y = 480 + (1&3)*2 = 482 = Zeile 2 des CLUT-Blocks von DATA/TEX.TIM (CLUT-Kopf x 256 y 480).
 *      Gelesen aus der Datei, Eintraege 1..6. Zusaetzlich: Zeile 0 (weiss) 1..6 == Kernverlauf der
 *      Dokumentseiten (sonst waere die 1:1-Abbildung Kern i -> Gruen i nicht belegt).
 *   B  FILE28 p01 ("The new code is: 4312") und FILE29 p01 ("... with the code: 5632"): es gibt
 *      gruene Pixel, und ALLE liegen im Kasten des Codes (Satzbericht doc_satz_brief.py:
 *      FILE28 Zeile 6 Kern-x 147..179, FILE29 Zeile 9 Kern-x 5..36), und im Kasten liegt kein
 *      weisser Kernpixel mehr (der ganze Code ist gruen, nicht nur ein Teil).
 *   C  Keine andere Seite der fuenf eigenen Dokumente (FILE25..29, Titel + alle Textseiten)
 *      traegt ein gruenes Pixel — nur die Codes sind hervorgehoben.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_re2doc.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_ASSET_RE2_DIR
#error RE15_ASSET_RE2_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

typedef struct { uint8_t r, g, b; } rgb_t;

static rgb_t rgb555(uint16_t c)
{
    rgb_t o = { (uint8_t)((c & 31) << 3), (uint8_t)(((c >> 5) & 31) << 3), (uint8_t)(((c >> 10) & 31) << 3) };
    return o;
}

/* TEX.TIM: Zeile `zeile` des CLUT-Blocks, Eintraege 0..15. */
static int tex_zeile(int zeile, uint16_t out[16])
{
    char p[512];
    snprintf(p, sizeof p, "%s/DATA/TEX.TIM", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    uint8_t kopf[20];
    if (fread(kopf, 1, 20, f) != 20) { fclose(f); return 0; }
    uint32_t magic = kopf[0] | kopf[1] << 8, flags = kopf[4] | kopf[5] << 8;
    int cx = kopf[12] | kopf[13] << 8, cy = kopf[14] | kopf[15] << 8;
    int cw = kopf[16] | kopf[17] << 8, ch = kopf[18] | kopf[19] << 8;
    if (magic != 0x10 || !(flags & 8) || cx != 256 || cy != 480 || zeile >= ch) { fclose(f); return 0; }
    fseek(f, 20 + (long)zeile * cw * 2, SEEK_SET);
    uint8_t b[32];
    if (fread(b, 1, 32, f) != 32) { fclose(f); return 0; }
    fclose(f);
    for (int i = 0; i < 16; i++) out[i] = (uint16_t)(b[2 * i] | b[2 * i + 1] << 8);
    return 1;
}

static rgb_t s_gruen[6], s_weiss[6];

static int in_menge(const rgb_t *m, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < 6; i++) if (m[i].r == r && m[i].g == g && m[i].b == b) return 1;
    return 0;
}

typedef struct { int n, x0, x1, y0, y1; } zaehl_t;

/* Gruene bzw. weisse Kernpixel einer Seite zaehlen (page < 0 = Titelseite). */
static void zaehle(int doc, int page, const rgb_t *menge, zaehl_t *z, int bx0, int bx1, int by0, int by1,
                   int *im_kasten)
{
    int w = 0, h = 0;
    memset(z, 0, sizeof *z);
    z->x0 = z->y0 = 99999; z->x1 = z->y1 = -1;
    if (im_kasten) *im_kasten = 0;
    if (!re15_re2doc_size(doc, page, RE15_RE2DOC_PAGE, &w, &h)) return;
    for (int v = 0; v < h; v++)
        for (int u = 0; u < w; u++) {
            uint8_t r, g, b;
            if (!re15_re2doc_pixel(doc, page, RE15_RE2DOC_PAGE, u, v, &r, &g, &b)) continue;
            if (!in_menge(menge, r, g, b)) continue;
            z->n++;
            if (u < z->x0) z->x0 = u;
            if (u > z->x1) z->x1 = u;
            if (v < z->y0) z->y0 = v;
            if (v > z->y1) z->y1 = v;
            if (im_kasten && u >= bx0 && u <= bx1 && v >= by0 && v <= by1) (*im_kasten)++;
        }
}

int main(void)
{
    char root[512];
    snprintf(root, sizeof root, "%s/FILES", RE15_ASSET_RE2_DIR);
    re15_re2doc_set_root(root);

    /* ---- A: das Soll-Gruen aus TEX.TIM ---- */
    uint16_t z0[16], z2[16];
    /* @0x80028984 `addiu v1,v1,480` + (N&3)*2 (@0x8002897c/80) fuer N = 1 -> 482 - 480 = 2 */
    const int zeile_gruen = (480 + (1 & 3) * 2 + ((1 & 4) ? 1 : 0)) - 480;
    CHECK(zeile_gruen == 2, "A Farbcode 05 01 -> CLUT-Zeile %d (y 482, @0x80028974-94)", zeile_gruen);
    CHECK(tex_zeile(0, z0) && tex_zeile(zeile_gruen, z2), "A DATA/TEX.TIM CLUT gelesen (Kopf x 256 y 480)");
    for (int i = 0; i < 6; i++) { s_gruen[i] = rgb555(z2[i + 1]); s_weiss[i] = rgb555(z0[i + 1]); }
    CHECK(s_gruen[0].r == 0 && s_gruen[0].g == 184 && s_gruen[0].b == 40,
          "A Gruen Eintrag 1 = (%d,%d,%d) (TEX.TIM @0x96 = e0 16)", s_gruen[0].r, s_gruen[0].g, s_gruen[0].b);

    /* Kernverlauf der Dokumentseiten == Zeile 0 (weiss): an einem bekannten weissen Text messen —
     * FILE28 p01 enthaelt weisse Kernpixel aller Stufen. */
    {
        int stufen[6] = { 0 };
        int w = 0, h = 0;
        re15_re2doc_size(28, 1, RE15_RE2DOC_PAGE, &w, &h);
        for (int v = 0; v < h; v++)
            for (int u = 0; u < w; u++) {
                uint8_t r, g, b;
                if (!re15_re2doc_pixel(28, 1, RE15_RE2DOC_PAGE, u, v, &r, &g, &b)) continue;
                for (int i = 0; i < 6; i++)
                    if (s_weiss[i].r == r && s_weiss[i].g == g && s_weiss[i].b == b) stufen[i]++;
            }
        int alle = 1;
        for (int i = 0; i < 6; i++) if (!stufen[i]) alle = 0;
        CHECK(alle, "A Dokument-Kern nutzt die 6 Farben der weissen Textmaler-Zeile 0 (%d/%d/%d/%d/%d/%d Pixel)",
              stufen[0], stufen[1], stufen[2], stufen[3], stufen[4], stufen[5]);
    }

    /* ---- B: der Code ist gruen, ganz, und nur er ---- */
    static const struct { int doc, page; const char *code; int x0, x1, y0, y1; } k_codes[2] = {
        { 28, 1, "4312", 147, 179, 96, 111 },   /* FILE28_satz.txt: p01 Zeile 6 Kern-x 147..179 */
        { 29, 1, "5632",   5,  36, 144, 159 },  /* FILE29_satz.txt: p01 Zeile 9 Kern-x 5..36    */
    };
    for (int k = 0; k < 2; k++) {
        zaehl_t g, w;
        int g_kasten = 0, w_kasten = 0;
        zaehle(k_codes[k].doc, k_codes[k].page, s_gruen, &g, k_codes[k].x0, k_codes[k].x1,
               k_codes[k].y0, k_codes[k].y1, &g_kasten);
        zaehle(k_codes[k].doc, k_codes[k].page, s_weiss, &w, k_codes[k].x0, k_codes[k].x1,
               k_codes[k].y0, k_codes[k].y1, &w_kasten);
        CHECK(g.n > 40, "B FILE%d p%02d Code \"%s\": %d gruene Kernpixel", k_codes[k].doc, k_codes[k].page,
              k_codes[k].code, g.n);
        CHECK(g_kasten == g.n, "B FILE%d alle gruenen Pixel im Code-Kasten x %d..%d y %d..%d "
              "(gemessen x %d..%d y %d..%d)", k_codes[k].doc, k_codes[k].x0, k_codes[k].x1,
              k_codes[k].y0, k_codes[k].y1, g.x0, g.x1, g.y0, g.y1);
        CHECK(g.x0 == k_codes[k].x0 && g.x1 == k_codes[k].x1,
              "B FILE%d gruen ueber die GANZE Codebreite (x %d..%d)", k_codes[k].doc, g.x0, g.x1);
        CHECK(w_kasten == 0, "B FILE%d im Code-Kasten kein weisser Kernpixel mehr (%d)", k_codes[k].doc, w_kasten);
        CHECK(w.n > 1000, "B FILE%d der uebrige Text bleibt weiss (%d Kernpixel)", k_codes[k].doc, w.n);
    }

    /* ---- C: keine andere eigene Seite ist gruen ---- */
    for (int doc = 25; doc <= 29; doc++) {
        int n = re15_re2doc_page_count(doc);
        int gesamt = 0;
        for (int p = -1; p <= n; p++) {
            if (p == 1 && (doc == 28 || doc == 29)) continue;
            zaehl_t g;
            zaehle(doc, p, s_gruen, &g, 0, 0, 0, 0, NULL);
            gesamt += g.n;
        }
        CHECK(n >= 2 && gesamt == 0, "C FILE%d (%d Textseiten): 0 gruene Pixel ausserhalb der Code-Seite (%d)",
              doc, n, gesamt);
    }

    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
