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
 * The "brain" — the transitions that pick the next state word @+0x4 — lives in the
 * per-AI-mode vtables (0x8011f840 / f960 / 120264 / 120324, each 8 entries):
 *   [0] FUN_80101b64 (search w/ timer) | FUN_80101c7c (approach-only)
 *   [1] FUN_80101de4 (search)          [2] FUN_80102058 (rich: grab/flee)
 *   [3..7] FUN_80102540 / FUN_80102bd0 / FUN_80102d20 (movement/anim, deferred)
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

/* ACTIVE state = FUN_8011d9f4: dispatch on grid_id & 0xf. For sub 0 (the live STAGE1
 * routing) this is FUN_8011da48's +0x5 logic/anim double-dispatch; subs 1..15 are the
 * generic-humanoid EXE leaves (deferred). Returns the resolved sub index (grid_id & 0xf).
 * The leaf BODIES are not executed yet — see the module header. */
int re15_enemy_ai_active(int slot);

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

#endif /* RE15_ENEMY_AI_H */
