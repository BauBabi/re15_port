# Tor ROOM1170 - Vermessung ueber die Raumkameras

Stand 2026-09-28. Werkzeug `re15_port/tools/tor/tor_vermessung.py` (nutzt `tor_kamera.py`,
`tor_kanten.py`), Messwerte `build/tor_1170/vermessung.json`, Kontrollbilder
`build/tor_1170/vermessung_*.png`. Nichts am Port wurde geaendert oder gebaut.

Kennzeichnung: `[DATEI]` = Bytes aus einer Datei, `[MESS]` = Messung im Bild (Befehl am Ende),
`[RECH]` = aus beidem gerechnet, `[ANNAHME]` = gesetzt, nicht gemessen.

Bezeichnungen: **Bereich A** = Landeplatz-Seite (Cut 0), **Bereich B** = Laufsteg-Seite (Cut 11,
Cut 12). **hi / lo** = Torseite mit grossem / kleinem Welt-x (Cut 0: hi = Bild rechts;
Cut 11 und 12: hi = Bild links). **u** laengs des Tors ab der Achse des hi-Rahmenrohrs nach lo,
**v** Hoehe ueber dem Boden y = -7200, **w** Dicke.

---

## 0. Ergebnis in einer Tabelle

Masse des Tors in Raumwelt-Einheiten (Verhaeltnisse aus Cut 12, Massstab aus Cut 0 und Cut 11):

| Bauteil | u von .. bis | v von .. bis | Mass | Guete |
|---|---|---|---|---|
| Rahmen, Aussenkontur | -85 .. 1916 | 269 .. 1543 | 2001 x 1275 | Hoehe gemessen; lo-Aussenkante `[ANNAHME]` Symmetrie |
| Rahmen, lichte Oeffnung | 85 .. 1748 | 387 .. 1392 | 1663 x 1005 | gemessen |
| Rohr oben | | 1392 .. 1543 | Dicke 152 | beide Kanten gegen Himmel |
| Rohr unten | | 269 .. 387 | Dicke 118 (hell: 70) | Oberkante dunkel vor dunkel |
| Rohr hi (Angelseite?) | -85 .. 85 | | Dicke 170 | beide Kanten gegen Himmel |
| Rohr lo | 1748 .. 1916 | | sichtbar 67 | nur Innenflanke sichtbar |
| Ecke oben/hi | | | Radius aussen 308, Mittellinie 221 | Kreis an gerade Kanten angelegt |
| Schild | 294 .. 1537 | 471 .. 1229 | 1243 x 759 | gemessen |
| Texttafel (Laufsteg-Seite) | 445 .. 1354 | 557 .. 1128 | 909 x 571 | gemessen |
| Schraffurrand | | | hi 151, lo 183, oben 101, unten 86 | gemessen |
| Aufhaengung hi / lo | 370 .. 555 / 1282 .. 1429 | 1229 .. 1392 | 185 x 162 / 148 x 162 | gemessen |
| Fuss hi / lo | 383 .. 551 / 1330 .. 1460 | 387 .. 471 | 168 x 84 / 130 x 84 | gemessen |
| Bodenfreiheit | | 0 .. 269 | 269 | gemessen |
| Pfosten hi (fest) | Achse -207 (Cut 0) / -213 (Cut 11) | 0 .. ~1800 | Breite 94 .. 125 | Fuss auf dem Boden gemessen |
| Pfosten lo (fest) | Achse 1738 (Cut 0) / 1932 (Cut 11, helle Flanke) | 0 .. ~1800 | Breite 104 | Widerspruch Cut 0 / Cut 11, s. 7 |

Massstab Cut 12: **s = 17,49 +- 1,09 Einheiten je Bildpunkt** (6 %). Alle Tabellenwerte tragen
diese 6 %. Torebene: **zA = 15110, zB = -27591, +-100**.

---

## 1. Kamerasaetze `[DATEI]`

`re15_port/shared_assets/PSX/STAGE1/ROOM1170.RDT`, 303564 B. Kopf @0x00 `00 0d 06 00 00 00 00 38`
(nCut = 13), Zeiger @0x24 = 0x60, 32 B je Cut (u16 flag, u16 fov, s32 Ort[3], s32 Ziel[3], u32 pri).

| Cut | Offset | rohe Bytes |
|---|---|---|
| 0 | 0x060 | `00 00 3c 68 6e fb ff ff 5e d2 ff ff d2 0f 00 00 aa 10 00 00 3e ec ff ff e4 45 00 00 6c 06 00 00` |
| 11 | 0x1C0 | `00 00 3c 68 0a c3 ff ff dc d2 ff ff 04 ae ff ff 2e cc ff ff dc ed ff ff 04 8a ff ff 40 0a 00 00` |
| 12 | 0x1E0 | `01 00 3c 68 fc d5 ff ff a8 e3 ff ff a4 a4 ff ff 74 d1 ff ff 1c db ff ff 94 8b ff ff 44 0a 00 00` |

| Cut | flag | fov | H = fov>>7 | Ort | Ziel | pri |
|---|---|---|---|---|---|---|
| 0 | 0 | 26684 | 208 | (-1170, -11682, 4050) | (4266, -5058, 17892) | 0x66C |
| 11 | 0 | 26684 | 208 | (-15606, -11556, -20988) | (-13266, -4644, -30204) | 0xA40 |
| 12 | 1 | 26684 | 208 | (-10756, -7256, -23388) | (-11916, -9444, -29804) | 0xA44 |

Alle drei stimmen mit den Werten des Auftrags ueberein. pri: @0x66C, @0xA40 und @0xA44 steht
jeweils `ff ff ff ff` - keiner der drei Cuts hat Masken, also auch keine Tiefenangabe zum Tor.

**18er-Raster** `[RECH]`: In den Cuts 0 bis 11 sind alle sechs Koordinaten durch 18 teilbar
(Ausfuhr aus dem Modellierprogramm mit Faktor 18). **Cut 12 ist der einzige Satz, der das nicht
erfuellt** (-10756 / 18 = -597,6; -7256 = -7200 - 56). Der Satz ist von Hand gesetzt. Folge in 6.

Tuer-Saetze `[DATEI]`: Slot 0 @0x1206 `3b 00 02 31 04 00 dc 05 40 38 34 08 a4 06 42 d2 e0 e3 7c 98 8d 0b 00 17 0b 04`,
Slot 6 @0x135A `3b 06 02 31 04 00 5c d1 de 90 d6 06 b0 04 fc 08 e0 e3 1d 38 00 04 00 17 00 04` -
Rechtecke und Spawns wie im Auftrag.

## 2. Projektion und Probe

Blickmatrix ganzzahlig wie `camera_common.c:77-110` (SquareRoot0, abschneidende Division, Q12),
Projektion `sx = 160 + H*vx/vz`, `sy = 120 + H*vy/vz` (`platform/pc/main.c:5277-5278`).

| Cut | R (Q12) | R*ey | Fluchtpunkt X | Fluchtpunkt Y |
|---|---|---|---|---|
| 0 | 3821 0 -1500 / -612 3739 -1557 / 1369 1669 3487 | (0, 0,9128, 0,4075) | (740,5, 27,0) | (160, 586,0) |
| 11 | -3975 0 -1009 / -595 3318 2343 / 817 2415 -3220 | (0, 0,8101, 0,5896) | (-852,0, -31,5) | (160, 405,8) |
| 12 | -4033 0 729 / -233 3885 -1284 / -692 -1304 -3826 | (0, 0,9485, -0,3184) | (1372,2, 190,0) | (160, -499,7) |

Zweite Komponente von R*ey ist in allen drei Cuts positiv: **Welt-unten (+y) ist Bild-unten.**
Die ganzzahlige Port-Matrix weicht von derselben Konstruktion in Gleitkomma um hoechstens
0,31 px ab (am Tor). Bild rechts ist in Cut 0 Welt +x, in Cut 11 und 12 Welt -x.

Probe (`vermessung_probe_cutNN.png`, angesehen): In Cut 0 liegt der Kreis der SCA-Zelle 6
(@0xAE0, Mitte (988, 14452), r 756) auf dem dunklen Sockel der Lampe links vor dem Tor, die
Vorderkante der Zelle 2 (z = 15046) laeuft die ganze Bildbreite parallel zum Gelaender. In Cut 11
laeuft die Kante z = -27134 (Zelle 27) auf der Oberkante des dunklen Traegers, der Laufsteg
(Gitterrost) fuellt genau den Streifen zwischen Zelle 27 und Zelle 28. Das Tuer-Rechteck liegt in
beiden Cuts unter dem Ausschnitt des Nutzers. In Cut 12 faellt die ganze Bodenebene auf die Zeilen
192..193 (Kamera 56 ueber dem Boden) - wie im Auftrag vermerkt unbrauchbar.

## 3. Bereich A und Bereich B sind dieselbe Szene, verschoben `[MESS]`

Beide Bereiche zeigen denselben Landeplatz von zwei Seiten. Der gelbe Randstreifen des
Landeplatzes (auf den Boden gemalt, laeuft laengs x) ist in Cut 0 und in Cut 11 zu sehen:

| Kante | Cut 0 (A), z am Boden | Cut 11 (B), z am Boden | Differenz |
|---|---|---|---|
| torseitig | 8157 (Bildpunkt 200,12 / 189,19; Streuung 0,05 px) | -34537 (190,08 / 51,77; 0,07 px) | -42693 |
| platzseitig | 7400 (220,17 / 205,85; 0,03 px) | -35309 (189,92 / 48,07; 0,04 px) | -42709 |
| Breite | 757 | 773 | |

**T.z = -42701** (die beiden Kanten streuen um +-8), T.y = 0. Gegenprobe Streifenbreite: 757
gegen 773 (2 %). Setzt man stattdessen die beiden SCA-Grenzen gleich (Zelle 2 in A und der Steg
zwischen Zelle 27 und 28 in B sind beide 2700 tief), kaeme -42180 heraus; mit dem gemessenen T.z
bleibt zwischen den beiden begehbaren Flaechen ein Streifen von 521.
T.x = -13810 aus Pfosten hi und Rahmenrohr hi (-13807 / -13813); die Bodenfugen laengs z geben
-14100 / -14000 / -13940 (Fugen A bei x = -1540, 480, 2560; B bei -15640, -13520, -11380).
**T.x ist auf +-150 unsicher**; es wird fuer kein Tormass gebraucht (u zaehlt in jedem Cut ab dem
dort gemessenen hi-Rahmenrohr).

SCA-Grenzen `[DATEI]`: Zelle 2 @0xAB0 `a0 71 8c 0a 8a cb c6 3a 01 ff 00 43` -> A gesperrt ab
z = 15046. Zelle 27 @0xBDC `27 31 40 06 47 ab c2 8f 01 ff 00 43` -> B gesperrt bis z = -27134
= 15567 in A-Koordinaten. Zwischen den begehbaren Flaechen liegt ein Streifen von **521**; in ihm
stehen Gelaender und Tor.

## 4. Torebene `[MESS]` `[RECH]`

Das Gelaender laeuft laengs x, die Torebene ist z = const.

**4.1 Schnitt zweier Sehebenen.** Jede waagrechte Linie legt in Cut 0 und in Cut 11 je eine Ebene
durch den Kameraort fest; ihr Schnitt (nach Verschiebung um T) gibt Hoehe und z zugleich.

| Linie | Cut 0 Bildpunkt | Cut 11 Bildpunkt | Hoehe | zA | 0,3 px Fehler |
|---|---|---|---|---|---|
| Holm oben, nah | 101,99 / 83,58 | 167,72 / 62,71 | 1794 | 15068 | -16 / +18 |
| Holm oben, fern | 47,52 / 87,89 | 260,06 / 71,32 | 1809 | 15112 | -16 / +18 |
| Holm mitte | 75,20 / 99,08 | 235,15 / 90,43 | 1094 | 15149 | -14 / +16 |
| Tor, Rohr oben | 148,01 / 84,41 | 98,00 / 66,41 | 1473 | 15221 | -15 / +16 |
| Tor, Rohr unten | 147,96 / 104,49 | 99,06 / 91,25 | 324 | 15037 | -12 / +14 |

**4.2 Pfostenfuesse in Cut 0.** Die hellen Endpfosten enden bei y = 111,57 (lo) und 106,98 (hi);
auf den Boden gelegt: zA = 15059 und 15107. **Die Pfosten stehen auf dem Boden y = -7200.**

**4.3 Ergebnis.** Mittel der sieben Werte 15108, Streuung 63 -> **zA = 15110, zB = -27591**.
Angesetzte Unsicherheit +-100 (eine um 4 % andere Brennweite in Cut 11 verschiebt den Schnitt um
100, eine um 200 andere T.z um 123).

**4.4 Die Unterkante des Tors steht NICHT auf dem Boden.** Verlangt man es, fordert Cut 0
zA = 15892 und Cut 11 zA = 14500 - Widerspruch 1392. Beide Bilder vertragen sich nur mit einem
unteren Rohr in Hoehe 324 (Schnitt) bzw. 340 / 365 (Cut 0 / Cut 11 auf der Ebene).

**4.5 Bordstein.** Hinter dem Gelaender liegt in Cut 0 ein helles Band (y 104,62..111,08 bei
x = 122; 98,11..103,81 bei x = 175). Seine Unterkante auf den Boden gelegt: zA = 15372 / 15382,
Hoehe dort 396 / 383. Es liegt 260 hinter der Torebene, auf der Laufsteg-Seite. Unter dem Tor ist
es in Cut 0 nicht zu sehen. (Deutung als Bordstein: teilweise belegt.)

**4.6 Empfindlichkeit: Ebene um +100 verschoben.**

| Quelle | Breite | Hoehe | Oberkante | Unterkante |
|---|---|---|---|---|
| Cut 0 allein | +0,90 % (+14,5 auf 1609) | +0,90 % | -27 | -38 |
| Cut 11 allein | -1,51 % (-27,4 auf 1811) | -1,51 % | +44 | +61 |
| Cut 12 mit RDT-Kamera | -2,38 % | -2,38 % | -35 | +1 |
| **Modell (Cut 12 mal s)** | **0,00 %** (s 17,49 / 17,49 / 17,49) | **0,00 %** | +6 | +6 |

Im Modell heben sich die Anker aus Cut 0 und Cut 11 auf: die Tormasse haengen nicht mehr an der
Ebene, nur die absolute Hoehe verschiebt sich um 6 je 100.

## 5. Messung in Cut 0 und Cut 11 (RDT-Kamera, Rueckprojektion auf die Ebene) `[MESS]`

Massstab am Tor: Cut 0 61,2 / 60,3 Einheiten je px (waagrecht / senkrecht), vz 12098.
Cut 11 43,6 / 43,0, vz 8067. Bezugszeile Cut 0 y = 94, Cut 11 y = 80; Bezugsspalte 147 bzw. 98.
Nummern = Beschriftung in `vermessung_merkmale_cut00.png` / `_cut11.png`.

| Cut 0 | Bild | u bzw. v | | Cut 11 | Bild | u bzw. v |
|---|---|---|---|---|---|---|
| 1 Pfosten hi | x 164,08 | -207 (B 135) | | 3 Pfosten hi (dunkel) | x 74,15 | -213 (B 110) |
| 3 Rahmenrohr hi | x 160,85 | 0 (B 111) | | 4 Rahmenrohr hi (dunkel) | x 78,74 | 0 (B 119) |
| 13 Tafel hi-Kante | x 157,14 | 235 | | 8 Schild hi-Kante | x 83,26 | 207 |
| 14 Tafel lo-Kante | x 140,09 | 1274 | | 10 Schild lo-Kante | x 113,61 | 1530 |
| 10 Schild lo-Kante | x 137,94 | 1399 | | 1 dunkle Linie lo | x 120,63 | 1819 (B 150) |
| 2 Pfosten lo | x 132,08 | 1738 (B 106) | | 2 helle Flanke lo | x 123,39 | 1932 (B 72) |
| 7 Rohr oben | y 84,45 | 1507 (D 128) | | 5 Rohr oben (dunkel) | y 66,45 | 1422 (D 106) |
| 11 Schild Oberkante | y 88,03 | 1299 | | 6 Rohr unten (hell) | y 91,19 | 365 (D 110) |
| 15 Tafel Oberkante | y 91,45 | 1097 | | | | |
| 16 Tafel Unterkante | y 99,23 | 629 | | | | |
| 12 Schild Unterkante | y 101,80 | 471 | | | | |
| 8 Rohr unten | y 103,91 | 340 | | | | |
| heller Fleck lo | 134,08..137,38 / 91,02..94,03 | u 1432..1614, v 1008..1203 | | | | |

## 6. Cut 12: die RDT-Kamera beschreibt das Bild nicht `[MESS]`

| Pruefung | RDT-Kamera sagt | im Bild gemessen |
|---|---|---|
| Fluchtpunkt der Senkrechten | (160, -500) | (174, -1203) |
| Steigung dx/dy, Rahmenrohr hi rechts, x = 119,74 | -0,0614 | -0,0276 +- 0,0006 |
| Steigung dx/dy, Pfosten hi links, x = 97,60 | -0,0961 | -0,0072 +- 0,0009 |
| Steigung dx/dy, Schild lo, x = 203,41 | +0,0654 | +0,0349 +- 0,0053 |
| Fluchtpunkt der Waagrechten | (1372, 190,0) | (1232, 190,4) |
| Steigung dy/dx, Rohr oben aussen | +0,0592 | +0,0650 +- 0,0010 |
| Einheiten je px auf der Torebene | 21,03 / 20,54 | 17,49 +- 1,09 |
| Hoehe Rohr unten (Unterkante) | 27 | 269 |
| 18er-Raster | - | nicht erfuellt |

Die Senkrechten laufen im Bild nur halb so stark zusammen wie vorhergesagt, das Tor ist im Bild
**1,20-mal groesser** als die RDT-Kamera es zeichnet, und sie stellt das Tor 240 tiefer. Zu den
gemessenen Fluchtpunkten gehoert (Bildmitte 160/120, quadratische Pixel) **H = 280 statt 208**.
Der Satz von Cut 12 ist eine von Hand gesetzte Naeherung; **Masse aus Cut 12 ueber die
RDT-Kamera waeren 20 % zu gross.**

Deshalb wird Cut 12 so ausgewertet: Abbildung Bild -> Torebene aus den beiden im Bild gemessenen
Fluchtpunkten (Klasse `Ebene12`, Bezugspunkt 164,5 / 155), Massstab s aus Ankern, die in Cut 0 und
Cut 11 mit den verlaesslichen Kameras gemessen sind:

| Anker | Quelle | Welt | Bild | s |
|---|---|---|---|---|
| Rohr oben Mitte -> Rohr unten Mitte | Cut 0 | 1167 | 65,15 | 17,90 |
| Rohr oben Mitte -> Schild Unterkante | Cut 0 | 1035 | 57,00 | 18,17 |
| Schild Ober- -> Unterkante | Cut 0 | 828 | 43,38 | 19,08 |
| Rohr oben Mitte -> Rohr unten Mitte | Cut 11 | 1058 | 65,15 | 16,23 |
| Rohr oben Mitte -> Rohr unten Mitte | Schnitt 4.1 | 1149 | 65,15 | 17,64 |
| Schildbreite | Cut 11 | 1322 | 71,08 | 18,60 |
| Rahmenrohr hi -> Schild lo-Kante | Cut 11 | 1530 | 87,86 | 17,41 |
| Rahmenrohr hi -> Schild lo-Kante | Cut 0 | 1399 | 87,86 | 15,93 |
| Rahmenrohr hi -> Tafel lo-Kante | Cut 0 | 1274 | 77,39 | 16,46 |

senkrecht 17,81 +- 1,03, waagrecht 17,10 +- 1,18, **s = 17,49 +- 1,09**. Hoehenanker: Rohr oben
Mitte v = 1467 (Cut 0: 1507, Cut 11: 1422, Schnitt: 1473).

Merkmale in Cut 12 (Bildlage auf der Bezugssenkrechten x = 164,5 bzw. Bezugswaagrechten y = 155;
`vermessung_merkmale_cut12.png`):

| waagrecht | Bild-y | v | | senkrecht | Bild-x | u |
|---|---|---|---|---|---|---|
| Rohr oben aussen | 118,52 | 1543 | | Pfosten hi links (hell) | 97,56 | -265 |
| Rohr oben innen | 126,81 | 1392 | | Pfosten hi rechts | 107,01 | -118 |
| Schild oben | 135,76 | 1229 | | Rahmenrohr hi links | 109,09 | -85 |
| Tafel oben | 141,42 | 1128 | | Rahmenrohr hi Hellgrenze | 115,73 | 20 |
| Tafel unten | 174,21 | 557 | | Rahmenrohr hi rechts | 119,78 | 85 |
| Schild unten | 179,30 | 471 | | Schild hi | 132,59 | 294 |
| Rohr unten, dunkle Oberkante | 184,27 | 387 | | Tafel hi | 141,69 | 445 |
| Rohr unten, helle Oberkante | 187,13 | 339 | | Tafel lo | 193,30 | 1354 |
| Rohr unten unten | 191,35 | 269 | | Schild lo | 203,10 | 1537 |
| | | | | heller Streifen lo links | 214,16 | 1748 |
| | | | | heller Streifen lo rechts | 217,63 | 1815 |

Weitere Teile: Aufhaengung (dunkle Laschen, Zeilen 129..133) x 137,20..148,22 und
189,39..197,37; Fuesse (Zeilen 181..183) x 138..148 und 192..199; heller Block x 204..213,
y 180..187; Ecke oben/hi Kreis aussen r 17,60 px (rms 0,44, 21 Punkte), innen r 15,35 px
(rms 0,65); Rohrdicke am geraden Stueck 9,97 px.

Schraffur (Helligkeit, Autokorrelation und Versatz zweier Abtastlinien):

| Rand | Periode laengs des Rands | Winkel Streifen gegen Randrichtung | Streifenbreite senkrecht |
|---|---|---|---|
| hi | 7,40 px = 129 | 72,7 Grad | 62 |
| lo | 7,05 px = 123 | 71,8 Grad | 59 |
| oben | 10,84 px = 190 | 47,9 Grad | 70 |
| unten | 11,55 px = 202 | 49,3 Grad | 77 |

Die Seitenraender zeigen flachere Streifen (18 Grad gegen die Waagrechte) als Ober- und
Unterrand (48 Grad). Bei 9 bis 10 px Randbreite und Farbe nur in 2x2-Bloecken ist das
**teilweise belegt**; fuer die Textur genuegt: gelb und schwarz je 60..75 breit.

## 7. Gegenueberstellung der drei Cuts

| Mass | Cut 0 | Cut 11 | Cut 12 | Spanne |
|---|---|---|---|---|
| Pfosten hi -> Rahmenrohr hi | 207 | 213 | 192 | 22 |
| Rohr oben Mitte (v) | 1507 | 1422 | 1467 (Anker) | 84 |
| Schild Oberkante (v) | 1299 | - | 1229 | 70 |
| Schild Unterkante (v) | 471 | - | 471 | 1 |
| Rohr unten Mitte (v) | 340 | 365 | 328 | 37 |
| Rahmenhoehe Mitte-Mitte | 1167 | 1058 | 1140 | 109 |
| Schildhoehe | 828 | - | 759 | 69 |
| Schild lo-Kante (u) | 1399 | 1530 | 1537 | 137 |
| Schildbreite | 1107 | 1322 | 1243 | 215 |
| Rahmen lo (u) | - | 1819 | 1781 | 38 |
| Pfosten lo (u) | 1738 | 1932 | - | 193 |
| Tafel Hoehe | 468 | - | 571 | 103 |
| Tafel hi-Kante (u) | 235 | - | 445 | 210 |

**Abweichung 1, waagrecht in Cut 0.** Alle Hoehen stimmen auf 1 bis 84 ueberein. Waagrecht liegen
die lo-Merkmale in Cut 0 um 8 % zu nah am hi-Rohr (1399 statt 1530; 1738 statt ~1880). Mal 1,08
gerechnet passt Cut 0 zu Cut 11 und Cut 12 (1511, 1877). Die Bodenfugen zeigen dasselbe in
klein (Fugenabstand A 2020 / 2080, B 2120 / 2140). **Ursache OFFEN** - die waagrechte Skala von
Cut 0 und die von Cut 11 vertragen sich auf 5..8 % nicht; welche der beiden Kameras abweicht, ist
aus den Bildern nicht zu entscheiden. Deshalb traegt s die 6 %.

**Abweichung 2, Texttafel.** Cut 0 zeigt die ANDERE Seite des Schilds. Dort nimmt die helle
Tafel 57 % der Schildhoehe ein, in Cut 12 sind es 75 %. Bei 7,8 px Tafelhoehe in Cut 0 ist das
an der Grenze der Aufloesung; ob die beiden Seiten verschieden bedruckt sind, bleibt OFFEN.

**Abweichung 3, Rohrdicke.**

| Quelle | Rohr oben | Rohr hi | Art |
|---|---|---|---|
| Cut 0, D = px * vz / H | 2,22 * 11814 / 208 = 126 | 1,74 * 12365 / 208 = 103 | helles Band, Halbwertsbreite |
| Cut 11, D = px * vz / H | 2,70 * 7727 / 208 = 100 | 2,58 * 8309 / 208 = 103 | dunkles Band, Halbwertsbreite |
| Cut 12, px * s | 8,28 px -> 145 (Ebene: 152) | 10,70 px -> 187 (Ebene: 170) | beide Kanten gegen Himmel |
| Cut 12, nur helle Flanke | 6,00 px -> 105 | 4,05 px -> 71 | |
| Cut 12 mit RDT-Kamera | 8,28 * 4510 / 208 = 180 | 211 | ungueltig, s. 6 |

Ursache gefunden: In Cut 0 und Cut 11 ist das Rohr 1,7 bis 2,7 px breit; gemessen wird dort die
beleuchtete bzw. die dunkle Flanke, nicht der Umriss. Die helle Flanke in Cut 12 (105) passt zu
Cut 0 (126 / 103). Der Umriss gegen den Himmel ist nur in Cut 12 messbar:
**Rohrdurchmesser 152 (oben), 170 (hi)**; das untere Rohr zeigt 118, seine Oberkante liegt
dunkel vor dunkel.

## 8. Welche Seite sieht welcher Cut; Angel

| Cut | Kamera z | Torebene | sieht |
|---|---|---|---|
| 0 | 4050 (A) | 15110 | Landeplatz-Seite (Flaechennormale -z) |
| 11 | -20988 (B) | -27591 | Laufsteg-Seite (+z), von oben |
| 12 | -23388 (B) | -27591 | Laufsteg-Seite (+z), von unten |

**Das Schild ist beidseitig:** Schraffurrand und helle Tafel sind in Cut 0 (Gelbheit im lo-Rand
bis 71, Tafel Helligkeit bis 197) und in Cut 12 zu sehen. Schrift ist nur in Cut 12 aufgeloest.

**Angel: teilweise belegt, hi-Seite.** Scharniere sind in keinem Cut aufgeloest. Indizien:
(a) Im Spalt zwischen Schild und lo-Rohr ist in Cut 12 nur bis y = 142,60 (v 1144) Himmel zu
sehen, darunter schwarz; im hi-Spalt reicht der Himmel bis y = 175 (v 534). (b) In Cut 0 sitzt an
derselben Stelle ein sehr heller Fleck (u 1432..1614, v 1008..1203). (c) Am lo-Pfosten haengt ein
Kasten (Cut 12 px 211..231 / 112..137). Alles Zusaetzliche sitzt an der lo-Seite - das spricht fuer
Riegel lo, Angel hi. Von Cut 0 gesehen waere die Angel am RECHTEN Pfosten, von Cut 11 und 12 am
LINKEN.

## 9. Beweglich / fest

| Teil | Urteil | Bildbeleg |
|---|---|---|
| Rohrrahmen | beweglich | geschlossener Umriss mit runden Ecken; zwischen Pfosten hi und Rahmenrohr hi ist in Cut 12 Himmel zu sehen (x 107,0..109,1, Helligkeit 19,5), in Cut 0 eine dunkle Spalte bei x = 162,5 (Helligkeit 25 zwischen 204 und 182) |
| Schild, Laschen, Fuesse | beweglich | haengen am oberen Rohr, stehen auf dem unteren |
| Pfosten hi, Pfosten lo | fest | tragen die Holme des Gelaenders, reichen bis auf den Boden (4.2) |
| Gelaender, Bordstein | fest | |
| Kasten am lo-Pfosten | fest | sitzt ueber der Tor-Oberkante (v 1260..1745) |
| dunkle Flaeche im lo-Spalt, heller Fleck, heller Block lo | OFFEN | Torteil oder Hintergrund |

Spalt Pfosten hi - Rahmen, Kante zu Kante in Cut 12: 3,41 px = 54 bei y = 139 (dort sind beide
Kanten gemessen), 2,08 px = 33 bei y = 155 (linke Rohrkante dorthin verlaengert) - die beiden
Kanten laufen nicht parallel (OFFEN 8). Achse zu Achse 207 / 213 (Cut 0 / Cut 11).

## 10. ROOM1170.RDT enthaelt kein Modell des Tors `[DATEI]`

Kopf @0x02 nOmodel = 6, Zeigertabelle @0x200 (Zeiger @0x30):
`6c 1b 03 00 88 f3 00 00 6c 1b 03 00 88 f3 00 00 8c 9d 03 00 3c 09 01 00 8c 9d 03 00 70 43 01 00 8c 9d 03 00 fc 48 01 00 ac 1f 04 00 f0 4b 01 00`

| Modell | MD1 | Punkte / Dreiecke / Vierecke | Ausdehnung x / y / z | Gestalt |
|---|---|---|---|---|
| 0, 1 | @0xF388 | 132 / 16 / 98 | 1872 / 1896 / 1859 | Kugel |
| 2 | @0x1093C | 270 / 456 / 0 | 18032 / 5360 / 4073 | Rumpf (Hubschrauber) |
| 3 | @0x14370 | 24 / 48 / 0 | 16959 / 0 / 16959 | flache Scheibe |
| 4 | @0x148FC | 13 / 24 / 0 | 3311 / 0 / 3311 | flache Scheibe |
| 5 | @0x14BF0 | 361 / 560 / 0 | 836 / 2978 / 1035 | stehende Figur |

Keines hat die Masse des Tors (2001 x 1275, flach). Die Gestalt-Spalte ist Deutung aus der
Ausdehnung.

## 11. Massstabsanker

| Was | Wert | Beleg |
|---|---|---|
| Trefferkasten Spieler | halbe Hoehe 1530 -> 3060 | `[DATEI]` PSX.EXE Datei 0x64694 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` |
| Hueftgelenk Spieler | 1804 ueber dem Boden | `[DATEI]` PL00.PLD Datei 0xC68 `00 00 f4 f8 00 00` |
| Gelaender, Holm oben | 1802 | `[MESS]` 4.1 |
| Gelaender, Holm mitte | 1094 | `[MESS]` 4.1 |
| Tor, Oberkante | 1543 | `[RECH]` |

Der obere Holm liegt auf Hoehe des Hueftgelenks (1802 / 1804 = 1,00), die Tor-Oberkante bei 0,86
davon. Das passt zu einem huefthohen Gelaendertor; der Massstab ist plausibel.

## 12. OFFEN

1. Ursache der 8 % zwischen der waagrechten Skala von Cut 0 und der von Cut 11 / 12.
2. lo-Seite des Rahmens: Aussenkante, Ecken und volle Rohrdicke sind nicht messbar (schwarz vor
   schwarz). 1916 ist die Spiegelung des hi-Rohrs an der Schildmitte; die gemessene Innenkante
   (1748) trifft die gespiegelte (1745). Die beleuchtete Oberkante des oberen Rohrs laeuft in
   Cut 12 gerade bis x = 219 (u 1863) - eine Rundung wie an der hi-Seite ist dort nicht zu sehen.
3. Angel: Scharniere in keinem Cut aufgeloest (Abschnitt 8 ist ein Indiz, kein Beleg).
4. Dicke des Schilds (w): in keinem Cut messbar.
5. Ob die beiden Schildseiten gleich bedruckt sind.
6. Dunkle Flaeche im lo-Spalt, heller Fleck (Cut 0), heller Block lo: Torteil oder Hintergrund.
7. Winkel der Schraffur (Seitenraender 18 Grad, Ober-/Unterrand 48 Grad gegen die Waagrechte).
8. Der helle Pfosten links in Cut 12 (x 97,6..107,0, Breite 165) steht senkrecht im Bild, das
   Rahmenrohr daneben neigt sich (-0,0276); ob er der Gelaenderpfosten hi ist, ist nicht belegt.
   Seine Lage passt (u -192 gegen -207 / -213).
9. Wahre Kamera von Cut 12 (H 280 ist aus zwei Fluchtpunkten gerechnet, der senkrechte ist auf
   +-400 unsicher).
10. T.x auf +-150.

## 13. Nachmessen

```
python re15_port/tools/tor/tor_vermessung.py alles     # alles, schreibt JSON + 9 Bilder
python re15_port/tools/tor/tor_vermessung.py kamera    # Abschnitt 1, 2, 10
python re15_port/tools/tor/tor_vermessung.py versatz   # Abschnitt 3
python re15_port/tools/tor/tor_vermessung.py ebene     # Abschnitt 4
python re15_port/tools/tor/tor_vermessung.py cut12     # Abschnitt 6
```

Bauteil-Verzeichnis: `build/tor_1170/vermessung.json`, Schluessel `bauteile.teile[]` (Name, Art,
u, v, w, `px` je Cut als [x0, y0, x1, y1]); Gegenueberstellung unter `gegenueberstellung`,
Rohrdicken unter `rohrdurchmesser`, Legende der Kontrollbilder unter `bilder_legende`.
