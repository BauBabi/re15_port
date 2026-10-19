/*
 * RE1.5 Rebuilt — Runde 39 Spur P "johnruf1041", Nachbesserung 1: Tuersperren fuer Elza.
 * Dossier mit Messungen und Belegen: analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8.
 *
 * NUTZER-VORGABE (woertlich, analysis/befunde_runde39/AUFTRAG.md, Nachtrag 12):
 *   "beim Musikwechsel, wenn Elza John retten soll, soll beim anklciken der Tür von ROOM 1041 zu Room 1031 kommen
 *    "I have to rescue John! He is in danger!". Auch bei der Tür 1051 zu 10A1."
 *   "Außerdem möchte ich das in Elzas Szenario die Tür im ROOM 10B1 ausnahmslos IMMER geschlossen ist. auch wenn sie nach
 *    Sherry sucht."
 *   KORREKTUR (2026-10-19, nach Nachbesserung 2): "Ah, ich meinte die Tür in Raum 11B1, nicht 10B1, die sowieso geschlossen ist ohne Strom. Einfach noch schnell hinzufügen."
 *   -> gesperrt wird Elzas Parkplatz-Tuer ROOM11B1 -> ROOM11C1; 10B1 bleibt wie ausgeliefert (siehe unten).
 *
 * BELEGT IST DIE FORM (RE1.5 hat diese Sperren nicht):
 *   - Tuer -> Text = Aot_reset des Tuer-Slots auf sce 1 MESSAGE (LAB_80040738: rec[0] = pc[2] @0x80040764, rec[1] =
 *     (rec[1]&0x80)+pc[3] @0x8004076c-78, Nutzlast @0x80040788-a8; Ziel-Raum/Cut bleiben im Satz). Port re15_aot_retype.
 *   - Panikphase (1041/1051): je Bild unter einer Bedingung = RE1.5 ROOM11B1 sub01 (je Bild neu gestartet, FUN_8003f038
 *     @0x8003f064-80) @0x0119C Ck(4,0xF3)==0 -> @0x011C0 `46 01 01 31 0e 00 ff ff 00 00` (Elzas Tuer ohne Strom -> Text
 *     msg 14). Bedingung = Bank 9 Bit 97 (re15_johnruf_r39.h, gesetzt am Ende von RUF1041, geloescht bei Johns Rettung).
 *   - 10B1 (immer): RE1.5 ROOM11A1 main00 @0x00F9C Door_aot_set Slot 2 + @0x00FBC `46 02 01 31 01 00 ff ff 00 00` ohne
 *     Bedingung ("It's locked.") = eine Tuer, die bei jedem Raumaufbau zur Meldung wird.
 *   - Aot_on auf den gesperrten Slot ruft den MESSAGE-Handler: LAB_800407bc @0x8004080c `lbu v0,0(v1)` rec[0],
 *     @0x8004082c `jalr v0` PTR_8007469c[rec[0]], [1] = 0x80043084 (@0x800430a0 `jal 0x80027e68`, Maske @0x800430a4).
 *     Die 10B1-Szene (sub04 @0x01A8E `47 01`) zeigt deshalb den Text statt des Raumwechsels, ohne geaenderte Szenen-Bytes.
 *   - Texte: gen/tuersperre_r39.inc (tools/r39_johnruf/tuersperre_bauen.py, Form je Zeile im Werkzeugkopf).
 *
 * WELCHE TUER IN 11B1: main00 @0x00F9C Door_aot_set Slot 1 -> Raum 0x1C = ROOM11C1 (Parkplatz). Die einzige Tuer des
 *   Raums, die "ohne Strom ohnehin zu" ist: sub01 (je Bild) @0x0119C `06 00 4a 00` @0x011A0 `21 04 f3 00` Ck(4,0xF3)==0 ->
 *   @0x011C0 `46 01 01 31 0e 00 ff ff 00 00` (Slot 1 -> msg 14 "It's too dark to see anything...I must find way to
 *   activate the energy."; dieselbe Zeile fuer Item 4, Rolltor 7, Werkstatt-Tuer 8). Mit Strom jetzt dieselbe Zeile mit
 *   msg 12 @0x179F "The door is locked." (Original-Text der Werkstatt-Tuer, sub00 @0x0112A Aot_set Slot 8). Andere Wege
 *   bleiben: Slot 0 @0x00F56/@0x00F7A, Schacht Slot 5 @0x00FFA -> ROOM1261 (offen ab (3,0x83), sub00 @0x010FA).
 *   ROOM11C1 ist eine Sackgasse (main00 @0x016AE einzige Tuer -> 0x1B = 11B1, sub00 fuenf Hunde Typ 0x20, keine Items,
 *   kein Nachrichtenblock) - die Sperre kann Elza nirgends einschliessen; Leon (ROOM11B0) bleibt unberuehrt.
 *
 * (Historie) WELCHE TUER IN 10B1: ROOM10B1 hat zwei Uebergaenge - Slot 0 @0x016D2 = die Leiter zum Dach (ROOM1171) und Slot 1
 *   @0x016F2 = der Kanaldeckel (ROOM1261 "Sewer Passage"). Der einzige, der sich in der Sherry-Szene oeffnet, ist Slot 1
 *   (sub01 @0x01816 Ck(3,0x82) + @0x0181E Cut 7 -> @0x01824 Evt_exec sub04 "John, wait! There's still a little girl
 *   trapped in there! ... we can go through the sewer tunnels. Follow me." -> @0x01A8E Aot_on Slot 1).
 */
#ifndef RE15_TUERSPERRE_R39_H
#define RE15_TUERSPERRE_R39_H

#include <stdint.h>

/* ---- Tuer-Saetze (ausgeliefert) ------------------------------------------------------------------------------------- */
#define RE15_R39P_TS_RAUM_1041   0x1041u
#define RE15_R39P_TS_SLOT_1041   1      /* ROOM1041 main00 @0x010B8 `3b 01 ..` pc[1]; Ziel @0x010CF Raum 0x03 = ROOM1031    */
#define RE15_R39P_TS_ZIEL_1041   0x03u  /* @0x010B8+23                                                                      */
#define RE15_R39P_TS_MSG_1041    41     /* Orchestrator Nachbesserung 1 (VERTRAG 1.2: P 1041); einfacher Text ohne Stimme */
#define RE15_R39P_TS_RAUM_1051   0x1051u
#define RE15_R39P_TS_SLOT_1051   4      /* ROOM1051 main00 @0x00B5A `3b 04 ..` pc[1]; Ziel Raum 0x0A = ROOM10A1            */
#define RE15_R39P_TS_ZIEL_1051   0x0Au  /* @0x00B5A+23                                                                      */
#define RE15_R39P_TS_MSG_1051    40     /* Orchestrator Nachbesserung 1; ohne Stimme (Original-Ids 0..5)                   */
#define RE15_R39P_TS_RAUM_10B1   0x10B1u
#define RE15_R39P_TS_SLOT_10B1   1      /* ROOM10B1 main00 @0x016F2 `3b 01 ..` pc[1]; Ziel Raum 0x26 = ROOM1261 (Deckel)    */
#define RE15_R39P_TS_ZIEL_10B1   0x26u  /* @0x016F2+23                                                                      */
#define RE15_R39P_TS_MSG_10B1    40     /* erste Port-Id hinter den Original-Ids 0..5 (msg_sec 0x1AB8), wie 11C0/1051       */
#define RE15_R39P_TS_SUB_10B1    4      /* ROOM10B1 sub01 @0x01824 `04 ff 18 04` = die Szene der Sherry-Suche               */
#define RE15_R39P_TS_RAUM_11B1   0x11B1u
#define RE15_R39P_TS_SLOT_11B1   1      /* ROOM11B1 main00 @0x00F9C `3b 01 ..` pc[1]; Ziel Raum 0x1C = ROOM11C1 (Parkplatz)  */
#define RE15_R39P_TS_ZIEL_11B1   0x1Cu  /* @0x00F9C+23                                                                      */
#define RE15_R39P_TS_MSG_11B1    12     /* ROOM11B1 msg 12 @0x179F "The door is locked." (Original; sub00 @0x0112A Slot 8)   */
#define RE15_R39P_TS_MSG_DUNKEL  14     /* ROOM11B1 msg 14 @0x17E1 "It's too dark..." (sub01 @0x011C0, ohne Strom)          */
#define RE15_R39P_TS_STROM_BANK  4      /* ROOM11B1 sub01 @0x011A0 `21 04 f3 00` Ck(4,0xF3): Strom an                        */
#define RE15_R39P_TS_STROM_BIT   0xF3u

/* ---- Satzform (Aot_reset-Zeile) ------------------------------------------------------------------------------------- */
#define RE15_R39P_TS_SCE_MESSAGE 1      /* PTR_8007469c[1] = 0x80043084                                                    */
#define RE15_R39P_TS_SCE_TUER    2      /* PTR_8007469c[2] = 0x800430bc                                                    */
#define RE15_R39P_TS_SAT         0x31u  /* Door_aot_set pc[3] @0x010BB/@0x00B5D/@0x016F5 = ROOM11A1 @0x00FBF / 11B1 @0x011C3 */
#define RE15_R39P_TS_MASKE       0xFFFFu/* ROOM11A1 @0x00FC2 / ROOM11B1 @0x011C6 `ff ff` (alle ausgelieferten sce-1-Saetze) */

/* ---- Haken -------------------------------------------------------------------------------------------------------- */
/* Raumaufbau NACH dem Init-Lauf (aus re15_johnruf_r39_install: scd_room_setup.c Blockende + main.c Boot-/CONTINUE-Weg):
 * Texte einsetzen, Sperren pruefen (1041/1051 in der Panikphase; 11B1 immer, solange Strom da ist). */
void re15_tuersperre_r39_install(uint16_t room_id);
/* Je Spielbild (aus re15_johnruf_r39_tick, game_step_common.c VOR dem AOT-Scan): Panikphase begonnen (Set(9,97)=1 in RUF1041,
 * Elza steht in 1041) -> Tuer im selben Raum sofort Text (Form ROOM11B1 sub01 je Bild). */
void re15_tuersperre_r39_tick(void);
/* scd_vm.c op_evt_exec: ROOM10B1 sub04 bei gesperrtem Deckel -> Szenen-Huelle SZENE10B1 (Gosub sub04 + Szenen-Ende). */
const uint8_t *re15_tuersperre_r39_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig);

/* ---- Pruefhaken (Riegel, kein Spielverhalten) ---------------------------------------------------------------------- */
int  re15_tuersperre_r39_anker_ok(unsigned room_id);              /* Tuer-Satz (11B1: und msg 12) wie ausgeliefert      */
int  re15_tuersperre_r39_gesperrt(unsigned room_id);              /* 1 = Slot des Raums ist gerade der Sperrtext         */
const uint8_t *re15_tuersperre_r39_text(unsigned room_id, int *out_len);   /* Sperrtext des Raums */
const uint8_t *re15_tuersperre_r39_szene(int *out_len);                   /* Programm SZENE10B1 */
unsigned re15_tuersperre_r39_zaehler(int k);   /* 0 Sperren gesetzt, 1 Szenen-Huellen, 2 frei */

#endif /* RE15_TUERSPERRE_R39_H */
