/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 1: Leons Kurbel-Animation in ROOM2020.
 *
 * NUTZER: "Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die Kurbel nutzt."
 * GEMESSEN (Dossier E_kleinkram_b.md, Laeufe w1/w5/w9a/w9c/f1/f2, RE15_ANIM_TRACE): ROOM2020 sub02 @0x00A9C
 *   `3f 00 00 00` Plc_motion(0,0,0) -> Spieler-Clip 0 der Raum-RBJ (ROOM2020 RBJ @0xBB0, rec0 Marker 1 = Spieler,
 *   40 Bilder, Wurzel (0,-1704,0) ohne Bewegung) und direkt danach @0x00AA0 `43 00 04 00` Plc_flg(0,4) = +0x1c4 |= 4
 *   (SCHLEIFE). Der Port spielte den Clip EINMAL (F443..F482) und hielt dann 138 Bilder lang Bild 39, waehrend die
 *   Buehne faehrt (bis Plc_ret @0x00B16 F621): Trace `cur` 40, 41, ... bei `slot` 39 (Klemmung des Zeichners), weil
 *   der Vorschub des Spielers (player_common.c `p->anim_frame++`) das Clip-Ende nicht kennt (re15_actor_clip_len
 *   liefert fuer den Spieler 0: keine Gegner-Bank).
 * ORIGINAL (selbst disassembliert, ghidra1_V2.txt): das Kommandowort 4 des Spielers (Plc_motion @0x80041bb0
 *   `sb v1(=4),4(v0)`) fuehrt den Dispatcher FUN_80031c44 auf FUN_80030660; dort
 *     @0x8003068c lbu v0,DAT_800aca59      ; Spieler +0x5 (= Plc_motion pc[1], fuer die Kurbel 0)
 *     @0x8003069c addiu at,at,0x3e30 / @0x800306a4 lw v0,0(at)=>PTR_LAB_80073e30 = 0x80050cb8 / @0x800306ac jalr v0
 *   = dieselbe Bewegungs-Phasen-FSM wie beim NPC-Executor (enemy_ai_common.c re15_npc_sub_motion, byte-true):
 *   Phase 1 spielt den Clip ueber FUN_8001f314 (posiert fc-1, wrappt +0x95 auf 0 @0x8001f63c, Rueckgabe 1), dann
 *   @0x80050da4 Phase 2 = HALTEN — ausser +0x1c4 & 0x04 @0x80050dbc-c8: Phase bleibt 1, der naechste Takt posiert
 *   Bild 0 = SCHLEIFE. Mit Plc_flg(0,4) kurbelt Leon im Original also durchgehend, bis Plc_ret die Steuerung
 *   zurueckgibt.
 * UMSETZUNG: der Spieler-Vorschub bekommt dasselbe Clip-Ende: Laenge des gerade posierten Clips (Raum-RBJ-Record
 *   mit Marker-Bit 0 = Spieler, sonst PL00 — dieselbe Bankwahl wie anim_select_common.c: entity!=0 -> PL00), am
 *   Ende +0x1c4&4 -> Bild 0 (Schleife), sonst fc-1 (Halten, wie bisher gezeichnet). Nur im Kommandowort 4.
 *   Das "Fliegen" selbst liess sich in sieben Laeufen (Leon/Elza, Aktionstaste/FIRE_AOT, Waffe, Nah-/Fernstart)
 *   nicht reproduzieren — OFFEN, s. Dossier.
 */
#ifndef RE15_PLCLOOP_R41_H
#define RE15_PLCLOOP_R41_H

#include <stdint.h>
#include "re15_actor.h"

/* Bilderzahl des Clips, den der Spieler in Kommandowort 4 gerade spielt (0 = unbekannt: dann kein Eingriff). */
int re15_plcloop_r41_clip_len(const re15_actor_t *p);

/* Nach dem Vorschub `anim_frame++` im Kommandowort 4 (player_common.c): Clip-Ende wie 0x80050cb8 Phase 1 —
 * +0x1c4 & 4 -> Bild 0 (@0x80050dbc-c8, @0x8001f63c), sonst fc-1 (Halten @0x80050da4). Sonst nichts. */
void re15_plcloop_r41_takt(re15_actor_t *p);

/* Pruefhaken: Zahl der Umlaeufe (Schleifen) seit Start, Zahl der Halte. */
unsigned re15_plcloop_r41_umlaeufe(void);
unsigned re15_plcloop_r41_halte(void);

#endif /* RE15_PLCLOOP_R41_H */
