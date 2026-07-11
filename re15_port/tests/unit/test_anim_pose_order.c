/* test_anim_pose_order.c — the byte-true pose-then-advance ordering + the 0x8000 marker skip
 * (audit KF-1/ADV-BLEND-1 + KF-2/ANIM-0x8000, skeleton/anim audit wf_8a6bd306).
 *
 * (A) FUN_8001f3bc poses the CURRENT counters then POST-advances (+0x95 ++ @0x8001f610-63c, +0x8f --
 *     @0x8001f5b0-b4), so the first rendered frame of a fresh clip is slot 0 with frac 7 (87.5% prev).
 *     The port advances in re15_player_tick and poses at render; the advance used to run at the END
 *     of the tick, AFTER the motion-change seed — so the render saw (1,6) and slot 0 was never shown.
 *     Moved to the TOP of the tick (advance-then-seed-then-pose == pose-then-post-advance): the seed
 *     survives to the render, and next tick's FSM reads the post-advance counter like the original.
 * (B) FUN_8001f314 (@0x8001f370-f37c) never poses a frame entry with bit 0x8000 — it routes to
 *     FUN_8001f8b4, which forward-scans past every marker (wrap at frame_count) and poses the next
 *     REAL frame (the marker's low 12 bits are a blend weight, NOT a keyframe index). The port's
 *     LOOP/forward branches used to pose `marker & 0xFFF` as a garbage keyframe for one tick.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_anim_select.h"

int main(void)
{
    int fail = 0;
    printf("=== pose-then-advance ordering + 0x8000 marker skip (wf_8a6bd306) ===\n");

    /* (A) player motion-change seed survives to the render: after the seeding tick the counters the
     *     render reads must be (0,7); the following ticks advance (1,6), (2,5) — the byte-true
     *     render sequence 0,1,2 with frac 7,6,5. */
    {
        scd_vm_init();
        re15_aot_init();
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0; p->hp = 100; p->motion = 0;
        p->anim_frame = 99; p->anim_frac = 0;                    /* stale pre-seed state */

        re15_player_tick(NULL, RE15_PAD_BIT_UP);                 /* seed tick: idle -> walk */
        if (p->anim_frame != 0 || p->anim_frac != 7) {
            fprintf(stderr, "FAIL(A1): seed tick must leave (frame,frac)=(0,7) for the render, got (%u,%u)\n",
                    p->anim_frame, p->anim_frac);
            fail = 1;
        }
        re15_player_tick(NULL, RE15_PAD_BIT_UP);                 /* steady tick 2 */
        if (p->anim_frame != 1 || p->anim_frac != 6) {
            fprintf(stderr, "FAIL(A2): tick 2 must render (1,6), got (%u,%u)\n", p->anim_frame, p->anim_frac);
            fail = 1;
        }
        re15_player_tick(NULL, RE15_PAD_BIT_UP);                 /* steady tick 3 */
        if (p->anim_frame != 2 || p->anim_frac != 5) {
            fprintf(stderr, "FAIL(A3): tick 3 must render (2,5), got (%u,%u)\n", p->anim_frame, p->anim_frac);
            fail = 1;
        }
        if (!fail) printf("  (A) player seed renders slot 0/frac 7 first; sequence (0,7),(1,6),(2,5)\n");
    }

    /* (B) the 0x8000 marker is never posed: LOOP wraps forward to frame 0, plain-forward skips, the
     *     HOLD-LAST backward scan is unchanged, reverse playback skips backward. */
    {
        re15_emd_animation_t anim; memset(&anim, 0, sizeof anim);
        re15_emd_skeleton_t  skel; memset(&skel, 0, sizeof skel);
        skel.keyframe_count = 16;
        anim.clip_count = 1; anim.frame_count = 4;
        anim.clips[0].first_frame = 0; anim.clips[0].frame_count = 4;
        anim.frames[0] = 5; anim.frames[1] = 6; anim.frames[2] = 7;
        anim.frames[3] = 0x8000u | 0x0FFFu;                       /* terminal marker, garbage kf bits */

        re15_actor_t a; memset(&a, 0, sizeof a);

        int kf = re15_compute_actor_kf(&anim, &skel, &a, /*loop*/0, /*cur=*/3);   /* LOOP seam on the marker */
        if (kf != 5) { fprintf(stderr, "FAIL(B1): LOOP marker slot must wrap to frame 0 (kf 5), got %d\n", kf); fail = 1; }

        a.motion = 0;
        kf = re15_compute_actor_kf(&anim, &skel, &a, /*no override*/-1, 3);       /* plain-forward on the marker */
        if (kf != 5) { fprintf(stderr, "FAIL(B2): forward marker slot must skip (kf 5), got %d\n", kf); fail = 1; }

        kf = re15_compute_actor_kf(&anim, &skel, &a, -1, 10);                     /* HOLD-LAST (cur > fc) */
        if (kf != 7) { fprintf(stderr, "FAIL(B3): hold-last must keep the last REAL frame (kf 7), got %d\n", kf); fail = 1; }

        a.anim_flags = 0x80;                                                      /* reverse playback */
        kf = re15_compute_actor_kf(&anim, &skel, &a, 0, 0);                       /* rev slot = fc-1 = marker */
        if (kf != 7) { fprintf(stderr, "FAIL(B4): reverse marker slot must skip backward (kf 7), got %d\n", kf); fail = 1; }

        a.anim_flags = 0;
        kf = re15_compute_actor_kf(&anim, &skel, &a, 0, 1);                       /* non-marker sanity */
        if (kf != 6) { fprintf(stderr, "FAIL(B5): non-marker slot must be unchanged (kf 6), got %d\n", kf); fail = 1; }

        if (!fail) printf("  (B) 0x8000 markers never posed: LOOP wraps, forward/reverse skip, hold-last intact\n");
    }

    if (fail) { printf("ANIM-POSE-ORDER: FAIL\n"); return 1; }
    printf("ANIM-POSE-ORDER: all checks passed\n");
    return 0;
}
