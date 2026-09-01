/* Karte: ROOM1120s Zeichnung liegt gegenueber ihren Weltkoordinaten um 180 Grad gedreht.
 *
 * Nutzer 2026-09-01: "Im Room 1120 lande ich wieder ganz komisch ploetzlich auf der
 * falschen Seite, wenn ich vom ROOM 1130 aus rein komme."
 *
 * GEMESSEN: die Tuer nach ROOM1130 liegt in ROOM1120s eigener Bbox bei Anteil
 * (0.11, 0.23) - unten links. Zwei voneinander unabhaengige Quellen sagen aber oben
 * rechts:
 *   * die KACHEL malt dieselbe Tuer bei (150,126) an der Ostwand von Rect 5,
 *   * ROOM1130 zeichnet denselben Durchgang bei (156,125).
 * Um 180 Grad gedreht landet sie bei (155,128): 5 px vom gemalten Symbol, 3 px von
 * ROOM1130s Position. Die Gegenhypothese "falsches Rechteck" ist geprueft und
 * verworfen - kein Rechteck der Seite 4 kommt in normaler Lage naeher als 18 px an den
 * Nachbarn.
 *
 * Moeglich ist das, weil jeder RDT-Raum sein EIGENES lokales Koordinatensystem hat.
 * Fuer die 33 Raeume mit ausgelieferter Zeile @0x800768b0 legt die Zeile die Abbildung
 * fest (sx/sy per `lhu`, positiv); fuer die 39 Stub-Raeume legt sie nichts fest. */
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    const re15_map_zone_t *z;
    int16_t mx, my;
    /* Die Tuer ROOM1120 -> ROOM1130, Weltmitte aus dem RDT (Aot_set 0x3b). */
    const int32_t TUER_X = -8450, TUER_Z = -2900;
    /* Seite 4 Rect 5 = ROOM1120s Zeichnung, aus der Rechteck-Tabelle @0x80076840. */
    const int RX = 120, RY = 119, RW = 40, RH = 40;

    printf("=== Karte: Orientierung der Zeichnung (ROOM1120) ===\n");
    z = re15_map_zone_at(0x1120, 0, 0);
    if (!z) { printf("  FAIL: ROOM1120 hat keine Zone\n"); return 1; }
    CHECK("ROOM1120 liegt auf Seite 4 Rect 5", z->page == 4 && z->rect == 5);
    CHECK("ROOM1120 traegt keine ausgelieferte Massstabszeile (Stub)",
          z->sx == 0 && z->sy == 0);
    CHECK("ROOM1120s Zeichnung ist als 180 Grad gedreht vermerkt",
          z->flip_x == 1 && z->flip_z == 1);

    if (!re15_map_zone_marker(z, TUER_X, TUER_Z, RX, RY, RW, RH, &mx, &my)) {
        printf("  FAIL: keine Projektion\n"); return 1;
    }
    printf("  [Projektion] Tuer -> ROOM1130 bei Welt(%d,%d) landet auf (%d,%d)\n",
           (int)TUER_X, (int)TUER_Z, (int)mx, (int)my);
    /* Das gemalte Symbol liegt bei (150,126), ROOM1130 zeichnet den Durchgang bei
     * (156,125). Die Projektion muss in deren Naehe liegen, nicht in der
     * gegenueberliegenden Ecke (124,149). */
    CHECK("die Tuer landet an der OSTwand, nicht unten links",
          mx >= 145 && my <= 135);
    { int d = abs((int)mx - 150) + abs((int)my - 126);
      printf("  [Abstand] zum gemalten Symbol (150,126): %d px (Manhattan)\n", d);
      CHECK("Abstand zum gemalten Symbol hoechstens 8 px", d <= 8); }
    { int d = abs((int)mx - 156) + abs((int)my - 125);
      printf("  [Abstand] zu ROOM1130s Position (156,125): %d px (Manhattan)\n", d);
      CHECK("Abstand zu ROOM1130s Position hoechstens 8 px", d <= 8); }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
