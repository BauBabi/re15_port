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

/* Locomotion speeds = the RE1.5 per-direction speed bytes (FUN_80041BE4 mode
 * tables 0x80076cXX): WALK=0x4B(75), RUN=0xC8(200), BACK=0x3C(60). Translation is
 * speed*heading (the W01 clips are in-place gait cycles). */
#define WALK_SPEED_PER_FRAME 75
#define RUN_SPEED_PER_FRAME  200
#define BACK_SPEED_PER_FRAME 60
/* Tank-control free-roam TURN rate (4096 units = 360°) — BYTE-TRUE from the PSX
 * player pad handler (ghidra1_V2.txt:127558 / :128011). Holding LEFT/RIGHT adds/
 * subtracts a per-camera turn byte to the heading accumulator DAT_800acabe once
 * per 30 Hz frame: WALK turn = DAT_80073ea5 = 0x30 (48), RUN turn = DAT_80073ee5
 * = 0x60 (96). That accumulator IS the facing heading (fed straight into the GTE
 * RotMatrix at FUN_8001ceb8), so its per-frame delta is the visible turn rate —
 * no further slew divides it down. The old single 0x80 (128) was 2.67× the walk
 * rate AND ignored the walk/run split → "Leon dreht sich zu schnell". */
#define TURN_RATE_WALK_PER_FRAME 0x30   /* 48 — walk turn (DAT_80073ea5) */
#define TURN_RATE_RUN_PER_FRAME  0x60   /* 96 — run turn  (DAT_80073ee5) */

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
#define RE15_MOTION_RAISE          17    /* PL00.EDD clip 17 = weapon-raise (one-shot, 10 frames)  */
#define RE15_RAISE_FC              10    /* clip 17 frame_count (PL00.EDD-verified) = raise done    */
#define RE15_MOTION_AIM            18    /* PL00.EDD clip 18 = aim-ready / fire (held, hold-last)   */
/* Player aim sub-phase (the action-8 FSM @0x80035810 collapsed to the visible raise->ready path):
 * 0 = not aiming, 1 = RAISE (clip 17 playing), 2 = AIM-READY (clip 18 held, the discharge is gated
 * here). File-scope so game_step can gate the shot via re15_player_aim_ready() — the original only
 * fires in state 5 (READY), never mid-raise. */
#define RE15_AIM_NONE   0
#define RE15_AIM_RAISE  1
#define RE15_AIM_READY  2
static int s_player_aim_phase = RE15_AIM_NONE;

int re15_player_aim_ready(void) { return s_player_aim_phase == RE15_AIM_READY; }
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
        /* RUN modifier: X (CROSS) on PSX / shift→CROSS on PC, HELD. (The door
         * action uses the CROSS press-EDGE elsewhere, so the two coexist.) */
        int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
        /* Byte-true turn rate: 0x60 while running (CROSS held), else 0x30 walk
         * (PSX reads DAT_80073ee5 in the RUN intent handler, DAT_80073ea5 in the
         * WALK one — both per-camera tables are uniformly 0x60 / 0x30). */
        int turn_rate = run ? TURN_RATE_RUN_PER_FRAME : TURN_RATE_WALK_PER_FRAME;
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
            /* action-8 aim sub-FSM @0x80035810 (byte-true): on aim-start play the RAISE clip 17
             * (state 2->3, motion 0x11) ONCE, then when it has played out advance to the held
             * AIM-READY pose clip 18 (state 4->5, motion 0x12). The original advances on anim_set's
             * terminal-frame return; the port mirrors it rate-aware via clip 17's frame_count
             * (RE15_RAISE_FC = 10, PL00.EDD-verified). speed stays 0 (rooted, set above). */
            if (s_player_aim_phase == RE15_AIM_NONE) s_player_aim_phase = RE15_AIM_RAISE;
            if (s_player_aim_phase == RE15_AIM_RAISE) {
                want_motion = RE15_MOTION_RAISE;                    /* clip 17 */
                if (p->motion == RE15_MOTION_RAISE && p->anim_frame >= RE15_RAISE_FC - 1)
                    s_player_aim_phase = RE15_AIM_READY;            /* raise played out -> READY */
            }
            if (s_player_aim_phase == RE15_AIM_READY)
                want_motion = RE15_MOTION_AIM;                      /* clip 18 held */
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
    }
    /* Also advance NPC anim_frames so their clips animate. EXCEPTION: a downed/corpse enemy
     * (its spawn action is a lie-down/get-up clip 0x0C/0x0E/0x12/0x13) must stay on its LYING
     * START frame — freeze its anim_frame at 0 so the looping clip renders slot 0 (prone). The
     * eating/walking enemies (other action indices) advance + loop normally. Full rate. */
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!g_actors[i].active) continue;
        uint16_t mo = g_actors[i].motion;
        if (mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) { g_actors[i].anim_frame = 0; continue; }
        if (g_actors[i].motion_init_delay > 0) {
            /* init-hold frame: render keyframe 0 without consuming the FRAC crossfade
             * (mirrors the player path above + byte-true FUN_8001f3bc decrement order). */
            g_actors[i].motion_init_delay--;
        } else {
            g_actors[i].anim_frame++;
            if (g_actors[i].anim_frac > 0) g_actors[i].anim_frac--;
        }
    }
}
