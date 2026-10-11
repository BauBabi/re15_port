/*
 * probe_r36_ppm_diff — MESS-WERKZEUG (kein add_test) fuer integration_r36_pri_schatten (Runde 36 Spur O).
 *
 * Aufruf: probe_r36_ppm_diff <a.ppm> <b.ppm> <x0> <y0> <x1> <y1> <schwelle>
 * Zaehlt im Rechteck [x0,x1) x [y0,y1) die Bildpunkte, deren groesste Kanaldifferenz > schwelle ist, und gibt
 * "n=<anzahl> max=<groesste Differenz>" aus. Rueckgabe 0 = gelesen, 2 = Datei/Format unbrauchbar.
 *
 * WOZU EINE SCHWELLE: die Vordergrundmasken sind RE-BLITS des Hintergrunds aus einem 8-bpp-Atlas (CLUT auf
 * 5 Bit gerundet, tools/maske/atlas.py "CLUT runden": mittlerer Kanalfehler +0.46, Spitze < 8). Mit und ohne
 * Masken unterscheidet sich deshalb JEDER maskierte Bildpunkt um ein paar Stufen — ohne Schwelle zaehlte
 * probe_r34_ppm_beitrag 1214 "geaenderte" Punkte im Schattenfenster, obwohl nichts sichtbar ueberdeckt war
 * (gemessen 2026-10-11). Der Schattenblob subtrahiert 16..96 (TEX.TIM CLUT 272,503, Memory reai-v2-char-shadow);
 * die Messungen des Dossiers O_pri.md benutzen durchgehend die Schwelle 24.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *lies_ppm(const char *p, int *w, int *h)
{
    FILE *f = fopen(p, "rb");
    char magic[3] = {0};
    int maxv = 0;
    size_t n;
    unsigned char *b;
    if (!f) return NULL;
    if (fscanf(f, "%2s %d %d %d", magic, w, h, &maxv) != 4 || strcmp(magic, "P6") != 0 || maxv != 255 ||
        *w <= 0 || *h <= 0) { fclose(f); return NULL; }
    fgetc(f);
    n = (size_t)(*w) * (size_t)(*h) * 3u;
    b = (unsigned char *)malloc(n);
    if (!b || fread(b, 1, n, f) != n) { free(b); fclose(f); return NULL; }
    fclose(f);
    return b;
}

int main(int argc, char **argv)
{
    int wa, ha, wb, hb, x0, y0, x1, y1, schwelle, x, y, n = 0, mx = 0;
    unsigned char *a, *b;
    if (argc != 8) { fprintf(stderr, "Aufruf: %s a.ppm b.ppm x0 y0 x1 y1 schwelle\n", argv[0]); return 2; }
    a = lies_ppm(argv[1], &wa, &ha); b = lies_ppm(argv[2], &wb, &hb);
    if (!a || !b || wa != wb || ha != hb) { fprintf(stderr, "PPM unlesbar oder ungleich gross\n"); return 2; }
    x0 = atoi(argv[3]); y0 = atoi(argv[4]); x1 = atoi(argv[5]); y1 = atoi(argv[6]); schwelle = atoi(argv[7]);
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0; if (x1 > wa) x1 = wa; if (y1 > ha) y1 = ha;
    for (y = y0; y < y1; y++)
        for (x = x0; x < x1; x++) {
            int k, d = 0;
            for (k = 0; k < 3; k++) {
                int v = (int)a[(y * wa + x) * 3 + k] - (int)b[(y * wa + x) * 3 + k];
                if (v < 0) v = -v;
                if (v > d) d = v;
            }
            if (d > mx) mx = d;
            if (d > schwelle) n++;
        }
    printf("n=%d max=%d\n", n, mx);
    free(a); free(b);
    return 0;
}
