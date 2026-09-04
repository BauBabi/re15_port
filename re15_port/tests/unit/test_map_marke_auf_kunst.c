/* Karte: sitzt jede Tuer-/Treppenmarke auf GEMALTEM Grundriss?
 *
 * ⛔ NUTZER-BEFUND 2026-09-04 (fehler/error1.png, 2F): das Treppensymbol stand auf
 * leerem Blau - "da fehlt das komplette Rechteck". Genau das faengt KEINE der
 * bestehenden Schranken: die Marke lag INNERHALB ihres Rechtecks. Ein Karten-Rechteck
 * ist naemlich nur der Kasten, aus dem geblittet wird; die Kachel darin enthaelt
 * Schwarz (Palettenindex 0 wird nicht gezeichnet). Eine Marke kann also im Kasten
 * liegen und trotzdem neben der Zeichnung stehen.
 *
 * GEMESSEN WIRD AM ZUSAMMENBAU, NICHT AN DER EIGENEN KACHEL.
 * Die Rechtecke einer Seite ueberlappen einander (der Kuenstler zeichnet Flure in
 * Saele hinein). Eine Marke kann deshalb neben der Zeichnung IHRES Rechtecks liegen
 * und trotzdem auf gemaltem Grundriss sitzen - naemlich auf dem des Nachbarn. Was
 * zaehlt, ist was der Spieler SIEHT: die Seite wird hier so zusammengesetzt, wie der
 * Zeichner sie zusammensetzt (jedes Rechteck aus seiner Kachel, uv aus Byte +8/+10 des
 * 12-Byte-Eintrags, SPRT-Aufbau @0x8004731c-60), und danach gefragt, ob unter der Marke
 * etwas liegt. Geometrie und uv kommen aus den Engine-Zugriffen
 * (re15_map_rect_geometry / re15_map_rect_uv), also aus demselben Weg, den der Zeichner
 * geht, samt der Ersatztabelle fuer Blatt 3. Zugestanden ist ein 3x3-Fenster: eine
 * Tuermarke klebt an der Wand und darf einen Punkt neben der Wandlinie liegen.
 * (Memory reai-v2-tabelle-vs-bild: was der Nutzer sieht, am gerasterten Bild pruefen.)
 *
 * SCHRANKE. Am AUSGELIEFERTEN Stand v0.6.1 gemessen: 173 Marken mit Rechteck, 13 auf
 * leerer Flaeche, davon auf Blatt 3 zwei von sechs - die schlechteste Quote aller
 * Blaetter und genau der gemeldete Fall. Mit der Ersatztabelle (BEFUND.md §44, gemessen
 * am FIXPUNKT des Generators, §45): 177 Marken, 8 leer, Blatt 3 eine von neun. Diese
 * eine ist die Treppenmarke an der UNTERKANTE ihres Rechtecks - sie liegt 2 px unter
 * der gezeichneten Flaeche, nicht mehr freischwebend ohne Rechteck.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Kartenblatt: headerlos, 256x256 4bpp, 128 Byte je Zeile, unteres Nibble = links.
 * Seite N -> MAP{N+1}.PIX (id-Tabelle @0x80074c4c, id 12..24 -> MAP01..MAP0D). */
static int blatt_lesen(int page, unsigned char px[256][256])
{
    char pfad[600];
    FILE *f;
    static unsigned char roh[256 * 128];
    int y, x;
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/DATA/MAP%02X.PIX",
             RE15_PORT_SRC_DIR, page + 1);
    f = fopen(pfad, "rb");
    if (!f) return 0;
    if (fread(roh, 1, sizeof roh, f) != sizeof roh) { fclose(f); return 0; }
    fclose(f);
    for (y = 0; y < 256; y++)
        for (x = 0; x < 256; x++)
            px[y][x] = (x & 1) ? (unsigned char)(roh[y * 128 + (x >> 1)] >> 4)
                               : (unsigned char)(roh[y * 128 + (x >> 1)] & 0x0F);
    return 1;
}

/* Die Seite so zusammensetzen, wie der Zeichner sie zusammensetzt. */
static void seite_bauen(int page, const unsigned char px[256][256],
                        unsigned char schirm[256][320])
{
    int ri, x, y, w, h, u, v, b, a;
    memset(schirm, 0, 256 * 320);
    for (ri = 0; ri < 64; ri++) {
        if (!re15_map_rect_geometry((unsigned)page, (unsigned)ri, &x, &y, &w, &h)) break;
        if (!re15_map_rect_uv((unsigned)page, (unsigned)ri, &u, &v)) break;
        for (b = 0; b < h; b++)
            for (a = 0; a < w; a++) {
                int sy = y + b, sx = x + a, ty = v + b, tx = u + a;
                if (ty < 0 || ty > 255 || tx < 0 || tx > 255) continue;
                if (sy < 0 || sy > 255 || sx < 0 || sx > 319) continue;
                if (px[ty][tx]) schirm[sy][sx] = 1;
            }
    }
}

int main(void)
{
    static unsigned char px[256][256];
    static unsigned char schirm[256][320];
    int n = re15_map_mark_count();
    int i, pg;
    int ges = 0, leer = 0, leer3 = 0, ges3 = 0, gelesen = 0;

    printf("== Marken auf gemaltem Grundriss (Zusammenbau der Seite) ==\n");
    for (pg = 0; pg < 13; pg++) {
        int pg_ges = 0, pg_leer = 0;
        if (!blatt_lesen(pg, px)) { printf("  (Blatt %d nicht lesbar)\n", pg); continue; }
        gelesen++;
        seite_bauen(pg, px, schirm);
        for (i = 0; i < n; i++) {
            int p, r, mx, my, kind, a, b, treffer = 0;
            re15_map_mark_get(i, &p, &r, &mx, &my, &kind);
            if (p != pg || r == 255) continue;
            pg_ges++;
            for (b = -1; b <= 1 && !treffer; b++)
                for (a = -1; a <= 1 && !treffer; a++) {
                    int qx = mx + a, qy = my + b;
                    if (qx >= 0 && qx < 320 && qy >= 0 && qy < 256 && schirm[qy][qx])
                        treffer = 1;
                }
            if (!treffer) {
                pg_leer++;
                printf("     Marke %3d Blatt %2d rect %2d (%3d,%3d) sitzt auf leerer "
                       "Flaeche\n", i, p, r, mx, my);
            }
        }
        if (pg_ges) printf("  Blatt %2d: %3d Marken, %d auf leerer Flaeche\n",
                           pg, pg_ges, pg_leer);
        ges += pg_ges; leer += pg_leer;
        if (pg == 3) { ges3 = pg_ges; leer3 = pg_leer; }
    }
    printf("  GESAMT: %d Marken mit Rechteck, %d auf leerer Flaeche\n", ges, leer);

    CHECK("alle 13 Kartenblaetter lesbar", gelesen == 13);
    CHECK("Blatt 3 (2F) hat Marken mit Rechteck", ges3 > 0);
    {
        char t[200];
        snprintf(t, sizeof t, "Blatt 3 (2F): hoechstens eine Marke auf leerer Flaeche "
                 "(ausgeliefert v0.6.1: 2 von 6) - ist %d von %d", leer3, ges3);
        CHECK(t, leer3 <= 1);
        snprintf(t, sizeof t, "gesamt hoechstens 8 Marken auf leerer Flaeche "
                 "(ausgeliefert v0.6.1: 13 von 173) - sind %d von %d", leer, ges);
        CHECK(t, leer <= 8);
    }
    return g_fail;
}
