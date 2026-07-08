/* test_aim_all_around.c — the player auto-aim LATCH selects the NEAREST enemy ALL-AROUND.
 *
 * Byte-true FUN_8003703c @0x8003703c: the latch computes ONLY SquareRoot0(dx^2+dz^2) and keeps the
 * smallest (3 flag-priority buckets) — there is NO angle-vs-facing test anywhere in the function.
 * The caller then AUTO-TURNS the player toward the pick (player_common.c:350-354 slews rot_y, the
 * original's FUN_8001a8f8). The port used to add a front-hemisphere arc gate in re15_player_aim_target
 * (re15_ai_arc_test(pl, ex,ez, 0x400) == 0) that dropped every enemy behind/beside the player — so the
 * player could never turn to face an off-axis threat. This proves the gate is gone:
 *   (1) EVERY position the front-arc test rejects (behind/beside) is STILL targeted -> selection is
 *       all-around, not front-only.
 *   (2) NEAREST wins across the facing: a near BEHIND enemy beats a far FRONT enemy (the old gate
 *       would have filtered the behind one out and locked the far front one).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);
extern int re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone);

/* a ring of 16 compass positions around the player at a given radius */
static void ring_pos(int i, int32_t r, int32_t *x, int32_t *z)
{
    /* 16 evenly-spaced points via a small integer octant table (no sin/cos needed) — magnitudes
     * are only used to place enemies in all quadrants, exact roundness is irrelevant. */
    static const int oct[5] = { 100, 92, 71, 38, 0 };   /* cos*100 at 0/22.5/45/67.5/90 deg */
    int q = (i & 3), s = (i >> 2) & 3;
    int a = q, cx, cz;
    (void)a;
    int c = oct[q], sn = oct[4 - q];
    switch (s) {
        case 0: cx =  c; cz =  sn; break;   /* +x +z */
        case 1: cx = -sn; cz =  c; break;   /* -x +z */
        case 2: cx = -c; cz = -sn; break;   /* -x -z */
        default: cx =  sn; cz = -c; break;  /* +x -z */
    }
    *x = (r * cx) / 100;
    *z = (r * cz) / 100;
}

static void set_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0; pl->floor = 0; pl->hp = 100;
}

int main(void)
{
    int fail = 0;
    printf("=== auto-aim latch: NEAREST enemy ALL-AROUND (FUN_8003703c, no front-arc gate) ===\n");

    /* (1) sweep a ring; for each position the FRONT-arc test rejects, a single enemy there must
     *     STILL be the aim target. */
    int behind_tested = 0, behind_ok = 0;
    for (int i = 0; i < 16; i++) {
        memset(g_actors, 0, sizeof g_actors);
        set_player();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *e = &g_actors[1];
        int32_t ex, ez; ring_pos(i, 3000, &ex, &ez);
        e->active = 1; e->type = 0x10; e->state = 1; e->x = ex; e->z = ez; e->rot_y = 0; e->hp = 100;
        re15_enemy_apply_hitbox(e, 0x10);   /* zombie box 400/1440 -> the latch considers it */

        int rejected_by_front_gate = (re15_ai_arc_test(pl, e->x, e->z, 0x400) != 0);
        int32_t tx = 0, tz = 0;
        int hit = re15_player_aim_target(30000, &tx, &tz);

        if (!hit || tx != ex || tz != ez) {
            fprintf(stderr, "FAIL(1): enemy at (%d,%d)%s NOT targeted (hit=%d)\n",
                    ex, ez, rejected_by_front_gate ? " [behind/beside]" : " [front]", hit);
            fail = 1;
        } else if (rejected_by_front_gate) {
            behind_tested++; behind_ok++;
        }
    }
    if (behind_tested == 0) { fprintf(stderr, "FAIL(1): test geometry bug — no behind/beside position hit the front-arc gate\n"); fail = 1; }
    else printf("  (1) all-around: %d/%d behind/beside enemies correctly targeted (front gate removed)\n", behind_ok, behind_tested);

    /* (2) NEAREST across the facing: a near BEHIND enemy (r=1500) beats a far FRONT enemy (r=9000). */
    {
        /* find a ring index that the front gate rejects, for the near enemy */
        int bi = -1; int32_t bx = 0, bz = 0;
        for (int i = 0; i < 16 && bi < 0; i++) {
            int32_t x, z; ring_pos(i, 1500, &x, &z);
            re15_actor_t probe; memset(&probe, 0, sizeof probe); probe.rot_y = 0; /* player facing */
            /* arc_test's first arg is the player (origin, rot 0) */
            memset(g_actors, 0, sizeof g_actors); set_player();
            if (re15_ai_arc_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], x, z, 0x400) != 0) { bi = i; bx = x; bz = z; }
            (void)probe;
        }
        if (bi < 0) { fprintf(stderr, "FAIL(2): no behind position found at r=1500\n"); fail = 1; }
        else {
            memset(g_actors, 0, sizeof g_actors);
            set_player();
            re15_actor_t *far_front = &g_actors[1];
            far_front->active = 1; far_front->type = 0x10; far_front->state = 1;
            far_front->x = 0; far_front->z = 9000; far_front->rot_y = 0; far_front->hp = 100;
            re15_enemy_apply_hitbox(far_front, 0x10);
            re15_actor_t *near_behind = &g_actors[2];
            near_behind->active = 1; near_behind->type = 0x10; near_behind->state = 1;
            near_behind->x = bx; near_behind->z = bz; near_behind->rot_y = 0; near_behind->hp = 100;
            re15_enemy_apply_hitbox(near_behind, 0x10);

            int32_t tx = 0, tz = 0;
            int hit = re15_player_aim_target(30000, &tx, &tz);
            if (!hit || tx != bx || tz != bz) {
                fprintf(stderr, "FAIL(2): near BEHIND enemy at (%d,%d) must win over far FRONT (0,9000); got hit=%d (%d,%d)\n",
                        bx, bz, hit, tx, tz);
                fail = 1;
            } else {
                printf("  (2) nearest wins across the facing: near behind (%d,%d) beat far front (0,9000)\n", bx, bz);
            }
        }
    }

    if (fail) { printf("AIM-ALL-AROUND: FAIL\n"); return 1; }
    printf("AIM-ALL-AROUND: all checks passed\n");
    return 0;
}
