/* test_birkin_ai.c — G-BIRKIN boss form 1 (type 0x30, EM030, STAGE3) Wave-1 AI.
 *
 * Byte-true INIT from workflow wf_5df42870-cba (STAGE3.BIN, INIT cluster adversarially CONFIRMED). Asserts:
 *   (1) INIT: state -> 1, boss HP 300 (hardcoded, not the HP-table), idle clip 0, grid 0x33 -> sub 9.
 *   (2) NAV-CHASE: a nearby player makes the boss close (Wave 1 placeholder; attacks = wave 2).
 *   (3) KILLABLE: a lethal hit -> DEATH -> CORPSE (state 7). (Form-3 morph = wave 2.)
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

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 4000; pl->hp = 100;

    const int BS = 1;
    re15_actor_t *e = &g_actors[BS];
    e->active = 1; e->type = 0x30; e->state = 0; e->grid_id = 0x33; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x30);

    printf("=== G-BIRKIN boss form 1 (type 0x30, EM030) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)      { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 300)       { fprintf(stderr, "FAIL(1): boss HP must be 300 (hardcoded), got %d\n", e->hp); fail = 1; }
    if (e->motion != 0)     { fprintf(stderr, "FAIL(1): INIT idle clip must be 0, got %d\n", e->motion); fail = 1; }
    if (e->sub_state_1 != 9){ fprintf(stderr, "FAIL(1): grid 0x33 -> sub 9, got %d\n", e->sub_state_1); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, boss hp=%d, clip=%d\n", e->sub_state_1, e->hp, e->motion);

    /* (2) NAV-CHASE toward the player (Wave 1 placeholder) */
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 120; f++) re15_enemy_ai_run_all(0);
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): the boss must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) NAV-CHASE: dist %d->%d (closing)\n", c0, c1);

    /* (2b) CLOSE ATTACK: with the player in the front cone within 2500 the boss's HUB (sub 1) commits a
     * BITE (sub 4) / LUNGE-CLAW (sub 3) that deals -10. Byte-true (@0x801177f0). */
    extern int16_t re15_atan2_q12(int32_t, int32_t);
    pl->x = e->x; pl->z = e->z + 1500; pl->hp = 100; pl->hit_react = 0;   /* just ahead, within the 2500 range */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* face the player */
    e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;           /* force the HUB */
    int attacked = 0; int16_t ahp0 = pl->hp;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 3 || e->sub_state_1 == 4) attacked = 1;
        pl->hit_react = 0;
        if (pl->hp < ahp0) break;
    }
    if (!attacked)         { fprintf(stderr, "FAIL(2b): boss never committed a close attack (sub 3/4); sub=%d\n", e->sub_state_1); fail = 1; }
    if (pl->hp != ahp0 - 10) { fprintf(stderr, "FAIL(2b): the close attack must deal -10, hp %d->%d\n", ahp0, pl->hp); fail = 1; }
    printf("  (2b) CLOSE ATTACK: sub reached %d, player hp %d->%d (-10 on connect)\n", e->sub_state_1, ahp0, pl->hp);

    /* (2c) TACKLE (sub 7): player in the 3200..3800 band + aligned -> the HUB commits a TACKLE (sub 7),
     * clip 5, two windows -> player.hp -= 5 (@0x80118b1c). Byte-true tackle damage. */
    pl->x = e->x; pl->z = e->z + 3500; pl->hp = 100; pl->hit_react = 0;   /* in the 3200..3800 tackle band */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
    int tackled = 0; int16_t thp0 = pl->hp;
    for (int f = 0; f < 240; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 7) tackled = 1;
        pl->hit_react = 0;
        if (pl->hp < thp0) break;
        /* keep the player pinned in the band so the boss doesn't close past it into sub 3/4 */
        if (e->sub_state_1 == 1) { pl->x = e->x; pl->z = e->z + 3500;
            e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff); }
    }
    if (!tackled)          { fprintf(stderr, "FAIL(2c): boss never committed a TACKLE (sub 7); sub=%d\n", e->sub_state_1); fail = 1; }
    if (pl->hp != thp0 - 5) { fprintf(stderr, "FAIL(2c): the TACKLE must deal -5, hp %d->%d\n", thp0, pl->hp); fail = 1; }
    printf("  (2c) TACKLE: sub reached %d, player hp %d->%d (-5 on connect)\n", e->sub_state_1, thp0, pl->hp);

    /* (3) KILLABLE: a lethal hit -> DEATH -> CORPSE */
    pl->x = 0; pl->z = 20000;
    e->hp = 4; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(3): a killed boss must reach CORPSE (state 7), state=%d hp=%d\n", e->state, e->hp); fail = 1; }
    printf("  (3) DEATH: lethal hit -> CORPSE (state %d, hp=%d)\n", e->state, e->hp);

    /* (4) FORM-5 (type 0x36, EM036): STAGE3 registers 0x30 AND 0x36 to the same root 0x80116230 (zero +0x8
     * type reads, zero 0x36 immediates in the boss code) -> byte-IDENTICAL AI. Assert 0x36 runs the same
     * boss brain: INIT -> HP 300, sub 9. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 4000; pl->hp = 100;
    re15_actor_t *e5 = &g_actors[BS];
    e5->active = 1; e5->type = 0x36; e5->state = 0; e5->grid_id = 0x33; e5->x = 0; e5->y = 0; e5->z = 0;
    re15_enemy_apply_hitbox(e5, 0x36);
    re15_enemy_ai_run_all(0);
    if (e5->state != 1)       { fprintf(stderr, "FAIL(4): form-5 INIT->ACTIVE expected state 1, got %d\n", e5->state); fail = 1; }
    if (e5->hp != 300)        { fprintf(stderr, "FAIL(4): form-5 must share the boss brain (HP 300), got %d\n", e5->hp); fail = 1; }
    if (e5->sub_state_1 != 9) { fprintf(stderr, "FAIL(4): form-5 grid 0x33 -> sub 9, got %d\n", e5->sub_state_1); fail = 1; }
    printf("  (4) FORM-5 (0x36): shares the boss brain -> state=%d hp=%d sub=%d\n", e5->state, e5->hp, e5->sub_state_1);

    if (fail) { printf("BIRKIN BOSS WAVE-1: FAIL\n"); return 1; }
    printf("BIRKIN BOSS WAVE-1: all checks passed\n");
    return 0;
}
