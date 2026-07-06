/* ============================================================================
 *  Dog (Cerberus, type 0x20) AI — Wave 1 port probe.
 *
 *  Byte-true 0x8010d7f8 family (RE15_DOG_AI.md). Directly seeds a type-0x20 actor +
 *  a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x8010d93c): state 0 -> 1 (ACTIVE), idle clip 1, HP seeded.
 *   (2) ACTIVE brain: with the player close + in the FOV cone, the dog leaves idle,
 *       enters CHASE (sub 2) and moves TOWARD the player (distance closes).
 *   (3) Receiving side: a lethal hit (re15_enemy_take_damage) drives the dog through
 *       DEATH (state 3) to CORPSE (state 7) — the dog is killable, unlike the crow.
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

static int32_t xz_dist(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t d2 = dx * dx + dz * dz;
    int32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    return r;
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 2000; pl->hp = 100;

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0; d->rot_y = 0;  /* faces +z = toward player */
    re15_enemy_apply_hitbox(d, 0x20);

    printf("=== DOG (Cerberus, type 0x20) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (d->state != 1)  { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", d->state); fail = 1; }
    if (d->motion != 1) { fprintf(stderr, "FAIL(1): idle clip 1 expected, got %d\n", d->motion); fail = 1; }
    if (d->hp <= 0)     { fprintf(stderr, "FAIL(1): HP must be seeded > 0, got %d\n", d->hp); fail = 1; }
    printf("  (1) INIT: state->1, clip=%d, hp=%d\n", d->motion, d->hp);

    /* (2) chase toward the player */
    int32_t c0 = xz_dist(d, pl);
    int chased = 0;
    for (int f = 0; f < 80; f++) {
        re15_enemy_ai_run_all(0);
        if (d->sub_state_1 == 2) chased = 1;   /* reached CHASE */
    }
    int32_t c1 = xz_dist(d, pl);
    if (!chased)   { fprintf(stderr, "FAIL(2): dog never entered CHASE (sub 2); sub=%d flags=%d\n", d->sub_state_1, d->dog_flags); fail = 1; }
    if (c1 >= c0)  { fprintf(stderr, "FAIL(2): dog must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: sub reached 2, dist %d->%d (closing)\n", c0, c1);

    /* (3) BITE: with the player in bite range, the dog enters ATTACK-RANGE -> BITE and deals -10 HP. */
    pl->x = d->x; pl->z = d->z + 1000; pl->hp = 100;   /* just ahead, inside the 2000/384 bite cone */
    int16_t php0 = pl->hp; int bit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        pl->hit_react = 0;                              /* clear so successive bites can land */
        if (d->sub_state_1 == 8) bit = 1;
        if (pl->hp < php0) break;
    }
    if (!bit)            { fprintf(stderr, "FAIL(3): dog never reached BITE (sub 8); sub=%d\n", d->sub_state_1); fail = 1; }
    if (pl->hp >= php0)  { fprintf(stderr, "FAIL(3): bite must deal damage, player HP %d->%d\n", php0, pl->hp); fail = 1; }
    printf("  (3) BITE: sub reached 8, player HP %d->%d (-10 on connect)\n", php0, pl->hp);

    /* (4) killable: a lethal hit -> DEATH -> CORPSE */
    d->hp = 4;
    re15_enemy_take_damage(d, 2);              /* generic damage entry (shared with the zombie) */
    int reached_corpse = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (d->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(4): a killed dog must reach CORPSE (state 7), state=%d hp=%d\n", d->state, d->hp); fail = 1; }
    printf("  (4) DEATH: lethal hit -> state 7 CORPSE (hp=%d)\n", d->hp);

    if (fail) { printf("DOG WAVE-1: FAIL\n"); return 1; }
    printf("DOG WAVE-1: all checks passed\n");
    return 0;
}
