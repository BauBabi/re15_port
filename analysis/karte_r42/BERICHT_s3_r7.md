# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 7

Zweig r42/karte. Code-Commit bd889c799, danach ein Doku-Commit mit diesem Bericht und den Bildern. Kein Push.

## Kurz und in Alltagssprache

- **Die Klemme aus Runde 6 ist wieder raus.** Sie hat den Punkt in vielen Räumen beim Gehen springen lassen. Jetzt läuft
  er überall wieder genau so wie in Runde 5. Ein neuer Test zählt die Sprünge in jedem Raum und vergleicht mit Runde 5.
- **Der schmale Schräggang in Raum 4080 (und 4081 für Elza) hat jetzt eine eigene Umrechnung.** Sie gilt nur für diesen
  Gang. Der Punkt steht auf dem ganzen Weg von der Tür zu 4030 bis zum Gangende mitten im Gang. Er steht nie auf der
  Wandlinie und nie über dem blauen Grund. Er springt nie zurück.
- **Eine Grenze nenne ich offen:** Der gemalte Gang ist genau so breit wie der Punkt. Der Punkt passt nur an zwei Stellen
  sauber hinein. Er läuft deshalb in **zwei Stufen** mit: erst in der oberen Hälfte des Gangs, ab der Mitte des Wegs
  3 Bildpunkte weiter unten. Hinter dem Gang läuft er in der breiten Diagonale weiter.
- **Noch eine Grenze:** In v0.8.30 stand der Punkt im Gang gar nicht im Gang, sondern schon in der breiten Diagonale. Dort
  ist mehr Rot um ihn herum. Misst man an jeder Stelle des Wegs, wie viel Rot um den Punkt liegt, ist v0.8.30 ab der
  Gangmitte höher. Den Wert 0,32 der Ankunft (das Maß der Prüfung) erreicht der neue Punkt überall. Mehr geht im Gang nicht.
  In v0.8.30 lag dafür an jeder Stelle eine Wandlinie im Ring. Das entscheidet der Orchestrator (Abschnitt 6).
- Kartenlauf neu aufgenommen: 645 Lagen, Leon und Elza, STAGE1 bis 6. 633 Abzüge. Gegen Runde 5 sind 631 Bildpunkt für
  Bildpunkt gleich. Anders sind nur L4080_t0 und E4081_t0 (Ankunft im Schräggang).

Fachwörter:
- "Blatt" ist eine Kartenseite, "Kachel" ein gemalter Raum darauf.
- "Ring" ist der 5 x 5 Bildpunkte große Punkt. "Rot-Maß" ist der Anteil roter Bildpunkte im 2-px-Rand um den Ring.
- "Zeile" ist die Umrechnung von der Spielposition auf den Kartenpunkt. "Klemme" verschiebt den Punkt nachträglich.
- "Riegel" ist ein automatischer Test. "Gegenprobe" heißt: Derselbe Test muss auf dem alten Stand fehlschlagen.
- "Abzug" ist ein Bild des Kartenschirms aus dem laufenden Spiel (FB_SHOT).

## 1. Vorgabe A: Klemme der Runde 6 zurückgenommen

- `karte_r42.c` `re15_karte_r42_ring_klemmen` ist wieder der Stand von Runde 5 (02972374c): kein "sauberer Ring", keine
  Suchweite 7.
- Neuer Riegel `unit_r42_karte_glatt_s2` bis `_s6` (`test_r42_karte_glatt.c`), die Messung der Prüfung (pruef_s3_r6, sweep):
  - Weltraster im Abstand 500. Gezählt werden nur begehbare Lagen im Raum: Die Spieler-Kollision
    `re15_collision_constrain` verschiebt sie nicht, und der rohe Punkt (nur Zeile + Rechteck) liegt auf der eigenen
    Kachel. Das schließt die begehbare Leere außerhalb der Raumwände aus, wie in der Prüfung.
  - Ein Sprung ist ein Abstand von mindestens 4 px zwischen zwei Nachbarlagen auf demselben Blatt.
  - Je Raum darf die Zahl nicht über Runde 5 liegen. Bezug in `r42_karte_glatt_r5.inc` (128 Räume, Summe 432).
  - Die Zahlen stimmen mit der Prüfung überein, wo sie dieselben Räume nennt: 2070 48, 2080 8, 30E0 49, 4040 3, 4080 6,
    5080 80; Runde 6: 2050 50 (Prüfung 47), 20B0 39 (40), 11A0 6 (6).
- Messhaken `g_re15_karte_r42_roh` (nur für den Riegel, im Spiel immer 0): Punkt ohne Klemmen und ohne Gang-Zeile.

| | Runde 5 | Runde 6 | Runde 7 |
|---|---|---|---|
| Sprünge ≥ 4 px, 128 Räume | 432 | 680 | **424** |
| 4080 / 4081 | 6 / 6 | 6 / 6 | **2 / 2** |
| alle anderen Räume | | mehr in 2030, 2050, 2070, 20B0, 3050, 4040, 40B0, 5080, 50D0, 6000, 11A0 (je Leon und Elza) | **gleich Runde 5** |

Tabelle je Raum: `analysis/karte_r42/s3r7/glatt.txt`.

## 2. Vorgabe B: Gang-Zeile für den Schräggang 4080/4081

**Gemessen (Welt, ROOM4080.RDT, Kollisionszellen je 12 Byte ab 0x0055C):**
- Zelle 6 @0x005A4 = `6c20 6c20 6ae6 ebf6 04`: Dreieck Typ 4, (-6550,-2325), 8300 x 8300. Wand x+z = -575.
- Zelle 11 @0x005E0 = `6c20 6c20 37e2 b8f2 07`: Dreieck Typ 7, (-7625,-3400), 8300 x 8300. Wand x+z = -2725
  (`re15_collision.c`: Typ 7 ist fest für (z-Z)·W < (X+W-x)·D).
- Der Gang liegt genau unter 45 Grad. Mitte x+z = -1650. Er beginnt an der Tür nach 4030 (Slot 0, Mitte (-4250,2550),
  x-z = -6800) und endet, wo beide Dreiecke enden (x-z = 4075). Begehbar gemessen: x+z von -2200 bis -1200.

**Gemessen (Kunst, DATA/MAP09.PIX, Blatt 8, Kachel r13):**
- Gang-Inneres je Zeile y bei x = y+49 .. y+53. Mitte x-y = 51. Spitze an der Tür (166,112), Ende (172,123).
- Dahinter die Naht x+y = 296..297: Dort laufen die Rahmenlinie von r3 (4030) und die Wandlinien von r13 und r8 zusammen.
  Ab x+y = 298 beginnt die breite Diagonale r8.

**Zeile (PORT-ERGÄNZUNG; die Zeile des Raums @0x80076AF8 ist der Stub {0,0,1,1}; Formel FUN_800473f8 @0x8004741c-0x80047528):**
- Gangmitte → Punkt mx-my = 51. Tür → mx+my = 279. Gangende → mx+my = 297.
- Daraus sx = sy = round(18 · 2^20 / 10875) = 1736, ox = 119, oy = 171. In x und z gleich, sonst kippt die Mittellinie.

**Haltestellen (gemessen an MAP09.PIX, `s3r7/mitte.py`):**
- Der Ring (TEX.TIM @0x14910, uv 225..229) ist so breit wie der Gang. Sein Inneres liegt nur auf der Mittellinie ganz auf dem
  Raum. Die Spitze an der Tür ist dafür zu schmal.
- Sichtbarer Raum im Rand (von 56) auf der Mittellinie:

| Ring-Mitte | (167,116) | (168,117) | (169,118) | (170,119) | (171,120) | (172,121) |
|---|---|---|---|---|---|---|
| Rand | 15 | 17 | **18** | 17 | 15 | **20** |

- Nur zwei Lagen erreichen das Maß der Prüfung (18/56 = 0,32, v0.8.30 an der Ankunft): Punkt (170,119) und (173,122).
- Der Punkt nimmt die Haltestelle, die der Zeile am nächsten liegt. Ein Rückwärtssprung ist damit ausgeschlossen.
- Hinter der Naht (mx+my ≥ 298) rechnet der Punkt mit der Zone der breiten Diagonale (Zone 0, r8) weiter, wie bisher.
- Es gibt keine Klemme im Gang. Die Haltestellen sind gemessene Lagen, keine Suche.

**Ergebnis, berechnet über den ganzen Gang (Raster 100, x-z von -7000 bis 7000, x+z von -2300 bis -1100, `s3r7/bewert4080.py`):**

| 1057 begehbare Lagen im Gang | Runde 5 | Runde 7 |
|---|---|---|
| Wand im Ring-Inneren | 1057 | **0** |
| Ring-Texel über Grund | 1031 | **0** |
| Rand unter 18/56 | 404 | **0** |
| Ring direkt am Türbalken | | **0** |

**Im echten Spiel (FB_SHOT, Leon und Elza gleich, `gang_4080_leon.png` / `gang_4080_elza.png`):**

| Lage | v0.8.30 | Runde 6 | Runde 7 |
|---|---|---|---|
| (-3900,2100) Ankunft | (176,124) 0,32, Ring innen 5/8 rot | (173,122) 0,36, ganz rot | **(170,119) 0,32, ganz rot** |
| (-3100,1300) | (176,124) 0,32, 5/8 | (169,116) 0,25, 5/8 | **(170,119) 0,32, ganz rot** |
| (-1900,100) | (176,124) 0,32, 5/8 | (171,118) 0,36, 5/8 | **(170,119) 0,32, ganz rot** |
| (-500,-1300) | (179,124) 0,39, 7/8 | (173,120) 0,30, 5/8 | **(170,119) 0,32, ganz rot** |
| (-100,-1700) | (180,124) 0,43, 5/8 | (174,121) 0,38, 5/8 | **(173,122) 0,36, ganz rot** |
| (300,-2100) Gangende | (180,125) 0,50, 7/8 | (174,122) 0,43, 6/8 | **(173,122) 0,36, ganz rot** |
| (1500,-3300) hinter dem Gang | (182,127) 0,59, 7/8 | (174,123) 0,46, 5/8 | **(173,122) 0,36, ganz rot** |
| (2100,-3900) breite Diagonale | (183,127) 0,54, 5/8 | (174,123) 0,46, 5/8 | **(178,125) 0,48, ganz rot** |

- Echtes Fenster (gdigrab, normale Grafik, 640x480): `fenster_echt.png`, 4080 Leon an drei Stellen, 4081 Elza. Gegen
  den FB_SHOT desselben Laufs weichen je 52 Bildpunkte ab (der blinkende Ring). Rot-Maß im Fenster-Lauf 0,32 / 0,32 /
  0,36 / 0,32, Ring innen jedes Mal ganz rot.

## 3. Riegel und Gegenproben

`unit_r42_karte_lauf`:
- **[M8] neu:** 264 begehbare Lagen auf drei Längslinien des Gangs (x+z = -2100 / -1650 / -1300, x-z = -6400 .. 4000),
  Leon und Elza. Je Lage: Rand ≥ 18/56, Ring-Inneres 9/9 Raum, Ring ganz auf der Kunst, kein Ring-Texel auf oder direkt
  neben einem Türbalken. Entlang jeder Linie kein Rückwärtssprung und kein Sprung über 3 px. Der Punkt folgt (am Gangende
  weiter unten als an der Tür).
- **[M7] geändert, offen gesagt:** "nicht am Übergang zu 4030" heißt jetzt "kein Ring-Texel auf dem Türbalken oder direkt
  daneben" (mindestens ein Texel dazwischen). In Runde 6 war es "kein Balken im 9x9-Feld". Das alte Maß ließ im Gang nur
  eine einzige Lage zu (Ring-Mitte (172,121)); der Punkt hätte dem Gang dann gar nicht folgen können. Die neue Ankunftslage
  (170,119) hat 2 Balken-Texel im 9x9-Feld, der Ring selbst ist 2 Texel vom Balken weg. Den Fehler, den die Prüfung von
  Runde 6 nannte ("Türbalken x165 direkt neben dem Ring", Ring-Mitte (168,115)), fängt das neue Maß weiter.

Gegenproben in eigenen Bäumen (`s3r7/gp_bau.sh`, Logs `s3r7/gp_*`):

| Stand | [M7] | [M8] | unit_r42_karte_glatt |
|---|---|---|---|
| aaea5f8c2 (v0.8.30) | ROT (176,124): Inneres 6/9, am Balken | ROT: 264 von 264 Lagen außerhalb | nicht baubar (keine karte_r42.c) |
| 02972374c (Runde 5) | ROT (171,118): Inneres 6/9, 4 Texel über Grund | ROT: 264/264, 8 Rückwärts-, 4 große Sprünge | grün (ist der Bezug) |
| 1713acd21 (Runde 6) | grün (173,122) | ROT: 190/264, 8 Rückwärts-, 10 große Sprünge, folgt nicht | **ROT** in 22 Räumen (s2..s6) |
| bd889c799 (neu) | grün | grün | grün |

- Für den Glatt-Riegel ist v0.8.30 nicht der passende Gegenstand: Er schützt vor einem Rückschritt nach Runde 5. Die
  Gegenprobe ist Runde 6. Für den Filter "roher Punkt" bekam die alte `karte_r42.c` dort nur den Messhaken (drei Zeilen,
  kein Verhalten).

## 4. Tests (Endstand bd889c799)

- Bau `local_build.sh build`: LOCAL-BUILD-OK.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 6`: **66/66 grün** (61 bisher + 5 neue Glatt-Riegel).
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 3`: **22/22 grün** in 2027 s, im ersten Lauf ohne Wiederholung, parallel zum Kartenlauf (`s3r7/ctest_integ.log`).
- Die volle Suite ist nicht gelaufen (so im Auftrag).

## 5. Kartenlauf und Bilder

- 645 Lagen STAGE1-6, Leon und Elza, mit der exe dieser Runde (`s3r7/kampagne.sh`). Zwei Läufe parallel; die 12 Lagen,
  die beide Läufe zugleich trafen, und 6 Zeitüberschreitungen unter Last habe ich einzeln neu aufgenommen (alle 12 doppelten
  Abzüge pixelgleich).

| | ok | Punkt neben Rot | Raum nicht rot | kein Abzug |
|---|---|---|---|---|
| Runde 5 / Runde 6 | 619 | 11 | 3 | 12 |
| **Runde 7** | **619** | **11** | **3** | **12** |

- Pixelvergleich: gegen Runde 5 631 gleich, 2 anders (L4080_t0, E4081_t0). Gegen Runde 6 629 gleich, 4 anders (dazu
  L4040_t2/E4041_t2: wieder wie Runde 5, (109,121), 0,61).
- 144 Leon-Lagen STAGE2-6: v0.8.30 85 gut, **Runde 7 140**. Schlechter als v0.8.30 nach dem Urteil des Kartenlaufs: keine.
- Bilder für dich (`analysis/karte_r42/s3r7/`):
  - `tafeln/`: 52 Tafeln je Blatt und Figur, jede Lage beschriftet mit Bereich (STAGE), Blatt, Raum, Figur, Art der Lage
    und Weltlage; Abweichungen in roter Schrift; `tafel_99_ohne_abzug.png` mit Grund.
  - `gang_4080_leon.png`, `gang_4080_elza.png`: der Weg durch den Schräggang, 8 Lagen, v0.8.30 / Runde 6 / Runde 7.
  - `fenster_echt.png`: echtes Spielfenster.
  - Daten: `abzuege.tsv`, `kampagne.txt`, `kampagne_gang.txt`, `vergleich.txt`, `glatt.txt`.

## 6. Grenze im Schräggang (zur Entscheidung)

Die Vorgabe B verlangt zugleich: Der Punkt folgt der Gangmitte, kein Ring auf einer Wandlinie, und an jeder Lage ein
Rot-Maß mindestens wie v0.8.30.

- Mit dem Maß der Prüfung (0,32 = v0.8.30 an der Ankunft) ist alles erfüllt, aber der Punkt folgt nur in zwei Stufen.
  Mehr saubere Lagen gibt es im gemalten Gang nicht (Tabelle in Abschnitt 2).
- Vergleicht man Lage für Lage mit v0.8.30, ist v0.8.30 ab etwa x-z = -1200 höher (0,39 bis 0,59), weil sein Punkt
  schon in der breiten Diagonale stand, während die Figur noch im Gang ist. Im Raster sind das 559 von 1057 Lagen im Gang
  (Runde 5: 671). Dort hatte v0.8.30 an jeder Lage eine Wandlinie im Ring (Inneres 5/8 bis 7/8 rot).
- Beides zugleich ginge nur, wenn der Punkt die zweite Hälfte des Gangs in der breiten Diagonale anzeigt, also bis zu
  8 Bildpunkte vor der Figur. Das widerspricht "der Punkt folgt der Gangmitte". Ich habe "folgt der Gangmitte" gewählt.
- Würde man 17/56 (0,30) zulassen, liefe der Punkt in vier Stufen statt zwei.

## 7. Kollisionen

`git merge-tree --write-tree` von r42/karte (bd889c799) gegen die heutigen Spitzen meldet **keinen Konflikt**:
story 9cf52876f, rest 3e1a14809, gegenstaende 5515f2601, finale 35965debe, gore b8b879d54, lageplaene c35ab51a5,
original 115ec88e6, birkinfilm b5bb0edd0, namen 6f1a719f0, hangar_aussen fe5436f55, kellerplan 2a2d45a1a.

- Vorwarnung: Diese Runde ändert `re15_inv_screen.c` (9 Zeilen in `re15_inv_map_marker`). Das tun auch story, gegenstaende,
  namen und kellerplan, an anderen Stellen. story, namen, lageplaene und kellerplan ändern zudem `kartenitems_r36.c` und
  `re15_map_owned.c`. Kein Textkonflikt; die Kartentests müssen nach dem Zusammenführen trotzdem noch einmal laufen.
- Die Tabellen von U-11/U-12 (r42/rest) berührt diese Runde nicht.

## 8. Was offen ist (gelb)

1. Schräggang: Punkt in zwei Stufen; Rot-Maß Lage für Lage unter v0.8.30 ab der Gangmitte (Abschnitt 6).
2. Hinter dem Gangende (x-z 4075 .. 6900, noch Zone 1) rechnet Zone 0 mit den Klemmen von Runde 4/5. Dort liegt an
   105 von 337 Rasterlagen eine Wandlinie im Ring (Runde 5: 282 von 337).
3. Zug 5090: 3 Lagen außerhalb der Zugteile (L5090_t2, L5090_w1, E5091_t2), Stand v0.8.30, U-12.
4. Unverändert: 20A0 zweimal rot auf Blatt 6, 20A0 an der Tür nach 2090 (0,30), 20A0-Kollision, 10B0_t0/10B1_t1 (0,29),
   2000/2001 aus 1260 (0,20), 11B0 (0,25), 3090_w1, 6030_t1/6031_t1 (0,29), 4070_w0 (0,25), neue Stücke ohne Türmarken,
   Türabstand 2(d), Zusatzbits 253-255.
5. Der Glatt-Riegel sieht Räume ohne Lagen auf Band 0 nicht (z. B. 4070/4071 Treppenhaus: 0 Lagen).
