/* probe_r38_rolltor20a0_karte.c — MESS-WERKZEUG (kein add_test) fuer integration_r38_rolltor20a0.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in ROOM20A0 (Leon) bzw. ROOM20A1 (Elza) — Muster
 * probe_r30_granate_karte.c —, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg (Titel -> CONTINUE -> Karte) in den Raum kommt, nicht per Debug-Sprung.
 *
 * Lage: unten vor der Leiter (Door_aot_set slot 1 @0x01766, Band 2, Rechteck x 16000..17000 z -21550..-20550),
 * (16500,-3600,-21050) Blick 3072, Kamera-Cut 2 (RVD: Zone 11 "3 -> 2" x 16500..18000 z -28000..-20000 enthaelt die
 * Stelle; die Anker-Zonen der Cuts 0..3 decken den unteren Gang). Figur: Leon (character 0) fuer 20A0, Elza
 * (character 4 = Elza-Bit, re15_gameflow.h) fuer 20A1.
 *
 * Aufruf: probe_r38_rolltor20a0_karte <kartendatei> <raum-hex 20A0|20A1>
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
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x20A0u;
    if (room != 0x20A0u && room != 0x20A1u) {
        printf("FAIL: Raum 0x%04X ist nicht ROOM20A0/20A1\n", room);
        return 2;
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    g_gameflow.character = (room == 0x20A1u) ? 4 : 0;
    g_scd.cam_id = 2;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 16500; pl->y = -3600; pl->z = -21050; pl->rot_y = 3072;

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, room == 0x20A1u ? "ELZA  20A1" : "LEON  20A0") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    printf("Karte %s: Raum 0x%04X Figur %u Lage (%d,%d,%d) Blick %d Cut %u\n", path, (unsigned)back.room,
           (unsigned)back.character, (int)back.player_x, (int)back.player_y, (int)back.player_z,
           (int)back.player_rot, (unsigned)back.camera_cut);
    if (back.room != (uint16_t)room || back.character != g_gameflow.character || back.player_x != 16500 ||
        back.player_z != -21050 || back.camera_cut != 2) {
        printf("FAIL: Ruecklesen weicht ab\n");
        return 1;
    }
    return 0;
}
