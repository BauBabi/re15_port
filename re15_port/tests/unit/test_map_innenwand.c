/* Karte: INNENWAENDE sind die Ausnahme, nicht die Regel.
 *
 * NUTZER-BEFUND 2026-09-07: "DU hast jetzt MASSENWEISE quatsch Waende eingezeichnet
 * die es nicht gibt ... glueckwunsch".
 *
 * WAS PASSIERT WAR. Die SCA-Zellen eines RDT sind die WAENDE (re15_collision.c: der
 * Spieler laeuft im band-freien Komplement; an 3727 Nutzer-Standorten zu 97,1 %
 * bestaetigt). Daraus habe ich gefolgert, jede solide Typ-1-Zelle sei eine Wand der
 * KARTE - und 355 Linien auf 70 Rechtecke gemalt, bis zu 27 in EINEM Raum.
 *
 * Der Fehlschluss: Kollisionshindernis != Wand. Ein Tisch, eine Saeule, eine Kiste
 * sind genauso solide Typ-1-Zellen. Der Filter der ersten Fassung ("beidseits liegt
 * gemalte Flaeche") trifft auf ein Moebelstueck mitten im Raum exakt genauso zu.
 *
 * Und: der KUENSTLER malt seine Innenwaende selbst in die Kachel (Palettenindex 4).
 * Alles, was der Port zusaetzlich zeichnet, ist per Definition eine Wand, die das
 * Original NICHT hat. Dafuer braucht es einen eigenen Beleg.
 *
 * DER BELEG: die Zelle TRENNT zwei Absetzpunkte von SELBST-Tueren desselben Raums.
 * Eine Selbst-Tuer existiert nur, weil man von A nach B nicht laufen kann; liegt eine
 * Wandzelle zwischen ihren beiden Enden, ist sie genau diese Grenze.
 *      ROOM1110: 12 Wandzellen, 4 Selbst-Tuer-Spawns  -> 2 Waende
 *      ROOM1130: 10 Wandzellen, 0 Selbst-Tueren       -> 0
 *      ROOM1120/1140/10E0/10D0: 12..22 Zellen, 0      -> 0
 *
 * ⛔ KEIN TEST HAT DIE 355 BEMERKT - das ist der eigentliche Befund. Diese Pruefung
 * schliesst die Luecke: sie deckelt die GESAMTZAHL und verlangt, dass jede Wand in
 * einem Raum steht, der ueberhaupt Selbst-Tueren hat.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    int n = re15_map_wall_count(), i;
    int je_rect[16][64];
    int schlimmster = 0, sr_pg = -1, sr_r = -1;
    char t[220];

    memset(je_rect, 0, sizeof je_rect);
    /* re15_map_wall_get zeigt nur Waende BESUCHTER Zonen - ohne dieses Setzen
     * liefert es nichts und der Haken pruefte eine leere Menge. */
    re15_map_visited_reset();
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        if (zn) re15_map_visited_mark(zn->room);
    }
    printf("=== Karte: Innenwaende sind die Ausnahme ===\n");
    printf("  %d Innenwand-Linien insgesamt\n", n);

    for (i = 0; i < n; i++) {
        int pg, r, x0, y0, x1, y1;
        if (!re15_map_wall_get(i, &pg, &r, &x0, &y0, &x1, &y1)) continue;
        printf("     Blatt %2d Rect %2d  (%3d,%3d)-(%3d,%3d)\n", pg, r, x0, y0, x1, y1);
        if (pg >= 0 && pg < 16 && r >= 0 && r < 64) {
            je_rect[pg][r]++;
            if (je_rect[pg][r] > schlimmster) {
                schlimmster = je_rect[pg][r]; sr_pg = pg; sr_r = r;
            }
        }
        /* Eine Wand von einem Punkt gibt es nicht. */
        snprintf(t, sizeof t, "Wand %d ist eine Linie, kein Punkt", i);
        CHECK(t, x0 != x1 || y0 != y1);
        /* Und sie ist achsenparallel - die Kachel kennt nur solche. */
        snprintf(t, sizeof t, "Wand %d ist achsenparallel", i);
        CHECK(t, x0 == x1 || y0 == y1);
    }

    /* ⛔ DIE ZAHL IST DER RIEGEL. Sie war 355 und ist 10; die Grenze liegt bei 24,
     * also weit ueber dem heutigen Stand und weit unter dem Ausrutscher. Wer hier
     * anschlaegt, hat wieder Moebel fuer Waende gehalten - NICHT die Grenze heben,
     * sondern den Beleg pruefen (trennt die Zelle zwei Selbst-Tuer-Spawns?). */
    snprintf(t, sizeof t, "hoechstens 24 Innenwaende im ganzen Spiel (Ausrutscher war 355) - sind %d", n);
    CHECK(t, n <= 24);

    /* Kein Raum ist ein Labyrinth: das Original malt seine Innenwaende selbst. */
    snprintf(t, sizeof t,
             "hoechstens 6 Innenwaende je Rechteck - schlimmstes ist Blatt %d Rect %d mit %d",
             sr_pg, sr_r, schlimmster);
    CHECK(t, schlimmster <= 6);

    /* ABDECKUNG: ohne Wandtabelle prueft der Haken nichts. */
    CHECK("es gibt ueberhaupt Innenwaende zu pruefen (ABDECKUNG)", n > 0);

    /* Die zwei aus ROOM1110 muessen dabei sein - sie sind mit fuenf F9-Marken des
     * Nutzers eingegrenzt (Marken 2/3 oberhalb, 4/5 unterhalb der Trennwand). */
    {
        int senk = 0, quer = 0;
        for (i = 0; i < n; i++) {
            int pg, r, x0, y0, x1, y1;
            if (!re15_map_wall_get(i, &pg, &r, &x0, &y0, &x1, &y1)) continue;
            if (pg != 3 || r != 5) continue;
            if (x0 == x1 && abs(x0 - 188) <= 1) senk = 1;
            if (y0 == y1 && abs(y0 - 133) <= 2) quer = 1;
        }
        CHECK("ROOM1110 hat die senkrechte Wand bei x~188", senk);
        CHECK("ROOM1110 hat die Trennwand bei y~133 (Nutzer: y=134)", quer);
    }

    /* ⛔ KEINE "INNENWAND" LIEGT AUF EINER WAND, DIE DIE KACHEL SCHON MALT.
     * NUTZER-BEFUND 2026-09-27 (Runde 30): "Roof ist irgendwie die Wand unten blau".
     * Die Tabellenzeile { 5, 1, 148, 155, 182, 155 } lag mit 35 von 35 Punkten auf
     * Kachel-Index 4 - der vom Kuenstler GEMALTEN Suedwand von Blatt 5 rect 1
     * (DATA/MAP06.PIX, Kachelzeile v=86 ab @Datei 0x2B00); die Zeile darunter traegt
     * Index 0. Der Tabellenkopf verlangt "nur wo BEIDSEITS Raum liegt"; die
     * Aussenwand-Regel des Generators war seit dem Selbst-Tuer-Beleg wirkungslos.
     * Index 4 = Wandlinie der Kartenpalette (TEX.TIM CLUT-Zeile 21 Eintrag 4 @Datei
     * 0x055C = 0x5AD6).
     * KEIN SCHWELLWERT: 100 % traf von den zehn Zeilen nur diese eine, die naechste
     * traegt 2 von 9 Punkten auf Index 4. Dossier karten-marken.md §2.9 / §4a. */
    {
        static unsigned char roh[256 * 128];
        int ganz_auf_wand = 0, geprueft = 0;
        for (i = 0; i < n; i++) {
            int pg, r, x0, y0, x1, y1, rx, ry, rw, rh, u, v, x, y, punkte = 0, auf4 = 0;
            char pfad[700]; FILE *f;
            if (!re15_map_wall_get(i, &pg, &r, &x0, &y0, &x1, &y1)) continue;
            if (!re15_map_rect_geometry((unsigned)pg, (unsigned)r, &rx, &ry, &rw, &rh)) continue;
            if (!re15_map_rect_uv((unsigned)pg, (unsigned)r, &u, &v)) continue;
            snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, pg + 1);
            f = fopen(pfad, "rb");
            if (!f) continue;
            if (fread(roh, 1, sizeof roh, f) != sizeof roh) { fclose(f); continue; }
            fclose(f);
            if (x0 > x1) { int h = x0; x0 = x1; x1 = h; }
            if (y0 > y1) { int h = y0; y0 = y1; y1 = h; }
            for (y = y0; y <= y1; y++)
                for (x = x0; x <= x1; x++) {
                    int tx = (u + (x - rx)) & 255, ty = (v + (y - ry)) & 255, idx;
                    if (x < rx || x >= rx + rw || y < ry || y >= ry + rh) { punkte++; continue; }
                    idx = (tx & 1) ? (roh[ty * 128 + (tx >> 1)] >> 4)
                                   : (roh[ty * 128 + (tx >> 1)] & 15);
                    punkte++;
                    if (idx == 4) auf4++;
                }
            geprueft++;
            printf("     Wand %d Blatt %2d Rect %2d: %d von %d Punkten auf Kachel-Index 4\n",
                   i, pg, r, auf4, punkte);
            if (punkte > 0 && auf4 == punkte) ganz_auf_wand++;
        }
        snprintf(t, sizeof t, "ABDECKUNG: jede Wand gegen ihre Kachel geprueft - %d von %d",
                 geprueft, n);
        CHECK(t, geprueft == n && n > 0);
        snprintf(t, sizeof t, "keine Innenwand liegt zu 100 %% auf Kachel-Index 4 "
                 "(vorher 1: die Dach-Suedwand) - sind %d", ganz_auf_wand);
        CHECK(t, ganz_auf_wand == 0);
    }

    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
