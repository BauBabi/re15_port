/*
 * re15_esp_brocken.h — Runde 35 Spur D "redhawk": die Routinen B 36/37 der ESP-Zeilenmaschine
 * (Landung der Fleisch-Brocken, Raum-Effekt-Id 7).
 *
 * Nutzer (AUFTRAG.md Z.17): "Wenn ich mit der Super Redhawk auf die Hunde schiesse bleiben die
 * Fleisch Effekte die sich rausloesen permanent da in loop."
 *
 * Mechanismus (Dossier analysis/befunde_runde35/D_redhawk.md, RE-Belege R3/R4/R6):
 *   Raum-Id 7 (z.B. ROOM1190/11D0/1140, je Sub ein Stream mit 2 Zeilen) fliegt mit Zeile 0
 *   (A 0, B 36, Beschl. (0,10,0), Flags 0x13, row[0x16] = 6, row[0x26] = 1) und zeigt dabei den
 *   Anim-Zyklus 0..4 mit der SCHLEIFENMARKE in Record 5. Nur die Routinen B 36/37 beenden das:
 *     36 @0x800187c4: Boden h = room_coll(slot+0x28/2a/2c, 0, 8, 0x100) (`jal 0x8001c6e8`
 *        @0x800187f8); h < Welt-y -> B := 37 (`sh v1(0x25),2` @0x80018828) und Geschw. slot+0x10/
 *        12/14 := 0 (@0x8001882c-38); sonst slot+0x1e := h (`sh a0,30` @0x80018848).
 *     37 @0x8001885c: slot+0x1e < Welt-y (`slt` @0x80018878) -> Flags slot+0x6c := row[0x0e]
 *        (@0x80018884-8c), Anim-Index slot+0x6e := row[0x16] (@0x8001889c-a4), Zeilen-Vorschub
 *        FUN_800174e4 (@0x800188a0). Anim 6 -> Records 7..9 -> Terminator 0/0 -> Flags := 0
 *        (@0x8001a40c) = Platz frei.
 */
#ifndef RE15_ESP_BROCKEN_H
#define RE15_ESP_BROCKEN_H

#include "re15_esp.h"

/** Routine B 36 bzw. 37 fuer den Platz `f` ausfuehren (Aufrufer: esp_fx_dispatch_b in re15_esp.c,
 *  Hauptlauf-Waehler `lhu v0,2(v0)` / `jalr v0` @0x8001a2b4-d4). Rueckgabe 1, wenn B 36 oder 37 war
 *  (dann ist der Platz behandelt), sonst 0. Die Weltlage f->wpos muss in diesem Takt schon
 *  gerechnet sein (re15_esp.c Stufe (d), Original @0x8001a118-2a4 vor dem B-Waehler). */
int  re15_esp_brocken_b(re15_esp_fx_t *f);

/** FUN_800174e4-Zwilling (Zeilen-Vorschub, definiert in re15_esp.c: Cursor slot+0x6f++ @0x800174f0-fc,
 *  40-Byte-Kopie der Zeile *(slot+0x80) + Cursor*40 nach slot+0x00 @0x8001750c-d8). */
void re15_esp_fx_zeile_weiter(re15_esp_fx_t *f);

/** MESSSCHIENE (kein Verhalten): Zahl der Landungen (36 -> 37) und der Abschluesse (37 -> Vorschub)
 *  seit Programmstart. Fuer die Sonde test_r35_redhawk. */
unsigned re15_esp_brocken_landungen(void);
unsigned re15_esp_brocken_abschluesse(void);

#endif /* RE15_ESP_BROCKEN_H */
