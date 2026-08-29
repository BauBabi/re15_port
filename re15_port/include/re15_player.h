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
#include "re15_emd.h"   /* PL00-Baenke fuer den Schiebe-Substate (Wurzelbewegung) */

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

/* Shared per-frame NPC animation advance — runs in EVERY game_step branch (unlike the player anim,
 * which is frozen when player_tick is skipped). Keeps zombies animating while the player is grabbed/
 * dead and lets a dying zombie's death clip complete to CORPSE. Byte-true: entity anim advances in
 * the per-type handler (FUN_8001a50c -> @0x80072bac[type]), independent of the player command FSM. */
void re15_actors_anim_advance(void);

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

/* ---- KISTEN SCHIEBEN — Spieler-Substate 8 ---------------------------------------------------
 * re15_player_push_substate() ist der Handshake, den der Objekt-Pass FUN_8002bd44 abfragt:
 * `DAT_800aca59 == 8` @0x8002bfe0/e4. Der Substate entsteht im Sub-ENTRY-1-Handler
 * (Vorwaertsgehen) aus dem Kontaktbit: @0x800323c0 liest DAT_800aca3c & 0x2000, @0x8003247c
 * schreibt 32-bittig `0x801` nach DAT_800aca58 (= Zustand 1 / Substate 8 / +0x06 = 0).
 * re15_player_push_phase() liefert DAT_800aca5a (0..7) bzw. -1 = nicht am Schieben. */
int  re15_player_push_substate(void);
int  re15_player_push_phase(void);
void re15_player_push_reset(void);
/* ---- Plc_dest-Modi 7/8 = RUECKWAERTSGEHEN aus der COMMON-Bank -------------------------------
 * Motion-Sentinel (Port-Repraesentation): PL00.EDD Clip 0. Aufgeloest in anim_select_common.c,
 * gesetzt von re15_to_re2_plc_dest_clip(7|8).
 *
 * BELEG (selbst disassembliert, info/Re1.5/PSX.EXE — Spieler-Mode-Tabelle @0x80073e30):
 *   Mode 7 = LAB_80031080, Mode 8 = LAB_800311f0, beide identisch aufgebaut:
 *     @0x800310bc / @0x8003122c  `sb zero,-13592(at)` -> +0x94 (0x800acae8) = CLIP 0
 *     @0x800310cc / @0x8003123c  `sb v0,-13597(at)`   -> +0x8f = 7
 *     @0x800310a8 / @0x80031218  `sh v0,-13600(at)`   -> +0x8c = 0x46 = 70 (einmalig im Init)
 *     @0x800310ec / @0x8003125c  `jal 0x800245d8` mit `a0 = 0x800` = 180-Grad-Translation
 *     @0x800310d0-e4/@0x80031240-54 Yaw-Slew FUN_8001aac4 mit RATE -48
 *   und der Clip kommt aus dem PL00-PAAR, nicht aus der Waffenbank:
 *     @0x80031134 `lw a0,0x800acad8` / @0x8003113c `lw a1,0x800acbc0` / @0x80031140 jal f314
 *     @0x800312a4 `lw a0,0x800acad8` / @0x800312ac `lw a1,0x800acbc0` / @0x800312b0 jal f314
 *   Die Modi 4/5/9 laden dagegen das PLW-Paar:
 *     @0x80030bec/f4 (4), @0x80030ec0/c8 (5), @0x80031488/90 (9) = 0x800acbc4 / 0x800acbc8
 *   Jedes Paar hat game-weit GENAU EINEN Schreiber (eigener EXE-Scan):
 *     0x800acbc0 @0x8003154c + 0x800acad8 @0x80031578 = PLD-Directory = PL00.EDD / PL00.EMR
 *     0x800acbc8 @0x80036be4 + 0x800acbc4 @0x80036c04 = PLW-Archiv (liest die Waffe 0x800aca5d
 *                                                       @0x80036bf8) = PL00W01.EDD / .EMR
 *   Dieselbe Trennung fuehrt anim_select_common.c bereits fuer Klettern und Kisten-Schieben.
 *
 * WARUM ES AUFFAELLT (Nutzer: "zum Schluss rennt Leon komisch, fast auf der Stelle"): der Port
 * bildete 7/8 auf den RUN-Sentinel 100 = PL00W01 Clip 0 ab. Aus den Assetbytes gemessen:
 * PL00.EDD Clip 0 = 34 Bilder (Wurzel-px/pz ueber alle 34 Bilder = 0 -> reiner In-Place-Zyklus,
 * die Translation liefert der Skalar +0x8c), PL00W01.EDD Clip 0 = 22 Bilder. Bei identischen
 * 70 Einheiten/Bild Bodengeschwindigkeit taktet der 22-Bild-Renn-Zyklus 34/22 = 1,55x zu
 * schnell = "Beine rennen, Koerper kriecht". */
#define RE15_PLAYER_MOTION_BACK_PL00 236

/* PL00-Baenke fuer den Schiebe-Substate (Cliplaengen 0x11/0x12 + die Wurzel-Translation der
 * EMR-Keyframes, aus der das Original in FUN_800369f8 Modus 0 den Schiebe-Schritt zieht). */
void re15_player_set_pl00_banks(const re15_emd_skeleton_t *skel,
                                const re15_emd_animation_t *anim);
/* Frame-Anzahl eines PL00-Clips (0 = Bank fehlt/ausser Bereich) — Gorilla-Wurf-
 * Aufsteher (Hook 0x8011c118 P3-P6: Leons Clips 0x10/0xb, enemy_ai_common.c). */
int re15_player_pl00_clip_frames(int clip);

#endif /* RE15_PLAYER_H */
