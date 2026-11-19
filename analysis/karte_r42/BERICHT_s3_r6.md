# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 6

Zweig r42/karte. Code-Commit 1713acd21, danach ein Doku-Commit mit diesem Bericht und den Bildern. Kein Push.

## Kurz und in Alltagssprache

- **Der rote Punkt der letzten Prüfung ist behoben: Raum 4080, Ankunft aus 4030.**
- Vorher lag der Ring des Punkts halb auf der Wand des schmalen Schräggangs. Ein Stück ragte über den blauen Grund
  hinaus. Und er stand oben am Gang, direkt unter der Tür zu 4030.
- **Jetzt steht der Punkt mitten im Gang, an dessen unterem Ende.** Im Inneren des Rings liegt nur roter Raum. Kein
  Bildpunkt des Rings liegt über blauem Grund. Die Tür zu 4030 ist 5 Bildpunkte entfernt.
- Der Messwert ist 0,36. In v0.8.30 war er 0,32. Er ist also mindestens so gut wie früher.
- Elza (4081) hat genau dasselbe Bild.
- **Nebenbei besser:** Raum 4040, Ankunft aus 4050. Der Punkt rückt 1 Bildpunkt nach links. Er liegt jetzt ganz im
  Raum statt halb auf der Wand (Messwert 0,61 auf 0,68).
- **Sonst hat sich nichts bewegt.** Ich habe den ganzen Kartenlauf neu aufgenommen: 645 Lagen, Leon und Elza,
  STAGE1 bis 6. Von 633 Kartenbildern sind 629 Bildpunkt für Bildpunkt gleich wie in Runde 5. Die 4 anderen sind genau
  4080/4081 aus 4030 und 4040/4041 aus 4050.
- Jeder Raum mit Karte zeigt weiter ein Stück Karte mit mitlaufendem Punkt: 200 von 200.

Fachwörter:
- "Blatt" ist eine Kartenseite, "Kachel" ein gemalter Raum darauf.
- "Ring" ist der 5 x 5 Bildpunkte große Punkt selbst.
- "Riegel" ist ein automatischer Test. "Gegenprobe" heißt: Derselbe Test muss auf dem alten Stand fehlschlagen.
- "Abzug" ist ein Bild des Kartenschirms direkt aus dem laufenden Spiel (FB_SHOT).
- "Rot-Maß" ist der Anteil roter Bildpunkte im 2-px-Rand um den Ring. Ab 0,3 steht der Punkt "auf dem Raum".

## 1. Der rote Punkt: 4080/4081 aus 4030

**Gemessen (Abzug Runde 5, DATA/MAP09.PIX Blatt 8, Kacheln r8 + r13 = 4080, r3 = 4030):**
- Der Schräggang ist innen genau 5 px breit, so breit wie der Ring. Je Zeile y liegt das Innere bei x = y+49 .. y+53.
- Der Ring kann darin nirgends ganz frei von der Wand liegen. Auf der Mittellinie berührt er die Wände aber nur mit
  seinen Ecken. Das Innere (3 x 3) bleibt dann frei.
- Kandidaten auf der Mittellinie, gezählt an der Kunst (Rand-Rot von 56):

| Ring-Mitte | Punkt | Rand | Inneres frei | Türbalken 4080↔4030 im 9x9-Feld |
|---|---|---|---|---|
| (168,117) | (169,118) | 17 = 0,30 | ja | ja |
| (169,118) | (170,119) | 18 = 0,32 | ja | ja (2 Texel) |
| (170,119) | (171,120) | 17 = 0,30 | ja | nein |
| (171,120) | (172,121) | 15 = 0,27 | ja | nein |
| **(172,121)** | **(173,122)** | **20 = 0,36** | **ja** | **nein** |

- Der Vorschlag der Prüfung (Ring-Mitte etwa (171,120)) hat nur 0,27 und wäre schlechter als v0.8.30.
- Nur (172,121) erfüllt alles zugleich: Rand ≥ 0,32, Inneres frei, kein Grund im Ring, Türbalken außerhalb des Felds.

**Änderung (karte_r42.c, PORT-WAHL für die Anzeige, keine Spielkonstante):**
- Neue Prüfung "sauberer Ring": Die 3 x 3 Texel im Ring liegen auf sichtbarem Raum (keine Wand, kein Grund, keine
  Tür- oder Treppenmarke). Keiner der 12 Ring-Texel liegt außerhalb der Kunst des Raums. Die eigene Wand an den
  Ring-Ecken ist erlaubt. Dazu muss der Rand mindestens 30 % Raum haben (das bisherige Auslösemaß).
  Ring-Form: TEX.TIM @0x14910, uv 225..229, Ecken leer.
- Die Klemme zieht einen sauberen Ring jedem unsauberen vor. Ausgelöst wird sie wie bisher (Ring auf fremder Kachel
  oder Rand unter 30 %). Sonst gilt die Reihenfolge aus Runde 4/5 unverändert.
- Ein sauberer Ring wird bis 7 px weit gesucht, ein unsauberer wie bisher bis 4 px.
- **Warum 7:** gemessen mit je eigenem Bau (unit_r42_karte_lauf [M7], R42_DUMP):
  - Weite 5 und 6: Punkt (170,119). Sauber, Rand 18/56, aber 2 Texel des Türbalkens im Feld.
  - Weite 7 und 8: Punkt (173,122). An allen 397 Tür-Ankünften dieselben Lagen.
  - 7 ist also die kleinste Weite, die den Punkt vom Übergang weg bringt.

**Ergebnis im Abzug (Kartenlauf Runde 6):** L4080_t0 und E4081_t0 (173,122), Rot-Maß 0,36. Ring-Inneres: alle 8 Texel
rot (Runde 5: 3 von 8 nicht rot, davon 2 Wand).

**Wirkung auf alle 397 Tür-Ankünfte STAGE1-6** (R42_DUMP vorher/nachher, `s3r6/d_vor.txt` / `s3r6/d_end.txt`):
- 4080/4081 aus 4030: (171,118) → (173,122). Gewollt.
- 4040/4041 aus 4050: (109,121) → (108,121). Der Ring lag mit seiner rechten Spalte auf der Wand und am Türbalken.
  Jetzt liegt er ganz im Raum. Abzug L4040_t2/E4041_t2: 0,61 → 0,68.
- 4070/4071 aus 4040 auf Blatt 11: (165,158) → (166,163). Das ist keine echte Lage: Der Test setzt dort y = 0. Die
  echte Ankunft ist y -21600 auf Blatt 8 und bleibt unverändert (L4070_t0 (149,164), 0,57). Auf Blatt 11 liegt das
  Ring-Innere jetzt nicht mehr auf den Treppenstrichen.
- Sonst keine.

## 2. Riegel und Gegenproben

`unit_r42_karte_lauf` [M7] prüft jetzt drei Dinge (Ankunft aus 4030 bei (-3900,2100)):
1. Rand mindestens 18 von 56 sichtbarer Raum (0,32 = v0.8.30).
2. Ring-Inneres 9 von 9 Raum, und kein Ring-Texel außerhalb der Kunst von r8/r13. Das ist der Vorschlag der Prüfung:
   "dass im Ringinneren keine Wand und kein blauer Grund liegt".
3. Kein Türbalken-Texel im 9x9-Feld um den Ring (nicht am Übergang zu 4030).

Das alte Maß "keine Kunst einer fremden Kachel im 9x9" entfällt. Grund, gemessen an MAP09.PIX: Die Rahmenlinie der
Kachel r3 (4030) läuft rechts und unten den ganzen Schräggang entlang, auch an seinem unteren Ende. Sie ist nicht der
Übergang. Der Übergang ist der Türbalken. Das sage ich offen, weil ein Riegel hier gelockert wurde. Dafür ist er an
anderer Stelle strenger.

Gegenprobe in eigenen Bäumen aus `git archive`, Test als `test_r42_karte_lauf_neu` (Skript `s3r6/gp_bau.sh`, Logs
`s3r6/gp_*_test.log`):

| Stand | Punkt | Rand | Inneres | Texel über Grund | Türbalken im Feld | [M7] |
|---|---|---|---|---|---|---|
| aaea5f8c2 (v0.8.30) | (176,124) | 28 | 6/9 | 0 | 4 | **ROT** |
| e2501f322 (Runde 4) | (170,117) | 16 | 6/9 | 4 | 4 | **ROT** |
| 02972374c (Runde 5) | (171,118) | 20 | 6/9 | 4 | 0 | **ROT** |
| 1713acd21 (neu) | (173,122) | 20 | 9/9 | 0 | 0 | grün |

Auf aaea5f8c2 sind auch [M1]-[M6] rot wie bisher. Auf e2501f322 [M5]/[M6] wie bisher.

## 3. Tests (Endstand 1713acd21)

- Bau `local_build.sh build`: LOCAL-BUILD-OK.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 6`: **61/61 grün** (37 s).
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 3`: **22/22 grün** (Abschnitt 3a).
- `unit_r42_karte_lauf` einzeln: [M1]-[M7] PASS, [M5] 200 von 200.
- Die volle Suite ist nicht gelaufen (so im Auftrag).

### 3a. Integration

- **22/22 grün** in 2645 s, im ersten Lauf ohne Wiederholung (`s3r6/ctest_integ.log`), parallel zum Kartenlauf und zu
  den Bau-Strängen der anderen Zweige. Darunter integration_map_raum_live, integration_map_uebergang,
  integration_r39_karten und die masken36_teile-Gruppe.

## 4. Kartenlauf (Abzüge aus dem echten Spiel)

- 645 Lagen STAGE1-6, Leon und Elza, alle mit der exe dieser Runde neu aufgenommen (Werkzeuge `s3r6/kampagne.sh`,
  `schuss.sh`, `auswerten2.py`, `vergleich6.py`).
- 6 Gehlagen liefen im ersten Durchgang unter Last ins Zeitlimit (L1010_w1, L1220_w0/w0k, L40A0_w0/w0k, L6000_w1). Mit
  längerer Laufzeit (EXTRA=900) gingen alle 6 durch. Wie in Runde 5.

| | ok | Punkt neben Rot | Raum nicht rot | kein Abzug |
|---|---|---|---|---|
| Runde 5 (645 Lagen) | 619 | 11 | 3 | 12 |
| **Runde 6 (645 Lagen)** | **619** | **11** | 3 | 12 |

- Dieselben 144 Leon-Lagen STAGE2-6: v0.8.30 85 gut, Runde 5 140, **Runde 6 140**. Besser als v0.8.30: 55.
  **Schlechter als v0.8.30: keine.**
- Pixelvergleich gegen Runde 5: 633 Abzüge, **629 gleich, 4 anders** (L4080_t0, E4081_t0, L4040_t2, E4041_t2).
- Die 12 Lagen ohne Abzug sind dieselben wie in Runde 5, mit Grund auf `tafel_99_ohne_abzug.png`.
- Ring-Inneres nicht ganz rot: 140 Lagen in Runde 5, **138 in Runde 6** (4080/4081 aus 4030 fallen weg). Zur
  Einordnung: Die übrigen sind fast alle Ankünfte an einer Tür, bei denen der Punkt auf der Türwand steht. Das macht
  auch STAGE1 mit den Original-Zeilen so (z. B. L1000_t0). Ich habe sie nicht angefasst.

## 5. Bilder für dich

- `analysis/karte_r42/s3r6/tafeln/`: 52 Tafeln, je Blatt und Figur, aus den Abzügen dieser Runde. Jede Lage ist mit
  Bereich (STAGE), Blatt, Raum, Figur, Art der Lage und Weltlage beschriftet. Abweichungen stehen rot.
- `analysis/karte_r42/s3r6/vorher_nachher.png`: 4080 Leon, 4081 Elza und 4040 Leon, je v0.8.30 / Runde 5 / Runde 6,
  Ausschnitt 6-fach. Für 4081 gibt es keinen Abzug aus v0.8.30. Bei 4040 lag der Punkt in v0.8.30 an anderer Stelle,
  der Ausschnitt zeigt dort nur leeres Blatt.
- `analysis/karte_r42/s3r6/fenster_echt.png`: echtes Spielfenster (gdigrab, normale Grafik, 640x480): 4080 Leon,
  4081 Elza, 4040 Leon. Gegen den FB_SHOT desselben Laufs weichen je 52 Bildpunkte ab (der blinkende Ring). Der
  FB_SHOT dieser Läufe ist pixelgleich mit dem Abzug im Kartenlauf.
- Daten: `analysis/karte_r42/s3r6/abzuege.tsv`, `kampagne.txt`, `vergleich.txt`.

## 6. Kollisionen

`git merge-tree --write-tree` von r42/karte (1713acd21) gegen die aktuellen Spitzen meldet **keinen Konflikt**:

| Zweig | Spitze |
|---|---|
| story | 93821acef |
| rest | 3e1a14809 |
| gegenstaende | 5515f2601 |
| finale | c5657d3ed |
| gore | b8b879d54 |
| lageplaene | c35ab51a5 |
| original | 7ec4ba1f0 |
| birkinfilm | b5bb0edd0 |
| namen | 4c53a03e6 |
| hangar_aussen | fe5436f55 |

- Diese Runde ändert nur `karte_r42.c`, `re15_karte_r42.h` und `test_r42_karte_lauf.c`. Kein anderer Zweig ändert diese
  Dateien. story, gegenstaende und namen ändern `re15_inv_screen.c`; das tut diese Runde nicht.
- Die Tabellen von U-11/U-12 (r42/rest) berührt diese Runde nicht.
- Einen Zweig "gefaengnis" gibt es nicht (das Gefängnis 1210/1211 liegt auf r42/original).

## 7. Was offen ist (gelb)

1. **Zug 5090:** 3 Lagen außerhalb aller Zugteile (L5090_t2, L5090_w1, E5091_t2). Stand v0.8.30, gehört zu U-12. Gelb
   nach Vorgabe Punkt 3.
2. **4080 aus 4030, Ort des Punkts:** Leon steht beim Betreten oben im Gang, an der Tür. Der Punkt steht jetzt am
   unteren Ende des Gangs, 8 Bildzeilen tiefer und 6 Spalten weiter rechts als die Rohlage. Näher an der Tür geht es nicht ohne Wand im Ring oder Türbalken daneben,
   weil der Gang nur so breit ist wie der Ring. In v0.8.30 stand der Punkt noch weiter weg, im großen Raum.
3. **4080 aus 4030, Nachbarlinie:** Die Rahmenlinie der Kachel 4030 läuft 2 px rechts neben dem Ring (dazwischen
   1 px blauer Grund). Das ist nicht die Tür. Siehe Abschnitt 2.
4. **Unverändert aus Runde 5:** 20A0 zweimal rot auf Blatt 6 (Grundriss und das alte Kästchen r10), 20A0 an der Tür nach
   2090 (0,30), 20A0-Kollision (Leon läuft durch die Westwand, eigener Strang), 10B0_t0/10B1_t1 (0,29), Grenzfälle mit
   ausgelieferter Zeile (2000/2001 aus 1260, 11B0, 3090_w1, 6030_t1/6031_t1), 4070_w0 (0,25, Treppengitter), neue
   Stücke ohne Türmarken (10B0, 1260, Labyrinth-Kammern, 20A0), Türabstand 2(d) (30A0_t0, 30B0_t0, 30C0_t2, 50B0_t0,
   50D0_t0, 4070_t0), Zusatzbits nur noch 253-255 frei.
