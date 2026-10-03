/* karte_fahrstuhl_1080.c — Runde 35 Spur G: Kartenblatt der Fahrstuhlkabine ROOM1080.
 * Belege und Herleitung: include/re15_karte_fahrstuhl.h, analysis/befunde_runde35/G_karte.md B5. */
#include "re15_karte_fahrstuhl.h"
#include "re15_scd.h"   /* re15_game_flag_get */

/* Spiegel von DAT_800b0fe6 (alter Raum): das Original sichert beim Raumwechsel den
 * bisherigen Raum, bevor es den neuen schreibt - FUN_8001d600 Tuer-Zweig @0x8001d92c
 * `lhu v0,0x0fe2(0x800b)` -> @0x8001d938 `sh v0,0x0fe6(0x800b)` -> @0x8001d95c
 * `sh v0,0x0fe2`. Der Port fuehrt diese Zelle nicht; sie wird hier aus dem Raum
 * nachgefuehrt, den re15_map_zone_update je Bild bzw. am Raumlade-Punkt meldet. */
static unsigned s_raum_jetzt = 0, s_raum_vorher = 0;

void re15_karte_raum_gesehen(unsigned room)
{
    if (room != s_raum_jetzt) { s_raum_vorher = s_raum_jetzt; s_raum_jetzt = room; }
}

int re15_karte_fahrstuhl_blatt(unsigned room)
{
    unsigned vor;
    if ((room & ~1u) != 0x1080u) return -1;
    /* (1) Man betritt die Kabine IMMER aus einem Etagenraum - dort steht sie also. */
    vor = s_raum_vorher & ~1u;
    if (s_raum_jetzt == room) {
        if (vor == 0x1040u) return 2;   /* Seiten-Setzer @0x8004b684: Raum 4  -> Blatt 2 */
        if (vor == 0x10C0u) return 3;   /*                @0x8004b6f8: Raum 12 -> Blatt 3 */
        if (vor == 0x1120u) return 4;   /*                @0x8004b758: Raum 18 -> Blatt 4 */
    }
    /* (2) Sonst die Etage, die das Spiel selbst fuehrt (Bank 3 Bit 54/55/56; genau eines
     * gesetzt - ROOM10C0 loescht 54/56 @0x1008/@0x1022, ROOM1120 54/55 @0x0D86/@0x0DA0). */
    if (re15_game_flag_get(3, 54)) return 2;   /* ROOM1040 @0x15D6 `22 03 36 01` */
    if (re15_game_flag_get(3, 55)) return 3;   /* ROOM10C0 @0x0FEE `22 03 37 01` */
    if (re15_game_flag_get(3, 56)) return 4;   /* ROOM1120 @0x0D6C `22 03 38 01` */
    return -1;
}
