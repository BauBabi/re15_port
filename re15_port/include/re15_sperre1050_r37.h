/*
 * RE1.5 Rebuilt — "I have to get the Chief first..." ZUSAETZLICH an der Tuer ROOM1050 -> ROOM10A0
 * (die Tuer, an der vor der Ada-Rettung die Ada-Szene laeuft), solange Leon den Chief noch holen muss.
 *
 * Runde 37, Spur F "sperre1050". NUTZER-VORGABE (woertlich, analysis/befunde_runde37/AUFTRAG.md):
 *   "bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die
 *    Luke bis nach F1 und kann den "Ich muss zuerst irons holen" Blocker bei der Tür überspringen. Ich
 *    schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada
 *    Cutscene triggern, wenn wir Ada noch nicht gerettet haben."
 * Dossier mit allen Belegen: analysis/befunde_runde37/F_sperre1050.md (Messung vorher = die Luecke an der
 * exe, RE-Belege, Wege-Zensus, Bedingung).
 *
 * DIE LUECKE (gemessen, F_belege/r7_e2e_vorher.txt): Lueftungsgitter ROOM10F0 Slot 1 @0x00F52 -> Laufsteg
 * ROOM1090 (Band 5, Spawn y -9000) -> ABSPRUNG ueber die SCA-Zelle ROOM1090 @0x01E3C (Typwort 0x530E,
 * Bit 1; Kletter-Sonde FUN_8002d474 Zweig ABWAERTS @0x8002d74c-794) -> Hof (Band 1) -> Tuer @0x0211A ->
 * ROOM1050-Sued -> Slot 4 @0x00B5A -> ROOM10A0 (B1) — an der Sperre 1060 -> 1040 vorbei. Der Wege-Zensus
 * ueber alle Door_aot_set der Leon-Raeume (F_belege/wege_zensus.txt) zeigt: 1050 -> 10A0 ist der EINZIGE
 * Uebergang in den Kellerteil mit dem Ende von Stage 1 (11B0/11C0) — die Sperre hier schliesst damit
 * auch den Fahrstuhl-Weg 1080 -> 1040.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. Im Original ist die Tuer nach der Ada-Rettung immer offen (ROOM1050
 * main00 installiert sie unbedingt @0x00B5A). Gebaut wie re15_tuer1060.h: ein Text-Platz (sce 1) auf dem
 * Rechteck der Tuer, solange ein Story-Flag fehlt (RE1.5-Vorbilder ROOM1170 main00 @0x01320, ROOM1130
 * sub01 @0x00A1C `46 03 01 31 01 00 ff ff 00 00`). Kein Asset-Patch: der Port widmet den Tuer-Slot nach
 * dem Init-Lauf von main00 um, wie es ein Aot_reset tut (LAB_80040738: `sb v0,0(a1)` @0x80040764 rec[0] =
 * sce, rec[1] = (rec[1]&0x80)+flags @0x8004076c-78, Nutzlast @0x80040788-a8, rec[2] = Band unberuehrt).
 *
 * BEDINGUNG (PORT-WAHL, Herleitung im Dossier "Bedingung"):
 *   gesperrt <=> Raum 0x1050  UND  (3,0xBB) = 1  UND  (9,71) = 1  UND  (9,73) = 0
 *   = "solange Leon den Chief noch holen muss" (Wortlaut "bevor dem Ende wo er noch zu Irons muss"): vom Plan
 *   "I'm going to get Chief Irons" (10F0-Szene, (9,71), re15_cut10f0.h) bis zur Irons-Todesszene ((9,73), deren
 *   ERSTES Opcode, re15_irons_tod.h) — dieselben Bits, mit denen die 1060-Sperre diesen Abschnitt erkennt
 *   (re15_tuer1060.h RE15_TUER1060_K_* / RE15_TUER1060_TOD_*); dort sind beide Sperren gleich.
 *   (3,0xBB)=0 (Ada noch nicht gerettet): nichts — dort gilt allein re15_adaruf.h (Szene Ereignis 13 bzw.
 *   Sperrtext msg 25). (9,71)=0 (Frueh-/Mittelspiel): nichts — die Original-Tuer bleibt (Riegel
 *   unit_r34n_d_adaruf_frei: (3,0xBB)=1 -> ROOM10A0). Der dritte Fall der 1060-Sperre "(3,94)=0" (noch nie beim
 *   Chief) gehoert zum Abstieg aus den oberen Etagen und ist mit (3,0xBB)=1 im Spiel nicht erreichbar (Dossier).
 *   Der Installer laeuft NACH re15_adaruf_install und fasst Slot 4 nur an, wenn er dann noch die Original-Tuer ist.
 */
#ifndef RE15_SPERRE1050_R37_H
#define RE15_SPERRE1050_R37_H

#include <stdint.h>
#include "re15_tuer1060.h"   /* RE15_TUER1060_K_* / RE15_TUER1060_TOD_* (Flag-Nummern), Nachrichten-Bytes */

/* Der Raum: ROOM1050 (Leon). ROOM1051 (Elza) bleibt offen — Elzas Spiel kennt weder die 10F0-Szene noch
 * die Irons-Szenen, und (3,0xBB) wird nur in ROOM1090 sub03 @0x024D2 gesetzt (re15_adaruf.h). */
#define RE15_S1050_RAUM          0x1050

/* Der Tuer-Satz: ROOM1050 main00 @Datei 0x00B5A
 *   `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 ...`
 *   = Door_aot_set Slot 4, sce 2, flags 0x31, Band 0, Rechteck (16700,-14700,1000,2000) -> Stage-Byte 0x00
 *     (@0x00B70), Raum 0x0A (@0x00B71) = ROOM10A0, Spawn (26200,-14400,24800). */
#define RE15_S1050_SLOT          4

/* FREIGABE der Ada-Logik: Ada gerettet = (3,0xBB), ROOM1090 sub03 @0x024D2 `22 03 bb 01` (re15_adaruf.h). */
#define RE15_S1050_ADA_BANK      3
#define RE15_S1050_ADA_BIT       0xBB

/* Form des Text-Platzes = die der RE1.5-Vorbilder und der 1060-Sperre (re15_tuer1060.h): sce 1, flags 0x31
 * (Tuer-Satz @0x00B5A pc[3]), Pausemaske 0xffff (alle 524 ausgelieferten sce-1-Saetze, scd_vm.c).
 * Handler sce 1 = 0x80043084: `lhu a3,2(v0)` @0x80043098 (Maske), `lhu a2,0(v0)` @0x8004309c (msg),
 * `jal 0x80027e68` @0x800430a0 = Nachricht oeffnen — kein Raumwechsel, keine Tuersequenz. */
#define RE15_S1050_SCE_TEXT      1
#define RE15_S1050_FLAGS         0x31
#define RE15_S1050_MASKE         0xffff

/* Der Abschnitt "Chief holen": dieselben Bits wie die 1060-Sperre (re15_tuer1060.h). */
#define RE15_S1050_PLAN_BANK     RE15_TUER1060_K_BANK     /* (9,71) 10F0-Szene gesehen (Spur K, Runde 35)       */
#define RE15_S1050_PLAN_BIT      RE15_TUER1060_K_BIT
#define RE15_S1050_TOD_BANK      RE15_TUER1060_TOD_BANK   /* (9,73) Irons-Todesszene gesehen (Spur L, Runde 35) */
#define RE15_S1050_TOD_BIT       RE15_TUER1060_TOD_BIT

/* Nachrichten-Id: VERTRAG Runde 37 §1.2, Spur F = ROOM1050 Port-Id 26..28. Belegt in ROOM1050: 0..8 (RDT,
 * Tabelle @0x00E44 off[0] = `12 00`), 20/21 (re15_rolltor.h), 22..25 (re15_adaruf.h). Der Text sind die
 * BYTES der 1060-Sperre (re15_tuer1060_meldung, tuer1060_1040.c k_meldung) — "denselben Text". */
#define RE15_S1050_MSG_ID        26

/* HAKEN scd_room_setup.c (NACH re15_adaruf_install, Blockende "Runde 37 Spur F") und Boot-/CONTINUE-Weg
 * (platform/pc/main.c, neben re15_tuer1060_install). Tut in jedem Raum ausser ROOM1050 nichts. */
void re15_sperre1050_r37_install(uint16_t room_id);

/* Die Bedingung allein (fuer Riegel und Dossier): 1 = Slot 4 wird zum Text-Platz (Raum nicht geprueft). */
int  re15_sperre1050_r37_aktiv(void);

/* Pruefhaken fuer Riegel: wurde die Sperre im aktuellen Raumaufbau gesetzt? */
int  re15_sperre1050_r37_gesperrt(void);

#endif /* RE15_SPERRE1050_R37_H */
