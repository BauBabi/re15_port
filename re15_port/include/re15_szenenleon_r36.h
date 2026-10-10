/* re15_szenenleon_r36.h — Runde 36, Spur J "szenenleon" (Dossier analysis/befunde_runde36/J_szenenleon.md).
 *
 * (1) Plc_dest Modus 0x13 = der SKRIPT-SCHUB (ROOM11B0 sub06 @0x0170E/@0x0171A, ROOM1171 @0x01566/@0x01572):
 *     Executor-Sub @0x80052864 (Spieler-Tabelle 0x80073e30[19] UND NPC-Tabelle 0x80076ca0[19]) samt dem
 *     Objekt-Bit 0x200 (FUN_8002bd44 @0x8002bec0 -> FUN_8002cabc(Spieler, Objekt, 1) @0x8002bed4).
 * (2) ROOM11B0: Erweiterung der Original-Szene sub06 (Marvin schliesst die Werkstatt-Tuer ab) — siehe unten.
 * (4) ROOM1150: Irons' Kopf folgt nach dem Tod niemandem mehr (Kopf-FSM-Flags 0x12, Form @0x80042618).
 */
#ifndef RE15_SZENENLEON_R36_H
#define RE15_SZENENLEON_R36_H

#include <stdint.h>
#include "re15_actor.h"

/* ---- Schub-Sub @0x80052864 (Plc_dest Modus 0x13) ---------------------------------------------------- */
#define RE15_SZL_MODUS_SCHUB     0x13   /* Plc_dest pc[2]; 0x80073e30[19] = 0x80076ca0[19] = 0x80052864     */
#define RE15_SZL_CLIP_DREH       5      /* +0x94 = 5 @0x800528bc/c0, Kanal +0x170/+0x174 (anim_set @0x8005294c) */
#define RE15_SZL_CLIP_ANSATZ     0x11   /* +0x94 = 0x11 @0x800529cc/d0, Kanal +0x84/+0x16c (anim_set @0x80052a08) */
#define RE15_SZL_CLIP_SCHUB      0x12   /* +0x94 = 0x12 @0x80052a8c/90, Kanal +0x84/+0x16c (anim_set @0x80052af4) */
#define RE15_SZL_BILD_SE         1      /* +0x95 == 1 @0x80052ac0-c8 -> FUN_80045024(0x02070000)            */
#define RE15_SZL_SE_SCHUB        7      /* 0x02070000: Bank 2 (snd0) Satz 7, @0x80052acc / @0x80052bc8      */
#define RE15_SZL_BIT_WURZEL      0x2000u/* Bild-Wort & 0x2000 @0x80052b18 -> FUN_800369f8(0,0) @0x80052b24  */
#define RE15_SZL_OBJ_SCHUB       0x200u /* Objekt-Wort 0 & 0x200 @0x8002bec0                                 */
#define RE15_SZL_MEMBER_FLAGS    6      /* Member_set Id 6 -> `sw a2,0(a0)` @0x800411c8 (Objekt-Wort 0)      */

/* Spieler: Start aus op_plc_dest (Modus 0x13), Ende ueber re15_player_event_reach_end (jeder neue
 * Plc_dest/Plc_motion/Plc_ret ersetzt +0x5 wie im Original @0x80041c18 / @0x80041bc4 / @0x80041f94). */
void re15_szl_schub_spieler_start(void);
void re15_szl_schub_spieler_ende(void);
void re15_szl_schub_spieler_tick(void);   /* NACH re15_player_tick (Spieler-FSM @0x8001ce0c), VOR dem Objekt-Pass */
int  re15_szl_schub_spieler_phase(void);  /* -1 = aus, sonst +0x6 (0..7) */
/* Render-Auskunft Spieler: >=0 = Clip der COMMON-Bank PL00 (Phasen 2..6), sonst -1. */
int  re15_szl_schub_spieler_pl00(void);
/* Render-Auskunft Spieler: >=0 = Clip des +0x170-Kanals (W-Bank-Paar B, Phasen 0/1), sonst -1. */
int  re15_szl_schub_spieler_wclip(void);

/* NPC: Executor-Sub 0x13 (re15_npc_sub_dispatch), Kanal-Auskunft fuer Uhr und Render:
 * 1 = Loco-Paar +0x84/+0x16c (Phasen 2..6), 2 = eigene Bank +0x170/+0x174 (Phasen 0/1), 0 = nicht Sub 0x13. */
void re15_szl_schub_npc(re15_actor_t *e);
int  re15_szl_schub_npc_kanal(const re15_actor_t *e);

/* Objekt-Pass: 1 = Objekt mit Bit 0x200 ist hier erledigt (Schub durch den Spieler, @0x8002bec0..e4). */
int  re15_szl_objekt_schub(int prop_idx);

/* ---- Punkt 4: Irons ---------------------------------------------------------------------------------- */
#define RE15_SZL_HALS_FREI       0x12   /* +0x1b8 = 0x12: `ori v0,zero,0x12` @0x80042614, `sb v0,440(s0)`
                                         * @0x80042618 (Sce_em_set-Ausnahme +0x9&0x40) = Plc_neck Modus 0
                                         * (caseD @0x80041ed8): Ziel = Keyframe (0x02) + Pitch 0 (0x10) */

/* ---- Takt (game_step_common.c, neben re15_cut10f0_tick) ---------------------------------------------- */
void re15_szl_tick(void);

/* ---- Punkt 2: ROOM11B0 — Marvin schliesst die Werkstatt-Tuer ab (Dossier §2) --------------------------
 * Programm: gen/szl11b0_szene.inc (tools/r36_j/szene11b0_bauen.py, Herleitung je Opcode dort).
 * Die "Werkstatt-Tuer" ist die Durchgangszelle der Trennwand, SCA Region 3 Index 12 @0x00EC8
 * (x -22150..-21410, z -17230..-15230, u0=ff fl=03); Leon sub00 @0x0118C/@0x01190 oeffnet sie bei Strom.
 * Elzas Text-Platz: ROOM11B1 sub00 @0x0112A (Platz 8, Rechteck (-22400,-17200,1200,1700), msg 12 @0x179F). */
#define RE15_SZL_RAUM            0x11B0u /* Leon; ROOM11B1 (Elza) bleibt Original                               */
#define RE15_SZL_SUB             6       /* sub06 = die Original-Szene (sub01 @0x011D2 Evt_exec 06)            */
#define RE15_SZL_SUB_START       0x01478 /* Datei-Offset von sub06 (scd_dump_room.py)                          */
#define RE15_SZL_SUB_AOT_ON      0x017EA /* `47 05` Aot_on 5 (Gully) — HIER haengt die Erweiterung davor      */
#define RE15_SZL_BANK_ZU         9       /* VERTRAG Runde 36 §1.1: Bit 84 = Werkstatt-Tuer verschlossen (J)    */
#define RE15_SZL_BIT_ZU          84
#define RE15_SZL_EREIGNIS_ERW    22      /* VERTRAG §1.3: J = 22, 23 in ROOM11B0                               */
#define RE15_SZL_EREIGNIS_TUER   23
#define RE15_SZL_MSG_HOLD        20      /* VERTRAG §1.2: ROOM11B0 J 20..27                                    */
#define RE15_SZL_MSG_FOLLOW      21
#define RE15_SZL_MSG_ZU          22      /* = ROOM11B1 msg 12 @0x179F "The door is locked." (Bytes 1:1)        */
#define RE15_SZL_ELZA_RAUM       0x11B1u
#define RE15_SZL_ELZA_MSG_ZU     12
#define RE15_SZL_SE_BANK         0x3A    /* Port-Bank (Se_on pc[1]) der RE2-Schlossbank SCHLOSS1010 — die
                                          * Bank-Weiche FUN_80045024 kennt nur 0..5 (audio_pc.c Haken davor)   */
#define RE15_SZL_SE_SCHLOSS      0x0A    /* RE2 ROOM1010 sub09 @0x01C20 `36 02 0a 01` -> EDT Satz 0x0A
                                          * @0x28 `00 00 83 01` = Prog 0 Ton 8 -> VAG 4                         */
#define RE15_SZL_MARVIN_SLOT     1       /* Aktor-Slot = 1 + Sce_em_set-Slot 0 (main00 @0x01080 `44 00 40`)    */

/* Raumaufbau (scd_room_setup.c, Ende des Installer-Blocks): Texte 20..22 in ROOM11B0; mit (9,84) zusaetzlich
 * Ereignis 23 = Tuer zu (Zelle solid + Text-Platz 8). */
void re15_szl_install(uint16_t room_id);
/* scd_event_fire: Programm fuer Ereignis 22/23 in ROOM11B0, sonst NULL. */
const uint8_t *re15_szl_ereignis(uint16_t room_id, uint8_t event_id);
/* op_aot_on: steht der Faden auf sub06 @0x017EA und ist (9,84)=0, springt er in die Erweiterung
 * (Rueckgabe 1); deren Schwanz ist dasselbe `47 05 01 00`. sub06_start = rdt->sub_scd[6]. */
int re15_szl_aot_on_umleitung(const uint8_t **pc, const uint8_t *sub06_start);
/* lock_se_common.c: (0x11B0, 22) spielt wie Elzas (0x11B1, 12) — 1 = umgeschrieben. */
int re15_szl_lock_stelle(unsigned *room_id, uint8_t *msg_id);
/* Riegel/Mess-Zugriff */
const uint8_t *re15_szl_programm(int *out_len);
const uint8_t *re15_szl_tuerprogramm(int *out_len);
const uint8_t *re15_szl_meldung(int msg_id, int *out_len);
int re15_szl_programm_laeuft(void);

#endif
