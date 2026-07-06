/* ============================================================================
 *  Dog (Cerberus, type 0x20) AI — Wave 1 port probe.
 *
 *  Byte-true 0x8010d7f8 family (RE15_DOG_AI.md). Directly seeds a type-0x20 actor +
 *  a player and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x8010d93c): state 0 -> 1 (ACTIVE), idle clip 1, HP seeded.
 *   (2) ACTIVE brain: with the player close + in the FOV cone, the dog leaves idle,
 *       enters CHASE (sub 2) and moves TOWARD the player (distance closes).
 *   (3) Receiving side: a lethal hit (re15_enemy_take_damage) drives the dog through
 *       DEATH (state 3) to CORPSE (state 7) — the dog is killable, unlike the crow.
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

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0; d->rot_y = 0;  /* faces +z = toward player */
    re15_enemy_apply_hitbox(d, 0x20);

    printf("=== DOG (Cerberus, type 0x20) Wave-1 AI ===\n");

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

    /* (3) BITE: with the player in bite range, the dog enters ATTACK-RANGE -> BITE and deals -10 HP. */
    pl->x = d->x; pl->z = d->z + 1000; pl->hp = 100;   /* just ahead, inside the 2000/384 bite cone */
    int16_t php0 = pl->hp; int bit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        pl->hit_react = 0;                              /* clear so successive bites can land */
        if (d->sub_state_1 == 8) bit = 1;
        if (pl->hp < php0) break;
    }
    if (!bit)            { fprintf(stderr, "FAIL(3): dog never reached BITE (sub 8); sub=%d\n", d->sub_state_1); fail = 1; }
    if (pl->hp >= php0)  { fprintf(stderr, "FAIL(3): bite must deal damage, player HP %d->%d\n", php0, pl->hp); fail = 1; }
    printf("  (3) BITE: sub reached 8, player HP %d->%d (-10 on connect)\n", php0, pl->hp);

    /* (4) killable: a lethal hit -> DEATH -> CORPSE */
    d->hp = 4;
    re15_enemy_take_damage(d, 2);              /* generic damage entry (shared with the zombie) */
    int reached_corpse = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (d->state == 7) { reached_corpse = 1; break; }
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(4): a killed dog must reach CORPSE (state 7), state=%d hp=%d\n", d->state, d->hp); fail = 1; }
    printf("  (4) DEATH: lethal hit -> state 7 CORPSE (hp=%d)\n", d->hp);

    /* (5) POUNCE-LAND (state 5, 0x80111350): a grid-0x43 dog does the byte-true leap arc (rises) ->
     *     lands -> back to CHASE (state 1 / sub 2). A normal dog (grid 0) routes there safely (no freeze). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;
    re15_actor_t *pd = &g_actors[1];
    pd->active = 1; pd->type = 0x20; pd->grid_id = 0x43; pd->state = 5; pd->sub_state_1 = 0; pd->sub_state_2 = 0;
    pd->x = 0; pd->y = 0; pd->z = 0; pd->hp = 20;
    re15_enemy_apply_hitbox(pd, 0x20);
    int32_t pky = 32767; int landed_chase = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if ((int32_t)pd->y < pky) pky = pd->y;                  /* track the peak of the rise */
        if (pd->state == 1 && pd->sub_state_1 == 2) { landed_chase = 1; break; }  /* landed -> chase */
    }
    if (pky >= 0)         { fprintf(stderr, "FAIL(5): pounce-land must rise (y<0 at peak), peak y=%d\n", (int)pky); fail = 1; }
    if (!landed_chase)    { fprintf(stderr, "FAIL(5): pounce-land must land -> CHASE (state 1/sub 2), state=%d sub=%d\n", pd->state, pd->sub_state_1); fail = 1; }
    printf("  (5) POUNCE-LAND: grid-0x43 leap peaked y=%d, landed -> CHASE\n", (int)pky);

    /* (6) EATEN-GRAB (the deferred kill-cutscene, now byte-true): an ARMED bite escalates to the
     *     eaten GRAB (sub 9/10, @0x8010f47c) -> the player is PINNED (re15_player_is_grabbed) and,
     *     with no struggle input, the feed countdown devours him (hp<0 / state 7). */
    extern int re15_player_is_grabbed(void);
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1000; pl->hp = 100; pl->rot_y = 0;
    re15_actor_t *gg = &g_actors[1];
    gg->active = 1; gg->type = 0x20; gg->state = 1; gg->sub_state_1 = 8; gg->sub_state_2 = 0;   /* seeded in BITE */
    gg->x = 0; gg->y = 0; gg->z = 0; gg->rot_y = 0; gg->hp = 20; gg->dog_grab_armed = 1;         /* pre-armed (sub-5 windup) */
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

    /* (7) ARM-GRAB: a weak player (hp<50) makes the attack-range decision arm the eaten grab
     *     (sub 5 windup sets +0x1e4 @0x8010ed54, or the coin-flip picks the low-HP lunge sub 7). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1500; pl->hp = 30; pl->rot_y = 0;   /* weak */
    re15_actor_t *g7 = &g_actors[1];
    g7->active = 1; g7->type = 0x20; g7->state = 1; g7->sub_state_1 = 3; g7->sub_state_2 = 0;        /* ATTACK-RANGE */
    g7->x = 0; g7->y = 0; g7->z = 0; g7->hp = 20;
    g7->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - g7->z, pl->x - g7->x) - 0x400) & 0xfff);      /* face the player (the steered bearing) */
    re15_enemy_apply_hitbox(g7, 0x20);
    int armed = 0;
    for (int f = 0; f < 90; f++) {
        re15_enemy_ai_run_all(0);
        if (g7->dog_grab_armed != 0 || g7->sub_state_1 == 5 || g7->sub_state_1 == 7) { armed = 1; break; }
    }
    if (!armed) { fprintf(stderr, "FAIL(7): weak player -> attack-range must arm the grab (sub 5) or pick low-HP lunge (sub 7), sub=%d\n", g7->sub_state_1); fail = 1; }
    printf("  (7) ARM-GRAB: weak player -> sub=%d, dog_grab_armed=%d\n", g7->sub_state_1, g7->dog_grab_armed);

    /* (8) OBSTACLE-REROUTE: a dog seeded in sub 13 (wall contact, escape heading in ai_contact hi
     *     nibble) turns to the heading, runs the reroute clip 9, and returns to CHASE (sub 2). */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 5000; pl->y = 0; pl->z = 5000; pl->hp = 100;
    re15_actor_t *g8 = &g_actors[1];
    g8->active = 1; g8->type = 0x20; g8->state = 1; g8->sub_state_1 = 13; g8->sub_state_2 = 0;
    g8->x = 0; g8->y = 0; g8->z = 0; g8->rot_y = 0; g8->hp = 20; g8->ai_contact = 0x11;   /* heading nibble 1 (0x100) + contact bit */
    re15_enemy_apply_hitbox(g8, 0x20);
    int rerouted = 0;
    for (int f = 0; f < 300; f++) {
        re15_enemy_ai_run_all(0);
        if (g8->state == 1 && g8->sub_state_1 == 2) { rerouted = 1; break; }   /* returned to CHASE */
    }
    if (!rerouted) { fprintf(stderr, "FAIL(8): reroute (sub 13) must navigate + return to CHASE (sub 2), sub=%d rot=%u\n", g8->sub_state_1, (unsigned)g8->rot_y); fail = 1; }
    printf("  (8) REROUTE: sub 13 -> turned to escape heading -> ran -> CHASE (sub 2)\n");

    if (fail) { printf("DOG WAVE-1: FAIL\n"); return 1; }
    printf("DOG WAVE-1: all checks passed\n");
    return 0;
}
