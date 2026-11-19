# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 8

Zweig r42/karte. Code-Commit d0b4096d3, danach ein Doku-Commit mit diesem Bericht und den Bildern. Kein Push.

## Kurz und in Alltagssprache

- **Der rote Punkt der Prüfung betraf nur den schmalen Schräggang in Raum 4080 (Elza: 4081).** Die Prüfung sagte:
  An gut der Hälfte des Gangs liegt in v0.8.30 mehr Rot um den Punkt als jetzt.
- **Ich habe nachgemessen, wo genau v0.8.30 besser war.** Es gibt zwei Arten von Stellen:
  - **Stellen, an denen v0.8.30 sauber und röter war.** Dort stand der alte Punkt ganz im Raum, nur etwas zu weit vorne.
    Das sind 34 Stellen in der Gangmitte. **Die sind jetzt behoben:** Der neue Punkt hat dort mehr Rot als v0.8.30
    (0,36 gegen 0,34).
  - **Stellen, an denen v0.8.30 röter war, aber auf einer Wandlinie stand.** Dort lag eine gemalte Wand mitten im Ring
    des alten Punkts, und der Punkt stand 6 bis 10 Bildpunkte vor der Figur. Diese Stellen bleiben. Mehr Rot geht im
    gemalten Gang nicht, ohne dass der Punkt aus dem Gang in die breite Fläche dahinter rutscht.
- **Was ich geändert habe:** Der Punkt springt jetzt genau in der Mitte des Gangs auf seine zweite Lage. Bisher sprang
  er erst nach drei Vierteln. Das war zugleich der gelbe Punkt der Prüfung ("drei Viertel des Gangs steht der Punkt
  still"). Jetzt ist es halb und halb.
- **Ein neuer automatischer Test** prüft genau diese 34 Stellen (68 mit Elza). Auf dem alten Stand schlägt er an, auf
  dem neuen nicht.
- **Im echten Spiel nachgemessen:** 58 Abzüge im Gang. An allen 10 Stellen, an denen v0.8.30 sauber war, ist der neue
  Punkt jetzt mindestens so rot. Alle anderen Räume sind unverändert (die Änderung gilt nur in diesem Gang).
- **Ein Satz im Bericht der Runde 7 war falsch.** Dort stand: "v0.8.30 hatte im Gang an jeder Lage eine Wandlinie im
  Ring". Das stimmt nicht für die Gangmitte. Die Prüfung hatte recht. Richtig ist es in Abschnitt 3.
- **Was offen bleibt (zur Entscheidung):** An 5 Stellen im Gang und 5 Stellen dahinter ist v0.8.30 weiter röter. Dort
  stand der alte Ring jedes Mal auf einer Wandlinie. Meine Empfehlung: diese Stellen nicht als "schlechter" werten.

Fachwörter:
- "Ring" ist der kleine Positions-Punkt auf der Karte (5 x 5 Bildpunkte). "Rot-Maß" ist der Anteil roter Bildpunkte
  im schmalen Rand um den Ring (56 Bildpunkte). "Sauber" heißt: Im Inneren des Rings liegt keine Wandlinie.
- "Haltestelle" ist eine der zwei Lagen, an denen der Ring ganz in den schmalen gemalten Gang passt.
- "Riegel" ist ein automatischer Test. "Gegenprobe" heißt: Derselbe Test muss auf dem alten Stand fehlschlagen.
- "Abzug" ist ein Bild des Kartenschirms aus dem laufenden Spiel (FB_SHOT).

## 1. Rot der Prüfung: Wo war v0.8.30 wirklich besser?

Zerlegt mit den Abzügen der Prüfung (`pruef_s3_r7/schuss_v030`, 24 Lagen) und meinen 8 neuen v0.8.30-Abzügen in der
Gangmitte (`s3r8/schuss_v030`). Messung: `pruef_s3_r7/ring2.py` (unverändert übernommen).

| Punkt in v0.8.30 | Rot | Ring-Inneres | Wo im Gang (Längslinie x+z) |
|---|---|---|---|
| (176,124) | 0,32 | 5/8 | Tür bis x-z -2500 / -2200 / -1700 (Linie -1300 / -1650 / -2100) |
| (177,124) | 0,32 | 7/8 | die nächsten 1100 Einheiten |
| **(178,124)** | **0,34** | **8/8 (sauber)** | **-1300: x-z -1200..-200; -1650: -900..200; -2100: -400..600** |
| (179,124) | 0,39 | 7/8 | danach |
| (180,124) / (180,125) | 0,43 / 0,50 | 5/8 / 7/8 | |
| (181,124) | 0,43 | 3/8 | Gangende |
| (182,126) .. (184,128) | 0,46 .. 0,54 | 3/8 .. 5/8 | hinter dem Gang |

- **Nur (178,124) ist sauber und röter als 0,32.** Alle anderen röteren v0.8.30-Lagen haben eine Wandlinie im Ring.
- Die Strecken für (178,124) stammen aus dem Sweep der Prüfung (`pruef_s3_r7/sweep/lin_v030_4080_*.txt`). Gerechnet mit
  der Gang-Zeile beginnen sie auf allen drei Linien genau bei Zeile mx+my = 288.
- Gang-Zeile: Tür mx+my = 279 (x-z = -6800), Gangende 297 (x-z = 4075). Die Mitte ist 288.
  **Die sauberen v0.8.30-Lagen liegen also genau in der zweiten Hälfte des Gangs.**

## 2. Änderung: Wechsel der Haltestelle in der Gangmitte

`karte_r42.c`, `re15_karte_r42_gang`, Abschnitt (C). PORT-WAHL (Anzeige, keine Spielkonstante).

- Vorher: Der Punkt nahm die nächste Haltestelle. Haltestelle 1 (170,119) liegt bei mx+my 289, Haltestelle 2 (173,122)
  bei 295. Weil Haltestelle 1 schon 10 vor der Tür liegt, kam der Wechsel erst bei mx+my 293, nach drei Vierteln des Wegs.
- Jetzt: zwei gleiche Hälften der Gang-Zeile. Tür 279 bis 287: Haltestelle 1. Ab 288 bis zum Gangende: Haltestelle 2.
  In Weltlage ist das ungefähr x-z = -1360.
- Belege der Eckwerte (unverändert aus Runde 7): Tür-Slot 0 ROOM4080 Mitte (-4250,2550) → x-z -6800; Gangende = Ende
  der Dreiecke Zelle 6 @ROOM4080.RDT 0x005A4 und Zelle 11 @0x005E0 → x-z 4075; Formel FUN_800473f8 @0x8004741c-0x80047528,
  Gang-Zeile 119/171/1736/1736 (die Zeile des Raums @0x80076AF8 ist der Stub {0,0,1,1}).
- Größter Abstand Zeile → Punkt bleibt 10 (an der Tür, wie bisher). In der zweiten Hälfte jetzt höchstens 7 statt 2.
- Haltestellen, Rand, Hinter-dem-Gang-Logik und alle anderen Räume: unverändert.

## 3. Korrektur des Berichts der Runde 7

Falsch war (BERICHT_s3_r7, Abschnitt 6): "Dort hatte v0.8.30 an jeder Lage eine Wandlinie im Ring (Inneres 5/8 bis 7/8 rot)."

Richtig ist: Auf der Gangmitte zwischen x-z etwa -1200 und +600 (je nach Längslinie) stand der v0.8.30-Punkt auf
(178,124). Dort war sein Ring sauber (Inneres 8/8 rot, kein Ring-Texel außerhalb der Kunst) und röter (0,34). Er stand
nur nicht im gemalten Gang, sondern in der breiten Diagonale davor. Genau diese Stellen behebt diese Runde. An den
übrigen röteren Lagen stimmt die Aussage (Inneres 3/8 bis 7/8).

## 4. Messung im echten Spiel (FB_SHOT)

Neue exe (`s3r8/exe`), 58 Lagen: die 50 Lagen der Prüfung (`pruef_s3_r7/liste.txt`, Leon Mitte und Seiten, Elza) und
8 neue auf den sauberen v0.8.30-Strecken. Tatsächliche Lage aus `debug.log` (die Kollision verschiebt einige Lagen).
Tabelle: `analysis/karte_r42/s3r8/gang_vgl8.txt`.

Mittellinie Leon (x+z = -1650), gegen v0.8.30 an derselben tatsächlichen Lage:

| x-z | Runde 7 | Runde 8 | v0.8.30 | Runde 8 ≥ v0.8.30? |
|---|---|---|---|---|
| -6276 .. -1800 (9 Lagen) | (170,119) 0,32 8/8 | (170,119) 0,32 8/8 | (176/177,124) 0,32 5/8 oder 7/8 | ja |
| -1200 | (170,119) 0,32 | **(173,122) 0,36** 8/8 | (177,124) 0,32 7/8 | ja |
| -600 | (170,119) 0,32 | **(173,122) 0,36** 8/8 | (178,124) 0,34 **8/8** | **ja (vorher NEIN)** |
| 0 | (170,119) 0,32 | **(173,122) 0,36** 8/8 | (178,124) 0,34 **8/8** | **ja (vorher NEIN)** |
| 600 | (170,119) 0,32 | **(173,122) 0,36** 8/8 | (179,124) 0,39 7/8 | nein, v0.8.30 mit Wandlinie |
| 1200 | (170,119) 0,32 | **(173,122) 0,36** 8/8 | (179,124) 0,39 7/8 | nein, v0.8.30 mit Wandlinie |
| 1800 / 2400 / 2500 | (173,122) 0,36 | (173,122) 0,36 8/8 | 0,43 / 0,50 / 0,43, 5/8 / 7/8 / 3/8 | nein, v0.8.30 mit Wandlinie |
| hinter dem Gang (7 Lagen) | unverändert | unverändert | 0,46 .. 0,54, 3/8 .. 5/8 | 2 ja, 5 nein (alle v0.8.30 mit Wandlinie) |

Neue Lagen auf den sauberen v0.8.30-Strecken (alle drei Längslinien), neu gegen v0.8.30 an derselben Lage:
8 von 8 mit **(173,122) 0,36 8/8** gegen (178,124) 0,34 8/8.

Zusammen: **An allen 10 Lagen, an denen v0.8.30 sauber war, ist Runde 8 mindestens so rot.** Runde 7: 0 von 10.

Elza (4081) und die Seitenlinien: gleiches Bild, Punkt immer (170,119) oder (173,122), Ring-Inneres immer 8/8, kein
Rückwärtssprung.

Auf den drei Längslinien der Prüfung (Sweep, Gang x-z -6800..4075, je 109 Lagen, v0.8.30-Werte aus den Abzügen;
`s3r8/linien_vgl.txt`):

| Längslinie | v0.8.30 sauber und röter: Runde 7 → Runde 8 | v0.8.30 röter mit Wandlinie (unverändert) |
|---|---|---|
| x+z = -1300 | 11 → **0** | 41 |
| x+z = -1650 | 12 → **0** | 38 |
| x+z = -2100 | 11 → **0** | 36 |

(Je Linie 10 bis 13 Lagen ohne v0.8.30-Abzug für genau diesen Punkt, z. B. (181,125); nicht gewertet.)

## 5. Riegel und Gegenproben

- **[M9] neu** in `unit_r42_karte_lauf`: alle 68 begehbaren Lagen der drei sauberen v0.8.30-Strecken (alle 100 Einheiten,
  Leon 4080 und Elza 4081). Je Lage: Rand ≥ 19/56 (= 0,34, v0.8.30-Abzug) und das [M7]-Maß (Inneres 9/9, Ring auf der
  Kunst, nicht direkt am Türbalken).
- Gegenproben (`s3r8/gp/`):

| Stand | [M9] |
|---|---|
| bd889c799 (Runde 7), nur karte_r42.c getauscht | ROT: 68 von 68, Punkt (170,119), Rand 18/56 |
| aaea5f8c2 (v0.8.30), eigener alter Baum | ROT: 68 von 68, Punkt (178,124), Rand 23/56, aber Balkenabstand 1 |
| d0b4096d3 (neu) | grün, 0 von 68 |

  Offen gesagt: Auf v0.8.30 schlägt [M9] über den Türbalken-Abstand an, nicht über das Rot. Der Riegel zählt den Rand
  aus der Kunst (r8 + r13). v0.8.30 zeigte r13 aber nicht rot, deshalb liegt sein Abzug-Wert (19) unter dem Kunst-Wert (23).
  Die eigentliche Gegenprobe für diese Runde ist Runde 7.
- [M8] weiter grün: 264 Lagen, alle im [M7]-Maß, 0 Rückwärtssprünge, 0 Sprünge über 3 px.
- Glatt-Riegel: 4080/4081 2/2 Sprünge (Runde 5: 6), alle anderen Räume unverändert.

## 6. Tests (Endstand d0b4096d3)

- Bau `local_build.sh build`: LOCAL-BUILD-OK.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 6`: **66/66 grün**.
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 3`: **22/22 grün** in 2412 s, im ersten Lauf ohne Wiederholung, parallel zu den Abzügen (`s3r8/ctest_integ.log`).
- Volle Suite nicht gelaufen (so im Auftrag).

## 7. Kartenlauf und Bilder

- Die Änderung wirkt nur im Schräggang von 4080/4081 (Zone 1, Zeile mx+my 288..292). Die 12 Kartenlauf-Lagen in
  4080/4081 habe ich neu aufgenommen: **12 von 12 Bildpunkt für Bildpunkt gleich wie in Runde 7** (Ankünfte aus 4030/4090/40A0/40B0, 2 s Gehen; alle liegen außerhalb der geänderten Strecke).
- Alle übrigen 633 Lagen des Kartenlaufs und die 52 Tafeln der Runde 7 (`analysis/karte_r42/s3r7/tafeln/`) gelten
  unverändert.
- Neue Bilder für dich (`analysis/karte_r42/s3r8/`):
  - `gang_4080_leon.png`, `gang_4080_elza.png`: der Weg durch den Schräggang, 10 Lagen von der Tür bis hinter das
    Gangende, je v0.8.30 / Runde 7 / Runde 8. Beschriftet mit Bereich, Raum, Figur, Lage, Punkt und Rot.
  - `fenster_echt.png`: echtes Spielfenster (gdigrab, normale Grafik, 640 x 480). Leon an der Ankunft, in der Gangmitte und am Gangende, Elza in der Gangmitte. Punkt im Fenster (170,119) / (173,122) / (173,122) / (173,122), Rot 0,32 / 0,36 / 0,36 / 0,36, Ring innen jedes Mal ganz rot, gleich dem FB_SHOT desselben Laufs (13 Bildpunkte abweichend: der blinkende Ring; aus 12 Fensterbildern wurde je eines mit sichtbarem Ring genommen).
  - Daten: `gang_vgl8.txt`, `linien_vgl.txt`.

## 8. Kollisionen

`git merge-tree --write-tree` von r42/karte (d0b4096d3) gegen die Spitzen: **kein Konflikt** mit story 9cf52876f,
rest 3e1a14809, gegenstaende 5515f2601, finale 35965debe, gore b8b879d54, lageplaene c35ab51a5, original 115ec88e6,
birkinfilm b5bb0edd0, namen 6f1a719f0, hangar_aussen fe5436f55, kellerplan 2a2d45a1a.

- Diese Runde ändert nur `karte_r42.c` (Funktion `re15_karte_r42_gang`) und `test_r42_karte_lauf.c`. Beide gibt es nur
  auf r42/karte.
- Vorwarnung wie in Runde 7: story, namen, lageplaene und kellerplan ändern `kartenitems_r36.c`, `re15_map_owned.c` und
  `re15_inv_screen.c`. Kein Textkonflikt; die Kartentests nach dem Zusammenführen noch einmal laufen lassen.
- U-11/U-12 (r42/rest) nicht berührt.

## 9. Was offen ist (gelb, zur Entscheidung)

1. **Schräggang:** An 5 Gang-Lagen (x-z 600 .. 2500) und 5 Lagen hinter dem Gang ist v0.8.30 weiter röter (0,39 .. 0,54).
   Dort stand der v0.8.30-Ring jedes Mal auf einer Wandlinie (Inneres 3/8 .. 7/8) und 6 bis 10 Bildpunkte vor der Figur.
   Mehr als 0,36 gibt der gemalte Gang nicht her. Empfehlung: diese Lagen nicht als "schlechter als v0.8.30" werten.
2. Der Punkt läuft im Gang weiter in zwei Stufen (jetzt halb und halb). In der zweiten Hälfte steht er bis zu
   7 Zeilen-Einheiten (etwa 5 Bildpunkte) vor der Figur, an der Tür wie bisher bis zu 10.
3. Unverändert aus Runde 7: hinter dem Gangende Zone 0 mit den Klemmen von Runde 4/5; Zug 5090 (L5090_t2, L5090_w1,
   E5091_t2; U-12); 20A0 zweimal rot auf Blatt 6, 20A0 an der Tür nach 2090, 20A0-Kollision; 10B0_t0/10B1_t1;
   2000/2001 aus 1260; 11B0; 3090_w1; 6030_t1/6031_t1; 4070_w0; neue Stücke ohne Türmarken; Türabstand 2(d);
   Zusatzbits 253-255; der Glatt-Riegel sieht Räume ohne Lagen auf Band 0 nicht.
