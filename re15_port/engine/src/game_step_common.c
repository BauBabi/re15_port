/*
 * game_step_common.c — the SHARED per-frame gameplay/interpreter step.
 * See re15_game_step.h. This file is compiled by BOTH the PSX-native and the
 * PC/SDL build so the two ports run the byte-identical interpreter order; the
 * only legitimate per-port divergence is architecture (render/input/audio/IO),
 * which stays in each main.c.
 */
#include <stdio.h>              /* fprintf — the dropped-event diagnostic below */
#include <stdlib.h>             /* getenv — RE15_STAIR_DEMO */
#include "re15_game_step.h"
#include "re15_engine.h"        /* re15_pad_virtual_word — the virtual pad-word builder
                                 * (pad_common.c; wave-6 finding 4) */
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_player.h"
#include "re15_scd.h"
#include "re15_anim_select.h"   /* re15_actor_footstep (foot-plant query) */
#include "re15_audio.h"         /* re15_audio_footstep */
#include "re15_rdt.h"           /* re15_rdt_floor_sound */
#include "re15_enemy_ai.h"      /* re15_enemy_ai_run_all — the LIVE-zombie per-frame pass (8.6) */
#include "re15_damage.h"        /* re15_player_is_dead / re15_player_death_tick (8.10 death FSM) */
#include "re15_menu.h"          /* re15_menu_* — the inventory/weapon-select menu (8.20) */
#include "re15_esp.h"           /* re15_esp_fx_spawn — the discharge muzzle/smoke/shell fx (ids 2/3/4) */
#include "re15_inventory.h"     /* re15_ammo_* — the byte-true magazine/reload model (FUN_8004ea6c/eae4) */
#include "re15_skeleton.h"      /* re15_sin_q12/re15_cos_q12 — the muzzle forward offset */
#include "re15_item_modal.h"    /* item-get pickup modal — freezes gameplay while presenting */

/* GAME-OVER / death presentation — REWRITTEN 2026-07-05 to the byte-true model (full raw RE of
 * LAB_8003694c + the game-over FSM FUN_8001500c/@0x80071d10, live-verified vs 92 DuckStation
 * samples, shots/you_died_seq/):
 *  - cmd 7 (LAB_8003694c) does NO fade and NO camera: it grows the player's floor-shadow quad into
 *    the dark-red BLOOD POOL (subtractive, color 0x38/0xff/0xff @0x8003699c-b8, half-extents
 *    +0xc/frame @0x800369bc-d8 from the 500x600 base; live terminal 0x7ac/0x810 after ~122 ticks).
 *    Its 0x78 timer is DEAD CODE (phase 0x800aca59 never advances - RAM-verified pinned 0x77).
 *  - The REAL presentation = the PARALLEL game-over FSM (main loop, cmd in {3,6,7}; sub @0x800b522a):
 *    sub0: BGM decrescendo/frame; ctr 0x32 -> WHITE ADDITIVE fade up rate +0x4bd (FUN_800217b0
 *          mode1=ABR1, brightness = level>>7; FUN_80021880)
 *    sub1: ctr 0x1b -> rate -0x2556 (decay) + flat-BLACK background (FUN_80021634(2,0))
 *    sub2: YOUDIED.TIM load + 4 letter quads (50-frame fly-in, FUN_80015a80 /0x32) + DEATH CAMERA
 *          (cut rewrite: target = corpse + {0x1f4,0xbb8,0x1f4} @0x8001547c-e4 + glide FUN_80015850;
 *          live: crane-up ~-100 y/tick, target locked on the corpse)
 *    sub3/4: heartbeat - lap every 0x13 frames, 3 laps, each a white pulse +0x2aaa (2f) / -0x2aaa
 *    sub5: swell +0x7ff, ctr 0xf -> -0x800
 *    sub6: ctr 0x50 -> subtractive fade-to-BLACK rate +0x400; ctr 0x6d -> stop audio + exit to the
 *          title flow (@0x80015810-38; main loop gate @0x8001d1e8). NO pad read - fixed timing.
 * Platform-read globals: */
int g_death_fade    = 0;   /* BLACK fade 0..255 (the sub-6 exit fade only) */
int g_gameover_active = 0; /* sub-6 ctr 0x6d reached -> the title tail */
int g_death_white   = 0;   /* ADDITIVE white overlay 0..255 (level>>7) */
int g_death_blackbg = 0;   /* flat-black background mode (room backdrop off, 3D corpse stays) */
int g_death_cam     = 0;   /* death-camera glide active (sub 2+) */
int g_death_pool    = 0;   /* blood-pool growth ticks (half-extents 500+12t x 600+12t, cap 122) */
int g_death_flyin   = -1;  /* YOU DIED letter fly-in tick 0..50 (-1 = hidden) */
int g_death_glow    = 0;   /* the spotlight-backdrop brightness (starts saturated at the black-bg
                            * switch, decays gradually — the live s14->s16 gray->dark ramp) */

static int s_go_sub = 0, s_go_ctr = 0, s_go_lap = 0, s_go_on = 0;
static int32_t s_go_lvl = 0, s_go_rate = 0;

/* PLAYER HIT-FLINCH state (hoisted to file scope so the status-screen open gate below
 * can read it — see the flinch block in re15_game_step for the full byte-true story). */
static int     s_hit_flinch = 0;
static int32_t s_hit_kb     = 0;             /* DAT_800acae0 knockback budget (decays 50/frame) */
static int16_t s_prev_hp    = 100;

/* ==== SPIELER-KNOCKDOWN-KLASSE (cmd-2 [4]/[5]) — analysis/player_knockdown.md (KD-1/KD-2
 * CONFIRMED, EXE-Handler 0x800360e8 (von VORN, Tabelle @0x80010b88) und 0x8003644c (von
 * HINTEN, @0x80010bb8) vollstaendig disassembliert). STAGE1-Ausloeser: der 0x27-Boss-Heavy-
 * Biss (aca59 = facing+4 @0x801187e8/f0); game-weit identische Writer in STAGE3/4/5.
 * Clips 0xb-0x10 aus der PLD-Basis-Bank (FUN_800314b0/FUN_80022300); i-Frames: +0x93 bleibt
 * fuer die GESAMTE Dauer gesetzt (@0x80036178 .. Exit @0x800362f8/@0x80036690). Die
 * aca3c-Bits 0x40/0x80 (Setter kartiert, Leser OFFEN — KD-11) sind dokumentiert, nicht
 * modelliert. Kraehen-Wurf (cmd-4 Mode 6) bleibt OFFEN (Exit unbelegt — Softlock-Gefahr,
 * §7.1) — dokumentierte Divergenz, nicht still degradiert. */
static int     s_knockdown  = 0;             /* 0 aus / 1 aktiv */
static uint8_t s_kd_dir     = 0;             /* 0 = vorn [4] / 1 = hinten [5] */
static uint8_t s_kd_phase   = 0;
static int32_t s_kd_speed   = 0;             /* DAT_800acae0-Aequivalent (Impuls 1000/500) */
static int16_t s_kd_t       = 0;             /* [5]-Decel-Zaehler t */

int  re15_player_knockdown_active(void) { return s_knockdown; }
/* ===== Plc_dest Mode 6 = EVENT-REACH auf dem SPIELER =====
 * Byte-true 0x800517f0 (Player-Sub-Tabelle 0x80073e30[6] — identisch zur NPC-Tabelle
 * 0x80076ca0[6]): Phase 0 spielt Clip 1 EINMAL (@0x80051844-74, Blend 7 @0x80051874),
 * dann Clip 2 als Idle-LOOP (@0x800518b4-dc, Blend 7 @0x800518d8). Kanal = player+0x170/
 * +0x174 = die PLW-Paar-B-Bank (FUN_80036b68; f314-Reads @0x80051884/88) — dieselbe
 * Maschine wie der Kraehen-Mode-6-Liegend-Halt (crow_victim_anim.md §1.4). Die dest-Felder
 * werden nie gelesen; Exit = der naechste Plc-Befehl des Scripts (Re-Init @0x80041bf8-c14).
 * Der alte Port startete stattdessen den WALKER Richtung (0,0) — Leon lief in ROOM10D0
 * die halbe Szene gegen den Weltursprung (marvin_10d0.md D3). Render: W-Bank-Override in
 * platform main (wie der Aim-Override); der FSM hier ist der einzige Frame-Advancer. */
static int s_ev_reach = 0;      /* 0 aus / 1 = Clip 1 einmal / 2 = Clip-2-Loop */
void re15_player_event_reach_begin(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_ev_reach = 1;
    pl->motion = 1; pl->anim_frame = 0; pl->anim_frac = 7;   /* Clip 1 @0x80051844/74 */
    pl->anim_freeze = 0;
    pl->walk_active = 0;
}
void re15_player_event_reach_end(void)  { s_ev_reach = 0; }

/* KOMMANDOREGISTER-RESET — das Port-Gegenstueck zum WORT-Store `sw zero,0x800aca58` @0x80031518
 * (FUN_800314b0, der Spieler-Load; einziger Caller ist der RAUMLADER FUN_800396fc, jal @0x80039788)
 * bzw. `sb zero,0x800aca58` @0x8001cbdc (Karten-Screen-Exit, direkt hinter jal FUN_80025c00
 * @0x8001cba4). Beide Original-Pfade loeschen cmd/Variante/Phase ZUSAMMEN — jeder Raum-Load und
 * jeder Rueckweg aus der Kartenmaske raeumt also einen laufenden Spieler-Befehl weg.
 *
 * Der Port hatte dafuer kein Gegenstueck: diese Statics leben ausserhalb von g_actors und wurden von
 * KEINEM Raum- oder Lade-Pfad angefasst. Ein Knockdown, den der Tod unterbricht, laesst s_knockdown==1
 * stehen (sein einziger Exit wird nie erreicht, weil der Death-Branch vorher gewinnt) — nach
 * Tod -> Title -> LOAD haette der Spieler die Kontrolle erst nach Ablauf der alten Phase zurueck.
 *
 * s_prev_hp = -1 = "keine Baseline": der HP-Drop-Detektor (unten, `hp < s_prev_hp`) kann im
 * ersten Tick nach einem Reset nicht feuern und re-basiert sich am Tick-Ende auf die live HP.
 * NICHT 100 setzen (der alte Wert): HP ueberlebt den Raumwechsel byte-true (einziger HP-Writer
 * ist der Session-Start @0x80031710-18, FUN_800314b0-Tail — der laeuft bei Tuerwechseln NICHT,
 * Gate @0x80039760-8c), eine 100er-Baseline fabrizierte deshalb bei hp<100 im ERSTEN Tick des
 * Zielraums einen PHANTOM-FLINCH: Clip 0xa + 500 Einheiten Rueckstoss ohne jede Eingabe
 * (gemessen, probe_hitdoor_entry_anim Lauf B/C vs. Kontrolle A). Im Original ist ein
 * Hit-Eintritt am Raumanfang mechanisch unerreichbar: cmd:=0 @0x8001cbdc, cmd-0 baut frisch auf.
 *
 * Aim + Idle: das cmd:=0 @0x8001cbdc toetet den GESAMTEN cmd-1-Zustand inkl. der ACTION-7-
 * Aim-Action (Dispatch @0x80031c88 indexiert aca58), und der cmd-0-Handler nullt die Aim-/
 * Turn-Bank @0x8003196c-94 und wischt per Wort-Store `sw 1,0x800aca58` @0x8003192c auch
 * +0x05/+0x06 (Idle-Substate/-Phase). Die Port-Statics dieser beiden FSMs muessen denselben
 * Reset erfahren, sonst ueberlebt eine LOWER-Phase die Tuer (Waffe-heben-Pantomime im
 * Zielraum) bzw. die Idle-Phase startet nicht neutral. */
void re15_player_cmd_reset(void)
{
    extern void re15_player_aim_interrupt(void);   /* player_common.c — Phase:=NONE, Knife-Latch
                                                    * bleibt (aca54-Bit 0x4000 ueberlebt, s. dort) */
    extern void re15_player_idle_reset(void);      /* player_common.c — Idle-Phase/-Timer */
    s_knockdown = 0; s_kd_dir = 0; s_kd_phase = 0; s_kd_speed = 0; s_kd_t = 0;
    s_hit_flinch = 0; s_hit_kb = 0;
    s_ev_reach = 0;
    s_prev_hp = -1;
    re15_player_aim_interrupt();
    re15_player_idle_reset();
}
int  re15_player_event_reach_clip(void) { return s_ev_reach ? ((s_ev_reach == 1) ? 1 : 2) : -1; }
static void re15_player_event_reach_tick(re15_actor_t *pl)
{
    extern int re15_player_wbank_clip_fc(int clip);   /* player_common.c: PLW-EDD-Laengen */
    if (!s_ev_reach) return;
    int clip = (s_ev_reach == 1) ? 1 : 2;
    int fc = re15_player_wbank_clip_fc(clip); if (fc < 1) fc = 1;
    pl->motion = (int16_t)clip;
    if ((int)pl->anim_frame + 1 >= fc) {
        if (s_ev_reach == 1) { s_ev_reach = 2; pl->motion = 2; pl->anim_frame = 0; pl->anim_frac = 7; }
        else pl->anim_frame = 0;                      /* Clip-2-Idle-Loop @0x800518dc */
    } else {
        pl->anim_frame++;
        if (pl->anim_frac > 0) pl->anim_frac--;
    }
}

void re15_player_knockdown_begin(int dir)
{
    /* cmd-2 [4]/[5] ersetzt wie jeder cmd-Write den kompletten cmd-1-Zustand inkl. Aim
     * (Dispatch @0x80031c88) — Aim-FSM beenden, Knife-in-Hand-Latch bleibt. */
    extern void re15_player_aim_interrupt(void);
    re15_player_aim_interrupt();
    s_knockdown = 1;
    s_kd_dir    = (uint8_t)(dir & 1);
    s_kd_phase  = 0;
    s_kd_t      = 0;
}

/* Clip setzen (+0x8f=7 an jeder Original-Clip-Site; rev = f314 a2=1 -> Rueckwaerts-Playback,
 * Port-Modell anim_flags 0x80 wie der BACK-Walk). */
static void kd_clip(re15_actor_t *pl, uint8_t clip, int rev)
{
    pl->motion = clip; pl->anim_frame = 0; pl->anim_frac = 7;
    pl->motion_init_delay = 1;
    if (rev) pl->anim_flags |= 0x80; else pl->anim_flags &= (uint16_t)~0x80u;
}

/* Bewegung entlang facing+0x800 (back=1, 245d8(0x800) @0x80036200) bzw. facing (back=0,
 * 245d8(0) @0x800365b8) mit Wand-Klemme; Rueckgabe 1 = Wand getroffen. mag darf negativ
 * werden ([4] hat KEINEN Decel-Clamp @0x800361c8-e0 — byte-true Vorwaerts-Drift). */
static int kd_move(const re15_game_ctx_t *c, re15_actor_t *pl, int32_t mag, int back)
{
    if (mag == 0) return 0;
    int32_t dx, dz, ox = pl->x, oz = pl->z;
    re15_player_knockback_delta(back ? pl->rot_y : (int16_t)(pl->rot_y + 0x800), mag, &dx, &dz);
    int32_t nx = ox + dx, nz = oz + dz;
    re15_collision_ensure_band(pl->y);
    re15_collision_constrain(c->rdt, ox, oz, &nx, &nz);
    re15_collision_objects(&nx, &nz);
    int wall = (nx != ox + dx || nz != oz + dz);
    pl->x = nx; pl->z = nz;
    return wall;
}

static int kd_adv(re15_actor_t *pl, int fc)
{
    if (pl->motion_init_delay > 0) { pl->motion_init_delay--; return 0; }
    pl->anim_frame++;
    return (int)pl->anim_frame >= fc - 1;
}

static void re15_player_knockdown_tick(const re15_game_ctx_t *c, re15_actor_t *pl)
{
    const re15_emd_animation_t *an = c->pl00_anim;
#define KD_FC(cl) ((an && (int)(cl) < an->clip_count && an->clips[(cl)].frame_count > 0) \
                   ? an->clips[(cl)].frame_count : 20)
    switch (s_kd_phase) {
    case 0:
        if (s_kd_dir == 0) {                      /* [4] Ph0 @0x80036144-a0: Sturz RUECKWAERTS */
            kd_clip(pl, 0x0d, 0);                 /* Clip 0xd @0x80036144-4c */
            s_kd_speed = 1000;                    /* Impuls @0x8003616c */
        } else {                                  /* [5] Ph0 @0x800364a8-50c: Sturz VORWAERTS */
            kd_clip(pl, 0x0c, 0);                 /* Clip 0xc @0x800364a8-b0 */
            s_kd_speed = 500;                     /* @0x800364c0/d0 */
        }
        pl->hit_react |= 1;                       /* i-Frames @0x80036178-80 / @0x800364e4 */
        re15_audio_core_se(1);                    /* SE 0x04010001 @0x80036184-88 */
        s_kd_phase = 1;
        break;
    case 1: {                                     /* Sturz-Playout + Physik */
        if (s_kd_dir == 0) {
            s_kd_speed -= 15 * (int32_t)pl->anim_frame;    /* @0x800361c8-e0, KEIN Clamp */
            if (kd_move(c, pl, s_kd_speed, 1)) {           /* Wand-Probe @0x8003620c-18 */
                kd_clip(pl, 0x0f, 0); s_kd_phase = 3;      /* SLAM-Zweig @0x8003622c-30 -> Ph5
                                                            * (Clip 0xf @0x80036348-5c; das
                                                            * aca3c|=0x80 ist dokumentiert, Leser
                                                            * OFFEN KD-11) */
                break;
            }
        } else if ((int)pl->anim_frame < 0xf) {            /* Move-Gate frame<15 @0x80036544 */
            if (kd_move(c, pl, s_kd_speed, 0)) s_kd_speed = 0;   /* Wand=Stopp @0x800365ac-b0 */
            s_kd_speed -= 5 * s_kd_t; s_kd_t++;            /* Decel 5*t @0x8003656c-8c */
        }
        if (kd_adv(pl, KD_FC(pl->motion))) {
            if (s_kd_dir == 0) { kd_clip(pl, 0x0e, 0); s_kd_phase = 2; }  /* [4] Ph2/3 Clip 0xe */
            else               { kd_clip(pl, 0x10, 0); s_kd_phase = 4; }  /* [5] Ph2 Clip 0x10 */
        }
        break; }
    case 2:                                       /* [4] Ph2/3: Aufstehen vorw. + speed-15 Clamp0 */
        s_kd_speed -= 15; if (s_kd_speed < 0) s_kd_speed = 0;   /* @0x800362bc-e0 */
        (void)kd_move(c, pl, s_kd_speed, 1);
        if (kd_adv(pl, KD_FC(pl->motion))) s_kd_phase = 6;      /* Exit Ph4 @0x800362f4 */
        break;
    case 3:                                       /* [4] Ph5/6: Slam-Clip 0xf Playout */
        if (kd_adv(pl, KD_FC(pl->motion))) { kd_clip(pl, 0x10, 0); s_kd_phase = 4; }  /* Ph7 Clip 0x10 */
        break;
    case 4:                                       /* Boden-Clip 0x10 Playout */
        if (kd_adv(pl, KD_FC(pl->motion))) { kd_clip(pl, 0x0b, 1); s_kd_phase = 5; }
                                                  /* Aufstehen = Clip 0xb RUECKWAERTS
                                                   * (ori a2,1 @0x800363c8 / @0x80036638) */
        break;
    case 5:                                       /* Aufstehen-Playout */
        if (kd_adv(pl, KD_FC(pl->motion))) s_kd_phase = 6;
        break;
    default:                                      /* EXIT ([4] Ph4/Ph11 @0x800362f4/@0x8003640c,
                                                   * [5] Ph6 @0x8003667c): aca58:=1, +0x93 = 0
                                                   * (@0x800362f8-Region / @0x80036690),
                                                   * aca3c &= ~0xC0 (@0x80036410-34/@0x80036680-a4).
                                                   * Clip +0x94 bleibt UNANGETASTET (dieselben
                                                   * `sw 1`-Wort-Exits wie der Flinch) — kein
                                                   * `motion = 0`-Blitz. Lief der Schluss-Clip
                                                   * RUECKWAERTS (Aufstehen = 0xb reversed,
                                                   * @0x800363c8/@0x80036638), ist die Endpose
                                                   * der Vorwaerts-Slot 0: Frame auf 0 setzen,
                                                   * wenn das Reverse-Bit faellt, damit derselbe
                                                   * Keyframe stehen bleibt. */
        s_knockdown = 0;
        if (pl->anim_flags & 0x80u) {
            pl->anim_frame = 0;
            pl->anim_flags &= (uint16_t)~0x80u;
        }
        pl->hit_react &= (uint8_t)~1u;
        break;
    }
#undef KD_FC
}

static void re15_gameover_fsm_reset(void)
{
    s_go_sub = s_go_ctr = s_go_lap = s_go_on = 0;
    s_go_lvl = s_go_rate = 0;
    g_death_fade = g_death_white = g_death_blackbg = g_death_cam = g_death_pool = 0;
    g_death_flyin = -1;
    g_gameover_active = 0;
}

/* One death tick of the byte-true chain (called from the death branch below). */
static void re15_gameover_fsm_tick(void)
{
    if (!s_go_on) { re15_gameover_fsm_reset(); s_go_on = 1; }
    if (g_death_pool < 122) g_death_pool++;               /* cmd-7 pool +0xc/frame, live cap 122 */
    s_go_lvl += s_go_rate;                                /* FUN_80021880: level += rate */
    if (s_go_lvl < 0) s_go_lvl = 0;
    int b = (int)(s_go_lvl >> 7);                         /* brightness = level>>7 */
    g_death_white = b > 255 ? 255 : b;
    if (g_death_glow > 56) g_death_glow -= 4;             /* ...and ramps down over ~50 ticks to the
                                                           * dim hold (live s14->s16 ~1.1s) */
    switch (s_go_sub) {
        case 0:                                           /* BGM decrescendo (0x3c) - port: no BGM yet */
            if (++s_go_ctr >= 0x32) { s_go_rate = 0x4bd; s_go_sub = 1; s_go_ctr = 0; }
            break;
        case 1:
            if (++s_go_ctr >= 0x1b) {
                s_go_rate = -0x2556;
                g_death_blackbg = 1;                      /* FUN_80021634(2,0) */
                g_death_glow = 255;                       /* backdrop starts saturated... */
                s_go_sub = 2; s_go_ctr = 0;
            }
            break;
        case 2:                                           /* YOU DIED + death camera arm */
            g_death_cam = 1;
            g_death_flyin = 0;
            s_go_sub = 3; s_go_ctr = 0;
            break;
        case 3:                                           /* heartbeat laps (0x13 frames each) */
            if (g_death_flyin < 50) g_death_flyin++;
            if (++s_go_ctr >= 0x13) {
                s_go_ctr = 0; s_go_lap++;
                s_go_sub = (s_go_lap < 3) ? 4 : 5;
            }
            break;
        case 4:                                           /* white heartbeat pulse */
            if (g_death_flyin < 50) g_death_flyin++;
            if (s_go_ctr == 0) s_go_rate = 0x2aaa;
            if (s_go_ctr == 2) { s_go_rate = -0x2aaa; s_go_sub = 3; }
            s_go_ctr++;
            break;
        case 5:                                           /* the final swell */
            if (g_death_flyin < 50) g_death_flyin++;
            if (s_go_ctr == 0) s_go_rate = 0x7ff;
            if (s_go_ctr == 0xf) { s_go_rate = -0x800; s_go_sub = 6; s_go_ctr = 0; break; }
            s_go_ctr++;
            break;
        case 6:                                           /* exit: fade to black + leave */
            if (g_death_flyin < 50) g_death_flyin++;
            if (s_go_ctr >= 0x50) {                       /* +0x400/frame subtractive -> 8/frame */
                int f = (s_go_ctr - 0x50) * 8;
                g_death_fade = f > 255 ? 255 : f;
            }
            if (s_go_ctr >= 0x6d && !g_gameover_active)
                g_gameover_active = 1;                    /* -> the title tail (audio stop deferred) */
            s_go_ctr++;
            break;
        default: break;
    }
}


void re15_game_step(const re15_game_ctx_t *c)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* ITEM-GET MODAL FREEZE (byte-true FUN_8001db28: g_pauseflags |= 0xff000000): while the pickup
     * zoom/flip presentation runs, the WHOLE game step is frozen — player move, collision, AOT scan,
     * enemy AI, model anim (re15_actors_anim_advance below), SCD-event dispatch all halt. Only the
     * modal FSM (ticked at 30 Hz from the platform loop) + rendering advance. The modal is armed from
     * inside re15_aot_scan on the TRIGGER frame (its tail still runs that frame, byte-true), then this
     * gate takes over from the next tick. Unreachable outside an item pickup = no room regression. */
    if (re15_item_modal_active()) return;

    if (s_go_on && !re15_player_is_dead())
        re15_gameover_fsm_reset();                       /* continue-reload revived the player */

    /* Action-button press edge (Square). Drives BOTH the stair trigger and the
     * door AOT scan below. */
    g_aot_action_pressed = (c->pad_pressed & RE15_PAD_BIT_SQUARE) ? 1 : 0;

    /* (Der Objekt-Notch-Refresh re15_object_notch_update lief frueher HIER am Step-Anfang —
     * er laeuft jetzt zusammen mit dem Aktor-Stempel am STEP-ENDE, byte-true zur
     * Frame-Position des AUTO-Pool-Scans FUN_800436a8 @0x8001ce1c. Die VM des Folgeframes
     * liest damit wie das Original immer den Stand des Vorframe-ENDES.) */

    /* Expose the per-frame VIRTUAL press-edge word to the SCD VM / dialog FSM — the
     * original reads the config-REMAPPED edge word DAT_800ac76c (FUN_80030444 tail
     * sw @0x8003057c), NOT the raw pad. Wave-6 finding 4: the previous identity feed
     * (g_scd_pad_edge = pad_pressed) delivered virtual confirm 0x4000 on physical
     * CROSS; per the preset-0 table @0x80073dbc[14] it is RAW SQUARE (0x0080). */
    extern uint16_t g_scd_pad_edge;
    g_scd_pad_edge = re15_pad_virtual_word(c->pad_pressed);

    /* HELD action-button state (Square) for the dialog FSM's fast-forward: the original
     * (FUN_80028134 state 1) reads DAT_800ac768 (held pad) — holding the button makes the
     * typewriter timer fall 4× = speed up the writing. g_aot_action_pressed above is the
     * EDGE (advance/confirm); this is the LEVEL (held). */
    extern uint8_t g_scd_action_held;
    g_scd_action_held = (c->pad_current & RE15_PAD_BIT_SQUARE) ? 1 : 0;
    /* Full HELD word for the dialog FSM's fast-forward: FUN_80028134 @0x80028214 reads the
     * VIRTUAL held pad DAT_800ac768 & 0x4000 — virtual 0x4000 <- RAW SQUARE (@0x80073dbc[14],
     * wave-6 finding 4; the old "= CROSS" label was wrong). */
    extern uint16_t g_scd_pad_held;
    g_scd_pad_held = re15_pad_virtual_word(c->pad_current);

    /* (Plc_neck head-look FSM is computed inside re15_skel_compute_pose at the head bone —
     * it needs the root bone matrix there to get the look angle in the correct frame. The
     * old re15_neck_update body-relative slew is retired to avoid double-slewing.) */

    /* STATUS/INVENTORY SCREEN (wave 2, byte-true): the gameplay-tail START poll
     * @0x8001cd64-cde8 — raw START press-edge sets the sticky open request; the latch is
     * gated on the player hit-react byte u8 @0x800acae7 (player base 0x800aca54 + 0x93)
     * == 0. The port's hit-react equivalents are the flinch/grabbed/death branches (the
     * hurt-FSM states this file models; OPEN: the original's exact +0x93 write sites
     * beyond the hurt FSM were not traced — spec cites only the ==0 gate). Once latched,
     * the transition FSM (DAT_800b5359 stages) + the menu task run INSTEAD of the whole
     * gameplay step: the original SUSPENDS task 0 (status|=0x40, FUN_80029bf8(0) as the
     * FIRST menu-init op @0x800460bc) — no player/enemy/SCD/AOT scan/action driver runs
     * while the menu is open (spec: the port's old "menu tick + AOT scan" model was the
     * flagged divergence — closed here). The pad globals above stay written so a
     * lingering timed/paged message (stage-1 wait @0x8001c9c8) can still be advanced by
     * the platform-side msg tick. */
    if (c->rdt_ok)
        re15_menu_start_poll(c->pad_pressed,
                             (s_hit_flinch == 0 && s_knockdown == 0 && !re15_player_is_grabbed() &&
                              !re15_player_is_dead()) ? 1 : 0);
    if (re15_menu_gameplay_frozen()) {
        re15_menu_fsm_tick(c->pad_pressed, c->pad_current);
        return;
    }
    int grabbed_branch = 0;      /* the grabbed-pin branch ran this tick (its body push happens AFTER
                                  * the victim placement at the end of the step; the normal branch
                                  * already pushed inline — never both, no same-tick double push) */

    /* PLAYER HIT-FLINCH (#22, byte-true FUN_80031c44 -> state 2 -> FUN_80035af0 -> hurt sub-FSM): a
     * non-lethal enemy hit routes the player command FSM to state 2 (HIT). FUN_80035af0 dispatches on
     * the direction sub-state DAT_800aca59 via the jump table @0x800741a8: entry [2] (@0x80035de0)
     * plays clip 0x8, entry [3] (@0x80035f64) plays clip 0x9 — the FRONT/BACK hurt anim, NOT the fixed
     * clip 0xa the port used to force. Both entries also arm a BACKWARD KNOCKBACK: DAT_800acae0 = 0xc8
     * (200, the step magnitude @0x80035e44/@0x80035fc8) decaying by DAT_800acaf2 = 0x32 (50) per frame
     * (@0x80035f20), pushed via FUN_800245d8(0x800) = rotate (mag,0,0) by Ry(facing + 0x800). UNBLOCKED
     * this session — the ported enemy AI now applies the damage. The live melee attacks write hp
     * DIRECTLY (they don't route through re15_player_take_damage), so the direction is derived here
     * from the byte-true YAW compare FUN_8001a780 against the nearest hostile (see the inline
     * citation at the clip pick below); no attacker (poison/environment) -> clip 0xa. Detect the
     * drop here (before re15_enemy_ai_run_all re-damages at the end of the step; s_prev_hp updated
     * pre-damage so the next tick sees the drop). Grabbed/dead/stair take precedence. */
    /* KEIN Aim-Gate (crow_victim_anim.md F6, CONFIRMED): das Original kennt keins — der
     * Flinch-Writer schreibt cmd 2 UNCONDITIONAL (@0x80113b00; einzige Gates des Dive sind
     * vert @0x80113ab0-b4 und hit_react==0 @0x80113ac0-cc), und der cmd-Dispatch @0x80031c88
     * ERSETZT den kompletten cmd-1-Zustand inkl. Aim. Der Flinch bricht das Zielen ab, nicht
     * umgekehrt (das alte `!re15_player_aim_active()`-Gate unterdrueckte jeden Flinch, solange
     * der Spieler zielte — der Normalfall im Kraehenkampf). */
    extern void re15_player_aim_interrupt(void);  /* player_common.c */
    if (c->rdt_ok && pl->hp < s_prev_hp && pl->hp >= 0 && s_hit_flinch == 0 &&
        s_knockdown == 0 &&        /* die Knockdown-Klasse hat Vorrang vor dem Flinch-Detector
                                    * (der Boss-Heavy-Biss wuerde sonst doppelt reagieren) */
        !re15_player_is_dead() && !re15_player_is_grabbed() && !re15_stair_active()) {
        const re15_actor_t *atk = re15_nearest_hostile(pl);      /* the enemy that struck (adjacent) */
        uint8_t clip = 0x0a;                                     /* [0]/[1] fallback (@0x80035bd0) */
        /* RICHTUNG = der byte-true YAW-Vergleich FUN_8001a780 (@0x8001a788-a4):
         *   ret = ((player.rot_y - attacker.rot_y + 0x400) & 0xfff) < 0x800
         * ret 0 = der Angreifer schaut dem Spieler ins GESICHT (frontal) -> aca59 = 0+2 = [2]
         * -> Clip 8 (@0x80035e38-40); ret 1 = gleiche Blickrichtung (Treffer von HINTEN) ->
         * [3] -> Clip 9 (@0x80035fbc-c4). aca59 = a780+2 (@0x80113b10-28 Kraehen-Dive; die
         * uebrigen cmd-2-Writer nutzen denselben Helper). KEINE Positionsrechnung — die alte
         * Port-Ableitung aus Positionen (hit_from_front -> 9) war invertiert
         * (crow_victim_anim.md F7). */
        if (atk) clip = ((((int)pl->rot_y - (int)atk->rot_y + 0x400) & 0xfff) < 0x800)
                            ? 0x09 : 0x08;
        re15_player_aim_interrupt();             /* cmd-2 ersetzt den Aim-Zustand (@0x80031c88) */
        s_hit_flinch = (clip == 0x0a) ? 20 : 22; /* byte-true clip play-out = PL00.EDD frame_count (clip 0x8=22,
                                                  * 0x9=22, 0xa=20; 1 tick/frame, no 0x8000 tween frames). The
                                                  * original ends the hurt FSM when anim_set FUN_8001f314 reaches
                                                  * the clip's EDD count (aca5a 1->2), NOT a fixed 15. */
        s_hit_kb     = 0xc8;                      /* DAT_800acae0 = 200 (@0x80035e44) */
        pl->motion = clip; pl->anim_frame = 0; pl->anim_frac = 7;
        pl->motion_init_delay = 1;               /* seed tick renders frame 0 (byte-true pose-then-advance:
                                                  * the flinch branch below would otherwise ++ it away
                                                  * before the first render — audit KF-1 class) */
    }
    s_prev_hp = pl->hp;                           /* pre-damage baseline for the NEXT tick's drop check */

    if (c->rdt_ok && re15_stair_active()) {
        /* Engine-driven stair traversal (action-triggered): auto-walk Leon
         * up/down + force the stair clip + sink/raise Y. The player does NOT
         * steer — SKIP player_tick + collision (the 0x4000-latch behaviour) —
         * but the RVD camera scan KEEPS running (byte-true: the original's
         * per-frame cam scan is ungated by the stair) so the pit's RVD zone
         * flips the cut as Leon crosses it during the descent. */
        re15_stair_tick(c->rdt, c->pl00_skel, c->pl00_anim);
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else if (c->rdt_ok && re15_player_is_dead()) {
        /* PLAYER DEATH (Phase 8.10, byte-true core of the death FSM): HP < 0 -> the player is dead.
         * The original routes the player's per-frame command FSM to the death-sequence handler
         * (@0x80073f90[state]: [7] = the GRABBED death @0x8003694c, what the grab reaches; [3] = the
         * generic death @0x800366bc), which freezes input and runs a fade + death camera on a
         * 120-frame timer, then game over — it NEVER reads the pad. The port freezes the player
         * exactly like the stair/grab branches (skip re15_player_tick + collision) and advances the
         * byte-true death timer (re15_player_death_tick = 0x78 -> 0). The full presentation IS ported:
         * the eaten-anim plays on Leon (re15_player_victim_tick, from the grabber's bank2 — zombie EM10
         * or dog EM020), and the parallel game-over FSM (re15_gameover_fsm_tick below) drives the white
         * flash / YOU DIED / death camera / fade-to-black / title tail (live-verified vs 92 DuckStation
         * samples). This branch is unreachable until a hit drives HP < 0 (the grab, in
         * ROOM1140), so a healthy room is unaffected = no 1170 regression. Death takes precedence over
         * the grab: a zombie that killed the player then dead-grabs the corpse (the engage's hp<0
         * dead-grab arm) while the player runs the death sequence. Keep the RVD cam scan (death cam). */
        (void)re15_player_death_tick();                  /* keep the legacy 0x78 counter ticking (its
                                                          * countdown is DEAD CODE in the original -
                                                          * the presentation is the FSM below) */
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
        re15_gameover_fsm_tick();                        /* the byte-true parallel game-over chain */
    } else if (c->rdt_ok && re15_player_is_grabbed()) {
        grabbed_branch = 1;
        /* PLAYER-GRABBED LOCK (Phase 8.10, byte-true LAB_80036834): a live zombie has the player
         * latched (DAT_800aca58 = cmd 5). The original routes the player's per-frame command FSM to
         * the cmd-5 grabbed handler, which pins the player + plays the grabbed pose and NEVER reads
         * the pad — so the player cannot steer or walk away while held (he takes the repeated -5
         * bites). The port pins the player exactly as the stair traversal does (engine-driven, the
         * player does NOT steer): SKIP re15_player_tick + collision + the door-start scan. The player
         * holds his current XZ (byte-verified 2026-07-13: there is NO XZ pin/pull, the player simply freezes; the
         * grabbed-Y reference DAT_800acc0e = -floor*1800 IS modeled (enemy_ai_common.c:458-463).
         * Still deferred: only the per-type grabbed bone POSE/anim). The grabbing zombie's hitbox + the bite damage are applied by
         * re15_enemy_ai_run_all at the END of this step (same slot as the lunge). The RVD camera cut
         * scan KEEPS running (byte-true: the per-frame cam scan is ungated by the player's state), so
         * the cut still frames the grab. This branch is unreachable unless a live zombie grabs, so a
         * room with no live zombie (ROOM1170/1240 boot) never enters it = no 1170 regression. */
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else if (c->rdt_ok && s_knockdown) {
        /* KNOCKDOWN-Klasse (cmd-2 [4]/[5], 0x800360e8/0x8003644c): engine-getrieben wie
         * Stair/Grab — kein Pad, kein Steer; RVD-Scan laeuft weiter. i-Frames halten
         * (+0x93 bleibt gesetzt bis zum Exit). analysis/player_knockdown.md F1. */
        re15_player_knockdown_tick(c, pl);
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else if (c->rdt_ok && s_hit_flinch > 0) {
        /* HIT-FLINCH branch: root the player + play the directional flinch clip (0x8/0x9/0xa, set at
         * trigger; same engine-driven skip as the stair/grab branches — no pad, no steer), keep the RVD
         * cam scan running. Each frame apply the BACKWARD KNOCKBACK (byte-true FUN_800245d8(0x800)
         * @0x80035f18): shove the player along facing + 0x800 (= 180 deg, away from the front) by the
         * current DAT_800acae0 magnitude — rotate (mag,0,0) by Ry(angle) exactly like the walker step
         * (actor_locomotion.c) — then clamp to the room walls/objects so a shove into a wall stops. Then
         * DAT_800acae0 -= DAT_800acaf2 (50), clamp at 0 (@0x80035f20) -> 200,150,100,50 over 4 frames.
         * When the clip plays out (timer -> 0) motion returns to idle. Unreachable unless a non-lethal
         * hit landed, so a room with no combat never enters it = no 1170 regression. */
        if (pl->motion_init_delay > 0) pl->motion_init_delay--;   /* seed tick: render frame 0 first */
        else pl->anim_frame++;
        if (s_hit_kb > 0) {
            int32_t ox = pl->x, oz = pl->z, dx, dz;
            re15_player_knockback_delta(pl->rot_y, s_hit_kb, &dx, &dz);    /* backward push @ facing+0x800 */
            int32_t nx = pl->x + dx, nz = pl->z + dz;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(c->rdt, ox, oz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            pl->x = nx; pl->z = nz;
            s_hit_kb -= 0x32;                                              /* DAT_800acaf2 = 50 */
            if (s_hit_kb < 0) s_hit_kb = 0;
        }
        /* Exit: das Original kippt NUR das cmd-Wort auf 1 (`sw 1` @0x80035c80/@0x80035db8 —
         * die Exits der cmd-2-Substate-Handler [0]-[3]); Clip +0x94 bleibt UNANGETASTET, der
         * Exit-Frame zeigt die letzte Flinch-Pose, erst der cmd-1-Tick des FOLGEframes waehlt
         * neu. Das alte `motion = 0` blitzte hier 1 Tick PL00-Base-Clip 0 (gemessen,
         * probe_hitdoor_entry_anim t=19). */
        --s_hit_flinch;
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else {
        /* NORMAL cmd-0 handler prologue (byte-true LAB_800318f8/FUN_80031c44): the original
         * UNCONDITIONALLY re-arms the player hit gate every frame the player is back in NORMAL —
         * @0x80031964 `sb zero,DAT_800acae7` (player+0x93) — and clears the knockdown latch bit —
         * @0x80031c44 decompile: `DAT_800aca52 = DAT_800aca52 & 0xfffe`. Without the +0x93 re-arm
         * ONE enemy hit (crow dive etc.) latched hit_react and blocked ALL later contact damage
         * AND re15_player_take_damage (re15_damage.c gate) forever. The latch still holds through
         * the flinch/grab/death branches above (they run INSTEAD of this one), exactly like the
         * original's non-normal command states. (audit wf_827f186d crow #2, HIGH) */
        pl->hit_react = 0;                       /* @0x80031964 */
        g_aca52_flags = (uint16_t)(g_aca52_flags & 0xfffe);   /* @0x80031c44 */
        /* GAMEPLAY-AUTO-LOOK (byte-true State-1-Prolog @0x80031de8-40, cutscene_headlook.md
         * B6 CONFIRMED): jeden Normal-Tick `flags |= 0x12` (@0x80031e04-08 = Idle-Release);
         * dann — ausser im Substate 7 (@0x80031e10; Port-Entsprechung: Aim aktiv) —
         * FUN_8003703c(a0=0xfa0=4000) (@0x80031e20-24): naechstes "lookable" SPL-Entity in
         * 4000 Units; Treffer -> Ziel nach player+0x1a8 + `flags &= ~0x12` (@0x80031e3c-40)
         * = ENTITY-TRACKING (Leons Kopf folgt dem naechsten NPC). Kategorien-Detail der
         * +0x9a-Vorzeichen-Bits ist OFFEN (O1) — Port konservativ: NPC-Typen 0x40-0x4f mit
         * hp<0 (der NPC-INIT-"lookable"-Marker `sh -1 -> +0x9a` @0x8011c748). Laeuft NUR im
         * Normal-Branch = dem cmd-0/1-Pfad des Originals (Cutscene-Kommandos laufen andere
         * cmds und behalten ihre Script-Necks). */
        if (g_scd.player_mode != 2) {
            extern int re15_player_aim_active(void);   /* player_common.c */
            pl->neck_flags = (uint8_t)(pl->neck_flags | 0x12);
            if (!re15_player_aim_active()) {
                int  best = -1; int32_t best_d2 = 4000 * 4000;
                for (int i = 1; i < RE15_ACTOR_MAX; i++) {
                    const re15_actor_t *n = &g_actors[i];
                    if (!n->active || n->hp >= 0) continue;
                    if (n->type < 0x40 || n->type > 0x4f) continue;
                    int32_t dx = n->x - pl->x, dz = n->z - pl->z;
                    int32_t d2 = dx * dx + dz * dz;
                    if (d2 < best_d2) { best_d2 = d2; best = i; }
                }
                if (best >= 0) {
                    pl->neck_target_slot = (int8_t)best;
                    pl->neck_flags = (uint8_t)(pl->neck_flags & ~0x12);
                }
            }
        }
        int32_t ox = pl->x, oz = pl->z;
        re15_player_tick(c->cam_view, c->pad_current);
        /* ENTITY BODY COLLISION (byte-true FUN_80031c44 order: cmd-FSM move -> FUN_8002b544 body
         * push-out -> FUN_8003b0a4 walls, so the WALLS win): push the player out of every live
         * enemy cylinder (450 + 400) — the "walk through zombies" fix. */
        re15_body_push_player();
        if (c->rdt_ok) {
            /* Room SCA collision then object collision: push the player out of
             * his band's perimeter wall cells, then out of solid Obj_model_set
             * props (helipad BOX). Walls first, then objects. */
            int32_t nx = pl->x, nz = pl->z;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(c->rdt, ox, oz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            pl->x = nx;
            pl->z = nz;
        }
        /* PLAYER WEAPON FIRE — the byte-true GUN-FSM discharge (cmd1/ACTION 7, item dispatch
         * @0x80074030 -> gun FSM 0x80032e9c, sub-table @0x800740f4). Hold R1 to AIM (re15_player_tick
         * runs RAISE clip 6 -> HOLD clip 8/10/12); FIRE = SQUARE **HELD** in HOLD (@0x80033308,
         * NOT an edge — the recoil clip gates the auto-refire cadence: discharge -> anim end ->
         * HOLD -> refire). The handgun (ITEM 3) one-shot @0x800337bc:
         *   FUN_80019700(0x02000800, yaw, posebuf+0x7a4, {0x8c,0x25d,0})   = MUZZLE FLASH (fx-id 2)
         *   FUN_80019700(0x03000c00, yaw, posebuf+0x7a4, {0x91,0x1f4,-25}) = SMOKE       (fx-id 3)
         *   FUN_80019700(0x04000800, yaw, posebuf+0x7a4, {0x91,0x109,-50}) = SHELL EJECT (fx-id 4)
         *   FUN_80011f50 (damage resolve) + FUN_8004eae4 (ammo-1).
         * The anchor posebuf+0x7a4 = the GUN-BONE matrix; the port has no engine-side pose buffer,
         * so the spawns anchor at the actor + the MEASURED aim-pose hand-bone height (pose-dump
         * shots/pose_aim.txt.leon b13 y=-2083) + the cited local offsets along facing (faithful-
         * line position, byte-true ids/offsets). There is NO direct shot SE in the discharge path —
         * the bang is driven by the spawned ESP effect's data; the port's fx tick NOW drives it: ROW VM routine 9 (re15_esp.c:446)
         * -> re15_esp_bang_hook -> pc_bang (audio_pc.c:684) = ARMS record 0 (the old
         * re15_audio_weapon_se(8) stand-in was the WRONG record;
         * RE15_COMBAT_SE_SUBSYSTEM.md §3). Aiming suppresses g_aot_action_pressed (no door/stair
         * while the weapon is raised). 1170-SAFE: needs R1+Square input; hits only live zombies. */
        if (c->rdt_ok && (c->pad_current & RE15_PAD_BIT_R1)) {
            /* THE HOLD FIRE GATE (@0x80033300-84, byte-true): Square HELD + FUN_8004ea6c
             * (mag>0) -> DISCHARGE. Mag EMPTY: only on the Square PRESS-EDGE (@0x80033338
             * lw 0x800ac76c) — held-but-not-newly-pressed does NOTHING — either auto-RELOAD
             * (FUN_8004eb70 reserve present AND item<9 @0x80033368 sltiu) or the empty-click
             * SE 0x01010001 (@0x80033384-90). Melee items (0-2) have no ammo model. */
            int eq_item = re15_player_equipped_weapon();   /* DAT_800aca5d; byte-true start = 1 KNIFE */
            if ((c->pad_current & RE15_PAD_BIT_SQUARE) && re15_player_aim_ready() &&
                eq_item >= 3 && !re15_ammo_mag_nonzero()) {
                if (c->pad_pressed & RE15_PAD_BIT_SQUARE) {         /* press-EDGE only */
                    extern void re15_player_reload_start(void);
                    if (re15_ammo_reserve_slot() > 0 && eq_item < 9)
                        re15_player_reload_start();                 /* sub=4 @0x80033378 */
                    else
                        re15_audio_weapon_se(1);                    /* click 0x01010001 */
                }
            }
            else if ((c->pad_current & RE15_PAD_BIT_SQUARE) && re15_player_aim_ready()) {
                extern void re15_player_fire_start(void);
                re15_player_fire_start();                 /* gun: recoil 7/9/11; melee: SLASH 7/9/11
                                                           * (swing SE inside; damage runs per-tick
                                                           * in the frame-6..11 window below) */
                if (eq_item >= 3) {                       /* GUN-only discharge side (@0x800337bc):
                                                           * the melee items 0-2 have no muzzle/shell */
                    re15_player_weapon_fire(eq_item);     /* FUN_80011f50 resolve (per-item dmg/reach) */
                    re15_ammo_consume();                  /* FUN_8004eae4 @0x80033888 (after damage,
                                                           * return unchecked for the handgun) */
                    /* discharge fx (byte-true ids 2/3/4 from CORE00.ESP; anchor faithful-line).
                     * The MUZZLE runs the ROW VM (stage 3b): st0 R8 (show + chain the 0x02040bb8
                     * secondary flash) -> R9 (the positional BANG, ARMS record 0, on the slot's
                     * 2nd tick — DATA-driven now; the s_bang_delay scheduler is gone) -> hold;
                     * st1 R10 (show + anim). */
                    /* DISCHARGE-FX ANCHOR — byte-true gun-bone matrix (FUN_80019700 3rd arg posebuf+0x7a4,
                     * world = R_gunbone*offset + T_gunbone per FUN_80019e20). re15_player_gunbone_world
                     * feeds the render's bone-11 world R+T; the 3 local offsets are the exact
                     * @0x800337e0/@0x80033818/@0x8003383c triples {0x8c,0x25d,0}/{0x91,0x1f4,-25}/
                     * {0x91,0x109,-50}. Faithful-line fallback (player centre + facing) only until the
                     * first frame has posed bone 11. */
                    extern int re15_player_gunbone_world(int32_t ox, int32_t oy, int32_t oz, int32_t out[3]);
                    int32_t fcos = re15_cos_q12((int)pl->rot_y);
                    int32_t fsin = re15_sin_q12((int)pl->rot_y);
                    int32_t gy   = pl->y - 2083;          /* fallback aim hand-bone height (b13) */
                    int32_t gp[3];
                    if (re15_player_gunbone_world(0x8c, 0x25d, 0, gp))    /* MUZZLE 0x02000800 {0x8c,0x25d,0} */
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 2, 0, 0x0800, gp[0], gp[1], gp[2], pl->y);
                    else
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 2, 0, 0x0800,
                            pl->x + ( fcos * 0x25d >> 12), gy, pl->z + (-fsin * 0x25d >> 12), gy + 2083);
                    if (re15_player_gunbone_world(0x91, 0x1f4, -25, gp)) /* SMOKE 0x03000c00 {0x91,0x1f4,-25} */
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 3, 0, 0x0c00, gp[0], gp[1], gp[2], pl->y);
                    else
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 3, 0, 0x0c00,
                            pl->x + ( fcos * 0x1f4 >> 12), gy - 25, pl->z + (-fsin * 0x1f4 >> 12), gy + 2083);
                    /* SHELL EJECT (byte-true @0x8003383c-64 of the handgun one-shot @0x800337bc =
                     * item-dispatch [6]/[7]): id 4 sub 0 scale 0x800 = 0x04000800, spawned INLINE at
                     * discharge alongside muzzle+smoke (offset {0x91,0x109,-50}) — the SAME handler the
                     * muzzle 0x02000800 + smoke 0x03000c00 above come from. The old code deferred a
                     * 0x040d1000 (id 4 sub 0xd) to a recoil-frame watcher, but that 0x040d1000-at-recoil
                     * is a DIFFERENT weapon handler (@0x80033680), never the handgun's — a mis-port. */
                    /* ROW-VM driven (stage 3, trace wf_a18487d9): R16 2-tick eject hold ->
                     * R11 RNG spread on the row seed (-35,-50,-140) -> gravity (0,10,0) +
                     * B=12 floor bounce (clink SE; kill on the 2nd contact). floor = gy. */
                    if (re15_player_gunbone_world(0x91, 0x109, -50, gp))  /* SHELL 0x04000800 {0x91,0x109,-50} */
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 4, 0, 0x0800, gp[0], gp[1], gp[2], pl->y);
                    else
                        re15_esp_fx_spawn_rows(re15_esp_global_bank(), 4, 0, 0x0800,
                            pl->x + ( fcos * 0x109 >> 12), gy - 50, pl->z + (-fsin * 0x109 >> 12), gy);
                }
            }
            g_aot_action_pressed = 0;         /* aiming blocks the door/stair action (no doors while aiming) */
        }
        /* GUNSHOT BANG (byte-true wf_efa45868-e53): the muzzle effect's 40-byte descriptor ROW
         * script fires it on the slot's SECOND tick — row1 u16[0]=9 -> routine 9 @0x80017654 ->
         * FUN_80045024(0x01000001, &slot_worldpos) = ARMS bank record 0, positional. The port's
         * fx layer NOW drives this from the ESP ROW VM: routine 9 @re15_esp.c:446 (rows loaded by
         * esp_fx_row_load @341, spawned by re15_esp_fx_spawn_rows @491) fires the bang on the slot's
         * 2nd tick; the old 1-tick scheduler is deleted. (Routine 9 also writes the noise global 0x800b5358=1 — consumer un-RE'd,
         * deferred.) The old immediate re15_audio_weapon_se(8) was the WRONG record (8 = the
         * knife-draw/flesh-hit SE). */
        /* (The BANG + the secondary flash are DATA-driven by the muzzle slot's rows now
         * — routine 9/8 in the row VM; the 1-tick scheduler block is deleted.) */
        /* (The shell casing is now ejected INLINE at discharge — see the one-shot above — matching
         * the handgun handler @0x800337bc; the old 0x040d1000-at-recoil watcher was a mis-ported
         * different-weapon effect and has been removed.) */
        /* MELEE SLASH DAMAGE WINDOW (byte-true @0x80035388-cc): while the slash clip's anim frame
         * is in [6..11], the resolver runs EVERY tick — the once-per-target latch + recursion
         * inside re15_player_weapon_fire keep it one-damage-per-target (and allow a second victim).
         * The original passes the BLADE-TIP world point ([0x800acbdc]+0x7b8); the port measures
         * from the player centre (faithful-line, OPEN O1 — slightly shorter effective range). */
        {
            extern int re15_player_slash_window(void);
            if (re15_player_slash_window())
                re15_player_weapon_fire(re15_player_equipped_weapon());
        }
        /* RE15_STAIR_DEMO=down|up: once, at a fixed frame, drop Leon INTO ROOM1170's
         * {2,4} staircase on the matching band and fire the action, so the stair
         * animation can be captured without the fragile long-range navigation.
         * (down = band 4 zone slot 10; up = band 2 zone slot 9; player_in_zone fires
         * the trigger regardless of facing.) Env-gated debug only. */
        {
            /* Typ MUSS int sein wie in re15_skeleton.h:39-40 (oben eingebunden) —
             * NICHT int32_t: auf dem PC sind beide identisch, auf MIPS ist
             * int32_t ein `long int` und die lokale Deklaration kollidiert dann
             * mit dem Header (PSX-Build-Blocker, siehe targets/psx/README.md). */
            extern int re15_sin_q12(int), re15_cos_q12(int);
            const char *dm = getenv("RE15_STAIR_DEMO");
            static int s_demo_done = 0;
            if (dm && *dm && !s_demo_done && !re15_stair_active()
                && (int)g_engine.frame_count >= 1500) {
                s_demo_done = 1;
                if (dm[0] == 'u' || dm[0] == 'U') {          /* ASCEND 2->4 */
                    pl->x = -23295; pl->z = -26155; pl->y = -3600; pl->rot_y = 3959;
                } else {                                     /* DESCEND 4->2 */
                    pl->x = -20690; pl->z = -25595; pl->y = -7200; pl->rot_y = 1911;
                }
                re15_collision_set_band(re15_collision_band_from_y(pl->y));
                g_scd.player_mode = 0;                       /* leave the intro cutscene gate (byte-true try_start bails at mode 2) */
                g_aot_action_pressed = 1;                    /* fire the stair this frame */
                fprintf(stderr, "[stairdemo] F%u placed %s at (%d,%d,%d) band=%d player_mode=%d\n",
                        (unsigned)g_engine.frame_count, dm, pl->x, pl->y, pl->z,
                        re15_collision_band_from_y(pl->y), (int)g_scd.player_mode);
            }
        }
        /* A stair may START this frame: ACTION pressed while in/against a stair
         * zone. If so it consumes the action and we SKIP the door scan;
         * otherwise scan the door AOTs (also action-gated). */
        if (!(c->rdt_ok && re15_stair_try_start(c->rdt, g_aot_action_pressed))) {
            re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
        }
    }

    /* FOOTSTEP SE (byte-true LAB_80030af0 walk / LAB_80030d28 run): while the player
     * walks(105)/runs(100), the W01 clip's CURRENT frame carries a foot-plant flag
     * (bit 0x4000) → fire one footstep, its material from floor.flr
     * (re15_rdt_floor_sound at the player XZ). Edge-gate on the anim frame so the
     * same foot-plant isn't double-fired (PC halves anim_frame at 60fps). */
    {
        static uint32_t s_last_foot_cur = 0xFFFFFFFFu;
        if (c->rdt_ok && c->w01_anim && (pl->motion == 105 || pl->motion == 100)) {
            uint32_t cur = (uint32_t)pl->anim_frame;
            if (cur != s_last_foot_cur) {
                s_last_foot_cur = cur;
                int clip = (pl->motion == 105) ? 5 : 0;   /* walk=W01 c5, run=c0 */
                int foot = re15_actor_footstep(c->w01_anim, pl, clip, cur);
                if (foot) re15_audio_footstep(foot, re15_rdt_floor_sound(c->rdt, pl->x, pl->z));
            }
        } else {
            s_last_foot_cur = 0xFFFFFFFFu;   /* reset when not locomoting */
        }
    }

    /* Same-room SCENARIO re-entry: a SELF-room door (dest == current room) queued
     * g_scd_pending_scenario = its target_cut during the scan above. Consume it
     * HERE — same frame, immediately after the scan (the canonical PSX order; PC
     * used to consume it a frame early at the top of the loop, which drifted) — to
     * re-run main00+sub00 with locals[10]=scenario → sub00's sub15 dispatch:
     *   door 0 (cut 11) → sub14 (outdoor courtyard dialog cutscene);
     *   door 6 (cut 0, courtyard→helipad return) → sub15 else = the 7 crows
     *     (the re-entry memset already despawned Elliot + heli).
     * No asset reload (same room); game flags persist; Leon is at the door spawn. */
    if (g_scd_pending_scenario >= 0 && c->rdt_ok) {
        uint8_t sc = (uint8_t)g_scd_pending_scenario;
        /* The door fire queued a camera change (g_scd.cam_id = target_cut,
         * cam_change_pending = 1) during the scan — but scd_room_reenter's
         * memset(&g_scd) is about to WIPE it. Capture the door's entry cut now. */
        uint8_t entry_cut = g_scd.cam_id;
        g_scd_pending_scenario = -1;
        scd_room_reenter(c->rdt, pl->x, pl->z, sc);
        /* Latch the byte-true intro handoff (ROOM1170 sub11 Aot_on(3) → door 3 → this reenter).
         * The intro's single Aot_on(3) is the ONLY reenter the original performs; the PC
         * hand-deferred fallback (main.c) checks this so it can no longer fire a redundant
         * second reenter that would spawn the intro crows. */
        g_scd_self_reenter_fired = 1;
        /* Door entry = GAMEPLAY → enable the RVD/CAM_SWITCH auto-camera so it FOLLOWS
         * the player between the section's camera cuts. scd_room_reenter's memset
         * cleared cut_auto_enabled, and a gameplay scenario (e.g. door 6 → sub15 crows)
         * never issues Cut_auto(1) — so without this the camera stays frozen on the
         * entry cut and "man läuft außerhalb des Kamera Views". The cross-room door
         * (re15_room_apply_pending) already does this; self-doors must too. Camera-debug
         * autopilot proved cut_auto=0 → no follow, cut_auto=1 → follows (11→10).
         * A cutscene scenario (sub14) re-issues its own Cut_auto/Cut_chg, so this is
         * harmless there (RVD is byte-true ALSO live during the cinematic). */
        g_scd.cut_auto_enabled = 1;
        /* Re-apply the door's entry cut so the camera FRAMES the teleported player
         * (the port's camera block applies this next frame: build view + load BG).
         * Only if the scenario's own SCD didn't already queue a Cut_chg this tick —
         * then respect that. Without this, a self-door whose scenario issues NO
         * Cut_chg (door 6 → sub15 ELSE = the crows) leaves the camera on the OLD
         * courtyard cut while the player teleports to the helipad → player drawn
         * off-screen ("Leon verschwindet"). The scenario's later Cut_chg (e.g. door
         * 0 → sub14 → Cut_chg 12) still overrides on its own tick. */
        if (!g_scd.cam_change_pending) {
            g_scd.cam_id             = entry_cut;
            g_scd.cam_change_pending = 1;
        }
    }

    /* Dispatch any AOT event fired this frame to its SCD handler (the handler
     * may set cam_change_pending (Cut_chg), enqueue audio (Se_on), or arm a
     * subtitle (Message_on) — each consumed downstream by the port). A full
     * event-slot pool drops the event (scd_event_fire returns <0, same as the
     * original's free-slot scan) — DIAGNOSE it loudly instead of silently: a
     * dropped examine (e.g. the save phone) looks like a dead button. */
    if (g_aot.fired_event_id_this_frame != 0) {
        if (scd_event_fire(g_aot.fired_event_id_this_frame) < 0) {
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[scd] WARN: event %u DROPPED (no free event slot)\n",
                    (unsigned)g_aot.fired_event_id_this_frame);
#endif
        }
    }

    /* ===== Phase 8.6 — the LIVE STAGE1 zombie AI pass ==================================
     * Byte-true to the original per-frame entity-update loop FUN_8001a50c (@0x8001ce04 in the
     * main update): it walks the entity array and dispatches @0x80072bac[type] per active entity.
     * re15_enemy_ai_run_all is the port's TYPE-GATED slice of that loop — it ticks ONLY the live
     * zombie types (0x10/0x11/0x16) through their handler (FUN_80100424 tick + the shared lunge
     * slice); every other type (Elliot 0x47, crows 0x21, the player, props) keeps its existing
     * path. That gate is what makes this 1170-SAFE: the ROOM1170 helipad + the ROOM1240 boot room
     * spawn no live zombie, so this is a verified no-op there (no intro/cinematic regression).
     *
     * Placement: at the END of the step, AFTER the player move/collision + AOT scan have settled
     * the frame. The original runs the AI-tick half (FUN_8001a50c) just BEFORE the AOT scan and
     * the lunge-EXECUTION half (the action driver FUN_80019e20 @0x8001ce2c) just AFTER it; the
     * port folds tick+execution into run_all, and placing it here lands the lunge hitbox after the
     * AOT scan (faithful to FUN_80019e20's slot) against the player's final XZ this frame.
     *
     * combat_active = g_scd.combat_active = the byte-true DAT_800aca3c & 1 latch the attack-arm
     * (FUN_8010ab2c) gates on; held, cleared on room load (see re15_scd.h). The briefing zombies
     * spawn in the feeding/lying sub-modes (grid_id & 0xf != 0), so the combat decision brain is
     * not even entered yet — they tick (INIT->ACTIVE) but do not attack until the byte-true
     * feeding->combat wake-up handler runs (re15_enemy_ai_live_feeding, enemy_ai_common.c:970). Verified in a real room by test_room1140_combat. */
    /* NPC ANIMATION ADVANCE — byte-true independent of the player command FSM (FUN_8001a50c per-type
     * handler, NOT gated by @0x80073f90). Runs UNCONDITIONALLY here, so it fires in the grabbed/dead/
     * stair/menu branches too (it used to sit inside re15_player_tick, which those branches skip →
     * the whole scene froze the instant the player was grabbed = the ROOM1140 "hang"). Placed BEFORE
     * run_all so the AI's clip-end gate (re15_enemy_clip_done) reads the frame this pass advanced. */
    re15_actors_anim_advance();

    /* MASH-ESCAPE feed (byte-true FUN_80037024): the grab's bite loop reads the press-EDGE pad bits
     * (any D-pad/face button & 0xf0f0) to drain its escape window 1 + 5*mash — the classic wiggle-out.
     * Fed every tick (also while the grabbed branch skips re15_player_tick — the pad edge still
     * arrives here), so mashing during the grab breaks Leon free via the THROW-OFF. */
    re15_enemy_ai_set_pad_pressed(c->pad_pressed);
    if (c->rdt_ok)
        re15_enemy_ai_run_all(g_scd.combat_active);

    /* LEON GRAB-VICTIM ANIMATION (state 5 struggle / state 6 collapse) — advance AFTER run_all so the
     * grab (re15_enemy_ai_live_grab) has latched the victim state this frame. Drives Leon's motion/
     * anim_frame off the grabbing zombie's bank 2 so he struggles + collapses instead of freezing
     * (byte-true player-command FSM @0x8010a28c/@0x8010a6f8). No-op when no zombie is grabbing. */
    re15_player_victim_tick();

    /* PLC_DEST-MODE-6 EVENT-REACH auf dem Spieler (byte-true 0x800517f0 via 0x80073e30[6]) —
     * tickt unabhaengig vom Branch, solange das Script den Halt haelt (Exit = naechster
     * Plc-Befehl, kein Timeout; marvin_10d0.md D3). */
    re15_player_event_reach_tick(pl);

    /* Body push WHILE GRABBED (byte-true FUN_80031c44: the cmd-5 victim handler — placement — is
     * followed by FUN_8002b544 body push then the walls in the SAME player tick): a THIRD zombie
     * still pushes the pinned player; the grabbing PAIR itself is exempt inside re15_body_push_player
     * (the both-0x1000 AND, FUN_8002aec4 @0x8002af14). Gated on grabbed_branch so the grab-COMMIT
     * tick (normal branch already pushed inline) does not double-push. Ordered AFTER the victim
     * placement above == the original's placement->push->walls order (walls win: a third zombie
     * cannot shove the pinned player through the SCA perimeter). */
    if (c->rdt_ok && grabbed_branch && re15_player_is_grabbed()) {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int32_t ox = pl->x, oz = pl->z;                   /* the anchored placement = the valid pos */
        re15_body_push_player();
        if (pl->x != ox || pl->z != oz) {                 /* pushed -> wall-resolve the push delta */
            int32_t nx = pl->x, nz = pl->z;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(c->rdt, ox, oz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            pl->x = nx;
            pl->z = nz;
        }
    }

    /* GLIED-1-STEMPEL + OBJEKT-NOTCH ALS LETZTER ZUSTANDS-TICK DES FRAMES. Frame-Position
     * byte-true: der AUTO-Pool-Scan FUN_800436a8 laeuft @0x8001ce1c NACH Gegner-AI
     * (@0x8001ce04) und Spieler (@0x8001ce0c); die VM des NAECHSTEN Frames (@0x8001cdec)
     * liest also immer den Stempel des Vorframe-ENDES. Die beiden Early-Returns oben
     * (Item-Modal, Menue-Freeze) lassen den Stempel wie das Original stehen — dort laeuft
     * auch im Original kein Scan (Task suspendiert). */
    if (c->rdt_ok) {
        re15_aot_stamp_entities();
        re15_object_notch_update();
    }
}

/* SHARED helicopter-rotor spatialization driver — see re15_game_step.h. Was inline
 * in the PSX main loop only; the PC had re15_audio_rotor_update implemented but never
 * called it (rotor never faded). Now both ports call this so the rotor tracks the
 * camera→heli distance + azimuth identically. */
void re15_rotor_drive(const re15_camera_cut_t *active_cut)
{
    if (!active_cut) return;
    for (int pi = 0; pi < (int)g_scd.prop_count; pi++) {
        if (!g_scd.props[pi].active || g_scd.props[pi].obj_id != 2) continue;
        int32_t eye[3] = { active_cut->pos_x, active_cut->pos_y, active_cut->pos_z };
        int32_t tgt[3] = { active_cut->target_x, active_cut->target_y, active_cut->target_z };
        int32_t hp[3]  = { g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z };
        re15_audio_rotor_update(eye, tgt, hp);
        return;
    }
}
