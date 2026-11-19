/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 3 (Plan S5, Konzept K-13): die WURFSZENE. John wirft Leon (ROOM5090) bzw. Elza
 * (ROOM5091) den Raketenwerfer zu, mit Zuruf, danach liegt der Werfer mit 2 Raketen neben dem Spieler zum Aufheben bereit.
 *
 * NUTZERWUNSCH 07.11.2026 (woertlich): "... machen wir ein finale wie beim Golem am Ende von Resident Evil 2. Das jemand Leon den
 * Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die Rakete angeflogen kommen und den Gegner
 * zersprengt."
 * NUTZERENTSCHEID 17.11.2026 (FINALE_PLAN F1/F2): "Ok, passt für mich. Aber ein Spruch wäre schon gut beim zuwerfen, so etwas wie:
 * Leon, send him to hell, oder Leon, let's finish this oder sowas." -> v0.8.31: John wirft bei Leon UND Elza; Zuruf John->Leon
 * "Leon, catch! Send him to hell!", John->Elza "Elza, catch! Let's finish this!" als Message_on mit Untertitel (VOICE-Option gilt,
 * Stimme synchro/STAGE5/room509x/main03.wav, solange sie fehlt nur Text).
 * NUTZER 18.11.2026 (woertlich, gilt ab Stufe 3 Runde 2):
 *   (1) "Die Wurf Animation ist aktuell noch Schrott. - zumindest in der Vorschau. Orientiere dich da mehr an Resident Evil 2."
 *   (2) "Aus der Vorschau - die Wurfanimation sah immer noch unpassend aus. Außerdem würde ich John eher am Eingang stehen lassen, als
 *       das er kurz da ist und dann mitten im Gang plötzlich verschwindet."
 *   (3) "John kann danach gerne wieder raus laufen. Sollte er auch, aber das geht schneller vom Eingang aus."
 *   -> John erscheint am Eingang von Wagen C, wirft STEHEND (ohne Knien) von dort, geht danach durch dieselbe Tuer hinaus; Kamera
 *      frontal und hell auf John; der Werfer fliegt sichtbar im Bogen bis neben den SPIELER.
 *
 * VORLAGE = RE2 ROOM7020 (info/re2leon/PL0/RDT/ROOM7020.RDT, Datei-Offsets @0x0xxxx) MAIN sub11 (Ausloeser, finale_r42.c), sub12
 * @0x0181A (Szene) und sub13 @0x019B2 (Flug). Ada -> John (Typ 0x49), Leon -> Leon/Elza. Der Wurf selbst kommt aus RE1.5 (Handgranate
 * des Spielers, PSX.EXE @0x800xxxxx, Bank PLD/PL00W09.PLW). Belege je Konstante unten; was der Zugwagen anders braucht, steht als
 * PORT-ZUORDNUNG (mit Messung, Werkzeug tools/r42_finale/u_wurf.py, Messzeilen finale.log) bzw. PORT-WAHL.
 *
 * Ablauf (Programm aus RE1.5-Opcodes wie annette_r41.c / kanister2080_r41.c; Signale Bank 5 Wort 0). W = erstes Programmbild
 * (sub12 Bild 0; der Ausloeser laeuft im Port in der Form-KI ein Bild davor, die VM vor dem Spielzug):
 *   W     Set(2,7,1) @0x0181A, Set(1,27,1) @0x0181E; Form RUHT und bleibt SICHTBAR (+0x10E = 0x8000 @0x01822-26; die RE2-Schleife
 *         @0x80026590 lhu 0x10e / @0x80026598 andi 0x8000 / @0x8002659c bne ueberspringt nur SquareRoot0 + Typ-Handler jalr
 *         @0x800265f4 - keine KI, kein Bildvorschub, KEIN Ausblenden; Runde 4, Pruefbericht s3 r3 Rot); Kamera/Lagen merken
 *         @0x0182A-0x0184A; Cut_chg 7 @0x0184E -> Schnitt 13 (frontal auf die Tuer); John erscheint an der Ankunft von Wagen C
 *         (Sce_em_set statt Pos_set @0x01868), Blick zum Spieler; Werfer (PL00W12) in Johns rechter Hand Teil 11 wie bei Leon (statt
 *         Super_set @0x01854 an Adas Teil 10, Runde 3); Plc_motion HEBEN
 *         (@0x01878)
 *   W+31  Sleep 1 @0x0187C + Sleep 30 @0x01892, Plc_motion WURF (@0x018AA)
 *   Los   Clip-Bild 22 des Wurfs (RE1.5 @0x800336fc statt Sleep 20 @0x018AE): Loslassen (Super_reset @0x018B6, Flug sub13 @0x018BE),
 *         Zuruf = Message_on (PORT-WAHL "beim Loslassen", RE2 Xa_on 0 @0x01888)
 *   Flug  je Bild Lage += v, v += a bis y >= -800 (@0x019CE-0x019D8); Flugkamera nach Kopf "Flugkamera" (Stufe 4 Runde 3): zeigt der
 *         Schnitt 13 Landestelle und Spieler, bleibt er (RE2); sonst Schnitt auf die Kamera des Spielers (bzw. eine Lauf-Kamera von
 *         Wagen C), sobald der Werfer eine Werferlaenge von der Hand weg ist (Runde 3); Landung: Set(5,0x0E), Se_on Raumbank 0x0E, Lage + Drehung fest (@0x019DE-0x01A02)
 *   ...   Landung abwarten (Do..Edwhile @0x018C4-0x018CC), Sleep 15 @0x018D0, Reaktion: Spieler blickt zu John (Dir_set @0x018E0),
 *         Cut_chg 11 @0x018FC -> Schnitt 13, Sleep 1 @0x018FE, John laeuft zur Tuer (Plc_dest Modus 5 @0x0193A), an der Tuer Tuerton,
 *         Cut zurueck @0x01944/@0x01948, Aot_reset 5 @0x0194A (Werfer aufhebbar), DANACH John geparkt (@0x01958; nur ausser Bild,
 *         Runde 3), Form-KI an @0x01968-76, Sleep 1
 *         @0x019AA, sub18 @0x01B54-0x01B60 (Plc_ret, Set(2,7,0), Set(1,27,0), Cut_auto 1).
 *
 * Messschiene: dieselbe wie finale_r42.c (RE15_FINALE_LOG=1 -> finale.log, Zeilen "[finale] T.. F.. Wurf: ...").
 */
#ifndef RE15_FINALE_WURF_R42_H
#define RE15_FINALE_WURF_R42_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_door_seq.h"   /* re15_door_seq_anfrage_t (Tuerton-Haken) */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Werfer-Figur -------------------------------------------------------------------------------------------------------- */
#define RE15_FW_JOHN_TYP     0x49   /* John (ROOM2001 @0x0179E `44 00 49 40 ..`; ROOM6031 sub06 @0x00E56 Typ 0x49) */
#define RE15_FW_JOHN_GRID    0x40   /* stehender NPC (ROOM2000/2001 pc[3] = 0x40, re15_kanister2080_r41.h RE15_K2080_NPC_GRID) */
#define RE15_FW_JOHN_SLOT    8      /* PORT-WAHL Skript-Slot wie RE15_K2080_NPC_SLOT (5090 belegt 0/1 @0x0122A/@0x0124A, 5091 0 @0x01232) */
/* NUTZER 18.11. (2)/(3): John steht am EINGANG von Wagen C = Ziel der einzigen Tuer nach Wagen C (ROOM5090 main00 Door_aot_set Slot 2
 * @0x0108E `.. e0 60 00 00 02 a4 00 08 04 09 0e ..` = (24800,0,-23550), Richtung 0x0800, Schnitt 0x0E; 5091 bytegleich) = die Ankunft
 * jeder Figur, die Wagen C betritt. RE2 setzt Ada an den Wurfort (Pos_set @0x01868, Dir_set 3700 @0x01870: Ada blickt in Wurfrichtung,
 * RE2-v (25,10) liegt 148/4096 neben ihrer Gier). PORT-WAHL: Johns Gier = Peilung John -> Spieler beim Szenenstart (er wirft zum
 * Spieler); RE15_FW_JOHN_GIER bleibt die Ankunfts-Gier der Tuer (Riegel, Rueckfall ohne Spieler). */
#define RE15_FW_JOHN_X       24800
#define RE15_FW_JOHN_Z     (-23550)
#define RE15_FW_JOHN_GIER    0x0800
/* PORT-ZUORDNUNG Wurf-Kamera (RE2 Cut_chg 7 @0x0184E zeigt Ada frontal: Bild-x +0,022, frontal 0,985, u_wurf.py): Schnitt 13 (RID
 * 5090 @0x60 + 13*32: Auge (11050,-3110,-24812) blickt den Gang entlang zur Tuer, 5091 bytegleich) zeigt John an der Ankunft von
 * vorn (frontal 0,996 bei Gier 0x0800, Bild-x +0,299, Groesse 0,0167); der Ankunfts-Schnitt 14 zeigt ihn von der Seite (frontal 0,164,
 * Runde 1 - "Schrott"). Keine andere Kamera von Wagen C hat John im Bild (u_wurf.py: Schnitte 11/12/15 hinter dem Auge bzw. Bild-x > 1). */
#define RE15_FW_CUT_WURF     13
#define RE15_FW_CUT_REAKTION 13     /* RE2 Cut_chg 11 @0x018FC zeigt Leon zum Werfer und Ada beim Abgang (Plc_dest @0x0193A): John geht
                                     * im Schnitt 13 zur Tuer (Runde 3: laeuft, Schnitt zurueck vor dem Parken) */
/* Wurf OHNE Knien (Vorgabe Runde 2): der Handgranaten-Wurf des RE1.5-Spielers aus PLD/PL00W09.PLW, auf Johns Gerippe (gleiche Eltern,
 * gleiche Beine; tools/r42_finale/john_wurf_modell.py) als Clips 24/25 in Paar 1 von JOHN_49_WURF.EMD. Port: Plc_motion(1, c) = State 4
 * Sub 1 = Paar 1 (main.c Executor-Kanal-Render), Flags 0 = einmal spielen und halten (op_plc_motion @0x80041bb0).
 *   HEBEN = W09-Clip 6 (10 Bilder): Feuer-FSM Sub 0 `ori v0,zero,0x6` / `sb v0,-13592(at)` (0x800acae8) @0x80032f68-0x80032f70
 *   WURF  = W09-Clip 7 (35 Bilder): Sub 2 Clip 7 + 2*hoch + 4*tief @0x800334a8-0x800334c8 (MITTE = 7)
 *   LOS   = Clip-Bild 0x16 = 22: `ori v0,zero,0x16` / `bne v1,v0` (0x800acae9) mit Flag MITTE 0x4000 -> Wurfkoerper 0x040D1000
 *           (`jal 0x80019700`) @0x800336fc-0x8003374c (Waffe 9, Gate @0x80033684-0x8003368c). Ersetzt Adas Sleep 20 @0x018AE (ihr
 *           Loslassbild im Clip 0x12). */
#define RE15_FW_CLIP_HEBEN   24
#define RE15_FW_CLIP_WURF    25
#define RE15_FW_LOS_BILD     22
#define RE15_FW_EMD_DATEI    "JOHN_49_WURF.EMD"
#define RE15_FW_SCHLAF_1      1     /* @0x0187C `09 0a 01 00` (und @0x018FE) */
#define RE15_FW_SCHLAF_30    30     /* @0x01892 `09 0a 1e 00` */
#define RE15_FW_SCHLAF_20    20     /* @0x018AE `09 0a 14 00` - nur RE2-Beleg (Adas Loslassbild), Port: RE15_FW_LOS_BILD */
#define RE15_FW_SCHLAF_15    15     /* @0x018D0 `09 0a 0f 00` */
/* RE2 Super_set @0x01854 `48 00 81 8a 00 00 58 02 ce ff 80 fc 00 00 00 04`: Teil pc[3] & 0x7F = 10, Versatz (0, 600, -50), Drehung
 * (-896, 0, 1024) (Handler 0x8005550c, @0x80055640-0x80055670) - gilt fuer ADAS Teil 10 und RE2s Werfermodell (R702, ROOM7020 Objekt 0).
 * Nur noch RE2-Beleg (Riegel liest die Bytes): Runde 2 setzte genau diese Werte an Johns Teil 10 (Unterarm) - gemessen haengt der Werfer
 * dann ~1 s waagrecht seitlich an der Huefte und verschwindet beim Ausholen hinter dem Koerper (Pruefbericht s3 r2, Rot 3).
 * Runde 3 (PORT-ZUORDNUNG, Pruefbericht "mit einer belegten Haltung statt waagrecht an der Huefte"): John haelt den Werfer GENAU WIE
 * LEON, also mit dem Griff des RE1.5-Spielers fuer den Raketenwerfer:
 *   - Teil 11 = rechte Hand: dort ersetzt das Spiel das Handmodell durch das Waffenmodell (Teil-Tabelle model+0x188, Teil 11 gepatcht,
 *     Savestate-Abgleich Skill re15-weapon-render; Port main.c "WEAPON-IN-HAND", gleiche Bone-11-Matrix). Johns Gerippe = Spielergerippe
 *     (tools/r42_finale/john_wurf_modell.py, Assert Eltern gleich).
 *   - Modell = Waffenband von PLD/PL00W12.PLW dir[2] (Raketenwerfer = Gegenstand 0x12 -> Datei W%02X = W12, main.c; finale.log Stufe 1
 *     "[equip] W-bank -> W12"): 38 Dreiecke mit v >= 224 (u 72..127, v 224..255), Ecken im Teil-11-Rahmen, Rohr entlang y von -744 bis
 *     +1027; das Handband (34 Dreiecke, v 109..154) bleibt weg, Johns eigene Hand bleibt. Textur = dir[3] (56 x 32, 8 bpp, eigene CLUT),
 *     im Spiel an (u 72, v 224) der Hautseite eingeblendet (main.c Komposit "X = 200, Y = 480") - im Port eigener Platz, UV - (72, 224).
 *     PL04W12 (Elza) hat dasselbe Waffenband und dieselbe dir[3]-TIM (md5 c06d2428). Hinweis: die 40%-Disc hat an Platz 12 ein anderes
 *     PLW (2 Waffendreiecke); der Port zeigt in Leons Hand die Datei des Asset-Baums - John haelt dieselbe.
 * Flug und Landung behalten RE2s Drehungen (Super_reset / Dir_set unten) ueber die Achsenzuordnung W12 -> R702 (PORT-ZUORDNUNG nach
 * Hauptachsen, s3r3/lieg.py): W12-y (Rohr, 1768 lang) -> R702-x (1135 lang), W12-x (Griffseite +380/-243) -> R702-y (+297/-114),
 * W12-z -> -R702-z (Rechtssystem). Liegend nach RE2 (@0x019FA) ist der W12-Werfer flach: y -177..74 (R702: -223..75). */
#define RE15_FW_TEIL         11
#define RE15_FW_ADA_TEIL     10     /* RE2-Beleg @0x01857 */
#define RE15_FW_ADA_VERSATZ_Y 600
#define RE15_FW_ADA_VERSATZ_Z (-50)
#define RE15_FW_ADA_HAND_RX (-896)
#define RE15_FW_ADA_HAND_RZ  1024
#define RE15_FW_W12_DATEI    "PLD/PL00W12.PLW"
#define RE15_FW_W12_V_MIN    224    /* Waffenband: alle drei v >= 224 */
#define RE15_FW_W12_U0       72     /* Komposit-Ursprung der dir[3]-TIM in der Hautseite (main.c X = 128 + 72) */
#define RE15_FW_W12_V0       224    /* (main.c Y = 256 + 224) */
#define RE15_FW_W12_LAENGE   1771   /* Rohr y -744..+1027 (dir[2] Ecken), Mass fuer den Schnitt im Flug */
/* Super_reset @0x018B6 `49 00 40 fb 40 02 00 04`: Drehung (-1216, 576, 1024) (Handler 0x800558b0). Landung Dir_set @0x019FA
 * `33 00 00 fc 00 00 00 04` = (-1024, 0, 1024). PORT-ZUORDNUNG: beide gelten in RE2 fuer Ada mit Gier 3700 (@0x01870) und das R702-
 * Modell; im Port um (Gier John - 3700) um die Hochachse gedreht (Weltdrehung links) und ueber die Achsenzuordnung oben auf W12. */
#define RE15_FW_FLUG_RX     (-1216)
#define RE15_FW_FLUG_RY       576
#define RE15_FW_FLUG_RZ      1024
#define RE15_FW_LIEG_RX     (-1024)
#define RE15_FW_LIEG_RZ      1024
#define RE15_FW_ADA_GIER     3700   /* Dir_set `33 00 00 00 74 0e 00 00` @0x01870 */
/* Flug sub13 (selbst gelesen): Speed_set 2 -> 10, 1 -> 20, 0 -> 25 (@0x019B6/@0x019BA/@0x019BE) = Startgeschwindigkeit (25, 20, 10),
 * Speed_set 8 -> 0, 7 -> 20, 6 -> 5 (@0x019C2/@0x019C6/@0x019CA) = Beschleunigung (5, 20, 0); Schleife Evt_next / Add_speed 0x80055ab0
 * (Lage += v, Weltachsen @0x80055abc-e0) / Add_aspeed 0x80055b2c (v += a); Ende sobald y >= -800 (Member_cmp `3e 00 0c 03 e0 fc`
 * @0x019D8). Aus Adas Abwurfhoehe (Steg y -7500 @0x01868 + Hand Teil 10 + (0,600,-50) im Clip 0x12 Bild 19/20 = -8635/-8650) dauert
 * der RE2-Flug 28 Bilder (10n^2+10n >= 7835/7850; u_wurf.py Assert, Riegel).
 * PORT-WAHL (Vorgabe Runde 2: "Flug nach RE2 sub13 ... mit an die Entfernung John-Spieler angepasster Startgeschwindigkeit"):
 * Beschleunigung, Schleife, Bodentest und Flugdauer 28 Bilder wie RE2; a_h = 5 (RE2 a_x) in Wurfrichtung (Peilung Hand -> Landestelle),
 * a_y = 20. Die Startgeschwindigkeit wird so gewaehlt, dass der Werfer nach 28 Bildern an der Landestelle liegt: je Achse
 * v = (Weg - a*378)/28 gerundet (378 = 28*27/2; bei D < 1890 a_h = D/378), v_y = die langsamste Startgeschwindigkeit (ab RE2 +20 abwaerts gezaehlt), mit
 * der die Schleife ab der Handhoehe genau nach 28 Bildern endet (re15_finale_wurf_vy).
 * Scheitel (gemessen s3 r4, Hand (23542,-1998,-23121), Spieler (8150,-23550)): y -3274 in Flugbild 11 bei x 18592. Im Schnitt 13
 * (Auge y -3110 IM Wagen) faellt der Scheitel auf die Stirnwand ueber der Tuer (unter der Lampe), nicht auf die Decke: die Sichtlinie
 * Auge -> Stirnwand durch den Scheitel ist frei, der Scheitel liegt also unter der Decke (Messbild s3r4/mb_flugbahn_s13.png).
 * Landestelle (PORT-WAHL "bis neben den SPIELER"): auf der Linie Spieler -> John, so weit vor dem Spieler, wie das RE2-Aufnahme-
 * Rechteck 3400 x 2500 (@0x0120C/0E, achsparallel um die Landestelle, Item_aot_set unten) ihn noch mit RE15_FW_LANDUNG_RAND Abstand
 * zum Rand einschliesst (er hebt ohne Laufen auf), hoechstens die halbe Strecke zu John. Laengs des Wagens (Wurf entlang x) = 1600.
 * Runde 2 nahm feste 1000: gemessen lag der Werfer dann im Schnitt 12 an Leons Fuessen, halb hinter den Beinen und in den Flammen
 * (Pruefbericht s3 r2, Rot 2); bei 1600 liegt er sichtbar vor ihm auf freiem Boden (Messbild s3r3/mb_landung_1000_1600.png). */
#define RE15_FW_FLUG_VX        25
#define RE15_FW_FLUG_VY        20
#define RE15_FW_FLUG_VZ        10
#define RE15_FW_FLUG_AX         5
#define RE15_FW_FLUG_AY        20
#define RE15_FW_FLUG_AZ         0
#define RE15_FW_FLUG_STOP     800
#define RE15_FW_FLUG_BILDER    28
#define RE15_FW_FLUG_MAX      120   /* Notbremse der Schleife (kein Boden erreicht) - RE2 kennt keine */
#define RE15_FW_LANDUNG_RAND   100    /* Abstand des Spielers zum Rand des Aufnahme-Rechtecks */
/* Flugkamera (Stufe 4 Runde 3, Pruefbericht s4 r2 Rot R1: bei Elza x 16904 lag die Landestelle x 18490 unter dem Auge der Spieler-
 * kamera 12 (x 18526) - nach dem Flugschnitt war der Werfer weder im Flug noch bei der Landung im Bild). Welche Kamera Flug und
 * Landung zeigt, haengt jetzt an der Lage, nicht an einer festen Wahl:
 *   1. RE2 ZUERST: RE2 schneidet waehrend Flug und Landung NICHT - zwischen Cut_chg 7 `29 07` @0x0184E und Cut_chg 11 `29 0b` @0x018FC
 *      steht in sub12 kein Byte 0x29 (0x01850-0x018FB), in sub13 + Landung (0x019B2-0x01A05) ebenfalls keins (selbst gelesen,
 *      ROOM7020.RDT). Der Wurf-Schnitt 13 bleibt also stehen, wenn er Landestelle UND Spieler zeigt (Elza an x 16904: 28/28 Flugbilder
 *      im Schnitt 13, finale.log s4r2/rec/elza).
 *   2. PORT-WAHL Runde 3 (unveraendert): sonst Schnitt auf die Kamera des Spielers nach einer Werferlaenge, wenn sie beides zeigt
 *      (Leon an x 8150 steht HINTER dem Auge von Schnitt 13, x 11050).
 *   3. PORT-WAHL Runde 3 erweitert: sonst die erste Lauf-Kamera von Wagen C, die beides zeigt. Lauf-Kameras = Ziele echter RVD-
 *      Wechsel von Wagen C (ROOM5090/5091 bytegleich): 11->12 @0x00490, 12->11 @0x004B8, 12->13 @0x004CC, 13->14 @0x00508 (Kamera 15
 *      hat nur ihren Anker @0x00544 = SCD-Kamera, nicht dabei). Gebraucht z. B. an x 14000 mit Kamera 13: dort zeigt weder 13 noch
 *      die Kamera des Spielers die Fuesse (unit_r42_finale_wurf t_flugkamera).
 *   "Zeigt" = der Bodenpunkt liegt im Bild 320x240 (Projektion wie der Renderer, re15_camera_build_view): Landestelle (x, 0, z) = die
 *   Lage nach der Landung (@0x019DE-0x01A02, y 0) und die Fuesse des Spielers (seine Lage). Zeigt keine Kamera beides, gilt in derselben
 *   Reihenfolge die erste, die die Landestelle zeigt (gemessen nur am Bankrand x 14000..14500, z <= -24800: dort sieht keine Kamera die
 *   Fuesse UND die Landestelle). Zeigt gar keine die Landestelle: Rueckfall Kamera des Spielers, Messzeile "zeigt Landestelle NICHT".
 *   Der Riegel (unit_r42_finale_wurf t_flugkamera) tastet den freien Boden von Wagen C (SCA-Zellen 5091 ausgespart) fuer jede
 *   Lauf-Kamera als Kamera des Spielers ab und verlangt die Landestelle stets im Bild. Keine neue Zahl: Kandidaten aus RE2 bzw. den
 *   RVD-Bytes, Massstab = Bildrand des Renderers. */
#define RE15_FW_LAUFKAMERA_11  11     /* RVD 5090/5091 @0x004B8 `01 ff 0c 0b` (12->11) */
#define RE15_FW_LAUFKAMERA_12  12     /* RVD @0x00490 `01 ff 0b 0c` (11->12), @0x004F4 (13->12) */
#define RE15_FW_LAUFKAMERA_14  14     /* RVD @0x00508 `01 ff 0d 0e` (13->14) */
#define RE15_FW_SE_LANDUNG   0x0E   /* Se_on `36 02 0e 00 ..` @0x019E2 -> Raumbank R702 Satz 0x0E (re15_finale_r42.h) */
/* Reaktion: RE2 stellt Leon auf (-18600,0,-21700) Gier 0xF8F8 & 0xFFF = 0x8F8 (@0x018D8/@0x018E0) = Peilung Leon -> Ada - 2
 * (u_wurf.py: 4094) und stellt die Lage am Ende zurueck (@0x0197A-0x0198E). PORT-WAHL: der Spieler bleibt stehen (der Werfer liegt
 * neben ihm), nur die Gier wird Peilung Spieler -> John - 2 und bleibt danach (er blickt zum Werfer, der zwischen ihm und John liegt). */
#define RE15_FW_LEON_REL     (-2)   /* (0x8F8 - Peilung((-18600,-21700) -> (-28000,-17900))) & 0xFFF = 4094 */
/* Abgang. RE2 ROOM7020 sub12 nach dem Reaktions-Schnitt (selbst gelesen, s3r3):
 *   @0x0193A `40 00 15 23 40 06 60 00` Plc_dest (Ada geht), @0x01942 `0b` Wsleep + @0x01943 `0c` Wsleeping (warten, bis Zeile/Stimme
 *   Xa 1 @0x0190A-0x01922 vorbei ist, Wsleeping @0x80053ac0), @0x01944 `1d 04 01 00` Work_copy + @0x01948 `29 00` Cut_chg = ZURUECK
 *   auf die gemerkte Kamera, @0x0194A Aot_reset, ERST DANACH @0x01954 Work_set(2) + @0x01958 Pos_set `32 00 a0 92 c8 ce 14 ba` =
 *   Ada nach y -12600 geparkt (ausser Bild). RE2 schneidet also weg, BEVOR die Figur verschwindet - kein Sleep dazwischen, der
 *   Apply-Durchlauf des Schnitts zeigt das alte Bild nicht mehr (kamerahalt_r41.c, @0x80021560).
 *   Runde 2 parkte John an der Wand IM Schnitt 13 (Pruefbericht s3 r2, Rot 1: "verschwindet vor der geschlossenen Tuer").
 * Port Runde 3: John LAEUFT (Modus 5, Nutzer 18.11. (3)) zur Tuer zurueck, durch die er kam - Tuer-AOT Slot 4 (5090/5091 @0x010CE
 * `3b 04 02 31 00 00 3e 62 2c 9d 84 03 6e 0f ..` = x 25150 + 900, z -25300 + 3950; Ziel Wagen B), Ziel = Vorderkante. Ankunft =
 * Plc_dest-Bit, die Wand (kein Fortschritt mehr) oder der Punkt 620 voraus im AOT (`ori 0x26c` @0x80042bd0, FUN_80042bac/b64).
 * PORT-WAHL fuer RE2s Warten (Wsleep auf Xa 1 - der Port hat keine zweite Zeile): bis zur Ankunft. Dann wie RE2: Schnitt zurueck,
 * Aot_reset, John geparkt - aber nur, wenn ihn die Kamera nach dem Schnitt NICHT sieht (Huelle RE15_FW_JOHN_HUELLE_*; sieht ihn die
 * zurueckgeschnittene Kamera doch - der Spieler stand selbst im Schnitt 13 -, bleibt er an der Tuer stehen (Sub 6) und wird erst geparkt,
 * sobald ihn das aktuelle Bild nicht mehr zeigt). */
#define RE15_FW_JOHN_HUELLE_XZ  650   /* Sichtpruefung: Quader um Johns Lage, gemessen am 0x49-Modell (CDEMD0.EMS, Eigenclips c0..c3 Bild 0,
                                       * s3r3): x -409..601, z -616..635 -> +-650 ... */
#define RE15_FW_JOHN_HUELLE_Y  3100   /* ... und y -3037..78 -> 0..-3100 */
#define RE15_FW_TUER_X       25150  /* Runde 3: Vorderkante des Tuer-AOT (x0) statt der Mitte - der NPC-Lauf (Sub 5) kommt bei
                                     * SquareRoot0-Abstand < 300 an (@0x80051764, enemy_ai_common.c Kopf Sub 5) und geht in Sub 6
                                     * (stehen, @0x800517f0); die Wand haelt ihn bei x 25024 (gemessen s3 r2) = 126 davor. Die Mitte
                                     * (x 25600) erreichte er nie (Runde 2: Wand-Erkennung + Parken in Sichtweite) */
#define RE15_FW_TUER_Z     (-23325)
#define RE15_FW_TUER_X0      25150
#define RE15_FW_TUER_Z0    (-25300)
#define RE15_FW_TUER_W       900
#define RE15_FW_TUER_D       3950
#define RE15_FW_GEHEN_MODUS   5    /* NUTZER 18.11. (3): "John kann danach gerne wieder raus laufen." -> Plc_dest Modus 5 = LAUFEN: NPC-
                                    * Sub-Tabelle @0x80076ca0 [5] = 0x80051484 (Mod-Baum und Version_40% gleich, s3r3), [4] = 0x80051148 Gehen */
#define RE15_FW_ABGANG_MAX  150     /* Notbremse (Bilder) fuer den Abgang - RE2 kennt keine */
/* Zuruf: Xa 0 (@0x01888) = STAGE7.BIN @0x144 `06 00 5c 00 18 00` = Kanal 6, Block 92, Laenge 24 -> 354 Sektoren = 2,36 s = 71 Bilder
 * (Halt der Zeile, tools/r42_finale/texte_finale.py). Id 3 = erste freie Id hinter RDT 0..1 und der Schlusszeile 2 (VERTRAG 2.1,
 * tools/r42_story/zensus_r42.py; 40..43 = Annette in 5090). RE2 wartet nach dem Loslassen auf das Ende von Xa 0 (Wsleep @0x018C2);
 * dort ist es bei W+72 laengst vor der Landung (W+79) vorbei, wirksam ist nur die Landung. Der Port ruft erst beim Loslassen und wartet
 * darum wie RE2 nur auf die Landung; die Zeile laeuft in den Reaktions-Schnitt weiter. */
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
#define RE15_FW_SIG_WURF      0x18u   /* Takt -> Programm: losgelassen im Clip-Bild 22 (Super_reset @0x018B6 + sub13) */
#define RE15_FW_SIG_GELANDET  0x19u   /* Takt -> Programm: Landung (Set(5,0x0E) @0x019DE) */
#define RE15_FW_SIG_REAKTION  0x1Bu   /* Programm -> Takt: Spieler blickt zu John (@0x018D4-0x018FA) */
#define RE15_FW_SIG_ENDE      0x1Du   /* Programm -> Takt: Werfer frei, Form-KI an (@0x0194A-0x019A6) */
#define RE15_FW_SIG_TUER      0x1Eu   /* Plc_dest-Ankunft bzw. Takt: John ist durch die Tuer (Ada parken @0x01958) */

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
/* PC-Haken: Tuerton beim Abgang (Argument = Anfrage der Tuer wie aot_common.c; Rueckgabe = gespielter Tonkopf-Eintrag, -1 = stumm). */
void re15_finale_wurf_tuer_ton_hook(int (*fn)(const re15_door_seq_anfrage_t *q));
/* Programm (nur Riegel): gebaut fuer die aktuellen Lagen. */
const uint8_t *re15_finale_wurf_programm(int *out_len);
/* Flugbahn (nur Riegel): Start, v, a -> Ende/Bilder (Schleife wie der Takt). */
void re15_finale_wurf_bahn(const int32_t start[3], const int32_t v[3], const int32_t a[3], int32_t ende[3], int *bilder);
/* Startwerte des Flugs (Kopf "Flug sub13"): von `start` zur Landestelle `ziel` (x/z) in RE15_FW_FLUG_BILDER Bildern. Liefert v, a und
 * die Wurfrichtung (Gier). a_h = 5 in Wurfrichtung (ganzzahlig je Achse, auf 0 gerundet), v_x/v_z danach so, dass der Weg nach 28
 * Bildern trifft (gerundete Division: hoechstens 14 daneben je Achse). */
void re15_finale_wurf_startwerte(const int32_t start[3], int32_t ziel_x, int32_t ziel_z, int32_t v[3], int32_t a[3], int16_t *gier);
/* v_y des Flugs: langsamste Startgeschwindigkeit (ab +20 abwaerts), mit der die RE2-Schleife ab y0 nach genau RE15_FW_FLUG_BILDER endet. */
int32_t re15_finale_wurf_vy(int32_t y0);
/* Landestelle neben dem Spieler (Kopf "Landestelle"): ab Spieler (px,pz) in Richtung (jx,jz), Spieler im Aufnahme-Rechteck, hoechstens die
 * halbe Strecke. */
void re15_finale_wurf_landestelle(int32_t px, int32_t pz, int32_t jx, int32_t jz, int32_t *lx, int32_t *lz);
/* Flugkamera (Kopf "Flugkamera"): Kamera, die Flug und Landung zeigt - zuerst der Wurf-Schnitt 13 (RE2, kein Schnitt im Flug), dann
 * die Kamera des Spielers `cut_spieler`, dann die Lauf-Kameras von Wagen C. Landestelle (lx, 0, lz), Spieler (px, py, pz); braucht
 * g_room_rdt. *sicht = 2: Landestelle und Spieler im Bild, 1: nur die Landestelle, 0: keine (Rueckgabe dann cut_spieler). */
int  re15_finale_wurf_flugkamera(int cut_spieler, int32_t lx, int32_t lz, int32_t px, int32_t py, int32_t pz, int *sicht);
/* Luecken im Inventar schliessen (FUN_8004dadc so oft, bis keine Luecke vor einem belegten Platz liegt). Rueckgabe = Schuebe. */
int  re15_finale_wurf_luecken_schliessen(void);

#ifdef __cplusplus
}
#endif

#endif /* RE15_FINALE_WURF_R42_H */
