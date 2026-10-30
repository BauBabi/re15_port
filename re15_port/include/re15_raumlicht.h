/*
 * RE1.5 Rebuilt — Raumlicht: Lichtscheine am Boden nach dem 80%-Video (analysis/video_80prozent/BEFUND.md).
 *
 * ⛔ PORT-ERGAENZUNG, nicht byte-true. Weder die Auslieferungs-RDTs noch das ESP der Raeume tragen diese
 * Lichter (ROOM50B0 setzt kein einziges Sce_espr_on). Positionen per Rueckprojektion der Video-1-Bilder
 * durch die RID-Kamera des Cuts auf den Boden (y aus den Sce_em_set-Spawns des Raums), Farben gemessen
 * (Rot-Ueberschuss gegen unseren Hintergrund), Groesse/Blinken Gestaltung.
 *   ROOM50B0/50B1 Cut 2 (V1 3:58-4:01): rot-rosa Schein am Boden vor der Tankreihe ("Container").
 *   ROOM2000/2001 Cut 2 (V1 2:06-2:15): weisse Glanzpunkte, die einzeln auf dem Kanalwasser aufblitzen.
 * RE15_KEIN_RAUMLICHT=1 schaltet alles ab.
 */
#ifndef RE15_RAUMLICHT_H
#define RE15_RAUMLICHT_H

#include <stdint.h>

typedef struct {
    int32_t  x, y, z;          /* Mitte am Boden (Weltkoordinaten) */
    int32_t  hx, hz;           /* halbe Ausdehnung in X/Z */
    uint8_t  r, g, b;          /* additiver Beitrag in der Mitte (Blob faellt zum Rand auf 0) */
} re15_raumlicht_t;

/* Die in diesem Bild sichtbaren Lichter des Raums `room` (Bildzaehler `frame` fuer das Blinken).
 * Rueckgabe = Anzahl (<= max). */
int re15_raumlicht_liste(unsigned room, uint32_t frame, re15_raumlicht_t *out, int max);

#endif
