/*
 * RE1.5 Rebuilt — Zielscheiben-Texte im Schiessstand ROOM1190/1191 (Runde 35 Spur H, Punkt 2).
 *
 * NUTZER-VORGABE (AUFTRAG.md Runde 35, woertlich): "in ROOM 1190 bei der Zielscheibe ganz links und
 * bei der 3. Zielscheibe von links, das dort der Text ergaenzt wird: 'This target has a surprisingly
 * large number of bullet holes.'. Die anderen beiden Zielscheiben sollen den Text haben 'This target
 * does not have many bullet holes'".  Dossier: analysis/befunde_runde35/H_raeume.md, Punkt 2.
 *
 * WELCHE SCHEIBE WELCHE IST — aus der KAMERA gemessen, nicht aus der Slot-Nummer geraten:
 *   Die vier Scheiben sind Obj 0..3 (sub13 Obj_model_set @RDT 0x0297E/0x029A0/0x029C2/0x029E4,
 *   x=-21340, z=-24516/-20916/-17316/-13716); jede Schiene gehoert zu EINEM Schalter-AOT (Slot n,
 *   main00 @0x02226/0x0223A/0x0224E/0x02262, Rechteck x=-4400 w=600, z=-25700/-22000/-18500/-14900
 *   d=2400) — sub02..05 `Work_set 2e 03 0n` bewegt Obj n. Projektion der Scheibenmitte durch die
 *   Raumkameras (FUN_80053ca4-Nachbau tools/maske/geom.py cut_view, RDT+0x24 Kamerablock) und
 *   Framedump je Cut (Dossier-Bild p2_kameras_cut5_6_7_9.png): in Cut 5 (Blick den Stand hinunter),
 *   Cut 6 (Kabinen 3/4), Cut 7 (Kabinen 1/2) und Cut 9 liegen die Scheiben IMMER in der Reihenfolge
 *   Obj 0, 1, 2, 3 von links nach rechts.  => "ganz links" = Slot 0, "3. von links" = Slot 2.
 *   Gegenprobe Raetsel (sub01 @0x02448-0x02468): die Hunde kommen bei Scheibe 0 UND 2 vorn
 *   (Bank 5 Bit 4 = Slot 0 @sub02 0x024F4, Bit 5 = Slot 2 @sub04 0x025E4) und 1/3 hinten — die beiden
 *   "vielen Einschuesse" sind genau die Loesung des Leon-Raetsels ((3,111) == 0).
 *
 * FORM (belegt) — der Text ist eine eigene Port-Nachricht mit zwei Seiten:
 *   Seite 1 = der Scheiben-Satz, Seite 2 = die unveraenderte Original-Nachricht des Platzes:
 *     msg 0 "There's a switch here. Push it?" (Ja/Nein; sub02..05 @0x024BA/0x02532/0x025AA/0x02622
 *           und sub12 @0x02892, `2b 00 80 ff`)
 *     msg 2 "I have nothing else to do here." (sce-1-Platz nach dem Hunde-Ereignis: main00 Else-Zweig
 *           @0x0227A.. und sub10 Aot_reset @0x027B4.., Nutzlast `02 00 ff ff`)
 *   Seitenumbruch `02 00` vor einer Ja/Nein-Frage `03` = ROOM1190 msg 4 @0x2EAF ("...armor / that
 *   should fit you." 02 00 "Will you equip it?" 03 02 01) — dieselbe Raum-Datei hat genau diese Form.
 *   Kopf `04 02` = Kopf von msg 0/2 (@0x2E14 / @0x2E5E), Zeilenumbruch 0x08 wie msg 2 ("to do" 08
 *   "here.", @0x2E79). Glyphen: Tabelle msg_common.c re15_msg_glyph (A-Z 0x1D.., a-z 0x3D.., '.'
 *   0x57) — gegengelesen an msg 0 ("There's" = 30 44 41 4e 41 3a 4f @0x2E16).
 *   Zeilenbreiten (include/font_width.h = DEBUG.BIN[0x4416+code]): "This target has a surprisingly"
 *   202 px / "large number of bullet holes." 195 px; "This target does not have" 174 px / "many
 *   bullet holes." 120 px (in einem Stueck 401 bzw. 298 px > 271 px = 99 % aller Zeilen des Bestands).
 *   Satzende "." auch beim zweiten Satz: Nutzer schreibt ihn beim ersten aus, der zweite im Gleichlauf
 *   (Praezedenz leiche_1110_1230.c L8: 1193 von 1227 ausgelieferten Nachrichten enden auf Satzzeichen).
 *
 * IDs (VERTRAG.md 1.2: ROOM1190/1191 -> 6..11): 6 = viele Einschuesse, 7 = wenige. Der Text wird bei
 * JEDEM Oeffnen neu unter der Id abgelegt (Seite 2 = die gerade faellige Original-Nachricht), die
 * Stimme ist damit je Satz EINE Datei: synchro/STAGE1/room1190/main06.wav bzw. main07.wav.
 *
 * WELCHER PLATZ — work_vars[0] = Slot, gestempelt vom Aktions-Scan beim Ausloesen
 * (FORWARD-Treffer `sh ...,0x800B0FD0` @0x80042f3c, Port aot_common.c `g_scd.work_vars[0] = i`);
 * genau diese Variable liest auch sub01 (`Switch 13 00` @0x023C4). Geprueft wird zusaetzlich, dass
 * der Slot ein aktiver Platz mit dem Scheiben-Rechteck ist (linke Kante x=-4400).
 */
#ifndef RE15_ZIEL1190_H
#define RE15_ZIEL1190_H

#include <stdint.h>

#define RE15_ZIEL1190_RAUM        0x1190u  /* RE15_ROOM_BASE: 1190 und 1191 */
#define RE15_ZIEL1190_MSG_SCHALTER 0u      /* msg 0 "There's a switch here. Push it?" @0x2E14 */
#define RE15_ZIEL1190_MSG_NICHTS   2u      /* msg 2 "I have nothing else to do here." @0x2E5E */
#define RE15_ZIEL1190_MSG_VIELE    6u      /* Port-Id (VERTRAG 1.2) */
#define RE15_ZIEL1190_MSG_WENIGE   7u      /* Port-Id (VERTRAG 1.2) */
#define RE15_ZIEL1190_KANTE_X     (-4400)  /* Aot_set-Rechteck x, main00 @0x0222C `d0 ee` */

/* 1 = Slot 0..3 traegt "viele Einschuesse" (Slot 0 und 2), 0 = "wenige" (Slot 1 und 3), -1 = kein Platz. */
int re15_ziel1190_viele(int slot);

/* Der Scheiben-Platz, an dem gerade ausgeloest wurde (0..3), sonst -1 (anderer Raum, anderer Platz). */
int re15_ziel1190_slot(void);

/* Baut die Zwei-Seiten-Nachricht (Satz + Original-Nachricht `orig_msg` des Raums) nach out.
 * Liefert die Laenge, 0 bei Fehler. Fuer Tests offen. */
int re15_ziel1190_bauen(int slot, unsigned orig_msg, uint8_t *out, int cap);

/* Haken in op_message_on (scd_vm.c), VOR dem Ja/Nein-Zweig: 0 = nicht zustaendig, 1 = fertig
 * (pc += 4), 2 = parken (PC nicht weiter) — dieselbe Park-Semantik wie der Ja/Nein-Zweig. */
int re15_ziel1190_message_on(const uint8_t *pc, uint32_t pause_mask);

/* Haken in re15_scd_show_message (sce-1-Platz): 1 = gezeigt, 0 = nicht zustaendig. */
int re15_ziel1190_show(uint8_t index, uint32_t pause_mask);

#endif /* RE15_ZIEL1190_H */
