# Marke 4 — ROOM10D0 Cut 7, „fehlende Stuecke vom Stuhl auf der rechten Seite"

Nutzer-Meldung (2026-09-21): *„Das meiste an PRIs sieht gut aus, aber es gibt ein paar PRIs
die sind zu falsch. Siehe F9 Marker. Gerade der letzte zeigt jetzt fehlende Stuecke vom Stuhl
auf der rechten Seite. Gerade da bin ich mir SICHER das ich den Stuhl in meiner vorherigen PRI
Implementierung richtig ausgeschnitten habe...."*

**Kurzfassung.** Der Nutzer hat recht, dass etwas falsch ist — aber nicht an seiner Freistellung
und nicht dort, wo es aussieht. Seine Freistellung `07_01.png` ist EINE zusammenhaengende Form
ueber ZWEI Gegenstaende in sehr verschiedener Entfernung. Das Tiefenmodell `aufrecht` gibt so
einer Form EINEN Standpunkt, hier den ihres tiefsten Punktes (Bildzeile 238) — also die Tiefe
des NAHEN Stuhls, 53..62. Damit trugen **18 Punkte des blaugrauen Kastens am Tischende** die
Stuhltiefe, obwohl der Kasten **1360..1675 Welteinheiten hinter dem Spieler** steht. Diese 18
Punkte sind hell (Hintergrund-Helligkeit Median 341, Spitze 469) und wurden VOR das Bein des
Spielers gezeichnet, waehrend die restlichen 255 Punkte des Kastens korrekt hinter ihm lagen.
Im Bild: ein heller Splitter schwebt vor dem Bein, der Gegenstand, zu dem er gehoert, ist weg —
genau das liest sich als „fehlende Stuecke auf der rechten Seite".

---

## 1. Reproduktion

| Was | Wert |
|---|---|
| Bild | `re15_port/build/platform/pc/befund_10D0_F3843_marke1.bmp`, 960x720 = genau 3x 320x240 (Blockgleichheit geprueft) |
| Protokoll | `befund.log` ab Zeile 13944 |
| Raum / Winkel | ROOM10D0 Cut 7, Bild F3843 |
| Spieler | `pos (418, 0, 26497)`, `rot -2361`, `vz 6073/5883/5692`, `scr (77,179)`, `kasten x59..95 y118..182` |
| Masken | 104 Rechtecke, Tiefen 53..105, Deckung 5435 Punkte |
| Hintergrund | `build/bg_ppm/ROOM10D07.ppm` |

Kameramatrix nachgerechnet (`mess_22_gegenprobe.py`): der Spieler projiziert auf
`scr (76.5, 179.3)` mit `vz 6052 / 5862 / 5672` — gegen die von der Engine protokollierten
`(77,179)` und `6073/5883/5692`. Abweichung 0,3 % (Festkomma-Rundung). Die Matrix stimmt,
alle folgenden Tiefen-Aussagen ruhen darauf.

## 2. Welche Stuecke fehlen — und in welchem Sinn

Die drei vom Auftrag genannten Faelle sind messbar unterscheidbar. Gemessen
(`mess_02_verlust.py`, `mess_16_urteil.py`, `mess_17_naht.py`):

| Messung | Zahl |
|---|---|
| Bildpunkte, die vom Hintergrund um >30 (Summe \|dRGB\|) abweichen | 2785 |
| davon **ohne jede Maske** (Figur hat den gemalten Hintergrund uebermalt) | **2771** |
| davon **unter einer deckenden Maske** (Renderer-Leck) | **14 = 0,26 %** |
| Punkte des blaugrauen Kastens (273 insgesamt), **ungedeckt** | 255 |
| Punkte des Kastens, **gedeckt mit Tiefe 53..55** (= Stuhltiefe, zu nah) | **18** |

Also **(a) nicht** — der Gegenstand ist im Hintergrund gemalt und wird immer gezeichnet.
Der Augenschein ist **(b)**: die Figur uebermalt ihn. Der DEFEKT ist aber **(c)**: die Maske
malt 18 seiner Texel mit einer fremden, viel zu nahen Tiefe VOR die Figur. Die 255 uebermalten
Texel sind an diesem Standort **richtig** uebermalt — der Gegenstand steht hinter dem Spieler
(§4.3). Was den Eindruck „abgeschnitten" erzeugt, ist nicht das Fehlen der 255, sondern das
Vorhandensein der 18: ein helles Bruchstueck bleibt sichtbar, der Rest ist zu Recht weg.
Der Renderer selbst ist sauber (14 von 2785).

Zeilenausschnitt (`mess_17_naht.py`, Format `Hintergrund/Render/Maskentiefe`, `-` = ungedeckt):

```
y134   x58 153/152/53   x59 351/360/53   x60 225/ 87/-   x61 177/ 79/-   x62 167/ 94/-
y137   x60  88/ 88/54   x61 469/496/54   x62 219/ 69/-   x63 165/ 69/-   x64 192/ 60/-
y140   x61 234/232/54   x62 376/376/54   x63 130/ 66/-   x64 173/ 74/-   x65 176/ 71/-
```

Links der Naht wird der Hintergrund wiederhergestellt (Render == Hintergrund), rechts davon
faellt er auf 60..94 (Hosenbein). Die Naht liegt bei x = 59..62 und wandert mit der Zeile.

**Bildbelege** (Ordner `marke4-10D0-C7-stuhl/`):
`42_augenschein.png` — Hintergrund gegen Render, 20-fach, ohne Gamma: der helle Gegenstand
links, im Render nur noch ein kurzer Splitter.
`37_lupe_gitter.png` — mit Koordinatengitter: rot = Nutzer-Freistellung, gruen = uebrige Deckung.
`33_was_ist_verloren.png` — der Hintergrund NUR an den verlorenen Punkten.
`35_komponenten.png` — die verlorenen Punkte als Zusammenhangskomponenten
(#0 1800 px = die Figur vor Wand/Rollwagen/Pflanze, #1 427 px = Beine ueber Boden und Kasten,
#2 339 px = die Leiche im Flur, also ein anderer Akteur).

## 3. Der Hintergrund als Vorgabe

`ROOM10D0` traegt fuer KEINEN seiner 16 Winkel Original-Kuenstlermasken (Vorarbeit des
Auftrags, im Riegel als erste Pruefung festgehalten). Im Original laeuft die Figur hier also vor
allem. Die Masken dieses Cuts sind eine Zutat des Ports — sie duerfen dem Original nicht
widersprechen, und sie duerfen vor allem nicht FALSCH sein.

Was rechts der Deckung gemalt ist (`mess_12_identifikation.py`, `38_gegenstand_farbe.png`):

| Gegenstand | Bildbereich | Farbe (Median) | b-r |
|---|---|---|---|
| blaugrauer Kasten am Tischende | x46..72, y126..159 | (51,61,71) | **+18** |
| Bodenkegel / Poliermaschine rechts | x72..100, y112..155 | (59,63,70) | +1 |
| Chromrohr des nahen Stuhls | x0..30, y118..150 | (66,68,66) | 0 |
| Chromrohr vorn (gedeckt, Referenz) | y190..230 | — | **+1** (25..75 %: −1..+3) |
| Boden (Referenz) | x72..100, y150..170 | (79/88/100 Helligkeit) | **−8** |

Der helle Strang, der vor dem Bein gezeichnet wird, hat b-r **+5..+18** — er gehoert also zum
blauen Kasten, nicht zum (neutralen) Chromrohr.

## 4. Ursache, belegt

### 4.1 Die Freistellung des Nutzers ist NICHT schuld

| Pruefung | Ergebnis |
|---|---|
| Lage `x=0 y=123` (`mess_04_lage.py`) | **Rang 1 von 30752** Lagen; mittl. \|dRGB\| **2,145** gegen Nullmodell-Median **99,87** und 1 %-Quantil 74,64; zweitbeste Lage 38,03 |
| bitgleich mit dem Hintergrund | 31,6 % bitgleich, 89,3 % mit \|dRGB\| <= 3 |
| Weg in die geschriebene `.PBM` (`mess_01_deckung.py`) | `07_01.png` 2677 Soll, 2677 gedeckt, **0 fehlen**; `07_02.png` 986/986 |
| Git-Verlauf (`mess_05_historie.py`) | in allen drei Fassungen 73x117; opake Punkte 2510 (61607499) → 2677 (a56cff68, 7b24600f); bbox stets x <= 72 — **nie groesser** |

Es gab also nie eine umfangreichere Fassung, und nichts von seiner Arbeit ist verloren gegangen.

### 4.2 Eine Form, zwei Entfernungen

Die Alpha-Maske von `07_01.png` ist **eine einzige** 8er-Zusammenhangskomponente: 2677 Punkte,
x1..72, y123..238 (`mess_26_alpha.py`, Bild `41_alpha_komponenten.png`). Sie umfasst das
Rohrgestell des NAHEN Klappstuhls (unten) und die dunkle Lehne samt hellem Kastenrand (oben).

`geometrie.standpunkte` (geometrie.py:122-140) gibt einer Form mit `"aufrecht": true`
**EINEN** Standpunkt: den tiefsten Punkt der Silhouette, in der Schwerpunktspalte.

```
Standpunkt 07_01: Bild (38.2, 238.5) -> Welt (-1665, 25993) -> Kamera-z 3985 -> Tiefe 62
Zeilenprofil darueber: Tiefe 53..62   (Baubericht: "Profil-Tiefe 53..62")
```

### 4.3 Wo der Kasten wirklich steht

Spaltenweiser Bodenkontakt seiner Silhouette (`mess_15_geometrie.py`):

| Spalte | unterste Zeile | Weltpunkt (x, z) | Kamera-z | Tiefe |
|---|---|---|---|---|
| 58 | 158 | (1613, 27958) | 7456 | 116 |
| 62 | 155 | (1871, 27981) | 7706 | 120 |
| 67 | 154 | (1987, 27854) | 7793 | 121 |

Gegenprobe Bild → Welt → Bild ist exakt: `(58,158) → (1613,27958) → (58.2,158.0)`.

**Nullmodell** (`mess_22_gegenprobe.py`): ein Gegenstand, der am Fuss des Spielers steht
(vz 6073), haette in den Spalten 58 / 64 / 70 seinen Sockel auf **Bildzeile 179**. Der Kasten
hat ihn auf **154..158**, also 21..25 Zeilen hoeher. Zweiter, unabhaengiger Beleg dafuer, dass er
HINTER dem Spieler steht — er braucht die Kameramatrix nicht, nur die Bildzeile des Sockels.

Der Spieler-Fuss bei vz 6073 setzt die wirksame Grenze auf `(1023*6073)>>16 = 94`
(`geometrie.verdeckt`: Maske gewinnt bei Tiefe < Grenze; ZSF3 = 341 `@0x80066c70`/`@0x80066c74`,
Figur-OT = `otz>>4` `@0x8002565c`). Tiefe 116..121 verliert also korrekt — Tiefe 53..55 gewinnt
falsch.

### 4.4 Die Abnahme kann das nicht sehen — das ist selbst ein Befund

`abnahme.py` prueft drei Dinge; keines davon trifft diesen Fehler:

* **(a) Deckung == Soll** ist selbstbezueglich: `soll` ist die Vereinigung der Objekte selbst.
  Vor dem Fix `fehlt 0 / zuviel 0`, nach dem Fix `fehlt 0 / zuviel 0`. Die Pruefung kann nicht
  merken, dass die Objektmenge den gemalten Gegenstand nur zur Haelfte und mit falscher Tiefe
  beschreibt.
* **(b) Standplatz-Schiene** sperrt nur FALSCH-POSITIVE (`VORverd == 0`). Vor dem Fix:
  `Standplaetze 6841, beruehrt 49, VOR n=21 verdeckt=0 teil=0, HINTER n=28 frei=2`.
  `HINTfrei` ist ausdruecklich nur eine Restliste, kein Tor — der Cut wurde mit 2 offenen
  Faellen geschrieben.
* Die HINTER-Frage lautet ausserdem „sind in den Spalten, die UEBERHAUPT Deckung haben, 95 %
  des Koerpers verdeckt?" — nicht „ist der gemalte Gegenstand vollstaendig und mit der
  richtigen Tiefe gedeckt?". Die 18 falschen und die 255 fehlenden Punkte sind ihr beide
  unsichtbar.

### 4.5 `ALPHA_SCHWELLE` ist hier nicht die Ursache

`07_01.png` hat **binaeres** Alpha (nur 0 und 255, gemessen): die Schwelle 110 verwirft
**0** Punkte. (`07_02.png` verliert 55 von 1041 = 5,3 %, spielt fuer diesen Befund keine Rolle.)

## 5. Was die Messung WIDERLEGT hat

1. **„Die Freistellung des Nutzers wurde ueberschrieben / ging verloren."** Nein: 2677 von
   2677 Punkten erreichen die `.PBM`; die Blobs sind seit 7b24600f unveraendert; in der ganzen
   Git-Historie war die Datei nie groesser als 73x117.
2. **„Die Alpha-Schwelle schneidet den Stuhl."** Nein: binaeres Alpha, 0 verworfene Punkte.
3. **„Der Renderer leckt."** Nein: 14 von 2785 uebermalten Punkten (0,26 %) liegen unter einer
   deckenden Maske. Die Nachbildung des Composites mit der ALTEN Maske reproduziert den echten
   Screenshot an 5372 gedeckten Punkten mit 14 Abweichungen.
4. **„Die Freistellung ist eine Farb-Heuristik (und deshalb zerrissen)."** Nein: die beste
   einzelne Helligkeitsschwelle erreicht Jaccard **0,396** gegen ein Bbox-Nullmodell von
   **0,321** — keine Ein-Schwellen-Regel erzeugt diese Form.
5. **„Die Maske sitzt falsch."** Nein: Rang 1 von 30752, 35-fache Trennung zum
   Nullmodell-Median.
6. **„Die Standplatz-Schiene sieht den Standort des Nutzers ueberhaupt nicht."** ⛔ Das hatte ich
   zunaechst gemessen — **mit dem falschen Dump**. `probe_r16_pri_masken_audit` liefert die
   Zell-INNENRAEUME (naechster Punkt zu (418,26497) war (-100,26550), 518 Einheiten weg).
   Richtig ist `probe_p2_floor_dump` (Spieler-Klemmpfad; genau der, den
   `phase3_pri-kreativ/protokoll/bau_alle.sh` benutzt): dort liegen **18** begehbare Punkte im
   Umkreis 600, der naechste (500,26550) in Manhattan-Abstand 135. Die Schiene sieht den
   Standort — sie prueft nur die falsche Eigenschaft (4.4).
7. **„Ein pechschwarzer Fleck frisst den Stuhl."** Weitgehend nein: 1882 Renderpunkte sind
   exakt (0,0,0), aber nur **200** davon liegen auf Hintergrund mit Helligkeit >= 60. Diese 200
   sind der Figurenschatten, als reines Schwarz gezeichnet — eigenes Thema, nicht dieser Befund.
8. **„Der Kasten steht vor dem Spieler und muesste ihn verdecken."** Nein, zweifach widerlegt
   (Sockelzeile 154..158 gegen 179; Kamera-z 7450..7787 gegen 6052). An Marke 4 verdeckt der
   Spieler ihn zu Recht. Deshalb ist der Fix **kein Hinzufuegen von Deckung vor dem Spieler**,
   sondern das Wegnehmen einer falschen.

## 6. Fix

### 6.1 Silhouette aus dem Hintergrund

`bau_huelle.py` → `pri/STAGE1/10D0/07_03.png`, 27x34, Lage (46,126), 273 opake Punkte.

| Schritt | Messwert |
|---|---|
| Fenster | x46..72, y124..159 |
| Kernregel | `b-r >= 0` UND `r+g+b >= 120` |
| Trennguete | **Boden-Fehlalarm 0 von 609** Referenzpunkten (x72..100, y150..170); Treffer 69,1 % der Gegenstandsreferenz |
| groesste 8er-Komponente | 262 von 270 Kernpunkten (naechste Komponenten 5 / 2 / 1) |
| 3x3 schliessen + Loecher fuellen | **+11 Punkte = 4 %** der Huelle |
| Ergebnis | 273 Punkte, x46..72 y126..159, **0 %** bodenartig (b-r < −4), Helligkeit Median 171, b-r Median +18 |

Das Fuellen erfindet keine Form: ein Kasten ist solide, seine Silhouette IST eine gefuellte
Flaeche (gleiches Argument wie bei der Pflanzen-Huelle in ROOM10D0 C6). 4 % Zuwachs ist der
Beleg, dass die Kernregel die Form schon fast allein traegt.

### 6.2 Eigene Tiefe

Eintrag in `analysis/esp_masken_2026-09-03/auswahl.json`, ROOM10D0 „7":

```json
{ "name": "Blaugrauer Kasten am Tischende", "png": "pri/STAGE1/10D0/07_03.png",
  "x": 46, "y": 126, "massstab": 1, "oben": 0, "grow": 0, "aufrecht": true }
```

`aufrecht` ist hier richtig: ein Kasten steht senkrecht, sein tiefster Silhouettenpunkt IST sein
Bodenkontakt. Gemessen: Standpunkt Bild (61.6, 159.5) → Kamera-z **7337**, Zeilenprofil
6990..7337 → **Tiefe 109..114**. Kein `zelle`-Eingriff: beide Varianten gebaut, das Werkzeug
liefert von sich aus das Profil (die Zellen-Zuordnung greift nicht), ein Override waere unbelegt.

### 6.3 Punktgenauer Abzug statt Rechteck

Neuer Schluessel `minus_png` in `bau_p2.kaesten_anwenden`: zieht eine andere Silhouette
**punktgenau** ab. Gebraucht, weil die Grenze keine Gerade ist — gemessen (`bau_huelle.py`):

```
Ueberlappung Huelle & Nutzer-Freistellung: 18 Punkte, x58..62 y134..159,
   alle hell (>=120), Helligkeit Median 341, b-r Median +10
```

Ein Rechteck x58..62 / y133..141 wuerde dort 33 Punkte treffen: die 18 Kastenpunkte **und 15
fast schwarze Stuhlpunkte** (Helligkeit Median 18). „Die Bbox ist nicht die Kunst" — deshalb
punktgenau. Die **Vereinigung** beider Objekte enthaelt weiter jeden Punkt der Freistellung des
Nutzers; nur die 18 Punkte wechseln vom nahen zum fernen Tiefen-Objekt.

## 7. Verifikation

### 7.1 Bau

| | vorher | nachher |
|---|---|---|
| Soll | 5435 px | **5690 px** |
| Deckung fehlt / zuviel | 0 / 0 | **0 / 0** |
| Rechtecke | 104 | **104** (unveraendert, Engine-Grenze 105 gehalten) |
| Tiefenstufen | 53 (53..105) | 59 (53..114) |
| Atlaspunkte | 25887 | 27937 |
| VOR n / verdeckt / teil | 21 / **0** / 0 | 21 / **0** / 0 |
| HINTER n / frei | 28 / 2 | 42 / 36 (s. 7.4) |

### 7.2 Composite-Nachbildung, mit Selbstpruefung

Regel wie in der Engine: Maske gewinnt bei `Tiefe < (1023*vz)>>16`, `vz` je Bildzeile aus der
senkrechten Weltlinie durch (418, 26497) (`geometrie.profil_spalte`).

* **Selbstpruefung:** 5372 Punkte, an denen die ALTE Maske gewinnt; davon weichen **14
  (0,26 %)** vom Hintergrund ab. Die Nachbildung reproduziert den echten Screenshot, ist also
  fuer das Nachher-Bild brauchbar.
* **Wirkung an Marke 4:** genau **18 Punkte** werden nicht mehr vor den Spieler gezeichnet
  (x58..62, y134..159, Hintergrund-Helligkeit Median 341); **0 Punkte** kommen hinzu — weil der
  Kasten hier hinter dem Spieler steht. Bild `50_vorher_nachher.png` (gelb = die 18 Punkte)
  und `51_vorher_nachher_gross.png`.

### 7.3 Wirkung ueber ALLE begehbaren Standplaetze

`mess_30_wirkung.py`, **Abdeckung: 6841** begehbare Standplaetze Band 0
(`probe_p2_floor_dump`, 200er-Raster):

| | Anzahl | Summe Koerperpunkte |
|---|---|---|
| Standplaetze mit Maskenwirkung | 397 | |
| **mehr** verdeckt (der Kasten wirkt jetzt) | **163** | **+10016** (Median +44) |
| **weniger** verdeckt (der Splitter faellt weg) | 21 | −167 (Median −6) |
| unveraendert | 213 | |

Groesste Zunahme (1700, 27950): **321 → 569** von 1000 Punkten des Koerperkastens — genau der
Platz direkt hinter dem Kasten. Deckungskarte: `52_dahinter_deckungskarte.png` (das ist eine
Deckungskarte, kein Renderbild — von diesem Standplatz existiert kein Screenshot).

### 7.4 Ehrlich zur Kennzahl: HINTfrei steigt von 2 auf 36

Das ist **keine** Verschlechterung, sondern ein Fehler der Kennzahl. `abnahme.standlinie` nimmt
je BILDSPALTE die NAECHSTE Standlinie ueber alle Objekte. Traegt eine Spalte Punkte von zwei
Gegenstaenden in verschiedener Entfernung, vergleicht die Schiene den Standplatz mit der Linie
des NAHEN und verlangt Verdeckung auch durch die Punkte des FERNEN. Gemessen fuer alle 12 neu
gelisteten Standplaetze (`mess_31_schiene_kritik.py`):

```
Standlinie Stuhl 3985 (Tiefe 62) | Standlinie Kasten 7337 (Tiefe 114)
(-1500,24950) Kamera-z 3952 | ( 500,26350) 6103 | ( 500,26550) 6139 | ( 500,26950) 6211
( -900,25550)         4630 | ( 500,26750) 6175 | ( 500,27150) 6247 | ( -300,25750) 5236
-> ALLE liegen VOR der Standlinie des Kastens. Der Kasten DARF sie nicht verdecken.
```

Die Behebung braucht eine Bewertung von VOR/HINTER **je Objekt** statt je Spalte in
`abnahme.py`. Das verschiebt die Zahlen jedes Raums und ist in dieser Runde **nicht** gemacht —
offener Punkt.

### 7.5 Riegel und Gegenprobe

`re15_port/tests/unit/probes/r19_marke4-10D0-C7-stuhl.cmake` →
`test_r19_marke4_kasten` (`unit_r19_marke4_kasten`), auf dem ECHTEN Ladeweg
(`re15_pri_msk_section_offset` → `re15_pri_parse_section` → `re15_tim_parse` → Blit,
Palettenindex != 0, je Punkt die NAECHSTE Tiefe). Referenzdaten:
`probes/r19_marke4_kasten.pbm` (273 Punkte) und `probes/r19_marke4_nutzer.pbm` (3641 Punkte).

Mit dem Fix:

```
PASS: ROOM10D0 Cut 7 hat im Original keine Masken
Kasten: 273 Punkte, 0 ungedeckt, Tiefen 109..114, 0 ausserhalb 109..114
Nutzer-Freistellungen: 3641 Punkte, 0 ungedeckt
PASS: jeder Punkt des fernen Kastens ist gedeckt
PASS: kein Punkt des fernen Kastens traegt eine fremde (nahe) Tiefe
PASS: die Freistellungen des Nutzers bleiben vollstaendig gedeckt
```

**Gegenprobe** — dieselbe exe gegen die alten `ROOM10D0.MSK` / `_PRI07.TIM`:

```
ZU NAH: (58,134) Tiefe 53 (erwartet 109..114)   ... 12 von 18 gelistet
Kasten: 273 Punkte, 255 ungedeckt, Tiefen 53..55, 18 ausserhalb 109..114
FAIL: jeder Punkt des fernen Kastens ist gedeckt
FAIL: kein Punkt des fernen Kastens traegt eine fremde (nahe) Tiefe
PASS: die Freistellungen des Nutzers bleiben vollstaendig gedeckt
EXIT=1
```

Der Riegel trennt also beide Anliegen: er faellt auf der alten Tiefe und bestaetigt gleichzeitig,
dass die Handarbeit des Nutzers in beiden Staenden vollstaendig gedeckt ist.

### 7.6 Gesamtsuite

`RE15_BUILD_DIR=re15_port/build_r19_marke41007stuhl bash re15_port/tools/local_build.sh all`:

| Durchlauf | Ergebnis |
|---|---|
| 1 (Fix geschrieben, Hash-Liste noch alt) | 331 Tests, **330 bestanden**, 148 s |
| 2 (Hash-Liste neu erzeugt) | **331/331**, 148 s |
| 3 (beide Riegel registriert) | **332/332**, 152 s |

Der einzige Fehlschlag im ersten Durchlauf war `unit_pri_hashes` — er hat
korrekt gemeldet, dass `pri/STAGE1/10D0/07_03.png` nicht in seiner Liste steht
(„UNGELISTET", 118 Dateien im Baum, 117 gelistet, 0 veraendert). Liste mit
`tools/maske/pri_hashes.py` neu erzeugt.

## 8. Nebenbefund derselben Marke: der Koerperkasten ist halb so hoch wie die Figur

Beim Nachmessen fiel auf, dass der **Koerperkasten**, mit dem `befund.log` sein „DECKT/frei"
urteilt (Hoehe **1700**, `platform/pc/main.c`) und mit dem die Masken-Abnahme ihren Riegel setzt
(`KOPF = 1500`, `tools/maske/abnahme.py:30`), viel kleiner ist als die gezeichnete Figur.

Gemessen am Abzug F3843 (`mess_33_koerperkasten.py`, Figur = Diff Render/Hintergrund > 30, Flur
mit der Leiche ausgeblendet):

| | Zeilen | Spalten | Punkte |
|---|---|---|---|
| gezeichnete Figur | **74..187** | **42..95** | 2366 |
| Kasten aus `befund.log` | 118..182 (66 = **57 %**) | 59..95 | davon **1023 = 43,2 %** |

Und die Umkehrung, mit der Engine-Kamera gerechnet: Welthoehe **3000** ueber dem Fuss
projiziert auf Bildzeile **73,8** — also genau auf die oberste Figurzeile 74. 1700 landet auf
Zeile 122, 1500 auf Zeile 129, beide mitten in der Figur.

Die halbe Breite 450 liefert die Spalten 45..95: rechts genau, links **3 Spalten zu wenig**
(3 von 54 = 5,6 %). ⛔ Die Notiz eines frueheren Durchlaufs, „die halbe Breite passt", war zu
grosszuegig und ist hier berichtigt. Der tragende Teil ist die **Hoehe**: 44 von 114 Zeilen
fehlen, Faktor 2.

Festgehalten, **nicht behoben**: die Konstante verschiebt die Zahlen jedes Raums und jeder
Marke. Riegel `unit_r19_koerperkasten` (`probes/probe_r19_koerperkasten.c`) haelt die Messung
fest — wer 1500/1700 anfasst, muss dort vorbei.

## 9. Offene Punkte / Grenzen dieses Befunds

1. `abnahme.py` bewertet VOR/HINTER je Bildspalte statt je Objekt (7.4). Solange das so ist,
   bestraft die Kennzahl richtige Daten.
2. Die Deckung ist nur eine Pruefung gegen die Objektmenge selbst. Es fehlt eine Pruefung
   „**deckt die Objektmenge den gemalten Gegenstand?**" — sie haette diesen Befund vor dem
   Nutzer gefunden.
3. Die Huelle des Kastens ist **meine** Silhouette, keine Freistellung des Nutzers. Trennbar ist
   er nur, wo er hell/blau ist; sein dunkler Fuss unterhalb Zeile 159 laesst sich farblich nicht
   vom Boden trennen, die Huelle endet dort.
4. 200 Renderpunkte sind reines Schwarz auf Hintergrund mit Helligkeit >= 60 — der
   Figurenschatten. Eigenes Thema.
5. Fuer den Standplatz hinter dem Kasten (7.3) liegt nur eine Deckungskarte vor, kein
   Screenshot.
6. Der Koerperkasten (§8) ist halb so hoch wie die gezeichnete Figur — gemessen,
   festgehalten (`unit_r19_koerperkasten`), nicht behoben.

## 10. Dateien

* Dossier: `analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl.md`
* Bilder und Messskripte: `analysis/befunde_2026-09-21/pri-runde19/marke4-10D0-C7-stuhl/`
* neue Silhouette: `pri/STAGE1/10D0/07_03.png`
* Auswahl: `analysis/esp_masken_2026-09-03/auswahl.json` (ROOM10D0 „7")
* Werkzeug: `re15_port/tools/maske/bau_p2.py` (`minus_png`)
* Riegel: `re15_port/tests/unit/probes/r19_marke4-10D0-C7-stuhl.cmake`,
  `probes/test_r19_marke4_kasten.c`, `probes/probe_r19_koerperkasten.c`,
  `probes/r19_marke4_kasten.pbm`, `probes/r19_marke4_nutzer.pbm`
* geschriebene Daten: `re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK`,
  `ROOM10D0_PRI07.{TIM,PBM,STAND}`
* Hash-Liste: `re15_port/tests/unit/pri_stage1_hashes.inc`
