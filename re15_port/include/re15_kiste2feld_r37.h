/* re15_kiste2feld_r37.h — Runde 37 Spur R "kiste2feld" (Orchestrator-Auftrag R zu Nachtrag 9 des Nutzers):
 *   2-Feld-Symbole (Waffen 0x0e..0x13) in der Kisten-Liste wie RE2 VOLLSTAENDIG zeichnen; dieselbe Kachel-Logik
 *   im Inventar-Gitter. Dossier analysis/befunde_runde37/R_kiste2feld.md (§1 Messung, §2 Belege).
 *
 * Befund (gemessen an der exe, Dossier §1.3): das 2-Feld-Symbol der Kisten-Liste (49x19) tastete die SCHMALE
 * ITEMALL-Kachel (40x30 = nur die LINKE Haelfte der Waffe) 1:1 ab und wiederholte Spalte 39 in den Spalten
 * 40..48; im Inventar-Gitter fehlte die rechte Haelfte, sobald die Kopfzelle in Spalte 2 des Symbol-Speichers lag.
 */
#ifndef RE15_KISTE2FELD_R37_H
#define RE15_KISTE2FELD_R37_H

#include <stdint.h>

/* QUELL-SPANNE des Kisten-Zeilensymbols in Texeln = RE2s POLY_FT4 u1 - u0 (FUN_800710dc; u0 = u2
 * `addiu t0,zero,40` @0x800717b4):
 *   schmal  u1 = u3 `addiu t1,zero,80`  @0x80071844                    -> 40 Texel (eine 40x30-Kachel)
 *   breit   u1 = u3 `addiu t1,zero,120` @0x8007183c (Delay-Slot von `j` @0x80071838; gespeichert
 *           `sb t1,-14(s0)` @0x80071848 = u1, `sb t1,2(s0)` @0x80071860 = u3) -> 80 Texel = ZWEI Kacheln
 *           nebeneinander im Cache: Kachel[Id] und Kachel[Id+0x56] (FUN_80069c40: zweite LoadImage nach
 *           Zelle+1 mit src+0x19320 @0x80069d4c-58, nur Id 14..18 `sltiu v0,v0,0x6` @0x80069ce4).
 * Breit/schmal entscheidet RE2 am Size-Byte der Kiste (`lbu v0,-30590(at)` @0x80071824, `beq v0,zero` @0x8007182c);
 * im Port RE15_BOX_KIND_WIDE (Size 3). Der Op traegt die Spanne im Feld u (RE15_INV_PAGE_ITEMTILE). */
#define RE15_KISTE2FELD_SPANNE_SCHMAL   40
#define RE15_KISTE2FELD_SPANNE_BREIT    80
/* v2 - v0 = 30 Texel (s2 = s1 + 30: `addiu s2,zero,270` @0x800717c8 / `addiu s1,zero,240` @0x800717d0, je Zeile -30). */
#define RE15_KISTE2FELD_SPANNE_HOCH     30

/* RE1.5-GEGENSTUECK der zwei RE2-Kacheln: das 80x30-Breitbild der 2-Feld-Waffe im ITPS-Block (ITEM/ITPS.ITP,
 * Block Id*0x3000) bei +0x21A0 — die Aufnahme reicht `*(0x800ac77c)+0x21A0` an FUN_8004dc4c (@0x8001e0b8-c8),
 * FUN_800492b8 Modus 1 laedt es 0x28 hw = 80 Texel breit (`ori v0,zero,0x28` @0x80049314), 0x1e = 30 hoch
 * (`ori v0,zero,0x1e` @0x8004931c). Spalten 0..39 = linke Haelfte (= ITEMALL-Kachel[Id]), 40..79 = rechte. */
#define RE15_KISTE2FELD_ITPS_BLOCK      0x3000
#define RE15_KISTE2FELD_ITPS_BREITBILD  0x21A0
#define RE15_KISTE2FELD_BREIT_W         80
#define RE15_KISTE2FELD_BREIT_H         30
#define RE15_KISTE2FELD_HAELFTE_W       40   /* eine Zelle = 40 Texel: FUN_80049390 RECT.w `ori v0,zero,0x14`
                                              * (20 hw) @0x800493ac                                         */

/* GPU-ABTASTUNG eines gestauchten POLY_FT4 (Spanne n_tex Texel auf n_px Pixel, rechte Kante exklusiv): der
 * PSX-GPU-Rasterizer in DuckStation fuehrt das Texel-Attribut in Fixkomma 12 Bit mit Startwert + 1/2 und
 * Schritt (n_tex << 12) / n_px — das Texel ist also auf die naechste Ganzzahl GERUNDET (dasselbe Modell wie die
 * Gouraud-Kaesten in inv_render_pc.c, "ATTRIB_SHIFT=12 ... +half init"). Gemessen am Bild des laufenden RE2
 * (analysis/itembox_re2/referenz/re2_itembox_original.png, Dossier §2.3): Gatling Gun 770/771, R. Launcher
 * 752/771, F. Aid Spray (schmal) 475/475 Masken-Pixel gleich — abgerundet nur 759 / 746 / 469.
 * n_tex == n_px -> Identitaet (p). */
static inline int re15_kiste2feld_texel(int n_tex, int n_px, int p)
{
    if (n_px <= 0) return 0;
    return (2048 + p * ((n_tex << 12) / n_px)) >> 12;
}

/* INVENTAR-SYMBOLSPEICHER: welche Haelfte des Breitbilds traegt Zelle `zelle` (0..9)? Im Original traegt
 * die Kopfzelle k (Anzeige-Flag 1) die LINKE, die Schwanzzelle k+1 (Flag 2) die RECHTE Haelfte — das 80x30-Bild
 * wird nur ueber Zelle 0/1 geladen (FUN_8004dc4c `addu a0,zero,zero` / `ori a1,zero,0x1` @0x8004de44-48 ->
 * FUN_800492b8 @0x8004df08) und danach je 40x30-ZELLE verschoben (FUN_80049390 MoveImage `jal 0x80068d50`
 * @0x8004945c; Einfuegen @0x8004dc98..@0x8004de3c, Zusammenschieben @0x8004db30). Damit stimmt die Zuordnung
 * auch, wenn die Kopfzelle in Speicher-Spalte 2 liegt (Zelle 2/5/8: Schwanz bei u 0 der naechsten Zeile).
 * Rueckgabe 1 fuer eine 2-Feld-Zelle (Flag 1 oder 2) mit *id = Item-Id und *spalte0 = erste Breitbild-Spalte
 * (0 = Kopf, RE15_KISTE2FELD_HAELFTE_W = Schwanz), sonst 0. */
int re15_kiste2feld_zelle(int zelle, uint8_t *id, int *spalte0);

#endif /* RE15_KISTE2FELD_R37_H */
