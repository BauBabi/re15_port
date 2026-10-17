/* probe_r38_wasserzombie_karte.c — MESS-WERKZEUG (kein add_test) fuer integration_r38_wasserzombie.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in einem Stage-2-Kanalraum, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet (Muster probe_r30_granate_karte.c).
 *
 * Aufruf: probe_r38_wasserzombie_karte <kartendatei> <raum-hex 2020|2000> [elza]
 *   2020  Spieler auf dem Suedsteg (-9500,-1800,2600), Band 1, Blick +Z — schraeg vor dem Wasserzombie
 *         (-11300,0,300), nicht auf dessen Landeplatz (Koerperstoss FUN_8002aec4)
 *   2000  Spieler auf dem Oststeg (-20300,-1800,-11600), Band 1 — vor den beiden 0x84-Wasserzombies
 *   elza  Spielfigur Elza (ROOMxxx1) — der Raum muss dann 2021/2001 heissen
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
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x2020u;
    int elza = (argc > 3 && strcmp(argv[3], "elza") == 0);
    int32_t x, z; int16_t rot;
    switch (room) {
    case 0x2020u: case 0x2021u: x = -9500;  z = 2600;   rot = 0x400; break;
    case 0x2000u: case 0x2001u: x = -20300; z = -11600; rot = 0x800; break;
    default: printf("FAIL: Raum 0x%04X ist kein Kanalraum (2000/2001/2020/2021)\n", room); return 2;
    }
    if (elza != ((room & 1u) != 0)) { printf("FAIL: Elza <-> ROOMxxx1 passt nicht\n"); return 2; }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    g_gameflow.character = elza ? 4 : 0;          /* PLD-Index: Leon 0, Elza 4 (re15_gameflow.h) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->y = -1800; pl->z = z; pl->rot_y = rot; pl->floor = 1;

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, elza ? "ELZA  KANAL" : "LEON  KANAL") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d,%d) Band %d\n", path, (unsigned)rr,
           g_actors[0].x, g_actors[0].y, g_actors[0].z, (int)g_actors[0].floor);
    if (rr != (uint16_t)room) { printf("FAIL: Ruecklesen Raum\n"); return 1; }
    return 0;
}
