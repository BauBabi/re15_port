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

    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
