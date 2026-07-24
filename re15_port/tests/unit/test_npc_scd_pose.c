/* ============================================================================
 *  NPC SCD-owns-animation yield — regression guard for 4cf6bead.
 *
 *  Bug (reported by the user, Irons ROOM1150 + other rooms): a cutscene NPC that
 *  the SCD has posed with Plc_motion would LOOP the wrong animation. Root cause:
 *  the SCD Plc_motion sets the NPC's pose (e.g. Irons lying = motion 3) once, its
 *  setter thread then ends, and re15_npc_ai_tick's INIT (state 0) stomps the pose
 *  back to idle clip 2 which the state-1 idle-overlay then loops at the wrong EM040
 *  clip length. The byte-true original never lets the NPC dispatch run while a
 *  cutscene owns the actor: the shared enemy/NPC root FUN_8011c654 @0x8011c654
 *  (line 19, read by 108 roots) skips its +0x4 state dispatch while the AI-freeze
 *  DAT_800aca40 & 0x20000000 is set. The port emulates this per-actor with the
 *  scd_anim_owned latch (set by op_plc_motion, cleared at spawn), and yields the
 *  WHOLE dispatch (HP=-1, +0x9a @0x8011c744) in re15_npc_ai_tick while it is set.
 *
 *  This test drives that path with NO SDL / NO assets (engine lib only), so it is
 *  reproducible in any session — no display required. It asserts:
 *   (A) SCD-posed NPC: motion STAYS at the Plc_motion clip (NOT stomped to idle 2),
 *       state STAYS 0 (tick yields before the state switch), hp = -1 (invulnerable).
 *       -> WITHOUT the fix this fails: motion would become 2 and state -> 1 (the loop).
 *   (B) the pose PLAYS: re15_actors_anim_advance advances anim_frame monotonically
 *       and never wraps back to 0 (the render HOLD-LASTs it at the real EMD length).
 *   (C) control: a NON-SCD-posed NPC still runs INIT normally (state 1, idle clip 2,
 *       hp = -1) -> proves stationary NPCs are unaffected (T-pose fix preserved).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"   /* re15_actors_anim_advance */

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;

    /* Slot 1: Chief Irons (0x40) that the SCD has Plc_motion'd into a cutscene pose.
     * op_plc_motion leaves: motion = the pose clip (3 = Irons lying), scd_anim_owned = 1,
     * and (freshly spawned) state = 0. This is the exact state the game loop ticks. */
    const int SCD = 1;
    re15_actor_t *n = &g_actors[SCD];
    n->active = 1; n->type = 0x40; n->state = 0; n->grid_id = 0;
    n->x = 1000; n->y = 0; n->z = 1000;
    n->motion = 3;            /* Plc_motion pose (Irons lying) */
    n->anim_frame = 0;
    n->scd_anim_owned = 1;    /* the SCD owns this actor's clip (op_plc_motion latch) */

    /* Slot 2: a plain stationary NPC (control) — never Plc_motion'd. */
    const int STN = 2;
    re15_actor_t *c = &g_actors[STN];
    c->active = 1; c->type = 0x40; c->state = 0; c->grid_id = 0;
    c->x = -1000; c->y = 0; c->z = 1000;

    printf("=== NPC SCD-owns-animation yield (regression guard, 4cf6bead) ===\n");

    /* Drive the game-loop order: re15_actors_anim_advance (game_step_common.c:566)
     * BEFORE re15_enemy_ai_run_all (:574), for many frames. */
    uint8_t  last_af   = n->anim_frame;
    int      af_advanced = 0;
    int      af_wrapped  = 0;   /* any reset-to-0 after advancing = the LOOP bug */
    for (int f = 0; f < 240; f++) {
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);

        if (n->anim_frame > last_af) af_advanced = 1;
        if (n->anim_frame < last_af) af_wrapped  = 1;   /* wrapped back = looped */
        last_af = n->anim_frame;
    }

    /* (A) SCD-posed NPC: pose held, dispatch yielded, invulnerable. */
    if (n->motion != 3) { fprintf(stderr, "FAIL(A): SCD pose stomped — motion %d != 3 (idle-clip regression)\n", n->motion); fail = 1; }
    if (n->state != 0)  { fprintf(stderr, "FAIL(A): tick did not yield — state %d != 0 (ran the dispatch)\n", n->state); fail = 1; }
    if (n->hp != -1)    { fprintf(stderr, "FAIL(A): yielded NPC must be invulnerable, hp=%d\n", n->hp); fail = 1; }
    printf("  (A) SCD-posed: motion=%d (held), state=%d (yielded), hp=%d\n", n->motion, n->state, n->hp);

    /* (B) the pose plays monotonically and never loops back to 0. */
    if (!af_advanced) { fprintf(stderr, "FAIL(B): Plc_motion pose never advanced (frozen at frame 0)\n"); fail = 1; }
    if (af_wrapped)   { fprintf(stderr, "FAIL(B): anim_frame wrapped back to 0 — the LOOP regression\n"); fail = 1; }
    printf("  (B) pose plays: anim_frame advanced=%d, wrapped(looped)=%d, final=%d\n",
           af_advanced, af_wrapped, n->anim_frame);

    /* (C) control NPC (no SCD lock): normal INIT — state 1, idle clip 2, invulnerable. */
    if (c->state != 1)  { fprintf(stderr, "FAIL(C): control NPC INIT broken — state %d != 1\n", c->state); fail = 1; }
    if (c->motion != 2) { fprintf(stderr, "FAIL(C): control NPC must idle clip 2, got %d (T-pose fix regressed)\n", c->motion); fail = 1; }
    if (c->hp != -1)    { fprintf(stderr, "FAIL(C): control NPC must be invulnerable, hp=%d\n", c->hp); fail = 1; }
    printf("  (C) control (no lock): state=%d, idle clip=%d, hp=%d\n", c->state, c->motion, c->hp);

    if (fail) { printf("NPC SCD-POSE: FAIL\n"); return 1; }
    printf("NPC SCD-POSE: all checks passed\n");
    return 0;
}
