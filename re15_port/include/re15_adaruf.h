/*
 * RE1.5 Rebuilt — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung.
 *
 * Runde 34 Nacht, Spur D. NUTZER-VORGABE (woertlich, analysis/befunde_runde34_nacht/AUFTRAG.md Z. 14-19):
 *   "Ich moechte, bevor Leon von ROOM 1050 zu ROOM 10A0 wechseln kann, das eine Cutscene stattfindet:
 *    Dort soll folgender Dialog passieren: - Ada: Hello? Anyone? Please, get me out of here!
 *    - Leon: Another civilian survivor. I have to help her! - Dabei soll Leon nach Adas Dialog einen
 *    Schritt zurueck machen von der Tuer, so wie in ROOM 1090 in der Cutscene, nachdem er zum Feuer
 *    laeuft. Dann bei "another Civilian Survivor" soll er diese Animation machen wo er den rechten Arm
 *    um 180 grad dreht und nach rechts bewegt [...]. Dann macht er den Arm den gleichen Weg wieder
 *    zurueck. Und bei "I have to help her" macht er seine Arm Schwung Animation [...].
 *    - Drueckt man nach dieser Cutscene erneut die Tuer soll der Text kommen "I have to help the
 *    Survivor first!". - Erst wenn man Ada gerettet hat in ROOM 1090, dann kann man durch die Tuer
 *    laufen."
 * Nutzer-Nachtrag 2026-09-30: "Du hast recht! Da muss woman: stehen statt Ada:".
 * Dossier mit allen Belegen: analysis/befunde_runde34_nacht/D_adaruf.md (§3 Mechanismus, §4 Zeitlinie,
 * §5 Bauplan, §9 Umsetzung); Gegenpruefung D_adaruf.gegenpruefung.md.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. Im Original ist die Tuer immer offen (ROOM1050 main00 installiert
 * sie unbedingt, @0x00B5A). Gebaut ist die Szene als PORTSEITIG eingespielter SCD-Bytecode aus
 * ORIGINAL-Opcodes (jede Zeile mit ihrem Vorbild in adaruf_1050.c), ausgefuehrt von der vorhandenen
 * VM mit der Original-Zeitsemantik — kein Asset-Patch, die RDT bleibt byte-true. Die Sperre danach ist
 * der RE1.5-eigene Mechanismus fuer genau diesen Fall: ein Text-Platz (sce 1) auf dem Rechteck der
 * Tuer, solange ein Story-Flag fehlt (ROOM1170 main00 @0x01318..@0x01338; ROOM1130 sub01 @0x00A1C;
 * im selben Handlungsstrang ROOM1090 sub00 @0x0230A `21 03 85 00` Ck(3,133)==0 -> @0x0230E Text-Platz
 * msg 7 "I must hurry up and get something to / put out this fire to save that woman!").
 *
 * ABLAUF:
 *   Raumaufbau (scd_room_setup.c, nach dem Init-Lauf von main00): re15_adaruf_install widmet den
 *   Tuer-Slot 4 um — solange (3,0xBB)=0: (9,65)=0 -> Ereignis-Platz sce 3 / Ereignis 13 (Szene),
 *   (9,65)=1 -> Text-Platz sce 1 / msg 25 (Sperrtext). (3,0xBB)=1 -> nichts: Original-Tuer inklusive
 *   der Tuersequenz (door_seq_zuordnung.c haengt an aot_fire_door, das dann wieder laeuft).
 *   Quadrat an Slot 4 -> Aktions-Scan -> scd_event_fire(13) -> re15_adaruf_ereignis liefert das
 *   Port-Programm (Ziele aus der Spielerposition im Druckbild eingesetzt) -> VM-Faden im ersten
 *   freien Ereignis-Slot wie jedes Raum-Sub. Das Programm setzt (9,65) als ERSTES Opcode und schaltet
 *   Slot 4 am Ende per Aot_reset selbst auf den Sperrtext um.
 */
#ifndef RE15_ADARUF_H
#define RE15_ADARUF_H

#include <stdint.h>

/* Der Raum: ROOM1050 (Leon). ROOM1051 (Elza) bleibt offen — die Freigabe gibt es in Elzas Spiel
 * nicht: ROOM1091 hat weder Feuer noch Ada noch Szene (sub00 @0x0222E = `01 00`, sub01 @0x02230 =
 * `01 00`), und (3,0xBB) wird in keinem der 206 RDTs ausser ROOM1090 sub03 @0x024D2 gesetzt.
 * Eine Sperre waere fuer Elza dauerhaft (gleiche Regel wie re15_tuer1120.h: nur ROOM1130). */
#define RE15_ADARUF_RAUM            0x1050

/* Der Tuer-Satz: ROOM1050 main00 @Datei 0x00B5A
 *   `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 ...`
 *   = Door_aot_set Slot 4, sce 2, flags 0x31, Band 0, Rechteck (16700,-14700,1000,2000)
 *     -> Stage-Byte 0x00 (@0x00B70), Raum 0x0A (@0x00B71) = ROOM10A0, Spawn (26200,-14400,24800). */
#define RE15_ADARUF_SLOT            4

/* FREIGABE = Ada gerettet: Bank 3, Bit 0xBB (187).
 *   ROOM1090 sub03 @0x024D2 `22 03 bb 01` Set(3,187) := 1 (Rettungsszene, zwischen @0x024CE
 *   `22 03 84 00` und @0x024D6 `22 03 6e 01`). Walker-Zensus 206 RDTs (40674 Opcodes, scd_walk_lib):
 *   gesetzt NUR dort, gelesen nur ROOM1090 sub00 @0x0237A, NIE geloescht.
 * ⛔ NICHT (3,0x6E): das setzt dieselbe Szene @0x024D6, aber ROOM1050 sub03 loescht es @0x00D88
 *   `22 03 6e 00` (erstes Opcode der "Hey, wait!"-Szene) beim ersten Wiederbetreten — die Tuer waere
 *   danach wieder zu (Softlock).
 * Bank 3 ist spielweit (Zonen-Tabelle @0x80074664[3] -> 0x800b0ff8; der Raum-SCD-Init FUN_8003ecec
 * loescht nur Bank 5 Wort 0, @0x8003ed74) und liegt im Speicherstand (re15_savedata.c memcpy flags). */
#define RE15_ADARUF_FREI_BANK       3
#define RE15_ADARUF_FREI_BIT        0xBB

/* "Szene gesehen" — PORT-WAHL (VERTRAG Runde 34 Nacht §1.1: Spur D = Bank-9-Bits 65/66;
 * Walker-Zensus 206 RDTs: kein Ck/Set (9,65)/(9,66); Item-Bits: freier Block 53..84 laut
 * re15_irons_tisch.h; kein Port-Code). Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9]), gespeichert
 * mit g_game.flags. Bit 66 bleibt Reserve. */
#define RE15_ADARUF_GESEHEN_BANK    9
#define RE15_ADARUF_GESEHEN_BIT     65

/* Ereignis-Nummer der Szene — PORT-WAHL, keine Original-Adresse. Grund: < RE15_RDT_MAX_SUB_SCD (32,
 * Schranke in scd_event_fire), kein ROOM1050-Sub (Sub-Tabelle @0x00C10 off[0] = `0a 00` = 5 Eintraege),
 * nicht Spur As Ereignis 2 (re15_rolltor.h); 13 = die Vertrags-Slotnummer (VERTRAG §1.2) zur
 * Wiedererkennung. */
#define RE15_ADARUF_EREIGNIS        13

/* Form der umgewidmeten Plaetze (Aot_reset-Semantik LAB_80040738, re15_aot_retype):
 *   Ereignis-Platz sce 3: AOT-Typtabelle @0x8007469c[3] = 0x800430f0; Handler `lhu a0,0(v0)`
 *     @0x800430fc (Nutzlast +0), `lbu a1,3(v0)` @0x80043100 (= Sub/Ereignis), `jal 0x8003ee3c`
 *     @0x80043104. Nutzlast-Form = Rolltor-Schalter ROOM1050 sub00 @0x00C22 `... ff 00 18 02 00 00`
 *     (p0 = 0x00FF, p1 = 0x0218 -> hier 0x0D18 = Ereignis 13 in Byte 3).
 *   Text-Platz sce 1: Typtabelle [1] = 0x80043084; `lhu a3,2(v0)` @0x80043098 (Maske), `lhu a2,0(v0)`
 *     @0x8004309c (msg), einziger Aufruf `jal 0x80027e68` @0x800430a0 = Nachricht oeffnen — kein
 *     Raumwechsel, keine Tuersequenz, kein Ton. Form ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff
 *     00 00` (msg, Maske 0xffff); alle 524 ausgelieferten sce-1-Saetze tragen 0xffff (scd_vm.c).
 *   Flags 0x31 = die des Tuer-Satzes (@0x00B5A pc[3]) und des Schalters (@0x00C22 pc[3]). */
#define RE15_ADARUF_SCE_EREIGNIS    3
#define RE15_ADARUF_SCE_TEXT        1
#define RE15_ADARUF_FLAGS           0x31
#define RE15_ADARUF_P0_EREIGNIS     0x00FF
#define RE15_ADARUF_P1_EREIGNIS     ((uint16_t)((RE15_ADARUF_EREIGNIS << 8) | 0x18))
#define RE15_ADARUF_MASKE_TEXT      0xFFFF

/* Portseitige Nachrichten (VERTRAG §1.3: Spur D = 22..25). ROOM1050-Nachrichtentabelle @0x00E44
 * off[0] = `12 00` -> 9 Eintraege (Id 0..8); der Raum-Lader re15_msg_load_room_block beschreibt nur
 * diese neun. PSX-Tabelle MSG_TABLE_N = 32. Sprachdateien (vom Nutzer, MiniMax):
 * synchro/STAGE1/room1050/main22.wav .. main24.wav (scd_queue_voice); msg 25 ist ein Text-Platz und
 * bekommt keine Stimme (scd_vm.c re15_scd_show_message "No voiceover"). */
#define RE15_ADARUF_MSG_RUF         22   /* "Woman: Hello? Anyone? Please, / get me out of here!" */
#define RE15_ADARUF_MSG_LEON_A      23   /* "Leon: Another civilian survivor."                    */
#define RE15_ADARUF_MSG_LEON_B      24   /* "Leon: I have to help her!"                           */
#define RE15_ADARUF_MSG_SPERRE      25   /* "I have to help the Survivor first!"                  */

/* Rueckschritt-Laenge = NUTZER-VORGABE "so wie in ROOM 1090": |(613,-2123) - (-130,-1988)| = 755,2 —
 * Startstellung ROOM1090 sub02 Member_set @0x0246C `34 00 65 02` (x=613) / @0x02470 `34 02 b5 f7`
 * (z=-2123), Ziel Plc_dest @0x0247C `40 00 08 20 7e ff 3c f8` (-130,-1988). Tatsaechlich gelaufen
 * werden ~700: Modus 8 (0x800311f0, Tabelle 0x80073e30[8]) schiebt 70/Bild (`ori v0,zero,0x46`
 * @0x80031210, `sh` +0x8c @0x80031218) und endet bei Rest < 100 (`slti v0,v0,100` @0x800312fc). */
#define RE15_ADARUF_SCHRITT         755

/* Nach dem Rueckschritt dreht Leon sich zur KAMERA (Plc_dest Modus 9 auf ihren Standort), dann erst
 * die zwei Leon-Zeilen mit den Gesten. ⛔ PORT-WAHL, keine Original-Adresse fuer die Blickrichtung —
 * Grund (gemessen an der echten exe, Dossier §9 / Gegenpruefung Auflage 7): alle Leon-Gesten bewegen
 * den LINKEN Arm (Dossier §3.5), der Nutzer beschreibt sie aber als "den rechten Arm ... nach rechts"
 * — das ist die Sicht von VORN (sein linker Arm liegt dann rechts im Bild und geht nach rechts
 * hinaus). Zur Tuer gewandt sieht Cut 4 Leon schraeg von hinten links: der Arm ginge im Bild nach
 * LINKS, der Schwung (Clip 17) verschwindet halb hinter dem Koerper. Eine Kamera, die Leon an der
 * Tuer von vorn zeigt, gibt es nicht: Kameratabelle RDT @0x00060 (10 Saetze a 0x20) — Cut 0..4, 6..9
 * stehen westlich/noerdlich (x 14332..16171), Cut 5 (@0x00100, Standort (20036,-22179)) suedoestlich:
 * er saehe Leons rechte Seite (Waffenhand) und zeigt ihn an der Tuer nur als Streifen hinter der
 * Wandkante (Dossier §1 L9). An der Tuer ist aus jeder Richtung Cut 4 aktiv (RVD-Baender, §2.2).
 * Also dreht sich Leon zur aktiven Kamera. Standort Cut 4 = ROOM1050.RDT @0x000E0 `00 00 3c 68 97 3a
 * 00 00 23 f2 ff ff 34 e0 ff ff ...` (pos_x @0x000E4 = 14999, pos_z @0x000EC = -8140). */
#define RE15_ADARUF_KAMERA_X        14999
#define RE15_ADARUF_KAMERA_Z        (-8140)

/* Operanden-Stellen im Programm, die die Weiche beim Ausloesen setzt (x/z als LE s16):
 *   +0x28 = Blickpunkt des Plc_dest Modus 9 (x + SCHRITT, z)  — Drehung zur Tuerwand
 *   +0x3C = Ziel des Plc_dest Modus 8      (x - SCHRITT, z)  — Rueckschritt
 * (+0x50 = Kamera-Standort, fest im Programm.) */
#define RE15_ADARUF_OFF_DREH        0x28
#define RE15_ADARUF_OFF_ZIEL        0x3C
#define RE15_ADARUF_OFF_KAMERA      0x50
#define RE15_ADARUF_PROG_LEN        162

/* Zustand nach der Installation (Pruefhaken). */
#define RE15_ADARUF_AUS             0    /* nicht ROOM1050, gerettet, oder Slot 4 keine Tuer */
#define RE15_ADARUF_SZENE           1    /* Slot 4 = Ereignis-Platz, Szene scharf            */
#define RE15_ADARUF_SPERRE          2    /* Slot 4 = Text-Platz msg 25                       */

/* HAKEN scd_room_setup.c (nach dem Init-Lauf von main00, neben re15_tuer1120_install): widmet Slot 4
 * um (siehe oben) und setzt die Nachrichten 22..25 ein. Tut in jedem anderen Raum nichts. */
void re15_adaruf_install(uint16_t room_id);

/* HAKEN scd_event_fire: liefert das Port-Programm, das statt sub_scd[ereignis] laufen soll, oder NULL
 * (dann gilt der ausgelieferte sub_scd-Eintrag; fuer ROOM1050 Ereignis 13 gibt es keinen -> das
 * Ereignis wird verworfen). Nicht-NULL NUR fuer ROOM1050, Ereignis 13, (9,65)=0, (3,0xBB)=0 und wenn
 * kein Faden das Programm schon ausfuehrt (Gegenpruefung Auflage 5: genau EIN Faden, Ziele unberuehrt). */
const uint8_t *re15_adaruf_ereignis(uint16_t room_id, uint8_t event_id);

/* Pruefhaken fuer Riegel/Sonde (kein Spielverhalten). */
const uint8_t *re15_adaruf_programm(int *out_len);     /* Vorlage k_ruf (Operanden +0x28/+0x3C = 0) */
const uint8_t *re15_adaruf_laufprogramm(void);         /* die RAM-Kopie, die die VM ausfuehrt       */
const uint8_t *re15_adaruf_meldung(int msg_id, int *out_len);   /* .msg-Rohbytes 22..25, sonst NULL */
int            re15_adaruf_zustand(void);              /* RE15_ADARUF_AUS / _SZENE / _SPERRE        */

#endif /* RE15_ADARUF_H */
