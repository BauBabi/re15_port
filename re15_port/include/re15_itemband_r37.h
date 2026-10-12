/*
 * RE1.5 Rebuilt — Runde 37 Spur D "itemband" (Dossier analysis/befunde_runde37/D_itemband.md).
 *
 * Nutzer (AUFTRAG.md #8): "Items auf erhoehten Flaechen lassen sich vom falschen Stockwerk aufnehmen, z.B. die
 * Super Redhawk in ROOM3050 ohne die Kiste." Zwei Original-Mechanismen fehlten dem Port, NUR GEMEINSAM richtig:
 *
 * (1) BAND-GATE FUER ITEM. Der Aktions-Scan FUN_80042bac prueft fuer JEDEN Satz der AOT-Tabelle 0x800ac9b0 das
 *     Band, BEVOR er den Typ (sce, rec[0] @0x80042f48/@0x80042f74) ueberhaupt liest:
 *         80042cac  lbu   v0,2(s0)            ; rec[2] = floor (Item_aot_set pc[4]: rec = pc+2 @0x800406cc)
 *         80042cb4  andi  v0,v0,0x80
 *         80042cb8  bne   v0,zero,0x80042cd4  ; Bit 0x80 = Band egal
 *         80042cc0  lbu   v1,130(s1)          ; Entity+0x82 (s1 = a0 = Spieler 0x800ACA54, @0x80031fdc)
 *         80042cc4  lbu   v0,2(s0)
 *         80042ccc  bne   v1,v0,0x8004301c    ; anderes Band -> naechster Satz
 *     Der Port wendete dieses Gate auf GENERIC/AUTO_EVENT/MESSAGE/EXAMINE/FLAG_CHG (b89ada17) und Tueren an, nicht auf
 *     ITEM (sce 9 -> 0x8007469c[9] = 0x80043328). re15_itemband_r37_gate() ist der fehlende Teil fuer ITEM.
 *
 * (2) MEMBER 0x12 = DAS KOLLISIONSBAND DES SPIELERS. Member_set (LAB_800410b8) -> FUN_8004116c, Tabelle
 *     0x80010c8c[18] = 0x80041224: `j 0x80041230` / Delay-Slot `80041228 sb a2,130(a0)` = Work-Entity+0x82. Nach
 *     Work_set(1,0) (@0x80040da4-b0: 0x800ACA54) ist das 0x800ACAD6 — dasselbe Byte, das der Scan @0x80042cc0, die
 *     Schubkette @0x8002bf38 und die Kletter-Sonde @0x800381c8 lesen. Im Port ist dieses Byte s_coll_band
 *     (re15_collision.c); re15_actor_set_member case 18 schrieb nur g_actors[].floor. Folge: der Lift von
 *     ROOM3050 (sub04 @0x023D0 `34 12 00 00`) liess den Spieler auf Band 4 stehen, der vorgesehene Weg zur Super
 *     Redhawk (Kiste in die Kerbe, hochklettern auf Band 1) war nicht gangbar. re15_itemband_r37_member_band() setzt
 *     fuer den SPIELER-Slot das Kollisionsband (re15_collision_set_band setzt beide Haelften); fuer andere Aktoren
 *     bleibt es bei +0x82 = floor.
 *
 * (3) NACHBESSERUNG 1 — PLATTFORM-SPERRE LIFT A ROOM2030/2031 (PORT-WAHL nach RE2-Vorbild, Beta -> Retail).
 *     Mit (2) nimmt der Lift den Spieler auf das Zielband mit, wo immer er beim Druck stand. Der Ausloeser von Lift A
 *     ist sub00 @0x01CE6 (slot 5, floor 3) / @0x01D20 (slot 4, floor 6) `2c 0? 03 31 .. da 16 56 2c dc 05 dc 05 ff 00
 *     18 02 00 00`: sat 0x31 = NUR der Punkt 620 voraus (FUN_80042bac @0x80042ea8 `andi 0x40` -> kein Mitte-Test,
 *     @0x80042ef8 `andi 0x20` -> voraus), Rechteck x 5850..7350 z 11350..12850 = die Kabine (Zellen r0[33/34/35] Band 3,
 *     r0[49/55/56] Band 6). Ein Druck aus dem Kabineneingang (z 10730..10900, Blick Sued) bzw. knapp suedlich der
 *     oberen Kabine (z 13050..13470, Blick Nord) faehrt, und die Ausschiebung aus r0[49] bzw. r0[33] setzt den Spieler
 *     auf die FALSCHE Seite (gemessen 10182 / 13768, im Original identisch) — Softlock, im Port erst durch (2).
 *     RE2 Retail hat genau das an der Retail-Fassung des Beta-Lifts ROOM50D0 (= RE2 ROOM6030: Tuer slot 0 mit gleichem
 *     Rechteck/sat/floor, Liftausloeser an derselben Ecke (-18200,-14400))
 *     behoben: Plattformzone sub00 @0x02B9C `2c 03 06 41 03 00 e8 b8 c0 c7 20 03 b0 04 05 00 22 00 01 00` (FLAG_CHG,
 *     sat 0x41 = Spieler | MITTE -> (5,0x22)) und am Anfang des Fahrt-Unterprogramms sub10 @0x02F70-7A
 *     `06 00 08 00 21 05 22 00 01 00 08 00` = if (Ck(5,0x22)==0) Evt_end; (5,0x22) ist ein Ein-Bild-Zonenbit (Wisch
 *     FUN_80052e20 @0x80052e48 `sw zero` Bank 5 Wort 1). Der zweite Retail-Lift ROOM5060 hat sat 0x51 (MITTE).
 *     re15_itemband_r37_liftsperre(): Lift A faehrt nur, wenn die Spielermitte im Ausloese-Rechteck des Satzes steht
 *     (Mitte-Test wie @0x80042ea8); sonst ist der Druck verbraucht und nichts passiert (RE2: sub10 endet @0x02F78).
 *     Der Satz, das Band-Gate und Member 0x12 bleiben byte-true. Nur Lift A: einziger Lift mit Fallenstellen
 *     (Zensus unit_r37_itemband Teil F, echte Port-Kollision; Lift B, ROOM3050, ROOM50D0: 0).
 *
 * Messschrieb (Diagnose, kein Verhalten): RE15_ITEMBAND_LOG=<datei> — je Aktionsdruck, bei dem ein ITEM-Satz in
 * geometrischer Reichweite liegt, eine Zeile "ITEM slot s floor f spieler-band b -> GESPERRT|FREI", je
 * Member_set 0x12 am Spieler "MEMBER 0x12 Spieler: Kollisionsband a -> b" und je Ausloesung von Lift A
 * "LIFT slot s sub n ... mitte im Ausloeser ja|nein -> FAEHRT|GESPERRT".
 */
#ifndef RE15_ITEMBAND_R37_H
#define RE15_ITEMBAND_R37_H

#include <stdint.h>

/* Member-Id des Band-Bytes +0x82 (FUN_8004116c Tabelle 0x80010c8c[18] -> @0x80041228 `sb a2,130(a0)`). */
#define RE15_R37D_MEMBER_BAND      18
/* Ignorier-Bit des Satz-Bandes (@0x80042cb4 `andi v0,v0,0x80` / @0x80042cb8 `bne` -> 0x80042cd4). */
#define RE15_R37D_BAND_EGAL        0x80u

/* Haken in aot_common.c re15_aot_scan, NACH der Geometrie und VOR dem Feuern: fuer einen ITEM-Satz `slot` das
 * Band-Gate @0x80042cac-ccc anwenden. Liegt der Satz auf einem anderen Band als der Spieler, werden *reach und
 * *inside auf 0 gesetzt (der Satz feuert in diesem Bild nicht). Gegatet wird wie bei den uebrigen Typen nur bei
 * etabliertem Band (re15_collision_debug_band() >= 0). Rueckgabe 1 = gesperrt. */
int  re15_itemband_r37_gate(int slot, int *reach, int *inside);

/* Haken in actor_common.c re15_actor_set_member case 18: Member 0x12 am Spieler-Slot ist das Kollisionsband
 * (0x800ACAD6). Fuer andere Slots ohne Wirkung. */
void re15_itemband_r37_member_band(int slot, int32_t value);

/* Haken in aot_common.c re15_aot_scan, Feuer-Zweig der Ereignis-Saetze (GENERIC, default): ist `slot` ein Ausloeser
 * von Lift A in ROOM2030/2031 (Tabelle in itemband_r37.c, Satz sub00 @0x01CE6/@0x01D20, Unterprogramm 2) und steht
 * die Spielermitte NICHT im Ausloese-Rechteck (`mitte` = Mitte-Test des Scans fuer diesen Satz), ist der Druck
 * verbraucht und die Fahrt startet nicht (RE2 ROOM6030 sub10 @0x02F78 Evt_end). Rueckgabe 1 = gesperrt. */
int  re15_itemband_r37_liftsperre(int slot, int mitte);

/* Fuer den Zensus im Riegel: 1, wenn `slot` im Raum `raum` ein Lift-A-Ausloeser mit Plattform-Sperre ist (ohne Log,
 * ohne Zaehler). */
int  re15_itemband_r37_ist_liftsperre(unsigned raum, int slot);

/* Nur fuer Tests: Zaehler seit Programmstart (gesperrte ITEM-Saetze / Band-Setzungen ueber Member 0x12 / gesperrte
 * Lift-A-Ausloesungen). */
int  re15_itemband_r37_gesperrt_zahl(void);
int  re15_itemband_r37_member_zahl(void);
int  re15_itemband_r37_liftsperre_zahl(void);

#endif /* RE15_ITEMBAND_R37_H */
