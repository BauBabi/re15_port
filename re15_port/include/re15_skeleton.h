/*
 * RE1.5 Rebuilt — Skeleton pose computation (Phase 4.5.7.2, 2026-05-18).
 *
 * Walks the EMR-parsed bone hierarchy at a given keyframe and produces
 * a world-space pose (3×3 rotation matrix + 3-vector translation) per
 * bone. Phase 4.5.7.3 will feed these matrices into the MD1 mesh
 * renderer so each body-part mesh draws at its bone's transformed
 * position.
 *
 * Coordinate / scaling conventions:
 *   - Euler angles read from EMR are 12-bit, in PSX trig units (4096
 *     = 360°). Rotation order matches PSn00bSDK's RotMatrix (ZYX).
 *   - Rotation matrix elements are Q12 fixed-point — 0x1000 = 1.0.
 *     Total mat-vec result is shifted right by 12 to descale.
 *   - Translations are s16-range PSX world units (post-accumulation),
 *     same scale as MD1 vertex coordinates.
 *
 * Trig: declared here, implemented per target. PSX wraps PSn00bSDK's
 * isin()/icos(); PC computes from sinf/cosf at boot.
 */
#ifndef RE15_SKELETON_H
#define RE15_SKELETON_H

#include <stdint.h>
#include "re15_emd.h"

#define RE15_SKEL_ONE       0x1000     /* 1.0 in Q12 */

/* World-space pose for one bone. Rotation is stored row-major; element
 * [r][c] is rot[r*3 + c]. */
typedef struct {
    int32_t rot[9];        /* 3x3 Q12                                    */
    int32_t trans[3];      /* x, y, z in s16-range PSX world units       */
} re15_skel_pose_t;

/* Per-target trig functions. Input: PSX trig units (4096 = full turn).
 * Output: Q12 (0x1000 = 1.0). PSX-side wraps libpsn00bsdk isin/icos;
 * PC-side computes via sinf/cosf. */
int re15_sin_q12(int angle_4096);
int re15_cos_q12(int angle_4096);

/* Compute the world-space pose for every bone in the skeleton at
 * `keyframe_index`. Walks parents-before-children using the parent[]
 * array filled by re15_emd_parse_skeleton().
 *
 * `poses` must have RE15_EMD_MAX_BONES slots. Returns 0 on success or
 * negative on error (null inputs / out-of-range keyframe). */
int re15_skel_compute_pose(const re15_emd_skeleton_t *skel,
                            int                       keyframe_index,
                            re15_skel_pose_t          poses[RE15_EMD_MAX_BONES]);

/* Wurzel-Y-Korrektur fuer die RE2-Hybrid-Skelette (siehe re15_emd.h, root_y_fix).
 * Liefert 0 fuer JEDES Skelett, das keine Tabelle traegt — also fuer den RE1.5-Flavor, den
 * Spieler, alle Nicht-Hybrid-Typen und die Victim-Bank. Damit ist der Fix ueber den
 * Skelett-ZEIGER verschluesselt, nicht ueber den Gegner-Typ. */
int re15_skel_root_y_fix(const re15_emd_skeleton_t *skel, int keyframe_index);

/* Transform one bone's MODEL-LOCAL pose translation (poses[b].trans, as produced by
 * re15_skel_compute_pose) into WORLD space for an actor at (ox,oy,oz) facing `yaw`:
 *
 *     world = (ox,oy,oz) + Ryaw(yaw) · trans
 *     Ryaw  = [  cos 0 sin ;  0 1 0 ; -sin 0 cos ]   (mesh faces +X at yaw 0)
 *
 * This is the EXACT model→world transform the NPC render loop (platform main.c) and the
 * stair foot-position probe (stair_common.c) apply per bone — factored out so a single
 * bone's world position can be queried in shared, testable engine code. Q12 trig, int64
 * accumulate then >>12 (GTE-faithful, bit-identical to the render path). `out` may not
 * alias `trans`. */
void re15_skel_bone_to_world(const int32_t trans[3], int16_t yaw,
                             int32_t ox, int32_t oy, int32_t oz, int32_t out[3]);

/* BYTE-TRUE keyframe crossfade (FUN_8001f3bc, 2026-06-15): the current actor
 * whose pose is being computed = the original's DAT_800ac784. re15_skel_compute_pose
 * reads its anim_frac / prev_angles / prev_root to blend the new clip in from the
 * prior rendered pose over `anim_frac` frames. The renderer sets this to the actor
 * pointer (re15_actor_t*) right before each body render, or NULL to disable the
 * blend (POSE QUERIES that must not mutate blend state — e.g. the stair foot-pos
 * query). void* to avoid an re15_actor.h dependency in this header. */
extern void *g_anim_pose_actor;

/* 0x8000 MARKER TWEEN (byte-true FUN_8001f8b4, trace wf_518cceff CONFIRMED): a marker anim frame
 * is a TWEEN — the original poses  ((0x1000-w)*A + w*B) >> 12  for the ROOT translation (GTE
 * GPF12/GPL12 @0x8001fa78-fae0) AND every bone angle (hidden-$a3 w -> FUN_8001fb94 ->
 * FUN_80020510 shortest-path lerp), where A = the last REAL frame posed, B = the next REAL frame
 * in playback direction, w = marker & 0xfff (complemented in reverse @0x8001f940-f948, which is
 * the SAME absolute pose — w always means "fraction toward the LATER bracketing frame").
 * Side channel in the g_anim_pose_actor pattern: re15_compute_actor_kf sets it whenever the
 * resolved slot is a marker (kf_from = the bracketing-BEFORE real frame's keyframe; the returned
 * kf = the bracketing-AFTER one); the next re15_skel_compute_pose consumes AND clears it. */
typedef struct { int active; int kf_from; int weight; } re15_kf_tween_t;
extern re15_kf_tween_t g_anim_kf_tween;

#endif /* RE15_SKELETON_H */
