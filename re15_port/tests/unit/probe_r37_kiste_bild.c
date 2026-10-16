/* probe_r37_kiste_bild.c — MESS-WERKZEUG fuer integration_r37_kiste (kein eigener Test), Runde 37 Spur M.
 * Dossier analysis/befunde_runde37/M_kiste.md. Prueft Bilder der ECHTEN exe (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1,
 * P6 320x240) gegen das RE2-Panel shared_assets/RE2/BOXPANEL.BIN (214x156 RGB555, bei (6,25) geblittet):
 *
 *   RAHMEN  jedes Bild: jedes Pixel des Listen-Panels (x 12..214, y 25..180) AUSSERHALB des Listenfensters
 *           (x 13..203, y 52..152) ist bitgleich das Panel-Pixel — kein Zeilen-Pixel liegt ueber dem Kasten, die
 *           Rahmen-Pixel sind unveraendert.
 *           Ausgenommen: der gelbe Scrollgriff in der Schiene (x 208..213, y 54..153, wandert mit dem Ring) und die
 *           linken Enden des Auswahl-Bands (x 12, y 92/93/112/113), beide unveraendert UEBER dem Rahmen.
 *   RUHE    (Bild nach "ruhe:"): Zeilen 0..4 mit Symbolart je Zeile (S = 25 px, B = 2-Feld 49 px): das Symbol
 *           beginnt bei x 204-Breite (Spalte hat Symbolpixel), die Spalte links davor ist reiner Listengrund
 *           (24,24,40) — kein Name reicht an das Symbol.
 *   RE:/HE: Ende eines Gleitschritts RUNTER/HOCH (Versatz -18/+18): die EINLAUFENDE Zeile ist zu sehen (Symbolfeld
 *           x 179..203 unten y 140..152 bzw. oben y 52..64 hat Symbolpixel), wie RE2 (Zustand 3 scroll+5 /
 *           Zustand 2 scroll-1).
 *
 * Aufruf: probe_r37_kiste_bild <BOXPANEL.BIN> <arten SBSSS> ruhe:<ppm> [<ppm>|RE:<ppm>|HE:<ppm> ...]
 * Ausgabe je Bild eine Zeile, Ende "KISTE-BILD OK" oder "KISTE-BILD FEHLER n"; exit 0/1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PW 214
#define PH 156
#define PX0 6
#define PY0 25
#define WX0 13
#define WX1 203
#define WY0 52
#define WY1 152

static uint16_t s_panel[PH][PW];
static unsigned char s_bild[240][320][3];

static int ppm_laden(const char *pfad)
{
    FILE *f = fopen(pfad, "rb");
    int w = 0, h = 0, mx = 0;
    if (!f) return 0;
    if (fscanf(f, "P6 %d %d %d", &w, &h, &mx) != 3 || w != 320 || h != 240 || mx != 255) { fclose(f); return 0; }
    fgetc(f);
    if (fread(s_bild, 1, sizeof s_bild, f) != sizeof s_bild) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

static int ist_grund(int x, int y)   /* Listengrund (24,24,40) — BOXPANEL Bild-x 7..197 / y 27..127 */
{
    return s_bild[y][x][0] == 24 && s_bild[y][x][1] == 24 && s_bild[y][x][2] == 40;
}
static int band_zeile(int y) { return y == 92 || y == 93 || y == 112 || y == 113; }

static int rahmen_pruefen(const char *name)
{
    int x, y, falsch = 0, fx = -1, fy = -1;
    /* Spalten x 6..11 und 215..219 des Panels liegen in JEDEM Bild unter der Menue-Chrome bzw. dem Inventar-
     * Gitter (Ops ueber dem Panel; vorher/nachher bitgleich gemessen, Dossier §4) — geprueft wird x 12..214. */
    for (y = PY0; y < PY0 + PH; y++)
        for (x = 12; x <= 214; x++) {
            uint16_t c;
            if (x >= WX0 && x <= WX1 && y >= WY0 && y <= WY1) continue;      /* Listenfenster */
            if (x >= 208 && x <= 213 && y >= 54 && y <= 153) continue;      /* Scrollgriff in der Schiene */
            if (x == 12 && band_zeile(y)) continue;                          /* Band-Enden */
            c = s_panel[y - PY0][x - PX0];
            if (s_bild[y][x][0] != (unsigned char)((c & 31) << 3) ||
                s_bild[y][x][1] != (unsigned char)(((c >> 5) & 31) << 3) ||
                s_bild[y][x][2] != (unsigned char)(((c >> 10) & 31) << 3)) {
                if (falsch == 0) { fx = x; fy = y; }
                falsch++;
            }
        }
    printf("%s: Rahmen %s (%d Pixel ausserhalb des Fensters anders%s", name, falsch ? "VERLETZT" : "ok", falsch,
           falsch ? ", erstes " : ")\n");
    if (falsch) printf("(%d,%d) = (%d,%d,%d))\n", fx, fy, s_bild[fy][fx][0], s_bild[fy][fx][1], s_bild[fy][fx][2]);
    return falsch ? 1 : 0;
}

static int ruhe_pruefen(const char *name, const char *arten)
{
    int k, fehler = 0;
    for (k = 0; k < 5 && arten[k]; k++) {
        int w = (arten[k] == 'B') ? 49 : 25, sx = 204 - w, ry = 53 + 20 * k, y, symbol = 0, grund = 1;
        for (y = ry; y <= ry + 18; y++) {
            if (band_zeile(y)) continue;
            if (!ist_grund(sx, y)) symbol = 1;
            if (!ist_grund(sx - 1, y)) grund = 0;
        }
        printf("%s: Zeile %d (%c) Symbol ab x %d %s, Spalte %d %s\n", name, k, arten[k], sx,
               symbol ? "ja" : "FEHLT", sx - 1, grund ? "Listengrund" : "NICHT frei (Name/Symbol reicht heran)");
        if (!symbol || !grund) fehler++;
    }
    return fehler;
}

static int einlauf_pruefen(const char *name, int unten)
{
    int x, y, n = 0, y0 = unten ? 140 : 52, y1 = unten ? 152 : 64;
    for (y = y0; y <= y1; y++)
        for (x = 179; x <= 203; x++)
            if (!ist_grund(x, y)) n++;
    printf("%s: einlaufende Zeile %s: %d Symbolpixel im Feld x 179..203 y %d..%d %s\n", name,
           unten ? "von UNTEN" : "von OBEN", n, y0, y1, n >= 20 ? "ok" : "FEHLT");
    return n >= 20 ? 0 : 1;
}

int main(int argc, char **argv)
{
    FILE *f;
    int i, fehler = 0;
    if (argc < 4) { printf("Aufruf: panel.bin arten ruhe:<ppm> [ppm|RE:ppm|HE:ppm ...]\n"); return 2; }
    f = fopen(argv[1], "rb");
    if (!f || fread(s_panel, 1, sizeof s_panel, f) != sizeof s_panel) {
        printf("FEHLER: %s nicht lesbar\n", argv[1]);
        if (f) fclose(f);
        return 2;
    }
    fclose(f);
    for (i = 3; i < argc; i++) {
        const char *a = argv[i], *pfad = a;
        int art = 0;   /* 0 = nur Rahmen, 1 = Ruhe, 2 = RUNTER-Ende, 3 = HOCH-Ende */
        if (!strncmp(a, "ruhe:", 5)) { art = 1; pfad = a + 5; }
        else if (!strncmp(a, "RE:", 3)) { art = 2; pfad = a + 3; }
        else if (!strncmp(a, "HE:", 3)) { art = 3; pfad = a + 3; }
        if (!ppm_laden(pfad)) { printf("%s: FEHLER Bild nicht lesbar (P6 320x240 erwartet)\n", pfad); fehler++; continue; }
        fehler += rahmen_pruefen(pfad);
        if (art == 1) fehler += ruhe_pruefen(pfad, argv[2]);
        if (art == 2) fehler += einlauf_pruefen(pfad, 1);
        if (art == 3) fehler += einlauf_pruefen(pfad, 0);
    }
    if (fehler) { printf("KISTE-BILD FEHLER %d\n", fehler); return 1; }
    printf("KISTE-BILD OK\n");
    return 0;
}
