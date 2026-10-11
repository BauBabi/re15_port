/* Runde 36 Spur R "dachkiste" — Messschrieb des Kisten-Schubs (RE15_PUSH_LOG=<datei>) und geschlossen geregelter
 * Messlauf (RE15_WEGPLAN=<datei>), siehe engine/src/dachkiste_r36.c. Haken: aot_common.c re15_prop_push_tick, direkt
 * nach FUN_8003b558(obj,2) (@0x8002bfb0/@0x8002bfb8); platform/pc/src/input_pc.c hinter der Skript-Ersetzung.
 * Kein Verhalten, nur Diagnose/Mess-Steuerung. */
#ifndef RE15_DACHKISTE_R36_H
#define RE15_DACHKISTE_R36_H

#include <stdint.h>

void re15_dachkiste_schub_log(int prop_idx, int32_t ziel_x, int32_t ziel_z, int gesperrt);
/* Liefert 1 und setzt *bits, wenn ein Wegplan (RE15_WEGPLAN) die Pad-Bits dieses Bildes bestimmt. */
int  re15_dachkiste_wegplan_pad(uint16_t *bits);

#endif
