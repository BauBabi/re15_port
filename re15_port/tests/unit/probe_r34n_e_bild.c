/* probe_r34n_e_bild.c — AUSWERTER (kein add_test) fuer den Bild-Riegel
 * integration_r34n_e_dokumente_bild (Runde 34 Nacht, Spur E "Vier neue Dokumente").
 *
 * Vergleicht zwei Framedumps (PPM P6, RE15_FRAMEDUMP) desselben Laufs — einmal mit dem Dokument
 * in der Welt, einmal mit gesetztem Genommen-Bit — und zaehlt im Fenster um die projizierte
 * Dokument-Mitte die Pixel, deren Farbabstand |dR|+|dG|+|dB| > 30 ist. Beide Laeufe sind bis auf
 * das Bit gleich (gleiche Karte, gleiche Eingaben), der Unterschied IST also das Dokument.
 *
 * Aufruf: probe_r34n_e_bild <mit.ppm> <ohne.ppm> <x0> <y0> <x1> <y1> <min_px>
 * Ausgabe: "FENSTER n / AUSSEN m"; Rueckgabe 0, wenn n >= min_px, sonst 1 (2 = Lesefehler).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;

static int lies_zahl(FILE *f)
{
    int c, v = 0, hat = 0;
    for (;;) {
        c = fgetc(f);
        if (c == '#') { while (c != '\n' && c != EOF) c = fgetc(f); continue; }
        if (c == EOF) return -1;
        if (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); hat = 1; }
        else if (hat) return v;
    }
}

static int lade(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    if (fgetc(f) != 'P' || fgetc(f) != '6') { fclose(f); return -1; }
    b->w = lies_zahl(f); b->h = lies_zahl(f);
    int maxv = lies_zahl(f);
    if (b->w <= 0 || b->h <= 0 || maxv != 255) { fclose(f); return -1; }
    size_t n = (size_t)b->w * (size_t)b->h * 3;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 8) { printf("FAIL: Aufruf <mit.ppm> <ohne.ppm> x0 y0 x1 y1 min_px\n"); return 2; }
    bild_t a, b;
    if (lade(argv[1], &a) || lade(argv[2], &b) || a.w != b.w || a.h != b.h) {
        printf("FAIL: Bilder nicht lesbar oder verschieden gross\n");
        return 2;
    }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
    int min_px = atoi(argv[7]);
    long innen = 0, aussen = 0;
    int bx0 = a.w, by0 = a.h, bx1 = -1, by1 = -1;
    for (int y = 0; y < a.h; y++)
        for (int x = 0; x < a.w; x++) {
            const unsigned char *p = a.px + ((size_t)y * a.w + x) * 3;
            const unsigned char *q = b.px + ((size_t)y * a.w + x) * 3;
            int d = abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]);
            if (d <= 30) continue;
            if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                innen++;
                if (x < bx0) bx0 = x;
                if (y < by0) by0 = y;
                if (x > bx1) bx1 = x;
                if (y > by1) by1 = y;
            } else aussen++;
        }
    printf("FENSTER x%d..%d y%d..%d: %ld Pixel verschieden (Soll >= %d), Huelle x%d..%d y%d..%d | "
           "AUSSEN: %ld\n", x0, x1, y0, y1, innen, min_px, bx0, bx1, by0, by1, aussen);
    return innen >= min_px ? 0 : 1;
}
