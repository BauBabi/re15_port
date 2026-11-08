# SF Messblatt V8: Koerperbau nach Nutzer-Skizze (Darstellung.png) - nur Messung (Messer, 2026-11-08)

Kein RE-Befund: Die Sichel-Form gibt es auf keiner Disc. Alle Zahlen sind Bildmessungen an der Vorlage und am Modell V7
(Spiel-Asset re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD, sha256 319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6,
Zweig r41/sichel_v8, HEAD 6a25db27b) bzw. Lesungen des Original-EM30 (CDEMD0.EMS Typ 0x30). Keine Zahl hier ist eine @0x-Konstante;
jede Bauzahl ist eine PORT-WAHL, belegt mit Nutzer-Wortlaut oder dieser Messung. Am Code, am Asset und unter Version_80%/, pri/,
synchro/ ist nichts geaendert. Kein Bau, kein Spiellauf: die Szenen-Zahlen stammen aus dem freigegebenen Lauf
sfv4/r/V7_szene/debug.log (nur gelesen).

Seiten: (B) = Sicht des Betrachters, (W) = Sicht des Wesens. H = Figurhoehe der Vorlage = Sohle 436 - Oberkante 182 = 254 px.
Massstab am Koerper (PV, senkrecht): 65,4 px je 1000 Modelleinheiten (E.). Rot = a* >= 10 (CIE-Lab, Videoraum; Modell nach
v4_gemein.videoraum, Gauss 1,5 + Eichung V = a + b P).

Belege: Tafeln SF_belege/v8/messung/*.png (roh), Rohdaten SF_belege/v8/messung/daten/*.json, Werkzeuge
SF_belege/v8/messung/werkzeuge/*.py (Python ohne -I; nutzen re15_port/tools/r41_sichel/{foto,v4,v2,v3,v6kopf}; Arbeitsordner
scratchpad/sfv8).

## 0. Ergebnis zuerst

| Punkt | Befund |
|---|---|
| Vorlage | Darstellung_Vorlage.png = Videobild **n1439** (Datei SF_belege/roh/alle/f1440.png; mittlere Abweichung 0,22 Grauwerte, naechstes n1438 0,97). 4 Bilder nach Foto 2 (n1435). Kamera 50A0 Cut 12. |
| Pose | **PV = Clip 17 Bild 47 Gier 587 (51,6 Grad) x -8246 z -21266**: Rumpf-IoU ohne Schwanz 0,639 (P2 0,516), Kopf-Anker 0,981, Sohle 0,992. |
| "Die rechte Seite" | = **(W)-rechts** = in n1439/PV die zur Kamera gewandte Flanke (Knochen 13 Oberarm (W)-rechts 3671 px sichtbar, Knochen 10 (W)-links 234 px). Bild-rechts ist NICHT roeter: Figur Bild-rechts 31,7 % rot, Bild-links 40,9 %. |
| Schwanz-Ursache | Nicht die Dicke, sondern **Wurzel und Verlauf**: Wurzel (Gelenk 16) sitzt 659 E. unter der Rumpf-Oberkante und 378 E. (W)-rechts -> der Buckel (Mesh 8) ragt ueber die Schwanzwurzel = Stufe/Huegel an J; die Biegung liegt fest auf drei Gelenken (39 / 73 / 48 Grad, in allen 912 Keyframes und in 130 von 144 Szenentakten), Gelenk 19 kehrt die Richtung um -> L/Zickzack, unteres Ende 43-50 px rechts der Vorlage. schwanz_r41.c erhaelt diese Biegung (V4-Kette, Knick-Ausnahme > 30 Grad). |
| V7 gegen Nutzerlinie T | Aussenkante V7 gegen Vorlage-Kante: Mittel 16,8 px, Max 50 px (22 Stationen); gegen die Linie T selbst 24,0 / 56 px. |
| V7 an J / Ruecken / Kopf | Oberkontur x 330-385 im Mittel 6,2 px (max 12) unter der Vorlage (Stufe an der Wurzel); Anstieg x 330 -> 405: V7 29 px, Vorlage 18 px; Buckelkuppe Prominenz V7 8 px, Vorlage 3 px; Sattel der V7-Oberkontur 6,6 px bei x 355; Huegel H: Vorlage 7,5 px ueber der Sehne x 405 -> 447 (bei x 437), V7 4,6 px (bei x 427); K (Kopf-Vorderkante) V7 3,0 px Abweichung von der Geraden, Nutzerlinie 2,2 px. |
| Rot (W)-rechts | Oberarm 13: Vorlage 49,4 %, V7 22,8 %; Rumpf 8 (W)-rechts: Vorlage 76,3 %, V7 **0,0 %**; Ruecken oben: 55,2 % / 0,0 %. V7-Zeilen 480-482 haben 0 Eintraege mit a* >= 10 (V5 hat sie vergraut). |
| Adern Original | Original-EM30: Adernbaum Seite 0 Zeile 480 (u 0-77 v 0-155, 1313 Ader-Texel) und Brustadern Seite 1 Zeile 481 (u 128-255 v 0-90, 605 Ader-Texel); V7 traegt dort zu 99,0 / 100 % die Original-Texel. Probe (nur im Speicher): Zeile "V7-480 + 62 Original-Aderfarben" auf Mesh 13 -> Oberarm 42,7 % rot; auf Mesh 8 (W)-rechts nur 1,7 %, weil die in PV sichtbaren Rumpfflaechen auf aderarmen Texeln liegen (9,6 % Ader) -> UV-Verschiebung je Flaeche noetig. |
| Riegel | S1-S8 entworfen (Abschnitt 7); V7 ist bei S1, S2, S3, S4, S6, S7 ROT, bei S5 und S8 gruen. |

## 1. Vorlage einordnen

- Suche (werkzeuge/m1_suche.py): mittlere absolute Abweichung Darstellung_Vorlage.png gegen alle 1538 Bilder SF_belege/roh/alle
  (Worktree r41_sichel, nur gelesen): f1440 0,221 (Figurbox 0,302), f1439 0,971, f1438 1,428, f1437 1,954, f1441 2,441.
  **n1439 = f1440** (n(N-1) = fN).
- Schnitt (m3_schnitt.py, Wandbox x 0-150 y 60-140, Abweichung zu n1439): n1351-n1379 1,2-2,8, n1383-n1435 <= 0,35, n1443-n1475
  2,5-2,9 (derselbe Schnitt, leichte Helligkeitsverschiebung), ab n1479 steigend (Abblende); vor n1351 anderer Schnitt (60,5).
- Bewegung des Schwanzbands (m21_nachbarn.py, Box x 262-330 y 220-340, Abweichung zu n1439): n1435 2,7 / n1436 1,8 / n1437 1,7 /
  n1438 0,5 / n1440 2,8 / n1441 3,6 / n1445 7,3 / n1451 9,7. Das Band steht n1435-n1440 still (Foto 2 zeigt denselben Schwanz), ab
  n1441 schwingt es nach vorn unten vor den Koerper (Tafel 03, n1445-n1451 mit roten Streifen). Die Ruecken-Box (x 330-450, y 175-215)
  dreht schon ab n1440 (11-31).
- Figur-Maske (m4d.py, m7_trimap.py): Konsens-Hintergrund je Pixel aus n1352-n1475 (ohne n1418-n1435, Vordergrund) = Median der 25 %
  Bilder, die dem BSS-Hintergrund (ROOM50A12, Videoraum) am naechsten sind; dE76 Vorlage gegen Konsens, Gauss 1,0, Schwelle 9, Schliessen,
  groesste Komponente (Tafel 09). Figur sicher 10 974 px, unbekannt 29 455 px; Beine und unteres Schwanzstueck bleiben unbekannt (dunkler
  Tuerraum, dE 2-7).
- Pose (m9_pose.py, Verfahren foto/f_pose.py): Gesamt = 0,5 IoU + 0,25 Kopf + 0,25 Sohle; IoU nur Rumpf (Knochen 0-15), Bild links der
  Nutzerlinie J = unbekannt. Kopfbox Vorlage (422,193)-(468,254), Sohle 436 (Messblatt V4). Grob 912 Keyframes x 72 Gier = 65 664
  Kandidaten, Feinsuche (Gier/Bild/Lage) aus den besten 24 und P2.

| Kandidat | Gesamt | IoU | Kopf | Sohle |
|---|---|---|---|---|
| **PV Clip 17 Bild 47 Gier 587 x -8246 z -21266** | **0,813** | **0,639** | 0,981 | 0,992 |
| Clip 17 Bild 49 Gier 644 x -8246 z -21330 | 0,809 | 0,629 | 0,988 | 0,992 |
| Clip 10 Bild 43 Gier 770 x -8326 z -21330 | 0,802 | 0,629 | 0,959 | 0,992 |
| P2 fein: Clip 12 Bild 48 Gier 868 x -8246 z -21330 | 0,720 | 0,539 | 0,811 | 0,992 |
| P2 (Messblatt Foto) Clip 12 Bild 48 Gier 861 x -8182 z -21330 | 0,709 | 0,516 | 0,813 | 0,992 |

Sichtkontrolle Tafel 08 (selbst angesehen): PV deckt Buckel, Kopf rechts und den Oberarm unter dem Buckel; die Beine weichen ab (sie
liegen in der Unbekannt-Zone).

## 2. Nutzerlinien (Annotationspixel)

Differenz Darstellung.png - Darstellung_Vorlage.png (max. Kanal > 40), 8er-Komponenten, Farbe je Komponente, Skelett + laengster Pfad
(m2_linien.py, m2b_poly.py, daten/m2_linien.json). Die Schrift-Komponenten (Textzeilen) sind ausgenommen.

| Linie | Farbe RGB | px | Polylinie (Bildkoordinaten) | Laenge px |
|---|---|---|---|---|
| T1 rot "tail area 1" | 237/28/36 | 209 | (268,296) -> (269,313) -> (270,333) | 37,8 |
| T2 gruen "tail area 2" | 34/177/76 | 848 | (372,180) -> (354,184) -> (336,187) -> (318,199) -> (300,215) -> (282,231) -> (278,249) -> (275,267) -> (272,288) | 172,3 |
| J blau "Tail directly smooth into the back - no hill or anything..." | 0/162/232 | 464 | Zeiger (384,157) -> (381,180); Linie (379,189) -> (369,199) -> (360,217) -> (355,229) -> (357,241) | 24,2 / 63,4 |
| H hellblau "Little red hill from the back" | 153/217/234 | 192 | (420,190) -> (436,190) -> (440,194) -> (445,206) | 35,7 |
| K weiss "smooth straight to head without further hills...." | 195/195/195 | 202 | (454,210) -> (458,218) -> (462,226) -> (463,244) | 37,7 |

Lesung: T = T2 + T1 (219 px, geglaettet) ist die Aussenkontur des Schwanzes von der Rueckenoberkante bis unten; die Linie liegt
**neben** der Bildkante: Median 11,0 px (19 Stationen) ausserhalb. J markiert die Naht Schwanz/Ruecken (Innenseite der Wurzel),
H die Rueckenkontur kurz vor dem Kopf, K die Kopf-Vorderkante.

## 3. Vorlage messen

### 3.1 Schwanz quer zu T (Stationen alle 10 px, Normale nach innen; m27_breite.py, daten/m27_breite.json, Tafel 02)
Aussenkante = erste Stelle mit 3 x dE >= 8; Innenkante = Ende des Laufs dE >= 5 (Luecke <= 4 px).

| s | T-Punkt | Kante (px ab T) | Breite Vorlage px | Breite (H) | Breite V7-Schwanz px |
|---|---|---|---|---|---|
| 0-80 | (370,180)-(305,211) | 6-14 | Ruecken (nicht trennbar) | - | 0-43 |
| 90 | (297,217) | 8 | 39 | 0,154 | 33 |
| 100 | (290,224) | 9 | 12 (dunkle Luecke im Band, Ausreisser) | - | 62 |
| 110 | (283,231) | 11 | 43 | 0,169 | 50 |
| 120 | (280,239) | 12 | 45 | 0,177 | 38 |
| 130 | (279,248) | 13 | 33 | 0,130 | 32 |
| 140 | (277,258) | 9 | 33 | 0,130 | 31 |
| 150 | (275,267) | 14 | 13 | 0,051 | 28 |
| 160 | (274,276) | 14 | 11 | 0,043 | 26 |
| 170 / 190 / 210 | - | keine Kante (dE < 8) | - | - | 12 / 12 / 9 |
| 180 | (269,295) | 6 | 7 | 0,028 | 9 |
| 200 | (269,314) | 11 | 9 | 0,035 | 9 |

- Wurzel: Die Innenkante der Wurzel ist J; Strecke J (379,189) -> (357,241) = 63,4 px, mit der Oberkante (+5-8 px) ein Wurzelquerschnitt
  von **55-63 px = 0,22-0,25 H** (Lesung, siehe Offen 1).
- Mittellinie Vorlage (T + Normale x (Kante + Breite/2)): (316,238) s90, (310,249) s110, (314,246) s120, (308,253) s130, (302,262) s140,
  (296,271) s150, (293,280) s160, (278,297) s180, (285,314) s200; Wurzelmitte an J ~(366,215). Ein Bogen: erst nach Bild-links, ab
  s 130 senkrecht nach unten, keine Umkehr.
- Ende: Die Linie T1 endet bei y 333 (Kante x 278). Der Boden unter diesem Bildpunkt liegt 5137 E. hinter der Lage PV
  (birkin_buehne.boden_aus_video, m20_raum.py); in Koerpertiefe liegt der Punkt ~1575 E. ueber dem Boden (103 px / 65,4 px je 1000 E.).
  Ob der Schwanz dort den Boden beruehrt, zeigt das Bild nicht (Offen 2).

### 3.2 Ruecken-Kontur, Huegel, Kopf (m13b.py, m25_j.py, daten/m13b_oben.json, m25_j.json, Tafel 04)
Oberkontur aus der Figur-Maske je Spalte: x 345-375 y 192-189, x 380-405 185-182 (Kuppe 182 bei x 405, Prominenz 3,0 px), x 410-430
185-190, x 435-445 192-197, ab x 450 Kopf (211). **Huegel H**: Die Kontur liegt bis 7,5 px ueber der Sehne x 405 -> 447, hoechste Stelle
x 437, dort rot (Tafel 07). **K**: Die Nutzerlinie weicht 2,2 px von der Geraden ab (fast gerade). Stufen >= 3 px in x 330-445: keine
(nur die Kopfkante x 446-450).

### 3.3 Rot (W)-rechts (Orte und Anteil; keine Texelwerte; m14_rot.py, daten/m14_rot.json, Tafel 07)
Teile = Id-Puffer V7 in PV auf den sicheren Figurpixeln der Vorlage; (W)-rechts am Rumpf = Knochenraum x < 0.

| Teil | Vorlage rot (a* Median) | V7 rot (a* Median) |
|---|---|---|
| Oberarm (W)-rechts, Knochen 13 (3132 px) | **49,4 %** (10,0) | 22,8 % (3,6) |
| Rumpf 8 (W)-rechts (2148 px) | **76,3 %** (11,6) | **0,0 %** (2,7) |
| Rumpf 8 (W)-links (938 px) | 15,1 % (6,3) | 8,0 % (2,4) |
| Kopf 9 (938 px) | 9,7 % (4,9) | 0,0 % (4,3) |
| Schwanz 16-22 (2511 px) | 26,5 % (6,9) | 0,0 % (-1,6) |
| Region Ruecken oben x 330-450 y 150-215 | 55,2 % | 0,0 % |
| Region Brust/Arm-Mitte x 345-430 y 215-300 | 43,0 % | 13,7 % |

Orte: Ruecken oben und Flanke (W)-rechts unter dem Buckel, Oberarm (W)-rechts (Seiten-/Hinterflaeche), kleiner Huegel H. Das V7-Rot
sitzt nur auf Mesh 13 Zeile 489 (735 px) und Mesh 8 Zeile 488 (114 px, Brust).

## 4. V7 dagegen (PV, gleiche Kamera; m10_v7.py, f_rast-Pixelprotokoll)

- Kontur gegen T (Tafel 02, daten/m27_breite.json; d = Aussenkante V7 - Vorlage-Kante entlang der Normale, + = V7 weiter innen):
  s 0-50: +2 bis +13 (V7-Rueckenoberkante tiefer); s 60-120: 0 bis -9 (der V7-Schwanz ragt bis 9 px ueber die Kante); s 130-210:
  +2 bis +50 (unteres Stueck 43-50 px nach Bild-rechts). **Mittel 16,8 px, Max 50 px**; gegen die Linie T: 24,0 / 56 px.
- Breite: V7 oben 31-62 px (Glieder schraeg zur Normale), s 150-160 26-28 px (Vorlage 11-13), unten 9-12 px (Vorlage 7-9). Durchmesser
  der Ringe in PV: Glied 16-22 = 40 / 36 / 36 / 33 / 22 / 10 / 10 px gegen den Wurzelquerschnitt der Vorlage 55-63 px.
- Kette in PV (Bild): 16 (387,218) -> 17 (351,223) -> 18 (325,227) -> 19 (303,242) -> **20 (334,292)** -> 21 (329,311) -> 22 (328,323).
  Bis Gelenk 19 liegt die Kette nahe der Vorlage-Mittellinie (303,242 gegen ~310,248); Glied 19 kehrt nach Bild-rechts um.
  Gelenkwinkel 17..22: 13,2 / 38,9 / 73,4 / 48,1 / 16,0 / 2,1 Grad; Glied 16 81,9 Grad gegen die Senkrechte.
- J: Oberkontur x 330-385 V7 - Vorlage +6,2 px im Mittel (max 12); V7 steigt x 330 -> 405 um 29 px (Vorlage 18). Rumpf und Schwanz
  getrennt gerastert (m26_huckel_j.py): Schwanz-Oberkante x 345-395 y 204-195, der Rumpf liegt ab x 360 darueber (x 380: 191 gegen 197;
  x 405: 179). Entlang J wechseln 19 von 25 Querpaaren (+-4 px) zwischen Schwanz- und Rumpfknochen = sichtbare Naht.
- Ruecken: V7-Kuppe y 179 bei x 405 (Vorlage 182), Prominenz 8,0 px (Vorlage 3,0); Sattel der V7-Oberkontur unter ihrer oberen Huelle
  (x 300-420) 6,6 px bei x 355; Ruecken -> Kopf (x 380-447) 6,4 px bei x 438. Huegel H 4,6 px bei x 427 (Vorlage 7,5 bei x 437).
- Kopf: V7-Vorderkante y 212-244 bei x 452-461, 3,0 px Abweichung von der Geraden. Die Kopfgeometrie bleibt (Nutzer "jetzt schon gut").
- Rot (W)-rechts: siehe 3.3 (Oberarm 22,8 %, Rumpf 0,0 %).

Tafel 01: Vorlage | Darstellung.png | V7 mit Nutzerlinien, V7-Umriss (weiss), Schwanz 16-22 (magenta), Vorlage-Figur (gelb) | 50 %.

## 5. Ursache

1. **Wurzel zu tief und seitlich.** Knochen 16 relpos im Raum von 8 = (-378, -602, -410); Rumpf-Oberkante y -1261 (Ecken (-395,-1261,-296),
   (-122,-1232,-304), (-339,-1231,-392)). Mit Wurzelradius 308 liegt die Rohroberkante bei y -910, **351 E. (~23 px) unter dem Ruecken**:
   Der Buckel steigt ueber der Wurzel auf (J-Stufe 6-12 px, Kuppe 8 px Prominenz). Die Vorlage zeigt die Schwanzoberkante als
   Fortsetzung der Rueckenkontur ("Tail directly smooth into the back - no hill").
2. **Feste L-Biegung.** Gelenkwinkel ueber alle 912 Keyframes (m19_kette.py): Median 14,5 / 37,8 / 73,3 / 45,5 / 16,0 / 13,9 Grad,
   P10-P90 nur 2-9 Grad breit -> jede Animation zeigt dieselbe Form. Glied 16 liegt im Median 72 Grad von der Senkrechten (waagerecht
   nach hinten); Gelenk 19 knickt 73 Grad und dreht die Kette zurueck unter den Koerper. Glied 19 ist mit 1121 E. das laengste
   (V6 x 2,14 fuer die alte Spitze in P2).
3. **Das Spiel erzwingt die L-Form.** schwanz_r41.c erkennt die Kette als V4 (Gliedlaengen 1121 / 204 = 5,5 > SW_GLATT_V4_VERH 2,0),
   haengen() laeuft nicht, glaetten() laesst jedes Gelenk mit Keyframe-Winkel > SW_GLATT_KNICK_AB (30 Grad) bei seinem Winkel (+3 Grad,
   oeffnet hoechstens SW_BIEGUNG_SPIEL 10 Grad) und begrenzt die uebrigen auf 12-16 Grad. Szene V7 (144 Takte, m28_szene.py):
   Gelenkwinkel Median 14,3 / 38,5 / 72,2 / 46,3 / 15,9 / 13,4 Grad, 130 Takte mit einem Gelenk > 60 Grad; kein Gelenk unter dem Boden
   (tiefstes y -108).
4. **Querschnitt.** Ringradien (Knochenraum, daten/m19_kette.json): Mesh 16 308 -> 273, 17 274 -> 272, 18 272 -> 255 (Bauch 282),
   19 255 -> 170, 20 170 -> 80, 21 80 -> 74, 22 74 -> Spitze. Mitte und Ende passen zur Vorlage (36-33 px, 10 px); zu schmal ist nur die
   Wurzel (40 px gegen 55-63 px), und sie steckt seitlich im Rumpf (Mesh 16 reicht bis t = -383 in den Rumpf, Endring sichtbar als Naht an J).
5. **Huckel.** (a) Stufe an J = Buckel ueber der tiefen Wurzel (Punkt 1). (b) Kuppe von Mesh 8 (Ecken siehe Punkt 1) mit 8 px Prominenz
   statt 3 px. (c) Sattel Ruecken -> Kopf 6,4 px bei x 438 (die Vorlage hat dort keinen; ihr Huegel H sitzt bei x 437 als Erhebung).

## 6. Bauplan V8 (PORT-WAHL; Ziel = Nutzerlinien in PV)

### 6a Schwanz
- **Wurzel:** Gelenk 16 (relpos in 8) auf die Rueckenmitte zwischen den Schulterblaettern: x ~ 0 (heute -378), Oberkante des Wurzelrings
  buendig mit der Rumpf-Oberkante (Ring-Mitte y ~ -1261 + r0, heute -602), z an der Rueckenflaeche (oberste Ecken z -296 bis -392) halb
  eingebettet. Feinwerte per Suche gegen S1/S2 in PV. Beleg: Nutzer "Tail directly smooth into the back - no hill or anything",
  frueher "vom ruecken aus geschlossen ein zusammenhaengendes stueck Das direkt I. den Ruecken uebergeht".
- **Radien (Durchmesser in PV -> E. mit 65,4 px je 1000 E.):** Wurzel 55-63 px -> r0 420-480 E.; s 90-130 40 px -> 305 E.; s 140 33 px ->
  250 E.; s 150-160 12 px -> 90 E.; Ende 8 px -> 60 E.; monoton fallend, Steigungsknick <= 15 Grad (Regel V5f), Gelenkringe deckungsgleich,
  Deckel V6 bleiben. Der Wurzelring liegt im Rumpf (keine sichtbare Ringkante, S2).
- **Ruheverlauf:** Kette in PV entlang der Vorlage-Mittellinie (3.1): Wurzel (366,215) -> (316,238) -> (302,262) -> (293,280) -> (278,297)
  -> (285,314) -> Ende ~(283,330). Biegung ~90 Grad gleichmaessig auf die Gelenke 17-21 (je <= 25 Grad), ohne Umkehr. Umsetzung im EMD:
  Drehungs-Keyframes der Knochen 16-22 = Ruhebogen (in PV gegen S1 eingepasst) plus die bisherige Bewegung je Bild (Keyframe minus
  Clip-Median); Gliedlaengen neu verteilt (etwa gleich lang statt 624/440/887/1121/350/204); Meshes 16-22 mit neuen Ringen (Radien oben),
  Flaechen/UV/Zeile 487 wie V6/V7 (Schwanzfarbe bleibt "dunkel gruenlich-grau").
- **Code (schwanz_r41.c):** Die V4-Erkennung (Gliedverhaeltnis > 2,0) und die Biegungs-Ausnahme (SW_GLATT_KNICK_AB 30 Grad,
  SW_BIEGUNG_SPIEL 10 Grad) wurden fuer die L-Form gebaut. V8 braucht einen eigenen Kettenmodus: glaetten() mit Grenze = Keyframe-Winkel
  + 3 Grad, hoechstens 25 Grad, ohne Ausnahme; haengen() aus (Glied 16 liegt fast waagerecht; SW_HANG_COS_MIN 80 Grad wuerde es
  herunterdruecken); Feder/Mitnahme wie V4; die Bodenregel (Radius + Dorn, ganze Kette hochdrehen) bleibt. Erkennungsmerkmal neu waehlen
  (z. B. Wurzel-relpos), weil gleich lange Glieder den V4-Pfad verlassen.
- **Szene (engine/src/birkin_formen.c ~575-681 und 816):** Schwanzbahn-Ziele (ziel/fuehrung), verdeckte Glieder h60-h70 und
  k_schwanz_dicke sind auf die alte Kette gelegt; neue Gesamtlaenge und Wurzel nachpruefen. Die Spider-Man-Szene (lauf_v4.sh) muss mit
  EXIT 0, 191 FRAMEDUMPs und "EM30 loaded: 23 meshes, 23 bones, 21 clips" laufen und gegen sfv4/r/V7_szene verglichen werden.
- **Boden:** Spitze (inkl. Dornspitze Mesh 22) in keinem der 912 Keyframes unter der tiefsten Koerperecke (V7: 82 darunter) und in der
  Szene nie y > 0.

### 6b Ruecken
- Buckel: Die obersten Ecken von Mesh 8 (Oberkante y -1261 / -1232 / -1231, Kuppe in PV x 395-415) so absenken, dass die Kuppe hoechstens
  4 px Prominenz hat (Vorlage 3) und die Oberkontur x 330-385 auf +-4 px an der Vorlage liegt; die Schwanzoberkante uebernimmt x 330-380.
- Huegel H: ein kleiner Huegel 5-10 px ueber der Sehne x 405 -> 447 bei x 428-445 (Vorlage 7,5 px bei x 437), rot (Ader-Zeile, 6c);
  sonst keine weiteren Huegel.
- K: Kopf (Mesh 9 und Zeile 486) bytegleich V7; Uebergang 8 -> 9 ohne Stufe >= 3 px (heute 201 -> 206 bei x 445 -> 450, gut).

### 6c Adern (W)-rechts (Original-EM30-Texel; m15_orig.py, m16_vgl_tex.py, m17_adern.py, daten/m17_adern.json, Tafel 05)
- Quelle: CDEMD0.EMS Typ 0x30 = Blob 16, 226 404 Byte, sha256 569e3138...; TIM 384 x 256, 8 bpp, CLUT-Zeilen 480-482.
- Ader-Texel = Original-Texel mit Paletten-a* >= 15 und L* <= 38 in der Zeile seiner Original-Flaechen, duenn (verschwindet unter der
  Oeffnung 5 x 5); Flaechen-Rot (bleibt unter der Oeffnung) = Wunde/Augapfel, nicht verwenden.

| Bereich | Seite / Zeile | Texel benutzt | Ader | Flaechen-Rot | Ader-Lab Median | Haut-Lab Median | V7 = Original |
|---|---|---|---|---|---|---|---|
| A Adernbaum u 0-77 v 0-155 | 0 / 480 | 10 046 | 1313 (13,1 %) | 92 | 22,8 / 21,9 / 10,4 | 51,4 / 10,8 / 5,5 | 99,0 % |
| B Brustadern u 128-255 v 0-90 | 1 / 481 | 7713 | 605 (7,8 %) | 233 | 20,7 / 23,3 / 11,2 | 46,3 / 8,2 / 6,9 | 100 % |
| C Schulter-Augapfel u 78-127 v 0-100 | 0 / 480 | 2415 | 225 | 342 | 28,7 / 30,5 / 15,3 | 55,0 / 12,8 / 7,2 | 100 % (nicht verwenden, Offen 7) |

- Zeilen: X1 = V7-Zeile 480 (graue Haut V5) + die 62 Original-Eintraege der Zeile 480 mit a* >= 15 und L* <= 38, bytegleich; X2 =
  V7-Zeile 481 + die 50 entsprechenden Original-Eintraege der Zeile 481. Eine Transformation je Material (Haut grau wie V5, Adern =
  Original), keine neue Farbe, keine Eichung je Fleck. Budget: 13 + 2 = 15 Zeilen, Port-Texturhoehe 256 x 15 = 3840 <= 4096.
- Flaechen: Mesh 13 (Oberarm (W)-rechts) Zeile 480 -> X1 (die Zeile-489-Flaechen bleiben); Mesh 8 (W)-rechts (Schwerpunkt x < 0) Zeile
  480 -> X1 (51 Flaechen), 481 -> X2 (6). Probe im Speicher (m22_adersim.py, Tafel 06): Oberarm 22,8 -> **42,7 %**, Rumpf (W)-rechts
  0,0 -> **1,7 %**.
- Rumpf: Die 18 in PV sichtbaren Flaechen von Mesh 8 (W)-rechts (d0, d15, d43, d48, d51, d55, d56, d64, d73, v4, v8, v13, v14, v18, v19,
  v21, v24, v25; zusammen 2954 Texel, heute 283 Ader = 9,6 %, u 1-193 v 5-101) liegen auf aderarmen Texeln; ein gemeinsamer Schub ist
  nicht moeglich (beide Seiten der Textur, m23b.py). Plan: je Flaeche bzw. je zusammenhaengender UV-Insel eine Verschiebung auf
  aderreiche Fenster derselben Seite/Zeile, Form gleich, nur auf benutzte Original-Texel (m24): A u 19-43 v 40-64 (43 %), u 0-12 v 22-46
  (51 %), u 0-12 v 69-93 (48 %); B u 188-212 v 29-53 (37 %), u 136-160 v 35-59 (23 %). Texel bleiben bytegleich (Mehrfachnutzung
  unschaedlich).
- Huegel H: dieselbe Ader-Zeile auf die Flaechen, die in PV x 425-445 y 188-200 bilden.

## 7. Riegel-Entwurf (Schwellen, V7-Wert, Gegenprobe)

| Riegel | Pruefung (PV, Spielkamera 50A0/12, RDT-Licht) | Schwelle | V7 |
|---|---|---|---|
| S1 Kontur T | Aussenkante Modell entlang der 22 T-Normalen gegen die Vorlage-Kante (wo nicht messbar: T + 11 px) | Mittel <= 4 px, Max <= 8 px | 16,8 / 50 -> **ROT** |
| S2 Uebergang J | (a) Oberkontur x 330-385 gegen Vorlage-Maske; (b) Sattel der Modell-Oberkontur unter ihrer oberen Huelle x 300-420; (c) Rumpf ueber der Schwanz-Oberkante an der Wurzel (m26) | (a) Mittel <= 4, Max <= 6 px; (b) <= 2 px; (c) <= 3 px | (a) 6,2 / 12; (b) 6,6; (c) 6-14 -> **ROT** |
| S3 Ruecken/H/K | (a) Kuppenprominenz; (b) Huegel H ueber der Sehne x 405 -> 447 und sein Ort; (c) keine weitere Erhebung >= 3 px in x 330-460; (d) Sattel Ruecken -> Kopf; (e) K: Vorderkante y 212-244, Abweichung von der Geraden | (a) <= 4 px; (b) 5-10 px bei x 428-445; (c) 0; (d) <= 2 px; (e) <= 3 px | (a) 8,0; (b) 4,6 bei 427; (d) 6,4; (e) 3,0 -> **ROT** |
| S4 Breite | Durchmesser der Ringe in PV quer zur eigenen Achse | Wurzel >= 55 px; s 90-130 >= 36 px; monoton fallend (+-2 px); Ende <= 10 px | Wurzel 40 px -> **ROT** |
| S5 Gesicht | Mesh 9 (Ecken, Normalen, Flaechen, UV, CLUT-Feld), Zeile 486, alle von Mesh 9 benutzten Texel | bytegleich V7 | gruen (Gegenprobe: V5f muss rot sein) |
| S6 Boden | Spitze + Dornspitze Mesh 22 gegen die tiefste Koerperecke in allen 912 Keyframes; Szene lauf_v4.sh alle Takte y <= 0 | 0 Keyframes, 0 Takte | 82 Keyframes -> **ROT** (Szene 0) |
| S7 Adern | (a) Herkunft: jeder Eintrag der neuen Zeilen = V7-Zeile (gleicher Index) oder Original-Eintrag mit a* >= 15, L* <= 38 (15 Bit gleich); jeder Texel unter den umgestellten Flaechen = Original-Texel; (b) Anteil a* >= 10 Oberarm 13; (c) Rumpf 8 (W)-rechts; (d) Ruhe: rote 8er-Komponenten unter weissem Licht auf (W)-rechts <= Original-EM30 in derselben Pose | (b) 35-59 % (Vorlage 49,4); (c) 25-80 % (Vorlage 76,3); (d) <= Original | (b) 22,8; (c) 0,0 -> **ROT** |
| S8 Technik | CLUT-Zeilen <= 16, 256 x Zeilen <= 4096, jedes CLUT-Feld zeigt auf eine vorhandene Zeile, 23 Meshes, 23 Knochen, 21 Clips, Port-Lader (unit_birkin_formen, unit_schwanz_r41, integration_r41_sichel_form) | alles | gruen (13 Zeilen, 3328) |

Gegenprobe-Pflicht: V7 (319bbd17) muss bei S1, S2, S3, S4, S6 und S7 rot sein (oben gemessen: ist es). S5 zusaetzlich gegen V5f
(alter Kopf) rot.

## 8. Tafeln (SF_belege/v8/messung/, roh)

| Datei | Inhalt |
|---|---|
| 01_vorlage_linien_v7.png | Vorlage n1439 / Darstellung.png / V7 in PV mit Nutzerlinien, V7-Umriss, Schwanz 16-22, Vorlage-Figur / 50 % (x3) |
| 02_kontur_entlang_T.png | Stationen entlang T: Vorlage-Kante, Innenkante, Mitte / V7-Aussenkante (x3) |
| 03_nachbarbilder_n1431-1451.png | Bewegung des Schwanzbands n1431-n1451 mit Nutzerlinien (Gamma 0,6, x2) |
| 04_oberkontur_J_H_K.png | Oberkontur Vorlage (gelb) gegen V7 (magenta) an J, H, K (x5) |
| 05_aderkarte_original_em30.png | Original-EM30-Textur (Zeilen der Original-Flaechen) / Karte: Ader rot, Flaechen-Rot gelb, Bereiche A-E (x2) |
| 06_adern_probe.png | Vorlage / V7 / Probe X1 (nur Zeilenwechsel), Ausschnitt Oberarm-Ruecken (x3) |
| 07_rotkarte_vorlage_v7.png | a* >= 10 Vorlage / V7 PV (x2) |
| 08_pose_kandidaten.png | Vorlage, drei beste Posen und P2 fein mit 50 %-Mischung |
| 09_trimap_n1439.png | Figur-Maske (gruen sicher, rot unbekannt) |

## 9. Offen

1. Der Wurzelquerschnitt 55-63 px ist aus der Lage von J gelesen (die Vorlage trennt Schwanz und Ruecken bei s 0-80 nicht). Liegt J
   weiter innen, ist die Wurzel schmaler.
2. Ende: T1 endet bei y 333; Bodenkontakt dort hiesse 5137 E. hinter dem Koerper, in Koerpertiefe haengt das Ende ~1575 E. ueber dem Boden.
   Ziel ist die Bildlinie; die Tiefe waehlt der Bau (S1 + S6). Bei Konflikt den Nutzer fragen.
3. Vorlage-Breite s 100 (12 px) und s 170/190/210 (keine Kante) sind Messluecken (dunkle Luecke, dE < 8); S1 nutzt dort T + 11 px.
4. Nachbarbilder n1441-n1451: Das Band schwingt nach vorn unten vor den Koerper (rote Streifen). Fuer Gang/Kampf ist nur die Form in
   n1435-n1440 gemessen; die Bewegung der neuen Kette im Spiel bleibt Pruefung am Szenenlauf.
5. Pose: Rumpf-IoU 0,639; die Beine liegen in der Unbekannt-Zone (dunkler Tuerraum) und sind nicht eingepasst.
6. S7 Rumpf: Die Vorlage zeigt 76 % rot (die Video-Unschaerfe verbreitert duenne Adern). Ob Adern allein (Original-Texel) nach dem UV-Schub
   25 % erreichen, ist nicht gemessen (Probe ohne UV-Schub: 1,7 %). Faellt S7c zu knapp aus, ist die Schwelle mit dem Nutzer am Bild zu
   klaeren - keine gemalten Flaechen (Memory "Texturen nie aus Videobildern eichen").
7. V5-Nutzersatz "roter Fleck oben auf der rechten Schulter, den gibt es in der Vorlage ganz offensichtlich nicht" (Foto 1 frontal) gegen
   heute "mehr rottoene ... mehr adern an verschiedenen Stellen, am Arm und Koerper": Lesung = Adern ja, Flaechen-Rot (Augapfel C) nein.
8. Kein Bau und kein Spiellauf in diesem Schritt; die Szenen-Zahlen sind aus dem freigegebenen V7-Lauf (nur gelesen).
