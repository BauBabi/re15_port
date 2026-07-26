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
 * The MOTION sentinel 213 tells the render "weapon-bank pose"; re15_player_aim_clip() carries the
 * actual W-bank clip. */
#define RE15_MOTION_AIM_W         213    /* sentinel: pose from the WEAPON bank (clip via aim_clip) */
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
int  re15_player_knife_in_hand(void) { return s_knife_in_hand; }  /* 0x800aca54 bit 0x4000 */
void re15_player_aim_reset(void)                              /* test/room-change clean slate */
{
    extern re15_actor_t g_actors[];
    s_player_aim_phase = RE15_AIM_NONE;
    s_aim_recoil = 0; s_aim_elev = 0; s_knife_in_hand = 0;
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
    if (p->motion_init_delay > 0) {
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
    if (g_scd.player_mode != 2 && !msg_block) {
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
        /* Idle FSM state (player only; single player_tick caller). s_idle_phase
         * mirrors the original idle sub-state classes of switchD_8003206c:
         *   0 neutral(clip3,timer) 1 settle(clip1,1-shot) 2 sway(clip2,timer)
         *   3 hair(clip4,1-shot)   4 injured(clip22/23,replay). */
        static int      s_idle_phase = -1;   /* -1 = not idle / needs (re)entry */
        static int      s_idle_timer = 0;
        static uint32_t s_frame_ctr  = 0;
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
         * other 6 NPC types (0x42/0x45/0x47/0x49/0x4b/0x4d) are gated here while in the executor. */
        { uint8_t t = a->type;
          if (a->state == 4 && (t==0x40||t==0x42||t==0x45||t==0x47||t==0x49||t==0x4b||t==0x4d)) continue; }
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
        if ((mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) && !getup) { a->anim_frame = 0; continue; }
        if (a->motion_init_delay > 0) {
            a->motion_init_delay--;
        } else {
            a->anim_frame++;
            if (a->anim_frac > 0) a->anim_frac--;
        }
    }
}
