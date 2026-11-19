/*
 * RE1.5 Rebuilt — RE2-Buehne: Bildtakt (VSync) und Bildschirm-Schuetteln der RE2-EXE als gemeinsamer Baustein
 * (Runde 42 Strang finale, Plan S3). Vorher lebten beide nur im Alligator-Modul (gator_re2_r41.c); das Finale braucht dieselben
 * zwei Globalen (Tyrant-Phase 7 @0x80103ad8-0x80103b10). Der Alligator ruft den Baustein unveraendert auf.
 *
 * RE2-Leon-EXE info/re2leon/PSX.EXE (Datei-Offset = Adresse - 0x80010000 + 0x800):
 *   VSync  DAT_800dfc1a: Hauptschleife FUN_8002b968 `lui a0,0x800e / lbu a0,-0x3e6(a0) / jal VSync` @0x8002b990-98;
 *          Raumstart 2 (`li v0,0x2 / sb v0,0x800dfc1a` @0x8003b9a8-b0 = 30 Spielbilder/s).
 *          Alligator R1 P0 = 4 (@0x8010235c-64), R1 P3 = 2 (@0x80102668/@0x8010267c) (CDEMD0 EM23-Overlay).
 *          Tyrant Phase 7 = 5 (`addiu v1,zero,5` @0x80103b00 / `sb v1,-998(at)` @0x80103b08), Phase 9 = 2 (`addiu v0,zero,2`
 *          @0x80103dd4 / `sb v0,-998(at)` @0x80103ddc) (CDEMD0 EM2B-Overlay).
 *   Schuetteln FUN_8002aa28 / FUN_8002aa6c: Modus aus 0x800cfbd8 Bit 8 (Modus 1) bzw. Bit 4 (Modus 2) @0x8002aa34-50, Index
 *          (idx + 1) & 0xf @0x8002aa90-a4, Tabellen @0x8009daf4 (Modus 1) und @0x8009db14 (Modus 2), je 16 Paare (dx, dy).
 *          Tyrant Phase 7: 0x800cfbd8 |= 4 (`ori v0,v0,4 / sw v0,(s1)` @0x80103b0c-10), Phase 8 Bild 16: &= ~4
 *          (`addiu a1,zero,-5 / and / sw` @0x80103d3c-44).
 */
#ifndef RE2_BUEHNE_H
#define RE2_BUEHNE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE2B_VSYNC_SPIEL      2   /* @0x8003b9a8-b0 */
#define RE2B_SCHUETTEL_STARK  8u  /* 0x800cfbd8 Bit 8 -> Modus 1 (@0x8002aa34-50) */
#define RE2B_SCHUETTEL_LEICHT 4u  /* 0x800cfbd8 Bit 4 -> Modus 2 (@0x8002aa34-50) */

/* Raumstart: VSync 2, Schuetteln aus. */
void re2b_reset(void);

/* DAT_800dfc1a */
int  re2b_vsync(void);
void re2b_vsync_setzen(int v);

/* FUN_8002aa28/FUN_8002aa6c je Bild: bits = Schuettel-Bits von 0x800cfbd8 (8 und/oder 4). */
void re2b_schuettel_takt(uint32_t bits);
void re2b_schuettel_offset(int *dx, int *dy);
int  re2b_schuettel_modus(void);

/* Pruefzugriff (Riegel unit_re2_buehne): Tabelle des Modus 1 oder 2, 32 Byte. */
const uint8_t *re2b_schuettel_tabelle(int modus);

#ifdef __cplusplus
}
#endif

#endif /* RE2_BUEHNE_H */
