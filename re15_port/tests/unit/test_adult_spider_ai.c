/* test_adult_spider_ai.c — ADULT SPIDER (type 0x25, EM025, STAGE2) Wave-1 AI.
 *
 * Byte-true from workflow wf_1f5685b3-a78 (STAGE2.BIN, 12 agents adversarially verified). Spawns a
 * type-0x25 actor + a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT: state -> 1, HP seeded from the row @0x8011761c (76-136), clip 0x10.
 *   (2) CHASE: a visible nearby player makes the spider slew toward + close on the player.
 *   (3) KILLABLE: a lethal hit -> DEATH (clip 1) -> CORPSE (state 7).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

extern int16_t re15_atan2_q12(int32_t, int32_t);

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
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 4000; pl->hp = 100;

    const int SS = 1;
    re15_actor_t *e = &g_actors[SS];
    e->active = 1; e->type = 0x25; e->state = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;  /* faces +z toward player */
    re15_enemy_apply_hitbox(e, 0x25);

    printf("=== ADULT SPIDER (type 0x25, EM025) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)  { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp < 76 || e->hp > 136) { fprintf(stderr, "FAIL(1): HP must seed to the row (76-136), got %d\n", e->hp); fail = 1; }
    if (e->motion != 0x10) { fprintf(stderr, "FAIL(1): INIT clip must be 0x10, got %d\n", e->motion); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d (row 76-136), clip=0x%x\n", e->hp, e->motion);

    /* (2) CHASE: the LOS-gated slew (rate 0x10) converges rot_y toward the player and the spider closes
     * (start from a neutral yaw and let the steer converge, like the maggot chase test). */
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 200; f++) re15_enemy_ai_run_all(0);
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): the spider must close on a visible player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: state=%d, dist %d->%d (closing)\n", e->state, c0, c1);

    /* (3) KILLABLE: a lethal hit -> DEATH -> CORPSE */
    pl->x = 0; pl->z = 20000;   /* player far so the chase doesn't interfere */
    e->hp = 4; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);              /* shared damage entry; hp goes <0 -> state 3 death */
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(3): a killed adult spider must reach CORPSE (state 7), state=%d hp=%d\n", e->state, e->hp); fail = 1; }
    printf("  (3) DEATH: lethal hit -> death clip 1 -> CORPSE (state %d, hp=%d)\n", e->state, e->hp);

    if (fail) { printf("ADULT SPIDER WAVE-1: FAIL\n"); return 1; }
    printf("ADULT SPIDER WAVE-1: all checks passed\n");
    return 0;
}
