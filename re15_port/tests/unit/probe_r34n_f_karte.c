/* probe_r34n_f_karte.c — MESS-WERKZEUG (kein add_test, Runde 34 Nacht, Spur F): schreibt eine
 * PSX-Speicherkarte mit EINEM Spielstand vor einer der zwei Leichen, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg in ROOM1110/1111 bzw. ROOM1230/1231 startet (Muster probe_r30_granate_karte.c).
 *
 * Aufruf: probe_r34n_f_karte <kartendatei> <raum-hex> [genommen]
 *   genommen   Bank-9-Bit der Leiche gesetzt ((9,61) in 1110/1111, (9,62) in 1230/1231) — der
 *              Stand NACH der Annahme: das Untersuchen muss den kurzen Text ohne Modal zeigen.
 * Spielerlage = der gemessene Standplatz vor der Leiche mit Blick +X (Ist-Lauf ist1110_b:
 * RE15_PLAYER_POS (10580,2650) -> nach dem Kollisionsschub (10448,2646); 1230: (3280,25900)).
 * Der Vorwaerts-620-Punkt liegt dann im Rechteck des Leichen-Platzes (ROOM1110 main00 @0x00AEE
 * (10700,2150, 1000x1000); ROOM1230 main00 @0x00D52 (3400,25400, 1000x1000)).
 * Inventar = Einsatzausruestung (re15_inv_load_briefing, u.a. H. Gun Bullets 50).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_leiche.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1110u;
    int genommen = (argc > 3 && strcmp(argv[3], "genommen") == 0);
    unsigned basis = room & 0xFFF0u;
    if (basis != RE15_LEICHE_1110_RAUM && basis != RE15_LEICHE_1230_RAUM) {
        printf("FAIL: Raum 0x%04X hat keine Leiche der Spur F\n", room);
        return 2;
    }
    uint8_t bit = (basis == RE15_LEICHE_1110_RAUM) ? RE15_LEICHE_1110_BIT : RE15_LEICHE_1230_BIT;

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    re15_inv_load_briefing();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0; pl->rot_y = 0;
    if (basis == RE15_LEICHE_1110_RAUM) { pl->x = 10448; pl->z = 2646; }
    else                                { pl->x = 3280;  pl->z = 25900; }
    if (genommen) re15_game_flag_set(9, bit, 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  LEICHE") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    /* Gegenprobe: zurueckgelesen muessen Raum und Bit stimmen. */
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(9, bit, 0);
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int fb = re15_game_flag_get(9, bit) ? 1 : 0;
    int menge = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == RE15_LEICHE_ITEM) menge += g_inv.slots[i].qty;
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d), Flag(9,%d)=%d, H. Gun Bullets %d\n",
           path, (unsigned)rr, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
           (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z, (int)bit, fb, menge);
    if (rr != (uint16_t)room || fb != genommen) { printf("FAIL: Ruecklesen\n"); return 1; }
    return 0;
}
