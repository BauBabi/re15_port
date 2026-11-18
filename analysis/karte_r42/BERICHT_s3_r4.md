# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 4

Zweig r42/karte. Code-Commit 7dc3f1841, Test-Commit 56b5a2ab8, danach ein Doku-Commit mit diesem Bericht. Kein Push.

## Kurz und in Alltagssprache

- **Jeder Raum hat jetzt ein Stück Karte mit Punkt.** Das war dein Wunsch vom 18.11.: "selbstverständlich sollen
  alle Räume auch eIn stück karte mit navigationspunkt haben."
- **Neu dazugekommen sind vier Orte, die bisher keine Karte hatten:**
  - **10B0** (Weg zwischen Tor 1170 und 1260): ein L-förmiger Grundriss rechts unten auf dem Blatt "Police Station 1F".
  - **1260** (große Halle unter dem Revier, mit Leitern nach 11B0 und 11D0): ein Kasten unten rechts auf "Police Station B1".
  - **20B0** (die geflutete Fassung von 2000): leuchtet jetzt auf dem Blatt "Drains B2" an der Stelle von 2000.
  - **Das Labyrinth 4050**: Es hat acht Kammern, gemalt sind nur zwei. Die anderen sechs stehen jetzt als kleine Kästen
    links auf "Laboratory B1". Der Punkt steht in der richtigen Kammer.
- **Wo das Original nichts gemalt hat, zeichnet der Port den Grundriss aus der Kollision.** "Kollision" sind die
  Wände, an denen die Figur im Spiel anstößt. So macht es STAGE1 schon länger.
- **Die Restfehler der letzten Prüfung:**
  - Im **Ostarm von 4080** läuft der Punkt jetzt mit. Der Arm hat eine eigene Umrechnung bekommen.
  - Im **Gang in 6000 zum Hangar** steht der Punkt nicht mehr auf der Wand. Der Gang ist jetzt gezeichnet.
  - **4070 und 4080 (Ankunft aus 40B0)** sind jetzt besser als in v0.8.30.
- **Eine Stelle bleibt knapp schlechter als v0.8.30:** 4080 bei der Ankunft aus 4030. Der Messwert ist 0,29 statt 0,32.
  Der Punkt steht dort an der Spitze eines schmalen Gangs, direkt an der Tür. Das ist der richtige Ort. v0.8.30 setzte
  ihn ans falsche Ende des Gangs. Begründung unten.
- **Ausgenommen sind nur Räume, in denen die Karte gar nicht aufgeht:** 1240 (eine reine Szene) und 6040 (der Abspann).
- **Neuer Kartenlauf:** 643 Lagen, 631 Bilder aus dem echten Spiel. Bei 615 steht der Punkt auf dem roten Raum.
  In der letzten Runde waren es 572 von 607.

Fachwörter:
- "Blatt" ist eine Kartenseite.
- "Kachel" ist ein gemalter Raum auf dem Blatt.
- "Zeile" ist die Umrechnung von der Spielposition auf den Kartenpunkt.
- "Zone" ist ein Teil eines Raums mit eigener Zeile.
- "Riegel" ist ein automatischer Test.
- "Gegenprobe" heißt: Derselbe Test muss auf dem alten Stand fehlschlagen.
- "Abzug" ist ein Bild des Kartenschirms direkt aus dem laufenden Spiel (FB_SHOT).
- "Rot-Maß" ist der Anteil roter Bildpunkte im 2-px-Rand um den Punkt. Die Grenze liegt bei 0,3, das ist das Maß der
  Prüfung.

## 1. Zensus aller Räume (Punkt 1 der Vorgabe)

**Quelle:** Jede `ROOM####.RDT` in `shared_assets/PSX/STAGE1..6`. Es gibt 240 Dateien:
- **34 davon sind leere 4-Byte-Platzhalter**, alle mit demselben Inhalt (md5 f1d3ff84): 1270/1271, 20C0-20F1, 30F0/30F1,
  4060/4061, 40C0-40F1, 5150-5171 und 6050-6071.
- **206 Räume haben Inhalt** (Leon und Elza). Werkzeug: `s3r4b/zensus.py`, Ergebnis in `s3r4b/zensus1.txt`.

**Geprüft wird das vom neuen Riegel `unit_r42_karte_lauf` [M5].** Er nimmt jeden der 206 Räume und jede Ankunft. Eine
Ankunft ist der Spawn jeder Tür, die in den Raum führt. Gibt es keine solche Tür, nimmt er die eigenen Türen; bei reinen
Skript-Warps die Mitte jeder Zone. Für jede Ankunft muss gelten:
- Es gibt ein Blatt mit einem Punkt.
- Im 3x3 um die Punktmitte liegt der Raum in der Aktuell-Farbe.
- Der Punkt wandert um mindestens 2 px, wenn die Figur um ±2000 oder ±4000 in x oder z versetzt wird.

| Raum | vorher (b94479047) | jetzt | Art |
|---|---|---|---|
| 10B0/10B1 | Zone ohne Zeichnung und ohne Zeile, Punkt auf (0,0), unsichtbar | Grundriss (4 Zellen) auf Blatt 2, rot, Punkt läuft | NUTZERWUNSCH, Port-Ergänzung |
| 1260/1261 | keine Zone, kein Punkt | Grundriss (Hülle) auf Blatt 0, rot, Punkt läuft | NUTZERWUNSCH, Port-Ergänzung |
| 20B0/20B1 | keine Zone, kein Punkt | Kachel r0 von 2000 mit der ausgelieferten Zeile von 2000, rot | NUTZERWUNSCH, Variante |
| 4050/4051 (Labyrinth) | 6 von 8 Kammern ohne Zone, Punkt klebte am Rand von r5/r6 | 6 Kammern als ein Ort (Schema), Punkt in der richtigen Kammer | NUTZERWUNSCH, Port-Ergänzung |
| 1240/1241 | ausgenommen | ausgenommen | reine Szene (Beleg unten) |
| 6040/6041 | ausgenommen | ausgenommen | Abspann (Beleg unten) |
| 5090/5091 | 3 Lagen außerhalb der Zugteile ohne Rot | unverändert | **gelb**, Vorgabe Punkt 3 (U-12) |
| 20A0/20A1 | gemalte Kachel 11x9 px | unverändert | **gelb**, siehe "Offen" |
| alle übrigen 186 | Stück Karte, rot, Punkt läuft | unverändert | |

Ergebnis [M5]: **198 von 198 geprüften Räumen** zeigen ein Stück Karte mit rotem Raum und mitlaufendem Punkt. Dazu
kommen 8 Räume, die mit Grund ausgenommen sind.

### Belege für die neuen Stücke (alle PORT-ERGÄNZUNG mit dem Wortlaut des NUTZERWUNSCHES im Code)

Gemeinsam für alle:
- Die Umrechnung ist die Original-Formel FUN_800473f8 @0x8004741c-0x80047528 mit einer Port-Zeile. Norden liegt oben,
  nichts ist gespiegelt.
- Die Lage auf dem Blatt ist eine PORT-WAHL im Kartenfeld des Originals (x39..293, y44..206; das prüft der Riegel
  `unit_map_synth`).
- Kein Stück überdeckt eine gemalte Kachel.

**10B0:**
- Das Blatt ist byte-true: FUN_8004b568 schickt Raumindex 11 über die Kette 0..11 auf Blatt 2 (@0x8004b680-684
  `ori v0,zero,0x2`).
- Die Zeile im Original ist der Stub @0x80076908 = `00 00 00 00 01 00 01 00`.
- Die Zellen kommen aus der begehbaren Fläche je Band (Flut über die SCA-Zellen):
  - Band 0 ist ein L mit der Tür nach 1260 (@ROOM10B0.RDT 0x016F2).
  - Band 1 ist eine Rampe.
  - Band 5 ist ein Gang mit der Tür nach 1170 (0x016D2, floor 5).
- Zeile (197,230,1900,1900), Kasten x214..291 / y160..196.

**1260:**
- Das Original gibt dem Raum **keine Seite**: `sltiu v0,v1,0x26` @0x8004b574, Index 0x26 fällt durch. Die Zeile mit
  Index 38 @0x800769E0 gehört zu ROOM2000 (STAGE2-Basis 38).
- Er liegt auf Blatt 0 (B1), weil dort die Leiter-Nachbarn 11B0 und 11D0 liegen.
- Die Kollision ist eine offene Halle: Die Flut füllt die ganze Hülle. Deshalb ist das Stück ein Kasten.
- Zeile (195,211,745,745), Kasten x200..239 / y174..205.

**20B0:**
- Der Raum ist eine Variante von 2000. Die Türen slot 0/1/2/4 liegen auf denselben Weltkoordinaten wie bei 2000.
- Im Original hat er Seite 0xd (@0x8004b9b4, CD-Datei 0) und den Stub @0x80076A38.
- Der Port nimmt die Kachel r0 und die ausgelieferte Zeile von 2000 @0x800769E0 (`50 00 c4 00 24 09 04 09`).
- Die Lagen im Lauf stehen auf Bit 3:235 = 1. Das ist der Stand nach der Flut: Set @ROOM20B0.RDT 0x01FBC am Ende von
  sub05, Prüfung @0x01AAA.

**4050:**
- Das Labyrinth hat acht Kammern (Werkzeug `s3r4b/kammern.py`). Man erreicht sie über die Selbst-Türen des Raums, zum
  Beispiel slot 3: Trigger (-1000,-9450) → Spawn (-15850,11650).
- Die sechs ungemalten Kammern sind **ein Ort** mit 6 Zellen und einem Besucht-Bit (Zusatzbit 252). Für mehr Bits war im
  Speicherformat kein Platz: 256 Bits, 252 belegt.
- Zeile (18,206,1540,1540).

**Ausnahme 1240:**
- Der Raum hat keine Tür (nDoor=0).
- sub00 hat `Evt_exec` ohne Bedingung @ROOM1240.RDT 0x00544. sub02 setzt Set 2:7=1 @0x0055A und endet mit `Aot_on 0`
  @0x00618, das ist der Wechsel nach 1170.
- **Gemessen:** Ein Lauf von 2600 Bildern ab Raumstart (`s3r4b/szene/L1240_szene`) ergab keinen Kartenschirm. Das Log
  zeigt danach ROOM1170.

**Ausnahme 6040:**
- Abspann: sub00 `Evt_exec` ohne Bedingung @ROOM6040.RDT 0x00522, sub02 Set 2:7=1 @0x00530, keine Tür.

## 2. Restfehler aus pruef_s3_r3

**(a) 4080 Ostarm: behoben.**
- Der Arm hat jetzt eine **eigene Zone 2 mit eigener Zeile**. Sie kommt aus den vier Armwänden:
  - x 21396 @ROOM4080.RDT 0x00598 → Linie x210
  - x 25457 @0x00568 → x218
  - z -3430 @0x0055C → y124
  - z -8460 @0x00598 → y134
- Daraus folgt die Zeile (105,181,2066,2085). Alle vier Wände liegen auf 0 px.
- Der Kasten ist um ZONE_SLACK nach innen gesetzt. Dadurch schaltet die Zonenwahl genau an den Armwänden um.
- Zone 0/1 rechnen jetzt mit der Zeile aus den Raumwänden (118,176,1833,1905). Das kommt aus dem Patch der angehaltenen
  Runde; ich habe ihn geprüft und übernommen.
- **Abzüge:** L4080_w1 (214→216, 2989 Einheiten) und L4080_w1k laufen mit. Rot-Maß 0,43.

**(b) 6000 Gang zum Hangar (50B0): behoben.**
- Der Gang ist jetzt ein Schema (synth 13) mit der Zeile des Bahnsteigs (Patch der angehaltenen Runde, geprüft).
- Abzüge:
  - L6000_t0: Punkt (215,76), Rot-Maß 0,68 (vorher 0,29 auf der Wandlinie).
  - L6000_w0: läuft mit.
  - Elza E6001_t0: 0,68.

**(c) 4070_t0, 4080_t0, 4080_t3:**

| Lage | v0.8.30 | Runde 3 | jetzt |
|---|---|---|---|
| L4070_t0 | 0,45 | 0,20 | **0,57** |
| L4080_t3 | 0,55 | 0,29 | **0,57** |
| L4080_t0 | 0,32 | 0,29 | **0,29** (gelb, s.u.) |

- **Wie:** Die Ring-Klemme aus dem Patch (Ring nicht auf einer fremden Kachel) greift jetzt auch, wenn im 2-px-Rand um
  den Ring weniger als 30 % Raum liegen. Dann sucht sie die nächste Lage, um die mindestens die Hälfte Raum liegt
  (Suchweite 3 px).
- Das ist eine PORT-WAHL für die Anzeige, keine Spielkonstante. Die Messwerte je Suchweite stehen in `karte_r42.c`.
- Eine Klemme, die jeden Punkt auf die beste Lage zieht, habe ich gemessen und verworfen: In schmalen Kacheln lief der
  Punkt dann nicht mehr mit (Riegel st456 [C]: 4040 Zone 1 8/19, 4070 21/49).
- **4080_t0 (aus 4030):**
  - Die Ankunft (-3900,2100) liegt 800 Einheiten vom Ende des schmalen Gangs zu 4030.
  - Mit der Zeile aus den Wänden fällt der Gang auf die gemalte Diagonale r13 von (167,114) bis (176,123).
  - Die ausgelieferte 4030-Zeile @0x80076AD0 setzt die Gegenseite der Tür auf (164,113). Die Spitze ist also die Tür.
  - Bis 4 px Suchweite ist (170,117) die Lage mit dem meisten Raum in der Kunst. Messwerte: Suchweite 2 → 0,27,
    3 → 0,29, 4 → 0,29.
  - v0.8.30 stand bei (176,124), das ist das andere Ende des Gangs.
  - Ich melde die Stelle offen als gelb. Nach der Regel der Prüfung ist sie formal schlechter (0,29 statt 0,32).

**(d) Abstand zur gemalten Tür:**

| Lage | v0.8.30 | jetzt (px) |
|---|---|---|
| 30A0_t0 | 3 | 7 |
| 30B0_t0 | 5 | 6 |
| 30C0_t2 | 5 | 7 |
| 50B0_t0 | 4 | 9 |
| 50D0_t0 | 2 | 6 |

- Neu dazu kommt 4070_t0 mit 5 → 7 px, weil die Klemme den Punkt vom Treppengitter wegschiebt.
- Alle fünf stehen tief im Raum: Rot-Maß 0,84 / 0,84 / 0,41 / 1,0 / 1,0.
- Näher an die Tür ginge nur mit einer anderen Zeile für den ganzen Raum. Die Zeilen sind aber aus Wänden und gemalten
  Linien belegt (Stufe 1/2). Eine Verschiebung zöge den Rest des Raums von den Wänden weg.
- Deshalb **gelb begründet**, nicht geändert.
- Gesamt bleibt es bei Median 5 px und höchstens 14 px. ≤ 4 px sind es 49 statt 57 (Folge der Klemme).

## 3. Riegel und Gegenproben

`unit_r42_karte_lauf` (neu, `tests/unit/test_r42_karte_lauf.c`) prüft:
- **[M1]** Ostarm 4080 läuft mit.
- **[M2]/[M3]** Gang 6000: Schema aktuell, Punkt läuft mit, auf dem Bahnsteig ist der Gang mit rot.
- **[M4]** 122 Ankünfte: kein Ring auf fremder Kachel.
- **[M5]** Zensus aller Räume.

Gegenprobe im eigenen git-archive-Baum (`s3r4b/gegenprobe_*.log`):
- **aaea5f8c2: ROT** in [M1]-[M5].
  - [M5]: 34 Räume fehlen, darunter 10B0, 1260, 20B0, 4050 und viele STAGE2-6-Räume, deren Punkt stand.
- **b94479047: ROT** in [M1]-[M5].
  - [M5]: 10B0 (Punkt (0,0)), 1260 und 20B0 (kein Blatt), 4050 (Punkt steht).
- Neuer Stand: grün.

Weitere Änderungen an den Tests:
- st456 [C]: Ausnahme für 4080 Zone 2 mit Grund. Der Kasten ist bewusst nach innen gesetzt und ergibt 15 von 49
  Rasterlagen. Das Mitlaufen prüft [M1] an echten Lagen.
- `unit_r42_karte_sichtbar`: TIMEOUT 120 → 300 s. Allein läuft der Test 36-64 s, im Verbund unter Last war er über
  120 s.

## 4. Tests (Endstand 7dc3f1841)

- Bau: `local_build.sh build` LOCAL-BUILD-OK.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 4`: **61/61 grün**.
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 3`: **22/22 grün**.
  - Im ersten Lauf waren es 21/22. Rot war `integration_r39_karten`, und das war **echt, nicht flatterhaft**: Der
    Sweep-Vergleich von Blatt 2 gegen deine Vorlage map1F_correct.png fand 742 Abweichungen. Das ist genau der neue
    Grundriss von 10B0, den die ältere Vorlage nicht kennt.
  - Behoben in 56b5a2ab8: Im Kasten von 10B0 gilt jetzt nicht die Vorlage. Verlangt wird stattdessen, dass der
    Grundriss gezeichnet ist (gemessen 742 Körper-/Wand-Pixel, keine Tür oder Treppe). Das ist dieselbe Bauart wie bei
    der 1210-Kammer.
  - Auf dem übrigen Blatt 2 gibt es 0 Abweichungen. Danach lief `integration_r39_karten` einzeln grün, zusammen mit den
    sechs `unit_r39_karten_*`-Tests (7/7).
- Unit-Teilmenge nach 56b5a2ab8 nochmals: 61/61.
- Die volle Suite ist nicht gelaufen (so im Auftrag).

## 5. Kartenlauf (Abzüge aus dem echten Spiel)

**Aufbau:**
- STAGE2-6, 10B0 und 1260 neu mit der exe des Endstands (421 Lagen).
- Die übrigen STAGE1-Lagen (222) stammen aus Runde 3. Für Blatt 0-5 ist der Code dort unverändert: Die neue Klemme
  wirkt nur auf Blatt 6-12, neue Zonen gibt es nur für 10B0 und 1260.
- Neu im Lauf: 20B0/20B1, die Labyrinth-Kammern k1-k6, der 4080-Arm 1 s und weitere Gehläufe.
- Werkzeuge: `s3r4b/kampagne_gesamt.txt`, `auswerten2.py`, `vergleich.py`, `tuerabstand.py`.

**Ergebnis:**

| | Lagen | ok | Punkt neben Rot | Raum nicht rot | kein Abzug |
|---|---|---|---|---|---|
| STAGE1 | 240 | 232 | 5 (11B0 x3, 10B0/10B1 0,29) | 0 | 3 (1240/1241) |
| STAGE2-6 | 403 | 383 | 8 | 3 (Zug, U-12) | 9 |

**Vorher/nachher an denselben 144 Leon-Lagen STAGE2-6:**

| Stand | gut | neben | nicht rot |
|---|---|---|---|
| v0.8.30 | 85 | 47 | 5 |
| Runde 3 | 136 | 6 | 1 |
| jetzt | **139** | 3 | 1 |

- Besser als v0.8.30: 55 Lagen. Schlechter: 1 (L4080_t0, 0,32 → 0,29).
- **Mitlaufen:** Von 145 Gehläufen mit mindestens 1000 Einheiten Weg bewegt sich der Punkt in 143 um mindestens 2 px.
  Die Ausnahmen:
  - L20A0_w0 mit 1 px (Kachel 11x9).
  - L4040_w1 mit 0 px: Der Lauf geht quer über den gemalten Gang, der innen nur 7 px breit ist. In Runde 3 war es 1 px.
- **Kein Abzug, mit Grund auf `tafel_99_ohne_abzug.png`:**
  - 1240/1241 und 6040/6041: Szene bzw. Abspann.
  - 50B0_w0/w1 und 6030_w0/w1: Der Lauf löst eine Szene aus.
  - L40A0_w0 und L6000_w1 klappten erst mit längerer Laufzeit (EXTRA 900).

## 6. Bilder

- **Für dich:** `analysis/karte_r42/s3r4/tafeln/`, 52 Tafeln. Jede Lage ist mit Bereich (STAGE), Blatt, Raum, Figur,
  Art der Lage und Weltlage beschriftet. Abweichungen stehen rot.
- **Echtes Fenster** (gdigrab, normale Grafik, 640x480): `analysis/karte_r42/s3r4/fenster_echt.png`.
  - Zu sehen sind 10B0, 1260, 20B0, Labyrinth-Kammer, 4080-Arm, 6000-Gang, 4070 und Elza 1261.
  - Gegen den FB_SHOT desselben Laufs sind 99,98 % der Bildpunkte gleich. Der Rest (52 px) ist der blinkende Punkt.
  - Die Aufnahme ist jetzt direkt 640x480. Das Zuschneiden ist eingebaut, falls die Anzeige skaliert.

## 7. Kollisionen

`git merge-tree --write-tree` von r42/karte (7dc3f1841, nochmals mit 56b5a2ab8) gegen die aktuellen Spitzen meldet **keinen Konflikt**:

| Zweig | Spitze |
|---|---|
| rest | 3e1a14809 |
| gegenstaende | 5515f2601 |
| finale | 1789c5e5e |
| gore | b8b879d54 |
| story | 8db089f5d |
| lageplaene | c35ab51a5 |
| original | 23815a1f7 |
| birkinfilm | b5bb0edd0 |
| namen | 237df7594 |
| hangar_aussen | fe5436f55 |

**Vorwarnungen:**
- gegenstaende, story und namen ändern ebenfalls `re15_inv_screen.c`, aber an anderen Stellen.
- Die Zusatzbits 251/252 sind belegt. Danach sind im Besucht-Feld nur noch 3 Bits frei (253-255).
- U-12 (Zugkacheln) ist nicht gebaut. Der Zug bleibt gelb.

## 8. Was offen ist

1. **4080_t0 / 4081_t0:** Rot-Maß 0,29. Der Punkt steht an der Tür zu 4030. Begründung in 2(c). **Gelb.**
2. **20A0/20A1:** Die gemalte Kachel r10 ist 11x9 px. Über den ganzen Raum wandert der Punkt nur etwa 5 px. Auf
   Blatt 6 ist neben der Kachel kein Platz für einen größeren Grundriss: Rechts ist der Rand, darüber der Pfeil, unten
   bleiben 20 px. Ausnahme in [M5] mit Grund. **Gelb.**
3. **Zug 5090:** 3 Lagen außerhalb aller Zugteile (U-12). **Gelb**, nach Vorgabe Punkt 3.
4. **Grenzfälle mit Rot-Maß 0,2-0,29:**
   - Mit **ausgelieferter** Zeile (byte-true, unverändert): 2000/2001 aus 1260, 3090_w1, 6030_t1/6031_t1, 11B0
     (STAGE1).
   - Neue Stücke: 10B0_t0/10B1_t1 (0,29). Der Westarm des Grundrisses ist schmal.
   - 4070_w0 (0,25, Treppengitter, wie in Runde 3).
5. **Neue Stücke ohne Türmarken.** 10B0, 1260 und die Labyrinth-Kammern haben auf ihrem Blatt keinen gezeichneten
   Nachbarn, mit dem eine Tür sie verbindet.
