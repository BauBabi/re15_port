/*
 * RE1.5 Rebuilt — SYMMETRISCHE GRIFFE AN DEN PORT-DOPPELTUEREN (Runde 35 Spur F, Punkt 1).
 * Dossier mit allen Messungen: analysis/befunde_runde35/F_inhalt.md, Punkt 1.
 *
 * NUTZER-BEFUND (AUFTRAG.md Z. 16, woertlich): "Einige Doppeltüren von den Türsequenzen die wir
 *   erstellt haben, haben unsymetrischen aufbaue, zum Beispiel unsymmetrische Türgriffe. Das ist so
 *   im allgemeinen nicht."
 *
 * MESSUNG (probe_r35_inhalt_tueren, Tuer-Maschine re15_door_seq_* = Door_init FUN_80013c1c /
 * Door_move FUN_80013eb4, jede Wahl der Tuer-Tabelle, Bild 2): je Doppeltuer Spitze bzw.
 * Huellen-Mitte jedes Griffs relativ zum Anhaengepunkt, Spiegelebene = Mittelebene der Angeln.
 *   RE2-Originale symmetrisch: DOOR1B V2 2,7 Grad, DOOR1D V3 0,0 Grad, DOOR04 V2 (P04B) 0,2 Grad.
 *   UNSYMMETRISCH (alle Port-Archive): P1DG/P1DK/P1DL (DOOR1D V3, Griff-Tausch <- DOOR07) 49,8 Grad,
 *   P1BD (DOOR1B V2, Griff-Tausch <- DOOR23) 37,1 Grad, P0CD (DOOR0C V0, Archiv-Griff) 180 Grad.
 *
 * URSACHE 1 (Griff-Tausch, door_scene_pc.c griff_zeichnen): die Archive spiegeln den Griff des
 *   ZWEITEN Fluegels ueber die z-Drehung seines Door_model_set (pc+16..21 -> rot0): DOOR1D V3
 *   obj 2 (62708,2048,0) / obj 3 (62708,2048,2048), DOOR1B obj 4 (63488,2048,0) / obj 5
 *   (63488,2048,2048), DOOR0C obj 2 (0,0,0) / obj 3 (0,0,2048). Der Tausch setzte die Grund-Drehung
 *   des Spenders (rot_vorn / rot_hinten) und verwarf diese Spiegel-Drehung -> beide Fluegel trugen
 *   denselben Griff. Behebung: rot[2] = Grund-Drehung[2] + rot0[2] des Archiv-Objekts (bei allen
 *   Eintueren und am ersten Fluegel ist rot0[2] = 0 -> dort unveraendert; gemessen: die getauschten
 *   Griffe mit rot0[2] = 2048 sitzen ausschliesslich am zweiten Fluegel). Danach 0,0 Grad.
 * URSACHE 2 (P0CD, DOOR0C ohne Tausch): DOOR0C traegt senkrechte Stangen, die NICHT um den
 *   Anhaengepunkt liegen (Huellen-Mitte 344 neben dem Punkt); die z-Spiegeldrehung 2048 des zweiten
 *   Fluegels klappt die Stange an die andere Seite des Punkts (oben statt unten). Fuer das
 *   Port-Archiv P0CD (Runde 33, "die wir erstellt haben") zeichnet der Port die Griffe des zweiten
 *   Fluegels deshalb mit rot[2] + 2048 (= ohne Spiegeldrehung) — gemessen 0,0 Grad. ⛔ PORT-WAHL
 *   (NUTZER-VORGABE "Das ist so im allgemeinen nicht"); das RE2-Archiv DOOR0C selbst bleibt
 *   unveraendert, die Korrektur gilt nur fuer die Port-Archive dieser Tabelle.
 * NACHBESSERUNG 1 (Abnahme 0, Mangel M3): die Messung oben paarte Griffe nach dem Vorzeichen der
 *   LOKALEN pos[0] — am zweiten Fluegel (um die Senkrechte gedreht) zeigt lokales +x aber auf die
 *   andere Seite; gemessen wurden also Vorderseite gegen Rueckseite. Neu: Paarung nach der
 *   KAMERASEITE (Huelle ragt vor/hinter die Blattmitte, Kameraraum der Tuerszene) plus Bildmass
 *   (Huellen in Pixeln, H 290). Ergebnis: Ursache 1 gilt nur fuer Spender DOOR07 (DOOR23 wurde
 *   dadurch schief: 37,1 Grad / 3,27 px), P0CD braucht dy 2048 + dz 2048 (dz allein: 33,9 Grad),
 *   und DOOR1D V3 obj 3 wandert fuer den getauschten Druecker auf die Vorderseite (1,28 -> 0,17 px).
 */
#ifndef RE15_TUER_SPIEGEL_H
#define RE15_TUER_SPIEGEL_H

#include <stdint.h>

/* Zusatz-Drehung (z bzw. y) fuer die Griffe des Fluegels `fluegel` (Objektindex des
 * Eltern-Fluegels) im Port-Archiv `kennung` (re15_tuer_eigen_t.kennung, NULL = RE2-Archiv) —
 * 0 = unveraendert. */
uint16_t re15_tuer_spiegel_dz(const char *kennung, int fluegel);
uint16_t re15_tuer_spiegel_dy(const char *kennung, int fluegel);

/* Drehung eines GETAUSCHTEN Griffs (Ursache 1): Grund-Drehung des Spenders + Ausschlag (x) +
 * Spiegel-Drehung des Archiv-Objekts (z, nur fuer Spender DOOR07 — Nachbesserung 1).
 * basis = rot_vorn/rot_hinten des Tausch-Satzes, spender = re15_griff_tausch_t.spender. */
void re15_tuer_griff_tausch_rot(const uint16_t basis[3], int32_t ausschlag_x, uint16_t rot0_z, int spender,
                                uint16_t out[3]);

/* Versatz von pos[0] fuer einen GETAUSCHTEN Griff (Nachbesserung 1, M3b): DOOR1D V3 obj 3 auf
 * die Vorderseite des zweiten Fluegels. 0 = unveraendert. */
int32_t re15_tuer_griff_tausch_dx(int spender, int fluegel, int32_t pos0, uint16_t rot0_z);

#endif /* RE15_TUER_SPIEGEL_H */
