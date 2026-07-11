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
#include "re15_skeleton.h"   /* g_anim_kf_tween + re15_skel_compute_pose (the tween consumer) */

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

    /* (B) the 0x8000 marker is a TWEEN, never posed as a keyframe (FUN_8001f8b4): the kf query
     *     returns the bracketing-AFTER real frame and arms g_anim_kf_tween with the bracketing-
     *     BEFORE frame + the marker's weight; the HOLD-LAST backward scan is unchanged; the pose
     *     is direction-independent (reverse arms the same tween). */
    {
        re15_emd_animation_t anim; memset(&anim, 0, sizeof anim);
        re15_emd_skeleton_t  skel; memset(&skel, 0, sizeof skel);
        skel.keyframe_count = 16;
        anim.clip_count = 1; anim.frame_count = 4;
        anim.clips[0].first_frame = 0; anim.clips[0].frame_count = 4;
        anim.frames[0] = 5; anim.frames[1] = 6; anim.frames[2] = 7;
        anim.frames[3] = 0x8000u | 0x0027u;                       /* marker, w=0x27 (the EM013 value) */

        re15_actor_t a; memset(&a, 0, sizeof a);

        int kf = re15_compute_actor_kf(&anim, &skel, &a, /*loop*/0, /*cur=*/3);   /* LOOP seam on the marker */
        if (kf != 5) { fprintf(stderr, "FAIL(B1a): LOOP marker must return the AFTER frame (kf 5), got %d\n", kf); fail = 1; }
        if (!g_anim_kf_tween.active || g_anim_kf_tween.kf_from != 7 || g_anim_kf_tween.weight != 0x27) {
            fprintf(stderr, "FAIL(B1b): tween must be {active, from kf 7, w 0x27}, got {%d,%d,%#x}\n",
                    g_anim_kf_tween.active, g_anim_kf_tween.kf_from, (unsigned)g_anim_kf_tween.weight);
            fail = 1;
        }

        a.motion = 0;
        kf = re15_compute_actor_kf(&anim, &skel, &a, /*no override*/-1, 3);       /* plain-forward on the marker */
        if (kf != 5 || !g_anim_kf_tween.active) { fprintf(stderr, "FAIL(B2): forward marker must tween to kf 5, got %d act=%d\n", kf, g_anim_kf_tween.active); fail = 1; }

        kf = re15_compute_actor_kf(&anim, &skel, &a, -1, 10);                     /* HOLD-LAST (cur > fc) */
        if (kf != 7 || g_anim_kf_tween.active) { fprintf(stderr, "FAIL(B3): hold-last must keep kf 7 with NO tween, got %d act=%d\n", kf, g_anim_kf_tween.active); fail = 1; }

        a.anim_flags = 0x80;                                                      /* reverse playback */
        kf = re15_compute_actor_kf(&anim, &skel, &a, 0, 0);                       /* rev slot = fc-1 = marker */
        if (kf != 5 || !g_anim_kf_tween.active || g_anim_kf_tween.kf_from != 7) {
            fprintf(stderr, "FAIL(B4): reverse marker must arm the SAME absolute tween (kf 5, from 7), got %d from %d\n", kf, g_anim_kf_tween.kf_from); fail = 1; }

        a.anim_flags = 0;
        kf = re15_compute_actor_kf(&anim, &skel, &a, 0, 1);                       /* non-marker sanity */
        if (kf != 6 || g_anim_kf_tween.active) { fprintf(stderr, "FAIL(B5): non-marker must be kf 6 with NO tween, got %d act=%d\n", kf, g_anim_kf_tween.active); fail = 1; }

        if (!fail) printf("  (B) 0x8000 marker arms the tween (after-kf returned, before-kf + w in the channel)\n");
    }

    /* (C) compute_pose consumes the tween: root translation = ((0x1000-w)*A + w*B) >> 12, and the
     *     side channel is CLEARED after the pose (one-shot). Synthetic 1-bone skel, 2 keyframes with
     *     known root positions, w = 0x800 -> exact midpoint (the EM02B shipped value). */
    {
        static uint8_t kfdata[40];                                /* 2 keyframes x 20 bytes */
        memset(kfdata, 0, sizeof kfdata);
        /* kf0 root = (100, 200, 300); kf1 root = (300, 400, 500) (LE s16 @+0/+2/+4; angles zeroed) */
        kfdata[0] = 100; kfdata[2] = 200; kfdata[4] = 44; kfdata[5] = 1;          /* 300 = 0x12C */
        kfdata[20] = 44; kfdata[21] = 1; kfdata[22] = 0x90; kfdata[23] = 1;       /* 300, 400 = 0x190 */
        kfdata[24] = 0xF4; kfdata[25] = 1;                                        /* 500 = 0x1F4 */
        re15_emd_skeleton_t skel; memset(&skel, 0, sizeof skel);
        skel.bone_count = 1; skel.bone_parent[0] = -1;
        skel.keyframe_size_bytes = 20; skel.keyframe_count = 2;
        skel.keyframe_data = kfdata; skel.keyframe_data_size = sizeof kfdata;

        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;                                  /* no crossfade */
        g_anim_kf_tween.active = 1; g_anim_kf_tween.kf_from = 0; g_anim_kf_tween.weight = 0x800;
        re15_skel_compute_pose(&skel, /*kf=*/1, poses);
        if (poses[0].trans[0] != 200 || poses[0].trans[1] != 300 || poses[0].trans[2] != 400) {
            fprintf(stderr, "FAIL(C1): tween w=0x800 root must be the midpoint (200,300,400), got (%d,%d,%d)\n",
                    poses[0].trans[0], poses[0].trans[1], poses[0].trans[2]);
            fail = 1;
        }
        if (g_anim_kf_tween.active) { fprintf(stderr, "FAIL(C2): compute_pose must CLEAR the tween channel\n"); fail = 1; }
        re15_skel_compute_pose(&skel, 1, poses);                   /* no tween -> plain kf1 root */
        if (poses[0].trans[0] != 300 || poses[0].trans[1] != 400 || poses[0].trans[2] != 500) {
            fprintf(stderr, "FAIL(C3): without the tween the plain kf1 root must pose (300,400,500), got (%d,%d,%d)\n",
                    poses[0].trans[0], poses[0].trans[1], poses[0].trans[2]);
            fail = 1;
        }
        if (!fail) printf("  (C) compute_pose lerps the root at w=0x800 to the midpoint + clears the channel\n");
    }

    if (fail) { printf("ANIM-POSE-ORDER: FAIL\n"); return 1; }
    printf("ANIM-POSE-ORDER: all checks passed\n");
    return 0;
}
