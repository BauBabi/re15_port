/*
 * RE1.5 Rebuilt — Runde 42 Strang "finale", Stufe 2 "Abfahrt": das Loch im Boden von Wagen B (ROOM509, Leon 5090 / Elza 5091).
 *
 * NUTZER (2026-11-17, Auswertung der unbenutzten Hintergruende, Frage 1 "Loch einfach da oder entstehen sehen?"): "Loch ist
 * einfach da". Vorschlag A1 (scratchpad hintergruende/VORSCHLAEGE.md): die Riesenform bricht beim Anfahren von unten in Wagen B
 * ein; vorn im Fuehrerstand blinkt dabei WARNING (A7, gebaut im Zugstart: annette_r41.c z_warnung / story6 k_s6_6031_zug); geht
 * man danach nach hinten, zeigt Wagen B Bild 10 (Boden aufgerissen) statt Bild 8.
 *
 * BILDER (ROOM509.BSS; Leon und Elza teilen die Datei, bg_pc.c): Bild 8 und Bild 10 zeigen dieselbe Stirnwand von Wagen B, in
 * Bild 10 ist der Boden vorn aufgerissen. Kameras RID 5090 Cut 8 @0x160 Auge (12450,-2810,-10562) Ziel (16274,-2094,-9456),
 * Cut 10 @0x1A0 Auge (10950,-3110,-10462) Ziel (15274,-1994,-9456), beide fov 29623 (5091 bytegleich). Bild 10 wird im
 * Auslieferungsstand nie gezeigt: RVD-Gruppe 10 ist nur der Anker @0x468, keine Zone fuehrt nach 10, kein Cut_chg 10 (Pruefung
 * pruefung_hg Abschnitt 1 Zeile 1). Auf Cut 8 fuehren die Zonen 7->8 @0x3F0 und 9->8 @0x454.
 *
 * MECHANISMUS = Original-Befehl Cut_replace (0x4B, `4b a b`, Handler LAB_80040414, scd_vm.c op_cut_replace: tauscht a<->b in
 * cam_from/cam_to JEDER RVD-Zone und in den Kamera-AOTs; Form ROOM30E1 main00 @0x9F2 `4b 01 05`, ROOM1130 sub00 Cut_replace(9,8)).
 * `4b 08 0a` beim Raumaufbau: die Zonen 7->8 @0x3F0 und 9->8 @0x454 zeigen danach Bild 10, die Ausgaenge der Gruppe 8 gelten fuer
 * Bild 10. Die RDT bleibt unveraendert (der Tausch wirkt auf die geparste Zonen-Kopie bis zum naechsten Raumladen, wie im
 * Original auf das RAM). Keine Tuer landet auf Cut 8 oder 10 (Eintritte Cut 4 aus 6030 @0xC92, Wagenuebergaenge 9/14/0/5 =
 * main00 @0x0104E..@0x010CE), der Tausch kann also keinen Tuer-Eintritt umlenken.
 *
 * WO: ROOM5090 sub00 @0x0127C / ROOM5091 sub00 @0x01260 `04 ff 18 03` (Evt_exec sub03, Zweig Ck(3,89)=1 @0x01278 / @0x0125C) ist
 * der Raumaufbau nach dem Tor. sub03 (5090 @0x012DA..@0x012F2, 5091 @0x012BE..@0x012D6, bytegleich, 24 B) ist die Licht-Schleife
 * Set(1,28,1) / Sleep 8 / Set(1,28,0) / Sleep 20 / Goto -16 / Evt_end. Port: dieselbe Stelle startet stattdessen das Programm
 * k_loch = `4b 08 0a` + sub03 unveraendert (Goto relativ zum Opcode, op_goto: pc[4..5] = -16 -> wieder auf den Set der Kopie).
 *
 * WANN: erst nach dem Zugstart der Figur - Leon (9,133) RE15_AN_ZUG_BIT (Setzer annette_r41.c z_bit, erstes Byte des Zugstart-
 * Programms), Elza (9,130) RE15_R42_BIT_ZUG_ELZA (Setzer k_s6_6031_zug +0x0000). Das Krachen gehoert zu diesem Programm und
 * laeuft darin immer; ein eigenes "eingebrochen"-Bit braucht es nicht. Vor dem Zugstart ist 5090/5091 ohnehin nicht betretbar
 * (Leon annette_r41.c k_tuer @0x0106A msg 10; Elza story6 k_s6_6031_tuer @0x00E90 msg 4).
 *
 * SPERRFLAECHE (A1: "Ueber dem Loch braucht es eine Sperrflaeche") - NICHT GEBAUT, gemessen (Messbild s2/M2_509_gitter_sca.png,
 * Bodengitter y=0 mit der Kamera-Rechnung von camera_common.c auf Bild 10 projiziert): das Loch reicht von etwa x 16300 (unter
 * dem Bildrand) bis x 18500 und quer von z -6300 bis z -11900. Der Gang von Wagen B ist zwischen den Waenden SCA 5090 #11
 * (z -5720..-2714, @0x8FC) und #12 (z -15600..-12734, @0x908) nur 7014 breit; neben dem Loch bleiben rund 580 (Wandseite z -5720)
 * bzw. 830 (z -12734) - weniger als der Koerperdurchmesser des Spielers 900 (re15_collision.c PR 450). Wagen B
 * ist der einzige Weg nach Wagen C (Tuer Slot 0 Wagen A -> B (24700,-9250), Slot 2 B @x -1000 -> C, main00 @0x0104E/@0x0108E).
 * Eine Sperrflaeche ueber dem Loch sperrt also den Weg zum Finale. Offen fuer den Nutzer (Bericht s2 r1).
 */
#ifndef RE15_ZUG_R42_H
#define RE15_ZUG_R42_H

#include <stdint.h>

#define RE15_ZUG_R42_RAUM_LEON   0x5090u
#define RE15_ZUG_R42_RAUM_ELZA   0x5091u
#define RE15_ZUG_R42_SUB_LICHT   3         /* 5090 sub00 @0x0127C / 5091 sub00 @0x01260 `04 ff 18 03`                    */
#define RE15_ZUG_R42_CUT_HEIL    8         /* RID @0x160, BSS ROOM509 Bild 8: Boden heil                                   */
#define RE15_ZUG_R42_CUT_LOCH    10        /* RID @0x1A0, BSS ROOM509 Bild 10: Boden aufgerissen                           */
#define RE15_ZUG_R42_SUB03_5090  0x012DAu  /* sub03 5090 (24 B)                                                            */
#define RE15_ZUG_R42_SUB03_5091  0x012BEu  /* sub03 5091 (24 B, bytegleich)                                                */
#define RE15_ZUG_R42_SUB03_N     24

/* op_evt_exec (scd_vm.c): 5090/5091 Evt_exec sub03 nach dem Zugstart -> Programm mit Cut_replace(8,10) + sub03. */
const uint8_t *re15_zug_r42_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target);
/* Tests: das Programm (Laenge) und die Zahl der Umleitungen seit dem Start. */
const uint8_t *re15_zug_r42_programm(int *out_len);
extern unsigned g_re15_zug_r42_loch;

#endif /* RE15_ZUG_R42_H */
