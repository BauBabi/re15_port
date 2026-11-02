/*
 * RE1.5 Rebuilt — G5-Auftritt nach dem Rausschiessen der Riesenform (Kopf: include/re15_g5auftritt_r41.h,
 * Dossier analysis/befunde_runde41/G5_auftritt.md, Nachbesserung 1).
 *
 * RE2 ROOM7040 MAIN sub06 ab dem Schnitt auf Kamera 9, Bild fuer Bild (Sleep n = n Bilder, Wsleeping wartet auf 0x800cfbd8 & 0x20
 * @0x80053ac0-d0; Dump `python re15_port/tools/r34n_c/re2_scd_dump.py ROOM7040 MAIN 6`):
 *
 *   VORLAUF (Abnahme 1 M7) - ab Cut_chg(9) @0x01892, vor der Armierung:
 *     @0x0187B `66` Plc_stop, @0x0187C `32 00 28 f1 00 00 ee a3` Pos_set(-3800,0,-23570), @0x01884 `33 00 00 00 00 00 00 00` Dir_set 0,
 *     @0x01892 `29 09`, @0x01898 Evt_next, @0x018A2 `41 02 00 00 38 ff 00 00 c8 64` Plc_neck, @0x018AC Sleep 20,
 *     @0x018B0 `40 00 05 20 78 05 ee a3` Plc_dest(Lauf, Bit 0x20, 1400,-23570), @0x018B8-C3 Do/Evt_next/Edwhile Ck(5,0x20,0),
 *     @0x018C7 `3f 00 0f 08` Plc_motion, @0x018CC `41 02 00 00 00 fe 80 00 64 10` Plc_neck, @0x018D6 Sleep 11
 *   Bild    0  @0x018DA/@0x018DE  `2e 03 00 | 34 17 01 00`  Work_set(Gegner 0), Member_set(0x17,1) = G5 +0x1D4 Bit 0
 *   Bild    1  @0x018EA `3f 00 0f 80` Plc_motion (RE2-Szenenbank +0x190 Clip 15, 12 Bilder, rueckwaerts), @0x018EE `5b 0b` Plc_cnt
 *   Bild   12  @0x01906 `3f 0c 01 04` Plc_motion(0x0C,1,4) = Laufen (Bank +0x180 Clip 1, Schleife) - Leon rennt nach Osten aus dem Bild
 *   Bild   72  @0x01912 `41 02 00 00 00 00 00 00 ff ff` Plc_neck (Kopf frei)
 *   Bild  437  @0x0192A `32 00 64 19 00 00 ee a3`, @0x01932 `33 00 00 00 00 08 00 00`, @0x0193A `29 0a`,
 *              @0x01940 `3f 0b 01 54` Plc_motion(0x0B,1,0x54) = rueckwaerts gehen (Basisbank Clip 1, Halbtakt)
 *   Bild  807  @0x01950 `29 05`, @0x01956 `32 00 4c 2c 00 00 ee a3`
 *   Bild 1162  @0x01962 `29 06`, @0x0196C `32 00 02 3a 00 00 ee a3`, @0x01974 Dir_set 0x800, @0x0197C `42` Plc_ret,
 *              @0x01981 `3c 01` Cut_auto(1), @0x01983 `22 02 07 00`, @0x01987 `22 01 1b 00`
 *
 * RE2-RAM der echten Szene (Abnahme 1, Savestates s01..s26, Spieler *(0x800cfe14), +0x38 x, +0x76 Richtung, +0x04 Routinewort):
 *   s01 x -3800 (Pos_set), s02 x 0 Wort 0x00020504 (Plc_dest laeuft), s03 x 5736 / s04 16308 Wort 0x00010c04 (Laufen), s05..s11
 *   x 20066 (Ostwand), s12..s19 x 6565 -> 16038 Richtung 2048 Wort 0x00010b04 Flags 0x54/0x74 (rueckwaerts, Halbtakt), s20..s26
 *   x 11394 -> 19468. Leon steht also nie in der Grossaufnahme.
 *
 * Dazwischen laeuft der G5 sein Intro (Sub 2 [T0..T14], enemy_ai_boss_g5.c) — unveraendert, er ist Overlay-Code, kein Skript.
 *
 * ⛔ PORT-WAHL (je im Code markiert, Begruendung im Dossier):
 *   - Die Schnitte 9 und 10 sind RE2-Szenenkameras (RVD nur Ankerzone) ohne RE1.5-Gegenstueck: Szenen-Schnitte 16/17 = RE2s Kamera
 *     mit RE2s Hintergrund und Masken (der Wagen ist an der Stelle zellengleich, ROOM5090 SCA [21]/[25] = room7040 [2]/[4]).
 *   - Schnitte 5 und 6 (Spielkameras) -> Cut 12, die RE1.5-Kamera desselben Blicks nach Westen.
 *   - Spielerlagen in Cut 12 (s. G5A_X_SCHNITT5_PORT / G5A_X_ENDE).
 *   - RE2-Szenenclip 15 (Bank +0x190 = RDT-Szenenbank, 12 Bilder ohne Weg) hat kein RE1.5-Gegenstueck: Leon steht in diesen Bildern.
 *   - Laufen nach Osten (RE2 Bank +0x180 Clip 1, Weg 216,8/Bild aus der Wurzelspur) = RE1.5-Plc_dest Lauf (W01 Clip 0 auf der
 *     Stelle, Tempo 200 @0x80073f24) bis zur gemessenen RE2-Endlage an der Ostwand.
 *   - Frau im Kittel (5090) und Raumfeuer sind in den RE2-Einstellungen 16/17 nicht sichtbar (Abnahme 1 M6).
 *   - Der Teil VOR dem Schnitt 9 (Deckenplatte, Dach-Tentakel, Lauf aus dem Hinterwagen) ist im Port die Rueckstoss-Szene.
 *
 * Ton (Abnahme 1 M5): Xa 0x0B bei Bild 132 und Xa 0x0C bei Bild 438 (re15_g5auftritt_r41.h). Die festen Bildzahlen 437/807
 * bleiben: sie sind das Lesefenster bei 30 Bildern/s (1314 bzw. 1682 Sektoren / 150 * 30 = 262,8 / 336,4), und die RE2-
 * Aufnahme bestaetigt sie (Schnitt 9 echt 15,84 s = 4,40 + 8,80 Xa + 1,40 + 1,10 CD-Standbild vor Schnitt 10 + 0,14 Rest;
 * Schnitt 10 echt 13,12 s = 11,21 Xa + 1,07 + 0,74 Standbild vor Schnitt 5 + 0,10). Die Standbilder sind RE2s CD-Ladehalte
 * (Hintergrund des neuen Schnitts / Xa-Suchlauf) und werden nicht nachgebildet.
 * Sce_bgm_control @0x018F4/@0x018FA/@0x01900 (Bild 12) wirken in RE2 auf MAIN11 (Programme 0/1 leiser) bzw. sind ein No-op (SUB) -
 * im Port ohne MAIN11 NICHT angewandt (Dossier Nachbesserung 1, M5).
 */
#include "re15_g5auftritt_r41.h"
#include "re15_birkin_formen.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_emd.h"
#include "re15_player.h"
#include "re15_math.h"
#include "re15_skeleton.h"     /* re15_cos_q12 / re15_sin_q12 */
#include "re15_engine.h"        /* g_engine.frame_count (Protokollzeile) */
#include "re15_audio.h"         /* re15_audio_re2_xa / _stop (Abnahme 1 M5) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);   /* enemy_ai_boss_g5.c */
void re15_player_aim_interrupt(void);                         /* player_common.c */
void re15_player_event_reach_end(void);                       /* player_common.c */
void re15_player_skriptlage(int32_t x, int32_t z);            /* game_step_common.c: Pos_set ist keine Bewegung (keine Nachklemme) */

/* RE2 room7040 RID Schnitt 9 @0x184 = `00 00 b7 73 8c 0a 00 00 8e fc ff ff 84 8e ff ff d0 f9 ff ff 00 ee ff ff fe c5 ff ff d8 0d 00 00`
 * (RID-Offset RDT-Kopf [7] = 0x64, 32 Byte je Satz). fov 0x73B7 -> H = 231 (fov >> 7, camera_common.c). Der PRI-Zeiger 0xDD8 gilt
 * in RE2s RDT; die Plattform liest den Abschnitt aus RE15_G5A_PRI_DATEI, deshalb hier "kein Abschnitt". */
static const re15_camera_cut_t k_re2_schnitt9 = {
    0u, 29623u,
    2700, -882, -29052,
    -1584, -4608, -14850,
    0xFFFFFFFFu
};
/* RE2 room7040 RID Schnitt 10 @0x1A4 = `00 00 3c 68 f2 22 00 00 58 fc ff ff d2 9a ff ff 52 f9 ff ff 26 f2 ff ff 46 ab ff ff 2c 0f 00 00`:
 * fov 0x683C (H 208), Auge (8946,-936,-25902), Ziel (-1710,-3546,-21690). */
static const re15_camera_cut_t k_re2_schnitt10 = {
    0u, 26684u,
    8946, -936, -25902,
    -1710, -3546, -21690,
    0xFFFFFFFFu
};

/* Spielerlagen. z -23570 = 0xA3EE in jedem Pos_set/Plc_dest von sub06 (@0x0187C, @0x018B0, @0x0192A, @0x01956, @0x0196C). */
#define G5A_Z               (-23570)
#define G5A_GIER_OST        0x000       /* Dir_set @0x01884 `33 00 00 00 00 00 00 00` */
#define G5A_GIER_WEST       0x800       /* Dir_set @0x01932 / @0x01974 `33 00 00 00 00 08 00 00` */
#define G5A_X_VORLAUF       (-3800)     /* Pos_set @0x0187C `32 00 28 f1 ..` (0xF128) */
#define G5A_X_ARMIERUNG     1400        /* Plc_dest-Ziel @0x018B0 `40 00 05 20 78 05 ee a3` (x 0x0578) */
#define G5A_DEST_LAUF       0x05        /* Plc_dest-Modus @0x018B0 pc[2] = Lauf (RE1.5-Walker Modus 5, actor_locomotion.c) */
#define G5A_DEST_BIT        0x20        /* Plc_dest-Flagbit @0x018B0 pc[3]; Warteschleife Ck(5,0x20,0) @0x018C0 `21 05 20 00` */
#define G5A_VL_SCHLAF1      (1 + 20)    /* Evt_next @0x01898 + Sleep 20 @0x018AC `09 0a 14 00` -> Plc_dest */
#define G5A_VL_SCHLAF2      11          /* Sleep 11 @0x018D6 `09 0a 0b 00` -> Armierung @0x018DE */
#define G5A_BILD_LAUF       12          /* Evt_next @0x018E6 + Sleep 11 @0x018F0 -> Plc_motion(0x0C,1,4) @0x01906 */
#define G5A_BILD_NACKEN_FREI 72         /* + Sleep 60 @0x0190A -> Plc_neck @0x01912 */
/* ⛔ PORT-WAHL: RE2 laeuft ab Bild 12 ohne Ziel (Plc_motion(0x0C,1,4): Bank +0x180 Clip 1, 20 Bilder, Wurzelspur 189 -> 4336, Schleife
 * Flag 4) und bleibt an der Ostwand stehen: RE2-RAM s05..s11 Leon x 20066. Der Port laeuft mit dem RE1.5-Plc_dest-Lauf (Modus 5) bis
 * zu genau dieser gemessenen Lage. */
#define G5A_X_OSTWAND       20066
#define G5A_X_SCHNITT10     6500        /* @0x0192A */
/* Rueckwaerts gehen = RE2 Plc_motion(0x0B,1,0x54) @0x01940 -> Spieler-Routine 4 Sub 0x0B (Tabelle @0x800a8d90[11] = 0x80066478 ->
 * FUN_80066dec(e, +0x108, +0x17C) = Basisbank) Clip 1, Flags 0x54 (0x40 ohne Blende, 0x10 Halbtakt @0x80065d28-3c / @0x80065d6c-78,
 * 0x04 Schleife). Die Basisbank-Clip-1-Bytes sind in RE1.5 PL00 Clip 1 DIESELBEN (34 Bilder, Wurzelspur x -65 .. -1908, gelesen aus
 * shared_assets/PSX/PLD/PL00.PLD dir[0]/dir[1] und RE2-RAM s04 +0x17C/+0x108). Die Wurzelspur bewegt Leon (FUN_80066dec `jal 0x80015e7c`
 * @0x80066e68/@0x80066ed4, `jal 0x80015350` @0x80066f3c/@0x80066f54). */
#define G5A_RW_CLIP         1
#define G5A_RW_FLAGS        0x54
/* ⛔ PORT-WAHL Cut-12-Teil: RE2 stellt Leon fuer Kamera 5 auf 11340 (@0x01956), 2880 vor deren Auge (RID @0x104 x 14220), und laesst ihn
 * von dort rueckwaerts auf die Kamera zu aus dem Bild gehen. Der Port zeigt dafuer Cut 12 (Auge x 18526, RID ROOM5090 @0x60 + 12*32);
 * derselbe Abstand vor diesem Auge = 18526 - 2880 = 15646, damit Leon nach derselben Zeit (2880 / 28 je Bild) das Bild verlaesst. */
#define G5A_X_SCHNITT5_PORT 15646
/* ⛔ PORT-WAHL Szenenende: RE2 stellt Leon fuer Kamera 6 auf 14850 (@0x0196C). RE1.5 hat dafuer nur Cut 12, und 14850 laege dort im
 * Zonenstreifen 12 -> 13 (RVD @0x4CC, Westkante x 14000..14100), die Kampfkamera wuerde sofort nach Osten (Cut 13) springen. Leon steht
 * deshalb auf der Ostkante des Streifens 13 -> 12 (RVD @0x4F4 x 13873), der oestlichsten Lage, die der Raum beim Betreten von Osten in
 * Cut 12 zeigt (Kampfstart sub01 @0x1290 Cmp(Cut == 12)) - 977 vor RE2s Endlage (Abnahme 1: vertretbar). */
#define G5A_X_ENDE          13873

/* Kameras. RE2-Schnitte 9 und 10 sind Szenenkameras ohne RE1.5-Gegenstueck -> Szenen-Schnitte 16/17 (re15_g5auftritt_r41.h).
 * ⛔ PORT-WAHL: RE2-Schnitte 5 (14220,-1854,-24606)->(3582,..) und 6 (21042,-882,-24390)->(11898,..) sind Spielkameras; RE1.5s Kamera
 * desselben Blicks ist Cut 12 (18526,-2656,-24660)->(1496,1514,-20370) (RID @0x60 + 12*32), fuer beide. */
#define G5A_SCHNITT5_RE15   0x0C
#define G5A_LICHT_RE15      0x0B   /* Lichtsatz des Wagenendes (Cut 11 = RE2-Spielkamera 4 am selben Ort) */

enum { G5A_VL_SCHLAF = 0, G5A_VL_LAUF, G5A_VL_BLICK, G5A_SZENE };

/* bild = KI-Bild der Szene ab der Armierung (Bild 0 = das Bild, in dem der G5 [T0] laeuft); im Vorlauf -1. `frisch`: der erste Tick
 * nach dem Start zaehlt nicht weiter - gleich, ob der Start aus re15_bf_raumtick kommt (Tick im selben Durchlauf) oder von aussen. */
static struct {
    int laeuft, phase, bild, slot, frisch;
    int vl, vl_t;                       /* Vorlauf: Bild seit dem Start, Teilzaehler                         */
    int rw;                             /* 1 = Rueckwaertsgehen (Plc_motion(0x0B,1,0x54)) laeuft             */
    int rw_bild;                        /* zuletzt gesehenes Bild des Rueckwaerts-Clips                     */
    int16_t rw_prev_x, rw_prev_z;       /* Vorwert der Wurzelspur (+0x20C/+0x210)                            */
    int frau, frau_no_draw;             /* 5090: Typ 0x4D, gesichertes no_draw                               */
} s_g5a = { 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0 };

static int raum_5090(void) { return (g_current_room_id & 0xFFFEu) == 0x5090u; }

const re15_camera_cut_t *re15_g5a_schnitt(int cut)
{
    if (!raum_5090()) return (const re15_camera_cut_t *)0;
    if (cut == RE15_G5A_SCHNITT9)  return &k_re2_schnitt9;
    if (cut == RE15_G5A_SCHNITT10) return &k_re2_schnitt10;
    return (const re15_camera_cut_t *)0;
}

int re15_g5a_re2_schnitt(int cut)
{
    if (!raum_5090()) return -1;
    return cut == RE15_G5A_SCHNITT9 ? 9 : (cut == RE15_G5A_SCHNITT10 ? 10 : -1);
}

/* ⛔ PORT-WAHL: RE2s Lichtsaetze von Schnitt 9/10 (light.lit @0x500 + n*40) beleuchten RE2-Figuren. Im Port stehen in diesen Bildern
 * die RE1.5-Spielfigur und der G5 desselben Wagenendes, das Cut 11 beleuchtet; der Lichtsatz ist der von Cut 11. */
int re15_g5a_licht_schnitt(int cut) { return re15_g5a_re2_schnitt(cut) >= 0 ? G5A_LICHT_RE15 : cut; }

int re15_g5a_laeuft(void)  { return s_g5a.laeuft; }
int re15_g5a_bild(void)    { return (s_g5a.laeuft && s_g5a.phase == G5A_SZENE) ? s_g5a.bild : -1; }
int re15_g5a_vorlauf(void) { return (s_g5a.laeuft && s_g5a.phase != G5A_SZENE) ? s_g5a.vl : -1; }

/* Cut_chg-Zwilling wie bf_kamera (birkin_formen.c, Abnahme 1 M9): Zonen aus, Schnitt anfordern. Cut_auto(0) steht in RE2 am
 * Szenenkopf @0x01780 `3c 00` und @0x01890 `3c 00`. */
static void kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

/* Pos_set + Dir_set auf den Spieler (Work_set(1,0) @0x01952 usw.): X/Y/Z und alle drei Winkel (op_pos_set / op_dir_set). Ein
 * laufender Plc_dest/Plc_motion endet (Plc_stop @0x0187B = +0x04 <-> 0x1404, RE2 @0x80056ff0); die Pose geht auf Stehen, bis der
 * naechste Befehl sie setzt (player_common.c, player_mode 2). */
static void spieler_setzen(int32_t x, int16_t gier)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->y = 0; pl->z = G5A_Z;
    re15_player_skriptlage(pl->x, pl->z);
    pl->rot_x = 0; pl->rot_y = gier; pl->rot_z = 0;
    pl->walk_active = 0;
    re15_player_event_reach_end();
    re15_actor_set_motion(pl, 200);                      /* IDLE-Waechter 200 (player_common.c RE15_MOTION_IDLE) */
    pl->anim_frame = 0; pl->anim_frac = 7;
    pl->anim_use_pl00 = 0; pl->anim_flags = 0;
    s_g5a.rw = 0;
}

/* Plc_neck auf den Spieler: dieselbe Feldbelegung wie op_plc_neck (scd_vm.c; RE1.5 @0x80041f20-6c, Modus 2 -> Bits 0x80|0x08). */
static void spieler_nacken(int16_t tx, int16_t ty, int16_t tz, int16_t speed, int modus)
{
    static const uint8_t mode_bits[5] = { 0x12, 0x04, 0x08, 0x2a, 0x58 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->neck_tx = tx; pl->neck_ty = ty; pl->neck_tz = tz; pl->neck_speed = speed;
    pl->neck_flags = (uint8_t)(0x80 | (modus >= 0 && modus < 5 ? mode_bits[modus] : 0));
    pl->neck_sweep = 0;
}

/* Plc_dest auf den Spieler: dieselbe Belegung wie op_plc_dest im Walk-Zweig (scd_vm.c, RE1.5 @0x80041c14-4c); der RE1.5-Walker
 * (actor_locomotion.c re15_actor_step_walk) laeuft dann jeden Takt und setzt bei Ankunft Bit (5, bit). */
static void spieler_dest(int32_t x, int32_t z, uint8_t modus, uint8_t bit)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_player_event_reach_end();
    re15_player_aim_interrupt();
    pl->state = 4;
    pl->walk_dest_x = (int16_t)x; pl->walk_dest_z = (int16_t)z;
    pl->walk_mode = modus; pl->walk_flag_bit = bit;
    pl->anim_flags = 0;
    pl->walk_active = 1; pl->walk_fsm = 0;
    re15_game_flag_set(5, bit, 0);
    s_g5a.rw = 0;
}

/* Plc_motion(0x0B, 1, 0x54) auf den Spieler (s. G5A_RW_*): Belegung wie op_plc_motion fuer pc[1] != 0 (PL00-Bank, anim_use_pl00 = 1;
 * +0x04 = 4 @0x80041bb0, +0x06 = 0, Bild 0, Halteblende 0 wegen 0x40 @0x80050d24-30, +0x1c4 = pc[3]). */
static void spieler_rueckwaerts(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->walk_active = 0;
    re15_player_event_reach_end();
    re15_actor_set_motion(pl, (int16_t)G5A_RW_CLIP);
    pl->anim_use_pl00 = 1;
    pl->state = 4; pl->sub_state_2 = 0;
    pl->anim_frame = 0; pl->motion_init_delay = 1; pl->anim_freeze = 0;
    pl->anim_frac = 0;
    pl->anim_flags = G5A_RW_FLAGS;
    s_g5a.rw = 1; s_g5a.rw_bild = 0; s_g5a.rw_prev_x = 0; s_g5a.rw_prev_z = 0;
}

/* Wurzelspur des Rueckwaerts-Clips, wie FUN_80066dec sie bucht: FUN_80015e7c liest die Spur (Keyframe +6/+10) am Bild +0x14D VOR dem
 * Vorschub, setzt den Vorwert bei Bild 0 auf 0 (`lbu v0,333(t0)` @0x80015f04, `sw zero,524(t0)` @0x80015f14, `sh zero,528(t0)`
 * @0x80015f1c), Delta = Spur - Vorwert (@0x80015fcc); FUN_80015350 dreht es mit der Richtung +0x76 und addiert es auf die Lage.
 * Der Spieler-Takt (player_common.c, Halbtakt @0x80030670-80) schiebt das Bild vor; hier wird jedes vorgeschobene Bild einmal gebucht. */
static void rueckwaerts_takt(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_emd_animation_t *an = re15_player_pl00_anim();
    const re15_emd_skeleton_t  *sk = re15_player_pl00_skel();
    if (!s_g5a.rw || !an || !sk || G5A_RW_CLIP >= an->clip_count) return;
    if (pl->motion != G5A_RW_CLIP || !pl->anim_use_pl00) { s_g5a.rw = 0; return; }
    {
        const re15_emd_clip_t *c = &an->clips[G5A_RW_CLIP];
        int alt = s_g5a.rw_bild, neu = (int)pl->anim_frame;
        if (c->frame_count <= 0 || neu == alt) return;      /* Halbtakt-Bild: kein Schritt, kein Weg */
        if (alt < 0 || alt >= c->frame_count) alt = 0;
        {
            int16_t sx = 0, sy = 0, sz = 0;
            int kf = (int)(an->frames[c->first_frame + alt] & 0xFFFu);
            if (!re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz)) { s_g5a.rw_bild = neu; return; }
            if (alt == 0) { s_g5a.rw_prev_x = 0; s_g5a.rw_prev_z = 0; }                 /* @0x80015f04-1c */
            {
                int32_t dx = (int32_t)sx - s_g5a.rw_prev_x, dz = (int32_t)sz - s_g5a.rw_prev_z;
                int32_t cs = re15_cos_q12((int)(pl->rot_y & 0xfff)), sn = re15_sin_q12((int)(pl->rot_y & 0xfff));
                pl->x += (cs * dx + sn * dz) >> 12;                                     /* RotY wie op_sce_espr_on / Walker */
                pl->z += (-sn * dx + cs * dz) >> 12;
            }
            s_g5a.rw_prev_x = sx; s_g5a.rw_prev_z = sz;
        }
        s_g5a.rw_bild = neu;
    }
}

/* ⛔ PORT-WAHL (Abnahme 1 M6): in den RE2-Einstellungen 16/17 ist ausser Leon niemand im Bild (RE2-Aufnahme 61,47..92,76 s). Die Frau
 * im Kittel (5090, Spawn sub00 @0x0122A `44 00 4d ..` auf (600,0,-25300)) wird dort nicht gezeichnet; ab Cut 12 gilt wieder ihr
 * vorheriger Zustand. (Das Raumfeuer: platform/pc/main.c pc_draw_effects.) */
static void frau_verbergen(int an)
{
    if (s_g5a.frau < 0) return;
    {
        re15_actor_t *f = &g_actors[s_g5a.frau];
        if (!f->active || f->type != 0x4Du) { s_g5a.frau = -1; return; }
        f->no_draw = (uint8_t)(an ? 1 : s_g5a.frau_no_draw);
    }
}

void re15_g5a_reset(void)
{
    if (s_g5a.laeuft) {                                  /* Raumwechsel mitten in der Szene: Sperre nicht stehen lassen */
        re15_game_flag_set(2, 7, 0);
        re15_game_flag_set(1, 27, 0);
        re15_audio_re2_xa_stop();                        /* und keine Birkin-Laute im naechsten Raum */
    }
    s_g5a.laeuft = 0; s_g5a.phase = G5A_VL_SCHLAF; s_g5a.bild = -1; s_g5a.slot = -1; s_g5a.frisch = 0;
    s_g5a.vl = 0; s_g5a.vl_t = 0; s_g5a.rw = 0; s_g5a.frau = -1;
}

void re15_bf_g5_auftritt_start(int slot)
{
    if (s_g5a.laeuft || !raum_5090() || slot < 1 || slot >= RE15_ACTOR_MAX) return;
    s_g5a.laeuft = 1; s_g5a.phase = G5A_VL_SCHLAF; s_g5a.bild = -1; s_g5a.slot = slot; s_g5a.frisch = 1;
    s_g5a.vl = 0; s_g5a.vl_t = 0; s_g5a.rw = 0;
    /* Szenenkopf sub06 @0x01778 `22 02 07 01` / @0x0177C `22 01 1b 01`: Spielersperre + Szenenbalken (re15_cine_active) */
    re15_game_flag_set(2, 7, 1);
    re15_game_flag_set(1, 27, 1);
    re15_player_aim_interrupt();                         /* die Rueckstoss-Szene liess Leon zielen und feuern */
    /* VORLAUF (Abnahme 1 M7): leere Tuer, Leon laeuft von links herein. @0x0187B Plc_stop, @0x0187C Pos_set(-3800), @0x01884 Dir_set 0,
     * @0x01890 Cut_auto(0), @0x01892 Cut_chg(9). */
    spieler_setzen(G5A_X_VORLAUF, G5A_GIER_OST);
    kamera(RE15_G5A_SCHNITT);
    /* Der G5 existiert in RE2 schon (MAIN sub00 Sce_em_set @0x011AC `44 00 00 36 ..` Typ 0x36 auf (-32000,0,-32000)) und wartet
     * unarmiert in [T0] (@0x801011b0-b8). Port: Typwechsel jetzt, Armierung (grid 0x13 = +0x1D4-Bit-0-Analogon) erst @0x018DE. */
    re15_bf_morph_zu_g5(slot);
    {
        re15_actor_t *e = &g_actors[slot];
        e->grid_id = 0;                                  /* unarmiert: enemy_ai_boss_g5.c tut bis grid 0x13 nichts */
        e->x = -32000; e->y = 0; e->z = -32000;          /* Parkplatz @0x011AC */
        e->no_draw = 0;
    }
    /* M6: Frau im Kittel (nur 5090) fuer die Einstellungen 16/17 merken und verbergen */
    s_g5a.frau = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x4Du) { s_g5a.frau = i; s_g5a.frau_no_draw = g_actors[i].no_draw; break; }
    frau_verbergen(1);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[g5auftritt] F%u Raum %04X: Vorlauf, Schnitt %d (RE2 room7040 Cut 9), Spieler x %d, Slot %d, Frau %d\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, RE15_G5A_SCHNITT, (int)G5A_X_VORLAUF, slot, s_g5a.frau);
#endif
}

static void armieren(void)
{
    re15_actor_t *e = &g_actors[s_g5a.slot];
    /* @0x018DA Work_set(3,0) + @0x018DE Member_set(0x17,1): +0x1D4 Bit 0 (Sprungtabelle @0x80011228[23] -> `sh a2,468(a0)` @0x80055d90).
     * Der G5-Tick desselben KI-Durchlaufs laeuft danach [T0] (X -9000 @0x801011d0, Clip 1, SE 10). @0x018E2 Member_set(7,0) =
     * `sh a2,270(a0)` (+0x10E, @0x80055d10) hat im Port keinen Leser. */
    e->grid_id = 0x13u;
    s_g5a.phase = G5A_SZENE; s_g5a.bild = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[g5auftritt] F%u Raum %04X: Armierung nach %d Vorlaufbildern, Schnitt %d, Spieler x %d\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, s_g5a.vl, RE15_G5A_SCHNITT,
            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x);
#endif
}

static void vorlauf_tick(void)
{
    s_g5a.vl++;
    kamera(RE15_G5A_SCHNITT);
    frau_verbergen(1);
    switch (s_g5a.phase) {
    case G5A_VL_SCHLAF:
        if (s_g5a.vl == 1)                               /* @0x018A2 `41 02 00 00 38 ff 00 00 c8 64` (nach Evt_next @0x01898) */
            spieler_nacken(0, (int16_t)0xFF38, 0, (int16_t)0x64C8, 2);
        if (s_g5a.vl >= G5A_VL_SCHLAF1) {                /* @0x018B0 Plc_dest(0,5,0x20,1400,-23570) */
            spieler_dest(G5A_X_ARMIERUNG, G5A_Z, G5A_DEST_LAUF, G5A_DEST_BIT);
            s_g5a.phase = G5A_VL_LAUF;
        }
        break;
    case G5A_VL_LAUF:                                    /* @0x018B8-C3: Do { Evt_next } Edwhile Ck(5,0x20,0) */
        if (re15_game_flag_get(5, G5A_DEST_BIT)) {
            /* @0x018C7 Plc_motion(0,15,0x08): RE2-Szenenbank-Clip (PORT-WAHL: Leon steht); @0x018CC Plc_neck: Blick zur Tuer */
            spieler_nacken(0, (int16_t)0xFE00, 0x0080, 0x1064, 2);
            s_g5a.phase = G5A_VL_BLICK; s_g5a.vl_t = 0;
        }
        break;
    case G5A_VL_BLICK:                                   /* Sleep 11 @0x018D6 */
        if (++s_g5a.vl_t >= G5A_VL_SCHLAF2) armieren();
        break;
    default: break;
    }
}

void re15_g5a_tick(void)
{
    if (!s_g5a.laeuft) return;
    if (!raum_5090()) { re15_g5a_reset(); return; }
    if (s_g5a.frisch) { s_g5a.frisch = 0; return; }
    if (s_g5a.phase != G5A_SZENE) { vorlauf_tick(); return; }
    const int b = ++s_g5a.bild;
    rueckwaerts_takt();
    if (b == RE15_G5A_BILD_XA0B) (void)re15_audio_re2_xa(RE15_G5A_XA0B_DATEI, RE15_G5A_XA0B_LESE);   /* @0x01920 `59 00 0b 00` */
    if (b == RE15_G5A_BILD_XA0C) (void)re15_audio_re2_xa(RE15_G5A_XA0C_DATEI, RE15_G5A_XA0C_LESE);   /* @0x01946 `59 00 0c 00` */
    if (b == G5A_BILD_LAUF)                              /* @0x01906 Plc_motion(0x0C,1,4): Laufen nach Osten (PORT-WAHL s. G5A_X_OSTWAND) */
        spieler_dest(G5A_X_OSTWAND, G5A_Z, G5A_DEST_LAUF, G5A_DEST_BIT);
    if (b == G5A_BILD_NACKEN_FREI)                       /* @0x01912 `41 02 00 00 00 00 00 00 ff ff` */
        spieler_nacken(0, 0, 0, (int16_t)0xFFFF, 2);
    if (b < RE15_G5A_BILD_SCHNITT10) {
        kamera(RE15_G5A_SCHNITT);
        frau_verbergen(1);
    } else if (b < RE15_G5A_BILD_SCHNITT5) {
        if (b == RE15_G5A_BILD_SCHNITT10) {              /* @0x0192A Pos_set(6500), @0x01932 Dir_set 0x800, @0x01940 Plc_motion(0x0B,1,0x54) */
            spieler_setzen(G5A_X_SCHNITT10, G5A_GIER_WEST);
            spieler_rueckwaerts();
        }
        kamera(RE15_G5A_SCHNITT10);
        frau_verbergen(1);
    } else if (b < RE15_G5A_BILD_ENDE) {
        if (b == RE15_G5A_BILD_SCHNITT5) {               /* @0x01950 Cut_chg(5) -> Cut 12, @0x01956 Pos_set (PORT-WAHL); der Rueckwaertsgang laeuft weiter */
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->x = G5A_X_SCHNITT5_PORT; pl->y = 0; pl->z = G5A_Z;
            re15_player_skriptlage(pl->x, pl->z);
            frau_verbergen(0);                           /* M6: in Cut 12 wieder wie vorher */
        }
        kamera(G5A_SCHNITT5_RE15);
    } else {
        /* Szenenende @0x01962..@0x01987. Plc_ret = RE1.5-Handler 0x80041f88 (scd_vm.c op_plc_ret): Spieler +0x4 = 1, +0x5..+0x7 = 0,
         * Kommandowort live; Pose Stehen. Set(1,4,1) @0x0197D hat im Port keinen Leser und wird nicht gesetzt. @0x01967 Plc_motion(2,2,0x44)
         * steht vor dem Plc_ret und wird von ihm im selben Bild beendet. */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        spieler_setzen(G5A_X_ENDE, G5A_GIER_WEST);       /* @0x0196C Pos_set + @0x01974 Dir_set 0x800 (Lage: PORT-WAHL oben) */
        re15_actor_set_motion(pl, 200); pl->anim_frame = 0;
        pl->state = 1; pl->sub_state_1 = 0; pl->sub_state_2 = 0; pl->sub_state_3 = 0;
        g_scd.player_mode = 1;
        /* Kamera: RE2 schaltet auf Kamera 6 und gibt die Zonen frei; im Port der Zonenschnitt der Spielerlage (Cut 12 bei 13873). */
        kamera(re15_bf_schnitt_aus_lage(pl->x));
        g_scd.cut_auto_enabled = 1;                      /* @0x01981 `3c 01` */
        re15_game_flag_set(2, 7, 0);                     /* @0x01983 */
        re15_game_flag_set(1, 27, 0);                    /* @0x01987 */
#ifdef RE15_PLATFORM_PC
        {   int sub = -1, ph = -1;
            (void)re15_g5_boss_zustand(&sub, &ph, 0);
            fprintf(stderr, "[g5auftritt] F%u Szenenende Bild %d: G5 Sub %d Ph %d, Spieler x %d, Cut %d\n",
                    (unsigned)g_engine.frame_count, b, sub, ph, (int)pl->x, (int)g_scd.cam_id); }
#endif
        s_g5a.laeuft = 0; s_g5a.slot = -1;
    }
}
