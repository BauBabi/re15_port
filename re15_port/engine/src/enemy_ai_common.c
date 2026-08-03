/*
 * RE1.5 Rebuilt — Enemy-AI FSM dispatch + decision brain (Phase 2).
 *
 * Byte-true port of the STAGE1 zombie AI's dispatch foundation + the confirmed
 * decision handlers. Every constant cites either the decompiled overlay
 * (RE_15_Quellcode_Overlays/STAGE1_full) or the raw STAGE1.BIN pointer tables
 * (load @0x80100000, no header — file offset = addr-0x80100000; tables decoded
 * 2026-06-29). See re15_enemy_ai.h for the full table map.
 *
 * Scope (Phase 2): the named state model, INIT (FUN_8011d84c), the tick entry +
 * main-state dispatch (FUN_8011d6d4), the ACTIVE sub-dispatch (FUN_8011d9f4), and
 * the byte-true decision handlers FUN_80101b64 / FUN_80101c7c / FUN_80101de4.
 * DEFERRED (cited, not guessed): the generic-humanoid EXE leaves (PTR_FUN_801217b4
 * [1..15] = 0x8004f../0x80050../0x80051.., the movement/anim execution), the +0x5
 * logic/anim leaves inside FUN_8011da48, FUN_80102058 (the directional grab + low-HP
 * flee — needs the FUN_8001a780 octant + the DAT_800acad6/DAT_800acae7 game globals),
 * and the HURT/DEATH/IDLE main-state bodies. Nothing here is wired into game_step yet
 * (no tick side effects → no 1170 risk; same stance as re15_damage.c).
 */
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_enemy.h"    /* re15_enemy_find — the loaded model bank (death-clip framecount) */
#include "re15_audio.h"    /* re15_audio_room_se — zombie combat SEs on snd1 (func_0x800453d0):
                            * grab-start 4, grab-release 7 (FUN_80102548), death groan 5/8 (FUN_80107cb0 f7) */
#include "re15_esp.h"      /* re15_esp_fx_spawn — the collapse frame-0x37 blood burst */
#include "re15_collision.h" /* re15_collision_constrain — the enemy SCA wall clamp (m0 root
                             * @0x8010062c jal 0x8003b0a4, runs per tick after the state handler) */
#include "re15_room.h"      /* g_room_rdt + g_current_room_id (em-status stage zone) */
#include "re15_scd.h"       /* re15_game_flag_get/set — the em-status kill-flag persistence */
#include "re15_math.h"     /* re15_squareroot0 — the engine's ONLY sqrt (BIOS 0x80065f60) */
#include "re15_damage.h"   /* re15_enemy_player_dist, re15_ai_arc_test, re15_engine_rand8,
                            * re15_enemy_apply_hitbox */
#include "re15_skeleton.h" /* re15_sin_q12 / re15_cos_q12 — forward-walk root-motion step (8.19) */
#include "re15_actor.h"    /* re15_atan2_q12 — heading toward the player for the approach/walk */
#include "re15_anim_select.h" /* re15_compute_actor_kf — current keyframe for the walk root-motion */
#include "re15_emd.h"      /* re15_emd_get_keyframe_speed — the walk clip's per-frame root translation */
#include <stdio.h>
#include <stdlib.h>        /* getenv — RE15_NPC_TURN_TEST diagnostic seed */

/* Engine-wide AI freeze = DAT_800aca40 & 0x20000000 (FUN_8011d6d4 gate). */
static int s_ai_paused = 0;
void re15_enemy_ai_set_paused(int paused) { s_ai_paused = paused ? 1 : 0; }

void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);                 /* fwd */
static void re15_enemy_footlock_step(int slot, re15_actor_t *e);                                /* fwd */
static uint8_t s_zfoot_ok[RE15_ACTOR_MAX];                     /* tentative def (full def below) */
static uint8_t s_wander_mag[RE15_ACTOR_MAX];                   /* tentative def (full def below) */
static uint8_t s_wander_idx[RE15_ACTOR_MAX];                   /* tentative def (full def below) */
/* FSM-CLOCK clip-end signal (defined below, used by the feeding stand-up + grab + death sub-FSMs). */
static int re15_enemy_clip_done(const re15_actor_t *e);

/* The 32-bit state word the decision handlers store at entity+0x4, split into the
 * port's per-byte state fields (state=+0x4, sub_state_1=+0x5, sub_state_2=+0x6,
 * sub_state_3=+0x7). Byte-true to `*(undefined4 *)(entity + 4) = word` (LE). */
void re15_ai_set_state_word(re15_actor_t *e, uint32_t word)
{
    if (!e) return;
    e->state       = (uint8_t)( word        & 0xff);
    e->sub_state_1 = (uint8_t)((word >>  8) & 0xff);
    e->sub_state_2 = (uint8_t)((word >> 16) & 0xff);
    e->sub_state_3 = (uint8_t)((word >> 24) & 0xff);
}

/* INIT — FUN_8011d84c (STAGE1_full). The spawn/setup main-state. Byte-true map:
 *   +0x4   = 1            -> state ACTIVE
 *   +0x0  |= 0x40000000   (a lifecycle flag word; the port has no +0x0 word repr)
 *   +0x78  = hitbox ptr   -> re15_enemy_apply_hitbox (PTR_DAT_8012179c = the per-type
 *                            struct; for 0x47 = 450/1530, confirmed in STAGE1.BIN data)
 *   +0x1a8 = &player      -> implicit (port slot 0 is always the player)
 *   +0x93 = 0  (hit guard), +0x8f = 0 (anim FRAC), +0x95 = 0 (frame index)
 *   model pool +0x5fc = 0x2c8 (attack arc), +0x5f8 = 0x40, +0x5fa = 0x30, +0x5fe = 0x138,
 *              +0x5f4 = 0, +0x5f6 = 0   -> the ai_* param fields (0 by spawn memset)
 *   if (+0x9 & 0x40) { +0x4 = 4 (IDLE); +0x5 = 6 }   -> stationary briefing zombie
 * DEFERRED (cited, no port field yet): +0x9a = 0xffff, +0x9e = 0x78, +0x1b8 = 0,
 *   +0x1b9 = 8, +0x94 = 2, the anim seed FUN_8001f314, the light FUN_8001af5c.
 *   ai_timer (+0x9c) is NOT seeded here — the original seeds it in a sub-handler. */
void re15_enemy_ai_init(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];

    e->state = (uint8_t)RE15_AI_STATE_ACTIVE;   /* +0x4 = 1 */
    re15_enemy_apply_hitbox(e, e->type);        /* +0x78 = per-type hitbox struct */
    e->hit_react  = 0;                          /* +0x93 = 0 */
    e->anim_frac  = 0;                           /* +0x8f = 0 */
    e->anim_frame = 0;                           /* +0x95 = 0 */
    e->ai_arc  = 0x2c8;                          /* model pool +0x5fc */
    e->ai_p5f8 = 0x40;                           /* +0x5f8 */
    e->ai_p5fa = 0x30;                           /* +0x5fa */
    e->ai_p5fe = 0x138;                          /* +0x5fe */

    if (e->grid_id & RE15_AI_GRID_STATIONARY) {  /* +0x9 & 0x40 */
        e->state       = (uint8_t)RE15_AI_STATE_IDLE;  /* +0x4 = 4 */
        e->sub_state_1 = 6;                            /* +0x5 = 6 */
    }
}

/* ACTIVE — FUN_8011d9f4: dispatch on grid_id & 0xf into PTR_FUN_801217b4. Sub 0 is
 * FUN_8011da48 (the +0x5 logic/anim double-dispatch — the routing the live STAGE1
 * zombies use, grid_id low nibble 0); subs 1..15 are the generic-humanoid EXE leaves.
 * The leaf BODIES (movement/anim) + the FUN_8011d9f4 tail func_0x80012aa4(3000) are
 * DEFERRED — this returns the resolved sub index so the routing is observable/testable
 * without executing un-ported code. The decision brain (below) is what those leaves
 * call to pick the next state; it is exposed directly for now. */
int re15_enemy_ai_active(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return -1;
    re15_actor_t *e = &g_actors[slot];
    int sub = e->grid_id & RE15_AI_GRID_SUB_MASK;
    /* sub 0 = FUN_8011da48 (the live STAGE1 path): run its LOGIC dispatch on +0x5 (the
     * ported assess/search/turn decisions; +0x5>=3 deferred). The companion anim dispatch
     * SUB[4+(+0x5)] + the subs 1..15 generic leaves are deferred. The decision only mutates
     * +0x5/+0x6 (no external side effects) so this is safe to run from the tick. */
    if (sub == 0)
        re15_ai_exe_dispatch(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
    return sub;
}

/* TICK — FUN_8011d6d4 entry. Honour the gate, cache the player distance @+0x1d0, then
 * dispatch the main state. Returns 1 if dispatched, 0 if the gate skipped this enemy. */
int re15_enemy_ai_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;   /* slot 0 = player, never an AI */
    re15_actor_t *e = &g_actors[slot];
    if (!e->active) return 0;

    /* gate: ((DAT_800aca40 & 0x20000000)==0) && ((entity+0x9 & 0x20)==0). If either is
     * set the gated block (dist + dispatch) is skipped; the original still runs the
     * always-on tail func_0x8001b064(+0xb0, hp@+0x1ba) — a health/render helper, deferred. */
    if (s_ai_paused) return 0;
    if (e->grid_id & RE15_AI_GRID_SKIP) return 0;

    /* cache dist @+0x1d0 (byte-true: 16-bit-wrapped ΔX/ΔZ, SquareRoot0). */
    e->ai_dist = (uint32_t)re15_enemy_player_dist(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
    /* func_0x8001bd60(-10,20) setup helper — deferred. func_0x80039e7c(&player,0,0) steer-writer is RESOLVED, IMPLEMENTED as re15_nav_update_steer (live tick @L2517/2523). */

    /* main-state dispatch (*PTR_FUN_801217a0[entity+0x4])(). */
    switch (e->state) {
        case RE15_AI_STATE_INIT:   re15_enemy_ai_init(slot);   break;
        case RE15_AI_STATE_ACTIVE: re15_enemy_ai_active(slot); break;
        case RE15_AI_STATE_HURT:   /* FUN_8011db40 — body not decoded, deferred */ break;
        case RE15_AI_STATE_DEATH:  /* FUN_8011db88 — body not decoded, deferred */ break;
        case RE15_AI_STATE_IDLE:   /* 0x80050be8 (EXE) — deferred                */ break;
        default: break;
    }
    /* post-dispatch FUN_8002b498/FUN_8002aec4/FUN_8002b544/... collision+render — other
     * subsystems, run by game_step in a later phase; not here. */
    return 1;
}

/* The per-enemy per-frame integration step (the entry game_step will call): the FSM tick
 * (decision) + the lunge slice of the action driver (fires the hitbox while a lunge is
 * active). re15_enemy_lunge_tick is the FUN_80019e20 lunge action (re15_damage.c). */
int re15_enemy_ai_step(int slot)
{
    int r = re15_enemy_ai_tick(slot);   /* FUN_8011d6d4 — gate, dist, FSM/decision */
    re15_enemy_lunge_tick(slot);        /* FUN_80019e20 lunge slice — hitbox per active frame */
    return r;
}

/* ============ System (A): the LIVE STAGE1 EXE-leaf path (dynamically confirmed) ======= *
 * Resolved empirically from stage_saves/mzd_stage1_combat_death.sav: the overlay dispatch
 * tables (PTR_FUN_801217a0/b4) are NOT patched at runtime (live RAM == on-disc STAGE1.BIN),
 * and the active type-0x10 zombies are state +0x4=1, sub +0x9=0, anim +0x5 in {1,2,6,12}.
 * So the live path is FUN_8011d9f4 -> FUN_8011da48 -> logic = SUB[1+(+0x5)] = the EXE
 * generic-humanoid leaves (0x8004f.., disassembled from PSX.EXE), NOT the per-mode overlay
 * brain (System B) which a different AI mode/type uses. FUN_8004f100 (the +0x5=0 leaf) is
 * ported here; the remaining live leaves (+0x5 = 1/2/6/12 = 0x8004f3a4/5e8/0x80050cb8/
 * 0x800517f0 + their anim siblings SUB[4+k]) are the next slice (some touch the model pool
 * -> the skeleton-mapped, deferred part). */

/* DAT_800aca52 & 1 — a STAGE1 global flag the assess leaf reads (only for type 0x4b). */
static int s_ai_global_flag = 0;
void re15_enemy_ai_set_global_flag(int v) { s_ai_global_flag = v ? 1 : 0; }

/* FUN_8004f100 (PSX.EXE) — the +0x5=0 assess leaf. Instruction map:
 *   8004f118  sltiu (dist < 0x5dd) ; 8004f11c bne -> if dist>=0x5dd: +0x5=1, +0x6=0
 *   8004f148  jal FUN_8001ab9c(playerX, playerZ, 0x4b0)  (= re15_ai_arc_test, cone 0x4b0)
 *   8004f150  if result != 0 (player outside the arc): +0x5=2, +0x6=0
 *   8004f17c  lhu DAT_800aca52 & 1 ; if set && type(+0x8)==0x4b: +0x5=6, +0x6=0
 *   8004f1c4  lbu DAT_800acae7 (player+0x93 hit_react) ; if != 0: +0x5=6, +0x6=0 */
void re15_ai_exe_assess(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    if (e->ai_dist >= 0x5ddu)        { e->sub_state_1 = 1; e->sub_state_2 = 0; }   /* far -> search */
    if (re15_ai_arc_test(e, player->x, player->z, 0x4b0) != 0)                     /* outside ±0x4b0 */
                                     { e->sub_state_1 = 2; e->sub_state_2 = 0; }   /* turn to face */
    if (s_ai_global_flag && e->type == 0x4b)
                                     { e->sub_state_1 = 6; e->sub_state_2 = 0; }
    if (player->hit_react != 0)      { e->sub_state_1 = 6; e->sub_state_2 = 0; }   /* react to hit */
}

/* FUN_8004f3a4 (PSX.EXE) — the +0x5=1 "search" leaf. Instruction map:
 *   8004f3bc  sltiu dist<0x1f4 ; if dist<500: +0x5=3, +0x6=1   (very close -> close-in)
 *   8004f3f0  jal arc_test(ai_target_x, ai_target_z, 0x400) ; if !=0: +0x5=3, +0x6=1
 *   8004f434  sltiu dist<0xbb9 ; if dist>=3001: +0x5=5, +0x6=0  (lost the player)
 *   8004f458  flag52&1 && type==0x4b -> +0x5=6 ; 8004f4a0 player.hit_react -> +0x5=6 */
void re15_ai_exe_search(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    if (e->ai_dist < 0x1f4u)            { e->sub_state_1 = 3; e->sub_state_2 = 1; }
    if (re15_ai_arc_test(e, e->ai_target_x, e->ai_target_z, 0x400) != 0)
                                        { e->sub_state_1 = 3; e->sub_state_2 = 1; }
    if (e->ai_dist >= 0xbb9u)           { e->sub_state_1 = 5; e->sub_state_2 = 0; }
    if (s_ai_global_flag && e->type == 0x4b) { e->sub_state_1 = 6; e->sub_state_2 = 0; }
    if (player->hit_react != 0)         { e->sub_state_1 = 6; e->sub_state_2 = 0; }
}

/* FUN_8004f5e8 (PSX.EXE) — the +0x5=2 "turn-to-face" leaf. Instruction map:
 *   8004f600  jal arc_test(playerX, playerZ, 0x40) ; if ==0 (dead ahead): +0x5=0, +0x6=0
 *   8004f644  sltiu dist<0x7d1 ; if dist>=2001: +0x5=1, +0x6=1
 *   8004f668  flag52&1 && type==0x4b -> +0x5=6 ; 8004f6b0 player.hit_react -> +0x5=6 */
void re15_ai_exe_turn(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    if (re15_ai_arc_test(e, player->x, player->z, 0x40) == 0)
                                        { e->sub_state_1 = 0; e->sub_state_2 = 0; }
    if (e->ai_dist >= 0x7d1u)           { e->sub_state_1 = 1; e->sub_state_2 = 1; }
    if (s_ai_global_flag && e->type == 0x4b) { e->sub_state_1 = 6; e->sub_state_2 = 0; }
    if (player->hit_react != 0)         { e->sub_state_1 = 6; e->sub_state_2 = 0; }
}

/* FUN_8011da48 logic dispatch (live System-A): +0x5 -> the EXE leaf. */
void re15_ai_exe_dispatch(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    switch (e->sub_state_1) {                                    /* +0x5 */
        case 0: re15_ai_exe_assess(e, player); break;            /* SUB[1]=0x8004f100 */
        case 1: re15_ai_exe_search(e, player); break;            /* SUB[2]=0x8004f3a4 */
        case 2: re15_ai_exe_turn(e, player);   break;            /* SUB[3]=0x8004f5e8 */
        default:                                                 /* SUB[4+]=movement/anim */
            /* +0x5>=3: anim_set/walker/model-pool leaves — DEFERRED (port-skeleton). */
            break;
    }
}

/* ================= System (B): parallel decision brain (per-mode vtable[0..1]) ======== *
 * func_0x8001a9cc(&player, cone) = re15_ai_arc_test(e, player->x, player->z, cone): 0 if
 * the player is inside the ±cone front arc, else ±cone. func_0x8001af20() = the shared
 * RNG (re15_engine_rand8). The state word is stored at +0x4 via re15_ai_set_state_word. */

/* FUN_80101b64 (STAGE1_full) — search with a countdown timer. */
void re15_ai_decide_search_timer(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    int narrow = re15_ai_arc_test(e, player->x, player->z, 0x2c8);   /* sVar2 */
    int wide   = re15_ai_arc_test(e, player->x, player->z, 0x5f4);   /* sVar3 */

    int16_t t = e->ai_timer;                 /* +0x9c */
    e->ai_timer = (int16_t)(t - 1);
    if (t == 0) re15_ai_set_state_word(e, 0x101);

    if (e->ai_dist < 2000u && narrow != 0)   /* dist<0x7d0 && off narrow front arc */
        re15_ai_set_state_word(e, 0x701);    /* attack-commit */

    if (wide == 0 && (e->ai_flags & 0x10) != 0) {      /* in wide arc + approach gate */
        re15_ai_set_state_word(e, 0x201);              /* approach (comma side effect) */
        if (e->ai_dist > 10000u && (re15_engine_rand8() & 7) == 0)
            re15_ai_set_state_word(e, 0x801);          /* wander (1/8, only if far) */
    }
}

/* FUN_80101c7c (STAGE1_full) — approach-only (no attack/timer). */
void re15_ai_decide_approach(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    int wide = re15_ai_arc_test(e, player->x, player->z, 0x5f4);

    if (wide == 0 && (e->ai_flags & 0x10) != 0 && e->ai_dist < 4000u) {
        re15_ai_set_state_word(e, 0x201);
        if ((re15_engine_rand8() & 7) == 0)
            re15_ai_set_state_word(e, 0x801);
    }
}

/* FUN_80101de4 (STAGE1_full) — search, no timer, adds the +0x1c4 anim-flag override. */
void re15_ai_decide_search(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    int narrow = re15_ai_arc_test(e, player->x, player->z, 0x2c8);
    int wide   = re15_ai_arc_test(e, player->x, player->z, 0x5f4);

    if (e->ai_dist < 2000u && narrow != 0)
        re15_ai_set_state_word(e, 0x701);

    if (wide == 0 && (e->ai_flags & 0x10) != 0) {
        re15_ai_set_state_word(e, 0x201);
        if (e->ai_dist > 10000u && (re15_engine_rand8() & 3) == 0)   /* mask &3 (not &7) */
            re15_ai_set_state_word(e, 0x801);
    }

    if ((e->anim_flags & 0x1000) != 0)        /* entity+0x1c4 & 0x1000 */
        re15_ai_set_state_word(e, 0x1001);
}

/* FUN_8001a780 — relative-facing octant. param_1 = the other actor; uses the current
 * entity (DAT_800ac784). 1 if the other's heading is within ±0x800 (the front
 * hemisphere) of ours: ((other.rot_y - e.rot_y + 0x400) & 0xfff) < 0x800. */
int re15_ai_facing_aligned(const re15_actor_t *e, const re15_actor_t *other)
{
    if (!e || !other) return 0;
    return ((((int32_t)other->rot_y - (int32_t)e->rot_y) + 0x400) & 0xfff) < 0x800;
}

/* FUN_80102058 (STAGE1_full) — the rich engage decision (vtable[2] = +0x5=2). */
void re15_ai_decide_engage(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;

    /* PORT OPTION (no original): with the RE2 AI flavor selected, THIS is the function RE2 replaces.
     * re15_ai_decide_engage is the RE1.5 DECIDE half for the walking engage substates (f840[2] and
     * f840[0x13]); RE2's counterpart is DECISION[1] @0x80101714, dispatched from 0x8010118C through
     * the table @0x8010C88C. So the RE2 ladder runs INSTEAD of the RE1.5 decision, not alongside it —
     * running both would be a chimera, and the RE1.5 arm would keep committing grabs the RE2 ladder
     * did not choose (measured: it did exactly that, which is why this moved here).
     *
     * Today the ladder can only ever commit 0x0301 = the side GRAB; every other block is gated on a
     * field with no proven producer in the port and is filled with a proven zero rather than a guess
     * (see re15_re2z_fill_gates). 0x0301 maps 1:1 onto the port's own byte-true grab: RE1.5 commits
     * with the same sub_state_1 = 3 (@case 7 below), so the RE2 DECISION drives the port's existing,
     * already byte-true grab EXECUTOR. Words other than 0x0301 are ignored on purpose — RE2's
     * sub_states 10/12/14 mean different things in RE1.5, so applying them would corrupt the state
     * machine instead of porting anything. */
    if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && re15_re2z_owns_type(e->type)) {
        re15_re2z_decision_t rd;
        int committed = re15_re2z_walk_decide(e, player, re15_player_is_grabbed(), &rd);
        if (getenv("RE15_RE2_TRACE") && (committed || e->ai_dist < 1500u)) {
            re15_re2z_gates_t gg;
            re15_re2z_fill_gates(e, player, re15_player_is_grabbed(), &gg);
            fprintf(stderr, "[re2z] gates d=%u flo=%u/%u claimed=%u g1=%d g2=%d -> %s 0x%08x\n",
                    gg.dist, gg.self_106, gg.pl_106, gg.pl_1d3, gg.g1_sector_hit, gg.g2_sector_hit,
                    committed ? "COMMIT" : "none", rd.word);
        }
        if (committed && rd.word == 0x00000301u)
            re15_ai_set_state_word(e, rd.word);            /* sw @0x80101958 / @0x8010199c */
        return;                                            /* RE2's ladder is the WHOLE decision */
    }

    uint8_t contact = e->ai_contact;                       /* bVar1 = entity+0x90 */

    /* contact-direction gate: (bVar1&0xf0)*0x10 = the packed contact heading; relative to
     * our facing (+0x6a), +0x200, &0xfff. NORMAL branch unless we're in firm contact
     * (bVar1&3) AND that contact is from ~ahead (the &0xfff result <= 0x3ff). */
    int32_t dir = (int32_t)(contact & 0xf0) * 0x10;        /* (bVar1 & 0xf0) * 0x10 */
    uint32_t off = (uint32_t)((dir - (int32_t)e->rot_y) + 0x200) & 0xfff;

    if ((contact & 3) == 0 || off > 0x3ff) {
        /* attack-commit (dist<0x7d0 && off the narrow front arc). */
        if (e->ai_dist < 2000u && re15_ai_arc_test(e, player->x, player->z, 0x2c8) != 0)
            re15_ai_set_state_word(e, 0x701);

        /* directional grab: player not mid-hit, very close (<0x4b0), inside the 0x200
         * front cone, on the SAME floor band -> 0x301 (face-to-face) / 0x401 (from behind). */
        if (player->hit_react == 0 &&                      /* DAT_800acae7 = player+0x93 */
            e->ai_dist < 0x4b0u &&
            re15_ai_arc_test(e, player->x, player->z, 0x200) == 0 &&
            player->floor == e->floor) {                   /* DAT_800acad6 = player+0x82 */
            int aligned = re15_ai_facing_aligned(e, player);
            re15_ai_set_state_word(e, (uint32_t)((aligned + 3) * 0x100) | 1u);
        }

        /* player-dead grab: close (<0x5dc) && player HP < 0 (DAT_800acaee = player+0x9a). */
        if (e->ai_dist < 0x5dcu && player->hp < 0)
            re15_ai_set_state_word(e, 0xc01);

        if ((e->anim_flags & 0x1000) != 0)                 /* entity+0x1c4 & 0x1000 */
            re15_ai_set_state_word(e, 0x1001);
    } else {
        /* firm contact from the front: the contact reaction (0x901 / 0xa01). */
        re15_ai_set_state_word(e, (uint32_t)(((contact & 1) + 9) * 0x100) | 1u);
    }
}

/* The +0x5 decision dispatch = FUN_8010168c's PTR_FUN_8011f840[entity+0x5] call (the DECIDE half).
 * Routes the active sub-mode to its byte-true decision handler. f840[3..6] are `jr ra` stubs (the
 * grab/walk states do all their work in the f890 ANIMATE half); f840[7] is the turn-state's
 * grab-commit check; indices 8.. are the deferred leaves. See re15_enemy_ai.h. */
void re15_ai_dispatch_decision(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    switch (e->sub_state_1) {                              /* entity+0x5 */
        case 0: re15_ai_decide_search_timer(e, player); break;  /* f840[0]=FUN_80101b64 */
        case 1: re15_ai_decide_search(e, player);       break;  /* f840[1]=FUN_80101de4 */
        case 2: re15_ai_decide_engage(e, player);       break;  /* f840[2]=FUN_80102058 */
        case 0x13: re15_ai_decide_engage(e, player);    break;  /* f840[0x13]=FUN_8010561c == the engage
                                                                 * decide: while APPROACHING, commit the
                                                                 * grab/turn once in range (8.19). */
        case 0x0c:  /* feeding decide FUN_801048a8 (@0x8011f840[0xc]): dist<0xbb8 (3000) && player
                     * alive -> +0x4=0xd01 (+0x5=0xd stand-up). The byte-true wake for a spawn-feeder. */
            if (e->ai_dist < 0xbb8u && player->hp >= 0)
                re15_ai_set_state_word(e, 0xd01);
            break;
        case 0x0d: break;                                       /* standup decide FUN_80104a48 = jr ra stub */
        case 0x11: break;                                       /* knockdown decide @0x8011f840[0x11] =
                                                                 * 0x80105124 = jr ra stub (byte-verified) */
        case 0x0b: break;                                       /* push-off decide @0x8011f840[0xb] =
                                                                 * 0x80103854 = jr ra stub (byte-verified) */
        case 9:     /* stagger decide @0x8011f840[9] = 0x801031a4 (raw-disasm'd, exact): while the
                     * push flag +0x1c0&0x8000 is HELD the 0x17/0x18 clips play; once clear, any
                     * sub-step <4 jumps to the 0x19 recover. (No +0x1c0 writer in the port yet ->
                     * the flag reads clear -> immediate recover; the writer = audit cluster F.) */
            if (e->sub_state_2 < 4)
                e->sub_state_2 = 4;
            break;
        case 0x0a: break;                                       /* edge-fall decide @0x8011f840[0xa] =
                                                                 * 0x801033c0 = jr ra stub (byte-verified) */
        case 0x12:  /* sleeping decide @0x8011f840[0x12] = 0x80105470 (raw-disasm'd): wake +0x6 1->2
                     * on the grid trigger (+0x9&0x1f)==0xf OR proximity dist<0xbb8 && player alive. */
            if (e->sub_state_2 == 1 &&
                (((e->grid_id & 0x1f) == 0x0f) || (e->ai_dist < 0xbb8u && player->hp >= 0)))
                e->sub_state_2 = 2;
            break;
        case 8: {   /* charge decide @0x8011f840[8] = FUN_80102f1c (decompile-exact): the engage
                     * contact + grab gates, WITHOUT the turn/dead-feed/0x1001 commits. */
            uint8_t contact = e->ai_contact;
            int32_t dir = (int32_t)(contact & 0xf0) * 0x10;
            uint32_t off = (uint32_t)((dir - (int32_t)e->rot_y) + 0x200) & 0xfff;
            if ((contact & 3) == 0 || off > 0x3ff) {
                if (player->hit_react == 0 && e->ai_dist < 0x4b0u &&
                    re15_ai_arc_test(e, player->x, player->z, 0x200) == 0 &&
                    player->floor == e->floor) {
                    int aligned = re15_ai_facing_aligned(e, player);
                    re15_ai_set_state_word(e, (uint32_t)((aligned + 3) * 0x100) | 1u);
                }
            } else {
                re15_ai_set_state_word(e, (uint32_t)(((contact & 1) + 9) * 0x100) | 1u);
            }
            break;
        }
        case 5: case 6:
            /* +0x5=5/6 = the DEVOUR-FINISH (D3/D5 disasm 2026-07-03): its DECIDE f840[5]/[6] =
             * FUN_80102bd0 = `jr ra` — NOTHING re-routes it, ever (the killer stays inert over the
             * corpse; timeline: +0x5=6 for 20+s at d 214..636 while the ENGAGE-state zombies committed
             * the 0x5dc dead-feed). The old port routed this into decide_engage (a walk stand-in) —
             * removed: byte-true these states only run their ANIMATE (re15_enemy_ai_live_devour). */
            break;
        case 7:  /* f840[7]=FUN_80102d20 — the TURN state's grab-commit check (byte-true): once the
                  * turn (animate) has the zombie close + facing within the ±0x200 cone + same floor +
                  * the player not mid-hit, commit the GRAB (+0x5=3/4). Otherwise stay turning. This
                  * is the same condition as the engage's directional-grab arm (re15_ai_decide_engage). */
            if (player->hit_react == 0 && e->ai_dist < 0x4b0u &&
                re15_ai_arc_test(e, player->x, player->z, 0x200) == 0 &&
                player->floor == e->floor) {
                int aligned = re15_ai_facing_aligned(e, player);
                re15_ai_set_state_word(e, (uint32_t)((aligned + 3) * 0x100) | 1u);
            }
            break;
        default:                                                /* f840[3..6],[8..] */
            /* grab/walk stubs + the deferred movement/anim-execution leaves (model pool). */
            break;
    }
}

/* ============ System (C): the LIVE STAGE1 zombie AI (@0x8011f7b4 family) ============== *
 * CORRECTION (2026-06-29, savestate-PROVEN): the active STAGE1 zombies (type 0x10/0x11/0x16)
 * are NOT driven by the @0x801217a0 / FUN_8011d6d4 family ported above — that is the type-0x47
 * family. The live per-frame loop FUN_8001a50c (@0x8001ce04) dispatches @0x80072bac[entity+0x8
 * type] = FUN_80100424 (the per-frame tick: pause/skip gates, dist@+0x1d0, then dispatch
 * @0x8011f7b4[entity+0x4]). The @0x8011f7b4 table: [0]=FUN_80100688 (INIT), [1]=FUN_80101224
 * (active), [2]=0x80105a8c, [3]=0x80106ba4, [4]=0x8010919c. Decoded byte-true direct from
 * STAGE1.BIN (the RE_15_Quellcode_Overlays/STAGE1/FUN_80100424.c decompile is MIS-ANALYZED — do
 * NOT trust it). The lunge/hitbox/damage execution layer (re15_enemy_lunge_*, re15_enemy_attack)
 * is SHARED + correct; this re-roots the AI decision/tick onto the right table. NOT wired into
 * game_step yet (no 1170 risk). */

/* DAT_800aca3c & 1 — the live family's "combat active" gate (FUN_8010ab2c only arms the lunge
 * when set). 0 by default (the arm stays inert until combat is enabled / game_step wires the real
 * flag); the test + the eventual wiring set it. (DAT_800aca3c also carries the cut-freeze bit
 * 0x100 elsewhere — see reai-v2-cut-opcodes; bit 0x1 is the combat-active sub-flag here.) */
static int s_live_combat_active = 0;
void re15_enemy_ai_set_combat_active(int v) { s_live_combat_active = v ? 1 : 0; }

/* DAT_800aca58 == cmd 5 — the PLAYER-GRABBED latch (Phase 8.10). When a live zombie's grab
 * execution latches the player (FUN_80102548 sub-step 0 @0x80102640: `0x800aca58 = ((+0x5-3)
 * <<8)|5`), the player's command FSM (FUN_8002e... -> the per-command vtable @0x80073f90[cmd])
 * dispatches cmd 5 = LAB_80036834 (the grabbed handler @0x80036834). That handler pins the
 * player (sets the grabbed-Y reference DAT_800acc0e = -floor*1800 @0x80036880) and runs a
 * per-enemy-type grabbed POSE handler (@0x800ac758[enemy.type]) — it NEVER reads the pad, so
 * the player cannot steer or walk away while held (he takes the repeated -5 bites until death
 * or, in the original, the deferred struggle frees him). The port models the byte-true
 * OBSERVABLE effect — "while grabbed the player is pinned, the pad is ignored" — as this flag,
 * which game_step reads to skip re15_player_tick (the same engine-driven-no-steer stance the
 * stair traversal already uses). FAITHFUL-LINE: the original LATCHES cmd 5 (the grab sets it,
 * does NOT clear it at exit @0x80102b90 — the player grabbed-FSM resets it when the grabbed anim
 * completes / the struggle frees him, both deferred). The port instead re-derives the latch each
 * frame = "is a live zombie currently in the grab sub-mode" (set in re15_enemy_ai_live_grab,
 * cleared at the top of re15_enemy_ai_run_all). That is a faithful stand-in for the deferred
 * player grabbed-FSM release that avoids a permanent soft-lock: when the engage stops re-committing
 * the grab (player out of the ±0x4b0 cone / different floor / mid-hit), no zombie is in grab state
 * → the player is free. GRAB-PIN CLARIFIED (byte-verified 2026-07-13): there is NO player-XZ "pin/pull"
 * during a grab — a ghidra-wide scan for player-XZ writes (0x800aca88/0x800aca90) across the grab handlers
 * (devour-FSM 0x8010a28c, grabbed-handler cmd5 0x80036834, grab-face state 0x8011f840[3]) is EMPTY: the
 * player simply FREEZES at its grab-commit position (pad ignored). Any port-vs-PSX hold-distance gap is just
 * different commit distances, not a divergence. The Y-reference DAT_800acc0e (=-floor*1800) is set by cmd5
 * @0x80036880 and IS modeled; the devour-collapse root-motion drag (~600u) is modeled (see below, ~L523).
 * The per-type grabbed bone POSE/anim (victim-set bank2 keyframes) is IMPLEMENTED (@L497-803). DEFERRED
 * (cited): the struggle-escape
 * (sub-step 5 @0x80102968, anim-gated + the bit-0x2 check), and the cmd-6 "being-approached" walk command. */
/* MASH-ESCAPE input (byte-true FUN_80037024 @0x80037024, raw disasm): returns 1 when the press-EDGE
 * pad register (0x800ac762) has ANY D-pad direction (bits 4-7) or face button (bits 12-15) set —
 * mask 0xf0f0. The port's pad bits mirror the PSX layout 1:1, so the mask applies unchanged to
 * pad_pressed. game_step feeds the edge bits each tick (re15_enemy_ai_set_pad_pressed); the grab's
 * bite loop drains its escape window by 1 + 5*mash — mash fast enough and the window goes negative
 * (THROW-OFF, break free alive) before the 100-tick kill counter devours you. */
static uint16_t s_pad_pressed_edge = 0;
void re15_enemy_ai_set_pad_pressed(uint16_t edge_bits) { s_pad_pressed_edge = edge_bits; }
static int re15_mash_pressed(void) { return (s_pad_pressed_edge & 0xf0f0u) != 0; }

/* JUST-ESCAPED mercy window (byte-true DAT_800aca50 bit 0): the grab's THROW-OFF [4] sets the flag +
 * a 0x5a=90-tick timer on the thrower (+0x1d5, FUN_80102548 case 4 @line "DAT_800aca50 |= 1" +
 * "+0x1d5 = 0x5a"); the zombie tick FUN_8010a8c8 decrements it and CLEARS the flag at 0. While set,
 * a grab IMPACT seeds the escape window 5 instead of 0x6e -> a re-grab within 3s throws off almost
 * immediately (the post-escape mercy). Port: one global timer (grabs are exclusive, so at most one
 * thrower runs a timer), decremented in run_all. */
static int16_t s_grab_mercy_timer = 0;
static int re15_grab_mercy_active(void) { return s_grab_mercy_timer > 0; }

static int s_player_grabbed = 0;
int re15_player_is_grabbed(void)
{
    /* Pinned while a zombie holds him (cmd 5) AND through the victim struggle/release-finish anims
     * (the original's cmd-5 handler stays routed until the release clip restores the free player) AND
     * the devour COLLAPSE (cmd 6) — hp stays 70 until collapse frame 35, so the death branch cannot
     * pin those first 35 frames; the cmd-6 handler never reads the pad either. */
    extern int re15_player_victim_state(void);
    int vs = re15_player_victim_state();
    return s_player_grabbed || vs == 1 || vs == 2 || vs == 3;
}

/* ===================== LEON GRAB-VICTIM ANIMATION (state 5 struggle / state 6 collapse) ===========
 * Byte-true source: the player-command FSM on DAT_800aca58 (EXE PTR_LAB_80073f90[cmd]) — [5] grabbed
 * @0x8010a28c, [6] devoured @0x8010a6f8. LEON is animated NOT from his own PL00 set but from the
 * GRAB-VICTIM anim set the ZOMBIE carries (DAT_800acbcc/acbd0 = *(zombie+0x178)/+0x17c = the em<NN>
 * EMD bank 2), applied to the player via func_0x8001ad68 + func_0x8001f314. STATE 5 (grabbed,
 * @0x8010a28c): variant DAT_800aca59 (0 front / 1 back) -> Leon motion = variant*3+phase => struggle
 * clips {0,1,2}(front)/{3,4,5}(back); phase (DAT_800aca5a) advances ONLY on clip-done. STATE 6
 * (devoured, @0x8010a6f8): Leon motion = variant+6 => collapse clip {6}/{7}, played to its end.
 *
 * The port previously FROZE Leon during the grab/death (the "no Leon reactions / death finish
 * missing"). This drives Leon's motion/anim_frame off the grabbing zombie's bank 2 so he struggles
 * then collapses. g_player_victim: 0 none / 1 struggle / 2 collapse. Set by re15_enemy_ai_live_grab
 * (the grab that pins him), advanced by re15_player_victim_tick (game_step, after the enemy AI). */
static int     g_player_victim = 0;           /* 0 none / 1 struggle / 2 collapse / 3 release-finish */
static uint8_t g_player_victim_type = 0;      /* grabbing zombie type -> its bank 2 (skel/anim_victim) */
static uint8_t g_player_victim_variant = 0;   /* 0 face / 1 behind (from the grab facing +0x5-3) */
static uint8_t s_victim_phase = 0;            /* struggle phase (DAT_800aca5a model: 0 intro, >=1 hold) */
static uint8_t s_victim_fresh = 0;            /* clip (re)set THIS tick -> pose frame 0 before advancing
                                               * (byte-true f314 POST-increment: phase 0 poses +0x95=0 on
                                               * the latch tick @0x8010a398/a438; the old ++-first port
                                               * skipped frame 0 = the pair ran 1 authored frame apart) */
static int     g_player_victim_zombie = -1;   /* grabbing zombie actor slot -> its XZ for the turn-to-face */
int     re15_player_victim_state(void) { return g_player_victim; }
uint8_t re15_player_victim_type(void)  { return g_player_victim_type; }

/* CLIP ROOT MOTION (byte-true func_0x8001ad68 @0x8001ad68): each frame the entity position is the
 * anchor + the CURRENT keyframe's root-translation offset rotated by the entity yaw. Per-frame delta
 * form (identical to the corpse-walk port): pos += rotate(off[kf_now] - off[kf_prev], yaw); skip the
 * delta on a clip change / wrap (= the original's re-anchor). Applies the FULL (sx, sz) offset vector
 * with the local->world convention of the foot-lock (wx = cos*lx + sin*lz, wz = -sin*lx + cos*lz).
 * TIMELINE-VERIFIED (deterministic /tmp/tl3): the grabbed PAIR drifts together (~65 units/1.3s) and
 * the devour collapse DRAGS Leon ~600 units — both are this root motion, missing before. */
/* ANCHOR SET (byte-true FUN_8001ac38 @0x8001ac38): anchor(+0xa0/+0xa2) = pos - rotate(off[kf], yaw)
 * for the CURRENT clip/frame's keyframe root offset — so the subsequent absolute placements replay
 * the clip's authored trajectory from the current position. */
static void re15_clip_anchor_set(re15_actor_t *a,
                                 const re15_emd_skeleton_t *skel,
                                 const re15_emd_animation_t *anim, int clip, int frame)
{
    a->anchor_x = a->x; a->anchor_z = a->z;
    if (!skel || !anim || clip < 0 || clip >= anim->clip_count) return;
    const re15_emd_clip_t *c = &anim->clips[clip];
    if (c->frame_count <= 0) return;
    int slot = frame % c->frame_count;
    int kf = (int)(anim->frames[c->first_frame + slot] & 0xFFFu);
    int16_t sx, sy, sz;
    if (!re15_emd_get_keyframe_speed(skel, kf, &sx, &sy, &sz)) return;
    int32_t cs = re15_cos_q12(a->rot_y), sn = re15_sin_q12(a->rot_y);
    a->anchor_x = a->x - (int32_t)(( (int64_t)cs * sx + (int64_t)sn * sz) >> 12);
    a->anchor_z = a->z - (int32_t)((-(int64_t)sn * sx + (int64_t)cs * sz) >> 12);
}

/* ABSOLUTE clip root motion (byte-true func_0x8001ad68 @0x8001ad68 core: entity+0x34/+0x3c =
 * anchor(+0xa0/+0xa2) + rotate(off[kf], yaw)) — the placement the grab pair uses: with BOTH anchors
 * equal (the grab [0] copies the zombie anchor to the player) the authored clip offsets hold the
 * pair in the interlocking formation instead of drifting into each other. */
static void re15_clip_root_motion_abs(re15_actor_t *a,
                                      const re15_emd_skeleton_t *skel,
                                      const re15_emd_animation_t *anim, int clip, int frame)
{
    if (!skel || !anim || clip < 0 || clip >= anim->clip_count) return;
    const re15_emd_clip_t *c = &anim->clips[clip];
    if (c->frame_count <= 0) return;
    int slot = frame % c->frame_count;
    int kf = (int)(anim->frames[c->first_frame + slot] & 0xFFFu);
    int16_t sx, sy, sz;
    if (!re15_emd_get_keyframe_speed(skel, kf, &sx, &sy, &sz)) return;
    int32_t cs = re15_cos_q12(a->rot_y), sn = re15_sin_q12(a->rot_y);
    a->x = a->anchor_x + (int32_t)(( (int64_t)cs * sx + (int64_t)sn * sz) >> 12);
    a->z = a->anchor_z + (int32_t)((-(int64_t)sn * sx + (int64_t)cs * sz) >> 12);
}

static void re15_clip_root_motion_delta(re15_actor_t *a,
                                        const re15_emd_skeleton_t *skel,
                                        const re15_emd_animation_t *anim,
                                        int clip, int fr_now, int fr_prev)
{
    if (!skel || !anim || clip < 0 || clip >= anim->clip_count) return;
    const re15_emd_clip_t *c = &anim->clips[clip];
    if (c->frame_count <= 0) return;
    if (fr_prev < 0 || fr_now != fr_prev + 1) return;              /* clip-set/jump: re-anchor */
    /* The shared anim pass advances anim_frame MONOTONICALLY (it does not wrap at the clip length —
     * the renderer takes slot = frame % frame_count). Mirror that here: consecutive frames map to
     * consecutive LOOP SLOTS; a slot wrap (now < prev) is the loop restart = re-anchor (skip). */
    int s_now  = fr_now  % c->frame_count;
    int s_prev = fr_prev % c->frame_count;
    if (s_now != s_prev + 1) return;                               /* wrapped -> re-anchor */
    int kf_n = (int)(anim->frames[c->first_frame + s_now ] & 0xFFFu);
    int kf_p = (int)(anim->frames[c->first_frame + s_prev] & 0xFFFu);
    int16_t sx, sy, sz, sx0, sz0;
    if (!re15_emd_get_keyframe_speed(skel, kf_n, &sx,  &sy, &sz))  return;
    if (!re15_emd_get_keyframe_speed(skel, kf_p, &sx0, &sy, &sz0)) return;
    int32_t dx = (int32_t)sx - sx0, dz = (int32_t)sz - sz0;
    if (dx == 0 && dz == 0) return;
    int32_t cs = re15_cos_q12(a->rot_y), sn = re15_sin_q12(a->rot_y);
    a->x += (int32_t)(( (int64_t)cs * dx + (int64_t)sn * dz) >> 12);
    a->z += (int32_t)((-(int64_t)sn * dx + (int64_t)cs * dz) >> 12);
}

/* Called by the grab (re15_enemy_ai_live_grab) each frame it pins the player: latch the victim anim
 * state (which zombie's bank, face/behind) + enter the STRUGGLE. (The COLLAPSE is NOT keyed off hp
 * here — byte-true it is the DEVOUR state's sub0 latching player cmd 6, re15_player_victim_devour.) */
static void re15_player_victim_latch(const re15_actor_t *zombie, re15_actor_t *player)
{
    re15_enemy_bank_t *vb = re15_enemy_find(zombie->type);
    if (!vb || !vb->victim_ok) return;                  /* no victim bank -> keep old frozen behaviour */
    g_player_victim_type    = zombie->type;
    g_player_victim_zombie  = (int)(zombie - g_actors);  /* remember the grabber for the turn-to-face */
    g_player_victim_variant = (zombie->type == 0x21)
        ? 0   /* CROW: the normal-room grab writes aca59 = 0 (front FSM) @0x80113e30 — the
               * rear FSM (aca59=1 @0x801152ac) exists only in the ROOM1171 event scene */
        : (uint8_t)((zombie->sub_state_1 >= 4) ? 1 : 0);  /* +0x5=4 behind else face */
    if (g_player_victim == 0 || g_player_victim == 3) { /* enter (or re-enter from the release finish) */
        g_player_victim = 1;
        s_victim_phase  = 0;
        player->anim_frame = 0;
        s_victim_fresh  = 1;
        /* The grab cmd-5 write REPLACES the whole player command state — the per-frame dispatch
         * @0x80031c88 indexes DAT_800aca58, so a latched aim FSM (cmd 1 action 7) simply stops
         * being run. Exit the port's aim FSM here (keep the knife-in-hand latch aca50&0x4000 —
         * a separate flag the cmd write does not touch). crow_victim_anim.md §4.3. */
        {
            extern void re15_player_aim_interrupt(void);
            re15_player_aim_interrupt();
        }
        if (zombie->type == 0x21) {
            /* CROW front victim FSM Ph0 (@0x801159f4, Hook A [0x21] = LAB_8011597c):
             * blend +0x8f = 7 @0x80115a28 (the zombie grab is a HARD cut — the crow
             * BLENDS Leon in), hit_react |= 1 @0x80115a40-4c, aca3c |= 0xC0
             * @0x80115a3c-44 (control latch; port repr = the grab pin), and NO yaw
             * snap — there is no a8f8 call anywhere in 0x801159bc-0x80115b7c: Leon
             * keeps his facing (crow_victim_anim.md F1). */
            player->anim_frac  = 7;
            player->hit_react |= 1;
        } else {
            /* ONE-SHOT yaw latch (byte-true phase 0, D1 disasm @0x8010a344-a3d4): a8f8(grabber pos,
             * ±0x800) snaps rot_y := bearing(Leon->zombie), face then adds 0x800 -> BOTH variants end at
             * bearing+0x800 = yaw(zombie->player) = the same value the zombie's own [0] snap writes
             * (live: pl_rot == zrot within -2..-6 in all 17 grab rows). NOT re-snapped per frame — the
             * only later yaw write is the release-exit ±0x800 fix-up (re15_player_victim_tick). */
            player->rot_y = (int16_t)(((int)re15_atan2_q12(player->z - zombie->z,
                                                           player->x - zombie->x) - 0x400) & 0x0fff);
            player->anim_frac = 0;   /* +0x8f := 0 (@0x8010a3a0) — Leon's grab-start pose is a HARD
                                      * cut; only the zombie blends (+0x8f=7 on its side) */
        }
    }
}

/* Victim-anim CLIP MAP per grabber type — which bank-2 clips the eaten player plays. Both the ZOMBIE
 * (EM10 bank2, 14 clips) and the DOG (EM020 bank2, 15-bone PL00 player set verified, 29 clips) carry a
 * player-eaten set, but with DIFFERENT layouts:
 *  - ZOMBIE (@0x8010a28c): intro = variant*3, hold = +1, release = +2; collapse = variant+6.
 *  - DOG (byte-true machine A @0x80111984): intro = 3*variant (@0x80111a0c), hold = 1 (@0x80111abc),
 *    release/kill = 2 (@0x80111b1c), collapse = 0xB (@0x80111bf4).
 * Default = the zombie layout, so the live-verified zombie grab is byte-identical (unchanged). */
static void re15_victim_clip_map(uint8_t *c_intro, uint8_t *c_hold, uint8_t *c_release, uint8_t *c_collapse)
{
    uint8_t v = g_player_victim_variant;
    if (g_player_victim_type == 0x20) {                 /* DOG (EM020) */
        *c_intro = (uint8_t)(3 * v); *c_hold = 1; *c_release = 2; *c_collapse = 0x0b;
    } else if (g_player_victim_type == 0x21) {          /* CROW (EM021) — front victim FSM
                                                         * 0x801159bc (Hook A [0x21]): intro clip 0
                                                         * @0x80115a18 (14f), hold LOOP clip 1
                                                         * @0x80115a88/@0x80115aa8 (36f, no
                                                         * self-advance), release clip 2 @0x80115ad4
                                                         * (20f) — EM021 victim EDD @blob+0x49d4.
                                                         * NO collapse: the crow never devours
                                                         * (Hook B [0x21] table [0] = jr-ra stub
                                                         * @0x80115d6c, no cmd-6 writer —
                                                         * crow_victim_anim.md F2); unreachable. */
        *c_intro = 0; *c_hold = 1; *c_release = 2; *c_collapse = 2;
    } else {                                            /* ZOMBIE (default) */
        uint8_t base = (uint8_t)(v * 3);
        *c_intro = base; *c_hold = (uint8_t)(base + 1);
        *c_release = (uint8_t)(base + 2); *c_collapse = (uint8_t)(v + 6);
    }
}

/* Spieler-Bone-Weltposition in der VICTIM-Pose (bite_blood_fx.md D2): posiert aus der
 * victim-Bank des Grabbers (skel_victim/anim_victim) — exakt die Bank, aus der der Port
 * Leon im Grab/Devour animiert. Bone 8 = +0x5A0 = 172*8+0x40 (@0x8010a570/@0x8010a7a4).
 * Fallback (kein Bank/headless) = Spieler-Root. */
static void re15_player_victim_bone_pos(int bone, int32_t out[3])
{
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    out[0] = player->x; out[1] = player->y; out[2] = player->z;
    re15_enemy_bank_t *vb = re15_enemy_find(g_player_victim_type);
    if (!vb || !vb->victim_ok || bone < 0 || bone >= vb->skel_victim.bone_count) return;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vb->skel_victim, player, -1,
                                   (uint32_t)player->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;                     /* Pose-QUERY, kein Render (kein Crossfade) */
    if (re15_skel_compute_pose(&vb->skel_victim, kf, poses) != 0) return;
    re15_skel_bone_to_world(poses[bone].trans, player->rot_y,
                            player->x, player->y, player->z, out);
}

/* The zombie's THROW-OFF [4] starts the player's release finish in lockstep (byte-true: the grab's
 * escape path writes DAT_800aca5a = 4 = the struggle FSM's release phase; clip base+2). */
void re15_player_victim_throwoff(void)
{
    if (g_player_victim != 1) return;
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_player_victim = 3;
    uint8_t vi, vh, vr, vc; re15_victim_clip_map(&vi, &vh, &vr, &vc);
    player->motion = vr;                                /* release clip (zombie base+2 / dog 2) */
    player->anim_frame = 0;
    s_victim_fresh = 1;
    player->anim_frac = 7;            /* +0x8f=7 (@0x8010a594, D1 disasm) — Leon BLENDS struggle ->
                                       * release over ~8 frames (unlike the hard grab-start cut) */
    player->anim_blend_rate = 0x200;  /* phase-5 f314 rate (@0x8010a5ec: a3=0x200) */
}

/* The zombie's DEVOUR-FINISH sub0 (FUN_80102bd8 @0x80102c80) latches player cmd = ((+0x5)-5)<<8 | 6 —
 * the devoured COLLAPSE. Byte-true: the variant byte is the zombie's (+0x5)-5 (5 face / 6 behind) and
 * HP is NOT touched here (the -1 lands at collapse frame 0x23 in the cmd-6 handler @0x8010a80c). */
void re15_player_victim_devour(const re15_actor_t *zombie)
{
    re15_enemy_bank_t *vb = re15_enemy_find(zombie->type);
    if (!vb || !vb->victim_ok) {                        /* no victim bank -> keep the old kill plumbing */
        re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (player->hp >= 0) player->hp = -1;
        player->state = 7;
        return;
    }
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_player_victim_type    = zombie->type;
    g_player_victim_zombie  = (int)(zombie - g_actors);
    g_player_victim_variant = (uint8_t)((zombie->sub_state_1 >= 6) ? 1 : 0);  /* (+0x5)-5 */
    g_player_victim = 2;
    player->anim_frame = 0;                             /* @0x8010a75c frame reset on collapse entry */
    s_victim_fresh = 1;
    re15_audio_core_se(1);                              /* collapse-entry SE: Se_on(0x4010001) = CORE
                                                         * bank4 record 1 (FUN_8010a6f8 init) */
    {   /* Devour-ENTRY-Blut (FUN_8010a6f8: a0=0x1500 @0x8010a770, Anker Spieler-Bone 8
         * @0x8010a7a0/a4 = +0x5A0; bite_blood_fx.md D4a, CONFIRMED). */
        int32_t db8[3]; re15_player_victim_bone_pos(8, db8);
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                             db8[0], db8[1], db8[2], (int16_t)player->rot_y);
    }
}

/* Advance Leon's grab-victim animation one game tick (game_step, after re15_enemy_ai_run_all so the
 * grab has latched this frame). Frees Leon (state->0) when the grab stops (struggle only; the collapse
 * persists until the death sequence reloads). Byte-true: struggle phase advances on clip-done. */
void re15_player_victim_tick(void)
{
    if (g_player_victim == 0) return;
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_enemy_bank_t *vb = re15_enemy_find(g_player_victim_type);
    if (!vb || !vb->victim_ok) { g_player_victim = 0; return; }
    /* +0x8f blend counter decay — byte-true f314 decrements it once per call (FUN_8001f3bc line 78),
     * and the cmd-5/6 handlers call f314 every tick. The port's normal decay sites (re15_player_tick /
     * re15_actors_anim_advance slot>=1) BOTH skip the pinned player, so without this the throw-off
     * blend seed (7) never decayed = a non-terminating fade that leaked into the post-release idle.
     * Skipped on the seed tick (fresh) so the full 7 renders once — the same first-frame-87.5%
     * cadence the zombie side shows (F84 frac=7 in the pose dump). */
    if (player->anim_frac > 0 && !s_victim_fresh) player->anim_frac--;
    /* VICTIM YAW: latched ONE-SHOT at grab commit (see re15_player_victim_latch) — D1 disasm: the
     * a8f8 call exists ONLY in phase 0 (@0x8010a3a4); no per-frame re-snap anywhere in the cmd-5
     * handler. Live-verified: pl_rot constant for the whole hold (behind 1547, face 2009/3083/...),
     * == the zombie's snapped yaw within -2..-6. The only later write is the release-exit ±0x800
     * fix-up below. (The old per-frame θl:=θz copy was observably equal during the hold but wrong
     * in mechanism, and it kept overwriting the release-exit flip.) */
    uint8_t c_intro, c_hold, c_release, c_collapse;
    re15_victim_clip_map(&c_intro, &c_hold, &c_release, &c_collapse);
    if (g_player_victim == 1) {                         /* STRUGGLE (state 5, @0x8010a28c 6-phase machine) */
        if (!s_player_grabbed) {                        /* grab ended alive -> RELEASE finish (phases 4/5:
                                                         * clip release ONCE, then restore the free player) */
            g_player_victim = 3;
            player->motion = c_release;
            player->anim_frame = 0;
            s_victim_fresh = 1;
            if (g_player_victim_type != 0x21) {        /* crow Ph3 @0x80115acc-dc writes clip 2 + frame 0
                                                        * ONLY — no +0x8f seed (the Ph0 blend 7 has long
                                                        * decayed): hard cut into the release. */
                player->anim_frac = 7;                 /* +0x8f=7 @0x8010a594 — blend into the release */
                player->anim_blend_rate = 0x200;
            }
            return;
        }
        /* TIMELINE-VERIFIED (deterministic /tmp/tl3 + the Q4 disasm of @0x8010a28c): phase 0/1 play the
         * intro clip base+0 once; from phase 2 on the handler LOOPS clip base+1 (motion=4 observed with
         * DAT_800aca5a self-held at 3, anim frame wrapping 0->18->15). The old port held base+2 — that is
         * the RELEASE clip (@0x8010a4e8 = acaf3*3+2), wrong during the hold. */
        uint8_t clip = (s_victim_phase < 1) ? c_intro : c_hold;
        int fc = (clip < vb->anim_victim.clip_count) ? vb->anim_victim.clips[clip].frame_count : 1;
        if (fc < 1) fc = 1;
        if (!s_victim_fresh &&
            ++player->anim_frame >= fc) {              /* clip-done -> intro advances, hold clip LOOPS
                                                        * (advance SKIPPED on the latch tick: frame 0 is
                                                        * posed first — byte-true f314 post-increment) */
            player->anim_frame = 0;
            if (s_victim_phase < 1) s_victim_phase++;
            clip = (s_victim_phase < 1) ? c_intro : c_hold;
        }
        s_victim_fresh = 0;
        player->motion = clip;
        if (g_player_victim_type != 0x21)              /* the crow victim FSM 0x801159bc-0x80115b7c has
                                                        * NO func_0x8001ad68 root-motion placement — only
                                                        * f314 (@0x80115a6c/abc/b08): Leon holds his XZ
                                                        * during the peck pin (crow_victim_anim.md F1) */
            re15_clip_root_motion_abs(player, &vb->skel_victim, &vb->anim_victim,
                                      clip, (int)player->anim_frame);
    } else if (g_player_victim == 2) {                 /* COLLAPSE (state 6): collapse clip, play once +
                                                        * ROOT MOTION (the ~600-unit devour drag), hold last */
        uint8_t clip = c_collapse;
        player->motion = clip;
        int fc = (clip < vb->anim_victim.clip_count) ? vb->anim_victim.clips[clip].frame_count : 1;
        if (fc < 1) fc = 1;
        if (!s_victim_fresh && player->anim_frame < fc - 1)     /* frame 0 posed on the entry tick */
            player->anim_frame++;
        s_victim_fresh = 0;
        re15_clip_root_motion_abs(player, &vb->skel_victim, &vb->anim_victim,
                                  clip, (int)player->anim_frame);   /* placed EVERY tick (ad68 runs in
                                                                     * all cmd-6 frames), incl. entry f0
                                                                     * and the held last frame */
        /* HP = -1 EXACTLY at collapse anim frame 0x23=35 (byte-true FUN_8010a6f8 @0x8010a80c/814,
         * gate @0x8010a7e8 == 0x23) — a DIRECT SET in the cmd-6 handler, NOT a clamp in the damage
         * path; the original hands the devour off at hp=70 and this store makes the corpse -1
         * (every kill save reads exactly -1). The zombie collapse clips (6/7) run well past 0x23; the
         * DOG collapse clip 0xB can be shorter, so cap the kill frame to the clip end (robust, the dog
         * eaten-death always resolves to the corpse). */
        int kill_fr = 0x23; if (fc - 1 < kill_fr) kill_fr = fc - 1;
        if (player->anim_frame == kill_fr && player->hp >= 0) {
            player->hp    = -1;
            player->state = 7;                          /* the port's death FSM keys off hp<0/state 7 */
        }
        /* frame 0x37=55: the big BLOOD burst at the player pos+yaw (byte-true FUN_8010a6f8:
         * FUN_80019700(0x2000) = effect-id 0 — the SAME spawn the hurt-fx uses) + its SE
         * Se_on(0x4030001) = CORE bank4 record 3 (Se_on RE'd: FUN_80045024 top byte = bank,
         * bank4 = the resident CORE00.EDH table @0x801fbd00, RAM-matched). The frame-0x23 chomp
         * Se_on(0x2070001) is BANK 2 (*(DAT_800ac778+8), a room-state pointer) — still deferred.
         * (Dog clip 0xB may end before 0x37 -> cap to the clip end so the eaten burst still fires.) */
        int blood_fr = 0x37; if (fc - 1 < blood_fr) blood_fr = fc - 1;
        if (player->anim_frame == blood_fr) {
            /* KORRIGIERT (bite_blood_fx.md D4b/F3): a0 = 0x2000 (@0x8010a82c — der alte Spawn
             * lief mit Default-Scale 0x1000 = halb so gross) und Anker = Spieler-Bone 8
             * (@0x8010a84c = +0x5A0), nicht der Root. */
            int32_t bb8[3]; re15_player_victim_bone_pos(8, bb8);
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000,
                                 bb8[0], bb8[1], bb8[2], (int16_t)player->rot_y);
            re15_audio_core_se(3);
        }
    } else {                                           /* RELEASE finish (state 3): release clip once -> free.
                                                        * (Entered via re15_player_victim_throwoff — the
                                                        * zombie holds sub-steps [4..7] while this plays,
                                                        * so s_player_grabbed stays latched; no re-enter.) */
        uint8_t clip = c_release;
        int fc = (clip < vb->anim_victim.clip_count) ? vb->anim_victim.clips[clip].frame_count : 1;
        if (fc < 1) fc = 1;
        if (!s_victim_fresh &&
            ++player->anim_frame >= fc) {              /* release clip done -> Leon is free */
            g_player_victim = 0;
            player->hit_react &= (uint8_t)~1u;         /* clear the grabbed flag (+0x93 &= ~1) — a new
                                                        * grab may commit again from here */
            /* BYTE-TRUE EXIT YAW FIX-UP (D1 disasm @0x8010a614-624, live-exact tlm2/04->05
             * 2009->4057 = +0x800): the FACE variant flips +0x6a += 0x800 -> Leon ends FACING the
             * zombie he shoved off (his entity yaw pointed AWAY for the whole grab; the face-victim
             * clips are authored root-180°-flipped). BEHIND keeps the grab yaw (no flip). Without
             * this Leon stood 180° reversed after every face-grab push-away. (The variant-3 -0x800
             * @0x8010a648 belongs to the second clip set {8..13} — not in the port's variant model.)
             * CROW: no flip — the front-FSM exit @0x80115b40-6c writes cmd/hit_react/aca3c only,
             * never +0x6a (and the crow latch never snapped the yaw to begin with). */
            if (g_player_victim_variant == 0 && g_player_victim_type != 0x21)
                player->rot_y = (int16_t)(((int)player->rot_y + 0x800) & 0x0fff);
            player->motion = 200;                      /* restore the idle sentinel: the stale bank2
                                                        * clip index must not feed the normal player
                                                        * anim select for a frame (a wrong-clip flash) */
            player->anim_frame = 0;
            player->anim_frac = 0;                     /* no victim-blend leak into the free idle */
            return;
        }
        s_victim_fresh = 0;
        player->motion = clip;
        if (g_player_victim_type != 0x21)              /* crow: no ad68 placement (see STRUGGLE) */
            re15_clip_root_motion_abs(player, &vb->skel_victim, &vb->anim_victim,
                                      clip, (int)player->anim_frame);
    }
}

/* Reset on room change / death-continue reload (called from re15_enemy_reset). */
void re15_player_victim_reset(void) { g_player_victim = 0; g_player_victim_type = 0;
                                      g_player_victim_variant = 0; s_victim_phase = 0;
                                      s_victim_fresh = 0;
                                      g_player_victim_zombie = -1; s_grab_mercy_timer = 0; }

/* FUN_80100688 (@0x8011f7b4[0], STAGE1.BIN) — the LIVE zombie INIT state. Byte-true core:
 *   +0x4 = 1            -> state ACTIVE        (sb @0x80100704)
 *   +0x1bc/+0x1be = player X/Z snapshot        (sh @0x8010071c/734 — a "last-seen" ref)
 *   +0x0 |= 0x40000000  (lifecycle flag word — no port repr)   @0x80100750
 *   +0x9c = 0x14 (20)   -> ai_timer            (sh @0x80100760)
 * The player-pos snapshot (+0x1bc/+0x1be) is IMPLEMENTED (init @L829-830, per-tick steer refresh
 * @L2523). DEFERRED (cited): the RNG seed (FUN_8001af20 @0x80100774), and the two const GTE vectors copied from
 * 0x80100004/0x80100014 (model-pool matrix setup). This is the LIVE analog of FUN_8011d84c. */
void re15_enemy_ai_live_init(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    e->state    = (uint8_t)RE15_AI_STATE_ACTIVE;   /* +0x4 = 1 */
    e->ai_timer = 0x14;                            /* +0x9c = 0x14 */
    e->hit_stun = (int16_t)((re15_engine_rand8() & 3) + 4);   /* +0x1dc POISE seed (@0x8010082c):
                                                * persistent across hits; decremented once per hit */
    /* +0x1bc/+0x1be steer target: the init write (sh @0x8010071c/734) — the per-tick refresh in
     * re15_enemy_ai_live_tick keeps it mirroring the player (RAM-arbitrated, see there). */
    e->steer_x = (int16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].x;
    e->steer_z = (int16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].z;
    /* OPEN (audit wf_827f186d zombie-live #4, LOW): the original INIT additionally rolls a 25%
     * spawn-VARIANT (@0x801008b0-8f0: `jal rng; andi 3; bne zero,skip` else `jal rng; andi 7;
     * lw @0x8011f7ec[v0]; jalr` — table {0x80101058,0x80101008,0x801010a8,0x801010a8,0x80101008,
     * 0x8010118c,0x8010113c,0x8010118c}). The variant fns write MODEL-POOL render fields (e.g.
     * 0x80101008: pool+0x158|=0x480, +0x1e0/+0x1e4=0x7e8, +0x1e2=0x1000, +0x1ec=0x80,
     * +0x1f0=0x8080e0 — per-instance render/tint variants) which the port render path does not
     * model — NOT implemented rather than faked; tracked as open in RE15_ENEMY_AI_AUDIT.md. */
    /* seed the per-spawn HURT stagger clip (FUN_80100688 @0x80100774-9c): +0x1d4 =
     * seed_table[rng()&7], table @0x8011f7e4 = {2,3,4,5,2,3,4,5} -> a random clip in {2,3,4,5}. */
    {
        static const uint8_t hurt_clip_seed[8] = { 2, 3, 4, 5, 2, 3, 4, 5 };
        e->hurt_clip = hurt_clip_seed[re15_engine_rand8() & 7];
    }
    /* BYTE-TRUE HP (FUN_80100688 @0x801007d8-f4): +0x9a = HPtable[type*0x20 + (rng&0xf)*2], base
     * @0x8011f034. Each zombie type has its own 16-entry per-grid-variant HP row (NOT the nominal 100
     * the spawn seeds). The exact pick is rng = session entropy (byte-true-equivalent to the original's
     * rng pick). Rows read directly from STAGE1.BIN @0x8011f034 + type*0x20. */
    {
        static const uint16_t z_hp[5][16] = {
            /* 0x10 */ { 61, 95, 79, 65, 98, 81, 67,101, 85, 69, 87, 73, 89, 75, 93, 77 },
            /* 0x11 */ { 71, 85,103, 73, 87,105, 75,107, 89, 77, 93, 79, 95, 81, 98, 83 },
            /* 0x12 */ { 71, 91,105, 75, 93,109, 79,111, 95, 81, 83, 99, 97, 89, 90,100 },
            /* 0x16 */ { 71, 83, 95, 83, 85, 97, 75, 85, 99, 77, 87, 87, 89, 81, 81, 91 },
            /* 0x18 */ { 71, 93, 75,1058, 75, 95, 77,107, 97, 81, 99,109, 99, 83,101,103 },
        };
        int row = -1;
        switch (e->type) {
            case 0x10: row = 0; break; case 0x11: row = 1; break; case 0x12: row = 2; break;
            case 0x16: row = 3; break; case 0x18: row = 4; break;
        }
        if (row >= 0) e->hp = (int16_t)z_hp[row][re15_engine_rand8() & 0xf];   /* +0x9a @0x801007f4 */
    }
    /* BYTE-TRUE SPAWN-STATE DECODER (@0x80100c20, branched from the init FUN_80100688 @0x80100904).
     * The Sce_em_set behavior byte (entity+0x9 = grid_id) carries, in its low nibble, the initial AI
     * state — NOT just the spawn pose. The port previously seeded only +0x94 (re15_enemy_spawn_action)
     * and left +0x5=0 / grid=0x86 -> the feeding zombies ran the grid-submode-6 FALLBACK instead of
     * the byte-true combat-submode +0x5=0xc. Here we add the +0x5/+0x6/grid the decoder writes so the
     * spawn state is RAM-identical to the original (briefing.sav: feeders grid=0 +0x5=0xc +0x6=2). */
    {
        uint8_t sel = (uint8_t)(e->grid_id & 0x1f);
        if (sel == 6) {                 /* feeding: +0x4=0x00020c01 @0x80100e60 + grid=0 @0x80100eb0 */
            e->sub_state_1 = 0x0c; e->sub_state_2 = 2; e->grid_id = 0; e->motion = 0x27;
        } else if (sel == 0x0d) {       /* pre-engaged: +0x4=0x201 + grid=0 @0x80100f40-54 */
            e->sub_state_1 = 0x02; e->grid_id = 0; e->motion = 0x27;
        } else if ((e->grid_id & 0x80) && (sel == 1 || sel == 3)) {
            /* byte-true: the WHOLE pose block (incl. this +0x5=5 @0x80100d68 / +0x94=0xc @0x80100d58)
             * is gated on behavior bit 0x80 — @0x80100cac-cb0 `andi v0,v1,0x80; beq v0,zero,0x80100e30`
             * skips it. The port wrote +0x5=5/mo=0xc for ANY 0x01/0x03 behavior (bit clear), clobbering
             * the correctly-gated spawn pose. (audit wf_827f186d zombie-live #5; the pose MOTIONS for
             * {4,7,9}->0x12 / {5,8,0xa}->0x13 / 6/0xb/0xd/0xe live byte-true in re15_enemy_spawn_action,
             * scd_vm.c:2769.) */
            e->sub_state_1 = 0x05; e->motion = 0x0c;
        }
        /* sel 8 (lying, 0x88) / 0xb / 0xe: pose-only in the decoder -> +0x5 stays 0, grid unchanged
         * (RAM: the 0x16 lyer keeps grid=0x88 +0x5=0, mo=0x13). */
    }
}

/* Feeding sub-mode (@0x8011f80c[6] = 0x801018f8, STAGE1.BIN) — the briefing zombie's dist-gated
 * WAKE-UP (byte-true, disasm-verified 2026-06-29 + savestate-confirmed: every active zombie in the
 * live ROOM1140 combat save has woken to +0x9=0). The feeding handler runs two stages each frame:
 *   stage A (0x80103980): if cached dist +0x1d0 < 0xfa0 (4000) AND +0x6 == 0 -> +0x6 = 1 and
 *     +0x9c = rand()&0xf (the wait timer). (The 1/4-chance feeding SE @0x800453d0 is deferred.)
 *   the +0x6 state machine (shared post-step 0x80103a58): 1 = count +0x9c down (0x80103ad0), at 0 ->
 *     +0x6=2 / +0x8f=0xf; 2 = play the stand-up anim (0x80103b08, anim_set +0x170/+0x174) -> +0x6 +=
 *     anim-done; 3 = COMMIT (0x80103b3c): `sb zero,9` -> +0x9 = 0 (combat sub-mode), `sw 0x201,4` ->
 *     +0x4 word = 0x201 (state 1 ACTIVE / +0x5 = 2 = the engage brain), `andi +0x93,0xfe`.
 * The woken zombie lands in combat sub 0 with the decision brain at phase 2 (engage =
 * re15_ai_decide_engage). NOT gated on DAT_800aca3c — the wake is distance-only (only the DORMANT
 * lunge-arm needs that flag; in this prototype nothing sets it, so the real attack is the GRAB the
 * engage commits, see 8.8). Faithful-line: the phase-2 stand-up anim duration is the deferred anim
 * layer, so the port advances 2->3 directly (the AI transition is byte-true; the exact anim frame
 * count is not modeled — same stance as re15_enemy_lunge_begin). The phase-0 busy writes (+0x93|=1 /
 * +0x1b8=1 @0x80103aac) are deferred: +0x93 is the port's shared hit-guard byte and +0x1b8 the
 * neck-flags, so replaying them would disturb the damage/neck subsystems — they are a wake-up busy
 * latch with no port consumer. Fields: sub_state_2 (+0x6), ai_timer (+0x9c), anim_frac (+0x8f). */
/* Feeding ANIMATE — byte-true FUN_801048e8 (@0x8011f890[+0x5=0xc], STAGE1.BIN). The combat-sub-mode
 * feeding loop the ROOM1140 briefing zombies SPAWN into (spawn decoder @0x80100e40: behavior nibble
 * 6 -> +0x5=0xc, +0x6=2, grid=0, mo=0x27). Phase (+0x6): entry(0) -> clip 0x29 intro (1) -> loop
 * {0x27 or 0x28} (2 re-roll -> 3 play), clearing the +0x1d8 attack bit, with a slow yaw-slew toward
 * the food target (+0x1bc/+0x1be, seeded to the player at init -> faithful-line: slew at the byte-true
 * 0x20 toward the player). The wake (decide FUN_801048a8) flips +0x5=0xc -> 0xd when dist<3000. */
static void re15_enemy_ai_feeding_animate(re15_actor_t *e, const re15_actor_t *player)
{
    uint8_t phase = e->sub_state_2;                        /* +0x6 */
    if (phase != 1) {
        if (phase > 1) {                                  /* the {0x27/0x28} feeding loop */
            if (phase == 2) {                             /* re-roll the feeding clip (@0x8010491c) */
                e->sub_state_2 = 3;
                e->motion = (uint8_t)(0x27 + (re15_engine_rand8() & 1));
                e->anim_frame = 0;
                e->anim_flags &= (uint16_t)~0x80u;        /* the loop plays FORWARD */
                return;                                   /* new clip -> not done this frame */
            }
            if (phase != 3) return;
            e->ai_flags &= (uint16_t)~0x10u;              /* +0x1d8 &= 0xffef */
            if (re15_enemy_clip_done(e)) e->sub_state_2 = 2;  /* clip done -> re-roll */
            return;
        }
        if (phase != 0) return;
        e->sub_state_2 = 1;                               /* entry: clip 0x29 intro (@0x801049a0) */
        e->motion = 0x29; e->anim_frame = 0; e->anim_frac = 7;
        e->anim_blend_rate = 0x200;                       /* every 0xc/0xd f314 = a3 0x200 (cluster E) */
        e->anim_flags |= 0x80;                            /* the KNEEL-DOWN plays clip 0x29 (the
                                                           * get-up) IN REVERSE — f314 a2=1
                                                           * (@0x80104994, cluster-E raw): the
                                                           * zombie SINKS from standing onto the
                                                           * corpse; the stand-up 0xd plays the
                                                           * same clip FORWARD (a2=0 @0x80104af0) */
    }
    (void)player;
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, 0x20);  /* func_0x8001aac4(+0x1bc,+0x1be,0x20)
                                                               * — the stored snapshot, byte-true */
    if (re15_enemy_clip_done(e)) e->sub_state_2 = 2;      /* 0x29 intro done -> the loop */
}

/* Stand-up ANIMATE — byte-true FUN_80104a50 (@0x8011f890[+0x5=0xd], STAGE1.BIN). The woken feeder's
 * GET-UP: play clip 0x29 (@0x80104aa8 sets +0x94=0x29) once to clip-end, then +0x4=0x201 (+0x5=2
 * engage) @0x80104b24. This is the byte-true state-machine home of the "stehen sauber auf" clip. */
/* ONE-SHOT clip HOLD: clamp anim_frame at the clip's last frame so the monotonic counter + the
 * render's %fc wrap can never REPLAY a one-shot from its start (byte-true f8b4 terminal clamp). */
static void re15_enemy_hold_last_frame(re15_actor_t *e)
{
    re15_enemy_bank_t *hb = re15_enemy_find(e->type);
    if (!hb || !hb->ok || (int)e->motion >= hb->anim.clip_count) return;
    int fc = hb->anim.clips[e->motion].frame_count;
    if (fc > 0 && (int)e->anim_frame > fc - 1) e->anim_frame = (uint16_t)(fc - 1);
}

static void re15_enemy_ai_standup_animate(re15_actor_t *e)
{
    uint8_t phase = e->sub_state_2;                        /* +0x6 */
    if (phase >= 2) {                                     /* clip played out -> engage */
        re15_enemy_hold_last_frame(e);                    /* keep holding through the handoff ticks */
        if (phase == 2) re15_ai_set_state_word(e, 0x201); /* +0x4 = 0x201 -> +0x5=2 */
        return;
    }
    if (phase == 0) {                                     /* entry: the get-up clip 0x29 */
        e->sub_state_2 = 1;
        e->motion = 0x29; e->anim_frame = 0; e->anim_frac = 7;
        if ((re15_engine_rand8() & 3) == 0) re15_audio_room_se(5);  /* @0x80104ae0 func_0x800453d0(5) */
    }
    /* ONE-SHOT HOLD (byte-true f8b4: a 0x8000-terminal frame entry CLAMPS the counter — one-shots
     * never wrap). The port's monotonic anim_frame + the render's %fc wrap REPLAYED the stand-up
     * from its crouching first frames during the exit handoff — RENDER-DUMP-PROVEN: F51 kf946
     * standing, F60 kf921 back in the crouch = the "falls down again after standing up" report. */
    if (re15_enemy_clip_done(e)) {
        re15_enemy_hold_last_frame(e);                    /* hold the standing last frame */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + 1);
    }
}

static void re15_enemy_ai_live_feeding(re15_actor_t *e)
{
    /* stage A — start the wake sequence when the player closes within 4000. */
    if (e->ai_dist < 4000u && e->sub_state_2 == 0) {     /* +0x1d0 < 0xfa0 && +0x6 == 0 */
        e->sub_state_2 = 1;                              /* +0x6 = 1 */
        e->ai_timer    = (int16_t)(re15_engine_rand8() & 0xf);  /* +0x9c = rand()&0xf */
    }
    switch (e->sub_state_2) {                            /* the +0x6 0->3 wake machine */
        case 0: break;                                   /* idle-feeding (busy writes deferred) */
        case 1:                                           /* count the wait timer down (0x80103ad0) */
            if (e->ai_timer != 0) e->ai_timer = (int16_t)(e->ai_timer - 1);
            else { e->sub_state_2 = 2; e->motion = 0x29; e->anim_frac = 0xf; e->anim_frame = 0; }
            /* enter stand-up: play the byte-true GET-UP clip 0x29 (59f). The RAM-authoritative wake
             * runs the feeding as combat-sub-mode +0x5=0xc (FUN_801048e8) -> +0x5=0xd stand-up
             * (FUN_80104a50 @0x8011f890[0xd]) which sets +0x94 = 0x29 and plays it to clip-end before
             * engaging (disasm STAGE1.BIN @0x80104aa8). The port was REPLAYING the loaded feeding clip
             * 0x27 here -> "stehen nicht sauber auf" (parity: original played the get-up, port did not).
             * Clip 0x29 verified present in the loaded zombie EDD (bank1 clip 0x29 = 59 frames). */
            break;
        case 2:                                           /* stand-up anim: play clip 0x29 (the get-up) FORWARD
                                                           * until clip-end, THEN commit — byte-true FUN_80104a50
                                                           * `+0x6 += func_0x8001f314(+0x170,+0x174,0,0x200)`. */
            e->sub_state_2 += (uint8_t)re15_enemy_clip_done(e);
            break;
        case 3:                                           /* COMMIT (0x80103b3c) -> combat / engage */
            e->grid_id     = 0;                          /* +0x9 = 0 (combat sub-mode 0) */
            re15_ai_set_state_word(e, 0x201);            /* +0x4 = state 1 / +0x5 = 2 (engage) */
            e->hit_react  &= (uint8_t)~1u;               /* +0x93 &= ~1 */
            break;
    }
}

/* Grab EXECUTION (@0x8011f890[3/4] = FUN_80102548, STAGE1.BIN) — the LIVE zombie's IN-GAME attack
 * (NOT the dormant lunge). byte-true, disasm-verified 2026-06-29 (full 9-sub-step machine + the
 * player-grabbed FSM RE'd; the f840[3/4] LOGIC half @0x80102540 is a `jr ra` no-op — all grab work
 * is in this f890 ANIMATE half). The engage brain commits the grab (re15_ai_decide_engage -> state
 * word 0x301/0x401, i.e. +0x5 = 3 face / 4 behind) when dist<0x4b0 && tight-arc && same-floor &&
 * facing; that word also resets +0x6 (its high bytes are 0), so the grab's sub-step machine starts
 * at 0. FUN_80102548 dispatches entity+0x6 (0..8) via the jump table @0x80100024; the byte-true
 * damage is [2] IMPACT = player.hp -= 10 (@0x8010277c) and [3] BITE = player.hp -= 5 (@0x801027dc,
 * which LOOPS on the +0x9c=0x6e window applying -5 each time the bite anim frame lands), and the
 * machine exits at [8] with +0x4 word = 0x201 (back to the engage brain, +0x5=2; @0x80102b9c).
 * FAITHFUL-LINE: the port has no zombie grab anim, so the anim-gated sub-step advances ([1]/[3]/[5]/
 * [7]) and the bite LOOP COUNT (anim-gated) are stand-ins — the port applies one -5 bite per grab
 * cycle (the engage re-commits the grab while the player stays in range, so the damage repeats over
 * cycles). IMPLEMENTED (was cited DEFERRED): the player-grabbed pose/lock FSM (cmd 5 -> LAB_80036834) —
 * the PIN is s_player_grabbed @L1027 (game_step reads re15_player_is_grabbed) and the ANIMATION is the
 * grab-victim FSM @L497-803 (Leon driven off the zombie's bank-2 grab-link globals 0x800acbcc/d0, grab
 * motion +0x94); the +0x93|=1 grabbed flag is port-field-aliased to the hit-guard. The byte-true -10/-5 HP damage +
 * the sub-step structure + the exit ARE ported. Uses sub_state_2 (+0x6), ai_timer (+0x9c). */
static void re15_enemy_ai_live_grab(re15_actor_t *e, re15_actor_t *player)
{
    if (!player) return;
    /* The grab has the player latched (DAT_800aca58 = cmd 5, byte-true @0x80102640): hold the
     * player-grabbed flag every frame the grab runs so game_step pins the player (re15_player_is_
     * grabbed). re15_enemy_ai_run_all clears it each frame, so it tracks "a live zombie is grabbing
     * THIS frame" = the faithful-line release for the deferred player grabbed-FSM. */
    if (e->sub_state_2 <= 3)
        s_player_grabbed = 1;   /* the pin tracks the HOLD [0..3] only: from the throw-off [4] on,
                                 * the player's OWN release clip (victim state 3, cmd->1 at ITS end)
                                 * governs his pin — byte-true he can already move while the zombie
                                 * still plays the [6]/[7] recovery (user-verified vs original). */
    /* Leon's victim latch (cmd 5) happens ONCE at the [0] commit (@0x80102640) — NOT here every
     * frame: the every-frame latch re-entered the struggle from the release state 3 one frame after
     * the THROW-OFF [4] (the latch's ==3 re-enter path) = Leon's push-away anim never played
     * (dump-proven: F131 vs=3 -> F132 vs=1 while the zombie ran its own throw-off [5]). */
    /* GRAB-CLIP ROOT MOTION — ABSOLUTE from the SHARED ANCHOR (P2 disasm 2026-07-04, survived
     * refutation): FUN_80102548 calls func_0x8001ad68(zombie, +0x170, +0x174) in sub-steps [1] pull-in,
     * [3] bite and [5] throw-off — an ABSOLUTE placement pos = anchor(+0xa0/+0xa2) + RotY(own yaw) *
     * off[kf] of the bank1 grab clip. Because the grab [0] copied ONE anchor to both the zombie and
     * the player (FUN_8001ac38 @0x801025f0), the pair sits in the clips' AUTHORED interlocking
     * formation — the old per-entity DELTA let them drift INTO each other (observed d=2..23 = Leon
     * clipped inside the zombie). Recovery [7] is NOT ad68 — it is a velocity walk-back (below). */
    if (e->sub_state_2 == 1 || e->sub_state_2 == 3 || e->sub_state_2 == 5) {
        re15_enemy_bank_t *gb = re15_enemy_find(e->type);
        if (gb && gb->ok)
            re15_clip_root_motion_abs(e, &gb->skel, &gb->anim,
                                      (int)e->motion, (int)e->anim_frame);
    }
    /* per-sub-step GRAB CLIP (FUN_80102548 +0x6 sub-steps 0/2/4 @0x801025bc/0x80102714/0x801028f0):
     * +0x94 = (+0x5-3)*3 + {0,1,2}. +0x5 = the facing (3 face / 4 behind, dynamic from the engage),
     * so face-grab plays clips {0,1,2}, behind-grab {3,4,5}; the release (sub-step 6) is the literal
     * clip 17 (@0x80102a64). The port maps its sub_state_2 machine (0/2 = the byte-true clip steps,
     * 6 = release); the K=2 hold step (sub-step 4) is collapsed (faithful-line). Each set resets
     * anim_frame (+0x95) = 0. */
    uint8_t grab_base = (uint8_t)((e->sub_state_1 - 3) * 3);   /* (+0x5-3)*3 */
    switch (e->sub_state_2) {                /* +0x6 sub-step (reset to 0 by the 0x301/0x401 commit) */
        case 0:                               /* [0] init/latch + grab clip base (@0x801025bc) */
            /* Leon's victim latch = the byte-true cmd-5 write @0x80102640 — [0] only (see above). */
            re15_player_victim_latch(e, player);
            e->motion = grab_base; e->anim_frame = 0;
            e->anim_frac = 7;                         /* +0x8f = 7 (byte-true [0], P2 disasm) — the
                                                       * ~8-frame walk->grab pose blend; without it the
                                                       * zombie SNAPPED from the bank0 walk pose to the
                                                       * bank1 pull-in = the grab-start flash */
            e->anim_blend_rate = 0x200;               /* one-shot f314 rate */
            /* GRABBED flag (byte-true player+0x93 |= 1, latched with the cmd-5 write @0x80102640):
             * the engage/turn grab-commits require +0x93 == 0, so a SECOND zombie cannot grab the
             * already-held player. TIMELINE-VERIFIED: the original's other zombie stayed in engage at
             * d=851 for the whole hold; the port had TWO simultaneous grabbers (F223) = double bites. */
            player->hit_react |= 1;
            e->ai_flags |= 1;                 /* +0x1d8 |= 1 "actively grabbing" (FUN_80102548.c Z.28);
                                               * cleared in throw-off [4] (Z.75). Read by the domino-
                                               * shove gate (Z.87: only a NON-grabbing bystander is
                                               * shovable). Dossier zombie_hit_1140.md D6. */
            e->grab_choreo = 1;               /* word0 |= 0x1000 grab-choreo latch (FUN_80102548.c
                                               * Z.21-22) — blocks the +0x1da windup-timeout death
                                               * (FUN_80101224.c Z.12) until the [8] exit clears it
                                               * (Z.115). Byte-true leak: a shot-aborted grab leaves
                                               * it set. Dossier zombie_hit_1140.md D5. */
            /* THE SHARED ANCHOR (byte-true FUN_8001ac38 @0x801025f0, P2 disasm): computed on the
             * PRE-SNAP yaw from the BANK0 pair (+0x84/+0x16c) clip[grab_base] frame-0 root offset
             * (em10: (0,7) ~= the latch position), then COPIED to the player (@0x8001ad28-ad48) —
             * both entities place ABSOLUTELY from this one anchor for the whole grab choreography. */
            {
                re15_enemy_bank_t *gb0 = re15_enemy_find(e->type);
                if (gb0 && gb0->loco_ok)
                    re15_clip_anchor_set(e, &gb0->skel_loco, &gb0->anim_loco, (int)grab_base, 0);
                else { e->anchor_x = e->x; e->anchor_z = e->z; }
                player->anchor_x = e->anchor_x;
                player->anchor_z = e->anchor_z;
            }
            /* YAW SNAP (byte-true [0] FUN_8001a8f8(&player_pos, 0x800), AFTER ac38: with clamp 0x800
             * the compare (0x800+delta)&0xfff < 0x1000 is always true -> the facing SNAPS to the
             * player. This aims the grab clips' baked root translation AT the player. */
            e->rot_y = (int16_t)(((int)re15_atan2_q12(player->z - e->z, player->x - e->x) - 0x400) & 0x0fff);
            re15_audio_room_se(4);            /* grab-START SE (@0x8010268c func_0x800453d0(4), snd1) */
            e->sub_state_2 = 1; break;
        case 1:                               /* [1] pull-in — play the grab-base clip, advance ON CLIP-END
                                               * (byte-true `+0x6 += func_0x8001f314(+0x5c,+0x5d,0,0x200)`;
                                               * was a 1-frame snap). No bank -> guard -> immediate. */
            e->sub_state_2 += (uint8_t)re15_enemy_clip_done(e);
            break;
        case 2:                               /* [2] IMPACT — clip base+1 (@0x80102714) + the byte-true -10 hit */
            e->motion = (uint8_t)(grab_base + 1); e->anim_frame = 0;
                                                      /* @0x80102714 writes only +0x94 — but the
                                                       * original +0x95 has just WRAPPED to 0 (f8b4)
                                                       * when [1] completed; the port counter is
                                                       * MONOTONIC, so frame=0 IS the equivalent
                                                       * (keeping it desynced the pair = bite-phase
                                                       * clipping, user-verified). */
            player->hp     = (int16_t)(player->hp - 10);
            if (player->hp < 0) player->state = 7;   /* hp<0 -> GRABBED death (state 7, save-confirmed;
                                                      * re15_player_is_dead() = hp<0 drives the death FSM) */
            e->ai_timer      = re15_grab_mercy_active() ? 5 : 0x6e;
                                             /* +0x9c ESCAPE window (@0x8010276c): 0x6e, or 5 while the
                                              * just-escaped mercy flag DAT_800aca50&1 is set -> the
                                              * re-grab throws off after ~5 ticks */
            e->grab_kill_ctr = 100;          /* +0x9e KILL counter (@0x8010276c: 0x64) */
            e->sub_state_2 = 3;
            break;
        case 3:                               /* [3] BITE-LOOP (@0x801027dc) — the HELD bite. TIMELINE-VERIFIED
                                               * (deterministic /tmp/tl3): the grab HOLDS this sub-step for the
                                               * whole +0x9c=0x6e (110-frame) window, applying -5 PER bite-clip
                                               * WRAP (observed hp 85->80->75->70, one -5 per ~26f clip ~0.87s);
                                               * at WINDOW EXPIRY the FATAL DEVOUR lands (FUN_801185dc: HP-=600,
                                               * player cmd 6 devour-collapse; observed corpse hp == -1 exactly,
                                               * kill at ~t+5.0 == 110 frames after the impact ✓). The zombie
                                               * then exits to +0x5=6 WALK (observed: killer walks clip 0xa,
                                               * circles the corpse). The old port did ONE -5 then released ->
                                               * ground the kill over re-grab cycles (~15s vs the true ~7.6s). */
            {
                /* RENDER-LOOP the bite (user report: "the bite animation plays once then FREEZES;
                 * in the original it plays the whole time"). The live zombie's clips loop via the
                 * clip's TERMINAL MARKER (FUN_8001f314 wraps at clip-end — there is NO +0x1c4 loop-flag
                 * write anywhere in the grab), but the port keeps anim_frame MONOTONIC (for the bite
                 * damage gate) and renders with clip_override=-1, so re15_compute_actor_kf HOLD-LASTs
                 * once af exceeds the clip length = the bite pose freezes while the anchor-placement
                 * (which wraps af%fc) keeps sliding = the clip. Set the port's render loop-hint
                 * +0x1c4&0x04 so re15_compute_actor_kf cycles slot = af % frame_count = the bite plays
                 * continuously like the original. Cleared on both grab exits (devour / throw-off). */
                e->anim_flags |= 0x04u;
                /* -5 ONCE PER BITE-CLIP CYCLE (@0x801027dc, gated on func_0x8001f314's wrap return).
                 * anim_frame runs monotonically (the shared pass does not wrap it), so "clip wrap" =
                 * the loop slot hitting the clip's last frame — exactly one frame per cycle (original
                 * cadence: -5 per ~26f ~0.87s; measured identical in v4). No bank -> immediate fallback. */
                re15_enemy_bank_t *bb = re15_enemy_find(e->type);
                int bite_fc = 0;
                if (bb && bb->ok && e->motion < bb->anim.clip_count)
                    bite_fc = bb->anim.clips[e->motion].frame_count;
                int bite_now = (bite_fc > 0) ? (((int)e->anim_frame % bite_fc) == bite_fc - 1)
                                             : re15_enemy_clip_done(e);
                if (bite_now) {
                    /* Biss-Zyklus-BLUT (@0x80102818, im selben Wrap-Zweig wie das HP-5
                     * @0x801027dc): Effekt 0 sub 0, scale 0x1500 (@0x80102808), am ZOMBIE-
                     * Bone 10 (Typen 0x13/0x14: 11; Select @0x801027f4-0x80102804), a1 =
                     * Zombie-rot_y (@0x8010280c), a3 = (0,0,0) @0x8011f7d4.
                     * bite_blood_fx.md D1 (CONFIRMED) — der vom Nutzer vermisste Biss-Effekt. */
                    int bbone = (e->type == 0x13 || e->type == 0x14) ? 11 : 10;
                    int32_t bg[3]; re15_enemy_bone_world_pos(e, bbone, bg);
                    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                                         bg[0], bg[1], bg[2], (int16_t)e->rot_y);
                }
                if (bite_now && player->hp >= 0) {
                    player->hp = (int16_t)(player->hp - 5);
                    if (player->hp < 0) player->state = 7;
                }
                /* TWO byte-true counters (D3/D5 disasm of FUN_80102548 [3]):
                 *  +0x9c ESCAPE window: -= 1 + 5*mash-presses (FUN_80037024; mash IMPLEMENTED via re15_mash_pressed @L1154);
                 *      goes NEGATIVE -> THROW-OFF [4] (the player breaks free ALIVE).
                 *  +0x9e KILL counter: -1 per tick; WAS 0 (or player hp<0) -> the DEVOUR handoff
                 *      +0x4 word = ((+0x5)+2)<<8|1 (face 3->5, behind 4->6; @word-write in [3]) — the
                 *      zombie's devour-finish state; HP is NOT touched here (the original hands off at
                 *      hp=70; the -1 lands at collapse frame 35 in the player cmd-6 handler).
                 * Un-mashed: kill (100) < escape (110) -> every held grab is fatal ✓ timeline. */
                int16_t kc = e->grab_kill_ctr;
                e->grab_kill_ctr = (int16_t)(kc - 1);
                if (kc == 0 || player->hp < 0) {
                    e->anim_flags &= (uint16_t)~0x04u;    /* leave the bite loop -> DEVOUR (play-once) */
                    re15_ai_set_state_word(e, ((uint32_t)(e->sub_state_1 + 2) << 8) | 1u);
                    break;
                }
                /* the escape window drains 1 + 5*mash (byte-true FUN_80037024 edge-press: any D-pad or
                 * face button = -6 that tick). Mash ~every 4 frames -> escape in ~50 ticks (< the 100-
                 * tick kill counter, breaks free ALIVE); no mash -> 110 > 100, devoured. */
                e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 5 * re15_mash_pressed()));
                if (e->ai_timer < 0) {                   /* escape window ran out -> THROW-OFF (alive) */
                    e->anim_flags &= (uint16_t)~0x04u;    /* leave the bite loop -> the fling/recovery clips play ONCE */
                    e->sub_state_2 = 4;
                }
            }
            break;
        case 4:                               /* [4] THROW-OFF — clip base+2 (@0x801028f0) + SE 7; the
                                               * player-side release finish starts in lockstep (the
                                               * original writes DAT_800aca5a=4 = the struggle FSM's
                                               * release phase). Sets the just-escaped mercy flag
                                               * DAT_800aca50|=1 + the 0x5a=90-tick clear timer (+0x1d5). */
            e->motion = (uint8_t)(grab_base + 2); e->anim_frame = 0;
            e->anim_frac = 7;                 /* +0x8f = 7 (@[4] decompile line 71) — walk->fling blend */
            e->ai_flags &= (uint16_t)~1u;     /* +0x1d8 &= 0xfffe (FUN_80102548.c Z.75) — the grab is
                                               * over, the zombie is a shovable bystander again */
            re15_audio_room_se(7);
            re15_audio_room_se(7);            /* SE 7 plays TWICE (@0x80102920 AND @0x80102960) */
            re15_player_victim_throwoff();
            /* UEBERLEBTER GRAB = die einzige Stelle, an der die Wund-Decals stempeln (Phase 4
             * der Opfer-FSM: Dispatcher @0x8010a580-5b0 jalr 0x801201b8[aca59]; player_hit_
             * chain.md HIT-1/F2, doppelt verifiziert via bite_blood_fx §2.2). dir = Grab-
             * Richtung: face (+0x5=3) -> 0, behind (4) -> 1 (aca59-Write des Grab-Commits
             * @0x80102640). Devour/Death stempeln NICHT. Dazu das Release-BLUT am Spieler-
             * Bone 8 (scale 0x1500 @0x8010a550, Anker +0x5A0 @0x8010a568/70, a3=(0,0,0)
             * @0x801201c8; bite_blood_fx.md D3). */
            re15_wound_release_stamp((e->sub_state_1 == 3) ? 0 : 1);
            {
                int32_t rb8[3]; re15_player_victim_bone_pos(8, rb8);
                re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                                     rb8[0], rb8[1], rb8[2], (int16_t)player->rot_y);
            }
            s_grab_mercy_timer = 0x5a;
            e->sub_state_2 = 5;
            break;
        case 5:                               /* [5] throw-off plays to its end (root motion active) */
            e->sub_state_2 += (uint8_t)(re15_enemy_clip_done(e) ? 1 : 0);
            if ((int)e->anim_frame == 0x18)   /* HARD CUT at anim frame 0x18 (@[5] decompile line 82:
                                               * `if (+0x95 == 0x18) +0x6 = 6`) — the fling clip is cut
                                               * to the recovery at frame 24 even before clip-end */
                e->sub_state_2 = 6;
            /* DOMINO SHOVE (@0x80102a50, live-observed 3x in the DYN trace): while the reeling
             * zombie has a body contact from another ENEMY (+0x1c2&2, ptr +0x1ac — written by the
             * body push) and +0x95 > 7, the CONTACTED bystander is knocked into the 0xb push-off
             * (gates: alive, not downed +0x9&0x80, +0x1d8&1 clear). */
            if ((e->contact_flags & 2) && (int)e->anim_frame > 7 &&
                e->contact_slot > 0 && e->contact_slot < RE15_ACTOR_MAX) {
                re15_actor_t *by = &g_actors[e->contact_slot];
                if (by->active && by->hp >= 0 && !(by->grid_id & 0x80) && !(by->ai_flags & 1))
                    re15_ai_set_state_word(by, 0xb01);
            }
            break;
        case 6:                               /* [6] recovery — set the literal clip 17 (0x11, @0x80102a64);
                                               * the SE 7 belongs to the THROW-OFF [4] (@0x80102920/60).
                                               * Seeds the walk-back speed +0x8c = 0x32 (P2 disasm). */
            e->motion = 0x11; e->anim_frame = 0;
            e->anim_frac = 0xf;               /* [6] re-seeds +0x8f = 0xf (D2 disasm) — the throw-off ->
                                               * stagger-back pose blend... */
            e->anim_blend_rate = 0x100;       /* ...consumed by [7]'s f314(1, 0x100) rate (@decompile
                                               * line 101) — slower 15-step fade than the grab's 0x200 */
            e->anim_flags |= 0x80;            /* [7] plays clip 0x11 in REVERSE (f314 a2=1 @decompile
                                               * line 101) — the stagger-back is the get-up clip run
                                               * BACKWARD (the port render honors flag 0x80) */
            e->speed_h = 0x32;
            player->hit_react &= (uint8_t)~1u; /* [6] DAT_800acae7 &= 0xfe (@decompile line 98) — the
                                                * zombie-side release of the player grab flag */
            e->sub_state_2 = 7; break;
        case 7:                               /* [7] recovery plays clip 0x11 BACKWARD — NO ad68 root
                                               * motion: the zombie VELOCITY-staggers BACKWARD
                                               * (func_0x800245d8(0x800) = move along yaw+180°; speed
                                               * +0x8c, -2/tick (+0x9e=2 @[6]), min 10) while the clip
                                               * plays out — AWAY from the player, not into him. */
            e->x -= (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->speed_h) >> 12);
            e->z += (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->speed_h) >> 12);
            e->speed_h = (int16_t)(e->speed_h - 2);
            if (e->speed_h < 10) e->speed_h = 10;
            e->sub_state_2 += (uint8_t)(re15_enemy_clip_done(e) ? 1 : 0);   /* 7 -> 8 on clip-end */
            break;
        default:                              /* [8] EXIT (0x80102b90) -> back to the engage brain */
            e->anim_flags &= (uint16_t)~0x80u;  /* drop the [6] reverse-playback flag */
            e->grab_choreo = 0;                 /* word0 &= 0xffffefff (FUN_80102548.c Z.115) — the
                                                 * ONLY clear of the grab-choreo latch */
            re15_ai_set_state_word(e, 0x201);   /* +0x4 = state 1 / +0x5 = 2 (engage) */
            /* the player-side release clears the grabbed flag (+0x93 &= ~1). With a victim bank the
             * release-finish anim does it; WITHOUT one (headless/unit tests) clear it here so a later
             * grab can commit again (no permanent grab-immunity). */
            if (re15_player_victim_state() == 0)
                player->hit_react &= (uint8_t)~1u;
            break;
    }
}

/* SEARCH-STAND animate (+0x5=0) — byte-true FUN_80101d08 (@0x8011f890[0], cluster-B raw): the idle
 * stand between wander walks. Entry: +0x9c=(rand&0x3f)+0x12c (300..363 ticks), clip 0 (idle),
 * +0x8f=0xf, rate 0x100, clears +0x93. No steering — it just stands (the decide escalates). */
static void re15_enemy_ai_live_search_stand(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;
        e->ai_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 0x12c);
        e->motion = 0; e->anim_frame = 0;
        e->anim_frac = 0xf; e->anim_blend_rate = 0x100;
        e->hit_react = 0;                                 /* +0x93 = 0 */
    }
}

/* WANDER-WALK animate (+0x5=1) — byte-true FUN_80101ef0 (@0x8011f890[1], cluster-B raw): the slow
 * unaware shamble. Entry: +0x9c=(rand&0xff)+0x258 (600..855 ticks), clip 1, +0x8f=0xf, +0x1d6 =
 * a RANDOM NAV ZONE (FUN_8003a07c) = the roam target. EVERY tick: +0x1d8 |= 8 (the one-shot roam
 * request the m0 root's nav steer consumes -> the zombie roams to the zone's midpoint), steer
 * aac4(+0x1bc,+0x1be, 8), rate 0x100, foot-lock; timer done -> +0x4 word = 1 (back to search). */
static void re15_enemy_ai_live_wander(int slot, re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_2 = 1;
        e->ai_timer = (int16_t)((re15_engine_rand8() & 0xff) + 0x258);
        e->motion = 0x01; e->anim_frame = 0;
        e->anim_frac = 0xf; e->anim_blend_rate = 0x100;
        e->ai_wp_node = re15_nav_rand_zone();             /* +0x1d6 = rand zone (@0x80101f5c-70) */
        e->hit_react = 0;
        s_zfoot_ok[slot] = 0;
    }
    e->ai_flags |= 8;                                     /* roam request, re-asserted per tick
                                                           * (@0x80101f90-9c); consumed + cleared by
                                                           * the root nav steer next tick */
    int16_t t = e->ai_timer;
    e->ai_timer = (int16_t)(t - 1);
    if (t == 0) { re15_ai_set_state_word(e, 0x0001); return; }   /* -> search-stand */
    e->anim_flags |= 0x04u;                              /* LOOP the shamble clip (see engage_animate) */
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, 8); /* aac4(+0x1bc,+0x1be,8) */
    re15_enemy_footlock_step(slot, e);
}

/* LOS SENSOR — byte-true FUN_8001bc08 (EXE, raw-disasm'd cluster B; NO decompile): the amortized
 * 16-tick line-of-sight probe the ACTIVE root runs per tick (@0x80101560), whose result drives
 * +0x1d8 bit 0x10 = "player visible" — the gate that lets search/wander escalate to ENGAGE/CHARGE.
 * Ticks 0..3 of the +0x1f0 counter probe one ray step each (original FUN_8001b84c(0x5e8, player));
 * on tick 3, blocked-bit clear -> +0x1bc/+0x1be := player snapshot, return 1 (visible); blocked ->
 * 0; ticks 4..15 return 2 (no update). FAITHFUL-LINE stand-in for the b84c ray step: sample the
 * enemy->player segment at (i+1)*0x5e8 units and test the SCA walkable floor (walls = sight
 * blockers) — the port has no separate ray-cast geometry. Counter/gating cadence is byte-true. */
static uint8_t s_los_counter[RE15_ACTOR_MAX];             /* +0x1f0 low bits */
static uint8_t s_los_blocked[RE15_ACTOR_MAX];             /* +0x1f0 bit 0x20 accumulator */
/* BYTE-TRUE LOS-Blocker-Test FUN_8003dcc4 (Decompile RE_15_Quellcode_V2/FUN_8003dcc4.c,
 * Feld-Mapping gegen re15_sca_entry_t: hw0=width hw1=density hw2=x hw3=z hw5=u1|floor<<8):
 * pro Step k wird NUR die Quadranten-Region k der SCA gescannt (Runtime-Pointer
 * @sca_base+8+k*4 aus FUN_8003aea0; Port = Praefixsummen sca_rgn[0..3], Region 4 NIE).
 * Zellen-Filter: (w5>>12) == GEGNER-Band (+0x82: Kraehe = -(y/1800) Root-Refresh
 * @0x80112128-54, Zombie = em_set pc[4] @0x800421d0) && (w5&0xf00) == 0x300.
 * Kreuzung: alle Koordinaten /0x12, Ray (enemy->player) muss BEIDE Straddle-Tests einer
 * Zell-Diagonale bestehen (OuterProduct0-Vorzeichen; Diagonalen (x1,z0)-(x0,z1) und
 * (x0,z0)-(x1,z1)). analysis/crow_shot_attack.md F1 (CONFIRMED): der alte on_floor-
 * Stand-in nutzte das SPIELER-Band mit invertierter Zellen-Semantik und blockte offene
 * Flaechen dauerhaft — die fliegende 1170-Kraehe (Band 5..7 > max. Zellen-Band 4) ist im
 * Original strukturell NIE geblockt, ROOM1140 (alle Zellen Band 0/Typ 3) bleibt identisch. */
static int32_t los_cross_z(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    return ax * bz - az * bx;                      /* OuterProduct0 y-Komponente */
}
static int los_seg_straddles_diag(int32_t ex, int32_t ez, int32_t px, int32_t pz,
                                  int32_t c1x, int32_t c1z, int32_t c2x, int32_t c2z)
{
    int32_t dx = c2x - c1x, dz = c2z - c1z;
    int32_t s1 = los_cross_z(dx, dz, px - c1x, pz - c1z);
    int32_t s2 = los_cross_z(dx, dz, ex - c1x, ez - c1z);
    if (((uint32_t)(s1 ^ s2) & 0x80000000u) == 0) return 0;   /* Ray-Enden straddeln nicht */
    int32_t rx = px - ex, rz = pz - ez;
    int32_t s3 = los_cross_z(rx, rz, c1x - ex, c1z - ez);
    int32_t s4 = los_cross_z(rx, rz, c2x - ex, c2z - ez);
    return ((uint32_t)(s3 ^ s4) & 0x80000000u) != 0;          /* Diagonal-Enden straddeln */
}
static int re15_los_ray_blocked(const re15_actor_t *e, const re15_actor_t *player, int step)
{
    if (!g_room_rdt.sca || step < 0 || step > 3) return 0;
    int start = 0;
    for (int g2 = 0; g2 < step; g2++) start += g_room_rdt.sca_rgn[g2];
    int cnt = g_room_rdt.sca_rgn[step];
    if (start + cnt > g_room_rdt.sca_count) return 0;
    int32_t ex = e->x / 0x12, ez = e->z / 0x12;               /* alle Koordinaten /0x12 */
    int32_t px = player->x / 0x12, pz = player->z / 0x12;
    for (int i = 0; i < cnt; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[start + i];
        if ((c->floor >> 4) != e->floor) continue;            /* w5>>12 == +0x82 */
        if ((c->floor & 0x0f) != 3) continue;                 /* (w5&0xf00) == 0x300 */
        int32_t x0 = (int32_t)c->x / 0x12, z0 = (int32_t)c->z / 0x12;
        int32_t x1 = ((int32_t)c->x + (int32_t)c->width)   / 0x12;
        int32_t z1 = ((int32_t)c->z + (int32_t)c->density) / 0x12;
        if (los_seg_straddles_diag(ex, ez, px, pz, x1, z0, x0, z1)) return 1;  /* Anti-Diagonale */
        if (los_seg_straddles_diag(ex, ez, px, pz, x0, z0, x1, z1)) return 1;  /* Haupt-Diagonale */
    }
    return 0;
}

static int re15_enemy_los_probe(int slot, re15_actor_t *e, const re15_actor_t *player)
{
    uint8_t step = (uint8_t)(s_los_counter[slot] & 0x0f);
    s_los_counter[slot] = (uint8_t)((s_los_counter[slot] + 1) & 0x0f);
    if (step > 3) return 2;                               /* ticks 4..15: keep the old bit */
    /* FOV CONE +-0x5e8 (FUN_8001b84c @0x8001b898-b0, cluster A): a player OUTSIDE the +-0x5e8
     * facing cone is NOT seen (rear approaches stay unnoticed) — the 0x5e8 arg is the cone. */
    {
        int fb = ((int)re15_atan2_q12(player->z - e->z, player->x - e->x) - 0x400) & 0x0fff;
        int d = (((fb - (int)e->rot_y) + 0x800) & 0x0fff) - 0x800;
        if (d < -0x5e8 || d > 0x5e8) s_los_blocked[slot] = 1;
    }
    if (g_room_rdt_ok) {                                  /* byte-true Ray: Region `step` gegen
                                                           * FUN_8003dcc4 (s. Helper oben) —
                                                           * ersetzt den on_floor-Stand-in, der
                                                           * offene Flaechen dauerhaft blockte
                                                           * (crow_shot_attack.md F1) */
        if (re15_los_ray_blocked(e, player, step))
            s_los_blocked[slot] = 1;
    }
    if (step == 3) {                                      /* the verdict tick */
        int blocked = s_los_blocked[slot];
        s_los_blocked[slot] = 0;                          /* both paths clear bit 0x20 (@0x8001bd04) */
        if (!blocked) {
            e->steer_x = (int16_t)player->x;              /* the last-seen snapshot (@0x8001bc90-b0) */
            e->steer_z = (int16_t)player->z;
            return 1;
        }
        return 0;
    }
    return 2;
}

/* CORPSE / PRONE-SETTLE (root state 7 = FUN_80109554, shared m0/m1; raw-disasm'd C7 2026-07-05):
 * a killed zombie does NOT freeze — it lies in clip 0x15 (face-down) / 0x14 (face-up, after the
 * backward fall 0xb or when downed +0x9&0x80), its blood pool spreads (+0xbc/+0xbe += 8/frame for
 * ~91 frames — the shadow quad recolored 0x00ffff38, same mechanism as the player death pool),
 * the lying clip CREEPS with random 1..8-frame hiccups, then it TWITCHES: random pauses
 * (rand&0x1f)+1 alternating with fast (rate 0x400 = 4 frames/tick) clip replays, on a 0x1f4-frame
 * master budget — then rests forever (sub 4 is terminal for zombies; NO corpse ever returns to
 * ACTIVE — the @0x80109758/801098b4 writes are +0x5 sub-steps, not revives). Entry writers all
 * store the +0x4=7 halfword (+0x5=0). Uses grab_kill_ctr=+0x9e (pool budget 0x5a, then twitch
 * delay) and ai_timer=+0x9c (creep hiccup, then the 0x1f4 master). */
static void re15_enemy_corpse_settle(re15_actor_t *e)
{
    switch (e->sub_state_1) {
        case 0:                                   /* INIT (@0x8010959c), falls into sub1 same tick */
            e->grab_kill_ctr = 0x5a;              /* +0x9e = 90 pool-spread frames */
            e->sub_state_1 = 1;
            e->motion = (uint8_t)(((e->grid_id & 0x80) || e->motion == 0x0b) ? 0x14 : 0x15);
            e->anim_frame = 0;
            e->anim_frac = 0xf; e->anim_blend_rate = 0x100;   /* +0x8f=0xf (@0x801096a4) */
            e->ai_timer = 0;                      /* +0x9c = 0 (@0x801096b4) */
            /* fallthrough */
        case 1:                                   /* POOL-SPREAD + lying-clip creep (@0x80109704) */
            if (e->ai_timer == 0) {
                e->anim_frame++;                  /* f314 rate 0x100 (@0x801097e8) */
                if (re15_enemy_clip_done(e)) {
                    e->anim_frame = 0;
                    e->ai_timer = (int16_t)((re15_engine_rand8() & 7) + 1);  /* hiccup (@0x801097f8) */
                }
            } else e->ai_timer--;
            if (e->grab_kill_ctr > 0) e->grab_kill_ctr--;     /* the pool grows render-side */
            else {
                e->sub_state_1 = 2;                            /* (@0x80109750-78) */
                e->grab_kill_ctr = (int16_t)((re15_engine_rand8() & 0x1f) + 1);
                e->ai_timer = 0x1f4;                           /* master twitch budget = 500 */
            }
            break;
        case 2:                                   /* PAUSE (@0x80109824): anim frozen */
            if (--e->grab_kill_ctr <= 0) e->sub_state_1 = 3;
            if (e->ai_timer-- <= 0) e->sub_state_1 = 4;        /* master expiry overrides (@0x80109860) */
            break;
        case 3:                                   /* TWITCH (@0x80109884): rate 0x400 = 4 frames/tick */
            e->anim_frame = (uint16_t)(e->anim_frame + 4);
            if (re15_enemy_clip_done(e)) {
                e->anim_frame = 0;
                e->sub_state_1 = 2;                            /* (@0x801098a4-c8) */
                e->grab_kill_ctr = (int16_t)((re15_engine_rand8() & 0x1f) + 1);
            }
            break;
        default: break;                           /* 4 REST — terminal (@0x80109918) */
    }
}

/* KNOCKDOWN / GET-UP (+0x5=0x11) — byte-true FUN_8010512c (@0x8011f890[0x11], cluster F): the poise-
 * break target. [0] fall clip 0xb, +0x8f=0xf rate 0x100, +0x93|=1, +0x1dc=0x80 (downed sentinel),
 * +0x9|=0x80 (reroutes HURT->flinch / DEATH->downed clip 0x1f while down); [1] play out; [2] lie
 * timer +0x9c = tbl@0x8011FB10[rand&0xf] * 30; [3] count down; [4] get-up clip 0x12; [5] play out;
 * [6] +0x9&=0x7f, word 0x201 (engage re-entry + re-roll), +0x93&=0xfe, POISE re-arm (rand&3)+4.
 * Without this state every non-fatal hit froze the zombie forever (HURT always exits to 0x11). */
static void re15_enemy_ai_live_knockdown(re15_actor_t *e)
{
    static const uint32_t lie_tbl[16] = { 12,2,1,6,25,20,8,11,5,10,7,13,15,9,3,1 };  /* @0x8011FB10 */
    switch (e->sub_state_2) {
        case 0:
            e->motion = 0x0b; e->anim_frame = 0;
            e->anim_frac = 0xf; e->anim_blend_rate = 0x100;
            e->anim_flags &= (uint16_t)~0x04u;        /* HOLD-LAST lie-down/get-up (drop inherited walk LOOP) */
            e->hit_react |= 1;
            /* fall-grunt SE roll (FUN_8010512c.c [0]: `rand(); if ((r&3)==0) { r2=rand();
             * SE(r2&1 ? 5 : 8); }` — TWO draws, first one unconditional; keeping both draws
             * keeps the RNG stream byte-true). Dossier zombie_hit_1140.md D3. */
            if ((re15_engine_rand8() & 3) == 0)
                re15_audio_room_se((re15_engine_rand8() & 1) ? 5 : 8);
            e->hit_stun = (int16_t)0x80;              /* +0x1dc = 0x80 downed sentinel */
            e->grid_id |= 0x80;                       /* +0x9 |= 0x80 (downed reroute) */
            e->sub_state_2 = 1;
            break;
        case 2:
            e->ai_timer = (int16_t)(lie_tbl[re15_engine_rand8() & 0xf] * 30);
            /* byte-true lie-start latch clear (@0x801052e8-f4 in row [2]=0x80105278: `lbu +0x93;
             * andi 0xfe; sb`): the original re-arms damage the moment the zombie starts LYING —
             * the port held bit 0 until the [6] get-up exit, so a knocked-down zombie was
             * INVULNERABLE for the whole lie (original: shot lands -> prone flinch -> get-up).
             * (audit wf_827f186d zombie-live #2 verify correction) */
            e->hit_react &= (uint8_t)~1u;
            e->sub_state_2 = 3;
            break;
        case 3:
            if (--e->ai_timer <= 0) e->sub_state_2 = 4;
            re15_enemy_hold_last_frame(e);            /* lie still on the fallen frame */
            break;
        case 4:                                       /* GET-UP (@0x8010532c, row [4] of @0x801000c4) */
            e->motion = 0x12; e->anim_frame = 0;      /* +0x94 = 0x12 @0x80105338, +0x95 = 0 @0x80105348 */
            e->anim_frac = 0xf;                       /* +0x8f = 0xf @0x80105368 */
            e->sub_state_2 = 5;                       /* +0x6 = 5 @0x80105358 */
            e->hit_react |= 1;                        /* +0x93 |= 1 @0x80105378-84 (was missing) */
            /* get-up-grunt SE roll (FUN_8010512c.c [4]: `rand(); if ((r&7)==0) { r2=rand();
             * SE(r2&1 ? 5 : 8); }` — 1/8 chance, both draws byte-true). Dossier D3. */
            if ((re15_engine_rand8() & 7) == 0)
                re15_audio_room_se((re15_engine_rand8() & 1) ? 5 : 8);
            break;
        case 6:
            e->grid_id &= (uint8_t)0x7f;
            re15_ai_set_state_word(e, 0x201);         /* -> engage (entry re-roll) */
            e->hit_react &= (uint8_t)~1u;
            e->hit_stun = (int16_t)((re15_engine_rand8() & 3) + 4);   /* poise re-arm */
            break;
        default:                                       /* [1]/[5]: play to clip end */
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re15_enemy_clip_done(e));
            break;
    }
}

/* SLEEPING-LYING (+0x5=0x12) — byte-true FUN_801054f4 (@0x8011f890[0x12]; decide 0x80105470
 * raw-disasm'd): a dormant floor zombie. [0] sets the sleep flag +0x1b8=1; [1] WAITS — the DECIDE
 * wakes it (+0x6=2) when (+0x9&0x1f)==0xf (grid trigger) OR dist<0xbb8 && player alive; [2] plays
 * the wake-stir clip 0x2a with +0x8f=0 (hard cut); [3] clip done -> [4] +0x4=0xd01 (stand-up),
 * clears +0x1b8 and +0x93 bit 0. */
static void re15_enemy_ai_live_sleeping(re15_actor_t *e)
{
    switch (e->sub_state_2) {
        case 0:
            e->sub_state_2 = 1;                   /* +0x1b8 sleep flag: no port field consumer yet */
            break;
        case 2:
            e->motion = 0x2a; e->anim_frame = 0;
            e->anim_frac = 0;                     /* +0x8f = 0 — hard pose cut */
            e->sub_state_2 = 3;
            /* fallthrough */
        case 3:
            e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re15_enemy_clip_done(e));
            break;
        case 4:
            re15_ai_set_state_word(e, 0xd01);     /* -> stand-up */
            e->hit_react &= (uint8_t)~1u;
            break;
        default: break;                           /* [1] wait for the decide */
    }
}

/* CHARGE / fast-approach (+0x5=8) — byte-true FUN_80103014 (@0x8011f890[8]; decide FUN_80102f1c =
 * the engage gates MINUS turn/dead-feed). Entered from the search decides at dist>0x2711 with a 25%
 * roll (0x801 overwrite). The zombie plays its arms-up walk clip +0x1d4 at DOUBLE anim rate (f314
 * called TWICE @0x80103128/@0x80103144, both `jal 0x8001f314; ori a3,0x200` — citation corrected by
 * audit wf_827f186d #6: @0x801030dc is the aac4 steer, @0x801030f0 the +0x9c timer load, NOT the
 * anim calls) with hard 0x40/tick homing (aac4, no weave) for (rand&0x3f)+0x96
 * ticks, then drops to +0x5=2 by BYTE write — +0x6 stays 1, so the engage entry roll is SKIPPED
 * (the original quirk: stale gait fields carry over). */
static void re15_enemy_ai_live_charge(int slot, re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                    /* entry (+0x6==0) */
        e->sub_state_2 = 1;
        e->motion = e->hurt_clip;                 /* +0x94 = +0x1d4 (the arms-up walk variant) */
        e->anim_frame = 0;
        e->anim_frac = 7;                         /* +0x8f = 7 */
        e->anim_blend_rate = 0x200;               /* f314 rate 0x200 */
        e->speed_h = 0x19;                        /* +0x8c = 0x19 */
        e->ai_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 0x96);   /* +0x9c = 150..213 */
        if ((re15_engine_rand8() & 3) == 0)       /* 1-in-4 moan */
            re15_audio_room_se((re15_engine_rand8() & 1) ? 4 : 5);
        s_zfoot_ok[slot] = 0;
    }
    e->anim_flags |= 0x04u;                       /* LOOP the arms-up charge walk (see engage_animate) */
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, 0x40);   /* aac4(+0x1bc,+0x1be,0x40) */
    int16_t t = e->ai_timer;
    e->ai_timer = (int16_t)(t - 1);
    if (t == 0)
        e->sub_state_1 = 2;                       /* BYTE write +0x5=2 (+0x6 untouched — no re-roll) */
    e->anim_frame++;                              /* the SECOND f314: double anim rate (the shared
                                                   * pass adds the first advance) */
    re15_enemy_footlock_step(slot, e);            /* FUN_8010939c — carries the doubled stride */
    /* FUN_800245d8(0) @0x8010318c (cluster-B raw): a +0x8c=0x19 (25 u/tick) FORWARD slide along
     * the heading ON TOP of the foot-lock — the charge hustles faster than the clip cadence. */
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->speed_h) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->speed_h) >> 12);
}

/* Slew the heading toward a TARGET ANGLE by up to `step` per tick (byte-true func_0x8001aa68
 * semantics: returns the applied delta, 0 once aligned — the caller adds it to +0x6a). */
static int16_t re15_slew_to_angle(re15_actor_t *e, int target, int step)
{
    int d = ((target - (int)e->rot_y) + 0x800) & 0x0fff;   /* signed residual in [-0x800,0x800) */
    d -= 0x800;
    if (d == 0) return 0;
    int16_t r = (int16_t)(d > step ? step : (d < -step ? -step : d));
    e->rot_y = (int16_t)(((int)e->rot_y + r) & 0x0fff);
    return r;
}

/* CONTACT-STAGGER (+0x5=9) — byte-true FUN_801031e4 (@0x8011f890[9]): the bumped-from-ahead
 * reaction the engage decide rolls with (rand&1)+9 on firm front contact (+0x90&3 && heading
 * ahead). 3-clip chain: 0x17 intro -> 0x18 hold (the decide 0x801031a4 releases it: +0x6=4 once
 * +0x1c0&0x8000 clears) -> 0x19 recover (frame seed 1, SE 0) -> exit 0x201 engage (also clears
 * +0x93, +0x1b8, seeds +0x1dc=3, drops grid bit 0x80). */
static void re15_enemy_ai_live_contact_stagger(re15_actor_t *e)
{
    switch (e->sub_state_2) {
        case 0:
            e->sub_state_2 = 1;
            e->motion = 0x17; e->anim_frame = 0;
            e->anim_frac = 7; e->anim_blend_rate = 0x200;
            e->hit_react |= 1;                      /* entity+0x93 = 1 (in-reaction flag) */
            break;
        case 2:
            e->sub_state_2 = 3;
            e->motion = (uint8_t)(e->motion + 1);   /* 0x17 -> 0x18 */
            e->anim_frame = 0;
            /* fallthrough: [3] plays without the f314 advance-add */
        case 3:
            /* [3] HOLDS clip 0x18; the DECIDE releases it (+0x6=4 while +0x1c4&0x8000 clear — the
             * port has no +0x1c0 writer yet, so the decide releases immediately = a short hold). */
            return;
        case 4:
            e->sub_state_2 = 5;
            e->motion = 0x19; e->anim_frame = 1;    /* +0x95 = 1 (@0x8010325c) */
            e->hit_react &= (uint8_t)~1u;
            re15_audio_room_se(0);
            break;
        case 6:
            re15_ai_set_state_word(e, 0x201);       /* exit -> engage */
            e->hit_react &= (uint8_t)~1u;
            e->hit_stun = 3;                        /* +0x1dc = 3 */
            e->grid_id &= (uint8_t)0x7f;            /* +0x9 &= 0x7f */
            return;
        default: break;                             /* [1]/[5]: play + advance below */
    }
    e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re15_enemy_clip_done(e));
}

/* EDGE-FALL (+0x5=0xa) — byte-true FUN_801033c8 (@0x8011f890[0xa]): the OTHER contact roll — the
 * zombie walks INTO the contact heading (clip 1 while slewing +0x10/tick to (+0x90&0xf0)<<4), then
 * TUMBLES OVER THE EDGE: clip 0x16, yaw snapped to the contact heading, +0x8c=0x474; at clip frame
 * 0x46 the fall lands (fall-vel +0xb0=0x474, HP -= 0x708=1800); clip done -> +0x82(floor) += 1,
 * Y -= 0x708 (the literal store `+0x38 += -0x708`; the port world IS the PSX frame — copy the
 * bytes, not an interpretation), +0x4 = 1 (idle re-init on the lower floor). THE BRIEFING-ROOM
 * PIT: a zombie chasing across the floor hole falls in. */
static void re15_enemy_ai_live_edge_fall(re15_actor_t *e)
{
    switch (e->sub_state_2) {
        case 0:
            e->sub_state_2 = 1;
            e->motion = 0x01; e->anim_frame = 0;
            e->anim_frac = 7; e->anim_blend_rate = 0x200;
            /* fallthrough into the [1] turn-to-contact */
        case 1: {
            int heading = ((int)(e->ai_contact & 0xf0) << 4) & 0x0fff;
            if (re15_slew_to_angle(e, heading, 0x10) == 0)
                e->sub_state_2 = 2;                 /* aligned -> the stumble */
            return;
        }
        case 2:
            e->sub_state_2 = 3;
            e->motion = 0x16; e->anim_frame = 0;
            e->anim_frac = 7; e->anim_blend_rate = 0x200;
            e->speed_h = 0x474;                     /* +0x8c = 0x474 */
            e->rot_y = (int16_t)((((int)(e->ai_contact & 0xf0) << 4)) & 0x0fff);  /* yaw snap */
            /* fallthrough */
        case 3:
            /* frame 0x46 (@0x80103554): +0xb0 = 0x474 fall velocity + +0x1ba -= 0x708 = the GROUND-Y
             * CACHE moves one band (cluster F0.1: +0x1ba is NOT hp — the original deals NO damage
             * here; the port's y/floor update below covers the ground shift; +0xb0 vel deferred). */
            if (re15_enemy_clip_done(e)) {          /* fall complete -> the floor below */
                e->floor = (uint8_t)(e->floor + 1);
                e->y -= 0x708;                      /* the literal +0x38 += -0x708 */
                re15_ai_set_state_word(e, 0x0001);  /* +0x4 word = 1 -> state 1 / +0x5 = 0 idle */
            }
            return;
        default: return;
    }
}

/* PUSH-OFF / Wegstoß-Stagger (+0x5=0xb) — byte-true FUN_8010385c (@0x8011f890[0xb]; decide
 * @0x8011f840[0xb] = jr-ra stub). Entered via the 0xb01 state-word: the grab machine's DOMINO shove
 * ([5] writes 0xb01 into the contacted bystander +0x1ac) — the zombie is KNOCKED back: one-shot
 * clip 0x10 with a sharp backward slide (speed +0x8c=300 decaying +0x9e=0x32/tick, min 10), then
 * back to the engage brain. Live-observed in the original mash-escape (ss1=0xb s6=7 mo=0x10). */
static void re15_enemy_ai_live_pushoff(re15_actor_t *e, re15_actor_t *player)
{
    if (e->sub_state_2 == 0) {                /* entry (+0x6==0) */
        e->motion = 0x10; e->anim_frame = 0;  /* +0x94 = 0x10, +0x95 = 0 */
        e->sub_state_2 = 7;                   /* +0x6 = 7 (the original's marker sub-step) */
        e->anim_frac = 7;                     /* +0x8f = 7 */
        e->anim_blend_rate = 0x200;           /* f314 rate 0x200 (per-frame call) */
        e->speed_h = 300;                     /* +0x8c = 300 — the knockback impulse */
        e->grab_kill_ctr = 0x32;              /* +0x9e = 0x32 — the per-tick decay step */
        re15_audio_room_se(0);                /* func_0x800453d0(0) */
        if (player) player->hit_react &= (uint8_t)~1u;   /* DAT_800acae7 &= 0xfe */
    }
    if (re15_enemy_clip_done(e)) {            /* f314 done -> +0x4 = 0x201 (engage) */
        re15_ai_set_state_word(e, 0x201);
        return;
    }
    /* func_0x800245d8(0x800): slide BACKWARD along yaw+180° at +0x8c, decaying by +0x9e, min 10. */
    e->x -= (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->speed_h) >> 12);
    e->z += (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->speed_h) >> 12);
    e->speed_h = (int16_t)(e->speed_h - e->grab_kill_ctr);
    if (e->speed_h < 10) e->speed_h = 10;
}

/* Turn-to-face ANIMATE (@0x8011f890[7] = FUN_80102dc8, STAGE1.BIN) — the +0x5=7 TURN state's
 * movement half (byte-true, disasm-verified). The active zombie enters +0x5=7 from the engage
 * (re15_ai_decide_engage 0x701: close <0x7d0 but OUTSIDE the narrow 0x2c8 arc -> turn to face). Each
 * frame it rotates the heading toward the player by the arc_test(player, 0x80) residual (the
 * @0x80102ecc-0x80102ee4 path: `entity+0x6a += arc_test`), i.e. ±0x80 (~7deg) per frame toward the
 * player, 0 once within the ±0x80 cone. NO translation (the turn state stands and pivots; the only
 * forward locomotion is the anim-root-motion walk in the +0x5=5/6 devour states, IMPLEMENTED @L2059-2095). The companion
 * DECIDE half f840[7] (re15_ai_dispatch_decision case 7) commits the GRAB as soon as the heading is
 * within the wider ±0x200 grab cone — so the zombie turns until it faces the player, then grabs.
 * (The within-±0x80 fine slew FUN_8001a8f8(player,0x201) @0x80102e90 is moot here: the ±0x200 grab
 * commit fires first since ±0x80 is inside ±0x200.) */
static void re15_enemy_ai_live_turn(re15_actor_t *e, const re15_actor_t *player)
{
    if (!player) return;
    /* TURN ANIMATE entry latch (@0x80102de0-e18): on the first turn frame (+0x6==0; the engage's
     * 0x701 commit reset +0x6 to 0) set the turn clip +0x94 = +0x1d4 variant {2,3,4,5} + +0x95=0,
     * then latch +0x6=1 so it is set only once on entry. */
    if (e->sub_state_2 == 0) {
        e->motion     = e->hurt_clip;   /* +0x94 = +0x1d4 (@0x80102e00/08) */
        e->anim_frame = 0;              /* +0x95 = 0 (@0x80102e18) */
        e->anim_frac  = 7;              /* +0x8f = 7 (decompile line 17) — the engage->turn blend
                                         * (was a hard pose pop) */
        e->anim_blend_rate = 0x200;     /* turn f314 rate 0x200 */
        if ((re15_engine_rand8() & 3) == 0)   /* 1-in-4 moan on turn entry (lines 19-25) */
            re15_audio_room_se((re15_engine_rand8() & 1) ? 4 : 5);
        e->sub_state_2 = 1;             /* +0x6 = 1 entry latch (@0x80102df0) */
    }
    int16_t turn = (int16_t)re15_ai_arc_test(e, player->x, player->z, 0x80);  /* ±0x80 toward player */
    /* byte-true FUN_80102dc8 @0x80102e5c: once WITHIN the ±0x80 cone (arc_test==0 = facing the
     * player), transition BACK TO ENGAGE (+0x5=2). The engage then rolls the behavior table and
     * breaks into the forward APPROACH — so the zombie turns to face, then WALKS at the player.
     * Was MISSING -> the zombie pivoted forever at medium range and never approached (8.19). */
    /* byte-true FUN_80102dc8 @0x80102e74-e78 `sll v0,s1,16; bne v0,zero,0x80102ecc`: the WHOLE
     * exit block — the a8f8(player,0x80) fine slew @0x80102e90, the 0x201 engage word @0x80102e94
     * AND the two exit rand writes +0x9e @0x80102eb4 / +0x9f @0x80102ec8 — is inside the
     * arc_test==0 branch and runs exactly ONCE, on the frame the heading enters the ±0x80 cone.
     * Off-cone frames jump straight to `+0x6a += arc` @0x80102ecc-ee4 (±0x80/frame pivot only).
     * (Audit wf_827f186d zombie-live #1: the port's brace-less if ran the snap + 2 RNG draws
     * EVERY turn tick — the zombie faced the player within one frame instead of slewing.) */
    if (turn == 0) {
        re15_ai_set_state_word(e, 0x201);                                     /* +0x5 = 2 (engage) */
        /* a8f8(&player, 0x80) @0x80102e90: within the +-0x80 cone the slew SNAPS -> the zombie
         * leaves the turn facing the player EXACTLY (feeds the next walk segment's geometry). */
        e->rot_y = (int16_t)(((int)re15_atan2_q12(player->z - e->z, player->x - e->x) - 0x400) & 0x0fff);
        {
            int ts = (int)(e - g_actors);                       /* exit rand writes (lines 33-36) */
            s_wander_mag[ts] = (uint8_t)((re15_engine_rand8() & 0x1f) + 8);   /* +0x9e */
            s_wander_idx[ts] = (uint8_t)((re15_engine_rand8() & 1) + 1);      /* +0x9f */
        }
    }
    e->rot_y = (int16_t)(((int32_t)e->rot_y + turn) & 0x0fff);                 /* +0x6a += residual */
}

/* ================= FORWARD-WALK / APPROACH (Phase 8.19, the piece the port had STUBBED) ==========
 * The ORIGINAL ROOM1140 zombie, once engaged, does NOT stand still: FUN_801021f8 (engage animate)
 * rolls a behavior byte from a table and BREAKS INTO A FORWARD APPROACH, walking UPRIGHT toward the
 * player before it grabs (validated at the real game: stage_saves/mzd_stage1_walked.sav slot 2 =
 * +0x5=6 WALK @dist 1731; shots/ORIG_wake.png frame #40 = a zombie walking upright at the player).
 * The port's engage was a bare `e->motion = e->hurt_clip` stub -> the zombie stood forever = the
 * user-reported "everything wrong". Byte-true anchors: behavior tables @0x8011faf0 (>=5 alive, more
 * aggressive) / @0x8011fb00 (<5), rolled rng&0x1f; value 2 -> +0x5=0x13 approach; walk clip = +0x5+4
 * (@0x80102bfc) = 0x0a; forward root-motion func_0x8001ad68 @0x8011f890[5/6]=FUN_80102bd8. */
static const uint8_t s_zbehavior_5plus[32] = {
    0,1,2,1,2,0,2,2,2,2,1,2,2,1,2,1,0,1,0,0,1,0,0,1,2,0,0,0,2,1,0,0 };   /* @0x8011faf0 */
static const uint8_t s_zbehavior_lt5[32] = {
    0,1,0,0,1,0,0,1,2,0,0,0,2,1,0,0,12,0,0,0,2,0,0,0,1,0,0,0,6,0,0,0 };  /* @0x8011fb00 */

/* ENEMY SPAWN COUNTER (byte-true DAT_800aca4e, cluster C6): incremented ONCE per spawn
 * (@0x80042558-60), reset at room init (@0x8003f014), NEVER decremented — corpses stay counted,
 * so a >=5-spawn room keeps the aggressive behavior table even after kills (the port's live count
 * dropped to the docile <5 table). */
static int s_enemy_spawn_count = 0;
void re15_enemy_spawn_count_inc(void)   { s_enemy_spawn_count++; }
void re15_enemy_spawn_count_reset(void) { s_enemy_spawn_count = 0; }
int  re15_enemy_spawn_count(void)       { return s_enemy_spawn_count; }

/* Count the live briefing zombies (byte-true DAT_800aca4e drives the behavior-table pick). */
static int re15_enemy_live_count(void)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *z = &g_actors[i];
        if (z->active && (z->type == 0x10 || z->type == 0x11 || z->type == 0x16)
            && z->state != (uint8_t)RE15_AI_STATE_CORPSE)
            n++;
    }
    return n;
}

/* func_0x8001aac4 (@0x8001aac4, EXE decompile-verified) — the WALK steer: slew the heading toward
 * the POINT (tx,tz) at |slew|/tick; NEGATIVE slew steers AWAY (bearing+0x800) — the gait rows'
 * -1 segments are the drunken-weave wiggle the walks are authored around. Byte-true math:
 *   bearing = a6d4(self, target)   (the yaw value that faces the point; port: atan2-0x400)
 *   slew<0 -> slew=-slew, bearing+=0x800
 *   delta = (slew + bearing - yaw) & 0xfff
 *   delta < 2*slew -> SNAP yaw=bearing; else yaw-=slew, and delta<0x801 -> yaw+=2*slew (short side).
 * slew==0 (the zero gait rows, roll 6/12) is a NO-OP = walk dead straight. */
void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew)
{
    if (slew == 0) return;
    int bearing = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff;
    if (slew < 0) { slew = -slew; bearing = (bearing + 0x800) & 0x0fff; }
    int yaw   = (int)e->rot_y & 0x0fff;
    int delta = (slew + bearing - yaw) & 0x0fff;
    if (delta < 2 * slew)
        e->rot_y = (int16_t)bearing;
    else {
        yaw = (yaw - slew) & 0x0fff;
        if (delta < 0x801) yaw = (yaw + 2 * slew) & 0x0fff;
        e->rot_y = (int16_t)yaw;
    }
}

/* Per-actor FOOT-LOCK state (byte-true FUN_8010939c): the planted support-foot's world pos, cached
 * from the previous tick, so the body can be dragged by the foot's per-tick world delta. By slot. */
static int32_t s_zfoot_ref[RE15_ACTOR_MAX][3];
static uint8_t s_zfoot_ok[RE15_ACTOR_MAX];
static uint8_t s_zfoot_sel[RE15_ACTOR_MAX];

/* Per-actor APPROACH WANDER state (byte-true FUN_801057bc): 0x9e=slew magnitude, 0x9f=table index,
 * 0x9c=segment timer. The look-at target (+0x1bc/+0x1be) is the LIVE player (updated @FUN_8010e6d4). */
static uint8_t s_wander_mag[RE15_ACTOR_MAX];   /* +0x9e */
static uint8_t s_wander_idx[RE15_ACTOR_MAX];   /* +0x9f */
static int16_t s_wander_tmr[RE15_ACTOR_MAX];   /* +0x9c */
/* @0x8011fb50: {u16 segment_timer, s16 rot_dir} x16, indexed by +0x9f (STAGE1.BIN, byte-verified). */
static const struct { uint16_t tmr; int16_t rot; } s_wander_tbl[16] = {
    {200,1},{80,-1},{210,1},{100,-1},{180,1},{90,-1},{150,1},{75,-1},
    {110,1},{100,-1},{150,1},{80,-1},{130,1},{70,-1},{130,1},{90,-1}
};

/* ENGAGE-walker gait rows (byte-true FUN_801021f8 @0x8011f9f0 + variant*0x80, W1 disasm 2026-07-03):
 * 32 x {u16 segment_timer, s16 rot_dir}, indexed by +0x9f (wrap &0x1f). The +0x1de gait VARIANT is
 * the s_zbehavior roll: 0 -> row0 (@0x8011f9f0), 1 -> row1 (@0x8011fa70), 2 -> SWITCH to the 0x13
 * lurch, other values (6/12 in the <5 table) -> all-zero rows = walk dead straight. */
/* THE GAIT REGION BLOB — STAGE1.BIN bytes 0x1f9f0..0x2006f verbatim (@0x8011f9f0, 13 x 0x80).
 * The original's row fetch `@0x8011f9f0 + variant*0x80 + idx*4` has NO bound check (cluster C):
 * variants 0/1 = the real gait rows; variant 2 = the BEHAVIOR TABLES reinterpreted as rows
 * (rot 258/514/513/258 at idx 0-3 -> the heading SNAPS onto the steer point on the roll==2
 * pass-through frame, ~6/32); variant 6 = zeros (straight, timer expires per frame = 1 rand
 * draw/frame); variant 12 = pointer bytes (a 30260-frame rot=-32752 latch = the "wanders off"
 * zombie: snap-toward for even pace, snap-AWAY for odd). Embedding the region byte-for-byte
 * reproduces every quirk. */
static const uint8_t s_gait_blob[0x680] = {
    0x96,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0x0a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0x0a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0xc8,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0x0a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0x0a,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0xfa,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0x0a,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0x28,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0x5a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0x2c,0x01,0x01,0x00,0x14,0x00,0xff,0xff,0x0a,0x00,0x01,0x00,0x19,0x00,0xff,0xff,
    0x14,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,0xbe,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0x0a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,0x14,0x00,0x01,0x00,0x0f,0x00,0xff,0xff,
    0x96,0x00,0x01,0x00,0x28,0x00,0xff,0xff,0xa0,0x00,0x01,0x00,0x32,0x00,0xff,0xff,
    0x6e,0x00,0x01,0x00,0x28,0x00,0xff,0xff,0x14,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0x82,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,0x14,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0xd2,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,0x14,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0x14,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,0x0a,0x00,0x01,0x00,0x14,0x00,0xff,0xff,
    0xc8,0x00,0x01,0x00,0x28,0x00,0xff,0xff,0x1e,0x00,0x01,0x00,0x19,0x00,0xff,0xff,
    0x14,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,0xbe,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0x14,0x00,0x01,0x00,0x0a,0x00,0xff,0xff,0x78,0x00,0x01,0x00,0x1e,0x00,0xff,0xff,
    0x00,0x01,0x02,0x01,0x02,0x00,0x02,0x02,0x02,0x02,0x01,0x02,0x02,0x01,0x02,0x01,
    0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x02,0x01,0x00,0x00,
    0x0c,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
    0x19,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x0b,0x00,0x00,0x00,
    0x05,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x0d,0x00,0x00,0x00,
    0x0f,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
    0xc8,0x00,0x01,0x00,0x50,0x00,0xff,0xff,0xd2,0x00,0x01,0x00,0x64,0x00,0xff,0xff,
    0xb4,0x00,0x01,0x00,0x5a,0x00,0xff,0xff,0x96,0x00,0x01,0x00,0x4b,0x00,0xff,0xff,
    0x6e,0x00,0x01,0x00,0x64,0x00,0xff,0xff,0x96,0x00,0x01,0x00,0x50,0x00,0xff,0xff,
    0x82,0x00,0x01,0x00,0x46,0x00,0xff,0xff,0x82,0x00,0x01,0x00,0x5a,0x00,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x5b,0x10,0x80,0x7c,0x5b,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x5b,0x10,0x80,0x7c,0x5b,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x5b,0x10,0x80,0x7c,0x5b,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x90,0x62,0x10,0x80,0x90,0x62,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x90,0x62,0x10,0x80,0x90,0x62,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x24,0x66,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x24,0x66,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x48,0x60,0x10,0x80,0x48,0x60,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x24,0x5a,0x10,0x80,0x4c,0x5a,0x10,0x80,0x4c,0x5a,0x10,0x80,0x6c,0x5a,0x10,0x80,
    0x6c,0x5a,0x10,0x80,0x6c,0x5a,0x10,0x80,0x6c,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,
    0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,
    0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,0x24,0x5a,0x10,0x80,
    0x24,0x5a,0x10,0x80,0x2c,0x5a,0x10,0x80,0x2c,0x5a,0x10,0x80,0x2c,0x5a,0x10,0x80,
    0x2c,0x5a,0x10,0x80,0xc8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x06,0x08,0x06,0x07,0x06,0x08,0x08,0x07,0x64,0x00,0x00,0x00,
    0x38,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x6c,0x10,0x80,
    0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x6c,0x10,0x80,
    0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x6c,0x10,0x80,
    0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x6c,0x10,0x80,
    0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x6c,0x10,0x80,
    0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xbc,0x8a,0x10,0x80,
    0xdc,0x6e,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x44,0x72,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xbc,0x8a,0x10,0x80,
    0xe0,0x7e,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x44,0x72,0x10,0x80,0x18,0x6c,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x76,0x10,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
static uint16_t gait_tmr(unsigned v, unsigned idx)
{
    unsigned off = (v & 0xff) * 0x80u + (idx & 0x1f) * 4u;
    if (off + 3 >= sizeof s_gait_blob) return 0;
    return (uint16_t)(s_gait_blob[off] | (s_gait_blob[off + 1] << 8));
}
static int16_t gait_rot(unsigned v, unsigned idx)
{
    unsigned off = (v & 0xff) * 0x80u + (idx & 0x1f) * 4u;
    if (off + 3 >= sizeof s_gait_blob) return 0;
    return (int16_t)(s_gait_blob[off + 2] | (s_gait_blob[off + 3] << 8));
}
static uint8_t s_gait_variant[RE15_ACTOR_MAX];   /* +0x1de per slot (persisted while engaged) */

/* APPROACH animate — byte-true FUN_801057bc (@0x8011f890[+0x5=0x13], STAGE1.BIN): the LIVE walk gait.
 * Plays the LOCOMOTION bank's clip 1 (bank0, entity+0x84 — the 99-frame approach walk) and carries
 * the body forward via the FUN_8010939c FOOT-LOCK: clip 1 has NET-ZERO keyframe root-motion (it
 * sways), so the forward travel EMERGES from the planted foot — p.X/Z -= (footWorld - footPrev)
 * (identical to the stair FK foot-lock FUN_800390e0, stair_common.c). The SUPPORT foot each frame is
 * chosen by the clip's per-keyframe 0x2000 flag (bank0 clip1 frames 30-83; foot bone 6 right / 3 left,
 * the leg-chain leaves). Rotates to track the player; the decide (re15_ai_decide_engage = FUN_8010561c)
 * commits the GRAB in range. This is the byte-true LIVE approach (+0x5=6 clip 0xa is the corpse-walk). */
static void re15_enemy_footlock_step(int slot, re15_actor_t *e);   /* fwd (defined below) */

static void re15_enemy_ai_live_approach(int slot, re15_actor_t *e, const re15_actor_t *player)
{
    if (!player || slot < 0 || slot >= RE15_ACTOR_MAX) return;
    if (e->sub_state_2 == 0) {                        /* entry (@0x801057c0-6c8): clip1 + wander seed */
        e->sub_state_2 = 1;
        s_wander_mag[slot] = (uint8_t)((re15_engine_rand8() & 7) + 8);   /* +0x9e = (rng&7)+8 */
        e->motion = 0x01;                                                /* +0x94 = clip 1 */
        e->anim_frame = (uint16_t)(re15_engine_rand8() & 0x1f);          /* +0x95 = rng & 0x1f (random phase) */
        e->anim_frac  = 0xf;                                             /* +0x8f */
        e->anim_blend_rate = 0x100;                                      /* walk-family f314 rate */
        if ((re15_engine_rand8() & 3) == 0)                             /* 1/4: SE 4 or 5 */
            re15_audio_room_se((re15_engine_rand8() & 1) ? 4 : 5);
        s_wander_idx[slot] = (uint8_t)(re15_engine_rand8() & 0xf);       /* +0x9f = rng & 0xf */
        s_wander_tmr[slot] = (int16_t)s_wander_tbl[s_wander_idx[slot]].tmr;  /* +0x9c */
        s_zfoot_ok[slot] = 0;
    }
    /* wander slew: sVar7 = +0x9e * table[+0x9f].rot (= +-(8..15)) — SIGNED into the aac4 steer at the
     * STEER SNAPSHOT +0x1bc/+0x1be (decompile FUN_801057bc:40), NOT the live player: negative rows
     * wiggle AWAY. The old port homed |sVar7| onto the LIVE player -> the facing error never grew ->
     * the TURN state never fired -> the wake-roll was absorbing (a 0x13 roll = a permanent arms-down
     * shambler glued to the player = the user report). Every table[+0x9f].tmr frames advance the index
     * (wrap 0xf) and add a random +-sVar7 jitter to +0x6a (FUN_801057bc @0x8010572c-58). */
    int16_t sVar7 = (int16_t)((int16_t)s_wander_mag[slot] * s_wander_tbl[s_wander_idx[slot]].rot);
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, sVar7);
    int16_t was = s_wander_tmr[slot];
    s_wander_tmr[slot] = (int16_t)(was - 1);
    if (was == 0) {
        s_wander_idx[slot] = (uint8_t)((s_wander_idx[slot] + 1) & 0xf);
        s_wander_tmr[slot] = (int16_t)s_wander_tbl[s_wander_idx[slot]].tmr;
        int16_t flip = (int16_t)(sVar7 - (int16_t)(re15_engine_rand8() & 1) * (int16_t)(sVar7 * 2));
        e->rot_y = (int16_t)(((int32_t)e->rot_y + flip) & 0x0fff);
    }

    re15_enemy_footlock_step(slot, e);
}

/* The FUN_8010939c FOOT-LOCK, factored (used by the 0x13 lurch AND the ENGAGE walker): pose the loco
 * bank's CURRENT clip (e->motion — clip 1 lurch / clips 2-5 engage walks), pick the support foot by
 * the per-frame 0x2000 EDD flag, and drag the body by the planted foot's world delta. */
static void re15_enemy_footlock_step(int slot, re15_actor_t *e)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_enemy_bank_t *bank = re15_enemy_find(e->type);
    if (!bank || !bank->loco_ok || slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_emd_animation_t *a = &bank->anim_loco;
    re15_emd_skeleton_t  *s = &bank->skel_loco;
    int ci = (int)e->motion;
    if (ci < 0 || ci >= a->clip_count) return;
    const re15_emd_clip_t *clip = &a->clips[ci];
    if (clip->frame_count <= 0) return;
    int fslot = (int)((uint32_t)e->anim_frame % (uint32_t)clip->frame_count);
    uint32_t fe  = a->frames[clip->first_frame + fslot];   /* the current EDD frame entry (with flags) */
    int      sel = (fe & 0x2000) ? 1 : 0;                    /* the byte-true support-foot flag */
    int      foot = sel ? 6 : 3;                             /* right leaf / left leaf (bind-pose feet) */
    if (foot >= s->bone_count) return;
    int kf = re15_compute_actor_kf(a, s, e, ci, (uint32_t)e->anim_frame);
    g_anim_pose_actor = NULL;                                /* pose QUERY, not a render (no crossfade) */
    if (re15_skel_compute_pose(s, kf, poses) != 0) return;
    int32_t lx = poses[foot].trans[0], lz = poses[foot].trans[2];
    int32_t cs = re15_cos_q12(e->rot_y), sn = re15_sin_q12(e->rot_y);   /* foot local->world by facing */
    int32_t wx = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
    int32_t wz = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
    if (s_zfoot_ok[slot] && sel == s_zfoot_sel[slot]) {      /* same planted foot -> drag the body */
        e->x -= (wx - s_zfoot_ref[slot][0]);
        e->z -= (wz - s_zfoot_ref[slot][2]);
    }
    s_zfoot_ref[slot][0] = wx; s_zfoot_ref[slot][2] = wz;
    s_zfoot_ok[slot] = 1; s_zfoot_sel[slot] = (uint8_t)sel;
}

/* ================== ENTITY BODY COLLISION (byte-true FUN_8002aec4 + FUN_8002b544) ================
 * The walk-through blocker (W2 disasm 2026-07-03): a mutual CYLINDER PUSH-OUT run on every entity's
 * own tick — a positional slide, never a move veto. FUN_8002aec4(pusher, pushee) MOVES THE PUSHEE:
 * pen = (r1+r2) - dist2D; pushee.pos += d * pen / (dist+1). Radii/heights from the hitbox structs:
 * PLAYER = {r 450, half_h 1530} (PSX.EXE file 0x64694: 00 00 06 fa 00 00 c2 01 fa 05 c2 01), ZOMBIE
 * = {r 400, half_h 1440} (STAGE1.BIN file 0x1f778) = the port's hit_radius_min/hit_height. Call
 * sites: the player tick FUN_80031c44 runs FUN_8002b544() AFTER the cmd-FSM move and BEFORE the SCA
 * wall resolver (walls win) -> the PLAYER is pushed out of every enemy; the zombie tick FUN_8010a8c8
 * runs aec4(&player, zombie) + b544 -> the ZOMBIE is pushed out of the player + the other zombies.
 * This is also what holds the GRABBED pair at ~850 = 400+450 (observed 822 live) — the pull-in root
 * motion presses in, the push holds them apart. Skip gates: the original skips when BOTH carry the
 * 0x1000 freeze or status&2 — port-observable equivalent: skip pairs involving a DEAD player (the
 * devour zombie overlaps the corpse, observed d=48) or a CORPSE zombie (state 7). Ellipse radii
 * (hitbox +6 != +0xa) degenerate to circles for player/zombies (450/450, 400/400) — circle only. */
#define RE15_BODY_R_PLAYER  450   /* PSX.EXE hitbox @file 0x64694: 0x1c2 */
#define RE15_BODY_H_PLAYER 1530   /* 0x5fa */

static int32_t re15_body_isqrt(int64_t v)
{
    if (v <= 0) return 0;
    /* The body push (FUN_8002aec4) computes dist via BIOS SquareRoot0 at 0x8002b1d8 (jal 0x80065f60)
     * on dx²+dz² (the ratan2/rsin/rcos above it is the ellipse radius-in-direction, DATA-DEAD since
     * every shipped box is circular). NOT floor(sqrt) — route through the byte-true replica on the
     * low 32 bits like the PSX mflo. Audit wf_f066b2ae (corrects the earlier "immaterial" note). */
    return (int32_t)re15_squareroot0((uint32_t)v);
}

/* Anisotropic-ellipse effective radius toward (ox,oz), byte-true FUN_8002aec4 @0x8002af58-b05c.
 * rx = box[+6] (along-heading, hit_radius_min), rz = box[+0xa] (lateral, hit_radius_max). For a
 * circular box (rx==rz — the @0x8002af68 beq skip) it is just rx; only the alligator (0x23) differs.
 *   bearing = ratan2(dz,dx) (0=+X); rel = bearing - rot_y folded to a [0,0x400] triangle wave;
 *   rz<rx: eff = rz + (rx-rz)*rcos(s4)/4096   (full rx along heading, rz perpendicular)
 *   else : eff = rx + (rz-rx)*rsin(s4)/4096 */
static int32_t body_eff_radius(const re15_actor_t *a, int32_t rx, int32_t ox, int32_t oz)
{
    int32_t rz = (int32_t)a->hit_radius_max;
    if (rz <= 0) return rx;                                              /* no lateral radius (scalar-only caller) */
    return re15_ellipse_radius(rx, rz, (int32_t)a->rot_y, (int32_t)oz - a->z, (int32_t)ox - a->x);
}

int re15_body_push(const re15_actor_t *pusher, int32_t r_pusher,
                   re15_actor_t *pushee, int32_t r_pushee)
{
    /* entity+0x0 FLAG GATES (byte-true FUN_8002aec4 @0x8002af04-af30): bit 0x2 on EITHER actor or
     * bit 0x4 on the PUSHEE skips the pair entirely. (The third gate — bit 0x1000 on BOTH = the
     * grab-pair freeze — is a word bit the port models via the state checks at the call sites.)
     * The port only ever sets flags=0x01 today, but Member_set (id 6) can script-write the byte,
     * so the gates are live mechanism, not dead code. */
    if ((pusher->flags | pushee->flags) & 0x2) return 0;
    if (pushee->flags & 0x4) return 0;
    /* Full 32-bit delta (byte-true FUN_8002aec4 @0x8002af8c): the original reads the two positions
     * with `lh` (s16) and takes their `subu` difference — it does NOT truncate the DIFFERENCE to s16.
     * Room coords are s16-range so the delta fits, and the fast-reject + isqrt handle the full width;
     * the old `(int16_t)` cast wrapped a delta > 32768 into a spurious close push (the "s16-truncated
     * like the original" comment mis-read the position `lh` as a delta truncation). */
    int32_t dx = pushee->x - pusher->x;
    int32_t dz = pushee->z - pusher->z;
    /* R = the two effective radii toward each other (byte-true @0x8002b164 radSum). For circular
     * boxes this is r_pusher + r_pushee; for the alligator (0x23) the ellipse blends by bearing. */
    int32_t R  = body_eff_radius(pusher, r_pusher, pushee->x, pushee->z)
               + body_eff_radius(pushee, r_pushee, pusher->x, pusher->z);
    if (dx > R || dx < -R || dz > R || dz < -R) return 0;     /* fast reject (aec4 pre-tests) */
    int32_t dist = re15_body_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
    int32_t pen  = R - dist;
    if (pen < 1) return 0;                                    /* @aec4: pen<1 -> no hit */
    /* Y BAND GATE (byte-true @0x8002b1f0-b228, evaluated AFTER the XZ circle test — trace
     * wf_518cceff, adversarially CONFIRMED): the push requires STRICT
     *     -(hA + hB) < dy < +(hA + hB)
     * with dy = (B.y + ofsB.y) - (A.y + ofsA.y), h = the +0x78 box[+8] half-heights
     * (port hit_height) and ofs.y = the +0x7c Y centre offsets (port hit_offset_y) —
     * bodies stacked vertically (an enemy above/below a ledge) never push each other.
     * Gated to pairs that CARRY heights: a 0-height actor has no +0x78 box and is never
     * in the original's push loop (the port's callers already skip hit_radius_min==0).
     * Flat-room safety (verified): same-floor actors give |dy| ~ tens vs hsum ~ 2970. */
    int32_t hsum = (int32_t)pusher->hit_height + (int32_t)pushee->hit_height;
    if (hsum > 0) {
        int32_t dy = (pushee->y + pushee->hit_offset_y) - (pusher->y + pusher->hit_offset_y);
        if (!(-hsum < dy && dy < hsum)) return 0;
    }
    /* ANISOTROPIC ellipse radius (@0x8002af68-b160) is now PORTED above (body_eff_radius) — it is
     * NOT data-dead: the alligator (0x23) box is {..,2200,720,800}, box[+6] != box[+0xa] (audit
     * wf_8b1360d4 refuted the old "every shipped box is circular" note). Still DATA-DEAD (documented,
     * not ported — trace wf_518cceff):
     *  - ANTI-TUNNELING crossing ejection (@0x8002b268-b40c): when the pushee's PREVIOUS-frame
     *    Y (+0x42) was OUTSIDE the height band (vertical entry this frame), a per-axis
     *    old-X/old-Z crossing test ejects by 2*radius. Unreachable on same-floor actors
     *    (old dy is inside the band whenever new dy is); needs the +0x40/42/44 prev-pos
     *    writer RE'd before it can be placed byte-true. */
    pushee->x += dx * pen / (dist + 1);
    pushee->z += dz * pen / (dist + 1);
    return 1;
}

/* The PLAYER-side pass (byte-true FUN_8002b544 from the player tick): push the player out of every
 * live enemy cylinder. Runs in game_step AFTER the player move, BEFORE the SCA wall resolver. */
void re15_body_push_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->hp < 0) return;                                   /* dead: the devour overlaps the corpse */
    int pl_locked = re15_player_is_grabbed();                 /* player +0x0 & 0x1000 (set at grab [0]
                                                               * @0x80102624, cleared at release end
                                                               * @0x8010a680) */
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->hit_radius_min == 0) continue;
        if (e->state == (uint8_t)RE15_AI_STATE_CORPSE) continue;
        /* GRABBING-PAIR EXEMPTION = an AND of BOTH freeze bits (byte-true FUN_8002aec4 @0x8002af14:
         * `and v0,a0,v1; andi 0x1000; bne -> return`): only the pair skips — a THIRD zombie still
         * pushes the grabbed player. Zombie-side bit: set at [0] @0x80102610, cleared at [8]
         * @0x80102bb8 = its sub_state_1 stays 3..6 for exactly that window. */
        if (pl_locked && e->state == 1 &&
            e->sub_state_1 >= 3 && e->sub_state_1 <= 6) continue;
        re15_body_push(e, (int32_t)e->hit_radius_min, pl, RE15_BODY_R_PLAYER);
    }
}

/* Body-contact melee-connect distance (audit wf_555f18eb Part B). re15_body_push_player above holds
 * the player at exactly hit_radius_min + RE15_BODY_R_PLAYER — the aec4/b544 standoff (both cited
 * above: aec4 reads the +0x78[6]/[0xa] box half-extents = the SAME dims as hit_radius_min; player
 * radius 0x1c2 @PSX.EXE file 0x64694). A melee gate set BELOW that standoff can therefore never
 * reach the pushed-away player, which left the big-bodied enemies (maggot 1600 / alligator 2200 /
 * adult-spider 1000) harmless in-game. Byte-true the attack lands via a FORWARD-projected attack
 * hitbox (0x8001bff8) that reaches the player at body contact, so we gate the connect on the
 * standoff itself. The +1 promotes re15_dog_arc's strict '<' to include the equilibrium dist == R. */
static inline int32_t re15_body_contact_reach(const re15_actor_t *e)
{
    return (int32_t)e->hit_radius_min + RE15_BODY_R_PLAYER + 1;
}

/* DEVOUR-FINISH animate — byte-true FUN_80102bd8 (@0x8011f890[+0x5=5/6], D3/D5 disasm 2026-07-03):
 * the state the grab's KILL counter hands off to ((+0x5)+2: 3->5 face, 4->6 behind). NOT a generic
 * walk: sub0 (@0x80102c0c..c80) sets clip +0x94 = (+0x5)+4 (9 face / 0xa behind), latches the victim
 * banks (DAT_800acbcc/d0/bfc) AND the player command DAT_800aca58 = ((+0x5)-5)<<8 | 6 = the devoured
 * COLLAPSE (whose handler stores hp=-1 at frame 0x23); sub1 plays the clip with func_0x8001ad68 root
 * motion (carries the zombie down ONTO the corpse — observed dist 822->214) + SE 3 at frame 0x28;
 * clip done -> sub2 = INERT forever (the observed hover; the decide f840[5]/[6]=0x80102bd0 is jr ra,
 * so nothing ever re-routes it). */
static void re15_enemy_ai_live_devour(re15_actor_t *e, const re15_actor_t *player)
{
    if (!player) return;
    if (e->sub_state_2 >= 2) { re15_enemy_hold_last_frame(e); return; }   /* sub2+: INERT — hold the
                                                       * final devour pose forever (no %fc re-wrap) */
    if (e->sub_state_2 == 0) {                        /* +0x6==0 entry (@0x80102c0c) */
        e->motion = (uint8_t)(e->sub_state_1 + 4);    /* (+0x5)+4: 9 face / 0xa behind */
        e->anim_frame = 0; e->anim_frac = 7;
        e->anim_blend_rate = 0x200;
        re15_audio_room_se(4);
        re15_player_victim_devour(e);                 /* player cmd 6 -> the devour collapse */
        e->sub_state_2 = 1;
    }
    if (e->anim_frame == 0x28) re15_audio_room_se(3); /* chomp SE (@ frame 0x28, D3 disasm) */
    if (re15_enemy_clip_done(e)) { e->sub_state_2 = 2; return; }   /* clip done -> INERT */
    /* NO steering in the devour (P2/D3 disasm: sub1 = ad68 root motion + SE only) — the yaw stays
     * as the grab left it; live: Leon (yaw-coupled) held rot 1547 constant through the collapse.
     * A leftover arc-steer here chased the falling corpse -> the pair swept a 3000-unit arc. */
    /* +0x5=6 WALK (byte-true FUN_80102bd8 -> func_0x8001ad68 @0x8001ad68 + the keyframe decoder
     * func_0x8001ae38 @0x8001ae38, disassembled from PSX.EXE). The forward step IS the walk clip's
     * baked root translation: clip 0xa's keyframes carry a CUMULATIVE forward offset at +6 (the
     * re15_emd_get_keyframe_speed `sx`): 821 -> 2654 -> 2237 over the 65 frames (verified from
     * EM10.EMD bank1). The original anchors at clip start (FUN_8001ac38: 0xa0 = pos - rotate(off))
     * then each frame pos = 0xa0 + rotate(off[kf], yaw). Equivalent per-frame form: pos += rotate(
     * off[kf] - off[kf-1], yaw). rot_y is steered at the player (face_player above); the engine's
     * yaw->world convention is (x += cos*d, z -= sin*d) — the SAME the player walk uses AND the frame
     * the zombie's own +1024 arc_test heading lives in (re15_ai_arc_test: rel = ang-(rot_y+1024)) —
     * so a facing zombie walks straight AT the player with feet planted to the clip cadence (no glide,
     * no fixed shamble). sz is ~0 in this clip (|sz|<=2) so the lateral cross-term is omitted. */
    /* ABSOLUTE placement from the grab's SHARED ANCHOR (P2 disasm: FUN_80102bd8 has NO ac38 — the
     * anchor set at the grab latch PERSISTS; per-frame ad68(zombie, bank1) places pos = anchor +
     * RotY(yaw)*off[kf]. The devour clip's 821->2654 offsets carry the zombie down ONTO the corpse
     * exactly as authored — while Leon's collapse clip (same anchor) drags him under it. */
    re15_enemy_bank_t *bank = re15_enemy_find(e->type);
    if (bank && bank->ok)
        re15_clip_root_motion_abs(e, &bank->skel, &bank->anim, (int)e->motion, (int)e->anim_frame);
}

/* ENGAGE animate — byte-true FUN_801021f8 (@0x8011f890[2], W1 disasm 2026-07-03): THE AWARE WALK.
 * Factored out of re15_enemy_ai_live_active case 2 (pure code motion, audit wf_827f186d zombie-girl
 * #1) because the ZOMBIE GIRL dispatches this SAME function twice: as her engage animate (girl
 * animate table @0x801202a8[2] = 0x801021f8, byte-identical to the standard @0x8011f898) AND — via
 * the 17-row girl DECIDE table's missing bounds check — as her +0x5=0x13 "decide" (@0x80120264 +
 * 0x13*4 = 0x801202b0 = the animate table's own row [2] word = 0x801021f8). See the girl block. */
static void re15_enemy_ai_live_engage_animate(int slot, re15_actor_t *e)
{
    /* Plays the per-zombie BANK0 walk clip +0x1d4 in {2,3,4,5} (seeded @0x80100774 / girl @0x8010ac58
     * from @0x8011f7e4 — the port's hurt_clip field IS +0x1d4) with the SAME foot-lock translation as
     * the 0x13 lurch, weaving toward the player via the 32-entry gait row (@0x8011f9f0 + variant*0x80). */
    /* WALK LOOPS CONTINUOUSLY: the original advances the walk clip every tick via FUN_8001f314
     * (@0x80102420), which WRAPS the frame counter at the clip's terminal 0x8000 marker (@0x8001f378)
     * unconditionally — the arms-out walk never freezes. In the port the render loop is gated on the
     * anim_flags 0x04 LOOP bit (anim_select_common.c:211); the live zombie spawns with anim_flags=0
     * so its walk render HELD-LAST = frozen slide. Set the LOOP bit here so the render cycles the clip
     * (foot-lock movement is unaffected — it always queries clip_override=motion). Cleared again by the
     * play-once entries (hurt/death/grab-devour). */
    e->anim_flags |= 0x04u;
    if (e->sub_state_2 == 0) {                       /* entry (+0x6==0 @0x80102204..) */
        /* byte-true @0x801022b8-bc `lbu v1,9(v0); andi 0x7f; sb`: every fresh engage
         * entry defensively clears the DOWNED bit — restores the standing HURT/DEATH
         * routing after any downed episode. (audit wf_827f186d zombie-live #7) */
        e->grid_id   &= (uint8_t)0x7f;
        e->motion     = e->hurt_clip;                /* +0x94 = +0x1d4 (walk clip 2..5) */
        e->anim_frame = (uint16_t)(re15_engine_rand8() & 0x1f);   /* +0x95 = rng&0x1f */
        e->anim_frac  = 0xf;                          /* +0x8f */
        e->anim_blend_rate = 0x100;                   /* walk-family f314 rate */
        s_wander_mag[slot] = (uint8_t)((re15_engine_rand8() & 0xf) + 8); /* +0x9e = 8..23 */
        if ((re15_engine_rand8() & 3) == 0)          /* 1-in-4 moan SE 4/5 */
            re15_audio_room_se((re15_engine_rand8() & 1) ? 4 : 5);
        const uint8_t *tbl = (re15_enemy_spawn_count() >= 5) ? s_zbehavior_5plus
                                                             : s_zbehavior_lt5;
        uint8_t beh = tbl[re15_engine_rand8() & 0x1f];   /* +0x1de = @0x8011faf0/fb00 */
        s_gait_variant[slot] = beh;
        s_wander_idx[slot] = (uint8_t)(re15_engine_rand8() & 0x1f);  /* +0x9f (32-row) */
        s_wander_tmr[slot] = (int16_t)gait_tmr(beh, s_wander_idx[slot]);
        s_zfoot_ok[slot] = 0;
        e->sub_state_2 = 1;
        if (beh == 2) {
            /* roll==2 PASS-THROUGH frame (C5, @0x8010234c-5c has NO early exit): the
             * SAME frame still seeds idx/tmr from the OVERLAPPED variant-2 row (the
             * behavior tables as rows -> rot >= 256 at idx 0-3 = the heading SNAPS
             * onto the steer point) and runs one steer + foot-lock with the engage
             * clip; 0x13 takes over next frame. */
            int16_t rot2 = gait_rot(2, s_wander_idx[slot]);
            int16_t sv = (int16_t)((int16_t)s_wander_mag[slot] * rot2);
            re15_enemy_steer_point(e, e->steer_x, e->steer_z, sv);
            re15_enemy_footlock_step(slot, e);
            e->sub_state_1 = 0x13; e->sub_state_2 = 0;
            return;
        }
    }
    /* per-frame walker: sVar7 = +0x9e * row[+0x9f].dir — SIGNED into the aac4 steer at
     * the STEER SNAPSHOT +0x1bc/+0x1be (decompile FUN_801021f8:57): +1 rows pull toward
     * the point, -1 rows wiggle AWAY = the authored drunken weave; the all-zero rows
     * (roll 6/12) steer NOT AT ALL (dead straight — the old "straight at him" fallback
     * was a live-homing bug). Live re-aim = the TURN state only. Segment timer ->
     * advance the row index (wrap 0x1f) + random +-sVar7 jitter; then the foot-lock
     * carries the body along the walk clip. */
    {
        uint8_t v = s_gait_variant[slot];
        int16_t rot = gait_rot(v, s_wander_idx[slot]);   /* unbounded fetch (blob) */
        int16_t sVar7 = (int16_t)((int16_t)s_wander_mag[slot] * rot);
        /* PORT OPTION (no original): with the RE2 AI flavor selected, the zombie's WALK TURN follows
         * the RE2 gate instead of this authored weave — far away it still weaves (+8/-8 by the gait
         * row's bit15), but inside 5001 it turns monotonically onto the player and inside 3000 it adds
         * a second rate-16 turn. See enemy_ai_re2_zombie.c for the byte-true disasm citations. */
        if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && re15_re2z_owns_type(e->type)) {
            /* The RE2 gait row (+0x16B) and its segment timer (+0x158) are owned by RE2's OWN gait
             * machine (@0x80101A7C-AC init, @0x80101B2C-90 tick) driving RE2's OWN PRNG — feeding it
             * from the RE1.5 wander index would be a cross-game guess, so it does not happen. */
            re15_re2z_walk_turn(e, g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                   g_actors[RE15_ACTOR_SLOT_PLAYER].z, e->ai_dist);
            /* RE2's own attack DECISION (DECISION[1] @0x80101714) runs on the same tick as the walk,
             * because 0x8010118C dispatches decision-then-executor (@0x801011A8-EC). Today the ladder
             * can only ever commit 0x0301 = the side GRAB: every other block is gated on a field with
             * no proven producer in the port, and those gates are filled with proven zeros rather
             * than guesses (see re15_re2z_fill_gates). 0x0301 maps 1:1 onto the port's own byte-true
             * grab entry — RE1.5 commits the grab with the same sub_state_1 = 3 (@enemy_ai_common
             * case 7), so the RE2 decision drives the port's existing, already byte-true grab. Any
             * other word is IGNORED on purpose: RE2's sub_states 10/12/14 mean different things in
             * RE1.5, so applying them would corrupt the state machine rather than port anything. */
            /* (the RE2 attack DECISION lives in re15_ai_decide_engage — this is the ANIMATE half) */
        } else {
            re15_enemy_steer_point(e, e->steer_x, e->steer_z, sVar7);
        }
        {   /* the timer/idx block runs for EVERY variant (the original has no v<=1
             * gate) — variant 6's zero rows expire per frame (1 rand draw/frame),
             * variant 12 latches the 30260-frame snap segment: byte-true quirks. */
            int16_t was = s_wander_tmr[slot];
            s_wander_tmr[slot] = (int16_t)(was - 1);
            if (was == 0) {
                s_wander_idx[slot] = (uint8_t)((s_wander_idx[slot] + 1) & 0x1f);
                s_wander_tmr[slot] = (int16_t)gait_tmr(v, s_wander_idx[slot]);
                int16_t flip = (int16_t)(sVar7 - (int16_t)(re15_engine_rand8() & 1) * (int16_t)(sVar7 * 2));
                e->rot_y = (int16_t)(((int32_t)e->rot_y + flip) & 0x0fff);
            }
        }
        re15_enemy_footlock_step(slot, e);
    }
}

/* FUN_80101224 (@0x8011f7b4[1], STAGE1.BIN) — the LIVE zombie ACTIVE handler. The ATTACK-WINDUP
 * half (byte-true): when the attack-arm bit (+0x1d8 & 0x100) is set and the freeze bit (+0x0 &
 * 0x1000) clear, the windup timer +0x1da counts down each frame; at == 0x12c (300) the original
 * injects action 0x16 into the 8 body-part model instances (8x FUN_80019d50(8,3,0x16,..) over the
 * LUT @0x8011f7a4 @0x80101290-0x80101504) -> the port models that single lunge as
 * re15_enemy_lunge_begin (the 0x20-frame action window, re15_damage.c); at == 0 it transitions to
 * the post-attack recovery: +0x4 word = 0x1503 (state 3 / +0x5=0x15) + the recovery action
 * +0x94 = 0xb (or 0x1f if +0x9 & 0x80) (@0x80101508-0x8010155c). Returns 1 on the frame the lunge
 * fires. The DECISION that ARMS the attack (+0x1d8 |= 0x100 + seeds +0x1da) is the unarmed-path
 * movement/decision tail (FUN_80101224 @0x80101560+, jal FUN_8001bc08). The LOS-probe half is
 * IMPLEMENTED (re15_enemy_los_probe @L2113-2121); only the ATTACK-ARM itself is DEFERRED (dormant). When not armed
 * this returns 0 (no windup). The +0x0 & 0x1000 freeze bit has no port repr (omitted). */
int re15_enemy_ai_live_active(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *e = &g_actors[slot];

    /* LOS SENSOR (byte-true FUN_80101224 @0x80101560-b0): run the amortized FUN_8001bc08 probe
     * every ACTIVE tick; result 0/1 (`(r&0xff)>>1==0`) updates +0x1d8 bit 0x10 = "player visible"
     * (clear @0x80101588-94, |= r<<4 @0x801015a4-b0); result 2 keeps the old bit. This gate is what
     * lets the search/wander decides escalate to ENGAGE (0x201) / CHARGE (0x801). */
    {
        int r = re15_enemy_los_probe(slot, e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
        if ((r >> 1) == 0)
            e->ai_flags = (uint16_t)((e->ai_flags & ~0x10u) | ((uint16_t)r << 4));
    }

    /* Windup-death gate (FUN_80101224.c Z.12): the armed path requires `(+0x1d8 & 0x100) &&
     * !(word0 & 0x1000)` — while the grab-choreo latch is set (active OR shot-aborted grab), the
     * +0x1da windup timeout is BLOCKED and the normal dispatch runs instead. (dossier D5) */
    if (!(e->ai_flags & 0x100) || e->grab_choreo) {
        /* UNARMED decision path (FUN_80101224 @0x80101560+): the original reads a FUN_8001bc08
         * sensor into +0x1d8, manages the +0x0 lifecycle, then dispatches @0x8011f80c[+0x9 & 0xf]
         * (the sub-mode table). For the COMBAT sub-mode (+0x9 & 0xf == 0 -> @0x8011f80c[0] =
         * FUN_8010168c -> @0x8011f840[entity+0x5]) the DECISION BRAIN runs. **That brain IS
         * re15_ai_dispatch_decision below — @0x8011f840 == &@0x8011f80c[13], i.e. the SAME vtable
         * (FUN_80101b64/de4/2058) ported in Phase 3, confirmed LIVE here (the earlier "System B =
         * parallel, not live" label was WRONG — it IS the live decision graph for type 0x10/0x11).**
         * It commits the attack (state word 0x701 -> +0x5=7) when the player is in range/off-arc.
         * The briefing zombies SPAWN in the non-0 sub-modes (+0x9 & 0xf = 6 feeding / 8 lying); the
         * feeding handler is the dist-gated WAKE-UP that transitions them to combat (sub 0). The FUN_8001bc08 sensor + the +0x1d8 update are IMPLEMENTED
         * (re15_enemy_los_probe @L2113-2121). DEFERRED (cited): the ATTACK-ARM (+0x1d8 |= 0x100 +
         * the +0x1da windup seed; FUN_8010ab2c) — which is DORMANT in this prototype (DAT_800aca3c & 1
         * is never set, savestate-proven), so the in-game attack is the GRAB the engage brain commits
         * (8.8), not the lunge. */
        switch (e->grid_id & 0xf) {                       /* @0x8011f80c[+0x9 & 0xf] sub-mode */
            case 0: {  /* combat sub-mode 0 -> FUN_8010168c: the DECIDE (f840) then the ANIMATE (f890) */
                re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                re15_ai_dispatch_decision(e, player);   /* f840[+0x5] decide (incl. 7 = grab-commit) */
                /* The f890 ANIMATE half, dispatched on the (possibly just-updated) +0x5 — same frame
                 * as the decide (FUN_8010168c calls decide then animate). The ported animate handlers:
                 *   +0x5 = 3/4 -> the GRAB (FUN_80102548) = the in-game attack (8.8);
                 *   +0x5 = 7   -> the TURN-to-face (FUN_80102dc8): rotate toward the player so the
                 *                 decide's grab-commit (the ±0x200 cone) can fire (8.9).
                 *   +0x5 = 2   -> the ENGAGE idle-track ANIMATE clip (+0x1d4 variant, 8.13 below).
                 * (The grab/turn/engage animate clips are ported [8.13]; the +0x5=5/6 forward walk
                 * [walk clip +0x5+4] IS IMPLEMENTED as the DEVOUR-FINISH animate re15_enemy_ai_live_devour
                 * @L2059-2095. The +0x5=0/1 search anim [search clip 0/1] is RE'd byte-true but NOT reached
                 * live by ROOM1140 m0 (engage/turn/grab only) -> deferred. The lunge-arm FUN_8010ab2c is a
                 * SEPARATE dispatch @0x80120208[+0x4=6] and is DORMANT — DAT_800aca3c&1 is never set,
                 * 8.7 — so it is not wired here; +0x5=7 is the TURN state, not the arm.) */
                if (e->sub_state_1 == 3 || e->sub_state_1 == 4)
                    re15_enemy_ai_live_grab(e, player);
                else if (e->sub_state_1 == 0x0c)
                    re15_enemy_ai_feeding_animate(e, player);   /* f890[0xc]=FUN_801048e8 */
                else if (e->sub_state_1 == 0x0d)
                    re15_enemy_ai_standup_animate(e);           /* f890[0xd]=FUN_80104a50 */
                else if (e->sub_state_1 == 7)
                    re15_enemy_ai_live_turn(e, player);
                else if (e->sub_state_1 == 0x13)
                    /* APPROACH (@0x8011f890[0x13]=FUN_801057bc): the byte-true LIVE walk gait — the
                     * locomotion bank's clip 1 + the FUN_8010939c FOOT-LOCK carries the zombie at the
                     * player. This is what LIVE zombies use (engage->0x13); +0x5=6 is the corpse-walk. */
                    re15_enemy_ai_live_approach(slot, e, player);
                else if (e->sub_state_1 == 5 || e->sub_state_1 == 6)
                    /* DEVOUR-FINISH (@0x8011f890[5/6]=FUN_80102bd8, D3/D5 disasm): NOT a generic walk —
                     * the state the grab's kill counter hands off to. sub0 latches the player's devour
                     * COLLAPSE (cmd 6); sub1 plays clip (+0x5)+4 (9 face / 0xa behind) with root motion
                     * (carries the zombie down onto the victim); sub2 = INERT forever (the hover). */
                    re15_enemy_ai_live_devour(e, player);
                else if (e->sub_state_1 == 0x0b)
                    /* PUSH-OFF stagger (@0x8011f890[0xb]=FUN_8010385c): knocked back by the grab
                     * machine's domino shove — one-shot clip 0x10 + backward slide, exit engage. */
                    re15_enemy_ai_live_pushoff(e, player);
                else if (e->sub_state_1 == 8)
                    /* CHARGE (@0x8011f890[8]=FUN_80103014): double-rate hustle at the steer target. */
                    re15_enemy_ai_live_charge(slot, e);
                else if (e->sub_state_1 == 0x12)
                    /* SLEEPING-LYING (@0x8011f890[0x12]=FUN_801054f4): dormant until the decide wakes. */
                    re15_enemy_ai_live_sleeping(e);
                else if (e->sub_state_1 == 0x11)
                    /* KNOCKDOWN/GET-UP (@0x8011f890[0x11]=FUN_8010512c): the poise-break fall. */
                    re15_enemy_ai_live_knockdown(e);
                else if (e->sub_state_1 == 0)
                    /* SEARCH-STAND (@0x8011f890[0]=FUN_80101d08): idle stand, LOS-gated escalation. */
                    re15_enemy_ai_live_search_stand(e);
                else if (e->sub_state_1 == 1)
                    /* WANDER-WALK (@0x8011f890[1]=FUN_80101ef0): shamble-roam to a random nav zone. */
                    re15_enemy_ai_live_wander(slot, e);
                else if (e->sub_state_1 == 9)
                    /* CONTACT-STAGGER (@0x8011f890[9]=FUN_801031e4): bumped from ahead. */
                    re15_enemy_ai_live_contact_stagger(e);
                else if (e->sub_state_1 == 0x0a)
                    /* EDGE-FALL (@0x8011f890[0xa]=FUN_801033c8): tumbles over the contact edge
                     * (the briefing-room pit) down one floor. */
                    re15_enemy_ai_live_edge_fall(e);
                else if (e->sub_state_1 == 2)
                    /* ENGAGE animate (byte-true FUN_801021f8 @0x8011f890[2]) — THE AWARE WALK,
                     * factored into re15_enemy_ai_live_engage_animate (pure code motion; the zombie
                     * girl shares it, audit wf_827f186d zombie-girl #1). */
                    re15_enemy_ai_live_engage_animate(slot, e);
                break;
            }
            case 5: case 6:   /* feeding (@0x8011f80c[5]/[6]=0x801018f8) -> the dist-gated wake-up */
                re15_enemy_ai_live_feeding(e);
                break;
            case 7: case 8:   /* lying (@0x8011f80c[7]/[8]=0x80101974). CITATION CORRECTED (audit
                               * wf_827f186d #8): 0x80101974 is NOT an empty `jr ra` — it is a DOUBLE
                               * dispatcher `lw @0x8011f9d8[+0x5*4]; jalr` then `lw @0x8011f9d4[+0x5*4];
                               * jalr`. Only decide row [0]=0x801039f4 is a stub; animate row
                               * [0]=0x80103a58 is the +0x6 wake machine — PORTED below (dossier
                               * analysis/zombie_hit_1140.md D2, user symptom b). Rows [5]/[6]=
                               * 0x80104b38/40 (lying get-up pair) remain OPEN — tracked in the audit;
                               * the phase-0->1 wake TRIGGER (who writes +0x6=1) is OPEN (dossier §5). */
                if (e->sub_state_1 == 0) {                /* row [0] = FUN_80103a58 (decompile-cited) */
                    switch (e->sub_state_2) {             /* +0x6 wake phase */
                    case 0:                               /* phase 0 — passive ambusher */
                        e->hit_react |= 1;                /* +0x93 |= 1 EVERY tick (@0x80103aac-ab8:
                                                           * lbu; ori 1; sb) — the passive lyer is
                                                           * UNSHOOTABLE: FUN_80011f50's guard latches
                                                           * +0x93|=2 and recurses past him. Was an
                                                           * empty break: he took damage, flinched,
                                                           * then froze forever (probe C). */
                        /* +0x1b8 = 1 (same phase-0 store) — no port field consumer (documented). */
                        break;
                    case 1: {                             /* phase 1 — wake countdown +0x9c */
                        int16_t was = e->ai_timer;        /* lh +0x9c; sh was-1 (post-dec store) */
                        e->ai_timer = (int16_t)(was - 1);
                        if (was == 0) {                   /* pre-decrement value == 0 -> rise */
                            e->sub_state_2 = 2;           /* +0x6 = 2 */
                            e->anim_frac   = 0xf;         /* +0x8f = 0xf */
                        }
                        break;
                    }
                    case 2:                               /* phase 2 — rise clip playout:
                                                           * +0x6 += f314(+0x170,+0x174,0,0x100) */
                        e->anim_blend_rate = 0x100;
                        e->sub_state_2 = (uint8_t)(e->sub_state_2 + (uint8_t)re15_enemy_clip_done(e));
                        break;
                    default:                              /* phase 3 — become a normal zombie */
                        e->grid_id = 0;                   /* +0x9 = 0 (downed bit AND nibble drop) */
                        re15_ai_set_state_word(e, 0x201); /* word +0x4 = 0x201 (subs 2/0/0) */
                        e->hit_react &= (uint8_t)~1u;     /* +0x93 &= 0xfe */
                        /* +0x1b8 = 0 — no port field consumer (documented). */
                        break;
                    }
                }
                break;
            default:  /* @0x8011f80c[1..4],[9..15]: other sub-modes — deferred (cited) */
                break;
        }
        return 0;
    }

    int fired = 0;
    e->ai_attack_timer = (int16_t)(e->ai_attack_timer - 1);   /* +0x1da -= 1 */
    if (e->ai_attack_timer == 0x12c) {             /* == 300: fire the lunge (8x action-0x16 inject) */
        re15_enemy_lunge_begin(slot);
        fired = 1;
    }
    if (e->ai_attack_timer == 0) {                 /* windup done -> post-attack recovery state */
        re15_ai_set_state_word(e, 0x1503);         /* +0x4 = state 3 / +0x5 = 0x15 */
        e->motion = (e->grid_id & 0x80) ? 0x1f : 0x0b;   /* +0x94 recovery action */
    }
    return fired;
}

/* FUN_8010ab2c (STAGE1.BIN, disasm-VERIFIED — the STAGE1/FUN_8010ab2c.c decompile IS correct,
 * unlike FUN_80100424.c) — the live zombie's attack-COMMIT setup, the ARM. It writes the live AI
 * params into the model pool (attack arc +0x5fc = 0x390 — the live family's, vs 0x2c8 for type 0x47;
 * +0x5f8=0x60/+0x5fa=0x30/+0x5fe=0x138), and — gated by the combat-active flag DAT_800aca3c & 1 —
 * ARMS the lunge windup: +0x1d8 |= 0x100 (the attack-arm bit FUN_80101224 gates the windup on) +
 * |= (rng & 1) << 9 (a left/right variant), and seeds the windup timer +0x1da = (rng & 0xff) +
 * (rng & 0xff) + 600 (600..1110; counts down to 0x12c=300 = the lunge fire). Byte-true @0x8010acbc-
 * 0x8010ad6c (3 rng draws: bit 0x200, then the two seed bytes). The 8x model-pool body-part anim
 * setup (func_80019700) + the HP roll (+0x9a = 0x32 + rng&0x1f) + the pose-setter are deferred /
 * other subsystems. The bleed-style RNG is non-reproducible-by-construction (re15_engine_rand8). */
void re15_enemy_ai_live_arm(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    e->ai_arc  = 0x390;   /* +0x5fc — the LIVE attack arc (wider than the 0x47 family's 0x2c8) */
    e->ai_p5f8 = 0x60;    /* +0x5f8 */
    e->ai_p5fa = 0x30;    /* +0x5fa */
    e->ai_p5fe = 0x138;   /* +0x5fe */
    if (!s_live_combat_active) return;                       /* DAT_800aca3c & 1 gate */
    e->ai_flags |= 0x100;                                    /* +0x1d8 |= 0x100 (attack-arm) */
    if (re15_engine_rand8() & 1) e->ai_flags |= 0x200;       /* (rng & 1) << 9 (variant) */
    {
        int seed = (int)re15_engine_rand8() + (int)re15_engine_rand8() + 600;   /* +0x1da seed */
        e->ai_attack_timer = (int16_t)seed;
    }
}

/* HURT — FUN_80105a8c (@0x8011f7b4[2], STAGE1.BIN). The live zombie's stagger reaction to a hit:
 * re15_enemy_take_damage (re15_damage.c, the FUN_80012d60 enemy branch) sets the zombie to state 2
 * (HURT) + the reaction clip (sub_state_1 = re15_react_table[type]); the tick then dispatches here.
 * The original runs a 2D nested anim FSM (dispatch table @0x8011fb90 indexed [+0x5][+0x6], @0x80105ae8-
 * 0x80105b14) that plays the stagger clip, then returns to ACTIVE when the stagger animation completes
 * (`+0x4 = 1` @0x80105b48, gated on the anim-done signal +0x1dc < 0 && grid_id & 0x80 clear). The
 * port has no zombie stagger animation, so FAITHFUL-LINE: advance HURT -> ACTIVE (the byte-true
 * transition; the stagger clip + its hit-stun DURATION = the reaction clip's frame count in the loaded
 * zombie EDD, the deferred anim layer — same stance as the wake-up's anim-skip). The grid_id & 0x80
 * special path (@0x80105ab0-0x80105ae0, sub-handlers 0x801068a0/0x80106a38) is the briefing-pose
 * stagger variant — also the deferred anim. Sub_state_2 cleared. */
/* HURT — FUN_80105a8c (@0x8011f7b4[2], STAGE1.BIN). A ROUTER: tests +0x9&0x80, then on the NORMAL
 * path dispatches the 2D stagger table @0x8011fb90[+0x5*8 + +0x6] (the live stagger handler 0x80105b7c
 * for +0x5 in {1,3,4}) and falls into the EXIT GATE @0x80105b18. The stagger handler 0x80105b7c is a
 * +0x7-phased machine: phase 0 copies the per-spawn clip +0x1d4 -> +0x94 (motion), sets +0x93|=1,
 * +0x7=1; phase 1+ plays it via anim_set (a3=0x200) and NEVER writes +0x95 (relies on the global
 * per-actor anim_frame advance, like the death port). The EXIT GATE (@0x80105b18) is NOT clip-playout
 * (unlike death): it reads the s16 hit-stun budget +0x1dc; if >= 0 it STAYS HURT (bgez @0x80105b2c);
 * else (and if +0x9&0x80 clear) it recovers: +0x4=1 ACTIVE (@0x80105b48), +0x5=0x11 (@0x80105b58),
 * +0x6=0 (@0x80105b68). +0x1dc is decremented per HURT frame by step[+0x5] from @0x8011fe30 =
 * {0,-2,-2,-3,-3,-3,-3,0,0,0,0,0} (dispatched inside the handler @0x80105d08); the canonical melee
 * stagger +0x5=react_table[atk]=3 subtracts 3/frame (seed 4..7 -> ~2 frames of stagger).
 *
 * PORT MODEL (re15_ems / death-port style): the port is a C function, not a table dispatch, so it runs
 * the canonical stagger directly — motion = hurt_clip (+0x1d4), hold HURT while hit_stun >= 0,
 * decrement by step[+0x5], recover to ACTIVE. byte-true constants cited above + the seed tables.
 * FAITHFUL-LINE / documented uncertainties (the RE workflow flagged these, no guessing):
 *  - The original time-shares +0x1dc with ai_target_x and seeds it once at INIT; the port re-seeds the
 *    DEDICATED hit_stun field per HURT entry with the byte-true value (rng&3)+4 -> a fresh stagger
 *    each hit (the time-sharing isn't replicated).
 *  - The player GUNSHOT sets +0x5=weapon_id (pistol=2); the original's stagger table row for +0x5=2 is
 *    NULL (FUN_80011f50 @0x800124bc + table @0x8011fbd0) -> the exact original pistol-hurt reaction is
 *    unresolved (no savestate witnesses HURT, C11). The port plays the verified canonical stagger for
 *    every hit; the stun step for +0x5=2 is the byte-true @0x8011fe30[2] = -2.
 *  - The +0x9&0x80 SPECIAL collapse branch (fixed clip 30/37/38) is DORMANT for ROOM1140 (set only by
 *    the grab-stun seed) -> a documented stub that just holds HURT. */
/* ⚠️ LOOP-BIT: the STANDING stagger must NOT touch anim_flags. A full store-census over the whole HURT
 * region (@0x80105a8c-0x80106060, self-disassembled 2026-07-28) finds NOT ONE store to +0x1c4 — the
 * original never changes the anim-playback flags while staggering, and its clip playback wraps
 * unconditionally (@0x8001f610-3c). The port used to clear bit 0x04 here ("HOLD-LAST the play-once
 * flinch"), which in anim_select_common.c:211 flips clip_override to -1 = play-once-HOLD-LAST — so the
 * zombie's pose FROZE on its last walk frame for the entire hit reaction (and the bit was never
 * restored on exit). That frozen pose is the "hit animation is wrong" the user reported. Savestate 4
 * corroborates: the hit zombie has motion=2 with anim_frame=18 — a clip that is still running. */
void re15_enemy_ai_live_hurt(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];

    /* DOWNED/LYING flinch (router @0x80105a9c-ad8: `lbu +0x9; andi 0x80; beq zero -> normal`, then
     * +0x5 in {0x12,0x13} -> FUN_80106a38 (prone flinch clip 37/38), else -> FUN_801068a0 (clip 0x1e)).
     * Was a bare `return` stub -> a downed/lying zombie hit non-lethally froze in state 2 FOREVER
     * (audit wf_827f186d zombie-live #2, HIGH). Both handlers raw-disasm'd end-to-end (2026-07-26)
     * and ported below as the same +0x7 phase machine the death/knockdown ports use. */
    if (e->grid_id & 0x80) {
        /* NO anim-flag clear here either (corrected 2026-07-28): a store census over BOTH downed
         * handlers — FUN_801068a0 @0x801068a0+90 and FUN_80106a38 @0x80106a38+90 — finds only
         * +0x94 (clip @0x80106900 / @0x80106a9c), +0x95 = 0 (@0x80106910 / @0x80106aac) and +0x8f
         * (@0x80106920 / @0x80106abc): NOT ONE store to +0x1c4. Same for the knockdown FUN_8010512c.
         * The port used to force HOLD-LAST here, which froze the downed flinch on its last frame —
         * the same defect that made the STANDING stagger look wrong. The phase machine does not need
         * it: re15_enemy_clip_done() compares the actor's own anim_frame against the clip length and
         * is independent of the LOOP bit. (These two handlers animate from the DEFAULT bank —
         * `lw a0,368(v0)` / `lw a1,372(v0)` = +0x170/+0x174 @0x801069b0-b4 / @0x80106b20-24 — NOT the
         * loco bank0 the standing stagger uses, which is why the render gate excludes downed hits.) */
        int prone = (e->sub_state_1 == 0x12 || e->sub_state_1 == 0x13);
        /* ZOMBIE GIRL router (FUN_8010bf80 @0x8010bfa4-c4): her downed gate routes +0x5==2 ->
         * FUN_80106a38 (prone flinch), ELSE -> FUN_801068a0 — the standard's {0x12,0x13} pair
         * selector does not apply (raw disasm: `lbu v1,5(a1); beq v1,v0(=2),0x8010bfc4`).
         * (audit wf_827f186d zombie-girl #4) */
        if (e->type == 0x13) prone = (e->sub_state_1 == 2);
        switch (e->sub_state_3) {
        case 0:
            if (prone) {                                    /* FUN_80106a38 phase 0 (@0x80106a84) */
                e->sub_state_3 = 1;                         /* +0x7=1 (delay slot @0x80106a88) */
                e->motion      = (uint8_t)(37 + (re15_engine_rand8() & 1)); /* clip 37/38 @0x80106a98-9c */
                e->anim_frame  = 0;                         /* +0x95=0 @0x80106aac */
                e->anim_frac   = 4;                         /* +0x8f=4 @0x80106abc */
                e->anim_blend_rate = 0x200;                 /* f314 a3=0x200 @0x80106b2c */
                re15_enemy_blood_at_bone(e, 0);             /* a2 = pool+0x40 = PART 0 @0x80106ad8 */
                e->hit_react |= 1;                          /* +0x93|=1 @0x80106af4-fc */
                if ((re15_engine_rand8() & 7) == 0)         /* 1/8 @0x80106b00-04 */
                    re15_audio_room_se(6);                  /* SE(6) @0x80106b0c-10 */
            } else {                                        /* FUN_801068a0 phase 0 (@0x801068f0) */
                e->sub_state_3 = 1;                         /* +0x7=1 @0x801068f0 */
                e->motion      = 0x1e;                      /* clip 30 @0x801068fc-900 */
                e->anim_frame  = 0;                         /* +0x95=0 @0x80106910 */
                e->anim_frac   = 3;                         /* +0x8f=3 @0x8010691c-20 */
                e->anim_blend_rate = 0x400;                 /* f314 a3=0x400 @0x801069bc */
                re15_enemy_blood_at_bone(e, 7);             /* a2 = pool+0x4f4 = 172*7+64 = PART 7 @0x8010693c */
                e->hit_react |= 1;                          /* +0x93|=1 @0x80106958-60 */
                re15_audio_room_se(6);                      /* SE(6) UNCONDITIONAL @0x80106954-5c */
                if (e->hit_stun == (int16_t)0x80) {         /* +0x1dc==0x80 downed sentinel @0x80106970-78 */
                    e->state       = (uint8_t)RE15_AI_STATE_ACTIVE;  /* sb 1,+0x4 @0x80106980 */
                    e->sub_state_1 = 0x11;                  /* @0x80106990 */
                    e->sub_state_2 = 4;                     /* @0x801069a0 — knockdown GET-UP phase */
                }
            }
            return;
        case 1:                                             /* playout: +0x7 += anim_set return
                                                             * (@0x80106b3c-4c / @0x801069cc-dc) */
            e->sub_state_3 = (uint8_t)(e->sub_state_3 + (uint8_t)re15_enemy_clip_done(e));
            return;
        default:                                            /* phase 2 (@0x80106b50 / @0x801069e0) */
            re15_ai_set_state_word(e, 0x1);                 /* `sw a1(=1),4(a0)`: ACTIVE, subs 0 */
            if ((e->grid_id & 0x1f) == 3)                   /* @0x80106b6c-74 / @0x801069f8-a04 */
                e->sub_state_1 = 5;                         /* the lying-wake sub-state */
            e->hit_react &= (uint8_t)~1u;                   /* +0x93&=0xfe @0x80106b8c-90 / @0x80106a1c-20 */
            return;
        }
    }

    /* CLUSTER-F CORRECTED MODEL (raw @0x80105b7c/@0x80105b18): POISE +0x1dc is decremented ONCE PER
     * HIT (the @0x8011fe30 step dispatch @0x80105d28 sits INSIDE the +0x7==0 phase-0 block), persists
     * across hits (seeded (rand&3)+4 at INIT @0x8010082c, re-armed at get-up / stagger-9 exit), the
     * stagger keeps the walk clip WITHOUT a frame reset (the recoil is a torso bend, phased on +0x7),
     * and the NORMAL exit is 0x10201 (engage, +0x6=1 = entry skipped) + an INLINE behavior
     * re-roll — only a poise BREAK (+0x1dc<0) routes to the 0x11 KNOCKDOWN.
     *
     * TORSO-BEND CADENCE (audit wf_827f186d zombie-live ADDENDUM, re-disasm'd 2026-07-26 —
     * CONFIRMED divergence, but with a CORRECTED count: the audit claimed 7 = 3+1+3; the raw
     * disasm shows phase 2 FALLS THROUGH into the phase-3 body — no branch between @0x80105dd0
     * and @0x80105dd4 — so its seed tick is already the 1st bend-up tick; the true total is
     * 6 ticks after the hit tick, not 7, and not the port's old 5):
     *   phase 0 (@0x80105be0-): +0x7=1, +0x9e=2 (@0x80105bf0), +0x9c=0 (@0x80105c00);
     *   phase 1 (@0x80105d30-): spine +0x9c -= 0x80/frame (@0x80105d7c-84; the bone apply at
     *     model+1204+100 via 0x80068098 @0x80105d80 = the deferred bone-bend layer), then the
     *     +0x9e countdown (@0x80105d94-a4: `bne a0,zero` returns while it WAS nonzero, delay
     *     slot always stores a0-1) -> 3 ticks, then +0x7=2 (@0x80105db8);
     *   phase 2 (@0x80105dbc-dd0): +0x7=3, re-seed +0x9e=2, falls through;
     *   phase 3 (@0x80105dd4-e48): spine +0x9c += 0x80/frame (@0x80105e20), same countdown
     *     (@0x80105e38-48) -> 3 ticks total bend-up; the 3rd reaches the EXIT block @0x80105e4c. */
    if (e->sub_state_3 == 0) {                         /* phase 0 — once per HIT (+0x7 reset by damage) */
        e->hit_react  |= 0x1;                          /* +0x93 |= 1 */
        e->motion      = e->hurt_clip;                 /* +0x94 = +0x1d4 — NO +0x95 reset (walk phase
                                                        * continues; the recoil is the bend, not a clip) */
        e->speed_h     = 0x14;                         /* +0x8c = 0x14 */
        e->anim_frac   = 0;                            /* +0x8f = 0 */
        /* HIT BLOOD — phase 0 spawns it, gated on the hit ZONE +0x6 (self-disassembled):
         *   80105c48: lbu v0,6(a2) ; 80105c50: bne v0,a1(=1),0x80105ca4
         *   80105c54: ori a0,zero,0x2000 ; 80105c9c: jal 0x80019700 ; 80105ca0: addiu a2,s0,64
         *      -> zone 1: spawn at the per-type gore bone (s0 = pool + 172*u8[0x8011f784+type])
         *   80105cb0-b8: lbu v0,6(v1) ; bne v0,zero,0x80105ce4      (zone 0)
         *   80105cbc: ori a0,zero,0x2000 ; 80105cd4: jal 0x80019700 ; 80105cd8: addiu a2,a2,408
         *      -> zone 0: spawn at the fixed part
         *   zone 2 (upward hit): NO spawn at all.
         * The player GUNSHOT sets +0x6 = 1 (@0x800124ac, savestate 4 confirms +0x6=1), so a shot
         * MUST bleed. The port had the spawn in re15_enemy_hurt_fx() behind `sub_state_3 != 0 ->
         * return`, called AFTER the AI step had already advanced +0x7 to 1 — structurally dead, so
         * a hit never bled. Same helper the downed flinch already uses (@0x8010693c/@0x80106ad8). */
        re15_enemy_hurt_blood(e);                      /* 0x80019700(0x2000, +0x6a, part+0x40, 0x8011fe84)
                                                        * — zone-gated + BONE-anchored (a2 = the part
                                                        * matrix, @0x80105ca0 / @0x80105cd8) */
        e->sub_state_3 = 1;                            /* +0x7 = 1 @0x80105be0 */
        e->grab_kill_ctr = 2;                          /* +0x9e = 2 @0x80105bf0 — bend-down countdown */
        e->ai_timer    = 0;                            /* +0x9c = 0 @0x80105c00 — spine-bend accumulator */
        re15_audio_room_se(6);                         /* FUN_800453d0(6) */
        {   /* poise ONCE-per-hit. Table @0x8011fe30 has 21 entries (it starts exactly where the 21-row
             * dispatch table 0x8011fb90 + 21*32 ends); [17..20] = 0x80105A2C = `addiu v0,v0,-1` = -1.
             * The old 12-entry cut silently returned 0 for ids 12..20. */
            static const int8_t stun_step[21] = { 0, -2, -2, -3, -3, -3, -3, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, -1, -1, -1, -1 };
            e->hit_stun = (int16_t)(e->hit_stun + (e->sub_state_1 < 21 ? stun_step[e->sub_state_1] : 0));
        }
        /* NO return/goto: phase 0 FALLS THROUGH into the phase-1 body in the SAME tick. Raw bytes
         * @0x80105d20: `lw v0,0(at)` / nop / `jalr v0` / nop(delay) / @0x80105d30 `lui v0,0x800b` —
         * and @0x80105d30 IS the phase-1 branch target (@0x80105ba0 `beq v1,a1,0x80105d30`). There is
         * no branch in between, so the poise step and the first bend tick share one frame: the whole
         * stagger is 6 ticks after the hit, not 7. (Self-verified 2026-07-28.) */
    }
    if (e->sub_state_3 == 1) {                         /* phase 1 — bend DOWN (@0x80105d30) */
        e->hurt_bend_bone = 7;                         /* s0 = pool+1204, stride 172 (@0x80105ba4) */
        e->hurt_bend_vz   = e->ai_timer;               /* part+100 += +0x9c BEFORE the ramp step
                                                        * (@0x80105d54-64 precedes @0x80105d7c-84) */
        e->ai_timer = (int16_t)(e->ai_timer - 0x80);   /* +0x9c -= 0x80 @0x80105d7c-84 */
        {
            int16_t was = e->grab_kill_ctr;            /* lbu +0x9e @0x80105d94 */
            e->grab_kill_ctr = (int16_t)(was - 1);     /* delay slot ALWAYS stores -1 @0x80105da4 */
            if (was != 0) goto router_gate;            /* bne a0,zero @0x80105da0 */
        }
        e->sub_state_3 = 2;                            /* +0x7 = 2 @0x80105db8 */
        goto router_gate;
    }
    if (e->sub_state_3 == 2) {                         /* phase 2 — seed + FALL THROUGH (@0x80105dbc-dd0) */
        e->sub_state_3   = 3;                          /* +0x7 = 3 @0x80105dc0 */
        e->grab_kill_ctr = 2;                          /* +0x9e = 2 @0x80105dd0 */
        /* no return: falls through into the phase-3 body (@0x80105dd0 -> @0x80105dd4) */
    }
    /* phase 3 body — bend UP (@0x80105dd4) */
    e->hurt_bend_bone = 7;                             /* s0 = pool+1204 (@0x80105ba4) */
    e->hurt_bend_vz   = e->ai_timer;                   /* part+100 += +0x9c BEFORE the ramp step
                                                        * (@0x80105df8-08 precedes @0x80105e20) */
    e->ai_timer = (int16_t)(e->ai_timer + 0x80);       /* +0x9c += 0x80 @0x80105e20 */
    {
        int16_t was = e->grab_kill_ctr;                /* lbu +0x9e @0x80105e38 */
        e->grab_kill_ctr = (int16_t)(was - 1);         /* delay slot ALWAYS stores -1 @0x80105e48 */
        if (was != 0) goto router_gate;                /* bne a0,zero @0x80105e44 */
    }
    /* phase-3 completion -> the handler's UNCONDITIONAL exit block (@0x80105e4c: 0x10201 + inline
     * re-roll). The poise check is NOT here — it is the ROUTER's gate (@0x80105b18), below, which
     * runs EVERY tick (audit wf_827f186d zombie-live addendum: the old port only knocked down after
     * the cadence; the router disasm shows the gate fires the same tick poise breaks). */
    {
        int slot2 = (int)(e - g_actors);
        /* Stagger over -> stop bending. The original's +0x9c is the SAME field the gait timer reuses
         * and the exit block overwrites it (@0x80105f8c), so no bend survives into ACTIVE. */
        e->hurt_bend_bone = -1;
        e->hurt_bend_vz   = 0;
        re15_ai_set_state_word(e, 0x10201u);           /* ACTIVE / +0x5=2 / +0x6=1 (entry SKIPPED) */
        e->hit_react &= (uint8_t)~0x1u;                /* +0x93 &= 0xfe (@0x80105f9c-fac): the hurt
                                                        * RECOVERY clears the once-per-attack hit
                                                        * latch — the next shot/slash damages again
                                                        * (needed by the weapon_fire latch/recursion) */
        s_wander_mag[slot2] = (uint8_t)((re15_engine_rand8() & 0x1f) + 8);   /* +0x9e */
        e->motion    = e->hurt_clip;                   /* +0x94 = +0x1d4 @0x80105e84-8c (exit re-arms the
                                                        * walk clip — audit wf_827f186d zombie-live addendum,
                                                        * missed exit write found in the cadence re-disasm) */
        e->anim_frac = 7;                              /* +0x8f = 7 @0x80105e98-9c (dito) */
        {
            /* byte-true: BOTH re-roll sites read the MONOTONIC spawn counter DAT_800aca4e (never
             * decremented), not the live count — hurt-recovery @0x80105ea4 `lbu 0x800aca4e; sltiu 0x5`
             * is identical to the engage re-roll @0x801022c4. The port used the DECREMENTING live count
             * here, so a 5+-spawn room fell to the docile <5 table after kills. (audit wf_27ae1ea7;
             * completes the engage-only C6 fix from 4a3220d6.) */
            const uint8_t *tbl = (re15_enemy_spawn_count() >= 5) ? s_zbehavior_5plus : s_zbehavior_lt5;
            uint8_t beh = tbl[re15_engine_rand8() & 0x1f];                    /* +0x1de */
            /* byte-true @0x80105f24-3c: beh==2 -> `sb 0x13,5(a0)` AND `sb zero,6(v0)` — +0x6=0
             * re-enters the 0x13 approach through its ENTRY block (re-seeds clip/phase/wander idx
             * = rng&0xf from the 16-entry table). The port missed the +0x6=0, so the approach ran
             * mid-entry with the rng&0x1f idx below = OOB read of s_wander_tbl[16..31].
             * (audit wf_827f186d zombie-live #3) */
            if (beh == 2) { e->sub_state_1 = 0x13; e->sub_state_2 = 0; }
            s_gait_variant[slot2] = beh;
            s_wander_idx[slot2] = (uint8_t)(re15_engine_rand8() & 0x1f);      /* +0x9f */
            s_wander_tmr[slot2] = (int16_t)gait_tmr(beh, s_wander_idx[slot2]);
        }
        e->hit_react &= (uint8_t)~1u;                  /* +0x93 &= 0xfe */
        if ((re15_engine_rand8() & 0xf) == 0)          /* rare: break into the CHARGE */
            re15_ai_set_state_word(e, 0x801);
        if (re15_ai_facing_aligned(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]))    /* a780 -> TURN */
            re15_ai_set_state_word(e, 0x701);
        /* falls into the router gate like the original (handler returns to @0x80105b18);
         * hit_stun >= 0 is guaranteed on this path, so the gate is a no-op here. */
    }
router_gate:
    /* FORWARD LURCH (@0x8010602c `jal 0x800245d8` with a0=0 — the stagger handler's common TAIL, i.e.
     * it runs on EVERY phase's exit): FUN_800245d8 reads the per-tick speed +0x8c (@0x800245f0
     * `lhu v0,140(v0)`) and writes the advanced world X/Z back (@0x800246e4 `sw v0,52(a0)` /
     * @0x800246f8 `sw v0,60(a0)`). Phase 0 seeded +0x8c = 0x14 = 20 u/tick (@0x80105c10), and the
     * savestate confirms it: ONLY the hit zombie carries +0x8c = 20 (the untouched ones have 0/210).
     * The port set speed_h but never integrated it, so the zombie took the hit standing perfectly
     * still. Same heading convention as the charge slide @L1500-1501. */
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->speed_h) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->speed_h) >> 12);

    /* ROUTER exit gate (@0x80105b18-68), reached EVERY tick after the jalr'd handler (fallthrough
     * from @0x80105b10): `lh +0x1dc; bgez -> return` (@0x80105b24-2c); poise BROKEN (< 0) and
     * +0x9&0x80 clear (@0x80105b34-40; always clear on this path — the downed branch bypassed the
     * gate via `j 0x80105b6c` @0x80105ad0/ae0) -> KNOCKDOWN: +0x4=1 (@0x80105b48), +0x5=0x11
     * (@0x80105b58), +0x6=0 (@0x80105b68) — the SAME tick the poise step broke it, i.e. already on
     * the phase-0 hit tick, NOT after the bend cadence. (audit wf_827f186d zombie-live addendum) */
    if (e->hit_stun < 0) {
        e->state       = (uint8_t)RE15_AI_STATE_ACTIVE;
        e->sub_state_1 = 0x11;
        e->sub_state_2 = 0;
        e->hurt_bend_bone = -1;   /* poise broke mid-stagger -> knockdown, drop the bend */
        e->hurt_bend_vz   = 0;
    }
}

/* DEATH — FUN_80106ba4 (@0x8011f7b4[3], STAGE1.BIN) -> FUN_80107cb0. The live zombie's death: a hit
 * that drives the zombie's HP < 0 sets state 3 (DEATH, re15_enemy_take_damage / the player shot);
 * the tick dispatches here each frame. FUN_80107cb0 is a 3-phase sub-FSM on +0x7 (sub_state_3):
 *   phase 0 (@0x80107d00): +0x93|=1, +0x94 (motion) = 0x1f (the DEATH clip 31), +0x95 (anim_frame) = 0,
 *                          +0x7 = 1  (start the death animation)
 *   phase 1 (@0x80107dcc): play clip 0x1f via anim_set (@0x80107e84); the death SE fires at frame 7
 *                          (0x800453d0, rng&1?8:5) + gore at frame 35 (FUN_80019700); when anim_set
 *                          hits the clip's terminal frame it advances +0x7 -> 2
 *   phase 2 (@0x80107eac): +0x0 |= 2 (lifecycle), +0x4 = 7 (@0x80107ec8) = CORPSE — OUT of the
 *                          @0x8011f7b4[0..4] dispatch range, so the tick stops dispatching it (an
 *                          inert corpse that can no longer engage/grab; motion stays 0x1f -> the
 *                          render holds the clip's last frame = the fallen pose).
 * Ported byte-true onto the port's shared anim playback (player_common.c advances every actor's
 * anim_frame + holds the last keyframe): set motion 0x1f, then hold state DEATH until the death clip
 * has played out (anim_frame reaches its last frame, read from the loaded model bank), then -> CORPSE.
 * IMPLEMENTED: the death SE (frame 7) + the gore spawn (frame 35) fire in re15_enemy_ai_live_death phase 1 (@L2487-2490).
 * Headless / model-not-loaded fallback: with no bank the clip length is unknown -> go straight to
 * CORPSE (the prior behaviour, no regression). */
/* FSM-CLOCK clip-end signal — the byte-true FUN_8001f3bc/FUN_8001f8b4 RETURN contract, the piece
 * every live-zombie ANIMATE sub-FSM gates its +0x6/+0x5 sub-step on: a sub-step advances ONLY when
 * its clip finishes a cycle (`+0x6 += func_0x8001f314(...)`). The port's original divergence was
 * advancing sub-steps every frame (immediate) instead of on clip-end, which snapped zombies through
 * their stand-up/grab/turn animations. The shared per-frame anim pass (re15_actors_anim_advance) is
 * the SOLE anim_frame incrementer; this only READS it (no double-increment). Returns 1 when the
 * current clip has reached its last frame, 0 while still playing. GUARD: returns 1 (done) when the
 * bank/clip is unloaded or empty so the sub-state FSM never stalls (avoids a new hang) — never the
 * FUN_8001f8b4 unbounded &0x8000 keyframe scan. Factored out of the death handler (was inline). */
/* ---- THE SINGLE BANK RULE -------------------------------------------------------------------
 * A STAGE1 zombie poses from TWO different animation banks depending on its state, and the two
 * consumers of that fact — the RENDERER (which picks the bank to pose) and the FRAME CLOCK (which
 * compares anim_frame against "the clip length") — MUST agree. They used to encode the rule twice:
 * the renderer inline in main.c, the clock implicitly by always taking the ACTION bank. For every
 * loco-bank state that meant the state machine compared the frame index against a DIFFERENT clip's
 * length. This is now the one definition; main.c calls it too, so they cannot drift apart.
 *
 * The rule itself is unchanged and stays as the renderer had it (W1 disasm 2026-07-03 + the
 * 2026-07-28 stagger correction): the walking ENGAGE (+0x5=2), the 0x13 lurch, the TURN (+0x5=7)
 * and the STANDING stagger (state 2 with +0x9 & 0x80 clear) pose the LOCOMOTION bank (entity+0x84,
 * `lw a0,132(v0)` @0x80105d3c / @0x80105de0); every other state poses the 43-clip action bank
 * (+0x170/+0x174). Downed/lying hits keep the action bank — those handlers set their own clips. */
int re15_actor_uses_loco_bank(const re15_actor_t *a)
{
    if (!a) return 0;
    return (a->state == 1 && (a->sub_state_1 == 0x13 || a->sub_state_1 == 2 || a->sub_state_1 == 7))
        || (a->state == 2 && !(a->grid_id & 0x80));
}

/* The frame count of the clip this actor is ACTUALLY being posed from — same bank the renderer
 * picks, same availability fallback. 0 = unknown (no bank / clip out of range). */
/* The PRE-FIX clock: always the action bank. Kept so the pose-stream trace can log both clocks
 * side by side and the checker can prove whether the bank rule actually mattered. */
int re15_actor_clip_len_legacy(const re15_actor_t *a)
{
    if (!a) return 0;
    re15_enemy_bank_t *b = re15_enemy_find(a->type);
    if (!b || (int)a->motion >= b->anim.clip_count) return 0;
    return b->anim.clips[a->motion].frame_count;
}

int re15_actor_clip_len(const re15_actor_t *a)
{
    if (!a) return 0;
    re15_enemy_bank_t *b = re15_enemy_find(a->type);
    if (!b) return 0;
    if (re15_actor_uses_loco_bank(a) && b->loco_ok && (int)a->motion < b->anim_loco.clip_count)
        return b->anim_loco.clips[a->motion].frame_count;
    if ((int)a->motion < b->anim.clip_count)
        return b->anim.clips[a->motion].frame_count;
    return 0;
}

static int re15_enemy_clip_done(const re15_actor_t *e)
{
    /* RE15_CLIPDONE_LEGACY=1 restores the pre-fix behaviour (always the ACTION bank) so the two
     * can be A/B'd in one build — the pose-stream checker then shows the bank-consistency
     * violations appear/disappear without touching the code. */
    int frames;
    if (getenv("RE15_CLIPDONE_LEGACY")) {
        re15_enemy_bank_t *bk = re15_enemy_find(e->type);
        if (!bk || (int)e->motion >= bk->anim.clip_count) return 1;
        frames = bk->anim.clips[e->motion].frame_count;
    } else {
        frames = re15_actor_clip_len(e);   /* the bank the RENDERER poses from, not always the
                                            * action bank — see re15_actor_uses_loco_bank */
    }
    if (frames <= 0) return 1;
    return (e->anim_frame >= frames - 1) ? 1 : 0;
}

void re15_enemy_ai_live_death(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];

    if (e->sub_state_3 == 0) {                         /* phase 0 — start the death anim */
        e->hit_react  |= 0x1;                          /* +0x93 |= 1 (@0x80107d08) */
        if (e->grid_id & 0x80) {                       /* DOWNED death (FUN_80107cb0): clip 0x1f */
            e->motion = 0x1f; e->anim_frame = 0;
        } else {                                       /* STANDING death FUN_80106c18 (cluster F):
                                                        * clip = ((s8)+0x93>>7)*2 + 0xd -> 0xb when the
                                                        * shot's front/back latch +0x93 bit0x80 is set
                                                        * (fall BACKWARD), else 0xd; random start frame
                                                        * rand&3; SE 5. */
            e->motion = (uint8_t)((e->hit_react & 0x80) ? 0x0b : 0x0d);
            e->anim_frame = (uint16_t)(re15_engine_rand8() & 3);
            re15_audio_room_se(5);
        }
        e->anim_frac = 0xf;                            /* +0x8f = 0xf pose-crossfade seed (STANDING
                                                        * @0x80106cd8 / DOWNED @0x80107d4c, both `ori
                                                        * v0,0xf; sb v0,143`) = the 15-frame blend into
                                                        * the death-fall clip (audit #8). */
        e->anim_flags &= (uint16_t)~0x04u;             /* CLEAR LOOP: the death clip 0x1f plays ONCE + holds
                                                        * its fallen last frame (render clip_override=-1), it
                                                        * must NOT loop like the feeding/idle clip did (0x04 was
                                                        * set at spawn). Complements the state-7 corpse freeze. */
        e->sub_state_3 = 1;                            /* +0x7 = 1 (@0x80107d1c) */
        re15_enemy_death_fx(e);                        /* burst at the per-type GORE BONE (@0x80106d2c
                                                        * standing / @0x80107cf4 downed) */
        if (!(e->grid_id & 0x80)) {
            /* STANDING death FUN_80106c18 fires TWO more bone-anchored bursts, gated on the hit zone
             * +0x6 (s1 = 1 @0x80106c34):
             *   @0x80106d48 bne +0x6,s1 -> @0x80106d5c/60 jal ; addiu a2,v0,64   = pool+64  = PART 0
             *   @0x80106d78 bne +0x6,zero -> @0x80106d8c/90 jal ; addiu a2,a2,408 = pool+408 = PART 2
             * (The downed death has no such pair.) */
            if (e->sub_state_2 == 1)      re15_enemy_blood_at_bone(e, 0);
            else if (e->sub_state_2 == 0) re15_enemy_blood_at_bone(e, 2);
        }
        return;
    }
    if (e->sub_state_3 == 1) {                         /* phase 1 — play the death clip to its end */
        if (e->grid_id & 0x80) {                       /* DOWNED death ONLY (FUN_80107cb0): the frame-7 SE
                                                        * (@0x80107de8) + frame-35 gore (@0x80107e18) gates
                                                        * live ONLY in the downed handler. The STANDING death
                                                        * (FUN_80106c18) fired its single SE(5) + all gore
                                                        * up-front in phase 0 and has NO per-frame SE/gore in
                                                        * phase 1 — ungated, every shot zombie double-groaned
                                                        * + double-sprayed blood (audit #4/#5). */
            if (e->anim_frame == 7)                    /* +0x95 == 7 (@0x80107de8): death groan SE (rng 5/8) */
                re15_audio_room_se((re15_engine_rand8() & 1) ? 5 : 8);
            if (e->anim_frame == 35)                   /* +0x95 == 0x23 (@0x80107e18): frame-35 gore burst */
                re15_enemy_death_fx(e);
        }
        if (!re15_enemy_clip_done(e)) return;          /* still playing the death clip (FSM-clock gate) */
        e->sub_state_3 = 2;
    }
    /* phase 2 — settle to corpse, EXCEPT the STANDING 1/8 secondary death-throe (FUN_80106c18
     * @0x80106e50: `jal rng; andi v0,0x7; bne v0,zero -> corpse`; on (rng&7)==0 it writes +0x6=4 /
     * +0x7=0 @0x80106e60-70 and JUMPS past the corpse write -> re-enters phase 0 -> replays the fall
     * clip = a visible extra death twitch). The downed handler has no such branch. (audit #9) */
    if (!(e->grid_id & 0x80) && (re15_engine_rand8() & 7) == 0) {
        e->sub_state_2 = 4;                            /* +0x6 = 4 (@0x80106e60) */
        e->sub_state_3 = 0;                            /* +0x7 = 0 -> phase-0 re-entry next tick */
        return;
    }
    e->state = (uint8_t)RE15_AI_STATE_CORPSE;          /* +0x4 = 7 (@0x80107ec8 / @0x80106e98): the
                                                        * halfword write zeroes +0x5 -> the settle
                                                        * FSM starts at INIT */
    e->sub_state_1 = 0; e->sub_state_2 = 0;
}

/* FUN_80100424 (@0x80072bac[0x10/0x11/0x16], STAGE1.BIN) — the LIVE zombie PER-FRAME TICK, the
 * entry the per-frame loop FUN_8001a50c dispatches each frame. Byte-true core (the live analog of
 * re15_enemy_ai_tick / FUN_8011d6d4): the pause gate (g_pauseflags & 0x20000000) + the per-entity
 * skip gate (+0x9 & 0x20), cache the player distance @+0x1d0 (same SquareRoot0(16-bit ΔX²+ΔZ²)),
 * then dispatch the main state @0x8011f7b4[entity+0x4] (INIT/ACTIVE/HURT[2]/DEATH[3] ported @L2532-2533, CORPSE[7] @L2534; only [4] idle deferred).
 * The post-dispatch attack-point (FUN_80104178 -> the port atk_pt skeleton map, re15_damage.c) +
 * the collision/render helpers (FUN_8002b498/aec4/b544/FUN_8003b0a4) are other subsystems, not run
 * here. Returns 1 if dispatched, 0 if a gate skipped it. */
int re15_enemy_ai_live_tick(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return 0;   /* slot 0 = player, never an AI */
    re15_actor_t *e = &g_actors[slot];
    if (!e->active) return 0;
    if (s_ai_paused) return 0;                          /* g_pauseflags & 0x20000000 */
    if (e->grid_id & RE15_AI_GRID_SKIP) return 0;       /* +0x9 & 0x20 */

    e->ai_dist = (uint32_t)re15_enemy_player_dist(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
    /* +0x1bc/+0x1be STEER TARGET — the per-tick writer is EXE FUN_80039e7c (RESOLVED 2026-07-04;
     * the RAM observation "== player pos every tick" was the SAME-ZONE case). Byte-true call
     * (zombie driver @0x8010a9c0-9e0 / m0 root @0x80100538): a0 = the player-pos block, a1 = the
     * waypoint node +0x1d6, a2 = +0x1d8 & 8 (one-shot roam request, cleared right after). Same
     * zone -> steer = raw player pos; CROSS-ZONE -> pathfind first-hop crossing (ROOM1140: the
     * 5-zone ring around the conference table — the zombies path AROUND it, RAM-verified). */
    re15_nav_update_steer(e, (int16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                             (int16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                          e->ai_wp_node, (int)(e->ai_flags & 8u));
    e->ai_flags &= (uint16_t)~8u;                        /* the @0x8010a9f8 one-shot clear */

    int32_t wall_ox = e->x, wall_oz = e->z;              /* pre-dispatch pos for the wall clamp */
    switch (e->state) {                                  /* @0x8011f7b4[entity+0x4] */
        case RE15_AI_STATE_INIT:   re15_enemy_ai_live_init(slot);   break;  /* [0] FUN_80100688 */
        case RE15_AI_STATE_ACTIVE: re15_enemy_ai_live_active(slot); break;  /* [1] FUN_80101224 */
        case RE15_AI_STATE_HURT:   re15_enemy_ai_live_hurt(slot);   break;  /* [2] FUN_80105a8c */
        case RE15_AI_STATE_DEATH:  re15_enemy_ai_live_death(slot);  break;  /* [3] FUN_80106ba4 */
        case RE15_AI_STATE_CORPSE:                       /* [7] = FUN_80109554 corpse settle */
            re15_enemy_corpse_settle(e);
            break;
        default: /* [4]=0x8010919c idle (deferred) */ break;
    }
    (void)wall_ox; (void)wall_oz;   /* the SCA wall clamp moved to run_all (byte-true b0a4 order:
                                      * AFTER the body pushes @0x8010062c) */
    return 1;
}

/* The per-enemy per-frame STEP for the LIVE family — the entry game_step will call for each active
 * 0x10/0x11/0x16 zombie: the FUN_80100424 tick (decision) + the lunge slice (re15_enemy_lunge_tick
 * fires the SHARED hitbox while a lunge window is open). The live analog of re15_enemy_ai_step.
 * NOT yet wired into game_step (no 1170 risk). */
int re15_enemy_ai_live_step(int slot)
{
    int r = re15_enemy_ai_live_tick(slot);   /* FUN_80100424 — gate, dist, @0x8011f7b4[+0x4] */
    re15_enemy_lunge_tick(slot);             /* shared lunge slice — hitbox per active frame */
    return r;
}

/* ============================================================================
 * CROW (type 0x21) — 3D flight AI. Byte-true port of the FUN_80112020 family
 * (root state table @0x8012111c; full spec + disasm citations in RE15_CROW_AI.md).
 * WAVE 1 = INIT + ACTIVE cruise (the altitude-corridor circling flight). Dive /
 * hurt / death are later waves. The crow shares the entity struct + shared infra
 * (steer / dist / trig) but flies in 3D — y (+0x38) IS the altitude (smaller y =
 * higher) and it is NOT ground-clamped, so run_all gives it its OWN branch that
 * skips the zombie body-push and SCA wall-clamp entirely.
 * ==========================================================================*/
/* ---- shared flock-coordination word (the port of 0x800aca50) --------------------------
 * The crows write high bits (0x2000/0x4000/0x8000) into this word as they fly; the flock
 * dispatcher reads them to force flock-mates into the coordinated maneuvers 14/15/17. Low
 * 12 bits carry the pending-yaw + count<<4 (INIT). Reset on room change. */
static uint16_t s_crow_flock;
/* the second crow global: the flag-array word @0x800b1028 (bit-array, MSB-first: bit N ->
 * mask 0x80000000>>(N&0x1f)). FLIGHT-2 uses bits 0x1c(mask 0x8, set by HOVER-grab), 0x1d
 * (mask 0x4, tested by the dispatcher -> spin), 0x1e (mask 0x2, tested by dive-arm); 0x1f
 * (mask 0x1) = the death gate, set only in STAGE3/STAGE5. In STAGE1 only 0x1c is ever set. */
static uint32_t s_crow_gflags;
/* DAT_800aca52 — player-reaction latch word (bit 0 = player knocked down). Setters = the EXE
 * knockdown handlers @0x800334e8-0x80033504 / @0x800345c8 `lhu aca52; ori v0,v0,1; sh` (the player
 * knockdown command FSM — NOT PORTED YET, so bit 0 currently has no producer: OPEN). Cleared by the
 * player normal handler (@0x80031c44 `DAT_800aca52 &= 0xfffe`, wired in game_step_common.c) and on
 * room init (FUN_8003ecec `DAT_800aca52 = 0`, wired below). (audit wf_827f186d crow #A) */
uint16_t g_aca52_flags;
void re15_crow_flock_reset(void) { s_crow_flock = 0; s_crow_gflags = 0;
                                   g_aca52_flags = 0; /* FUN_8003ecec: DAT_800aca52 = 0 on room init */ }
/* the scripted "crows die" event trigger (STAGE3/STAGE5 set bit 0x1f of 0x800b1028 via
 * FUN_80118d00/80119514): the root then promotes every state-4 grid&0x40 crow to DEATH. */
void re15_crow_death_event(void) { s_crow_gflags |= 0x1u; }

/* Height oracle — byte-true FUN_80115dc8: returns +1 (descend, y grows) / -1 (climb) to
 * hold the crow in the corridor 1800..5400 units ABOVE its perch height. In-band the
 * +0x1d4 bit 0x80 chooses climb-vs-descend (hysteresis). */
static int re15_crow_height_dir(const re15_actor_t *e)
{
    int32_t target = e->crow_perch_h;
    int32_t cur    = e->y;
    if (cur < target - 5400) return  1;      /* @0x80115ddc: above ceiling -> descend */
    if (cur > target - 1800) return -1;      /* @0x80115dec: below band     -> climb   */
    return (e->crow_mode & 0x80) ? -1 : 1;   /* @0x80115e04: in-band hysteresis         */
}

/* setters — 0x80115d94 (clip) / 0x80115d74 (sub-state, zeroes the nested step +0x6) */
static void re15_crow_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static void re15_crow_sub (re15_actor_t *e, uint8_t s) { e->sub_state_1 = s; e->sub_state_2 = 0; }

/* horizontal advance along yaw at crow_speed (pos_advance FUN_800245d8, a0=0) */
static void re15_crow_advance(re15_actor_t *e)
{
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * e->crow_speed) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * e->crow_speed) >> 12);
}

/* EM021 (crow) clip frame-counts — byte-true, embedded verbatim from the model's EDD
 * (CDEMD0.EMS idx 8, re15_emd_parse_animation). Embedded (not read from the lazily
 * render-loaded bank) so the flight brain has the real lengths every tick even while the
 * crow is off-camera / its bank not yet uploaded — same rationale as the gait-blob embed. */
static const uint8_t s_crow_clip_len[14] =
    { 23, 35, 21, 16, 10, 24, 33, 40, 8, 30, 40, 36, 15, 16 };

/* clip-advance — byte-true 0x8001f314: POST-increment +0x95 (anim_frame), decay the +0x8f
 * blend, and WRAP at the real EM021 clip length (FUN_8001f8b4), returning the wrap flag =
 * the "clip done" the move handlers branch on. anim_frame==8 is the wing-flap re-thrust key. */
static int re15_crow_anim(re15_actor_t *e)
{
    int fc = (e->motion < 14) ? s_crow_clip_len[e->motion] : 1;
    if (fc <= 0) fc = 1;
    e->anim_frame++;                              /* +0x95++ (POST-inc) */
    if (e->anim_frac > 0) e->anim_frac--;         /* +0x8f blend decay  */
    if ((int)e->anim_frame >= fc) { e->anim_frame = 0; return 1; }   /* wrap -> clip done */
    return 0;
}

/* proximity yaw-weave — byte-true 0x80115e24: latch a bank dir in +0x1d7 when dist<2000,
 * roll +0x6a by ±80/tick; clear the latch when dist>=2000. */
static void re15_crow_weave(re15_actor_t *e)
{
    if (e->crow_dist < 2000) {                                 /* @0x80115e38 sltiu 0x7d0 */
        if (e->crow_bank == 0) e->crow_bank = (e->crow_mode & 1) ? 2 : 1;  /* @0x80115e5c */
    } else {
        e->crow_bank = (uint8_t)(e->crow_bank & 0xfc);         /* @0x80115e9c */
    }
    if      (e->crow_bank == 1) e->rot_y = (int16_t)(e->rot_y + 80);   /* @0x80115ee4 */
    else if (e->crow_bank == 2) e->rot_y = (int16_t)(e->rot_y - 80);   /* @0x80115ef0 */
}

/* ATTACK-SITE wound stamper — byte-true 0x801161e8. KORRIGIERT (crow_victim_anim.md F3/F8,
 * beide CONFIRMED): das ist KEIN Screech — die sechs Calls in 0x8011620c-70 sind
 * `jal 0x80037edc` = der Per-Panel-WUND-Akkumulator (Panel in a0, Betrag in a1 =
 * 0xa/0x28/0x8); die Routine enthaelt keinen einzigen SE-Call. Die alte Port-Fassung
 * (re15_crow_screech) spielte die Panel-Nummern als Room-SEs ab. Staffelung auf dem
 * vert-Fehler der Kraehe (+0x1ec, lh @0x801161f4):
 *   <1500      -> (1,+0xa) @0x8011620c, (2,+0xa) @0x80116214-1c/70
 *   1500..2999 -> (3,+0xa) @0x8011622c, (4,+0x28) @0x80116238,
 *                 (6,+0x28) @0x80116244, (5,+0x28) @0x80116250-70
 *   3000..3599 -> (0,+0x8) @0x80116260, (7,+0x28) @0x80116268-70
 *   >=3600     -> nichts (beq @0x80116258 -> Epilog)
 * Caller (jal-Zensus STAGE1.BIN): Dive-Connect @0x80113b7c, Grab move[13] step0
 * @0x80113ef4, Strike move[16] @0x801144bc. */
static void re15_crow_attack_wounds(const re15_actor_t *e)
{
    int16_t ve = e->crow_vert_err;
    if      (ve < 1500) { re15_wound_add(1, 0x0a); re15_wound_add(2, 0x0a); }
    else if (ve < 3000) { re15_wound_add(3, 0x0a); re15_wound_add(4, 0x28);
                          re15_wound_add(6, 0x28); re15_wound_add(5, 0x28); }
    else if (ve < 3600) { re15_wound_add(0, 0x08); re15_wound_add(7, 0x28); }
    /* >= 3600: nothing (@0x80116258) */
}

/* room_coll FUN_8001c6e8 for the crow root (a0=&+0x34, a1=dim[3]=200, a2=8, a3=0x400) — the
 * floor-Y reference under the crow (+0x1ba), refreshed EVERY root tick @0x80112158-84.
 * Byte-true semantics (FUN_8001c6e8 decompile, RE_15_Quellcode_V2): scan SCA bands 7..0
 * (a2=8, `uVar8 -= 0x1000` loop) for a cell CONTAINING (x,z) shrunk by r=200 per side
 * (`x - (cell.x+r) < w - 2r` unsigned) whose attr matches `uVar8 == hw5 & 0xf002` (=> band ==
 * floor>>4 AND u1 bit1 CLEAR) and `hw4 & 0x400` (=> u0 bit2 SET); on match return
 * -(band+1)*1800 (`((uVar8>>12)+1) * -0x708`), else 0. GROUND-TRUTH CHECK (ROOM10C0.RDT):
 * the SCA is all wall blockers (u0=0xff, bands 0..2) and NO cell contains the 3 crow spawns
 * (em_set Y = 0) -> room_coll = 0 = spawn plane = perch over the open helipad, -(b+1)*1800
 * over the wall strips — exactly the "don't land on walls" gate steer[7]/[8] implements.
 * OPEN (sub-branch): the entity-overlap arm (`param_4 & 0x10000`==0 -> scan the actor pool
 * @0x800b3f38 stride 0x94 via FUN_8002da4c, returning the standing entity's top Y
 * `+0x1ba? = ent.y + height*-2`) is not ported — it only fires when the crow XZ-overlaps
 * another actor's box. (audit wf_827f186d crow #5) */
static int16_t re15_crow_room_coll(const re15_actor_t *e)
{
    if (!g_room_rdt_ok) return 0;                       /* no cells -> 0 (byte-true no-match) */
    for (int b = 7; b >= 0; b--) {
        for (int i = 0; i < g_room_rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[i];
            if ((c->floor >> 4) != b) continue;         /* hw5 & 0xf000 == band<<12 */
            if (c->u1 & 0x02) continue;                 /* hw5 bit 1 must be clear (mask 0xf002) */
            if (!(c->u0 & 0x04)) continue;              /* hw4 & 0x400 (a3 mask) */
            if ((uint32_t)(e->x - ((int32_t)c->x + 200)) < (uint32_t)((int32_t)c->width   - 400) &&
                (uint32_t)(e->z - ((int32_t)c->z + 200)) < (uint32_t)((int32_t)c->density - 400))
                return (int16_t)(-(b + 1) * 1800);      /* ((band)+1) * -0x708 */
        }
    }
    return 0;                                           /* `if (uVar8 == 0) return 0` */
}

/* FUN_80115f00 — the crow's PERCH-RETURN oracle (the way home after attacking), raw disasm
 * @0x80115f10-58: `y < perch-1800 && +0x1ba == +0x1ea` -> sub 7 (descend-to-perch); else
 * `+0x1d6 < 3` -> sub 5 (climb-out); else nothing. (audit wf_827f186d crow #4) */
static void re15_crow_perch_return(re15_actor_t *e)
{
    if (e->y < e->crow_perch_h - 1800 &&                /* @0x80115f18 addiu -1800; slt */
        e->crow_floor == e->crow_perch_h) {             /* @0x80115f28-30 lh 442 == a1(+0x1ea) */
        re15_crow_sub(e, 7);                            /* @0x80115f58 a0=7 */
        return;
    }
    if (e->crow_atk_ctr < 3) re15_crow_sub(e, 5);       /* @0x80115f44-58 sltiu 3 -> a0=5 */
}

/* the byte-true flat damage the crow deals: dive/strike -4, grab -8 (the player+0x93 hit
 * gate keeps it once-per-contact; on a lethal hit broadcast the KILL bit 0x2000 to the flock).
 * PLAYER COMMANDS (audit wf_827f186d crow #2): each original hit also issues a player reaction
 * command via 0x800aca58 — dive hit `sb 2` @0x80113b00 (+ dir aca59=facing+2 @0x80113b28,
 * aca5a=0 @0x80113b30), grab `sb 5` @0x80113e48, lethal `sb 3` @0x80113b48 — dispatched through
 * @0x80073f90[cmd] ([2]=0x80035af0 hit, [5]=0x80036834 grab, [3]=0x800366bc death). The port's
 * player reaction = the hp-drop flinch detector + grab pin in game_step_common.c (cmd FSM
 * itself = OPEN). The CRITICAL re-arm — the cmd-0 NORMAL handler prologue clearing +0x93
 * every frame (@0x80031964 `sb zero,DAT_800acae7`, UNCONDITIONAL in LAB_800318f8) — is now
 * wired in the game_step normal branch; without it one crow hit latched the player immune
 * to ALL further damage forever. */
static void re15_crow_hit_player(re15_actor_t *e, re15_actor_t *player, int dmg)
{
    if (player->hit_react != 0) return;              /* +0x93 gate — DIVE-ONLY: einzig der Dive-Hit
                                                      * prueft +0x93==0 (@0x80113ac0-cc) und steht
                                                      * damit schon in der Commit-Bedingung */
    player->hp = (int16_t)(player->hp - dmg);        /* @0x80113b04 (-4 dive) */
    player->hit_react |= 1;
    if (player->hp < 0) {                            /* lethal -> KILL broadcast            */
        s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x2000);   /* @0x80113b58 */
        e->crow_hs = 1;                              /* +0x1d8 self-exempt                  */
    }
}

/* UNGATED damage — Grapple (-8 @0x80113e34-3c) und Strike (-4 @0x801144f0-fc): der komplette
 * Original-Kontaktpfad enthaelt KEINE Instruktion auf +0x93 (weder Read noch Write; Disasm
 * 0x80113dd4-e58 / 0x801144f0-fc, Dossier crow_1170.md D4 CONFIRMED) — Folge-Strikes STAPELN im
 * Original, waehrend der Port sie ueber den hit_react-Gate verschluckte. Der hit_react-Latch
 * bleibt hier als PORT-PROXY fuer die (unportierte) Player-Cmd-FSM-Knockdown-Seite (Flinch-
 * Detektor in game_step_common.c keyt darauf) — er unterdrueckt aber keinen Schaden mehr. */
static void re15_crow_hit_player_ungated(re15_actor_t *e, re15_actor_t *player, int dmg)
{
    player->hp = (int16_t)(player->hp - dmg);
    player->hit_react |= 1;                          /* PORT-PROXY (Original verwaltet +0x93 in den
                                                      * Player-Cmd-Handlern, nicht hier) */
    if (player->hp < 0) {                            /* lethal -> KILL broadcast            */
        s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x2000);   /* @0x80113b58 */
        e->crow_hs = 1;
    }
}

/* FLOCK DISPATCHER 0x80116068 — reads s_crow_flock, forces flock-mates into 15/14/17. */
static void re15_crow_flock_dispatch(re15_actor_t *e)
{
    uint16_t fw = s_crow_flock;
    if (fw & 0x8000) {                               /* @0x80116080 */
        if (e->grid_id < 0x80) {                     /* @0x801160a0 not downed */
            if (e->crow_hs != 0) s_crow_flock = (uint16_t)(fw & 0xfff);   /* initiator clears */
            else if (e->sub_state_1 >= 5) re15_crow_sub(e, 15);          /* @0x801160d8 */
            e->crow_hs = 0;
            fw = s_crow_flock;
        }
    }
    if (fw & 0x4000) {                               /* @0x80116100 */
        if (e->crow_hs != 0) s_crow_flock = (uint16_t)(fw & 0xfff);
        else if (e->sub_state_1 >= 4) re15_crow_sub(e, 14);             /* @0x80116144 */
        e->crow_hs = 0;
    }
    uint16_t fw2 = s_crow_flock;
    if (fw2 & 0x2000) {                              /* @0x8011616c */
        if (e->crow_hs != 0) s_crow_flock = (uint16_t)((fw2 & 0xfff) | 0x1000);  /* -> dive bit */
        e->crow_hs = 0;
        if (e->sub_state_1 >= 4 && e->grid_id != 0x80) re15_crow_sub(e, 17);     /* @0x801161d0 */
    }
}

/* ---- STEER phase — steer-table @0x8012113c[+0x5] (decision/transition only) ------------ */
static void re15_crow_steer(re15_actor_t *e, re15_actor_t *player)
{
    switch (e->sub_state_1) {
    case 0: case 1: case 2: case 3: {   /* DIVE-DECIDE 0x80112628 (4 commit paths, all evaluated;
                                         * NO early return — byte-true the last set_substate wins). */
        if (e->crow_pturn != 0) {                                   /* (A) pending-turn @0x80112638-9c */
            e->crow_pturn = 0;
            e->rot_y = (int16_t)(e->rot_y + ((e->crow_mode & 1) ? 40 : -40));
            re15_crow_sub(e, (uint8_t)(e->crow_mode & 3));
        }
        int ring = (e->grid_id < 0x80) ? 5000 : 10000;             /* (B) @0x801126b4 sltiu 0x80;
                                                                    * 0x1388/0x2710 @0x801126c8/d4 */
        if (e->crow_dist < ring && e->crow_vert_err < 5400) re15_crow_sub(e, 4);  /* @0x801126e8-f8 */
        /* (C) PLAYER-DOWN latch @0x80112700-38: `lhu 0x800aca52; andi 1` -> `lh +0x1ec; slti 5400`
         * -> `jal 0x80115d74(4)` — while the knockdown latch is set every patrol crow dives.
         * (audit wf_827f186d crow #A — raw-disasm CONFIRMED; producer = knockdown FSM, OPEN) */
        if ((g_aca52_flags & 1) && e->crow_vert_err < 5400) re15_crow_sub(e, 4);
        /* (D) flock 0x1000 -> dive @0x8011273c-88: `andi 0x1000; beq zero->exit` then straight to
         * the mod-3 pick — NO +0x1ec read in this path (the 5400 gates belong to paths B/C only;
         * the old port gate here was invented — audit wf_827f186d crow #12). */
        if (s_crow_flock & 0x1000)
            re15_crow_sub(e, (uint8_t)((e->crow_mode % 3) + 1));
        return;
    }
    case 5:   /* DIVE-END 0x80112bac: climbed >3600 above perch -> resume */
        if (e->y < e->crow_perch_h - 3600) re15_crow_sub(e, 9);
        return;
    case 7: case 8:   /* 0x80112e4c / 0x801130fc: floor-ref (+0x1ba) != perch (+0x1ea) -> sub 5
                       * (steer[7] @0x80112e5c-68, steer[8] @0x8011310c-18 `lh 442; lh 490; beq`)
                       * — the "don't land on a wall strip" gate; fires ONLY where the terrain
                       * under the crow differs from the spawn plane, never on the open floor
                       * (the old y<perch-400 stand-in fired during every airborne descent).
                       * (audit wf_827f186d crow #5) */
        if (e->crow_floor != e->crow_perch_h) re15_crow_sub(e, 5);
        return;
    case 9: {   /* 0x8011325c: settle re-arm, then ATTACK-COMMIT */
        if (e->crow_pturn != 0) {                       /* @0x8011326c-7c: clear +0x1d3 */
            e->crow_pturn = 0;
            if ((e->crow_mode & 0xf) != 0) re15_crow_sub(e, 9);   /* @0x80113294-a4 */
            else if (e->crow_mode & 0x80)  re15_crow_perch_return(e);  /* @0x801132a8->2c0 jal
                                                                        * 0x80115f00 (audit crow #4) */
            else if (e->sub_state_1 != 6)  re15_crow_sub(e, 6);   /* @0x801132b0-b8 (+0x5!=6 -> 6) */
            else                           re15_crow_perch_return(e);  /* +0x5==6 arm (@0x801132c0;
                                                                        * dead in-context: +0x5==9) */
            if (e->crow_atk_ctr >= 3) re15_crow_perch_return(e);
                                                              /* @0x801132e4-f8 `lbu +0x1d6; sltiu 3;
                                                               * miss -> jal 0x80115f00` — NUR im
                                                               * pturn-Pfad (@0x80113274 beq ->
                                                               * 0x80113300 ueberspringt es sonst;
                                                               * crow_shot_attack.md F7 CONFIRMED —
                                                               * lief faelschlich jeden Tick)
                                                               * (audit wf_827f186d crow #4) */
        }
        if (e->crow_parity != 0 && e->crow_armed != 0 &&
            e->crow_dist < 10000 && e->crow_atk_ctr < 3) {          /* attack-commit @0x8011330c-70 */
            re15_crow_sub(e, (e->grid_id & 0x80) ? 10 : 12);        /* downed->cruise, else grapple */
        }
        return;
    }
    case 10:   /* 0x801134f8 — all three checks fall through to ONE epilogue @0x80113580; the
                * LAST set_substate wins (set_substate 0x80115d74 is unconditional). The old
                * early-return on dist<2500 skipped the vert-err override (audit wf_827f186d crow #11). */
        if (e->crow_armed == 0)          re15_crow_sub(e, 5);       /* @0x80113508-1c */
        if (e->crow_dist < 2500)         re15_crow_sub(e, 14);      /* @0x80113534-44 sltiu 0x9c4 */
        if (e->crow_vert_err >= 5401)    { re15_crow_sub(e, 5); e->crow_atk_ctr = 4; }  /* @0x8011355c-7c */
        return;
    case 11: {  /* 0x8011376c — sequential like steer[10]: wall-abort, AABB, vert-err all evaluated */
        if (e->crow_wall != 0) re15_crow_sub(e, 14);    /* @0x8011377c-90 `lbu +0x1d1; bne zero ->
                                                         * jal 0x80115d74(0xe)` — dive-attack breaks
                                                         * off into the bank-away on wall contact
                                                         * (audit wf_827f186d crow #B, CONFIRMED) */
        int32_t adx = player->x - e->x, adz = player->z - e->z, ady = player->y - e->y;   /* AABB box */
        if (adx < 0) adx = -adx; if (adz < 0) adz = -adz; if (ady < 0) ady = -ady;
        if (adx < 0xf00 && adz < 0xf00 && ady < 0x300) re15_crow_sub(e, 14);  /* 0x8001b9b4 @0x8011379c-b0
                                                         * (falls through to the vert check @0x801137b4) */
        if (e->crow_vert_err >= 5401)    { re15_crow_sub(e, 5); e->crow_atk_ctr = 4; }  /* @0x801137c0-e8 */
        return;
    }
    case 12: case 13: case 15: case 16:   /* 0x80113c0c (shared) */
        if (e->crow_armed == 0)          { re15_crow_sub(e, 14); return; }
        if (e->crow_vert_err >= 5401)    { re15_crow_sub(e, 5); e->crow_atk_ctr = 4; }
        return;
    case 14:   /* 0x80114100 */
        if (e->crow_dist >= 12001)       re15_crow_sub(e, 9);      /* too far -> break off */
        return;
    default:   /* 4,6,17 = bare ret */
        return;
    }
}

/* ---- MOVE phase — move-table @0x80121184[+0x5] (motion + physics) ---------------------- */
static void re15_crow_move(re15_actor_t *e, re15_actor_t *player)
{
    switch (e->sub_state_1) {
    /* --- patrol[0-3]: clip-set + anim-advance + timer; transitions owned by steer --- */
    case 0:
        if (e->sub_state_2 == 0) { e->crow_timer = (uint8_t)(e->crow_mode % 60); e->sub_state_2 = 1; }
        {   /* @0x80112824-2c: `addiu v1,a0,255; bne a0,zero; sb v1` — Dekrement IMMER (0 wrappt
             * zu 255, Store im Delay-Slot), bei t==0 ZUSAETZLICH +0x1d3++ (@0x80112844-48).
             * Dieselbe Wrap-Semantik wie move[6]/[7]/[12]/[13]/[14]. Dossier crow_1170.md D5. */
            uint8_t t0 = e->crow_timer;
            e->crow_timer = (uint8_t)(t0 - 1);
            if (t0 == 0) e->crow_pturn++;              /* completion flag */
        }
        return;
    case 1:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 1); e->sub_state_2 = 1; }
        e->crow_pturn = (uint8_t)re15_crow_anim(e);
        return;
    case 2:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 2); e->sub_state_2 = 1; }
        e->crow_pturn = (uint8_t)re15_crow_anim(e);
        return;
    case 3:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 0); e->sub_state_2 = 1; }
        e->crow_pturn = (uint8_t)re15_crow_anim(e);
        if (e->crow_pturn) re15_audio_room_se(6);          /* Se(6) on anim-complete @0x801129f4 */
        return;

    /* --- sub 4: DIVE LAUNCH (climb via vvel -80 + frame-8 re-thrust) --- */
    case 4:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 6); e->crow_diveflag = 1; re15_audio_room_se(1); e->sub_state_2 = 1; }  /* Se(1) @0x80112a7c */
        if (e->sub_state_2 == 1) { e->crow_vvel = -80; e->crow_speed = 60; e->sub_state_2 = 2; }
        if (e->anim_frame == 8) { e->sub_state_2 = 1; re15_audio_room_se(1); }   /* frame-8 re-thrust + flap Se(1) @0x80112b10 */
        e->crow_vvel = (int16_t)(e->crow_vvel + 6);        /* gravity */
        e->y += e->crow_vvel;
        re15_crow_weave(e);                                /* jal 0x80115e24 @0x80112b60 (Delay-Slot
                                                            * sw y) — Weave-Bank VOR pos_advance;
                                                            * fehlte = Take-off ohne Ausweich-Bank
                                                            * bei dist<2000. Dossier crow_1170.md D2. */
        re15_crow_advance(e);
        if (re15_crow_anim(e)) re15_crow_sub(e, 5);        /* anim end -> second arc */
        return;

    /* --- sub 5: SECOND ARC (stronger climb vvel -120) --- */
    case 5:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 4); re15_audio_room_se(0); e->sub_state_2 = 1; }  /* Se(0) @0x80112c38 */
        if (e->sub_state_2 == 1) { e->crow_vvel = -120; e->crow_speed = 160; e->sub_state_2 = 2; }
        if (e->anim_frame == 8) e->sub_state_2 = 1;        /* frame-8 re-thrust (@0x80112cac) */
        e->crow_vvel = (int16_t)(e->crow_vvel + 6);
        e->y += e->crow_vvel;
        re15_crow_weave(e);                                /* jal 0x80115e24 @0x80112cf8 — Weave-Bank
                                                            * VOR pos_advance (Dossier D2) */
        re15_crow_advance(e);
        re15_crow_anim(e);
        return;

    /* --- sub 6: CRUISE / oracle-driven climb-descend (altitude + yaw only) ---
     * move[6] 0x80112d34: the entry (+0x6==0 @0x80112d64-98) FALLS THROUGH into the main body
     * @0x80112d9c the SAME tick, the timer decrements UNCONDITIONALLY in the branch delay slot
     * (`addiu v1,a0,255; bne a0,zero; sb v1` @0x80112db0-b8 — 0 wraps to 255), the sub(9) call on
     * expiry @0x80112dbc ALSO falls through to the steer/oracle/integrate, and the tick ends with
     * anim_set f314 @0x80112e34 (was missing). (audit wf_827f186d crow #16) */
    case 6: {
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 6); e->crow_timer = (uint8_t)(e->crow_mode & 0x32); re15_audio_room_se(0); e->sub_state_2 = 1; }  /* Se(0) @0x80112d78 */
        uint8_t t6 = e->crow_timer;
        e->crow_timer = (uint8_t)(t6 - 1);                     /* unconditional, wraps @0x80112db8 */
        if (t6 == 0) re15_crow_sub(e, 9);                      /* @0x80112dbc — NO return */
        re15_enemy_steer_point(e, player->x, player->z, 0x32); /* a8f8(player,0x32) @0x80112dcc */
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) * re15_crow_height_dir(e));
        e->y += e->crow_vvel;
        re15_crow_anim(e);                                     /* f314 @0x80112e34 */
        return;
    }

    /* --- sub 7: CRASH-DIVE / BOUNCE ---
     * move[7] 0x80112e88: step0 (@0x80112edc-f1c) FALLS THROUGH into step1 @0x80112f20 the same
     * tick; step1's timer decrements unconditionally (wraps 0->255, delay slot @0x80112f3c); the
     * bounce gate measures the per-tick FLOOR ref @0x80113018-24 `lh +0x1ba; addiu -750`, not the
     * spawn perch. (audit wf_827f186d crow #16 + #5) */
    case 7:
        if (e->sub_state_2 == 0) { e->crow_speed = 140; e->crow_vvel = 0; re15_crow_clip(e, 5); e->crow_timer = 21; e->sub_state_2 = 1; }  /* @0x80112edc-f00 -> falls into step1 */
        if (e->sub_state_2 == 1) {
            uint8_t t7 = e->crow_timer;
            e->crow_timer = (uint8_t)(t7 - 1);                 /* unconditional wrap @0x80112f3c */
            if (t7 == 0) e->sub_state_2 = 2;                   /* @0x80112f4c-58 (falls through) */
            e->crow_speed = (int16_t)(e->crow_speed + 3);
            e->crow_vvel  = (int16_t)(e->crow_vvel + 8);
            e->y += e->crow_vvel; re15_crow_advance(e); re15_crow_anim(e);
        } else if (e->sub_state_2 == 2) {
            e->crow_vvel = (int16_t)(e->crow_vvel + 3);
            e->y += e->crow_vvel;
            if (e->y > e->crow_floor - 750) { e->sub_state_2 = 3; e->crow_vvel = 0; e->crow_timer = 12; }  /* +0x1ba-750 @0x80113018-20 */
            re15_crow_weave(e); re15_crow_advance(e);
        } else if (e->sub_state_2 == 3) {   /* step 3: bounce */
            e->crow_speed = (int16_t)(e->crow_speed + 9);
            e->crow_vvel  = (int16_t)(e->crow_vvel - 9);
            e->y += e->crow_vvel;
            uint8_t t7b = e->crow_timer;
            e->crow_timer = (uint8_t)(t7b - 1);                /* unconditional wrap @0x801130d8 */
            if (t7b == 0) re15_crow_sub(e, 8);                 /* @0x801130dc — falls to advance */
            re15_crow_advance(e);                              /* @0x801130e4 */
        }
        return;

    /* --- sub 8: DESCEND-AND-LAND (clamp to FLOOR-400, anim end -> sub 0) ---
     * move[8] 0x80113138: step0 (@0x80113168-98) FALLS THROUGH into the body @0x8011319c the
     * same tick; the land clamp is `lh +0x1ba; addiu -400` @0x801131e4-fc — the floor under the
     * crow, not the spawn perch. (audit wf_827f186d crow #16 + #5) */
    case 8:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 7); e->crow_vvel = 0; e->sub_state_2 = 1; }  /* no return — falls through */
        e->crow_vvel = (int16_t)(e->crow_vvel + 10);
        e->y += e->crow_vvel;
        if (!(e->y < e->crow_floor - 400)) e->y = e->crow_floor - 400;   /* land clamp @0x801131e4-fc */
        if (re15_crow_anim(e)) { re15_crow_sub(e, 0); e->crow_diveflag = 0; e->crow_atk_ctr = 0; }
        return;

    /* --- sub 9: RESUME / CLIMB-BACK cruise toward the player --- */
    case 9:
        if (e->sub_state_2 == 0) {
            if (e->crow_mode & 1) { e->crow_speed = 160; re15_crow_clip(e, 3); }
            else                  { e->crow_speed = 180; re15_crow_clip(e, 4); }
            e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) * re15_crow_height_dir(e));
            e->sub_state_2 = 1;
        }
        e->y += e->crow_vvel;
        if (e->crow_atk_ctr < 3) { re15_enemy_steer_point(e, player->x, player->z, 0x1e); re15_crow_weave(e); }
        else if (e->crow_mode & 4) re15_enemy_steer_point(e, player->x, player->z, 0x1e);
        re15_crow_advance(e);
        e->crow_pturn = (uint8_t)re15_crow_anim(e);
        return;

    /* --- sub 10: CRUISE-TO-PLAYER (approach at corridor, arrival -> sub 11 dive) --- */
    case 10:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 6); re15_audio_room_se(1); e->crow_speed = 0; e->sub_state_2 = 1; }  /* Se(1) @0x801135e8 */
        if (e->sub_state_2 == 1) {
            re15_enemy_steer_point(e, player->x, player->z, 0x32);
            int32_t tgt = e->crow_perch_h;
            if (tgt - 3600 < e->y)      { e->crow_vvel = (int16_t)(-(e->crow_mode & 0x3f)); e->y += e->crow_vvel; }
            else if (e->y < tgt - 5400) { e->crow_vvel = (int16_t)( (e->crow_mode & 0x3f)); e->y += e->crow_vvel; }
            else { e->crow_speed = 50; e->sub_state_2 = 2; }
            re15_crow_anim(e);       /* ALLE step1-Zweige (beide Korridor-Zweige UND der In-Band-
                                      * Uebergang) muenden in jal 0x8001f314 @0x801136dc-f4 — ohne
                                      * den Tick friert clip 6 waehrend der Steigphase auf Frame 0
                                      * (Kraehen sind vom geteilten Anim-Pass ausgenommen).
                                      * Dossier crow_1170.md D3 (CONFIRMED). */
            return;
        }
        re15_enemy_steer_point(e, player->x, player->z, 0x32);
        re15_crow_advance(e);
        {   /* ARRIVAL oracle 0x8001a804(thresh=9000, yaw-tol=100): dist<=9000 AND facing player */
            int fb = ((int)re15_atan2_q12(player->z - e->z, player->x - e->x) - 0x400) & 0xfff;
            int df = (((fb - (int)e->rot_y) + 0x800) & 0xfff) - 0x800;
            if (e->crow_dist <= 9000 && df >= -100 && df <= 100) re15_crow_sub(e, 11);
        }
        return;

    /* --- sub 11: DIVE ATTACK (the swoop; -4 HP on connect) --- */
    case 11:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 4); e->crow_speed = 80; e->crow_accel = 20; e->crow_vvel = 200; e->crow_yawrate = 5; e->crow_timer = 9; e->sub_state_2 = 1; }
        else if (e->sub_state_2 == 1) {
            if (e->crow_timer == 0) { e->crow_timer = 30; e->sub_state_2 = 2; } else e->crow_timer--;
            re15_enemy_steer_point(e, player->x, player->z, e->crow_yawrate);
            e->crow_speed = (int16_t)(e->crow_speed + 20);
            e->crow_vvel  = (int16_t)(e->crow_vvel - 7);
            re15_crow_anim(e);
        } else {
            if (e->crow_timer == 0) { re15_crow_sub(e, 5); } else e->crow_timer--;
            e->crow_yawrate = (int16_t)(e->crow_yawrate + 2);
            re15_enemy_steer_point(e, player->x, player->z, e->crow_yawrate);
            e->crow_speed = (int16_t)(e->crow_speed + e->crow_accel);
            e->crow_vvel  = (int16_t)(e->crow_vvel - 7);
            if (e->crow_dist < 600 && (uint16_t)(e->crow_vert_err - 1) < 3599 && player->hit_react == 0) {
                e->crow_bank = 6;                            /* +0x1d7=6 post-hit latch @0x80113ad4-d8
                                                              * `ori v0,6; sb v0,471` — suppresses the
                                                              * weave re-latch (0x80115e44-4c skips on
                                                              * ANY nonzero +0x1d7) for ~6 ticks
                                                              * (audit wf_827f186d crow #13) */
                e->crow_accel = -20;                         /* @0x80113ae4/aec */
                re15_audio_room_se(4);                       /* Se(4) hit @0x80113ae8 */
                re15_crow_hit_player(e, player, 4);          /* DIVE HIT: -4 HP @0x80113b04; player
                                                              * cmd 2 @0x80113b00 + dir @0x80113b28 +
                                                              * aca5a=0 @0x80113b30 -> Port-Flinch-
                                                              * Detector (game_step_common.c) */
                re15_crow_attack_wounds(e);                  /* 0x801161e8 @0x80113b7c — die
                                                              * VOLLSTAENDIGEN vert-Band-Wundstempel
                                                              * (die alte Inline-Teilkopie fehlte
                                                              * Panels 6/5 im Mittelband + das ganze
                                                              * 3000er-Band; crow_victim_anim.md §4.4) */
            }
            /* phase-2 tail @0x80113b90-bd0: nonzero +0x1d7 -> PRO FRAME ein BLUT-Spawn mit
             * scale = ctr<<11 (@0x80113b98 sll 0xb, Spawn @0x80113bb0) am KRAEHEN-Bone 2
             * (pool+0x198), a3=(0,0,0) @0x8012110c; Dekrement @0x80113bcc. KORRIGIERT
             * (bite_blood_fx.md §3.1 + Addendum, CONFIRMED): die alte "NULL table =
             * invisible"-Lesart war falsch — das ist die sichtbare 6-Frame-Blutfahne
             * nach dem Dive-Treffer. */
            if (e->crow_bank != 0) {
                int32_t cb2[3]; re15_enemy_bone_world_pos(e, 2, cb2);
                re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0,
                                     (uint16_t)((unsigned)e->crow_bank << 11),
                                     cb2[0], cb2[1], cb2[2], (int16_t)e->rot_y);
                e->crow_bank--;
            }
        }
        e->y += e->crow_vvel;
        re15_crow_advance(e);
        return;

    /* --- sub 12: GRAPPLE STRIKE (contact -> grab, -8 HP) ---
     * move[12] 0x80113c7c: step0 (@0x80113cac-d58) stores ONLY +0x8c speed / +0x1e4 vvel / +0x6
     * step++ — NO +0x1d5 store (the old `timer = 30` was invented; the timeout @0x80113e68-78
     * `lbu 469; addiu 255; bne; sb` decrements whatever +0x1d5 held on ENTRY, wrapping 0->255).
     * step1's anim-wrap does `+0x6 -= 1` @0x80113d6c-8c — each completed flap cycle re-runs step0
     * with the CURRENT mode/vert-err (fresh clip 3/4, speed 160/180, vvel sign).
     * (audit wf_827f186d crow #10) */
    case 12:
        if (e->sub_state_2 == 0) {
            if (e->crow_mode & 1) { re15_crow_clip(e, 3); e->crow_speed = 160; }
            else                  { re15_crow_clip(e, 4); e->crow_speed = 180; }
            e->crow_vvel = (int16_t)(e->crow_mode & 0x3f);
            if (e->crow_vert_err < 2000) e->crow_vvel = (int16_t)(-e->crow_vvel);
            e->sub_state_2 = 1;                              /* NO timer write; step0 exits
                                                              * (j 0x80113e84 @0x80113d50 — byte-true) */
            return;
        }
        if (re15_crow_anim(e)) e->sub_state_2 = 0;           /* flap wrap -> +0x6-- @0x80113d6c-8c */
        e->y += e->crow_vvel;
        re15_enemy_steer_point(e, player->x, player->z, 0x32);
        re15_crow_advance(e);
        if (e->crow_contact) {                               /* +0x1d0 contact -> GRAB (@0x80113dd4) */
            e->crow_hs = 1;                                  /* +0x1d8=1 self-exempt @0x80113ddc */
            s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x8000);   /* @0x80113dfc */
            re15_crow_hit_player_ungated(e, player, 8);      /* GRAB: -8 HP @0x80113e34 UNGATED (kein
                                                              * +0x93 im Kontaktpfad — Dossier D4) */
            re15_player_victim_latch(e, player);             /* player cmd 5 @0x80113e48 + aca59=0
                                                              * @0x80113e30 + Paar-C-Link acbcc/acbd0 =
                                                              * crow+0x178/+0x17c @0x80113e08-14 ->
                                                              * Hook A [0x21] = LAB_8011597c front
                                                              * victim FSM: Leon spielt EM021-victim
                                                              * Clips 0 -> 1(Loop) -> 2
                                                              * (crow_victim_anim.md F1/F5) */
            e->crow_struggle = 100;
            re15_crow_sub(e, 13);
        } else {
            uint8_t t12 = e->crow_timer;
            e->crow_timer = (uint8_t)(t12 - 1);              /* leftover value, wraps @0x80113e70-78 */
            if (t12 == 0) re15_crow_sub(e, 5);
        }
        return;

    /* --- sub 13: GRAB-HOLD / FEEDING (peck, struggle drain, release -> sub 14) --- */
    case 13:
        if (e->sub_state_2 == 0) {                           /* step0 0x80113ec4-fa4 */
            re15_crow_clip(e, 8);                            /* @0x80113ec4-c8 */
            e->crow_atk_ctr++;                               /* @0x80113ee0-e4 */
            e->crow_struggle = 100;                          /* @0x80113ef0/ef8 (delay slot) */
            re15_crow_attack_wounds(e);                      /* 0x801161e8 @0x80113ef4 — der Grab-
                                                              * Entry STEMPELT die vert-Band-Wunden
                                                              * (kein Screech; das Se(2) folgt separat
                                                              * @0x80113f74-78 unten) */
            {   /* Peck-Grab-Entry-BLUT: 0x2000 am Kraehen-Bone 2 (@0x80113f0c/0x80113f6c;
                 * bite_blood_fx.md §3.1 D5b, CONFIRMED) */
                int32_t pb2[3]; re15_enemy_bone_world_pos(e, 2, pb2);
                re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000,
                                     pb2[0], pb2[1], pb2[2], (int16_t)e->rot_y);
            }
            if (player->hp < 0) {                            /* lethal recheck @0x80113f08: cmd 3
                                                              * @0x80113f20 (death FSM = OPEN) + KILL
                                                              * broadcast @0x80113f24-30 + +0x1d8=1 */
                s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x2000);
                e->crow_hs = 1;                              /* @0x80113f50 */
            }
            re15_audio_room_se(2);                           /* Se(2) @0x80113f74-78 */
            e->crow_timer = 30;                              /* +0x1d5=0x1e @0x80113f84-88 */
            e->sub_state_2 = 1;
        }
        re15_crow_anim(e);
        if (e->crow_timer == 0) { e->crow_timer = 30; re15_audio_room_se(2);    /* re-peck every 30f @0x8011400c */
            /* pro Peck-Zyklus ein 0x2000-Blut am Bone 2 (Zyklus-Gate @0x80113fc4, Spawn
             * @0x80113fe4; bite_blood_fx.md §3.1, CONFIRMED) */
            int32_t kb2[3]; re15_enemy_bone_world_pos(e, 2, kb2);
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000,
                                 kb2[0], kb2[1], kb2[2], (int16_t)e->rot_y);
        }
        else e->crow_timer--;
        if ((e->crow_timer % 10) == 0) re15_audio_room_se(0);   /* chirp every 10f @0x80114060 */
        /* struggle drain = PAD-MASH driven, byte-true move[13] @0x80114068 `jal 0x80037024` ->
         * @0x80114078-8c `+0x9c -= 1 + 3*ret` (drain 1 or 4): FUN_80037024 = pad word 0x800ac762
         * & 0xf0f0 (any D-pad/face button) — mashing releases ~4x faster. The old RNG drain
         * `1 + (rand8()%3)*3` was invented (audit wf_827f186d crow #6). */
        e->crow_struggle = (int16_t)(e->crow_struggle - (1 + 3 * (re15_mash_pressed() ? 1 : 0)));
        if (e->crow_struggle < 0) {                          /* RELEASE */
            s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x4000);
            e->crow_hs = 1;
            re15_crow_sub(e, 14);
        }
        return;

    /* --- sub 14: BANK / TURN-AROUND (regroup peel-off) ---
     * move[14] 0x8011413c: step0 (@0x80114188-210) FALLS THROUGH into step1 @0x80114214 the same
     * tick (yaw already rotates on the entry tick); both timers decrement unconditionally with
     * wrap (@0x80114248-50 / @0x8011428c-94); the common tail ends with anim f314 @0x80114304
     * (was missing). (audit wf_827f186d crow #16) */
    case 14:
        if (e->sub_state_2 == 0) {
            re15_audio_room_se(0);                             /* Se(0) @0x80114188 */
            re15_crow_clip(e, 4); e->crow_speed = 180; e->crow_timer = 32;   /* @0x80114190-b4 */
            e->crow_yawrate = (e->crow_mode & 0x80) ? -60 : 60; /* +0x1de=0x3c, negate @0x801141e8-f4 */
            e->sub_state_2 = 1;                                /* falls into step1 @0x80114214 */
        }
        if (e->sub_state_2 == 1) {
            e->rot_y = (int16_t)(e->rot_y + e->crow_yawrate);  /* @0x80114220-30 */
            uint8_t t14 = e->crow_timer;
            e->crow_timer = (uint8_t)(t14 - 1);                /* unconditional wrap @0x80114250 */
            if (t14 == 0) { e->sub_state_2 = 2; e->crow_timer = 90; }  /* @0x80114260-80 */
        } else if (e->sub_state_2 == 2) {
            uint8_t t14b = e->crow_timer;
            e->crow_timer = (uint8_t)(t14b - 1);               /* unconditional wrap @0x80114294 */
            if (t14b == 0) re15_crow_sub(e, 9);                /* @0x80114298 */
        }
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) * re15_crow_height_dir(e));
        e->y += e->crow_vvel;
        re15_crow_advance(e);
        re15_crow_anim(e);                                     /* f314 @0x80114304 */
        return;

    /* --- sub 15: HOMING DESCENT (coordinated swoop-in -> strike) --- */
    case 15:
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 4); e->crow_speed = 180; e->crow_atk_ctr++;
            e->crow_vvel = (int16_t)(e->crow_mode & 0x3f);
            if (e->crow_vert_err < 2000) e->crow_vvel = (int16_t)(-e->crow_vvel);
            e->sub_state_2 = 1;
            return;
        }
        if (re15_crow_anim(e)) e->sub_state_2 = 0;
        re15_enemy_steer_point(e, player->x, player->z, 0x32);
        e->y += e->crow_vvel;
        if (e->crow_contact) re15_crow_sub(e, 16);   /* +0x1d0 contact -> STRIKE (@0x80114464), then
                                                      * pos_advance @0x8011446c runs REGARDLESS — the
                                                      * contact tick still moves (audit wf_827f186d
                                                      * crow #16; old return skipped the advance) */
        re15_crow_advance(e);
        return;

    /* --- sub 16: STRIKE / PECK (-4 HP) --- */
    case 16:
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 8);
            re15_crow_attack_wounds(e);                      /* 0x801161e8 @0x801144bc — Strike
                                                              * stempelt die vert-Band-Wunden */
            {   /* Strike-BLUT: 0x1000 am Kraehen-Bone 2 (@0x801144c4/0x801144e0;
                 * bite_blood_fx.md §3.1 D5c, CONFIRMED) */
                int32_t sb2[3]; re15_enemy_bone_world_pos(e, 2, sb2);
                re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1000,
                                     sb2[0], sb2[1], sb2[2], (int16_t)e->rot_y);
            }
            re15_crow_hit_player_ungated(e, player, 4);      /* STRIKE: -4 HP @0x801144f0-fc UNGATED
                                                              * (kein +0x93 im Pfad — Dossier D4) */
            e->sub_state_2 = 1;
        }
        re15_crow_anim(e);
        return;

    /* --- sub 17: FAST PLUNGE-TO-GROUND ---
     * move[17] 0x80114594: step0 (@0x801145c4-f4) FALLS THROUGH into the body @0x801145f8 the
     * same tick; the plunge stop + land gates measure the per-tick FLOOR ref (`lh +0x1ba;
     * addiu -750` @0x80114634-40 and @0x801146a0-ac, the land commit is STRICT floor-750 < y).
     * (audit wf_827f186d crow #16 + #5) */
    case 17:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 3); e->crow_speed = 100; e->sub_state_2 = 1; }  /* no return — falls through */
        re15_enemy_steer_point(e, player->x, player->z, 0x64);   /* a8f8(player,0x64) @0x80114600 */
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) + 48);    /* @0x8011461c-24 */
        if (e->y < e->crow_floor - 750) e->y += e->crow_vvel;    /* @0x80114634-58 */
        re15_crow_advance(e);                                    /* @0x8011465c */
        re15_crow_anim(e);                                       /* f314 @0x80114678 */
        if (e->crow_dist < 1000 && e->y > e->crow_floor - 750)   /* sltiu 0x3e8 @0x80114694;
                                                                  * slt floor-750,y @0x801146a0-b0 */
            re15_crow_sub(e, 8);
        return;

    default:
        return;
    }
}

/* FLIGHT-2 (state 4) sub-machine — byte-true 0x80114e54 family (workflow wf_2c7076b7, 0 refutations).
 * The grid&0x40 event crow's perch sequence via the substate table @0x80121220[0-4]: ASCEND -> HOVER
 * -> (grab on contact) ORIENT; SPIN/DIVE-ARM only fire on the event flags s_crow_gflags bit 0x1d/0x1e
 * (never set in STAGE1). FLIGHT-2 never writes +0x4 (no state exit) and never touches the death bit
 * 0x1f — so the event crow stays in state 4 and remains byte-true unkillable in STAGE1. */
static void re15_crow_flight2_sub(re15_actor_t *e, re15_actor_t *player)
{
    switch (e->sub_state_1) {
    case 0:   /* ASCEND 0x80114fb8: climb (vvel -120 + frame-8 re-thrust) -> HOVER when high enough */
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 4); e->sub_state_2 = 1; }
        if (e->sub_state_2 == 1) { e->crow_vvel = -120; e->crow_speed = 160; e->sub_state_2 = 2; }
        if (e->anim_frame == 8) e->sub_state_2 = 1;                     /* @0x80115088 re-thrust */
        e->crow_vvel = (int16_t)(e->crow_vvel + 6);                     /* @0x801150a4 gravity */
        re15_enemy_steer_point(e, player->x, player->z, 0x14);          /* @0x801150cc yaw-slew rate 20 */
        e->y += e->crow_vvel;                                           /* @0x801150d0 integrate */
        re15_crow_advance(e);
        re15_crow_anim(e);
        if (e->crow_vert_err >= 2001) re15_crow_sub(e, 1);             /* @0x80115118 -> HOVER */
        break;

    case 1:   /* HOVER 0x80115130: oscillate + yaw-slew; grab the player on contact -> ORIENT */
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 4); e->crow_speed = 180;                  /* @0x80115164/70 */
            e->crow_vvel = (int16_t)(e->crow_mode & 0x3f);             /* @0x80115188 */
            if (e->crow_vert_err < 2000) e->crow_vvel = (int16_t)(-e->crow_vvel);  /* @0x801151a4 */
            e->sub_state_2 = 1;
        }
        if (re15_crow_anim(e)) e->sub_state_2 = 0;                      /* clip done -> next half-cycle */
        re15_enemy_steer_point(e, player->x, player->z, 0x32);          /* @0x801151f4 rate 50 */
        e->y += e->crow_vvel;
        re15_crow_advance(e);
        if (e->crow_contact) {                                          /* @0x80115248 grab gate (+0x1d0) */
            s_crow_gflags |= 0x8u;                                      /* @0x801152c0 setbit 0x1c */
            re15_crow_hit_player(e, player, 8);                         /* grab: 0x800aca58=5 (@0x801152a4) */
            re15_crow_sub(e, 2);                                        /* @0x801152b8 -> ORIENT */
        }
        break;

    case 2:   /* ORIENT 0x801152e0: hold the attack pose; promoted to SPIN by the dispatcher 0x1d-tick */
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 8); e->sub_state_2 = 1; }  /* @0x80115310 clip 8 */
        re15_crow_anim(e);   /* pose 0x80019700(a3=0x8012110c null table) = no distinct effect; hold */
        break;

    case 3:   /* SPIN 0x801153ac: spin the yaw, timer -> back to HOVER */
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 4); e->crow_speed = 180; e->crow_vvel = -30; e->crow_timer = 32; e->sub_state_2 = 1; }
        e->rot_y = (int16_t)(e->rot_y + 60);                           /* @0x80115460 yaw += 60 */
        if (e->sub_state_2 == 1) {
            if (e->crow_timer == 0) { e->sub_state_2 = 2; e->crow_vvel = 10; e->crow_timer = 40; }  /* @0x801154b4/c4 */
            else e->crow_timer--;
        } else {
            if (e->crow_timer == 0) re15_crow_sub(e, 1);              /* @0x801154e4 -> HOVER */
            else e->crow_timer--;
        }
        e->y += e->crow_vvel; re15_crow_advance(e); re15_crow_anim(e); /* @0x801154f8 */
        break;

    case 4:   /* DIVE-ARM 0x8011553c: the armed descent (attack flags +0x0|0x2/0x40/0x8); gated on bit 0x1e */
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 4); e->crow_speed = 80; e->crow_accel = 20;
            e->crow_vvel = 200; e->crow_yawrate = 5; e->crow_timer = 9;  /* @0x801155a4-d8 */
            if (s_crow_gflags & 0x2u) e->sub_state_2 = 1;              /* @0x801155e8 testbit 0x1e */
        } else if (e->sub_state_2 == 1) {
            if (e->crow_timer == 0) { e->crow_timer = 30; e->sub_state_2 = 2; } else e->crow_timer--;
            e->crow_speed = (int16_t)(e->crow_speed + 20);            /* @0x801156c0 */
            e->crow_vvel  = (int16_t)(e->crow_vvel - 7);
            re15_crow_anim(e);
        } else {
            if (e->crow_timer != 0) e->crow_timer--;
            e->crow_speed = (int16_t)(e->crow_speed + e->crow_accel);
            e->crow_vvel  = (int16_t)(e->crow_vvel - 7);
        }
        e->y += e->crow_vvel; re15_crow_advance(e);                    /* @0x801157b4 tail */
        break;

    default:  /* sub 5-9 = the SPECIAL/fade handlers (0x80115830+), out of scope in STAGE1 */
        break;
    }
}

/* DEATH (state 3) — byte-true 0x801146d0 (crow-cluster-wave2, verified). Fall from the sky
 * (gravity +0x1e8=0x26), land (clip 0x0a), finish -> state 7 (corpse). Reached via the root
 * promotion (state 4 + grid&0x40 + mode bit-0x1f); byte-true UNREACHABLE in STAGE1 (bit-0x1f only
 * STAGE3/5), ported for full-stage portability. Lane on +0x5: [0-6] normal (nested on +0x7 =
 * sub_state_3), [7] gib (the feather burst). */
static void re15_crow_death(re15_actor_t *e)
{
    /* Death-Lane = weapon_id (+0x5 nach dem Schuss, @0x800124bc; Dispatcher @0x80114700-24
     * jalr @0x801211cc[+0x5] OHNE Bounds-Check): [7,8,9,10,11,13,15,16,17,18] -> GIB-Lane
     * 0x801149c4 (auch Shotgun + alle schweren Waffen zerlegen in Federn — der Port GIBte
     * vorher nur bei Waffe 7), [0-6,12,19,20] -> Normal-Fall 0x80114738, [14] -> dritte
     * Lane 0x80114ba4 (Body ungelesen, dokumentiert OFFEN — faellt hier auf Normal-Fall).
     * crow_shot_attack.md F3 (CONFIRMED). */
    {
        int wl = e->sub_state_1;
        int gib = (wl >= 7 && wl <= 11) || wl == 13 || (wl >= 15 && wl <= 18);
        if (gib && e->sub_state_1 != 7) e->sub_state_1 = 7;   /* auf die Port-GIB-Spur mappen */
    }
    if (e->sub_state_1 == 7) {   /* GIB lane 0x801149c4: 13 feather children (@0x80114a50) -> corpse */
        if (e->sub_state_2 == 0) {
            re15_esp_fx_splatter(re15_esp_room_bank(), 0, 13, e->x, e->y, e->z, e->crow_perch_h);
            e->hp = -1; e->crow_timer = 0x32; e->sub_state_2 = 1;    /* +0x1d5=0x32 */
            /* GIB-Lane-Flock-Write @0x801149fc-4a20: aca50++, bei armed Re-Arm-Broadcast
             * (aca50&0xf0ff)|0x800 (Dossier crow_1170.md D6). */
            s_crow_flock = (uint16_t)(s_crow_flock + 1);
            if (e->crow_armed)
                s_crow_flock = (uint16_t)((s_crow_flock & 0xf0ff) | 0x800);
        } else if (e->crow_timer == 0) { e->state = 7; re15_crow_sub(e, 0); }  /* @0x80114b90 +0x4=7 */
        else e->crow_timer--;
        return;
    }
    switch (e->sub_state_3) {     /* normal death, nested on +0x7 (step-router 0x80114738) */
    case 0:   /* INIT 0x80114784 */
        re15_audio_room_se(3);                                       /* Se(3) @0x80114784 */
        e->hp = -1; e->crow_vvel = 0; e->crow_grav = 0x26;           /* +0x9a=-1, +0x1e4=0, +0x1e8=0x26 */
        e->crow_speed = 0; e->rot_z = 0; e->sub_state_3 = 1;         /* +0x8c=0, +0x6c=0 */
        /* Death-Lane-Flock-Write @0x801147fc-8024: `lhu aca50; addiu +1; sh`, dann bei armed
         * (+0x1db!=0) Re-Arm-Broadcast `(aca50)&0xf0ff|0x800` an die disarmten Flock-Mates
         * (Konsument = ACTIVE-Tail-One-Shot @0x801125dc-614). GIB-Lane-Write oben; die
         * @0x80114c68-c90-Writes gehoeren zur Waffen-14-Lane 0x80114ba4 (nicht portiert,
         * crow_shot_attack.md F3). KORREKTUR (F8): die Death-Lanes SIND in STAGE1 per Schuss
         * erreichbar (jeder Treffer toetet, Lane = weapon_id) — nur die Root-PROMOTION
         * (bit 0x1f) ist STAGE3/5-only; das alte "STAGE1-unerreichbar"-Etikett galt nur ihr. */
        s_crow_flock = (uint16_t)(s_crow_flock + 1);
        if (e->crow_armed)
            s_crow_flock = (uint16_t)((s_crow_flock & 0xf0ff) | 0x800);
        break;
    case 1: { /* FALL 0x80114828 */
        e->rot_z = (int16_t)(e->rot_z + 140); if (e->rot_z > 1024) e->rot_z = 1024;  /* +0x6c spin, clamp */
        e->crow_speed = 60;                                          /* +0x8c=60 */
        e->crow_vvel = (int16_t)(e->crow_vvel + e->crow_grav);       /* gravity @0x80114894 */
        e->y += e->crow_vvel; re15_crow_advance(e);                  /* integrate @0x801148b4 */
        if (e->y >= e->crow_floor - 400) {                           /* land: `lh +0x1ba; addiu -400`
                                                                      * @0x801148cc-e4 (audit wf_827f186d
                                                                      * crow #5 — was the spawn perch) */
            e->y = e->crow_floor - 400;
            re15_crow_clip(e, 0x0a); re15_audio_room_se(5);          /* land clip 0x0a + Se(5) @0x801148f4 */
            e->crow_timer = 11; e->sub_state_3 = 2;                  /* +0x1d5=11 */
        }
        break;
    }
    case 2:   /* FINISH 0x80114934 */
        re15_crow_anim(e);
        if (e->crow_timer == 0) { e->state = 7; e->sub_state_1 = 0; e->sub_state_3 = 0; }  /* +0x4=7 @0x80114978 */
        else e->crow_timer--;
        break;
    }
}

/* SPECIAL (state 7) — byte-true 0x801157e8: the CORPSE settle / color-fade (dispatch on +0x5;
 * [0] color-fade 0x80115830, [2/3] scripted event — scene-specific, deferred). The dead crow lies
 * in its landed pose (clip 0x0a); the render draws it like the zombie corpse (state 7). The exact
 * +0xbc/+0xbe/+0xc4 shadow-recolor fade is the shared render-side corpse pool. */
static void re15_crow_special(re15_actor_t *e)
{
    re15_crow_anim(e);   /* hold the corpse pose; the fade is render-side (like the zombie corpse) */
}

/* One crow AI tick (dispatched from re15_enemy_ai_run_all for type 0x21).
 * Full flight brain — byte-true port of the FUN_80112020 family (RE15_CROW_AI.md):
 * root pre-pass -> INIT / ACTIVE (sense -> flock-dispatch -> steer[+0x5] -> move[+0x5]). */
static void re15_crow_ai_tick(int slot)
{
    re15_actor_t *e      = &g_actors[slot];
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* --- ROOT pre-pass (0x80112020) --- */
    /* +0x1d4 = a FRESH RNG BYTE every root tick: @0x80112028 `jal 0x8001af20` (rng), stored
     * @0x8011204c `sb v0,468(v1)` in the DELAY SLOT of the @0x80112048 testbit jal — v0 still
     * holds the rng return there (only s0/a0/v1/a1 written in between). The old port read
     * `crow_mode = testbit(0x800b1028,0x1f)` was a delay-slot misread (same error as
     * RE15_CROW_AI.md:236) that zeroed every mode-derived speed/timer/dir in STAGE1.
     * (audit wf_827f186d crow #1, HIGH) */
    e->crow_mode = re15_engine_rand8();
    /* DEATH promotion (@0x80112050-98): the testbit(0x800b1028,0x1f) result gates ONLY this —
     * a state-4 grid&0x40 crow dies when the scripted death bit 0x1f is set (STAGE3/5 only). */
    if ((s_crow_gflags & 0x1u) && e->state == 4 && (e->grid_id & 0x40)) {
        e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4=3, 0x80115d74(0) */
        s_crow_gflags &= ~0x1u;                                                     /* clearbit 0x1f @0x80112098 */
    }
    /* @0x801120a0-e8: testbit(0x800b1028,0x1d) && player-cmd 0x800aca58==5 && 0x800aca5a<3 ->
     * 0x800aca5a=3 (the FLIGHT-2 SPIN escalation of the GRABBED player command FSM). OPEN: the
     * player command bytes aca58/aca59/aca5a are the unported player reaction dispatcher
     * @0x80073f90 (see re15_crow_hit_player). */
    /* GLOBAL FREEZE GATE @0x801120ec-124 (audit wf_827f186d crow #7): skip the state dispatch +
     * post-pass iff (g_pauseflags & 0x20000000) && !(grid_id & 0x20) — NOTE the crow's grid&0x20
     * is a freeze-EXEMPTION (`lbu +0x9; andi 0x20; beq zero -> skip` @0x80112114-20), NOT the
     * zombie-style OR-skip. The frozen crow still draws its shadow (@0x8011221c-234, render-side). */
    if (s_ai_paused && !(e->grid_id & 0x20)) return;
    /* +0x82 = -(y/1800) @0x80112128-54 (mult 0x91a2b3c5; mfhi+addu; sra 10; negate) — the crow's
     * altitude band byte, refreshed every unfrozen root tick (audit wf_827f186d crow #15). */
    e->floor = (uint8_t)(-(e->y / 1800));
    /* +0x1ba floor-Y refresh @0x80112158-84: room_coll FUN_8001c6e8(&+0x34, a1=dim[3]=200
     * (lhu +0x78[6] @0x8011216c), a2=8, a3=0x400) every unfrozen root tick, ALL states
     * (audit wf_827f186d crow #5). */
    e->crow_floor = re15_crow_room_coll(e);

    switch (e->state) {
    case 0: {  /* INIT — FUN_80111a4c: count-flock, capture perch, lift off, enter ACTIVE */
        int n = 0;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x21) n++;
        s_crow_flock    = (uint16_t)(n << 4);        /* 0x800aca50 = count<<4 (@FUN_80111a4c) */
        e->crow_perch_h = (int16_t)e->y;             /* +0x1ea = spawn Y                      */
        e->y           -= 400;                       /* +0x38 -= 400 lift-off                 */
        e->crow_mode    = 0;                          /* +0x1d4 = 0: the INIT preamble clears the
                                                       * flight block +0x1d0..+0x1ec (FUN_80111a4c);
                                                       * the root pre-pass overwrites it with a fresh
                                                       * rng byte every tick from the next tick on
                                                       * (@0x80112028/0x8011204c) */
        e->crow_vvel = 0; e->crow_speed = 0; e->crow_atk_ctr = 0; e->crow_diveflag = 0;
        re15_enemy_apply_hitbox(e, 0x21);             /* INIT FUN_80111a4c installiert +0x78 =
                                                       * DAT_80121108 -> Box @0x801210fc =
                                                       * {0,0,0,200,180,200} (byte-verifiziert
                                                       * STAGE1.BIN file 0x210fc). FEHLTE im Port:
                                                       * hit_radius_min blieb 0 -> crow_contact
                                                       * (aec4-Tail) konnte NIE feuern -> Kraehen
                                                       * haben im Port noch nie zugepackt
                                                       * (Nutzer-Report 2026-08-02, Sonde
                                                       * probe_crow_1170: sub 12 dauerhaft ohne
                                                       * Kontakt). */
        e->crow_armed   = (uint8_t)((e->grid_id & 0x10) ? 0 : 1);
                                                      /* +0x1db = (grid&0x10) ? 0 : 1 — INIT
                                                       * @0x801123cc-e8 (`andi grid,0x10; bne ->
                                                       * sb zero,475 im j-Delay-Slot; sonst sb 1`).
                                                       * ROOM1170 spawnt 7 Kraehen mit grid
                                                       * 2x0x90/3x0x10/2x0x00 -> 5/7 PERMANENT
                                                       * disarmed: sie launchen (Dive-Decide liest
                                                       * +0x1db nicht), aber steer[9] @0x8011331c-24,
                                                       * steer[10] @0x80113508-1c und steer[12/13/
                                                       * 15/16] @0x80113c1c-2c verweigern jeden
                                                       * Attack-Commit. Re-Arm NUR via aca50-Bit
                                                       * 0x800 aus den Death-Lanes. Der alte
                                                       * Hartcode `=1` galt nur fuer ROOM10C0
                                                       * (grid=0). Dossier analysis/crow_1170.md D1
                                                       * (CONFIRMED). */
        re15_crow_clip(e, 0);
        re15_crow_anim(e);                            /* INIT macht nach dem Clip-Set EINEN f314-Tick
                                                       * (@0x80112300) -> +0x95=1 am Spawn-Ende
                                                       * (Dossier D11; ohne ihn 1 Frame Phasenversatz
                                                       * aller Wrap-Trigger des ersten Clips) */
        e->anim_flags   = 0x04;                       /* PORT-SHIM (kein Original-Write; em_set
                                                       * schreibt +0x1c4=0): Render-Loop-Hint fuer
                                                       * den geteilten KF-Pfad (Dossier D11) */
        e->state        = 1;                          /* +0x4 = 1 ACTIVE (@0x80112388)        */
        if (e->grid_id & 0x40) e->state = 4;          /* byte-true override (@0x80112408): the
                                                       * grid&0x40 event crow (0xe1) enters FLIGHT-2 */
        e->sub_state_1  = 0; e->sub_state_2 = 0;      /* INIT leaves +0x5/+0x6 = 0 (patrol/ascend) */
        break;
    }

    case 1: {  /* ACTIVE (0x80112420): sense -> flock-dispatch -> steer -> move -> tail -> post-pass */
        /* +0x1d2 = FUN_8001bc08() & 1 (@0x80112428 jal; @0x8011243c/0x80112454-58 andi 1 + sb):
         * the amortized 16-tick LOS SENSOR — 1 ONLY on the phase-3 tick whose 4 staged probes
         * (FOV cone ±0x5e8 vs facing + collision ray 0x8003dcc4(0xf00,0x300)) all passed, which
         * also snapshots the player pos to +0x1bc/+0x1be. The old `crow_parity ^= 1` frame toggle
         * was invented — it opened the steer[9] attack-commit every other frame regardless of
         * facing/occlusion (audit wf_827f186d crow #3). Shares the byte-true probe the zombie
         * root uses (re15_enemy_los_probe = the same FUN_8001bc08). */
        e->crow_parity   = (uint8_t)(re15_enemy_los_probe(slot, e, player) & 1);
        e->crow_dist     = (uint16_t)re15_enemy_player_dist(e, player); /* +0x1dc SquareRoot0, `sh 476`
                                                                         * @0x801124ac; ALL consumers lhu
                                                                         * (audit crow #14) */
        e->crow_vert_err = (int16_t)(player->y - e->y);                 /* +0x1ec (@0x801124cc) */
        if (s_crow_flock & 0xff00) re15_crow_flock_dispatch(e);        /* 0x80116068 */
        int32_t crow_ox = e->x, crow_oz = e->z;                        /* pre-move pos for the wall sweep */
        re15_crow_steer(e, player);                                    /* steer[+0x5] */
        re15_crow_move(e, player);                                     /* move[+0x5]  */
        /* --- ACTIVE tail (@0x80112560-0x80112614, audit wf_827f186d crow #15) --- */
        /* OPEN (presentation-side): the vert-band proximity flags on the entity[0] WORD —
         * `entity[0] &= 0x1fffffff` @0x80112560-70, then vert-err>=4001 -> jal 0x80012a0c(0x1770)
         * (sets entity[0]|=0x80000000 when player-dist<6000 @0x80012a6c/84), <800 -> jal
         * 0x80012974(0x1770) (|=0x20000000 @0x800129d4/ec), else |=0x40000000 @0x801125bc-c8.
         * The port models only the LOW byte of +0x00 (re15_actor.flags) and has no EXE
         * render/audio consumer of the top bits — not faked. Likewise OPEN: jal 0x80115f70
         * @0x801125cc (altitude presentation helper: +0x9a = vert-err>=5200 ? -1 : 0 targetability
         * latch @0x80115f88-9c, shadow scale +0xbc/+0xbe = clamp((y-floor)>>4+400, min 100)
         * @0x80115fa0-e4, shadow tint from (y-floor)>>5+128 @0x80115fe8-...) — render-side
         * shadow-pool fields the port draws from its own shadow path. */
        if ((s_crow_flock & 0x800) && e->crow_armed == 0) {   /* re-arm one-shot @0x801125dc-614 */
            s_crow_flock = (uint16_t)(s_crow_flock & 0xf0ff); /* @0x80112608-0c */
            e->crow_armed = 1;                                /* @0x80112610-14 */
        }
        /* ELEVATION-BAND-Stempel (@0x80112560-c8: word0 &= 0x1fffffff, dann vert>=4001 ->
         * jal 0x80012a0c(0x1770) = UP-Bit 0x80000000 NUR wenn player-dist<6000; vert<800 ->
         * jal 0x80012974(0x1770) = DOWN-Bit 0x20000000; sonst LEVEL 0x40000000) — der
         * Schuss-Resolver trifft die Kraehe nur im passenden Aim-Band (vorher traf der Port
         * pauschal mit LEVEL). crow_shot_attack.md F5 (CONFIRMED). */
        if (e->crow_vert_err >= 4001)    e->aim_band = (uint8_t)((e->crow_dist < 6000) ? 4 : 0);
        else if (e->crow_vert_err < 800) e->aim_band = 1;
        else                             e->aim_band = 2;
        /* +0x9a-LATCH (Tail-Helper 0x80115f70, jal @0x801125cc): hp = vert>=5200 ? -1 : 0
         * (@0x80115f88-9c) — die hohe Kraehe ist kein Waffen-/Auto-Aim-Ziel (Resolver und
         * FUN_8003703c uebergehen hp<0; 703c-Fallback-Bucket dokumentiert offen).
         * crow_shot_attack.md F4 (CONFIRMED). */
        e->hp = (int16_t)((e->crow_vert_err >= 5200) ? -1 : 0);
        /* ROOT post-pass body-push (aec4 @0x801121d4): crow pushed out of the player;
         * +0x1d0 = contact (the strike/grab connect the handlers read next tick). */
        e->crow_contact = (uint8_t)(re15_body_push(player, RE15_BODY_R_PLAYER, e,
                                                   (int32_t)e->hit_radius_min) ? 1 : 0);
        /* ROOT post-pass WALL PASS (@0x801121f8-218): +0x1d1 = 0x8003b0a4(&+0x34, a1=dim[3]=200
         * (lhu +0x78[6] @0x80112200), a2=4) — the SCA wall clamp; nonzero return = wall contact,
         * consumed by the steer[11] dive-abort @0x8011377c-90. Port = the standard enemy wall
         * clamp (re15_collision_constrain_enemy = the same 0x8003b0a4 pass, mask 4).
         * (audit wf_827f186d crow #B, raw-disasm CONFIRMED) */
        if (g_room_rdt_ok) {
            int32_t nx = e->x, nz = e->z;
            re15_collision_constrain_enemy(&g_room_rdt, crow_ox, crow_oz, &nx, &nz, 200, e->y);
            e->crow_wall = (uint8_t)((nx != e->x || nz != e->z) ? 1 : 0);
            e->x = nx; e->z = nz;
        } else e->crow_wall = 0;
        /* GRAB-HOLD pins the player (byte-true move[12] @0x80113e48: 0x800aca58 = cmd 5 grabbed).
         * s_player_grabbed (cleared at the top of run_all) latches the pin for sub 13 = game_step
         * skips re15_player_tick, exactly like the zombie grab. */
        if (e->sub_state_1 == 13) s_player_grabbed = 1;
        break;
    }

    case 4: {  /* FLIGHT-2 (0x80114e54): preamble -> 0x1d-tick -> substate[+0x5] (the event crow) */
        e->crow_dist     = (uint16_t)re15_enemy_player_dist(e, player); /* +0x1dc (sh @0x80114f1c) */
        e->crow_vert_err = (int16_t)(player->y - e->y);                 /* +0x1ec (@0x80114f40) */
        if ((s_crow_gflags & 0x4u) && e->sub_state_1 != 3)             /* testbit 0x1d (@0x80114f34) */
            re15_crow_sub(e, 3);                                        /* -> SPIN */
        re15_crow_flight2_sub(e, player);
        e->crow_contact = (uint8_t)(re15_body_push(player, RE15_BODY_R_PLAYER, e,
                                                   (int32_t)e->hit_radius_min) ? 1 : 0);
        if (e->sub_state_1 == 2) s_player_grabbed = 1;                  /* ORIENT = holding the grabbed player */
        break;
    }

    case 3:    /* DEATH (0x801146d0): fall from the sky + land + gib -> state 7. */
        re15_crow_death(e);
        break;

    case 7:    /* SPECIAL / CORPSE (0x801157e8): the dead crow settles + fades. */
        re15_crow_special(e);
        break;

    default:   /* states 2/5/6 = HURT (byte-true empty stub, no reaction) — hold */
        break;
    }
}

/* ============================================================================
 * DOG (Cerberus, type 0x20) — ground chase/lunge/bite AI. Byte-true port of the
 * 0x8010d7f8 family (RE15_DOG_AI.md; wave 1 wf_ccc60f69, full re-audit wave
 * wf_827f186d: 15 findings fixed against raw STAGE1.BIN disasm + the STAGE1_full
 * decompiles — every constant below carries its @0x).
 * Root FUN_8010d7f8 dispatches +0x4 via @0x80120f74: [0]INIT(0x8010d93c)
 * [1]ACTIVE(0x8010dbcc) [2]HURT(0x801108f0) [3]DEATH(0x80110dc0)
 * [4/5/6]SCRIPTED(0x80111350) [7]CORPSE(0x80111774).
 * ACTIVE is a dual dispatch on +0x5: DECISION table @0x80120f94 (real entries:
 * [0] idle 0x8010ddb8, [2] chase 0x8010e0c4, [3] attack-range 0x8010e568 — the
 * rest are jr-ra stubs) THEN ACT table @0x80120fd4 (the act runs on the sub the
 * decision just wrote — same frame). Subs: 0 idle / 1 turn / 2 chase / 3
 * attack-range circle / 4 stationary bark chain / 5 arm-grab windup / 6 snap /
 * 7 low-HP hop / 8 bite leap machine / 9-10 eaten-GRAB / 0xb died-in-grab /
 * 0xc release / 13-14 obstacle reroute.
 * The dog shares the zombie take_damage receiving side (states 2/3/7); the
 * HURT/DEATH dispatchers re-dispatch on +0x5 = the weapon/reaction row
 * (@0x80121018 / @0x80121070, 22 rows: 7-11/13-18/21 = the AIRBORNE variants).
 * OPEN (documented, not faked):
 *  - FUN_80012974(4000) in the root tail (@0x8010dd48) sets entity word0 flag
 *    0x20000000 (proximity <4000, sltu @0x800129cc + or @0x800129ec) — the port
 *    does not model the entity word0 flag bits (engine-wide flag word).
 *  - +0x1c0 = 0x8000 hit-tween latch (hurt entries @0x80110a80/@0x80110c3c) —
 *    no +0x1c0 reader/writer is modeled port-wide (also open for the zombie).
 *  - FUN_80111870 skeletal root-motion assist (chase @0x8010e550, sub-7 hop
 *    @0x8010f118): bone-matrix delta displacement on top of func_0x800245d8 —
 *    the port's enemy walker is the 245d8 component only.
 *  - reroute-13 aux +0xb0=0xc1c/0 (@0x80110440/@0x801104d0) — shadow-block aux
 *    field of the af5c shadow descriptor, not modeled.
 *  - entity word0 |= 2|0x40 at corpse entry (@0x801117cc/e8) — flag word, see
 *    above.
 *  - FUN_80037edc(id, 0x32) SE-tier accumulator (knockdown grunts @0x8010f360):
 *    the port plays the tier's SE id directly (same stance as the crow screech).
 */
static const uint8_t s_dog_clip_len[28] =   /* EM020 clip frame-counts, byte-true (CDEMD0.EMS idx 7) */
    { 49,18,30,14,14,14,18,15,30,35,35,14,42,14,44,14,27,14,50,29,17,13,20,29,18,40,30,99 };

static void re15_dog_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static void re15_dog_sub (re15_actor_t *e, uint8_t s) { e->sub_state_1 = s; e->sub_state_2 = 0; }
static int  re15_dog_anim(re15_actor_t *e)
{
    int fc = (e->motion < 28) ? s_dog_clip_len[e->motion] : 1; if (fc <= 0) fc = 1;
    e->anim_frame++; if (e->anim_frac > 0) e->anim_frac--;
    if ((int)e->anim_frame >= fc) { e->anim_frame = 0; return 1; }
    return 0;
}
static void re15_dog_advance(re15_actor_t *e, int32_t sp)   /* pos_advance along yaw (0x800245d8) */
{
    e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * sp) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * sp) >> 12);
}
/* func_0x800245d8(a0) — the BYTE-TRUE walker: the movement SPEED comes from entity+0x8c
 * (`lhu v0,140(v0)` @0x800245f0) and a0 is a YAW OFFSET added to rot_y before RotMatrixY
 * (`lh v0,106(v0); addu a0,v0,a0` @0x80024658-64). a0=0 forward, a0=0x800 = 180° rearward
 * (the same convention as the player knockback @0x80035f18). (audit wf_827f186d dog #6/#10) */
static void re15_dog_advance_ofs(re15_actor_t *e, int32_t yaw_ofs)
{
    int a = ((int)e->rot_y + (int)yaw_ofs) & 0xfff;
    e->x += (int32_t)(((int32_t)re15_cos_q12((int16_t)a) * e->crow_speed) >> 12);
    e->z -= (int32_t)(((int32_t)re15_sin_q12((int16_t)a) * e->crow_speed) >> 12);
}
/* arc-range test 0x8001a804: player within dist_thresh AND facing within yaw_tol (Q12). */
static int re15_dog_arc(const re15_actor_t *e, const re15_actor_t *pl, int dth, int ytol)
{
    /* range: compute the dist FRESH as int32 — the byte-true range checks (0x8001a804/0x8001a9cc)
     * recompute the distance internally, so they never overflow. The cached +0x1d4 (dog_dist) is s16
     * and WRAPS NEGATIVE past 32767 (live bug: a maggot in ROOM11C0 at ~45000 wrapped to a "close"
     * negative and bit the air). Use the int32 dist here to match the original. */
    if ((int32_t)re15_enemy_player_dist(e, pl) >= dth) return 0;
    int fb = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
    int df = (((fb - (int)e->rot_y) + 0x800) & 0xfff) - 0x800;
    return (df >= -ytol && df <= ytol);
}
/* FUN_80111f0c(step, cap): ramp entity+0x8c UP toward cap (@0x80111f28-5c). */
static void re15_dog_speed_ramp_up(re15_actor_t *e, int16_t step, int16_t cap)
{
    if (e->crow_speed < cap) {
        e->crow_speed = (int16_t)(e->crow_speed + step);
        if (e->crow_speed > cap) e->crow_speed = cap;
    }
}
/* FUN_80111f6c(step, floor): ramp entity+0x8c DOWN toward floor. */
static void re15_dog_speed_ramp_down(re15_actor_t *e, int16_t step, int16_t flo)
{
    if (e->crow_speed > flo) {
        e->crow_speed = (int16_t)(e->crow_speed - step);
        if (e->crow_speed < flo) e->crow_speed = flo;
    }
}
/* FUN_80111fcc(turn): menace clip select from the 3-byte table @0x801210f8 = {5,3,4}
 * (raw dump: 05 03 04): turn<0 -> clip 5, ==0 -> clip 3 (straight), >0 -> clip 4. Writes
 * +0x94 ONLY when different — NO frame reset (@0x80111ff0-80112008). */
static void re15_dog_clip_keep(re15_actor_t *e, int turn)
{
    uint8_t c = (turn < 0) ? 5 : (turn == 0) ? 3 : 4;
    if (e->motion != c) e->motion = c;
}
/* +0x1da — the per-frame SCA wall-probe result the dog ROOT stores (FUN_8010d7f8 @0x8010d834:
 * `+0x1da = FUN_8003b0a4(&+0x34, dim, 4)`, nonzero on wall contact). Port: the run_all wall
 * clamp encodes the same contact into +0x90 (ai_contact bit0 + heading nibble), so "blocked
 * last frame" = the +0x90 contact bits. (audit wf_827f186d dog #15) */
static int re15_dog_blocked(const re15_actor_t *e) { return (e->ai_contact & 3) != 0; }
/* func_0x80019700(0x2000, rot_y, bone-block, &DAT_80120f54) — the universal hit-blood burst
 * (hurt @0x80110aa0 bone+0xec, airborne @0x80110c88 bone+0x40, bark-chain death @0x8010ebe0
 * bone+0x2f0). Port: the same effect-0 spawn the zombie hurt fx uses (re15_damage.c
 * re15_enemy_hurt_fx); position = actor world pos (bone offset = faithful-line). */
static void re15_dog_blood(re15_actor_t *e)
{
    re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0, e->x, e->y, e->z, (int16_t)e->rot_y);
}
/* DAT_800aca50 bit 0 — the dog PACK-ALERT bit: cleared by the dog INIT (`sh zero,0x800aca50`
 * @0x8010db84), set by the bark step (@0x8010eb58 `ori v0,v0,1`), cleared again at bark step 4
 * (@0x8010eb78-7c `andi 0xfffe`), read by the idle decision (@0x8010de60-84) and the chase
 * decision (@0x8010e21c-240). NOTE: on the PSX this is the SAME global halfword the zombie
 * throw-off mercy latch sets (port: s_grab_mercy_timer) — no shipped STAGE1 room mixes dogs
 * with zombies, so the port keeps the dog's bit separate. (audit wf_827f186d dog #7) */
static uint16_t s_dog_pack_alert;
/* run_all clamp bypass for the reroute-13 leap burst: the original snaps the OLD-POS mirror
 * +0x40/+0x42/+0x44 to the new position right after the func_0x800245d8(0) burst
 * (@0x801104e0-0x8011052c) so the SCA sweep does NOT clamp the hop back off the obstacle.
 * The port's equivalent sweep is the run_all constrain from the tick-start pos — this mask
 * skips it for the burst frame. */
static uint32_t s_dog_noclamp_mask;
static int re15_dog_consume_noclamp(int slot)
{
    uint32_t b = 1u << (unsigned)slot;
    int r = (s_dog_noclamp_mask & b) != 0;
    s_dog_noclamp_mask &= ~b;
    return r;
}

/* DAT_800aca58 — the PLAYER's +0x4 state:sub halfword (player block @0x800aca54). The dog
 * wake/menace triggers test two state-1 sub-modes (audit wf_827f186d dog #7):
 *  0x0201 = state 1 sub 2 = RUNNING — proven: the sub-2 handler LAB_80032604 loads the run
 *    speed `lbu DAT_80073f24[..]` = 0xC8 (200) @0x800327e8 into the player speed word
 *    (`sh a0,DAT_800acae0` @0x80032830) and selects clip 0 (`sb zero,DAT_800acae8`
 *    @0x8003280c) — the RUN clip (port: W01 c0 = motion 100, game_step footstep block).
 *  0x0701 = state 1 sub 7 = WEAPON-DRAWN/AIM — proven: stage-A [7] @0x80032e3c is `jr ra`
 *    and stage-B [7] @0x80032e44 dispatches the weapon-pose table @0x80074030[aca5d]
 *    (the aim FSM; port: re15_player_aim_active).
 * The port player FSM does not mirror the aca58 word — these predicates read the port's own
 * markers of exactly those two states. */
static int re15_dog_player_running(void)
{
    return g_actors[RE15_ACTOR_SLOT_PLAYER].motion == 100;
}
static int re15_dog_player_aiming(void)
{
    extern int re15_player_aim_active(void);
    return re15_player_aim_active();
}

/* The SCA cell the dog stands on — the original keeps a live pointer at entity+0x1b4 (floor
 * resolver); the reroute commit latches its attr halfword `*(u16*)(cell+10)` (@0x8010e294-2a4).
 * Port: point-in-rect scan on the dog's band (same unsigned idiom as
 * re15_collision_floor_band_at); cell+10 = the aliased entry's u1|floor<<8 bytes. */
static uint16_t re15_dog_sca_attr(const re15_actor_t *e)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca) return 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((c->floor >> 4) != e->floor) continue;
        if ((uint32_t)(e->x - (int32_t)c->x) < (uint32_t)c->width &&
            (uint32_t)(e->z - (int32_t)c->z) < (uint32_t)c->density)
            return (uint16_t)((uint16_t)c->u1 | ((uint16_t)c->floor << 8));
    }
    return 0;
}

/* Shared reroute commit — chase decision @0x8010e254-2ec AND attack-range decision else-branch
 * @0x8010e628-6c0 (identical gate; audit wf_827f186d dog #12):
 *   needs +0x90 contact bits 0-1 (`andi +0x90,3` @0x8010e268) AND the escape heading within
 *   ±0x200 of the current yaw (`(((+0x90&0xf0)<<4) - rot_y + 0x200) & 0xfff < 0x400`
 *   @0x8010e274-28c); latches +0x1ea from the SCA cell (@0x8010e294-2a4) and +0x1e8 = +0x90&1
 *   (@0x8010e2bc-c0); picks sub 13 (bit0=1) or 14 (bit0=0): `0xe - +0x1e8` @0x8010e2d4-dc,
 *   `sh zero -> +0x6/+0x7`. OPEN: the +0x90 bit-0/1 contact CLASS comes from the SCA resolver
 *   FUN_8003b0a4 (leap-over vs drop-edge, not RE'd); the port clamp writes bit 0 only, so the
 *   port reaches sub 13 (13 = the byte-true bit0=1 selection). */
static int re15_dog_try_reroute(re15_actor_t *e)
{
    if ((e->ai_contact & 3) == 0) return 0;                              /* @0x8010e268 */
    int heading = ((int)(e->ai_contact & 0xf0)) << 4;
    if ((((heading - (int)(int16_t)e->rot_y) + 0x200) & 0xfff) >= 0x400) /* @0x8010e274-28c */
        return 0;
    e->dog_reroute_sca = re15_dog_sca_attr(e);                           /* +0x1ea @0x8010e294-2a4 */
    e->dog_reroute_dir = (uint16_t)(e->ai_contact & 1);                  /* +0x1e8 @0x8010e2bc-c0 */
    e->sub_state_1 = (uint8_t)(0x0e - e->dog_reroute_dir);               /* 13/14  @0x8010e2d4-dc */
    e->sub_state_2 = 0; e->sub_state_3 = 0;                              /* sh zero,+0x6 */
    return 1;
}

/* ============================ DOG OBSTACLE-REROUTE (ACTIVE sub 13/14) ======================== *
 * Byte-true 0x801102dc (sub 13, leap OVER — UP one floor level) + 0x801105bc (sub 14, drop DOWN
 * 1-4 levels). +0x82 is the FLOOR INDEX (not a counter) and both hops move y/+0x1ba by ±0x708
 * per level (audit wf_827f186d dog #13). The B[13]/B[14] decision slots are jr-ra stubs. */
static void re15_dog_reroute13(re15_actor_t *e, int slot)   /* FUN_801102dc */
{
    int heading = (((int)e->ai_contact & 0xf0) << 4) & 0xfff;   /* +0x90 hi-nibble, read FRESH (no latch in 13) */
    switch (e->sub_state_2) {
    case 0:                                             /* INIT 0x8011031c */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 7;     /* +0x94=0,+0x95=0,+0x8f=7 */
        e->sub_state_2 = 1;
        /* FALLTHROUGH to step 1 (byte-true @0x80110358 -> 0x8011035c) */
        /* fall through */
    case 1: {                                           /* TURN 0x8011035c: slew to escape heading */
        re15_dog_anim(e);                               /* anim_set clip 0 (advance) @0x80110370 */
        int delta = (16 + heading - (int)e->rot_y) & 0xfff;     /* 0x8001aa68(target, step 16) */
        int inc   = (delta < 32) ? 0 : (delta < 0x801 ? 16 : -16);
        e->rot_y  = (uint16_t)(((int)e->rot_y + inc) & 0xfff);  /* +0x6a += inc (always) @0x801103b8 */
        if (inc != 0) break;                            /* still turning -> stay step 1 @0x801103b4 */
        e->sub_state_2 = 2;                             /* aligned -> step 2 @0x801103cc */
        break;
    }
    case 2:                                             /* COMMIT 0x801103d0 */
        e->motion = 9; e->anim_frame = 0;               /* +0x94=9 (run), +0x95=0 @0x801103ec */
        e->rot_y  = (uint16_t)heading;                  /* snap yaw = (wall&0xf0)<<4 @0x8011041c */
        e->sub_state_2 = 3;
        /* FALLTHROUGH to step 3 (byte-true @0x8011041c -> 0x80110420) */
        /* fall through */
    case 3:                                             /* RUN/LEAP forward 0x80110420 */
        if (e->anim_frame == 0x12)                      /* frame 0x12: pre-arm the level hop @0x8011042c */
            e->dog_floor_y = (int16_t)(e->dog_floor_y - 0x708);  /* +0x1ba -= 1800 @0x80110458 (+0xb0=0xc1c aux OPEN) */
        if (re15_dog_anim(e)) {                         /* run clip 9 done @0x8011047c */
            e->floor++;                                 /* +0x82 += 1 (FLOOR INDEX up) @0x80110498 */
            e->y -= 0x708;                              /* +0x38 -= 1800 (one level UP) @0x801104b4 */
            e->crow_speed = 0xc1c;                      /* +0x8c = 3100 @0x801104c8 */
            re15_dog_advance_ofs(e, 0);                 /* func_0x800245d8(0) burst @0x801104d4 */
            s_dog_noclamp_mask |= 1u << (unsigned)slot; /* old-pos snap +0x40/+0x42/+0x44 @0x801104e0-52c */
            e->sub_state_2 = 4;                         /* -> step 4 @0x80110534 */
        }
        break;
    default:                                            /* RESET -> CHASE 0x80110538 (step 4) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;     /* +0x94=0,+0x95=0,+0x8f=0 */
        re15_dog_anim(e);                               /* anim_set @0x8011057c */
        e->sub_state_1 = 2; e->sub_state_2 = 0;         /* +0x5=2 CHASE, +0x6=0 @0x80110590 */
        e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);/* consume the contact bit (port clamp re-arms) */
        break;
    }
}

static void re15_dog_reroute14(re15_actor_t *e)   /* FUN_801105bc, clip 0x0a, drop 1-4 levels */
{
    switch (e->sub_state_2) {
    case 0:                                             /* INIT 0x80110610 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 7;     /* +0x94=0,+0x95=0,+0x8f=7 */
        e->dog_aux9f = (int8_t)e->ai_contact;           /* +0x9f = +0x90 LATCH @0x80110658 */
        e->sub_state_2 = 1;
        /* FALLTHROUGH to phase 1 (byte-true @0x80110658 -> 0x8011065c) */
        /* fall through */
    case 1: {                                           /* TURN-SLEW 0x8011065c */
        re15_dog_anim(e);                               /* anim_set clip 0 @0x80110674 */
        int heading = (((int)e->dog_aux9f & 0xf0) << 4) & 0xfff;   /* aa68(((s8)+0x9f&0xf0)<<4,0x10) */
        int delta = (16 + heading - (int)e->rot_y) & 0xfff;
        int inc   = (delta < 32) ? 0 : (delta < 0x801 ? 16 : -16);
        e->rot_y  = (uint16_t)(((int)e->rot_y + inc) & 0xfff);  /* +0x6a += inc @0x801106b8 */
        if (inc != 0) break;                            /* still turning @0x801106b4 */
        e->sub_state_2 = 2;                             /* aligned -> phase 2 @0x801106cc */
        break;
    }
    case 2:                                             /* SNAP + reroute clip 0x0a 0x801106d0 */
        e->motion = 0x0a; e->anim_frame = 0; e->anim_frac = 0;  /* +0x94=0x0a,+0x95=0,+0x8f=0 */
        e->rot_y  = (uint16_t)((((int)e->dog_aux9f & 0xf0) << 4) & 0xfff);  /* hard-snap @0x80110714 */
        e->sub_state_2 = 3;
        /* FALLTHROUGH to phase 3 (byte-true @0x80110724 -> 0x80110728) */
        /* fall through */
    default:                                            /* RUN + DROP -> CHASE 0x80110728 (phase 3) */
        if (e->anim_frame == 0x0a) {                    /* frame 10 @0x80110734 */
            e->anim_frac  = 7;                          /* +0x8f=7 @0x80110744 */
            e->anim_frame = 0x0e;                       /* frame := 14 (skip 10-13) @0x80110754 */
            {   /* drop ((+0x1ea & 0xc) >> 2) + 1 levels (do-while @0x80110774-b0) */
                int n = (int)((e->dog_reroute_sca & 0xc) >> 2);
                for (int i = 0; i <= n; i++) {
                    e->dog_floor_y = (int16_t)(e->dog_floor_y + 0x708);  /* +0x1ba += 1800 */
                    e->floor--;                                          /* +0x82 -= 1 */
                }
            }
            e->crow_speed = 0x509;                      /* +0x8c = 1289 launch burst @0x801107c8 */
            re15_dog_advance_ofs(e, 0);                 /* func_0x800245d8(0) @0x801107d4 */
            e->y += 0x3f8;                              /* +0x38 += 1016 @0x801107e0 */
            e->ai_timer = (int16_t)(e->dog_floor_y - e->y);   /* +0x9c = floor - y @0x80110804 */
        }
        if ((uint32_t)(e->anim_frame - 0x0e) < 6u) {    /* frames 14..19 @0x80110818 */
            e->y += (e->ai_timer + (e->ai_timer < 0 ? 7 : 0)) >> 3;   /* y += +0x9c/8 @0x80110834 */
            e->crow_speed = 300;                        /* +0x8c = 300 @0x80110848 */
            re15_dog_advance_ofs(e, 0);                 /* @0x80110858 */
        }
        if (e->anim_frame == 0x14)                      /* frame 20: floor snap @0x80110878-88 */
            e->y = e->dog_floor_y;
        if (re15_dog_anim(e)) {                         /* reroute clip 0x0a done @0x801108a8 */
            e->sub_state_1 = 2; e->sub_state_2 = 0;     /* +0x5=2 CHASE, +0x6=0 @0x801108bc */
            e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);
        }
        break;
    }
}

/* ============================ DOG eaten-GRAB hold (ACTIVE sub 9 / sub 10) ==================== *
 * Byte-true 7-step FSM @0x8010f80c (sub 9 GRAB) / @0x8010fc60 (sub 10 GRABVAR). The dog latches onto
 * the player and plays the eat cadence: clip 0x17/0x1a (latch) -> 0x18 (eat/shake) -> 0x19 (release).
 * The PLAYER is pinned + animated by the shared victim FSM (aca58=5 = player cmd 5, the same command
 * the zombie grab issues; Leon animates from EM020's bank2). Struggle drain +0x9c (100, -= 1+100*mash);
 * feed countdown +0x9e (50). Exits: sub 0xb (player died / feed timeout -> devour) or sub 0xc (broke
 * free -> recover). Sub 9 = front variant (aca59=0, clip 0x17); sub 10 = behind (aca59=1, clip 0x1a).
 * Verified byte-for-byte identical except those two step-0 bytes. */
static void re15_dog_grabhold(re15_actor_t *e, re15_actor_t *pl)
{
    uint8_t variant    = (e->sub_state_1 == 10) ? 1 : 0;        /* aca59 @0x8010f928 / @0x8010fd7c */
    uint8_t latch_clip = variant ? 0x1a : 0x17;                /* step-0 clip @0x8010f86c / @0x8010fcc0 */
    switch (e->sub_state_2) {
    case 0:                                             /* LATCH 0x8010f84c / 0x8010fca0 */
        re15_audio_room_se(3);                          /* Se(3) latch/roar @0x8010f84c */
        e->sub_state_2 = 1;
        e->motion = latch_clip; e->anim_frame = 0; e->anim_frac = 7;   /* clip,+0x95=0,+0x8f=7 */
        e->anchor_x = pl->x; e->anchor_z = pl->z;       /* +0xa0/+0xa2 = player pos @0x8010f8b8 */
        s_player_grabbed = 1;                           /* aca58=5 pin (player cmd 5) @0x8010f910 */
        re15_player_victim_latch(e, pl);                /* animate Leon from the dog's victim bank */
        g_player_victim_variant = variant;              /* override to the dog's variant (0 front/1 behind) */
        pl->hit_react |= 1;                             /* player+0x93 |= 1 @0x8010f974 */
        e->rot_y = (uint16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* face player @0x8010f978 */
        /* FALLTHROUGH to step 1 */
        /* fall through */
    case 1:                                             /* run latch anim 0x8010f980 */
        s_player_grabbed = 1;
        if (re15_dog_anim(e)) e->sub_state_2 = 2;       /* clip done -> step 2 @0x8010f9d0 */
        break;
    case 2:                                             /* init eat loop 0x8010f9dc */
        e->motion = 0x18; e->anim_frame = 0;            /* eat/shake clip @0x8010f9e8 */
        e->ai_timer = 0x64;                             /* +0x9c = 100 struggle budget @0x8010fa08 */
        e->grab_kill_ctr = 0x32;                        /* +0x9e = 50 feed countdown @0x8010fa18 */
        e->sub_state_2 = 3;
        /* FALLTHROUGH to step 3 */
        /* fall through */
    case 3:                                             /* EAT loop 0x8010fa1c */
        s_player_grabbed = 1;
        re15_dog_anim(e);                               /* eat clip 0x18 (drag root-motion) */
        if (e->grab_kill_ctr == 0) { re15_dog_sub(e, 0xb); break; }   /* feed timeout -> died @0x8010fa70 */
        e->grab_kill_ctr--;                             /* +0x9e-- @0x8010fa6c */
        if (pl->hp < 0) { re15_dog_sub(e, 0xb); break; }              /* player died -> sub 0xb @0x8010fa84 */
        e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 100 * re15_mash_pressed()));  /* struggle drain @0x8010fa9c */
        if (e->ai_timer < 0) { e->sub_state_2 = 4; s_victim_phase = 4; }  /* broke free -> step 4 @0x8010faec */
        break;
    case 4:                                             /* init release 0x8010fb00 */
        e->motion = 0x19; e->anim_frame = 0; e->anim_frac = 7;   /* release clip @0x8010fb0c */
        e->sub_state_2 = 5;
        e->grab_kill_ctr = (uint8_t)((re15_engine_rand8() + 30) & 0x3f);  /* +0x9e rand @0x8010fb64 */
        /* FALLTHROUGH to step 5 */
        /* fall through */
    case 5:                                             /* run release anim 0x8010fb68 */
        s_player_grabbed = 1;
        if (re15_dog_anim(e)) { re15_audio_room_se(7); e->sub_state_2 = 6; }  /* clip done -> Se(7) @0x8010fbd4 */
        break;
    default:                                            /* cleanup 0x8010fbec (step 6) */
        if (e->grab_kill_ctr == 0) { re15_dog_sub(e, 0xc); break; }   /* -> release continuation @0x8010fc30 */
        e->grab_kill_ctr--;                             /* +0x9e-- @0x8010fc1c */
        break;
    }
}

/* STATE 4/5/6 (0x80111350) — the SCRIPTED dog states. Sub-dispatch on +0x5 (table @0x801210c8,
 * raw dump): [0]=0x80111398 scripted pounce (grid-0x40 spawn), [1]=0x80111658 window hold
 * (grid-0x41 spawn), [2]/[3]=0x80111764/6c stubs, [4/5]=0x80111984 kill machine A,
 * [10/11]=0x80111cf0 kill machine B. ENTERED ONLY from INIT (grid 0x40 -> +0x4=4, grid 0x41 ->
 * +0x4=0x104 @0x8010db88-b8) or the SCD — the ACTIVE lunge (sub 4) NEVER writes state 5 (its
 * @0x8010eb88-bf0 transition is the inner step +0x6=5, audit wf_827f186d dog #2/#14). */
static void re15_dog_state456(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {
    case 0:   /* FUN_80111398 — scripted pounce: WAIT for SCD grid 0x43, then leap + exit to chase */
        switch (e->sub_state_2) {
        case 0:
            if (e->grid_id != 0x43) break;              /* gate: `bne +0x9,0x43 -> jr ra` WAIT @0x801113e4-ec */
            e->hit_react |= 3;                          /* +0x93 |= 3 @0x801113f8 */
            e->sub_state_2 = 1;
            re15_dog_clip(e, 0x14); e->crow_speed = 0xf0;   /* clip 0x14, +0x8c=0xf0 @0x80111410-24 */
            re15_audio_room_se(5);                      /* Se(5): `ori a0,5` @0x8011142c + jal 453d0 @0x80111450 (audit #14) */
            /* FALLTHROUGH (decompile case 0 -> case 1) */
            /* fall through */
        case 1:                                         /* LEAP: rise + lunge forward @0x80111458 */
            if (re15_dog_anim(e)) e->sub_state_2 = 2;   /* f314 advance */
            if (e->anim_frame > 0x0c) {
                e->crow_speed = (int16_t)(e->crow_speed + 6);
                re15_dog_advance_ofs(e, 0);             /* 245d8(0) @0x801114cc */
                e->y -= 0x14;                           /* +0x38 -= 20 rise @0x801114dc */
                e->dog_floor_y = 0;                     /* +0x1ba = 0 @0x801114f0 */
            }
            break;
        case 2:
            e->sub_state_2 = 3;
            e->motion = 0x15; e->anim_frame = 5; e->anim_frac = 7;  /* land clip 0x15 FROM frame 5 @0x80111504-1c */
            e->ai_timer = 0;                            /* +0x9c = 0 @0x80111528 */
            /* fall through */
        case 3:                                         /* ballistic settle @0x80111530 */
            if (e->anim_frame < 2) {
                re15_dog_advance_ofs(e, 0);             /* 245d8(0) @0x80111548 */
            } else {
                /* FUN_8001c1a4(+0x8c, 0, -0x1e, +0x1ba): forward rotate (speed,0,0) by Ry(rot_y)
                 * (@0x8001c1f0/8001c20c) then y -= (a1 + a2*(+0x9c)) with +0x9c++ per call; lands
                 * (y >= floor -> clamp + return!=0) -> +0x6=4 (@0x80111570). */
                int32_t vy = 0 + (-0x1e) * (int32_t)e->ai_timer;
                re15_dog_advance_ofs(e, 0);
                e->y -= vy;
                if (e->y >= (int32_t)e->dog_floor_y) { e->y = e->dog_floor_y; e->sub_state_2 = 4; }
                else e->ai_timer++;
            }
            re15_dog_anim(e);                           /* f314 (return NOT phase-added) @0x801115a4 */
            if (e->anim_frame == 0) e->anim_frame = 5;  /* loop land clip from frame 5 @0x801115c4-d4 */
            break;
        case 4:                                         /* land commit @0x801115e0 */
            e->dog_atk_cd = 0x78;                       /* +0x1d6 = 0x78 @0x801115f0 */
            e->floor = 0;                               /* +0x82 = 0 @0x80111600 */
            e->hit_react = 0;                           /* +0x93 = 0 @0x80111610 */
            e->grid_id = 0;                             /* grid CONSUMED: `sb zero,9` @0x8011161c (audit #14) */
            e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* word 0x201 @0x8011162c */
            e->y = e->dog_floor_y;                      /* +0x38 = +0x1ba @0x80111644 */
            break;
        default: break;
        }
        (void)pl;
        break;

    case 1:   /* FUN_80111658 — window-hold (grid-0x41 spawn): clip-8 pose at +300, wait grid 0x42 */
        if (e->sub_state_2 == 0) {
            e->sub_state_2 = 1;
            e->motion = 8; e->anim_frame = 0; e->anim_frac = 0;   /* clip 8, +0x8f=0 @0x80111684-a4 */
            re15_dog_anim(e);                                     /* one f314 @0x801116c8 */
            /* +0xbc/+0xbe += 300 (@0x801116e8-fc): the shadow/pool descriptor offset pair (the
             * dog sits raised in the kennel window) — the +0xb0 shadow block is not modeled (OPEN). */
        } else if (e->sub_state_2 != 1) break;
        if (e->grid_id == 0x42) {                        /* SCD release mark 'B' @0x8011170c-14 */
            e->grid_id = 0;                              /* consumed @0x80111718 */
            e->state = 1; e->sub_state_1 = 0x0c; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* word 0xc01 @0x8011172c */
            /* +0xbc/+0xbe -= 300 (@0x8011173c-50) — shadow pair, see above */
        }
        break;

    case 4: case 5:    /* MACHINE A (0x80111984) — the pounce "eaten by dog" player-kill (9 phases) */
    case 10: case 11:  /* MACHINE B (0x80111cf0) — the eat/finish variant (5 phases) */
        /* Byte-true: both machines animate the PLAYER (not the dog) through the eaten cadence via the
         * player's victim-bank pointers (= the dog's bank2, bound at grab-start) indexed by s_victim_phase
         * (aca5a), spawn the blood/gore fx (machine A phase 4 fx 0x1500 @0x80111b58; machine B phase 1
         * fx 0x2000 + gore @0x80111ddc), and KILL him (machine B phase 2: aca58 = player STATE 7
         * @0x80111ea0). Port: drive the shared victim FSM (Leon animates from EM020 bank2 — its 15-bone
         * PL00 player set, via the byte-true dog clip map in re15_victim_clip_map — collapses and dies
         * via re15_player_victim_devour) + the blood burst; the dog holds its land pose. The death
         * presentation (white flash / YOU DIED / death camera / fade / game-over) is the ported
         * re15_gameover_fsm_tick, which this player death drives. VERIFIED COMPLETE (static RE + the
         * live mzd_death_cmd5/6/7 savestates): the acb10/acb12 that machine B writes are NOT a camera
         * pan — they are the BLOOD-POOL half-extents (live base 500/600, +12/frame, terminal 1964/2064),
         * already ported as g_death_pool. And aca3c|=0xc0 (set through the whole eaten death, save-
         * confirmed 0xc0 in cmd5/6/7) only gates PSX-hardware display re-inits (@0x8001cd04 framebuffer
         * swap / @0x800214e8 fade) that the PC renderer has no equivalent of = a no-op here. So there is
         * no unported dog-death behaviour; only the shared game-over CAMERA fade infra is engine-wide. */
        s_player_grabbed = 1;
        if (e->sub_state_2 == 0) {                        /* setup phase @0x80111b04 / @0x80111d28 */
            re15_player_victim_latch(e, pl);              /* pin + animate Leon from the dog's victim bank */
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000 /* Original-a0 (D6) */,
                              pl->x, pl->y, pl->z, (int16_t)pl->rot_y);   /* eaten blood burst */
            e->sub_state_2 = 1;
        } else if (e->sub_state_2 == 1 && s_victim_phase >= 2) {   /* eaten past the shake phases -> KILL */
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x2000 /* Original-a0 (D6) */,
                              pl->x, pl->y, pl->z, (int16_t)pl->rot_y);   /* gore burst @kill */
            re15_player_victim_devour(e);                 /* collapse -> player STATE 7 (aca58=7 @0x80111ea0) */
            e->sub_state_2 = 2;
        }
        re15_dog_anim(e);                                 /* the dog holds its pounce-land pose */
        break;

    default:  /* [2]/[3] = 0x80111764/0x8011176c stubs; [6..9] = NULL rows — inert WAIT
               * (the old "-> chase" fallback was an invented route, audit wf_827f186d dog #14) */
        break;
    }
}

/* HURT/DEATH shared bits (audit wf_827f186d dog #4/#5 + the verified grounded-death detour):
 * both dispatchers FUN_801108f0 / FUN_80110dc0 run FALL PHYSICS first (airborne = y above
 * +0x1ba: on the first frame (+0x7==0) FORCE the row +0x5=7 and seed +0x9c=floor-y
 * (@0x80110928-2c / @0x80110df8-dfc), then y += +0x9c/12 per frame (0x2aaaaaab magic);
 * grounded clamps y=floor), then dispatch table[+0x5]. Rows 7-11/13-18/21 = the AIRBORNE
 * handlers (raw dumps @0x80121018 / @0x80121070). */
static int re15_dog_row_airborne(int row)
{
    return (row >= 7 && row <= 11) || (row >= 13 && row <= 18) || row == 21;
}
static void re15_dog_fall_physics(re15_actor_t *e)
{
    if (e->y < (int32_t)e->dog_floor_y) {                 /* airborne (y above floor) */
        if (e->sub_state_3 == 0) {
            e->sub_state_1 = 7;                           /* force row 7 @0x80110928-2c */
            e->ai_timer = (int16_t)(e->dog_floor_y - e->y);   /* +0x9c seed */
        }
        e->y += (int32_t)(e->ai_timer / 12);              /* y catch-up /12 @0x80110948-98 */
    } else {
        e->y = e->dog_floor_y;                            /* grounded clamp */
    }
}

static void re15_dog_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT FUN_8010d93c: idle pose, seed steer/HP/timers, grid-scripted start */
        re15_dog_clip(e, 1); e->anim_frac = 0;                /* +0x94=1,+0x95=0,+0x96=0,+0x8f=0 @0x8010d954-98 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be @0x8010d9a8 */
        e->ai_timer = 0; e->grab_kill_ctr = 0; e->dog_aux9f = 0;   /* +0x9c/+0x9e/+0x9f = 0 @0x8010da34-58 */
        e->dog_atk_cd = 0; e->dog_pounce_cd = 0; e->dog_flags = 0; /* +0x1d6/+0x1e6/+0x1d0 = 0 @0x8010db18-7c */
        e->dog_blocked_ctr = 0; e->dog_grab_armed = 0;             /* +0x1dc/+0x1e4 = 0 @0x8010db44-6c */
        e->dog_floor_y = (int16_t)e->y;                            /* +0x1ba floor = spawn ground (port infra) */
        {   /* BYTE-TRUE HP (dog INIT @0x8010dae8-db0c): +0x9a = HPtable[type*0x20 + (rng&0xf)*2], the
             * SAME shared table @0x8011f034 as the zombie/maggot. Dog row @0x8011f434 (type 0x20). */
            static const uint16_t dog_hp[16] =
                { 65, 85, 85, 69,105, 87, 73, 87,107, 77,111, 80, 93, 83, 97,103 };
            if (e->hp <= 0) e->hp = (int16_t)dog_hp[re15_engine_rand8() & 0xf];
        }
        s_dog_pack_alert = 0;                                 /* `sh zero,0x800aca50` @0x8010db84 (audit #1) */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4=1 @0x8010d950 */
        if (e->grid_id == 0x40) {                             /* scripted pounce dog @0x8010db88-98 */
            e->state = 4; e->sub_state_1 = 0;                 /* word +0x4 = 4 (audit #1) */
        } else if (e->grid_id == 0x41) {                      /* window-hold dog @0x8010dba8-b8 */
            e->state = 4; e->sub_state_1 = 1;                 /* word +0x4 = 0x104 (audit #1) */
        }
        break;

    case 1: {  /* ACTIVE FUN_8010dbcc: perception -> DECISION[+0x5] -> ACT[+0x5] -> root tail */
        int los = re15_enemy_los_probe(slot, e, pl);          /* 0x8001bc08 @0x8010dbd4 */
        if (los != 2) { if (los) e->dog_flags |= 1; else e->dog_flags = (uint16_t)(e->dog_flags & ~1u); }  /* +0x1d0 bit0 sticky LOS */
        e->dog_dist = (int16_t)re15_enemy_player_dist(e, pl); /* +0x1d4 (SquareRoot0) @0x8010dc8c */

        /* ---- DECISION pass (@0x80120f94[+0x5]; only [0]/[2]/[3] are real) ---- */
        switch (e->sub_state_1) {
        case 0:   /* IDLE decision FUN_8010ddb8 (raw @0x8010ddb8-de98) */
            if (e->dog_dist < 4000 && (e->dog_flags & 1)) { re15_dog_sub(e, 1); break; }   /* @0x8010ddcc-e4 */
            if (e->dog_dist < 5000 && re15_dog_player_aiming())  { re15_dog_sub(e, 1); break; }  /* aca58==0x701 @0x8010de08-2c (audit #7) */
            if (e->dog_dist < 6000 && re15_dog_player_running()) { re15_dog_sub(e, 1); break; }  /* aca58==0x201 @0x8010de38-5c (audit #7) */
            if (s_dog_pack_alert & 1) re15_dog_sub(e, 1);                                        /* aca50&1 @0x8010de60-84 (audit #7) */
            break;
        case 2: { /* CHASE decision FUN_8010e0c4 */
            int in_cone = (pl->hit_react == 0) && re15_dog_arc(e, pl, 2500, 192);   /* acae7==0 && a804(0x9c4,0xc0)<0 @0x8010e0d0-f8 */
            if (in_cone) { re15_dog_sub(e, 3); break; }                              /* -> ATTACK-RANGE */
            if (e->dog_dist < 5000 && re15_dog_player_aiming() && e->dog_blocked_ctr == 0)   /* 0x701 @0x8010e110-138 (audit #7) */
                { re15_dog_sub(e, 3); break; }
            if (e->dog_dist < 6000 && re15_dog_player_running() && e->dog_blocked_ctr == 0)  /* 0x201 @0x8010e148-178 (audit #7) */
                { re15_dog_sub(e, 3); break; }
            if (e->dog_pounce_cd == 0 && pl->hp > 0x50 && e->dog_dist > 7000 && (e->dog_flags & 1)) {  /* pounce gate @0x8010e194-1c8 */
                uint32_t r1 = re15_engine_rand8(), r2 = re15_engine_rand8();   /* BOTH rng draws unconditional @0x8010e1d0/e1e0 */
                if ((r1 & 1) && (r2 & 1)) { re15_dog_sub(e, 4); break; }       /* (r1&1)*(r2&1) @0x8010e1f4-218 */
            }
            if ((s_dog_pack_alert & 1) == 0) {           /* aca50&1 clear -> reroute-or-stay @0x8010e21c-24c */
                re15_dog_try_reroute(e);                 /* AFTER the pounce roll (audit #12) */
                break;
            }
            re15_dog_sub(e, 3);                          /* pack alert -> commit menace @0x8010e244 (audit #7) */
            break;
        }
        case 3:   /* ATTACK-RANGE decision FUN_8010e568 */
            if (e->dog_atk_cd == 0 && re15_dog_arc(e, pl, 3000, 384)) {   /* +0x1d6==0 && a804(3000,0x180)<0 @0x8010e574-90 */
                uint8_t pick = 8;                          /* default BITE @0x8010e5ac */
                if (pl->hit_react != 0) {                  /* `beq acae7,zero -> exit` @0x8010e5cc: escalation ONLY
                                                            * while the player is hit-reacting (audit #11) */
                    pick = 6;                              /* quick SNAP @0x8010e5e0 */
                    if (pl->hp < 0x32) {                   /* acaee < 50 @0x8010e5e8-f4 */
                        pick = 5;                          /* arm-grab windup @0x8010e608 */
                        if (re15_engine_rand8() & 1) pick = 7;   /* low-HP hop @0x8010e624 */
                    }
                }
                re15_dog_sub(e, pick);
            } else {
                re15_dog_try_reroute(e);                   /* identical reroute gate in the else-branch
                                                            * @0x8010e628-6c0 (audit #12) */
            }
            break;
        default: break;                                    /* all other decision rows are jr-ra stubs */
        }

        /* ---- ACT pass (@0x80120fd4[+0x5]) — runs on the sub the decision just wrote ---- */
        switch (e->sub_state_1) {
        case 0:   /* IDLE act FUN_8010dea0 */
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 300); re15_dog_clip(e, 1); e->anim_frac = 0; e->sub_state_2 = 1; }  /* +0x9c=rng+300 @0x8010dec0 */
            else if (e->ai_timer == 0) re15_dog_sub(e, 1);   /* idle timeout -> leave idle @0x8010df34 */
            else e->ai_timer--;
            re15_dog_anim(e);
            break;

        case 1:   /* TURN/REORIENT act FUN_8010df9c */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 2); e->sub_state_2 = 1; }   /* clip 2 @0x8010df9c */
            if (re15_dog_anim(e)) re15_dog_sub(e, 2);         /* clip done -> CHASE @0x8010e06c */
            break;

        case 2:   /* CHASE act FUN_8010e304 (audit #6: speed roll + rng steer + wander mode) */
            if (e->sub_state_2 == 0) {
                e->crow_speed = (int16_t)((re15_engine_rand8() & 7) + 10);  /* +0x8c=(rng&7)+10 @0x8010e32c-44 */
                e->sub_state_2 = 1;
                e->ai_timer = 0; e->grab_kill_ctr = 0;        /* +0x9c=0 / +0x9e=0 @0x8010e360-70 */
                re15_dog_clip(e, 0);                          /* walk clip 0 @0x8010e378-98 */
            }
            if (e->dog_blocked_ctr < 0x1f) {                  /* slti +0x1dc,31 @0x8010e3bc */
                e->dog_yawrate = (int16_t)((re15_engine_rand8() & 0xf) + 1);   /* +0x1e2=(rng&0xf)+1 @0x8010e468-84 */
                re15_enemy_steer_point(e, pl->x, pl->z, e->dog_yawrate);       /* a8f8(&player,+0x1e2) @0x8010e49c */
            } else {                                          /* WANDER mode (blocked >= 31 frames) */
                if (e->ai_timer == 0) {                       /* re-latch when +0x9c==0 @0x8010e3d0 */
                    int t = (int)(int16_t)e->rot_y + ((re15_engine_rand8() & 1) ? 0x400 : -0x400);  /* rng&1 -> +-1024 @0x8010e3dc-f0 */
                    e->dog_aux9f = (int8_t)(t & 0xff);        /* BYTE-TRUNCATED latch `sb` @0x8010e3f4 (quirk preserved) */
                }
                re15_slew_to_angle(e, (int)e->dog_aux9f, 0x20);   /* aa68((s8)+0x9f, 0x20); +0x6a += @0x8010e408-28 */
                {
                    int16_t old = e->ai_timer;
                    e->ai_timer = (int16_t)(old + 1);         /* +0x9c++ (delay-slot store) @0x8010e454 */
                    if (old >= 91) e->ai_timer = 0;           /* slti 91 wrap @0x8010e44c-64 */
                }
            }
            re15_dog_anim(e);                                 /* f314 @0x8010e4c0 */
            if (e->dog_pounce_cd) e->dog_pounce_cd--;         /* +0x1e6 ticks ONLY in the chase act @0x8010e4cc-e4e0 (audit #15) */
            if (e->anim_frame == 0 || e->anim_frame == 0x1e) re15_audio_room_se(6);  /* footstep Se(6) @0x8010e4f8-538 */
            /* FUN_80111870(0, +0x95<0x1e) root-motion assist @0x8010e550 — OPEN (see header) */
            re15_dog_advance_ofs(e, 8);                       /* 245d8(8): speed=+0x8c, yaw OFFSET 8 @0x8010e548-54c (audit #6) */
            break;

        case 3: { /* ATTACK-RANGE act FUN_8010e6d4 — CIRCLING menace (audit #8) */
            re15_dog_speed_ramp_up(e, 0x14, 0xf0);            /* FUN_80111f0c(0x14,0xf0) @0x8010e6e8 */
            if (e->sub_state_2 == 0) {
                e->crow_speed = (int16_t)((re15_engine_rand8() & 0x3f) + 0xf0);  /* +0x8c=(rng&0x3f)+240 @0x8010e714-24 */
                e->sub_state_2 = 1;
                e->ai_timer = 0;                              /* +0x9c=0 @0x8010e734 */
                e->grab_kill_ctr = (int16_t)((re15_engine_rand8() & 0xf) + 0x40);  /* +0x9e=(rng&0xf)+0x40 turn rate @0x8010e748-58 */
                e->dog_aux9f = 0;                             /* +0x9f=0 @0x8010e764 */
                re15_dog_clip(e, 3);                          /* clip 3 @0x8010e76c-90 */
            }
            int sv;                                           /* a9cc turn direction (feeds the clip select) */
            if (e->dog_blocked_ctr == 0 && e->dog_atk_cd == 0 && e->crow_speed >= 0xf0) {   /* @0x8010e7a4-c8 */
                sv = re15_ai_arc_test(e, pl->x, pl->z, (int)e->grab_kill_ctr);   /* a9cc(&player,(s8)+0x9e) @0x8010e7f8 */
                re15_enemy_steer_point(e, pl->x, pl->z, (int)e->grab_kill_ctr);  /* a8f8(&player,(s8)+0x9e) @0x8010e810 */
            } else {
                sv = re15_ai_arc_test(e, pl->x, pl->z, 0x10);                    /* a9cc(&player,0x10) @0x8010e824 */
                if (re15_dog_blocked(e))                                          /* +0x1da != 0 @0x8010e838 */
                    sv = re15_ai_arc_test(e, pl->x, pl->z, 0x40);                /* a9cc(&player,0x40) @0x8010e848 */
                e->rot_y = (uint16_t)(((int)e->rot_y + sv) & 0xfff);             /* +0x6a += sv @0x8010e860-70 */
            }
            if (e->dog_dist < 5000) {                          /* turn-boost counter @0x8010e884-98 */
                e->ai_timer++;
                if (e->ai_timer > 0x3c) re15_enemy_steer_point(e, pl->x, pl->z, 0x40);   /* a8f8(0x40) after 61f @0x8010e8b0-c8 */
            }
            if (e->dog_flags == 0) {                           /* +0x1d0 == 0: LOS lost @0x8010e8e4 */
                e->dog_aux9f = (int8_t)(e->dog_aux9f + 1);     /* +0x9f++ @0x8010e8f4-900 */
                if (e->dog_aux9f == 6) re15_dog_speed_ramp_down(e, 0x1e, 200);   /* FUN_80111f6c(0x1e,200) @0x8010e90c-18 */
                if (e->dog_blocked_ctr > 0x1e) {               /* blocked > 30 frames @0x8010e924-2c */
                    re15_dog_sub(e, 2); e->sub_state_3 = 0;    /* -> CHASE (+0x5=2,+0x6=0,+0x7=0) @0x8010e93c-60;
                                                                * the ONLY exit — NO dist>=5000 exit exists (audit #8) */
                    break;
                }
            } else {
                e->dog_aux9f = 0;                              /* +0x9f=0 @0x8010e96c */
                e->crow_speed = 0xf0;                          /* +0x8c=0xf0 @0x8010e978 */
                e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be = player @0x8010e984-9c */
            }
            re15_dog_clip_keep(e, sv);                         /* FUN_80111fcc(sv): {5,3,4} @0x8010e9a8 */
            re15_dog_anim(e);                                  /* f314 @0x8010e9c4 */
            if ((e->anim_frame % 7) == 6) re15_audio_room_se(6);   /* footstep frame%7==6 Se(6) @0x8010e9e0-a14 */
            re15_dog_advance_ofs(e, 0);                        /* 245d8(0) at +0x8c=240..303 @0x8010ea1c (audit #8) */
            break;
        }

        case 4:   /* STATIONARY BARK CHAIN FUN_8010ea44 (audit #2): 7-step inner FSM on +0x6
                   * (jt @0x801001ac). The dog NEVER moves and NEVER enters state 5 from here. */
            switch (e->sub_state_2) {
            case 0:
                e->sub_state_2 = 1;
                re15_dog_clip(e, 0x0b); e->anim_flags = 0;    /* clip 0xb (+0x94=0xb,+0x95/96=0,+0x8f=7) @0x8010ea90-ad4 */
                e->dog_pounce_cd = 0x3c;                      /* +0x1e6 = 0x3c (NOT 0x78) @0x8010eadc-eae4 (audit #2) */
                if (re15_dog_anim(e)) e->sub_state_2++;       /* shared f314 tail @0x8010ec40 */
                break;
            case 2:
                e->sub_state_2 = 3;
                e->motion++; e->anim_frame = 0; e->anim_frac = 7;   /* clip 0xc (+0x94+1) @0x8010eb04-38 */
                re15_audio_room_se(2);                        /* bark Se(2) @0x8010eb40-48 */
                s_dog_pack_alert |= 1;                        /* aca50 |= 1 @0x8010eb58-5c (audit #7) */
                if (re15_dog_anim(e)) e->sub_state_2++;
                break;
            case 4:
                s_dog_pack_alert = (uint16_t)(s_dog_pack_alert & 0xfffe);   /* aca50 &= ~1 @0x8010eb78-7c */
                if (e->hp < 0) {                              /* shot during the chain @0x8010eb88 */
                    e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 1;  /* +0x4=3,+0x7=1 @0x8010eb98-bc8 */
                    re15_dog_blood(e);                        /* 19700(0x2000, rot_y, bone+0x2f0) @0x8010ebe0 */
                    break;
                }
                e->sub_state_2 = 5;                           /* +0x6=5 = INNER step (NOT +0x4=5!) @0x8010ebf0 (audit #2) */
                e->motion++; e->anim_frame = 0; e->anim_frac = 7;   /* clip 0xd @0x8010ebf8-ec2c */
                if (re15_dog_anim(e)) e->sub_state_2++;
                break;
            case 6:
                e->sub_state_1 = 3; e->sub_state_2 = 0;       /* exit -> sub 3 ATTACK-RANGE @0x8010ec7c-88 (audit #2) */
                break;
            default:                                          /* steps 1/3/5 wait on the clip */
                if (re15_dog_anim(e)) e->sub_state_2++;       /* +0x6 += f314 done @0x8010ec40-70 */
                break;
            }
            break;

        case 5:   /* WINDUP / arm-grab FUN_8010ecb4 (inner @0x801001cc, clips 0x0f->0x10->0x11) */
            switch (e->sub_state_2) {
            case 0: re15_dog_clip(e, 0x0f); e->dog_grab_armed = 1; e->sub_state_2 = 1; break;  /* clip 0x0f + ARM +0x1e4=1 @0x8010ed54 */
            case 2: re15_dog_clip(e, 0x10); re15_audio_room_se(8); e->sub_state_2 = 3; break;  /* clip 0x10 + Se(8) @0x8010edb0 */
            case 4: re15_dog_clip(e, 0x11); e->sub_state_2 = 5; break;                          /* clip 0x11 @0x8010edc0 */
            case 6: re15_dog_sub(e, 3); break;                                                  /* exit -> ATTACK-RANGE @0x8010ee64 */
            default: if (re15_dog_anim(e)) e->sub_state_2++; break;                             /* shared wait @0x8010ee1c */
            }
            break;

        case 6:   /* SNAP (player-reacting) FUN_8010ee90 (clip 0x12): a single quick growl-bite */
            if (e->sub_state_2 == 0) { e->motion = 0x12; e->anim_frame = 0; e->anim_frac = 0; e->sub_state_2 = 1; }  /* clip 0x12, no blend @0x8010ef1c */
            else if (e->sub_state_2 == 1) {
                if (e->anim_frame == 0x0a || e->anim_frame == 0x19 || e->anim_frame == 0x2d) re15_audio_room_se(0);  /* growl Se(0) @0x8010ef4c */
                if (re15_dog_anim(e)) e->sub_state_2 = 2;
            } else re15_dog_sub(e, 3);                        /* exit -> ATTACK-RANGE: `ori v0,3` @0x8010eed0 (delay) ->
                                                               * `sb v0,5` @0x8010ef90 (audit #9 — NOT chase) */
            break;

        case 7:   /* LOW-HP HOP FUN_8010efbc (clip 0x13): yaw-homing 20-unit/frame REARWARD hop */
            if (e->anim_frame == 0 || e->anim_frame == 0x1b) re15_audio_room_se(6);   /* Se(6) @0x8010efe0-f00c */
            if (e->sub_state_2 == 0) {
                e->sub_state_2 = 1;
                e->motion = 0x13; e->anim_frame = 0;          /* clip 0x13 (+0x94/+0x95/+0x96; NO +0x8f write) @0x8010f038-64 */
                e->crow_speed = 0x14;                         /* +0x8c = 20 (the SPEED) @0x8010f070-74 (audit #10) */
            }
            if (e->sub_state_2 == 1) {
                e->dog_yawrate = (int16_t)((re15_engine_rand8() & 0xf) + 1);   /* +0x1e2 @0x8010f090-a4 */
                re15_enemy_steer_point(e, pl->x, pl->z, e->dog_yawrate);       /* a8f8 homing @0x8010f0ac */
                if (re15_dog_anim(e)) e->sub_state_2 = 2;
                /* FUN_80111870(0, +0x95<0xd) root motion @0x8010f118 — OPEN (see header) */
                re15_dog_advance_ofs(e, 0x800);               /* 245d8(0x800) = 180° REARWARD at +0x8c=20
                                                               * @0x8010f120-124 (audit #10 — was a 0x800-speed charge) */
            } else if (e->sub_state_2 == 2) {
                re15_dog_sub(e, 3);                           /* exit -> sub 3: `ori v0,3` @0x8010f028 (delay) ->
                                                               * `sb v0,5` @0x8010f130 (audit #9 — NOT chase) */
            }
            break;

        case 8:   /* BITE FUN_8010f15c — the 7-phase leap machine (jt @0x801001ec; audit #3):
                   * leap -> connect => knockdown-or-grab; whiff => land + 2nd window -> sub 3;
                   * knockdown => recoil backward -> CHAINED attack pick 7/5/6. */
            switch (e->sub_state_2) {
            case 0:
                e->sub_state_2 = 1;
                re15_dog_clip(e, 0x14);                       /* clip 0x14 @0x8010f174-1c4 */
                e->crow_speed = 0xf0;                         /* +0x8c = 240 (NO +0x1d6 write at start — audit #3) @0x8010f1f8-200 */
                re15_audio_room_se(5);                        /* Se(5) @0x8010f1fc */
                /* fall through */
            case 1:
                if (re15_dog_anim(e)) e->sub_state_2++;       /* f314(a2=+0x7) @0x8010f198-1e8 */
                if (e->anim_frame > 0x0c) {                   /* leap window @0x8010f254 */
                    e->crow_speed = (int16_t)(e->crow_speed + 6);   /* +0x8c += 6 @0x8010f25c-64 */
                    re15_dog_advance_ofs(e, 0);               /* 245d8(0) @0x8010f26c */
                    e->y -= 0xdc;                             /* +0x38 -= 220 rise @0x8010f27c-8c (audit #3) */
                    if (pl->hit_react == 0 && re15_dog_arc(e, pl, 2000, 384)) {   /* acae7==0 @0x8010f294 && a804(2000,0x180) @0x8010f2a8 */
                        pl->hp = (int16_t)(pl->hp - 10);      /* acaee -= 10 @0x8010f2c4-d0 */
                        if (e->dog_grab_armed == 0 && pl->hp >= 0) {   /* +0x1e4==0 @0x8010f2dc && hp>=0 @0x8010f2e4 */
                            /* KNOCKDOWN (audit #3): player cmd 2 (aca58=2 @0x8010f30c = the state-2
                             * hit FSM the port's game_step HP-drop flinch models), aca59=facing+2
                             * (@0x8010f314-24), aca5a=0 (@0x8010f330), acae7|=1 (@0x8010f338-44). */
                            re15_audio_room_se(3);            /* Se(3) connect @0x8010f2ec-f4 */
                            e->sub_state_2 = 5;               /* dog +0x6=5 recoil @0x8010f300 */
                            pl->hit_react |= 1;               /* acae7 |= 1 */
                            {
                                /* KORRIGIERT (player_hit_chain.md HIT-4, verify 2026-08-03): der
                                 * 37edc-Call @0x8010f34c-60 ist der WUND-STEMPEL — FUN_80037edc(
                                 * panel = byte[0x80121010+4 + aca59*2+(rng&1) - 4], amount 0x32).
                                 * Die Bytes 05 07 04 06 @0x80121014 sind PANEL-Indizes (front ->
                                 * 5/7 Brust, back -> 4/6 Ruecken), KEINE SE-Nummern — die alte
                                 * knock_se[]-Deutung spielte stattdessen zufaellige Raum-SEs. */
                                static const uint8_t knock_panel[4] = { 5, 7, 4, 6 };   /* @0x80121014 */
                                int facing = ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800 ? 1 : 0;  /* a780(&player) */
                                re15_wound_add(knock_panel[(facing ? 2 : 0) + (re15_engine_rand8() & 1)], 0x32);
                            }
                        } else {                              /* GRAB escalation LAB_8010f468 */
                            int facing = ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800 ? 1 : 0;  /* a780 @0x8010f468 */
                            re15_dog_sub(e, (uint8_t)(9 + facing));   /* +0x5=9/10, +0x6=0 @0x8010f47c-84 */
                            e->y = e->dog_floor_y;            /* +0x38 = +0x1ba @0x8010f49c */
                            e->dog_grab_armed = 0;            /* +0x1e4 = 0 @0x8010f4b4 */
                        }
                    }
                }
                break;
            case 2:
                e->sub_state_2 = 3;
                re15_dog_clip(e, 0x15);                       /* land clip 0x15 @0x8010f4c8-508 */
                e->ai_timer = (int16_t)(e->dog_floor_y - e->y);   /* +0x9c = floor - y @0x8010f518 */
                /* fall through */
            case 3:
                if (e->anim_frame > 1) {                      /* @0x8010f52c */
                    e->crow_speed = (int16_t)(e->crow_speed - 3);   /* +0x8c -= 3 @0x8010f534-40 */
                    re15_dog_advance_ofs(e, 8);               /* 245d8(8) @0x8010f548 */
                    e->y += (int32_t)(e->ai_timer / 11);      /* gravity return /11 @0x8010f558-7c */
                    if (e->y > (int32_t)e->dog_floor_y) {     /* landed @0x8010f580 */
                        e->y = e->dog_floor_y;
                        e->sub_state_2 = 4;                   /* +0x6=4 @0x8010f590 */
                    }
                    e->sub_state_2 = (uint8_t)(e->sub_state_2 + re15_dog_anim(e));   /* +0x6 += f314 (LAB_8010f598) */
                } else {                                      /* 2nd bite window (frames 0/1) @0x8010f5b0 */
                    re15_dog_advance_ofs(e, 0);               /* 245d8(0) @0x8010f5b8 */
                    if (pl->hit_react == 0 && re15_dog_arc(e, pl, 2000, 384)) {   /* @0x8010f5c0-d4 */
                        pl->hp = (int16_t)(pl->hp - 10);
                        if (e->dog_grab_armed == 0 && pl->hp >= 0) {   /* knockdown (no 37edc in window 2) */
                            re15_audio_room_se(3);
                            e->sub_state_2 = 5;
                            pl->hit_react |= 1;
                            e->sub_state_2 = (uint8_t)(e->sub_state_2 + re15_dog_anim(e));   /* goto LAB_8010f598 */
                        } else {
                            int facing = ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800 ? 1 : 0;
                            re15_dog_sub(e, (uint8_t)(9 + facing));
                            e->y = e->dog_floor_y; e->dog_grab_armed = 0;
                        }
                    } else {
                        if (re15_dog_anim(e)) e->sub_state_2++;   /* LAB_8010f598 */
                    }
                }
                break;
            case 4:                                           /* whiff-land exit @0x8010f5e0 */
                e->dog_atk_cd = 10;                           /* +0x1d6 = 0xa @0x8010f5e4 (audit #3) */
                re15_dog_sub(e, 3);                           /* -> ATTACK-RANGE @0x8010f7c4 */
                e->y = e->dog_floor_y;                        /* LAB_8010f7c8 */
                break;
            case 5:
                e->sub_state_2 = 6;
                e->motion = 0x16; e->anim_frame = 0; e->anim_frac = 0;   /* recoil clip 0x16, +0x8f=0 @0x8010f5f8-64c */
                e->crow_speed = 0x78;                         /* +0x8c = 0x78 @0x8010f654 */
                e->ai_timer = (int16_t)(e->dog_floor_y - e->y);   /* +0x9c @0x8010f668 */
                /* fall through */
            case 6:
                if (re15_dog_anim(e)) e->sub_state_2++;       /* f314 @0x8010f680-6a8 */
                if (e->anim_frame <= 0x10) {                  /* @0x8010f6bc */
                    e->crow_speed = (int16_t)(e->crow_speed - 3);   /* +0x8c -= 3 @0x8010f6c4-dc */
                    re15_dog_advance_ofs(e, 0x800);           /* 245d8(0x800) recoil BACKWARD @0x8010f6d0-e0 */
                    e->y += (int32_t)(e->ai_timer / 14);      /* /0xe descent @0x8010f6e8-70c */
                    if (e->y > (int32_t)e->dog_floor_y) e->y = e->dog_floor_y;   /* clamp @0x8010f720-38 */
                }
                break;
            case 7: {                                         /* CHAINED attack pick @0x8010f75c */
                e->dog_atk_cd = 0x14;                         /* +0x1d6 = 0x14 @0x8010f768 (audit #3) */
                uint8_t pick = 7;                             /* +0x5=7 @0x8010f778 */
                if (pl->hp < 0x32) pick = 5;                  /* acaee<50 -> 5 @0x8010f790-a0 */
                if (re15_engine_rand8() & 1) pick = 6;        /* rng&1 -> 6 @0x8010f7a4-c4 */
                re15_dog_sub(e, pick);                        /* LAB_8010f7c8: +0x6=0 */
                e->y = e->dog_floor_y;
                break;
            }
            default: break;
            }
            break;

        case 9: case 10:   /* eaten-GRAB hold (7-step FSM 0x8010f80c/0x8010fc60): the dog eats the player */
            re15_dog_grabhold(e, pl);
            break;

        case 0x0b:   /* DIED-IN-GRAB 0x801100b4 (clip 0x1b): the player died -> the dog feeds (freeze) */
            if (e->sub_state_2 == 0) {
                re15_dog_clip(e, 0x1b);                       /* corpse-feed clip @0x801100f0 */
                s_player_grabbed = 1;
                re15_player_victim_devour(e);                 /* aca58=6 -> devour/collapse -> death @0x80110130 */
                e->sub_state_2 = 1;
            } else if (e->sub_state_2 == 1) {
                s_player_grabbed = 1;
                if (re15_dog_anim(e)) e->sub_state_2 = 2;      /* clip done -> feed hold */
            }
            /* byte-true: NO exit — the dog stays feeding on the corpse (+0x6>=2 -> return @0x801100cc) */
            break;

        case 0x0c:   /* RELEASE 0x801101e4 (clip 0x08): the player broke free -> recover to CHASE */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 8); e->sub_state_2 = 1; }   /* stand-up clip 8 @0x8011023c */
            else if (e->sub_state_2 == 1) { if (re15_dog_anim(e)) e->sub_state_2 = 2; }
            else re15_dog_sub(e, 2);                          /* exit -> CHASE @0x801102b0 */
            break;

        case 13:  /* OBSTACLE-REROUTE 13: leap OVER (one level UP) -> CHASE */
            re15_dog_reroute13(e, slot);
            break;
        case 14:  /* OBSTACLE-REROUTE 14: drop DOWN (1-4 levels) -> CHASE */
            re15_dog_reroute14(e);
            break;

        default:  /* only sub-states 0-14 exist -> unhandled falls back to chase */
            re15_dog_sub(e, 2);
            break;
        }

        /* ---- root tail FUN_8010dbcc @0x8010dd48-ddb0 (runs AFTER the dispatch) ---- */
        /* jal 0x80012974(4000) @0x8010dd48: entity word0 |= 0x20000000 when dist<4000 — OPEN (header) */
        if (e->dog_atk_cd) e->dog_atk_cd--;                   /* +0x1d6 tick @0x8010dd5c-70 */
        if (!re15_dog_blocked(e)) e->dog_blocked_ctr = 0;     /* +0x1da==0 -> +0x1dc=0 @0x8010dd80-8c */
        else e->dog_blocked_ctr++;                            /* else +0x1dc++ @0x8010dd94-da4 (audit #15) */
        break;
    }

    case 4: case 5: case 6:   /* SCRIPTED states (0x80111350): grid-0x40/0x41 dogs + kill machines */
        re15_dog_state456(e, pl);
        break;

    case 2: { /* HURT dispatcher FUN_801108f0 -> rows @0x80121018 (audit #4/#5) */
        re15_dog_fall_physics(e);
        if (!re15_dog_row_airborne(e->sub_state_1)) {
            /* GROUNDED flinch FUN_801109e0 */
            switch (e->sub_state_3) {
            case 0:
                e->sub_state_3 = 1;
                re15_dog_clip(e, 6);                          /* clip 6 @0x80110a3c-70 */
                re15_audio_room_se(1);                        /* Se(1) @0x80110a7c */
                /* +0x1c0 = 0x8000 @0x80110a80 — OPEN (header) */
                re15_dog_blood(e);                            /* 19700(0x2000, rot_y, bone+0xec) @0x80110a84-aa0 (audit #4) */
                /* fall through */
            case 1:
                if (re15_dog_anim(e)) e->sub_state_3++;       /* f314 @0x80110ab0-dc */
                break;
            case 2:
                e->y = e->dog_floor_y;                        /* @0x80110af4 */
                if (e->hp < 0) {                              /* @0x80110b04 */
                    e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 1;  /* death, +0x7=1 @0x80110b10-40 */
                } else {
                    e->state = 1; e->sub_state_1 = 7;         /* recover -> COUNTER-LUNGE sub 7:
                                                               * `sb 1,+0x4` @0x80110b48 + `sb 7,+0x5` @0x80110b58 (audit #4) */
                    e->sub_state_2 = 0; e->sub_state_3 = 0;
                    e->hit_react = 0;                         /* +0x93 = 0 @0x80110b70 */
                }
                break;
            default: break;
            }
        } else {
            /* AIRBORNE knock-slide FUN_80110b9c (audit #5) */
            switch (e->sub_state_3) {
            case 0:
                e->sub_state_3 = 1;
                e->motion = 7; e->anim_frame = 0; e->anim_frac = 0;   /* clip 7, +0x8f=0 @0x80110bec-c14 */
                e->crow_speed = 0x96;                         /* +0x8c = 0x96 @0x80110c34-38 */
                /* +0x1c0 = 0x8000 @0x80110c3c — OPEN */
                e->grab_kill_ctr = (int16_t)((re15_engine_rand8() + 0x2d) & 0x3f);   /* +0x9e @0x80110c48-5c */
                re15_audio_room_se(1);                        /* Se(1) @0x80110c68 */
                re15_dog_blood(e);                            /* 19700(0x2000, rot_y, bone+0x40) @0x80110c6c-88 */
                /* fall through */
            case 1:
                if (re15_dog_anim(e)) e->sub_state_3++;       /* f314 @0x80110c98-cc0 */
                e->crow_speed = (int16_t)(e->crow_speed - 2); /* +0x8c -= 2 @0x80110cd0-e0 */
                re15_dog_advance_ofs(e, (1 - (e->hit_react & 0x80)) * 0x800);   /* 245d8((1-(+0x93&0x80))*0x800)
                                                               * @0x80110ce4-f8 (both cases ≡ 0x800 mod 0x1000) */
                if (e->sub_state_3 == 2) re15_audio_room_se(7);   /* Se(7) on 1->2 @0x80110d08-14 */
                break;
            case 2: {
                e->y = e->dog_floor_y;                        /* @0x80110d34 */
                int16_t old = e->grab_kill_ctr;
                e->grab_kill_ctr = (int16_t)(old - 1);        /* +0x9e-- @0x80110d44-50 */
                if (old == 0) {                               /* counter WAS 0 -> stand-up @0x80110d54 */
                    e->state = 1; e->sub_state_1 = 0x0c;      /* recover -> sub 0xc stand-up @0x80110d60-78 */
                    e->sub_state_2 = 0; e->sub_state_3 = 0;
                    e->hit_react = 0;                         /* +0x93 = 0 @0x80110d90 */
                }
                break;
            }
            default: break;
            }
        }
        break;
    }

    case 3: { /* DEATH dispatcher FUN_80110dc0 -> rows @0x80121070 (audit #5 + verified detour claim) */
        re15_dog_fall_physics(e);
        if (!re15_dog_row_airborne(e->sub_state_1)) {
            /* GROUNDED death FUN_80111120 */
            switch (e->sub_state_3) {
            case 0:
                /* fresh kill: kill flag AT DEATH ENTRY + DETOUR through the hurt flinch first
                 * (unverified-claim VERIFIED: +0x93|=2 @0x8011113c-54, `sb 2,+0x4`+zero +0x5/+0x6/+0x7
                 * @0x80111160-19c, jal 0x8004ef90(0x800b1038,+0x1c6) @0x801111b8-c4). */
                e->hit_react |= 2;
                if (e->em_flag_id != 0xFF)
                    re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);   /* ef90(0x800b1038,+0x1c6) */
                e->state = 2; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            case 1:
                e->y = e->dog_floor_y;                        /* @0x801111d8 */
                e->sub_state_3 = 2;
                re15_dog_clip(e, 0x0e);                       /* collapse clip 0xe @0x801111f0-121c */
                /* fall through */
            case 2:
                if (re15_dog_anim(e)) e->sub_state_3++;       /* f314 @0x80111228-50 */
                if (e->sub_state_3 == 3) re15_audio_room_se(7);   /* Se(7) on 2->3 @0x80111260-6c */
                break;
            case 3:
                e->state = 7; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* word +0x4=7 @0x80111284 */
                break;
            default: break;   /* the in-handler case-4 sink (@0x801112a0..) is unreachable — the live
                               * corpse fade is the state-7 root (FUN_80111774), see CORPSE below */
            }
        } else {
            /* AIRBORNE death FUN_80110eb0 */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2;                            /* +0x93 |= 2 @0x80110ed4-e0 */
                e->sub_state_3 = 1;
                e->motion = 7; e->anim_frame = 0; e->anim_frac = 0;   /* clip 7, +0x8f=0 @0x80110ef4-f20 */
                e->crow_speed = 0x96;                         /* +0x8c = 0x96 @0x80110f40 */
                /* +0x1c0 = 0x8000 — OPEN */
                re15_audio_room_se(4);                        /* Se(4) @0x80110f50 */
                re15_dog_blood(e);                            /* 19700(0x2000, rot_y, bone+0x40) @0x80110f54-70 */
                if (e->em_flag_id != 0xFF)
                    re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);   /* ef90(0x800b1038,+0x1c6)
                                                               * AT DEATH ENTRY @0x80110f80-8c (audit #5) */
                /* fall through */
            case 1:
                if (re15_dog_anim(e)) e->sub_state_3++;       /* f314 */
                e->crow_speed = (int16_t)(e->crow_speed - 2); /* +0x8c -= 2 */
                re15_dog_advance_ofs(e, (1 - (e->hit_react & 0x80)) * 0x800);   /* backward slide */
                if (e->sub_state_3 == 2) re15_audio_room_se(7);   /* Se(7) on 1->2 */
                break;
            case 2:
                e->y = e->dog_floor_y;
                e->state = 7; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* word +0x4=7 */
                break;
            default: break;   /* in-handler case 3 sink unreachable — see CORPSE root */
            }
        }
        break;
    }

    case 7:   /* CORPSE root FUN_80111774 (raw disasm): phase 0 seeds the fade, phase 1 runs it,
               * phase 2 = inert. (verified corpse-fade part of the death-detour claim) */
        if (e->sub_state_3 == 0) {
            e->grab_kill_ctr = 0x5a;                          /* +0x9e = 90 fade frames @0x801117a0-a4 */
            e->sub_state_3 = 1;                               /* +0x7 = 1 @0x801117b0-b4 */
            /* entity word0 |= 2 (@0x801117cc) | 0x40 (@0x801117e8) — flag word OPEN (header) */
        }
        if (e->sub_state_3 == 1) {
            /* +0xc4/+0xec = (old & 0xff000000) | 0xffff38 tint + pool +0xbc/+0xbe += 8/frame
             * (@0x801117f0-834): the corpse blood-pool spread — render-side keyed on the state-7
             * countdown, same presentation channel as the zombie corpse pool. */
            int16_t old = e->grab_kill_ctr;
            e->grab_kill_ctr = (int16_t)(old - 1);            /* +0x9e-- @0x80111844-54 */
            if (old == 0) e->sub_state_3 = 2;                 /* WAS 0 -> phase 2 inert @0x80111850-64 */
        }
        break;

    default:  /* only states 0-7 exist as top-level (@0x80120f74) */
        break;
    }
}

/* ============================ SPIDER-BABY (type 0x26, EM026) — Wave 1 ======================== *
 * Byte-true 0x80116288 family (RE15_SPIDER_AI.md; workflow wf_b77c2591, adversarially verified). A
 * STATIONARY web-spitter/ambush: root 0x80116288 dispatches +0x4 via @0x80121268 (INIT / ACTIVE /
 * HURT / DEATH), the spider EMERGES vertically from its spawn (spider_phase < 13 = intangible), then is
 * solid + deals a -2 contact stagger. STATE[1] is a stationary brain that paces web-spit SPRITE effects
 * (NO locomotion — exhaustive-scan confirmed). The -2 CONTACT is the spider's ONLY player damage.
 * LIVE-VERIFIED byte-true (ROOM1090 savestate): type 0x26, state 1, hitbox 600/720, variant behaviour.
 * CORRECTION (byte-verified via the collision-instance readers + RE wf_5d64f47e): STATE[1]'s "hit-codes"
 * (0x12/0x22/0x23 armed via 0x80019d50) are NOT damage — inst+0 is a DISPATCH INDEX into the routine table
 * @0x80071d40, and the spider selects only VISUAL routines (table[0x12] = 0x80017c8c = the web-spit
 * PARTICLE/SPRITE handler). The table DOES also hold model-instance DAMAGE routines — idx25 @0x80017fa4 and
 * idx31 @0x8001854c call the player-damage resolver FUN_80012d60 (@0x80018008, radius 500, per-type table
 * @0x8006f418) — but the spider NEVER arms those, so it is gameplay-COMPLETE via the -2 contact alone. (The
 * earlier "no hp write in region 0x80017600-0x80018000" wording was imprecise: the damage routines exist in
 * the table; the spider simply doesn't use them, and no port enemy arms idx25/31 via 0x55 either.)
 * OPEN (audit wf_827f186d spider #6 — subsystem-scale PRESENTATION gap, exact bytes): the per-cycle
 * model-instance hit-code choreography. Original arms via 0x80019d50(a0=part 8/0x10 by variant tables
 * @0x8010031c/@0x80100334/@0x8010034c, a1=3, a3=*(+0x188)+64): STEP a2=0x22 @0x80116c4c-50, COMMIT
 * a2=0x12 @0x80116cc8-cc, hurt-windup a2=0x23 @0x80116bd0-d4; the mouth sprite spawns via 0x80019700
 * with a0=(phase<<8)|(0x0803../0x1003.. via table @0x80100364) a1=rot_y(+0x6a) a2=*(+0x188)+64
 * a3=pose @0x80121248 @0x80116d44-88. The port has no model-instance arming subsystem — it spawns a
 * generic telegraph fx at the roll site (below) and arms nothing; NOT approximated further.
 * WEB-SPIT byte-true RE (workflow wf_b95101b2, 2026-07-13 — corrects two earlier assumptions):
 *  - The "attack workstruct" *(0x800b52c4) is a CURSOR into the 96*0x84 EFFECT/SPRITE POOL @0x800a73b8 (the
 *    shared fx pool). Driver FUN_80019e20 walks all 96 slots and per active slot (+0x6c&1) dispatches
 *    routine_table[u16 +0x00] (0x80071d40) AND a secondary routine_table[u16 +0x02] in the draw pass.
 *  - The spider emits ONE mouth-anchored web sprite per sub-cycle: brain 0x80116d00 -> FUN_80019700 with
 *    a0=(0x1003<<16|counter<<8) large / (0x0803..) small => part id 0x10/0x08, tag 3, anchored at the mouth
 *    transform *(entity+0x188)+64; arm 0x80116c68 -> FUN_80019d50(a2=0x12). +0x00 alternates 0x22 (animate:
 *    +0x72 sprite-frame += 0x100) during emission and 0x12 (0x80017c8c) in flight; +0x02 = 0x24 (room-collide
 *    +0x28/+0x2a/+0x2c -> on hit set +0x02=0x25, zero velocity = stick/expire).
 *  - CORRECTION A: tick 0x80017c8c is a PURE velocity oscillator (advance +0x04/+0x06 by +0x1e/+0x26 for 10
 *    frames, then negate +0x0e/+0x1e/+0x26 & reload timer=10) — NO rng, NO angle, NO spawn/draw. The rng /
 *    +0xc00 / jal 0x800199d4 emit is the NEXT entry 0x13 (0x80017d08), which the spider NEVER arms.
 *  - CORRECTION B: NOT a ballistic projectile. Registrar FUN_80019700 zeros pos +0x34/+0x38/+0x3c but never
 *    writes velocity +0x10/+0x12/+0x14 or the oscillator vel fields; only hit-code 0x11 (field-init 0x80017c00)
 *    would, and the spider never arms 0x11 => stationary mouth-anchored sprite ("stationärer Web-Spitter").
 *  - HONEST RESIDUAL (faithful-line boundary): because 0x11 is never armed, the 0x12 oscillator reads
 *    UNINITIALIZED pool memory for +0x04/+0x06/+0x1e/+0x26 — whether the wobble is visibly nonzero is NOT
 *    statically determinable; it needs a ROOM1090 DuckStation savestate of the mouth slot mid-spit. A full
 *    byte-true SPRITE port also needs the web-glob sprite (part 0x10/0x08 sub 3) from its bank + the mouth
 *    anchor + the +0x00/+0x02 dual dispatch. The placeholder below is a defensible faithful-line until then. */
static void re15_spider_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint8_t variant  = (uint8_t)(e->grid_id & 0x7f);

    switch (e->state) {
    case 0:   /* INIT 0x801164b0: one-shot -> ACTIVE */
        if (e->hp <= 0) e->hp = 100;                      /* +0x9a=100 @0x801164f8 */
        e->hit_react = 0; e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;  /* clear @0x801164ec-544 */
        e->spider_phase = 0; e->spider_timer = 0;         /* +0x1d0..+0x1ec = 0 (8-word loop @0x801165b0) */
        e->spider_home_x = (int16_t)e->x;                 /* +0x1d8 = x (lhu +0x34; sh) @0x801165fc */
        e->spider_home_z = (int16_t)e->z;                 /* +0x1da = z (lhu +0x3c; sh) @0x80116614 */
        e->spider_home_y = (int16_t)e->y;                 /* +0x1d6 = y (lhu +0x38; sh) @0x8011662c */
        e->state = 1; e->sub_state_1 = variant;           /* +0x4=1 @0x80116690, +0x5 = +0x9 & 0x7f @0x801166ac */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        /* spawn/emergence fx 0x09031800, gated on !(grid&0x80): `andi v0,0x80; bne -> skip;
         * lui a0,0x903; ori a0,0x1800; a3=pose @0x80121248; a2=*(+0x188)+0x40; a1=lh +0x6a;
         * jal 0x80019700` @0x801166c4-e8. Port fx registrar = the ESP pool spawn (same model as
         * the ACTIVE web telegraph below; exact part id/pose = the OPEN sprite subsystem).
         * (audit wf_827f186d spider #7) */
        if (!(e->grid_id & 0x80))
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              e->x, e->y, e->z, (int16_t)e->rot_y);
        break;

    case 1: {  /* ACTIVE 0x801166fc: the stationary strike-arming brain (Behavior A/B on +0x6). */
        uint8_t budget = (e->sub_state_1 >= 3) ? 0x2c : 0x28;   /* A(0,1,2)=40 / B(3,4)=44 @0x80116784/8011689c */
        switch (e->sub_state_2) {
        case 0:   /* +0x6==0: seed the strike budget + telegraph "web" fx (ROLL 0x80116d00) */
            e->spider_phase = budget;
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              e->x, e->y, e->z, (int16_t)e->rot_y);   /* web telegraph fx 0x0803../0x1003.. @0x80116d84 */
            e->spider_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 16);  /* +0x1d4 = rng&0x3f+16 [16,79] @0x80116da0 */
            e->sub_state_2 = 1;
            break;
        case 1: { /* +0x6==1: wind-up; on timer expiry arm a STEP strike (hit-code 0x22 = OPEN below) */
            uint16_t t = (uint16_t)e->spider_timer;
            e->spider_timer = (int16_t)(t - 1);   /* +0x1d4-- ALWAYS stored (delay slot `sh v0,468`
                                                   * @0x801167d4 / @0x801168ec — wraps 0->0xffff) */
            if (t != 0) break;                    /* `bne v1,zero,exit` on the PRE-decrement value */
            if (e->spider_phase >= 0x30) break;                      /* budget exhausted -> hold @0x801167ec */
            if (e->sub_state_1 < 3 && e->spider_phase < 8) { e->sub_state_2 = 3; break; }  /* A dead-abort @0x8011683c
                                                   * (Behavior B @0x80116910 only SKIPS the 0x22 arm when phase<8) */
            /* STEP arm FUN_80116bec: 0x80019d50(part 8/0x10 via table @0x80100334, a1=3, a2=0x22,
             * a3=*(+0x188)+64) @0x80116c4c-50 — model-instance hit-code arming = OPEN (see header) */
            e->spider_phase++;                                       /* +0x1d0++ @0x8011681c */
            e->sub_state_2 = 2;
            break;
        }
        case 2:   /* +0x6==2: COMMIT strike FUN_80116c68 (hit-code 0x12 arm @0x80116cc8-cc = OPEN)
                   * + unconditional timer re-roll -> loop */
            e->spider_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 16);  /* rng&0x3f+16 @0x80116cd4-ec */
            e->sub_state_2 = 1;
            break;
        default:  /* +0x6==3: Behavior-A dead sub-state (idle hold) */
            break;
        }
        break;
    }

    case 2: case 3: case 4:
        /* HURT *AND* DEATH: state table @0x80121268 [2]=[3]=[4]=0x8011697c, which dispatches
         * @0x80121290[+0x5] (`addiu at,at,4752` @0x8011699c) — ALL 6 entries = the flinch
         * FUN_80116a04 (table dumped). The EXE damage writers only ever store 2 or 3
         * (@0x8001252c/0x80012538, @0x80013018/0x80013020) and hp is read NOWHERE in
         * 0x80116288-0x80116db4, so hp<0 (state 3) runs the SAME 3-phase flinch and resets to
         * ACTIVE: the original spider is UNKILLABLE, never a corpse, always respawns. The port's
         * former state-8/9 gib->corpse latch + state-7 kill-flag persist were invented — removed.
         * (audit wf_827f186d spider #1) */
        switch (e->sub_state_3) {                        /* +0x7 flinch phase */
        case 0:
            e->hit_react = 3;                            /* +0x93 = 3 hit-flash (sb @0x80116a50) */
            if (e->spider_phase >= 8) {
                /* windup arm FUN_80116b70 (`sltiu v0,v0,8; bne -> skip; jal 0x80116b70`
                 * @0x80116a68-78): 0x80019d50(part 8/0x10 via table @0x8010031c, a1=3, a2=0x23,
                 * a3=*(+0x188)+64) @0x80116bd0-d4 — model-instance hit-code arming = OPEN. */
            } else if (variant < 3) {
                /* flag bitset 0x8004ef90(a0=0x800b1028, a1=variant+29) @0x80116ac0-cc, gated
                 * phase<8 && (+0x9&0x7f)<3 @0x80116a90/ab4 — bit (variant+29) of the shared
                 * flag-array word @0x800b1028 (the port's s_crow_gflags, MSB-first). */
                s_crow_gflags |= 0x80000000u >> ((unsigned)(variant + 29) & 0x1f);
            }
            if (e->spider_phase >= 2)                    /* `sltiu v0,v0,2; bne -> skip` @0x80116ae4-e8 */
                e->spider_phase--;                       /* +0x1d0-- floor 1 @0x80116af8-b00 */
            e->sub_state_3++;                            /* +0x7++ @0x80116b18-28 */
            break;
        case 1:
            /* COMMIT FUN_80116c68 (`jal 0x80116c68` @0x80116b04): 0x12 arm @0x80116cc8-cc (OPEN)
             * + UNCONDITIONAL strike-timer re-roll — one rng draw, fresh [16,79] cadence.
             * (audit wf_827f186d spider #5) */
            e->spider_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 16);  /* +0x1d4 @0x80116cd4-ec */
            e->sub_state_3++;                            /* +0x7++ @0x80116b18-28 */
            break;
        case 2:
            e->state = 1; e->sub_state_2 = 1; e->sub_state_3 = 0;  /* +0x4 word = 0x10001
                                                          * (lui 0x1 @0x80116a44; ori 1 @0x80116b2c;
                                                          * sw +0x4 @0x80116b30) */
            e->sub_state_1 = variant;                    /* +0x5 = +0x9 & 0x7f restored @0x80116b40-4c */
            e->hit_react = 0;                            /* +0x93 = 0 @0x80116b5c */
            break;
        default:                                         /* +0x7 >= 3: no-op exit (j @0x80116a34) */
            break;
        }
        break;

    default:
        /* states 5-9: table @0x80121268 [5-7]=0x80116758 (Behavior A) / [8-9]=0x80116870 (B) —
         * the SAME words double as the ACTIVE variant table @0x8012127c[+0x5]. As ROOT states
         * they are UNREACHABLE: exhaustive disasm 0x80116288-0x80116db4 shows exactly three +0x4
         * writes (=1 @0x80116690, =0x10001 @0x801169d4 / @0x80116b30) and the EXE damage writes
         * only 2/3 — no writer of 5..9 exists. (audit wf_827f186d spider #1) */
        break;
    }

    /* --- root tail @0x80116304-498: runs after EVERY state dispatch (the tail never re-reads
     * +0x4 — contact/damage/pins apply in ACTIVE, HURT and "DEATH" alike). Adversarial verify of
     * the flagged claim wf_827f186d: the aec4 call IS there, but it pushes the SPIDER (pushee),
     * not the player — the player-side solidity is the b544 pass (port: re15_body_push_player),
     * which was never missing. What WAS missing: the emerge intangibility flags, the aec4-return
     * -2 gating (any state, per overlap frame) and the unconditional home pins. --- */
    e->contact_flags = 0;                                /* FUN_8002b498 contact clear @0x8011630c */
    if (e->spider_phase < 13) {                          /* `sltiu v0,v0,0xd; bne` @0x80116328-2c:
                                                          * EMERGING = intangible */
        e->flags |= 0x02;                                /* +0x0 |= 2 @0x801163dc-e8 (aec4 skip-bit:
                                                          * re15_body_push gate, so the player pass
                                                          * walks through an emerging spider) */
        e->flags |= 0x40;                                /* +0x0 |= 0x40 @0x801163ec-404 */
    } else {
        e->flags = (uint8_t)(e->flags & ~0x02u);         /* `addiu v1,-3; and` @0x80116330-40 */
        e->flags = (uint8_t)(e->flags & ~0x40u);         /* `addiu v1,-65; and` @0x80116350-5c */
        /* `jal 0x8002aec4` @0x80116368 with a0=0x800aca54 (player block, `addiu s0,s0,20`
         * @0x8011634c) a1=entity: pushes the SPIDER out of the player — undone by the pins
         * below — and its nonzero RETURN is the contact condition (`beq v0,zero,exit`
         * @0x80116370). (audit wf_827f186d spider body-push claim: CONFIRMED, direction fixed) */
        if (re15_body_push(pl, RE15_BODY_R_PLAYER, e, (int32_t)e->hit_radius_min)) {
            if (pl->hit_react == 0) {                    /* `bne v0,zero` @0x80116384 gates ONLY the
                                                          * stagger request, NOT the -2 */
                /* player stagger command: aca58=2 @0x8011638c-90, aca59=a780(player)+2
                 * @0x80116394-a4, aca5a=0 @0x801163a8-ac — the aca58 player-reaction cmd FSM is
                 * the unported dispatcher (OPEN; same trio as the crow dive @0x80113b00-30). The
                 * port-observable model is the hit_react latch the cmd-2 stagger holds. */
                pl->hit_react |= 1;
            }
            if (pl->hp >= 4)                             /* `slti v0,v0,4; bne -> skip` @0x801163c0-c4 */
                pl->hp = (int16_t)(pl->hp - 2);          /* -2 EVERY overlap tick, ANY state, NOT
                                                          * hit_react-gated @0x801163c8-d0
                                                          * (audit wf_827f186d spider #3) */
        }
    }
    /* home pins: UNCONDITIONAL — all three paths converge at 0x80116408 (beq @0x80116370,
     * j @0x801163d4, emerge fall-through @0x80116404). x/z re-pinned every tick; y slaved to
     * +0x1d0 in EVERY state: ACTIVE (phase 0x28/0x2c) holds y = home_y - step*(phase-1), each
     * HURT phase-- visibly raises it one step. (audit wf_827f186d spider #2) */
    e->x = e->spider_home_x;                             /* +0x34 = +0x1d8 (lh; sw) @0x80116414-1c */
    e->z = e->spider_home_z;                             /* +0x3c = +0x1da (lh; sw) @0x8011642c-34 */
    {
        int step = (e->grid_id & 0x80) ? 40 : 20;        /* (phase-1)*5<<3 / <<2 @0x80116444-90 */
        e->y = (int32_t)e->spider_home_y - step * ((int)e->spider_phase - 1);  /* +0x38 = +0x1d6 -
                                                          * step*(phase-1) @0x80116494-98 */
    }
}

/* ============================ MAGGOTS (type 0x27, EM027 = MAGGOTS_BABY) — Wave 3 ============= *
 * Byte-true 0x80116db8 family (RE15_MAGGOT_AI.md; wf_f597f55d + FULL re-port audit wf_827f186d).
 * A LARGE moving ground creature (1600x1440 body, HP 180 @0x8011f514): root 0x80116db8 dispatches
 * +0x4 via @0x801213c8, then body-push (0x8002aec4/0x8002b544) + SCA wall-clamp (FUN_8003b0a4 ->
 * +0x1d6 @0x80116e64-84). STATE[1] 0x80117254 = two-table brain: A decision @0x801213e8 + B movement
 * @0x80121428 on +0x5, then the UNCONDITIONAL timer tail (+0x1dc-- @0x801173f8-40c, +0x1e1--
 * @0x8011741c-38). Subs: 0 idle-wander / 1 dormant (grid&1) / 2 rise / 3 CHASE / 4 SELECTOR /
 * 5 BITE -6 / 6 HEAVY -12 / 7 LEAP / 8 mid-air FINISHER -600 / 15 REAR-UP PIN. States: 2 HURT
 * (lanes @0x801214a8: 0-6 flinch, 7-8 air-hit, 9-11 crash), 3 DEATH (@0x80121500: 0-6 ground,
 * 7-8 mid-air, 9 crash), 7 CORPSE fade 0x8011be54.
 * OPEN (subsystem gaps, exact bytes documented at the sites): the Path-A ZONE leaps (A[4]
 * @0x80117ecc-8011802c: FUN_8003b93c SCA-zone query for attr 0x10/0x20 + the FUN_8003b0a4 +0x90
 * heading-nibble — the port has neither the zone query nor the b0a4 escape-heading writer), and
 * the FUN_8011bf50/8011c024 locator matrices come from the model pool (port: recomputed from the
 * EMR pose — see re15_maggot_footlock). */
static const uint8_t s_maggot_clip_len[29] =   /* EM027 clip frame-counts, byte-true (CDEMD0.EMS idx 12, dir[1]) */
    { 78,20,15,70,78,39,24,12,25,25,40,40,100,40,70,90,35,20,25,40,40,21,58,30,50,40,70,30,52 };
static void re15_maggot_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_maggot_anim(re15_actor_t *e)   /* POST-inc +0x95, wrap at the real EM027 clip length */
{
    uint8_t c = e->motion; int fc = (c < 29) ? s_maggot_clip_len[c] : 1; if (fc < 1) fc = 1;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    return done;
}
/* hit-blood/gore burst func_0x80019700(0x2000, +0x6a, skel_part, &vec) — the maggot passes part
 * +0x40 (death/hurt @0x8011b0bc-130 / @0x8011b854-8d0) or +0x244 (mid-air death @0x8011bb78) and a
 * 4-word param vec @0x80121388[(+0x6&1)*0x20] (hurt/death, +0x6 = the take-damage hit-dir) or
 * @0x801213a8 (shot-in-flight @0x8011a904-4c / @0x80118b94-bd0). Port: the effect-0 burst at the
 * actor (same mapping as re15_dog_blood; bone offset + param vec = presentation-side). */
static void re15_maggot_gore(re15_actor_t *e)
{
    re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0, e->x, e->y, e->z, (int16_t)e->rot_y);
}
/* FUN_8001a9cc(&playerX, tol) — signed arc residue: 0 iff |bearing-rot_y| < tol (mod 0x1000),
 * else +tol when the wrapped residue <= 0x800, -tol beyond (decompile FUN_8001a9cc.c). The A[3]
 * rear-up gate proceeds on >= 0 (@0x80117ae4-ec: beq 0 -> proceed, blez -> skip). */
static int re15_maggot_a9cc(const re15_actor_t *e, const re15_actor_t *pl, int tol)
{
    int fb = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
    unsigned uv = (unsigned)((tol + (fb - (int)e->rot_y)) & 0xfff);
    if (uv < (unsigned)(tol << 1)) return 0;
    return (uv <= 0x800u) ? tol : -tol;
}
/* FUN_8001a780(player block) — heading alignment: 1 iff ((self.rot_y - player.rot_y) + 0x400)
 * & 0xfff < 0x800 (both facing the same way), else 0 (@0x8001a788-94, re15_damage.c F2 cite). */
static int re15_maggot_a780(const re15_actor_t *e, const re15_actor_t *pl)
{
    return ((((int)e->rot_y - (int)pl->rot_y) + 0x400) & 0xfff) < 0x800;
}
/* FUN_8011bf50(0, a1) / FUN_8011c024(0, a1) — the maggot's LOCATOR FOOT-PLANT root motion.
 * Byte-true mechanism (raw disasm @0x8011bf50-8011c020 / @0x8011c024-8011c114): recompute the
 * tracked locator bone's WORLD transform by chaining the entity matrix (+0x20) with the pool's
 * per-bone local matrices, then move the ENTITY by the NEGATIVE of the locator's world delta
 * vs the pool's stored last-rendered position: `+0x34 -= (m.tx - rec[84])` @0x8011bfd4-e8,
 * `+0x3c -= (m.tz - rec[92])` @0x8011bfec-c008 — a planted foot stays world-fixed and the body
 * crawls over it. The pool bone RECORD stride is 172 bytes (matrix +24, world t at +84/+88/+92):
 * bf50 chains records 0 -> 12+3*a1 -> 13+3*a1 -> 14+3*a1 (block = a1*516+0x968, 516 = 3*172,
 * 0x968 = 14*172) = TRACKED BONE 14+3*a1; c024 chains 0 -> 1 -> 4+4*a1 -> 5+4*a1 -> 6+4*a1
 * (block = a1*688+0x408, 688 = 4*172, 0x408 = 6*172) = TRACKED BONE 6+4*a1.
 * PORT: the pool matrix stack does not exist engine-side, so the two poses (frame, frame-1) are
 * recomputed from the EMR via re15_skel_compute_pose + the keyframe root channel — the same
 * authored data the pool matrices are built from. Self-skips without the loaded bank and on a
 * clip change (the original pops there and lets the FRAC blend smooth it). */
static void re15_maggot_footlock(re15_actor_t *e, int bone)
{
    static uint8_t s_prev_clip[RE15_ACTOR_MAX];
    int slot = (int)(e - g_actors);
    re15_enemy_bank_t *bank = re15_enemy_find(0x27);
    if (!bank || !bank->ok) return;
    const re15_emd_skeleton_t  *sk = &bank->skel;
    const re15_emd_animation_t *an = &bank->anim;
    int clip = e->motion;
    if (clip < 0 || clip >= an->clip_count || bone < 0 || bone >= sk->bone_count) return;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (c->frame_count <= 1) return;
    if (slot >= 0 && slot < RE15_ACTOR_MAX) {
        uint8_t pc = s_prev_clip[slot]; s_prev_clip[slot] = (uint8_t)clip;
        if (pc != (uint8_t)clip) return;               /* clip change: stored prev = old clip -> skip the pop */
    }
    int s_now  = (int)e->anim_frame % c->frame_count;
    int s_prev = (s_now + c->frame_count - 1) % c->frame_count;
    int kf_n = (int)(an->frames[c->first_frame + s_now ] & 0xFFFu);
    int kf_p = (int)(an->frames[c->first_frame + s_prev] & 0xFFFu);
    static re15_skel_pose_t s_pose[RE15_EMD_MAX_BONES];
    int32_t mn[3], mp[3], wn[3], wp[3];
    int16_t rx, ry, rz;
    if (re15_skel_compute_pose(sk, kf_n, s_pose)) return;
    mn[0] = s_pose[bone].trans[0]; mn[1] = s_pose[bone].trans[1]; mn[2] = s_pose[bone].trans[2];
    if (re15_emd_get_keyframe_position(sk, kf_n, &rx, &ry, &rz)) { mn[0] += rx; mn[2] += rz; }
    if (re15_skel_compute_pose(sk, kf_p, s_pose)) return;
    mp[0] = s_pose[bone].trans[0]; mp[1] = s_pose[bone].trans[1]; mp[2] = s_pose[bone].trans[2];
    if (re15_emd_get_keyframe_position(sk, kf_p, &rx, &ry, &rz)) { mp[0] += rx; mp[2] += rz; }
    re15_skel_bone_to_world(mn, e->rot_y, 0, 0, 0, wn);
    re15_skel_bone_to_world(mp, e->rot_y, 0, 0, 0, wp);
    e->x -= (wn[0] - wp[0]);                           /* @0x8011bfe4-e8 */
    e->z -= (wn[2] - wp[2]);                           /* @0x8011c004-08 */
}
static void re15_maggot_bf50(re15_actor_t *e, int a1) { re15_maggot_footlock(e, 14 + 3 * a1); }
static void re15_maggot_c024(re15_actor_t *e, int a1) { re15_maggot_footlock(e,  6 + 4 * a1); }
/* FUN_8001c1a4(hspd, vimp, grav, floorY) — the shared ballistic step (raw disasm @0x8001c1a4):
 * rotate (hspd,0,0) by rot_y and add to x/z (@0x8001c1e8-248), s1 = (s16)(vimp + grav * +0x9c),
 * y -= s1 (@0x8001c258-7c); floor < y -> LAND: y = floor, return s1 (@0x8001c294-b4); else
 * +0x9c++ and return 0 (@0x8001c2a0-b0). +0x9c is the airtime counter. */
static int re15_maggot_ballistic(re15_actor_t *e, int hspd, int vimp, int grav)
{
    if (hspd) re15_dog_advance(e, hspd);
    int32_t s1 = (int32_t)(int16_t)(vimp + grav * (int)e->ai_timer);
    e->y -= s1;
    if ((int32_t)e->dog_floor_y < e->y) { e->y = e->dog_floor_y; return (int)s1; }
    e->ai_timer++;
    return 0;
}

static void re15_maggot_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    if (e->grid_id & 0x20) return;                            /* root skip-tick gate @0x80116df4-f8 */

    switch (e->state) {
    case 0:   /* INIT 0x80116f50 */
        if (e->hp <= 0) e->hp = 180;                          /* +0x9a=180 (HP tab @0x8011f034 row, rng col @0x801170cc-104) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;  /* clear @0x80116f9c-fcc */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be = player @0x80116fd0-ffc */
        e->state = 1;
        e->sub_state_1 = (uint8_t)((e->grid_id & 1) ? 1 : 0);  /* +0x5 = 1 if +0x9&1 (word 0x101 @0x80116f88-8c) */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->ai_timer = 0;                                      /* +0x9c @0x8011702c */
        e->grab_kill_ctr = 0;                                 /* +0x9e @0x8011703c */
        e->dog_blocked_ctr = 0;                               /* +0x1dc attack lockout @0x8011705c */
        e->mag_airborne = 0;                                  /* +0x1e0 @0x8011707c */
        e->mag_pin_cd = 0;                                    /* +0x1e1 @0x8011708c */
        e->mag_boost = 4;                                     /* +0x1e2 = 4 @0x80117098-9c */
        e->mag_1e3 = 0;                                       /* +0x1e3 @0x801170ac */
        e->dog_flags = 0;                                     /* +0x1d0 LOS latch */
        e->dog_floor_y = (int16_t)e->y;                       /* +0x1ba floor Y (engine floor probe; port: spawn Y, dog convention) */
        break;

    case 1: {  /* ACTIVE brain 0x80117254 */
        int los = re15_enemy_los_probe(slot, e, pl);          /* 0x8001bc08 -> +0x1d8 @0x8011725c-70 */
        /* +0x1d0 bit0 = STICKY LOS latch, updated only when the probe verdict is 0/1
         * (@0x80117288-c0: `if ((+0x1d8>>1)==0) { +0x1d0 &= ~1; +0x1d0 |= +0x1d8&1; }`) */
        if ((los >> 1) == 0) e->dog_flags = (uint16_t)((e->dog_flags & ~1u) | (unsigned)(los & 1));
        int32_t dist = re15_enemy_player_dist(e, pl);         /* SquareRoot0 @0x80117300 */
        e->dog_dist = (int16_t)dist;                          /* +0x1d4 @0x80117314 */
        switch (e->sub_state_1) {
        case 0:   /* A[0] 0x80117484 + B[0] idle-wander 0x80117574 (clip 0x16) */
            if (e->dog_blocked_ctr == 0) {                    /* +0x1dc!=0 -> NO decision @0x80117490-98 (audit wf_827f186d maggot #14) */
                if (dist < 5000 && (e->dog_flags & 1)) { re15_dog_sub(e, 3); e->sub_state_3 = 0; break; }  /* @0x801174a8-cc */
                if (dist < 6000 && re15_dog_player_aiming()) { re15_dog_sub(e, 4); e->sub_state_3 = 0; break; }  /* aca58==0x701 grab/aim-assist @0x801174e4-504 (#14; 0x701 = state1-sub7 aim, dog #7 proof) */
                if (dist < 3000) { re15_dog_sub(e, 3); e->sub_state_3 = 1; break; }  /* blind @0x8011753c-68 */
            }
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 59); re15_maggot_clip(e, 0x16); e->sub_state_2 = 1; }  /* +0x9c=rng+59, clip 0x16 @0x801175a4-c8 */
            {   /* unconditional -1 INCLUDING the seed tick (sh in the branch delay slot @0x801175f8-608);
                 * transition at PRE-decrement 0 @0x80117618 -> period exactly rng+59 (audit #18) */
                int16_t t = e->ai_timer; e->ai_timer = (int16_t)(t - 1);
                if (t == 0) { re15_dog_sub(e, 3); e->sub_state_3 = 1; break; }
            }
            re15_maggot_anim(e);
            break;

        case 1:   /* A[1] 0x80117668 + B[1] 0x80117764 — grid&1 dormant spawn (clip 0), wakes to sub 2 */
            if (dist < 5000 && (e->dog_flags & 1)) { re15_dog_sub(e, 2); e->sub_state_3 = 0; break; }  /* @0x80117674-9c */
            if (dist < 6000 && re15_dog_player_aiming()) { re15_dog_sub(e, 2); e->sub_state_3 = 1; break; }  /* @0x801176cc-714 */
            if (dist < 3000) { re15_dog_sub(e, 2); e->sub_state_3 = 2; break; }  /* @0x80117724-58 */
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 59); e->sub_state_2 = 1; re15_maggot_clip(e, 0); }  /* clip 0 @0x80117784-d8 */
            { int16_t t = e->ai_timer; e->ai_timer = (int16_t)(t - 1);
              if (t == 0) { re15_dog_sub(e, 2); e->sub_state_3 = 2; break; } }   /* @0x801177f0-828 */
            re15_maggot_anim(e);
            break;

        case 2:   /* B[2] 0x80117860 RISE/recover (A[2] @0x80117858 = jr ra). +0x7 selects the rise clip. */
            switch (e->sub_state_2) {
            case 0:
                e->sub_state_2 = 1;
                e->anim_frame = 0; e->anim_frac = 7;          /* @0x801178b4-cc */
                e->motion = (int16_t)((e->sub_state_3 == 0) ? 3 : (e->sub_state_3 == 1) ? 2 : 1);  /* clips {3,2,1} @0x801178f4-792c */
                /* fall through */
            case 1:
                if (e->sub_state_3 == 0 && e->anim_frame == 0x1d) re15_audio_room_se(4);   /* Se(4) @0x8011793c-60 */
                if (re15_maggot_anim(e)) e->sub_state_2 = 2;
                re15_maggot_bf50(e, 0);                       /* bf50(0,0) @0x80117988-98 */
                break;
            default:  /* exit @0x801179a8-7a28: +0x7 0->chase / 1->selector / 2->blind chase */
                if (e->sub_state_3 == 1)      { re15_dog_sub(e, 4); e->sub_state_3 = 0; }
                else if (e->sub_state_3 == 2) { re15_dog_sub(e, 3); e->sub_state_3 = 1; }
                else                          { re15_dog_sub(e, 3); e->sub_state_3 = 0; }
                break;
            }
            break;

        case 3: {  /* CHASE — A[3] 0x80117a3c decide + B[3] 0x80117c90 crawl */
            if (pl->hit_react == 0                            /* bite needs player NOT in hit-react @0x80117a54-5c (audit #15) */
                && re15_dog_arc(e, pl, 3000, 384)             /* a804(0xbb8,0x180) @0x80117a60-74 */
                && e->dog_blocked_ctr == 0) {                 /* +0x1dc @0x80117a88-90 */
                re15_dog_sub(e, 5); e->sub_state_3 = 0;       /* BITE @0x80117a94-b8 (clip set by B[5] entry) */
                /* REAR-UP upgrade (audit #1): the committed bite is OVERRIDDEN to sub 15 when the
                 * player is in 2500/cone-256 and both locks are clear @0x80117ab4-b3c:
                 * a9cc(player,0x20) -> +0x1da, proceed on >=0 (@0x80117ac0-ec), a804(0x9c4,0x100)
                 * (@0x80117ae8-b04), +0x1d6==0 (@0x80117b18-20), +0x1e1==0 (@0x80117b28-30). */
                if (re15_maggot_a9cc(e, pl, 0x20) >= 0
                    && re15_dog_arc(e, pl, 2500, 256)
                    && !re15_dog_blocked(e)                   /* +0x1d6 = FUN_8003b0a4 wall clamp (root @0x80116e64-84); port = run_all clamp contact */
                    && e->mag_pin_cd == 0)
                    e->sub_state_1 = 15;                      /* @0x80117b34-3c */
                break;
            }
            /* decision tail @0x80117b40-c74 */
            if (pl->hit_react != 0) {
                if (dist < 3000 && (e->dog_flags & 1) && e->sub_state_3 != 0) { e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* blind->sighted re-entry @0x80117b54-9c */
            } else if (dist < 6001) {
                if ((e->dog_flags & 1) && e->sub_state_3 != 0) { e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* @0x80117bb4-c08 */
            } else if (e->dog_flags & 1) {
                re15_dog_sub(e, 4); e->sub_state_3 = 0; break;   /* far+LOS -> SELECTOR @0x80117c2c-74 */
            }
            /* B[3] crawl */
            if (e->sub_state_2 == 0) {
                e->sub_state_2 = 1;                           /* @0x80117cac-b0 */
                re15_maggot_clip(e, 4);                       /* provisional clip 4 @0x80117cc0 */
                if (e->sub_state_3 == 0) {                    /* SIGHTED -> clip 5 (audit #3; @0x80117cd8 bne +0x7; clip 5 delay-slot store @0x80117cdc-e4) */
                    e->motion = 5;
                    e->grab_kill_ctr = (int16_t)((re15_engine_rand8() & 0x1f) + 2);   /* +0x9e seed @0x80117ce8-cf8 */
                }
            }
            {   /* fresh random slew 6-37 EVERY tick (audit #5) @0x80117d1c-34, a8f8 @0x80117d50 */
                int slew = (int)(re15_engine_rand8() & 0x1f) + 6;
                e->grab_kill_ctr = (int16_t)slew;
                re15_enemy_steer_point(e, pl->x, pl->z, slew);
            }
            re15_maggot_anim(e);
            {   /* locator root-motion, window <0x18 (sighted clip 5) / <0x29 (blind clip 4)
                 * @0x80117d90-e04; footstep Se(8) at frame 1 and the window frame @0x80117dc4-e2c */
                int win = (e->sub_state_3 == 0) ? 0x18 : 0x29;
                re15_maggot_bf50(e, ((int)e->anim_frame < win) ? 1 : 0);
                if (e->anim_frame == 1 || (int)e->anim_frame == win) re15_audio_room_se(8);
            }
            break; }

        case 4: {  /* SELECTOR — A[4] 0x80117e40 decide + B[4] 0x80118110 heavy-approach (clip 6) */
            /* abort: player in hit-react + LOS -> CHASE @0x80117e44-70 -> 0x801180d4 (audit #10:
             * there is NO distance-based abandon anywhere in A[4]) */
            if (pl->hit_react != 0 && (e->dog_flags & 1)) { re15_dog_sub(e, 3); e->sub_state_3 = 0; break; }
            if (pl->hit_react == 0 && re15_dog_arc(e, pl, 4000, 0xc0) && e->dog_blocked_ctr == 0) {
                re15_dog_sub(e, 6); e->sub_state_3 = 0; break;   /* HEAVY @0x80117e88-ec4 (clip by B[6] entry) */
            }
            /* OPEN (subsystem gap, audit #11): the Path-A ZONE leaps @0x80117ecc-8011802c —
             * +0x1e2!=0 (@0x80117ed4-dc, always 4) -> FUN_8003b93c(+0x34, hw+100, +0x82, a3=0x10)
             * (@0x80117efc) then a3=0x20 (@0x80117f98, gated !(+0x1d0&1) @0x80117fbc); on a match the
             * yaw window ((+0x90&0xf0)<<4 - +0x6a + 512)&0xfff < 1024 (@0x80117f20-38/0x80117fc8-e4)
             * latches +0x9f=+0x90 (@0x80117f40/0x80117ff0) and commits +0x5=7 with +0x7=1 (attr 0x10,
             * @0x80117f50-74) or +0x7=3 (attr 0x20 blind, @0x80118000-24; launch impulse 0x32a
             * @0x80118a94-aa0, yaw snap (+0x9f&0xf0)<<4 + +0x82=1 @0x80118ac0-af0). The port has
             * neither the b93c SCA-attr zone query nor the FUN_8003b0a4 +0x90 escape-heading writer
             * (actor.h: port writer deferred) — the B[7] +0x7-variant launch code below is in place
             * for when they land. */
            /* Path-B far leap @0x80118028-8100 */
            if ((e->dog_flags & 1)                            /* +0x1d0&1 @0x80118034-40 */
                && !re15_dog_blocked(e)                       /* +0x1d6==0 @0x80118054-5c — the WALL-CLAMP gate, NOT +0x1dc (audit #15) */
                && dist >= 6001                               /* @0x8011806c */
                && re15_maggot_a9cc(e, pl, 0x20) == 0         /* aimed +-32 @0x80118084-8c */
                && (re15_dog_player_aiming()                  /* aca58==0x701 bypasses the coin-flip @0x80118090-a0 (audit #15) */
                    || (re15_engine_rand8() & 1))             /* rng&1 @0x801180a8-b4 */
                && re15_maggot_a780(e, pl) == 0) {            /* player faces the maggot @0x801180bc-c4 */
                re15_dog_sub(e, 7); e->sub_state_3 = 0; break;   /* @0x801180cc-f8 (clip by B[7] entry) */
            }
            /* B[4] heavy-approach (audit #4) */
            if (e->sub_state_2 == 0) { e->sub_state_2 = 1; re15_maggot_clip(e, 6); }   /* clip 6 @0x80118128-60 */
            e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180);   /* +0x8c = 180-211 per tick @0x80118164-80 */
            {   int slew = (int)(re15_engine_rand8() & 0x1f) + 64;           /* +0x9e = 64-95 @0x80118184-94 */
                if (dist < 2000) slew += 24;                                 /* +24 inside 2000 @0x801181a4-c4 */
                e->grab_kill_ctr = (int16_t)slew;
                re15_enemy_steer_point(e, pl->x, pl->z, slew);               /* a8f8 @0x801181e0 */
            }
            re15_maggot_anim(e);
            if (((int)e->anim_frame % 13) == 1) re15_audio_room_se(8);       /* footstep every 13 @0x80118204-4c */
            re15_dog_advance_ofs(e, 0);                                      /* 245d8(0) @0x80118250 */
            break; }

        case 5: {  /* BITE B[5] 0x80118270 (A[5] @0x80118268 = jr ra), clip 0x12, -6 */
            switch (e->sub_state_2) {
            case 0:
                e->sub_state_2 = 1; re15_maggot_clip(e, 0x12);   /* @0x801182d4-304 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) e->sub_state_2 = 2;     /* +0x6 += anim ret @0x80118308-40 */
                re15_maggot_bf50(e, 0);                          /* bf50(0,0) @0x8011832c-40 */
                if (e->anim_frame == 0x0b) re15_audio_room_se(9);   /* whoosh Se(9) @0x80118394-a8 (audit #17) */
                if (pl->hit_react == 0
                    && e->anim_frame >= 0x0c && e->anim_frame <= 0x0f   /* window = ANY of table {0c,0d,0e,0f} @0x8012146c, loop @0x80118400-28 (audit #17: no per-attack latch — hit_react gates) */
                    && re15_dog_arc(e, pl, re15_body_contact_reach(e), 384)) {   /* bff8 part+0x64c r=0x3e8 @0x801183c4-d0; port mapping audit wf_555f18eb Part B */
                    pl->hp = (int16_t)(pl->hp - 6);              /* @0x80118460-6c */
                    e->dog_blocked_ctr = 0x2d;                   /* +0x1dc=45 @0x80118470-78 */
                    re15_audio_room_se(6);                       /* Se(6) @0x80118474 (a0=6 @0x80118454) */
                    pl->hit_react |= 1;                          /* @0x801184c0-d4. The stagger cmd aca58=2/aca59=facing+2 (hp<0 -> 3)
                                                                  * @0x8011847c-b8 = the player-command FSM; port convention = hit_react (audit #17, dog/crow parity) */
                }
                break;
            default:  /* exit @0x801184e0-8520: lockout 0x14 if clear, -> CHASE */
                if (e->dog_blocked_ctr == 0) e->dog_blocked_ctr = 0x14;   /* @0x801184e0-f0 */
                re15_dog_sub(e, 3); e->sub_state_3 = 0;
                break;
            }
            break; }

        case 6: {  /* HEAVY B[6] 0x8011854c (A[6] @0x80118544 = jr ra), clip 0x13, -12.
                    * NO steer call exists in 0x8011854c-0x801188f8 (audit #9) — the lunge IS the
                    * clip's locator root-motion. */
            switch (e->sub_state_2) {
            case 0:
                e->sub_state_2 = 1; re15_maggot_clip(e, 0x13);   /* @0x801185b4-e4 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) e->sub_state_2 = 2;     /* @0x801185e8-61c */
                if ((int)e->anim_frame < 6)         re15_maggot_c024(e, 0);   /* frames <6: c024(0,0) @0x80118634-44 */
                else if ((int)e->anim_frame < 0x15) re15_maggot_bf50(e, 1);   /* 6..0x14: bf50(0,1) — @0x80118650 `sltiu v0,v1,0x15` is the MOVE gate (audit #9 wrong-citation fix) */
                else                                re15_maggot_bf50(e, 0);   /* >=0x15: bf50(0,0) @0x8011865c-68 */
                if (e->anim_frame == 0x14) re15_audio_room_se(9);   /* Se(9) @0x80118678-8c */
                if (pl->hit_react == 0) {
                    int fr = (int)e->anim_frame;
                    int inwin = (fr >= 0x15 && fr <= 0x18) || fr == 0x21 || fr == 0x22;   /* window table @0x80121470 = {15,16,17,18,21,22} (audit #9) — incl. the 2nd chomp; NO connect-once latch */
                    if (inwin && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0xc0)) { /* dual bff8 +0x448/+0x6f8 r=0x320 @0x801186f0-714; port mapping wf_555f18eb */
                        pl->hp = (int16_t)(pl->hp - 12);         /* @0x801187b4-c0 */
                        e->dog_blocked_ctr = 0x2d;               /* @0x801187c4-cc */
                        re15_audio_room_se(5);                   /* Se(5) @0x801187c8 (a0=5 @0x80118798) */
                        pl->hit_react |= 1;                      /* @0x80118814-28 */
                        /* KNOCKDOWN-Klasse statt Flinch-Degradation (player_knockdown.md KD-3/F1,
                         * CONFIRMED — dieser Heavy-Biss gehoert dem 0x27-BOSS): aca58=2 +
                         * aca59 = a780(facing) + 4 (@0x801187d0-f8, jal a780 @0x801187dc) ->
                         * EXE-Handler [4] (vorn, 0x800360e8) / [5] (hinten, 0x8003644c).
                         * HP<0 -> Death hat Vorrang (@0x801187fc-8810, bestehender Pfad). */
                        if (pl->hp >= 0)
                            re15_player_knockdown_begin(re15_maggot_a780(e, pl));
                    }
                }
                break;
            default:  /* exit @0x80118834-8b8 (audit #8): re-BITE / SELECTOR / chase */
                if (pl->hit_react == 0 && re15_dog_arc(e, pl, 4000, 384)) {   /* a804(0xfa0,0x180) @0x80118848-58 */
                    if (e->dog_blocked_ctr == 0) { re15_dog_sub(e, 5); e->sub_state_3 = 0; break; }   /* chain re-BITE @0x8011887c-80 */
                    re15_dog_sub(e, 3); e->sub_state_3 = 0; break;            /* @0x80118874-78 */
                }
                if (e->dog_blocked_ctr == 0) { e->dog_blocked_ctr = 0x14; re15_dog_sub(e, 4); e->sub_state_3 = 0; break; }   /* +0x1dc=0x14 -> SELECTOR @0x80118898-b0 */
                re15_dog_sub(e, 3); e->sub_state_3 = 0;                       /* @0x801188b4 */
                break;
            }
            break; }

        case 7: {  /* LEAP B[7] 0x80118908 (A[7] @0x80118900 = jr ra), clip 0x14 — ballistic
                    * REPOSITION pounce, 0 damage (wf_c1de93d6-bae). */
            switch (e->sub_state_2) {
            case 0:   /* entry @0x8011895c-89b4 */
                e->sub_state_2 = 1;
                re15_maggot_clip(e, 0x14);
                e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180);   /* +0x8c windup crawl @0x80118994-a4 (audit #6) */
                e->ai_timer = 0;                                                 /* +0x9c @0x801189b4 */
                /* fall through */
            case 1:   /* WINDUP @0x801189b8: frames 0-9 CRAWL FORWARD at +0x8c (245d8(0) @0x80118dbc, audit #6) */
                if (e->sub_state_3 != 0) {                    /* zone variant: slew toward (+0x9f&0xf0)<<4 at 0x20 (0x8001aa68 @0x801189cc-a00) */
                    int tgt = ((int)((uint8_t)e->dog_aux9f & 0xf0) << 4) & 0xfff;
                    int df = (((tgt - (int)e->rot_y) + 0x800) & 0xfff) - 0x800;
                    if (df >  0x20) df =  0x20;
                    if (df < -0x20) df = -0x20;
                    e->rot_y = (int16_t)(((int)e->rot_y + df) & 0xfff);
                }
                re15_maggot_anim(e);
                if (e->anim_frame == 0x0a) {                  /* LAUNCH at frame 10 @0x80118a2c-40 */
                    e->sub_state_2 = 2;
                    e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 200   /* @0x80118a44-54 */
                                              + (int)e->mag_boost * 10);           /* += +0x1e2*10 = +40 @0x80118a64-80 (audit #6) */
                    if (e->sub_state_3 == 3) e->crow_speed = 0x32a;                /* blind zone leap @0x80118a94-aa0 */
                    e->mag_airborne = 1;                                           /* +0x1e0=1 @0x80118aa4-b0 */
                    if (e->sub_state_3 != 0) {                                     /* zone: yaw snap + floor byte @0x80118ab4-af0 */
                        e->rot_y = (int16_t)((((int)((uint8_t)e->dog_aux9f & 0xf0)) << 4) & 0xfff);
                        e->floor = 1;
                    }
                    e->hit_react |= 1;                                             /* self hit-guard (damage-immune in flight) @0x80118af4-b10 */
                }
                re15_dog_advance_ofs(e, 0);                   /* 245d8(0) @0x80118dbc (windup + launch tick) */
                break;
            case 2: {  /* IN-FLIGHT @0x80118b14 */
                if ((e->hit_react & 0x02) && (e->hit_react & 0x40) && e->sub_state_3 == 0) {
                    /* shot down by a crit-class weapon (magnum/close shotgun sets +0x93|=0x40,
                     * FUN_80011f50 LAB_80012370) -> CRASH: state 2 sub 9 @0x80118b14-74 (audit #7) */
                    e->ai_timer = 0;
                    e->state = 2; e->sub_state_1 = 9; e->sub_state_2 = 3; e->sub_state_3 = 0;
                    break;
                }
                if (e->hit_react & 0x02) {                    /* non-crit shot in flight: gore puff only @0x80118b78-bd0 */
                    e->hit_react &= 0xfd; re15_maggot_gore(e);
                }
                re15_maggot_anim(e);
                {   int vimp = 600 + (int)e->mag_boost * 30;  /* +0x1d8 = +0x1e2*30+600 = 720 @0x80118bf4-c18 */
                    if (e->sub_state_3 == 3) vimp = 0x2d0;    /* @0x80118c1c-38 */
                    if (re15_maggot_ballistic(e, e->crow_speed, vimp, -60) != 0) {   /* c1a4(+0x8c,vimp,-60,+0x1ba) @0x80118c3c-58 */
                        e->sub_state_2 = 3;                   /* land @0x80118c64-70 */
                        e->crow_speed = 100;                  /* +0x8c=0x64 @0x80118c7c-84 */
                        re15_audio_room_se(2);                /* Se(2) @0x80118c80 */
                        e->mag_airborne = 0; e->floor = 0; e->sub_state_3 = 0;
                        e->hit_react = 0;                     /* +0x93=0 @0x80118cc4 */
                        break;
                    }
                }
                /* mid-air FINISHER commit @0x80118cc8-d68 (audit #7): frame 0x13, player.hp<50,
                 * hit-react clear, a804(0x1388,0xc0), +0x1dc==0 -> sub 8 + Se(7) */
                if (e->anim_frame == 0x13 && pl->hp < 50 && pl->hit_react == 0
                    && re15_dog_arc(e, pl, 5000, 0xc0) && e->dog_blocked_ctr == 0) {
                    re15_dog_sub(e, 8); e->sub_state_3 = 0;
                    re15_audio_room_se(7);                    /* @0x80118d64-68 */
                    break;
                }
                re15_dog_advance_ofs(e, 0);                   /* the flight DOUBLE-advance: c1a4 + 245d8(0) @0x80118dbc */
                break; }
            default:  /* phase 3 landing tail @0x80118d74-dc0: play out, slide at +0x8c=100, -> SELECTOR */
                if (re15_maggot_anim(e)) { re15_dog_sub(e, 4); e->sub_state_3 = 0; }
                else re15_dog_advance_ofs(e, 0);
                break;
            }
            break; }

        case 8: {  /* mid-air FINISHER B[8] 0x80118ddc (A[8] @0x80118dd4 = jr ra), clip 0x15 (audit #7).
                    * Entered mid-leap; +0x8c (flight speed) carries over — B[8] entry seeds none. */
            switch (e->sub_state_2) {
            case 0:   /* @0x80118e44-ea0 */
                e->sub_state_2 = 1;
                re15_maggot_clip(e, 0x15);
                e->mag_airborne = 1;                          /* @0x80118e78-84 */
                e->hit_react |= 1;                            /* @0x80118e88-a0 */
                /* fall through */
            case 1: {
                if ((e->hit_react & 0x02) && (e->hit_react & 0x40) && e->sub_state_3 == 0) {
                    /* crit-shot mid-finisher -> crash-DEATH lane: state 3 sub 9 @0x80118eb8-f0c */
                    e->ai_timer = 0;
                    e->state = 3; e->sub_state_1 = 9; e->sub_state_2 = 3; e->sub_state_3 = 0;
                    break;
                }
                if (e->hit_react & 0x02) { e->hit_react &= 0xfd; re15_maggot_gore(e); }   /* @0x80118f10-64 */
                re15_maggot_anim(e);
                {   int vimp = 600 + (int)e->mag_boost * 30;  /* @0x80118f84-fa8 */
                    if (e->sub_state_3 == 3) vimp = 0x2d0;    /* @0x80118fac-c8 */
                    if (re15_maggot_ballistic(e, e->crow_speed, vimp, -60) != 0) {   /* @0x80118fcc-e8 */
                        e->sub_state_2 = 2;                   /* land -> recovery @0x80118ff4-9000 */
                        e->crow_speed = 100; re15_audio_room_se(2);
                        e->mag_airborne = 0; e->floor = 0; e->sub_state_3 = 0;
                        e->hit_react = 0;                     /* @0x80119054 */
                        break;
                    }
                }
                if (e->anim_frame == 7) re15_audio_room_se(9);   /* Se(9) @0x80119064-78 */
                if (pl->hit_react == 0
                    && (e->anim_frame == 8 || e->anim_frame == 9 || e->anim_frame == 0x0a)   /* window table @0x80121478 = {08,09,0a}, loop @0x80119120-5c */
                    && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0xc0)) {   /* dual bff8 +0x448/+0x6f8 r=0x320 @0x801190dc-9100 */
                    pl->hp = (int16_t)(pl->hp - 600);         /* player.hp -= 600 @0x80119198-ac */
                    e->dog_blocked_ctr = 0x12c;               /* +0x1dc=300 @0x801191b0-b8 */
                    re15_audio_room_se(1);                    /* Se(1) @0x801191b4 (a0=1 @0x80119184) */
                    s_player_grabbed = 1;
                    re15_player_victim_devour(e);             /* aca58=6 (sw @0x801191c8-cc) + acbfc/acbcc/acbd0 victim banks @0x801191d8-9204 */
                    pl->hit_react |= 1;                       /* @0x801191d0-fc */
                }
                break; }
            default:  /* phase 2 recovery @0x80119210-5c: play out (245d8(0) while running) -> SELECTOR */
                if (re15_maggot_anim(e)) { re15_dog_sub(e, 4); e->sub_state_3 = 0; }
                else re15_dog_advance_ofs(e, 0);
                break;
            }
            break; }

        case 15: {  /* REAR-UP GRAB/PIN — A[15] 0x8011a878 + B[15] 0x8011a960, phases @0x801003cc
                     * = {a9a8,aa24,abe8,acb4,ad10,ad98,add8,ae5c} (audit #1). */
            /* A[15]: shot during the rear-up/pin */
            if (e->hit_react & 0x02) {
                if (e->hit_react & 0x40) {                    /* crit-shot -> abort to HURT air lane @0x8011a898-8fc */
                    e->y = (int32_t)e->dog_floor_y;           /* restore ground @0x8011a8a4-ac */
                    e->ai_timer = 0;                          /* +0x9c=0 @0x8011a8bc */
                    e->state = 2; e->sub_state_1 = 7; e->sub_state_2 = 1; e->sub_state_3 = 0;   /* @0x8011a8c8-fc */
                    if (g_player_victim == 1 || g_player_victim == 2) s_victim_phase = 4;   /* port: release the pin */
                    break;
                }
                e->hit_react &= 0xfd; re15_maggot_gore(e);    /* gore puff @0x8011a900-4c */
            }
            switch (e->sub_state_2) {
            case 0:   /* REAR-UP @0x8011a9a8 */
                e->y = (int32_t)e->dog_floor_y + 1600;        /* +0x38 = +0x1ba + 0x640 @0x8011a9b4-c0 */
                e->sub_state_2 = 1;
                e->motion = 0x1c; e->anim_frame = 0; e->anim_frac = 0;   /* clip 0x1c, +0x8f=0 hard cut @0x8011a9dc-aa04 */
                re15_audio_room_se(7);                        /* Se(7) @0x8011a9fc-aa00 */
                e->hit_react |= 1;                            /* self hit-guard @0x8011aa14-20 */
                /* fall through */
            case 1: {  /* connect checks @0x8011aa24-abe4 */
                re15_maggot_anim(e);
                int connected = 0;
                if (e->anim_frame == 4) {                     /* @0x8011aa4c-54 */
                    if (re15_maggot_a780(e, pl) == 0          /* a780==0 gate @0x8011aa64-6c */
                        && pl->hit_react == 0                 /* @0x8011aac0-cc */
                        && re15_dog_arc(e, pl, 3000, 256))    /* a804(0xbb8,0x100) -> +0x1d8 @0x8011aaa0-bc */
                        connected = 1;
                } else if (e->anim_frame == 0x0f) {           /* @0x8011aafc-b04 */
                    if (re15_maggot_a780(e, pl) != 0          /* a780!=0 gate @0x8011ab14-1c */
                        && pl->hit_react == 0                 /* @0x8011ab7c-88 */
                        && re15_dog_arc(e, pl, re15_body_contact_reach(e), 384))   /* bff8 part+0x39c r=0x320 @0x8011ab24-78; port mapping wf_555f18eb */
                        connected = 1;
                    if (!connected) {                         /* MISS -> phase 7 reverse-play @0x8011abb4-e4 */
                        e->sub_state_2 = 7; e->anim_frame = 0x23;
                        break;
                    }
                }
                if (connected) e->sub_state_2 = 2;            /* @0x8011aba8-b0 */
                break; }
            case 2:   /* PIN latch @0x8011abe8 */
                re15_audio_room_se(10);                       /* Se(10) @0x8011abe8-f8 */
                e->sub_state_2 = 3;                           /* @0x8011abf4-fc */
                s_player_grabbed = 1;                         /* aca58=5 @0x8011ac40-48 (+ entity/player flags|=0x1000 @0x8011ac2c-54) */
                re15_player_victim_latch(e, pl);              /* player anchor 0x8001ac38 @0x8011ac18 + a8f8(player,0x800) yaw latch @0x8011acac-b0 */
                g_player_victim_variant = (uint8_t)re15_maggot_a780(e, pl);   /* aca59 = a780 ret @0x8011ac50-68 */
                pl->hit_react |= 1;                           /* @0x8011ac8c-aa0 */
                if (re15_maggot_anim(e)) e->sub_state_2 = 4;  /* anim + (+0x6 += ret) @0x8011acd4-d0c */
                break;
            case 3:   /* RIDE @0x8011acb4: hold the pin (0x8001ad68 root-place) until clip 0x1c ends */
                s_player_grabbed = 1;
                if (re15_maggot_anim(e)) e->sub_state_2 = 4;
                break;
            case 4:   /* RELEASE init @0x8011ad10 */
                e->hit_react = 0;                             /* +0x93=0 @0x8011ad1c */
                e->y = (int32_t)e->dog_floor_y;               /* @0x8011ad2c-34 */
                e->sub_state_2 = 5;                           /* @0x8011ad44 */
                e->motion = 3; e->anim_frame = 0x16; e->anim_frac = 0;   /* clip 3 @frame 0x16 @0x8011ad50-78 */
                re15_audio_room_se(4);                        /* Se(4) @0x8011ad60/74 */
                s_victim_phase = 4;                           /* unpin: flags &= ~0x1000 @0x8011ad88-94 (port: victim release phase) */
                /* fall through */
            case 5:   /* release play-out @0x8011ad98 */
                re15_maggot_anim(e);
                if (e->anim_frame == 0x31) e->sub_state_2 = 6;   /* @0x8011adc0-d4 */
                break;
            case 6:   /* connect exit @0x8011add8: lockouts + hand off to the sub-2 recovery */
                e->dog_blocked_ctr = 0x3c;                    /* +0x1dc=0x3c @0x8011ade0-e4 */
                e->mag_pin_cd = 0xff;                         /* +0x1e1=0xff @0x8011adf0-f4 */
                e->sub_state_1 = 2; e->sub_state_2 = 1; e->sub_state_3 = 0;   /* @0x8011ae04-24 */
                e->motion = 3; e->anim_frame = 0x1d; e->anim_frac = 7;        /* clip 3 @frame 0x1d @0x8011ae30-58 */
                break;
            default:  /* 7: MISS reverse-play @0x8011ae5c (f314 a2=1) then restore -> CHASE */
                if (e->anim_frame > 0) e->anim_frame--;
                else {
                    e->hit_react = 0;                         /* @0x8011ae8c */
                    e->y = (int32_t)e->dog_floor_y;           /* @0x8011ae9c-a4 */
                    e->motion = 6; e->anim_frame = 0; e->anim_frac = 0;   /* clip 6 @0x8011aeb0-d4 */
                    e->dog_blocked_ctr = 0x1e;                /* +0x1dc=0x1e @0x8011aefc-f00 */
                    e->mag_pin_cd = 0x5a;                     /* +0x1e1=0x5a @0x8011af0c-10 */
                    re15_dog_sub(e, 3); e->sub_state_3 = 0;   /* @0x8011af1c-40 */
                }
                break;
            }
            break; }

        default:  /* subs 9-14 (B @0x8011936c/971c/9a6c/9d0c/a1f8/a44c) — no shipped writer reaches
                   * them (+0x1e3 stays 0, zone leaps OPEN); fold to the brain */
            e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
            break;
        }
        /* brain TAIL (audit #16): the timers count down EVERY brain tick AFTER the A/B dispatch —
         * +0x1dc @0x801173f8-40c, +0x1e1 @0x8011741c-38. (+0x1de blocked-frames counter
         * @0x80117448-6c has no consumer in the shipped subs.) */
        if (e->dog_blocked_ctr) e->dog_blocked_ctr--;
        if (e->mag_pin_cd) e->mag_pin_cd--;
        break;
    }

    case 2: {  /* HURT root 0x8011af5c (audit #2). take_damage/weapon-fire left +0x5 = the weapon
                * reaction lane, +0x6 = hit-dir, +0x7 = 0 (phase). */
        if (e->mag_airborne) {                                /* ballistic settle c1a4(0,0,-50,+0x1ba) @0x8011af6c-88 */
            if (re15_maggot_ballistic(e, 0, 0, -50) != 0) {
                e->mag_airborne = 0;                          /* @0x8011afa0 */
                re15_audio_room_se(2);                        /* Se(2) @0x8011af90/cc (+ box restore @0x80121368[+0x1e2] — port hitbox is static) */
            }
        }
        if (e->sub_state_1 < 7) {                             /* lanes @0x801214a8: [0-6] = 0x8011b018 FLINCH */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2;                            /* @0x8011b064-70 */
                e->sub_state_3 = 1;                           /* @0x8011b07c-80 */
                e->motion = 7; e->anim_frame = 0; e->anim_frac = 7;   /* flinch clip 7 @0x8011b08c-90 */
                re15_maggot_gore(e);                          /* vec @0x80121388[(+0x6&1)*0x20] @0x8011b0bc-130 */
                re15_audio_room_se(3);                        /* Se(3) @0x8011b134-38 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) e->sub_state_3 = 2;  /* @0x8011b13c-74 */
                break;
            default:  /* exit @0x8011b178-1d8: -> ACTIVE sub 7 = RETALIATION LEAP (sub 9 if +0x1e3) */
                e->hit_react = 0;                             /* @0x8011b178 */
                e->state = 1;                                 /* sb 1 @0x8011b188 */
                e->sub_state_1 = (uint8_t)((e->mag_1e3 != 0) ? 9 : 7);   /* @0x8011b194-98 / @0x8011b1c8-d8 */
                e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            }
        } else if (e->sub_state_1 < 9) {                      /* [7-8] = 0x8011b1ec AIR-HIT lane */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2; e->sub_state_3 = 1;        /* @0x8011b238-54 */
                e->motion = (int16_t)((e->hit_react & 0x80) ? 9 : 8);   /* clip 8 @0x8011b260-64, 9 if front-latch @0x8011b274-8c */
                e->anim_frame = 0; e->anim_frac = 7;
                re15_maggot_gore(e); re15_audio_room_se(3);   /* @0x8011b2b0-334 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) e->sub_state_3 = 2;  /* @0x8011b338-6c */
                re15_maggot_bf50(e, (e->motion == 8) ? 0 : 1);   /* @0x8011b370-a0 */
                break;
            default:  /* @0x8011b3ac-ec */
                e->hit_react = 0; e->state = 1; e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            }
        } else {                                              /* [9-11] = 0x8011b400 CRASH lane, phases @0x801003ec */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2; e->sub_state_3 = 1;        /* @0x8011b44c-68 */
                e->motion = (int16_t)((e->hit_react & 0x80) ? 0x0b : 0x0a);   /* @0x8011b474-9c */
                e->anim_frame = 0; e->anim_frac = 7;
                e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 80);   /* +0x8c @0x8011b4c4-d4 */
                e->ai_timer = 0;                              /* +0x9c=0 @0x8011b4e4 */
                re15_maggot_gore(e); re15_audio_room_se(3);   /* @0x8011b4f0-56c */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) e->sub_state_3 = 2;  /* @0x8011b570-5a4 */
                e->crow_speed = (int16_t)(0x50 - ((int)e->ai_timer << 2));   /* +0x8c = 0x50 - 4*+0x9c @0x8011b5b4-c8 */
                re15_dog_advance_ofs(e, 0x800);               /* BACK-slide 245d8(0x800) @0x8011b5c4 */
                if (e->hit_react & 0x80) re15_dog_advance_ofs(e, 0);   /* front-latch: extra 245d8(0) @0x8011b5d8-f0 */
                break;
            case 2:
                e->sub_state_3 = 3;                           /* @0x8011b604-08 */
                e->motion = (int16_t)((e->hit_react & 0x80) ? 0x11 : 0x10);   /* get-up @0x8011b614-3c */
                e->anim_frame = 0; e->anim_frac = 7;
                /* fall through */
            case 3:
                if (re15_maggot_anim(e)) e->sub_state_3 = 4;  /* @0x8011b660-98 */
                break;
            default:  /* @0x8011b69c-e8 */
                e->hit_react = 0; e->state = 1; e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            }
        }
        break; }

    case 3: {  /* DEATH root 0x8011b6fc: airborne settle @0x8011b70c-70, lanes @0x80121500 (audit #13) */
        if (e->mag_airborne) {
            if (re15_maggot_ballistic(e, 0, 0, -50) != 0) { e->mag_airborne = 0; re15_audio_room_se(2); }
        }
        if (e->sub_state_1 < 7) {                             /* [0-6] ground death 0x8011b7b8 */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2; e->sub_state_3 = 1;        /* @0x8011b804-20 */
                e->motion = 0x0e; e->anim_frame = 0; e->anim_frac = 7;   /* clip 0xe @0x8011b82c-30 */
                re15_maggot_gore(e);                          /* @0x8011b854-8d0 */
                re15_audio_room_se(0);                        /* Se(0) @0x8011b8d4-d8 */
                /* kill-flag persist 0x8004ef90(0x800b1038, +0x1c6) @0x8011b8e8-f4 — port: the run_all
                 * em_flag commit */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) { e->sub_state_3 = 2; e->anim_frame = s_maggot_clip_len[0x0e] - 1; }  /* hold the fallen frame */
                re15_maggot_bf50(e, 0);                       /* bf50(0,0) @0x8011b918-34 */
                if (e->anim_frame == 0x3d) re15_audio_room_se(2);   /* frame-61 thud Se(2) @0x8011b944-58 */
                break;
            default:  /* @0x8011b964-88: settle gore + STATE WORD 7 */
                re15_maggot_gore(e);
                e->state = 7; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sw 7 @0x8011b984-88 */
                break;
            }
        } else if (e->sub_state_1 < 9) {                      /* [7-8] mid-air death 0x8011b998 */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2; e->sub_state_3 = 1;        /* @0x8011b9e4-a00 */
                e->motion = (int16_t)((e->hit_react & 0x80) ? 0x0b : 0x0a);   /* clip 0xa/0xb @0x8011ba0c-38 */
                e->anim_frame = 0; e->anim_frac = 7;
                re15_maggot_gore(e); re15_audio_room_se(0);   /* @0x8011ba5c-ae0 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) { e->sub_state_3 = 2; e->anim_frame = s_maggot_clip_len[e->motion & 0x1f] - 1; }
                if (e->anim_frame == 0x13) re15_audio_room_se(2);   /* Se(2) @0x8011bb48-5c */
                break;
            default:  /* @0x8011bb68-88: gore(part +0x244) + state word 7 */
                re15_maggot_gore(e);
                e->state = 7; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            }
        } else {                                              /* [9] crash-death 0x8011bb9c (crit-shot finisher) */
            switch (e->sub_state_3) {
            case 0:
                e->hit_react |= 2; e->sub_state_3 = 1;        /* @0x8011bbe8-c04 */
                e->motion = (int16_t)((e->hit_react & 0x80) ? 0x0b : 0x0a);   /* @0x8011bc10-38 */
                e->anim_frame = 0; e->anim_frac = 7;
                e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 80);   /* @0x8011bc60-70 */
                e->ai_timer = 0;                              /* @0x8011bc80 */
                re15_maggot_gore(e); re15_audio_room_se(0);   /* @0x8011bc84-d08 */
                /* fall through */
            case 1:
                if (re15_maggot_anim(e)) { e->sub_state_3 = 2; e->anim_frame = s_maggot_clip_len[e->motion & 0x1f] - 1; }
                e->crow_speed = (int16_t)(0x50 - ((int)e->ai_timer << 2));   /* @0x8011bd70-84 */
                re15_dog_advance_ofs(e, 0x800);               /* back-slide @0x8011bd80 */
                if (e->hit_react & 0x80) re15_dog_advance_ofs(e, 0);   /* @0x8011bd94-ac */
                break;
            default:  /* @0x8011bdb8-d8 */
                re15_maggot_gore(e);
                e->state = 7; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
                break;
            }
        }
        break; }

    case 7:   /* CORPSE 0x8011be54 (audit #12): NO clip write — hold the death pose; 90-tick fade */
        if (e->sub_state_3 == 0) {
            e->grab_kill_ctr = 0x5a;                          /* +0x9e = 90 @0x8011be80-84 */
            e->sub_state_3 = 1;                               /* @0x8011be90-94 */
            e->flags |= 0x02;                                 /* @0x8011beac-b0 */
            e->flags |= 0x40;                                 /* @0x8011bec8-cc */
        }
        if (e->sub_state_3 == 1) {
            /* per tick: +0xc4/+0xec = (x & 0xff000000) | 0x00ffff38 corpse tint and pool spread
             * +0xbc/+0xbe += 8 @0x8011bed0-bf14 — model-pool color/pool channel, render-side (the
             * same channel as the zombie corpse pool, re15_enemy_corpse_settle) */
            int16_t t = e->grab_kill_ctr; e->grab_kill_ctr = (int16_t)(t - 1);   /* unconditional dec @0x8011bf24-34 */
            if (t == 0) e->sub_state_3 = 2;                   /* @0x8011bf38-44 */
        }
        /* +0x7 >= 2: fully inert @0x8011bf48 (frozen on the death clip's last frame) */
        break;

    default:  /* states 4/5 (0x8011bdec — sub-dispatch @0x80121558, shipped entries are jr-ra stubs
               * @0x8011be34-4c) and 6 (0x8011c598) have no STAGE1 writer -> resume the brain */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ NPCs (Irons/Sherry/Annette/Katherine) — Wave 1 ================ *
 * Byte-true 0x8011c5a0 family (RE15_NPC_AI.md; workflow wf_69c86050, adversarially verified). The STAGE1
 * NPCs (0x40/0x42 Chief Irons, 0x45 Sherry, 0x47/0x49 Annette, 0x4b Katherine) are INVULNERABLE cutscene
 * actors (HP = -1) that idle-pose, walk (nav-steer), look-at the player, and (Katherine) trigger dialogue.
 * They share a common EXE behaviour library: state [4] 0x80050be8 = a nested sub-dispatcher on +0x5
 * (@0x80076ca0: idle-pose 0-3 / walk-to-target 4/5/7/8 / turn-look 9), states [6]/[7] = event-watchers.
 * WAVE 1: the Irons (0x40) INIT + the idle-pose hold (the NPC stands + animates, invulnerable). WAVE 2
 * IMPLEMENTED: the walk-to-target (re15_npc_sub_walk @L4036) + look-at (re15_npc_sub_turn @L4009), both
 * dispatched by re15_npc_sub_dispatch @L4080-4082, and all 7 NPC types (0x40/42/45/47/49/4b/4d) routed to
 * re15_npc_ai_tick @L5264-5278. DEFERRED to wave 2: the dialogue behaviour VM + the per-NPC overlay states. */
static const uint8_t s_irons_clip_len[24] =   /* EM040 (Chief Irons) clip frame-counts (CDEMD0.EMS idx 18, dir[1]) */
    { 34,32,50,26,20,20,50,1,1,1,1,25,1,1,1,1,1,10,25,1,1,1,30,30 };

/* Elliot (0x47) is a ROOM cinematic actor whose Plc_motion GESTURE clips come from his OWN loaded
 * EDD (PLD/ELLIOT.EDD → elliot_anim, 26 clips: clip 15 "Hey!"=20f, 16=30, 17=30, 20=25, 25=82 —
 * dumped live via RE15_ELLIOT_CLIPDUMP 2026-07-26), NOT the shared EM040/Irons table (whose
 * [15/16/20]=1 and out-of-range [25] froze his wave on frame 0). Byte-true: anim_set (0x8001f314)
 * wraps anim_frame at the ENTITY'S OWN EDD clip count (channel +0x174), so the motion executor must
 * read Elliot's real bank. The platform registers the loaded bank (stable function-scope storage).
 * ⚠ ROOM1170-AUSNAHME (marvin_10d0.md §6.3 / OFFEN O3): der RBJ-Marker bindet dort REC1 an Elliot,
 * der Port bleibt bewusst auf Elliots eigener Bank (Clip 25 existiert nur dort; das Original-OOB-
 * Verhalten auf REC1 ist ungemessen). Vor einer Umstellung per Savestate messen. */
static const re15_emd_animation_t *s_npc_elliot_anim = NULL;
void re15_npc_set_elliot_anim(const re15_emd_animation_t *a) { s_npc_elliot_anim = a; }

/* Executor-Sub → Anim-KANAL (adversarial verifiziert, marvin_10d0.md Verify D2/D4 — jeder Sub
 * liest ein ANDERES Bank-Paar der Entity):
 *   sub 0 @0x80050d40/48: +0x180/+0x184 = der RBJ-BINDER-Kanal (FUN_8001b3f8 — Cutscene-Gesten)
 *   sub 1 @0x80050ddc:    +0x84/+0x16c  = die Loco-Bank
 *   sub 2 @0x80050f88/90: +0x170/+0x174 = die Entity-EIGENE Bank (Spawn @0x80042374/8c)
 *   sub 3 @0x80051024:    +0x178/+0x17c = die Victim-Bank
 *   sub 6 @0x80051884/88 + Walk-INIT @0x800511bc: +0x170/+0x174 (eigene Bank)
 * Der RBJ-Kanal wird pro AKTOR-Slot vom Marker-Binder registriert (re15_actor_rbj_anim,
 * enemy_common.c); Fallback = die eigene Bank (der Spawn initialisiert beide Kanaele gleich,
 * der Binder ueberschreibt nur bei gesetztem Marker-Bit). */
static const re15_emd_animation_t *re15_npc_channel_anim(const re15_actor_t *e)
{
    extern const re15_emd_animation_t *re15_actor_rbj_anim(int slot);
    re15_enemy_bank_t *bank = re15_enemy_find(e->type);
    if (e->state == 4 && e->sub_state_1 == 0 && !e->walk_active) {   /* sub 0 -> RBJ-Kanal (+0x180);
                                                          * waehrend eines Plc_dest-Walks besitzt der
                                                          * Walker den Clip (5, eigene Bank) */
        const re15_emd_animation_t *rb = re15_actor_rbj_anim((int)(e - g_actors));
        if (rb) return rb;
    }
    if (e->state == 4 && e->sub_state_1 == 1 && bank && bank->loco_ok)
        return &bank->anim_loco;                          /* sub 1 -> Loco (+0x84/+0x16c) */
    if (e->state == 4 && e->sub_state_1 == 3 && bank && bank->victim_ok)
        return &bank->anim_victim;                        /* sub 3 -> Victim (+0x178/+0x17c) */
    if (e->type == 0x47 && s_npc_elliot_anim) return s_npc_elliot_anim;   /* Elliot: eigene Bank */
    if (bank && bank->ok && bank->anim.clip_count > 0) return &bank->anim;/* eigene Bank (+0x170) */
    return NULL;                                          /* -> s_irons-Fallback-Tabelle */
}
static int re15_npc_motion_clip_len(const re15_actor_t *e)
{
    const re15_emd_animation_t *an = re15_npc_channel_anim(e);
    if (an && (int)e->motion < an->clip_count && an->clips[e->motion].frame_count > 0)
        return an->clips[e->motion].frame_count;
    uint8_t c = e->motion; int fc = (c < 24) ? s_irons_clip_len[c] : 1; return (fc < 1) ? 1 : fc;
}
static void re15_npc_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_npc_anim(re15_actor_t *e)     /* POST-inc +0x95, wrap at the ENTITY channel's clip length */
{
    int fc = re15_npc_motion_clip_len(e); if (fc < 1) fc = 1;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    return done;
}

/* ===== NPC shared executor sub-VM (@0x80076ca0, driven by state[4] 0x80050be8) — Wave 2 (phase 1) =====
 * Byte-true (RE wf_65933e19 + self-verified disasm). Reuses re15_ai_arc_test (0x8001ab9c, 0=aligned) and
 * re15_enemy_steer_point (0x8001aac4, yaw-slew). The per-type TURN cone is @0x80076c41. */
static const uint8_t s_npc_turn_cone[16] =    /* @0x80076c41 byte[(type-0x40)*2] (0x40=0x60, 0x4b=0x50) */
    { 0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x50,0x50,0x60,0x60,0x50 };
static uint8_t re15_npc_tbl(const uint8_t *t, uint8_t type)   /* byte[(type-0x40)], clamp to 0..15 */
    { int i = (int)type - 0x40; if (i < 0 || i > 15) i = 0; return t[i]; }
static uint8_t re15_npc_type_cone(uint8_t type) { return re15_npc_tbl(s_npc_turn_cone, type); }

/* subs 1-3: idle-pose (a 2-layer looping idle FSM on +0x6, no move/turn). The port's faithful-line is the
 * clip-2 hold — the byte-true anim_set-loop folds to "advance a real looping idle clip". */
static void re15_npc_sub_idle(re15_actor_t *e)
{
    if (e->motion < 24 && s_irons_clip_len[e->motion] <= 1) re15_npc_clip(e, 2);
    re15_npc_anim(e);
}

/* sub 0: MOTION phase-FSM — byte-true port of 0x80050cb8 (the shared-executor sub-table[0]). This is the
 * handler a Plc_motion pose lands on (Plc_motion @0x80041bc4 sets +0x5 = pc[1]; for the shipped poses
 * pc[1]=0). It plays the +0x94 clip once and HOLDS the last frame, unless the LOOP flag +0x1c4 & 0x04 is
 * set (then it replays). The phase lives in +0x6 (= sub_state_2): 0=init, 1=play, 2=held (@0x80050cc8
 * dispatch; @0x80050da4 sets phase 2 at clip-end; @0x80050dc0-c8 gates LOOP on +0x1c4 & 0x04). The frame
 * is advanced HERE (single advancer — re15_actors_anim_advance skips executor NPCs, see there), matching
 * the original where anim_set is the sole stepper for a state-4 actor. */
static void re15_npc_sub_motion(re15_actor_t *e)
{
    int fc = re15_npc_motion_clip_len(e); if (fc < 1) fc = 1;   /* KANAL-Laenge (Sub->Kanal-Map oben) */
    switch (e->sub_state_2) {
    case 0:   /* phase 0 init (@0x80050cec-d0c / sub2 @0x80050f34-54): +0x95=0, seed +0x8f crossfade
               * (UNLESS the no-blend bit +0x1c4&0x40 @0x80050f64-78 -> +0x8f=0), phase->1, fall to play */
        e->anim_frame = 0; e->anim_frac = (uint8_t)((e->anim_flags & 0x40) ? 0 : 7);
        e->sub_state_2 = 1;
        /* fallthrough — the original phase-0 branch continues into the phase-1 body @0x80050d34 */
    case 1:   /* phase 1 play (@0x80050d34 / sub2 @0x80050f94): advance the clip (TWICE with the
               * double-step bit +0x1c4&0x8 — second f314 call @0x80050fb8/d0); at end latch phase 2
               * (HOLD @0x80050da4/0x80050fec) or replay (LOOP nur mit +0x1c4&0x4 @0x80050dc8/0x80051004-10) */
    {
        int steps = (e->anim_flags & 0x08) ? 2 : 1;
        while (steps-- > 0) {
            if (e->anim_frame + 1 >= fc) {                   /* anim_set signals clip end (@0x80050d54) */
                if (e->anim_flags & 0x04) { e->anim_frame = 0; e->sub_state_2 = 1; }  /* LOOP replay */
                else { e->anim_frame = (uint8_t)(fc - 1); e->sub_state_2 = 2; }        /* HOLD last frame */
                break;
            }
            e->anim_frame++;                                 /* +0x95++ */
            if (e->anim_frac > 0) e->anim_frac--;
        }
        break;
    }
    case 2:   /* phase 2 held (@0x80050cdc exit): retain the last posed frame, no advance */
        break;
    }
}

/* sub 9: TURN / look-at IN PLACE (@0x80051cf8). Slews rot_y toward the steer point (+0x1bc/+0x1be) at
 * cone[type]/step; on alignment hands +0x5 to sub 6 (event-reach). NO position advance. */
static void re15_npc_sub_turn(re15_actor_t *e)
{
    if (e->sub_state_2 > 1) return;                          /* >1 -> exit (@0x80051d20) */
    if (e->sub_state_2 == 0) {                               /* INIT (@0x80051d2c-5c) */
        e->sub_state_2 = 1; e->motion = 5; e->anim_frame = 0; e->anim_frac = 7;
    }
    uint8_t cone = re15_npc_type_cone(e->type);              /* @0x80076c41 */
    if (re15_ai_arc_test(e, e->steer_x, e->steer_z, cone) == 0) {   /* ALIGNED (@0x80051d90-98) */
        e->sub_state_1 = 6; e->sub_state_2 = 0;              /* -> sub 6 event-reach (@0x80051dac/dbc) */
        if (e->anim_flags & 4) { e->sub_state_1 = 9; e->sub_state_2 = 2; }   /* re-arm (@0x80051df4) */
    }
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, cone); /* yaw-slew (@0x80051e44) */
    re15_npc_anim(e);                                        /* advance the turn clip (motion 5) */
}

/* per-type WALK param @0x80076c00 byte[(type-0x40)*2] — INIT writes it to +0x8c (sh, halfword). */
static const uint8_t s_npc_walk_param[16] =
    { 0x4b,0x4b,0x4b,0x4b,0x46,0x46,0x46,0x46,0x4b,0x4b,0x4b,0x32,0x32,0x46,0x46,0x32 };

/* subs 4/5/7/8: WALK-to-target (@0x80051148). A 3-phase FSM on +0x6 (sub_state_2), disasm-verified:
 *   0 INIT (@0x80051198): +0x8c = walk_param, motion=5, anim_frac=7, sub_state_2=1 — falls through to RUN.
 *   1 RUN / turn-to-face (@0x80051200): arc_test(steer, 0x15e=350); aligned -> sub_state_2=2; steer + anim.
 *   2 ARRIVED / walk-straight (@0x800512d4): aligned, advance forward; steer + anim.
 * The FORWARD DISPLACEMENT is the executor's root-motion channel 0 (anim_flags&1 → FUN_800369f8), NOT this
 * sub: verified that NEITHER the walk nor turn sub writes +0x1c4 (anim_flags) — the bit is armed by the
 * un-RE'd anim/clip-set path. So this sub is byte-true for the STEERING; the movement plugs into the
 * executor gate once that arming is present. SUPERSEDED (@L4047-4053): the byte-true forward TRANSLATION
 * is actually pos_advance(a0=0) (FUN_800245d8), IMPLEMENTED @L4052-4053 — NOT the root-motion channel; so
 * this sub's walk-straight forward step IS ported. */
static void re15_npc_sub_walk(re15_actor_t *e)
{
    if (e->sub_state_2 > 2) return;                          /* >2 -> exit (@0x80051190) */
    if (e->sub_state_2 == 0) {                               /* INIT (@0x80051198) -> falls through to RUN */
        e->crow_speed  = (int16_t)re15_npc_tbl(s_npc_walk_param, e->type);  /* +0x8c walk param (sh) */
        e->sub_state_2 = 1; e->motion = 5; e->anim_frame = 0; e->anim_frac = 7;
    }
    if (e->sub_state_2 == 1 &&                               /* RUN: turn-to-face (@0x80051214) */
        re15_ai_arc_test(e, e->steer_x, e->steer_z, 0x15e) == 0)
        e->sub_state_2 = 2;                                  /* aligned (arc 350) -> walk-straight (@0x80051230) */
    re15_enemy_steer_point(e, e->steer_x, e->steer_z, re15_npc_type_cone(e->type));  /* @0x80051264/133c yaw-slew */
    /* WALK-STRAIGHT (phase 2 only): the forward TRANSLATION is pos_advance (FUN_800245d8) with a0=0
     * (@0x80051344-48: `jal pos_advance; addu a0,zero,zero`) — move +0x8c (crow_speed = the per-type walk
     * param loaded by INIT) along rot_y. It is NOT clip root-motion: EM040 clip 5 carries a CONSTANT
     * sx=383 across all 20 keyframes (dumped from the loaded bank) = zero frame delta = an in-place step.
     * The turn-to-face phase (1) has no pos_advance (@0x800512cc jumps to exit) so it pivots in place. */
    if (e->sub_state_2 == 2)
        re15_dog_advance(e, e->crow_speed);                  /* pos_advance(a0=0): +0x8c fwd along rot_y */
    re15_npc_anim(e);
}

/* sub 6: EVENT-REACH (@0x800517f0). A 4-phase +0x6 FSM that plays clip 1 (the 32-frame arrival/gesture)
 * ONCE, then settles into clip 2 (the idle loop). This is what a STATIONARY NPC actually animates — the
 * INIT seeds +0x5=6 for a +0x9&0x40 spawn, so Chief Irons & co. run THIS, not a bare idle hold. Phases:
 *   0 setup clip 1 (motion=1, anim_frac=7) -> falls through to play; 1 play clip 1, last frame -> phase 2;
 *   2 setup clip 2 idle (motion=2) -> falls through to play; 3 play clip 2 idle loop. Advance = anim_set
 *   returning 1 at clip end (re15_npc_anim's `done`). */
static void re15_npc_sub_event_reach(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0: e->motion = 1; e->anim_frame = 0; e->anim_frac = 7; e->sub_state_2 = 1;  /* @0x80051844 setup clip 1 */
        /* fall through: same tick plays clip 1 (@0x80051878) */
    case 1: if (re15_npc_anim(e)) e->sub_state_2 = 2; break;                          /* @0x80051878 play; done -> 2 */
    case 2: e->motion = 2; e->anim_frame = 0; e->anim_frac = 7; e->sub_state_2 = 3;  /* @0x800518b4 setup clip 2 idle */
        /* fall through: same tick plays clip 2 (@0x800518dc) */
    default: re15_npc_anim(e); break;                                                 /* @0x800518dc idle loop */
    }
}

/* Executor sub-dispatch: sub_library[+0x5] (@0x80076ca0): idle (0-3), walk-to-target (4/5/7/8),
 * event-reach (6 = the stationary NPC's clip1->idle), turn (9). */
static void re15_npc_sub_dispatch(re15_actor_t *e)
{
    switch (e->sub_state_1) {
    case 0:                                  /* motion phase-FSM = Plc_motion pose (@0x80076ca0[0]=0x80050cb8) */
    case 1:                                  /* 0x80050ddc — DIESELBE Play-once+Hold-FSM auf dem Loco-Kanal */
    case 2:                                  /* 0x80050f00 — dito auf der Entity-EIGENEN Bank (+0x170/174);
                                              * der alte Endlos-Idle-Loop fuer Subs 1-3 war erfunden —
                                              * ROOM10D0 Plc_motion(2,6,0) loopte Marvins Clip 6 statt
                                              * einmal zu spielen und zu halten (marvin_10d0.md D4) */
    case 3:                                  /* 0x80051024 — dito auf der Victim-Bank (+0x178/17c) */
        re15_npc_sub_motion(e); break;
    case 4: case 5: case 7: case 8: re15_npc_sub_walk(e); break;
    case 6: re15_npc_sub_event_reach(e); break;
    case 9: re15_npc_sub_turn(e); break;
    default: re15_npc_sub_idle(e); break;
    }
}

/* STATE[4] EXECUTOR @0x80050be8: HALF-RATE gate then sub-dispatch then toggle 0x20 (+ root-motion, phase 3).
 * Gate (self-verified @0x80050c00-c0c): dispatch iff !((anim_flags&0x10) && (anim_flags&0x20)) — skip ONLY
 * when both bits are set; the 0x10-clear path always dispatches. 0x20 toggles each executor tick. */
static void re15_npc_executor(re15_actor_t *e)
{
    int fr_prev = (int)e->anim_frame;   /* the sub's re15_npc_anim advances anim_frame below */
    if (!((e->anim_flags & 0x10) && (e->anim_flags & 0x20))) re15_npc_sub_dispatch(e);   /* @0x80050c00-c34 */
    e->anim_flags ^= 0x20;                                        /* @0x80050c50 */
    /* root-motion channel 0 (@0x80050c6c-78): `if (anim_flags & 1) FUN_800369f8(0,0)` mode 0 =
     * re15_clip_root_motion_delta — the current clip's per-frame keyframe root translation, rotated by
     * rot_y, into +0x34/+0x3c. Byte-true GATE (disasm-verified). Two things must be present for the NPC
     * to actually displace, and BOTH are deferred (documented in re15_npc_sub_walk):
     *   (1) anim_flags&1 armed — set by the un-RE'd anim/clip-set path, NOT any executor sub;
     *   (2) the NPC's EM bank loaded — re15_enemy_find(type) is NULL in the loadable rooms (the NPC
     *       renders via def_skel there), so there is no skeleton to read the keyframe from.
     * The gate + wiring are kept byte-true; the delta form self-skips on a non-advancing/ wrapped frame.
     * Channel 1 (@0x80050c94: `if (anim_flags & 2) FUN_800369f8(0,1)`) is a second segment — deferred. */
    if (e->anim_flags & 1) {
        /* Byte-true executor gate (@0x80050c6c): apply channel-0 clip root-motion. NOTE the NPC's own
         * clips are IN-PLACE — EM040 clip 5 carries a CONSTANT sx=383 across all 20 frames (verified by
         * dumping bank->anim/anim_loco keyframe speeds), so the frame-to-frame delta is 0 and this is a
         * no-op for the NPC walk. The NPC forward translation therefore comes from a DIFFERENT mechanism
         * (a fixed-speed step, not clip root-motion) that is not yet RE'd — see re15_npc_sub_walk. The
         * gate is kept byte-true for actors whose clips DO carry per-frame root translation. */
        re15_enemy_bank_t *bank = re15_enemy_find(e->type);
        if (bank) re15_clip_root_motion_delta(e, &bank->skel, &bank->anim,
                                              (int)e->motion, (int)e->anim_frame, fr_prev);
    }
}

static void re15_npc_ai_tick(int slot)
{
    re15_actor_t *e = &g_actors[slot];

    /* RE15_NPC_TURN_TEST / RE15_NPC_WALK_TEST: contrived verification of the executor sub-VM (the STAGE1
     * NPCs park off-screen, so this seeds an observable behaviour). TURN = force sub 9 aimed at the LIVE
     * player. WALK = force sub 4 toward the live player (the NPC steers to face it; the forward advance is
     * the executor's deferred root-motion channel, so the position holds until that arming is RE'd). */
    { static int s_tt = -2;
      if (s_tt == -2) s_tt = getenv("RE15_NPC_WALK_TEST") ? 4 : (getenv("RE15_NPC_TURN_TEST") ? 9 : 0);
      if (s_tt && e->state != 0) {
          re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
          e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* live player as the steer target */
          if (e->state != 4) { e->state = 4; e->sub_state_2 = 0; }     /* enter the executor (INIT the sub) */
          e->sub_state_1 = (uint8_t)s_tt; e->anim_flags |= 0x10;       /* force the turn(9)/walk(4) sub */
      } }

    /* GLOBAL AI-freeze gate — byte-true to the SHARED root FUN_8011c654 @0x8011c654 line 19, the SAME
     * gate read by 108 enemy/NPC/object roots and by the port's generic enemy tick (L128-129/2516-2517):
     *   skip the +0x4 state dispatch iff (DAT_800aca40 & 0x20000000) || (entity+0x9 & 0x20).
     * This is engine-wide, NOT room-specific — the two shipped freeze engines are:
     *   (a) s_ai_paused = the GLOBAL message/modal freeze DAT_800aca40&0x20000000 (set by a Message_on
     *       whose flag halfword has bit 0x2000 @0x800404f4->FUN_80027e68 @0x80027ed0, or the item modal
     *       FUN_8001db28; cleared by FUN_80028134). [Currently a dead stub in the port — see the
     *       message-FSM gap — but wired here byte-true so NPCs freeze the instant it is driven.]
     *   (b) grid_id & 0x20 = the PER-ENTITY skip, set by SCD Member_set(12, val&0x20) after Work_set(2,N)
     *       (@0x800410b8->FUN_8004116c case 0xc; value 0x30 freezes, 0x10 unfreezes) — LIVE in shipped
     *       content (ROOM1141/11C0 combat intros). The port's decode (re15_actor_set_member case 12 ->
     *       grid_id) + read here are byte-true, so an NPC frozen this way now yields correctly.
     * The ROOM1170 intro Elliot uses NEITHER (his Plc_dest walk is a routine-REDIRECT, handled by the
     * case-4 walk_active/scd-event proxy below), so this gate leaves his behaviour unchanged. */
    if (getenv("RE15_NPC_YIELD_DBG")) {
        static int s_n = 0;
        if ((s_n++ % 30) == 0)
            fprintf(stderr, "[npcyield] slot=%d t=%02x st=%d sub=%d ph=%d mo=%d af=%d flags=0x%04x walk=%d grid=%02x\n",
                    slot, e->type, e->state, e->sub_state_1, e->sub_state_2, e->motion, e->anim_frame,
                    e->anim_flags, e->walk_active, e->grid_id);
    }

    if (s_ai_paused || (e->grid_id & RE15_AI_GRID_SKIP)) return;

    /* SCD Plc_dest WALK yield (the ROOM1170 Elliot fix, 6b1bdd5f): while an SCD thread walks this
     * actor (Plc_dest -> walk_active) or otherwise Work_set-owns it (scd_slot_event_controlled), skip
     * the state dispatch so the SCD alone drives the walk clip (else the executor overwrites it every
     * frame -> the "Elliot glides in an idle pose" float). Keep the actor INVULNERABLE (HP = -1, the
     * INIT's byte-true +0x9a @0x8011c744).
     * NOTE: a Plc_motion POSE is NOT yielded here — op_plc_motion now puts the NPC in the byte-true
     * STATE 4 (executor @0x80041bb0, sub-VM re15_npc_sub_motion), so it dispatches state 4 below and
     * the motion phase-FSM plays+holds the clip exactly like the original. The INIT (state 0) never
     * runs to stomp +0x94, and 2c8d9a69's ROOM11B0 idle T-pose fix is unaffected (those NPCs are never
     * Plc_motion'd -> they reach the executor's idle path). */
    /* A Plc_motion POSE (state 4 / sub_state_1 == 0 = the motion phase-FSM) must NOT yield even while
     * the SCD Work_set-owns the slot: re15_npc_sub_motion is the SOLE advancer of a state-4 pose clip,
     * so yielding freezes it on frame 0. MEASURED (RE15_NPC_YIELD_DBG): the ROOM1170 Elliot "Hey!"
     * wave sat at walk=0, st=4, sub=0, af=0 for the whole gesture because a Work_set(2,0) thread owned
     * slot 1 → scd_slot_event_controlled=1 → yielded. A Plc_dest WALK still yields (walk_active=1, or
     * the post-arrival gap where the SCD owns the slot but sub_state_1 != 0 = not the motion pose). */
    /* Pose-Subs 0-3 (Plc_motion) UND der Event-Reach 6 (Plc_dest Mode 6) duerfen NICHT yielden —
     * der Executor ist ihr einziger Frame-Advancer. Vorher yieldete alles ausser Sub 0, weshalb
     * Marvins Mode-6-Idle in 10D0 nie lief (marvin_10d0.md D3). */
    int in_motion_pose = (e->state == 4 && (e->sub_state_1 <= 3 || e->sub_state_1 == 6));
    if (e->walk_active || (re15_scd_slot_event_controlled(slot) && !in_motion_pose)) {
        e->hp = -1;
        return;
    }
    if (in_motion_pose) e->hp = -1;   /* stay invulnerable while posing (the yield used to enforce this) */

    switch (e->state) {
    case 0:   /* INIT 0x8011c6dc: idle pose, INVULNERABLE, -> state 1 (or the shared executor) */
        e->hp = -1;                                       /* +0x9a = -1 (no HP / invulnerable) @0x8011c744 */
        e->motion = 2; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;  /* idle clip 2 @0x8011c7bc */
        e->ai_timer = 0x78;                               /* +0x9e = 120 @0x8011c754 */
        /* NPC-Neck-/Head-Look-Init (byte-true FUN_8011c6dc, cutscene_headlook.md B2/B3/B5):
         * Default-Blickziel = SPIELER (sw &player -> +0x1a8 @0x8011c738), Flags = 0 = TRACK
         * (sb 0 -> +0x1b8 @0x8011c768), Kopf-Bone 8 (sb 8 -> +0x1b9 @0x8011c778), Steps 64/48
         * (sh @0x8011c790/98), Yaw-Klemme ±0x2c8 (@0x8011c7a0), Pitch-Klemme ±0x138
         * (@0x8011c7b0), Akkus 0 (@0x8011c7a8/ac), Speed-Override-Byte 0x78 (@0x8011c758).
         * SPL-Spawn-Ausnahme: entity+0x9 & 0x40 -> Flags = 0x12 (neck-idle statt Tracking,
         * @0x8004260c-18) — genau die 10D0-Marvin-Konfiguration (behavior 0x40). */
        e->neck_bone        = 8;
        e->neck_target_slot = RE15_ACTOR_SLOT_PLAYER;
        e->neck_step_yaw    = 64;  e->neck_step_pitch  = 48;
        e->neck_clamp_yaw   = 0x2c8; e->neck_clamp_pitch = 0x138;
        e->neck_yaw = 0; e->neck_pitch = 0; e->neck_sweep = 0;
        e->neck_speed       = 0x78;
        e->neck_flags       = (uint8_t)((e->grid_id & 0x40) ? 0x12 : 0x00);
        if (e->grid_id & 0x40) { e->state = 4; e->sub_state_1 = 6; }  /* +0x9&0x40 -> shared executor, +0x5=6 @0x8011c860 */
        else e->state = 1;                                /* default -> Irons overlay state 1 @0x8011c884 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;

    case 4:   /* shared executor 0x80050be8: the sub-VM (@0x80076ca0, dispatch on +0x5) — Wave-2 phase 1:
               * idle-pose (subs 0-3) + turn/look-at (sub 9). Walk (4/5/7/8) + watchers = later phases.
               * The SCD-owns-animation yield now lives at the top of this function (global, all states),
               * so an NPC still in the executor without an SCD lock runs its idle pose here. */
        re15_npc_executor(e);
        break;

    case 1:   /* Irons overlay state 1 = idle/scripted (wave 2) -> hold the idle pose */
    default:  /* all other NPC states (watchers / overlay) = wave 2 -> hold the idle pose */
        if (e->motion < 24 && s_irons_clip_len[e->motion] <= 1) re15_npc_clip(e, 2);  /* keep a real looping idle clip */
        re15_npc_anim(e);
        break;
    }
}

/* ============================ ZOMBIE GIRL (type 0x13, EM013 = ZOMBIE_GIRL) ==================== *
 * BYTE-TRUE REWRITE (audit wf_827f186d zombie-girl #1 HIGH + #2 HIGH + #3/#4/#5 + #6/#7/#8): the
 * shipped flat instant-aggro chase modeled the UNREACHABLE mode-1 nav walk (the clip-26 walk fn
 * 0x8010be50 has exactly ONE reference in STAGE1.BIN = the mode-1 animate[0] pointer word
 * @0x80120308; zero jal refs; no shipped room spawns mode 1 — SCD census: STAGE1 has 0 type-0x13
 * spawns, STAGE4 ROOM4050/4051 spawn `44 00 13 00` = behavior byte 0x00 -> mode 0). The REAL brain:
 *   root FUN_8010a8c8 (@0x80072bac[0x13]): pause gate @0x8010a8d0-e0, skip gate +0x9&0x20
 *   @0x8010a8f4-900, dist cache +0x1d0 @0x8010a908-64 (SquareRoot0), mercy +0x1d5 tick + aca50&=
 *   ~1 @0x8010a974-9b4, nav steer FUN_80039e7c(+0x1bc/+0x1be, +0x1d6, +0x1d8&8) @0x8010a9c0-e0 +
 *   the &8 one-shot clear @0x8010a9f0-fc, then the STATE dispatch @0x80120208[+0x4] @0x8010aa0c-2c,
 *   then the shared tail (0x8001b4e4 / attack-pt 0x80104178 / b498 / aec4+b544 / SCA b0a4 / SFX
 *   0x8001b38c @0x8010aa80-aaf4 — the run_all t==0x13 branch).
 * STATE TABLE @0x80120208: [0]=INIT 0x8010ab2c, [1]=ACTIVE 0x8010b274, [2]=HURT 0x8010bf80,
 *   [3]=DEATH 0x8010c014, [4]=0x8010919c (idle, deferred port-wide), [7]=CORPSE 0x80109554 (the
 *   SAME shared corpse settle as the standard zombie -> re15_enemy_corpse_settle).
 * ACTIVE = mode dispatch @0x8010b694-b6b8 on +0x9&0xf via @0x80120230; mode 0 = FUN_8010b6d4 = the
 *   STANDARD-ZOMBIE phase FSM: decide @0x80120264[+0x5] / animate @0x801202a8[+0x5], both tables
 *   WORD-DIFF-VERIFIED byte-identical to the standard @0x8011f840/@0x8011f890 rows [0..0x10] except
 *   row [0xa] = the girl FLOOR-DROP pair 0x8010bbe0/0x8010bbe8 (replaces the edge-fall) -> the port
 *   REUSES the ported standard handlers (search/wander/engage/grab/devour/turn/charge/stagger/
 *   pushoff/feeding/standup).
 * HURT/DEATH masters @0x8012039c/@0x8012063c ([+0x5]*0x20+[+0x6]*4, FUN_8010bf80 @0x8010bfd4-ffc /
 *   FUN_8010c014 @0x8010c048-70) are WORD-DIFF-VERIFIED identical to the standard masters
 *   @0x8011fb90/@0x8011feac except weapon-row 1 dir 0/1 = NULL words (a jalr-0 crash lane in the
 *   original — dormant, not reproduced); downed death routes to the SAME FUN_80107cb0 (@0x8010c038).
 *   -> re15_enemy_ai_live_hurt / re15_enemy_ai_live_death (fixes #3 death-fall + corpse settle and
 *   #4 no-flinch; the girl-specific downed-hurt router +0x5==2 gate is inside live_hurt).
 * Anim advance: the girl now uses the SHARED per-frame advancer + clip banks like the standard
 *   zombie (0x13 removed from re15_type_self_advances_anim). */

/* Girl animate row [0x11] — the byte-true TABLE-OVERFLOW word: the girl decide/animate tables have
 * 17 rows [0..0x10] and FUN_8010b6d4 has NO bounds check (@0x8010b718-38/@0x8010b740-6c), so
 * +0x5=0x11 reads animate[0x11] = word @0x801202ec = 0x801035f8 = the mode-1 idle DECIDE (also
 * mode-1 decide[0] @0x801202ec). Raw-disasm'd end-to-end: */
static void re15_zgirl_overflow_row11(re15_actor_t *e, re15_actor_t *pl)
{
    /* if player.hit_react==0 (@0x8010360c-14) && dist +0x1d0 < 0x4b0 (@0x80103628-34) &&
     * arc_test(player,0x200)==0 (@0x80103640-48) && player.floor == +0x82 (@0x80103650-68):
     * +0x4 word = ((facing_aligned(0x8001a780)+1)<<8)|1 = 0x101/0x201 (@0x80103670-8c). */
    if (pl->hit_react == 0 && e->ai_dist < 0x4b0u &&
        re15_ai_arc_test(e, pl->x, pl->z, 0x200) == 0 && pl->floor == e->floor) {
        int aligned = re15_ai_facing_aligned(e, pl);
        re15_ai_set_state_word(e, (uint32_t)((aligned + 1) * 0x100) | 1u);
    }
    /* (+0x1c4 & 0x2000 @0x8010369c-a4) && !(+0x1d8 & 0x80 @0x801036b0-bc) -> word 0x601
     * (@0x801036c0-c4). */
    if ((e->anim_flags & 0x2000) && !(e->ai_flags & 0x80))
        re15_ai_set_state_word(e, 0x601);
}

/* Girl FLOOR-DROP animate — byte-true FUN_8010bbe8 (girl animate table [0xa] @0x801202d0; the
 * decide half 0x8010bbe0 @0x8012028c is `jr ra`). Her contact-roll replacement for the standard
 * edge-fall 0x801033c8: walk INTO the contact heading, then drop one floor. */
static void re15_zgirl_floor_drop(re15_actor_t *e)
{
    switch (e->sub_state_2) {
        case 0:                                     /* @0x8010bc3c-7c */
            e->sub_state_2 = 1;                     /* +0x6=1 @0x8010bc3c */
            e->motion = 0x01; e->anim_frame = 0;    /* +0x94=1 @0x8010bc4c, +0x95=0 @0x8010bc5c */
            e->anim_frac = 7;                       /* +0x8f=7 @0x8010bc78-7c (+0x96=0 @0x8010bc6c) */
            e->anim_blend_rate = 0x200;             /* f314 a3=0x200 @0x8010bc94-98 */
            /* fallthrough into [1] (@0x8010bc80) */
        case 1: {                                   /* anim_set(+0x84,+0x16c,0,0x200) @0x8010bc94;
                                                     * slew to the CONTACT heading ((+0x90&0xf0)<<4)
                                                     * at 0x10/tick (0x8001aa68 @0x8010bcac-b8);
                                                     * aligned (delta==0 @0x8010bcd4-d8) -> +0x6=2
                                                     * (@0x8010bce8-f0). */
            int heading = ((int)(e->ai_contact & 0xf0) << 4) & 0x0fff;
            if (re15_slew_to_angle(e, heading, 0x10) == 0)
                e->sub_state_2 = 2;
            return;
        }
        case 2:                                     /* @0x8010bcf4-bd68 */
            e->sub_state_2 = 3;                     /* +0x6=3 @0x8010bcf4-f8 */
            e->motion = 0x16; e->anim_frame = 0;    /* +0x94=0x16 @0x8010bd04-08, +0x95=0 @0x8010bd18 */
            e->anim_frac = 7;                       /* +0x8f=7 @0x8010bd34-38 (+0x96=0 @0x8010bd28) */
            e->anim_blend_rate = 0x200;
            e->speed_h = 0x474;                     /* +0x8c=0x474 @0x8010bd44-48 */
            e->rot_y = (int16_t)((((int)(e->ai_contact & 0xf0) << 4)) & 0x0fff);
                                                    /* +0x6a = (+0x90&0xf0)<<4 @0x8010bd58-68 */
            /* fallthrough (@0x8010bd6c) */
        case 3:
            /* frame 0x46 (@0x8010bd78-80): +0xb0=0x474 fall velocity @0x8010bd88-8c + the ground-Y
             * cache +0x1ba -= 1800 @0x8010bd9c-a8 (+0xb0 vel is deferred like the edge-fall's). */
            if (re15_enemy_clip_done(e)) {          /* anim_set(+0x170,+0x174,0,0x200) done @0x8010bdc0-c8 */
                e->floor = (uint8_t)(e->floor + 1); /* +0x82 += 1 @0x8010bddc-e8 */
                e->y -= 0x708;                      /* +0x38 += -1800 @0x8010bdf8-be04 (literal bytes) */
                e->sub_state_1 = 0;                 /* +0x5=0 @0x8010be14 */
                e->sub_state_2 = 0;                 /* +0x6=0 @0x8010be24 */
                /* +0xb0=0 @0x8010be34 + one func_0x800245d8(0) walker step @0x8010be30 — the
                 * fall-velocity walker is deferred (same stance as the standard edge-fall port). */
            }
            return;
        default: return;
    }
}

static void re15_zgirl_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* root FUN_8010a8c8 preamble (see the header block for the @0x… map): */
    if (s_ai_paused) return;                        /* g_pauseflags & 0x20000000 @0x8010a8d0-e0 */
    if (e->grid_id & RE15_AI_GRID_SKIP) return;     /* +0x9 & 0x20 @0x8010a8f4-900 */
    e->ai_dist = (uint32_t)re15_enemy_player_dist(e, pl);   /* +0x1d0 @0x8010a908-64 */
    /* mercy +0x1d5 tick @0x8010a974-9b4 -> the global s_grab_mercy_timer (run_all) stands in;
     * FUN_8001bd60(-10,0x14) @0x8010a9b8-bc (look aux) is unmodeled port-wide. */
    re15_nav_update_steer(e, (int16_t)pl->x, (int16_t)pl->z,
                          e->ai_wp_node, (int)(e->ai_flags & 8u));  /* FUN_80039e7c @0x8010a9c0-e0 */
    e->ai_flags &= (uint16_t)~8u;                   /* the one-shot clear @0x8010a9f0-fc */

    switch (e->state) {                             /* @0x80120208[+0x4] @0x8010aa0c-2c */
    case 0:                                         /* INIT FUN_8010ab2c */
        e->state = 1;                               /* +0x4=1 @0x8010aba4-a8 */
        e->steer_x = (int16_t)pl->x;                /* +0x1bc = playerX @0x8010abb8-c0 */
        e->steer_z = (int16_t)pl->z;                /* +0x1be = playerZ @0x8010abd0-d8 */
        /* +0x0 |= 0x40000000 @0x8010abe8-f4 — render/lifecycle word unmodeled */
        e->ai_timer = 0x14;                         /* +0x9c = 0x14 @0x8010ac00-08 */
        e->hp = (int16_t)((re15_engine_rand8() & 0x1f) + 50);   /* +0x9a = (rng&0x1f)+50 @0x8010ac0c-1c */
        /* +0x78 = dim word [0x8011f790] @0x8010ac28-34 — collision dims come from the spawn hitbox */
        {   /* +0x1d4 walk/turn variant = @0x8011f7e4[rng&7] @0x8010ac38-58; table bytes dumped:
             * {2,3,4,5,2,3,4,5} — the port's hurt_clip field IS +0x1d4 (engage/turn walk clip). */
            static const uint8_t walk_variant[8] = { 2, 3, 4, 5, 2, 3, 4, 5 };  /* @0x8011f7e4 */
            e->hurt_clip = walk_variant[re15_engine_rand8() & 7];
        }
        /* +0x1b8=0 @0x8010ac68 / +0x1b9=8 @0x8010ac74-78 (render words) + +0x1d5=0 @0x8010ac88
         * (mercy, global stand-in) + +0x1d7=4 @0x8010aca4-a8 (SCA dim row) — unmodeled port-wide */
        e->ai_flags = 0;                            /* +0x1d8 = 0 @0x8010ac98 */
        re15_enemy_ai_live_arm(slot);               /* pool +0x5f8=0x60/+0x5fa=0x30/+0x5fc=0x390/
                                                     * +0x5fe=0x138 @0x8010acbc-ace0 + the DAT_800aca3c
                                                     * &1-gated lunge arm (+0x1d8|=0x100, |=(rng&1)<<9,
                                                     * +0x1da=(rng&0xff)+(rng&0xff)+600) @0x8010ace8-
                                                     * 0x8010ad6c — the same writes as the standard's
                                                     * arm, so the shared port fn is exact here */
        /* shadow FUN_8001af5c(0,0,0x258,0x2bc,0x00808080,+0xb0) @0x8010b00c-38 — shadow subsystem */
        if (e->grid_id & 0x80) {                    /* spawn-pose decode, gated @0x8010b048-54 */
            uint8_t sel = (uint8_t)(e->grid_id & 0xf);
            if (sel == 4 || sel == 7 || sel == 9)   e->motion = 0x0c;   /* @0x8010b05c-84 */
            if (sel == 5 || sel == 8 || sel == 0xa) e->motion = 0x0e;   /* @0x8010b0a0-c8 */
            if (sel == 1 || sel == 3) { e->motion = 0x0c; e->sub_state_1 = 5; } /* @0x8010b0e4-b10c */
            e->anim_frame = 0;                      /* +0x95=0 @0x8010b11c */
            e->anim_frac  = 0;                      /* +0x8f=0 @0x8010b12c */
            /* anim_set(+0x170,+0x174,0,0x200) @0x8010b144-48, then the LYING pose clip replaces
             * the setup clip: */
            if (sel == 4 || sel == 7 || sel == 9)   e->motion = 0x12;   /* @0x8010b164-8c */
            if (sel == 5 || sel == 8 || sel == 0xa) e->motion = 0x14;   /* @0x8010b1a8-d0 */
        } else if (e->grid_id == 0x06) {            /* whole +0x9 byte == 6 @0x8010b1e0-e8 */
            e->motion = 0x27;                       /* +0x94=0x27 @0x8010b1f0-f4 */
            /* +0x4 word = 0x00020c00 @0x8010b1ec-204: state 0 / +0x5=0xc / +0x6=2 (re-INITs!) */
            re15_ai_set_state_word(e, 0x00020c00u);
            e->anim_frame = 0; e->anim_frac = 0;    /* @0x8010b214/b224 */
            e->grid_id = 0;                         /* +0x9 = 0 @0x8010b250 */
        }
        /* behavior byte 0x00 (ALL shipped spawns): neither branch taken -> +0x5/+0x6/+0x94 stay as
         * spawned (sub 0 = search/idle: "she idles until woken"). */
        break;

    case 1: {                                       /* ACTIVE FUN_8010b274 */
        /* ARMED lunge branch (@0x8010b288-b5a8, +0x1d8&0x100, skipped while +0x0&0x1000 @0x8010b29c-
         * a8): +0x1da-- @0x8010b2b0-bc; ==0x12c -> the 8x body-part action-0x16 inject (8x jal
         * 0x80019d50 over the LUT @0x8011f7a4, @0x8010b2dc-b554) = re15_enemy_lunge_begin; ==0 ->
         * word 0x1503 @0x8010b570-74 + clip 0xb @0x8010b580-84 (0x1f when +0x9&0x80 @0x8010b594-5ac)
         * and SKIP the dispatch this frame (j 0x8010b6c0); != 0 falls THROUGH to the LOS + mode
         * dispatch (@0x8010b56c bne -> 0x8010b5b0). Armed only under DAT_800aca3c&1 (INIT
         * @0x8010ace8-f4). The old flat chase played this DORMANT lunge lane's clip 0xb for every
         * grab (audit zombie-girl #7 wrong-constant — the reachable grab clips are the shared
         * (+0x5-3)*3 families via FUN_80102548). */
        if (e->ai_flags & 0x100) {
            e->ai_attack_timer = (int16_t)(e->ai_attack_timer - 1);
            if (e->ai_attack_timer == 0x12c) re15_enemy_lunge_begin(slot);
            if (e->ai_attack_timer == 0) {
                re15_ai_set_state_word(e, 0x1503);
                e->motion = (uint8_t)((e->grid_id & 0x80) ? 0x1f : 0x0b);
                break;
            }
        }
        /* LOS probe @0x8010b5b0-b600 — identical bytes to the standard ACTIVE's (@0x80101560-b0):
         * (r&0xff)>>1==0 -> +0x1d8 = (+0x1d8 & 0xffef) | (r<<4). */
        {
            int r = re15_enemy_los_probe(slot, e, pl);
            if ((r >> 1) == 0)
                e->ai_flags = (uint16_t)((e->ai_flags & ~0x10u) | ((uint16_t)r << 4));
        }
        /* +0x0 lifecycle bits @0x8010b61c-4c/+0x9&0x80 variant @0x8010b664-84 + FUN_80012aa4(0xbb8)
         * @0x8010b650-54 — the +0x0 render word / EXE aux are unmodeled port-wide (same stance as
         * the standard root). */
        if ((e->grid_id & 0x0f) != 0) break;        /* MODE dispatch @0x8010b694-b6b8 via @0x80120230:
                                                     * only mode 0 (FUN_8010b6d4) ships — mode 1
                                                     * (FUN_8010b784: tables @0x801202ec/@0x80120308,
                                                     * clip-26 walk 0x8010be50 with speed FIELD
                                                     * +0x8c=0x14 @0x8010be6c-70 — NOT a flat 30/tick
                                                     * advance, audit #6 — and steer 0x10 via jal
                                                     * 0x8001aac4 @0x8010bed8-dc, audit #8 citation
                                                     * fix) has ZERO spawns game-wide; modes 2..0xc =
                                                     * pose stubs @0x8010b800.. — all unrouted. */
        /* FUN_8010b6d4 anim-interrupt: (+0x1c0 & 0x9fff) == 0x8001 -> +0x5=9, +0x6=0
         * (@0x8010b6e4-b708). No +0x1c0 writer exists port-wide (documented OPEN, same as the
         * standard zombie's stagger release) -> the interrupt cannot fire yet. */
        {   /* DECIDE @0x80120264[+0x5] (@0x8010b718-38). Rows [0..0x10] == the standard f840 rows
             * (word-diff: only [0xa] differs and BOTH are jr-ra stubs — girl 0x8010bbe0, standard
             * 0x801033c0). NO bounds check -> +0x5 >= 0x11 reads PAST the 17-row table INTO the
             * animate table @0x801202a8 (byte-true overflow): decide[0x11] = word @0x801202a8 =
             * 0x80101d08 (search-stand ANIMATE), [0x12] = 0x80101ef0 (wander ANIMATE), [0x13] =
             * 0x801021f8 (ENGAGE ANIMATE — the girl's post-hurt 0x13 'approach' IS the aware
             * engage walk; she has NO 0x13 grab-commit decide, unlike the standard's 0x8010561c). */
            uint8_t sub = e->sub_state_1;
            if      (sub == 0x11) re15_enemy_ai_live_search_stand(e);
            else if (sub == 0x12) re15_enemy_ai_live_wander(slot, e);
            else if (sub == 0x13) re15_enemy_ai_live_engage_animate(slot, e);
            else                  re15_ai_dispatch_decision(e, pl);
        }
        {   /* ANIMATE @0x801202a8[+0x5] (@0x8010b740-6c; +0x5 RE-READ after the decide, exactly as
             * FUN_8010b6d4 does @0x8010b74c). Rows == the standard f890 rows except [0xa] = the girl
             * FLOOR-DROP 0x8010bbe8; overflow rows [0x11] = word @0x801202ec = 0x801035f8 (the
             * mode-1 idle decide) and [0x12]/[0x13] = 0x80103b8c (`jr ra` stub). */
            uint8_t sub = e->sub_state_1;
            if      (sub == 3 || sub == 4)      re15_enemy_ai_live_grab(e, pl);      /* [3]/[4]=0x80102548 */
            else if (sub == 0x0c)               re15_enemy_ai_feeding_animate(e, pl);/* [0xc]=0x801048e8 */
            else if (sub == 0x0d)               re15_enemy_ai_standup_animate(e);    /* [0xd]=0x80104a50 */
            else if (sub == 7)                  re15_enemy_ai_live_turn(e, pl);      /* [7]=0x80102dc8 */
            else if (sub == 5 || sub == 6)      re15_enemy_ai_live_devour(e, pl);    /* [5]/[6]=0x80102bd8 */
            else if (sub == 0x0b)               re15_enemy_ai_live_pushoff(e, pl);   /* [0xb]=0x8010385c */
            else if (sub == 8)                  re15_enemy_ai_live_charge(slot, e);  /* [8]=0x80103014 */
            else if (sub == 0x12 || sub == 0x13) { /* overflow [0x12]/[0x13]=0x80103b8c = jr ra */ }
            else if (sub == 0x11)               re15_zgirl_overflow_row11(e, pl);    /* overflow=0x801035f8 */
            else if (sub == 0)                  re15_enemy_ai_live_search_stand(e);  /* [0]=0x80101d08 */
            else if (sub == 1)                  re15_enemy_ai_live_wander(slot, e);  /* [1]=0x80101ef0 */
            else if (sub == 9)                  re15_enemy_ai_live_contact_stagger(e);/* [9]=0x801031e4 */
            else if (sub == 0x0a)               re15_zgirl_floor_drop(e);            /* [0xa]=0x8010bbe8 */
            else if (sub == 2)                  re15_enemy_ai_live_engage_animate(slot, e); /* [2]=0x801021f8 */
            /* rows [0xe]/[0xf]/[0x10] = 0x80109d98/0x80109e4c/0x80104f80 — identical to the
             * standard's rows and unported there too (deferred port-wide). */
        }
        break;
    }

    case 2:   /* HURT FUN_8010bf80 -> masters @0x8012039c == standard @0x8011fb90 (see header);
               * the girl downed router (+0x5==2 -> prone @0x8010bfa4-c4) is gated inside live_hurt.
               * Fixes the same-frame no-op reset (audit zombie-girl #4). */
        re15_enemy_ai_live_hurt(slot);
        break;

    case 3:   /* DEATH FUN_8010c014 -> master @0x8012063c == standard @0x8011feac; downed -> the
               * same FUN_80107cb0 (@0x8010c038). Death-fall clips + settle handoff via the shared
               * port (audit zombie-girl #3). */
        re15_enemy_ai_live_death(slot);
        break;

    case 7:   /* CORPSE @0x80120208[7] = 0x80109554 — the SAME shared corpse settle (pool spread,
               * creep, twitch, rest) as the standard zombie. */
        re15_enemy_corpse_settle(e);
        break;

    default:  /* [4] = 0x8010919c idle — deferred port-wide (same as the standard zombie). */
        break;
    }
}

/* ============================ ADULT SPIDER (type 0x25, EM025 = SPIDER) ====================== *
 * BYTE-TRUE rebuild of the 0x801109e4 family (STAGE2.BIN, overlay base 0x80100000; audit wf_efd92a2c
 * adult-spider — all 25 finder claims re-verified against RAW disasm, none refuted). Root 0x801109e4
 * (pause-gate DAT_800aca40&0x20000000, skip-gate +0x9&0x20) dispatches +0x4 via the 16-entry state
 * table @0x80118e24 -> shared body-tail (contact-clear/body-push/SCA-wall-clamp/box-rebuild). The
 * ACTIVE brain (state[1] 0x80110e50) is a DUAL-TABLE A/B FSM: grid-DECIDE @0x80118e44 by +0x9 (ONLY
 * when the commit-latch +0x5==0) then substep-ACT @0x80118e64 by +0x6 (ALWAYS). Its attack is a GRAB
 * (latch DAT_800aca58=2 + keep-alive clamp; NO direct player.hp write anywhere — the shared player
 * grab-FSM applies the damage).
 *   DECIDE cascades (each matching branch overwrites +0x6, LAST wins): ROAM @0x80111040 (grid 0),
 * APPROACH @0x80111240 (grid 1/5), ATTACK @0x80111488 (grid 2). NONE ever writes sub2=4 — CHASE is
 * reached ONLY from SCAN-faced and HURT-recover (the old port's `else if(los)->CHASE` was invented).
 * After a GRAB-A the brain resets grid:=0 @0x801126bc (ROAM family); GRAB-B re-rolls grid:=rng&1
 * @0x80112a84. Field reuse: +0x5=sub_state_1, +0x6=sub_state_2, +0x7=sub_state_3(phase), +0x9c=ai_timer,
 * +0x1d6=dog_atk_cd(lockout), +0x1d4=dog_dist, +0x8c=crow_speed, +0x1e6=speed_h(turn/scan latch),
 * +0x1dc=ai_target_x(stuck ctr).
 *   HONEST-OPEN (subsystem-scale, the port abstracts these): CLIMB/HOP/CEILING subs 9/10/12/13 and the
 * GRAB-A wall/ceiling transit (SCA +0x90/+0x1b4 data, wall-helper 0x801118a8); the 9-phase AMBUSH wall-
 * DROP trajectory (Y-descent/rot_x/floor) — only its trigger-gate is modeled; leg-dismember/debris
 * (model-part pool, +0x93&0x40) and the blood/gore FX 0x80019700; CORPSE color-tint/sink and DEATH
 * gore phases (render). The 0x80012aa4(4000) body-contact call @0x80110fd0 and +0x166=0xccc @0x80110df8
 * (unidentified consumer) are not modeled. */
static const uint16_t s_aspider_hp_row[16] =    /* HP table @0x8011761c = HPbase 0x8011717c + 0x25*0x20 */
    { 76,105,125,78,80,109,129,83,113,132,87,117,136,93,99,121 };   /* hp = row[rng()&0xf] @0x80110da0 */
/* idle-next-substep table @0x80118ea4 [2 rows x 8, idx=(LOS&1)*8 + (rng&7)] (byte-true). NOTE: consumed
 * ONLY by the grid-0 IDLE substep after its dwell timer @0x80111714-44 — NOT by the ROAM DECIDE (which is
 * the 6-branch cascade @0x80111040). (audit wf_efd92a2c adult-spider #25) */
static const uint8_t s_aspider_idle_next[16] =
    { 0x02,0x01,0x02,0x02,0x02,0x01,0x02,0x05,   /* row0 (no LOS) */
      0x0b,0x04,0x05,0x0b,0x03,0x05,0x04,0x04 }; /* row1 (LOS) */
static void re15_aspider_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_aspider_anim(re15_actor_t *e)
{
    re15_enemy_bank_t *bb = re15_enemy_find(e->type);   /* real EM025 clip lengths from the loaded bank */
    int fc = 30;                                        /* fallback when the bank is absent (unit test) */
    if (bb && bb->ok && e->motion < bb->anim.clip_count && bb->anim.clips[e->motion].frame_count > 0)
        fc = bb->anim.clips[e->motion].frame_count;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    if (e->anim_frac > 0) e->anim_frac--;
    return done;
}
static void re15_adult_spider_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x80110b6c: -> ACTIVE/AMBUSH, HP row, clip 0x10, steer=player */
        if (e->hp <= 0) e->hp = (int16_t)s_aspider_hp_row[re15_engine_rand8() & 0xf];  /* +0x9a @0x80110da0 */
        e->motion = 0x10; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;   /* clip 0x10 @0x80110b90, +0x8f=0 @0x80110bb0, +0x93=0 @0x80110bc0 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be = player @0x80110bd8/bf0 */
        e->ai_timer = 0; e->dog_atk_cd = 0; e->dog_dist = 0; e->ai_target_x = 0;  /* clear +0x9c/+0x1d6/+0x1dc */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        /* INIT pose-cluster (byte-true @0x80110c20-cc8): grid<2 -> rot_x/rot_z=0, Y=-1800*floor
         * (@0x80110c34-74); grid 2..0x1f -> rot_x=0, rot_z(pitch)=0x800, Y=-10800, grid:=2 (ceiling-
         * drop, @0x80110c84-cc8). @0x80110df8 +0x166=0xccc (unidentified consumer, no port field, OPEN). */
        if ((e->grid_id & 0xf) < 2) {                                    /* grid<2 @0x80110c28 */
            e->rot_x = 0; e->rot_z = 0; e->y = -1800 * (int32_t)e->floor;         /* @0x80110c54-74 */
        } else if ((unsigned)((e->grid_id & 0xffu) - 2u) < 0x1eu) {      /* grid 2..0x1f @0x80110c90 */
            e->rot_x = 0; e->rot_z = 0x800; e->y = -10800; e->grid_id = 2;        /* pitch + grid:=2 @0x80110cac-cc */
        }
        /* grid 0x40 -> state 4 sub 0, grid 0x41 -> state 4 sub 1 (AMBUSH) @0x80110cd0-14; else ACTIVE. */
        if (e->grid_id == 0x40)      { e->state = 4; e->sub_state_1 = 0; }   /* +0x4=0x004 @0x80110cf0 */
        else if (e->grid_id == 0x41) { e->state = 4; e->sub_state_1 = 1; }   /* +0x4=0x104 @0x80110d14 */
        else                         { e->state = 1; e->sub_state_1 = 0; }
        break;

    case 1: {  /* ACTIVE brain 0x80110e50 — DUAL-TABLE driver. Head: LOS(+0x1d0 bit0)+dist(+0x1d4). Then
                * grid-DECIDE @0x80118e44[+0x9] ONLY if the commit-latch +0x5==0, then substep-ACT
                * @0x80118e64[+0x6] ALWAYS. Tail: lockout-- and stuck-counter (byte-true @0x80110f8c). */
        int p = re15_enemy_los_probe(slot, e, pl);            /* 0x8001bc08 -> +0x1e0 @0x80110e58 */
        int los, rr; int32_t dist;
        if (p != 2) e->member_0b = (uint8_t)(p & 1);          /* +0x1d0 bit0 LATCHED on verdict tick @0x80110e70-bc */
        los  = e->member_0b & 1;
        dist = re15_enemy_player_dist(e, pl);                 /* SquareRoot0 -> +0x1d4 @0x80110efc */
        e->dog_dist = (int16_t)dist;

        if (e->sub_state_1 == 0) {                            /* grid-DECIDE gated on +0x5==0 @0x80110f20 */
            int grid = e->grid_id & 0xf;
            if (grid == 1 || grid == 5) {                     /* APPROACH 0x80111240 — 7-branch cascade */
                int faced80  = re15_dog_arc(e, pl, 0x7fff, 0x80);   /* +0x1e2 = arc_test(0x80) @0x80111258 (==0 -> faced) */
                if (los && re15_dog_arc(e, pl, 0x7fff, 0x100)) {}   /* +0x1e4 = arc_test(0x100) @0x80111270 (stored, no reachable consumer) */
                /* 0x701-latch && NOT-faced80 && aca5a==2 -> TURN(3) @0x8011128c-dc: modeled without the
                 * player-grab globals (aca58/aca5a not tracked in port) — OPEN, folded into the dist<8000 branch */
                if (dist < 8000 && !faced80) { e->sub_state_1 = 1; e->sub_state_2 = 3; e->sub_state_3 = 0; }  /* not facing -> TURN @0x801112ec-320 */
                if (e->dog_atk_cd == 0) {                     /* lockout gate @0x80111330 */
                    if (dist < 7500 && los) {                 /* lunge-trigger @0x80111340-80 */
                        e->sub_state_1 = 1; e->sub_state_2 = (uint8_t)(6 + (re15_engine_rand8() & 1)); e->sub_state_3 = 0;
                    }
                    if (dist < 6000 && faced80 && e->floor >= pl->floor) {   /* facing + close + floor-ok -> fixed SPIT(7) @0x80111384-e0 */
                        e->sub_state_1 = 1; e->sub_state_2 = 7; e->sub_state_3 = 0;
                    }
                }
                if (e->ai_target_x >= 31) { e->sub_state_1 = 1; e->sub_state_2 = 0xa; e->sub_state_3 = 0; }  /* stuck>=31 -> reroute(0xa) @0x80111440-70 */
            } else if (grid == 2) {                           /* ATTACK 0x80111488 — 4-branch */
                if (dist < 3000 && e->dog_atk_cd == 0) { e->sub_state_1 = 1; e->sub_state_2 = 8; e->sub_state_3 = 0; }  /* -> GRAB-B @0x8011149c-d0 */
                else if (dist >= 5001) {                      /* dist>=5001 -> SCAN(2) @0x8011151c-4c */
                    e->sub_state_1 = 1; e->sub_state_2 = 2; e->sub_state_3 = 0;
                    if (!re15_dog_arc(e, pl, 0x7fff, 0x80)) { e->sub_state_2 = 0xb; }  /* + not-faced -> CREEP(0xb) @0x80111550-a4 */
                }
            } else {                                          /* ROAM 0x80111040 — 6-branch cascade (grid 0) */
                if (dist < 6000 && !los) { e->sub_state_1 = 1; e->sub_state_2 = 3; e->sub_state_3 = 0; }  /* close, no LOS -> TURN(3) @0x80111058-88 */
                if (e->dog_atk_cd == 0) {                      /* +0x1d6==0 gate @0x80111098 */
                    if ((re15_engine_rand8() & 0xff) == 0 && los) { e->sub_state_1 = 1; e->sub_state_2 = 6; e->sub_state_3 = 0; }  /* rare LOS -> GRAB-A(6) @0x801110a8-ec */
                    if (dist < 7000 && re15_dog_arc(e, pl, 0x7fff, 0x80) == 0 && e->floor >= pl->floor) {  /* not-faced + floor -> 6+rng&1 @0x80111104-64 */
                        e->sub_state_1 = 1; e->sub_state_2 = (uint8_t)(6 + (re15_engine_rand8() & 1)); e->sub_state_3 = 0;
                    }
                }
                if (e->ai_target_x >= 31) { e->sub_state_1 = 1; e->sub_state_2 = 0xa; e->sub_state_3 = 0; }  /* stuck>=31 -> 0xa @0x801111c4-f4 */
                if (pl->hit_react & 1)    { e->sub_state_1 = 1; e->sub_state_2 = 0xb; e->sub_state_3 = 0; }  /* player mid-react -> CREEP(0xb) @0x801111f8-2c */
            }
        }

        switch (e->sub_state_2) {                             /* substep-ACT @0x80118e64[+0x6] */
        case 0:   /* IDLE 0x801115e4: grid-0 idle-timer + idle_next; grid>=1 clip-0 hold */
            if (e->motion != 0) re15_aspider_clip(e, 0);      /* clip 0 @0x80111670/8f=7 */
            if ((e->grid_id & 0xf) == 0) {                    /* grid-0 idle @0x8011162c */
                if (e->ai_timer == 0 && e->sub_state_3 == 0) {                       /* seed timer once */
                    e->ai_timer = (int16_t)((re15_engine_rand8() & 0x3f) + (los ? 0 : 80));  /* +0x9c=(rng&0x3f)+(LOS?0:80) @0x80111690-c4 */
                    e->sub_state_3 = 1;
                }
                if (e->ai_timer > 0 && --e->ai_timer == 0) {                         /* expiry @0x801116f8 */
                    e->sub_state_1 = 0;                                              /* +0x5=0 @0x80111710 */
                    e->sub_state_2 = s_aspider_idle_next[(los ? 8 : 0) + (re15_engine_rand8() & 7)];  /* idle_next @0x80118ea4[(LOS)*8+(rng&7)] @0x80111714-44 */
                    e->sub_state_3 = 0;
                }
            }                                                 /* grid>=1: clip-0 hold until DECIDE fires @0x80111768 */
            re15_aspider_anim(e);
            break;
        case 1:   /* WALK 0x801117fc: clip 3, LOS-steer 0x10, +0x8c += 60, advance (grid2 = rear 0x800) */
            if (e->motion != 3) re15_aspider_clip(e, 3);      /* clip 3 @0x80111808 */
            if (los) re15_enemy_steer_point(e, pl->x, pl->z, 0x10);  /* rot += (LOS&1)*arc(0x10) @0x8011181c-50 */
            e->crow_speed = (int16_t)(e->crow_speed + 60);    /* +0x8c += 60 @0x80111868 */
            /* wall-helper 0x801118a8 (SCA-driven CLIMB transit) = OPEN */
            re15_dog_advance(e, e->crow_speed >> 5);          /* pos_advance @0x80111890 (grid2 rear-0x800 offset = OPEN, ceiling only) */
            re15_aspider_anim(e);
            break;
        case 2:   /* SCAN 0x80111c5c: clip 3, sweep step ±0x18 LATCHED once, faced(±0x18) -> CHASE */
            if (e->sub_state_3 == 0) {                        /* phase 0 @0x80111c8c */
                re15_aspider_clip(e, 3);                      /* clip 3 @0x80111c9c */
                e->ai_timer  = (int16_t)(re15_engine_rand8() & 0x3f);          /* +0x9c=rng&0x3f @0x80111cc0 */
                e->speed_h   = (int16_t)((e->ai_timer & 1) ? -0x18 : 0x18);    /* +0x1e6 latched ±0x18 ONCE @0x80111ce0-08 */
                e->sub_state_3 = 1;
            }
            e->rot_y = (int16_t)((e->rot_y + e->speed_h) & 0xfff);            /* rot_y += latched step @0x80111d24 */
            if (los && re15_dog_arc(e, pl, 0x7fff, (e->speed_h < 0 ? -e->speed_h : e->speed_h))) {  /* faced within ±0x18 -> CHASE(4) @0x80111d5c-90 */
                e->sub_state_2 = 4; e->sub_state_3 = 0;
            } else if (e->ai_timer > 0 && --e->ai_timer == 0) {              /* expiry @0x80111da8 */
                e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 4 : 0); e->sub_state_3 = 0;  /* +0x5=0, +0x6=LOS?4:0 @0x80111dc0-e0 */
            }
            re15_aspider_anim(e);
            break;
        case 3: case 5: {  /* TURN 0x80111e04 (clip 2, win 0x80) / TURN-narrow 0x8011207c (clip 3, win 0x20) */
            int win = (e->sub_state_2 == 3) ? 0x80 : 0x20;    /* +0x1e6 @0x80111e24 / @0x8011209c */
            if (e->sub_state_3 == 0) {                        /* phase 0 @0x80111e8c */
                re15_aspider_clip(e, (uint8_t)(e->sub_state_2 == 3 ? 2 : 3));  /* clip 2 (TURN) / 3 (narrow) @0x80111e18 / @0x80112090 */
                e->ai_timer = (int16_t)((re15_engine_rand8() & 0x1f) + 80);    /* +0x9c=(rng&0x1f)+80 @0x80111ec0 */
                e->sub_state_3 = 1;
            } else if (e->sub_state_3 == 1) {                 /* phase 1: slew + exit test @0x80111ec8 */
                int faced = re15_dog_arc(e, pl, 0x7fff, win);
                re15_enemy_steer_point(e, pl->x, pl->z, win); /* rot += arc(win) toward player @0x80111f6c */
                if ((e->ai_timer > 0 && --e->ai_timer == 0) || faced) e->sub_state_3 = 2;  /* timer==0 OR faced -> phase2 @0x80111f10-30 */
            } else {                                          /* phase 2 @0x80111f84 */
                e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 11 : 0); e->sub_state_3 = 0;  /* +0x5=0, +0x6=11*LOS @0x80111f84-b0 */
                e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;      /* +0x1bc/+0x1be=player @0x80111fc4-f0 */
            }
            re15_aspider_anim(e);
            break;
        }
        case 4:   /* CHASE 0x80112004: clip 2, steer 0x30, +0x8c += 100, advance. NO exit (persists). */
            if (e->motion != 2) re15_aspider_clip(e, 2);      /* clip 2 @0x80112028 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);    /* rot += arc(0x30) @0x80112024-48 */
            /* wall-helper 0x801118a8 = OPEN */
            e->crow_speed = (int16_t)(e->crow_speed + 100);   /* +0x8c += 100 @0x80112060 */
            re15_dog_advance(e, e->crow_speed >> 5);           /* pos_advance @0x80112064 */
            re15_aspider_anim(e);
            break;
        case 11:  /* CREEP 0x8011302c: clip 3, steer 0x10, +0x8c -= 20 UNCONDITIONAL, advance. */
            if (e->motion != 3) re15_aspider_clip(e, 3);      /* clip 3 @0x80113038 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x10);    /* rot += arc(0x10) @0x8011304c-70 */
            /* wall-helper 0x801118a8 = OPEN */
            e->crow_speed = (int16_t)(e->crow_speed - 20);     /* +0x8c -= 20 every frame (no init/clamp) @0x80113088 */
            re15_dog_advance(e, e->crow_speed >> 5);           /* pos_advance @0x8011308c */
            re15_aspider_anim(e);
            break;
        case 6:   /* GRAB-A 0x801120b8 (walking lunge-bite, +0x7 9-phase @0x8010019c). NON-DAMAGING. */
            if (e->sub_state_3 == 0) { re15_aspider_clip(e, 8); e->sub_state_3 = 1; }         /* [0] clip 8 windup @0x801122cc */
            else if (e->sub_state_3 <= 2) {                                                    /* [1] advance @0x8011262c */
                re15_dog_advance(e, 100 >> 5);
                if (re15_aspider_anim(e)) { re15_aspider_clip(e, 0x0b); e->ai_timer = (int16_t)((re15_engine_rand8() & 0x1f) + 20); e->sub_state_3 = 3; }  /* -> clip 0xb track, timer=(rng&0x1f)+20 @0x80112310-64 */
            } else if (e->sub_state_3 == 3) {                                                  /* [3] track: face; expiry->recover @0x80112368 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x40);                                 /* arc(0x40) track @0x8011238c */
                if (re15_dog_arc(e, pl, 0x7fff, 0x40)) e->sub_state_3 = 4;                     /* faced -> STRIKE @0x801123d8 */
                else if (e->ai_timer > 0 && --e->ai_timer == 0) e->sub_state_3 = 6;            /* timer==0 -> recover @0x801123f8-410 */
                re15_aspider_anim(e);
            } else if (e->sub_state_3 == 4) {                                                  /* [4] enter STRIKE @0x80112418 */
                re15_aspider_clip(e, 0x0b); e->ai_timer = (int16_t)((re15_engine_rand8() & 0x1f) + 10); e->sub_state_3 = 5;  /* clip 0xb, timer=(rng&0x1f)+10 @0x80112448-6c */
            } else if (e->sub_state_3 == 5) {                                                  /* [5] STRIKE body @0x80112470 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x10);                                 /* arc(0x10) @0x8011248c-b8 */
                re15_dog_advance(e, 0x12c >> 5);                                               /* pos_advance(300) @0x801124c4 */
                if ((e->ai_timer > 0 && --e->ai_timer == 0) || !re15_dog_arc(e, pl, 0x7fff, 0x200)) {  /* timer==0 OR lost cone(0x200) -> miss/recover @0x801124e8-510 */
                    e->sub_state_3 = 6;
                } else if (pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x100)) {  /* CONNECT (+0x1d2 hitbox && !reacting) @0x80112514-40 */
                    re15_audio_room_se(2); s_player_grabbed = 1; pl->hit_react |= 1;           /* SE 0x800453d0(2), aca58=2 @0x8011254c-58 */
                    if (pl->hp < 0) pl->hp = 1;                                                /* keep-alive clamp (NO damage) @0x80112594-9c */
                    e->sub_state_3 = 6;
                }
            } else if (e->sub_state_3 == 6 || e->sub_state_3 == 7) {                           /* [6] clip 9 recover @0x801125dc */
                if (e->motion != 9) re15_aspider_clip(e, 9);                                   /* clip 9 @0x80112618 */
                re15_dog_advance(e, 100 >> 5);                                                 /* pos_advance(100) @0x80112670 */
                if (re15_aspider_anim(e)) e->sub_state_3 = 8;
            } else {                                                                          /* [8] reset @0x80112680 */
                e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;                    /* +0x5=0, +0x6=0 @0x8011268c-9c */
                e->dog_atk_cd = 0xf; e->grid_id = 0;                                           /* +0x1d6=0xf, grid:=0 (ROAM family) @0x801126ac-bc */
            }
            break;
        case 7:   /* SPIT 0x801126d4 (venom, distinct from GRAB-A): clip 0xc + FX + SE(4) -> CREEP + lockout */
            if (e->sub_state_3 == 0) {                                                         /* phase 0 @0x80112720 */
                re15_aspider_clip(e, 0x0c);                                                    /* clip 0xc @0x80112750 */
                /* venom particle 0x80019700(0x0d002000,rot_y,bone+0x40,@0x80118ee8) @0x80112770 = OPEN (FX) */
                re15_audio_room_se(4);                                                         /* SE 0x800453d0(4) @0x80112778 */
                e->sub_state_3 = 1;
            } else if (e->sub_state_3 == 1) {                                                  /* phase 1 @0x80112780 */
                if (re15_aspider_anim(e)) e->sub_state_3 = 2;
            } else {                                                                          /* phase 2 @0x801127bc */
                e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0x0b; e->sub_state_3 = 0;   /* sh 1,4 (+0x5=0), +0x6=0xb CREEP @0x801127bc-cc */
                e->dog_atk_cd = 0xf;                                                           /* +0x1d6=0xf @0x801127d8 */
            }
            break;
        case 8:   /* GRAB-B 0x801127f0 (ballistic LEAP, +0x7 5-phase @0x801001c4). NON-DAMAGING. */
            if (e->sub_state_3 == 0) { re15_aspider_clip(e, 0); e->ai_timer = 0; e->sub_state_3 = 1; }  /* [0] clip 0, timer 0, NO steer @0x80112834-80 */
            else if (e->sub_state_3 == 1) {                                                    /* [1] parabola @0x80112884 */
                e->y += 8 * (int32_t)e->ai_timer * (int32_t)e->ai_timer; e->ai_timer++;        /* Y += 8*t^2 @0x80112898-b4 */
                e->rot_z = (int16_t)(e->rot_z + 341); if (e->rot_z >= 0) e->rot_z = 0;         /* rot_x(+0x6c) += 341 clamp 0 @0x801128c4-f0 */
                { int32_t floor_y = -1800 * (int32_t)e->floor;                                 /* +0x1ba current floor @0x801128fc-04 */
                  if (pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x100)) {  /* CONNECT @0x8011299c-c0 */
                      re15_audio_room_se(2); s_player_grabbed = 1; pl->hit_react |= 1;         /* SE(2), aca58=2 @0x801129c4-d4 */
                      if (pl->hp < 0) pl->hp = 1;                                              /* keep-alive @0x80112a0c-14 */
                  }
                  if (floor_y < e->y) { e->y = floor_y; e->floor = (uint8_t)(-e->y / 1800); e->sub_state_3 = 2; }  /* land at floor, recompute +0x82 @0x80112900-70 */
                }
            } else if (e->sub_state_3 == 2) {                                                  /* [2] clip 7 + grid re-roll @0x80112a2c */
                re15_aspider_clip(e, 7);                                                       /* clip 7 @0x80112a6c */
                e->grid_id = (uint8_t)(re15_engine_rand8() & 1);                               /* grid := rng&1 @0x80112a70-84 */
                e->sub_state_3 = 3;
            } else if (e->sub_state_3 == 3) {                                                  /* [3] recover anim @0x80112a88 */
                if (re15_aspider_anim(e)) e->sub_state_3 = 4;
            } else {                                                                          /* [4] reset @0x80112ac4 */
                e->sub_state_1 = 0; e->sub_state_2 = 0;                                        /* +0x5=0, +0x6=0 ONLY (no lockout, no grid keep) @0x80112ad0-e0 */
            }
            break;
        default:  /* CLIMB/HOP/CEILING subs 9/10/12/13/14/15 = SCA-driven, subsystem-scale -> OPEN.
                   * Minimal fallback: resume the driver so a spawned actor never wedges. */
            e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 4 : 0); e->sub_state_3 = 0;
            re15_aspider_anim(e);
            break;
        }

        /* ACTIVE tail @0x80110f8c: lockout-- (AFTER decide) then stuck-counter += 1 if wall-blocked. */
        if (e->dog_atk_cd) e->dog_atk_cd--;                   /* +0x1d6-- @0x80110fe4 (port: was at head, moved to tail) */
        /* 0x80012aa4(4000) body-contact call (grid<2||grid==5) @0x80110fd0 = OPEN */
        rr = re15_dog_blocked(e);                             /* +0x1da SCA wall-clamp proxy (ai_contact) */
        e->ai_target_x = (int16_t)(rr ? e->ai_target_x + 1 : 0);  /* +0x1dc stuck counter @0x80110ffc-30 */
        break;
    }

    case 4:   /* AMBUSH 0x80114e18[+0x5] (grid 0x40 -> 0x80114e60, grid 0x41 -> 0x80114ec8): CLING + HOLD
               * until the grid is changed EXTERNALLY (SCD trigger), then drop. Both handlers do NOTHING
               * while their spawn grid is unchanged. The 9-phase wall-DROP choreography (Y-descent/rot_x/
               * SCA floor) and the grid-0x40 CLIMB descent are subsystem-scale -> OPEN. */
        if (e->motion != 0x10) re15_aspider_clip(e, 0x10);    /* cling pose clip 0x10 @0x80114f34 */
        if (e->sub_state_1 == 1) {                            /* grid-0x41 @0x80114ec8: phase 0 waits while grid==0x41 @0x80114f14 */
            if (e->grid_id != 0x41) {                         /* externally triggered -> land into APPROACH */
                e->grid_id = 5; e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* grid:=5, exit sh 1,4 @0x80115244-a0 (drop = OPEN) */
            }
        } else {                                              /* grid-0x40 @0x80114e60: hold while grid==0x40 @0x80114e74 */
            if (e->grid_id != 0x40) {                         /* externally triggered -> CLIMB(9) descent */
                e->grid_id = 0; e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 9; e->sub_state_3 = 0;  /* grid:=0, sh 0x101,4, +0x6=9 @0x80114e7c-bc (CLIMB = OPEN) */
            }
        }
        re15_aspider_anim(e);
        break;

    case 2:   /* HURT 0x80113a0c: flinch clip 1 + Se(2); recover byte-true (@0x80113e84). +0xb0/+0x1ba
               * reset @0x80113a2c and the blood FX 0x80019700 + leg-dismember (+0x93&0x40) = OPEN. */
        if (e->sub_state_3 == 0) { re15_aspider_clip(e, 1); re15_audio_room_se(2); e->sub_state_3 = 1; }  /* flinch @0x80113a0c */
        else if (re15_aspider_anim(e)) {                      /* recover @0x80113e84 */
            int los; int pp = re15_enemy_los_probe(slot, e, pl);  /* +0x1d0 bit0 @0x80113f04 */
            if (pp != 2) e->member_0b = (uint8_t)(pp & 1);
            los = e->member_0b & 1;
            e->state = 1; e->hit_react = 0;                   /* state=1, +0x93:=0 FULL clear @0x80113ef4 */
            e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be=player @0x80113ec4-e4 */
            if (los) { e->sub_state_1 = 1; e->sub_state_2 = 0xa; }      /* LOS -> +0x5=1, +0x6=0xa @0x80113f18-28 */
            else     { e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(3 + (e->grid_id & 1)); }  /* else +0x6=3+(grid&1) @0x80113e9c-a4 */
            e->sub_state_3 = 0;
        }
        break;

    case 3:   /* DEATH 0x80113f40: entry kill-flag 0x8004ef90(0x800b1038,+0x1c6); collapse clip 1 + Se(5)
               * -> CORPSE (state 7). The 2-stage gore (part-detach x2, phased FX) = OPEN (render). */
        if (e->sub_state_3 == 0) {
            re15_aspider_clip(e, 1); re15_audio_room_se(5); e->hit_react |= 2;
            if (e->em_flag_id != 0xFF)
                re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);  /* kill-flag persist @0x80113fa0-ac */
            e->sub_state_3 = 1;
        } else if (re15_aspider_anim(e)) { e->state = 7; e->sub_state_3 = 0; }
        break;

    case 7:   /* CORPSE 0x801153d4: settle clip 5, inert. Phased timer(0x5a)/flags(0x2|0x40)/color-tint/
               * sink(+8/frame)/Se(9) @0x80115414-5cc = OPEN (render/cosmetic). */
        if (e->motion != 5) re15_aspider_clip(e, 5);
        re15_aspider_anim(e);
        break;

    default:  /* unreachable states -> resume the brain */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ COCKROACH (type 0x29, EM029) — STAGE3 ========================= *
 * THOROUGH byte-true rebuild of the 0x80110b00 family (STAGE3.BIN, overlay base 0x80100000; audit
 * wf_efd92a2c cockroach). A small flying scurrier — NOT a "no-flee" maggot: after a survived hit it
 * LEAPS/FLIES AWAY (subs 7/9). Root 0x80110b00 dispatches +0x4 via the 8-entry state table @0x8011eca4
 * ([0]=INIT@0x80110c98 [1]=ACTIVE@0x8011101c [2]=HURT@0x80114790 [3]=DEATH@0x80114fb4 [4]=[5]=0x80115734
 * [6]=0x80116228 [7]=CORPSE@0x80115a6c). The ACTIVE brain is a two-table A/B FSM: decide A[+0x5]@0x8011ecc4
 * then act B[+0x5]@0x8011ed04. It deals DIRECT player.hp damage: BITE -5 (clip 0x12) + HEAVY -10 (clip 0x13).
 *
 * State word +0x4 = {state(+4), sub(+5), phase(+6=sub_state_2), sighted-flag(+7=sub_state_3)}. The +0x7
 * SIGHTED/BLIND flag selects the walk clip (5 sighted / 4 blind), the root-motion window and the SE frames.
 * The +0x1dc attack cooldown (dog_atk_cd) decrements ONCE per ACTIVE frame in the brain tail (@0x801111c4),
 * NOT per-substate. LOS is a LATCHED bit (+0x1d0 roach_los): only a 0/1 probe verdict updates it (@0x8011105c).
 *
 * SPAWN behavior comes from the grid/behavior byte (+0x9 grid_id): beh&1 = DORMANT (spawn into state1 sub1
 * sleep, wake to sub 2); beh&2 sets roach_beh2 (HURT-exit -> sub 5); INIT sets roach_esc(+0x1e3)=1 default,
 * cleared by beh&4 (HURT-exit -> sub 9 fly-away). All shipped roaches (beh 0x00/0x61) keep esc=1 -> the
 * DEFAULT post-hurt path is the sub-9 fly-away escape.
 *
 * HONEST-OPEN (subsystems the port lacks wholesale; exact bytes cited, never faked):
 *  - Locator-bone root motion FUN_80115b68 drives the APPROACH (sub 3) forward speed (window 0x18 sighted /
 *    0x29 blind); the port applies the byte-true per-frame clip root delta via the loaded EM029 bank
 *    (re15_clip_root_motion_delta), which is exact IN-GAME but displaces nothing when the bank is absent —
 *    exact approach speed is the locator subsystem. The SCURRY (sub 4) uses the byte-true additive +0x8c
 *    speed (180-211/frame) via re15_dog_advance and is pixel-exact.
 *  - Attack-box overlap 0x8001bff8 (skeletal part spheres: BITE part+1612 r=1000; HEAVY parts+1096/+1784
 *    r=800) drives every connect; the port proxies it with re15_dog_arc (dist+cone), as at every other
 *    melee site. The window frames, damage, lockout and SE ids are byte-true; only the hit geometry is a proxy.
 *  - Flight subsystem (subs 7/8/9): the leap launch (+0x8c=(rng&0x1f)+200 @0x801124f8), airborne latch
 *    (+0x1e0), wall-hit -> sub-9 handoff and the fly-then-land -> idle FSM ARE ported; the flight-MODEL swap
 *    (+0x78 = @0x8011ec48 @0x80112e88), the ballistic helper 0x8001c1a4 (gravity -60) and the "flee to a
 *    ROOM point" nav-steer (0x80019700 from @0x8011ec84) are render/nav subsystems -> forward crow_speed proxy.
 *    The A[4] leap-escape extra gates (aca58==0x701 || +0x1d6==0 || 0x8001a9cc(0x20)==0 || 0x8001a780==0)
 *    are the aca58 player-cmd / narrow-arc subsystems, proxied by los && far && rng&1.
 *  - Blood/gore FX (0x80019700 @0x80115184/0x80114960) + the corpse colour-dim (+0xc4/+0xec masked
 *    0xff000000|0x00ffff38, pool spread +0xbc/+0xbe) are render-side; the corpse flags |0x2|0x40 and the
 *    0x5a fade counter ARE ported (audio SE calls ARE ported). */
static const uint16_t s_roach_hp_row[16] =      /* HP table @0x8011d6e8 (base 0x8011d1c8 + 0x29*0x20) */
    { 81,109,97,83,99,113,87,101,117,89,91,103,121,93,105,95 };   /* hp = row[rng()&0xf] = 81-121 */
static const uint8_t s_roach_clip_len[32] =     /* EM029 bank0 dir[1] EDD clip frame-counts (32 clips) */
    { 78,20,15,70,78,39,24,12,25,25,40,40,100,40,70,90,35,20,25,40,40,21,58,30,50,40,70,30,39,20,51,70 };
static void re15_roach_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_roach_anim(re15_actor_t *e)
{
    uint8_t c = e->motion; int fc = (c < 32) ? s_roach_clip_len[c] : 1; if (fc < 1) fc = 1;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    if (e->anim_frac > 0) e->anim_frac--;
    return done;
}
static void re15_cockroach_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x80110c98/0x80110cbc: -> ACTIVE, HP row, idle clip 0x16, grid-nibble behavior */
        if (e->hp <= 0) e->hp = (int16_t)s_roach_hp_row[re15_engine_rand8() & 0xf];   /* +0x9a, HP 81-121 */
        e->motion = 0x16; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;      /* clip 0x16 @0x80111310 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
        e->ai_timer = 0; e->dog_atk_cd = 0; e->dog_dist = 0;
        e->roach_los = 0; e->roach_air = 0; e->roach_fade = 0;
        e->roach_esc  = 1;                                                            /* +0x1e3 = 1 default @0x80110e18 (sb a0,483; a0=1) */
        if (e->grid_id & 4) e->roach_esc = 0;                                         /* cleared by beh&4 @0x80110e28-3c */
        e->roach_beh2 = (uint8_t)((e->grid_id & 2) ? 1 : 0);                          /* +0x1e2 = beh&2 (HURT-exit sub 5) */
        e->state = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->sub_state_1 = (uint8_t)((e->grid_id & 1) ? 1 : 0);                         /* beh&1 -> state1 SUB 1 dormant @0x80110cbc; else sub 0 */
        break;

    case 1: {  /* ACTIVE two-table A/B brain 0x8011101c (A decide @0x8011ecc4 + B act @0x8011ed04 on +0x5) */
        int r = re15_enemy_los_probe(slot, e, pl);            /* 0x8001bc08 @0x8011101c -> +0x1d8 */
        if ((r >> 1) == 0) e->roach_los = (uint8_t)(r & 1);   /* only a 0/1 verdict LATCHES +0x1d0 @0x8011105c-88 */
        int los = e->roach_los;                               /* every decision reads the LATCHED bit @0x80111250 */
        int32_t dist = re15_enemy_player_dist(e, pl);         /* SquareRoot0 -> +0x1d4 */
        e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 0:  /* A[0] idle 0x80111220 + B[0] 0x80111ee4: clip 0x16 */
            if (e->dog_atk_cd == 0) {                                                 /* +0x1dc!=0 freezes ALL idle decisions @0x8011122c */
                if (dist < 0x1388 && los)                       { re15_dog_sub(e, 3); e->sub_state_3 = 0; break; }   /* sighted engage @0x80111264 (+0x7=0) */
                if (dist < 0x1770 && re15_dog_player_aiming())  { re15_dog_sub(e, 4); break; }                       /* aca58==0x701 -> SCURRY @0x80111280 */
                if (dist < 0xbb8)                               { re15_dog_sub(e, 3); e->sub_state_3 = 1; break; }   /* blind close engage @0x801112e0 (+0x7=1) */
                if (e->sub_state_2 == 0) { re15_roach_clip(e, 0x16); e->ai_timer = (int16_t)(re15_engine_rand8() + 0x3b); e->sub_state_2 = 1; }  /* idle timer rng+59 */
                else if (--e->ai_timer <= 0) { re15_dog_sub(e, 3); e->sub_state_3 = 1; break; }                     /* timer expiry -> approach @0x801113d0 */
            }
            re15_roach_anim(e);
            break;
        case 1:  /* B[1] DORMANT sleep 0x80111500 (clip 0) + A[1] wake decision 0x80111404 */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 0); e->sub_state_2 = 1; }   /* sleep clip 0 (sb zero,148 @0x80111554) */
            if (dist < 0x1388 && los)                      { e->sub_state_3 = 0; re15_dog_sub(e, 2); break; }   /* wake sighted @0x80111404 (+0x7=0) */
            if (dist < 0x1770 && re15_dog_player_aiming()) { e->sub_state_3 = 1; re15_dog_sub(e, 2); break; }   /* wake aim (+0x7=1) */
            if (dist < 0xbb8)                              { e->sub_state_3 = 2; re15_dog_sub(e, 2); break; }   /* wake close (+0x7=2) */
            re15_roach_anim(e);
            break;
        case 2: {  /* B[2] WAKE 0x801115fc: wake clip {3,2,1}[+0x7] + SE(3)@frame 0x1d -> approach sub 3 */
            static const uint8_t wake_clip[3] = { 3, 2, 1 };                          /* @0x801116b4-c8 by +0x7 */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, wake_clip[e->sub_state_3 < 3 ? e->sub_state_3 : 0]); e->sub_state_2 = 1; }
            if (e->sub_state_3 == 0 && e->anim_frame == 0x1d) re15_audio_room_se(3);  /* SE(3) @0x801116e0-fc (+0x7==0 variant) */
            if (re15_roach_anim(e)) { re15_dog_sub(e, 3); e->sub_state_3 = 1; }        /* wake done -> approach @0x80111744-c4 */
            break;
        }
        case 3: {  /* A[3] decide 0x801117d8 + B[3] APPROACH 0x80111910: clip 5 sighted / 4 blind, locator root-motion */
            if (e->dog_atk_cd == 0 && pl->hit_react == 0 && re15_dog_arc(e, pl, 3000, 0x180)) {   /* in range -> BITE */
                re15_roach_clip(e, 0x12); re15_dog_sub(e, 5); break; }
            if (dist >= 6001 && los) { re15_dog_sub(e, 4); break; }                    /* far + LOS -> fast SCURRY */
            if (e->sub_state_2 == 0) {
                re15_roach_clip(e, 4);                                                 /* clip 4 (ori 0x4; sb 148 @0x8011193c) */
                if (e->sub_state_3 == 0) re15_roach_clip(e, 5);                        /* clip 5 when +0x7==0 sighted @0x8011195c */
                e->sub_state_2 = 1;
            }
            re15_enemy_steer_point(e, pl->x, pl->z, (int16_t)((re15_engine_rand8() & 0x1f) + 6));  /* turn = (rng&0x1f)+6 @0x801119a4 */
            { int win = (e->sub_state_3 == 0) ? 0x18 : 0x29;                           /* root-motion / SE window @0x80111a18/64 */
              if (e->anim_frame == 1 || e->anim_frame == (uint32_t)win) re15_audio_room_se(8); }  /* footstep SE(8) @0x80111a44-a8 */
            { int fr_prev = (int)e->anim_frame;                                        /* locator root-motion 0x80115b68 (OPEN: exact speed = EM029 locator bank) */
              re15_enemy_bank_t *bank = re15_enemy_find(e->type);
              re15_roach_anim(e);
              if (bank) re15_clip_root_motion_delta(e, &bank->skel, &bank->anim, (int)e->motion, (int)e->anim_frame, fr_prev); }
            break;
        }
        case 4:  /* A[4] decide 0x80111ac0 + B[4] SCURRY 0x80111c08: clip 6, +0x8c re-rolled 180-211 EVERY frame */
            if (e->dog_atk_cd == 0 && pl->hit_react == 0 && re15_dog_arc(e, pl, 4000, 0xc0)) {   /* in range -> HEAVY */
                re15_roach_clip(e, 0x13); re15_dog_sub(e, 6); break; }
            if (los && dist >= 0x1771 && (re15_engine_rand8() & 1)) { re15_dog_sub(e, 7); break; }  /* leap-escape gate @0x80111b30-d0 (extra gates OPEN) */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 6); e->sub_state_2 = 1; }
            e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180);             /* +0x8c=180..211 re-rolled EVERY tick @0x80111c5c-78 */
            re15_enemy_steer_point(e, pl->x, pl->z, (int16_t)((re15_engine_rand8() & 0x1f) + 64));  /* +0x9e=(rng&0x1f)+64 @0x80111c7c */
            if (((int)e->anim_frame % 13) == 1) re15_audio_room_se(8);                 /* footstep SE(8) every 13th frame @0x80111d04-44 */
            re15_dog_advance(e, e->crow_speed);                                        /* pos_advance +0x8c UNSHIFTED (walker @0x800245f0) @0x80111d48 */
            re15_roach_anim(e);
            break;
        case 5:  /* B[5] BITE 0x80111d68 (clip 0x12): part-box connect on ANY of frames 12-15 -> -5 */
            if (e->anim_frame == 0x0b) re15_audio_room_se(9);                          /* windup SE(9) @0x80111e90-a0 */
            if (e->anim_frame >= 0x0c && e->anim_frame <= 0x0f && pl->hit_react == 0
                && re15_dog_arc(e, pl, 1000, 0x400)) {                                 /* box part+1612 radius 0x3e8=1000 (proxy) @0x80111eb8 */
                pl->hp = (int16_t)(pl->hp - 5);                                        /* player.hp -= 5 @0x80111f60 */
                re15_audio_room_se(5);                                                 /* SE(5) @0x80111f4c */
                pl->hit_react |= 1; e->dog_atk_cd = 0x2d;                              /* lockout +0x1dc=0x2d */
            }
            if (re15_roach_anim(e)) { re15_dog_sub(e, 3); e->sub_state_3 = 1; if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14; }
            break;
        case 6:  /* B[6] HEAVY 0x80112044 (clip 0x13): window {21,22,23,24,33,34} -> -10; chains to BITE when in-arc */
            if (e->anim_frame == 0x14) re15_audio_room_se(9);                          /* windup SE(9) @0x80112174-84 */
            { int f = (int)e->anim_frame;
              int in_window = (f >= 0x15 && f <= 0x18) || (f == 0x21) || (f == 0x22);  /* table @0x8011ed4c = {21,22,23,24,33,34} */
              if (in_window && pl->hit_react == 0 && re15_dog_arc(e, pl, 800, 0x400)) {/* parts+1096/+1784 radius 0x320=800 (proxy) @0x801121e8/0x80112208 */
                  pl->hp = (int16_t)(pl->hp - 10);                                     /* player.hp -= 10 @0x801122b4 */
                  re15_audio_room_se(4);                                               /* SE(4) @0x80112290 (NOT 5) */
                  pl->hit_react |= 1; e->dog_atk_cd = 0x2d;                            /* lockout 0x2d @0x801122bc */
              } }
            if (re15_roach_anim(e)) {                                                  /* B[6] phase 2 @0x8011232c */
                if (pl->hit_react == 0 && e->dog_atk_cd == 0 && re15_dog_arc(e, pl, 4000, 0x180)) {
                    re15_roach_clip(e, 0x12); re15_dog_sub(e, 5);                      /* still in arc & cd clear -> immediate follow-up BITE @0x80112374 */
                } else {
                    re15_dog_sub(e, 3); e->sub_state_3 = 1;
                    if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14;                      /* cd=0x14 ONLY on the out-of-arc path @0x80112398 */
                }
            }
            break;
        case 7:  /* B[7] LEAP-ESCAPE 0x801123fc: clip 0x14 launch; wall-hit -> flight sub 9 */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 0x14); e->sub_state_2 = 1; }  /* clip 0x14 @0x80112460 */
            if (e->anim_frame == 0xa) {                                                 /* launch frame @0x801124f8 */
                e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 200);          /* +0x8c=(rng&0x1f)+200 @0x801124f8 */
                e->roach_air = 1; e->hit_react |= 1;                                    /* airborne +0x1e0=1 @0x8011250c, +0x93|=1 */
            }
            if (e->roach_air) re15_dog_advance(e, e->crow_speed);                       /* ballistic forward (OPEN: room-point steer 0x8001c1a4 gravity -60) */
            if (e->roach_air && re15_dog_blocked(e)) { e->state = 2; re15_dog_sub(e, 9); e->roach_air = 0; break; }  /* wall-hit +0x93&0x40 -> state2 sub9 @0x8011254c-7c */
            if (re15_roach_anim(e)) { re15_dog_sub(e, 9); }                             /* leap done -> flight sub 9 */
            break;
        case 8:  /* B[8] LEAP variant 0x80112784 — same escape family (OPEN: exact variant divergence) */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 0x14); e->sub_state_2 = 1; }
            if (e->roach_air) re15_dog_advance(e, e->crow_speed);
            if (re15_roach_anim(e)) { re15_dog_sub(e, 9); }
            break;
        case 9:  /* B[9] FLIGHT-ESCAPE 0x80112d48: clip 0xc take-off, fly, then land -> resume idle */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 0x0c); e->sub_state_2 = 1; }  /* take-off clip 0xc @0x80112da0 */
            if (e->anim_frame == 0x10) e->roach_air = 1;                                /* airborne +0x1e0=1 @0x80112e78 (OPEN: flight-model swap @0x8011ec48 -> +0x78) */
            if (e->roach_air && e->crow_speed > 0) re15_dog_advance(e, e->crow_speed);  /* OPEN: flight nav/ballistic; forward crow_speed proxy */
            if (re15_roach_anim(e)) { e->roach_air = 0; re15_dog_sub(e, 0); }           /* land -> resume idle */
            break;
        default:
            re15_dog_sub(e, 0); re15_roach_anim(e);
            break;
        }
        if (e->dog_atk_cd) e->dog_atk_cd--;                                            /* +0x1dc-- ONCE per ACTIVE frame (brain tail @0x801111c4-d4) */
        break;
    }

    case 2:   /* HURT 0x80114790: flinch (clip 7, airborne clip 8) + Se(2) -> FLY-AWAY escape (sub 7/5/9) */
        if (e->sub_state_3 == 0) {                                                    /* phase 0 @0x8011484c */
            re15_roach_clip(e, e->roach_air ? 8 : 7); re15_audio_room_se(2);          /* flinch clip 7 / airborne 8 @0x80114a40 */
            e->sub_state_3 = 1;                                                        /* OPEN: blood FX 0x80019700 @0x80114960 */
        } else if (re15_roach_anim(e)) {                                              /* phase 2 recovery @0x801149ac */
            e->hit_react = 0;                                                          /* clears the WHOLE +0x93 (sb zero,147 @0x801149ac), not just bit0 */
            e->state = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
            uint8_t nsub = 7;                                                          /* default sub=7 leap @0x801149c8 */
            if (e->roach_beh2) nsub = 5;                                              /* sub=5 if +0x1e2 @0x801149fc-a0c */
            if (e->roach_esc)  nsub = 9;                                              /* sub=9 fly-away if +0x1e3 @0x80114a1c-2c (default) */
            e->sub_state_1 = nsub;
        }
        break;

    case 3:   /* DEATH 0x80114fb4: collapse clip 0xe (70f) + Se(7) + SE(1)@frame 0x3d -> CORPSE (state 7) */
        if (e->sub_state_3 == 0) {                                                    /* phase 0 @0x801150e8 */
            re15_roach_clip(e, 0x0e); re15_audio_room_se(7); e->hit_react |= 2;       /* clip 0xe @0x801150e8 (OPEN: blood 0x80019700 @0x80115184) */
            e->sub_state_3 = 1;
        } else {
            if (e->anim_frame == 0x3d) re15_audio_room_se(1);                         /* SE(1) at frame 61 @0x80115230-40 */
            if (re15_roach_anim(e)) { e->state = 7; e->sub_state_3 = 0; e->roach_fade = 0; }
        }
        break;

    case 7:   /* CORPSE 0x80115a6c: flags|0x2|0x40, 0x5a fade counter, then inert */
        if (e->sub_state_3 == 0) {                                                    /* phase 0 @0x80115a98 */
            e->roach_fade = 0x5a;                                                      /* +0x9e=90 fade counter @0x80115a98 */
            e->flags |= (uint8_t)(0x2 | 0x40);                                         /* entity flags |=0x2 @0x80115ac4, |=0x40 @0x80115ae0 (OPEN: colour dim +0xc4/+0xec) */
            e->sub_state_3 = 1;
        }
        if (e->roach_fade > 0) { e->roach_fade--; re15_roach_anim(e); }               /* fade + pool spread (spread OPEN); expiry -> inert */
        else e->sub_state_3 = 2;                                                       /* phase 2 inert — stop ticking @0x80115b58 */
        break;

    default:
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ G-BIRKIN BOSS (type 0x30 EM030 / 0x36 EM036) — STAGE3 ============ *
 * THOROUGH byte-true rebuild of the whole 0x80116230 family (STAGE3.BIN, overlay base 0x80100000;
 * audit wf_efd92a2c birkin). The prior port block was a near-stub with NO phase-2 mutation. This ports
 * the ROOT phase-driver, INIT grid-dispatch, the sub-1 HUB DECIDE cascade, every ACT sub, the HURT
 * super-armor+flinch machine, the DEATH-guard mutation-revive, and the DEATH morph-tail -> run-off.
 *
 * State word +0x4 = {state(+4), sub(+5), phase(+6), +7}. Root @0x80116230 dispatches +0x4 -> table
 * @0x8011ee84 [0=INIT@0x801166e0 1=BRAIN@0x80116d38 2=HURT@0x8011a060 3=DEATH@0x8011a3f0]. BRAIN runs
 * DECIDE[sub]@0x8011eeb8 then ACT[sub]@0x8011eef0 by +0x5. In the BRAIN, +0x6 (sub_state_2) is the ACT
 * PHASE byte; in HURT/DEATH, +0x7 (sub_state_3) is the flinch/death phase and +0x6 is the hit-dir bone
 * selector (re15_damage writes both). Ports of the PSX offsets: hit_stun=+0x1dc (super-armor),
 * birkin_flags=+0x1dd, birkin_hurt_cd=+0x1de, birkin_atk_cd=+0x1df, birkin_saved_*=+0x1d8, ai_timer=+0x9c,
 * birkin_pause=+0x9e, birkin_runoff_*=+0x1d4/+0x1d6.
 *
 * FORM MODEL (grid_id +0x9): bit 0x10 = FORM-2 flag. The hp<100 root MUTATION force (@0x8011667c) and the
 * DECIDE mutate (@0x8011719c) are BOTH gated !(grid&0x10). A boss SPAWNED form-1 (grid&0x10 clear, e.g.
 * ROOM3071 grid 0x21) mutates to form-2 at hp<100 (sub 6: grid|=0x10, HP=150). A boss SPAWNED form-2
 * (ROOM3070 grid 0x33/0x10 — bit set, VERIFIED in ROOM3070.RDT SCD Sce_em_set @0x33ce/@0x340c) never
 * re-mutates and instead runs the death morph-tail. (Corrects the finder's "mutation reachable in
 * ROOM3070" to "reachable when spawned form-1".)
 *
 * HONEST-OPEN (subsystems the port lacks wholesale; exact bytes cited, never faked):
 *  - Attack-box overlap 0x8001a5e0 (skeletal bone-box geometry) drives every claw/bite/throw/tackle
 *    connect; the port proxies it with re15_dog_arc (dist+cone) as at every other melee site. The window
 *    frames, damage, clamp, latch and SEs are byte-true; only the hit geometry is a proxy.
 *  - Root motion FUN_8011b5c4 (per-clip skeleton root delta -> +0x8c) + nav-zone steer 0x80039e7c: the
 *    port advances by the byte-true ADDITIVE speed constants (+0x8c += 0x14 etc.) without the clip root
 *    delta, so approach speed is not pixel-exact.
 *  - aca58 player-command FSM (grab cmd 5 / throw cmd 6 / knockdown cmd 2) is unported port-wide; the
 *    port uses s_player_grabbed (via the birkin_grab latch) for cmd 5/6 and hit_react|=1 for the knockdown.
 *  - rng per-bone writhe/breathe (0x8003a95c) + gore/blood FX (0x80019700, ESP 0x1c/0x1d/0x1e/0x1f via
 *    0x8004ef90) are skeleton/effect render-side, not modeled (audio SE calls ARE ported).
 *  - DEATH phase 3 WAITS for (grid&0xf)==2 — an EXTERNAL room-SCD morph trigger — then run-off sub-11
 *    hands the fight to a room-SCD-spawned next form. The AI FSM is faithful; the SCD grid-nibble=2 flip
 *    and the next-form spawn are NOT wired -> the boss plays the collapse then HOLDS at phase 3 (gated on
 *    the SAME grid&0xf==2 field). OPEN until the STAGE3 room-SCD morph handoff is ported.
 *    NB (audit wf_efd92a2c dormant #2, wrong-citation): the morph target is NOT "type 0x33". In the RDT
 *    Sce_em_set record [op,slot,type,grid], ROOM3070 @0x33ce = `44 00 30 33` and @0x340c = `44 00 30 10`
 *    are BOTH type 0x30 — 0x33/0x10 are the grid bytes, not a type. Type 0x33 is unregistered game-wide:
 *    EXE dispatch slot 0x33 @0x80072c78 = 0 and no overlay writes it (a jalr NULL). The only sibling type
 *    sharing this root is 0x36 — STAGE3 registration @0x8011cec0 stores root 0x80116230 to slot 0x30
 *    (sw @0x8011cf48 -> 0x80072c6c) AND slot 0x36 (sw @0x8011cf50 -> 0x80072c84), never 0x33 (raw-verified). */
static void re15_birkin_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_birkin_anim(re15_actor_t *e)
{
    re15_enemy_bank_t *bb = re15_enemy_find(e->type);     /* EM030 clip lengths from the loaded bank */
    int fc = 40;
    if (bb && bb->ok && e->motion < bb->anim.clip_count && bb->anim.clips[e->motion].frame_count > 0)
        fc = bb->anim.clips[e->motion].frame_count;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    if (e->anim_frac > 0) e->anim_frac--;
    return done;
}
static void re15_birkin_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* ============================ ROOT pre-dispatch (0x80116230) ============================ */
    if (e->birkin_hurt_cd > 0) {                       /* +0x1de countdown @0x801163c8-3d4 */
        if (--e->birkin_hurt_cd == 0)                  /* at 0 -> clear +0x93 bit0 @0x801163fc-400 */
            e->hit_react = (uint8_t)(e->hit_react & ~1u);
    }
    if (e->birkin_atk_cd > 0) e->birkin_atk_cd--;      /* +0x1df lunge/tackle cooldown @0x80116420-42c */
    /* dist +0x1d0: vs the RUN-OFF override point when +0x1dd&1 (@0x80116318-398), else vs the player (@0x8011627c) */
    int32_t dist;
    if (e->birkin_flags & 0x1) {
        int32_t dx = e->x - e->birkin_runoff_x, dz = e->z - e->birkin_runoff_z;
        dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
    } else {
        dist = re15_enemy_player_dist(e, pl);
    }
    e->dog_dist = (int16_t)dist;
    if (e->birkin_grab) s_player_grabbed = 1;          /* re-assert the global grab channel aca58==5 (run_all cleared it) */

    switch (e->state) {
    case 0:   /* INIT 0x801166e0: HP 300 UNCONDITIONAL, grid-nibble sub dispatch, work-byte seeds */
        e->hp = 300;                                   /* +0x9a = 0x12c UNCONDITIONAL @0x8011690c-910 (not `if hp<=0`) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;  /* clip 0 @0x8011683c, +0x95=0 @0x8011684c, +0x8f=0 @0x8011685c */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be = player @0x80116710/728 */
        e->ai_timer = 100;                             /* +0x9c = 0x64 @0x8011677c */
        e->hit_stun = 9;                               /* +0x1dc = 9 super-armor @0x80116800 (prior port mislabeled this dog_atk_cd) */
        e->birkin_flags = 0; e->birkin_hurt_cd = 0; e->birkin_atk_cd = 0;  /* +0x1dd=0 @0x801167f0, +0x1de=0 @0x80116810, +0x1df clear */
        e->birkin_saved_state = e->birkin_saved_sub = e->birkin_saved_ph2 = e->birkin_saved_ph3 = 0;  /* +0x1d8=0 @0x801167e0 */
        e->birkin_grab = 0; e->birkin_pause = 0;
        e->state = 1;                                  /* +0x4 = 1 -> BRAIN @0x801166f8 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        {
            uint8_t nib = (uint8_t)(e->grid_id & 0xf);
            if (nib == 3) { e->anim_frame = 0x10; e->sub_state_1 = 9; }  /* grid&0xf==3: +0x95=0x10 @0x80116880 + sub 9 @0x80116890 (emerge) */
            else if (nib == 1) { e->sub_state_1 = 10; }                  /* grid&0xf==1: sub 10 @0x801168b8 (+flags|=8 render, spawn entrance) */
            else e->sub_state_1 = 0;                                     /* else sub 0 (grid==4 = pose variant) -> DECIDE[0] promotes to 1 next tick */
        }
        break;

    case 1: {  /* BRAIN 0x80116d38: DECIDE[sub]@0x8011eeb8 then ACT[sub]@0x8011eef0 (both by +0x5). +0x6 = ACT phase byte. */
        /* ---- DECIDE[0] 0x80116ec4: promote sub 0 -> sub 1 ---- */
        if (e->sub_state_1 == 0) { e->sub_state_1 = 1; e->sub_state_2 = 0; }
        /* ---- DECIDE[1] 0x80116f6c HUB cascade (may re-target the sub; writes fall through, LAST match wins) ---- */
        if (e->sub_state_1 == 1) {
            int grabbed = s_player_grabbed;            /* aca58==5 proxy */
            uint8_t nsub = 0;                          /* 0 = keep sub 1 (walk) */
            /* sub-3 (0x301) @0x80116fe8: +0x1df==0 && dist<0xc80 && aca58!=5 && arc(0x338) */
            if (e->birkin_atk_cd == 0 && !grabbed && dist < 0xc80 && re15_dog_arc(e, pl, 0xc80, 0x338)) nsub = 3;
            /* sub-4 (0x401) @0x80117038: dist<0x9c4 && aca58!=5 && arc(0x1f4) — overrides sub-3 at closer range */
            if (!grabbed && dist < 0x9c4 && re15_dog_arc(e, pl, 0x9c4, 0x1f4)) nsub = 4;
            /* sub-7 (0x701) @0x8011706c: dist<0xed8 && arc(0x464) && !arc(0x10) && player+0x8c>=101 && +0x1df==0 && aca58!=5.
             * player+0x8c (0x800acae0) = the player's instantaneous speed; the tackle only targets a RUNNING player. */
            if (!grabbed && dist < 0xed8 && re15_dog_arc(e, pl, 0xed8, 0x464)
                && !re15_dog_arc(e, pl, 0xed8, 0x10) && pl->speed_h >= 101 && e->birkin_atk_cd == 0) nsub = 7;
            if (nsub) { e->sub_state_1 = nsub; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            else if (grabbed) {                        /* grab-tail @0x80117118 (aca58==5); fall-through, last match wins */
                uint8_t gsub = 0;
                if (dist < 0xbb8 && re15_dog_arc(e, pl, 0xbb8, 0x10)) gsub = 5;   /* @0x8011712c dist<3000 & dead-ahead -> THROW */
                if (dist < 0x7d0) gsub = 8;                                       /* @0x80117160 dist<2000 -> REPOSITION */
                if (!(e->grid_id & 0x10) && (e->birkin_flags & 0x4)) gsub = 6;    /* @0x8011719c !form2 && heave -> MUTATE */
                if (gsub) { e->sub_state_1 = gsub; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            }
        }
        /* ---- ACT[sub] 0x8011eef0 (dispatch on the possibly-updated sub; sub_state_2 = phase) ---- */
        switch (e->sub_state_1) {
        case 1: {  /* WALK 0x801171d4: measured two-phase approach; +0x8c += 0x14 (+10 form-2) */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 1); e->anim_frac = 0xf; e->sub_state_2 = 1; }  /* phase 0 @0x80117220 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);              /* steer 0x20 @0x80117268 (toward +0x1bc/be) */
            re15_dog_advance(e, 0x14 + ((e->grid_id & 0x10) ? 10 : 0)); /* +0x8c += 0x14 @0x801172ac (+10 form-2 @0x801172dc) */
            if (e->sub_state_2 == 2) {                                  /* phase 2 pause @0x80117330: hold, +0x9e -> phase 1 */
                if (e->birkin_pause == 0) e->sub_state_2 = 1;          /* @0x801173c4 */
                else e->birkin_pause--;                                /* +0x9e-- @0x801173b8 */
            } else if (!re15_dog_arc(e, pl, 30000, 0x400) && e->anim_frac == 0) {  /* arc(0x400)!=0 @0x801172f4 && +0x8f==0 @0x80117310 */
                e->sub_state_2 = 2; e->birkin_pause = 0x5a;            /* phase->2, +0x9e=0x5a @0x80117318/324 */
            }
            re15_birkin_anim(e);
            break;
        }
        case 3: {  /* CLAW/LUNGE 0x801174fc: clip 3 strike, window [0x24..0x2b] -> hp-10 + grab; recovery clip 0xa */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x801175b0: clip 3, +0x95=0x14 form-2, +0x8f=0xf */
                re15_birkin_clip(e, 3);
                if (e->grid_id & 0x10) e->anim_frame = 0x14;           /* form-2 anim seed @0x80117600 */
                e->anim_frac = 0xf; e->sub_state_2 = 1;
            }
            if (e->sub_state_2 == 1) {                 /* phase 1 @0x8011762c: strike */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);         /* steer 0x50 @0x80117624 */
                int done = re15_birkin_anim(e);                        /* anim_set @0x80117640 (advance before the window read) */
                /* continue-gate @0x80117668: (arc(0x400) && dist<0xfa1) || frame>=0x1e; else abort (phase 2) when frame<0x1e */
                if (!re15_dog_arc(e, pl, 0xfa1, 0x400) && e->anim_frame < 0x1e) {
                    e->sub_state_2 = 2;                                /* abort @0x801176b8 */
                } else {
                    /* damage window [0x24..0x2b] tested EVERY frame (attack-box 0x8001a5e0 -> re15_dog_arc proxy) @0x801176ec */
                    if (e->anim_frame >= 0x24 && e->anim_frame <= 0x2b && pl->hit_react == 0
                        && re15_dog_arc(e, pl, 2500, 0x400)) {
                        s_player_grabbed = 1; e->birkin_grab = 1;      /* aca58 = (facing<<8)|5 grab latch @0x801177d0 */
                        pl->hit_react |= 1; re15_audio_room_se(7);     /* Se(7) @0x801177dc */
                        pl->hp = (int16_t)(pl->hp - 10);               /* hp-=10 @0x801177f0 */
                        if (pl->hp < 0) pl->hp = 1;                    /* clamp-to-1 @0x80117810 */
                    }
                    if (e->anim_frame == 0x25) re15_audio_room_se(4);  /* Se(4)@frame 0x25 @0x80117878 */
                    if (done) e->sub_state_2 = 3;                      /* ground-probe contact proxy -> recovery @0x8011783c */
                }
            } else if (e->sub_state_2 == 2) {          /* phase 2 abort @0x80117888: -> HUB, +0x1df=0x1e */
                e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                e->birkin_atk_cd = 0x1e;                               /* +0x1df = 0x1e @0x801179a8 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            } else if (e->sub_state_2 == 3) {          /* phase 3 recovery init @0x801178c0: clip 0xa, Se(6), phase->4 */
                re15_birkin_clip(e, 0x0a); e->anim_frac = 3; e->sub_state_2 = 4;
                re15_audio_room_se(6);                                 /* Se(6) @0x801178ec/fc */
                re15_birkin_anim(e);
            } else {                                   /* phase 4/5 recovery play @0x80117904: -> HUB, +0x1df=0x3c */
                if (re15_birkin_anim(e)) {
                    e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                    e->birkin_atk_cd = 0x3c;                           /* +0x1df = 0x3c @0x801179a4 */
                }
            }
            break;
        }
        case 4: {  /* FAST BITE 0x801179d8: clip 4, window [0x23..0x2a] -> hp-10 + grab(<0x26)/knockdown(>=0x26) -> HUB */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x80117a94: clip 4, +0x95=0x14 form-2 */
                re15_birkin_clip(e, 4);
                if (e->grid_id & 0x10) e->anim_frame = 0x14;           /* @0x80117ae8 */
                e->anim_frac = 0xf; e->sub_state_2 = 1;
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);         /* steer 0x50 @0x80117afc */
            }
            if (e->sub_state_2 == 1) {
                int done = re15_birkin_anim(e);
                /* continue-gate @0x80117b10: (arc(0x320) && dist<0xdad) || frame>=0xf; else abort (phase 2) frame<0xf */
                if (!re15_dog_arc(e, pl, 0xdad, 0x320) && e->anim_frame < 0xf) {
                    e->sub_state_2 = 2;                                /* abort @0x80117b60 */
                } else {
                    if (e->anim_frame >= 0x23 && e->anim_frame <= 0x2a && pl->hit_react == 0
                        && re15_dog_arc(e, pl, 2500, 0x400)) {         /* box proxy @0x80117bec */
                        if (e->anim_frame < 0x26) { s_player_grabbed = 1; e->birkin_grab = 1; }  /* aca58=0x205 grab @0x80117c1c */
                        pl->hit_react |= 1;                            /* aca58=0x202 knockdown (frame>=0x26) @0x80117c38 -> hit_react proxy */
                        re15_audio_room_se(7);                         /* Se(7) @0x80117c64 */
                        pl->hp = (int16_t)(pl->hp - 10);               /* hp-=10 @0x80117c78 */
                        if (pl->hp < 0) pl->hp = 1;                    /* clamp @0x80117c94 */
                    }
                    if (e->anim_frame == 0x25) re15_audio_room_se(4);  /* Se(4)@0x25 @0x80117cb4 */
                    if (done) e->sub_state_2 = 2;                      /* clip end -> exit */
                }
            } else {                                   /* phase 2 exit @0x80117cc4: -> HUB (no cooldown) */
                e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            }
            break;
        }
        case 5: {  /* GRAB-THROW 0x80117d30: clip 0xb, Se(3) start, frame 0x2c -> throw(Se2)/miss(Se6), Se(4)@0x25 -> HUB */
            if (e->sub_state_2 == 0) {                 /* phase 0: clip 0xb, Se(3), phase->1 */
                re15_birkin_clip(e, 0x0b); e->anim_frac = 0xf; e->sub_state_2 = 1;
                re15_audio_room_se(3);                                 /* Se(3) start @0x80117e34 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            }
            if (e->sub_state_2 == 1) {
                int done = re15_birkin_anim(e);
                if (e->anim_frame == 0x2c) {                           /* frame 0x2c box overlap @0x80117e84 */
                    if (re15_dog_arc(e, pl, 2500, 0x400)) {            /* hit: throw (aca58=6) + Se(2) @0x80117ee4/f0c */
                        s_player_grabbed = 1; pl->hit_react |= 1; re15_audio_room_se(2);
                    } else {
                        re15_audio_room_se(6);                         /* miss: Se(6) @0x80117f08 */
                    }
                }
                if (e->anim_frame == 0x25) re15_audio_room_se(4);      /* Se(4)@0x25 @0x80117f30 */
                if (done) e->sub_state_2 = 2;
            } else {                                   /* phase 2 exit @0x80117f40: -> HUB, release the grab latch */
                e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                e->birkin_grab = 0;                                    /* throw resolved -> aca58 no longer 5 */
            }
            break;
        }
        case 6: {  /* MUTATION set-piece 0x80117f80 — THE PHASE-2 TRANSFORM: grid|=0x10 form-2, HP=150, ESP 0x1d */
            e->hit_stun = 9;                           /* +0x1dc = 9 re-armed EVERY tick @0x80117fb8 (unflinchable during mutation) */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x80118004: the transform */
                re15_birkin_clip(e, 0x14); e->anim_frac = 0xf;         /* clip 0x14 @0x80118014 */
                e->grid_id = (uint8_t)(e->grid_id | 0x10);            /* grid |= 0x10 FORM-2 FLAG @0x80118050-54 */
                e->ai_timer = 0xfff;                                  /* +0x9c = 0xfff @0x80118064 */
                e->birkin_pause = 1;                                  /* +0x9e = 1 @0x80118078 */
                /* ESP effect 0x1d (0x8004ef90 gore/morph FX) @0x80118074 — render-side, OPEN */
                re15_audio_room_se(1); re15_audio_room_se(8);         /* Se(1) @0x8011807c + Se(8) @0x80118084 */
                e->hp = 150;                                          /* HP RESET = 0x96 @0x80118094-98 */
                e->birkin_flags |= 0x8;                               /* +0x1dd |= 8 mutating (revive-guard) @0x801180b0 */
                e->sub_state_2 = 1;
            }
            if (e->sub_state_2 == 1) {                 /* phase 1 @0x801180b8: play clip 0x14 (SFX; bone-writhe render OPEN) */
                if (e->anim_frame == 0x3c || e->anim_frame == 0x60 || e->anim_frame == 0x78) re15_audio_room_se(8);  /* @0x801180e4 */
                if (e->anim_frame == 0x96) re15_audio_room_se(9);     /* @0x80118108 */
                if (re15_birkin_anim(e)) e->sub_state_2 = 2;          /* clip end -> phase 2 @0x80118880 */
            } else if (e->sub_state_2 == 2) {          /* phase 2 exit @0x8011888c: -> HUB (0x101), keep form-2, clear revive bit */
                e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4 = 0x101 */
                e->grid_id = (uint8_t)(e->grid_id | 0x10);            /* grid |= 0x10 @0x801188a4 */
                e->birkin_flags = (uint8_t)(e->birkin_flags & ~0x8u); /* +0x1dd &= ~8 @0x801188c0 */
            }
            break;
        }
        case 7: {  /* TACKLE 0x801188f8: clip 5, Se(4)@frames 1&0x12, windows [4..13]/[0x12..0x1b] -> hp-5 + knockdown, +0x1df=0xa */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x801189bc: clip 5 */
                re15_birkin_clip(e, 5); e->anim_frac = 0xf; e->sub_state_2 = 1;
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            }
            if (e->sub_state_2 == 1) {
                re15_dog_advance(e, 0x50);                             /* lunge forward (pos-advance @0x80118a10) */
                int done = re15_birkin_anim(e);
                if (e->anim_frame == 1 || e->anim_frame == 0x12) re15_audio_room_se(4);  /* Se(4) whoosh @0x80118a94 */
                if (((e->anim_frame >= 4 && e->anim_frame <= 13) || (e->anim_frame >= 0x12 && e->anim_frame <= 0x1b))
                    && pl->hit_react == 0 && re15_dog_arc(e, pl, 2500, 0x400)) {  /* box proxy @0x80118adc */
                    pl->hit_react |= 1;                                /* aca58=0x202 knockdown @0x80118b04 -> hit_react proxy */
                    re15_audio_room_se(7);                             /* Se(7) on hit @0x80118b08 */
                    pl->hp = (int16_t)(pl->hp - 5);                    /* hp-=5 @0x80118b1c */
                    if (pl->hp < 0) pl->hp = 1;                        /* clamp @0x80118b38 */
                }
                if (done) e->sub_state_2 = 2;
            } else {                                   /* phase 2 exit @0x80118a44: -> HUB, +0x1df=0xa */
                e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                e->birkin_atk_cd = 0xa;                                /* +0x1df = 0xa @0x80118a68 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            }
            break;
        }
        case 8: {  /* REPOSITION 0x80118b58: retarget player, clip 1, timer 0x78 -> GRAB (sub 5) */
            e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* retarget +0x1bc/be = player @0x80118b70/88 */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x80118bbc: clip 1, +0x9c=0x78 */
                re15_birkin_clip(e, 1); e->anim_frac = 0xf; e->ai_timer = 0x78; e->sub_state_2 = 1;
            }
            /* early exit -> sub 5 @0x80118c28: dist>=0x9c5 && arc(0x10) dead-ahead && timer<100 */
            if (dist >= 0x9c5 && re15_dog_arc(e, pl, 30000, 0x10) && e->ai_timer < 100) {  /* @0x80118c50 -> 0x501 */
                e->sub_state_1 = 5; e->sub_state_2 = 0; e->sub_state_3 = 0; break;
            }
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);            /* steer 0x20 @0x80118c68 */
            re15_dog_advance(e, 30);                                  /* +0x8c += 30 @0x80118cdc */
            re15_birkin_anim(e);
            if (e->ai_timer > 0) e->ai_timer--;
            else { e->sub_state_1 = 5; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* timeout -> GRAB (sub 5) @0x80118cb8 */
            break;
        }
        case 9: {  /* EMERGENCE 0x80119378: clip 0x10 (Se3) -> clip 0xd (Se10) -> immediate LUNGE (sub 3), grid&=0xfc */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x801193d4: clip 0x10, Se(3) */
                re15_birkin_clip(e, 0x10); e->anim_frac = 0; e->sub_state_2 = 1;
                re15_audio_room_se(3);                                 /* Se(3) @0x80119410 */
            }
            if (e->sub_state_2 == 1) {                 /* phase 1 @0x80119418: play clip 0x10 */
                if (re15_birkin_anim(e)) e->sub_state_2 = 2;
            } else if (e->sub_state_2 == 2) {          /* phase 2 @0x80119454: SECOND clip 0xd, Se(10) */
                re15_birkin_clip(e, 0x0d); e->anim_frac = 0xf; e->sub_state_2 = 3;
                re15_audio_room_se(10);                                /* Se(10) @0x80119484 */
                re15_birkin_anim(e);
            } else if (e->sub_state_2 == 3) {          /* phase 3 @0x8011948c: play clip 0xd */
                if (re15_birkin_anim(e)) e->sub_state_2 = 4;
            } else {                                   /* phase 4 @0x801194d0: -> LUNGE (sub 3), grid&=0xfc */
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sub=3 @0x801194d8, phase=0 @0x801194ec */
                e->grid_id = (uint8_t)(e->grid_id & 0xfc);            /* grid&=0xfc (0x33->0x30) @0x80119504 */
            }
            break;
        }
        case 10: {  /* CHARGE-COMBO 0x80119524: 14-phase grid-1 SPAWN ENTRANCE (effects 0x1f/0x1e, clips 1/0xf/0xd/0x13).
                     * The full spawn choreography is a cinematic gap (OPEN); the AI-relevant behavior is the charge-in
                     * that EXITS to sub 3 (LUNGE) + grid&=~1 (@0x801194d8/0x80119ae8). Faithful-compact: charge -> sub 3. */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 1); e->anim_frac = 0xf; e->sub_state_2 = 1; }
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            re15_dog_advance(e, 0x14);                                 /* charge approach (byte-true +0x8c base 0x14; root-motion OPEN) */
            if (re15_birkin_anim(e) || dist < 0x9c4) {                 /* clip end or bite range -> LUNGE */
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sub 3 @0x801194d8 */
                e->grid_id = (uint8_t)(e->grid_id & ~1u);             /* grid &= ~1 */
            }
            break;
        }
        case 11: {  /* RUN-OFF 0x80119b50: run to the fixed morph point (-22000,-12000), freeze there (next-form handoff) */
            if (e->sub_state_2 == 0) {                 /* phase 0 @0x80119b78: clip 0xe, +0x1dd|=1, set the run-off point */
                re15_birkin_clip(e, 0x0e); e->anim_frac = 7; e->sub_state_2 = 1;
                e->birkin_flags |= 0x1;                                /* +0x1dd |= 1 run-off active @0x80119bcc (root now steers/dist vs the override) */
                e->birkin_runoff_x = -22000; e->birkin_runoff_z = -12000;  /* +0x1d4/+0x1d6 = (0xAA10,0xD120) @0x80119bd8-ec */
            }
            if (e->sub_state_2 == 1) {                 /* phase 1 @0x80119bf0: run to the point, freeze at dist<500 */
                re15_enemy_steer_point(e, e->birkin_runoff_x, e->birkin_runoff_z, 0x60);  /* steer 0x60 @0x80119c04 */
                re15_dog_advance(e, 0xa0);                             /* +0x8c = root/2 + 0xa0 @0x80119c98 (root-motion OPEN) */
                re15_birkin_anim(e);
                if (dist < 0x1f4) e->sub_state_2 = 2;                  /* freeze when dist-to-point < 500 @0x80119c3c */
            }
            /* phase 2 = frozen at the morph point; the next-form room-SCD morph handoff is OPEN (the target is
             * NOT type 0x33 — that slot is unregistered @0x80072c78=0; see block header, audit dormant #2). */
            break;
        }
        default:
            e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
            re15_birkin_anim(e);
            break;
        }
        break;
    }

    case 2:   /* HURT 0x8011a060: super-armor soak (+0x1dc) resuming the SAVED action; at 0 -> flinch clip 7 -> HUB */
        if (e->sub_state_3 == 0) {                      /* +0x7==0: not yet flinching @0x8011a0a8 */
            e->birkin_hurt_cd = 9;                     /* +0x1de = 9 @0x8011a0c4 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);            /* steer 0x20 @0x8011a0fc */
            re15_audio_room_se(0);                     /* Se(0) @0x8011a104 */
            if (dist >= 0xfa1) e->hp = (int16_t)(e->hp + 5);          /* long-range hit refund: boss hp+=5 @0x8011a138 */
            /* blood FX 0x80019700 by hit-dir bone (+0x6) — render-side, OPEN */
            if (e->hit_stun == 0) {                    /* +0x1dc==0 @0x8011a1f4: FLINCH */
                re15_birkin_clip(e, 7); e->anim_frac = 0xf;          /* clip 7 @0x8011a2e0 */
                e->hit_stun = (int16_t)((re15_engine_rand8() & 3) + 3);  /* re-arm +0x1dc = (rng&3)+3 @0x8011a31c */
                re15_audio_room_se((re15_engine_rand8() & 1) ? 1 : 3);   /* Se(rng&1 ? 1 : 3) @0x8011a330 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);       /* @0x8011a348 */
                e->sub_state_3 = 1;                    /* enter flinch phase 1 @0x8011a2d0 */
            } else {                                   /* +0x1dc>0: SOAK -> resume the saved pre-hit action */
                e->hit_stun--;                         /* +0x1dc-- @0x8011a1f8 */
                e->state = e->birkin_saved_state;      /* +0x4 = *(+0x1d8) resume @0x8011a208 */
                e->sub_state_1 = e->birkin_saved_sub;
                e->sub_state_2 = e->birkin_saved_ph2;
                e->sub_state_3 = e->birkin_saved_ph3;
            }
        } else {                                       /* +0x7!=0: flinch clip 7 playing @0x8011a350 */
            if (re15_birkin_anim(e)) {                 /* clip end -> HUB (0x101) @0x8011a3a8 */
                e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
                e->hit_react |= 1;                     /* +0x93 |= 1 @0x8011a39c */
            }
        }
        break;

    case 3:   /* DEATH 0x8011a3f0: mutation-revive guard, else the morph-tail down-machine 0x8011a5d8 (+0x7 = phase) */
        /* revive guard @0x8011a43c: a lethal hit DURING the mutation (+0x1dd&8) does NOT kill -> HP=50, resume */
        if (e->sub_state_3 == 0 && (e->birkin_flags & 0x8)) {         /* +0x7==0 && +0x1dd&8 */
            e->birkin_hurt_cd = 9;                     /* +0x1de = 9 @0x8011a458 */
            e->hp = 50;                                /* HP = 0x32 revive @0x8011a534 */
            e->hit_react = (uint8_t)(e->hit_react & ~1u);            /* +0x93 &= ~1 @0x8011a550 */
            e->state = e->birkin_saved_state;          /* resume +0x4 = *(+0x1d8) @0x8011a568 */
            e->sub_state_1 = e->birkin_saved_sub;
            e->sub_state_2 = e->birkin_saved_ph2;
            e->sub_state_3 = e->birkin_saved_ph3;
            break;
        }
        /* real death down-machine 0x8011a5d8 — 7-phase morph tail on +0x7 (sub_state_3) */
        switch (e->sub_state_3) {
        case 0:   /* phase 0 @0x8011a618: clip 9 collapse, +0x93|=1, Se(1), phase->1 */
            re15_birkin_clip(e, 9); e->anim_frac = 7; e->hit_react |= 1;   /* clip 9 @0x8011a634, +0x93|=1 @0x8011a66c */
            re15_audio_room_se(1);                     /* Se(1) @0x8011a670 */
            e->sub_state_3 = 1;
            break;
        case 1:   /* phase 1 @0x8011a678: play clip 9, Se(10)@frame 0x50, -> phase 2 */
            if (e->anim_frame == 0x50) re15_audio_room_se(10);        /* Se(10) @0x8011a744 */
            if (re15_birkin_anim(e)) e->sub_state_3 = 2;
            break;
        case 2:   /* phase 2 @0x8011a690: ESP 0x1c gore, +0x9c=300, -> phase 3 (or 4 if grid&0xf already 2) */
            e->ai_timer = 300;                         /* +0x9c = 0x12c @0x8011a6a8 */
            /* ESP effect 0x1c (0x8004ef90 gore) @0x8011a6b8 — render-side, OPEN */
            e->sub_state_3 = ((e->grid_id & 0xf) == 2) ? 4 : 3;       /* grid&0xf==2 -> skip the wait @0x8011a6d8 */
            break;
        case 3:   /* phase 3 @0x8011a6c0: WAIT for the room-SCD morph trigger (grid&0xf)==2. OPEN: not wired -> HOLD */
            if ((e->grid_id & 0xf) == 2) e->sub_state_3 = 4;
            break;
        case 4:   /* phase 4 @0x8011a6e8: SECOND collapse clip 0xc, phase->5 */
            re15_birkin_clip(e, 0x0c); e->anim_frac = 0; e->sub_state_3 = 5;   /* clip 0xc @0x8011a704 */
            break;
        case 5:   /* phase 5 @0x8011a728: play clip 0xc, Se(10)@frame 0x3c, -> phase 6 */
            if (e->anim_frame == 0x3c) re15_audio_room_se(10);        /* Se(10)@0x3c @0x8011a744 */
            if (re15_birkin_anim(e)) e->sub_state_3 = 6;
            break;
        default:  /* phase 6 @0x8011a788: -> ACTIVE sub 11 RUN-OFF (state1 sub11 = 0xb01) @0x8011a790 */
            e->state = 1; e->sub_state_1 = 11; e->sub_state_2 = 0; e->sub_state_3 = 0;
            break;
        }
        break;

    case 7:   /* CORPSE fallback: settle, inert (the byte-true death instead HOLDS at DEATH phase 3 / RUN-OFF phase 2) */
        re15_birkin_anim(e);
        break;

    default:
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;
    }

    /* ============================ ROOT post-dispatch (0x801164d4) ============================ */
    if (!(e->grid_id & 0x10) && e->hp < 200) {         /* gate !(grid&0x10) @0x801164e8 && hp<200 @0x801164fc */
        e->birkin_flags ^= 0x4;                        /* +0x1dd ^= 4 heave toggle @0x80116510 (wounded-breathing; DECIDE mutate gate) */
        if (e->hp < 100) {                             /* slti hp,100 @0x8011667c */
            e->state = 1; e->sub_state_1 = 6;          /* FORCE state1-sub6 MUTATION: sw 0x601 @0x80116684-88 (form-1 only) */
            e->sub_state_2 = 0; e->sub_state_3 = 0;
        }
    }
    /* +0x1d8 = +0x4 snapshot (consumed by the HURT/revive resume) @0x80116698-a0 */
    e->birkin_saved_state = e->state; e->birkin_saved_sub = e->sub_state_1;
    e->birkin_saved_ph2 = e->sub_state_2; e->birkin_saved_ph3 = e->sub_state_3;
}

/* ============================ WRITHE-HAZARD (type 0x1a, EM01A) — STAGE1 ROOM1210/1211 =========== *
 * BYTE-TRUE REBUILD of the 0x8010c1ec family against raw STAGE1.BIN disasm (overlays @0x80100000,
 * audit wf_efd92a2c writher). The prior port ("wf_5c34ffe7") sold this as an UNKILLABLE / harmless /
 * zero-locomotion hazard — ALL THREE are false on hardware, and it invented a grid&0x80 emerge branch
 * + an idle clip-cycle. State table @0x8012093c (read as 8 words):
 *   [0]=INIT 0x8010c33c  [1]=ACTIVE 0x8010c488  [2]=HURT 0x8010d0f8  [3]=DEATH 0x8010d474
 *   [4]=0x8010d768  [5]=0  [6]=0  [7]=CORPSE 0x8010d770 (= jr ra, inert)
 * ACTIVE grid dispatch @0x8012095c[grid&0xf] -> dual A[sub]@0x80120968 + B[sub]@0x80120984 (0x8010c510).
 *
 * (#5, KILLABLE — verified here vs raw STAGE1.BIN) INIT installs a real 300-radius damage box
 * +0x78 = *(0x80120934) = @0x8012091c = {0,-1440,0,300,1440,300} @0x8010c3bc-3c4 -> targetable by the
 * weapon-fire scan (FUN_80011f50 tests every entity's +0x78, no whitelist). INIT writes NO HP (+0x9a)
 * anywhere in 0x8010c33c-0x8010c484 -> spawn HP stays 0; type 0x1a<0x20 -> any damaging shot drives
 * hp<0 -> DEATH (state 3) -> CORPSE (state 7). Death 0x8010d4c4: +0x7 phase machine — clip 3 (+0x94=3
 * @0x8010d520), +0x8f=7 @0x8010d540, +0x93|=1 @0x8010d55c, gore 0x80019700(0x2000) @0x8010d578, play
 * clip 3 via anim_set 0x8001f314 @0x8010d594, at completion +0x7->2 -> sw 7 into +0x4 @0x8010d5bc.
 *
 * (#1) Solid body-push obstacle: the ROOT arms +0x1b8|=0x12 @0x8010c2d8 and runs the push chain
 * 0x8002b498/0x8002aec4(vs player block 0x800aca54)/0x8002b544(vs enemies)/0x8003b0a4(wall-clamp) EVERY
 * unfrozen tick @0x8010c2e8-324. The 300-radius box the port installs makes re15_body_push_player
 * (game_step) push the player out and re15_enemy_body_push_tail (dispatch) separate other enemies.
 *
 * (#2/#3) REMOVED the invented grid&0x80 emerge (the cited clip 0x1f @0x80107d2c lives in the ZOMBIE-
 * GIRL root's code 0x80107cb0, unreachable from 0x8010c1ec — no +0x9 grid read in the writher INIT at
 * all) and the idle clip 0->1->2 cycling. With the real rooms' grid 0x00: A[0] @0x8010c608 leaves
 * sub 0, B[0] @0x8010c678 sets clip 0 ONCE @0x8010c6bc then only anim_set @0x8010c6f4 -> the unhit
 * writher loops clip 0 forever, no clip changes, no movement. (rng&0x1f)+30 @0x8010c3fc seeds +0x1d0
 * which only drives the post-hit sub2<->sub3 switch, never sub 0.
 *
 * (#4) INIT byte-true: +0x1bc/+0x1be=player @0x8010c368/380, flags|=0x40000000 @0x8010c390, +0x9c=0x14
 * @0x8010c3ac, +0x78=box @0x8010c3c4, +0x1b8=0 @0x8010c3d4, +0x1b9=1 @0x8010c3e4, +0x1d0=(rng&0x1f)+30
 * @0x8010c3fc, +0x1d2=(rng&3)+1 @0x8010c41c, part-block @0x8010c434-454 + shadow 0x8001af5c @0x8010c470.
 * +0x8f=7 is a CLIP-START write (sub-brain / death @0x8010d540), NOT an INIT write.
 *
 * OPEN (latent — UNREACHABLE with the shipped grid-0x00 + HP-0 spawns; both ROOM1210/1211 = 10
 * Sce_em_set(0x1a) records, grid byte 0x00): the 7-sub dual-brain writhe/submerge choreography —
 * subs 1/2 translate via 0x800245d8 with +0x8c = 0x320/0x14/0xc8 @0x8010c7b4/7f4/85c; sub 3 Y-bobs
 * +0x38=+0x9c±(int8)+0x9e @0x8010cc88; HURT flinch 0x8010d188 (clip 2 @0x8010d200, knockback
 * pos_advance(0x800) @0x8010d2f8) decrements the +0x1d2 hit-counter, at <0 -> submerge sub 4 (clip 3
 * @0x8010cea0); buried idle sub 5 dwell (rng&0xff)+30 @0x8010d044; re-emerge sub 6 -> sub 2 @0x8010cf60.
 * Every one of those needs SURVIVING a hit (a 0-damage weapon row vs a HP-0 enemy) or a grid&0x1f in
 * {1,2} spawn — neither occurs in the real rooms, so the reachable behavior is: clip-0 idle + body-push
 * obstacle + one-hit death. The submerge cycle is documented, not invented. */
static void re15_writher_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x8010c33c: state->1, spawn seeds, NO HP write, NO grid&0x80 emerge. */
        e->state = 1;                                              /* +0x4 = 1 @0x8010c350 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be = player @0x8010c368/380 */
        e->ai_timer = 0x14;                                        /* +0x9c = 0x14 @0x8010c3ac */
        e->motion = 0; e->anim_frame = 0;                         /* clip 0 idle (B[0] +0x94=0 @0x8010c6bc) */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;/* grid 0 -> A[0] keeps sub 0 @0x8010c608 */
        /* The 300-radius +0x78 box (@0x8010c3c4), flags|=0x40000000, +0x1b8/+0x1b9, the +0x1d0/+0x1d2
         * rng seeds, the +0x188 part-block and the shadow (0x8001af5c) are engine bookkeeping; the port
         * installs the box via re15_enemy_apply_hitbox(0x1a) at spawn and models the reachable idle +
         * death only (the submerge seeds feed the OPEN latent cycle documented above). NO +0x9a set. */
        break;

    case 1:   /* ACTIVE 0x8010c488 -> A[0]/B[0]: grid-0 rooted idle. X/Z (+0x34/+0x3c) NEVER advanced;
               * clip 0 looped by anim_set. No clip cycling, no emerge — those were invented (#2/#3). */
        e->motion = 0;                                            /* B[0] +0x94=0 @0x8010c6bc */
        e->anim_frame++;                                         /* clip-0 anim advances, looped @0x8010c6f4 */
        break;

    case 2:   /* HURT 0x8010d0f8 (LATENT: only reachable via a 0-damage weapon vs the HP-0 spawn). Flinch
               * clip 2 @0x8010d200; the +0x1d2 hit-counter -> submerge sub 4 @0x8010d144 is OPEN (above).
               * Resume the idle rather than invent the unmodeled submerge transition. */
        e->motion = 2; e->anim_frac = 7;                         /* +0x94=2 flinch, +0x8f=7 clip-start blend */
        if (re15_enemy_clip_done(e)) { e->state = 1; e->sub_state_1 = 0; }
        else e->anim_frame++;
        break;

    case 3:   /* DEATH 0x8010d474 -> 0x8010d4c4 (+0x7 phase machine): clip 3 topple + gore -> CORPSE 7. (#5) */
        if (e->sub_state_3 == 0) {                                /* +0x7==0 @0x8010d510: arm clip 3 + gore */
            e->sub_state_3 = 1;                                   /* +0x7 = 1 @0x8010d510 */
            e->motion = 3; e->anim_frame = 0;                     /* +0x94=3 @0x8010d520, +0x95=0 @0x8010d530 */
            e->anim_frac = 7;                                     /* +0x8f=7 @0x8010d540 */
            e->hit_react |= 1;                                    /* +0x93 |= 1 @0x8010d55c */
            re15_enemy_death_fx(e);                               /* gore 0x80019700(0x2000) @0x8010d578 */
        } else {                                                 /* +0x7==1 @0x8010d580: play clip 3 (anim_set) */
            if (re15_enemy_clip_done(e)) {                        /* clip complete -> +0x7->2 @0x8010d5b0 */
                e->state = 7; e->sub_state_1 = 0; e->sub_state_3 = 0;  /* sw 7 into +0x4 @0x8010d5bc */
            } else {
                e->anim_frame++;
            }
        }
        break;

    case 7:   /* CORPSE 0x8010d770 = jr ra: inert, no fade. */
    default:  /* latent submerge states (4/5/6, OPEN above): freeze — no invented transition. */
        break;
    }
}

/* ============================ ALLIGATOR boss (type 0x23, EM023) — STAGE2 sewer ================ *
 * BYTE-TRUE REBUILD of the WHOLE 0x8010c448 family (audit wf_efd92a2c alligator, 12 findings, 5 HIGH).
 * The port was a near-stub that INVENTED a "grab -> 100-frame jaws-hold -> unconditional swallow-kill"
 * — none of which exists on hardware. Re-ported against raw STAGE2.BIN disasm (overlays @0x80100000):
 *   state table @0x80118bc8  ([1]=ACTIVE 0x8010c860, [2]=HURT 0x8010e570, [7]=CORPSE 0x8010eca4)
 *   DECIDE table A@0x80118be8 (A[0]=0x8010cb04, A[2]=0x8010cd70, A[4]=0x8010d3a4, A[6]=0x8010da0c)
 *   ACT    table B@0x80118c28 (B[3]=0x8010cfbc, B[4]=0x8010d684, B[5]=0x8010d868, B[6]=0x8010db50,
 *                              B[7]=0x8010dc90, B[9]=0x8010df34)
 *   connect-frame byte tables  @0x80118c68 = {19,20,21} (B[3]).
 * The ACTIVE brain is a decide(A[sub]) + act(B[sub]) dual-dispatch on +0x5 (same shape as Birkin/
 * Tyrant), and the ACTIVE tail decrements three cooldowns every frame (audit #7).
 *
 * HEADLINE (audit #0/#2): the bite does NOT pin-and-swallow. On connect the handler writes the
 * engine-wide KNOCKDOWN player command DAT_800aca58=2 / aca59=facing+2 (byte-identical to the zombie
 * @0x8010f30c and spider @0x8011638c knockdown) — the player is knocked down and RECOVERS. The player
 * hp access at the bite sites is a lhu/sh SELF-STORE (zero damage); the aca58=3 "eaten" cmd @0x8010d2ac
 * is bgez-gated on player.hp<0 (fires same-frame ONLY if already dead). There is NO hold loop and NO
 * player.hp write anywhere in 0x8010c448..0x8010ee38 (full sweep). So the port models the bite as the
 * shared hit_react|=1 knockdown latch (the aca58 player-command FSM is unported port-wide, L2729/L3475).
 *
 * Field map to the PSX entity offsets (reusing the matching dedicated actor fields):
 *   +0x1dc lunge/re-attack cooldown -> hit_stun    +0x1e0 mode (0=land,1=water) -> dog_atk_cd
 *   +0x1e2 roar cooldown            -> dog_yawrate  +0x1e3 aux cooldown          -> dog_pounce_cd
 *   +0x9c  aim/misc timer           -> ai_timer     +0x9e  dwell/corpse timer    -> grab_kill_ctr
 *   +0x1d0 LOS raycast bit          -> dog_flags b0 +0x1d4 cached dist           -> dog_dist
 *   +0x1ba floor Y                  -> dog_floor_y  +0x9f  hit-direction latch   -> dog_aux9f
 *   +0x6   act phase                -> sub_state_2  +0x7   phase/mode-2 rise      -> sub_state_3
 *
 * OPEN (subsystem gaps documented inline, NOT faked):
 *   (a) +0x1d0&1 is the room-collision LOS raycast — not reproducible in the collision-less AI harness;
 *       the water-idle hub / roar gate on an in-arc proxy for it.
 *   (b) the JAW-BONE box hit (0x8001bff8 on skel+2644 box[0]=700/a2=800 for B[3]; skel+1612 for B[9])
 *       has no per-bone spheres in the port (the alligator MODEL is a known data gap — absent from
 *       CDEMD0.EMS) -> connect uses the {19,20,21}/{7..15} frame window + range/arc proxy; the 700/800
 *       box constants + jaw bone are cited but proxied.
 *   (c) clip root-motion (0x800245d8 reads +0x8c from the clip) + exact clip lengths are clip-data,
 *       also part of the model gap -> movement uses re15_dog_advance proxy speeds and a frame-window
 *       clip-end proxy (flagged); the submerge (sub 5) / water-rise (B[4] mode-2 Y-=50 clamp -1200)
 *       depend on the room water level (+0x1e0 water zone) which the harness has no room for.
 *   (d) corpse tint/fade (+0xc4/+0xec color, +0xbc/+0xbe +8/frame) is render-side; the flag/timer half
 *       is ported. */
static void re15_alligator_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0: {  /* INIT 0x8010c56c: HP 300 unconditional; +0x8f=0; default=water sub6, grid-odd=land sub0.
                * (audit #9: default Y=-1200 / +0x1ba mirror / +0x1e0=1; grid-odd word 0x01000001 =
                *  state1 sub0 sub3=1 / clip 0 / +0x1e0=0 / Y=-1800*floor; no INIT steer store.) */
        if (e->hp <= 0) e->hp = 300;                 /* +0x9a row @0x801175dc (16x300) @0x8010c6d4 */
        e->anim_frame = 0; e->anim_frac = 0;         /* +0x8f = 0 @0x8010c5b0 (was invented anim_frac=7, #9) */
        e->hit_react = 0; e->ai_timer = 0;
        e->hit_stun = 0; e->dog_yawrate = 0; e->dog_pounce_cd = 0; e->dog_flags = 0;  /* +0x1dc/+0x1e2/+0x1e3/+0x1d0 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        if (e->grid_id & 1) {                        /* grid-odd (+0x9&1 @0x8010c750-758) = LAND melee */
            e->motion = 0;                           /* clip 0 @0x8010c77c */
            e->dog_atk_cd = 0;                       /* +0x1e0 = 0 (land) @0x8010c78c */
            e->y = -1800 * (int32_t)e->floor;        /* Y = -1800*(+0x82) @0x8010c7a4-7bc */
            e->dog_floor_y = (int16_t)e->y;
            e->state = 1; e->sub_state_1 = 0; e->sub_state_3 = 1;   /* word 0x01000001 @0x8010c764-76c */
        } else {                                     /* default = WATER ranged/approach */
            e->motion = 4;                           /* clip 4 @0x8010c590 */
            e->dog_atk_cd = 1;                       /* +0x1e0 = 1 (water) @0x8010c61c-620 */
            e->y = -1200;                            /* Y = -1200 @0x8010c65c-660 */
            e->dog_floor_y = -1200;                  /* +0x1ba mirror @0x8010c670-678 */
            e->state = 1; e->sub_state_1 = 6;        /* word 0x601 = state1 sub6 @0x8010c578-580 */
        }
        break; }

    case 1: {  /* ACTIVE 0x8010c860: decide A[sub] (may change +0x5) then act B[sub]; tail decrements
                * the three cooldowns +0x1dc/+0x1e2/+0x1e3 every frame @0x8010c9d4-0x8010ca40 (audit #7). */
        int32_t dist = re15_enemy_player_dist(e, pl);
        e->dog_dist = (int16_t)dist;

        /* LOS proxy (OPEN (a)): +0x1d0&1 is the room-collision has-LOS raycast (FUN_8001b84c), which
         * the collision-less AI harness cannot run, so the bit stays 0 (LOS "unconfirmed"). With it 0
         * the water-idle hub A[6] always elects the swim-chase (sub 4), and roar/submerge (which REQUIRE
         * the bit) are gated off — the byte-true fallback when LOS is not established. */
        e->dog_flags = (uint16_t)(e->dog_flags & ~1u);

        /* ---------- DECIDE A[sub] (may retarget +0x5) ---------- */
        switch (e->sub_state_1) {
        case 0:   /* A[0] 0x8010cb04: promote out of idle. dist<0x1388(5000)+facing -> sub 1; dist<0xfa0(4000) -> sub 2. */
            if ((uint32_t)dist < 0xfa0u)                                    e->sub_state_1 = 2;   /* @0x8010c... land-chase */
            else if ((uint32_t)dist < 0x1388u && re15_dog_arc(e, pl, 0x1388, 0x400)) e->sub_state_1 = 1;
            break;
        case 1: case 2:   /* A[2] 0x8010cd70 land-chase: commit the lunge; frontal-hit -> sub 10 turn. */
            if (pl->hit_react == 0 && re15_dog_arc(e, pl, 0x1770, 0x180) && e->hit_stun == 0) {  /* 0x8001a804(0x1770=6000,0x180) + +0x1dc==0 @0x8010ce34-64 (audit #4) */
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0; e->motion = 3;
            }
            break;
        case 4:   /* A[4] 0x8010d3a4 swim decide: commit lunge (same 6000/0x180/+0x1dc==0); sub-5 submerge
                   * gate = dist<0xbb8(3000) && !(+0x1d0&1) @0x8010d4b4-d4 (audit #10, was mislabeled). */
            if (pl->hit_react == 0 && re15_dog_arc(e, pl, 0x1770, 0x180) && e->hit_stun == 0) {  /* @0x8010d3e0-410 (audit #4) */
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0; e->motion = 3;
            } else if ((uint32_t)e->dog_dist < 0xbb8u && !(e->dog_flags & 1)) {
                e->sub_state_1 = 5; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sub 5 submerge @0x8010d4d4 */
            }
            break;
        case 6:   /* A[6] 0x8010da0c water-idle decision HUB (audit #3): -> sub 5 grab-follow (aca58==0x701,
                   * unported cmd FSM); else sub 4 swim-chase when NOT(dist 4000..6000 && LOS); else roar
                   * sub 7 when dist<6000 && +0x1e2==0. NEVER walks/lunges directly. */
            if (pl->hit_react == 0 && !((uint32_t)(dist - 4000) < 0x7d1u && (e->dog_flags & 1))) {
                e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* -> swim-chase @0x8010dab0 */
            } else if ((uint32_t)dist < 0x1770u && e->dog_yawrate == 0) {
                e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* -> roar @0x8010da80/aa0 (+0x1e2==0) */
            }
            break;
        default: break;
        }

        /* ---------- ACT B[sub] ---------- */
        switch (e->sub_state_1) {
        case 1: case 2:   /* land-chase locomotion (clip root-motion = model gap -> proxy advance). */
            if (e->motion != 4) { e->motion = 4; e->anim_frame = 0; }
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            re15_dog_advance(e, 48);
            e->anim_frame++;
            break;

        case 4: {  /* B[4] 0x8010d684 swim-chase: yaw-slew rng rate (rng&0x1f)+6 @0x8010d740-50; root-motion
                    * advance @0x8010d848; mode-2 water-rise Y-=50 clamp -1200 -> mode 1 @0x8010d7e4-834
                    * (OPEN (c): rise needs the room water level). Movement speed = clip root-motion proxy. */
            int slew = (re15_engine_rand8() & 0x1f) + 6;
            re15_enemy_steer_point(e, pl->x, pl->z, slew);
            re15_dog_advance(e, 48);
            e->anim_frame++;
            break; }

        case 3:   /* B[3] 0x8010cfbc LUNGE-GRAB: connect only on anim frames {19,20,21} (@0x80118c68) at the
                   * jaw bone (box[0]=700/a2=800, OPEN (b)); on connect KNOCKDOWN (aca58=2 -> hit_react|=1),
                   * +0x1dc=0x64; cmd3 "eaten" only if pl->hp already<0. Clip-end -> +0x1dc=0x2d if 0,
                   * sub=(+0x1e0==0)?2:4 (audit #1/#8). */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            if (dist > re15_body_contact_reach(e)) re15_dog_advance(e, 64); /* lunge close, halt at jaw contact */
            if ((e->anim_frame == 19 || e->anim_frame == 20 || e->anim_frame == 21)   /* {19,20,21} @0x80118c68 */
                && pl->hit_react == 0 && dist <= re15_body_contact_reach(e)
                && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x400)) {  /* jaw box[0]=700/a2=800 sits on bone skel+2644 (+2644 FORWARD of center @0x8010d1dc) -> reaches from center by ~the body radius, proxied as body_contact_reach (audit #1 + wf_555f18eb Part B); OPEN (b) */
                pl->hit_react |= 1;                                         /* KNOCKDOWN aca58=2/aca59=facing+2 @0x8010d27c (cmd FSM OPEN) */
                e->hit_stun = 0x64;                                         /* +0x1dc = 100 @0x8010d26c-70 (audit #7) */
                if (pl->hp < 0) { /* eaten cmd3 same-frame only if already dead @0x8010d2a0-ac */ }
                e->sub_state_2 = 3;                                         /* -> recovery phase */
            }
            e->anim_frame++;
            if (e->anim_frame > 21 || e->sub_state_2 == 3) {               /* clip-end proxy (OPEN (c): anim_set complete) */
                if (e->hit_stun == 0) e->hit_stun = 0x2d;                   /* +0x1dc=0x2d if 0 @0x8010d33c-340 */
                e->sub_state_1 = (e->dog_atk_cd == 0) ? 2 : 4;             /* sub=(+0x1e0==0)?2:4 @0x8010d350-364 */
                e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0;
            }
            break;

        case 9:   /* B[9] 0x8010df34 GRAB (second site): window frames 7..15 (@0x8010e0cc-d0), jaw bone
                   * skel+1612 (no 700 override); on connect KNOCKDOWN + +0x1dc=0x78; clip-end -> sub=4,
                   * yaw+=0x800 (180 turn), +0x1dc=0x3c if 0 (audit #1/#8). */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            if (dist > re15_body_contact_reach(e)) re15_dog_advance(e, 64); /* halt at jaw contact */
            if (e->anim_frame >= 7 && e->anim_frame <= 15                   /* window 7..15 @0x8010e0cc-d0 */
                && pl->hit_react == 0 && dist <= re15_body_contact_reach(e)
                && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x400)) {  /* jaw bone skel+1612 (no 700 override), reach proxied as body_contact_reach; OPEN (b) */
                pl->hit_react |= 1;                                         /* KNOCKDOWN @0x8010e120 (cmd FSM OPEN) */
                e->hit_stun = 0x78;                                         /* +0x1dc = 120 @0x8010e110-14 (audit #7) */
                if (pl->hp < 0) { /* eaten cmd3 @0x8010e154 gated on hp<0 */ }
                e->sub_state_2 = 3;
            }
            e->anim_frame++;
            if (e->anim_frame > 15 || e->sub_state_2 == 3) {               /* clip-end proxy */
                e->rot_y = (int16_t)(((int)e->rot_y + 0x800) & 0xfff);     /* yaw += 2048 (180) @0x8010e014-020 */
                if (e->hit_stun == 0) e->hit_stun = 0x3c;                   /* +0x1dc=0x3c if 0 @0x8010e038-040 */
                e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0;
            }
            break;

        case 5:   /* B[5] 0x8010d868 SUBMERGE: go under, resurface -> hub (OPEN (c): submerge/water-rise
                   * choreography needs the room water level; no numeric body constant fabricated). */
            e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;
            break;

        case 6:   /* B[6] 0x8010db50 idle-clip cycler: STATIONARY, dwell (rng&0x7f)+59 @0x8010db9c-b0.
                   * (Reached only if A[6] left +0x5==6 this tick.) */
            if (e->grab_kill_ctr <= 0)
                e->grab_kill_ctr = (int16_t)((re15_engine_rand8() & 0x7f) + 59);  /* +0x9e dwell @0x8010db9c-b0 */
            else e->grab_kill_ctr--;
            e->motion = 6;                                                 /* clip 6 @0x8010dbf0 */
            e->anim_frame++;
            break;

        case 7:   /* B[7] 0x8010dc90 ROAR: clip 0x14 once -> sub 6 + roar cooldown +0x1e2=0x78
                   * @0x8010dcec/@0x8010dd7c (audit #5). Roar clip length = model gap -> frame proxy. */
            e->motion = 0x14;
            e->anim_frame++;
            if (e->anim_frame >= 30) {                                     /* clip-end proxy (OPEN (c)) */
                e->dog_yawrate = 0x78;                                     /* +0x1e2 = 120 @0x8010dd4c/dd7c */
                e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0;
            }
            break;

        case 8:   /* sub 8 = water-hurt turn-bite (entered from HURT water path @0x8010e988, audit #6):
                   * turn toward the aimed direction (+0x9c) then commit the lunge. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            re15_dog_advance(e, 48);
            e->anim_frame++;
            if (pl->hit_react == 0 && re15_dog_arc(e, pl, 0x1770, 0x180) && e->hit_stun == 0) {
                e->sub_state_1 = 9; e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0;  /* water grab site */
            }
            break;

        case 10:  /* sub 10 frontal-turn reaction (A[2]/A[4] @0x8010cdb0/d54c): latch hit-dir +0x9f, then
                   * sub3=1 -> immediate lunge (audit #5). */
            e->dog_aux9f = (int8_t)e->hit_react;                           /* +0x9f = hit-dir @0x8010d56c */
            e->sub_state_3 = 1;                                            /* sub3=1 @0x8010d5a0 */
            e->sub_state_1 = 3; e->sub_state_2 = 0; e->anim_frame = 0; e->motion = 3;
            break;

        case 0: default:  /* still idle (A[0] did not promote): hold */
            break;
        }

        /* ---------- ACTIVE tail: decrement the three cooldowns every frame (audit #7 @0x8010c9d4-0x8010ca40) ---- */
        if (e->hit_stun     > 0) e->hit_stun--;       /* +0x1dc lunge/re-attack cooldown */
        if (e->dog_yawrate  > 0) e->dog_yawrate--;    /* +0x1e2 roar cooldown */
        if (e->dog_pounce_cd > 0) e->dog_pounce_cd--; /* +0x1e3 aux cooldown */
        break; }

    case 2:   /* HURT 0x8010e570 (audit #6): +0x1e0!=0 (water) -> aimed turn-bite sub 8; else land flinch
               * clip 0xa -> sub 2. NEVER resumes sub 6. */
        if (e->dog_atk_cd != 0) {                     /* +0x1e0!=0 water @0x8010e580 -> 0x8010e91c */
            e->ai_timer = (int16_t)(((int)pl->rot_y - (int)e->rot_y) & 0xfff);  /* +0x9c aim = (DAT_800acabe - yaw)&0xfff @0x8010e954-968 */
            e->state = 1; e->sub_state_1 = 8;         /* state1 sub8 @0x8010e978-988 */
        } else {                                      /* land flinch @0x8010e5d8: clip 0xa -> sub 2 */
            e->hit_react = (uint8_t)(e->hit_react | 2);   /* +0x93 |= 2 @0x8010e630 */
            e->motion = 0xa;                          /* clip 0xa @0x8010e64c-650 */
            e->state = 1; e->sub_state_1 = 2;         /* phase2 -> state1 sub2 @0x8010e6f0/714 */
        }
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH (take_damage +0x4=3): -> corpse. Exact death-topple clip = faithful-line. */
        e->state = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;

    case 7:   /* CORPSE 0x8010eca4 (audit #11): phase0 seeds +0x9e=0x5a, flags|=2, flags|=0x40; phase1
               * fades (color/scale = render-side OPEN (d)), 90-frame countdown -> phase2 inert. */
        if (e->sub_state_2 == 0) {
            e->grab_kill_ctr = 0x5a;                  /* +0x9e = 90 @0x8010ecd0-d4 */
            e->flags = (uint8_t)(e->flags | 2);       /* word0 |= 2 @0x8010ecfc-d00 */
            e->flags = (uint8_t)(e->flags | 0x40);    /* word0 |= 0x40 @0x8010ed18-1c */
            e->sub_state_2 = 1;                        /* phase +0x7=1 */
        } else if (e->sub_state_2 == 1) {
            /* color +0xc4/+0xec fade + scale +0xbc/+0xbe +=8/frame = render-side (OPEN (d)) */
            if (e->grab_kill_ctr > 0) e->grab_kill_ctr--;   /* 90-frame countdown @0x8010ed74-90 */
            else e->sub_state_2 = 2;                    /* -> inert */
        }
        e->anim_frame++;
        break;

    default:
        e->state = 1; e->sub_state_1 = 6; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ AMBIENT FX-EMITTER (type 0x24, EM024) — STAGE2 =================== *
 * Byte-true from workflow wf_5c34ffe7 (root 0x8010ee9c). EM024 is NOT a combat enemy — it is a
 * scripted particle/swarm EMITTER (single-bone flat green billboard) used as room ambience. It has
 * NO HP, DISABLES its own collision at INIT (*+0x188 &= ~1 @0x8010efe4), and does ZERO player damage
 * (no player.hp write, no grab, no damage-entry, no hitbox anywhere in 0x8010ee9c..0x80110a00 —
 * full-range raw scan finds zero DAT_800aca58 / player-HP refs). Its ACTIVE loop (0x8010f020) caches
 * the SquareRoot0 player dist to +0x1d4 (sh @0x8010f078), runs a per-frame +0x5 sub-dispatch BEFORE
 * the drift (jalr @0x8010f0a8), then does an accelerating leftward X-DRIFT that wraps.
 *
 * PORT SCOPE (audit wf_efd92a2c dormant #1, missing-mechanism medium — BUT byte-true for the dominant
 * spawn): the +0x5 dispatch table @0x80118d58 was raw-verified — [0]=0x8010f130, [1]=0x8010f3fc,
 * [5]=[6]=[7]=0x8011084c. Leaf 0x8011084c is `jr ra; nop` = a NO-OP. ROOM20B0/20B1 (the ONLY EM024
 * rooms, all-RDT Sce_em_set(0x44) scan) spawn overwhelmingly grid 0x05 (~32 of ~35 per loaded room),
 * so ~91% of emitters dispatch the NOP leaf and run drift-only — for THEM the port's tick below is
 * byte-true. INIT copies grid once to +0x5 (sb @0x8010f00c) and nothing else writes +0x5, so the
 * finder's cited "grid-5 handler 0x80110338" is actually table index [4] (grid 4), which never spawns
 * and is dead code. HONEST-OPEN (subsystem gap, NOT faked): only the 1x grid-0 + 2x grid-1 ambient
 * emitters reach real handlers (grid-0 0x8010f130 -> +0x6 table @0x80118d88, grid-1 0x8010f3fc ->
 * @0x80118d90) that fire timed particle bursts via helpers 0x801108cc/0x80110988 -> 0x80019700 (the
 * EXE ESP/particle FX-spawn family) on a +0x6 tick budget. That is render-side ambience only (no
 * player interaction) and belongs to the un-ported ESP/particle subsystem -> OPEN.
 *
 * INIT also flips a GLOBAL: DAT_800acc0c := 1 (ori v1,0x1 @0x8010ef28; sb v1,-13300(at) @0x8010ef34).
 * Consumer EXE FUN_80024c30 (@0x80024ce0 lbu) skips a player secondary-motion pose add (+0x5f4) while
 * bit 0 is set. That consumer chain (skeleton_common.c:311-313 spring secondary-motion) is itself
 * un-ported, so the write has zero observable effect in the port today -> documented OPEN, not faked
 * as a dead global (audit wf_efd92a2c dormant #3, missing-mechanism low). */
static void re15_fx_emitter_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    (void)pl;
    switch (e->state) {
    case 0:   /* INIT 0x8010ef1c: no HP, disable own collision, drift timer 120 -> ACTIVE. */
        /* NOTE: original also sets DAT_800acc0c:=1 @0x8010ef34 (player secondary-motion pose switch);
         * its consumer chain is un-ported so the write is OPEN, not modeled (see block header, dormant #3). */
        e->hit_react = 1;                        /* +0x93 = 1 @0x8010ef58 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;   /* clip 0 via FUN_8011089c @0x801108a8 */
        e->ai_timer = 0x78;                      /* +0x8c drift timer = 120 @0x8010ef6c */
        e->sub_state_1 = e->grid_id;             /* +0x5 = grid @0x8010f00c */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->state = 1;                            /* +0x4 = 1 @0x8010eff4 */
        break;
    case 1:   /* ACTIVE 0x8010f020: accelerating leftward X-drift. No damage. */
        /* Original runs the +0x5 sub-dispatch (jalr @0x8010f0a8) BEFORE this drift: for grid-5 (~91% of
         * spawns) it is the `jr ra` NOP @0x8011084c, so drift-only is byte-true here. The grid-0/grid-1
         * particle-burst sequencer (0x80019700 ESP FX) is render-side ambience only -> OPEN (dormant #1).
         * The +0x1d4 SquareRoot0 dist cache (sh @0x8010f078) feeds only that sequencer -> also OPEN. */
        e->ai_timer++;                           /* timer++ @0x8010f0c8 */
        e->x -= e->ai_timer;                     /* X -= timer @0x8010f0e8 */
        if (e->x < -25000) { e->x = 20000; e->ai_timer = 0xe1; }   /* wrap @0x8010f100-11c (0x4e20/0xe1) */
        e->anim_frame++;                         /* clip frame advance (billboard); no burst modeled (OPEN) */
        break;
    default:
        e->state = 1;
        break;
    }
}

/* ============================ TYRANT boss (type 0x2b, EM02B) — STAGE4/5 ======================= *
 * BYTE-TRUE REBUILD of the WHOLE 0x80111a64 family (audit wf_efd92a2c tyrant, 16 findings). The port
 * was a near-stub: ATTACK1 fired from the wrong sub on invented ranges, no charge/stagger pipeline, no
 * facing-based grab select, grab was a harmless timed pin (no collapse/mash/finish), no attack cooldown,
 * no real hurt-react, single-clip death, wrong emerge clip. Re-ported against raw STAGE4.BIN disasm:
 *   state table @0x8011a0b4  ([1]=ACTIVE 0x80111c98, [2]=HURT 0x80114770, [3]=DEATH 0x80114c68,
 *                             [4/5/6]=EMERGE 0x80114fe4, [7]=CORPSE 0x80115af0 — all table-dumped & confirmed)
 *   decide table A@0x8011a0d4 (A[0]=0x80111e94 idle, A[1]=0x80112024 walk, A[3]=0x801123e8 charge/attack1,
 *                             A[13]=0x80113c74 attack2 — table-dumped & confirmed)
 *   act    table B@0x8011a114 (B[0..15]; B[1]=0x80112124 walk, B[3]=0x801125dc fast, B[4]=0x8011275c
 *                             ATTACK1, B[5]=0x80112a64 GRAB1, B[7]=0x801145c8 charge, B[8]=0x80112e20
 *                             collapse, B[9]=0x80112f58 rear-grab, B[14]=0x80113e48 ATTACK2, B[15]=
 *                             0x8011411c GRAB2 — table-dumped & confirmed; port's old B[4] 0x80112840 /
 *                             B[14] 0x80113f00 cites were mid-body labels, fixed — audit #13).
 * The ACTIVE brain is a decide(A[sub]) + act(B[sub]) dual-dispatch on +0x5 (same shape as Birkin).
 * OPEN (subsystem gaps documented inline, NOT faked): (a) the roar/attack DECIDE gate +0x1d0&1 is the
 * room-collision LOS raycast FUN_8001b84c @0x8001bc08 — not reproducible in the collision-less AI
 * harness, so roar is gated on the range+arc proxy; (b) ATTACK2's per-limb sphere hit (0x8001bff8 on
 * skel+580/+2472 r=0x320) has no per-bone spheres in the port -> close-arc proxy; (c) the aca58/aca59
 * player-command victim FSM is unported port-wide (L2729/L3475) -> grab drives the player via the shared
 * s_player_grabbed + re15_player_victim_devour, as Birkin/Dog do; (d) corpse tint/sink is render-side. */
static const uint16_t s_tyrant_hp[16] =    /* HP pool @0x80118b00 (index = rng & 0xf) */
    { 86, 89, 103, 119, 91, 107, 121, 93, 109, 124, 117, 97, 113, 126, 99, 101 };

static void re15_tyrant_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Field map to the PSX entity offsets (dedicated actor fields at the matching offset):
     *   +0x1de attack cooldown  -> birkin_hurt_cd   +0x1dc hp-snapshot / charge-arm -> hit_stun
     *   +0x9c  timer/budget      -> ai_timer          +0x9e  hold/recovery countdown  -> grab_kill_ctr
     *   +0x6   act phase byte    -> sub_state_2        +0x7   death/stagger phase       -> sub_state_3 */

    switch (e->state) {
    case 0:   /* INIT 0x80111a64: HP from the pool UNCONDITIONAL; grid-variant EMERGE (0x40/0x41/0x43). */
        e->hp = (int16_t)s_tyrant_hp[re15_engine_rand8() & 0xf];   /* +0x9a UNCONDITIONAL @0x80111c04
                                                                    * (audit #14: dropped the port-invented
                                                                    * `if(hp<=0)` guard — no branch exists
                                                                    * between the rng call and the sh @0x80111c04) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;      /* clear +0x94/95/8f/93 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;                   /* +0x1bc/+0x1be */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->birkin_hurt_cd = 0; e->hit_stun = 0; e->ai_timer = 0; e->grab_kill_ctr = 0;
        /* +0x1b8=0 @0x80111af8 / +0x1b9=0x19(25) @0x80111b04-08 are the shared-walker init seeds — no port
         * consumer (audit #14, +0x1b8/+0x1b9 map to neck_flags/member_0a which no enemy path reads). */
        e->member_0a = 0x19;
        if (e->grid_id == 0x40)      { e->state = 4; e->sub_state_1 = 0; }          /* EMERGE var0 @0x80111b5c */
        else if (e->grid_id == 0x41) { e->state = 4; e->sub_state_1 = 1; }          /* EMERGE var1 @0x80111b7c */
        else if (e->grid_id == 0x43) { e->state = 4; e->sub_state_1 = 2; }          /* EMERGE var2 @0x80111ba0 */
        else                         { e->state = 1; e->sub_state_1 = 0; }          /* +0x4 = 1 @0x80111a78 */
        break;

    case 1: {  /* ACTIVE 0x80111c98: decide A[sub] (may retarget) then act B[sub]. Root decrements the
                * +0x1de attack cooldown once per ACTIVE frame @0x80111e38-4c (audit #6). */
        int32_t dist = re15_enemy_player_dist(e, pl); e->dog_dist = (int16_t)dist;
        if (e->birkin_hurt_cd > 0) e->birkin_hurt_cd--;          /* +0x1de-- per hub frame @0x80111e38-4c */

        /* ---------- DECIDE A[sub] (may change +0x5) ---------- */
        switch (e->sub_state_1) {
        case 0:   /* A[0] idle-decide 0x80111e94: LOS bit +0x1d0&1 -> roar sub 2. (LOS proxy: in arc) */
            if (pl->hit_react == 0 && dist < 8000 && re15_dog_arc(e, pl, 8000, 0xc0)) { e->sub_state_1 = 2; e->sub_state_2 = 0; }
            break;
        case 1:   /* A[1] walk-decide 0x80112024: in a804(8000,0xc0) && LOS +0x1d0&1 -> roar sub 2. */
            if (pl->hit_react == 0 && dist < 8000 && re15_dog_arc(e, pl, 8000, 0xc0)) { e->sub_state_1 = 2; e->sub_state_2 = 0; }
            break;
        case 3:   /* A[3] charge/attack1/roar decide 0x801123e8 (all gated hit_react==0). */
            if (pl->hit_react == 0) {
                /* CHARGE (sub 7): a804(3000,0x100) && +0x1de==0 && player.hp>=81 && dist>=2501 (audit #9):
                 *   @0x80112404-0c arc, @0x80112428 cd, @0x80112444 slti 81, @0x80112458 sltiu 0x9c5. */
                if (dist < 3000 && re15_dog_arc(e, pl, 3000, 0x100) && e->birkin_hurt_cd == 0
                    && pl->hp >= 81 && dist >= 2501) {
                    e->hit_stun = 100;                            /* +0x1dc = 0x64 charge-arm @0x80112460-64 */
                    e->sub_state_1 = 7; e->sub_state_2 = 0;
                }
                /* ATTACK1 (sub 4): a804(1800,0x180) && +0x1de==0 (audit #0 — NOT dist<2501/±0x300):
                 *   @0x8011248c-94 arc(0x708,0x180), @0x801124b0-b8 cd. */
                else if (dist < 1800 && re15_dog_arc(e, pl, 1800, 0x180) && e->birkin_hurt_cd == 0) {
                    e->sub_state_1 = 4; e->sub_state_2 = 0;
                }
                /* ROAR (sub 2): dist>=7001 (sltiu 0x1b59 @0x80112510). */
                else if (dist >= 7001) { e->sub_state_1 = 2; e->sub_state_2 = 0; }
            }
            break;
        case 13:  /* A[13] attack2-decide 0x80113c74: dist<2000 && +0x1de==0 && hit_react==0 -> sub 14
                   * (audit #4/#5/#10 — the ATTACK2 decide lives in the sub-13 regime, not sub 0). */
            if (dist < 2000 && e->birkin_hurt_cd == 0 && pl->hit_react == 0) { e->sub_state_1 = 14; e->sub_state_2 = 0; }
            break;
        default: break;
        }

        /* ---------- ACT B[sub] ---------- */
        switch (e->sub_state_1) {
        case 0: {  /* B[0] IDLE 0x80111ee4: stand (clip 0), rng timer +0x9c=rng+59 -> WALK sub 1. */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 0); e->ai_timer = (int16_t)(re15_engine_rand8() + 59); e->sub_state_2 = 1; }  /* +0x9c @0x80111f04-14 */
            re15_birkin_anim(e);
            if (e->ai_timer > 0) e->ai_timer--;
            if (e->ai_timer == 0) { e->sub_state_1 = 1; e->sub_state_2 = 0; }       /* -> sub 1 @0x80111f88 */
            break;
        }
        case 1: {  /* B[1] WALK (slow) 0x80112124: per-frame randomized speed/slew (audit #10). */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 1); e->anim_frac = 7; e->sub_state_2 = 1; }
            int16_t sp   = (int16_t)((re15_engine_rand8() & 0x3f) + 12);            /* +0x8c=(rng&0x3f)+12 @0x80112190-a4 */
            int16_t slew = (int16_t)((re15_engine_rand8() & 0x1f) + 8);             /* +0x9e=(rng&0x1f)+8  @0x801121a8-b8 */
            re15_enemy_steer_point(e, pl->x, pl->z, slew);                          /* 0x8001a8f8(player,+0x9e) @0x801121d4 */
            if ((e->anim_frame % 12) != 0xb) re15_dog_advance(e, sp);              /* walker 0x800245d8(a0=0), foot-plant skip @0x80112258 */
            re15_birkin_anim(e);
            break;
        }
        case 2: {  /* B[2] ROAR 0x80112294: clip 2, SE1@frame 0x14 -> sub 3 (audit #10: +0x7==0 -> sub 3). */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 2); e->anim_frac = 7; e->sub_state_2 = 1; }  /* clip 2 @0x801122f0 */
            if (e->anim_frame == 0x14) re15_audio_room_se(1);                       /* SE 1 @0x80112334-44 */
            if (re15_birkin_anim(e)) { e->sub_state_1 = 3; e->sub_state_2 = 0; }    /* roar done -> sub 3 @0x801123c4 */
            break;
        }
        case 3: {  /* B[3] WALK (fast) 0x801125dc: fast regime, A[3] decides the attacks above. */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 3); e->anim_frac = 7; e->sub_state_2 = 1; }  /* clip 3 @0x8011264c */
            int16_t sp   = (int16_t)((re15_engine_rand8() & 0x3f) + 130);           /* +0x8c=(rng&0x3f)+130 @0x80112610 */
            int16_t slew = (int16_t)((re15_engine_rand8() & 0x1f) + 64);            /* +0x9e=(rng&0x1f)+64  @0x80112638 */
            re15_enemy_steer_point(e, pl->x, pl->z, slew);
            re15_dog_advance(e, sp);
            re15_birkin_anim(e);
            break;
        }
        case 4: {  /* B[4] ATTACK1 0x8011275c: clip 0xc + SE3; hit frame>=10 via a804(2200,0x180); hp-10;
                    * hp<50 -> facing grab (front sub5 / rear sub9); else SE9+knockdown+reaction-SE; recovery
                    * clip 0xd -> sub 3 + cooldown +0x1de=0x2d (audit #1/#2/#13/#15). */
            if (e->sub_state_2 == 0) {                                              /* phase 0 @0x801127a0 */
                re15_birkin_clip(e, 0x0c); e->anim_frac = 7;                        /* clip 0xc @0x801127bc (NOT clip 4) */
                re15_audio_room_se(3);                                             /* SE 3 windup @0x801127ec (NOT SE 4) */
                e->sub_state_2 = 1;
            } else if (e->sub_state_2 == 1) {                                       /* phase 1: strike */
                int done = re15_birkin_anim(e);
                /* hit ONLY frame>=10 (sltiu +0x95,0xa @0x8011284c-54 — port's old 3..9 was INVERTED) */
                if (e->anim_frame >= 10 && pl->hit_react == 0 && dist < 2200 && re15_dog_arc(e, pl, 2200, 0x180)) {
                    pl->hp = (int16_t)(pl->hp - 10);                               /* hp-=10 @0x80112898 */
                    if (pl->hp < 50) {                                             /* slti 50 @0x801128ac */
                        int rear = re15_ai_facing_aligned(e, pl);                   /* 0x8001a780 @0x801128b8 */
                        e->sub_state_1 = (uint8_t)(rear * 4 + 5);                   /* sub = facing*4+5 = 5 or 9 @0x801128c0-d0 */
                        e->sub_state_2 = 0; e->sub_state_3 = 0; break;
                    }
                    re15_audio_room_se(9);                                         /* on-hit SE 9 @0x801128d4-d8 (NOT SE 4) */
                    pl->hit_react |= 1;                                            /* aca58=2 knockdown proxy @0x801128f0 */
                    /* reaction-SE table @0x8011a150 via 0x80037edc @0x80112920-48 (data-driven — faithful) */
                    e->sub_state_2 = 2;
                }
                if (done) e->sub_state_2 = 2;
            } else if (e->sub_state_2 == 2) {                                       /* phase 2: play out clip 0xc */
                if (re15_birkin_anim(e)) { re15_birkin_clip(e, 0x0d); e->anim_frac = 7; e->grab_kill_ctr = 0xa; e->sub_state_2 = 3; }  /* recovery clip 0xd @0x8011296c, +0x9e=0xa @0x801129b0 */
            } else {                                                               /* phase 3/4: recovery -> sub 3 */
                re15_birkin_anim(e);
                if (e->grab_kill_ctr > 0) e->grab_kill_ctr--;                       /* +0x9e-- @0x80112a0c */
                if (e->grab_kill_ctr == 0) {
                    e->birkin_hurt_cd = 0x2d;                                      /* +0x1de=0x2d (45) @0x80112a20-24 */
                    e->sub_state_1 = 3; e->sub_state_2 = 0;                        /* -> sub 3 @0x80112a30-34 */
                }
            }
            break;
        }
        case 14: {  /* B[14] ATTACK2 0x80113e48: clip 0x17 + SE3; hit frame>=11 (limb spheres -> close-arc
                     * proxy, OPEN); hp-10; hit -> GRAB2 sub 15; miss -> sub 13 + cooldown +0x1de=0x1e
                     * (audit #4/#5/#13/#15). */
            if (e->sub_state_2 == 0) {                                              /* phase 0 @0x80113e48 */
                re15_birkin_clip(e, 0x17); e->anim_frac = 7;                        /* clip 0x17=23 @0x80113eac (NOT clip 5) */
                re15_audio_room_se(3);                                             /* SE 3 windup @0x80113ed0 (NOT SE 4) */
                e->sub_state_2 = 1;
            } else if (e->sub_state_2 == 1) {
                int done = re15_birkin_anim(e);
                /* hit ONLY frame>=11 (sltiu +0x95,0xb @0x80113fac); limb-sphere hit -> close arc proxy (OPEN) */
                if (e->anim_frame >= 11 && pl->hit_react == 0 && dist < 2200 && re15_dog_arc(e, pl, 2200, 0x180)) {
                    pl->hp = (int16_t)(pl->hp - 10);                               /* hp-=10 @0x80113ff8 (no SE at the damage site) */
                    pl->hit_react |= 1;
                    e->sub_state_1 = 15; e->sub_state_2 = 0; e->sub_state_3 = 0;    /* hit -> GRAB2 sub 15 @0x80114004-08 */
                    break;
                }
                if (done) e->sub_state_2 = 2;
            } else {                                                               /* miss exit @0x801140d0 */
                e->birkin_hurt_cd = 0x1e;                                          /* +0x1de=0x1e (30) @0x801140d0-d4 */
                e->sub_state_1 = 13; e->sub_state_2 = 0;                           /* -> sub 13 regime @0x801140e0-e4 */
            }
            break;
        }
        case 5:    /* B[5] GRAB1 front 0x80112a64 (clip 0xe) */
        case 9: {  /* B[9] GRAB1 rear  0x80112f58 (clip 0x12, aca59=1) — shared machine, variant by sub. */
            int rear = (e->sub_state_1 == 9);
            if (e->sub_state_2 == 0) {                                              /* LATCH phase */
                re15_birkin_clip(e, (uint8_t)(rear ? 0x12 : 0x0e)); e->anim_frac = 7;  /* clip 0xe/0x12 @0x80112ac4 / @0x80112fb8 */
                s_player_grabbed = 1; e->birkin_grab = 1;                          /* aca58=5 PIN @0x80112b34 / @0x8011302c */
                re15_player_victim_latch(e, pl); g_player_victim_variant = (uint8_t)rear;  /* aca59=rear @0x80113034 */
                pl->hit_react |= 1;                                               /* player+0x93|=1 @0x80112b80 */
                e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);  /* a8f8(±0x800) @0x80112b8c */
                e->ai_timer = 100;                                                /* +0x9c escape budget @0x80112bc8 */
                e->grab_kill_ctr = 0x32;                                          /* +0x9e HOLD timer = 50 @0x80112bd8 */
                e->sub_state_2 = 1;
            } else {                                                               /* HOLD/DRAIN phase */
                s_player_grabbed = 1;
                re15_birkin_anim(e);
                /* hold expiry (+0x9e) OR player dead -> COLLAPSE sub 8 (cmd 6 throw/finish) @0x80112c58 */
                if (e->grab_kill_ctr <= 0 || pl->hp < 0) { e->sub_state_1 = 8; e->sub_state_2 = 0; e->sub_state_3 = 0; break; }
                e->grab_kill_ctr--;                                               /* +0x9e-- HOLD countdown */
                e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 100 * re15_mash_pressed()));  /* +0x9c -= 1+100*mash @0x80112c6c-9c */
                if (e->ai_timer < 0) {                                             /* MASHED FREE -> release @0x80112cc4 */
                    s_player_grabbed = 0; e->birkin_grab = 0;
                    re15_player_victim_throwoff(); s_grab_mercy_timer = 0x5a;      /* aca5a=4 release @0x80112cc4 */
                    re15_birkin_clip(e, 0x10); e->anim_frac = 7;                   /* release clip 0x10 @0x80112cd8 */
                    e->birkin_hurt_cd = 0x3c;                                      /* +0x1de=0x3c (60) @0x80112dac */
                    e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;     /* -> sub 3 @0x80112dbc */
                }
            }
            break;
        }
        case 8: {  /* B[8] COLLAPSE 0x80112e20: clip 0x11 + grab-cmd 6 (throw/finish) + SE 8 -> devour. */
            s_player_grabbed = 1;
            if (e->sub_state_2 == 0) {
                re15_birkin_clip(e, 0x11); e->anim_frac = 7;                        /* clip 0x11 @0x80112e5c */
                re15_audio_room_se(8);                                            /* SE 8 @0x80112ee0 */
                re15_player_victim_devour(e);                                     /* aca58=6 devour/finish @0x80112ea4 */
                e->sub_state_2 = 1;
            } else if (re15_birkin_anim(e)) {                                       /* collapse played out -> release to sub 3 */
                s_player_grabbed = 0; e->birkin_grab = 0;
                e->birkin_hurt_cd = 0x3c;
                e->sub_state_1 = 3; e->sub_state_2 = 0;
            }
            break;
        }
        case 15: { /* B[15] GRAB2 0x8011411c: carry-pin — clip 0x18 drag, cmd5, clip 0x19, +0x9c=100 mash,
                    * clip 0x1a, cooldown +0x1de=0x1e -> sub 13 (audit #5). */
            s_player_grabbed = 1;
            if (e->sub_state_2 == 0) {
                re15_birkin_clip(e, 0x18); e->anim_frac = 7;                        /* clip 0x18 @0x80114178 */
                e->birkin_grab = 1; re15_player_victim_latch(e, pl); pl->hit_react |= 1;
                e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
                e->ai_timer = 100;                                                /* +0x9c mash budget @0x801142f4 */
                e->sub_state_2 = 1;
            } else {
                re15_dog_advance(e, 40);                                          /* drag: pos_advance(0x66c) @0x801141e0-e4 (player X/Z follow) */
                pl->x = e->x; pl->z = e->z;                                       /* drag playerX/Z @0x80114204/14 */
                re15_birkin_anim(e);
                e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 100 * re15_mash_pressed()));  /* mash 0x80037024 @0x80114340 */
                if (e->ai_timer < 0 || pl->hp < 0) {                              /* release clip 0x1a @0x801143a4 */
                    s_player_grabbed = 0; e->birkin_grab = 0;
                    re15_player_victim_throwoff(); s_grab_mercy_timer = 0x5a;
                    re15_birkin_clip(e, 0x1a); e->anim_frac = 7;
                    e->birkin_hurt_cd = 0x1e;                                      /* +0x1de=0x1e @0x8011441c-20 */
                    e->sub_state_1 = 13; e->sub_state_2 = 0;                       /* -> sub 13 (0xd) @0x8011442c-30 */
                }
            }
            break;
        }
        case 6: {  /* B[6] STAGGER->CHARGE 0x80114494 (audit #8): post-hurt threat pose clip 6, hold
                    * ~120-135f, snapshot hp -> +0x1dc, break into CHARGE sub 7 on timer==0 OR front arc. */
            if (e->sub_state_2 == 0) {                                              /* phase 0 @0x801144c4 */
                re15_birkin_clip(e, 6); e->anim_frac = 7;                           /* clip 6 @0x801144d8 (the real stagger pose) */
                e->ai_timer = (int16_t)((re15_engine_rand8() & 0xf) + 120);         /* +0x9c=(rng&0xf)+120 @0x8011451c-20 */
                e->hit_stun = e->hp;                                               /* +0x1dc = +0x9a hp snapshot @0x80114530-38 */
                e->sub_state_2 = 1;
            }
            re15_birkin_anim(e);
            if (e->ai_timer > 0) e->ai_timer--;
            if (e->ai_timer == 0 || re15_dog_arc(e, pl, 30000, 0x400)) {           /* timer==0 OR arc(0x400) @0x80114570-88 */
                e->sub_state_1 = 7; e->sub_state_2 = 0;                            /* -> CHARGE sub 7 @0x8011459c */
            }
            break;
        }
        case 7: {  /* B[7] CHARGE 0x801145c8 (audit #9): clip 4; if +0x1dc==100 && player outside arc 0x400
                    * -> clip 5 swipe + FX 0x0d002000 + cooldown +0x1de=45; done -> sub 3. */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 4); e->anim_frac = 7; e->sub_state_2 = 1; }  /* clip 4 @0x80114624 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            re15_dog_advance(e, 60);
            if (e->sub_state_2 == 1 && e->hit_stun == 100 && !re15_dog_arc(e, pl, 30000, 0x400)) {  /* escalate to the swipe @0x80114668 */
                re15_birkin_clip(e, 5); e->anim_frac = 7;                           /* clip 5 @0x80114678 */
                /* FX 0x80019700(0x0d002000, table @0x8011a158) @0x801146d4-ec — render-side, OPEN */
                e->birkin_hurt_cd = 0x2d;                                          /* +0x1de=45 @0x801146fc-700 */
                e->hit_stun = 0; e->sub_state_2 = 2;
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 3; e->sub_state_2 = 0; }    /* done -> sub 3 @0x8011474c */
            break;
        }
        case 11: { /* B[11] TURN 0x80113538: fast-turn clip 0x16 -> sub 13 regime (audit #10). */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 0x16); e->anim_frac = 7; e->sub_state_2 = 1; }  /* clip 0x16 @0x80113598 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            if (re15_birkin_anim(e)) { e->sub_state_1 = 13; e->sub_state_2 = 0; }   /* turn done -> sub 0xd(13) @0x80113a50-54 */
            break;
        }
        case 13: { /* B[13] WALK (sub-13 regime) 0x80113d3c: walk; A[13] decides ATTACK2 above. */
            if (e->sub_state_2 == 0) { re15_birkin_clip(e, 1); e->anim_frac = 7; e->sub_state_2 = 1; }
            re15_enemy_steer_point(e, pl->x, pl->z, (int16_t)((re15_engine_rand8() & 0x1f) + 8));
            re15_dog_advance(e, (int16_t)((re15_engine_rand8() & 0x3f) + 12));
            re15_birkin_anim(e);
            break;
        }
        default:
            e->sub_state_1 = 1; e->sub_state_2 = 0;                                /* unknown sub -> walk */
            re15_birkin_anim(e);
            break;
        }
        break; }

    case 2: {  /* HURT 0x80114770 (audit #7/#8): full react clip 4 (or 7 while +0x1dc set) + gore/blood +
                * SE 0; hits DURING an armored window are hp-neutral (snapshot restore); then STAGGER sub 6.
                * Grab/charge subs resume at CHARGE sub 7 (0x701). */
        if (e->sub_state_3 == 0) {                                                 /* phase 0: react clip */
            re15_birkin_clip(e, (uint8_t)(e->hit_stun ? 7 : 4)); e->anim_frac = 7;  /* clip 4, or 7 if +0x1dc @0x801148c8/e8 */
            e->hit_react = (uint8_t)(e->hit_react | 2);                            /* gore flag +0x93|=2 @0x801148a4 */
            re15_audio_room_se(0);                                                 /* SE 0 @0x801149a0 */
            /* blood FX 0x80019700 @0x80114998 — render-side, OPEN */
            e->sub_state_3 = 1;
        }
        if (re15_birkin_anim(e)) {                                                 /* react anim done -> STAGGER sub 6 (0x601 @0x801149fc) */
            e->state = 1;
            if (e->hit_stun) { e->hit_stun = 100; e->sub_state_1 = 7; }            /* grab/charge hurt -> CHARGE sub 7 @0x80114a1c-2c */
            else e->sub_state_1 = 6;                                               /* normal hurt -> STAGGER sub 6 */
            e->sub_state_2 = 0; e->sub_state_3 = 0;
            e->hit_react = (uint8_t)(e->hit_react & ~1u);
        }
        break;
    }

    case 3:   /* DEATH 0x80114cb0 (audit #12): two-stage. Phase 0/1 fall clip 8 (gib clip 9 if +0x93&0x80)
               * + blood FX + SE 2, SE 7 @frame 0x18 -> phase 2 settle clip 0xa (0xb if gore +0x93&2) ->
               * CORPSE (state 7). */
        switch (e->sub_state_3) {
        case 0:   /* phase 0: fall/gib clip + SE 2 */
            re15_birkin_clip(e, (uint8_t)((e->hit_react & 0x80) ? 9 : 8)); e->anim_frac = 7;  /* clip 8 / gib 9 @0x80114d28-50 */
            re15_audio_room_se(2);                                                 /* SE 2 @0x80114e04 */
            /* blood FX 0x80019700 @0x80114dfc — render-side, OPEN */
            e->sub_state_3 = 1;
            break;
        case 1:   /* phase 1: play the fall; SE 7 @frame 0x18 -> phase 2 */
            if (e->anim_frame == 0x18) re15_audio_room_se(7);                       /* SE 7 @0x80114ee4-e8 */
            if (re15_birkin_anim(e)) e->sub_state_3 = 2;
            break;
        case 2:   /* phase 2: settle clip 0xa (0xb if gore) */
            re15_birkin_clip(e, (uint8_t)((e->hit_react & 2) ? 0x0b : 0x0a)); e->anim_frac = 7;  /* clip 0xa/0xb @0x80114f18-40 */
            e->sub_state_3 = 3;
            break;
        default:  /* phase 3/4: play settle -> CORPSE */
            if (re15_birkin_anim(e)) { e->state = 7; e->ai_timer = 0x5a; e->sub_state_3 = 0; }  /* -> state 7, corpse fade 90f @0x80114fcc-d0 / @0x80115b1c */
            break;
        }
        break;

    case 4:   /* EMERGE 0x80114fe4 (audit #11): variant clips 0x1c/0x1d (sub0/grid-0x41) & 0xa (sub2) —
               * NOT clip 0x10 (the GRAB1 release clip). sub_state_1 = the emerge variant from INIT. */
        if (e->sub_state_2 == 0) {
            uint8_t clip = (e->sub_state_1 == 2) ? 0x0a                             /* sub2 0x801155ac: clip 0xa @0x8011569c + 8x dust FX (OPEN) */
                         : (e->sub_state_1 == 1) ? 0x1d                             /* grid-0x41 override: clip 0x1d @0x801150e4 */
                         :                          0x1c;                           /* sub0 0x8011502c: clip 0x1c @0x801150c0 */
            re15_birkin_clip(e, clip); e->anim_frac = 7;
            e->sub_state_2 = 1;
        }
        if (re15_birkin_anim(e)) { e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* -> ACTIVE (0x101) */
        break;

    case 7:   /* CORPSE (state table [7] 0x80115af0): 90-frame timer then inert. The +0xc4/+0xec tint and
               * +0xbc/+0xbe sink (@0x80115b6c-b0) are render-side (same channel as zombie-girl L4608). */
        if (e->ai_timer > 0) e->ai_timer--;                                        /* +0x9c-- corpse fade @0x80115b1c */
        re15_birkin_anim(e);
        break;

    default:
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ IVY plant-grappler (type 0x2d, EM02D) — STAGE4 lab ============== *
 * Byte-true rebuild from audit wf_efd92a2c (root 0x801168c4; state table @0x8011a2c0 raw-read =
 * EXACTLY two code pointers: INIT 0x80116920, ACTIVE 0x801169b8 — word[2] @0x8011a2c8 = 0xfa060000
 * is DATA, so no state>=2 dispatch can exist without jalr'ing data).
 *
 * The shipped ivy is a DORMANT SCRIPTED PROP, NOT a combat enemy. The prior port INVENTED an entire
 * nav-chase + grab-instakill + hurt/death/corpse machine by mis-reading the NEIGHBOURING type-0x40
 * NPC root (0x80116be4, registered right after the ivy) as the ivy brain:
 *   - The ivy chain (0x801168c4-0x80116bcc) makes ZERO walker (0x800245d8)/pathfind (0x800509e4) calls
 *     and NEVER reads the player. The walker+pathfind-15000 chase is type-0x40 state-1 @0x80116ec8
 *     (jal 0x800509e4, a0=0x3a98=15000) — every chase constant (steer 0x30, adv 40/48, dist 1400/900,
 *     timer 60, miss-40) has no source in the ivy code (audit ivy #73/#78).
 *   - ACTIVE 0x801169b8 dispatches on +0x5: sub 0 @0x80116a08 = DORMANT-until-flag — jal
 *     0x8004efe4(0x800b1028,0x1f) = game-flag z5:31 test @0x80116a14; ONLY when set does it consume the
 *     flag (jal 0x8004efb8 @0x80116a30) and sub++ @0x80116a34. NO player read, NO movement. sub 1
 *     @0x80116a40 = clip-field reset (+0x94/+0x95/+0x8f=0), sub++, falls through into sub 2. sub 2
 *     @0x80116a58 = procedural bone-sway (skeleton words of model inst lw 392(s0), keyed on +0x95 bands).
 *     sub>=3 = `j 0x80116bcc` epilogue = inert forever (audit ivy #73).
 *   - The grab VM (sw 7->0x800aca58 @0x80116858, driver 0x801165f4, 5 stages @0x801003c4) is ORPHANED
 *     prototype code: unreachable — the ivy dispatch table never points at 0x801165f4 and STAGE4's
 *     grabbed-cmd registrar never writes the 0x2d driver slot 0x800ac80c. The shipped ivy has NO attack;
 *     0x80116850-58 is a lone store in dead code, not a grab latch (audit ivy #74/#79).
 *   - TRIPLE weapon-immunity: dmg row @0x8006f048 (PSX.EXE, 0x8006e0d0+0x2d*0x58) verified ALL-ZERO;
 *     INIT sets +0x93=3 (ori v0,3 @0x8011693c; sb v0,147(a1) @0x80116944) tripping the take_damage
 *     hit-once guard (hit_react&1 -> early return); NO +0x78 damage box is ever installed — the
 *     {450,1530} box @0x8011a2c8 is consumed by the type-0x40 NPC INIT 0x80116d20 (lw [0x8011a2d4]->
 *     +0x78), NOT the ivy. So the ivy is never a weapon/auto-aim target nor a body-push obstacle, and the
 *     engine never sets its state beyond 1 (audit ivy #75/#76/#77).
 *   - The root 0x801168c4-0x8011691c is pause-gate + state jalr ONLY: NO collision/push/shadow/anim-SFX
 *     tail (contrast the 0x40 root 0x80116be4 which calls them @0x80116c98-0x80116d08) (audit ivy #81).
 *
 * INIT side effects modeled: HP=0x64 UNCONDITIONAL @0x80116954, +0x93=3 @0x8011693c-44, clip 0
 * @0x8011698c-94, +0x4=1 @0x80116988.  OPEN (render-side, HONEST — no port equivalent, same treatment
 * as alligator/birkin): DAT_800acc0c:=1 @0x80116938 (shared-EXE animator secondary-motion pose switch),
 * the anim_set 0x8001f314(a3=0x200) emerge pose @0x801169a0, and the sub-2 bone-sway skeleton program. */
static void re15_ivy_ai_tick(int slot)
{
    re15_actor_t *e = &g_actors[slot];

    switch (e->state) {
    case 0:   /* INIT 0x80116920: HP 100 unconditional, +0x93=3 immunity guard, clip 0, state 1. */
        e->hp = 100;                              /* +0x9a = 0x64 UNCONDITIONAL @0x80116954 (not `if hp<=0`) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;   /* clip 0 fields @0x8011698c-94 */
        e->hit_react = 3;                         /* +0x93 = 3 @0x8011693c-44 = take_damage hit-once guard set */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->state = 1;                             /* +0x4 = 1 @0x80116988 */
        /* OPEN: DAT_800acc0c:=1 @0x80116938 + anim_set emerge @0x801169a0 (render-side). NO player read. */
        break;

    case 1:   /* ACTIVE 0x801169b8: dormant scripted prop — dispatch on +0x5. NO player read, NO movement. */
        switch (e->sub_state_1) {
        case 0:   /* sub 0 @0x80116a08: DORMANT until the room SCD sets game-flag z5:31. */
            if (re15_game_flag_get(5, 31)) {                  /* jal 0x8004efe4(0x800b1028,0x1f) @0x80116a14 */
                re15_game_flag_set(5, 31, 0);                 /* consume: jal 0x8004efb8 @0x80116a30 */
                e->sub_state_1++;                             /* sb (+0x5)+1 @0x80116a34 */
            }
            break;
        case 1:   /* sub 1 @0x80116a40: reset clip fields, advance — falls through into the sub-2 sway. */
            e->sub_state_2 = 0; e->sub_state_3 = 0; e->anim_frame = 0;  /* +0x94/+0x95/+0x8f=0 @0x80116a44-4c */
            e->sub_state_1++;                                 /* +0x5+1 @0x80116a54 (no branch -> 0x80116a58) */
            /* fallthrough */
        case 2:   /* sub 2 @0x80116a58: procedural bone-sway (render-side, stays at sub 2). */
            /* OPEN (HONEST): +0x95-banded skeleton-word writes to model inst @0x80116a58-0x80116bc0. */
            break;
        default:  /* sub>=3 @0x80116bcc: inert forever. */
            break;
        }
        break;

    default:  /* states >=2 are UNREACHABLE: the state table @0x8011a2c0 has no entry (word[2]=data) and
               * the ivy is weapon-immune (+0x93=3 guard + no +0x78 box), so take_damage never promotes it
               * past state 1. Keep it pinned to the dormant ACTIVE state rather than jalr'ing data. */
        e->state = 1;
        break;
    }
}

/* Shared ENEMY-vs-ENEMY body separation (the FUN_8002b544 pass of the enemy root tail): push THIS
 * ground enemy out of every OTHER active, non-corpse enemy with a hitbox. The original enemy roots
 * run aec4(&player,this)+b544(others); in the PORT the enemy-vs-PLAYER half is already the separate
 * re15_body_push_player pass (game_step, live-verified for the maggot bite), so only the enemy-vs-
 * enemy half was missing — non-zombie swarms (maggots, dog packs, spiders) inter-penetrated to a
 * single point (audit wf_246147e3). Adding aec4(player) here would DOUBLE-push AND over-separate
 * (hit_radius_min is the DAMAGE box: maggot 1600 / alligator 2200 -> the enemy is shoved out of
 * attack range), so we do only the b544 enemy-vs-enemy loop. No grab-freeze exemption needed here
 * (that gate is for the zombie's own aec4-vs-player, which this does not do). */
static void re15_enemy_body_push_tail(int s, re15_actor_t *e)
{
    if (e->state == (uint8_t)RE15_AI_STATE_CORPSE || e->hit_radius_min == 0) return;
    for (int o = RE15_ACTOR_SLOT_PLAYER + 1; o < RE15_ACTOR_MAX; o++) {          /* b544: vs every other enemy */
        if (o == s) continue;
        re15_actor_t *z2 = &g_actors[o];
        if (!z2->active || z2->hit_radius_min == 0) continue;
        if (z2->state == (uint8_t)RE15_AI_STATE_CORPSE) continue;
        re15_body_push(z2, (int32_t)z2->hit_radius_min, e, (int32_t)e->hit_radius_min);
    }
}

/* Phase 8.6 — the per-frame LIVE-zombie AI pass. The port's faithful, TYPE-GATED slice of the
 * original entity-update loop FUN_8001a50c (@0x8001ce04): the original walks the entity array
 * (DAT_800acc2c, stride 0x1f4) and, for every active entity (+0x0 & 1), dispatches its per-type
 * tick @0x80072bac[entity+0x8 type]. The port has the player on its own path (re15_player_tick)
 * and only the LIVE STAGE1 zombie types (0x10/0x11/0x16) ported here, so this runs JUST those
 * through re15_enemy_ai_live_step (FUN_80100424 tick + the shared lunge slice). Every other type
 * (Elliot 0x47, crows 0x21, room props) is left to its existing handling. Because of the type
 * gate, a room with no live zombie (e.g. the ROOM1170 boot/helipad) makes this a pure no-op =
 * no 1170 regression. `combat_active` is forwarded to the arm gate (DAT_800aca3c & 1). */
/* Byte-true FUN_80109554/FUN_80106edc index math: stage 0-based from the room id (ROOM1140 -> 0);
 * the em-status kill-flag bank is SCD flag zone 7 (stage<3) / 8 (stage>=3) = PSX 0x800b1038 /
 * 0x800b1058 (op_set table @0x80074664[7]/[8], DAT_800b0fe0<3 discriminator @0x800420ec). */
uint8_t re15_em_status_zone(void)
{
    int stage0 = (int)((g_current_room_id >> 12) & 0xFu) - 1;   /* room id packs (stage+1)<<12 */
    return (stage0 < 3) ? 7 : 8;
}

/* FUN_8001b38c (@0x8001b38c, called by the zombie root each tick): per-frame ANIM-SFX. Reads the CURRENT
 * clip frame's packed u32 (entity+0x168 word0 in the original), takes the top 10 bits (>>22) as a sound
 * bitmask, and for each set bit N plays room-SE N via FUN_800453d0 (= re15_audio_room_se). EM10/EM11 carry
 * these flags in the LOCO bank (bit 1 = SE 1 footstep, ~2/walk-cycle) and the action bank (SE 0/1/3 at the
 * hurt/attack/lie clips) — verified by dumping frames[]>>22. Byte-true bank selection = the render's: the
 * STAGE1 walk states (state 1, sub_state_1 in {0x13,2,7}) pose the loco bank, everyone else the action bank. */
static void re15_enemy_anim_sfx(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return;
    const re15_emd_animation_t *A = &b->anim;
    if (e->state == 1 && (e->sub_state_1 == 0x13 || e->sub_state_1 == 2 || e->sub_state_1 == 7)
        && b->loco_ok && (int)e->motion < b->anim_loco.clip_count)
        A = &b->anim_loco;                                  /* the walk states play the loco clip (main.c:3166) */
    if ((int)e->motion >= A->clip_count) return;
    const re15_emd_clip_t *c = &A->clips[e->motion];
    if (c->frame_count <= 0) return;
    uint32_t sfx = A->frames[c->first_frame + (e->anim_frame % c->frame_count)] >> 22;  /* top 10 = SFX mask */
    for (int bit = 0; sfx; bit++, sfx >>= 1)
        if (sfx & 1u) re15_audio_room_se(bit);              /* FUN_800453d0(bit): SE id == bit index */
}

void re15_enemy_ai_run_all(int combat_active)
{
    re15_enemy_ai_set_combat_active(combat_active);
    /* Re-derive the player-grabbed latch each frame (the faithful-line release for the deferred
     * player grabbed-FSM, see s_player_grabbed): cleared here, then re15_enemy_ai_live_grab sets it
     * for any live zombie in the grab sub-mode this frame. A room with no grabbing zombie (or no
     * live zombie at all, e.g. ROOM1170) leaves it 0 → game_step never pins the player = 1170-safe. */
    s_player_grabbed = 0;
    /* just-escaped mercy timer (the thrower's +0x1d5, ticked by FUN_8010a8c8; clears DAT_800aca50&1
     * at 0 — port: the global timer stands in, see re15_grab_mercy_active). */
    if (s_grab_mercy_timer > 0) s_grab_mercy_timer--;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        /* EM-STATUS KILL PERSISTENCE (byte-true FUN_80109554 @0x801096fc / FUN_80106edc @0x8010716c set
         * flag[entity+0x1C6] on the death-commit): once an enemy is a CORPSE (state 7), set its em-status
         * kill flag so re-entering the room does NOT respawn it (the Sce_em_set gate GETs this flag). The
         * set is idempotent (re15_game_flag_set writes the same bit) so doing it each corpse tick == the
         * PSX's set-once-on-commit for the spawn gate's purposes; em_flag_id==0xFF means "never persist". */
        if (e->state == (uint8_t)RE15_AI_STATE_CORPSE && e->em_flag_id != 0xFF)
            re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);
        uint8_t t = e->type;
        if (t == 0x10 || t == 0x11 || t == 0x16 || t == 0x12 || t == 0x18) { /* the STAGE1-5 zombie
             * variants that share the root FUN_80100424: 0x10/0x11/0x16 (briefing) plus 0x12 and 0x18,
             * which have real type-indexed HP rows (@0x8011f034+type*0x20: 0x12={71,91,105,..},
             * 0x18={71,93,75,..}). Types 0x1c/0x1d/0x1e/0x1f are ALSO registered to this root (STAGE1
             * sw 0x80100424 -> 0x80072c1c/c20/c24/c28 @0x8011e88c-e8cc; STAGE3 raw-verified sw 0x80100510
             * -> 0x80072c1c/c20 @0x8011cf00-cf08) but their HP rows @0x8011f034+type*0x20 are all-ZERO
             * (base 0x8011f3b4..0x8011f433) and their EXE dmg rows @0x8006e0d0+type*0x58 are all-zero too
             * = unused registration slots, so they are deliberately NOT routed (a spawned one would be a
             * 0-HP walking zombie, weapon-immune, dying to the first hit — but there are 0 such spawns in
             * all 240 shipped RDTs, byte-true census). Keeping them UNROUTED is the accepted byte-true
             * decision (audit wf_efd92a2c dormant #4, reachable=NO). Adding 0x12/0x18 is safe for the
             * tested rooms: ROOM1140/1170 contain only 0x10/0x11/0x16, so their combat is unchanged. */
            int32_t sweep_ox = e->x, sweep_oz = e->z;    /* pre-dispatch pos (wall-sweep origin) */
            re15_enemy_ai_live_step(s);
            re15_enemy_anim_sfx(e);        /* FUN_8001b38c: per-frame clip-flag SFX (footsteps/attack) */
            /* (hit blood moved INTO re15_enemy_ai_live_hurt phase 0 — the byte-true spot,
             * @0x80105c50-d8; the old call here could never fire, see the note there) */
            re15_enemy_gore_tick(e);    /* FUN_80106a44 +0x93&2 -> gore effect spawn */
            re15_enemy_gore_setup(e);   /* FUN_80106edc sub_state_1==0x58 -> effect-5 gore setup */
            /* CONTACT CLEAR — byte-true FUN_8002b498 position (@0x801005f4, AFTER the state
             * dispatch, BEFORE the pushes): the handlers above read LAST tick's +0x1c2 (the grab-[5]
             * domino read @0x801029ec) — clearing at tick START kept the domino permanently dead =
             * "the push-out feels dead" (user report). */
            e->contact_flags = 0;
            /* BODY COLLISION, zombie side (byte-true FUN_8010a8c8 tail: aec4(&player, this) + the
             * b544 pass): this zombie is pushed out of the LIVE player and the other zombies.
             * FREEZE GATE (aec4 entry: skip when BOTH carry the 0x1000 freeze): the grab sets the
             * zombie freeze @[0] and the cmd-5 handler pins the player -> the GRABBING pair is
             * exempt (live-verified: the original face-grab closed to d=293, the devour to d=48 —
             * impossible with the push active). Gate = this zombie in the grab machine (+0x5 3/4,
             * all sub-steps incl. throw-off/recovery) or devouring (5/6). */
            if (e->state != (uint8_t)RE15_AI_STATE_CORPSE && e->hit_radius_min != 0) {
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                int pair_frozen = (e->state == 1 && e->sub_state_1 >= 3 && e->sub_state_1 <= 6);
                                                       /* the both-0x1000 AND: only the ACTIVE grab
                                                        * window (a HURT sub 4 must not be exempt) */
                if (pl->hp >= 0 && !pair_frozen)
                    if (re15_body_push(pl, RE15_BODY_R_PLAYER, e, (int32_t)e->hit_radius_min))
                        e->contact_flags |= 1;         /* +0x1c2 |= 1: the PLAYER pushed me (aec4) */
                for (int o = RE15_ACTOR_SLOT_PLAYER + 1; o < RE15_ACTOR_MAX; o++) {
                    if (o == s) continue;
                    re15_actor_t *z2 = &g_actors[o];
                    if (!z2->active || z2->hit_radius_min == 0) continue;
                    if (z2->state == (uint8_t)RE15_AI_STATE_CORPSE) continue;
                    if (re15_body_push(z2, (int32_t)z2->hit_radius_min, e, (int32_t)e->hit_radius_min)) {
                        e->contact_flags |= 2;         /* +0x1c2 |= 2 + +0x1ac = the pushing ENEMY */
                        e->contact_slot   = (int8_t)o; /* -> the grab [5] domino target */
                    }
                }
            }
            /* ENEMY SCA WALL CLAMP — byte-true order (@0x8010062c: b0a4 runs AFTER aec4+b544,
             * unconditionally): pushes can no longer leave a zombie inside a wall. */
            if (g_room_rdt_ok && (e->x != sweep_ox || e->z != sweep_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, sweep_ox, sweep_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x21) {   /* CROW (type 0x21) — 3D flight AI (Wave 1: INIT + cruise).
                                 * Own branch: flies in 3D, NOT ground-clamped, no body-push. */
            re15_crow_ai_tick(s);
        }
        else if (t == 0x20) {   /* DOG (Cerberus, type 0x20) — ground chase/bite AI.
                                 * Ground enemy: SCA wall-clamp after the tick (byte-true 0x8003b0a4). */
            int32_t dog_ox = e->x, dog_oz = e->z;
            re15_dog_ai_tick(s);
            re15_enemy_body_push_tail(s, e);                  /* aec4+b544 body separation (dog root tail) */
            /* the SCA resolver clears the contact bit at ITS start (@0x8003b1dc), i.e. AFTER the AI
             * dispatch in the same frame — so the AI reads LAST frame's +0x90 contact. The port used
             * to clear BEFORE the tick, which zeroed bit0 for every AI read and killed the reroute
             * gate + the +0x1dc blocked counter (audit wf_827f186d dog #12/#15). */
            e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);
            if (re15_dog_consume_noclamp(s)) {
                /* reroute-13 leap burst: the original snaps the old-pos mirror +0x40/+0x42/+0x44 to
                 * the post-burst position (@0x801104e0-52c) so the SCA sweep does not clamp the hop
                 * back off the obstacle — skip the port's sweep for this frame. */
            } else if (g_room_rdt_ok && (e->x != dog_ox || e->z != dog_oz)) {
                int32_t ix = e->x, iz = e->z;                 /* the AI's INTENDED position this frame */
                int32_t nx = ix, nz = iz;
                re15_collision_constrain_enemy(&g_room_rdt, dog_ox, dog_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
                if (nx != ix || nz != iz) {                   /* the clamp moved it = WALL CONTACT (the SCA resolver's +0x90) */
                    int push = ((int)re15_atan2_q12(nz - iz, nx - ix) - 0x400) & 0xfff;  /* wall-normal / escape heading */
                    e->ai_contact = (uint8_t)((((push >> 8) & 0xf) << 4) | 1);   /* +0x90: hi-nibble heading (16 dirs) + bit0 contact */
                }
            }
        }
        else if (t == 0x26) {   /* SPIDER-BABY (type 0x26) — stationary web-spitter (Wave 1).
                                 * Emerges vertically + -2 contact; does NOT translate -> no wall-clamp. */
            re15_spider_ai_tick(s);
        }
        else if (t == 0x25) {   /* ADULT SPIDER (type 0x25, EM025, STAGE2) — mobile ground creature (Wave 1).
                                 * Chases the player -> SCA wall-clamp after the tick like the maggot/dog. */
            int32_t asp_ox = e->x, asp_oz = e->z;
            re15_adult_spider_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != asp_ox || e->z != asp_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, asp_ox, asp_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x29) {   /* COCKROACH (type 0x29, EM029, STAGE3) — small FLYING scurrier. Root 0x80110b00
                                 * dispatches +0x4 via the 8-entry table @0x8011eca4. Scurries toward the player +
                                 * bites, and LEAPS/FLIES AWAY after a survived hit -> SCA wall-clamp after the tick. */
            int32_t rc_ox = e->x, rc_oz = e->z;
            re15_cockroach_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != rc_ox || e->z != rc_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, rc_ox, rc_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x30 || t == 0x36) {   /* G-BIRKIN boss (type 0x30 form-1 EM030 / type 0x36 form-5 EM036).
                                 * STAGE3 registers BOTH 0x30 and 0x36 to the SAME root 0x80116230, which has ZERO
                                 * +0x8 type reads and ZERO 0x36 immediates in [0x80116230..0x8011a800] — the two
                                 * forms run byte-IDENTICAL AI; the type only selects the EMD model (EM030 vs
                                 * EM036) at spawn. So one brain covers both. Wall-clamp like the rest. */
            int32_t bk_ox = e->x, bk_oz = e->z;
            re15_birkin_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != bk_ox || e->z != bk_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, bk_ox, bk_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x27) {   /* MAGGOTS (type 0x27) — large moving ground creature.
                                 * Root tail (@0x80116e28-84): body push (0x8002aec4/0x8002b544) then the
                                 * SCA resolver FUN_8003b0a4(+0x34, dim+6, 4) whose result lands in +0x1d6
                                 * (`sh v0,470(v1)` @0x80116e84) — the wall-clamp gate the rear-up
                                 * (@0x80117b18) and the far-leap (@0x80118054) test. Port: the run_all
                                 * clamp encodes the same contact into +0x90/ai_contact (dog #15 pattern);
                                 * the AI reads LAST frame's contact (resolver clears at ITS start). */
            int32_t mag_ox = e->x, mag_oz = e->z;
            re15_maggot_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);
            if (g_room_rdt_ok && (e->x != mag_ox || e->z != mag_oz)) {
                int32_t ix = e->x, iz = e->z;
                int32_t nx = ix, nz = iz;
                re15_collision_constrain_enemy(&g_room_rdt, mag_ox, mag_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
                if (nx != ix || nz != iz) {               /* clamp moved it = wall contact (+0x1d6 != 0) */
                    int push = ((int)re15_atan2_q12(nz - iz, nx - ix) - 0x400) & 0xfff;
                    e->ai_contact = (uint8_t)((((push >> 8) & 0xf) << 4) | 1);
                }
            }
        }
        else if (t == 0x40 || t == 0x42 || t == 0x45 || t == 0x47 || t == 0x49 || t == 0x4b ||
                 t == 0x4d) {
            /* NPC family — invulnerable cutscene actors that SHARE the same EXE behaviour lib (INIT →
             * HP=-1 + idle clip 2 → shared executor state 4 @0x80050be8 + shared watcher states [6-11]
             * @0x8004f2xx). STAGE1: 0x40 Irons / 0x42 / 0x45 / 0x47 Annette / 0x49 / 0x4b. STAGE6 adds
             * 0x4d — the STAGE6 overlay registers its root @0x801017a0 (dispatch @0x80072ce0), which is
             * byte-for-byte the NPC root pattern (pause gate → look helper 0x8001bd60 → nav-steer
             * 0x80039e7c → +0x4 state dispatch) and whose state table @0x80102794 has the IDENTICAL
             * EXE-shared entries ([4]=0x80050be8, [6-11]=0x8004fxxx) as the STAGE1 NPCs; its INIT
             * @0x80101918 sets entity+0x9a = -1 (invulnerable). re15_npc_ai_tick is type-agnostic
             * (state/grid-driven), so routing all seven is byte-true to the shared lib.
             * The dispatch was previously wired for 0x40 ONLY, so the others spawned stuck at state 0 /
             * motion 0 (a T-pose, never invulnerable) — confirmed in ROOM11B0 (0x42/0x4b st=0 mo=0 vs
             * 0x40 st=4 mo=2) and ROOM6000 (0x4d st=0). The walk/look/dialogue VM stays Wave-2 deferred. */
            re15_npc_ai_tick(s);
        }
        else if (t == 0x13) {   /* ZOMBIE GIRL (type 0x13) — the standard-zombie phase FSM on her own
                                 * root FUN_8010a8c8 (audit wf_827f186d zombie-girl #1). Her root TAIL
                                 * (@0x8010aa80-aaf4: b4e4 / attack-pt 0x80104178 / b498 contact clear /
                                 * aec4+b544 body pushes / SCA b0a4 / SFX 0x8001b38c) is the SAME helper
                                 * chain as the standard zombie root's -> mirror the standard branch
                                 * (grab-pair push exemption included: she runs the shared FUN_80102548
                                 * grab machine now). */
            int32_t zg_ox = e->x, zg_oz = e->z;
            re15_zgirl_ai_tick(s);
            re15_enemy_anim_sfx(e);        /* FUN_8001b38c @0x8010aad8 */
            /* (hit blood: see re15_enemy_ai_live_hurt phase 0 @0x80105c50-d8) */
            re15_enemy_gore_tick(e);       /* +0x93&2 -> gore effect spawn */
            re15_enemy_gore_setup(e);      /* sub_state_1==0x58 -> effect-5 gore setup */
            e->contact_flags = 0;          /* FUN_8002b498 @0x8010aa98 (after dispatch, before pushes) */
            if (e->state != (uint8_t)RE15_AI_STATE_CORPSE && e->hit_radius_min != 0) {
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                int pair_frozen = (e->state == 1 && e->sub_state_1 >= 3 && e->sub_state_1 <= 6);
                if (pl->hp >= 0 && !pair_frozen)
                    if (re15_body_push(pl, RE15_BODY_R_PLAYER, e, (int32_t)e->hit_radius_min))
                        e->contact_flags |= 1;
                for (int o = RE15_ACTOR_SLOT_PLAYER + 1; o < RE15_ACTOR_MAX; o++) {
                    if (o == s) continue;
                    re15_actor_t *z2 = &g_actors[o];
                    if (!z2->active || z2->hit_radius_min == 0) continue;
                    if (z2->state == (uint8_t)RE15_AI_STATE_CORPSE) continue;
                    if (re15_body_push(z2, (int32_t)z2->hit_radius_min, e, (int32_t)e->hit_radius_min)) {
                        e->contact_flags |= 2;
                        e->contact_slot   = (int8_t)o;
                    }
                }
            }
            if (g_room_rdt_ok && (e->x != zg_ox || e->z != zg_oz)) {   /* SCA b0a4 @0x8010aad0 */
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, zg_ox, zg_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x1a) {   /* WRITHE-HAZARD (type 0x1a, EM01A) — anchored, KILLABLE, solid 300-radius
                                 * body obstacle. The root runs the push chain 0x8002b498/aec4/b544/b0a4
                                 * every tick (@0x8010c2e8-324): re15_body_push_player (game_step) pushes
                                 * the player out, body_push_tail here separates other enemies (audit
                                 * wf_efd92a2c writher #1). NO wall-clamp: it never advances its own X/Z. */
            re15_writher_ai_tick(s);
            re15_enemy_body_push_tail(s, e);                  /* b544 body separation (root tail @0x8010c300) */
        }
        else if (t == 0x23) {   /* ALLIGATOR boss (type 0x23, EM023, STAGE2) — giant ground walk-chaser +
                                 * grab-eat. SCA wall-clamp after the tick like the dog/zombie. */
            int32_t al_ox = e->x, al_oz = e->z;
            re15_alligator_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != al_ox || e->z != al_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, al_ox, al_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x24) {   /* AMBIENT FX-EMITTER (type 0x24, EM024, STAGE2) — harmless drifting swarm.
                                 * NO wall-clamp: it drives its own scripted X-drift/wrap and disables its
                                 * own collision (byte-true: not a physical obstacle). */
            re15_fx_emitter_ai_tick(s);
        }
        else if (t == 0x2b) {   /* TYRANT boss (type 0x2b, EM02B, STAGE4/5) — bipedal ground walk-chaser +
                                 * 2x -10 attacks + grab-pins. SCA wall-clamp after the tick. */
            int32_t ty_ox = e->x, ty_oz = e->z;
            re15_tyrant_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != ty_ox || e->z != ty_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, ty_ox, ty_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x2d) {   /* IVY plant-grappler (type 0x2d, EM02D, STAGE4) — DORMANT scripted prop.
                                 * The ivy root 0x801168c4 is pause-gate + state jalr ONLY: NO body-push,
                                 * NO SCA wall-clamp, NO shadow/anim-SFX tail (audit wf_efd92a2c ivy #81).
                                 * It never moves, so no collision pass is needed or byte-true. */
            re15_ivy_ai_tick(s);
        }
        /* type 0x22 (EM022, STAGE2 root 0x8010c080) is a VERIFIED STUB (wf_5c34ffe7): a scaffolded
         * state machine whose every dispatch leaf is a `jr ra` no-op — NO HP, NO clip, NO locomotion,
         * NO attack, NO player.hp write. The registered-but-unimplemented pattern (like the never-spawned
         * unregistered type slot 0x33 @0x80072c78=0). Byte-true = deliberately UNROUTED: it spawns and sits
         * inert (its only real code is the shared cutscene model-draw aux, which the port's renderer already
         * covers). */
    }
}
