# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 5

Zweig r42/karte. Code-Commit 02972374c, danach ein Doku-Commit mit diesem Bericht und den Bildern. Kein Push.

## Kurz und in Alltagssprache

- **Beide roten Punkte der letzten Prüfung sind behoben.**
- **Raum 20A0 (Kanalisation) hat jetzt ein richtiges Stück Karte.** Bisher war er ein winziges Kästchen
  (11 x 9 Bildpunkte) am rechten Rand. Der Punkt bewegte sich dort kaum. Jetzt ist der Raum als Grundriss gezeichnet,
  rechts oben auf dem Blatt "Drains B2", über dem Nachbarraum 2090. Man sieht seine Form: unten ein Gang, links und
  rechts je ein Arm, in der Mitte die Plattform mit der Leiter. Der Punkt läuft mit, wenn Leon oder Elza gehen.
  Das kleine gemalte Kästchen bleibt und leuchtet mit, weil es derselbe Raum ist.
- **Raum 4080 (Labor), Ankunft aus 4030:** Der Punkt steht jetzt ein Stück im schmalen Gang. Er klebt nicht mehr am
  Übergang zum runden Raum 4030. Der Messwert ist 0,36. In v0.8.30 war er 0,32, in Runde 4 0,29.
- **Sonst hat sich nichts verschoben.** Ich habe alle 397 Ankünfte durch eine Tür in STAGE1 bis 6 vorher und nachher
  verglichen. Nur 4080/4081 aus 4030 hat sich bewegt. Von 631 Kartenbildern aus dem echten Spiel sind 624 Bildpunkt
  für Bildpunkt gleich wie in Runde 4. Die 7 anderen sind genau 20A0 und 4080 aus 4030.
- **Jeder Raum mit Karte hat jetzt ein Stück Karte mit mitlaufendem Punkt: 200 von 200.** Ausgenommen sind nur noch
  die Szenen 1240 und 6040, in denen die Karte gar nicht aufgeht, und der Zug 5090. Der Zug ist nach der Vorgabe gelb.
- **Nebenbei gefunden und behoben:** Mein erster Bau dieser Runde machte die Karte langsam. Ein Test brauchte 304 statt
  14 Sekunden. Der Grund war eine teure Abfrage, die für jeden Bildpunkt neu lief. Jetzt läuft sie einmal je Karte.
  Der Test braucht wieder 16 Sekunden.

Fachwörter:
- "Blatt" ist eine Kartenseite.
- "Kachel" ist ein gemalter Raum auf dem Blatt.
- "Zeile" ist die Umrechnung von der Spielposition auf den Kartenpunkt.
- "Grundriss" ist ein vom Port gezeichneter Raum, wo das Original nichts gemalt hat.
- "Riegel" ist ein automatischer Test.
- "Gegenprobe" heißt: Derselbe Test muss auf dem alten Stand fehlschlagen.
- "Abzug" ist ein Bild des Kartenschirms direkt aus dem laufenden Spiel (FB_SHOT).
- "Rot-Maß" ist der Anteil roter Bildpunkte im 2-px-Rand um den Punkt. Ab 0,3 steht der Punkt "auf dem Raum".

## 1. Rot 2 der Prüfung: 20A0/20A1

**Was das Original hat:**
- Seite 0xD (`ori v1,zero,0xd` @0x8004b9b4, Bytes `0d 00 03 34`).
- Die CD-Datei dieser Seite ist 0 (@0x80074c66 = `00 00`), also keine Kunst.
- Die Zeile @0x80076A30 (Index 38+10) ist der Stub `00 00 00 00 01 00 01 00`.
- Die Kachel r10 auf Blatt 6 war schon in Runde 1 eine PORT-WAHL.

**Was der Port jetzt zeichnet (PORT-ERGÄNZUNG "Grundriss aus der Kollision", NUTZERWUNSCH 18.11.2026 wörtlich:
"selbstverständlich sollen alle Räume auch eIn stück karte mit navigationspunkt haben."):**
- Schema synth 17 auf Blatt 6, Kasten x235..268 / y51..89.
- **Platz:** gemessen am Abzug L20A0_t0 ist Blatt 6 dort von x228..273 / y40..103 frei. Der Kasten berührt keine
  Rechteck-Box: r5 endet bei x233, r7 bei x229, r9 beginnt bei y130, der Kompass bei x272. Der Riegel
  `unit_map_synth` prüft das.
- **Zellen = die Kamera-Bereiche des Raums**, an den Außenwänden der Kollision geschnitten:

| Zelle | Welt | Beleg (ROOM20A0.RDT) |
|---|---|---|
| Süd (Gang) | x -3600..27000, z -26900..-20500 | RVD ct 0 @0x002A0; Westwand @0x009B8, Südwand @0x0097C, Ostwand @0x00A78 |
| Ost (Arm) | x 20550..27000, z -20500..50 | RVD ct 0 @0x003A4; Trennwand @0x00994, Nordwand @0x00958 |
| Mitte (Plattform) | x 8500..18550, z -20500..-8500 | RVD ct 0 @0x00534 / @0x0055C |
| West (Gang nach 2090) | x 1500..8500, z -20500..7000 | RVD ct 0 @0x00598; die Nordkappe @0x00A6C ist Band 2, die Figur steht an der Tür bei z 5468 |

- **Türen:** nach 2090 (4950,5000) am Nordende des Westarms, nach 11A0 (25800,50) am Nordende des Ostarms,
  die Leiter (Selbst-Tür, 16500,-21050) am Rand der Plattform.
- **Zeile** für FUN_800473f8 (@0x8004741c-0x80047528): R180, also flip_x und flip_z. Das ist eine Drehung, kein Spiegel
  (Regel aus Runde 2). Dadurch zeigt die Tür nach 2090 nach unten zu r9. Ein Maßstab sx = sy = 1121 (37 px auf die
  34600 Einheiten der Hülle), ox 299, oy 47. PORT-WAHL, keine Spielkonstante.
- **Die Zellen liegen um (-1,-1) gegen die Projektion.** Die sichtbare Mitte des Punkts ist (mx-1,my-1)
  (TEX.TIM @0x14910). Ohne die Verschiebung stand der Punkt nach dem Gang zur Plattform-Ostkante in der Lücke
  zwischen zwei Zellen (gemessen, Riegel [M6]).
- **Die Kachel r10 bleibt** als zweite Zeile desselben Orts (gleiche Zeile, gleicher zid 114). Sie leuchtet mit
  (Regel "ein Ort auf zwei Kacheln", re15_map_zones.c rect_state_impl). Sie bekommt nie den Punkt: Bei gleicher Box
  nimmt die Zonenwahl die erste Zeile.
- **Tür-Zug** (re15_inv_screen.c tuer_anziehen, PORT-WAHL): Eine Marke, die nicht am eigenen Grundriss liegt (±2 px),
  zieht nicht mehr. Sonst holte die alte Türmarke (277,150) an r10 den Punkt im Westgang 60 px aus dem Raum.
  Gemessen an allen 397 Ankünften STAGE1-6: keine andere Lage ändert sich.

**Abzüge aus dem echten Spiel:**

| Lage | Runde 4 | jetzt |
|---|---|---|
| L20A0_t0 Plattform (16500,-20000) | (285,144) 0,43 | (247,60) **0,77** |
| L20A0_w0 nach 2 s (16500,-17568) | (284,144), 1 px gewandert | (247,62) 0,70, **2 px** gewandert |
| L20A0_t1 Westgang (7168,-16250) | (284,146) 0,70 | (257,64) **1,0** |
| L20A0_w1 nach 2 s (18082,-16250) | (284,144), 2 px gewandert | (245,64) 0,79, **12 px** gewandert |
| L20A0_t2 an der Tür nach 2090 (4950,5468), neu | - | (259,87) 0,30 |
| E20A1_t0 / E20A1_w0 (Elza, neu gelaufen) | (285,144) 0,43 | (247,60) 0,77 / (247,62) 0,70 |

## 2. Rot 1 der Prüfung: 4080/4081 aus 4030

- **Ursache:** Die Ring-Klemme aus Runde 4 zählte den Raum in der Kunst. Auf dem Schirm liegt aber der Türbalken
  4080↔4030 (Marke (165,113) kind 3, x165 y111..115) über diesem Raum. Deshalb hielt die Klemme (170,117) für die
  beste Lage. Im Abzug waren es nur 0,29.
- **Änderung (karte_r42.c, PORT-WAHL für die Anzeige):** Das Ziel wird am SICHTBAREN Raum gemessen. Texel unter einer
  Tür- oder Treppenmarke zählen nicht. Die Suchweite steigt von 3 auf 4 px. Der Auslöser bleibt gleich.
- **Ergebnis:** (171,118), Rot-Maß im Abzug **0,36** (v0.8.30: 0,32). Im 9x9-Feld um den Ring liegt keine Kunst einer
  fremden Kachel und kein Türbalken. Der Punkt steht also nicht mehr am Übergang zu 4030.
- **Verworfen (gemessen):** ein zusätzlicher Vorrang "9x9-Feld frei von fremder Kunst". Er schob auch 4080 aus 4090
  und 4040 aus 4050 um 2 px und brach das Mitlaufen im Ostarm (Riegel [M1]).

## 3. Riegel und Gegenproben

`unit_r42_karte_lauf` hat zwei neue Prüfungen:
- **[M6] 20A0:** An 5 Lagen liegt der Punkt in einer Zelle des Grundrisses, rot unterlegt, und r10 leuchtet mit. Das
  gilt auch am Tür-Trigger nach 2090. Auf beiden 2-s-Wegen wandert der Punkt mindestens 2 px.
- **[M7] 4080 aus 4030:** Im Rand liegen mindestens 18 von 56 Texeln sichtbarer Raum (0,32 = v0.8.30), und im
  9x9-Feld liegt keine fremde Kunst.
- **[M5]:** 20A0/20A1 sind nicht mehr ausgenommen. Ergebnis: **200 von 200** Räumen.

Gegenprobe in eigenen Bäumen aus `git archive` (Test als `test_r42_karte_lauf_neu`, Logs `s3r5/gp_*_test.log`):
- **aaea5f8c2 (v0.8.30): ROT** in [M1]-[M7]. [M5] 164 von 200, [M6] kein Blatt, [M7] (176,124) mit 7 Texeln fremder
  Kunst (r3) im Feld.
- **e2501f322 (Runde 4): ROT** in [M5] (20A0/20A1 "Punkt steht", 1 px), [M6] (Punkt auf r10, nicht im Grundriss,
  1 px Weg) und [M7] ((170,117), 16 von 56).
- **Neuer Stand: grün** in [M1]-[M7].

Weitere Teständerung:
- `unit_map_speichern_laden` zählte jede Zeile als eigenen Ort. Die zweite Zeile von 20A0 (r10) ist aber nie treffbar.
  Jetzt gilt: Eine Zeile mit derselben Weltbox und demselben zid wie eine frühere Zeile des Raums ist kein eigener Ort.

Kleine Korrektur aus der Prüfung (gelb 3):
- Der Beleg der Ausnahme 1240 im Test nennt jetzt Set 2:7=1 @ROOM1240.RDT 0x00562 (0x0055A ist Set 3:139=1).

## 4. Tests (Endstand 02972374c)

- Bau: `local_build.sh build`: LOCAL-BUILD-OK.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 6`: **61/61 grün** (46 s).
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 4`: **22/22 grün** (1968 s, im ersten Lauf nach der Beschleunigung, ohne Wiederholung). Darunter integration_map_raum_live (16 s), integration_map_uebergang, integration_r39_karten und die masken36_teile-Gruppe.
- Langsamkeit, gefunden und behoben: Im ersten Integrationslauf lief `integration_map_raum_live` in den Timeout
  (180 s). Allein gemessen brauchte er 304 s, in Runde 4 waren es 14 s. Ursache: Meine neue Klemme rief für jedes
  Texel `re15_map_mark_get` auf. Jetzt werden die Marken einmal je Aufruf gesammelt. Danach: 16 s,
  `unit_r42_karte_lauf` 16 s statt 112 s. Die Kartenbilder sind vor und nach dieser Änderung pixelgleich (6 Lagen
  nachgestellt: L4080_t0, L20A0_t0, L20A0_t2, E4081_t0, L4070_t0, L4080_t3).
- Die volle Suite ist nicht gelaufen (so im Auftrag).

## 5. Kartenlauf (Abzüge aus dem echten Spiel)

- 645 Lagen STAGE1-6, Leon und Elza, alle mit der exe dieser Runde neu aufgenommen. Die STAGE2-Lagen (74) sind mit der
  End-exe aufgenommen.
- Werkzeuge: `s3r5/kampagne.txt`, `auswerten2.py` (Maß der Prüfung), `vergleich5.py`.

| | ok | Punkt neben Rot | Raum nicht rot | kein Abzug |
|---|---|---|---|---|
| Runde 4 (643 Lagen) | 615 | 13 | 3 | 12 |
| **Runde 5 (645 Lagen)** | **619** | **11** | 3 | 12 |

- Vorher/nachher an denselben 144 Leon-Lagen STAGE2-6: v0.8.30 85 gut, Runde 4 139, **jetzt 140**.
  Besser als v0.8.30: 55 Lagen. **Schlechter als v0.8.30: keine.**
- Pixelvergleich gegen Runde 4: 631 Abzüge verglichen, 624 gleich, 7 anders (alle 20A0/20A1 und 4080/4081 aus 4030).
- Die 12 Lagen ohne Abzug sind dieselben wie in Runde 4, mit Grund auf `tafel_99_ohne_abzug.png`: Szenen 1240/6040,
  50B0_w0/w1 und 6030_w0/w1 (der Lauf löst eine Szene aus).

## 6. Bilder für dich

- `analysis/karte_r42/s3r5/tafeln/`: 52 Tafeln, je Blatt und Figur. Jede Lage ist mit Bereich (STAGE), Blatt, Raum,
  Figur, Art der Lage und Weltlage beschriftet. Abweichungen stehen rot.
- `analysis/karte_r42/s3r5/vorher_nachher.png`: 20A0 (Plattform, Westgang) und 4080 aus 4030, links Runde 4, rechts jetzt.
- `analysis/karte_r42/s3r5/fenster_echt.png`: echtes Spielfenster (gdigrab, normale Grafik, 640x480): 20A0 Leon zweimal,
  20A1 Elza, 4080 Leon, 4081 Elza. Gegen den FB_SHOT desselben Laufs weichen je 52 Bildpunkte ab (der blinkende
  Punkt, 13 px in 320x240). Der FB_SHOT dieser Läufe ist pixelgleich mit dem Abzug im Kartenlauf.
- Daten: `analysis/karte_r42/s3r5/abzuege.tsv`, `kampagne.txt`, `vergleich.txt`.

## 7. Kollisionen

`git merge-tree --write-tree` von r42/karte (02972374c) gegen die aktuellen Spitzen meldet **keinen Konflikt**:

| Zweig | Spitze |
|---|---|
| story | 93821acef |
| rest | 3e1a14809 |
| gegenstaende | 5515f2601 |
| finale | b54996114 |
| gore | b8b879d54 |
| lageplaene | c35ab51a5 |
| original | 7ec4ba1f0 |
| birkinfilm | b5bb0edd0 |
| namen | 4c53a03e6 |
| hangar_aussen | fe5436f55 |

**Vorwarnung:** story, gegenstaende und namen ändern ebenfalls `re15_inv_screen.c`, aber an anderen Stellen (diese
Runde: nur `tuer_anziehen`). Die Tabellen von U-11/U-12 (r42/rest) berührt diese Runde nicht.

## 8. Was offen ist (gelb)

1. **Zug 5090:** 3 Lagen außerhalb aller Zugteile (L5090_t2, L5090_w1, E5091_t2). Stand v0.8.30, gehört zu U-12.
   Gelb nach Vorgabe Punkt 3.
2. **20A0 an der Tür nach 2090:** Rot-Maß 0,30, knapp an der Grenze. Der Punkt steht unten im 8 px breiten Westarm.
3. **20A0, Kollision (nicht Karte):** Im ersten Lauf ging Leon von der Tür nach 2090 (4950,5468) zwei Sekunden nach
   Westen bis x -6825. Das liegt außerhalb jeder Kollisionszelle und jedes Kamerabereichs des Raums. Der Punkt klemmt
   dann an den Rand des Grundrisses (Rot-Maß 0). Das ist eine Lücke der Port-Kollision, nicht der Karte. Nachgestellt in
   `s3r5/kollision_20a0/L20A0_w2west` (Spiel-Log: `pl pos=(-6825,-1800,5468)` nach F360). Die Lage ist deshalb nicht
   im Kartenlauf; der Gehlauf an der Tür nach Süden bleibt stehen (Wasserrinne, Band-1-Zelle x 3250..6700). Gehört zu
   einem Kollisions-Strang.
4. **10B0_t0 / 10B1_t1:** Rot-Maß 0,29, wie in Runde 4. Der Westarm des Grundrisses ist 6 px breit.
5. **Grenzfälle mit ausgelieferter Zeile, unverändert:** 2000/2001 aus 1260 (0,20), 3090_w1, 6030_t1/6031_t1 und 11B0
   (0,25-0,29). Dazu 4070_w0 (0,25, Treppengitter).
6. **Neue Stücke ohne Türmarken:** 10B0, 1260, Labyrinth-Kammern und jetzt auch der 20A0-Grundriss. Die Tür nach 2090
   trägt weiter ihre Marke an r10.
7. **Zusatzbits:** In dieser Runde ist kein neues Bit dazugekommen (20A0 nutzt seinen zid 114). Frei bleiben 253-255.
8. **Türabstand 2(d):** unverändert begründet gelb wie in Runde 4 (30A0_t0, 30B0_t0, 30C0_t2, 50B0_t0, 50D0_t0, 4070_t0).
