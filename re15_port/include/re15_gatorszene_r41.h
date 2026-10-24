/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene": ROOM2090/2091 ohne Spinnen, Ankunftsszene mit dem Alligator,
 * Schluessel bei der Leiche. Dossier: analysis/befunde_runde41/F_gatorszene.md. Code: engine/src/gatorszene_r41.c.
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23, woertlich):
 *   "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."
 *
 * ---- 1. SPINNEN ---------------------------------------------------------------------------------------------------
 * Das RDT stellt sie in main00 auf (ROOM2090.RDT und ROOM2091.RDT bytegleich):
 *   @0x00AB0 `44 00 25 00 00 00 00 73 9c ff 00 00 98 df ..`  Slot 0, Typ 0x25, Kill-Flag 0x73, (-100, 0, -8296)
 *   @0x00AC4 `44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2 ..`  Slot 1, Typ 0x25, Kill-Flag 0x74, (-2300, 0, -23896)
 * FORM (belegt): das Original verwirft einen Sce_em_set-Satz ueber das Kill-Flag-Tor FUN_800420a0 @0x80042120-38
 * (Satz erscheint nicht, `ori v0,zero,0x8000` / `sw v0,0(s0)` @0x80042230-38 = "Record verworfen"). Der Port verwirft
 * GENAU diese beiden Saetze beim Lesen (scd_vm.c op_sce_em_set, Haken neben re15_zombie_abstand_anwenden) — KEIN
 * RDT-Patch, kein Kill-Flag gesetzt (die Bits 0x73/0x74 bleiben unberuehrt). Schluessel = Raum-Basis + Typ +
 * ORIGINAL-Lage (Satz-Waechter wie zombie_abstand_r35.c), der Slot wird nicht verglichen. */
#ifndef RE15_GATORSZENE_R41_H
#define RE15_GATORSZENE_R41_H

#include <stdint.h>

#define RE15_GSZ_RAUM            0x2090u   /* Leon; Elza = 0x2091 (Raum-Basis & 0xFFFE)                     */
#define RE15_GSZ_SPINNE_TYP      0x25u     /* Sce_em_set pc[2] @0x00AB2 / @0x00AC6                          */
#define RE15_GSZ_SPINNE0_X       (-100)    /* @0x00AB8 `9c ff`                                              */
#define RE15_GSZ_SPINNE0_Z       (-8296)   /* @0x00ABC `98 df`                                              */
#define RE15_GSZ_SPINNE1_X       (-2300)   /* @0x00ACC `04 f7`                                              */
#define RE15_GSZ_SPINNE1_Z       (-23896)  /* @0x00AD0 `a8 a2`                                              */

/* HAKEN scd_vm.c op_sce_em_set: 1 = diesen Satz verwerfen (Spinne 0x25 in 2090/2091 an einer der zwei RDT-Lagen). */
int  re15_gatorszene_r41_spawn_weg(uint16_t room_id, uint8_t type, int16_t x, int16_t z);
/* NUR fuer Riegel, die die Original-Spinnen von 2090 als Messobjekt brauchen (Wasser-Kanal, Wandklemme): 0 = aus.
 * Standard 1. Kein Spielschalter, keine Umgebungsvariable. Rueckgabe = alter Wert. */
int  re15_gatorszene_r41_spinnen_weg_setzen(int an);
/* Pruefhaken: Zahl der verworfenen Saetze seit Programmstart. */
int  re15_gatorszene_r41_spinnen_verworfen(void);

#endif /* RE15_GATORSZENE_R41_H */
