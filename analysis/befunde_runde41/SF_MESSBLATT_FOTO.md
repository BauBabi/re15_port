# SF Messblatt Fotovorlage: Foto 1/2 gegen V5r4 in der passenden Pose (Messer, 2026-11-08)

Kein RE-Befund: Die Sichel-Form gibt es auf der Disc nicht. Alles hier ist Bildmessung an den beiden Originalfotos
(Version_80%/Beschreibung1_Vorlage.png = n1485, Beschreibung2_Vorlage.png = n1435, Spielkamera 50A0 Cut 12) und
an der Variante V5r4 (sfv4/emd/EM30_V4.EMD, sha256 38562ced...). Am Modell, am Spiel-Asset (EM30_HANGAR.EMD cfd1ac42)
und unter Version_80%/, pri/ und synchro/ ist nichts geaendert. Keine Zahl hier ist eine @0x-Konstante.

Seiten: **(B)** = Sicht des Betrachters, **(W)** = Sicht des Wesens (Messblatt V4 Abschnitt 1). Masse in H = Figurhoehe
(Foto: H1 = 302 Messblatt V4, H2 = 253; Modell: H_port der Pose, Oberkante bis Sohle ohne Schwanz). Lab = CIE-Lab D65;
Foto 1 entblendet (I' = 17 + (I - 17) / 0,766), Foto 2 roh; Modell im Videoraum (Gauss 1,5, V = a + b P, v4_gemein).

Werkzeuge: `re15_port/tools/r41_sichel/foto/` (Python, ohne -I). Rohdaten: `SF_belege/foto/daten/*.json`.

| Werkzeug | misst | Ausgabe |
|---|---|---|
| f_maske.py (+ f_gitter.py) | Umriss als Trimap (Figur / Hintergrund / unbekannt), Handspur-Polygone | trimap_*.npy, Beleg 30/31 |
| f_pose.py | Pose + Gier + Lage, alle 912 Keyframes x 72 Gier, IoU + Anker, Feinsuche | pose_*_lauf1.json, pose_2.json |
| f_pose_sohle.py | Foto 1: dieselbe Suche mit fester Sohle (Lage geloest), Gier frontal | pose_1.json, pose_1_sohle442.json |
| f_rast.py | Rasterer mit Pixelprotokoll (Knochen, Flaeche, CLUT-Zeile, Texel, Palettenindex, Knochenraum) | - |
| f_kopf_foto.py | Fotoseite Kopf: obere Kontur, Auge, Maulband, Haarfeld | kopf_foto.json |
| f_analyse.py | Modellseite in der Pose: Teile-Karte, Rot-Karte, Abweichungsliste, Kopf/Maul/Auge/Haar/Kontur | analyse.json |
| f_zusatz.py | Rot je fester Bildregion, Huckel-Liste, Kopfhoehe, Maul in der Vorderansicht "135" | zusatz.json |
| f_schwanz.py, f_schwanz_geo.py | Schwanz-Stationen Foto/Modell, Gliedgrenzen, Radien je Glied | schwanz.json |
| f_krallen.py | Krallen Modell (Komponenten Mesh 12) gegen Foto (n1476/1479/1485) | krallen.json |
| f_tafeln.py, f_kandidaten.py | Leisten | SF_belege/foto/*.jpg |

## 0. Kurzantworten auf die Nutzerpunkte (Befund, kein Bauvorschlag)

| Nutzer | Befund (Abschnitt) |
|---|---|
| (1) Seitenansicht ohne Auge, ohne dicken Schwanz | Neue Vergleichsansicht Foto 2: Clip 12 Bild 48 Gier 861 (75,7 Grad), IoU 0,758 (alt 0,722), Kopf-Anker 0,70 (alt 0,32). Auge: im Foto heller Fleck L 48,5 an (452,5/237,2), (W)-rechts; im Modell dort L 20-22, kein heller Fleck (3.3). Schwanz: Modell 0,146 H zu kurz (Spitze y 306 statt 343), heller und roter (L 28,4, a* 8,7 gegen L 17,8, a* 4,8) (5). |
| (2) grauer Schnabel ueber dem Mund | Ueber/vor dem Maul liegen die Kopf-Flaechen um die vorderste Ecke (310,-45,-87) von Mesh 9: v13 (Zeile 486), d11, d19, d20, d23 (alle Zeile 486); sie verdecken das Maul in Pose 1 (18 px) und Pose 2 (alle 13 px des Maulumrisses) (3.2). |
| (3) Schwanz grau, ein Stueck, ohne Huckel | Foto: Schwanz dunkel neutralgrau mit leichtem Rotstich (17,8 / 4,8 / 1,5), Rumpfhaut 23,8 / 3,9 / 1,5, dE00 4,3. Modell-Huckel: Gliedgrenze 18/19 +10 px Breitensprung bei 90 Grad Knick, 17/18 +4 px (Radius +29 % im Knochenraum), 21/22 Endglied breiter als 21 (+41 %); Schwanz-Oberkante am Bogen 7-13 px ueber der Foto-Kontur (5). |
| (4) Ruecken -> Kopf fliessend, Huckel davor | Foto 2: obere Kontur stetig, keine Stufe >= 3 px. Modell: Rumpf-Flaechen v19/v14/v8/d5 (Knochen 8, Buckel) liegen direkt vor dem Kopf 8-14 px (0,03-0,055 H) ueber der Foto-Kontur; Modell-Kopf-Oberkante 0,085 H unter der Figur-Oberkante, im Foto 0,016 H (3.5). |
| (5) roter Fleck rechte Schulter | Foto 1 Region Schulter/Oberarm Bild-links = (W)-rechts: rot 0,5 %; Modell dort 49 %. Flaechen 13/d30, d32, d33 (Zeile 480), 8/v24 (480), 8/v26 (481) (4). |
| (6) Mund dunkelgrau geschlossen | Foto 1: dunkles Band y 183-187 unter der Nase ueber das ganze Messfenster x 376-414, Lab 13,1 / 8,5 / 2,7, kein heller Pixel. Modell Vorderansicht: Maul 0,096 x 0,085 H, Lab 26,5 / 12,9 / 8,3, 27 helle Zahn-Pixel L 48,9 (3.2). |
| (7) links kein Auge, Haarbogen | Foto 1: dunkles Feld (B)-rechts = (W)-links, L 11,9, Bogenkante abgemessen; Foto 2: dunkler Streifen an der Kopf-Vorderkante = (W)-linke Kopfseite (zeigt in Pose 2 von der Kamera weg und liegt am Bildrand des Kopfes). Modell: dort kein dunkles Haarfeld (3.4). |
| (8) Rot korrekt Brust + oberer linker Arm, sonst grau | Foto 1 Brust Mitte 48 % rot; Foto 2 der unter dem Buckel haengende Oberarm (Bild-links der Kopfmitte, Id-Puffer = Knochen 13, (W)-rechts) 51 % rot, Modell 49 % (stimmt). Zu rot im Modell: Buckel oben (Foto 2: 41 % gegen 78 %), Schulter (W)-rechts/(W)-links vorn, Oberarm (W)-links, Sichel-Unterarm, Schwanz (4). |
| (9) Krallen linke Hand falsch und zu klein | (W)-linke Hand nur in Foto 1 sichtbar (Bild-rechts). Foto: ein heller Krallenkeil L 30 / a* 4,9, 12 x 11 px (0,040 x 0,036 H). Modell: 4 duenne Krallen, dunkel L 13,1 / a* 8,3 (wie die Handflaeche L 13,6): sie heben sich nicht ab (6). |

## 1. Umriss (Trimap)

Ein Schwellen-Umriss ist unbrauchbar: dE76 Foto gegen BSS 50A0/12 (Videoraum) liegt im dunklen Tuerraum innen wie aussen
bei 3-6 (auto_*_t5/t8.png). Daher halbautomatisch: Handspur-Polygone an Gitterbildern (x4, Gamma 0,6, 10-px-Gitter),
in Foto 2 vereinigt mit der Schwanzmaske der Leser V4 (Bewegung/Chroma). Zonen ohne ablesbaren Rand sind UNBEKANNT und
zaehlen in keiner IoU.

| Foto | Figur sicher | unbekannt | Oberkante | Unbekannt-Zonen |
|---|---|---|---|---|
| 1 (n1485) | 33 821 px | 26 055 px | y 139 | Sichel Bild-links x 282-355 y 245-425 (dunkel vor dunklem Tuerraum); Spalt Rumpf/Arm x 440-485 y 255-350; Fuesse/Schatten x 340-480 y 370-450 |
| 2 (n1435) | 19 579 px | 135 980 px | y 186 | y >= 346 (gruener Vordergrund verdeckt die Beine); x >= 515 (oranger Vordergrund); Spalt Schwanz/Arm; dunkler Bereich vor dem Pfosten x 436-462 |

H: Foto 2 H2 = 253 (Messblatt V4). **Foto 1: Sohle neu y 448 +-5 statt 441-443.** Die Figur kommt naeher: Fusssohle
n1470 ~437, n1475 ~442, n1479 ~445, ab n1482 unter der Letterbox-Kante y 452; der Bodenschatten wandert n1470 -> n1485
um ~10 px nach unten (Beleg 32). Damit H1(n1485) ~ 309.

## 2. Pose / Drehung (Vergleichsansicht)

Suche: alle 912 Keyframes (21 Clips, je Keyframe-Index einmal) x Gier 0-355 Grad in 5 Grad, je Kandidat Umriss-IoU
(Trimap) und Anker; Feinsuche Gier/Bild/Lage. Der Umriss allein trennt vorn/hinten nicht (reine IoU-Besten fuer Foto 2:
Clip 12 Bild 46 Gier 150 Grad, IoU 0,81, aber Kopf am falschen Ende). Darum Gesamtwert = 0,5 IoU + 0,5 Mittel der Anker:
Kopf (Schwerpunkt Knochen 9 gegen Mitte Kopfbox V4, max(0, 1 - d/40 px)), Foto 1 Klauenhand (Knochen 12 gegen rote
Hand x 476-510 y 332-374), Foto 2 Schwanz (IoU Knochen 16-22 gegen Schwanzmaske), Sohle (max(0, 1 - |d|/120 px)).
Gleichstand (Gesamt innerhalb 0,003): hoechste IoU.

| Foto | Pose alt (posen_v4) | IoU alt | Anker alt | **Pose neu** | **IoU neu** | Anker neu |
|---|---|---|---|---|---|---|
| 1 | "135": Clip 0 Bild 9 Gier 1737 (153 Grad) x -9279 z -22511 | 0,706 | Kopf 0, Klauenhand 0, Sohle 0,48 | **Clip 20 Bild 139 Gier 1422 (125 Grad) x -8420 z -21908**, Sohle y 449 | **0,709** | Kopf 0, Klauenhand 0,97, Sohle 0,99 |
| 2 | "85": Clip 1 Bild 26 Gier 556 (49 Grad) x -8070 z -21202 | 0,722 | Kopf 0,32, Schwanz 0,52, Sohle 0,99 | **Clip 12 Bild 48 Gier 861 (75,7 Grad) x -8182 z -21330** | **0,758** | Kopf 0,70, Schwanz 0,60, Sohle 0,99 |

Foto 1, verworfene Laeufe (Daten in SF_belege/foto/daten):
- Lauf 1 frei (pose_1_lauf1.json): Clip 11 Bild 89 Gier 1475, IoU 0,751, aber Sohle y 513 (H 359 statt ~309): das zu
  grosse Modell fuellt den Oberkoerper; Beine/Fuesse liegen in der Unbekannt-Zone und bremsen das nicht.
- Sohle fest 442, alle Gier (pose_1_sohle442.json): bester Wert Clip 7 Bild 1 Gier 3380 (297 Grad), Kopf-Anker 0,99, aber
  Klauenhand-Anker 0: Rueckansicht (Klauenhand Bild-links). Foto 1 zeigt die Klauenhand Bild-rechts und die Sichel
  Bild-links -> vorn. Darum Lauf 2: Sohle 448, Gier nur 1225-2249 (108-198 Grad, um "135").
- **Kein einziger Kandidat (912 x 72 frei, 912 x 36 und 912 x 19 mit Sohle) erreicht in Foto 1 einen Kopf-Anker > 0 bei
  Vorderansicht:** Der Kopf liegt im Modell tiefer zwischen den Schultern als im Foto (Abschnitt 3.5). Die IoU ist darum
  kaum besser als alt (0,709 gegen 0,706); gewonnen sind Hand- und Sohlenlage. Foto 1 ist frontal und symmetrisch; die
  neue Pose ist 28 Grad weiter gedreht als "135" (Beleg 21). Fuer Kopf-/Maulvergleich von vorn gilt zusaetzlich "135".

Sichtkontrolle (selbst gelesen): Beleg 02/22: Foto 2 und Modell decken sich in Buckel, Schwanzbogen und Kopfseite
(Kopf rechts, Schwanz links nach unten); Beleg 01/21: Foto 1 Hand und Sohle passen, Kopf/Schulterhoehe nicht.

Blickrichtung (Kamera-Raum, Knochen 9): Foto 1 zeigt das Gesicht nach vorn (vorn dz -246) und (W)-rechts nach Bild-links
(dx -282). Foto 2: vorn dz -104 / dx +178 (Wesen blickt nach Bild-rechts, leicht zur Kamera), (W)-rechts dz -189 (zur
Kamera), (W)-links dz +189 (abgewandt). **Beide Fotos zeigen dieselbe Wesensseite: das helle Auge ist (W)-rechts.**

## 3. Kopf

### 3.1 Kopfumriss
- Foto 2 obere Kontur (dE > 10 gegen BSS, je Spalte) x 336-466: 193 -> 185 (x 387-412) -> 197 (x 440) -> 210 (x 455) ->
  229 (x 465): eine stetige Kuppe, keine Stufe >= 3 px (nur Maskenrauschen x 332-335 am Schwanz). Kopf und Buckel
  bilden EINE Rundung, der Kopf faellt nach vorn ab, keine Spitze.
- Foto 1: Kopf ist der hoechste Punkt der Figur (y 135-139 bei x 385-396), Kopfbox 0,159 x 0,172 H1.
- Modell: Kopfbox Pose 1 0,160 x 0,181 H (passt), Pose 2 0,104 x 0,166 H gegen Foto-Kopfbox V4 0,217 x 0,289 H2
  (die V4-Box schliesst Hals/Buckelrand ein; als obere Schranke).

### 3.2 Maul (Nutzer 2, 6)
| | Foto | Modell |
|---|---|---|
| Lage | Foto 1: dunkelste Zeile y 185 (Zeilenmittel L 12,6), Band y 183-187 direkt unter der Nase (395,177) | Vorderansicht "135": Maul (Zeile 487) Box x 348-382 y 189-219, 622 px sichtbar |
| Groesse | Band fuellt das ganze Fenster x 376-414 (>= 39 px = 0,129 H1); keine Oeffnung, keine Kante | 0,096 x 0,085 H (offenes Maul mit Zahnkachel) |
| Farbe | 13,1 / 8,5 / 2,7 (dunkelgrau, leicht rotbraun), dunkelste Pixel 11,8 / 8,6 / 2,8; **0 Pixel L > 45** | 26,5 / 12,9 / 8,3; **27 Pixel L > 45** (Zaehne, 48,9 / 13,2 / 12,4) |
| Foto 2 | Unterseite Kopfvorderkante x 436-464: L faellt von 22,8 (y 244) auf 13,9 (y 265), Lab 14,0 / 1,0 / -0,7, 0 helle Pixel | Pose 2: Maul ganz verdeckt (13 px Umriss, 0 sichtbar) |

Graue Ueberdeckung des heutigen Mauls (Verdecker = Flaechen, die im Maulumriss vorn liegen):
- Pose 1: 9/v13 (Zeile 486) 18 px. Pose 2: 9/d11 7, 8/d72 3, 9/d23 2, 8/d67 1. Vorderansicht "135": 8/v15 8, 8/d12 7.
- Kopfflaechen direkt ueber dem Maul ("135", bis 6 px): 9/d36 (488) 69, 9/d38 (488) 56, 9/v13 (486) 30, 9/v1 (488) 30,
  9/d49 13, 9/d55 9, 9/d53 2.
- Knochenraum Mesh 9: v13 = (310,-45,-87) (304,-191,-37) (315,-49,10) (298,-155,26); d11/d19/d20/d23 teilen die Ecke
  (310,-45,-87) mit (181,144,-259), (202,-79,-160), (204,200,-154), (218,-179,-161). Das ist die vorderste Kuppe des
  Kopfes (x max 315) = der graue "Schnabel".

### 3.3 Auge (Nutzer 1, 7)
| | Foto 1 | Foto 2 |
|---|---|---|
| Fleck | Schwerpunkt (376,9/167,2), 8 x 10 px, 56 px, Lab 47,0 / 7,2 / -1,1, Extrem L 55,2 | (452,5/237,2), 10 x 14 px, 77 px, Lab 48,5 / 3,9 / 0,6, Extrem L 57,6 |
| Seite | (B)-links = (W)-rechts (Projektion Kopf (W)-rechts/(W)-links 71 / 99 px entfernt) | (W)-rechts (19,0 / 21,9 px; (W)-rechts zeigt zur Kamera, (W)-links weg) |
| Modell an der Stelle | Pose 1: Hintergrund (Kopf liegt 48 px rechts, 44 px tiefer, Kopfmitte (440,209) gegen (392,165)) | Pose 2: Knochen 10 Zeile 480, L 19,9-22,5 (5x5), kein heller Fleck |

### 3.4 Haarbogen (Nutzer 7)
- Foto 1, dunkles Feld (B)-rechts = (W)-links (L < 16 entblendet, 4er-Komponente um (400,165), Kopfbox y <= 181):
  496 px, x 391-416, y 150-181, Lab 11,9 / 7,1 / 1,6 (dunkel, braunrot-stichig). Oberkante (Kopfbox-Anteile u,v;
  u 0 = Bild-links): u 0,52-0,75 bei v 0,21-0,29 (y 150-154), Senke u 0,77-0,83 bei v 0,46-0,52 (y 163-166), wieder
  u 0,85-0,98 bei v 0,37-0,42 (y 158-161). Volle Liste kopf_foto.json "haar_1" (Schwellen 14/16/18: 349/496/650 px).
- Foto 2: dunkler Streifen an der Kopf-Vorderkante x 455-466, y 222-252, 2-6 px breit, Lab 21,7 / 0,9 / 2,4, unmittelbar
  vor dem Augenfleck: die (W)-linke Kopfseite liegt in Pose 2 an der Bild-rechten Kopfkante (Projektion (W)-links
  (458,216)) - der Streifen ist dieselbe Haarmasse von der Kante gesehen.
- Modell: an den Foto-Haarpixeln Pose 1 nur Rumpf (8/Zeile 480, 402 px, L 33,8); Pose 2 Kopf 11 px Zeile 486/488,
  L 28,1 / -1,7 / 0,5. Kein dunkles Haarfeld auf der (W)-linken Kopfseite.

### 3.5 Obere Kontur Ruecken -> Kopf (Nutzer 4)
Huckel = Spalten, in denen die Modell-Oberkante >= 4 px ueber der Foto-Kontur liegt (zusatz.json "huckel", rote Striche
in Beleg 01/02):
- **Foto 2, x 394-424 direkt vor dem Kopf (Kopf ab x 433):** Knochen 8 Flaechen v19 (x 400-418, 9-14 px), v14, v8, d5,
  d55, Zeile 480; Knochenraum x -616..-342, y -1254..-1050, z -316..-294 = Buckel-Oberseite hinten (W)-rechts.
  Maximal 14 px = 0,055 H bei x 418.
- Foto 2, x 300-312 (Schwanzbogen): Knochen 18 Flaechen v5/v6/v18, 6-13 px.
- Foto 2, x 450-454: Kopf 9/d14, 4-6 px.
- Foto 1 (Pose 1), x 426-456: Knochen 8 v4/v18/v19/d50, 4-17 px.
- Kopfhoehe: Modell-Kopf-Oberkante unter der Figur-Oberkante 0,080 H (Pose 1) / 0,085 H (Pose 2); Foto 0,000 H1 /
  0,016 H2. Im Foto ist der Kopf (fast) der hoechste Punkt; im Modell ragt der Buckel 0,07-0,08 H ueber den Kopf.

## 4. Koerperfarbe (Nutzer 5, 8)

Rot = a* >= 10 (Figur-Median a* Foto 1 3,4 / Foto 2 4,5; P90 10,6 / 12,1). Grau-Gegenprobe = Foto a* < 6.

### 4.1 Rot je fester Bildregion (posenunabhaengig, nur Figur sicher)
| Foto | Region (Box) | Foto rot | Foto Lab | Modell rot (gleiche Box, Pose neu) |
|---|---|---|---|---|
| 1 | Kopf (368-416, 139-191) | 19 % | 20,3 / 8,3 / 2,2 | 49 % |
| 1 | **Brust Mitte (360-450, 192-245)** | **48 %** | 16,9 / 9,9 / 4,2 | 21 % |
| 1 | **Schulter/Oberarm Bild-links = (W)-rechts (296-355, 175-250)** | **0,5 %** | 21,2 / 4,0 / 1,7 | **49 %** |
| 1 | Schulter/Oberarm Bild-rechts = (W)-links (455-512, 178-265) | 2,2 % | 21,6 / 4,4 / -0,7 | 30 % |
| 1 | Unterarm/Hand Bild-rechts (478-512, 265-372) | 9,6 % | 16,9 / 4,0 / -2,0 | 9,5 % |
| 1 | Bauch/Huefte (360-445, 250-300) | 0 % | 10,0 / -0,9 / -0,7 | 9,2 % |
| 2 | Kopf (412-467, 190-263) | 23 % | 25,8 / 7,0 / 2,2 | 25 % |
| 2 | **Buckel/Ruecken oben (330-410, 186-225)** | 42 % | 32,8 / 8,3 / 2,0 | **78 %** |
| 2 | **Schulter/Oberarm Mitte (360-430, 228-300)** | 28 % | 26,2 / 6,9 / 1,9 | 28 % |
| 2 | Schwanzbogen (280-335, 222-262) | 8 % | 16,4 / 5,1 / 1,6 | 11 % |

### 4.2 Rot je Koerperteil (Id-Puffer der Pose; Foto 2 belastbar, Foto 1 wegen Kopf-/Schulterversatz nur grob)
Foto 2 (Teil: px, Foto rot / Modell rot): Oberarm (W)-rechts 3735 px 51 % / 49 %; Schulter (W)-rechts 2168 px 55 % / 61 %;
Ruecken/Buckel 565 px 19 % / 34 %; Kopf 765 px 26 % / 0 %; Brust 452 px 4 % / 15 %; Unterarm (W)-rechts (Sichel) 2172 px
0 % / 20 %; Schwanz 3261 px 15 % / 31 %; Bauch 7 % / 4 %; Oberarm (W)-links 1 % / 9 %.
Foto 1: Brust 7 % / 34 %; Schulter (W)-rechts 49 % / 25 %; Oberarm (W)-rechts 8 % / 48 %; Oberarm (W)-links 0 % / 35 %;
Ruecken/Buckel 0 % / 41 %; Schulter (W)-links 0 % / 13 %. (Die Foto-1-Zuordnung ist durch den tieferen Modellkopf
verschoben: die Foto-Brustroete faellt dort auf Modell-Kopf/Schulter.)

### 4.3 "linker Arm" (Nutzer 8)
Im Foto 1 ist weder der Bild-linke ((W)-rechte, 0,5 %) noch der Bild-rechte ((W)-linke, 2,2 %) Oberarm rot. In Foto 2 ist
der unter dem Buckel haengende Oberarm rot (28 % Region, 51 % Teil); er liegt Bild-links der Kopfmitte und ist laut
Id-Puffer der **(W)-rechte Oberarm (Knochen 13, Sichelarm)**. Befund: "oberer linker Arm" = **Bild-links in Foto 2 =
(W)-rechter Oberarm**; seine Roete stimmt im Modell (49 %). In Foto 1 (von vorn) ist derselbe Arm oben grau: die Roete
sitzt auf der Seiten-/Hinterflaeche des Oberarms, nicht auf der Vorderseite.

### 4.4 Abweichungsliste: Modell rot, Foto grau (a* < 6) - Flaechen / Texel / CLUT
Volle Liste mit Palettenindizes: analyse.json "rot_abweichung" (je Foto). Groesste Posten (px = Bildpixel):

| Foto | Knochen/Flaeche | Teil | px | CLUT-Zeile | Texel u / v | Texel | Mitnutzer anderer Knochen |
|---|---|---|---|---|---|---|---|
| 1 | 8/d32 | Brust | 280 | 481 | 191-223 / 43-53 | 185 | 4 (nur Knochen 8) |
| 1 | 14/v9 | Unterarm (W)-rechts | 172 | 482 | 275-295 / 89-111 | 160 | 13 (Klauenhand 12/d11, d12, d16, d17, d23, d25 ...) |
| 1 | 8/v15, 8/d12, 8/d2, 8/d23 | Brust | 155/155/148/138 | 481 | 179-211 / 25-55 | 81-102 | 0-7 (nur 8) |
| 1 | 10/v8, 10/v5, 10/d14 | Oberarm (W)-links | 148/121/79 | 480 | 16-30 / 9-76 | 71-116 | **21-41 (Beine 1/5, Arm 13, Rumpf 8)** |
| 1 | 8/v18, 8/v8 | Ruecken/Buckel | 109/76 | 480 | 9-27 / 25-35 | 53-80 | 9-16 (Arme 10/13) |
| 1 | **8/v26, 8/v24** | **Schulter (W)-rechts** | 107/78 | 481/480 | 156-182 / 44-52; 55-69 / 24-36 | 80/50 | 2 / 25 |
| 1 | **13/d32, d33, d30** | **Oberarm (W)-rechts (Schulterkuppe)** | 104/76/76 | 480 | 101-118 / 25-47 | 64-83 | 10-11 (Beine 3/6) |
| 2 | 14/v9, 14/v10 | Unterarm (W)-rechts | 238/105 | 482 | 256-290 / 86-108 | 238/103 | 15-24 (Klauenhand 12) |
| 2 | 17/v7, 16/v7, 16/v6, 17/v6, 17/v8 | Schwanz | 153/140/63/50/26 | 480 | 320-335 / 21-84 | 22-107 | 3-7 (nur 16-18) |
| 2 | 8/d56, 8/d26 | Schulter (W)-rechts | 68/29 | 480 | 49-66 / 29-52 | 64/19 | 25/12 |
| 2 | 8/v13, 8/d73 | Ruecken/Buckel | 50/31 | 480 | 1-24 / 49-63 | 36/14 | 16/7 |

Texel-Mehrfachnutzung: Die Oberarm- und Schulter-Texel der Zeile 480 (u < 70) werden von 20-40 fremden Flaechen
(Beine, beide Arme, Rumpf) mitbenutzt; eine Texelaenderung dort faerbt alle mit. Die Brust-Texel Zeile 481 (u 151-224)
und die Schwanz-Texel (u 320-335) sind fast exklusiv.

## 5. Schwanz (Nutzer 1, 3)

Foto 2, Mittellinie Beginn (357,243) -> Biegung (295,252) -> Knick (290,308) -> Ende (295,343), 11 Stationen; Modell:
Gelenkkette 16..22 + Spitze in Pose 2. Breite = Lauf der Maske quer zur Achse (Foto H2 = 253, Modell H = 259).

| f | Foto Mitte | Foto Breite px (H) | Foto Lab | Modell Mitte | Modell Breite px (H) | Modell Lab |
|---|---|---|---|---|---|---|
| 0,0 | (354,224) | 41 (0,162) | 17,2 / 7,7 / 0,5 | (388,215) Gelenk 16 verdeckt (Knochen 13) | - | - |
| 0,1 | (339,228) | 46,5 (0,184) | 14,6 / 7,5 / 0,7 | (366,200) | 4,5 (Randlauf) | - |
| 0,2 | (325,240) | 36,5 (0,144) | 11,3 / 6,4 / 1,7 | (354,220) | 35 (0,135) | 28,1 / 13,3 / 7,9 |
| 0,3 | (310,240) | 41 (0,162) | 16,4 / 5,0 / 1,8 | (338,225) | 37 (0,143) | 24,4 / 12,7 / 7,1 |
| 0,4 | (297,260) Biegung | 57 (0,225) | 16,9 / 3,7 / 2,6 | (320,229) | 40 (0,154) | 27,9 / 5,8 / 2,7 |
| 0,5 | (292,266) | 21 (0,083) | 14,7 / 3,5 / 2,3 | (304,235) | 33 (0,127) | 31,7 / 6,7 / 3,6 |
| 0,6 | (291,282) | 26 (0,103) | 14,0 / 2,8 / 3,2 | (309,240) Gelenk 18/19 | 48,5 (0,187) | 29,0 / 8,2 / 4,1 |
| 0,7 | (279,296) | 11,5 (0,045) | 16,8 / 0,4 / 0,5 | (300,262) | 11,5 (0,044) | 15,9 / 9,6 / 4,7 |
| 0,8 | (281,314) Knick | 15 (0,059) | 26,4 / 2,0 / 1,6 | (292,274) | 8,5 (0,033) | 27,6 / 3,8 / 2,7 |
| 0,9 | (289,328) | 10 (0,040) | 12,8 / 3,8 / 1,3 | (283,289) | 8,5 (0,033) | 26,9 / 7,6 / 2,7 |
| 1,0 | (292,343) Ende | 13,5 (0,053) | 16,5 / 2,8 / 0,9 | (280,306) Spitze | 8 (0,031) | 30,6 / 4,3 / 1,8 |

- Achslaenge: Foto 154 px = 0,61 H2 (ab Beginn-Punkt); Modell 171 px = 0,66 H, aber **Spitze y 306 statt 343: das
  Modell endet 37 px = 0,146 H zu hoch, auf Hoehe des Foto-Knicks.** Das Foto-Endstueck Knick -> Ende (0,145 H) fehlt.
- Foto-Breite: am Ansatz 0,16-0,18 H, an der Biegung bis 0,225 H, darunter 0,04-0,10 H; Modell an der Wurzel nicht
  sichtbar (Gelenk 16 hinter dem (W)-rechten Oberarm, Knochen 13), sonst 0,13-0,19 H oben, 0,03-0,04 H unten.
- **Farbe Foto:** gesamt 17,8 / 4,8 / 1,5 (4888 px), a* < 0 nur 5,5 %, Median a* 4,4 / b* 1,4; zur Spitze grauer
  (f 0,7-1,0: a* 0,4-3,8). Rumpfhaut grau (Foto 2, a* < 6) 23,8 / 3,9 / 1,5: **dE00 4,3**. Ref2-Sichel (Mesh 15)
  gerendert 19,2 / -0,9 / 2,7, Texel 22,4 / -0,8 / 4,3: dE00 8,0 / 8,8. Modell-Schwanz heute 28,4 / 8,7 / 4,7: dE00 8,9
  (heller und roter = "rosa"). Lab-Befund: der Foto-Schwanz ist dunkles Grau mit leichtem Rotstich, nicht gruen
  (a* > 0); er ist aber deutlich weniger rot als das Modell und naeher an der grauen Rumpfhaut als an der Sichel.
- Uebergang Wurzel -> Ruecken (Foto): obere Kontur x 336-412 stetig 193 -> 185, keine Stufe; die Schwanz-Oberseite geht
  ohne Kante in den Buckel.
- **Modell-Huckel je Gliedgrenze (Pose 2, Breite 4 px vor/nach dem Gelenk, Knick der Kette):**
  16/17 +2,5 px (1,9 Grad); 17/18 +4 px (8 Grad); **18/19 +10 px (38,5 -> 48,5) bei 90 Grad Knick**; 19/20 -3 px
  (61 Grad); 20/21 +0,5 (16 Grad); 21/22 +0,5 (9 Grad).
- Knochenraum (f_schwanz_geo.py, Ringe nur an Anfang/Ende je Glied): Radius Anfang/Ende 16: 315/281, 17: 281/267,
  **18: 344/135 (Sprung 17->18 +77 = +29 %)**, 19: 135/98, 20: 98/58,5, 21: 58,5/82,4 (**Endglied 22 beginnt mit 82,4
  > 58,5, +41 %**), 22: 82,4/0. Glied 16 ragt bis t = -0,63 in den Rumpf; naechste Rumpfecke 396 vom Gelenk 16 entfernt
  (Gelenk 16 im Raum von 8: (-378,-602,-410), also hinten-oben-(W)-rechts).

## 6. Krallen der (W)-linken Hand (Nutzer 9)

- Sichtbar: nur in Foto 1, Bild-rechts (Knochen 12; Klauenhand-Anker 0,97). In Foto 2 ist die (W)-linke Hand abgewandt
  (Modell 31 px, Foto-Zone unbekannt/dunkel).
- Foto (n1485 entblendet): rote Hand x 482-511, y 340-367 = 0,099 x 0,093 H1, 545 px; Krallen nicht aufloesbar.
  n1476 (gleicher Ort, kaum abgeblendet): rote Handflaeche x 478-491, y 340-351; darunter **ein heller Krallenkeil**
  x 481-492, y 350-360 (12 x 11 px = 0,040 x 0,036 H1, 70 px), Lab 30,3 / 4,9 / 1,7, je Zeile von 11 px (y 354) auf
  3 px (y 360) spitz zulaufend; Hand gesamt y 340-360 = 0,07 H1. n1479: nur ein heller Rand x 476-479 (Pfosten).
- Modell (Mesh 12 = Referenz 2 bytegleich, 104 Ecken; y-Histogramm 0-100: 8, 100-200: 16, 200-300: 36, 300-330: 2,
  330-400: 30, 400-500: 12): **4 Krallen** (Flaechen-Komponenten mit y >= 330), Laenge im Knochenraum 115 / 136 / 148 /
  67, Spitzen (-10,426,-48) (82,451,-48) (61,473,-50) (80,386,10). In Pose 1 projiziert 5,4 / 10,9 / 7,9 / 4,4 px =
  0,019 / 0,038 / 0,027 / 0,015 H; Hand Gelenk -> Spitze 38 px = 0,133 H; Krallenpixel 89 px, Box x 482-494 y 359-369,
  **Lab 13,1 / 8,3 / 3,9 gegen Handflaeche 13,6 / 4,0 / 0,5**: die Krallen sind so dunkel wie die Hand.
- Befund: Im Foto ein heller, kompakter Keil (0,040 x 0,036 H, L 30) unter einer roten Handflaeche; im Modell vier duenne,
  dunkle Spitzen (je 0,015-0,038 H lang, 2-4 px breit), die sich nicht von der Hand abheben. Das Modell ist zudem laenger
  (Hand 0,13 H gegen 0,07 H1).

## 7. Tafeln (SF_belege/foto/, roh, Foto 1 entblendet, x3)

| Datei | Inhalt |
|---|---|
| 01_overlay_foto1.jpg / 02_overlay_foto2.jpg | Foto / Modell V5r4 in der neuen Pose / 50 % mit Foto-Umriss (gruen sicher, rot unbekannt), Modellumriss (weiss), Rot-Abweichung (magenta), Nutzerpunkte (gelb), Foto-Auge (cyan), Modell-Kopf (W)-rechts/(W)-links, Modell-Maul (orange), Huckel (rote Striche), Foto 2 Gliedgrenzen + Kette, Foto 1 Krallenspitzen + Foto-Hand n1476 |
| 11/12_rotkarte_foto*.jpg | Rot-Karte Foto gegen Modell (a* >= 10) |
| 21/22_pose_alt_neu_foto*.jpg | Vergleichsansicht: Foto / posen_v4 / neue Pose |
| 30/31_trimap_foto*.png | Umriss-Trimap auf dem Foto (Gitter 10 px) |
| 32_fuesse_n1470_1488.png | Sohlen-Nachweis Foto 1 |
| 33_hand_n1476_n1479_x8.png | Krallen im Video |
| 34/35_kandidaten_*_lauf1.jpg | Kandidaten Lauf 1 (alt, beste frei) |

## 8. Offen

1. Foto 1: Keine Pose bringt den Modellkopf an die Foto-Kopfstelle (Kopf-Anker 0 in allen Laeufen). Ob das Bild nur durch
   Kopfhoehe/-lage (Abschnitt 3.5) oder auch durch den Oberkoerper-Zuschnitt (Schulterbreite) entsteht, trennt diese
   Messung nicht; die Vergleichsansicht 1 ist darum fuer Kopf/Maul nur mit "135" zusammen zu lesen.
2. Foto-1-Sohle 448 +-5 ist aus der Bildfolge hochgerechnet (Fuesse ab n1482 unter der Letterbox).
3. Foto-Schwanzbreite f 0,0-0,4 ist mit dem Buckel-Ansatz vermischt (Maske ohne Kante zum Ruecken); Werte dort obere
   Schranke.
4. "dunkel gruenlich" (Nutzer 3) ist im Lab nicht messbar (a* +4,8); messbar ist "grau, deutlich weniger rot als das
   Modell". Ob der Nutzer den Eindruck relativ zum roten Modell meint, beantwortet kein Bild.
5. Krallenzahl im Foto nicht aufloesbar (ein Keil); n1476 ist nicht entblendet (Beginn der Abblende ~n1475, Wirkung < 3 %).
