/*
 * RE1.5 Rebuilt — Player controller (Phase 4.5.9-D, 2026-05-19).
 *
 * RE2-pure: the player IS actor slot 0 (g_actors[0]). This tick reads
 * pad bits and updates the player's actor entry directly. No separate
 * `g_game.player_*` fields anymore.
 *
 * Controls (tank, RE2-faithful):
 *   UP    : forward along player rot_y
 *   DOWN  : backward
 *   LEFT  : rotate -TURN_RATE
 *   RIGHT : rotate +TURN_RATE
 *
 * Yaw is the canonical heading (g_actors[0].rot_y, 4096-units). The
 * renderer derives face sin/cos via re15_sin_q12 / re15_cos_q12 each
 * frame; no caching here.
 */

#include <stdint.h>
#include "re15_player.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"   /* WELLE B: RE2-Flavor-Gates im Anim-Advancer (Clip-Pin-Konflikte) */
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_scd.h"        /* g_scd.player_mode (BL-round input gate) */
#include "re15_enemy_ai.h"
#include "re15_esp.h"       /* re15_esp_fx_spawn — the weapon-7 speedloader drop */   /* RE15_AI_STATE_CORPSE — corpse-hold in re15_actors_anim_advance */

/* Locomotion speeds = the RE1.5 per-direction speed bytes (FUN_80041BE4 mode
 * tables 0x80076cXX): WALK=0x4B(75), RUN=0xC8(200), BACK=0x3C(60). Translation is
 * speed*heading (the W01 clips are in-place gait cycles). */
#define WALK_SPEED_PER_FRAME 75
#define RUN_SPEED_PER_FRAME  200
#define BACK_SPEED_PER_FRAME 70   /* @0x80073ec4 = [70,48] (back speed 70); cinematic 0x8003109c + PLC
                                   * mode 7/8 also load 0x46=70. Port had 60 -> retreat ~14% too slow. */
/* Tank-control free-roam TURN rate (4096 units = 360°) — BYTE-TRUE from the PSX player pad handler.
 * Holding LEFT/RIGHT adds/subtracts a PER-MOVE-SUB-STATE, per-camera turn byte to the heading
 * accumulator DAT_800acabe once per 30 Hz frame; that accumulator IS the facing heading (fed straight
 * into the GTE RotMatrix), so its per-frame delta is the visible turn rate.
 *
 * CORRECTION 2026-07-03 (DuckStation parity, DETERMINISTIC direct-load measurement — re15-parity-verify)
 * + 2026-07-08 (byte-true audit, tables re-read from PSX.EXE): there are THREE distinct FORWARD/pivot
 * turn rates, one per move sub-state, each a [speed,turn] pair indexed by camera*2:
 *   - WALK-forward  @0x80073ea4 = [75, 48]  -> turn 0x30 = 48/frame
 *   - RUN-forward   @0x80073f24 = [200, 72] -> turn 0x48 = 72/frame   (byte @0x80073f25 = 0x48)
 *   - TURN-IN-PLACE @0x80073ee4 = [0, 96]   -> turn 0x60 = 96/frame   (MEASURED: hold R 1s -> +2784)
 *   - BACK          @0x80073ec4 = [70, 48]  -> speed 70, turn 48
 * The port previously collapsed RUN into IN-PLACE (both 0x60=96); byte-verified that run-forward
 * turn is 72, not 96, so running+steering was ~33% too sharp. */
#define TURN_RATE_WALK_PER_FRAME     0x30   /* 48 — turn while walking forward (@0x80073ea4 = [75,48]) */
#define TURN_RATE_RUN_PER_FRAME      0x48   /* 72 — turn while RUNNING forward (@0x80073f24 = [200,72]);
                                             * distinct from in-place 0x60 (the port had wrongly shared it) */
#define TURN_RATE_IN_PLACE_PER_FRAME 0x60   /* 96 — pivot in place (@0x80073ee4 = [0,96]), MEASURED byte-true */

/* Animation-clip sentinels consumed by the renderer:
 *   105 -> PL00W01 clip 5 = Walk_Forward (FSM bank 0x174, motion 0x30)
 *   100 -> PL00W01 clip 0 = Run          (bank 0x174, motion 0x48)
 * TURN reuses 105 (the FSM plays the walk-step clip while yawing in place).
 *
 * IDLE = PL00.edd clip 22 (sentinel 200) — the base STATIC stand (clip analysis:
 * foot ~4/9, corr +1.00 = no gait; clip0=a walk, clip6=a fall, so NOT those).
 * BACK = the walk clip played in REVERSE (motion 105 + anim_flags 0x80) → the gait
 * runs backward = stepping backward, distinct from the forward walk. (The exact
 * distinct back clip lives in the per-char common bank CD file #60, which is a raw
 * LBA region not in the extracted data; reverse-walk is the faithful-available
 * stand-in until #60 is extracted.) */
#define RE15_MOTION_WALK 105
#define RE15_MOTION_RUN  100
#define RE15_MOTION_BACK RE15_MOTION_WALK   /* + reverse flag, set below */
#define RE15_ANIM_REVERSE 0x80

/* IDLE-FIDGET — byte-exact port of the free-standing idle FSM switchD_8003206c
 * @0x80032038 (20-agent RE workflow, multi-verified). The FSM (idle sub-state
 * DAT_800aca5a) writes the clip index to player+0x94(=DAT_800acae8) and plays it
 * via FUN_8001f314 from the WEAPON bank (PL00W01, = our re15_w01) and, on
 * HP-escalation, the COMMON bank (PL00.edd, = our re15_pl00):
 *   case0->1 : clip 3  (1-frame neutral hold), held by a (rand&0x1f)+0x5a tick timer
 *   case2->3 : clip 1  (16f settle/weight-shift, one-shot)
 *   case4->5 : clip 2  (52f low-amplitude breathing sway; if HP<50 -> case9)
 *   case6->7 : clip 4  (50f leg fidget / weight-shift, one-shot) -> case8 -> case4 loop
 *   case9->a : clip 22 (or 23 if HP<30) from the COMMON bank
 * DAT_800acaee is HP (player+0x9a, init 0x64=100 @0x80031718; the damage handler
 * FUN_80012d60 does `HP -= dmg[type]`, <0 = death). (2026-06-09: the audit's
 * "idle ENERGY budget" label was WRONG — it IS HP; the idle FSM just reads HP to
 * gate the injured idle.) A separate POISON-style drain exists (FUN @0x80031e98:
 * `HP -= 2` then clamp to 1 if it went negative — never lethal, gated, not ported
 * = gameplay-only). HP does NOT drain while merely standing -> the
 * escalation (clips 22/23) never fires from a cold standing spawn, so a standing
 * player shows: neutral(3) -> settle(1) -> loop{ sway(2), leg-fidget(4) }.
 * All WEAPON clips are PL00W01.EDD: clip frame_counts [22,16,52,1,50,30,...].
 * (The "hand-through-hair" gesture is NOT in these FSM clips — corrected FK shows
 * no hand rises near the head in WEAPON/COMMON; it belongs to the phase-4 scripted
 * IDLE-bank path LAB_80050cb8, not this free-standing FSM. Open item, not guessed.)
 * Sentinels -> (bank, clip) resolved in main.c. */
#define RE15_MOTION_IDLE          200   /* neutral hold  -> W01 clip 3 (1f)   */
#define RE15_MOTION_IDLE_SETTLE   210   /* settle        -> W01 clip 1 (16f)  */
#define RE15_MOTION_IDLE_SWAY     211   /* breathing sway-> W01 clip 2 (52f)  */
#define RE15_MOTION_IDLE_LEG      212   /* hand-thru-hair-> W01 clip 4 (50f)  */
#define RE15_MOTION_IDLE_HURT1    213   /* injured idle  -> PL00 clip 22 (30f) HP<50 */
#define RE15_MOTION_IDLE_HURT2    214   /* worse-injured -> PL00 clip 23 (30f) HP<30 */
/* AIM/RAISE/FIRE poses (Phase 8.14 + 8.16). NOT sentinels — the byte-true PL00.EDD clip indices
 * used DIRECTLY. The player aim/fire FSM (action-8 sub-FSM FUN_80035810, jump table @0x80010b68 on
 * the aim sub-state DAT_800aca5a) writes the player motion (entity+0x94 = 0x800acae8) from the
 * COMMON bank PL00.EDD across its phases (disasm-verified):
 *   state 2->3 (@0x8003592c): RAISE  = clip 0x11=17, played to completion — the original advances
 *                             on anim_set's terminal-frame return (`aim_sub += anim_set()`), then
 *   state 4->5 (@0x800359d4): AIM-READY/FIRE = clip 0x12=18 (held; the discharge FUN_80045024 fires
 *                             at anim_frame==1 @0x80035a00 and re-plays clip 18 = the recoil).
 * Frame counts are PL00.EDD-VERIFIED (clip table u16 @ i*4): clip 17 = 10 frames, clip 18 = 25.
 * (NB 2026-06-30: an earlier handover note had these SWAPPED as 25/20 — the asset bytes are 10/25;
 *  always verify the clip table, never trust a remembered count.) Setting p->motion to a real PL00
 * clip (NOT a sentinel) makes anim_select skip the W01/idle remap and play that PL00.EDD clip with
 * HOLD-LAST (clip_override=-1); a fire resets anim_frame=0 -> clip 18 replays = the visible recoil
 * (game_step). In a gameplay (RBJ-less) room def_*==PL00.EDD, so the clip indexes the same byte-true
 * bank the original uses. The motion-0 prep + aim-elevation pitch (states 0/1, +0x6a & 0x3e0) are
 * the deferred aim-elevation subsystem (faithful-line collapsed). */
/* GUN FSM (RE-CORRECTED 2026-07-05 round 2, full discharge-chain disasm): the weapon aim/fire is
 * cmd1/ACTION 7 (@0x80032018 R1-held entry), gun sub-FSM @0x800740f4 dispatched by the equipped
 * INVENTORY ITEM @0x80074030 (handgun = ITEM 3). Clips are from the WEAPON PLW bank:
 *   RAISE  = clip 6  (sub0 @0x80032f18: frame 0, +0x8f=7, elevation LEVEL 0x4000,
 *                     autoaim latch FUN_8003703c(30000), auto-track a8f8(target, 0xC8))
 *   HOLD   = clip 8 / 10 (UP) / 12 (DOWN)  (sub1 @0x80033180; dpad switches elevation)
 *   FIRE   = SQUARE HELD in hold (@0x80033308) -> sub2 DISCHARGE: recoil clip 7/9/11 plays out,
 *            muzzle flash (0x02000800) + smoke (0x03000c00) + shell (0x04000800) at the gun bone,
 *            FUN_80011f50 damage, ammo-1; anim end -> back to HOLD (auto-refire while held)
 *   NO direct shot SE — the bang is data-driven by the spawned ESP effect.
 * (The earlier clip-0xD model was the MELEE FSM's knife DRAW — FUN_80035538 belongs to items 0-2;
 * its SE 0x1080001 is the knife draw. PL00 clips 17/18 = the box push. Both prior models wrong.)
 * The MOTION sentinel 215 tells the render "weapon-bank pose"; re15_player_aim_clip() carries the
 * actual W-bank clip. (2026-08-08: WAR 213 — kollidierte mit RE15_MOTION_IDLE_HURT1==213. Folge 1:
 * der One-Shot-Clamp unten klemmte den 30-Frame-Injured-Idle PL00 Clip 22 auf die W-Bank-Cliplaenge
 * (Freeze bei Frame fc(W)-1). Folge 2: stale Aim-Phase + Injured-Idle posierte die W-Bank statt
 * PL00 Clip 22. Sentinels sind Port-Konstrukte — die Trennung stellt das Original-Verhalten her:
 * der Injured-Idle spielt seine vollen 30 EDD-Frames (PL00.EDD Clip 22), der Aim-Zustand lebt im
 * Original ausschliesslich im cmd-1/ACTION-7-Zustand, nie im Clip-Register vermischt.) */
#define RE15_MOTION_AIM_W         215    /* sentinel: pose from the WEAPON bank (clip via aim_clip) */
#define RE15_AIM_TURN_RATE         24    /* @0x80074090: 5-u16 records per weapon, rec[0] low byte
                                          * = 0x3018&0xff = 0x18 = 24 — identical for ALL weapons */
/* Player aim sub-phase (the action-8 FSM @0x80035810 collapsed to the visible raise->ready path):
 * 0 = not aiming, 1 = RAISE (clip 17 playing), 2 = AIM-READY (clip 18 held, the discharge is gated
 * here). File-scope so game_step can gate the shot via re15_player_aim_ready() — the original only
 * fires in state 5 (READY), never mid-raise. */
#define RE15_AIM_NONE   0
#define RE15_AIM_RAISE  1
#define RE15_AIM_READY  2
#define RE15_AIM_RELOAD 3   /* gun FSM sub4 @0x80033d7c: clip 0xD plays out -> refill + HOLD */
#define RE15_AIM_LOWER  4   /* melee sub3 @0x80035424: clip 6 REVERSED plays out -> exit */
static int s_player_aim_phase = RE15_AIM_NONE;
/* Per-clip frame counts of the equipped W bank (the platform re-feeds them on bank switch).
 * PLW-verified layout (PL00W01≡W00≡W02 melee / PL00W03≡W04 gun, both 14 clips): melee draw+hold
 * = clip 0xD (15f); gun raise 6 (10f), holds 8/10/12 (1f), recoils 7/9/11 (23/24/24f), reload 0xD
 * (32f). re15_player_set_aim_clip_len(fc) survives as the "all clips = fc" test mock. */
#define RE15_AIM_CLIP_MAX 14
static uint16_t s_aim_clip_fcs[RE15_AIM_CLIP_MAX] = {0};
static int s_aim_cur_clip = 6;              /* the current W-bank clip (melee 0xD; gun 6/8/7/...) */
static int s_aim_recoil = 0;                /* 1 while the discharge/slash clip plays */
static int s_aim_elev = 0;                  /* 0 level / +1 up / -1 down (acaec bits 15/13) */
static int s_aim_melee = 0;                 /* latched item class at raise entry (item < 3) */
static int s_knife_in_hand = 0;             /* player word 0x800aca54 bit 0x4000: the knife DRAW
                                             * (sub4, clip 0xD + SE + model attach) runs only ONCE;
                                             * later aims RE-RAISE (sub0, clip 6, no SE). Persists
                                             * across lower/raise (@0x80034e88-a8 pre-check). */
/* Idle-FSM state (player only; single player_tick caller). s_idle_phase mirrors the original
 * idle sub-state classes of switchD_8003206c: 0 neutral(clip3,timer) 1 settle(clip1,1-shot)
 * 2 sway(clip2,timer) 3 hair(clip4,1-shot) 4 injured(clip22/23,replay). File-scope, damit der
 * Raum-/Lade-Reset sie wischen kann: der cmd-0-Wort-Store `sw 1,0x800aca58` @0x8003192c nullt
 * +0x05/+0x06 (Substate+Phase) bei JEDEM Raumwechsel — die Idle-Phase startet im Zielraum
 * frisch (vorher ueberlebte z.B. Phase 4/Injured die Tuer und der Zielraum begann mit Sway
 * statt neutral — gemessen, probe_hitdoor_entry_anim Kontroll-Lauf). */
static int      s_idle_phase = -1;   /* -1 = not idle / needs (re)entry */
static int      s_idle_timer = 0;
static uint32_t s_frame_ctr  = 0;    /* free-running RNG feed — bewusst NICHT resettet (kein
                                      * Original-Gegenstueck fuer einen Zaehler-Reset) */
void re15_player_idle_reset(void) { s_idle_phase = -1; s_idle_timer = 0; }
void re15_player_set_aim_clip_len(int fc)
{
    for (int i = 0; i < RE15_AIM_CLIP_MAX; i++) s_aim_clip_fcs[i] = (uint16_t)fc;
}
void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n)
{
    if (!fcs) return;
    if (n > RE15_AIM_CLIP_MAX) n = RE15_AIM_CLIP_MAX;
    for (int i = 0; i < n; i++) s_aim_clip_fcs[i] = fcs[i];
}
static int aim_cur_fc(void)
{
    return (s_aim_cur_clip >= 0 && s_aim_cur_clip < RE15_AIM_CLIP_MAX)
               ? (int)s_aim_clip_fcs[s_aim_cur_clip] : 0;
}
int  re15_player_aim_active(void) { return s_player_aim_phase != RE15_AIM_NONE; }
int  re15_player_aim_clip(void)   { return s_aim_cur_clip; }
int  re15_player_aim_elevation(void) { return s_aim_elev; }   /* -1 down / 0 level / +1 up */
/* TEST HOOK ONLY (same stance as re15_player_set_aim_clip_len): force the aim elevation so the
 * damage resolver's band gate can be exercised without driving the whole R1 + dpad aim FSM. */
void re15_player_set_aim_elevation_for_test(int elev) { s_aim_elev = (elev > 0) ? 1 : (elev < 0) ? -1 : 0; }
int  re15_player_knife_in_hand(void) { return s_knife_in_hand; }  /* 0x800aca54 bit 0x4000 */
void re15_player_aim_reset(void)                              /* test/room-change clean slate */
{
    extern re15_actor_t g_actors[];
    s_player_aim_phase = RE15_AIM_NONE;
    s_aim_recoil = 0; s_aim_elev = 0; s_knife_in_hand = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_flags &= (uint8_t)~0x80u;
}
/* HIT/GRAB interrupt (byte-true mechanism: the player command dispatch @0x80031c88 indexes
 * DAT_800aca58 — a cmd-2 flinch / cmd-5 grab write REPLACES the whole cmd-1 state incl. the
 * aim action; the dive/flinch writers carry NO aim gate (@0x80113b00 unconditional). Exit the
 * aim FSM but KEEP the knife-in-hand latch (aca50 bit 0x4000 — a separate flag the cmd write
 * does not touch; the LOWER-exit comment above documents the same persistence). Without this
 * the port suppressed the flinch whenever the player aimed — the crow-fight normal case
 * (crow_victim_anim.md F6). */
void re15_player_aim_interrupt(void)
{
    extern re15_actor_t g_actors[];
    s_player_aim_phase = RE15_AIM_NONE;
    s_aim_recoil = 0; s_aim_elev = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_flags &= (uint8_t)~0x80u;
}
/* SLASH damage window (byte-true @0x80035388-94): while the melee SLASH clip plays and the anim
 * frame is in [6..11], the resolver FUN_80011f50 is called EVERY tick (once-per-target latch +
 * recursion live inside weapon_fire). game_step polls this. */
int re15_player_slash_window(void)
{
    extern re15_actor_t g_actors[];
    if (!s_aim_melee || !s_aim_recoil) return 0;
    int f = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame;
    return (f >= 6 && f <= 11);
}
/* RELOAD entry (gun FSM sub4 @0x80033d7c, fired by the empty+press-edge gate @0x80033378):
 * clip 0xD from frame 0 with blend 7, elevation reset to LEVEL ((x&0x1fff)|0x4000 -> acaec),
 * NO SE at entry — the reload SE 0x01030001 plays at clip COMPLETION together with the
 * refill (@0x80033ebc-d4). */
void re15_player_reload_start(void)
{
    extern re15_actor_t g_actors[];
    if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil || s_aim_melee) return;
    s_player_aim_phase = RE15_AIM_RELOAD;
    s_aim_cur_clip = 0x0d;
    s_aim_elev = 0;                                       /* acaec -> 0x4000 LEVEL */
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frac  = 7;      /* blend 7 @0x80033dc8 */
}
int re15_player_reloading(void) { return s_player_aim_phase == RE15_AIM_RELOAD; }
/* W-Bank-Cliplaenge (die vom Platform-Loader via re15_player_set_aim_clip_lens gefuetterten
 * PLW-EDD-Frame-Counts) — Konsument: der Plc_dest-Mode-6-Event-Reach des Spielers
 * (0x800517f0 spielt PLW-Paar-B-Clips 1/2; game_step_common.c). */
int re15_player_wbank_clip_fc(int clip)
{
    return (clip >= 0 && clip < RE15_AIM_CLIP_MAX) ? (int)s_aim_clip_fcs[clip] : 0;
}
/* FIRE trigger (game_step, SQUARE held in HOLD/READY): GUN = DISCHARGE — recoil clip 7/9/11
 * plays out, then back to HOLD (sub2 @0x80033460: anim end -> sub1 = auto-refire cadence).
 * MELEE = SLASH (sub2 @0x80035314, byte-true): the SAME 7/9/11 elevation-clip formula
 * (7+2*up+4*down @0x8003535c-70), swing/whiff SE 0x1050001 at entry (@0x8003537c, ALWAYS —
 * hit and whiff alike); the damage runs per-tick in frames 6..11 (re15_player_slash_window).
 * Clip done -> HOLD; SQUARE held -> auto-repeat with one 1-tick hold gap. */
void re15_player_fire_start(void)
{
    extern re15_actor_t g_actors[];
    if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil) return;
    s_aim_recoil = 1;
    s_aim_cur_clip = 7 + (s_aim_elev > 0 ? 2 : s_aim_elev < 0 ? 4 : 0);   /* 7/9/11 (both FSMs) */
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frac  = 7;
    if (s_aim_melee) {
        extern void re15_audio_weapon_se(int idx);
        re15_audio_weapon_se(5);                       /* swing SE 0x1050001 (@0x8003537c) */
    }
}
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
static int16_t re15_atan2_q12_pl(int32_t dz, int32_t dx) { return re15_atan2_q12(dz, dx); }

int re15_player_aim_ready(void) { return s_player_aim_phase == RE15_AIM_READY && !s_aim_recoil; }
/* Test-Sichtfenster (nur Diagnose, kein Spiel-Code liest das): Phase im Low-Nibble,
 * Recoil-Flag in Bit 4. */
int re15_player_aim_phase_debug(void) { return (int)s_player_aim_phase | (s_aim_recoil ? 0x10 : 0); }
/* One-shot phase durations = the clip's exact frame_count (compute_actor_kf maps
 * anim_frame 1:1, so one cycle = frame_count ticks; a longer timer replays it —
 * that was the "hair 2x" bug). Timer-gated phases use the byte-exact pseudo-random
 * idle timers (FUN_8001af20 hash). PL00W01.EDD frame_counts: c1=16,c2=52,c3=1,
 * c4=50; PL00.edd c22=c23=30. */
#define IDLE_FC_SETTLE  16    /* W01 clip1 one cycle  */
#define IDLE_FC_LEG     50    /* W01 clip4 one cycle (the hair) */
#define IDLE_FC_HURT    30    /* PL00 clip22/23 one cycle */
/* HP thresholds for the injured idle (switchD_8003206c case4 slti 0x32, case9
 * slti 0x1e). HP (player+0x9a, DAT_800acaee) init 100; drops on damage later. */
#define IDLE_HP_CAUTION 50    /* HP < 50 -> injured idle (clip 22)  */
#define IDLE_HP_DANGER  30    /* HP < 30 -> worse injured (clip 23) */

/* Byte-exact idle timer randomiser: FUN_8001af20 @0x8001af20 = stateless hash
 * (x + (x>>7)) & 0xff. The original feeds it leftover register a0 (incidental,
 * not a designed RNG, so cycle-identical reproduction is impossible); the faithful
 * port (RE-agent recommendation) feeds a free-running frame counter through the
 * EXACT hash + the EXACT formulas: case0 (h&0x1f)+0x5a = 90-121; case4 h+0x3c. */
static inline int re15_idle_rand(uint32_t x) { return (int)(((x + (x >> 7)) & 0xffu)); }

void re15_player_cycle_motion(int delta, int clip_count)
{
    if (clip_count <= 0) return;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int m = (int)p->motion + delta;
    while (m < 0) m += clip_count;
    while (m >= clip_count) m -= clip_count;
    re15_actor_set_motion(p, (int16_t)m);
}

/* =============================================================================================
 *  KISTEN SCHIEBEN — Spieler-Substate 8 (der Handshake zum Objekt-Pass FUN_8002bd44)
 * =============================================================================================
 * Der Spieler-FSM hat DREI Tabellen (ghidra1_V2.txt 242019-242030):
 *   PTR_LAB_80073f90 (8)  = HAUPT-Zustand  (Spieler +0x04), Dispatch @0x80031cac
 *   PTR_LAB_80073fb0 (16) = SUB-ENTRY      (Spieler +0x05), Dispatch @0x80031eec
 *   PTR_LAB_80073ff0 (16) = SUB-UPDATE     (Spieler +0x05), Dispatch @0x80031f14
 * Substate 1 = Vorwaertsgehen (ENTRY LAB_800322e8), Substate 8 = SCHIEBEN
 * (ENTRY LAB_8003579c, UPDATE LAB_80035810).
 *
 * EINTRITT — der erste der genau ZWEI Leser des Kontaktbits 0x2000:
 *   @0x800323c0 `lw v0,DAT_800aca3c` / @0x800323c8 `andi v0,v0,0x2000`
 *   @0x800323cc `bne v0,zero,LAB_8003247c` mit Verzoegerungsschlitz `ori v0,zero,0x801`
 *   @0x8003247c `sw v0,DAT_800aca58`   (32 Bit!) -> +0x04 = 1, +0x05 = 8, +0x06 = 0, +0x07 = 0
 * ==> "+0x05 == 8", die Bedingung, die FUN_8002bd44 @0x8002bfe0/e4 verlangt, entsteht GENAU HIER.
 *
 * AUSSTIEG — der zweite Leser, der Sub-ENTRY-Handler LAB_8003579c:
 *   @0x800357a0 `lw v0,DAT_800ac768` / @0x800357a8 `andi v0,v0,0x10`   (virt. Bit 4 = UP)
 *   @0x800357b8 `lw v0,DAT_800aca3c` / @0x800357c0 `andi v0,v0,0x2000`
 *   @0x800357c4 `bne v0,zero,0x80035808`  -> beides erfuellt: NICHTS tun (weiterschieben)
 *   @0x800357d0-d8 sonst: nur wenn +0x06 == 5
 *   @0x800357e4 +0x06 = 6 / @0x800357f0 Clip 0x11 / @0x800357fc Frame 0 / @0x80035804 +0x8F = 7
 *
 * UPDATE LAB_80035810 = Sprungtabelle @0x80010b68 ueber +0x06 (DAT_800aca5a), 8 Zustaende:
 *   [0] @0x80035848  +0x06 = 1, Clip 0, Frame 0, +0x8F = 7, Speed(+0x8C) = 0   -> FAELLT in [1]
 *   [1] @0x80035878  Schritt-SE (EDD-Bit 0x4000), anim_set; YAW-RASTUNG auf 90 Grad
 *                    @0x800358d4-fc: t = (yaw>>2)&0xff ; yaw = (yaw & 0x200) ? yaw+t : yaw-t
 *                    @0x80035910-20: (yaw & 0x3e0) == 0 -> +0x06 = 2
 *   [2] @0x8003592c  +0x06 = 3, Clip 0x11, Frame 0, +0x8F = 7                  -> FAELLT in [3]
 *   [3] @0x80035958  anim_set(a2=0); +0x06 += Rueckgabe (1 = Clip zu Ende) -> 4
 *   [4] @0x800359d4  +0x06 = 5, Clip 0x12, Frame 0, +0x8F = 7                  -> FAELLT in [5]
 *   [5] @0x80035a00  Frame == 1 -> FUN_80045024(0x02070000) = Schiebe-SE (Bank 2 = snd0, Satz 7);
 *                    anim_set(a2=0) = Clip 0x12 im LOOP; EDD-Bit 0x2000 @0x80035a5c ->
 *                    FUN_800369f8(0,0) = Wurzelbewegung auf die Spielerposition
 *   [6] @0x80035a78  anim_set(a2=1) = Clip 0x11 RUECKWAERTS; +0x06 += Rueckgabe -> 7
 *   [7] @0x80035ab4  `sw 1,DAT_800aca58` = Zustand 1 / Substate 0 = Idle. Fertig.
 *
 * WURZELBEWEGUNG: FUN_800369f8 Modus 0 @0x80036aac-ae8 verschiebt den Spieler um die Differenz
 * der Wurzel-Weltposition zwischen diesem und dem gespeicherten Frame (`x -= cur - ref`,
 * `z -= cur - ref`). Der Port hat keinen Engine-Pose-Puffer (+0x188) und nimmt dieselbe Groesse
 * aus ihrer Quelle: der Wurzel-Translation der EMR-Keyframes (re15_emd_get_keyframe_speed),
 * yaw-rotiert — derselbe Weg, den der Port fuer die Gegner-/Grab-Wurzelbewegung schon fuehrt
 * (func_0x8001ad68). MESSBELEG (probe_1090_push, echte Assets): PL00.EDD Clip 0x12 hat 25 Frames
 * und traegt Bit 0x2000 auf GENAU den Frames 0..14; ueber diese Frames laeuft die Wurzel-X von
 * -312 auf +211 = +523 Einheiten Schub je Zyklus, Frames 15..24 (ohne Bit) sind die Erholung. */
static int s_push_sub = -1;      /* -1 = nicht im Substate 8; sonst DAT_800aca5a (0..7) */

int re15_player_push_substate(void) { return s_push_sub >= 0; }   /* DAT_800aca59 == 8 */
int re15_player_push_phase(void)    { return s_push_sub; }        /* DAT_800aca5a (Diagnose) */
void re15_player_push_reset(void)
{
    extern re15_actor_t g_actors[];
    if (s_push_sub >= 0) g_actors[RE15_ACTOR_SLOT_PLAYER].anim_flags &= (uint8_t)~0x80u;
    s_push_sub = -1;
}

/* PL00-Baenke fuer die Wurzelbewegung + die Cliplaengen 0x11/0x12 (der Port haelt sie im
 * game_step-Kontext; hier gespiegelt, damit der Substate-8-Handler sie lesen kann). */
static const re15_emd_skeleton_t  *s_pl00_skel;
static const re15_emd_animation_t *s_pl00_anim;
void re15_player_set_pl00_banks(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an)
{
    s_pl00_skel = sk; s_pl00_anim = an;
}
/* NUR-LESE-Zugriff auf Leons EIGENE Knochen-Hierarchie/Bind-Pose. Gebraucht von den
 * Opfer-Bone-Abfragen in enemy_ai_common.c: das Original ankert die Grab-/Fress-Effekte am
 * SPIELER-Part-Pool `*(Spieler+0x188) + 0x5A0` (= Part 8, 172*8+0x40) — z.B.
 *   80111dd4: lw a2,-13348(a2)   ; a2 = *(0x800ACBDC) = *(Spieler+0x188)
 *   80111de0: addiu a2,a2,1440   ; + 0x5A0 = Part 8
 * Dieser Pool wird aus LEONS Skelett gestellt (die Opfer-Bank liefert nur Clips+Keyframes) —
 * genau die Komposition, die auch der Renderer fuer den Grab-Override baut. NULL, solange
 * die Plattform die Bank noch nicht gespiegelt hat (dann greift der Rueckfall). */
const re15_emd_skeleton_t *re15_player_pl00_skel(void) { return s_pl00_skel; }
static int push_clip_fc(int clip)
{
    if (s_pl00_anim && clip >= 0 && clip < s_pl00_anim->clip_count &&
        s_pl00_anim->clips[clip].frame_count > 0)
        return s_pl00_anim->clips[clip].frame_count;
    /* Rueckfall, falls die Bank (noch) nicht gespiegelt ist — die AUS DEN ASSETBYTES
     * gemessenen Laengen von PL00.EDD (probe_1090_push): Clip 0x11 = 10, Clip 0x12 = 25.
     * Ohne das haenge die Phase 3 endlos, weil "Clip zu Ende" nie eintraete. */
    if (clip == 0x11) return 10;
    if (clip == 0x12) return 25;
    return 0;
}
/* Wurzel-Delta eines Clip-Frames, yaw-rotiert (Konvention wie der Gegner-/Grab-Pfad:
 * wx = cos*lx + sin*lz, wz = -sin*lx + cos*lz). Wendet nichts an, wenn Keyframe-Daten fehlen. */
static void push_root_step(re15_actor_t *p, int clip, int slot_now, int slot_prev)
{
    if (!s_pl00_skel || !s_pl00_anim) return;
    if (clip < 0 || clip >= s_pl00_anim->clip_count) return;
    const re15_emd_clip_t *c = &s_pl00_anim->clips[clip];
    if (c->frame_count <= 0) return;
    if (slot_prev < 0 || slot_now != slot_prev + 1) return;   /* Clip-Start / Umlauf: neu ankern */
    if (slot_now >= c->frame_count) return;
    int kf_n = (int)(re15_emd_get_frame_entry(s_pl00_anim, clip, slot_now)  & 0xFFFu);
    int kf_p = (int)(re15_emd_get_frame_entry(s_pl00_anim, clip, slot_prev) & 0xFFFu);
    int16_t sx, sy, sz, sx0, sz0;
    if (!re15_emd_get_keyframe_speed(s_pl00_skel, kf_n, &sx,  &sy, &sz))  return;
    if (!re15_emd_get_keyframe_speed(s_pl00_skel, kf_p, &sx0, &sy, &sz0)) return;
    int32_t dx = (int32_t)sx - sx0, dz = (int32_t)sz - sz0;
    if (dx == 0 && dz == 0) return;
    int32_t cs = re15_cos_q12((int)p->rot_y), sn = re15_sin_q12((int)p->rot_y);
    p->x += (int32_t)(( (int64_t)cs * dx + (int64_t)sn * dz) >> 12);
    p->z += (int32_t)((-(int64_t)sn * dx + (int64_t)cs * dz) >> 12);
}
/* Clip setzen wie an jeder Original-Clip-Stelle des Schiebe-FSM: Clip, Frame 0, +0x8F = 7. */
static void push_set_clip(re15_actor_t *p, int clip, int reverse)
{
    p->motion = (int16_t)clip;
    p->anim_frame = 0;
    p->anim_frac = 7;
    p->motion_init_delay = 0;
    if (reverse) p->anim_flags |= RE15_ANIM_REVERSE;
    else         p->anim_flags &= (uint8_t)~RE15_ANIM_REVERSE;
}
/* Die Yaw-Rastung auf 90 Grad (@0x800358d4-fc). Auf dem ROHEN 16-Bit-Kurswert wie das
 * Original (`lhu`/`sh` auf DAT_800acabe). */
static void push_yaw_snap(re15_actor_t *p)
{
    int yaw = (int)(uint16_t)p->rot_y;
    int t   = (yaw >> 2) & 0xff;
    yaw = (yaw & 0x200) ? (yaw + t) : (yaw - t);
    p->rot_y = (int16_t)(uint16_t)yaw;
}
static int push_yaw_aligned(const re15_actor_t *p)
{
    return (((int)(uint16_t)p->rot_y) & 0x3e0) == 0;   /* @0x80035910 */
}

/* Rueckgabe 1 = der Substate 8 besitzt diesen Frame (der normale Lauf-/Ziel-/Idle-Zweig
 * von re15_player_tick muss dann ausgelassen werden, genau wie das Original ausschliesslich
 * die Substate-8-Handler dispatcht). */
static int re15_player_push_fsm(re15_actor_t *p, uint16_t pad_bits)
{
    extern int re15_prop_push_contact(void);           /* aot_common.c: DAT_800aca3c & 0x2000 */
    extern void re15_audio_room_se_snd0(int se_id);

    if (s_push_sub < 0) {
        /* EINTRITT aus dem Vorwaerts-Gehen (Sub-ENTRY 1 @0x800323c0-cc -> `sw 0x801`).
         * Das Kontaktbit setzt der Objekt-Pass des Vorframes und verlangt dort selbst schon
         * UP gehalten (@0x8002bf24). Beim Zielen dispatcht das Original den Substate-7-DECIDE,
         * der ein reines `jr ra` ist (@0x80032e3c) — also kein Schiebe-Eintritt. */
        if (!re15_prop_push_contact()) return 0;
        /* @0x80032328 `andi v0,a2,0xf` / @0x8003232c: liegt KEINE der vier Richtungen an,
         * schreibt der Handler `ori v0,zero,0x1` = Substate 0 (Idle) und kommt am
         * 0x2000-Test @0x800323c0 gar nicht vorbei. */
        if (!(pad_bits & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN |
                          RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT))) return 0;
        if (s_player_aim_phase != RE15_AIM_NONE) return 0;
        s_push_sub = 0;                                 /* +0x06 = 0 aus dem 32-Bit-Store */
        s_idle_phase = -1;
    }

    /* Sub-ENTRY 8 = der AUSSTIEG (LAB_8003579c) — laeuft VOR dem Update (@0x80031eec vor
     * @0x80031f14). Virtuelles Pad-Bit 4 (0x10) ist derselbe RAW-UP-Knopf wie Bit 0
     * (Preset-Tabelle @0x80073dbc[4], siehe pad_common.c). */
    if (!((pad_bits & RE15_PAD_BIT_UP) && re15_prop_push_contact())) {
        if (s_push_sub == 5) {
            s_push_sub = 6;                             /* @0x800357e4 */
            push_set_clip(p, 0x11, 1);                  /* @0x800357f0/fc/@0x80035804 + a2=1 */
        }
    }

    switch (s_push_sub) {
    case 0:                                             /* @0x80035848 */
        s_push_sub = 1;
        push_set_clip(p, 0, 0);                         /* Clip 0, Frame 0, +0x8F = 7 */
        /* @0x80035874 `sh zero,DAT_800acae0` = Schrittgeschwindigkeit 0 — im Port gibt es
         * kein Skalar-Speed-Feld; der Schiebe-Zweig ruft schlicht keinen pos_advance. */
        /* fall through — das Original faellt @0x80035874 in [1] */
    case 1:                                             /* @0x80035878 */
        push_yaw_snap(p);
        if (push_yaw_aligned(p)) s_push_sub = 2;        /* @0x80035914-20, dann Ruecksprung */
        break;
    case 2:                                             /* @0x8003592c */
        s_push_sub = 3;
        push_set_clip(p, 0x11, 0);
        /* fall through — das Original faellt @0x80035954 in [3] */
    case 3: {                                           /* @0x80035958 */
        int fc = push_clip_fc(0x11);
        if (fc > 0 && (int)p->anim_frame >= fc - 1) s_push_sub = 4;   /* +0x06 += anim_set() */
        /* KEIN `else` — das Original speichert +0x06 @0x80035990 UNBEDINGT und prueft die
         * Yaw-Rastung DANACH (@0x80035988 `andi v0,a0,0x3e0` / @0x80035994 `beq -> ret`).
         * Endet der Clip in genau dem Bild, in dem der Kurs noch schief steht, laeuft die
         * Rastung also TROTZDEM ein letztes Mal. */
        if (!push_yaw_aligned(p)) push_yaw_snap(p);                   /* @0x80035988-c8 */
        break;   /* [4] dispatcht das Original erst im naechsten Frame (@0x80035994 -> ret) */
    }
    case 4:                                             /* @0x800359d4 */
        s_push_sub = 5;
        push_set_clip(p, 0x12, 0);
        /* fall through — das Original faellt @0x800359fc in [5] */
    case 5: {                                           /* @0x80035a00 */
        int fc = push_clip_fc(0x12); if (fc <= 0) fc = 25;
        if ((int)p->anim_frame == 1)
            re15_audio_room_se_snd0(7);                 /* 0x02070000 @0x80035a18 (Bank 2, Satz 7) */
        int slot = (int)p->anim_frame;
        if (slot >= fc) { p->anim_frame = 0; slot = 0; }               /* anim_set(a2=0) = LOOP */
        uint32_t e = s_pl00_anim ? re15_emd_get_frame_entry(s_pl00_anim, 0x12, slot) : 0u;
        if (e & 0x2000u)                                                /* @0x80035a5c */
            push_root_step(p, 0x12, slot, slot - 1);                    /* FUN_800369f8(0,0) */
        break;
    }
    case 6: {                                           /* @0x80035a78 */
        int fc = push_clip_fc(0x11);
        if (fc > 0 && (int)p->anim_frame >= fc - 1) s_push_sub = 7;
        break;
    }
    default:                                            /* [7] @0x80035ab4 */
        s_push_sub = -1;
        p->anim_flags &= (uint8_t)~RE15_ANIM_REVERSE;
        p->motion = RE15_MOTION_IDLE;                   /* Zustand 1 / Substate 0 */
        p->anim_frame = 0; p->anim_frac = 7;
        s_idle_phase = -1;
        return 0;
    }
    p->walk_active = 0;
    return 1;
}

void re15_player_tick(const re15_camera_view_t *view, uint16_t pad_bits)
{
    (void)view;

    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* CLIP ADVANCE — MOVED to the TOP of the tick (audit KF-1/ADV-BLEND-1, byte-true ordering).
     * FUN_8001f3bc poses the CURRENT counters then POST-advances them (+0x95 ++ @0x8001f610-63c,
     * +0x8f -- @0x8001f5b0-b4). The port splits pose (render, after game_step) from advance (here),
     * so "pose-then-advance" must map to "advance-then-(FSM read/seed)-then-pose": a fresh clip
     * seeded LATER in this same tick survives to the render (slot 0 with frac 7 = the byte-true
     * 87.5% first blend frame — it used to be advanced away to slot 1/frac 6 before ever rendering),
     * and next tick's FSM reads the post-advance counter exactly like the original's handlers do.
     * This mirrors the already-correct enemy ordering (re15_actors_anim_advance @game_step:491 runs
     * BEFORE the AI seeds in run_all @:499). SCD Plc_motion seeds (scd_vm_tick, before game_step)
     * are consumed identically at either position.
     *
     * Rate: FULL-RATE (1 keyframe per 30 Hz frame) — a "half-rate" divider citing FUN_80030660 bit
     * 0x10 was tried + REVERTED; DAT_800acc18 bit 0x10 is NEVER set (only `xori 0x20` toggle +
     * `sh zero` clear), so the half-rate path is dead and the original advances every frame. */
    /* AUSNAHME EVENT-REACH (Plc_dest-Sub 6 @0x800517f0, Spieler-Tabelle 0x80073e30[6]): dieser
     * Sub advanct +0x95 SELBST. Sein Body ruft `jal 0x8001f314` genau EINMAL pro Tick
     * (Phase 0/1 @0x8005188c, Phase 2/3 @0x800518f0), und anim_set erhoeht dort +0x95 um 1
     * (@0x8001f618 `addiu v0,v0,1` / @0x8001f61c `sb v0,149(t0)`) und dekrementiert +0x8f um 1
     * (@0x8001f5b0-b4) — einmal, nicht zweimal. Der Port bildet den Sub in
     * re15_player_event_reach_tick (game_step_common.c) ab, der genau diesen einen Advance macht;
     * lief der Advance hier zusaetzlich, waren es zwei pro Bild.
     * GEMESSEN (echter Lauf, ROOM1090 sub02, RE15_MOTRACE, vor dem Fix):
     *   [mot] F61 mo=1 af=2 frac=5 / F62 af=4 frac=3 / F63 af=6 frac=1 / F64 af=8 frac=0 ...
     * also +2 pro 30-Hz-Tick — der Halte-Clip lief exakt doppelt so schnell wie im Original
     * (16 Bilder PL00W01 Clip 1 in 8 statt 16 Ticks). */
    extern int re15_player_event_reach_clip(void);   /* game_step_common.c: >=0 = Sub 6 aktiv */
    if (re15_player_event_reach_clip() >= 0) {
        /* Sub 6 besitzt den Frame-Zaehler — hier NICHT advancen. */
    } else if (p->motion_init_delay > 0) {
        /* State-4 init hold (PSX +0x4=4): render keyframe 0 once WITHOUT advancing
         * the clip or consuming the FRAC crossfade (the original decrements +0x8f
         * INSIDE FUN_8001f3bc as it advances +0x95 — consuming it during the hold
         * would drop the frac=7 first blend frame). */
        p->motion_init_delay--;
    } else {
        p->anim_frame++;
        if (p->anim_frac > 0) p->anim_frac--;
        /* W-bank clips are ONE-SHOTS (raise/hold/recoil hold their terminal pose): clamp after
         * the advance so the render's %fc wrap can never replay them. Uses LAST tick's clip
         * state — byte-true, since this advance logically belongs to the END of the last tick. */
        if (p->motion == RE15_MOTION_AIM_W && aim_cur_fc() > 0 &&
            p->anim_frame > aim_cur_fc() - 1)
            p->anim_frame = (uint16_t)(aim_cur_fc() - 1);
    }

    /* BL-round 2026-05-29: player-mode FSM input gate. While SCRIPTED
     * (player_mode==2, the cinematic) the SCD owns the actor — ignore PAD
     * MOVEMENT only (PSX gates input on player_mode≠0). CRITICAL: the anim_frame
     * advancement below must STILL run so the cinematic clips play — gating the
     * whole function froze all animations (the bug). Modes 0/1 = gameplay.
     * 2026-06-13: ALSO freeze movement while a MESSAGE is on screen (the original
     * blocks the player during dialog/prompts). message_display_frames>0 = a
     * subtitle/prompt is showing; message_query = a YES/NO prompt is waiting for
     * input. Only MOVEMENT is gated (anim still ticks), same as the cinematic. */
    int msg_block = (g_scd.message_display_frames > 0) || (g_scd.message_query != 0);
    /* SCHIEBEN (Substate 8) besitzt Yaw, Pose und Translation vollstaendig: das Original
     * dispatcht in diesen Frames AUSSCHLIESSLICH die Substate-8-Handler (@0x80031eec
     * LAB_8003579c + @0x80031f14 LAB_80035810) — der Lauf-/Ziel-/Idle-Zweig unten laeuft
     * dann nicht. */
    int pushing = (g_scd.player_mode != 2 && !msg_block) ? re15_player_push_fsm(p, pad_bits) : 0;
    if (g_scd.player_mode != 2 && !msg_block && !pushing) {
        int yaw_delta = 0;
        int move_dir  = 0;
        /* RUN modifier: X (CROSS) on PSX / shift→CROSS on PC, HELD (Nutzer-bestätigt: RE1.5/RE2 walk
         * by default, hold X + direction to run). The door action uses the CROSS press-EDGE elsewhere. */
        int run    = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
        int moving = (pad_bits & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN)) != 0;
        /* Byte-true turn rate (see the constants block): TURNING IN PLACE (no forward/back) pivots at
         * 96/frame (DAT_80073ee5, MEASURED); turning WHILE walking forward curves at 48 (DAT_80073ea5).
         * The old model used walk/run (CROSS) here — that was wrong and made in-place pivots half-speed. */
        int turn_rate = !moving ? TURN_RATE_IN_PLACE_PER_FRAME
                                : (run ? TURN_RATE_RUN_PER_FRAME : TURN_RATE_WALK_PER_FRAME);
        if (pad_bits & RE15_PAD_BIT_LEFT)  yaw_delta -= turn_rate;
        if (pad_bits & RE15_PAD_BIT_RIGHT) yaw_delta += turn_rate;
        if (pad_bits & RE15_PAD_BIT_UP)    move_dir  += 1;
        if (pad_bits & RE15_PAD_BIT_DOWN)  move_dir  -= 1;

        if (yaw_delta != 0) {
            p->rot_y = (int16_t)((int)p->rot_y + yaw_delta);
        }

        /* AIM (Phase 8.14, faithful-line of the action-8 aim FSM @0x80035810): hold R1 to aim.
         * The original aim state ROOTS the player (no walk/run translation) while keeping the gun
         * up; turning (rot_y, above) stays allowed. Disable forward/back move so want_motion below
         * picks the aim pose, not walk/idle. (The raise IS implemented (gun clip 6 / melee draw clip 0xD @L432-436) and the 3-level
         * elevation command FSM too (holds 8/10/12 @L451-454, recoils 7/9/11 @L218/466) — clip-17 was
         * re-identified as the box-push, NOT the aim raise. Only the +
         * aim-elevation pitch are deferred; the held aim pose + the fire recoil are byte-true.) */
        /* R1 released -> LOWER (sub 3, BOTH machines — gun @0x80033c74 / melee @0x80035424 are
         * structurally identical: clip 6 REVERSED (f314 a2=1), elevation cleared, no SE; exit
         * clears the action). Three byte-true entry paths: (1) HOLD + !R1 (@0x80033200 gun /
         * @0x8003513c melee), (2) gun DISCHARGE + !R1 once frame > the per-weapon threshold
         * byte2 @0x80074092+(w-1)*5 (handgun rec = [24,48,7,..] -> 7; @0x8003364c) — the recoil
         * can be broken into the lower, (3) post-reload-exec + !R1 (@0x80033fbc; the port's
         * RELOAD->READY->next-tick-LOWER is the same observable order). Raise/slash/reload
         * themselves never test R1 and play out first. */
        int r1_held = (pad_bits & RE15_PAD_BIT_R1) != 0;
        if (!r1_held && s_player_aim_phase != RE15_AIM_NONE &&
            s_player_aim_phase != RE15_AIM_LOWER) {
            int enter_lower = 0;
            /* Per-weapon recoil-break frame threshold = byte2 @0x80074092+(w-1)*5 (byte-true, record
             * index = equipped weapon aca5d - 1): handgun records 2/3 = 7, the heavier guns records
             * 4..12 = 10. The port had hardcoded 7 (correct only for the STAGE1 handgun; 3 frames early
             * for the Glock/Beretta/Redhawk/M870/SPAS). */
            static const uint8_t recoil_break[16] = {0,0,7,7,10,10,10,10,10,10,10,10,10,0,0,0};
            extern int re15_player_equipped_weapon(void);   /* re15_damage.c (DAT_800aca5d) */
            int eq_w = re15_player_equipped_weapon();
            int rb_thr = (eq_w >= 1 && eq_w <= 16) ? recoil_break[eq_w - 1] : 7;
            if (s_player_aim_phase == RE15_AIM_READY && !s_aim_recoil)
                enter_lower = 1;                        /* HOLD + !R1 */
            else if (!s_aim_melee && s_aim_recoil && p->anim_frame > rb_thr)
                enter_lower = 1;                        /* gun recoil break (@0x8003364c, per-weapon byte2) */
            if (enter_lower) {
                s_player_aim_phase = RE15_AIM_LOWER;
                s_aim_recoil = 0;
                s_aim_cur_clip = 6;                     /* clip 6, REVERSED (a2=1) */
                s_aim_elev = 0;                         /* acaec &= 0x1fff (@LOWER entry) */
                p->anim_frame = 0; p->anim_frac = 7;
                p->anim_flags |= RE15_ANIM_REVERSE;
            }
        }
        /* Aim stays ACTIVE past the R1 release until the machine exits through LOWER (both
         * machines): a raise/slash/recoil plays out, then HOLD sees !R1 -> LOWER -> exit. */
        int aiming = r1_held || (s_player_aim_phase != RE15_AIM_NONE);
        if (aiming) move_dir = 0;   /* rooted: no translation while aiming */
        if (aiming) {
            /* Manual aim turn (table @0x80074090 = [24,48,...], 5-BYTE records per weapon): byte0=24
             * during RAISE/DRAW **and DISCHARGE/recoil**, byte1=48 during HOLD only (@0x80035270 base
             * 0x80074091); LOWER = constant 24 (@0x800354a0/c4). The DISCHARGE sub (aca5a=5 @0x800359dc,
             * clip 0x12) turns at byte0=24 (`acabe -= byte @0x80074090+(wpn-1)*5` @0x80035000-18), NOT
             * the HOLD byte1=48 — so while the recoil clip plays the turn is HALF the HOLD rate. The
             * port keeps the player in AIM_READY during recoil (s_aim_recoil), so gate it: 24 while
             * recoiling, 48 in steady HOLD. (Sustained handgun fire keeps Leon in recoil most frames,
             * so this is the common STAGE1 case.) */
            int rate = (s_player_aim_phase == RE15_AIM_READY) ? (s_aim_recoil ? 24 : 48) : 24;
            if (pad_bits & RE15_PAD_BIT_LEFT)  p->rot_y = (int16_t)(((int)p->rot_y + rate) & 0xfff);
            if (pad_bits & RE15_PAD_BIT_RIGHT) p->rot_y = (int16_t)(((int)p->rot_y - rate) & 0xfff);
            /* AUTO-TRACK toward the latched front target — byte-true ONLY during the RAISE/DRAW
             * sub (@0x80034fa0-c0 melee slew 0xC0 / gun sub0 slew 0xC8); the HOLD subs have no
             * a8f8 call. Latch radii: gun 30000, melee draw 2000, melee re-raise 5000.
             * L1 RETARGET (gun sub5 @0x80033eec, HOLD writer of 5 @0x800332f8): holding L1 in
             * HOLD re-runs the a8f8 slew + arc-test until aligned -> back to HOLD — the quick
             * re-face onto the nearest target. Port: L1 held in READY re-enables the track. */
            if (s_player_aim_phase == RE15_AIM_RAISE ||
                (s_player_aim_phase == RE15_AIM_READY && !s_aim_recoil &&
                 !s_aim_melee && (pad_bits & RE15_PAD_BIT_L1))) {
                extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);
                int32_t radius = s_aim_melee ? (s_aim_cur_clip == 0x0d ? 2000 : 5000) : 30000;
                int32_t slew   = s_aim_melee ? 0xc0 : 0xc8;
                int32_t tx, tz;
                if (re15_player_aim_target(radius, &tx, &tz)) {
                    int bearing = ((int)re15_atan2_q12_pl(tz - p->z, tx - p->x) - 0x400) & 0xfff;
                    int d = (((bearing - (int)p->rot_y) + 0x800) & 0xfff) - 0x800;
                    if (d >  slew) d =  slew;
                    if (d < -slew) d = -slew;
                    p->rot_y = (int16_t)(((int)p->rot_y + d) & 0xfff);
                }
            }
        }

        /* Pick the locomotion state: forward = walk (or run if held), back = the
         * walk clip with a negated step (RE1.5 mode 8: motion 0x30, step negated —
         * no distinct back animation), nothing = idle. */
        /* Idle-FSM state: file-scope (s_idle_phase/s_idle_timer/s_frame_ctr, decl. oben
         * bei den Aim-Statics) — der Raum-/Lade-Reset wischt Phase+Timer. */
        s_frame_ctr++;                       /* free-running RNG feed for the hash */

        int16_t want_motion = RE15_MOTION_IDLE;
        int32_t speed = 0;
        if (aiming) {
            /* byte-true ACTION-7 dispatch BY ITEM (@0x80074030): items 0-2 -> MELEE FSM 0x80034e70
             * (cold entry: sub==0 && !in-hand(0x4000) -> DRAW sub4 = clip 0xD + SE 0x1080001 +
             * model attach; else RAISE sub0 = clip 6, latch 5000, NO SE); items 3+ -> GUN FSM
             * 0x80032e9c (RAISE clip 6). BOTH continue to HOLD 8/10/12 with dpad elevation and
             * fire clips 7/9/11. The W bank switches with the item (melee trio W00≡W01≡W02,
             * gun pair W03≡W04 — CD-file index = DAT_800741e8[char]=76 + item, loader @0x80036b80). */
            if (s_player_aim_phase == RE15_AIM_NONE) {
                extern int re15_player_equipped_weapon(void);       /* re15_damage.c (DAT_800aca5d) */
                s_player_aim_phase = RE15_AIM_RAISE;
                s_aim_melee = (re15_player_equipped_weapon() < 3);  /* @0x80074030 class split */
                s_aim_recoil = 0; s_aim_elev = 0;
                if (s_aim_melee && !s_knife_in_hand) {
                    s_aim_cur_clip = 0x0d;                          /* DRAW (sub4 @0x80035538) */
                    extern void re15_audio_weapon_se(int idx);
                    re15_audio_weapon_se(8);                        /* SE 0x1080001 (@0x800355f8) */
                } else {
                    s_aim_cur_clip = 6;                             /* RAISE (sub0; melee @0x80034ee8) */
                }
            }
            want_motion = RE15_MOTION_AIM_W;
            if (s_player_aim_phase == RE15_AIM_RAISE &&
                p->motion == RE15_MOTION_AIM_W &&
                aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                if (s_aim_melee && s_aim_cur_clip == 0x0d)
                    s_knife_in_hand = 1;                            /* DRAW done -> in-hand 0x4000 */
                s_player_aim_phase = RE15_AIM_READY;                /* raise/draw done -> HOLD */
                s_aim_cur_clip = 8; p->anim_frame = 0;              /* HOLD clip 8 (both machines) */
            }
            if (s_player_aim_phase == RE15_AIM_READY && !s_aim_recoil) {
                /* dpad elevation (BOTH machines, same acaec scheme @0x80035164/1b8/204): the hold
                 * clip re-enters as 8/10/12 = 8 + 2*up + 4*down. */
                int elev = (pad_bits & RE15_PAD_BIT_UP) ? 1 : (pad_bits & RE15_PAD_BIT_DOWN) ? -1 : 0;
                if (elev != s_aim_elev) {                           /* elevation switch re-enters */
                    s_aim_elev = elev;
                    s_aim_cur_clip = 8 + (elev > 0 ? 2 : elev < 0 ? 4 : 0);   /* 8/10/12 */
                    p->anim_frame = 0;
                }
            }
            if (s_player_aim_phase == RE15_AIM_LOWER &&
                aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                s_player_aim_phase = RE15_AIM_NONE;                 /* LOWER done -> exit action-7 */
                p->anim_flags &= (uint8_t)~RE15_ANIM_REVERSE;       /* (in-hand flag persists) */
                /* LEAVE the aim sentinel THIS SAME tick. The exit clears phase→NONE, so the
                 * render's aim override (main.c, gated on re15_player_aim_active()) stops
                 * applying — but want_motion is still RE15_MOTION_AIM_W from above, and ohne
                 * aktiven Override ist der AIM_W-Sentinel fuer anim_select unaufloesbar (kein
                 * Sentinel-Zweig; er fiele als direkter Clip-Index in die def-Bank). Historie:
                 * als AIM_W noch ==213 war, posierte dieser eine Frame PL00 Clip 22 (Injured-
                 * Idle-Hunch) und seedete den FRAC-Crossfade = die gemessene "Land-Animation"
                 * (exit-frame kfi=661). Drop to the idle sentinel now so the render sees
                 * motion 200 the instant the aim ends. */
                want_motion = RE15_MOTION_IDLE;
                s_idle_phase = -1;                                  /* clean idle-FSM (re)entry */
            }
            if (s_aim_recoil && aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                s_aim_recoil = 0;                                   /* recoil/slash played out */
                if (!s_aim_melee) {
                    s_aim_cur_clip = 8 + (s_aim_elev > 0 ? 2 : s_aim_elev < 0 ? 4 : 0);
                    p->anim_frame = 0;                              /* gun: back to HOLD (refire) */
                }
            }
            /* RELOAD weapon-7 SPEEDLOADER drop (@0x80033e34-88): item 7 (SUPER REDHAWK) at anim
             * frame 10 spawns effect 0x04060800 (fx-id 4, sub 6 = the speedloader/shell drop) at
             * the gun bone + {0x91,0x1f4,-25} — the same anchor/offset as the discharge smoke. */
            if (s_player_aim_phase == RE15_AIM_RELOAD && p->anim_frame == 10) {
                extern int re15_player_equipped_weapon(void);
                if (re15_player_equipped_weapon() == 7) {
                    int32_t fcos = re15_cos_q12((int)p->rot_y);
                    int32_t fsin = re15_sin_q12((int)p->rot_y);
                    re15_esp_fx_spawn_ex(re15_esp_global_bank(), 4, 6, 0x0800,   /* 0x04060800 */
                        p->x + ( fcos * 0x1f4 >> 12), p->y - 2083 - 25,
                        p->z + (-fsin * 0x1f4 >> 12), (int16_t)p->rot_y);
                }
            }
            /* RELOAD sub4 exit (@0x80033ea8-d4): the 0xD clip finishing writes sub=1 (HOLD),
             * THEN the refill FUN_8004ebdc, THEN the reload SE 0x01030001 (ARMS record 3) —
             * refill + SE at clip COMPLETION, not at the sub4 entry. */
            if (s_player_aim_phase == RE15_AIM_RELOAD &&
                aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                extern void re15_ammo_reload_exec(void);
                extern void re15_audio_weapon_se(int idx);
                s_player_aim_phase = RE15_AIM_READY;                /* sub=1 -> HOLD */
                s_aim_cur_clip = 8; p->anim_frame = 0;
                re15_ammo_reload_exec();                            /* @0x80033ec0 */
                re15_audio_weapon_se(3);                            /* 0x01030001 @0x80033ec8-d4 */
            }
            s_idle_phase = -1;
        } else if (move_dir > 0) {
            if (run) { want_motion = RE15_MOTION_RUN;  speed = RUN_SPEED_PER_FRAME;  }
            else     { want_motion = RE15_MOTION_WALK; speed = WALK_SPEED_PER_FRAME; }
            s_idle_phase = -1;
        } else if (move_dir < 0) {
            /* BACK: distinct base-track back-walk clip (PL00.edd clip 1), moved in
             * reverse (move_dir = -1 negates the step, = FSM mode 8). */
            want_motion = RE15_MOTION_BACK; speed = BACK_SPEED_PER_FRAME;
            s_idle_phase = -1;
        } else if (yaw_delta != 0) {
            /* TURN (FSM mode 9, motion 0x60): the original plays the walk-step clip
             * (bank 0x174 clip 5) looping while yawing in place, speed 0 → "stepping
             * while turning". */
            want_motion = RE15_MOTION_WALK; speed = 0;
            s_idle_phase = -1;
        } else {
            /* IDLE-FIDGET FSM — byte-exact port of switchD_8003206c @0x80032038
             * (free-standing idle: phase +0x4==1, action +0x5==0). HP-gated, where
             * HP = the player's health (RE1.5 DAT_800acaee, our p->hp; init 100,
             * drops on damage). Healthy plays WEAPON-bank (PL00W01=re15_w01) clips
             * 3->1->{2,4}; when HP<50 the sway-entry escalates to the COMMON-bank
             * (PL00.edd=re15_pl00) INJURED idle clip 22, and clip 23 when HP<30
             * (original case4 slti 0x32 -> case9 slti 0x1e). Any movement resets.
             * Timers are the byte-exact pseudo-random idle timers (re15_idle_rand);
             * one-shot phases run exactly one clip cycle (= frame_count). */
            if (s_idle_phase < 0) {                 /* (re)enter: case0 neutral hold */
                s_idle_phase = 0;
                s_idle_timer = (re15_idle_rand(s_frame_ctr) & 0x1f) + 0x5a; /* 90-121 */
            } else if (--s_idle_timer <= 0) {
                switch (s_idle_phase) {
                    case 0: /* neutral -> settle (case2/3, clip1 one-shot) */
                        s_idle_phase = 1; s_idle_timer = IDLE_FC_SETTLE; break;
                    case 1: /* settle -> sway, OR injured if HP<50 (case4 slti 0x32) */
                        if (p->hp < IDLE_HP_CAUTION) { s_idle_phase = 4; s_idle_timer = IDLE_FC_HURT; }
                        else { s_idle_phase = 2; s_idle_timer = re15_idle_rand(s_frame_ctr) + 0x3c; }
                        break;
                    case 2: /* sway -> hair (case6/7, clip4 one-shot) */
                        s_idle_phase = 3; s_idle_timer = IDLE_FC_LEG; break;
                    case 3: /* hair -> sway, OR injured if HP<50 (loop back to case4) */
                        if (p->hp < IDLE_HP_CAUTION) { s_idle_phase = 4; s_idle_timer = IDLE_FC_HURT; }
                        else { s_idle_phase = 2; s_idle_timer = re15_idle_rand(s_frame_ctr) + 0x3c; }
                        break;
                    default: /* 4 = injured: TERMINAL — replays clip 22/23 forever. Byte-true (audit
                        * wf_4e8af27f): idle sub-state-10 handler @0x800322b8 is JUST an anim_set + jr ra
                        * (no HP load, no slti, no phase/timer write) — it does NOT recover to neutral on
                        * HP>=50. The injured idle is only LEFT by an EXTERNAL reset: any movement sets
                        * s_idle_phase=-1, and damage/action reset the FSM elsewhere. The old HP>=50
                        * recovery branch here was port-invented. Just re-arm the loop timer. */
                        s_idle_timer = IDLE_FC_HURT;
                        break;
                }
            }
            switch (s_idle_phase) {
                case 0:  want_motion = RE15_MOTION_IDLE;        break; /* W01 clip 3 */
                case 1:  want_motion = RE15_MOTION_IDLE_SETTLE; break; /* W01 clip 1 */
                case 2:  want_motion = RE15_MOTION_IDLE_SWAY;   break; /* W01 clip 2 */
                case 3:  want_motion = RE15_MOTION_IDLE_LEG;    break; /* W01 clip 4 */
                default: want_motion = (p->hp < IDLE_HP_DANGER) /* injured (PL00) */
                                       ? RE15_MOTION_IDLE_HURT2   /* clip 23, HP<30 */
                                       : RE15_MOTION_IDLE_HURT1;  /* clip 22        */
                         break;
            }
        }

        if (move_dir != 0) {
            /* Forward follows Leon's VISUAL facing. The renderer's R_y maps the
             * model's local forward (+X) to world (cos, -sin); move_dir = -1 backs
             * up (negated step). */
            int32_t sin_y = re15_sin_q12((int)p->rot_y);
            int32_t cos_y = re15_cos_q12((int)p->rot_y);
            p->x += ( cos_y * speed * move_dir) / 4096;
            p->z += (-sin_y * speed * move_dir) / 4096;
        }

        /* BACK plays the walk clip in reverse (legs step backward); all other
         * states play forward — EXCEPT the melee LOWER, which owns the reverse flag
         * (clip 6 played backwards, FUN_8001f314 a2=1 @0x800354b4). */
        if (s_player_aim_phase != RE15_AIM_LOWER) {
            if (move_dir < 0) p->anim_flags |= RE15_ANIM_REVERSE;
            else              p->anim_flags &= (uint8_t)~RE15_ANIM_REVERSE;
        }

        /* Drive the animation: on a clip change restart the cycle from frame 0. */
        if (p->motion != want_motion) {
            p->motion = want_motion;
            p->anim_frame = 0;
            p->motion_init_delay = 0;
            p->anim_frac = 7;   /* crossfade the locomotion/idle transition (FUN_8001f3bc) */
        }
        /* The player moves by PAD, not dest-seeking locomotion. walk_active drives
         * actor_locomotion (walk toward a stored Plc_dest) — keep it OFF so a stale
         * intro destination can't auto-rotate Leon. The walk/run clip still LOOPS:
         * compute_actor_kf loops W01 sentinels (105/100) via motion_override. */
        p->walk_active = 0;
    } else if (g_scd.player_mode != 2 && msg_block) {
        /* 2026-06-13: a message/prompt is on screen. Movement is already gated
         * (the big block above is skipped), but skipping it ALSO leaves the
         * previous motion in place — so if Leon was walking when the prompt
         * appeared he keeps playing the WALK clip on the spot. The original
         * stands him STILL during dialog. Force the IDLE pose (and clear any
         * dest-seek) so he holds the neutral stance until the message clears. */
        if (p->motion != RE15_MOTION_IDLE) {
            p->motion = RE15_MOTION_IDLE;
            p->anim_frame = 0;
            p->motion_init_delay = 0;
            p->anim_frac = 7;   /* crossfade into the dialog idle stance (FUN_8001f3bc) */
        }
        p->walk_active = 0;
    } else if (g_scd.player_mode == 2) {
        /* 2026-06-17: SCRIPTED cutscene. The SCD drives Leon via Plc_motion/Plc_dest; if it
         * has NOT (he's still in a LEFTOVER pad-walk/run sentinel carried in from gameplay —
         * motion 105/100, and it's neither a scripted Plc_dest walk [walk_active] nor a
         * post-arrival hold [anim_freeze]), settle him to IDLE. Otherwise a cutscene that
         * doesn't animate Leon (e.g. the ROOM1130 roller-door rise after the switch) leaves
         * him LOOPING the walk clip in place — the original stands him idle. Scripted motions
         * (other clip ids), in-progress Plc_dest walks, and the byte-true post-walk hold are
         * all left untouched. */
        if ((p->motion == 105 || p->motion == 100) && !p->walk_active && !p->anim_freeze) {
            p->motion = RE15_MOTION_IDLE;
            p->anim_frame = 0;
            p->motion_init_delay = 0;
            p->anim_frac = 7;
        }
    }
    /* (The per-tick clip advance runs at the TOP of this function — see the KF-1/ADV-BLEND-1
     * ordering note there: advance-then-seed-then-pose == the original's pose-then-post-advance.) */
    /* NPC anim advance MOVED OUT to re15_actors_anim_advance() (called UNCONDITIONALLY from
     * game_step). It used to live here inside re15_player_tick, which is SKIPPED in the grabbed/
     * dead/stair/menu branches — so the moment the player was grabbed, EVERY zombie's animation
     * (and the dying zombie's death clip) froze on a stale frame while HP drained through death =
     * the ROOM1140 "hang". The original advances each entity's anim in its own per-type handler
     * (FUN_8001a50c -> @0x80072bac[type]), INDEPENDENT of the player command FSM @0x80073f90. */
}

/* Shared per-frame NPC animation advance — byte-true independence of the player command FSM.
 * Runs in EVERY game_step branch (normal/grabbed/dead/stair/menu) so zombies keep animating and a
 * dying zombie's DEATH clip 0x1f can complete to CORPSE even while the player is grabbed/dead.
 * Freeze rules (byte-true): (a) state==CORPSE(7) holds its fallen pose — the original stops
 * dispatching a state-7 entity (out of the @0x8011f7b4[0..4] range) so its anim never re-advances;
 * (b) the lie-down spawn clips 0x0C/0E/12/13 stay pinned to their prone START frame; (c) the
 * Plc_motion 1-tick init-delay holds keyframe 0. Otherwise advance anim_frame + consume the FRAC
 * crossfade (mirrors FUN_8001f3bc). The render (re15_compute_actor_kf) wraps the frame per clip. */
/* Types whose per-type AI tick advances +0x95 (anim_frame) ITSELF (each ports its own handler's
 * f314 advance): dog 0x20 / crow 0x21 / alligator 0x23 / fx-emitter 0x24 / adult-spider 0x25 /
 * maggot 0x27 / cockroach 0x29 / tyrant 0x2b / ivy 0x2d / birkin 0x30+0x36 / zombie-girl 0x13 /
 * writher 0x1a / NPC 0x40. re15_actors_anim_advance must NOT also advance them — a SECOND advance
 * here doubles their clip rate, and the zombie lie-down pin below (clips 0x0C/0E/12/13 -> frame 0)
 * would freeze their ATTACK clips: the maggot bite (clip 0x12) / heavy (0x13) stuck at frame 0 =
 * the "harmless maggot" bug (live-diagnosed ROOM11C0: af pinned at 0/1, the damage window af 0x0c
 * never reached). Only the shared-root ZOMBIE types (0x10/0x11/0x12/0x16/0x18, ticked by
 * re15_enemy_ai_live_step) and the stationary SPIDER-BABY (0x26) do NOT self-advance and so rely on
 * this pass. Verified 2026-07-07: every mobile enemy tick self-advances in every state. */
static int re15_type_self_advances_anim(uint8_t t)
{
    switch (t) {
    /* 0x13 (zombie girl) REMOVED from the self-advance list (audit wf_827f186d zombie-girl #1):
     * she now runs the SHARED standard-zombie phase handlers (girl mode-0 tables @0x80120264/
     * @0x801202a8 are byte-identical to @0x8011f840/@0x8011f890 except row [0xa]) whose sub-FSMs
     * clock on the GLOBAL advancer + re15_enemy_clip_done, exactly like types 0x10/0x11/0x16. */
    case 0x1a: case 0x20: case 0x21: case 0x23: case 0x24:
    case 0x25: case 0x27: case 0x29: case 0x2b: case 0x2d: case 0x30:
    case 0x36: case 0x40:
        return 1;
    default:
        return 0;
    }
}

void re15_actors_anim_advance(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active) continue;
        if (a->state == RE15_AI_STATE_CORPSE) continue;   /* +0x4==7: corpse holds its fallen pose */
        if (re15_type_self_advances_anim(a->type)) continue;  /* self-advancing tick owns +0x95 (dog/maggot/crow/…) */
        /* NPC-family actors in the state-4 executor advance +0x95 via the executor's own sub-VM
         * (re15_npc_sub_motion / re15_npc_anim = the byte-true anim_set path). The original has NO
         * separate global advancer for a state-4 actor — anim_set is the sole stepper — so the generic
         * advancer must NOT also step them (a double advance would defeat the motion-FSM's play-once
         * HOLD, wrapping the pose instead of holding). Only 0x40 is self-advancing (skipped above); the
         * other 6 NPC types (0x42/0x45/0x47/0x49/0x4b/0x4d) are gated here while in the executor.
         * (Elliot-1170-Gleit-Historie 2026-08-09: dieser Skip hungerte Elliots Run aus, solange sein
         * Plc_dest auf dem alten WALKER-Sonderpfad lief — state 4 aus dem INIT (grid&0x40 @0x8011c860),
         * aber kein Sub-VM-f314. Seit der 7b-Umstellung laeuft 0x47 wie alle NPCs in der Sub-VM
         * (op_plc_dest, scd_vm.c), deren Walk-Subs jeden Tick f314 rufen (Sub 5 @0x80051630/@0x8005171c)
         * — der Skip hier ist damit auch fuer Elliot korrekt (Sub-VM = einziger Advancer). */
        { uint8_t t = a->type;
          int npc_fam = (t==0x40||t==0x42||t==0x45||t==0x47||t==0x49||t==0x4b||t==0x4d);
          /* ⛔ STATE 1 = die ESKORTE (Nutzer-Report 2026-08-28 "Adas Lauf- und Idle Animation
           * beim Folgen sind noch falsch"). Sie ist genau wie der State-4-Executor eine
           * Sub-VM, die ihren Bildzaehler SELBST fuehrt: jeder ihrer Exec-Subs endet mit
           * `jal 0x8001f314` (Stehen @0x8004f38c, Gehen @0x8004f5c8, Drehen @0x8004f7c4,
           * Nah @0x8004fb14, Laufen @0x8004ff68), und anim_set ist im Original der EINZIGE
           * Stepper (`lbu v0,149(v1)` / `addiu v0,v0,1` / `sb v0,149(v1)` @0x8001f60c-1c).
           * Ohne diesen Skip taktete der Port zweimal pro Bild - gemessen +2 Bilder je Bild
           * im Idle. */
          if ((a->state == 4 || a->state == 1) && npc_fam) continue; }
        /* ROOM1030 Kriechtor: der Sub-0x10-TOGGLE (f890[0x10]/f920[6] = FUN_80104f80) und die
         * Grid-1-Kriechmaschine (FUN_801036dc) rufen f314 SELBST in jedem Tick (@0x8010506c
         * bzw. @0x80103790-9c) — der globale Advancer muss diese Zustaende auslassen (sonst
         * Doppel-Advance), und der Clip-0x0C/0x0E/0x12/0x13-Pin unten darf sie NICHT greifen
         * (B2-Caveat 3: der Toggle spielt Clip 0x12 mit Startframe rng&3 @0x80104ffc jeden
         * Frame — ein Frame-0-Pin wuerde den Uebergang einfrieren). */
        { uint8_t t = a->type;
          if ((t==0x10||t==0x11||t==0x12||t==0x16||t==0x18) && a->state == 1 &&
              (a->sub_state_1 == 0x10 || (a->grid_id & 0x0f) == 1) &&
              !re15_ai_re2_for_type(t)) continue; }   /* RE1.5-Sub-Semantik; unter RE2-Flavor
                                                       * advanct der Brain-Besitz normal (WELLE B).
                                                       * MIXED (2026-08-23): typ-bezogen — die
                                                       * Zombie-Familie ist dort NICHT RE2, also
                                                       * gilt hier wieder die RE1.5-Semantik.
                                                       * Der Hund faellt gar nicht erst hier rein:
                                                       * 0x20 ist in re15_type_self_advances_anim
                                                       * (oben) und wird vorher uebersprungen —
                                                       * die Header-Warnung "ein NEUER Typ erbt die
                                                       * Exemptions NICHT automatisch" ist damit
                                                       * fuer 0x20 geprueft und erfuellt. */
        /* WELLE B (RE2-Flavor): every RE1.5-clip-number-specific pin below (0x0C/0x0E/0x12/0x13
         * lie-down, 0x2A sleeper) is meaningless against the RE2 EM01x bank — 0x0C IS the RE2
         * grab-BITE clip (grab 0x0B + 1 @0x80102830-34), 0x0E the crawler grab, 0x12 the feeding
         * loop (@0x80100ADC). Pinning them at frame 0 froze the RE2 bite (bite frame 16
         * @0x80100014 never reached). The RE2 brain owns its holds itself -> plain advance. */
        int re2z_owned = (re15_ai_re2_for_type(a->type) && re15_re2z_owns_type(a->type));
        uint16_t mo = a->motion;
        /* These clips hold frame 0 for the SPAWN lie-down (a downed zombie stays flat). But clips
         * 0x12/0x13 are DUAL-USE: the KNOCKDOWN GET-UP (re15_enemy_ai_live_knockdown case 4 sets
         * motion 0x12) MUST advance — its case-5 step waits on re15_enemy_clip_done to reach case 6
         * (re-engage). The PSX distinguishes by STATE, not clip: the get-up state calls FUN_8001f314
         * each frame (advance); the lie/downed states don't (hold). Release the pin only for the
         * get-up (sub_state_1==0x11, sub_state_2>=4 = case 4 onward), so a poise-broken zombie stands
         * back up instead of freezing forever in the get-up start pose. Strictly additive: every other
         * lie-down (incl. the spawn one, sub_state_2<4) still holds exactly as before. */
        int getup = (a->sub_state_1 == 0x11 && a->sub_state_2 >= 4);
        /* Second dual use of the SAME clips: the LYING wake machine FUN_80103a58. Its phases 0 and 1
         * hold (no f314 call — @0x80103aac-cc and @0x80103ad0-b04 don't touch +0x95), but phase 2 is a
         * clip PLAYOUT driven by the function's only f314 call @0x80103b14 (a0=+0x170, a1=+0x174,
         * a3=0x100). Without releasing the pin there the rise clip never finishes, so a script-woken
         * zombie stays frozen mid-getup forever (ROOM1070 user report). Gated as tightly as the
         * mechanism allows: zombie types only, state 1, decide row 0, phase >= 2, lying nibbles 7..10
         * (nibble 7/8 with phase >= 2 is unreachable in shipped data — no producer writes +0x6 >= 2
         * there — so in practice this only fires for the script-bumped nibble 9/10). */
        { uint8_t nib = (uint8_t)(a->grid_id & 0x0f); uint8_t t = a->type;
          if (!getup && (t==0x10||t==0x11||t==0x12||t==0x16||t==0x18)
              && a->state == 1 && a->sub_state_1 == 0 && a->sub_state_2 >= 2
              && nib >= 7 && nib <= 10)
              getup = 1; }
        if ((mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) && !getup && !re2z_owned)
            { a->anim_frame = 0; continue; }
        /* SLEEPING-LYING Warte-Halt (byte-true FUN_801054f4: die Schlafphasen 0/1 haben KEINEN
         * f314-Call — case 0 @0x80105534 setzt nur +0x1b8/+0x6, case 1 -> Epilog @0x8010560c;
         * der erste f314 kommt erst in der Wake-Phase @0x801055a8). Ohne den Halt spielte der
         * globale Advancer den Liege-Clip 0x2A einmal durch und pinnte den LETZTEN Frame =
         * die "nach vorne gebeugte" Dauerpose des ROOM10D0-Zombies (zombie_lyer_10d0.md D3).
         * Die Wake-Phase (sub_state_2>=2) advanct normal (clip_done treibt die Maschine). */
        if (mo == 0x2A && a->sub_state_1 == 0x12 && a->sub_state_2 <= 1 &&
            (!re2z_owned || a->re2z_re15_pose))
            /* re2z_re15_pose: der SITZ-IMPORT (10D0) spielt die RE1.5-Schlaf-Sequenz auch im
             * RE2-Flavor — der Frame-0-Halt (FUN_801054f4 Phase 0/1 ohne f314) gilt dort mit. */
            { a->anim_frame = 0; continue; }
        /* RE2-LIEGE-HALT (byte-true EXEC[7]: P0 speichert das Clip-Wort PLAIN = Rate 0
         * @0x801037CC-E4, und P1 @0x8010381C-28 ruft KEINEN Advance — Frame 0 steht, bis der
         * Limpet-Latch faellt). Der Port-Advancer spielte die 10 Liege-Frames durch und hielt
         * Frame 9 (gemessen f9~f0: Delta-Root <=1 unit — kosmetisch, trotzdem nachgezogen).
         * P2+ advanct normal (clip_done treibt die Aufsteh-Kette @0x801038E4-FC). */
        if (re2z_owned && a->state == 1 && a->sub_state_1 == 7 && a->sub_state_2 <= 1)
            { a->anim_frame = 0; continue; }
        if (a->motion_init_delay > 0) {
            a->motion_init_delay--;
        } else {
            /* BYTE-TRUE frame step — the original's anim_set (@0x8001F610-3C, self-disassembled,
             * cited in full in commit ceba1ba1):
             *   8001f610: lbu   v0,149(v1)   ; +0x95 is a BYTE
             *   8001f618: addiu v0,v0,1 ; 8001f61c: sb v0,149(v1)
             *   8001f624: sltu  v0,v0,s4     ; (frame+1) < the clip's frame count ?
             *   8001f628: bne   -> keep running    8001f638/3c: return 1 AND +0x95 = 0
             * +0x95 therefore always stays inside [0, fc-1]. The port's bare `anim_frame++` let a
             * clip-switch WITHOUT frame reset (byte-true: the standing stagger stores no +0x95,
             * census 0x80105b7c-0x80105e60) inherit an out-of-range index -> the stagger froze
             * (user symptom, ceba1ba1). That fix was reverted (991e566e) ONLY because it wrapped
             * with the DEFAULT bank's clip length while the renderer poses loco-bank states from
             * bank0 — the wrap point has to come from the SAME bank the renderer uses. That bank
             * rule now exists as re15_actor_clip_len() (re15_actor_uses_loco_bank, the one
             * definition main.c renders with) — exactly the condition the revert documented.
             * HOLD-LAST branch: the original has none (the caller stops calling anim_set at
             * return 1); the port's single global advancer pins a play-once clip (LOOP bit 0x04
             * clear) on its last frame — the long-standing emulation of that stop.
             * (dossier analysis/zombie_hit_1140.md D7) */
            int fc = re15_actor_clip_len(a);
            uint16_t nf = (uint16_t)(a->anim_frame + 1);
            if (fc > 0 && (int)nf >= fc) {
                a->anim_frame = (a->anim_flags & 0x04u)
                              ? 0                                /* LOOP  -> @0x8001f63c +0x95 = 0 */
                              : (uint16_t)(fc - 1);              /* play-once -> pin (port emulation) */
            } else {
                a->anim_frame = nf;
            }
            if (a->anim_frac > 0) a->anim_frac--;
        }
    }
}
