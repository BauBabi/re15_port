/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene" (ROOM2090/2091). Herleitung, Belege und Konstanten:
 * include/re15_gatorszene_r41.h, Dossier analysis/befunde_runde41/F_gatorszene.md.
 */
#include "re15_gatorszene_r41.h"

#include <stddef.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ==== 1. Spinnen ================================================================================================ */
static int s_spinnen_weg = 1;
static int s_spinnen_verworfen = 0;

int re15_gatorszene_r41_spinnen_weg_setzen(int an)
{
    int alt = s_spinnen_weg;
    s_spinnen_weg = an ? 1 : 0;
    return alt;
}

int re15_gatorszene_r41_spinnen_verworfen(void) { return s_spinnen_verworfen; }

int re15_gatorszene_r41_spawn_weg(uint16_t room_id, uint8_t type, int16_t x, int16_t z)
{
    if (!s_spinnen_weg) return 0;
    if ((uint16_t)(room_id & 0xFFFEu) != RE15_GSZ_RAUM || type != RE15_GSZ_SPINNE_TYP) return 0;
    if (!((x == RE15_GSZ_SPINNE0_X && z == RE15_GSZ_SPINNE0_Z) ||
          (x == RE15_GSZ_SPINNE1_X && z == RE15_GSZ_SPINNE1_Z))) return 0;      /* Satz-Waechter: nur die RDT-Saetze */
    s_spinnen_verworfen++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X: Spinne 0x25 bei (%d,%d) verworfen (Nutzer: \"Entferne die Spinnen aus dem "
                    "ROOM 2090\"; Form = Kill-Flag-Tor @0x80042120-38)\n", (unsigned)room_id, (int)x, (int)z);
#endif
    return 1;
}

/* ==== 2. Ankunftsszene ============================================================================================ */
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_gator_re2_r41.h"
#include "re15_boss_gator.h"
#include "re15_blink_r41.h"
#include "re15_math.h"
#include "re15_camera.h"   /* Nachbesserung 1 (M2): re15_camera_build_view (Schluessel-Lage im Bild der Kamera) */
#include "re15_light.h"    /* Nachbesserung 1 (M2/M5): NCCT wie der Prop-Zeichner (Neigung unter der Puls-Kappung) */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdlib.h>
#endif
#include "re15_audio.h"   /* re15_audio_start_room_bgm (Musik-Weiche), re15_audio_re2_tuer_laden/_se (Knall), _door_se (Tor) */
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_lock_se.h"
#include "re15_item_discard.h"
#include "gen/knall_bank.inc"      /* k_knall_bank: Satz 0 Tuerknall (RE2 ROOM1060 Se_on 0x0a), Satz 1 Knall ROOM1030 */
#include "gen/re2_door_bank.inc"   /* RE2_DOOR_SE_AUF_KEY (TUERSE.VBS Satz 4 = RE2 ROOM1140 Satz 0x25)                 */
extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);   /* scd_vm.c (sce-1-Weg), wie schluessel_r36.c */
static void musik_takt(void);
static void musik_takt_vor(void) { musik_takt(); }

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbild je Form:
 *   Set            `22 bb ii vv`                         ROOM1150 sub08 @0x01110
 *   Sleep          `09 0a nn nn`                         ROOM1150 sub08 @0x01162
 *   Cut_chg        `29 cc`                               ROOM1150 sub08 @0x01178
 *   Work_set       `2e 01 00` + Nop, Plc_ret `42` + Nop  ROOM1150 sub08 @0x012E0 (Rueckgabe des Spielers)
 *   Cut_auto       `3c 01`                               ROOM1150 sub08 @0x012E2
 *   Ankunfts-Poll  `11 00 08 00 02 00 12 04 21 05 bb 00` ROOM1150 sub08 @0x0112A..@0x01132 (Bit bb statt 0x20)
 *   Sce_em_set     `44 ss tt gg ff 00 p6 kk xx xx yy yy zz zz 00 00 rr rr 00 00` ROOM2020 main00 @0x008F6 (pc[4] = Band)
 *   Door_aot_set   `3b ss 02 31 00 00 <Rechteck 0> <x y z gier> st rm ct ..` ROOM2090 main00 @0x00996 (Nutzlast)
 *   Aot_on         `47 ss`                               ROOM1240 sub02 (Intro-Handoff), ROOM1080 sub07 */
#define GSZ_LE16(v)        (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define GSZ_SET(b,i,v)     0x22, (b), (i), (v)
#define GSZ_SLEEP(n)       0x09, 0x0a, GSZ_LE16(n)
#define GSZ_CUT(c)         0x29, (c)
#define GSZ_CUT_AUTO       0x3c, 0x01
#define GSZ_CUT_OLD        0x2a, 0x00                             /* Cut_old + Nop: ROOM2010 sub03 @0x008F6/@0x008F7 */
#define GSZ_PLC_RET        0x2e, 0x01, 0x00, 0x00, 0x42, 0x00
#define GSZ_WAIT(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define GSZ_EM(s,t,g,p6,x,y,z,band,rot) 0x44, (s), (t), (g), (band), 0x00, (p6), 0xff, GSZ_LE16(x), GSZ_LE16(y), \
                                        GSZ_LE16(z), 0x00, 0x00, GSZ_LE16(rot), 0x00, 0x00
#define GSZ_DOOR           0x3b, RE15_GSZ_TUER_SLOT, 0x02, 0x31, 0x00, 0x00, 0,0,0,0,0,0,0,0, \
                           0,0, 0,0, 0,0, 0,0, 0x01, 0x00, 0x00, 0x00, 0,0,0,0,0,0          /* Ziel per tuer_setzen */
#define GSZ_AOT_ON         0x47, RE15_GSZ_TUER_SLOT
#define GSZ_END            0x01, 0x00
#define GSZ_RIEGEL         GSZ_SET(2, 7, 1), GSZ_SET(1, 27, 1)   /* ROOM1150 sub08 @0x01114/@0x01118 */
#define GSZ_RIEGEL_AUF     GSZ_SET(2, 7, 0), GSZ_SET(1, 27, 0)   /* ROOM1150 sub08 @0x012E4/@0x012E8 */
#define GSZ_ABSCHNITT_ENDE GSZ_SLEEP(1)                          /* irons_tod_1150.c OP_ABSCHNITT_ENDE (Riegel im ersten Spieltakt) */
#define GSZ_DOOR_LEN       32
#define GSZ_DOOR_X         14   /* Nutzlast +0 (scd_vm.c op_door_aot_set nl[0]) */
#define GSZ_DOOR_Y         16
#define GSZ_DOOR_Z         18
#define GSZ_DOOR_YAW       20
#define GSZ_DOOR_STAGE     22
#define GSZ_DOOR_ROOM      23
#define GSZ_DOOR_CUT       24

/* ---- PORT-WAHL der Inszenierung (Nachtrag 6 "Betreff Aligator darfst du alles machen was du willst. Du darfst auch die
 * Startposition des Aligators und die Position der schwimmenden Zombie Leiche ... aendern"), gemessen am Bild (Dossier §1): */
/* Leiche + Szenenkamera. Fassung 1 (leiche_lage.py): obj 2 nach (-5500,-1740,-23700) Gier 384, Cut 1, Anlauf aus dem Norden
 * (Gier 1024) - Rumpfmitte (-4594,-24306), rund 3800 von der Leiter-Ankunft; der Kampf begann mit der Gator-Wurzel 6162 vor
 * Leon, erster Treffer nach 61 (stehend) bzw. 33 Bildern (weglaufend) - Abnahme 3 M1. Nachbesserung 4 (NUTZER-VORGABE Nachtrag
 * 29, 2026-10-25 12:08: "Die Zombie Leiche inklusive der Kette - Schluessel - Aligator Fressen etc. muss weiter weg sein, damit
 * man am Start des Kampfes eine Chance hat"; Sollwert = RE2-Intro-Ende ROOM40A0, tools/r41_gatorszene/probe_re2_intro_ende.c:
 * Wurzel (503,-24100) nach Pos_set @SCD 0x0736 + Clip 6/3 @0x801008ac-948, Leon (-10800,-25800) nach Plc_dest @SCD 0x0286 ->
 * Abstand 11430, vorderster Knochen auf Leons x nach 121 Bildern): Wahl tools/r41_gatorszene/leiche_lage_n4.py - Kette im Wasser
 * des Beckens (gb_becken_draussen), Abstand Kau-Lage <-> Leiter-Ankunft - 700 >= 11430, Leiche ganz im Bild, groesster Wert
 * Bildgroesse^2 x Licht des Schluessels (die Regel von Fassung 1). Die beste Lage liegt in Cut 5 bei (700,-8200) Gier 2432 - 1063
 * neben der zweiten treibenden Leiche obj 3 (Obj_model_set @0x00A8E `2d 03 00 00 02 00 0a 00 00 00 e7 ff 34 f9 01 e3 00 00 78 06`
 * = (-25,-1740,-7423) Gier 1656). Darum ist die Szenen-Leiche obj 3 selbst, an diese Lage gesetzt (Nutzer: "Es schwimmt bereits
 * ein Zombie im Wasser, nimm vielleicht diesen"; Nachtrag 6: ihre Lage darf sich aendern); obj 2 bleibt an seiner Original-Lage
 * (-1725,-21723) @0x00A6C (Cut 0), 13739 entfernt. Szenenkamera Cut 5 (RID @0x060 + 5 * 32: (2754,-3132,-6408) ->
 * (-7992,-18,-10782)), die Kamera, in der auch das Original obj 3 zeigt: ganze Leiche im Bild (62,132)-(261,205), Rumpfmitte
 * (-206,-7594), Kau-Lage (-2077,-9725) = 16968 von der Leiter-Ankunft (Fassung 1: rund 3800). */
/* Nachbesserung 5 (Abnahme 4 M1-M4 = Orchestrator-Entscheid F-E2, 2026-10-25 14:21: "Kampfbeginn = RE2 ... so nah wie moeglich
 * ... nicht darunter", "Schluessel mindestens so gut sichtbar wie in der abgenommenen Ankunftseinstellung", "Fressen ... nicht von
 * hinten, Frontalwert > 0 ... der Freiheitsgrad ist die Lage der Leiche", Rangfolge 1 vor 2 vor 3):
 *  (1) RE2-Zonenbeziehung am Intro-Ende (probe_re2_band_n5.c): Zonenbyte +0x21A aus INIT 0 (`sb zero,538(s0)` @0x80100654),
 *      Wechsel nur ueber die GATOR-Lage (`lw v0,56(s0) / slti v0,v0,-17877` @0x80100990-98, `lw v0,64(s0) / slti v0,v0,-7322`
 *      @0x801009d8-e0); am Intro-Ende Wurzel (503,-24100) Zone 0, Kontakt mit Leon (-10800,-25800) nach 121 Bildern ohne
 *      Zonenwechsel = ohne Dash. Port: der Dash kommt beim Wechsel des Gators IN Leons Beckenzone (gator_re2_r41.c
 *      re15_gr2_dash_zonenwechsel) - Nachbesserung 4 begann mit dem Gator im Nordbecken (Zone 1), Leon an der Leiter in Zone 0:
 *      300 Bilder Dash vor dem Biss (Abnahme 4 M1). Also Kampfbeginn mit dem Gator in Zone 0 (Suedbecken, z <= -18100).
 *  (2) RE2-Sollband (probe_re2_band_n5.c): Abstand 11430 (Intro-Ende Cut 1 + 150) .. 13924 (fruehester Kampfbeginn fuer Leon:
 *      sub03 Plc_ret @SCD 0x02B6 nach Cut 1 + 113), Kontakt 121 .. 158 Bilder. Zone 0 + Abstand >= 11430 heisst: Kampfbeginn im
 *      Ostteil des Suedbeckens.
 *  (3) Schluessel wie abgenommen (Licht >= 168 = Primitivfarbe Ruhe >= 84, >= 10,4 x 9,5 Bildpunkte, ganze Leiche im Bild) geht
 *      in ROOM2090 NUR in Cut 1 bei x -5500..-5200 / z -24700..-23700 (= Fassung 1, 2700..3300 von der Leiter; leiche_lage_n5.py
 *      --m2), also nicht zugleich mit (1) und Nachtrag 29 ("Die Zombie Leiche ... muss weiter weg sein"). Unter (1) liegt das
 *      Licht >= 168 nur noch in Cut 0 (Lichtsatz RDT @0x468 + 40 * 0: Licht bei (2510,-4000,-24087)); dort ist die Leiche nur
 *      mit dem Gator von OSTEN erreichbar (Kette bis zur Kau-Lage; die Kette mit dem ganzen Clip 6 = 10715 passt von Osten nicht
 *      ins Becken: Ostwand x 7200) - darum endet der Durchbruch nach dem Aufsteigen (Clip 6 Bild 0..15: Kopf y +1611 -> -1392)
 *      und dem Vorstoss (Bild 15..31: Wurzel -5880 -> -1627 roh), dann der Biss (s. GSZ_DURCHBRUCH_BIS).
 *  (4) Fressen von vorn: Anlauf nach Westen -> Spur B's FINISHER-Wahl (gator_re2_r41.c re15_gr2_kamera_wahl) findet Cut 1 (bei
 *      (-7740,-25956), Blick nach Osten) vor dem Kopf, unter der Massstab-Grenze 1892 (Tiefe >= 4804).
 * Wahl = Messung mit dem ECHTEN Ablauf (test_r41_gatorszene lagesuche: Szene ueber die Leiter, Boss-Takt mit Klemme, Spur-B-
 * Kamerawahl) ueber 2873 Kandidaten aus leiche_lage_n5.py (Leiche ganz im Bild von Cut 0, Licht >= 168 moeglich, Kette mit
 * Wurzel-Rand 1500 = ueber der Klemme hit_radius_min 1466): alle drei Punkte erfuellen 34 Leichen-Lagen; davon die mit dem
 * groessten Schluessel (Quad-Kasten 11,7 x 12,0 Bildpunkte, 0,87 x Fassung 1) und Abstand am naechsten ueber dem RE2-Wert:
 *   Leiche obj 2 (0,-1740,-26000) Gier 2944 (Original @0x00A6C pc[10..11] `43 f9` = -1725, pc[14..15] `25 ab` = -21723, Gier
 *   `ad 02` = 685), Anlauf-Gier 1952, Durchbruch bis Bild 22 -> Kampfbeginn Gator (3197,-24407) Zone 0, Abstand 11448
 *   (SquareRoot0; RE2 11402), erster Biss nach 131 / Treffer nach 138 Bildern (RE2-Band 121..158), Fress-Cut 1 frontal 4116.
 *   (Anlauf-Gier 1920 gaebe genau 11402 = die Untergrenze; 1952 laesst 46 Rand fuer Rundung zwischen Riegel und exe.) */
#define GSZ_CUT_LEICHE        0         /* RID @0x060 + 0 * 32: (2754,-3132,-26190) -> (-7992,-18,-21564), Lichtsatz @0x468 */
#define GSZ_LEICHE_X_STD      0
#define GSZ_LEICHE_Z_STD      (-26000)
#define GSZ_LEICHE_ROT_Y_STD  2944
#define GSZ_GATOR_GIER_STD    1952      /* Anlauf nach Westen (Vorwaerts (cos, -sin) = (-0,99, -0,15)) */
/* Letztes Durchbruch-Bild (Clip 6): Bild 0..15 steigt der Kopf aus dem Wasser (Rig-y +1611 -> -1392, EMR-Keyframes der
 * RE2-EM23-Bank, Sonde probe_clip6), ab Bild 15 der Vorstoss (Wurzel-Versatz -5880 -> -1627 roh bis Bild 31, groesster Schritt
 * 480); ab Bild 37 gleitet er nur noch aus (bis Bild 124 weitere 1597 x 2/3). Bild 22 = laengster Durchbruch, mit dem die Kette
 * der gewaehlten Lage von Osten ins Becken passt (Wurzel-Rand 1500). Danach der Biss (RE2 FRESSEN P1 Clip 4 @0x801010ec-f4).
 * PORT-WAHL der Inszenierung (Nachtrag 6), RE2 laesst Clip 6 im Intro bis zum Ende laufen (@0x801008ac-c8). */
#define GSZ_DURCHBRUCH_BIS_STD 22
#define GSZ_LEICHE_MITTE_X    1090
#define GSZ_LEICHE_BILDER     90     /* Halt auf der Leiche (1,5 Blinkperioden 0x3c, re15_blink_r41.h) */
#define GSZ_LEICHE2_BILDER    45     /* "erneut Kamera auf Zombie mit Schluessel" vor dem Gator-Signal */
#define GSZ_KNALL_BILDER      45     /* Knall -> Schnitt: irons_tod_1150.c k_p_szene `OP_KNALL(..), OP_SLEEP(45)` */
#define GSZ_MONTAGE_BILDER    150    /* Halt auf den Zombies: Irons-Montage 1130-Schritt "15 + 10 + 150" (irons_tod_1150.c) */
/* 2080-Einstellung (Nachtrag 20): Kamera Cut 2 (RID ROOM2080 @0x0A0 pos (-4832,-19777,-18507) -> (-14827,-14428,-16815)) =
 * Treppenabsatz + oberer Laufgang von Osten (Spur G RE15_K2080_CUT_SCHUSS). Spieler geparkt an SEINER Ankunftslage ueber die
 * Leiter = Nutzlast von ROOM2090 Tuer 0 @0x00996 `22 d9 c0 c7 9c 9b 00 08` (-9950,-14400,-25700) Gier 2048: in BLK-Zone 0
 * von ROOM2080 (@0x0B6C x -14500..-8984, z -27100..-6500), damit der Navigator der Zombies ihn als Ziel nimmt (re15_montage_r38.h
 * Baustein 2, FUN_80039e7c gleiche Zone); in Cut 2 nicht im Bild (65 Grad neben der Blickachse, halber Bildwinkel 37,6). */
#define GSZ_2080_CUT          2
#define GSZ_2080_PARK_X       (-9950)
#define GSZ_2080_PARK_Y       (-14400)  /* Band 8 = -8 * 0x708 */
#define GSZ_2080_PARK_Z       (-25700)
#define GSZ_2080_PARK_YAW     2048

/* ---- Zustand ---------------------------------------------------------------------------------------------------- */
static int      s_zustand = RE15_GSZ_AUS;
static int32_t  s_leon_x, s_leon_y, s_leon_z;    /* Eintrittslage in 2090 (Rueckkehr-Tuersatz) */
static int16_t  s_leon_yaw;
static uint8_t  s_leon_cut;                      /* Eintritts-Cut (Raumlader @0x8001d940 = Tuer-Nutzlast; Leiter = Cut 0) */
static uint8_t  s_prog[640];
static int      s_prog_len = 0;

#define GSZ_FOLGE_MAX 64
static re15_gsz_folge_t s_folge[GSZ_FOLGE_MAX];
static int s_folge_n = 0;
static void folge(char art, int wert, int wert2)
{
    if (s_folge_n < GSZ_FOLGE_MAX) {
        re15_gsz_folge_t *f = &s_folge[s_folge_n++];
        f->art = art; f->raum = (uint16_t)g_current_room_id; f->bild = (unsigned)g_engine.frame_count;
        f->wert = wert; f->wert2 = wert2;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Folge %c ROOM%04X F%u wert=%d/%d zustand=%d\n", art, (unsigned)g_current_room_id,
            (unsigned)g_engine.frame_count, wert, wert2, s_zustand);
#endif
}
int re15_gatorszene_r41_folge_n(void) { return s_folge_n; }
const re15_gsz_folge_t *re15_gatorszene_r41_folge(int i) { return (i >= 0 && i < s_folge_n) ? &s_folge[i] : NULL; }
int re15_gatorszene_r41_zustand(void) { return s_zustand; }

/* ---- Programme --------------------------------------------------------------------------------------------------- */
/* PROGRAMM 1 - ROOM2090/2091 Ankunft: Riegel, Schnitt auf die Leiche (GSZ_CUT_LEICHE) mit dem blinkenden Schluessel, Knall von oben (Nachtrag 17,
 * "erst Knall, dann die Zombies, dann der Schnitt" wie die Irons-Montage), dann Montage nach 2080. */
static const uint8_t k_p1[] = {
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_CUT_LEICHE),
    GSZ_SLEEP(GSZ_LEICHE_BILDER),
    GSZ_SET(5, RE15_GSZ_SIG_KNALL, 1),                   /* Port-Takt: Knall-Bank Satz 0 (irons_tod_1150.c (5,28)-Form) */
    GSZ_SLEEP(GSZ_KNALL_BILDER),
    GSZ_DOOR, GSZ_AOT_ON,
    GSZ_END,
};
/* PROGRAMM 2 - ROOM2080/2081 Montage: die acht Zombies sind durch die Tuer gebrochen und laufen OBEN (Nachtrag 20).
 * Lagen/Typ/Band/Byte 6 = Spur G (re15_kanister2080_r41.h k_zombies, Typ 0x16, Band 8, y -14400 = -8 * 1800, `.. 08 00 01 ..`
 * Stand f78be367; Satzform ROOM2020 sub00 @0x008F6); Gier = Blick auf den Treppenabsatz wie dort. Abweichend: grid 0x0d =
 * ENGAGE ab INIT (STAGE1.BIN `ori v0,zero,0x201` / `sw v0,4(v1)` @0x80100f40-44; Raum-Records ROOM1040 main00 @0x011FC;
 * Port-Vorbild irons_tod_1150.c EM_1130_DURCH) - sie LAUFEN ("oberhalb von der Treppe laufen"), Ziel = der geparkte Spieler an
 * der Leiter; Kill-Flag 0xff = kein Tor (@0x80042128) - Szenenfiguren nur fuer diese Einstellung. Die Treppe betreten sie nicht
 * (Band-Vergleich FUN_8003b0a4 @0x8003b228-3c, Band 8 gegen die Treppen-Baender). */
#define GSZ_2080_ZOMBIES \
    GSZ_EM(0, 0x16, 0x0d, 0x01, -12800, -14400, -19600, 8, 2698), \
    GSZ_EM(1, 0x16, 0x0d, 0x01, -11400, -14400, -20000, 8, 2569), \
    GSZ_EM(2, 0x16, 0x0d, 0x01, -10000, -14400, -19300, 8, 2392), \
    GSZ_EM(3, 0x16, 0x0d, 0x01, -12300, -14400, -18300, 8, 2455), \
    GSZ_EM(4, 0x16, 0x0d, 0x01, -10700, -14400, -17500, 8, 2204), \
    GSZ_EM(5, 0x16, 0x0d, 0x01, -12700, -14400, -15800, 8, 1838), \
    GSZ_EM(6, 0x16, 0x0d, 0x01, -11300, -14400, -15300, 8, 1833), \
    GSZ_EM(7, 0x16, 0x0d, 0x01,  -9900, -14400, -14700, 8, 1819)
static const uint8_t k_p2[] = {
    GSZ_2080_ZOMBIES,                                    /* Raumaufbau: vor dem ersten Bild (Form EM_1130_DURCH) */
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_2080_CUT),
    GSZ_SLEEP(GSZ_MONTAGE_BILDER),
    GSZ_DOOR, GSZ_AOT_ON,
    GSZ_END,
};
/* PROGRAMM 3 - ROOM2090/2091 Rueckkehr: erneut die Leiche, Gator-Signal, warten, Szene gesehen, Riegel auf. */
static const uint8_t k_p3[] = {
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_CUT_LEICHE),
    GSZ_SLEEP(GSZ_LEICHE2_BILDER),
    GSZ_SET(5, RE15_GSZ_SIG_GATOR, 1),
    GSZ_WAIT(RE15_GSZ_SIG_FERTIG),
    GSZ_SET(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1),
    GSZ_PLC_RET,
    GSZ_CUT_AUTO,
    GSZ_RIEGEL_AUF,
    GSZ_END,
};
/* PROGRAMM 4 - ROOM2090/2091 nach dem Tod (Nachbesserung 2, M2): Einstellung auf den blinkenden Schluessel. Den Schnitt auf die
 * Kamera macht der Port im Bild des Todes (key_einstellung, Cut_chg-Zwilling LAB_800402a0 - wie die Kamera-Uebergabe M1 im SELBEN
 * Bild wie Spur B's Rueckschnitt, sonst stuende dessen Zonen-Cut ein Bild lang da); das Programm haelt mit Riegel und kehrt
 * zurueck. FORM: RE1.5 "Kamera auf einen Gegenstand" ROOM2010 sub03 = `29 05` Cut_chg @0x008EE, `2b 02 ff ff` Message_on
 * @0x008F0 (hier: Halt, kein Text), `2a` Cut_old @0x008F6, `00` @0x008F7, `3c 01` Cut_auto @0x008F8 (ROOM1180 sub02
 * @0x00B3C-0x00B46 gleich); Riegel Set(2,7,1)/Set(1,27,1) + Plc_ret + Riegel auf wie ROOM1150 sub08 (@0x01114/@0x01118,
 * @0x012E0, @0x012E4/@0x012E8); Halt = GSZ_LEICHE_BILDER (1,5 Blinkperioden 0x3c, wie die Leichen-Einstellung der Ankunft). */
#define GSZ_KEY_SCHUSS_BILDER GSZ_LEICHE_BILDER
static const uint8_t k_p4[] = {
    GSZ_RIEGEL,
    GSZ_SLEEP(GSZ_KEY_SCHUSS_BILDER),
    GSZ_CUT_OLD,
    GSZ_PLC_RET,
    GSZ_CUT_AUTO,
    GSZ_RIEGEL_AUF,
    GSZ_END,
};
const uint8_t *re15_gatorszene_r41_programm(int nr, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if (nr == 1) { p = k_p1; n = (int)sizeof k_p1; }
    else if (nr == 2) { p = k_p2; n = (int)sizeof k_p2; }
    else if (nr == 3) { p = k_p3; n = (int)sizeof k_p3; }
    else if (nr == 4) { p = k_p4; n = (int)sizeof k_p4; }
    if (out_len) *out_len = n;
    return p;
}

static void tuer_setzen(int door_off, int32_t x, int32_t y, int32_t z, int yaw, uint8_t room, uint8_t cut)
{
    uint8_t *d = s_prog + door_off;
    d[GSZ_DOOR_X] = (uint8_t)(x & 0xff); d[GSZ_DOOR_X + 1] = (uint8_t)((x >> 8) & 0xff);
    d[GSZ_DOOR_Y] = (uint8_t)(y & 0xff); d[GSZ_DOOR_Y + 1] = (uint8_t)((y >> 8) & 0xff);
    d[GSZ_DOOR_Z] = (uint8_t)(z & 0xff); d[GSZ_DOOR_Z + 1] = (uint8_t)((z >> 8) & 0xff);
    d[GSZ_DOOR_YAW] = (uint8_t)(yaw & 0xff); d[GSZ_DOOR_YAW + 1] = (uint8_t)((yaw >> 8) & 0xff);
    d[GSZ_DOOR_STAGE] = 0x01;          /* STAGE2 (Nutzlast +8, ROOM2090 @0x00996 `01 08`) */
    d[GSZ_DOOR_ROOM]  = room;          /* Raum-Byte; die Elza-Variante waehlt der Lader (ROOM2091 @0x00996 traegt ebenfalls `01 08`) */
    d[GSZ_DOOR_CUT]   = cut;
}

static int programm_bauen(int nr)
{
    int n = 0;
    const uint8_t *p = re15_gatorszene_r41_programm(nr, &n);
    if (!p || n > (int)sizeof s_prog) return 0;
    memcpy(s_prog, p, (size_t)n);
    s_prog_len = n;
    const int door_off = n - (GSZ_DOOR_LEN + 2 + 2);       /* Door_aot_set + Aot_on + Evt_end am Ende (Programme 1/2) */
    if (nr == 1)
        tuer_setzen(door_off, GSZ_2080_PARK_X, GSZ_2080_PARK_Y, GSZ_2080_PARK_Z, GSZ_2080_PARK_YAW, 0x08, GSZ_2080_CUT);
    else if (nr == 2)
        tuer_setzen(door_off, s_leon_x, s_leon_y, s_leon_z, s_leon_yaw, 0x09, GSZ_CUT_LEICHE);
    return 1;
}
static const uint8_t *tor_druck(uint16_t room_id);   /* Abschnitt 5 */
const uint8_t *re15_gatorszene_r41_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (event_id == RE15_GSZ_TOR_EREIGNIS || event_id == RE15_GSZ_TOR20A0_EREIGNIS) return tor_druck(room_id);
    if (event_id != RE15_GSZ_EREIGNIS) return NULL;
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    int nr = 0;
    if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_ANKUNFT) nr = 1;
    else if (basis == 0x2080u && s_zustand == RE15_GSZ_MONTAGE) nr = 2;
    else if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_RUECKKEHR) nr = 3;
    else if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_SCHLUESSEL) nr = 4;   /* Nachbesserung 2 (M2) */
    if (!nr || !programm_bauen(nr)) return NULL;
    folge('P', nr, 0);
    return s_prog;
}

static int prop_slot(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* ==== 3. Schluessel ================================================================================================ */
#include "gen/gatorszene_r41_schluessel.inc"   /* tools/r41_gatorszene/schluessel_export.py */

/* Prop-Felder wie Obj_model_set (LAB_80040914): flags pc[6..7] | 1 (@0x80040990-a4), Band pc[4] -> pool+0x82 (@0x8004096c-74).
 * attr 0 = NCCT-beleuchtet (FUN_8002c18c @0x8002c2ec-f8): Voraussetzung des Helligkeitspulses (re15_blink_r41.h). */
#define GSZ_KEY_FLAGS     0x000B    /* ROOM2090 obj 2 @0x00A6C / obj 3 @0x00A8E pc[6..7] `0a 00` | 1 */
#define GSZ_KEY_BAND      0
/* Lage auf dem Ruecken der Leiche, Objektraum der Leiche (Elternobjekt, Anhaenge-Form 0xC0|n @0x80040a84-9c): hoechste Stelle
 * des Rueckens im Leichen-Modell MD1 @0x2054 (y -341 bei x 800..1100, z -100); die Mitte 420 darueber, weil das gekippte
 * Quadrat (180) in y bis +-73 reicht. Kippung zur Szenenkamera, hoechstens 60 Grad, mit dem hoechsten Wert cos(Blick) x Licht
 * UNTER der Puls-Grenze Licht x 0xBE / 0x80 <= 255 (Nachbesserung 1, M5; Blink-Spitze 0xBE der Folge @0x801028fc/@0x8010290c;
 * NCCT-Ausgabe = Licht / 2, light_common.c `IR_c >> 5`; der PC-Zeichner kappt die Primitivfarbe bei 0x80 = Texel x 1,0,
 * render_pc.c psx_prim_to_sdl_vert), Drehfolge main.c pc_prop_rot_q12. Nachbesserung 4: Kamera Cut 5, Lichtsatz RDT @0x468 +
 * 40 * 5 (Umgebung 63/73/63, die drei Lichter bei x -29490/-24699/2000 reichen kaum ins Nordbecken) -> lokal rx 672 / rz -144
 * (59,9 Grad), Bildflaeche cos 0,922, Licht 103,8 (Spitze 154 < 172), ~11,5 x 10,6 Bildpunkte (leiche_lage_n4.py; Fassung 1 Cut 1:
 * rx 544 / rz -464, Licht 169,8, ~10,4 x 9,5). Nachbesserung 5: Kamera Cut 0, Lichtsatz RDT @0x468 + 40 * 0 (Licht bei
 * (2510,-4000,-24087)); groesster projizierter Quad-Kasten bei Licht >= 168 (Primitivfarbe Ruhe >= 84 = abgenommen) und unter der
 * Puls-Grenze -> lokal rx -48 / rz -608, Licht 171,5, Quad-Kasten 11,7 x 12,0 (Fassung 1: 13,3 x 13,8; gemessen an der exe:
 * Ring 6 x 8 .. 7 x 9 Bildpunkte, hellster Pixel 86 .. 110; Fassung 1 8 x 10, 72 .. 108). Lage auf dem Ruecken wie bisher. PORT-WAHL. */
#define GSZ_KEY_AUF_X_STD 1090
#define GSZ_KEY_AUF_Y_STD (-420)
#define GSZ_KEY_AUF_Z_STD (-100)
#define GSZ_KEY_AUF_RX_STD (-48)
#define GSZ_KEY_AUF_RZ_STD (-608)
/* Szenen-Lage (Nachbesserung 5): Standard = die PORT-WAHL oben; re15_gatorszene_r41_lage_setzen nur fuer Sonden/Riegel. */
static const re15_gsz_lage_t k_lage_std = { GSZ_LEICHE_X_STD, GSZ_LEICHE_Z_STD, GSZ_LEICHE_ROT_Y_STD, GSZ_GATOR_GIER_STD,
                                            GSZ_DURCHBRUCH_BIS_STD, GSZ_KEY_AUF_RX_STD, GSZ_KEY_AUF_RZ_STD,
                                            GSZ_KEY_AUF_X_STD, GSZ_KEY_AUF_Y_STD, GSZ_KEY_AUF_Z_STD };
static re15_gsz_lage_t s_lage = { GSZ_LEICHE_X_STD, GSZ_LEICHE_Z_STD, GSZ_LEICHE_ROT_Y_STD, GSZ_GATOR_GIER_STD,
                                  GSZ_DURCHBRUCH_BIS_STD, GSZ_KEY_AUF_RX_STD, GSZ_KEY_AUF_RZ_STD,
                                  GSZ_KEY_AUF_X_STD, GSZ_KEY_AUF_Y_STD, GSZ_KEY_AUF_Z_STD };
void re15_gatorszene_r41_lage_setzen(const re15_gsz_lage_t *l) { s_lage = l ? *l : k_lage_std; }
void re15_gatorszene_r41_lage(re15_gsz_lage_t *out) { if (out) *out = s_lage; }
/* Item-Zone nach dem Tod (re15_aot_set_item_tk_prop = Item_aot_set @0x80040644-Form), Rechteck 1000 x 1000 wie ROOM2090 main00
 * @0x00A12 (`e8 03 e8 03`), AOT-Platz 15: frei in 2090 (main00 0..5, Kamerazonen 48..63). */
#define GSZ_KEY_AOT       15
#define GSZ_KEY_RECHTECK  1000
#define GSZ_WASSER_Y      (-1620)   /* Wasseroberflaeche: sce-8-Zone p0 ROOM2090 main00 @0x009D6 pc[14..15] `ac f9` */
/* Ersatzlage nach dem Laden eines Spielstands (die Todeslage lebt nur im RAM): die Rumpfmitte der gefressenen Leiche. */
static void leiche_mitte_konst(int32_t *x, int32_t *z)
{
    *x = s_lage.x + (int32_t)(((int64_t)re15_cos_q12(s_lage.rot_y) * GSZ_LEICHE_MITTE_X) >> 12);
    *z = s_lage.z - (int32_t)(((int64_t)re15_sin_q12(s_lage.rot_y) * GSZ_LEICHE_MITTE_X) >> 12);
}

static int32_t s_key_x = 0, s_key_y = GSZ_WASSER_Y, s_key_z = 0;
static int16_t s_key_rx = 0, s_key_rz = 0;      /* Neigung zur Kamera (Nachbesserung 1, M2), Welt (parent -1) */
static int     s_key_lage_ok = 0;               /* Lage + Neigung aus dem Tod (nur RAM) */
static int     s_key_neigung_offen = 0;         /* Ersatzlage nach dem Laden: Neigung im ersten Takt (s. key_bei_leiche) */
static int     s_key_cut = -1;                  /* Kamera, fuer die Lage/Neigung gewaehlt wurde (Messzeile/Riegel) */

int re15_gatorszene_r41_obj_id(uint16_t room_id)
{
    return ((room_id & 0xFFFEu) == RE15_GSZ_RAUM) ? RE15_GSZ_KEY_OBJ : -1;
}
const uint8_t *re15_gatorszene_r41_md1_bytes(uint16_t room_id, int *n)
{
    if (re15_gatorszene_r41_obj_id(room_id) < 0) { if (n) *n = 0; return NULL; }
    if (n) *n = (int)sizeof re15_gsz_key_md1;
    return re15_gsz_key_md1;
}
const uint8_t *re15_gatorszene_r41_tim_bytes(uint16_t room_id, int *n)
{
    if (re15_gatorszene_r41_obj_id(room_id) < 0) { if (n) *n = 0; return NULL; }
    if (n) *n = (int)sizeof re15_gsz_key_tim;
    return re15_gsz_key_tim;
}
const uint8_t *re15_gatorszene_r41_name(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM ? re15_gsz_key_name : NULL; }
const uint8_t *re15_gatorszene_r41_desc(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM ? re15_gsz_key_desc : NULL; }
int re15_gatorszene_r41_ist_schluessel(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM; }
void re15_gatorszene_r41_icon_einsetzen(uint8_t *itemall, int size)
{
    if (itemall && (RE15_GSZ_KEY_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_GSZ_KEY_ITEM * 1200, re15_gsz_key_icon, 1200);
}
void re15_gatorszene_r41_bild_einsetzen(uint8_t *itps, int size)
{
    if (itps && (RE15_GSZ_KEY_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_GSZ_KEY_ITEM * 0x3000, re15_gsz_key_itps, 0x3000);
}

/* Prop obj 4 anlegen (falls noch nicht da) - Felder wie dining_r38.c schluessel_1011. */
static int key_prop(void)
{
    int k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        k = (int)g_scd.prop_count++;
        memset(&g_scd.props[k], 0, sizeof g_scd.props[k]);
        g_scd.props[k].obj_id   = RE15_GSZ_KEY_OBJ;
        g_scd.props[k].obj_type = 0;
        g_scd.props[k].flags    = GSZ_KEY_FLAGS;
        g_scd.props[k].band     = GSZ_KEY_BAND;
        g_scd.props[k].attr     = 0;
    }
    return k;
}
/* Waehrend die Szene aussteht: Schluessel auf dem Ruecken der Leiche, Kind von obj 3 (wippt mit ihr, sub03 @0x00B5E). */
static void key_auf_leiche(uint16_t room_id)
{
    int k = key_prop();
    if (k < 0) return;
    g_scd.props[k].active = 1;
    g_scd.props[k].parent_obj = RE15_GSZ_LEICHE_OBJ;
    g_scd.props[k].x = s_lage.key_x; g_scd.props[k].y = s_lage.key_y; g_scd.props[k].z = s_lage.key_z;
    g_scd.props[k].rot_x = s_lage.key_rx; g_scd.props[k].rot_y = 0; g_scd.props[k].rot_z = s_lage.key_rz;
    re15_blink_r41_setzen(room_id, RE15_GSZ_KEY_OBJ, 1);
}
/* Nach dem Tod: Schluessel liegt bei der Leiche (Weltraum), Item-Zone mit Taken-Bit 112 und Prop-Verstecken. */
static void key_bei_leiche(uint16_t room_id)
{
    if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GENOMMEN)) return;
    int k = key_prop();
    if (k < 0) return;
    if (!s_key_lage_ok) {
        /* Nach dem Laden eines Spielstands (die Todeslage lebt nur im RAM): Ersatzlage = Rumpfmitte der gefressenen Leiche;
         * die Neigung zur Kamera rechnet der erste Takt (re15_gatorszene_r41_tick) - beim Raumaufbau steht noch der Lichtsatz
         * des VORIGEN Raums (room_common.c Schritt 10 parst ihn erst nach scd_room_reenter). */
        leiche_mitte_konst(&s_key_x, &s_key_z);
        s_key_y = GSZ_WASSER_Y; s_key_rx = 0; s_key_rz = 0;
        s_key_neigung_offen = 1;
    }
    g_scd.props[k].active = 1;
    g_scd.props[k].parent_obj = -1;
    g_scd.props[k].x = s_key_x; g_scd.props[k].y = s_key_y; g_scd.props[k].z = s_key_z;
    g_scd.props[k].rot_x = s_key_rx; g_scd.props[k].rot_y = 0; g_scd.props[k].rot_z = s_key_rz;
    re15_blink_r41_setzen(room_id, RE15_GSZ_KEY_OBJ, 1);
    re15_aot_set_item_tk_prop(GSZ_KEY_AOT, s_key_x, s_key_z, GSZ_KEY_RECHTECK / 2, GSZ_KEY_RECHTECK / 2,
                              RE15_GSZ_KEY_ITEM, 1, RE15_GSZ_BIT_GENOMMEN, RE15_GSZ_KEY_OBJ);
    g_aot.slots[GSZ_KEY_AOT].band = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X: Water Key (Item 0x%02X, Bits (9,%d)/(9,%d)) bei der Leiche (%d,%d,%d) Neigung rx %d rz %d"
                    " (Kamera %d%s), Zone Slot %d\n",
            (unsigned)room_id, RE15_GSZ_KEY_ITEM, RE15_GSZ_BIT_LIEGT, RE15_GSZ_BIT_GENOMMEN, s_key_x, s_key_y, s_key_z,
            (int)s_key_rx, (int)s_key_rz, s_key_cut, s_key_neigung_offen ? ", Neigung folgt im ersten Takt" : "", GSZ_KEY_AOT);
#endif
}

/* ---- Raumaufbau -------------------------------------------------------------------------------------------------- */
/* Erster Abschnitt eines eben gestarteten Fadens SOFORT (Raumaufbau): Form elzazombies_r39.c / irons_tod_1150.c
 * erster_abschnitt_sofort = Init-Lauf FUN_8003ef6c -> FUN_8003f0a0 @0x8003f018 vor der Einblendung @0x8001cbb8..@0x8001cc28. */
static void erster_abschnitt_sofort(int slot)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) return;
    uint8_t aktiv[SCD_THREAD_COUNT];
    for (int s = 0; s < SCD_THREAD_COUNT; s++) { aktiv[s] = g_scd.threads[s].active; if (s != slot) g_scd.threads[s].active = 0; }
    scd_vm_set_room_init(1);
    scd_vm_tick();
    scd_vm_set_room_init(0);
    for (int s = 0; s < SCD_THREAD_COUNT; s++) if (s != slot) g_scd.threads[s].active = aktiv[s];
}


/* Leiche obj 3: Lage der Szene (GSZ_LEICHE_*, leiche_lage_n4.py) - nur waehrend die Szene noch aussteht. */
static void leiche_lage(void)
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k < 0) return;
    g_scd.props[k].x = s_lage.x;
    g_scd.props[k].z = s_lage.z;
    g_scd.props[k].rot_y = s_lage.rot_y;
}
static void leiche_weg(void)
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k >= 0) g_scd.props[k].active = 0;
    k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k >= 0) g_scd.props[k].active = 0;
}

static void tor_sperren(uint16_t room_id);   /* Abschnitt 5 */
static int s_einst_slot;   /* Einstellung auf den Schluessel (Abschnitt "Schluessel nach dem Tod") */
static int s_r1_cut = -1;  /* Nachbesserung 3: Cut im ersten R1-Takt (Abschnitt "Schluessel-Schnitt statt R1-Rueckschnitt") */
static int s_r1_lief = 0;  /* Nachbesserung 3: R1 lief in DIESEM Raumbesuch (key_tod_pruefen) */
void re15_gatorszene_r41_install(uint16_t room_id)
{
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    s_einst_slot = -1;                                      /* Nachbesserung 2: Raumaufbau setzt die SCD-Faeden neu */
    s_r1_cut = -1; s_r1_lief = 0;                           /* Nachbesserung 3: kein R1 im neuen Raum */
    tor_sperren(room_id);
    if (basis == RE15_GSZ_RAUM) {
        if (s_zustand == RE15_GSZ_MONTAGE) {                /* zurueck aus 2080: Programm 3 */
            s_zustand = RE15_GSZ_RUECKKEHR;
            leiche_lage();
            key_auf_leiche(room_id);
            if (scd_event_fire(RE15_GSZ_EREIGNIS) < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 3, 0); return; }
            return;
        }
        if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 1); s_zustand = RE15_GSZ_AUS; }
        if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN)) {
            leiche_weg();                                   /* gefressen */
            if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT)) key_bei_leiche(room_id);
            return;
        }
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        s_leon_x = pl->x; s_leon_y = pl->y; s_leon_z = pl->z; s_leon_yaw = pl->rot_y;
        s_leon_cut = (uint8_t)g_scd.cam_id;               /* scd_room_setup.c: g_scd.cam_id = Eintritts-Cut vor den Installern */
        s_folge_n = 0;
        leiche_lage();
        key_auf_leiche(room_id);
        s_zustand = RE15_GSZ_ANKUNFT;
        folge('A', (int)s_leon_x, (int)s_leon_z);
        if (scd_event_fire(RE15_GSZ_EREIGNIS) < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 1, 0); }
        return;
    }
    if (basis == 0x2080u) {
        if (s_zustand == RE15_GSZ_ANKUNFT) {                /* Montage-Schritt: Programm 2 */
            s_zustand = RE15_GSZ_MONTAGE;
            int slot = scd_event_fire(RE15_GSZ_EREIGNIS);
            if (slot < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 2, 0); return; }
            erster_abschnitt_sofort(slot);
            int n = 0;
            for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type) n++;
            folge('M', n, (int)g_scd.cam_id);
            return;
        }
        if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 2); s_zustand = RE15_GSZ_AUS; }
        return;
    }
    if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 3); s_zustand = RE15_GSZ_AUS; }
}

/* ---- Gator-Ablauf ------------------------------------------------------------------------------------------------ */
enum { GP_AUS = 0, GP_DURCHBRUCH, GP_BISS, GP_SCHUETTELN, GP_KAUEN, GP_WENDE, GP_SCHREI, GP_ENDE };
static struct {
    int     phase;
    int     t;                      /* Bilder in der Phase */
    int32_t anker_x, anker_z;       /* FRESSEN-Anker (FUN_80015b94 @0x80101110-34): Wurzel - R*s*off */
    int     leiche_dran;            /* Leiche haengt am Kopf (ab Clip-4-Bild 13) */
    int     leiche_weg;             /* verschlungen */
    int     kampf_gemeldet;
} s_ga;
int re15_gatorszene_r41_gator_phase(void) { return s_ga.phase; }
/* Nachbesserung 5 (M1/M3/M4): Messgroessen der Szene (nur RAM, Pruefhaken re15_gatorszene_r41_kampfbeginn/_kontakt/_fress_kamera). */
static int      s_fress_cut = -1, s_fress_frontal = 0;    /* FINISHER-Schnitt der Spur B und sein Frontalwert (gr2_frontal-Form) */
static int      s_kb_ok = 0, s_kb_zone_g = -1, s_kb_zone_s = -1, s_kb_takt = 0, s_kontakt = -1;
static int32_t  s_kb_abstand = 0;
int re15_gatorszene_r41_kampfbeginn(int32_t *abstand, int *zone_gator, int *zone_spieler)
{
    if (abstand) *abstand = s_kb_abstand;
    if (zone_gator) *zone_gator = s_kb_zone_g;
    if (zone_spieler) *zone_spieler = s_kb_zone_s;
    return s_kb_ok;
}
int re15_gatorszene_r41_kontakt(void) { return s_kontakt; }
int re15_gatorszene_r41_fress_kamera(int *frontal) { if (frontal) *frontal = s_fress_frontal; return s_fress_cut; }

static inline int32_t q12m(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 12); }
static int32_t ga_scale(const re15_actor_t *e) { return e->render_scale_q12 ? e->render_scale_q12 : 4096; }

/* Keyframe-Index eines Clip-Bildes (0x8000-Marker = Tween-Wort; Form gator_re2_r41.c gr2_kf). */
static int ga_kf(const re15_emd_animation_t *an, int clip, int frame)
{
    if (!an || clip < 0 || clip >= an->clip_count) return -1;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (frame < 0 || frame >= c->frame_count) return -1;
    int fi = c->first_frame + frame, fend = c->first_frame + c->frame_count;
    while ((an->frames[fi] & 0x8000u) && fi + 1 < fend) fi++;
    return (int)(an->frames[fi] & 0xfffu);
}
/* Kumulierter Wurzelversatz (Keyframe-Bytes 6..11, FUN_80015e7c) eines Clip-Bildes, Rig-Raum. */
static void ga_off(const re15_enemy_bank_t *bk, int clip, int frame, int32_t o[3])
{
    int16_t a = 0, b = 0, c = 0;
    o[0] = o[1] = o[2] = 0;
    int kf = ga_kf(&bk->anim, clip, frame);
    if (kf < 0) return;
    re15_emd_get_keyframe_speed(&bk->skel, kf, &a, &b, &c);
    o[0] = a; o[1] = b; o[2] = c;
}
/* Rig-Vektor -> Welt-Versatz (x' = c*x + s*z, z' = -s*x + c*z wie re15_skel_bone_to_world), mit Zeichen-Skala. */
static void ga_rig_welt(const re15_actor_t *e, int yaw, const int32_t v[3], int32_t out[2])
{
    int32_t s = ga_scale(e);
    int32_t c = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int32_t vx = q12m(v[0], s), vz = q12m(v[2], s);
    out[0] = (int32_t)(((int64_t)c * vx + (int64_t)sn * vz) >> 12);
    out[1] = (int32_t)((-(int64_t)sn * vx + (int64_t)c * vz) >> 12);
}
/* Clipwechsel FUN_8001a330(em, clip, n, 0): Bild 0, Ueberblendung n (Zaehler +0x14E = n @0x8001a364, Schritt 4096/(n+1)
 * @0x8001a37c-ac; Abbau je Takt im Boss-Takt @0x800299c0-cc) - Form gator_re2_r41.c gr2_blend. */
static void ga_clip(re15_actor_t *e, int clip, int frame, int n)
{
    e->motion = (int16_t)clip;
    e->anim_frame = frame;
    e->anim_frac = (uint8_t)n;
    e->anim_blend_rate = (uint16_t)(n > 0 ? 4096 / (n + 1) : 0);
}
/* Ein Bild authored Wurzelbewegung (FUN_8001a330 -> FUN_80015e7c + FUN_800152c8): Delta off[f] - off[f-1] gedreht und
 * skaliert auf die Wurzel; Bild 0 = Neu-Anker ohne Schritt (gator_re2_r41.c gr2_wurzel_schritt / gr2_clip_tick).
 * Rueckgabe 2 = letztes Bild liegt an. */
static int ga_schritt(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int clip = e->motion;
    int n = bk->anim.clips[clip].frame_count;
    int f = (int)e->anim_frame + 1;
    if (f >= n - 1) { e->anim_frame = n - 1; return 2; }
    int32_t a[3], b[3];
    ga_off(bk, clip, f, b); ga_off(bk, clip, f - 1, a);
    int32_t v[3] = { b[0] - a[0], 0, b[2] - a[2] }, w[2];
    ga_rig_welt(e, e->rot_y, v, w);
    e->x += w[0]; e->z += w[1];
    e->anim_frame = f;
    return 1;
}
/* Absolute Lage vom FRESSEN-Anker (FUN_80015cb8: Lage = Anker + R*off[f], `sw v0,56(s1)` @0x80015d68 / `sw v0,64(s1)`
 * @0x80015d90), fuer P1 ab dem Koppelbild sowie P2/P3 (@0x80101174 / @0x801011cc). */
static void ga_anker_lage(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t o[3], w[2];
    ga_off(bk, e->motion, (int)e->anim_frame, o);
    int32_t v[3] = { o[0], 0, o[2] };
    ga_rig_welt(e, e->rot_y, v, w);
    e->x = s_ga.anker_x + w[0]; e->z = s_ga.anker_z + w[1];
}
static void ga_anker_setzen(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t o[3], w[2];
    ga_off(bk, e->motion, (int)e->anim_frame, o);
    int32_t v[3] = { o[0], 0, o[2] };
    ga_rig_welt(e, e->rot_y, v, w);
    s_ga.anker_x = e->x - w[0]; s_ga.anker_z = e->z - w[1];
}

/* FUN_8001569c (RE2 PSX.EXE @0x8001569c, Decompilat RE2_Quellcode_V2/FUN_8001569c.c; Port-Zwilling enemy_ai_boss_g5.c
 * g5_yaw_toward): Gier auf ziel mit fester Schrittweite rate. Negative rate = auf ziel + 0x800. Rueckgabe 1 = eingerastet
 * (Zweig `(rate + ziel - gier) & 0xfff < 2 * rate`: gier := ziel). Sonst gier -= rate, und bei `(..) & 0xfff < 0x801` += 2*rate. */
static int ga_drehen(re15_actor_t *e, int32_t ziel, int32_t rate)
{
    if ((int16_t)rate < 0) { rate = -(int16_t)rate; ziel = (ziel + 0x800) & 0xfff; }
    const uint32_t d = (uint32_t)((rate + (ziel - (int32_t)(uint16_t)e->rot_y)) & 0xfff);
    const int16_t zurueck = (int16_t)(e->rot_y - (int16_t)rate);
    if ((int32_t)d < rate * 2) { e->rot_y = (int16_t)ziel; return 1; }
    e->rot_y = zurueck;
    if (d < 0x801u) e->rot_y = (int16_t)(zurueck + (int16_t)rate * 2);
    e->rot_y = (int16_t)(e->rot_y & 0xfff);          /* Port: Gier 0..4095 (RE2 laesst das Halbwort ueberlaufen, mod 4096 gleich) */
    return 0;
}

/* Weltlage des Kopfes (Part 6) einer Probe-Pose (Clip, Bild) an der Lage (x,z) mit Gier yaw. */
static int ga_kopf_probe(const re15_actor_t *e, int clip, int frame, int32_t x, int32_t z, int yaw, int32_t kopf[3])
{
    re15_actor_t b = *e;
    b.x = x; b.z = z; b.rot_y = (int16_t)yaw; b.motion = (int16_t)clip; b.anim_frame = frame;
    b.anim_frac = 0; b.anim_prev_valid = 0;
    return re15_gr2_bone_welt_pub(&b, 6, kopf);
}

/* Szenen-Gier = s_lage.gier (PORT-WAHL-Block oben; Nachbesserung 4: 3552 aus dem Westkanal ins Nordbecken, Fassung 1: 1024 aus
 * dem Norden). */
/* Biss: Kopf (Part 6) im Koppelbild 13 von Clip 4 (RE2 FRESSEN P1 @0x80101104-34) auf dem Rumpf der Leiche. */
#define GSZ_KOPPEL_BILD       13
#define GSZ_WENDE_RATE        75     /* RE2 R1 Ziel-Gier 75/Bild (FUN_8001569c @0x801025f8-608, `addiu a2,zero,75` @0x80102608) */
/* PORT-WAHL (Notbremse der Wende, Nachbesserung 1 M6; die Wende vor dem Schrei ist Nutzer-Inszenierung Nachtrag 6 "Macht danach
 * Richtung Leon eine schrei und Schuettel Animation", RE2 R4 @0x80102bd0 selbst dreht nicht): FUN_8001569c rastet ein, sobald
 * der Rest kleiner als ein Schritt ist (`(rate + ziel - gier) & 0xfff < 2 * rate`), und waehlt die Richtung am Halbkreis
 * (`< 0x801`). Die groesste Wende ist also 0x800, sie braucht hoechstens ceil(0x800 / 75) = 28 Bilder - nach so vielen Bildern
 * ist das Ziel bei ruhendem Leon (Riegel Set(2,7,1) haelt ihn) immer erreicht. Die Grenze greift nur, wenn sich das Ziel je
 * Bild weiter dreht als die Wende (Gator kreist dicht um Leon) - dann bricht sie die Wende nach derselben Zahl Bilder ab. */
#define GSZ_WENDE_MAX         ((0x800 + GSZ_WENDE_RATE - 1) / GSZ_WENDE_RATE)
#define GSZ_SCHREI_SE         0      /* RE2 R4: `addu a0,zero,zero` @0x80102c0c + `jal 0x8005bd6c` @0x80102c10 (ENEMSE Bank 17) */
#define GSZ_SCHREI_BLEND      3      /* RE2 R4: Clip 10 mit `addiu a2,zero,3` @0x80102c44 */
#define GSZ_SCHWIMM_BLEND     32     /* RE2 Sub 1: Clip 0 Blend 32 @0x80100c38 */
#define GSZ_BISS_CD           110    /* +0x238 = 110 nach dem RE2-Intro @0x8010091c-20 (re15_gr2_biss_cd) */

static void leiche_punkt(int32_t p[3])
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k < 0) { leiche_mitte_konst(&p[0], &p[2]); p[1] = -1740; return; }
    int ry = g_scd.props[k].rot_y;
    p[0] = g_scd.props[k].x + q12m(re15_cos_q12(ry), GSZ_LEICHE_MITTE_X);
    p[1] = g_scd.props[k].y;
    p[2] = g_scd.props[k].z - q12m(re15_sin_q12(ry), GSZ_LEICHE_MITTE_X);
}

/* Buehne: Lage S und Gier des Clip-6-Starts so, dass der Kopf im Koppelbild 13 von Clip 4 auf dem Leichen-Rumpf liegt
 * (RE2 FRESSEN P0 setzt den Gator relativ zum Opfer, @0x80101048-84). Kette: S --Clip 6 Bild 1..bis--> A --Clip 4 Bild
 * 1..13--> Koppelbild; Kopf(13) = A + Weg4 + Kopf-Versatz. Nachbesserung 5: bis = s_lage.durchbruch_bis (vorher 124 = Clip-Ende). */
static int ga_durchbruch_bis(const re15_enemy_bank_t *bk)
{
    const int n6 = bk->anim.clips[6].frame_count;
    return (s_lage.durchbruch_bis > 0 && s_lage.durchbruch_bis < n6 - 1) ? s_lage.durchbruch_bis : n6 - 1;
}
static void ga_buehne(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t c[3]; leiche_punkt(c);
    const int yaw = s_lage.gier;
    int32_t kopf[3] = { 0, 0, 0 };
    ga_kopf_probe(e, 4, GSZ_KOPPEL_BILD, 0, 0, yaw, kopf);      /* Kopf relativ zur Wurzel im Koppelbild */
    int32_t a0[3], a1[3], b0[3], b1[3], w4[2], w6[2];
    ga_off(bk, 4, 0, a0); ga_off(bk, 4, GSZ_KOPPEL_BILD, a1);
    int32_t v4[3] = { a1[0] - a0[0], 0, a1[2] - a0[2] };
    ga_rig_welt(e, yaw, v4, w4);
    ga_off(bk, 6, 0, b0); ga_off(bk, 6, ga_durchbruch_bis(bk), b1);
    int32_t v6[3] = { b1[0] - b0[0], 0, b1[2] - b0[2] };
    ga_rig_welt(e, yaw, v6, w6);
    int32_t ax = c[0] - kopf[0] - w4[0], az = c[2] - kopf[2] - w4[1];
    e->x = ax - w6[0]; e->z = az - w6[1];
    e->rot_y = (int16_t)yaw;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Buehne: Leiche (%d,%d) Kopf13-Versatz (%d,%d) Weg4 (%d,%d) Weg6 (%d,%d) -> A (%d,%d) S (%d,%d) Gier %d\n",
            c[0], c[2], kopf[0], kopf[2], w4[0], w4[1], w6[0], w6[1], ax, az, e->x, e->z, yaw);
#endif
}

/* SE des Gators (ENEMSE Bank 17, Paar-Tabelle @0x800A7400) - vom PC-Teil angemeldet (wie re15_gator_audio_hook). */
static void (*s_se_fn)(int, int) = NULL;
static void (*s_bank_fn)(int) = NULL;
void re15_gatorszene_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int)) { s_se_fn = se_fn; s_bank_fn = bank_fn; }
static void ga_se(int id)
{
    if (s_bank_fn) s_bank_fn(17);                            /* GATOR_ENEMSE_BANK (enemy_ai_boss_gator.c) */
    if (s_se_fn) s_se_fn(id, 0);
}

static re15_actor_t s_opfer;                                  /* Messpunkt fuer die Kamerawahl (nur Lage) */
static const re15_actor_t *leiche_als_opfer(void)
{
    int32_t c[3]; leiche_punkt(c);
    memset(&s_opfer, 0, sizeof s_opfer);
    s_opfer.x = c[0]; s_opfer.y = c[1]; s_opfer.z = c[2];
    return &s_opfer;
}

/* Spielfigur im Bild von Cut cut? Fuss (pl->y) UND Kopf (eine Bandhoehe 0x708 hoeher, Baender je -1800: ROOM2090 Tuer 0
 * @0x00996 Nutzlast-y 0xC7C0 = -14400 = 8 Baender) muessen auf dem Schirm liegen (Projektion wie gator_re2_r41.c gr2_im_bild,
 * re15_camera_build_view). Rueckgabe 1/0, Bildpunkt des Fusses nach sx/sy. Mess-/Pruefgroesse der Uebergabe (M1). */
static int spieler_im_bild(int cut, int *sx, int *sy)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t fuss[3] = { pl->x, pl->y, pl->z }, kopf[3] = { pl->x, pl->y - 0x708, pl->z };
    int kx = -1, ky = -1;
    const int a = re15_gr2_im_bild_cut(cut, fuss, sx, sy);
    const int b = re15_gr2_im_bild_cut(cut, kopf, &kx, &ky);
    return a && b;
}
int re15_gatorszene_r41_spieler_im_bild(int cut) { int sx, sy; return spieler_im_bild(cut, &sx, &sy); }

/* Szenenende (Nachbesserung 1, M1): die Kamera geht an die Spielerlage zurueck. FORM = Original-Szenenende ROOM1150 sub08:
 * Cut_chg auf den Cut des Spielers `29 05` @0x012C4 vor Plc_ret `42` @0x012E0 und Cut_auto `3c 01` @0x012E2 (die beiden
 * letzten macht Programm 3 im naechsten Bild). Ziel = der Eintritts-Cut (s_leon_cut; Leiter ROOM2080 Slot 1 @0x00BC2 Ziel-Cut
 * 0); der Spieler steht an seiner Eintrittslage (Montage-Tuer von Programm 2). Ohne den Schnitt blieb die Zonen-Zustands-
 * maschine auf P3s Leichen-Cut (Fassung 1: Cut 1; eine Zone feuert nur mit cam_from = angezeigter Cut, `lbu v1,22(a0)` @0x80014254), in
 * dem keine Zone die Spielerlage traegt - gemessen Abnahme 0 Lauf L1: F405-F699 cam=1, Leon hinter der Kamera. Im SELBEN
 * Bild wie re15_gr2_kamera_frei (dessen Rueckschnitt auf den Leichen-Cut sonst ein Bild lang zu sehen waere: SCD-Takt vor dem
 * Spielschritt, main.c). Cut_chg-Zwilling = LAB_800402a0 wie scd_vm.c op_cut_chg: alter Cut @0x800402c0/@0x800402e4,
 * work_vars[0x0C] @0x800402ec, work_vars[0x0A] @0x800402fc, Bit 0x100 (Zonenkamera aus) @0x800402d4. */
static void cut_chg_zwilling(int cut)
{
    g_scd.cam_id_prev        = (uint8_t)g_scd.work_vars[0x0A];   /* @0x800402c0 / @0x800402e4 */
    g_scd.cam_id             = (uint8_t)cut;
    g_scd.cam_arg2           = 0;
    g_scd.cam_arg3           = 0;
    g_scd.cam_change_pending = 1;
    g_scd.work_vars[0x0C]    = g_scd.work_vars[0x0A];            /* @0x800402ec */
    g_scd.work_vars[0x0A]    = (int16_t)cut;                     /* @0x800402fc */
    g_scd.cut_auto_enabled   = 0;                                /* @0x800402d4 `ori ...,0x100` */
}
static void kam_uebergabe(void)
{
    const int cut = (int)s_leon_cut;
    cut_chg_zwilling(cut);
    int sx = -1, sy = -1;
    const int im = spieler_im_bild(cut, &sx, &sy);
    folge('C', cut, im);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Kamera-Uebergabe: Cut %d (Eintritts-Cut), Spielfigur %s (Fuss-Bildpunkt %d,%d)\n",
            cut, im ? "im Bild" : "NICHT im Bild", sx, sy);
#endif
}

/* ---- Nachbesserung 5: Messungen im Mass des RE2-Sollbands (tools/r41_gatorszene/probe_re2_band_n5.c) ----------------------
 * Beckenzone wie enemy_ai_boss_gator.c gb_zone (die drei sce-8-Wasserzonen von ROOM2090 main00 @0x009D6/@0x009EA/@0x009FE:
 * Suedbecken z bis -18100, Nordbecken ab -14500, dazwischen der Westkanal-Streifen); der Dash der Spur B kommt beim Wechsel des
 * Gators IN Leons Zone (gator_re2_r41.c re15_gr2_dash_zonenwechsel) = Gegenstueck der RE2-Zonenlinien @0x80100990-ec. */
#define GSZ_ZONE_SUED_Z   (-18100)  /* @0x009D6 Aot_set sce 8: z -27000 + Tiefe 8900 */
#define GSZ_ZONE_NORD_Z   (-14500)  /* @0x009EA Aot_set sce 8: z -14500 */
static int gsz_zone(int32_t z) { return (z <= GSZ_ZONE_SUED_Z) ? 0 : (z >= GSZ_ZONE_NORD_Z) ? 1 : 2; }
/* Frontalwert der Fress-Einstellung = gator_re2_r41.c gr2_frontal: cos(Blickachse Bone 1 -> Bone 6, Kopf -> Kamera) in xz, Q12. */
static void fress_kamera_messen(const re15_actor_t *e, int cut)
{
    s_fress_cut = cut; s_fress_frontal = 0;
    int32_t k6[3], k1[3];
    if (cut < 0 || !g_room_rdt_ok || !g_room_rdt.cuts || cut >= g_room_rdt.cut_count) return;
    if (!re15_gr2_bone_welt_pub(e, 6, k6) || !re15_gr2_bone_welt_pub(e, 1, k1)) return;
    const int64_t ax = k6[0] - k1[0], az = k6[2] - k1[2];
    const int64_t cx = (int64_t)g_room_rdt.cuts[cut].pos_x - k6[0], cz = (int64_t)g_room_rdt.cuts[cut].pos_z - k6[2];
    const uint32_t la = re15_squareroot0((uint32_t)(ax * ax + az * az)), lc = re15_squareroot0((uint32_t)(cx * cx + cz * cz));
    if (la && lc) s_fress_frontal = (int)(((ax * cx + az * cz) * 4096) / ((int64_t)la * lc));
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Fress-Einstellung: Cut %d frontal %d (Kopf (%d,%d), Achse (%d,%d))\n", cut, s_fress_frontal,
            k6[0], k6[2], (int)ax, (int)az);
#endif
}
/* Kampfbeginn (Bild der Kamera-Uebergabe = Folge C): Abstand Wurzel <-> Spielfigur (SquareRoot0 wie RE2 +0x1F0 `jal 0x8008d2f4`
 * @0x800265d4) und die Beckenzonen. */
static void kampfbeginn_messen(const re15_actor_t *e)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int64_t dx = (int64_t)e->x - pl->x, dz = (int64_t)e->z - pl->z;
    s_kb_abstand = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
    s_kb_zone_g = gsz_zone(e->z); s_kb_zone_s = gsz_zone(pl->z);
    s_kb_ok = 1; s_kb_takt = 0; s_kontakt = -1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Kampfbeginn F%u: Gator (%d,%d) Gier %d Zone %d, Spielfigur (%d,%d) Zone %d, Abstand %d\n",
            (unsigned)g_engine.frame_count, e->x, e->z, (int)e->rot_y, s_kb_zone_g, pl->x, pl->z, s_kb_zone_s, (int)s_kb_abstand);
#endif
}
/* Kontakt-Mass (gleich auf beiden Seiten, probe_re2_band_n5.c): erstes Bild nach dem Kampfbeginn, in dem ein Knochen die
 * Spielfigur laengs der Schwimmrichtung erreicht: max_b <P_b - L, h> >= 0, h = (cos Gier, -sin Gier) (FUN_800152c8-Konvention). */
static void kontakt_pruefen(const re15_actor_t *e)
{
    const re15_enemy_bank_t *bk = re15_enemy_find(e->type);
    if (!bk || !bk->ok) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int64_t hx = re15_cos_q12(e->rot_y), hz = -re15_sin_q12(e->rot_y);
    int64_t best = 0; int bb = -1;
    for (int b = 0; b < bk->skel.bone_count; b++) {
        int32_t p[3];
        if (!re15_gr2_bone_welt_pub(e, b, p)) continue;
        const int64_t v = ((int64_t)p[0] - pl->x) * hx + ((int64_t)p[2] - pl->z) * hz;
        if (bb < 0 || v > best) { best = v; bb = b; }
    }
    if (bb < 0 || best < 0) return;
    s_kontakt = s_kb_takt;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Kontakt F%u: +%d Bilder nach dem Kampfbeginn (Knochen %d, Gator (%d,%d) Gier %d Clip %d, "
            "Spielfigur (%d,%d))\n", (unsigned)g_engine.frame_count, s_kontakt, bb, e->x, e->z, (int)e->rot_y, (int)e->motion,
            pl->x, pl->z);
#endif
}

static void ga_start(int slot, re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    memset(&s_ga, 0, sizeof s_ga);
    s_fress_cut = -1; s_fress_frontal = 0; s_kb_ok = 0; s_kb_takt = 0; s_kontakt = -1;   /* Nachbesserung 5 (Messgroessen) */
    ga_buehne(e, bk);
    e->y = -1200;                                             /* GB_WATER_Y = Wasser-INIT-Y der 0x23-KI @0x8010c65c */
    ga_clip(e, 6, 0, 0);                                      /* Clip 6 (0) @0x801008ac-b8 */
    s_ga.phase = GP_DURCHBRUCH;
    re15_gr2_kamera_auto(0);                                  /* die Szene haelt die Kamera selbst (Nachtrag 4 API) */
    /* Ohne Opfer-Bedingung: der Anlauf FRONTAL (RE2 Cut 10, soll +4092) - mit der Leiche als Opfer blieb nur der Seitenblick
     * Cut 0 (frontal 3250; Cut 7 frontal 3894 "-Opfer", Lauf s3 gator_boss.log). Die Leiche zeigt der FINISHER-Schnitt. */
    int cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_DURCHBRUCH, e, NULL, "gatorszene Durchbruch");
    folge('G', GP_DURCHBRUCH, cut);
    (void)slot;
}

int re15_gatorszene_r41_boss(int slot, void *gator_actor)
{
    re15_actor_t *e = (re15_actor_t *)gator_actor;
    if (!e || (g_current_room_id & 0xFFFEu) != RE15_GSZ_RAUM) return 0;
    if (s_zustand == RE15_GSZ_ANKUNFT || s_zustand == RE15_GSZ_RUECKKEHR) return 1;   /* untergetaucht halten (B9) */
    if (s_zustand == RE15_GSZ_KAMPF) {
        if (s_kb_ok && s_kontakt < 0) s_kb_takt++;          /* Nachbesserung 5: Bilder seit dem Kampfbeginn (Kontakt-Mass) */
        if (!s_ga.kampf_gemeldet) { s_ga.kampf_gemeldet = 1; return 3; }
        return 0;
    }
    if (s_zustand != RE15_GSZ_GATOR) return 0;
    re15_enemy_bank_t *bk = re15_enemy_find(e->type);
    if (!bk || !bk->ok || bk->anim.clip_count < 12) return 1;
    if (s_ga.phase == GP_AUS) ga_start(slot, e, bk);
    e->y = -1200;
    s_ga.t++;
    switch (s_ga.phase) {
    case GP_DURCHBRUCH:                                       /* Clip 6 Bild 1..bis: aus dem Wasser, auf die Leiche zu */
        if (ga_schritt(e, bk) == 2 || (int)e->anim_frame >= ga_durchbruch_bis(bk)) {
            ga_clip(e, 4, 0, 0);                              /* FRESSEN P1: FUN_8001a330(em,4,0,0) @0x801010ec-f4 */
            s_ga.phase = GP_BISS; s_ga.t = 0;
            int cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_FINISHER, e, leiche_als_opfer(), "gatorszene Fressen");
            fress_kamera_messen(e, cut);                      /* Nachbesserung 5 (M3): Frontalwert der Fress-Einstellung */
            folge('G', GP_BISS, cut);
        }
        break;
    case GP_BISS:                                             /* Clip 4 Bild 1..14 (P1), Koppelbild 13 */
        if (!s_ga.leiche_dran) ga_schritt(e, bk); else { e->anim_frame++; ga_anker_lage(e, bk); }
        if (!s_ga.leiche_dran && (int)e->anim_frame >= GSZ_KOPPEL_BILD) {
            s_ga.leiche_dran = 1;
            ga_anker_setzen(e, bk);                           /* FUN_80015b94 @0x80101110-34 */
            folge('K', (int)e->anim_frame, 0);
        }
        if ((int)e->anim_frame >= 14) {
            ga_clip(e, 5, 0, 0);                              /* P2 Clip 5 @0x80101180-88 */
            ga_anker_lage(e, bk);
            s_ga.phase = GP_SCHUETTELN; s_ga.t = 0;
            folge('G', GP_SCHUETTELN, 0);
        }
        break;
    case GP_SCHUETTELN: {                                     /* Clip 5, 120 Bilder (das Rumschleudern) */
        int n = bk->anim.clips[5].frame_count;
        if ((int)e->anim_frame + 1 >= n) {
            /* P3 Clip 11: `addiu a1,zero,11` @0x801011d8 + `jal 0x8001a240` @0x801011e0 - FUN_8001a240 setzt nur Clip/Blend
             * (+0x14C/+0x14E @0x8001a258-60) und schaltet das Bild (FUN_8002959c @0x8001a28c), OHNE den Wurzelschritt
             * FUN_80015e7c/FUN_800152c8 von FUN_8001a330 (@0x8001a374/@0x8001a3c0); die Lage setzt davor FUN_80015cb8 vom Anker
             * (@0x801011cc) = ga_anker_lage (Clip 11 hat eine feste Wurzel 3132, gleiche Lage je Bild). */
            ga_clip(e, 11, 0, 0);
            ga_anker_lage(e, bk);
            s_ga.phase = GP_KAUEN; s_ga.t = 0;
            folge('G', GP_KAUEN, 0);
        } else { e->anim_frame++; ga_anker_lage(e, bk); }
        break;
    }
    case GP_KAUEN: {                                          /* Clip 11, ein Kau-Kreis (59 Bilder), dann verschlungen */
        int n = bk->anim.clips[11].frame_count;
        if ((int)e->anim_frame + 1 >= n) {
            s_ga.leiche_dran = 0; s_ga.leiche_weg = 1;
            leiche_weg();
            folge('V', 0, 0);                                 /* verschlungen: Leiche + Schluessel weg */
            ga_clip(e, 0, 0, GSZ_SCHWIMM_BLEND);
            s_ga.phase = GP_WENDE; s_ga.t = 0;
            folge('G', GP_WENDE, 0);
        } else { e->anim_frame++; ga_anker_lage(e, bk); }
        break;
    }
    case GP_WENDE: {                                          /* Clip 0 schwimmend, Gier 75/Bild auf Leon */
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int ziel = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 1024) & 0xfff;
        const int fertig = ga_drehen(e, ziel, GSZ_WENDE_RATE);   /* FUN_8001569c(em, ziel, 75) @0x801025f8-608 */
        if (ga_schritt(e, bk) == 2) e->anim_frame = 0;
        if (fertig || s_ga.t > GSZ_WENDE_MAX) {
            ga_clip(e, 10, 0, GSZ_SCHREI_BLEND);              /* R4: Clip 10 Blend 3 @0x80102c40-48 */
            ga_se(GSZ_SCHREI_SE);                             /* R4: SE 0 @0x80102c0c-10 */
            re15_gr2_frame_wort(slot, 0x28000000u);           /* Stoss-Ereignis 1 -> Ruetteln + Schuetteln (@0x801003f0-40c) */
            s_ga.phase = GP_SCHREI; s_ga.t = 0;
            /* Kamera: die laufende (FINISHER-Schnitt), solange sie den Kopf zeigt - die Wahl ANGRIFF nahm in Lauf s3 Cut 3, in dem
             * der Kopf (Bild (266,106)) hinter dem Vordergrund des Kessels liegt (PRI-Masken pruefen die Wahl nicht). */
            int32_t kopf[3];
            int cut = (int)g_scd.cam_id;
            if (!re15_gr2_bone_welt_pub(e, 6, kopf) || !re15_gr2_im_bild_cut(cut, kopf, NULL, NULL))
                cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_ANGRIFF, e, NULL, "gatorszene Schrei");
            folge('G', GP_SCHREI, cut);
        }
        break;
    }
    case GP_SCHREI:                                           /* Clip 10 (30 Bilder, Aufbaeumen + Rueckstoss) */
        if (ga_schritt(e, bk) == 2) {
            ga_clip(e, 0, 0, GSZ_SCHWIMM_BLEND);
            re15_gr2_biss_cd(slot, GSZ_BISS_CD);
            re15_gr2_kamera_frei("gatorszene");               /* Spur-B-Sperre auf (Cut_old-Zwilling auf P3s Leichen-Cut) */
            re15_gr2_kamera_auto(1);                          /* Nachbesserung 1 (N1): B's Programme R1/Finisher/Intro wieder an */
            kam_uebergabe();                                  /* Nachbesserung 1 (M1): im SELBEN Bild auf den Cut des Spielers */
            kampfbeginn_messen(e);                            /* Nachbesserung 5 (M1/M4): Abstand + Zonen am Kampfbeginn */
            s_ga.phase = GP_ENDE; s_ga.t = 0;
            s_zustand = RE15_GSZ_KAMPF;
            re15_game_flag_set(5, RE15_GSZ_SIG_FERTIG, 1);    /* Programm 3 laeuft weiter (Ankunfts-Poll) */
            folge('G', GP_ENDE, 0);
        }
        break;
    default:
        break;
    }
    return 2;
}

/* ---- Leiche im Maul: Kind der Part-6-Weltmatrix wie der RE2-Kanister (@0x801018d0-e0) ---------------------------- */
/* Anhaengen im Koppelbild = FUN_8002d030(Part-6-Matrix, Objekt-Matrix) @0x801018f0: rel = R6^T * Ro, lokal = R6^T * (o - p6)
 * (Lokallage `sw v0,56(s2)` / `sw v1,60(s2)` / `sw a3,64(s2)` @0x80101910-18); danach je Bild Ro = R6 * rel, o = p6 + R6 * lokal
 * (Form gator_re2_r41.c re15_gr2_nachtakt / gr2_kan_folgen). Die Leiche haengt also so im Maul, wie sie im Koppelbild lag. */
static int32_t s_rel[9], s_lokal[3];
static int     s_rel_ok = 0;
/* Prop-Drehmatrix wie main.c pc_prop_rot_q12 (RotMatrix-Konvention des Prop-Zeichners). */
static void prop_matrix(int16_t rx, int16_t ry, int16_t rz, int32_t m[9])
{
    int32_t sx = re15_sin_q12(rx), cx = re15_cos_q12(rx);
    int32_t sy = re15_sin_q12(ry), cy = re15_cos_q12(ry);
    int32_t sz = re15_sin_q12(rz), cz = re15_cos_q12(rz);
    m[0] = q12m(cz, cy);
    m[1] = -q12m(sz, cy);
    m[2] = sy;
    m[3] = (int32_t)(((((int64_t)sz * cx) << 12) + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)(((((int64_t)cz * cx) << 12) - (int64_t)sz * sy * sx) >> 24);
    m[5] = -q12m(cy, sx);
    m[6] = (int32_t)(((((int64_t)sz * sx) << 12) - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)(((((int64_t)cz * sx) << 12) + (int64_t)sz * sy * cx) >> 24);
    m[8] = q12m(cy, cx);
}
static void euler_aus_matrix(const int32_t m[9], int16_t *rx, int16_t *ry, int16_t *rz)
{
    /* Umkehrung von prop_matrix: m[2] = sin(ry), m[0] = cz*cy, m[1] = -sz*cy, m[5] = -cy*sx, m[8] = cy*cx.
     * math-atan2(Y, X) = re15_atan2_q12(-Y, X) - 1024 (re15_actor.h: Gier 0 = +X, Vorwaerts (cos,-sin)). */
    int32_t cy = (int32_t)re15_squareroot0((uint32_t)((int64_t)m[0] * m[0] + (int64_t)m[1] * m[1]));
    *ry = (int16_t)(((int)re15_atan2_q12(-m[2], cy) - 1024) & 0xfff);
    *rx = (int16_t)(((int)re15_atan2_q12(m[5], m[8]) - 1024) & 0xfff);
    *rz = (int16_t)(((int)re15_atan2_q12(m[1], m[0]) - 1024) & 0xfff);
}
static void key_tod_pruefen(void);   /* Abschnitt "Schluessel nach dem Tod" (Nachbesserung 2, M2) */
static void key_rueckschnitt_pruefen(int slot);   /* Abschnitt "Schluessel-Schnitt statt R1-Rueckschnitt" (Nachbesserung 3) */
void re15_gatorszene_r41_nachtakt(int slot, void *gator_actor)
{
    re15_actor_t *e = (re15_actor_t *)gator_actor;
    /* Nachbesserung 3 (Abnahme 2, M1): hat B's R1 auf eine Explosions-Kamera geschnitten, ersetzt der Schluessel-Schnitt B's
     * Rueckschnitt am Clip-9-Ende (RE2 P1-Ende @0x8010249c / @0x801024a4-ec) im SELBEN Takt - zwei Takte vor dem Tot-Flag. */
    key_rueckschnitt_pruefen(slot);
    /* Nachbesserung 2 (M2): Tod im SELBEN Bild erkennen, in dem Spur B's R1 P3 das Tot-Flag setzt und die Szenen-Kamera
     * freigibt (@0x80102654-64 / @0x80102688-94) - der Schnitt auf die Schluessel-Kamera ersetzt dessen Rueckschnitt, bevor
     * das Bild gezeichnet wird (sonst stuende der Zonen-Cut ein Bild lang da; der SCD-Takt laeuft vor dem Spielschritt).
     * Das gilt fuer R1 OHNE Explosionsschnitt ("Sperre ohne Schnitt") und die Todesrolle; MIT Schnitt hat key_rueckschnitt_pruefen
     * den Schluessel schon gelegt (Bit 111). */
    key_tod_pruefen();
    if (e && s_zustand == RE15_GSZ_KAMPF && s_kb_ok && s_kontakt < 0 && s_kb_takt >= 1) kontakt_pruefen(e);   /* Nachbesserung 5 */
    if (!e || !s_ga.leiche_dran || s_ga.leiche_weg) return;
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    int32_t r6[9], p6[3];
    if (k < 0 || !re15_gr2_part6_gezeichnet(r6, p6)) return;
    if (!s_rel_ok) {                                         /* Koppelbild: Relation einfrieren (FUN_8002d030 @0x801018f0) */
        int32_t ro[9];
        prop_matrix(g_scd.props[k].rot_x, g_scd.props[k].rot_y, g_scd.props[k].rot_z, ro);
        int32_t d[3] = { g_scd.props[k].x - p6[0], g_scd.props[k].y - p6[1], g_scd.props[k].z - p6[2] };
        for (int r = 0; r < 3; r++) {
            s_lokal[r] = (int32_t)(((int64_t)r6[0*3+r] * d[0] + (int64_t)r6[1*3+r] * d[1] + (int64_t)r6[2*3+r] * d[2]) >> 12);
            for (int c = 0; c < 3; c++)
                s_rel[r*3+c] = (int32_t)(((int64_t)r6[0*3+r] * ro[0*3+c] + (int64_t)r6[1*3+r] * ro[1*3+c]
                                        + (int64_t)r6[2*3+r] * ro[2*3+c]) >> 12);
        }
        s_rel_ok = 1;
        folge('H', s_lokal[0], s_lokal[2]);
    }
    int32_t rw[9];
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rw[r*3+c] = (int32_t)(((int64_t)r6[r*3] * s_rel[c] + (int64_t)r6[r*3+1] * s_rel[3+c] + (int64_t)r6[r*3+2] * s_rel[6+c]) >> 12);
    g_scd.props[k].x = p6[0] + (int32_t)(((int64_t)r6[0] * s_lokal[0] + (int64_t)r6[1] * s_lokal[1] + (int64_t)r6[2] * s_lokal[2]) >> 12);
    g_scd.props[k].y = p6[1] + (int32_t)(((int64_t)r6[3] * s_lokal[0] + (int64_t)r6[4] * s_lokal[1] + (int64_t)r6[5] * s_lokal[2]) >> 12);
    g_scd.props[k].z = p6[2] + (int32_t)(((int64_t)r6[6] * s_lokal[0] + (int64_t)r6[7] * s_lokal[1] + (int64_t)r6[8] * s_lokal[2]) >> 12);
    euler_aus_matrix(rw, &g_scd.props[k].rot_x, &g_scd.props[k].rot_y, &g_scd.props[k].rot_z);
}

/* ---- Takt (scd_vm_tick, Gameplay-Lauf) --------------------------------------------------------------------------- */
static re15_actor_t *gator_aktor(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x23u) return &g_actors[i];
    return NULL;
}
/* Alligator tot: RE2-R1-Endzustand (Kanister/Kiefer, +0x10E |= 0x4000 @0x80102654-64 = re15_gr2_tot) ODER die Todesrolle
 * Clip 7 des Boss-Moduls (Explosionen/GB_TEST=3) mit dem byte-true CORPSE-Zustand 7 (enemy_ai_boss_gator.c GBP_DIE -> GBP_DEAD
 * `e->state = 7`). */
static int gator_tot(void)
{
    if (re15_gr2_tot()) return 1;
    const re15_actor_t *g = gator_aktor();
    return g && g->state == 7;
}
/* ---- Schluessel-Lage nach dem Tod (Nachbesserung 1, M2) ---------------------------------------------------------------
 * Abnahme 0: an beiden gemessenen Todesstellen (R1 (2795,-1620,-26820), Todesrolle (-7277,-1620,-25029)) lag der FLACHE
 * Schluessel unter bzw. hinter den Zonenkameras (Cut 0 steht bei (2754,-3132,-26190), Cut 1 bei (-7740,-3132,-25956); RID
 * @0x060) - in allen 13 Kameras bitgleich zum Lauf ohne Schluessel. Die erste Fassung nahm den Kopf des Alligators ohne Blick
 * auf eine Kamera und legte das Quadrat flach (Bildhoehe 180 x sin(Blickhoehe ~10 Grad) < 1 Bildpunkt). PORT-WAHL der Lage
 * (Nutzer: "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seine Leiche sein und blinken"), FORM wie der
 * Schluessel der Ankunft (tools/r41_gatorszene/leiche_lage.py):
 *  - Kamera (Nachbesserung 2): die Zonenkamera (RVD-Gruppe mit Wechselsatz: ROOM2090 Cut 0..7, wie gator_re2_r41.c
 *    gr2_zonenkamera) mit dem GROESSTEN Schluessel-Bild; die Einstellung nach dem Tod schneidet auf sie (key_einstellung). Bis
 *    Nachbesserung 1 die laufende Kamera (R1 P3 setzt das Tot-Flag +0x10E |= 0x4000 @0x80102654-64 und gibt im selben Schritt
 *    die Szenen-Kamera frei @0x80102688-94) - darin nur 4-6 Bildpunkte (Abnahme 1, M2);
 *  - Lage: Rasterpunkt (100) im Wasser (re15_aot_water_at fuer Mitte und alle vier Ecken des Quadrats +-90 = MD1 @0x354C
 *    Ecken -90/90) im Bild der Kamera (Mitte x 24..296, y 40..205 wie leiche_lage.py), von der Leiche NICHT verdeckt (je Knochen
 *    eine Kugel mit der halben Breite 800 des RE1.5-Alligator-Kastens {1000,-720,0,2200,720,800} @0x80118b98, re15_damage.c;
 *    leiche_verdeckt - bis Nachbesserung 1 "vor JEDEM Knochen - 800"), hoechstens so weit vom naechsten Knochen wie der Kopf der
 *    Leiche von ihrer Wurzel (Knochen 6 zu Knochen 0 der Leichen-Pose; ohne Bank 3055 = Kopf-Versatz der Buehnen-Messzeile)
 *    und darunter mit dem GROESSTEN Bild (kleinste Tiefe; gleich gross: naeher an der Leiche). Erste Messung dieser Fassung
 *    mit "der Leiche am naechsten" (Lauf T1n, Todesrolle Cut 0): Tiefe 6948, nur 4 Bildpunkte - darum das groesste Bild
 *    innerhalb des Kopf-Wurzel-Abstands;
 *  - Neigung: zur Kamera, hoechstens 60 Grad (wie die Ankunft), groesster Wert cos(Blick) x Helligkeit, deren Blink-Spitze
 *    (Takt 0x1f = Wert 0xBE, re15_blink_r41_wert) die Primitivfarbe 0x80 nicht ueberschreitet (M5: render_pc.c
 *    psx_prim_to_sdl_vert kappt darueber) - Licht genau wie der Zeichner (main.c Prop-Pfad: Lichtsatz des Cuts,
 *    re15_light_setup_actor an der Lage, re15_light_ctx_rotate_for_bone, re15_light_shade_vertex der MD1-Normalen
 *    (0,-4096,0) @0x354C+0x60 = 00 00 00 f0);
 *  - Hoehe: die tiefste Ecke des gekippten Quadrats auf der Wasseroberflaeche (p0 der Wasserzone, ROOM2090 @0x009D6 ac f9). */
#define GSZ_RASTER          100
#define GSZ_KEY_HALB        90      /* Quadrat-Halbseite: re15_gsz_key_md1 Ecken (-90/90) = ROOM7040 MD1 @0x354C+0x38 */
#define GSZ_BILD_X0         24      /* Bildrand wie leiche_lage.py (x 24..296, y 40..205) */
#define GSZ_BILD_X1         296
#define GSZ_BILD_Y0         40
#define GSZ_BILD_Y1         205
#define GSZ_GATOR_HALBBREITE 800    /* RE1.5-Alligator-Kasten @0x80118b98 = {1000,-720,0,2200,720,800}, z 800 (re15_damage.c) */
#define GSZ_KIPP_MIN_Q12    2048    /* cos 60 Grad = 0,5 (Kippung hoechstens 60 Grad, wie leiche_lage.py) */
#define GSZ_PRIM_KAPPE      0x80    /* render_pc.c psx_prim_to_sdl_vert: Primitivfarbe 0x80 -> SDL 0xFF = Texel x 1,0 (Kappung) */
#define GSZ_LEICHE_PUNKTE   40

static int kam_sicht(int cut, re15_camera_view_t *v)
{
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    return re15_camera_build_view(&g_room_rdt.cuts[cut], v) == 0;
}
/* Projektion wie der Zeichner (gator_re2_r41.c gr2_im_bild): 1 = vor der Kamera; Bildpunkt + Tiefe vz. */
static int kam_proj(const re15_camera_view_t *v, const int32_t p[3], int *sx, int *sy, int32_t *vz_out)
{
    const int64_t px = p[0], py = p[1], pz = p[2];
    const int32_t vx = (int32_t)(((int64_t)v->rot[0]*px + (int64_t)v->rot[1]*py + (int64_t)v->rot[2]*pz) >> 12) + v->trans[0];
    const int32_t vy = (int32_t)(((int64_t)v->rot[3]*px + (int64_t)v->rot[4]*py + (int64_t)v->rot[5]*pz) >> 12) + v->trans[1];
    const int32_t vz = (int32_t)(((int64_t)v->rot[6]*px + (int64_t)v->rot[7]*py + (int64_t)v->rot[8]*pz) >> 12) + v->trans[2];
    if (vz_out) *vz_out = vz;
    if (vz <= 0) return 0;
    if (sx) *sx = 160 + (int)(((int64_t)vx * v->fov_screen_dist) / vz);
    if (sy) *sy = 120 + (int)(((int64_t)vy * v->fov_screen_dist) / vz);
    return 1;
}
/* Zonenkamera = Cut mit mindestens einem Wechselsatz ausser dem Such-Anker (rdt_common.c), wie gr2_zonenkamera. */
static int zonenkamera(int cut)
{
    int n = 0;
    for (int i = 0; i < g_room_rdt.zone_count; i++) if ((int)g_room_rdt.zones[i].cam_from == cut) n++;
    return n >= 2;
}
/* Leichen-Punkte: alle Knochen der GEZEICHNETEN Pose (re15_gr2_bone_welt_pub, mit Zeichen-Skala und Wurzelversatz des
 * Bildes); die Aktorlage selbst nur, wenn keine Bank da ist (sie liegt bei einem Wurzelversatz nicht auf dem Koerper). */
static int leichen_punkte(const re15_actor_t *g, int32_t pk[GSZ_LEICHE_PUNKTE][3], int32_t kopf[3])
{
    int n = 0;
    kopf[0] = g->x; kopf[1] = g->y; kopf[2] = g->z;
    re15_enemy_bank_t *bk = re15_enemy_find(g->type);
    const int nb = (bk && bk->ok) ? bk->skel.bone_count : 0;
    for (int b = 0; b < nb && n < GSZ_LEICHE_PUNKTE; b++)
        if (re15_gr2_bone_welt_pub(g, b, pk[n])) {
            if (b == 6) { kopf[0] = pk[n][0]; kopf[1] = pk[n][1]; kopf[2] = pk[n][2]; }
            n++;
        }
    if (n == 0) { pk[0][0] = g->x; pk[0][1] = g->y; pk[0][2] = g->z; n = 1; }
    return n;
}
static int wasser_quadrat(int32_t x, int32_t z, int32_t *wy)
{
    const int32_t y = re15_aot_water_at(x, z);
    if (!y) return 0;
    for (int e = 0; e < 4; e++)
        if (!re15_aot_water_at(x + ((e & 1) ? GSZ_KEY_HALB : -GSZ_KEY_HALB), z + ((e & 2) ? GSZ_KEY_HALB : -GSZ_KEY_HALB)))
            return 0;
    *wy = y;
    return 1;
}
typedef struct { int32_t x, y, z, vz; int64_t d2; } gsz_lage_t;
/* Nachbesserung 2 (M2): verdeckt die Leiche den Schluessel im Bild? Je Knochen eine Kugel mit der halben Breite des
 * Alligator-Kastens (800, {1000,-720,0,2200,720,800} @0x80118b98): verdeckt, wenn die Kugel nicht ganz hinter dem Schluessel
 * liegt (Tiefe - 800 < Schluessel-Tiefe) und ihr Bildkreis (800 x H / Tiefe) den Schluessel (Mitte + halbe Quadrat-Bildgroesse)
 * trifft; eine Kugel, die die Bildebene schneidet (0 < Tiefe <= 800), verdeckt immer, Knochen hinter der Kamera nie. Die erste
 * Fassung verlangte "vor JEDEM Knochen - 800" - das schloss jede Kamera aus, hinter der ein Teil der Leiche liegt oder vor der die
 * Leiche naeher als ihr Bildrand liegt (gemessen Lauf T1m dieser Nachbesserung: von acht Zonenkameras blieben Cut 0/3/6, der
 * groesste Schluessel 7 Bildpunkte). */
static int leiche_verdeckt(const re15_camera_view_t *v, int32_t pk[][3], int npk, int sx, int sy, int32_t vz_key)
{
    const int64_t h = v->fov_screen_dist;
    const int64_t halb = vz_key > 0 ? (GSZ_KEY_HALB * h) / vz_key : 0;
    for (int i = 0; i < npk; i++) {
        int bx = 0, by = 0; int32_t bz = 0;
        if (!kam_proj(v, pk[i], &bx, &by, &bz)) continue;               /* Knochen hinter der Kamera */
        if (bz - GSZ_GATOR_HALBBREITE >= vz_key) continue;              /* Kugel ganz hinter dem Schluessel */
        if (bz <= GSZ_GATOR_HALBBREITE) return 1;                       /* Kugel schneidet die Bildebene */
        const int64_t r = (GSZ_GATOR_HALBBREITE * h) / bz + halb;
        const int64_t dx = sx - bx, dy = sy - by;
        if (dx * dx + dy * dy <= r * r) return 1;
    }
    return 0;
}
/* Beste Lage fuer Kamera cut (s. Kopf); r_nah = hoechster Abstand zum naechsten Knochen; 0 = keine. */
static int key_suche(int cut, int32_t pk[GSZ_LEICHE_PUNKTE][3], int npk, int32_t r_nah, gsz_lage_t *out)
{
    re15_camera_view_t v;
    if (!kam_sicht(cut, &v)) return 0;
    int32_t x0 = 0x7fffffff, x1 = -0x7fffffff, z0 = 0x7fffffff, z1 = -0x7fffffff;
    for (int i = 0; i < RE15_AOT_MAX; i++) {                  /* Rasterbereich = Huelle der Wasserzonen */
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_WATER) continue;
        if (a->has_quad) {
            for (int k = 0; k < 4; k++) {
                if (a->xs[k] < x0) x0 = a->xs[k];
                if (a->xs[k] > x1) x1 = a->xs[k];
                if (a->zs[k] < z0) z0 = a->zs[k];
                if (a->zs[k] > z1) z1 = a->zs[k];
            }
        } else {
            if (a->x - a->half_w < x0) x0 = a->x - a->half_w;
            if (a->x + a->half_w > x1) x1 = a->x + a->half_w;
            if (a->z - a->half_h < z0) z0 = a->z - a->half_h;
            if (a->z + a->half_h > z1) z1 = a->z + a->half_h;
        }
    }
    if (x0 > x1 || z0 > z1) return 0;
    x0 = (x0 / GSZ_RASTER) * GSZ_RASTER; z0 = (z0 / GSZ_RASTER) * GSZ_RASTER;
    int gefunden = 0;
    for (int32_t z = z0; z <= z1; z += GSZ_RASTER)
        for (int32_t x = x0; x <= x1; x += GSZ_RASTER) {
            /* Reihenfolge nach Kosten (Ergebnis gleich): Naehe zur Leiche, Bild/Tiefe, zuletzt die Wasserzonen (je bis zu
             * RE15_AOT_MAX Saetze) - so laufen die teuren Pruefungen nur fuer den Ring um die Leiche (PSX-Laufzeit). */
            int64_t d2 = INT64_MAX;
            for (int i = 0; i < npk; i++) {
                const int64_t dx = (int64_t)x - pk[i][0], dz = (int64_t)z - pk[i][2];
                if (dx * dx + dz * dz < d2) d2 = dx * dx + dz * dz;
            }
            if (d2 > (int64_t)r_nah * r_nah) continue;                     /* nicht mehr "in der Naehe seiner Leiche" */
            const int32_t wy0 = re15_aot_water_at(x, z);
            if (!wy0) continue;
            const int32_t p[3] = { x, wy0, z };
            int sx = 0, sy = 0; int32_t vz;
            if (!kam_proj(&v, p, &sx, &sy, &vz)) continue;
            if (sx < GSZ_BILD_X0 || sx > GSZ_BILD_X1 || sy < GSZ_BILD_Y0 || sy > GSZ_BILD_Y1) continue;
            if (leiche_verdeckt(&v, pk, npk, sx, sy, vz)) continue;    /* nicht von der Leiche verdeckt (Nachbesserung 2) */
            int32_t wy;
            if (!wasser_quadrat(x, z, &wy)) continue;
            if (!gefunden || vz < out->vz || (vz == out->vz && d2 < out->d2)) {
                out->x = x; out->y = wy; out->z = z; out->vz = vz; out->d2 = d2; gefunden = 1;
            }
        }
    return gefunden;
}
/* Neigung zur Kamera cut an der Lage p (s. Kopf). Rueckgabe 1 + rx/rz, Mittelpunkt-y (tiefste Ecke auf p[1]) und die
 * Primitivfarben (Ruhe, Spitze) des hellsten Kanals; ohne Lichtsatz (Riegel ohne Raum-Licht) nur cos(Blick). */
static int key_neigung(int cut, const int32_t p_wasser[3], int16_t *rx_out, int16_t *rz_out, int32_t *y_out,
                       int *prim_ruhe, int *prim_spitze)
{
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    const re15_camera_cut_t *c = &g_room_rdt.cuts[cut];
    int32_t p[3] = { p_wasser[0], p_wasser[1], p_wasser[2] };
    int brx = 0, brz = 0, bruhe = 0, bspitze = 0;
    for (int pass = 0; pass < 2; pass++) {                   /* 2. Durchgang: Licht an der gehobenen Mitte */
        const int64_t dx = (int64_t)c->pos_x - p[0], dy = (int64_t)c->pos_y - p[1], dz = (int64_t)c->pos_z - p[2];
        const uint32_t len = re15_squareroot0((uint32_t)(dx * dx + dy * dy + dz * dz));
        if (!len) return 0;
        const int32_t dv[3] = { (int32_t)(dx * 4096 / len), (int32_t)(dy * 4096 / len), (int32_t)(dz * 4096 / len) };
        re15_actor_lightctx_t welt;
        const int licht = g_re15_room_lights_ok && cut < g_re15_room_lights.cut_count;
        if (licht) re15_light_setup_actor(&g_re15_room_lights.cuts[cut], p, NULL, &welt);
        int64_t best = -1;
        for (int rx = -1024; rx <= 1024; rx += 32)
            for (int rz = -1024; rz <= 1024; rz += 32) {
                int32_t m[9];
                prop_matrix((int16_t)rx, 0, (int16_t)rz, m);
                const int32_t n[3] = { -m[1], -m[4], -m[7] };   /* Welt-Normale der MD1-Normalen (0,-4096,0) */
                if (-n[1] < GSZ_KIPP_MIN_Q12) continue;
                const int32_t cv = (int32_t)(((int64_t)n[0] * dv[0] + (int64_t)n[1] * dv[1] + (int64_t)n[2] * dv[2]) >> 12);
                if (cv <= 0) continue;
                int ruhe = GSZ_PRIM_KAPPE, spitze = 0;
                if (licht) {
                    re15_actor_lightctx_t ctx, sp;
                    uint8_t r, g, b;
                    re15_light_ctx_rotate_for_bone(&welt, m, &ctx);
                    re15_light_shade_vertex(&ctx, 0, -4096, 0, &r, &g, &b);
                    ruhe = r > g ? (r > b ? r : b) : (g > b ? g : b);
                    sp = ctx;
                    re15_blink_r41_lichtctx((uint16_t)g_current_room_id, RE15_GSZ_KEY_OBJ, RE15_BLINK_R41_UP_BELOW, &sp);
                    re15_light_shade_vertex(&sp, 0, -4096, 0, &r, &g, &b);
                    spitze = r > g ? (r > b ? r : b) : (g > b ? g : b);
                    if (spitze > GSZ_PRIM_KAPPE) continue;      /* Puls liefe in die Kappung (M5) */
                }
                const int64_t w = (int64_t)cv * ruhe;
                if (w > best) { best = w; brx = rx; brz = rz; bruhe = ruhe; bspitze = spitze; }
            }
        if (best < 0) return 0;
        int32_t m[9], ext = 0;
        prop_matrix((int16_t)brx, 0, (int16_t)brz, m);
        for (int e = 0; e < 4; e++) {                            /* tiefste Ecke (y waechst nach unten) */
            const int32_t ex = (e & 1) ? GSZ_KEY_HALB : -GSZ_KEY_HALB, ez = (e & 2) ? GSZ_KEY_HALB : -GSZ_KEY_HALB;
            const int32_t ey = (int32_t)(((int64_t)m[3] * ex + (int64_t)m[5] * ez) >> 12);
            if (ey > ext) ext = ey;
        }
        p[1] = p_wasser[1] - ext;
    }
    *rx_out = (int16_t)brx; *rz_out = (int16_t)brz; *y_out = p[1];
    if (prim_ruhe) *prim_ruhe = bruhe;
    if (prim_spitze) *prim_spitze = bspitze;
    return 1;
}
static int s_key_prim_ruhe = -1, s_key_prim_spitze = -1;
/* Lage + Kamera + Neigung beim Tod (Kopf oben). */
static void key_lage_bei_tod(void)
{
    re15_actor_t *g = gator_aktor();
    leiche_mitte_konst(&s_key_x, &s_key_z);
    s_key_y = GSZ_WASSER_Y; s_key_rx = 0; s_key_rz = 0; s_key_cut = -1;
    s_key_lage_ok = 1; s_key_neigung_offen = 0;
    re15_blink_r41_setzen((uint16_t)g_current_room_id, RE15_GSZ_KEY_OBJ, 1);   /* fuer die Spitzen-Rechnung (re15_blink_r41_lichtctx) */
    if (!g) return;
    static int32_t pk[GSZ_LEICHE_PUNKTE][3];
    int32_t kopf[3];
    const int npk = leichen_punkte(g, pk, kopf);
    /* Naehe = Kopf-Wurzel-Abstand der Leiche (Knochen 6 zu Knochen 0, xz); ohne Bank der Kopf-Versatz 3055 der Buehne */
    int32_t r_nah = 3055;
    if (npk > 6) {                                          /* kopf = Knochen 6, pk[0] = Knochen 0 (leichen_punkte) */
        const int64_t dx = (int64_t)kopf[0] - pk[0][0], dz = (int64_t)kopf[2] - pk[0][2];
        r_nah = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
    }
    gsz_lage_t l, b;
    memset(&l, 0, sizeof l);
    int cut = -1;
    const int lauf = (int)g_scd.cam_id;
    /* Nachbesserung 2 (Abnahme 1, M2): die Zonenkamera mit dem GROESSTEN Schluessel-Bild (kleinste Tiefe) unter allen, nicht
     * mehr die laufende - die Einstellung nach dem Tod (key_einstellung) schneidet auf sie. Gemessen Abnahme 1: in der laufenden
     * Kamera nur 4 Bildpunkte (Todesrolle Cut 0, Tiefe 4835) bzw. 6 (R1 Cut 1, Tiefe 5766). Gleiche Tiefe: die laufende, sonst
     * der kleinere Cut. Zonenkamera = RVD-Gruppe mit Wechselsatz (ROOM2090 Cut 0..7): dort sieht der Spieler den Schluessel auch,
     * wenn er hingeht; die Sonderkameras 8..12 tragen nur den Such-Anker (gator_re2_r41.c gr2_zonenkamera). */
    for (int c = 0; c < g_room_rdt.cut_count; c++) {
        if (!zonenkamera(c) || !key_suche(c, pk, npk, r_nah, &b)) continue;
#ifdef RE15_PLATFORM_PC
        {   re15_camera_view_t kv;
            const int h = kam_sicht(c, &kv) ? (int)kv.fov_screen_dist : 0;
            fprintf(stderr, "[gatorszene] Schluessel-Kandidat: Kamera %d%s, Lage (%d,%d,%d) Tiefe %d = Quadrat %d Bildpunkte\n", c,
                    c == lauf ? " (laufende)" : "", (int)b.x, (int)b.y, (int)b.z, (int)b.vz,
                    b.vz > 0 ? (int)((2 * GSZ_KEY_HALB * h) / b.vz) : 0); }
#endif
        if (cut < 0 || b.vz < l.vz || (b.vz == l.vz && c == lauf)) { l = b; cut = c; }
    }
    if (cut < 0) {                                          /* keine Kamera: Kopf in eine Wasserzone gezogen (erste Fassung) */
        for (int i = 0; i <= 8; i++) {
            const int32_t x = kopf[0] + (int32_t)(((int64_t)(g->x - kopf[0]) * i) / 8);
            const int32_t z = kopf[2] + (int32_t)(((int64_t)(g->z - kopf[2]) * i) / 8);
            if (re15_aot_water_at(x, z) != 0) { s_key_x = x; s_key_z = z; break; }
        }
        return;
    }
    s_key_x = l.x; s_key_z = l.z; s_key_y = l.y; s_key_cut = cut;
    const int32_t p[3] = { l.x, l.y, l.z };
    int16_t rx = 0, rz = 0; int32_t y = l.y;
    if (key_neigung(cut, p, &rx, &rz, &y, &s_key_prim_ruhe, &s_key_prim_spitze)) { s_key_rx = rx; s_key_rz = rz; s_key_y = y; }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Schluessel-Lage: Kamera %d (groesstes Bild, laufende %d), Lage (%d,%d,%d) Tiefe %d, %u von der "
                    "Leiche (%d Punkte, Naehe <= %d), Neigung rx %d rz %d, Primitivfarbe Ruhe %d / Blink-Spitze %d (Kappe 0x80)\n",
            cut, lauf, s_key_x, s_key_y, s_key_z, (int)l.vz,
            (unsigned)re15_squareroot0((uint32_t)(l.d2 > 0xffffffffLL ? 0xffffffffLL : l.d2)), npk, (int)r_nah,
            (int)s_key_rx, (int)s_key_rz, s_key_prim_ruhe, s_key_prim_spitze);
#endif
}
/* Ersatzlage nach dem Laden: Neigung zur laufenden Kamera (oder zur Zonenkamera, in deren Bild sie liegt), im ersten Takt. */
static void key_neigung_nachholen(void)
{
    s_key_neigung_offen = 0;
    const int k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k < 0 || !g_scd.props[k].active) return;
    const int32_t wy = re15_aot_water_at(s_key_x, s_key_z);
    const int32_t p[3] = { s_key_x, wy ? wy : GSZ_WASSER_Y, s_key_z };
    int cut = -1;
    const int lauf = (int)g_scd.cam_id;
    re15_camera_view_t v; int sx = 0, sy = 0; int32_t vz;
    if (kam_sicht(lauf, &v) && kam_proj(&v, p, &sx, &sy, &vz) && sx >= GSZ_BILD_X0 && sx <= GSZ_BILD_X1 &&
        sy >= GSZ_BILD_Y0 && sy <= GSZ_BILD_Y1) cut = lauf;
    else {
        int32_t best = 0x7fffffff;
        for (int c = 0; c < g_room_rdt.cut_count; c++)
            if (zonenkamera(c) && kam_sicht(c, &v) && kam_proj(&v, p, &sx, &sy, &vz) && sx >= GSZ_BILD_X0 &&
                sx <= GSZ_BILD_X1 && sy >= GSZ_BILD_Y0 && sy <= GSZ_BILD_Y1 && vz < best) { best = vz; cut = c; }
    }
    if (cut < 0) return;
    int16_t rx, rz; int32_t y;
    if (!key_neigung(cut, p, &rx, &rz, &y, &s_key_prim_ruhe, &s_key_prim_spitze)) return;
    s_key_rx = rx; s_key_rz = rz; s_key_y = y; s_key_cut = cut;
    g_scd.props[k].y = y; g_scd.props[k].rot_x = rx; g_scd.props[k].rot_z = rz;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Schluessel-Ersatzlage (%d,%d,%d): Neigung rx %d rz %d zur Kamera %d\n", s_key_x, y, s_key_z,
            (int)rx, (int)rz, cut);
#endif
}
/* ---- Einstellung auf den Schluessel nach dem Tod (Nachbesserung 2, Abnahme 1 M2) ----------------------------------------
 * Der Schluessel ist das flache RE2-Bodenmodell Platform Key (Quadrat 180, ROOM7040 MD1 @0x354C); aus den Zonenkameras an den
 * Beckenenden (RID @0x060, y -3132) war er in der LAUFENDEN Kamera nur 4-6 Bildpunkte gross (Abnahme 1: Todesrolle Cut 0 Tiefe
 * 4835, R1 Cut 1 Tiefe 5766). Jetzt: Lage fuer die Zonenkamera mit dem groessten Bild (key_lage_bei_tod), Schnitt im Bild des
 * Todes auf diese Kamera (Cut_chg-Zwilling LAB_800402a0, cut_chg_zwilling), Programm 4 haelt 90 Bilder mit Riegel und kehrt mit
 * Cut_old (FUN_8004032c: gemerkter angezeigter Cut DAT_800b3f7b @0x8004033c, Zonenkamera an @0x80040374-84) + Cut_auto zurueck.
 * FORM = RE1.5 "Kamera auf einen Gegenstand" ROOM2010 sub03 @0x008EE-0x008F8 / ROOM1180 sub02 @0x00B3C-0x00B46 (Cut_chg,
 * Message_on, Cut_old, Cut_auto) mit dem Halt der Leichen-Einstellung der Ankunft statt eines Textes (kein Text vorgegeben).
 * PORT-WAHL der Inszenierung (Nutzer: "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seine Leiche sein und
 * blinken"; Nachtrag 6: "sieht die Zombie Leiche mit dem Schluessel in der Cutscene" = dieselbe Einstellung fuer den Schluessel). */
static int      s_einst_slot = -1;      /* SCD-Faden von Programm 4 (-1 = keiner) */
static int      s_einst_cut = -1, s_einst_vorher = -1;
static unsigned s_einst_bild = 0;
static void key_einstellung(void)
{
    const int cut = s_key_cut;
    if (cut < 0 || !g_room_rdt_ok || !g_room_rdt.cuts || cut >= g_room_rdt.cut_count) return;
    const int vorher = (int)g_scd.work_vars[0x0A];           /* angezeigter Cut = Ziel von Cut_old (@0x800402c0/@0x800402e4) */
    cut_chg_zwilling(cut);
    const int alt = s_zustand;
    s_zustand = RE15_GSZ_SCHLUESSEL;                          /* nur fuer re15_gatorszene_r41_ereignis -> Programm 4 */
    const int slot = scd_event_fire(RE15_GSZ_EREIGNIS);
    s_zustand = alt;
    if (slot < 0) {                                           /* kein Ereignis-Faden frei: sofort zurueck (Cut_old-Zwilling) */
        g_scd.cam_id = g_scd.cam_id_prev;
        g_scd.cam_change_pending = 1;
        g_scd.work_vars[0x0C] = g_scd.work_vars[0x0A];       /* @0x8004035c */
        g_scd.work_vars[0x0A] = (int16_t)g_scd.cam_id_prev;  /* @0x80040364 */
        g_scd.cut_auto_enabled = 1;                          /* &= ~0x100 @0x80040374-84 (FUN_8004032c) */
        folge('X', 4, 0);
        return;
    }
    s_einst_slot = slot; s_einst_cut = cut; s_einst_vorher = vorher; s_einst_bild = (unsigned)g_engine.frame_count;
    folge('E', cut, vorher);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Schluessel-Einstellung: Cut %d (vorher Cut %d) F%u, Halt %d Bilder mit Riegel, dann Cut_old + "
                    "Cut_auto (Form ROOM2010 sub03 @0x008EE-0x008F8)\n", cut, vorher, (unsigned)g_engine.frame_count,
            (int)GSZ_KEY_SCHUSS_BILDER);
#endif
}
/* Ende von Programm 4 erkennen (Messzeile/Riegel): der Faden laeuft nicht mehr in s_prog. */
static void key_einstellung_takt(void)
{
    if (s_einst_slot < 0 || s_einst_slot >= SCD_THREAD_COUNT) return;
    const scd_thread_t *t = &g_scd.threads[s_einst_slot];
    if (t->active && t->pc >= s_prog && t->pc < s_prog + s_prog_len) return;
    s_einst_slot = -1;
    folge('O', (int)g_scd.cam_id, s_einst_cut);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Schluessel-Einstellung Ende: Cut %d F%u (Einstellung Cut %d ab F%u, vorher Cut %d), Zonenkamera %s\n",
            (int)g_scd.cam_id, (unsigned)g_engine.frame_count, s_einst_cut, s_einst_bild, s_einst_vorher,
            g_scd.cut_auto_enabled ? "an" : "aus");
#endif
}
int re15_gatorszene_r41_key_einstellung(int *cut, int *vorher, unsigned *bild)
{
    if (cut) *cut = s_einst_cut;
    if (vorher) *vorher = s_einst_vorher;
    if (bild) *bild = s_einst_bild;
    return s_einst_slot >= 0;
}
/* Schluessel nach dem Tod noch offen: Raum 2090/2091, Szene gesehen (9,110), Schluessel noch nicht gelegt (9,111). */
static int key_tod_offen(void)
{
    if ((uint16_t)(g_current_room_id & 0xFFFEu) != RE15_GSZ_RAUM) return 0;
    return re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN) && !re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT);
}
/* Lage + Kamera, (9,111), Schluessel + Zone, Einstellung. */
static void key_tod_setzen(void)
{
    key_lage_bei_tod();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, 1);
    key_bei_leiche((uint16_t)g_current_room_id);
    folge('L', (int)s_key_x, (int)s_key_z);
    key_einstellung();
}
/* Tod erkennen (Nachtakt im Bild des Todes, Rueckfall im SCD-Takt). Nachbesserung 3 (Nebenbefund): der R1-Endzustand zaehlt nur,
 * wenn R1 in DIESEM Raumbesuch lief (s_r1_lief, key_rueckschnitt_pruefen) - re15_gr2_tot() fragt ALLE Slots ab (gator_re2_r41.c:
 * `if (s_g[i].tot) return 1`), s_g eines Slots loescht erst der Boss-INIT des neuen Aktors (re15_gr2_reset, enemy_ai_boss_gator.c
 * INIT), und dieser Rueckfall laeuft im ersten Bild VOR dem ersten Boss-Takt. Ohne die Bedingung legte ein frueherer R1-Tod derselben
 * Sitzung den Schluessel beim Betreten sofort (gemessen: Riegel unit_r41_gatorszene_tod_explosion, zweiter Fall im selben Prozess -
 * Einstellung ab Bild 0, kein Rueckschnitt-Ersatz). Die Todesrolle (Zustand 7 des aktuellen Aktors) ist davon nicht betroffen. */
static void key_tod_pruefen(void)
{
    if (!key_tod_offen()) return;
    const re15_actor_t *g = gator_aktor();
    if (!((s_r1_lief && re15_gr2_tot()) || (g && g->state == 7))) return;
    key_tod_setzen();
}
/* ---- Schluessel-Schnitt statt R1-Rueckschnitt (Nachbesserung 3, Abnahme 2 M1) -------------------------------------------
 * RE2 R1 (EM23_OVL_0000.BIN, selbst disassembliert): P0 schneidet nur, wenn der laufende Cut den Kopf nicht zeigt, und merkt das
 * (+0x218 |= 0x40 @0x80102248); P1 laeuft Clip 9 bis zum letzten Bild (`jal 0x8001a330` a1 = 9 @0x80102494, `beq v0,zero,
 * 0x8010252c` @0x8010249c) und schneidet DORT zurueck (`lhu v1,536(s1)` @0x801024a4, `andi v0,v1,0x40` @0x801024ac, `beq
 * v0,zero` @0x801024b0, Cut 6/13 `jal 0x8002c7ac` @0x801024ec = Cut setzen 0x800cfbf2 @0x8002c7b8); die Sperre der Zonenkamera
 * (0x800cfbd8 Bit 0x100) bleibt bis P3 (`addiu v1,zero,-257 / and / sw` @0x80102688-94), dazwischen P2 (+0x6 += 1 @0x80102638-50)
 * und das Tot-Flag (+0x10E |= 0x4000 @0x8010265c-64). Spur B bildet das ab (gator_re2_r41.c re15_gr2_r1_tick: gr2_kam_zurueck am
 * Clip-9-Ende, gr2_kam_frei in P3). Schnitt der Port erst mit dem Tot-Flag auf den Schluessel (Nachbesserung 2), stand der Zonen-
 * Cut zwei Bilder lang dazwischen - gemessen Abnahme 2 Lauf KW/KWE und Nachbesserung 3 Lauf V_KW/V_KWE: state.log cam 1 F160-F195,
 * cam 0 F196-F197, cam 2 ab F198 (Leon und Elza).
 * PORT-WAHL der Inszenierung (Form wie Nachbesserung 2, RE1.5 "Kamera auf einen Gegenstand" ROOM2010 sub03 @0x008EE): der
 * Schluessel-Schnitt ERSETZT diesen Rueckschnitt im selben Takt. Die Explosions-Einstellung behaelt damit ihre RE2-Laenge (Clip 9 bis
 * zum letzten Bild), an der Stelle des RE2-Rueckschnitts steht die Einstellung auf den Schluessel, und deren Cut_old kehrt auf
 * denselben Ausgangs-Cut zurueck (key_einstellung: vorher = work_vars[0x0A], von B's Rueckschnitt gesetzt = Cut_chg-Zwilling
 * @0x800402fc). Die Leiche liegt schon in der Endpose (Clip 9 letztes Bild; P2/P3 bewegen sie nicht, re15_gr2_r1_tick), die Lage
 * des Schluessels ist dieselbe wie im Tot-Bild. B's Programm endet mit re15_gr2_kamera_frei (Spur-B-API "Programm-Ende"), damit P3
 * die Sperre der Einstellung nicht aufhebt (gr2_kam_frei laeuft dort nur bei aktivem Programm).
 * Erkennung: R1 laeuft (re15_gr2_r1_laeuft, vor dem Tot-Flag), B haelt die Kamera (re15_gr2_kamera_gesperrt) und der Cut ist ein
 * anderer als im ersten R1-Takt (nach B's P0-Schnitt) - zwischen P0 und P3 aendert B den Cut nur mit diesem Rueckschnitt, und die
 * Zonenkamera ist gesperrt. Ohne Explosionsschnitt ("Sperre ohne Schnitt") aendert sich der Cut nicht: dann bleibt es beim Schnitt im
 * Tot-Bild (key_tod_pruefen, Nachbesserung 2). Nur bei offenem Schluessel (key_tod_offen) - sonst bleibt B's Ablauf unberuehrt. */
static void key_rueckschnitt_pruefen(int slot)
{
    if (!re15_gr2_r1_laeuft(slot)) { s_r1_cut = -1; return; }
    s_r1_lief = 1;
    const int cut = (int)g_scd.cam_id;
    if (s_r1_cut < 0) { s_r1_cut = cut; return; }           /* erster R1-Takt: Cut nach B's P0 (Explosions-Cut oder Sperre) */
    if (cut == s_r1_cut || !re15_gr2_kamera_gesperrt() || !key_tod_offen()) return;
    const int expl = s_r1_cut;
    s_r1_cut = cut;
    re15_gr2_kamera_frei(NULL);                              /* B's R1-Programm endet hier (Cut bleibt, kein P3-Freigeben) */
    folge('R', expl, cut);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Schluessel-Schnitt statt R1-Rueckschnitt: Explosions-Cut %d -> Ausgangs-Cut %d F%u (RE2 P1-Ende "
                    "@0x801024a4-ec), Schluessel jetzt\n", expl, cut, (unsigned)g_engine.frame_count);
#endif
    key_tod_setzen();
}
int re15_gatorszene_r41_key_kamera(void) { return s_key_cut; }
/* Pruefhaken (M5/M2): Primitivfarbe (hellster Kanal) des Schluessels JETZT, wie der Prop-Zeichner sie rechnet: Weltmatrix
 * und -lage ueber die Elternkette wie main.c pc_prop_world (Typ-4-Absenkung re15_prop_render_y), Lichtsatz des Cuts an der
 * Weltlage, Drehung, MD1-Normale (0,-4096,0); Ruhe = ohne Puls, Spitze = Blink-Takt 0x1f (Wert 0xBE). 0 = kein Schluessel. */
int re15_gatorszene_r41_key_prim_jetzt(int cut, int *ruhe, int *spitze)
{
    int k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k < 0 || !g_scd.props[k].active || !g_re15_room_lights_ok || cut < 0 || cut >= g_re15_room_lights.cut_count) return 0;
    int32_t r[9], t[3];
    prop_matrix(g_scd.props[k].rot_x, g_scd.props[k].rot_y, g_scd.props[k].rot_z, r);
    t[0] = g_scd.props[k].x; t[1] = g_scd.props[k].y; t[2] = g_scd.props[k].z;
    for (int cur = k, tiefe = 0; tiefe < 4 && g_scd.props[cur].parent_obj >= 0; tiefe++) {
        const int ps = prop_slot((uint8_t)g_scd.props[cur].parent_obj);
        if (ps < 0 || ps == cur) break;
        int32_t pr[9], nr[9], nt[3];
        prop_matrix(g_scd.props[ps].rot_x, g_scd.props[ps].rot_y, g_scd.props[ps].rot_z, pr);
        const int32_t pt[3] = { g_scd.props[ps].x, re15_prop_render_y((int)g_scd.props[ps].obj_type, g_scd.props[ps].y),
                                g_scd.props[ps].z };
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++)
                nr[row*3+col] = (int32_t)(((int64_t)pr[row*3] * r[col] + (int64_t)pr[row*3+1] * r[3+col] + (int64_t)pr[row*3+2] * r[6+col]) >> 12);
            nt[row] = (int32_t)(((int64_t)pr[row*3] * t[0] + (int64_t)pr[row*3+1] * t[1] + (int64_t)pr[row*3+2] * t[2]) >> 12) + pt[row];
        }
        memcpy(r, nr, sizeof r); memcpy(t, nt, sizeof t);
        cur = ps;
    }
    t[1] = re15_prop_render_y((int)g_scd.props[k].obj_type, t[1]);
    re15_actor_lightctx_t welt, ctx, sp;
    uint8_t cr, cg, cb;
    re15_light_setup_actor(&g_re15_room_lights.cuts[cut], t, NULL, &welt);
    re15_light_ctx_rotate_for_bone(&welt, r, &ctx);
    re15_light_shade_vertex(&ctx, 0, -4096, 0, &cr, &cg, &cb);
    if (ruhe) *ruhe = cr > cg ? (cr > cb ? cr : cb) : (cg > cb ? cg : cb);
    sp = ctx;
    re15_blink_r41_lichtctx((uint16_t)g_current_room_id, RE15_GSZ_KEY_OBJ, RE15_BLINK_R41_UP_BELOW, &sp);
    re15_light_shade_vertex(&sp, 0, -4096, 0, &cr, &cg, &cb);
    if (spitze) *spitze = cr > cg ? (cr > cb ? cr : cb) : (cg > cb ? cg : cb);
    return 1;
}
int re15_gatorszene_r41_key_prim(int *ruhe, int *spitze)
{
    if (ruhe) *ruhe = s_key_prim_ruhe;
    if (spitze) *spitze = s_key_prim_spitze;
    return s_key_cut;
}
void re15_gatorszene_r41_tick(void)
{
    const uint16_t basis = (uint16_t)(g_current_room_id & 0xFFFEu);
    key_tod_pruefen();                                       /* Rueckfall ohne Boss-Takt (Nachtakt erkennt den Tod frueher) */
    key_einstellung_takt();                                  /* Nachbesserung 2 (M2): Ende der Einstellung (Messzeile) */
    if (basis == RE15_GSZ_RAUM && s_key_neigung_offen) key_neigung_nachholen();   /* Ersatzlage nach dem Laden (M2) */
    if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_ANKUNFT && re15_game_flag_get(5, RE15_GSZ_SIG_KNALL)) {
        re15_game_flag_set(5, RE15_GSZ_SIG_KNALL, 0);
        /* Knall-Bank in den Tuerbank-Platz (wie irons_tod_1150.c knallbank_laden; die Tuersequenz nach 2080 ersetzt sie), dann
         * Satz 0 = der Tuerknall der Irons-Montage ((5,28) -> re15_audio_re2_tuer_se(KNALL_SE_TUER), irons_tod_1150.c). */
        int ok = re15_audio_re2_tuer_laden(k_knall_bank, (int)KNALL_BANK_SIZE);
        if (ok) re15_audio_re2_tuer_se(KNALL_SE_TUER);
        folge('T', KNALL_SE_TUER, ok);
    }
    musik_takt_vor();
    if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_RUECKKEHR && re15_game_flag_get(5, RE15_GSZ_SIG_GATOR)) {
        re15_game_flag_set(5, RE15_GSZ_SIG_GATOR, 0);
        memset(&s_ga, 0, sizeof s_ga);
        s_rel_ok = 0;
        s_zustand = RE15_GSZ_GATOR;
        folge('S', 0, 0);
    }
}

/* ==== 5. Tor 2090 <-> 20A0 (Nachtrag 21; Form + Belege re15_gatorszene_r41.h) ======================================= */
/* Texte = ROOM20A0.RDT Nachrichtenblock @0x2008: msg 1 @0x203C "It's locked." und msg 2 @0x204C "You've opened the lock.",
 * bytegleich (Satzkopf `04 02`, Ende `01 00`; der Riegel vergleicht mit der ausgelieferten RDT). */
static const uint8_t k_tor_msg_zu[16] = {
    0x04,0x02,0x25,0x50,0x3a,0x4f,0x00,0x48,0x4b,0x3f,0x47,0x41,0x40,0x57,0x01,0x00,
};
static const uint8_t k_tor_msg_auf[27] = {
    0x04,0x02,0x35,0x4b,0x51,0x3a,0x52,0x41,0x00,0x4b,0x4c,0x41,0x4a,0x41,0x40,0x00,0x50,0x44,0x41,0x00,0x48,0x4b,0x3f,
    0x47,0x57,0x01,0x00,
};
/* Leeres Ereignis-Programm (Evt_end `01 00`): der Druck erledigt alles in C wie RE2s Tuer-Handler LAB_80051514 (schluessel_r36.c). */
static const uint8_t k_tor_evt_end[2] = { 0x01, 0x00 };
#define GSZ_TOR_SAT        0x31     /* sat der Tuer-Saetze selbst: ROOM2090 @0x009B9 / ROOM20A0 @0x01749 `31` */
#define GSZ_TOR_MASKE      0xffffu  /* Pausemaske jeder RE1.5-Raummeldung am Platz (re15_schluessel_r36.h RE15_R36Q_MASKE) */
static uint8_t s_tor_slot = 0;      /* 1 + gesperrter Slot, 0 = nichts gesperrt */
static int16_t s_tor_next[3];       /* Door_aot_set-Nutzlast +0/+2/+4 fuers Zurueckwidmen (Aot_reset @0x80040788-a8) */
int re15_gatorszene_r41_tor_gesperrt(void) { return s_tor_slot != 0; }
const uint8_t *re15_gatorszene_r41_tor_text(int nr, int *out_len)
{
    const uint8_t *b = nr == 0 ? k_tor_msg_zu : nr == 1 ? k_tor_msg_auf : NULL;
    if (out_len) *out_len = nr == 0 ? (int)sizeof k_tor_msg_zu : nr == 1 ? (int)sizeof k_tor_msg_auf : 0;
    return b;
}

static void tor_meldung(int id, const uint8_t *b, size_t n)
{
    re15_msg_install_text((unsigned char)id, b, n);
    int d = re15_msg_compute_duration(b, n, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)id, d);
}

static void tor_sperren(uint16_t room_id)
{
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    s_tor_slot = 0;
    if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_TOR)) return;              /* schon aufgeschlossen */
    int slot, ereignis;
    if (basis == RE15_GSZ_RAUM)  { slot = RE15_GSZ_TOR_SLOT;     ereignis = RE15_GSZ_TOR_EREIGNIS; }
    else if (basis == 0x20A0u)   { slot = RE15_GSZ_TOR20A0_SLOT; ereignis = RE15_GSZ_TOR20A0_EREIGNIS; }
    else return;
    const re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;
    const re15_aot_door_params_t *dp = &g_aot.door_params[slot];
    if (dp->dest_room != (basis == RE15_GSZ_RAUM ? 0x0Au : 0x09u)) return;        /* nur der echte Tor-Satz */
    if (basis == RE15_GSZ_RAUM) {
        tor_meldung(RE15_GSZ_MSG_ZU, k_tor_msg_zu, sizeof k_tor_msg_zu);
        tor_meldung(RE15_GSZ_MSG_AUF, k_tor_msg_auf, sizeof k_tor_msg_auf);
    }
    s_tor_next[0] = (int16_t)dp->spawn_x; s_tor_next[1] = (int16_t)dp->spawn_y; s_tor_next[2] = (int16_t)dp->spawn_z;
    g_aot.slots[slot].band = dp->band;      /* der Ereignis-Platz liest a->band (schluessel_r36.c, gleiche Begruendung) */
    re15_aot_retype(slot, 3, GSZ_TOR_SAT, 0, (uint16_t)(ereignis << 8), 0);       /* sce 3, Nutzlast +3 = Sub (@0x80043100) */
    s_tor_slot = (uint8_t)(slot + 1);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X Slot %d (Tor) gesperrt: Ereignis %d, Bit (9,%d) = 0\n",
            (unsigned)room_id, slot, ereignis, RE15_GSZ_BIT_TOR);
#endif
}

unsigned g_re15_gsz_tor_zu = 0, g_re15_gsz_tor_auf = 0;   /* Pruefzaehler (Riegel) */
/* (Raum, Nachricht) ist eine Text-Platz-Stelle der Schloss-Ton-Tabelle (Weg Bit 0 = re15_scd_show_message)? */
static int lock_stelle(uint16_t room_id, uint8_t msg)
{
    for (int i = 0; i < re15_lock_se_site_count(); i++) {
        unsigned r = 0; int m = -1, art = 0, wege = 0;
        if (re15_lock_se_site(i, &r, &m, &art, &wege) == 0 && r == room_id && m == (int)msg && (wege & RE15_LOCK_WEG_AOT))
            return 1;
    }
    return 0;
}
static const uint8_t *tor_druck(uint16_t room_id)
{
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    if (!s_tor_slot || (basis != RE15_GSZ_RAUM && basis != 0x20A0u)) return NULL;
    const int slot = s_tor_slot - 1;
    if (basis == RE15_GSZ_RAUM && re15_inv_find_item(RE15_GSZ_KEY_ITEM) >= 0) {
        /* RE2 LAB_80051514 Treffer-Zweig: Besitz VOR dem Oeffnen (@0x80051628/34), Text (@0x8005164c), Aufschliess-Ton
         * (@0x80051654/58), Wegwerf-Fortsetzung (@0x80051670), Schlossbit (@0x80051690); kein Durchgang bei diesem Druck. */
        (void)re15_discard_besitz_vor_nachricht(room_id, RE15_GSZ_MSG_AUF);
        re15_scd_show_message(RE15_GSZ_MSG_AUF, GSZ_TOR_MASKE << 16);
        re15_audio_re2_door_se(RE2_DOOR_SE_AUF_KEY);
        re15_discard_notice_message(room_id, RE15_GSZ_MSG_AUF);
        re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_TOR, 1);
        g_aot.slots[slot].band = g_aot.door_params[slot].band;
        re15_aot_retype(slot, 2, GSZ_TOR_SAT, (uint16_t)s_tor_next[0], (uint16_t)s_tor_next[1], (uint16_t)s_tor_next[2]);
        g_aot.slots[slot].was_inside = 1;
        s_tor_slot = 0;
        g_re15_gsz_tor_auf++;
        folge('U', slot, 1);
    } else {
        /* Nicht-Treffer-Zweig: GENAU EIN Verschlossen-Ton (RE2 @0x800516a0 `lui a0,0x216` / @0x800516a4 `jal 0x8005ba28`, Art M
         * der Mini-Bank TUERSE.VBS) + Text (@0x800516b8). Steht die Meldung als Text-Platz-Stelle in gen/lock_se_sites.inc
         * (20A0/20A1 msg 1 @0x0203C, Zeile 49/50), spielt re15_scd_show_message diesen Ton schon (re15_lock_se_notice, scd_vm.c
         * RE2-ERGAENZUNG) - dann hier keiner (Nachbesserung 1, M3: vorher zwei Aufrufe). 2090/2091 msg 42 ist Port-Text,
         * dort spielt ihn dieser Zweig. */
        const uint8_t msg = basis == RE15_GSZ_RAUM ? RE15_GSZ_MSG_ZU : RE15_GSZ_MSG20A0_ZU;
        if (!lock_stelle(room_id, msg)) re15_audio_re2_door_se(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
        re15_scd_show_message(msg, GSZ_TOR_MASKE << 16);
        g_re15_gsz_tor_zu++;
        folge('Z', slot, 0);
    }
    return k_tor_evt_end;
}

int re15_gatorszene_r41_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item)
{
    if (((uint16_t)room_id & 0xFFFEu) != RE15_GSZ_RAUM || msg_id != RE15_GSZ_MSG_AUF) return 0;
    if (out_item) *out_item = RE15_GSZ_KEY_ITEM;
    return 1;
}

/* ==== 4. RE2-Kampfmusik ============================================================================================ */
/* RE2 ROOM40A0: Raumtabellen-Eintrag `5a ff` (INIT_TBL.DAT Szenarioblock Tabelle @0x22C, Stage-Versatz @0x800a80b0 = 0x4d,
 * Eintrag 0x4d + 0x0A @0x2DA; beide Bloecke @0x2DA / @0x19A2 gleich) = MAIN 0x1A mit Manuell-Start-Bit 0x40 (FUN_8005a444
 * laedt, spielt nicht), gespielt von sub11 `51 00 01 00 78 40` @SCD 0x071F (Handler @0x80057e20: a0 = pc1<<28|pc2<<24|..,
 * FUN_8005b6f0 Fall 1 = SsSeqPlay) beim Intro, ausgeblendet von `51 00 05 00 00 00` @0x0762/@0x078A (Fall 5 FUN_8007a120)
 * nach dem Tod. Datei = COMMON/SOUND/MAIN1A.BGM (Index 0x1A der MAIN-Dateitabelle @0x800a8168), Container wie RE1.5
 * (Anhang u32 @size-4 SEQ 0, @size-8 VH 0x64, @size-12 VB 0xE88; FUN_8005a97c) -> shared_assets/RE2/SOUND/MAIN1A.BGM bytegleich.
 * PORT-Weiche: Tabellen-Eintrag 0xFF3C (Muster cut_10f0.c 0xFF01) - MAIN-Platz 0x3C ist im RE1.5-Raumtabellen-Zensus
 * (SS_BGMTBL UNK_80074828) unbenutzt und SOUND/MAIN3C.BGM ein 4-Byte-Platzhalter; der Lader liest ihn aus RE2/SOUND/MAIN1A.BGM.
 * Wann (NUTZER-VORGABE Nachtrag 6 "Danach geht der Kampf los, mit der Kampfmusik"): ab dem Kampfbeginn der Szene bzw. ab dem
 * Auftauchen beim spaeteren Betreten (RE2: Musik mit dem Intro), bis der Alligator tot ist (RE2: Ausblenden nach dem Tod). */
#define GSZ_BGM_EINTRAG   0xFF3C
#define GSZ_BGM_PLATZ     0x3C
static int s_bgm_stand = 0;                 /* letzte Auskunft an die Audio-Schicht: 1 = 0xFF3C */
static int musik_soll(void)
{
    if ((g_current_room_id & 0xFFFEu) != RE15_GSZ_RAUM) return 0;
    if (!re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN)) return 0;
    if (gator_tot()) return 0;
    re15_actor_t *g = gator_aktor();
    if (!g) return 0;
    if (re15_gator_boss_untergetaucht(g)) return 0;
    return 1;
}
int re15_gatorszene_r41_bgm_eintrag(int stage, int room)
{
    (void)stage; (void)room;
    int e = musik_soll() ? GSZ_BGM_EINTRAG : -1;
    s_bgm_stand = (e >= 0);
    return e;
}
const char *re15_gatorszene_r41_bgm_re2(const char *name, int slot)
{
    if (name && name[0] == 'M' && slot == GSZ_BGM_PLATZ && s_bgm_stand) return "SOUND/MAIN1A.BGM";
    return NULL;
}
static void musik_takt(void)
{
    const int soll = musik_soll();
    if (soll == s_bgm_stand) return;
    const int stage = (int)((g_current_room_id >> 12) & 0xf) - 1;
    const int room  = (int)((g_current_room_id >> 4) & 0xff);
    folge('B', soll, 0);
    re15_audio_start_room_bgm(stage, room);  /* Weiche wie cut_10f0.c bgm_fenster_tick: MAIN wechselt -> Ausblendung + Load */
    s_bgm_stand = soll;
}
