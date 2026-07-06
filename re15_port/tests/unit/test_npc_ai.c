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

    /* (2) IDLE: stationary + the idle pose animates (anim_frame advances over the 50-frame clip 2) */
    int32_t nx0 = n->x, nz0 = n->z;
    int saw_advance = 0; uint8_t last = n->anim_frame;
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        if (n->anim_frame != last) saw_advance = 1;
        last = n->anim_frame;
    }
    if (n->x != nx0 || n->z != nz0) { fprintf(stderr, "FAIL(2): idle NPC must stay put, moved (%d,%d)->(%d,%d)\n", nx0, nz0, n->x, n->z); fail = 1; }
    if (!saw_advance)               { fprintf(stderr, "FAIL(2): idle pose must animate (anim_frame advances)\n"); fail = 1; }
    if (n->hp != -1)                { fprintf(stderr, "FAIL(2): NPC must stay invulnerable, hp=%d\n", n->hp); fail = 1; }
    printf("  (2) IDLE: held at (%d,%d), idle pose animated, hp=%d\n", n->x, n->z, n->hp);

    if (fail) { printf("NPC WAVE-1: FAIL\n"); return 1; }
    printf("NPC WAVE-1: all checks passed\n");
    return 0;
}
