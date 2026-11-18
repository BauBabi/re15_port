/*
 * RE1.5 Rebuilt — Runde 42 Strang "birkinfilm": der RE2-Film "Birkins Mutation" (R409.STR) in Leons erstem Treffen mit Annette
 * (ROOM6000), mit Rahmenzeilen Annettes aus derselben RE2-Szene (RE2-Stimmen); Stufe 2: Elzas Fassung in ROOM5011 (ohne neue
 * Saetze, Abschnitt "Stufe 2" unten).
 *
 * NUTZER (18.11.2026, woertlich): "Zusaetzlich moechte ich das du die Birkin Mutation Erklaerungscutscene aus Resident Evil 2
 * irgendwo sinnvoll mit einbaust." ORCHESTRATOR-ENTSCHEID (18.11., Ort vom Nutzer freigestellt): Leon ROOM6000 - der Film nach
 * "Where's Ada?"/"I see..." (msg 2 @0x0113A) und vor "Come on..." (msg 3 @0x01156); Rahmenzeilen Annettes, wo ein RE2-Satz passt
 * mit RE2-Stimme; Einschub 1:17,5-1:29,7 heraus; keine Untertitel im Film; Bild wie RE2 mit 40 Zeilen Rand oben; Uebergang wie
 * RE2 bei genau diesem Film (64 Bilder ab, harter Schnitt, 64 Bilder auf); einmal je Spielstand.
 * Sinn: Ada sagt in 50B0 "He's dead. Umbrella killed him." - der Film zeigt, dass sie ihn angeschossen haben und er sich das
 * G-Virus spritzt; das bereitet 5090 vor.
 *
 * ⛔ PORT-STORY, kein Auslieferungscode. Die RDTs bleiben bytegleich; die Szene ist ein Programm aus Original-Opcodes.
 *
 * WO (ROOM6000.RDT, tools/scd_dump_room.py):
 *   sub00 @0x00F3C Ifel_ck / @0x00F40 Ck(3,100)=0 -> @0x00F44 Sce_em_set Annette (Typ 0x4D) + @0x00F58 `04 ff 18 06` Evt_exec sub06.
 *   sub06 [@0x0106A, @0x011A6): @0x0106A `22 03 64 01` Set(3,100,1) als ERSTES -> die Szene laeuft einmal je Spielstand. Damit
 *   braucht der Film KEIN eigenes Bit (Zensus: der einzige Schreiber von (3,100) ist @0x0106A, der einzige Leser @0x00F40; ein
 *   zweites Bit waere immer gleich (3,100)).
 *   @0x0113A msg 2 "Annette: I see...", @0x0113E Work_set Annette, @0x01142..@0x01155 Geste, @0x01156 msg 3 "Annette: Come on. ...".
 * WIE: Evt_exec sub06 (sub00 @0x00F58) startet statt sub06 das Programm k_bf_6000_prog (gen/birkinfilm_r42.inc, tools/
 *   r42_birkinfilm/szene_bauen.py) = sub06 bytegleich mit einem Einschub vor @0x01156 (Ablauf und Belege im Kopf des Werkzeugs):
 *   msg 40 (Xa 0x09) - Geste - msg 41 (Xa 0x11) - Geste - Warte-Zeile 43 (RE2 Wsleep @0x027E2) - Sleep 2 (@0x027E4) - Sleep 60
 *   (@0x027F0) - MAIN anhalten (@0x02804) - Film-Zeile 44 (zws_r41.c, Blendenart RE15_ZWS_BLENDART_4090) - Sleep 63 (@0x02820) -
 *   MAIN weiter (@0x0282B) - msg 42 (Xa 0x14) - Geste - Original msg 3. Vorlage RE2 Leon-Disc ROOM4090 sub02.
 *   Umleitung nur, wenn die RDT-Bytes von sub06 gleich k_bf_6000_sub06 sind (sonst Original-Ablauf).
 *
 * RAHMENZEILEN = RE2-Saetze Annettes aus ROOM4090 sub02 (Leon-Disc), Stimme aus PL0/VOICE/VOICE04.XAS (Stage-Tabelle
 *   @0x80010818[4] = 0x347 -> Dateisatz @0x800988a4 + 0x347*8 = {3438592 B, LBA 99094}), XA-Tabelle COMMON/BIN/STAGE4.BIN @0x384:
 *     40  Xa 0x09 @0x3BA (Kanal 6, Block 0, Laenge 68)  "Annette Birkin. My husband is the man responsible for the creation of
 *                                                        the T-virus. William Birkin."
 *     41  Xa 0x11 @0x3EA (4, 0, 71)                     "And it's all Umbrella's fault. None of this would have happened if they
 *                                                        hadn't tried to steal his research away from him."  (RE2: direkt vor dem Film)
 *     42  Xa 0x14 @0x3FC (3, 0, 78)                     "As a result of his virus-induced transmutation, William should have lost
 *                                                        any prior memories he had as a human by now."
 *   Lesefenster = Laenge*16 - 0x1E Laufwerkssektoren (FUN_800129b4: Start @0x80012ac4-@0x80012adc, Ende @0x80012af4-@0x80012b10).
 *   Dateien shared_assets/RE2/R409_XAnn.XAS (tools/r42_birkinfilm/xa_schneiden.py). Gespielt ueber den Sprachkanal wie eine
 *   synchro-Aufnahme (audio_pc.c re15_voice_load_clip -> re15_birkinfilm_r42_stimme), damit Stimmen-Riegel, Option VOICE und
 *   Untertitel-Nachhall unveraendert gelten; ohne Pegel-Anhebung (RE2-Bytes).
 *   Ids 40..44: ROOM6000 RDT 0..7, Port-Neutext 0/3 (story_r36), 30/31 (r42/lageplaene) -> frei; < 64 (VOICE_MAX_MSG).
 *
 * WARTE-ZEILE 43 (`2b 2b 00 00`, kein Text): = RE2 `0b 0c` @0x027E2 (Wsleep 0x80053a8c / Wsleeping 0x80053ac0: wartet, solange
 *   das XA-Bit 0x20 in 0x800cfbd8 steht). Im Port wartet der Stimmen-Riegel davor (scd_vm.c op_message_on) auf das Ende der
 *   Stimme; die Zeile selbst wartet zusaetzlich, bis der Untertitel weg ist (PORT-WAHL: bei VOICE OFF laeuft sonst ein
 *   Untertitel in die Abblende), Notbremse RE15_BF_WARTEN_NOTBREMSE.
 *
 * FILM: shared_assets/PSX/MOVIE/R409BIRK.STR = R409.STR ohne die Sektoren der Bilder 2327..2692 (tools/r42_birkinfilm/
 *   film_schneiden.py), 3927 Bilder 320x160, Bildlage y = 40 (RE2-Filmtabelle @0x800a22cc +0x12 = 0x0028, gelesen
 *   `lhu v0,0x12(a3)` @0x80032254, addiert @0x80033200).
 */
#ifndef RE15_BIRKINFILM_R42_H
#define RE15_BIRKINFILM_R42_H

#include <stdint.h>

#define RE15_BF_RAUM            0x6000u
#define RE15_BF_SUB             6        /* sub00 @0x00F58 `04 ff 18 06`                                        */
#define RE15_BF_SZENE_BANK      3        /* (3,100): Schreiber sub06 @0x0106A `22 03 64 01`, Leser sub00 @0x00F40 */
#define RE15_BF_SZENE_BIT       100

#define RE15_BF_MSG_XA09        40
#define RE15_BF_MSG_XA11        41
#define RE15_BF_MSG_XA14        42
#define RE15_BF_MSG_WARTEN      43
#define RE15_BF_MSG_FILM        44
#define RE15_BF_WARTEN_NOTBREMSE 300     /* = RE15_VOICE_NOTBREMSE in scd_vm.c op_message_on                     */

/* RE2-Stimmen (STAGE4.BIN @0x384 + Nr*6, Lesefenster Laenge*16 - 0x1E) */
#define RE15_BF_XA09_DATEI      "R409_XA09.XAS"
#define RE15_BF_XA09_LESE       1058     /* 68*16 - 30, @0x3BA `06 00 00 00 44 00` */
#define RE15_BF_XA11_DATEI      "R409_XA11.XAS"
#define RE15_BF_XA11_LESE       1106     /* 71*16 - 30, @0x3EA `04 00 00 00 47 00` */
#define RE15_BF_XA14_DATEI      "R409_XA14.XAS"
#define RE15_BF_XA14_LESE       1218     /* 78*16 - 30, @0x3FC `03 00 00 00 4e 00` */

/* ==== Stufe 2: Elza ROOM5011 (Annette trifft Elza nie) ==================================================================
 * ORCHESTRATOR-ENTSCHEID 18.11.: der Film zwischen "John. I read some files ... terrifying things." (msg 3 @0x00640) und "She's
 * been infected by what they were working on here." (msg 4 @0x00690); keine neuen Saetze.
 * WO (ROOM5011.RDT, tools/scd_dump_room.py; Zeigertabelle sub_scd @0x00524 -> sub02 @0x0055A):
 *   sub00 @0x00536 Ifel_ck / @0x0053A `21 04 ed 01` Ck(4,237)=1 -> @0x0053E `04 ff 18 02` Evt_exec sub02.
 *   sub02 [@0x0055A, @0x0093A): @0x0055A `22 04 ed 00` Set(4,237,0) als ERSTES; @0x00640 `2b 03 00 00` msg 3, dann Elzas
 *   Gesten-/Blickblock @0x00644..@0x0068F (letzter Opcode Sleep 26 @0x0068C `09 0a 1a 00`), @0x00690 `2b 04 00 00` msg 4.
 *   Keine Sprung-Opcodes in sub02 (nur Sleep, Plc-Opcodes, Work_set, Message_on, Set, Cut_chg, Cut_auto, Gosub, Pos_set,
 *   Dir_set, Nop; szene_bauen.py prueft das) -> der Einschub verschiebt kein Sprungziel.
 * EINMAL JE SPIELSTAND OHNE NEUES BIT (Zensus tools/r42_birkinfilm/szene_bauen.py --zensus ueber alle RDTs): (4,237) schreibt
 *   nur ROOM5021 sub02 @0x01356 `22 04 ed 01` (=1; sub02 setzt vorher @0x01352 Set(4,192,1), sub00 @0x012EA startet es nur bei
 *   Ck(4,192)=0 -> einmal) und ROOM5011 sub02 @0x0055A (=0); einziger Leser ROOM5011 sub00 @0x0053A. Port-Code schreibt (4,237)
 *   nicht. Ein eigenes Bit waere immer gleich (4,237).
 * MUSIK: ROOM5011 hat keinen Sce_bgm_control; die Raummusik kommt aus der Tabelle @0x80074828 (FUN_80044210):
 *   Eintrag @0x800748C4 = [0x4D + 1] = `24 ff` -> MAIN 0x24, kein SUB (High-Byte 0xFF) -> nur Slot 0 anhalten, wie in 6000.
 * EINSCHUB vor @0x00690 = derselbe RE2-Ablauf wie in 6000 ohne Rahmenzeilen: Warte-Zeile 46 (RE2 Wsleep @0x027E2), Sleep 2
 *   (@0x027E4), Sleep 60 (@0x027F0), MAIN Op 4 (@0x02804), Film-Zeile 47, Sleep 63 (@0x02820), MAIN Op 3 (@0x0282B).
 * Ids 46/47: ROOM5011 RDT 0..45 (msg_sec @0x0E4C), Port 0x2C/0x2D (Kiste/Rechner = RDT-Ids), r42/rest 40/45 (RDT-Ids) -> frei,
 *   < 64 (VOICE_MAX_MSG); keine Datei synchro/STAGE5/room5011/main46/47.wav (die Haken verbrauchen die Zeilen vor dem Sprachkanal).
 */
#define RE15_BF_RAUM_ELZA       0x5011u
#define RE15_BF_ELZA_SUB        2        /* sub00 @0x0053E `04 ff 18 02`                                          */
#define RE15_BF_ELZA_BANK       4        /* (4,237): Leser sub00 @0x0053A, Schreiber 5021 @0x01356 (=1), 5011 @0x0055A (=0) */
#define RE15_BF_ELZA_BIT        237
#define RE15_BF_ELZA_MSG_WARTEN 46
#define RE15_BF_ELZA_MSG_FILM   47

#define RE15_BF_FILM            "MOVIE/R409BIRK.STR"
#define RE15_BF_FILM_BILDER     3927     /* 4293 - 366 (film_schneiden.py; Riegel prueft die Datei)              */
#define RE15_BF_FILM_Y          40       /* RE2 @0x800a22cc +0x12 = 0x0028 (FUN_80032150 @0x80032254)              */

extern unsigned g_re15_bf_umleitungen;   /* Evt_exec 6000 sub06 / 5011 sub02 -> Programm */
extern unsigned g_re15_bf_warten;        /* Takte, die die Warte-Zeile zuletzt auf den Untertitel gewartet hat */

void           re15_birkinfilm_r42_texte(uint16_t room_id);                                     /* msg_common.c */
const uint8_t *re15_birkinfilm_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc);  /* scd_vm.c op_evt_exec */
/* scd_vm.c op_message_on hinter dem Stimmen-Riegel: 0 = nicht diese Zeile, 1 = verbraucht (pc += 4), 2 = parken. */
int            re15_birkinfilm_r42_message_on(const uint8_t *pc);
/* Sprachkanal (audio_pc.c re15_voice_load_clip): RE2-Strecke fuer (Raum, Id) oder NULL; *lese = Lesefenster in Sektoren. */
const char    *re15_birkinfilm_r42_stimme(unsigned room_id, int voice_id, int *lese);
const uint8_t *re15_birkinfilm_r42_programm(int *out_len);                                       /* Tests (6000) */
const uint8_t *re15_birkinfilm_r42_programm_raum(unsigned room_id, int *out_len);                 /* Tests (6000/5011) */
const uint8_t *re15_birkinfilm_r42_text(uint8_t id, int *out_len);                               /* Tests */

#endif /* RE15_BIRKINFILM_R42_H */
