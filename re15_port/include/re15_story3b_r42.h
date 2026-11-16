/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE3b: 30E0 in einem Besuch, Zeile in 3070.
 *
 * PORT-STORY nach dem freigegebenen Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md
 * (Abschnitt 3.2 L8-L10, Zeitleiste L-11/L-12; Entscheid F-08) und dem Nutzerbefund vom 15.11. ("In STAGE3 room 30E0 schliesst
 * die letzte Cutscene mit Marvin zu frueh ab, waehrend untertitel und Voiceover noch laufen"). T-03/T-04 (Birkin) stehen in
 * enemy_ai_common.c re15_birkin_anim_szene. Ids = VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3 (re15_story_r42.h).
 * NUR Leon: Raum exakt 30E0/3070 (Elzas 30E1/3071 sind eigene RDTs und bleiben unberuehrt).
 *
 * ---- Original-Ablauf (Mod-RDT ROOM30E0, SCD-Datenblock; belege/technik/scd_30E0.txt) ---------------------------------------
 * sub11 @0x01060: IF (4,194) -> Sce_em_set Ada @0x01068, Evt_exec sub02 @0x0107C; ELSE IF (3,75) -> Ada @0x0108C + Marvin @0x010A0,
 *   Evt_exec sub03 @0x010B4. (4,194) setzt ROOM3070 sub01 @0x03496 nach Birkins Tod ((5,0x1c) @0x0348E).
 * sub02 @0x00CB0..@0x00E62 (Ada): Set(4,194,0) @0x00CB0, Set(3,75,1) @0x00CB4, Riegel (2,7)/(1,27) @0x00CBC/@0x00CC0, Leon Pos_set
 *   @0x00CC8, msg 0..8 (msg 8 "That glass won't hold up..." @0x00E10), Nacken zurueck @0x00E38/@0x00E4A, Plc_ret @0x00E54, Cut_auto
 *   @0x00E56, Riegel los @0x00E58/@0x00E5C, Evt_end @0x00E60.
 * sub03 @0x00E62..@0x00FC0 (Marvin): Set(3,75,0) @0x00E62, Set(3,180,1) @0x00E66 (-> 3070 sub00 @0x03438 Aufzug-Szene), Riegel,
 *   Aot_reset Slot 0 -> msg 14 @0x00E72, Cut_chg 0 @0x00E7C, Leon Pos_set (-23730,-22800) @0x00E9C Dir 0xF3C @0x00EA4, msg 9..13
 *   (msg 13 @0x00F5A), danach Sleep 0x32+0x19+0x19+0x14 (@0x00F66..@0x00F82), Plc_ret x3, Sleep 0x1E @0x00FB0, Cut_auto @0x00FB4,
 *   Riegel los @0x00FB6/@0x00FBA, Evt_end @0x00FBE.
 * Disc (Version_40%) hat dieselbe Zweiteilung (sub00 @0x002B6/@0x002F4, Konzept [B14]).
 *
 * ---- L8-L10: EIN Besuch (Programm aus Original-Bytes + Uebergang) ------------------------------------------------------------
 * Evt_exec sub02 (Leon) -> sub02 [@0x00CB0, @0x00E54) + UEBERGANG + sub03 [@0x00E62, @0x00F86) + WARTEN + sub03 [@0x00F86, @0x00FC0).
 * UEBERGANG (PORT-STORY, Formen mit Beleg):
 *   Glasbruch   Se_on Port-Bank 0x3E Satz 0x21 (= RE2 ROOM1090 sub15 @0x02BB0 `36 02 21 01 ..`, GLAS1090; re15_dining_r39.h
 *               RE15_R39J_SATZ_GLAS), Form `36 3e 21 00 ..` wie gen/dining_r39_programme.inc.
 *   Hinhoeren   Form ROOM3091 sub07 nach den Toenen @0x01A62..: Sleep 0x0A @0x01A9E, je Figur Plc_neck mode 1 + Plc_dest mode 9 (Drehen)
 *               auf die Quelle @0x01AA6/@0x01AB0, Sleep 5 @0x01AB8, zweite Figur @0x01AC0/@0x01ACA, Sleep 0x1E @0x01AD2.
 *               Quelle = Tuer Slot 0 (main00 @0x009F2 Door_aot_set -> 3010, Rechteck x -18900..-18100 z -23500..-22180), durch die
 *               Marvin aus der Lobby kommt.
 *   Zeilen      msg 16 (Leon), msg 17 (Ada), je mit Text UND Textdauer (fehlt die Aufnahme: Untertitel mit der Textdauer).
 *   Schnitt     Cut_chg 0 wie sub03 @0x00E7C; im selben Bild Leon auf die sub03-Lage (Pos_set @0x00E9C / Dir_set @0x00EA4) und Ada auf
 *               ihre sub11-Lage des zweiten Besuchs (Sce_em_set @0x0108C: (-22490,-25870) Gier 0xD8E) - Form ROOM3070 sub02
 *               @0x0357E..@0x0358A (Work_set 2 + Member_set 0x00/0x02/0x04).
 *   Marvin      Sce_em_set-Satz von sub11 @0x010A0 (`44 01 40 40 00 01 00 ff`, Typ 0x40, Verhalten 0x40 = Executor) an der Tuer,
 *               Gier 2048 (Blick -x in den Raum), dann Plc_dest mode 4 (Gehen, Form ROOM1021 sub02 @0x020B2 `40 00 04 22`) auf seine
 *               sub11-Lage (-20020,-23340), Ankunftsbit wie ROOM5080 sub02 @0x007F4/@0x00802 (Do/Edwhile Ck(5,bit,0)).
 *   Humpeln     NICHT gebaut (Stufe STAGE3a NB1 B4: kein belegter Hinke-Zyklus in Marvins Bank).
 * WARTEN (Nutzerbefund 15.11., Mechanismus s. u.): Set(5,FREI,0), Sleep 1, Do/Edwhile Ck(5,FREI,0) nach der Sleep-Kette von msg 13,
 * vor dem Ausklang @0x00F86 (Nacken zurueck, Plc_ret x3, Sleep 0x1E, Cut_auto @0x00FB4). Erster Bau wartete vor Cut_auto: im
 * Entwicklungslauf leon1 liefen Marvin und Ada nach ihrem Plc_ret @0x00F90/@0x00F9A schon los, waehrend msg 13 noch sprach.
 * Ein Altstand mit (3,75) = 1 (Spielstand von v0.8.30 zwischen den Besuchen) bekommt sub03 mit demselben WARTEN.
 *
 * ---- Nutzerbefund 15.11.: Mechanismus (gemessen, scratchpad story/s3b_NOTIZ.md) ------------------------------------------
 * Die Sprachausgabe ist eigene Produktion (RE1.5 hat keine englische Stimme). Den Riegel "Satz ausreden lassen" hat der Port nur am
 * NAECHSTEN Message_on (scd_vm.c op_message_on, RE15_VOICE_NOTBREMSE): msg 13 ist die letzte Zeile von sub03, danach kommt nur die
 * Sleep-Kette @0x00F66..@0x00FB0 (152 Bilder) bis Cut_auto/Riegel los/Evt_end. main13.wav ist 9,01 s = 270 Bilder lang (gemessen) ->
 * die Szene gibt den Spieler frei, waehrend Untertitel und Stimme noch ~118 Bilder laufen. Original-Takt bleibt: gewartet wird nur,
 * solange Zeile oder Stimme noch laufen (Takt-Signal FREI wie Stufe STAGE3a: !g_scd.message_active && keine Stimme).
 *
 * ---- L-11/F-08: 3070 ----------------------------------------------------------------------------------------------------------
 * Wenn ROOM3070 sub01 @0x03496 (4,194) im laufenden Besuch setzt (Birkins Tod: (5,0x1c) aus der Todesmaschine @0x8011a6b8, Phase 2
 * = Zusammenbruch fertig), sagt Leon msg 6. Form der Reaktionszeile = Stufe STAGE3a "Gunshots" (Pad-Sperre Set(2,7) wie ROOM2080
 * sub02 @0x0CB6, kein Letterbox). Einmalig ueber den Uebergang 0 -> 1 im Besuch (30E0 sub02 loescht (4,194) @0x00CB0).
 * Nachbesserung 1 (B3): vor der Zeile poltert es im Lager (vier Holzschlaege, Takt ROOM3091 sub07 @0x01A56..@0x01A9E), Leon dreht
 * den Kopf zur Tuer nach 30E0 (Plc_neck mode 1, Form @0x01AA6) und nach der Zeile zurueck (Form ROOM30E0 sub02 @0x00E38).
 */
#ifndef RE15_STORY3B_R42_H
#define RE15_STORY3B_R42_H

#include <stdint.h>

#define RE15_S3B_RAUM_30E0        0x30E0u
#define RE15_S3B_RAUM_3070        0x3070u

/* ROOM30E0 SCD-Datenblock (Datei-Offsets, belege/technik/scd_30E0.txt) */
#define RE15_S3B_SUB02_OFF        0x00CB0   /* Set(4,194,0)                                                              */
#define RE15_S3B_SUB02_SCHLUSS    0x00E54   /* Plc_ret, danach Cut_auto/Riegel los/Evt_end (@0x00E54..@0x00E61)        */
#define RE15_S3B_SUB02_ENDE       0x00E62
#define RE15_S3B_SUB03_OFF        0x00E62   /* Set(3,75,0)                                                               */
#define RE15_S3B_SUB03_SCHLUSS    0x00FB4   /* Cut_auto 01                                                               */
#define RE15_S3B_SUB03_WARTEN     0x00F86   /* Plc_neck 41 00 (Marvin) nach msg 13 + Sleep-Kette: hier wird gewartet      */
#define RE15_S3B_SUB03_ENDE       0x00FC0
#define RE15_S3B_SUB_ADA          2         /* sub11 @0x0107C Evt_exec sub02                                            */
#define RE15_S3B_SUB_MARVIN       3         /* sub11 @0x010B4 Evt_exec sub03                                            */
#define RE15_S3B_MARVIN_SATZ      0x010A0   /* sub11 Sce_em_set Marvin                                                   */
#define RE15_S3B_ADA_SATZ         0x0108C   /* sub11 Sce_em_set Ada (zweiter Besuch)                                    */

/* Lagen (aus den Saetzen oben gelesen, hier zur Pruefung) */
#define RE15_S3B_MARVIN_X         (-20020)  /* @0x010A8 `cc b1`                                                         */
#define RE15_S3B_MARVIN_Z         (-23340)  /* @0x010AC `d4 a4`                                                         */
#define RE15_S3B_ADA_X            (-22490)  /* @0x01094 `26 a8`                                                         */
#define RE15_S3B_ADA_Z            (-25870)  /* @0x01098 `f2 9a`                                                         */
#define RE15_S3B_ADA_GIER         0x0D8E    /* @0x0109C `8e 0d`                                                         */
#define RE15_S3B_LEON_X           (-23730)  /* sub03 Pos_set @0x00E9C `4e a3`                                           */
#define RE15_S3B_LEON_Z           (-22800)  /* `f0 a6`                                                                  */
#define RE15_S3B_LEON_GIER        0x0F3C    /* sub03 Dir_set @0x00EA4 `3c 0f`                                           */
/* Tuer Slot 0 (main00 @0x009F2: Rechteck x -18900 w 800, z -23500 d 1320); Ostwand des West-Rings = SCA Eintrag 0 x >= -18800
 * (@0x51C). Marvin tritt 500 vor der Wandflaeche in den Raum (Wandklemme re15_npc_wall_clamp, NPC-Radius), z = Mitte der Tuer. */
#define RE15_S3B_TUER_X           (-19300)
#define RE15_S3B_TUER_Z           (-22840)
#define RE15_S3B_MARVIN_GIER_TUER 2048      /* Blick -x (0 = +x, 2048 = -x; dachkiste_r36.c wp_yaw_zu)                  */

/* Signale Bank 5 (Raum-Scratch; ROOM30E0-Skripte nutzen nur 0x00..0x06: sub01 @0x00BF0.., sub04 @0x00FC0.., sub09/10). */
#define RE15_S3B_SIG_FREI         0x14      /* Takt -> Programm: keine Zeile, keine Stimme mehr (wie RE15_S3A_SIG_FREI) */
#define RE15_S3B_SIG_WARTEN       0x15      /* Programm -> Takt: Messmarke 'Sleep-Kette nach msg 13 durch' (kein Verhalten)  */
#define RE15_S3B_AN_LEON          0x20      /* Plc_dest-Ankunftsbits (Form ROOM5080 sub02 @0x007F4 `40 00 09 20`)       */
#define RE15_S3B_AN_MARVIN        0x21
#define RE15_S3B_AN_ADA           0x22

/* Glasbruch (Port-Bank 0x3E, re15_dining_r39.h) */
#define RE15_S3B_GLAS_BANK        0x3E
#define RE15_S3B_GLAS_SATZ        0x21
/* 3070 (Nachbesserung 1, B3): Poltern aus dem Lager vor msg 6 = Port-Bank 0x3E Satz 0x00 (RE2 DOOR0F.DO2 Tonteil Satz 0, Holzschlag;
 * re15_dining_r39.h RE15_R39J_SATZ_KLOPFEN), Kopf zur Tuer Slot 1 nach 30E0 (ROOM3070 main00 @0x03322: x -13500 w 2200 = 0x0898,
 * z -19700 d 1200 = 0x04B0 -> Mitte). */
#define RE15_S3B_POLTERN_BANK     0x3E
#define RE15_S3B_POLTERN_SATZ     0x00
#define RE15_S3B_LAGER_X          (-12400)  /* -13500 + 2200/2 (@0x03328 `44 cb`, @0x0332C `98 08`)                     */
#define RE15_S3B_LAGER_Z          (-19100)  /* -19700 + 1200/2 (@0x0332A `0c b3`, @0x0332E `b0 04`)                     */

extern unsigned g_re15_s3b_zaehler[6];   /* [0] 30E0 ein Besuch [1] sub03 Altstand [2] 3070-Zeile [3] Warten am Ende (Bilder)
                                          * [4] Ende-Warten begonnen [5] Programmbau gescheitert (Bytes anders) */

void           re15_story3b_r42_texte(uint16_t room_id);      /* msg_common.c re15_msg_load_room_block (nach den RDT-Texten)  */
void           re15_story3b_r42_install(uint16_t room_id);    /* scd_room_setup.c                                            */
const uint8_t *re15_story3b_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc);   /* scd_vm.c Evt_exec      */
void           re15_story3b_r42_tick(void);                   /* scd_vm.c je Bild                                            */
const uint8_t *re15_story3b_r42_programm(int welches, int *out_len);   /* Tests: 0 = ein Besuch, 1 = sub03 Altstand, 2 = 3070 */
const uint8_t *re15_story3b_r42_text(uint16_t room_id, uint8_t id, int *out_len);
int            re15_story3b_r42_szene_laeuft(void);

#endif
