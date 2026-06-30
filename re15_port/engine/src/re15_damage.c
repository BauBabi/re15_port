/*
 * RE1.5 Rebuilt — Player damage resolution (#13).
 *
 * Byte-true port of the PLAYER branch of FUN_80012d60 (the unified hit/damage
 * resolver). Every constant here cites the RE1.5 PSX.EXE disassembly
 * (ghidra1_V2.txt) — see re15_damage.h for the subsystem overview and the
 * deferred in-game trigger (enemy attack-action FSM + actor hitbox test).
 *
 * Instruction map of the player branch (ghidra1_V2.txt):
 *   80012e24-e30  if (player+0x93 & 1)  -> skip (one hit per attack window)
 *   80012e44-e64  player+0x9a (HP) -= dmg_table[type]          (lhu/sub/sh)
 *   80012e58/e68  type<2 gate (sltiu type,2)
 *   80012e70      FUN_800453d0(10)        = play hit SE #10    (DEFERRED audio)
 *   80012e78-eb4  2× FUN_8001af20()&1 -> if both odd, +0x98 |= 2 (bleed/poison)
 *   80012ebc      player+0x4 (state) = 2  (hurt)
 *   80012ec8-ed4  player+0x5 = FUN_8001a7a8(player,hbX,hbZ) + 2 (front/back anim)
 *   80012ee0      player+0x6 = 0
 *   80012eec      player+0x93 |= 1        (set guard; delay slot -> always runs)
 *   80012ee8-efc  if (HP < 0) state=3 (death), +0x5=0, +0x6=0
 */
#include "re15_damage.h"
#include "re15_skeleton.h"   /* re15_skel_compute_pose / re15_skel_bone_to_world / g_anim_pose_actor */
#include "re15_esp.h"        /* re15_esp_fx_spawn — zombie gore effect (FUN_80106a44) */

/* DAT_8006f418 — ghidra1_V2.txt:223455-223478 (11×s16 LE). */
const int16_t re15_damage_table[11] = {
    10,   /* 0  zombie bite/grab (FUN_80017fa4 lunge, attack_type 0) */
    20,   /* 1                                                       */
    1000, /* 2  instakill class                                      */
    1000, /* 3                                                       */
    1000, /* 4                                                       */
    50,   /* 5                                                       */
    100,  /* 6                                                       */
    200,  /* 7                                                       */
    300,  /* 8                                                       */
    1000, /* 9                                                       */
    0,    /* 10                                                      */
};

/* DAT_8006f430 — ghidra1_V2.txt:223480-223490 (enemy-branch reaction codes). */
const uint8_t re15_react_table[11] = {
    0x03, 0x03, 0x09, 0x0A, 0x0B, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14
};

/* Bleed/poison RNG. The PSX calls FUN_8001af20 twice (@80012e78/e80) — a stateless
 * hash (x + (x>>7)) & 0xff fed leftover register a0 (incidental, NON-reproducible by
 * construction — same documented stance as the idle-timer rand in player_common.c).
 * We preserve the EXACT hash and the EXACT 1/4 trigger probability (both draws odd)
 * by feeding a documented xorshift32 entropy source in place of the unreachable
 * register value. Cycle-identical reproduction is impossible; the OBSERVABLE
 * behaviour (a type<2 hit bleeds ~1/4 of the time) is faithful. */
static uint32_t s_rng = 0x2545f491u;

void re15_damage_seed_rng(uint32_t seed) { s_rng = seed ? seed : 1u; }

/* FUN_8001af20 — the engine's shared random draw. Public so the enemy-AI module draws
 * from the SAME global sequence the original does (one RNG for bleed AND AI decisions). */
uint8_t re15_engine_rand8(void)
{
    uint32_t x = s_rng;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;   /* xorshift32 entropy */
    s_rng = x;
    return (uint8_t)((x + (x >> 7)) & 0xffu);   /* FUN_8001af20 hash */
}

static uint32_t dmg_rng(void) { return re15_engine_rand8(); }

/* FUN_8001a7a8 (@8001a7a8) — 1 if the hit came from the player's FRONT hemisphere
 * (within ±90° of facing), else 0; selects the hurt-anim sub-state (2 = back, 3 =
 * front). The PSX computes
 *     ang = FUN_8001a6d4(hbX, hbZ, player+0x34, player+0x3c)
 *         = atan2(player.z - hbZ, player.x - hbX)   (engine RAW angle convention)
 *     rel = ang - player_heading(+0x6a)
 *     front = ((rel + 0x400) & 0xfff) < 0x800
 * re15_atan2_q12 IS the port of FUN_8001a6d4 (actor_locomotion.c). The player's raw
 * heading = rot_y(mesh) + 1024 (the port's standing -1024 mesh offset, used the same
 * way in stair_common.c / skeleton_common.c). The +0x34/+0x3c collision centre and
 * +0x6a heading map to the port's x/z/rot_y — a faithful mapping that is currently
 * UNOBSERVABLE (the port has no distinct front/back hurt clips), so this is the one
 * detail not yet runtime-pinned; it is derived from the disasm, not guessed. */
static int hit_from_front(const re15_actor_t *p, int32_t src_x, int32_t src_z)
{
    int32_t ang = re15_atan2_q12(p->z - src_z, p->x - src_x);
    int32_t rel = (ang - ((int32_t)p->rot_y + 1024) + 0x400) & 0xfff;
    return rel < 0x800;
}

int re15_player_take_damage(re15_actor_t *p, uint8_t attack_type,
                            int32_t src_x, int32_t src_z)
{
    if (!p) return 0;

    /* hit-once guard (@80012e24-e30): one damage application per attack window. */
    if (p->hit_react & 0x1) return 0;

    uint8_t type = (uint8_t)(attack_type & 0xff);
    int16_t dmg  = (type < 11) ? re15_damage_table[type] : 0;

    /* HP -= dmg (@80012e44-e64). HP is s16; the death test below is SIGNED. */
    p->hp = (int16_t)(p->hp - dmg);

    /* type<2 → hit SE + bleed/poison roll (@80012e68-eb8). The SE (FUN_800453d0(10))
     * is DEFERRED until the SE-id table is wired; the bleed roll is byte-true. */
    if (type < 2) {
        if ((dmg_rng() & 1) && (dmg_rng() & 1))
            p->status_flags |= 0x2;            /* DAT_800acaec bit0x2 = bleed/poison */
    }

    /* hurt state (@80012ebc-eec). */
    p->state       = 2;                                                  /* +0x4 hurt */
    p->sub_state_1 = (uint8_t)(hit_from_front(p, src_x, src_z) + 2);     /* +0x5 anim */
    p->sub_state_2 = 0;                                                  /* +0x6      */
    p->hit_react  |= 0x1;                                                /* +0x93 guard */

    /* death override (@80012ee8-efc): signed HP < 0 → death state, clear sub-states. */
    if (p->hp < 0) {
        p->state       = 3;                                             /* +0x4 death */
        p->sub_state_1 = 0;
        p->sub_state_2 = 0;
    }
    return 1;
}

/* ===== Player DEATH state (Phase 8.10) — the byte-true core of the player death FSM ========= *
 * The player dies when HP goes signed-negative: FUN_80012d60 @0x80012ee8 (`bgez hp` → state 3),
 * and the GRAB drains HP the same way (re15_enemy_ai_live_grab) and reaches the same HP<0. The
 * original then runs the death-sequence COMMAND handler — the player command register 0x800aca58
 * (= player block +0x4) is dispatched via @0x80073f90[state]: [3] = the GENERIC death (@0x800366bc,
 * a shot-to-death), [7] = the GRABBED death (@0x8003694c — the live combat-death save shows 0x800aca58
 * = 7, the player died while held). The death handler @0x8003694c INIT (sub-state 0) seeds the
 * fade timer DAT_800acaf2 = 0x78 (120 frames) and a screen colour fade (DAT_800acb18/acb40 masked
 * 0xff000000 | 0x00ffff38), then per frame (sub-state 1) advances a death camera and counts the timer
 * down to sub-state 2 = game over. It NEVER reads the pad — the player is frozen for the whole death
 * sequence. The port models the byte-true CORE: "the player is dead -> input frozen -> the 120-frame
 * death sequence runs", and game_step freezes the player (skip player_tick) while dead. DEFERRED
 * (cited, the port has no fade/game-over infra): the colour fade + death camera (@0x8003694c), the
 * zombie-grabbed eaten-animation FSM (@0x8010a28c), and the game-over screen (sub-state 2). */
#define RE15_DEATH_SEQ_FRAMES 0x78   /* @0x8003694c INIT: DAT_800acaf2 = 0x78 (120) death-fade timer */
static int s_death_seq = -1;         /* -1 = not in the death sequence; >=0 = frames left (120..0) */

int re15_player_is_dead(void)
{
    /* signed HP < 0 = death (FUN_80012d60 @0x80012ee8). */
    return g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0;
}

void re15_player_death_reset(void) { s_death_seq = -1; }

int re15_player_death_tick(void)
{
    if (!re15_player_is_dead()) { s_death_seq = -1; return -1; }   /* alive -> not in the sequence */
    if (s_death_seq < 0) s_death_seq = RE15_DEATH_SEQ_FRAMES;      /* first dead frame = INIT (seed 120) */
    else if (s_death_seq > 0) s_death_seq--;                       /* sub-state 1: count the fade timer down */
    return s_death_seq;   /* 0 = the death sequence is complete -> game over (deferred presentation) */
}

/* ===== Player WEAPON SHOT (Phase 8.10, two-sided combat) = the byte-true core of FUN_80011f50 ===== *
 * The player gun/knife fire-and-resolve is a SEPARATE function from FUN_80012d60 (the enemy resolver
 * the rest of re15_damage.c ports): FUN_80011f50(weapon_id = DAT_800aca5d, aim_ptr = player+0x7c)
 * @0x80011f50. It iterates the active enemies (DAT_800acc2c, stride 0x1f4), runs a per-weapon AIM-CONE
 * hit test (PTR_LAB_8006e548[weapon_id], a line-vs-box tester), picks the NEAREST hit via the min-dist
 * accumulator DAT_8008f5e0 (seed 0x7fffffff @0x8001205c), then on the winner applies, byte-true
 * (@0x800124b0-0x80012528):
 *   dmg  = u16 @ [0x8006e0d0 + enemy_type(+0x8)*0x58 + weapon_id*4]   (a DISTINCT table from the
 *          enemy-attack DAT_8006f418 — do NOT reuse re15_damage_table)
 *   +0x5 = weapon_id  (the reaction clip @0x800124bc)
 *   HP  -= dmg        (+0x9a @0x800124f4/0x80012504)
 *   +0x93 |= 1        (the one-hit guard @0x800124f8)
 *   crit: (weapon_id==7) || (weapon_id==8 && dist<3000) -> +0x93|=0x40; if (+0x93&0x40 && type<0x20)
 *         HP = -1     (instant kill, @0x800124fc-0x8001251c)
 *   +0x4 = (HP >= 0) ? 2 (HURT) : 3 (DEATH)   (@0x80012520)
 *   +0x6 = DAT_8006f410[heading>>0x1d]  (the hit-direction clip — deferred, anim detail)
 * The damaged enemy then runs the zombie HURT/DEATH state (re15_enemy_ai_live_hurt/death).
 *
 * BYTE-TRUE TABLES (dumped from PSX.EXE): the zombie damage row (enemy types 0x10/0x11/0x16 are all
 * IDENTICAL, @0x8006e650/0x8006e6a8/0x8006e860) and the per-weapon reach (@0x8006e5a0, u32[22]).
 *
 * THE PER-WEAPON CONE TESTER (PTR_LAB_8006e548[weapon], @0x80011fec call): the gun weapons all use one
 * of two BYTE-IDENTICAL pure-2D testers — FUN_800127fc (pistol w1/w2) and FUN_800128a0 (magnum/shotgun
 * class). Disassembled byte-true, both are:
 *     dx = enemy(+0x34) - aim.x ; dz = enemy(+0x3c) - aim.z ; dist = SquareRoot0(dx*dx + dz*dz)
 *     R   = (u16 @ enemy_hitbox_data+6  i.e. hit_radius_min) + (reach & 0xffff)
 *     HIT iff (dist < R)  [strict, unsigned] AND (dist < DAT_8008f5e0)  -> latch DAT_8008f5e0 = dist
 * i.e. it is a radial reach test from the AIM POINT, picking the nearest, where the per-enemy hitbox
 * radius (hbdata+6) is ADDED to the weapon reach. PORTED byte-true here: R = reach + e->hit_radius_min,
 * strict dist < R (was: dist <= reach, the radius omitted -> the zombie's 400-unit radius shrank the
 * pistol's effective range from the true 1399 to 999). The knife (FUN_80012574) is a projected polygon
 * (5x FUN_8004f008) -> deferred (needs the camera projection).
 *
 * FAITHFUL-LINE (proven blocked on the aim FSM the port lacks): the original measures dist from the
 * forward-projected AIM POINT (param_2 = a 16-bit world SVECTOR the aim/fire FSM @0x80035810 computes);
 * the port measures from the player and keeps re15_ai_arc_test(player, ex,ez, 0x400) (front hemisphere)
 * as the directional stand-in for both the aim point and the camera line-of-fire geometry block
 * (FUN_80011f50 @0x80012280-0x80012370 = DAT_800ac784 camera + DAT_800aca88/8c/90 aim ray + FUN_8001bf04,
 * none of which the port has). DEFERRED (each needs absent infra, do NOT guess): the hit-direction clip
 * +0x6 = DAT_8006f410[heading>>0x1d] = {7,0,1,7,2,0,0,0} (needs DAT_800acaec/aim-Z, anim detail), the
 * per-type damage rows for non-zombies, the equipped-weapon source DAT_800aca5d (inventory), and the
 * aim/fire input FSM. The aim-mode alternate reach row (@+0x58, DAT_800aca5c&4) is byte-IDENTICAL to the
 * base row -> nothing to port. Returns the hit enemy slot+1 (0 = no target in cone/reach). */

/* Per-weapon damage to a zombie (types 0x10/0x11/0x16, identical rows @0x8006e650). 22 weapons. */
static const uint16_t s_player_wpn_dmg_zombie[22] = {
    0, 6, 24, 5, 5, 15, 15, 200, 40, 100, 200, 100, 10, 100, 10, 100, 200, 100, 400, 20, 0, 100
};
/* Per-weapon shot reach (UNK_8006e5a0, u32). 22 weapons. */
static const uint16_t s_player_wpn_reach[22] = {
    1000, 1100, 1000, 1000, 1100, 1000, 1200, 1000, 1500, 1000, 1000,
    1000, 1300, 1800, 1000, 1000, 1000, 1000, 1000, 1100, 1000, 1000
};

int re15_player_weapon_fire(int weapon_id)
{
    if (weapon_id < 0 || weapon_id >= 22) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint32_t reach = s_player_wpn_reach[weapon_id];

    /* auto-aim: nearest live zombie in front, within reach (DAT_8008f5e0 min-dist, seed 0x7fffffff). */
    int best = -1;
    uint32_t best_dist = 0x7fffffffu;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (e->type != 0x10 && e->type != 0x11 && e->type != 0x16) continue;  /* the port's hittable enemies */
        if (e->state == 7) continue;   /* RE15_AI_STATE_CORPSE — already a corpse (literal: avoid the AI-header dep) */
        uint32_t dist = (uint32_t)re15_enemy_player_dist(e, pl);
        /* byte-true cone tester FUN_800127fc/800128a0: R = reach + enemy hitbox radius (hbdata+6),
         * hit iff strict dist < R (unsigned). The 400-unit zombie radius is part of the reach. */
        uint32_t R = reach + ((uint32_t)e->hit_radius_min & 0xffffu);
        if (dist >= R) continue;                                             /* out of (reach + radius) */
        if (re15_ai_arc_test(pl, e->x, e->z, 0x400) != 0) continue;          /* not in front (faithful-line dir) */
        if (dist < best_dist) { best_dist = dist; best = s; }
    }
    if (best < 0) return 0;   /* no target in cone/reach */

    re15_actor_t *e = &g_actors[best];
    int dmg = s_player_wpn_dmg_zombie[weapon_id];   /* byte-true per-weapon zombie damage */
    e->sub_state_1 = (uint8_t)weapon_id;            /* +0x5 = reaction clip = weapon_id (@0x800124bc) */
    e->hp          = (int16_t)(e->hp - dmg);        /* +0x9a -= dmg */
    e->hit_react  |= 0x1;                           /* +0x93 |= 1 (one-hit guard) */
    /* crit/headshot (@0x800124fc-0x8001251c): weapon 7, or weapon 8 within 3000 -> instant kill (type<0x20). */
    if ((weapon_id == 7 || (weapon_id == 8 && best_dist < 3000u)) && e->type < 0x20)
        e->hp = -1;
    e->sub_state_3 = 0;                              /* +0x7 = 0 (@0x80012... line 157) — start the hurt/death anim FSM at phase 0 */
    e->state       = (e->hp >= 0) ? 2 : 3;          /* +0x4 = HURT(2) / DEATH(3) (@0x80012520) */
    e->sub_state_2 = 0;                              /* +0x6 hit-dir clip (DAT_8006f410[heading>>0x1d]) — deferred */
    return best + 1;                                /* hit (slot+1, non-zero) */
}

/* Enemy branch of FUN_80012d60 (@80012f08-80013034): apply a resolved hit to an
 * ENEMY actor — the counterpart to the player branch. Same dmg table, but the
 * reaction sub-state is the hit-clip from re15_react_table (not the front/back
 * selector) and there is NO bleed roll (player-only). The resolver LOOP owns the
 * per-frame bits the original sets around this: the +0x93 &= 1 clear (@80012f88),
 * the FUN_8001a7a8 collision-confirm → +0x93 bit0x80 (@80012fa8), the self-exclusion
 * vs the attacker +0x188 (@80012f40), and the +0x90 0x3000000 state-mask gate
 * (@80012f54). This applies the per-enemy damage ONCE per attack window.
 * Returns 1 if the hit landed, 0 if the enemy was already hit this attack. */
int re15_enemy_take_damage(re15_actor_t *e, uint8_t attack_type)
{
    if (!e) return 0;
    /* hit-once guard +0x93 bit0x1 (@80012fb4-c0): already hit → mark bit0x2, no re-damage. */
    if (e->hit_react & 0x1) {
        e->hit_react |= 0x2;                                       /* +0x93 |= 2 (@80012fcc) */
        return 0;
    }
    uint8_t type = (uint8_t)(attack_type & 0xff);
    int16_t dmg  = (type < 11) ? re15_damage_table[type] : 0;
    e->sub_state_3 = 0;                                            /* +0x7 = 0 (@80012fd4) */
    e->sub_state_2 = 1;                                            /* +0x6 = 1 (@80012fd8) */
    e->sub_state_1 = (type < 11) ? re15_react_table[type] : 0;     /* +0x5 = reaction clip (@80012fe8) */
    e->hp = (int16_t)(e->hp - dmg);                               /* +0x9a -= dmg (@80012ffc) */
    e->hit_react |= 0x1;                                           /* +0x93 |= 1 (@8001300c) */
    e->state = 2;                                                  /* +0x4 = 2 hurt (@80013018) */
    if (e->hp < 0) e->state = 3;                                  /* signed HP<0 → death (@80013020) */
    return 1;
}

void re15_player_clear_hit_guard(re15_actor_t *p)
{
    if (p) p->hit_react &= (uint8_t)~0x1;
}

/* Zombie gore effect — byte-true FUN_80106a44 @0x80106a98-0x80106abc (STAGE1 briefing zombie
 * behaviour handler): per frame, if the "took a 2nd hit while still hit-guarded" bit
 * (+0x93 & 2, set by re15_enemy_take_damage @0x80012fcc) is set, spawn the gore effect at the
 * zombie and clear the bit. Byte-true: a0 = 0x2000 -> effect-id 0, sub 0; a1 = entity+0x6a =
 * rot_y; spawn handler &DAT_8012017c. The original position is the model_inst bone block
 * (entity+0x188 + 0x4f4); the port has no such bone block, so it uses the actor's world
 * position (faithful-line, flagged). Bank-gated: effect-id 0 only resolves where the room RDT
 * loads it — ROOM1140 loads ids 05/07, so this no-ops there byte-true (the visible ROOM1140
 * gore is the effect-id-5 spawn at the behaviour SETUP, which needs the full FUN_80106a44 port).
 * The setup-spawn + the behaviour state machine are the next port unit (C3_RENDER_DESIGN.md §2d). */
void re15_enemy_gore_tick(re15_actor_t *e)
{
    if (!e || !e->active) return;
    if (e->hit_react & 0x2) {                                  /* +0x93 & 2 (@0x80106a98) */
        re15_esp_fx_spawn(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/,
                          e->x, e->y, e->z, (int16_t)e->rot_y);  /* a1 = entity+0x6a = rot_y */
        e->hit_react &= (uint8_t)~0x2;                         /* +0x93 &= 0xfd (@0x80106abc) */
    }
}

/* Zombie gore-SETUP behaviour — byte-true FUN_80106edc @0x80106edc (STAGE1). Dispatched via
 * (*(code*)(&PTR_LAB_8011fe30)[entity+0x5])() at index 0x58 (0x8011fe30 + 0x58*4 = 0x8011ff90 =
 * FUN_80106edc) -> the byte-true trigger is sub_state_1 (entity+0x5) == 0x58. On first entry
 * (entity+7==0): a 50% RNG (func_0x8001af20 & 1) either defers (==0 -> entity+6=5, retry) or runs
 * the SETUP (==1): entity+0x93|=1, entity+7=1, then the gore burst at the zombie bone:
 *   2x func_0x80019700(0x2000, rot_y, bone, &LAB_8012016c)  = effect-id 0
 *   1x func_0x80019700(0x5002800, rot_y, bone, &LAB_8012016c) = effect-id 5  <-- VISIBLE ROOM1140 gore
 * Position is the model_inst bone block (entity+0x188 + LAB_8011f784[entity+8]*0xac) -> actor world
 * position (faithful-line). The anim/behaviour fields +0x8f/+0x94/+0x95/+0x1b8, the spawned slot's
 * model_inst init, and the func_0x800453d0(9) sound are NOT modelled in the port -> flagged-skipped.
 * Fires once the AI puts the zombie in sub_state_1==0x58 (the transition INTO this state is the next
 * brick, C3_RENDER_DESIGN.md §2e). */
void re15_enemy_gore_setup(re15_actor_t *e)
{
    if (!e || !e->active) return;
    if (e->sub_state_1 != 0x58) return;      /* PTR_LAB_8011fe30[+0x5]==0x58 -> FUN_80106edc */
    if (e->sub_state_3 != 0) return;         /* entity+7 != 0: setup already run (anim phase) */
    if ((re15_engine_rand8() & 1) == 0) {    /* func_0x8001af20 & 1 == 0 -> defer, no spawn */
        e->sub_state_2 = 5;                  /* entity+6 = 5 */
        return;
    }
    e->hit_react   |= 0x1;                    /* entity+0x93 |= 1 */
    e->sub_state_3  = 1;                      /* entity+7 = 1 (run the setup once) */
    const re15_esp_t *bank = re15_esp_room_bank();
    re15_esp_fx_spawn(bank, 0, 0, e->x, e->y, e->z, (int16_t)e->rot_y);  /* a0=0x2000   effect-id 0 */
    re15_esp_fx_spawn(bank, 0, 0, e->x, e->y, e->z, (int16_t)e->rot_y);  /* a0=0x2000   effect-id 0 */
    re15_esp_fx_spawn(bank, 5, 0, e->x, e->y, e->z, (int16_t)e->rot_y);  /* a0=0x5002800 effect-id 5 */
}

/* ====================================================================== *
 *  Attack-hitbox vs actor collision test — FUN_8002b5d0                   *
 *  (ghidra1_V2.txt:118005-118130). The per-target geometry test the       *
 *  resolver loop FUN_80012d60 runs against each enemy + the player.       *
 * ====================================================================== */

/* BIOS SquareRoot0 (floor sqrt) — the original calls it @8002b740. Bit-by-bit
 * integer isqrt, identical to re15_collision.c's coll_isqrt: each combat /
 * collision / light / camera module keeps its own private copy, the established
 * port convention (the PSX inlines SquareRoot0 at every call site). */
static int32_t dmg_isqrt(int64_t x)
{
    if (x <= 0) return 0;
    uint64_t v = (uint64_t)x, res = 0, bit = (uint64_t)1 << 62;
    while (bit > v) bit >>= 2;
    while (bit) {
        if (v >= res + bit) { v -= res + bit; res = (res >> 1) + bit; }
        else res >>= 1;
        bit >>= 2;
    }
    return (int32_t)res;
}

/* CIRCULAR path of FUN_8002b5d0 (radius_min == radius_max @ hbdata+6/+0xa). The
 * angular-SECTOR path (radius interpolated over the target's facing via the BIOS
 * ratan2 + rsin/rcos) is DEFERRED: the port has only the catan-based atan2
 * (re15_atan2_q12 = FUN_8001a6d4), not the BIOS ratan2 the sector branch calls
 * @8002b65c — porting that is a separate byte-true step. Symmetric hitboxes
 * (the common case) take this circular branch directly @8002b6fc.
 *
 * (cx,cy,cz) = the target hitbox centre = target world pos (+0x34/+0x38/+0x3c)
 * + the local offset (the short[3] at target+0x7c). radius/height = hbdata+6/+8.
 * (atk_x,atk_y,atk_z) = attack origin (param_2[0..2]); atk_radius = param_1.
 * Instruction map (ghidra1_V2.txt):
 *   8002b6fc  R   = (radius&0xffff) + (atk_radius&0xffff)
 *   8002b704  dx  = atk_x - cx;   8002b714  reject unless |dx| <= R (incl.)
 *   8002b724  dz  = atk_z - cz;   8002b730  reject unless |dz| <= R (incl.)
 *   8002b740  dist= SquareRoot0(dx*dx + dz*dz)
 *   8002b75c  reject unless 0 < R - dist   (dist < R, strict)
 *   8002b768  h   = (atk_radius&0xffff) + height
 *   8002b76c  dy  = atk_y - cy;   8002b778  accept iff -h < dy && dy < h (strict) */
int re15_hitbox_overlap(int32_t cx, int32_t cy, int32_t cz,
                        int32_t radius, int32_t height,
                        int32_t atk_x, int32_t atk_y, int32_t atk_z,
                        int32_t atk_radius)
{
    int32_t R = (int32_t)((uint32_t)radius & 0xffff) + (int32_t)((uint32_t)atk_radius & 0xffff);

    /* X/Z AABB broad-phase — the original's unsigned |d|<=R idiom: (u32)(d+R) <= (u32)(2R). */
    int32_t dx = atk_x - cx;
    if ((uint32_t)(dx + R) > (uint32_t)(R * 2)) return 0;
    int32_t dz = atk_z - cz;
    if ((uint32_t)(dz + R) > (uint32_t)(R * 2)) return 0;

    /* Euclidean distance < R (strict, via 0 < R - dist). */
    int32_t dist = dmg_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
    if (R - dist <= 0) return 0;

    /* Y/height range: -h < dy < h, h = attack_radius + hitbox height. */
    int32_t h  = (int32_t)((uint32_t)atk_radius & 0xffff) + (int32_t)((uint32_t)height & 0xffff);
    int32_t dy = atk_y - cy;
    return (-h < dy && dy < h) ? 1 : 0;
}

/* FUN_8002b5d0 wrapper over a port actor: read its hitbox dims (the +0x78 struct)
 * + local offset (+0x7c) and run the overlap test. Circular path is byte-true; the
 * angular-SECTOR case (hit_radius_min != hit_radius_max) needs the BIOS ratan2 +
 * rsin/rcos directional-reach interpolation (@8002b65c) which the port hasn't yet
 * ported — it is DEFERRED and routed through the circular path on radius_min as a
 * documented placeholder (never reached in-game today: enemy hitbox values aren't
 * wired yet and the AI attack trigger is deferred). */
int re15_hitbox_test(const re15_actor_t *target, const re15_attack_box_t *atk)
{
    if (!target || !atk) return 0;
    int32_t cx = target->x + target->hit_offset_x;          /* +0x34 + offset[0] */
    int32_t cy = target->y + target->hit_offset_y;          /* +0x38 + offset[1] */
    int32_t cz = target->z + target->hit_offset_z;          /* +0x3c + offset[2] */
    int32_t radius = target->hit_radius_min;                /* == hit_radius_max (circular) */
    return re15_hitbox_overlap(cx, cy, cz, radius, target->hit_height,
                               atk->x, atk->y, atk->z, atk->radius);
}

/* ====================================================================== *
 *  Damage-resolver LOOP — FUN_80012d60 (ghidra1_V2.txt:77607-77814).      *
 *  Tests one attack hitbox against the player block + every active enemy  *
 *  and applies the damage. The two branches (player/enemy take_damage)    *
 *  are ported above; this is the iteration + gating around them.          *
 * ====================================================================== */
int re15_resolve_attack(const re15_attack_box_t *atk, uint8_t attack_type,
                        int attacker_slot)
{
    if (!atk) return 0;

    int collected[RE15_ACTOR_MAX];
    int ncol = 0;
    int hits = 0;

    /* (1) Enemy pass — collect every active enemy the hitbox overlaps. The original
     * scans DAT_800acc2c (stride 0x1f4) until DAT_800aca4e active entries are seen
     * (@80012d7c-de4); the port iterates slots 1.. (slot 0 = the player block, tested
     * separately @80012df0) and skips inactive slots — same active set. */
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active) continue;                              /* (*puVar6 & 1) @80012d9c */
        if (re15_hitbox_test(e, atk))                          /* FUN_8002b5d0 @80012db4 */
            collected[ncol++] = i;                             /* local_78[] @80012dc8 */
    }

    /* (2) Player block — ALWAYS tested, regardless of the enemy pass (@80012df0).
     * re15_player_take_damage is the player branch (guard + HP + bleed + hurt/death
     * state @80012e18-f04); cVar9 is set on overlap even if the guard blocked re-damage. */
    if (re15_hitbox_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], atk)) {
        re15_player_take_damage(&g_actors[RE15_ACTOR_SLOT_PLAYER], attack_type,
                                atk->x, atk->z);
        hits++;                                                /* cVar9 = 1 @80012efe */
    }

    /* (3) Enemy application — reverse collected order (the original do-while
     * decrements bVar8 @80012f12-30). */
    for (int k = ncol - 1; k >= 0; k--) {
        int slot = collected[k];
        re15_actor_t *e = &g_actors[slot];

        /* GATE A — self-exclusion (@80012f40): the attacker never damages itself.
         * Original: (e+0x188+0x40) == (attacker+0x74), i.e. the same model-pool
         * pointer ↔ the same actor; the port maps that identity to slot equality. */
        if (slot == attacker_slot) continue;

        /* GATE B — terminal-state skip (@80012f54): the original skips when
         * (e+0x90 & 0x3000000) == 0x3000000 (the enemy's death / despawn terminal
         * flags). The bit WRITER is the enemy death/lifecycle FSM, which the port has
         * not yet implemented, so no enemy can be in that state today → the gate is
         * inert and is OMITTED here (behaviourally identical now, same documented-
         * deferral stance as the hit-SE below). FUN_80011f50 reads the same gate. */

        /* Per-attack collision bits (@80012f70-fac): clear all but the hit-once bit,
         * then set bit0x80 when the hit came from the target's FRONT (FUN_8001a7a8). */
        e->hit_react &= 1;
        if (hit_from_front(e, atk->x, atk->z))
            e->hit_react |= 0x80;

        /* type<2 → hit SE FUN_800453d0(10) @80012f80 — DEFERRED (audio SE-id table). */

        /* Enemy branch (@80012fb4-3034): applies the hit once per window (bit0 guard),
         * else marks the re-hit bit0x2. */
        re15_enemy_take_damage(e, attack_type);
        hits++;                                                /* cVar9 += 1 @80012fec */
    }
    return hits;
}

/* FUN_80017fa4 (ghidra1_V2.txt:84648-84717) — the enemy attack-action's DAMAGE
 * trigger. The original builds the hitbox descriptor from the attacker's precomputed
 * forward attack-point (DAT_800b52c4 +0x28/+0x2a/+0x2c = the s16 fields read @80017fc4-
 * fe8) and calls FUN_80012d60(0x1f4, &point, 0) @80018008 — attack_radius 500,
 * attack_type 0 (= 10 dmg). This ports that damage path onto the port actor's atk_pt_*
 * fields + re15_resolve_attack.
 *
 * DEFERRED (the rest of the attack-action handler; its offsets/SE don't map to the
 * port's actor subset and depend on unported subsystems):
 *   - the lunge per-frame advance (attacker+0x4/+0x6 += 0x1e @80017fb4/fcc),
 *   - the FUN_8001c6e8 room-collision secondary trigger (the `sVar2 <= Y` OR-branch),
 *   - the on-hit SE (FUN_800199d4 / FUN_80045024) + attacker state-reset + the
 *     +0x6e = 0xd next-action transition (@80018030-98),
 *   - the AI DECISION that enters this action (range/state) — lives in the STAGE1
 *     overlay, address not yet RE'd; both agents recommend a room1140 savestate. */
int re15_enemy_attack(int attacker_slot)
{
    if (attacker_slot < 0 || attacker_slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *atk = &g_actors[attacker_slot];
    re15_attack_box_t box;
    box.x      = atk->atk_pt_x;     /* attacker+0x28 (s16) */
    box.y      = atk->atk_pt_y;     /* attacker+0x2a (s16) */
    box.z      = atk->atk_pt_z;     /* attacker+0x2c (s16) */
    box.radius = 500;               /* FUN_80012d60(500,...) — radius 500, confirmed in FUN_80017fa4.c */
    return re15_resolve_attack(&box, 0, attacker_slot);   /* attack_type 0 = 10 dmg */
}

/* Lunge attack-action driver — LAB_80017eb0 (setup) + LAB_80017f50 (per-frame tick).
 * LAB_80017eb0:  work-struct +0xe (sh) = 0x20  -> 32 active frames; +0x0 = action 0x17.
 * LAB_80017f50:  if (+0xe != 0) +0xe -= 1; else FUN_800174e4();  then FUN_80017fa4() each call.
 * FUN_80017fa4:  on (FUN_80012d60 != 0 || room-wall) -> reset the action (+0x6e = 0xd) so the
 *                lunge bites at most once. The port models the active window + the per-frame
 *                fire + the bite-once reset; the GTE attack-point + movement + the end-of-window
 *                keyframe transition (FUN_800174e4) are the deferred skeleton/model-pool parts. */
void re15_enemy_lunge_begin(int attacker_slot)
{
    if (attacker_slot < 0 || attacker_slot >= RE15_ACTOR_MAX) return;
    g_actors[attacker_slot].lunge_frames = 0x20;   /* +0xe = 0x20 active frames */
}

int re15_enemy_lunge_tick(int attacker_slot)
{
    if (attacker_slot < 0 || attacker_slot >= RE15_ACTOR_MAX) return 0;
    re15_actor_t *a = &g_actors[attacker_slot];
    if (a->lunge_frames == 0) return 0;             /* action not active */
    a->lunge_frames--;                               /* +0xe -= 1 (LAB_80017f50) */
    int hits = re15_enemy_attack(attacker_slot);     /* FUN_80017fa4 fires every active frame */
    if (hits > 0) a->lunge_frames = 0;               /* connect -> reset (+0x6e=0xd): bite once */
    return hits;
}

/* Phase 8.1 — faithful-line attack-point: pose the enemy's skeleton at `keyframe` (QUERY,
 * crossfade state preserved) and store `attack_bone`'s world position into atk_pt_*. The
 * model->world transform is byte-true (re15_skel_bone_to_world = the render-loop math); the
 * BONE selection is the documented faithful-line stand-in for the deferred GTE/model-pool
 * attack point (see re15_damage.h). atk_pt_* are int16 to match the original's short store. */
void re15_enemy_update_attack_point(int slot, const re15_emd_skeleton_t *skel,
                                    int keyframe, int attack_bone)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !skel) return;
    if (attack_bone < 0 || attack_bone >= skel->bone_count) return;
    re15_actor_t *a = &g_actors[slot];

    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;     /* pose QUERY — don't consume/mutate the crossfade blend */
    int rv = re15_skel_compute_pose(skel, keyframe, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;

    int32_t w[3];
    re15_skel_bone_to_world(poses[attack_bone].trans, a->rot_y, a->x, a->y, a->z, w);
    a->atk_pt_x = (int16_t)w[0];
    a->atk_pt_y = (int16_t)w[1];
    a->atk_pt_z = (int16_t)w[2];
}

/* ====================================================================== *
 *  Hitbox-dimension setup — byte-true values for the +0x78 struct data.   *
 * ====================================================================== */

/* Player hitbox = the fixed EXE-static struct @0x80073e94, VERIFIED in the on-disc
 * PSX.EXE at file offset 0x64694 (bytes 00 00 06 fa 00 00 c2 01 fa 05 c2 01 →
 * +6 radius_min = 0x01c2 = 450, +8 height = 0x05fa = 1530, +10 radius_max = 0x01c2
 * = 450; radius_min==radius_max → circular). Call at player init. */
void re15_player_apply_hitbox(re15_actor_t *p)
{
    if (!p) return;
    p->hit_radius_min = p->hit_radius_max = 450;
    p->hit_height     = 1530;
    /* The player's +0x7c local offset was not extracted; XZ offset 0 centres the
     * hitbox on the player horizontally (the only component the circular XZ test
     * uses); the Y-centre offset is left 0 (unconfirmed for the player). */
    p->hit_offset_x = p->hit_offset_y = p->hit_offset_z = 0;
}

/* Enemy hitbox by type. The per-type +0x78 struct (DAT_800b2248[type<<2] + 0x8) is
 * overlay-PATCHED at runtime — the on-disc table slots are null — so these are read
 * byte-true from room-savestate RAM (player-HP=100 sanity-gated, the +6/+8/+10 layout
 * cross-checked against FUN_8002b5d0). Each enemy's +0x7c local offset = (0,-height,0).
 * Unknown types get NO hitbox (0) — no guessing. Call at enemy spawn. */
void re15_enemy_apply_hitbox(re15_actor_t *a, uint8_t type)
{
    if (!a) return;
    uint16_t r, h;
    switch (type) {
        case 0x47: r = 450;  h = 1530; break;  /* STAGE1 humanoid/zombie (HASH-..._1.sav)   */
        /* The LIVE STAGE1 briefing zombies (0x10/0x11/0x16) all read 400/1440 — byte-true
         * from the live combat RAM (stage_saves/mzd_stage1_combat_death.sav, Phase 8.7: every
         * active 0x10/0x11 entity's *(+0x78) hitbox struct = radius 400 / height 1440, the
         * value the on-disc table is overlay-patched to). Was 0x16-only (a STAGE2 save);
         * 0x10/0x11 fell through to "no hitbox" — now wired from the STAGE1 combat save. */
        case 0x10: case 0x11:
        case 0x16: r = 400;  h = 1440; break;  /* STAGE1 briefing zombies + STAGE2 enemy     */
        case 0x29: r = 1100; h = 1080; break;  /* wide/short creature (HASH-..._5.sav)       */
        default:   return;                      /* unverified type → no hitbox (no guessing) */
    }
    a->hit_radius_min = a->hit_radius_max = r;
    a->hit_height     = h;
    a->hit_offset_x   = 0;
    a->hit_offset_y   = (int16_t)(-(int32_t)h);   /* +0x7c local offset = (0, -height, 0) */
    a->hit_offset_z   = 0;
}

/* ====================================================================== *
 *  STAGE1 zombie AI (type 0x47, AI handler FUN_8011d6d4) — byte-true.      *
 *  The AI is a 4-level nested FSM (main state +0x4 → PTR_FUN_801217a0;     *
 *  sub-state → PTR_FUN_801217b4; etc.). The DECISION transitions below are *
 *  confirmed (the SAME condition appears verbatim in FUN_80101b64 /        *
 *  FUN_80101de4 / FUN_80102058): dist<2000 && arc!=0 → state word 0x701    *
 *  (the attack-commit state), and dist<4000 && arc==0 → 0x201 (approach,   *
 *  FUN_80101c7c). Open (next phases): the 0x701→FUN_80017fa4 lunge chain    *
 *  (the action sequence + the attack-point), the movement AI, the tick.    *
 *  Caveat: re15_ai_arc_test carries the documented +1024 angle-convention   *
 *  note — the thresholds/structure are byte-true; the arc orientation wants *
 *  a dynamic (mid-lunge) confirmation.                                     *
 * ====================================================================== */

/* Player distance the zombie AI caches at entity+0x1d0 (FUN_8011d6d4 @8011d6e8-708):
 * SquareRoot0(DX² + DZ²) with DX/DZ = the 16-bit-wrapped (player - zombie) X/Z (the
 * original reads each position through a 16-bit load). func_0x80065f60 = SquareRoot0. */
int32_t re15_enemy_player_dist(const re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return 0;
    int32_t dx = (int16_t)((int32_t)player->x - (int32_t)e->x);
    int32_t dz = (int16_t)((int32_t)player->z - (int32_t)e->z);
    return dmg_isqrt((int64_t)dx * dx + (int64_t)dz * dz);
}

/* FUN_8001a9cc — the AI arc test (used by the approach-state handler FUN_80101b64).
 * ang = FUN_8001a6d4(eX,eZ,pX,pZ) (= re15_atan2_q12, raw atan2, 0=+Z); the original
 * subtracts the heading +0x6a — the port's mesh rot_y is that heading minus 1024 (the
 * same +1024 convention re15_damage.c's hit_from_front uses), so the relative angle =
 * ang - (rot_y + 1024). u = (cone + rel) & 0xfff. Returns 0 when the player is INSIDE
 * the ±cone front arc, else ±cone (the turn direction: u>0x800 → -cone). Byte-true to
 * @8001a9cc (verified: player ahead → 0, player to a side/behind → ±cone). */
int re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone)
{
    if (!e) return 0;
    int32_t ang = re15_atan2_q12(pz - e->z, px - e->x);
    int32_t rel = ang - ((int32_t)e->rot_y + 1024);
    int32_t u   = (cone + rel) & 0xfff;
    int r = 0;
    if ((cone << 1) <= u) { r = cone; if (u > 0x800) r = -cone; }
    return r;
}

/* The zombie's byte-true ATTACK-COMMIT condition — the transition that sets the
 * entity state word @+0x4 to 0x701 (the attack-commit state). Confirmed: the SAME
 * test appears verbatim in three STAGE1 handlers (FUN_80101b64, FUN_80101de4,
 * FUN_80102058):  (dist < 2000 (0x7d0)) && (re15_ai_arc_test(player,0x2c8) != 0).
 * dist = re15_enemy_player_dist (the AI's cached +0x1d0). Returns 1 when the zombie
 * commits the attack this frame. The companion approach transition (FUN_80101c7c) is
 * dist<4000 && arc==0 → 0x201. NOTE: the 0x701→FUN_80017fa4 lunge chain (action
 * sequence + attack-point) and the arc's +1024 convention are the open items — the
 * threshold/structure here are byte-true (3-handler-confirmed). */
int re15_enemy_should_attack(const re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return 0;
    if ((uint32_t)re15_enemy_player_dist(e, player) >= 2000) return 0;   /* dist < 0x7d0 */
    return re15_ai_arc_test(e, player->x, player->z, 0x2c8) != 0;
}
