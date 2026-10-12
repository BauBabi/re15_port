/* re15_kiste_r37.h — Runde 37 Spur M "kiste" (Nachtrag 9 des Nutzers, AUFTRAG.md):
 *   "In der Kiste muessen die Icons offenbar so weit wie moeglich rechts liegen, damit sie nicht vom Text
 *    ueberdeckt werden. [...] beim scrollen/bewegen [...] die Items oberhalb des Kastens liegen. Das darf
 *    nicht sein, der Kasten muss layertechnisch ueber den Items liegen."
 * Dossier analysis/befunde_runde37/M_kiste.md. Geometrie der Box-LISTE (re15_inv_screen.c Block 7b) in
 * Port-Bildschirmkoordinaten. Das RE2-Panel BOXPANEL.BIN wird bei x 6 geblittet (Bild-Ursprung = RE2-Schirm
 * (6,13)), also ist Port-x = RE2-x und Port-y = RE2-y + RE15_BOX_PANEL_DY (12).
 *
 * RE2-Belege (ghidra_re2_Leon.txt, boxX = DAT_800d5c40 = 7, boxY = DAT_800d5c42 = 14):
 *   Fenster links   boxX+6    = 13   (Streifen sVar24 = boxX+6, FUN_800710dc)
 *   Fenster rechts  boxX+0xc4 = 203  (Auswahllinie x2 `addiu v0,t1,0xc4` @0x80071adc, inklusiv;
 *                   Streifen-Kante `addiu s6,t1,0xc5` @0x80071238 = 204 exklusiv)
 *   Fenster oben    RE2 y 40 = Kopfband (6,6) 20 hoch endet boxY+25 (Rahmentabelle @0x800a9c38)
 *   Fenster unten   RE2 y 140 = Fussband (6,127) beginnt boxY+127 = 141
 *   = BOXPANEL.BIN Listengrund (24,24,40) Bild-x 7..197 / Bild-y 27..127 (gemessen, Dossier §2.1). */
#ifndef RE15_KISTE_R37_H
#define RE15_KISTE_R37_H

#include <stdint.h>

/* Lage des Listen-Panels (BOXPANEL.BIN 214x156) auf dem Port-Schirm — wie der PANEL-Op in re15_inv_screen.c. */
#define RE15_KISTE_PANEL_X        6
#define RE15_KISTE_PANEL_W        214
#define RE15_KISTE_PANEL_H        156

/* Listenfenster im PANEL-BILD (inklusive Grenzen). Ausserhalb liegt der Rahmen ("Kasten"). */
#define RE15_KISTE_FENSTER_BX0    7      /* = RE2 x 13  (boxX+6)                       */
#define RE15_KISTE_FENSTER_BX1    197    /* = RE2 x 203 (boxX+0xc4 @0x80071adc)         */
#define RE15_KISTE_FENSTER_BY0    27     /* = RE2 y 40  (unter dem Kopfband)            */
#define RE15_KISTE_FENSTER_BY1    127    /* = RE2 y 140 (ueber dem Fussband boxY+127)   */

/* M1 — SYMBOL SO WEIT RECHTS WIE MOEGLICH: rechte Kante (exklusiv) = rechte Kante des Listenfensters
 * boxX+0xc5 = 204 (`addiu s6,t1,0xc5` @0x80071238; letztes Fensterpixel boxX+0xc4 = 203 @0x80071adc).
 * Jedes Symbol endet buendig am Fensterrand: x = 204 - Breite (schmal 25 -> 179, 2-Feld 49 -> 155).
 * NUTZER-VORGABE ("so weit wie moeglich rechts"); RE2 setzt beide Breiten an die gemeinsame linke Kante
 * boxX+0x92 = 153 (`addiu t1,t1,0x92` @0x800717d4) — die Grenze nach rechts ist dieselbe Fensterkante. */
#define RE15_KISTE_SYMBOL_RECHTS  204

/* MENGEN-ZIFFERN im Symbol unten links, wie RE2: FUN_80072c6c Tabelle @0x800aae2c (-67, -18+20k) + Ursprung
 * (220,70) (`li v0,0xdc` @0x80074b88, `li v0,0x46` @0x80074b90) -> x = boxX+0x92 = linke Symbolkante,
 * y = Zeilenoberkante (boxY+0x1b) + 11. Die Glyphen bleiben RE1.5s 8x8-Ziffern (box_digits). */
#define RE15_KISTE_ZIFFER_DX      0
#define RE15_KISTE_ZIFFER_DY      11

/* EINLAUFENDE ZEILE beim Gleiten (RE2 Zustaende 2/3): Zeile -1 (HOCH, Platz scroll-1: Name @0x80070128-74,
 * Symbol local_50 = -0x14, Ziffern `li s7,-0x26` @0x80072e74) bzw. Zeile 5 (RUNTER, Platz scroll+5: Name
 * @0x800702b4-0304, Symbol local_50 = 100, Ziffern `li s7,0x52` @0x80072e48). */
#define RE15_KISTE_EINLAUF_HOCH   (-1)
#define RE15_KISTE_EINLAUF_RUNTER 5

/* Die vier Rahmen-Teile = Panel-Bild ohne das Listenfenster, in Bild-Koordinaten {u, v, w, h}. Als
 * PANELTEIL-Ops VOR den Zeilen ausgegeben liegen sie UEBER ihnen — RE2 haengt die 16 Rahmen-Sprites in
 * OT-Eintrag 2 (`addiu v0,t1,0x8` @0x8007112c), Symbole und Namen in Eintrag 6 (`addiu v0,t1,0x18`
 * @0x80071154; Textkopf 12 -> Eintrag 6, FUN_80031bf0 @0x80031c38-58), Ziffern in Eintrag 3
 * (FUN_8007300c AddPrim 0x800cc1f4); ClearOTagR(…,8) @0x8002af34/@0x8002af90 + DrawOTag(+0x1c)
 * @0x8002bd54-5c zeichnen 7 -> 0, Eintrag 2 also NACH (= ueber) 6 und 3. */
typedef struct { uint8_t u, v, w, h; } re15_kiste_r37_teil_t;
#define RE15_KISTE_RAHMEN_TEILE   4
static const re15_kiste_r37_teil_t re15_kiste_r37_rahmen[RE15_KISTE_RAHMEN_TEILE] = {
    { 0, 0, RE15_KISTE_PANEL_W, RE15_KISTE_FENSTER_BY0 },                                      /* oben  */
    { 0, RE15_KISTE_FENSTER_BY1 + 1, RE15_KISTE_PANEL_W,
      RE15_KISTE_PANEL_H - (RE15_KISTE_FENSTER_BY1 + 1) },                                     /* unten */
    { 0, RE15_KISTE_FENSTER_BY0, RE15_KISTE_FENSTER_BX0,
      RE15_KISTE_FENSTER_BY1 + 1 - RE15_KISTE_FENSTER_BY0 },                                   /* links */
    { RE15_KISTE_FENSTER_BX1 + 1, RE15_KISTE_FENSTER_BY0,
      RE15_KISTE_PANEL_W - (RE15_KISTE_FENSTER_BX1 + 1),
      RE15_KISTE_FENSTER_BY1 + 1 - RE15_KISTE_FENSTER_BY0 },                                   /* rechts */
};

#endif /* RE15_KISTE_R37_H */
