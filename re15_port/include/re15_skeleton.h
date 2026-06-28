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

/* BYTE-TRUE keyframe crossfade (FUN_8001f3bc, 2026-06-15): the current actor
 * whose pose is being computed = the original's DAT_800ac784. re15_skel_compute_pose
 * reads its anim_frac / prev_angles / prev_root to blend the new clip in from the
 * prior rendered pose over `anim_frac` frames. The renderer sets this to the actor
 * pointer (re15_actor_t*) right before each body render, or NULL to disable the
 * blend (POSE QUERIES that must not mutate blend state — e.g. the stair foot-pos
 * query). void* to avoid an re15_actor.h dependency in this header. */
extern void *g_anim_pose_actor;

#endif /* RE15_SKELETON_H */
