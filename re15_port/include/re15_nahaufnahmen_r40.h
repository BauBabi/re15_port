/*
 * RE1.5 Rebuilt — Runde 40 "nahaufnahmen": STAGE1-Nahaufnahmen, die im Auslieferungsstand nie gezeigt werden, beim
 * Untersuchen der zugehoerigen Stelle zeigen.
 *
 * NUTZER-VORGABE (2026-10-23, woertlich): "ok. GIbt es noch weitere Dinge in STAGE1 mit son einer Nahaufnahme die auf
 * Rätsel oder Informationen hindeuten könnten? Wenn ja, baue die ruhig mit ein."
 * Herleitung: analysis/befunde_runde40/UEBERGABE.md (Zensus "Kamera ohne Ausloeser").
 *
 * ZENSUS (alle STAGE1-RDTs): Cut ohne RVD-Zielzone, ohne Cut_chg/Cut_replace in irgendeiner Region und ohne Tuer-
 * Eintritt aus irgendeinem Raum (Door_aot_set pc[22..24]). Davon Nahaufnahmen VOR einer Untersuchungsstelle, deren
 * Text zum Bild passt (Kamera P/T gegen die Zonen-Mitte; Bilder per bg_dump):
 *   11D0/11D1 Cut 12  Zwinger-Pult mit den Hundenamen  P(-3637,-21815) -> Platz 2 main00 @0x01248 msg 0
 *                     "A panel to control the cells of the dogs. It does not work." (1867 vor der Kamera)
 *   1110/1111 Cut 11  Waffenspinde (Plakette, Schloss)  P(-10991,3773) Blick -z -> Platz 16 main00 @0x00C70 msg 4
 *                     "Weapons seized, those that remain are unusable." (Blickpunkt (-11031,-2438) in der Zone)
 *   1110/1111 Cut 12  Spind "EXPLOSIVE"                  P(-12861,541) Blick -x -> Platz 20 sub00 @0x00CD4 -> sub03
 *                     "Will you take the Time Bomb?" (Zone (-14200..-13400,-1250..-250), 1596 vor der Kamera)
 *   1100/1101 Cut 9/10 VUS-Anzeige "INVALID" / "TRY AGAIN" — das Stimmschloss zeigt im Original nur Cut 12 (leer),
 *                     7 "SYSTEM READY", 8 "SAY HELLO" (sub08 @0x00D86/@0x00D90) und 11 "ACCEPTED" (sub02 @0x00C9C);
 *                     der Fehlerfall fehlt. Stelle: Platz 2 sub00 @0x00AD6 msg 1 "It's a control panel. A type of
 *                     voice device is required to open the lock." (nur solange (3,0x67)=0, Ifel @0x00AC4)
 *   11F0/11F1 Cut 12  Notstrom-Pult an der Wand (Weltkamera, 5 Lampen) P(124,-15167) -> Platz 1 sub00 @0x00D64 -> sub16
 *                     (msg 0 Beschreibung, msg 1 Frage, Ja -> Cut 10 = Raetselbuehne @0x015C0)
 * Ohne Text/Stelle und deshalb NICHT eingebaut: 1010 Cut 2/3 (Messlatte), 1090 Cut 14 (ausgebranntes Fahrzeug),
 * 10E0 Cut 10 (Bett), 1171 Cut 12 (Schild), 1190 Cut 10 (Bahnfenster "1"). 1050/1051 Cut 7/8 (Sicherungskasten) nutzt
 * der Port schon (rolltor_1050.c); 1050 Cut 9 (Leiche mit Waffe) zeigt Elzas 1051 sub03 @0x00DB4 selbst.
 *
 * FORM DER PROGRAMME — jede Zeile ein Original-Befehl:
 *   `22 02 07 01` Set(2,7,1) Pad-Sperre      = ROOM1051 sub03 @0x00DA4 / ROOM11D0 sub02 @0x01754
 *   `29 nn`       Cut_chg Nahaufnahme        = ROOM1051 sub03 @0x00DB4 (`29 09`, Leiche)
 *   `2b mm ff ff` Message_on + `02 00` Evt_next = ROOM1051 sub03 @0x00DB6/@0x00DBA
 *   `2a 00` Cut_old + `3c 01` Cut_auto 1    = ROOM11D0 sub02 @0x01884/@0x01886
 *   `22 02 07 00` Set(2,7,0)                 = ROOM1051 sub03 @0x00DD2 / ROOM11D0 sub02 @0x01888
 *   VUS: Takt Sleep 30 `09 0a 1e 00` = ROOM1100 sub08 @0x00D88; Rueckweg `22 02 07 00` `29 06` `3c 01`
 *        = ROOM1100 sub02 @0x00CC0/@0x00CC4/@0x00CC6.
 *   Die Original-Teile (1110 sub03, 11F0 sub16) stehen Byte fuer Byte drin; der Riegel vergleicht sie mit der RDT.
 *
 * STELLEN: die drei reinen Text-Plaetze werden zum Ereignis-Platz umgewidmet in der Form ROOM1110 sub02 @0x00D18
 *   `46 05 03 31 ff 00 18 02 00 00` (Aot_reset sce 3, sat 0x31, Nutzlast ff 00 18 <Ereignis>), Rechteck bleibt.
 *   Ereignis 31 (< RE15_RDT_MAX_SUB_SCD 32; keiner der Raeume hat so viele Subs, kein anderes Port-Programm nutzt 31).
 *   Zeitbombe und Notstrom-Pult behalten ihre Original-Ereignisse 3 bzw. 16; der Port reicht das umhuellte Programm.
 */
#ifndef RE15_NAHAUFNAHMEN_R40_H
#define RE15_NAHAUFNAHMEN_R40_H

#include <stdint.h>

#define RE15_N40_EREIGNIS        31

#define RE15_N40_PULT_SLOT       2      /* 11D0/11D1 main00 @0x01248, msg 0 */
#define RE15_N40_PULT_MSG        0
#define RE15_N40_PULT_CUT        12

#define RE15_N40_SPIND_SLOT      16     /* 1110/1111 main00 @0x00C70, msg 4 */
#define RE15_N40_SPIND_MSG       4
#define RE15_N40_SPIND_CUT       11

#define RE15_N40_BOMBE_EREIGNIS  3      /* 1110/1111 sub00 @0x00CD4 Platz 20 -> sub03 */
#define RE15_N40_BOMBE_CUT       12

#define RE15_N40_VUS_SLOT        2      /* 1100/1101 sub00 @0x00AD6, msg 1 (nur (3,0x67)=0) */
#define RE15_N40_VUS_MSG         1

#define RE15_N40_PANEL_EREIGNIS  16     /* 11F0/11F1 sub00 @0x00D64 Platz 1 -> sub16 */
#define RE15_N40_PANEL_CUT       12

/* Nach dem Init-Lauf (scd_room_setup.c): die drei Text-Plaetze zu Ereignis 31 umwidmen, sonst nichts. */
void re15_nahaufnahmen_r40_install(uint16_t room_id);

/* scd_event_fire-Kette: Port-Programm fuer (Raum, Ereignis) oder NULL. */
const uint8_t *re15_nahaufnahmen_r40_ereignis(uint16_t room_id, uint8_t event_id);

/* Fuer den Riegel: Programm je Name ("pult", "spind", "bombe", "vus", "panel") und seine Laenge. */
const uint8_t *re15_nahaufnahmen_r40_programm(const char *name, int *out_len);

#endif /* RE15_NAHAUFNAHMEN_R40_H */
