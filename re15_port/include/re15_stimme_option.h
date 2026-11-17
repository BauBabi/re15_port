/* re15_stimme_option.h — Runde 41 Stimmen: die Port-Option "VOICE  ON|OFF" im OPTIONS-Schirm (Standard ON).
 *
 * NUTZER-VORGABE (17.11.2026, woertlich): "Erstelle mir bitte zusaetzlich im Optionsmenu eine Option die Voiceovers an
 * oder ab zu schalten."
 *
 * ⛔ KEIN Original-Verhalten — eine Port-Option wie OPTIONS->AI und ULTIMATE 1.5. RE1.5 hat keine englische
 * Sprachausgabe (scd_vm.c scd_queue_voice: "synchro/ ist eigene Produktion des Projekts"); die Option schaltet nur
 * diese Aufnahmen. Sie aendert keine Regel, keine Zahl, keinen Ablauf des Originals.
 *
 * WAS "Stimme" HEISST (Zensus aller Tonquellen, Bericht stimmen_option/BERICHT_r1.md):
 *   AUS bei OFF — alle Aufnahmen aus synchro/, die ueber den Sprachkanal laufen (audio_pc.c re15_voice_play, EIN Weg):
 *     Message_on-Zeilen jeder Szene (SCD_AUDIO_VOICE_ON, synchro/STAGEn/roomXXXX/mainNN.wav), samt der Umleitungen
 *     Irons' Rekorder (r38), Elza-Montage 1241 <- 1240 (r38), Port-Szenen-Reihenfolge (r39).
 *   BLEIBEN bei OFF (Musik und Geraeusche):
 *     - NUTZERWUNSCH-Auslegung (Entscheid 17.11.2026, "Voiceover" = gesprochene Dialogzeilen MIT Untertitel): Laute ohne
 *       Untertitel sind Hinweise bzw. Geraeusche -> re15_stimme_option_ist_geraeusch: Johns Hilferufe help01..05
 *       (1141/1041/1131 Id 53..57, r39), Johns Ruf 1141 main42 (r38), Annettes Schrei 5090 main41 (r41), das
 *       Stoehnen main16.5 (1011 Id 24, r36);
 *     - der Tuerton door_sound.wav in ROOM1141 (Sprach-Id 59, re15_dining_r39.h RE15_R39J_TUERTON_ID, Nutzer-Nachtrag 17
 *       Punkt 5): er laeuft nur technisch ueber den Sprachkanal, ist aber ein Geraeusch -> re15_stimme_option_spielt;
 *     - RE2-XA der G5-Szene (R704_XA0B/0C.XAS, re15_audio_re2_xa) = Birkin-Laute (G5_auftritt.md M5), kein Dialog;
 *     - SE-Baenke (auch RE2-Gegnerlaute, Leons Schmerzlaute, der Titel-Ansager CORE11 SE 0), BGM, Ambience;
 *     - Film-Ton (STR/FMV): Sprache und Musik liegen dort in EINER Spur, sie laesst sich nicht trennen.
 *
 * WIRKUNG BEI OFF: re15_voice_play kehrt vor dem Laden zurueck, genau wie wenn es fuer (Raum, Id) keine Datei gibt.
 * Damit startet der Sprachkanal s_xa nie, g_re15_voice_laeuft bleibt 0, und jeder Wartende (op_message_on voice_wait,
 * Untertitel-Nachhall msg_common.c, Rekorder r38, Kanister 2080 r41) laeuft mit der Textdauer weiter — derselbe Weg
 * wie eine Zeile ohne Aufnahme. Umschalten auf OFF waehrend einer laufenden Aufnahme beendet sie sofort (Hook der
 * Plattform, unter Mixer-Sperre); der Untertitel bleibt, weil die Nachricht nicht an der Stimme haengt.
 *
 * DATEI: re15_options.cfg (re15_lautstaerke_r41.c), Zeile `voice=off` NUR bei OFF — bei ON (Standard) bleibt die Datei
 * Byte fuer Byte wie bisher (Muster ultimate=on). Fehlt die Zeile -> ON. Messhaken RE15_VOICE=0|1 uebersteuert beim
 * Laden (nur Harness/Tests). */
#ifndef RE15_STIMME_OPTION_H
#define RE15_STIMME_OPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_STIMME_SCHLUESSEL  "voice="     /* Zeile in re15_options.cfg (nur bei OFF geschrieben)         */
#define RE15_STIMME_ENV         "RE15_VOICE" /* Messhaken: 0/1 uebersteuert die Options-Datei beim Laden    */

int  re15_stimme_option(void);               /* 1 = ON (Standard), 0 = OFF                                   */
/* 0/1. Wechsel ON -> OFF ruft den Stopp-Hook der Plattform (laufende Aufnahme endet). */
void re15_stimme_option_set(int an);
/* Plattform: beendet eine laufende synchro-Aufnahme (PC: audio_pc.c re15_audio_stimme_stoppen). */
void re15_stimme_option_stopp_hook(void (*fn)(void));

/* Die Datei zu (Raum, Sprach-Id) ist ein GERAEUSCH/Laut ohne Untertitel, das nur technisch ueber den Sprachkanal laeuft
 * (Tuerton 1141/59, Hilferufe, Johns Ruf, Annettes Schrei, Stoehnen main16.5). */
int  re15_stimme_option_ist_geraeusch(unsigned room_id, int voice_id);
/* Darf der Sprachkanal (Raum, Id) jetzt spielen? ON -> immer; OFF -> nur Geraeusche. */
int  re15_stimme_option_spielt(unsigned room_id, int voice_id);

/* Options-Datei: 1 fuer "voice=on", 0 fuer "voice=off", -1 wenn der Schluessel fehlt. */
int  re15_stimme_option_parse(const char *text);
/* Beim Laden der Options-Datei: Wert aus dem Text (fehlt er -> ON), danach der Messhaken RE15_VOICE. */
void re15_stimme_option_aus_text(const char *text);

#ifdef __cplusplus
}
#endif
#endif /* RE15_STIMME_OPTION_H */
