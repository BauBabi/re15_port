/* =============================================================================================
 * re15_elzaems_r37.h — Runde 37 Spur N "elzaems": das Modellarchiv JEDES Gegner-/NPC-Typs wie im
 * Original — Elzas Szenario laedt aus EMD/CDEMD1.EMS, Leons aus EMD/CDEMD0.EMS, und vier Raumskripte
 * schalten das Archiv fuer EINEN Spawn um (Birkin-Form).
 *
 * ORIGINAL (selbst disassembliert, Dossier analysis/befunde_runde37/N_elzaems.md §1.1/§2):
 *   Modell-Lader FUN_80022300, einziger Aufrufer der Sce_em_set-Handler (`jal 0x80022300` @0x80042328):
 *     8002232c  lw   v1,-13764(v1)   ; DAT_800aca3c
 *     80022334  bltz v1,0x800223ac   ; Bit 31 — fuer JEDEN Typ, kein Typ-Test
 *     8002233c  ori  v0,zero,0x26    ; Bit 31 = 0: Datei 0x26 = CDEMD0.EMS, TOC @0x80072f38
 *     800223ac  ori  v0,zero,0x27    ; Bit 31 = 1: Datei 0x27 = CDEMD1.EMS, TOC @0x80073178
 *   Bit 31 von DAT_800aca3c ist zugleich SCD-Flag (1,0): Set (Opcode 0x22, LAB_8003fdd0) schreibt ueber die
 *   Bank-Tabelle @0x80074664 ([1] = 0x800aca3c) die Maske 0x80000000 >> (bit & 0x1f) (@0x8003fdd4 `lui t0,
 *   0x8000`). Die Figurenwahl spiegelt das Bit (TITLE.BIN Elza @0x801024a8/@0x801024e4, Leon @0x801024ac-
 *   c8); zwoelf Set(1,0,v) in vier Raeumen klammern je einen Birkin-Spawn (n1_bit31_zensus.txt):
 *     Elza ROOM3071 sub00 @0x03430/@0x03448 (+ @0x03476/@0x0348E), ROOM50F1 @0x009A8/@0x009C0 (+ @0x009D0/
 *     @0x009E8): Bit 0 -> Birkin aus CDEMD0;  Leon ROOM5080 @0x00742/@0x0075A, ROOM5090 @0x01246/@0x0125E:
 *     Bit 1 -> Birkin aus CDEMD1. Jede Klammer stellt im selben Abschnitt den Wert der Figur wieder her.
 *   Gemessen am Original (DuckStation-GDB, Blob im RAM nach dem CD-Lesen @0x80022470): Elza 1101 Zombie 0x10
 *   == CDEMD1, Elza 3071/50F1 Birkin == CDEMD0, Elza 5081 und LEON 5080 Birkin == CDEMD1.
 *
 * PORT:
 *   re15_elzabit()                — Zwilling von DAT_800aca3c Bit 31: zuletzt per Set(1,0,v) geschriebener Wert,
 *                                   solange dieselbe Figur gespielt wird; sonst die Figur (re15_char_variant).
 *                                   (Gleichwertig zum Original, weil jede Skript-Klammer den Figurenwert
 *                                   wiederherstellt — Zensus aller 240 RDTs.)
 *   re15_elzabit_skript_set(v)    — Haken in re15_game_flag_set(1, 0, v) (game_state.c).
 *   re15_ems_sce_em_set_merken(t) — Haken in op_sce_em_set (scd_vm.c): das Archiv, aus dem FUN_80022300 den Typ
 *                                   JETZT laden wuerde, je Typ und Raum merken. Der PC-Port laedt die Bank erst
 *                                   spaeter (beim ersten Zeichnen) — bis dahin hat das Skript das Bit schon
 *                                   zurueckgestellt (@0x03448 folgt @0x03434 im selben Lauf).
 *   re15_ems_archiv_fuer_typ(t)   — Archiv fuer die Bank des Typs im aktuellen Raum (gemerkt, sonst aktuell).
 * ============================================================================================= */
#ifndef RE15_ELZAEMS_R37_H
#define RE15_ELZAEMS_R37_H

#include <stdint.h>

int  re15_elzabit(void);
void re15_elzabit_skript_set(int value);
void re15_ems_sce_em_set_merken(uint8_t type);
int  re15_ems_archiv_fuer_typ(uint8_t type);

/* Testhaken: Merkwerte und Skript-Zwilling verwerfen (Unit-Tests; kein Spielpfad ruft das). */
void re15_elzaems_r37_reset(void);

#endif /* RE15_ELZAEMS_R37_H */
