/* Uebersetzen (Git-Bash, gegen das Bauverzeichnis des Arbeitsbaums; KEIN Projekt-Bau):
 *   WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c/re15_port
 *   PATH=/c/msys64/mingw64/bin:$PATH gcc -std=c11 -O1 -DRE15_PLATFORM_PC=1 -I$WT/include <datei>.c \
 *     -L$WT/build_r34_c/tests -L$WT/build_r34_c/engine -lre15_test_support -lre15_engine -lre15_test_support -lm \
 *     -o <datei>.exe
 */
/* Mess-Werkzeug (Scratchpad, Nachbesserung M1 Spur C): Speicherkarte mit EINEM Spielstand im Raum
 * <raum-hex> an (x,z,rot) — damit die echte exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
 * RE15_CARD_SLOT=0 den BOOT-/LADE-Pfad in diesen Raum nimmt (pc_load_room_esp VOR main00).
 * Muster: tests/unit/probe_r30_granate_karte.c (ohne Raumbeschraenkung, ohne Flags). */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x2000u;
    int x = (argc > 3) ? atoi(argv[3]) : -26450, z = (argc > 4) ? atoi(argv[4]) : 10250;
    int rot = (argc > 5) ? atoi(argv[5]) : 0;
    scd_vm_init(); re15_actor_init(); re15_aot_init();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->y = 0; pl->z = z; pl->rot_y = (int16_t)rot;
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  MESS") != 0) { printf("FAIL: schreiben\n"); return 1; }
    re15_savedata_t back; uint16_t rr = 0;
    if (re15_memcard_load(path, 0, &back) != 0 || re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: lesen\n"); return 1; }
    printf("Karte %s: Raum 0x%04X Spieler (%d,%d,%d) rot %d\n", path, (unsigned)rr,
           g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].y, g_actors[RE15_ACTOR_SLOT_PLAYER].z, rot);
    return rr == (uint16_t)room ? 0 : 1;
}
