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
#include "re15_damage.h"   /* re15_enemy_player_dist, re15_ai_arc_test, re15_engine_rand8,
                            * re15_enemy_apply_hitbox */

/* Engine-wide AI freeze = DAT_800aca40 & 0x20000000 (FUN_8011d6d4 gate). */
static int s_ai_paused = 0;
void re15_enemy_ai_set_paused(int paused) { s_ai_paused = paused ? 1 : 0; }

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
static int s_player_grabbed = 0;
int re15_player_is_grabbed(void) { return s_player_grabbed; }

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
            else { e->sub_state_2 = 2; e->anim_frac = 0xf; }
            break;
        case 2: e->sub_state_2 = 3; break;               /* stand-up anim (0x80103b08) — faithful stand-in */
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
    s_player_grabbed = 1;
    switch (e->sub_state_2) {                /* +0x6 sub-step (reset to 0 by the 0x301/0x401 commit) */
        case 0: e->sub_state_2 = 1; break;   /* [0] init/latch (player register/flag + motion deferred) */
        case 1: e->sub_state_2 = 2; break;   /* [1] pull-in (anim-gated -> faithful stand-in advance) */
        case 2:                               /* [2] IMPACT — the byte-true -10 grab hit (0x801026f0) */
            player->hp     = (int16_t)(player->hp - 10);
            if (player->hp < 0) player->state = 3;   /* hp<0 -> death (FUN_80012d60 @0x80012ee8) */
            e->ai_timer    = 0x6e;           /* +0x9c bite window (the loop count is anim-gated) */
            e->sub_state_2 = 3;
            break;
        case 3:                               /* [3] BITE — the byte-true -5/bite (0x80102788, loops) */
            player->hp     = (int16_t)(player->hp - 5);
            if (player->hp < 0) player->state = 3;
            e->sub_state_2 = 6;             /* one bite/cycle: the anim-gated bite LOOP is deferred */
            break;
        case 6: e->sub_state_2 = 8; break;   /* [6] release anim (player grabbed-flag clear deferred) */
        default:                              /* [8] EXIT (0x80102b90) -> back to the engage brain */
            re15_ai_set_state_word(e, 0x201);   /* +0x4 = state 1 / +0x5 = 2 (engage) */
            break;
    }
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
    int16_t turn = (int16_t)re15_ai_arc_test(e, player->x, player->z, 0x80);  /* ±0x80 toward player */
    e->rot_y = (int16_t)(((int32_t)e->rot_y + turn) & 0x0fff);                 /* +0x6a += residual */
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
                 * (The +0x5=0/1/2 animate halves are the idle/track anim — deferred; the +0x5=5/6
                 * forward walk is anim-root-motion-coupled — deferred. The lunge-arm FUN_8010ab2c is a
                 * SEPARATE dispatch @0x80120208[+0x4=6] and is DORMANT — DAT_800aca3c&1 is never set,
                 * 8.7 — so it is not wired here; +0x5=7 is the TURN state, not the arm.) */
                if (e->sub_state_1 == 3 || e->sub_state_1 == 4)
                    re15_enemy_ai_live_grab(e, player);
                else if (e->sub_state_1 == 7)
                    re15_enemy_ai_live_turn(e, player);
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

    switch (e->state) {                                  /* @0x8011f7b4[entity+0x4] */
        case RE15_AI_STATE_INIT:   re15_enemy_ai_live_init(slot);   break;  /* [0] FUN_80100688 */
        case RE15_AI_STATE_ACTIVE: re15_enemy_ai_live_active(slot); break;  /* [1] FUN_80101224 */
        default: /* [2]=0x80105a8c / [3]=0x80106ba4 / [4]=0x8010919c — deferred */ break;
    }
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
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        uint8_t t = e->type;
        if (t == 0x10 || t == 0x11 || t == 0x16)   /* the live STAGE1 zombie types only */
            re15_enemy_ai_live_step(s);
    }
}
