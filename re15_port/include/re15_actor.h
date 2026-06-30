/*
 * RE1.5 Rebuilt — Actors (Phase 4.4.8, 2026-05-19).
 *
 * Per-actor state for NPCs / enemies (and eventually the player when
 * we unify game_state into the actor pool). RE2 has a 0xAC-byte actor
 * struct with 44 properties indexed by ID via PTR_LAB_800a4030 — see
 * RE2_Quellcode/FUN_80055cb0.c (set), FUN_80055f50.c (get).
 *
 * We port a subset enough for SCD `Member_set` / `Member_cmp` /
 * `Sce_em_set` to drive the spawn-and-position behavior used by most
 * RE1.5 room scripts. The remaining ~36 properties can be added
 * incrementally as scripts reference them.
 *
 * Member-id semantics match RE2 (see actor_common.c property table).
 */

#ifndef RE15_ACTOR_H
#define RE15_ACTOR_H

#include <stdint.h>

#define RE15_ACTOR_MAX        16
#define RE15_ACTOR_SLOT_PLAYER 0   /* Slot 0 is always the player (RE2-pure) */

/* T-round (2026-05-25): RE2 actor flag bits at +0x1c4 — control anim FSM. */
#define RE15_ANIM_FLAG_LOOP          0x04   /* loop on clip end (else freeze) */
#define RE15_ANIM_FLAG_SECOND_PASS   0x08   /* enable second pass */
#define RE15_ANIM_FLAG_CLEAR_INTERP  0x40   /* clear interp counter */
#define RE15_ANIM_FLAG_REVERSE       0x80   /* reverse playback */

typedef struct {
    uint8_t  active;       /* 0 = slot free, 1 = active */
    uint8_t  type;         /* enemy / NPC type (slot 0 = player, type=0) */
    /* RE2 base fields (per FUN_80055cb0.c Member_set dispatch) ===========
     * Member ID → field mapping verified F1 agent 2026-05-21. RE1.5 SCD
     * scripts use the same Member IDs as RE2 because both engines share
     * lineage.                                                           */
    /* RE1.5 SCD Member fields — byte-true id→offset from the engine's own member
     * cores FUN_8004116c (set) / FUN_80041358 (get); full table in actor_common.c.
     * (The earlier "RE2 id" labels were the RE2-translation that wrote Member id12
     * → Leon.y = "player under the floor"; corrected to the direct RE1.5 ids [#11].) */
    uint8_t  state;        /* RE1.5 Member id 8  → +0x04 */
    uint8_t  flags;        /* RE1.5 Member id 6  → +0x00 (subset: low byte of the word) */
    uint8_t  sub_state_1;  /* RE1.5 Member id 9  → +0x05 */
    uint8_t  sub_state_2;  /* RE1.5 Member id 10 → +0x06 */
    uint8_t  sub_state_3;  /* RE1.5 Member id 11 → +0x07 */
    uint8_t  sub_state_4;  /* RE1.5 Member id 13 → +0x08 */
    int32_t  member_0c;    /* RE1.5 Member id 7  → +0x0c (word) — stored, no consumer yet */
    uint8_t  grid_id;      /* RE1.5 Member id 12 → +0x09 — grid/cell id, the MOST frequent Member_set (117 sites) */
    uint8_t  member_0a;    /* RE1.5 Member id 14 → +0x0a — stored, no consumer yet */
    uint8_t  member_0b;    /* RE1.5 Member id 15 → +0x0b — stored, no consumer yet */
    uint8_t  floor;        /* RE1.5 Member id 18 → +0x82 (floor band) */
    int32_t  x;            /* RE1.5 Member id 0 → +0x34 (world X)          */
    int32_t  y;            /* RE1.5 Member id 1 → +0x38 (world Y)          */
    int32_t  z;            /* RE1.5 Member id 2 → +0x3c (world Z)          */
    int16_t  rot_x;        /* RE1.5 Member id 3 → +0x68 (rot_x)           */
    int16_t  rot_y;        /* RE1.5 Member id 4 → +0x6a (rot_y, 4096=360°)*/
    int16_t  rot_z;        /* RE1.5 Member id 5 → +0x6c (rot_z)           */
    int16_t  hp;            /* RE1.5 Member id 19 → +0x1ba (hp; lh signed)  */
    /* Combat hit-reaction state — byte-true player branch of the damage
     * resolver FUN_80012d60 (ghidra1_V2.txt:77656-77714). hit_react (RE2 +0x93):
     * bit0x1 = "already hit this attack" guard — SET @80012eec, CHECKED @80012e30
     * → exactly one damage application per attack window (cleared between attacks
     * by the deferred enemy-attack FSM, see re15_player_clear_hit_guard). bit0x2 /
     * bit0x80 are the enemy-side collision flags (unused on the player). status_flags
     * (RE2 +0x98, DAT_800acaec, u16): bit0x2 = bleed/poison, rolled on a type<2 hit
     * via 2×RNG&1 @80012ea4. See re15_damage.c. */
    uint8_t  hit_react;    /* RE2 +0x93 — per-attack hit guard + enemy collision bits */
    uint16_t status_flags; /* RE2/RE1.5 +0x98 (Member id 17, lhu) — bit0x2 = bleed/poison */
    /* Attack-hitbox dimensions — FUN_8002b5d0 reads these via target+0x78 (the
     * dim struct: radius_min@+6, height@+8, radius_max@+10) and target+0x7c (the
     * local centre offset). radius_min==radius_max → circular hitbox; else angular
     * sector (DEFERRED). hit_height = Y half-extent. SOURCE (RE'd 2026-06-29):
     * enemies = a per-type table (FUN_80019700 @80019894 sets +0x78 = type_table+0x8,
     * +0x7c @800198b0), overlay-patched → values read byte-true from room savestates;
     * player = a fixed EXE struct @0x80073e94 (radius 450/height 1530, VERIFIED in
     * PSX.EXE @0x64694). Wired by re15_player_apply_hitbox / re15_enemy_apply_hitbox
     * (re15_damage.c); 0 = no hitbox. All known instances are circular (radius_min==max). */
    uint16_t hit_radius_min, hit_radius_max, hit_height;
    int16_t  hit_offset_x, hit_offset_y, hit_offset_z;
    /* Forward attack-point (attacker +0x28/+0x2a/+0x2c) — the lunge's reach point at
     * which the attack trigger FUN_80017fa4 resolves a radius-500 hitbox. Precomputed
     * by the (deferred) lunge-movement AI; read by re15_enemy_attack. 0 until wired. */
    int16_t  atk_pt_x, atk_pt_y, atk_pt_z;
    /* Lunge attack-action active-frame counter (the work-struct +0xe; LAB_80017eb0 sets
     * it to 0x20 = 32). While > 0 the lunge action fires the hitbox each frame (re15_enemy_
     * lunge_tick -> re15_enemy_attack). 0 = not lunging. See re15_damage.c. */
    uint8_t  lunge_frames;
    /* ===== Enemy-AI state (byte-true, STAGE1 zombie handler FUN_8011d6d4) ==========
     * The AI is a nested FSM dispatched off three entity bytes the port maps onto the
     * existing actor fields (no new bytes for these — they ARE state/grid_id/sub_state_1):
     *   main state  entity+0x4  = actor.state       -> PTR_FUN_801217a0[state]
     *   sub index   entity+0x9  = actor.grid_id      -> PTR_FUN_801217b4[grid_id & 0xf]
     *               (for enemies grid_id ALSO packs flags: bit0x20 = skip-tick this frame,
     *                bit0x40 = stationary spawn — set by the room1140 briefing zombies)
     *   anim phase  entity+0x5  = actor.sub_state_1  -> the +0x5 logic/anim leaves
     * The transitions write a 32-bit word at +0x4 (e.g. 0x701 = state 1 / sub_state_1 7).
     * The fields below are the AI work-area the decision handlers read/write that DON'T
     * already exist above; offsets cite the RE1.5 entity struct (see re15_enemy_ai.c).   */
    uint32_t ai_dist;       /* +0x1d0: cached player distance SquareRoot0(ΔX²+ΔZ²) (tick) */
    int16_t  ai_timer;      /* +0x9c : search decision countdown (FUN_80101b64; 0 -> 0x101) */
    uint16_t ai_flags;      /* +0x1d8: bit0x10 = "approach permitted" gate (decision block)  */
    uint8_t  ai_contact;    /* +0x90 : contact/collision bits (FUN_80102058, low byte)        */
    /* AI params FUN_8011d84c writes (PSX: into the model pool entity[0x62]+0x5fx). The
     * port has no model pool, so they live here as named actor fields (faithful-line:
     * map the data representation onto the port, don't rebuild the GTE/model-pool). The
     * attack cone 0x2c8 / wide cone 0x5f4 the decision handlers use are LITERALS in the
     * disasm, so these are init-faithful state rather than read by the decision path.   */
    int16_t  ai_arc;        /* +0x5fc = 0x2c8 (attack front arc) */
    int16_t  ai_p5f8;       /* +0x5f8 = 0x40                     */
    int16_t  ai_p5fa;       /* +0x5fa = 0x30                     */
    int16_t  ai_p5fe;       /* +0x5fe = 0x138                    */
    /* AI target point (+0x1dc/+0x1de) — the XZ the search leaf (FUN_8004f3a4, +0x5=1)
     * arc-tests against (a "where I'm heading / last seen the player" point). Written by
     * a movement leaf (deferred, model-pool); read here as an input. 0 = origin. */
    int16_t  ai_target_x;   /* +0x1dc */
    int16_t  ai_target_z;   /* +0x1de */
    /* LIVE STAGE1 zombie (@0x8011f7b4 family, FUN_80100424/FUN_80101224) attack windup timer
     * (entity +0x1da, s16). The active handler FUN_80101224 counts it down while the attack-arm
     * bit (ai_flags & 0x100 = +0x1d8 & 0x100) is set; at == 0x12c (300) it fires the lunge, at
     * == 0 it transitions to the post-attack recovery state. See re15_enemy_ai_live_active. */
    int16_t  ai_attack_timer;   /* +0x1da */
    /* HURT-state (FUN_80105a8c) fields. The original time-shares +0x1dc with ai_target_x (search
     * target) and uses +0x1d4 for the per-spawn stagger clip; the port keeps DEDICATED fields:
     *  - hurt_clip (+0x1d4): the stagger clip, seeded once per spawn = {2,3,4,5}[rng&7] (init
     *    FUN_80100688 @0x8010079c, table @0x8011f7e4). Copied to motion (+0x94) on HURT entry
     *    (stagger handler 0x80105b7c phase 0 @0x80105c30/c38).
     *  - hit_stun: the s16 hit-stun countdown (the +0x1dc semantic in HURT state). HURT holds while
     *    hit_stun >= 0, decrements per frame by step[+0x5] (@0x8011fe30), recovers to ACTIVE at < 0
     *    (FUN_80105a8c exit gate @0x80105b18). */
    uint8_t  hurt_clip;         /* +0x1d4 — per-spawn stagger clip {2,3,4,5} */
    int16_t  hit_stun;          /* HURT-stun countdown (the +0x1dc-in-HURT semantic) */
    /* Phase 4.5.13-RE2 F1: speed was at ID 27 (wrong) — correct ID is
     * 0x16 (+0x1CC in RE2). Renamed for clarity; opcode 0x35 Speed_set
     * uses an indexed velocity vector (ID 0x17..0x1A), not this scalar. */
    int16_t  speed_h;      /* ID 0x16 → speed_h    (RE2 +0x1CC)           */
    /* Velocity vector — written by Speed_set (0x35), integrated by
     * Add_speed (0x36) each tick. RE2 uses IDs 0x17/0x18/0x19/0x1A for
     * X/Y/Z/W. The W component is angular (rot_y) velocity. */
    int16_t  vel_x;        /* ID 0x17 → velocity X (RE2 +0x1D4)           */
    int16_t  vel_y;        /* ID 0x18 → velocity Y (RE2 +0x1D6)           */
    int16_t  vel_z;        /* ID 0x19 → velocity Z (RE2 +0x1D8)           */
    int16_t  vel_w;        /* ID 0x1A → angular Y  (RE2 +0x1DA)           */
    /* Target/lookat (IDs 0x20..0x25, RE2 +0x94..+0x9E). Used by Plc_neck
     * mode 4 ("look-at") and the F8 NCCT lighting target.               */
    int16_t  target_x;     /* RE2 +0x94                                   */
    int16_t  target_y;     /* RE2 +0x96                                   */
    int16_t  target_z;     /* RE2 +0x98                                   */
    int16_t  lookat_x;     /* head-look PITCH applied to bone 3 (set by neck FSM) */
    int16_t  lookat_y;     /* head-look YAW   applied to bone 3 (set by neck FSM) */
    int16_t  lookat_z;     /* RE2 +0x9E                                   */
    /* Plc_neck head-look FSM (byte-true FUN_80024e40, 2026-06-16). The opcode stores a
     * WORLD look-at TARGET (+0x160/162/164) + mode flags (+0x1b8); re15_neck_update slews
     * the head yaw/pitch toward it each frame (damped) and publishes to lookat_x/y, which
     * skeleton_common.c adds to bone 3 (PL00 head) — player only (enemies never Plc_neck). */
    uint8_t  neck_flags;   /* +0x1b8: 0x80=enabled; 0x04/0x08=world look-at active; mode bits */
    int16_t  neck_tx, neck_ty, neck_tz;  /* +0x160/162/164: world look-at target */
    int16_t  neck_speed;   /* +0x9e: engage rate (reserved) */
    int16_t  neck_yaw;     /* +0x94: current slewed head yaw   (signed) */
    int16_t  neck_pitch;   /* +0x96: current slewed head pitch (signed) */
    /* Animation =========================================================*/
    int16_t  motion;       /* ID 8 (0x8) → motion_no/clip id (RE2 +0x0A)  */
    int32_t  anim_frame;   /* per-clip frame counter (engine-side, RE2
                            * stores this in a separate work-area)        */
    uint8_t  anim_freeze;  /* 2026-06-17: set on a scripted Plc_dest walk/run ARRIVAL
                            * (actor_locomotion.c) → re15_compute_actor_kf freezes the held
                            * walk clip at frame 0 (byte-true FUN_8001f3bc end-of-clip wrap),
                            * matching the original which holds the walk clip (no idle reset).
                            * Cleared by re15_actor_set_motion on the next motion change.    */
    /* T-round (2026-05-25): per-actor anim flag word (RE2 +0x1c4).
     * Bit 0x04 = LOOP (else FREEZE on clip end — original engine default).
     * Bit 0x08 = "second pass" enable.
     * Bit 0x40 = clear interp counter.
     * Bit 0x80 = REVERSE playback (Plc_flg(0,128,0) sets this).
     * Written by Plc_motion's pc[3] high byte + Plc_flg subop=OR/SET/XOR. */
    uint16_t anim_flags;   /* also RE1.5 Member id 16 → +0x1c4 (lhu) */
    /* BD-round 2026-05-28: per-actor Plc_motion FSM-init delay. PSX
     * Plc_motion writes state=4 to actor+0x4; on NEXT tick FSM transitions
     * state=4→state=1 and zeros anim_frame (per T8 disasm). Set to 1 on
     * motion change so first tick the animation is FROZEN (no anim_frame
     * progression), matching PSX's 1-tick init delay between Plc_motion
     * fire and visible animation start. */
    uint8_t  motion_init_delay;
    /* === Keyframe CROSSFADE (byte-true FUN_8001f3bc FRAC = entity+0x8f) =====
     * On a motion change the original blends sub-frames from the PREVIOUS
     * rendered pose into the new clip: root translation via GTE gpf12/gpl12 and
     * EVERY bone angle via FUN_80020510 (shortest-arc 12-bit lerp), with weight
     * 0x1000-0x200*frac. anim_frac counts down (one step per rendered frame); 0
     * = no blend (steady playback). prev_angles/prev_root hold the last RENDERED
     * pose, updated in re15_skel_compute_pose (which reads the actor via the
     * global g_anim_pose_actor = the original's DAT_800ac784 current-actor).
     * The [32] MUST match RE15_EMD_MAX_BONES (re15_emd.h, included below the
     * struct so the literal is used here). Seeded to 7 on every motion change. */
    uint8_t  anim_frac;
    uint8_t  anim_prev_valid;
    /* Player animation-bank select. 1 = PL00.EDD (the original's COMMON/char-keyed bank, holds
     * the ROOM1150 kneel clip 11 = the real ~25-frame stand→kneel pelvis fold py -1810→-761);
     * 0 = the room RBJ cinematic overlay (ROOM1170 wave + the clip-11 HOLD/settle, gesture clips
     * absent from PL00.EDD). Set by op_plc_motion, honored in re15_actor_anim_select for the
     * player. RESULT-CORRECT selector, validated on all shipped data. Full RE (2026-06-18,
     * see memory anim_bank_selection_mechanism_2026_06_18): the Plc_motion entity operand (pc[1])
     * only lands in actor+0x05 — it does NOT pick the bank in the original — but it co-varies
     * faithfully with the bank everywhere checked. Incl. ROOM1150 sub08: clip 11 at entity 1 =
     * kneel DOWN/UP (the PL00 fold), at entity 0 = HOLD the kneel during dialog (the RBJ settle).
     * clip 11 is deliberately authored in BOTH banks (PL00.EDD=fold, RBJ=settle), so the entity
     * byte IS the intended fold-vs-hold switch — matches original_kneeing/ (fold-then-hold).
     * ⚠️ The original's COMMON-vs-RBJ pose selector itself is NOT fully RE'd (the RBJ-pose reader
     * is unconfirmed; DAT_800aca5a switches WEAPON↔COMMON, not COMMON↔RBJ). The entity rule is the
     * faithful, lower-risk expression; revisit only if a room breaks the entity↔bank co-variance. */
    uint8_t  anim_use_pl00;
    int16_t  prev_angles[32][3];
    int32_t  prev_root[3];
    /* Plc_dest walker state (Phase 4.5.13-RE2 F3 — port FUN_8001ed9c) ==
     * When SCD fires Plc_dest, we stash dest+mode here and the per-frame
     * integrator drains it: yaw lerp toward dest, position step, set
     * arrival flag when Manhattan/Euclidean distance < step_size.       */
    uint8_t  walk_active;     /* 1 = walking toward dest                  */
    uint8_t  walk_mode;       /* hi-byte from Plc_dest: 0x05 RUN, 0x09 TURN */
    uint8_t  walk_flag_bit;   /* arrival flag bit in zone 5 (lo byte)     */
    uint8_t  walk_pad;        /* (legacy, unused since BO walker FSM)      */
    uint8_t  walk_fsm;        /* BO Tier-3: PSX DAT_800aca5a 0=init 1=align 2=step */
    int16_t  walk_dest_x;
    int16_t  walk_dest_z;
    /* AO6-round 2026-05-26: target yaw captured ONCE when Plc_dest fires,
     * not recomputed each tick. Per [[walker_canonical_N4_2026_05_24]] the
     * PSX walker (FUN_800245d8) doesn't do atan2 — target is set ONCE by
     * the motion-mode handler via FUN_8001a6d4 atan2, then FUN_8001a8f8
     * slews actor.rot_y toward it. Per-tick atan2 in our previous code
     * caused overshoot oscillation: walker passes dest by step-size, new
     * atan2 flips 180°, walker spins back, overshoots, repeats forever.
     * User-visible: Leon spinning around Y-axis after spawn. */
    int16_t  walk_target_yaw;

    /* Root-motion integration cache (Phase 4.5.13-RE2 I-FINAL).
     * Stores the previous tick's keyframe pos so we can compute the
     * per-frame delta (current_kf_pos - prev_kf_pos) and apply it
     * (yaw-rotated) to actor.pos. RE2-faithful: clip keyframes carry
     * absolute root positions in clip-local coords; the engine moves
     * the actor by their per-frame differential. */
    int16_t  root_prev_kf;     /* last applied keyframe index (-1 = none) */
    int16_t  root_prev_motion; /* motion when prev_kf was recorded        */
    int16_t  root_prev_x;
    int16_t  root_prev_y;
    int16_t  root_prev_z;
    int16_t  root_pad;
} re15_actor_t;

extern re15_actor_t g_actors[RE15_ACTOR_MAX];
extern uint8_t      g_actor_count;   /* highest active slot + 1 — for HUD */

/* Phase 4.5.13 O5 (2026-05-23): Plc_motion handler @ PSX.EXE 0x80041b90
 * does NOT reset anim_frame (+0x95) — it only writes motion_no (+0x94)
 * and clears state sub-flags (+0x06/+0x07/+0x1c8/+0x1ca). The anim_frame
 * wraps via the keyframe processor's `if (frame_count <= bVar3) +0x95=0`
 * at clip end. Phase-locked transitions are intentional. */
static inline void re15_actor_set_motion(re15_actor_t *a, int16_t m)
{
    /* T-round: on every Plc_motion, original engine clears
     * actor+0x06/0x07/0x95 (state + frame_index). Reset anim_frame to 0
     * so the new clip starts from frame 0 (not at whatever residual count
     * from the prior clip). Required for FREEZE-default renderer to
     * correctly distinguish "currently playing" (cur < frame_count)
     * vs "ended, freeze on last" (cur >= frame_count). */
    if (a->motion != m) {
        a->anim_freeze = 0;
        a->anim_frame  = 0;
        a->anim_flags  = 0;       /* clear sticky flags from prior clip */
        a->motion_init_delay = 1; /* BD-round: PSX state=4→state=1 1-tick FSM */
        a->anim_frac   = 7;       /* seed FRAC crossfade (FUN_8001f3bc +0x8f) — blend
                                   * from the prior rendered pose into the new clip. NEEDED:
                                   * cutscene clips (e.g. room1150 clip 10) have frame-0 =
                                   * full kneel, so the kneel-DOWN is produced ENTIRELY by
                                   * this blend (no in-clip bend). */
    }
    a->motion = m;
}

void re15_actor_init(void);

/* Allocate a slot for a new actor of `type`. Returns slot index, -1
 * if all slots busy. */
int  re15_actor_alloc(uint8_t type);

/* Reset slot. */
void re15_actor_free(int slot);

/* Member access — used by SCD Member_set / Member_cmp.
 * Returns the property as s32 (sign-extending smaller types). Writes
 * truncate to the property's native width. Unknown member_id → 0 / no-op. */
int32_t re15_actor_get_member(int slot, uint8_t member_id);
void    re15_actor_set_member(int slot, uint8_t member_id, int32_t value);

/* Phase 4.5.13-RE2 F3: per-frame walker integrator (port of RE2 FUN_8001ed9c).
 * `step_walk` advances ONE actor's locomotion; `step_all_walkers` drains
 * every active walker. Call once per SCD tick from main.c after scd_vm_tick. */
void re15_actor_step_walk(re15_actor_t *a);
void re15_actor_step_all_walkers(void);

/* Yaw (Q12, 0 = +Z) from a world XZ delta. `re15_atan2_q12(dz,dx) - 1024` gives
 * the mesh rot_y (0 = +X) that faces direction (dx,dz). */
int16_t re15_atan2_q12(int32_t dz, int32_t dx);

/* Per-frame Plc_neck head-look FSM (byte-true FUN_80024e40 damped look-at) is computed
 * inside re15_skel_compute_pose at the head bone (bone 8), where the root bone matrix is
 * available so the look angle is taken in the correct root-local frame. No standalone
 * call — the old body-relative re15_neck_update was retired to avoid double-slewing. */

/* Phase 4.5.13-RE2 I5: keyframe-driven actor translation. Mirrors RE1.5
 * engine's FUN_8001F3BC integrator that reads root delta from bytes 0..5
 * of each keyframe and adds (yaw-rotated) to actor world pos. */
#include "re15_emd.h"
void re15_actor_apply_root_motion(re15_actor_t *a,
                                  const re15_emd_skeleton_t *skel,
                                  const re15_emd_animation_t *anim);

#endif /* RE15_ACTOR_H */
