/* probe_r36_elza_auswertung.c — Runde 36 Spur L: AUSWERTE-WERKZEUG der Integrationshaken (kein add_test).
 *
 *   bild <leon.ppm> <elza.ppm>
 *       Vergleicht zwei Framedumps (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, 320x240) an den 309 Pixeln der
 *       Schadens-Ueberlagerung (gen/fenster1120_schaden.inc): erwartet wird ELZA = LEON (+) Op, also genau die Formel
 *       von re15_fenster1120_pc_zeichnen (glas1120_pc.c). Ausgabe "ops N formel M verschieden D".
 *   mitschnitt <cap.raw> <exit-bild> <bild> <laenge> [<bild> <laenge> ...]
 *       RE15_AUDIO_CAP_SYNC-Mitschnitt (Stereo s16, 1470 Frames je Spielbild, Offset = Bild * 1470): Raum-Bild f
 *       liegt bei Mitschnitt-Takt (Takte - 1 - exit) + f. Je Fenster: groesste Spitze (Mono-Mittel) und der Median
 *       der Spitzen aller Raum-Takte. Ausgabe je Fenster "fenster B..B+L spitze S median M".
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { int16_t x, y; uint8_t art, a, r, g, b; } op_t;
static const op_t k_ops[] = {
#include "../../engine/src/gen/fenster1120_schaden.inc"
};

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

static int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

int main(int argc, char **argv)
{
    if (argc >= 4 && !strcmp(argv[1], "bild")) {
        int w1, h1, w2, h2;
        unsigned char *a = ppm(argv[2], &w1, &h1), *b = ppm(argv[3], &w2, &h2);
        if (!a || !b || w1 != w2 || h1 != h2) { printf("FEHLER: Bilder\n"); return 1; }
        int n = (int)(sizeof k_ops / sizeof k_ops[0]), formel = 0, versch = 0;
        for (int i = 0; i < n; i++) {
            const op_t *o = &k_ops[i];
            if (o->x < 0 || o->x >= w1 || o->y < 0 || o->y >= h1) continue;
            const unsigned char *pa = a + ((size_t)o->y * w1 + o->x) * 3, *pb = b + ((size_t)o->y * w1 + o->x) * 3;
            int r = pa[0], g = pa[1], bb = pa[2], er, eg, eb;
            if (o->art == 0) { er = r * o->a / 256; eg = g * o->a / 256; eb = bb * o->a / 256; }
            else { er = r + ((int)o->r - r) * o->a / 256; eg = g + ((int)o->g - g) * o->a / 256; eb = bb + ((int)o->b - bb) * o->a / 256; }
            if (abs(pb[0] - er) <= 2 && abs(pb[1] - eg) <= 2 && abs(pb[2] - eb) <= 2) formel++;
            if (pb[0] != pa[0] || pb[1] != pa[1] || pb[2] != pa[2]) versch++;
        }
        printf("ops %d formel %d verschieden %d\n", n, formel, versch);
        return 0;
    }
    if (argc >= 6 && !strcmp(argv[1], "mitschnitt")) {
        FILE *f = fopen(argv[2], "rb"); if (!f) { printf("FEHLER: %s\n", argv[2]); return 1; }
        fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
        int16_t *s = (int16_t *)malloc((size_t)sz);
        if (!s || fread(s, 1, (size_t)sz, f) != (size_t)sz) { printf("FEHLER: lesen\n"); return 1; }
        fclose(f);
        const long T = 1470; long ticks = sz / 4 / T;
        int exitb = atoi(argv[3]);
        long off = ticks - 1 - exitb;
        if (off < 0) { printf("FEHLER: Mitschnitt kuerzer als der Raum\n"); return 1; }
        int *pk = (int *)malloc(sizeof(int) * (size_t)ticks);
        for (long t = 0; t < ticks; t++) {
            int m = 0;
            for (long i = 0; i < T; i++) {
                int v = (s[(t * T + i) * 2] + s[(t * T + i) * 2 + 1]) / 2; if (v < 0) v = -v; if (v > m) m = v;
            }
            pk[t] = m;
        }
        int nr = (int)(ticks - off); int *srt = (int *)malloc(sizeof(int) * (size_t)nr);
        memcpy(srt, pk + off, sizeof(int) * (size_t)nr); qsort(srt, (size_t)nr, sizeof(int), cmp_int);
        int median = srt[nr / 2];
        for (int a = 4; a + 1 < argc; a += 2) {
            int b0 = atoi(argv[a]), l = atoi(argv[a + 1]), mx = 0, at = -1;
            for (int k = 0; k < l; k++) { long t = off + b0 + k; if (t >= 0 && t < ticks && pk[t] > mx) { mx = pk[t]; at = b0 + k; } }
            printf("fenster %d..%d spitze %d bei %d median %d\n", b0, b0 + l - 1, mx, at, median);
        }
        return 0;
    }
    printf("Aufruf: bild <a.ppm> <b.ppm> | mitschnitt <cap.raw> <exit-bild> <bild> <laenge> ...\n");
    return 2;
}
