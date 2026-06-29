/*
 * RE1.5 Rebuilt — Enemy-AI FSM dispatch + decision brain (Phase 2, 2026-06-29).
 *
 * Byte-true port of the STAGE1 zombie AI's dispatch foundation. The per-type tick
 * entry is the entity dispatch table in PSX.EXE @0x80072bac: type 0x47 -> FUN_8011d6d4
 * (RE_15_Quellcode_Overlays/STAGE1_full). The AI is a nested function-pointer FSM,
 * decoded byte-true from STAGE1.BIN (raw load @0x80100000, no header — Weg C):
 *
 *   FUN_8011d6d4 (tick)   gate, cache dist@+0x1d0, (*PTR_FUN_801217a0[entity+0x4])()
 *   PTR_FUN_801217a0[0..4]   main-state table  (file 0x217a0):
 *        0 = FUN_8011d84c  INIT      (-> sets state 1; the spawn/setup)
 *        1 = FUN_8011d9f4  ACTIVE    (-> sub-dispatch on entity+0x9 & 0xf)
 *        2 = FUN_8011db40  HURT      (body not decoded — deferred)
 *        3 = FUN_8011db88  DEATH     (body not decoded — deferred)
 *        4 = 0x80050be8    IDLE      (EXE; stationary briefing zombies sit here)
 *   PTR_FUN_801217b4[0..15]  sub table (= &PTR_FUN_801217a0[5]):
 *        [0]  = FUN_8011da48  (the +0x5 logic/anim double-dispatch)
 *        [1..15] = 0x8004f.. / 0x80050.. / 0x80051.. generic-humanoid EXE leaves
 *   FUN_8011da48 (sub 0):  logic = SUB[1 + (entity+0x5)] , anim = SUB[4 + (entity+0x5)]
 *        (verified: +0x5=7 -> logic SUB[8]=0x80050ddc, anim SUB[11]=0x80051148.)
 *
 * There are TWO decision systems (DYNAMICALLY DISAMBIGUATED from the live combat
 * savestate, dispatch tables un-patched at runtime — see re15_enemy_ai.c):
 *  (A) the LIVE STAGE1 combat path: state +0x4=1 -> FUN_8011d9f4 -> sub +0x9=0 ->
 *      FUN_8011da48 -> logic = PTR_FUN_801217b4[1+(+0x5)], anim = [4+(+0x5)] — the
 *      EXE generic-humanoid leaves (0x8004f.., e.g. FUN_8004f100 = the +0x5=0 assess).
 *      The active type-0x10 zombies run THIS (verified +0x9=0, +0x5 in {1,2,6,12}).
 *  (B) a PARALLEL per-AI-mode vtable system, dispatched on +0x5 by FUN_8010168c /
 *      FUN_80101784 / FUN_8010b6d4 / FUN_8010b800 (held in the mode table @0x8011f800)
 *      into 0x8011f840 / f960 / 120264 / 120324 (decide) + f890/1202a8 (animate):
 *        [0] FUN_80101b64 (search w/ timer) | FUN_80101c7c (approach-only)
 *        [1] FUN_80101de4 (search)          [2] FUN_80102058 (rich: grab/flee)
 *        [3..] FUN_80102540 / FUN_80102bd0 / FUN_80102d20 (movement/anim, deferred)
 *      This system is byte-true RE'd below but is NOT on the confirmed live +0x9=0
 *      path — it serves a different AI mode / type. Kept (real code) + clearly labelled.
 *
 * This module ports: the named state model, INIT (FUN_8011d84c), the tick entry +
 * main dispatch (FUN_8011d6d4), the ACTIVE sub-dispatch structure (FUN_8011d9f4), and
 * the confirmed decision handlers FUN_80101b64 / FUN_80101c7c / FUN_80101de4. The
 * generic-humanoid EXE leaves (movement/anim execution) + FUN_80102058 (needs the
 * FUN_8001a780 octant + the DAT_800acad6/ae7 game globals) are DEFERRED to a later
 * phase, with their byte-true addresses cited. NOT wired into game_step yet (no tick
 * side effects -> no 1170 risk; same stance as the damage engine). See re15_enemy_ai.c.
 */
#ifndef RE15_ENEMY_AI_H
#define RE15_ENEMY_AI_H

#include <stdint.h>
#include "re15_actor.h"

/* Main-state values at entity+0x4 (= actor.state) — PTR_FUN_801217a0 index. */
enum {
    RE15_AI_STATE_INIT   = 0,   /* FUN_8011d84c */
    RE15_AI_STATE_ACTIVE = 1,   /* FUN_8011d9f4 */
    RE15_AI_STATE_HURT   = 2,   /* FUN_8011db40 (deferred) */
    RE15_AI_STATE_DEATH  = 3,   /* FUN_8011db88 (deferred) */
    RE15_AI_STATE_IDLE   = 4    /* 0x80050be8  (deferred)  */
};

/* grid_id (entity+0x9) bit usage for an enemy actor (the original packs all three into
 * the one byte): low nibble = the ACTIVE sub-dispatch index, plus these flag bits. */
#define RE15_AI_GRID_SUB_MASK   0x0f   /* PTR_FUN_801217b4 index */
#define RE15_AI_GRID_SKIP       0x20   /* tick-gate: skip this frame (FUN_8011d6d4) */
#define RE15_AI_GRID_STATIONARY 0x40   /* spawned stationary -> INIT routes to IDLE/+0x5=6 */

/* Decode a 32-bit state word (the value the decision handlers store at +0x4) into the
 * port's split state bytes: state=+0x4, sub_state_1=+0x5, sub_state_2=+0x6, sub_state_3=
 * +0x7. e.g. 0x701 -> state 1 / sub_state_1 7. Byte-true to the original's word store. */
void re15_ai_set_state_word(re15_actor_t *e, uint32_t word);

/* INIT = FUN_8011d84c: state -> ACTIVE(1), clears the hit/anim guards, wires the per-type
 * hitbox (re15_enemy_apply_hitbox), seeds the AI params (ai_arc=0x2c8, ai_p5f8/5fa/5fe).
 * If grid_id has the STATIONARY bit -> state IDLE(4) / sub_state_1 6 (the briefing zombie).
 * Idempotent setup; safe to call at enemy spawn. */
void re15_enemy_ai_init(int slot);

/* Engine-wide AI pause = the original's DAT_800aca40 & 0x20000000 gate (cutscene/menu
 * freeze). 0 = run (default). The Phase-5 game_step wiring connects this to the real
 * freeze flag; exposed now so the gate is testable in isolation. */
void re15_enemy_ai_set_paused(int paused);

/* Tick ONE enemy = FUN_8011d6d4 entry: honour the pause + per-entity skip (grid_id &
 * 0x20) gate, cache the player distance at ai_dist (+0x1d0), then dispatch the main
 * state (INIT/ACTIVE real; HURT/DEATH/IDLE bodies deferred). Returns 1 if the state was
 * dispatched this frame, 0 if the gate skipped it. The post-dispatch collision/render
 * helpers (FUN_8002b498/FUN_8002aec4/...) are other subsystems and are NOT run here. */
int re15_enemy_ai_tick(int slot);

/* The per-enemy per-frame step = the port's single integration entry that game_step will
 * call for each active enemy: the FSM tick (FUN_8011d6d4, the decision) followed by the
 * lunge slice of the action driver (the FUN_80019e20 lunge action -> re15_enemy_lunge_tick,
 * which fires the hitbox while a lunge is active). Combines the two per-frame passes the
 * original runs separately (the entity-update loop + the model-instance action driver).
 * Returns the tick result. NOT yet called from game_step — the live wiring (+ the room1140
 * overlay spawn + the skeleton attack-point + the anim-keyframe lunge-begin) is the
 * remaining integration block; this is the byte-true-composed entry, exercised by the test. */
int re15_enemy_ai_step(int slot);

/* ACTIVE state = FUN_8011d9f4: dispatch on grid_id & 0xf. For sub 0 (the live STAGE1
 * routing) this is FUN_8011da48's +0x5 logic/anim double-dispatch; subs 1..15 are the
 * generic-humanoid EXE leaves (deferred). Returns the resolved sub index (grid_id & 0xf).
 * The leaf BODIES are not executed yet — see the module header. */
int re15_enemy_ai_active(int slot);

/* ==== System (A): the LIVE STAGE1 EXE-leaf path (dynamically confirmed) ============== *
 * The active type-0x10 zombies dispatch through FUN_8011da48 to the EXE generic-humanoid
 * leaves. These are pure decision logic (no model pool) and ARE the live AI. */

/* DAT_800aca52 & 1 — a STAGE1 global flag the assess leaf reads (only for type 0x4b).
 * 0 by default; setter for the Phase-5 wiring + the test. */
void re15_enemy_ai_set_global_flag(int v);

/* FUN_8004f100 — the +0x5=0 "assess" leaf (PTR_FUN_801217b4[1], the SUB[1+0] logic for
 * FUN_8011da48 at +0x5=0): the live sub-mode that picks the next +0x5 from dist/arc/
 * flags. Byte-true (PSX.EXE @0x8004f100). Writes (last condition wins):
 *   dist (ai_dist) >= 0x5dd      -> +0x5 = 1 (+0x6 = 0)   [far -> search]
 *   player OUTSIDE the ±0x4b0 arc -> +0x5 = 2 (+0x6 = 0)   [turn to face]
 *   (global_flag & 1) && type==0x4b -> +0x5 = 6 (+0x6 = 0)
 *   player.hit_react != 0         -> +0x5 = 6 (+0x6 = 0)   [react to the player's hit]
 * (FUN_8001ab9c — the leaf's arc test — is FUN_8001a9cc with the cone passed directly, i.e.
 * re15_ai_arc_test; verified identical incl. the 0x800 boundary.) */
void re15_ai_exe_assess(re15_actor_t *e, const re15_actor_t *player);

/* FUN_8004f3a4 — the +0x5=1 "search" leaf (PTR_FUN_801217b4[2]). Byte-true:
 *   dist < 0x1f4 (very close)               -> +0x5=3, +0x6=1
 *   player OUTSIDE the ±0x400 arc of ai_target (+0x1dc/+0x1de) -> +0x5=3, +0x6=1
 *   dist >= 0xbb9 (lost him)                -> +0x5=5, +0x6=0
 *   (global_flag & 1) && type==0x4b         -> +0x5=6, +0x6=0
 *   player.hit_react != 0                   -> +0x5=6, +0x6=0 */
void re15_ai_exe_search(re15_actor_t *e, const re15_actor_t *player);

/* FUN_8004f5e8 — the +0x5=2 "turn-to-face" leaf (PTR_FUN_801217b4[3]). Byte-true:
 *   player INSIDE the tight ±0x40 arc (now facing him) -> +0x5=0, +0x6=0 (back to assess)
 *   dist >= 0x7d1 (got far)                 -> +0x5=1, +0x6=1 (search)
 *   (global_flag & 1) && type==0x4b         -> +0x5=6, +0x6=0
 *   player.hit_react != 0                   -> +0x5=6, +0x6=0 */
void re15_ai_exe_turn(re15_actor_t *e, const re15_actor_t *player);

/* FUN_8011da48 LOGIC dispatch (the live System-A half): routes the active sub-mode +0x5
 * to its EXE leaf. +0x5=0/1/2 -> the ported assess/search/turn decisions; +0x5>=3 are the
 * movement/anim-execution leaves (anim_set/walker/model-pool -> port-skeleton-mapped,
 * DEFERRED). The companion anim dispatch (SUB[4+(+0x5)]) is deferred entirely. NOTE: no
 * live leaf writes +0x5=7 — the lunge ATTACK is not a +0x5 decision; it emerges from the
 * lunge animation + the model-instance action driver (FUN_80019e20 action 0x16-0x19 ->
 * FUN_80017fa4 = re15_enemy_attack), the deferred execution layer. */
void re15_ai_exe_dispatch(re15_actor_t *e, const re15_actor_t *player);

/* ==== System (B): the parallel per-AI-mode decision brain (byte-true, not the live path) */
/* ---- Decision brain (byte-true; the per-mode vtable[0..1] entries) ------------------ *
 * Each reads the cached ai_dist + the arc tests (re15_ai_arc_test) + ai_timer/ai_flags/
 * anim_flags + the shared RNG, and writes the next state word at +0x4 via
 * re15_ai_set_state_word. They mutate the actor in place. (FUN_80102058 — the directional
 * grab + low-HP flee — is deferred: it needs the FUN_8001a780 octant and game globals.) */

/* FUN_80101b64 — search with a countdown: ai_timer-- (0 -> 0x101); dist<2000 &&
 * arc(0x2c8)!=0 -> 0x701 (attack); in wide arc(0x5f4) + ai_flags&0x10 -> 0x201
 * (approach), and if also dist>10000 && rand&7==0 -> 0x801 (wander). */
void re15_ai_decide_search_timer(re15_actor_t *e, const re15_actor_t *player);

/* FUN_80101c7c — approach-only: in wide arc(0x5f4) + ai_flags&0x10 && dist<4000 ->
 * 0x201, and if rand&7==0 -> 0x801. */
void re15_ai_decide_approach(re15_actor_t *e, const re15_actor_t *player);

/* FUN_80101de4 — search (no timer): dist<2000 && arc(0x2c8)!=0 -> 0x701; wide arc +
 * ai_flags&0x10 -> 0x201, and dist>10000 && rand&3==0 -> 0x801; anim_flags&0x1000 ->
 * 0x1001. */
void re15_ai_decide_search(re15_actor_t *e, const re15_actor_t *player);

/* FUN_8001a780 — relative-facing test: 1 if `other`'s heading (rot_y, +0x6a) is within
 * the front hemisphere of `e`'s heading: ((other.rot_y - e.rot_y + 0x400) & 0xfff) <
 * 0x800. Used by the engage decision to pick a grab-from-front vs grab-from-behind. */
int re15_ai_facing_aligned(const re15_actor_t *e, const re15_actor_t *other);

/* FUN_80102058 — the rich "engage" decision (the per-mode vtable[2] = the +0x5=2 sub-
 * mode). Two arms gated by the contact byte ai_contact (+0x90):
 *  - in firm contact (ai_contact&3) from ~ahead -> 0x901 / 0xa01 (the contact reaction);
 *  - else: attack 0x701 (dist<2000 && arc(0x2c8)!=0); a directional GRAB 0x301/0x401
 *    (player not mid-hit && dist<0x4b0 && in the 0x200 front cone && SAME floor band,
 *    front/back via re15_ai_facing_aligned); a player-DEAD grab 0xc01 (dist<0x5dc &&
 *    player.hp<0); and anim_flags&0x1000 -> 0x1001. The original's DAT_800acae7/ad6/ee
 *    are the player block's +0x93 (hit_react) / +0x82 (floor) / +0x9a (hp) — i.e. the
 *    `player` actor here. */
void re15_ai_decide_engage(re15_actor_t *e, const re15_actor_t *player);

/* The +0x5 decision dispatch = FUN_8010168c's first call (*PTR_FUN_8011f840[entity+0x5])().
 * Routes the active-AI sub-mode (+0x5 = sub_state_1) to its decision handler. Byte-true
 * f840 vtable: [0]=FUN_80101b64 (search+timer) [1]=FUN_80101de4 (search) [2]=FUN_80102058
 * (engage) [3,4]=FUN_80102540 [5,6]=FUN_80102bd0 [7]=FUN_80102d20 [8]=FUN_80102f1c
 * [9]=0x801031a4 [10]=0x801033c0 [11]=0x80103854 — indices 3.. are the movement / attack-
 * execution leaves (DEFERRED; they drive the model pool + skeleton). The f960 mode is the
 * same table with [0]=FUN_80101c7c (approach-only, re15_ai_decide_approach). The companion
 * animate dispatch (*PTR_FUN_8011f890[+0x5])() is a separate per-mode anim vtable (deferred).
 * NOT auto-wired into the tick yet: the live path reaches here through the generic-humanoid
 * EXE leaves (PTR_FUN_801217b4[1..15] = 0x8004f.., raw-byte in the dump) which are deferred;
 * this models the decision LAYER byte-true and is exposed for testing + later wiring. */
void re15_ai_dispatch_decision(re15_actor_t *e, const re15_actor_t *player);

#endif /* RE15_ENEMY_AI_H */
