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
 *   probe_r41_schwanz_zusammen <N> <T> <L> <F0> <F1> [nah=2] [min_teil=3]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv)
{
    if (argc < 6) { fprintf(stderr, "Aufruf: %s <N> <T> <L> <F0> <F1> [nah] [min_teil]\n", argv[0]); return 2; }
    const int f0 = atoi(argv[4]), f1 = atoi(argv[5]);
    const int nah = argc > 6 ? atoi(argv[6]) : 2, min_teil = argc > 7 ? atoi(argv[7]) : 3;
    int fehler = 0, bilder = 0, mit_schwanz = 0;
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
        free(sw); free(fig); free(lab); free(stapel); free(n.p); free(t.p); free(l.p);
    }
    printf("Bilder %d, mit Schwanz %d, mit frei haengendem Schwanzteil %d -> %s\n", bilder, mit_schwanz, fehler,
           fehler ? "SCHWANZ-NICHT-ZUSAMMEN" : "SCHWANZ-ZUSAMMEN-OK");
    return fehler ? 1 : 0;
}
