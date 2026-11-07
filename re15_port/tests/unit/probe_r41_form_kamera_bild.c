/*
 * Runde 41 Spur Form, Nachbesserung 4 (Abnahme 2 M-A): Bildmass der Rueckstoss-Szene aus einer RE15_FRAMEDUMP-Serie.
 *
 * MESS-WERKZEUG (kein add_test): je vorhandenem Bild <praefix><bild 6-stellig>.ppm (P6, wie render_pc.c es schreibt) der Anteil
 * fast schwarzer Bildpunkte in Promille - Luma (299 R + 587 G + 114 B) / 1000 < 24, dieselbe Schwelle und Gewichtung wie das
 * Werkzeug der Abnahme 2 (abn2/werkzeug/hell.py, PIL 'L'). Ausgabe je Bild: "F<n> dunkel=<promille> mittel=<luma x 10>".
 *
 *   probe_r41_form_kamera_bild <praefix> <von> <bis>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ppm_lesen(const char *pfad, int *w, int *h, unsigned char **px)
{
    FILE *fp = fopen(pfad, "rb");
    char typ[3] = { 0 };
    int maxv = 0;
    size_t n;
    if (!fp) return 0;
    if (fscanf(fp, "%2s %d %d %d", typ, w, h, &maxv) != 4 || strcmp(typ, "P6") || *w <= 0 || *h <= 0 || maxv != 255) { fclose(fp); return 0; }
    fgetc(fp);                                           /* ein Trennzeichen nach dem Kopf */
    n = (size_t)(*w) * (size_t)(*h) * 3u;
    *px = (unsigned char *)malloc(n);
    if (!*px || fread(*px, 1, n, fp) != n) { fclose(fp); free(*px); *px = NULL; return 0; }
    fclose(fp);
    return 1;
}

int main(int argc, char **argv)
{
    int von, bis, gelesen = 0;
    if (argc < 4) { fprintf(stderr, "Aufruf: %s <praefix> <von> <bis>\n", argv[0]); return 2; }
    von = atoi(argv[2]); bis = atoi(argv[3]);
    for (int f = von; f <= bis; f++) {
        char pfad[1024];
        int w = 0, h = 0;
        unsigned char *px = NULL;
        long dunkel = 0, summe = 0, n;
        snprintf(pfad, sizeof pfad, "%s%06d.ppm", argv[1], f);
        if (!ppm_lesen(pfad, &w, &h, &px)) continue;
        n = (long)w * h;
        for (long i = 0; i < n; i++) {
            const long l = (299L * px[i * 3] + 587L * px[i * 3 + 1] + 114L * px[i * 3 + 2]) / 1000L;
            if (l < 24) dunkel++;
            summe += l;
        }
        printf("F%d dunkel=%ld mittel=%ld\n", f, dunkel * 1000 / n, summe * 10 / n);
        free(px);
        gelesen++;
    }
    fprintf(stderr, "%d Bilder gelesen\n", gelesen);
    return gelesen > 0 ? 0 : 1;
}
