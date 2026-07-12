/*
 * RE1.5 Rebuilt — Camera math (Phase 4.5.8.1, 2026-05-19).
 *
 * Target-agnostic view-matrix builder + bundled ROOM1100 test cuts.
 *
 * ⚠️ HISTORY: this once built the matrix in FLOAT (a `forward=(target-pos)/|..|`
 * cross-product LookAt + Newton-Raphson sqrt + Q12_ROUND). That was NOT byte-true and
 * has been REPLACED — see re15_camera_build_view below. The byte-true implementation is
 * the pure-INTEGER RE1.5 setupCameraLookAtMatrix (FUN_80053ca4 @0x80053ca4):
 *   dist  = SquareRoot0(dx²+dy²+dz²);  horiz = SquareRoot0(dx²+dz²)   (the BIOS table sqrt)
 *   sin/cos of pitch+yaw = TRUNCATING integer division (component*4096)/dist   (no float, no rounding)
 *   V = Rx(pitch) · Ry(yaw) via per-product-truncated GTE MulMatrix; t = V·(-pos) via ApplyMatrixLV.
 * fov→H = fov>>7 (fov_to_screen_dist); the projection sx=160+H·x/z is byte-true (audit wf_afe47fe0
 * confirmed fov→H, the RTPS formula, and the model-scale are all factor-1.0 byte-true — the historical
 * "25-37% character-size mismatch" was THIS float LookAt's z-error and is resolved by the integer form).
 *
 * The matrix is Q12 (0x1000 = 1.0), matching skeleton_common.c so the renderer mul's view × bone
 * with the same Q12 helpers. Called ONCE per cut change, never per frame.
 */

#include <stdint.h>
#include "re15_camera.h"
#include "re15_math.h"       /* re15_squareroot0 — the engine's ONLY sqrt (BIOS 0x80065f60) */

#define ONE_Q12     0x1000

/* (the float Newton-Raphson my_sqrtf is gone — the only caller, the orbit-radius init, now uses the
 * byte-true re15_squareroot0; the view matrix is integer SquareRoot0 via re15_camera_build_view.) */

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

static uint32_t cam_isqrt(uint32_t x);   /* byte-true SquareRoot0 (defined below) */

int re15_camera_build_view(const re15_camera_cut_t *cut,
                            re15_camera_view_t      *view_out)
{
    if (!cut || !view_out) return -1;

    /* BYTE-TRUE RE1.5 setupCameraLookAtMatrix (FUN_80053ca4 @0x80053ca4; audit #3). The old code
     * built the view matrix in FLOAT (my_sqrtf + float products) then rounded to Q12 with +0.5 — the
     * "T-REZ2" note claimed rounding matched the GTE, but the original is pure INTEGER: SquareRoot0
     * for the magnitudes + TRUNCATING integer division ((component*4096)/dist) for the normalized
     * sin/cos of pitch and yaw, then V = Rx(pitch) * Ry(yaw) via the GTE MulMatrix, translation via
     * ApplyMatrixLV. No float, no +0.5 rounding = the camera half of the pixel-shift. Rx/Ry are
     * sparse, so each V element reduces to one (a*b)>>12 product (per-product == sum truncation). */
    int32_t dx = cut->target_x - cut->pos_x;
    int32_t dy = cut->target_y - cut->pos_y;
    int32_t dz = cut->target_z - cut->pos_z;
    /* dist + horiz via the BIOS SquareRoot0 (FUN_80053ca4 jal 0x80053d60 + 0x80053dd8 -> 0x80065f60),
     * NOT an exact isqrt — the table approximation feeds the normalized sin/cos, so it is part of the
     * byte-true camera (pixel-shift). Audit wf_f066b2ae. */
    int32_t dist  = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dy*dy + (int64_t)dz*dz));
    if (dist == 0) return -2;                                   /* pos == target -> degenerate (the original traps) */
    int32_t horiz = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
    /* pitch: sin_p = (pos.y - target.y)*4096/dist = -dy*4096/dist; cos_p = horiz*4096/dist (sVar1/uVar7) */
    int32_t sp = (int16_t)(int32_t)(((int64_t)(-dy) * 4096) / dist);
    int32_t cp = (int16_t)(int32_t)(((int64_t)horiz * 4096) / dist);
    int32_t r0, r1, r2, r3, r4, r5, r6, r7, r8;
    if (horiz != 0) {
        int32_t sy = (int16_t)(int32_t)(((int64_t)dx * 4096) / horiz);   /* sin_yaw (uVar7') */
        int32_t cy = (int16_t)(int32_t)(((int64_t)dz * 4096) / horiz);   /* cos_yaw (uVar8) */
        r0 = cy;                        r1 = 0;   r2 = -sy;
        r3 = (sp * sy) >> 12;           r4 = cp;  r5 = (sp * cy) >> 12;
        r6 = (cp * sy) >> 12;           r7 = -sp; r8 = (cp * cy) >> 12;
    } else {                                                    /* straight up/down: skip yaw, V = Rx */
        r0 = 4096; r1 = 0;   r2 = 0;
        r3 = 0;    r4 = cp;  r5 = sp;
        r6 = 0;    r7 = -sp; r8 = cp;
    }
    view_out->rot[0] = r0; view_out->rot[1] = r1; view_out->rot[2] = r2;
    view_out->rot[3] = r3; view_out->rot[4] = r4; view_out->rot[5] = r5;
    view_out->rot[6] = r6; view_out->rot[7] = r7; view_out->rot[8] = r8;
    /* translation t = V * (-pos): ApplyMatrixLV = per-row MAC then >>12 (int64 accumulate) */
    {
        int64_t npx = -(int64_t)cut->pos_x, npy = -(int64_t)cut->pos_y, npz = -(int64_t)cut->pos_z;
        view_out->trans[0] = (int32_t)(((int64_t)r0*npx + (int64_t)r1*npy + (int64_t)r2*npz) >> 12);
        view_out->trans[1] = (int32_t)(((int64_t)r3*npx + (int64_t)r4*npy + (int64_t)r5*npz) >> 12);
        view_out->trans[2] = (int32_t)(((int64_t)r6*npx + (int64_t)r7*npy + (int64_t)r8*npz) >> 12);
    }

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
#include "re15_actor.h"      /* re15_atan2_q12 — the byte-true catan bearing (shared) */

/* (The old inline cam_atan2_q12 used the `num*512/den` linear-in-tangent approximation — even coarser
 * than the removed ATAN256 LUT, up to ~4° error. It now routes through the byte-true catan bearing
 * re15_atan2_q12. This animator is presently dead code, but the duplicate divergent atan2 was a
 * landmine if re-enabled; unified so there is ONE byte-true heading in the engine.) */

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
    /* Orbit radius = SquareRoot0(dx²+dz²): RE1.5's cut-init (FUN_80015850 @0x8001538c) uses the BIOS
     * table sqrt (jal 0x80065f60), NOT a float Newton-Raphson — SquareRoot0 UNDER-estimates
     * floor(sqrt) (#16). Low-32-bit mult arg (mflo), like the console. NOTE: presently vestigial —
     * the per-frame orbit rebuild (cam.x = rcos(yaw)*r + tgt.x) is SKIPPED in this port (see
     * re15_camera_animator_tick); this keeps the radius byte-true for when the orbit is re-enabled. */
    uint32_t rr = (uint32_t)((int64_t)dx * dx + (int64_t)dz * dz);
    anim->dist_residual   = (int32_t)re15_squareroot0(rr);
    anim->y_dist_residual = 0;
    anim->dist_step       = 0;
    anim->y_dist_step     = 0;
    anim->yaw_q12         = re15_atan2_q12(dz, dx);   /* byte-true catan bearing (was cam_atan2_q12 approx) */
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

/* Build the RotY(yaw) matrix from an ANGLE via the game trig LUT — byte-true to RE1.5's RotMatrixY
 * (0x800659d0), which reads DAT_800794c4[yaw&0xfff] = (cos<<16)|sin (Q12) and writes
 *   [ cos   0   sin ]
 *   [  0  0x1000  0 ]
 *   [-sin   0   cos ]
 * The character shadow (FUN_8001b064 @0x8001b0e4) calls RotMatrixY(ACTOR rot_y = entity+0x6a) — the
 * actor's own facing, NO sqrt, NO camera vector. re15_sin_q12/re15_cos_q12 ARE that LUT. Feed out_rot
 * to re15_camera_compose_view_bone. (RE'd wf_13911cba; replaces the old forward-vector normalize +
 * cam_isqrt, a port-only construct with the wrong angle source AND a sqrt the PSX never does.) */
void re15_camera_yaw_matrix_angle(int16_t yaw, int32_t out_rot[9])
{
    int32_t sy = re15_sin_q12((int)yaw);
    int32_t cy = re15_cos_q12((int)yaw);
    out_rot[0] =  cy; out_rot[1] = 0;      out_rot[2] = sy;
    out_rot[3] =  0;  out_rot[4] = 0x1000; out_rot[5] = 0;
    out_rot[6] = -sy; out_rot[7] = 0;      out_rot[8] = cy;
}
