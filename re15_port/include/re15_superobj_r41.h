/*
 * RE1.5 Rebuilt — Runde 41 Spur J: SCD-Opcodes 0x48 Super_set / 0x49 Super_reset (Objekt an ein Gegner-Teil haengen).
 *
 * Nutzer-Punkt (AUFTRAG.md 2026-10-23): "in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht
 * nicht". Zum Hineinkriechen gehoert das Gitter vor der Oeffnung: ROOM3061 sub04 @0x0127A `2e 03 00` (Work_set Objekt 0
 * = Gitter, Obj_model_set @0x00F80 (-19095,-495,-27045) Rot (0,3072,0)), @0x0127E Super_set
 * `48 00 80 8e fc 00 c0 00 67 ff 00 00 00 04 00 0c`, Sleep 26, Se_on, @0x0129E Super_reset `49 00 48 00 00 0c 00 00`,
 * dann Speed_set/Add_speed +x 6x40 + 4x80 + 2x40 = 640 (For-Zaehler @0x012AE/@0x012C0/@0x012CE).
 *
 * Original (PSX.EXE, selbst disassembliert, Dossier §2.2):
 *   Super_set @0x80040b20 (16 Bytes): a2 = Work-Objekt (`lw a2,340(t0)`), Art = pc[2] & 0xc0 (@0x80040b38):
 *     0x00 -> Vater = 0x80072d4c (Welt, @0x80040b78..84)
 *     0x40 -> Spieler: pc[3] & 0x80 ? Part (pc[3]&0x7f) Welt-Matrix (0x800acbdc = Spieler+0x188, +64) : 0x800aca74
 *     0x80 -> Gegner (pc[2]-0x80): pc[3] & 0x80 ? Part (pc[3]&0x7f) Welt-Matrix (@0x80040c18 0x8009d3b4 + pc[2]*0x1F4 =
 *             Gegner+0x188, @0x80040c30 +64) : Gegner+0x20 (@0x80040c48 0x8009d24c + pc[2]*0x1F4)
 *     0xc0 -> Objekt (pc[2]-0xc0) Welt-Matrix (@0x80040c6c 0x800ad0e0 + pc[2]*0x94)
 *     @0x80040c74 `sw v0,116(a2)` Vater-Zeiger +0x74; @0x80040c78..cbc lokal: pc+4/6/8 (s16) -> +0x34/+0x38/+0x3c,
 *     pc+10/12/14 -> Rot +0x68/+0x6a/+0x6c.
 *   Super_reset @0x80040cd0 (8 Bytes): @0x80040cd8..ce0 Welt-Translation +0x5c/+0x60/+0x64 (= Welt-Matrix +0x48 des
 *     Objekts, letztes Zusammensetzen) -> @0x80040cf0..cf8 lokal +0x34/+0x38/+0x3c; @0x80040cec Vater = 0x80072d4c;
 *     @0x80040cfc..d1c Rot = pc+2/4/6.
 *   Objekt-Welt beim Zeichnen = Vater-Matrix x [RotMatrix(+0x68) | +0x34] (0x80022da0).
 * GEMESSEN (Original, Savestate-Reihe J41b/orig/o3061_s2, Objekt-Struktur 0x800b3f98, Stride 0x94, Welt-Matrix +0x48):
 *   vorher lokal = Welt (-19095,-495,-27045) Rot (0,3072,0), Vater 0x80072d4c; t10 (Sherry Clip 0 Bild 78) lokal
 *   (-18356,-266,-26788) Rot (72,3072,0); ab t11 Endlage (-18116,-266,-26788) Rot (72,3072,0) = Loslass-Lage + 640 x.
 * Port: Vater-Art 0x80 mit Part-Bit (Gegner-Teil) und 0x00 (Welt). Die Gegner-Pose kommt aus dem Kanal des State-4-Subs
 * (re15_fussanker_r41_kanal / _teil_matrix, gleiche Rechnung wie das Zusammensetzen). OFFEN (kein Lieferfall): Vater-Art
 * 0x40 (Spieler), 0xc0 (Objekt) und Gegner ausserhalb State 4 (ROOM4030/4031 sub07 `48 00 80 81` = Typ 0x2D Part 1) —
 * dort bleibt das alte Verhalten (Opcode wird uebersprungen, Messzeile [superobj41] ... OFFEN).
 */
#ifndef RE15_SUPEROBJ_R41_H
#define RE15_SUPEROBJ_R41_H

#include <stdint.h>
#include "re15_scd.h"

int  re15_superobj_r41_op_set(scd_thread_t *t);     /* 0x48 @0x80040b20 */
int  re15_superobj_r41_op_reset(scd_thread_t *t);   /* 0x49 @0x80040cd0 */
/* Zeichner (platform/pc/main.c pc_prop_world): Weltmatrix eines angehaengten Props. 1 = angehaengt, r/t gesetzt. */
int  re15_superobj_r41_welt(int prop_slot, int32_t r[9], int32_t t[3]);
void re15_superobj_r41_reset(void);

#endif /* RE15_SUPEROBJ_R41_H */
