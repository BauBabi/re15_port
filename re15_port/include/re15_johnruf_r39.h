/*
 * RE1.5 Rebuilt — Runde 39 Spur P "johnruf1041": Johns Hilferufe ab ROOM1041 (Elza), Elzas Satz, Karte mit ROOM1141,
 * Panikphase (Bank 9 Bit 97) mit Leons Schlussmusik bis zu Johns Rettung, lauterer Ruf im Flur ROOM1131.
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde39/P_johnruf1041.md.
 *
 * NUTZER-VORGABE (woertlich, analysis/befunde_runde39/AUFTRAG.md, Nachtrag 10):
 *   "Ich möchte das die Hilferufe von John erstmalig bereits erstmalig leise im ROOM 1040 kommen, nachdem John
 *    verschwunden ist. Also die Hilferufe sollen leise sein. dann soll bereits von Elza kommen: Oh, no, this is John!
 *    He is in Trouble, I have to help him! - nicht erst in ROOM 1041. Kopiere mir auch das voicefile stattdessen dort
 *    hin. Dann soll die Karte aufgehen und den ROOM 1141 markieren. Danach soll wieder die gleiche Panische finale Musik
 *    kommen wie bei Leon zum Ende - wenn Irons stirbt und er zu den gorillas muss. Wenn wir uns den Flur 1131 näheren,
 *    könnte das please help von John auch noch mal kommen, aber schon etwas lauter. [...]"
 *   "Die panische Musik hört auf, sobald man John gerettet hat. Der Rest bleibt normal."
 * Lesart (Orchestrator, verbindlich): Elza-Raeume = ROOMxxx1; "ROOM 1040" = ROOM1041; "nicht erst in ROOM 1041" = 1141.
 *
 * ABLAUF
 *   ROOM1041  Installer (scd_room_setup.c Blockende + Boot-/CONTINUE-Weg): (4,0xF0) = 1 (John-Szene 1021 gelaufen, John
 *             verschwunden: ROOM1021 sub02 @0x01FE0 `22 04 f0 01`, @0x01FD8 (4,0x60)=0 -> kein Spawn mehr @0x01DF6) und
 *             Bit 97 = 0 -> Programm RUF1041 (gen/johnruf_r39_programme.inc) in einem freien Ereignis-Faden. Form =
 *             ROOM1041 sub11 @0x021EA..@0x0223C. Leise Rufe (Xa_vol 50), Elza dreht sich zur Treppe, msg 40 (Stimme
 *             room1041/main40.wav), Warte-Zeile 59: Kartenhinweis ROOM1041 -> ROOM1141, Faden parkt bis zum Schliessen,
 *             dann Set(9,97)=1.
 *   Panikphase  Bit 97 = 1: MAIN01 (Leons Panikmusik nach Irons' Tod, Runde 35 K) in allen STAGE1-Raeumen ausser ROOM1141
 *             (dort die eigene Musik MAIN16 der Spur J, wie 11C0 bei Leon), Sce_bgm_control der Raumskripte an Slot 0 gilt
 *             dann nicht. ROOM1131: einmal je Phase der Ruf help02 lauter (Xa_vol 100), Programm RUF1131.
 *   Rettung   (4,0xF0) = 0 und (4,0xF2) = 0 (ROOM1141 sub02 @0x00C18 loescht F0, sub01 @0x00C0C loescht F2 = beide
 *             Gorillas tot, @0x00C06 MAIN16 Stop) -> Bit 97 = 0, Fenster zu, Raummusik = Tabelle.
 *
 * KEIN ASSET-PATCH: Programme = ROOM1041-Original-Bytes + eingefuegte Zeilen in Original-Form (Listing im .inc).
 */
#ifndef RE15_JOHNRUF_R39_H
#define RE15_JOHNRUF_R39_H

#include <stdint.h>
#include <stddef.h>
#include "re15_scd.h"   /* scd_thread_t (Opcode 0x5F) */

/* ---- Raeume ------------------------------------------------------------------------------------------------------ */
#define RE15_R39P_RAUM_1041      0x1041u
#define RE15_R39P_RAUM_1131      0x1131u
#define RE15_R39P_RAUM_1141      0x1141u
#define RE15_R39P_BGM_RAUM_1141  0x14   /* Raum-Byte von 1140/1141 (Stage 1) in ss_bgm_entry: eigene Musik (Tabelle
                                         * @0x80074828[0x14] = 0xFF56 = MAIN16 mit Handstart-Flag, Runde 38 F)         */

/* ---- Ressourcen (analysis/befunde_runde39/VERTRAG.md 1.1 / 1.2) --------------------------------------------------- */
#define RE15_R39P_BIT_BANK       9
#define RE15_R39P_BIT_PANIK      97     /* Panikphase: gesetzt am Ende von RUF1041, geloescht bei Johns Rettung; Q/R lesen */
#define RE15_R39P_MSG_SATZ       40     /* ROOM1041 "Elza: Oh, no, this is John! / He is in trouble, I have to help him!"    */
#define RE15_R39P_HILFE_ID0      53     /* Sprach-Ids 53..57 = synchro/STAGE1/room1141/help01..05.wav (Zuordnung wie Spur J
                                         * RE15_R39J_HILFE_ID0), in 1041/1131 nur Stimme ohne Text                          */
#define RE15_R39P_HILFE_N        5
#define RE15_R39P_MSG_KARTE      59     /* ROOM1041 Port-Warte-Zeile ohne Text: Kartenhinweis anfordern, parken bis zu      */

/* ---- Johns Flags (Original, nur gelesen) -------------------------------------------------------------------------- */
#define RE15_R39P_F0_BANK        4
#define RE15_R39P_F0_BIT         0xF0   /* John-Szene 1021 gelaufen (ROOM1021 sub02 @0x01FE0 Set, ROOM1141 sub02 @0x00C18 Clr) */
#define RE15_R39P_F2_BANK        4
#define RE15_R39P_F2_BIT         0xF2   /* Gorillas leben (ROOM1141 sub02 @0x00C1C Set, sub01 @0x00C0C Clr = Rettung)       */

/* ---- Lautstaerke = RE2 Xa_vol (Opcode 0x5F, Handler @0x80058090; XA-Lautstaerke 0x800d5b50) ----------------------- */
#define RE15_R39P_XA_OPCODE      0x5F   /* RE2-Verteiler @0x800a74c8[0x5F] = 0x80058090; RE1.5-Tabelle endet bei 0x5E    */
#define RE15_R39P_XA_NORMAL      120    /* RE2-Standard: FUN_800128c4 @0x80012958 `li v0,0x78`, je Raum-Skript-Init
                                         * FUN_80052f3c @0x80053014 `addiu v0,zero,120` / @0x80053038 `sw v0,0x5b50(at)`  */
#define RE15_R39P_XA_LEISE       50     /* "leise" 1041: RE2 ROOM11D0.RDT sub09 @0x01F84 `5f 32` (Stimme aus dem Off)       */
#define RE15_R39P_XA_LAUTER      100    /* "etwas lauter" 1131: RE2 ROOM1020.RDT sub02 @0x01960 `5f 64` = dieselbe XA-Stimme
                                         * 0x0B, die ROOM11D0 @0x01F7A mit 70 spielt (RE2 regelt sie je Raum). 100 statt 70
                                         * GEMESSEN (Dossier 4.2): in 1131 laeuft MAIN01; mit 70 laege der Ruf nur +2,9 dB ueber
                                         * der Musik (1041 leise: +4,2 dB ueber dem Raumklang), mit 100 +5,9 dB                  */
#define RE15_R39P_XA_GRENZE      0x80   /* Handler nimmt nur Werte < 0x80 an: @0x800580a0 `sltiu v0,v1,0x80`               */
#define RE15_R39P_CD_VOL_VOLL    0x3FFF /* Port-Mischer: CD_VOL bei Standard 120 (audio_pc.c re15_xa_init = CD_initvol)    */

/* ---- Rufe (PORT-WAHL der Dateien, Dossier 3) ---------------------------------------------------------------------- */
#define RE15_R39P_RUF1041_A      3      /* help03 zuerst (wie Spur J MIX in 1141)                                         */
#define RE15_R39P_RUF1041_B      2      /* help02 "Please help!" (Spracherkennung Dossier 3.2)                             */
#define RE15_R39P_RUF1131        2      /* "das please help von John auch noch mal" = derselbe Ruf, lauter                */
#define RE15_R39P_TREPPE_X       (-19650)   /* Elza dreht sich zur Tuer slot 2 -> ROOM1061 (Treppe): ROOM1041 main00     */
#define RE15_R39P_TREPPE_Z       (-13000)   /* @0x010D8 Rechteck x -20400 B 1500, z -14100 T 2200 -> Mitte               */

/* ---- Musik: Form der Runde 35 Spur K (re15_cut10f0.h, MAIN01 bis zum Parkplatz) --------------------------------------- */
#define RE15_R39P_BGM_EINTRAG    0xFF01 /* = RE15_CUT10F0_BGM_EINTRAG: MAIN01 (Slot 1) ohne Handstart-Flag, ohne SUB      */

/* ---- Haken ------------------------------------------------------------------------------------------------------- */
/* scd_room_setup.c (Ende des Installer-Blocks) und platform/pc/main.c (Boot-/CONTINUE-Weg): je Raum. */
void re15_johnruf_r39_install(uint16_t room_id);
/* game_step_common.c (je Spielbild, nach re15_tagebuch_r38_tick): Rettung -> Bit 97 aus; Musikfenster + Raummusik. */
void re15_johnruf_r39_tick(void);
/* scd_vm.c op_message_on (hinter dem Stimmen-Riegel): 0 = nicht zustaendig, 1 = verbraucht (pc += 4), 2 = parken. Nur
 * Zeilen IN den Programmen dieses Moduls (PC-Schranke). */
int  re15_johnruf_r39_message_on(const uint8_t *pc);
/* scd_vm.c Opcode-Tabelle [0x5F]: RE2 Xa_vol NUR in den Programmen dieses Moduls; ueberall sonst exakt op_unknown
 * (pc += 1, Satzbreite s_opcode_sizes[0x5F] = 1, RE1.5-Tabelle 0x800744a8 endet bei 0x5E). */
int  re15_johnruf_r39_op_xa_vol(scd_thread_t *t);
/* audio_pc.c re15_voice_play: CD_VOL fuer die naechste Sprachzeile = 0x3FFF * Xa_vol / 120 (Standard = 0x3FFF). */
unsigned re15_johnruf_r39_xa_cdvol(void);
/* audio_pc.c re15_voice_load_clip: (1041|1131, Id 53..57) -> Ordner room1141, Stamm help01..05; sonst unveraendert. */
void re15_johnruf_r39_stimme(unsigned room_id, int voice_id, char *reldir, size_t nd, char *stamm, size_t ns);
/* audio_pc.c ss_bgm_entry: 0xFF01 im Panik-Fenster (STAGE1 ausser 1141), sonst -1. */
int  re15_johnruf_r39_bgm_eintrag(int stage, int room);
/* audio_pc.c SEQ_CTL: 1 = Sce_bgm_control an Slot 0 im Panik-Fenster nicht anwenden. */
int  re15_johnruf_r39_main_gesperrt(unsigned slot, int op, long cap_tick);

/* ---- Pruefhaken (Riegel, kein Spielverhalten) -------------------------------------------------------------------- */
const uint8_t *re15_johnruf_r39_programm(int nr, int *out_len);   /* 0 RUF1041, 1 RUF1131 */
const uint8_t *re15_johnruf_r39_text(int *out_len);              /* msg 40 */
int  re15_johnruf_r39_xa_vol(void);                              /* aktuelle XA-Lautstaerke (0..127) */
int  re15_johnruf_r39_fenster(void);                             /* 1 = Panik-Musikfenster offen */
int  re15_johnruf_r39_ruf1131_gespielt(void);
unsigned re15_johnruf_r39_zaehler(int k);   /* 0 Rufe 1041, 1 Rufe 1131, 2 Kartenhinweise, 3 Rettungen */

#endif /* RE15_JOHNRUF_R39_H */
