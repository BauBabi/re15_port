/*
 * RE1.5 Rebuilt — stair traversal (shared, v10 BYTE-TRUE handler 2026-06-07).
 * See re15_stair.h.
 *
 * ====================================================================
 * BYTE-EXACT port of the descend handler LAB_80038c60 (player FSM sub-state 0xc;
 * ascend = LAB_80038850, sub-state 0xb). Fully decoded from ghidra1_V2.txt:
 *
 *   Phase 0 (init): clip DAT_800acae8 = 0x15 (PL00 clip 21, the stair-step anim);
 *     forward speed DAT_800acae0 = 0xa (10); band DAT_800acad6 = 7; cursor
 *     DAT_800acae9 = 0; saved-Y DAT_800acaf0 = current Y. (step-count DAT_800acaf2
 *     is set at ENTRY = number of 0x708 levels to drop.)
 *   Phase 1 (each tick): FUN_8001f314 advances the clip cursor by EXACTLY +1
 *     (FUN_8001f8b4: cursor++, wrap at frame_count) → FUN_800390e0(0, cursor∈
 *     [11,26)) FK foot-lock → Y += 0x96 at cursor 11 & 27 → FUN_800245d8(0)
 *     forward = speed(10) along facing while cursor < 31 → at cursor == 0x20 (32):
 *     Y = savedY + 0x708, savedY = Y, step-count--, cursor → 2 (loop); when
 *     step-count hits 0 → phase 2 (finalise, commit band, return to normal state).
 *
 * So per band the body travels forward 10/tick over ~32 ticks (~310) + the small
 * FK foot-lock — a STEEP, short descent (the bulk of the motion is the +0x708 Y
 * drop). The number of bands = |targetY - startY| / 0x708.
 *
 * GENERIC (works for ANY stair, no per-room data): step-count and target band are
 * derived from the type-12/13 AOT PAIR (each staircase = two zones sharing
 * event_id, one per platform); the descent direction is the player's facing,
 * snapped once toward the paired (lower/upper) zone so he heads down the run.
 * No hardcoded savestate vector.
 * ==================================================================== */
#include "re15_stair.h"
#include "re15_aot.h"          /* g_aot.slots — the stair zones */
#include "re15_actor.h"        /* the player actor + re15_atan2_q12 */
#include "re15_skeleton.h"     /* re15_sin_q12 / re15_cos_q12 + re15_skel_compute_pose */
#include "re15_anim_select.h"  /* re15_compute_actor_kf — the foot-lock keyframe */
#include "re15_collision.h"    /* band get/set + range */
#include "re15_player.h"       /* RE15_PLAYER_MOTION_* */
#include "re15_scd.h"          /* g_scd.player_mode */

#define STAIR_REACH      450   /* body-radius reach (player held just outside the rect) */
#define STAIR_FWDPROBE   800   /* FUN_8002d474: probe 800u ahead of facing for the stair zone */
#define RE15_ANIM_REVERSE 0x80

/* BYTE-TRUE descend handler LAB_80038c60. ANIMATION = PL00 clip 21 (down)/20 (up)
 * — USER-CONFIRMED. The body XZ travel EMERGES from the FK foot-lock FUN_800390e0
 * (body dragged by the planted support FOOT, bones 4(L)/7(R) — byte-confirmed from
 * the FUN_800390e0 bone chain + PL00.emr floor-Y) + the forward-10 (FUN_800245d8).
 * No landing vector, no lerp — generalises to any stair. */
#define STAIR_CLIPLEN     42   /* PL00 clip 21/20 frame count */
#define STAIR_FOOT_L       4   /* footsel 0 = LEFT foot bone 4  */
#define STAIR_FOOT_R       7   /* footsel 1 = RIGHT foot bone 7 */
#define STAIR_FWD_SPEED   10   /* DAT_800acae0 forward units/tick (FUN_800245d8) */
#define STAIR_YBOB     0x96    /* Y += 0x96 tread bob at cursor 11 & 27 */
#define STAIR_YBAND   0x708    /* one floor level = 1800 (per-band Y step) */
#define STAIR_MAXFRAMES 600    /* safety */

static int     s_active = 0;
static int     s_finalize = 0;      /* DAT_800aca5b==2: the SEPARATE settle tick */
static int     s_frames = 0;
static int     s_target_band = 0;
static int     s_motion = RE15_PLAYER_MOTION_STAIR_DOWN;  /* 220 down (PL00 c21) / 221 up (c20) */
static int32_t s_target_y = 0;
static int32_t s_saved_y = 0;       /* DAT_800acaf0: Y at the band's start */
static int     s_ydir = 1;          /* +1 descend (Y -> 0), -1 ascend */
static int     s_bands_left = 0;    /* DAT_800acaf2: 0x708 levels still to drop */
static int     s_cursor = 0;        /* DAT_800acae9: in-clip frame */
static int32_t s_footref[3] = {0,0,0};  /* prev-tick support-foot WORLD pos (iVar3+0x54) */
static int     s_footref_ok = 0;
static int     s_prev_sel = -1;

int  re15_stair_active(void) { return s_active; }
void re15_stair_reset(void)  { s_active = 0; s_finalize = 0; }

static int s_abs(int32_t v) { return v < 0 ? (int)-v : (int)v; }

static int player_in_zone(const re15_actor_t *p, const re15_aot_t *a)
{
    return s_abs(p->x - a->x) <= a->half_w + STAIR_REACH &&
           s_abs(p->z - a->z) <= a->half_h + STAIR_REACH;
}

static int point_in_zone(int32_t x, int32_t z, const re15_aot_t *a)
{
    return s_abs(x - a->x) <= a->half_w && s_abs(z - a->z) <= a->half_h;
}

void re15_stair_tick(const re15_rdt_t *rdt,
                     const re15_emd_skeleton_t  *skel,
                     const re15_emd_animation_t *anim)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];

    /* FINALIZE tick (byte-true LAB_80038e50, DAT_800aca5b==2). The original runs
     * this as a SEPARATE tick AFTER the last band committed: it switches to the
     * STANDBY clip 2 (DAT_800acae8=2), restores player control (DAT_800aca59=0),
     * recomputes the collision band from the COMMITTED Y (DAT_800acad6 =
     * -DAT_800acc0e/0x708), and HANDS BACK.
     *
     * The byte-true descent (PL00 clip 21/20 + the FK foot-lock FUN_800390e0 + the
     * forward-10) walks the player diagonally down/up the run and lands him at the
     * FOOT of the staircase — which, by the original's own geometry, sits a few hundred
     * units INSIDE the destination band's perimeter WALL cell (e.g. descend 4->2 lands
     * X=-23733, inside band-2 wall SCA[22] x[-24063..-22765]). The ORIGINAL relies on
     * its floor resolver FUN_8003b0a4 to push the (now-stationary) player back OUT of
     * that wall cell onto the walkable platform on the NEXT gameplay frame.
     *
     * RELIABILITY (the user's "sometimes lands mid-stair / ascent stuck before the
     * wall"): leaving the un-stick to the *next* gameplay frame is FRAGILE — that frame
     * runs re15_player_tick FIRST, so if the player happens to hold a direction the
     * resolver sees old!=new and push_rect's code==0 face-case SNAPS HIM BACK to the
     * wall (prev) instead of ejecting (the un-stick only fires when old==new). So we run
     * the resolver HERE, on this stationary finalize tick (old==new = the committed
     * landing), exactly the configuration in which FUN_8003b0a4 deterministically
     * min-axis ejects the player fully onto the band's WALKABLE complement (verified PC:
     * 4->2 -23733->-24531, 2->0 z-20845->-19969, 0->2 z-24013->-24553 — all land
     * outside every solid cell, forward immediately walks). This is the SAME resolver
     * the original uses for the un-stick; we just guarantee it sees the stationary
     * landing (not a pad-perturbed one) so the hand-back is reliable for BOTH descend
     * and ascend, independent of what the player is pressing. (0x4000 latch released as
     * s_active clears, so the resolver is fully live again from the next frame.) */
    if (s_finalize) {
        p->motion     = (int16_t)RE15_PLAYER_MOTION_IDLE; /* standby clip 2 */
        p->anim_frame = 0;
        p->anim_flags = 0;
        p->y = s_target_y;                                /* committed bottom Y */
        re15_collision_set_band(re15_collision_band_from_y(p->y));
        /* FORWARD eject (byte-true OUTCOME of LAB_80038e50). The original's finalize
         * does NOT reposition the player at all — it hands back control at the descent
         * ENDPOINT, which the original's full forward+FK travel leaves a few hundred
         * units PAST the destination wall-cell's midpoint, so the next gameplay frame's
         * stationary nearest-edge push (FUN_8003b0a4) ejects him FORWARD onto the
         * platform. Our forward-10 + FK foot-lock APPROXIMATION can undershoot to the
         * cell MIDPOINT, where nearest-edge flips BACKWARD = "Leon hängt auf halber
         * Treppe in der Wand" (autopilot-proven: land x=-23309 → push +1012 back up
         * into the wall, vs the healthy x=-23422 → push -1109 forward). A plain
         * nearest-edge constrain is therefore a coin-flip near the midpoint. Instead we
         * eject DETERMINISTICALLY FORWARD: step along the descent facing (rot_y, already
         * aimed down the run toward the paired zone) to the FIRST walkable cell = the
         * foot of the staircase, exactly the original's landing — never backward. */
        if (rdt) {
            /* forward unit (player-move convention: x+=cos*v, z+=-sin*v). */
            int32_t cs = re15_cos_q12((int)p->rot_y);
            int32_t sn = re15_sin_q12((int)p->rot_y);
            int32_t lx = p->x, lz = p->z;     /* committed landing (never go behind it) */
            int32_t tx = lx, tz = lz;         /* marching probe */
            int     done = 0;
            for (int step = 0; step < 48; step++) {
                int32_t cx = tx, cz = tz;
                re15_collision_constrain(rdt, tx, tz, &cx, &cz);
                /* sign of the eject ALONG the descent facing: >0 forward (down the
                 * run onto the platform), <0 backward (up into the stair wall). */
                int64_t fwd = (int64_t)(cx - tx) * cs - (int64_t)(cz - tz) * sn;
                if (fwd >= 0) {                /* FORWARD edge, or already walkable (0) */
                    p->x = cx; p->z = cz; done = 1; break;
                }
                /* nearest edge is BACKWARD — probe is still in the cell's rear half;
                 * nudge it +100u forward past the midpoint and re-test (→ the FORWARD
                 * edge = the foot of the stairs). NEVER commit a backward push. */
                tx += (int32_t)(( (int64_t)cs * 100) >> 12);
                tz += (int32_t)((-(int64_t)sn * 100) >> 12);
            }
            /* Bulletproof fallback (geometrically unreachable on ROOM1170's stairs — the
             * forward edge is always found within ~12 steps): if the whole 4800u forward
             * span somehow never yielded a forward resolution, leave the player at the
             * committed landing rather than snapping him BACKWARD into the stair wall.
             * Next-frame collision then resolves him exactly as the original would — i.e.
             * worst case == the original, best/normal case == clean forward landing. */
            if (!done) { p->x = lx; p->z = lz; }
        }
        s_finalize = 0;
        s_active   = 0;
        return;
    }

    /* PL00 clip 21 (down) / 20 (up) stepping gait. */
    if (p->motion != (int16_t)s_motion) p->motion = (int16_t)s_motion;
    p->anim_flags       &= (uint16_t)~RE15_ANIM_REVERSE;
    p->motion_init_delay = 0;

    int     up   = (s_motion == RE15_PLAYER_MOTION_STAIR_UP);
    /* (1) FUN_8001f314: advance the clip cursor +1/tick (forward playback). */
    s_cursor++;
    int frame = s_cursor % STAIR_CLIPLEN;
    p->anim_frame = (uint16_t)frame;

    int     clip = (s_motion == RE15_PLAYER_MOTION_STAIR_DOWN) ? 21 : 20;
    int32_t cs   = re15_cos_q12((int)p->rot_y);
    int32_t sn   = re15_sin_q12((int)p->rot_y);

    /* ASCEND (LAB_80038850) shifts every cursor +1 vs the descend (LAB_80038c60):
     * footsel window [12,27), tread bob at 12&28, band step at cursor 33, loop to 3. */
    int foot_lo = up ? 12 : 11, foot_hi = up ? 27 : 26;
    int bob_a   = up ? 12 : 11, bob_b   = up ? 28 : 27;
    int band_cur= up ? 33 : 32, loop_to = up ? 3  : 2;

    /* (2) FK foot-lock FUN_800390e0(0, footsel): byte-true (137057..137125) — the body
     * (player 0x34/0x38/0x3c = X/Y/Z) is DRAGGED by the planted SUPPORT foot:
     *     p.X -= (footWorld.X - footPrev.X); same for Y,Z
     * where footWorld = RotMatrix(player+0x68) chained down the bone tree to the support
     * foot (local_24/20/1c), and footPrev = *(s3+0x54/58/5c) = the support foot's world
     * pos cached from the PREVIOUS render tick. So this is the foot's PER-TICK world
     * delta; over the gait it carries the body forward as the planted foot pushes back.
     * THIS XZ travel is REQUIRED — it walks the player diagonally down/up the run; the
     * forward-10 (step 4) is the minor component. Support foot = bone 7 inside the window,
     * else bone 4 (clip 20 ascend = opposite leg phase → foot = (sel^up)). */
    if (skel && anim && skel->bone_count > STAIR_FOOT_R) {
        int sel  = (s_cursor >= foot_lo && s_cursor < foot_hi) ? 1 : 0;
        int foot = (sel ^ up) ? STAIR_FOOT_R : STAIR_FOOT_L;
        int kf   = re15_compute_actor_kf(anim, skel, p, clip, (uint32_t)frame);
        /* Pose QUERY (foot world pos) — NOT a render. Disable the crossfade blend so
         * this probe doesn't consume/mutate the player's prev-pose state. */
        g_anim_pose_actor = NULL;
        re15_skel_compute_pose(skel, kf, poses);
        int32_t lx = poses[foot].trans[0], ly = poses[foot].trans[1], lz = poses[foot].trans[2];
        /* rotate the root-local foot pos into world by player facing (RotMatrix +0x68). */
        int32_t wx = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
        int32_t wz = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
        int32_t wy = ly;
        if (s_footref_ok && sel == s_prev_sel) {
            /* body -= support-foot world delta (foot planted, NO slide) for BOTH. */
            p->x -= (wx - s_footref[0]);
            p->z -= (wz - s_footref[2]);
            p->y -= (wy - s_footref[1]);
        }
        s_footref[0] = wx; s_footref[1] = wy; s_footref[2] = wz;
        s_footref_ok = 1; s_prev_sel = sel;
    }

    /* (3) tread bob: Y += 0x96 toward the target (s_ydir = -1 up -> -0x96) (transient). */
    if (s_cursor == bob_a || s_cursor == bob_b) p->y += s_ydir * STAIR_YBOB;

    /* (4) FUN_800245d8 forward = speed 10 along facing while cursor < 31. */
    if (s_cursor < 31) {
        p->x += (int32_t)(( (int64_t)cs * STAIR_FWD_SPEED) >> 12);
        p->z += (int32_t)((-(int64_t)sn * STAIR_FWD_SPEED) >> 12);
    }

    s_frames++;
    /* (5) band-step cursor (32 down / 33 up): commit one band, loop or finalise. */
    if (s_cursor >= band_cur || s_frames >= STAIR_MAXFRAMES) {
        s_saved_y += s_ydir * STAIR_YBAND;   /* up: -0x708, down: +0x708 */
        p->y = s_saved_y;
        s_footref_ok = 0;            /* re-seed across the band loop */
        if (s_bands_left <= 0 || s_frames >= STAIR_MAXFRAMES) {
            /* LAST band committed (DAT_800acaf2 hit 0). Byte-true LAB_80038e40:
             * commit the final Y here, then ARM the separate finalize/settle tick
             * (DAT_800aca5b=2) — do NOT hand back to gameplay this same tick. The
             * settle tick (top of re15_stair_tick) recomputes the band + restores
             * control with the player stationary, so collision un-sticks cleanly
             * instead of wedging Leon at the last step until forward is held. */
            p->y = s_target_y;
            s_finalize = 1;
            return;
        }
        s_bands_left--;
        s_cursor = loop_to;
    }
}

int re15_stair_try_start(const re15_rdt_t *rdt, int action_pressed)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    if (s_active) return 1;
    if (g_scd.player_mode == 2) return 0;

    if (!action_pressed) return 0;

    int cur = re15_collision_debug_band();
    if (cur < 0) cur = re15_collision_band_from_y(p->y);

    /* Entry = the byte-true FUN_8002d474 test: a point 800u AHEAD of the player's
     * facing must lie in a stair zone (the player is BLOCKED by the wall outside
     * the rect, so "player in rect" never fires — that was the "nichts passiert"
     * bug). Also accept the player's own body in the zone (reach). Prefer a zone
     * whose stored band == cur (the right end when two zones overlap mid-platform). */
    int32_t sn = re15_sin_q12((int)p->rot_y), cs = re15_cos_q12((int)p->rot_y);
    int32_t fx = p->x + (int32_t)(( cs * STAIR_FWDPROBE) / 4096);
    int32_t fz = p->z - (int32_t)(( sn * STAIR_FWDPROBE) / 4096);
    const re15_aot_t *zone = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        /* BAND GATE (byte-true FUN_8002da4c: a stair zone fires ONLY when
         * `zone[0x82] == player_band`). A staircase zone for platform band N is
         * triggerable ONLY while the player is actually STANDING on band N — so
         * a STACKED stair you are not yet on is ignored. The old code fell back
         * to the first XZ-overlapping zone regardless of band, which made the
         * TOP stair (band 4) wrongly start when the player pressed action at the
         * DOOR below it (player on a lower band): the door sits inside the top
         * stair's XZ rect, but the player is NOT on band 4. With the gate there
         * is no stair match at the door's band → the AOT door scan takes the
         * action. You must first ascend the lower stair onto band N before the
         * stair from band N is reachable. (2026-06-08) */
        if (a->active && a->type == RE15_AOT_TYPE_STAIR &&
            (int)a->band == cur &&
            (point_in_zone(fx, fz, a) || player_in_zone(p, a))) {
            zone = a; break;
        }
    }
    if (!zone) return 0;

    /* Destination band = the OTHER end of THIS staircase (the type-12/13 PAIR).
     * Each staircase is two AOT zones sharing event_id (the type/end flag), one
     * on each platform (chain bands {0,2} for type-13, {2,4} for type-12). The
     * player goes to the paired band he is NOT on. This is robust against the SCA
     * having a non-platform band 3 (the big floor cells 0,1 = floor 0x33) which
     * broke the old next_band_below() target (it returned 3, not 2 -> no vector
     * -> "nichts passiert"). */
    const re15_aot_t *paired = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_STAIR &&
            a->event_id == zone->event_id && a != zone) {
            paired = a; break;
        }
    }
    if (!paired) return 0;
    int target = (cur == (int)zone->band) ? (int)paired->band : (int)zone->band;
    if (target == cur) return 0;
    const re15_aot_t *dest_zone = (target == (int)paired->band) ? paired : zone;

    /* Byte-true set-up (LAB_80038c60 phase 0). No landing vector: the body XZ EMERGES
     * from forward-10 each tick over |Δband| band cycles; Y from the +0x708 per cycle.
     *
     * DETERMINISM (symptom 1 fix): the step count must come from the INTEGER band
     * difference, NOT a Y subtraction. The original derives the count from the band
     * numbers (cur/target = integer floors); deriving it from |targetY - p->y|/0x708
     * made the count depend on the player's EXACT start Y, so a slightly-off start Y
     * (e.g. a prior tick left him a few units off a clean band multiple, or the FK
     * foot-lock nudged Y) rounded to a WRONG number of band-cycles → the descent
     * finalised one band early/late = "ended in the middle of the staircase". We now
     * SNAP s_saved_y to the exact source-band multiple and count integer bands, so the
     * cycle count and the committed landing Y are identical regardless of the entry Y. */
    s_target_band = target;
    s_target_y    = -(int32_t)(target * STAIR_YBAND);
    s_saved_y     = -(int32_t)(cur * STAIR_YBAND);        /* snap to exact source-band Y */
    s_ydir        = (target < cur) ? +1 : -1;             /* descend: Y -> 0 (toward pit) */
    s_bands_left  = s_abs(target - cur);                  /* integer band difference */
    if (s_bands_left > 0) s_bands_left -= 1;               /* DAT_800acaf2 = count-1 (N cycles) */

    /* Face the paired (target) zone so the stepping run points toward it. */
    int32_t rdx = dest_zone->x - p->x, rdz = dest_zone->z - p->z;
    if (rdx || rdz)
        p->rot_y = (int16_t)((re15_atan2_q12(rdz, rdx) - 1024) & 0x0FFF);
    s_motion  = (target < cur) ? RE15_PLAYER_MOTION_STAIR_DOWN   /* 220 -> PL00 clip 21 */
                               : RE15_PLAYER_MOTION_STAIR_UP;     /* 221 -> PL00 clip 20 */
    p->motion = (int16_t)s_motion;
    p->anim_flags = 0;

    s_active      = 1;
    s_finalize    = 0;
    s_frames      = 0;
    s_cursor      = 0;
    s_footref_ok  = 0;
    s_prev_sel    = -1;
    p->anim_frame = 0;
    return 1;
}
