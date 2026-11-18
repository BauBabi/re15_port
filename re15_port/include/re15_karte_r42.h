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
 * Rechteck-Klemmung wird der Punkt, falls er nicht auf einem gemalten Texel der EIGENEN Kachel liegt, auf den
 * naechsten gemalten Texel dieser Kachel gesetzt (euklidisch, bei Gleichstand die erste Spanne in Zeilen-
 * reihenfolge). Gemalt = Index != 0 in DATA/MAP0x.PIX an der Kachel-uv der Seitentabelle @0x80076840
 * (tools/gen_map_kunst.py -> engine/src/re15_map_kunst.h). Keine Schwelle, keine freie Zahl.
 * Nur Blatt 6..12: auf Blatt 1-4 zeichnet der Port die Kacheln nach den Nutzer-Vorlagen der Runde 39 um
 * (karte_r39.c), dort ist die PIX-Maske nicht das gezeigte Bild; STAGE1 ist ausserdem die Messlatte.
 * Nicht fuer Zonen mit eigenen Teilbereichen (Zug 5090 auf Blatt 9 r7, U-12 in r42/rest) und nicht fuer Zonen,
 * die mit der AUSGELIEFERTEN Zeile ihres Raums rechnen (@0x800768b0 + 8*(Stage-Basis+Raum)): dort gilt die
 * Original-Formel byte-true (re15_inv_screen.c zone_echte_zeile).
 * ========================================================================== */
#ifndef RE15_KARTE_R42_H
#define RE15_KARTE_R42_H
#include <stdint.h>

/* Setzt (*mx,*my) auf den naechsten gemalten Texel von Kachel <rect> auf Blatt <page>, falls er nicht schon auf
 * einem liegt. Rueckgabe: quadrierte Verschiebung in Pixeln (0 = unveraendert oder keine Kunst bekannt). */
int re15_karte_r42_kunst_klemmen(unsigned page, unsigned rect, int16_t *mx, int16_t *my);

/* 1, wenn (x,y) auf einem gemalten Texel von Kachel <rect> auf Blatt <page> liegt (Messhilfe fuer Riegel). */
int re15_karte_r42_kunst_gemalt(unsigned page, unsigned rect, int x, int y);

#endif
