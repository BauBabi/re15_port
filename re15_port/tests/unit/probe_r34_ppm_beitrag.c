/*
 * probe_r34_ppm_beitrag — MESS-WERKZEUG (kein add_test) fuer integration_r34_granaten (Integration W8).
 *
 * Aufruf: probe_r34_ppm_beitrag <ohne.ppm> <mit.ppm> <x0> <y0> <x1> <y1> <abr>
 * Rechnet je Pixel des Rechtecks, das sich zwischen den beiden Framedumps aendert, den wirksamen
 * Beitrag F*m eines halbtransparenten Effekt-Quads zurueck:
 *   ABR 0 (0.5*B + 0.5*F*m):  F*m = 2*out - B
 *   ABR 1 (B + F*m):          F*m = out - B
 * und gibt je Kanal das Maximum aus: "max=<r>,<g>,<b> n=<geaenderte Pixel>".
 * Rueckgabe 0 = gelesen, 2 = Datei/Format unbrauchbar.
 *
 * Zweck (Integration W8): das Original moduliert jedes ESP-POLY_FT4 mit der Primitivfarbe 0x80 = x 1.0
 * (FUN_800537e4 `ori s3,zero,0x80` @0x800537ec / `sb s3,4/5/6(s0)` @0x800538fc-908; FUN_800534c4 schreibt
 * nur das Code-Byte `sb v0,7(t2)` @0x8005369c). Der hellste Feuerball-Texel (Palette 483 Index 1 =
 * (248,248,248), DATA/TEX.TIM) muss deshalb mit ~248 beitragen, nicht mit ~124 (Farbe 128 als SDL-Faktor).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *lies_ppm(const char *p, int *w, int *h)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    char magic[3] = {0};
    int maxv = 0;
    if (fscanf(f, "%2s %d %d %d", magic, w, h, &maxv) != 4 || strcmp(magic, "P6") != 0 || maxv != 255 ||
        *w <= 0 || *h <= 0) { fclose(f); return NULL; }
    fgetc(f);                                   /* genau ein Trennzeichen nach maxval */
    size_t n = (size_t)(*w) * (size_t)(*h) * 3u;
    unsigned char *b = (unsigned char *)malloc(n);
    if (b && fread(b, 1, n, f) != n) { free(b); b = NULL; }
    fclose(f);
    return b;
}

int main(int argc, char **argv)
{
    if (argc != 8) { fprintf(stderr, "Aufruf: %s ohne.ppm mit.ppm x0 y0 x1 y1 abr\n", argv[0]); return 2; }
    int wa, ha, wb, hb;
    unsigned char *a = lies_ppm(argv[1], &wa, &ha);
    unsigned char *b = lies_ppm(argv[2], &wb, &hb);
    if (!a || !b || wa != wb || ha != hb) { fprintf(stderr, "PPM unbrauchbar\n"); return 2; }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]), abr = atoi(argv[7]);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > wa) x1 = wa;
    if (y1 > ha) y1 = ha;
    int mx[3] = { -1000, -1000, -1000 }, n = 0;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            const unsigned char *pa = a + ((size_t)y * (size_t)wa + (size_t)x) * 3u;
            const unsigned char *pb = b + ((size_t)y * (size_t)wa + (size_t)x) * 3u;
            if (pa[0] == pb[0] && pa[1] == pb[1] && pa[2] == pb[2]) continue;
            n++;
            for (int c = 0; c < 3; c++) {
                int f = (abr == 0) ? (2 * (int)pb[c] - (int)pa[c]) : ((int)pb[c] - (int)pa[c]);
                if (f > mx[c]) mx[c] = f;
            }
        }
    }
    printf("max=%d,%d,%d n=%d\n", mx[0], mx[1], mx[2], n);
    free(a); free(b);
    return 0;
}
