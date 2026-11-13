/*
 * RE1.5 Rebuilt — Runde 41, Zwischensequenz Raum 114 (Besprechungsraum, Elzas Szenario ROOM1141, John und Elza).
 *
 * NUTZER-VORGABE (woertlich, 2026-11-13): "Ich wuerde im Dialog bei John und Elza vor 'What was that?' diesen Schrei-Sound
 * von dem Gorilla einbauen, aus 11C Sekunde 6. Und dann kurz warten, bevor ich das Video abspiele. Nach dem Video geht es
 * normal weiter mit 'Shit! that is what I was talking about!'". Nachtrag Koordinator: gemeint ist "der Auf-die-Brust-
 * klopfen-Sound" -> vorlaeufig Satz 4, an EINER Stelle austauschbar (RE15_ZWS114_SCHREI_SATZ); Pause = Tonende + kurz.
 *
 * WO DER DIALOG LIEGT: ROOM1141.RDT sub02, msg 6 "Elza: What was that?!" @0x00DCA `2b 06 00 00`, msg 7 "John: Shit! that
 * is what I was / talking about!" @0x00E14 `2b 07 00 00` (Nachrichten @0x01243 / @0x01262, tools/rdt_msgdump.py).
 * ROOM1140 (Leon) hat diesen Dialog nicht (msg 0..5: Schloss, Gemaelde, Tisch, "The streets are infested ...").
 * Im Port laeuft die Szene als Programm ENTRIEGELN39 (Runde 39 Spur J, include/re15_dining_r39.h; Original-Bytes von
 * sub02 ab @0x00CD4 mit eingefuegten Zeilen, tools/r39_dining/programme_bauen.py -> engine/src/gen/dining_r39_programme.inc).
 * Zwischen msg 6 und msg 7 stehen im Original: Plc_neck Elza @0x00DD2, Sleep 5 @0x00DDC, Plc_neck John @0x00DE4, Sleep 5
 * @0x00DEE, Plc_dest TURN Elza @0x00DF6, Sleep 5 @0x00DFE, Plc_dest TURN John @0x00E06, Gosub sub04 @0x00E0E (wartet, bis
 * John steht), Sleep 86 @0x00E10 `09 0a 56 00` - beide drehen sich zum Fenster, dann Pause.
 *
 * ABLAUF (eingefuegt in ENTRIEGELN39, Message_on-Form `2b <id> 00 00` wie die Port-Warte-Zeile 58 der Runde 39):
 *   ... msg 5, Message_on 58 (Johns Stimme zu msg 5 ausreden lassen, Runde 39)
 *   Message_on 61 = SCHREI: Gorilla-Ton, das Skript parkt bis zum Tonende (Laenge aus der Tonbank, s. u.)
 *   Sleep 20 @0x00DC6 `09 0a 14 00` = Original-Takt vor der Zeile = die "kurze Pause" nach dem Ton
 *   msg 6 "What was that?!" - Drehen zum Fenster, Sleep 86 @0x00E10 (Original) = "kurz warten"
 *   Message_on 60 = FILM: R114GORI.STR (Film des Nutzers), das Skript parkt, bis die Plattform ihn gespielt hat
 *   msg 7 "Shit! that is what I was talking about!" und weiter wie bisher.
 * Der Glasbruch der Runde 39 (Punkt 6, Se_on(0x3E,0x21) x2 vor msg 6) entfaellt an dieser Stelle: der Film zeigt das
 * Zerspringen der Scheiben selbst (Bild 278/284, Ton im Film); zweimal Glas waere ein Widerspruch. Der Satz 0x21 und
 * sein Lader bleiben (re15_dining_r39.h RE15_R39J_SATZ_GLAS).
 * Beide Zeilen haben keinen Text; ihr Haken steht in op_message_on HINTER dem Stimmen-Riegel (scd_vm.c "DEN VORIGEN SATZ
 * AUSREDEN LASSEN"), also beginnt der Schrei erst, wenn Johns Zeile 5 verklungen ist, und der Film erst nach Elzas Zeile 6.
 * PSX-Fassung des Ports: keine Plattform-Haken -> beide Zeilen laufen ohne Ton/Film durch.
 *
 * DER SCHREI: Raum-Tonbank snd1 von ROOM11C0 (Gorilla-Raum; RDT-Kopf +0x14/+0x18/+0x1C = EDT/VH/VB, rdt_common.c), auf der
 * 40-%-Disc bytegleich. Der Gorilla (Typ 0x27, STAGE1.BIN) spielt seine Toene nur ueber FUN_800453d0 (Raum-SE, snd1):
 *   Satz 4: @0x8011793c `lbu v0,7(v1)` / @0x80117944 bne (+0x7 == 0) / @0x8011794c `lbu v1,149(v1)` / @0x80117954 bne
 *           (+0x95 == 0x1d) / @0x8011795c `jal 0x800453d0` / @0x80117960 `ori a0,zero,0x4` = Clip 3 (aufrichten,
 *           Brusttrommeln) Bild 0x1d - ein Ruf/Schrei von 1,23 s (Messung scratchpad ton/NOTIZ.md Abschnitt 2).
 *   Satz 2: @0x80118c60 `ori a0,zero,0x2` / @0x80118c80 `jal 0x800453d0` = Landung nach dem Sprung (0,51 s).
 * Gespielt wird der Satz wie jeder Raum-SE (audio_pc.c re15_audio_se_play_bank -> se_play_layers: Ton-Pegel/-Pan, Tonhoehe
 * note2pitch2). Die Wartezeit = laengste Lage des Satzes in SCD-Takten (re15_zws114_r41_satz_takte): Proben nach dem
 * END-Flag-Abbruch (re15_vag_adpcm_decode) / (44100 * pitch / 0x1000) s (audio_pc.c: "0x1000 == 44100 Hz Ausgabe"),
 * aufgerundet auf 30 Takte/s (scd_vm.c op_sleep: "30 Hz SCD VM tick").
 *
 * DER FILM: shared_assets/PSX/MOVIE/R114GORI.STR (+ R114GORI.TXT Herkunft), gebaut von tools/r41_zws114/film_bauen.py aus
 * Cutscenes/Rooms/114/Nachbau_Blender/Raum114_Gorillas_v3_Ton.mp4 (unveraendert), Format wie MOVIE/CAPCOM.STR (MDEC BS v2
 * 320x240 + CD-XA 37800 Hz 4 Bit Stereo), 367 Bilder = 12,23 s. Die PC-Plattform spielt ihn mit dem Weg des Vorspanns
 * (re15_str.c + re15_xa.c, Ton = Takt, START ueberspringt), zwischen zwei Spielbildern (platform/pc/src/zws114_r41_pc.c).
 */
#ifndef RE15_ZWS114_R41_H
#define RE15_ZWS114_R41_H

#include <stdint.h>
#include <stddef.h>

#define RE15_ZWS114_RAUM         0x1141u
/* Port-Nachrichten ohne Text (ROOM1141: 40-42 Runde 38, 50-58 Runde 39, Sprach-Id 59 door_sound -> 60/61 frei). */
#define RE15_ZWS114_MSG_FILM     60
#define RE15_ZWS114_MSG_SCHREI   61

/* ---- EINZIGE Stelle zum Tauschen des Schreis: Satz der snd1-Bank von ROOM11C0 (4 = Ruf Clip 3, 2 = Landung) ---- */
#define RE15_ZWS114_SCHREI_SATZ  4
#define RE15_ZWS114_SCHREI_RDT   "STAGE1/ROOM11C0.RDT"
#define RE15_ZWS114_SCHREI_BANK  1        /* snd1 = Raum-SE-Bank (FUN_800453d0) */

#define RE15_ZWS114_FILM         "MOVIE/R114GORI.STR"
#define RE15_ZWS114_FILM_BILDER  367      /* Quelle: 367 Bilder bei 30/s (Riegel prueft die Datei) */

#define RE15_ZWS114_TAKTE_JE_S   30       /* scd_vm.c op_sleep: SCD-VM laeuft mit 30 Hz */

/* Zustand des Films (Riegel/Plattform). */
enum { RE15_ZWS114_FILM_AUS = 0, RE15_ZWS114_FILM_OFFEN = 1, RE15_ZWS114_FILM_GESPIELT = 2 };

/* HAKEN op_message_on (scd_vm.c, nach dem Stimmen-Riegel): 0 = nicht diese Zeile, 1 = verbraucht (pc += 4), 2 = parken. */
int re15_zws114_r41_message_on(const uint8_t *pc);
/* Installer (Ende von re15_dining_r39_install, jeder Raum): Zustand zuruecksetzen. */
void re15_zws114_r41_install(uint16_t room_id);

/* Plattform-Dienste (platform/pc/src/zws114_r41_pc.c). NULL = nicht vorhanden -> Zeile laeuft sofort durch.
 *   schrei_hook(satz): Satz spielen, Rueckgabe = Laenge in SCD-Takten (< 0 = nicht gespielt).
 *   film_spieler: != 0, wenn die Plattform den Film zwischen zwei Spielbildern spielt (re15_zws114_r41_film_offen /
 *                 re15_zws114_r41_film_gespielt). */
extern int (*re15_zws114_r41_schrei_hook)(int satz);
extern int re15_zws114_r41_film_spieler;
int  re15_zws114_r41_film_offen(void);      /* 1 = Film angefordert, Skript parkt */
void re15_zws114_r41_film_gespielt(void);   /* Plattform: Film zu Ende (oder uebersprungen/fehlt) */
int  re15_zws114_r41_film_zustand(void);

/* Laenge des Satzes `satz` einer Raumbank in SCD-Takten (laengste Lage, aufgerundet), < 0 = Satz leer/Bank defekt. */
int re15_zws114_r41_satz_takte(const uint8_t *edt, const uint8_t *vh, int vh_size, const uint8_t *vb, int vb_size,
                               int satz);

/* Pruefhaken (Riegel test_r41_zws114). */
int re15_zws114_r41_schrei_rest(void);   /* > 0: Skript parkt noch so viele VM-Takte auf dem Schrei */
extern unsigned g_re15_zws114_schreie;   /* gestartete Schreie (Message_on 61) */
extern unsigned g_re15_zws114_filme;     /* angeforderte Filme (Message_on 60) */
extern int      g_re15_zws114_schrei_takte;   /* Wartezeit des letzten Schreis laut Hook */

#endif /* RE15_ZWS114_R41_H */
