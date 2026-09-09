# RE1.5 Port — v0.7.33 (Early Preview)

**1F-Karte: Türsymbole standen über Kreuz — gespiegelte Zeilen werden jetzt erkannt.**

---

## Deine Marken 1–6 (Kartenansicht)

**Marken 3–6** („gehe oben durch die Tür, lande unten"): Engine und Türen waren
unschuldig — die HERGELEITETE Kartenzeile von ROOM1010 war z-gespiegelt (jeder Raum
hat sein eigenes lokales Koordinatensystem; die Herleitung kannte keine Spiegelung,
und bei zwei Türen zum selben Nachbarn paarte sie beide mit demselben Gegen-Record).
Jetzt: Ordnungs-Signatur je Achse (nur ausgelieferte Nachbarzeilen als Zeugen),
Spawn-basierte Gegentür-Paarung, Spiegel-Spalten wirken auch im Zeilen-Pfad.
Prüfmaß Türpaar-Konsistenz game-weit: Median 2,2 → 0,0 px; Ausreißer >12 px 19 → 8;
dein Fall 1010↔1020 von 24 px über Kreuz auf deckungsgleich.

**Marke 2** (Original-Tür „nicht sauber entfernt"): der Rest steht in Palettenindex 2
(orange), nicht in der Wandfarbe — zweite Filterklasse ergänzt (klein + wandnah;
die großen wandfernen Index-2-Flächen auf anderen Blättern sind legitime Kunst
und bleiben).

**Marke 1** (Wand-Stück an der Treppenkachel): Tabellenlage der Symbole ist per
Original-Anker korrekt; die Zeichnung an der Kachelkante braucht noch eine
verlässliche Panel-Messung — offen, kein Blindfix.

## Außerdem (dein Auftrag)

PRI-Aufräumen: 10 unbenutzte Dateien entfernt, 5 auf deine NN_MM-Nomenklatur
umbenannt (10D0/01a+01b→01_01/01_02, 06+06b→06_01/06_02, 10E0-Pult→07_01);
die abgenommenen Masken bleiben byte-identisch. Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.32 (Early Preview)

**1F-Karte: das Treppenhaus landet auf seiner Treppenkachel.**

---

## Dein Befund (fehler/MAP03.bmp)

Ankunft unten im Treppenhaus (1F): die Karte markierte den langen Flur statt des
kleinen Treppenbereichs. Gemessen: ROOM1060 saß per Zeilen-Block-Override auf
Rect 2 (Westflur-Zeichnung); die Treppenhaus-Vorgabe war nie wirksam, und auf der
Treppenkachel (Rect 10) saßen stattdessen zwei Flur-Splitter (ROOM1000-z0/1030).

**Fix:** ROOM1060 → Rect 10 (die 24×24-Treppenkachel mit Treppensymbol), die
Splitter → Rect 4 über eine neue NICHT-exklusive Vorgabenliste (ein harter
ZONE_FIX hätte Rect 4 gesperrt und die geeichten Nachbarn verdrängt — die
Kaskade des ersten Versuchs). Nebeneffekt, ebenfalls belegt: die alte
Fahrstuhl-Vorgabe (ROOM1080 → Rect 9) greift jetzt auch.

**Beweis-Anker:** die Tür 1040→1060 projiziert über 1040s AUSGELIEFERTE
Maßstabszeile auf Karte (118,151) = die linke Kante von Rect 10; die Tür
1040→1080 auf (114,145) = Kante von Rect 9. Der alte 1060-Marker rechnete sich
auf ~(112,154) — unten in Rect 2, exakt „der längere Flur" deines Befunds.

Die Gegenprobe im Kartentest wurde getauscht (1000↔1010 war ungültig — ROOM1000
ist ein Flur, seine zweite Zone saß schon immer Rahmen-an-Rahmen mit Rect 1):
neu ROOM1010↔ROOM10A0, Türgraph-Abstand 5, Rahmenlücke 15 px. Suite 282/282
(lokal + Docker).

---

# RE1.5 Port — v0.7.31 (Early Preview)

**Dein Lasso IST die Stuhl-Zuordnung — Kunst trägt jetzt die Tiefe ihres eigenen Stuhls.**

---

## Communication Room, fünfte Runde (deine zwei Marken F697/F1327)

Dein Befund nach v0.7.30: der Stuhl deckt dich weiterhin, obwohl du davor stehst.
Die Messkette: du standest bei z=11816 ZWISCHEN zwei Stühlen — vor dem fernen
(Zelle 12200..13700), hinter dem nahen (8200..9700). Die Kunst des FERNEN Stuhls
über deiner Hüfte trug die Tiefe der NAHEN Zelle (z 5120..6464 statt ~8900).

**Ursache:** die Zuordnung "welche Zellen-Box trifft der Sehstrahl zuerst"
beantwortet nicht, welchen Stuhl das Pixel ZEIGT. Die Kollisionszellen sind
1500×1500, die gezeichneten Stühle ~700 — die Fantasie-Box der nahen Zelle
überlappt im Bild die Kunst des fernen Stuhls.

**Fix:** gemessen liegt jede deiner Lasso-Komponenten zu ≥0,99 in genau EINER
Zellen-Silhouette — also bekommt jedes Lasso als Ganzes den Quader SEINER Zelle,
und die Schwarz-Regel etikettiert: Tiefschwarz ≤7 Pixel neben eigener Kunst in
eigener Silhouette = Lehnen-Lücke (gemessen: Lücken-Klasse 1..7, Pult-Dunkel ab 7),
sonst nächste enthaltende Zelle, ohne Silhouette gar nicht (kein Boden-Blit).

## Gemessen, an neun Marken

```
ungerechtfertigte Deckung (Maske näher als Szene):  0 an allen neun Marken
Sichtproben F697/F1327: Bein wieder sichtbar; nahe Stühle decken weiter korrekt
Suite 282/282 (lokal + Docker); Masken in beiden Paketen hash-verifiziert
```

---

# RE1.5 Port — v0.7.30 (Early Preview)

**Das Atlas-Leck: eine Stuhl-Kachel deckte fremde Pixel mit der eigenen Tiefe.**

---

## Communication Room, vierte Runde (deine zwei Marken)

Dein Befund: die beiden Stühle im Hintergrund decken Leon auch dann, wenn er
davor steht. Die Messkette führte NICHT zu den Tiefen der Lasso-Objekte, sondern
in den Atlas-Packer:

1. **Täter identifiziert:** die deckenden Pixel an F1476 sind Kunst des FERNEN
   Stuhls (−1600, 12200) — sein Quader-Raycast steht dort bei vz≈8900..9280
   (dürfte dich nie decken). Geblittet wurden sie aber von Kacheln des NAHEN
   Bürostuhls (−1800, 8200) mit t=97..103 (z 6208..6592).
2. **Ursache:** `atlas.build` machte jede Kachel mit der VEREINIGUNG aller
   Objektregionen deckend — jede Kachel nahm Fremd-Pixel in ihrem Rechteck mit
   und gab ihnen die eigene (hier: zu nahe) Tiefe. Deshalb blieben die
   Messzahlen über drei Umbauten bitgleich: die Täter-Kacheln gehörten einem
   Objekt, das ich gar nicht anfasste.
3. **Fix:** jede Kachel deckt nur noch Pixel des EIGENEN Objekts (Herkunft wird
   durch Zerlegung und Halbierung verfolgt). Dazu läuft die Szene-Attribution
   der Lassos nur noch über Möbel-Kreiszellen — Schreibtisch-Säulen ohne Deckel
   schnappten sich sonst Pixel, über die man in Wahrheit hinwegsieht.

## Gemessen, an sieben Marken

```
ungerechtfertigte Deckung (Maske näher als Szene):  212 -> 0
Alt-Neu-Diff je Marker-Kasten: weggefallen NUR Leck-Deckung ohne Möbel davor,
0 gerechtfertigte Deckung verloren, 0 neu hinzu
Sichtproben F1254/F1476: Stuhl deckt hinter ihm, gibt frei davor
Suite 282/282 (lokal + Docker)
```

---

# RE1.5 Port — v0.7.29 (Early Preview)

**Die Stuhllehnen decken bis zur echten Höhe — Schwarz zählt als Stuhl.**

---

## Communication Room, dritte Runde (deine vier Marken)

Zwei gemessene Ursachen, warum Leon noch teilweise im Vordergrund war:

1. **Die schwarzen Lehnen sind höher als gedacht:** an drei Schwarz-Säulen aufgelöst
   liegen sie bei h≈−1950 (−1970/−1927/−1970) — meine −1100-Boxen endeten darunter,
   die Lehnenspitzen blieben frei.
2. **Dein Lasso hat schwarz-auf-schwarz-Lücken** (gemessen: bg-Farbsummen 4..32 an
   Pixeln, die nicht im Lasso sind — dieselbe Klasse wie beim 10D0-Klappstuhl). Mit
   „nur Kunst deckt" blieb genau dort nichts übrig.

**Fix:** Höhe −1950, und die Deck-Region ist jetzt *deine Kunst ODER Tiefschwarz*
(Farbsumme < 45). In diesem Raum ist Schwarz eindeutig Stuhl — der Boden ist hell
(Summe 180+), die Boden-Überblendungen aus der zweiten Runde bleiben damit
ausgeschlossen.

## Gemessen, an fünf Marken

```
schwarze Vordergrund-Stuhlfläche:  1790 / 747 / 878 / 1049 / 1711 Punkte
davon ungedeckt:                      0 /   0 /   0 /    0 /    0
Sichtproben F1195/F1321: Stuhlmechanik, dunkle und weiße Lehne sauber VOR der Figur
```

Tests: **282/282** (im Release-Container).
