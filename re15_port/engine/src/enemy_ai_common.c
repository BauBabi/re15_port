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

/* The +0x5 decision dispatch = FUN_8010168c's PTR_FUN_8011f840[entity+0x5] call. Routes
 * the active sub-mode to its (byte-true) decision handler; indices 3.. are the deferred
 * movement / attack-execution leaves. See re15_enemy_ai.h for the full vtable + why this
 * is not auto-wired into the tick yet (the generic-humanoid EXE leaf layer is deferred). */
void re15_ai_dispatch_decision(re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return;
    switch (e->sub_state_1) {                              /* entity+0x5 */
        case 0: re15_ai_decide_search_timer(e, player); break;  /* f840[0]=FUN_80101b64 */
        case 1: re15_ai_decide_search(e, player);       break;  /* f840[1]=FUN_80101de4 */
        case 2: re15_ai_decide_engage(e, player);       break;  /* f840[2]=FUN_80102058 */
        default:                                                /* f840[3..]=FUN_80102540/  */
            /* 2bd0/2d20/2f1c/.. movement+attack-execution leaves — DEFERRED (model pool). */
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
    if (!(e->ai_flags & 0x100)) return 0;          /* not attack-armed -> movement tail (deferred) */

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
