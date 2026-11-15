/* probe_r41_schwanz_zusammen.c — Auswerter (kein add_test) des Riegels integration_r41_schwanz_zusammen.
 *
 * Nutzer 2026-11-15 (Videos 30_sichel_schwanz_einheitlich, Standbilder birkin_fehler/): "in den Szenen teilweise so aus als taucht der
 * schwanz jetzt garnicht mehr auf, oder ist nicht mehr sauber connected" / "vorher WAR er ja sauber connected". Ursache (Messlauf
 * scratchpad schwanz_r9): die Hangar-Szene zeichnete die Glieder Wurzel -> Video-Austritt mit Querschnitt 0; der Port-Koerper deckt den
 * Video-Austritt nicht ab, das sichtbare Stueck hing h25-h29 frei in der Luft (Abstand zum Koerper bis 104 Punkte in 960x720).
 *
 * Eingabe: drei Framedump-Ordner desselben Szenenlaufs (gleiche exe, gleiche Eingabe, bitgleich bis auf den Messhaken)
 *   N  normal
 *   T  RE15_SCHWANZ_MASKE=schwanz  (Knochen 16-22 ohne Flaeche)
 *   L  RE15_SCHWANZ_MASKE=figuren  (Sichel-Form und Spider-Man ohne Flaeche)
 * Schwanzpunkte = N != T, Figurpunkte = T != L (Birkin ohne Schwanz + Spider-Man: alles, was den Schwanz verdecken darf).
 * Je Bild: Teile des Schwanzes (8er-Nachbarschaft, ab MIN_TEIL Punkten). Ein Teil ohne Figurpunkt im Abstand <= NAH (Schachbrett)
 * haengt frei in der Luft -> Fehler. Ausgabe je Bild eine Zeile, Exit 1 bei einem Fehlerbild oder fehlenden Dateien.
 *
 *   probe_r41_schwanz_zusammen <N> <T> <L> <F0> <F1> [nah=2] [min_teil=3] [<B> <schwanz.log> <knick_max> <vor_f0> <vor_f1> <vor_promille> <vor_min>]
 *
 * Runde 2 (Pruefer r9 Runde 1, scratchpad schwanz_r9/pruefung/PRUEFUNG.md: (1) allein ist ein Pixelnaehe-Mass - es blieb gruen bei einem
 * Knick wie ein Ellbogen h26-h30 und einem Schwanzstueck VOR dem rechten Oberschenkel h58-h60 / h71-h72). Mit den optionalen Argumenten:
 *   B  RE15_SCHWANZ_MASKE=birkin (ganze Sichel-Form ohne Flaeche) -> Koerperpunkte K = T != B (Birkin ohne Schwanz, ohne Spider-Man).
 *   (3) VOR DEM KOERPER: in vor_f0..vor_f1 je Bild V = Schwanzpunkte, die einen Koerperpunkt verdecken (N != T und T != B). Fehler, wenn
 *       V > vor_min UND V > vor_promille / 1000 des sichtbaren Schwanzes.
 *   (4) SICHTBARER KNICK: Gelenke 0-7 im 320x240-Bild aus dem Messhaken RE15_SCHWANZ_LOG ([schwanz-bild]-Zeilen des Laufs N). Ein Gelenk
 *       1..6 zaehlt, wenn BEIDE Glieder daran sichtbar sind: im Bild >= 4 Punkte (320x240) lang und bei 30 % und 70 % ihrer Laenge ein
 *       Schwanzpunkt im Abstand <= 2 (320x240, auf die Bildgroesse umgerechnet). Fehler, wenn der Bildwinkel dort > knick_max Grad.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct { int w, h; unsigned char *p; } bild_t;

static int lade(const char *dir, int f, bild_t *b)
{
    char pfad[1024];
    snprintf(pfad, sizeof pfad, "%s/f%06d.ppm", dir, f);
    FILE *fp = fopen(pfad, "rb");
    if (!fp) return 0;
    int mx = 0;
    if (fscanf(fp, "P6 %d %d %d", &b->w, &b->h, &mx) != 3 || b->w <= 0 || b->h <= 0 || mx != 255) { fclose(fp); return 0; }
    fgetc(fp);
    const size_t n = (size_t)b->w * b->h * 3;
    b->p = (unsigned char *)malloc(n);
    const int ok = b->p && fread(b->p, 1, n, fp) == n;
    fclose(fp);
    return ok;
}

static int anders(const bild_t *a, const bild_t *b, size_t i)
{
    return memcmp(a->p + 3 * i, b->p + 3 * i, 3) != 0;
}

/* [schwanz-bild] F<n> verdeckt <v> 0(x,y,z) ... 7(x,y,z): Gelenke im 320x240-Bild. Rueckgabe 1 = alle 8 gelesen. */
static int gelenke(const char *log, int f, double P[8][2])
{
    FILE *fp = fopen(log, "r");
    if (!fp) return 0;
    char z[2048];
    int ok = 0;
    while (fgets(z, sizeof z, fp)) {
        int ff = -1;
        if (sscanf(z, "[schwanz-bild] F%d", &ff) != 1 || ff != f) continue;
        const char *q = strstr(z, " verdeckt ");
        int n = 0;
        for (int j = 0; j < 8 && q; j++) {
            char m[8];
            snprintf(m, sizeof m, " %d(", j);
            q = strstr(q, m);
            if (!q || sscanf(q + strlen(m), "%lf,%lf", &P[j][0], &P[j][1]) != 2) break;
            n++;
            q += strlen(m);
        }
        ok = (n == 8);
    }
    fclose(fp);
    return ok;
}

int main(int argc, char **argv)
{
    if (argc < 6) { fprintf(stderr, "Aufruf: %s <N> <T> <L> <F0> <F1> [nah] [min_teil]\n", argv[0]); return 2; }
    const int f0 = atoi(argv[4]), f1 = atoi(argv[5]);
    const int nah = argc > 6 ? atoi(argv[6]) : 2, min_teil = argc > 7 ? atoi(argv[7]) : 3;
    const int extra = argc > 14;
    const char *bdir = extra ? argv[8] : NULL, *slog = extra ? argv[9] : NULL;
    const double knick_max = extra ? atof(argv[10]) : 0.0;
    const int vor_f0 = extra ? atoi(argv[11]) : 0, vor_f1 = extra ? atoi(argv[12]) : -1;
    const int vor_promille = extra ? atoi(argv[13]) : 0, vor_min = extra ? atoi(argv[14]) : 0;
    int fehler = 0, bilder = 0, mit_schwanz = 0, fehler_vor = 0, fehler_knick = 0;
    double knick_groesst = 0.0;
    int vor_groesst = 0;
    for (int f = f0; f <= f1; f++) {
        bild_t n = { 0 }, t = { 0 }, l = { 0 };
        if (!lade(argv[1], f, &n) || !lade(argv[2], f, &t) || !lade(argv[3], f, &l) || n.w != t.w || n.w != l.w || n.h != t.h || n.h != l.h) {
            printf("F%d: Framedump fehlt oder Groesse verschieden\n", f);
            free(n.p); free(t.p); free(l.p);
            return 1;
        }
        const int w = n.w, h = n.h;
        const size_t np = (size_t)w * h;
        unsigned char *sw = (unsigned char *)calloc(np, 1), *fig = (unsigned char *)calloc(np, 1);
        int *lab = (int *)calloc(np, sizeof(int)), *stapel = (int *)malloc(np * sizeof(int));
        int sw_n = 0;
        for (size_t i = 0; i < np; i++) {
            sw[i] = (unsigned char)anders(&n, &t, i);
            fig[i] = (unsigned char)anders(&t, &l, i);
            sw_n += sw[i];
        }
        int teile = 0, frei_teile = 0, frei_pkt = 0, frei_x = -1, frei_y = -1;
        for (size_t s0 = 0; s0 < np; s0++) {
            if (!sw[s0] || lab[s0]) continue;
            int sp = 0, groesse = 0, kontakt = 0, bx = (int)(s0 % (size_t)w), by = (int)(s0 / (size_t)w);
            lab[s0] = 1; stapel[sp++] = (int)s0;
            while (sp > 0) {
                const int i = stapel[--sp], x = i % w, y = i / w;
                groesse++;
                for (int dy = -nah; dy <= nah && !kontakt; dy++)
                    for (int dx = -nah; dx <= nah; dx++) {
                        const int xx = x + dx, yy = y + dy;
                        if (xx >= 0 && xx < w && yy >= 0 && yy < h && fig[(size_t)yy * w + xx]) { kontakt = 1; break; }
                    }
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        const int xx = x + dx, yy = y + dy;
                        if (xx < 0 || xx >= w || yy < 0 || yy >= h) continue;
                        const size_t j = (size_t)yy * w + xx;
                        if (sw[j] && !lab[j]) { lab[j] = 1; stapel[sp++] = (int)j; }
                    }
            }
            if (groesse < min_teil) continue;
            teile++;
            if (!kontakt) { frei_teile++; frei_pkt += groesse; frei_x = bx; frei_y = by; }
        }
        bilder++;
        if (sw_n > 0) mit_schwanz++;
        printf("F%d Schwanz %d Teile %d frei %d (%d Punkte)%s", f, sw_n, teile, frei_teile, frei_pkt, frei_teile ? " FEHLER" : "");
        if (frei_teile) printf(" erster freier Teil bei (%d,%d)", frei_x, frei_y);
        printf("\n");
        if (frei_teile) fehler++;
        if (extra) {
            bild_t bb = { 0 };
            double P[8][2];
            if (!lade(bdir, f, &bb) || bb.w != w || bb.h != h || !gelenke(slog, f, P)) {
                printf("F%d: Framedump B oder [schwanz-bild]-Zeile fehlt (%s, %s)\n", f, bdir, slog);
                free(bb.p); free(sw); free(fig); free(lab); free(stapel); free(n.p); free(t.p); free(l.p);
                return 1;
            }
            int vor = 0;
            for (size_t i = 0; i < np; i++) vor += sw[i] && anders(&t, &bb, i);
            if (f >= vor_f0 && f <= vor_f1) {
                if (vor > vor_groesst) vor_groesst = vor;
                if (vor > vor_min && (long)vor * 1000 > (long)vor_promille * sw_n) {
                    printf("F%d (3) VOR DEM KOERPER: %d von %d Schwanzpunkten verdecken die Birkin FEHLER\n", f, vor, sw_n);
                    fehler_vor++;
                }
            }
            const double sx = w / 320.0, sy = h / 240.0;
            const int r = (int)(2.0 * sx + 0.5);
            int sicht[7];
            for (int g = 0; g < 7; g++) {
                const double dx = P[g + 1][0] - P[g][0], dy = P[g + 1][1] - P[g][1];
                sicht[g] = sqrt(dx * dx + dy * dy) >= 4.0;
                for (int k = 0; k < 2 && sicht[g]; k++) {
                    const double tt = k ? 0.7 : 0.3;
                    const int cx = (int)lround((P[g][0] + dx * tt) * sx), cy = (int)lround((P[g][1] + dy * tt) * sy);
                    int da = 0;
                    for (int yy = cy - r; yy <= cy + r && !da; yy++)
                        for (int xx = cx - r; xx <= cx + r; xx++)
                            if (xx >= 0 && xx < w && yy >= 0 && yy < h && sw[(size_t)yy * w + xx]) { da = 1; break; }
                    sicht[g] = da;
                }
            }
            for (int j = 1; j <= 6; j++) {
                if (!sicht[j - 1] || !sicht[j]) continue;
                const double ax = P[j][0] - P[j - 1][0], ay = P[j][1] - P[j - 1][1], cx = P[j + 1][0] - P[j][0], cy = P[j + 1][1] - P[j][1];
                double c = (ax * cx + ay * cy) / (sqrt(ax * ax + ay * ay) * sqrt(cx * cx + cy * cy));
                c = c > 1.0 ? 1.0 : (c < -1.0 ? -1.0 : c);
                const double wk = acos(c) * 180.0 / 3.14159265358979;
                if (wk > knick_groesst) knick_groesst = wk;
                if (wk > knick_max) {
                    printf("F%d (4) SICHTBARER KNICK Gelenk %d: %.0f Grad > %.0f FEHLER\n", f, j, wk, knick_max);
                    fehler_knick++;
                }
            }
            free(bb.p);
        }
        free(sw); free(fig); free(lab); free(stapel); free(n.p); free(t.p); free(l.p);
    }
    printf("Bilder %d, mit Schwanz %d, mit frei haengendem Schwanzteil %d -> %s\n", bilder, mit_schwanz, fehler,
           fehler ? "SCHWANZ-NICHT-ZUSAMMEN" : "SCHWANZ-ZUSAMMEN-OK");
    if (extra)
        printf("(3) vor dem Koerper F%d-F%d: %d Fehlerbilder (groesster Wert %d Punkte) -> %s\n"
               "(4) sichtbarer Knick: %d Fehlergelenke (groesster %.0f Grad, Grenze %.0f) -> %s\n",
               vor_f0, vor_f1, fehler_vor, vor_groesst, fehler_vor ? "SCHWANZ-VOR-KOERPER" : "VOR-OK",
               fehler_knick, knick_groesst, knick_max, fehler_knick ? "SCHWANZ-KNICK" : "KNICK-OK");
    return (fehler || fehler_vor || fehler_knick) ? 1 : 0;
}
