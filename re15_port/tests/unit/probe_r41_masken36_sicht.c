/*
 * Runde 41 Spur M "masken36", Nachbesserung 3 (Abnahme 2, Mangel 1) — Messwerkzeug fuer integration_r41_masken36_fern.
 * Dossier analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 3".
 *
 * Vier Framedumps derselben Szene (Bild 120, RE15_WINDOW_SCALE=1):
 *   A   mit Masken, Figur an der Stelle
 *   B   ohne Masken (RE15_NO_PRI=1), Figur an der Stelle
 *   C   Hintergrund des Cuts (probe_bg_dump; der figurlose Spielrahmen ist bitgleich, R41 Punkt 2)
 *   A0  mit Masken, Figur ausserhalb des Regionsvierecks des Cuts (Akteur-Cull FUN_8002c18c) = dieselben Masken ohne Figur
 * Im Fenster x0..x1-1, y0..y1-1 (Kanal-Differenz > schwelle wie probe_r37_masken36_ppm):
 *   S = Figurpunkte |B - C| > s,  H = davon uebermalt |A - B| > s,
 *   V = davon im Endbild SICHTBAR |A - A0| > s - die Figur weicht vom Maskenbild ohne Figur ab. Liegt die Figur HINTER einem
 *       Gegenstand, dessen Maske sie verdeckt, ist A an diesem Punkt exakt A0 (dieselben Masken-Texel) -> V == 0. Anders als
 *       S - H haengt V nicht davon ab, wie nah die Texelfarbe der Maske an der Figurfarbe liegt.
 * Ausgabe "S=<n> H=<n> V=<n>". Aufruf: probe_r41_masken36_sicht A B C A0 x0 y0 x1 y1 schwelle
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
    int w[4], h[4], x0, y0, x1, y1, s, x, y, k, nS = 0, nH = 0, nV = 0;
    unsigned char *P[4];
    if (argc < 10) { fprintf(stderr, "Aufruf: probe_r41_masken36_sicht A B C A0 x0 y0 x1 y1 schwelle\n"); return 2; }
    for (k = 0; k < 4; k++) {
        P[k] = lies_ppm(argv[1 + k], &w[k], &h[k]);
        if (!P[k] || w[k] != w[0] || h[k] != h[0]) { fprintf(stderr, "PPM fehlt/ungleich: %s\n", argv[1 + k]); return 1; }
    }
    x0 = atoi(argv[5]); y0 = atoi(argv[6]); x1 = atoi(argv[7]); y1 = atoi(argv[8]); s = atoi(argv[9]);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > w[0]) x1 = w[0];
    if (y1 > h[0]) y1 = h[0];
    for (y = y0; y < y1; y++)
        for (x = x0; x < x1; x++) {
            int i = y * w[0] + x;
            if (!diff(P[1], P[2], i, s)) continue;
            nS++;
            if (diff(P[0], P[1], i, s)) nH++;
            if (diff(P[0], P[3], i, s)) nV++;
        }
    printf("S=%d H=%d V=%d\n", nS, nH, nV);
    for (k = 0; k < 4; k++) free(P[k]);
    return 0;
}
