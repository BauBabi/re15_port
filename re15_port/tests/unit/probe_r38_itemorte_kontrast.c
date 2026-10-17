/* probe_r38_itemorte_kontrast.c — AUSWERTER (kein add_test) fuer integration_r38_itemorte (Runde 38 Spur K,
 * Nachbesserung 1, Mangel M1 "Prison Key nicht gut sichtbar").
 *
 * Wie probe_r34n_e_bild (Runde 34 Nacht E): zwei Framedumps (PPM P6, RE15_FRAMEDUMP) desselben Laufs — einmal mit dem
 * Gegenstand in der Welt, einmal mit gesetztem Genommen-Bit — und im Fenster die Pixel mit |dR|+|dG|+|dB| > 30 (der
 * Unterschied IST der Gegenstand). ZUSAETZLICH die KONTRASTSUMME Sum|dL| ueber diese Pixel, L = 0,299 R + 0,587 G +
 * 0,114 B (Rec. 601) — das Mass, an dem die Abnahme 0 den Mangel festgemacht hat (K_abnahme_0.md Abschnitt 5: Prison
 * Key ~500 gegen ~3430 beim Parking Lot Key, Skala 1). Gerechnet in Tausendsteln, ganzzahlig, ohne libm.
 *
 * Aufruf: probe_r38_itemorte_kontrast <mit.ppm> <ohne.ppm> <x0> <y0> <x1> <y1>
 * Ausgabe: "FENSTER x..y..: n Pixel, SUMDL s, Huelle x..y.. | AUSSEN: m"; Rueckgabe 0 (2 = Lesefehler).
 */
#include <stdio.h>
#include <stdlib.h>

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

/* Luminanz x 1000 (Rec. 601) */
static long lum1000(const unsigned char *p) { return 299L * p[0] + 587L * p[1] + 114L * p[2]; }

int main(int argc, char **argv)
{
    if (argc < 7) { printf("FAIL: Aufruf <mit.ppm> <ohne.ppm> x0 y0 x1 y1\n"); return 2; }
    bild_t a, b;
    if (lade(argv[1], &a) || lade(argv[2], &b) || a.w != b.w || a.h != b.h) {
        printf("FAIL: Bilder nicht lesbar oder verschieden gross\n");
        return 2;
    }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
    long innen = 0, aussen = 0, sumdl1000 = 0;
    int bx0 = a.w, by0 = a.h, bx1 = -1, by1 = -1;
    for (int y = 0; y < a.h; y++)
        for (int x = 0; x < a.w; x++) {
            const unsigned char *p = a.px + ((size_t)y * a.w + x) * 3;
            const unsigned char *q = b.px + ((size_t)y * a.w + x) * 3;
            int d = abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]);
            if (d <= 30) continue;
            if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                long dl = lum1000(p) - lum1000(q);
                sumdl1000 += dl < 0 ? -dl : dl;
                innen++;
                if (x < bx0) bx0 = x;
                if (y < by0) by0 = y;
                if (x > bx1) bx1 = x;
                if (y > by1) by1 = y;
            } else aussen++;
        }
    printf("FENSTER x%d..%d y%d..%d: %ld Pixel, SUMDL %ld, Huelle x%d..%d y%d..%d | AUSSEN: %ld\n",
           x0, x1, y0, y1, innen, (sumdl1000 + 500) / 1000, bx0, bx1, by0, by1, aussen);
    return 0;
}
