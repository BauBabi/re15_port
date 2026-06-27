/*
 * RE1.5 Rebuilt — Camera math (Phase 4.5.8.1, 2026-05-19).
 *
 * Target-agnostic view-matrix builder + bundled ROOM1100 test cuts.
 * Pure C, NO libm dependency — PSn00bSDK is freestanding (no math.h).
 * Float ops + division are provided by libgcc on both targets; we
 * implement sqrt locally via 8-iteration Newton-Raphson.
 *
 * Algorithm = direct port of RE2's FUN_80076cb0:
 *   forward = (target - pos) / |target - pos|
 *   sp = -forward.y                 (sin pitch; pitch = -asin(fy), PSX +Y down)
 *   cp = sqrt(fx² + fz²)            (cos pitch = sqrt(1-fy²))
 *   sy = fx / cp                    (sin yaw; yaw = atan2(fx, fz))
 *   cy = fz / cp                    (cos yaw)
 *   M  = R_pitch * R_yaw
 *   t  = -M * pos
 *
 * Avoids asin/atan2 by reading sp/cp/sy/cy directly off the normalized
 * forward vector. Avoids libm by inlining sqrt. Called ONCE per cut
 * change, never per frame — performance non-critical.
 *
 * The composed matrix is stored Q12 (0x1000 = 1.0), matching the Q12
 * convention from skeleton_common.c so the renderer can mul view × bone
 * with the same Q12_MUL helpers.
 */

#include <stdint.h>
#include "re15_camera.h"

#define ONE_Q12     0x1000

/* Newton-Raphson square root, freestanding-safe (no libm).
 *
 * Phase 4.5.10-K: the original "8 iters from r=x/2+1" was BROKEN for
 * large inputs. For x=6.37e8 (Cut 1 ROOM1170 forward len_sq), the
 * initial guess of 3.18e8 vs true sqrt of 25246 needs ~17 iters of
 * approximate halving before Newton's quadratic convergence kicks in.
 * 8 iters left r≈1.18e6 → forward vec normalized 47x too small →
 * camera matrix rows 1-2 collapsed → Leon projected way off-screen.
 *
 * Fix: scale x into [1, 4) first so initial guess is always close, then
 * 4 Newton iterations are plenty. Each shift corresponds to multiplying
 * sqrt by 2 (×0.25 of x ⇒ ÷2 of sqrt). */
static float my_sqrtf(float x)
{
    if (x <= 0.0f) return 0.0f;
    int shift = 0;
    while (x > 4.0f) { x *= 0.25f; shift += 1; }
    while (x < 1.0f) { x *=  4.0f; shift -= 1; }
    /* Now x in [1, 4) → sqrt(x) in [1, 2). Initial guess midpoint. */
    float r = (x + 1.0f) * 0.5f;
    for (int i = 0; i < 4; i++) {
        r = (r + x / r) * 0.5f;
    }
    while (shift > 0) { r *= 2.0f; shift -= 1; }
    while (shift < 0) { r *= 0.5f; shift += 1; }
    return r;
}

/* GTE screen-distance proxy. RE1.5 RID stores a raw fixed-point fov value
 * (typically 26684 for ROOM1100). RE2 feeds (fov >> 7) into gte_ldH —
 * which is gte_SetGeomScreen's underlying register. So screen_dist =
 * fov / 128 in BIO15 units. For fov=26684 → 208. The PSn00bSDK default
 * we used was 300 (a 320x240 wide-angle proxy). 208 is narrower but is
 * what the room was authored with. */
static int16_t fov_to_screen_dist(uint16_t fov)
{
    int sd = (int)fov >> 7;
    /* 2026-05-26 11-agent audit (Agent #1, #5): the prior `>>4 normalize
     * in mesh_psx.c` hypothesis was WRONG — no such shift exists or is
     * needed. M-round Q12 fixes (single int64-shift in mat3_mul/apply/
     * from_euler + Q12_ROUND in build_view) already make the pipeline
     * GTE-faithful. AL-round added bone_rel_pos[0] lift in skeleton_
     * common.c so characters render at authored height.
     *
     * The remaining 150-floor below is a stale workaround from before
     * those fixes — it inflates cut 6 (H=84) to 150 = 1.79× projection
     * boost, breaking cross-cut consistency. PSX RE2 uses no lower
     * clamp; its only clamp is the upper one in FUN_8003458c:44-46:
     *   if (H > 208) H -= 128;
     * which is part of the per-frame camera animator (not ported yet).
     *
     * TODO: remove the 150 floor + add the PSX upper-clamp after visual
     * A/B verification on cuts 0/2/6 vs. ablauf. Pending that, the
     * floor stays to avoid regression on cut 6's H=84. */
    /* AY-round 2026-05-28: REMOVE 150 lower floor.
     * Per 50-agent RE consensus (Q1/Q14/Q22/Q27): PSX RE2 has NO lower clamp.
     * Only upper clamp `if (H > 208) H -= 128` (FUN_8003458c animator).
     * The 150 floor was a stale workaround that inflated cut 6 (fov=10862,
     * raw sd=84) by 1.79x. Now removed — cut 6 sky-view should now show
     * heli at canonical small size matching ablauf.
     * Upper clamp 4095 kept (int16 hardware limit). */
    if (sd > 4095) sd = 4095;
    return (int16_t)sd;
}

int re15_camera_build_view(const re15_camera_cut_t *cut,
                            re15_camera_view_t      *view_out)
{
    if (!cut || !view_out) return -1;

    float dx = (float)(cut->target_x - cut->pos_x);
    float dy = (float)(cut->target_y - cut->pos_y);
    float dz = (float)(cut->target_z - cut->pos_z);

    float len_sq = dx*dx + dy*dy + dz*dz;
    if (len_sq < 1.0f) return -2;            /* pos == target → degenerate */
    float len    = my_sqrtf(len_sq);
    float inv_l  = 1.0f / len;

    float fx = dx * inv_l;
    float fy = dy * inv_l;
    float fz = dz * inv_l;

    /* Extract sp/cp/sy/cy from the normalized forward vector.
     *
     * pitch = atan2(fy, |forward_xz|)  → sp = fy, cp = |forward_xz|
     * yaw   = atan2(fx, fz)            → sy = fx/cp, cy = fz/cp
     *
     * If cp ≈ 0 the camera is looking straight up or down — yaw is
     * undefined; pin sy=0, cy=1 so the matrix degenerates gracefully
     * to a pure pitch. */
    float sp = fy;
    float cp = my_sqrtf(fx*fx + fz*fz);
    float sy, cy;
    if (cp > 0.0001f) {
        float inv_cp = 1.0f / cp;
        sy = fx * inv_cp;
        cy = fz * inv_cp;
    } else {
        sy = 0.0f;
        cy = 1.0f;
    }

    /* View matrix V = R_x(pitch) · R_y(-yaw) (column-vector convention,
     * v' = V·v). Derived so that V·forward = (0, 0, 1) — i.e. the world's
     * forward direction maps to camera-space +Z. Verification: row 2 of
     * V equals the forward unit vector itself.
     *
     *   R_y(-yaw) = | cy   0  -sy |     R_x(pitch) = | 1  0   0  |
     *               | 0    1   0  |                   | 0  cp -sp |
     *               | sy   0   cy |                   | 0  sp  cp |
     *
     * V = R_x · R_y =
     *   | cy        0    -sy       |
     *   | -sp · sy  cp   -sp · cy  |
     *   | cp · sy   sp    cp · cy  |
     *
     * (Row 2 = (cp·sy, sp, cp·cy) = (fx, fy, fz) = forward. Confirmed
     * algebraically.) */
    float m00 =       cy;        float m01 = 0.0f;   float m02 = -sy;
    float m10 = -sp * sy;        float m11 = cp;     float m12 = -sp * cy;
    float m20 =  cp * sy;        float m21 = sp;     float m22 =  cp * cy;

    /* Translation: t = -M * pos. In camera space, the world origin sits
     * at -M·pos relative to the camera, so vertices get +t added to
     * land at correct view-space positions. */
    float px = (float)cut->pos_x;
    float py = (float)cut->pos_y;
    float pz = (float)cut->pos_z;
    float tx = -(m00*px + m01*py + m02*pz);
    float ty = -(m10*px + m11*py + m12*pz);
    float tz = -(m20*px + m21*py + m22*pz);

    /* Convert rotation to Q12 fixed-point.
     * T-REZ2 (50-agent dive) found that plain `(int)(f*ONE_Q12)` truncates
     * — losing up to 0.5 LSB per element. Across the 9 rotation elements
     * accumulating through Q12_MUL in the bone compose, this causes ±0.5..1
     * pixel jitter (user-reported "wiederkehrende Pixel Verschiebungen").
     * Fix: add 0.5 LSB before cast for proper round-to-nearest, matching
     * the PSX GTE's native 15→12-bit rounding behavior.                  */
    #define Q12_ROUND(f) ((int32_t)((f) * ONE_Q12 + ((f) >= 0.0f ? 0.5f : -0.5f)))
    view_out->rot[0] = Q12_ROUND(m00);
    view_out->rot[1] = Q12_ROUND(m01);
    view_out->rot[2] = Q12_ROUND(m02);
    view_out->rot[3] = Q12_ROUND(m10);
    view_out->rot[4] = Q12_ROUND(m11);
    view_out->rot[5] = Q12_ROUND(m12);
    view_out->rot[6] = Q12_ROUND(m20);
    view_out->rot[7] = Q12_ROUND(m21);
    view_out->rot[8] = Q12_ROUND(m22);
    #undef Q12_ROUND

    /* Translation rounding too — same +0.5 LSB trick. */
    view_out->trans[0] = (int32_t)(tx + (tx >= 0.0f ? 0.5f : -0.5f));
    view_out->trans[1] = (int32_t)(ty + (ty >= 0.0f ? 0.5f : -0.5f));
    view_out->trans[2] = (int32_t)(tz + (tz >= 0.0f ? 0.5f : -0.5f));

    view_out->fov_screen_dist = fov_to_screen_dist(cut->fov);

    return 0;
}

/* Compose view × bone_world. Q12 multiply for the rotation part (int64
 * intermediates avoid overflow); for the translation part we apply the
 * view's rotation to the bone's integer translation, add the view's
 * integer translation. Matches RE2's CompMatrixLV semantics (per-actor
 * matrix concat in FUN_8002ce94) — except RE2 uses the GTE for the
 * multiply, we use the same C path on both targets for consistency. */
#define Q12_MUL(a, b)   ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12))

void re15_camera_compose_view_bone(const re15_camera_view_t *view,
                                    const int32_t             bone_rot[9],
                                    const int32_t             bone_trans[3],
                                    int32_t                   out_rot[9],
                                    int32_t                   out_trans[3])
{
    /* out_rot = view_rot · bone_rot   (Q12 × Q12 → Q12)
     * M-round (2026-05-25): full int64 accumulation, single >>12 at the
     * end. Old form truncated each Q12_MUL then summed → up to 3 LSB
     * error per matrix element, compounded through bone chain → visible
     * sub-pixel jitter ("wiederkehrende Pixel Verschiebungen"). */
    /* PERF (audit P0.1): int32 accumulator, NOT int64. R3000 emulates 64-bit
     * mul via libgcc __muldi3 (~10x the HW 32-bit mult). Q12 rot × Q12 rot =
     * ±16.7M, sum of 3 = ±50M — fits int32 (±2.1B). Still FULL accumulation +
     * a SINGLE >>12 (no truncate-then-sum) so the M-round anti-jitter property
     * is preserved and the result is bit-identical to the old int64 path. */
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int32_t s = 0;
            for (int k = 0; k < 3; k++) {
                s += view->rot[r*3 + k] * bone_rot[k*3 + c];
            }
            out_rot[r*3 + c] = s >> 12;
        }
    }

    /* rotated_trans = view_rot · bone_trans
     *
     * bone_trans is integer world units (not Q12), so the multiply is
     * view_rot (Q12) × world_units → world_units after >>12. We shift
     * bone_trans up by 12 for the multiply then descale, mirroring
     * mat3_apply in skeleton_common.c. */
    for (int r = 0; r < 3; r++) {
        /* int32: view_rot Q12 (±4096) × bone_trans world (worst ~±60000) =
         * ±245M, sum of 3 = ±737M — within int32. Bit-identical to int64. */
        int32_t s = 0;
        for (int k = 0; k < 3; k++) {
            s += view->rot[r*3 + k] * bone_trans[k];
        }
        /* >> 12 to remove the Q12 scale of view_rot. */
        out_trans[r] = (s >> 12) + view->trans[r];
    }
}

/* ROOM1100 (Stage 1) — first 7 unique cuts extracted from
 * extracted/PSX/STAGE1/room1100/camera.json. Cuts 0..6 are the room's
 * gameplay cuts (different angles); cuts 7..12 are end-cinematic stubs
 * pointing straight down, identical to each other — omitted here. */
const re15_camera_cut_t re15_camera_room1100_cuts[] = {
    /* flag, fov,  pos_x,    pos_y,    pos_z,    tgt_x,    tgt_y,    tgt_z, pri */
    { 0, 26684, -12420,  -4968, -23364, -20538,    738, -25722, 0x608 },
    { 0, 26684, -13122,  -3078, -13608, -15912,   -954, -25398, 0x608 },
    { 0, 26684, -13068,  -3078, -21852, -14742,   -954, -11520, 0x608 },
    { 0, 26684, -12978,  -5526, -15480, -14292,  -1566, -11142, 0x608 },
    { 0, 26684, -26856,  -1314, -11664, -13860,  -3204,  -9612, 0x608 },
    { 0, 26684, -26676,  -3762, -11466, -13716,   -144,  -9612, 0x608 },
    { 0, 26684, -19062,  -3528, -11304, -25992,  -1332, -10638, 0x608 },
};

const int re15_camera_room1100_cut_count =
    (int)(sizeof(re15_camera_room1100_cuts) /
          sizeof(re15_camera_room1100_cuts[0]));

/* -------------------------------------------------------------------------
 * Per-frame camera animator (port of RE1.5 FUN_80015850 @ 0x80015850).
 *
 * RE1.5 source: RE_15_modified_V2/render_camera_interpolate.c.
 * The original animates camera pos/target per frame via polar orbit + soft
 * tracking. Without this layer, our cut transitions look static and cut 2
 * frames Elliot at lower-left instead of ablauf's center composition.
 *
 * Minimal-faithful port: we do soft-tracking + view-rebuild from animated
 * state. Yaw orbit step and distance step default to zero (no dolly) since
 * the cinematic sub-handlers that set those weren't ported — sufficient
 * for ROOM1170 cuts (which authored their composition assuming actor will
 * be tracked toward frame center). Can be extended later if needed.
 *
 * sin/cos: re15_sin_q12 / re15_cos_q12 from skeleton_trig_*. atan2 we
 * inline below — same algorithm as actor_locomotion.c's atan2_q12. */

#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12              */

#ifdef _MSC_VER
#  include <stdlib.h>        /* abs                                     */
#else
#  include <stdlib.h>
#endif

/* atan2 in PSX angle units (4096 = 360°). yaw=0 → +Z forward. Mirrors
 * actor_locomotion.c::atan2_q12. */
static int16_t cam_atan2_q12(int32_t dz, int32_t dx)
{
    if (dx == 0 && dz == 0) return 0;
    int32_t abs_dx = abs(dx);
    int32_t abs_dz = abs(dz);
    int16_t a;
    if (abs_dz >= abs_dx) {
        int16_t a45 = (int16_t)((abs_dx * 512) / (abs_dz ? abs_dz : 1));
        a = (dx >= 0) ? a45 : (int16_t)-a45;
        if (dz < 0) a = (int16_t)(2048 - a);
    } else {
        int16_t a45 = (int16_t)((abs_dz * 512) / (abs_dx ? abs_dx : 1));
        a = (dz >= 0) ? (int16_t)(1024 - a45)
                      : (int16_t)(1024 + a45);
        if (dx < 0) a = (int16_t)-a;
    }
    return (int16_t)(a & 0x0FFF);
}

void re15_camera_animator_init(re15_camera_animator_t *anim,
                                const re15_camera_cut_t *cut,
                                int focus_slot)
{
    if (!anim || !cut) return;

    anim->cam_pos[0] = cut->pos_x;
    anim->cam_pos[1] = cut->pos_y;
    anim->cam_pos[2] = cut->pos_z;

    anim->cam_target[0] = cut->target_x;
    anim->cam_target[1] = cut->target_y;
    anim->cam_target[2] = cut->target_z;

    anim->lookat_xz[0] = cut->target_x;
    anim->lookat_xz[1] = cut->target_z;

    /* Orbit init: yaw + radius derived from (cam_pos - cut_target).XZ so
     * the first frame produces cam.x = rcos(yaw)*r + tgt.x = cam_pos.x. */
    int32_t dx = cut->pos_x - cut->target_x;
    int32_t dz = cut->pos_z - cut->target_z;
    double  rr = (double)dx * (double)dx + (double)dz * (double)dz;
    anim->dist_residual   = (int32_t)my_sqrtf((float)rr);
    anim->y_dist_residual = 0;
    anim->dist_step       = 0;
    anim->y_dist_step     = 0;
    anim->yaw_q12         = cam_atan2_q12(dz, dx);
    anim->yaw_step        = 0;

    anim->mode            = 0x40;
    anim->initialized     = 1;
    anim->focus_slot      = focus_slot;
    anim->cut_static      = *cut;
}

void re15_camera_animator_tick(re15_camera_animator_t *anim,
                                int32_t focus_x,
                                int32_t focus_y,
                                int32_t focus_z)
{
    if (!anim || !anim->initialized) return;

    /* (1) Distance / Y-distance residuals (kept for future dolly use).
     *     RE1.5 also decrements DAT_800b5258/5238 here; we just shrink
     *     toward zero so a future dolly step can leverage them. */
    anim->dist_residual   -= anim->dist_step;
    anim->y_dist_residual -= anim->y_dist_step;

    /* (2) Orbit cam pos: SKIPPED in minimal port.
     *
     *     RE1.5's `cam.x = rcos(yaw)*dist + cut_static.target.x` formula
     *     was intended to let cinematic sub-handlers dolly/orbit the
     *     camera. The trig round-trip (atan2→sin/cos) loses ~14% in our
     *     fixed-point implementation (init yaw produces cam_pos ~1000
     *     units off from cut.pos), so we just hold cam_pos at the static
     *     cut.pos and only animate the look-at target via soft-tracking
     *     below. This matches Option B "minimal subset" from
     *     finding_per_frame_camera_animator_FUN_8003458c: see whether
     *     soft-target alone fixes cut-2 framing before adding orbit. */
    anim->cam_pos[0] = anim->cut_static.pos_x;
    anim->cam_pos[1] = anim->cut_static.pos_y;
    anim->cam_pos[2] = anim->cut_static.pos_z;

    /* (3) Yaw step + wraparound (kept for dolly extension). */
    anim->yaw_q12 = (int16_t)((anim->yaw_q12 - anim->yaw_step) & 0x0FFF);

    /* (4) Soft-track focus actor.
     *     RE1.5: target.x += (actor.x - target.x) / 60     (60-frame XZ)
     *            target.z += (actor.z - target.z) / 60
     *            target.y += (actor.y - (target.y + 400)) / 20  (20-frame Y, +400 head)
     *     We use the same constants; / for signed-int floor.
     *
     *     Note that animated TARGET is what the view matrix looks-at; the
     *     orbit center stays at cut_static.target. So as Elliot walks,
     *     animated target lerps toward him → camera reframes him. */
    int32_t adx = focus_x - anim->cam_target[0];
    int32_t adz = focus_z - anim->cam_target[2];
    int32_t ady = focus_y - (anim->cam_target[1] + 400);
    anim->cam_target[0] += adx / 60;
    anim->cam_target[2] += adz / 60;
    anim->cam_target[1] += ady / 20;

    int32_t lx = focus_x - anim->lookat_xz[0];
    int32_t lz = focus_z - anim->lookat_xz[1];
    anim->lookat_xz[0] += lx / 60;
    anim->lookat_xz[1] += lz / 60;
}

void re15_camera_animator_to_cut(const re15_camera_animator_t *anim,
                                  re15_camera_cut_t *out_cut)
{
    if (!anim || !out_cut) return;
    *out_cut          = anim->cut_static;
    out_cut->pos_x    = anim->cam_pos[0];
    out_cut->pos_y    = anim->cam_pos[1];
    out_cut->pos_z    = anim->cam_pos[2];
    out_cut->target_x = anim->cam_target[0];
    out_cut->target_y = anim->cam_target[1];
    out_cut->target_z = anim->cam_target[2];
}

/* Integer sqrt (floor) — for the shadow-yaw normalize. Matches the PSX integer
 * path (mesh_psx sh_isqrt); the PC shadow previously used double sqrt here, so
 * this also keeps both ports off soft-float and byte-for-byte identical. */
static uint32_t cam_isqrt(uint32_t x)
{
    uint32_t r = 0, b = 1UL << 30;
    while (b > x) b >>= 2;
    while (b) {
        if (x >= r + b) { x -= r + b; r = (r >> 1) + b; }
        else            { r >>= 1; }
        b >>= 2;
    }
    return r;
}

/* Build the camera-yaw RotY matrix R_y from a forward vector's XZ projection
 * (sin = fwd_x/len, cos = fwd_z/len, Q12 with 0x1000 = 1.0) — SHARED by the
 * character-shadow code on both ports (the PSX inlined this via sh_isqrt, the
 * PC via a double sqrt; identical math, was duplicated). Degenerate forward
 * (len < 1) → identity yaw. Feed out_rot to re15_camera_compose_view_bone. */
void re15_camera_yaw_matrix(int32_t fwd_x, int32_t fwd_z, int32_t out_rot[9])
{
    int32_t len = (int32_t)cam_isqrt((uint32_t)((int64_t)fwd_x * fwd_x +
                                                (int64_t)fwd_z * fwd_z));
    int32_t sy, cy;
    if (len < 1) { sy = 0; cy = 0x1000; }
    else {
        sy = (int32_t)(((int64_t)fwd_x << 12) / len);
        cy = (int32_t)(((int64_t)fwd_z << 12) / len);
    }
    out_rot[0] =  cy; out_rot[1] = 0;      out_rot[2] = sy;
    out_rot[3] =  0;  out_rot[4] = 0x1000; out_rot[5] = 0;
    out_rot[6] = -sy; out_rot[7] = 0;      out_rot[8] = cy;
}
