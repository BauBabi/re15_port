/*
 * RE1.5 Rebuilt — LAUTHEITS-BODEN DER SPRACHAUFNAHMEN (Runde 39 Spur A "stimmen", Nachbesserung 1).
 * Dossier mit allen Messungen: analysis/befunde_runde39/A_stimmen.md §9.
 *
 * NUTZER-VORGABE (AUFTRAG.md Nachtrag 2, 2026-10-18 09:48, woertlich): "er Port kann die Stimme ruhig lauter anheben
 * das es passt." — Irons' Zeilen der Sterbeszene ROOM1150 (main18-21) lagen mit dem alten 4x-Deckel 10-12 dB unter
 * der BGM MAIN1E (A_abnahme_0.md §4.2: Stimme rms 512-577 gegen Rest 1669-2068).
 *
 * BEFUND (Zensus 2026-10-18 ueber alle 216 Dateien synchro/STAGE<n>/room<RAUM>/main<NN>.wav, Dossier §9.1): der Bestand ist
 * zweiklassig. 206 Dateien haben einen Effektivwert (rms) von 0,034-0,092 FS bei Spitzen 0,21-0,66 FS (Crest 4,5-13,4);
 * 10 Dateien — alle Irons, room1150/main02/05/07/09/12/13/18-21 — haben rms 0,0040-0,0062 FS bei Spitzen 0,053-0,096 FS
 * (Crest 12,5-20,6: die Spitze ist ein einzelner Transient, die Sprache liegt 22-26 dB darunter). Die Spitzen-Stufe des
 * Laders (audio_pc.c re15_voice_load_clip "Level-floor": Ziel 14745 = 45 % Spitze, Deckel 4x, seit aa346af1
 * 2026-06-28 mit der Begruendung "cap the gain so near-silent clips don't blow up their noise floor") bringt genau diese
 * 10 Dateien an den Deckel und nur auf rms 0,016-0,025; zwischen den Klassen bleibt eine Luecke (0,0247 .. 0,0355),
 * keine Datei der normalen Klasse beruehrt den Deckel.
 *
 * REGEL (Stufe 2, nach der Spitzen-Stufe, auf den auf Geraeterate gerechneten 16-bit-Werten):
 *   Liegt der Effektivwert einer Datei unter T, wird sie auf den Effektivwert R angehoben, hoechstens aber so weit,
 *   dass ihre Spitze C nicht ueberschreitet (Spitzenbegrenzung: kein einziger Abtastwert wird geklemmt). Dateien mit
 *   rms >= T bleiben bitgleich. Alle drei Zahlen sind Werte der 16-bit-Skala, so wie dieser Code sie rechnet
 *   (ganzzahlig, Riegel unit_r39_stimmen_pegel druckt sie am Bestand mit).
 *   T = 985   (0,0301 FS) = Mitte der Luecke zwischen den beiden Klassen des Bestands: lauteste Datei der leisen
 *                           Klasse nach Stufe 1 rms 808 (room1150/main05, 0,0247 FS), leiseste Datei auf Spitzenpegel
 *                           rms 1162 (room2000/main02, 0,0355 FS) — je 18 % Abstand zu beiden; eine Schwelle AUF dem
 *                           Rand (1163, erste Herleitung) kippte um ein LSB und hob room2000/main02 mit an.
 *   R = 2040  (0,0623 FS) = Median des Effektivwerts der 206 Dateien auf Spitzenpegel: die Mitte des Bestands, so wie
 *                           das Ziel der Stufe 1 ("~45% full-scale, matches the loud clips") die Mitte der Spitzen ist.
 *   C = 21759 (0,664 FS)  = Spitze der lautesten gelieferten Datei (room1240/main05.wav): kein angehobener Clip wird
 *                           in der Spitze lauter als das, was der Port heute schon ungeklemmt mit BGM mischt (der
 *                           Mischer audio_pc.c :482-486 summiert die Stimme mit cd_vol 0x3FFF>>14 = 1,0 und klemmt
 *                           erst die Summe; BGM-Spitzen in den Zeilenluecken der Sterbeszene 0,105-0,276 FS, §9.2).
 *   Ergebnis am Bestand (Riegel unit_r39_stimmen_pegel): genau die 10 Irons-Dateien werden angehoben, alle laufen in
 *   die Spitzenbegrenzung (Gesamtfaktor 6,9-12,6x statt 4x; main18 12,6x, main19 8,9x, main20 7,4x, main21 11,6x),
 *   die 206 anderen Dateien bleiben bitgleich (Leon, Ada, Marvin, John, Elza: Stufe 1 wie bisher; Tabellenzeilen 1,00-1,97x, im Bestand bis 3,00x bei den 48-kHz-Dateien room1170/main00-03 - Abnahme 1 §3).
 *   Rauschteppich: Irons' Dateien haben zwischen den Worten 0,00046-0,00061 FS (10. Perzentil der 20-ms-Fenster),
 *   dasselbe absolute Niveau wie Leons Dateien; angehoben liegt er bei 0,0034-0,0077 FS (-49 .. -42 dBFS), in der
 *   Sterbeszene 16-20 dB unter der BGM (rms 0,05-0,08 FS). Das ist der Preis der NUTZER-VORGABE, gemessen, nicht geraten.
 */
#ifndef RE15_STIMMEN_PEGEL_R39_H
#define RE15_STIMMEN_PEGEL_R39_H

#include <stdint.h>

#define RE15_STIMMEN_PEGEL_T 985     /* 0,0301 FS: Mitte der Luecke 808 (room1150/main05) .. 1162 (room2000/main02), Zensus 2026-10-18 */
#define RE15_STIMMEN_PEGEL_R 2040    /* 0,0623 FS: Median rms der 206 Dateien auf Spitzenpegel, Zensus 2026-10-18                      */
#define RE15_STIMMEN_PEGEL_C 21759   /* 0,664 FS: Spitze der lautesten gelieferten Datei (room1240/main05), Zensus 2026-10-18           */

typedef struct {
    int rms_vorher, rms_nachher;        /* Effektivwert in 16-bit-Skala, vor / nach Stufe 2          */
    int spitze_vorher, spitze_nachher;  /* groesster |Abtastwert|, vor / nach Stufe 2                 */
    int faktor_q8;                      /* angewandter Faktor Q8 (256 = unveraendert)                 */
    int begrenzt;                       /* 1 = die Spitzenbegrenzung C hat den Faktor unter R/rms gekappt */
} re15_stimmen_pegel_t;

/* Stufe 2 auf pcm[0..n) anwenden. Liefert den Faktor Q8 (256 = unveraendert, dann ist pcm bitgleich).
 * stamm: Dateistamm fuer die Logzeile "[voice] pegel ..." (PC: stderr = debug.log), darf NULL sein.
 * info: Messwerte (darf NULL sein). */
int re15_stimmen_pegel_r39(int16_t *pcm, long n, const char *stamm, re15_stimmen_pegel_t *info);

#endif /* RE15_STIMMEN_PEGEL_R39_H */
