/* ============================================================================================
 * re15_rumble.h — die zwei RE2-Rumble-Ringe (DualShock-Aktuatoren), byte-true
 * --------------------------------------------------------------------------------------------
 * BELEG (RE2-Retail `info/re2leon/PSX.EXE`, in Runde 18 selbst disassembliert):
 *
 *   Ring A @0x800EAAD8  (Alloc `FUN_8003947C` @0x8003947c)  -> DAT_800CBC20 = Aktuator 0
 *   Ring B @0x800EAC28  (Alloc `FUN_80039514` @0x80039514,
 *                               `FUN_800395B8` @0x800395b8) -> DAT_800CBC21 = Aktuator 1
 *
 *   Beide Ringe: 0x20 = 32 Slots, STRIDE 10 Byte
 *     `sltiu v0,v1,0x20` @0x800397d0 / @0x80039804 / @0x8003979c
 *     `addiu a0,a0,10`   @0x800397d8 / @0x8003980c / @0x800397a4
 *
 *   Slot-Layout (10 Byte), aus `FUN_800396FC` @0x800396fc (a1 = slot+1):
 *     +0  u8   belegt          `lbu v0,0(a0)`  @0x8003970c
 *     +1  u8   Ausgabewert     `sb v1,0(a1)`   @0x80039778
 *     +2  u16  Vorlauf-Delay   `lhu v0,1(a1)`  @0x8003971c
 *     +4  s16  Restdauer       `lhu v0,3(a1)`  @0x80039734
 *     +6  s16  Schritt (9.7)   `lh  v0,5(a1)`  @0x80039754
 *     +8  u16  Akkumulator     `lhu v0,7(a1)`  @0x80039764
 *
 *   Konsument: Pad-Handler `FUN_80038BBC`. Er tickt BEIDE Ringe und legt die Maxima in die
 *   Aktuator-Tabelle:
 *     `addiu a0,a0,-21800` @0x80038da0  (Ring A) -> `sb v0,0(s0)` @0x80038db8
 *     `addiu a0,a0,-21464` @0x80038db0  (Ring B) -> `sb v0,1(s0)` @0x80038dc0
 *     `addiu a1,a1,-0x43e0`@0x80038c8c  (= &DAT_800CBC20)
 *     `jal 0x800960e4`     @0x80038c90  = PadSetAct(port 0, Tabelle, len 2 @0x80038c94)
 *   Align-Tabelle @0x8009DBA4 = `00 01 ff ff ff ff` (`jal 0x80096064` = PadSetActAlign
 *   @0x80038cc8) — Aktuator 0 -> Byte 0, Aktuator 1 -> Byte 1.
 *
 *   ⛔ Das ist AUSDRUECKLICH **Controller-Vibration**, KEIN Kamera-Shake: alle 11 Xrefs auf
 *   DAT_800CBC20 sind Schreibzugriffe plus das eine `addiu` fuer PadSetAct; es gibt keinen
 *   Kamera-, Viewport- oder SPU-Leser. (Die Dossiers bis Runde 17 fuehrten die Kaskaden als
 *   "Rumble/Shake" — der Shake-Anteil existiert im Original nicht.)
 *
 *   Ohne Analogmodus (`PadInfoMode(0,2,0)==0`, Gate @0x80038d2c) wird pauschal voll gefahren:
 *     `addiu v0,zero,64`  @0x80038e60  -> Aktuator 0 = 0x40
 *     `addiu v0,zero,255` @0x80038e68  -> Aktuator 1 = 0xFF
 * ========================================================================================== */
#ifndef RE15_RUMBLE_H
#define RE15_RUMBLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* `FUN_80039694` @0x80039694 — beide Ringe freigeben (`sb zero` je Slot, Schrittweite 10
 * @0x800396c0). Der Reset raeumt 0x21 = 33 Slots (`sltiu v0,a0,0x21` @0x800396b8), also 10 Byte
 * ueber das Ringende hinaus; hier bewusst auf die echten 32 begrenzt, weil der Ueberlauf im
 * Original in eine Luecke faellt (Ring A endet 0x800EAC17, Ring B beginnt 0x800EAC28). */
void re15_rumble_reset(void);

/* `FUN_8003947C` @0x8003947c — kleiner Motor (Aktuator 0), AN/AUS.
 *   Slot[1] = (dauer != 0) = 1  (`sb v0,1(v1)`, Schritt bleibt 0 -> der Tick ueberschreibt nie)
 * dauer = Frames, delay = Vorlauf-Frames (Slot[2..3]). */
void re15_rumble_small(int dauer, int delay);

/* `FUN_80039514` @0x80039514 — grosser Motor (Aktuator 1), KONSTANTE Staerke.
 *   Slot[1] = amp (0..255), Schritt 0. */
void re15_rumble_large(int dauer, int amp, int delay);

/* `FUN_800395B8` @0x800395b8 — grosser Motor, lineare RAMPE von `von` nach `bis`.
 *   Slot[8..9] = von << 7, Slot[6..7] = ((bis - von) * 0x80) / dauer   (9.7-Fixkomma)
 * Slot[1] wird NICHT gesetzt — der Tick schreibt ihn beim ersten nicht-verzoegerten Bild. */
void re15_rumble_ramp(int dauer, int von, int bis, int delay);

/* `FUN_800396FC` @0x800396fc, zweimal aufgerufen aus `FUN_80038BBC` @0x80038da4/@0x80038db4.
 * Tickt beide Ringe EIN Bild und liefert die Aktuator-Bytes. Beide Zeiger duerfen 0 sein. */
void re15_rumble_tick(uint8_t *aktuator0_out, uint8_t *aktuator1_out);

/* Diagnose fuer die Sonden: Anzahl belegter Slots je Ring (`FUN_800397E4` @0x800397e4). */
int re15_rumble_belegt(int ring);

#ifdef __cplusplus
}
#endif

#endif /* RE15_RUMBLE_H */
