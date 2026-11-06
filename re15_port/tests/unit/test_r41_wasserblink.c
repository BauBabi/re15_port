/* test_r41_wasserblink.c — Runde 41 Spur W "wasserblink": Mess-Werkzeug fuer den Riegel integration_r41_wasserblink_*
 * (Dossier analysis/befunde_runde41/W_wasserblink.md). Liest RE15_FRAMEDUMP-Bilder (P6) der echten exe.
 *
 *   flacker <x0> <x1> <y0> <y1> <schwelle> <max_wechsel> <bild.ppm>...
 *       zaehlt die Bildwechsel, in denen ein Bildpunkt des Fensters in einem Kanal um mehr als <schwelle> springt.
 *       Ziel: Rueckgabe 0, wenn die Zahl <= <max_wechsel>. (Vorher: die Glanzpunkte aus raumlicht.c Fall 0x2000
 *       leuchteten in 46 von 250 Bildwechseln je 4 Bilder auf, Spruenge +43..+76.)
 *   gleich <x0> <x1> <y0> <y1> <max_diff> <a.ppm> <b.ppm>
 *       groesster Kanalunterschied im Fenster zwischen zwei Bildern; Rueckgabe 0, wenn <= <max_diff>.
 *   rot <x0> <x1> <y0> <y1> <min_zuwachs> <mit.ppm> <ohne.ppm>
 *       mittlerer Rot-Zuwachs (mit - ohne) im Fenster; Rueckgabe 0, wenn >= <min_zuwachs> (Gegenprobe: das
 *       Raumlicht in 50B0 zeichnet noch, der Messweg sieht es). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;

static int lesen(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    b->px = NULL;
    if (!f) { printf("ppm: %s nicht lesbar\n", pfad); return -1; }
    char magic[3] = {0}; int maxv = 0;
    if (fscanf(f, "%2s %d %d %d", magic, &b->w, &b->h, &maxv) != 4 || strcmp(magic, "P6") != 0 || b->w <= 0 || b->h <= 0) {
        fclose(f); printf("ppm: %s kein P6\n", pfad); return -1;
    }
    fgetc(f);
    size_t n = (size_t)b->w * (size_t)b->h * 3;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { fclose(f); free(b->px); b->px = NULL; printf("ppm: %s kurz\n", pfad); return -1; }
    fclose(f);
    return 0;
}

static int max_diff(const bild_t *a, const bild_t *b, int x0, int x1, int y0, int y1, int *mx, int *my)
{
    int m = 0;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || y < 0 || x >= a->w || y >= a->h || x >= b->w || y >= b->h) continue;
            const unsigned char *p = a->px + ((size_t)y * (size_t)a->w + (size_t)x) * 3;
            const unsigned char *q = b->px + ((size_t)y * (size_t)b->w + (size_t)x) * 3;
            for (int c = 0; c < 3; c++) {
                int d = abs((int)p[c] - (int)q[c]);
                if (d > m) { m = d; if (mx) *mx = x; if (my) *my = y; }
            }
        }
    return m;
}

int main(int argc, char **argv)
{
    if (argc >= 9 && !strcmp(argv[1], "flacker")) {
        int x0 = atoi(argv[2]), x1 = atoi(argv[3]), y0 = atoi(argv[4]), y1 = atoi(argv[5]);
        int schwelle = atoi(argv[6]), max_wechsel = atoi(argv[7]);
        bild_t vor = {0, 0, NULL}, jetzt;
        int wechsel = 0, paare = 0, groesst = 0;
        for (int i = 8; i < argc; i++) {
            if (lesen(argv[i], &jetzt) != 0) { free(vor.px); return 1; }
            if (vor.px) {
                int mx = -1, my = -1;
                int d = max_diff(&vor, &jetzt, x0, x1, y0, y1, &mx, &my);
                paare++;
                if (d > groesst) groesst = d;
                if (d > schwelle) { wechsel++; printf("  Wechsel %s: %d bei (%d,%d)\n", argv[i], d, mx, my); }
                free(vor.px);
            }
            vor = jetzt;
        }
        free(vor.px);
        printf("flacker: %d Bilder, %d von %d Wechseln > %d (groesster Sprung %d), erlaubt %d\n",
               argc - 8, wechsel, paare, schwelle, groesst, max_wechsel);
        return (paare > 0 && wechsel <= max_wechsel) ? 0 : 1;
    }
    if (argc == 9 && !strcmp(argv[1], "gleich")) {
        int x0 = atoi(argv[2]), x1 = atoi(argv[3]), y0 = atoi(argv[4]), y1 = atoi(argv[5]), erlaubt = atoi(argv[6]);
        bild_t a, b;
        if (lesen(argv[7], &a) != 0) return 1;
        if (lesen(argv[8], &b) != 0) { free(a.px); return 1; }
        int mx = -1, my = -1;
        int d = max_diff(&a, &b, x0, x1, y0, y1, &mx, &my);
        printf("gleich: groesster Unterschied %d bei (%d,%d), erlaubt %d\n", d, mx, my, erlaubt);
        free(a.px); free(b.px);
        return d <= erlaubt ? 0 : 1;
    }
    if (argc == 9 && !strcmp(argv[1], "rot")) {
        int x0 = atoi(argv[2]), x1 = atoi(argv[3]), y0 = atoi(argv[4]), y1 = atoi(argv[5]);
        double min_zuwachs = atof(argv[6]);
        bild_t a, b;
        if (lesen(argv[7], &a) != 0) return 1;
        if (lesen(argv[8], &b) != 0) { free(a.px); return 1; }
        double acc = 0.0; long n = 0;
        for (int y = y0; y <= y1; y++)
            for (int x = x0; x <= x1; x++) {
                if (x < 0 || y < 0 || x >= a.w || y >= a.h || x >= b.w || y >= b.h) continue;
                acc += (double)a.px[((size_t)y * (size_t)a.w + (size_t)x) * 3] - (double)b.px[((size_t)y * (size_t)b.w + (size_t)x) * 3];
                n++;
            }
        free(a.px); free(b.px);
        double z = n ? acc / (double)n : 0.0;
        printf("rot: mittlerer Rot-Zuwachs %.2f ueber %ld Bildpunkte, verlangt >= %.2f\n", z, n, min_zuwachs);
        return (n > 0 && z >= min_zuwachs) ? 0 : 1;
    }
    printf("Aufruf: flacker|gleich|rot ... (Kopf der Datei)\n");
    return 2;
}
