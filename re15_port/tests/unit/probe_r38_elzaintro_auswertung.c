/* probe_r38_elzaintro_auswertung.c — Runde 38 Spur E: Auswertung der Integrationslaeufe (kein add_test).
 *
 *   rms <cap.raw> <exit-bild> <vorlauf> <bild> <laenge> [<bild> <laenge> ...]
 *       RE15_AUDIO_CAP_SYNC-Mitschnitt (Stereo s16, 1470 Frames je Spielbild). Der Lauf endet in Bild <exit-bild>
 *       des LETZTEN Raum-Abschnitts; <vorlauf> = Bildzahl des Abschnitts davor (0 = der Lauf endet im gemessenen
 *       Abschnitt). Bild f des gemessenen Abschnitts liegt bei Takt (Takte - (exit+1) - vorlauf) + f.
 *       Je Fenster: RMS des Mono-Mittels ueber alle Samples. Ausgabe "fenster B..E rms R".
 *   bild <a.ppm> <b.ppm> <x0> <y0> <x1> <y1>
 *       Pixelvergleich zweier gleich grosser P6-Bilder: "verschieden N hell_a A hell_b B feld_a FA feld_b FB",
 *       hell = max(R,G,B) > 16, feld = helle Pixel im Rechteck x0..x1 / y0..y1 (einschliesslich).
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static unsigned char *ppm(const char *p, int *w, int *h)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    char m[3] = {0}; int mx = 0;
    if (fscanf(f, "%2s %d %d %d", m, w, h, &mx) != 4 || strcmp(m, "P6")) { fclose(f); return NULL; }
    fgetc(f);
    size_t n = (size_t)(*w) * (size_t)(*h) * 3;
    unsigned char *b = (unsigned char *)malloc(n);
    if (b && fread(b, 1, n, f) != n) { free(b); b = NULL; }
    fclose(f);
    return b;
}

static int hell(const unsigned char *p) { int m = p[0]; if (p[1] > m) m = p[1]; if (p[2] > m) m = p[2]; return m > 16; }

int main(int argc, char **argv)
{
    if (argc >= 7 && !strcmp(argv[1], "rms")) {
        FILE *f = fopen(argv[2], "rb"); if (!f) { printf("FEHLER: %s\n", argv[2]); return 1; }
        fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
        int16_t *s = (int16_t *)malloc((size_t)sz);
        if (!s || fread(s, 1, (size_t)sz, f) != (size_t)sz) { printf("FEHLER: lesen\n"); return 1; }
        fclose(f);
        const long T = 1470; long ticks = sz / 4 / T;
        long off = ticks - (atol(argv[3]) + 1) - atol(argv[4]);
        if (off < 0) { printf("FEHLER: Mitschnitt kuerzer als die Abschnitte\n"); return 1; }
        printf("takte %ld abschnitt_bild0 %ld\n", ticks, off);
        for (int a = 5; a + 1 < argc; a += 2) {
            long b0 = atol(argv[a]), l = atol(argv[a + 1]);
            double q = 0.0; long n = 0;
            for (long t = off + b0; t < off + b0 + l && t < ticks; t++)
                for (long i = 0; i < T; i++) {
                    double v = ((double)s[(t * T + i) * 2] + (double)s[(t * T + i) * 2 + 1]) / 2.0;
                    q += v * v; n++;
                }
            printf("fenster %ld..%ld rms %.0f\n", b0, b0 + l - 1, n ? sqrt(q / (double)n) : 0.0);
        }
        free(s);
        return 0;
    }
    if (argc >= 8 && !strcmp(argv[1], "bild")) {
        int w1, h1, w2, h2;
        unsigned char *a = ppm(argv[2], &w1, &h1), *b = ppm(argv[3], &w2, &h2);
        if (!a || !b || w1 != w2 || h1 != h2) { printf("FEHLER: Bilder\n"); return 1; }
        int x0 = atoi(argv[4]), y0 = atoi(argv[5]), x1 = atoi(argv[6]), y1 = atoi(argv[7]);
        long versch = 0, ha = 0, hb = 0, fa = 0, fb = 0;
        for (int y = 0; y < h1; y++)
            for (int x = 0; x < w1; x++) {
                const unsigned char *pa = a + ((size_t)y * w1 + x) * 3, *pb = b + ((size_t)y * w1 + x) * 3;
                if (memcmp(pa, pb, 3)) versch++;
                ha += hell(pa); hb += hell(pb);
                if (x >= x0 && x <= x1 && y >= y0 && y <= y1) { fa += hell(pa); fb += hell(pb); }
            }
        printf("verschieden %ld hell_a %ld hell_b %ld feld_a %ld feld_b %ld\n", versch, ha, hb, fa, fb);
        free(a); free(b);
        return 0;
    }
    printf("Aufruf: rms <cap.raw> <exit-bild> <vorlauf> <bild> <laenge> ... | bild <a.ppm> <b.ppm> x0 y0 x1 y1\n");
    return 2;
}
