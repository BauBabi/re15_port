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
    uint8_t  em_flag_id;   /* +0x1C6 — the Sce_em_set (0x44) pc[7] kill-flag index, latched at spawn.
                            * On death-commit the enemy sets the em-status flag[em_flag_id] so it does
                            * not respawn on room re-entry (the spawn gate GETs it). 0xFF = never persist. */
    uint16_t status_flags; /* RE2/RE1.5 +0x98 (Member id 17, lhu) — bit0x2 = bleed/poison */
    int16_t  water_y;      /* +0x88 — the sce=8 water-level stamp (LAB_8004330c re-stamps it every
                            * frame the entity is inside a water zone; wade/ripple FX consumer is
                            * presentation-side). 0 = no water. [wf_f536e1ee step 4] */
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
    int16_t  grab_kill_ctr; /* +0x9e : the grab KILL counter (FUN_80102548 [2]=100; [3] -- per tick;
                             * was-0 OR player-hp<0 -> the DEVOUR handoff word ((+0x5)+2)<<8|1) */
    int32_t  anchor_x;      /* +0xa0 : the clip root-motion ANCHOR (FUN_8001ac38: pos - rotate(off));
                             * +0xa2   func_0x8001ad68 places pos = anchor + rotate(off[kf], yaw).
                             *         The grab [0] COPIES the zombie anchor onto the player -> the
                             *         pair interlocks in the AUTHORED formation (no clipping). */
    int32_t  anchor_z;
    uint16_t ai_flags;      /* +0x1d8: bit0x10 = "approach permitted" gate (decision block);
                             * bit0x1 = "actively grabbing" (set grab-[0] FUN_80102548.c Z.28,
                             * cleared throw-off-[4] Z.75; read by the domino-shove gate Z.87) */
    uint8_t  aim_band;      /* word0-Bits 29-31 (Kraehen-ACTIVE-Tail @0x80112560-c8): Elevation-
                             * Band fuer den Schuss-Resolver. 4 = UP (0x80000000 via 0x80012a0c
                             * (0x1770), vert>=4001 && dist<6000), 1 = DOWN (0x20000000 via
                             * 0x80012974(0x1770), vert<800), 2 = LEVEL (0x40000000), 0 = kein
                             * Band (unschiessbar). Nur der Kraehen-Tail stempelt es in STAGE1
                             * (crow_shot_attack.md F5). */
    uint8_t  grab_choreo;   /* word0 (+0x0) bit 0x1000 — grab-choreography latch: set in grab-[0]
                             * (FUN_80102548.c Z.21-22; the original also sets the PLAYER's word0 —
                             * no modeled consumer, not mirrored), cleared ONLY in grab-exit [8]
                             * (Z.115 &0xffffefff; a shot-aborted grab leaves it set = byte-true
                             * leak). Consumer: windup-timeout death gate FUN_80101224.c Z.12
                             * `(+0x1d8&0x100) && !(word0&0x1000)`. Dossier analysis/zombie_hit_1140.md D5/D6. */
    uint8_t  ai_contact;    /* +0x90 : WALL-contact byte (writer = the SCA resolver FUN_8003b0a4:
                             * heading-nibble<<4 | 8 | cell-attr&3; low nibble cleared per pass).
                             * Port writer deferred (ROOM1140 has 0 attr cells). */
    uint8_t  contact_flags; /* +0x1c2: BODY-push contact bits (FUN_8002aec4: |=1 player pushed me,
                             * |=2 an enemy did); cleared per tick (FUN_8002b498). */
    int8_t   contact_slot;  /* +0x1ac: the contacting ENEMY's actor slot (ptr in the original) —
                             * the grab [5] domino-shove target. */
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
    /* STEER TARGET (+0x1bc/+0x1be) — the player XZ, REFRESHED EVERY TICK (RAM-arbitrated: in all
     * 16 live tl_run2 samples every zombie's +0x1bc/+0x1be == the player pos; the init store
     * @0x8010071c/734 is only the FIRST write; the per-tick writer evades an offset-literal scan —
     * exact address = open RE item). BOTH live walks steer toward it with the SIGNED gait slew
     * (FUN_801021f8:57/FUN_801057bc:40: func_0x8001aac4(+0x1bc,+0x1be,±sVar7) — the -1 rows steer
     * AWAY = the authored weave). */
    int16_t  steer_x;       /* +0x1bc */
    int16_t  steer_z;       /* +0x1be */
    uint8_t  repath_timer;  /* +0x91 : nav repath cycle (low7: reload |7 at 0, -1/tick; the DFS
                             * runs only on the 0-tick). bit 0x80 = nav-disable latch (checked,
                             * never set in the shipped game). Spawn seed = spawn_index & 7
                             * (@0x80042244) — staggers the per-tick DFS across entities. */
    uint8_t  ai_wp_node;    /* +0x1d6: wander-roam waypoint node id (set by the state-1 roam
                             * entry = re15_nav_rand_zone; consumed with ai_flags bit3) */
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
    /* HURT torso-bend (the visible RECOIL of a hit). The stagger handler does NOT switch clips — it
     * bends one bone while the walk clip keeps playing: `lhu part+100; lhu ent+0x9c; addu; sh part+100`
     * (@0x80105d54-64 bend-down / @0x80105df8-08 bend-up), where part = model_pool + 1204 = part index
     * 7 (stride 172, @0x80105ba4). +0x9c ramps -0x80/tick for 3 ticks, then +0x80/tick for 3
     * (@0x80105d7c-84 / @0x80105e20). The offset lands on the bone's vz euler — the SAME slot the
     * Plc_neck pitch uses (+0x64), see skeleton_common.c. bend_bone < 0 = inactive.
     * NOTE: only the INDEX 7 is proven (1204/172); the anatomical bone identity is not parsed. */
    /* RE2-AI option (inactive while re15_ai_flavor()==RE15). RE2 struct offsets in the comments. */
    uint8_t  re2z_gaitrow;      /* +0x16B RE2 gait row 0..31 into tbl @0x8010C924 */
    uint8_t  re2z_gaitinit;     /* port-only: has the gait machine been seeded for this walk? */
    uint16_t re2z_gaittmr;      /* +0x158 RE2 gait segment timer (frames left in this row)     */
    int16_t  hurt_bend_bone;    /* part index to bend, -1 = none */
    int16_t  hurt_bend_vz;      /* the PRE-update +0x9c applied this tick */
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
    /* Plc_neck head-look FSM — byte-true FUN_80037358 (per-frame consumer) + opcode handler
     * @0x80041e98. GLOBAL (cutscene_headlook.md, 2026-08-03): the FSM runs for the PLAYER
     * (caller @0x80031d78) AND for every STAGE1 NPC root (jal 0x80037358 @0x8011c69c and
     * siblings) — the opcode targets the per-thread WORK entity (`lw v1,0x154(a0)`
     * @0x80041e9c), never implicitly the player. Flag bits (+0x1b8, after the 0x80 base):
     *   0x04 world point (+0x160/162/164) · 0x08 RELATIVE (+0x162 yaw offset on the body
     *   facing, +0x164 pitch target) · 0x40/0x20 yaw/pitch SWEEP (counter +0x160, mirror on
     *   snap-arrival, completion -> flags=0x12 @0x80037698/@0x80037858) · 0x10 pitch->0 ·
     *   0x02 target=keyframe (release) · 0x80 steps from the SCD speed bytes (+0x9e/0x9f)
     *   instead of the part defaults. Flags LOW BITS == 0 -> ENTITY TRACKING (look at the
     *   head part of neck_target_slot; NPC INIT default = the player @0x8011c738). */
    uint8_t  neck_flags;   /* +0x1b8 flag bits (see above) */
    int16_t  neck_tx, neck_ty, neck_tz;  /* +0x160/162/164: operands (world point / relative
                                          * offsets / sweep counter in tx, mirrored target in ty) */
    int16_t  neck_speed;   /* SCD speed s16: low byte = yaw step (+0x9e), high = pitch (+0x9f) */
    int16_t  neck_yaw;     /* part+0x94: current slewed head yaw   (signed) */
    int16_t  neck_pitch;   /* part+0x96: current slewed head pitch (signed) */
    uint8_t  neck_bone;    /* +0x1b9: head part index (player 8 @0x80031938, NPC 8 @0x8011c778);
                            * 0 = entity has no neck data -> FSM skipped (only player + NPC
                            * roots run FUN_80037358 in STAGE1) */
    uint8_t  neck_sweep;   /* port: sweep armed latch (first pass seeds target = full clamp) */
    int16_t  neck_step_yaw, neck_step_pitch;    /* part+0x98/+0x9a defaults: player 96/96
                                                 * @0x800319a4-ac, NPC 64/48 @0x8011c790-98 */
    int16_t  neck_clamp_yaw, neck_clamp_pitch;  /* part+0x9c/+0x9e clamps: player ±0x200/±0x138
                                                 * @0x800319b0-c4, NPC ±0x2c8/±0x138 @0x8011c7a0/b0 */
    int8_t   neck_target_slot;   /* +0x1a8 entity-tracking target (actor slot; -1 = none) */
    /* Cached head-part WORLD position from this actor's last posed frame (= the original's
     * part+0x54/58/5c, read by a TRACKING looker on the next frame). ok=0 until first pose. */
    int32_t  head_world[3];
    uint8_t  head_world_ok;
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
    uint16_t anim_blend_rate;   /* the FUN_8001f314 4th arg = the crossfade WEIGHT STEP: prev weight
                                 * = anim_frac * rate (0x1000 = 100%). WALK-family zombie states pass
                                 * 0x100 (max 0xf00 = 94%, decaying -6%/frame — never a frozen
                                 * plateau); one-shots (stand-up, grab, devour, the player handlers)
                                 * pass 0x200. 0 -> treated as 0x200 (the player default). */
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

    /* Crow (type 0x21) 3D-flight state — byte-true FUN_80112020 family (RE15_CROW_AI.md).
     * The crow shares this actor struct but flies in 3D: y (+0x38) IS the altitude
     * (smaller y = higher). Its flight fields live BEYOND the modeled zombie offsets
     * (+0x1e4/+0x1ea/+0x1ec/+0x1d4), so they get dedicated names — same pattern as the
     * dedicated hurt_clip/hit_stun fields above. */
    int16_t  crow_perch_h;   /* +0x1ea: target/perch height, seeded once at INIT (= spawn y) */
    int16_t  crow_vert_err;  /* +0x1ec: playerY - y, recomputed every ACTIVE tick            */
    int16_t  crow_vvel;      /* +0x1e4: vertical velocity, integrated into y each fly tick    */
    uint8_t  crow_mode;      /* +0x1d4: a FRESH RNG byte every root tick (@0x80112028 jal rng ->
                              * @0x8011204c sb v0,468 in the testbit-jal delay slot) — low6 = |vvel|,
                              * bit0x80 = climb/descend, %60/%3/&1/&0x32 timers/dirs (audit wf_827f186d crow #1) */
    uint16_t crow_dist;      /* +0x1dc: horizontal distance to the player (SquareRoot0). UNSIGNED:
                              * every original consumer is `lhu 476` + sltiu (@0x80115e30/@0x801126c0/
                              * @0x801126cc/@0x8011332c/@0x80113a94/@0x80114110/@0x8011352c/@0x8011468c;
                              * 2x sh, 8x lhu, ZERO lh in 0x80111a00-0x80116400) (audit wf_827f186d crow #14) */
    int16_t  crow_floor;     /* +0x1ba: floor-Y reference under the crow, refreshed EVERY root tick from
                              * room_coll FUN_8001c6e8(&pos, dim[3], a2=8, a3=0x400) @0x80112158-84
                              * (audit wf_827f186d crow #5) */
    uint8_t  crow_wall;      /* +0x1d1: SCA wall-pass hit flag, root post-pass @0x801121f8-218
                              * `0x8003b0a4(&+0x34, a1=dim[3]=200, a2=4)` -> sb 465 (audit wf_827f186d crow #B) */
    int16_t  crow_grav;      /* +0x1e8: vertical accel added to vvel each dive/fall tick       */
    int16_t  crow_speed;     /* +0x8c : horizontal move speed (per flight sub-state)           */
    uint8_t  crow_timer;     /* +0x1d5: sub-state countdown (climb-back / maneuver duration)   */
    uint8_t  crow_pturn;     /* +0x1d3: anim/timer completion flag (dive-decide pending-turn)  */
    uint8_t  crow_hs;        /* +0x1d8: flock handshake byte (dispatcher force-substate gate)  */
    uint8_t  crow_parity;    /* +0x1d2: per-tick LOS/parity toggle (wing-flap alternation)     */
    uint8_t  crow_atk_ctr;   /* +0x1d6: attack/peck attempt counter (gates re-commit, <3)      */
    uint8_t  crow_bank;      /* +0x1d7: proximity yaw-weave bank latch (0x80115e24)            */
    uint8_t  crow_diveflag;  /* +0x1da: dive-committed flag                                    */
    uint8_t  crow_armed;     /* +0x1db: attack-armed flag (LOS-gated)                          */
    int16_t  crow_yawrate;   /* +0x1de: per-state yaw-slew rate                                */
    int16_t  crow_accel;     /* +0x1e6: per-tick speed accel (dive ramp)                       */
    uint8_t  crow_contact;   /* +0x1d0: player-contact flag (strike/grab connect)             */
    int16_t  crow_struggle;  /* +0x9c : grab-hold struggle meter (drains, <0 = release)        */
    uint16_t crow_shadow_w;  /* +0xbc: Schatten-Halbbreite — ACTIVE-Tail ((y-floor)>>4)+400 min 100
                              * (@0x80115fa0-e4); Corpse-Pool-Grower +10/Tick (@0x8011589c-a0);
                              * GIB-Wipe = 1 (@0x80115938). (crow_death_pool.md §1.3/1.4) */
    uint16_t crow_shadow_h;  /* +0xbe: Schatten-Halbtiefe — dieselben Writer (@0x801158a8-ac etc.) */
    uint8_t  crow_tint;      /* Grauwert der Prim-Farbwoerter +0xc4/+0xec: ((y-floor)>>5)+128 min 32,
                              * v|v<<8|v<<16 Top-Byte erhalten (@0x80115fe8-6058); Wipe -> 0 (schwarz,
                              * &=0xff000000 @0x80115940-54) */
    uint8_t  crow_pool;      /* Farb-Zustand +0xc4/+0xec: 1 = (alt&0xff000000)|0x00ffff38 dunkelrote
                              * Lache (@0x80115884-c8), 0 = Grau-Tint/Wipe */
    uint8_t  crow_hide;      /* GIB-Scatter: Original zerlegt den Koerper in die 13 spawn-allozierten
                              * Bone-Parts (+0x188, Armierung @0x80114a50-aa4) und toetet sie nach 50
                              * Ticks (`sw zero,0(part)` @0x80114b78) -> Koerper weg; der Port
                              * verbirgt das Mesh ab GIB-step-0 (Part-Scatter-Mover nicht RE'd,
                              * crow_death_pool.md §4.2) */

    /* Dog (Cerberus, type 0x20) AI state — byte-true 0x8010d7f8 family (RE15_DOG_AI.md).
     * A ground chase/lunge/bite enemy; shares the zombie steering/collision + take_damage.
     * State 1 (ACTIVE) is a dual-dispatch brain on sub_state_1 (decision @0x80120f94 + act
     * @0x80120fd4): sub 0 idle / 1 turn / 2 chase / 3 attack-range / 4 lunge / 8 bite. */
    int16_t  dog_dist;       /* +0x1d4: cached dist-to-player (lh signed, SquareRoot0)          */
    uint16_t dog_flags;      /* +0x1d0: bit0 = sticky has-LOS                                   */
    uint8_t  dog_atk_cd;     /* +0x1d6: attack-cooldown timer (menace)                          */
    uint8_t  dog_pounce_cd;  /* +0x1e6: pounce-cooldown timer                                   */
    int16_t  dog_yawrate;    /* +0x1e2: per-approach yaw-slew rate (+0x7 micro-step = sub_state_3) */
    int16_t  dog_grab_armed; /* +0x1e4: grab-armed flag (set=1 by sub 5 windup @0x8010ed54; a bite
                              * then escalates to the eaten GRAB even when non-lethal). Cleared on
                              * grab entry @0x8010f4b4 / recover. */
    int16_t  dog_blocked_ctr;/* +0x1dc: frames-blocked counter — root FUN_8010dbcc tail
                              * @0x8010dd80-dda4: `+0x1da==0 -> +0x1dc=0 else +0x1dc++` (audit
                              * wf_827f186d dog #15). Feeds the chase wake gates (@0x8010e130/e170),
                              * the chase steer-vs-wander split (@0x8010e3bc slti 31) and the
                              * menace LOS-loss exit (@0x8010e92c slti 31). */
    uint16_t dog_reroute_dir;/* +0x1e8 = +0x90 & 1, latched at the reroute commit (@0x8010e2bc-c0) */
    uint16_t dog_reroute_sca;/* +0x1ea = *(u16*)(*(+0x1b4)+10) — the standing SCA cell's attr
                              * halfword, latched at the reroute commit (@0x8010e294-2a4); bits 2-3
                              * = the reroute-14 drop-level count (@0x80110764). */
    int8_t   dog_aux9f;      /* +0x9f byte: chase wander heading latch (sb @0x8010e3f4, read back
                              * sign-extended lb @0x8010e404) / menace no-LOS counter (@0x8010e9xx)
                              * / reroute-14 wall-byte latch (@0x80110658). */
    int16_t  dog_floor_y;    /* +0x1ba: the dog's floor Y (ground level). The original's +0x1ba is
                              * maintained by the engine floor probe; the port seeds it at INIT from
                              * the spawn Y and moves it with the reroute level hops (+-0x708). */

    /* ---- Maggot (type 0x27, EM027) — byte-true 0x80116db8 family work bytes (audit wf_827f186d).
     * All four are INIT-cleared/-seeded by FUN_80116f50 (@0x8011707c/8c/98-9c/ac). */
    uint8_t  mag_airborne;   /* +0x1e0: airborne latch — set 1 at leap LAUNCH (@0x80118ab0) / finisher
                              * entry (@0x80118e84); gates the state-2/3 ballistic settle (@0x8011af6c,
                              * @0x8011b70c); cleared on land (@0x80118c94) */
    uint8_t  mag_pin_cd;     /* +0x1e1: rear-up/pin re-attempt lockout — 0xff on pin connect
                              * (@0x8011adf4), 0x5a on miss (@0x8011af10); -1/brain tick (@0x8011741c-38);
                              * ==0 gates the sub-15 upgrade (@0x80117b28-30) */
    uint8_t  mag_boost;      /* +0x1e2 = 4 (INIT @0x80117098-9c, no other writer): leap impulse
                              * += *10 (@0x80118a64-80), vert impulse 600+*30 (@0x80118c00-18),
                              * collision-box index @0x80121368[.] (INIT @0x801171a0-c0) */
    uint8_t  mag_1e3;        /* +0x1e3: hurt-exit variant — !=0 routes the flinch exit to sub 9
                              * instead of the sub-7 leap (@0x8011b1c8-d8). ONLY writer in STAGE1.BIN
                              * is the INIT clear (@0x801170ac) -> always 0 in shipped play. */

    /* ---- Spider-Baby (type 0x26, EM026) — a STATIONARY web-spitter/ambush (RE15_SPIDER_AI.md).
     * Emerges vertically from its spawn point (spider_phase < 13 = intangible), then solid + a -2
     * contact stagger. STATE[1] arms collision hit-codes + telegraph "web" fx; killable (hurt/death). */
    uint8_t  spider_phase;   /* +0x1d0: emerge/attack budget (0..12 emerging-intangible; strike init 40/44) */
    int16_t  spider_timer;   /* +0x1d4: inter-strike wind-up timer (rng&0x3f + 16 = [16,79]) */
    int16_t  spider_home_x;  /* +0x1d8: cached spawn X (u16) */
    int16_t  spider_home_y;  /* +0x1d6: cached spawn Y (u16) — the vertical emerge anchor */
    int16_t  spider_home_z;  /* +0x1da: cached spawn Z (u16) */

    /* ---- G-Birkin BOSS (type 0x30/0x36, EM030/EM036) — byte-true 0x80116230 family (STAGE3.BIN).
     * The shared boss root snapshots/decrements these each root tick; the ACT/HURT/DEATH sub-machines
     * read them. Dedicated port fields for the PSX offsets that overlap the zombie/dog work bytes above
     * (same pattern as the dog_ / crow_ / mag_ / spider_ groups). hit_stun already carries +0x1dc. */
    uint8_t  birkin_flags;   /* +0x1dd: bit0(0x1)=run-off active (root steers to override pt), bit2(0x4)=
                              * heave toggle (wounded, gates DECIDE mutate), bit3(0x8)=mutating (revive-guard) */
    uint8_t  birkin_hurt_cd; /* +0x1de: HURT lockout countdown (set 9 on flinch @0x8011a0c4; at 0 clears +0x93&1) */
    uint8_t  birkin_atk_cd;  /* +0x1df: lunge/tackle cooldown (sub-3 done 0x3c/abort 0x1e, sub-7 done 0xa) */
    uint8_t  birkin_saved_state, birkin_saved_sub, birkin_saved_ph2, birkin_saved_ph3; /* +0x1d8 word snapshot
                              * of +0x4 each root tick (@0x80116698); HURT/revive resume +0x4=*(+0x1d8) */
    int16_t  birkin_runoff_x, birkin_runoff_z; /* +0x1d4/+0x1d6: death-morph run-off point (-22000,-12000) */
    uint8_t  birkin_pause;   /* +0x9e: ACT[1] two-phase close-in pause countdown (0x5a) */
    uint8_t  birkin_grab;    /* PORT-LOCAL persistent latch for the GLOBAL grab channel DAT_800aca58==5
                              * (aca58 is engine-global, and run_all clears s_player_grabbed each frame): set on
                              * a claw connect, re-asserts s_player_grabbed each tick so the HUB DECIDE grab-tail
                              * (which gates on aca58==5) is reachable; cleared on throw/HURT/death. */

    /* ---- Cockroach (type 0x29, EM029) — byte-true 0x80110b00 family (STAGE3.BIN; audit wf_efd92a2c
     * cockroach). Dedicated fields for the PSX offsets that overlap the shared work bytes above. The
     * roach reuses dog_atk_cd(+0x1dc attack cooldown), crow_speed(+0x8c scurry speed), ai_timer(+0x9c idle
     * timer), sub_state_2(+0x6 phase byte) and sub_state_3(+0x7 sighted/blind flag). */
    uint8_t  roach_los;      /* +0x1d0 bit0: LATCHED LOS — only a 0/1 probe result updates it (@0x8011105c) */
    uint8_t  roach_air;      /* +0x1e0: airborne latch (leap/flight subs 7/9) (@0x8011250c) */
    uint8_t  roach_beh2;     /* +0x1e2: behavior&2 -> HURT-exit sub 5 (INIT clear @0x80110e2x; set by beh&2) */
    uint8_t  roach_esc;      /* +0x1e3: default 1 (cleared by beh&4) -> HURT-exit sub 9 fly-away (@0x80110e18) */
    int16_t  roach_fade;     /* +0x9e: corpse fade counter, seeded 0x5a (@0x80115a98) */
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
