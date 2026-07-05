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
#include "re15_enemy_ai.h"   /* RE15_AI_STATE_CORPSE — corpse-hold in re15_actors_anim_advance */

/* Locomotion speeds = the RE1.5 per-direction speed bytes (FUN_80041BE4 mode
 * tables 0x80076cXX): WALK=0x4B(75), RUN=0xC8(200), BACK=0x3C(60). Translation is
 * speed*heading (the W01 clips are in-place gait cycles). */
#define WALK_SPEED_PER_FRAME 75
#define RUN_SPEED_PER_FRAME  200
#define BACK_SPEED_PER_FRAME 60
/* Tank-control free-roam TURN rate (4096 units = 360°) — BYTE-TRUE from the PSX player pad handler.
 * Holding LEFT/RIGHT adds/subtracts a PER-MOVE-SUB-STATE, per-camera turn byte to the heading
 * accumulator DAT_800acabe once per 30 Hz frame; that accumulator IS the facing heading (fed straight
 * into the GTE RotMatrix), so its per-frame delta is the visible turn rate.
 *
 * CORRECTION 2026-07-03 (DuckStation parity, DETERMINISTIC direct-load measurement — re15-parity-verify):
 * the 48-vs-96 split is NOT walk-vs-run (the old model). It is MOVING-FORWARD vs TURNING-IN-PLACE:
 *   - TURN-IN-PLACE (LEFT/RIGHT, no forward/back): DAT_80073ee5 = 0x60 = 96/frame  <- MEASURED:
 *       hold R 1s -> +2784, 2s -> +5664 (linear ~96/frame at 30fps); the port's 48 was HALF -> Leon
 *       pivoted at half speed, so the same tank-path reached a DIFFERENT spot than the original.
 *   - turning WHILE walking forward (UP + LEFT/RIGHT): DAT_80073ea5 = 0x30 = 48/frame (the walk curve).
 * The four per-camera tables are @0x80073ea5(48 walk-fwd) / ec5(48 back) / ee5(96 turn-in-place) /
 * f25(72). The walk speed (75/frame) + forward direction + wall x=-5118 were already parity-matched;
 * only the turn-in-place rate was off. */
#define TURN_RATE_WALK_PER_FRAME     0x30   /* 48 — turn while walking forward (DAT_80073ea5) */
#define TURN_RATE_RUN_PER_FRAME      0x60   /* 96 — run turn (DAT_80073ee5 shares 0x60) */
#define TURN_RATE_IN_PLACE_PER_FRAME 0x60   /* 96 — pivot in place (DAT_80073ee5), MEASURED byte-true */

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
 * MELEE (items 0-2) = faithful-line slash stand-in: re-play the draw clip 0xD as the cadence
 * gate (the true melee ATTACK FSM 0x80034e70 slash chain is un-RE'd, documented deferred). */
void re15_player_fire_start(void)
{
    extern re15_actor_t g_actors[];
    if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil) return;
    s_aim_recoil = 1;
    s_aim_cur_clip = s_aim_melee ? 0x0d
                   : 7 + (s_aim_elev > 0 ? 2 : s_aim_elev < 0 ? 4 : 0);   /* 7/9/11 */
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frac  = 7;
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
         * picks the aim pose, not walk/idle. (The exact 3-level command FSM + the raise clip 17 +
         * aim-elevation pitch are deferred; the held aim pose + the fire recoil are byte-true.) */
        int aiming = (pad_bits & RE15_PAD_BIT_R1) != 0;
        if (aiming) move_dir = 0;   /* rooted: no translation while aiming */
        else s_player_aim_phase = RE15_AIM_NONE;   /* dropped R1 -> reset the raise/ready FSM */
        if (aiming) {
            /* byte-true aim steering (FUN_80035538): manual turn +-24/tick (weapon-1 rate, table
             * @0x80074090) on LEFT/RIGHT; AUTO-TURN toward the nearest front zombie within 2000
             * (FUN_8003703c latch) at a8f8 slew 0xc0/tick. */
            if (pad_bits & RE15_PAD_BIT_LEFT)  p->rot_y = (int16_t)(((int)p->rot_y + RE15_AIM_TURN_RATE) & 0xfff);
            if (pad_bits & RE15_PAD_BIT_RIGHT) p->rot_y = (int16_t)(((int)p->rot_y - RE15_AIM_TURN_RATE) & 0xfff);
            {
                extern int re15_player_aim_target(int32_t *tx, int32_t *tz);   /* re15_damage.c */
                int32_t tx, tz;
                if (re15_player_aim_target(&tx, &tz)) {              /* FUN_8003703c(30000) latch */
                    int bearing = ((int)re15_atan2_q12_pl(tz - p->z, tx - p->x) - 0x400) & 0xfff;
                    int d = (((bearing - (int)p->rot_y) + 0x800) & 0xfff) - 0x800;
                    if (d >  0xc8) d =  0xc8;                        /* a8f8 slew 0xC8 (@sub0) */
                    if (d < -0xc8) d = -0xc8;
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
             * (draw = W-bank clip 0xD via FUN_80035538, one-shot + hold-last, entry SE 0x1080001);
             * items 3+ -> GUN FSM 0x80032e9c (RAISE clip 6 -> HOLD 8/10/12 with dpad elevation;
             * the DISCHARGE swaps in recoil 7/9/11 which plays out and drops back to HOLD
             * @0x80033460). The W bank itself switches with the item (melee trio W00≡W01≡W02,
             * gun pair W03≡W04 — CD-file index = DAT_800741e8[char]=76 + item, loader @0x80036b80). */
            if (s_player_aim_phase == RE15_AIM_NONE) {
                extern int re15_player_equipped_weapon(void);       /* re15_damage.c (DAT_800aca5d) */
                s_player_aim_phase = RE15_AIM_RAISE;
                s_aim_melee = (re15_player_equipped_weapon() < 3);  /* @0x80074030 class split */
                s_aim_cur_clip = s_aim_melee ? 0x0d : 6;            /* draw 0xD / RAISE 6 */
                s_aim_recoil = 0; s_aim_elev = 0;
                if (s_aim_melee) {
                    extern void re15_audio_weapon_se(int idx);
                    re15_audio_weapon_se(8);                        /* FUN_80035538 entry SE 0x1080001 */
                }
            }
            want_motion = RE15_MOTION_AIM_W;
            if (s_player_aim_phase == RE15_AIM_RAISE &&
                p->motion == RE15_MOTION_AIM_W &&
                aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                s_player_aim_phase = RE15_AIM_READY;                /* raise/draw done -> READY */
                if (!s_aim_melee) { s_aim_cur_clip = 8; p->anim_frame = 0; }   /* gun: HOLD clip 8 */
                /* melee: clip 0xD holds its terminal pose (hold-last, no clip switch) */
            }
            if (!s_aim_melee && s_player_aim_phase == RE15_AIM_READY && !s_aim_recoil) {
                int elev = (pad_bits & RE15_PAD_BIT_UP) ? 1 : (pad_bits & RE15_PAD_BIT_DOWN) ? -1 : 0;
                if (elev != s_aim_elev) {                           /* elevation switch re-enters */
                    s_aim_elev = elev;
                    s_aim_cur_clip = 8 + (elev > 0 ? 2 : elev < 0 ? 4 : 0);   /* 8/10/12 */
                    p->anim_frame = 0;
                }
            }
            if (s_aim_recoil && aim_cur_fc() > 0 && p->anim_frame >= aim_cur_fc() - 1) {
                s_aim_recoil = 0;                                   /* recoil/slash played out */
                if (!s_aim_melee) {
                    s_aim_cur_clip = 8 + (s_aim_elev > 0 ? 2 : s_aim_elev < 0 ? 4 : 0);
                    p->anim_frame = 0;                              /* gun: back to HOLD (refire) */
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
                    default: /* 4 = injured: replays clip 22/23; recover (HP>=50) -> neutral */
                        if (p->hp >= IDLE_HP_CAUTION) {
                            s_idle_phase = 0;
                            s_idle_timer = (re15_idle_rand(s_frame_ctr) & 0x1f) + 0x5a;
                        } else { s_idle_timer = IDLE_FC_HURT; }
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
         * states play forward. */
        if (move_dir < 0) p->anim_flags |= RE15_ANIM_REVERSE;
        else              p->anim_flags &= (uint8_t)~RE15_ANIM_REVERSE;

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
    /* Phase 4.5.13-A: advance anim_frame every tick (not only on input) so
     * non-walk animations (wave, idle, etc.) play their full cycle. The
     * renderer divides anim_frame by 2 for 30Hz playback (main.c:400).
     *
     * BD-round 2026-05-28: respect motion_init_delay (PSX state=4→state=1
     * 1-tick FSM). On the first tick after Plc_motion, anim_frame stays
     * at 0 — visible as a "freeze" pose before animation begins. User-
     * verified in ablauf F217020-21 (Elliot freeze after fade-in). */
    /* FULL-RATE clip advance (1 keyframe per 30 Hz frame). NOTE 2026-06-17: a "half-rate"
     * divider (every-other-frame, citing FUN_80030660 bit 0x10) was tried + REVERTED — the
     * disasm shows DAT_800acc18 bit 0x10 is NEVER set (the only writes are `xori 0x20` toggle
     * @80030c8 + `sh zero` clear @8003197c), so the half-rate path is DEAD and the original
     * advances the player clip EVERY frame = full rate (kneel = 25 keyframes / 25 game-frames,
     * = the user's ~51 capture-frames @60fps). (The ROOM1150 kneel "too fast" was NEVER a rate
     * or crossfade issue — it was the wrong animation BANK: RBJ settle-clip vs PL00.EDD clip 11,
     * RESOLVED 2026-06-18, see anim_select_common.c anim_use_pl00.) */
    if (p->motion_init_delay > 0) {
        /* State-4 init hold (PSX +0x4=4): render keyframe 0 once WITHOUT advancing
         * the clip or consuming the FRAC crossfade. The original seeds +0x8f=7 on
         * the play frame and decrements it INSIDE FUN_8001f3bc as it advances +0x95;
         * decrementing here too would drop the frac=7 (12.5%) first blend frame and
         * front-load the fold. Hold frac during the init frame. */
        p->motion_init_delay--;
    } else {
        p->anim_frame++;
        if (p->anim_frac > 0) p->anim_frac--;
        /* W-bank clips are ONE-SHOTS (raise/hold/recoil hold their terminal pose): clamp after
         * the advance so the render's %fc wrap can never replay them. Per-clip lengths (the
         * platform feeds the equipped bank's EDD frame counts via set_aim_clip_lens). */
        if (p->motion == RE15_MOTION_AIM_W && aim_cur_fc() > 0 &&
            p->anim_frame > aim_cur_fc() - 1)
            p->anim_frame = (uint16_t)(aim_cur_fc() - 1);
    }
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
void re15_actors_anim_advance(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active) continue;
        if (a->state == RE15_AI_STATE_CORPSE) continue;   /* +0x4==7: corpse holds its fallen pose */
        uint16_t mo = a->motion;
        if (mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) { a->anim_frame = 0; continue; }
        if (a->motion_init_delay > 0) {
            a->motion_init_delay--;
        } else {
            a->anim_frame++;
            if (a->anim_frac > 0) a->anim_frac--;
        }
    }
}
