/* probe_r41_schwanz_farbe.c — Auswerter (kein add_test) der Farbregel 8g im Riegel integration_r41_schwanz_zusammen (Runde 41 Schwanz r9 Runde 5).
 *
 * Nutzer 2026-11-15 zur Vorschau der zweibeinigen Birkin: "laeuft es SCHON WIEDER darauf hinaus, das die Oberseite des Schwanzes wieder eher rot
 * als dunkel ist.... Warum? Das war doch vorher gut" / "NUR DIE UNTERSEITE WAR KOMISCH MIT DEN ABSCHNITTEN!!!!". Gilt: Oberseite, Ruecken, Flanken
 * und Nadel dunkel wie Video 28 (Asset 8c d31f9534), nur die Unterseite ueber die Glieder einheitlich = Asset 8g a02a6df3 (r41/sichel_v8,
 * scratchpad schwanz_r9/farbe_8g/BERICHT.md). Ersetzt die Regel (6) "Ton Mitte/Rand im Bogen" (d8ef12963), die 8f (Flanke rosa) belohnte.
 *
 * Eingabe: Framedump-Ordner desselben Szenenlaufs (gleiche exe und Eingabe; das Schwanz-Asset ueber RE15_CD_ROOT ausgetauscht):
 *   N     normal (Spiel-Asset)
 *   T     RE15_SCHWANZ_MASKE=schwanz (Schwanz ohne Flaeche)  -> Schwanzpunkte = N != T
 *   C8    normal mit Asset 8c d31f9534 (Video 28)
 *   MK    normal mit dem Klassen-Markerasset (runde8g/werkzeug/marker.py klasse aus dem Spiel-Asset): jede Schwanzflaeche einfarbig nach Lage
 *         (Bauch rot, Bauch-Fuge magenta, Flanke/Ruecken gruen, Flanken-Fuge cyan, Nadel blau, Wurzel gelb; Kanal an = Wert > 24)
 *   NAHT  normal mit dem Naht-Markerasset (marker.py naht: Texel Seite 0 u 54-55 v 46-60 gruen)
 *
 *   probe_r41_schwanz_farbe <N> <T> <C8> <MK> <NAHT> <F0> <F1> <b_f0> <b_f1> <b_promille_max> <c_max_x10> <naht_max>
 *
 * (b) OBERSEITE WIE 8c: in b_f0..b_f1 (Bogen h74-h100, darin F336 = Nutzerbild 3) die Bildpunkte, die im Markerlauf Flanke/Ruecken/Nadel sind
 *     (samt 8er-Nachbarschaft, also ohne die Klassengrenze) und in N und C8 Schwanz: hoechstens b_promille_max / 1000 davon verschieden.
 * (c) UNTERSEITE OHNE ABSCHNITTE: in F0..F1 alle Bauch- und Bauch-Fugen-Punkte (Markerlauf, ohne Klassengrenze) des Laufs N; Abstand dE76 (CIELab
 *     D65) der Mittel Bauch / Bauch-Fuge hoechstens c_max_x10 / 10 (mindestens 50 Fugenpunkte).
 * (d) NAHT: in F0..F1 Schwanzpunkte, in denen NAHT von N abweicht (= sie zeigen ein Texel u 54-55): hoechstens naht_max.
 * Messung und Grenzen (320x240, F255-F350, exe Runde 5): scratchpad schwanz_r9/r5 (BERICHT_r5.md).
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

static int anders(const bild_t *a, const bild_t *b, size_t i) { return memcmp(a->p + 3 * i, b->p + 3 * i, 3) != 0; }

static double lin(double c) { c /= 255.0; return c <= 0.04045 ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4); }
static double fl(double t) { return t > 0.008856 ? cbrt(t) : 7.787 * t + 16.0 / 116.0; }
static void lab_von(const unsigned char *q, double o[3])
{
    const double r = lin(q[0]), g = lin(q[1]), b = lin(q[2]);
    const double x = (0.412453 * r + 0.357580 * g + 0.180423 * b) / 0.950456, y = 0.212671 * r + 0.715160 * g + 0.072169 * b,
                 z = (0.019334 * r + 0.119193 * g + 0.950227 * b) / 1.088754;
    o[0] = y > 0.008856 ? 116.0 * cbrt(y) - 16.0 : 903.3 * y;
    o[1] = 500.0 * (fl(x) - fl(y));
    o[2] = 200.0 * (fl(y) - fl(z));
}

/* Markerklasse: 0 Bauch, 1 Bauch-Fuge, 2 Flanke, 3 Flanken-Fuge, 4 Nadel, 5 Wurzel, -1 keine */
static int klasse(const unsigned char *q)
{
    const int r = q[0] > 24, g = q[1] > 24, b = q[2] > 24;
    if (r && !g && !b) return 0;
    if (r && !g && b) return 1;
    if (!r && g && !b) return 2;
    if (!r && g && b) return 3;
    if (!r && !g && b) return 4;
    if (r && g && !b) return 5;
    return -1;
}

int main(int argc, char **argv)
{
    if (argc < 13) {
        fprintf(stderr, "Aufruf: %s <N> <T> <C8> <MK> <NAHT> <F0> <F1> <b_f0> <b_f1> <b_promille_max> <c_max_x10> <naht_max>\n", argv[0]);
        return 2;
    }
    const int f0 = atoi(argv[6]), f1 = atoi(argv[7]), b_f0 = atoi(argv[8]), b_f1 = atoi(argv[9]);
    const double b_max = atof(argv[10]) / 1000.0, c_max = atof(argv[11]) / 10.0;
    const long naht_max = atol(argv[12]);
    long b_n = 0, b_d = 0, naht = 0, n0 = 0, n1 = 0;
    double s0[3] = { 0 }, s1[3] = { 0 };
    for (int f = f0; f <= f1; f++) {
        bild_t n = { 0 }, t = { 0 }, mk = { 0 }, na = { 0 }, c8 = { 0 };
        const int mit_b = f >= b_f0 && f <= b_f1;
        if (!lade(argv[1], f, &n) || !lade(argv[2], f, &t) || !lade(argv[4], f, &mk) || !lade(argv[5], f, &na) || (mit_b && !lade(argv[3], f, &c8)) ||
            t.w != n.w || mk.w != n.w || na.w != n.w || t.h != n.h || mk.h != n.h || na.h != n.h || (mit_b && (c8.w != n.w || c8.h != n.h))) {
            printf("F%d: Framedump fehlt oder Groesse verschieden\n", f);
            return 1;
        }
        const int w = n.w, h = n.h;
        const size_t np = (size_t)w * h;
        signed char *kl = (signed char *)malloc(np);
        for (size_t i = 0; i < np; i++) kl[i] = (signed char)(anders(&mk, &t, i) ? klasse(mk.p + 3 * i) : -1);
        long fb_n = 0, fb_d = 0, fn = 0;
        for (int y = 1; y < h - 1; y++)
            for (int x = 1; x < w - 1; x++) {
                const size_t i = (size_t)y * w + x;
                if (!anders(&n, &t, i)) continue;
                if (anders(&na, &n, i)) fn++;
                const int k = kl[i];
                if (k < 0 || k == 5) continue;
                int innen = 1;
                for (int dy = -1; dy <= 1 && innen; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        const int kk = kl[(size_t)(y + dy) * w + x + dx];
                        const int gleich = (k <= 1) ? (kk == k) : (kk >= 2 && kk <= 4);
                        if (!gleich) { innen = 0; break; }
                    }
                if (!innen) continue;
                if (k >= 2) {
                    if (mit_b && anders(&c8, &t, i)) { fb_n++; fb_d += anders(&n, &c8, i); }
                } else {
                    double o[3];
                    lab_von(n.p + 3 * i, o);
                    double *s = k ? s1 : s0;
                    for (int c = 0; c < 3; c++) s[c] += o[c];
                    if (k) n1++; else n0++;
                }
            }
        naht += fn;
        b_n += fb_n; b_d += fb_d;
        if (mit_b || fn) printf("F%d h%d%s: Flanke/Ruecken/Nadel %ld Punkte, anders als 8c %ld; Naht-Punkte %ld\n", f, f - 243, f == 336 ? " (Nutzerbild 3)" : "",
                                fb_n, fb_d, fn);
        free(kl); free(n.p); free(t.p); free(mk.p); free(na.p); free(c8.p);
    }
    int fehler = 0;
    const double anteil = b_n ? (double)b_d / b_n : 1.0;
    const int fb = b_n < 100 || anteil > b_max;
    printf("(b) Oberseite wie 8c F%d-F%d: %ld Punkte, %ld anders (%.2f %%, Grenze %.2f %%) -> %s\n", b_f0, b_f1, b_n, b_d, 100.0 * anteil, 100.0 * b_max,
           fb ? "OBERSEITE-NICHT-8C" : "OBERSEITE-OK");
    fehler += fb;
    double de = 1e9;
    if (n0 && n1) {
        double d2 = 0;
        for (int c = 0; c < 3; c++) { const double d = s0[c] / n0 - s1[c] / n1; d2 += d * d; }
        de = sqrt(d2);
    }
    const int fc = n1 < 50 || n0 < 50 || de > c_max;
    printf("(c) Unterseite F%d-F%d: Bauch %ld Punkte L* %.1f, Bauch-Fuge %ld Punkte L* %.1f, dE76 %.2f (Grenze %.1f) -> %s\n", f0, f1, n0,
           n0 ? s0[0] / n0 : 0.0, n1, n1 ? s1[0] / n1 : 0.0, de, c_max, fc ? "UNTERSEITE-ABSCHNITTE" : "UNTERSEITE-OK");
    fehler += fc;
    const int fd = naht > naht_max;
    printf("(d) Naht u 54-55 F%d-F%d: %ld Schwanzpunkte (Grenze %ld) -> %s\n", f0, f1, naht, naht_max, fd ? "NAHT" : "NAHT-OK");
    fehler += fd;
    return fehler ? 1 : 0;
}
