/* probe_r35_entladen_karte.c — MESS-WERKZEUG (kein add_test) fuer den Lade-Lauf des Entladen-Pins.
 *
 * Runde 35 Spur I (analysis/befunde_runde35/I_entladen.md, Lauf C): schreibt eine PSX-Speicherkarte
 * mit EINEM Spielstand am Eintrittspunkt des Raums (re15_room_spawns, Cut des Eintritts), damit die
 * echte exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 dort startet — und nach
 * dem Tod noch einmal GENAU DORT (gleicher Raum, gleicher Cut). Genau dieser Fall braucht den
 * Generationsvergleich am PRI-Riegel in main.c: (Raum,Cut) bleibt gleich, die Maskenliste wurde
 * am Spielende entladen und muss im ersten Spielbild neu abgeleitet werden.
 *
 * Aufruf: probe_r35_entladen_karte <kartendatei> <raum-hex> [<x> <y> <z> <cut>]
 *   ohne Lage: Eintrittspunkt aus re15_room_spawns; mit Lage: genau diese Stelle + dieser Cut
 *   (Lauf C nimmt den Sprungpunkt des Debug-Menues ROOM1020 (-26000,0,-8700) Cut 0 = 24 Masken).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_room_list.h"
#include "re15_room_spawns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1020u;

    int idx = -1;
    for (int i = 0; i < (int)(sizeof re15_room_ids / sizeof re15_room_ids[0]); i++)
        if (re15_room_ids[i] == room) { idx = i; break; }
    if (idx < 0) { printf("FAIL: Raum 0x%04X nicht in re15_room_ids\n", room); return 2; }
    const re15_room_spawn_t *sp = &re15_room_spawns[idx];

    int x = sp->x, y = sp->y, z = sp->z, cut = sp->cut;
    if (argc > 6) { x = atoi(argv[3]); y = atoi(argv[4]); z = atoi(argv[5]); cut = atoi(argv[6]); }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = room;
    g_scd.cam_id = (uint8_t)cut;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = sp->yaw;

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  ENTLADEN") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X Cut %u Lage (%d,%d,%d)\n", path, (unsigned)rr,
           (unsigned)back.camera_cut, x, y, z);
    if (rr != (uint16_t)room || back.camera_cut != (uint8_t)cut) { printf("FAIL: Ruecklesen\n"); return 1; }
    return 0;
}
