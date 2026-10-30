/*
 * RE1.5 Rebuilt — Runde 42: erwachsene Spinne 0x25 (STAGE2) an Wand, Absatz und Decke.
 * Code: engine/src/spinne_r42.inc (eingebunden von enemy_ai_common.c). Nur der RE1.5-KI-Pfad
 * (RE15_AI_FLAVOR=re15); unter dem RE2-Gehirn laeuft re15_re2spider_tick mit eigener Oberflaechenschicht.
 *   0x801118a8  Wand-/Kletter-Test + Lauf-Zeitautomat (aus WALK/CHASE/CREEP/13)
 *   ACT 9  0x80112af8  Absatz hoch/runter     ACT 10 0x80112e58  Ausweichen
 *   ACT 12 0x801130a4  Flaechenwechsel        ACT 13 0x80113734  an der Wand
 * Felder ausserhalb des Aktor-Structs (Offset-Schluessel = Original-Offset): 0x1e2, 0x1e8, 0x1ea, 0x1ec, 0x1ee, 0xb0.
 */
#ifndef RE15_SPINNE_R42_H
#define RE15_SPINNE_R42_H

#include <stdint.h>
#include "re15_actor.h"

int16_t re15_spinne42_feld(int slot, int ofs);
void    re15_spinne42_setzen(int slot, int ofs, int16_t v);
void    re15_spinne42_reset(int slot);
/* Root-Schwanz: alte Lage nach dem Absatz, Fusslagen nach der Klemme merken. */
void    re15_spinne42_altlage(re15_actor_t *e, int32_t *ox, int32_t *oz);
void    re15_spinne42_nach_klemme(re15_actor_t *e);

#endif
