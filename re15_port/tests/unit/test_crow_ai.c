/* ============================================================================
 *  Crow (type 0x21) 3D-flight AI — full flight-brain port probe.
 *
 *  Byte-true FUN_80112020 family (root state table @0x8012111c; full spec +
 *  disassembly citations in RE15_CROW_AI.md). Directly seeds a type-0x21 actor +
 *  a player (no RDT/SCD needed) and drives re15_enemy_ai_run_all, asserting:
 *
 *   (1) INIT (FUN_80111a4c): first tick moves state 0 -> 1 (ACTIVE), captures the
 *       perch height (+0x1ea = spawn y), applies the -400 lift-off, sets clip 0 and
 *       leaves the sub-state at 0 (patrol — the byte-true INIT does not touch +0x5).
 *   (2) The crow PATROLS in place while the player is far (> the 5000 dive ring),
 *       moving neither X nor Z (move[0-3] are pure clip/anim/timer).
 *   (3) With the player brought inside the ring, the dive-decide (steer[0-3]) fires:
 *       the crow leaves patrol (sub-state 4 launch) and engages the full flight loop,
 *       and its attack eventually connects — the player takes damage (dive -4 / grab -8).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 300; pl->y = 0; pl->z = 30000; pl->hp = 100;

    /* spawn a crow at (2000, 700, 300) facing rot 0 (state 0 = INIT, as op_sce_em_set seeds) */
    const int CS = 1;
    re15_actor_t *c = &g_actors[CS];
    c->active = 1; c->type = 0x21; c->state = 0;
    c->x = 2000; c->y = 700; c->z = 300; c->rot_y = 0;

    printf("=== CROW (type 0x21) full flight-brain ===\n");

    /* (1) INIT on the first tick */
    re15_enemy_ai_run_all(0);
    if (c->state != 1)        { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", c->state); fail = 1; }
    if (c->crow_perch_h != 700){ fprintf(stderr, "FAIL(1): perch +0x1ea=700, got %d\n", c->crow_perch_h); fail = 1; }
    if (c->y != 300)          { fprintf(stderr, "FAIL(1): -400 lift-off y=300, got %d\n", c->y); fail = 1; }
    if (c->sub_state_1 != 0)  { fprintf(stderr, "FAIL(1): INIT leaves patrol sub 0, got %d\n", c->sub_state_1); fail = 1; }
    if (c->motion != 0)       { fprintf(stderr, "FAIL(1): clip 0 expected, got %d\n", c->motion); fail = 1; }
    printf("  (1) INIT: state->1, perch=%d, y=%d, sub=%d, clip=%d\n",
           c->crow_perch_h, c->y, c->sub_state_1, c->motion);

    /* (2) player FAR (z=30000, dist ~30000 >> 5000 ring): crow patrols in place */
    int32_t px0 = c->x, pz0 = c->z;
    int patrolled_still = 1;
    for (int f = 0; f < 40; f++) {
        re15_enemy_ai_run_all(0);
        if (c->sub_state_1 > 3) { patrolled_still = 0; break; }   /* left patrol early = wrong */
    }
    if (!patrolled_still) { fprintf(stderr, "FAIL(2): crow left patrol with player far (sub=%d)\n", c->sub_state_1); fail = 1; }
    if (c->x != px0 || c->z != pz0) { fprintf(stderr, "FAIL(2): patrol must not move XZ, (%d,%d)->(%d,%d)\n", (int)px0,(int)pz0,(int)c->x,(int)c->z); fail = 1; }
    printf("  (2) PATROL: player far -> sub stays %d, position held @(%d,%d)\n", c->sub_state_1, (int)c->x, (int)c->z);

    /* (3) bring the player INTO the ring (close, at the crow's altitude band) and drive the
     *     flight loop; the crow must engage (leave patrol) and its attack must connect. */
    pl->x = c->x; pl->z = c->z + 800; pl->y = c->y; pl->hp = 100;
    int max_sub = 0, engaged = 0; int16_t hp_before = pl->hp;
    for (int f = 0; f < 600; f++) {
        re15_enemy_ai_run_all(0);
        if (c->sub_state_1 >= 4) engaged = 1;
        if (c->sub_state_1 > max_sub) max_sub = c->sub_state_1;
        pl->hit_react = 0;                /* clear the once-per-contact gate so re-hits can land */
        if (!c->active) break;            /* (defensive) */
    }
    if (!engaged) { fprintf(stderr, "FAIL(3): crow never engaged (max sub-state %d) with player in range\n", max_sub); fail = 1; }
    if (pl->hp >= hp_before) { fprintf(stderr, "FAIL(3): crow attack never connected, player HP %d->%d\n", hp_before, pl->hp); fail = 1; }
    printf("  (3) ENGAGE: max sub-state %d, player HP %d->%d (attack connected)\n", max_sub, hp_before, pl->hp);

    /* (4) FLIGHT-2: the grid&0x40 event crow — INIT overrides to state 4, then ASCEND (climbs) ->
     *     HOVER; it never writes +0x4 (no state exit) so it stays in FLIGHT-2 (byte-true). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 300; pl->y = 0; pl->z = 30000; pl->hp = 100;
    re15_actor_t *ev = &g_actors[1];
    ev->active = 1; ev->type = 0x21; ev->state = 0; ev->grid_id = 0x40;   /* the event crow */
    ev->x = 2000; ev->y = 700; ev->z = 300; ev->rot_y = 0;
    re15_enemy_ai_run_all(0);
    if (ev->state != 4) { fprintf(stderr, "FAIL(4): grid&0x40 crow must INIT to FLIGHT-2 state 4, got %d\n", ev->state); fail = 1; }
    int32_t f2y0 = ev->y; int f2_hover = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (ev->state != 4) break;                       /* must never leave state 4 */
        if (ev->sub_state_1 == 1) f2_hover = 1;          /* reached HOVER */
    }
    if (ev->state != 4) { fprintf(stderr, "FAIL(4): FLIGHT-2 crow left state 4 (state=%d) — should never exit\n", ev->state); fail = 1; }
    if (ev->y >= f2y0)   { fprintf(stderr, "FAIL(4): ASCEND must climb (smaller y), y %d->%d\n", (int)f2y0, (int)ev->y); fail = 1; }
    if (!f2_hover)       { fprintf(stderr, "FAIL(4): FLIGHT-2 crow never reached HOVER (sub 1)\n"); fail = 1; }
    printf("  (4) FLIGHT-2: grid&0x40 -> state 4, ascended y %d->%d, reached HOVER, stayed in state 4\n", (int)f2y0, (int)ev->y);

    /* (5) DEATH: the scripted death bit (0x1f, STAGE3/5) promotes a state-4 grid&0x40 crow to
     *     DEATH (state 3) -> fall from the sky -> land -> CORPSE (state 7). Byte-true unreachable
     *     in STAGE1, so the test triggers the event explicitly via re15_crow_death_event(). */
    {
        extern void re15_crow_death_event(void);
        memset(g_actors, 0, sizeof g_actors);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 300; pl->y = 0; pl->z = 30000; pl->hp = 100;
        re15_actor_t *dc = &g_actors[1];
        dc->active = 1; dc->type = 0x21; dc->state = 0; dc->grid_id = 0x40;
        dc->x = 2000; dc->y = 700; dc->z = 300; dc->rot_y = 0;
        for (int f = 0; f < 40; f++) re15_enemy_ai_run_all(0);   /* INIT -> state 4, ASCEND (climbs high) */
        int32_t dy0 = dc->y;
        re15_crow_death_event();                                  /* the scripted "crows die" trigger */
        int died = 0, corpse = 0;
        for (int f = 0; f < 400; f++) {
            re15_enemy_ai_run_all(0);
            if (dc->state == 3) died = 1;
            if (dc->state == 7) { corpse = 1; break; }
        }
        if (!died)   { fprintf(stderr, "FAIL(5): bit-0x1f must promote the state-4 crow to DEATH (state 3)\n"); fail = 1; }
        if (!corpse) { fprintf(stderr, "FAIL(5): the dead crow must fall + land -> CORPSE (state 7), state=%d\n", dc->state); fail = 1; }
        printf("  (5) DEATH: bit-0x1f -> state 3 (fell from y=%d) -> state 7 CORPSE @y=%d\n", (int)dy0, (int)dc->y);
    }

    if (fail) { printf("CROW FLIGHT-BRAIN: FAIL\n"); return 1; }
    printf("CROW FLIGHT-BRAIN: all checks passed\n");
    return 0;
}
