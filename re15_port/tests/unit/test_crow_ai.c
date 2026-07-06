/* ============================================================================
 *  Crow (type 0x21) 3D-flight AI — Wave 1 port probe.
 *
 *  Byte-true FUN_80112020 family (root state table @0x8012111c; full spec +
 *  disassembly citations in RE15_CROW_AI.md). Directly seeds a type-0x21 actor +
 *  a player (no RDT/SCD needed) and drives the single entry re15_enemy_ai_run_all,
 *  asserting the byte-true Wave-1 behaviour:
 *
 *   (1) INIT (FUN_8011224c): first tick moves state 0 -> 1 (ACTIVE), captures the
 *       perch height (+0x1ea = spawn y, @0x801123a0), applies the -400 lift-off
 *       (+0x38 -= 400, @0x801123b8), sets clip 0 and the cruise sub-state 6.
 *   (2) ACTIVE cruise (FUN_80112420 + move[6] 0x80112d34): the crow flies TOWARD
 *       the player — the yaw-slew (rate 50, 0x8001a8f8) turns it toward the player
 *       and the horizontal advance closes the XZ distance. With its mode byte 0 (the
 *       0x80116068 climb-rate command path is a later wave) the vertical velocity is
 *       (mode & 0x3f) * dir = 0, so it HOLDS altitude — the byte-true mode=0 result.
 *   (3) TYPE GATE: the crow rides its OWN run_all branch (not the zombie path).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"

/* integer XZ distance (avoids linking libm just for the test) */
static int32_t xz_dist(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t d2 = dx * dx + dz * dz;
    int32_t r = 0;
    while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    return r;
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    /* player at world (300, 0, 9550) — y is the altitude axis (the crow reads playerY) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 300; pl->y = 0; pl->z = 9550; pl->hp = 100;

    /* spawn a crow at (2000, 700, 300) facing rot 0 — exactly as op_sce_em_set seeds a
     * fresh enemy: active, type, state 0 (INIT), world position. */
    const int CS = 1;
    re15_actor_t *c = &g_actors[CS];
    c->active = 1; c->type = 0x21; c->state = 0;
    c->x = 2000; c->y = 700; c->z = 300; c->rot_y = 0;

    printf("=== CROW (type 0x21) Wave-1 flight AI ===\n");

    /* (1) INIT on the first tick */
    re15_enemy_ai_run_all(0);
    if (c->state != 1) {
        fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", c->state); fail = 1; }
    if (c->crow_perch_h != 700) {
        fprintf(stderr, "FAIL(1): perch height +0x1ea=700 (spawn y), got %d\n", c->crow_perch_h); fail = 1; }
    if (c->y != 300) {
        fprintf(stderr, "FAIL(1): -400 lift-off expected y=300, got %d\n", c->y); fail = 1; }
    if (c->sub_state_1 != 6) {
        fprintf(stderr, "FAIL(1): cruise sub-state 6 expected, got %d\n", c->sub_state_1); fail = 1; }
    if (c->motion != 0) {
        fprintf(stderr, "FAIL(1): clip 0 expected, got %d\n", c->motion); fail = 1; }
    printf("  (1) INIT: state->1, perch=%d, y=%d (lifted -400), sub=%d, clip=%d\n",
           c->crow_perch_h, c->y, c->sub_state_1, c->motion);

    /* (2) ACTIVE cruise: fly toward the player, hold altitude */
    int32_t d0 = xz_dist(c, pl);
    int16_t rot0 = c->rot_y;
    int32_t y0 = c->y;
    for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
    int32_t d1 = xz_dist(c, pl);
    if (d1 >= d0) {
        fprintf(stderr, "FAIL(2): crow must fly TOWARD the player, dist %d->%d\n", d0, d1); fail = 1; }
    if (c->rot_y == rot0) {
        fprintf(stderr, "FAIL(2): yaw must slew toward the player, rot stuck at %d\n", rot0); fail = 1; }
    if (c->y != y0) {
        fprintf(stderr, "FAIL(2): mode=0 -> altitude held, y %d->%d\n", y0, c->y); fail = 1; }
    printf("  (2) CRUISE: dist %d->%d (closing), rot %d->%d (slew), y held at %d\n",
           d0, d1, rot0, c->rot_y, c->y);

    /* (3) type gate: still ACTIVE, never routed through the zombie machine */
    if (c->state != 1) {
        fprintf(stderr, "FAIL(3): crow left ACTIVE unexpectedly (state=%d)\n", c->state); fail = 1; }

    if (fail) { printf("CROW WAVE-1: FAIL\n"); return 1; }
    printf("CROW WAVE-1: all checks passed\n");
    return 0;
}
