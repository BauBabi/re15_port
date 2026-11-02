# Variante K (E20): Alligator-Kopf, Krallen, geschlossene Naehte (laufendes Dossier)

Auftrag: Orchestrator nach E20 (Nutzer 2026-11-02 14:22) und Nachtrag "Loecher zwischen Bein und Oberkoerper". Nur offline (Python, Offline-Buehne),
kein C-Bau, kein Einbau ins Spiel-Asset. Basis J (`varianten/J_dicker`, Textur H2). Kein RE-Wert: neues Modell nach Videostandbildern und Nutzer-Teilen.
Generator `K_alligator/generator/` (k_*.py, Ablauf `k_variante.sh`). Dieses Dossier wird fortlaufend geschrieben; Abschnitt 0 ist der Stand.

## 0. Stand

| Schritt | Stand |
|---|---|
| 1 Teile vermessen | erledigt (Abschnitt 1, 3) |
| 2 Krallen lesen | erledigt (Abschnitt 2) |
| 3 Varianten K1/K2/K3 | gebaut (Abschnitt 4, 5) |
| 4 Messen | Abschnitt 6 |
| 5 Kontaktblaetter | `blaetter/` (Abschnitt 9) |
| 6 Folgen Stufe 3 | Abschnitt 8 |
| 7 E20 | in NUTZER_ENTSCHEIDE.md angehaengt |
| Nachtrag Loecher | Messwerkzeug, Gelenkkoerper, vorher/nachher (Abschnitt 7) |

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

## 6. Messwerte der Varianten

(folgt nach dem Endbau)

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

(Endstand folgt)

## 8. Folgen fuer Stufe 3 (notiert, nicht umgesetzt)

(folgt)

## 9. Dateien

(folgt)
