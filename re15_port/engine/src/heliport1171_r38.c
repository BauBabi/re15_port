/*
 * heliport1171_r38.c — Runde 38 Spur I: Elzas Heliport-Szene ROOM1171 (Kraehen-Rueckgriff, Wurf).
 * Belege und Port-Wahl: include/re15_heliport1171_r38.h, Dossier analysis/befunde_runde38/I_heliport1171.md.
 */
#include "re15_heliport1171_r38.h"
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_scd.h"
#include "re15_audio.h"
#include <stdio.h>

extern int  re15_player_victim_state(void);           /* enemy_ai_common.c */
extern void re15_player_event_reach_begin(void);      /* game_step_common.c: Modus 6 0x800517f0 */
extern void re15_player_event_reach_end(void);
extern void re15_player_aim_interrupt(void);          /* player_common.c */

/* Port-Abbild des Spieler-Kommandos fuer DIESEN Griff: aca58 == 5 && aca59 == 1. */
static int s_aktiv  = 0;
static int s_phase  = 0;   /* aca5a */
static int s_clip   = 0;   /* acae8 */
static int s_bild   = 0;   /* acae9 */
static int s_griffe = 0;
static int s_wuerfe = 0;

int re15_r38i_ereigniskraehe(const re15_actor_t *e)
{
    return e && e->type == RE15_R38I_KRAEHE_TYP && (e->grid_id & RE15_R38I_EREIGNIS_BIT);
}

static int clip_laenge(int clip)
{
    /* Paar C der Kraehe (acbcc/acbd0 = Kraehe+0x178/+0x17c @0x80115290/@0x801152b4); die Bank liefert die
     * Laengen, sonst die eingebetteten Werte derselben Datei (s. Header). */
    const re15_enemy_bank_t *vb = re15_enemy_find((uint8_t)RE15_R38I_KRAEHE_TYP);
    if (vb && vb->victim_ok && clip < vb->anim_victim.clip_count && vb->anim_victim.clips[clip].frame_count > 0)
        return vb->anim_victim.clips[clip].frame_count;
    return (clip == RE15_R38I_CLIP_INTRO) ? RE15_R38I_LEN_INTRO
         : (clip == RE15_R38I_CLIP_HALTEN) ? RE15_R38I_LEN_HALTEN : RE15_R38I_LEN_WURF;
}

void re15_r38i_rueckgriff(re15_actor_t *kraehe, re15_actor_t *pl)
{
    (void)kraehe;
    /* @0x80115264-70: `lbu v1,0x800aca58` / `beq v1,v0(=5)` -> laeuft schon ein Griff, nur ORIENT + Bit.
     * Port: dieser Griff selbst oder jede andere Opfer-Maschine (Kommando 5/6 der Greifer). */
    if (s_aktiv || re15_player_victim_state() != 0) return;
    s_aktiv = 1;
    s_phase = 0;                                       /* aca5a = 0 @0x80115288 */
    s_griffe++;
    /* aca58 = 5 @0x801152a0 ERSETZT das laufende Kommando: Ziel-Aktion, Plc-Gang und Modus-6-Halt enden
     * (Dispatch @0x80031c88 indexiert aca58 - derselbe Schritt wie re15_player_victim_latch_ex). */
    re15_player_aim_interrupt();
    re15_player_event_reach_end();
    if (pl) pl->walk_active = 0;
}

void re15_r38i_wurfstoss(void)
{
    /* @0x801120a0-e8: TEST(0x800b1028,0x1d) && aca58==5 && aca5a<3 -> `sb 3,aca5a` @0x801120e8.
     * aca58==5 mit (5,0x1D) gibt es game-weit nur hier: (5,0x1D) setzen nur ROOM1171 sub02 @0x01404 und der
     * Feuer-Emitter 0x26 in ROOM1090 (@0x80116ac8, dort keine Kraehe, also keine Kraehen-Wurzel). */
    if (!s_aktiv) return;
    if (!re15_game_flag_get(RE15_R38I_ZBANK, RE15_R38I_BIT_WURF)) return;
    if (s_phase < RE15_R38I_PHASE_WURF) s_phase = RE15_R38I_PHASE_WURF;
}

void re15_r38i_rueckgriff_tick(void)
{
    if (!s_aktiv) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Ein Raumwechsel/Reset hat die Opfer-Maschine geloescht (re15_player_victim_reset) -> Kommando weg. */
    if (s_phase > 0 && re15_player_victim_state() != 4) { s_aktiv = 0; return; }
    int vorschub = 1;   /* 0 im Bild von P0/P3: f314 zeigt Bild 0 (Port-Takt wie re15_player_victim_tick) */
    switch (s_phase) {
    case 0:   /* P0 @0x80115bb8 */
        pl->anim_frac = RE15_R38I_BLEND;               /* +0x8f = 7 @0x80115bd0 */
        s_clip = RE15_R38I_CLIP_INTRO;                 /* @0x80115bec */
        s_bild = 0;                                    /* @0x80115bf4 */
        s_phase = 1;                                   /* aca5a++ @0x80115c00 */
        re15_audio_room_se(RE15_R38I_SE_SATZ);         /* Se_on 0x03020001 @0x80115c04 */
        vorschub = 0;                                  /* faellt in P1: f314 zeigt Bild 0 */
        break;
    case 1:   /* P1 @0x80115c0c: f314 @0x80115c20 */
        if (++s_bild >= clip_laenge(s_clip)) {
            re15_audio_room_se(RE15_R38I_SE_SATZ);     /* @0x80115c3c */
            s_clip = RE15_R38I_CLIP_HALTEN;            /* @0x80115c54 */
            s_bild = 0;                                /* @0x80115c5c */
            s_phase = 2;                               /* aca5a++ @0x80115c68 */
        }
        break;
    case 2:   /* P2 @0x80115c74: f314 @0x80115c88, Halte-Schleife */
        if (++s_bild >= clip_laenge(s_clip)) {
            s_bild = 0;
            re15_audio_room_se(RE15_R38I_SE_SATZ);     /* @0x80115ca4 */
        }
        break;
    case 3:   /* P3 @0x80115cb4 (vom Wurfstoss der Wurzel gesetzt) */
        s_clip = RE15_R38I_CLIP_WURF;                  /* @0x80115cc4 */
        s_bild = 0;                                    /* @0x80115ccc */
        s_phase = 4;                                   /* aca5a++ @0x80115cd8, faellt in P4 */
        vorschub = 0;
        break;
    default:  /* P4 @0x80115cdc: f314 @0x80115cf0; Umlauf -> Kommando 4 / Modus 6 */
        if (++s_bild >= clip_laenge(s_clip)) {
            /* aca58 = 4 @0x80115d04, aca59 = 6 @0x80115d10, aca5a = 0 @0x80115d18: der Spieler-Sub
             * 0x800517f0 uebernimmt (PLW-Clip 1 einmal, Clip 2 als Schleife) bis zum naechsten Plc_dest. */
            s_aktiv = 0;
            s_wuerfe++;
            re15_player_victim_force_end();
            re15_player_event_reach_begin();
            return;
        }
        break;
    }
    /* f314 zieht +0x8f je Aufruf um 1 ab (FUN_8001f3bc); im P0-Bild steht der volle Wert. */
    if (vorschub && pl->anim_frac > 0) pl->anim_frac--;
    /* Spieler aus der Opferbank der Kraehe posieren (Kommando 5 = Pin, kein Pad: re15_player_is_grabbed). */
    re15_player_victim_force((uint8_t)RE15_R38I_KRAEHE_TYP, s_clip, (uint32_t)s_bild);
}

void re15_r38i_install(uint16_t room)
{
    (void)room;
    /* Jeder Raumaufbau loescht das Kommandowort (FUN_800314b0 `sw zero,0x800aca58` @0x80031518). */
    if (s_aktiv && re15_player_victim_state() == 4) re15_player_victim_force_end();
    s_aktiv = 0; s_phase = 0; s_clip = 0; s_bild = 0;
}

int re15_r38i_phase(void)  { return s_aktiv ? s_phase : -1; }
int re15_r38i_griffe(void) { return s_griffe; }
int re15_r38i_wuerfe(void) { return s_wuerfe; }
