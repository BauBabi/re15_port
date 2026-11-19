/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 1: Finale im hinteren Zugwagen ROOM5090 (Leon) nach dem RE2-Ende.
 *
 * NUTZERWUNSCH 07.11.2026 (woertlich): "Für Die Version 31 möchte ich dann, das du die Form Birkin G5 aus dem Finale wieder
 * entfernst. Die passt nicht... Stattdessen, machen wir ein finale wie beim Golem am Ende von Resident Evil 2. Das jemand Leon den
 * Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die Rakete angeflogen kommen und den
 * Gegner zersprengt."
 * NUTZERWUNSCH 17.11.2026 (woertlich): "Beim erschießen des endgegners mit dem raketenwerfer möchte ich das du das game over voiceover
 * von Leon übernimmst und das voiceover von Claire."
 *
 * Bauplan: Strang-Scratchpad FINALE_PLAN.md (S0-S4, S6-S8, S10), Dossiers A (RE2-Finale), B (Schlusszeile), C (Port heute).
 * Vorbild ist der RE2-Super-Tyrant (Typ 0x2B), Overlay build/extracted/re2_ems/CDEMD0_EM2B_ai1.BIN gegen 0x80100000 gelinkt
 * (Datei-Offset = Adresse - 0x80100000), und das Raumskript info/re2leon/PL0/RDT/ROOM7020.RDT (Datei-Offsets @0x0xxxx).
 *
 * Teile dieser Stufe:
 *   Wurzel (S4)    HP 20200, Merker "genug Schaden", Untergrenze 10000, "passend" (Gehzustand, Abstand >= 2801), Uhr 1800 Bilder,
 *                  Wurf-Ausloeser wie ROOM7020 sub11 (die Wurfszene selbst folgt in Stufe 3; hier nur Merker + Messzeile).
 *   Abschuss (S6)  Ausloeser wie die Tyrant-Wurzel @0x80101904-0x80101a00, Zustand 3 Phasen 0..9 @0x801035c0-0x80103e68, das
 *                  Raumprogramm von sub15 (@0x01A22-0x01A70) als Port-Programm aus Original-Opcodes, Schlusszeile "Game over."
 *                  als Message_on (K-11, VOICE-Option gilt).
 *   Ende (S7/S8)   (5,0x1C) + Bank-9-Bit 129 im Kill-Bild (FUN_80018fb0 @0x80103d7c) -> 5090 sub01 -> statt sub02 das Port-Programm
 *                  der Stufe 5 (ohne "THANK YOU FOR PLAYING", Abblenden + Endvideo-Zeile wie RE2 ROOM4090, include/re15_endvideo_r42.h)
 *                  -> ROOM6040 Diashow -> Set(0,1,0) @0x00590 -> Titel.
 *
 * Messschiene RE15_FINALE_LOG=1 -> finale.log, Zeilen "[finale] T<takt> F<bild> <ereignis>" (T = Spieltakt dieses Moduls).
 */
#ifndef RE15_FINALE_R42_H
#define RE15_FINALE_R42_H

#include <stdint.h>
#include "re15_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Wurzel (S4) ------------------------------------------------------------------------------------------------------
 * HP: `lw v1,-0x48c(v1)` = 0x800cfb74 @0x80101b78-7c; Bit 0x40 gesetzt (`andi v0,v1,0x40 / beqz` @0x80101b84-88; RE2-Spielstart
 * setzt es bedingungslos @0x8002b4ac, Beleg re15_gator_re2_r41.h), Bit 0x20 = EASY/ROOKIE aus (`andi v0,v1,0x20 / beqz`
 * @0x80101b8c-90; der Port faehrt Normal, wie der G4-Zweig 1000 @0x80100678 in re15_birkin_formen.h) -> `lhu v0,0x4c96(v0)`
 * @0x80101bac = 200 (Datei @0x4C96 `c8 00`), `addiu v0,v0,0x4e20` @0x80101bb4, `sh v0,0x156(s0)` @0x80101be0. */
#define RE15_FI_HP_START   (20000 + 200)
#define RE15_FI_HP_GENUG   20000   /* `slti v0,v0,0x4e20` @0x80101ab8 -> +0x1D8 = 1 (`sh v0,0x1d8(s0)` @0x80101ac4)            */
#define RE15_FI_HP_UNTEN   10000   /* `slti v0,v0,0x2710` @0x80101ad0 -> `addiu v0,zero,0x2710 / sh v0,0x156(s0)` @0x80101ad8-dc */
#define RE15_FI_PASSEND_AB  2801   /* Gehzustand @0x80101d74: `lw v0,0x1f0(s0) / sltiu v0,v0,0xaf1` @0x80101d98-a0, sonst
                                    * `sh 1,0x1da(s0)` @0x80101db0; geloescht je Bild `sh zero,0x1da(s0)` @0x80101a10        */
#define RE15_FI_UHR         1800   /* ROOM7020 sub10 @0x017D4 `09 0a 08 07` = Sleep 0x0708, danach Set(5,0x0F,1) @0x017D8      */

/* ---- Abschuss (S6) ----------------------------------------------------------------------------------------------------- */
#define RE15_FI_BLICK        300   /* `slti v0,v0,0x12c` @0x801019dc: |Peilung - Spielergier| < 300 */
#define RE15_FI_SCHLAF_ZEILE  15   /* ROOM7020 sub15 @0x01A30 `09 0a 0f 00` */
/* Lesefenster der Zeile: Leon 12*16 - 0x1E = 162 Sektoren (STAGE7.BIN @0x150 `01 00 6b 00 0c 00`; Ende @0x80012ae8-0x80012b10),
 * 150 Sektoren/s (Modus 0xC8 @0x8009a415) = 1,08 s = 32,4 Bilder; Wsleeping prueft je Bild (@0x80053ac0-0x80053ad0) -> 33. */
#define RE15_FI_ZEILE_BILDER  33
#define RE15_FI_SCHLAF_W1A    32   /* sub15 @0x01A60 `09 0a 20 00` */
#define RE15_FI_SCHLAF_W1B     5   /* sub15 @0x01A64 `09 0a 05 00` */
#define RE15_FI_RAKETE_WEG     8   /* Phase 1: +0x16A == 8 -> FUN_8001cefc(2,0x0D,0) (`addiu v0,zero,8 / bne` @0x80103754-58) */
#define RE15_FI_W1_ABSTAND 10000   /* FUN_801016cc(e, 0x2710) @0x80103810-14 */
#define RE15_FI_W2_ABSTAND  5000   /* @0x80103914-18 `addiu a1,zero,0x1388` */
#define RE15_FI_W3_ABSTAND  3000   /* @0x80103a18-1c `addiu a1,zero,0xbb8` */
#define RE15_FI_W1_DAUER      40   /* `addiu v1,zero,0x28` @0x801038a4 */
#define RE15_FI_W1_HALT       15   /* `addiu v0,zero,0xf` @0x80103848 -> Bank 2 |= 0x10000000 (2,3) @0x80103888-9c */
#define RE15_FI_W2_DAUER      32   /* `addiu v1,zero,0x20` @0x801039a8 */
#define RE15_FI_W2_HALT        7   /* `addiu v0,zero,7` @0x8010394c */
#define RE15_FI_W3_DAUER      10   /* `addiu v1,zero,0xa` @0x80103a80 */
#define RE15_FI_NACHKNALL     14   /* Phase 8: `slti v0,v0,0xe` @0x80103c8c (alter Zaehler 0..13) */
#define RE15_FI_BROCKEN_BILD  16   /* Phase 8: `addiu v0,zero,0x10 / bne` @0x80103d24-28 */
#define RE15_FI_VSYNC_KNALL    5   /* `addiu v1,zero,5 / sb v1,-998(at)` @0x80103b00-08 */
/* Todesschrei Xa 0x03 direkt aus dem Overlay (`jal 0x800129b4` a0 = 0, a1 = 3 @0x80103bec-f4): STAGE7.BIN @0x156 `05 00 00 00 42 00`
 * = Kanal 5, Block 0, Laenge 66 -> 66*16 - 0x1E = 1026 Sektoren. Datei tools/r42_finale/re2_xa_finale.py.
 * Ende = ECHTZEIT (Pruefbericht s1 r1 R1): Phase 7 setzt VSync 5 (@0x80103b00-08), startet die XA und setzt 0x800cfbd8 |= 0x20
 * (@0x80103bf8-c08); Phase 8 prueft das Bit je Bild (`andi v0,v0,0x20 / bnez` @0x80103d64-74) und toetet erst, wenn es weg ist
 * (`jal 0x80018fb0` @0x80103d7c). Das Bit loescht der CD-Lesezweig nach dem Lesefenster (Ende @0x80012e2c-0x80012e4c, Bit weg
 * @0x80012f7c) mit 150 Sektoren/s (Modus 0xC8 @0x8009a415) - unabhaengig vom Bildtakt. Der Port zaehlt darum die Vertikal-
 * Austastungen (60/s) der vergangenen Bilder (je Bild + VSync-Wert DAT_800dfc1a, re2_buehne) und toetet im ersten Bild mit
 * vbl * 150 >= Sektoren * 60. Leon: 1026 * 60 / 150 = 410,4 vbl -> bei VSync 5 das 83. Bild nach dem Einschlag (6,9 s statt 17,1 s). */
#define RE15_FI_XA03_DATEI   "R702_XA03_LEON.XAS"
#define RE15_FI_XA03_LESE    1026
#define RE15_FI_CD_SEKTOREN_S 150  /* Modus 0xC8 @0x8009a415, Lesefenster @0x80012ae8-0x80012b10 */
#define RE15_FI_VBL_S          60  /* NTSC-Austastung; VSync(n) wartet n davon (Hauptschleife @0x8002b990-98) */
/* Elza (5091) = Claire-Fassung (Dossier B 6: Claire-Overlay CDEMD1.EMS mit denselben Konstanten, Adressen -0x104): Zeile Xa 0x02 =
 * Claire-STAGE7.BIN @0x150 `06 00 4e 00 18 00` -> 24*16 - 0x1E = 354 Sektoren = 2,36 s = 70,8 -> 71 Bilder; Xa 0x03 = @0x156 Kanal 5
 * Block 0 Laenge 79 -> 79*16 - 0x1E = 1234 Sektoren (Echtzeit 8,23 s, bei VSync 5 das 99. Bild). Die Datei ist aus der Claire-PAL-Disc
 * des Nutzers geschnitten (Dossier B 1.3: info/re2clair VOICE11.XAS ist Nullbytes): Stufe 4, tools/r42_finale/re2_xa_finale.py --claire,
 * VOICE17.XAS LBA 96572 Kanal 5 Sektoren 5..1237 (78), md5 df88fe577d84dce0671668c4f96808bc (unit_r42_finale_assets). */
#define RE15_FI_ZEILE_BILDER_ELZA 71
#define RE15_FI_XA03_DATEI_ELZA   "R702_XA03_CLAIRE.XAS"
#define RE15_FI_XA03_LESE_ELZA    1234

/* ---- U3: Fixpunkte und Schnitte in Wagen C (Messung tools/r42_finale/u3_schnitte.py = Strang-Scratchpad u3b/u3_suche4.py) -------
 * RE2 stellt im Ausloesebild beide auf Fixpunkte (Tyrant (-14547,-14636) @0x801035cc-e0, Leon (-19025,-20861) @0x80103638-74;
 * Abstand 7668) und schneidet auf 11 ("Game over"), in den Wiederholungen auf 8/9/10 (`jal 0x8002c7ac` a0 = 11 @0x801036d8,
 * 8 @0x801037dc, 9 @0x801038fc, 0xa @0x80103a00). Diese Schnitte zeigen in ROOM702.BSS den Bahnsteig (Plan D6). PORT-ZUORDNUNG nach
 * Rolle: je RE2-Schnitt Bild-x und Groesse von Form und Schuetze (bzw. Spieler) aus den RID-Bytes (ROOM7020 @0x64 + 32*Schnitt,
 * ROOM5090/5091 @0x60 + 32*Schnitt), Suche ueber alle Lagen der Form in Wagen C und alle Blickachsen.
 * Runde 3 (Pruefbericht s1 r2 R1: W3 lag auf dem "Game over"-Schnitt 15 HINTER der Form, verdeckt; "Game over" zeigte die Form
 * bildfuellend statt ausser Bild): die Form ist jetzt das Modell BAHNSTEIG_FORM.EMD in der Sequenzpose (Clip 2 Bild 0, form_halten)
 * als Tiefenpuffer je Schnitt. Harte Regeln: "Game over" Form ausser Bild (RE2 Schnitt 11: +86,5 Grad) und Spieler sichtbar; W1-W3
 * Form im Bild und die Bahn der Wiederholungsrakete zu >= 60 % sichtbar (nicht hinter der Form); W3 Quelle vor der Form (RE2 1181 vor
 * 4112); jede Rolle ein eigener Schnitt gegenueber der vorigen und W3 != "Game over"; die Form steht ganz im Wagen (SCA 5090).
 * Ergebnis: mit den fuenf Wagen-C-Schnitten 11-15 gibt es KEINE Lage mit drei verschiedenen W-Kameras (u3b/diag5.py) - W1 und W3
 * teilen sich Schnitt 13 (nicht aufeinanderfolgend, die Rakete startet in W3 7000 naeher an der Form). Beste Loesung (Kosten 9,28,
 * Gleichstand-Regel: innerhalb 0,1 der kleinsten Kosten die groesste Summe der Sichtbarkeit):
 *   Form (22000,-23250) am Vorraum-Ende, Gier 2016 (Achse 130/256), Spieler 7668 davor auf der Achse (FUN_801016cc-Regel).
 *   W1 Schnitt 13 (RE2 8), W2 Schnitt 14 (RE2 9), W3 Schnitt 13 (RE2 10).
 * Runde 4 (Pruefbericht s1 r3 R1: Runde 3 nahm "Game over" = Schnitt 12, eine LAUF-Kamera in der Mitte von Wagen C; drueckte man dort
 * ab, gab es keinen Schnitt und Leon sprang im selben Bild 6191 nach vorn): "Game over" ist wie RE2 Schnitt 11 KEINE Lauf-Kamera.
 *   Beleg RVD (Kopf +0x28, je Kamera erster Eintrag = Anker FUN_80014324, die uebrigen = echte Wechsel): RE2 ROOM7020 wechselt nur nach
 *   1..3 (`01ff0001` @0x00280, `01ff0102` @0x002BC, `01ff0203` @0x002F8, `01ff0302` @0x00320); ROOM5090 Wagen C: 11->12 @0x00490,
 *   12->11 @0x004B8, 12->13 @0x004CC, 13->12 @0x004F4, 13->14 @0x00508, 14->13 @0x00530 - nur Schnitt 15 hat bloss seinen Anker
 *   `01ff0f00` @0x00544 (SCD-Kamera des Auftritts, Cut_chg(15)). Damit ist der Ausloeser in jeder Lauf-Kamera 11-14 ein Wechsel, und
 *   das Bild mit dem Versetzen wird nicht gezeigt: RE2 zeichnet das Bild der Schnitt-Anforderung nicht (FUN_8002b968 Dirty @0x8002ba0c,
 *   `jal 0x8002bdf4` @0x8002ba50, `j 0x8002bd60` @0x8002bb58 ueber die DrawOTag @0x8002bd40-58; RE1.5 @0x80021558/@0x80021560 = Port
 *   kamerahalt_r41.c).
 *   Neu gerechnet (u3_schnitte.py --gemeinsam, Strang-Scratchpad u3c/ergebnis.txt): an EINER Lage fuer alle vier Rollen gibt es nur
 *   noch 3 Loesungen, alle mit der W-Lage unten und "Game over" 15 - dort steht Leon aber am rechten Bildrand (Bild-x 315-319, 34
 *   Bildpunkte hoch, Groesse 0,015 gegen RE2 0,089) und die Form ist zu 74 % im Bild. Ein "Game over" auf 15 mit Leon gross und der
 *   Form ausser Bild gibt es nur mit der Form um x 9250 - dort ist keine Wiederholung moeglich (u3c/diag_go15_alle.txt).
 *   PORT-WAHL (Runde 4): "Game over" bekommt eine EIGENE Lage (RE15_FI_GO_*), die Wiederholungen behalten ihre (RE15_FI_FORM_*).
 *   RE2 stellt nur eine Lage (@0x801035cc-e0); der Port setzt die Form erst beim Schnitt auf W1 um (Wiederholung = Schnitt, Bild 0
 *   nicht gezeigt wie oben). Die echte Rakete des "Game over"-Bilds ist 7 Bilder nach der Freigabe geloescht (@0x80103750-68), sie
 *   erreicht die Form in keiner der beiden Lagen. Ohne "Game over" ergibt die Suche fuer die Wiederholungen dieselbe Lage wie Runde 3
 *   (Schritt 1: 21 Loesungen, beste (22000,-23250) Achse 130, 13/14/13); drei verschiedene W-Kameras gibt es auch so nicht
 *   (u3c/diag_w_ohne_go.txt: 0 Lagen).
 *   Schritt 2 (Spieler >= 80 % sichtbar, Formmitte ausser Bild, Kosten k1 + 6 x Bedeckung): Form (9250,-25750) Gier 2288 (Achse
 *   113/256), Spieler 7668 davor bei (2095,-22990): u +0,15, Groesse 0,085 (RE2 Schnitt 11: +0,18 / 0,089), Formmitte Bild-x 394,
 *   Bedeckung 0,07 (Teile der Form am rechten Rand).
 * 5091 hat dieselben RID-Daten (Schnitte 0-15 bytegleich, @0x60-0x25F) und dieselbe RVD fuer Wagen C (unit_r42_finale t_schnitte). */
#define RE15_FI_FORM_X     22000
#define RE15_FI_FORM_Z    (-23250)
#define RE15_FI_FORM_GIER   2016
#define RE15_FI_T_ABSTAND   7668   /* |(-14547,-14636) - (-19025,-20861)| (@0x801035cc-e0 / @0x80103638-74) */
#define RE15_FI_GO_FORM_X     9250   /* PORT-WAHL Runde 4: eigene "Game over"-Lage (u3_schnitte.py Schritt 2, Kopf oben) */
#define RE15_FI_GO_FORM_Z  (-25750)
#define RE15_FI_GO_FORM_GIER  2288
#define RE15_FI_CUT_ZEILE     15   /* Runde 4: keine Lauf-Kamera (RVD Wagen C nur Anker `01ff0f00` @0x00544), RE2 11 @0x801036d8 */
#define RE15_FI_CUT_W1        13
#define RE15_FI_CUT_W2        14
#define RE15_FI_CUT_W3        13
/* Quelle der Wiederholungsrakete: Teil 11 des Spielers (`addiu a2,s0,0x7ac` = 11*0xAC + 0x48 @0x80103858/@0x8010395c/@0x80103a5c)
 * + Versatz (600, 0, 400) (`addiu v0,zero,0x258 / sh 0x10(sp)`, `addiu v0,zero,0x190 / sh 0x14(sp)`, `sh zero,0x12(sp)`
 * @0x80103828-38 / @0x80103a30-40); a1 = 0x80/0x81/0x82 (@0x80103854/@0x80103958/@0x80103a50) -> +0x22 = a1 >> 16 = 0 (@0x8001c988-8c).
 * Port-Teil 11 = Waffenknochen (re15_player_gunbone_matrix, s_hand_rot = Knochen 11, jedes Bild auch unsichtbar gestellt). */
#define RE15_FI_QUELLE_X     600
#define RE15_FI_QUELLE_Z     400

/* Port-Signale Programm -> Takt (Bank 5 Wort 0, Raum-Scratch wie RE15_AN_SIG_*): Zensus 5090/5091 (scd_dump_room.py) belegt nur
 * (5,0x1C) und (5,32); Annette 0x10..0x13. Gegenstueck RE2: Member_set(0x18,2) @0x01A5C bzw. (0x18,3) @0x01A6C = Gegner +0x1D6.
 * Runde 2 (Pruefbericht s1 r1): nicht 0x14 - das Bit setzt RE2 sub15 selbst (Set(5,0x14,1) @0x01A28, Schuettel-Schleife sub09 aus) und
 * das Programm fuehrt den Befehl jetzt mit. */
#define RE15_FI_SIG_FREI   0x16u
#define RE15_FI_SIG_W1     0x17u
#define RE15_FI_SCHUETTEL_AUS 0x14u   /* sub15 @0x01A28 `22 05 14 01`, gelesen von sub09 @0x01750/@0x017B4 */

/* Schlusszeile (K-11): Message_on-Id nach VERTRAG 2.1 (Zensus tools/r42_story/zensus_r42.py, Zeile ROOM5090 Id 2). */
#define RE15_FI_MSG_ZEILE_LEON  2
#define RE15_FI_MSG_ZEILE_ELZA  2   /* ROOM5091 Id 2 (Zensus wie 5090: RDT 0..1) */

/* ---- U5: Zerplatzen (Phase 7 @0x80103bfc-0x80103c50, Phase 8 FUN_8010024c je Bild, Bild 16 @0x80103d48-60) ---------------------
 * RE2: FUN_80100108 gibt den 15 Teilen der Tabelle @0x8010476c (20 Byte je Eintrag: Teil, Gier, v, a, vy, Zaehler, Dreh, Dreh-a,
 * Seite, Marke) Bit 0x40 und Zufallswerte; FUN_8010158c schickt die Teile 0, 3, 5, 7, 13, 0x14 als Brocken (Bit 0x4A, vy -10, Gier +
 * 0x800, Farbe 0x202038, @0x8010158c-f0); Bild 16 loescht die Brocken (`sw zero` @0x80103d48-60) und FUN_801005ac setzt an den Teilen
 * mit Marke den Effekt 0x040E2000 (@0x801005ac-640). Die Teilnummern sind die des Tyrant-Gerippes (21 Teile, CDEMD0 EM2B); die Riesenform
 * hat 32. PORT-ZUORDNUNG nach Rolle (Messung Strang-Scratchpad u5/u5_zuordnung.md, Tabelle in finale_r42.c k_flug/k_brocken). */
#define RE15_FI_TEILE_FLUG     15   /* `sltiu v0,s3,0xf` @0x8010021c / `sltiu v0,s4,0xf` @0x80100578 */
#define RE15_FI_TEILE_BROCKEN   6   /* sechs `jal 0x8010158c` @0x80103c0c-50 (RE2-Teile; in der Form 15 Teile, s. k_brocken) */

/* ---- S1 Raumtonbank R702 (shared_assets/RE2/R702.EDT/.VH/.VB = ROOM7020 snd0, RDT-Kopf +0x08/+0x0C/+0x10 = @0x05C90/@0x05D50/
 * @0x06B70, tools/r42_finale/re2_r702_esp.py). Einschlag: Se_on 0x020F0001 = Raumbank 2 Satz 0x0F (`lui a0,0x20f / ori a0,a0,1 /
 * jal 0x8005ba28` @0x80103b54-5c; EDT @0x05CCC `00 00 c3 20`), a1 = Lage der Form (+0x38). Landung des Werfers Satz 0x0E (@0x05CC8,
 * Stufe 3). PORT-WAHL: wie die uebrigen RE2-Raumbaenke im Port nicht positional (re15_audio_se_play_bank). */
#define RE15_FI_SE_EINSCHLAG  0x0F
#define RE15_FI_SE_LANDUNG    0x0E
#define RE15_FI_TONBANK       "R702"
/* PC-Seite (platform/pc/src/finale_r42_pc.c), gesetzt in platform/pc/main.c. */
void re15_finale_r42_se_hook(void (*fn)(int satz));

/* Phasen (RE2 Zustand 3 +0x06; 0 = aus, 1.. = Phase + 1) */
enum {
    RE15_FI_AUS = 0, RE15_FI_P1 = 2, RE15_FI_P2 = 3, RE15_FI_P4 = 5, RE15_FI_P6 = 7, RE15_FI_P7 = 8, RE15_FI_P8 = 9, RE15_FI_P9 = 10,
    RE15_FI_ENDE = 11
};

/* Raum: 5090/5091 mit der Bahnsteig-Form (re15_bf_bahnsteig_aktiv). */
int  re15_finale_r42_raum(void);
/* Raumaufbau (scd_room_setup.c): Texte einsetzen, Zustand bei Raumwechsel loeschen (Selbst-Tueren behalten ihn). */
void re15_finale_r42_install(uint16_t room_id);
void re15_finale_r42_reset(void);

/* Wurzel: Start-HP beim Eintritt der Form (birkin_bahnsteig.c) und der Takt nach dem Zustand (RE2 @0x80101a10-0x80101adc). */
int  re15_finale_r42_start_hp(int bisher);
void re15_finale_r42_form_takt(int slot, int gehen, int32_t dist);

/* Am Anfang von re15_game_step: Ausloeser und Sequenz (RE2: Gegnerschleife vor dem Spieler, @0x80026570-0x80026620).
 * Rueckgabe 1 = pad/pressed wurden ersetzt. */
int  re15_finale_r42_vor_spielzug(uint16_t *pad, uint16_t *pressed);
/* Die Form gehoert waehrend der Sequenz dem Finale (KI schweigt, wie re15_annette_r41_besitzt). */
int  re15_finale_r42_besitzt(int slot);
/* Gate 2 des RE2-Geschoss-Appliers (@0x80047138-40) fuer die Riesenform: +0x1D3 Bit 0x80 bei gefuehrtem Raketenwerfer
 * (@0x801018d4-0x80101900) und ab Phase 0 (@0x8010361c-24). */
int  re15_finale_r42_kein_ziel(const re15_actor_t *e);

/* Abspann-Freigabe (S7): 1, sobald das Finale beendet ist (Bank 9 Bit RE15_R42_BIT_FINALE_ENDE). */
int  re15_finale_r42_ende(void);

/* Pruef- und Messzugriffe */
int  re15_finale_r42_phase(void);
int  re15_finale_r42_genug(void);
int  re15_finale_r42_passend(void);
int  re15_finale_r42_uhr(void);              /* verbleibende Bilder, -1 = noch nicht gestartet */
int  re15_finale_r42_wurf_faellig(void);
const uint8_t *re15_finale_r42_programm(int elza, int *out_len);
void re15_finale_r42_kampf_start_test(void);  /* nur Riegel: Uhr starten wie das Ende von sub04 */
/* nur Riegel (Runde 4, Pruefbericht s1 r3 R1): Fixpunkte + "Game over"-Schnitt der Phase 0 (@0x801035cc-0x801036dc) mit der Form in
 * form_slot; Rueckgabe = Schnitt vor dem Ausloesen (work_vars[0x0A]). */
int  re15_finale_r42_schnitt_test(int form_slot);
/* R1: 1, sobald das Xa-0x03-Lesefenster (Sektoren) nach vbl Austastungen abgelaufen ist (vbl * 150 >= Sektoren * 60). */
int  re15_finale_r42_xa_aus(uint32_t vbl, int sektoren);
/* U5: Eintrag i der Zuordnung (0..14 Flugteile, ab 15 Brocken): RE2-Teil, Form-Teil, Marke. 0 = kein Eintrag. */
int  re15_finale_r42_teil(int i, int *re2, int *form, int *marke);

#ifdef __cplusplus
}
#endif

#endif /* RE15_FINALE_R42_H */
