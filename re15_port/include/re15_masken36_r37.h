#ifndef RE15_MASKEN36_R37_H
#define RE15_MASKEN36_R37_H
/*
 * Runde 37 Spur P (masken36, Orchestrator-Auftrag P, Regression 53741fe5): WELCHER ATLAS GEHOERT ZU DEN MASKEN EINES CUTS.
 * Dossier: analysis/befunde_runde37/P_masken36.md.
 *
 * ORIGINAL (PSX.EXE): es gibt genau EINEN Masken-Atlas je Cut, den Vordergrund-Atlas aus dem BSS-Block.
 *   - Der BSS-Lader entpackt den SLD-Block (@0x80021db8 jal 0x800c47e8) und laedt das TIM an eine feste VRAM-Stelle:
 *     @0x80021de8 `ori v0,zero,0x15` -> 0x800aca4c, @0x80021dec `jal 0x8004ee78`; dort @0x8004eea8 `sll a0,v1,6`,
 *     @0x8004eeb8 `addiu a0,a0,-1024` (x = 320), @0x8004eed0 `sll v0,v0,8` (y = 256), @0x8004eee0 `jal LoadImage`.
 *   - Jede Maske ist ein SPRT auf genau dieser Seite: u = Masken-Byte 0 (@0x80039408 lbu s7,0(a2) -> @0x800394dc
 *     sb s7,-2(s0)), v = Masken-Byte 1 (@0x80039418 lbu fp,-9(s2) -> @0x800394e0 sb fp,-1(s0)), CLUT 0x7800 = VRAM
 *     (0,480) (@0x80039498 ori v0,zero,0x7800 -> @0x80039500 sh v0,0(s0)), tpage 0x95 = 8 bpp bei (320,256)
 *     (@0x80039630 ori a3,zero,0x95 -> @0x80039634 jal SetDrawMode).
 *
 * PORT-MASKEN (R15M, NUTZER-VORGABE fuer Cuts mit NULL-Sektion) gibt es in zwei Bauweisen:
 *   1. Generator 2026-09-03 (b8db7b2b/02c62294, analysis/esp_masken_2026-09-03/gen_masken.py): Masken-Byte = src IM
 *      BSS-ATLAS des Cuts — der Original-Mechanismus oben. Alle 30 Dateien der Stages 3..6 und ROOM1221.MSK, ohne
 *      eigenen Atlas.
 *   2. Freistellungen des Nutzers (Maskeneditor tools/maske): EIGENER Atlas MASKS/ROOM%04X_PRI%02d.TIM, weil srcX/srcY
 *      u8 sind (@0x80039408/@0x80039418) und ein 320 Pixel breites Bild damit nicht adressierbar ist.
 * 53741fe5 (2026-09-08) liess jede R15M-Sektion NUR mit eigenem Atlas zeichnen -> die Bauweise 1 war seitdem fuer
 * niemanden mehr sichtbar (Zensus 224 Sektionen, fg_atlas=0). Die Regel hier stellt beide richtig:
 *   R15M-Sektion MIT eigenem Atlas  -> eigener Atlas (unveraendert; ROOM1220 Cut 0/2/4/6/8 haetten sonst den falschen,
 *                                      Blit-Pruefung 77.7/765 statt 5.1)
 *   R15M-Sektion OHNE eigenen Atlas -> BSS-Atlas des Cuts (wie bis 53741fe5 und wie das Original; Blit-Pruefung der 224
 *                                      Sektionen 12.63/765 = Palettenrundung)
 *   Original-Sektion (RDT)          -> BSS-Atlas (unveraendert)
 */

#define RE15_PRI_ATLAS_BSS   0   /* Vordergrund-Atlas des Cuts aus dem BSS-Block (bg_pc.c Wege 1/2) */
#define RE15_PRI_ATLAS_EIGEN 1   /* MASKS/ROOM%04X_PRI%02d.TIM des Masken-Raums (re15_pri_masken_raum) */

/* nachgezeichnet = die Sektion stammt aus dem R15M-Container; eigener_atlas_da = MASKS/ROOM%04X_PRI%02d.TIM liegt vor. */
static inline int re15_pri_atlas_quelle(int nachgezeichnet, int eigener_atlas_da)
{
    return (nachgezeichnet && eigener_atlas_da) ? RE15_PRI_ATLAS_EIGEN : RE15_PRI_ATLAS_BSS;
}

/* Generator-Rechtecke, die eine Figur VOR jedem Objekt uebermalen wuerden, bleiben aus (engine/src/masken36_r37.c,
 * Tabelle gen/masken36_ausschluss_r37.inc). masken_raum = Raum-Id der MSK-Datei (re15_pri_masken_raum); idx = Rechteck in
 * der Reihenfolge von re15_pri_parse_section. */
#include "re15_pri.h"
int re15_masken36_ausgeschlossen(unsigned masken_raum, int cut, int idx);
/* Entfernt die ausgeschlossenen Rechtecke aus pri (Reihenfolge bleibt, draw_count folgt); Rueckgabe = neue mask_count. */
int re15_masken36_filter(unsigned masken_raum, int cut, re15_pri_cut_t *pri);
int re15_masken36_zeilen_zahl(void);

#endif
