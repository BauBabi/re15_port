/* probe_r36_kartenitems_bmp.c — AUSWERTER (kein add_test) fuer integration_r36_kartenitems_bild
 * (Runde 36 Spur N): vergleicht zwei Kartenschirm-Abzuege (RE15_INV_FB_SHOT, 24-bit BMP, unten-oben)
 * und zaehlt im Fenster die Pixel mit |dR|+|dG|+|dB| > 30.
 *
 * Aufruf: probe_r36_kartenitems_bmp <a.bmp> <b.bmp> <x0> <y0> <x1> <y1> <min_px>
 * Ausgabe "FENSTER n / AUSSEN m"; Rueckgabe 0 wenn n >= min_px, 1 sonst, 2 = Lesefehler.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;   /* RGB, oben-unten */

static int lade(const char *pfad, bild_t *b)
{
    unsigned char k[54];
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    if (fread(k, 1, 54, f) != 54 || k[0] != 'B' || k[1] != 'M') { fclose(f); return -1; }
    int off = k[10] | (k[11] << 8) | (k[12] << 16) | (k[13] << 24);
    b->w = k[18] | (k[19] << 8) | (k[20] << 16) | (k[21] << 24);
    int h = k[22] | (k[23] << 8) | (k[24] << 16) | (k[25] << 24);
    int bpp = k[28] | (k[29] << 8);
    int unten_oben = h > 0;
    b->h = h > 0 ? h : -h;
    if (bpp != 24 || b->w <= 0 || b->h <= 0) { fclose(f); return -1; }
    int zeile = (b->w * 3 + 3) & ~3;
    unsigned char *roh = (unsigned char *)malloc((size_t)zeile * (size_t)b->h);
    b->px = (unsigned char *)malloc((size_t)b->w * (size_t)b->h * 3);
    if (!roh || !b->px || fseek(f, off, SEEK_SET) != 0 ||
        fread(roh, 1, (size_t)zeile * (size_t)b->h, f) != (size_t)zeile * (size_t)b->h) {
        fclose(f); free(roh); return -1;
    }
    fclose(f);
    for (int y = 0; y < b->h; y++) {
        const unsigned char *q = roh + (size_t)(unten_oben ? (b->h - 1 - y) : y) * (size_t)zeile;
        for (int x = 0; x < b->w; x++) {
            b->px[(y * b->w + x) * 3 + 0] = q[x * 3 + 2];
            b->px[(y * b->w + x) * 3 + 1] = q[x * 3 + 1];
            b->px[(y * b->w + x) * 3 + 2] = q[x * 3 + 0];
        }
    }
    free(roh);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 8) { printf("Aufruf: %s <a.bmp> <b.bmp> <x0> <y0> <x1> <y1> <min>\n", argv[0]); return 2; }
    bild_t a, b;
    if (lade(argv[1], &a) || lade(argv[2], &b) || a.w != b.w || a.h != b.h) {
        printf("Lesefehler oder Groesse verschieden\n"); return 2;
    }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]), mn = atoi(argv[7]);
    long innen = 0, aussen = 0;
    for (int y = 0; y < a.h; y++)
        for (int x = 0; x < a.w; x++) {
            const unsigned char *p = a.px + (y * a.w + x) * 3, *q = b.px + (y * b.w + x) * 3;
            int d = abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]);
            if (d <= 30) continue;
            if (x >= x0 && x <= x1 && y >= y0 && y <= y1) innen++; else aussen++;
        }
    printf("FENSTER x%d..%d y%d..%d: %ld Pixel verschieden (Soll >= %d) | AUSSEN: %ld\n",
           x0, x1, y0, y1, innen, mn, aussen);
    return innen >= mn ? 0 : 1;
}
