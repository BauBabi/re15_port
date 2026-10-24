/*
 * RE1.5 Rebuilt — Runde 41 Spur H "kollision" (Dossier analysis/befunde_runde41/H_kollision.md).
 *
 * Nutzer (AUFTRAG.md, 2026-10-23):
 *   (1) "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
 *   (2) "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"
 *
 * (1) NPC-WURZEL-SCHWANZ. Jede der 24 NPC-Wurzeln (Typ 0x40/0x42/0x45/0x47/0x49/0x4b/0x4d, STAGE1..6, Dispatch-
 *     Eintraege 0x80072bac[typ], selbst gescannt) laeuft nach dem Zustands-`jalr` dieselbe Kette, z.B. Ada STAGE1
 *     Wurzel 0x8011cb70:
 *         8011cc2c  jal 0x8002b498                 Kasten-Versatz drehen, Kontakt loeschen
 *         8011cc3c  jal 0x8002aec4                 FUN_8002aec4(a0 = 0x800aca54 SPIELER, a1 = NPC)
 *         8011cc40  addiu a0,s0,20                 (Delay-Slot: s0 = 0x800aca40 -> a0 = Spieler)
 *         8011cc44  jal 0x8002b544                 NPC aus JEDER anderen aktiven Entitaet schieben
 *         8011cc64  jal 0x8003b0a4                 Wandklemme (der Port: re15_npc_wall_clamp)
 *     FUN_8002aec4 BEWEGT DEN ZWEITEN (re15_body_push, pushee); FUN_8002b544 laeuft ueber das Gegner-Feld
 *     0x800acc2c (Stride 500, g_active_count 0x800aca4e) und ruft fuer jede aktive Entitaet ausser sich selbst
 *     (@0x8002b590 `beq a1,s0`) FUN_8002aec4(andere, NPC) @0x8002b598 — OHNE Typfilter, also auch NPC gegen NPC.
 *     Der Port hatte fuer die NPC-Familie nur die Wandklemme; Spieler- und Gegner-Seite schoben sich zwar aus
 *     den NPCs heraus, aber zwei NPCs (Eskorte Ada + Marvin, ROOM3070) liefen ineinander (gemessen: 23 Einheiten).
 *
 * (2) OBJEKT-GEGEN-OBJEKT-PASS der Schubkette FUN_8002bd44 @0x8002c03c-c0a8: nach angenommenem Zellentest
 *     (@0x8002bfb8) und im Schub-Substate (@0x8002bfe4) prueft FUN_8002cdf4(anderes, geschobenes) jedes andere
 *     aktive Pool-Objekt (Pool 0x800b3f98, Stride 0x94, obj-Nummer 0..nOmodel-1); Ueberlappung -> @0x8002c0b0
 *     Zaehler obj+0x8C = 10 und Ruecksetzung der Lage (@0x8002c0bc-d4) = GESPERRT.
 */
#ifndef RE15_KOLLISION_R41_H
#define RE15_KOLLISION_R41_H

#include <stdint.h>
#include "re15_rdt.h"

/* (1) Haken in re15_npc_ai_tick (enemy_ai_common.c) zwischen Zustands-Dispatch und Wandklemme. */
void re15_kollision_r41_npc_schwanz(int slot);

/* (2) FUN_8002cdf4 fuer EIN Paar (Port-Pool-Plaetze): a = anderes Objekt (t2), b = geschobenes Objekt (a3) mit der
 * Kandidatenlage (bx, bz). alt_ya/alt_yb = Spiegel +0x42 (Vor-y) beider Objekte. Rueckgabe 1 = Ueberlappung; im
 * Zweig "vertikaler Eintritt" (@0x8002cf18-3c) liefert *neu_yb die neue y von b (sonst unveraendert). */
int  re15_kollision_r41_8002cdf4(int a, int b, int32_t bx, int32_t bz, int32_t alt_ya, int32_t alt_yb,
                                 int32_t *neu_yb);

/* (2) Der Durchlauf @0x8002c03c-c0a8 fuer das geschobene Objekt p mit Kandidatenlage (nx, nz). Rueckgabe 1 =
 * GESPERRT (der Aufrufer setzt den Zaehler 10 und verwirft die Lage, @0x8002c0b0-d4). */
int  re15_kollision_r41_kisten_pass(const re15_rdt_t *rdt, int p, int32_t nx, int32_t nz);

/* (2) Spiegel obj+0x42 (y) — Haken in game_step_common.c NACH dem Objekt-Pass des Bildes (s. kollision_r41.c). */
void re15_kollision_r41_objspiegel(void);

#endif /* RE15_KOLLISION_R41_H */
