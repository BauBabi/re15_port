/* probe_r39_elzazombies_karte.c — MESS-WERKZEUG (kein add_test), Runde 39 Spur Q "elzazombies".
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in einem Raum an einer Stelle MIT Hoehe und Kamera-Schnitt
 * (ROOM1061 hat drei Etagen: die 10C1-Tuer liegt auf Etage 4 = y -7200, Door_aot_set ROOM10C1 @0x00E42 Spawn y
 * `e0 e3`), damit die echte exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 am LADE-Weg startet.
 * Muster: probe_r36_elza_karte.c (Figur aus der RDT-Variante, xxx1 = Elza 4). Der Spielstand traegt y und den Schnitt
 * (re15_savedata_capture: player_y, camera_cut = g_scd.cam_id), das Band leitet der Restore aus y ab.
 * Aufruf: probe_r39_elzazombies_karte <kartendatei> <raum-hex> <x> <y> <z> <gier> <cut> [bank:bit ...] [give:id]
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 8) { printf("Aufruf: karte raum-hex x y z gier cut [bank:bit ...] [give:id]\n"); return 2; }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    g_current_room_id = (int)room;
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[3]); pl->y = atoi(argv[4]); pl->z = atoi(argv[5]); pl->rot_y = (int16_t)atoi(argv[6]);
    g_scd.cam_id = (uint8_t)atoi(argv[7]);
    for (int a = 8; a < argc; a++) {
        if (strncmp(argv[a], "give:", 5) == 0) {
            long id = strtol(argv[a] + 5, NULL, 0);
            if (id <= 0 || id > 255 || re15_inv_grant((uint8_t)id, 1) != 0) { printf("FAIL: '%s'\n", argv[a]); return 2; }
            continue;
        }
        char *c = strchr(argv[a], ':'); if (!c) { printf("FAIL: '%s'\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)strtol(argv[a], NULL, 0), (uint8_t)strtol(c + 1, NULL, 0), 1);
    }
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "ELZA  R39Q  ") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d (Elza=4), Lage (%d,%d,%d) Gier %d Cut %u\n", path, (unsigned)rr,
           (int)g_gameflow.character, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (unsigned)back.camera_cut);
    return (rr == (uint16_t)room && g_gameflow.character == ((room & 1u) ? 4 : 0)) ? 0 : 1;
}
