/*
 * RE1.5 Rebuilt — Skeleton pose math (Phase 4.5.7.2, 2026-05-18).
 *
 * Pure C, target-agnostic. Builds per-bone rotation matrices from
 * 12-bit Euler angles read out of the EMR keyframe block, then
 * accumulates parent transforms to produce world-space poses.
 *
 * Math is Q12 fixed-point throughout — matches the PSX GTE's native
 * 4.12 format so on PSX the matrices can be passed straight to
 * gte_SetRotMatrix after a quick int32 → int16 cast in Phase 4.5.7.3.
 *
 * Rotation order: Z then Y then X (matches PSn00bSDK RotMatrix). The
 * composite matrix is M = Rx * Ry * Rz, applied to a column vector as
 * v' = M * v.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>    /* RE15_NECK_TRACE diagnostics */
#include <stdlib.h>   /* getenv */
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_actor.h"     /* g_actors for Plc_neck head-look */
#include "re15_math.h"      /* re15_squareroot0 — the BIOS sqrt approx for the neck-pitch horiz dist */

/* Q12 helpers — int32 intermediates to avoid overflow during multiply.
 *
 * M-round (2026-05-25): Q12_MUL truncates per-element, so a 3-term dot
 * product like sum(Q12_MUL(a,b)) loses up to 3 LSB. The PSX GTE keeps
 * full int64 precision across the MAC accumulator and shifts ONCE at the
 * end (one truncation, ≤0.5 LSB error). Fixed by inlining the loops below
 * with int64 accumulators — Q12_MUL is kept for single-product callers
 * only. This is the root cause of the user-reported "wiederkehrende
 * Pixel Verschiebungen" — earlier T-REZ2 fix only rounded the view-matrix
 * float→Q12 conversion; the much bigger source was the bone-chain
 * Q12_MUL accumulation in mat3_mul / mat3_apply / mat3_from_euler. */
#define Q12_MUL(a, b)   (int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12)

/* ----- Matrix builders ------------------------------------------------- */

/* Set 3x3 matrix to identity (diagonal = ONE, off-diagonal = 0). */
static void mat3_identity(int32_t m[9])
{
    m[0] = RE15_SKEL_ONE; m[1] = 0;             m[2] = 0;
    m[3] = 0;             m[4] = RE15_SKEL_ONE; m[5] = 0;
    m[6] = 0;             m[7] = 0;             m[8] = RE15_SKEL_ONE;
}

/* C = A * B (all 3x3 Q12). C may not alias A or B.
 * GTE-faithful: accumulate full precision, shift once. */
static void mat3_mul(const int32_t a[9], const int32_t b[9], int32_t c[9])
{
    /* PERF (audit P0.1): int32 accumulator (R3000 emulates int64 mul in libgcc
     * ~10x slower). Q12 (±4096) × Q12 = ±16.7M, sum of 3 = ±50M — within int32.
     * Full accumulation + single >>12 preserved → bit-identical, no jitter. */
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int32_t s = 0;
            for (int k = 0; k < 3; k++) {
                s += a[row*3 + k] * b[k*3 + col];
            }
            c[row*3 + col] = s >> 12;
        }
    }
}

/* v' = M * v. v is 3-vector of integer world coords (not Q12), m is Q12.
 * GTE-faithful: int64 accumulator, single >>12 at the end (one truncation,
 * not three). Old form did (Q12_MUL(m,v<<12)+...) which round-tripped v
 * through Q12 and incurred 3 separate truncations per output coord. */
static void mat3_apply(const int32_t m[9], const int32_t v[3], int32_t out[3])
{
    /* PERF (audit P0.1): int32. m Q12 (±4096) × v bone-local (±few-thousand) =
     * tens of M, sum of 3 well within int32. Bit-identical to the int64 path. */
    int32_t sx = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
    int32_t sy = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
    int32_t sz = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
    out[0] = sx >> 12;
    out[1] = sy >> 12;
    out[2] = sz >> 12;
}

/* Build per-axis rotation matrices. Inputs are angle in PSX trig units. */

static void mat3_rot_x(int angle_4096, int32_t m[9])
{
    int32_t c = re15_cos_q12(angle_4096);
    int32_t s = re15_sin_q12(angle_4096);
    /* Rx = | 1   0    0 |
     *      | 0   c   -s |
     *      | 0   s    c |   */
    m[0] = RE15_SKEL_ONE; m[1] =  0;            m[2] =  0;
    m[3] = 0;             m[4] =  c;            m[5] = -s;
    m[6] = 0;             m[7] =  s;            m[8] =  c;
}

static void mat3_rot_y(int angle_4096, int32_t m[9])
{
    int32_t c = re15_cos_q12(angle_4096);
    int32_t s = re15_sin_q12(angle_4096);
    /* Ry = |  c   0   s |
     *      |  0   1   0 |
     *      | -s   0   c |   */
    m[0] =  c;            m[1] =  0;            m[2] =  s;
    m[3] =  0;            m[4] =  RE15_SKEL_ONE;m[5] =  0;
    m[6] = -s;            m[7] =  0;            m[8] =  c;
}

static void mat3_rot_z(int angle_4096, int32_t m[9])
{
    int32_t c = re15_cos_q12(angle_4096);
    int32_t s = re15_sin_q12(angle_4096);
    /* Rz = | c  -s   0 |
     *      | s   c   0 |
     *      | 0   0   1 |   */
    m[0] =  c;            m[1] = -s;            m[2] = 0;
    m[3] =  s;            m[4] =  c;            m[5] = 0;
    m[6] =  0;            m[7] =  0;            m[8] = RE15_SKEL_ONE;
}

/* Composite Euler rotation, BYTE-TRUE to RE1.5's own RotMatrix (RE_15_Quellcode_V2/RotMatrix.c,
 * disasm @0x80068130; audit #1). Same convention/terms/signs as before (they already matched
 * RE2==RE1.5 — a decode scare about flipped triple-term signs was resolved: the original folds the
 * -sin(vy) negation INTO the intermediate `iVar4 = cz*(-sy)>>12`, so m[1][0] = sz*cx + cz*sy*sx as
 * here). The ONLY change is the TRUNCATION, which is what the pixel-shift needed:
 *   (a) PER-PRODUCT >>12 with a short cast on each term (the old form accumulated the whole
 *       off-diagonal in Q36 and shifted once = MORE precise = not byte-true);
 *   (b) NEGATE-BEFORE-SHIFT on m[1]/m[5] (`(-(sz*cy))>>12`, not `-((sz*cy)>>12)` — differs by 1 LSB
 *       whenever the product is not a multiple of 4096, @0x80068170).
 * PSX int32 arithmetic: every product fits in 32 bits, matching the original mflo (low word). */
static void mat3_from_euler(int ax, int ay, int az, int32_t m[9])
{
    int32_t sx = re15_sin_q12(ax), cx = re15_cos_q12(ax);
    int32_t sy = re15_sin_q12(ay), cy = re15_cos_q12(ay);
    int32_t sz = re15_sin_q12(az), cz = re15_cos_q12(az);
    int32_t nsy = -sy;                                            /* iVar9 = -(int)sin(vy) @line 22 */
    m[2] = (int16_t)sy;                                           /* m[0][2] @line 25 */
    m[5] = (int16_t)((-(cy * sx)) >> 12);                         /* m[1][2] negate-before-shift @line 26 */
    m[8] = (int16_t)((cy * cx) >> 12);                            /* m[2][2] @line 27 */
    m[0] = (int16_t)((cz * cy) >> 12);                            /* m[0][0] @line 32 */
    m[1] = (int16_t)((-(sz * cy)) >> 12);                         /* m[0][1] negate-before-shift @line 33 */
    {
        int32_t t1 = (cz * nsy) >> 12;                            /* iVar4 = cz*(-sy)>>12 @line 34 */
        m[3] = (int16_t)((int16_t)((sz * cx) >> 12) - (int16_t)((t1 * sx) >> 12));   /* m[1][0] @line 35 */
        m[6] = (int16_t)((int16_t)((sz * sx) >> 12) + (int16_t)((t1 * cx) >> 12));   /* m[2][0] @line 36 */
    }
    {
        int32_t t2 = (sz * nsy) >> 12;                            /* iVar9' = sz*(-sy)>>12 @line 37 */
        m[4] = (int16_t)((int16_t)((cz * cx) >> 12) + (int16_t)((t2 * sx) >> 12));   /* m[1][1] @line 38 */
        m[7] = (int16_t)((int16_t)((cz * sx) >> 12) - (int16_t)((t2 * cx) >> 12));   /* m[2][1] @line 39 */
    }
}

/* ----- Public API ------------------------------------------------------ */

/* BYTE-TRUE current-pose actor (= the original's DAT_800ac784). The renderer sets
 * this to the actor being posed so the FRAC crossfade below can read its blend state;
 * NULL = no blend (pose QUERY, e.g. the stair foot-position probe). */
void *g_anim_pose_actor = NULL;

/* 0x8000 marker TWEEN side channel (FUN_8001f8b4) — set by re15_compute_actor_kf, consumed +
 * cleared by the next re15_skel_compute_pose. See re15_skeleton.h. */
re15_kf_tween_t g_anim_kf_tween = {0, 0, 0};

/* Byte-true shortest-arc 12-bit angle lerp (FUN_80020510 + LoadAverageShort12):
 * mask prev to 12 bits, unwrap cur to within ±0x800 of prev, then
 * out = (prev*wp + cur*(0x1000-wp)) >> 12  (wp = prev weight = 0x200*frac). */
static int16_t re15_blend_angle12(int prev, int cur, int wp)
{
    int wc = 0x1000 - wp;
    prev &= 0xfff;
    unsigned d = (unsigned)(uint16_t)((cur - prev) + 0x800);
    if (d > 0x1000) cur += (d & 0x8000) ? 0x1000 : -0x1000;
    return (int16_t)((prev * wp + cur * wc) >> 12);
}

int re15_skel_compute_pose(const re15_emd_skeleton_t *skel,
                            int                       keyframe_index,
                            re15_skel_pose_t          poses[RE15_EMD_MAX_BONES])
{
    if (!skel || !poses) return -1;
    if (skel->bone_count <= 0 || skel->bone_count > RE15_EMD_MAX_BONES) return -2;

    /* 0x8000 MARKER TWEEN (FUN_8001f8b4): consume + clear the side channel armed by the preceding
     * re15_compute_actor_kf. When active, `keyframe_index` is the bracketing-AFTER real frame (B)
     * and tween.kf_from the bracketing-BEFORE one (A); the pose below lerps A->B at weight w for
     * the root AND every bone angle, exactly like the original's GPF12/GPL12 + FUN_80020510 pass.
     * Applied to the RAW keyframe values FIRST, then the FRAC crossfade blends on top — the same
     * two-stage order as f8b4's blend-active branch (FUN_8001ffd8 @0x8001fb38). */
    re15_kf_tween_t tween = g_anim_kf_tween;
    g_anim_kf_tween.active = 0;
    int tw = tween.active ? (tween.weight & 0xfff) : 0;   /* w = fraction toward THIS keyframe (B) */

    /* FRAC crossfade state (FUN_8001f3bc): blend from the actor's previously
     * rendered pose into this keyframe over anim_frac frames. wp/wc = prev/current
     * weights (0x200 per FRAC step, matching the 0x200 unit the PSX caller passes). */
    re15_actor_t *bact   = (re15_actor_t *)g_anim_pose_actor;
    int           blend  = (bact && bact->anim_frac > 0 && bact->anim_prev_valid);
    /* wp = prev weight = frac * 0x200 (byte-true FUN_8001f3bc: weight = entity+0x8f * param_5,
     * param_5 = 0x200 from FUN_80035538/FUN_80036718; clamp to 0x1000). The blend is recursive
     * (prev = the blended output, snapshotted below) and only smooths the first few frames. */
    int           rate   = (bact && bact->anim_blend_rate) ? (int)bact->anim_blend_rate : 0x200;
    /* rate = the f314 4th arg (byte-true): player handlers 0x200; ZOMBIE WALK states 0x100 =
     * max 0xf00/94% decaying 6%/frame, never a frozen 100% plateau (the old flat 0x200+clamp
     * froze the pose 8 frames per walk-state entry -> the upright 'unaware-looking' walk). */
    int           wp     = blend ? ((int)bact->anim_frac * rate) : 0;
    if (wp > 0x1000) wp = 0x1000;
    int           wc     = 0x1000 - wp;

    /* RE2-faithful root handling: the keyframe's px/py/pz is a per-frame
     * delta for game-driven player movement (e.g. walk forward 12 units
     * this frame). The renderer LOCKS the root bone at the caller's
     * supplied position; gameplay code is responsible for advancing
     * pos_x/y/z over time using the keyframe delta. Without this lock,
     * Leon's walk animation would visibly drift him out of frame because
     * we'd render the cumulative walk position every frame.
     *
     * Phase 4.7+ player-controller will read re15_emd_get_keyframe_position
     * and apply it to game state. */
    /* BYTE-TRUE (RE'd 2026-06-15, deep STAGE1.BIN+PSX.EXE disasm workflow): the keyframe root
     * (px,py,pz) is NEVER additively injected into the render translation for ANY actor. The
     * original stores it only in the per-actor work struct (entity+0x188) and consumes it as an
     * optional XZ locomotion DELTA into the entity world X/Z (FUN_800369f8, gated by entity+0x1c4
     * bit0); the Y root-delta (FUN_800390e0) is PLAYER-ONLY (climb/ladder anims) and has ZERO
     * enemy callers. Each bone = EMR bone-relative offset + 12-bit angles; the root bone enters
     * at the entity world XYZ (the parent matrix). So kf root is locked to 0 here for everyone —
     * adding it (the earlier re15_skel_apply_root experiment) lifted enemy poses off the floor. */
    /* BYTE-TRUE FIX (RE'd 2026-06-15, FUN_8001f3bc @ghidra1_V2.txt:98... /
     * RE_15_Quellcode_V2/FUN_8001f3bc.c:28-37): the ROOT bone's local
     * translation = the keyframe's first 3 s16 (kf bytes 0/2/4 = px,py,pz),
     * loaded DIRECTLY into the root work struct (work+0x2c/0x30/0x34) and
     * consumed by the FUN_80022da0 bone transform. It is NOT the EMR bind
     * bone[0] offset, and the bind is NOT added to it — the keyframe REPLACES
     * the root translation every frame. The eating/lying enemy clips ramp the
     * root py from the standing ~-1751 down to ~-175..-470 (pelvis on the
     * floor); locking the root to the static bind -1751 lifts the whole
     * skeleton ~1300-1576 units off the floor = the room1140 flying corpse /
     * eating zombies and room1150 floating Irons. Read the per-frame root. */
    int16_t kf_px = 0, kf_py = 0, kf_pz = 0;
    re15_emd_get_keyframe_position(skel, keyframe_index, &kf_px, &kf_py, &kf_pz);
    if (tween.active) {
        /* marker ROOT lerp (GTE GPF12/GPL12 @0x8001fa78-fae0): root = ((0x1000-w)*A + w*B) >> 12 */
        int16_t fx = 0, fy = 0, fz = 0;
        re15_emd_get_keyframe_position(skel, tween.kf_from, &fx, &fy, &fz);
        kf_px = (int16_t)(((int)fx * (0x1000 - tw) + (int)kf_px * tw) >> 12);
        kf_py = (int16_t)(((int)fy * (0x1000 - tw) + (int)kf_py * tw) >> 12);
        kf_pz = (int16_t)(((int)fz * (0x1000 - tw) + (int)kf_pz * tw) >> 12);
    }

    /* ROOM1150 KNEEL root-Y — RESOLVED 2026-06-17 (FK-measured, ablauf4-verified): there is
     * NO root-py override here, and that is CORRECT (byte-true). Earlier rounds chased a "pop"
     * by forcing the player root py to the bind offset; that was WRONG and is permanently
     * rejected. Measured ground truth (full forward-kinematics over the real banks):
     *   WALK W01 clip5    : root py ≈ -1804..-1760, FEET world-Y ≈ -166  (grounded)
     *   RBJ standing clip0: root py ≈ -1762,        FEET ≈ -175           (grounded)
     *   RBJ KNEEL clip11  : root py  =  -761 (constant 25f), FEET ≈ -127..-130 (grounded!)
     *   RBJ kneel + forced root -1804 (the rejected fix): FEET ≈ -1170 = ~1000u SUNK into floor.
     * The kneel pelvis IS authored high (-761): clip11's bent-leg chain folds the legs so the
     * FEET land at the same floor height as standing. ablauf4 F216160-216200 confirm a forward
     * spine BOW with planted feet (not a vertical drop). The 7-frame FRAC crossfade blends the
     * ROOT py (-1760→-761) AND every bone angle (straight→folded) IN LOCKSTEP (both weighted by
     * 0x200*frac), so the feet stay grounded throughout the transition (measured feet -84..-149,
     * within ~70u of standing -166, monotonic settle). Blending the root py as the raw absolute
     * keyframe value is exactly what FUN_8001f3bc does (gpf12/gpl12 on the work-struct root,
     * lines 49-57). Do NOT add any per-clip / per-actor root-py override — it desyncs the
     * pelvis from the folded legs and buries (or floats) the feet. */

    /* Root-translation crossfade (FUN_8001f3bc gpf12/gpl12): lerp the keyframe root
     * toward the prior rendered root. rt_* = the rendered (possibly blended) root. */
    int32_t rt_x = kf_px, rt_y = kf_py, rt_z = kf_pz;
    if (blend) {
        rt_x = ((int)kf_px * wc + bact->prev_root[0] * wp) >> 12;
        rt_y = ((int)kf_py * wc + bact->prev_root[1] * wp) >> 12;
        rt_z = ((int)kf_pz * wc + bact->prev_root[2] * wp) >> 12;
    }
    if (bact) { bact->prev_root[0] = rt_x; bact->prev_root[1] = rt_y; bact->prev_root[2] = rt_z; }

    /* Plc_neck head-look needs the root bone's keyframe Y euler (the original's
     * "bodyYaw" = model bone[0]+0x62) to reduce the world look angle into the head's
     * parent-local frame by scalar subtraction. Captured at b==0 below. */
    int16_t root_kf_ay = 0;

    for (int b = 0; b < skel->bone_count; b++) {
        /* Local Euler angles for this bone in this keyframe. */
        int16_t ax = 0, ay = 0, az = 0;
        re15_emd_get_keyframe_angles(skel, keyframe_index, b, &ax, &ay, &az);
        if (tween.active) {
            /* marker ANGLE lerp (hidden-$a3 w -> FUN_8001fb94 -> FUN_80020510 shortest-path):
             * angle = ((0x1000-w)*A + w*B) >> 12 == blend_angle12(A, B, wp = 0x1000-w). */
            int16_t fax = 0, fay = 0, faz = 0;
            re15_emd_get_keyframe_angles(skel, tween.kf_from, b, &fax, &fay, &faz);
            ax = re15_blend_angle12(fax, ax, 0x1000 - tw);
            ay = re15_blend_angle12(fay, ay, 0x1000 - tw);
            az = re15_blend_angle12(faz, az, 0x1000 - tw);
        }

        /* Per-bone angle crossfade (FUN_80020510): blend the keyframe angle toward
         * the prior rendered angle, then snapshot the result as prev for next frame.
         * Done on the RAW keyframe angle (before the Plc_neck head-look add) so prev
         * stays in keyframe space. */
        if (blend) {
            ax = re15_blend_angle12(bact->prev_angles[b][0], ax, wp);
            ay = re15_blend_angle12(bact->prev_angles[b][1], ay, wp);
            az = re15_blend_angle12(bact->prev_angles[b][2], az, wp);
        }
        if (bact) { bact->prev_angles[b][0] = ax; bact->prev_angles[b][1] = ay; bact->prev_angles[b][2] = az; }

        if (b == 0) root_kf_ay = ay;   /* "bodyYaw" for the head-look frame reduction */

        /* Plc_neck/HEAD-LOOK FSM — BYTE-TRUE port of FUN_80037358 + FUN_8003790c, GLOBAL
         * (cutscene_headlook.md B1-B7, 2026-08-03): the FSM runs for EVERY actor with neck
         * data (+0x1b9 head part index != 0) — the player (caller @0x80031d78) AND the NPC
         * roots (jal 0x80037358 @0x8011c69c + siblings); the old `b==8 && player` gate left
         * NPCs headless and mis-owned NPC Plc_necks. Mechanics:
         *   origin = head part WORLD position; FUN_8003790c yaw/pitch (atan2 + SquareRoot0);
         *   head-LOCAL yaw = worldYaw - heading - rootBoneYaw; per-axis accumulator slews by
         *   a constant step, CLAMPED to the part limits, then ADDs to the keyframe euler
         *   (yaw -> vy +0x62, pitch -> vz +0x64), matrix built, offset restored. */
        if (bact && bact->neck_bone != 0 && b == (int)bact->neck_bone) {
            re15_actor_t *a = bact;
            uint8_t  fl = a->neck_flags;
            /* head MODEL pos = root_rot·relpos[b] + root_trans (root = poses[0]). */
            int32_t rp8[3] = { (int32_t)skel->bone_relative_pos[b][0],
                               (int32_t)skel->bone_relative_pos[b][1],
                               (int32_t)skel->bone_relative_pos[b][2] };
            int32_t hm[3]; mat3_apply(poses[0].rot, rp8, hm);
            hm[0] += poses[0].trans[0]; hm[1] += poses[0].trans[1]; hm[2] += poses[0].trans[2];
            /* head MODEL -> WORLD: headWorld = actorPos + Ryaw(rot_y)·hm. */
            int32_t cs = re15_cos_q12((int)a->rot_y), sn = re15_sin_q12((int)a->rot_y);
            int32_t ox = a->x + (( cs*hm[0] + sn*hm[2]) >> 12);
            int32_t oy = a->y + hm[1];
            int32_t oz = a->z + ((-sn*hm[0] + cs*hm[2]) >> 12);
            /* Cache the head WORLD pos for OTHER lookers' entity-tracking (the original's
             * part+0x54/58/5c, read next frame — cutscene_headlook.md B5). */
            a->head_world[0] = ox; a->head_world[1] = oy; a->head_world[2] = oz;
            a->head_world_ok = 1;

            /* --- target selection (FUN_80037358 flag bits) --- */
            int   have_world = 0;             /* world-point target in (twx,twy,twz)? */
            int32_t twx = 0, twy = 0, twz = 0;
            int   yaw_keyframe = 0, pit_keyframe = 0;   /* residual -> 0 (release) */
            int32_t rel_yaw = 0, rel_pit = 0; int rel = 0;
            if (fl & 0x04) {                  /* world point +0x160/162/164 (mode 1) */
                have_world = 1; twx = a->neck_tx; twy = a->neck_ty; twz = a->neck_tz;
            } else if (fl & 0x08) {           /* RELATIVE (modes 2/3/4): local yaw offset =
                                               * +0x162, pitch target = +0x164 (decompile
                                               * Z.37-41: world yaw = bodyYaw+heading+ofs ->
                                               * local = the raw offset) */
                rel = 1; rel_yaw = a->neck_ty; rel_pit = a->neck_tz;
            } else if (fl & 0x02) {           /* target = keyframe -> release */
                yaw_keyframe = 1; pit_keyframe = 1;
            } else {                          /* flags low bits 0 = ENTITY TRACKING: look at
                                               * the head part of +0x1a8 (NPC default = the
                                               * player @0x8011c738; player auto-look writes
                                               * it via FUN_8003703c). */
                int ts = (int)a->neck_target_slot;
                if (ts >= 0 && ts < RE15_ACTOR_MAX && g_actors[ts].active) {
                    have_world = 1;
                    if (g_actors[ts].head_world_ok) {
                        twx = g_actors[ts].head_world[0];
                        twy = g_actors[ts].head_world[1];
                        twz = g_actors[ts].head_world[2];
                    } else {                  /* not yet posed: fall back to the entity root */
                        twx = g_actors[ts].x; twy = g_actors[ts].y; twz = g_actors[ts].z;
                    }
                } else { yaw_keyframe = 1; pit_keyframe = 1; }
            }
            int32_t tgt_yaw = 0, tgt_pit = 0;
            int     yaw_active = 0, pit_active = 0;
            if (have_world) {
                /* FUN_8003790c world look angles. dy = origin.y - target.y (Y inverted);
                 * horiz via the byte-true SquareRoot0 replica (audit wf_8cc15b53). */
                int32_t dx = twx - ox, dz = twz - oz, dy = oy - twy;
                uint32_t horiz = re15_squareroot0((uint32_t)(dx*dx + dz*dz));
                int32_t wYaw = ((int32_t)re15_atan2_q12(dz, dx) - 1024) & 0xFFF;
                int32_t wPit = ((int32_t)re15_atan2_q12(dy, (int32_t)horiz) - 1024) & 0xFFF;
                tgt_yaw = (((wYaw - (int32_t)a->rot_y - (int32_t)root_kf_ay) + 0x800) & 0xFFF) - 0x800;
                tgt_pit = ((wPit + 0x800) & 0xFFF) - 0x800;
                yaw_active = 1; pit_active = 1;
            }
            if (rel) { tgt_yaw = rel_yaw; tgt_pit = rel_pit; yaw_active = 1; pit_active = 1; }
            if (fl & 0x10) { pit_keyframe = 1; pit_active = 1; }   /* 0x10 Pitch-Null: der
                                               * Akku laeuft auf 0 zurueck (Bestandteil des
                                               * 0x12-Release und von Mode 4 = 0x58) */

            /* --- SWEEP modes (bits 0x40 yaw / 0x20 pitch; decompile Z.79-94): counter in
             * +0x160 (neck_tx); first pass seeds the target = the FULL clamp; on snap-arrival
             * the target MIRRORS (Spiegelformel = -alt in local terms) and the counter
             * decrements; at 0 -> flags = 0x12 (release) @0x80037698/@0x80037858. Mode 4
             * (0x58) = yaw sweep = Kopfschuetteln; mode 3 (0x2a) = pitch sweep = Nicken. */
            if (fl & 0x40) {
                if (!a->neck_sweep) { a->neck_sweep = 1; a->neck_ty = a->neck_clamp_yaw; }
                tgt_yaw = a->neck_ty; yaw_active = 1;
            }
            if (fl & 0x20) {
                if (!a->neck_sweep) { a->neck_sweep = 1; a->neck_tz = a->neck_clamp_pitch; }
                tgt_pit = a->neck_tz; pit_active = 1;
            }

            /* accumulator residual = (local target - keyframe euler); release -> 0. */
            int32_t resY = yaw_active && !yaw_keyframe
                             ? ((((tgt_yaw - (int32_t)ay) + 0x800) & 0xFFF) - 0x800) : 0;
            int32_t resP = pit_active && !pit_keyframe
                             ? ((((tgt_pit - (int32_t)az) + 0x800) & 0xFFF) - 0x800) : 0;
            /* CLAMP the residual to the part limits (FUN_80037358 Z.57-67: target reduction
             * BEFORE the slew; player ±0x200/±0x138 @0x800319b0-c4, NPC ±0x2c8/±0x138
             * @0x8011c7a0/b0). This bound is exactly what the measured "verdreht" (yaw -901,
             * pitch 426) exceeded — cutscene_headlook.md B3. */
            int32_t cY = (int32_t)a->neck_clamp_yaw, cP = (int32_t)a->neck_clamp_pitch;
            if (cY > 0) { if (resY >  cY) resY =  cY; if (resY < -cY) resY = -cY; }
            if (cP > 0) { if (resP >  cP) resP =  cP; if (resP < -cP) resP = -cP; }
            /* step source: bit 0x80 -> the SCD speed bytes (low yaw/high pitch, +0x9e/0x9f);
             * else the part defaults +0x98/+0x9a (player 96/96, NPC 64/48). A 0 byte holds
             * the axis (byte-true: the slew moves by the literal byte — 10D0 mode 2 sends
             * speed 0x0A00 = yaw frozen, pitch 10/frame). */
            int32_t stepY, stepP;
            if (fl & 0x80) { stepY = (int32_t)((uint16_t)a->neck_speed & 0xFF);
                             stepP = (int32_t)(((uint16_t)a->neck_speed >> 8) & 0xFF); }
            else           { stepY = a->neck_step_yaw; stepP = a->neck_step_pitch; }
            int32_t dY = (((resY - (int32_t)a->neck_yaw)   + 0x800) & 0xFFF) - 0x800;
            int snapY = 0;
            if      (dY >  stepY) a->neck_yaw = (int16_t)(a->neck_yaw + stepY);
            else if (dY < -stepY) a->neck_yaw = (int16_t)(a->neck_yaw - stepY);
            else                  { a->neck_yaw = (int16_t)resY; snapY = 1; }
            { static FILE *nt = NULL; static int nti = 0;
              if (!nti) { nti = 1; const char *pp = getenv("RE15_NECK_TRACE"); if (pp && *pp) nt = fopen(pp, "w"); }
              if (nt && yaw_active) { fprintf(nt, "slot=%d tgt_yaw=%d resY=%d neck_yaw=%d (step=%d fl=%02x)\n",
                                          (int)(a - g_actors), (int)tgt_yaw, (int)resY, (int)a->neck_yaw, (int)stepY, fl); fflush(nt); } }
            int32_t dP = (((resP - (int32_t)a->neck_pitch) + 0x800) & 0xFFF) - 0x800;
            int snapP = 0;
            if      (dP >  stepP) a->neck_pitch = (int16_t)(a->neck_pitch + stepP);
            else if (dP < -stepP) a->neck_pitch = (int16_t)(a->neck_pitch - stepP);
            else                  { a->neck_pitch = (int16_t)resP; snapP = 1; }
            /* sweep arrival: mirror + count down; 0 -> flags = 0x12 (auto-release,
             * @0x80037698 yaw / @0x80037858 pitch). */
            if ((fl & 0x40) && snapY) {
                a->neck_ty = (int16_t)(-a->neck_ty);                       /* Spiegelung */
                if (a->neck_tx > 0) a->neck_tx--;
                if (a->neck_tx <= 0) { a->neck_flags = 0x12; a->neck_sweep = 0; }
            }
            if ((fl & 0x20) && snapP) {
                a->neck_tz = (int16_t)(-a->neck_tz);
                if (a->neck_tx > 0) a->neck_tx--;
                if (a->neck_tx <= 0) { a->neck_flags = 0x12; a->neck_sweep = 0; }
            }
            ay = (int16_t)(ay + a->neck_yaw);   /* YAW  -> vy (+0x62) */
            az = (int16_t)(az + a->neck_pitch); /* PITCH -> vz (+0x64), byte-true (NOT ax) */
        }

        /* HURT torso-bend: the stagger handler adds the entity's +0x9c ramp onto ONE part's vz
         * (`lhu v0,100(s0); lhu v1,156(ent); addu; sh v0,100(s0)` @0x80105d54-64 / @0x80105df8-08,
         * s0 = model_pool+1204 = part 7 @0x80105ba4). Per-tick ADDITIVE, never accumulating: anim_set
         * rewrites part+96..100 from part+120..124 every tick before the handler adds (@0x8001f54c-58),
         * and the un-blended pose path is what runs here because the stagger holds +0x8f == 0
         * (@0x8001f408/@0x8001f41c). Same vz slot as the neck pitch above. */
        if (bact && bact->hurt_bend_bone >= 0 && b == (int)bact->hurt_bend_bone)
            az = (int16_t)(az + bact->hurt_bend_vz);

        int32_t local_rot[9];
        mat3_from_euler((int)ax, (int)ay, (int)az, local_rot);

        int32_t bone_rel_pos[3] = {
            (int32_t)skel->bone_relative_pos[b][0],
            (int32_t)skel->bone_relative_pos[b][1],
            (int32_t)skel->bone_relative_pos[b][2],
        };

        int parent = (int)skel->bone_parent[b];
        if (parent < 0 || parent >= b) {
            /* Root bone: parent matrix is identity, so its world trans is
             * just bone_relative_pos[0] (+ optional keyframe delta). The
             * PL00.EMR pelvis offset (~(0,-1804,0) → ~1800 PSX units UP)
             * lifts the model so feet sit at actor.y. Original RE1.5 in
             * math_gte_matrix_transform.c (FUN_80022da0) applies the
             * parent matrix to every bone uniformly including the root,
             * which for an identity parent reduces to "trans = bone_rel".
             *
             * AL-round 2026-05-26: prior code zeroed this (assuming root
             * = actor pivot), which collapsed the skeleton's vertical
             * span by ~1804 units → characters rendered ~1.8× smaller
             * than authored. That mismatched the H=fov>>7 cuts (RE2 ships
             * H=84 cuts that need full-height skeletons to fill the
             * frame). The fov_to_screen_dist clamp of 150 was a workaround
             * hiding this bug. */
            /* AL-round 2026-05-26: prior code zeroed this (assuming root
             * = actor pivot), which collapsed the skeleton's vertical
             * span by ~1804 units → characters rendered ~1.8× smaller
             * than authored. That mismatched the H=fov>>7 cuts (RE2 ships
             * H=84 cuts that need full-height skeletons to fill the
             * frame). The fov_to_screen_dist clamp of 150 was a workaround
             * hiding this bug.
             *
             * AX-round 2026-05-28: tested revert (= trans = kf only) but
             * user reports Elliot becomes mis-positioned. Keeping AL fix.
             * The 25-37% size mismatch vs ablauf was the CAMERA-PROJECTION
             * z-error of the old FLOAT LookAt — RESOLVED by the integer
             * setupCameraLookAtMatrix rewrite (camera_common.c). Audit
             * wf_afe47fe0 (2026-07-11) confirmed fov→H (fov>>7), the RTPS
             * formula (160+H·x/z), and the model-scale (identity) are all
             * factor-1.0 byte-true — no residual size factor remains here. */
            memcpy(poses[b].rot, local_rot, sizeof(local_rot));
            /* Root translation = keyframe px/py/pz ONLY (FUN_8001f3bc.c:28-37
             * assigns, does NOT add the EMR bind). The bind bone[0] is the
             * structural default that the keyframe overrides every frame. */
            (void)bone_rel_pos;
            poses[b].trans[0] = rt_x;
            poses[b].trans[1] = rt_y;
            poses[b].trans[2] = rt_z;
        } else {
            /* world_rot = parent_world_rot * local_rot
             * world_trans = parent_world_rot * bone_rel_pos + parent_world_trans */
            mat3_mul(poses[parent].rot, local_rot, poses[b].rot);

            int32_t rotated_offset[3];
            mat3_apply(poses[parent].rot, bone_rel_pos, rotated_offset);
            poses[b].trans[0] = rotated_offset[0] + poses[parent].trans[0];
            poses[b].trans[1] = rotated_offset[1] + poses[parent].trans[1];
            poses[b].trans[2] = rotated_offset[2] + poses[parent].trans[2];
        }
    }

    /* Mark prev valid so the NEXT motion change has a pose to blend from. The crossfade
     * counter (anim_frac, = FUN_8001f3bc +0x8f) is NOT decremented here: this fn can run
     * MORE than once per logic tick for the same actor (e.g. a shadow / second render pass
     * with g_anim_pose_actor set), which would decrement frac multiple times per tick. The
     * original decrements +0x8f once per 30 Hz frame, so we do it once per LOGIC TICK in
     * re15_player_tick (player + NPCs), locked 1:1 with the anim_frame advance.
     * (NB: this is a render-correctness fix; it is NOT what made the ROOM1150 kneel look fast —
     * that was the wrong animation BANK, see anim_select_common.c anim_use_pl00.) */
    if (bact) bact->anim_prev_valid = 1;
    return 0;
}

/* Model-local bone translation -> world. Same Ryaw convention + int64-accumulate-then->>12
 * as the NPC render loop (platform main.c: nyaw[]={c,0,s, 0,1,0, -s,0,c}; trans rotated then
 * + actor.xyz) and the stair foot probe (stair_common.c). Single source of truth for the
 * transform so a queried bone (e.g. the lunge attack point) is bit-identical to what renders. */
void re15_skel_bone_to_world(const int32_t trans[3], int16_t yaw,
                             int32_t ox, int32_t oy, int32_t oz, int32_t out[3])
{
    if (!trans || !out) return;
    int32_t cs = re15_cos_q12((int)yaw);
    int32_t sn = re15_sin_q12((int)yaw);
    out[0] = ox + (int32_t)(( (int64_t)cs * trans[0] + (int64_t)sn * trans[2]) >> 12);
    out[1] = oy + trans[1];
    out[2] = oz + (int32_t)((-(int64_t)sn * trans[0] + (int64_t)cs * trans[2]) >> 12);
}

/* TEST HOOK (byte-true RotMatrix verification, audit #1): a non-static thin wrapper so
 * test_rotmatrix.c can drive the internal mat3_from_euler over a wide angle vector and compare it,
 * element-for-element, to an independent transliteration of RE_15_Quellcode_V2/RotMatrix.c. Not
 * used by the engine itself. */
void re15_skel_euler_matrix_for_test(int ax, int ay, int az, int32_t m[9])
{
    mat3_from_euler(ax, ay, az, m);
}
