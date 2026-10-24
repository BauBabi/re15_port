/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (master-AUFTRAG E-N2): Bildreihenfolge
 * Nachrichten-Takt gegen Spiel-Task, gemessen am Kurbel-Anlauf ROOM2020 im Gehen/Rennen.
 *
 * ORIGINAL (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Hauptschleife: @0x80020c88 `jal 0x80030444` (Pad) -> @0x80020de4 `jal 0x800298b0` (Task-Plan -> Spiel-Task
 *   FUN_8001c958: @0x8001cdec `jal 0x8003f038` SKRIPT, @0x8001ce0c `jal 0x80031c44` SPIELER, @0x8001ce1c AOT,
 *   @0x8001ce2c ESP, @0x8001ce34 `jal 0x8001db28` Item-Modal) -> @0x80020f3c `jal 0x80010000` -> @0x80010044
 *   `jal 0x800280b4` NACHRICHTEN-TAKT (@0x800280dc `jal 0x80028134`) -> @0x80020f4c Present.
 *   Der Nachrichten-Takt laeuft also NACH dem Spieler. Schliesst er eine Nachricht (Restore `lw DAT_800b853c` ->
 *   `sw g_pauseflags` @0x800285a4 / @0x800286cc / @0x8002871c), war der Spieler in DIESEM Bild schon angehalten
 *   (@0x80031c54 `lw a0,g_pauseflags` / @0x80031c78 `bltz`); frei ist er erst im naechsten Bild — und dort laeuft
 *   VOR ihm der Skript-Takt, der z.B. das naechste Message_on (@0x800404f4, Rueckgabe 1 @0x80040520) oeffnet.
 *   Der Port takte SKRIPT -> NACHRICHTEN -> SPIEL (platform/pc/main.c); dadurch war der Spieler im Schliess-Bild
 *   frei. GEMESSEN (DuckStation-GDB, Dossier "Nachbesserung 2"): Original ROOM2020 Geh-Anlauf n46/n47 Clip 5 Bild 9
 *   durch beide Nachrichten; Port R2_geh F328 Bild 9 -> 10 (ein Bild Steuerung zwischen Message_on 0 @0x0A5C und
 *   Message_on 1 @0x0A62).
 *   UMSETZUNG 1: ein Restore, der IM Nachrichten-Takt des Hauptloops faellt, wird bis hinter den Spiel-Takt (vor dem
 *   Item-Modal-Takt, s. main.c) aufgeschoben. Oeffnet im selben Bild eine neue Nachricht, laeuft der aufgeschobene
 *   Restore vorher (sonst sichert @0x80027ec8 den Schnappschuss mit der alten Maske).
 *
 *   AUSWAHL-NACHRICHT: Message_on kehrt im Original immer sofort zurueck; den Faden haelt der Skript-Freeze
 *   (@0x8003f040-@0x8003f04c g_pauseflags & 0x02000000), solange die Frage offen ist. Census der ausgelieferten RDTs:
 *   102 Auswahl-Message_on, Masken 0xffff0000 (69) / 0xff800000 (33) — alle mit dem Skript-Bit, alle 102 gefolgt von
 *   Evt_next (0x02). Der Port parkte den Faden AUF dem Opcode (scd_vm.c op_message_on, return 2) und lief erst ein Bild
 *   nach dem Schliessen weiter (R2_geh: Pos_set @0x0A8C erst F383 statt im Bild nach dem Schliessen, Original n95).
 *   UMSETZUNG 2: haelt die Maske den Skript-Lauf an, laeuft der Opcode durch wie im Original (pc += 4, weiter mit
 *   Evt_next); message_query (das Port-Tor fuer msg_block) wird mit dem aufgeschobenen Restore zurueckgenommen.
 *
 * Aktiv nur, wenn die Plattform die Reihenfolge so faehrt (PC-main ruft re15_msgtakt_r41_takt_beginn); ohne Aufruf
 * (PSX-main, Unit-Sonden) bleibt alles wie vorher.
 */
#ifndef RE15_MSGTAKT_R41_H
#define RE15_MSGTAKT_R41_H

#include <stdint.h>

/* PC-main direkt VOR re15_msg_tick: ein liegengebliebener Restore wird nachgeholt, dann scharf. */
void re15_msgtakt_r41_takt_beginn(void);
/* PC-main direkt NACH re15_msg_tick. */
void re15_msgtakt_r41_takt_ende(void);
/* msg_common.c am Schliessen (fsm 6): 1 = Restore aufgeschoben (nur innerhalb des Nachrichten-Takts). */
int  re15_msgtakt_r41_schliessen_aufschieben(void);
/* msg_common.c vor re15_pauseflags_open: aufgeschobenen Restore zuerst ausfuehren. */
void re15_msgtakt_r41_vor_oeffnen(void);
/* PC-main hinter dem Spiel-Takt (und als Absicherung vor dem Bildende): aufgeschobenen Restore ausfuehren. */
void re15_msgtakt_r41_nach_spieltakt(void);
/* scd_vm.c op_message_on, Auswahl-Zweig beim Oeffnen: 1 = wie das Original durchlaufen (Maske haelt den Skript-Lauf). */
int  re15_msgtakt_r41_frage_durchlaufen(uint32_t pause_mask);

/* Pruefhaken */
int      re15_msgtakt_r41_aktiv(void);
int      re15_msgtakt_r41_restore_offen(void);
unsigned re15_msgtakt_r41_aufgeschoben_zahl(void);
void     re15_msgtakt_r41_zuruecksetzen(void);   /* Sonden: Ausgangszustand (inaktiv) */

#endif /* RE15_MSGTAKT_R41_H */
