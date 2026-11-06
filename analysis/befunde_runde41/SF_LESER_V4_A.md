# SF V4 - Leser A: unabhaengige Lesung der Nutzer-Beschreibung (Beschreibung1/2)

Stand 2026-11-06, Leser A, Worktree r41_sichel (Zweig r41/sichel_nb2), nichts committet.
Quellen nur: `Version_80%/Beschreibung1.png`, `Beschreibung1_Vorlage.png`, `Beschreibung2.png`, `Beschreibung2_Vorlage.png`,
`Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4` (ohne `_slp`). SF_FORMBLATT_V2 / SF_MESSBLATT_V3 wurden fuer Werte NICHT gelesen.
Die Port-Vergleiche nutzen nur die Leisten `SF_belege/v3/02_vorn_h118_h125.jpg`, `01_seite_h83_h85_h92.jpg`, `05_kopf_nah.jpg` (Spalte V3 = Commit 6447782e6, V3 Runde 2).

Werkzeuge (alle neu, `re15_port/tools/r41_sichel/v4/`): `a_l1_linien.py` (L1), `a_bildsuche.py` (Bild n), `a_schwanz.py` (L3, Parameter RG/diff),
`a_messung.py` (Abblendung, Oberkanten), `a_messung2.py` (L2, L5, H), `a_leisten_map.py` + `a_port_remap.py` (Port-Leisten -> 640er-Koordinaten), `a_farben.py` (L4), `a_belege.py` (Belegbilder).
Laufausgaben (JSON/NPY): scratchpad `sfv4/`; die Mess-JSONs liegen als Kopie in `SF_belege/v4_lesung/A_l1.json`, `A_messung.json`, `A_farben.json`, `A_schwanz.json` (+ `_lo`/`_hi`).
Belegbilder: `analysis/befunde_runde41/SF_belege/v4_lesung/A_*.png` (12 Stueck, Liste am Ende). Die B_*-Dateien dort gehoeren zu Leser B und wurden nicht gelesen.

Konventionen: Koordinaten (x, y) in Pixeln des 640x480-Bildteils, y nach unten. Seiten: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens.
Lab = CIE-Lab D65 (skimage rgb2lab, sRGB). "Schwarzwert ab" = (v-17)*255/238, geklemmt 0..255. "erodiert2" = Fenster/Maske um 2 px je Seite verkleinert.

## 0 Grundlagen (gemessen)

| Groesse | Wert | Beleg |
|---|---|---|
| Vorlage 1 = Videobild | **n = 1484** (h = 134), MAE 0,00 (Nachbarn n1483 1,46, n1485 1,80) | `a_bildsuche.py`, sequentielles Dekodieren mit cv2 ab n=0; Video 1538 Bilder, 29,97 fps |
| Vorlage 2 = Videobild | **n = 1434** (h = 84), MAE 0,00 (Nachbar n1432 6,57) | dito |
| Kamera | statisch ueber n1350..1473 (Wand links n1380..1470: RGB 154,6-155,1 / 155,9-156,1 / 159,5-160,2) | `a_messung.py`-Vorlauf, Feld x40-119 y150-299 |
| **Abblendung in Vorlage 1** | n1484 liegt in der Ausblendung (beginnt n~1474): (v1484-17) = k*(v_Ruhe-17), **k = 0,7657 / 0,7644 / 0,7682 (R/G/B)**, RMSE 2,4/2,1/2,4 | Ruhe = Median n1440..1470; Felder Wand links x20-249 y100-329, Boden x30-199 y380-439, Wand rechts x560-629 y230-329. Die Farben aus Vorlage 1 sind also um ~23 % (ueber Schwarz) zu dunkel; Tabellen geben "entblendet" mit an. |
| Schwarzwert | angesetzt 17 (Vorgabe); gemessen n1484 Balken unten RGB 15,7/16,0/13,0, oben 15,5/17,3/14,5 | `a_messung_teil1.json` |
| **H1 (Bild 1)** | Oberkante y=138 (x390/392), Sohle y=442 -> **H1 = 304 px** (Sohle +6/-3 unsicher) | Oberkante: erste Zeile mit |n1484 - Median(n1350..1473)*k| > 45 (Spalten x360-430); Sohle: Beinspalte x372-384 Lum 23-25 gegen Schlagschatten x400-410 Lum 22 bis y=442, darunter nicht trennbar |
| **H2 (Bild 2)** | Oberkante y=186 (x380-392), Sohle y=439 -> **H2 = 253 px** | Sohle in n1434 vom gruenen Vordergrund verdeckt; Standfuss (rot, R-G>=10, x420-451) Unterkante y=439 in n1440/1442/1444 (n1438: 437), gleiche Kamera, Oberkante n1440 = 185 (n1434: 186). Annahme: Standfuss steht n1434..1440 still (in n1436 ebenfalls verdeckt). |

## 1 L1 - Linienzuordnung

Maske: Kernrot |RGB - (237,28,36)| <= 40 (Paint-Rot = haeufigste Nichtweiss-Farbe im Randfeld: 673 bzw. 3411 Pixel). Gegenprobe: Linienpixel im Bildteil = Pixel mit |Beschreibung - Vorlage| > 40: **3798 = 3798** (Bild 1) und **6712 = 6712** (Bild 2), d. h. die Maske ist im Bildteil exakt die Nutzer-Zeichnung.
Linien vs. Schrift: 8er-Komponenten mit Breite > 40 px = Linie, Rest im Randfeld = Glyphen; Textbloecke = Glyphen 13x9 geschlossen, Zeilen mit Abstand <= 10 px zusammengefasst.
Endpunkte = Skelett-Enden (skimage skeletonize) im Bildteil; "Spitze" = aeusserster Kernpixel in Linienrichtung (Querabstand <= 2 px).
Belegung = Anteil der Abtastpunkte (Schritt 0,5 px) der Strecke Linienstart (am Text) -> Kandidat auf Kernrot (3x3 aufgeweitet), fuer JEDE Zuordnung; Gesamtzuordnung = Permutation mit maximaler Summe.

Ergebnis (Endpunkt = Spitze, in Klammern Skelett-Ende):

| Bild | Text | Endpunkt (x, y) | Belegung gewaehlt | beste andere Zuordnung | deren Belegung |
|---|---|---|---|---|---|
| 1 | Auge | **(381, 167)** (382,165) | 0,387 gerade / **1,000 als Polylinie** (660,112)->(386,146)->(382,165) | (403,160) | 0,372 gerade / 0,958 Polylinie; eigene Komponente (Auge-Linie = Komponente 1, Haare 2, Nase 3, keine Beruehrung) |
| 1 | Haare | **(401, 161)** (403,160) | 1,000 | (382,165) | 0,946 (laeuft 21 px ueber das eigene Linienende hinaus ins Auge-Haekchen; andere Komponente) |
| 1 | Nase | **(392, 178)** (394,177) | 1,000 | (403,160) | 0,349 |
| 2 | Auge von der Seite etwa hier | **(457, 236)** (459,235) | 1,000 | (467,241) | 0,548 (eigene Komponente) |
| 2 | Haare von der Seite | **(465, 242)** (467,241) | 1,000 | (459,235) | 0,608 (eigene Komponente) |
| 2 | Schwanzspitze Ende | **(292, 345)** (294,344) | 1,000 | (459,235) | 0,300 (unter den Schwanz-Enden: 0,114 / 0,189) |
| 2 | Schwanz Beginn in den Ruecken ueberfuehrend. | **(355, 242)** (357,243) | 1,000 | (459,235) | 0,299 (unter den Schwanz-Enden: 0,179 / 0,178) |
| 2 | Schwanz Knick, von da an wird es breiter nach oben | **(288, 307)** (290,308) | 1,000 | (459,235) | 0,208 (unter den Schwanz-Enden: 0,135 / 0,138) |

Kreuzungen: Bild 1 keine (die "Auge"-Linie hat einen Knick nach unten, als Polylinie gerechnet kreuzt sie nichts). Bild 2: "Schwanzspitze Ende" kreuzt "Schwanz Beginn" (bei ~(546,283)) und "Schwanz Knick" (bei ~(375,325)) - gewollt, die drei Schwanzlinien bilden eine 8er-Komponente; die Zuordnung ist trotzdem eindeutig (gewaehlt 1,000 gegen <= 0,19 fuer jede Vertauschung). Alle drei Schwanzlinien sind gerade (max. Abweichung Skelett-Pfad von der Sehne 2,5 px).

Belege: `A_L1_bild1_linien.png`, `A_L1_bild2_linien.png`, `A_L1_matrix.png`. Volle Matrizen unten im Anhang.

## 2 L2 - was am Endpunkt liegt

Struktur: Kern 3x3 gegen Ring (15x15 ohne 7x7); "Blob" = 4er-Komponente im 31x31-Fenster unterhalb/oberhalb der Schwelle (Kern+Ring)/2, die den Endpunkt enthaelt (bzw. naechste). Farben 9x9/15x15 je "voll" und "erodiert2" (= 5x5 bzw. 11x11).

| Bild | Text | Struktur am Endpunkt | Kern3 / Ring15 Lum | Blob (Schwelle, Groesse, Pixel, Huelle, Schwerpunkt) | RGB 5x5 (erod. 9x9) | Lab 5x5 roh | Lab 5x5 Schwarzwert ab | Bild 1: Lab entblendet + SW ab |
|---|---|---|---|---|---|---|---|---|
| 1 | Auge (381,167) | **heller, blass rosa-grauer Fleck**, Endpunkt am rechten (B) Rand des Flecks | 78,4 / 65,7 | Schwelle 72,1; bbox 374-383 x 161-172 = **10 x 12 px**; 80 px; Huelle 68 px2; SP (377,6 / 167,2); geschlossen (beruehrt 31er-Rand nicht) | 84,7 / 72,7 / 77,7 | 32,2 / 6,0 / -1,1 | 26,7 / 6,7 / -1,2 | 35,1 / 8,2 / -1,3 |
| 1 | Haare (401,161) | **dunkles, leicht violettstichiges Feld** rechts (B) neben/ueber dem Auge | 28,2 / 34,2 | Schwelle 31,2; bbox 394-408 x 154-176 = 15 x 23 px; 187 px; beruehrt 31er-Rand (Feld offen nach unten) | 35,7 / 24,8 / 26,7 | 10,1 / 5,8 / 0,8 | 3,2 / 4,1 / 0,6 | 4,4 / 6,5 / 1,0 |
| 1 | Nase (392,178) | schwach hellere Stelle (Leiste) zwischen dunklen Flaechen, ohne Rand | 42,6 / 38,9 (+3,7) | Blob nicht trennscharf: bei Schwelle 40,7 laeuft er in den Augenfleck (bbox 377-396 x 163-181, 247 px) | 50,4 / 38,4 / 39,2 | 16,7 / 5,8 / 1,6 | 9,4 / 6,7 / 2,0 | 13,4 / 8,4 / 2,6 |
| 2 | Auge von der Seite (457,236) | **heller, blass grau-rosa Fleck**, Endpunkt am vorderen (B rechts) Rand | 82,9 / 72,7 | Schwelle 77,8; bbox 444-458 x 229-244 = **15 x 16 px**; 137 px; Huelle 134 px2; SP (452,1 / 236,7); geschlossen | 84,6 / 79,2 / 81,1 | 34,2 / 2,6 / -0,4 | 28,9 / 2,9 / -0,4 | - |
| 2 | Haare von der Seite (465,242) | **dunkle Vorderkante** des Kopfes (x462-465, Lum 23-40) direkt vor dem hellen Tuerpfosten; KEIN Hintergrund: dasselbe Pixel ist in n1360..1430 und n1452 Lum 56-61 | 43,7 / 69,8 | Schwelle 56,8; bbox 450-470 x 227-257, 334 px, beruehrt Rand (Kante laeuft weiter) | 54,6 / 54,7 / 44,9 (Fenster schliesst gelben Pfosten ein) | 22,6 / -2,1 / 6,1 | 16,0 / -2,3 / 6,9 | - |
| 2 | Schwanzspitze Ende (292,345) | dunkles schmales Band (unterster Schwanzteil) ueber heller Bodenkante y348-352 | 38,3 / 46,1 | bbox 277-307 x 330-345 (offen) | 44,4 / 36,3 / 36,6 | 15,2 / 3,9 / 1,2 | 7,6 / 4,5 / 1,3 | - |
| 2 | Schwanz Beginn (355,242) | sehr dunkle, rotstichige Unterseite des Schwanzes, wo sie an Buckel/Oberarm stoesst | 24,3 / 34,4 | bbox 340-362 x 240-257 (offen) | 27,1 / 22,3 / 26,8 | 8,1 / 3,5 / -2,3 | 2,0 / 2,0 / -1,3 | - |
| 2 | Schwanz Knick (288,307) | dunkles Band, Ort der Richtungsaenderung der Aussenkante (s. 3) | 34,7 / 39,4 | bbox 279-303 x 292-309 (offen) | 39,4 / 33,3 / 31,1 | 13,5 / 2,4 / 2,6 | 5,8 / 2,4 / 2,2 | - |

9x9/15x15 voll und 11x11 stehen in `a_messung.json` (Schluessel linien[].ort.f9/f15). Belege `A_kopf_vorn_auge.png`, `A_kopf_vorn_haare.png`, `A_kopf_vorn_nase.png`, `A_kopf_seite_auge.png`, `A_kopf_seite_haare.png` (gelb 9x9, cyan 15x15, magenta Blob-Huelle, orange Kopfbox).

### Lage im Kopf (L2/L5)

Kopfbox vorn (Bild 1): **x368-416, y138-191 = 48 x 53 px = 0,158 x 0,174 H1, B/H 0,906**. Regeln: links = Lum-Minimum Zeile y165 in x355-374 (dunkle Kontur, x368); oben = H1-Oberkante; unten = Lum-Minimum Spalte x381 in y178-199 (dunkler Spalt, y191); rechts = linke Kante an der Nase (x392) gespiegelt, weil die rechte Kopfkante (B) im dunklen Haarfeld NICHT aufloest.
Kopfbox seitlich (Bild 2): **x412-464, y190-263 = 52 x 73 px = 0,206 x 0,289 H2, B/H 0,712**. vorn = dunkle Vorderkante (Lum-Minimum Zeile y244, x464); oben = erste Zeile R-G >= 12 in Spalte x428 (rote Zone, y190); hinten = Lum-Minimum Zeile y236 in x405-419 (x412); unten = Ende des Helligkeitsabfalls unter dem Augenfleck (Zeilenmittel x438-459 erreicht Plateau+1 bei y263) - KEINE Kinnkante, unscharf.

| Merkmal | Endpunkt | x/Breite | y/Hoehe | zur Kopfmitte dx, dy px | in H | Seite |
|---|---|---|---|---|---|---|
| Auge vorn | (381,167) | 0,271 | 0,547 | -11,0 / +2,5 | -0,036 / +0,008 H1 | (B) links = **(W) rechts** (Wesen blickt zur Kamera, n1450-1500 geht es auf die Kamera zu) |
| Augenfleck-Schwerpunkt vorn | (377,6 / 167,2) | 0,200 | 0,551 | -14,4 / +2,7 | -0,047 / +0,009 H1 | (B) links = (W) rechts |
| Haare vorn | (401,161) | 0,688 | 0,434 | +9,0 / -3,5 | +0,030 / -0,012 H1 | (B) rechts = **(W) links**, hoeher als das Auge |
| Nase vorn | (392,178) | 0,500 | 0,755 | 0,0 / +13,5 | 0 / +0,044 H1 | Mitte (per Konstruktion der rechten Boxkante; Nase liegt auch 11 px (B) rechts vom Auge) |
| Auge seitlich | (457,236) | 0,865 | 0,630 | +19,0 / +9,5 | +0,075 / +0,038 H2 | vorn-unten am Kopf (Wesen blickt nach (B) rechts) |
| Augenfleck-SP seitlich | (452,1 / 236,7) | 0,771 | 0,640 | +14,1 / +10,2 | +0,056 / +0,040 H2 | |
| Haare seitlich | (465,242) | 1,019 (= Vorderkante) | 0,712 | +27,0 / +15,5 | +0,107 / +0,061 H2 | Vorderkante, 8 px vor und 6 px unter dem Augen-Endpunkt |

Abstaende vorn: Auge->Haare dx +20, dy -6 = **20,9 px = 0,069 H1**; Auge->Nase dx +11, dy +11 = 15,6 px = 0,051 H1; Haare->Nase dx -9, dy +17 = 19,2 px = 0,063 H1. Seitlich Auge->Haare dx +8, dy +6 = 10,0 px = 0,040 H2.
Augengroesse: vorn 10 x 12 px (0,033 x 0,039 H1), seitlich 15 x 16 px (0,059 x 0,063 H2) - beide Male ein **heller** (nicht dunkler) Fleck ohne Pupille/Ring; Rand weich (Schwellen-Blob). Ein zweites Auge ist im Bild 1 nicht beschriftet; an der Spiegelstelle (x~403, y~167) liegt das dunkle Haarfeld.
Beleg: `A_kopf_boxen.png`.

### L5 Kopfform, helle Kuppe, rote Zone, Maul

- Vorn: rote Zone **oben am Kopf (B) mittig-rechts**, x389-407 y140-156 (R-G >= 15; Spalte x398: R-G steigt ab y136, Maximum 40 bei y148-150, faellt unter 15 ab y158). Links (B) davon die graumauve Kuppe x372-388 y147-157 (Lab roh 21,0/8,6/2,3). Eine **helle** Kuppe ist im Vorderbild nicht vorhanden: die hellste Kopfstelle ist der Augenfleck (Lum 78).
- Seitlich: rote Zone **oben auf dem Kopf**, x418-441 y193-211 (R-G bis 45-50 bei y196-204), Lab roh 28,8/17,8/5,0. Dahinter der helle Buckel (x372-414, Lum >= 80: Lab roh 59,4/11,0/1,2). Kopf unter dem Auge grau-dunkel (Lab roh 16,6/2,0/0,3), keine Kinnkante.
- Maul: **nicht beschriftet und im Bild nicht als Struktur aufloesbar.** Vorn liegt unter dem Kopf (x378-411, y183-194) ein dunkler Spalt (Lum-Minimum 17 bei y191 in Spalte x381) und darunter helle Flecken y185-205 (x375-380, 400-405, 430), die ebenso Zaehne wie Rippen/Brustwunde sein koennen - nicht entscheidbar. Seitlich keine Maulkontur.

## 3 Schwanzverlauf (L3, Bild 2 = n1434, H2 = 253 px)

### 3a Textlesung (Polygonzug aus den drei Nutzerlinien)

| Punkt | (x, y) | ueber Sohle (y=439) | unter Oberkante (y=186) |
|---|---|---|---|
| Beginn (in den Ruecken ueberfuehrend) | (355, 242) | 197 px = 0,779 H | 56 px = 0,221 H |
| Knick | (288, 307) | 132 px = 0,522 H | 121 px = 0,478 H |
| Spitze / Ende | (292, 345) | 94 px = 0,372 H (Bildhoehe; der Schwanz steht hinter dem Wesen, die Bodenhoehe dort ist nicht messbar) | 159 px = 0,628 H |

- Beginn -> Knick: Laenge 93,3 px = 0,369 H, Richtung -135,9 Grad (Bild: 0 Grad = rechts, +90 = oben; also nach links unten, 44 Grad unter der Waagerechten). Relativ zum Wesen (blickt nach (B) rechts): **nach hinten und unten**.
- Knick -> Ende: 38,2 px = 0,151 H, Richtung -84,0 Grad (fast senkrecht nach unten, 6 Grad nach (B) rechts, also leicht wieder nach vorn).
- Richtungsaenderung am Knick: **51,9 Grad**.
- Lage laengs des Rueckens: Beginn liegt an der **Unterseite** des Schwanzansatzes, wo er an Buckel und nahen Oberarm stoesst (x355 = 0,22 H unter der Oberkante, 109 px = 0,43 H hinter der Kopf-Vorderkante x464). Die Oberseite des Ansatzes liegt an derselben Stelle bei y ~196-205 (Maske, s. 3b).
- "von da an wird es breiter nach oben": "oben" in Bildkoordinaten = -y. Vom Knick aus nach oben liegt der Teil Knick -> Beginn (der Schwanz steigt vom Knick aus zum Ruecken an). Relativ zum Wesen = **zum Ansatz hin (proximal), nach oben/dorsal**. Der Teil Knick -> Spitze liegt UNTER dem Knick. Die Text-Lesung ist also: duenn von der Spitze bis zum Knick, ab dem Knick nach oben zum Ruecken hin breiter werdend. (Die Aufgabe fragt "Breite bei 0,1..0,9 der Strecke Knick -> Ende"; beides ist unten gemessen, aber "nach oben" ist nach dem Bild die Strecke Knick -> Beginn.)
- Kreuzungen der Nutzerlinien: Spitze x Beginn bei (545,5 / 283,5), Spitze x Knick bei (373,9 / 324,8) - beide im Randfeld bzw. ueber dem Koerper, nicht am Schwanz.

### 3b Maske (Gegenprobe)

Maske (Standard): ROI x262-358, y180-356 ohne (x>=345 & y>=250: Arm), ohne (y>=330 & x>=306: Boden/Vordergrund), ohne (x>=330 & y<196: dunkle Tuer-Oberkante);
Pixel = (R-G >= 5 & Lum < 120 & (x >= 318 oder |I - Platte| >= 15)) ODER (|I - Platte| >= 40 & x < 318); Platte = Median n1350..1473 (fuer x < 318 frei von Birkin, gemessen: dort |Platte - n1360| klein, rechts davon steht Birkin im Median);
3x3 geschlossen, nur die Komponenten an Spitze/Knick/Beginn, Loecher < 300 px gefuellt. Achse = Skelett, Pfad Beginn -> Spitze. Breite = Lauflaenge der Maske senkrecht zur Achse (Tangente aus +-5 Pfadpunkten).
Gegenproben mit (R-G>=4, diff>=30) und (R-G>=6, diff>=50). Beleg `A_schwanz_polygon.png`.

Befunde:
1. **Die Maske bestaetigt die Text-Lesung in der Breite**: oberhalb des Knicks waechst die Breite zum Ruecken hin (Knick->Beginn 0,1/0,3/0,5/0,7: 7,0 / 17,0 / 31,5 / 38,0 px = 0,028 / 0,067 / 0,125 / 0,150 H; Gegenproben 8,0/21,5/32,0/37,0 und -/18,0/32,5/27,0). Der Wert bei 0,9 (55 px) ist durch den ROI-Schnitt x<=358 bzw. den Buckel begrenzt und NICHT verwertbar.
2. Unterhalb des Knicks (Knick -> Spitze) ist der Schwanz **schmal und etwa gleich breit**: 6,0 / 8,0 / 9,0 / 9,0 / 7,0 px bei 0,1/0,3/0,5/0,7/0,9 (= 0,024-0,036 H); Gegenprobe 6/9/9/12/7. Letzte 10 %: 7-13 px; eine Verjuengung zur Spitze ist **nicht messbar**, Spitzenform **nicht aufloesbar** (Unterkante der Maske y353 stoesst an die helle Bodenkante y348-352).
3. **Die Maske hat am Knick eine Luecke** von 10,4 px ((282,294) -> (285,304)): dort ist der Schwanz gegen den Tuerpfosten kontrastlos (R-G 2-5, |I-Platte| 24-35). Die engste gemessene Stelle (6-7 px) liegt unmittelbar ober- und unterhalb dieser Luecke, also **am Knick**.
4. **Verlauf nach der Maske weicht von der geraden Text-Strecke Beginn -> Knick ab** (nicht geglaettet): Vom Beginn laeuft die Achse zuerst fast **waagerecht nach (B) links** (Richtungen -173 / -158 / -169 / -149 / -163 Grad zwischen x356 und x298, y231-247), biegt bei ~(298,247) **nach unten** um (-121 / -101 / -96 Grad) und laeuft von y~257 bis y~297 nahezu **senkrecht** (x289-292, -84 bis -96 Grad). Die staerkste Kruemmung der Achse liegt also bei ~(295,250) (~70 Grad Drehung), nicht am Nutzer-Knick (288,307). Am Nutzer-Knick dreht die Achse nur wenig (-117 -> -96 -> -58 -> -96 Grad zwischen y297 und y335), dort aendert sich vor allem die **Breite** (und die Aussenkante: |I-Platte|-Maxima wandern von x282 bei y280-288 nach x274-278 bei y304-316 und wieder nach x290-294 bei y344). Die Text-Strecke Beginn->Knick ist die Sehne dieses Bogens (Polygon 93,3 px, Maskenachse Beginn->Knick 140,1 px = 0,554 H; gesamt 188,1 px = 0,743 H).
5. Farben/Helligkeit quer: Aussen-/Oberseite des Bogens heller grau (die |I-Platte|>=40-Kante), Innen-/Unterseite dunkel rotstichig (R-G 10-29 bei x300-350, y224-252).

Maskenachse (jeder 10. Punkt, Richtung zum naechsten): (356,241) 135 | (346,231) -173 | (338,232) -158 | (328,236) -169 | (318,238) -149 | (308,244) -163 | (298,247) -121 | (292,257) -101 | (290,267) -96 | (289,277) -84 | (290,287) -84 | (291,297) -117 | (286,307) -96 | (285,317) -58 | (290,325) -96 | (289,335) -84 Grad.
Bild 1 (vorn): Schwanz als sehr dunkles Band links (B) neben dem Koerper (Maske 16 in `A_farbregionen.png`, x297-332 y340-375); ob dieses Band Schwanz oder Schlagschatten ist, gibt Bild 1 allein nicht her (Farbe ~ Schwarzwert).

## 4 Farben (L4) - "EXAKT seine Farben"

Regionen als Boxen bzw. Masken (Belege `A_farbregionen.png` mit Nummern, `A_farbfelder.png` mit Farbfeldern roh / Schwarzwert ab / entblendet). Boxen 2 px erodiert; Blobs und Schwanzmasken unerodiert bzw. 1-2 px wie angegeben. Volle Tabelle (RGB, Std, Lab roh, Lab Schwarzwert ab, Lab entblendet) im Anhang; JSON `a_farben.json`.

Weissabgleich/Bezug des Videos (Vorlage 2, unabgeblendet): Wand links RGB 156,0/156,9/161,1 (Lab 64,7/0,5/-2,3), Boden 64,6/65,3/71,0 (Lab 27,8/1,0/-3,5) - leicht blaustichig neutral (b* -2 bis -4). Vorlage 1: Wand links 123,3/124,2/127,3, entblendet = Ruhewert.

Kernaussagen (Lab nach Schwarzwert-Abzug; Bild 1 zusaetzlich entblendet):
- **Die Haut ist ein dunkles, gering gesaettigtes Mauve-Grau** (a* +4..+12, b* -1..+3, Buntton 350-30 Grad), nicht rot: Oberarm/Schulter (B) links vorn 27,2/5,8/3,2, Augenumfeld vorn 27,0/7,9/-1,1, Oberarm nah seitlich 21,3/8,5/3,1, Buckel hell seitlich 56,3/12,0/1,3 (hellste Hautstelle, Lum >= 80).
- **Rot** ist nur in klar begrenzten Zonen: Brust vorn (R-G >= 15, 1498 px) 10,5/19,1/7,7; rote Zone Kopf oben vorn 18,1/22,3/6,9; seitlich 23,2/19,7/6,1; Klaue (B) rechts 6,1/16,0/3,2. Chroma 16-24, Buntton 17-22 Grad - ein dunkles Blutrot, keine hellen Rottoene.
- **Auge** = hellster Kopfpunkt, neutral-rosa: vorn 39,9/8,0/-1,2 (entblendet+SW; roh 35,8/5,9/-1,1), seitlich 38,7/4,5/0,5. **Haare** = fast schwarz, leicht violettstichig: vorn 3,2/4,4/0,7 (roh RGB 31,8/22,9/24,3), seitlich (Kante) 10,9/1,2/3,0.
- **Jeans**: vorn sehr dunkel neutral-blaugrau 4,3/-0,6/-1,3 (roh 25,8/28,0/29,4), seitlich 9,9/0,7/-3,7 (roh 40,9/42,4/47,1) - nur schwach blau.
- **Schwanz** (Bild 2): nahe Ruecken (Unterseite, Maske 0-15 % Beginn->Knick) dunkel rotstichig 4,5/8,8/1,1 (roh 44,1/25,6/29,7); breiter Teil 11,5/5,7/1,6 (roh 53,4/42,8/43,6, Std 16-20 = hell/dunkel geteilt); am Knick grau 16,5/1,6/1,8 (roh 58,4/54,3/52,8); Spitze 11,7/4,0/1,4 (roh 51,6/44,0/44,2). Also: dunkles Mauve-Grau wie die Haut, zum Ansatz hin rotstichiger, am Knick neutral grau. Kein Gruen-/Olivstich (b* <= +1,8).

### Port V3 r2 gegen Original

Abbildung der Leisten: Spalte "Original" per NCC gegen das Videobild n = 1350 + h verortet (02_vorn h118: NCC 0,864; 01_seite h83: 0,885; 05_kopf h118: 0,960; h85: 0,979), die V3-Spalte mit derselben Abbildung in 640er-Koordinaten zurueckgerechnet (`a_leisten_map.py`, `a_port_remap.py`).
**Eichung ueber den gemeinsamen Hintergrund** (Port zeigt dasselbe Raumbild): 02_vorn h118 Port -> Video n1468 an 4 statischen Feldern: 140,1->145,3; 85,1->95,8; 44,0->55,0; 49,8->60,3 (Mittel RGB), also Video ~ 13,7 + 0,94 * Port - passt zum Schwarzwert-Abzug (Port ~ Original "Schwarzwert ab" +-4). **01_seite h83 ist NICHT farbvergleichbar**: dort Port 99,0->Video 141,4, 65,7->82,4, 46,0->45,1 (Steigung ~0,6, Leiste abgedunkelt/anders verarbeitet); dE der Seitenregionen deshalb nur als Richtung.

| Original (bereinigt) | Port V3 r2 | dE00 | Befund |
|---|---|---|---|
| Kopf-Kuppe vorn 19,5/12,2/3,6 | Kopf-Kuppe 31,5/9,6/3,3 | 9,1 | Port 12 L* zu hell |
| Augenumfeld vorn 27,0/7,9/-1,1 | Augenumfeld 32,4/5,7/-0,1 | 4,8 | Port 5 L* zu hell |
| Auge vorn 39,9/8,0/-1,2 | Augenumfeld (Port hat dort ein dunkles Auge) | 6,7 | Original-Auge ist HELLER als die Umgebung, Port-Auge dunkel |
| Haare vorn 3,2/4,4/0,7 | Kopf (B) rechts neben Auge 31,8/6,7/0,9 | **19,4** | Port hat dort kein dunkles Feld (+28,6 L*) |
| unter Kopf/Kinn 8,1/10,6/3,8 | Maul 6,4/8,9/1,8 | 2,3 | |
| Brust rot 10,5/19,1/7,7 | Brust rot 24,7/27,6/14,1 | 10,9 | Port +14 L*, Chroma 31 statt 21 |
| Brust hell 24,8/5,9/1,6 (nur 14 px!) | Brust hell 31,5/5,5/4,3 | 5,6 | |
| Oberarm/Schulter (B) links 27,2/5,8/3,2 | 24,9/17,1/7,9 | 9,7 | Port viel roter (C 18,8 statt 6,6) |
| Jeans 4,3/-0,6/-1,3 | 15,2/-2,1/-8,1 | 9,0 | Port heller und deutlich blauer |
| Schwanz vorn 0,6/0,0/-0,2 | 14,3/-0,9/1,8 | 8,7 | Original-Band ~ Schwarz (Zuordnung offen) |
| Seite: rote Zone Kopf 23,2/19,7/6,1 | Kopf oben grau 39,2/6,4/-0,1 | 16,7 | Port-Kopf seitlich grau statt rot oben |
| Seite: Buckel hell 56,3/12,0/1,3 | 31,7/12,9/3,2 | 23,0 | Leiste abgedunkelt - nur Richtung |
| Seite: Schwanz nahe Ruecken 4,5/8,8/1,1 | 20,0/7,8/10,5 | 12,6 | Port gelbstichig (h 53 statt 7 Grad) |
| Seite: Schwanz breiter Teil 11,5/5,7/1,6 | 18,6/7,5/5,9 | 6,0 | Port gelbstichiger (h 38 statt 16) |
| Seite: Schwanz Spitze 11,7/4,0/1,4 | 20,2/10,0/-4,4 | 9,4 | Port-Box trifft evtl. Kante (h 336) |

Volle Tabelle mit Rohwerten im Anhang.

## 5 Offen (nicht aufloesbar oder unsicher)

1. **Vorlage 1 ist abgeblendet** (n1484, k = 0,766): jede Farbe aus Bild 1 muss entblendet werden; ein unabgeblendetes Bild in genau dieser Pose gibt es nicht (Abblendung beginnt n~1474). Der Faktor ist aus statischen Wand-/Bodenfeldern gefittet (RMSE <= 2,4) und gilt nur, wenn die Abblendung linear ueber dem Schwarzwert wirkt.
2. **H1-Sohle** (y442) ist die Grenze, ab der Bein und Schlagschatten nicht mehr trennbar sind (+6/-3 px). **H2-Sohle** stammt aus n1440 (Annahme: Standfuss steht n1434..1440 still).
3. **Rechte (B) Kopfkante vorn** loest im dunklen Haarfeld nicht auf; die Kopfbox ist dort an der Nase gespiegelt. **Kinn seitlich** ohne Kante (Box-Unterkante nur Helligkeitsplateau).
4. **Nase**: nur eine um 3,7 Lum hellere Stelle ohne Rand; als eigene Struktur nicht trennbar vom Augenfleck. Form/Groesse nicht aufloesbar.
5. **Maul**: nicht beschriftet; im Bild nicht als Struktur aufloesbar (dunkler Spalt + helle Flecken unter dem Kopf koennen Zaehne oder Brust sein).
6. **Zweites Auge** vorn: nicht beschriftet; an der Spiegelstelle liegt dunkles (Haar-)Feld.
7. **Schwanz am Knick**: Maskenluecke 10 px (kontrastlos gegen Tuerpfosten); Breite dort 6-8 px aus den Raendern ober-/unterhalb. **Spitzenform** nicht aufloesbar (letzte 10 % 7-13 px, Unterkante an heller Bodenkante). **Ansatzbreite** (0,9 Knick->Beginn) nicht verwertbar, weil Schwanzoberseite und Buckel nicht trennbar sind.
8. **Bodenhoehe unter der Schwanzspitze** nicht messbar (Schwanz hinter dem Wesen); "ueber Sohle" ist nur Bildhoehe.
9. **Schwanz in Bild 1**: dunkles Band x297-332 y340-375 ~ Schwarzwert; Schwanz oder Schlagschatten nicht entscheidbar.
10. **Port-Farbvergleich Seite**: Leiste 01_seite ist gegen das Video abgedunkelt (Hintergrund-Steigung ~0,6); fuer die Seitenregionen ist ein eigener Software-Render in der Kamera von n1434 noetig, wenn exakte dE gebraucht werden. Port-Regionsboxen sind von Hand auf die Port-Pose gesetzt (Port-Pose h83/h118 != Original h84/h134).
11. **Widerspruch Aufgabe/Text**: Die Aufgabe nimmt die Verbreiterung auf der Strecke Knick -> Ende an; Text und Maske sagen: Verbreiterung vom Knick **nach oben zum Ruecken** (Knick -> Beginn), unter dem Knick bleibt der Schwanz schmal. Beide Strecken sind gemessen.
12. **Krummung**: Die staerkste Achsenkruemmung liegt bei ~(295,250), der Nutzer-Knick bei (288,307) markiert die Breitenaenderung/Aussenkanten-Aenderung - ob der Nutzer mit "Knick" die Biegung oder den Beginn des duennen Endstuecks meint, entscheidet das Bild nicht; der Endpunkt (288,307) ist gemessen.

## Belegbilder (`analysis/befunde_runde41/SF_belege/v4_lesung/`)

- `A_L1_bild1_linien.png`, `A_L1_bild2_linien.png` - jede Linie nachgezeichnet, Endpunkt-Kreis, Textblock-Rahmen, Belegung gewaehlt/Alternative
- `A_L1_matrix.png` - Belegungsmatrizen beider Bilder
- `A_kopf_vorn_auge.png`, `A_kopf_vorn_haare.png`, `A_kopf_vorn_nase.png`, `A_kopf_seite_auge.png`, `A_kopf_seite_haare.png`, `A_kopf_boxen.png` - Kopf-Ausschnitte mit Fenstern, Blob-Huelle, Kopfbox
- `A_schwanz_polygon.png` - Text-Polygon, Maskenrand, Maskenachse, Breiten, Luecke
- `A_farbfelder.png`, `A_farbregionen.png` - Farbfelder und Regionsmasken

## Anhang - volle Tabellen

**Beschreibung1 - Belegungsmatrix** (Zeile = Text, Spalte = Kandidat-Endpunkt im Bildteil; Wert = Anteil der Abtastpunkte (0,5 px) der Strecke Linienstart->Endpunkt auf Kernrot, 3x3 aufgeweitet)

| Text | (403,160) | (382,165) | (394,177) |
|---|---|---|---|
| Auge | 0.372 | **0.387** | 0.356 |
| Haare | **1.000** | 0.946 | 0.339 |
| Nase | 0.349 | 0.316 | **1.000** |

Kreuzungen der gewaehlten Strecken: keine

| Text | Textbox (x0,y0,x1,y1) | Linienstart am Text | Endpunkt Skelett | Endpunkt Spitze | Belegung gewaehlt | beste Alternative | Belegung Alternative | 8er-Komponente Start / Ende |
|---|---|---|---|---|---|---|---|---|
| Auge | [679, 106, 710, 118] | [660, 112] | [382, 165] | [381, 167] | 0.387 (Polylinie 1.000) | [403, 160] | 0.372 (Polylinie 0.958) | 1 / 1 |
| Haare | [677, 134, 714, 143] | [660, 138] | [403, 160] | [401, 161] | 1.000 | [382, 165] | 0.946 | 2 / 2 |
| Nase | [680, 161, 710, 170] | [664, 166] | [394, 177] | [392, 178] | 1.000 | [403, 160] | 0.349 | 3 / 3 |

**Beschreibung2 - Belegungsmatrix** (Zeile = Text, Spalte = Kandidat-Endpunkt im Bildteil; Wert = Anteil der Abtastpunkte (0,5 px) der Strecke Linienstart->Endpunkt auf Kernrot, 3x3 aufgeweitet)

| Text | (459,235) | (467,241) | (357,243) | (290,308) | (294,344) |
|---|---|---|---|---|---|
| Auge von der Seite etwa hier | **1.000** | 0.548 | 0.201 | 0.310 | 0.234 |
| Haare von der Seite | 0.608 | **1.000** | 0.501 | 0.139 | 0.239 |
| Schwanzspitze Ende | 0.300 | 0.160 | 0.114 | 0.189 | **1.000** |
| Schwanz Beginn in den Ruecken ueberfuehrend. | 0.299 | 0.252 | **1.000** | 0.179 | 0.178 |
| Schwanz Knick, von da an wird es breiter nach oben | 0.208 | 0.182 | 0.135 | **1.000** | 0.138 |

Kreuzungen der gewaehlten Strecken: Schwanzspitze Ende x Schwanz Beginn in den Ruecken ueberfuehrend., Schwanzspitze Ende x Schwanz Knick, von da an wird es breiter nach oben

| Text | Textbox (x0,y0,x1,y1) | Linienstart am Text | Endpunkt Skelett | Endpunkt Spitze | Belegung gewaehlt | beste Alternative | Belegung Alternative | 8er-Komponente Start / Ende |
|---|---|---|---|---|---|---|---|---|
| Auge von der Seite etwa hier | [664, 171, 822, 200] | [651, 177] | [459, 235] | [457, 236] | 1.000 | [467, 241] | 0.548 | 1 / 1 |
| Haare von der Seite | [682, 212, 809, 222] | [668, 221] | [467, 241] | [465, 242] | 1.000 | [459, 235] | 0.608 | 2 / 2 |
| Schwanzspitze Ende | [675, 260, 768, 289] | [660, 256] | [294, 344] | [292, 345] | 1.000 | [459, 235] | 0.300 | 3 / 3 |
| Schwanz Beginn in den Ruecken ueberfuehrend. | [681, 317, 784, 366] | [664, 309] | [357, 243] | [355, 242] | 1.000 | [459, 235] | 0.299 | 3 / 3 |
| Schwanz Knick, von da an wird es breiter nach oben | [687, 381, 784, 449] | [675, 385] | [290, 308] | [288, 307] | 1.000 | [459, 235] | 0.208 | 3 / 3 |

### Farbtabelle Original

| Bild | Region / Maske | n | RGB roh | Std RGB | Lab roh | Lab Schwarzwert ab | Lab entblendet + Schwarzwert ab (nur Bild 1) |
|---|---|---|---|---|---|---|---|
| bild1 | Kopf-Kuppe (Oberkopf B-links der roten Zone) box 372-388 x 147-157; Maske 91 px, bbox x374-386 y149-155, 2 px erodiert | 91 | [64.0, 45.8, 47.4] | [5.6, 6.7, 6.4] | [21.0, 8.6, 2.3] | [14.2, 9.8, 2.7] | [19.5, 12.2, 3.6] |
| bild1 | Kopf rote Zone (oben) box 389-407 x 140-156, R-G>=15; Maske 147 px, bbox x391-405 y142-154, 2 px erodiert | 147 | [70.8, 38.1, 42.2] | [16.7, 15.2, 13.8] | [19.6, 16.0, 4.0] | [13.0, 17.8, 5.3] | [18.1, 22.3, 6.9] |
| bild1 | Augenumfeld box 371-391 x 157-177; Maske 289 px, bbox x373-389 y159-175, 2 px erodiert | 289 | [70.6, 59.5, 64.0] | [16.6, 16.1, 16.4] | [26.4, 5.8, -1.0] | [20.2, 6.4, -1.0] | [27.0, 7.9, -1.1] |
| bild1 | Auge (heller Fleck, Blob-Schwelle 72,1); Maske 80 px, bbox x374-383 y161-172 | 80 | [93.1, 81.1, 86.1] | [10.4, 10.4, 10.4] | [35.8, 5.9, -1.1] | [30.6, 6.5, -1.2] | [39.9, 8.0, -1.2] |
| bild1 | Haare (dunkles Feld, Blob-Schwelle 31,2); Maske 187 px, bbox x394-408 y154-176 | 187 | [31.8, 22.9, 24.3] | [3.8, 3.0, 3.7] | [8.8, 4.8, 0.7] | [2.4, 3.0, 0.4] | [3.2, 4.4, 0.7] |
| bild1 | Nase box 387-397 x 173-183; Maske 49 px, bbox x389-395 y175-181, 2 px erodiert | 49 | [50.3, 38.6, 39.7] | [6.0, 5.8, 5.6] | [16.8, 5.7, 1.3] | [9.4, 6.6, 1.6] | [13.5, 8.2, 2.2] |
| bild1 | unter dem Kopf/Kinn box 378-411 x 183-194; Maske 240 px, bbox x380-409 y185-192, 2 px erodiert | 240 | [44.3, 29.4, 30.0] | [19.1, 18.3, 18.5] | [12.8, 7.4, 2.5] | [5.5, 7.1, 2.2] | [8.1, 10.6, 3.8] |
| bild1 | Brust rot box 372-424 x 195-234, R-G>=15; Maske 1498 px, bbox x374-422 y197-232, 2 px erodiert | 1498 | [55.3, 28.1, 30.0] | [13.0, 12.2, 12.6] | [14.1, 13.6, 4.5] | [7.0, 15.1, 4.8] | [10.5, 19.1, 7.7] |
| bild1 | Brust hell box 372-424 x 195-234, R-G<15 und Lum>=45; Maske 14 px, bbox x374-382 y231-232, 2 px erodiert | 14 | [66.0, 56.7, 57.5] | [1.4, 2.6, 2.1] | [24.8, 4.2, 1.0] | [18.4, 4.7, 1.2] | [24.8, 5.9, 1.6] |
| bild1 | Oberarm/Schulter B-links box 305-349 x 165-214; Maske 1886 px, bbox x307-347 y167-212, 2 px erodiert | 1886 | [70.7, 60.6, 59.8] | [24.4, 24.1, 24.3] | [26.6, 4.2, 2.1] | [20.4, 4.7, 2.4] | [27.2, 5.8, 3.2] |
| bild1 | Oberarm B-rechts box 435-479 x 180-229; Maske 1886 px, bbox x437-477 y182-227, 2 px erodiert | 1886 | [45.3, 33.3, 35.0] | [12.8, 13.8, 13.6] | [14.3, 6.1, 1.1] | [6.8, 6.6, 1.3] | [10.1, 8.6, 2.0] |
| bild1 | Unterarm B-rechts box 462-494 x 265-319; Maske 1479 px, bbox x464-492 y267-317, 2 px erodiert | 1479 | [62.7, 63.2, 64.7] | [30.8, 31.4, 30.4] | [26.7, 0.1, -1.0] | [20.6, 0.1, -1.1] | [27.4, 0.0, -1.2] |
| bild1 | Klaue B-rechts box 482-507 x 330-356, R-G>=15; Maske 128 px, bbox x488-503 y344-354, 2 px erodiert | 128 | [45.5, 23.1, 27.4] | [5.9, 5.1, 5.2] | [11.0, 12.0, 1.9] | [4.1, 10.3, 1.8] | [6.1, 16.0, 3.2] |
| bild1 | Hand/Klaue B-links box 375-409 x 320-354; Maske 961 px, bbox x377-407 y322-352, 2 px erodiert | 961 | [46.4, 35.8, 38.1] | [17.4, 14.8, 14.0] | [15.4, 5.5, 0.3] | [7.8, 6.3, 0.5] | [11.4, 8.0, 0.7] |
| bild1 | Jeans box 372-439 x 262-299; Maske 2176 px, bbox x374-437 y264-297, 2 px erodiert | 2176 | [25.8, 28.0, 29.4] | [9.0, 9.1, 9.5] | [10.1, -0.7, -1.3] | [3.2, -0.3, -0.8] | [4.3, -0.6, -1.3] |
| bild1 | Schwanz vorn (Band links, |I-bg*k|>=30, Lum<60); Maske 512 px, bbox x297-333 y343-376, 1 px erodiert | 512 | [17.7, 18.2, 18.1] | [3.2, 3.1, 3.6] | [5.5, -0.3, 0.0] | [0.5, -0.0, -0.1] | [0.6, -0.0, -0.2] |
| bild1 | Bezug Wand links box 40-119 x 150-299; Maske 11096 px, bbox x42-117 y152-297, 2 px erodiert | 11096 | [123.3, 124.2, 127.3] | [30.5, 29.8, 30.9] | [52.1, 0.3, -1.8] | [48.4, 0.3, -2.0] | [62.2, 0.2, -2.1] |
| bild1 | Bezug Wand rechts box 560-629 x 230-329; Maske 6336 px, bbox x562-627 y232-327, 2 px erodiert | 6336 | [85.4, 92.1, 105.4] | [18.4, 18.2, 19.5] | [39.0, 0.5, -8.4] | [34.1, 0.6, -9.2] | [44.3, 0.6, -11.2] |
| bild1 | Bezug Boden box 30-199 x 380-439; Maske 9296 px, bbox x32-197 y382-437, 2 px erodiert | 9296 | [51.3, 52.8, 57.3] | [7.2, 7.2, 7.4] | [22.1, 0.4, -3.0] | [15.4, 0.5, -3.4] | [20.9, 0.5, -4.1] |
| bild2 | Kopf rote Zone (Kuppe oben) box 418-441 x 193-211, R-G>=25; Maske 236 px, bbox x420-439 y196-209, 2 px erodiert | 236 | [96.0, 57.2, 60.9] | [17.4, 15.9, 17.8] | [28.8, 17.8, 5.0] | [23.2, 19.7, 6.1] | - |
| bild2 | Buckel/Ruecken hell box 372-414 x 195-227, Lum>=80; Maske 342 px, bbox x374-412 y197-225, 2 px erodiert | 342 | [163.3, 136.4, 141.3] | [30.9, 30.6, 30.6] | [59.4, 11.0, 1.2] | [56.3, 12.0, 1.3] | - |
| bild2 | Augenumfeld box 440-461 x 224-249; Maske 396 px, bbox x442-459 y226-247, 2 px erodiert | 396 | [77.7, 67.9, 69.5] | [27.3, 27.1, 27.2] | [29.8, 4.5, 0.5] | [24.0, 5.0, 0.6] | - |
| bild2 | Auge (heller Fleck, Blob-Schwelle 77,8); Maske 137 px, bbox x444-458 y229-244 | 137 | [109.4, 99.7, 101.4] | [17.2, 17.7, 17.2] | [43.2, 4.1, 0.5] | [38.7, 4.5, 0.5] | - |
| bild2 | Haare (dunkle Vorderkante, Blob-Schwelle 56,8); Maske 334 px, bbox x450-470 y227-257 | 334 | [47.3, 43.7, 40.6] | [9.0, 8.8, 7.4] | [18.1, 1.0, 2.6] | [10.9, 1.2, 3.0] | - |
| bild2 | Kopf unterhalb Auge box 436-461 x 250-262; Maske 198 px, bbox x438-459 y252-260, 2 px erodiert | 198 | [44.3, 40.2, 40.8] | [6.9, 5.5, 5.3] | [16.6, 2.0, 0.3] | [9.2, 2.3, 0.3] | - |
| bild2 | Oberarm (nah) box 345-394 x 250-284; Maske 1426 px, bbox x347-392 y252-282, 2 px erodiert | 1426 | [77.6, 60.3, 60.2] | [48.9, 36.7, 39.0] | [27.3, 7.5, 3.0] | [21.3, 8.5, 3.1] | - |
| bild2 | Unterarm/Klaue box 340-371 x 285-321; Maske 924 px, bbox x342-369 y287-319, 2 px erodiert | 924 | [41.6, 33.1, 31.9] | [12.2, 9.3, 10.0] | [13.7, 3.8, 2.3] | [6.1, 3.9, 2.1] | - |
| bild2 | Jeans box 395-434 x 296-329; Maske 1080 px, bbox x397-432 y298-327, 2 px erodiert | 1080 | [40.9, 42.4, 47.1] | [23.8, 21.0, 21.7] | [17.3, 0.5, -3.3] | [9.9, 0.7, -3.7] | - |
| bild2 | Bezug Wand links box 40-119 x 150-299; Maske 11096 px, bbox x42-117 y152-297, 2 px erodiert | 11096 | [156.0, 156.9, 161.1] | [39.9, 38.5, 40.7] | [64.7, 0.5, -2.3] | [62.1, 0.5, -2.5] | - |
| bild2 | Bezug Boden box 30-199 x 380-439; Maske 9296 px, bbox x32-197 y382-437, 2 px erodiert | 9296 | [64.6, 65.3, 71.0] | [9.3, 9.5, 9.5] | [27.8, 1.0, -3.5] | [21.7, 1.1, -3.9] | - |
| bild2 | Schwanz nahe Ruecken (s 0-15 % Beginn->Knick), Maske ero2; Maske 374 px, bbox x342-356 y208-247 | 374 | [44.1, 25.6, 29.7] | [8.7, 9.2, 8.4] | [11.6, 10.0, 1.0] | [4.5, 8.8, 1.1] | - |
| bild2 | Schwanz breiter Teil (s 35-65 % Beginn->Knick), Maske ero2; Maske 1114 px, bbox x283-326 y220-260 | 1114 | [53.4, 42.8, 43.6] | [16.4, 20.5, 20.2] | [18.6, 5.0, 1.3] | [11.5, 5.7, 1.6] | - |
| bild2 | Schwanz am Knick (|s-s_Knick|<=10 px), Maske ero1; Maske 45 px, bbox x275-286 y303-312 | 45 | [58.4, 54.3, 52.8] | [15.2, 16.5, 16.2] | [23.1, 1.4, 1.6] | [16.5, 1.6, 1.8] | - |
| bild2 | Schwanz Spitze (letzte 20 % Knick->Ende), Maske ero1; Maske 133 px, bbox x282-302 y337-352 | 133 | [51.6, 44.0, 44.2] | [12.1, 11.2, 11.5] | [18.8, 3.5, 1.2] | [11.7, 4.0, 1.4] | - |

### Port V3 r2 (Leisten SF_belege/v3, in 640er-Koordinaten zurueckgerechnet, 2 px erodiert, Overlay-Farben ausgeschlossen)

| Leiste | Region | Box | n | RGB | Std | Lab |
|---|---|---|---|---|---|---|
| vorn_h118 | Kopf-Kuppe | [370, 160, 396, 172] | 176 | [90.7, 68.7, 69.2] | [20.9, 23.2, 21.4] | [31.5, 9.6, 3.3] |
| vorn_h118 | Augenumfeld | [370, 170, 390, 185] | 176 | [85.2, 73.1, 76.5] | [20.0, 19.9, 20.8] | [32.4, 5.7, -0.1] |
| vorn_h118 | Kopf B-rechts neben Auge (Haare-Lage) | [386, 168, 398, 184] | 96 | [86.0, 71.1, 73.5] | [14.8, 14.1, 14.1] | [31.8, 6.7, 0.9] |
| vorn_h118 | Maul | [373, 188, 395, 203] | 198 | [32.5, 15.4, 17.7] | [18.2, 11.4, 12.3] | [6.4, 8.9, 1.8] |
| vorn_h118 | Brust rot (R-G>=30) | [395, 208, 430, 240] | 287 | [100.0, 39.0, 38.6] | [19.3, 18.8, 16.7] | [24.7, 27.6, 14.1] |
| vorn_h118 | Brust hell | [380, 250, 420, 280] | 936 | [85.4, 70.9, 67.6] | [23.3, 23.9, 23.5] | [31.5, 5.5, 4.3] |
| vorn_h118 | Oberarm/Schulter B-links | [300, 195, 330, 235] | 936 | [86.6, 48.7, 48.0] | [36.7, 24.1, 22.9] | [24.9, 17.1, 7.9] |
| vorn_h118 | Unterarm B-links | [300, 265, 326, 300] | 682 | [43.8, 30.1, 30.6] | [19.6, 17.8, 18.8] | [13.0, 6.7, 2.3] |
| vorn_h118 | Jeans | [362, 300, 395, 330] | 754 | [27.3, 39.4, 49.2] | [17.4, 21.0, 25.2] | [15.2, -2.1, -8.1] |
| vorn_h118 | Schwanz vorn | [322, 350, 341, 372] | 270 | [36.1, 36.5, 33.7] | [10.5, 10.6, 11.5] | [14.3, -0.9, 1.8] |
| kopf_h85 | Kopf oben grau (seitlich) | [442, 188, 463, 204] | 204 | [102.8, 88.9, 92.7] | [31.4, 29.5, 30.3] | [39.2, 6.4, -0.1] |
| kopf_h85 | Maul seitlich | [442, 206, 462, 216] | 96 | [107.2, 66.7, 66.4] | [41.0, 32.2, 33.4] | [32.9, 17.5, 7.5] |
| kopf_h85 | Schulterwunde rot | [408, 215, 440, 250] | 868 | [122.2, 44.0, 43.4] | [29.4, 25.7, 25.0] | [29.7, 34.0, 18.4] |
| seite_h83 | Buckel/Ruecken hell | [372, 190, 405, 212] | 522 | [95.8, 67.1, 69.9] | [21.8, 22.5, 23.8] | [31.7, 12.9, 3.2] |
| seite_h83 | Oberarm (nah) | [355, 240, 385, 262] | 468 | [59.7, 48.2, 46.1] | [25.3, 20.8, 19.9] | [21.1, 4.8, 3.3] |
| seite_h83 | Jeans | [350, 292, 372, 318] | 396 | [23.3, 35.6, 45.2] | [15.4, 20.4, 23.3] | [13.3, -2.2, -8.0] |
| seite_h83 | Schwanz nahe Ruecken | [322, 200, 345, 214] | 190 | [64.0, 43.8, 33.3] | [17.8, 21.5, 8.4] | [20.0, 7.8, 10.5] |
| seite_h83 | Schwanz breiter Teil | [282, 215, 302, 233] | 224 | [58.7, 41.1, 37.0] | [17.6, 18.8, 10.8] | [18.6, 7.5, 5.9] |
| seite_h83 | Schwanz Spitze | [228, 282, 241, 296] | 90 | [60.4, 43.8, 55.4] | [18.2, 12.4, 18.5] | [20.2, 10.0, -4.4] |

### Delta E 2000 Original (bereinigt) gegen Port V3 r2

| Original | Port | Lab Original bereinigt | Lab Port | dE00 bereinigt | dE00 roh | dL (Port-Orig) | C Orig / Port | h Orig / Port |
|---|---|---|---|---|---|---|---|---|
| bild1: Kopf-Kuppe (Oberkopf B-links der roten Zone) box 372-388 x 147-157 | vorn_h118: Kopf-Kuppe | [19.5, 12.2, 3.6] | [31.5, 9.6, 3.3] | 9.1 | 7.9 | 12.0 | 12.7 / 10.2 | 16 / 19 |
| bild1: Augenumfeld box 371-391 x 157-177 | vorn_h118: Augenumfeld | [27.0, 7.9, -1.1] | [32.4, 5.7, -0.1] | 4.8 | 4.7 | 5.4 | 8.0 / 5.7 | 352 / 359 |
| bild1: Auge (heller Fleck, Blob-Schwelle 72,1) | vorn_h118: Augenumfeld | [39.9, 8.0, -1.2] | [32.4, 5.7, -0.1] | 6.7 | 2.9 | -7.5 | 8.1 / 5.7 | 352 / 359 |
| bild1: Haare (dunkles Feld, Blob-Schwelle 31,2) | vorn_h118: Kopf B-rechts neben Auge (Haare-Lage) | [3.2, 4.4, 0.7] | [31.8, 6.7, 0.9] | 19.4 | 16.1 | 28.6 | 4.5 / 6.8 | 9 / 8 |
| bild1: unter dem Kopf/Kinn box 378-411 x 183-194 | vorn_h118: Maul | [8.1, 10.6, 3.8] | [6.4, 8.9, 1.8] | 2.3 | 4.3 | -1.7 | 11.3 / 9.1 | 20 / 11 |
| bild1: Brust rot box 372-424 x 195-234, R-G>=15 | vorn_h118: Brust rot (R-G>=30) | [10.5, 19.1, 7.7] | [24.7, 27.6, 14.1] | 10.9 | 11.6 | 14.2 | 20.6 / 31.0 | 22 / 27 |
| bild1: Brust hell box 372-424 x 195-234, R-G<15 und Lum>=45 | vorn_h118: Brust hell | [24.8, 5.9, 1.6] | [31.5, 5.5, 4.3] | 5.6 | 5.9 | 6.7 | 6.1 / 7.0 | 15 / 38 |
| bild1: Oberarm/Schulter B-links box 305-349 x 165-214 | vorn_h118: Oberarm/Schulter B-links | [27.2, 5.8, 3.2] | [24.9, 17.1, 7.9] | 9.7 | 11.4 | -2.3 | 6.6 / 18.8 | 29 / 25 |
| bild1: Jeans box 372-439 x 262-299 | vorn_h118: Jeans | [4.3, -0.6, -1.3] | [15.2, -2.1, -8.1] | 9.0 | 6.7 | 10.9 | 1.4 / 8.4 | 245 / 256 |
| bild1: Schwanz vorn (Band links, |I-bg*k|>=30, Lum<60) | vorn_h118: Schwanz vorn | [0.6, -0.0, -0.2] | [14.3, -0.9, 1.8] | 8.7 | 5.8 | 13.7 | 0.2 / 2.0 | 270 / 117 |
| bild2: Kopf rote Zone (Kuppe oben) box 418-441 x 193-211, R-G>=25 | kopf_h85: Kopf oben grau (seitlich) | [23.2, 19.7, 6.1] | [39.2, 6.4, -0.1] | 16.7 | 12.9 | 16.0 | 20.6 / 6.4 | 17 / 359 |
| bild2: Auge (heller Fleck, Blob-Schwelle 77,8) | kopf_h85: Kopf oben grau (seitlich) | [38.7, 4.5, 0.5] | [39.2, 6.4, -0.1] | 2.2 | 4.4 | 0.5 | 4.5 / 6.4 | 6 / 359 |
| bild2: Buckel/Ruecken hell box 372-414 x 195-227, Lum>=80 | seite_h83: Buckel/Ruecken hell | [56.3, 12.0, 1.3] | [31.7, 12.9, 3.2] | 23.0 | 26.5 | -24.6 | 12.1 / 13.3 | 6 / 14 |
| bild2: Oberarm (nah) box 345-394 x 250-284 | seite_h83: Oberarm (nah) | [21.3, 8.5, 3.1] | [21.1, 4.8, 3.3] | 3.9 | 5.4 | -0.2 | 9.0 / 5.8 | 20 / 34 |
| bild2: Jeans box 395-434 x 296-329 | seite_h83: Jeans | [9.9, 0.7, -3.7] | [13.3, -2.2, -8.0] | 5.6 | 5.8 | 3.4 | 3.8 / 8.3 | 281 / 255 |
| bild2: Schwanz nahe Ruecken (s 0-15 % Beginn->Knick), Maske ero2 | seite_h83: Schwanz nahe Ruecken | [4.5, 8.8, 1.1] | [20.0, 7.8, 10.5] | 12.6 | 10.0 | 15.5 | 8.9 / 13.1 | 7 / 53 |
| bild2: Schwanz breiter Teil (s 35-65 % Beginn->Knick), Maske ero2 | seite_h83: Schwanz breiter Teil | [11.5, 5.7, 1.6] | [18.6, 7.5, 5.9] | 6.0 | 4.4 | 7.1 | 5.9 / 9.5 | 16 / 38 |
| bild2: Schwanz Spitze (letzte 20 % Knick->Ende), Maske ero1 | seite_h83: Schwanz Spitze | [11.7, 4.0, 1.4] | [20.2, 10.0, -4.4] | 9.4 | 8.0 | 8.5 | 4.2 / 10.9 | 19 / 336 |

### Schwanzbreiten (Maske, senkrecht zur Maskenachse; H2 = 253 px)

| Strecke | Anteil | Punkt Standard | Breite Standard px (H) | Breite RG>=4/diff>=30 px | Breite RG>=6/diff>=50 px |
|---|---|---|---|---|---|
| Knick -> Ende | 0.1 | [283.0, 308.0] | 6.0 (0.024) | 6.0 | 1.0 |
| Knick -> Ende | 0.3 | [282.0, 316.0] | 8.0 (0.032) | 9.0 | 1.0 |
| Knick -> Ende | 0.5 | [289.0, 322.0] | 9.0 (0.036) | 9.0 | 1.0 |
| Knick -> Ende | 0.7 | [289.0, 331.0] | 9.0 (0.036) | 12.0 | 1.0 |
| Knick -> Ende | 0.9 | [290.0, 341.0] | 7.0 (0.028) | 7.0 | 9.0 |
| Knick -> Beginn ("nach oben") | 0.1 | [281.0, 291.0] | 7.0 (0.028) | 8.0 | 1.0 |
| Knick -> Beginn ("nach oben") | 0.3 | [290.0, 269.0] | 17.0 (0.067) | 21.5 | 18.0 |
| Knick -> Beginn ("nach oben") | 0.5 | [301.0, 247.0] | 31.5 (0.125) | 32.0 | 32.5 |
| Knick -> Beginn ("nach oben") | 0.7 | [325.0, 236.0] | 38.0 (0.150) | 37.0 | 27.0 |
| Knick -> Beginn ("nach oben") | 0.9 | [346.0, 231.0] | 55.0 (0.217) | 55.0 | 53.0 |
