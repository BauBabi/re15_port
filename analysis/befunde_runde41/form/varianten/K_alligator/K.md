# Variante K (E20): Alligator-Kopf, Krallen, geschlossene Naehte (laufendes Dossier)

Auftrag: Orchestrator nach E20 (Nutzer 2026-11-02 14:22) und Nachtrag "Loecher zwischen Bein und Oberkoerper". Nur offline (Python, Offline-Buehne),
kein C-Bau, kein Einbau ins Spiel-Asset. Basis J (`varianten/J_dicker`, Textur H2). Kein RE-Wert: neues Modell nach Videostandbildern und Nutzer-Teilen.
Generator `K_alligator/generator/` (k_*.py, Ablauf `k_variante.sh`). Dieses Dossier wird fortlaufend geschrieben; Abschnitt 0 ist der Stand.

## 0. Stand

| Schritt | Stand |
|---|---|
| 1 Teile vermessen | erledigt (Abschnitt 1, 3) |
| 2 Krallen lesen | erledigt (Abschnitt 2) |
| 3 Varianten K1/K2/K3 | gebaut (Abschnitt 4, 5): K1 schlank/lang, K2 breit/kurz, K3 bester Fit |
| 4 Messen | Abschnitt 6: Bild 1 IoU 0,954 (J 0,948), K2/K3 31/31; Bild 2 0,948-0,961 (J 0,935); Unterkiefer IoU bis 0,80 |
| 5 Kontaktblaetter | `K_alligator/*.png` (Abschnitt 9) |
| 6 Folgen Stufe 3 | Abschnitt 8 |
| 7 E20 | in NUTZER_ENTSCHEIDE.md angehaengt |
| Nachtrag Loecher | Hauptnaehte -94 %, alle Naehte ohne Mundwinkel -83 bis -86 %; "in keiner Pose" NICHT erreicht, Rest in Abschnitt 7 |
| Offen beim Nutzer | Wahl K1/K2/K3; heller Unterkiefer im sauberen Render; Kiefer/Mundwinkel in den Stufe-3-Clips |

## 1. Nutzer-Teile Oberkiefer / Unterkiefer (`k_teile.py`)

- Fundstelle in Screenshot 2 (`Version_80%/vlcsnap-2026-10-31-00h29m02s830.png` = `bild2.png`), `cv2.matchTemplate` TM_SQDIFF mit Alpha-Maske:
  Oberkiefer x 216, y 279, 121 x 97 (Rest -0,00006), Unterkiefer x 239, y 318, 105 x 92 (Rest -0,0003). Beide bildpunktgenau wie im Auftrag.
- Leinwand-Masken `nutzer_teile/maske_oberkiefer.png` (5832 px), `maske_unterkiefer.png` (3355 px), Schwelle Alpha >= 200 wie die vorhandenen Masken
  (gemessen: `maske_left_arm_front.png` = Alpha >= 200, 5607 von 5607 px). Kopien der Teile `nutzer_teile/oberkiefer.png`, `unterkiefer.png`.
- Lage in `nutzer_teile/lage_E20.json`, NICHT in `lage.json`: `form_pruefung.py` (re15_port/tools, nicht anzufassen) liest `lage.json` fuer jede Pruefung;
  zwei neue Bild-2-Teile ohne Knochen haetten die Berichte aller Varianten und der parallel laufenden Stufe-3-Abnahme veraendert. Einpflegen = Entscheid Orchestrator.
  Gemessen werden die Kiefer darum mit `k_mass.py` (Abschnitt 6).
- Ueberlappung mit den alten Teilen: Oberkiefer liegt zu 96 % in `face_and_upper_body` (5609 px) und 12 px in `right_arm_front`.
  **Unterkiefer liegt zu 74 % in `right_arm_front` (2477 von 3355 px)**: das schraege, rot gerippte Band, das bisher als rechter Unterarm galt
  (FORMBLATT2 "Unterarm auf dem gerippten Streifen", lesung_fremd "Streifen unten, V-Form (263-304,355-399) offen"), ist nach E20 der Unterkiefer.
  Folge: der rechte Unterarm liegt in Bild 2 hinter dem Kiefer (J legt ihn genau in dieses Band).
- Was J dort hat (Tiefenpuffer, `lage_J.json`): unter dem Oberkiefer Huefte 1259 px, Wirbel 1056, Unterarm r 1095, Oberschenkel r 771, Kiefer 645,
  Unterschenkel r 355, frei 461; unter dem Unterkiefer Unterarm r 1694, Hand r 843, frei 613. Kopf von J (Knochen 22) liegt bei (262-388, 183-319),
  IoU mit dem Oberkiefer 0,003, Kiefer von J 0,000. Der Kopf von J sitzt in der Mitte der Masse, der Nutzer liest ihn links unten davor.
- Bild 1 (Rueckansicht): Von J sind dort 763 px Hals/Kopf/Kiefer sichtbar, zwischen rechtem Bein und rechtem Arm unter der Brust (344-379, 275-326);
  ziel2 fuehrt diese Stelle als Figur (weiss 336, grau 405, schwarz 22 px). Figur blickt +X, Kamera schaut +X: ein langer Kopf zeigt von der Kamera weg,
  sichtbar bleibt nur Hinterkopf/Kieferunterseite unter der Brust. Die Rueckseite verlangt: Kopf haengt tief, bleibt zwischen Bein und Arm, ragt seitlich
  nicht aus dem Umriss. Alle Varianten halten das (Bild 1 IoU und Punkte Abschnitt 6; sichtbar vom Kopf 378-746 px).

## 2. Krallen in Screenshot 2 (`k_krallen.py`, `krallen_lesung.json`)

Spitzen wie `form_pruefung.spitzen` (ragen >= 12 px aus dem Sattel), Laenge ueber die Tiefe der Hand von J und fx 451.

| Hand | Spitze (x, y) | ragt px | Richtung im Bild | Breite px | Laenge Einheiten | Y Spitze / Hand | Video RGB | Rueckrechnung RGB |
|---|---|---|---|---|---|---|---|---|
| rechts (right_arm_front ohne Unterkiefer) | (274,443) | Ende | 39 Grad | 24 | 320 | 62 / 41 | (111,40,45) | (47,25,19) |
| rechts | (249,476) | 21 | 50 Grad | 32 | 220 | 114 / 41 (unter Logo) | (145,104,86) | (61,60,40) |
| rechts | (197,476) | 47 | 102 Grad (nach unten) | 31 | 500 | 70 / 41 (unter Logo) | (132,43,43) | (56,29,18) |
| rechts | (229,351) | 14 | -99 Grad (nach oben) | 10 | 185 | 49 / 41 | (98,27,34) | (45,21,15) |
| rechts | Wurzelende (145,353) | | nach links oben | | | | | |
| links (left_arm_front) | (468,417) | Ende | 50 Grad | 18 | 375 | 94 / 56 | (152,70,70) | (59,36,25) |
| links | (439,418) | 19 | 81 Grad (nach unten) | 12 | 225 | 110 / 56 | (144,93,105) | (56,49,45) |

Lesung:
- Zahl: rechts 4 Spitzen plus das Wurzelende oben links (zwei davon unter der Logo-Schrift, nur im Handschnitt des Nutzers belegt), links 2.
- Laenge 185-500 Einheiten frei aus dem Sattel, Grundbreite 10-32 px = 130-400 Einheiten: kurze, kraeftige, spitze Krallen, die meisten zum Boden
  (81-102 Grad) oder in Handrichtung (39-50 Grad).
- Kruemmung und Querschnitt: im Bild nicht messbar (Spitzen 10-32 px breit, Video-Unschaerfe rund 2 px, zwei Spitzen unter dem Logo).
- Farbe: Krallen sind im Screenshot NICHT grau. Sie haben den Ton der Hand und werden zur Spitze heller: Y-Verhaeltnis Spitze/Hand 1,2 / 1,5 / 1,7 / 2,0
  (ohne die zwei unter dem Logo), Mittel 1,6. Die hellste Spitze (439,418) ist rosa-beige. Die Rueckrechnung zeigt an den Spitzen weniger Rot als an der Hand
  (Spitze (45-59, 21-49, 15-45) gegen Hand (43-47, 19-23, 13-16)).
- Gegen J: Lage passt schon (J rechts 4 von 5 Spitzen innerhalb 12,7 px, links 3 von 3 in 11,9 px, `mass_kiefer_krallen.json`). Die Textur von J ist an den
  Krallen grau und unruhig: Mittel (48-118) ohne Farbstich, rechte Krallen dunkler als die Hand (Y 50-77 gegen 74), linke viel heller (Y 95-118).
- Umsetzung in K (`k_bau.krallen`): Form wie J (Haken 0,18 der Krallenlaenge zum Boden probiert: Spitzen 2 px weiter von den Handschnitten, 14,6 gegen 12,7 px;
  die Kruemmung ist nicht belegt, darum weg). Neu ist der Ton: Kralle ab 55 % der Fingerlaenge (rf_plan.finger: letzter voller Fingerring bei 0,52 L)
  auf eigener Textur, Wurzel = Mittel der Hand-Textur von J, Spitze 1,6-fach heller, ohne eigenen Farbton. Das Kanalverhaeltnis der Rueckrechnung
  (1,2 / 2,1 / 2,8) auf die graue Hand von J gelegt ergaebe Blau, das der Screenshot nicht zeigt (E14).

## 3. Kopf: was die Teile zeigen (`k_teile.py`, `k_kopf_fit.py`)

Im Bild (Screenshot 2):
- Oberkiefer: Schwerpunkt (262,7 / 324,9), Hauptachse 11,6 Grad, 107 x 79 px (Streckung 1,36); Oberkante von (216,280) nach (330,316), 17,5 Grad.
  Links breit (96 px hoch bei x 216-250), nach rechts schmal: die breite, flache Schnauze vorn, hinten zum Hals.
- Unterkiefer: Schwerpunkt (293,6 / 364,0), Achse 141,5 Grad (von oben rechts (342,320) nach unten links (262,405)), 109 x 37 px, Streckung 3,3.
  6 helle Flecken entlang des Bandes (Prominenz >= 15 Y), Abstand im Median 16 px = rund 190 Einheiten: Zahnreihe.
- Maulwinkel im Bild: Oberkante (nach links oben, 197,5 Grad) gegen Unterkiefer-Achse (141,5 Grad) = 56 Grad, gegen die Oberkiefer-Achse 50 Grad.
- Lesung in 3D: Der Kopf ist zur Frau links im Bild gedreht (Figur-rechts), das Maul weit offen; die Kamera sieht von oben vorn rechts auf die Oberseite
  der Schnauze (dunkel) und in den offenen Unterkiefer (Maulboden rot, Zaehne hell). Die Rueckrechnung bestaetigt: Oberkiefer (28,9,8) wie Rumpf (27,12,12),
  Y 24 gegen 25 (gleiche Haut, Abweichung unter dem Filterrauschen, farbe.md MAE 15-21); Unterkiefer (70,41,35), Y 94 gegen 25 (Fleisch, hell).

Zurueckgerechnet mit der Kamera von Bild 2 (Pose der Variante, Tiefenpuffer gegen die Masken; Werte Abschnitt 5):
- Lage zum Rumpf: Kopfursprung am Halsende (Knochen 22 bei Hals + 503). Gegen die Brust (Knochenraum 10) zeigt der Hals in Bild 2 rund 92-111 Grad nach rechts
  und 57-65 Grad nach unten (J: 65 Grad rechts, 29 Grad nach oben), der Kopf 91-97 Grad nach rechts und 3-8 Grad nach unten (J: 45 rechts, 12 oben).
  Der Hals senkt den Kopf vor die Brust; die Schnauze liegt vor Huefte und rechtem Oberschenkel (Tiefe dort J 5881, Kopf davor).
- Schnauzenlaenge: K3 (bester Fit) Kopf 1629 Einheiten ab Halsende, Kiefer 1089 ab Gelenk; K1 2400 / 1860; K2 1700 / 1160. J: Kopf 1300 (FORMBLATT2).
- Oeffnung des Mauls in 3D: K3 25 Grad, K2 28 Grad, K1 21 Grad (Winkel Kopfachse gegen Kieferachse). Das Bild zeigt 50-56 Grad, weil der Kiefer fast
  zur Kamera zeigt (Verkuerzung).
- Bild 1: dieselbe Kopfform, eigene Kopfwinkel (Clip 0), so gelegt, dass der Umriss nicht schlechter wird: Kopf 47-50 Grad nach unten unter die Brust,
  Kiefer zu (K2, K3) bzw. 53 Grad offen (K1, unsichtbar).

## 4. Bau der Varianten (`k_form.py`, `k_kopf_fit.py`, `k_bau.py`)

- Basis J: FORM.EMD/TIM von `J_dicker`, Knochen und Versaetze unveraendert (32 Knochen, keiner neu). Ersetzt werden nur die Meshes 22 (Kopf) und 23
  (Kiefer); an den Fingern 14,15,28,30,19,20,29,31 und der fuenften Kralle (in 13/18) nur UV und CLUT der Krallenspitzen. Dazu Gelenkkoerper (Abschnitt 7).
- Kopf (Knochen 22): Rohr aus 8 Ringen zu 10 Ecken, Querschnitt flacher, abgerundeter Kasten (Krokodilschaedel): Schaedel ab x -220 (steckt im Hals),
  Schnauze ab x 700 bis L, Gaumen flach bei y 270, zwei Augenhoecker oben bei x 520, Zaehne (7 je Seite, dreiseitige Pyramiden 120 lang) an der Unterkante.
- Kiefer (Knochen 23, Gelenk bei Kopf (500, 400) wie J): U-Trog (Rand, Maulboden, Kinnboden), Rand liegt geschlossen am Gaumen, 7 Zaehne je Seite.
- Pose: Hals 21, Kopf 22, Kiefer 23 in Clip 1 (Bild 2) per Powell-Suche (3 Starts) auf IoU Oberkiefer + IoU Unterkiefer, Strafe wenn der Umriss unter J
  (0,935) faellt; Kiefer nur Oeffnen (Seitendrehung hoechstens 3,5 Grad), Schaedel mindestens so breit wie der Schnauzenansatz. Bild 1 (Clip 0): Umriss,
  Kiefer 0-53 Grad, leichte Bindung an die Kopflage von J. Ruhe (Clip 2): Hals wie J, Kopf 13 Grad nach unten, Kiefer 5 Grad offen (nur fuer das Rundumblatt).
- Varianten (Kopfmasse in Einheiten, halbe Breiten):

| | L (Schnauzenspitze) | Schaedel Wk | Schnauzenansatz Ws | Schnauzenspitze Wt | Hoehe Schnauze yt_s | Wahl |
|---|---|---|---|---|---|---|
| K1 schlank, lang | 2400 | 420 | 320 | 240 | -250 | fest |
| K2 breit, kurz | 1700 | 460 | 430 | 390 | -280 | fest |
| K3 bester Fit | 1629 | 330 | 327 | 375 | -188 | frei (L +-40 %, sonst +-25 %) |

  K3 laeuft auf eine kurze, flache Schnauze, die vorn breiter ist als am Ansatz (U-foermige Alligatorschnauze); der Schaedel liegt an der Untergrenze
  (Wk = Ws, die Suche will ihn schmaler: breiter Schaedel liegt in Bild 2 ueber der Oberkante des Oberkiefers).
- Textur: Seiten 0-2 (TIM-x 0-383) und CLUT-Zeilen 0-2 bytegleich wie J; neue Seite 3 (TIM-x 384-511, TIM 512 x 256) mit eigener CLUT-Zeile 3 (255 Farben,
  k-Mittel) fuer Kopf, Kiefer, Zaehne, Augenhoecker, Krallen. Stoff = H2-Haut der Brust von J (gespiegelt gekachelt, nur Struktur), umgefaerbt:
  - Kopf: wie Rumpf (Faktor 1), Gaumen wie Maulboden.
  - Kiefer innen und aussen: Fleisch aus der Lichtmessung (Rueckrechnung `farbe_bild2_echt.png` geteilt durch das Port-Licht je Pixel, mit dem Faktor des
    Rumpfs von J in Texturwerte gebracht), dann Helligkeit auf das Video geeicht (Y im sichtbaren Unterkiefer durch video_filter gegen Video, 2 Runden).
    Ergebnis ein heller, warmer Ton (kein gesaettigtes Rot): unter dem Licht und Filter von Bild 2 wird er so rot wie das Band im Video.
  - Zaehne/Flecken: Ton des Fleisches auf Y 230 gehoben, halb mit Neutralweiss; die Rueckrechnung ergibt dort Y > 255 und einen Gruenstich aus dem
    Kanalfaktor des Rumpfs, den das Video nicht zeigt (E14).
  - Krallen: Abschnitt 2.

## 5. Kopf-Fits (`_lauf/K*/kopf.json`)

| | IoU Oberkiefer | IoU Ober. ohne Rumpf | IoU Unterkiefer | Umriss Bild 2 | Umriss Bild 1 | Kiefer offen | Hals / Kopf gegen Brust (rechts, unten) |
|---|---|---|---|---|---|---|---|
| K1 | 0,390 | 0,442 | 0,716 | 0,956 | 0,947 | 21 Grad | 111/63, 93/8 |
| K2 | 0,504 | 0,717 | 0,755 | 0,944 | 0,947 | 28 Grad | 99/65, 91/3 |
| K3 | 0,574 | 0,685 | 0,811 | 0,943 | 0,947 | 25 Grad | 92/57, 97/7 |

Tiefenpuffer-Raster aus `k_kern` (Umriss J zum Vergleich: Bild 2 0,934, Bild 1 0,9465; form_pruefung: 0,935 / 0,948). "ohne Rumpf": der uebrige
Oberkoerper (face_and_upper_body ohne die Kiefer-Teile) zaehlt auf keiner Seite (dunkel auf dunkel).

## 6. Messwerte der Varianten (Endstand, mit Gelenkkoerpern)

**Form gegen die Screenshots** (`form_pruefung.py pruefe`, je Variante `K*/pruefung/bericht.txt`):

| | Flaechen | Bild 1 IoU | Bild 1 Punkte | Bild 1 Boden/Kerbe | Bild 2 IoU | Bild 2 Punkte | Bild 2 Boden/Verd. | Zeichnung 1 / 2 | Verbote 2 |
|---|---|---|---|---|---|---|---|---|---|
| J | 2398 | 0,948 | 31/31 | 5/7 | 0,935 | 7/7 | 9/9 | 5/9 / 4/7 (Bild 2: Q1, Q2, Q5) | 1/1 |
| K1 | 3642 | 0,954 | 30/31 (b1.nr_y322_rechts) | 5/7 | 0,961 | 7/7 | 9/9 | 5/9 / 2/7 (Bild 2: Q1, Q3, Q4, Q5, P) | 0/1 |
| K2 | 3642 | 0,954 | 31/31 | 5/7 | 0,948 | 7/7 | 9/9 | 5/9 / 3/7 (Bild 2: Q1, Q4, Q5, P) | 0/1 |
| K3 | 3642 | 0,954 | 31/31 | 5/7 | 0,948 | 7/7 | 9/9 | 5/9 / 3/7 (Bild 2: Q1, Q4, Q5, P) | 0/1 |

- Bild 1 wird nicht schlechter als J: IoU 0,954 gegen 0,948, K2 und K3 31 von 31 Punkten; K1 30/31 (b1.nr_y322_rechts, rechte Beinkante: der lange Kopf
  liegt in Bild 1 hinter dem Bein und schiebt die Kante). Boden L1/L2 wie bei J offen (5/7). Bild 2: IoU 0,948 (K1 0,961) gegen 0,935, 7/7, 9/9.
- Zeichnung Bild 2: Q3, Q4 und P fallen neu (Q2 besteht neu). Q (Zug Q) und P (Wangenspitze) waren Merkmale am Kopfrand von J (FORMBLATT2: "Rand auf
  Zug Q", "Wangenspitze, Stelle P"); nach E20 liegt dort kein Kopf mehr. Verbot N1 (Rumpfinneres 290-340/225-280 ohne helle Zeichnung) faellt: wo der
  Kopf von J sass, sieht man jetzt Oberarm r und Hals mit ihrer H2-Haut (heller Anteil 0,21). Beides folgt aus der neuen Kopflage, nicht aus der Textur.
- Flaechen 3642 (J 2398): Kopf und Kiefer haben zusammen rund so viele Flaechen wie bei J, dazu 31 Gelenkkoerper zu je 40 Flaechen.

**Ober-/Unterkiefer und Krallen** (`k_mass.py`, `mass_kiefer_krallen.json`; Tiefenpuffer, Pose Bild 2):

| | IoU Oberkiefer | ohne Rumpf | Schwerpunkt px / Achse Grad | IoU Unterkiefer | Schwerpunkt px / Achse Grad | Arm r ohne Unterkiefer | Krallen rechts (Paare/Soll, mittl. px) | Krallen links |
|---|---|---|---|---|---|---|---|---|
| J | 0,003 | 0,008 | 107,2 / 25,2 | 0,000 | 77,2 / 20,8 | 0,526 | 4/5, 12,7 | 3/3, 11,9 |
| K1 | 0,378 | 0,425 | 12,6 / 37,5 | 0,725 | 4,8 / 3,3 | 0,596 | 3/5, 5,3 | 2/3, 1,5 |
| K2 | 0,500 | 0,671 | 35,1 / 32,0 | 0,746 | 4,2 / 3,1 | 0,743 | 4/5, 12,3 | 2/3, 1,5 |
| K3 | 0,508 | 0,625 | 31,5 / 31,8 | 0,801 | 0,7 / 2,3 | 0,736 | 4/5, 12,8 | 2/3, 1,5 |

- Unterkiefer: K3 trifft das Band fast deckungsgleich (IoU 0,80, Schwerpunkt 0,7 px, Achse 2,3 Grad); K1 und K2 0,73-0,75.
- Oberkiefer: IoU 0,38-0,51, ohne den uebrigen Oberkoerper (dunkel auf dunkel) 0,43-0,67. Die Achse weicht 32-38 Grad ab: der sichtbare Kopf schliesst
  hinten den Schaedel und die Muffe Kopf-Hals ein (rechts oben, ausserhalb der Maske), die Maske ist nur die Schnauze. Ohne Gelenkkoerper lag K3 bei 0,57.
- Rechter Arm ohne den Unterkiefer-Anteil: J 0,53, K 0,60-0,74 (der Arm liegt jetzt, wie E20 verlangt, hinter dem Kiefer).
- Krallen: Lage wie J (Form unveraendert, rechts 4 von 5 Spitzen in 12,3-12,8 px). Links paart die Messung 2 von 3 Spitzen in 1,5 px; die dritte ist
  das obere Wurzelende des Handschnitts, das sich mit den Gelenkkoerpern an Ellbogen und Hand verschiebt, keine Krallenspitze.
- Farben (bau.json): Maulboden und Kieferunterseite K3 = Brust-Textur x (2,86 / 2,69 / 2,14), also rund (163,140,122); Zaehne rund (243,226,211). Unter dem
  Licht von Cut 11 und dem Videofilter wird daraus das rote Band (Kiefer-Eichung Soll/Ist 1,075 -> 1,005). Im sauberen Render ist der Unterkiefer
  dadurch hell beige (Rundumblatt): so verlangt es die Rueckrechnung, ein gesaettigtes Rot im Modell zeigte unter dem Filter zu rot.

## 7. Nachtrag Nutzer: Loecher zwischen Bein und Oberkoerper (`k_loecher.py`, `k_naht.py`)

Wortlaut Nachtrag: "Und unser 4 Beiner Birkin Modell hat noch ganz klare Loecher zwischen Bein und Oberkoerper. Das ist nicht durchgaengig geschlossen...."

**Messung** (`k_loecher.py`): je Clip bis zu 8 gleich verteilte Bilder, je Bild 10 Ansichten (vorn, hinten, links, rechts, oben, unten, 3/4 vorn rechts
oben, 3/4 hinten links oben, 3/4 vorn links unten, 3/4 hinten rechts unten), orthografisch, fester Massstab 0,075 px je Einheit (wie das Spiel in Bild 2),
Zeichnen wie der Port (Rueckseiten nach re15_nclip_r41_weg verworfen, dann Tiefenpuffer). Loch = Hintergrund-Flaeche (4er-Nachbarschaft, ab 3 px), die
den Bildrand nicht erreicht. Nahtloch = Loch, dessen Rand zu mindestens 50 % von den Knochen eines Gelenks stammt und dessen fernstes Pixel hoechstens
1,5 Gelenkradien vom Drehpunkt liegt (sonst ist es eine Luecke zwischen Gliedern, z. B. Arm vor Bein, die zur Form gehoert). Offene Kanten je Mesh: 0
(alle Meshes von J sind geschlossen); die Loecher sind Spalte ZWISCHEN den starren Meshes, wenn ein Gelenk beugt.

**Vorher** (J = Stufe-3-Asset, 28 Clips, 1120 Posen-Ansichten): 1245 eingeschlossene Flaechen, davon 312 Nahtloecher mit 18 289 px. Nach Naht:
Brust-Wirbel 7617 (Flucht, Clip 11: Flanken reissen bei der Rumpfbeuge auf), Bein l - Huefte 3265, Hals - Brust 1508, Bein r - Huefte 1413, Hand r 1010,
Kopf - Hals 541, Knie r 503, Kiefer - Kopf 390, Hand l 383, Fuss l 293, Schwanz 3-4 268, Arm l - Brust 261, Ellbogen l 237, Ellbogen r 170,
Arm r - Brust 156, Knie l 139, Fuss r 78, Wirbel - Huefte 42, Schwanz - Becken 15. Bein-Huefte-Loecher in fast jedem Clip (Stehen, Gehen, Hiebe,
Drohen, Biss, Griff). Datei `loecher/vorher_J_stufe3.json`.

**Weg** (das Format kennt keine Gewichte, Mesh i haengt starr an Knochen i; neue Knochen gehen nicht):
- Gelenkkugel am Eltern (`k_naht.GELENKE`, 23 Gelenke): Ellipsoid um den Drehpunkt im Raum des Elternknochens. Eine Kugel um den Drehpunkt dreht sich in sich,
  sie fuellt den Raum ums Gelenk in jeder Pose gleich. Quer fq, nach aussen fn mal Ansatzradius des Kindes (90. Perzentil des Achsabstands der Kind-Ecken
  nahe dem Drehpunkt).
- Muffe am Kind (`k_naht.MUFFEN`, nach der ersten Messung dazu): Ellipsoid am Kind, Mitte im Drehpunkt, lang entlang der Kindachse (fa bis 1,6 Ansatzradien).
  Das Stueck hinter dem Drehpunkt steckt im Eltern und schwenkt beim Beugen in den Spalt auf der Gegenseite.
- Textur der Koerper: UV, CLUT und Seite der naechsten Flaeche des Mesh, an dem sie haengen (Haut der Umgebung, Textur unveraendert).
- Abstimmung je Koerper gegen die Screenshots, groesstes Volumen zuerst: (a) Naeherung im Tiefenpuffer: hoechstens 10 neue Figurpixel auf Ziel-Schwarz und 150
  ausserhalb Ziel-Weiss je Bild; (b) am echten Pruefweg (Buehne + Punkte + Boden/Verdeckung/Kerbe E3 von form_pruefung): kein Punkt, der ohne den Koerper
  besteht, darf mit ihm fallen, Umriss-IoU je Bild hoechstens -0,001. Mundwinkel (Kiefer - Kopf) zusaetzlich so klein, dass IoU Ober- und Unterkiefer je
  hoechstens 0,01 verlieren (`k_maulwinkel.py`); mit voller Groesse deckte er in Bild 2 den Unterkiefer zu (K3: 0,804 -> 0,576).
- Erste Runde (nur Kugeln, ohne Mundwinkel-Grenze): Nahtloecher ohne Kiefer 17 899 -> 3 397-3 695 px (K1-K3). Rest: Bein r - Huefte (rund 840 px, vor allem
  Biss und Hieb links), Bein l - Huefte, Kopf - Hals, Haende, Arm l - Brust; Bild 2 begrenzt die Kugel an Arm l (0,5/0,6) und Bein r (0,9/0,8).

**Endstand** (alle 28 Clips, 1120 Posen-Ansichten je Variante, `K*/loecher_stufe3.json`; Mundwinkel = Kiefer - Kopf getrennt ausgewiesen):

Nahtloecher je Clip (Zahl / px; in Klammern davon Mundwinkel Kiefer - Kopf, px):

| Clip | J vorher | K1 | K2 | K3 |
|---|---|---|---|---|
| 0 rennen | 26 / 1020 (102) | 16 / 448 (0) | 11 / 173 (0) | 15 / 201 (0) |
| 1 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 2 stehen | 24 / 696 (0) | 36 / 1092 (1008) | 61 / 736 (650) | 49 / 529 (423) |
| 3 hieb_rechts | 25 / 764 (0) | 42 / 1400 (874) | 51 / 810 (497) | 45 / 881 (347) |
| 4 hieb_links | 20 / 1254 (0) | 40 / 1382 (809) | 53 / 1337 (758) | 39 / 1268 (516) |
| 5 gehen | 29 / 1217 (0) | 43 / 1680 (1572) | 59 / 1275 (1165) | 41 / 812 (698) |
| 6 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 7 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 8 zucken | 13 / 463 (21) | 27 / 862 (794) | 43 / 780 (733) | 36 / 560 (505) |
| 9 nachhieb | 36 / 719 (65) | 24 / 594 (469) | 36 / 996 (681) | 31 / 545 (230) |
| 10 drohen | 19 / 714 (0) | 29 / 1013 (771) | 33 / 817 (559) | 32 / 664 (394) |
| 11 flucht | 35 / 7681 (18) | 22 / 576 (324) | 26 / 641 (568) | 22 / 457 (383) |
| 12 sturz | 18 / 778 (87) | 10 / 274 (152) | 10 / 195 (74) | 17 / 271 (123) |
| 13 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 14 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 15 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 16 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 17 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 18 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 19 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 20 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 21 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 22 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 23 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 24 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 25 frei (Ruhe) | 2 / 91 (0) | 4 / 104 (104) | 6 / 90 (90) | 4 / 56 (56) |
| 26 biss | 16 / 384 (80) | 33 / 802 (606) | 46 / 970 (780) | 36 / 701 (515) |
| 27 griff | 19 / 1143 (17) | 21 / 590 (405) | 30 / 624 (381) | 25 / 615 (278) |
| Summe | 312 / 18289 (390) | 407 / 12377 (9448) | 555 / 10794 (8286) | 452 / 8400 (5308) |

Nahtloecher je Gelenk ueber alle 28 Clips, px:

| Gelenk | J vorher | K1 | K2 | K3 |
|---|---|---|---|---|
| Brust - Wirbel (Hauptnaht) | 7617 | 29 | 133 | 131 |
| Bein l - Huefte (Hauptnaht) | 3265 | 231 | 372 | 377 |
| Hals - Brust (Hauptnaht) | 1508 | 0 | 0 | 0 |
| Bein r - Huefte (Hauptnaht) | 1413 | 263 | 275 | 289 |
| Hand r | 1010 | 366 | 232 | 397 |
| Kopf - Hals (Hauptnaht) | 541 | 212 | 26 | 77 |
| Knie r | 503 | 240 | 222 | 225 |
| Kiefer - Kopf | 390 | 9448 | 8286 | 5308 |
| Hand l | 383 | 816 | 539 | 614 |
| Fuss l | 293 | 301 | 295 | 308 |
| Schwanz 3-4 | 268 | 58 | 32 | 46 |
| Arm l - Brust (Hauptnaht) | 261 | 46 | 64 | 47 |
| Ellbogen l | 237 | 0 | 0 | 12 |
| Ellbogen r | 170 | 18 | 47 | 70 |
| Arm r - Brust (Hauptnaht) | 156 | 0 | 15 | 21 |
| Knie l | 139 | 185 | 181 | 175 |
| Fuss r | 78 | 8 | 8 | 11 |
| Wirbel - Huefte | 42 | 97 | 12 | 116 |
| Schwanz - Becken (Hauptnaht) | 15 | 43 | 43 | 41 |
| Schwanz 2-3 | 0 | 9 | 5 | 5 |
| Schwanz 4-5 | 0 | 7 | 7 | 130 |
| Summe ohne Mundwinkel | 17899 | 2929 | 2508 | 3092 |
| Summe Hauptnaehte | 14776 | 824 | 928 | 983 |

Gewaehlte Koerper K3 (`K3_fit/naht.json`, 31 Stueck): Kugeln an allen 23 Gelenken, u. a. Bein l 1,0/0,6, Bein r 0,9/0,8, Arm l 0,6/0,4 (Bild 2 laesst nicht
mehr zu), Arm r 0,9/1,0, Hals 1,0/1,0, Kopf 1,0/0,8, Mundwinkel 0,8/1,0; Muffen Bein l 0,6/1,6, Bein r 0,7/1,6, Arm r 0,8/1,3, Hals 1,0/1,3, Kopf 1,0/1,6,
Schwanz 0,9/1,3, Haende 1,0/1,6 (Muffe Arm l verworfen: Bild 2 +2549 px auf Schwarz).

Lesart:
- Die Hauptnaehte (Bein, Arm, Hals, Schwanz, Kopf, Rumpfbeuge) fallen von 14 776 auf 824-983 px (-94 %), alle Naehte ohne Mundwinkel von 17 899 auf
  2 508-3 092 px (-83 bis -86 %). Hals-Brust ist zu (0 px), Brust-Wirbel in der Flucht von 7 617 auf 29-133 px, Bein l - Huefte von 3 265 auf 231-377 px.
- NICHT erreicht: "in keiner Pose". Es bleiben kleine Spalte, vor allem Bein r - Huefte (Hieb links Bild 35, Biss), Bein l - Huefte (Drohen Bild 36),
  Knie r (Rennen), Hand l (Sturz Bild 0, Hieb links). Blatt `loecher_rest_K3.png`. Grund: an Huefte und linkem Arm begrenzen die Screenshot-Posen die
  Koerper (Bild 1 bzw. Bild 2 bekaemen Pixel auf Ziel-Schwarz oder Punkte fielen). Naechster Schritt dafuer: die Koerper nur dort groesser, wo die Pose
  der Screenshots sie nicht sieht (z. B. Muffe Bein l nach vorn statt rund), oder die Beinwinkel der Stufe-3-Clips an diesen Bildern um wenige Grad aendern.
- Mundwinkel (Kiefer - Kopf): 390 px bei J, 5 308-9 448 px bei K. Das ist der Spalt zwischen Ober- und Unterkiefer, weil die Stufe-3-Clips den Kiefer auch
  im Stehen und Gehen 4-11 Grad oeffnen (Kieferwinkel des alten Kopfes 46-126). Probe K3 mit geschlossenem Kiefer (az 0) in Stehen und Gehen: Mundwinkel
  423 -> 101 px und 698 -> 66 px (`_lauf/K3_kiefer_zu.json`). Bei weit offenem Maul (Biss, Drohen, Griff) sieht man durchs Maul, gewollt nach E20; den
  Mundwinkel selbst schliesst ein Koerper nur bis 0,8/1,0, sonst verdeckt er in Bild 2 den Unterkiefer (voll: IoU 0,804 -> 0,576). Fuer Stufe 3: Kiefer
  zu, wenn nicht gebissen oder gedroht wird (Abschnitt 8).
- Kontaktblaetter: `loecher_vorher_nachher_K3.png` (die 8 schlimmsten Posen von J, daneben K3 in derselben Pose und Ansicht, je ganz und nah) und
  `loecher_rest_K3.png` (die 6 schlimmsten verbliebenen Posen von K3). Pixelzahlen dort im Massstab des Blatts (rund 0,04-0,05 px je Einheit), die
  Tabellen oben im Messmassstab 0,075.

## 8. Folgen fuer Stufe 3 (notiert, nicht umgesetzt)

Gemessen an den 28 Clips der Stufe 3 mit den Meshes von K (`k_s3.py`, `k_s3_kopf.py`, `stufe3_kopf.json`); die Clips selbst sind unveraendert.

| Clip | Kiefer (Knochen 23, az) | Kopf/Hals | Bedeutung fuer K |
|---|---|---|---|
| 26 Biss | bis 524 (46 Grad) | Kopf seitlich bis +-88 | Hauptclip fuer das Maul; Treffer ueber EDD-Bit 16 |
| 10 Drohen | bis 547 (48 Grad) | Bild 45-52 = Pose Bild 2 mit den Kopfwinkeln von J | Gebruell (Toene im EDD); einen eigenen Gebruell-Clip gibt es nicht |
| 27 Griff | bis 547 | +-63 | Maul offen beim Griff, Opfer-Spur |
| 9 Nachhieb | bis 478 | | |
| 12 Sturz | bis 635 (56 Grad) | Bild 0 = Pose Bild 2 | Rueckstoss-Szene im Zug |
| 8 Zucken | bis 387 | | |
| 3/4 Hiebe | bis 342 | | |
| 2 Stehen, 5 Gehen | 46-126 (4-11 Grad) | | oeffnet den langen Kiefer: Spalt sichtbar (Abschnitt 7) |

Was ein langer Kopf verlangt:
1. Pose Bild 2 (Drohen 45-52, Sturz 0) mit den Kopfwinkeln der Variante (K3: Hals 21 = [-471, 935, 1116], Kopf 22 = [128, 54, -561], Kiefer 23 = [0, 40, 279];
   gegen die Brust Hals 92 Grad rechts / 57 unten, Kopf 97 rechts / 7 unten), sonst sitzt der Kopf dort anders als im Screenshot. Rennen Bild 11
   (= Pose Bild 1) entsprechend mit den Bild-1-Winkeln.
2. Kiefer in Stehen/Gehen/Ruhe auf az 0 (zu); offen nur in Biss, Drohen, Griff, Sturz, Nachhieb. Kieferwinkel der Angriffe neu abstimmen: ein 46-Grad-Maul
   ist bei K3 an der Spitze rund 790 Einheiten offen (Kiefer 1089 lang), bei J rund 520.
3. Biss-Trefferpunkt: der Tick prueft Teil 10 (Brust) + (1400, 0, 0), Radius 1500 (birkin_bahnsteig.c bb_biss, `bb_nah(e, pl, BB_TEIL_RUMPF, BB_L(1400), 0,
   BB_L(1500))`). Die Kieferspitze liegt in den Stufe-3-Clips 1632-2198 (K3), 1703-2266 (K2), 2392-2947 (K1) Einheiten davon (J 1413-2207); bei K1 liegt sie
   ausserhalb der 1500er-Kugel, mit den Bild-2-Kopfwinkeln (Kopf vorgestreckt) bei allen weiter. Vorschlag: Bisspunkt an Teil 23 (Kiefer) oder Versatz nach vorn.
4. Laenge: Schnauze in den Stufe-3-Clips vorn bis x 4380 (K3), 4450 (K2), 5150-5780 (K1) gegen 4330-4820 bei J; die Gesamtlaenge bleibt vom Schwanz
   bestimmt (7924-9447). Mit vorgestrecktem Kopf waechst sie vorn um rund L - 1300 (K3 330, K1 1100). Kasten hit_offset_x 800 und Schatten 2300 x 1300
   (main.c, werkzeug.md Abschnitt 3) decken die Schnauze dann nicht: nachziehen, vor allem fuer K1.
5. Kopf am Boden: in Rennen, Hieben, Nachhieb und Flucht ist der Kopf (wie bei J) der tiefste Punkt der Figur (Abstand 0 zur Fussebene). Mit der laengeren
   Schnauze schleift sie dort; Hals/Kopf in diesen Clips einige Grad hoeher nehmen (bei K1 noetig, bei K3 Grenzfall).
6. Trefferzone: Schuesse auf die Form kennen keine Kopfzone (Teile 10/13/18). Ob Kasten und Trefferpruefung den Kopf erfassen, im Spiel pruefen.
7. Kamera-Nahsicht: Sturz/Rueckstoss im Zug (5090) und der Auftritt am Bahnsteig. Ein vorgestreckter Kopf ragt um die Schnauzenlaenge weiter zur
   Kamera; Spielblatt (form_spiel.sh) mit den neuen Winkeln pruefen, bevor die Laenge festgelegt wird.
8. Griff: der Kopf ist beim Griff (Bild 8) nahe am Opfer; mit 1089-1860 langen Kiefern Durchdringung mit dem Opfer pruefen (nicht gemessen).
9. Gelenkkoerper: +1244 Flaechen (3642 gesamt, RE2-Bosse 963-1427). Fuer den PC ohne Belang, fuer ein PSX-Budget zu viel; dann Koerper mit 6 statt 8
   Ecken (24 statt 40 Flaechen) und nur an den Hauptnaehten.
10. Textur: neue Seite 3 und CLUT-Zeile 3 (TIM 512 x 256). form_emd.py und der Port lesen das (TIM-Breite bis 15*128+256, werkzeug.md); Tests und
    Paket-Gate beim Tausch des Assets auf die TIM-Groesse pruefen.

## 9. Dateien und Ablauf

Kontaktblaetter fuer den Nutzer (alle in `varianten/K_alligator/`):
- `bild2_vergleich.png`: je Zeile J, K1, K2, K3: Screenshot 2 | Overlay (Modellumriss gruen, Kopf gelb, Kiefer magenta, Nutzer-Teile orange/violett) |
  Render sauber | Render durch Videofilter, Kamera von Bild 2.
- `bild1_vergleich.png`: dasselbe fuer Bild 1 (Rueckansicht).
- `nah_maul_krallen.png`: Maul, Hand rechts, Hand links, je Screenshot | gefiltert | sauber, 1,5-fach.
- `rundum.png`: Ruhepose aus 6 Winkeln, ganze Figur und Kopf 2,6-fach, je J, K1, K2, K3.
- `loecher_vorher_nachher_K3.png`, `loecher_rest_K3.png`: Nachtrag Loecher.

Varianten: `K1_lang/`, `K2_breit/`, `K3_fit/` je FORM.EMD, FORM.TIM (512 x 256, 4 CLUT-Zeilen), form.json (fuer form_pruefung), bau.json (Masse, Farben,
Krallen, Koerper), kopf.json (Kopfform, Winkel, Fit-Masse), naht.json (Koerper, Proben, Nachpruefung, Mundwinkel), seite3.png (neue Texturseite),
loecher_stufe3.json, pruefung/bericht.txt + bericht.json (Pruefbilder lokal, nachbaubar). Messdaten: `mass_kiefer_krallen.json`, `stufe3_kopf.json`,
`krallen_lesung.json`, `lage_J.json`, `loecher/vorher_J_stufe3.json`, `tabellen.md`.

Generator `generator/`: k_kern (Laden, Tiefenpuffer), k_teile (Masken), k_lage, k_krallen, k_form (Kopf/Kiefer-Geometrie), k_kopf_fit + k_bild1 (Posen),
k_bau (Bau, Textur, Lichtmessung, Kiefer-Eichung, Krallen), k_naht (Gelenkkoerper, Abstimmung, Nachpruefung), k_maulwinkel, k_loecher (Loch-Messung),
k_s3 (Variante mit Stufe-3-Clips), k_s3_kopf, k_mass, k_blatt (Kontaktblaetter), k_bericht (Tabellen), k_lochblick, k_sicht (Hilfsbilder).

Ablauf (aus `generator/`):
- `python k_teile.py`, `python k_lage.py`, `python k_krallen.py`
- `python k_kopf_fit.py --name K1 --form L=2400,Ws=320,Wt=240,Wk=420 --aus ../_lauf/K1/kopf.json --iters 700 --starts 3`
- `python k_kopf_fit.py --name K2 --form L=1700,Ws=430,Wt=390,Wk=460,yt_s=-280,yt_t=-190 --aus ../_lauf/K2/kopf.json --iters 700 --starts 3`
- `python k_kopf_fit.py --name K3 --frei L,Ws,Wt,Wk,yt_s,yt_t --aus ../_lauf/K3/kopf.json --iters 1000 --starts 3`
- `python k_bild1.py ../_lauf/K1/kopf.json` (ebenso K2, K3)
- `bash k_variante.sh K1 ../K1_lang`, `bash k_variante.sh K2 ../K2_breit`, `bash k_variante.sh K3 ../K3_fit`
- `python k_loecher.py messen ../../../stufe3/asset/BAHNSTEIG_FORM.EMD --aus ../loecher/vorher_J_stufe3.json`
- `python k_blatt.py bild|nah|rundum|loecher ...`, `python k_mass.py ...`, `python k_s3_kopf.py ...`, `python k_bericht.py`

Parallel laufende Varianten brauchen getrennte Arbeitsordner (k_maulwinkel: `_lauf/<K>_naht_maulwinkel`); ein gemeinsamer Ordner hat einen ersten Lauf
verdorben (K2 abgebrochen, K1/K3 wiederholt, Endstand aus dem Wiederholungslauf).

Nicht angefasst: re15_port/ (nur gelesen), Spiel-Asset, stufe3/ (nur gelesen), STUFE3_abnahme_1.md, nutzer_teile/lage.json, pri/, synchro/, Version_80%/.
