/* probe_r38_tagebuch_bild.c — MESS-WERKZEUG (kein add_test) der Integration test_r38_tagebuch.cmake, Runde 38 Spur G.
 * Wertet Framedumps (PPM P6, 320x240, RE15_WINDOW_SCALE=1) der echten exe aus:
 *
 *   gruen <ppm> <x0> <y0> <x1> <y1> <soll>
 *       Pixel in den sechs Farben der Textmaler-Zeile 482 (DATA/TEX.TIM CLUT-Zeile 2, Eintraege 1..6 — Steuerbyte
 *       0x05 Arg 1 @0x80028974-94) zaehlen: im Kasten genau <soll>, ausserhalb 0.
 *   blink <praefix> <von> <bis> <schritt> <blatt> <rechteck> <min>
 *       Folge <praefix>NNNNNN.ppm: fuer jedes Paar aufeinanderfolgender Bilder die Differenz; liegt ihr Huellkasten
 *       GANZ im Bildschirm-Kasten der Kachel (re15_map_rect_geometry(blatt, rechteck) = Sprite-Lage, re15_inv_screen.c
 *       Zielkachel-Zweig) und hat sie >= <min> Pixel, zaehlt sie als Blinkwechsel dieser Kachel. Ausgabe: Zahl der
 *       Wechsel und Bild des ersten; Erfolg = mindestens einer.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_room.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

typedef struct { int w, h; uint8_t *rgb; } bild_t;

static int ppm_lesen(const char *p, bild_t *b)
{
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    char mag[3] = { 0 };
    int w = 0, h = 0, mx = 0;
    if (fscanf(f, "%2s %d %d %d", mag, &w, &h, &mx) != 4 || strcmp(mag, "P6") || mx != 255 || w <= 0 || h <= 0) {
        fclose(f); return 0;
    }
    fgetc(f);
    b->w = w; b->h = h;
    b->rgb = (uint8_t *)malloc((size_t)w * h * 3);
    int ok = b->rgb && fread(b->rgb, 1, (size_t)w * h * 3, f) == (size_t)w * h * 3;
    fclose(f);
    return ok;
}

static int tex_gruen(uint8_t out[6][3])
{
    char p[512];
    snprintf(p, sizeof p, "%s/DATA/TEX.TIM", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    uint8_t k[20];
    if (fread(k, 1, 20, f) != 20) { fclose(f); return 0; }
    int cx = k[12] | k[13] << 8, cy = k[14] | k[15] << 8, cw = k[16] | k[17] << 8;
    if (k[0] != 0x10 || !(k[4] & 8) || cx != 256 || cy != 480) { fclose(f); return 0; }
    uint8_t z[32];
    fseek(f, 20 + 2L * cw * 2, SEEK_SET);                       /* Zeile 2 = y 482 */
    if (fread(z, 1, 32, f) != 32) { fclose(f); return 0; }
    fclose(f);
    for (int i = 0; i < 6; i++) {
        unsigned c = (unsigned)(z[2 * (i + 1)] | z[2 * (i + 1) + 1] << 8);
        out[i][0] = (uint8_t)((c & 31) << 3); out[i][1] = (uint8_t)(((c >> 5) & 31) << 3);
        out[i][2] = (uint8_t)(((c >> 10) & 31) << 3);
    }
    return 1;
}

static int gruen(int argc, char **argv)
{
    if (argc < 8) return 2;
    bild_t b;
    uint8_t g[6][3];
    if (!ppm_lesen(argv[2], &b) || !tex_gruen(g)) { printf("FEHLER: Bild/TEX.TIM nicht lesbar\n"); return 1; }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]), soll = atoi(argv[7]);
    int innen = 0, aussen = 0, bx0 = 9999, bx1 = -1;
    for (int y = 0; y < b.h; y++)
        for (int x = 0; x < b.w; x++) {
            const uint8_t *p = b.rgb + ((size_t)y * b.w + x) * 3;
            int treffer = 0;
            for (int i = 0; i < 6; i++) if (p[0] == g[i][0] && p[1] == g[i][1] && p[2] == g[i][2]) treffer = 1;
            if (!treffer) continue;
            if (x >= x0 && x <= x1 && y >= y0 && y <= y1) { innen++; if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; }
            else aussen++;
        }
    printf("gruen: im Kasten (%d,%d)-(%d,%d) %d Pixel (x %d..%d), ausserhalb %d; Soll %d / 0\n",
           x0, y0, x1, y1, innen, bx0, bx1, aussen, soll);
    return (innen == soll && aussen == 0) ? 0 : 1;
}

static int blink(int argc, char **argv)
{
    if (argc < 9) return 2;
    const char *pre = argv[2];
    int von = atoi(argv[3]), bis = atoi(argv[4]), schritt = atoi(argv[5]);
    int blatt = atoi(argv[6]), rechteck = atoi(argv[7]), min = atoi(argv[8]);
    int rx = 0, ry = 0, rw = 0, rh = 0;
    if (!re15_map_rect_geometry((unsigned)blatt, (unsigned)rechteck, &rx, &ry, &rw, &rh)) {
        printf("FEHLER: keine Geometrie Blatt %d Rechteck %d\n", blatt, rechteck); return 1;
    }
    int wechsel = 0, erstes = -1, gelesen = 0;
    bild_t a = { 0 }, c = { 0 };
    char p[700];
    for (int f = von; f <= bis; f += schritt) {
        snprintf(p, sizeof p, "%s%06d.ppm", pre, f);
        bild_t n = { 0 };
        if (!ppm_lesen(p, &n)) continue;
        gelesen++;
        if (a.rgb && a.w == n.w && a.h == n.h) {
            int d = 0, x0 = 9999, y0 = 9999, x1 = -1, y1 = -1;
            for (int y = 0; y < n.h; y++)
                for (int x = 0; x < n.w; x++) {
                    const uint8_t *q = a.rgb + ((size_t)y * n.w + x) * 3, *r = n.rgb + ((size_t)y * n.w + x) * 3;
                    if (q[0] == r[0] && q[1] == r[1] && q[2] == r[2]) continue;
                    d++;
                    if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
                }
            if (d >= min && x0 >= rx && x1 <= rx + rw - 1 && y0 >= ry && y1 <= ry + rh - 1) {
                if (!wechsel) erstes = f;
                wechsel++;
                printf("  Bild %d: %d Pixel gewechselt, Huelle (%d,%d)-(%d,%d)\n", f, d, x0, y0, x1, y1);
            }
        }
        free(a.rgb);
        a = n;
    }
    free(a.rgb); (void)c;
    printf("blink Blatt %d Rechteck %d Kasten (%d,%d)-(%d,%d): %d Wechsel in %d Bildern, erster bei %d\n",
           blatt, rechteck, rx, ry, rx + rw - 1, ry + rh - 1, wechsel, gelesen, erstes);
    return wechsel > 0 ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "gruen")) return gruen(argc, argv);
    if (argc > 1 && !strcmp(argv[1], "blink")) return blink(argc, argv);
    printf("Aufruf: gruen <ppm> x0 y0 x1 y1 soll | blink <praefix> von bis schritt blatt rechteck min\n");
    return 2;
}
