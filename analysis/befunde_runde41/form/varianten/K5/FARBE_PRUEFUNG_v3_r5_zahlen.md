# Pruefbericht Zahlen, Farbe Versuch 3 Runde 5 (K5c, nur die Krallen)

Pruefer: Zahlen-Pruefer, 2026-11-07 Abend. Nicht committet.

Gegenstand: `farbe/K5c_farbe_v3/`.
- FORM.EMD sha256 090e0b80e214ae45..., bytegleich mit `farbe/K5C_FARBE_V3.EMD`.
- FORM.TIM sha256 512828067ea7f048..., bytegleich mit `farbe/K5C_FARBE_V3.TIM` und mit Runde 4 (`farbe/K5c_farbe_v3r4/FORM.TIM`).
- Zweig r41/form, HEAD 278a4792c. Geprueft wurden die Commits 07578cc04, 14a0831fb, 6349350a4, e2816e70f, 2b3bb75ba und 278a4792c.

Bindend ist der Nutzersatz vom 2026-11-07 ~17:10: "... entfernst die uebertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"
Runde 5 ist laut Orchestrator auf die Krallen begrenzt.

Ausserdem hat der Workflow diese Nutzernachricht weitergereicht (woertlich):
"Nein, es passt eben nicht.... man wy... ich wei0 nicht wie ich das noch beschreiben soll.... da ist links ein Auge. Linke Seite ist so
graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?"
Dazu Abschnitt 7.

**Eigener Code** liegt im Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/pz_r5/`:
- `md1.py`, `tim.py`: eigene EMD-, MD1- und TIM-Leser.
- `diff1.py`, `diff2.py`, `diff3.py`: Byte-Diff r4/r5/r3, Zuordnung zu Flaechen und Meshes, CLUT-Zeilen und Texel.
- `texel.py` -> `texel.json`: Adern gegen Haut im Texelraum, je CLUT-Zeile und je Aderkomponente. Ader = G4-Basisfarbe des Index mit a\* >= 8 (Nachtrag 1.3, A0 Texel), Ring = 3 Texel.
- `kennung.py`: eigene Kennungs-CLUT mit diesen Codes:
  - Ader rot, Haut gruen (aus der G4-Basisfarbe);
  - Leuchtzeilen 6, 7 und 8 blau;
  - Krallenzeile magenta;
  - Zeile 0 (Maulinneres und Augen) gelb.
- `szene.py` -> `szene.json`: beide Szenenketten fuer r3, r4 und r5. Darin:
  - Ader gegen Haut je Pixel und in 12 x 12-Bloecken;
  - RV1-RV4 nachgerechnet auf `rot_masken.npz`.
- `saat.py`: RV1-RV4 so gerechnet, wie der Nachtrag sie definiert (Mass je Saat, dann Mittel), fuer die Saaten 15/12345/7 und 1/2/3, an r5 und r4.
- `tupfen.py` -> `tupfen.json`: eigene Tupfen-Zaehlung an den Krallen. Dazu Rot eng der Krallen allein.
- `riegel_nach.txt`: Nachlauf von `riegel_farbe_v3.py` mit `--gegenprobe-v2`, `--gegenprobe-r3` und `--gegenprobe-r4`.
- `ctest.log`: eigener Testlauf.
- `tdiff_*.png`: Tafel-Diff Runde 5 gegen Runde 4. Gruen markiert sind die verschiedenen Pixel.

## 0. Ergebnis

**Nicht bestanden. Der Krallen-Umfang selbst ist aber sauber.**

**Was stimmt (eigener Code):**
- Runde 5 aendert genau das, was sie aendern soll: 252 Krallenflaechen, und an ihnen nur das CLUT-Feld.
- Die Krallen sind in allen Feldern bytegleich mit Runde 3.
- Die Tupfen der Runde 4 sind weg; im Rohbild sind die Zahlen identisch mit Runde 3.
- RV1-RV4 45/45, RV6 gruen.
- Geometrie-Diff 0.
- Spiel-Asset, Pins und re15_port sind unberuehrt.
- Form-Tests 11/11.

**Was gegen eine Abnahme spricht:**
1. **P2 Klauen ist weiter ROT: +16,3 / +21,0 bei der Grenze +8.**
   - Der Orchestrator hat P2 Klauen ausdruecklich als Pruefpunkt der Krallen genannt.
   - Der Wert ist genau der Stand der Runde 3, denn die Krallenflaechen sind bytegleich mit Runde 3.
   - Er ist also kein neuer Fehler, aber auch nicht erfuellt.
2. **Das Riegel-Paket endet mit Exit 1, 77 von 80 bindenden Zeilen GRUEN.**
   - Die 3 ROT sind P2 Klauen, P2 uebrige Teile und B11.
   - B11 war in Runde 4 GRUEN (3,72). Laut Bauer kam das aber nur von den Tupfen; ohne Kralltexel sind es in Runde 4 und in Runde 5 gleich 3,25.
   - Das ist plausibel, denn ausser den Krallen ist alles bytegleich.
3. **Die Krallen sind deutlich roeter als die Vorlage. Das widerspricht "Rotmenge nach Vorlage".**
   - Bild 2, Kette T, Krallen allein:
     - Rot eng: Runde 5 76,3 %, Vorlage 46,4 %.
     - a\*: Runde 5 27,2, Vorlage 16,7.
   - Die alte bindende Zeile "Bild 2 Vorderbeine RV1" ist mit 77,32 % ROT (Fenster bis 74,45). Sie wurde in Runde 5 zur Info gemacht.
   - Bindend ist jetzt eine neue Region ohne Krallen. Das deckt der Orchestrator-Auftrag ("sonst die Krallen-Texel aus der RV-Messregion ausnehmen").
   - Der Auftrag verlangte, das "im Nachtrag mit Zahl zu belegen". Der Nachtrag `MESSBLATT_FARBE_V3_NACHTRAG_ROT.md` ist **nicht** geaendert; der Beleg steht nur im Dossier V3R5.1 und in `riegel_rot.py`.
4. **Die Nutzernachricht oben lehnt das Mischen der Farben ab ("Warum mischst du die Farben so daemlich?").**
   - Runde 5 laesst die Zweifarbigkeit absichtlich unveraendert: Rumpf, Flanke und Schwanz violett, Vorderteil braunrot.
   - Gemessen im Spielfenster 5080: Runde 5 und Runde 4 unterscheiden sich nur in 172 Pixeln an den Krallen.
   - Nach der Aufgabe ist das kein Ablehnungsgrund. Die Nutzerstimme hat aber Vorrang, und sie sagt "passt nicht". Abschnitt 7.

## 1. Aenderungsumfang gegen Runde 4 (eigener Byte-Diff)

| Pruefpunkt | Messung (`diff1.py`, `diff2.py`, `diff3.py`) | Bewertung |
|---|---|---|
| EMD r4 -> r5 | 504 Bytes verschieden, alle in UV-Saetzen an Byte 2-3 (CLUT-Feld). Wert 0x7C00 (Zeile 16) -> 0x7840 (Zeile 1), je 252 mal | GRUEN |
| Flaechen | 252 geaendert: Mesh 14: 30, 15: 42, 19: 30, 20: 30, 28: 30, 29: 30, 30: 30, 31: 30 = die Krallen-Meshes. Uebrige Flaechen: 0 geaendert | GRUEN |
| Geometrie | Bytes ausserhalb der UV-Saetze: **0** verschieden (Ecken, Normalen, Flaechen, Skelett, Frames, Clips) | GRUEN |
| TIM r4 -> r5 | bytegleich (sha256 51282806...), also 0 Texel und 0 CLUT-Zeilen geaendert, 17 Zeilen | GRUEN |
| Krallen gegen Runde 3 | 252/252 UV-Saetze bytegleich mit r3. TIM-Zeilen 0-2 r3 = r5. Texelbild r3 = r5 zu 100 % | Krallen = Runde 3, bytegenau |
| CLUT-Nutzung r5 | Zeile 1 nur von den 252 Krallenflaechen benutzt; Zeile 0 von Kopf/Kiefer (155 Flaechen = Maulinneres F2 91 + Augen F10 64, bau.json) | wie gemeldet |
| Pfade | `git diff --name-only 657512427..HEAD` ausserhalb `varianten/K5/`: keine. re15_port zuletzt 56050f737 (Spur N3), BAHNSTEIG_FORM zuletzt 67408cfcb (K4) | GRUEN |

Die Meldung des Bauers (AE1) stimmt in jeder Zahl.

## 2. Krallen

### 2.1 A7 und P2 (Riegel-Nachlauf, `riegel_nach.txt`)
- Der Nachlauf ist bis auf die Stelle der Zeile "EXIT 1" bytegleich mit `farbe/riegel_K5c_farbe_v3.txt` (Vergleich ohne CR).
- **A7 Kralle 0,096: GRUEN** (Runde 3 0,096, Runde 4 0,349).
- **P2 Klauen (W) l +16,3, r +21,0: ROT.**
  - Die Krallenflaechen, die Texel und Zeile 1 sind bytegleich mit Runde 3. Der Render der Klauen ist deshalb derselbe wie in Runde 3.
  - Den Runde-3-Wert ("+16,3 / +21,0", Dossier V3R3 Lauf v12) habe ich nicht neu gerechnet; er folgt aus der Bytegleichheit.
  - Die Ursache liegt also nicht in der Farbzeile. Zu der Behauptung "UV-Vergroesserung der Krallenwurzel 1,73-2,22" habe ich keine eigene Messung.

### 2.2 Tupfen (eigene Umsetzung, `tupfen.py`)

**Definition:**
- Tupfen = Pixel mit L\* > Median der Krallenpixel im 15 x 15-Fenster + 10. Gegenprobe mit 9 x 9.
- Komponenten in 8-Nachbarschaft.
- Das ist bewusst ein anderes Umfeld als beim Bauer (Box-Mittel 9 x 9).

**Masken:**
- Krallen Bild 2: eigener Kennungs-Render, 6265 px. Er ist gleich der Bauer-Maske; mit 2 px Saum ist er pixelgleich `krallen_bild2.npz` (8701 px, XOR 0).
- Die Seiten wurden mit einer EMD-Kopie getrennt: rechte Krallen auf Kennungszeile 6. Linke Klaue (W) 1112 px, rechte Hand (W) 5153 px.
- Spielfenster: Kennungs-Spielbild des Bauers, 1356 px.

| Ansicht | Teil | Vorlage | Runde 3 | Runde 4 | Runde 5 |
|---|---|---|---|---|---|
| Bild 2, Disc-Licht roh, 15 px | linke Klaue (W) | 59 / 11 | 81 / 15 | 169 / 30 | **81 / 15** |
| | rechte Hand (W) | 606 / 17 | 136 / 34 | 384 / 64 | **136 / 34** |
| Bild 2, roh, 9 px | linke Klaue / rechte Hand | 28/7, 398/23 | 37/16, 65/30 | 145/31, 275/63 | **37/16, 65/30** |
| Bild 2 + Videofilter Saat 15, 15 px | linke Klaue / rechte Hand | - | 14/8, 68/10 | 20/8, 201/17 | **20/8, 80/10** |
| Spielfenster 5090 (gdigrab), 15 px | alle | - | 61 / 14 | 149 / 23 | **65 / 16** |
| Spielfenster 5090, 9 px | alle | - | 29 / 12 | 99 / 23 | **29 / 13** |

Lesart der Tabelle:
- **Roh** ist Runde 5 identisch mit Runde 3, in beiden Fenstern.
- **Gefiltert** liegt Runde 5 etwas ueber Runde 3. Der Videofilter verschmiert den Koerper (Runde 3 hatte andere Koerperzeilen) in die Kralle; die Krallenpixel selbst sind gleich.
- **Im Spielfenster** liegt Runde 5 gleichauf mit Runde 3 (+4 px / +2 Komponenten bzw. +0 / +1). Die Laeufe haben verschiedene Feuer-Frames.
- Die Ziele des Sicht-Pruefers (426 / 53 bzw. 136 / 26) beruhen auf einem anderen Verfahren. Mit gleichem Verfahren gilt "wie Runde 3": **erfuellt**.
- Die Vorlage-Spalte trifft wegen der Lageabweichung Hintergrund und Feuer; sie ist nur Info.
- **Durchgehender Verlauf statt Mosaik:**
  - Tafel 10 Z.1 zeigt fuer Runde 3 und Runde 5 dieselbe dunkelrote Klaue mit heller Spitze, ohne Sprenkel.
  - Das folgt zwingend aus der Bytegleichheit mit Runde 3.

### 2.3 Rotmenge der Krallen gegen die Vorlage (Kette T, je Saat, Mittel)

| | Vorlage | Runde 3 | Runde 4 | Runde 5 |
|---|---|---|---|---|
| Rot eng alle Krallen % (A0 19) | 46,35 | 77,08 | 49,61 | **76,34** |
| linke Klaue / rechte Hand % | 60,1 / 43,4 | 100 / 72,1 | 95,9 / 39,6 | **99,5 / 71,3** |
| Lab Mittel (T) | 39,4 / 16,7 / 16,5 | 37,9 / 28,6 / 22,2 | 30,2 / 19,9 / 18,9 | **36,8 / 27,2 / 23,4** |

Der Bauer misst dasselbe (51,0 / 78,1 / 51,6 %; seine Region ist leicht anders). Die Runde-3-Krallenfarbe ist im Feuerlicht etwa 30 Punkte roeter als die Video-Klaue.
Das ist die bewusste Wahl "Runde-3-Farbe" (Dossier V3R5.5 Punkt 1). Gemessen am Nutzersatz "Rot nach Vorlage" ist es eine Abweichung, die dem Nutzer vorgelegt werden muss.

## 3. RV1-RV6 (eigene Rechnung)

- **RV1-RV4 nach Nachtrag-Verfahren (`saat.py`, Mass je Saat, dann Mittel):**
  - Runde 5: **45/45 GRUEN** mit den Saaten 15/12345/7 und ebenso mit 1/2/3.
  - Runde 4: ebenfalls 45/45.
  - Die Werte stimmen auf 2 Nachkommastellen mit dem Bauer ueberein, z. B. Vorderbeine ohne Krallen 75,01 / 35,59 / 0,81 / 44,06 und Hals RV3 -1,87.
- **Gegenprobe Methode (`szene.py`, erst Lab ueber die Saaten mitteln, dann messen):**
  - 44/45. **Bild 2 Hals RV3 -0,82 liegt ausserhalb des Fensters -6,27..-1,53.** Runde 4 hat mit derselben Methode -0,81.
  - Die Zeile ist also knapp (0,3 L\* Abstand) und haengt an der Methode. Durch Runde 5 ist sie nicht beruehrt.
- **Runde 3 mit eigener Rechnung:** 19/45 (26 ROT); `riegel_rot.py` meldet 20/45. Die Gegenprobe faellt aus wie verlangt.
- **Rundlauf** (`riegel_rot.py --rundlauf`, Nachlauf): 45/45.
- **Neue Region "Vorderbeine ohne Krallen":**
  - Vorlage und Variante werden auf derselben Pixelmenge gemessen (Kennung + 2 px Saum, eigene Maske pixelgleich).
  - Das Ziel ist nach dem Verfahren von `ziele.py` gebildet: O 64,7, Fenster 53,27..76,11.
  - Fair ist das nur mit dem Auftrag, die Krallen auszunehmen. Die Krallen selbst sind roeter als die Vorlage (2.3); das Ausnehmen verschiebt sie aus der Messung, nicht aus dem Bild.
  - Die Saum-Konstante (5 x 5 = 2 px) traegt in `ziel_ohne_krallen.py` keinen Quellen-Kommentar. Gemeint ist offenbar Nachtrag 1.5 (2 px = Optik-sigma).
- **RV6:** im Riegel GRUEN, alle Regionen dL und da\* <= 0. Eigene Gegenrechnung in Abschnitt 4.

## 4. Adern nirgends heller oder roter als die Haut, keine roten Bloecke (eigener Code)

### 4.1 Texelraum (`texel.py`, Runde 5; je CLUT-Zeile)

| Zeile / Gruppe | Ader-Texel | dL Ader - Ring | da\* Ader - Ring | Aderpixel in Komponenten heller (> 0,5) / roter (> 0,5) |
|---|---|---|---|---|
| 3 Schwanz | 1011 | -3,8 | +3,2 | 2,0 % / 100 % |
| 4 Hinterbeine | 366 | -2,2 | +1,6 | 15,9 % / 74,8 % |
| 5/9 Rumpf Ruecken | 29 / 2 | -1,3 / -5,5 | -3,9 / +1,8 | 0 / 0 |
| 10 Rumpf Flanke | 29 | -1,0 | -10,1 | 0 / 0 |
| 12 Vorderbein r B1 | 1985 | -3,8 | +1,8 | 1,7 % / 98,2 % |
| 13 Brust | 905 | -4,4 | +1,0 | 0 / 56,9 % |
| 14 Hals Bauch | 867 | -5,4 | +2,2 | 2,7 % / 61,8 % |
| 15 Kopf | 927 | -3,7 | -1,4 | 1,3 % / 55,1 % |
| 16 Vorderbeine | 2554 | -6,2 | +1,1 | 0,9 % / 61,1 % |
| 1 Krallen (G4-Zeile 1) | 317 | -19,4 | +14,9 | 0 / 100 % |
| 0 Maulinneres/Augen (G4 roh, Leuchtstelle) | 507 | +28,7 | +41,0 | unveraendert seit Runde 3, ausgenommen wie RV6_AUS |

**Heller:**
- Die Adern sind ueberall dunkler als die Haut.
- Lokal sind in den Hinterbeinen 15,9 % der Aderpixel (36 von 226 Texeln in Komponenten) um mehr als 0,5 L\* heller.
- Komponenten mit dL > 3 oder da\* > 3 gibt es dort 8 mit zusammen 94 Texeln. Das ist unveraendert seit Runde 4.

**Roter:**
- Mit der Aderschwelle a\* >= 8 sind die Adern im Mittel um +1,0..+3,2 a\* roeter als ihr Ring.
- Mit der Schwelle a\* >= 15, also den deutlichen Adern, ist da\* <= 0 in allen Koerperzeilen: Schwanz -0,5, Vorderbeine -1,1, Hals -0,8.
- Das schwache Plus kommt also vom blassen Aderrand. Das ist kein "roter als die Haut" im Sinne von Feuerrillen, woertlich "nirgends" ist es aber nicht.
- Der Riegel RV6 (G4-Rampen-Klasse) misst da\* -0,4..-3,6. Die Aussage haengt an der Ader-Definition.
- Durch Runde 5 ist das unveraendert: Koerperzeilen und Texel sind bytegleich mit Runde 4.

**Krallen:** Die "Adern" der Klaue sind ihr dunkelroter Koerper. Er ist roeter als die khaki Spitze (+14,9 a\*), aber nicht heller. Das ist die Runde-3-Klaue (2.3).

### 4.2 Szenenketten (`szene.py`, Kennungs-Render, Messmaske `rot_masken.npz` x Figur, Ring 3 px)

| | dL Ader - Haut | da\* Ader - Haut | 12 x 12-Bloecke heller > 2 / roter > 2 |
|---|---|---|---|
| Bild 1, Kette T | -1,49 | +0,58 | 23 / 137, 10 / 137 |
| Bild 1, Texel x Licht roh | -1,47 | +0,67 | 8 / 137, 37 / 137 |
| Bild 2, Kette T | -0,69 | +1,45 | 5 / 206, 30 / 206 |
| Bild 2, Texel x Licht roh | -1,61 | -0,40 | 0 / 206, 32 / 206 |
| (Runde 4, Bild 2, Kette T) | (+0,84) | (+1,12) | (5 / 227, 47 / 227) |
| (Runde 3, Bild 1, Kette T) | (+2,17) | (+2,85) | (51 / 128, 13 / 128) |

**Ergebnis:**
- **Im Mittel sind die Adern in beiden Ketten dunkler als die Haut.**
- In Bild 2 ist Runde 5 besser als Runde 4: dL -0,69 statt +0,84, weil die gesprenkelten Krallen nicht mehr als Haut und Ader zaehlen.
- Leicht roeter (+0,6 / +1,5 a\*) sind sie in der Kette. Roh sind sie in Bild 2 sogar weniger rot.
- Feuerrillen (hell UND rot) gibt es nicht mehr; in Runde 3 waren es 51 von 128 hellen Bloecken.

### 4.3 Rote Bloecke
- RV4 (groesste Rotflaeche eng) ist in allen bindenden Regionen GRUEN (eigene Rechnung).
- Beispiele: Bild 1 Figur 2,70 % (Grenze 5,55), Schwanz 6,92 % (Grenze 9,65); Bild 2 Figur 47,40 (Grenze 87,08).
- Neue rote Bloecke gibt es nur an den Krallen selbst: linke Klaue 99,5 % Rot eng. Das sind 1112 px, 2,9 % der Bild-2-Figur.

## 5. Tafeln (Diff Runde 5 gegen `farbe/v3r4/`, `tdiff_*.png`)

| Tafel | Diff | Bewertung |
|---|---|---|
| 01, 06, 08 | 0 Pixel (bytegleich) | wie gemeldet |
| 02 | Z.1 (roh): nur Krallen + Kopfzeile. Z.2/Nahkachel (Videofilter): Bloecke > 10 nur um die Krallen, sonst Streurauschen +-1-2 (151 832 px verschieden, davon 51 272 > 2) | bildgleich ausser Krallen (Filter-Rauschen) |
| 03, 05 | nur Krallen und Beschriftung | GRUEN |
| 04 | Krallenfeld im Atlas, Herkunftskarte (Klauen) und Beschriftung | GRUEN |
| 07 | neuer Spiellauf. 5080: 172 px verschieden, Box x 273-341, y 201-226 = Krallen. 5090: Feuer-Frame verschieden | GRUEN |
| 09 | neue Zeile "Vorderbeine ohne Krallen" (Hoehe 2421 -> 2667) | wie gemeldet |
| 10 neu | je Zeile gleicher Ausschnitt und gleiches Licht, roh, kein Gamma. Z.3: Runde 3 aus einem anderen Feuer-Frame (in der Unterschrift genannt) | fair |

Ein einseitiges Aufhellen ist nicht zu finden. Die Familienkacheln liegen unter gleichem Licht.

## 6. Tests, Gegenproben, Konstanten

| Pruefpunkt | Ergebnis | Beleg |
|---|---|---|
| Form-Tests | `ctest -R "form\|bahnsteig\|birkin_form\|r41_form" -j 3`: **11/11**, 95,42 s | `pz_r5/ctest.log` |
| Riegel-Paket | 77/80 bindend GRUEN, **Exit 1**; Nachlauf gleich der Bauer-Tabelle | `pz_r5/riegel_nach.txt` |
| Gegenproben V2/G4/(b2)/Orange/Gelb | alle ROT bzw. GRUEN wie verlangt, "falsch ausgefallen: keine" | ebd. |
| Gegenprobe Runde 3 | RV 25/45 ROT, RV6 VERLETZT in allen Regionen, AE1 ROT (3340 Nicht-Krallen-Flaechen, 9 statt 17 Zeilen) | ebd. |
| Gegenprobe Runde 4 | A7 Kralle 0,349 ROT, P2 Klauen +39,8 / +50,4 ROT | ebd. |
| Rundlauf | 45/45 | Nachlauf `riegel_rot.py --rundlauf` |
| Konstanten R5 | `R5_KRALLEN` mit Quelle (bau.json r3 flaechen_herkunft, Pruefberichte R4); AE_REF = farbe/K5c_farbe_v3r4. Der 2-px-Saum in `ziel_ohne_krallen.py` hat keinen Quellen-Kommentar (Kleinigkeit) | Code |
| Nachtrag | nicht nachgefuehrt (Auftrag: "im Nachtrag mit Zahl belegen") | `git log` Nachtrag: letzter Commit 657512427 |

## 7. Nutzernachricht zur Farbmischung

- Die Nachricht beschreibt eine Figur, deren linke Seite grau ist (Auge bis Nase) und deren rechte Seite braeunlich (Haare). Der Nutzer lehnt das Mischen der Farben ab.
- Welche Ansicht er meint, steht nicht in der Nachricht. Gemessen ist:
  - Unter weissem Licht und im Spielfenster 5080 ist die Variante seit Runde 4 zweifarbig: Rumpf, Flanke und Schwanz violett, Vorderbeine, Brust und Hals braunrot.
  - Zahlen des Sicht-Pruefers Runde 4 im 5080-Fenster: violett 28,7 %, braun 33,9 %, neutral 37,3 %.
  - Runde 5 aendert daran nichts: 5080-Spielbild r5 gegen r4 nur 172 px verschieden, alle an den Krallen. Tafeln 03 und 05 unterscheiden sich nur an den Krallen.
- Die Aufgabe nennt den Zweifarben-Befund "bewusst unveraendert (Nutzerfrage)". Die Nutzernachricht ist aber die Antwort darauf, und sie lautet "passt nicht".
- **Diese Runde kann also nicht als Abnahme gelten.** Die Farbmischung muss mit dem Nutzer geklaert werden, am besten an dem Bild, das er beschreibt.

## 8. Fazit

- Der Krallen-Umfang ist sauber und genau so umgesetzt wie gemeldet. Gegen Runde 4 haben die Krallen keinen Rueckschritt.
- Nicht bestanden ist die Runde wegen:
  - P2 Klauen ROT, auf Runde-3-Niveau und nur ueber die Krallen-UV behebbar;
  - Exit 1 des Riegel-Pakets (P2 x2, B11);
  - Krallen-Rot 76 % gegen Vorlage 46 % (die bindende Zeile wurde dafuer auf eine Region ohne Krallen umgestellt; der Nachtrag ist nicht nachgefuehrt);
  - die Nutzerablehnung der Farbmischung.
- Nach dem Orchestrator-Hinweis ("letzte Runde, danach abnehmen oder dem Nutzer vorlegen") empfehle ich: **dem Nutzer vorlegen**, mit diesen Punkten:
  - Tafel 10 und 2.3: Krallen in Runde-3-Farbe, roeter als das Video;
  - die Zweifarbigkeit (Abschnitt 7);
  - die Hauthelligkeit in Bild 2 (Dossier V3R5.5 Punkt 3).
