/* test_aim_lower_exit.c — regression: the aim LOWER must not flash the INJURED-IDLE pose.
 *
 * BUG (measured 2026-07-28, root-Y instrument + render capture): the port's aim-pose motion
 * sentinel RE15_MOTION_AIM_W == 213 collides with RE15_MOTION_IDLE_HURT1 == 213. On the tick
 * the aim LOWER completes, player_common.c clears the aim phase (re15_player_aim_active()==0)
 * but left want_motion at 213 for that one tick. The renderer's aim override is gated on
 * re15_player_aim_active(), so with the phase already cleared it no longer applies, and
 * anim_select maps motion 213 -> PL00 base clip 22 = the INJURED-IDLE hunch. That hunched pose
 * seeds the FRAC crossfade -> Leon visibly bends forward and rises = a spurious "land animation"
 * at the end of EVERY aim-release, on every weapon.
 *
 * INVARIANT: whenever the aim FSM is INACTIVE, the (full-HP) player's motion must never be the
 * AIM_W sentinel (213). The fix drops to the idle sentinel (200) the same tick the LOWER exits.
 */
#include "re15_player.h"       /* re15_player_tick + RE15_PAD_BIT_R1                         */
#include "re15_actor.h"        /* g_actors + RE15_ACTOR_SLOT_PLAYER + re15_actor_init        */
#include "re15_damage.h"       /* re15_player_set_equipped_weapon (gun class = item >= 3)    */
#include <stdio.h>
#include <stdint.h>

/* player_common.c:129 / :97 — the two sentinels that must not be conflated after aim ends. */
#define AIM_W_SENTINEL 213

/* Defined in player_common.c (not surfaced in re15_player.h; declared extern like main.c does). */
extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

int main(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = pl->y = pl->z = 0; pl->rot_y = 0;
    pl->hp = 100;                 /* FULL HP -> motion 213 after aim ends can ONLY be the bug   */
    pl->state = 0;
    pl->motion = 200;             /* start idle                                                 */

    re15_player_set_equipped_weapon(3);   /* BROWNING HP (gun class) — RAISE clip 6, HOLD 8     */
    re15_player_set_aim_clip_len(6);      /* small per-clip fc so raise/hold/lower advance fast */
    re15_player_aim_reset();

    int fail = 0;
    printf("=== aim LOWER exit must not flash the injured-idle sentinel (213) ===\n");

    /* Hold R1: RAISE -> HOLD. The aim FSM must engage. */
    int engaged = 0;
    for (int f = 0; f < 24; f++) {
        re15_player_tick(NULL, RE15_PAD_BIT_R1);
        if (re15_player_aim_active()) engaged = 1;
    }
    if (!engaged) { fprintf(stderr, "FAIL: holding R1 never engaged the aim FSM\n"); fail = 1; }

    /* Release R1: the LOWER plays out, then the aim FSM exits. On the exit tick and every tick
     * after, motion must have LEFT the AIM_W sentinel (else the renderer poses the injured-idle
     * hunch). Track the exact tick aim goes inactive and assert motion != 213 from then on. */
    int inactive_seen = 0, exit_motion = -1, bad_frame = -1;
    for (int f = 0; f < 40; f++) {
        re15_player_tick(NULL, 0);           /* R1 released */
        if (!re15_player_aim_active()) {
            if (!inactive_seen) { inactive_seen = 1; exit_motion = (int)pl->motion; }
            if ((int)pl->motion == AIM_W_SENTINEL && bad_frame < 0) bad_frame = f;
        }
    }

    if (!inactive_seen) {
        fprintf(stderr, "FAIL: aim never released after dropping R1\n"); fail = 1;
    } else if (bad_frame >= 0) {
        fprintf(stderr, "FAIL: motion left at AIM_W(213) while aim INACTIVE (tick %d) "
                        "-> injured-idle hunch flash = the 'land' bug\n", bad_frame); fail = 1;
    } else if (exit_motion == AIM_W_SENTINEL) {
        fprintf(stderr, "FAIL: exit-tick motion == 213 (the collision)\n"); fail = 1;
    } else {
        printf("  aim released cleanly: exit-tick motion = %d (idle/locomotion, not 213)\n",
               exit_motion);
    }

    printf(fail ? "RESULT: FAIL\n" : "RESULT: PASS\n");
    return fail;
}
