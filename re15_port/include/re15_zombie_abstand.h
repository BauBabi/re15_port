/*
 * RE1.5 Rebuilt — ZOMBIE-ABSTAND ZUR EINTRITTSTUER (Runde 35 Spur F, Punkt 3).
 * Dossier mit allen Messungen: analysis/befunde_runde35/F_inhalt.md, Punkt 3.
 *
 * NUTZER-VORGABE (Runde 35, AUFTRAG.md Z. 19, woertlich):
 *   "Im ROOM 1010 sind die stehenden Zombies zu nah an der Tuer. Das ist unfair, da man so keine
 *    Chance hat, den Zombies auszuweichen. Bitte setze sie ein Stueck weiter zurueck. In ROOM 1220
 *    teilweise genauso. Die Zombies in den Zellen muessen zumindest so weit weg sein, das man eine
 *    Chance hat aus dem Raum wieder raus zu drehen."
 *
 * FORM (belegt): die Gegner entstehen im Original ueber Sce_em_set (Op 0x44, 20 Byte; x/y/z an
 * pc+8/+10/+12, Richtung pc+16) im Raumskript sub00, je Eintritts-Cut (Switch work_vars[0x0A]).
 * Der Port tauscht NUR x/z der genannten Saetze beim Lesen (scd_vm.c op_sce_em_set) — KEIN
 * RDT-Patch, Typ/Verhalten/Richtung/Kill-Flag bleiben die des Originals.
 * Die neuen Lagen sind PORT-WAHL nach der Nutzer-Vorgabe; ihre Regel und die Messung stehen im
 * Dossier und an der Tabelle in zombie_abstand_r35.c.
 */
#ifndef RE15_ZOMBIE_ABSTAND_H
#define RE15_ZOMBIE_ABSTAND_H

#include <stdint.h>

/* Sce_em_set-Haken: ersetzt x/z, wenn (Raum-Basis, Typ, ORIGINAL-x/z) in der Tabelle steht —
 * die Original-Lage ist Schluessel und Satz-Waechter zugleich (ROOM1011 fuehrt dieselben Saetze
 * mit um 1 verschobener Slot-Nummer; `slot` wird deshalb nicht verglichen). Rueckgabe 1 = ersetzt. */
int re15_zombie_abstand_anwenden(uint16_t room_id, uint8_t slot, uint8_t type,
                                 int16_t *x, int16_t *z);

/* NUR fuer den Riegel (Original-gegen-Port-Messung im selben Prozess): 0 = Tabelle aus.
 * Standard 1. Kein Spielschalter, keine Umgebungsvariable. */
void re15_zombie_abstand_set_aktiv(int on);

/* Pruefhaken: Anzahl der Tabelleneintraege und ein Eintrag (Original- und neue Lage). */
int re15_zombie_abstand_anzahl(void);
int re15_zombie_abstand_eintrag(int i, uint16_t *raum, uint8_t *slot, uint8_t *type,
                                int16_t *x0, int16_t *z0, int16_t *x1, int16_t *z1);

#endif /* RE15_ZOMBIE_ABSTAND_H */
