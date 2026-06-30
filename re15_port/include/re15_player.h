/*
 * RE1.5 Rebuilt — Player controller (Phase 4.4.5, 2026-05-19).
 *
 * Reads pad bits + active camera view → updates g_game.player_x/z
 * with camera-relative D-Pad walk. Cross-platform (shared between PSX
 * and PC via SHARED_SRC).
 *
 * D-Pad UP/DOWN moves along the camera's XZ forward direction; LEFT/
 * RIGHT moves along the camera's XZ right axis. This matches modern
 * "third-person camera-relative" controls (UP = "away from camera"),
 * not classic RE tank controls — tank controls can come later as a
 * style toggle once we have a yaw-driven Leon model.
 *
 * Motion state: 0 = idle, 1 = walk. Set automatically from |velocity|.
 * SCD scripts can override via Plc_motion if they want a specific clip.
 *
 * Yaw: not updated here yet. Leon retains the SCD-set yaw_4096. Phase
 * 4.4.5.1 will add direction-from-velocity yaw (needs PSX-side atan2
 * in fixed-point — simple 8-way LUT or PSn00bSDK's ratan2).
 */

#ifndef RE15_PLAYER_H
#define RE15_PLAYER_H

#include <stdint.h>
#include "re15_camera.h"

/* Pad bit masks (match PSn00bSDK PAD_* convention). */
#define RE15_PAD_BIT_SELECT   0x0001
#define RE15_PAD_BIT_START    0x0008
#define RE15_PAD_BIT_UP       0x0010
#define RE15_PAD_BIT_RIGHT    0x0020
#define RE15_PAD_BIT_DOWN     0x0040
#define RE15_PAD_BIT_LEFT     0x0080
#define RE15_PAD_BIT_L2       0x0100
#define RE15_PAD_BIT_R2       0x0200
#define RE15_PAD_BIT_L1       0x0400
#define RE15_PAD_BIT_R1       0x0800
#define RE15_PAD_BIT_TRIANGLE 0x1000
#define RE15_PAD_BIT_CIRCLE   0x2000
#define RE15_PAD_BIT_CROSS    0x4000
#define RE15_PAD_BIT_SQUARE   0x8000

/* Locomotion clip sentinel for the WALK_FORWARD gait (PL00W01 clip 5). Defined
 * locally in player_common.c; published here so the stair-traversal module can
 * force the same walk clip while auto-walking Leon down/up the steps (the
 * original plays the ordinary walk while the floor solver lowers Y — there is
 * NO dedicated stair clip in PL00.edd, agent-verified 2026-06-07). */
#define RE15_PLAYER_MOTION_WALK 105

/* Stair-traversal clips (PL00.edd base bank), byte-true from the original stair
 * FSM states: FUN_80035538 sets player+0x94 (=DAT_800acae8) = 0xd (clip 13, the
 * step-DOWN cycle, 10f); FUN_80036718 sets = 7 (clip 7, the step-UP cycle, 113f).
 * The floor solver moves the body while these animate the legs (no root motion),
 * which is why a prior "no stair clip" read was wrong. Mapped to the pl00 bank in
 * anim_select_common.c. */
#define RE15_PLAYER_MOTION_STAIR_DOWN 220
#define RE15_PLAYER_MOTION_STAIR_UP   221

/* STANDBY motion the stair finalize hands back to (byte-true LAB_80038e50 sets
 * DAT_800acae8=2, the neutral standby clip). Matches the player idle FSM's
 * neutral hold (RE15_MOTION_IDLE in player_common.c) so the hand-off to
 * re15_player_tick on the next frame is seamless. */
#define RE15_PLAYER_MOTION_IDLE       200

/* Update g_game.player_x/z from pad input + camera basis. Call once per
 * frame from main, AFTER re15_input_tick and AFTER the active camera
 * view is rebuilt for the current cut. */
void re15_player_tick(const re15_camera_view_t *view, uint16_t pad_bits);

/* Aim sub-phase query for game_step (Phase 8.16): returns 1 ONLY when the weapon-raise (PL00.EDD
 * clip 17) has played out and the player is in the held AIM-READY pose (action-8 state 5) — the
 * byte-true gate for the discharge (the original fires only in state 5, never mid-raise). Returns
 * 0 while raising or not aiming. */
int re15_player_aim_ready(void);

/* Phase 4.4.5.2 debug helper: bump g_game.player_motion by `delta`
 * (typically +1 or -1), wrapped to [0, clip_count). Used to find which
 * EDD clip is forward-walk for the test asset. Call from main on
 * edge-trigger of a debug key/button (NOT on hold). */
void re15_player_cycle_motion(int delta, int clip_count);

#endif /* RE15_PLAYER_H */
