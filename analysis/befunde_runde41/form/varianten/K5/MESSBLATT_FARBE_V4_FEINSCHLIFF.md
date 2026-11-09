# Messblatt Farbe V4: Feinschliff der Riesenform K5c (Messer, 2026-11-08)

Gegenstand: das Spiel-Asset seit E30 (`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`, Basis `stufe3/basis_k5c`, 16 CLUT-Zeilen).
Hier wird nur gemessen, nichts gebaut. Auftrag: Nutzersaetze 2026-11-08 (Details fehlen, Schwanzspitze fleischroetlich, Gelenke weisslich,
Lippe ohne Rot, rote Fleischstelle auf dem Buckel vorn, insgesamt heller, Krallen vorn zu viele und zu rot).
K5c ist ein neues Modell (PORT-WAHL "eher c"). Hier gibt es keinen Disc-Wert. Jede Zahl unten ist an Datei, Bild oder Render gemessen.

**Werkzeuge** (`farbe_lesung/V4F_skripte/`, Python ohne -I):

| Skript | Aufgabe |
|---|---|
| `v4f.py` | Kennungs-Render: Nachbau von `form_buehne.dreiecke/rastern` und `form_pruefung.rundum_dreiecke`. Je Bildpunkt Mesh, Flaeche, Texel u/v, CLUT-Zeile und Eintrag. Gegenprobe gegen `Kandidat.rendern`: 0 Pixel Unterschied (Asset Bild 2, K4 Bild 1/2). |
| `blick_alle.py`, `ketten.py` | Cache je Stand und Bild (Szenenkette siehe unten) |
| `gelenke.py` | erkennt die 31 Gelenkkoerper (`k_naht.kugel`: 24 Vierecke + 16 Dreiecke an 34 Ecken am Stueck) und benennt sie nach dem naechsten Kindknochen |
| `referenzen.py` | Referenzzaehlung je Zeile und Eintrag (UV-Polygone im Texelraster). Die Flaechenzahlen je Zeile stimmen mit FARBE.md E.1 ueberein. |
| `krallen_spitzen.py` | Krallen: Spitzen im Modell und im Nutzerumriss |
| `messen.py` | Hauptmessung, schreibt `feinschliff/messung_v4f.json` |
| `tafeln_v4f.py` | Tafeln m1 und m2 |

Die Seitenzaehlung und die Indizes der Flaechen von `form_emd.lese_emd` und `EA.Modell` sind identisch (3828/3828 Flaechen gleich).

**Szenenkette** (wie Tafeln 01/02, fuer alle Staende gleich):
1. `Kandidat.rendern(n)`: Disc-Licht x k aus `pruefung/licht.json`, Bild 1 = 5080 Cut 2, Bild 2 = 5090 Cut 11.
2. `form_video.video_filter(entzerren=False, rauschen=True)`, Saaten 15 / 12345 / 7.
3. Fuer "T": `p_zonen.rueck`, das ist die Kette rueckwaerts (Licht heraus).

Die Vorlage geht durch denselben Rueckweg. Lab = CIE-Lab D65, OpenCV float. Seiten: (W) = aus Sicht des Wesens.

**Staende in allen Messungen:**
- K4 = Spiel-Asset bis E30, `stufe3/basis_n2` (NB2-Textur 1408 x 256)
- K4 kroko = `K4_heute`, das war die Spalte "K4 heute" der Tafeln 01-10
- K5c roh, V2, V3R3, V3R4
- Asset K5c = Runde 5, 16 Zeilen; rendert pixelgleich zu V3 Runde 5 (E-Z3)

Tafeln (roh, kein Gamma, gleiche Kette je Zeile): `feinschliff/m1_details.png`, `feinschliff/m2_regionen.png`.
Rohdaten: `feinschliff/messung_v4f.json`, `messung_v4f_flecken.npz` (Fleckmasken), `krallen_spitzen.json`, `referenzen_asset.json` (+ `_texel.npy`).

## 0. Ergebnis in Kuerze

1. **Die verlorenen Details sind helle Gelenkkappen, die Schwanzspitze und die Buckel-Fleischstelle.** Alle drei hatte K4 (ebenso K4 kroko und
   K5c roh); seit V3R3 (G4-Uebertragung) fehlen sie.
   - Die Vorlage hat in Bild 1 neun helle Gelenkflecken mit T-L\* 41-57 (weisslich bis kuehl).
   - Am Asset liegen dort Ketten-L\* 13-28, an K4 34-51.
   - Der Anteil heller Pixel (T-L\* > 40) in der Figur: Bild 1 Vorlage 9,9 %, K4 15,3 %, Asset 4,8 %; Bild 2 14,5 / 16,9 / 9,0 %.
2. **Krallen vorn: im Modell 7 sichtbare Krallen, im Nutzerumriss 3 Spitzen.**
   - Rechte Hand (W): 4 Krallen (Meshes 29, 20, 19, 31). Im Umriss hat nur Kralle 31 eine Spitze (14 px daneben).
   - Linke Hand (W): 3 Krallen (14, 15, 28). Der Umriss hat 2 Spitzen, Kralle 14 und 15 (7 bzw. 2 px).
   - Ohne Gegenstueck sind die Krallen 29, 20, 19 und 28. Kralle 29 ragt 241 px ueber den Nutzerumriss hinaus.
3. **Das Rot der Krallen kommt aus der Albedo der Zeile 1** (G4-Klauenzeile): 47,2 % der Krallentexel sind rot (a\* > 15, C\* > 15), Rot-Lab (29,7 / 27,3 / 12,6).
   - In der Vorlage haben die Krallen die Farbe der Hand: dE 4 (r) und 3,8 (l) unter T.
   - Am Asset sind die Krallen 19 bzw. 15 L\* heller als die Hand.
   - K4-Krallen waren grau (Albedo a\* 0,0, b\* -1,3, 0 % rot).
4. **Helligkeit:** Die Figur braucht x 1,14 (Bild 1) und x 1,37 (Bild 2) mit Videofilter, x 1,38 und x 1,41 ohne. Fuer "mindestens wie die Vergleichsbilder" also
   **x 1,37 (Video) bzw. x 1,41 (ohne Filter)** auf die Figur. Je Region liegt der Bedarf bei x 0,9 bis 1,7, je Zeile mehr (Abschnitt 3).
5. **Zeilenbudget:** keine 17. Zeile noetig.
   - Jede der 16 Zeilen hat 107-235 unbenutzte Eintraege.
   - Zeile 1 (Krallen) gehoert nur den Krallen und kann direkt umgefaerbt werden.
   - Gelenke, Schwanzspitze und Buckel teilen ihre Eintraege und fast alle Texel mit Nachbarflaechen. Sie brauchen eigene Texelinseln mit freien
     Eintraegen ihrer Zeile; in der TIM sind 108 309 Texel frei.

## 1. Verlorene Details (Vorlage | K4 | fruehere Staende | Asset)

**Verfahren:**
- Helle Flecken der Vorlage: T-L\* > 40 (Bild 1) bzw. > 28 (Bild 2) in den Nutzer-Teilmasken (`nutzer_teile`), 2 px vom Umriss entfernt,
  ohne Logo, Boden (`p_zonen.BODEN`) und Weisskappe, mindestens 8 bzw. 10 px.
- Je Fleck: Ort am Modell aus dem Kennungs-Render der Szene (Asset und K4), Albedo dort, Ketten-L\* aller Staende auf denselben Pixeln (Mittel der 3 Saaten).
- Tafel `m1_details.png`, Z.1-4: Kreis gruen, wenn der Stand >= 80 % des Vorlagen-T-L\* erreicht, sonst rot. Darunter rundum K4 | Asset mit
  beschrifteten Gelenkkoerpern.

### 1.1 Bild 1 (5080, Rueckseite)

| Fleck | Lage (px), Groesse | Vorlage Lab Video / T | Ort am Asset (px) | Zeile Asset | Albedo Asset / K4 | Kette L\*: K4 / K5c roh / V2 / V3R3 / V3R4 / Asset |
|---|---|---|---|---|---|---|
| F04 Ellbogen r (W) | (399,284), 184 | (51,7/4,5/-16,3) / (50,4/-2,8/-3,2) | Gelenkkoerper Ellbogen r 184 | 11 | (8,9/1,2/-0,6) / (35,5/-2,5/-4,8) | 51,3 / 49,2 / 29,5 / 15,0 / 13,0 / **13,0** |
| F07 Huefte r | (322,298), 457 | (57,1/7,1/-19,2) / (54,6/-0,3/-6,4) | Mesh 9 169, Gelenk Huefte r 163, Muffe Huefte r 125 | 3, 9 | (11,4/6,2/-3,7) / (33,8/3,3/-6,5) | 48,8 / 56,4 / 51,1 / 19,0 / 17,9 / **17,9** |
| F05 Huefte l | (241,289), 69 | (55,4/7,1/-16,9) / (51,7/-1,0/-4,1) | Mesh 8 40, Huefte l 16 | 2, 3, 9 | (9,2/9,0/-5,8) / (31,0/1,8/-3,2) | 45,3 / 44,2 / 48,3 / 19,2 / 16,9 / **16,9** |
| F08 Knie l | (241,345), 92 | (46,1/7,4/-28,6) / (44,5/-1,8/-11,6) | Knie l 89 | 3 | (19,8/0,7/-0,5) / (23,8/2,9/-5,7) | 34,0 / 32,7 / 25,1 / 28,1 / 27,0 / **27,0** |
| F10 Knie r | (330,352), 152 | (52,6/4,2/-17,5) / (49,3/-2,6/-3,9) | Knie r 80, Mesh 9 39, Mesh 26 33 | 3 | (11,3/12,7/-6,3) / (25,9/1,1/-0,6) | 39,6 / 40,3 / 22,6 / 26,3 / 25,6 / **25,6** |
| F12 Hand r (W) | (412,357), 24 | (49,8/1,4/-16,4) / (44,4/-3,5/-4,3) | Mesh 18 24 | 11 | (5,3/3,4/1,0) / (38,5/-0,4/-4,5) | 36,9 / 37,0 / 32,8 / 14,9 / 13,1 / **13,1** |
| F13 Fuss l | (206,404), 71 | (42,8/12,3/-27,4) / (41,4/2,8/-11,3) | Mesh 25 71 | 3 | (7,9/8,1/-3,0) / (40,2/1,4/-7,6) | 45,4 / 18,4 / 29,8 / 14,4 / 14,0 / **14,0** |
| F14 Fuss r | (302,413), 76 | (54,6/13,9/-28,9) / (52,2/4,8/-14,7) | Asset: 3 px (Fuss liegt daneben) | 3 | - / (64,0/-4,8/-14,3) | 47,0 / 67,7\* / ... / **67,6\*** (\* Boden, Asset deckt nicht) |
| F03 Ruecken unter F1 | (318,266), 295 | (52,4/16,9/-26,5) / (51,0/6,5/-11,4) | Mesh 0 288 (Zeile 5 236 px = F1-Flaechen) | 5, 10 | (15,7/7,1/-3,4) / (69,1/6,7/-12,8) | 49,6 / 51,1 / 18,3 / 16,8 / 16,6 / **16,6** |
| F06, F11 Schwanzglieder 3-4, 4-5 | (273,286) 39, (266,358) 39 | T (43,5/7,9/-15,4), (42,9/-0,8/-14,2) | Mesh 4/5 + Gelenk Schwanz 3-4 / 4-5 | 2 | (12,3/4,4/-0,7), (7,5/4,0/-2,3) | 38,4 / 37,7 / 20,7 / 20,3 / 19,6 / **19,6**; 18,1 / ... / **13,2** |
| **F15 Schwanzspitze** | (271,447), 543 | (56,8/21,6/-7,7) / (53,3/11,9/3,1) | Mesh 6 502, Gelenk Schwanz 5-6 37 | 2 | (14,2/7,5/-3,5) / (35,0/11,6/0,6) | 44,2 / 44,2 / 42,5 / 25,1 / 22,5 / **22,5** |
| F01 (F1-Leuchtstelle, Kontrolle) | (321,237), 347 | (54,4/31,0/-13,2) / (52,5/18,8/-1,3) | Mesh 0, Zeile 5 | 5 | (47,3/20,6/-3,2) / (51,2/18,6/-3,3) | 57,1 / 57,1 / 34,7 / 52,4 / 52,4 / **52,4** (bleibt, E30) |

Schwanzspitze genauer: Mesh-6-Pixel im Nutzer-Schwanz (1180 px).

| | Lab | Rot-Anteil (a\* >= 12, C\* >= 15, L\* >= 20) |
|---|---|---|
| Vorlage T | (44,3 / 12,4 / 1,0) | 37,5 %, Rot-Lab (41,3 / 22,5 / -1,5), Farbton -3,8 Grad, also rosa-fleischfarben |
| K4 Kette | (42,7 / 8,4 / -1,7) | 10,3 % |
| Asset Kette | (23,4 / 3,3 / -4,4) | 2,6 % |

Zum Vergleich: Im ganzen Schwanz der Vorlage sind 5,9 % rot.

### 1.2 Bild 2 (5090, Vorderseite)

| Fleck | Lage, Groesse | Vorlage Lab Video / T | Ort Asset (Zeile) | Albedo Asset / K4 | Kette L\*: K4 / K5c roh / V2 / V3R3 / V3R4 / Asset |
|---|---|---|---|---|---|
| **F01 Buckel** | (286,185), 629 | (22,1/30,1/-2,1) / (41,4/15,7/-4,3) | Mesh 10 575 (Z12 467, Z13 108), Kopf-Hals 54 | (11,8/16,0/-9,5) / (63,9/6,1/-1,8) | 63,5 / 34,5 / 35,4 / 14,4 / 12,0 / **12,0** |
| **F02 Buckel** | (318,179), 751 | (17,0/30,1/-1,8) / (33,2/22,2/-6,4) | Mesh 10 751 (Z12 620, Z13 131) | (12,5/15,7/-9,8) / (46,4/22,0/-11,7) | 44,7 / 28,5 / 29,6 / 12,7 / 12,3 / **12,3** |
| F03 Buckel Rand | (347,189), 192 | (14,7/24,9/-3,8) / (31,5/15,2/-6,3) | Mesh 10 (Z13 141, Z12 51) | (12,2/7,4/-4,3) / (27,8/8,5/-4,6) | 24,4 / 12,9 / 17,9 / 13,8 / 10,9 / **10,9** |
| F05 Schulter l (W) | (407,229), 750 | (26,6/23,6/-3,1) / (44,2/6,3/-1,9) | Gelenk Schulter l 348, Mesh 11 244, Muffe Hals-Brust 117 | (17,3/8,8/0,8) / (34,9/1,7/-2,1) | 33,4 / 33,9 / 28,2 / 15,3 / 17,9 / **17,9** |
| F06 Schulter r (W) | (231,251), 167 | (17,9/15,5/-4,7) / (39,6/-3,2/0,2) | Muffe Schulter r 158 (Z15) | (15,7/12,2/10,6) / (31,1/3,6/-2,3) | 32,1 / 26,7 / 19,6 / 9,5 / 13,6 / **13,6** |
| F04 Brust/Schulter l | (375,204), 301 | (16,7/17,9/-6,8) / (36,5/1,3/-4,7) | Mesh 10 Z13 243, Schulter l 39 | (18,7/7,2/-2,8) / (18,0/2,4/-1,8) | 15,5 / 16,9 / 21,0 / 15,7 / 17,1 / 17,1 |
| F12 Hals-Brust | (368,285), 136 | (16,1/20,6/-5,4) / (31,6/8,0/-4,7) | Muffe Hals-Brust 135 (Z13) | (12,6/6,3/-3,7) / (22,3/3,0/-3,2) | 19,8 / 16,6 / 29,5 / 17,6 / 14,2 / 14,2 |
| F13 Hand/Krallen l (W), F14 Kiefer + Hand r | 2553 / 5388 | T (39,2/25,4/22,1) / (49,9/26,9/20,5) | Feuerschein; Z15/Z1 bzw. Z6/Z1/Z11 | - | 33,3 / ... / 26,1 bzw. 49,4 / ... / 38,6 |

### 1.3 Liste der Details (je ein Satz, Beleg oben)

- **D1 Weissliche Gelenkkappen der Beine (Bild 1):**
  - Hueften l/r (Gelenkkoerper Huefte l/r und Muffen, Meshes 8/9) zeigt die Vorlage bei T-L\* 52-55, Knie l/r (Gelenkkoerper Knie l/r) bei 44-49, Fuss l (Mesh 25) bei 41.
  - K4 hatte dort Albedo L\* 24-40, grau (Kette 34-49); das Asset hat Albedo L\* 8-20 (Kette 14-27).
- **D2 Weissliche Gelenke der Arme:**
  - Bild 1: Ellbogen r (W) (Gelenkkoerper Ellbogen r, Zeile 11) T (50,4/-2,8/-3,2) und Hand r (Mesh 18) T-L\* 44. K4 Kette 51 bzw. 37, Asset 13.
  - Bild 2: Schulter l (Gelenk Schulter l + Mesh 11, Zeilen 13/15) T (44,2/6,3/-1,9) und Muffe Schulter r (Zeile 15) T (39,6/-3,2/0,2). K4 33/32, Asset 18/14.
- **D3 Ruecken-Glanzband unter der F1-Leuchtstelle (Bild 1):** (318,266), T (51,0/6,5/-11,4), auf Mesh 0. Das Asset legt dort zu 80 % die
  F1-Zeile 5 und 18 % Zeile 10, Kette 16,6; K4 hatte Albedo L\* 69 (Kette 49,6).
- **D4 Schwanzspitze fleischroetlich (Bild 1):** Mesh 6, T (53,3/11,9/3,1), 37,5 % rosa-rot (Farbton -3,8 Grad). K4 Kette L\* 44, Asset 22,5
  mit 2,6 % Rot. Daher liest der Nutzer sie als "einheitlich grau/blau".
- **D5 Rote Fleischstelle auf dem Buckel vorn (Bild 2):** Mesh 10 (Brust, in Pose 2 die Buckeloberseite), Flecken F01+F02 = 1380 px um (286-318, 179-185).
  - Vorlage T (33-41 / 16-22 / -4..-6): gedeckt mauve-rot, Video (17-22 / 30 / -2).
  - K4 hatte dort eine helle rosa Stelle (Albedo (46-64 / 6-22 / -2..-12), Kette 45-64), das Asset 12.
- **D6 Schwanzglieder-Glanz (Bild 1):** kleine kuehle Flecken T-L\* 43 an den Gelenkkoerpern Schwanz 3-4 und 4-5 (je 39 px). K4 hatte an 3-4 38, das Asset 20 bzw. 13.
- **Nicht verloren:**
  - Zaehne: Zeile 7, Albedo L\* 74,5, wie gewollt.
  - Maul-Band und F1: E30.
  - Der cremefarbene Unterkiefer von K4 war ein frueherer Nutzer-Einwand ("Kiefer nicht cremefarben") und gilt nicht als verloren.
  - Die hellen G4-Adern (V3R3) hat der Nutzer selbst entfernt ("Feuerrillen").

## 2. Regionen der Nutzerpunkte (Modell und Foto), Tafel `m2_regionen.png`

**Spaltenbedeutung** ("Zeile" = Asset-Zeile 0-15):
- Flaechen R / Z = Flaechen der Region / aller Flaechen dieser Zeile.
- Eintraege nur R = Eintraege, die ausser der Region niemand in der Zeile liest.
- Mitnutzer = Meshes mit Flaechen ausserhalb der Region auf denselben Eintraegen.
- Exkl. Texel = TIM-Texel, die nur Regionsflaechen bedecken.

### 2.1 Krallen vorn (Bild 2)

| | rechte Hand (W), Nutzermaske right_arm_front | linke Hand (W), left_arm_front |
|---|---|---|
| Krallen im Modell (Meshes) | 29, 20, 19, 31 (alle 1 Komponente, je 30 Flaechen, Geometrie = K4) | 14, 15, 28 sichtbar, 30 verdeckt (2 px) |
| sichtbar px / Laenge (bbox-Diagonale px) | 29: 1479 / 90,5; 20: 1734 / 74,6; 19: 1521 / 73,8; 31: 419 / 57,5 | 14: 357 / 56,6; 15: 461 / 62,4; 28: 292 / 38,3 |
| Spitzen im Nutzerumriss (Winkel < 75 Grad) | (274,443) und die Kerbe (229,351) am Kiefer; dazu eine schmale Aussparung bei x 203-208, y 427-470 | (439,418), (468,416); (406,260) ist die Schulter |
| Zuordnung Modellspitze -> Umrissspitze (Abstand) | 31 -> (274,443) 14,3 px; 29 94 px, 20 118 px, 19 90 px: **kein Gegenstueck** | 14 -> 7,4 px, 15 -> 2,0 px; 28 22,9 px: **kein eigenes Gegenstueck** |
| Pixel ausserhalb des Nutzerumrisses | 29: 241, 19: 213 (Logo), 20: 117, 31: 134 | 15: 71, 28: 47, 14: 17 |
| K4 (gleiche Krallen) sichtbar px | 29: 947, 20: 1311, 19: 1255, 31: 358 (Arm in K5c naeher an der Kamera, `k5_arm.py`) | 14: 336, 15: 461, 28: 293 |
| Vorlage T-Lab Krallen / Hand | (31,4/22,9/20,6) / (31,0/26,6/22,1): **dE 4, gleiche Farbe** | (45,3/22,2/21,7) / (41,8/21,7/20,4): **dE 3,8** |
| Asset-Kette Krallen / Hand | (34,7/26,0/20,4) / (15,6/23,1/15,1): Krallen +19 L\* | (37,6/33,8/29,0) / (22,5/30,2/25,9): +15 L\* |
| K4-Kette Krallen (K4-Pixel) | (32,3/14,1/3,6) | (35,9/29,7/14,0) |

- **Anzahl:** Die Vorlage zeigt je Hand hoechstens 2 einzeln lesbare Spitzen, das Modell 4 + 3.
- **Form:**
  - Kralle 29 (rechte Hand, nach oben links) ist mit 90,5 px die laengste. Sie hat 241 px ausserhalb des Umrisses; die Vorlage hat dort Feuer (weisse Flecken (160-230, 325-385)).
  - Der Nutzerumriss laesst diese Flecken aussen vor; hier als Feuer gewertet, siehe Offen 2.
  - Die Vorlage zeigt die rechte Hand als breite dunkle Masse mit einer duennen Spitze unten rechts (= Kralle 31).
- **Farbe:**
  - Zeile 1 (nur Krallen) mit 75 benutzten Eintraegen und 23 636 Texelzugriffen (375 Texel).
  - Albedo-Mittel (38,1/17,5/13,6); 47,2 % rot mit Rot-Lab (29,7/27,3/12,6), die Spitzen khaki (45,6/8,8/14,5).
  - Unter Feuerlicht ergibt das Rot eng 78,1 % gegen Vorlage 51,0 % (FARBE V3R5.0).
  - In der Vorlage sind die Krallen von der Hand nicht zu trennen (dE < 4). Das Rot dort ist der Feuerschein auf der ganzen Hand.
  - K4-Krallen: Zeilen 1/3 der NB2-Textur, Albedo (32,0/0,0/-1,3) bzw. (38,3/0,6/-2,3), 0 % rot.

### 2.2 Schwanzspitze (Bild 1)

| Region | Flaechen | Zeile | Eintraege (nur R / geteilt) | Mitnutzer | Exkl. Texel / bedeckt | Albedo (texelgew.) |
|---|---|---|---|---|---|---|
| Mesh 6 ganz | 80 von 770 | 2 | 83 (0 / 83) | Meshes 1-5 | 5 / 1817 | (11,7/4,9/-2,2), 2,8 % rot |
| am Fleck F15 sichtbar | 10 (Meshes 5, 6) | 2 | 72 (0 / 72) | 1-6 | 0 / 933 | (12,2/5,1/-2,4) |

Die Schwanzspitze hat keinen eigenen Eintrag und fast keinen eigenen Texel. Ein Umfaerben der Zeile 2 faerbt den ganzen Schwanz (770 Flaechen).

### 2.3 Verbindungsstuecke / Gelenke

**Gelenkkoerper** (31 Stueck, 1280 Flaechen):
- Sie teilen in jeder Zeile alle Eintraege mit dem umgebenden Glied. Ausnahmen: Kiefer-Kopf (4 eigene in Zeile 14), Muffe Kopf-Hals (7 in Zeile 0), Muffe Schulter r (5 in Zeile 11).
- Exklusive Texel gesamt: 1448 von 16 118.
- Die Gelenkkoerper kopieren die UV der naechsten Elternflaeche (`k_naht.kugel`); seit V3 liegt dort G4-Haut.

| Gelenk (Vorlagenfleck) | Flaechen | Zeile(n) (Flaechen) | Albedo Asset | exkl. Texel / bedeckt |
|---|---|---|---|---|
| Ellbogen r (B1-F04) | 40 | 11 (15), 15 (25) | (8,4/2,6/-1,1), (11,8/12,0/9,9) | 14 / 2497 |
| Huefte r + Muffe (B1-F07) | 40 + 40 | 4/8/9/10 bzw. 3 | (5,6/14,5/-14,7) Z10, Muffe (10,5/9,0/-4,5) | 9 / 1019, 133 / 1082 |
| Huefte l + Muffe (B1-F05) | 40 + 40 | 4/8/9/10 bzw. 3 | (6,4/13,2/-13,4) Z10, Muffe (11,4/8,8/-4,5) | 28 / 1055, 142 / 1136 |
| Knie l (B1-F08) | 40 | 3 | (11,8/9,7/-4,6) | 2 / 383 |
| Knie r (B1-F10) | 40 | 3 | (10,3/12,5/-5,9) | 12 / 267 |
| Hand r (B1-F12) + Muffe | 40 + 40 | 11/15 | (11,3/4,4/-1,2) / (18,0/12,8/11,6) | 198 / 1301 |
| Fuss l / Fuss r (B1-F13/F14) | je 40 | 3 | (12,5/7,7/-4,0) | 16 / 378, 12 / 429 |
| Schulter l (B2-F05) | 40 | 12 (26), 13 (14) | (13,0/16,2/-10,4) | 12 / 1557 |
| Muffe Schulter r (B2-F06) | 40 | 11 (4), 15 (36) | (14,5/11,7/9,8) | 102 / 3250 |
| Muffe Hals-Brust (B2-F12) | 80 | 13 | (14,3/7,3/-3,9) | 4 / 2511 |

An den hellen Flecken sind ausser Gelenkkoerpern auch Gliedflaechen sichtbar: Bild 1 59 Flaechen, Zeilen 2/3/10/11/12, 11 exklusive Texel von 3050.
Bild 2: 222 Flaechen, inklusive Krallen und Maul.

**Zielfarbe weisslich (Vorlage T):**
- Bild 1: L\* 41-57, a\* -3,5..+7, b\* -1..-15, also weiss-grau bis kuehl.
- Bild 2 (Feuer): L\* 40-44, a\* -3..+6, b\* -2..0.
- K4-Albedo dort: L\* 24-40 grau, Kette 34-51. Sie reichte in Bild 1 an die Vorlage heran (gruene Kreise in m1).

### 2.4 Lippe

**Lippensaum** = Flaechen der Meshes 22/23, die in 10 Kopfansichten (Clip 1 offen / Clip 2 Ruhe, je 5 Winkel) innerhalb von 4 px an Zahnpixeln liegen; ohne Zaehne.
140 Flaechen, davon **111 mit roter Albedo** (a\* > 15, C\* > 15):

| Zeile | rote Lippenflaechen / Flaechen der Zeile | Eintraege (nur Lippe / geteilt) | Mitnutzer | Albedo (texelgew.), Rot-Anteil |
|---|---|---|---|---|
| 0 (G4-Kopfzeile: Maulinneres F2, Augen) | 55 / 155 | 70 (0 / 70) | Meshes 22 (52 Fl.), 23 (12) | (31,9/39,4/18,7), 99,8 % rot |
| 6 (Maul-Band, Leuchtstelle Bild 2, E30) | 18 / 25 (Saum gesamt 23 / 25) | 79 (41 / 38) | Mesh 23 (7) | (54,6/24,5/19,2), 64,6 % |
| 14 (Kopf/Kiefer-Haut) | 38 / 232 | 63 (0 / 63) | 22 (173), 23 (21) | (10,7/14,6/-2,6), 44,9 % (G4-Adern) |

Exklusive Texel des roten Saums: 559 von 3030.

Die "komischen roetlichen Stellen" (Tafel m2 Z.3/4) sind:
- die roten G4-Zahnfleisch-/Maulinnen-Flaechen (Zeile 0), die bei geschlossenem Maul aussen am Saum liegen;
- die Lachs-Flecken des Maul-Bands (Zeile 6), die in Ruhe als unregelmaessige Rechtecke zwischen den Zaehnen sichtbar sind;
- rote Adern der Zeile 14 am Kieferrand.

Die Vorlage Bild 2 zeigt am Oberkiefer T (16,7 Mittel L\*) dunkel. Das Rot am Unterkiefer ist das Fleischband (E30).

### 2.5 Rote fleischige Stelle auf dem Buckel vorn (Bild 2)

| Region | Flaechen | Zeilen | Eintraege (nur R / geteilt) | Mitnutzer | exkl. Texel / bedeckt |
|---|---|---|---|---|---|
| Buckel, Flaechen an F01-F03 sichtbar | 14 (Mesh 10: 13, Meshes 21/22: 1) | 12 (10), 13 (3), 14 (1) | Z12 62 (0 / 62), Z13 49 (0 / 49) | Z12 nur Mesh 10 (121 Fl.), Z13 Meshes 10/21 | 3 / 1840 |

- Lage Foto: (271-347, 159-213), Mittelpunkte (286,185) und (318,179), zusammen 1380 px (1572 mit F03).
- Farbe Foto: Video (17-22 / 30 / -2), T (33-41 / 16-22 / -4..-6).
- Asset dort heute: Albedo (12/16/-10), Kette 12. K4: Albedo (46-64 / 6-22 / -2..-12).

## 3. Helligkeit

### 3.1 Mittleres L\*

Pixelmenge fuer alle Staende gleich: Nutzer-Teilmasken & Asset-Modell & ohne Weisskappe; Bild 1 29 706 px, Bild 2 42 350 px.
Spalten: T = Kette rueckwaerts, Video = mit Videofilter (3 Saaten), ohne = Disc-Licht x k ohne Filter bzw. video_umkehr der Vorlage.

| Bild 1 | Vorlage | K4 | K5c roh | V2 | V3R3 | Asset |
|---|---|---|---|---|---|---|
| Figur T / Video / ohne | 20,3 / 18,3 / 7,6 | 25,2 / 24,5 / 9,0 | 24,3 / 24,2 / 8,4 | 21,1 / 20,4 / 6,7 | 19,8 / 18,2 / 6,0 | **18,0 / 16,4 / 5,1** |
| Schwanz | 16,7 / 13,9 / 6,1 | 19,9 / 17,9 / 7,0 | | | | 18,1 / 15,4 / 6,0 |
| Ruecken/Flanke | 23,9 / 21,8 / 9,3 | 29,9 / 29,7 / 11,4 | | | | 16,3 / 14,6 / 4,4 |
| Hinterbein l | 23,6 / 22,4 / 9,0 | 27,6 / 27,3 / 9,4 | | | | 19,1 / 17,9 / 4,4 |
| Hinterbein r | 20,7 / 19,3 / 7,5 | 28,7 / 29,9 / 9,9 | | | | 24,1 / 25,1 / 6,3 |
| Vorderglied r | 21,0 / 19,8 / 7,7 | 26,0 / 26,4 / 9,1 | | | | 14,4 / 13,7 / 3,4 |

| Bild 2 | Vorlage | K4 | K5c roh | V2 | V3R3 | Asset |
|---|---|---|---|---|---|---|
| Figur T / Video / ohne | 24,9 / 15,4 / 7,3 | 26,9 / 15,1 / 8,3 | 26,4 / 14,4 / 8,0 | 24,5 / 14,2 / 7,3 | 20,3 / 12,4 / 5,4 | **18,8 / 10,9 / 4,9** |
| Oberkiefer | 16,7 / 9,3 / 4,2 | 24,8 / 13,8 / 7,3 | | | | 14,4 / 7,6 / 3,3 |
| Kopf/Rumpf | 18,9 / 9,1 / 5,1 | 21,8 / 9,8 / 6,4 | | | | 13,0 / 5,2 / 3,2 |
| Unterkiefer | 58,6 / 42,7 / 20,9 | 58,7 / 40,8 / 20,6 | | | | 47,6 / 34,7 / 15,4 |
| Klaue r | 30,7 / 21,8 / 9,4 | 32,0 / 19,2 / 10,1 | | | | 25,8 / 17,5 / 6,9 |
| Klaue l | 34,1 / 26,5 / 10,6 | 27,9 / 20,1 / 8,8 | | | | 24,3 / 18,4 / 5,7 |

### 3.2 Noetiger Helligkeitsfaktor

**Verfahren:** Die Asset-Pixel der Region im Disc-Licht-Bild werden mit f skaliert. Das ist linear wie Textur x Licht / 255 und damit gleich einer Albedo x f, solange nichts kappt.
- Mit Filter: f so, dass das Video-L\* (Saat 15) gleich dem Vorlagen-Video ist.
- Ohne Filter: f so, dass das Disc-Licht-L\* gleich der video_umkehr der Vorlage ist.
- Bisektion 16 Schritte.
- Pruefung am Endfaktor: 3 Saaten Video und T.

| Region | Bild 1 mit / ohne Filter | Pruefung bei f: Video (3 Saaten) / T (Vorlage T) | Bild 2 mit / ohne | Pruefung Video / T |
|---|---|---|---|---|
| **Figur** | **1,143 / 1,381** | 18,27 (18,29) / 19,78 (20,32) | **1,373 / 1,407** | 15,38 (15,37) / 25,04 (24,88) |
| Schwanz | 0,926 / 1,022 | 13,87 / 16,48 | | |
| Ruecken/Flanke | 1,514 / 1,892 | 21,76 / 22,36 | | |
| Hinterbein l | 1,413 / 1,764 | | | |
| Hinterbein r | 0,571 / 1,152 (Video: Bodensaum des versetzten Asset-Beins, siehe Offen 4) | | | |
| Vorderglied r | 1,714 / 1,927 | | | |
| Oberkiefer | | | 1,237 / 1,209 | |
| Kopf/Rumpf | | | 1,542 / 1,452 | 9,17 / 20,13 |
| Unterkiefer | | | 1,224 / 1,324 | |
| Klaue r / l | | | 1,246 / 1,295; 1,600 / 1,647 | |

**Je CLUT-Zeile** (nur die Pixel dieser Zeile skaliert, Ziel = Vorlage auf denselben Pixeln; mit / ohne Filter; px):

| Zeile | Bild 1 | Bild 2 |
|---|---|---|
| 0 | | 2,30 / 2,29 (516) |
| 1 (Krallen) | | 1,03 / 1,00 (3966) |
| 2 Schwanz | 0,94 / 1,03 (10 985) | |
| 3 Hinterbeine | 1,09 / 1,46 (7519) | 1,86 / 1,53 (178) |
| 4 | 1,56 / 2,01 (536) | 3,11 / 2,66 (142) |
| 5 F1 | 1,22 / 1,43 (2219) | |
| 6 Maul-Band | | 1,00 / 0,93 (1494) |
| 7 Zaehne | | 1,13 / 0,90 (449) |
| 8 | 1,19 / 1,08 (531) | |
| 9 | 1,78 / 2,92 (1601) | 2,50 / 2,66 (618) |
| 10 | 2,47 / 2,74 (1888) | 3,56 / 3,32 (90) |
| 11 Vorderbein r | 1,71 / 1,90 (3991) | 4,19 / 4,04 (2600) |
| 12 Brust | 1,67 / 1,67 (282) | 2,00 / 1,93 (3515) |
| 13 Hals | | 1,52 / 1,27 (9013) |
| 14 Kopf | - / 2,10 (125, Ziel unter Asset) | 1,39 / 1,41 (13 766) |
| 15 Vorderbeine | | 1,85 / 1,90 (6003) |

Lesart: Faktoren ueber 2 entstehen dort, wo die Vorlage helle Details hat (Gelenke, Buckel, Feuer-Randlicht) und die Zeile sie nicht hat.
Eine reine Zeilenaufhellung wuerde dort das Detail als Flaeche nachbilden.
Mindest-Helligkeit "wie die Vergleichsbilder": **Figur x 1,37 (Video) / x 1,41 (ohne Filter)**. Bild 2 ist der bindende Fall, Bild 1 ist mit x 1,14 / 1,38 erreicht.

### 3.3 Echtes Spielfenster (Tafel 11, gdigrab)

**Verfahren:**
- Figurmaske aus `form_buehne.buehne`: Raum-Kamera 5080 Schnitt 1 bzw. 5090 Schnitt 11, Pose wie im Spiellauf, Asset und K4 je mit eigener EMD.
- Deckung per `form_buehne.deckung`: Versatz 0/0, Abstand 2,7-8,4.
- Original: Video-Standbild in den Nutzer-Teilmasken ohne Logo.

| Szene | Asset K5c L\* Mittel / p90 | K4 L\* Mittel / p90 | Original-Standbild L\* Mittel / p90 |
|---|---|---|---|
| 5080 Nachstellung Bild 1 (35 891 / 32 501 / 32 327 px) | **3,15 / 4,75** | 5,87 / 11,79 | 19,31 / 40,99 |
| 5090 Pose Bild 2 (43 127 / 53 789 / 45 038 px) | **7,74 / 21,75** | 8,35 / 17,83 | 15,57 / 36,10 |

- Im Spielfenster ist das Asset in 5080 halb so hell wie K4 (L\* 3,2 gegen 5,9, p90 4,8 gegen 11,8).
- Das Original-Standbild ist eine Videoaufnahme (TV-Kette, Logo-Saum). Der absolute Abstand zum Fenster (L\* 19 gegen 3) misst also auch die Abfilmung. Vergleichbar ist das Verhaeltnis der Staende untereinander.
- In 5090 liegen Feuersprites vor der Figur und heben den Wert (p90 21,8).

## 4. Zeilenbudget (16 Zeilen, keine 17.)

| Zeile | Flaechen | Meshes | Eintraege benutzt / frei |
|---|---|---|---|
| 0 | 155 | 22, 23 | 95 / 161 |
| 1 | 252 | Krallen 14, 15, 19, 20, 28-31 | 75 / 181 |
| 2 | 770 | Schwanz 1-6 | 105 / 151 |
| 3 | 696 | Hinterbeine 8, 9, 24-27 | 70 / 186 |
| 4 | 62 | 0, 7 | 21 / 235 |
| 5 | 6 | 0 (F1) | 80 / 176 |
| 6 | 25 | 22, 23 (Maul-Band) | 82 / 174 |
| 7 | 114 | 22, 23 (Zaehne) | 33 / 223 |
| 8 | 45 | 0, 7 | 29 / 227 |
| 9 | 187 | 0, 7 | 21 / 235 |
| 10 | 152 | 0, 7 | 28 / 228 |
| 11 | 204 | 16, 17, 18 | 148 / 108 |
| 12 | 131 | 10 | 70 / 186 |
| 13 | 229 | 10, 21 | 70 / 186 |
| 14 | 232 | 22, 23 | 74 / 182 |
| 15 | 568 | 11, 12, 13, 16, 17 | 149 / 107 |

"Frei" = von keinem Texel einer Flaeche dieser Zeile gelesen. Ein neuer Wert dort aendert keine sichtbare Flaeche. Eintrag 0 ist in keiner Zeile transparent.

TIM 512 x 256: 22 763 Texel von Flaechen bedeckt, 108 309 frei. Groesstes freies Quadrat je Seite: 91 / 49 / 101 / 127 Texel.

**Wege je Nutzerpunkt** (Messbefund, keine Bauentscheidung):

| Nutzerpunkt | Weg |
|---|---|
| Krallen (Farbe) | Zeile 1 gehoert allein den 252 Krallenflaechen (Eintraege nur R 75/75, Texel exklusiv 375/375). Die Eintraege lassen sich direkt umfaerben, ohne andere Flaechen zu beruehren. Die Anzahl ist Geometrie (Meshes 29, 20, 19, 28): ausserhalb der Farbe, BLEIBT-Liste beachten (Offen 1). |
| Schwanzspitze | Zeile 2 teilt alle Eintraege mit Meshes 1-5; 5 exklusive Texel. Weg: eigene Texelinsel fuer die 80 Flaechen von Mesh 6 (bzw. die 10 am Fleck sichtbaren) im freien TIM-Bereich, Indizes auf freie Eintraege von Zeile 2 (151 frei). |
| Gelenke | Die 31 Gelenkkoerper liegen in 13 Zeilen und teilen Eintraege und Texel. Weg wie Schwanzspitze: Texelinseln, die freie Eintraege der jeweiligen Zeile lesen (3: 186, 11: 108, 12/13: 186, 15: 107 frei). Zeilen bleiben, nur UV dieser Flaechen und neue Eintragswerte aendern sich. |
| Buckel | Zeile 12 gehoert allein Mesh 10, aber nur 10 der 131 Flaechen sind Buckel; 3 exklusive Texel. Weg: Texelinsel fuer die 13 Buckelflaechen, Eintraege aus den 186 freien der Zeile 12. |
| Lippe | Zeile 6 liest fast nur der Saum (23/25 Flaechen; 70 Eintraege nur Lippe). Zeile 0 und 14 teilen alle Eintraege mit Maulinnerem bzw. Kopfhaut, exklusive Saum-Texel 559 von 3030. Ein Umfaerben der Zeile 6 trifft die E30-Leuchtstelle (Offen 3). |
| Helligkeit | Je Zeile ein Faktor auf alle benutzten Eintraege. Aendert keine Zuordnung, nur Werte. |

## 5. Offen

1. **Krallenzahl gegen BLEIBT-Liste:** "Form/Geometrie/Knochen bleiben" (E30) gegen "zu viele Krallen" (Nutzer 2026-11-08).
   - Weniger sichtbare Krallen gehen nur ueber Geometrie oder Pose: die Meshes 29/20/19 rechts und 28 links weglassen oder verkuerzen, oder Kralle 29 in der Pose aus dem Umriss nehmen.
   - Eine reine Farbaenderung (Krallen in Handfarbe wie in der Vorlage, dE < 4) macht sie nur unauffaelliger. Entscheid Orchestrator/Nutzer.
2. **Weisse Flecken bei (160-230, 325-385) in Bild 2:** Der Nutzerumriss right_arm_front laesst sie aus. Hier sind sie als Feuer gewertet und nicht als Krallen. Waeren es Krallen, haette die Vorlage dort 3-4 helle Spitzen.
3. **Lippe gegen E30-Leuchtstelle:** 23 der 25 Flaechen des Maul-Bands (Zeile 6) gehoeren zum Lippensaum. "Durchweg grau/blau" an der Lippe und "Leuchtstelle Maul bleibt" schliessen sich fuer Zeile 6 aus.
   Ohne Konflikt sind die roten Zeile-0-Flaechen aussen am Saum (55) und die Adern der Zeile 14 (38 Flaechen). Rueckfrage noetig, ob das Band in Pose 2 (Maul offen) bleiben soll.
4. **Hinterbein r Bild 1, Faktor 0,57:** Das Asset-Bein liegt neben dem Vorlagenbein auf hellem Boden. Der Videofilter traegt den Boden ein, das ist kein Hinweis auf zu hell. Ebenso Fleck F14 Fuss r: Das Asset deckt ihn nicht (Pose).
5. **Spielfenster gegen Video-Standbild:** nur relativ vergleichbar (3.3). Ein absolutes Ziel im Fenster gibt es aus den Fotos nicht.
6. **Kennzahl "Details" als Riegel fuer den Bauer (Vorschlag):** Anteil T-L\* > 40 in der Figur, Bild 1 >= 9,9 %, Bild 2 >= 14,5 %.
   Dazu Ketten-L\* je Gelenkfleck >= 80 % der Vorlage (m1-Kreise gruen). Die F1-, Band- und Rot-Riegel (RV1-RV6) gelten weiter.
