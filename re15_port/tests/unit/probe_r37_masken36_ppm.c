/*
 * Runde 37 Spur P (masken36) — Messwerkzeug fuer integration_r37_masken36_*: drei Framedumps derselben Szene
 *   A = mit Masken, B = ohne Masken (RE15_NO_PRI=1), C = ohne Figur (Leon ausserhalb des Regionsvierecks -> Akteur-Cull)
 * Im Fenster x0..x1-1, y0..y1-1: Figurpunkte S = Kanaldifferenz |B - C| > schwelle, davon uebermalt H = |A - B| > schwelle.
 * Ausgabe "S=<n> H=<n>". Aufruf: probe_r37_masken36_ppm <A.ppm> <B.ppm> <C.ppm> x0 y0 x1 y1 schwelle
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *lies_ppm(const char *pfad, int *w, int *h)
{
    FILE *f = fopen(pfad, "rb"); char kopf[3] = {0}; int mx; unsigned char *p;
    if (!f) return NULL;
    if (fscanf(f, "%2s %d %d %d", kopf, w, h, &mx) != 4 || strcmp(kopf, "P6") || *w <= 0 || *h <= 0) { fclose(f); return NULL; }
    fgetc(f);
    p = (unsigned char *) malloc((size_t) (*w) * (*h) * 3);
    if (p && fread(p, 1, (size_t) (*w) * (*h) * 3, f) != (size_t) (*w) * (*h) * 3) { free(p); p = NULL; }
    fclose(f);
    return p;
}

static int diff(const unsigned char *a, const unsigned char *b, int i, int s)
{
    int k;
    for (k = 0; k < 3; k++) if (abs((int) a[i * 3 + k] - (int) b[i * 3 + k]) > s) return 1;
    return 0;
}

int main(int argc, char **argv)
{
    int wa, ha, wb, hb, wc, hc, x0, y0, x1, y1, s, x, y, nS = 0, nH = 0;
    unsigned char *A, *B, *C;
    if (argc < 9) { fprintf(stderr, "Aufruf: probe_r37_masken36_ppm A B C x0 y0 x1 y1 schwelle\n"); return 2; }
    A = lies_ppm(argv[1], &wa, &ha); B = lies_ppm(argv[2], &wb, &hb); C = lies_ppm(argv[3], &wc, &hc);
    if (!A || !B || !C || wa != wb || wa != wc || ha != hb || ha != hc) { fprintf(stderr, "PPM fehlt/ungleich\n"); return 1; }
    x0 = atoi(argv[4]); y0 = atoi(argv[5]); x1 = atoi(argv[6]); y1 = atoi(argv[7]); s = atoi(argv[8]);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > wa) x1 = wa;
    if (y1 > ha) y1 = ha;
    for (y = y0; y < y1; y++)
        for (x = x0; x < x1; x++) {
            int i = y * wa + x;
            if (!diff(B, C, i, s)) continue;
            nS++;
            if (diff(A, B, i, s)) nH++;
        }
    printf("S=%d H=%d\n", nS, nH);
    free(A); free(B); free(C);
    return 0;
}
