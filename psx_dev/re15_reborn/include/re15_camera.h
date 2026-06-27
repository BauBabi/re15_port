/*
 * RE1.5 Rebuilt — Cinematic camera system (Phase 4.5.8.1, 2026-05-19).
 *
 * RE2-faithful per-room camera cuts. Each cut is a static 3D viewpoint
 * (position, look-at target, FOV) — matching RE1.5's RDT BIO15 RID
 * layout 1:1 (32 bytes per cut). A room has N cuts; one is "active"
 * at a time. Cuts switch when:
 *   (a) the player walks across a trigger-zone quad (defined in RVD),
 *   (b) an SCD `Cut_chg` opcode requests a specific cut index,
 *   (c) a scripted cinematic transition (e.g. door-passing) sets one.
 *
 * The view matrix is recomputed on cut change via the RE2 pitch-then-yaw
 * LookAt algorithm (FUN_80076cb0). No roll, no up-vector — gives RE2's
 * signature roll-locked feel. Per-actor render then composes `view ×
 * actor_world` (FUN_8002ce94 equivalent) and uploads to the GTE.
 *
 * Layout matches RE1.5 RIDParser.java (BIO15, 32 bytes/entry).
 */

#ifndef RE15_CAMERA_H
#define RE15_CAMERA_H

#include <stdint.h>

/* One static-camera definition. 32 bytes, little-endian on disk.
 * Field offsets match RE1.5 BIO15 RID and RE2's per-cut record at
 * RE2_Quellcode/FUN_8003432c.c:12-19. */
typedef struct {
    uint16_t flag;          /* +0x00 0=normal, 1=cinematic               */
    uint16_t fov;           /* +0x02 raw fixed-point FOV (e.g. 26684)    */
    int32_t  pos_x;         /* +0x04 camera position in world units      */
    int32_t  pos_y;         /* +0x08                                     */
    int32_t  pos_z;         /* +0x0C                                     */
    int32_t  target_x;      /* +0x10 look-at point in world units        */
    int32_t  target_y;      /* +0x14                                     */
    int32_t  target_z;      /* +0x18                                     */
    uint32_t pri_offset;    /* +0x1C sprite-mask offset (sprite.pri)     */
} re15_camera_cut_t;

/* Resolved view transform — world → camera-space.
 *
 * Q12 fixed-point rotation (same scale as re15_skel_pose_t.rot, so the
 * Q12_MUL helpers in skeleton_common.c can be reused for view×bone
 * compose). Translation is in raw world units (no Q12 scale), matching
 * the convention of MD1 vertex coordinates and bone trans values.
 *
 * Layout: rot[r*3 + c]; rot is "row-major" same as MATRIX.m on PSX.
 *
 * The forward direction of the camera is +Z (look from pos toward
 * target), Y axis matches PSX-screen-Y-down. Roll is always zero. */
typedef struct {
    int32_t rot[9];         /* Q12 view rotation                         */
    int32_t trans[3];       /* view translation (= -rot * cam_pos)       */
    int16_t fov_screen_dist;/* converted from cut.fov, fed to gte_SetGeomScreen */
} re15_camera_view_t;

/* Build view from cut params. Implements the RE2 LookAt builder
 * (FUN_80076cb0): forward = normalize(target - pos), then pitch =
 * -asin(forward.y) around X, yaw = atan2(forward.x, forward.z) around
 * Y, composed as M = R_pitch * R_yaw applied to a column vector.
 *
 * Numerics: forward vector + sqrt computed in float to avoid overflow
 * on large coords (BIO15 positions are int32 PSX world units, often
 * ±30000). The final matrix is converted back to Q12 int32 for renderer
 * upload. Float math executes ONCE per cut change, never per frame.
 *
 * Returns 0 on success, -1 if cut is NULL, -2 if pos == target (degenerate
 * forward vector). */
int re15_camera_build_view(const re15_camera_cut_t *cut,
                            re15_camera_view_t      *view_out);

/* Compose view × bone_world transform.
 *
 * Given a bone's Q12 world rotation + integer world translation (as
 * produced by re15_skel_compute_pose), produces the rotation and
 * translation that takes a model-space vertex of THIS bone's mesh to
 * camera (view) space:
 *
 *   v_view = combined_rot · v_model + combined_trans
 *          = view_rot · (bone_rot · v_model + bone_trans) + view_trans
 *          = (view_rot · bone_rot) · v_model
 *                            + (view_rot · bone_trans + view_trans)
 *
 * Output is Q12 rotation + integer translation, matching the input
 * conventions so the result can flow directly into gte_SetRotMatrix /
 * gte_SetTransMatrix (after Q12→s16 cast) on PSX or into PROJECT_VERT
 * on PC. Called per bone per frame; the math is plain Q12 multiplies,
 * no float, no sqrt. */
void re15_camera_compose_view_bone(const re15_camera_view_t *view,
                                    const int32_t             bone_rot[9],
                                    const int32_t             bone_trans[3],
                                    int32_t                   out_rot[9],
                                    int32_t                   out_trans[3]);

/* Build a camera-yaw RotY matrix R_y(cam) from a forward vector's XZ projection
 * (sin = fwd_x/len, cos = fwd_z/len, Q12). SHARED by the character-shadow code on
 * both ports (integer isqrt — no soft-float). Degenerate (len<1) → identity yaw.
 * Feed out_rot to re15_camera_compose_view_bone as the bone rotation. */
void re15_camera_yaw_matrix(int32_t fwd_x, int32_t fwd_z, int32_t out_rot[9]);

/* Bundled test data: ROOM1100 (extracted from
 * extracted/PSX/STAGE1/room1100/camera.json). 13 cuts in the actual
 * room; we bundle the first 7 (cuts 8..12 are degenerate end-cams
 * pointing straight down — all identical). Used by Phase 4.5.8.3 demo
 * loop to cycle through real RE1.5 angles. */
extern const re15_camera_cut_t re15_camera_room1100_cuts[];
extern const int               re15_camera_room1100_cut_count;

/* -------------------------------------------------------------------------
 * Per-frame camera animator — port of RE1.5's FUN_80015850
 * (RE_15_modified_V2/render_camera_interpolate.c, kamera_interpolation_berechnen).
 *
 * Original RE1.5 runs this animator every render frame between SCD tick
 * and projection. It:
 *  (1) Orbits camera position around the cut record's static target XZ via
 *      yaw angle + distance residual that shrinks per frame
 *      (cam.x = rcos(yaw)*dist + cut.target.x, same for Z).
 *  (2) Soft-tracks the active actor's world XYZ into the animated camera
 *      target via per-axis lerps:
 *        target.x += (actor.x - target.x) / 60     (XZ: 60-frame settle)
 *        target.z += (actor.z - target.z) / 60
 *        target.y += (actor.y - (target.y + 400)) / 20  (Y: 20-frame, +400 head height)
 *      Also lerps lookat XZ at 60-frame rate.
 *  (3) Rebuilds the view matrix from animated cam_pos + animated target.
 *
 * Original engine globals at DAT_800b52** (52 bytes). Mirrored here in a
 * single struct. Mode byte (DAT_800b5250) = 0x40 means "animator active".
 *
 * For our cinematic ROOM1170, the missing piece is soft-target tracking:
 * the static cut 2 record's target (918,-5742,-4230) doesn't aim at
 * Elliot's actual position (-2281,-7200,5769); the original game's
 * animator lerps target toward the active actor so cut 2 framing centers
 * on the speaker. Without this animator we render Elliot at lower-left
 * (109,151) instead of ablauf's center (~130,115). */
typedef struct {
    int32_t cam_pos[3];          /* animated camera position (world)        */
    int32_t cam_target[3];       /* animated camera target  (world)         */
    int32_t lookat_xz[2];        /* secondary lookat XZ (RE1.5 DAT_800b5254/56) */
    int32_t dist_residual;       /* XZ-distance residual (shrinks each tick) */
    int32_t y_dist_residual;     /* Y-distance residual                     */
    int32_t dist_step;           /* XZ-distance step per frame              */
    int32_t y_dist_step;         /* Y-distance step per frame               */
    int16_t yaw_q12;             /* current orbit yaw (0x1000 = 360°)       */
    int16_t yaw_step;            /* yaw step per frame                      */
    uint8_t mode;                /* 0x40 = active                           */
    uint8_t initialized;
    int     focus_slot;          /* which actor slot to soft-track (g_actors[]) */
    re15_camera_cut_t cut_static;/* cached cut record (orbit center, fov)   */
} re15_camera_animator_t;

/* Reset + initialize animator state from a cut record. Seeds animated
 * cam_pos/target = static cut.pos/target, computes initial yaw and
 * distance from cam_pos vs cut_target so the first frame produces the
 * cut's authored framing exactly (no jump-cut on Cut_chg). */
void re15_camera_animator_init(re15_camera_animator_t *anim,
                                const re15_camera_cut_t *cut,
                                int focus_slot);

/* Per-frame tick. Reads focus actor's world XYZ via `focus_x/y/z`,
 * advances animator state by one frame. After this call, anim->cam_pos
 * and anim->cam_target hold the animated positions for view-matrix
 * rebuild. */
void re15_camera_animator_tick(re15_camera_animator_t *anim,
                                int32_t focus_x,
                                int32_t focus_y,
                                int32_t focus_z);

/* Convenience: pack animator's animated cam_pos/target into a transient
 * re15_camera_cut_t so re15_camera_build_view can build the view matrix.
 * fov is taken from the cached static cut record. */
void re15_camera_animator_to_cut(const re15_camera_animator_t *anim,
                                  re15_camera_cut_t *out_cut);

#endif /* RE15_CAMERA_H */
