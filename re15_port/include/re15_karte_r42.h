/* =============================================================================
 * r42/karte Stufe 3 Runde 2 — Positionsmarker auf die GEMALTE Kachel (STAGE2-6, Blatt 6..12)
 * =============================================================================
 * Nutzer 2026-11-18: "das die [Map] fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1 ...
 * Inklusive dem - wo befindet sich Leon - Marker."
 *
 * Befund (Pruefbericht Stufe 3 Runde 1 + Vorpruefung an ECHTEN Ankunftslagen, Door_aot_set-Spawn):
 * re15_inv_map_marker klemmt den Marker nur ins RECHTECK der Kachel (Rand-Reserve 4). Die gemalte Kachel
 * darin ist oft nicht rechteckig (Memory reai-v2-bbox-ist-nicht-kunst). An 6 von 213 Ankunftslagen in
 * STAGE2-6 stand der Marker deshalb 3-8 px neben der eigenen Kachel: ROOM6000 Ankunft aus 50B0 (der Gang
 * nach Norden ist auf Blatt 12 r5 nicht gemalt), 4040 aus 4050, 4050 (zwei Lagen), 4080 aus 4090 (Ankunft an
 * der Tuer, Punkt auf der Nachbarkachel), 11A0 aus 3000 (Gast-Zeichnung Blatt 6).
 *
 * PORT-WAHL (kein Original-Vorbild: RE1.5 klemmt gar nicht, die Formel FUN_800473f8 @0x8004741c-0x80047528
 * schreibt den Punkt ungeprueft; fuer 39 von 72 Raeumen ist ihre Zeile ein Stub {0,0,1,1}): nach der
 * Rechteck-Klemmung wird der Punkt, falls er nicht auf dem RAUMKOERPER der EIGENEN Kachel liegt, auf den
 * naechsten Texel des Raumkoerpers gesetzt (euklidisch, bei Gleichstand die erste Spanne in Zeilenreihenfolge).
 * Raumkoerper = Index != 0 (transparent) und != 4 (Wandlinie, CLUT-Index 4 = 0x5ad6, Beleg re15_inv_screen.c
 * Tuer-Nische an DATA/MAP05.PIX) in DATA/MAP0x.PIX an der Kachel-uv der Seitentabelle @0x80076840
 * (tools/gen_map_kunst.py -> engine/src/re15_map_kunst.h). Auf der Wandlinie stuende der Punkt halb ausserhalb
 * des roten Raums (Abzug L6000_t0). Keine Schwelle, keine freie Zahl.
 *
 * Dazu (re15_map_zones.c rect_state_impl, Blatt 6..12): Zonen DESSELBEN Raums mit DERSELBEN Zeile auf demselben
 * Blatt sind ein Ort, den der Kuenstler auf zwei Kacheln gemalt hat (4040 r4+r12, 4080 r8+r13 Ring-Diagonale,
 * 5140 r10+r12 mit der AUSGELIEFERTEN Zeile @0x80076BB8) - steht die Figur in einer, sind beide aktuell (rot).
 * Vorher war in 4080 aus 4030 kommend nur der 2-3 px schmale Streifen r13 rot (Pruefbericht s3 r1: "Raum nur als
 * Linie rot", 35-39 rote Pixel).
 * Nur Blatt 6..12: auf Blatt 1-4 zeichnet der Port die Kacheln nach den Nutzer-Vorlagen der Runde 39 um
 * (karte_r39.c), dort ist die PIX-Maske nicht das gezeigte Bild; STAGE1 ist ausserdem die Messlatte.
 * Nicht fuer Zonen mit eigenen Teilbereichen (Zug 5090 auf Blatt 9 r7, U-12 in r42/rest) und nicht fuer Zonen,
 * die mit der AUSGELIEFERTEN Zeile ihres Raums rechnen (@0x800768b0 + 8*(Stage-Basis+Raum)): dort gilt die
 * Original-Formel byte-true (re15_inv_screen.c zone_echte_zeile).
 * ========================================================================== */
#ifndef RE15_KARTE_R42_H
#define RE15_KARTE_R42_H
#include <stdint.h>

/* Setzt (*mx,*my) auf den naechsten Texel des Raumkoerpers von Kachel <rect> auf Blatt <page>, falls er nicht schon auf
 * einem liegt. Rueckgabe: quadrierte Verschiebung in Pixeln (0 = unveraendert oder keine Kunst bekannt). */
int re15_karte_r42_kunst_klemmen(unsigned page, unsigned rect, int16_t *mx, int16_t *my);

/* 1, wenn (x,y) auf einem Texel des Raumkoerpers von Kachel <rect> auf Blatt <page> liegt (Messhilfe fuer Riegel). */
int re15_karte_r42_kunst_gemalt(unsigned page, unsigned rect, int x, int y);

/* r42/karte Stufe 3 Runde 4 - DER RING NICHT AUF DER NACHBARKACHEL (PORT-WAHL, Pruefbericht pruef_s3_r3 ROT: 4080_t3 "halb auf
 * der Wandlinie" - die Linie x177 ist die Wand von r11/40B0; Orchestrator-Vorgabe 2(c) "kein Punkt auf einer Wandlinie oder am
 * Uebergang"). Nach re15_karte_r42_kunst_klemmen steht die Ring-MITTE auf dem Raumkoerper der eigenen Kachel; der 5x5-Ring
 * (TEX.TIM @0x14910, uv 225..229) kann aber noch auf der Kunst einer ANDEREN Kachel liegen (Wandlinie oder Koerper des
 * Nachbarraums). Dann wird die Mitte um hoechstens halbe Ringbreite + 1 (3 px) verschoben: zuerst so wenig fremde Texel
 * wie moeglich, dann ein Rand mit mindestens der Haelfte Raumkoerper des eigenen ORTS (eigene Kachel + Kacheln desselben Raums
 * mit derselben Zeile), dann der kuerzeste Weg, dann mehr Rand. Die Mitte bleibt auf dem Raumkoerper des Orts;
 * die eigene Wandlinie und Treppenstriche im Inneren loesen dabei nichts aus.
 * Nachbesserung (Pruefbericht pruef_s3_r3 ROT 4070_t0/4080_t0): ausgeloest wird ausserdem, wenn im 2-px-RAND um den Ring
 * (9x9 ohne 5x5) weniger als 30 % Raumkoerper des Orts liegen (Mass der Pruefung); Ziel ist dann die naechste Lage mit
 * mindestens der Haelfte, Suchweite halbe Ringbreite + 1 (Belege und Messwerte in karte_r42.c). PORT-WAHL.
 * Stufe 3 Runde 6 (Pruefbericht pruef_s3_r5 ROT 4080_t0: Wandlinie im Ringinneren, Ring teils ueber blauem Grund): vor dem
 * Rand-Mass zaehlt ein SAUBERER Ring (3x3 im Ring sichtbarer Raumkoerper, kein Ring-Texel ausserhalb der Kunst des Orts, Rand
 * >= 30 %), gesucht bis 7 px; Herleitung, Messwerte und die Wirkung auf alle 397 Ankuenfte in karte_r42.c. PORT-WAHL.
 * Daten: re15_map_kunst.h s_map_kunst_alles (tools/gen_map_kunst.py, Index != 0). */
int re15_karte_r42_ring_klemmen(const void *zone, unsigned page, unsigned rect, int16_t *mx, int16_t *my);

#endif
