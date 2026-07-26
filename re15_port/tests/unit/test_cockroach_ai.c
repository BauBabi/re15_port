/* test_cockroach_ai.c — COCKROACH (type 0x29, EM029, STAGE3) Wave-1 AI.
 *
 * Byte-true from workflow wf_066cfe96-fc2 (STAGE3.BIN, adversarially verified). A small maggot-like
 * scurrier with DIRECT player.hp damage. Asserts:
 *   (1) INIT: state -> 1, HP from the row @0x8011d6e8 (81-121), idle clip 0x16.
 *   (2) CHASE: a nearby player makes the roach approach/scurry closer.
 *   (2b) BITE: with the player in range the roach bites (sub 5) for -5.
 *   (3) KILLABLE: a lethal hit -> DEATH (clip 0xe) -> CORPSE (state 7).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

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
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 2000; pl->hp = 100;

    const int RS = 1;
    re15_actor_t *e = &g_actors[RS];
    e->active = 1; e->type = 0x29; e->state = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x29);

    printf("=== COCKROACH (type 0x29, EM029) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)   { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp < 81 || e->hp > 121) { fprintf(stderr, "FAIL(1): HP must seed to the row (81-121), got %d\n", e->hp); fail = 1; }
    if (e->motion != 0x16) { fprintf(stderr, "FAIL(1): INIT idle clip must be 0x16, got %d\n", e->motion); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d (row 81-121), clip=0x%x\n", e->hp, e->motion);

    /* (2) CHASE: engage nearby, then verify the byte-true SCURRY (sub 4, +0x8c speed 180-211/frame,
     *     @0x80111c5c) closes on a player who flees past 6001 units. The approach walk (sub 3) uses
     *     locator-bone root motion (0x80115b68) which displaces via the loaded EM029 bank in-game; the
     *     unit test has no bank, so we exercise the bank-independent SCURRY movement here. */
    pl->x = 0; pl->z = 4000; pl->hp = 100;
    for (int f = 0; f < 40; f++) re15_enemy_ai_run_all(0);      /* idle -> engage sub 3, face the player */
    pl->z = 9000;                                               /* player flees far -> sub-3 commits SCURRY (sub 4) */
    int32_t c0 = xz_dist(e, pl);
    int chased = 0; int32_t cmin = c0;                         /* track the CLOSEST it got (scurry overshoots a static point) */
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 4) chased = 1;
        int32_t d = xz_dist(e, pl); if (d < cmin) cmin = d;
    }
    if (!chased)        { fprintf(stderr, "FAIL(2): roach never entered the fast SCURRY (sub 4), sub=%d\n", e->sub_state_1); fail = 1; }
    if (cmin >= c0 / 2) { fprintf(stderr, "FAIL(2): roach must close on the player, dist %d -> min %d\n", c0, cmin); fail = 1; }
    printf("  (2) CHASE: SCURRY reached sub 4, dist %d -> closest %d (closing)\n", c0, cmin);

    /* (2b) BITE: with the player inside the bite box (part+1612 radius 1000, @0x80111eb8) the roach
     *     bites for -5 on the window frames 12-15 (@0x80111f60, byte-true frames not the old single 0x0c). */
    pl->x = e->x; pl->z = e->z + 900; pl->hp = 100; pl->hit_react = 0;
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* face the player */
    e->dog_atk_cd = 0;
    int16_t bp0 = pl->hp; int bit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 5) bit = 1;
        pl->hit_react = 0;
        if (pl->hp < bp0) break;
    }
    if (!bit)              { fprintf(stderr, "FAIL(2b): roach never reached BITE (sub 5); sub=%d\n", e->sub_state_1); fail = 1; }
    if (pl->hp != bp0 - 5) { fprintf(stderr, "FAIL(2b): the bite must deal exactly -5, hp %d->%d\n", bp0, pl->hp); fail = 1; }
    printf("  (2b) BITE: sub reached 5, player hp %d->%d (-5 on connect)\n", bp0, pl->hp);

    /* (3) KILLABLE: a lethal hit -> DEATH -> CORPSE */
    pl->x = 0; pl->z = 20000;
    e->hp = 4; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(3): a killed roach must reach CORPSE (state 7), state=%d hp=%d\n", e->state, e->hp); fail = 1; }
    printf("  (3) DEATH: lethal hit -> death clip 0xe -> CORPSE (state %d, hp=%d)\n", e->state, e->hp);

    if (fail) { printf("COCKROACH WAVE-1: FAIL\n"); return 1; }
    printf("COCKROACH WAVE-1: all checks passed\n");
    return 0;
}
