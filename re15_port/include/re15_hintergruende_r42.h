/*
 * RE1.5 Rebuilt — Runde 42 "rest", Stufe 2 "Kleine Hintergrund-Funde": nie gezeigte Kameras beim Untersuchen zeigen.
 *
 * AUFTRAG (Orchestrator Strang rest, Stufe 2/3, nach scratchpad hintergruende/VORSCHLAEGE.md B5/B6/D4 und PRUEFUNG.md; die
 * Pruefung hat Vorrang): "B5 Funkpult 10F (Cut 9/10/11) als Nahbild zum Satz 'It's out of commission'; B6 Memory-Card-Ecke 1010
 * (Cut 7); ... D4 6031 Elza als Zusatz-Einstellungen - nur, was PRUEFUNG.md bestaetigt. Nur Original-Befehle
 * (Cut_chg/Cut_replace), RDT unveraendert, Leon und Elza." Nutzer 17.11. (Memory reai-v2-unbenutzte-hintergruende):
 * "unbenutzte Hintergruende ... in der Story verweben".
 *
 * ZENSUS "nie gezeigt" (PRUEFUNG.md Abschnitt 1, Zeilen 8/9, unabhaengig nachgezaehlt): Kamera ohne RVD-Zone hinein, ohne
 * Cut_chg/Cut_replace in irgendeiner Region, ohne Tuer-Eintritt und ohne Port-Code:
 *   10F0/10F1 Cut 9/10/11  Kameras RDT @0x180/@0x1A0/@0x1C0, RVD-Anker @0x3F4/@0x408/@0x41C (nur Anker)
 *   1010/1011 Cut 7        Kamera RDT @0x140, RVD-Anker @0x2B0 (nur Anker)
 * D4 (6031 Cut 11) steht in tools/r42_story/szenen_stage6.py (Elzas Wagen-Programm), nicht hier.
 *
 * BILD GEGEN STELLE (Messbild scratchpad rest/s2/M1..M3, Projektion H = fov>>7 = 208 wie camera_common.c, Spiegel geprueft an
 * der r35-Kartenmitte: 1010 Cut 7 -> (273,8 ; 164,8), re15_inhalt_r35.h:51-56 nennt (274,0 ; 164,5)):
 *   10F Cut 10 = Nahaufnahme der Funkkonsole (L-foermig an Nord- und Ostwand). Beide Text-Saetze msg 0 "A large
 *     communications device. It's out of commission. I can't send any messages from here." liegen vor dieser Konsole:
 *       Platz 2 main00 @0x00F72 `2c 02 01 31 .. 58 1b 28 0a 20 03 e4 25 00 00 ff ff` Rechteck (7000,2600,800,9700) Ostwand
 *       Platz 3 main00 @0x00F86 `2c 03 01 31 .. 84 03 a8 2f 70 17 20 03 00 00 ff ff` Rechteck (900,12200,6000,800) Nordwand
 *     (Platz 3 liegt ganz in Cut 10, die Ostwand ab z ~9000; dieselbe Konsole, derselbe Text -> beide Plaetze).
 *   10F Cut 9 = Regal mit Aktenordnern und Geraeteschrank; Platz 6 main00 @0x00FC2 `2c 06 01 31 .. f0 f1 88 fa 80 0c 20 03 02 00
 *     ff ff` msg 2 "There isnt anything useful on this shelf." Rechteck (-3600,-1400,3200,800) liegt im Bild vor dem Regal.
 *   10F Cut 11 (Ueberblick mit Stuhl) hat keine eigene Stelle -> NICHT eingebaut (offen im Bericht).
 *   1010 Cut 7 = hintere Ecke mit dem Regal, in dem die Memory Card (Runde 35, re15_inhalt_r35.h) steht.
 *
 * FORM DER PROGRAMME — jede Zeile ein Original-Befehl (Vorbilder wie re15_nahaufnahmen_r40.h):
 *   `22 02 07 01` Set(2,7,1) Pad-Sperre      = ROOM1051 sub03 @0x00DA4
 *   `29 nn`       Cut_chg                    = ROOM1051 sub03 @0x00DB4 (`29 09`, Leiche)
 *   `2b mm ff ff` Message_on + `02 00` Evt_next = ROOM1051 sub03 @0x00DB6/@0x00DBA
 *   `2a 00` Cut_old + `3c 01` Cut_auto 1    = ROOM11D0 sub02 @0x01884/@0x01886
 *   `22 02 07 00` Set(2,7,0)                 = ROOM11D0 sub02 @0x01888
 * MEMORY CARD (Nehmen mit Nahbild) in der Form der Leiche ROOM1051, die Elza schon so zeigt:
 *   ROOM1051 main00 @0x00C0E `2c 0b 03 31 .. 92 3b ae e3 e8 03 e8 03 ff 00 18 03` Ereignis-Platz 11 (sce 3) mit dem Rechteck,
 *   ROOM1051 main00 @0x00C22 `50 0c 09 31 00 00 00 00 00 00 00 00 00 00 04 00 0f 00 a5 00 ff 00` Item-Platz 12 mit Rechteck 0,
 *   ROOM1051 sub03 @0x00DBC `47 0c` Aot_on 12 (Aufnahme-Frage) + @0x00DBE Evt_next, danach die Kamera zurueck.
 *   Der Port legt die Karte (r35) in Platz 10 mit Rechteck; hier: Platz 10 behaelt Item/Menge/Bit/Prop, sein Rechteck wird 0
 *   (Form @0x00C22), Platz 11 wird der Ereignis-Platz mit dem alten Rechteck (Form @0x00C0E). Der Aktions-Scan nimmt den
 *   kleinsten treffenden Platz (FUN_80042bac, kartenitems_r36.c) - Platz 10 mit Rechteck 0 trifft nichts mehr ausser genau
 *   dem Punkt (0,0) (Vergleich `<=`, aot_common.c:481; dieselbe Randlage hat 1051 Platz 12).
 *   Nach der Frage: Karte genommen (Bit (9,80) RE15_R35I_KARTE_BIT) -> Aot_reset Platz 11 sce 0. Das ist Byte fuer Byte der
 *   Waechter der Leiche ROOM1051 sub01 @0x00CB2..@0x00CC5 `06 00 10 00 | 21 09 a5 01 | 46 0b 00 00 00 00 00 00 00 00 | 08 00`
 *   (Ck(9,0xA5)=1 -> Aot_reset 11 sce 0), nur mit dem Karten-Bit 0x50 statt 0xA5. Sonst ("Nein") bleibt der Platz und die
 *   Karte kann spaeter genommen werden.
 *
 * EREIGNIS-NUMMERN — PORT-WAHL (< RE15_RDT_MAX_SUB_SCD 32): 31 wie re15_nahaufnahmen_r40.h (dort nur 11D0/1110/1100), 30 fuer das
 *   Regal. In 10F0/10F1/1010/1011 sind die RDT-Subs 0..1 (scd_dump_room.py), die Port-Programme nutzen dort 20 (cut_10f0) und
 *   27 (funk10f1); 30/31 sind in diesen Raeumen frei. PLATZ 11 in 1010/1011: RDT nutzt 0..7 bzw. 0..8, der Port 9 (Dokument 4),
 *   10 (Karte) und in 1011 14 (Dining Key, re15_dining_r38.h) -> 11 frei.
 */
#ifndef RE15_HINTERGRUENDE_R42_H
#define RE15_HINTERGRUENDE_R42_H

#include <stdint.h>

#define RE15_HG42_EREIGNIS        31     /* PORT-WAHL (siehe oben) */
#define RE15_HG42_EREIGNIS_REGAL  30     /* PORT-WAHL (siehe oben) */

/* 10F0/10F1 Funkkonsole: Plaetze 2 und 3 (msg 0), Cut 10 */
#define RE15_HG42_FUNK_SLOT_OST   2      /* main00 @0x00F72 */
#define RE15_HG42_FUNK_SLOT_NORD  3      /* main00 @0x00F86 */
#define RE15_HG42_FUNK_MSG        0
#define RE15_HG42_FUNK_CUT        10     /* Kamera RDT @0x1A0 */
/* 10F0/10F1 Regal: Platz 6 (msg 2), Cut 9 */
#define RE15_HG42_REGAL_SLOT      6      /* main00 @0x00FC2 */
#define RE15_HG42_REGAL_MSG       2
#define RE15_HG42_REGAL_CUT       9      /* Kamera RDT @0x180 */

/* 1010/1011 Memory Card: Item-Platz 10 (RE15_R35I_KARTE_SLOT), Ereignis-Platz 11, Cut 7 */
#define RE15_HG42_KARTE_EVT_SLOT  11
#define RE15_HG42_KARTE_CUT       7      /* Kamera RDT @0x140 */

/* Nach dem Init-Lauf (scd_room_setup.c, nach allen Installern): Plaetze umwidmen bzw. anlegen. */
void re15_hintergruende_r42_install(uint16_t room_id);

/* scd_event_fire-Kette: Port-Programm fuer (Raum, Ereignis) oder NULL. */
const uint8_t *re15_hintergruende_r42_ereignis(uint16_t room_id, uint8_t event_id);

/* Fuer den Riegel: Programm je Name ("funk", "regal", "karte") und seine Laenge. */
const uint8_t *re15_hintergruende_r42_programm(const char *name, int *out_len);

#endif /* RE15_HINTERGRUENDE_R42_H */
