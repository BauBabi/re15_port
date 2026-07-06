/* ============================================================================
 *  Spider-Baby (type 0x26, EM026) AI — Wave 1 port probe.
 *
 *  Byte-true 0x80116288 family (RE15_SPIDER_AI.md). A STATIONARY web-spitter that
 *  emerges vertically, then deals a -2 contact stagger, and is killable. Seeds a
 *  type-0x26 actor + a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x801164b0): state 0 -> 1 (ACTIVE), HP=100, variant seeded from grid.
 *   (2) STATIONARY: the spider does NOT translate (no locomotion in STATE[1]).
 *   (3) CONTACT: a player inside the body box takes -2 (floored at hp>=4).
 *   (4) KILLABLE: a lethal hit -> DEATH gib-burst (state 8) -> CORPSE (state 7).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 8000; pl->hp = 100;  /* far away */

    const int SS = 1;
    re15_actor_t *s = &g_actors[SS];
    s->active = 1; s->type = 0x26; s->state = 0; s->x = 3000; s->y = 0; s->z = 3000;
    s->grid_id = 0x02;   /* variant 2 (Behavior A), bit 0x80 clear -> emerge step 20 */
    re15_enemy_apply_hitbox(s, 0x26);

    printf("=== SPIDER-BABY (type 0x26) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (s->state != 1)        { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", s->state); fail = 1; }
    if (s->hp != 100)         { fprintf(stderr, "FAIL(1): HP must seed to 100, got %d\n", s->hp); fail = 1; }
    if (s->sub_state_1 != 2)  { fprintf(stderr, "FAIL(1): +0x5 variant must seed from grid&0x7f=2, got %d\n", s->sub_state_1); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d, variant=%d\n", s->hp, s->sub_state_1);

    /* (2) STATIONARY: run a while, the spider must not translate in XZ */
    int32_t sx0 = s->x, sz0 = s->z;
    for (int f = 0; f < 120; f++) re15_enemy_ai_run_all(0);
    if (s->x != sx0 || s->z != sz0) { fprintf(stderr, "FAIL(2): spider must be stationary, moved (%d,%d)->(%d,%d)\n", sx0, sz0, s->x, s->z); fail = 1; }
    printf("  (2) STATIONARY: XZ held at (%d,%d) over 120 frames\n", s->x, s->z);

    /* (3) CONTACT: put the player inside the body box (<600) -> -2 per contact (hit_react gated) */
    pl->x = s->x; pl->z = s->z + 400; pl->hp = 100; pl->hit_react = 0;   /* 400 < 600 body box */
    int16_t php0 = pl->hp; int bit = 0;
    for (int f = 0; f < 60; f++) {
        re15_enemy_ai_run_all(0);
        if (pl->hp < php0) { bit = 1; break; }
        pl->hit_react = 0;   /* clear so a fresh contact can land */
    }
    if (!bit)             { fprintf(stderr, "FAIL(3): spider contact must deal damage, hp stayed %d\n", pl->hp); fail = 1; }
    if (pl->hp != php0 - 2) { fprintf(stderr, "FAIL(3): contact must be exactly -2, hp %d->%d\n", php0, pl->hp); fail = 1; }
    printf("  (3) CONTACT: player inside body box -> hp %d->%d (-2)\n", php0, pl->hp);

    /* (4) KILLABLE: a lethal hit -> DEATH (gib) -> CORPSE (state 7) */
    pl->x = 0; pl->z = 8000;   /* move the player away so contact doesn't interfere */
    s->hp = 4; s->hit_react = 0;
    re15_enemy_take_damage(s, 2);              /* generic damage entry (shared); hp 4-24 < 0 -> death */
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (s->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(4): a killed spider must reach CORPSE (state 7), state=%d hp=%d\n", s->state, s->hp); fail = 1; }
    printf("  (4) DEATH: lethal hit -> gib burst -> CORPSE (state %d, hp=%d)\n", s->state, s->hp);

    if (fail) { printf("SPIDER WAVE-1: FAIL\n"); return 1; }
    printf("SPIDER WAVE-1: all checks passed\n");
    return 0;
}
