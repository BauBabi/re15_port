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

static void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);          /* fwd */
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
    g_player_victim_variant = (uint8_t)((zombie->sub_state_1 >= 4) ? 1 : 0);  /* +0x5=4 behind else face */
    if (g_player_victim == 0 || g_player_victim == 3) { /* enter (or re-enter from the release finish) */
        g_player_victim = 1;
        s_victim_phase  = 0;
        player->anim_frame = 0;
        s_victim_fresh  = 1;
        /* ONE-SHOT yaw latch (byte-true phase 0, D1 disasm @0x8010a344-a3d4): a8f8(grabber pos,
         * ±0x800) snaps rot_y := bearing(Leon->zombie), face then adds 0x800 -> BOTH variants end at
         * bearing+0x800 = yaw(zombie->player) = the same value the zombie's own [0] snap writes
         * (live: pl_rot == zrot within -2..-6 in all 17 grab rows). NOT re-snapped per frame — the
         * only later yaw write is the release-exit ±0x800 fix-up (re15_player_victim_tick). */
        player->rot_y = (int16_t)(((int)re15_atan2_q12(player->z - zombie->z,
                                                       player->x - zombie->x) - 0x400) & 0x0fff);
        player->anim_frac = 0;       /* +0x8f := 0 (@0x8010a3a0) — Leon's grab-start pose is a HARD
                                      * cut; only the zombie blends (+0x8f=7 on its side) */
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
    } else {                                            /* ZOMBIE (default) */
        uint8_t base = (uint8_t)(v * 3);
        *c_intro = base; *c_hold = (uint8_t)(base + 1);
        *c_release = (uint8_t)(base + 2); *c_collapse = (uint8_t)(v + 6);
    }
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
            player->anim_frac = 7;                     /* +0x8f=7 @0x8010a594 — blend into the release */
            player->anim_blend_rate = 0x200;
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
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              player->x, player->y, player->z, (int16_t)player->rot_y);
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
             * @0x8010a648 belongs to the second clip set {8..13} — not in the port's variant model.) */
            if (g_player_victim_variant == 0)
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
                    re15_ai_set_state_word(e, ((uint32_t)(e->sub_state_1 + 2) << 8) | 1u);
                    break;
                }
                /* the escape window drains 1 + 5*mash (byte-true FUN_80037024 edge-press: any D-pad or
                 * face button = -6 that tick). Mash ~every 4 frames -> escape in ~50 ticks (< the 100-
                 * tick kill counter, breaks free ALIVE); no mash -> 110 > 100, devoured. */
                e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 5 * re15_mash_pressed()));
                if (e->ai_timer < 0) {                   /* escape window ran out -> THROW-OFF (alive) */
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
            re15_audio_room_se(7);
            re15_audio_room_se(7);            /* SE 7 plays TWICE (@0x80102920 AND @0x80102960) */
            re15_player_victim_throwoff();
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
    if (g_room_rdt_ok) {                                  /* one amortized ray sample per step */
        int32_t dx = player->x - e->x, dz = player->z - e->z;
        int32_t sx = e->x + dx * (step + 1) / 5;
        int32_t sz = e->z + dz * (step + 1) / 5;
        if (!re15_collision_on_floor(&g_room_rdt, sx, sz))
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
            e->hit_react |= 1;
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
        case 4:
            e->motion = 0x12; e->anim_frame = 0;      /* get-up clip */
            e->anim_frac = 0xf;
            e->sub_state_2 = 5;
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
static void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew)
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

    if (!(e->ai_flags & 0x100)) {
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
                else if (e->sub_state_1 == 2) {
                    /* ENGAGE animate (byte-true FUN_801021f8 @0x8011f890[2], W1 disasm 2026-07-03):
                     * THE AWARE WALK — not a stationary tracker. Plays the per-zombie BANK0 walk clip
                     * +0x1d4 in {2,3,4,5} (seeded @0x80100774 from @0x8011f7e4 — the port's hurt_clip
                     * field IS +0x1d4) with the SAME foot-lock translation as the 0x13 lurch, weaving
                     * toward the player via the 32-entry gait row (@0x8011f9f0 + variant*0x80). The
                     * old port kept the engage STANDING and did all walking in 0x13/clip 1 — clip 1 is
                     * the UNAWARE shamble (the state-1 wander uses it too) = the user-reported "walks
                     * like he hasn't noticed Leon". */
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
                            break;
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
                        re15_enemy_steer_point(e, e->steer_x, e->steer_z, sVar7);
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
                break;
            }
            case 5: case 6:   /* feeding (@0x8011f80c[5]/[6]=0x801018f8) -> the dist-gated wake-up */
                re15_enemy_ai_live_feeding(e);
                break;
            case 7: case 8:   /* lying (@0x8011f80c[7]/[8]=0x80101974). CITATION CORRECTED (audit
                               * wf_827f186d #8): 0x80101974 is NOT an empty `jr ra` — it is a DOUBLE
                               * dispatcher `lw @0x8011f9d8[+0x5*4]; jalr` then `lw @0x8011f9d4[+0x5*4];
                               * jalr`. Only decide row [0]=0x801039f4 is a stub; animate row
                               * [0]=0x80103a58 is the +0x6 wake machine, rows [5]/[6]=0x80104b38/40
                               * the lying get-up pair. The ROOM1140 lyer (+0x5=0) stays passive at
                               * stage A, matching this break; the shot-awake path now runs via the
                               * downed-HURT flinch (state-2 handler above). The full lying decide/
                               * animate sub-machine (wake rows 5/6) is OPEN — tracked in the audit. */
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
        int prone = (e->sub_state_1 == 0x12 || e->sub_state_1 == 0x13);
        switch (e->sub_state_3) {
        case 0:
            if (prone) {                                    /* FUN_80106a38 phase 0 (@0x80106a84) */
                e->sub_state_3 = 1;                         /* +0x7=1 (delay slot @0x80106a88) */
                e->motion      = (uint8_t)(37 + (re15_engine_rand8() & 1)); /* clip 37/38 @0x80106a98-9c */
                e->anim_frame  = 0;                         /* +0x95=0 @0x80106aac */
                e->anim_frac   = 4;                         /* +0x8f=4 @0x80106abc */
                e->anim_blend_rate = 0x200;                 /* f314 a3=0x200 @0x80106b2c */
                re15_enemy_death_fx(e);                     /* gore 0x80019700(0x2000,rot,pool+0x40) @0x80106ad8 */
                e->hit_react |= 1;                          /* +0x93|=1 @0x80106af4-fc */
                if ((re15_engine_rand8() & 7) == 0)         /* 1/8 @0x80106b00-04 */
                    re15_audio_room_se(6);                  /* SE(6) @0x80106b0c-10 */
            } else {                                        /* FUN_801068a0 phase 0 (@0x801068f0) */
                e->sub_state_3 = 1;                         /* +0x7=1 @0x801068f0 */
                e->motion      = 0x1e;                      /* clip 30 @0x801068fc-900 */
                e->anim_frame  = 0;                         /* +0x95=0 @0x80106910 */
                e->anim_frac   = 3;                         /* +0x8f=3 @0x8010691c-20 */
                e->anim_blend_rate = 0x400;                 /* f314 a3=0x400 @0x801069bc */
                re15_enemy_death_fx(e);                     /* gore 0x80019700(0x1500,rot,pool+0x4f4) @0x8010693c */
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
     * stagger keeps the walk clip WITHOUT a frame reset (the recoil is a 4-frame torso bend, phases
     * on +0x7), and the NORMAL exit is 0x10201 (engage, +0x6=1 = entry skipped) + an INLINE behavior
     * re-roll — only a poise BREAK (+0x1dc<0) routes to the 0x11 KNOCKDOWN. */
    if (e->sub_state_3 == 0) {                         /* phase 0 — once per HIT (+0x7 reset by damage) */
        e->hit_react  |= 0x1;                          /* +0x93 |= 1 */
        e->motion      = e->hurt_clip;                 /* +0x94 = +0x1d4 — NO +0x95 reset (walk phase
                                                        * continues; the recoil is the bend, not a clip) */
        e->speed_h     = 0x14;                         /* +0x8c = 0x14 */
        e->anim_frac   = 0;                            /* +0x8f = 0 */
        e->sub_state_3 = 1;
        re15_audio_room_se(6);                         /* FUN_800453d0(6) */
        {                                              /* poise ONCE-per-hit (@0x8011fe30[+0x5]) */
            static const int8_t stun_step[12] = { 0, -2, -2, -3, -3, -3, -3, 0, 0, 0, 0, 0 };
            e->hit_stun = (int16_t)(e->hit_stun + (e->sub_state_1 < 12 ? stun_step[e->sub_state_1] : 0));
        }
        return;
    }
    if (e->sub_state_3 < 5) {                          /* the 4-frame torso-bend cadence (phases 1/3:
                                                        * spine +-0x80/frame — bone-bend deferred; the
                                                        * HOLD duration is the byte-true 4 ticks) */
        e->sub_state_3++;
        return;
    }
    /* exit gate (@0x80105b18): poise still >= 0 -> NORMAL stagger exit 0x10201 + inline re-roll
     * (FUN_80105b7c phase 3); poise broken -> KNOCKDOWN 0x11. */
    if (e->hit_stun >= 0) {
        int slot2 = (int)(e - g_actors);
        re15_ai_set_state_word(e, 0x10201u);           /* ACTIVE / +0x5=2 / +0x6=1 (entry SKIPPED) */
        e->hit_react &= (uint8_t)~0x1u;                /* +0x93 &= 0xfe (@0x80105f9c-fac): the hurt
                                                        * RECOVERY clears the once-per-attack hit
                                                        * latch — the next shot/slash damages again
                                                        * (needed by the weapon_fire latch/recursion) */
        s_wander_mag[slot2] = (uint8_t)((re15_engine_rand8() & 0x1f) + 8);   /* +0x9e */
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
        return;
    }
    e->state       = (uint8_t)RE15_AI_STATE_ACTIVE;    /* poise BROKEN -> KNOCKDOWN (@0x80105b48-68) */
    e->sub_state_1 = 0x11;
    e->sub_state_2 = 0;
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
static int re15_enemy_clip_done(const re15_actor_t *e)
{
    re15_enemy_bank_t *bank = re15_enemy_find(e->type);
    if (!bank || (int)e->motion >= bank->anim.clip_count) return 1;
    int frames = bank->anim.clips[e->motion].frame_count;
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
        e->anim_flags &= (uint16_t)~0x04u;             /* CLEAR LOOP: the death clip 0x1f plays ONCE + holds
                                                        * its fallen last frame (render clip_override=-1), it
                                                        * must NOT loop like the feeding/idle clip did (0x04 was
                                                        * set at spawn). Complements the state-7 corpse freeze. */
        e->sub_state_3 = 1;                            /* +0x7 = 1 (@0x80107d1c) */
        re15_enemy_death_fx(e);                        /* death-start blood burst (@0x80107cf4 spawn) */
        return;
    }
    if (e->sub_state_3 == 1) {                         /* phase 1 — play clip 0x1f to its end */
        if (e->anim_frame == 7)                        /* +0x95 == 7 (@0x80107cf4): death groan SE (rng: 5 or 8) */
            re15_audio_room_se((re15_engine_rand8() & 1) ? 5 : 8);
        if (e->anim_frame == 35)                       /* +0x95 == 0x23 (@0x80107d94): frame-35 gore burst */
            re15_enemy_death_fx(e);
        if (!re15_enemy_clip_done(e)) return;          /* still playing death clip 0x1f (FSM-clock gate) */
        e->sub_state_3 = 2;
    }
    e->state = (uint8_t)RE15_AI_STATE_CORPSE;          /* phase 2 — +0x4 = 7 (@0x80107ec8): the
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
void re15_crow_flock_reset(void) { s_crow_flock = 0; s_crow_gflags = 0; }
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

/* crow screech — byte-true 0x801161e8: a vertical-distance-tiered vocalization. The original
 * layers several SE ids per tier via 0x80037edc; the crow room's SE bank holds its caws, so the
 * port plays the tier's primary caw through re15_audio_room_se (the same room-SE path the zombie
 * combat SEs use). Cited tier thresholds on +0x1ec: 1500 / 3000 / 3600. */
static void re15_crow_screech(const re15_actor_t *e)
{
    int16_t ve = e->crow_vert_err;
    if      (ve < 1500) re15_audio_room_se(1);   /* @0x80116200/08 (nearest -> screech 1) */
    else if (ve < 3000) re15_audio_room_se(3);   /* @0x80116220/28 (mid -> screech 3,4,6) */
    else if (ve < 3600) re15_audio_room_se(5);   /* @0x80116254/60 (screech 5)            */
    else                re15_audio_room_se(7);   /* @0x80116268    (far -> screech 7)      */
}

/* the byte-true flat damage the crow deals: dive/strike -4, grab -8 (the player+0x93 hit
 * gate keeps it once-per-contact; on a lethal hit broadcast the KILL bit 0x2000 to the flock). */
static void re15_crow_hit_player(re15_actor_t *e, re15_actor_t *player, int dmg)
{
    if (player->hit_react != 0) return;              /* +0x93 gate (once per contact)      */
    player->hp = (int16_t)(player->hp - dmg);        /* @0x80113b04 (-4) / @0x80113e34 (-8) */
    player->hit_react |= 1;
    if (player->hp < 0) {                            /* lethal -> KILL broadcast            */
        s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x2000);   /* @0x80113b58 */
        e->crow_hs = 1;                              /* +0x1d8 self-exempt                  */
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
        if (e->crow_pturn != 0) {                                   /* (A) pending-turn */
            e->crow_pturn = 0;
            e->rot_y = (int16_t)(e->rot_y + ((e->crow_mode & 1) ? 40 : -40));
            re15_crow_sub(e, (uint8_t)(e->crow_mode & 3));
        }
        int ring = (e->grid_id < 0x80) ? 5000 : 10000;             /* (B) ring: grid<0x80 -> 5000 */
        if (e->crow_dist < ring && e->crow_vert_err < 5400) re15_crow_sub(e, 4);
        if ((s_crow_flock & 0x1000) && e->crow_vert_err < 5400)     /* (D) flock 0x1000 -> dive */
            re15_crow_sub(e, (uint8_t)((e->crow_mode % 3) + 1));
        return;
    }
    case 5:   /* DIVE-END 0x80112bac: climbed >3600 above perch -> resume */
        if (e->y < e->crow_perch_h - 3600) re15_crow_sub(e, 9);
        return;
    case 7: case 8:   /* 0x80112e4c / 0x801130fc: floor-ref (+0x1ba) != perch (+0x1ea) -> sub 5.
                       * faithful-line: still airborne (y well above the perch/ground) -> abort. */
        if (e->y < e->crow_perch_h - 400) re15_crow_sub(e, 5);
        return;
    case 9: {   /* 0x8011325c: settle re-arm, then ATTACK-COMMIT */
        if (e->crow_pturn != 0) {
            e->crow_pturn = 0;
            if ((e->crow_mode & 0xf) != 0) re15_crow_sub(e, 9);
            else if (e->sub_state_1 != 6)  re15_crow_sub(e, 6);
        }
        if (e->crow_parity != 0 && e->crow_armed != 0 &&
            e->crow_dist < 10000 && e->crow_atk_ctr < 3) {          /* attack-commit */
            re15_crow_sub(e, (e->grid_id & 0x80) ? 10 : 12);        /* downed->cruise, else grapple */
        }
        return;
    }
    case 10:   /* 0x801134f8 */
        if (e->crow_armed == 0)          { re15_crow_sub(e, 5); return; }
        if (e->crow_dist < 2500)         { re15_crow_sub(e, 14); return; }
        if (e->crow_vert_err >= 5401)    { re15_crow_sub(e, 5); e->crow_atk_ctr = 4; }
        return;
    case 11: {  /* 0x8011376c */
        int32_t adx = player->x - e->x, adz = player->z - e->z, ady = player->y - e->y;   /* AABB box */
        if (adx < 0) adx = -adx; if (adz < 0) adz = -adz; if (ady < 0) ady = -ady;
        if (adx < 0xf00 && adz < 0xf00 && ady < 0x300) { re15_crow_sub(e, 14); return; }  /* 0x8001b9b4 */
        if (e->crow_vert_err >= 5401)    { re15_crow_sub(e, 5); e->crow_atk_ctr = 4; }
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
        if (e->crow_timer == 0)  e->crow_pturn++;          /* completion flag */
        else                     e->crow_timer--;
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
        re15_crow_advance(e);
        re15_crow_anim(e);
        return;

    /* --- sub 6: CRUISE / oracle-driven climb-descend (altitude + yaw only) --- */
    case 6:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 6); e->crow_timer = (uint8_t)(e->crow_mode & 0x32); re15_audio_room_se(0); e->sub_state_2 = 1; }  /* Se(0) @0x80112d78 */
        if (e->crow_timer == 0) { re15_crow_sub(e, 9); return; }
        e->crow_timer--;
        re15_enemy_steer_point(e, player->x, player->z, 0x32);
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) * re15_crow_height_dir(e));
        e->y += e->crow_vvel;
        return;

    /* --- sub 7: CRASH-DIVE / BOUNCE --- */
    case 7:
        if (e->sub_state_2 == 0) { e->crow_speed = 140; e->crow_vvel = 0; re15_crow_clip(e, 5); e->crow_timer = 21; e->sub_state_2 = 1; }
        else if (e->sub_state_2 == 1) {
            if (e->crow_timer == 0) e->sub_state_2 = 2; else e->crow_timer--;
            e->crow_speed = (int16_t)(e->crow_speed + 3);
            e->crow_vvel  = (int16_t)(e->crow_vvel + 8);
            e->y += e->crow_vvel; re15_crow_advance(e); re15_crow_anim(e);
        } else if (e->sub_state_2 == 2) {
            e->crow_vvel = (int16_t)(e->crow_vvel + 3);
            e->y += e->crow_vvel;
            if (e->y > e->crow_perch_h - 750) { e->sub_state_2 = 3; e->crow_vvel = 0; e->crow_timer = 12; }
            re15_crow_weave(e); re15_crow_advance(e);
        } else {   /* step 3: bounce */
            e->crow_speed = (int16_t)(e->crow_speed + 9);
            e->crow_vvel  = (int16_t)(e->crow_vvel - 9);
            e->y += e->crow_vvel;
            if (e->crow_timer == 0) re15_crow_sub(e, 8); else e->crow_timer--;
            re15_crow_advance(e);
        }
        return;

    /* --- sub 8: DESCEND-AND-LAND (clamp to floor-400, anim end -> sub 0) --- */
    case 8:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 7); e->crow_vvel = 0; e->sub_state_2 = 1; return; }
        e->crow_vvel = (int16_t)(e->crow_vvel + 10);
        e->y += e->crow_vvel;
        if (!(e->y < e->crow_perch_h - 400)) e->y = e->crow_perch_h - 400;   /* land clamp */
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
                e->crow_accel = -20;
                re15_audio_room_se(4);                       /* Se(4) hit @0x80113ae8 */
                re15_crow_hit_player(e, player, 4);          /* DIVE HIT: -4 HP @0x80113b04 */
                re15_crow_screech(e);                        /* 0x801161e8 @0x80113b7c */
            }
        }
        e->y += e->crow_vvel;
        re15_crow_advance(e);
        return;

    /* --- sub 12: GRAPPLE STRIKE (contact -> grab, -8 HP) --- */
    case 12:
        if (e->sub_state_2 == 0) {
            if (e->crow_mode & 1) { re15_crow_clip(e, 3); e->crow_speed = 160; }
            else                  { re15_crow_clip(e, 4); e->crow_speed = 180; }
            e->crow_vvel = (int16_t)(e->crow_mode & 0x3f);
            if (e->crow_vert_err < 2000) e->crow_vvel = (int16_t)(-e->crow_vvel);
            e->crow_timer = 30; e->sub_state_2 = 1;
            return;
        }
        re15_crow_anim(e);
        e->y += e->crow_vvel;
        re15_enemy_steer_point(e, player->x, player->z, 0x32);
        re15_crow_advance(e);
        if (e->crow_contact) {                               /* +0x1d0 contact -> GRAB (@0x80113dd4) */
            e->crow_hs = 1;                                  /* +0x1d8=1 self-exempt @0x80113ddc */
            s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x8000);   /* @0x80113dfc */
            re15_crow_hit_player(e, player, 8);              /* GRAB: -8 HP @0x80113e34 */
            e->crow_struggle = 100;
            re15_crow_sub(e, 13);
        } else if (e->crow_timer == 0) { re15_crow_sub(e, 5); } else e->crow_timer--;
        return;

    /* --- sub 13: GRAB-HOLD / FEEDING (peck, struggle drain, release -> sub 14) --- */
    case 13:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 8); e->crow_atk_ctr++; e->crow_struggle = 100; re15_audio_room_se(2); e->crow_timer = 30; e->sub_state_2 = 1; }  /* Se(2) peck @0x80113f74 */
        re15_crow_anim(e);
        if (e->crow_timer == 0) { e->crow_timer = 30; re15_audio_room_se(2); }  /* re-peck every 30f @0x8011400c */
        else e->crow_timer--;
        if ((e->crow_timer % 10) == 0) re15_audio_room_se(0);   /* chirp every 10f @0x80114060 */
        e->crow_struggle = (int16_t)(e->crow_struggle - (1 + (re15_engine_rand8() % 3) * 3));
        if (e->crow_struggle < 0) {                          /* RELEASE */
            s_crow_flock = (uint16_t)((s_crow_flock & 0xfff) | 0x4000);
            e->crow_hs = 1;
            re15_crow_sub(e, 14);
        }
        return;

    /* --- sub 14: BANK / TURN-AROUND (regroup peel-off) --- */
    case 14:
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 4); re15_audio_room_se(0); e->crow_speed = 180; e->crow_timer = 32;  /* Se(0) screech @0x80114188 */
            e->crow_yawrate = (e->crow_mode & 0x80) ? -60 : 60;
            e->sub_state_2 = 1;
        } else if (e->sub_state_2 == 1) {
            e->rot_y = (int16_t)(e->rot_y + e->crow_yawrate);
            if (e->crow_timer != 0) e->crow_timer--; else { e->sub_state_2 = 2; e->crow_timer = 90; }
        } else {
            if (e->crow_timer != 0) e->crow_timer--; else re15_crow_sub(e, 9);
        }
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) * re15_crow_height_dir(e));
        e->y += e->crow_vvel;
        re15_crow_advance(e);
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
        if (e->crow_contact) { re15_crow_sub(e, 16); return; }      /* +0x1d0 contact -> STRIKE */
        re15_crow_advance(e);
        return;

    /* --- sub 16: STRIKE / PECK (-4 HP) --- */
    case 16:
        if (e->sub_state_2 == 0) {
            re15_crow_clip(e, 8);
            re15_crow_screech(e);                            /* 0x801161e8 @0x801144bc */
            re15_crow_hit_player(e, player, 4);              /* STRIKE: -4 HP @0x801144f0 */
            e->sub_state_2 = 1;
        }
        re15_crow_anim(e);
        return;

    /* --- sub 17: FAST PLUNGE-TO-GROUND --- */
    case 17:
        if (e->sub_state_2 == 0) { re15_crow_clip(e, 3); e->crow_speed = 100; e->sub_state_2 = 1; return; }
        re15_enemy_steer_point(e, player->x, player->z, 0x64);
        e->crow_vvel = (int16_t)((e->crow_mode & 0x3f) + 48);
        if (e->y < e->crow_perch_h - 750) e->y += e->crow_vvel;
        re15_crow_advance(e);
        re15_crow_anim(e);
        if (e->crow_dist < 1000 && !(e->y < e->crow_perch_h - 750)) re15_crow_sub(e, 8);
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
    if (e->sub_state_1 == 7) {   /* GIB lane 0x801149c4: 13 feather children (@0x80114a50) -> corpse */
        if (e->sub_state_2 == 0) {
            re15_esp_fx_splatter(re15_esp_room_bank(), 0, 13, e->x, e->y, e->z, e->crow_perch_h);
            e->hp = -1; e->crow_timer = 0x32; e->sub_state_2 = 1;    /* +0x1d5=0x32 */
        } else if (e->crow_timer == 0) { e->state = 7; re15_crow_sub(e, 0); }  /* @0x80114b90 +0x4=7 */
        else e->crow_timer--;
        return;
    }
    switch (e->sub_state_3) {     /* normal death, nested on +0x7 (step-router 0x80114738) */
    case 0:   /* INIT 0x80114784 */
        re15_audio_room_se(3);                                       /* Se(3) @0x80114784 */
        e->hp = -1; e->crow_vvel = 0; e->crow_grav = 0x26;           /* +0x9a=-1, +0x1e4=0, +0x1e8=0x26 */
        e->crow_speed = 0; e->rot_z = 0; e->sub_state_3 = 1;         /* +0x8c=0, +0x6c=0 */
        break;
    case 1: { /* FALL 0x80114828 */
        e->rot_z = (int16_t)(e->rot_z + 140); if (e->rot_z > 1024) e->rot_z = 1024;  /* +0x6c spin, clamp */
        e->crow_speed = 60;                                          /* +0x8c=60 */
        e->crow_vvel = (int16_t)(e->crow_vvel + e->crow_grav);       /* gravity @0x80114894 */
        e->y += e->crow_vvel; re15_crow_advance(e);                  /* integrate @0x801148b4 */
        if (e->y >= e->crow_perch_h - 400) {                         /* land: floor +0x1ba-400 (floor=perch, faithful-line) */
            e->y = e->crow_perch_h - 400;
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
    e->crow_mode = (uint8_t)((s_crow_gflags & 0x1u) ? 1 : 0);   /* +0x1d4 = testbit(0x800b1028, 0x1f) */
    /* DEATH promotion (@0x80112050-8c): a state-4 grid&0x40 crow dies when the scripted death bit
     * 0x1f is set (STAGE3/5 only — byte-true unreachable in STAGE1). */
    if (e->crow_mode && e->state == 4 && (e->grid_id & 0x40)) {
        e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4=3, 0x80115d74(0) */
        s_crow_gflags &= ~0x1u;                                                     /* clearbit 0x1f @0x80112098 */
    }

    switch (e->state) {
    case 0: {  /* INIT — FUN_80111a4c: count-flock, capture perch, lift off, enter ACTIVE */
        int n = 0;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x21) n++;
        s_crow_flock    = (uint16_t)(n << 4);        /* 0x800aca50 = count<<4 (@FUN_80111a4c) */
        e->crow_perch_h = (int16_t)e->y;             /* +0x1ea = spawn Y                      */
        e->y           -= 400;                       /* +0x38 -= 400 lift-off                 */
        e->crow_mode    = 0;                          /* +0x1d4 = testbit(0x800b1028,0x1f) — byte-true 0
                                                       * in STAGE1: bit 0x1f is set only by STAGE3/
                                                       * STAGE5 handlers, never STAGE1 (root @0x80112048) */
        e->crow_vvel = 0; e->crow_speed = 0; e->crow_atk_ctr = 0; e->crow_diveflag = 0;
        e->crow_armed   = 1;                          /* +0x1db = 1 — byte-true (FUN_80111a4c: grid&0x10
                                                       * -> 0, else -> 1; the ROOM10C0 crows have grid=0) */
        re15_crow_clip(e, 0);
        e->anim_flags   = 0x04;
        e->state        = 1;                          /* +0x4 = 1 ACTIVE (@0x80112388)        */
        if (e->grid_id & 0x40) e->state = 4;          /* byte-true override (@0x80112408): the
                                                       * grid&0x40 event crow (0xe1) enters FLIGHT-2 */
        e->sub_state_1  = 0; e->sub_state_2 = 0;      /* INIT leaves +0x5/+0x6 = 0 (patrol/ascend) */
        break;
    }

    case 1: {  /* ACTIVE (0x80112420): sense -> flock-dispatch -> steer -> move -> post-pass */
        e->crow_parity = (uint8_t)(e->crow_parity ^ 1);              /* +0x1d2 = 0x8001bc08 & 1
                                                                      * (the frame-parity/blink toggle
                                                                      * @0x8011243c; NOT the ground
                                                                      * floor-LOS — a crow is a flyer) */
        e->crow_dist     = (int16_t)re15_enemy_player_dist(e, player);  /* +0x1dc SquareRoot0 */
        e->crow_vert_err = (int16_t)(player->y - e->y);                 /* +0x1ec (@0x801124cc) */
        if (s_crow_flock & 0xff00) re15_crow_flock_dispatch(e);        /* 0x80116068 */
        re15_crow_steer(e, player);                                    /* steer[+0x5] */
        re15_crow_move(e, player);                                     /* move[+0x5]  */
        /* ROOT post-pass body-push (aec4 @0x801121d4): crow pushed out of the player;
         * +0x1d0 = contact (the strike/grab connect the handlers read next tick). */
        e->crow_contact = (uint8_t)(re15_body_push(player, RE15_BODY_R_PLAYER, e,
                                                   (int32_t)e->hit_radius_min) ? 1 : 0);
        /* GRAB-HOLD pins the player (byte-true move[12] @0x80113e48: 0x800aca58 = cmd 5 grabbed).
         * s_player_grabbed (cleared at the top of run_all) latches the pin for sub 13 = game_step
         * skips re15_player_tick, exactly like the zombie grab. The crow releases via its own
         * struggle drain (move[13]), not player mash — so the pin lifts when it leaves sub 13. */
        if (e->sub_state_1 == 13) s_player_grabbed = 1;
        break;
    }

    case 4: {  /* FLIGHT-2 (0x80114e54): preamble -> 0x1d-tick -> substate[+0x5] (the event crow) */
        e->crow_dist     = (int16_t)re15_enemy_player_dist(e, player);  /* +0x1dc (@0x80114f1c) */
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
 * 0x8010d7f8 family (RE15_DOG_AI.md; workflow wf_ccc60f69, adversarially verified).
 * Root dispatches +0x4 via @0x80120f74: [0]INIT [1]ACTIVE(brain) [2]HURT [3]DEATH
 * [7]CORPSE. ACTIVE is a dual-dispatch on +0x5 (decision @0x80120f94 / act @0x80120fd4):
 * sub 0 idle / 1 turn / 2 chase / 3 attack-range / 5 arm-grab / 6 snap / 7 low-HP lunge /
 * 4 pounce-lunge / 8 bite / 9-10 eaten-GRAB / 0xb died-in-grab / 0xc release / 13-14 reroute.
 * The dog shares the zombie steering/collision + take_damage (states 2/3/7 = receiving side).
 * COMPLETE byte-true: INIT + the full ACTIVE brain (all attack + eaten-GRAB + obstacle-reroute
 * sub-states) + HURT/DEATH/CORPSE + the state-4/5/6 pounce-kill machines A/B. Zero faithful-line
 * on the dog side; the player-eaten anim reuses the shared victim FSM (same as the zombie grab). */
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

/* ============================ DOG OBSTACLE-REROUTE (ACTIVE sub 13/14) ======================== *
 * Byte-true 0x801102dc (sub 13, leap-over) + 0x801105bc (sub 14, turn-around). Entered from the
 * CHASE decision when the dog is blocked: +0x5 = 0xe - (+0x90 & 1) -> sub 13 on wall contact, sub 14
 * otherwise (@0x8010e2b4/d4). Both read +0x90 (ai_contact) — the SCA resolver's wall byte, whose HIGH
 * nibble is the escape heading = (+0x90 & 0xf0) << 4 (16 directions) — turn to it, run forward over the
 * obstacle, return to CHASE (+0x5=2). The +0x90 low nibble/bit0 is cleared each frame by the resolver
 * (@0x8003b1dc) and set from the port's collision clamp (run_all, byte-true-equivalent: the same wall
 * normal the resolver encodes). The B[13]/B[14] pre-decision slots are empty jr-ra stubs. */
static void re15_dog_reroute13(re15_actor_t *e)   /* 5-step @0x8010024c */
{
    int heading = (((int)e->ai_contact & 0xf0) << 4) & 0xfff;   /* +0x90 hi-nibble -> escape yaw */
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
        if (re15_dog_anim(e)) {                         /* run clip 9 done @0x8011047c */
            e->crow_speed = 0xc1c;                      /* +0x8c = 3100 forward @0x801104c8 */
            re15_dog_advance(e, e->crow_speed);         /* pos_advance @0x801104d4 (the far side) */
            e->floor++;                                 /* +0x82 reroute counter @0x8011049c */
            e->sub_state_2 = 4;                         /* -> step 4 @0x80110534 */
        }
        /* (byte-true also raises/drops +0x38/+0xb0/+0x1ba for the vertical hop; the port dog is
         * flat-ground — the horizontal leap over the obstacle is the observable behaviour) */
        break;
    default:                                            /* RESET -> CHASE 0x80110538 (step 4) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;     /* +0x94=0,+0x95=0,+0x8f=0 */
        re15_dog_anim(e);                               /* anim_set @0x8011057c */
        e->sub_state_1 = 2; e->sub_state_2 = 0;         /* +0x5=2 CHASE, +0x6=0 @0x80110590 */
        e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);/* consume the contact bit */
        break;
    }
}

static void re15_dog_reroute14(re15_actor_t *e)   /* inline +0x6 FSM @0x801105bc, clip 0x0a */
{
    int heading = (((int)e->ai_contact & 0xf0) << 4) & 0xfff;   /* target from latched wall byte (+0x9f) */
    switch (e->sub_state_2) {
    case 0:                                             /* INIT 0x80110610 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 7;     /* +0x94=0,+0x95=0,+0x8f=7 */
        e->sub_state_2 = 1;
        /* +0x9f = +0x90 latch @0x80110658 (the escape heading is preserved in ai_contact hi-nibble) */
        /* FALLTHROUGH to phase 1 (byte-true @0x80110658 -> 0x8011065c) */
        /* fall through */
    case 1: {                                           /* TURN-SLEW 0x8011065c */
        re15_dog_anim(e);                               /* anim_set clip 0 @0x80110674 */
        int delta = (16 + heading - (int)e->rot_y) & 0xfff;     /* 0x8001aa68(target, step 16) */
        int inc   = (delta < 32) ? 0 : (delta < 0x801 ? 16 : -16);
        e->rot_y  = (uint16_t)(((int)e->rot_y + inc) & 0xfff);  /* +0x6a += inc @0x801106b8 */
        if (inc != 0) break;                            /* still turning @0x801106b4 */
        e->sub_state_2 = 2;                             /* aligned -> phase 2 @0x801106cc */
        break;
    }
    case 2:                                             /* SNAP + reroute clip 0x0a 0x801106d0 */
        e->motion = 0x0a; e->anim_frame = 0; e->anim_frac = 0;  /* +0x94=0x0a,+0x95=0,+0x8f=0 */
        e->rot_y  = (uint16_t)heading;                  /* hard-snap +0x6a @0x80110714 */
        e->sub_state_2 = 3;
        /* FALLTHROUGH to phase 3 (byte-true @0x80110724 -> 0x80110728) */
        /* fall through */
    default:                                            /* RUN + settle -> CHASE 0x80110728 (phase 3) */
        /* byte-true this is a scripted parabola driven by anim_frame milestones 0x0a/0x0e/0x13/0x14
         * (@0x80110734+): forward at +0x8c=0x509 then 0x12c with a +0x38/+0x1ba/+0x9c vertical ease.
         * The port dog is flat-ground — the observable is the forward run over the obstacle: */
        if (e->anim_frame >= 0x0a) { e->crow_speed = 0x12c; re15_dog_advance(e, e->crow_speed); }  /* +0x8c=300 @0x80110858 */
        if (re15_dog_anim(e)) {                         /* reroute clip 0x0a done @0x801108a8 */
            e->sub_state_1 = 2; e->sub_state_2 = 0;     /* +0x5=2 CHASE, +0x6=0 @0x801108bc */
            e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);
        }
        break;
    }
}

/* ============================ DOG eaten-GRAB hold (ACTIVE sub 9 / sub 10) ==================== *
 * Byte-true 7-step FSM @0x8010020c (sub 9 GRAB) / @0x8010022c (sub 10 GRABVAR). The dog latches onto
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

/* STATE 4/5/6 (0x80111350) — the POUNCE-LAND + grid-0x40 player-kill cutscenes. Sub-dispatch on +0x5
 * (table @0x801210c8): [0/1] pounce-land (grid-0x43/0x42 gated), [4/5] kill machine A (0x80111984),
 * [10/11] kill machine B (0x80111cf0). Reached from the ACTIVE lunge (sub 4 -> state 5 when the dog's
 * hp>=0). Byte-true pounce-land for a grid-0x43 dog; a non-special dog routes safely back to chase. */
static void re15_dog_state456(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {
    case 0: case 1:   /* POUNCE-LAND 0x80111398: leap arc (rise) -> land -> chase (grid-0x43/0x42 gated) */
        if (e->sub_state_2 == 0) {
            if (e->grid_id != 0x43 && e->grid_id != 0x42) { e->state = 1; re15_dog_sub(e, 2); break; }  /* @0x801113e8 gate: normal dog -> chase (safe) */
            e->hit_react |= 3; re15_dog_clip(e, 0x14); e->crow_speed = 0xf0; re15_audio_room_se(2);  /* +0x93|=3, clip 0x14, +0x8c=0xf0 @0x801113d8 */
            e->sub_state_2 = 1;
        } else if (e->sub_state_2 == 1) {                 /* LEAP: rise + lunge forward @0x80111458 */
            if (e->anim_frame >= 0x0d) { e->crow_speed = (int16_t)(e->crow_speed + 6); re15_dog_advance(e, e->crow_speed); e->y -= 20; }  /* +0x38-=20 rise @0x801114dc */
            if (re15_dog_anim(e)) { re15_dog_clip(e, 0x15); e->crow_timer = 0; e->sub_state_2 = 2; }   /* land pose clip 0x15 @0x801114f4 */
        } else {                                          /* LAND -> back to chase @0x801115e0 */
            e->y = 0;                                     /* +0x38 = +0x1ba floor (dog ground = 0) */
            e->dog_atk_cd = 0x78; e->hit_react = 0;       /* +0x1d6=0x78, +0x93=0 */
            e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0;  /* +0x4=0x201 -> state 1 / sub 2 CHASE @0x8011162c */
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
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              pl->x, pl->y, pl->z, (int16_t)pl->rot_y);   /* eaten blood burst */
            e->sub_state_2 = 1;
        } else if (e->sub_state_2 == 1 && s_victim_phase >= 2) {   /* eaten past the shake phases -> KILL */
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              pl->x, pl->y, pl->z, (int16_t)pl->rot_y);   /* gore burst @kill */
            re15_player_victim_devour(e);                 /* collapse -> player STATE 7 (aca58=7 @0x80111ea0) */
            e->sub_state_2 = 2;
        }
        re15_dog_anim(e);                                 /* the dog holds its pounce-land pose */
        break;

    default:
        e->state = 1; re15_dog_sub(e, 2);   /* unhandled -> chase */
        break;
    }
}

static void re15_dog_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x8010d93c: idle pose, seed steer-target, HP, -> ACTIVE */
        re15_dog_clip(e, 1);                                  /* +0x94=1 idle clip @0x8010d960 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be @0x8010d9a8 */
        e->ai_timer = 0; e->dog_atk_cd = 0; e->dog_pounce_cd = 0; e->dog_flags = 0;
        {   /* BYTE-TRUE HP (dog INIT @0x8010dae8-db0c): +0x9a = HPtable[type*0x20 + (rng&0xf)*2], the
             * SAME shared table @0x8011f034 as the zombie/maggot. Dog row @0x8011f434 (type 0x20). */
            static const uint16_t dog_hp[16] =
                { 65, 85, 85, 69,105, 87, 73, 87,107, 77,111, 80, 93, 83, 97,103 };
            if (e->hp <= 0) e->hp = (int16_t)dog_hp[re15_engine_rand8() & 0xf];
        }
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4=1 @0x8010d950 */
        break;

    case 1: {  /* ACTIVE 0x8010dbcc: perception -> decision[+0x5] -> act[+0x5] */
        int los = re15_enemy_los_probe(slot, e, pl);          /* 0x8001bc08 @0x8010dbd4 */
        if (los != 2) { if (los) e->dog_flags |= 1; else e->dog_flags = (uint16_t)(e->dog_flags & ~1u); }  /* +0x1d0 bit0 sticky LOS */
        e->dog_dist = (int16_t)re15_enemy_player_dist(e, pl); /* +0x1d4 @0x8010dc8c */
        if (e->dog_atk_cd)    e->dog_atk_cd--;                /* +0x1d6 attack-cooldown @0x8010dd6c */
        if (e->dog_pounce_cd) e->dog_pounce_cd--;             /* +0x1e6 pounce-cooldown */

        switch (e->sub_state_1) {
        case 0:   /* IDLE (dec 0x8010ddb8 / act 0x8010dea0) */
            if (e->dog_dist < 4000 && (e->dog_flags & 1)) re15_dog_sub(e, 1);   /* player close+LOS -> leave idle @0x8010ddcc */
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 300); re15_dog_clip(e, 1); e->sub_state_2 = 1; }  /* +0x9c=rng+300 @0x8010dec0 */
            else if (e->ai_timer == 0) re15_dog_sub(e, 1);   /* idle timeout -> leave idle @0x8010df34 */
            else e->ai_timer--;
            re15_dog_anim(e);
            break;

        case 1:   /* TURN/REORIENT (act 0x8010df9c) */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 2); e->sub_state_2 = 1; }   /* clip 2 @0x8010df9c */
            if (re15_dog_anim(e)) re15_dog_sub(e, 2);         /* clip done -> CHASE @0x8010e06c */
            break;

        case 2:   /* CHASE / APPROACH (dec 0x8010e0c4 / act 0x8010e304) */
            if (pl->hit_react == 0 && re15_dog_arc(e, pl, 2500, 192)) { re15_dog_sub(e, 3); break; }  /* bite cone @0x8010e0e0 -> ATTACK-RANGE */
            if (e->ai_contact & 1) { re15_dog_sub(e, (uint8_t)(0x0e - (e->ai_contact & 1))); break; }  /* blocked by a wall -> REROUTE (sub 13 leap / 14 turn) @0x8010e2b4/d4 */
            /* POUNCE gate (@0x8010e194): cd==0 && player.hp>=81 && dist>=7001 && LOS && 1/4 -> sub 4 LUNGE */
            if (e->dog_pounce_cd == 0 && pl->hp >= 81 && e->dog_dist >= 7001 && (e->dog_flags & 1)
                && (re15_engine_rand8() & 1) && (re15_engine_rand8() & 1)) { re15_dog_sub(e, 4); break; }
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 0); e->sub_state_2 = 1; }   /* walk clip 0 @0x8010e324 */
            re15_enemy_steer_point(e, pl->x, pl->z, 12);      /* yaw-slew toward player @0x8010e468 */
            re15_dog_advance(e, 8);                           /* pos_advance speed 8 @0x8010e548 */
            if (e->anim_frame == 0 || e->anim_frame == 0x1e) re15_audio_room_se(6);  /* footstep Se(6) @0x8010e4f8 */
            re15_dog_anim(e);
            break;

        case 3:   /* ATTACK-RANGE / MENACE (dec 0x8010e568 / act 0x8010e6d4) */
            if (e->dog_atk_cd == 0 && re15_dog_arc(e, pl, 3000, 384)) {   /* in commit cone @0x8010e590 -> pick the attack */
                uint8_t pick = 8;                             /* default BITE @0x8010e5ac */
                if (pl->hit_react != 0) pick = 6;             /* player already reacting -> quick SNAP @0x8010e5e0 */
                if (pl->hp < 50) {                            /* weak player -> ARM the eaten grab @0x8010e608 */
                    pick = 5;
                    if (re15_engine_rand8() & 1) pick = 7;    /* + coin flip -> low-HP homing LUNGE @0x8010e624 */
                }
                re15_dog_sub(e, pick); break;
            }
            if (e->dog_dist >= 5000) { re15_dog_sub(e, 2); break; }   /* fell out of range -> CHASE */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 3); e->sub_state_2 = 1; }   /* growl clip 3 @0x8010e6d4 */
            re15_dog_anim(e);
            break;

        case 8:   /* BITE 0x8010f15c (clip 0x14): lunge forward + the -10 HP bite on connect */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 0x14); re15_audio_room_se(5); e->dog_atk_cd = 0x78; e->crow_speed = 0; e->sub_state_2 = 1; }  /* Se(5) @0x8010f1fc; crow_speed = the +0x8c field */
            if (e->anim_frame >= 13) {                        /* connect window @0x8010f254 (frame>=0xd) */
                e->crow_speed = (int16_t)(e->crow_speed + 6);
                re15_dog_advance(e, e->crow_speed);           /* lunge forward @0x8010f26c */
                if (pl->hit_react == 0 && re15_dog_arc(e, pl, 2000, 384)) {   /* +0x93==0 @0x8010f294 & cone 2000/384 @0x8010f2a8 */
                    pl->hp = (int16_t)(pl->hp - 10);          /* BITE: player.hp -= 10 @0x8010f2d0 */
                    pl->hit_react |= 1;                       /* gate re-hit this bite */
                    /* ESCALATE to the eaten GRAB (@0x8010f458): if the dog pre-armed (sub 5 windup) OR
                     * the bite is LETHAL (hp-10 < 0) -> +0x5 = facing_aligned + 9 (sub 9 front / 10 behind).
                     * So a lethal bite always eats you; a non-lethal bite only when the dog armed at hp<50. */
                    if (e->dog_grab_armed != 0 || pl->hp < 0) {
                        int facing = ((((int)pl->rot_y - (int)e->rot_y) + 0x400) & 0xfff) < 0x800 ? 1 : 0;  /* 0x8001a780 half-plane */
                        re15_dog_sub(e, (uint8_t)(9 + facing));   /* +0x5=9/10, +0x6=0 @0x8010f47c */
                        e->dog_grab_armed = 0;                    /* clear +0x1e4 @0x8010f4b4 */
                        break;
                    }
                }
            }
            if (re15_dog_anim(e)) re15_dog_sub(e, 2);         /* clip done -> back to CHASE */
            break;

        case 4:   /* LUNGE / POUNCE (0x8010ea44, inner-jt @0x801001ac): windup -> leap -> land.
                   * Byte-true windup (clip 0xb @0x8010ea9c, bark Se(2) @0x8010eb40, aim). The byte-true
                   * land writes +0x4=5 -> state 5 (0x80111350 pounce-land, grid-0x43 gated): a special-
                   * grid dog leaps + pins for the pounce-kill (machine A/B), a normal dog routes safely
                   * back to chase (re15_dog_state456). Shot mid-leap (hp<0) -> DEATH. */
            if (e->sub_state_2 == 0) {
                re15_dog_clip(e, 0x0b); e->dog_pounce_cd = 0x78;
                re15_enemy_steer_point(e, pl->x, pl->z, 0x20);   /* aim at the player before the leap */
                re15_audio_room_se(2);                           /* bark Se(2) @0x8010eb40 */
                e->sub_state_2 = 1;
            }
            re15_dog_advance(e, 40);                             /* leap forward */
            if (re15_dog_anim(e)) {                              /* step4 transition @0x8010eb88 (on the dog's hp) */
                if (e->hp >= 0) { e->state = 5; e->sub_state_1 = 0; e->sub_state_2 = 0; }  /* -> state 5 POUNCE-LAND @0x8010eb8c */
                else { e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 1; }  /* shot mid-leap -> DEATH @0x8010eb90 */
            }
            break;

        case 5:   /* WINDUP / arm-grab 0x8010ecb4 (inner @0x801001cc, clips 0x0f->0x10->0x11) */
            switch (e->sub_state_2) {
            case 0: re15_dog_clip(e, 0x0f); e->dog_grab_armed = 1; e->sub_state_2 = 1; break;  /* clip 0x0f + ARM +0x1e4=1 @0x8010ed54 */
            case 2: re15_dog_clip(e, 0x10); re15_audio_room_se(8); e->sub_state_2 = 3; break;  /* clip 0x10 + Se(8) @0x8010edb0 */
            case 4: re15_dog_clip(e, 0x11); e->sub_state_2 = 5; break;                          /* clip 0x11 @0x8010edc0 */
            case 6: re15_dog_sub(e, 3); break;                                                  /* exit -> ATTACK-RANGE @0x8010ee64 */
            default: if (re15_dog_anim(e)) e->sub_state_2++; break;                             /* shared wait @0x8010ee1c */
            }
            break;

        case 6:   /* SNAP (player-reacting) 0x8010ee90 (clip 0x12): a single quick growl-bite */
            if (e->sub_state_2 == 0) { e->motion = 0x12; e->anim_frame = 0; e->anim_frac = 0; e->sub_state_2 = 1; }  /* clip 0x12, no blend @0x8010ef1c */
            else if (e->sub_state_2 == 1) {
                if (e->anim_frame == 0x0a || e->anim_frame == 0x19 || e->anim_frame == 0x2d) re15_audio_room_se(0);  /* growl Se(0) @0x8010ef4c */
                if (re15_dog_anim(e)) e->sub_state_2 = 2;
            } else re15_dog_sub(e, 2);                        /* exit -> CHASE @0x8010ef90 (+0x5=2) */
            break;

        case 7:   /* LOW-HP LUNGE 0x8010efbc (clip 0x13): homing lunge with a <13-frame bite window */
            if (e->anim_frame == 0 || e->anim_frame == 0x1b) re15_audio_room_se(6);   /* Se(6) @0x8010efe0 */
            if (e->sub_state_2 == 0) { re15_dog_clip(e, 0x13); e->crow_speed = 0x14; e->sub_state_2 = 1; }  /* clip 0x13, +0x8c=0x14 @0x8010f070 */
            else if (e->sub_state_2 == 1) {
                e->dog_yawrate = (int16_t)((re15_engine_rand8() & 0xf) + 1);   /* +0x1e2 yaw jitter @0x8010f090 */
                re15_enemy_steer_point(e, pl->x, pl->z, e->dog_yawrate);       /* homing yaw-slew @0x8010f0ac */
                re15_dog_advance(e, 0x800);                                     /* pos_advance forward @0x8010f120 */
                if (re15_dog_anim(e)) e->sub_state_2 = 2;
            } else re15_dog_sub(e, 2);                        /* exit -> CHASE @0x8010f130 */
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

        case 13:  /* OBSTACLE-REROUTE (leap-over, wall contact) -> CHASE */
            re15_dog_reroute13(e);
            break;
        case 14:  /* OBSTACLE-REROUTE (turn-around) -> CHASE */
            re15_dog_reroute14(e);
            break;

        default:  /* only sub-states 0-14 exist -> unhandled falls back to chase */
            re15_dog_sub(e, 2);
            break;
        }
        break;
    }

    case 4: case 5: case 6:   /* POUNCE-LAND + grid-0x40 player-kill cutscenes (0x80111350) */
        re15_dog_state456(e, pl);
        break;

    case 2:   /* HURT 0x801108f0: flinch clip 6 -> recover (state 1) / death (state 3) */
        if (e->sub_state_3 == 0) { re15_dog_clip(e, 6); re15_audio_room_se(1); e->sub_state_3 = 1; }  /* clip 6 + Se(1) @0x80110a3c */
        else if (e->sub_state_3 == 1) { if (re15_dog_anim(e)) e->sub_state_3 = 2; }
        else {
            if (e->hp >= 0) { e->state = 1; re15_dog_sub(e, 2); e->sub_state_3 = 0; e->hit_react = 0; }  /* recover @0x80110b48 */
            else { e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 1; }           /* death @0x80110b10 */
        }
        break;

    case 3:   /* DEATH 0x80110dc0: death clip 14 + yelp -> state 7 */
        if (e->sub_state_3 <= 1) { re15_dog_clip(e, 0x0e); e->hit_react |= 2; e->sub_state_3 = 2; }   /* clip 14 @0x80111214 */
        else if (e->sub_state_3 == 2) { if (re15_dog_anim(e)) { re15_audio_room_se(7); e->sub_state_3 = 3; } }  /* Se(7) yelp @0x801112a0 */
        else { e->state = 7; e->sub_state_3 = 0; }            /* -> CORPSE @0x801112c0 */
        break;

    case 7:   /* CORPSE 0x80111774: settle/fade (render-side), then inert */
        re15_dog_anim(e);
        break;

    default:  /* only states 0-7 exist as top-level (@0x80120f74); 8-11 are ACTIVE sub-handlers */
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
 * The only remaining wave-2 item is PRESENTATION: the exact web-spit sprite choreography (the port already
 * spawns a telegraph fx below as a faithful approximation).
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
        e->spider_phase = 0; e->spider_timer = 0;         /* +0x1d0..+0x1ec = 0 @0x80116594 (phase starts 0) */
        e->spider_home_x = (int16_t)e->x; e->spider_home_y = (int16_t)e->y; e->spider_home_z = (int16_t)e->z;  /* cache spawn @0x801165fc-2c */
        e->state = 1; e->sub_state_1 = variant;           /* +0x4=1 ACTIVE, +0x5 = +0x9 & 0x7f @0x80116690/ac */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
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
        case 1:   /* +0x6==1: wind-up; on timer expiry arm a STEP strike (wave 2: hit-code 0x22) */
            if (e->spider_timer > 0) { e->spider_timer--; break; }   /* +0x1d4-- @0x801167d4 */
            if (e->spider_phase >= 0x30) break;                      /* budget exhausted -> hold @0x801167ec */
            if (e->sub_state_1 < 3 && e->spider_phase < 8) { e->sub_state_2 = 3; break; }  /* A dead-abort @0x8011683c */
            e->spider_phase++;                                       /* +0x1d0++ @0x8011681c */
            e->sub_state_2 = 2;
            break;
        case 2:   /* +0x6==2: COMMIT strike (wave 2: hit-code 0x12) + re-roll the timer -> loop */
            e->spider_timer = (int16_t)((re15_engine_rand8() & 0x3f) + 16);  /* @0x80116cec */
            e->sub_state_2 = 1;
            break;
        default:  /* +0x6==3: Behavior-A dead sub-state (idle hold) */
            break;
        }
        break;
    }

    case 2:   /* HURT ENTRY: the shared re15_enemy_take_damage sets +0x4=2 (hurt) generically; for the
               * spider that state-table slot [2-4] trampolines to the +0x7 flinch 0x80116a04 -> route it. */
        e->state = 10; e->sub_state_3 = 0;
        break;
    case 3:   /* DEATH ENTRY: take_damage sets +0x4=3 when hp<0; route to the spider gib-burst 0x80116870. */
        e->state = 8; e->sub_state_2 = 0;
        break;

    case 8: case 9:   /* DEATH 0x80116870: gib burst -> corpse (+0x1d0 climbs to 0x30 then inert) */
        if (e->sub_state_2 == 0) {
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              e->x, e->y, e->z, (int16_t)e->rot_y);   /* gib burst @0x80116d84 */
            e->spider_phase = (uint8_t)((re15_engine_rand8() & 0x3f) + 16);
            e->sub_state_2 = 1;
        } else if (e->spider_phase < 0x30) {
            e->spider_phase++;                                       /* climb to the corpse latch @0x80116934 */
        } else {
            e->state = 7;                                           /* permanent inert corpse @0x80116904 */
        }
        break;

    case 7:   /* CORPSE: inert */
        break;

    default:  /* states 10-15 = HURT 0x80116a04 (+0x7 3-frame one-shot -> reset to state 1) */
        if (e->state >= 10 && e->state <= 15) {
            switch (e->sub_state_3) {
            case 0:  e->hit_react = 3; e->sub_state_3 = 1; break;     /* +0x93=3 hit-flash @0x80116a50 */
            case 1:  e->sub_state_3 = 2; break;                      /* COMMIT clip 0x12 @0x80116b04 */
            default: e->state = 1; e->sub_state_1 = variant;         /* RESET +0x4=0x10001 -> state 1 @0x80116b30 */
                     e->sub_state_2 = 1; e->sub_state_3 = 0; e->hit_react = 0; break;
            }
        }
        break;
    }

    /* --- root tail (@0x80116288): EMERGE gate (+0x1d0 < 13 intangible + vertical) + -2 contact --- */
    if (e->spider_phase < 13) {                          /* EMERGE: intangible, vertical climb slaved to +0x1d0 */
        int step = (e->grid_id & 0x80) ? 40 : 20;        /* step 40/20 @0x80116474/90 */
        e->y = (int32_t)e->spider_home_y - step * ((int)e->spider_phase - 1);  /* +0x38 = +0x1d6 - step*(phase-1) @0x80116494 */
    } else if (e->state == 1) {                          /* SOLID: -2 contact stagger on body overlap */
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        int32_t rc = (int32_t)e->hit_radius_min + RE15_BODY_R_PLAYER;   /* aec4 body-contact standoff */
        /* @0x80116368: the -2 is gated on FUN_8002aec4's RETURN (a body-push happened), i.e. an actual
         * OVERLAP: pen = R - SquareRoot0(dx²+dz²) > 0. Byte-true = the SAME SquareRoot0 the push uses,
         * strict '<', NOT an inclusive Euclidean dx²+dz²<=rc² (audit wf_f066b2ae / wf_8b1360d4). */
        if ((int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz)) < rc && pl->hit_react == 0) {
            pl->hit_react |= 1;                          /* DAT_800aca58=2 stagger marker is byte-true but the
                                                          * port keys the hit-react gate off +0x93 (faithful) */
            if (pl->hp >= 4) pl->hp = (int16_t)(pl->hp - 2);  /* player.hp -= 2 (floor: never below 2) @0x801163c8 */
        }
    }
}

/* ============================ MAGGOTS (type 0x27, EM027 = MAGGOTS_BABY) — Wave 1 ============= *
 * Byte-true 0x80116db8 family (RE15_MAGGOT_AI.md; workflow wf_f597f55d, adversarially verified). A
 * LARGE moving ground creature (1600x1440 body, HP 180 @0x8011f514, ~1.7 crawl): root 0x80116db8
 * dispatches +0x4 via @0x801213c8 (16 states), then the shared body-push + SCA wall-clamp tail. STATE[1]
 * 0x80117254 is a two-table brain (A decision + B movement on +0x5): idle-wander (clip 0x16) -> CHASE
 * (yaw-slew + crawl toward the player) -> GRAB/bite (-6/-12) / LEAP (ballistic). WAVE 1: INIT + idle +
 * chase + killable (death clip 0xe -> corpse clip 0xa). WAVE 2 IMPLEMENTED: the bite/heavy-bite/leap
 * attacks (0x80118270/854c/908, direct -6/-12 + grab handshake + ballistic pounce) are cases 5/6/7
 * @L3884-3910. DEFERRED: only the exact crawl speed (move-helper 0x8011bf50; port uses re15_dog_advance) and
 * any remaining special far-states routed to the wave-2 default (@L3964). */
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

static void re15_maggot_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x80116f50: -> ACTIVE, HP 180, steer=player, variant from grid&1 */
        if (e->hp <= 0) e->hp = 180;                          /* +0x9a=180 (HP tab @0x8011f514 row 0x27) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;  /* clear @0x80116f9c-fcc */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be = player @0x80116fe4/ffc */
        e->state = 1;
        e->sub_state_1 = (uint8_t)((e->grid_id & 1) ? 1 : 0);  /* +0x5 = 1 if +0x9&1 else 0 @0x80116f88 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;

    case 1: {  /* ACTIVE brain 0x80117254: LOS + dist -> decision(A)/move(B) on +0x5 */
        int los = re15_enemy_los_probe(slot, e, pl);          /* 0x8001bc08 @0x8011725c */
        int32_t dist = re15_enemy_player_dist(e, pl);         /* SquareRoot0 @0x80117300 */
        e->dog_dist = (int16_t)dist;                          /* +0x1d4 dist cache */
        switch (e->sub_state_1) {
        case 0:   /* A[0] DECISION 0x80117484 + B[0] idle-wander 0x80117574 (clip 0x16) */
            if (dist < 5000 && los != 0) { e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; break; }  /* dist<5000 & LOS -> CHASE @0x801174c4 */
            if (dist < 3000) { e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 1; break; }  /* blind chase (dist<3000) @0x80117548 */
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 59); re15_maggot_clip(e, 0x16); e->sub_state_2 = 1; }  /* +0x9c=rng+59, idle clip 0x16 @0x801175a4/c4 */
            else if (e->ai_timer == 0) { e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 1; }  /* wander expiry -> CHASE @0x80117618 */
            else e->ai_timer--;
            re15_maggot_anim(e);
            break;
        case 3:   /* CHASE (A[3] 0x80117a3c decision / B[3] 0x80117c90 crawl) */
            if (e->dog_atk_cd) e->dog_atk_cd--;               /* +0x1dc attack lockout (reused field) */
            /* A[3] decision @0x80117a5c: player in range 3000 & cone 384 & lockout==0 -> BITE (+0x5=5). */
            if (e->dog_atk_cd == 0 && re15_dog_arc(e, pl, 3000, 384)) { re15_maggot_clip(e, 0x12); re15_dog_sub(e, 5); break; }
            /* A[3] tail @0x80117c54: player.hit_react==0 & dist>=6001 & LOS(+0x1d0&1) -> SELECTOR (+0x5=4).
             * The maggot commits to a HEAVY approach when the player is FAR-but-visible (byte-true + live-
             * verified: mzd_stage1_maggot_heavy.sav, the +0x5 3->4->6 escalation). */
            if (pl->hit_react == 0 && dist >= 6001 && los != 0) { re15_maggot_clip(e, 6); re15_dog_sub(e, 4); break; }
            if (e->sub_state_2 == 0) { re15_maggot_clip(e, 4); e->sub_state_2 = 1; }   /* crawl clip 4 (rng 4/5/7 = wave 2b) @0x80117cc0 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);    /* yaw-slew toward player @0x80117d50 */
            re15_dog_advance(e, 40);                          /* crawl forward (exact speed via 0x8011bf50 = wave 2b) */
            re15_maggot_anim(e);
            break;

        case 4:   /* SELECTOR (A[4] state[12] 0x80117e40 decide + B[4] crawl clip 6): close in, then HEAVY.
                   * @0x80117e88 player.hit_react==0 & @0x80117e90 in range 4000 (a0=0xfa0) cone 192 (a1=0xc0)
                   * & @0x80117eb0 +0x1dc lockout==0 -> +0x5=6 HEAVY (@0x80117ec4). LIVE-VERIFIED (the
                   * selector persists, crawling clip 6, until in range -> heavy). The leap else-branch
                   * (@0x80117f50/0x80118000/0x801180d8 -> +0x5=7, ballistic) = wave 2c. */
            if (e->dog_atk_cd) e->dog_atk_cd--;
            if (pl->hit_react == 0 && e->dog_atk_cd == 0 && re15_dog_arc(e, pl, 4000, 0xc0)) {
                re15_maggot_clip(e, 0x13); re15_dog_sub(e, 6); e->sub_state_3 = 0; break;   /* -> HEAVY (clear connect-once) */
            }
            /* LEAP else-branch (far-ballistic path B @0x80118028, workflow wf_c1de93d6-bae CONFIRMED):
             * player OUT of the heavy window but LOS(+0x1d0&1) & dist(+0x1d4)>=6001 (@0x8011806c) &
             * aimed at player within +-32 (arc_test 0x8001a9cc @0x80118084) & the PLAYER facing the maggot
             * (facing_aligned 0x8001a780==0 @0x801180bc: playerYaw vs maggotYaw > +-90deg) & rng&1 (~50%,
             * @0x801180a8, bypassed if player action *(0x800aca58)==0x701). -> +0x5=7 LEAP (+0x7=0). */
            if (dist >= 6001 && los != 0 && e->dog_atk_cd == 0
                && re15_dog_arc(e, pl, 0x7fff, 0x20)                       /* arc_test: maggot aimed at player (+-32) */
                && (((int)(pl->rot_y - e->rot_y) + 0x400) & 0xfff) >= 0x800 /* facing_aligned==0: player faces the maggot */
                && (re15_engine_rand8() & 1)) {                            /* rng&1 coin-flip @0x801180a8 */
                re15_maggot_clip(e, 0x14); re15_dog_sub(e, 7); break;      /* -> LEAP (sub 7) */
            }
            if (dist >= 12000) { re15_dog_sub(e, 3); break; }  /* lost the player -> back to CHASE (faithful: selector abandons a far target) */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);     /* crawl toward player (B[4]) */
            re15_dog_advance(e, 40);
            re15_maggot_anim(e);
            break;

        case 5:   /* BITE 0x80118270 (clip 0x12): -6 HP on the damage-window frames {0x0c-0x0f} */
            if (e->anim_frame >= 0x0c && e->anim_frame <= 0x0f          /* damage window @0x80118400 (tab @0x8012146c) */
                && pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 384)) {   /* hitbox 0x8001bff8 (1000) @0x801183cc; connect at body contact (was 2000 < push standoff 2050; audit wf_555f18eb Part B) */
                if (e->anim_frame == 0x0c) {                            /* connect once per bite */
                    pl->hp = (int16_t)(pl->hp - 6);                     /* player.hp -= 6 @0x80118468 */
                    re15_audio_room_se(6);                              /* Se(6) bite @0x80118474 */
                    pl->hit_react |= 1; e->dog_atk_cd = 0x2d;           /* +0x1dc = 45 lockout @0x80118478 */
                }
            }
            if (re15_maggot_anim(e)) { re15_dog_sub(e, 3); if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14; }  /* -> CHASE, lockout 20 @0x801184f0 */
            break;

        case 6:   /* HEAVY-BITE 0x8011854c (clip 0x13): the maggot LUNGES forward (move-helper 0x8011bf50
                   * @0x80118664) closing the gap, then its dual-hitbox 800/800 (0x8001bff8 a2=0x320 ×2)
                   * on the damage window (frame>=0x15 @0x80118650) -> player.hp -= 12 (@0x801187bc) +
                   * Se(5) (@0x80118798) + grab-handshake (aca58/59/5a @0x801187d8). LIVE-VERIFIED
                   * (-12: mzd_stage1_maggot_heavy.sav, HP 94->82). sub_state_3 = connect-once latch. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);              /* keep facing the player */
            re15_dog_advance(e, 40);                                    /* lunge forward (0x8011bf50) */
            if (e->sub_state_3 == 0 && e->anim_frame >= 0x15            /* damage window @0x80118650 */
                && pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0xc0)) {   /* dual-hitbox 800 (0x8001bff8); connect at body contact (was 1600 < push standoff 2050; audit wf_555f18eb Part B) */
                pl->hp = (int16_t)(pl->hp - 12);                        /* player.hp -= 12 @0x801187bc */
                re15_audio_room_se(5);                                  /* Se(5) heavy @0x80118798 */
                pl->hit_react |= 1; e->dog_atk_cd = 0x2d; e->sub_state_3 = 1;   /* connect once */
            }
            if (re15_maggot_anim(e)) { re15_dog_sub(e, 3); e->sub_state_3 = 0; if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14; }  /* -> CHASE */
            break;

        case 7: {  /* LEAP 0x80118908 (clip 0x14): a 4-phase FSM on +0x6. A ballistic REPOSITION pounce
                    * that deals ZERO damage (no player.hp write in the whole 260-instr handler - workflow
                    * wf_c1de93d6-bae CONFIRMED); it closes the gap, lands, and returns to the SELECTOR so
                    * the HEAVY then applies the -12. LIVE-VERIFIED (forced +0x5=7: impulse +0x8c=201 seeded,
                    * maggot closed dist 10000->596, player HP untouched by the leap itself). */
            if (e->sub_state_2 == 0) {                        /* phase 0/1: INIT + WINDUP (frames 0..9) */
                if (e->motion != 0x14) { re15_maggot_clip(e, 0x14); e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 180); }  /* @0x8011895c seed impulse */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x20);            /* slew facing toward the launch dir */
                if (re15_maggot_anim(e) || e->anim_frame >= 0x0a) {       /* LAUNCH at frame 10 (@0x80118a3c) */
                    e->sub_state_2 = 2;                                   /* +0x6=2 in-flight (@0x80118aa4) */
                    e->crow_speed   = (int16_t)((re15_engine_rand8() & 0x1f) + 200);  /* re-roll impulse @0x80118a50 */
                    e->ai_timer     = 0;                                 /* +0x9c airtime = 0 */
                    e->crow_perch_h = (int16_t)e->y;                     /* cache ground-Y (+0x1ba) for the land */
                }
            } else {                                          /* phase 2: IN-FLIGHT ballistic arc (@0x80118b14) */
                re15_crow_advance(e);                                     /* horizontal pounce at crow_speed */
                int32_t s1 = 720 - 60 * (int32_t)e->ai_timer;            /* vert-vel +0x1d8=720, gravity -60 (0x8001c1a4) */
                e->y -= s1;                                              /* +0x38 -= s1 (Y-down: rises then falls) */
                e->ai_timer++;
                re15_maggot_anim(e);
                if ((int32_t)e->ai_timer >= 25 || ((int32_t)e->ai_timer > 12 && e->y >= (int32_t)e->crow_perch_h)) {
                    e->y = (int32_t)e->crow_perch_h;                     /* land: clamp back to ground */
                    re15_dog_sub(e, 4);                                 /* recovery (+0x5=9 @0x80118b60) folded -> SELECTOR */
                }
            }
            break;
        }

        default:  /* sub 8/9 (mid-air finisher / landing-recovery) — brief anims that resolve back to the
                   * selector; fold to the brain (no damage of their own) */
            e->sub_state_1 = 0; e->sub_state_2 = 0;
            break;
        }
        break;
    }

    case 2:   /* HURT ENTRY: shared take_damage sets +0x4=2. The maggot's state 2 is a ballistic special
               * (leap-abort); for wave 1 flinch briefly then resume the brain. */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH 0x8011b6fc -> death-anim 0x8011b7b8: clip 0xe + gore + Se(0) -> corpse */
        if (e->sub_state_3 == 0) { re15_maggot_clip(e, 0x0e); re15_audio_room_se(0); e->hit_react |= 2; e->sub_state_3 = 1; }  /* death clip 0xe @0x8011b830 */
        else if (e->sub_state_3 == 1) { if (re15_maggot_anim(e)) e->sub_state_3 = 2; }
        else { e->state = 7; e->sub_state_3 = 0; }            /* -> CORPSE */
        break;

    case 7:   /* CORPSE 0x8011b998: settle to the corpse pose (clip 0xa), inert */
        if (e->motion != 0x0a) re15_maggot_clip(e, 0x0a);
        re15_maggot_anim(e);
        break;

    default:  /* the ballistic far-states [2-7] (leap/special) = wave 2 -> resume the brain */
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
static void re15_npc_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_npc_anim(re15_actor_t *e)     /* POST-inc +0x95, wrap at the real EM040 clip length */
{
    uint8_t c = e->motion; int fc = (c < 24) ? s_irons_clip_len[c] : 1; if (fc < 1) fc = 1;
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
    uint8_t c = e->motion; int fc = (c < 24) ? s_irons_clip_len[c] : 1; if (fc < 1) fc = 1;
    switch (e->sub_state_2) {
    case 0:   /* phase 0 init (@0x80050cec-d0c): +0x95=0, seed +0x8f crossfade, phase->1, then fall to play */
        e->anim_frame = 0; e->anim_frac = 7; e->sub_state_2 = 1;
        /* fallthrough — the original phase-0 branch continues into the phase-1 body @0x80050d34 */
    case 1:   /* phase 1 play (@0x80050d34): advance the clip; at end latch phase 2 (HOLD) or replay (LOOP) */
        if (e->anim_frame + 1 >= fc) {                       /* anim_set signals clip end (@0x80050d54) */
            if (e->anim_flags & 0x04) { e->anim_frame = 0; e->sub_state_2 = 1; }  /* LOOP replay @0x80050dc8 */
            else { e->anim_frame = (uint8_t)(fc - 1); e->sub_state_2 = 2; }         /* HOLD last frame @0x80050da4 */
        } else {
            e->anim_frame++;                                 /* +0x95++ */
            if (e->anim_frac > 0) e->anim_frac--;
        }
        break;
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
    case 0: re15_npc_sub_motion(e); break;   /* motion phase-FSM = Plc_motion pose (@0x80076ca0[0]=0x80050cb8) */
    case 4: case 5: case 7: case 8: re15_npc_sub_walk(e); break;
    case 6: re15_npc_sub_event_reach(e); break;
    case 9: re15_npc_sub_turn(e); break;
    default: re15_npc_sub_idle(e); break;   /* 1-3 idle */
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
    if (e->walk_active || re15_scd_slot_event_controlled(slot)) {
        e->hp = -1;
        return;
    }

    switch (e->state) {
    case 0:   /* INIT 0x8011c6dc: idle pose, INVULNERABLE, -> state 1 (or the shared executor) */
        e->hp = -1;                                       /* +0x9a = -1 (no HP / invulnerable) @0x8011c744 */
        e->motion = 2; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;  /* idle clip 2 @0x8011c7bc */
        e->ai_timer = 0x78;                               /* +0x9e = 120 @0x8011c754 */
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

/* ============================ ZOMBIE GIRL (type 0x13, EM013 = ZOMBIE_GIRL) — Wave 1 ========== *
 * Byte-true 0x8010a8c8 family (RE15_ZOMBIEGIRL_AI.md; workflow wf_21e29175, adversarially verified). A
 * NAV-PATHING female-zombie variant that REUSES the standard zombie's combat machinery: her active brain
 * (0x8010b274) dispatches the SAME phase handlers (engage 0x80102058, GRAB 0x80102548 = cmd5 + player.hp
 * -=10) and shares the zombie corpse-settle (state 7 = 0x80109554). WAVE 1: INIT (HP 50-81) + nav-chase
 * (clip 26) + the grab (-10, pins the player via the shared victim FSM) + killable (-> corpse). The repeated grab bites (HELD BITE-LOOP -5/clip) are IMPLEMENTED @L4212-4216. DEFERRED to
 * wave 2: the exact mode-dispatch (+0x9&0xf @0x80120230) and the lunge-arm timer choreography. */
static const uint8_t s_zgirl_clip_len[42] =   /* EM013 clip frame-counts (CDEMD0.EMS idx 3, dir[3]) */
    { 14,30,26,14,30,26,3,3,3,65,65,55,1,55,1,40,40,60,98,98,5,5,138,17,1,85,79,19,15,21,25,68,2,1,64,1,65,3,3,68,89,59 };
static void re15_zgirl_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }
static int re15_zgirl_anim(re15_actor_t *e)
{
    uint8_t c = e->motion; int fc = (c < 42) ? s_zgirl_clip_len[c] : 1; if (fc < 1) fc = 1;
    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    return done;
}

static void re15_zgirl_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x8010ab2c: -> ACTIVE, HP 50-81, nav-walk pose, steer=player */
        if (e->hp <= 0) e->hp = (int16_t)((re15_engine_rand8() & 0x1f) + 50);  /* +0x9a = (rng&0x1f)+50 @0x8010ac1c */
        e->motion = 0x1a; e->anim_frame = 0; e->anim_frac = 7; e->hit_react = 0;  /* nav-walk clip 26 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;  /* +0x1bc/+0x1be = player @0x8010abc0 */
        e->ai_timer = 0x14;                                   /* +0x9c = 20 @0x8010ac04 */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* +0x4=1 @0x8010aba8 */
        break;

    case 1: {  /* ACTIVE brain 0x8010b274: nav-chase (nav-walk clip 26) -> engage/GRAB */
        e->dog_dist = (int16_t)re15_enemy_player_dist(e, pl);
        /* engage decider 0x80102058: player close (<1200) & aligned & not reacting -> GRAB (state 3) @0x80102128 */
        if (pl->hit_react == 0 && re15_dog_arc(e, pl, 1200, 384)) { e->state = 3; e->sub_state_3 = 0; break; }
        if (e->sub_state_2 == 0) { re15_zgirl_clip(e, 0x1a); e->sub_state_2 = 1; }   /* nav-walk clip 26 @0x8010be50 */
        re15_enemy_steer_point(e, pl->x, pl->z, 0x10);        /* steer rate 0x10 (0x8001aac4) @0x8010bdec */
        re15_dog_advance(e, 30);                              /* nav-walk forward (root-motion 0x8010c088) */
        re15_zgirl_anim(e);
        break;
    }

    case 3:   /* commit to attack GRAB 0x8010c014 (hp>=0) — OR take_damage DEATH (hp<0). The zgirl REUSES
               * the standard zombie's grab FUN_80102548 byte-true: [0/2] IMPACT -10 then a HELD BITE-LOOP
               * of -5 per clip cycle, gated by the kill (+0x9e=100) and escape (+0x9c=110) counters —
               * NOT a single -10 then release. */
        if (e->hp < 0) { e->state = 7; e->sub_state_3 = 0; break; }   /* lethal -> shared corpse */
        s_player_grabbed = 1; re15_player_victim_latch(e, pl);        /* pin the player (cmd5 @0x80102640) */
        if (e->sub_state_3 == 0) {                                    /* [0/2] commit + IMPACT */
            re15_zgirl_clip(e, 0x0b);                                 /* lunge/grab clip 11 @0x8010b584 */
            if (pl->hit_react == 0) { pl->hp = (int16_t)(pl->hp - 10); pl->hit_react |= 1; }  /* -10 @0x8010277c */
            e->grab_kill_ctr = 100; e->ai_timer = 110;               /* +0x9e kill / +0x9c escape windows */
            e->sub_state_3 = 1;
        } else {                                                     /* [3] BITE-LOOP: -5 per clip cycle */
            if (re15_zgirl_anim(e) && pl->hp >= 0) {                  /* one bite per grab-clip wrap @0x801027dc */
                pl->hp = (int16_t)(pl->hp - 5);
                if (pl->hp < 0) pl->state = 7;                        /* devoured -> player death */
            }
            if (--e->grab_kill_ctr <= 0 || pl->hp < 0) {             /* kill window ran out -> devour handoff */
                pl->state = 7; e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; break;
            }
            e->ai_timer = (int16_t)(e->ai_timer - (int16_t)(1 + 5 * re15_mash_pressed()));  /* escape drain @0x80037024 */
            if (e->ai_timer < 0) { e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; }  /* THROW-OFF (escape alive) */
        }
        break;

    case 2:   /* HURT (take_damage +0x4=2) -> flinch -> resume the brain */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 7:   /* CORPSE (shared zombie 0x80109554): settle, inert */
        re15_zgirl_anim(e);
        break;

    default:  /* the mode-dispatch attack/turn sub-states = wave 2 -> resume the brain */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ ADULT SPIDER (type 0x25, EM025 = SPIDER) — Wave 1 ============= *
 * Byte-true 0x801109e4 family (STAGE2.BIN; workflow wf_1f5685b3-a78, 12 agents adversarially verified).
 * The larger MOBILE counterpart to the ported 0x26 SPIDER_BABY. Root 0x801109e4 (pause-gate DAT_800aca40
 * &0x20000000, skip-gate +0x9&0x20) dispatches +0x4 via @0x80118e24 (16 states) -> shared body-tail
 * (0x8002b498 contact-clear, 0x8002aec4+0x8002b544 body-push, 0x8003b0a4 SCA-wall-clamp, 0x8001b064 box-
 * rebuild) — a ground creature like the maggot. The ACTIVE brain (state[1] 0x80110e50) is a DUAL-TABLE
 * A/B brain (grid-DECIDE @0x80118e44 by +0x9 gated on +0x5==0, substep-ACT @0x80118e64 by +0x6). Its
 * attack is a GRAB (latch DAT_800aca58=2 + a keep-alive clamp; NO direct player.hp write anywhere in
 * 0x801109e4-0x801158a0 — the shared player grab-FSM applies the damage). WAVE 1: INIT + chase + killable.
 * DEFERRED to wave 2 (needs a STAGE2 provoke via the repacker + the DAT_800aca58=2 player-FSM): the exact
 * grab damage/pin, the full grid/substep dispatch, the ceiling-drop variant, the exact chase speed. */
static const uint16_t s_aspider_hp_row[16] =    /* HP table @0x8011761c = HPbase 0x8011717c + 0x25*0x20 */
    { 76,105,125,78,80,109,129,83,113,132,87,117,136,93,99,121 };   /* hp = row[rng()&0xf] @0x80110da0 */
/* ROAM (grid 0) idle-next-substep table @0x80118ea4 [2 rows x 8, idx=(LOS&1)*8 + (rng&7)] (byte-true). */
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
    case 0:   /* INIT 0x80110b6c: -> ACTIVE, HP row, clip 0x10, steer=player */
        if (e->hp <= 0) e->hp = (int16_t)s_aspider_hp_row[re15_engine_rand8() & 0xf];  /* +0x9a @0x80110da0 */
        e->motion = 0x10; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;   /* clip 0x10 @0x80110b90 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be = player @0x80110bd8/bf0 */
        e->ai_timer = 0; e->dog_atk_cd = 0; e->dog_dist = 0;        /* clear +0x9c/+0x1dc lockout etc */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        /* grid-branch (byte-true @0x80110b6c, ceiling-drop cluster CONFIRMED): grid 0x40 -> state 4 sub 0,
         * grid 0x41 -> state 4 sub 1 (the AMBUSH cling-approach @0x80114ec8, 9-phase @0x8010027c) — the
         * ROOM2060 ground spiders spawn grid 0x41; grid 2-0x1f -> ceiling-drop (y=-10800; unexercised);
         * else -> ACTIVE. */
        if (e->grid_id == 0x40 || e->grid_id == 0x41) {
            e->state = 4; e->sub_state_1 = (uint8_t)(e->grid_id & 1);
        } else {
            if ((unsigned)((e->grid_id & 0xffu) - 2u) < 0x1eu) e->y = -10800;   /* ceiling-drop start (unexercised) */
            e->state = 1; e->sub_state_1 = 0;
        }
        break;

    case 1: {  /* ACTIVE brain 0x80110e50 — the byte-true TWO-TABLE driver (workflow wf_3467b541-a93,
                * grid-DECIDE + substep-ACT adversarially CONFIRMED). Every frame: LOS + dist, then
                * grid-DECIDE @0x80118e44[+0x9] (ONLY if the +0x5 commit-latch is 0), then substep-ACT
                * @0x80118e64[+0x6] (ALWAYS). +0x5=commit-latch, +0x6=substep, +0x7=grab-step. */
        int los = re15_enemy_los_probe(slot, e, pl) & 1;      /* +0x1d0 bit0 = clear LoS @0x80110e50 */
        int32_t dist = re15_enemy_player_dist(e, pl);         /* SquareRoot0 -> +0x1d4 */
        e->dog_dist = (int16_t)dist;
        if (e->dog_atk_cd) e->dog_atk_cd--;                   /* +0x1d6 post-grab lockout */

        if (e->sub_state_1 == 0) {                            /* grid-DECIDE (grid fixed at spawn; no +0x9 write) */
            int grid = e->grid_id & 0xf;
            if (grid == 1 || grid == 5) {                     /* APPROACH 0x80111240 */
                if (e->dog_atk_cd == 0 && dist < 7500 && los) {          /* lunge-trigger @0x80111368 */
                    e->sub_state_1 = 1; e->sub_state_2 = (uint8_t)(6 + (re15_engine_rand8() & 1)); e->sub_state_3 = 0;
                } else if (los) { e->sub_state_1 = 1; e->sub_state_2 = 4; e->sub_state_3 = 0; }  /* else CHASE [4] */
            } else if (grid == 2) {                           /* ATTACK 0x80111488 */
                if (e->dog_atk_cd == 0 && dist < 3000) { e->sub_state_1 = 1; e->sub_state_2 = 8; e->sub_state_3 = 0; }
                else if (los) { e->sub_state_1 = 1; e->sub_state_2 = 4; e->sub_state_3 = 0; }
            } else {                                          /* ROAM 0x80111040: idle-next table @0x80118ea4 */
                int idx = (los ? 8 : 0) + (re15_engine_rand8() & 7);
                e->sub_state_1 = 1; e->sub_state_2 = s_aspider_idle_next[idx]; e->sub_state_3 = 0;
            }
        }

        switch (e->sub_state_2) {                             /* substep-ACT @0x80118e64[+0x6] */
        case 2:   /* SCAN 0x80111c5c: clip 3, rot_y += ±0x18, wander timer -> re-decide */
            if (e->motion != 3) { re15_aspider_clip(e, 3); e->ai_timer = (int16_t)(re15_engine_rand8() & 0x3f); }
            e->rot_y = (int16_t)((e->rot_y + ((e->ai_timer & 1) ? 0x18 : -0x18)) & 0xfff);
            if (los && re15_dog_arc(e, pl, 0x7fff, 0x40)) { e->sub_state_2 = 4; }   /* now facing -> CHASE @0x80111d5c */
            else if (e->ai_timer-- <= 0) { e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 4 : 0); }  /* @0x80111da8 */
            re15_aspider_anim(e);
            break;
        case 3: case 5:  /* TURN-to-face 0x80111e04/0x8011207c: clip 2, slew, -> CREEP/idle when faced */
            if (e->motion != 2) re15_aspider_clip(e, 2);
            re15_enemy_steer_point(e, pl->x, pl->z, (e->sub_state_2 == 3) ? 0x80 : 0x20);
            if (re15_dog_arc(e, pl, 0x7fff, 0x40)) { e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 11 : 0); }  /* @0x80111f2c */
            re15_aspider_anim(e);
            break;
        case 4:   /* CHASE 0x80112004: clip 2, +0x8c += 100/frame, steer 0x30, advance forward @0x80112064 */
            if (e->motion != 2) { re15_aspider_clip(e, 2); e->crow_speed = 0; }
            e->crow_speed = (int16_t)(e->crow_speed + 100);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            re15_dog_advance(e, e->crow_speed >> 5);          /* faithful: pos_advance a0<<16 GTE (>>5 stand-in) */
            if (e->dog_atk_cd == 0 && dist < 7500 && los) { e->sub_state_1 = 0; }  /* re-decide -> may lunge */
            re15_aspider_anim(e);
            break;
        case 11:  /* CREEP 0x8011302c: clip 3, +0x8c -= 20/frame, steer 0x10, advance @0x8011308c */
            if (e->motion != 3) { re15_aspider_clip(e, 3); e->crow_speed = 400; }
            if (e->crow_speed > 20) e->crow_speed = (int16_t)(e->crow_speed - 20);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x10);
            re15_dog_advance(e, e->crow_speed >> 5);
            if (e->dog_atk_cd == 0 && dist < 3000 && los) { e->sub_state_1 = 0; }
            re15_aspider_anim(e);
            break;
        case 6: case 7:  /* GRAB A 0x801120b8 (walking lunge-bite, +0x7 9-phase @0x8010019c). NON-DAMAGING. */
            if (e->sub_state_3 == 0) { re15_aspider_clip(e, 8); e->sub_state_3 = 1; }        /* [0] clip 8 windup */
            else if (e->sub_state_3 <= 2) {                                                   /* [1/2] windup adv @100 */
                re15_dog_advance(e, 100 >> 5);
                if (re15_aspider_anim(e)) { re15_aspider_clip(e, 0x0b); e->sub_state_3 = 3; } /* -> clip 0xb track */
            } else if (e->sub_state_3 == 3) {                                                 /* [3] track: face player */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
                if (re15_dog_arc(e, pl, 0x7fff, 0x40)) e->sub_state_3 = 4;                    /* faced -> STRIKE */
                re15_aspider_anim(e);
            } else if (e->sub_state_3 == 4) {                                                 /* [4/5] STRIKE adv @300 */
                re15_enemy_steer_point(e, pl->x, pl->z, 0x10);
                re15_dog_advance(e, 300 >> 5);
                if (pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x100)) {  /* contact CONNECT @0x8011254c; body contact (was 900 < push standoff 1450; audit wf_555f18eb Part B) */
                    re15_audio_room_se(2); s_player_grabbed = 1; pl->hit_react |= 1;
                    if (pl->hp < 0) pl->hp = 1;                                               /* keep-alive clamp (NO damage) */
                    re15_aspider_clip(e, 9); e->sub_state_3 = 6;                              /* -> recover */
                } else if (re15_aspider_anim(e)) { re15_aspider_clip(e, 9); e->sub_state_3 = 6; }  /* miss -> recover */
            } else {                                                                          /* [6/7] clip 9 recover */
                re15_dog_advance(e, 100 >> 5);
                if (re15_aspider_anim(e)) { e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; e->dog_atk_cd = 0xf; }  /* [8] reset + lockout 15 */
            }
            break;
        case 8:   /* GRAB B 0x801127f0 (ballistic LEAP grab, +0x7 5-phase @0x801001c4). NON-DAMAGING. */
            if (e->sub_state_3 == 0) { re15_aspider_clip(e, 0); e->crow_perch_h = (int16_t)e->y; e->ai_timer = 0;
                                       re15_enemy_steer_point(e, pl->x, pl->z, 0x40); e->sub_state_3 = 1; }  /* [0] launch */
            else if (e->sub_state_3 == 1) {                                                   /* [1] leap parabola @0x80112884 */
                e->y += 8 * (int32_t)e->ai_timer * (int32_t)e->ai_timer;   /* Y += 8*t^2 (Y-down) */
                e->ai_timer++;
                re15_dog_advance(e, 200 >> 5);                                                /* forward while airborne */
                if (pl->hit_react == 0 && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x100)) {  /* CONNECT @0x801129cc; body contact (was 900 < push standoff 1450; audit wf_555f18eb Part B) */
                    re15_audio_room_se(2); s_player_grabbed = 1; pl->hit_react |= 1;
                    if (pl->hp < 0) pl->hp = 1;                                               /* keep-alive clamp (NO damage) */
                }
                if (e->y >= (int32_t)e->crow_perch_h) { e->y = (int32_t)e->crow_perch_h; re15_aspider_clip(e, 7); e->sub_state_3 = 2; }  /* land */
            } else {                                                                          /* [2..4] clip 7 recover */
                if (re15_aspider_anim(e)) { e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; e->dog_atk_cd = 0xf; }
            }
            break;
        default:  /* feed/reroute/hold substeps (7/9/10/12/13) = cited mechanism -> resume the driver */
            e->sub_state_1 = 0; e->sub_state_2 = (uint8_t)(los ? 4 : 0); e->sub_state_3 = 0;
            re15_aspider_anim(e);
            break;
        }
        break;
    }

    case 4: {  /* AMBUSH 0x80114e18[+0x5]=0x80114ec8 (grid 0x40/0x41): a 9-phase +0x7 cling-approach maneuver
                * (@0x8010027c) — clip 0x10 cling pose (@0x80114f34) + pos_advance @200 (0xc8 @0x80114fa8)
                * toward the player; when the player is in LOS + range it hands off to the ACTIVE brain. */
        if (e->motion != 0x10) re15_aspider_clip(e, 0x10);
        re15_enemy_steer_point(e, pl->x, pl->z, 0x20);
        re15_dog_advance(e, 0xc8 >> 5);                                /* pos_advance @200 @0x80114fb0 */
        if ((re15_enemy_los_probe(slot, e, pl) & 1) && re15_enemy_player_dist(e, pl) < 4000) {
            e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* -> ACTIVE brain */
        }
        re15_aspider_anim(e);
        break;
    }

    case 2:   /* HURT 0x80113a0c: flinch clip 1 + blend 7 + Se(2) -> recover to ACTIVE */
        if (e->sub_state_3 == 0) { re15_aspider_clip(e, 1); re15_audio_room_se(2); e->sub_state_3 = 1; }  /* @0x80113a0c */
        else if (re15_aspider_anim(e)) { e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
                                         e->sub_state_3 = 0; e->hit_react = (uint8_t)(e->hit_react & ~1u); }
        break;

    case 3:   /* DEATH 0x80113f40: collapse clip 1 + gore (0x8004ef90) + Se(5) + kill-flag -> CORPSE (state 7) */
        if (e->sub_state_3 == 0) { re15_aspider_clip(e, 1); re15_audio_room_se(5); e->hit_react |= 2; e->sub_state_3 = 1; }  /* @0x80113f40 */
        else if (re15_aspider_anim(e)) { e->state = 7; e->sub_state_3 = 0; }
        break;

    case 7:   /* CORPSE 0x801153d4: final clip 5, sink into the floor, inert (flags 0x2|0x40) */
        if (e->motion != 5) re15_aspider_clip(e, 5);          /* @0x801153d4 */
        re15_aspider_anim(e);
        break;

    default:  /* the grid/substep attack sub-states = wave 2 -> resume the brain */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ COCKROACH (type 0x29, EM029) — STAGE3 ========================= *
 * Byte-true 0x80110b00 family (STAGE3.BIN; workflow wf_066cfe96-fc2, adversarially verified). A small
 * MAGGOT-LIKE ground creature: root 0x80110b00 dispatches +0x4 via @0x8011eca4 (16 states) -> shared
 * body-tail. The ACTIVE brain (state[1] 0x8011101c) is a two-table A/B brain (A decision @0x8011ecc4 +
 * B movement @0x8011ed04 on +0x5, like the maggot). It scurries TOWARD the player (no flee, no swarm) and
 * deals DIRECT player.hp damage: BITE -5 (clip 0x12 @0x80111f60) + HEAVY -10 (clip 0x13 @0x801122ac).
 * WAVE 1: INIT + idle/approach/scurry chase + bite/heavy + killable (HURT clip 7 / DEATH clip 0xe /
 * CORPSE). DEFERRED (cited): the exact scurry speed, the lunge sub-states 8/9, the -500 overwhelm attack. */
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
    case 0:   /* INIT 0x80110c98: -> ACTIVE, HP row, idle clip 0x16, steer=player */
        if (e->hp <= 0) e->hp = (int16_t)s_roach_hp_row[re15_engine_rand8() & 0xf];   /* +0x9a, HP 81-121 */
        e->motion = 0x16; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;      /* clip 0x16 @0x80111310 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
        e->ai_timer = 0; e->dog_atk_cd = 0; e->dog_dist = 0;
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;

    case 1: {  /* ACTIVE two-table A/B brain 0x8011101c (A decision @0x8011ecc4 + B movement @0x8011ed04) */
        int los = re15_enemy_los_probe(slot, e, pl);          /* 0x8001bc08 -> +0x1d0/+0x1d8 */
        int32_t dist = re15_enemy_player_dist(e, pl);         /* SquareRoot0 -> +0x1d4 */
        e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 0: case 1:  /* A[0/1] idle (clip 0x16) + decision @0x80111220 */
            if ((dist < 5000 && los != 0) || dist < 3000) { re15_dog_sub(e, 3); break; }   /* -> APPROACH */
            if (e->sub_state_2 == 0) { e->ai_timer = (int16_t)(re15_engine_rand8() + 0x3b); re15_roach_clip(e, 0x16); e->sub_state_2 = 1; }  /* idle timer rng+59 */
            else if (e->ai_timer-- <= 0) { re15_dog_sub(e, 3); }
            re15_roach_anim(e);
            break;
        case 3:  /* A[3] approach-walk (clip 4, steer only) + decision @0x801117d8 */
            if (e->dog_atk_cd) e->dog_atk_cd--;               /* +0x1dc lockout */
            if (e->dog_atk_cd == 0 && pl->hit_react == 0 && re15_dog_arc(e, pl, 3000, 0x180)) {  /* in range 3000/384 -> BITE */
                re15_roach_clip(e, 0x12); re15_dog_sub(e, 5); break; }
            if (dist >= 6001 && los != 0) { re15_dog_sub(e, 4); break; }   /* far + LOS -> fast SCURRY */
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 4); e->sub_state_2 = 1; }
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);    /* steer toward player (clip root-motion advance) */
            re15_dog_advance(e, 30);                          /* faithful root-motion advance */
            re15_roach_anim(e);
            break;
        case 4:  /* A[4] fast forward scurry (clip 6, pos_advance @+0x8c) + decision @0x80111ac0 */
            if (e->dog_atk_cd) e->dog_atk_cd--;
            if (e->dog_atk_cd == 0 && pl->hit_react == 0 && re15_dog_arc(e, pl, 4000, 0xc0)) {   /* in range 4000/192 -> HEAVY */
                re15_roach_clip(e, 0x13); re15_dog_sub(e, 6); break; }
            if (e->sub_state_2 == 0) { re15_roach_clip(e, 6); e->crow_speed = (int16_t)((re15_engine_rand8() & 0x1f) + 0xb4); e->sub_state_2 = 1; }  /* speed rng+180 @0x80111c08 */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            re15_dog_advance(e, e->crow_speed >> 3);          /* pos_advance @speed +0x8c (faithful scale) */
            re15_roach_anim(e);
            break;
        case 5:  /* B[5] BITE (clip 0x12): player.hp -= 5 on the damage window @0x80111d68 */
            if (e->anim_frame >= 0x0c && e->anim_frame <= 0x0f && pl->hit_react == 0 && re15_dog_arc(e, pl, 2000, 0x180)) {
                if (e->anim_frame == 0x0c) {
                    pl->hp = (int16_t)(pl->hp - 5);           /* player.hp -= 5 @0x80111f60 */
                    re15_audio_room_se(5); pl->hit_react |= 1; e->dog_atk_cd = 0x2d;   /* Se5 + lockout 45 @0x80111f74 */
                }
            }
            if (re15_roach_anim(e)) { re15_dog_sub(e, 3); if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14; }
            break;
        case 6:  /* HEAVY (clip 0x13): player.hp -= 10 @0x801122ac */
            if (e->anim_frame >= 0x0c && e->anim_frame <= 0x11 && pl->hit_react == 0 && re15_dog_arc(e, pl, 2000, 0xc0)) {
                if (e->anim_frame == 0x0c) {
                    pl->hp = (int16_t)(pl->hp - 10);          /* player.hp -= 10 @0x801122ac */
                    re15_audio_room_se(5); pl->hit_react |= 1; e->dog_atk_cd = 0x2d;
                }
            }
            if (re15_roach_anim(e)) { re15_dog_sub(e, 3); if (e->dog_atk_cd == 0) e->dog_atk_cd = 0x14; }
            break;
        default:  /* reorient/lunge sub-states (7/8/9) = deferred -> resume the brain */
            re15_dog_sub(e, 0); re15_roach_anim(e);
            break;
        }
        break;
    }

    case 2:   /* HURT 0x80114790: flinch clip 7 (12f) + Se(2) -> ACTIVE */
        if (e->sub_state_3 == 0) { re15_roach_clip(e, 7); re15_audio_room_se(2); e->sub_state_3 = 1; }  /* @0x801148c4 */
        else if (re15_roach_anim(e)) { e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; e->hit_react = (uint8_t)(e->hit_react & ~1u); }
        break;

    case 3:   /* DEATH 0x80114fb4: collapse clip 0xe (70f) + Se(7) -> CORPSE (state 7) */
        if (e->sub_state_3 == 0) { re15_roach_clip(e, 0x0e); re15_audio_room_se(7); e->hit_react |= 2; e->sub_state_3 = 1; }  /* @0x801150e8 */
        else if (re15_roach_anim(e)) { e->state = 7; e->sub_state_3 = 0; }
        break;

    case 7:   /* CORPSE 0x80115a6c: hold the final death pose, sink/fade, inert */
        re15_roach_anim(e);
        break;

    default:
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ G-BIRKIN boss form 1 (type 0x30, EM030) — STAGE3 ============== *
 * Byte-true 0x80116230 family (STAGE3.BIN; workflow wf_5df42870-cba, INIT cluster adversarially CONFIRMED).
 * The central STAGE3 BOSS (STAGE2's 0x30 is an un-registered placeholder). Root 0x80116230: pause/skip gates
 * -> dist -> NAV-STEER 0x80039e7c (RDT nav zones) -> +0x4 dispatch @0x8011ee84 (16 states). Spawns ROOM3070
 * (with 0x33 = the next form, BIRKIN_3), grid 0x33.
 * WAVE 1: INIT byte-true (boss HP 300 hardcoded, hurt-box 1000/1440, idle clip 0, -> ACTIVE sub 9) + a
 * faithful NAV-CHASE placeholder + killable. DEFERRED (the boss's meat, needs a decomposed RE of the ~13KB
 * ACTIVE brain 0x80116d38): the claw/lunge/grab ATTACK sub-states (player.hp damage), the sub-dispatch
 * (@0x8011eeb8 by +0x5 / @0x8011eea4 by grid), and the FORM-3 (type 0x33) transition-on-death (morph). */
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

    switch (e->state) {
    case 0:   /* INIT 0x801166e0 (byte-true CONFIRMED): boss HP 300, idle clip 0, steer=player -> ACTIVE sub 9 */
        if (e->hp <= 0) e->hp = 300;                          /* +0x9a = 0x12c hardcoded @0x80116910 (boss HP) */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;   /* idle clip 0 @0x8011683c */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be = player @0x80116710/728 */
        e->ai_timer = 100; e->dog_atk_cd = 9;                 /* +0x9c=100 @0x8011677c, +0x1dc=9 @0x80116800 */
        e->state = 1; e->sub_state_1 = 9; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* grid 0x33 -> sub 9 @0x80116890 */
        break;

    case 1: {  /* ACTIVE brain 0x80116d38 — the byte-true sub-dispatch on +0x5 (workflow wf_204436c3-0fb).
                * Boss starts at sub 9 (emergence) -> sub 1 (HUB: walk + decide) -> sub 3/4 (claw/bite -10). */
        int32_t dist = re15_enemy_player_dist(e, pl);
        e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 9:   /* EMERGENCE (sub 9 0x80119378): clip 0x10 emerge pose -> the HUB */
            if (e->motion != 0x10) re15_birkin_clip(e, 0x10);
            if (re15_birkin_anim(e)) { e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 0: case 1:  /* HUB (sub 1 0x80116f6c decide + 0x801171d4 act): walk clip 1 toward the player, DECIDE
                          * the attack by distance/cone (byte-true thresholds from the sub-1 DECIDE cascade). */
            if (e->motion != 1) re15_birkin_clip(e, 1);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);    /* walk steer rate 0x20 @0x80117254 */
            re15_dog_advance(e, 40);
            if (pl->hit_react == 0) {
                if (dist < 2500 && re15_dog_arc(e, pl, 2500, 0x1f4)) {          /* -> BITE (sub 4) @0x80116f6c */
                    re15_birkin_clip(e, 4); e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; }
                else if (dist < 3200 && re15_dog_arc(e, pl, 3200, 0x338)) {     /* -> LUNGE/CLAW (sub 3) */
                    re15_birkin_clip(e, 3); e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; }
                else if (dist < 3800 && re15_dog_arc(e, pl, 3800, 0x464)) {     /* -> TACKLE (sub 7): the sub-1
                          * DECIDE gate is (arc 0x10 != 0 && arc 0x464 == 0) i.e. the player is inside the WIDE
                          * ±0x464 cone but not razor-dead-ahead (±0x10); the ±0x10 exclusion is faithful-line
                          * (a negligible sliver the closer sub 3/4 gates already own). */
                    re15_birkin_clip(e, 5); e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0; }
                else if (dist >= 3800 && dist < 9000) {                        /* -> CHARGE-COMBO (sub 10, faithful far-range) */
                    re15_birkin_clip(e, 1); e->sub_state_1 = 10; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            }
            re15_birkin_anim(e);
            break;
        case 3:   /* LUNGE/CLAW GRAB (sub 3 0x801174fc): clip 3 strike -> player.hp -= 10 on the window
                   * (+0x95 [0x24..0x2b] @0x801177f0, grab latch aca58=(facing<<8)|5) -> clip 0xa recovery ->
                   * GRAB-THROW (sub 5). */
            if (e->sub_state_3 == 0) {
                re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
                if (e->anim_frame >= 0x24 && e->anim_frame <= 0x2b && pl->hit_react == 0 && re15_dog_arc(e, pl, 2500, 0x400)) {
                    if (e->anim_frame == 0x24) {
                        pl->hp = (int16_t)(pl->hp - 10);        /* player.hp -= 10 @0x801177f0 */
                        if (pl->hp < 0) pl->hp = 1;             /* keep-alive clamp @0x80117810 */
                        re15_audio_room_se(7); pl->hit_react |= 1; s_player_grabbed = 1;   /* grab latch (cmd 5) */
                        e->sub_state_2 = 1;                     /* mark: grabbed -> throw combo */
                    }
                }
                if (re15_birkin_anim(e)) { re15_birkin_clip(e, 0x0a); e->sub_state_3 = 1; }
            } else if (re15_birkin_anim(e)) {
                e->sub_state_1 = (uint8_t)(e->sub_state_2 ? 5 : 1);   /* grabbed -> THROW (sub 5), else -> HUB */
                e->sub_state_2 = 0; e->sub_state_3 = 0;
            }
            break;
        case 4:   /* FAST BITE/CLAW (sub 4 0x801179d8): clip 4 -> player.hp -= 10 -> HUB */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            if (e->anim_frame >= 0x0c && e->anim_frame <= 0x12 && pl->hit_react == 0 && re15_dog_arc(e, pl, 2500, 0x400)) {
                if (e->anim_frame == 0x0c) {
                    pl->hp = (int16_t)(pl->hp - 10);            /* player.hp -= 10 (2nd -10 site) */
                    if (pl->hp < 0) pl->hp = 1;
                    re15_audio_room_se(7); pl->hit_react |= 1;
                }
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 5:   /* GRAB-AND-HOLD/THROW (sub 5 0x80117d30): clip 0xb. NON-damaging grab-throw — latches the
                   * player grab-cmd DAT_800aca58=6 at the contact frame (+0x95==0x2c @0x80117ee4, via the
                   * attack-box overlap); the throw+damage is the shared player grab-FSM (cmd 6). -> HUB. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            if (e->motion != 0x0b) re15_birkin_clip(e, 0x0b);
            if (e->anim_frame == 0x2c && re15_dog_arc(e, pl, 2500, 0x400)) {
                s_player_grabbed = 1; pl->hit_react |= 1; re15_audio_room_se(2);   /* grab-throw latch (cmd 6) */
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 6:   /* THROW FOLLOW-UP (sub 6 0x80117f80): clip 0x14, NO damage (mesh/throw anim) -> HUB */
            if (e->motion != 0x14) re15_birkin_clip(e, 0x14);
            if (re15_birkin_anim(e)) { e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 7:   /* TACKLE (sub 7 0x801188f8): clip 5, a LUNGING body-slam from up to 3800. The boss charges
                   * forward (the sub-7 movement advances the body @0x80118a10) and the attack-box overlap
                   * (0x8001a5e0) connects on the TWO windows ([4..13]/[18..27] @0x80118ab0) -> player.hp -= 5
                   * (@0x80118b1c) + keep-alive clamp (@0x80118b38) + latch aca58=0x202 -> HUB. The forward
                   * lunge speed is faithful-line (exact scale via the 0x80118a10 pos-advance). */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x50);
            re15_dog_advance(e, 0x50);                          /* lunge forward into the tackle */
            if (((e->anim_frame >= 4 && e->anim_frame <= 13) || (e->anim_frame >= 18 && e->anim_frame <= 27))
                && pl->hit_react == 0 && re15_dog_arc(e, pl, 2500, 0x400)) {
                if (e->anim_frame == 4 || e->anim_frame == 18) {
                    pl->hp = (int16_t)(pl->hp - 5);             /* player.hp -= 5 @0x80118b1c */
                    if (pl->hp < 0) pl->hp = 1;                 /* keep-alive clamp @0x80118b38 */
                    re15_audio_room_se(4); pl->hit_react |= 1;
                }
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 8:   /* REPOSITION (sub 8 0x80118b58): clip 1, NO damage, a short (0x78=120) approach -> GRAB (sub 5) */
            if (e->motion != 1) re15_birkin_clip(e, 1);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x20);
            re15_dog_advance(e, 40);
            if (re15_birkin_anim(e)) { e->sub_state_1 = 5; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 10:  /* CHARGE-COMBO (sub 10 0x80119524): 14-step, clip 1(charge)->0xf(atk A)->0x13(atk B). The
                   * charge damage is via the shared body-contact hitbox (no direct hp write in the handler).
                   * Fast charge toward the player -> LUNGE (sub 3). */
            if (e->motion != 1) re15_birkin_clip(e, 1);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);    /* charge steer 0x30 */
            re15_dog_advance(e, 80);                          /* fast charge (faithful; exact speed via 0x80119xxx) */
            if (re15_birkin_anim(e) || dist < 1500) { re15_birkin_clip(e, 3); e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* -> LUNGE */
            break;
        default:
            e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
            re15_birkin_anim(e);
            break;
        }
        break;
    }

    case 2:   /* HURT / super-armor FLINCH (0x8011a3f0): a non-lethal hit while the boss is in an armored
               * action (+0x7==0 && +0x1dd&8 @0x8011a43c/450) plays a gore flinch (+0x1de=9 @0x8011a458,
               * blood FX via 0x80019700 by bone) and RESUMES the saved pre-hit action (+0x4 = *(+0x1d8)
               * @0x8011a560-568). The lethal case clears the armor flag in the damage entry so it falls to
               * DEATH instead — so mapping (state 2 = resume, state 3 = die) is byte-true-equivalent. Resume
               * to the HUB (sub 1), NOT re-emergence: the boss returns to fighting. */
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH down-machine (0x8011a56c): the lethal path sets the downed flags (+0x1b8 |= 0x12
               * @0x8011a584) and runs the clip-9 topple (the +0x5/+0x6 sub-table @0x8011f1e4 dispatch, whose
               * down-machine 0x8011a5d8 sets +0x94 = 9 @0x8011a634) -> settle to CORPSE. */
        if (e->motion != 9) re15_birkin_clip(e, 9);            /* clip 9 = death topple @0x8011a634 */
        if (re15_birkin_anim(e)) { e->state = 7; e->sub_state_3 = 0; }   /* topple done -> CORPSE */
        break;

    case 7:   /* CORPSE: settle, inert */
        re15_birkin_anim(e);
        break;

    default:
        e->state = 1; e->sub_state_1 = 9; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ ROOTED WRITHE-HAZARD (type 0x1a, EM01A) — STAGE1 ================ *
 * Byte-true from workflow wf_5c34ffe7 (root 0x8010c1ec, state table @0x8012093c). A ROOTED, ground-
 * emergent, segmented WRITHING contact-hazard: it NEVER walks/chases (zero steer 0x8001aac4 / yaw-slew
 * 0x8001a8f8 / atan2 0x8001a6d4 calls anywhere in 0x8010c014..0x8010d914), only bobs vertically and
 * rotates in place. It is UNKILLABLE — the handler sets NO HP (+0x9a) and has NO hurt/death/corpse
 * states. It does NO player damage — no write to player.hp (0x800acaee), no grab (DAT_800aca58), no
 * shared damage-entry (0x80012d60); it only arms the shared body-contact flag (+0x1b8 |= 0x12
 * @0x8010c2d8) which pushes the player out (0x8003b0a4). So the port models: INIT (record spawn pos +
 * rng writhe timers, NO HP) -> ACTIVE writhe: emerge from ground when buried (grid & 0x80 -> clip 0x1f
 * @0x80107d2c), else loop the in-place writhe clips 0/1/2 with rng-jittered timers. The exact 16-grid-
 * variant dual-table + 2D sub-brain writhe choreography is faithful-line (cosmetic; cited above). */
static void re15_writher_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x8010c33c: record player spawn pos (+0x1bc/+0x1be), timers, NO HP. -> ACTIVE. */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;   /* +0x1bc/+0x1be @0x8010c368/380 */
        e->ai_timer = 0x14;                                          /* +0x9c = 0x14 @0x8010c3ac */
        e->anim_frac = 7;                                           /* +0x8f blend seed (writhe crossfade) */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        /* buried spawn (grid bit 0x80) emerges from the ground first (clip 0x1f), else writhe idle */
        if (e->grid_id & 0x80) { e->motion = 0x1f; e->anim_frame = 0; e->sub_state_1 = 1; }  /* emerge @0x80107d2c */
        else                   { e->motion = 0;    e->anim_frame = 0; e->sub_state_1 = 0; }
        e->state = 1;                                               /* +0x4 = 1 @0x8010c350 */
        break;

    case 1: {  /* ACTIVE writhe (0x8010c488): rooted — X/Z (+0x34/+0x3c) are NEVER advanced. Emerge if
                * buried, then loop the in-place writhe clips with rng-jittered dwell timers. */
        (void)pl;
        if (e->sub_state_1 == 1) {          /* EMERGE from ground: play clip 0x1f once -> writhe idle */
            if (++e->anim_frame >= 40) { e->sub_state_1 = 0; e->motion = 0; e->anim_frame = 0; }
        } else {                            /* WRITHE: cycle the twitch clips 0/1/2 on a jittered timer */
            if (e->ai_timer > 0) e->ai_timer--;
            if (e->ai_timer == 0) {
                e->ai_timer = (int16_t)((re15_engine_rand8() & 0x1f) + 30);   /* +0x1d0 = (rng&0x1f)+30 @0x8010c3fc */
                e->motion = (uint8_t)((e->motion >= 2) ? 0 : e->motion + 1);  /* writhe clips 0/1/2 (faithful) */
                e->anim_frame = 0; e->anim_frac = 7;
            } else {
                e->anim_frame++;            /* twitch anim advances in place */
            }
        }
        break; }

    default:  /* UNKILLABLE: it has no hurt/death state. If the damage entry ever forced state 2/3,
               * snap it back to writhing (byte-true: 0x1a can never leave the writhe machine). */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        break;
    }
}

/* ============================ ALLIGATOR boss (type 0x23, EM023) — STAGE2 sewer ================ *
 * Byte-true from workflow wf_5c34ffe7 (root 0x8010c448, state table @0x80118bc8). A giant ground
 * WALK-CHASER whose ONE attack is a lunging GRAB-and-EAT (jaws). HP 300 (type row @0x801175dc = 16x
 * 300, RNG-invariant). The grab does NO chip hp write in the handler (the report's player.hp touches
 * are redundant self-stores); it latches the shared grab-command DAT_800aca58 = 2 (in-jaws) and
 * escalates to = 3 ("eaten"/swallow = death) — @0x8010d27c/@0x8010d288 (sub 3) and @0x8010e120/
 * @0x8010e130 (sub 9). So the port models the grab as a MASH-window jaws-hold that SWALLOWS (kills)
 * the player when the hold expires. Locomotion is the shared walker (steer 0x8001a804 + pos_advance
 * 0x800245d8); the exact directional walk-clip LUT (@0x80118d44) + dual-table decision choreography
 * are faithful-line (the grab mechanism + HP + ranges are byte-true). */
static void re15_alligator_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x8010c56c: HP 300, clip 4, state 1 sub 6, mode +0x1e0. Grid-odd = melee. */
        if (e->hp <= 0) e->hp = 300;                 /* +0x9a from row @0x801175dc (16x300) @0x8010c6d4 */
        e->motion = 4; e->anim_frame = 0; e->anim_frac = 7;   /* clip 4 @0x8010c590 */
        e->hit_react = 0; e->ai_timer = 0;
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
        if (e->grid_id & 1) { e->sub_state_1 = 0; e->dog_atk_cd = 0; }   /* grid-odd = melee (+0x1e0=0) @0x8010c620-region */
        else                { e->sub_state_1 = 6; e->dog_atk_cd = 1; }   /* default = ranged/approach (+0x1e0=1, sub 6) */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->state = 1;                                /* +0x4 = 0x601 -> state 1 @0x8010c580 */
        break;

    case 1: {  /* ACTIVE 0x8010c860: walk-chase + decision hub (dist-gated). Dual A/B brain on +0x5. */
        int32_t dist = re15_enemy_player_dist(e, pl);
        e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 6: case 0: default:   /* APPROACH / decision HUB (A[6] init 0x8010da0c / A[4] 0x8010d3a4):
                                    * walk toward the player; when in jaws range + facing -> LUNGE-GRAB. */
            if (e->motion != 4) { e->motion = 4; e->anim_frame = 0; }        /* directional walk clip (faithful) */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);                    /* yaw-slew 0x8001a8f8 */
            re15_dog_advance(e, 48);                                         /* pos_advance 0x800245d8 */
            e->anim_frame++;
            if (pl->hit_react == 0 && dist < 3000 && re15_dog_arc(e, pl, 3000, 0x338)) {  /* commit range 0xbb8 @A[4] */
                e->motion = 3; e->anim_frame = 0; e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 3:   /* LUNGE-GRAB (B[3] 0x8010cfbc): lunge into the jaws-reach; on connect latch the grab
                   * (DAT_800aca58=2) and hold -> SWALLOW (eaten) at hold-end. Reach 800 @0x8001bff8. */
        case 9:   /* GRAB (B[9] 0x8010df34): the same jaws-grab, second site. Route through here. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            if (e->sub_state_3 == 0) {                       /* lunge windup: close into reach */
                re15_dog_advance(e, 64);
                if (e->anim_frame >= 3 && dist <= re15_body_contact_reach(e) && re15_dog_arc(e, pl, re15_body_contact_reach(e), 0x400) && !s_player_grabbed) {   /* jaws reach at body contact (was 1400 < push standoff 2650; audit wf_555f18eb Part B) */
                    s_player_grabbed = 1; pl->hit_react |= 1;      /* grab latch cmd 2 (in-jaws) @0x8010d27c */
                    e->ai_timer = 100;                             /* +0x1dc hold timer = 100 @0x8010d270 */
                    e->sub_state_3 = 1; re15_audio_room_se(2);
                }
                if (++e->anim_frame >= 40) { e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* missed -> HUB */
            } else {                                          /* JAWS HOLD -> swallow (eaten) at hold-end */
                s_player_grabbed = 1;                          /* keep the player pinned */
                if (e->ai_timer > 0) e->ai_timer--;
                if (e->ai_timer == 0) {                        /* SWALLOW: eaten -> death (cmd 2 -> 3 @0x8010d288) */
                    if (pl->hp >= 0) pl->hp = -1;              /* player.hp<0 -> DAT_800aca58=3 (eaten) */
                    s_player_grabbed = 0;
                    e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;
                }
            }
            break;
        }
        break; }

    case 2:   /* HURT (take_damage +0x4=2): flinch -> resume the approach/decision hub */
        e->state = 1; e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH (take_damage +0x4=3): -> corpse. Exact death-topple clip = faithful-line. */
        e->state = 7; e->sub_state_3 = 0;
        break;

    case 7:   /* CORPSE: settle, inert */
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
 * (no player.hp write, no grab, no damage-entry, no hitbox anywhere in 0x8010ee9c..0x80110a00). Its
 * ACTIVE loop is an accelerating leftward X-DRIFT that wraps, plus a 3-level (+0x4/+0x5/+0x6) phase
 * sequencer firing timed particle bursts (0x80019700). The port models the byte-true gameplay:
 * INIT (no HP, state 1) + the X-drift/wrap; the particle-burst choreography is faithful-line (the
 * EM024 sprite belongs to the ESP/particle subsystem). */
static void re15_fx_emitter_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    (void)pl;
    switch (e->state) {
    case 0:   /* INIT 0x8010ef1c: no HP, disable own collision, drift timer 120 -> ACTIVE. */
        e->hit_react = 1;                        /* +0x93 = 1 @0x8010ef58 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0;   /* clip 0 via FUN_8011089c @0x801108a8 */
        e->ai_timer = 0x78;                      /* +0x8c drift timer = 120 @0x8010ef6c */
        e->sub_state_1 = e->grid_id;             /* +0x5 = grid @0x8010f00c */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->state = 1;                            /* +0x4 = 1 @0x8010eff4 */
        break;
    case 1:   /* ACTIVE 0x8010f020: accelerating leftward X-drift + timed FX bursts (faithful). No damage. */
        e->ai_timer++;                           /* timer++ @0x8010f0c8 */
        e->x -= e->ai_timer;                     /* X -= timer @0x8010f0e8 */
        if (e->x < -25000) { e->x = 20000; e->ai_timer = 0xe1; }   /* wrap @0x8010f100-11c (0x4e20/0xe1) */
        e->anim_frame++;                         /* the phase sequencer would fire 0x80019700 bursts here */
        break;
    default:
        e->state = 1;
        break;
    }
}

/* ============================ TYRANT boss (type 0x2b, EM02B) — STAGE4/5 ======================= *
 * Byte-true from workflow wf_5c34ffe7 (root 0x801118d0, state table @0x8011a0b4). A bipedal ground
 * WALK-CHASER with an EMERGE intro, TWO -10 melee attacks and two grab-PINs; real hurt/death (a
 * killable prototype Tyrant, HP pool 86..126). Uses the generic bank clip/anim helpers (re15_birkin_
 * clip/anim are type-parameterized: they look up EM02B's loaded bank). Byte-true damage/HP/ranges;
 * the exact 16-sub dual-table walk choreography + emerge clip are faithful-line. */
static const uint16_t s_tyrant_hp[16] =    /* HP pool @0x80118b00 (index = rng & 0xf) */
    { 86, 89, 103, 119, 91, 107, 121, 93, 109, 124, 117, 97, 113, 126, 99, 101 };

static void re15_tyrant_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x80111a64: HP from the pool; grid-variant EMERGE (0x40/0x41/0x43 -> state 4). */
        if (e->hp <= 0) e->hp = (int16_t)s_tyrant_hp[re15_engine_rand8() & 0xf];   /* +0x9a @0x80111c04 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;      /* clear +0x94/95/8f/93 */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;                   /* +0x1bc/+0x1be */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        if (e->grid_id == 0x40)      { e->state = 4; e->sub_state_1 = 0; }          /* @0x80111b5c */
        else if (e->grid_id == 0x41) { e->state = 4; e->sub_state_1 = 1; }          /* @0x80111b7c */
        else if (e->grid_id == 0x43) { e->state = 4; e->sub_state_1 = 2; }          /* @0x80111ba0 */
        else                         { e->state = 1; e->sub_state_1 = 0; }          /* +0x4 = 1 @0x80111a78 */
        break;

    case 1: {  /* ACTIVE 0x80111c98: walk-chase + dual A/B brain on +0x5. */
        int32_t dist = re15_enemy_player_dist(e, pl); e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 0: default:  /* APPROACH + DECIDE (A[3] 0x801123e8 / A[13] 0x80113c74): close lunge in <2000,
                           * near attack in <2501 (@0x80112458/0x80113c88), else walk toward the player. */
            if (e->motion != 1) re15_birkin_clip(e, 1);
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);            /* yaw-slew 0x8001a8f8 */
            re15_dog_advance(e, 40);                                 /* pos_advance 0x800245d8 */
            if (pl->hit_react == 0 && dist < 2000 && re15_dog_arc(e, pl, 2000, 0x300)) {   /* -> ATTACK2 (sub 14) */
                re15_birkin_clip(e, 5); e->sub_state_1 = 14; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            else if (pl->hit_react == 0 && dist < 2501 && re15_dog_arc(e, pl, 2501, 0x300)) {  /* -> ATTACK1 (sub 4) */
                re15_birkin_clip(e, 4); e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            re15_birkin_anim(e);
            break;
        case 4:   /* ATTACK1 near bite/claw (B[4] 0x80112840): player.hp -= 10 (@0x80112898) on the window
                   * (+0x95<10 @0x80112854). If the hit drops the player below 50 -> GRAB1 (sub 5) @0x801128ac. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            if (e->sub_state_2 == 0 && e->anim_frame >= 3 && e->anim_frame < 10 && re15_dog_arc(e, pl, 2500, 0x400)) {
                pl->hp = (int16_t)(pl->hp - 10);                    /* player.hp -= 10 @0x80112898 (no clamp) */
                re15_audio_room_se(4); pl->hit_react |= 1; e->sub_state_2 = 1;
                if (pl->hp < 50 && pl->hp >= 0) { re15_birkin_clip(e, 6); e->sub_state_1 = 5; e->sub_state_2 = 0; e->sub_state_3 = 0; break; }  /* -> GRAB1 */
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 14:  /* ATTACK2 close lunge (B[14] 0x80113f00): player.hp -= 10 (@0x80113ff8), window +0x95<11. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
            if (e->sub_state_2 == 0 && e->anim_frame >= 3 && e->anim_frame < 11 && re15_dog_arc(e, pl, 2500, 0x400)) {
                pl->hp = (int16_t)(pl->hp - 10);                    /* player.hp -= 10 @0x80113ff8 */
                re15_audio_room_se(4); pl->hit_react |= 1; e->sub_state_2 = 1;
            }
            if (re15_birkin_anim(e)) { e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 5:   /* GRAB1 (A[5] 0x80112a5c, 7-phase @0x8010026c): LATCH grab-cmd DAT_800aca58=5 (PIN)
                   * @0x80112b34 + hold (+0x9c=100), later collapse (cmd 6 @0x80112ea4). No grab hp subtract
                   * (the throw damage is the shared cmd 5/6 FSM = faithful-line). -> back to APPROACH. */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            if (e->sub_state_3 == 0) { s_player_grabbed = 1; pl->hit_react |= 1; e->ai_timer = 100; e->sub_state_3 = 1; re15_audio_room_se(2); }
            else {
                s_player_grabbed = 1;
                if (e->ai_timer > 0) e->ai_timer--;
                if (e->ai_timer == 0) { s_player_grabbed = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* collapse -> release */
            }
            break;
        }
        break; }

    case 2:   /* HURT (0x80114770): stagger -> resume ACTIVE at sub 6 (byte-true +0x4=0x601 @0x801147d8;
               * the stagger clip is a +0x5 sub-dispatch = faithful-line — the HUB owns sub 6 == sub 0). */
        e->state = 1; e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH (0x80114c68 -> single handler 0x80114cb0): play the topple clip 8 (+0x94=8
               * @0x80114d28, +0x93|=2 gore @0x80114d08) to its end -> CORPSE. */
        if (e->motion != 8) re15_birkin_clip(e, 8);        /* death clip 8 (generic bank clip/anim) */
        if (re15_birkin_anim(e)) { e->state = 7; e->sub_state_3 = 0; }
        break;

    case 4:   /* EMERGE (0x80114fe4, state-4 sub-table @0x8011a270): play the emerge anim (sub2 0x801155ac
               * fires 8x FX-spawn dust) -> go ACTIVE (+0x4=0x101). Exact emerge clip = faithful-line. */
        if (e->motion != 0x10) re15_birkin_clip(e, 0x10);
        if (re15_birkin_anim(e)) { e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }
        break;

    case 7:   /* CORPSE: settle, inert */
        re15_birkin_anim(e);
        break;

    default:
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
        break;
    }
}

/* ============================ IVY plant-grappler (type 0x2d, EM02D) — STAGE4 lab ============== *
 * Byte-true from workflow wf_5c34ffe7 (root 0x801168c4, state table @0x8011a2c0). A humanoid plant
 * grappler (HP 100 @0x80116954): nav-walks/chases on the shared EXE humanoid walker library, emerges/
 * idles with a procedural bone-sway, and its ONE attack is a GRAB -> EATEN-DEATH. The grab does NO
 * chip hp (the report found no player.hp subtract); it drives the player straight into the death FSM
 * via the shared grab-command DAT_800aca58 = 7 (instant kill, @0x80116858, the 5-stage grab VM
 * @0x801003c4). So the port models: INIT (HP 100) -> nav-chase -> GRAB (pin + eaten-death). Killable
 * (real hurt/death). The dual-table walk/look choreography + bone-sway are faithful-line. */
static void re15_ivy_ai_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    switch (e->state) {
    case 0:   /* INIT 0x80116920: HP 100, clip 0, state 1 (ACTIVE). */
        if (e->hp <= 0) e->hp = 100;              /* +0x9a = 0x64 @0x80116954 */
        e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;   /* clip 0 @0x8011698c */
        e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->state = 1;                             /* +0x4 = 1 @0x80116988 */
        break;

    case 1: {  /* ACTIVE 0x801169b8: nav-chase the player; in body-contact range -> GRAB. */
        int32_t dist = re15_enemy_player_dist(e, pl); e->dog_dist = (int16_t)dist;
        switch (e->sub_state_1) {
        case 0: default:   /* NAV-CHASE (walker 0x800245d8 + pathfind 0x800509e4(15000)) */
            if (e->motion != 1) { e->motion = 1; e->anim_frame = 0; }
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            re15_dog_advance(e, 40);
            e->anim_frame++;
            if (pl->hit_react == 0 && dist < 1400 && re15_dog_arc(e, pl, 1400, 0x400)) {   /* body AABB 0x2c8 reach -> GRAB */
                e->motion = 2; e->anim_frame = 0; e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; }
            break;
        case 3:   /* GRAB -> EATEN-DEATH (5-stage @0x801003c4): lunge to contact, latch the pin + drive the
                   * player death FSM (DAT_800aca58=7 @0x80116858) — an instant kill (no chip damage). */
            re15_enemy_steer_point(e, pl->x, pl->z, 0x30);
            if (e->sub_state_3 == 0) {                       /* lunge into the grab */
                re15_dog_advance(e, 48);
                if (e->anim_frame >= 3 && dist < 900 && re15_dog_arc(e, pl, 900, 0x400) && !s_player_grabbed) {
                    s_player_grabbed = 1; pl->hit_react |= 1;    /* grab latch @0x80116850-58 */
                    re15_audio_room_se(2); e->ai_timer = 60; e->sub_state_3 = 1;
                }
                if (++e->anim_frame >= 40) { e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0; }  /* missed -> chase */
            } else {                                          /* HOLD -> eaten (cmd 7) = player death */
                s_player_grabbed = 1;
                if (e->ai_timer > 0) e->ai_timer--;
                if (e->ai_timer == 0) {                       /* EATEN-DEATH: DAT_800aca58=7 -> instant kill */
                    if (pl->hp >= 0) pl->hp = -1;
                    s_player_grabbed = 0;
                    e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
                }
            }
            break;
        }
        break; }

    case 2:   /* HURT (take_damage +0x4=2): flinch -> resume the chase */
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hit_react = (uint8_t)(e->hit_react & ~1u);
        break;

    case 3:   /* DEATH (take_damage +0x4=3): -> corpse */
        e->state = 7; e->sub_state_3 = 0;
        break;

    case 7:   /* CORPSE: settle, inert */
        e->anim_frame++;
        break;

    default:
        e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0;
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
             * 0x18={71,93,75,..}). Types 0x1c/0x1d/0x1e/0x1f are ALSO registered to this root but their
             * HP rows are all-zero = unused registration slots, so they are deliberately NOT routed
             * (a spawned one would be inert). Adding 0x12/0x18 is safe for the tested rooms: ROOM1140/
             * 1170 contain only 0x10/0x11/0x16, so their combat is unchanged. */
            int32_t sweep_ox = e->x, sweep_oz = e->z;    /* pre-dispatch pos (wall-sweep origin) */
            re15_enemy_ai_live_step(s);
            re15_enemy_anim_sfx(e);        /* FUN_8001b38c: per-frame clip-flag SFX (footsteps/attack) */
            re15_enemy_hurt_fx(e);      /* FUN_80105a8c/FUN_80105b7c hurt -> effect-0 hit blood (visible) */
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
        else if (t == 0x20) {   /* DOG (Cerberus, type 0x20) — ground chase/bite AI (Wave 1).
                                 * Ground enemy: SCA wall-clamp after the tick (byte-true 0x8003b0a4). */
            int32_t dog_ox = e->x, dog_oz = e->z;
            e->ai_contact = (uint8_t)(e->ai_contact & 0xf0);  /* the SCA resolver clears the contact bit each frame (@0x8003b1dc) */
            re15_dog_ai_tick(s);
            re15_enemy_body_push_tail(s, e);                  /* aec4+b544 body separation (dog root tail) */
            if (g_room_rdt_ok && (e->x != dog_ox || e->z != dog_oz)) {
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
        else if (t == 0x29) {   /* COCKROACH (type 0x29, EM029, STAGE3) — small maggot-like scurrier (Wave 1).
                                 * Scurries toward the player + bites -> SCA wall-clamp after the tick. */
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
        else if (t == 0x27) {   /* MAGGOTS (type 0x27) — large moving ground creature (Wave 1).
                                 * Crawls toward the player -> SCA wall-clamp after the tick like the dog. */
            int32_t mag_ox = e->x, mag_oz = e->z;
            re15_maggot_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != mag_ox || e->z != mag_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, mag_ox, mag_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
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
        else if (t == 0x13) {   /* ZOMBIE GIRL (type 0x13) — nav-pathing zombie variant (Wave 1).
                                 * Ground enemy: SCA wall-clamp after the tick like the dog/zombie. */
            int32_t zg_ox = e->x, zg_oz = e->z;
            re15_zgirl_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != zg_ox || e->z != zg_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, zg_ox, zg_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        else if (t == 0x1a) {   /* ROOTED WRITHE-HAZARD (type 0x1a, EM01A) — anchored, unkillable, no
                                 * damage. NO wall-clamp: it never advances X/Z, so there is nothing to
                                 * constrain (byte-true: zero locomotion primitives in the handler). */
            re15_writher_ai_tick(s);
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
        else if (t == 0x2d) {   /* IVY plant-grappler (type 0x2d, EM02D, STAGE4) — nav-chasing humanoid +
                                 * grab-instakill. SCA wall-clamp after the tick like the other walkers. */
            int32_t iv_ox = e->x, iv_oz = e->z;
            re15_ivy_ai_tick(s);
            re15_enemy_body_push_tail(s, e);
            if (g_room_rdt_ok && (e->x != iv_ox || e->z != iv_oz)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, iv_ox, iv_oz, &nx, &nz, e->hit_radius_min, e->y);
                e->x = nx; e->z = nz;
            }
        }
        /* type 0x22 (EM022, STAGE2 root 0x8010c080) is a VERIFIED STUB (wf_5c34ffe7): a scaffolded
         * state machine whose every dispatch leaf is a `jr ra` no-op — NO HP, NO clip, NO locomotion,
         * NO attack, NO player.hp write. The registered-but-unimplemented pattern (like G-Birkin
         * form-3 0x33). Byte-true = deliberately UNROUTED: it spawns and sits inert (its only real code
         * is the shared cutscene model-draw aux, which the port's renderer already covers). */
    }
}
