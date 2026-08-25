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
#include "re15_scd.h"       /* g_re15_pauseflags + RE15_PAUSE_* — Freeze-Gate der Neck-FSM (s.u.) */

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

int re15_skel_root_y_fix(const re15_emd_skeleton_t *skel, int keyframe_index)
{
#ifdef RE15_PLATFORM_PC
    if (!skel || !skel->root_y_fix) return 0;
    if (keyframe_index < 0 || keyframe_index >= skel->root_y_fix_count) return 0;
    return (int)skel->root_y_fix[keyframe_index];
#else
    (void)skel; (void)keyframe_index;
    return 0;              /* PSX-Target kennt den Hybrid nicht */
#endif
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
    kf_py = (int16_t)(kf_py + re15_skel_root_y_fix(skel, keyframe_index));
    if (tween.active) {
        /* marker ROOT lerp (GTE GPF12/GPL12 @0x8001fa78-fae0): root = ((0x1000-w)*A + w*B) >> 12 */
        int16_t fx = 0, fy = 0, fz = 0;
        re15_emd_get_keyframe_position(skel, tween.kf_from, &fx, &fy, &fz);
        /* Die Hybrid-Korrektur haengt am KEYFRAME, also wird sie VOR dem Lerp auf BEIDE Seiten
         * gelegt und damit genauso mitgelerpt wie die Wurzel selbst. Wuerde man sie erst danach
         * addieren, spraenge sie an jeder Marker-Grenze um die Differenz der beiden Korrekturen. */
        fy = (int16_t)(fy + re15_skel_root_y_fix(skel, tween.kf_from));
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
         *   head-LOCAL yaw = worldYaw - heading - rootBoneYaw; das LOKALE ZIEL wird auf die
         *   Part-Klemmen begrenzt, DANN der Kopf-Keyframe-Euler abgezogen = Akku-Ziel; der
         *   Akku slewt mit konstantem Schritt und wird auf den Keyframe-Euler ADDIERT
         *   (yaw -> vy +0x62, pitch -> vz +0x64), Matrix gebaut, Offset zurueckgenommen.
         * KORREKTUR 2026-08-17 (Batch A1, Reports 1/3/4): die alte Etikette "CLAMPED ...
         * then ADDs" beschrieb die Klemme auf dem RESIDUAL (nach kf-Abzug). Die Bytes sagen
         * das Gegenteil — `subu a3,v1,s1` @0x80037598 bildet das Residual OHNE part+0x62,
         * die Klemme @0x800375a4-d0 sitzt darauf, und part+0x62 kommt erst im Slew-Delta
         * (`lhu a1,98(s0)` @0x800375dc). Ebenso ist Bit 0x10 ein ABSOLUTES Pitch-Ziel 0
         * (`sh zero,20(sp)` @0x80037560), kein "Akku -> 0". Beides unten korrigiert.
         *
         * KORREKTUR 2026-08-17 (Report "Irons schaut diagonaler", Runde 2): der frueher hier
         * stehende OPEN-Text ("Irons' NPC-INIT @0x8011d2b8 laeuft auch im Original NIE, der
         * Neck-Zustand wird aus dem Vorraum GEERBT") ist WIDERLEGT — statisch UND dynamisch:
         *   - `Sce_em_set` ruft den Typ-Root INLINE beim Spawn mit state 0 auf
         *     (`sw zero,4(s0)` @0x800421e0 ... `jalr v0` @0x8004259c) -> der INIT laeuft
         *     IMMER, bevor irgendein SCD-Opcode (auch Plc_motion) die Entity anfassen kann.
         *   - 7 NPC-Instanzen aus 5 sauberen Savestates (mzd_stage1_npc/dog/maggot,
         *     orig_1170_gp, lampwalk_base) stehen auf state=4 und tragen trotzdem +0x1b9=8,
         *     +0x1a8=&player, +0x9e=120, part8 step=(64,48), clamp=(0x2c8,0x138) — Werte, die
         *     nur der INIT schreibt.
         * `neck_bone != 0` bleibt als Gate stehen, ist aber keine Kruecke mehr, sondern der
         * exakte "+0x1b9 wurde gesetzt"-Test des Originals (`lbu ... 0x1b9(wk)` -> Part-Index).
         * Gefuellt wird es jetzt zur SPAWN-Zeit von re15_npc_neck_spawn_init()
         * (game_step_common.c), dem Port-Aequivalent des `jalr` @0x8004259c — dort steht auch
         * die vollstaendige Adress-Herleitung. */
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

            /* ===== FREEZE-GATE der Neck-FSM (Fix-Runde Cluster 1, 2026-08-17) =============
             * Selbst nachdisassembliert + selbst gescannt; die FSM ist im Original KEIN
             * Top-Level-Treiber, sie haengt ausschliesslich an Aufrufern, die sich vorher auf
             * ihr eigenes Pause-Bit gaten:
             *   SPIELER (Bit 0x80000000) — eigener wortweiser EXE-Scan nach `jal 0x80037358`
             *     (= 0x0C00DCD6) liefert in der GANZEN PSX.EXE genau EINEN Treffer: @0x80031d78.
             *     Der liegt INNERHALB von FUN_80031c44 zwischen
             *       80031c54  lw   a0,-13760(a0)      a0 = g_pauseflags (0x800aca40)
             *       80031c78  bltz a0,0x80031da8      Vorzeichen-Bit -> Sprung UEBER den Call
             *     und dem Ziel; @0x80031da8 enthaelt nur noch den Schatten-Call
             *     (@0x80031dcc `jal 0x8001b064`) und den Epilog (@0x80031dd4 `lw ra,20(sp)` ...
             *     @0x80031de0 `jr ra`). Steht 0x80000000, laeuft die Neck-FSM also NICHT.
             *   NPC (Bit 0x20000000) — die Overlay-Roots rufen sie; eigener Scan von STAGE1.BIN
             *     findet 6 Call-Sites (0x8011c69c, 0x8011cc6c, 0x8011d278, 0x8011d80c,
             *     0x8011dd58, 0x8011e320) und JEDER umschliessende Root traegt im Prolog das
             *     AI-Gate `lui v1,0x2000` (Roots 0x8011c5a0/0x8011cb70/0x8011d140/0x8011d6d4/
             *     0x8011dc68/0x8011e22c -> Gates @0x8011c5b8/cb88/d158/d6ec/dc80/e244).
             *     Exemplarisch 0x8011c5a0: @0x8011c5ac `addiu s0,s0,-13760` (= 0x800aca40) /
             *     @0x8011c5b4 `lw v0,0(s0)` / @0x8011c5b8 `lui v1,0x2000` / @0x8011c5bc and /
             *     @0x8011c5c0 `bne v0,zero,0x8011c6ac` — das Ziel liegt HINTER dem Call
             *     @0x8011c69c.
             * Der Port ruft diese FSM aus dem RENDER-Pfad (platform/pc/main.c), wo es kein
             * Pause-Bit gibt: ohne dieses Gate slewten die Koepfe waehrend jedes eingefrorenen
             * Textes weiter und die Sweep-Zaehler (+0x160 = neck_tx, unten `a->neck_tx--`)
             * liefen bis zum Auto-Release `neck_flags = 0x12` herunter — mitten im Freeze.
             * GEFROREN wird exakt die FSM (Ziel-Wahl, Klemme, Slew, Zaehler, Flags). Der bereits
             * erreichte AKKU wird weiterhin auf den Keyframe addiert (unten `ay + a->neck_yaw`),
             * denn im Original friert parallel der Pose-Builder FUN_80019e20 ein
             * (@0x80019e28 lw / @0x80019e3c and / @0x80019e40 `bne -> 0x8001a4a4` = Epilog,
             * Bit 0x10000000) und die zuletzt gebaute Kopf-Matrix bleibt unveraendert stehen.
             * Der `goto` bildet den `bltz`-Sprung 1:1 ab (Call uebersprungen, Rest laeuft). */
            {
                const uint32_t neck_pause_bit =
                    (a == &g_actors[RE15_ACTOR_SLOT_PLAYER]) ? RE15_PAUSE_PLAYER   /* @0x80031c78 */
                                                             : RE15_PAUSE_AI;      /* @0x8011c5c0 */
                if (g_re15_pauseflags & neck_pause_bit) goto neck_apply_acc;
            }

            /* --- target selection (FUN_80037358 flag bits) ---
             * BYTE-TRUE REIHENFOLGE, selbst nachdisassembliert 2026-08-17 (PSX.EXE, alle
             * Adressen unten im Code zitiert). Das Original haelt ZWEI Ziele auf dem Stack:
             *   18(sp) = ABSOLUTES Welt-Ziel-Yaw   ·   20(sp) = LOKALES Ziel-Pitch
             * und ueberschreibt sie in dieser Reihenfolge:
             *   0x04 Zielpunkt -> 0x08 relativ -> 0x40 Yaw-Sweep -> 0x02 Release-Yaw ->
             *   0x10 Pitch-Null -> [Yaw-Klemme+Slew] -> 0x20 Pitch-Sweep -> [Pitch-Klemme+Slew].
             * Der Port haelt beide Ziele LOKAL (= a3 des Originals, `18(sp)-heading-bodyYaw`
             * @0x80037580-98) — das ist dieselbe Groesse, nur ohne den konstanten Offset. */
            int   have_world = 0;             /* world-point target in (twx,twy,twz)? */
            int32_t twx = 0, twy = 0, twz = 0;
            int32_t rel_yaw = 0, rel_pit = 0; int rel = 0;
            if (fl & 0x04) {                  /* world point +0x160/162/164 (mode 1) */
                have_world = 1; twx = a->neck_tx; twy = a->neck_ty; twz = a->neck_tz;
            } else if (fl & 0x08) {           /* RELATIVE (modes 2/3/4): local yaw offset =
                                               * +0x162, pitch target = +0x164 (@0x80037480-b0:
                                               * 18(sp) = +0x162 + heading + bodyYaw -> lokal =
                                               * der rohe Offset; 20(sp) = +0x164 direkt) */
                rel = 1; rel_yaw = a->neck_ty; rel_pit = a->neck_tz;
            } else if (fl & 0x02) {           /* target = keyframe -> release (siehe unten) */
                /* nichts zu tun: die Default-Ziele unten SIND das Release */
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
                }
                /* sonst: PORT-GUARD (kein gueltiges Trackingziel) -> Release-Defaults unten.
                 * Das Original hat hier IMMER einen gueltigen +0x1a8-Zeiger (Default-Store
                 * `addiu v0,a1,48 / sw v0,0(a1)` = Entity-Slot 0 @0x800371d8-dc). */
            }
            /* LOKALE Ziele (a3 des Originals bzw. 20(sp)). Default = Release-Semantik. */
            int32_t tgt_yaw = (int32_t)ay;    /* Release-Ziel Yaw = Kopf-Keyframe-vy (s.u.) */
            int32_t tgt_pit = 0;
            if (have_world) {
                /* FUN_8003790c world look angles. dy = origin.y - target.y (Y inverted);
                 * horiz via the byte-true SquareRoot0 replica (audit wf_8cc15b53). */
                int32_t dx = twx - ox, dz = twz - oz, dy = oy - twy;
                uint32_t horiz = re15_squareroot0((uint32_t)(dx*dx + dz*dz));
                int32_t wYaw = ((int32_t)re15_atan2_q12(dz, dx) - 1024) & 0xFFF;
                int32_t wPit = ((int32_t)re15_atan2_q12(dy, (int32_t)horiz) - 1024) & 0xFFF;
                tgt_yaw = (((wYaw - (int32_t)a->rot_y - (int32_t)root_kf_ay) + 0x800) & 0xFFF) - 0x800;
                tgt_pit = ((wPit + 0x800) & 0xFFF) - 0x800;
            }
            if (rel) { tgt_yaw = rel_yaw; tgt_pit = rel_pit; }
            /* --- SWEEP-Seeds. Zaehler in +0x160 (neck_tx), Ziel in +0x162 (neck_ty), LATCH in
             * +0x164 (neck_tz) — BEIDE Sweeps teilen sich Latch UND Ziel-Feld; sie sind nie
             * gleichzeitig aktiv (Mode 3 = 0x2a nur Pitch, Mode 4 = 0x58 nur Yaw).
             *   YAW-Seed  @0x800374c8-f8: `lh v0,356(a0)` (+0x164) == 0 -> +0x164 = 1 und
             *                             +0x162 = part+0x9c + heading + bodyYaw (lokal = +Klemme)
             *   PITCH-Seed @0x800376fc-20: dieselbe Latch-Pruefung; +0x162 = part+0x9e
             *                             (das Pitch-Ziel IST bereits lokal)
             * Der Port benutzte hier ein privates Byte neck_sweep und seedete deshalb IMMER;
             * das Original seedet NICHT, wenn das Skript einen Pitch-Operanden != 0 gesetzt
             * hat (+0x164 ist bei Mode 2/3/4 zugleich der relative Pitch, @0x800374a8-b0).
             * Fuer die belegten Faelle identisch: ROOM1170 sub02 Plc_neck(4,3,0,0,0x3c) hat
             * pc[6..7] = 0 -> Latch frei -> Seed wie bisher. neck_sweep bleibt als
             * Kompatibilitaets-Feld stehen (scd_vm.c:2142 setzt es), wird hier aber nur noch
             * mitgefuehrt. */
            if (fl & 0x40) {                                        /* @0x800374bc-c0 */
                if (a->neck_tz == 0) {                              /* @0x800374c8-d0 */
                    a->neck_tz = 1;                                 /* @0x800374d4-d8 */
                    a->neck_ty = a->neck_clamp_yaw;                 /* @0x800374dc-f8 */
                    a->neck_sweep = 1;
                }
                tgt_yaw = a->neck_ty;                               /* @0x800374fc-510 */
            }
            /* 0x02 RELEASE-YAW @0x80037528-48: 18(sp) = heading + part+0x62 + bodyYaw, das
             * lokale Ziel ist also EXAKT der Kopf-Keyframe-Euler vy — der Kopf faehrt auf die
             * Keyframe-Pose zurueck. (Kommt NACH dem Yaw-Sweep -> 0x02 gewinnt; deshalb ist
             * Mode 3 = 0x2a yaw-seitig ein Release.) */
            if (fl & 0x02) tgt_yaw = (int32_t)ay;
            /* 0x10 PITCH-NULL @0x80037554-60 (`andi 0x10; sh zero,20(sp)`): das Ziel ist die
             * GESAMT-Neigung 0 — NICHT "Akku -> 0". Der Slew konvergiert (part+0x64 + Akku)
             * gegen 0 (@0x800377a0-b8), der Akku laeuft also auf -kf_vz. Savestate-Beleg:
             * montage1240_orig/orig_intro_late accP = -186 bei Keyframe vz = +186
             * (lane17_NECK.md 7.2). Der alte Port setzte resP = 0 -> der eingebackene
             * Keyframe-Pitch (~16 Grad gesenkt) blieb waehrend des ganzen Intros stehen. */
            if (fl & 0x10) tgt_pit = 0;
            if (fl & 0x20) {                                        /* @0x800376f0-f4 */
                if (a->neck_tz == 0) {                              /* @0x800376fc-704 */
                    a->neck_tz = 1;                                 /* @0x80037708-0c */
                    a->neck_ty = a->neck_clamp_pitch;               /* @0x80037718-20 */
                    a->neck_sweep = 1;
                }
                tgt_pit = a->neck_ty;                               /* @0x80037724-38 */
            }

            /* KLEMME auf das KOERPER-RESIDUAL, BEVOR der Kopf-Keyframe abgezogen wird
             * (@0x80037580-98: `subu a3,v1,s1` = Ziel - heading - bodyYaw, OHNE part+0x62;
             *  @0x800375a4-d0: (a3+cl)&0xfff > 2*cl -> Ziel = bodyYaw+heading ± cl, Vorzeichen
             *  aus a3&0x800; Pitch analog @0x80037760-98 auf dem absolut-lokalen 20(sp)).
             * Der Kopf-Keyframe kommt ERST im Slew-Delta dazu (`lhu a1,98(s0)` @0x800375dc /
             * `lhu v0,100(s0)` @0x800377a0). Der alte Port klemmte das Residual NACH dem
             * kf-Abzug -> gerenderter Kopf = kf ± Klemme statt ± Klemme (Report 3/6-N1).
             * Klemmen: Spieler ±0x200/±0x138 @0x800319b0-c4, NPC ±0x2c8/±0x138 @0x8011c7a0/b0. */
            int32_t cY = (int32_t)a->neck_clamp_yaw, cP = (int32_t)a->neck_clamp_pitch;
            if (((tgt_yaw + cY) & 0xFFF) > cY * 2) tgt_yaw = (tgt_yaw & 0x800) ? -cY : cY;
            if (((tgt_pit + cP) & 0xFFF) > cP * 2) tgt_pit = (tgt_pit & 0x800) ? -cP : cP;
            /* Akku-Ziel = geklemmtes LOKALES Ziel - Kopf-Keyframe-Euler (Snap-Form
             * @0x80037630-4c yaw / @0x800377ec-fc pitch). */
            int32_t resY = (((tgt_yaw - (int32_t)ay) + 0x800) & 0xFFF) - 0x800;
            int32_t resP = (((tgt_pit - (int32_t)az) + 0x800) & 0xFFF) - 0x800;
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
            /* YAW-Sweep-Ankunft @0x80037664-c4 (nur im Snap-Fenster, `beq` @0x80037628):
             * +0x160-- ; ==0 -> flags = 0x12 @0x80037698 ; danach IMMER spiegeln
             * `+0x162 = 2*(bodyYaw+heading) - +0x162` @0x800376a0-c4 = lokal -alt. */
            if ((fl & 0x40) && snapY) {
                a->neck_ty = (int16_t)(-a->neck_ty);                       /* Spiegelung */
                if (a->neck_tx > 0) a->neck_tx--;
                if (a->neck_tx <= 0) { a->neck_flags = 0x12; a->neck_sweep = 0; }
            }
            int32_t dP = (((resP - (int32_t)a->neck_pitch) + 0x800) & 0xFFF) - 0x800;
            int snapP = 0;
            if      (dP >  stepP) a->neck_pitch = (int16_t)(a->neck_pitch + stepP);
            else if (dP < -stepP) a->neck_pitch = (int16_t)(a->neck_pitch - stepP);
            else                  { a->neck_pitch = (int16_t)resP; snapP = 1; }
            /* PITCH-Sweep-Ankunft @0x80037814-a4 — das Ziel ALTERNIERT Klemme <-> 0, es wird
             * NICHT gespiegelt (Report 3/6-N3, selbst disassembliert):
             *   80037820 lh v0,20(sp) ; beq zero -> 0x80037874        (Ziel == 0?)
             *   80037830-3c +0x160-- ; 80037844 bne !=0 -> 0x8003785c ; 80037858 sb 0x12,+0x1b8
             *   80037868 sh zero,354(v0)    -> +0x162 = 0             (Ziel war != 0)
             *   80037880-a4 flags&0x20 && 20(sp)==0 -> +0x162 = part+0x9e (Ziel war 0)
             * Der Zaehler laeuft also NUR auf dem Weg zurueck zur 0 herunter (halbe Rate ggue.
             * dem Yaw-Sweep). Der alte Port spiegelte das Vorzeichen (-Klemme) und zaehlte
             * jeden Snap herunter. Ziel-Feld ist +0x162 = neck_ty (NICHT +0x164/neck_tz —
             * +0x164 ist im Original der Sweep-Latch). */
            if ((fl & 0x20) && snapP) {
                if (tgt_pit != 0) {
                    if (a->neck_tx > 0) a->neck_tx--;
                    if (a->neck_tx <= 0) { a->neck_flags = 0x12; a->neck_sweep = 0; }
                    a->neck_ty = 0;                       /* @0x80037868 */
                } else {
                    a->neck_ty = a->neck_clamp_pitch;     /* @0x8003789c-a4 */
                }
            }
            { static FILE *nt = NULL; static int nti = 0;
              if (!nti) { nti = 1; const char *pp = getenv("RE15_NECK_TRACE"); if (pp && *pp) nt = fopen(pp, "w"); }
              if (nt) { fprintf(nt, "slot=%d fl=%02x tgt=(%d,%d) res=(%d,%d) acc=(%d,%d) kf=(%d,%d) step=(%d,%d)\n",
                                (int)(a - g_actors), fl, (int)tgt_yaw, (int)tgt_pit, (int)resY, (int)resP,
                                (int)a->neck_yaw, (int)a->neck_pitch, (int)ay, (int)az,
                                (int)stepY, (int)stepP); fflush(nt); } }
        neck_apply_acc:
            /* Sprungziel des Freeze-Gates oben: der Akku wird IMMER addiert (im Freeze der
             * eingefrorene Stand — der Kopf haelt seine letzte Pose statt auf den Keyframe
             * zurueckzuschnappen). */
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

        /* RE2-TREFFERREAKTION — das Oberkoerper-ZUCKEN (Nutzer-Report "das feine Zucken beim
         * Treffer fehlt"). Das Original dreht NACH dem Anim-Advance zwei Part-Matrizen des
         * Modell-Pools (+0x198), Part 0 nach-, Part 1 vor-multipliziert mit der GEGENLAEUFIGEN
         * Matrix — die drei Fundstellen sind ein und derselbe Mechanismus:
         *   Haupt-Handler P1  @0x801057A4-E8  RotMatrix 0x8008E1F4 -> MulMatrix  0x8008D934(part0+24)
         *                     @0x801057EC-838 (Gegenwinkel)        -> MulMatrix2 0x8008DA44(part1+24)
         *   Haupt-Handler P2  @0x801058D4-960 (dasselbe ohne den <<3-Faktor)
         *   Ragdoll  0x801066FC @0x80106A04-3C und @0x80106CA0-D4 — dort steht die zweite Matrix
         *                     EXPLIZIT als TransposeMatrix 0x8008E1B4, was den Gegenwinkel-Trick
         *                     der beiden anderen Stellen als Transposition ausweist (Rz(-t) ==
         *                     Rz(t)^T, RotMatrix negiert bei negativem Winkel nur den Sinus
         *                     @0x8008E204-2C).
         * MulMatrix  (0x8008D934) schreibt das Ergebnis nach a0 -> part0 = part0 * R.
         * MulMatrix2 (0x8008DA44) schreibt nach a1                -> part1 = R^T  * part1.
         * Die Nach-Multiplikation von Part 0 mit Rz(t) ist identisch zu "az += t", weil
         * mat3_from_euler (= RotMatrix, RE_15_Quellcode_V2/RotMatrix.c) M = Rx*Ry*Rz baut; fuer den
         * Ragdoll-Vektor mit allen drei Achsen gilt das NICHT, darum steht hier die Matrix-Form,
         * die alle drei Fundstellen exakt trifft. Part-Index == Bone-Index (dieselbe Zuordnung, die
         * die RE1.5-Stagger-Beuge oben mit hurt_bend_bone = 7 = Pool+1204 benutzt).
         * WIRKUNG: bone1s Welt-Rotation kuerzt sich exakt weg (W0*R * R^T*L1 == W0*L1) — es dreht
         * sichtbar nur Bone 0, und der Rest des Koerpers wird um den mitgedrehten Bind-Offset
         * versetzt. Genau das ist das feine Zucken.
         * ABSOLUT pro Tick, nicht akkumulierend: das Original baut die Part-Matrizen im Advance
         * (FUN_8002959C, unmittelbar VOR jeder dieser Stellen) jeden Tick neu — sonst waere die
         * P2-Rampe +0x158 16->0 wirkungslos. */
        if (bact && bact->re2_lean_on && (b == 0 || b == 1)) {
            int32_t R[9], tmp[9];
            mat3_from_euler((int)bact->re2_lean[0], (int)bact->re2_lean[1],
                            (int)bact->re2_lean[2], R);
            if (b == 0) {
                mat3_mul(local_rot, R, tmp);                  /* MulMatrix  @0x801057E4 */
            } else {
                int32_t Rt[9];
                for (int r = 0; r < 3; r++)
                    for (int c = 0; c < 3; c++) Rt[r*3+c] = R[c*3+r];   /* 0x8008E1B4 */
                mat3_mul(Rt, local_rot, tmp);                 /* MulMatrix2 @0x80105834 */
            }
            memcpy(local_rot, tmp, sizeof local_rot);
        }
        /* Wurzel-Bone-Blend des Rutsch-Handlers 0x8010703C: FUN_80028F48 interpoliert die 9
         * Matrix-Shorts elementweise zwischen der Identitaet @0x8009DB44 (00 10 diag) und der
         * gesicherten Part-0-Matrix mit Gewicht w = +0x158 (GTE GPF12/GPL12 @0x80028F7C/@0x80028FA4,
         * Aufrufe @0x801072F8 und @0x801073C0): part0 = ((4096-w)*I + w*M) >> 12. */
        if (bact && bact->re2_bone0_wgt > 0 && b == 0) {
            int w = (int)bact->re2_bone0_wgt;
            if (w > 0x1000) w = 0x1000;
            for (int k = 0; k < 9; k++) {
                int32_t idv = (k == 0 || k == 4 || k == 8) ? RE15_SKEL_ONE : 0;
                local_rot[k] = (idv * (0x1000 - w) + local_rot[k] * w) >> 12;
            }
        }

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
