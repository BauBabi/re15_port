/*
 * RE1.5 Rebuilt — Runde 41 Spur O "keyitems": Key Items, die keine Schluessel sind, werden wie in RE2 Retail aus dem Inventar
 * benutzt. Dossier mit allen Messungen und Belegen: analysis/befunde_runde41/O_keyitems.md.
 *
 * NUTZER-VORGABE (AUFTRAG.md Nachtrag 34, woertlich):
 *   "Bei Resident Evil 2 ist die Kurbel Animation offenbar deshalb nicht kaputt/fliegend, weil man EXPLIZIT die Items im Player
 *    Inventory auswaehlen muss und dann genau richtig steht bei der Nutzung. Das moechte ich bei Key Items, die nicht Schluessel
 *    sind bei unseren Port auch haben!"
 *   Karten: "Schaue wie es in Resident Evil 2 gemacht wird, und mache es genauso."
 *
 * WAS RE2 TUT (info/re2leon/PSX.EXE, selbst disassembliert; volle Kette im Dossier):
 *   Stelle  = AOT sce 6 sat 0x21 (Auto, Vorwaerts-Punkt) auf dem Rechteck des Text-/Ereignis-AOT, Nutzlast Bank 0x0A Bit <Item-Id>
 *             (ROOM1110 @0x020A0 `2c 07 06 21 .. 0a 00 43 00 01 00`); Bank 0x0A lebt ein Bild (FUN_80052e20 @0x800537a0).
 *   USE     = FUN_8006affc Fall 0 @0x8006b21c Bit-Test -> Fall 3 @0x8006b2e0 Schliessen + @0x8006b2ec Variable 2 := Id;
 *             sonst Text 7 @0x8006b248, Inventar bleibt offen.
 *   Skript  = Raum-Task `cmp 0,2,0,<Id>` (ROOM1110 sub01 @0x02336) -> Ereignis OHNE Text und Frage.
 *   Aktion  = am selben Rechteck nur der Text (ROOM1110 Slot 8 sce 4 @0x020B4, keine Frage).
 *   Kurbel  = sub13 Pos_set @0x02EDA/@0x02EE6, sub15 @0x02FC1 `3f 00 0f 45` Plc_motion Flags 0x40 = KEINE Blende, Waffe abgelegt
 *             (@0x80068d0c `li v0,0x43`, Waffe 0 @0x80068d34), Ende @0x02F66 `62 43` sce_item_lost.
 *   Karten  = Leser/Ereignis wie die Kurbel (Red Card Key ROOM2110 @0x01812/@0x01AD8), Karte als TUERSCHLOSS (Lab Card Key, Door_aot
 *             key_type 0x61, Handler 0x80051514) bei der Aktionstaste ohne Frage.
 *
 * WAS DER PORT DARAUS MACHT: die RDT-Skripte bleiben bytegleich. Die Tabelle k_stellen (keyitems_r41.c) nennt je Stelle Raum, Item,
 * AOT-Slot, Ereignis und Frage-Id. USE im Inventar prueft dieselbe Stelle wie die Aktionstaste (Band + Rechteck/Vorwaerts-Punkt
 * 620 @0x80042bd0) und startet das Ereignis nach dem Schliessen ohne Texte und Frage mit Antwort Ja; die Aktionstaste zeigt nur die
 * Beschreibung, die Frage entfaellt ohne Anzeige mit Antwort Nein.
 */
#ifndef RE15_KEYITEMS_R41_H
#define RE15_KEYITEMS_R41_H

#include <stdint.h>

/* Die Kurbel gibt es in RE1.5 nur als Flag ((3,114), ROOM2010 sub02 @0x008CA): kein Eintrag in der 102er-Namenstabelle
 * (DEBUG.BIN @0x800c495c). PORT-WAHL Id 0x33: nie platziert (0 Item_aot_set in 206 RDTs, Zensus), Eigenschaftssatz @0x8007500c =
 * `01 00 00 00 88 4c 07 80 00 00 00 00` wie alle Schluesselkarten (Obergrenze 1), ausgeliefert "Minidisc", das ohne den seit
 * Spur F umgewidmeten Spieler 0x34 keinen Zweck mehr hat. Name, CHECK-Text, Icon, Bild = RE2 Item 0x43 (tools/r41_keyitems). */
#define RE15_R41O_KURBEL_ITEM   0x33
/* "Timer Bomb" (Name DEBUG.BIN, CHECK-Text "Time Bomb", ITPS-Block mit Bild) - in RE1.5 nie als Gegenstand vergeben: (3,146)
 * ROOM1110 sub03 @0x00D32. */
#define RE15_R41O_BOMBE_ITEM    0x35

/* USE im Inventar (menu_common.c, Klassifizierer hinter dem Heil-Test): 1 = Id passt zu einer Stelle vor dem Spieler -> Inventar
 * schliessen (RE2 @0x8006b2e0); 0 = "You can't use it here." (RE2 @0x8006b248 / RE1.5 c3=6 @0x8004ab6c). */
int  re15_keyitems_r41_use(uint8_t item_id);
/* Inventar-Teardown (close_phase): 1 = USE-Lauf vorgemerkt: das Ereignis ist gestartet und die Welt laeuft in der Blende, damit
 * Kamera und Lage stehen, bevor das Spielbild zurueckkommt. */
int  re15_keyitems_r41_schliessen(void);
/* op_message_on: 0 = normal weiter; 1 = Nachricht entfaellt (pc += 4, Antwort steht in (12,31)). */
int  re15_keyitems_r41_message_on(uint8_t msg_id);
/* op_evt_next: 1 = das Evt_next hinter einer entfallenen Nachricht zaehlt nicht (USE-Lauf: alles im ersten Skripttakt). */
int  re15_keyitems_r41_evt_next(void);
/* op_sleep: 1 = der Sleep vor dem Plc_motion entfaellt (Kurbel: RE2 hat ihn nicht, ROOM1110 sub13 -> sub15). */
int  re15_keyitems_r41_sleep(void);
/* op_plc_motion: 0x40 = RE2-Flag "keine Blende" (ROOM1110 @0x02FC1), sonst 0. */
int  re15_keyitems_r41_plc_flags(void);
/* re15_dialog_step Seitenumbruch: 1 = die Beschreibung endet hier (Karten-Leser: Text und Frage stehen in EINER Nachricht). */
int  re15_keyitems_r41_text_ende(int parse);

/* Je Spielbild (game_step, nicht im Menue): Besitz der Flag-Gegenstaende (Kurbel, Bombe, Sicherung aus 2030), Verbrauch. */
void re15_keyitems_r41_tick(void);
/* Raumaufbau (scd_room_setup.c, Ende des Blocks): Flanken-Merker und USE-Lauf zuruecksetzen. */
void re15_keyitems_r41_raum(uint16_t room_id);

/* Name (0x07-terminiert), CHECK-Text (01 00-terminiert) der Kurbel; NULL fuer jede andere Id. */
const uint8_t *re15_keyitems_r41_name(uint8_t item_id);
const uint8_t *re15_keyitems_r41_desc(uint8_t item_id);
/* Icon-Tile / Item-Bild im GELADENEN Puffer (Muster re15_sicherung.h). Dazu die Rechtecke des Blocks 0x35 (ausgeliefert RE2-Form
 * (0,480)/(0,0)): @0x800c0258 laedt an die eingebetteten Rechtecke, sonst bliebe das Foto der Bombe leer (Falle Block 0x40). */
void re15_keyitems_r41_icon_einsetzen(uint8_t *itemall, int size);
void re15_keyitems_r41_bild_einsetzen(uint8_t *itps, int size);

/* Pruefhaken (Riegel): Ja-Zweig der Stelle von `item_id` im aktuellen Raum starten, ohne Menue und Lage-Pruefung (das Gegenstueck zu
 * re15_aot_fire_slot der Aktionstaste); Messhaken RE15_USE_TEST="<item>@<bild>#<hexraum>" (main.c neben RE15_FIRE_AOT). */
int  re15_keyitems_r41_testlauf(uint8_t item_id);
/* Pruefhaken fuer Riegel, die den Skript-Zweig HINTER der Frage messen (Wegwerf-Abfrage, Tastenfeld, Rolltor): die entfallene Frage
 * gilt als Ja. Das Spiel ruft das nie; dort fuehrt nur USE in den Ja-Zweig. */
void re15_keyitems_r41_test_ja(int an);
void re15_keyitems_r41_testhaken(void);
/* Pruefhaken (Riegel). */
int  re15_keyitems_r41_stellen_anzahl(void);
int  re15_keyitems_r41_stelle(int i, uint16_t *raum, uint8_t *item, int *slot, int *ereignis, uint8_t *frage, uint8_t *art);
int  re15_keyitems_r41_lauf_aktiv(void);
unsigned re15_keyitems_r41_verschluckt(int ja);    /* Zaehler: entfallene Fragen, ja = 1 USE-Lauf, 0 Aktionstaste */

/* art-Bits der Stellen-Tabelle */
#define RE15_R41O_KOMBI    0x01   /* Beschreibung + Frage in EINER Nachricht (10D0/1230/11E0 msg 7): letzte Seite faellt weg  */
#define RE15_R41O_SOFORT   0x02   /* Kurbel: kein Sleep vor Plc_motion, keine Blende (RE2 ROOM1110 sub13/sub15)               */
#define RE15_R41O_WAFFE    0x04   /* Kurbel: Waffe ablegen (RE2 @0x80068cfc-@0x80068d40)                                       */
#define RE15_R41O_FLAGITEM 0x08   /* Gegenstand gibt es in RE1.5 nur als Flag: ohne Eintrag (volles Inventar im Altstand) bleibt die Frage */

#endif /* RE15_KEYITEMS_R41_H */
