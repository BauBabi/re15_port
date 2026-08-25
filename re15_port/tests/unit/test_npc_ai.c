/* ============================================================================
 *  NPC (Chief Irons, type 0x40, EM040) AI — Wave 1 port probe.
 *
 *  Byte-true 0x8011c5a0 family (RE15_NPC_AI.md). The STAGE1 NPCs are invulnerable
 *  cutscene actors (HP = -1) that idle-pose, walk, look-at, and dialogue. Seeds a
 *  type-0x40 actor and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x8011c6dc): state 0 -> 1 (ACTIVE), HP = -1 (invulnerable), idle clip 2.
 *   (2) IDLE: the NPC holds its position and animates the idle pose (clip 2 loops).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;

    const int NS = 1;
    re15_actor_t *n = &g_actors[NS];
    n->active = 1; n->type = 0x40; n->state = 0; n->x = 1000; n->y = 0; n->z = 1000;

    printf("=== NPC (Chief Irons, type 0x40) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (n->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", n->state); fail = 1; }
    if (n->hp != -1)   { fprintf(stderr, "FAIL(1): NPC must be invulnerable (HP=-1), got %d\n", n->hp); fail = 1; }
    if (n->motion != 2){ fprintf(stderr, "FAIL(1): idle clip 2 expected, got %d\n", n->motion); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d (invulnerable), idle clip=%d\n", n->hp, n->motion);

    /* (2) ESKORTE — bis 2026-08-27 stand hier "idle NPC must stay put".
     *
     * Das war eine Zusicherung auf einen NICHT IMPLEMENTIERTEN Zweig: NPC-State 1 lief im Port
     * in `case 1: default:` und hielt nur die Idle-Pose. Das Original hat dort eine vollstaendige
     * ESKORTE-Maschine (Wurzel 0x8011ce54, siehe re15_npc_escort_tick): Ziel = ein Punkt 1500
     * Einheiten hinter dem Spieler (@0x8011ce78 `ori a0,zero,0x5dc`, Winkel ±0x800 @0x8011ceb0),
     * losgehen bei Distanz >= 1501 (@0x8004f118 `sltiu v0,v0,0x5dd`), stehenbleiben bei < 500
     * (@0x8004f3bc `sltiu v0,v0,0x1f4`), wieder los bei >= 1001 (@0x8004f818 `sltiu v0,v0,0x3e9`).
     * Der Nutzer-Befund war genau das: "ausserdem folgt mir ada nicht nach der cutscene".
     *
     * Die Wache prueft deshalb jetzt die SCHLEIFE statt der Regungslosigkeit — das ist schaerfer:
     * sie faellt sowohl, wenn die NPC gar nicht laeuft, als auch, wenn sie durch den Spieler
     * hindurchlaeuft oder nie zur Ruhe kommt. */
    int32_t nx0 = n->x, nz0 = n->z;
    int saw_advance = 0; uint8_t last = n->anim_frame;
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        if (n->anim_frame != last) saw_advance = 1;
        last = n->anim_frame;
    }
    int32_t moved = (n->x - nx0) * (n->x - nx0) + (n->z - nz0) * (n->z - nz0);
    if (moved == 0) {
        fprintf(stderr, "FAIL(2): die NPC muss dem Spieler FOLGEN (Eskorte 0x8011ce54), "
                        "blieb aber bei (%d,%d) stehen\n", n->x, n->z);
        fail = 1;
    }
    /* Sie muss die Schleife auch SCHLIESSEN: nach dem Anlaufen steht sie im Halte-Band.
     * Sub 1 -> Sub 3 bei Distanz < 500 (@0x8004f3c8), Sub 3 -> Sub 1 erst wieder ab 1001. */
    if (n->sub_state_1 != 3) {
        fprintf(stderr, "FAIL(2): die Eskorte kommt nicht zur Ruhe — Sub %u statt 3 "
                        "(Ankunft @0x8004f3c8), Distanz %u\n", n->sub_state_1, n->ai_dist);
        fail = 1;
    }
    if (n->ai_dist >= 0x1f4u) {
        fprintf(stderr, "FAIL(2): Halte-Abstand nicht erreicht — +0x1d0 = %u, erwartet < 500 "
                        "(@0x8004f3bc)\n", n->ai_dist);
        fail = 1;
    }
    if (!saw_advance)               { fprintf(stderr, "FAIL(2): idle pose must animate (anim_frame advances)\n"); fail = 1; }
    if (n->hp != -1)                { fprintf(stderr, "FAIL(2): NPC must stay invulnerable, hp=%d\n", n->hp); fail = 1; }
    printf("  (2) ESKORTE: (%d,%d) -> (%d,%d), Sub %u, +0x1d0 = %u, hp=%d\n",
           nx0, nz0, n->x, n->z, n->sub_state_1, n->ai_dist, n->hp);

    if (fail) { printf("NPC WAVE-1: FAIL\n"); return 1; }
    printf("NPC WAVE-1: all checks passed\n");
    return 0;
}
