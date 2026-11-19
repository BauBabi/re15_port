# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 3

Zweig r42/karte. Code-Commit f2ae0361a, danach ein Doku-Commit mit diesem Bericht. Kein Push.

## Kurz und in Alltagssprache

- **Der Zug ist wieder rot.** Steht Leon im Zug (Raum 5090), leuchtet der Zug wieder rot, so wie in v0.8.30.
  In Runde 2 war er braun.
- **Der Fehler lag in meinem eigenen Zweig.** In Runde 2 hatte ich ihn dem Zweig r42/rest zugeschoben. Das war
  falsch, die Pruefung hatte recht. Die Ursache war meine Reparatur aus Stufe 2: Damit der T-Gang neben dem Zug
  sichtbar wird, habe ich das ganze Feld gruen unterlegt. Das Rot des Zugs ist halb durchsichtig. Auf Gruen wird es
  braun. Jetzt bleibt unter dem Zug eine Luecke im Gruen. Der Zug liegt dadurch wieder direkt auf dem blauen Grund.
- **Ein neuer automatischer Test passt darauf auf.** Er prueft fuer jedes Zugstueck und fuer die drei Teile des
  Asservatenraums 1110: Unter einem roten Stueck darf nichts anderes liegen. Der Test schlaegt auf dem alten Stand
  v0.8.30 an und auf dem Stand von Runde 2 auch. Auf dem neuen Stand ist er gruen.
- **Elza in 4001 und 4031 hat jetzt Bilder.** Bisher lief dort eine Szene, und die Karte ging nicht auf. Ich habe
  in den Spieldaten den Schalter gefunden, der die Szene abstellt (Belege unten). Alle 7 Bilder sind gut.
- **Neuer Kartenlauf mit dem neuen Stand:** 619 Lagen, 607 Bilder. In STAGE2-6 steht der Punkt in 356 von 383 Lagen
  auf dem roten Raum. In Runde 2 waren es 334 von 377.
- **An denselben 144 Leon-Stellen wie v0.8.30:** vorher 85 gut, Runde 2 132 gut, jetzt 136 gut.
- **Schlechter als v0.8.30 sind noch 3 Stellen, nicht mehr 7.** Die 4 Zug-Stellen sind behoben. Uebrig sind 4070
  und 4080 (zweimal). Dort steht der Punkt jetzt **naeher an der Tuer**, durch die Leon hereinkam. Das Mass der
  Pruefung zaehlt das Rot rund um den Punkt. An der Tuer ist um den Punkt herum mehr Wand, deshalb faellt der Wert
  knapp unter die Grenze (29 % statt 30 %). Ich zaehle diese 3 Stellen trotzdem als **nicht erledigt**.
- **"Der Punkt steht ueberall richtig" ist noch nicht erreicht.** Offen sind 15 Grenzfaelle in STAGE2-6 und
  3 Lagen im Zug, an denen nichts rot ist (das war auch in v0.8.30 schon so). Dazu kommen 12 Lagen ohne Bild, jede
  mit Grund, und 10B0 und 1260 in STAGE1 ohne Punkt (Frage an dich).
- **Die Tafeln** zeigen jetzt jede Lage. Wo es kein Bild gibt, steht die Lage mit Grund auf einer eigenen Tafel.

Fachwoerter: "Blatt" = Kartenseite. "Kachel" = ein gemalter Raum auf dem Blatt. "Abzug" = Bild des Kartenschirms
direkt aus dem laufenden Spiel (FB_SHOT). "Riegel" = automatischer Test. "Gegenprobe" = derselbe Test auf dem alten
Stand muss fehlschlagen. "Ankunft" = die Stelle hinter einer Tuer, an die das Spiel die Figur stellt. "STP" =
Halbtransparenz-Bit der Farbe.

## Die Punkte der Pruefung (pruef_s3_r2)

| Punkt | Stand jetzt |
|---|---|
| ROT 1 Zug braun, falsch zugeordnet | **Behoben in f2ae0361a.** Die Ursache lag in diesem Zweig (94b132b32, "Grund der fremden Zone"), nicht bei r42/rest. Gemessen an L5090_t0/t1/t3/t4: 337/130/341/155 px in der Aktuell-Farbe (48,8,48), 0 braun. Das ist bitgleich mit v0.8.30. Im echten Fenster (gdigrab) sind es 1348 px = 4 x 337 bei Fenstergroesse 2x. |
| ROT 2 7 Stellen schlechter, Kurzfassung verschweigt das | Zug-Stellen behoben (4). **3 bleiben unter der Grenze**: 4070_t0 (0,20), 4080_t0 (0,29), 4080_t3 (0,29). Sie stehen oben in der Kurzfassung. Gegenmass unten: Abstand zur gemalten Tuer. |
| Elza 4001/4031 ohne Bild | **Behoben** (Szenen-Schalter, Belege unten): 7 von 7 gut. |
| 22 Lagen ohne Abzug, nicht auf den Tafeln | 10 davon haben jetzt ein Bild. Die uebrigen **12 stehen mit Grund** auf `tafel_99_ohne_abzug.png`. |
| GELB 1 Tafel-Satz "Rot = Raum, in dem die Figur steht" | Stimmt jetzt auch im Zug. Ausnahme sind 3 Zug-Lagen ausserhalb aller Zugstuecke (offen 2). Diese sind auf der Tafel rot als "Raum nicht rot" beschriftet. |
| GELB 2 "611 Abzuege" | Jetzt korrekt gezaehlt: **619 Lagen, 607 Abzuege, 12 ohne Abzug**. Alle 619 Lagen stehen auf 51 Tafeln. |
| GELB 3 Riegel [A] teils selbstbestaetigend | Neues, unabhaengiges Mass (nur Bericht, kein Riegel): Abstand Punkt zur **gemalten** Tuer der Ankunft. Siehe unten. |
| GELB 4/5 Abweichungen mit ausgelieferter Zeile, Mitlauf-Ausnahmen 4080/4020 | Unveraendert, offen (Liste unten). |
| GELB 6 10B0/1260 | Unveraendert, Frage an dich. |

## Was geaendert ist (f2ae0361a)

`re15_port/engine/src/re15_inv_screen.c`, Zeichner der Rechtecke mit Teilbereichen:
- Bisher: Ein Rechteck mit Teilbereichen, auf dem auch eine **fremde** Zone liegt (Blatt 9 r7: Zug 5090 und T-Gang
  5070/5130), bekam einen Grund ueber das **ganze** Rechteck. Darauf lagen die Teile.
- Die Aktuell-Zeile 502 hat den Eintrag 680808 **mit** STP-Bit (RE2 `addiu s5,s5,1` @0x8006E648). Ein aktueller
  Teil mischt sich also mit dem, was unter ihm liegt. Auf dem Blau des Blatts ergibt das (48,8,48), auf dem gruenen
  Grund (56,32,24).
- Jetzt: Der Grund wird nur noch auf dem Rest gezeichnet. Die Flaechen der Teile, die ueber ihm gemalt werden, spart
  er aus. Gemeint sind Teile mit staerkerem Zustand als der Grund. Rechnerisch ist das Rechteck minus die
  Teil-Ausschnitte, in Stuecken derselben Kachel-uv.
- Das ist eine PORT-WAHL ohne neue Verhaltens-Konstante. Die Teilbereiche selbst sind eine Port-Ergaenzung
  (Nutzer 2026-09-07). Das Original zeichnet je Rechteck zwei SPRTs ohne Zustand (FUN_80046fd8, Schleife
  @0x800472fc-0x800473e0).
- Pixelgleich ueberprueft: ROOM1110, T-Gang 5070 und 4080_t3 sind mit dem Endstand gleich wie im Kartenlauf.

## Riegel

`unit_r42_karte_sichtbar` (`tests/unit/test_r42_karte_sichtbar.c`):
- **[K] geaendert.** Der Test verlangte bisher ein **ganzes** gruenes Rechteck unter dem roten Zugteil, also genau
  den Defekt. Jetzt verlangt er nur noch: rot oben, gruen darunter.
- **[L] neu.** Ein aktueller Teil liegt auf keinem anders gefaerbten Stueck desselben Rechtecks. Nach einem
  Nachbarbesuch sind ausserdem mindestens 90 % der gemalten Texel gezeichnet. Das sind 18 Faelle:
  - die 5 Zugteile, jeweils an der Mitte ihrer Weltbox aus s_map_teile;
  - je nach 5070, nach 5070+5130 und ohne Nachbar;
  - dazu die 3 Teile von 1110.
- **Gegenprobe:**
  - auf 821a8b120 ROT: 882/294/238/72/63 px rot auf Gruen (`analysis/karte_r42/s3r3/gegenprobe_sichtbar_821a8b120.txt`);
  - auf aaea5f8c2 ROT: T-Gang nicht gezeichnet, 766 von 1145 Texeln; dazu [I]/[J] 5020 (eigener Baum aus `git archive`,
    `gegenprobe_sichtbar_aaea5f8c2.txt`);
  - auf dem neuen Stand gruen.

Tests auf dem Endstand:
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan|teil)" -j 6`: **60/60**.
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan|inv_screen|teil)" -j 3`: 20/22 im Lauf unter Last. Die beiden
  roten Tests (integration_r36_kartenitems_bild "kein Framedump", integration_r38_tagebuch_karte) habe ich einzeln
  wiederholt, danach **2/2 gruen**. Ergebnis also **22/22**. Dabei sind integration_map_raum_live und
  integration_map_uebergang.
- Die volle Suite ist nicht gelaufen (so im Auftrag).

## Kartenlauf (alle Bilder aus dem echten Spiel, Endstand)

- Liste wie in Runde 2: `kampagne.txt`, 611 Lagen plus 8 Laeufe mit 1 s statt 2 s.
- Neue Szenen-Schalter, mit Beleg aus der Szenensprache (SCD) des Raums:
  - **4001 (Elza):** `Ck bank=3 bit=33 val=0` -> `Evt_exec sub=10` -> `Set 3:33=1` @ROOM4001.RDT 0x0137C.
    sub10 ist die Eintrittsszene (Plc_dest/Plc_motion @0x018AE ff.).
  - **4031:** `Ck bank=3 bit=88 val=0` @ROOM4031.RDT 0x00552 -> `Evt_exec sub=8`. sub08 setzt 3:88 @0x0075C.
  - **E1041_t3:** Dort laeuft eine Szene bis Bild 187. Die Karte geht erst danach auf, deshalb habe ich das Ende
    des Laufs spaeter gelegt (Bild 600).

| | Lagen | ok | Punkt neben Rot | Raum nicht rot | kein Punkt | kein Abzug |
|---|---|---|---|---|---|---|
| STAGE1 | 236 | 216 | 3 | 0 | 14 (10B0, 1260) | 3 |
| STAGE2-6 | 383 | 356 | 15 | 3 (Zug, ausserhalb der Zugstuecke) | 0 | 9 |

- **Vorher/nachher an denselben 144 Leon-Lagen STAGE2-6** (`vergleich_basis.txt`):

  | Stand | gut | neben | nicht rot |
  |---|---|---|---|
  | v0.8.30 | 85 | 47 | 5 |
  | Runde 2 | 132 | 6 | 5 |
  | jetzt | 136 | 6 | 1 |

  - Besser als v0.8.30: 54 Lagen.
  - Schlechter: 3 Lagen (4070_t0, 4080_t0, 4080_t3).
- **Leon und Elza an derselben Lage:** 214 Paare mit gleicher Lage, davon 213 mit gleichem Punkt. Die Ausnahme ist
  11C0_t0 mit 1 px Abstand, beide ok (wie in Runde 2). Die uebrigen Paare mit ungleichem Punkt haben auch ungleiche
  Lagen, weil die Tuerlisten verschieden sind (z. B. 4000_t1).
- **Mitlaufen:** 130 Laeufe mit mindestens 1000 Einheiten Weg. Davon bewegt sich der Punkt in 125 um 2 px oder mehr.
  Fast still bleibt er bei 20A0_w0, 4040_w1, 4070_w0, 4080_w1 und 6000_w0 (offen 4).
- **Echtes Fenster** (gdigrab, normale Grafik, `fenster_echt.png`): 5090, 5070, Elza 4001 und 4080. Gegen den
  FB_SHOT desselben Laufs: 99,98 % bzw. 100 % gleich. Der Rest ist der blinkende Punkt.

## Unabhaengiges Mass: Abstand zur gemalten Tuer (Antwort auf GELB 3)

- Bei einer Ankunft steht die Figur direkt hinter der Tuer. Wo das Original diese Tuer **gemalt** hat, muss der Punkt
  in ihrer Naehe stehen.
- Die Lage der gemalten Tuersymbole kommt aus der Kunst DATA/MAP0x.PIX (Kommentar "gemaltes Symbol (x,y)" der
  Tuermarken in re15_map_zones.h, Stufe 1 Runde 2). Sie kommt nicht aus der Rechnung fuer den Punkt.
- Marken "Mitte der Projektionen" stammen aus der Zeile selbst und sind deshalb ausgelassen.
- Werkzeug: `werkzeuge/tuerabstand.py`.

Ergebnis an den 59 gemeinsamen Lagen mit gemaltem Symbol (v0.8.30 gegen jetzt):

| | Median | besser | gleich | schlechter |
|---|---|---|---|---|
| Abstand zur gemalten Tuer | **9 px -> 5 px** | 35 | 19 | 5 |

- Die 5 schlechteren Lagen haben jeweils hoechstens 9 px Abstand: 30A0_t0 3->7, 30B0_t0 5->6, 30C0_t2 5->7,
  50B0_t0 4->9, 50D0_t0 2->6.
- Alle 126 Ankuenfte mit gemaltem Symbol liegen jetzt hoechstens 14 px davon entfernt. Vorher waren es bis zu 77 px.
- Die 3 Stellen, die nach dem Rot-Mass schlechter sind:
  - **4080_t3** (aus 40B0): Abstand zur gemalten Tuer (175,163) 8 px -> **3 px**. Der Punkt steht am Fussende der
    Kachel. Dort ist an drei Seiten Wand, deshalb 29 % Rot.
  - **4070_t0** (aus 4040): Abstand zur gemalten Tuer (141,163) 5 px -> **4 px**. Der Punkt ist um 1 px verschoben
    und liegt im kleinen Feld des gemalten Treppengitters.
  - **4080_t0** (aus 4030): Diese Tuer hat kein gemaltes Symbol. Der Punkt steht jetzt 2 px neben dem Ende der
    gemalten Diagonale r13, wo sie den Kreis 4030 beruehrt (166,114). Vorher stand er 10 px entfernt am Ende von r8.
    Dieses Ende grenzt in der Kunst gar nicht an 4030.
  - Ursache der Verschiebung ist die in Stufe 2 hergeleitete 4080-Zeile: {127,169,1540,1605} statt {125,170,1793,1589}.
    Das ist keine neue Aenderung dieser Runde.
- Bewertung: Nach dem unabhaengigen Mass stehen die Punkte naeher an der Tuer als in v0.8.30. Nach dem Mass der
  Pruefung bleiben es Grenzfaelle. Ich melde sie deshalb als **offen**.

## Was noch offen ist

1. **15 Grenzfaelle STAGE2-6** (20-29 % Rot um den Punkt), alle auf der eigenen Kachel:
   - 2000 aus 1260 und 2001;
   - 3090_w1;
   - 4070_t0/w0 und 4071;
   - 4080_t0/t3 und 4081 (zwei Lagen);
   - 6000_t0/w0 und 6001 (Gang nach 50B0 ungemalt);
   - 6030_t1 und 6031.
   - 2000, 3090 und 6030 rechnen mit der **ausgelieferten** Zeile (byte-true, unveraendert).
2. **Zug 5090: 3 Lagen ohne Rot** (L5090_t2, L5090_w1, E5091_t2, Lage (300,5000) bzw. (13025,-23550)).
   - Die Lagen liegen in keiner der 5 Weltboxen der Zugteile (s_map_teile Blatt 9 r7: Teil 0 z <= -4716, die Teile
     1-4 x >= 20532). Also ist kein Teil aktuell.
   - In v0.8.30 war es genauso (L5090_t2 schon damals "Raum nicht rot").
   - Das gehoert zur Aufteilung des Zugs (U-12, r42/rest). **Vorwarnung unten.**
3. **Mitlaufen fast still:**
   - **4080_w1** (0 px bei 2989 Einheiten). Gemessen: Die Zeile setzt die Ankunft aus 4090 auf x=206 (sichtbar 205).
     Der gemalte Arm beginnt aber erst bei x=211 und ist nur 7 px breit (Abzug, Zeilen 125-134). Nach 3000 Einheiten
     nach Osten waere die Formel bei x=210, also immer noch links vom Arm. Die Klemme haelt den Punkt deshalb am Rand.
     Ursache: Der Kuenstler hat den Arm nicht massstabsgetreu gemalt (etwa 0,9 px je 1000 Einheiten, die Zeile hat
     1,47). Eine einzige lineare Zeile passt nicht zu beiden Teilen des Raums. Nicht behoben.
   - **6000_w0**: Der Gang nach 50B0 ist nicht gemalt, der Punkt bleibt an der Tuer.
   - **20A0_w0, 4040_w1, 4070_w0**: je 1 px, nicht untersucht.
4. **12 Lagen ohne Abzug** (`tafel_99_ohne_abzug.png`, Gruende in `gruende.json`):
   - 1240/1241 und 6040/6041 sind reine Szenenraeume: nDoor=0, sub00 `Evt_exec` ohne Bedingung @ROOM1240.RDT 0x00544
     bzw. @ROOM6040.RDT 0x00522. Die Karte ist dort nie aufrufbar.
   - 50B0_w0/w1 und 6030_w0/w1: Der Lauf loest eine Szene aus (Plc_dest/Plc_motion bzw. die Wagen-Szene), die Karte
     ist gesperrt. Mit 1 s statt 2 s klappt es fuer 50B0_w0 und 6030_w0, nicht fuer 50B0_w1 und 6030_w1.
5. **10B0/1260 (STAGE1) ohne Punkt**: Beleg unveraendert aus Runde 2 (@0x80076908 Stub, @0x8004b574
   `sltiu v0,v1,0x26`). Frage an dich.
6. **Ausgelieferte Zeile gegen Original-Formel**: 248 Abzuege.
   - 166 genau, 23 mit 1 px, 14 mit 2 px.
   - Die groesseren Abstaende (1170/1190/11A0/11B0, 30E0 Zone 0, 6030, 1030) sind vorbestehende Ausnahmen aus
     STAGE1 bzw. Stufe 2, unveraendert.
7. Mitlauf-Ausnahmen in st456 (4080 Zone 1, Fahrstuhl 4020) unveraendert.

## Was gegenueber vorher besser ist (fuer dich)

- Im Zug ist der Raum wieder rot, wie in v0.8.30. Der T-Gang daneben bleibt sichtbar.
- Elza hat jetzt in 4001 und 4031 eine Karte mit Punkt (7 Bilder, alle gut).
- 10 Lagen, die in Runde 2 kein Bild hatten, haben jetzt eins. Die restlichen 12 stehen mit Grund auf einer Tafel.
- In STAGE2-6 ist der Punkt jetzt an 356 von 383 Lagen auf dem roten Raum. In Runde 2 waren es 334 von 377, in
  v0.8.30 an den gemeinsamen Stellen 85 von 144 (jetzt 136 von 144).
- Bei Ankuenften steht der Punkt naeher an der gemalten Tuer: Median 5 px statt 9 px in v0.8.30.

## Bilder

- Fuer dich: `analysis/karte_r42/s3r3/tafeln/` (Kopie `scratchpad/karte/s3r3/tafeln/`).
  - 50 Kartentafeln je Blatt und Figur mit allen 607 Abzuegen. Jedes Feld traegt diese Beschriftung:
    - STAGE (aus der Raumnummer), Blattname, Raum und Figur;
    - "Ankunft aus XXXX" oder "2 s / 1 s gelaufen ab Tuer aus XXXX" mit der Lage;
    - Abweichungen in roter Schrift (35). Der pinke Kreis ist nur eine Suchhilfe.
  - Dazu `tafel_99_ohne_abzug.png` mit den 12 Lagen ohne Bild und ihrem Grund.
- `fenster_echt.png`: vier Bilder des echten Spielfensters (gdigrab): Zug rot, T-Gang, Elza 4001, 4080.
- Messbild `mb_zug.png` (Kopfzeile "Messbild - kein Ergebnis"): Zug in v0.8.30 / Runde 2 / jetzt.

## Kollisionen

`git merge-tree --write-tree` von r42/karte (f2ae0361a) gegen die anderen r42-Zweige meldet **keinen Konflikt**:

| Zweig | Stand |
|---|---|
| r42/rest | 3e1a14809 |
| r42/gegenstaende | 5515f2601 |
| r42/finale | 6ae26a1a5 |
| r42/gore | b8b879d54 |
| r42/story | 5849c8996 |
| r42/lageplaene | c35ab51a5 |
| r42/original | db67b1388 |
| r42/birkinfilm | b5bb0edd0 |

Kartendateien beruehren nur zwei dieser Zweige:
- r42/rest: `re15_map_owned.c` +4 Zeilen;
- r42/gegenstaende: `re15_inv_screen.c` +4 Zeilen, an anderer Stelle.

**Vorwarnung U-12 (r42/rest):**
- Die Abhilfe dieser Runde sitzt im Teilbereichs-Zweig des Zeichners (`re15_inv_screen.c`, "Grund der fremden Zone").
  Baut U-12 die Zugteile um, etwa auf die Wagenkacheln von Blatt 12, muss dort dieselbe Regel gelten: Kein
  halbtransparenter Aktuell-Teil darf auf einem anderen Grund liegen. Riegel [L] prueft das fuer Blatt 9 r7 und
  Blatt 3 r5.
- Die 3 Zug-Lagen ohne Rot (offen 2) liegen ausserhalb aller Zugstuecke. Das gehoert zur Aufteilung des Zugs.
- Ausserdem hat r42/rest (f7392eb72, U-08) die Tuer 4001 -> 401 geoeffnet. Das aendert nichts an den hier
  gemessenen Ankuenften in 4001, die Tuerliste kann sich beim Mischen aber verlaengern.

## Dateien

- Code: `re15_port/engine/src/re15_inv_screen.c`. Riegel: `re15_port/tests/unit/test_r42_karte_sichtbar.c` ([K], [L]).
- Daten und Werkzeuge: `analysis/karte_r42/s3r3/`, darin:
  - `abzuege_s3r3.tsv`, `auswertung.txt`, `summe.txt`, `vergleich_basis.txt`, `tuerabstand.txt`, `gruende.json`;
  - die Gegenproben;
  - `werkzeuge/`.
- Arbeitsordner: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r3/`.
