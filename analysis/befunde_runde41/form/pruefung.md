# Messwerkzeug fuer die Form-Kandidaten

Stand 2026-10-31. Werkzeug: `re15_port/tools/form_pruefung.py`. Es baut keine Figur, es prueft jeden Kandidaten gleich.
Vorrang hat die Lesung des Nutzers, Abschnitt 14. Wo die Abschnitte 1-13 anderes sagen, gilt Abschnitt 14.
Pixel: Originalbild 640x480. Sicherheit: sicher / wahrscheinlich / offen.

⛔ Kein RE-Wert. Das Werkzeug misst Vertraeglichkeit mit zwei Videostandbildern. Nichts davon gehoert in die Engine.

## 1. Aufrufe

Alle aus dem Wurzelverzeichnis des Arbeitsbaums `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form`. Der Lauf eines Kandidaten dauert rund 7 s.

```bash
# Einmalig, liegt fertig vor. Baut ziel_bild1.png, ziel_bild2.png und pruefung/*. Nur noetig, wenn formblatt.json sich aendert.
python re15_port/tools/form_pruefung.py ziele

# Einmalig, liegt fertig vor. Baut ziel2_bild1.png, ziel2_bild2.png. Nur noetig, wenn nutzer_teile oder ziel_bild*.png sich aendern. --logo-neu misst die Logo-Maske neu.
python re15_port/tools/form_pruefung.py ziele2

# Ein Kandidat
python re15_port/tools/form_pruefung.py pruefe X.json --aus analysis/befunde_runde41/form/pruefung/X

# Nur die Teile des Nutzers, rund 3 s
python re15_port/tools/form_pruefung.py teile X.json --aus DIR

# Mehrere nebeneinander, eine Zeile je Kandidat
python re15_port/tools/form_pruefung.py vergleiche DIR1 DIR2 DIR3

# Probe des Masses selbst, Ausgabe liegt in pruefung/selbstprobe.txt
python re15_port/tools/form_pruefung.py selbstprobe

# Probe der Teile-Messung mit kleinem Dummy, Ausgabe liegt in pruefung/teile/selbstprobe.txt
python re15_port/tools/form_pruefung.py teileprobe

# Pruefmodell aus formblatt.json als EMD + TIM + JSON
python re15_port/tools/form_pruefung.py dummy --aus DIR
python re15_port/tools/form_pruefung.py dummy --aus DIR --gebacken
```

Unter Git-Bash vorher `export PYTHONIOENCODING=utf-8`, die Warnungen von `FE.pruefe` enthalten sonst Zeichen, die cp1252 nicht kennt.

### Eingabe X.json

Pfade relativ zur Datei oder absolut.

```json
{"name": "A", "emd": "A.EMD", "tim": "A.TIM",
 "bild1": {"lage": [-18514, 0, -18726], "gier": 97},
 "bild2": {"lage": [2625, 0, -23625], "gier": 3129}}
```

Optional: je Bild `clip` und `bild` (Vorgabe Clip 0 fuer Bild 1, Clip 1 fuer Bild 2, Bild 0), oben `ruhe` `{"clip": 2, "bild": 0}` fuer das Rundumblatt, `licht` `"mess"` (Vorgabe) oder `"disc"`.
Neu: `teile` ordnet Knochen den Teilen des Nutzers zu, Abschnitt 14.1. Ohne den Eintrag laeuft alles andere, der Bericht meldet dann `Teile des Nutzers: NICHT geprueft`.
Lage = Aktorlage (x, y, z), Gier 0-4095 wie im Port. Die Werte oben sind die Probe aus FORMBLATT 3.1 und 3.2 mit Ende F vorn, sie sind keine Messung der Weltlage.

### Ausgabe in DIR

| Datei | Aufgabenpunkt | Inhalt |
|---|---|---|
| `bericht.txt`, `bericht.json` | a, b, c, d, g | alle Zahlen; die JSON ist fuer Skripte |
| `blatt_bild1.png`, `blatt_bild2.png` | e | Original, Render sauber, Render durch `video_filter`, Differenz, Umriss, Kennzahlen, je 640x480 |
| `ausschnitt_bild1.png`, `ausschnitt_bild2.png` | e | dieselben Tafeln 3-fach um die Figur, Pruefpunkte mit Nummer wie in `bericht.txt`, dazu Albedo |
| `rundum.png` | f | vorn, hinten, links, rechts, oben, unten, zweimal 3/4, Spielart; dazu Spielperspektive Bild 1 und Bild 2, je viermal rundum (14.7) |
| `rundum_hell.png` | f | dasselbe, Figur mit Gamma 0,5 aufgehellt. Die Textur ist fast schwarz, die Form liest sich sonst schlecht |
| `teile_bild1.png`, `teile_bild2.png` | Teile | Original mit Nutzerumrissen neben Render mit Teil-Farben, nur mit `teile`, Abschnitt 14.2 |

### Lesen

- `Abstand` in Zeile und Spalte: + = rechts oder unten vom Soll. Bei `kontur`: + = Soll liegt ausserhalb des Renders, Render zu klein; - = Render zu gross.
- Grau heisst: Das Bild entscheidet nicht. Es heisst nicht: dort ist Figur erlaubt und gut. Wer dort etwas setzt, setzt es frei (FORMBLATT Abschnitt 5).
- `zaehlt nicht` = reine Anzeige: gelesene Kante Bild 2, Z6b, Z6c, dazu die stillgelegten Punkte aus 14.5, jeweils mit Grund.
- Die Spalte `Y Video/Render` bei der Zeichnung ist Anzeige, kein Kriterium, siehe Abschnitt 6.
- IoU allein genuegt nicht. 6 px Versatz der ganzen Figur kosten nur 0,03 IoU, aber 17 von 31 Punkten (Abschnitt 8).

## 2. Zielmasken

`pruefe` misst gegen `ziel2_bild*.png` (14.4), dieser Abschnitt beschreibt deren Grundlage `ziel_bild*.png`.

`ziel_bild1.png`, `ziel_bild2.png`: 255 sicher Figur, 128 offen, 0 sicher nicht. Beleg zum Ansehen: `pruefung/ziel_bild1_auf_original.png`, `pruefung/ziel_bild2_auf_original.png`.
Zuordnung der drei Formblatt-Stufen: offen wird grau. Sicher und wahrscheinlich werden weiss oder schwarz, je nach Richtung der Aussage. So verlangt es auch der Auftrag fuer die Zone SCH, die nur "nicht Figur: wahrscheinlich" ist.

| | weiss | grau | schwarz |
|---|---|---|---|
| Bild 1 | 22931 | 35942 | 248327 |
| Bild 2 | 23653 | 66611 | 216936 |

### 2.1 Bild 1

Grundlage ist `umriss_bild1` aus formblatt.json, nicht mehr `maske_bild1.png`. Die alte Maske schloss neben SCH auch weissen Boden (290-320,405-430) und X als Figur ein.

| Stelle | Ergebnis | Quelle |
|---|---|---|
| L1, L2, SCH | schwarz | Abschnitt 7, W7 |
| B5, C5, X | grau, Polygon und Bereich | 1.4 offen |
| Sohlen | grau: x<290 ab y=401, x>=290 ab y=415, Scheiben r=16 um K2, K3, K4, dazu (195-253,400-436) | 1.2 letzte harte Stellen |
| C4 Aussenkante | grau, Ring +-12 px um das Polygon, ohne die harte linke Kante | A5, W7 |
| Logo | grau (0-253,436-480) | Abschnitt 5 |
| Oberkante x 306-345 | grau y 188-206 | W16 |
| Dunkel auf dunkel | grau (250-276,150-225), Saum 10 px fuer x 250-300 | 1.4, Abschnitt 5 |
| Schulter unten rechts | grau (412-436,278-302) | A10 |
| Umriss sonst | grauer Saum nach gemessener Kantenbreite, 3,0-13,0 px, Median 5,0 | Abschnitt 4 hier |

### 2.2 Bild 2

Weiss ist Mindestausdehnung. Bild 2 hat keinen geschlossenen Umriss, alles ausser den Boden-Zonen und dem frei sichtbaren Raum ist grau.

| Stelle | Ergebnis | Quelle |
|---|---|---|
| Rumpf | weiss bis zur gelesenen und belegten Kante, rechts bis x=413 fuer y 225-262, links bis x=262 | 1.4, W9, W8 |
| U, Band unter dem Haken | weiss: V1 (222-298), V2 (232-285), Spitze (240,372) | 1.4, W15 |
| G, P, Haken | weiss, Polygone | 1.4, 2.2 |
| Unterkante rechts | weiss x 340-364 bis y=345, darunter grau bis y=381 | W10 |
| Rechts unten | Grenze aus der Verdeckung: (413,262)-(384,266)-(382,284)-(366,298) | Beleg-Karte, siehe unten |
| Vor der Tuer | grau, ueberall wo der Raum zu dunkel ist | A2, Abschnitt 5 |
| Rechts neben G, (300-345,300-320) | grau | Abschnitt 5 |
| F, Y, Bereich unter G | grau | 1.4, W13 |
| Tuerplatte, Boden (278-450,395-475), BO, Fuge | schwarz | 1.4, A13, Abschnitt 7 |

Beleg-Karte: Video-Rot gegen gefilterten Raum, Raum mindestens R 60. Bei x=350-390 liegt das Video bis y rund 284 weit unter dem Raum, z.B. (360,242) 46 gegen 187, und das Rohr ist bei x=408 fuer y 230-262 verdeckt, 64-95 gegen 129-189.
Die Karte entscheidet nur auf der Wand. Am Boden unter y rund 330 ist sie kein Beleg (W13, Gegenprobe). Bei (300-345,300-320) truegt sie: Der gefilterte Raum enthaelt Streulicht vom Lichtbalken, den die Figur in Wahrheit verdeckt. Dort gilt deshalb das Formblatt.

### 2.3 Abweichungen vom Formblatt

Jede davon ist eine Entscheidung dieses Werkzeugs. Bitte pruefen, wenn das Formblatt weitergefuehrt wird.

| Nr | Stelle | Formblatt | Hier | Grund |
|---|---|---|---|---|
| P1 | Feuer Bild 2 | "sicher nicht Figur" | grau | Die Pixel sind nicht Figur, aber Feuer kann vor ihr stehen. W9 und 6B sagen das selbst ("Bein FR nicht pruefbar"). Schwarz haette Kandidaten fuer etwas bestraft, das niemand sehen kann |
| P2 | L2 | Polygon endet bei y=320 | Spitze (293,313) ergaenzt | A20 nennt sie. Ohne Spitze gibt es die Kante von Bein NL bei y=318 aus Abschnitt 7 gar nicht |
| P3 | Umriss bei x=280 | Polygon y=195 | Stuetzpunkt (280,191) | 1.4 und Abschnitt 7; Bildmessung Versatz -0,2 px |
| P4 | Umriss Bild 1 | Polygon | 282 Stellen auf die gemessene Kante gezogen, Median 1,7 px, groesster Weg 4,6 px | An fuenf Punkten lag das Polygon 3-5 px neben der Kante, die Abschnitt 7 und die Bildmessung auf 1,5 px gleich nennen. Ein Modell genau auf Abschnitt 7 haette (c) verfehlt. Gezogen wird nur mit Luma-Hub ab 36, innen dunkel, aussen hell, hoechstens 6 px |
| P5 | V6 | Teil von "Figur deckt" | Kantenpunkt bei (411,206) | Die Kante liegt in der Strecke: R 194 bei y=204, 117 bei y=208. Vollabdeckung waere falsch |
| P6 | Kantenbreite | Gegenprobe 3.1: Pfeiler 7, Saeule 8, Kugel 6 | 5,1, 5,0, 4,6 | Abschnitt 4 hier |

## 3. Hintergruende

**Bild 1:** `pruefung/hintergrund_bild1.png`. `video_umkehr(bild1)`, Figur plus 7 px ausgemalt (Telea auf halber Aufloesung, weichgezeichnet). Unter der Figur steht ein heller Fleck, der wird vom Render verdeckt und zaehlt nur als Streulichtquelle.
Durch den Filter gegen das Original: fern MAE 5,7, im Ring 8-28 px direkt um die Figur MAE 5,7, mittlere Abweichung -0,8. Das Streulicht stimmt also auch neben der Figur (sicher). Luecken eines Kandidaten zeigen den Fleck, der dort nicht echt ist.

**Bild 2:** `pruefung/hintergrund_bild2.png`. ROOM509.BSS Cut 11 aus `probe_bg_dump`, pixelgleich mit `lesehilfen/farbe_port_ROOM50911.png`, Abweichung 0. Lage ueber dasselbe B wie die Figur, dann `video_filter(entzerren=False)`. Der Weg ueber B und der Weg ueber H_GEO geben gleich gute Bilder: MAE aussen 18,7 gegen 18,5.
Feuer und Frau fehlen, wie im Auftrag. An der schwarzen Tuer bleibt Rest +2,5 / +2,0 / +2,3, das fehlende Feuerstreulicht ist dort klein. Die Differenz-Tafel zeigt Feuer und Frau deshalb immer als Abweichung, das ist kein Fehler des Kandidaten.

## 4. Grenzen der Pruefpunkte

Fuer jeden Punkt aus Abschnitt 7 einzeln, in `pruefung/grenzen.json`:

    Grenze = Wurzel( (Kante 10-90 / 2)^2 + 1,25^2 + (Abstand Soll zu gemessener 50-%-Kante)^2 )

| Glied | Bedeutung | Beleg |
|---|---|---|
| Kante / 2 | innerhalb der halben Flanke kann das Video die Lage nicht trennen | je Punkt im Original gemessen |
| 1,25 px | Ecken liegen ganzzahlig im Port: Abschneiden plus Phase -0,375..+0,625 Port-Pixel, Massstab 2 | Buehne gegen `form_kamera`: bis 2,08 px schraeg |
| Versatz | Ablesefehler des Solls: Ein Modell auf der echten Kante und eines auf dem Soll bestehen beide | je Punkt gemessen |

Messung: Querprofil, staerkste Flanke nahe der Mitte, Pegel = Extrem je Seite der Flanke in +-18 px. Normale: Spalte und Zeile fest, `kontur` aus dem Bildgradienten. Von Hand gesetzte Normalen taeuschten am Rundling -6,8 px vor.
Kanal: Luma zuerst, wenn es Hub ab 36 hat und die Flanke hoechstens 9 px vom Soll liegt, denn Farbgrenzen sind im Video nur auf 5-10 px genau (`lesung_zeichnung.json`). Sonst der Kanal mit dem groessten nach Naehe gewichteten Hub, in Bild 2 auch das Restbild Video minus Raum.
Gegenprobe gegen reine Hub-Wahl, nachgemessen mit der fertigen Kantenmessung: An 8 von 47 Punkten aendert sich die Grenze um 1,5 px oder mehr. Beispiele: Bild 2 (394,187) faengt rein nach Hub die Raumstufe, Versatz +13,0 im Rotkanal gegen 0,0 im Restbild. Bein FR rechts y=324 misst in Blau 11,3 px breit, in Luma 4,9.
Die Regel macht nicht jede Grenze enger: Schulterkuppe 11,6 statt 8,5, Bild-2-Kante 4 9,5 statt 6,6. Luma ist dort die ehrlichere, weichere Messung.

Ergebnis: 2,5 bis 13,3 px. Median Bild 1 3,6, Bild 2 6,4. Keine Stelle ist ohne messbare Kante.

| Punkt | Grenze | Breite | Versatz | Kanal | Lesart |
|---|---|---|---|---|---|
| Flanke 2 (366,215) | 2,6 | 4,5 | -0,4 | Y | hart |
| Bein FR links y=344 | 3,2 | 5,8 | 0,4 | Y | hart |
| Stange | 3,2 | 5,8 | -0,2 | Y | hart |
| Sohle Bein FR | 6,8 | 12,6 | 2,2 | Y | weich, passt zur Gegenprobe (13 px) |
| Rundling (306,405) | 7,5 | 6,0 | 6,8 | Y | Soll liegt rund 7 px im Dunkeln neben der Kante |
| Schulterkuppe | 11,6 | 15,4 | 8,6 | Y | Lampenschein laeuft weich in die Figur aus |
| Kappe Basis | 13,3 | 20,4 | -8,5 | B | Unterkante weich, Bandkante um y=290 |
| Bild 2 Kante 1 (354,161) | 6,4 | 11,2 | 2,8 | Y | Plattenkante |
| Bild 2 Kante 5 (390,181) | 11,0 | 13,9 | 8,5 | R | Stufe im Raum, siehe Gegenprobe 3.3 |
| Bild 2 Kante 7 (411,206) | 2,5 | 4,2 | -0,5 | Y | Rohrende, einzige harte Stelle |

Abgesehen von diesen Stellen liegen die Soll-Werte aus Abschnitt 7 bis auf 1,5 px auf der gemessenen Kante (sicher). Abschnitt 7 traegt.

Widerspruch zwischen den Vormessungen (W nicht im Formblatt): Gegenprobe 3.1 nennt fuer harte Kanten 6-8 px. `farbe.md` (Median 4,5-5,1) und `lesung_zeichnung.json` (hart <= 5) stimmen mit dieser Messung (4,6-5,5) ueberein. Hart und weich trennen alle drei gleich, weiche Stellen hier 18-20 gegen 20-25.

| Stelle | Gegenprobe | hier |
|---|---|---|
| Pfeiler (415,331) | 7 | 5,1 |
| Saeule (228,330) | 8 | 5,0 |
| Kugel (203,390) | 6 | 4,6 |
| Oberkante C5 (306,405) | 7 | 5,5 |
| C4 rechts (370,402) | 21 | 18,2 |
| C4 rechts (367,412) | 25 | 20,4 |
| C4 unten (354,427) | 9 | 7,2 |
| C4 unten (345,433) | 20 | 18,8 |

## 5. Flaechen

Boden frei (L1, L2, SCH, BO, Fuge): Pruefflaeche = Zone minus grauer Saum. Der Saum ist schon die Grenze aus der Unschaerfe, bestanden heisst also 0 Treffer. `Eindringen` nennt die Tiefe.

Verdeckung (V1 bis V5): Render muss auf der Linie liegen, die Enden zaehlen erst innerhalb ihrer Kantengrenze, hoechstens 8 px. V5 zaehlt seit Abschnitt 14: Flaeche Y ist die Hand. V6 ist Kantenpunkt (P5).
Zusatz aus 1.4 und Abschnitt 7: Rand rechts mindestens x=413 bei y 225-262 (3 px Spiel). Der Haken ist stillgelegt (14.5).

## 6. Zeichnung

Sollflaechen sind gemessene Polygone: `lesung_zeichnung.json` (A.., B..), `lesung_gesamtbau.json`, G und P aus formblatt.json. Zuordnung und Material nach FORMBLATT 2.2.

| Stelle | Quelle | Material | Regel |
|---|---|---|---|
| Z1 | A01 | rosa_stelle | Spanne der Palette + 8 Stufen (5-Bit-CLUT) |
| Z2, Z4, Z6a | A02, A04, A14 | helle_baender | wie oben |
| Z6b, Z6c | A15, A16 | helle_baender | zaehlt nicht, offen |
| Z3, P | A03, Polygon P | weisskerne | nur Y mindestens wie Band, mehr ist nicht belegt |
| Z5 | dreieck_gelenk_A | helle_baender | wie oben |
| Z7 | A20, A22, A23 | feine_linien | wie oben |
| Z8 | A09, A08, A07 | feine_linien | wie oben |
| Z9 | schraegstrich_beule | (85,80,60) | +-30, kein Fehlerband im Blatt |
| Q1, Q3, Q5 | B01+B02, B06, innenlinie_rechts_unten | feine_linien | wie oben |
| Q2 | scheitel_sichel | rotbraun_mittelton | wie oben |
| Q4 | B07 | helle_baender | wie oben |
| G | Polygon G | untere_streifen | Y ab Rotbraun, hoechstens Obergrenze + 8 |

Gemessen wird am **Albedo-Render**, also Textur ohne Licht. Licht darf das Urteil nicht tragen, die Beta-Beleuchtung ist nicht belegt.
Bestanden: mindestens 0,6 der Sollflaeche liegt innerhalb 3 px von passendem Material. Die 3 px sind die halbe Kante; ohne sie wuerde jede duenne Linie an 2 px Lagefehler scheitern.

Die 0,6 ist eine **Festlegung dieses Werkzeugs, keine Bildaussage**. Sie liegt in einer Luecke: Gebackener Dummy, gezaehlte Anteile 0,524, 0,535, 0,738, dann ab 0,933. Das Ergebnis 14/16 bleibt fuer jede Schwelle von 0,54 bis 0,73 gleich. Einfarbiger Dummy: 16-mal 0,00.

Verbote aus 2.3 zusaetzlich: N1 Rumpfinneres Bild 2 ohne Zeichnung, N2 Bein FR unter Z5 ohne Band, je hoechstens 5 % helle Pixel (Festlegung).

### Licht

Blaetter nutzen `licht: "mess"`: Disc-Licht mal fester Faktor k aus `pruefung/licht.json`. Die Palette ist Schirm durch gemessenen Faktor (`farbe.md` 5), reines Disc-Licht zeichnet dunkler. Ohne k saehe eine palettentreue Textur im Vergleich zu dunkel aus.

| | Disc-Faktor am Pruefmodell | gemessen | k |
|---|---|---|---|
| Bild 1, ROOM5080 Cut 2 | 0,459 / 0,453 / 0,453 | 0,487 / 0,432 / 0,374 | 1,061 / 0,953 / 0,825 |
| Bild 2, ROOM5090 Cut 11 | 0,321 / 0,306 / 0,306 | 0,385 / 0,342 / 0,307 | 1,198 / 1,119 / 1,004 |

k ist fuer alle Kandidaten gleich, nie je Kandidat. Bild 1 hat keinen Schnitt; Cut 2 liegt ihm am naechsten, und L0 von Cut 1 und 2 ist gleich.
`licht: "disc"` zeigt das reine Disc-Licht. Das echte Spiel zeigt nur `form_spiel.sh`. Umriss, Punkte, Flaechen und Zeichnung haengen nicht am Licht.

Warnung fuer den Texturbau: L0 steht in Bild 1 fast senkrecht ueber der Figur. Eine senkrechte, zur Kamera zeigende Flaeche bekommt daher wenig Licht. Am gebackenen Dummy steht Z2 mit Y 57 gegen 130 im Video, Z4 59 gegen 141, obwohl das Material stimmt. Oben liegende Flaechen laufen dagegen weiss zu. Das ist eine Messung am Ellipsoid, keine Vorgabe fuer die Normalen.

## 7. Kennzahlen

Flaechen (Dreiecke + Vierecke), Knochen, Meshes, Textur, CLUT-Zeilen, Clips, `FE.pruefe()`. `Rahmen:` prueft die Auftragsgrenzen: rund 1900 Flaechen, 32 Knochen, 384x256, Clip 0, 1, 2 vorhanden, Clip 0 und 1 je ein Bild, tiefster Punkt je Pose auf y=0 (+-2).
Das Werkzeug kennt keinen Knochenbau. Ob Knochen 10 der Rumpf ist, kann es nicht sehen; es meldet nur, was `FE.pruefe` an fehlenden Teilen findet.

## 8. Beweise

Volle Ausgabe: `pruefung/selbstprobe.txt`. Die Tabellen hier sind der Stand vor Abschnitt 14 mit dem alten Soll-Umriss. Die Datei ist neu gerechnet: Bild 1 29/29 Punkte, 3/3 Flaechen, Bild 2 7/7 Punkte, 8/9 Flaechen. Das eine Nein ist V5, der alte Umriss hat keine Hand.

**Soll-Umriss als Render.** Was hier fiele, waere ein Widerspruch im Mass selbst.

| | Punkte | Flaechen | IoU | Schwarz getroffen |
|---|---|---|---|---|
| Bild 1 | 31/31 | 3/3 | 1,000 | 0 |
| Bild 2 | 7/7 | 9/9 | 1,000 | 0 |

**Versatzprobe.** Ganze Figur verschoben. Zeigt, dass die Grenzen anschlagen.

| Versatz | Bild 1 Punkte | Bild 1 IoU | Bild 2 Punkte | Bild 2 IoU |
|---|---|---|---|---|
| +3,0 | 25/31 | 1,000 | 7/7 | 1,000 |
| +6,0 | 14/31 | 0,971 | 6/7 | 0,971 |
| +10,0 | 10/31 | 0,898 | 4/7 | 0,929 |
| 0,+6 | 23/31 | 0,991 | 5/7 | 0,982 |
| 0,+10 | 16/31 | 0,964 | 3/7 | 0,953 |

Bild 2 misst grober, seine Kanten sind weich. Mehr gibt das Bild nicht her.

**Kantenlage.** Soll-Umriss in Grundhaut durch den Filter, gleiche Messung auf beiden Seiten: In Bild 1 liegen die 16 harten Kanten im Median 0,8 px von der Original-Kante, groesster Wert 5,7. Der Filter selbst schiebt eine Kante im Median 0,8 px zur hellen Seite, Spanne -0,2 bis +2,5. Das liegt in der Grenze.
Bild 2: Nur eine Stelle bleibt uebrig. Die Zahlen -6,0 und +8,2 sind ein Einzelwert, keine Aussage. Die Kantenlage in Bild 2 ist damit **nicht bewiesen**.

**Dummy.** Pruefmodell aus formblatt.json, 12 Ellipsoide, 12 Knochen, 1680 Flaechen, Textur 384x240. Lage aus FORMBLATT 3.1 und 3.2.

| Lauf | IoU 1 | Weiss 1 | Punkte 1 | Flaechen 1 | Zeichnung 1 | IoU 2 | Weiss 2 | Punkte 2 | Flaechen 2 | Zeichnung 2 |
|---|---|---|---|---|---|---|---|---|---|---|
| einfarbig | 0,958 | 0,977 | 22/33 | 0/3 | 0/9 | 0,772 | 0,909 | 6/7 | 4/9 | 0/7 |
| gebacken | 0,958 | 0,977 | 24/33 | 0/3 | 8/9 | 0,772 | 0,909 | 6/7 | 4/9 | 6/7 |

Der gebackene Dummy traegt die Sollflaechen, aus beiden Kameras auf die Ellipsoide projiziert. Er zeigt den Trefferfall, der einfarbige den Fehlfall. Die zwei Fehlschlaege dort sind echt: An Z8 steht die Figur zu 0,45, an Q4 zu 0,37 an der Sollstelle.
Die IoU 0,772 in Bild 2 liegt im Bereich, den das Formblatt fuer das Pruefmodell nennt (0,73-0,90).

**Knochenbau.** Laeuft ohne Aenderung mit 3 Knochen (FORM_PROBE, 2 Clips), 12 (Dummy), 22 (EM23) und 27 (BAHNSTEIG_G4). Die Fremdmodelle sind reine Robustheitsprobe, keine Formvorlage, ihre Blaetter wurden nach der Messung geloescht. Rahmen meldet je das Richtige, etwa `nur 2 Clips` und `Textur 512x256`. Sie fallen an den Punkten erwartungsgemaess durch, 0-2 von 33.

**Wiederholung.** Zwei Laeufe desselben Kandidaten: `bericht.json` und Blatt byte-gleich.

## 9. Beim Bau gefundene Fehler

Alle behoben, hier zur Einordnung frueherer Zwischenstaende.

| Fehler | Wirkung | Wie gefunden |
|---|---|---|
| 50-%-Suche lief bei fallenden Flanken in die falsche Richtung | Versatz an jeder fallenden Kante falsch, Stange -11,8 statt -0,2 | Vergleich mit der Vormessung |
| Pegel endeten am Gradientenende | weiche Kanten als hart gemessen, 7 statt 20-25 px | Referenzwerte Gegenprobe 3.1 |
| Kanalwahl nur nach Hub | Bild 2 (394,187) Versatz +13,0 an der Raumstufe, Bein FR rechts 11,3 statt 4,9 px breit | Durchsicht der Grenzen. Die zeitweise -14 px an der Stange kamen dagegen allein vom ersten Fehler dieser Tabelle |
| `boden_nachfuehren` am Dummy | Figur 118 Einheiten zu hoch, alle Stellen rund 10 px daneben | Spalte Lage, Scheitel einheitlich -8,5 |
| Rundum links und rechts vertauscht | Tafel "links" zeigte -Z | gebackene Zeichnung auf der falschen Seite |
| Rasterglied 1,0 statt 1,25 px | Grenzen minimal zu eng | Buehne gegen `form_kamera` |
| Polygon statt gemessene Kante | Soll-Umriss fiel selbst an 5 von 31 Punkten durch | Selbstprobe, siehe P2 bis P4 |

## 10. Was im Bild noch abweicht

Am gebackenen Dummy, Blaetter unter `pruefung/dummy/lauf_gebacken/`:

- Bild 1: Ellipsoide treffen die Grossform, nicht den Bau. Die Kuppel ist zu rund und zu glatt, ihr rechter Hang steht bis 9,7 px ueber (Flanke 1), die Beine sind Ketten aus Eiern. Boden getroffen: L1 14 px, L2 40 px, SCH 1 px.
- Bild 1: Durch die Luecken leuchtet der ausgemalte Fleck. Am echten Kandidaten sollen dort L1 und L2 frei sein, sonst Figur.
- Bild 2: Bein FR steht rechts im Feuer, der Fuss ueber dem Boden BO. Das kommt aus der Probelage und Pose 1, die Sichtfrage bleibt offen (6B).
- Bild 2: Unten fehlt Weiss in der Untermasse, V2 und V3 sind nicht gedeckt.
- Feuer und Frau fehlen in beiden Render-Tafeln von Bild 2.

## 11. Grenzen des Werkzeugs

- Es prueft zwei Ansichten. Rueckseite, Ende F und Unterseite sieht nur das Rundumblatt, und dort urteilt ein Auge, keine Zahl.
- Kopf, Vorsprung und alle freien Stellen aus Abschnitt 5 sind nicht messbar. Bestehen heisst dort nur: nichts widerspricht.
- Bild-1-Weltlage und Kamera sind geschaetzt (`kamera.md`). Lage und Gier sind daher Freiheitsgrade des Kandidaten. Ein Kandidat kann sich also gut einpassen, ohne dass seine Lage im Spiel damit bewiesen waere.
- Die Buehne zeigt keine Vordergrundmasken und keine Effekte (`werkzeug.md` 7). Die Abnahme bleibt `form_spiel.sh`.
- `Weiss` fuer Masse C3, C4-Kern, B4 ruht auf "wahrscheinlich". Am Boden von Bild 1 heisst dunkel nicht Koerper (1.2).
- Grau in Bild 2 ist gross, 66611 Pixel. Der Massstab dort ist bewusst duenn, weil das Bild duenn ist.

## 12. Fuer die Kandidaten-Bauer

- `form_buehne.py --kamera kamera_bildN.json` scheitert mit `KeyError 'pos'`: `kamera_laden` kennt die Schluessel `auge` und `abbildung_port_nach_bild` nicht (gemessen). Dieses Werkzeug nimmt deshalb `form_pruefung.kamera(n)`. Der Lader ist nicht geaendert, das lag ausserhalb dieses Auftrags.
- `form_buehne.deckung` und die Buehne selbst sind davon nicht betroffen.
- Als Bibliothek: `import form_pruefung as P`; `P.kamera(n)`, `P.punkte_abschnitt7()`, `P.stellen()`, `P.ueberdeckung(maske, ziel)`.

## 13. Dateien

| Datei | Inhalt |
|---|---|
| `re15_port/tools/form_pruefung.py` | das Werkzeug |
| `analysis/befunde_runde41/form/ziel_bild1.png`, `ziel_bild2.png` | Zielmasken |
| `analysis/befunde_runde41/form/pruefung/ziel_bild1_auf_original.png`, `ziel_bild2_auf_original.png` | Masken auf dem Original |
| `analysis/befunde_runde41/form/pruefung/hintergrund_bild1.png`, `hintergrund_bild2.png` | Hintergruende, Spielbild in Video-Bildlage |
| `analysis/befunde_runde41/form/pruefung/grenzen.json` | Grenze je Punkt mit Breite, Versatz, Kanal, Hub |
| `analysis/befunde_runde41/form/pruefung/licht.json` | Lichteichung |
| `analysis/befunde_runde41/form/pruefung/figur_bild1.png`, `figur_bild2.png` | Soll-Umriss vor Saum, fuer die Selbstprobe |
| `analysis/befunde_runde41/form/pruefung/selbstprobe.txt` | Ausgabe der Selbstprobe |
| `analysis/befunde_runde41/form/pruefung/dummy/` | Dummy-EMD, -TIM, -JSON, Laeufe `lauf_einfarbig`, `lauf_gebacken` |
| weitere Dateien | Abschnitt 14.10 |

Nicht geaendert: `maske_bild1.png`, `maske_bild2.png`, `formblatt.json`, `FORMBLATT.md`, die uebrigen Werkzeuge. Kein Commit.

## 14. Teile (Lesung des Nutzers)

Stand 2026-10-31. Der Nutzer hat acht Teile aus den Bildern freigestellt: `nutzer_teile/`. Bild 2 zeigt die Figur von vorn (`face_and_upper_body`, `left_arm_front`, `right_arm_front`), Bild 1 von hinten (`tail`, `left_leg`, `right_leg`, `right_arm`, `part_of_Body`).
Die Lesung ist bindend. Die Umrisse legen fest, WAS WO ist, nicht jede Kante auf den Pixel.

### 14.1 Eingabe

```json
"teile": {"tail": [12, 13, 14], "left_leg": [3, 4], "right_leg": [5, 6], "right_arm": [7, 8], "part_of_Body": [0, 1],
          "face_and_upper_body": [0, 1, 2], "left_arm_front": [9, 10], "right_arm_front": [7, 8]}
```

- Derselbe Knochen darf in einem Teil von Bild 1 und einem von Bild 2 stehen. `right_arm` und `right_arm_front` sind derselbe Arm, `part_of_Body` und `face_and_upper_body` teilen sich den Rumpf.
- Innerhalb eines Bildes zaehlt ein Knochen nur fuer ein Teil, sonst warnt der Bericht.
- Namen des jeweils anderen Bildes bleiben sichtbar: Ein Bein heisst in Bild 2 weiter `left_leg`, hat dort aber keine Maske. Weitere Namen (`kopf`) sind erlaubt und zaehlen als fremde Teile. Knochen ohne Namen sind `Rest`.
- Fehlende Namen und unbekannte Knochen meldet der Bericht als Warnung.

### 14.2 Ausgabe

| Datei | Inhalt |
|---|---|
| `teile_bild1.png`, `teile_bild2.png` | links Original mit Nutzerumrissen, rechts Render ohne Filter mit denselben Umrissen. Je Knochen eine Farbe im Ton des Teils, Knochennummern im Bild, darunter die Kennzahlen. Grau umrandet: Logo-Schrift |
| `bericht.txt`, `bericht.json` | Block Teile am Ende, Schluessel `teile` |
| `teile.txt`, `teile.json` | nur bei Befehl `teile` |
| `vergleiche` | zweite Zeile je Kandidat: IoU je Teil |

Farben wie in `teilekarte_bild*.png`. Der Ausschnitt umfasst alle Masken und den Render, ein Teil weit daneben bleibt also sichtbar.

### 14.3 Logo und Handschnitt

**Logo-Schrift.** Gemessen statt geschwellt. Das Logo steht ueber allen Bildern still, das Bild darunter wechselt. Zeitstreuung je Pixel ueber jedes fuenfte Bild des Videos (835 Bilder, Quelle `Version_80%/Biohazard 1.5 Final Build.mp4`, liegt nur im Hauptbaum). Otsu trennt bei 36,0: Schrift bis rund 20, Bild ab rund 40.
Ergebnis `pruefung/logo_maske.png`: 10069 Pixel, x 2-255, y 434-479, Beleg `pruefung/logo_beleg.png`. Die roten Luecken zwischen den Buchstaben bleiben Bild. Gegenprobe: Bild 1 gegen Bild 2 mittlerer Y-Abstand in der Schrift 6,3, in der Zone daneben 44,8.
Der Auftrag nennt die Buchstaben im rechten Arm. Die Regel gilt hier fuer jedes Teil, denn die Schrift liegt in Bild 1 genauso ueber dem Schwanz. Schrift in den Masken: `right_arm_front` 2371 px, `tail` 1278, `left_leg` 92.

**Handschnitt.** Abstand der Nutzerumrisse zu den 36 gemessenen Kantenpunkten: Median 1,6 px, 80 % 3,5, 90 % 5,9, groesster 20,1. Die Umrisse und Abschnitt 7 bestaetigen sich gegenseitig (sicher). Ueber 8 px liegen nur `b1.schulterkuppe` 11,5 (weiche Kante, Grenze 11,6) und `b1.rand_rechts` 20,1 (14.9).
Daraus der Saum `TEIL_SAUM_PX` = 3 px fuer Kern und +Saum. Gleich der halben Kante aus Abschnitt 6.

### 14.4 Gesamtziele

`ziel2_bild1.png`, `ziel2_bild2.png`, `pruefe` nimmt sie automatisch. Beleg `pruefung/ziel2_bild*_auf_original.png`, Zahlen `pruefung/ziel2_stand.json`.

| Regel | Bedeutung |
|---|---|
| Weiss | alle Teile des Nutzers, wie geschnitten |
| Schwarz | Schwarz aus `ziel_bild*.png`, dazu der Sockel (FORMBLATT 1.4 sicher, im alten Ziel nur wegen A10 grau) |
| Grau | alles andere. Auch altes Weiss ausserhalb der Teile: Bild 1 1433 px, Bild 2 829 px. Die Teile sind keine Vollstaendigkeitsaussage |
| Logo-Schrift | grau, was darunter liegt, zeigt das Bild nicht |
| Weiss gegen Schwarz | Weiss gewinnt, Vorrang der Lesung |

| | weiss | grau | schwarz |
|---|---|---|---|
| Bild 1 | 33511 | 25452 | 248237 |
| Bild 2 | 45465 | 47706 | 214029 |

Die Boden-frei-Pruefung nimmt ihre Flaeche aus diesem Ziel. Wer dem Nutzerumriss folgt, wird dort also nie bestraft. Solche Pixel:

| Teil | Flaeche | davon Logo | auf altem Schwarz | Stelle |
|---|---|---|---|---|
| `right_arm_front` | 12314 | 2371 | 2069 | Hand um das alte Grau herum, A13 49 px, Fuge 2 px |
| `left_arm_front` | 5607 | 0 | 679 | Klauen, A13 354 px, BO 74 px |
| `left_leg` | 4345 | 92 | 86 | Rand |
| `right_leg` | 6896 | 0 | 73 | L1 7 px, L2 136 px, davon 4 px im Kern |
| `tail` | 13644 | 1278 | 10 | Rand |
| `face_and_upper_body` | 29990 | 0 | 6 | Rand |
| `part_of_Body` | 7293 | 0 | 4 | Rand |
| `right_arm` | 4299 | 0 | 0 | L1 78 px im alten Saum |

Sichere Zonen in ziel2 (Pixel): L1 969 von 1586 schwarz, L2 629 von 1204, SCH 679 von 1120, Sockel 589 von 589, BO 841 von 915, Fuge 883 von 885, A13 13610 von 14013, Tuerplatte 2289 von 2806, Kisten 18600 von 18600. Der Rest ist alter Saum oder Weiss der Teile. Feuer bleibt grau (P1), dort stehen die Arme.

### 14.5 Stillgelegt und geblieben

Stillgelegte Stellen bleiben im Bericht mit Grund, sie zaehlen nicht. Ids bleiben gleich, die Texte tragen die neuen Namen.

| Stelle | Stand | Grund |
|---|---|---|
| `b1.rundling_1`, `_2` | still | liegen 8 und 18 px innen in `right_leg`. C4 und C5 waren getrennt gelesen, der Nutzer schneidet beide in ein Teil |
| `HAKEN` | still | Haken ist ueberholte Anatomie, die Stelle gehoert zu `face_and_upper_body` |
| `b2.gelesen_1` bis `_7` | zaehlten nie, jetzt mit Grund | gelesene Kuppelkante, der Nutzerumriss liegt 5-28 px weiter aussen |
| V5 | zaehlt jetzt | Flaeche Y ist die Hand, W13 ist entschieden. Linie liegt zu 1,00 in `right_arm_front` |

Es bleiben unveraendert: alle uebrigen Kanten aus Abschnitt 7, L1, L2, SCH, BO, Fuge, V1 bis V4, V6, Rand 413, Z1-Z9, Q1-Q5, G, P, N1, N2. Sie sind Bildmessungen, nur ihre Deutung war falsch.
`Kappe` heisst jetzt Z5, `Scheitel` Oberkante `tail`, `Bein FR` `right_arm`, `Bein NR` `right_leg`, `Bein NL` links `left_leg` und rechts `tail`.
Zaehlwerte: Bild 1 31 Punkte (vorher 33), 3 Flaechen. Bild 2 7 Punkte, 9 Flaechen (V5 rein, Haken raus).

Zeichnung in den Teilen, Anteil der Stelle in der Maske:

| Teil | Stellen |
|---|---|
| `part_of_Body` | Z1, Z2, Z3 und Z4 (Z3, Z4 auch in `right_leg`, Schnitte ueberlappen) |
| `right_leg` | Z6a, Z6b, Z6c |
| `right_arm` | Z5 0,94 |
| `left_leg`, `tail` | Z8 0,30 und 0,70, Z7 0,37 in `left_leg` |
| `face_and_upper_body` | Q1 bis Q5 |
| `right_arm_front` | G 0,76 |
| in keinem Teil | Z9, P (0,06), Rest von Z7 |

### 14.6 Messgroessen je Teil

M = Maske ohne Logo, V = sichtbarer Teil ohne Logo, A = Teil allein gerendert. Der Render nimmt denselben Weg wie `Kandidat.rendern`, nur mit Knochen-Kennung statt Textur. Abweichung gemessen: 0 Pixel. Durchsichtige Texel bleiben durchsichtig.

| Groesse | Rechnung |
|---|---|
| IoU | V und M gegen V oder M |
| Deckung, Kern | wie viel von M gedeckt ist, Kern = M minus 3 px Saum |
| Innen, +Saum | wie viel von V in M liegt, +Saum = M plus 3 px |
| Verdeckt, verdeckt von | 1 - sichtbar / allein, und durch welche Teile. Folgt der Tiefensortierung des Spiels, nicht einem Z-Puffer |
| in der Maske statt des Teils | andere Modell-Teile in M, je Anteil |
| Leer | M ohne jedes Modell |
| Schwerpunkt | dx, dy, Abstand. + = rechts oder unten vom Soll |
| Achse | Winkel der Hauptachsen in Grad, quer = Abstand des Schwerpunkts von der Soll-Achse in Pixeln |
| Laenge, Breite | Ist/Soll laengs und quer zur Hauptachse, 1-99 % |
| Enden | laengster Weg innerhalb der Form. Soll ohne Logo, Ist am ganzen Teil, auch verdeckt. Kennzeichen `verdeckt`, `Soll unter Logo` |
| Spitzen | Gipfel des Wegs von der Schulter, die mindestens 12 px aus ihrem Sattel ragen. Soll am rohen Schnitt, Paare nach Abstand, `fehlt` und `ueberzaehlig` sind benannt |
| Merkmal | X Schwanzspitze in `tail`, Y Hand in `right_arm_front`: Anteil vom richtigen Teil gedeckt |
| Trefferquote | je Bild: alle Treffer durch alle wertbaren Maskenpixel |

Schnitte ueberlappen sich: `part_of_Body` mit `right_leg` 1316 px, `tail` mit `right_leg` 190, `tail` mit `left_leg` 90. Dort darf jeder der beiden Teile stehen. Steht der andere, faellt das Pixel aus der Wertung (`Wertung` im Bericht), sonst koennte kein Modell auf 1,00 kommen.

Festlegungen des Werkzeugs, keine Bildaussagen: Saum 3 px, Spitze 12 px. Der Bericht zieht keine Schwelle, er misst nur.
Enden sind bei Flaechen (`face_and_upper_body`, `part_of_Body`) nur Anhalt, siehe 14.8. Soll-Enden und -Spitzen stehen in `pruefung/teile/selbstprobe.txt`, etwa Klauen (468,417), (439,418), Klauenhand (145,353), (197,476), (222,475), (274,443), Schwanzende (251,477).

### 14.7 Rundumblatt

Zwei neue Reihen in `rundum.png` und `rundum_hell.png`: Kamera von Bild 1 (Augenhoehe 2180) und Bild 2 (Augenhoehe 3256), Figur viermal um je 90 Grad gedreht. Die erste Tafel ist die Sicht aus dem Bild.
Spielfenster ohne Video-Abbildung, Pose und Licht des jeweiligen Bildes, Drehachse senkrecht durch die Mitte der Figur, Massstab gleich in der Reihe. Geprueft am Kandidaten A, seine Nullgrad-Tafeln zeigen die Sicht aus Bild 1 und Bild 2.
Beim Probe-Dummy sind die Reihen unbrauchbar: Er besteht aus zwei Inseln 20000 auseinander, die Drehung schwenkt sie aus dem Blick. Das ist Eigenart des Dummys.

### 14.8 Selbstprobe

Volle Ausgabe `pruefung/teile/selbstprobe.txt`, Dummy unter `pruefung/teile/dummy/`: 17 Knochen, 768 Flaechen, je Teil zwei Ellipsoide auf der Hauptachse der Maske, in frei gewaehlter Tiefe. Lauf rund 25 s.

| Probe | Soll | Ist |
|---|---|---|
| Maske als Render, 8 Teile | IoU 1, Abstaende 0 | 1,000, 0,0 |
| Versatz +7,-4 | Schwerpunkt +7,-4 | +7,0,-4,0 |
| Drehung 20 Grad | 20 Grad, quer 0 | 19,9-20,0, quer 0,0 |
| Streifen verdeckt `tail` | 0,189 von `right_leg` | 0,189 von `right_leg` |
| Ueberlapp mit `right_leg` davor | nichts fehlt | IoU 1,000. Mit `tail` davor 0,820, das muss fehlen |
| Schrift im Render, mit und ohne | zaehlt nicht | IoU 1,000 beide |
| Kennung gegen `Kandidat.rendern` | 0 Pixel | 0 von 36958, 0 von 53463 |
| Verdeckung im Render | `part_of_Body` hinter `right_leg` 0,202 | 0,202 |
| Wahre Enden gegen gefundene | wenige Pixel | schlanke Teile 1-8 px, `right_arm_front` 16, `face_and_upper_body` 36-41. Bei dicken Formen laeuft der laengste Weg diagonal |
| Tausch `left_leg` gegen `right_leg` | beide fallen | 0,61 -> 0,00, 0,79 -> 0,00, `tail`, `right_arm` fast gleich, `part_of_Body` 0,72 -> 0,62: Die Ueberlapp-Regel haengt am Namen |
| Schub der Lage um 300,-200 | wie projiziert | 0,1-0,7 px. `tail` und `left_leg` bis 3,9 px, dort aendert die Schrift die gezaehlte Menge |
| Wiederholung | gleich | gleich |

Nutzer-Soll als Render: IoU 1,000, Schwarz 0, in beiden Bildern. Bild 1 26/29 Umrisspunkte, 3/3 Flaechen. Bild 2 7/7, 7/9. Die Neins stehen in 14.9.
Robustheit: Kandidat A (23 Knochen, rund 1900 Flaechen) laeuft mit einer Probe-Zuordnung ohne Sinn in 3 s, unbekannter Knochen 99 wird gemeldet, ein Teil ganz hinter anderen ergibt Verdeckt 1,00 ohne Absturz. Der alte Dummy laeuft ohne `teile` unveraendert durch.

Was am Dummy-Blatt im Bild abweicht: Ellipsoide treffen Lage und Achse, nicht die Form. `right_arm_front` IoU 0,49, keine Finger, Klauen fehlen, `tail` endet 14 px ueber dem Soll. Das ist gewollt, der Dummy prueft das Werkzeug.
Schub und Tausch sind Werkzeugproben, ihre Blaetter werden nach dem Lauf geloescht.

### 14.9 Offene Stellen

Wo Messung und Nutzerumriss nicht auf denselben Pixel fallen. Kein Eintrag widerspricht der Lesung: Grau heisst hier weder gefordert noch verboten.

| Nr | Stelle | Befund | Stand |
|---|---|---|---|
| O1 | Rechts neben `right_arm`, (405-429,262-300) | `b1.rand_rechts` (429,272) ist harte Kante, Grenze 3,6, und Z9 (411,240)-(420,262) liegt dort. Der Nutzerumriss endet in Zeile 272 bei x=404, mit eigenem Haken bei (409,259) | Beide bleiben, gemessene Kante und Zeichnung gelten laut Auftrag. Wer dem Umriss folgt, faellt an diesem Punkt durch. **Nutzer fragen, was dort liegt** |
| O2 | V3 (340-364,338) | liegt in keinem Teil, in der Luecke zwischen den Armen | V3 zaehlt weiter, ein Modell braucht dort etwas Dunkles. Grau |
| O3 | Keil zwischen `left_leg` und `tail`, (218-255,345-445) | dunkel, traegt 0,63 von Z7, ist in keinem Teil | grau |
| O4 | P (358-374,296-313) | zwischen Rumpf und `left_arm_front`, 0,06 in Teilen | bleibt |
| O5 | Knappe Neins des Nutzer-Solls | `b1.scheitel_x322` +14,5 gegen 8,6, `b1.nl_y330_rechts` -3,5 gegen 3,3, Rand 413 bei y=261 | Handschnitt an weichen Stellen, keine Anatomie. Bleiben |
| O6 | Klauen auf A13 | 354 px, die das Formblatt "sicher nicht Figur" nennt | Lesung gewinnt, die Klauen sind dort im Bild dunkel zu sehen |

Grenzen: Die Masken liegen je in einem Bild. Ob Rueckseite und Vorderseite zueinander passen, sieht nur das Rundumblatt. Verzahnte Grenzen zweier Teile im Blatt sind die Tiefensortierung des Spiels, kein Fehler.

### 14.10 Dateien

| Datei | Inhalt |
|---|---|
| `analysis/befunde_runde41/form/ziel2_bild1.png`, `ziel2_bild2.png` | neue Gesamtziele |
| `analysis/befunde_runde41/form/pruefung/ziel2_bild1_auf_original.png`, `ziel2_bild2_auf_original.png` | Ziele auf dem Original |
| `analysis/befunde_runde41/form/pruefung/ziel2_stand.json` | Pixel je Teil und Zone |
| `analysis/befunde_runde41/form/pruefung/logo_maske.png`, `logo_beleg.png` | Logo-Schrift, Beleg auf beiden Bildern |
| `analysis/befunde_runde41/form/pruefung/teile/selbstprobe.txt` | Ausgabe der Probe |
| `analysis/befunde_runde41/form/pruefung/teile/dummy/` | Dummy und Lauf `lauf` mit allen Blaettern |

Geaendert: `form_pruefung.py`, diese Datei, `pruefung/selbstprobe.txt`. Nicht geaendert: `ziel_bild*.png`, `FORMBLATT.md`, `formblatt.json`, `nutzer_teile/`, die uebrigen Werkzeuge. Kein Commit.
