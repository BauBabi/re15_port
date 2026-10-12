/* re15_fahrstuhl_r37.h — Runde 37 Spur C "fahrstuhl": Tuersequenz beim AUSSTEIGEN aus den Fahrstuhlkabinen
 * ROOM1080/1081 (Revier) und ROOM4020/4021 (Labor). Dossier analysis/befunde_runde37/C_fahrstuhl.md.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail): RE1.5 laesst den Spieler nach der Fahrt per Plc_dest zur Kabinentuer gehen
 * und feuert dann die Null-Rechteck-Tuer per Aot_on (ROOM1080 sub08 @0x007F8 Plc_dest, @0x0080E Aot_on 1; Aot_on
 * @0x800407bc -> sce-2-Handler per jalr @0x8004082c), die Tuermaschine laeuft dort mit DOOR00 = Evt_end. RE2 Retail
 * spielt an derselben Stelle die Fahrstuhltuer: ROOM6010 sub02 @0x00740 Aot_reset aot 0 sce 1 sat 0x21 (Nutzlast
 * +12/+13 = 27 00 aus main00 @0x004C2), @0x0075A Plc_dest in das Tuerrechteck -> DOOR27 V0.
 */
#ifndef RE15_FAHRSTUHL_R37_H
#define RE15_FAHRSTUHL_R37_H

#include <stdint.h>

#include "re15_aot.h"
#include "re15_door_seq.h"

/* Ist (room_id, Tuer-Satz) ein Kabinen-Ausstieg (Null-Rechteck, Ziel eine der drei Etagen)? Dann die Anfrage
 * (aktiv = 1) nach out und 1, sonst 0. Ohne Tabellen (PSX: kein Laeufer) immer 0. */
int re15_fahrstuhl_r37_ausstieg(unsigned room_id, const re15_aot_t *a, const re15_aot_door_params_t *d,
                                re15_door_seq_anfrage_t *out);

/* Anzahl der Ausstiegs-Saetze und Satz i (Tests/Pruefhaken): Raum ohne Spielervariante, Ziel Stage/Raum,
 * Seite/Tuer (Runde 31), RDT-Datei-Offset des Door_aot_set, Archiv-Kennung (NULL = RE2-Datei DOOR27). */
int re15_fahrstuhl_r37_anzahl(void);
int re15_fahrstuhl_r37_satz(int i, uint16_t *raum, uint8_t *ziel_stage, uint8_t *ziel_raum, uint16_t *seite,
                            uint16_t *tuer, uint32_t *off, const char **kennung);

#endif
