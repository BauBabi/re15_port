/* probe_r30_irons_tisch_licht.c — AUSWERTER (kein add_test) fuer den Licht-Riegel
 * integration_r30_irons_tisch_licht (Runde 30 Nachschliff, Spur tischlicht).
 *
 * BEFUND: Irons Diary (obj 5) und Memory Card (obj 6) waren in der Nahaufnahme Cut 6 fast
 * schwarz, Mittel-RGB (17,16,8) / (17,17,10), auf hellem gemaltem Tisch. Ursache: der
 * ausgelieferte Lichtsatz von Cut 6 (ROOM1150.RDT @0x00488, ambient 40,40,24). Der Port nimmt
 * fuer genau diese zwei Props jetzt den Lichtsatz von Cut 2 (@0x003E8) - PORT-WAHL, keine
 * Original-Adresse; Messtabelle in include/re15_irons_tisch.h.
 *
 * DIESES WERKZEUG vergleicht vier Framedumps (RE15_FRAMEDUMP = Readback des fertig
 * komponierten Frames vor SDL_RenderPresent, beschleunigter Renderer):
 *   P6  Cut 6, Buch und Karte da    P60 Cut 6, beides genommen (Nullbild)
 *   P2  Cut 2, Buch und Karte da    P20 Cut 2, beides genommen
 * Prop-Pixel = P != P0; Buch und Karte werden an der Mitte der gemessenen Luecke zwischen
 * ihren Huellen getrennt (Einzel-Laeufe r30_tl_lauf.sh, 960er-Spalten: Cut 6 Karte 445..504,
 * Buch 547..639 -> Grenze 525; Cut 2 Karte 409..429, Buch 445..477 -> Grenze 437).
 * ZIELMASS = die GEMALTE Umgebung im Nullbild desselben Cuts: Klemmbrett K und Buch B, je
 * ein Polygon (320er-Koordinaten; Cut 6 abgelesen, Cut 2 ueber die Tischplatte y = -1520 mit
 * der Engine-Matrix projiziert - r30_tl_licht_auswertung.py polygone(), Schrumpf 0). Ein
 * 960er-Pixel c zaehlt, wenn (c + 0,5) / 3 im Polygon liegt. Helligkeit Y = 0,299 R +
 * 0,587 G + 0,114 B des Mittel-RGB (das Wort des Auftrags: "mittlere Helligkeit").
 * PRUEFUNGEN:
 *   1. Cut 6: Buch und Karte je > 0 Pixel.
 *   2. Cut 6: Y(Buch) und Y(Karte) INNERHALB der Spanne [Y(B), Y(K)] der gemalten Umgebung.
 *   3. Kein Cut dunkler gegen seine Malerei als Cut 2 unter seinem Original-Licht:
 *      dY(Cut 6) >= dY(Cut 2) je Prop (dY = vorzeichenbehafteter Abstand zur Spanne,
 *      negativ = dunkler als das dunkelste gemalte Objekt).
 * GEMESSEN (Abnahme, Dossier Abschnitt 10): vorher Cut 6 Y 15,6 / 16,5 -> 2. und 3. rot;
 * nachher 39,4 / 44,0 in [37,8 .. 105,0] -> gruen.
 *
 * Aufruf: probe_r30_irons_tisch_licht <P6.ppm> <P60.ppm> <P2.ppm> <P20.ppm>
 * Ausgabe: Messzeilen, zuletzt "ERGEBNIS OK" bzw. "ERGEBNIS FEHLER: ..."; Rueckgabe 0/1
 * (2 = Bild nicht lesbar).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;
typedef struct { double x, y; } pkt_t;

static int lies_zahl(FILE *f)
{
    int c, v = 0, hat = 0;
    for (;;) {
        c = fgetc(f);
        if (c == '#') { while (c != '\n' && c != EOF) c = fgetc(f); continue; }
        if (c == EOF) return -1;
        if (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); hat = 1; }
        else if (hat) return v;
    }
}

static int lade(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) { printf("FEHLER: %s nicht lesbar\n", pfad); return -1; }
    if (fgetc(f) != 'P' || fgetc(f) != '6') { printf("FEHLER: %s ist kein P6\n", pfad); fclose(f); return -1; }
    b->w = lies_zahl(f); b->h = lies_zahl(f);
    int mx = lies_zahl(f);
    if (b->w <= 0 || b->h <= 0 || mx != 255) { printf("FEHLER: %s Kopf\n", pfad); fclose(f); return -1; }
    size_t n = (size_t)b->w * (size_t)b->h * 3u;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { printf("FEHLER: %s kurz\n", pfad); fclose(f); return -1; }
    fclose(f);
    return 0;
}

static int gleich(const bild_t *a, const bild_t *b, size_t i)
{
    return a->px[i * 3] == b->px[i * 3] && a->px[i * 3 + 1] == b->px[i * 3 + 1] &&
           a->px[i * 3 + 2] == b->px[i * 3 + 2];
}

/* Punkt-in-Polygon (Strahlverfahren) - dieselbe Regel wie innen() im Python-Werkzeug. */
static int innen(const pkt_t *P, int n, double x, double y)
{
    int ins = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((P[i].y > y) != (P[j].y > y)) &&
            (x < (P[j].x - P[i].x) * (y - P[i].y) / (P[j].y - P[i].y + 1e-12) + P[i].x))
            ins = !ins;
    }
    return ins;
}

static double luma(const double rgb[3])
{
    return 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2];
}

/* Mittel-RGB ueber ein Polygon im Bild B. */
static long mittel_polygon(const bild_t *B, const pkt_t *P, double rgb[3])
{
    double s[3] = { 0, 0, 0 };
    long n = 0;
    for (int y = 0; y < B->h; y++)
        for (int x = 0; x < B->w; x++) {
            if (!innen(P, 4, (x + 0.5) / 3.0, (y + 0.5) / 3.0)) continue;
            size_t i = (size_t)y * (size_t)B->w + (size_t)x;
            for (int k = 0; k < 3; k++) s[k] += B->px[i * 3 + k];
            n++;
        }
    for (int k = 0; k < 3; k++) rgb[k] = n ? s[k] / (double)n : 0.0;
    return n;
}

/* Mittel-RGB der Prop-Pixel (P != P0) links (links=1) bzw. rechts der 960er-Grenzspalte. */
static long mittel_prop(const bild_t *P, const bild_t *P0, int grenze, int links, double rgb[3])
{
    double s[3] = { 0, 0, 0 };
    long n = 0;
    for (int y = 0; y < P->h; y++)
        for (int x = 0; x < P->w; x++) {
            if ((x < grenze) != (links != 0)) continue;
            size_t i = (size_t)y * (size_t)P->w + (size_t)x;
            if (gleich(P, P0, i)) continue;
            for (int k = 0; k < 3; k++) s[k] += P->px[i * 3 + k];
            n++;
        }
    for (int k = 0; k < 3; k++) rgb[k] = n ? s[k] / (double)n : 0.0;
    return n;
}

static double dy(double y, double lo, double hi)
{
    if (y < lo) return y - lo;
    if (y > hi) return y - hi;
    return 0.0;
}

typedef struct {
    int cut, grenze;
    pkt_t K[4], B[4];
    double yb, yk, lo, hi;          /* Referenz */
    double y_buch, y_karte;         /* Props */
    long n_buch, n_karte;
} cut_t;

static void miss(cut_t *c, const bild_t *P, const bild_t *P0)
{
    double rk[3], rb[3], pb[3], pk[3];
    long nk = mittel_polygon(P0, c->K, rk);
    long nb = mittel_polygon(P0, c->B, rb);
    c->yk = luma(rk); c->yb = luma(rb);
    c->lo = c->yk < c->yb ? c->yk : c->yb;
    c->hi = c->yk < c->yb ? c->yb : c->yk;
    c->n_karte = mittel_prop(P, P0, c->grenze, 1, pk);
    c->n_buch  = mittel_prop(P, P0, c->grenze, 0, pb);
    c->y_karte = luma(pk); c->y_buch = luma(pb);
    printf("Cut %d gemalt: Klemmbrett %ld Pixel RGB (%.1f,%.1f,%.1f) Y %.1f | Buch %ld Pixel "
           "RGB (%.1f,%.1f,%.1f) Y %.1f | Spanne Y %.1f .. %.1f\n",
           c->cut, nk, rk[0], rk[1], rk[2], c->yk, nb, rb[0], rb[1], rb[2], c->yb, c->lo, c->hi);
    printf("Cut %d Props:  Buch (obj 5) %ld Pixel RGB (%.1f,%.1f,%.1f) Y %.1f dY %+.1f | "
           "Karte (obj 6) %ld Pixel RGB (%.1f,%.1f,%.1f) Y %.1f dY %+.1f\n",
           c->cut, c->n_buch, pb[0], pb[1], pb[2], c->y_buch, dy(c->y_buch, c->lo, c->hi),
           c->n_karte, pk[0], pk[1], pk[2], c->y_karte, dy(c->y_karte, c->lo, c->hi));
}

int main(int argc, char **argv)
{
    if (argc != 5) {
        printf("Aufruf: %s <P6.ppm> <P60.ppm> <P2.ppm> <P20.ppm>\n", argv[0]);
        return 2;
    }
    bild_t P6, P60, P2, P20;
    if (lade(argv[1], &P6) || lade(argv[2], &P60) || lade(argv[3], &P2) || lade(argv[4], &P20))
        return 2;
    if (P60.w != P6.w || P60.h != P6.h || P20.w != P2.w || P20.h != P2.h ||
        P6.w != 960 || P6.h != 720 || P2.w != 960 || P2.h != 720) {
        printf("FEHLER: Bildgroessen %dx%d / %dx%d, erwartet 960x720\n", P6.w, P6.h, P2.w, P2.h);
        return 2;
    }

    /* Polygone: r30_tl_licht_auswertung.py polygone() (Schrumpf 0). */
    cut_t c6 = { 6, 525,
        { {176.300, 105.800}, {211.300, 105.300}, {219.700, 149.700}, {181.700, 150.300} },
        { {244.700, 101.300}, {284.200, 116.300}, {275.000, 152.000}, {235.000, 135.000} },
        0, 0, 0, 0, 0, 0, 0, 0 };
    cut_t c2 = { 2, 437,
        { {146.475, 122.027}, {159.108, 121.586}, {161.365, 128.511}, {148.043, 128.995} },
        { {171.162, 120.587}, {184.736, 122.611}, {180.449, 128.273}, {166.981, 126.074} },
        0, 0, 0, 0, 0, 0, 0, 0 };
    miss(&c6, &P6, &P60);
    miss(&c2, &P2, &P20);

    int fehler = 0;
    char grund[512] = "";
    size_t gl = 0;
#define FEHLER(...) do { fehler = 1; gl += (size_t)snprintf(grund + gl, sizeof grund - gl, __VA_ARGS__); } while (0)
    if (c6.n_buch <= 0)  FEHLER("Cut 6 Buch unsichtbar; ");
    if (c6.n_karte <= 0) FEHLER("Cut 6 Karte unsichtbar; ");
    if (c2.n_buch <= 0 || c2.n_karte <= 0) FEHLER("Cut 2 Prop unsichtbar; ");
    if (!fehler) {
        if (c6.y_buch < c6.lo || c6.y_buch > c6.hi)
            FEHLER("Cut 6 Buch Y %.1f ausserhalb der gemalten Spanne %.1f..%.1f; ", c6.y_buch, c6.lo, c6.hi);
        if (c6.y_karte < c6.lo || c6.y_karte > c6.hi)
            FEHLER("Cut 6 Karte Y %.1f ausserhalb der gemalten Spanne %.1f..%.1f; ", c6.y_karte, c6.lo, c6.hi);
        double b6 = dy(c6.y_buch, c6.lo, c6.hi),  b2 = dy(c2.y_buch, c2.lo, c2.hi);
        double k6 = dy(c6.y_karte, c6.lo, c6.hi), k2 = dy(c2.y_karte, c2.lo, c2.hi);
        printf("Quervergleich: Buch dY Cut 6 %+.1f / Cut 2 %+.1f | Karte dY Cut 6 %+.1f / Cut 2 %+.1f\n",
               b6, b2, k6, k2);
        if (b6 < b2) FEHLER("Cut 6 Buch dunkler gegen die Malerei als Cut 2 (dY %+.1f < %+.1f); ", b6, b2);
        if (k6 < k2) FEHLER("Cut 6 Karte dunkler gegen die Malerei als Cut 2 (dY %+.1f < %+.1f); ", k6, k2);
    }
    if (fehler) { printf("ERGEBNIS FEHLER: %s\n", grund); return 1; }
    printf("ERGEBNIS OK\n");
    return 0;
}
