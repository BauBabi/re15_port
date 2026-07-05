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
#include "re15_room.h"      /* g_room_rdt for the enemy wall clamp + the nav-zone graph */
#include "re15_damage.h"   /* re15_enemy_player_dist, re15_ai_arc_test, re15_engine_rand8,
                            * re15_enemy_apply_hitbox */
#include "re15_skeleton.h" /* re15_sin_q12 / re15_cos_q12 — forward-walk root-motion step (8.19) */
#include "re15_actor.h"    /* re15_atan2_q12 — heading toward the player for the approach/walk */
#include "re15_anim_select.h" /* re15_compute_actor_kf — current keyframe for the walk root-motion */
#include "re15_emd.h"      /* re15_emd_get_keyframe_speed — the walk clip's per-frame root translation */

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
    /* func_0x8001bd60(-10,20) / func_0x80039e7c(&player,0,0) setup helpers — deferred. */

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
 * → the player is free. DEFERRED (cited): the per-type grabbed POSE/anim, the exact XZ/Y pin
 * (DAT_800acc0e), the struggle-escape (sub-step 5 @0x80102968, anim-gated + the bit-0x2 check),
 * and the cmd-6 "being-approached" walk command (coupled to the deferred forward-walk). */
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

/* The zombie's THROW-OFF [4] starts the player's release finish in lockstep (byte-true: the grab's
 * escape path writes DAT_800aca5a = 4 = the struggle FSM's release phase; clip base+2). */
void re15_player_victim_throwoff(void)
{
    if (g_player_victim != 1) return;
    re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_player_victim = 3;
    player->motion = (uint8_t)(g_player_victim_variant * 3 + 2);
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
    uint8_t base = (uint8_t)(g_player_victim_variant * 3);
    if (g_player_victim == 1) {                         /* STRUGGLE (state 5, @0x8010a28c 6-phase machine) */
        if (!s_player_grabbed) {                        /* grab ended alive -> RELEASE finish (phases 4/5:
                                                         * clip base+2 ONCE, then restore the free player) */
            g_player_victim = 3;
            player->motion = (uint8_t)(base + 2);
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
        uint8_t clip = (uint8_t)(base + (s_victim_phase < 1 ? 0 : 1));
        int fc = (clip < vb->anim_victim.clip_count) ? vb->anim_victim.clips[clip].frame_count : 1;
        if (fc < 1) fc = 1;
        if (!s_victim_fresh &&
            ++player->anim_frame >= fc) {              /* clip-done -> intro advances, hold clip LOOPS
                                                        * (advance SKIPPED on the latch tick: frame 0 is
                                                        * posed first — byte-true f314 post-increment) */
            player->anim_frame = 0;
            if (s_victim_phase < 1) s_victim_phase++;
            clip = (uint8_t)(base + (s_victim_phase < 1 ? 0 : 1));
        }
        s_victim_fresh = 0;
        player->motion = clip;
        re15_clip_root_motion_abs(player, &vb->skel_victim, &vb->anim_victim,
                                  clip, (int)player->anim_frame);
    } else if (g_player_victim == 2) {                 /* COLLAPSE (state 6): clip variant+6, play once +
                                                        * ROOT MOTION (the ~600-unit devour drag), hold last */
        uint8_t clip = (uint8_t)(g_player_victim_variant + 6);
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
         * (every kill save reads exactly -1). */
        if (player->anim_frame == 0x23) {
            player->hp    = -1;
            player->state = 7;                          /* the port's death FSM keys off hp<0/state 7 */
        }
        /* frame 0x37=55: the big BLOOD burst at the player pos+yaw (byte-true FUN_8010a6f8:
         * FUN_80019700(0x2000) = effect-id 0 — the SAME spawn the hurt-fx uses) + its SE
         * Se_on(0x4030001) = CORE bank4 record 3 (Se_on RE'd: FUN_80045024 top byte = bank,
         * bank4 = the resident CORE00.EDH table @0x801fbd00, RAM-matched). The frame-0x23 chomp
         * Se_on(0x2070001) is BANK 2 (*(DAT_800ac778+8), a room-state pointer) — still deferred. */
        if (player->anim_frame == 0x37) {
            re15_esp_fx_spawn(re15_esp_room_bank(), 0, 0,
                              player->x, player->y, player->z, (int16_t)player->rot_y);
            re15_audio_core_se(3);
        }
    } else {                                           /* RELEASE finish (state 3): clip base+2 once -> free.
                                                        * (Entered via re15_player_victim_throwoff — the
                                                        * zombie holds sub-steps [4..7] while this plays,
                                                        * so s_player_grabbed stays latched; no re-enter.) */
        uint8_t clip = (uint8_t)(base + 2);
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
 * DEFERRED (cited): the player-pos snapshot (+0x1bc/+0x1be, consumed by the deferred movement
 * decision), the RNG seed (FUN_8001af20 @0x80100774), and the two const GTE vectors copied from
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
    /* seed the per-spawn HURT stagger clip (FUN_80100688 @0x80100774-9c): +0x1d4 =
     * seed_table[rng()&7], table @0x8011f7e4 = {2,3,4,5,2,3,4,5} -> a random clip in {2,3,4,5}. */
    {
        static const uint8_t hurt_clip_seed[8] = { 2, 3, 4, 5, 2, 3, 4, 5 };
        e->hurt_clip = hurt_clip_seed[re15_engine_rand8() & 7];
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
        } else if (sel == 1 || sel == 3) { /* +0x5=5, +0x94=0xc @0x80100d54 */
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
 * cycles). DEFERRED (cited): the player-grabbed pose/lock FSM (the player command register
 * 0x800aca58 = cmd 5 -> LAB_80036834, which pins + animates the player) + the player+0x93|=1 grabbed
 * flag (port-field-aliased to the hit-guard) + the grab motion +0x94 + the grab-link globals
 * 0x800acbcc/d0 — those are the player subsystem + the anim layer. The byte-true -10/-5 HP damage +
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
                 *  +0x9c ESCAPE window: -= 1 + 5*mash-presses (FUN_80037024; mash deferred = -1/tick);
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
 * called TWICE @0x801030dc/f0) with hard 0x40/tick homing (aac4, no weave) for (rand&0x3f)+0x96
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
 * forward locomotion is the deferred anim-root-motion walk in the +0x5=5/6 states). The companion
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
    if (turn == 0)
        re15_ai_set_state_word(e, 0x201);                                     /* +0x5 = 2 (engage) */
        /* a8f8(&player, 0x80) @0x80102e90: within the +-0x80 cone the slew SNAPS -> the zombie
         * leaves the turn facing the player EXACTLY (feeds the next walk segment's geometry). */
        e->rot_y = (int16_t)(((int)re15_atan2_q12(player->z - e->z, player->x - e->x) - 0x400) & 0x0fff);
        {
            int ts = (int)(e - g_actors);                       /* exit rand writes (lines 33-36) */
            s_wander_mag[ts] = (uint8_t)((re15_engine_rand8() & 0x1f) + 8);   /* +0x9e */
            s_wander_idx[ts] = (uint8_t)((re15_engine_rand8() & 1) + 1);      /* +0x9f */
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
    int64_t r = v, last = 0;
    /* Newton, converges in <8 iters for our <2^32 magnitudes; exactness beyond the original's
     * SquareRoot0 rounding is immaterial (the push is a per-frame relaxation). */
    for (int i = 0; i < 24 && r != last; i++) { last = r; r = (r + v / r) >> 1; }
    return (int32_t)r;
}

int re15_body_push(const re15_actor_t *pusher, int32_t r_pusher,
                   re15_actor_t *pushee, int32_t r_pushee)
{
    int32_t dx = (int32_t)(int16_t)(pushee->x - pusher->x);   /* s16-truncated like the original */
    int32_t dz = (int32_t)(int16_t)(pushee->z - pusher->z);
    int32_t R  = r_pusher + r_pushee;
    if (dx > R || dx < -R || dz > R || dz < -R) return 0;     /* fast reject (aec4 pre-tests) */
    int32_t dist = re15_body_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
    int32_t pen  = R - dist;
    if (pen < 1) return 0;                                    /* @aec4: pen<1 -> no hit */
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
 * movement/decision tail (FUN_80101224 @0x80101560+, jal FUN_8001bc08) — DEFERRED; when not armed
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
         * feeding handler is the dist-gated WAKE-UP that transitions them to combat (sub 0). DEFERRED
         * (cited): the FUN_8001bc08 sensor + the +0x1d8 update, and the ATTACK-ARM (+0x1d8 |= 0x100 +
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
                 * (The grab/turn/engage animate clips are ported [8.13]; the +0x5=0/1 search anim +0x5=5/6
                 * forward walk are RE'd byte-true [search clip 0/1, walk clip +0x5+4] but NOT reached
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
            case 7: case 8:   /* lying (@0x8011f80c[7]/[8]=0x80101974): stage-A is an empty `jr ra` —
                               * no dist gate; the lying zombie stays passive until externally nudged. */
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

    /* SPECIAL branch (@0x80105aa8 +0x9&0x80): killed-mid-reaction collapse — DORMANT for ROOM1140.
     * Mirror the exit gate's bne-stay (@0x80105b40): hold HURT, never recover (fixed clip deferred). */
    if (e->grid_id & 0x80) return;

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
        s_wander_mag[slot2] = (uint8_t)((re15_engine_rand8() & 0x1f) + 8);   /* +0x9e */
        {
            const uint8_t *tbl = (re15_enemy_live_count() >= 5) ? s_zbehavior_5plus : s_zbehavior_lt5;
            uint8_t beh = tbl[re15_engine_rand8() & 0x1f];                    /* +0x1de */
            if (beh == 2) e->sub_state_1 = 0x13;
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
 * FAITHFUL-LINE: the death SE (frame 7) + the gore spawn (frame 35) are the deferred presentation.
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
 * then dispatch the main state @0x8011f7b4[entity+0x4] (INIT/ACTIVE ported; [2]/[3]/[4] deferred).
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

/* Phase 8.6 — the per-frame LIVE-zombie AI pass. The port's faithful, TYPE-GATED slice of the
 * original entity-update loop FUN_8001a50c (@0x8001ce04): the original walks the entity array
 * (DAT_800acc2c, stride 0x1f4) and, for every active entity (+0x0 & 1), dispatches its per-type
 * tick @0x80072bac[entity+0x8 type]. The port has the player on its own path (re15_player_tick)
 * and only the LIVE STAGE1 zombie types (0x10/0x11/0x16) ported here, so this runs JUST those
 * through re15_enemy_ai_live_step (FUN_80100424 tick + the shared lunge slice). Every other type
 * (Elliot 0x47, crows 0x21, room props) is left to its existing handling. Because of the type
 * gate, a room with no live zombie (e.g. the ROOM1170 boot/helipad) makes this a pure no-op =
 * no 1170 regression. `combat_active` is forwarded to the arm gate (DAT_800aca3c & 1). */
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
        uint8_t t = e->type;
        if (t == 0x10 || t == 0x11 || t == 0x16) { /* the live STAGE1 zombie types only */
            int32_t sweep_ox = e->x, sweep_oz = e->z;    /* pre-dispatch pos (wall-sweep origin) */
            re15_enemy_ai_live_step(s);
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
                re15_collision_constrain(&g_room_rdt, sweep_ox, sweep_oz, &nx, &nz);
                e->x = nx; e->z = nz;
            }
        }
    }
}
