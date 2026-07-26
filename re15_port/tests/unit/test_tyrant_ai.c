/* test_tyrant_ai.c — TYRANT boss (type 0x2b, EM02B, STAGE4/5) AI.
 *
 * Byte-true rebuild (audit wf_efd92a2c tyrant, root 0x80111a64, state table @0x8011a0b4). A bipedal
 * ground walk-chaser: idle(sub0)->walk(sub1)->roar(sub2)->attack-decide(sub3) dual A/B dispatch, two
 * -10 melee attacks and facing-based grab-pins; a killable prototype Tyrant (HP pool 86..126
 * @0x80118b00). Asserts (driving the BYTE-TRUE subs, not the old "sub0==approach" stub):
 *   (1) INIT: state -> 1, HP in the pool {86..126}, clip 0.
 *   (2) CHASE: in the WALK regime (sub 1, B[1] 0x80112124) the Tyrant closes on the player.
 *   (3) ATTACK1: from the sub-3 decide regime (A[3] 0x801123e8) an aligned <1800 player takes -10
 *       (@0x80112898) — the byte-true trigger a804(1800,0x180), NOT the old dist<2501/±0x300.
 *   (4) GRAB: chipping the player below 50 (slti 50 @0x801128ac) triggers a facing-based grab-PIN
 *       (front sub 5 / rear sub 9, aca58=5; sub = facing*4+5 @0x801128c0-d0).
 *   (5) KILLABLE: a lethal hit -> DEATH -> CORPSE (state 7).
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

    const int TS = 1;
    re15_actor_t *e = &g_actors[TS];
    e->active = 1; e->type = 0x2b; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x2b);

    printf("=== TYRANT boss (type 0x2b, EM02B) AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)  { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    if (e->hp < 86 || e->hp > 126) { fprintf(stderr, "FAIL(1): HP must be in the pool 86..126, got %d\n", e->hp); fail = 1; }
    printf("  (1) INIT: state->1 sub=%d, hp=%d (pool), clip=%d\n", e->sub_state_1, e->hp, e->motion);

    /* (2) CHASE: byte-true, sub 0 is IDLE (stands rng+59 frames @0x80111f04) then promotes to WALK
     * sub 1. Drive the WALK regime (B[1] 0x80112124) directly and assert it closes on the player. */
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; e->motion = 0; e->anim_frame = 0;
    int32_t c0 = xz_dist(e, pl);
    for (int f = 0; f < 200; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    int32_t c1 = xz_dist(e, pl);
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): Tyrant must close on the player, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: dist %d->%d (closing)\n", c0, c1);

    /* (3) ATTACK1 -10: from the sub-3 DECIDE regime (A[3]) an aligned player <1800 is bitten. */
    e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; e->motion = 0; e->anim_frame = 0;
    e->birkin_hurt_cd = 0;                                               /* +0x1de cooldown clear */
    pl->x = e->x; pl->z = e->z + 1500; pl->hp = 100; pl->hit_react = 0;  /* dist 1500 < 1800 */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    int16_t ahp0 = pl->hp; int hit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        pl->hit_react = 0;
        pl->x = e->x; pl->z = e->z + 1500;                              /* keep the player pinned in range/arc */
        e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
        if (pl->hp <= ahp0 - 10) { hit = 1; break; }
    }
    if (!hit) { fprintf(stderr, "FAIL(3): the Tyrant ATTACK1 must deal -10, hp %d->%d (sub=%d)\n", ahp0, pl->hp, e->sub_state_1); fail = 1; }
    printf("  (3) ATTACK1: player hp %d->%d (-10 on connect)\n", ahp0, pl->hp);

    /* (4) GRAB: a low-HP player (<50 after the -10) drives ATTACK1 -> facing grab (front sub 5 / rear
     * sub 9). Drive B[4] directly; either grab sub, or the pin latch, counts. */
    e->state = 1; e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; e->motion = 0x0c; e->anim_frame = 0;
    pl->x = e->x; pl->z = e->z + 1200; pl->hp = 55; pl->hit_react = 0;   /* 55 - 10 = 45 < 50 -> grab */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
    int grabbed = 0;
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 == 5 || e->sub_state_1 == 9 || re15_player_is_grabbed()) grabbed = 1;
        pl->hit_react = 0;
        if (grabbed) break;
    }
    if (!grabbed) { fprintf(stderr, "FAIL(4): chipping <50 must trigger the grab-PIN (sub 5/9); sub=%d hp=%d\n", e->sub_state_1, pl->hp); fail = 1; }
    printf("  (4) GRAB: reached grab-PIN (sub=%d, player grabbed=%d)\n", e->sub_state_1, re15_player_is_grabbed());

    /* (5) KILLABLE */
    pl->x = 0; pl->z = 30000;
    e->hp = 5; e->hit_react = 0;
    re15_enemy_take_damage(e, 2);
    int corpse = 0;
    for (int f = 0; f < 120; f++) { re15_enemy_ai_run_all(0); if (e->state == 7) { corpse = 1; break; } }
    if (!corpse) { fprintf(stderr, "FAIL(5): a killed Tyrant must reach CORPSE (state 7), state=%d\n", e->state); fail = 1; }
    printf("  (5) DEATH: lethal hit -> CORPSE (state %d)\n", e->state);

    if (fail) { printf("TYRANT BOSS: FAIL\n"); return 1; }
    printf("TYRANT BOSS: all checks passed\n");
    return 0;
}
