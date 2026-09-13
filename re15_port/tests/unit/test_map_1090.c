/* test_map_1090.c — ROOM1090 steht auf der Karte: Hof auf Blatt 2, Dach auf Blatt 3.
 *
 * BEFUND (Nutzer 2026-09-13): "Fuer ROOM 1090 fehlt noch das Kartenstueck...."
 *
 * URSACHE, gemessen: s_map_floors fuehrte vier Etagenzeilen fuer 0x1090/0x1091, s_map_zones
 * aber keine einzige. Damit lieferte zone_index_at -1, re15_map_zone_at NULL - und Marker,
 * Besucht-Bit und Rechteck-Faerbung fielen zusammen aus.
 *
 * WAS JETZT DRIN STEHT, und woher es kommt:
 *  - Heimatblatt 2: der Stage-1-Karteninit FUN_8004b568 schickt Raumindex (0x1090>>4)&0xFF
 *    = 9 ueber die Kette 0..11, deren Schwanz @0x8004b680 `j 0x8004b888` mit
 *    `ori v0,zero,0x2` im Verzoegerungsschlitz steht.
 *  - Rechteck 5 auf Blatt 2: es traegt das einzige gemalte Tuerblatt bei (191,66), und die
 *    Tuer ROOM1050 -> ROOM1090 projiziert mit 1050s ausgelieferter Massstabszeile durch
 *    FUN_800473f8 (@0x8004741c-0x80047528) auf (191,70).
 *  - Rechteck 7 auf Blatt 3 fuer die Dachebene: von Blatt 3s zehn Rechtecken sind nur 2 und
 *    7 frei, und rect7 grenzt im 2F-Grundriss an rect6 (ROOM1100) und ueberlappt rect9
 *    (ROOM10F0) - genau die beiden Nachbarn der oberen Ebene.
 *  - Zonen-Nummer 22: die 37 war belegt (ROOM2000, Blatt 6), und weil die zid das
 *    Besucht-Bit ist, liessen sich zwei Orte dasselbe Bit teilen.
 */
#include "re15_room.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

int main(void)
{
    printf("=== ROOM1090 auf der Karte (Nutzer 2026-09-13) ===\n");

    /* (1) Beide Zeilen existieren - vorher gab es fuer 0x1090 gar keine. */
    const re15_map_zone_t *hof  = re15_map_zone_fuer(0x1090, 0, 2);
    const re15_map_zone_t *dach = re15_map_zone_fuer(0x1090, 0, 3);
    CHECK("ROOM1090 hat eine Zeile auf Blatt 2 (Hof)", hof != NULL);
    CHECK("ROOM1090 hat eine Zeile auf Blatt 3 (Dach)", dach != NULL);
    if (!hof || !dach) { printf("=== FEHLGESCHLAGEN ===\n"); return 1; }

    /* (2) Auf den Rechtecken, die die Kunst vorgibt. */
    CHECK("Hof liegt auf Blatt 2 Rechteck 5 (das Tuerblatt zu ROOM1050)",
          hof->page == 2 && hof->rect == 5);
    CHECK("Dach liegt auf Blatt 3 Rechteck 7 (grenzt an ROOM1100 und ROOM10F0)",
          dach->page == 3 && dach->rect == 7);

    /* (3) Dieselbe Zonen-Nummer - ein Ort, ein Besucht-Bit; das Dach haengt am Etagen-Bit. */
    CHECK("beide Zeilen teilen die Zonen-Nummer (ein Ort = ein Besucht-Bit)",
          hof->zid == dach->zid);
    CHECK("die Gast-Zeile ist als Etage markiert (sonst gilt sie zu frueh als bekannt)",
          hof->etage == 0 && dach->etage == 1);

    /* (4) DIE ZONEN-NUMMER GEHOERT NIEMAND ANDEREM. Genau daran ist der erste Versuch
     *     gescheitert: zid 37 gehoerte schon ROOM2000, und weil die zid das Besucht-Bit
     *     ist, schwebten auf Blatt 6 fuenf Tuermarken frei im Blau. */
    {
        int fremd = 0;
        for (unsigned raum = 0x1000; raum <= 0x6FFF; raum += 0x10) {
            if ((raum & ~1u) == 0x1090) continue;
            for (unsigned pg = 0; pg <= 12; pg++) {
                for (int idx = 0; idx < 4; idx++) {
                    const re15_map_zone_t *z = re15_map_zone_fuer(raum, idx, pg);
                    if (z && z->zid == hof->zid) {
                        fprintf(stderr, "   ROOM%04X idx %d Blatt %u traegt zid %u ebenfalls\n",
                                raum, idx, pg, (unsigned)z->zid);
                        fremd++;
                    }
                }
            }
        }
        CHECK("die Zonen-Nummer von ROOM1090 gehoert keinem anderen Raum", fremd == 0);
    }

    /* (5) Die Weltbox deckt BEIDE Ebenen ab - sonst findet zone_index_at den Spieler auf
     *     dem Dach nicht. Die Zahlen sind die Huelle aller 61 SCA-Zellen (Baender 0/1/2
     *     unten, 4/5/6 oben). */
    CHECK("die Weltbox umfasst den Hof (Band 1, z um 4200)",
          hof->wx0 <= -10100 && hof->wx1 >= -10100 && hof->wz0 <= 4200 && hof->wz1 >= 4200);
    CHECK("die Weltbox umfasst das Dach (Band 5, Eintritt aus ROOM10F0 bei (-13600,1300))",
          hof->wx0 <= -13600 && hof->wx1 >= -13600 && hof->wz0 <= 1300 && hof->wz1 >= 1300);
    CHECK("und die Band-6-Tuer nach ROOM1100 bei (-5820,-18690)",
          hof->wx0 <= -5820 && hof->wx1 >= -5820 && hof->wz0 <= -18690 && hof->wz1 >= -18690);

    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
