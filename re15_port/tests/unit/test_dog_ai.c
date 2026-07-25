/* ============================================================================
 *  Dog (Cerberus, type 0x20) AI — byte-true probe (audit wave wf_827f186d).
 *
 *  Byte-true 0x8010d7f8 family (RE15_DOG_AI.md). Directly seeds a type-0x20 actor +
 *  a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x8010d93c): state 0 -> 1 (ACTIVE), idle clip 1, HP seeded.
 *   (2) ACTIVE brain: with the player close + in the FOV cone, the dog leaves idle,
 *       enters CHASE (sub 2) and moves TOWARD the player (distance closes).
 *   (3) BITE leap machine (FUN_8010f15c): -10 on connect.
 *   (4) killable: lethal enemy-sourced hit (react row 9 = AIRBORNE variant) -> CORPSE.
 *   (5) scripted pounce (state 4 sub 0, FUN_80111398): waits for grid 0x43, leaps,
 *       consumes the grid mark, exits to CHASE with +0x1d6=0x78. (audit #1/#14)
 *   (6) EATEN-GRAB via an armed bite (sub 9/10) -> devour.
 *   (7) attack pick: hit_react==0 keeps the pick at BITE even at hp<50 (audit #11);
 *       hit_react!=0 && hp<50 escalates to 5/7.
 *   (8) reroute 13 leaps OVER: floor index +1, y -= 0x708 (audit #13).
 *   (9) sub 4 = STATIONARY bark chain: never moves, never state 5, +0x1e6=0x3c,
 *       exits to sub 3; pack-alert wakes a far idle dog (audit #2/#7).
 *  (10) INIT grid routing: 0x40 -> state 4 sub 0 (waits), 0x41 -> state 4 sub 1
 *       hold until grid 0x42 -> state 1 sub 0xc (audit #1/#14).
 *  (11) grounded HURT: flinch clip 6, recover -> sub 7 counter-hop -> sub 3 (audit #4/#9).
 *  (12) grounded DEATH detours through the flinch first (state 3 -> 2 -> 3 +0x7=1 ->
 *       collapse 0xe -> corpse fade phases) (verified death-detour claim).
 *  (13) airborne DEATH row (sub_state_1=7): knock-fly clip 7 -> corpse (audit #5).
 *  (14) low-HP hop (sub 7) moves REARWARD at +0x8c=20 and exits to sub 3 (audit #9/#10).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */
#include "re15_math.h"

static int32_t xz_dist(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t d2 = dx * dx + dz * dz;
    int32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    return r;
}

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 2000; pl->hp = 100;

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0; d->rot_y = 0;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);

    printf("=== DOG (Cerberus, type 0x20) byte-true AI (wf_827f186d) ===\n");

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

    /* (3) BITE: player in bite range + the dog pre-aligned (the byte-true chase steer is a
     *     RANDOM 1..16/frame slew @0x8010e468-84, so a 180-deg re-aim legitimately takes
     *     hundreds of frames — pre-face like the standoff geometry). -10 HP on connect. */
    pl->x = d->x; pl->z = d->z + 1000; pl->hp = 100; pl->hit_react = 0;
    face(d, pl);
    d->dog_atk_cd = 0;
    int16_t php0 = pl->hp; int bit = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        pl->hit_react = 0;                              /* clear so successive bites can land */
        if (d->sub_state_1 == 8) bit = 1;
        if (pl->hp < php0) break;
    }
    if (!bit)            { fprintf(stderr, "FAIL(3): dog never reached BITE (sub 8); sub=%d\n", d->sub_state_1); fail = 1; }
    if (pl->hp >= php0)  { fprintf(stderr, "FAIL(3): bite must deal damage, player HP %d->%d\n", php0, pl->hp); fail = 1; }
    printf("  (3) BITE: sub reached 8, player HP %d->%d (-10 on connect)\n", php0, pl->hp);

    /* (4) killable: a lethal enemy-sourced hit (react row 9 -> the AIRBORNE death variant
     *     @0x80121070[9]=0x80110eb0: knock-fly clip 7 + backward slide) -> CORPSE. */
    d->hp = 4;
    re15_enemy_take_damage(d, 2);              /* react_table[2] = 9 (airborne row) */
    int reached_corpse = 0, fly_clip = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (d->state == 3 && d->motion == 7) fly_clip = 1;
        if (d->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(4): a killed dog must reach CORPSE (state 7), state=%d hp=%d\n", d->state, d->hp); fail = 1; }
    if (!fly_clip)       { fprintf(stderr, "FAIL(4): react row 9 death must play the AIRBORNE clip 7 (@0x80110ef4), motion=%d\n", d->motion); fail = 1; }
    printf("  (4) DEATH: lethal row-9 hit -> airborne clip 7 -> state 7 CORPSE (hp=%d)\n", d->hp);

    /* (5) SCRIPTED POUNCE (state 4 sub 0, FUN_80111398): WAITS until the SCD marks grid 0x43
     *     (@0x801113e4-ec), then leaps (rise), lands, CONSUMES the mark (`sb zero,9` @0x8011161c)
     *     and exits word 0x201 with +0x1d6=0x78 (@0x801115f0). (audit #14) */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;
    re15_actor_t *pd = &g_actors[1];
    pd->active = 1; pd->type = 0x20; pd->grid_id = 0x40; pd->state = 0;
    pd->x = 0; pd->y = 0; pd->z = 0; pd->hp = 20; pd->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(pd, 0x20);
    re15_enemy_ai_run_all(0);                       /* INIT */
    if (pd->state != 4 || pd->sub_state_1 != 0) { fprintf(stderr, "FAIL(5): grid-0x40 INIT must start state 4 sub 0 (@0x8010db88-98), state=%d sub=%d\n", pd->state, pd->sub_state_1); fail = 1; }
    for (int f = 0; f < 30; f++) re15_enemy_ai_run_all(0);
    if (pd->state != 4 || pd->sub_state_2 != 0) { fprintf(stderr, "FAIL(5): grid-0x40 dog must WAIT inert until grid 0x43, state=%d phase=%d\n", pd->state, pd->sub_state_2); fail = 1; }
    pd->grid_id = 0x43;                             /* SCD fires the pounce mark */
    int32_t pky = 32767; int landed_chase = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if ((int32_t)pd->y < pky) pky = pd->y;                  /* track the peak of the rise */
        if (pd->state == 1 && pd->sub_state_1 == 2) { landed_chase = 1; break; }  /* landed -> chase */
    }
    if (pky >= 0)         { fprintf(stderr, "FAIL(5): pounce must rise (y<0 at peak), peak y=%d\n", (int)pky); fail = 1; }
    if (!landed_chase)    { fprintf(stderr, "FAIL(5): pounce must land -> CHASE (state 1/sub 2), state=%d sub=%d\n", pd->state, pd->sub_state_1); fail = 1; }
    if (pd->grid_id != 0) { fprintf(stderr, "FAIL(5): the 0x43 mark must be CONSUMED (@0x8011161c), grid=%02x\n", pd->grid_id); fail = 1; }
    if (pd->dog_atk_cd != 0x78 && pd->dog_atk_cd < 0x70) { fprintf(stderr, "FAIL(5): land must seed +0x1d6=0x78 (@0x801115f0), got %d\n", pd->dog_atk_cd); fail = 1; }
    printf("  (5) SCRIPTED POUNCE: waited, leap peaked y=%d, grid consumed, -> CHASE (cd=%d)\n", (int)pky, pd->dog_atk_cd);

    /* (6) EATEN-GRAB: an ARMED bite escalates to the eaten GRAB (sub 9/10, @0x8010f47c). */
    extern int re15_player_is_grabbed(void);
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1000; pl->hp = 100; pl->rot_y = 0;
    re15_actor_t *gg = &g_actors[1];
    gg->active = 1; gg->type = 0x20; gg->state = 1; gg->sub_state_1 = 8; gg->sub_state_2 = 0;   /* seeded in BITE */
    gg->x = 0; gg->y = 0; gg->z = 0; gg->hp = 20; gg->dog_grab_armed = 1; gg->em_flag_id = 0xFF; /* pre-armed (sub-5 windup) */
    face(gg, pl);
    re15_enemy_apply_hitbox(gg, 0x20);
    int grabbed = 0, escalated = 0, eaten = 0;
    for (int f = 0; f < 240; f++) {
        re15_enemy_ai_run_all(0);
        if (gg->sub_state_1 == 9 || gg->sub_state_1 == 10) escalated = 1;
        if (re15_player_is_grabbed()) grabbed = 1;
        if (pl->hp < 0 || pl->state == 7) { eaten = 1; break; }
    }
    if (!escalated) { fprintf(stderr, "FAIL(6): armed bite must escalate to the eaten GRAB (sub 9/10), sub=%d\n", gg->sub_state_1); fail = 1; }
    if (!grabbed)   { fprintf(stderr, "FAIL(6): the eaten grab must PIN the player\n"); fail = 1; }
    if (!eaten)     { fprintf(stderr, "FAIL(6): the eaten grab must devour the player (hp<0 / state 7), hp=%d state=%d\n", pl->hp, pl->state); fail = 1; }
    printf("  (6) EATEN-GRAB: bite->grab(sub %d), pinned=%d, player hp=%d state=%d\n", gg->sub_state_1, grabbed, pl->hp, pl->state);

    /* (7) ATTACK PICK (FUN_8010e568, audit #11): the hp<50 escalation is NESTED under
     *     player.hit_react != 0 (`beq acae7,zero -> exit` @0x8010e5cc). A weak but
     *     NON-reacting player always gets the default BITE (8). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1500; pl->hp = 30; pl->rot_y = 0;   /* weak */
    re15_actor_t *g7 = &g_actors[1];
    g7->active = 1; g7->type = 0x20; g7->state = 1; g7->sub_state_1 = 3; g7->sub_state_2 = 0;        /* ATTACK-RANGE */
    g7->x = 0; g7->y = 0; g7->z = 0; g7->hp = 20; g7->em_flag_id = 0xFF;
    face(g7, pl);
    re15_enemy_apply_hitbox(g7, 0x20);
    pl->hit_react = 0;
    re15_enemy_ai_run_all(0);                       /* one decision at hit_react==0 */
    if (g7->sub_state_1 != 8) { fprintf(stderr, "FAIL(7a): hit_react==0 && hp<50 must still pick BITE 8 (@0x8010e5cc), sub=%d\n", g7->sub_state_1); fail = 1; }
    /* reset to the standoff and re-run with the player hit-reacting -> 5 or 7 */
    g7->sub_state_1 = 3; g7->sub_state_2 = 0; g7->dog_atk_cd = 0;
    face(g7, pl);
    pl->hit_react = 1;
    re15_enemy_ai_run_all(0);
    if (g7->sub_state_1 != 5 && g7->sub_state_1 != 7) { fprintf(stderr, "FAIL(7b): hit_react!=0 && hp<50 must pick 5/7 (@0x8010e608-24), sub=%d\n", g7->sub_state_1); fail = 1; }
    printf("  (7) ATTACK PICK: hit_react=0 -> 8; hit_react=1+weak -> %d\n", g7->sub_state_1);
    pl->hit_react = 0;

    /* (8) OBSTACLE-REROUTE 13 (FUN_801102dc, audit #13): turns to the escape heading, runs
     *     clip 9, JUMPS UP one floor level (+0x82 += 1 @0x80110498, y -= 0x708 @0x801104b4)
     *     and returns to CHASE. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 5000; pl->y = 0; pl->z = 5000; pl->hp = 100;
    re15_actor_t *g8 = &g_actors[1];
    g8->active = 1; g8->type = 0x20; g8->state = 1; g8->sub_state_1 = 13; g8->sub_state_2 = 0;
    g8->x = 0; g8->y = 0; g8->z = 0; g8->rot_y = 0; g8->hp = 20; g8->ai_contact = 0x11;   /* heading nibble 1 (0x100) + contact bit */
    g8->floor = 1; g8->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(g8, 0x20);
    int rerouted = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (g8->state == 1 && g8->sub_state_1 == 2) { rerouted = 1; break; }   /* returned to CHASE */
    }
    if (!rerouted) { fprintf(stderr, "FAIL(8): reroute (sub 13) must navigate + return to CHASE (sub 2), sub=%d rot=%u\n", g8->sub_state_1, (unsigned)g8->rot_y); fail = 1; }
    if (g8->floor != 2)          { fprintf(stderr, "FAIL(8): reroute-13 must raise the floor index +0x82 (1->2, @0x80110498), floor=%d\n", g8->floor); fail = 1; }
    if (g8->y != -0x708)         { fprintf(stderr, "FAIL(8): reroute-13 must hop one level UP (y -= 0x708 @0x801104b4), y=%d\n", (int)g8->y); fail = 1; }
    printf("  (8) REROUTE-13: turned -> leapt one level UP (floor=%d, y=%d) -> CHASE\n", g8->floor, (int)g8->y);

    /* (9) SUB-4 = STATIONARY BARK CHAIN (FUN_8010ea44, audit #2): the dog never moves and never
     *     enters state 5; +0x1e6=0x3c (@0x8010eadc-eae4); exits to sub 3 (@0x8010ec7c). The bark
     *     (aca50|=1 @0x8010eb58) wakes a FAR idle dog (idle dec @0x8010de60-84, audit #7). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 20000; pl->hp = 100;
    re15_actor_t *ba = &g_actors[1];                 /* the barker, forced into sub 4 */
    ba->active = 1; ba->type = 0x20; ba->state = 1; ba->sub_state_1 = 4; ba->sub_state_2 = 0;
    ba->x = 0; ba->y = 0; ba->z = 0; ba->hp = 20; ba->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(ba, 0x20);
    re15_actor_t *bb = &g_actors[2];                 /* a far idle pack mate (no LOS trigger possible) */
    bb->active = 1; bb->type = 0x20; bb->state = 0;
    bb->x = 12000; bb->y = 0; bb->z = 20000; bb->hp = 20; bb->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(bb, 0x20);
    int32_t bx = ba->x, bz = ba->z;
    int woke = 0, entered5 = 0, exit3 = 0, cd3c = 0;
    for (int f = 0; f < 160; f++) {
        re15_enemy_ai_run_all(0);
        if (ba->dog_pounce_cd == 0x3c) cd3c = 1;
        if (ba->state == 5 || ba->state == 4) entered5 = 1;
        if (ba->state == 1 && ba->sub_state_1 == 3) { exit3 = 1; }
        if (bb->state == 1 && bb->sub_state_1 != 0) woke = 1;
        if (exit3 && woke) break;
    }
    if (ba->x != bx || ba->z != bz) { fprintf(stderr, "FAIL(9): the bark chain is STATIONARY (no 245d8 in 0x8010ea44-0x8010ec40), moved %d/%d\n", (int)(ba->x - bx), (int)(ba->z - bz)); fail = 1; }
    if (entered5) { fprintf(stderr, "FAIL(9): sub 4 must NEVER enter state 4/5 (@0x8010ebf0 is +0x6=5, audit #2)\n"); fail = 1; }
    if (!exit3)   { fprintf(stderr, "FAIL(9): bark chain must exit to sub 3 (@0x8010ec7c), state=%d sub=%d\n", ba->state, ba->sub_state_1); fail = 1; }
    if (!cd3c)    { fprintf(stderr, "FAIL(9): chain entry must set +0x1e6=0x3c (@0x8010eadc), cd=%d\n", ba->dog_pounce_cd); fail = 1; }
    if (!woke)    { fprintf(stderr, "FAIL(9): the bark (aca50|=1) must wake the far idle dog (@0x8010de60-84), bb sub=%d\n", bb->sub_state_1); fail = 1; }
    printf("  (9) BARK CHAIN: stationary, cd=0x3c, exit sub 3, pack mate woke (sub=%d)\n", bb->sub_state_1);

    /* (10) INIT grid routing 0x41 (FUN_80111658): window hold clip 8 until the SCD marks 0x42
     *      -> state 1 sub 0xc stand-up, mark consumed (audit #1/#14). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 20000; pl->hp = 100;
    re15_actor_t *wd = &g_actors[1];
    wd->active = 1; wd->type = 0x20; wd->state = 0; wd->grid_id = 0x41;
    wd->x = 0; wd->y = 0; wd->z = 0; wd->hp = 20; wd->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(wd, 0x20);
    for (int f = 0; f < 20; f++) re15_enemy_ai_run_all(0);
    if (wd->state != 4 || wd->sub_state_1 != 1 || wd->motion != 8) { fprintf(stderr, "FAIL(10): grid-0x41 INIT must hold state 4 sub 1 clip 8 (@0x8010dba8-b8/@0x80111684), state=%d sub=%d clip=%d\n", wd->state, wd->sub_state_1, wd->motion); fail = 1; }
    wd->grid_id = 0x42;                              /* SCD release mark */
    re15_enemy_ai_run_all(0);
    if (wd->state != 1 || wd->sub_state_1 != 0x0c) { fprintf(stderr, "FAIL(10): grid 0x42 must release to state 1 sub 0xc (word 0xc01 @0x8011172c), state=%d sub=%d\n", wd->state, wd->sub_state_1); fail = 1; }
    if (wd->grid_id != 0) { fprintf(stderr, "FAIL(10): the 0x42 mark must be consumed (@0x80111718), grid=%02x\n", wd->grid_id); fail = 1; }
    printf("  (10) WINDOW HOLD: clip-8 hold, 0x42 release -> sub 0xc stand-up\n");

    /* (11) grounded HURT (FUN_801109e0, audit #4): flinch clip 6, then recover STRAIGHT into
     *      the counter-hop sub 7 (`sb 1,+0x4` @0x80110b48 + `sb 7,+0x5` @0x80110b58), whose
     *      exit is sub 3 (@0x8010f130, audit #9). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 4000; pl->y = 0; pl->z = 0; pl->hp = 100;
    re15_actor_t *hd = &g_actors[1];
    hd->active = 1; hd->type = 0x20; hd->hp = 40; hd->em_flag_id = 0xFF;
    hd->x = 0; hd->y = 0; hd->z = 0;
    re15_enemy_apply_hitbox(hd, 0x20);
    hd->state = 2; hd->sub_state_1 = 2; hd->sub_state_2 = 1; hd->sub_state_3 = 0;   /* handgun row 2 = grounded */
    int flinch6 = 0, counter7 = 0, then3 = 0;
    for (int f = 0; f < 400; f++) {
        re15_enemy_ai_run_all(0);
        if (hd->state == 2 && hd->motion == 6) flinch6 = 1;
        if (hd->state == 1 && hd->sub_state_1 == 7) counter7 = 1;
        if (counter7 && hd->state == 1 && hd->sub_state_1 == 3) { then3 = 1; break; }
    }
    if (!flinch6)  { fprintf(stderr, "FAIL(11): grounded hurt must play flinch clip 6 (@0x80110a3c), motion=%d\n", hd->motion); fail = 1; }
    if (!counter7) { fprintf(stderr, "FAIL(11): hurt recover must exit to SUB 7 counter-hop (@0x80110b58), state=%d sub=%d\n", hd->state, hd->sub_state_1); fail = 1; }
    if (!then3)    { fprintf(stderr, "FAIL(11): the counter-hop must exit to sub 3 (@0x8010f130), sub=%d\n", hd->sub_state_1); fail = 1; }
    printf("  (11) HURT: flinch 6 -> sub 7 counter-hop -> sub 3\n");

    /* (12) grounded DEATH detour (FUN_80111120, verified claim): a fresh kill (+0x7==0) BOUNCES
     *      to the hurt flinch first (`sb 2,+0x4` @0x80111160), then re-enters state 3 with
     *      +0x7=1 -> collapse clip 0x0e -> CORPSE with the 90-frame fade phase (FUN_80111774). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 4000; pl->y = 0; pl->z = 0; pl->hp = 100;
    re15_actor_t *dd = &g_actors[1];
    dd->active = 1; dd->type = 0x20; dd->hp = -2; dd->em_flag_id = 0xFF;
    dd->x = 0; dd->y = 0; dd->z = 0;
    re15_enemy_apply_hitbox(dd, 0x20);
    dd->state = 3; dd->sub_state_1 = 2; dd->sub_state_2 = 1; dd->sub_state_3 = 0;   /* fresh kill, grounded row */
    int detour2 = 0, collapse = 0, faded = 0;
    for (int f = 0; f < 400; f++) {
        re15_enemy_ai_run_all(0);
        if (dd->state == 2 && dd->motion == 6) detour2 = 1;
        if (dd->state == 3 && dd->motion == 0x0e) collapse = 1;
        if (dd->state == 7 && dd->sub_state_3 == 2) { faded = 1; break; }
    }
    if (!detour2)  { fprintf(stderr, "FAIL(12): grounded death must DETOUR through the flinch (state 2, clip 6) first (@0x80111160), state=%d\n", dd->state); fail = 1; }
    if (!collapse) { fprintf(stderr, "FAIL(12): then collapse with clip 0x0e (@0x801111f0), motion=%d\n", dd->motion); fail = 1; }
    if (!faded)    { fprintf(stderr, "FAIL(12): corpse must run the 90-frame fade to phase 2 (@0x801117a0-864), state=%d p=%d\n", dd->state, dd->sub_state_3); fail = 1; }
    printf("  (12) DEATH DETOUR: 3 -> flinch(2) -> collapse 0x0e -> corpse fade done\n");

    /* (13) low-HP hop (FUN_8010efbc, audit #10): clip 0x13 at +0x8c=20 moving REARWARD
     *      (245d8(0x800) @0x8010f120-124), exit -> sub 3 (@0x8010f130). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 8000; pl->y = 0; pl->z = 0; pl->hp = 100;
    re15_actor_t *lh = &g_actors[1];
    lh->active = 1; lh->type = 0x20; lh->hp = 20; lh->em_flag_id = 0xFF;
    lh->x = 0; lh->y = 0; lh->z = 0;
    re15_enemy_apply_hitbox(lh, 0x20);
    face(lh, pl);                                     /* facing +x -> rearward = -x */
    lh->state = 1; lh->sub_state_1 = 7; lh->sub_state_2 = 0;
    int hop3 = 0;
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        if (lh->sub_state_1 == 3) { hop3 = 1; break; }
    }
    if (lh->x >= 0) { fprintf(stderr, "FAIL(13): the hop must move REARWARD (x<0), x=%d\n", (int)lh->x); fail = 1; }
    if (lh->x < -1200) { fprintf(stderr, "FAIL(13): hop speed is +0x8c=20/frame (@0x8010f070), not a 0x800 charge; x=%d\n", (int)lh->x); fail = 1; }
    if (!hop3)      { fprintf(stderr, "FAIL(13): the hop must exit to sub 3 (@0x8010f130), sub=%d\n", lh->sub_state_1); fail = 1; }
    printf("  (13) LOW-HP HOP: rearward to x=%d, exit sub 3\n", (int)lh->x);

    if (fail) { printf("DOG: FAIL\n"); return 1; }
    printf("DOG byte-true: all checks passed\n");
    return 0;
}
