/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE3a: Marvin. Code: engine/src/story3a_r42.c, Texte:
 * tools/r42_story/texte_stage3a.py -> engine/src/gen/story3a_r42_texte.inc.
 *
 * PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, vom
 * Nutzer am 08.11. freigegeben): Abschnitt 5 T-01/T-02 (Figurwechsel nach RE2), 3.2 L4-L7, Zeitleiste L-05..L-10, Entscheide
 * F-04 (Leon kaempft gegen die Hunde, Marvin kommt danach aus der Tuer), F-05 (Keycard UND Fabrikplan), F-06 (alle Funde, die
 * eigene Pistole behaelt Marvin). Bits und Nachrichten-Ids NUR aus dem VERTRAG analysis/befunde_runde42/VERTRAG.md
 * (re15_story_r42.h): Bit 123 (Schuesse gehoert), 124 (Uebergabe), 35 (Fabrikplan), 139 (Marvin traegt den Fabrikplan, Strang
 * fabrikplan K-16: 17/18 nur damit, Kopf re15_kartenitems_r36.h STRANG FABRIKPLAN); ROOM3020 msg 0..2 (Neutext), 7..9;
 * ROOM3060 msg 11..27. NUR Leons Fassung (Raum exakt 3020/3060/3010, Figur Leon); Elzas 3021/3061/3011 bleiben unveraendert.
 *
 * ---- T-01/T-02: FIGURWECHSEL NACH RE2 (Vorbild FUN_80068f9c, ghidra_re2_Leon.txt:202702ff, Aufruf @0x80049fd0 aus
 *      FUN_80049e48 mit a0 = neue Figur DAT_800d482c) ---------------------------------------------------------------------
 *   Eintritt (a0 - 0xe < 2, `addiu v0,s2,-0xe` @0x80068fc8 / `sltiu v0,v0,0x2` @0x80068fcc):
 *     Ausruest-Platz d5bf8 -> ce5e2 (`sb` @0x80068ff4), Platzzahl d46ac -> ce5e3 (@0x80068ffc), Merkbit 0x7f in d4854
 *     (`jal 0x8007730c` @0x80069000), 11 Plaetze (`sltiu v0,s0,0xb` @0x800690ec) Inventar d4a3c -> Sicherung ce5e4
 *     (@0x80069088..@0x8006909c) und die Seite der Figur aus der Tabelle 0x800a93d0 (@0x800690a8..@0x800690d8);
 *     Ada (0xe): Waffe d5bfa := 3 (@0x8006901c), Ausruest-Platz := 0 (@0x80069024). Ada-Seite @0x800a93d0 = `03 0d 00 / 14 2d 00
 *     / 23 01 00` (Pistole mit 13 Schuss, Munition, Spray).
 *   Rueckweg (jede andere Figur): nur bei gesetztem Merkbit (`jal 0x80077360` @0x80069108, `beq v0,zero` @0x80069110):
 *     Plaetze zurueck (@0x80069118..@0x80069150), Platzzahl (@0x80069168), Ausruest-Platz (@0x8006916c), Waffe := Gegenstand
 *     des Ausruest-Platzes (@0x80069180..@0x800691a0, 0x80 -> 0 @0x800691a4), Merkbit loeschen (`jal 0x80077334` @0x800691e8).
 *     Die Funde der Nebenfigur verfallen dabei (ihre Seite wird ueberschrieben) - das Konzept L6 macht daraus die Uebergabe.
 *   Port (RE1.5 hat kein Tauschsystem, Konzept B33): Leons 11 Plaetze (DAT_800b10ac, re15_inventory.h), sein Ausruest-Platz
 *   (DAT_800b25c8) und seine Waffe (DAT_800aca5d) werden gesichert; Marvin bekommt eine eigene Seite. Abweichungen:
 *     - 11 statt 10 Plaetze zurueck: RE2 erneuert Platz 10 (Feuerzeug/Dietrich 0x2f/0x30 @0x800691a8..@0x800691dc); RE1.5 hat
 *       keinen Sonderplatz.
 *     - Die Waffe kommt aus der Sicherung: im Port ist das Messer kein Gegenstand (Runde 35 Spur E, messer_rueckfall.c,
 *       Ausruest-Platz 0x80 -> Waffe 1 nach der Commit-Regel @0x80046654-88); RE2s Regel "0x80 -> 0" wuerde es verlieren.
 *   Leben und Gift (NB1 B8) wie RE2s AUFRUFER FUN_80049e48: Eintritt `lhu v0,0x3b66(s0)` (Leben +0x156) @0x8004a000, Gift
 *     @0x8004a004/08 -> d46be/d46b3/d46b4 @0x8004a020/2c/38, Nebenfigur := Hoechstleben @0x8004a00c/@0x8004a040; Rueckweg (alte Figur
 *     > 0xb, `sltiu v0,s1,0xc` @0x8004a044) zurueck @0x8004a058-@0x8004a07c. Port: HP +0x9a und Gift-Bit 0x2 von +0x98 (0x800acaec)
 *     VOR dem RE1.5-Schwanz gesichert (der setzt HP 100 @0x80031710 und Status 0 @0x80031720), beim Rueckwechsel zurueck; dazu der
 *     Vor-Platz des Ports (re15_inv_prev_equip_slot).
 *     NB2 N5 (selbst gelesen): cfe10 ist das GIFT-WORT (Op 32 `sh v0(0x1518),-0x1f0(at)` @0x80020130; der Takt fragt nur `lhu`/`beq zero`
 *     @0x8003c130/38), cfe12 der TAKT-ZAEHLER (`lbu` @0x8003c144, -1 @0x8003c14c/54, := 30 @0x8003c15c/64; Op 32 := 30 @0x80020138/40).
 *     Gegenstueck des Gift-Worts = RE1.5-Bit 0x2 (Gift `ori v0,v0,0x2` @0x80012eac, Heilung @0x8004af8c; zustand_r39.c Kopf), des
 *     Zaehlers = zustand_r39.c s_takt (RE2-Takt im Port). Beide werden gesichert und zurueckgelegt; RE1.5s eigener Zaehler 0x800acc27
 *     (+0x1d3, @0x80031e5c) wird ohnehin bei jedem Raumwechsel genullt (Kommando 0 @0x8003196c) und braucht keine Sicherung.
 *   Marvins Seite (PORT-STORY, Konzept T-01 "Browning HP mit 15 Schuss sowie 50 H. Gun Bullets, mit ausgeruesteter Pistole"):
 *     dieselben Mengen wie Leons Start - Starttabelle PSX.EXE @0x80074bb8 `01 03 15` / Mengen @0x80074bd0 `00 0f 32` (Lesestellen
 *     @0x80045e44 / @0x80045e98), ohne das Messer (Runde 35 Spur E); ausgeruestet Platz 0 wie Ada (@0x80069024).
 *   Die Sicherung landet im Spielstand (Konzept T-01; re15_savedata.c v10 story_r42[]). RE2s Lade-Kopie MEM_CARD.BIN
 *   @0x801c13d0..@0x801c13e4 deckt [0x800d44a4, +0x798) - ce5e4 liegt ausserhalb; dass RE2 die Sicherung mitspeichert, ist NICHT
 *   belegt (RE2 laesst als Ada/Sherry vermutlich gar nicht speichern) - hier ist es Konzept-Vorgabe.
 *
 * ---- MESSUNGEN (Mod-Baum shared_assets/PSX, scd_dump_room.py / Rohbytes) --------------------------------------------------
 * ROOM3020 sub02 @0x0C10..@0x0D81 (Fensterszene): ... Message_on 0 @0x00CB8, 1 @0x00D12, 2 @0x00D32, Cut 6 @0x00D0C,
 *   Sleep 10 @0x00D76, Wechsel `24 10 0d 00` @0x00D7A (work_vars[0x10] = Marvin 0x0D), Aot_on 2 `47 02` @0x00D7E (Tuer @0x00B6E
 *   -> 30C0), Evt_end @0x00D80. Ausgeloest von Aot_set Slot 3 sce 3 Ereignis 2 (sub00 @0x00BE2, nur bei Ck(4,0xC7)=1 @0x00BDE).
 *   Signale: 3020-Skripte nutzen Bank 5 nur 0x00/0x01 (sub03 @0x00D82, sub04 @0x00D94).
 * ROOM3060 sub03 @0x0104A..@0x010E7 (Marvin kommt aus 3040): Pos_set (2063,0,-17192) @0x01064, Dir 1320 @0x0106C, Hunde los
 *   (Member_set 0x0C = 66 @0x0109A/@0x010A6/@0x010B2), Plc_motion(2,6) @0x010BE (W-Clip 6 = Waffe heben, wclip_r41.c), Sleep 40
 *   @0x010C2, Ifel_ck (3,0x75) @0x010C6 -> Wechsel `24 10 00 00` @0x010D2 / `24 10 01 00` @0x010DE, Aot_on 3 `47 03` @0x010E4
 *   (Tuer @0x00EF8 -> 3020 (-11250,0,-13250) Cut 6), Evt_end @0x010E6. Gestartet aus sub00 @0x0102A (Ck(4,0xC4)=1, Ck(3,0xA0)=0,
 *   Set(3,0xA0) @0x01026). Bank 5: kein Skript des Raums.
 *   Hunde: (3,0xA0)=0 beh 0x41 @0x00F78/@0x00F8C/@0x00FA0, sonst beh 0x00 @0x00FB8/@0x00FCC/@0x00FE0; Kill-Ids pc[7] 0x50/0x51/0x52
 *   (Em-Status-Tor FUN_800420a0 @0x80042120-38, Zone 7 fuer STAGE1..3 = re15_em_status_zone).
 *   Orange Tuer = main00 Slot 2 @0x00ED6 (Rechteck x 1500..4700 z -17200..-15900, nach 3040 nur bei Ck(4,0xC4)=1 @0x00EBA).
 *   Kamera 4 (RID @0x0E0): Auge (-3114,-2178,-21906) -> Ziel (9990,-1512,-15462) - blickt auf die orange Tuer (dieselbe
 *   Einstellung, mit der sub03 Marvin dort zeigt, Cut_chg 4 @0x0105E).
 * ROOM3010 Kartentuer Slot 6 (L-10 "oeffnet erst nach der Uebergabe" folgt aus T-01 + der Kistenregel des Ports, kistenregel_r37.c
 *   Fall K7: Ck(3,0x3B)==0 @0x020AE gilt, solange die Green Keycard nicht in Leons INVENTAR liegt; die Uebergabe setzt (3,0x3B) wie
 *   ROOM3040 sub01 @0x014DA, das nur beim Betreten von 3040 laeuft): Ck(3,0x3B)=0 @0x020AE -> Aot_set msg 0 @0x020B2 `2c 06 01 31 01 00 c0 95 70 ae 84 03 48 08 00 00 ff
 *   ff 00 00` ("It's locked. An ID card is required to open it."); Ck(3,0x3C)=0 @0x020CE -> sub02 @0x020D2 (Set(3,0x3C) @0x02302,
 *   msg 1 "You've used the Green Keycard."); sonst Tuer @0x020EA -> 30E0. (3,0x3B) setzt ROOM3040 sub01 @0x014DA, sobald die
 *   Karte genommen ist (Ck(9,0x55) @0x014CE) - also schon, wenn MARVIN sie aufhebt.
 * Funde auf Marvins Weg (belege/marvin30c0): Green Keycard 0x36 x1 Bit 85 ROOM3040 @0x014B2, Super Redhawk 0x07 x6 Bit 92 ROOM3050
 *   @0x01EC6; ROOM30E0 Green Medicine 0x24 @0x00AFA und Magnum Bullets 0x17 @0x00B10; ROOM30A0 Red Master Keycard 0x46 @0x00742.
 *
 * ROOM3050 Rechner K8 (Red Master Keycard; NB2 N2): main00 Ck(3,0xB2)=0 @0x01F90 -> Slot 10 sce 1 msg 3; sonst Ck(3,0xB3)=0 @0x01FB0 ->
 *   Aot_set Slot 10 sce 3 Ereignis 10 @0x01FB4 (Rechteck -25900,-16900,1000,2000, Band 0). (3,0xB2) setzt ROOM30A0 sub01 @0x0086C, sobald
 *   die Karte genommen ist (Ck(9,180)=1 @0x00860). sub10 @0x02578 (msg 3, Frage msg 4, Ck(12,31,0) @0x02588) startet das Kran-Steuerspiel
 *   (Cut 15 @0x02596, Bank 5 Bits 0..5 @0x02598..@0x025AC); sub01 @0x0219E..@0x0221C steuert obj 3 (Sce_key_ck -> sub11..14) und startet bei
 *   Treffer (Member_cmp(15==0x0b) @0x0220E, Sce_key_ck 0x40 @0x02218) sub15 @0x025E4: msg 5 "You've used the Red Master Keycard." @0x02608
 *   (Wegwerf-Frage gen/discard_sites.inc 3050/5/0x46, Ja vorbelegt), Set(3,0xB3) @0x0260E, Set(3,0xB0) @0x02612, msg 6, Slot 10 -> msg 7.
 *   ROOM3040 sub00 Ck(3,0xB0)=0 @0x0134A: erst danach schaltet der Knopf (Ereignis 3 statt 4). Die Karte ist also im Normalfall VERBRAUCHT
 *   (Konzept 6b); nur wer das Wegwerfen ablehnt, gibt sie Leon in der Uebergabe.
 *
 * ---- SCHUESSE (L5 Punkt 1 "Marvin ruft, man hoert Schuesse, Abblende") -----------------------------------------------------
 * PORT-STORY aus belegten Bausteinen: Abblende = die Raumwechsel-Abblende (Rampe +0x1800 @0x8001ca64-88, statisches Schwarz
 * 0x7fff @0x80046620-50; wie story1_r42.c RE15_S1_BLENDE_*). Schuesse = ARMS-Satz 0 der ausgeruesteten Waffe (der Knall,
 * audio_pc.c re15_audio_weapon_se / pc_bang, "ARMS record 0 (the gunshot)"), Marvin traegt die Browning (ARMS-Bank 3 nach dem
 * Figurwechsel, FUN_80043d8c @0x80031708). Drei Schuesse = drei Hunde (@0x00F78/@0x00F8C/@0x00FA0); Abstand = Feuer-Clip 7 der
 * Browning PL00W03.PLW (EDD @0x0008, Clip-Tabelle @0x0024 `17 00` = 23 Bilder; Feuern = Quadrat gehalten im Halten, der
 * Rueckstoss-Clip gibt den Takt vor, game_step_common.c "recoil clip gates the auto-refire cadence"). Schwarz-Haltezeit nach dem
 * letzten Schuss = RE15_S1_BLENDE_HALTEN (PORT-WAHL der Stufe STAGE1, 30 Bilder).
 */
#ifndef RE15_STORY3A_R42_H
#define RE15_STORY3A_R42_H

#include <stdint.h>
#include "re15_story_r42.h"
#include "re15_inventory.h"

/* ---- Raeume (exakt: nur Leons Fassung) --------------------------------------------------------------------------------- */
#define RE15_S3A_RAUM_3020       0x3020u
#define RE15_S3A_RAUM_3060       0x3060u
#define RE15_S3A_RAUM_3010       0x3010u

/* ---- Figuren (Nibble DAT_800aca5c = work_vars[0x10], Tabelle @0x80073f70) ------------------------------------------------- */
#define RE15_S3A_MODELL_MARVIN   0x0D     /* ROOM3020 @0x00D7A `24 10 0d 00`                                                */
#define RE15_S3A_LEON_MAX        3        /* Leon 0..3 (Waffen-Basis @0x800741e8 = 76 x4; Ruestungen 0/1/2)                 */

/* ---- Marvins Seite (T-01) --------------------------------------------------------------------------------------------- */
#define RE15_S3A_PISTOLE         0x03     /* @0x80074bb8 [1] = BROWNING HP                                                  */
#define RE15_S3A_PISTOLE_MENGE   15       /* @0x80074bd0 [1] = 0x0f                                                         */
#define RE15_S3A_MUNITION        0x15     /* @0x80074bb8 [2] = H. GUN BULLETS                                               */
#define RE15_S3A_MUNITION_MENGE  50       /* @0x80074bd0 [2] = 0x32                                                         */
#define RE15_S3A_LEER_PLATZ      0x80     /* "nichts ausgeruestet" (@0x80045fe0, RE2 @0x80069030)                           */

/* ---- Funde / Uebergabe (L6, F-05, F-06) --------------------------------------------------------------------------------- */
#define RE15_S3A_KEYCARD         0x36     /* Green Keycard, ROOM3040 @0x014B2 pc[14]                                        */
#define RE15_S3A_REDHAWK         0x07     /* Super Redhawk, ROOM3050 @0x01EC6 pc[14]                                        */
#define RE15_S3A_ROT             0x46     /* Red Master Keycard, ROOM30A0 @0x00742 pc[14]; benutzt am Rechner K8 ROOM3050 (s. u.) */
#define RE15_S3A_FABRIK_BLATT    7        /* Kartenblatt der Fabrik (re15_map_zones.h ROOM3000.. Spalte Blatt = 7)          */

/* ---- Bedingungen -------------------------------------------------------------------------------------------------------- */
#define RE15_S3A_A0_BANK         3
#define RE15_S3A_A0_BIT          0xA0     /* Marvins 3060-Szene gelaufen: Set @0x01026                                      */
#define RE15_S3A_KARTE_BANK      3
#define RE15_S3A_KARTE_BIT       0x3B     /* "Karte vorhanden" Ck @0x020AE (3010), Set ROOM3040 @0x014DA; die Uebergabe setzt es */
#define RE15_S3A_KARTE_BENUTZT   0x3C     /* Ck @0x020CE, Set @0x02302                                                      */
#define RE15_S3A_HUND_KILL0      0x50     /* pc[7] @0x00F78 / @0x00FB8; 0x51 @0x00F8C, 0x52 @0x00FA0                        */
#define RE15_S3A_HUNDE           3

/* ---- Bytes / Plaetze -------------------------------------------------------------------------------------------------- */
#define RE15_S3A_3020_SUB02_OFF  0x0C10
#define RE15_S3A_3020_WECHSEL    0x0D7A   /* `24 10 0d 00` - die neuen Zeilen 7/8 stehen davor                              */
#define RE15_S3A_3020_SUB02_ENDE 0x0D82
#define RE15_S3A_3020_EREIGNIS   2        /* Aot_set Slot 3 @0x00BE2 Nutzlast `ff 00 18 02`                                 */
#define RE15_S3A_3060_SUB03_OFF  0x104A
#define RE15_S3A_3060_NACH_SLEEP 0x10C6   /* nach Sleep 40 @0x010C2 - hier Zeile 11 und die Abblende                        */
#define RE15_S3A_3060_SUB03_ENDE 0x10E8
#define RE15_S3A_3060_SUB        3
#define RE15_S3A_3010_KARTE_SLOT 6        /* main00 @0x020B2 / @0x020D2 / @0x020EA; L-10 ueber kistenregel_r37.c K7            */

/* ---- Schuesse + Abblende (s. Kopf) ------------------------------------------------------------------------------------- */
#define RE15_S3A_SCHUESSE        RE15_S3A_HUNDE
#define RE15_S3A_SCHUSS_TAKT     23       /* PL00W03.PLW EDD Clip 7 = 23 Bilder (@0x0024 `17 00`)                          */
#define RE15_S3A_KNALL_SATZ      0        /* ARMS-Satz 0 = Schuss                                                           */

/* ---- Wiedersehen 3060 (PORT-WAHL der Inszenierung, gemessen mit der Kamera-4-Projektion, scratchpad story/proj3060.py) ----- */
#define RE15_S3A_CUT_TUER        4
#define RE15_S3A_TUER_X          2063     /* Marvins Ankunft aus 3040 = Pos_set @0x01064                                     */
#define RE15_S3A_TUER_Z          (-17192)
#define RE15_S3A_GIER_TUER       1320     /* Dir_set @0x0106C `33 00 00 00 28 05` (Blick aus der Tuer)                      */
/* Lagen N (Marvin) / P (Leon): quer zur Blickachse von Kamera 4, s. story3a_r42.c (gemessen, Werte im Log) */
#define RE15_S3A_NPC_X           1550
#define RE15_S3A_NPC_Z           (-18950)
#define RE15_S3A_SPIELER_X       2100
#define RE15_S3A_SPIELER_Z       (-20000)
#define RE15_S3A_NPC_SLOT        8        /* Skript-Slot wie RE15_S2_NPC_SLOT; die Hunde nutzen 0..2                        */
#define RE15_S3A_TYP_MARVIN      0x40     /* ROOM3060 sub00 @0x00F58 `44 00 40 00 ..` (beh 0x00)                            */

/* ---- Signale Bank 5 (Raum-Scratch; 3020 nutzt nur 0x00/0x01, 3060 keine) ----------------------------------------------- */
#define RE15_S3A_SIG_FREI        0x14     /* Takt -> Programm: keine Zeile, keine Stimme mehr                               */
#define RE15_S3A_SIG_ABBLENDE    0x15     /* Programm -> Takt: abblenden (3060: + Schuesse)                                 */
#define RE15_S3A_SIG_SCHWARZ     0x16     /* Takt -> Programm: schwarz (und gehalten)                                       */
#define RE15_S3A_SIG_EIN         0x17     /* Programm -> Takt: einblenden                                                   */
#define RE15_S3A_SIG_HELL        0x18     /* Takt -> Programm: Einblende fertig                                             */
#define RE15_S3A_SIG_UEBERGABE   0x19     /* Programm -> Takt: Uebergabe-Schritt (s_uebergabe_schritt)                      */
#define RE15_S3A_SIG_MARVIN_WEG  0x1A     /* Programm -> Takt: Marvin geht durch die Tuer (Aktor frei)                      */
/* ---- L-10 gegen Marvins Weg (Pruefer pruef_s3a_r1 B2) - GEMESSEN: kein Riegel noetig ----------------------------------------
 * Verdacht: 30E0 main00 Slot 0 @0x009F2 (Door_aot_set -> 3010 (-25190,-1800,-20200) Cut 5 = Rueckseite der Kartentuer 3010 Slot 6
 * @0x020EA) liegt ohne Bedingung auf Marvins Weg; mit der Karte in seiner Seite oeffnete Marvin die Kartentuer (K7 @0x020AE).
 * Messung SCA ROOM30E0 (RDT+0x20 -> @0x51C, 5 x 19 Eintraege): drei GESCHLOSSENE Wandringe.
 *   West  Eintr. 0..8  x -29328..-17460, z -28900..-19600: Slot 0 (-18900,-23500,800,1320) in der Ostwand (Eintr. 0
 *         -18800/-28900 1340x9300), Slot 1 -> 3070 (-27100,-21800,2700,1200) in der Nordwand (Eintr. 6 -28550/-21755 10960x1500)
 *   Ost   Eintr. 13..18 x 1775..29450, z 275..17300: Slot 6 -> 3050 (3375,3650,650,1950) in der Westwand (Eintr. 13), Slot 7 -> 3030
 *         (6225,1900,1800,600) in der Suedwand (Eintr. 14); Marvins Ankunft aus 3030 (3030 Slot 0 @0x001D2 -> (7025,0,2700)) liegt
 *         im OST-Ring. Wegsuche (scratchpad story/s3a/weg.py, Band 0): kein Weg von (7025,2700) in den West-Ring.
 *   Den West-Ring erreicht man nur aus 3010 (Kartentuer) und 3070 (30E0 Slot 1); nach 3010/3060 kommt Marvin nicht: 3040 -> 3060
 *   ist bis (4,0xC4) kein Durchgang (main00 Ck(4,0xC4)=0 @0x01242 -> Aot_set Slot 6 sce 3 + Door_aot_set sce 0 @0x0125A), und mit
 *   (4,0xC4) startet 3060 sub00 @0x0101A den Rueckwechsel; 3090 -> 3060 ist bis (4,8) "latched" (3090 sub00 Ck(4,8)=0 @0x0143E ->
 *   Aot_reset Slot 1 msg 3 @0x01442); 30D0 -> 3010 erst nach (3,71) (3090 @0x01326 / 3010 @0x02036). Riegel des Ports:
 *   unit_r42_story3a_weg30e0 (Marvin am Ankunftspunkt, gesteuert gegen den West-Ring) + Durchlauf (30E0 Ost auf Marvins Weg). */
#define RE15_S3A_RAUM_30E0       0x30E0u
#define RE15_S3A_30E0_OST_X0     1775     /* SCA Eintr. 13/15 x (Westwand des Ost-Rings)                                     */
#define RE15_S3A_AN_SPIELER      0x20     /* Plc_dest-Ankunftsbits (Form ROOM2040 sub02 @0x01648)                           */
#define RE15_S3A_AN_NPC          0x21

/* ---- Spielstand (re15_savedata.c v10, story_r42[]) ---------------------------------------------------------------------- */
#define RE15_S3A_SPEICHER_BYTES  96
typedef struct {
    uint8_t aktiv;                                /* 1 = Leons Seite gesichert, Marvin spielt (RE2 Merkbit 0x7f in d4854) */
    uint8_t leon_platz;                           /* Ausruest-Platz (RE2 ce5e2)                                           */
    uint8_t leon_waffe;                           /* DAT_800aca5d (Port-Abweichung, s. Kopf)                              */
    uint8_t marvin_offen;                         /* 1 = Marvins Funde noch nicht uebergeben                              */
    re15_inv_slot_t leon[RE15_INV_MAX_SLOTS];     /* RE2 ce5e4                                                           */
    re15_inv_slot_t marvin[RE15_INV_MAX_SLOTS];   /* Marvins Seite beim Rueckwechsel (Uebergabe L6)                       */
    /* NB1 B8 (am Ende: die Lage der Felder oben bleibt): wie RE2 FUN_80049e48 (Leben/Gift sichern @0x8004a000-@0x8004a038,
     * zurueck @0x8004a058-@0x8004a07c) und der Vor-Platz des Ports */
    uint8_t leon_vorplatz;                        /* re15_inv_prev_equip_slot (Port), Marvin: 0x80                         */
    /* Bit 0 = Gift-Bit 0x2 von +0x98 (0x800acaec) = RE2 Gift-Wort cfe10 (@0x8004a008 -> d46b4 @0x8004a038, zurueck @0x8004a070/7c).
     * Bits 1..5 (NB2 N5, die 96 Bytes sind voll) = RE2 Takt-Zaehler cfe12 (@0x8004a004 -> d46b3 @0x8004a02c, genullt @0x8004a010,
     * zurueck @0x8004a064/@0x8004a078) = im Port zustand_r39.c s_takt; Wertebereich 0..30 (:= 0x1e @0x8003c15c/64, Op 32 @0x80020138),
     * passt in 5 Bit. Altstaende (NB1: nur 0/1) lesen sich als Takt 0. */
    uint8_t leon_gift;
    uint8_t leon_hp_lo, leon_hp_hi;               /* +0x9a (RE2 cfd4e @0x8004a000 -> d46be @0x8004a020, zurueck @0x8004a074) */
} re15_s3a_figur_t;
#define RE15_S3A_GIFT_BIT        0x01u
#define RE15_S3A_TAKT_SHIFT      1
#define RE15_S3A_TAKT_MASKE      0x1Fu

/* ---- Haken -------------------------------------------------------------------------------------------------------------- */
int  re15_story3a_r42_figur_vor(int alt, int neu);   /* sherry_r41.c modell_setzen: 1 = Neuladen im laufenden Tausch (kein Schwanz) */
void re15_story3a_r42_figur_nach(int alt, int neu);  /* sherry_r41.c modell_setzen: Tausch (T-01/T-02)                            */
void re15_story3a_r42_install(uint16_t room_id);     /* scd_room_setup.c (Ende des Installer-Blocks): Schuesse 3020 */
void re15_story3a_r42_texte(uint16_t room_id);       /* msg_common.c re15_msg_load_room_block (NACH den RDT-Texten): 3020/3060/30E0 */
const uint8_t *re15_story3a_r42_ereignis(uint16_t room_id, uint8_t event_id);                 /* scd_vm.c scd_event_fire */
const uint8_t *re15_story3a_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc);   /* scd_vm.c op_evt_exec    */
void re15_story3a_r42_tick(void);                    /* scd_vm.c scd_vm_tick (Gameplay-Lauf)                                      */
int  re15_story3a_r42_blatt(unsigned page);          /* re15_map_owned.c: Fabrik-Blatt (Leon Bit 35; Marvin spielt + Bit 139)     */
void re15_story3a_r42_neues_spiel(void);             /* Neues Spiel: kein Tausch                                                  */
void re15_story3a_r42_export(uint8_t out[RE15_S3A_SPEICHER_BYTES]);
void re15_story3a_r42_import(const uint8_t in[RE15_S3A_SPEICHER_BYTES]);   /* nach dem Laden; setzt work_vars[0x10] bei Marvin */
int  re15_story3a_r42_marvin_spielt(void);

/* Pruefhaken / Messzeilen */
const re15_s3a_figur_t *re15_story3a_r42_figur(void);
const uint8_t *re15_story3a_r42_text(uint16_t room_id, uint8_t id, int *out_len);
const uint8_t *re15_story3a_r42_programm(int welches, int *out_len);   /* 0 = 3020 sub02, 1 = 3060 sub03, 2 = Wiedersehen */
int  re15_story3a_r42_szene_laeuft(void);
extern unsigned g_re15_s3a_zaehler[12];  /* 0 Tausch hin, 1 Tausch zurueck, 2 Neuladen im Tausch, 3 3020-Programm, 4 3060-Programm,
                                          * 5 Schuesse, 6 Schuesse gehoert (msg 9), 7 Wiedersehen-Start, 8 Uebergabe-Gegenstaende,
                                          * 9 frei, 10 Fabrikplan, 11 Uebergabe in die Kiste */

#endif /* RE15_STORY3A_R42_H */
