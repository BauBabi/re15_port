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
#include "re15_light.h"         /* re15_light_parse (Lichtsatz der Szenen-Schnitte) */
#include "re15_rumble.h"        /* NB6: Vibration sub14 (Vib_set1 / Vib_fade_set) */
#include "re2_fx.h"             /* NB6: Brocken sub05 (Sce_espr_on Effekt 0x19) */
#include <string.h>
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
#define G5A_LICHT_RE15      0x0B   /* Rueckfall ohne RE2-Lichtsatz: Cut 11 (= RE2-Spielkamera 4 am selben Ort) */

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

/* ═══ M4 (Nachbesserung 5): TUNNELLAMPEN, BEBEN und BEBEN-TON des Vorlaufs (RE2 ROOM7040 sub09/sub10/sub11/sub12, sub04) ═══
 *
 * sub06 @0x01892 `29 09` Cut_chg(9), @0x01894 `04 07 18 09` Evt_exec(7, sub09), @0x01898 Evt_next, @0x0189A `04 06 18 04`
 * Evt_exec(6, sub04); @0x0193A `29 0a` Cut_chg(10), @0x0193C `04 07 18 0a` Evt_exec(7, sub10).
 *   sub09 @0x01B20 `04 04 18 0b` Evt_exec(4, sub11), @0x01B24 `09 0a 0a 00` Sleep 10, @0x01B28 `04 03 18 0c` Evt_exec(3, sub12).
 *   sub11 (Objekt 3) / sub12 (Objekt 4) @0x01B4C / @0x01B98:
 *     Work_set(4,n) `2e 04 0n` (Typ 4 -> 0x800d0324 + n*0x1F8, Tabelle @0x800111f0[3] = 0x80055994),
 *     Pos_set `32 00 00 00 d0 ee 60 be` @0x01B50/@0x01B9C = (0,-4400,-16800) (Handler @0x80055ba0: sw +0x38/+0x3C/+0x40),
 *     Dir_set `33 00 00 0c 00 0c 00 04` @0x01B58/@0x01BA4 = (0x0C00,0x0C00,0x0400) (Handler @0x80055bd0: sh +0x74/+0x76/+0x78),
 *     Schleife: If Member_cmp `3e 00 0b 04 e0 b1` @0x01B68/@0x01BB4 (Member 0x0B = +0x38 `lw` @0x80055ffc, Op 4 = "<=" @0x80055f18)
 *       -> Pos_set wie oben @0x01B72/@0x01BBE; Member_copy(V16 <- +0x38) `3d 10 0b`, Calc `26 00 01 10 e8 03` @0x01B84/@0x01BD0
 *       (Op 1 = `subu` @0x80054664, `sh` @0x80054754: 16 Bit), Member_set2(+0x38 <- V16) `35 0b 10` (`lh` @0x80055c7c, `sw` @0x80055d30),
 *       Evt_next, Goto `17 ff ff 00 d0 ff`.
 *   sub10 @0x01B2E: Evt_kill 3/4, Pos_set `32 00 00 83 00 00 00 83` @0x01B36/@0x01B42 = (-32000,0,-32000) fuer Objekt 3 und 4.
 *   RE2-RAM (Abnahme-1-Savestates, Objekt n = 0x800d0324 + n*0x1F8): s02..s11 Objekt 3/4 +0x3C -4400, +0x40 -16800, +0x74..78
 *   0C00/0C00/0400, +0x38 Vielfache von 1000 in -20000..-1000, Objekt 4 = Objekt 3 +- 10000; Szenenbild f (Zuordnung Abnahme 4) ->
 *   Objekt 3 +0x38 = -1000 * (((f - 3) mod 20) + 1) an s03..s11 ohne Ausnahme; ab s12 beide (-32000,0,-32000).
 *
 * TASK-REIHENFOLGE: die Skript-Schleife FUN_800536c4 laeuft die Plaetze 0..9 aufsteigend (0x800d7860 + i*0x174, `addiu s1,s1,0x174`
 * @0x8005379c, `sltiu v0,s2,0xa` @0x80053794); Evt_exec @0x800538a4 -> FUN_80053138 legt den Task in Platz pc[1] an (< 10 direkt,
 * @0x80053154 -> @0x800531f8). sub06 laeuft in Platz 9 (MAIN sub00 @0x01292 `04 09 18 02` + Evt_chain @0x013A8), sub09 in 7, sub04 in 6,
 * sub11 in 4, sub12 in 3: ein angelegter Task kleinerer Nummer laeuft erst im NAECHSTEN Bild. Cut_chg(9) im Bild C -> sub09 C+1 ->
 * sub11 (erstes Bild, Lampe 3 bei -1000) C+2; sub06 @0x01898 Evt_next -> Evt_exec(6, sub04) C+1 -> erstes Beben C+2.
 * Aufnahme: Schnitt sichtbar 61,467 s (Spielbild 2 Videobilder unbewegt), Beben 3 Zeilen ab 61,500 s - ein Bild spaeter, wie der Port
 * (Kamera im Bild nach der Anforderung sichtbar, Beben ab Vorlaufbild G5A_SKRIPT_START).
 * ⛔ PORT-WAHL (Phase): RE2 Cut_chg(9) -> Armierung = 59 Bilder (RAM-Phase: Lampe 3 beginnt bei Szenenbild -57 mod 20 = C+2; Aufnahme:
 * Schnitt bei -59,8 + Achsenversatz 1,5..3,3 = C+1), der Port-Vorlauf ist 55 (F1558 -> F1613, Abnahme 4). Damit die Lampe bei gleichem
 * Szenenbild an derselben Stelle steht wie in RE2 (RAM s03..s11 bitgleich), laeuft sub11 im Port beim Start G5A_LAMPE_VORSPRUNG = 59 - 55
 * Bilder nach: das Skript selbst ist unveraendert, nur sein erstes Bild steht 4 Bilder spaeter in sub11s eigener Zeit.
 */
#define G5A_LAMPE_X0        0          /* Pos_set @0x01B50/@0x01B9C x */
#define G5A_LAMPE_Y         (-4400)    /* ebenda y 0xEED0 */
#define G5A_LAMPE_Z         (-16800)   /* ebenda z 0xBE60 */
#define G5A_LAMPE_MARKE     (-20000)   /* Member_cmp @0x01B68/@0x01BB4 `e0 b1`, Op 4 "<=" @0x80055f18 */
#define G5A_LAMPE_SCHRITT   1000       /* Calc @0x01B84/@0x01BD0 `e8 03`, Op 1 Abzug @0x80054664 */
#define G5A_LAMPE_VERSATZ   10         /* sub09 Sleep 10 @0x01B24 zwischen sub11 und sub12 (RAM: Objekt 4 = Objekt 3 +- 10000) */
#define G5A_LAMPE_PARK      (-32000)   /* sub10 @0x01B36/@0x01B42 und Obj_model_set @0x01240/@0x01266 (x/z) */
#define G5A_LAMPE_VORSPRUNG 4          /* ⛔ PORT-WAHL, s.o.: RE2-Vorlauf 59 - Port-Vorlauf 55 */
#define G5A_SKRIPT_START    2          /* Vorlaufbild, in dem sub11 und sub04 ihr erstes Bild laufen (Cut_chg-Bild 0, Task-Reihenfolge s.o.) */
static const int16_t k_lampe_dir[3]  = { 0x0C00, 0x0C00, 0x0400 };   /* Dir_set @0x01B58/@0x01BA4 */
static const int16_t k_lampe_park[3] = { 0x0000, 0x0C00, 0x0000 };   /* Obj_model_set @0x01240 / @0x01266 pc[0x14..0x19] */

static struct { int laeuft, t; int an[2]; int32_t x[2]; } s_lampe;

/* Ein Bild von sub11 (i = 0, Objekt 3) bzw. sub12 (i = 1, Objekt 4). */
static void lampe_task(int i)
{
    const int start = i ? G5A_LAMPE_VERSATZ : 0;
    if (s_lampe.t < start) return;
    if (s_lampe.t == start) { s_lampe.an[i] = 1; s_lampe.x[i] = G5A_LAMPE_X0; }     /* @0x01B50 Pos_set, @0x01B58 Dir_set */
    if (s_lampe.x[i] <= G5A_LAMPE_MARKE) s_lampe.x[i] = G5A_LAMPE_X0;               /* @0x01B64 If Member_cmp, @0x01B72 Pos_set */
    s_lampe.x[i] = (int16_t)(s_lampe.x[i] - G5A_LAMPE_SCHRITT);                     /* @0x01B80 Member_copy .. @0x01B8A Member_set2 */
}

static void lampe_takt(void)
{
    if (!s_lampe.laeuft) return;
    lampe_task(0); lampe_task(1);
    s_lampe.t++;
}

static void lampen_start(void)                                      /* erstes Bild von sub11 (sub09 @0x01B20 Evt_exec(4, sub11)) */
{
    memset(&s_lampe, 0, sizeof s_lampe);
    s_lampe.x[0] = s_lampe.x[1] = G5A_LAMPE_PARK;
    s_lampe.laeuft = 1;
    for (int k = 0; k <= G5A_LAMPE_VORSPRUNG; k++) lampe_takt();    /* erstes Bild + Vorsprung (PORT-WAHL) */
}

static void lampen_parken(void)                                     /* @0x0193C Evt_exec(7, sub10) */
{
    s_lampe.laeuft = 0; s_lampe.an[0] = s_lampe.an[1] = 0;
    s_lampe.x[0] = s_lampe.x[1] = G5A_LAMPE_PARK;
}

int re15_g5a_lampe(int i, int32_t pos[3], int16_t dir[3])
{
    if (i < 0 || i > 1 || !raum_5090()) return 0;
    const int an = s_lampe.an[i];
    if (pos) { pos[0] = an ? s_lampe.x[i] : G5A_LAMPE_PARK; pos[1] = an ? G5A_LAMPE_Y : 0; pos[2] = an ? G5A_LAMPE_Z : G5A_LAMPE_PARK; }
    if (dir) for (int k = 0; k < 3; k++) dir[k] = an ? k_lampe_dir[k] : k_lampe_park[k];
    return an;
}

/* RE2 FUN_800363f0 zeichnet ein Raumobjekt nur, wenn FUN_8002c820(+0x68 Weltlage, *(0x800ce338)) != 0 (`jal 0x8002c820` @0x80036534,
 * `bnez v0` @0x8003653c) oder Flag 0x10 (@0x8003654c; Objekt 3/4: Wort +0 = 0x0B). *(0x800ce338) = Ankerzone des aktuellen Schnitts
 * im RVD (RAM s01..s11: 0x8011a600 = RDT +0x45C, s12: +0x470). ROOM7040.RDT @0x45C `01 ff 09 00 18 ca 79 88 88 c8 d8 c3 b0 1d d8 c3
 * 88 2c 78 88` (Schnitt 9), @0x470 `01 ff 0a 00 bc d0 34 8b 3c dd 1c c1 d0 20 d8 c3 88 2c 78 88` (Schnitt 10). */
static const int16_t k_zone9[8]  = { -13800, -30599, -14200, -15400, 7600, -15400, 11400, -30600 };
static const int16_t k_zone10[8] = { -12100, -29900, -8900, -16100, 8400, -15400, 11400, -30600 };

/* FUN_8002c820 Schritt fuer Schritt (Produkte = mflo, 32 Bit). */
static int32_t mflo32(int32_t a, int32_t b) { return (int32_t)(uint32_t)((int64_t)a * (int64_t)b); }
int re15_g5a_lampe_zone(int cut, int32_t px, int32_t pz)
{
    const int16_t *q = (cut == RE15_G5A_SCHNITT9) ? k_zone9 : (cut == RE15_G5A_SCHNITT10) ? k_zone10 : 0;
    if (!q || !raum_5090()) return 0;
    const int32_t x0 = q[0], z0 = q[1], x1 = q[2], z1 = q[3], x2 = q[4], z2 = q[5], x3 = q[6], z3 = q[7];
    int32_t t2 = pz - z0, t3 = x1 - x0, a0 = px - x0, a2 = z1 - z0;
    if (mflo32(a2, a0) < mflo32(t3, t2)) return 0;                                  /* @0x8002c864 */
    const int32_t a3 = x3 - x0, t0 = z3 - z0;
    if (mflo32(a3, t2) < mflo32(t0, a0)) return 0;                                  /* @0x8002c888 */
    t2 -= z2 - z0; t3 -= x2 - x0; a0 -= x2 - x0; a2 -= z2 - z0;                     /* @0x8002c89c-b8 */
    if (mflo32(t3, t2) < mflo32(a2, a0)) return 0;                                  /* @0x8002c8c8 */
    if (mflo32(t0 - (z2 - z0), a0) < mflo32(a3 - (x2 - x0), t2)) return 0;          /* @0x8002c8ec, sonst 1 @0x8002c8f4 */
    return 1;
}

/* BEBEN: sub04 @0x0169E (ab dem Bild nach dem Schnitt, @0x01898 Evt_next): Evt_exec(8, sub05) @0x0169E, Gosub sub14 @0x016A2 (Vib_*),
 * Sce_shake_on `5c 00 fd` @0x016A4, Evt_next, `5c 00 fb` @0x016A8, Evt_next, Se_on `36 02 0c 00 01 ..` @0x016AC, For 5 (`0d 00 06 00 05 00`
 * @0x016B8, Zaehler `lh 4(pc)` @0x80053b28) { `5c 00 f9`, Evt_next }, dann fa/fb/fc/fd/fe/ff je ein Bild (@0x016C4..@0x016D8).
 * Sce_shake_on @0x80058164: s3 = (s8)pc[2] (@0x800581a4), je Zeile 0..239 (`addiu s5,zero,0xf0` @0x800581b0) FUN_8002ac94: Quelle
 * Zeile + pc[2] (@0x8002ad10, Rand geklemmt @0x8002ad14-38) -> Ziel Zeile, Breite 0x140 (@0x8002acc4) = das Bild rutscht um -pc[2] Zeilen
 * nach unten. Aufnahme (Abnahme-2-Mitschnitt 640x480, 61,50..61,92 s): Verschiebung 6/11/15x5/13/11/9/6/4/2 Halbzeilen, 13 Bilder. */
static const int8_t k_beben[13] = { -3, -5, -7, -7, -7, -7, -7, -6, -5, -4, -3, -2, -1 };
#define G5A_BEBEN_TON_BILD  3          /* drittes Bild von sub04: Se_on @0x016AC nach @0x016A4/@0x016A8 */
/* sub04-Bild s = 1.. ab Vorlaufbild G5A_SKRIPT_START (s = vl - G5A_SKRIPT_START + 1). */
static int s_beben;
void (*re15_g5a_beben_ton_hook)(void) = 0;

int re15_g5a_beben(void) { return (raum_5090() && s_g5a.laeuft) ? s_beben : 0; }

/* NB6 VIBRATION: sub04 @0x016A2 `18 0e` Gosub sub14 im ersten sub04-Bild, vor dem ersten Beben. sub14 @0x01C02:
 *   `8c 00 64 c8 07 00 00 00` Vib_fade_set, `8b fa 05 00 02 00` Vib_set1, `8c 00 c8 32 11 00 05 00` Vib_fade_set.
 * Vib_set1 @0x80059394: a0 = pc[2..3] (`lhu a0,2(v0)` @0x800593ac) Dauer, a1 = pc[1] (`lbu a1,1(v0)` @0x800593b0) Staerke,
 *   a2 = pc[4..5] (`lhu a2,4(v0)` @0x800593b4) Vorlauf -> `jal 0x80039514` @0x800593b8 = re15_rumble_large(Dauer, Staerke, Vorlauf);
 * Vib_fade_set @0x800593e4: a0 = pc[4..5] Dauer (@0x800593fc), a1 = pc[2] von (@0x80059400), a2 = pc[3] bis (@0x80059404),
 *   a3 = pc[6..7] Vorlauf (@0x80059408) -> `jal 0x800395b8` @0x8005940c = re15_rumble_ramp(Dauer, von, bis, Vorlauf).
 * (Vib_set0 @0x80059348 -> FUN_8003947C(pc[2..3], pc[4..5]) = re15_rumble_small benutzt die Szene nicht.) Opcode-Tabelle @0x800a74c8:
 * [0x8A] = 0x80059348, [0x8B] = 0x80059394, [0x8C] = 0x800593e4. Die Raum-Schleife sub15 (@0x01C1A: 6 x Vib_set1, Sleep 0x34, Goto;
 * Platz 6, MAIN sub00 @0x01296) beendet sub06 @0x01784 `05 06` vor dem Vorlauf; sub13 (@0x01BE4) ruft sub06 @0x0179E `18 0d` im Teil VOR
 * Cut_chg(9), den der Port durch Rueckstoss/Sturz der Riesenform ersetzt (Dossier 1.1) - beide gehoeren nicht zur Port-Szene.
 * RE2-RAM s01..s03 (nb6/messung_ring_ram.txt): Ringe A/B leer, Aktuatoren 0x800CBC20/21 = 0 0.
 * PC: der Ring-Takt laeuft im Pad-Takt (input_pc.c pad_rumble_tick, Runde 18) und geht an SDL_GameControllerRumble; ohne Pad
 * geschieht nichts - wie RE2 (DAT_800CBC20 hat nur den PadSetAct-Leser, kein Kamera-Beben; re15_rumble.h). */
static void vibration_sub14(void)
{
    re15_rumble_ramp(0x07, 0x64, 0xC8, 0x00);           /* @0x01C02 */
    re15_rumble_large(0x05, 0xFA, 0x02);                /* @0x01C0A */
    re15_rumble_ramp(0x11, 0xC8, 0x32, 0x05);           /* @0x01C10 */
}

/* NB6 BROCKEN: sub04 @0x0169E `04 08 18 05` Evt_exec(8, sub05) im ersten sub04-Bild; Platz 8 > 6 -> sub05 laeuft im SELBEN Bild
 * (Schleife FUN_800536c4 aufsteigend). sub05 @0x016DE: Sce_espr_on `3a 00 19 00 00 00 00 10 x x 50 ec z z 00 00` (Handler @0x800565a4:
 * Bank pc[2] / Sub pc[3] / Skala pc[6..7] -> a0 = 0x19001000 @0x800565cc-e8, Matrix pc[4] = 0 -> Einheit 0x8009DB44, Lage pc[8..13]
 * (y -5040 = 0xEC50), Gier pc[14..15] = 0; `jal 0x8001bf10` @0x80056614 = sofort lebendig), dazwischen Sleep 5/5/3/5/5/3
 * (@0x016FE..@0x01772). RE2-RAM s02 (sub05-Bild 39): Alter der Brocken 29/26/21/21/16 = Spawnbilder 11/14/19/19/24. */
static const struct { int bild; int16_t x, z; } k_brocken[8] = {
    {  1, 1060, -21520 }, {  1, 3000, -24545 },           /* @0x016DE, @0x016EE */
    {  6, 2670, -23420 },                                 /* @0x01702 nach Sleep 5 @0x016FE */
    { 11,  500, -25330 },                                 /* @0x01716 nach Sleep 5 @0x01712 */
    { 14, 3000, -24545 },                                 /* @0x0172A nach Sleep 3 @0x01726 */
    { 19, 2670, -23420 }, { 19, 2830, -21090 },           /* @0x0173E, @0x0174E nach Sleep 5 @0x0173A */
    { 24,  500, -25330 },                                 /* @0x01762 nach Sleep 5 @0x0175E */
};
#define G5A_BROCKEN_Y       (-5040)    /* pc[10..11] 0xEC50 */
#define G5A_BROCKEN_A0      0x19001000u/* Bank 0x19, Sub 0, Skala 0x1000 (pc[2], pc[3], pc[6..7]) */
static int s_brocken_n = 0;            /* Messung: gespawnte Brocken dieser Szene */
int re15_g5a_brocken_gespawnt(void) { return s_brocken_n; }

static void brocken_takt(int sb)
{
    for (int k = 0; k < 8; k++) {
        if (k_brocken[k].bild != sb) continue;
        const int16_t ofs[4] = { k_brocken[k].x, G5A_BROCKEN_Y, k_brocken[k].z, 0 };
        if (re2fx_spawn_sofort(G5A_BROCKEN_A0, 0, re2fx_einheitsmatrix, ofs) >= 0) s_brocken_n++;
    }
}

static void beben_takt(int vl)
{
    const int sb = vl - G5A_SKRIPT_START + 1;
    if (sb == 1) { s_brocken_n = 0; vibration_sub14(); }   /* @0x0169E Evt_exec sub05, @0x016A2 Gosub sub14 vor @0x016A4 */
    brocken_takt(sb);
    s_beben = (sb >= 1 && sb <= (int)sizeof k_beben) ? k_beben[sb - 1] : 0;
    if (sb == G5A_BEBEN_TON_BILD && re15_g5a_beben_ton_hook) re15_g5a_beben_ton_hook();
}

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

/* LICHT der Szenen-Schnitte (Nachbesserung G5 2, Punkt 5). Der Lichtsatz gehoert zur KAMERA, nicht zur Figur: RE2 laedt je Schnitt
 * einen 40-Byte-Satz aus dem LIT-Abschnitt (ROOM7040.RDT Kopf [9] = 0x500, Format = RE1.5 re15_light_parse: Skala, 3 Typbytes,
 * 3 Farben, Umgebung, 3 Lagen, 3 Helligkeiten) und beleuchtet damit G5, Arme und Leon. Schnitt 9 @0x668:
 *   `01 00 00 00 a8 a8 a8 be 8c 32 ff ff ff 49 49 49 ac 0d 6c ee 98 a4 68 f7 00 00 34 a4 e4 f3 00 00 fc bd f8 2a 10 27 40 1f`
 *   = drei Punktlichter: grau (168,168,168) bei (3500,-4500,-23400) Hell 11000, ORANGE (190,140,50) am Boden vor der Tuer
 *   (-2200,0,-23500) Hell 10000, weiss (255,255,255) bei (-3100,0,-16900) Hell 8000; Umgebung (73,73,73).
 * Schnitt 10 @0x690: dieselben zwei ersten Lichter, drittes aus, Umgebung (74,73,73).
 * ⛔ HIER STAND die PORT-WAHL "Lichtsatz von Cut 11" (RE1.5 ROOM5090 LIT @0x55c + 11*40: Skala 2, alle drei Farben (0,0,0), nur
 * Umgebung (83,78,78)) - die Masse und ihre Stacheln blieben flach braun, in RE2 gluehen sie im orangen Bodenlicht (Vergleich
 * nb1/vergleich_re2_leon_elza_nb1_1.jpg Bild 180/240). */
static const uint8_t k_re2_licht9[40] = {
    0x01, 0x00, 0x00, 0x00, 0xa8, 0xa8, 0xa8, 0xbe, 0x8c, 0x32, 0xff, 0xff, 0xff, 0x49, 0x49, 0x49,
    0xac, 0x0d, 0x6c, 0xee, 0x98, 0xa4, 0x68, 0xf7, 0x00, 0x00, 0x34, 0xa4, 0xe4, 0xf3, 0x00, 0x00,
    0xfc, 0xbd, 0xf8, 0x2a, 0x10, 0x27, 0x40, 0x1f };
static const uint8_t k_re2_licht10[40] = {
    0x01, 0x00, 0x00, 0x00, 0xa8, 0xa8, 0xa8, 0xbe, 0x8c, 0x32, 0x00, 0x00, 0x00, 0x4a, 0x49, 0x49,
    0xac, 0x0d, 0x6c, 0xee, 0x98, 0xa4, 0x68, 0xf7, 0x00, 0x00, 0x34, 0xa4, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xf8, 0x2a, 0x10, 0x27, 0x00, 0x00 };
static int s_g5a_licht_da = 0;

void re15_g5a_licht_einsetzen(void *licht_satz)
{
    re15_light_set_t *ls = (re15_light_set_t *)licht_satz;
    re15_light_set_t tmp;
    uint8_t roh[80];
    s_g5a_licht_da = 0;
    if (!ls || !raum_5090() || RE15_LIGHT_MAX_CUTS < RE15_G5A_SCHNITT10 + 1) return;
    memcpy(roh, k_re2_licht9, 40); memcpy(roh + 40, k_re2_licht10, 40);
    if (re15_light_parse(roh, sizeof roh, &tmp) != 0 || tmp.cut_count != 2) return;
    ls->cuts[RE15_G5A_SCHNITT9]  = tmp.cuts[0];
    ls->cuts[RE15_G5A_SCHNITT10] = tmp.cuts[1];
    if (ls->cut_count < RE15_G5A_SCHNITT10 + 1) ls->cut_count = RE15_G5A_SCHNITT10 + 1;
    s_g5a_licht_da = 1;
}

int re15_g5a_licht_schnitt(int cut)
{
    if (re15_g5a_re2_schnitt(cut) < 0) return cut;
    return s_g5a_licht_da ? cut : G5A_LICHT_RE15;      /* ohne eingesetzten RE2-Satz (kein Parse) bleibt der alte Satz 11 */
}

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
    lampen_parken(); s_beben = 0;                        /* NB5 M4 */
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
    lampen_parken(); s_beben = 0;                        /* NB5 M4: @0x01894 Evt_exec(7, sub09) im Bild des Cut_chg(9); sub11 ab G5A_SKRIPT_START */
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
    if (s_g5a.vl == G5A_SKRIPT_START) lampen_start();    /* NB5 M4: erstes Bild von sub11 (C+2, s. Task-Reihenfolge) */
    else lampe_takt();                                   /* NB5 M4: sub11/sub12 je Bild */
    beben_takt(s_g5a.vl);                                /* NB5 M4: sub04 (Evt_exec @0x0189A, erstes Bild C+2) */
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
    s_beben = 0;                                         /* sub04 endet @0x016DC vor der Armierung */
    lampe_takt();                                        /* NB5 M4: sub11/sub12 */
    if (b == RE15_G5A_BILD_SCHNITT10 + 1) lampen_parken();   /* NB5 M4: @0x0193C Evt_exec(7, sub10) im Bild des Cut_chg(10); Platz 7 < 9
                                                          * -> sub10 laeuft im Folgebild, nach sub12/sub11 (Plaetze 3/4) */
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
