/* Eine sichtbare Tuermarke darf nicht frei im Blau stehen.
 *
 * ⛔ NUTZER-BEFUND 2026-09-05 (fehler/error.png, 2F): "This door is flying - not on
 * the wall/Edge." Eine gelbe Marke stand frei im Blau, rechts neben dem Rechteck des
 * Flurs. Gemessen: Marke #51, Blatt 3, zid 12 (ROOM10D0) / zid2 11 (ROOM10C0), Lage
 * (141,117). Ihre LAGE stammt aus der Projektion IHRER Zone auf DEREN Rechteck
 * (Rect 3, (135,76) 72x88). ROOM10D0 war unbetreten, Rect 3 wurde also nicht
 * gezeichnet - die alte Regel zeigte die Marke trotzdem, weil die ZWEITE Zone besucht
 * war.
 *
 * ⛔ DER KASTEN REICHT ALS ERKLAERUNG NICHT, und daran waere der erste Fix
 * gescheitert: (141,117) liegt IM Kasten von Rect 0 (102,116) 40x40 - ein Kasten ist
 * aber nur die Bounding-Box, die Kachel darin traegt dort Index 0. Gemalt ist die
 * Stelle nur in Rect 3s Kachel.
 *
 * REGEL SEIT v0.6.5: der Generator rechnet je Marke das Bit auf_partner aus - liegt
 * sie auf der GEMALTEN Flaeche des Partner-Rechtecks? 47 von 70 ja. Sichtbar ist eine
 * Marke, wenn ihre eigene Zone besucht ist ODER auf_partner gilt und die Partnerzone
 * besucht ist. (Der Zwischenstand "nur die eigene Zone" machte unit_inv_fsm und
 * unit_map_re2_system rot: auf dem Dach-Blatt verschwand eine korrekt sitzende Tuer.)
 *
 * GEPRUEFT WERDEN ZWEI DINGE, beide ueber drei Besuchsstaende:
 *   1. HARTE INVARIANTE (Schranke 0): keine sichtbare Marke liegt auf nichts Gemaltem
 *      UND gehoert zugleich zu einem ungezeichneten Rechteck. Das ist genau die
 *      gemeldete Klasse. Gegenprobe mit der alten Regel gefahren: sie schlaegt an.
 *   2. GEMESSENE SCHRANKE: wie viele sichtbare Marken ueberhaupt neben der GEMALTEN
 *      Flaeche liegen. Vorher/nachher:
 *          2F-Anfang        2 -> 1     (die entfallene ist die gemeldete Marke)
 *          3F-Kette         0 -> 0
 *          alles betreten   5 -> 5
 *      Die verbleibenden gehoeren einer anderen Klasse an - die Marke sitzt auf dem
 *      eigenen, gezeichneten Kasten, aber ausserhalb der Zeichnung darin (auf Blatt 3
 *      das Treppensymbol bei (118,157), zwei Pixel unter der gemalten Flaeche; s.
 *      BEFUND.md §44). Die fuehrt unit_map_marke_auf_kunst als Quote; hier steht sie
 *      als Nicht-Verschlechterung.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Kartenblatt: headerlos, 256x256 4bpp, 128 Byte je Zeile, unteres Nibble = links. */
static int blatt_lesen(int page, unsigned char px[256][256])
{
    char pfad[600]; FILE *f; static unsigned char roh[256 * 128]; int y, x;
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

static unsigned char g_px[13][256][256];

static void blaetter_laden(void)
{
    static int geladen = 0;
    int i;
    if (geladen) return;
    for (i = 0; i < 13; i++) blatt_lesen(i, g_px[i]);
    geladen = 1;
}

/* ⛔ DER KASTEN REICHT NICHT. Ein Karten-Rechteck ist nur die Bounding-Box; die Kachel
 * darin enthaelt Schwarz (Index 0 wird nicht gezeichnet). Genau daran waere diese
 * Schranke beinahe blind geblieben: die gemeldete Marke (141,117) liegt IM Kasten von
 * Rect 0 (102,116) 40x40, dessen Kachel traegt dort aber Index 0. Gemalt ist die
 * Stelle nur in Rect 3s Kachel - und Rect 3 wird nicht gezeichnet.
 * Geprueft wird deshalb gegen das GEMALTE Bild der Seite, aus den Rechtecken
 * zusammengesetzt, die wirklich gezeichnet werden. */
static int haengt_an(int page, int mx, int my, const unsigned char px[256][256])
{
    int zi, nz = re15_map_zone_count();
    for (zi = 0; zi < nz; zi++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(zi);
        int rx, ry, rw, rh, u, v, a, b;
        if (!zn || zn->page != page || zn->rect == 255) continue;
        if (!re15_map_visited(zn->room)) continue;
        if (!re15_map_rect_geometry((unsigned)page, zn->rect, &rx, &ry, &rw, &rh)) continue;
        if (!re15_map_rect_uv((unsigned)page, zn->rect, &u, &v)) continue;
        for (b = -1; b <= 1; b++)
            for (a = -1; a <= 1; a++) {
                int sx = mx + a, sy = my + b, tx, ty;
                if (sx < rx || sx >= rx + rw || sy < ry || sy >= ry + rh) continue;
                tx = u + (sx - rx); ty = v + (sy - ry);
                if (tx < 0 || tx > 255 || ty < 0 || ty > 255) continue;
                if (px[ty][tx]) return 1;
            }
    }
    return 0;
}

/* ⛔ DIE HARTE INVARIANTE - und warum sie NICHT "das eigene Rechteck muss
 * gezeichnet sein" lautet.
 *
 * Der Nutzer meldete "This door is flying": eine Marke stand FREI IM BLAU. Meine erste
 * Fassung pruefte statt dessen, ob ihr eigenes Rechteck gezeichnet wird - ein
 * Stellvertreter, der STRENGER ist als der Befund. Er schlug prompt bei einem Fall an,
 * den die auf_partner-Regel bewusst erlaubt: Marke 60 (Blatt 4, rect 1 = ROOM1060,
 * (137,151)) ist sichtbar, ihr eigenes Rechteck ungezeichnet - aber sie liegt auf der
 * GEMALTEN Flaeche des Nachbarn. Sie schwebt also gerade nicht.
 *
 * Geprueft wird deshalb der Befund selbst: eine sichtbare Marke, die auf NICHTS
 * Gemaltem liegt UND deren eigenes Rechteck ungezeichnet ist. Das ist genau die Klasse
 * des gemeldeten Fehlers und schliesst die verbleibenden 5 aus, die ihr eigenes,
 * gezeichnetes Rechteck haben und nur neben der Zeichnung DARIN sitzen (§44).
 * Gegenprobe mit der alten Sichtbarkeitsregel gefahren: sie schlaegt an (Marke #51). */
static int fremdes_rect(const char *name, const unsigned *raeume, int n,
                        const unsigned char px[13][256][256])
{
    int i, schlecht = 0;
    re15_map_visited_reset();
    for (i = 0; i < n; i++) re15_map_visited_mark(raeume[i]);
    for (i = 0; i < re15_map_mark_count(); i++) {
        int p, r, mx, my, kind, zi, nz, eigen = 0;
        if (!re15_map_mark_get(i, &p, &r, &mx, &my, &kind)) continue;
        if (r == 255) continue;
        if (p >= 0 && p <= 12 && haengt_an(p, mx, my, px[p])) continue;  /* liegt auf Kunst */
        nz = re15_map_zone_count();
        for (zi = 0; zi < nz && !eigen; zi++) {
            const re15_map_zone_t *zn = re15_map_zone_by_index(zi);
            if (zn && zn->page == p && zn->rect == r && re15_map_visited(zn->room))
                eigen = 1;
        }
        if (!eigen) {
            schlecht++;
            printf("     Marke %3d Blatt %2d rect %2d (%3d,%3d): nichts Gemaltes UND ihr "
                   "Rechteck wird nicht gezeichnet\n", i, p, r, mx, my);
        }
    }
    printf("  [%s] frei im Blau schwebende Marken: %d\n", name, schlecht);
    return schlecht;
}

/* Ein Besuchsstand: nur die genannten Raeume sind betreten. */
static int pruefe(const char *name, const unsigned *raeume, int n)
{
    int i, frei = 0, sichtbar = 0;
    blaetter_laden();
    re15_map_visited_reset();
    for (i = 0; i < n; i++) re15_map_visited_mark(raeume[i]);
    for (i = 0; i < re15_map_mark_count(); i++) {
        int p, r, mx, my, kind;
        if (!re15_map_mark_get(i, &p, &r, &mx, &my, &kind)) continue;  /* unsichtbar */
        if (r == 255) continue;      /* Schema-Zeichnung, kein gemaltes Rechteck */
        sichtbar++;
        if (p < 0 || p > 12 || !haengt_an(p, mx, my, g_px[p])) {
            frei++;
            printf("     Marke %3d Blatt %2d rect %2d (%3d,%3d) haengt an nichts\n",
                   i, p, r, mx, my);
        }
    }
    printf("  [%s] %d sichtbare Marken mit Rechteck, %d davon freischwebend\n",
           name, sichtbar, frei);
    return frei;
}

int main(void)
{
    /* Der gemeldete Fall: 2F betreten (Treppenhaus), eine Tuer weiter in den Flur. */
    static const unsigned nur_2f_anfang[] = { 0x1060u, 0x10C0u };
    /* Ein zweiter Stand, damit nicht nur eine Konstellation geprueft wird. */
    static const unsigned drei_raeume[]   = { 0x1130u, 0x1140u, 0x1120u };
    /* Und der volle Aufdeck-Fall (dort ist alles gezeichnet, es darf nichts frei sein). */
    unsigned alle[64];
    int na = 0, zi, nz = re15_map_zone_count();

    blaetter_laden();
    printf("== Keine sichtbare Tuermarke schwebt frei im Blau ==\n");
    CHECK("es gibt ueberhaupt Marken", re15_map_mark_count() > 0);

    CHECK("2F-Anfang: keine frei schwebende Marke (der gemeldete Fehler)",
          fremdes_rect("2F-Anfang", nur_2f_anfang, 2, g_px) == 0);
    CHECK("3F-Kette: keine frei schwebende Marke",
          fremdes_rect("3F-Kette", drei_raeume, 3, g_px) == 0);
    CHECK("2F-Anfang: hoechstens eine Marke neben der gemalten Flaeche (vorher 2)",
          pruefe("2F-Anfang", nur_2f_anfang, 2) <= 1);
    CHECK("3F-Kette: keine Marke neben der gemalten Flaeche",
          pruefe("3F-Kette", drei_raeume, 3) == 0);

    for (zi = 0; zi < nz && na < 64; zi++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(zi);
        int k, doppelt = 0;
        if (!zn) continue;
        for (k = 0; k < na; k++) if (alle[k] == zn->room) { doppelt = 1; break; }
        if (!doppelt) alle[na++] = zn->room;
    }
    CHECK("alles betreten: keine frei schwebende Marke",
          fremdes_rect("alles betreten", alle, na, g_px) == 0);
    CHECK("alles betreten: hoechstens fuenf Marken neben der gemalten Flaeche (vorher 5)",
          pruefe("alles betreten", alle, na) <= 5);

    re15_map_visited_reset();
    return g_fail;
}
