/*
 * RE1.5 Rebuilt — Runde 38 Spur R "spinnen": Stage-2-Spinnen (Typ 0x25 Adult, 0x26 Baby).
 *
 * Nutzer-Punkte (analysis/befunde_runde38/AUFTRAG.md Z. 68-70, Dossier
 * analysis/befunde_runde38/R_spinnen.md):
 *   1. Spinnen schweben in der Luft und kommen teilweise nicht aus den Schaechten raus.
 *   2. Spinnen verschiessen kein Gift (Vergiftung -> Gegengift).
 *   3. Minispinnen nach dem Tod schweben und greifen nicht an.
 *
 * Belege (Adressen/Bytes) im Dossier und an den Definitionen in engine/src/spinnen_r38.c.
 */
#ifndef RE15_SPINNEN_R38_H
#define RE15_SPINNEN_R38_H

#include <stdint.h>

/* MESSSCHIENE (env RE15_SPINNEN_LOG=<datei>, kein Spielverhalten): eine Zeile je Bild und
 * Spinne (Typ 0x25/0x26) mit Lage X/Y/Z, Zustandsbytes, Modus und Boden. Gerufen einmal je
 * Spielbild aus platform/pc/main.c (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_log_frame(unsigned frame);

#endif /* RE15_SPINNEN_R38_H */
