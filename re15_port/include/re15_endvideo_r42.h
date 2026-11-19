/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 5: "THANK YOU FOR PLAYING" raus, Platz fuer das Endvideo (Leon 5090, Elza 5091).
 *
 * NUTZERWUNSCH 18.11.2026 (woertlich, zum Ablaufvorschlag "direkt nach dem Raketentreffer ... Danach kommen 'THANK YOU FOR PLAYING'
 * und der Abspann"): "Passt nicht - das thank you for Playing fliegt vollständig raus, und es kommt ein richtiges Endvideo. Der Rest
 * klingt erst einmal gut."
 *
 * WAS DAS ORIGINAL (Mod-Baum) HIER TUT: ROOM5090/5091 sub01 sieht im Bild nach dem Kill (5,0x1C) (5090 @0x012A6, 5091 @0x0128A),
 * haelt SUB an, spielt MAIN (@0x012AE/@0x012B4) und startet sub02 (@0x012BA, 5091 @0x0129E) = Abspann-Vorspann:
 *   5090 @0x012C2 / 5091 @0x012A6  `22 02 07 01` Set(2,7,1)   Pad-Sperre
 *   5090 @0x012C6 / 5091 @0x012AA  `22 01 1b 01` Set(1,27,1)  Kinobalken
 *   5090 @0x012CA / 5091 @0x012AE  `09 0a 14 00` Sleep 20
 *   5090 @0x012CE / 5091 @0x012B2  `2b 00 00 00` Message_on 0 = "THANK YOU FOR PLAYING !!!" (5090 Text @0x1360, 5091 @0x1344)
 *   5090 @0x012D2 / 5091 @0x012B6  `09 0a 64 00` Sleep 100 = Standzeit der Meldung
 *   5090 @0x012D6 / 5091 @0x012BA  `47 05`       Aot_on(5) -> Door_aot_set Slot 5 (5090 @0x010EE) -> ROOM6040 / ROOM6041 (Diashow)
 *   5090 @0x012D8 / 5091 @0x012BC  `01 00`       Evt_end
 * Die 24 Bytes sind in beiden Raeumen gleich (selbst gelesen, xxd), deshalb gilt EIN Port-Programm fuer beide.
 *
 * WAS DER PORT DARAUS MACHT (Port-Programm k_ende in engine/src/endvideo_r42.c, gestartet statt sub02, sobald das Finale beendet
 * ist = Bank 9 Bit 129 RE15_R42_BIT_FINALE_ENDE; ohne das Bit bleibt sub02 gestrichen wie bisher, scd_vm.c op_evt_exec):
 *   Set(2,7,1), Set(1,27,1), Sleep 20                    Original-Bytes @0x012C2..@0x012CD (Leon) / @0x012A6..@0x012B1 (Elza);
 *                                                        davor MAIN anhalten (RE2 @0x02804, Lage PORT-WAHL "MUSIK" unten)
 *   [Message_on 0 und Sleep 100 entfallen]                NUTZERWUNSCH 18.11. (oben)
 *   Abblenden, Musik anhalten, Film, Einblenden          RE2-Form eines Films am Ende einer Szene, ROOM4090 (unten)
 *   Aot_on(5), Evt_end                                   Original-Bytes @0x012D6 / @0x012BA -> Diashow 6040/6041, Titel wie bisher
 *
 * DIE RE2-FORM (info/re2leon/PL0/RDT/ROOM4090.RDT, selbst gelesen mit xxd @0x027F0..@0x02831; Opcode-Handler in ghidra_re2_Leon.txt):
 *   @0x027F4 `53 02 02 07 00 02`  Sce_fade_set: Kanal 2 (pc[1], `lbu s0,1(s2)` @0x80057f18), ABR 2 = B-F subtraktiv (pc[2] @0x80057f38),
 *                                  RGB 7 (pc[3] @0x80057f3c), Wert +0x0200 (`lhu s3,4(s2)` @0x80057f1c) = Schritt je Bild;
 *                                  Wert > 0 -> Pegel 0 (`bgtz s1` @0x80057f90 -> `sh zero,0(v1)` @0x80057fa4) = ABBLENDEN in
 *                                  0x8000 / 0x200 = 64 Bildern (Helligkeit = Pegel >> 7, Takt FUN_8002c378)
 *   @0x027FA `51 02 04 00 64 40`  SUB anhalten;  @0x02800 `09 0a 01 00` Sleep 1
 *   @0x02804 `51 00 04 01 33 41`  MAIN anhalten (Fall 4 SsSeqPause, RE2 FUN_8005b6f0 @0x8005b938)
 *   @0x0280A `09 0a 3e 00`        Sleep 62
 *   @0x0280E `29 0a`              Cut_chg 10 (die Kamera nach dem Film; im Port gibt es danach keine Szene im Raum -> entfaellt)
 *   @0x02810 `02`                 Evt_next (+ @0x02811 `00` Nop)
 *   @0x02812 `53 02 02 07 00 fe`  Sce_fade_set Wert -0x200: Pegel = Wert + 0x8000 (`ori v0,zero,0x8000` @0x80057f94, `addu` @0x80057f98,
 *                                  `sh` @0x80057fa0) = 0x7E00, Schritt -0x200 -> haelt das Bild schwarz, bevor die Abblende bei
 *                                  0x8000 ausliefe (Bit 15 = Kanal aus, `bltz` @0x8002c3b4), und blendet nach dem Film wieder ein
 *   @0x02818 `6f 0b`              Movie_on 11 (Tabelle @0x800a7684 -> LAB_80058d64; gespielt im Bildwechsel FUN_8002b968 @0x8002ba28)
 *   @0x0281A `02`                 Evt_next (+ @0x0281B `00` Nop)
 *   @0x0281C `74 02 ff 7f`        Sce_fade_adjust (LAB_80057fd8, Tabelle @0x800a7698): Kanal pc[1] @0x80057ff4, `lh a1,2(s0)` @0x80057ff8
 *                                  -> FUN_8002c2b0 `sh a1,0(a0)` @0x8002c2d4 = Pegel 0x7FFF -> nach dem Film 64 Bilder aus Schwarz
 *   @0x02820 `09 0a 3f 00`        Sleep 63; @0x02824 / @0x0282B SUB / MAIN weiter (Fall 3 SsSeqReplay)
 * Die zweite RE2-Form (ROOM1080 sub14 @0x0252A, Commit bddc94eec, re15_zws_r41.h "UEBERBLENDUNG WIE RE2") ist der Film MITTEN in
 * einer laufenden Szene: harter Schnitt hinein, 15 Bilder Einblenden. Das Endvideo beendet die Szene -> Form ROOM4090.
 * Querbezug: der Strang r42/birkinfilm (nicht in diesem Zweig) baut dieselbe ROOM4090-Form fuer den Film "Birkins Mutation" in C an
 * der Film-Zeile (RE15_ZWS_BLENDART_4090 = 1, Zaehlung dort: Abblende F0..F63, Film in F64 - gleich der Messung hier). Das Endvideo
 * blendet stattdessen im Skript (RE15_ZWS_BLENDART_SKRIPT = 2), weil das Abblenden auch ohne Film-Datei laufen muss.
 *
 * UMSETZUNG IN RE1.5-OPCODES (gleiche Wirkung, Belege der RE1.5-Seite in include/re15_fade.h und annette_r41.c "Musik unter dem Zugfilm"):
 *   Sce_fade_set = 0x56 Konfiguration (Handler @0x80042a58 -> FUN_800217b0, Ebene 7 `ori a3,7` @0x80042a70) + 0x57 Start
 *     (@0x80042ab4 -> FUN_800216ec): bei Schritt != 0 Pegel 0 (Schritt > 0) bzw. 0x7FFF (Schritt <= 0), @0x80021710-20.
 *     -> RE2 @0x027F4 = `56 02 02 07 00 02` + `57 02 00 00` (Pegel 0, gleich RE2).
 *     -> RE2 @0x02812 = `56 02 02 07 00 fe` + `57 02 00 00` (Pegel 0x7FFF statt RE2 0x7E00: Helligkeit 255 statt 252 im einen Bild
 *        vor dem Film; der RE1.5-Start kennt keinen anderen Startwert - Abweichung um 3 Stufen in einem schwarzen Bild).
 *     -> RE2 @0x0281C Sce_fade_adjust 0x7FFF = `57 02 00 00` bei Schritt -0x200 (Pegel 0x7FFF, gleich RE2).
 *   MAIN anhalten = 0x54 Fall 4 SsSeqPause @0x80044efc, in der Form der Raumzeile 5090 sub01 @0x012B4 / 5091 @0x01298
 *     `54 00 01 00 00 00` mit Op 4 (wie Strang finale Stufe 2 im Zugfilm). SUB laeuft hier nicht mehr (sub01 @0x012AE stoppt ihn),
 *     also nur MAIN. Kein "weiter" danach: der Raum wird verlassen, die Diashow startet MAIN selbst (6040 sub02 @0x0052A `54 00 01`).
 *   MUSIK, PORT-WAHL der Lage: RE2 haelt MAIN ein Bild nach dem Abblend-Start an (@0x02800 Sleep 1, @0x02804). Hier steht das
 *     Anhalten schon vor Sleep 20 @0x012CA, im ersten Takt von sub02. Grund, gemessen (Strang-Scratchpad s5r1, Video Leon mit
 *     RE2-Lage): sub01 @0x012B4 startet MAIN im Kill-Bild neu (die Mod liess die Raummusik unter "THANK YOU" laufen); mit der
 *     RE2-Lage klang sie 21 Bilder = 0,7 s an (-35..-30 dB) und brach dann ab. Mit dem Anhalten im selben Takt wie der Start
 *     (sub01 Slot 1 vor sub02 Slot 10 im selben VM-Takt) ist nach dem Todesschrei Stille, wie in RE2 waehrend der Abblende.
 *   Movie_on = Port-Zeile `2b 3c 00 00` (Message_on 60 ohne Text) -> Haken re15_zws_r41_message_on, Tabellenzeilen "ende5090" /
 *     "ende5091" in engine/src/zws_r41.c: Datei da -> spielen (Skript parkt), Datei fehlt / keine Plattform -> sofort weiter.
 *     Die Tabellenzeile traegt RE15_ZWS_BLENDART_SKRIPT: das C-Einblenden der Form ROOM1080 entfaellt, die Blende macht das Skript.
 *   Evt_next = 0x02 (scd_vm.c op_evt_next), Sleep = `09 0a <n16>`.
 *   Id 60: ROOM5090 belegt 0, 1 (RDT), 2 (Schlusszeile), 3 (Zuruf), 40..43 (Annette); ROOM5091 0, 1, 2, 3 -> 60 ist frei (dieselbe Id
 *     wie der Zugfilm in 6030/6031).
 *
 * DAS ENDVIDEO SELBST: noch nicht gebaut (Ablauf scratchpad zws_ende/BEDARF_Ende.md). Dateien je Figur (PORT-WAHL Name, 8.3 wie die
 * uebrigen Filme): MOVIE/ENDE5090.STR (Leon) und MOVIE/ENDE5091.STR (Elza), Format wie R600ZUG.STR (tools/r41_zws114/film_bauen.py
 * --quelle <mp4> --ziel <STR>). Solange die Datei fehlt, geht es nach dem Abblenden direkt in den Abspann.
 */
#ifndef RE15_ENDVIDEO_R42_H
#define RE15_ENDVIDEO_R42_H

#include <stdint.h>

#define RE15_EV_RAUM_LEON    0x5090u
#define RE15_EV_RAUM_ELZA    0x5091u
#define RE15_EV_SUB          0x02u      /* sub02 = Abspann-Vorspann (5090 sub01 @0x012BA `04 ff 18 02`, 5091 @0x0129E) */
#define RE15_EV_MSG_FILM     60         /* Port-Zeile `2b 3c 00 00` (Id frei, s. oben)                                   */

/* Blende (RE2 ROOM4090, s. oben). */
#define RE15_EV_BLENDE_KANAL  2         /* @0x027F5 / @0x02813 pc[1]                                                     */
#define RE15_EV_BLENDE_ABR    2         /* @0x027F6 pc[2] subtraktiv                                                    */
#define RE15_EV_BLENDE_RGB    7         /* @0x027F7 pc[3]                                                               */
#define RE15_EV_ABBLENDEN     0x0200    /* @0x027F8 `00 02` Schritt je Bild -> 64 Bilder bis Schwarz                    */
#define RE15_EV_EINBLENDEN   (-0x0200)  /* @0x02816 `00 fe`                                                             */
#define RE15_EV_SCHLAF_1      1         /* @0x02800 `09 0a 01 00`                                                       */
#define RE15_EV_SCHLAF_2      62        /* @0x0280A `09 0a 3e 00`                                                       */

/* Original-Bytes von sub02 (beide Raeume gleich) und Datei-Offsets der Pruefung. */
#define RE15_EV_SUB02_LEON    0x012C2u
#define RE15_EV_SUB02_ELZA    0x012A6u
#define RE15_EV_SUB02_LAENGE  24
extern const uint8_t g_re15_ev_sub02_original[RE15_EV_SUB02_LAENGE];

/* Das Port-Programm (Riegel test_r42_finale_abspann liest es). */
extern const uint8_t g_re15_ev_programm[];
extern const unsigned g_re15_ev_programm_laenge;

/* HAKEN op_evt_exec (scd_vm.c, hinter der Streichliste): in 5090/5091 sub02 bei gesetztem Bit 129 das Port-Programm statt des
 * Original-sub02, sonst pc unveraendert. Das Original-sub02 muss die 24 Bytes oben tragen (anderer Stand -> Original). */
const uint8_t *re15_endvideo_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc);
extern unsigned g_re15_ev_starts;   /* Pruefhaken: so oft wurde das Port-Programm gestartet */

#endif /* RE15_ENDVIDEO_R42_H */
