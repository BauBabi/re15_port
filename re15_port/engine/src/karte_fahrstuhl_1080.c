/* karte_fahrstuhl_1080.c — Runde 35 Spur G: Kartenblatt der Fahrstuhlkabine ROOM1080.
 * Belege und Herleitung: include/re15_karte_fahrstuhl.h, analysis/befunde_runde35/G_karte.md B5. */
#include "re15_karte_fahrstuhl.h"
#include "re15_scd.h"   /* re15_game_flag_get */

int re15_karte_fahrstuhl_blatt(unsigned room)
{
    if ((room & ~1u) != 0x1080u) return -1;
    /* Genau ein Bit ist gesetzt (die Etagenraeume loeschen die anderen beiden, ROOM10C0
     * @0x0FFA/@0x1014, ROOM1120 @0x0D88/@0x0DA2). */
    if (re15_game_flag_get(3, 54)) return 2;   /* Kabine 1F: ROOM1040 @0x15D6 -> Blatt 2 */
    if (re15_game_flag_get(3, 55)) return 3;   /* Kabine 2F: ROOM10C0 @0x0FEE -> Blatt 3 */
    if (re15_game_flag_get(3, 56)) return 4;   /* Kabine 3F: ROOM1120 @0x0D6C -> Blatt 4 */
    return -1;
}
