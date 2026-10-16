#ifndef RE15_NCLIP_R38_H
#define RE15_NCLIP_R38_H

/* ============================================================================
 * Runde 38 Spur A "cursor" — Rueckseiten-Verwurf der OBJEKT-Zeichner (GTE NCLIP)
 * Dossier: analysis/befunde_runde38/A_cursor.md
 * ============================================================================
 * Nutzer (AUFTRAG.md Runde 38, woertlich): "Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet
 * wird, hat immer noch so einen komischen Schatten. Den hat er im Original nicht..."
 *
 * BEFUND (gemessen, Dossier §1): der Cursor ist ein Kasten (ROOM11F0.RDT MD1 @0x001928, TIM @0x018DAC). Oberseite
 * y=-900 (49 Vierecke) und UNTERSEITE y=+923 (4 Dreiecke) tragen dasselbe Muster (Eckwinkel + Kreuz, u 4..125
 * v 0..82); die Oberseite ist zu 92,8 % Texel 0 (durchsichtig). Der PC-Zeichner reihte die rueckseitigen Dreiecke
 * der Unterseite ein (RE15_TRILOG: 4 Dreiecke NCLIP < 0, Tiefe 17515 gegen 15687); sie schienen 1828 Einheiten
 * tiefer, kleiner und dunkel schattiert durch die Oberseite = der "komische Schatten".
 *
 * ORIGINAL: die Objekt-Zeichenschleife FUN_8002c18c ruft fuer jedes Obj_model_set-Objekt FUN_800254a0 (GT3) +
 * FUN_800256b0 (GT4), bei Attribut & 0x10 FUN_80022f0c + FUN_80023098 (flach), Typ 3 ueber FUN_8003e64c dieselben
 * zwei. ALLE VIER verwerfen eine Flaeche, deren NCLIP-Ergebnis MAC0 < 0 ist (selbst disassembliert):
 *   FUN_800254a0  NCLIP `4b400006` @0x80025580, swc2 MAC0 `e9980000` @0x8002559c, @0x800255a8 `bgez v0,0x800255d0`
 *   FUN_800256b0  NCLIP @0x80025798, swc2 @0x800257b4, @0x800257c0 `bgez v0,0x800257e8`
 *   FUN_80022f0c  NCLIP @0x80022fec, swc2 @0x80023004, @0x80023010 `bltz v0,0x80023080`
 *   FUN_80023098  NCLIP @0x80023168, swc2 @0x80023184, @0x80023190 `bgez v0,0x800231b8`
 * Ecken: die ersten drei des Flaechensatzes (n0 v0 n1 v1 n2 v2 [n3 v3]) = v0, v1, v2 — auch beim Viereck
 * (FUN_800254a0 @0x800254f4/f8/0x80025508 `lh` Satz+2/+6/+10 -> RTPT @0x8002554c; FUN_800256b0 RTPT @0x80025764,
 * die vierte Ecke danach per RTPS). Verworfen wird STRENG kleiner 0; MAC0 = 0 wird gezeichnet.
 *
 * NCLIP (psx-spx docs/geometrytransformationenginegte.md:437-441, COP2 1400006h):
 *   MAC0 = SX0*SY1 + SX1*SY2 + SX2*SY0 - SX0*SY2 - SX1*SY0 - SX2*SY1
 * auf den Bildschirm-FIFO-Werten, die RTPS/RTPT auf -400h..+3FFh saettigen (ebd. :418-419, FLAG.14/13 :294-295).
 * Diese Saettigung wird hier nachgebildet (nur fuer die Entscheidung; gezeichnet wird weiter mit den Port-Koordinaten).
 * ============================================================================ */

#include <stdint.h>

/* Bildschirm-FIFO-Saettigung der GTE (SX2/SY2, psx-spx :418-419). */
#define RE15_NCLIP_R38_SXY_MIN (-0x400)
#define RE15_NCLIP_R38_SXY_MAX   0x3FF

/* MAC0 der GTE-Operation NCLIP auf drei Bildschirmpunkten (vorher wie die GTE gesaettigt). */
int32_t re15_nclip_r38_mac0(int x0, int y0, int x1, int y1, int x2, int y2);

/* 1 = das Original verwirft die Flaeche (MAC0 < 0), 0 = es zeichnet sie. Zaehlt die Verwuerfe mit. */
int re15_nclip_r38_verwerfen(int x0, int y0, int x1, int y1, int x2, int y2);

/* Messhaken: Anzahl verworfener Objekt-Flaechen seit Programmstart (nur lesen). */
extern unsigned g_re15_nclip_r38_verworfen;

#endif /* RE15_NCLIP_R38_H */
