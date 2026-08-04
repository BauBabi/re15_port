/*
 * RE1.5 Rebuilt — Player walker (Phase 4.5.13 L-FINAL, 2026-05-21).
 *
 * Port of RE1.5's `SpielerPositionNachRotationBewegen` (FUN_800245d8 in
 * PSX.EXE) — the actual cinematic-walker the engine runs every tick:
 *
 *   SVECTOR move = { speed, 0, 0 };
 *   RotMatrixY(yaw + offset, &R);
 *   gte_SetRotMatrix(&R);
 *   gte_ldv0(&move); gte_rtv0();
 *   pos.x += rotated.x;
 *   pos.z += rotated.z;
 *
 * The engine's per-mode dispatcher (PTR_FUN_8009d868[motion_id + state])
 * decides the target yaw + the scalar speed each tick. We collapse that
 * into a single function here:
 *
 *   target_yaw = atan2(dest.z - cur.z, dest.x - cur.x)
 *   slew rot_y toward target_yaw by PLC_YAW_SLEW
 *   speed = per-mode constant (RUN=72, WALK=48, TURN=0)
 *   if walking: rotate (0,0,speed) by rot_y, add to pos.x/pos.z
 *   if arrived: set flag(5, walk_flag_bit) = 1, clear walk_active
 *
 * Y is left alone — separate floor system (Pos_set + FUN_8004fba0 SCA probe
 * for non-flat rooms; sub02's helipad is flat at Y=-7965 from sub14).
 *
 * Forensic basis: re_locomotion_lround_FINAL_2026_05_21.md (memory).
 * Key files cross-referenced:
 *   RE_15_modified_V2/player_move_by_rotation.c (verbatim walker = the
 *                                                player's SCALAR-speed walk,
 *                                                FUN_800245d8 / +0x8C)
 *   RE_15_modified_V2/anim_keyframe_process.c   (the BONE-ROTATION half of the
 *                                                keyframe; NOTE: keyframes ALSO
 *                                                carry root SPEED at bytes 6..11
 *                                                — see re15_actor_apply_root_motion
 *                                                below + emd_common.c
 *                                                re15_emd_get_keyframe_speed)
 *   PSX.EXE 0x80041BE4                          (Plc_dest opcode handler
 *                                                — state-init only, no math)
 *
 * Refuted theories (don't re-introduce):
 *   - K9/I3 "speed_xyz at kf bytes 6..11"      → bytes are UNREAD by engine
 *   - K2 "root pos at kf bytes 0..5"           → those are bone-0 Euler Q12
 *   - K10 "yaw set ONCE at Plc_dest fire"      → handler has no math
 *   - L2 "accel→vel→accumulator chain"         → enemy walker only, not player
 */

#include "re15_actor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_math.h"       /* re15_catan — byte-true BIOS arctan (walker heading) */
#include "re15_scd.h"        /* re15_game_flag_set */
#include "re15_emd.h"
#include "re15_to_re2.h"     /* re15_to_re2_plc_dest_clip (BO Tier-3 A2) */
#include <stdlib.h>          /* abs */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>           /* fprintf for walker diagnostic logging */
#endif

/* Per-mode speed LUT — values from O4 agent's disasm of the real PSX.EXE
 * per-state walker handlers (dispatch table at 0x80073E40):
 *   Handler 0x80030AF0 forward-walk: 0x4B (75) state-0, 0x60 (96) state-1
 *   Handler 0x80030D28 run:          0xC8 (200) state-0, 0x60 (96) state-1
 *   Handler 0x80031080 backward:     0x46 (70), yaw-offset -48
 * SUPERSEDED by the AG-round below: the 0x60 'state-1' value is the yaw-slew RATE, not a
 * steady-state speed — the code uses PLC_STEP_WALK=75 / PLC_STEP_RUN=200, and the walker is a
 * full 3-state align/active FSM (walk_fsm 0/1/2, L177/208/216), not a one-state model.                  */
/* AG-round (2026-05-26): canonical PSX walker speeds per agent E1's
 * ghidra1_V2 disasm of mode handlers 0x80030AF0 (walk) and 0x80030D28 (run):
 *   mode 4 WALK: motion_id=0x4B → speed=75 written to actor+0x8C
 *   mode 5 RUN:  motion_id=0xC8 → speed=200 written to actor+0x8C
 * The 0x60 value was misidentified as steady-state speed — it's actually
 * the yaw-slew rate (a2 arg to FUN_8001aac4 yaw rotator). With both modes
 * set to 96, Leon's run was 2.67× too slow, causing many small visible
 * leg cycles per Plc_dest distance ("viele Schritte"). */
#define PLC_STEP_WALK       75      /* mode 4 — 0x4B per PSX disasm         */
#define PLC_STEP_RUN        200     /* mode 5 — 0xC8 per PSX disasm         */
/* Y-round (2026-05-25): REVERT W-round. Mode 0x09 IS turn-in-place per
 * deep-RE projection math (A10): with PLC_STEP_TURN=96, Elliot walks
 * ALL THE WAY TO (-3461, 4491) = pilot's exact position → screen pixels
 * (148,146) and (150,145), only 2 px apart = visually overlapping.
 * With turn-in-place (0), Elliot stays at (-2261, 5791) after first walk
 * → ~40 pixel separation from pilot at Cut 2 camera = side-by-side as
 * in original ablauf. The "walk-and-turn" interpretation was wrong. */
/* BO-round 2026-05-29 (hack audit): backward-walk modes write 0x46=70, not the
 * old 96 placeholder. PSX LAB_80031080 / LAB_800311f0. */
#define PLC_STEP_MODE7      70      /* 0x46 — PSX LAB_80031080              */
#define PLC_STEP_MODE8      70      /* 0x46 — PSX LAB_800311f0              */
#define PLC_STEP_TURN       0       /* mode 9 — turn-in-place               */

/* Yaw slew + arrival thresholds (Q12: 4096 = 360°). */
/* Turn-init yaw-slew rate = 0x15e (350) — the BYTE-TRUE a2 turn-rate from the 2026-06-09
 * deep-RE of the walk/run handlers 0x80030af0 / 0x80030d28 (fed to FUN_8001aac4/ab9c).
 * Supersedes the earlier tuned 0x60 (and the older 0x80). The USER A/B-tested 0x15e vs 0x60
 * in the sub02 cutscene and saw NO visible difference — the init-slew only fires on a large
 * initial delta, which converges in 1-2 ticks either way.
 * RULE (user 2026-06-09): when a byte-true change is VISUALLY NEUTRAL, KEEP the byte-true
 * value — do NOT fall back to a tuned/eyeballed constant. The walker is otherwise structurally
 * byte-true (atan2/catan heading FUN_8001a6d4, arrival dist<100 WALK/<300 RUN, mode-9
 * rotate-in-place, completion-flag); the "0x40 walker deviation" was largely a phantom of the
 * disproven "handlers write speeds 75/96/200" premise (those are yaw turn-rates, not speeds).
 * Shared by both ports (actor_locomotion.c). State-2 fine-adjust stays 0x30. */
#define PLC_YAW_SLEW_INIT    0x60   /* 96 — state-1 align slew: FUN_8001aac4 a2 = 0x60, byte-verified at
                                     * both callers @0x80030ba4 (walk) and @0x80030df8 (run). The value
                                     * had been 0x15e (350) = the SEPARATE convergence THRESHOLD below
                                     * (FUN_8001ab9c), so the align slewed ~3.6x too fast per tick. */
#define PLC_YAW_SLEW_ACTIVE  0x30   /* ~4.2° per tick, state 2               */
#define PLC_YAW_SLEW_THRESH  0x100  /* |delta| > this → use INIT slew         */
#define PLC_YAW_ARRIVAL      0x20   /* ~2.8° — mode 9 fires when |Δ| ≤ this  */
/* BO-round 2026-05-29 (hack audit): PSX walkers test EUCLIDEAN distance
 * (SquareRoot0(dx²+dz²) < 0x64=100), not a per-axis Manhattan box. The old
 * "80 / matches RE2 0x50" was wrong on both formula and constant. We compare
 * squared distance to avoid the sqrt. PSX LAB_80030c08, ghidra1_V2.txt:125492
 * (slti v0,v0,0x64). */
#define PLC_ARRIVAL_DIST     100    /* 0x64 — Euclidean radius, PSX-canonical */
/* BO-round (Tier-3 A5): PSX 3-state walker FSM constants (LAB_80030AF0). */
#define PLC_CONV_THRESHOLD   0x15e  /* ~30.8° — state-1→2 yaw convergence (FUN_8001ab9c) */
#define PLC_YAW_SLEW_RUN_S2  0x48   /* run state-2 slew (ghidra1:125625) — walk uses 0x30 */
#define PLC_ARRIVAL_RUN      300    /* 0x12c — run arrival radius (LAB_80030e5c) */

/* atan2 in PSX angle units (4096 = 360°). Returns yaw measured CW from +Z (so yaw=0 → +Z forward,
 * matches renderer's mat3_rot_y). BYTE-TRUE to the game's walker heading FUN_8001a6d4(0,0,dx,dz): it
 * builds the bearing from the PsyQ BIOS `catan` (re15_catan, the 12-iter CORDIC @0x800658fc), NOT a
 * float/LUT arctan. The port's `+Z-zero` convention = the game's raw output + 0x400 (verified: game
 * yields 0 for +X, the port wants 0x400). The old port-generated ATAN256 LUT diverged from the game's
 * catan on 302/360 directions by up to 6 Q12 units (~0.53°) — a real steering divergence, now byte-
 * true (the game's own catan rounding, e.g. catan(0)=1 → +X yaw is 0x3ff not 0x400). Audit: the
 * SquareRoot0-class insight — the game's approximate math must be replicated, not smoothed. */
static int16_t atan2_q12(int32_t dz, int32_t dx)
{
    uint32_t a = 0x400;                                 /* FUN_8001a6d4 uVar1 */
    if (dz > 0) a = 0xc00;                              /* if (0 < Δz) */
    if (dx != 0) {                                      /* iVar4 = Δx != 0 */
        int32_t ratio = (int32_t)(((int64_t)dz * 0x1000) / dx);  /* catan arg = (Δz<<12)/Δx, trunc */
        int32_t at    = (int16_t)re15_catan(ratio);    /* (short)iVar2 */
        int32_t q     = (dx < 0) ? 0x800 : 0;
        a = (uint32_t)((q - at) & 0xfff);
    }
    return (int16_t)((a + 0x400) & 0xfff);              /* + the port's +Z-zero convention offset */
}

/* Public wrapper: yaw (Q12, 0 = +Z) from a world XZ delta. The stair traversal
 * uses `re15_atan2_q12(dz,dx) - 1024` to face the player toward the destination
 * (mesh convention 0 = +X), same as this file's dest-seeking walker. */
int16_t re15_atan2_q12(int32_t dz, int32_t dx) { return atan2_q12(dz, dx); }

/* Shortest-path delta on circular Q12 axis. Result in (-2048, +2048]. */
static int16_t yaw_delta(int16_t cur, int16_t target)
{
    int32_t d = ((int32_t)target - (int32_t)cur) & 0x0FFF;
    if (d > 2048) d -= 4096;
    return (int16_t)d;
}

/* Map walk_mode to per-tick forward speed (PSX units). */
static int16_t mode_to_speed(uint8_t mode)
{
    switch (mode) {
    case 0x04: return PLC_STEP_WALK;
    case 0x05: return PLC_STEP_RUN;
    case 0x07: return PLC_STEP_MODE7;
    case 0x08: return PLC_STEP_MODE8;
    case 0x09: return PLC_STEP_TURN;
    default:   return 0;
    }
}

void re15_actor_step_walk(re15_actor_t *a)
{
    if (!a->walk_active) return;

    /* BO-round (Tier-3 A5): PSX 3-state walker FSM (LAB_80030AF0, DAT_800aca5a).
     *   state 0 = init (set by op_plc_dest) → state 1 this same tick
     *   state 1 = ALIGN: slew yaw 0x60/tick, NO advance; within ~31° → state 2
     *   state 2 = ACTIVE: slew 0x30 (walk) / 0x48 (run) + advance at mode speed
     * Replaces the old fixed walk_pad=2 freeze with the real variable-length
     * align phase (0–~22 ticks, scaled by the initial yaw error) — the missing
     * delay behind intro #6's residual cut-timing drift and Elliot's stand→walk. */
    if (a->walk_fsm == 0) {
        a->walk_fsm = 1;   /* fall through to state 1 this tick (no freeze) */
    }

    /* Compute target yaw from current pos to dest. */
    int32_t dx = (int32_t)a->walk_dest_x - a->x;
    int32_t dz = (int32_t)a->walk_dest_z - a->z;
    /* AF-round (2026-05-26): subtract 1024 (90° Q12) to align with PSX
     * convention where rot_y=0 = mesh facing +X. Our atan2_q12 returns
     * Q12 with 0=+Z, 1024=+X; original PSX uses 0=+X, 1024=+Z (per agent
     * D3 disasm of FUN_8001a6d4). Subtracting 1024 rotates our result
     * to match PSX/mesh forward direction (+X = head/face direction
     * per agent D5 OBJ vertex analysis). Without this bias, mesh face
     * was perpendicular (90° off) from walk direction. */
    int16_t target = (int16_t)((atan2_q12(dz, dx) - 1024) & 0x0FFF);

    /* T-round (2026-05-25): SNAP REMOVED. Original PSX engine FUN_8001a8f8
     * slews yaw gradually toward target every tick — there is NO snap. Our
     * earlier snap was overwriting Elliot's Sce_em_set spawn yaw (3844 ≈
     * 338° facing +Z) with the atan2 dest-target (~2521 ≈ 221° facing SW),
     * making him face AWAY from pilot during cut 2 dialog. Per
     * RE_15_modified_V2/player_move_by_rotation.c the walker just reads
     * rot_y at +0x6A — never writes it. The per-mode handler reads
     * dest+pos via FUN_8001a6d4 atan2, then FUN_8001a8f8 slews actor.rot_y
     * by ±slew_step (typically ±0x40) per tick toward the target.
     *
     * For RE2-faithful cinematic, Elliot's yaw stays at his spawn value
     * and gradually slews while walking toward dest. */
    int16_t delta = yaw_delta(a->rot_y, target);
    int16_t abs_delta = (delta < 0) ? -delta : delta;

    if (a->walk_fsm == 1) {
        /* STATE 1 — ALIGN: slew yaw at 0x60/tick, NO position advance. When the
         * heading is within ~31° (0x15e) of the target → enter state 2 (active).
         * PSX LAB_80030b70 + FUN_8001ab9c convergence gate. */
        int16_t s1 = (delta >  PLC_YAW_SLEW_INIT) ?  PLC_YAW_SLEW_INIT
                   : (delta < -PLC_YAW_SLEW_INIT) ? -PLC_YAW_SLEW_INIT :  delta;
        a->rot_y = (int16_t)(((int32_t)a->rot_y + s1) & 0x0FFF);
        if (abs_delta <= PLC_CONV_THRESHOLD) {
            a->walk_fsm = 2;
            /* A2: the walk/run EDD clip starts ONLY now (active stepping) — the
             * actor STOOD through the align phase. PSX Plc_dest sets no motion;
             * the walker does, here. Matches Elliot stand 217020→walk 217022.
             * NPC-Slots: Clip 5 der ENTITY-EIGENEN Bank (NPC-Walk-Sub-INIT
             * @0x800511dc `+0x94 = 5`; Kanal +0x170/+0x174) — NICHT die Player-
             * W01-Sentinels 100/105, die auf einer NPC-Bank falsche Clips
             * indizieren (marvin_10d0.md D5, CONFIRMED: Marvin lief mit mo=105).
             * ⚠ ELLIOT-PORT-AUSNAHME (Nutzer-Regression 2026-08-03, "Elliot
             * gleitet zum Heli"): der Port rendert Elliot aus ELLIOT.PLD, dessen
             * EDD eine ANDERE Cliptabelle traegt als die EM047-EMD-Bank des
             * Originals — Clip 5 ist dort KEIN Walk. Sein verifizierter Pfad
             * (Sentinel 105/100 -> W01-Walk via anim_select) bleibt, bis die
             * EM047-Bank-Frage gemessen ist (Familie marvin_10d0.md O3). */
            if (a != &g_actors[RE15_ACTOR_SLOT_PLAYER] && a->type != 0x47) {
                re15_actor_set_motion(a, 5);
            } else {
                int clip = re15_to_re2_plc_dest_clip((int)a->walk_mode, /*rbj=*/1);
                if (clip >= 0) re15_actor_set_motion(a, (int16_t)clip);
            }
        }
        return;   /* no position advance during the align state */
    }

    /* STATE 2 — ACTIVE: the actor is stepping, so the walk/run clip must ANIMATE. Byte-true to the WALK
     * handler LAB_80030af0, whose state-2 branch advances the clip each frame via the anim processor
     * FUN_8001f3bc (it only HOLDS/wraps-to-0 on the ARRIVAL branch @0x80030cc4). Clear the port's
     * arrival-hold flag here so a CONSECUTIVE Plc_dest with the SAME clip re-animates: a mode-9 TURN
     * arrives and sets anim_freeze=1 with motion 105, then the following mode-4 WALK re-uses clip 105 so
     * re15_actor_set_motion (clears anim_freeze only on a motion CHANGE) leaves the stale freeze set ->
     * the walk clip stays pinned at frame 0 = "Leon floats to Irons". The arrival branch below re-sets it
     * once THIS walk arrives. (ROOM1150 sub08 intro; STOP-GATE reproduce: [walk] frz=1 while wact=1.) */
    a->anim_freeze = 0;

    /* finer slew (0x48 run / 0x30 walk) + advance below. Mode-9 (turn-in-place) is a
     * special case: its handler @0x80031360 slews a CONSTANT 0x60 with NO state-2 slow-down (@0x80031440
     * `ori a2,0x60`), so keep 0x60 here instead of the walk 0x30. (audit wf_4e8af27f) */
    int16_t slew_s2 = (a->walk_mode == 0x05) ? PLC_YAW_SLEW_RUN_S2
                    : (a->walk_mode == 0x09) ? PLC_YAW_SLEW_INIT
                                             : PLC_YAW_SLEW_ACTIVE;
    int16_t step  = (delta >  slew_s2) ?  slew_s2
                  : (delta < -slew_s2) ? -slew_s2
                  :  delta;
    a->rot_y = (int16_t)(((int32_t)a->rot_y + step) & 0x0FFF);

    /* Mode-9 arrival cone = 0x60 (@0x800313d4 `ori a2,0x60`), NOT the port-invented 0x20 (which made the
     * scripted turn-in-place fire ~3x too tightly aligned). (audit wf_4e8af27f) */
    int16_t arrival_cone = (a->walk_mode == 0x09) ? 0x60 : PLC_YAW_ARRIVAL;
    int yaw_aligned = (delta >= -arrival_cone && delta <= arrival_cone);

    /* Translate forward at per-mode speed EVERY tick — the real engine's
     * walker (FUN_800245d8) doesn't gate translation on yaw alignment.
     * Leon steps in his current facing each frame, with yaw concurrently
     * slewing toward target. This gives a smooth arc into the dest line
     * rather than a visible rotate-then-walk staircase.                 */
    int16_t speed = mode_to_speed(a->walk_mode);
    if (speed) {
        /* AF-round (2026-05-26): rotate (speed, 0, 0) by Ry(rot_y) to match
         * original PSX walker FUN_800245d8 which moves vector (speed,0,0).
         * Mesh forward is +X (head/face at +X per agent D5 OBJ analysis).
         * Walker now walks in mesh's facing direction.
         *   | c 0  s |   | speed |   | c*speed |
         *   | 0 1  0 | * |   0   | = |    0    |
         *   |-s 0  c |   |   0   |   |-s*speed |                          */
        int32_t c = re15_cos_q12(a->rot_y);
        int32_t s = re15_sin_q12(a->rot_y);
        a->x += (int32_t)((c * (int32_t)speed) >> 12);
        a->z -= (int32_t)((s * (int32_t)speed) >> 12);
    }

    /* [SEIT marvin_glide_end.md Fix #1 (2026-08-04) laufen NPC-Plc_dest-Walks (0x40-0x4d ausser
     * Elliot 0x47) byte-true in der State-4-Sub-VM (re15_npc_sub_walk) — die NPC-Zweige hier
     * (motion=5-Set, Clip-Advance, Arrival-Freeze) erreichen nur noch Elliot/Player.] */
    /* NPC-WALK-CLIP-ADVANCE (Nutzer-Regression 2026-08-03: "Marvin gleitet nur"): der
     * Original-Walk-Sub tickt anim_set JEDEN Frame (jal f314 im Walk-Body @0x8005134c-Region)
     * — im Port ist waehrend eines Plc_dest-Walks aber NIEMAND der Advancer: der NPC-Tick
     * yieldet auf walk_active (enemy_ai_common), der globale Advancer ueberspringt
     * Self-Advancing-Typen (0x40) bzw. State-4-NPCs. Also advanced der Walker hier selbst:
     * Wrap an der Laenge derselben Bank, aus der der Renderer posiert (re15_actor_clip_len =
     * die eigene EM-Bank; EM040 Clip 5 = 20f Gait). Elliot (0x47) behaelt seinen alten
     * Advance-Pfad (kein State 4, globaler Advancer). */
    if (a != &g_actors[RE15_ACTOR_SLOT_PLAYER] && a->type != 0x47 && a->motion == 5) {
        extern int re15_actor_clip_len(const re15_actor_t *a);   /* enemy_ai_common.c */
        int wfc = re15_actor_clip_len(a);
        if (wfc > 1) {
            a->anim_frame = (uint16_t)(((int)a->anim_frame + 1) % wfc);
            if (a->anim_frac > 0) a->anim_frac--;
        }
    }

    /* Arrival test: mode 0x09 is turn-in-place — arrives when yaw aligned.
     * Other modes arrive when within distance threshold. */
    int arrived;
    if (a->walk_mode == 0x09) {
        arrived = yaw_aligned;
    } else {
        int32_t new_dx = (int32_t)a->walk_dest_x - a->x;
        int32_t new_dz = (int32_t)a->walk_dest_z - a->z;
        /* Euclidean: dx²+dz² < r²  (PSX SquareRoot0). BO Tier-3 A5: run uses a
         * larger radius (0x12c=300, LAB_80030e5c) than walk (0x64=100). */
        int32_t dist2 = new_dx * new_dx + new_dz * new_dz;
        int32_t arr_r = (a->walk_mode == 0x05) ? PLC_ARRIVAL_RUN : PLC_ARRIVAL_DIST;
        arrived = (dist2 < arr_r * arr_r);
    }

    if (arrived) {
        re15_game_flag_set(5, a->walk_flag_bit, 1);
        a->walk_active = 0;
        /* 2026-06-17 BYTE-TRUE (disasm-verified, WALK handler LAB_80030af0 arrival branch
         * 0x80030cc4-0x80030d11): on Plc_dest arrival the original sets the SCD arrival flag
         * + flips the walker's OWN FSM state byte — it writes NOTHING to the actor's motion
         * (+0x94), frame (+0x95) or anim-flags (+0x1c4). It HOLDS the walk clip; the anim
         * processor (FUN_8001f3bc) wraps the frame counter to 0 at clip-end and freezes there.
         * So we do NOT reset motion here (the old motion=0/200 reset was port-invented). The
         * walk/run sentinel (100/105) is held; re15_compute_actor_kf freezes it at frame 0
         * (= the arms-down walk-start pose, W01 clip5 frame0 == PL00 clip0 frame0) once
         * walk_active clears — exactly what the original shows before the next Plc_motion. */
        if (a->motion == 100 || a->motion == 105 ||
            (a->motion == 5 && a != &g_actors[RE15_ACTOR_SLOT_PLAYER])) {
            /* Freeze the held walk/run clip at frame 0 (FUN_8001f3bc end-of-clip wrap+freeze).
             * anim_freeze is read by re15_compute_actor_kf; the next Plc_motion's set_motion
             * clears it. The pad-walk (motion 105, walk_active 0) NEVER sets this, so gameplay
             * walking still loops normally — only a scripted-walk ARRIVAL freezes. (NPC walks
             * hold their clip-5 gait the same way until the next executor command.) */
            a->anim_freeze = 1;
        }
    }
}

/* Drive all active walkers in the actor pool. Called once per SCD tick. */
void re15_actor_step_all_walkers(void)
{
    for (int i = 0; i < RE15_ACTOR_MAX; i++) {
        if (g_actors[i].active && g_actors[i].walk_active) {
            re15_actor_step_walk(&g_actors[i]);
        }
    }
}

/* Phase 4.5.13 L-FINAL → CORRECTED 2026-06-29 (Phase 8.10 forward-walk RE).
 *
 * This stub stays a NO-OP, but its old justification ("EMR keyframes contain
 * ONLY bone rotations ... bytes 6..11 are UNREAD ... there is no root motion
 * data in keyframes") is WRONG and is corrected here so it does not mislead.
 *
 * The EMR keyframe is "12B position+speed + variable-length packed angles"
 * (RE15_KNOWLEDGE.md §5.2 L461): bytes 0..5 the renderer reads as the bone-0
 * pose, and bytes 6..11 ARE the root SPEED_xyz (s16 LE) = the per-frame root
 * translation. This codebase already reads them: re15_emd_get_keyframe_speed
 * (emd_common.c) returns +6/+8/+10. The ORIGINAL applies them two ways:
 *   - player CINEMATIC clips (e.g. ROOM1170 RBJ walk-to-helicopter): FUN_8001F3BC
 *     GTE-rotates speed_xyz by the actor yaw and ADDS to pos each active tick.
 *   - ENEMY locomotion (the zombie forward-walk, +0x5=5/6): FUN_8001AD68 →
 *     FUN_8001AE38 reads the SAME keyframe speed (dx=+6, dz=+10), RotMatrix by
 *     entity rot_y(+0x6a), ApplyMatrix, then entity.x/z = base(+0xa0/+0xa2)+disp.
 * So root motion IS in the keyframes; the reader exists.
 *
 * Why this stays a no-op anyway: the PLAYER GAMEPLAY walk/run legitimately does
 * NOT use keyframe root motion — the player command FSM steps via the SCALAR
 * speed field +0x8C (FUN_800245d8, the `walk_active` path above), a separate,
 * byte-true mechanism (FUN_8001AD68 has zero EXE callers — it is overlay/enemy
 * only). The deferred consumer is the ENEMY forward-walk: it needs the zombie
 * EDD/EMR loaded + re15_emd_get_keyframe_speed wired into the +0x5=5/6 walk
 * animate (FUN_80102BD8). That, plus the still-unresolved m0 +0x5=6 trigger
 * (the variant-layer puzzle, see HANDOVER §8.10), is what blocks the byte-true
 * forward-walk port — not the keyframe data, which is present + readable here. */
void re15_actor_apply_root_motion(re15_actor_t *a,
                                  const re15_emd_skeleton_t *skel,
                                  const re15_emd_animation_t *anim)
{
    (void)a;
    (void)skel;
    (void)anim;
}
