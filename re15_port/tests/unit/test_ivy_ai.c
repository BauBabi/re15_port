/* test_ivy_ai.c — IVY plant-grappler (type 0x2d, EM02D, STAGE4 lab) AI.
 *
 * Byte-true from workflow wf_5c34ffe7 (root 0x801168c4). A humanoid plant grappler (HP 100) that
 * nav-chases and whose one attack is a GRAB -> EATEN-DEATH (instant kill via DAT_800aca58=7). Asserts:
 *   (1) INIT: state -> 1, HP 100 (@0x80116954), clip 0.
 *   (2) CHASE: a distant player makes the Ivy close in.
 *   (3) GRAB-KILL: a player in grab range gets grabbed (pinned) then instant-killed.
 *   (4) KILLABLE: a lethal hit -> DEATH -> CORPSE (state 7).
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
    memset(g_actors, 0, sizeof g_actors);
    extern int16_t re15_atan2_q12(int32_t, int32_t);
    extern int re15_player_is_grabbed(void);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 5000; pl->hp = 100;

    const int IS = 1;
    re15_actor_t *e = &g_actors[IS];
    e->active = 1; e->type = 0x2d; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x2d);

    printf("=== IVY plant-grappler (type 0x2d, EM02D) AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 100)  { fprintf(stderr, "FAIL(1): HP must be 100, got %d\n", e->hp); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, hp=%d, clip=%d\n", e->sub_state_1, e->hp, e->motion);

    /* (2) CHASE */
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 100; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): Ivy must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: dist %d->%d (closing)\n", c0, c1);

    /* (3) GRAB-KILL */
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    pl->x = e->x; pl->z = e->z + 1200; pl->hp = 100; pl->hit_react = 0;
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    int grabbed_seen = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (re15_player_is_grabbed()) grabbed_seen = 1;
        pl->hit_react = 0;
        if (pl->hp < 0) break;
    }
    if (!grabbed_seen) { fprintf(stderr, "FAIL(3): the Ivy never grabbed (pinned) the player\n"); fail = 1; }
    if (pl->hp >= 0)   { fprintf(stderr, "FAIL(3): the grab must EATEN-KILL the player, hp=%d\n", pl->hp); fail = 1; }
    printf("  (3) GRAB-KILL: player grabbed=%d, eaten -> hp=%d\n", grabbed_seen, pl->hp);

    /* (4) KILLABLE */
    pl->x = 0; pl->z = 30000;
    e->hp = 5; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    int corpse = 0;
    for (int f = 0; f < 120; f++) { re15_enemy_ai_run_all(0); if (e->state == 7) { corpse = 1; break; } }
    if (!corpse) { fprintf(stderr, "FAIL(4): a killed Ivy must reach CORPSE (state 7), state=%d\n", e->state); fail = 1; }
    printf("  (4) DEATH: lethal hit -> CORPSE (state %d)\n", e->state);

    if (fail) { printf("IVY GRAPPLER: FAIL\n"); return 1; }
    printf("IVY GRAPPLER: all checks passed\n");
    return 0;
}
