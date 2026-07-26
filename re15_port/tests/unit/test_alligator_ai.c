/* test_alligator_ai.c — ALLIGATOR boss (type 0x23, EM023, STAGE2 sewer) AI.
 *
 * Byte-true rebuild (audit wf_efd92a2c alligator, root 0x8010c448). A giant water/ground chaser whose
 * one attack is a lunging BITE. On connect it writes the engine-wide KNOCKDOWN player command
 * DAT_800aca58=2 (byte-identical to the zombie/spider knockdown) — the player is knocked down and
 * RECOVERS. There is NO jaws-hold and NO player.hp write (the hp access at the bite sites is a no-op
 * self-store; the aca58=3 "eaten" cmd is bgez-gated on player.hp<0, i.e. only if already dead). The
 * old "100-frame hold -> unconditional pl->hp=-1 swallow" was invented (audit #0/#2). Asserts:
 *   (1) INIT: state -> 1, HP 300 (row @0x801175dc = 16x300), clip 4, sub 6 (water/approach).
 *   (2) CHASE: a distant player makes the alligator close in.
 *   (3) BITE = KNOCKDOWN: a player in bite range is knocked down (hit_react latched) but NOT pinned
 *       and NOT killed (hp stays >= 0) — the player recovers.
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
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 6000; pl->hp = 100;

    const int AS = 1;
    re15_actor_t *e = &g_actors[AS];
    e->active = 1; e->type = 0x23; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x23);

    printf("=== ALLIGATOR boss (type 0x23, EM023) AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)       { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 300)        { fprintf(stderr, "FAIL(1): HP must be 300, got %d\n", e->hp); fail = 1; }
    if (e->motion != 4)      { fprintf(stderr, "FAIL(1): INIT clip must be 4, got %d\n", e->motion); fail = 1; }
    if (e->sub_state_1 != 6) { fprintf(stderr, "FAIL(1): default spawn -> sub 6 (ranged), got %d\n", e->sub_state_1); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, hp=%d, clip=%d\n", e->sub_state_1, e->hp, e->motion);

    /* (2) CHASE */
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 100; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): alligator must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: dist %d->%d (closing)\n", c0, c1);

    /* (3) BITE = KNOCKDOWN (audit #0/#2): a player in bite range facing the gator is KNOCKED DOWN
     * (hit_react latched by the connect) but NOT pinned (no grab) and NOT killed (hp stays >= 0). */
    e->state = 1; e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hit_stun = 0;
    pl->x = e->x; pl->z = e->z + 1200; pl->hp = 100; pl->hit_react = 0;
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    int knock_seen = 0, pinned_seen = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (pl->hit_react & 1) knock_seen = 1;      /* KNOCKDOWN latched this tick */
        if (re15_player_is_grabbed()) pinned_seen = 1;
        pl->hit_react = 0;                          /* player recovers from the knockdown */
    }
    if (!knock_seen)          { fprintf(stderr, "FAIL(3): the bite must KNOCK the player down (hit_react)\n"); fail = 1; }
    if (pinned_seen)          { fprintf(stderr, "FAIL(3): the bite is a KNOCKDOWN, not a pin/grab\n"); fail = 1; }
    if (pl->hp < 0)           { fprintf(stderr, "FAIL(3): the bite must NOT swallow/kill the player, hp=%d\n", pl->hp); fail = 1; }
    printf("  (3) BITE=KNOCKDOWN: knocked=%d, pinned=%d, hp=%d (player recovers)\n", knock_seen, pinned_seen, pl->hp);

    /* (4) KILLABLE: a lethal hit -> DEATH -> CORPSE */
    pl->x = 0; pl->z = 30000;
    e->hp = 5; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    int reached_corpse = 0;
    for (int f = 0; f < 120; f++) { re15_enemy_ai_run_all(0); if (e->state == 7) { reached_corpse = 1; break; } }
    if (!reached_corpse) { fprintf(stderr, "FAIL(4): a killed alligator must reach CORPSE (state 7), state=%d\n", e->state); fail = 1; }
    printf("  (4) DEATH: lethal hit -> CORPSE (state %d)\n", e->state);

    if (fail) { printf("ALLIGATOR BOSS: FAIL\n"); return 1; }
    printf("ALLIGATOR BOSS: all checks passed\n");
    return 0;
}
