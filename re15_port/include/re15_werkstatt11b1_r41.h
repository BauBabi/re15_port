/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 3: Elzas Werkstatt-Tuer in ROOM11B1 bleibt DAUERHAFT zu.
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23): "Die Tuer der Werkstatt in ROOM 11B1 bei Elza, wenn sie ueber ROOM 2000 rein
 * kommt und die Cutscene abspielt wo sie Elza sucht, ist immer noch offen. Ich wollte das sie dauerhaft geschlossen
 * bleibt in Elzas Szenario."
 *
 * ORIGINAL (ROOM11B1.RDT, Datei-Offsets; re15_port/tools/scd_dump_room.py):
 *   Die "Werkstatt-Tuer" ist KEINE Tuer, sondern die Durchgangszelle der Trennwand: SCA Region 3 Index 0x0C
 *   @0x00EC8 `e4 02 d0 07 7a a9 b2 bc 01 ff 00 03 ..` = x -22150 w 740, z -17230 h 2000, u0 = 0xff (solid), fl = 0x03
 *   (analysis/story_leon_elza/D_gegenpruefung.md §2.1, Runde 36 J §2.1). Im Bild ist sie nur Hintergrund: Cuts 0..6
 *   zeigen sie geschlossen, 7..10 offen (Runde 36 J N1.1: BSS Bild 2 vs 8 nur bbox (127,86)-(170,145) = das Tuerblatt).
 *   sub00 @0x010F6 (Init-Lauf, Slot 1):
 *     @0x010FA `21 03 83 01`  Ck(3,0x83)==1  ("Schacht offen"; Setzer ROOM10B1 sub04 @0x0195E `22 03 83 01`,
 *                                             Leon ROOM11B0 sub06 @0x01478)
 *     @0x01102 `23 00 0a 00 00 00` / @0x01108 `29 07` / @0x0110A `3c 01`  Eingangs-Cut 0 -> 7 (offenes Bild)
 *     @0x0110E `4b 00 07` + @0x01112 `4b 02 08`  Cut_replace 0<->7, 2<->8 (RVD-Zonen zeigen die offenen Bilder;
 *                                             LAB_80040414: Tausch, selbstinvers — @0x80040458..@0x80040494)
 *     @0x01116 `37 03 0c 00` + @0x0111A `39 03 0c 40`  Zelle (3,0x0C) AUF
 *     @0x0111E `2e 03 00` + @0x01122 `34 02 58 b4`  Container obj 0 nach z=-19368 (bleibt)
 *     @0x01126 Else: @0x0112A `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00` = Text-Platz 8
 *              (-22400,-17200,1200,1700) msg 12 @0x179F "The door is locked.", @0x0113E Aot_reset Slot 5 (Schacht zu),
 *              @0x01148..@0x01154 Zellen (2,4)/(3,9) auf.
 *   Elzas Weg: ROOM2001 @0x01958 `22 03 46 01` -> ROOM1261 Slot 0 @0x0112A -> 11B1 (-195,0,-21821) Cut 6 ->
 *   sub01 @0x01192 Ck(3,0x46)==1 -> sub03 @0x01438 ("Sherry!? / Where has she gone!?" msg 10 @0x01480, msg 11
 *   @0x014CE). Gemessen (Dossier E_kleinkram_b.md, Lauf w6): sub00 nimmt bei (3,0x83)==1 den offenen Zweig
 *   (Trace @0x1108 Cut_chg 7, @0x110E/@0x1112 Cut_replace, @0x1116/@0x111A Zelle auf) — die Tuer ist danach offen.
 *
 * PORT-WAHL (Form belegt): dasselbe Mittel wie Marvins Abschliessen bei Leon (Runde 36 Spur J, Ereignis 23 in
 * ROOM11B0, gen/szl11b0_szene.inc k_szl_tuerprog): ein Port-Ereignis, das im Init-Lauf HINTER sub00 laeuft
 * (FUN_8003ef6c @0x8003efd4 / Dispatcher @0x8003f17c: Slots der Reihe nach, ein im Lauf gestarteter hoeherer Slot
 * laeuft im selben Lauf) und den offenen Zweig Byte fuer Byte umkehrt:
 *   Cut_replace 0<->7 und 2<->8 noch einmal (selbstinvers), Zelle (3,0x0C) u0=0xff fl=0x03 (RDT @0x00EC8),
 *   Text-Platz 8 = die Original-Zeile @0x0112A, Cut 7 -> 0 bzw. 8 -> 2 (Form sub00 @0x01102..@0x0110A umgekehrt).
 * Schacht (Slot 5), Container und die Zellen (2,4)/(3,9) bleiben wie im offenen Zweig — Elza kommt weiter durch den
 * Schacht (ROOM1261 fuehrt auch nach 11D1 @0x0118A und 10B1 @0x0116A: kein Softlock). Nur ROOM11B1 (Elza); Leons
 * ROOM11B0 unberuehrt. Kein neues Bit, keine neue Nachricht (VERTRAG §1.1/§1.2: E nur vorhandene Nachrichten).
 */
#ifndef RE15_WERKSTATT11B1_R41_H
#define RE15_WERKSTATT11B1_R41_H

#include <stdint.h>

#define RE15_WK11B1_RAUM           0x11B1u  /* Elzas Garage (Leon: 0x11B0 — dort gilt Runde 36 J)              */
#define RE15_WK11B1_EREIGNIS       24       /* Port-Ereignis in ROOM11B1 (RDT-Subs 0..6; < RE15_RDT_MAX_SUB_SCD) */
#define RE15_WK11B1_FLAG_BANK      3        /* (3,0x83) "Schacht offen": sub00 @0x010FA, Setzer 10B1 @0x0195E    */
#define RE15_WK11B1_FLAG_BIT       0x83
#define RE15_WK11B1_ZELLE_REGION   3        /* SCA Region 3 Index 0x0C @0x00EC8                                 */
#define RE15_WK11B1_ZELLE_INDEX    0x0C
#define RE15_WK11B1_ZELLE_U0       0xFF     /* RDT @0x00ED1 u0 = ff (solid)                                     */
#define RE15_WK11B1_ZELLE_FL       0x03     /* RDT @0x00ED3 fl = 03                                             */
#define RE15_WK11B1_PLATZ          8        /* Text-Platz sub00 @0x0112A                                        */
#define RE15_WK11B1_MSG            12       /* "The door is locked." @0x179F                                    */
#define RE15_WK11B1_CUT_OFFEN_A    7        /* Cut_replace-Paare sub00 @0x0110E (0<->7) / @0x01112 (2<->8)      */
#define RE15_WK11B1_CUT_ZU_A       0
#define RE15_WK11B1_CUT_OFFEN_B    8
#define RE15_WK11B1_CUT_ZU_B       2
#define RE15_WK11B1_ANKER_OFF      0x10F6u  /* sub00 im RDT (102 B, Dump scd_dump_room.py)                       */
#define RE15_WK11B1_ANKER_LEN      102u

/* Raumaufbau, direkt hinter dem Start von sub00 im Init-Lauf (scd_room_setup.c, Muster re15_szl_init_lauf):
 * ROOM11B1 + (3,0x83)==1 + RDT-Anker stimmt -> Programm des Ereignisses RE15_WK11B1_EREIGNIS in einem freien
 * Ereignis-Slot starten (scd_thread_start, Slot-Suche wie scd_event_fire). */
void re15_werkstatt11b1_r41_init_lauf(uint16_t room_id);

/* Ereignis-Programm fuer (ROOM11B1, 24), sonst NULL. Seit Nachbesserung 1 NICHT mehr in der scd_event_fire-Kette
 * (scd_vm.c bleibt unberuehrt): re15_werkstatt11b1_r41_init_lauf startet das Programm selbst im ersten freien
 * Ereignis-Slot (SCD_EVENT_SLOT_FIRST..LAST, scd_thread_start). Zugriff fuer Riegel/Diagnose. */
const uint8_t *re15_werkstatt11b1_r41_ereignis(uint16_t room_id, uint8_t event_id);

/* Pruefhaken: Programmbytes + Laenge; 1, wenn der geladene RDT-Raum den Ankern entspricht; Zaehler der Starts. */
const uint8_t *re15_werkstatt11b1_r41_programm(int *out_len);
int            re15_werkstatt11b1_r41_anker_ok(void);
unsigned       re15_werkstatt11b1_r41_starts(void);

#endif /* RE15_WERKSTATT11B1_R41_H */
