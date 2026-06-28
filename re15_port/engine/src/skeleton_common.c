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
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_actor.h"     /* g_actors for Plc_neck head-look */

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

/* Composite Euler rotation = Ry * Rx * Rz (YXZ Euler — apply Rz first to a
 * column vector, then Rx, then Ry). Ported byte-for-byte from RE2's stock
 * `RotMatrix` at `RE2_Quellcode/RotMatrix.c:25-39` so per-bone orientation
 * matches the original engine exactly. Previously this routine built
 * `Rx*Ry*Rz` (XYZ) which scrambles every bone with non-zero pitch+roll. */
static void mat3_from_euler(int ax, int ay, int az, int32_t m[9])
{
    int32_t sx = re15_sin_q12(ax), cx = re15_cos_q12(ax);
    int32_t sy = re15_sin_q12(ay), cy = re15_cos_q12(ay);
    int32_t sz = re15_sin_q12(az), cz = re15_cos_q12(az);
    /* M-round (2026-05-25): the off-diagonal rows are 2-term sums where
     * each term has different multiply-depth (sz·cx is Q24, cz·sy·sx is
     * Q36). Old form `Q12_MUL(a,b) + Q12_MUL(Q12_MUL(c,d),e)` truncated
     * the triple-product TWICE before summing — up to 2 LSB error. New
     * form accumulates everything in Q36 and shifts once at the end. */
    /* Row 0 */
    m[0] = Q12_MUL(cz, cy);
    m[1] = -Q12_MUL(sz, cy);
    m[2] = sy;
    /* Row 1 — sum two terms in Q36 then truncate once. */
    {
        int64_t t1 = ((int64_t)sz * (int64_t)cx) << 12;            /* Q24<<12 = Q36 */
        int64_t t2 = (int64_t)cz * (int64_t)sy * (int64_t)sx;      /* Q36 */
        m[3] = (int32_t)((t1 + t2) >> 24);
    }
    {
        int64_t t1 = ((int64_t)cz * (int64_t)cx) << 12;
        int64_t t2 = (int64_t)sz * (int64_t)sy * (int64_t)sx;
        m[4] = (int32_t)((t1 - t2) >> 24);
    }
    m[5] = -Q12_MUL(cy, sx);
    /* Row 2 — same pattern. */
    {
        int64_t t1 = ((int64_t)sz * (int64_t)sx) << 12;
        int64_t t2 = (int64_t)cz * (int64_t)sy * (int64_t)cx;
        m[6] = (int32_t)((t1 - t2) >> 24);
    }
    {
        int64_t t1 = ((int64_t)cz * (int64_t)sx) << 12;
        int64_t t2 = (int64_t)sz * (int64_t)sy * (int64_t)cx;
        m[7] = (int32_t)((t1 + t2) >> 24);
    }
    m[8] = Q12_MUL(cy, cx);
}

/* ----- Public API ------------------------------------------------------ */

/* BYTE-TRUE current-pose actor (= the original's DAT_800ac784). The renderer sets
 * this to the actor being posed so the FRAC crossfade below can read its blend state;
 * NULL = no blend (pose QUERY, e.g. the stair foot-position probe). */
void *g_anim_pose_actor = NULL;

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

    /* FRAC crossfade state (FUN_8001f3bc): blend from the actor's previously
     * rendered pose into this keyframe over anim_frac frames. wp/wc = prev/current
     * weights (0x200 per FRAC step, matching the 0x200 unit the PSX caller passes). */
    re15_actor_t *bact   = (re15_actor_t *)g_anim_pose_actor;
    int           blend  = (bact && bact->anim_frac > 0 && bact->anim_prev_valid);
    /* wp = prev weight = frac * 0x200 (byte-true FUN_8001f3bc: weight = entity+0x8f * param_5,
     * param_5 = 0x200 from FUN_80035538/FUN_80036718; clamp to 0x1000). The blend is recursive
     * (prev = the blended output, snapshotted below) and only smooths the first few frames. */
    int           wp     = blend ? ((int)bact->anim_frac * 0x200) : 0;
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

        /* Plc_neck head-look on bone 8 (PL00 head = mesh 6). BYTE-TRUE port of
         * FUN_80037358 + FUN_8003790c (the REAL head-look; FUN_80024c30/FUN_80024e40 is a
         * SEPARATE spring secondary-motion chain — not this). The original:
         *   origin = head bone WORLD position;
         *   worldYaw   = atan2(dz,dx) in the mesh-facing convention (re15_atan2_q12-1024);
         *   worldPitch = atan2(dy,horiz), dy = origin.y - target.y (PSX Y inverted);
         *   head-LOCAL target yaw = worldYaw - actorHeading(rot_y) - rootBoneYaw  (a SCALAR
         *     angle subtraction — NOT a 3D matrix transform, which was the earlier bug);
         *     head-local target pitch = worldPitch;
         *   per-axis accumulator slews toward (target - keyframe) by a CONSTANT max step
         *     (the Plc_neck speed bytes wk+0x9e/0x9f = 96/96 in room1150), then ADD to the
         *     head bone's keyframe euler — YAW -> ay (vy, +0x62), PITCH -> az (vz, +0x64,
         *     NOT ax). The original adds the offset, builds the matrix, then subtracts it
         *     back; our accumulator (neck_yaw/neck_pitch) is exactly that persistent offset.
         * Player-only — enemies never Plc_neck; bone 3 is the LEG knee so it must never get
         * this. (Per-bone yaw/pitch RANGE clamp wk+0x9c/0x9a not yet extracted from the model
         * neck-data init — omitted; room1150 look stays within range.) */
        if (b == 8 && bact == &g_actors[RE15_ACTOR_SLOT_PLAYER]) {
            re15_actor_t *a = bact;
            int      active = (a->neck_flags & 0x80) && (a->neck_flags & 0x04);
            int32_t  tgt_yaw = 0, tgt_pit = 0;
            if (active) {
                /* head MODEL pos = root_rot·relpos[8] + root_trans (root = poses[0]). */
                int32_t rp8[3] = { (int32_t)skel->bone_relative_pos[8][0],
                                   (int32_t)skel->bone_relative_pos[8][1],
                                   (int32_t)skel->bone_relative_pos[8][2] };
                int32_t hm[3]; mat3_apply(poses[0].rot, rp8, hm);
                hm[0] += poses[0].trans[0]; hm[1] += poses[0].trans[1]; hm[2] += poses[0].trans[2];
                /* head MODEL -> WORLD: headWorld = actorPos + Ryaw(rot_y)·hm
                 * (Ryaw = [cs 0 sn; 0 1 0; -sn 0 cs], mesh faces +X at rot_y=0). */
                int32_t cs = re15_cos_q12((int)a->rot_y), sn = re15_sin_q12((int)a->rot_y);
                int32_t ox = a->x + (( cs*hm[0] + sn*hm[2]) >> 12);
                int32_t oy = a->y + hm[1];
                int32_t oz = a->z + ((-sn*hm[0] + cs*hm[2]) >> 12);
                /* FUN_8003790c world look angles. dy = origin.y - target.y (Y inverted). */
                int32_t dx = (int32_t)a->neck_tx - ox;
                int32_t dz = (int32_t)a->neck_tz - oz;
                int32_t dy = oy - (int32_t)a->neck_ty;
                uint32_t h2 = (uint32_t)(dx*dx + dz*dz);
                uint32_t horiz = 0; { uint32_t bb = 1u<<30, xx = h2, rr = 0;  /* int sqrt */
                    while (bb > xx) bb >>= 2;
                    while (bb) { if (xx >= rr+bb){ xx -= rr+bb; rr = (rr>>1)+bb; } else rr >>= 1; bb >>= 2; }
                    horiz = rr; }
                /* worldYaw/worldPitch in mesh-facing convention (= re15_atan2_q12(a,b)-1024,
                 * which equals FUN_8003790c's 0x1000-catan(a<<12/b)-quadrant form). */
                int32_t wYaw = ((int32_t)re15_atan2_q12(dz, dx) - 1024) & 0xFFF;
                int32_t wPit = ((int32_t)re15_atan2_q12(dy, (int32_t)horiz) - 1024) & 0xFFF;
                /* head-LOCAL absolute targets: scalar reduce yaw by actor heading + root yaw;
                 * pitch is absolute (the head's parent has ~no pitch). Wrap to signed-12. */
                tgt_yaw = (((wYaw - (int32_t)a->rot_y - (int32_t)root_kf_ay) + 0x800) & 0xFFF) - 0x800;
                tgt_pit = ((wPit + 0x800) & 0xFFF) - 0x800;
            }
            /* accumulator target = (head-local target - this bone's keyframe euler); on
             * release (no active look-at) the accumulator eases to 0 = back to keyframe. */
            int32_t resY = active ? ((((tgt_yaw - (int32_t)ay) + 0x800) & 0xFFF) - 0x800) : 0;
            int32_t resP = active ? ((((tgt_pit - (int32_t)az) + 0x800) & 0xFFF) - 0x800) : 0;
            /* constant max-step slew toward the residual, snap within one step.
             * step = Plc_neck speed bytes (wk+0x9e yaw / wk+0x9f pitch); 96/96 in room1150. */
            int32_t stepY = (int32_t)((uint16_t)a->neck_speed & 0xFF);        if (stepY < 1) stepY = 1;
            int32_t stepP = (int32_t)(((uint16_t)a->neck_speed >> 8) & 0xFF); if (stepP < 1) stepP = 1;
            int32_t dY = (((resY - (int32_t)a->neck_yaw)   + 0x800) & 0xFFF) - 0x800;
            if      (dY >  stepY) a->neck_yaw = (int16_t)(a->neck_yaw + stepY);
            else if (dY < -stepY) a->neck_yaw = (int16_t)(a->neck_yaw - stepY);
            else                  a->neck_yaw = (int16_t)resY;
            int32_t dP = (((resP - (int32_t)a->neck_pitch) + 0x800) & 0xFFF) - 0x800;
            if      (dP >  stepP) a->neck_pitch = (int16_t)(a->neck_pitch + stepP);
            else if (dP < -stepP) a->neck_pitch = (int16_t)(a->neck_pitch - stepP);
            else                  a->neck_pitch = (int16_t)resP;
            ay = (int16_t)(ay + a->neck_yaw);   /* YAW  -> vy (+0x62) */
            az = (int16_t)(az + a->neck_pitch); /* PITCH -> vz (+0x64), byte-true (NOT ax) */
        }

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
             * user reports Elliot becomes mis-positioned. Keeping AL fix
             * — the 25-37% size mismatch vs ablauf must come from another
             * source (camera projection, viewport scaling, or PSX-side
             * factor we're missing). 50-agent RE follow-up dispatched. */
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
