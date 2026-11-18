# Bericht Strang "karte", Stufe 2 (STAGE4 bis STAGE6), Runde 1

Zweig r42/karte, Commits **c7536ba7e** (Code) und der Doku-/Pin-Commit danach (Id im Schema). Kein Push.

## Kurz und in Alltagssprache

- **Im Labor (STAGE4), in der Fabrik (STAGE5) und am Bahnsteig (STAGE6) steht der Leon-Punkt jetzt in jedem Raum
  auf seinem eigenen gemalten Raum und laeuft mit.** Vorher lagen viele Raeume auf einem fremden Feld: zum Beispiel
  4050 auf dem kleinen Fahrstuhl-Feld, 5040 und 50A0 auf dem Nachbarfeld, 6010 auf einem Zugwagen.
- **Im echten Spiel nachgemessen:** 31 Abzuege des Kartenschirms, in allen 31 sitzt der Punkt genau dort, wo er
  hingehoert. 4 Raeume lassen sich so nicht abziehen, weil beim Betreten eine Szene laeuft (wie schon in Stufe 1).
- **Zwei Raeume hatten gar keine Karte:** 5070 und seine spaetere Fassung 5130 (der T-foermige Gang). Jetzt sind sie da.
- **Fuenf Raeume nutzen jetzt die Umrechnung, die das Originalspiel selbst mitliefert,** auf dem richtigen Feld
  (5040, 5120, 50A0, 5140, 5100). Bei 5100 war sie bisher gar nicht benutzt.
- **Alle 19 gemalten Tueren der Blaetter 8 bis 12 tragen ihr gelbes Tuerzeichen.** Vorher fehlten 8.
- **Treppen** stehen jetzt auf dem Blatt: im Raum 5020, viermal im Bahnsteigraum 6010, im Treppenhaus 4070.
- **Zwei Raeume zeigen ein anderes Blatt als das Originalspiel vorsieht**, weil nur dort ihr Bild gemalt ist:
  6020 (zwei achteckige Raeume, gemalt neben dem T-Gang der Fabrik) und 5080 (gemalt am Bahnsteig).
  Das ist meine Wahl (PORT-WAHL), kein Originalverhalten.
- **Nicht angefasst:** der Zug 5090 (das baut r42/rest als U-12) und 6040 (das ist der Abspann, dort gibt es keine Karte).

Fachwoerter: "Blatt" = Kartenseite. "Kachel"/"Feld" = ein gemalter Raum auf dem Blatt. "Zeile" = Umrechnung von
der Spielposition auf den Kartenpunkt. "Drehung" = wie der Raum auf dem Blatt liegt (0/90/180/270 Grad, nie
gespiegelt). "Riegel" = automatischer Test. "FB_SHOT" = Abzug des Kartenschirms aus dem echten Spiel.
"Schleier" = Raeume, die man noch nicht besucht hat, sind nicht zu sehen.

## Was gemacht wurde

1. **Welcher Raum ist welches Feld** (Tabelle in `analysis/karte_r42/BESTAND_s2_r1.md` §1): ausgelieferte Zeile
   (@0x800768b0 + 8*Slot, Formel FUN_800473f8 @0x8004741c-0x80047528) -> das Feld, auf das die Kollisionszellen fallen;
   sonst Tuer-Identitaet (gemaltes Tuerzeichen an genau dieser Grenze) und Form. Umgezogen: 4040, 4050, 4070 (Gast
   Blatt 9), 5020, 5040/5120, 5050, 50A0/5140, 50D0 (Gast Blatt 11), 5100, 6000, 6010, 6020, 5080; neu: 5070, 5130,
   Zonen 1 fuer 4040/4080 (Diagonalstueck an 4030), 4050 (zweite Labyrinth-Zelle), 5140 (Ostgang).
2. **Drehung belegt** (`tools/karte_r42_zeilen.py 8..12`, Abschnitt DREHUNG). Neu: Belegart (d) = Gegen-Tuer mit der
   Zeile eines Nachbarn, dessen Drehung selbst belegt ist; Vorrang (a) vor (c) an derselben Tuer (Ring-Diagonalen).
   Blatt 6/7 bleiben bytegleich (Ausgabe verglichen). 13 Raeume BELEGT, 5 PORT-WAHL mit nur einer Tuer (40B0, 5050,
   50E0, 50F0, 5080) - jeweils mit Grund im Werkzeug. 5000 (R90cw) und 6020 (R90cw) liegen per Achsentausch gedreht.
3. **Marken Blatt 8-12** neu aus dem Werkzeug; Marke in der Kasten-Ueberdeckung und auf gemalter Kachel (nur ab
   Blatt 8). 5090-Marken (zid 84) unveraendert.

## Tests

- Riegel neu `unit_r42_karte_st456` [A]-[H] gruen. GEGENPROBE (derselbe Test im Baum von aaea5f8c2): ROT in [A]
  (5070/5130 ohne Zone), [B], [C], [D] (u.a. 4030/4040/4050/4070/4080), [F] (16 von 21 Tueren 17-109 px daneben oder
  ohne Zone), [G] (alle 6 Treppen), [H] (8 gemalte Tuerzeichen ohne Marke); [E] war dort schon gruen (Waechter gegen
  Spiegelung). Hier: [F] 21 Tueren 2-7 px (Median 4), [D] 123 Tueren mit eigener Marke.
- `ctest -R "unit_.*(map|karte)"` 51/51 gruen.
- `ctest -R "map|karte|kartenbits|inv_screen|lageplan" -j 6` (mit GUI): 63/64 gruen; rot war
  integration_map_raum_live ("3 von 4 Tueren zeigen ihr Symbol": 167/225). Ursache: die 4050-Labyrinth-Warps zaehlten
  bisher nur zufaellig als Treffer (4050 stand auf der Fahrstuhlkachel, Marker fest). Pin angepasst: Warps im selben
  Raum zaehlen nicht als Tuer -> 159/197 (81 %), Einzellauf gruen. Ebenfalls angepasst: unit_r30_karte_nutzerstand
  (Klasse C leer, die Marke (9,190,91) gehoerte 5020 auf dem fremden r7).

## Offen

- 4050: nur die zwei Verteiler-Zellen haben Felder; die sechs Seitenkammern sind nicht gemalt (Punkt am Rand des
  naechsten Felds). Fuer eigene Zonen fehlen Besucht-Bits (5 frei, 6 noetig).
- 6000: das Feld r5 passt schlecht zur Kollision (25/48 Rasterpunkte auf gemalter Flaeche); Tuer 6000<->6030: die
  Original-Zeile von 6030 legt die Tuer 64 px neben 6000s eigene Lage, Marke am Zugende (265,111).
- 5080: Tuer 14 px neben dem gemalten Zeichen. 50D0 auf Blatt 11: Tuersprung bis 16 px.
- Fahrstuhl 4020: weiter ohne Zeile (Streckung) und ohne Tuermarken.
- 6030: Original-Zeile laeuft aus dem Bild, Punkt klemmt am Zugende (Auslieferungsstand).
- Ring 4040/4080 und T-Gang 5070: 61-74 % der Rasterpunkte auf gemalter Flaeche (Kunst zeigt nur die Gaenge).
- integration_map_raum_live: 38 Tueren ohne sichtbares Zeichen im Umkreis 4 px (davon STAGE4-6: 50D0 4, 6000 3,
  4040 2, 6030 2, je 1 in 4030/4080/5080/50E0/50F0/6010).
- Ein gdigrab-Video habe ich nicht gemacht; die Abzuege sind FB_SHOT (deterministisch, Memory karten-sichtlauf).

## Kollisionen

`git merge-tree --write-tree` von c7536ba7e gegen r42/finale df31b74e2, r42/gegenstaende 5515f2601, r42/gore
3cb1c655d, r42/rest 649e3a57a, r42/story 5849c8996, r42/lageplaene bce9c91a8: **kein Konflikt**.

Vorwarnung U-12 (r42/rest): 5090/5091 steht weiter auf Blatt 9 r7 - das ist jetzt das Feld des T-Gangs 5070. Wer
5090 auf die Wagenkacheln von Blatt 12 legt, sollte auch die drei 5090-Marken (zid 84, Blatt 9 r7) und die
vier Teilbereich-Eintraege (s_map_teile, Blatt 9 r7, zid 84) mitnehmen. 6030 bleibt auf Blatt 12 r7, 6010 ist jetzt r0 (nicht mehr die
Wagenkachel r4). Neue zids 115-120 (5070, 5130, 5140 Zone 1, 4050 Zone 1, 4040 Zone 1, 4080 Zone 1).
U-11 (Wandplan 402 -> Blatt 8) aendert in r42/rest nur re15_map_owned.c - keine gemeinsame Tabelle.

## Dateien

- Bestand: `analysis/karte_r42/BESTAND_s2_r1.md`; Werkzeug `re15_port/tools/karte_r42_zeilen.py`; Riegel
  `re15_port/tests/unit/test_r42_karte_st456.c`.
- Messbilder (Kopfzeile "Messbild - kein Ergebnis"): `analysis/karte_r42/messbild_blatt8_s2.png`,
  `messbild_blatt9_s2.png`, `messbild_blatt10_12_s2.png`, `messbild_zeilen_blatt8_s2.png`, `_blatt9_s2.png`,
  `_blatt12_s2.png`.
- Arbeitsdateien: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s2/`.
