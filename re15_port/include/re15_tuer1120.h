/*
 * RE1.5 Rebuilt — Tuer ROOM1130 -> ROOM1120 erst nach der ersten Irons-Szene.
 *
 * Runde 33, Thema R (Nutzer, 2026-09-29): "Man soll nicht eher von Room 1130 zu Room 1120 wechseln
 * koennen, bevor man einmal die 1. Cutscene mit chief Irons getriggert hat [...]. Vorher soll der
 * Text stehen "I have to report the situation to the chief first...""
 * Dossier mit allen Belegen: analysis/befunde_runde33/tuer_1130_1120.md.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. Im Original ist die Tuer immer offen (ROOM1130 main00 installiert
 * sie unbedingt). Gebaut ist die Sperre mit dem Mechanismus, den RE1.5 selbst fuer genau diesen
 * Fall hat — ein Text-Platz (sce 1) auf dem Rechteck der Tuer, solange ein Story-Flag fehlt:
 *   ROOM1170 main00 @0x01318 Ifel / @0x0131C `21 04 c3 00` Ck(4,195)==0 /
 *            @0x01320 `2c 05 01 31 00 00 2c a7 26 94 e8 08 ba 04 0c 00 ff ff 00 00` (Text, msg 12,
 *            Maske 0xffff, Rechteck der Tuer) / @0x01334 Else / @0x01338 Door_aot_set Slot 5.
 *   ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff 00 00` (Aot_reset Tuer 3 -> Text msg 1).
 * RE2 Retail sperrt Story-Tueren genauso und STUMM (ROOM2190 @0x00B6A -> sub02 @0x00B7A
 * "I have to get back to Ben!", ROOM6010 @0x0068A Text-Platz "I can't leave Sherry behind!";
 * RE2-Text-Handler @0x80051948 ruft nur `jal 0x8002fe38` @0x80051968, kein Se_on).
 */
#ifndef RE15_TUER1120_H
#define RE15_TUER1120_H

#include <stdint.h>

/* Der gesperrte Raum: ROOM1130 (Leon). ROOM1131 (Elza) bleibt offen — die Freigabe-Bedingung
 * gibt es in Elzas Spiel nicht: ROOM1151 hat weder den Ausloeser (ROOM1150 main00 @0x00DE2..
 * @0x00DFF fehlt) noch sub08, und (3,94) wird in keinem der 206 RDTs ausser ROOM1150 sub08
 * gesetzt (Zensus im Dossier §1b). Eine Sperre waere fuer Elza dauerhaft. */
#define RE15_TUER1120_RAUM        0x1130

/* Der Tuer-Satz: ROOM1130 main00 @Datei 0x008AE
 *   `3b 01 02 31 00 00 22 f2 b2 f3 e8 03 d0 07 50 e2 00 00 ac f4 00 00 00 12 03 00 ...`
 *   = Door_aot_set Slot 1, sce 2, flags 0x31, Band 0, Rechteck (-3550,-3150,1000,2000)
 *     -> Raum 0x12 (ROOM1120), Spawn (-7600,0,-2900), Cut 3. */
#define RE15_TUER1120_SLOT        1

/* Die Freigabe: Bank 3, Bit 94 = das Flag der ersten Irons-Szene.
 *   ROOM1150 main00 @0x00DE6 `21 03 5e 00` Ck(3,94)==0 -> @0x00DEA Aot_set Slot 6 (AUTO) -> sub08;
 *   ROOM1150 sub08  @0x01110 `22 03 5e 01` Set(3,94) := 1 (zweites Opcode der Szene).
 * Bank 3 ist spielweit: Zonen-Tabelle @0x80074664[3] -> 0x800b0ff8; der Raum-SCD-Init
 * FUN_8003ecec loescht nur Bank 5 Wort 0 (@0x8003ed74 `sw zero,0x800b1028`), nie Bank 3. */
#define RE15_TUER1120_FLAG_BANK   3
#define RE15_TUER1120_FLAG_BIT    94

/* Form des Text-Platzes = die der RE1.5-Vorbilder: sce 1, flags 0x31 (Tuer-Satz @0x008AE pc[3]
 * und ROOM1170 @0x01320 pc[3]), Pausemaske 0xffff (ROOM1170 @0x01320 Nutzlast +2, ROOM1130
 * @0x00A1C pc[6..7]; alle 524 ausgelieferten sce-1-Records tragen 0xffff, scd_vm.c). */
#define RE15_TUER1120_SCE_TEXT    1
#define RE15_TUER1120_FLAGS       0x31
#define RE15_TUER1120_MASKE       0xffff

/* Nachrichten-Id: die erste freie hinter den Raum-Nachrichten. ROOM1130-Nachrichtensektion
 * @Datei 0x0B0C, off[0] = `0c 00` -> 0x0C/2 = 6 Eintraege (Id 0..5). Der Raum-Lader
 * re15_msg_load_room_block beschreibt nur diese sechs, Id 6 bleibt uns (auch auf der PSX,
 * MSG_TABLE_N = 32). */
#define RE15_TUER1120_MSG_ID      6

/* Beim Raumaufbau (scd_room_setup.c, nach dem Init-Lauf von main00) rufen. Tut in jedem Raum
 * ausser ROOM1130 nichts; in ROOM1130 bei gesetztem Flag ebenfalls nichts (Original-Tuer). */
void re15_tuer1120_install(uint16_t room_id);

/* Pruefhaken fuer Riegel: die eingebackene Nachricht (.msg-Rohbytes) und ob die Sperre im
 * aktuellen Raumaufbau gesetzt wurde. */
const uint8_t *re15_tuer1120_meldung(int *out_len);
int  re15_tuer1120_gesperrt(void);

#endif /* RE15_TUER1120_H */
