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

    /* (2b) SPIT: BYTE-TRUE, an APPROACH (grid 1/5) spider that is FACING the player within 6000 and on an
     * equal-or-higher floor commits the FIXED venom attack SPIT (sub_state_2==7, clip 0xc) — NOT a grab.
     * The APPROACH decide's fixed-7 branch @0x80111384-e0 overwrites the lunge roll @0x80111368 whenever
     * (dist<6000 && faced-within-0x80 && floor>=player.floor). SPIT is distinct from GRAB-A (audit
     * wf_efd92a2c adult-spider #3/#5). It deals ZERO HP damage. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 800; pl->hp = 100;
    e = &g_actors[SS]; e->active = 1; e->type = 0x25; e->state = 1; e->sub_state_1 = 0; e->grid_id = 1; e->hp = 100; e->x = 0; e->z = 0;  /* grid 1 = APPROACH */
    re15_enemy_apply_hitbox(e, 0x25);
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* face the player */
    int spat = 0; int16_t shp0 = pl->hp;
    for (int f = 0; f < 40; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_2 == 7 || e->motion == 0x0c) { spat = 1; break; }   /* SPIT commit: sub2=7 / clip 0xc */
    }
    if (!spat)          { fprintf(stderr, "FAIL(2b): a facing+close APPROACH spider must commit SPIT(7); sub2=%d clip=0x%x\n", e->sub_state_2, e->motion); fail = 1; }
    if (pl->hp != shp0) { fprintf(stderr, "FAIL(2b): SPIT must deal ZERO damage (byte-true), hp %d->%d\n", shp0, pl->hp); fail = 1; }
    printf("  (2b) SPIT: facing+close APPROACH -> venom attack (spat=%d, clip=0x%x), hp %d->%d (0 damage)\n", spat, e->motion, shp0, pl->hp);

    /* (2c) GRAB: the walking lunge-bite (GRAB-A, sub_state_2==6) is a NON-DAMAGING stagger-grab — the
     * STRIKE connect @0x8011254c latches DAT_800aca58=2 + a keep-alive clamp only, no hitbox/take_damage/
     * direct hp write anywhere in 0x801109e4-0x801158a0. Force the lunge and verify the pin + 0 damage. */
    extern int re15_player_is_grabbed(void);
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 600; pl->hp = 100;
    e = &g_actors[SS]; e->active = 1; e->type = 0x25; e->state = 1; e->grid_id = 1; e->hp = 100; e->x = 0; e->z = 0;
    e->sub_state_1 = 1; e->sub_state_2 = 6; e->sub_state_3 = 0;   /* FORCE GRAB-A (sub2=6, commit-latched) */
    re15_enemy_apply_hitbox(e, 0x25);
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* face the player */
    int grabbed = 0; int16_t ghp0 = pl->hp;
    for (int f = 0; f < 200; f++) {
        if (e->sub_state_2 != 6 && !grabbed) { e->sub_state_1 = 1; e->sub_state_2 = 6; e->sub_state_3 = 0; }  /* re-force between cycles */
        re15_enemy_ai_run_all(0);
        if (re15_player_is_grabbed()) grabbed = 1;
        if (grabbed) break;
    }
    if (!grabbed)       { fprintf(stderr, "FAIL(2c): GRAB-A must stagger-pin the player; sub2=%d ph=%d\n", e->sub_state_2, e->sub_state_3); fail = 1; }
    if (pl->hp != ghp0) { fprintf(stderr, "FAIL(2c): the grab must deal ZERO damage (byte-true), hp %d->%d\n", ghp0, pl->hp); fail = 1; }
    printf("  (2c) GRAB-A: stagger-pin (grabbed=%d), player hp %d->%d (byte-true 0 damage)\n", grabbed, ghp0, pl->hp);

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
