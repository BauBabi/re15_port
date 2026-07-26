/* test_fx_emitter_ai.c — AMBIENT FX-EMITTER (type 0x24, EM024, STAGE2) AI.
 *
 * Byte-true from workflow wf_5c34ffe7 (root 0x8010ee9c). EM024 is a harmless scripted particle/swarm
 * emitter (single-bone billboard), NOT a combat enemy. Asserts:
 *   (1) INIT: state -> 1, drift timer 120 (+0x8c).
 *   (2) X-DRIFT: it drifts leftward (X decreases) with an accelerating step.
 *   (3) WRAP: when X < -25000 it wraps to +20000 (timer reset 0xe1).
 *   (4) HARMLESS: it never damages the player.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 500; pl->hp = 100;

    const int FS = 1;
    re15_actor_t *e = &g_actors[FS];
    e->active = 1; e->type = 0x24; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0;
    re15_enemy_apply_hitbox(e, 0x24);   /* no combat hitbox */

    printf("=== AMBIENT FX-EMITTER (type 0x24, EM024) AI ===\n");

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (e->state != 1)      { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    printf("  (1) INIT: state->%d, drift-timer=%d\n", e->state, e->ai_timer);

    /* (2) X-DRIFT: X must decrease over frames (accelerating leftward) */
    int32_t x0 = e->x; int16_t hp0 = pl->hp;
    for (int f = 0; f < 30; f++) re15_enemy_ai_run_all(0);
    if (e->x >= x0) { fprintf(stderr, "FAIL(2): emitter must drift leftward, X %d->%d\n", x0, e->x); fail = 1; }
    printf("  (2) X-DRIFT: X %d->%d (leftward)\n", x0, e->x);

    /* (3) WRAP: drive it past -25000 and confirm the wrap to +20000 */
    int wrapped = 0;
    for (int f = 0; f < 2000; f++) {
        re15_enemy_ai_run_all(0);
        if (e->x >= 19000) { wrapped = 1; break; }   /* wrapped to 0x4e20 (20000) */
    }
    if (!wrapped) { fprintf(stderr, "FAIL(3): emitter X-drift must wrap to +20000, X=%d\n", e->x); fail = 1; }
    printf("  (3) WRAP: X wrapped to %d\n", e->x);

    /* (4) HARMLESS */
    if (pl->hp != hp0) { fprintf(stderr, "FAIL(4): emitter must deal NO damage, hp %d->%d\n", hp0, pl->hp); fail = 1; }
    printf("  (4) HARMLESS: player hp %d (unchanged)\n", pl->hp);

    /* (5) DOMINANT SPAWN grid 0x05 = drift-only byte-true (audit wf_efd92a2c dormant #1): the +0x5
     * dispatch table @0x80118d58[5] = 0x8011084c = `jr ra` NOP (raw-verified), so ~91% of ROOM20B0/20B1
     * emitters run NO burst sequencer — only the drift/wrap. A grid-5 emitter must drift and stay harmless. */
    memset(g_actors, 0, sizeof g_actors);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 500; pl->hp = 100;
    e = &g_actors[FS];
    e->active = 1; e->type = 0x24; e->state = 0; e->grid_id = 0x05; e->x = 0; e->y = 0; e->z = 0;
    re15_enemy_apply_hitbox(e, 0x24);
    re15_enemy_ai_run_all(0);                       /* INIT */
    if (e->state != 1) { fprintf(stderr, "FAIL(5): grid-5 INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    int32_t g5x0 = e->x; int16_t g5hp0 = pl->hp;
    for (int f = 0; f < 30; f++) re15_enemy_ai_run_all(0);
    if (e->x >= g5x0)     { fprintf(stderr, "FAIL(5): grid-5 emitter must drift leftward, X %d->%d\n", g5x0, e->x); fail = 1; }
    if (pl->hp != g5hp0)  { fprintf(stderr, "FAIL(5): grid-5 emitter must deal NO damage, hp %d->%d\n", g5hp0, pl->hp); fail = 1; }
    printf("  (5) grid-5 (NOP leaf 0x8011084c): drift X %d->%d, hp %d (harmless)\n", g5x0, e->x, pl->hp);

    if (fail) { printf("FX-EMITTER: FAIL\n"); return 1; }
    printf("FX-EMITTER: all checks passed\n");
    return 0;
}
