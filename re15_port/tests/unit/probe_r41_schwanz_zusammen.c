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
 *
 * Runde 3 (Pruefer r9 Runde 2, scratchpad schwanz_r9/pruefung_r2/PRUEFUNG.md: "rosa Wurzelstueck an der rechten Flanke bis ueber den Guertel"
 * h59-h70, der Schwanz kommt seitlich aus der Taille statt in Hueft-Hoehe hinter der Huefte hervor; (3) sah es nicht, weil der Streifen
 * NEBEN der Silhouette lag; dazu Entscheid des Orchestrators zu "braune Streifen an den Raendern": der Schwanz einheitlich ueber die Breite).
 * Weitere optionale Argumente:  <taille_f0> <taille_f1> <ueber_max> [<ton_f0> <ton_f1> <ton_max_x10>]
 *   (5) UEBER DER AUSTRITTSZEILE: in taille_f0..taille_f1 (Takt h = F - 243, nur h58-h72) je Bild die Schwanzpunkte ueber der Zeile
 *       y_aus(h) - 4,5 (320x240, auf die Bildgroesse umgerechnet), auch neben der Silhouette. y_aus = Austritt des Original-Videos
 *       (k_aus_video: Videozeile 640x480 je geradem h, analysis/befunde_runde41/SF_belege/v8/bewegung/tabelle_v8.json "austritt", ungerade h
 *       linear), Port-Zeile (vy - 1,3) / 2,079 (Registrierung bf_video_welt); 4,5 = halbe Schwanzbreite (Median 9 Punkte der sichtbaren
 *       Zeilen h58-h70, Lauf 4f2b7a354): so weit reicht der Rand eines schraeg hervorkommenden Schwanzes ueber seine Mittellinie. Fehler bei
 *       mehr als ueber_max Punkten (320x240-Punkte; Messung scratchpad schwanz_r9/r3: 4f2b7a354 38-190 je Bild h58-h70, Runde 3 hoechstens 3).
 *   (6) TON MITTE/RAND: in ton_f0..ton_f1 (Bogen) Querprofile senkrecht zu jedem Glied (Gelenke aus schwanz.log, Bildlaenge >= 5 Punkte
 *       320x240) bei t 0,2..0,8: Halbbreite je Seite bis zum Rand der Schwanzmaske (beide >= 3), Mitte = Versatz |s| <= 0,2 w (5 Proben),
 *       Rand je Seite 0,6 w .. 0,9 w (4 Proben). sRGB -> Lab (D65). Je Bild dEab = groesster Abstand in a*b* (ohne L* = Schattierung) der
 *       Mittel Mitte / Rand einer Seite. Fehler, wenn dEab > ton_max_x10 / 10 oder weniger als 10 Bilder messbar.
 *
 * Runde 4 (Pruefer r9 Runde 3, scratchpad schwanz_r9/pruefung_r3/PRUEFUNG.md: h58-h61 / h71 Schwanz fast unsichtbar (13-58 bzw. 0 Punkte
 * 320x240), h15/h16 rosa Wurzelstueck an der linken Taille bis 54 Zeilen (960x720) ueber dem Guertel, h17 / h29-h35 Stueck vor Schritt und
 * Oberschenkel - kein Riegel deckte es ab). Weitere optionale Argumente:  <taille2_f0> <taille2_f1> <sicht_f0> <sicht_f1> <sicht_min>
 *   (5b) TAILLE LINKS: in taille2_f0..taille2_f1 je Bild hoechstens ueber_max Schwanzpunkte NEBEN der Silhouette ueber dem Guertel (oberste
 *       Jeanszeile der Birkin) rechts der Koerpermitte (Bild rechts = Birkins linke Taille). Original h16-h24: der Schwanz kommt links aus der
 *       Koerpermitte (Bild links); rechts zwischen Rumpf und Arm ist keiner.
 *   (7) MINDEST-SICHTBARKEIT: in sicht_f0..sicht_f1 je Bild mindestens sicht_min Schwanzpunkte (320x240, umgerechnet).
 *   (3) faehrt der Riegel seit Runde 4 ueber F258-F316 (h15-h73, vorher F301-F316).
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

/* (5) Austritt des Original-Videos (Videozeile 640x480) h58, h60, ..., h72 (tabelle_v8.json "austritt" y) */
static const int k_aus_video[8] = { 322, 320, 325, 320, 320, 322, 322, 338 };

static double aus_zeile(int h)                       /* Port-Zeile 320x240 */
{
    if (h < 58) h = 58;
    if (h > 72) h = 72;
    const int i = (h - 58) / 2, i1 = i < 7 ? i + 1 : i;
    const double vy = k_aus_video[i] + (k_aus_video[i1] - k_aus_video[i]) * ((h - 58) % 2) / 2.0;
    return (vy - 1.3) / 2.079;
}

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

/* (6) Tonunterschied Mitte/Rand eines Bilds (dEab, siehe Kopf); -1 = zu wenig Proben */
static double ton_bild(const bild_t *n, const unsigned char *sw, const double P[8][2])
{
    const int w = n->w, h = n->h;
    const double sx = w / 320.0, sy = h / 240.0;
    double sm[3] = { 0 }, sp[3] = { 0 }, sn[3] = { 0 }, o[3];
    int cm = 0, cp = 0, cn = 0;
    for (int g = 0; g < 7; g++) {
        const double ax = P[g][0] * sx, ay = P[g][1] * sy, dx = P[g + 1][0] * sx - ax, dy = P[g + 1][1] * sy - ay, L = sqrt(dx * dx + dy * dy);
        if (L < 5.0 * sx) continue;
        const double nx = -dy / L, ny = dx / L;
        for (int k = 2; k <= 8; k++) {
            const double cx = ax + dx * k / 10.0, cy = ay + dy * k / 10.0;
            int ws[2];
            for (int sg = 0; sg < 2; sg++) {
                const double sv = sg ? -1.0 : 1.0;
                int m = 0;
                while (m < 60) {
                    const int x = (int)lround(cx + sv * nx * (m + 1)), y = (int)lround(cy + sv * ny * (m + 1));
                    if (x < 0 || x >= w || y < 0 || y >= h || !sw[(size_t)y * w + x]) break;
                    m++;
                }
                ws[sg] = m;
            }
            const int wm = ws[0] < ws[1] ? ws[0] : ws[1];
            if (wm < (int)lround(3.0 * sx)) continue;
            const int x0 = (int)lround(cx), y0 = (int)lround(cy);
            if (x0 < 0 || x0 >= w || y0 < 0 || y0 >= h || !sw[(size_t)y0 * w + x0]) continue;
            for (int sg = 0; sg < 2; sg++) {
                const double sv = sg ? -1.0 : 1.0;
                for (int q = 0; q < 4; q++) {
                    const double s = ws[sg] * (0.6 + 0.1 * q);
                    const int x = (int)lround(cx + sv * nx * s), y = (int)lround(cy + sv * ny * s);
                    if (x < 0 || x >= w || y < 0 || y >= h) continue;
                    lab_von(n->p + 3 * ((size_t)y * w + x), o);
                    double *acc = sg ? sn : sp;
                    for (int c = 0; c < 3; c++) acc[c] += o[c];
                    if (sg) cn++; else cp++;
                }
            }
            for (int q = 0; q < 5; q++) {
                const double s = -0.2 * wm + 0.1 * wm * q;
                const int x = (int)lround(cx + nx * s), y = (int)lround(cy + ny * s);
                if (x < 0 || x >= w || y < 0 || y >= h) continue;
                lab_von(n->p + 3 * ((size_t)y * w + x), o);
                for (int c = 0; c < 3; c++) sm[c] += o[c];
                cm++;
            }
        }
    }
    if (cm < 10 || cp < 10 || cn < 10) return -1.0;
    double best = 0.0;
    for (int sg = 0; sg < 2; sg++) {
        const double *acc = sg ? sn : sp;
        const int c_ = sg ? cn : cp;
        const double da = acc[1] / c_ - sm[1] / cm, db = acc[2] / c_ - sm[2] / cm, d = sqrt(da * da + db * db);
        if (d > best) best = d;
    }
    return best;
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
    const int r3 = extra && argc > 17;
    const int t_f0 = r3 ? atoi(argv[15]) : 0, t_f1 = r3 ? atoi(argv[16]) : -1, ueber_max = r3 ? atoi(argv[17]) : 0;
    const int ton = r3 && argc > 20;
    const int n_f0 = ton ? atoi(argv[18]) : 0, n_f1 = ton ? atoi(argv[19]) : -1;
    const double ton_max = ton ? atof(argv[20]) / 10.0 : 0.0;
    const int r4 = ton && argc > 25;
    const int u_f0 = r4 ? atoi(argv[21]) : 0, u_f1 = r4 ? atoi(argv[22]) : -1;
    const int s_f0 = r4 ? atoi(argv[23]) : 0, s_f1 = r4 ? atoi(argv[24]) : -1, sicht_min = r4 ? atoi(argv[25]) : 0;
    int fehler_sicht = 0, sicht_kleinst = 1 << 30, fehler_5b = 0, groesst_5b = 0;
    int fehler_ueber = 0, ueber_groesst = 0, fehler_ton = 0, ton_bilder = 0;
    double ton_groesst = 0.0;
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
            if (r4 && f >= u_f0 && f <= u_f1) {
                /* (5b) Runde 4: Guertel = oberste Jeanszeile der Birkin (Lauf T ohne Schwanz, Koerper K = T != B, Jeans blau: b > r + 15,
                 * b > g + 5, b > 60; Zeile mit >= 8 Punkten (320x240, umgerechnet), 2-%-Zeile), Mitte = mittleres x der Jeanspunkte.
                 * Gezaehlt: Schwanzpunkte NEBEN der Silhouette (N != T, T == B) ueber dem Guertel rechts der Mitte (Birkins linke Taille). */
                int *zj = (int *)calloc((size_t)h, sizeof(int));
                long sx_ = 0, nj = 0;
                for (int yy = 0; yy < h; yy++)
                    for (int xx = 0; xx < w; xx++) {
                        const size_t i = (size_t)yy * w + xx;
                        const unsigned char *q = t.p + 3 * i;
                        if (anders(&t, &bb, i) && q[2] > q[0] + 15 && q[2] > q[1] + 5 && q[2] > 60) { zj[yy]++; sx_ += xx; nj++; }
                    }
                const int min_z = (int)lround(8.0 * sx);
                int zeilen = 0, gz = -1;
                for (int yy = 0; yy < h; yy++) zeilen += zj[yy] >= min_z;
                for (int yy = 0, k = 0; yy < h && gz < 0; yy++)
                    if (zj[yy] >= min_z && ++k > zeilen / 50) gz = yy;
                free(zj);
                if (gz >= 0 && nj > 0) {
                    const int cx = (int)(sx_ / nj);
                    int ub = 0;
                    for (int yy = 0; yy < gz; yy++)
                        for (int xx = cx; xx < w; xx++) {
                            const size_t i = (size_t)yy * w + xx;
                            ub += sw[i] && !anders(&t, &bb, i);
                        }
                    const int ub320 = (int)lround(ub / (sx * sy));
                    if (ub320 > groesst_5b) groesst_5b = ub320;
                    if (ub320 > ueber_max) {
                        printf("F%d (5b) TAILLE LINKS h%d: %d Schwanzpunkte (320x240) neben der Silhouette ueber dem Guertel (Zeile %.1f) rechts der Mitte FEHLER\n",
                               f, f - 243, ub320, gz / sy);
                        fehler_5b++;
                    }
                }
            }
            if (r3 && f >= t_f0 && f <= t_f1 && f - 243 >= 58 && f - 243 <= 72) {
                const int ymax = (int)ceil((aus_zeile(f - 243) - 4.5) * sy);      /* Zeilen y < ymax liegen ueber der Grenze */
                int ub = 0;
                for (int yy = 0; yy < ymax && yy < h; yy++)
                    for (int xx = 0; xx < w; xx++) ub += sw[(size_t)yy * w + xx];
                const int ub320 = (int)lround(ub / (sx * sy));
                if (ub320 > ueber_groesst) ueber_groesst = ub320;
                if (ub320 > ueber_max) {
                    printf("F%d (5) UEBER DER AUSTRITTSZEILE h%d: %d Schwanzpunkte (320x240) ueber Zeile %.1f FEHLER\n", f, f - 243, ub320,
                           aus_zeile(f - 243) - 4.5);
                    fehler_ueber++;
                }
            }
            if (r4 && f >= s_f0 && f <= s_f1) {               /* (7) Mindest-Sichtbarkeit (Runde 4) */
                const int s320 = (int)lround(sw_n / (sx * sy));
                if (s320 < sicht_kleinst) sicht_kleinst = s320;
                if (s320 < sicht_min) {
                    printf("F%d (7) SCHWANZ KAUM SICHTBAR h%d: %d Schwanzpunkte (320x240) < %d FEHLER\n", f, f - 243, s320, sicht_min);
                    fehler_sicht++;
                }
            }
            if (ton && f >= n_f0 && f <= n_f1) {
                const double d = ton_bild(&n, sw, P);
                if (d >= 0.0) {
                    ton_bilder++;
                    if (d > ton_groesst) ton_groesst = d;
                    printf("F%d (6) Ton Mitte/Rand dEab %.1f%s\n", f, d, d > ton_max ? " FEHLER" : "");
                    if (d > ton_max) fehler_ton++;
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
    if (r3)
        printf("(5) ueber der Austrittszeile F%d-F%d (h58-h72): %d Fehlerbilder (groesster Wert %d Punkte, Grenze %d) -> %s\n", t_f0, t_f1,
               fehler_ueber, ueber_groesst, ueber_max, fehler_ueber ? "SCHWANZ-UEBER-TAILLE" : "TAILLE-OK");
    if (ton)
        printf("(6) Ton Mitte/Rand F%d-F%d: %d Bilder gemessen, %d Fehlerbilder (groesster dEab %.1f, Grenze %.1f) -> %s\n", n_f0, n_f1,
               ton_bilder, fehler_ton, ton_groesst, ton_max, (fehler_ton || ton_bilder < 10) ? "SCHWANZ-RANDTON" : "TON-OK");
    if (r4)
        printf("(5b) Taille links F%d-F%d: %d Fehlerbilder (groesster Wert %d Punkte, Grenze %d) -> %s; (7) Sichtbarkeit F%d-F%d: %d Fehlerbilder (kleinster Wert %d Punkte, Grenze %d) -> %s\n", u_f0, u_f1, fehler_5b, groesst_5b, ueber_max,
               fehler_5b ? "SCHWANZ-TAILLE-LINKS" : "TAILLE-LINKS-OK", s_f0, s_f1, fehler_sicht, sicht_kleinst, sicht_min, fehler_sicht ? "SCHWANZ-UNSICHTBAR" : "SICHT-OK");
    return (fehler || fehler_vor || fehler_knick || fehler_ueber || fehler_5b || fehler_sicht || (ton && (fehler_ton || ton_bilder < 10))) ? 1 : 0;
}
