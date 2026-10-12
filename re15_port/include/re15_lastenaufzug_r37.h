/* re15_lastenaufzug_r37.h — Runde 37 Spur C Nachbesserung 1 (AUFTRAG.md Nachtrag 5a "ja, dann mache den noch" =
 * Tuersequenz beim Aussteigen aus dem Lastenaufzug ROOM3080 "WAREHOUSE LIFT" -> ROOM4000). Dossier
 * analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1".
 *
 * RE1.5 (Bytes selbst gelesen): ROOM3080.RDT main00 @0x0072A Door_aot_set Platz 0, Rechteck (0,0,0,0), Ziel Stage 3
 * Raum 00 Cut 1 (-7600,0,-6760), Archiv-Byte +26 = 0 (DOOR00 = Evt_end, keine Sequenz); gefeuert am Ende der Fahrt
 * per Aot_on 0 @0x00A22 (sub02, erste Fahrt mit Ada, (4,96)=0) bzw. @0x00BFA (sub09). ROOM3081 (Elza) traegt
 * denselben Satz @0x0072A, Aot_on @0x0081E (sub02) / @0x00862 (sub03). Waehrend der Fahrt sind (2,7)/(1,0x1B)
 * gesetzt (sub02 @0x007FE/@0x00802, sub09 @0x00BBA/@0x00BBE) -> player_mode 2.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail): RE2s Lastenaufzug ist die Hubbuehne DOOR2D; ihr einziger Satz
 * ROOM6170 main00 @0x035A2 `3b 01 01 41 .. 2d 05` (Byte 26/27) spielt beim Hinabfahren zum Bahnsteig ROOM7000
 * DOOR2D Variante 5. DOOR2D Skript 0 @Datei 0x601c verteilt V0/V2/V4 -> Skript 1 @0x6050 (Skript 4 @0x6300: obj 2
 * y += 125 je Bild = Schacht faellt, Buehne steigt = HINAUF) und V1/V3/V5 -> Skript 2 @0x6130 (Skript 5 @0x6344:
 * y -= 125 = HINAB, 451 Bilder). ROOM3080 -> ROOM4000 faehrt hinab -> Variante 5. Archiv = das der Gegenrichtung
 * S218 (ROOM4000 -> ROOM3070, Runde 33 G10): Port-Archiv P2DS (DOOR2D ohne Bedientafel/Lampe), RE2: beide Seiten
 * eines Paares tragen dasselbe Archiv (Runde 31 tueren_02_re2.md 2.1, 238/238).
 */
#ifndef RE15_LASTENAUFZUG_R37_H
#define RE15_LASTENAUFZUG_R37_H

#include <stdint.h>

#include "re15_aot.h"
#include "re15_door_seq.h"

/* Ist (room_id, Tuer-Satz) der Ausstieg aus dem Lastenaufzug (ROOM3080/3081, Null-Rechteck, Ziel Stage 3 Raum 00)?
 * Dann die Anfrage (aktiv = 1) nach out und 1, sonst 0. PSX (kein Laeufer) immer 0. */
int re15_lastenaufzug_r37_ausstieg(unsigned room_id, const re15_aot_t *a, const re15_aot_door_params_t *d,
                                   re15_door_seq_anfrage_t *out);

/* Satz-Daten fuer Riegel: Raum ohne Variante, Ziel Stage/Raum, RDT-Datei-Offset des Door_aot_set, Kennung des
 * Port-Archivs, RE2-Basis, Variante. */
void re15_lastenaufzug_r37_satz(uint16_t *raum, uint8_t *ziel_stage, uint8_t *ziel_raum, uint32_t *off,
                                const char **kennung, uint8_t *basis, uint8_t *variante);

#endif
