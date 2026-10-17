/* probe_r38_npckante_karte.c — MESS-WERKZEUG (kein add_test), Runde 38 Spur M "npckante".
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand, damit die echte exe ueber RE15_CONTINUE_TEST=1
 * RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 am LADE-Weg (Raum-Init main00 + sub00 mit den Sce_em_set) startet. Anders als
 * probe_r36_elza_karte (y = 0) traegt der Stand auch die HOEHE des Spielers (das Band leitet re15_savedata_restore
 * aus y ab) und den Kamera-Cut (re15_savedata_t.camera_cut; ROOM10B1 sub02 wartet mit `While(Cmp(0x0A != 2))`
 * @0x01888 auf Cut 2, bevor John zur Kante laeuft).
 * Figur aus der RDT-Variante (FUN_800396fc @0x800397ec): xxx1 = Elza (4), sonst Leon (0).
 *
 * Aufruf: probe_r38_npckante_karte <kartendatei> <raum-hex> <x> <y> <z> <gier> <cut> [bank:bit ...]
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 8) { printf("Aufruf: karte raum-hex x y z gier cut [bank:bit ...]\n"); return 2; }
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
    pl->x = atoi(argv[3]); pl->y = atoi(argv[4]); pl->z = atoi(argv[5]);
    pl->rot_y = (int16_t)atoi(argv[6]);
    int cut = atoi(argv[7]);
    g_scd.cam_id = (uint8_t)cut;
    for (int a = 8; a < argc; a++) {
        char *c = strchr(argv[a], ':'); if (!c) { printf("FAIL: '%s'\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)strtol(argv[a], NULL, 0), (uint8_t)strtol(c + 1, NULL, 0), 1);
    }
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "NPCKANTE R38") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d, Lage (%d,%d,%d) Band %d, Cut %d\n", path, (unsigned)rr,
           (int)g_gameflow.character, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->floor, (int)back.camera_cut);
    return (rr == (uint16_t)room && (int)back.camera_cut == cut) ? 0 : 1;
}
