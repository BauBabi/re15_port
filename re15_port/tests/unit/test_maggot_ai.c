/* ============================================================================
 *  Maggots (type 0x27, EM027 = MAGGOTS_BABY) AI — Wave 1 port probe.
 *
 *  Byte-true 0x80116db8 family (RE15_MAGGOT_AI.md). A large moving ground creature
 *  that idle-wanders, chases the player (yaw-slew + crawl), and is killable. Seeds a
 *  type-0x27 actor + a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x80116f50): state 0 -> 1 (ACTIVE), HP=180.
 *   (2) CHASE: a nearby player makes the maggot enter CHASE (sub 3) and crawl closer.
 *   (3) KILLABLE: a lethal hit -> DEATH (state 3, clip 0xe) -> CORPSE (state 7).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

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

    const int MS = 1;
    re15_actor_t *m = &g_actors[MS];
    m->active = 1; m->type = 0x27; m->state = 0; m->x = 0; m->y = 0; m->z = 0; m->rot_y = 0;  /* faces +z toward player */
    re15_enemy_apply_hitbox(m, 0x27);

    printf("=== MAGGOTS (type 0x27, MAGGOTS_BABY) Wave-1 AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (m->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", m->state); fail = 1; }
    if (m->hp != 180)  { fprintf(stderr, "FAIL(1): HP must seed to 180, got %d\n", m->hp); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d\n", m->hp);

    /* (2) CHASE toward the player (dist<3000 -> blind chase) */
    int32_t c0 = xz_dist(m, pl);
    int chased = 0;
    for (int f = 0; f < 100; f++) {
        re15_enemy_ai_run_all(0);
        if (m->sub_state_1 == 3) chased = 1;
    }
    int32_t c1 = xz_dist(m, pl);
    if (!chased)  { fprintf(stderr, "FAIL(2): maggot never entered CHASE (sub 3), sub=%d\n", m->sub_state_1); fail = 1; }
    if (c1 >= c0) { fprintf(stderr, "FAIL(2): maggot must crawl closer, dist %d->%d\n", c0, c1); fail = 1; }
    printf("  (2) CHASE: sub reached 3, dist %d->%d (closing)\n", c0, c1);

    /* (2b) BITE: with the player in range the maggot enters BITE (sub 5) and deals -6 on connect */
    pl->x = m->x; pl->z = m->z + 1200; pl->hp = 100; pl->hit_react = 0;   /* just ahead, inside the 3000 bite range */
    m->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - m->z, pl->x - m->x) - 0x400) & 0xfff);  /* face the player (the steered bearing) */
    m->dog_atk_cd = 0;
    int16_t bp0 = pl->hp; int bit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (m->sub_state_1 == 5) bit = 1;
        pl->hit_react = 0;                              /* clear so successive bites can land */
        if (pl->hp < bp0) break;
    }
    if (!bit)             { fprintf(stderr, "FAIL(2b): maggot never reached BITE (sub 5); sub=%d\n", m->sub_state_1); fail = 1; }
    if (pl->hp != bp0 - 6){ fprintf(stderr, "FAIL(2b): bite must deal exactly -6, hp %d->%d\n", bp0, pl->hp); fail = 1; }
    printf("  (2b) BITE: sub reached 5, player hp %d->%d (-6 on connect)\n", bp0, pl->hp);

    /* (3) KILLABLE: a lethal hit -> DEATH -> CORPSE */
    pl->x = 0; pl->z = 8000;   /* move the player away so the bite doesn't interfere */
    m->hp = 4; m->hit_react = 0;
    re15_enemy_take_damage(m, 2);              /* shared damage entry; hp goes <0 -> state 3 death */
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (m->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(3): a killed maggot must reach CORPSE (state 7), state=%d hp=%d\n", m->state, m->hp); fail = 1; }
    printf("  (3) DEATH: lethal hit -> death clip 0xe -> CORPSE (state %d, hp=%d)\n", m->state, m->hp);

    if (fail) { printf("MAGGOT WAVE-1: FAIL\n"); return 1; }
    printf("MAGGOT WAVE-1: all checks passed\n");
    return 0;
}
