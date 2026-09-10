# RE1.5 Port — v0.7.39 (Early Preview)

**ROOM2090: Lauerstellung zur echten Leiter, natürliches Klettern, Bogen in Bogenform.**

---

Deine drei Punkte:

1. **Startrichtung**: Er lauerte messbar exakt auf meinen falschen Leiter-Anker
   (der lag 5,3 m südlich — eine Bilddeutungs-Panne). Die Leiter ist dein
   Einstieg an der Türnische oben links; der Anker sitzt jetzt dort. Kontrolle:
   Maul zeigt auf die Leiter, keine 180°-Wende mehr beim Kampfstart.
2. **Klettern**: Kein Sprung mehr an die Stegkante — er schwimmt sie jetzt
   sichtbar an (dabei saß er zunächst exakt am Kollisions-Radius fest: die
   Wand-Klemme drückte ihn vom Steg weg, während er hinwollte — behoben).
   Zusätzlich neigt sich der Körper entlang der Bahn: Aufstieg Nase hoch,
   Abstieg Nase runter.
3. **Bogenform**: Die Wirbelkrümmung war seitenverkehrt (beide Enden hoben
   nach oben). Jetzt wie von dir beschrieben: unten Kopf — Rücken oben —
   unten Schweif; im Bild hängt der Kopf vorn über die Kante, der Rücken
   wölbt sich über den Steg, der Schweif hängt hinten nach.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.38 (Early Preview)

**ROOM2090: die Überquerung kommt jetzt im normalen Kampf — und damit auch die Spinnen-Flucht.**

---

Deine Marker („Spinne flieht nicht, Krokodil klettert nicht über die Platform"):
Die Überquerung feuerte bisher nur, wenn Leon exakt AUF dem Plattform-Block
stand — beim Umlaufen des Beckens kam sie nie, und die Spinnen-Flucht hängt an
ihrem Start. Jetzt gilt dein Wortlaut („wenn der Aligator Richtung Platform
kommt"): Sobald die Plattform in der Verfolgung zwischen ihm und Leon liegt,
hebt er den Oberkörper und schiebt sich im Bogen darüber — die Spinnen rennen
in dem Moment zur Wand und klettern hoch. Nach einer Passage umschwimmt er das
Becken erst wieder ~10 s, damit sich beides abwechselt.

Dein Marker zeigte außerdem GAR KEINEN Alligator mehr in der Gegnerliste —
das konnte ich in Kampf-, Dauer- und Todes-Proben nicht reproduzieren (auch
seine Leiche bleibt liegen). Deshalb liegt jetzt ein Watchdog neben der exe:
`gator_boss.log` protokolliert Spawn und jedes Verschwinden mit Frame. Wenn
es dir wieder passiert, schick mir bitte diese Datei mit.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.37 (Early Preview)

**ROOM2090: der Alligator schwimmt wieder mit dem Maul voran.**

---

Dein Befund („läuft mit Hintern Richtung Charakter"): der 180°-Ausrichtungs-
Offset aus v0.7.35 war eine Fehldeutung meines damaligen Standbilds — beim noch
raumgroßen Modell hatte ich Kopf und Schwanz verwechselt. Das RE2-Modell steht
nativ richtig; der Offset ist entfernt, und alle Blick-Setzungen des Bosses
(Lauern zur Leiter, Überquerungs-Bahn) nutzen jetzt exakt dieselbe Engine-
Peilung wie die Verfolgung selbst.

Diesmal als Bewegt-Serie abgenommen (zwei Aufnahmen im Sekundenabstand): er
rückt auf Leon zu, und das Maul führt die Bewegung an.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.36 (Early Preview)

**ROOM2090: der Alligator auf ⅔-Maßstab — er war länger als der Pool.**

---

Dein Befund („ein wenig zu groß für den Raum") nachgemessen: das RE2-Modell ist
in Bindpose 18,3 m lang und 4 m hoch — länger als die kurze Poolseite (16,1 m).
Jetzt ⅔-Maßstab über den byte-true Entity-Render-Scale (derselbe Mechanismus,
mit dem das Original den Gorilla-Boss auf 1,7× zieht): ~12 m lang, ~2,6 m hoch
(≈ 1,5 Leon-Höhen), passt in die Ring-Rinne um die Plattform. Trefferboxen und
Reichweiten bleiben die byte-true 0x23-Werte; der Überquerungs-Bogen wurde auf
die neue Bauchhöhe nachgeeicht (er liegt am Scheitel auf der Plattform auf).

Sichtgeprüft: Lauerstellung an der Leiter und Plattform-Überquerung in
stimmiger Proportion.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.35 (Early Preview)

**ROOM2090: der Alligator-Bosskampf nach deinem Drehbuch.**

---

## Deine 8 Punkte

1. **Spinnen auf der Plattform**: die zwei Raum-Spinnen sitzen jetzt oben auf dem
   Mittelblock und bleiben dort verankert sitzen.
2. **Lauerstellung**: der Alligator startet links oben im Wasser und fixiert die
   Leiter an der Westwand. Die Türnische ist sicher — der Kampf beginnt erst, wenn
   du in den Pool vordringst, ihm direkt vor die Nase läufst oder schießt.
3. **Biss**: byte-true Fenster/Reichweite der 1.5-KI, jetzt mit echtem Schaden
   (50, die byte-true Tabellenzeile) + Knockdown. Ein Biss pro Angriff, danach
   die originale Wieder-Angriffs-Sperre.
4. **Boss-HP 3000** (10× der originale 300er-Wert — Magnum 10 Treffer,
   Shotgun 15, Pistole 150).
5. **Treffer zeigen**: Blutspritzer bei jedem Treffer am vorderen Rumpf; alle
   10% HP zuckt er sichtbar zurück (Flinch-Clip), dazwischen Boss-Panzerung.
6. **Ring-Verfolgung**: er umschwimmt den Plattform-Block auf der kürzeren
   Seite, statt in ihm hängenzubleiben.
7. **Spinnen-Flucht**: sobald er die Überquerung ansetzt, rennen die Spinnen
   zur Wand dahinter und klettern sie hoch.
8. **Plattform-Überquerung**: steht Leon oben, hebt der Alligator den
   Oberkörper, schiebt sich im Halbbogen über die Plattform (Wirbelsäule
   krümmt sich mit, Kopf taucht drüben ein, Schwanz noch diesseits) und
   gleitet auf der anderen Seite ins Wasser.

Sichtgeprüft am laufenden Spiel: Lauerstellung flach im Wasser, Lunge-Biss mit
aufgerissenem Maul, Überquerung mit gehobenem Oberkörper. Das RE2-Modell stand
anfangs 180° verkehrt (RE2-Grundpose blickt −X) — behoben im Renderer.

**Zum Selbsttesten**: RE15_GB_TEST=1 startet den Kampf sofort, =2 erzwingt
sofort die Plattform-Überquerung.

**Bekannt/offen**: Clip-Deutung der RE2-Bank ist Statistik-basiert (Biss/Flinch/
Todesrolle sitzen sichtbar richtig; Feinschliff nach deinem Eindruck). Der
Debug-Sprung direkt nach 2090 lädt je nach Latenz 25–55 s.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.34 (Early Preview)

**Karte: der aktuelle Raum zeigt jetzt seine Zeichnung — rot geschleiert statt schwarz. Plus: der Alligator schwimmt in ROOM2090.**

---

## Deine zwei Nachzügler-Marken + „Kartenstück freischalten"

**„Das soll weg verdammt!" — der Bogen (Blatt 1F):** Der Rest-Türschwenk stand in
der WANDFARBE (Index 4) und klebte an den kurzen Kastenwänden eines Zimmerchens —
das Lauflängen-Wachstum verschmolz beide, und die Größenschranke verwarf dann ALLES.
Zweiter Filterpass nur für solche Reste (Saat ≤3, Wachstum ≤6, Komponente ≤13 px =
Schwenkgröße §27). Gemessen am Blatt: 10 statt 9 Symbole entfernt, Nachbarblätter
unverändert in der dokumentierten Spanne.

**Wandbündige Türbalken:** die synthetischen Querbalken sind jetzt 1 px tief und
liegen AUF der Wandlinie statt 2 px in den Raum zu ragen (deine Marke 1,
„Wandstück links vom Spieler").

**„Wenn ich Room 1010 betrete … Kartenstück freischalten":** Der Raum WURDE
freigeschaltet — aber unsichtbar: die rote Modulation multiplizierte die grüne
Kachel-Kunst zu fast-Schwarz. Jetzt zeichnet der aktuelle Raum seine Kunst im
Besucht-Ton und legt einen halbtransparenten roten Schleier darüber (50/50-Mix,
neu im Rasterizer). Live geprüft (Framebuffer-Dump 1F + 3F): Wände, Auskerbungen
und Türsymbole sind unter dem Rot sichtbar.

## Außerdem (dein Auftrag 3)

**Alligator in ROOM2090:** Das RE2-EM23-Modell (22 Bones, 12 Clips) hängt an der
byte-true RE1.5-KI (Typ 0x23, State-Tabelle @0x80118bc8, Hitbox @0x80118b98) und
spawnt im leeren dritten Wasser-Areal des Pools (−5300, 0, −16300), Grid „Wasser".
Bitte im Spiel ansehen — Clip-Zuordnung der RE2-Bank ist noch nicht abgenommen.

**Bekannt:** Der Debug-Sprung direkt nach 1010/2090 wirft nach ~2 s in den
Vorraum zurück (Sprung-Spawn steht in einem Tür-AOT) — der normale Weg durch
die Tür ist nicht betroffen.

Suite 282/282 (lokal + Docker).

---

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
