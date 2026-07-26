/* test_birkin_ai.c — G-BIRKIN BOSS (type 0x30 EM030 / 0x36 EM036, STAGE3) byte-true AI.
 *
 * Rebuilt for the full 0x80116230 family (audit wf_efd92a2c birkin). The prior Wave-1 test PINNED two
 * WRONG behaviors that the byte-true rebuild corrects, so those assertions are REWRITTEN here:
 *   - the boss tackled a STANDING player  -> byte-true (finding #8 @0x801170bc): the tackle fires ONLY
 *     vs a RUNNING player (player+0x8c speed >= 101) who is OFF-CENTER (outside the ±0x10 razor cone).
 *   - a kill went straight to CORPSE(7)   -> byte-true (finding #3): DEATH is a morph-tail down-machine
 *     that plays clip 9, WAITS for the room-SCD trigger (grid&0xf)==2, then RUNS OFF (sub 11) for the
 *     FORM-3 handoff. It never becomes an inert state-7 corpse.
 * New checks: (5) the PHASE-2 MUTATION (the headline finding #1/#2) and its lethal-hit revive (finding #5).
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

/* face rot_y so the player at (px,pz) sits `off` Q12 units off the boss's dead-ahead (0 = dead ahead). */
static int16_t face_off(const re15_actor_t *e, int32_t px, int32_t pz, int off)
{
    return (int16_t)((((int)re15_atan2_q12(pz - e->z, px - e->x) - 0x400) + off) & 0xfff);
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

    printf("=== G-BIRKIN BOSS (type 0x30, EM030) byte-true AI ===\n");

    /* (1) INIT 0x801166e0: grid 0x33 (nibble 3) -> sub 9 (emerge); HP 300 UNCONDITIONAL; idle clip 0. */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)      { fprintf(stderr, "FAIL(1): INIT->BRAIN expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp != 300)       { fprintf(stderr, "FAIL(1): boss HP must be 300 (hardcoded @0x8011690c), got %d\n", e->hp); fail = 1; }
    if (e->motion != 0)     { fprintf(stderr, "FAIL(1): INIT idle clip must be 0, got %d\n", e->motion); fail = 1; }
    if (e->sub_state_1 != 9){ fprintf(stderr, "FAIL(1): grid 0x33 (nibble 3) -> sub 9, got %d\n", e->sub_state_1); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, boss hp=%d, clip=%d\n", e->sub_state_1, e->hp, e->motion);

    /* (2) CHASE: after the emergence set-piece (sub 9 -> lunge -> HUB) the boss walks in and closes.
     * Byte-true emergence is slower than the old single-clip stub, so allow the full sequence to run. */
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 600; f++) re15_enemy_ai_run_all(0);
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): the boss must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: dist %d->%d (closing)\n", c0, c1);

    /* (2b) CLOSE ATTACK: player in the front cone within 2500 -> the HUB commits a BITE (sub 4) / CLAW
     * (sub 3) dealing -10 (@0x80117c78 / @0x801177f0). */
    pl->x = e->x; pl->z = e->z + 1500; pl->hp = 100; pl->hit_react = 0;
    e->rot_y = face_off(e, pl->x, pl->z, 0);            /* dead ahead */
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_grab = 0;
    int attacked = 0; int16_t ahp0 = pl->hp;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 3 || e->sub_state_1 == 4) attacked = 1;
        pl->hit_react = 0;
        if (pl->hp < ahp0) break;
    }
    if (!attacked)           { fprintf(stderr, "FAIL(2b): boss never committed a close attack (sub 3/4); sub=%d\n", e->sub_state_1); fail = 1; }
    if (pl->hp != ahp0 - 10) { fprintf(stderr, "FAIL(2b): the close attack must deal -10, hp %d->%d\n", ahp0, pl->hp); fail = 1; }
    printf("  (2b) CLOSE ATTACK: sub reached %d, player hp %d->%d (-10 on connect)\n", e->sub_state_1, ahp0, pl->hp);

    /* (2c) TACKLE gate — byte-true DECIDE @0x8011706c: sub-7 needs dist<0xed8 && arc(0x464) && !arc(0x10)
     * (off-center) && player+0x8c >= 101 (RUNNING) && +0x1df==0. The old test's "tackle a STANDING player"
     * was the WRONG behavior finding #8 flagged. */
    /* (2c-i) a STANDING player (speed 0), off-center in the band, is NOT tackled */
    pl->speed_h = 0;
    int tackled_standing = 0;
    for (int f = 0; f < 40; f++) {
        pl->x = e->x; pl->z = e->z + 3000; pl->hit_react = 0;
        e->rot_y = face_off(e, pl->x, pl->z, 0x40);     /* 0x40 off-center (>0x10 razor, <0x464 wide) */
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_atk_cd = 0; e->birkin_grab = 0;
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 7) { tackled_standing = 1; break; }
    }
    if (tackled_standing) { fprintf(stderr, "FAIL(2c-i): a STANDING player must NOT be tackled (needs +0x8c>=101)\n"); fail = 1; }
    /* (2c-ii) a RUNNING off-center player IS tackled (sub 7 committed) */
    pl->speed_h = 101;
    int tackle_committed = 0;
    for (int f = 0; f < 40; f++) {
        pl->x = e->x; pl->z = e->z + 3000; pl->hit_react = 0;
        e->rot_y = face_off(e, pl->x, pl->z, 0x40);
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_atk_cd = 0; e->birkin_grab = 0;
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 7) { tackle_committed = 1; break; }
    }
    if (!tackle_committed) { fprintf(stderr, "FAIL(2c-ii): a RUNNING off-center player must be tackled (sub 7); sub=%d\n", e->sub_state_1); fail = 1; }
    /* (2c-iii) the TACKLE deals -5 (@0x80118b1c) */
    pl->speed_h = 101; pl->x = e->x; pl->z = e->z + 2000; pl->hp = 100; pl->hit_react = 0;
    e->rot_y = face_off(e, pl->x, pl->z, 0);
    e->state = 1; e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;
    int16_t thp0 = pl->hp;
    for (int f = 0; f < 80; f++) {
        re15_enemy_ai_run_all(0);
        pl->hit_react = 0;
        if (pl->hp < thp0) break;
    }
    if (pl->hp != thp0 - 5) { fprintf(stderr, "FAIL(2c-iii): the TACKLE must deal -5, hp %d->%d\n", thp0, pl->hp); fail = 1; }
    printf("  (2c) TACKLE: standing=NOT tackled, running=tackled, -5 on connect (hp %d->%d)\n", thp0, pl->hp);
    pl->speed_h = 0;

    /* (3) DEATH morph tail 0x8011a5d8 (form-2 boss, grid&0x10 set): clip 9 collapse -> WAIT (grid&0xf)==2
     * -> clip 0xc -> RUN-OFF sub 11. Byte-true: it HOLDS at the wait (the SCD trigger is not wired) and
     * NEVER becomes an inert state-7 corpse. Simulating the trigger drives the run-off. */
    pl->x = 0; pl->z = 20000;
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_flags = 0; e->birkin_grab = 0;
    e->grid_id = 0x33;                                  /* form-2 (bit 0x10 set) -> no re-mutation, runs the death tail */
    e->hp = 4; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);                       /* lethal hit -> state 3 */
    int reached_wait = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->state == 3 && e->sub_state_3 == 3) { reached_wait = 1; break; }   /* the (grid&0xf)==2 morph wait */
    }
    if (!reached_wait)  { fprintf(stderr, "FAIL(3): DEATH must reach the morph-wait (state 3, phase 3), got state=%d ph=%d\n", e->state, e->sub_state_3); fail = 1; }
    if (e->state == 7)  { fprintf(stderr, "FAIL(3): byte-true DEATH must NOT become a state-7 corpse\n"); fail = 1; }
    if (e->motion != 9) { fprintf(stderr, "FAIL(3): collapse clip must be 9, got %d\n", e->motion); fail = 1; }
    e->grid_id = (uint8_t)((e->grid_id & ~0xf) | 2);   /* simulate the room-SCD morph trigger: grid nibble -> 2 */
    int ran_off = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (e->state == 1 && e->sub_state_1 == 11) { ran_off = 1; break; }   /* -> RUN-OFF (form-3 handoff) */
    }
    re15_enemy_ai_run_all(0);                          /* one more tick so ACT[11] phase 0 sets the run-off point */
    if (!ran_off)                    { fprintf(stderr, "FAIL(3): after the morph trigger the boss must RUN OFF (sub 11); state=%d sub=%d\n", e->state, e->sub_state_1); fail = 1; }
    if (!(e->birkin_flags & 1))      { fprintf(stderr, "FAIL(3): run-off must set +0x1dd&1\n"); fail = 1; }
    if (e->birkin_runoff_x != -22000){ fprintf(stderr, "FAIL(3): run-off point X must be -22000, got %d\n", e->birkin_runoff_x); fail = 1; }
    printf("  (3) DEATH: collapse clip 9 -> morph-wait -> (trigger) -> RUN-OFF sub 11 to (%d,%d)\n",
           e->birkin_runoff_x, e->birkin_runoff_z);

    /* (5) PHASE-2 MUTATION (the headline, finding #1/#2): a FORM-1 boss (grid&0x10 clear) at HP<100 is
     * FORCED into sub 6 (@0x80116684); sub 6 sets grid|=0x10 (form-2), HP=150, +0x1dd|=8, clip 0x14. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 20000; pl->hp = 100;
    e = &g_actors[BS];
    e->active = 1; e->type = 0x30; e->state = 0; e->grid_id = 0x21; e->x = 0; e->y = 0; e->z = 0;  /* grid 0x21 = FORM-1 (bit 0x10 clear) */
    re15_enemy_apply_hitbox(e, 0x30);
    re15_enemy_ai_run_all(0);                          /* INIT -> BRAIN */
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* force HUB */
    e->hp = 90;                                        /* drop below the 100 mutation threshold */
    int mutated = 0;
    for (int f = 0; f < 80; f++) {
        re15_enemy_ai_run_all(0);
        if ((e->grid_id & 0x10) && e->hp == 150) { mutated = 1; break; }
    }
    if (!mutated)                { fprintf(stderr, "FAIL(5): HP<100 form-1 must MUTATE (grid|=0x10, HP=150); grid=0x%02x hp=%d sub=%d\n", e->grid_id, e->hp, e->sub_state_1); fail = 1; }
    if (!(e->birkin_flags & 8))  { fprintf(stderr, "FAIL(5): mutation must set the revive-guard +0x1dd&8\n"); fail = 1; }
    if (e->motion != 0x14)       { fprintf(stderr, "FAIL(5): mutation clip must be 0x14, got 0x%02x\n", e->motion); fail = 1; }
    printf("  (5) MUTATION: form-1 HP<100 -> sub 6 -> grid=0x%02x hp=%d clip=0x%02x flags&8=%d\n",
           e->grid_id, e->hp, e->motion, (e->birkin_flags & 8) != 0);

    /* (5b) MUTATION-REVIVE (finding #5): a lethal hit DURING the mutation (+0x1dd&8) does NOT kill —
     * HP is restored to 50 and the boss resumes the mutation (@0x8011a534). */
    e->birkin_saved_state = 1; e->birkin_saved_sub = 6; e->birkin_saved_ph2 = 1; e->birkin_saved_ph3 = 0;
    e->birkin_flags |= 8;                              /* mid-mutation (revive-guard armed) */
    e->hp = -5; e->state = 3; e->sub_state_3 = 0;      /* a lethal hit landed (as re15_damage would set it) */
    re15_enemy_ai_run_all(0);
    if (e->hp != 50)   { fprintf(stderr, "FAIL(5b): a lethal hit during mutation must REVIVE HP=50, got %d\n", e->hp); fail = 1; }
    if (e->state != 1) { fprintf(stderr, "FAIL(5b): the revive must RESUME the saved action (state 1), got %d\n", e->state); fail = 1; }
    printf("  (5b) MUTATION-REVIVE: lethal-during-mutation -> HP=%d, resumed state=%d sub=%d\n", e->hp, e->state, e->sub_state_1);

    /* (4) FORM-5 (type 0x36, EM036): STAGE3 registers 0x30 AND 0x36 to the same root -> byte-IDENTICAL AI. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 4000; pl->hp = 100;
    re15_actor_t *e5 = &g_actors[BS];
    e5->active = 1; e5->type = 0x36; e5->state = 0; e5->grid_id = 0x33; e5->x = 0; e5->y = 0; e5->z = 0;
    re15_enemy_apply_hitbox(e5, 0x36);
    re15_enemy_ai_run_all(0);
    if (e5->state != 1)       { fprintf(stderr, "FAIL(4): form-5 INIT->BRAIN expected state 1, got %d\n", e5->state); fail = 1; }
    if (e5->hp != 300)        { fprintf(stderr, "FAIL(4): form-5 must share the boss brain (HP 300), got %d\n", e5->hp); fail = 1; }
    if (e5->sub_state_1 != 9) { fprintf(stderr, "FAIL(4): form-5 grid 0x33 -> sub 9, got %d\n", e5->sub_state_1); fail = 1; }
    printf("  (4) FORM-5 (0x36): shares the boss brain -> state=%d hp=%d sub=%d\n", e5->state, e5->hp, e5->sub_state_1);

    if (fail) { printf("BIRKIN BOSS: FAIL\n"); return 1; }
    printf("BIRKIN BOSS: all checks passed\n");
    return 0;
}
