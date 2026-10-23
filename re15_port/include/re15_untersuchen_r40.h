/*
 * RE1.5 Rebuilt — Runde 40 "untersuchen1120": drei Untersuchungstexte in ROOM1120/1121, die das ausgelieferte Skript
 * nie zeigt, bekommen ihren Text-Platz.
 *
 * NUTZER-VORGABE (2026-10-23, woertlich): "Also, bei ROOM 1120/1121: 1, 3 und 4 koennen wir gerne zuordnen.
 * 1070/1071 wuerde ich erst einmal so lassen."
 * Herleitung, Bilder und die nicht gewaehlten Vorschlaege: analysis/untersuchungstexte_2026-10-23/README.md.
 *
 * ⛔ PORT-INHALT AUF NUTZERWUNSCH. Im Auslieferungsstand hat KEINER dieser Texte einen Platz:
 *   - ROOM1120.RDT main00 legt nur die Plaetze 0..3 an (Tueren 0/1/2 @0x00C76/@0x00CB6/@0x00CD8, Text-Platz 1
 *     @0x00C9E msg 0, Telefon Platz 3 @0x00CF8 msg 6); sub00/sub01 enthalten kein Aot_set.
 *   - Rohsuche `2c ?? 01 .. .. 00 <8 B> <msg> ff ff` ausserhalb der gewalkten Befehle: in STAGE1 kein Treffer.
 *   - ROOM1121.RDT: main00/sub00/sub01 byte-gleich zu ROOM1120 (md5 je Region), Texttabelle gleich.
 * Die Texte selbst sind ORIGINAL (RDT-Texttabelle msg 1/3/4); der Port legt nur den Platz an.
 *
 * SATZFORM = der Original-Satz des Telefons ROOM1120 main00 @0x00CF8
 *   `2c 03 01 31 00 00 b2 0c 9e ee e8 03 e8 03 06 00 ff ff 00 00`
 *   (sce 1, sat 0x31 = Vorwaertspunkt-Test FUN_80042bac, Band 0, Pausemaske 0xffff); getauscht sind nur Platz,
 *   Rechteck und msg. Reichweite: Punkt 620 vor dem Spieler (ori 0x26c @0x80042bd0) muss im Rechteck liegen, daher
 *   reicht jedes Rechteck etwa 500 ueber die Wand (wie ROOM1070 m4 @0x014BA ueber die Ostwand).
 *
 * ORTE (Rueckprojektion der Kamerabilder, Kamera = RID-Satz Cut n, tools/maske/geom.py; Wandebenen aus der SCA):
 *   msg 1 "A painting, titled: The Music Lesson"  — Bild Suedwand (Zelle 1 ab z 1650), Cut 2 px 2..38 -> x -197..943;
 *         bunte Figurengruppe in Rot/Gruen (Matisse, "The Music Lesson" 1917)        -> (-200, 1150, 1150, 1000)
 *   msg 3 "Only the lights of the buildings are visible." — Jalousiefenster Ostwand (Zelle 3 ab x 6650), Cut 2
 *         px 86..165 -> z -1037..3839, zwischen den Lamellen erleuchtete Fenster     -> (6150, -1050, 1000, 4900)
 *   msg 4 "Various reports and case information. Nothing useful." — Schrank mit Ordnern, Zelle 10
 *         (x 5450..6650, z -5800..-3950), Front nach Westen (Cut 4)                     -> (4600, -5800, 1350, 1850)
 *
 * PLAETZE 5..7: belegt sind 0..3 (main00), 4 (Fenster-Ereignis ROOM1120 re15_fenster1120.h / Hund ROOM1121
 * re15_hund1121_r39.h) und die 13 RVD-Kamerazonen von oben (rdt_common.c, ab 63 abwaerts = 51..63).
 * Kein Rechteck ueberschneidet die Fenster-Zone x 3500..6650 / z 4300..6400 (re15_fenster1120.h).
 */
#ifndef RE15_UNTERSUCHEN_R40_H
#define RE15_UNTERSUCHEN_R40_H

#include <stdint.h>

#define RE15_U40_RAUM_LEON      0x1120
#define RE15_U40_RAUM_ELZA      0x1121

#define RE15_U40_SLOT_BILD      5
#define RE15_U40_SLOT_FENSTER   6
#define RE15_U40_SLOT_AKTEN     7

#define RE15_U40_MSG_BILD       1   /* RDT-Texttabelle ROOM1120/1121 msg 1 */
#define RE15_U40_MSG_FENSTER    3   /* msg 3 */
#define RE15_U40_MSG_AKTEN      4   /* msg 4 */

#define RE15_U40_SAT            0x31     /* = @0x00CF8 pc[3] */
#define RE15_U40_PAUSE          0xFFFFu  /* = @0x00CF8 pc[16..17] */

/* Die drei Saetze (Form @0x00CF8), fuer Riegel und Install gemeinsam. */
extern const uint8_t re15_untersuchen_r40_saetze[3][20];

/* Nach dem Init-Lauf (scd_room_setup.c): in ROOM1120/1121 die drei Text-Plaetze anlegen, sonst nichts. */
void re15_untersuchen_r40_install(uint16_t room_id);

#endif /* RE15_UNTERSUCHEN_R40_H */
