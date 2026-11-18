# Karte STAGE4-6: Zuordnung, Drehung, Belege (Strang r42/karte, Stufe 2, Runde 1)

Stand: Zweig r42/karte auf 392f21b2d (Stufe 1 = b0a9ddbb9). Werkzeug `re15_port/tools/karte_r42_zeilen.py 8|9|10|11|12`
(Abschnitt DREHUNG, ZEILEN, TUERMARKEN, TREPPENMARKEN, PRUEFGROESSE), Header-Patch aus dessen Ausgabe
(`re15_port/engine/src/re15_map_zones.h`, Abschnitt "r42/karte Stufe 2"), Riegel `unit_r42_karte_st456`.
Nicht in dieser Stufe: 5090/5091 (Wagen-Kacheln Blatt 12 = U-12 in r42/rest; Zeile, Marken zid 84 unveraendert),
6040/6041 (Abspann-Diashow, aot_common.c: kein Kartenschirm).

## 1. Welcher Raum ist welche Kachel

Belegart der Kachel: **Z** = die AUSGELIEFERTE Zeile (@0x800768b0 + 8*Slot, Slot = Stage-Basis 65/77/98 + Raum)
projiziert die SCA-Zellen des Raums auf genau diese Kachel (gemalte Texel gezaehlt); **T** = Tuer-Identitaet
(gemaltes Tuersymbol an der Grenze zu genau diesem Nachbarn, Nachbar mit Z-Zeile); **F** = Form der Kachel.

| Raum | Blatt/Kachel vorher | jetzt | Beleg |
|---|---|---|---|
| 4000, 4010, 4030, 4090, 40A0 | r0, r1, r3, r9, r10 | unveraendert | Z @0x80076AB8/AC0/AD0/B00/B08 |
| 4020 (Fahrstuhl) | 8/r2 + Gast 9/r13, 10/r3 | unveraendert | T: Tueren 4000 W, 5000 S(->W), 50C0 E auf die 16x16-Kacheln |
| 4040 | 8/r12 (16x16) | 8/r4 + Zone 1 r12 | F: linke Ringhaelfte (senkrechter Gang + zwei Diagonalen); T: gemalt (141,163) E auf r4 -> 4070, (100,104) E auf r5 -> 4050 |
| 4050 (Labyrinth, 8 Zellen per Warp) | 8/r2 (Fahrstuhl-Kachel!) | Zone 0 Verteiler A -> r5, Zone 1 Verteiler B -> r6 | T: die zwei 4040-Tueren (W oben -> r5, O unten -> r6), gemalt (100,104) E / (113,119) W |
| 4070 (Treppenhaus) | 8/r7, Gast 9/**r1** (= 5010!), 11/r3 | 8/r7, 9/**r14**, 11/r3 | F: gemaltes Treppenhaus-Symbol 15x15 auf allen drei Blaettern; T: 50A0 Z -> (139,179) an r14 |
| 4080 | 8/r8 | 8/r8 + Zone 1 r13 | F: rechte Ringhaelfte; T: gemalt TUER2 (175,163) -> 40B0, (207,128) -> 4090 |
| 40B0 | 8/r11 | unveraendert | T: gemalt TUER2 an r8/r11 |
| 5000 | 9/r0 (ohne Zeile) | 9/r0 R90cw | T: vier Tueren auf die vier Nachbarn r1/r2/r3/r13 |
| 5020 | 9/r7 | 9/**r2** | T: gemalt TUER2 (153,73) N auf r0; F: 24900x16700 breit |
| 5030, 5110 | 9/r3 | unveraendert | Z @0x80076B30/BA0 |
| 5040, 5120 | 9/**r5** | 9/**r4** | Z @0x80076B38/BA8 trifft r4 (60 bzw. 75 Texel) - r5 liegt IN r4 |
| 5050 | 9/**r4** | 9/**r5** | Z: 5040 legt seine 5050-Tuer auf (88,100) = Westkante r5 |
| 5060 | 9/r6 | unveraendert | Z @0x80076B48 |
| 5070, 5130 | keine Zone | 9/**r7** R180 (neu) | F: T-Gang (Ostschenkel, Arm nach Westen zu r3); T: 5030 Z -> (172,125), gemalt (223,91)/(223,155) E zu den Achtecken |
| 5080 | 9/r9 (Achteck) | **12/r6** PORT-WAHL | T: 6010s einzige Westtuer fuehrt nach 5080, gemalt TUER E (135,130) zwischen r6 und r0 |
| 50A0 | 9/**r14** | 9/**r10** | Z @0x80076B68 trifft r10 (101 Texel), r14 ist das Treppenhaus 4070 |
| 5140 | 9/r14 | 9/r10 + Zone 1 (Ostgang) r12 | Z @0x80076BB8: der Ostgang (nur in 5140) faellt auf r12, gemalt TUER2 (195,171) N -> 50B0 |
| 50B0 | 9/r11 | unveraendert, neue Zeile | T: 5140 Z -> (196,169) |
| 50C0 | 10/r0 | unveraendert | Z @0x80076B78 |
| 50D0 | 10/r1 + Gast 11/**r0** | 10/r1 + Gast 11/**r1** | T: gemalt TUER2 (160,133) E auf 11/r1 -> 5100; 5100 Z |
| 50E0, 50F0 | 10/r2, 11/r2 (ohne Zeile) | unveraendert, neue Zeilen | F: zwei Stockwerke derselben Form (Achteck- und Viereck-Loch) |
| 5100 | 11/**r1** (ohne Zeile) | 11/**r0** mit Z | Z @0x80076B98 trifft r0 (91 Texel) - war ungenutzt |
| 6000 | 12/r7 | **12/r5** | T: 6000s zwei Westtueren = gemalt (191,103)/(191,133) E an r0/r5 |
| 6010 | 12/r4 (Wagen-Kachel) | **12/r0** | T: 6010s zwei Osttueren dieselben Symbole; F: die gemalten Treppen nord/sued des Gleises (Aot_set sce 13 bei z -7430/-20853) |
| 6020 | 12/r6 + r5 (ohne Zeile) | **9/r8** (Zone 0) + **9/r9** (Zone 1) R90cw, PORT-WAHL Blatt 9 | F: zwei gleiche Achteck-Raeume; T: die zwei gemalten Osttueren von r7 (5070 W-Wand z 13300 / -24650, 64 px gemalt, 65 px gerechnet) |
| 6030 | 12/r7 | unveraendert | Z @0x80076BD8 (Zug laeuft nach x 249..310 aus der Kunst - Auslieferungsstand) |

## 2. Drehung je Raum (Werkzeug, Abschnitt DREHUNG)

Neue Belegart (d): Gegen-Tuer mit der hergeleiteten Zeile eines Nachbarn, dessen Drehung selbst BELEGT ist (Kette
ueber Sackgassen). Neue Vorrangregel: hat eine Tuer eine (a)-Stimme (gemaltes Symbol an genau dieser Tuer), zaehlt
ihre (c)-Stimme (Lage des Nachbar-Rechtecks) nicht - sie ist kein unabhaengiger Beleg und an den Ring-Diagonalen
falsch (r11 liegt suedlich von r8, die Tuer 4080->40B0 sitzt aber an der Westseite). Blatt 6/7 bleiben bytegleich
(Werkzeugausgabe ZEILEN/MARKEN gegen Stufe 1 verglichen).

| Raum | Drehung | Stimmen | Ergebnis |
|---|---|---|---|
| 4040 | R0 | 3 (a)(d), 2 Tueren | BELEGT |
| 4050 Zone 0 / Zone 1 | R90ccw / R90cw (je Zone: die Zellen sind nur per Warp verbunden) | je 2 (a)(d) | BELEGT |
| 4070 (Blatt 8/9/11) | R0 | je 2 (a)(d) bzw. (a)(b) | BELEGT |
| 4080 | R0 | 2 (a)(c), 2 Tueren | BELEGT |
| 40B0 | R0 | 0 | PORT-WAHL (Tuer im Rauminneren, keine Weltseite) |
| 5000 | R90cw (Achsentausch) | 9 aus 5 Tueren (a)(b)(c)(d) | BELEGT |
| 5020 | R0 | 2 (a)(d) | BELEGT |
| 5050 | R0 | 1 (b) | PORT-WAHL (eine Tuer; nur R0 bildet sie ab) |
| 5070, 5130 | R180 | 6 (a)(b)(c)(d) | BELEGT |
| 50B0 | R0 | 2 (a)(b) | BELEGT |
| 6020 | R90cw | 4 (a)(d), 2 Tueren | BELEGT |
| 50D0 | R0 | 3 (a)(b)(c) | BELEGT |
| 50E0, 50F0 | R0 | 1 (c) | PORT-WAHL (eine Tuer) |
| 5080 | R0 | 1 (a) | PORT-WAHL (eine Tuer) |
| 6000 | R0 | 7 (a)(b)(d) | BELEGT |
| 6010 | R0 | 7 (a)(d) | BELEGT |

Versatz und Massstab: Kollisions-Bbox in die gemalte Flaeche (zeile_rot), Massstab <= Blattmassstab (Median der
ausgelieferten Zeilen des Blatts). PORT-WAHL dabei: 4040/4080 Zielkasten = r4+r12 bzw. r8+r13 (die Diagonale an 4030
ist eine eigene 16x16-Kachel); 6000/6010 Kasten nur bis zur gemeinsamen Tuerlinie (beide Kollisionen reichen ~9400
Einheiten ueber sie hinaus, Gleisbereich), 6000 an die Westkante von r5 gelegt - die zwei Tueren 6000<->6010 sind
fuer 6000 daher KEIN freier Beleg.

## 3. Pruefgroessen ausserhalb des Fits (Riegel unit_r42_karte_st456)

- [F] 21 Tueren: Marker an der Tuer -> gemaltes Symbol bzw. ausgelieferte Nachbar-Zeile 2-7 px (Median 4), jedes
  Mal naeher als die Zonenmitte (aaea5f8c2: 16 von 21 Tueren daneben (17-109 px) oder ohne Zone).
- [H] alle 19 gemalten Tuersymbole der Blaetter 8-12 tragen ihre Marke (<= 1 px neben der Wandlinie);
  aaea5f8c2: 8 ohne.
- [G] Treppen: 5020 (154,55), 6010 viermal, 4070 (150,170) - aaea5f8c2: alle sechs fehlen.
- [D] 123 Tueren (Leon+Elza) mit eigener Marke; 4030<->4040/4080 ueber den Spawn (read_rdt liest die 4030-Tueren
  nicht: Ziel 0xBF0A0/0xC5F00).

## 4. Offen

- 4050: nur die zwei Verteiler-Zellen sind Zonen (r5/r6); die sechs Seitenkammern sind nicht gemalt, der Marker
  steht dort am Rand der naechsten Kachel. Fuer eigene Zonen fehlen Besucht-Bits (Zusatzbits 251-255 = 5 frei, 6 noetig).
- 6000: r5 traegt einen ungemalten Block oben links; nur 25/48 Rasterpunkte auf gemalten Texeln (Ausnahme [B]).
  6000<->6030: die ausgelieferte 6030-Zeile legt die Tuer 64 px neben 6000s eigene Lage; Marke am Zugende (265,111).
- 5080 auf r6: Tuer 14 px neben dem gemalten Symbol (r6 ist anders geformt als die 5080-Kollision).
- Fahrstuhl 4020: weiter Bbox-Streckung (keine Tuer im RDT, nur Spawn), keine Tuermarken.
- 6030: ausgelieferte Zeile laeuft aus der Kunst, Marker klemmt an r7 (Auslieferungsstand).
- 5090/5091: Zone weiter auf Blatt 9 r7 (jetzt 5070s Kachel) - U-12 (r42/rest).
- Seitenwahl ausserhalb des Original-Inits: 5080 (Blatt 12 statt 9), 6020 (Blatt 9 statt 12) - PORT-WAHL, weil nur
  dort ihre Kunst liegt (FUN_8004bdd4 / FUN_8004bf70 setzen Blatt 9 bzw. 0xC, @0x8004bf50 / @0x8004c038).
