/* probe_r30_irons_tisch_bild.c — AUSWERTER (kein add_test) fuer den Bild-Riegel
 * integration_r30_irons_tisch_bild (Runde 30, Thema irons-diary-welt, Nachbesserung nach dem
 * Gegenpruefer).
 *
 * BEFUND DES GEGENPRUEFERS (Mutationsprobe): ohne die Tiefen-Klemme im Prop-Zeichner
 * (platform/pc/main.c, Dreiecke und Vierecke) sind Buch und Memory Card in Cut 2 unsichtbar
 * (0 Pixel gegen das Nullbild statt 674), und die ganze Suite blieb gruen:
 * integration_r30_irons_tisch_laden prueft nur die Logzeile "[prop-render] pi=5/6", die VOR
 * der Klemme ausgegeben wird, und unit_r30_irons_tisch Teil K nur den Rueckgabewert von
 * re15_irons_tisch_sort_max (ohne main.c).
 *
 * DIESES WERKZEUG vergleicht vier Framedumps DESSELBEN Bilds in Cut 2 (RE15_FRAMEDUMP =
 * Readback des fertig komponierten Frames vor SDL_RenderPresent, beschleunigter Renderer):
 *   P   Buch und Karte da,        Spieler weit weg vom Tisch
 *   P0  beides genommen (Nullbild: Bits (9,54)/(9,55) im Spielstand), Spieler weit weg
 *   S   Buch und Karte da,        Spieler vor dem Tisch
 *   S0  beides genommen,          Spieler vor dem Tisch
 * und prueft das SICHTBARE Ergebnis:
 *   1. Prop-Pixel = P != P0. In der ROTEN Marke des Nutzerbilds "irons items.png" (x 146..158,
 *      y 120..132 in 320x240, marken.txt) liegen > 0 davon (das Buch), in der BLAUEN Marke
 *      (x 135..144, y 121..131) > 0 (die Karte). Pixel ausserhalb beider Marken gehoeren der
 *      naeheren Markenmitte; die Huellenmitte jedes Props liegt IN seiner Marke. (Lage B legt
 *      das Buch auf das gemalte Klemmbrett, gut 2 px neben der Markenmitte; sein Rand reicht
 *      eine 320er-Spalte ueber die Marke hinaus.)
 *   2. Spieler-Pixel = S0 != P0, Ueberschneidung O = beides. O > 0 (sonst sagt die Probe
 *      nichts), und in O gilt ueberall S == S0: die Figur VOR dem Tisch liegt UEBER den
 *      Props — die Klemme darf die Props nicht vor alles legen.
 * Marken-Mitten (marken.txt): rot (152,5 ; 126,5), blau (140,0 ; 126,5) - stetige
 * 320er-Koordinaten, Pixel p deckt [p, p+1) (dieselbe Konvention wie die Projektion
 * 160 + H*vx/vz der Sonde und unit_r30_irons_tisch Teil M). Ein 960er-Pixel c hat die Mitte
 * (c + 0,5) / 3. ⛔ Die Auswerter der ersten Abnahme (r30_idw_bau_auswertung.py, diff.py des
 * Gegenpruefers) rechneten (c + 0,5) / 3 - 0,5 und verglichen das mit diesen Marken - ein
 * halbes Pixel Versatz in x UND y.
 *
 * Aufruf: probe_r30_irons_tisch_bild <P.ppm> <P0.ppm> <S.ppm> <S0.ppm>
 * Ausgabe: Messzeilen und zuletzt "ERGEBNIS OK" bzw. "ERGEBNIS FEHLER: ..."; Rueckgabe 0/1
 * (2 = Bild nicht lesbar).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct { int w, h; unsigned char *px; } bild_t;

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

typedef struct { int x0, x1, y0, y1; double mx, my; const char *name; } marke_t;

int main(int argc, char **argv)
{
    if (argc != 5) {
        printf("Aufruf: %s <P.ppm> <P0.ppm> <S.ppm> <S0.ppm>\n", argv[0]);
        return 2;
    }
    bild_t P, P0, S, S0;
    if (lade(argv[1], &P) || lade(argv[2], &P0) || lade(argv[3], &S) || lade(argv[4], &S0))
        return 2;
    if (P0.w != P.w || S.w != P.w || S0.w != P.w || P0.h != P.h || S.h != P.h || S0.h != P.h) {
        printf("FEHLER: Bildgroessen verschieden\n");
        return 2;
    }
    const int W = P.w, H = P.h;
    const double kx = W / 320.0, ky = H / 240.0;
    /* Marken des Nutzerbilds, 320x240, Grenzen eingeschlossen (marken.txt). */
    const marke_t mk[2] = {
        { 146, 158, 120, 132, 152.5, 126.5, "ROT (Buch)"  },
        { 135, 144, 121, 131, 140.0, 126.5, "BLAU (Karte)" },
    };
    /* je Prop: n_in = Pixel IN seiner Marke, n_zug = alle ihm zugeordneten Pixel (in der Marke,
     * sonst die naechste Markenmitte), Huelle ueber alle zugeordneten */
    long n_in[2] = { 0, 0 }, n_zug[2] = { 0, 0 }, n_aus = 0, n_prop = 0;
    int hx0[2] = { 1 << 30, 1 << 30 }, hx1[2] = { -1, -1 }, hy0[2] = { 1 << 30, 1 << 30 },
        hy1[2] = { -1, -1 };
    long n_spieler = 0, n_ueber = 0, n_oben = 0, n_unten = 0;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            size_t i = (size_t)y * (size_t)W + (size_t)x;
            int prop = !gleich(&P, &P0, i);
            int spieler = !gleich(&S0, &P0, i);
            if (spieler) n_spieler++;
            if (prop && spieler) {
                n_ueber++;
                if (gleich(&S, &S0, i)) n_oben++; else n_unten++;
            }
            if (!prop) continue;
            n_prop++;
            /* 320er-Pixel (Index) und stetige 320er-Koordinate der Pixelmitte. Konvention wie
             * marken.txt und die Projektion 160 + H*vx/vz: Pixel p deckt [p, p+1), Mitte p+0.5. */
            int qx = (int)floor(x / kx), qy = (int)floor(y / ky);
            double sx = (x + 0.5) / kx, sy = (y + 0.5) / ky;
            int k = -1;
            for (int m = 0; m < 2; m++)
                if (qx >= mk[m].x0 && qx <= mk[m].x1 && qy >= mk[m].y0 && qy <= mk[m].y1) k = m;
            if (k >= 0) {
                n_in[k]++;
            } else {
                n_aus++;
                double d0 = hypot(sx - mk[0].mx, sy - mk[0].my);
                double d1 = hypot(sx - mk[1].mx, sy - mk[1].my);
                k = (d1 < d0) ? 1 : 0;
            }
            n_zug[k]++;
            if (x < hx0[k]) { hx0[k] = x; }
            if (x > hx1[k]) { hx1[k] = x; }
            if (y < hy0[k]) { hy0[k] = y; }
            if (y > hy1[k]) { hy1[k] = y; }
        }
    }

    printf("Bild %dx%d (Faktor %.2f / %.2f zu 320x240)\n", W, H, kx, ky);
    printf("Prop-Pixel (P != P0): %ld, davon ausserhalb beider Marken %ld\n", n_prop, n_aus);
    int mitte_in[2] = { 0, 0 };
    for (int m = 0; m < 2; m++) {
        if (!n_zug[m]) { printf("  %-12s 0 Pixel\n", mk[m].name); continue; }
        /* Huellenmitte, stetige 320er-Koordinaten (Pixelmitte = p + 0.5) */
        double cx = ((hx0[m] + hx1[m]) / 2.0 + 0.5) / kx;
        double cy = ((hy0[m] + hy1[m]) / 2.0 + 0.5) / ky;
        mitte_in[m] = cx >= mk[m].x0 && cx < mk[m].x1 + 1 && cy >= mk[m].y0 && cy < mk[m].y1 + 1;
        printf("  %-12s %ld Pixel (%ld in der Marke x %d..%d y %d..%d), Huelle320 x %.2f..%.2f "
               "y %.2f..%.2f, Mitte (%.2f ; %.2f) %s der Marke, %.2f px neben der Markenmitte "
               "(%.1f ; %.1f)\n",
               mk[m].name, n_zug[m], n_in[m], mk[m].x0, mk[m].x1, mk[m].y0, mk[m].y1,
               hx0[m] / kx, (hx1[m] + 1) / kx, hy0[m] / ky, (hy1[m] + 1) / ky, cx, cy,
               mitte_in[m] ? "IN" : "AUSSERHALB", hypot(cx - mk[m].mx, cy - mk[m].my),
               mk[m].mx, mk[m].my);
    }
    printf("Spieler-Pixel (S0 != P0): %ld, Ueberschneidung mit den Props %ld: Figur obenauf %ld, "
           "Prop obenauf %ld\n", n_spieler, n_ueber, n_oben, n_unten);

    char grund[512] = "";
    if (!n_in[0]) strcat(grund, " Buch in der roten Marke unsichtbar (0 Pixel);");
    if (!n_in[1]) strcat(grund, " Karte in der blauen Marke unsichtbar (0 Pixel);");
    if (n_in[0] && !mitte_in[0]) strcat(grund, " Buchmitte ausserhalb der roten Marke;");
    if (n_in[1] && !mitte_in[1]) strcat(grund, " Kartenmitte ausserhalb der blauen Marke;");
    if (!n_ueber) strcat(grund, " keine Ueberschneidung Figur/Props (Probe sagt nichts);");
    if (n_unten)  strcat(grund, " Prop liegt UEBER der Figur vor dem Tisch;");
    if (grund[0]) { printf("ERGEBNIS FEHLER:%s\n", grund); return 1; }
    printf("ERGEBNIS OK\n");
    return 0;
}
