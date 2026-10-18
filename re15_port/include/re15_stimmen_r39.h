/*
 * RE1.5 Rebuilt — SPRACHAUFNAHMEN DER PORT-SZENEN (Runde 39 Spur A "stimmen").
 * Dossier mit allen Messungen: analysis/befunde_runde39/A_stimmen.md.
 *
 * NUTZER-PUNKTE (AUFTRAG.md, Wortlaut):
 *   1 "In Room 1050 wird das Voiceover nicht abgespielt von vor dem Retten von Ada - das ich unter main09-main11
 *      abgelegt habe. Also die Voiceovers für unsere neue Cutscene"
 *   2 "Die Voiceovers für die letzte Cutscene in ROOM 1150, wenn Irons stirbt sind nicht drin...."
 *   3 "In ROOM 11B0 bei der finalen Cutscene mit Ada und Marvin, fehlt das neue Voiceover von But hold on und I want
 *      to make sure we don't have any unexpected Visitors. Was dir eigentlich vorliegt."
 *   4 "Bei "Alright, let's get down from here" fehlt aus irgendeinen Grund mal wieder das Voiceover."
 *
 * URSACHE (gemessen, echte exe, Dossier §3): die Sprachausgabe sucht synchro/STAGE<n>/room<RAUM>/main<MSG-ID>.wav
 * (audio_pc.c re15_voice_load_clip, Regel synchro/README.md). Die neuen Szenen sprechen mit PORT-Nachrichten-Ids
 * (1050: 22-24 adaruf_1050.c, 1150: 22-29 irons_tod_1150.c, 11B0: 20-21 gen/szl11b0_szene.inc) bzw. in ROOM1171
 * mit Original-Id 3; der Nutzer hat seine Aufnahmen mit der NAECHSTEN FREIEN DATEINUMMER des Ordners in
 * Szenenreihenfolge abgelegt (e2cc7673, 2026-10-16). Keine Datei heisst wie die Id ihrer Zeile -> stumm.
 *
 * NACHTRAG 2 (2026-10-18 09:48, Nachbesserung 1): ROOM1150 msg 29 "Leon: SIR, Sir?!" <- main22.wav (Nutzer: "das SIR,
 * sir! habe ich vergessen und jetzt mit main22 ergänzt"); die Pegel-Anhebung ueber 4x hinaus steht in
 * re15_stimmen_pegel_r39.h (Stufe 2 des Laders).
 *
 * ZUORDNUNG = NUTZER-VORGABE (seine Dateien, seine Reihenfolge). Gemessen je Datei (Dossier §1): Windows-
 * Spracherkennung (Diktat + Wahl unter den Raum-Saetzen) und MFCC/DTW gegen seine Roh-Exporte
 * synchro/unused/STAGE1/room<RAUM>/MiniMax_<Zeit>_<Sprecher>.wav (gleicher Satz 1,26-1,85, anderer >= 2,2).
 *
 * FORM = RE2 RETAIL (Beta -> Retail; RE1.5 hat keine englische Sprachausgabe): RE2 waehlt die Stimme NICHT ueber
 * die Nachrichtennummer, sondern mit einem eigenen Skriptbefehl und eigener Nummer —
 *   xa_on (0x59) Handler @0x80058028: @0x80058044 `lhu a1,2(v0)` (Stimmnummer = eigener Operand),
 *     @0x80058048 `jal 0x800129b4` (XA-Start); message_on (0x2B) Handler @0x80054A8C: @0x80054aa0 `lbu a2,2(v0)`,
 *     @0x80054ac0 `jal 0x8002fe38` (nur Text).
 *   Zensus info/re2leon/PL0/RDT: 243 Paare xa_on -> message_on, nur 8 mit gleicher Nummer; z.B. ROOM1010 MAIN sub22
 *   @0x02380 `59 00 0a 00` (Stimme 10) -> @0x02384 `2b 00 04 00 00 00` (Text 4).
 *   -> eine Tabelle (Raum, Nachricht) -> Aufnahme ist die RE2-Form; die Ordnerregel bleibt der Normalfall.
 *
 * SPERRE (gemessen, Dossier §1/§3): eine Datei, die laut Tabelle einer ANDEREN Zeile gehoert, spielt nicht mehr
 * unter ihrer eigenen Nummer —
 *   ROOM11B0 msg 13 "It doesn't seem like you could move / this on your own." (Message_on sub03 @0x0126E/@0x01298
 *     `2b 0d ff ff`) spielte seit e2cc7673 Marvins "But, hold on..." (main13.wav); msg 14 (Text-Platz 7 main00
 *     @0x0106C) gehoert ebenso nicht zu main14.wav;
 *   ROOM1171 msg 2 "C'mon, give me a hand!" (vom Skript nie gezeigt: Message_on nur sub02 @0x014BA/0x0151E/0x01586
 *     = 0/1/3) ist nicht main02.wav ("Alright, let's get down from here!").
 */
#ifndef RE15_STIMMEN_R39_H
#define RE15_STIMMEN_R39_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t raum;        /* gepackte Raum-Id wie der Ordnername (0x1150 -> room1150) */
    uint8_t  nachricht;   /* Message-Id, die die Szene zeigt (Message_on pc[1])        */
    uint8_t  datei;       /* Nummer der Nutzer-Datei main<datei>.wav                    */
} re15_stimmen_r39_t;

/* Tabelle (fuer Tests/Werkzeuge). */
int re15_stimmen_r39_anzahl(void);
const re15_stimmen_r39_t *re15_stimmen_r39_eintrag(int i);

/* Dateistamm fuer (Raum, Nachricht) in `stamm` (Groesse n):
 *    1 = die Zeile hat eine umgeordnete Nutzer-Aufnahme, `stamm` = "main<datei>";
 *    0 = Regel synchro/README.md (main<Nachricht>), `stamm` unveraendert;
 *   -1 = gesperrt: main<Nachricht>.wav gehoert laut Tabelle zu einer anderen Zeile -> diese Nachricht bleibt stumm.
 * Ein Aufruf mit stamm == NULL liefert nur das Ergebnis. */
int re15_stimmen_r39_stamm(unsigned room_id, int msg_id, char *stamm, size_t n);

/* Messschiene (PC: stderr = debug.log): fuer (Raum, Nachricht) wurde keine Datei gefunden. */
void re15_stimmen_r39_fehlt(unsigned room_id, int msg_id, const char *pfad);

#endif /* RE15_STIMMEN_R39_H */
