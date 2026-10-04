/* probe_r35_inhalt_karte.c — MESS-WERKZEUG (kein add_test, Runde 35 Spur F): schreibt eine
 * PSX-Speicherkarte mit EINEM Spielstand an frei gewaehlter Stelle, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg dort startet (Muster probe_r34n_f_karte.c).
 *
 * Aufruf: probe_r35_inhalt_karte <kartendatei> <raum-hex> <x> <z> <rot> [bit:<n>]...
 *   bit:<n>  setzt Bank-9-Bit n (z.B. bit:80 = Memory Card ROOM1010 genommen)
 * Inventar = Einsatzausruestung (re15_inv_load_briefing).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 6) {
        printf("Aufruf: %s <karte> <raum-hex> <x> <z> <rot> [bit:<n>]...\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    re15_inv_load_briefing();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    pl->x = atoi(argv[3]); pl->z = atoi(argv[4]); pl->rot_y = (int16_t)atoi(argv[5]);
    for (int a = 6; a < argc; a++)
        if (strncmp(argv[a], "bit:", 4) == 0) re15_game_flag_set(9, atoi(argv[a] + 4), 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  R35F") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    uint16_t rr = 0;
    if (re15_memcard_load(path, 0, &back) != 0 || re15_savedata_restore(&back, &rr) != 0) {
        printf("FAIL: Ruecklesen\n");
        return 1;
    }
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d) rot %d\n", path, (unsigned)rr,
           (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
           (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y);
    return rr == (uint16_t)room ? 0 : 1;
}
