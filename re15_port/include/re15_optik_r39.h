#ifndef RE15_OPTIK_R39_H
#define RE15_OPTIK_R39_H
/*
 * Runde 39 Spur L (optik) — Dossier analysis/befunde_runde39/L_optik.md.
 *
 * Nutzer (AUFTRAG.md, Marke befund_1100_F741_marke1.bmp): "Leon ist immer noch vom Boden ueberdeckt an den einen
 * Streifen." ROOM1100 Cut 1, Leon (-16007,-23658) im Quergang hinter der Ecke der Westwand.
 *
 * ORIGINAL-MECHANISMUS (PSX.EXE, selbst disassembliert): eine Maske ist ein SPRT auf der Vordergrund-Seite des
 * Hintergrunds, FUN_80039590 @0x80039628..38 SetDrawMode(p, 1, 1, tpage 0x95, 0) (@0x80039630 `ori a3,zero,0x95`,
 * @0x80039634 `jal 0x80069858`), eingehaengt in OT[depth] (@0x80039650 `lh a0,2(s3)`, @0x80039658 `sll a0,a0,2`,
 * @0x8003965c `addu a0,a0,s6`, @0x80039660 `jal 0x8006b538` AddPrim). Die Figur haengt in OT[otz>>4]
 * (@0x8002565c `sra v1,v1,4`), otz = (1023*vz)>>12 (ZSF3 @0x80066c70 `addiu t0,zero,341`). Eine Maske malt also die
 * HINTERGRUNDPUNKTE ihres Rechtecks ueber jede fernere Figur — was immer diese Punkte zeigen.
 *
 * PORT-DATEN: ROOM1100 hat im Original keine Masken (Cut-Saetze @0x80/@0xA0 Satz+0x1C -> 0x60C/0x610 = FF FF FF FF).
 * Die Port-Masken (MASKS/ROOM1100.MSK, NUTZER-VORGABE Marken F205/F595) sind die Westwand als Quader
 * [-17600,-23200,1910,11100,-4000] (Cut 1) bzw. [-17600,-23200,1890,11100,-4000] (Cut 2). Die Quaderenden z -23200 /
 * -12100 sind die SCA-Zelle 2 (ROOM1100.RDT @0x6A4 `d0 07 5c 2b 40 bb 60 a5` = w 2000, d 11100, x -17600, z -23200),
 * NICHT die gemalte Wand: im Hintergrund endet die Wand 5..7 (Cut 1) bzw. 4..5 (Cut 2) Bildspalten frueher. Die
 * Rechtecke jenseits der gemalten Ecke tragen Boden und Rueckwand des HINTEREN Gangs mit der Tiefe der Wand und malen
 * sie ueber jede Figur, die dort steht (Cut 1: 474 Punkte, Rechtecke 45..51; Cut 2: 372 Punkte, Rechtecke 46..52).
 *
 * KORREKTUR (MASKS/ * bleibt unveraendert, wie masken36_r37.c): die Rechtecke dieser zwei Cuts werden beim Laden auf
 * die GEMALTE Wand gekappt — je Bildzeilenband die Spalten, in denen der Hintergrund die nahe Wand zeigt (gemessen am
 * Engine-Dekoder-Bild, Halbwert-Kanten s. optik_r39.c). Kein Rechteck waechst, keine Tiefe aendert sich; Rechtecke ganz
 * jenseits der Ecke entfallen, Rechtecke ueber beide Zeilenbaender werden geteilt.
 */
#include "re15_pri.h"

/* Kappt die Rechtecke von `pri` (Sektion des Masken-Raums `masken_raum`, re15_pri_masken_raum) fuer `cut` an der
 * gemalten Wand. Rueckgabe: neue mask_count (unveraendert, wenn fuer Raum/Cut kein Fenster eingetragen ist). */
int re15_optik_r39_kappen(unsigned masken_raum, int cut, re15_pri_cut_t *pri);

/* Fenster-Abfrage fuer Riegel: liegt Bildpunkt (x,y) fuer Raum/Cut innerhalb der gemalten Wand (1), ausserhalb (0)
 * oder ist fuer Raum/Cut nichts eingetragen (-1)? */
int re15_optik_r39_in_wand(unsigned masken_raum, int cut, int x, int y);

#endif /* RE15_OPTIK_R39_H */
