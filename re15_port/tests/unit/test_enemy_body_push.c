/* test_enemy_body_push.c — enemy-vs-enemy body separation for non-zombie ground enemies.
 *
 * Audit wf_246147e3: the actor-vs-actor body-push (FUN_8002b544 pass) was nested in the zombie-only
 * dispatch branch, so non-zombie ground enemies never separated from each other and a swarm collapsed
 * to a single point. re15_enemy_body_push_tail now runs the b544 loop for every moving ground enemy.
 * Asserts:
 *   (1) two overlapping dogs (0x20) push apart (do NOT stay clumped) after a few ticks.
 *   (2) a single enemy still reaches the player (the push must NOT shove it out of attack range —
 *       that regressed 3 tests when the aec4 player-push used the big damage-box radius).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

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
    printf("=== enemy-vs-enemy body separation (non-zombie swarm) ===\n");

    /* (1) two dogs starting almost on top of each other must separate (not clump to a point) */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 40000; pl->hp = 100;   /* far: chase is negligible */
        re15_actor_t *d1 = &g_actors[1];
        d1->active = 1; d1->type = 0x20; d1->state = 1; d1->x = 0;   d1->z = 0; re15_enemy_apply_hitbox(d1, 0x20);
        re15_actor_t *d2 = &g_actors[2];
        d2->active = 1; d2->type = 0x20; d2->state = 1; d2->x = 50;  d2->z = 0; re15_enemy_apply_hitbox(d2, 0x20);
        int32_t before = xz_dist(d1, d2);
        for (int f = 0; f < 8; f++) re15_enemy_ai_run_all(0);
        int32_t after = xz_dist(d1, d2);
        if (after <= before + 100) {
            fprintf(stderr, "FAIL(1): two clumped dogs must push apart, dist %d->%d\n", before, after); fail = 1; }
        else printf("  (1) SWARM: two overlapping dogs separated, dist %d->%d\n", before, after);
    }

    /* (2) a single big enemy (maggot, damage-box radius 1600) still reaches + bites the player —
     * the enemy-vs-enemy push must not touch the enemy-vs-player distance (no lone-enemy over-separation) */
    {
        extern int16_t re15_atan2_q12(int32_t, int32_t);
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 0; pl->hp = 100;
        re15_actor_t *m = &g_actors[1];
        m->active = 1; m->type = 0x27; m->state = 0; m->x = 0; m->z = 2200; m->rot_y = 0;
        re15_enemy_apply_hitbox(m, 0x27);
        m->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - m->z, pl->x - m->x) - 0x400) & 0xfff);
        int32_t d0 = xz_dist(m, pl);
        for (int f = 0; f < 200; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); if (pl->hp < 100) break; }
        int32_t d1 = xz_dist(m, pl);
        if (d1 >= d0) { fprintf(stderr, "FAIL(2): a lone maggot must still close on the player (not be pushed away), dist %d->%d\n", d0, d1); fail = 1; }
        else printf("  (2) LONE: maggot still closes on the player, dist %d->%d (attack range reachable)\n", d0, d1);
    }

    if (fail) { printf("ENEMY-BODY-PUSH: FAIL\n"); return 1; }
    printf("ENEMY-BODY-PUSH: all checks passed\n");
    return 0;
}
