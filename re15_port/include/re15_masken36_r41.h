#ifndef RE15_MASKEN36_R41_H
#define RE15_MASKEN36_R41_H
/*
 * Runde 41 Spur M (masken36): Generator-Masken der Stages 3..6 GESCHLOSSEN - Tiefe aus der Erreichbarkeit.
 * Dossier: analysis/befunde_runde41/M_masken36.md (Punkt 1).
 *
 * Seit Runde 41 zeichnen die 216 Generator-Sektionen (30 MSK-Dateien ROOM3040..ROOM6031) ihre verifizierte Region ganz
 * (re15_port/tools/maske/schliessen_r41.py: alte Rechtecke bytegleich + Saum-Rechtecke, eigener Atlas
 * MASKS/ROOM%04X_PRI%02d.TIM). Ein Rechteck, das an einem erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle
 * davor verdecken wuerde, war bisher AUSGESCHLOSSEN (masken36_r37.c) - das liess Objekte in Splittern stehen (Dossier
 * 1.1: 94 Objekte teilweise ausgeschlossen, 358 358 Bildpunkte). Jetzt bekommt es stattdessen die kleinste Tiefe, die
 * keinen dieser Punkte verdeckt: tiefe = groesster Bucket eines solchen Figurpunkts unter dem Rechteck. Original-Urteil
 * re15_pri_mask_occludes: verdeckt gdw. depth < (1023*vz)>>16 (@0x8002565c otz>>4, ZSF3 = 341 @0x80066c70, Masken-OT
 * = depth @0x80039650..60). depth >= bucket(p) fuer jeden solchen Punkt p => kein solcher Punkt wird verdeckt; jeder
 * Punkt mit groesserem Bucket (weiter hinten) bleibt verdeckt. Gegenueber dem Ausschluss also nie schlechter.
 * Tabelle gen/masken36_tiefe_r41.inc, erzeugt von tests/unit/test_r37_masken36.c ("erzeuge_tiefe"), nachgerechnet vom
 * Riegel unit_r41_masken36_tiefe.
 */
#include "re15_pri.h"

/* Setzt die Tiefe der in der Tabelle stehenden Rechtecke (Index in der Reihenfolge von re15_pri_parse_section, VOR dem
 * Ausschluss). masken_raum = Raum-Id der MSK-Datei (re15_pri_masken_raum). Rueckgabe = Zahl der gesetzten Rechtecke. */
int re15_masken36_tiefe_r41(unsigned masken_raum, int cut, re15_pri_cut_t *pri);
/* Tabellenzugriff fuer den Riegel: Tiefe aus der Tabelle oder -1. */
int re15_masken36_tiefe_r41_wert(unsigned masken_raum, int cut, int idx);
int re15_masken36_tiefe_r41_zeilen(void);

#endif
