/*
 * RE1.5 Rebuilt — Hunde-Schatten auf die BODEN-Referenz (Runde 35 Spur H, Punkt 1).
 *
 * Nutzerbefund (AUFTRAG.md Runde 35): "In ROOM 1190 haben die Hunde einen Schatten waehrend sie durch
 * die Luke springen in der Luft. Das ist quatsch und muss waehrend des Springens raus."
 * Dossier: analysis/befunde_runde35/H_raeume.md, Punkt 1.
 *
 * GEMESSEN (Basisstand 154a73c1, ROOM1190 sub10, RE15_FRAMEDUMP F184..F216): der Schatten-Quad haengt
 * unter dem springenden Hund an der Wand. Ursache: der Port legte den NPC-Schatten auf npc->y (Koerper).
 *
 * BELEG — beide Originale legen den Hunde-Schatten auf die BODEN-Referenz, nie auf den Koerper:
 *   RE1.5 Hunde-Root FUN_8010d7f8 (STAGE1.BIN), Schwanz, jedes Bild:
 *       8010d91c  lh   a1,442(v0)      ; a1 = +0x1ba (Boden-Referenz)
 *       8010d920  jal  0x8001b064      ; Schatten-Quad
 *       8010d924  addiu a0,a0,176      ; a0 = +0xb0 (Schatten-Block)
 *     FUN_8001b064 (PSX.EXE): `local_88.t[1] = (long)param_2;` = Quad-Hoehe = a1.
 *   RE1.5 Sprung-Maschine FUN_80111398 Fall 1 (Absprung, ab Bild 0xD `sltiu v0,v0,0xd` @0x801114a8):
 *       801114dc  addiu v0,v0,-20 / 801114e0 sw v0,56(v1)   ; Koerper +0x38 steigt um 20
 *       801114f0  sh   zero,442(v0)                          ; Boden-Referenz +0x1ba := 0 (Raumboden)
 *   RE2 (Gegenprobe) Entity-Schleife PSX.EXE: 80026874 `sh v0,450(s0)` (+0x1C2 = Boden-Sonde 0x8004fba0),
 *       800268c8 `lh a1,450(s0)` -> 800268d8 `jal 0x800168b4` -> rec+0x0A (Quad-Y) = +0x1C2.
 *   Der Port fuehrt beide Felder als dog_floor_y (re15_actor.h: "+0x1ba ... +0x1C2-Analog").
 *
 * Damit liegt der Schatten waehrend des Sprungs am Raumboden (y = 0) unter dem Hund — nicht mehr in
 * der Luft. Kein Original blendet ihn beim Sprung aus (RE2-Fenstersprung 0x80102C78: kein 0x400-Bit,
 * Dossier 1.2); unter Cut 11 (Kamera blickt zur Luke hinauf) liegt der Raumboden ausserhalb des Bildes.
 */
#ifndef RE15_HUNDESCHATTEN_H
#define RE15_HUNDESCHATTEN_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_HUNDESCHATTEN_TYP  0x20u   /* Cerberus — Typ-Byte +0x8 (Sce_em_set ROOM1190 sub13 `44 00 20 ..`) */

/* Quad-Hoehe des Hunde-Schattens = Boden-Referenz (+0x1ba / RE2 +0x1C2), s. o.
 * Fuer andere Typen: e->y (unveraendertes Port-Verhalten). */
int32_t re15_hundeschatten_y(const re15_actor_t *e);

#endif /* RE15_HUNDESCHATTEN_H */
