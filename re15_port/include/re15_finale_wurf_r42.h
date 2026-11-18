/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 3 (Plan S5, Konzept K-13): die WURFSZENE. John wirft Leon (ROOM5090) bzw. Elza
 * (ROOM5091) den Raketenwerfer zu, mit Zuruf, danach liegt der Werfer mit 2 Raketen zum Aufheben bereit.
 *
 * NUTZERWUNSCH 07.11.2026 (woertlich): "... machen wir ein finale wie beim Golem am Ende von Resident Evil 2. Das jemand Leon den
 * Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die Rakete angeflogen kommen und den Gegner
 * zersprengt."
 * NUTZERENTSCHEID 17.11.2026 (FINALE_PLAN F1/F2): "Ok, passt für mich. Aber ein Spruch wäre schon gut beim zuwerfen, so etwas wie:
 * Leon, send him to hell, oder Leon, let's finish this oder sowas." -> v0.8.31: John wirft bei Leon UND Elza; Zuruf John->Leon
 * "Leon, catch! Send him to hell!", John->Elza "Elza, catch! Let's finish this!" als Message_on mit Untertitel (VOICE-Option gilt,
 * Stimme synchro/STAGE5/room509x/main03.wav, solange sie fehlt nur Text).
 *
 * VORLAGE = RE2 ROOM7020 (info/re2leon/PL0/RDT/ROOM7020.RDT, Datei-Offsets @0x0xxxx) MAIN sub11 (Ausloeser, finale_r42.c), sub12
 * @0x0181A (Szene) und sub13 @0x019B2 (Flug). Ada -> John (Typ 0x49), Leon -> Leon/Elza. Belege je Konstante unten; was der Zugwagen
 * anders braucht, steht als PORT-ZUORDNUNG (mit Messung, Werkzeug tools/r42_finale/u_wurf.py) bzw. PORT-WAHL.
 *
 * Ablauf (Programm aus RE1.5-Opcodes wie annette_r41.c / kanister2080_r41.c; Signale Bank 5 Wort 0):
 *   W     Set(2,7,1) @0x0181A, Set(1,27,1) @0x0181E; Form ruht (+0x10E = 0x8000 @0x01822-26: Typroutine uebersprungen -> Pose steht);
 *         Kamera/Lagen merken @0x0182A-0x0184A; Cut_chg 7 @0x0184E -> Ankunfts-Schnitt der Tuer nach Wagen C; John erscheint an der
 *         Ankunft (Sce_em_set); Werfer an Johns Teil 10 (Super_set @0x01854); Plc_motion Wurf-Clip @0x01878
 *   W+1   Sleep 1 @0x0187C; Zuruf = Message_on (Xa_on 0 @0x01888 + Message_on 5 @0x0188C; K-11-Weg)
 *   +30   Plc_motion Wurf-Clip noch einmal (Sleep 30 @0x01892, @0x018AA)
 *   +20   Loslassen (Sleep 20 @0x018AE, Super_reset @0x018B6, Flug sub13 @0x018BE)
 *   Flug  je Bild Lage += v, v += a bis y >= Boden - 800 (@0x019CE-0x019D8); Landung: Set(5,0x0E), Se_on Raumbank 0x0E, Lage + Drehung
 *         fest (@0x019DE-0x01A02)
 *   ...   Wsleep @0x018C2 (Zuruf zu Ende), Do..Edwhile Ck(5,0x0E) @0x018C4-0x018CC, Sleep 15 @0x018D0, Lagen @0x018D4-0x018FA,
 *         Cut_chg 11 @0x018FC (Reaktion), Sleep 1 @0x018FE, Wsleep @0x01942 (Leons Antwort Xa 1 = Lesefenster), Cut zurueck
 *         @0x01944/@0x01948, Aot_reset 5 @0x0194A (Werfer aufhebbar), Ada parken @0x01954, Form-KI an @0x01968-76, Lagen zurueck
 *         @0x0197A-0x019A6, Sleep 1 @0x019AA, sub18 @0x01B54-0x01B60 (Plc_ret, Set(2,7,0), Set(1,27,0), Cut_auto 1).
 *
 * Messschiene: dieselbe wie finale_r42.c (RE15_FINALE_LOG=1 -> finale.log, Zeilen "[finale] T.. F.. Wurf: ...").
 */
#ifndef RE15_FINALE_WURF_R42_H
#define RE15_FINALE_WURF_R42_H

#include <stdint.h>
#include "re15_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Werfer-Figur -------------------------------------------------------------------------------------------------------- */
#define RE15_FW_JOHN_TYP     0x49   /* John (ROOM2001 @0x0179E `44 00 49 40 ..`; ROOM6031 sub06 @0x00E56 Typ 0x49) */
#define RE15_FW_JOHN_GRID    0x40   /* stehender NPC (ROOM2000/2001 pc[3] = 0x40, re15_kanister2080_r41.h RE15_K2080_NPC_GRID) */
#define RE15_FW_JOHN_SLOT    8      /* PORT-WAHL Skript-Slot wie RE15_K2080_NPC_SLOT (5090 belegt 0/1 @0x0122A/@0x0124A, 5091 0 @0x01232) */
/* PORT-ZUORDNUNG "Ada steht auf dem Steg" -> "John kommt durch die einzige Tuer nach Wagen C": Lage, Gier und Schnitt = Ziel von
 * ROOM5090 main00 Door_aot_set Slot 2 @0x0108E `.. e0 60 00 00 02 a4 00 08 04 09 0e ..` (5091 bytegleich, tools/r42_finale/u_wurf.py
 * Assert) = die Ankunft jeder Figur, die Wagen C betritt. Gemessen (u_wurf.py): im Schnitt 14 Bild-x +0,324, Groesse 0,0332 - RE2
 * Schnitt 7 zeigt Ada mit +0,022 / 0,0323 (Ada (-28000,-7500,-17900) @0x01868 Gier 3700 @0x01870). */
#define RE15_FW_JOHN_X       24800
#define RE15_FW_JOHN_Z     (-23550)
#define RE15_FW_JOHN_GIER    0x0800
#define RE15_FW_CUT_WURF     14     /* RE2 Cut_chg 7 @0x0184E -> Ankunfts-Schnitt @0x0108E +24 */
/* Wurf-Clip: RE2 Plc_motion `3f 0a 12 40` @0x01878 / @0x018AA = Clip 0x12 des ROOM7020-RBJ (55 Bilder) -> JOHN_49_WURF.EMD Paar 1
 * Clip 24 (tools/r42_finale/john_wurf_modell.py: gleiches Gerippe, 15 Knochen, Eltern gleich, Keyframe 80 B). Port: Plc_motion(1, 24)
 * = State 4 Sub 1 = Paar 1 (main.c Executor-Kanal-Render). Flags 0 wie die NPC-Gesten des Ports (kanister2080_r41.c OP_MOTION). */
#define RE15_FW_CLIP_WURF    24
#define RE15_FW_EMD_DATEI    "JOHN_49_WURF.EMD"
#define RE15_FW_SCHLAF_1      1     /* @0x0187C `09 0a 01 00` */
#define RE15_FW_SCHLAF_30    30     /* @0x01892 `09 0a 1e 00` */
#define RE15_FW_SCHLAF_20    20     /* @0x018AE `09 0a 14 00` */
#define RE15_FW_SCHLAF_15    15     /* @0x018D0 `09 0a 0f 00` */
/* Super_set @0x01854 `48 00 81 8a 00 00 58 02 ce ff 80 fc 00 00 00 04`: Teil pc[3] & 0x7F = 10, Versatz (0, 600, -50), Drehung
 * (-896, 0, 1024) (Handler 0x8005550c, @0x80055640-0x80055670). */
#define RE15_FW_TEIL         10
#define RE15_FW_VERSATZ_Y    600
#define RE15_FW_VERSATZ_Z   (-50)
#define RE15_FW_HAND_RX     (-896)
#define RE15_FW_HAND_RZ      1024
/* Super_reset @0x018B6 `49 00 40 fb 40 02 00 04`: Drehung (-1216, 576, 1024) (Handler 0x800558b0). Landung Dir_set @0x019FA
 * `33 00 00 fc 00 00 00 04` = (-1024, 0, 1024). PORT-ZUORDNUNG: beide gelten in RE2 fuer Ada mit Gier 3700 (@0x01870); im Port um
 * (Gier John - 3700) um die Hochachse gedreht (Weltdrehung links), damit der Werfer beim Loslassen in der Hand bleibt, wie bei Ada. */
#define RE15_FW_FLUG_RX     (-1216)
#define RE15_FW_FLUG_RY       576
#define RE15_FW_FLUG_RZ      1024
#define RE15_FW_LIEG_RX     (-1024)
#define RE15_FW_LIEG_RZ      1024
#define RE15_FW_ADA_GIER     3700   /* Dir_set `33 00 00 00 74 0e 00 00` @0x01870 */
/* Flug sub13: Speed_set 6 -> 5, 7 -> 20, 8 -> 0 (@0x019CA/@0x019C6/@0x019C2) = Beschleunigung (5, 20, 0) je Bild (Add_aspeed
 * 0x80055b2c: v += a), Add_speed 0x80055ab0 (Lage += v, Weltachsen @0x80055abc-e0), Ende sobald y >= -800 (Member_cmp `3e 00 0c 03
 * e0 fc` @0x019D8). RE2-Startgeschwindigkeit (25, 20, 10) (@0x019BE/@0x019BA/@0x019B6) gilt fuer Adas Steg (y -7500): von dort
 * faellt der Werfer 28 Bilder. PORT-ZUORDNUNG (Plan U7): aus Johns Standhoehe ist die Startgeschwindigkeit der Weg der Wurfhand
 * (Teil 10 + Versatz) im Loslassbild des Clips (Bild -> Bild + 1), die Beschleunigung die von sub13 im Blickrahmen des Werfers
 * (RE2 wirft nach +x, wie re15_kanister2080_r41.h vektor_drehen). */
#define RE15_FW_FLUG_AX         5
#define RE15_FW_FLUG_AY        20
#define RE15_FW_FLUG_AZ         0
#define RE15_FW_FLUG_STOP     800
#define RE15_FW_FLUG_MAX      120   /* Notbremse der Schleife (kein Boden erreicht) - RE2 kennt keine */
#define RE15_FW_SE_LANDUNG   0x0E   /* Se_on `36 02 0e 00 ..` @0x019E2 -> Raumbank R702 Satz 0x0E (re15_finale_r42.h) */
/* Reaktion: RE2 stellt Leon auf (-18600,0,-21700) Gier 0xF8F8 & 0xFFF = 0x8F8 (@0x018D8/@0x018E0) = Peilung Leon -> Ada - 2
 * (u_wurf.py: 4094), und den Tyrant ausser Sicht (@0x018EC (-8000,0,-25000)); Schnitt 11 zeigt Leon von hinten (frontal -0,96).
 * PORT-ZUORDNUNG: Leon auf die "Game over"-Lage der Stufe 1 (RE15_FI_GO_*: Schnitt 15, gemessen +0,15 / 0,085 gegen RE2 Schnitt 11
 * +0,18 / 0,089), Gier = Peilung Leon -> John - 2, die Form unsichtbar (statt geparkt). */
#define RE15_FW_CUT_REAKTION  15
#define RE15_FW_LEON_REL     (-2)   /* (0x8F8 - Peilung((-18600,-21700) -> (-28000,-17900))) & 0xFFF = 4094 */
/* Leons Antwort Xa 1 (@0x0190A/@0x01918 `59 00 01 00`): STAGE7.BIN @0x14A `04 00 54 00 1d 00` = Kanal 4, Block 84, Laenge 29 ->
 * 29*16 - 0x1E = 434 Sektoren / 150 je s (@0x80012ae8-0x80012b10) = 2,893 s = 86,8 Bilder -> Wsleep @0x01942 wartet 87. Der Port
 * hat fuer diese Stelle keine Zeile (F2: nur der Werfer ruft) - PORT-WAHL: das RE2-Lesefenster bleibt als Dauer des Reaktions-Schnitts. */
#define RE15_FW_REAKTION_BILDER 87
/* Zuruf: Xa 0 (@0x01888) = STAGE7.BIN @0x144 `06 00 5c 00 18 00` = Kanal 6, Block 92, Laenge 24 -> 354 Sektoren = 2,36 s = 71 Bilder
 * (Halt der Zeile, tools/r42_finale/texte_finale.py). Id 3 = erste freie Id hinter RDT 0..1 und der Schlusszeile 2 (VERTRAG 2.1,
 * tools/r42_story/zensus_r42.py; 40..43 = Annette in 5090). */
#define RE15_FW_MSG_ZURUF     3
#define RE15_FW_ZURUF_BILDER 71
/* Aufnahme: Item_aot_set @0x01202 `4e 05 02 31 00 00 a8 99 60 a5 48 0d c4 09 11 00 02 00 28 00 00 01` = Rechteck 3400 x 2500 um
 * die Landestelle (-24650,-22000) (@0x019F2; Mitte (-24500,-21950)), Gegenstand 0x11 = RE1.5 0x12 (Raketenwerfer), ANZAHL 2
 * (@0x01212). RE1.5-Satzform ROOM40A1 @0x00ED0 `50 0a 09 31 00 00 .. 3c 00 01 00 11 00 ff 00` (Version_40%). PORT-ZUORDNUNG:
 * Rechteck um die Landestelle des Ports; AOT-Slot 6 = erster freier hinter main00 0..5 (5090/5091 @0x0104E-0x010EE). */
#define RE15_FW_ITEM          0x12
#define RE15_FW_ANZAHL        2
#define RE15_FW_AOT_W         3400
#define RE15_FW_AOT_D         2500
#define RE15_FW_AOT_SLOT      6

/* Signale Programm <-> Takt (Bank 5 Wort 0; 5090/5091 belegen (5,0x1C) und (5,32) (scd_dump_room.py), Annette 0x10..0x13,
 * Abschuss 0x14/0x16/0x17 (re15_finale_r42.h)). */
#define RE15_FW_SIG_WURF      0x18u   /* Programm -> Takt: loslassen (Super_reset @0x018B6 + sub13) */
#define RE15_FW_SIG_GELANDET  0x19u   /* Takt -> Programm: Landung (Set(5,0x0E) @0x019DE) */
#define RE15_FW_SIG_WSLEEP    0x1Au   /* Takt -> Programm: Zuruf und Stimme zu Ende (Wsleeping @0x80053ac0, kanister2080_r41.h) */
#define RE15_FW_SIG_REAKTION  0x1Bu   /* Programm -> Takt: Lagen fuer den Reaktions-Schnitt (@0x018D4-0x018FA) */
#define RE15_FW_SIG_ENDE      0x1Du   /* Programm -> Takt: Werfer frei, John parken, Form-KI an, Lagen zurueck (@0x0194A-0x019A6) */

/* Zustand des Werfers (Gegenstand) */
enum { RE15_FW_W_AUS = 0, RE15_FW_W_HAND, RE15_FW_W_FLUG, RE15_FW_W_LIEGT, RE15_FW_W_GENOMMEN };

/* Szene starten (finale_r42.c, im Bild des sub11-Ausloesers). Rueckgabe 0 = gestartet. */
int  re15_finale_wurf_start(int form_slot);
/* Je Spieltakt in 5090/5091 (finale_r42.c re15_finale_r42_vor_spielzug). */
void re15_finale_wurf_takt(void);
/* 1, solange die Szene laeuft (Form gehoert ihr, kein Abschuss). */
int  re15_finale_wurf_aktiv(void);
int  re15_finale_wurf_besitzt(int slot);
void re15_finale_wurf_reset(void);
/* Werfer zum Zeichnen (PC): Weltlage + Drehung Q12; 0 = nicht zu zeichnen. */
int  re15_finale_wurf_werfer_lage(int32_t pos[3], int32_t rot[9]);
int  re15_finale_wurf_werfer_zustand(void);
/* EMD-Ersatz fuer John in 5090/5091 (platform/pc/main.c pc_enemy_read_re15_emd), sonst NULL. */
const char *re15_finale_wurf_emd_datei(uint8_t type);
/* item_modal_common.c Zustand 5, VOR der Platz-Vorpruefung: Aufnahme DIESES Werfers (Taken-Bit) - Luecken schliessen, damit der
 * 2-Zellen-Schub (inventory_common.c:40-55) keine Zellen 8/9 verliert (Auftrag Stufe 3). */
void re15_finale_wurf_vor_aufnahme(uint8_t type, uint8_t taken_bit);
/* Programm (nur Riegel): gebaut fuer die aktuellen Lagen. */
const uint8_t *re15_finale_wurf_programm(int *out_len);
/* Flugbahn (nur Riegel): Start, v, Gier -> Ende/Bilder (Schleife wie der Takt). */
void re15_finale_wurf_bahn(const int32_t start[3], const int32_t v[3], int16_t gier, int32_t ende[3], int *bilder);
/* Luecken im Inventar schliessen (FUN_8004dadc so oft, bis keine Luecke vor einem belegten Platz liegt). Rueckgabe = Schuebe. */
int  re15_finale_wurf_luecken_schliessen(void);

#ifdef __cplusplus
}
#endif

#endif /* RE15_FINALE_WURF_R42_H */
