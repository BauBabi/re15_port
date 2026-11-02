# Form bemalt: Dossier

Stand 2026-11-01, Arbeitsbaum r41_form, kein Commit, kein RE-Wert. Neues Modell nach zwei Videostandbildern. Formvorlagen nur `bild1.png`, `bild2.png`, bindend `nutzer_teile/` und NUTZER_ENTSCHEIDE E1 bis E4.
Ausgang `rohbau_final/` (unveraendert: FORM.EMD md5 71e7f298, FORM.TIM f98623f4, stand_final.json bfff27f6). Generator dorthin kopiert (`rf_*.py`), neu sind `fb_*.py`.

**Abgabe:** `FORM.EMD` md5 1a61353b, `FORM.TIM` md5 a4853be4, `form.json`, 2208 Flaechen, 32 Knochen, 3 Clips, Textur 384x256, 255 Farben, 1 CLUT-Zeile.
Blaetter: `neben_bild1.png`, `neben_bild2.png` (Aufbau wie rohbau_abnahme_3/bild), `rundum.png`, `rundum_hell.png`, `spiel_pose2_r5090_s11.png`, `spiel_pose1_r5080_s1.png`, `spiel_ruhe_r5080_s1.png`, `spiel_pose1_nachstellung_r5080.png`, dazu `bericht.txt`, `teile_bild1.png`, `teile_bild2.png`, `textur.png`, `atlas.json`. Messlauf `lauf/m33/`.

## 1. Kennzahlen (form_pruefung.py pruefe, vorher = rohbau_final in diesem Ordner gebaut, bytegleich)

| Messung | Rohbau | Jetzt |
|---|---|---|
| Umriss-IoU Bild 1 / Bild 2 | 0,9801 / 0,9166 | **0,9810 / 0,9402** |
| Pruefpunkte | 31/31, 7/7 | 31/31, 7/7 |
| Boden, Verdeckung Bild 1 | 5/7 (L1 6 px, L2 11 px) | 5/7 (L1 3 px, L2 7 px, je 1,0 / 1,4 px tief); KERBE, KERB_S, KERB_O, KERB_U ok |
| Verdeckung Bild 2 | 9/9 | 9/9 |
| Zeichnung | 8/9 (Z5 NEIN), 7/7 | **9/9, 7/7** (Z5 Material 0,97), N1 0,000, N2 0,000 |
| Original gegen Filter Bild 1 (MAE / NCC in Weiss) | 18,8 / 0,684 | 19,2 / 0,699 |
| Original gegen Filter Bild 2 | 14,6 / 0,647 | 13,6 / 0,674 |
| Teile IoU Bild 1: tail, left_leg, right_leg, right_arm, part_of_Body | 0,87 0,68 0,82 0,78 0,80 | 0,89 0,65 0,82 0,78 0,76 |
| Teile IoU Bild 2: face_and_upper_body, left_arm_front, right_arm_front | 0,83 0,75 0,70 | 0,84 0,75 0,70 |
| Spitzen linke Klaue Bild 2 (439,418) / (468,417), naechstes Renderpixel | 7,1 / 6,7 px | **2,2 / 1,0 px** |
| Flaechen | 1900 | 2208 (Rahmen "rund 1900" um 16 % ueber, Abschnitt 8) |

8 von 8 Teilen am Platz (teile_bild1.png, teile_bild2.png). part_of_Body faellt, weil die Schwanzwurzel jetzt nach A1 tiefer im Becken laeuft und der Schwanz den Koerper anders verdeckt (verdeckt von tail 0,46).

## 2. Glaetten (E4)

- **Rundum**: Catmull-Rom geschlossen durch die Ecken, halb flaechentreu. Rumpf und Kopf 12 statt 10 Ecken, Schwanz 10 statt 8, Glieder 8.
- **Laengs**: PCHIP je Spalte (kein Ueberschwingen), Zwischenringe nur ueber Luecken ab 420 Einheiten.
- **Normalen** je Ecke aus den Seitenflaechen; Gelenkenden ohne Kuppelring, im Nachbarn versteckt, Normalen dort radial (kein dunkler Ring an der Fuge). Port-Windung (cross(b-a, c-a) gegen die Normale), `FE.orientiere` dreht 0 Flaechen.
- **Schwanz** (`fb_form.schwanz_profil`, `ketten`, `normalen_ketten`): alle Ringe der sechs Glieder in Rastlage hintereinander, je Ecke ein Glaettungsspline ueber die ganze Kette, Radius = max(alt, Spline): die Taillen an den Gelenken sind gefuellt, die Breite in Bild 1 bleibt. Querschnitt runder (Abweichung vom Ringmittel x 0,45). An jedem Gelenk laufen beide Glieder auf das Mittel ihrer Radien zu (Zone 260), die Ueberlappringe liegen mit 0,8 innen (mit 0,98 zeichnete der Maler Saegezaehne). Normalen an End- und Anfangsring gemittelt. Blatt `zw/m26_s.png` gegen `zw/m22_s.png`.
- **A1**: schwanz1-Ringe vor der Hueftmitte (t < -60) sinken mit Rampe (260) um 430 nach Knochen +Y in den Becken, die Kegelspitze ebenso.
- **Schulter**: Brust umschliesst oben und seitlich die Oberarm-Masse (konvexe Huelle aus Brustring und Kugel 430 an der mittleren Lage der drei Clips). Oberarmkopf (Ringe t -270, -130, 60) x 0,62 / 0,74 / 0,88. Die Ruhepose (frei) hat die Arme jetzt unter den Schultern: erst damit liest der Oberarm von vorn als Glied statt als Kugel (`zw/g8_ton_c2.png` gegen `zw/g9_ton_c2.png`).
- **Nachpassung** (`fb_nach.py`): log-Faktor je Ausgangsring und Ecke auf den Glattecken, weiche Rasterung mit gemessenem Versatz gegen den Pruefrender (-0,75/-0,75 und -1,0/-0,75 px, `zw/dbg_nach.py`), Zonen der Passung, Boden L1, L2, SCH, BO, FUGE, Kante V6, Schwanzkante nl_y342 als eigener Kanal. Groesster Faktor 1,27.
- Knollen und Lappen am linken Bein: per Malerei (Abschnitt 4). Form war nicht noetig, der Umriss folgt der Nutzermaske schon (`zw/linkes_bein.png`).

## 3. Klaue links (Auftrag 2)

Kegelspitzen von `klaue_l2` und `kralle_l` im Knochenraum verschoben (`fb_form.KLAUE_LINKS`, `zw/dbg_klaue.py`): Spitze projiziert in Bild 2 auf das Ziel der Abnahme plus 2 px in Klauenrichtung, Nebenbedingung in Pose 1 und Ruhe mindestens 25 ueber dem tiefsten anderen Punkt (ohne sie hob boden_nachfuehren Bild 1 um 4 px, Lauf g16). Vorring voller (0,5 / 0,62).

## 4. Malen (`fb_mal.py`, Stellschleife `fb_stell.py`)

Schichten je Texel: Grund zwischen Tiefe, Schatten, Licht der Grundhaut, gemaltes Oberlicht aus der Normale der Ruhepose, gemalte Falten (Naehe zu anderen Teilen), Querfalten laengs der Knochenachse, Muskelfurchen an den Gliedern, dunkelrote Adern, Glanz in Rotbraun auf den hoechsten Woelbungen. Dann die **Bildstruktur**: wo Bild 1 oder 2 die Flaeche zeigt, traegt die Albedo das Hell-Dunkel des Bilds im Kleinen (Verhaeltnis zum Umfeld, Staerke 1,7); deutlich hellere Stellen werden Lichtflecken in "feine helle Linien" (die blaeulich-weissen Flecken des Videos sind neutral helles Grau). So sitzen Knollen und Lappen des linken Beins an ihrer Stelle. Nicht am Schwanz (E4). Freie Seiten (kein Bild zeigt sie) tragen dieselbe Sprache kraeftiger: Flecken, Falten, Glanz.

Zeichnung an ihrer Stelle, weich gerandet: Z1 rosa, Z2-Z4, **Z5 als Dreieck** (400,276)-(386,299)-(402,300)-(418,295)-(410,286), rund 32x24 px, Unterrand nach E1 bis (417,293), Spitze auf b1.kappe_spitze (400,277) (ein hoeheres Dreieck, 28 px, riss den Pruefpunkt: -6,5 px), Z6-Z9, Zug Q als Linie, Q1-Q5, G am Unterarm, P, Schwanzende X rosa-weiss-rot. Faktor je Marke aus der Stellschleife (Render gegen zurueckgerechnete Schirmfarbe), Z8 untere Grenze 1,15.

G und Bild 1: Dieselben Texel des rechten Unterarms zeigt Bild 2 als hellen Streifen G und Bild 1 dunkel neben Z5. Geloest: G nur auf Unterarm/Hand rechts oder auf Texeln, die Bild 1 nicht sieht, nicht im Armzylinder N2, und neben Z5 in der Farbe der Baender (hell in beiden Bildern). G Y 113 Video gegen 70 Render (Rohbau 54).

Farbe und Helligkeit (gemessen, `fb_hautmass.py`): Die Haut im Rohbau-Render war halb so hell wie die zurueckgerechnete Schirmfarbe (Bild 1 Y 6-16 gegen 15-24). Darum Hautfaktor je Knochen (zw/haut.json, Grenze 0,7-1,8) gegen farbe_bild*_echt.png. Farbton: Durch den Filter lag die Figur in Bild 1 zu rot (Rot 76 gegen 52, Lauf m4). Hautton nur aus Bild 1 (Bild 2 hat Rot-Einstreuung, farbe.md) und mit einer Palettenregel: R mindestens 1,3 x G, B hoechstens R, Kanaele in der Huelle Grundhaut von bis Rotbraun bis. Ohne die Regel wurde die Haut grau-blau (Lauf m5), das ist keine Farbe der Palette. Textur jetzt Mittel (43,30,37), Median (38,23,31); Palette Grundhaut (44,18,27). Filter Bild 1 Figur (41,33,56) gegen Original (40,36,61).
Haende und Krallen in Palettenton Rotbraun (103,63,84) x 0,75 statt rf_textur.ROTBRAUN (las als rosa Handschuhe).

## 5. Spiel (echtes Fenster, `fb_spiel.sh`, gdigrab, je `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, rc 0)

- ROOM5090 Schnitt 11 (Pose 2, `spiel_pose2_r5090_s11.png`): Kuppel mit Bogen Q und hellen Marken, beide Arme und die linke Klaue sind zu erkennen; dunkle Masse vor dunkler Tuer, das Feuer des Ports steht vor Hand und Unterarm.
- ROOM5080 Schnitt 1 (Pose 1 `spiel_pose1_r5080_s1.png`, Ruhe `spiel_ruhe_r5080_s1.png`, Nachstellung von Bild 1 `spiel_pose1_nachstellung_r5080.png`): Glieder, Schwanz, Kopf und Krallen lesbar, Haut violett-dunkel mit Flecken und Glanz; in der Nachstellung tragen Ruecken und Arm die Baender, der Schwanz das rosa Ende. Vorher (Rohbau, Abnahme 3): "nur als Umriss".

## 6. Was im Bild noch abweicht

- Bild 1: Das Original zerfaellt staerker in kleine Lappen und Lichter als der Render, besonders Ruecken und Schwanzwurzel; der Schwanz ist nach E4 glatt gelassen. Der grosse helle Bereich rechts am Koerper (Z4) ist im Render fleckiger.
- Bild 2: Der Unterarm G ist im Original ein breiter, hell gerippter Streifen, im Render dunkler (Y 70 gegen 113): Bild 1 sieht dieselben Texel dunkel. Die Hand ist im Original dunkler.
- Rundum: Die rueckprojizierten Marken lesen aus fremden Blickwinkeln als Flecken, nicht als Baender. Am Rumpf bleiben Naehte zwischen Huefte, Wirbel und Brust (Teleskop, in den Posen gebogen), am Schwanz kleine Saegezaehne an den Gelenken (Maler ohne Tiefenpuffer, wie im Port). Oberarm von vorn in der Ruhepose weiter massig.
- Farbton der Haut: Bild 1 verlangt neutraler, die Palette rotbraun; die Palettenregel gewinnt, der Rest bleibt als leichtes Magenta im Filter.

## 7. Durchgaenge

| Lauf | Aenderung | Befund am Bild / an der Zahl |
|---|---|---|
| basis | `rf_bau.py stand_final.json` | bytegleich mit rohbau_final |
| g1 | Glaetter (Catmull-Rom, PCHIP, Kuppeln, Normalen) | 6660 Flaechen, IoU 0,977; Kugel-Oberarme bleiben |
| g2 | A1, Gelenkangleichung, Schulterwulst | Normalen nach innen gefunden, behoben; SCH 19 px vom Knie |
| g3-g5 | Beine aus der Kette, halb flaechentreu, Nachpassung | 31/31, Bild 2 0,932 |
| g6 | Rahmen: keine allgemeine Verfeinerung | 2474 Flaechen |
| g7-g9 | Schulterhuelle, Oberarmkopf, Ruhepose Arme unter den Schultern | von vorn Saeulen statt Kugeln |
| g10-g17 | Versatz weich/Pruefrender, Schwanzkante, Klaue links mit Bodenbedingung | 31/31, 7/7, 9/9, Spitzen 2,2 / 1,0 px |
| m1-m3 | Maler (Grund, Form, Falten, Bildstruktur, Zeichnung), Z5 Dreieck, Atlas dichter (0,0268 -> 0,0285) | Zeichnung 9/9; Spiel: glatt und dunkel |
| m4-m6 | Haut je Knochen an die Schirmfarbe, Hautton aus Bild 1, Palettenregel | Filter Bild 1 von Magenta zu Violett |
| m7-m8 | Bildstruktur 1,7, Lichtflecken | NCC Bild 1 0,707; Knollen am linken Bein sichtbar |
| m9-m15 | G am Unterarm gegen Bild 1 abgewogen | G Y 64 -> 70, kein roter Fleck mehr neben Z5 |
| m16-m18 | Freie Seiten kraeftiger, Haende in Palettenton | Spiel 5080: Flecken und Glanz lesbar |
| m19-m26 | Schwanz: Normalen gemittelt, Profil ueber die ganze Kette, Taillen gefuellt | Schwanz durchgehend (zw/m26_s.png) |
| m27-m33 | Nachpassung Regel 0,5, Z8, Vorringe und Kuppelringe gespart, Grund palettentreu (gruene Hand), Augen kleiner | 2208 Flaechen, IoU 0,981 / 0,940, alles gruen ausser L1/L2 |

## 8. Frei gesetzt (kein Bildbeleg)

Kopf und Gesicht (Augen, Maul, Zaehne), Ruhepose (Arme unter den Schultern, oz 12, uz -30, hz 18), Schultermasse (Kugel 430 an (680,130,+-840) im Brustraum), Oberarmkopf-Faktoren, A1-Rampe (260) und Tiefe (430), Schwanzglaettung (Spline-Staerke, Rundung 0,45, Gelenkzone 260, Innenlage 0,8), Eckenzahlen und Ringabstand, Hauttoene innerhalb der Palette, Faltenabstand und -staerke, Adern, Glanz, Flecken der freien Seiten, Staerke der Bildstruktur, weicher Markenrand, Dreiecksform von Z5 im Rahmen der Lesung. Flaechenzahl 2208 gegen den Rahmen "rund 1900" (pruefung.md): Rumpf und Kopf 12 Ecken, Schwanz 10, sonst wie der Rohbau; RE2 G4 hat 1427.

## 9. Dateien und Lauf

Generator: `fb_glatt.py` (Glaetter), `fb_form.py` (A1, Schulter, Oberarm, Schwanz, Klaue), `fb_bau.py` (Bau, Atlas, k-Mittel-CLUT), `fb_nach.py` (Nachpassung), `fb_mal.py` (Maler), `fb_stell.py` (Marken, Haut, Hautton), `fb_neben.py`, `fb_blick.py` (Sichtblatt), `fb_diff.py`, `fb_hautmass.py`, `fb_spiel.sh`, `fb_abgabe.sh`; aus dem Rohbau `rf_*.py` (`rf_ruhe.py` mit Parametern oz/uz/hz erweitert). Zwischenstaende in `zw/` (delta.json, haut.json, hautton.json, marken.json, ruhe.json), Laeufe in `lauf/`.

```bash
cd analysis/befunde_runde41/form/form_bemalt
export PYTHONIOENCODING=utf-8
python rf_ruhe.py stand_final.json arm=12 oz=12 uz=-30 hz=18 hand_x=120 hand_z=-330 ausg=zw/ruhe.json
python fb_bau.py stand_final.json --tag a                       # Form mit zw/delta.json
python fb_nach.py stand_final.json --schritte 250 --start zw/delta.json --reg 0.5
bash fb_abgabe.sh ende spiel                                    # Stellschleife, Bau, Pruefung, Blaetter, Spiel
```

Kein Commit. Geschrieben nur in `form_bemalt/` und in `re15_port/build/form_spiel/` (meine zehn Laufordner danach geloescht). rohbau_final, pri/, synchro/, shared_assets/ und der Hauptbaum unberuehrt; nur eigene Prozesse.
