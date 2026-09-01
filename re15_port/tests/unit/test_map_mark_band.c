/* Karte: eine Marke gehoert auf das Blatt IHRES Bandes, nicht auf das der Zone.
 *
 * Nutzer 2026-09-01: "Bei ROOM 1170 zeigt er die Tuer, die eigentlich fuer die Etage
 * unten gedacht ist, bereits bei der Roof-Etage an."
 *
 * ROOM1170s zweiter Bereich (Zone 1) ist auf ZWEI Blaettern gezeichnet: Seite 5 Rect 0
 * (ROOF) und Seite 4 Rect 3 (3F). Er fuehrt drei Tueren, und das Band steht in ihrem
 * Aot_set-Datensatz (pc[4] = obj[0x82]; das Original gattet die Interaktion darauf,
 * FUN_8002bd44 @0x8002bf38):
 *     Band 0, Welt(-21230,-17925) -> ROOM1130   (Seite 4 = 3F)
 *     Band 0, Welt(-21600,-27005) -> ROOM1140   (Seite 4 = 3F)
 *     Band 4, Welt(-11065,-27850) -> ROOM1170   (Seite 5 = ROOF)
 * und vier Treppen-Datensaetze mit den Baendern 0, 2, 2, 4.
 *
 * Bis v0.3.80 legte der Generator ALLE Marken einer Zone auf die Vorgabeseite der Zone
 * (hier ROOF) und kopierte sie von dort auf die Etagenblaetter — die beiden 3F-Tueren
 * standen damit auf dem Dach. Dieser Pin haelt fest, dass das ROOF-Blatt keine
 * Band-0-Marke mehr traegt und dass die Treppe auf BEIDEN Etagen erscheint, die sie
 * verbindet (sie liegt als zwei Datensaetze vor, einer je Ende, jeder mit eigenem Band). */
#include <stdio.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    int n, k, roof_tueren = 0, roof_treppen = 0, f3_treppen = 0, f3_tueren = 0;

    printf("=== Karte: Marke folgt ihrem Band (ROOM1170 ROOF gegen 3F) ===\n");
    re15_map_visited_reset();
    re15_map_debug_reveal_page(5);
    re15_map_debug_reveal_page(4);

    n = re15_map_mark_count();
    for (k = 0; k < n; k++) {
        int pg, r, mx, my, kind;
        if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
        if (pg == 5 && r == 0) { if (kind >= 4) roof_treppen++; else roof_tueren++; }
        if (pg == 4 && r == 3) { if (kind >= 4) f3_treppen++;  else f3_tueren++;  }
    }
    printf("  [ROOF Seite 5 Rect 0] %d Tueren, %d Treppen\n", roof_tueren, roof_treppen);
    printf("  [3F   Seite 4 Rect 3] %d Tueren, %d Treppen\n", f3_tueren, f3_treppen);

    /* Die beiden Band-0-Tueren duerfen NICHT mehr auf dem Dach stehen. Auf dem Dach
     * liegt von diesem Bereich nur die Band-4-Tuer — und die wird mit ROOM1170s erstem
     * Bereich zu EINEM Symbol zusammengefuehrt und dort gezeichnet (Seite 5 Rect 1),
     * also traegt Rect 0 gar keine Tuermarke mehr. */
    CHECK("das ROOF-Blatt traegt keine Tuer des unteren Bereichs mehr", roof_tueren == 0);
    /* Die Treppe verbindet ROOF und 3F und muss auf BEIDEN erscheinen. */
    CHECK("die Treppe erscheint auf dem ROOF-Blatt", roof_treppen > 0);
    CHECK("die Treppe erscheint auch auf dem 3F-Blatt", f3_treppen > 0);
    /* Gegenprobe, dass der Bereich auf 3F ueberhaupt gezeichnet wird. */
    CHECK("Seite 4 Rect 3 ist bekannt (der Bereich wird auf 3F gezeichnet)",
          re15_map_rect_state(4, 3) != RE15_MAP_RECT_UNMAPPED);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
