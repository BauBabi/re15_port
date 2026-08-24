/*
 * RE1.5 Rebuilt — Player damage resolution (#13).
 *
 * Byte-true port of the PLAYER branch of FUN_80012d60 (the unified hit/damage
 * resolver). Every constant here cites the RE1.5 PSX.EXE disassembly
 * (ghidra1_V2.txt) — see re15_damage.h for the subsystem overview. The in-game
 * trigger (enemy attack-action FSM + actor hitbox test) is now WIRED: the hitbox test is
 * re15_hitbox_test (:660/:690) and the live enemy attack FSM is the @0x8011f7b4 family
 * (enemy_ai_common.c).
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
#include <stdlib.h>          /* getenv — RE15_RE2_DMG_MODEL (Negativ-Test-Hebel) */
#include "re15_damage.h"
#include "re15_math.h"       /* re15_squareroot0 — the engine's ONLY sqrt (BIOS 0x80065f60) */
#include "re15_skeleton.h"   /* re15_skel_compute_pose / re15_skel_bone_to_world / g_anim_pose_actor */
#include "re15_esp.h"        /* re15_esp_fx_spawn — zombie gore effect (FUN_80106a44) */
#include "re15_room.h"       /* g_current_room_id + g_room_change + re15_room_request_change (death continue) */
#include "re15_room_list.h"  /* re15_room_ids[] / RE15_ROOM_COUNT — current-room index for the continue spawn */
#include "re15_room_spawns.h"/* re15_room_spawns[] — the current room's entry spawn for the continue-reload */
#include "re15_enemy.h"      /* re15_enemy_find — the per-type EMD bank (skel/anim) for the gore-bone pose */
#include "re15_ai_flavor.h"  /* re15_re2_owns_type — der +0x1D2-Stempel des RE2-Flavors (s.u.) */

/* fix_1d2_spec — Definition vor take_damage. `row_src`/`row_id`: welcher Port-Erzeuger die
 * RE2-Trefferreaktions-ZEILE (+0x5) speist (0 = RE1.5-Waffen-Id, 1 = RE1.5-Angriffstyp). */
static void re15_re2_stamp_hit(re15_actor_t *e, int row_src, unsigned row_id);
#include "re15_anim_select.h"/* re15_compute_actor_kf — motion+frame -> keyframe for the gore-bone pose */

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
 * +0x6a heading map to the port's x/z/rot_y. NOW OBSERVABLE: the game-step hit-flinch
 * uses this to pick the directional hurt clip (front aca59=3 -> 0x9, back aca59=2 ->
 * 0x8, @0x80035de0/f64), reproducing the exact FUN_8001a7a8 formula (bearing minus
 * heading, +0x400-centred, split at 0x800) — derived from the disasm, not guessed. */
/* ==== Blut-Decal-Wundsystem (Spieler) — "zerrissene blutige Kleidung" ====================== *
 * Vollstaendig RE'd in analysis/blood_decals.md (alle Findings adversarial CONFIRMED, Ground
 * Truth stage_saves/mzd_blood_decals_hp30.sav). Original-Mechanik:
 *  - Tabelle @0x800b10ec: 8 Panels x {level, akku} + vorgebautes DR_MOVE-Prim (16hw x 64
 *    VRAM->VRAM-Blit, SetDrawMove @0x8006b824), gebaut beim Spieler-Load (FUN_80037c1c,
 *    Caller NUR @0x800316c8/@0x800318cc -> Reset bei jedem Spieler-Load, s. BD-6).
 *  - Akkumulator FUN_80037edc(panel, amount): akku += amount (@0x80037f04-08); erst ab
 *    Schwelle 0x78=120 (@0x80037f14 sltiu) wird gestempelt: akku=0 (@0x80037f24),
 *    level++ (@0x80037f2c), Clamp auf 2 (@0x80037f40/f50), srcY = level*128+128+
 *    (panel>=4 ? 0x40 : 0) (@0x80037f48-74), Prim in die OT (@0x80037f7c-fc4).
 *  - Quelle = Damage-Bank in Page 2 der Spieler-TIM (srcX = 576 + (panel&3)*16 hw,
 *    @0x80037c64-84); Ziel = statische Panel-LUT @0x80074208 (4 Chars x 8 (x,y)-hw-Paare).
 *  - Persistenz: VRAM + Tabelle ueberleben den Raumwechsel; Heilung loescht NICHT;
 *    Spieler-Load (New Game/Load) nullt (BD-6).
 * Der Port haelt level/akku hier; die Blit-Anwendung ist Render-Sache (PC: Slot-0-Stash,
 * main.c Wound-Sync; Re-Apply nach Atlas-Rebuild = Modell des toten Re-Inserts LAB_80037d1c). */
typedef struct { uint8_t level, acc; } re15_wound_t;
static re15_wound_t s_wounds[8];
static int          s_wounds_gen;

void re15_wound_reset(void)
{
    /* Builder FUN_80037c1c: level := 0 (@0x80037c48 sb zero,0) + akku := 0 (@0x80037ce8) */
    for (int i = 0; i < 8; i++) { s_wounds[i].level = 0; s_wounds[i].acc = 0; }
    s_wounds_gen++;
}

void re15_wound_add(int panel, int amount)
{
    if (panel < 0 || panel > 7) return;
    re15_wound_t *w = &s_wounds[panel];
    int sum = (int)w->acc + amount;               /* lbu +1; addu (@0x80037f04) — Vergleich auf
                                                   * dem UNtrunkierten Register wie das Original */
    w->acc = (uint8_t)sum;                        /* sb (@0x80037f08) */
    if (sum < 0x78) return;                       /* @0x80037f14 sltiu 0x78: <120 -> nur Akku */
    w->acc = 0;                                   /* @0x80037f24 */
    w->level = (uint8_t)(w->level + 1);           /* @0x80037f2c */
    if (w->level >= 3) w->level = 2;              /* @0x80037f40/f50 Level-Clamp auf 2 */
    s_wounds_gen++;                               /* OT-Einhaengen @0x80037f7c-fc4 -> Sync-Marke */
}

int re15_wound_level(int panel) { return (panel >= 0 && panel < 8) ? s_wounds[panel].level : 0; }
int re15_wound_generation(void) { return s_wounds_gen; }

/* Save-Serialisierung (analysis/save_injured_state.md SI-1): die Wund-Tabelle @0x800b10ec
 * liegt bei GSB+0x130 IM Original-Save-Blob (SAVE-memcpy a1=0x800b0fbc a2=0x1230
 * @0x800261c4-d8; LOAD-Restore @0x80026290-a0) — die Blut-Level werden mitgespeichert.
 * Load setzt alle 8 Panels exakt (ersetzt damit auch das fehlende wound_reset im
 * CONTINUE-Pfad, SI-3) und bumpt die Generation, damit der Platform-Wound-Sync nach dem
 * naechsten TIM-Upload automatisch re-stempelt. */
void re15_wound_save(uint8_t out[8][2])
{
    for (int i = 0; i < 8; i++) { out[i][0] = s_wounds[i].level; out[i][1] = s_wounds[i].acc; }
}
void re15_wound_load(const uint8_t in[8][2])
{
    for (int i = 0; i < 8; i++) {
        s_wounds[i].level = (uint8_t)((in[i][0] >= 2) ? 2 : in[i][0]);   /* Level-Clamp wie @0x80037f40 */
        s_wounds[i].acc   = in[i][1];
    }
    s_wounds_gen++;
}

/* Debug-Eingang des Originals (LAB_80037de4, im Shipped-Build OHNE Caller — der
 * "alles-verwunden"-Knopf): setzt ALLE 8 Levels auf das Byte @0x80074288 (initial 1)
 * und blittet alle; @0x80037e9c-ed0 zykelt das Byte 1->2->1. Port-Zugang: env
 * RE15_WOUND_DEBUG (Render-Pfad-Verifikation ohne Gameplay). */
void re15_wound_debug_all(int level)
{
    for (int i = 0; i < 8; i++) { s_wounds[i].level = (uint8_t)((level >= 2) ? 2 : (level <= 0 ? 0 : 1)); s_wounds[i].acc = 0; }
    s_wounds_gen++;
}

/* GRAB-RELEASE-Wund-Helper (KORRIGIERT 2026-08-03, analysis/player_hit_chain.md HIT-1/HIT-2:
 * der Dispatcher @0x8010a580 ist PHASE 4 der Grab-OPFER-FSM 0x8010a2cc (cmd 5) und laeuft
 * genau EINMAL pro UEBERLEBTEM Grab — aca59 ist dort die GRAB-RICHTUNG, kein Hit-Substate;
 * plain cmd-2-Hits stempeln NIE ueber diese Tabelle, die EXE-Handler [0]/[1] sind tot).
 * jalr 0x801201b8[dir] -> Helper: */
static const struct { int8_t panel; uint8_t amt; } s_wound_helper[4][3] = {
    { {0,10}, {5,50}, {7,50} },   /* dir 0 @0x8010a1cc — Grab von VORN ueberlebt  */
    { {0,10}, {4,50}, {7,50} },   /* dir 1 @0x8010a208 — Grab von HINTEN ueberlebt */
    { {1,50}, {-1,0}, {-1,0} },   /* dir 2 @0x8010a244 — Krabbler-Grab (offen)     */
    { {2,50}, {-1,0}, {-1,0} },   /* dir 3 @0x8010a268                             */
};

/* Stempel-Eintritt fuer den ueberlebten Grab-Release (player_hit_chain.md F2): Beleg
 * @0x8010a580-5b0 (lbu aca59; jalr 0x801201b8[dir]); Devour/Death stempeln NICHT. */
void re15_wound_release_stamp(int dir)
{
    if (dir < 0 || dir > 3) return;
    for (int i = 0; i < 3 && s_wound_helper[dir][i].panel >= 0; i++)
        re15_wound_add(s_wound_helper[dir][i].panel, s_wound_helper[dir][i].amt);
}

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

    /* type<2 → hit SE + bleed/poison roll (@80012e68-eb8). The SE plays FIRST (@0x80012e70
     * `jal FUN_800453d0` delay-slot `ori a0,0xa` = SE #10), THEN the bleed roll. re15_audio_room_se IS
     * FUN_800453d0 (verified: the maggot bite Se(6) @0x80118474 = `jal 0x800453d0`). Both RNG draws are
     * UNCONDITIONAL in the original: two back-to-back `jal FUN_8001af20` @0x80012e78/e80, THEN the two
     * `&1` results are AND'd @0x80012e88-90. A short-circuit `&&` would skip the 2nd draw when the 1st
     * is even, consuming ONE rng value where the PSX consumes TWO — and re15_engine_rand8 is the
     * SHARED engine stream (bleed AND AI decisions), so dropping a draw desyncs everything downstream. */
    if (type < 2) {
        extern void re15_audio_room_se(int idx);
        re15_audio_room_se(10);                /* hit SE #10 @0x80012e70 — plays before the bleed roll */
        uint32_t r1 = dmg_rng() & 1;
        uint32_t r2 = dmg_rng() & 1;           /* draw BOTH, unconditionally, before testing */
        if (r1 && r2)
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
    /* KEIN Wund-Stempel hier (KORREKTUR 2026-08-03, player_hit_chain.md F1/HIT-3): das Original
     * stempelt beim plain cmd-2-Hit NICHTS — Byte-Scan der EXE-Handler-Region 0x80035b70-0x80036700
     * ohne jeden 0x80037edc-Aufruf, und FUN_80012d60 ruft ihn ebenfalls nicht. Der fruehere
     * Hurt-Entry-Stempel hier war eine Fehldeutung des Dispatchers @0x8010a580 (der ist Phase 4
     * der Grab-Opfer-FSM -> re15_wound_release_stamp am Grab-Release). */
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
 * death sequence runs", and game_step freezes the player (skip player_tick) while dead. IMPLEMENTED
 * since (2026-07-05): the colour fade + white flash + death camera + YOU DIED fly-in + game-over
 * screen are the parallel game-over FSM (game_step_common.c re15_gameover_fsm_tick :68-126,
 * live-verified), and the zombie-grabbed eaten-animation FSM (@0x8010a28c) is the grab-victim
 * FSM (enemy_ai_common.c :497-528). STILL DEFERRED: only the BGM decrescendo + title-tail audio stop. */
#define RE15_DEATH_SEQ_FRAMES 0x78   /* @0x8003694c INIT: DAT_800acaf2 = 0x78 (120) death-fade timer */
static int s_death_seq = -1;         /* -1 = not in the death sequence; >=0 = frames left (120..0) */

int re15_player_is_dead(void)
{
    /* RE2-FRESS-/FINISHER-PACING (Nutzer-Report 2026-08-23 "Zombies fressen auf Becken- statt
     * Halshoehe"): Das RE2-Original schreibt den Spieler-Todesmarker PL+0x156 = -32768 erst in
     * Kollaps-PHASE 2 — dem Tick NACH dem Clip-Ende (Kollaps-Maschine 0x8010B464:
     * `sb 3,6(s2)` @0x8010B728 / `sh -32768,342(s2)` @0x8010B738). Der Biss-Kill laesst hp aber
     * schon am Kill-Tick negativ (FUN_800401d4-Zwilling re2z_player_damage: Todeszweig ohne
     * Restaurierung von DAT_800cfd4e). hp<0 als Praesentations-Gate war im RE2-Pfad damit
     * ~116 Ticks zu frueh: Blackout/Death-Cam ab Kill+77 deckte exakt F0..F77 der Devour-
     * Choreo = die Becken-Phase; die Hals-Formation (ab F110, Wurzel-Kurven CDEMD0.EMS EM010
     * Paar-2-Clip 24 vs. Paar-3-Clip 13, byte-gelesen) fiel komplett ins Schwarz.
     * Solange die RE2-Victim-Maschine laeuft, gilt der Spieler deshalb erst mit dem
     * P2-Handoff (state=7, enemy_ai_common.c at_end_prev-Zeile) als tot. Der RE1.5-Pfad
     * bleibt unveraendert (hp=-1 bei Kollaps-Frame 0x23, FUN_8010a6f8 @0x8010a80c-814). */
    {
        extern int      re15_player_victim_state(void);
        extern uint8_t  re15_player_victim_type(void);
        unsigned vt = re15_player_victim_type();
        if (re15_player_victim_state() != 0 &&
            g_actors[RE15_ACTOR_SLOT_PLAYER].state != 7 &&
            re15_ai_re2_for_type(vt) &&
            (re15_re2z_owns_type(vt) || vt == 0x20))   /* Zombie-Familie + RE2-Hund (dessen
                                                        * Maschine B schreibt HP ebenfalls erst
                                                        * am Ende — kein HP-Write @0x80111cb0-f08) */
            return 0;
    }
    /* signed HP < 0 = death (FUN_80012d60 @0x80012ee8). */
    return g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0;
}

void re15_player_death_reset(void) { s_death_seq = -1; }

int re15_player_death_tick(void)
{
    if (!re15_player_is_dead()) { s_death_seq = -1; return -1; }   /* alive -> not in the sequence */
    if (s_death_seq < 0) s_death_seq = RE15_DEATH_SEQ_FRAMES;      /* first dead frame = INIT (seed 120) */
    else if (s_death_seq > 0) s_death_seq--;                       /* sub-state 1: count the fade timer down */
    return s_death_seq;   /* 0 = the death sequence is complete -> game over (game_step_common.c re15_gameover_fsm_tick presents fade/YOU-DIED/death-cam) */
}

/* RE-style CONTINUE — fired by game_step when the death sequence completes (re15_player_death_tick
 * returned 0 = the byte-true 0x78 death timer expired). The port has NO game-over/continue screen:
 * the original PARKS at @0x8003694c (sets DAT_800aca59=2) and a SEPARATE EXE subsystem (the reader of
 * DAT_800aca59==2, NOT in the STAGE1 overlay) drives the continue/title. Per the user-chosen behavior
 * we do the RE "continue" = RELOAD THE CURRENT ROOM fresh: queue a room change to g_current_room_id at
 * its entry spawn; the main loop's re15_room_apply_pending re-inits the actors (re15_actor_init restores
 * player HP=100 -> re15_player_is_dead() clears -> the death branch exits) and the SCD respawns the
 * zombies at their feeding poses. The fade + "YOU DIED" presentation is the parallel game-over FSM (game_step_common.c re15_gameover_fsm_tick :68-126; g_death_flyin/g_death_fade/g_death_cam).
 * FL: the exact continue-vs-title routing is the missing subsystem; a current-room reload is the
 * faithful functional stand-in (the death handler never leaves the room by itself). */
void re15_player_continue_reload(void)
{
    if (g_room_change.pending) return;   /* a reload is already queued this frame — don't double-fire */
    unsigned room = g_current_room_id;
    int32_t sx = g_actors[RE15_ACTOR_SLOT_PLAYER].x;   /* fallback: where he died (walkable, he got there) */
    int32_t sy = g_actors[RE15_ACTOR_SLOT_PLAYER].y;
    int32_t sz = g_actors[RE15_ACTOR_SLOT_PLAYER].z;
    int16_t syaw = 0; int scut = 0;
    for (int i = 0; i < RE15_ROOM_COUNT; i++) {
        if (re15_room_ids[i] == room) {
            const re15_room_spawn_t *rs = &re15_room_spawns[i];
            if (rs->x || rs->y || rs->z) { sx = rs->x; sy = rs->y; sz = rs->z; }  /* the room's entry spawn */
            syaw = rs->yaw; scut = rs->cut;
            break;
        }
    }
    re15_room_request_change(room, sx, sy, sz, syaw, scut);   /* main loop applies it AFTER the AOT scan */
    re15_player_death_reset();                                /* clear the death-sequence counter for the fresh life */
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
 *   +0x6 = DAT_8006f410[heading>>0x1d]  (the hit-direction clip — IMPLEMENTED :487-494 via re15_player_aim_elevation)
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
 * none of which the port has). IMPLEMENTED since: the hit-direction clip
 * +0x6 = DAT_8006f410[heading>>0x1d] = {7,0,1,7,2,...} (:487-494 via re15_player_aim_elevation) and the
 * per-type non-zombie damage rows (s_wpn_dmg_* @:251-280) are ported. STILL DEFERRED: the
 * equipped-weapon source DAT_800aca5d (inventory) and the aim/fire input FSM. The aim-mode alternate reach row (@+0x58, DAT_800aca5c&4) is byte-IDENTICAL to the
 * base row -> nothing to port. Returns the hit enemy slot+1 (0 = no target in cone/reach). */

/* PER-TYPE per-weapon damage table (byte-true @0x8006e0d0: dmg = u16[type*0x58 + weapon*4], 22 weapons/
 * row). Read verbatim from PSX.EXE for every routed combat type — the damage varies a LOT per type (a
 * crow takes 2/shot, a tyrant 16, an alligator/maggot/birkin 30), and three types have ALL-ZERO rows =
 * WEAPON-IMMUNE: spider-baby 0x26 (tightly bracketed by 0x25=24 / 0x27=30), ivy 0x2d, and birkin-5 0x36
 * (an isolated zero between the populated 0x30-0x35 boss rows) — matching RE2, where ivies/final-forms
 * are not killable by normal gunfire. Zombies 0x10/0x11/0x12/0x16/0x18 + zgirl 0x13 share the row. */
static const uint16_t s_player_wpn_dmg_zombie[22] = {  /* 0x10/0x11/0x12/0x13/0x16/0x18 @0x8006e650 */
    0, 6, 24, 5, 5, 15, 15, 200, 40, 100, 200, 100, 10, 100, 10, 100, 200, 100, 400, 20, 0, 100
};
static const uint16_t s_wpn_dmg_dog[22]     = { 0,5,20,10,10,24,24,200,40,100,100,200,30,100,10,100,100,200,400,20,0,100 };  /* 0x20 */
static const uint16_t s_wpn_dmg_crow[22]    = { 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,100 };                            /* 0x21 */
static const uint16_t s_wpn_dmg_gator[22]   = { 0,7,30,7,7,21,21,50,30,70,30,30,8,50,8,70,30,30,400,20,0,100 };            /* 0x23 */
static const uint16_t s_wpn_dmg_aspider[22] = { 0,6,24,10,10,24,24,200,36,100,50,200,10,100,10,100,50,200,400,20,0,100 };  /* 0x25 */
static const uint16_t s_wpn_dmg_maggot[22]  = { 0,7,30,12,12,28,28,50,38,40,40,70,8,50,8,40,40,70,400,20,0,100 };          /* 0x27 */
static const uint16_t s_wpn_dmg_roach[22]   = { 0,7,30,14,14,25,25,200,40,50,100,200,10,80,15,50,100,200,400,20,0,100 };   /* 0x29 */
static const uint16_t s_wpn_dmg_tyrant[22]  = { 0,4,16,10,10,20,20,200,30,50,200,100,10,80,10,50,200,100,400,20,0,100 };   /* 0x2b */
static const uint16_t s_wpn_dmg_birkin[22]  = { 0,7,30,15,15,28,28,50,40,40,70,40,8,50,8,40,70,40,400,20,0,100 };          /* 0x30 */
static const uint16_t s_wpn_dmg_immune[22]  = { 0 };  /* 0x26 spider-baby / 0x2d ivy / 0x36 birkin-5 = weapon-immune */

/* ⛔ RE2-FLAVOR: die BABY-SPINNE (Typ 0x26) ist NUR in RE1.5 waffen-immun ------------------
 * VERIFIZIERT 2026-08-19 (Nebenbefund aus eb841053/9178ddba, eigener Zensus + Messung):
 *
 * (a) RE1.5 ist KORREKT und bleibt unangetastet. Zeile 0x26 @0x8006EDE0 (= 0x8006e0d0 + 0x26*0x58)
 *     ist 22x0, gedumpt aus PSX.EXE; die Tabelle hat GENAU EINEN Xref (`addiu a0,a0,-7984`
 *     @0x800124B8 — eigener Voll-Scan aller PSX.EXE-Instruktionen), also gibt es in RE1.5 keinen
 *     zweiten Schadensweg auf die Baby-Spinne. Der Crit-Instakill greift nicht
 *     (`sltiu v0,v0,0x20` @0x80012510, 0x26 >= 0x20). Sie ist byte-true unzerstoerbar.
 *
 * (b) RE2 gibt DEMSELBEN Typ-Index eine volle Zeile. Der RE2-Applier FUN_800470C0 rechnet
 *     (@0x80047218-68, selbst disassembliert):
 *       80047218: lbu v0,8(s1)            ; Gegner-TYP
 *       8004722c: lw  a1,27272(at)        ; a1 = *(u32*)(0x800A6A88 + typ*4)  Per-Typ-ZEIGER
 *       80047230-3c: v0 = (w*5)<<2 - 20   ; Zeile = 20 Byte je Waffe, w 1-basiert
 *       8004724c: lw  v1,0(a1)            ; Wort 0 = DREI 10-Bit-Werte
 *       80047254: srlv v1,v1,v0           ; v0 = Bracket*10   (s6 = a3>>28 @0x80047114)
 *       8004725c: andi v1,v1,0x3ff
 *       80047260/68: subu v0,v0,v1 / sh v0,342(s1)   ; HP(+0x156) -= dmg
 *     0x800A6A88[0x25] == 0x800A6A88[0x26] == 0x800A4B90 — Adult UND Baby teilen sich die Zeile.
 *     BRACKET = 0: in diesem Port ist das der belegte Wert (die Herleitung steht unten bei
 *     re15_re2_stamp_hit, "Bracket 0 = INNERSTE Teilbox = der direkte Treffer ... genau der
 *     RE1.5-Hitscan FUN_80011F50 des Ports"; Zensus aller 17 FUN_800470C0-Aufrufe: jeder DIREKTE
 *     Einschlag hat Bracket 0).
 *     WAFFEN-INDEX = die RE2-Item-Zeile, dieselbe Karte, die enemy_ai_re2_spider.c:1525
 *     (s_re2s_row_from_weapon) schon fuer den Trefferreaktions-Dispatch benutzt.
 *     Baby-HP = 1 (`sh v0,342` EMS26.BIN @0x801000F8) und die kleinste Zone-0-Zahl der Zeile ist
 *     10 -> in RE2 toetet JEDER Treffer die Baby-Spinne sofort.
 *
 * (c) DER FEHLER, den das behebt (GEMESSEN, probe_re2_baby_spider_hit, ROOM1090, echter
 *     game_step-Schussweg): im RE2-Modus lieferte die RE1.5-Nullzeile HP 1 - 0 = 1 >= 0, also
 *     `+0x4 = 2` (HURT, @0x80012520-2C). Das RE2-Baby-Modul hat aber gar keinen HURT-Handler —
 *     Wurzeltabelle @0x80101084 Eintrag [2] = 0x80100BB8 = `jr ra`. Messwert: nach dem ersten
 *     Schuss GENAU EIN Zustandswechsel in 300 Frames, +0x93 blieb 0x03 = die Spinne war
 *     PERMANENT eingefroren und nicht mehr treffbar. Mit der RE2-Zeile geht sie wie im Original
 *     direkt auf `+0x4 = 3` (DEATH @0x80103C80-Zwilling 0x80100BFC).
 *
 * Werte = Zone 0 von 0x800A4B90 + (RE2-Zeile-1)*20, je Waffe einzeln aus der EXE gelesen. */
static const uint16_t s_re2_wpn_dmg_spiderbaby[22] = {
    /* w0  -> r1  @0x800a4b90 */  15,
    /* w1  -> r1  @0x800a4b90 */  15,
    /* w2  -> r1  @0x800a4b90 */  15,
    /* w3  -> r3  @0x800a4bb8 */  17,
    /* w4  -> r2  @0x800a4ba4 */  17,
    /* w5  -> r4  @0x800a4bcc */  17,
    /* w6  -> r4  @0x800a4bcc */  17,
    /* w7  -> r5  @0x800a4be0 */ 130,
    /* w8  -> r7  @0x800a4c08 */  55,
    /* w9  -> r9  @0x800a4c30 */  60,
    /* w10 -> r11 @0x800a4c58 */  90,
    /* w11 -> r10 @0x800a4c44 */ 130,
    /* w12 -> r15 @0x800a4ca8 */  10,
    /* w13 -> r8  @0x800a4c1c */ 130,
    /* w14 -> r16 @0x800a4cbc */  10,
    /* w15 -> r9  @0x800a4c30 */  60,
    /* w16 -> r11 @0x800a4c58 */  90,
    /* w17 -> r10 @0x800a4c44 */ 130,
    /* w18 -> r17 @0x800a4cd0 */ 200,
    /* w19 -> r18 @0x800a4ce4 */  18,
    /* w20 -> r13 @0x800a4c80 */  17,
    /* w21 -> r1  @0x800a4b90 */  15
};

/* ============================================================================================
 * ⛔ RE2-FLAVOR — DAS VOLLSTAENDIGE RE2-HP-/SCHADENSMODELL (Nutzer-Auftrag 2026-08-19)
 * ============================================================================================
 * VORGESCHICHTE: der RE2-Modus fuhr bis hierher NUR das VERHALTEN; HP und Schaden blieben
 * RE1.5. Das stand mit Begruendung in enemy_ai_re2_zombie.c ("nur die 250 nachzuziehen waere
 * ein halbes Modell"). Der Nutzer hat diese Entscheidung aufgehoben ("die Zombies stecken viel
 * zu viel ein, viel mehr als im Original") — also wird das Modell VOLLSTAENDIG portiert:
 * Schadenszeile UND Start-HP, fuer JEDEN Typ, den das RE2-Gehirn besitzt.
 *
 * ---- (1) DER RE2-SCHADENSAPPLIER FUN_800470C0 (info/re2leon/PSX.EXE, selbst disassembliert)
 *   80047218: lbu  v0,8(s1)            ; Gegner-TYP
 *   80047220: sll  v0,v0,2
 *   80047224: lui  at,0x800a
 *   80047228: addu at,at,v0
 *   8004722c: lw   a1,27272(at)        ; a1 = *(u32*)(0x800A6A88 + typ*4)  PER-TYP-ZEIGER
 *   80047230: sll  v0,v1,2             ; v1 = Hitcode & 0xffff = RE2-ITEM-ZEILE (1-basiert)
 *   80047234: addu v0,v0,v1
 *   80047238: sll  v0,v0,2
 *   8004723c: addiu v0,v0,-20          ; Zeilen-Offset = 20*(w-1)  (20 Byte je Waffe)
 *   80047240: addu a1,a1,v0
 *   80047244: sll  v0,s6,2             ; s6 = Hitcode>>28 = BRACKET (@0x80047114)
 *   80047248: addu v0,v0,s6
 *   8004724c: lw   v1,0(a1)            ; Wort 0 = DREI 10-Bit-Werte (Zonen)
 *   80047250: sll  v0,v0,1             ; Schiebeweite = 10*Bracket
 *   80047254: srlv v1,v1,v0
 *   80047258: lhu  v0,342(s1)          ; HP (+0x156)
 *   8004725c: andi v1,v1,0x3ff
 *   80047260: subu v0,v0,v1
 *   80047268: sh   v0,342(s1)          ; HP -= dmg
 * BRACKET 0 = der direkte Einschlag — Herleitung unten bei re15_re2_stamp_hit, plus der Zensus
 * aller 17 `jal FUN_800470C0` in info/re2leon/PSX.EXE: jeder DIREKTE Treffer traegt Bracket 0.
 * WAFFEN-INDEX = die RE2-Item-Zeile aus derselben Karte, die der RE2-Trefferreaktions-Dispatch
 * schon benutzt (re2z_row_from_weapon, enemy_ai_re2_zombie.c:1676 == s_re2s_row_from_weapon,
 * enemy_ai_re2_spider.c:1556): {1,1,1,3,2,4,4,5,7,9,11,10,15,8,16,9,11,10,17,18,13,1}.
 *
 * PER-TYP-ZEIGER 0x800A6A88[typ] (selbst gedumpt, `table 0x800a6a88 48`):
 *   0x10 @0x800A6AC8 -> 0x800A412C | 0x11 @0x800A6ACC -> 0x800A412C
 *   0x12 @0x800A6AD0 -> 0x800A412C | 0x13 @0x800A6AD4 -> 0x800A412C
 *   0x16 @0x800A6AE0 -> 0x800A42A8 | 0x18 @0x800A6AE8 -> 0x800A412C
 *   0x20 @0x800A6B08 -> 0x800A4424 | 0x21 @0x800A6B0C -> 0x800A45A0
 *   0x25 @0x800A6B1C -> 0x800A4B90 | 0x26 @0x800A6B20 -> 0x800A4B90
 * Die Tabellen unten sind Zone 0 von `basis + 20*(zeile-1)`, je RE1.5-Waffe einzeln aus der
 * RE2-EXE gelesen (Generator-Lauf ueber info/re2leon/PSX.EXE, jede Zelle mit ihrer Adresse).
 * GEGENPROBE: die Spinnen-Zeile reproduziert s_re2_wpn_dmg_spiderbaby (Commit 49de51f3) Zelle
 * fuer Zelle — dieselbe Methode, dasselbe Ergebnis. */

/* 0x800A412C — Zombies 0x10/0x11/0x12/0x13/0x18 */
static const uint16_t s_re2_wpn_dmg_zombie[22] = {
    /* w0  -> r1  @0x800A412C */   3, /* w1  -> r1  @0x800A412C */   3,
    /* w2  -> r1  @0x800A412C */   3, /* w3  -> r3  @0x800A4154 */  16,
    /* w4  -> r2  @0x800A4140 */  16, /* w5  -> r4  @0x800A4168 */  16,
    /* w6  -> r4  @0x800A4168 */  16, /* w7  -> r5  @0x800A417C */ 900,
    /* w8  -> r7  @0x800A41A4 */ 200, /* w9  -> r9  @0x800A41CC */ 200,
    /* w10 -> r11 @0x800A41F4 */ 200, /* w11 -> r10 @0x800A41E0 */ 200,
    /* w12 -> r15 @0x800A4244 */   4, /* w13 -> r8  @0x800A41B8 */ 300,
    /* w14 -> r16 @0x800A4258 */  15, /* w15 -> r9  @0x800A41CC */ 200,
    /* w16 -> r11 @0x800A41F4 */ 200, /* w17 -> r10 @0x800A41E0 */ 200,
    /* w18 -> r17 @0x800A426C */ 900, /* w19 -> r18 @0x800A4280 */   8,
    /* w20 -> r13 @0x800A421C */  16, /* w21 -> r1  @0x800A412C */   3
};
/* 0x800A42A8 — Zombie-Typ 0x16 hat in RE2 eine EIGENE, schwaechere Zeile (Zeiger 0x800A6AE0
 * zeigt zusammen mit 0x15/0x17 auf diese Tabelle, alle anderen Zombie-Typen auf 0x800A412C). */
static const uint16_t s_re2_wpn_dmg_zombie16[22] = {
    /* w0  -> r1  @0x800A42A8 */   3, /* w1  -> r1  @0x800A42A8 */   3,
    /* w2  -> r1  @0x800A42A8 */   3, /* w3  -> r3  @0x800A42D0 */  11,
    /* w4  -> r2  @0x800A42BC */  11, /* w5  -> r4  @0x800A42E4 */  11,
    /* w6  -> r4  @0x800A42E4 */  11, /* w7  -> r5  @0x800A42F8 */ 900,
    /* w8  -> r7  @0x800A4320 */  60, /* w9  -> r9  @0x800A4348 */  80,
    /* w10 -> r11 @0x800A4370 */ 200, /* w11 -> r10 @0x800A435C */  80,
    /* w12 -> r15 @0x800A43C0 */   3, /* w13 -> r8  @0x800A4334 */  80,
    /* w14 -> r16 @0x800A43D4 */  12, /* w15 -> r9  @0x800A4348 */  80,
    /* w16 -> r11 @0x800A4370 */ 200, /* w17 -> r10 @0x800A435C */  80,
    /* w18 -> r17 @0x800A43E8 */ 900, /* w19 -> r18 @0x800A43FC */   6,
    /* w20 -> r13 @0x800A4398 */  11, /* w21 -> r1  @0x800A42A8 */   3
};
/* 0x800A4424 — Hund 0x20 */
static const uint16_t s_re2_wpn_dmg_dog[22] = {
    /* w0  -> r1  @0x800A4424 */  10, /* w1  -> r1  @0x800A4424 */  10,
    /* w2  -> r1  @0x800A4424 */  10, /* w3  -> r3  @0x800A444C */  18,
    /* w4  -> r2  @0x800A4438 */  18, /* w5  -> r4  @0x800A4460 */  18,
    /* w6  -> r4  @0x800A4460 */  18, /* w7  -> r5  @0x800A4474 */ 200,
    /* w8  -> r7  @0x800A449C */  59, /* w9  -> r9  @0x800A44C4 */ 300,
    /* w10 -> r11 @0x800A44EC */ 300, /* w11 -> r10 @0x800A44D8 */ 300,
    /* w12 -> r15 @0x800A453C */  10, /* w13 -> r8  @0x800A44B0 */ 300,
    /* w14 -> r16 @0x800A4550 */  12, /* w15 -> r9  @0x800A44C4 */ 300,
    /* w16 -> r11 @0x800A44EC */ 300, /* w17 -> r10 @0x800A44D8 */ 300,
    /* w18 -> r17 @0x800A4564 */ 300, /* w19 -> r18 @0x800A4578 */  18,
    /* w20 -> r13 @0x800A4514 */  18, /* w21 -> r1  @0x800A4424 */  10
};
/* 0x800A45A0 — Kraehe 0x21 */
static const uint16_t s_re2_wpn_dmg_crow[22] = {
    /* w0  -> r1  @0x800A45A0 */  15, /* w1  -> r1  @0x800A45A0 */  15,
    /* w2  -> r1  @0x800A45A0 */  15, /* w3  -> r3  @0x800A45C8 */  15,
    /* w4  -> r2  @0x800A45B4 */  15, /* w5  -> r4  @0x800A45DC */  15,
    /* w6  -> r4  @0x800A45DC */  15, /* w7  -> r5  @0x800A45F0 */ 200,
    /* w8  -> r7  @0x800A4618 */  60, /* w9  -> r9  @0x800A4640 */  60,
    /* w10 -> r11 @0x800A4668 */  60, /* w11 -> r10 @0x800A4654 */  60,
    /* w12 -> r15 @0x800A46B8 */  15, /* w13 -> r8  @0x800A462C */ 120,
    /* w14 -> r16 @0x800A46CC */  15, /* w15 -> r9  @0x800A4640 */  60,
    /* w16 -> r11 @0x800A4668 */  60, /* w17 -> r10 @0x800A4654 */  60,
    /* w18 -> r17 @0x800A46E0 */ 300, /* w19 -> r18 @0x800A46F4 */  30,
    /* w20 -> r13 @0x800A4690 */  15, /* w21 -> r1  @0x800A45A0 */  15
};
/* 0x800A4B90 — Spinnen 0x25 UND 0x26 (beide Zeiger zeigen auf dieselbe Tabelle). Identisch
 * zu s_re2_wpn_dmg_spiderbaby oben; die Baby-Konstante bleibt als Beleg-Anker stehen. */
#define s_re2_wpn_dmg_spider s_re2_wpn_dmg_spiderbaby

/* ---- der MODELL-SCHALTER (Port-Hebel, im Original nicht vorhanden) -------------------------
 * Nur damit der Negativ-Test das RE2-Modell ABschalten und die alten Trefferzahlen
 * reproduzieren kann. Default = AN (im RE2-Flavor). Der RE1.5-Pfad liest ihn nie: JEDE
 * RE2-Abfrage unten ist zusaetzlich mit `re15_ai_flavor() == RE15_AI_FLAVOR_RE2` gegatet. */
static int s_re2_model = 1;
static int s_re2_model_env_read = 0;
int re15_re2_damage_model(void)
{
    if (!s_re2_model_env_read) {
        const char *v = getenv("RE15_RE2_DMG_MODEL");
        s_re2_model_env_read = 1;
        if (v && (v[0] == '0' || v[0] == 'n' || v[0] == 'N')) s_re2_model = 0;
    }
    return s_re2_model;
}
void re15_re2_damage_model_set(int on)
{
    s_re2_model = on ? 1 : 0;
    s_re2_model_env_read = 1;    /* explizit gesetzt schlaegt die Umgebungsvariable */
}
/* Gilt fuer diesen Typ das RE2-Modell? (Flavor + Schalter + RE2-Besitz — alles typ-fest.)
 * MIXED (2026-08-23): der Flavor-Teil ist jetzt typ-bezogen, damit im MIXED-Modus GENAU der
 * Hund 0x20 die RE2-Schadenszeile (0x800A4424) und die RE2-INIT-HP bekommt und alles andere
 * seine RE1.5-Zeile @0x8006e0d0 behaelt. */
static int re15_re2_model_owns(uint8_t type)
{
    return re15_ai_re2_for_type((unsigned)type) && re15_re2_damage_model()
        && re15_re2_owns_type((unsigned)type);
}

/* ============================================================================================
 * ⛔ PORT-OPTION 2026-08-20 — DAS RE2-SCHADENS-/HP-MODELL AUCH IM RE1.5-MODUS
 * --------------------------------------------------------------------------------------------
 * AUSDRUECKLICHER NUTZER-AUFTRAG (woertlich): "Bei RE2 AI und RE2 AI + Model ist es so, dass
 * auch mit der Handfeuerwaffe Gliedmassen abgeschossen werden koennen. Bei RE1.5 AI noch nicht.
 * Und genau das, sowie die Schadenswerte fuer Zombies, moechte ich auch in RE1.5 AI haben."
 * Das ist damit KEIN byte-true Fix, sondern eine gewollte Abweichung vom RE1.5-Original — und
 * sie ist als solche geschaltet (re15_re15_re2z_import, re15_ai_flavor.h; Default AN, zurueck
 * mit RE15_RE15_RE2Z_IMPORT=0). Alles UNTER dem Schalter bleibt byte-true zu RE2: es sind
 * exakt dieselben Tabellen und dieselben Zuege wie im RE2-Modus, nur die AUSWAHL ist neu.
 *
 * ⛔ HP UND SCHADEN GEHEN NUR ZUSAMMEN. Nur eine der beiden Haelften waere eine unbelegte
 * Balance-Aenderung — genau der Fehler aus 717d13e0. Deshalb keyen re15_enemy_dmg_row() UND
 * re15_re2_hp_sync() auf DIESELBE Funktion hier.
 *
 * NUR DIE ZOMBIE-FAMILIE: re15_re15_re2z_import_owns() haengt an re15_re2z_owns_type
 * (0x10/0x11/0x12/0x13/0x16/0x18). Hund 0x20, Kraehe 0x21, Spinnen 0x25/0x26 sehen im
 * RE1.5-Modus weiterhin GENAU ihre RE1.5-Zeile @0x8006e0d0 und ihre RE1.5-INIT-HP
 * (Regressionswachen: tests/unit/test_re2_hp_model.c Abschnitt 7(b) fuer die Schadenszeile,
 * tests/unit/test_re15_re2z_import.c Abschnitt 8 fuer den Zerleger).
 * ========================================================================================== */
static int re15_re15_import_owns(uint8_t type)
{
    return re15_re15_re2z_import_owns((unsigned)type);
}

/* actor -> its byte-true damage row (@0x8006e0d0 + type*0x58). */
static const uint16_t *re15_enemy_dmg_row(const re15_actor_t *e)
{
    uint8_t type = e->type;
    /* RE2-Flavor: die Baby-Spinne faehrt das RE2-Gehirn (EMS26.BIN) mit RE2-HP; sie muss
     * deshalb auch die RE2-Schadenszeile bekommen, sonst ist sie unzerstoerbar UND friert im
     * nicht existierenden RE2-HURT-Zustand ein (Belege am Tabellen-Kopf).
     * ⚠ Die Baby-Spinne behaelt ihre Zeile AUCH bei abgeschaltetem Modell-Schalter: sie ist
     * kein Balance-Thema, sondern der Fix gegen den permanenten Einfrierer (49de51f3).
     * ⛔ KORREKTUR 2026-08-22: das Gate hing am TYP und hat damit auch die sieben
     * RDT-gesetzten 0x26er von ROOM1090 erwischt — und das sind in RE1.5 keine Spinnen, sondern
     * die FEUER-EMITTER (0x80072bac[0x26] = 0x80116288, Registrierung @0x8011E8F4/@0x8011E8FC).
     * Deren byte-true Zeile ist die NULLZEILE @0x8006EDE0 (s_wpn_dmg_immune unten): hp wird in
     * 0x80116288-0x80116DB4 nirgends gelesen, sie sind waffen-immun. Jetzt entscheidet die
     * HERKUNFT (re15_re2spider_baby_owns, enemy_ai_re2_spider.c). */
    if (re15_ai_re2_for_type((unsigned)type) && re15_re2spider_baby_owns(e))
        return s_re2_wpn_dmg_spiderbaby;   /* MIXED: 0x26 != 0x20 -> RE1.5-Nullzeile bleibt */
    /* ⛔ PORT-OPTION (Block oben): im RE1.5-Modus bekommt die ZOMBIE-FAMILIE dieselbe
     * RE2-Schadenszeile. re15_re15_import_owns() ist zombie-fest, der switch kann fuer diesen
     * Zweig also gar keine Hund-/Kraehen-/Spinnen-Zeile treffen. */
    if (re15_re2_model_owns(type) || re15_re15_import_owns(type)) {   /* die RE2-Schadenszeile */
        switch (type) {
            case 0x10: case 0x11: case 0x12: case 0x13: case 0x18:
                                return s_re2_wpn_dmg_zombie;     /* 0x800A412C */
            case 0x16: return s_re2_wpn_dmg_zombie16;            /* 0x800A42A8 */
            case 0x20: return s_re2_wpn_dmg_dog;                 /* 0x800A4424 */
            case 0x21: return s_re2_wpn_dmg_crow;                /* 0x800A45A0 */
            case 0x25: return s_re2_wpn_dmg_spider;              /* 0x800A4B90 */
            default:   break;                                    /* kein RE2-Zeiger -> RE1.5 */
        }
    }
    switch (type) {
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x16: case 0x18: return s_player_wpn_dmg_zombie;
        case 0x20: return s_wpn_dmg_dog;
        case 0x21: return s_wpn_dmg_crow;
        case 0x23: return s_wpn_dmg_gator;
        case 0x25: return s_wpn_dmg_aspider;
        case 0x27: return s_wpn_dmg_maggot;
        case 0x29: return s_wpn_dmg_roach;
        case 0x2b: return s_wpn_dmg_tyrant;
        case 0x30: return s_wpn_dmg_birkin;
        case 0x26: case 0x2d: case 0x36: return s_wpn_dmg_immune;   /* all-zero rows = weapon-immune */
        default:   return s_player_wpn_dmg_zombie;                  /* fallback (unrouted combat types) */
    }
}
/* ============================================================================================
 * ⛔ RE2-FLAVOR — (2) DIE START-HP (+0x156) AUS DEN RE2-EM-MODULEN
 * ============================================================================================
 * WELCHER ZWEIG GILT? Jedes RE2-EM-INIT waehlt seine HP-Tabelle ueber ZWEI Bits des
 * RE2-Spielworts DAT_800CFB74 (0x40 und 0x20). Der Port hat dieses Wort nicht — bisher stand
 * deshalb ueberall "kein Produzent -> beide Bits 0". DAS IST WIDERLEGT (eigener Voll-Scan aller
 * `sw`-Instruktionen mit Immediate 0xFB74 in info/re2leon/PSX.EXE + allen EM-Overlays; es gibt
 * GENAU 9 Schreiber):
 *   0x8002B4B4  sw v0,-1164(at)   in FUN_8002B48C = `DAT_800cfb74 |= 0x40`  (Boot-Systeminit)
 *   0x8002C698  = `DAT_800cfb74 &= 0x10008D9`  -> 0x8D9 & 0x40 = 0x40, das Bit UEBERLEBT
 *   0x80026320 |0x4 | 0x80026488 |0x08000000 | 0x8002B928 | 0x80038FE0 | 0x80039408 (AND mit
 *   0x…FFFF) | 0x80051BDC |0x00040000 | 0x8010295C |0x100
 * KEIN Schreiber setzt jemals 0x20, und keiner loescht 0x40.
 * => Im ausgelieferten RE2 ist BIT 0x40 IMMER GESETZT und BIT 0x20 NIE. Damit ist genau EIN
 *    Zweig je Modul erreichbar — der wird hier portiert, die uebrigen drei sind toter Code.
 *
 * ZOMBIE-FAMILIE (EMOVL10_S0.BIN, INIT @0x8010065C):
 *   801006d4: andi v0,v0,0x40        ; 801006d8: beq -> 0x8010075c   (0x40 gesetzt = Fallthrough)
 *   801006e0/e8: 2x jal 0x80015fe8   ; s0 = 1. Zug, v0 = 2. Zug
 *   801006f0: andi v0,v0,0x3         ; 801006f4: srlv s0,s0,v0
 *   801006f8: andi s0,s0,0xf         ; 801006fc: sll s0,s0,1
 *   80100708: lhu v0,-14736(at)      ; Tabelle 0x8010C670
 *   80100710: sh  v0,342(s2)         ; HP
 *   8010071c: andi v0,v0,0x20        ; 0x20 KLAR -> weiter bei 0x801007f0
 *   801007f4: lbu v0,-1037(v0)       ; DAT_800CFBF3
 *   801007fc: sltiu v0,v0,0x4        ; 80100800: beq -> 0x80100818
 *   80100810: addiu v0,v0,15         ; 80100814: sh v0,342(s2)   -> HP += 15 wenn Zaehler < 4
 *   801008bc: bne v1,v0(=17),0x801008d8   \  NUR Typ 0x11:
 *   801008c8: addiu v1,zero,250            > HP = 250 FEST (ueberschreibt den Wuerfelwert)
 *   801008cc: sh  v1,342(s2)               /
 * DAT_800CFBF3 IST BELEGT DIE GEGNERZAHL DES RAUMS, nicht geraten: FUN_80049E48 nullt sie beim
 * Raum-Reset (`sb zero,-1037(at)` @0x80049EE4) und initialisiert daneben die 33-Eintrag-
 * Entity-Liste ab 0x800CFE18; die beiden Entity-Allokatoren erhoehen sie (@0x80057220 /
 * @0x800578B4, `lbu/addiu 1/sb` auf s4+14859 = 0x800CFBF3, uebersprungen fuer Kategorie -1);
 * und der Auto-Aim-Zielscan FUN_80045C10 laeuft mit GENAU diesem Zaehler die Gegnerliste ab
 * 0x800CFE1C ab (`uVar5 = DAT_800cfbf3` / `while(uVar6--)` / `+0x156`-Test).
 * PORT-ANALOGON: die Zahl der Gegner-Aktoren im Raum (Typen 0x10..0x3F, Spieler-Slot aus).
 *
 * HUND (EMD0G_MOD0.BIN, INIT @0x80100150):
 *   80100160: andi v0,v0,0x40 -> gesetzt: 0x8010016C   80100180: andi 0x20 -> klar: 0x801001AC
 *   80100184: andi s1,v0,0x3          ; s1 = 1. Zug & 3
 *   801001C4: lhu v0,21424(at)        ; Tabelle 0x801053B0, Index (2. Zug & 0xf)
 *   801001CC: addu v0,v0,s1           ; 80100234: sh v0,342(s0)
 * KRAEHE (EMOVL21_S0.BIN): 80100324: addiu v0,zero,10 / 80100348: sh v0,342(s1) -> HP = 10 FEST
 * SPINNE (EMS25.BIN, INIT @0x801001FC):
 *   80100234: andi 0x40 -> gesetzt: 0x80100240        80100254: andi 0x20 -> klar: 0x80100280
 *   80100258: andi s0,v0,0x3          ; 80100298: lhu v0,25396(at) = Tabelle 0x80106334
 *   801002A0: addu v0,v0,s0           ; 80100308: sh v0,342(s2)
 * BABY-SPINNE (EMS26.BIN): HP = 1 (@0x801000F8) bzw. -1 = unverwundbar (@0x80100204) — bereits
 *   byte-true im Brain (enemy_ai_re2_spider.c:2320/2342) und hier BEWUSST NICHT angefasst.
 *
 * ⚠ Hund und Spinne setzen ihre HP heute schon selbst, aber aus dem WIDERLEGTEN Zweig
 *   ("beide Bits 0" -> 0x80105340 bzw. 0x801062C4). Der Stempel unten korrigiert sie auf den
 *   ausgelieferten Zweig, ohne die fremden Brain-Dateien anzufassen. Die Kraehe stempelt auf
 *   denselben Wert 10, den ihr Brain schon setzt (kein Verhaltensunterschied, aber EIN Ort).
 *   OFFEN (gemeldet, nicht stillschweigend): der Hunde-HURT-P4-Re-Roll @0x80103850
 *   (enemy_ai_re2_dog.c:1473) zieht weiter aus 0x80105340; er ist laut eigener Datei "im
 *   HURT-Fluss unerreicht" und liegt ausserhalb dieser Datei. */
/* ⛔ DER EINZIGE TYP-SPEZIFISCHE SONDERFALL DES MODELLS: RE2 setzt die HP von KIND 0x11 FEST
 * auf 250. SCHARF, ohne Schalter. Hier stand bis 2026-08-20 ein `RE15_RE2_ZOMBIE11_250`-Gate
 * mit der Begruendung "nicht belegt, dass RE1.5-Typ 0x11 dasselbe Wesen ist wie RE2-Kind 0x11".
 * Diese Begruendung ist WIDERLEGT; der Schalter ist weg. Die fuenf Belege, in der Reihenfolge,
 * in der sie die Frage schliessen:
 *
 * (1) MECHANISMUS — kein Index, keine Tabelle, kein Flag: ein DIREKTER Vergleich von ENTITY+0x8
 *     gegen 17. Luecklos selbst disassembliert (EMOVL10_S0.BIN; INIT = Dispatch-Slot 0 der
 *     Zustandstabelle @0x8010C830 -> 0x8010065C, `addu s2,a0,zero` @0x80100664 = Entity):
 *       80100894: lbu   v1,8(s2)          ; v1 = ENTITY+0x8 = KIND
 *       80100898: addiu v0,v0,16          ; (fremder Zug: +0x223 = (rand&0xf)+16)
 *       8010089c: sb    v0,547(s2)
 *       801008a0: addiu v0,zero,17        ; v0 = 17 = 0x11
 *       801008a4-b8: sb/sh zero,560/561/566/569/570/571   ; v0 und v1 UNBERUEHRT
 *       801008bc: bne   v1,v0,0x801008d8  ; jedes andere KIND ueberspringt
 *       801008c0: sb    zero,572(s2)      ; delay slot
 *       801008c4: lhu   v0,538(s2)
 *       801008c8: addiu v1,zero,250
 *       801008cc: sh    v1,342(s2)        ; +0x156 = HP = 250 (schlaegt Wurf UND das +15)
 *       801008d0: ori   v0,v0,0x8000
 *       801008d4: sh    v0,538(s2)        ; +0x21A |= 0x8000
 *     Beide DAT_800CFB74-Zweige des INIT laufen vor 0x80100894 zusammen (0x80100818) — kind
 *     0x11 erreicht die Zeile IMMER, es gibt keinen frueheren Ruecksprung.
 *
 * (2) DAS FELD IST IN BEIDEN SPIELEN DASSELBE. Der RE1.5-Zombie-INIT liest den Typ aus dem
 *     GLEICHEN Offset +0x8 (STAGE1.BIN, selbst disassembliert):
 *       801007c4/c8: a0 = 0x8011f034      ; RE1.5-eigene HP-Tabelle
 *       801007d8:    lbu v1,8(a1)         ; ENTITY+0x8 = TYP
 *       801007e0:    sll v1,v1,5          ; Zeile = typ*0x20, Spalte = (rng&0xf)*2
 *       801007ec:    lhu v0,0(v0)
 *       801007f4:    sh  v0,154(a1)       ; RE1.5-HP +0x9A
 *     Keines der beiden Spiele hat eine Uebersetzungsschicht: der Typ IST der kind.
 *
 * (3) DIE ZUORDNUNG DES PORTS IST BEREITS 1:1 — sie wird hier nicht erfunden, sondern
 *     EINGEHALTEN. Der RE1.5-Typ indiziert im RE2-Modus heute schon:
 *       - den RE2-ASSET-TOC: main.c pc_enemy_load(type) -> re2_ems_load_bank(.., kind=type, ..)
 *         -> re2_ems_toc_entry (re2_ems.c): `i = ((kind - 0x10) * 4 + rec) * 2`. Der TOC
 *         @0x8009ADF4 fuehrt fuer kind 0x11 EIGENE Assets (rec2=TIM Sektor 157 / 66592 B,
 *         rec3=EMD Sektor 190 / 149468 B — der groesste EMD der ganzen Zombie-Familie). Im
 *         ausgelieferten shared_assets/RE2/CDEMD0.EMS nachgeprueft: der EMD @Datei-0x5F000
 *         traegt dir_count == 8 (gueltiger RE2-EMD), und die EM011-TIM stimmt mit der
 *         EM010-TIM in nur 313 von 66592 Bytes ueberein = eine voellig andere Textur.
 *         Mit dem Default RE15_AI_MODELS=RE2 SIEHT der Spieler den RE1.5-Typ-0x11-Zombie
 *         also bereits als RE2-EM011.
 *       - die RE2-Schadenszeile 0x800A6A88[typ] (oben), re2_hybrid_perm(kind) (re2_ems.c),
 *         re15_re2_owns_type(typ), die ENEMSE-Bankwahl.
 *     RE2-EM011 IST BRAD VICKERS — info/Resident_Evil_und_Playstation_Information/
 *     BioModels-master/src/BioModels.h:186 `{ "EM011.EMD", BRAD_VICKERS }` (Zeile 170:
 *     EM010 = ZOMBIE_POLICE). Der eine, absichtlich zaehe Sonder-Zombie von RE2.
 *
 * (4) DIE ZWEITE HAELFTE DESSELBEN bne WAR SCHON SCHARF: enemy_ai_re2_zombie.c setzt seit
 *     Welle B `if (e->type == 0x11) e->re2z_flags21a |= 0x8000u;` mit Zitat @0x801008BC-D4 und
 *     OHNE jeden Schalter. Die 250 stammen aus GENAU DERSELBEN Verzweigung, aus den zwei
 *     Instruktionen direkt davor. Eine Haelfte scharf, die andere hinter einer
 *     Umgebungsvariable — das war ein Widerspruch, kein Vorbehalt.
 *
 * (5) GEGENPROBE RE1.5: dort ist 0x11 ein voellig normaler Zombie, und es gibt KEINEN
 *     Sonderfall, den man haette verletzen koennen. Zeile @0x8011f254 =
 *     {71,85,103,73,87,105,75,107,89,77,93,79,95,81,98,83} — dasselbe Band wie 0x10
 *     (@0x8011f234, 61..101), 0x12 (@0x8011f274, 71..111), 0x16 (@0x8011f2f4, 71..99).
 *     Voll-Scan ueber STAGE1.BIN: NULL `addiu rX,zero,250`, NULL `addiu rX,zero,17`.
 *     RE1.5 kennt keinen zaehen Zombie-Typ — die Zaehigkeit bringt der RE2-Modus mit.
 *
 * WIRKUNG, GEMESSEN (probe_re2_hp_model, ROOM1140, echter game_step-Weg mit Pad R1/SQUARE und
 * echten Sce_em_set-Spawns, 2026-08-20). Pistolentreffer bis zum Tod:
 *     Typ   RE1.5             RE2 (Modell AUS)   RE2 (Modell AN, dieser Stand)
 *     0x10  20 (hp 95/dmg 5)  20 (hp 95/dmg 5)   5  (hp  79 / dmg 16)
 *     0x11  15 (hp 71/dmg 5)  15 (hp 71/dmg 5)   16 (hp 250 / dmg 16)
 *     0x16  n/a (*)           18 (hp 85/dmg 5)   6  (hp  65 / dmg 11)
 *   (*) der 0x16 landet im RE1.5-Modus in diesem Messrahmen keinen Treffer (sein RE1.5-Brain
 *       laeuft aus der Reichweite) — die Sonde weist das als "0-" aus, das ist KEIN Nullschaden
 *       und keine Aussage ueber den RE1.5-Pfad; Abschnitt 1 des PIN-Tests deckt ihn separat ab.
 * Der Dinner-Room-Zombie 0x11 ist im RE2-Modus also rund 3x zaeher als seine Nachbarn — das ist
 * byte-true, im RE2-Modus IST er Brad. Bemerkenswert fuer den Balance-Eindruck: gegenueber dem
 * VORHERIGEN RE2-Modus (Modell AUS) bewegt sich seine Trefferzahl kaum (15 -> 16), weil die
 * RE2-Schadenszeile 0x800A412C ebenfalls dreimal hoeher liegt (16 statt 5). Nur mit dem Messer
 * (RE2-Zeile 3) stirbt er im 4000-Frame-Messbudget nicht mehr (250/3 = 84 noetige Treffer) —
 * das ist RE2s eigene Arithmetik, kein Port-Defekt.
 * Der EINZIGE Hebel, der die RE2-Zahlen abschaltet, bleibt RE15_RE2_DMG_MODEL=0 (Negativ-Test);
 * einen zweiten, typ-lokalen gibt es nicht mehr. */

static const uint16_t s_re2_hp_zombie[16] =     /* @0x8010C670 (EMOVL10_S0.BIN, selbst gedumpt) */
    { 80, 94, 128, 75, 60, 95, 58, 75, 50, 83, 79, 66, 80, 65, 82, 65 };
static const uint16_t s_re2_hp_dog[16] =        /* @0x801053B0 (EMD0G_MOD0.BIN) */
    { 129, 95, 95, 95, 129, 80, 95, 95, 80, 95, 69, 80, 69, 95, 69, 80 };
static const uint16_t s_re2_hp_spider[16] =     /* @0x80106334 (EMS25.BIN) */
    { 109, 109, 109, 109, 129, 109, 109, 129, 109, 109, 109, 129, 109, 99, 109, 109 };

/* Die Gegnerzahl des Raums = das Port-Analogon zu DAT_800CFBF3 (Beleg oben). */
static int re15_re2_room_enemy_count(void)
{
    int n = 0;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10u && g_actors[s].type < 0x40u) n++;
    return n;
}

/* Der RE2-INIT-HP-Zug fuer einen Typ. Rueckgabe < 0 = dieser Typ hat kein RE2-HP-Modell (dann
 * bleibt der Wert stehen, den sein Brain gesetzt hat). Zieht aus dem RE2-PRNG @0x80015FE8
 * (re15_re2_rand) — derselbe Generator, aus dem die Original-INITs ziehen.
 * ⚠ PORT-PLUMBING: der Port hat den RE1.5-INIT bereits laufen lassen (der seinerseits gewuerfelt
 * hat), diese Zuege kommen also ZUSAETZLICH in den Strom. Der RE2-Modus ist ohnehin ein Hybrid
 * (RE1.5-Raumdaten + RE2-Gehirn) und war nie strom-identisch zum RE2-Original. */
int16_t re15_re2_init_hp(const re15_actor_t *e)
{
    if (!e) return -1;
    switch (e->type) {
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x16: case 0x18: {
            uint32_t r1 = re15_re2_rand();                       /* @0x801006E0 */
            uint32_t r2 = re15_re2_rand();                       /* @0x801006E8 */
            int hp = s_re2_hp_zombie[(r1 >> (r2 & 3u)) & 0xfu];  /* @0x801006F0-710 */
            if (re15_re2_room_enemy_count() < 4)                 /* sltiu ..,0x4 @0x801007FC */
                hp += 15;                                        /* @0x80100810-14 */
            /* KIND 0x11 = RE2-EM011 (Brad Vickers): feste 250, ueberschreibt Wurf UND +15.
             * `bne v1,v0` @0x801008BC / `addiu v1,zero,250` @0x801008C8 / `sh v1,342(s2)`
             * @0x801008CC. Zwillingszeile derselben Verzweigung: das +0x21A-Bit 0x8000
             * (@0x801008D0-D4) in enemy_ai_re2_zombie.c re2z_init — beide gehoeren zusammen,
             * beide sind scharf. Voller Beleg im Block ueber dieser Funktion. */
            if (e->type == 0x11) hp = 250;
            return (int16_t)hp;
        }
        case 0x20: {
            uint32_t r1 = re15_re2_rand() & 3u;                  /* @0x8010016C/184 */
            return (int16_t)(s_re2_hp_dog[re15_re2_rand() & 0xfu] + (int)r1);  /* @0x801001AC-234 */
        }
        case 0x21: return 10;                                    /* @0x80100324/348 */
        case 0x25: {
            uint32_t r1 = re15_re2_rand() & 3u;                  /* @0x80100240/258 */
            return (int16_t)(s_re2_hp_spider[re15_re2_rand() & 0xfu] + (int)r1); /* @0x80100280-308 */
        }
        default: return -1;     /* 0x26 Baby: HP 1/-1 kommt byte-true aus seinem eigenen Brain */
    }
}

/* Stempelt die RE2-INIT-HP auf jeden RE2-eigenen Aktor, dessen INIT gerade gelaufen ist.
 * WARUM HIER UND NICHT IM BRAIN: die HP der Zombie-Familie entsteht in re15_enemy_ai_live_init
 * (enemy_ai_common.c) bzw. re2z_init (enemy_ai_re2_zombie.c) — beide Dateien werden in dieser
 * Session parallel bearbeitet und duerfen nicht angefasst werden. Der Stempel ist deshalb ein
 * Nachlauf: er feuert GENAU EINMAL pro Spawn, in dem Tick, in dem der INIT (Zustand 0) auf
 * einen Nicht-Null-Zustand umschaltet — also bevor irgendein Treffer fallen kann.
 * ⛔ EINZIGER AUFRUFER IST game_step (direkt nach re15_enemy_ai_run_all) — BEWUSST NICHT die
 * Schadenspfade. Ein zusaetzlicher "idempotenter" Aufruf in re15_player_weapon_fire stand hier
 * kurzzeitig drin, damit auch KI-Sonden ohne game_step das Modell sehen; er hat aber die
 * Invariante "ein Schuss senkt HP" gebrochen: laeuft der Stempel erst IM Schuss, ersetzt er die
 * (niedrigeren) RE1.5-INIT-HP durch die (hoeheren) RE2-HP und der Treffer sieht wie eine
 * HP-ERHOEHUNG aus. GEMESSEN: test_re2_livepath "ROOM1190 Hunde: HP unveraendert — kein
 * Schadens-Durchgriff" und der Knockdown-Block, der `e->hp = 60` selbst setzt. Der Stempel
 * gehoert an den Frame-Schritt; Harnesse, die nur re15_enemy_ai_run_all ticken, bleiben damit
 * exakt so, wie sie vor dieser Aenderung waren (sie sehen die RE1.5-INIT-HP). */
/* ⛔ 2026-08-20 — der Stempel laeuft JETZT AUCH IM RE1.5-MODUS, aber nur unter der Port-Option
 * und nur fuer die Zombie-Familie (Block ueber re15_re15_import_owns). Ohne die Option ist er
 * im RE1.5-Modus weiterhin ein VOLLSTAENDIGES No-op — inklusive des Port-Feldes
 * re2_hp_stamped, worauf test_re2_hp_model.c Abschnitt 5/6 pinnt. */
/* ⚠️ MIXED (2026-08-23): DIE EINZIGE der 30 Vergleichsstellen, die NICHT rein typ-fest war —
 * sie berechnete zwei GLOBALE Modi vorweg und waehlte danach die Besitzmenge. Im MIXED-Modus
 * sind BEIDE Seiten gleichzeitig wahr (Hund ueber die RE2-Seite, Zombie-Familie ueber die
 * RE1.5-Import-Seite), ein globaler Modus haette also zwangslaeufig eine der beiden verloren.
 * Aufgeloest, indem der Besitz PRO AKTOR entschieden wird (re15_hp_sync_owns) — RE15 und RE2
 * liefern dabei exakt dieselbe Menge wie vorher:
 *   RE15 : re15_ai_re2_for_type == 0 fuer alles -> nur der Import-Zweig (== `flavor == RE15`)
 *   RE2  : re15_ai_re2_for_type == 1 fuer alles -> nur der RE2-Zweig
 *   MIXED: 0x20 -> RE2-Zweig, Rest -> Import-Zweig */
static int re15_hp_sync_owns(uint8_t type)
{
    if (re15_ai_re2_for_type((unsigned)type))
        return re15_re2_damage_model() && re15_re2_owns_type((unsigned)type);
    return re15_re15_import_owns(type);   /* RE1.5-Seite: nur die Zombie-Familie (typ-fest) */
}

void re15_re2_hp_sync(void)
{
    /* Frueh-Ausstieg wie bisher (er ueberspringt AUCH das re2_hp_stamped-Ruecksetzen — darauf
     * pinnt test_re2_hp_model.c Abschnitt 5/6), nur flavor-verallgemeinert: raus, wenn WEDER die
     * RE2-Seite NOCH die RE1.5-Import-Seite ueberhaupt einen Typ stempeln koennte. */
    re15_ai_flavor_t fl = re15_ai_flavor();
    int re2_side   = (fl != RE15_AI_FLAVOR_RE15) && re15_re2_damage_model();
    int re15_side  = (fl != RE15_AI_FLAVOR_RE2)  && re15_re15_re2z_import();
    if (!re2_side && !re15_side) return;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) { e->re2_hp_stamped = 0; continue; }
        /* RE2-Modus: die ganze RE2-Besitzmenge. RE1.5-Modus: NUR die Zombie-Familie — Hund,
         * Kraehe und Spinne behalten dort ihre RE1.5-INIT-HP. MIXED: beides nebeneinander. */
        if (!re15_hp_sync_owns(e->type)) continue;
        if (e->state == 0) { e->re2_hp_stamped = 0; continue; }   /* INIT steht noch aus */
        if (e->re2_hp_stamped) continue;
        e->re2_hp_stamped = 1;
        int16_t hp = re15_re2_init_hp(e);
        if (hp >= 0) e->hp = hp;
    }
}

/* Per-weapon shot reach (UNK_8006e5a0, u32). 22 weapons. */
static const uint16_t s_player_wpn_reach[22] = {
    1000, 1100, 1000, 1000, 1100, 1000, 1200, 1000, 1500, 1000, 1000,
    1000, 1300, 1800, 1000, 1000, 1000, 1000, 1000, 1100, 1000, 1000
};

/* The equipped weapon (DAT_800aca5d). Byte-true default = 1: pristine STAGE1 briefing/combat savestates
 * read DAT_800aca5d == 1 (the handgun; 0x801fcd00 == ARMS01.EDH). Room-init sets it via FUN_800c00a8 —
 * which the inventory-weapon-select RE (RE15_INVENTORY_SUBSYSTEM.md §2.2) showed is a PER-CHARACTER
 * static table read `table[DAT_800aca5c]` @0x800c00d4 (char 0..14 -> 1, char 15 -> 0), NOT an inventory
 * dereference (that function lives in the DEBUG.BIN overlay @0x800C0000, all-?? in the EXE dump —
 * disassembled from savestate RAM). The in-game equip overwrites it at the status-screen CLOSE
 * (menu_common.c close_phase -> re15_player_set_equipped_weapon; byte-true equip-commit @0x80046688 —
 * the run-phase USE only changes the equip RECORD DAT_800b25c8). Default 1 = the pristine entry. */
static int s_player_weapon = 1;   /* RE-CORRECTED twice: aca5d = the inventory ITEM id. The byte-true
                                   * GAME-START equip = ITEM 1 = the COMBAT KNIFE (briefing loadout,
                                   * savestate-confirmed: mzd_stage1_briefing.sav DAT_800aca5d==1;
                                   * slot1 holds item 3 = the BROWNING HP x15, equipped only via the
                                   * menu -> equip_test.sav aca5d==3). Item class split @0x80074030:
                                   * 0-2 melee (knife dmg row [1]=6, reach [1]=1100), 3+ gun (row
                                   * [3]=5, reach [3]=1000). My interim default 3 was a start-state
                                   * divergence (parity oracle: the original spawns with the knife). */
int  re15_player_equipped_weapon(void) { return s_player_weapon; }
void re15_player_set_equipped_weapon(int weapon_id)
{
    if (weapon_id >= 0 && weapon_id < 22) {
        s_player_weapon = weapon_id;
        /* keep the equipped SLOT (DAT_800b25c8) in sync — the byte-true menu commit stores the
         * cursor SLOT; this id-based setter (tests/SCD) derives it via the FUN_8004dfec scan.
         * No match -> 0x80 (nothing equipped), the ea6c sentinel. */
        extern int  re15_inv_find_item(uint8_t id);
        extern void re15_inv_set_equipped_slot(int s);
        int s = re15_inv_find_item((uint8_t)weapon_id);
        re15_inv_set_equipped_slot(s >= 0 ? s : 0x80);
    }
}

/* AIM TARGET LATCH (byte-true FUN_8003703c(radius)): the nearest live hitbox-bearing enemy within
 * `radius`, ALL-AROUND (no angle-vs-facing gate) — the raise-sub auto-turn then slews toward it, so
 * the player faces whatever he locked. Radii per machine/sub: gun raise 30000
 * (0x7530), melee DRAW 2000 (0x7d0 @0x800355d0), melee RE-RAISE 5000 (0x1388 @0x80034f7c).
 * Returns 1 + its XZ. */
int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int best = -1; uint32_t bd = (radius > 0) ? (uint32_t)radius : 30000u;
    for (int s2 = RE15_ACTOR_SLOT_PLAYER + 1; s2 < RE15_ACTOR_MAX; s2++) {
        re15_actor_t *e = &g_actors[s2];
        if (!e->active) continue;
        if (e->hit_radius_min <= 0) continue;   /* byte-true: the aim-latch (FUN_8003703c), like the fire
                                                 * resolver, considers any entity with a DAMAGE HITBOX —
                                                 * not a zombie-only whitelist. So the player auto-turns
                                                 * toward EVERY combat enemy (weapon-immune ivies/etc. still
                                                 * lock the reticle; the non-combat types have no box). */
        if (e->state == 7 || e->hp < 0) continue;
        uint32_t d = (uint32_t)re15_enemy_player_dist(e, pl);
        /* NEAREST enemy ALL-AROUND — byte-true FUN_8003703c @0x8003703c: the auto-aim latch computes
         * ONLY SquareRoot0(dx^2+dz^2) and keeps the smallest (3 flag-priority buckets); it has NO
         * angle-vs-facing test anywhere. The player then AUTO-TURNS toward the pick (the caller slews
         * rot_y, player_common.c:350-354 = the original's FUN_8001a8f8), so a front-arc gate here was a
         * divergence — it stopped the player turning to an enemy off to the side/behind. */
        if (d < bd) { bd = d; best = s2; }
    }
    if (best < 0) return 0;
    *tx = g_actors[best].x; *tz = g_actors[best].z;
    return 1;
}

/* Public wrapper for the byte-true front/back hit test (FUN_8001a7a8): 1 if the source hit the
 * player's FRONT hemisphere, else 0. Used by the game-step hit-flinch to pick the directional hurt
 * clip (0x9 front / 0x8 back) when the live melee attacks apply hp directly (they don't route
 * through re15_player_take_damage, which would have set +0x5 = this+2 itself). */
int re15_player_hit_from_front(const re15_actor_t *p, int32_t sx, int32_t sz)
{
    return hit_from_front(p, sx, sz);
}

/* Nearest LIVE hostile actor to the player (or NULL) — the attacker identity for a contact hit.
 * The live melee attacks (grab/bite/leap/dive) all apply damage on adjacency, so when the player
 * takes a non-lethal hit the nearest alive, non-corpse enemy IS the one that struck (faithful-line
 * attacker-ID; the front/back formula + clip + knockback it feeds are byte-true). Skips the player,
 * dead bodies (state 7 corpse) and invulnerable NPCs (hp = -1). */
const re15_actor_t *re15_nearest_hostile(const re15_actor_t *pl)
{
    int best = -1; uint32_t bd = 0x7fffffffu;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (e->hp < 0) continue;            /* dead this frame / invulnerable NPC */
        if (e->state == 7) continue;        /* corpse */
        uint32_t d = (uint32_t)re15_enemy_player_dist(e, pl);
        if (d < bd) { bd = d; best = s; }
    }
    return (best < 0) ? NULL : &g_actors[best];
}

/* Backward hit-knockback push delta (byte-true FUN_800245d8(0x800) @0x80035f18): the hurt handler
 * shoves the player by rotating the step vector (mag,0,0) by Ry(facing + 0x800) — i.e. 180 deg
 * BEHIND the facing — using the same RotMatrixY step the walker uses (actor_locomotion.c). The
 * caller adds (*dx,*dz) to the player XZ (then clamps to the room walls) and decays mag by
 * DAT_800acaf2 = 50 each frame (0xc8 -> 0x96 -> 0x64 -> 0x32 -> 0 over 4 frames). */
void re15_player_knockback_delta(int16_t rot_y, int32_t mag, int32_t *dx, int32_t *dz)
{
    int32_t ang = ((int32_t)rot_y + 0x800) & 0xfff;
    if (dx) *dx =  (int32_t)((re15_cos_q12(ang) * mag) >> 12);
    if (dz) *dz = -(int32_t)((re15_sin_q12(ang) * mag) >> 12);
}

/* BLADE/HAND world point (byte-true: the melee slash passes *(0x800acbdc)+0x7b8 = the player's
 * bone-11 world-matrix translation as the hit-test point — the GUNS pass the player entity
 * position instead). The platform render feeds it per frame (1-frame stale, faithful-line). */
static int32_t s_hand_world[3];
static int     s_hand_valid = 0;
static int32_t s_hand_rot[9];               /* bone-11 (gun-bone) WORLD rotation R_gunbone (Q12, row-major) */
static int     s_hand_rot_valid = 0;
static int32_t dmg_isqrt(int64_t x);        /* fwd (BIOS SquareRoot0 clone, defined below) */
void re15_player_set_hand_world(int32_t x, int32_t y, int32_t z)
{
    s_hand_world[0] = x; s_hand_world[1] = y; s_hand_world[2] = z;
    s_hand_valid = 1;
}
void re15_player_set_hand_rot(const int32_t r[9])
{
    for (int i = 0; i < 9; i++) s_hand_rot[i] = r[i];
    s_hand_rot_valid = 1;
}
/* Byte-true FUN_80019e20 (@L70-75): the world position of a GUN-BONE-LOCAL offset {ox,oy,oz} =
 * R_gunbone * offset + T_gunbone, where the gun-bone matrix is posebuf+0x7a4 (bone 11). The port
 * feeds R (s_hand_rot = yawed_rot) + T (s_hand_world = kine+0x7b8) from the render each frame
 * (1-frame stale, faithful-line). Returns 0 if the render hasn't posed bone 11 yet (leave out[]). */
int re15_player_gunbone_world(int32_t ox, int32_t oy, int32_t oz, int32_t out[3])
{
    if (!s_hand_valid || !s_hand_rot_valid) return 0;
    out[0] = (int32_t)(((int64_t)s_hand_rot[0]*ox + (int64_t)s_hand_rot[1]*oy + (int64_t)s_hand_rot[2]*oz) >> 12) + s_hand_world[0];
    out[1] = (int32_t)(((int64_t)s_hand_rot[3]*ox + (int64_t)s_hand_rot[4]*oy + (int64_t)s_hand_rot[5]*oz) >> 12) + s_hand_world[1];
    out[2] = (int32_t)(((int64_t)s_hand_rot[6]*ox + (int64_t)s_hand_rot[7]*oy + (int64_t)s_hand_rot[8]*oz) >> 12) + s_hand_world[2];
    return 1;
}

/* Byte-true GUN hit-region — FUN_80012574 (@0x80012574), the predicate the weapon table @0x8006e548
 * dispatches for guns (weapon 0/3-8). It is a forward STRIP that is UNBOUNDED in range: FUN_800126c8
 * (@0x800126c8) tests the target against two semi-infinite triangles sharing apex V5 (the aim
 * direction) via three cross products (FUN_80012944 = ax*bz-az*bx); the distance (SquareRoot0
 * @0x800127a4) is computed ONLY AFTER the angular gate and compared to the min-dist accumulator
 * @0x8008f5e0 — i.e. it selects the NEAREST in-strip target, it is NOT a range cap. Corners in the
 * player-local frame (X = lateral, Z = forward), from FUN_80012574 @0x800125c4-64:
 *   V1=(50, r+200)  V2=(50, -(r+200))  V3=(650, r+reach)  V4=(650, -(r+reach))  V5=(650, 0)
 * each rotated by rot_y (FUN_8004f008 = RotMatrix(0,rot_y,0)); r = enemy hitbox radius (hbdata+6),
 * reach = the per-weapon table @0x8006e5a0. Rotation convention matches the port's cos/sin
 * (out.x=(c*X+s*Z)>>12, out.z=(-s*X+c*Z)>>12). VERIFIED against savestate 4: a zombie at dist 6695
 * tests INSIDE the strip (the original hit it; the port's old reach+radius cap missed it). */
static int64_t re15_wedge_cross(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    return (int64_t)ax * bz - (int64_t)az * bx;   /* FUN_80012944 */
}

/* ===== ELEVATIONS-BAND-STEMPEL — FUN_80012aa4 (@0x80012aa4, PSX.EXE) ========================= *
 * Der Gegner traegt sein Ziel-Band in word0 Bit 31/30/29 (UP/LEVEL/DOWN); der Resolver
 * FUN_80011f50 laesst einen Kandidaten nur durch, wenn `enemy.word0 & player.word0 & 0xE0000000`
 * gesetzt ist (@0x800120d0-ec). Gestempelt wird es im ACTIVE-Tail der Zombie-Familie, DIREKT vor
 * dem Sub-Dispatch @0x8011f80c[+0x9 & 0xf] — also fuer JEDES Sub-Mode-Nibble, auch Fresser (6)
 * und Liegende (7/8). Selbst disassembliert (STAGE1.BIN roh @0x80100000):
 *   801015c0-cc  lbu +0x9 ; andi 0x80 ; bne -> 0x80101600      (downed -> Block ueberspringen)
 *   801015d4-e0  word0 &= 0x1FFFFFFF                            (alle drei Bandbits weg)
 *   801015f0-fc  word0 |= 0x40000000                            (LEVEL — toter Store, s.u.)
 *   80101600-04  jal 0x80012aa4 ; ori a0,zero,0xbb8             (R = 3000, IMMER, auch downed)
 *   80101614-20  lbu +0x9 ; andi 0x80 ; beq -> 0x80101640
 *   80101628-3c  word0 &= ~0x40000000 ; jal 0x80012974 ; a0 = 0x1388 (5000)
 *   80101640-70  lbu +0x9 ; andi 0xf ; lw 0x8011f80c[..] ; jalr  (der Sub-Dispatch)
 * Derselbe Tail-Bauplan beim Zombie-Girl-Root: @0x8010b640-4c `word0 |= 0x40000000`,
 * @0x8010b650-54 `jal 0x80012aa4 / a0 = 0xbb8`, @0x8010b664-84 der Downed-LEVEL-Clear.
 *
 * FUN_80012aa4(R) selbst (PSX.EXE, selbst disassembliert):
 *   80012abc/acc/ad4  v1 = playerY(DAT_800aca8c) - enemyY(+0x38)
 *   80012af0-afc      word0 &= 0x1FFFFFFF          <- raeumt ALLE drei Bandbits, auch bei downed
 *                                                     (darum ist das LEVEL von 801015f8 tot)
 *   80012ac0/ad8/b0c-b1c  s0 = v1 / 1800   (Magic 0x91A2B3C5 = ceil(2^42/1800), `+n`, `sra 10`,
 *                                           `- (n>>31)` = Trunkierung Richtung 0 = C-`/`)
 *   80012b20-24       sltiu v0,s0,2  -> (unsigned)s0 < 2
 *   80012b2c-44         word0 |= 0x40000000   (LEVEL)
 *   80012b48-5c       dist = SquareRoot0(dx*dx + dz*dz) zum Spieler
 *   80012b64-84       v1 = |s0| * 1000        (`sll 5; subu; sll 2; addu; sll 3` = *31*4+1 = *125*8)
 *   80012b88-8c       sltu  dist, R + |s0|*1000
 *   80012b90-94       slt   |s0|*1000, dist          -> nur INNERHALB des Rings
 *   80012b9c/ba0        s0 > 0 -> word0 |= 0x80000000 (UP)
 *   80012bb8            sonst  -> word0 |= 0x20000000 (DOWN)
 * FUN_80012974(R): dist < R -> word0 |= 0x20000000 (@0x800129cc-f0), kein Clear.
 *
 * WAS DER PORT VORHER TAT (und was daran falsch war): `grid&0x80 ? (dist<5000?DOWN:0) : LEVEL`.
 * Das ist eine ERFUNDENE Bandtrennung — sie kennt weder den Hoehen-Index noch den Ring. Konkret
 * fehlte: (a) ein NICHT-niedergeschlagener Gegner innerhalb 3000 traegt ZUSAETZLICH das DOWN-Band
 * (ein nach unten gerichteter Schuss auf einen nahen stehenden/knienden Zombie ging ins Leere),
 * (b) das UP-Band fuer Gegner >= 1800 ueber dem Spieler, (c) LEVEL faellt weg, sobald der
 * Hoehen-Index ausserhalb {0,1} liegt.
 * GEGENPROBE gegen die fuenf im Original gemessenen word0-Werte (HASH-957757946319438E_resume.sav,
 * Spieler -7663/-17629, flacher Boden -> s0 = 0), die frueher hier im Kommentar standen:
 *   downed grid 0x80 dist 2307 -> aa4: LEVEL + Ring(0<2307<3000) DOWN ; downed: LEVEL weg,
 *                                 974(5000) DOWN            = 0x20000000  == gemessen
 *   downed grid 0x80 dist 5538 -> aa4: LEVEL, kein Ring     ; downed: LEVEL weg, 974: nein = 0
 *   downed grid 0x80 dist 5752 -> dito                                                     = 0
 *   stehend grid 0x00 dist 7075 -> aa4: LEVEL, kein Ring                       = 0x40000000
 *   liegend 0x16 grid 0x88 dist 7228 -> wie downed 5538                                    = 0
 * 5/5 exakt reproduziert.
 *
 * GELTUNGSBEREICH (Zensus aller `jal` auf die drei Band-Helfer in STAGE1..5.BIN, selbst
 * gescannt): 0x80012aa4 wird IMMER mit a0 = 0xbb8 gerufen — STAGE1 @0x80101600 (Zombie-ACTIVE),
 * @0x8010b650 (Zombie-Girl), @0x8011d49c/@0x8011da30 (der generische 0x47-Root). Andere
 * Gegner-Familien stempeln ANDERS und bleiben hier unangetastet: Hund @0x8010dd38-4c
 * (`word0 |= 0x40000000` + `jal 0x80012974 / a0 = 0xfa0`), Kraehe @0x801125ac (974 mit 0x1770)
 * / @0x80112594 (0x80012a0c) — die faehrt weiter ueber e->aim_band. */
static uint32_t re15_band_stamp_aa4(const re15_actor_t *pl, const re15_actor_t *e,
                                    uint32_t dist, int32_t R)
{
    int32_t vd   = pl->y - e->y;                       /* @0x80012abc/acc/ad4 */
    int32_t s0   = vd / 1800;                          /* @0x80012ad8/b0c-b1c (Magic 0x91A2B3C5) */
    int32_t ring = (s0 < 0 ? -s0 : s0) * 1000;         /* @0x80012b64-84 */
    uint32_t band = 0;
    if ((uint32_t)s0 < 2u) band |= 0x40000000u;        /* sltiu s0,2 @0x80012b20 -> @0x80012b3c-44 */
    if (dist < (uint32_t)(R + ring) &&                 /* sltu @0x80012b88-8c */
        ring < (int32_t)dist)                          /* slt  @0x80012b90-94 */
        band |= (s0 > 0) ? 0x80000000u : 0x20000000u;  /* @0x80012b9c/ba0 UP | @0x80012bb8 DOWN */
    return band;
}
/* Die Zombie-Familie = genau die Typen, deren ACTIVE-Tail der oben zitierte Block ist
 * (Live-Zombies 0x10/0x11/0x12/0x16/0x18 ueber FUN_80101224 @0x80101600, Zombie-Girl 0x13 ueber
 * @0x8010b650). */
static int re15_band_is_zombie_family(uint8_t type)
{
    return type == 0x10u || type == 0x11u || type == 0x12u ||
           type == 0x13u || type == 0x16u || type == 0x18u;
}
/* FUN_800126c8: enemy (relx,relz) inside the semi-infinite triangle (VA,VB,V5). */
static int re15_wedge_tri(int32_t VAx, int32_t VAz, int32_t VBx, int32_t VBz,
                          int32_t V5x, int32_t V5z, int32_t relx, int32_t relz)
{
    if (re15_wedge_cross(V5x, V5z, relx - VAx, relz - VAz) > 0) return 0;   /* @0x80012734 bgtz -> out */
    if (re15_wedge_cross(V5x, V5z, relx - VBx, relz - VBz) < 0) return 0;   /* @0x80012758 bltz -> out */
    if (re15_wedge_cross(VAx - VBx, VAz - VBz, relx - V5x, relz - V5z) > 0) return 0;  /* @0x8001278c bgtz */
    return 1;
}
static int re15_gun_wedge_inside(const re15_actor_t *pl, int32_t ex, int32_t ez,
                                 int32_t reach, int32_t radius)
{
    int32_t c = re15_cos_q12(pl->rot_y), s = re15_sin_q12(pl->rot_y);
    int32_t Vx[5], Vz[5];
    static const int32_t lx[5] = { 50, 50, 650, 650, 650 };
    int32_t lz[5]; lz[0] = radius + 200; lz[1] = -(radius + 200);
    lz[2] = radius + reach; lz[3] = -(radius + reach); lz[4] = 0;
    for (int i = 0; i < 5; i++) {
        Vx[i] = (int32_t)(( (int64_t)c * lx[i] + (int64_t)s * lz[i]) >> 12);
        Vz[i] = (int32_t)((-(int64_t)s * lx[i] + (int64_t)c * lz[i]) >> 12);
    }
    int32_t relx = ex - pl->x, relz = ez - pl->z;
    return re15_wedge_tri(Vx[0],Vz[0], Vx[1],Vz[1], Vx[4],Vz[4], relx, relz)    /* triangle (V1,V2,V5) */
        || re15_wedge_tri(Vx[2],Vz[2], Vx[3],Vz[3], Vx[4],Vz[4], relx, relz);   /* triangle (V3,V4,V5) */
}

int re15_player_weapon_fire(int weapon_id)
{
    if (weapon_id < 0 || weapon_id >= 22) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint32_t reach = s_player_wpn_reach[weapon_id];
    /* the melee hit-test ORIGIN = the blade/hand point (@0x800353a4-c8); guns = player pos */
    int32_t ox = pl->x, oz = pl->z;
    if (weapon_id < 3 && s_hand_valid) { ox = s_hand_world[0]; oz = s_hand_world[2]; }

    /* auto-aim: nearest live zombie in front, within reach (DAT_8008f5e0 min-dist, seed 0x7fffffff).
     * ONCE-PER-TARGET latch (byte-true FUN_80011f50 internals, melee-FSM arbitration): candidates
     * with +0x93 bits 0+1 BOTH set are excluded (@0x800120c0/f4 0x3000000-mask... the +0x93-pair
     * equivalent); a selected target that already carries bit0 gets |=2 and the resolver RECURSES
     * to the next-nearest (@0x80012404-18). Without this the knife's 6-tick damage window
     * (frames 6-11, one resolver call per tick) would deal 6x damage to one target — and one
     * slash can legitimately wound TWO zombies (the recursion). Guns call this once per shot,
     * where the latch is a no-op on fresh targets. */
    int best = -1;
    uint32_t best_dist = 0x7fffffffu;
retry_after_latch:
    best = -1; best_dist = 0x7fffffffu;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        /* BYTE-TRUE target set: FUN_80011f50 iterates ALL entities and tests each one's DAMAGE HITBOX
         * (+0x78 present) — NOT a per-type whitelist. apply_hitbox gives a radius to every combat enemy
         * (zombies 0x10/0x11/0x12/0x16/0x18, dog 0x20, crow 0x21, spiders 0x25/0x26, maggot 0x27,
         * cockroach 0x29, birkin 0x30/0x36, alligator 0x23, tyrant 0x2b, zgirl 0x13, AND the
         * writher 0x1a — its INIT installs a real 300-radius +0x78 box @0x8012091c, so it IS shootable:
         * spawn HP=0 + type<0x20 -> one hit kills, audit wf_efd92a2c writher #5); the truly non-combat
         * types (fx-emitter 0x24 / stub 0x22 / NPC 0x40 / IVY 0x2d) get NO box and are excluded
         * automatically (the ivy is a dormant weapon-immune prop, audit wf_efd92a2c ivy #77).
         * So gate on the hitbox, which makes every ported enemy with a box shootable. */
        if (e->hit_radius_min <= 0) continue;   /* no damage hitbox -> not a valid auto-aim target */
        if (e->state == 7) continue;   /* RE15_AI_STATE_CORPSE — already a corpse (literal: avoid the AI-header dep) */
        /* ⛔ RE2-FLAVOR / BABY-SPINNE: HP < 0 heisst in RE2 "kein gueltiges Ziel", nicht "gleich
         * tot". Der RE2-Applier verwirft solche Kandidaten VOR dem Schaden:
         *   80047148: lh   v0,342(s0)          ; HP (+0x156)
         *   80047150: bltz v0,0x8004740c       ; HP < 0 -> naechster Kandidat
         * Der RE1.5-Resolver kennt dieses Gate nicht — er rechnet HP -= dmg und liest nur das
         * Vorzeichen (@0x80012520-2C). Die Baby-Spinne mit Spawn-Deskriptor 0 setzt sich in ihrem
         * RE2-INIT aber genau darueber auf UNVERWUNDBAR (`sh -1,342` EMS26.BIN @0x80100204, Zweig
         * `+0x10E & 0xFF == 0` @0x801001F8). GEMESSEN (probe_re2_baby_spider_hit, ROOM1090 Baby #0,
         * grid 0x00): sie ging beim ERSTEN Schuss auf `+0x4 = 3` (Tod) statt unberuehrt zu bleiben.
         * Eng gefasst auf genau diesen Fall (RE2-Flavor + Typ 0x26) — die RE1.5-Semantik von
         * HP < 0 bei allen anderen Typen bleibt unveraendert. */
        /* MIXED: typ-bezogen (0x26 != 0x20) -> RE1.5-Semantik von HP<0 bleibt dort. */
        if (e->type == 0x26u && re15_ai_re2_for_type(e->type) && e->hp < 0) continue;
        if ((e->hit_react & 0x3) == 0x3) continue;   /* already hit + re-touched this attack -> excluded */
        /* ELEVATION-BAND gate (byte-true @0x800120d0-ec: candidate needs
         * enemy.word0 & player_word & 0xe0000000 != 0, player band = acaec<<16 ->
         * UP bit31 / LEVEL bit30 / DOWN bit29).
         *
         * Das Gegner-Band selbst kommt aus dem ACTIVE-Tail-Stempel — die vollstaendige
         * Herleitung (FUN_80012aa4 @0x80012aa4, der Tail @0x801015c0-0x80101640, die Ring-Formel
         * und die 5/5-Gegenprobe gegen die Savestate-Messwerte) steht im Kopf von
         * re15_band_stamp_aa4 weiter oben. Kurz: NICHT-downed heisst NICHT pauschal LEVEL. */
        {
            extern int re15_player_aim_elevation(void);
            int elev = re15_player_aim_elevation();
            uint32_t pband = (elev > 0) ? 0x80000000u : (elev < 0) ? 0x20000000u : 0x40000000u;
            uint32_t eband;
            if (e->type == 0x21) {
                /* KRAEHE: das Band kommt aus dem ACTIVE-Tail-Stempel (@0x80112560-c8) —
                 * UP 0x80000000 (0x80012a0c(0x1770): vert>=4001 && dist<6000) / DOWN
                 * 0x20000000 (0x80012974(0x1770): vert<800) / LEVEL 0x40000000 / 0 = kein
                 * Ziel. Vorher traf der Port die fliegende Kraehe pauschal mit LEVEL.
                 * (crow_shot_attack.md F5, CONFIRMED) */
                eband = (e->aim_band == 4) ? 0x80000000u :
                        (e->aim_band == 1) ? 0x20000000u :
                        (e->aim_band == 2) ? 0x40000000u : 0u;
            } else {
                /* Die Helfer messen vom SPIELER (0x800aca88/90 gegen entity +0x34/+0x3c), nicht vom
                 * Nahkampf-Klingenpunkt — also hier immer die Spieler-Distanz, unabhaengig von der
                 * Waffe. */
                int32_t bdx = e->x - pl->x, bdz = e->z - pl->z;
                uint32_t bdist = (uint32_t)dmg_isqrt((int64_t)bdx*bdx + (int64_t)bdz*bdz);
                if (re15_band_is_zombie_family(e->type)) {
                    /* Der byte-true ACTIVE-Tail-Stempel: aa4(0xbb8) IMMER (@0x80101600-04) — auch
                     * fuer einen niedergeschlagenen Gegner —, danach der Downed-Nachlauf. */
                    eband = re15_band_stamp_aa4(pl, e, bdist, 0xbb8);
                    /* LIEGEND-KLASSIFIKATION: RE1.5-Zwilling = grid&0x80 (@0x80101614-20).
                     * Fuer RE2-OWNED Zombies zaehlt ZUSAETZLICH das RE2-eigene Liege-Bit
                     * +0x21A & 0x2 — exakt das Bit, auf dem RE2s eigene Treffer-Routung
                     * "liegend" entscheidet (`lhu v1,538 / andi v0,v1,0x2` @0x80105168-70;
                     * Produzenten: EXEC[5]-P0 |0x202, EXEC[11]-P0 |0x2; Clear EXEC[5]-P7
                     * `andi ~0x2` @0x801036C8-CC). Noetig fuer den 0x20501-Pfad (Kriecher-
                     * Abwurf-Flop @0x801045D4 betritt EXEC[5] OHNE P0): der geflopte
                     * Kriecher trug kein grid&0x80 und war nicht als liegend klassifiziert
                     * = untreffbar (w20-Trace 2026-08-24). Monotone Erweiterung (ODER):
                     * alle bisherigen Liege-Fenster klassifizieren unveraendert. */
                    {   int lying = (e->grid_id & 0x80) ||
                                    (re15_ai_re2_for_type(e->type) &&
                                     re15_re2z_owns_type(e->type) &&
                                     (e->re2z_flags21a & 0x2u));
                        if (lying) {
                            eband &= ~0x40000000u;            /* @0x80101624-3c LEVEL weg */
                            if (bdist < 0x1388u) eband |= 0x20000000u;  /* 0x80012974(0x1388)
                                                               * @0x800129cc-f0 */
                        }
                    }
                } else {
                    /* OFFEN (kein Rate-Ersatz, sondern der alte, bewusst konservative Stand): die
                     * uebrigen Familien stempeln mit anderen Helfern/Radien — Hund @0x8010dd38-4c
                     * (`|= 0x40000000` + 0x80012974(0xfa0)), Maggot @0x801173a8-b8, die generischen
                     * 0x47-Roots @0x8011d49c/@0x8011da30 (aa4(0xbb8), gleiche Form wie oben). Bis
                     * jede davon einzeln disassembliert ist, bleibt fuer sie das bisherige Verhalten
                     * unveraendert stehen. */
                    if (e->grid_id & 0x80)
                        eband = (bdist < 0x1388u) ? 0x20000000u : 0u;  /* @0x80101630-38 -> @0x800129cc-f0 */
                    else
                        eband = 0x40000000u;                           /* @0x801015f4-fc */
                }
            }
            if ((pband & eband) == 0) continue;
        }
        /* distance from the hit-test ORIGIN (0x800127fc measures POINT->target: lh 0(a2) vs
         * actor+0x34): guns = player pos; melee = the bone-11 blade point (extends the effective
         * knife range forward, byte-true). */
        int32_t ddx = e->x - ox, ddz = e->z - oz;
        uint32_t dist = (uint32_t)dmg_isqrt((int64_t)ddx*ddx + (int64_t)ddz*ddz);
        /* GUNS (weapon 0/3-8 -> FUN_80012574 @0x8006e548): the byte-true forward STRIP, UNBOUNDED
         * in range (re15_gun_wedge_inside; dist is only for the nearest-target min-select below, NOT
         * a cap — proven vs savestate 4, a zombie hit at dist 6695). The old `dist >= reach+radius`
         * cap was a decode error that limited guns to ~1400u = the "can't hit them" bug.
         * MELEE (weapon 1/2 -> FUN_800127fc): the bounded cone tester R = reach + enemy radius. */
        /* ⛔ KORREKTUR 2026-08-18 (Nutzer-Vorgabe "einzelne staerkere Waffen muessen reagieren"):
         * die Menge stand frueher als `w == 0 || (3 <= w <= 8)` da und verfehlte 12, 13, 19, 21.
         * Die Tester-Dispatch-Tabelle @0x8006E548 (selbst gedumpt, `table 0x8006e548 22`) sagt:
         *   [0]=0x80012574 [1]=0x800127FC [2]=0x800127FC [3..8]=0x80012574 [9,10,11]=0x800128A0
         *   [12]=0x80012574 [13]=0x80012574 [14..18]=0x800128A0 [19]=0x80012574 [20]=0x800128A0
         *   [21]=0x80012574
         * Ingram M10 (12), SPAS-12 (13) und H&K MC51 (19) liefen damit im Port durch den
         * begrenzten Nahkampf-Kegel (Reach 1300/1800/1100 @0x8006E5A0) statt durch den
         * unbegrenzten Schuss-Streifen — genau die "starke Waffe trifft/reagiert nicht". */
        int is_gun_strip = (weapon_id == 0 || (weapon_id >= 3 && weapon_id <= 8) ||
                            weapon_id == 12 || weapon_id == 13 || weapon_id == 19 ||
                            weapon_id == 21);
        if (is_gun_strip) {
            if (!re15_gun_wedge_inside(pl, e->x, e->z, (int32_t)reach,
                                       (int32_t)((uint32_t)e->hit_radius_min & 0xffffu))) continue;
        } else {
            uint32_t R = reach + ((uint32_t)e->hit_radius_min & 0xffffu);
            if (dist >= R) continue;                                         /* out of (reach + radius) */
            if (re15_ai_arc_test(pl, e->x, e->z, 0x400) != 0) continue;      /* not in front */
        }
        if (dist < best_dist) { best_dist = dist; best = s; }
    }
    if (best < 0) return 0;   /* no target in cone/reach */
    /* CRIT-CLASS flag +0x93 |= 0x40 (byte-true FUN_80011f50 LAB_80012370, decompile lines 145-148:
     * `+0x93 &= 1; if ((wpn==8 && dist<3000) || wpn==7) +0x93 |= 0x40`) — set BEFORE the bit0-latch
     * branch, so it also lands on a hit-guarded target. Types < 0x20 die from it (the hp=-1 crit
     * below, decompile line 170-172); types >= 0x20 carry the flag: the MAGGOT's airborne/pin
     * handlers read bits (2|0x40) as the "shot down by magnum/close shotgun" knockout
     * (@0x80118b1c-28 leap crash, @0x80118eb8-c4 finisher crash, @0x8011a890-98 pin abort).
     * (audit wf_827f186d maggot #7) */
    g_actors[best].hit_react = (uint8_t)(g_actors[best].hit_react & 0x1);   /* +0x93 &= 1 (line 145) */
    if (weapon_id == 7 || (weapon_id == 8 && best_dist < 3000u))
        g_actors[best].hit_react |= 0x40;
    if (g_actors[best].hit_react & 0x1) {            /* hit earlier in THIS attack window */
        g_actors[best].hit_react |= 0x2;             /* +0x93 |= 2 (@0x8001240c) */
        goto retry_after_latch;                      /* the @0x80012418 recursion (seek 2nd victim) */
    }

    re15_actor_t *e = &g_actors[best];
    int dmg = re15_enemy_dmg_row(e)[weapon_id];     /* byte-true PER-TYPE per-weapon damage @0x8006e0d0 */
    e->sub_state_1 = (uint8_t)weapon_id;            /* +0x5 = reaction clip = weapon_id (@0x800124bc) */
    e->hp          = (int16_t)(e->hp - dmg);        /* +0x9a -= dmg */
    e->hit_react  |= 0x1;                           /* +0x93 |= 1 (one-hit guard) */
    /* FRONT/BACK latch (FUN_80011f50: FUN_8001a780(entity) -> +0x93 |= 0x80, cluster F2): shot
     * heading aligned with the zombie's -> the standing death falls FORWARD with clip 0xb instead
     * of 0xd (FUN_80106c18 reads (s8)+0x93>>7). a780 = headings within +-0x400. */
    {
        const re15_actor_t *pl2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* byte-true FUN_8001a780 @0x8001a788-94: `lh v0,106(enemy); lh v1,106(player); subu v0,v0,v1` =
         * (enemy.heading - PLAYER.heading), +0x400, &0xfff, slti <0x800. The port had the subtraction
         * reversed (player - enemy), flipping the ±0x400/0xc00 boundary = the front/back fall latch on the
         * ±90° shot. (audit wf_2ff80cc7) */
        if (((((int32_t)e->rot_y - (int32_t)pl2->rot_y) + 0x400) & 0xfff) < 0x800)
            e->hit_react |= 0x80;
    }
    /* crit/headshot (@0x800124fc-0x8001251c): weapon 7, or weapon 8 within 3000 -> instant kill (type<0x20). */
    if ((weapon_id == 7 || (weapon_id == 8 && best_dist < 3000u)) && e->type < 0x20)
        e->hp = -1;
    e->sub_state_3 = 0;                              /* +0x7 = 0 (@0x80012428) — start the hurt/death anim FSM at phase 0 */
    e->state       = (e->hp >= 0) ? 2 : 3;          /* +0x4 = HURT(2) / DEATH(3) (@0x80012520) */
    /* RE2-Flavor: +0x1D2/+0x6 (der Stempel steht am ENDE, damit er den RE1.5-+0x6-Schreiber
     * unten ueberschreibt — Belege am Helfer). */
    /* Port bookkeeping: drop the REVERSE-playback bit. In the original, reverse is NOT entity
     * state — it is the a2=1 argument of single f314 calls (grab-recovery @0x80102aec `ori a2,zero,1;
     * jal 0x8001f314`; feeding kneel-down likewise), and the forced +0x4=2/3 here makes the next
     * handler set a NEW clip forward. The port models reverse as persistent anim_flags 0x80, which
     * without this clear LEAKED on a shot into grab-[6]/[7]: stagger, walk, fall and death all
     * posed backwards (dossier analysis/zombie_hit_1140.md D1, CONFIRMED + probe-reproduced). */
    e->anim_flags &= (uint16_t)~0x80u;
    /* +0x6 = VERTICAL HIT-DIR (@0x80012438-50): DAT_8006f410[player_word>>29] = [7,0,1,7,2,...]
     * indexed by the aim-elevation bits (acaec<<16): UP(bit31,idx4)->2, LEVEL(bit30,idx2)->1,
     * DOWN(bit29,idx1)->0. Feeds the hurt master's hit-dir column. */
    {
        extern int re15_player_aim_elevation(void);  /* player_common.c */
        int elev = re15_player_aim_elevation();      /* -1 down / 0 level / +1 up */
        e->sub_state_2 = (uint8_t)(elev > 0 ? 2 : elev < 0 ? 0 : 1);
        /* Y-corrections of the vertical hit code (FUN_80011f50): @0x80012458-7c `lw playerY
         * (DAT_800aca8c); slt playerY,enemyY; +0x6==0 -> sb 1` and the mirror @0x80012480-ac
         * `slt enemyY,playerY; +0x6==2 -> sb 1`. Strict slt -> no-op on equal ground (ROOM1140).
         * (dossier analysis/zombie_hit_1140.md D4) */
        {
            const re15_actor_t *ply = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            if (ply->y < e->y && e->sub_state_2 == 0) e->sub_state_2 = 1;
            if (e->y < ply->y && e->sub_state_2 == 2) e->sub_state_2 = 1;
        }
    }
    /* knife-only HIT SE (@0x800123c0-d8): weapon==1 && hit landed -> FUN_80045024(0x1080001)
     * = the flesh-hit SE at the player position (the whiff plays only the swing SE 0x1050001). */
    if (weapon_id == 1) {
        extern void re15_audio_weapon_se(int idx);
        re15_audio_weapon_se(8);
    }
    re15_re2_stamp_hit(e, 0, (unsigned)weapon_id);  /* RE2-Flavor: +0x1D2 + +0x6 + ZEILE (s.u.) */
    /* ⛔ PORT-OPTION (Nutzer-Auftrag): im RE1.5-Modus zusaetzlich der RE2-ZERLEGER. No-op im
     * RE2-Modus (dort macht das re2z_hurt @0x80104F40 selbst), no-op ohne die Option, no-op
     * fuer Nicht-Zombies und bei toedlichem Treffer. Steht NACH `e->state = 2/3`, damit die
     * Bruecke das "ueberlebt?" aus e->state lesen kann. Volle Begruendung an
     * re15_re15_re2z_gore_hit (enemy_ai_re2_zombie.c). */
    re15_re15_re2z_gore_hit(e, &g_actors[RE15_ACTOR_SLOT_PLAYER], 0, (unsigned)weapon_id);
    return best + 1;                                /* hit (slot+1, non-zero) */
}

/* WELLE-D-Nachtrag (fix_1d2_spec, alle Zitate selbst nachdisassembliert 2026-08-16): der
 * RE2-+0x1D2-PRODUZENT. +0x1D2 ist PRO TREFFER ein SET auf `zone + 3*bracket` (KEIN Zaehler) —
 * beide RE2-Applier tragen die identische Formel:
 *   - Projektil/AoE FUN_800470C0 (zweifach): Basis 1 @0x80047294-98 / @0x800474B8-BC;
 *     s5&0x20000 + Schuss unterhalb der Halbhoehe (a0 = sign16(+0x98)>>1 @0x800472AC-B8,
 *     `enemy_y+a0 < shot_y` @0x800472BC-CC) -> zone 0 @0x800472D4 / @0x800474F8;
 *     word0&0x10000000 + Schuss oberhalb 3/4-Hoehe (`shot_y < enemy_y+3*a0` @0x800472E8-304)
 *     -> zone 2 @0x8004730C / @0x80047530; dann `+0x1D2 = zone + 3*s6` @0x80047310-30 /
 *     @0x80047564-80 mit s6 = Hitcode>>28 (@0x80047114).
 *   - Hitscan-Applier: dieselbe Formel `3*s1 + zone -> sb 466` @0x80041A8C-9C.
 * ZONE-Port: die Schuss-Y-Regeln haengen an s5&0x20000/word0&0x10000000 ohne Port-Produzent ->
 * deklarierte Naeherung ueber die Aim-Elevation (DOWN->0 Beine, UP->2 Kopf, LEVEL->1; dieselbe
 * Quelle, die oben +0x6 stempelt). BRACKET-Port: 0 (OPEN) — die Hitcode-Produzenten sind
 * waffen-/pfadspezifisch (Bowgun-Bolzen 0xC @FUN_80046304:153, GL-Runden (+0x1b)+0x30009
 * @FUN_8001ed9c:44/47, Sparkshot 0x2002000A @FUN_80020758:28, Hitscan-s1-Producer nicht RE'd);
 * ein RE1.5-Waffen->Bracket-Mapping existiert nicht. Konsumenten: Zombie-Blut %3==0 = ZONE 0
 * (Bein-Treffer, @0x801050B0-E4), Hunde-Gore /3 = BRACKET (@0x80103D00-28/@0x80103DB8-E0),
 * Hunde-Gore-Tod Zeile 9 `<3` = Bracket 0 (@0x801046A8-C4). */
/* ⛔ KORREKTUR 2026-08-18 (Nutzer-Report "die RE2-Zombies reagieren nicht auf die Schuesse"):
 * die bisherige Aim-Elevation-Naeherung war doppelt falsch.
 *
 * (a) ZONE. Beide RE2-Applier setzen +0x1D2 UNBEDINGT auf die Basis 1
 *       80047294: addiu v0,zero,1
 *       80047298: sb   v0,466(s1)          (Zwilling @0x800474B8-BC)
 *     und korrigieren erst DANACH: Zone 0 nur wenn `hitcode & 0x20000` (`lui v0,0x2; and v0,s5,v0;
 *     beq -> 0x80047314` @0x8004729C-A4) UND der Schuss unter der halben Koerperhoehe liegt
 *     (a0 = sign16(+0x98)>>1 @0x800472AC-B8, `enemy_y + a0 < shot_y` @0x800472BC-CC -> `sb zero,466`
 *     @0x800472D4); Zone 2 nur wenn `word0 & 0x10000000` (@0x800472D8-E4) UND `shot_y < enemy_y +
 *     3*a0` (@0x800472E8-304 -> `sb 2,466` @0x8004730C). BEIDE Korrekturen haengen an Bits des
 *     Hitcodes bzw. an einer Schuss-Y-Koordinate, fuer die der Port KEINEN Produzenten hat
 *     (der Port-Schuss ist der RE1.5-Hitscan FUN_80011F50, ohne RE2-Hitcode). Der einzige
 *     BELEGTE Wert ist deshalb die Basis 1. Die alte Elevation-Naeherung erzeugte ausserdem
 *     systematisch Zone 2 = Spalte 2 — und Spalte 2 ist in 11 von 17 Zeilen der Dispatch-Tabelle
 *     @0x8010C964 NULL (selbst gedumpt), also eine im Original unmoegliche Kombination.
 *     BRACKET bleibt 0. ⛔ 2026-08-18 ZU ENDE DISASSEMBLIERT — der Erzeuger ist jetzt bekannt,
 *     und er belegt, dass 0 der RICHTIGE Wert fuer den Port ist (kein Platzhalter):
 *
 *     (i) KONTAKT-/TREFFERBOX-APPLIER FUN_800410CC (JEDE gefuehrte Waffe: der Spieler-Angriff
 *         FUN_80047C6C ruft ihn @0x80047EF4 mit `a2 = 0x800A68E8 + id*24 + 0x800A6F8C[band]*8`
 *         @0x80047EB4-F8). Der Bracket ist der INDEX DER GETROFFENEN ANGRIFFS-TEILBOX:
 *           80041224: addu  s3,a0,v0        ; s3 = Angriffs-Volumen-Record (28 B: 4 Maskenbytes
 *                                           ;      + DREI Teilboxen bei +4 / +12 / +20)
 *           80041460: addiu a2,s3,4  / 80041464: jal 0x80041ce4   ; Teilbox 0
 *           800414a4: addiu s4,v0,1                                ; -> s4 |= 1
 *           80041568: addiu a2,s3,12 / 8004156c: jal 0x80041ce4   ; Teilbox 1
 *           800415ac: addiu s4,v0,2                                ; -> s4 |= 2
 *           80041670: addiu a2,s3,20 / 80041674: jal 0x80041ce4   ; Teilbox 2
 *           800416b4: addiu s4,v0,4                                ; -> s4 |= 4
 *           80041750: sltu  v0,v1,t0 ; der NAECHSTE Treffer gewinnt (sp+96 = beste Distanz)
 *           80041768: sw    s4,72(sp)
 *           8004182c: lw    t0,72(sp)
 *           80041834: andi  v0,t0,0x2 / 80041838: sltu s1,zero,v0    ; Teilbox 1 -> Bracket 1
 *           8004183c: andi  v0,t0,0x4 / 80041848: addiu s1,zero,2    ; Teilbox 2 -> Bracket 2
 *         (Der ZONEN-Anteil s7 kommt aus derselben Schleife: `srl s7,v1,1` @0x8004149C/
 *          @0x800415A4/@0x800416AC ueber die Maskentabelle 0x800A6DB4, gegated mit der
 *          Haltungsklasse des Opfers `s6 = (word0>>26)&7` @0x800413CC-D4; `+0x1D2 = 3*s1 + s7`
 *          @0x80041A6C/@0x80041A88-9C.)
 *         Die drei Teilboxen sind KONZENTRISCH: die per-Waffe-Records liegen in Dreiergruppen
 *         (id 2 z.B. 0x800A6618/0x800A6634/0x800A6650, je 28 B, selbst gedumpt), und die
 *         Schadens-/Poise-Daten steigen nach innen: Zeile 7 (Schrot) 200/60/40 Schaden,
 *         Zeile 2 (Pistole) 16/15/14, Poise-Kosten 4/2/0 bzw. 2/1/0. **Zeile 1 (Messer) hat
 *         `w0 = 0x00000003` = 3/0/0 — nur Bracket 0 macht ueberhaupt Schaden.**
 *         -> Bracket 0 = INNERSTE Teilbox = der direkte Treffer. Genau das ist der RE1.5-
 *            Hitscan FUN_80011F50 des Ports: EIN Treffertest auf Zielhoehe, ohne Abstufung.
 *     WAS DEM PORT FEHLT, um 1 oder 2 zu erzeugen (namentlich): der Overlap-Test FUN_80041CE4
 *         @0x80041CE4, die Angriffs-Volumen-Tabelle 0x800A68E8 (19 Waffen x 3 Records x 3
 *         Teilboxen) und die Ziel-Haltungsklasse (word0>>26)&7. Alle drei sind RE2-Datensaetze
 *         ohne RE1.5-Gegenstueck — es gibt keine Groesse, aus der der Port sie ableiten koennte.
 *
 *     (ii) PROJEKTIL-/AoE-APPLIER FUN_800470C0: `srl s6,s5,28` @0x80047114, s5 = a3 = der
 *         Hitcode; `+0x5 = a3 & 0xFF` = die ZEILE (`sb s5,5(s1)` @0x80047324) und
 *         `+0x1D2 = 3*s6 + zone` (`sll v1,s6,1` @0x80047310 / `addu v1,v1,s6` @0x80047320 /
 *         `sb v0,466` @0x80047330). ZENSUS aller 17 `jal FUN_800470C0` in info/re2leon/PSX.EXE
 *         (selbst gescannt), Hitcode -> (Zeile, Bracket):
 *           0x0003000E @0x8001E97C-80  -> Zeile 14, 0   0x00030011 @0x8001F7B0/C0 -> 17, 0
 *           0x00020011 @0x8001F820-2C -> 17, 0          0x0002000A @0x80021058-70/@0x800214F0 -> 10, 0
 *           0x00020010 @0x800233E8-F4 -> 16, 0          0x00060001 @0x80042F84-88 -> 1, 0
 *           0x0000000C @0x800467C4    -> 12, 0          (+0x1b)+0x30009 @0x8001EED0-DC -> 9..11, 0
 *           0x1002_0009 @0x80020D54-58/@0x80020D84-88 -> 9, **1**
 *           0x1002000B @0x800216E4/F0/@0x800216FC     -> 11, **1**
 *           0x2002000A @0x80020794-A0                  -> 10, **2**
 *         D.h. JEDER DIREKTE Projektil-Einschlag hat Bracket 0; 1 und 2 tragen ausschliesslich
 *         die NACHBRENNER-Entities der Granatwerfer-Runden (Explosiv-Druckwelle, Saeure-Pfuetze,
 *         Brand-Flaeche). Die Port-Zeilen 9/10/11/12/16/17 sind genau die Waffen mit dem
 *         NULL-Geometrie-Record 0x800A6350 (@0x800A68E8, selbst gedumpt) — sie treffen NUR ueber
 *         diesen Applier, und dort ist ihr Bracket byte-true 0. Der Port hat keine
 *         Nachbrenner-Entities (OPEN: FUN_8001ED9C/FUN_80020758 haben keinen Port-Zwilling).
 *     -> +0x1D2 = 1 + 3*0 = 1 ist damit fuer JEDEN Pfad, den der Port ueberhaupt abbildet, der
 *        belegte Wert. Die Spalten 3..8 der Reaktionstabelle @0x8010C940 bleiben unerreichbar,
 *        weil ihr Eingang (Teilbox 1/2 bzw. AoE-Nachbrenner) im Port nicht existiert.
 *
 * (b) +0x6. Der RE2-Applier schreibt +0x4 als WORT:
 *       8004727C: lh   v1,342(s1)          ; HP
 *       80047284: bgez v1,0x80047294
 *       80047288: sw   v0,4(s1)            ; v0 = 2  (Delay-Slot, laeuft IMMER)
 *       8004728C: addiu v0,zero,3
 *       80047290: sw   v0,4(s1)            ; v0 = 3 bei HP < 0
 *     Das `sw` nullt +0x5/+0x6/+0x7 mit; nur +0x5 wird danach wieder gesetzt
 *     (`sb s5,5(s1)` @0x80047324 / `sb v1,5(t0)` @0x80041AB4). **+0x6 ist nach JEDEM RE2-Treffer
 *     0.** Der Port stempelte hier die RE1.5-Aim-Elevation (0/1/2) hinein; +0x6 ist im
 *     RE2-HURT aber die REAKTIONS-PHASE (der Master @0x80105478-B8 verzweigt darauf, und die
 *     Onset-Phase setzt +0x6=1 @0x801055B4). Mit +0x6 != 0 war sowohl das Flinch-Gate
 *     (`bne v0,zero,0x80105168` @0x80105064) als auch die Onset-Phase strukturell blockiert —
 *     genau der gemeldete "keine Reaktion". */
/* ⛔ NACHTRAG 2026-08-18 (Nutzer-Vorgabe "einzelne staerkere Waffen muessen auch in RE1.5
 * reagieren"): der Stempel setzt jetzt AUCH die ZEILE +0x5.
 *
 * Im Original ist die Zeile die ITEM-ID der gefuehrten Waffe — die Kette ist lueckenlos belegt:
 *   8006b034: lbu v0,-30636(at)   ; Item-Id des Inventar-Slots (0x800D4A3C + slot*4)
 *   8006b040: sltiu v0,v0,0x14    ; Item-Id < 20 == Waffe
 *   8006b09c: sb   v0,23546(at)   ; 0x800D5BFA = Item-Id der ausgeruesteten Waffe
 *   8003bd4c: sh   v0,270(s2)     ; +0x10E = Item-Id (bzw. 0 @0x8003BD50, wenn nichts gefuehrt)
 *   80047eb4: lhu  v0,270(s0) / andi 0xfff / addiu a1,v0,-1 / sll a1,16   -> FUN_800410CC
 *   80041aa0: lw t0,64(sp) / srl v1,t0,16 / addiu v1,v1,1 / sb v1,5(t0)   ; +0x5 = Item-Id
 * Der Port schiesst mit dem RE1.5-Hitscan FUN_80011F50 (`+0x5 = weapon_id` @0x800124BC) — ein
 * anderer Id-Raum mit 22 statt 19 Eintraegen. Ungeuebersetzt fielen 7 von 22 RE1.5-Waffen auf
 * eine NULL-Zelle bzw. aus der Tabelle (gemessen: w 0,5,6,17,19,20,21 = gar keine Reaktion).
 * re15_re2z_row_for_weapon/-atktype (enemy_ai_re2_zombie.c) uebersetzt klassenweise; dort steht
 * die Zuordnung mit Begruendung je Waffe.
 *
 * NUR die Zombie-Familie bekommt die Zeile: sie ist der einzige Konsument der 2D-Tabelle
 * @0x8010C940. Hund (0x20) und Kraehe (0x21) benutzen +0x5 als ACTIVE-Substate und behalten
 * exakt das bisherige Verhalten.
 *
 * Reihenfolge: der Stempel laeuft am ENDE beider Applier, also NACH `+0x5 = weapon_id` und nach
 * dem `+0x4 = 2/3` — `survived` kann damit aus e->state gelesen werden. */
static void re15_re2_stamp_hit(re15_actor_t *e, int row_src, unsigned row_id)
{
    /* MIXED (2026-08-23): typ-bezogen. Im MIXED-Modus passiert GENAU der Hund dieses Tor und
     * bekommt damit denselben Stempel wie im RE2-Modus (+0x1D2 Basis-Zone, +0x6 = 0). Die
     * ZEILE +0x5 unten bleibt der Zombie-Familie vorbehalten — der Hund benutzt +0x5 als
     * ACTIVE-Substate (Block oben), das ist in MIXED wie in RE2 gleich. */
    if (!re15_ai_re2_for_type(e->type)) return;
    /* ⛔ 0x26 nach HERKUNFT (2026-08-22): der Stempel nullt +0x6 (sub_state_2). Bei den sieben
     * RDT-gesetzten 0x26ern von ROOM1090 — den FEUER-EMITTERN, 0x80072bac[0x26] = 0x80116288 —
     * ist +0x6 der ACTIVE-Substate des Emitters (@0x80116784/@0x8011689C), ein Treffer haette
     * den Flammen-Zyklus also neu gestartet. Nur echte RE2-Babys (re2s_baby_spawned, Spawner
     * `addiu a0,zero,38` @0x80105DE8) gehoeren in den RE2-Stempel. */
    if (e->type == 0x26u ? !re15_re2spider_baby_owns(e)
                         : !re15_re2_owns_type(e->type)) return;
    /* NEGATIV-PROBE (2026-08-18, Ergebnis im Report): ersetzt man diese zwei Zeilen wieder
     * durch den alten Elevation-Stempel `hits1d2 = elev<0?0:elev>0?2:1` OHNE das +0x6-Nullen,
     * wird probe_re2_livepath sofort rot — kein Grunzer (SE=-1), +0x223 unveraendert 20, kein
     * Knockdown. Das ist exakt der gemeldete Fehler. */
    e->re2z_hits1d2 = 1u;   /* Basis-Zone 1 @0x80047294-98 + 3*Bracket 0 (OPEN, s.o.) */
    e->sub_state_2  = 0u;   /* +0x6 = 0 durch das Wort-`sw` @0x80047288/@0x80047290 */
    if (re15_re2z_owns_type(e->type)) {
        int survived = (e->state != 3);   /* +0x4 == 3 -> DEATH-Wurzel statt HURT (@0x8004728C-90) */
        e->sub_state_1 = row_src ? re15_re2z_row_for_atktype(row_id, e->re2z_hits1d2, survived)
                                 : re15_re2z_row_for_weapon (row_id, e->re2z_hits1d2, survived);
    }
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
    re15_re2_stamp_hit(e, 1, (unsigned)type);                     /* RE2-Flavor: +0x1D2 + ZEILE (s.o.) */
    /* ⛔ PORT-OPTION (Nutzer-Auftrag): der RE2-Zerleger auch aus dem Trefferbox-Pfad. Der
     * Treffer-URSPRUNG dieses Appliers ist der Spieler-Angriff (FUN_80012d60-Gegner-Zweig wird
     * ausschliesslich aus re15_resolve_attack fuer Spieler-Angriffe erreicht), also ist der
     * Spieler-Aktor die Peilungsquelle des +0x1D0-Stempels. */
    re15_re15_re2z_gore_hit(e, &g_actors[RE15_ACTOR_SLOT_PLAYER], 1, (unsigned)type);
    e->anim_flags &= (uint16_t)~0x80u;   /* same port-bookkeeping REVERSE drop as the gun path —
                                          * reverse is a per-call f314 argument (@0x80102aec), not
                                          * entity state; the hijacked state sets a new forward clip
                                          * (dossier analysis/zombie_hit_1140.md D1) */
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
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/, 0x2000 /* Original-a0 (D6, bite_blood_fx.md F6) */,
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
/* GORE-BONE world position (byte-true FUN_80106edc @0x80106ff8-0x80107030): the gore effects spawn at
 * the model_inst bone matrix (entity+0x188 + LAB_8011f784[type]*0xac, +0x40 = translation), NOT the
 * actor root. LAB_8011f784 (STAGE1.BIN @0x8011f784, byte-indexed by type): zombies 0x10-0x12/0x15-0x1a
 * = bone 14, zombie-girl/writhers 0x13/0x14/0x1b-0x1e = bone 8. Reproduce it: pose the enemy's skeleton
 * at its current keyframe in QUERY mode (g_anim_pose_actor=NULL, crossfade preserved — same as
 * re15_enemy_update_attack_point) and rotate the bone's model-local translation into world via
 * re15_skel_bone_to_world (the render-loop RotY math). Falls back to the actor root if no bank/skel. */
static const uint8_t s_gore_bone[0x30] = {
    0,0,76,255,0,0,144,1,180,0,144,1,120,247,17,128,  /* 0x00-0x0f: non-enemy types (unused) */
    14,14,14, 8, 8,14,14,14,14,14,14, 8, 8, 8, 8, 0,  /* 0x10-0x1f: zombies=14, zgirl/writhers=8 */
    0, 7, 8,14, 2, 4, 9,13, 0, 7, 8,14, 1, 5,10,11,   /* 0x20-0x2f */
};
/* World position of ONE part of the enemy's posed skeleton (the original passes `part + 0x40`, the
 * composed part MATRIX, whose translation is at +0x14 of it — see re15_enemy_hurt_blood). */
void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3])
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z;      /* fallback = actor root */
    if (!e) return;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || b->skel.bone_count <= 0 || bone < 0 || bone >= b->skel.bone_count) return;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;                          /* QUERY: don't mutate the crossfade blend */
    int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    re15_skel_bone_to_world(poses[bone].trans, e->rot_y, e->x, e->y, e->z, out);
}

/* The gore/death anchor keeps its own per-type bone table (LAB_8011f784[type]). */
static void re15_enemy_gore_bone_pos(const re15_actor_t *e, int32_t out[3])
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    if (e->type >= 0x30) return;
    re15_enemy_bone_world_pos(e, (int)s_gore_bone[e->type], out);
}

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
    int32_t g[3];
    re15_enemy_gore_bone_pos(e, g);          /* byte-true model_inst bone (LAB_8011f784[type]), not the root */
    re15_esp_fx_spawn_ex(bank, 0, 0, 0x2000, g[0], g[1], g[2], (int16_t)e->rot_y);  /* a0=0x2000 */
    re15_esp_fx_spawn_ex(bank, 0, 0, 0x2000, g[0], g[1], g[2], (int16_t)e->rot_y);  /* a0=0x2000 */
    re15_esp_fx_spawn_ex(bank, 5, 0, 0x2800, g[0], g[1], g[2], (int16_t)e->rot_y);  /* a0=0x5002800 */
}

/* Zombie HURT hit-effect — byte-true: the master-table hurt dispatch FUN_80105a8c (entity+4==2)
 * calls master[entity+5=react-zone][entity+6=hit-dir] = the hit handler FUN_80105b7c (rows 1/3/4),
 * whose phase-0 (entity+7==0) spawns the hit/blood effect: func_0x80019700(0x2000, rot_y, bone+0x40,
 * &LAB_8011fe84) = effect-id 0 (the UNIVERSAL hit fx, from the global CORE00.ESP bank — NOW resolvable
 * via re15_esp_set_global_bank). Position is the model_inst bone block in the original -> the actor
 * world position (faithful-line). re15_enemy_take_damage already sets state=2 + sub_state_3=0, so this
 * fires once on the hurt entry (shoot a zombie -> visible blood). +0x93|=1, entity+7=1 to advance. */
void re15_enemy_hurt_fx(re15_actor_t *e)
{
    if (!e || !e->active) return;
    if (e->state != 2) return;               /* HURT state (entity+4==2) */
    if (e->sub_state_3 != 0) return;         /* FUN_80105b7c phase 0 (entity+7==0): spawn once */
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/, 0x2000 /* Original-a0 (D6, bite_blood_fx.md F6) */,
                      e->x, e->y, e->z, (int16_t)e->rot_y);   /* a0=0x2000 -> effect-id 0, a1=rot_y */
    e->hit_react   |= 0x1;                    /* entity+0x93 |= 1 */
    e->sub_state_3  = 1;                      /* entity+7 = 1 (phase advance) */
}

/* Zombie DEATH gore — byte-true: FUN_80107cb0 (the death sub-FSM under @0x8011f7b4[3]) spawns the
 * effect-id 0 blood burst TWICE during the death animation: at death START (phase 0 @L23,
 * func_0x80019700(0x2500, rot_y, entity+0x188+0x40, ...)) and again at anim_frame 35 (+0x95==0x23
 * @L44, func_0x80019700(0x2000, rot_y, bone*0xac + entity+0x188+0x40, ...)). Both are effect-0 (from
 * the global CORE00.ESP bank) = the same universal hit/blood fx as the hurt spawn. Called from
 * re15_enemy_ai_live_death at those two points. Position = the actor world pos (the frame-35 bone
 * offset DAT_8011f784[entity+8]*0xac is the deferred refinement, faithful-line). Frame 7 death SE
 * (func_0x800453d0 = re15_audio_room_se) IS implemented in re15_enemy_ai_live_death
 * (enemy_ai_common.c :2487-2488, death-groan SE rng 5/8). */
/* HURT hit BLOOD — byte-true anchor (RE'd 2026-07-28). The stagger handler's phase 0 spawns it with
 *   80105c54/cbc: ori a0,zero,0x2000          effect id
 *   80105c74/ccc: lh a1,106(...)              rot_y (+0x6a)
 *   80105ca0:     addiu a2,s0,64              s0 = model_pool + 172*u8[0x8011f784 + type]  (ZONE 1)
 *   80105cd8:     addiu a2,a2,408             = model_pool + 172*2 + 64                    (ZONE 0)
 *   80105c60/cc4: a3 = 0x8011fe84
 * and FUN_80019700 treats a2 as a MATRIX POINTER: it stores it (`sw a2,116(t0)` @0x8001980c/181c)
 * and copies 32 bytes out of it (`lw 0/4/8/12(a2)` @0x80019820-2c, `lw 16/20/24/28(a2)`
 * @0x80019840-4c) — a PSX MATRIX (9 x int16 rot + pad + 3 x int32 trans at +0x14). part+0x40 is
 * exactly that composed part matrix (its translation sits at part+0x54 = 0x40+0x14). So the blood is
 * anchored at the BONE's world transform, NOT at the actor root:
 *   zone 1 (the normal shot, +0x6 == 1) -> the per-type gore bone (zombies 0x10/0x11/0x16 = 14)
 *   zone 0                              -> part 2
 *   zone 2 (upward hit)                 -> no spawn at all (@0x80105cb8 bne v0,zero)
 * The port spawned at the actor origin, which is why the user saw the blood in the wrong place. */
void re15_enemy_hurt_blood(re15_actor_t *e)
{
    if (!e || !e->active) return;
    if (e->sub_state_2 > 1) return;                    /* zone 2: the original spawns nothing */
    int bone = (e->sub_state_2 == 1)
                 ? ((e->type < 0x30) ? (int)s_gore_bone[e->type] : 0)   /* @0x8011f784[type] */
                 : 2;                                                   /* @0x80105cd8: part 2 */
    int32_t g[3];
    re15_enemy_bone_world_pos(e, bone, g);
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/, 0x2000 /* Original-a0 (D6, bite_blood_fx.md F6) */,
                      g[0], g[1], g[2], (int16_t)e->rot_y);
    re15_esp_fx_splatter(re15_esp_room_bank(), 0 /*blood*/, 8,
                         g[0], g[1], g[2], e->y);      /* floor = the actor's ground Y */
}

/* One blood burst anchored at ONE part's world transform — the shape every FUN_80019700 call in the
 * zombie overlay has (a0 = 0x2000, a1 = +0x6a, a2 = model_pool + 172*part + 64). */
void re15_enemy_blood_at_bone(re15_actor_t *e, int bone)
{
    if (!e || !e->active) return;
    int32_t g[3];
    re15_enemy_bone_world_pos(e, bone, g);
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/, 0x2000 /* Original-a0 (D6, bite_blood_fx.md F6) */,
                      g[0], g[1], g[2], (int16_t)e->rot_y);
}

/* DEATH blood — byte-true STANDING death FUN_80106c18 (self-disassembled 2026-07-28). Its phase-0
 * fires up to THREE bursts, all bone-anchored, none at the actor root:
 *   80106cb8: ori a0,zero,0x2000 ; 80106ce4-e8: a3 = 0x8012016c ; 80106cec-d00: bone = u8[0x8011f784+type]
 *   80106d08-28: v0 = 172*bone + pool(+0x188)      (the x172 chain: 3,12,11,44,43,172)
 *   80106d2c/30: jal 0x80019700 ; addiu a2,v0,64   -> ALWAYS: the per-type GORE BONE matrix
 *   80106d40-48: lbu +0x6 ; bne v0,s1  (s1 = 1 @0x80106c34)
 *   80106d5c/60: jal ; addiu a2,v0,64              -> zone 1: PART 0 matrix (pool + 64)
 *   80106d78:    bne v0,zero
 *   80106d8c/90: jal ; addiu a2,a2,408             -> zone 0: PART 2 matrix (pool + 408)
 * (The same shape repeats in the other death variant @0x80107000-30.) The port spawned ONE burst at
 * the actor origin = at the corpse's FEET; a gunshot death now bleeds at the gore bone AND part 0. */
void re15_enemy_death_fx(re15_actor_t *e)
{
    if (!e || !e->active) return;
    int32_t g[3];
    int gore = (e->type < 0x30) ? (int)s_gore_bone[e->type] : 0;   /* @0x8011f784[type] */
    re15_enemy_bone_world_pos(e, gore, g);
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0 /*effect-id*/, 0 /*sub*/, 0x2000 /* Original-a0 (D6, bite_blood_fx.md F6) */,
                      g[0], g[1], g[2], (int16_t)e->rot_y);        /* @0x80106d2c ALWAYS */
    /* The two ZONE-GATED extra bursts (@0x80106d5c part 0 / @0x80106d8c part 2) belong to the STANDING
     * death handler FUN_80106c18 specifically, not to every caller of this helper — they are emitted
     * in re15_enemy_ai_live_death's standing branch. */
    /* BLOOD SPLATTER (byte-true parent→child chain, RE15_ESP_ROWMACHINE.md): the death burst
     * throws a spray of physics droplets (gravity + RNG spread + floor bounce) — the byte-true
     * blood spread that the single cycling sprite alone lacked. Floor = the actor's ground Y. */
    re15_esp_fx_splatter(re15_esp_room_bank(), 0 /*blood*/, 8,
                         g[0], g[1], g[2], e->y);   /* from the gore bone, floor = ground Y */
}

/* ====================================================================== *
 *  Attack-hitbox vs actor collision test — FUN_8002b5d0                   *
 *  (ghidra1_V2.txt:118005-118130). The per-target geometry test the       *
 *  resolver loop FUN_80012d60 runs against each enemy + the player.       *
 * ====================================================================== */

/* BIOS SquareRoot0 — the original calls it @0x8002b764 (hitbox FUN_8002b5d0),
 * @0x80012848 (weapon cone FUN_800127fc) and @0x80117300 (enemy dist). It is the
 * table APPROXIMATION, NOT floor(sqrt) — an exact isqrt diverges on ~82% of inputs
 * and shifts the AI distance thresholds. Route through the shared byte-true replica
 * on the low 32 bits (like the PSX mflo). Audit wf_f066b2ae. */
static int32_t dmg_isqrt(int64_t x)
{
    if (x <= 0) return 0;
    return (int32_t)re15_squareroot0((uint32_t)x);
}

/* CIRCULAR path of FUN_8002b5d0 (radius_min == radius_max @ hbdata+6/+0xa). The
 * angular-SECTOR path (radius interpolated over the target's facing via the BIOS
 * ratan2 + rsin/rcos, @8002b65c) is now IMPLEMENTED in the wrapper re15_hitbox_test
 * (:697-700): it computes the effective radius via re15_ellipse_radius (re15_math.c
 * :276) + the BIOS re15_ratan2 (re15_math.c :233), so this overlap primitive stays
 * circular by design (it takes a precomputed radius). Symmetric hitboxes
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
 * + local offset (+0x7c) and run the overlap test. The angular-SECTOR case
 * (hit_radius_min != hit_radius_max, e.g. the alligator's 2200/800 box) blends the
 * directional reach by the attack's bearing vs the target heading via the BIOS
 * ratan2 + rsin/rcos (@0x8002b65c) — byte-identical to the body-push ellipse, so it
 * routes through the shared re15_ellipse_radius. Circular boxes take radius_min directly
 * (the beq @0x8002b61c). */
int re15_hitbox_test(const re15_actor_t *target, const re15_attack_box_t *atk)
{
    if (!target || !atk) return 0;
    int32_t cx = target->x + target->hit_offset_x;          /* +0x34 + offset[0] */
    int32_t cy = target->y + target->hit_offset_y;          /* +0x38 + offset[1] */
    int32_t cz = target->z + target->hit_offset_z;          /* +0x3c + offset[2] */
    /* eff radius: circular -> radius_min; sector -> ellipse toward the attack point (@0x8002b65c). */
    int32_t radius = re15_ellipse_radius((int32_t)target->hit_radius_min,
                                         (int32_t)target->hit_radius_max,
                                         (int32_t)target->rot_y,
                                         atk->z - cz, atk->x - cx);
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
 *   - the AI DECISION that enters this action IS now RE'd: the live handler
 *     FUN_80101224 @0x8011f7b4[1] + the decide vtable FUN_80101b64/de4/2058
 *     (enemy_ai_common.c :2097-2281 + re15_ai_dispatch_decision). The lunge-ARM
 *     FUN_8010ab2c is DORMANT here (DAT_800aca3c&1 never set), so the in-game
 *     attack is the GRAB, not this lunge. */
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
    /* Y-Offset jetzt BYTE-VERIFIZIERT (2026-08-02): die On-Disc-Struktur @0x80073e94
     * (PSX.EXE Datei-Offset 0x64694) lautet vollstaendig
     *   00 00 | 06 fa | 00 00 | c2 01 | fa 05 | c2 01
     *   ofs_x=0, ofs_y=-1530, ofs_z=0, r_min=450, h=1530, r_max=450
     * — das +0x7c-Offset ist (0,-1530,0), NICHT 0. Der alte 0-Wert ("unconfirmed")
     * verschob das Y-Band des Body-Push um 1530: am Boden folgenlos (Zombie-dy +90
     * statt -1440, Band +-2970), aber der FLIEGENDE Kraehen-Grapple (Hover ~2000
     * ueber dem Spieler) fiel damit aus dem Band -> Kraehen konnten NIE zupacken
     * (Nutzer-Report 2026-08-02, Sonde probe_crow_1170). */
    p->hit_offset_x = p->hit_offset_z = 0;
    p->hit_offset_y = -1530;
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
        case 0x10: case 0x11: case 0x12:
        case 0x16: case 0x18: r = 400;  h = 1440; break;  /* STAGE1-5 zombie variants (share the root
                                                * FUN_80100424): 0x10/0x11/0x16 briefing + 0x12/0x18   */
        case 0x13: r = 400;  h = 1440; break;  /* ZOMBIE GIRL (EM013) — zombie-family (shares the
                                                * zombie combat/corpse; faithful-line dims)     */
        case 0x29: r = 1100; h = 1080; break;  /* COCKROACH (EM029, STAGE3) — live-observed box
                                                * (HASH-..._5.sav); maggot-like scurrier            */
        case 0x30:                             /* G-BIRKIN boss form 1 (EM030, STAGE3) — byte-true from */
        case 0x36: r = 1000; h = 1440; break;  /* INIT 0x801166e0: +0x78 = box @0x8011ee64 =
                                                * {0,-1440,0,1000,1440,1000} (circular, radius_min==max). Type
                                                * 0x36 (form-5 EM036) shares the boss root (no +0x8 branch) so
                                                * it uses the same box.  */
        case 0x20: r = 500;  h = 600;  break;  /* DOG (Cerberus) — low+wide ground enemy
                                                * (faithful-line: exact +0x78 dims deferred to Wave 2) */
        case 0x23: r = 2200; h = 720;  break;  /* ALLIGATOR (EM023, STAGE2) — byte-true box @0x80118b98 =
                                                * {1000,-720,0,2200,720,800}: x_max 2200 (wide), y_max 720
                                                * (low, giant) — INIT +0x78 @0x8010c708 */
        case 0x2b: r = 800;  h = 1710; break;  /* TYRANT (EM02B, STAGE4/5) — byte-true box @0x8011a094 =
                                                * {0,-1710,0,800,1710,800,0,-900}: x_max 800, y_max 1710
                                                * (tall biped) — INIT +0x78 @0x80111c38 */
        /* IVY plant-grappler (EM02D, STAGE4) gets NO box: the {450,1530} data @0x8011a2c8 is consumed
         * by the neighbouring type-0x40 NPC INIT 0x80116d20 (lw [0x8011a2d4]->+0x78 @0x80116d60-68), NOT
         * the ivy — the ivy INIT 0x80116920 never writes +0x78. The ivy is weapon-immune (dmg row
         * @0x8006f048 all-zero + INIT +0x93=3 hit-once guard) and is NOT a weapon/auto-aim target nor a
         * body-push obstacle; it falls to `default: return` = no box (audit wf_efd92a2c ivy #77). */
        case 0x21: r = 200;  h = 180;  break;  /* CROW — byte-true from its +0x78 dim block
                                                * @0x801210fc: hw[+6]=0xc8=200 (radius),
                                                * hw[+8]=0xb4=180 (height), hw[+10]=200 (INIT
                                                * FUN_80111a4c: +0x78 = DAT_80121108)          */
        case 0x26: r = 600;  h = 720;  break;  /* SPIDER-BABY — byte-true from its +0x78 collision box
                                                * @0x80121258 = {0,0,0,600,720,600} (INIT 0x801164b0:
                                                * +0x78 = DAT_80121264)                         */
        case 0x27: r = 1600; h = 1440; break;  /* MAGGOTS_BABY — byte-true from its +0x78 collision box
                                                * @0x80121350 = {0,-1440,0,1600,1440,1600} (INIT
                                                * 0x80116f50: +0x1e2=4 -> table[4] @0x80121368)  */
        case 0x1a: r = 300;  h = 1440; break;  /* WRITHE-HAZARD (EM01A, STAGE1 ROOM1210/1211) — byte-true
                                                * from INIT +0x78 = *(0x80120934) = box @0x8012091c =
                                                * {0,-1440,0,300,1440,300} (radius 300) @0x8010c3bc-3c4.
                                                * A real damage box: the writher is TARGETABLE (spawn HP=0,
                                                * type<0x20 -> one damaging hit kills) and a solid 300-radius
                                                * body-push obstacle (audit wf_efd92a2c writher #1/#5). */
        case 0x25: r = 1000; h = 900;  break;  /* ADULT SPIDER (EM025, STAGE2) — byte-true from its +0x78
                                                * box A @0x80118dd4 = off(0,-900,0) half(1000,900,1000)
                                                * (INIT 0x80110b6c: +0x78 = *(0x80118e04) = box A;
                                                * state-swapped to box C @0x80118dec (200,1440,200)) */
        default:   return;                      /* unverified type → no hitbox (no guessing) */
    }
    /* box[+6] = along-heading radius (hit_radius_min), box[+0xa] = lateral (hit_radius_max).
     * Every shipped box is circular EXCEPT the alligator (0x23): box @0x80118b98 = {..,2200,720,800},
     * so box[+6]=2200 != box[+0xa]=800 — the body-push anisotropic-ellipse branch (FUN_8002aec4
     * @0x8002af68) is REACHABLE there (audit wf_8b1360d4). */
    uint16_t rz = r;
    if (type == 0x23) rz = 800;
    a->hit_radius_min = r;
    a->hit_radius_max = rz;
    a->hit_height     = h;
    a->hit_offset_x   = 0;
    a->hit_offset_y   = (int16_t)(-(int32_t)h);   /* +0x7c local offset = (0, -height, 0) —
                                                   * BYTE-VERIFIZIERT fuer die Zombie-Familie:
                                                   * Box @STAGE1.BIN file 0x1f778 =
                                                   * {0,-1440,0,400,1440,400} (2026-08-02) */
    a->hit_offset_z   = 0;
    if (type == 0x21)                             /* CROW-Ausnahme: Box @0x801210fc (STAGE1.BIN
                                                   * file 0x210fc, byte-verifiziert 2026-08-02) =
                                                   * {0,0,0,200,180,200} — ofs_y ist 0, NICHT -h */
        a->hit_offset_y = 0;
}

/* ====================================================================== *
 *  STAGE1 zombie AI (type 0x47, AI handler FUN_8011d6d4) — byte-true.      *
 *  The AI is a 4-level nested FSM (main state +0x4 → PTR_FUN_801217a0;     *
 *  sub-state → PTR_FUN_801217b4; etc.). The DECISION transitions below are *
 *  confirmed (the SAME condition appears verbatim in FUN_80101b64 /        *
 *  FUN_80101de4 / FUN_80102058): dist<2000 && arc!=0 → state word 0x701    *
 *  (the attack-commit state), and dist<4000 && arc==0 → 0x201 (approach,   *
 *  FUN_80101c7c). Now IMPLEMENTED (live @0x8011f7b4): the 0x701→FUN_80017fa4 lunge chain    *
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
 * sequence + attack-point) is IMPLEMENTED (re15_enemy_lunge_begin/tick + the live
 * @0x8011f7b4 family in enemy_ai_common.c); only the arc's +1024 convention is still
 * open — the threshold/structure here are byte-true (3-handler-confirmed). */
int re15_enemy_should_attack(const re15_actor_t *e, const re15_actor_t *player)
{
    if (!e || !player) return 0;
    if ((uint32_t)re15_enemy_player_dist(e, player) >= 2000) return 0;   /* dist < 0x7d0 */
    return re15_ai_arc_test(e, player->x, player->z, 0x2c8) != 0;
}
