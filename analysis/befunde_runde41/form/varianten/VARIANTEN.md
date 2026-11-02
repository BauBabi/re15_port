# Bahnsteig-Form: Variantenmatrix zu E8 (und E9)

Stand 2026-11-01, Arbeitsbaum r41_form, kein Commit, kein RE-Wert. Neues Modell nach zwei Videostandbildern; RE2-Modelle nur fuer das Handwerk angesehen, nie fuer die Form.
Ausgang: `form_bemalt/` (FORM.EMD md5 1a61353b, FORM.TIM a4853be4). Generator-Kopie in `varianten/gen/` (bytegleich nachgebaut, Lauf `gen/lauf/basis`).

Kurzantwort Abschnitt 5, Messwerte Abschnitt 3, Bilder: `uebersicht_bild1.png`, `uebersicht_bild2.png`, `spiel_uebersicht.png`.

## 1. Handwerk der RE2-Gegner, gemessen (`gen/re2_handwerk.py`)

CDEMD0.EMS ueber die TOC (wie birkin_bahnsteig.lade_rohdaten). Dichte = Wurzel(UV-Flaeche / 3D-Flaeche) je Dreieck, flaechengewichtet, Texel je Einheit.
Farben = verschiedene 15-Bit-Farben, die die Flaechen einer CLUT-Zeile benutzen; "90 %" = so viele Farben decken 90 % der Texel.
Stil = Nachbar-Texel (waagrecht und senkrecht) im benutzten Bereich: gleich / Unterschied Y 1-6 / Unterschied Y ab 20.

| Modell | Flaechen | TIM | CLUT-Zeilen | Hoehe | Dichte p10 / Median / p90 | Farben je Zeile | 90 % | gleich / klein / hart | Mittel Y |
|---|---|---|---|---|---|---|---|---|---|
| RE2 0x34 (G4) | 1427 | 384x256 | 3 | 2263 | 0,026 / 0,042 / 0,059 | 256, 248, 244 | 112, 71, 94 | 0,35 / 0,17 / 0,12 | 51 |
| RE2 0x36 (G5) | 963 | 512x256 | 3 | 2489 | 0,024 / 0,031 / 0,039 | 251, 251, 240 | 97, 114, 92 | 0,25 / 0,19 / 0,19 | 64 |
| RE2 0x33 | 1065 | 384x256 | 3 | 4474 | 0,035 / 0,048 / 0,063 | 231, 255, 240 | 82, 79, 95 | 0,33 / 0,21 / 0,12 | 60 |
| RE2 0x31 | 965 | 384x256 | 3 | 3646 | 0,045 / 0,060 / 0,082 | 253, 255, 247 | 110, 102, 101 | 0,30 / 0,16 / 0,17 | 72 |
| RE2 0x30 | 947 | 384x256 | 3 | 3468 | 0,048 / 0,064 / 0,085 | 254, 254, 253 | 121, 132, 115 | 0,24 / 0,13 / 0,26 | 99 |
| RE2 0x23 | 979 | 512x256 | 4 | 1973 | 0,015 / 0,024 / 0,044 | 241, 246, 255, 253 | 97-110 | 0,21 / 0,21 / 0,19 | 66 |
| RE2 0x10 | 457 | 256x256 | 2 | 3218 | 0,045 / 0,059 / 0,076 | 253, 256 | 93, 110 | 0,35 / 0,17 / 0,15 | 72 |
| **form_bemalt** | **2208** | 384x256 | **1** | 2926 | **0,016 / 0,028 / 0,037** | 255 | **39** | **0,61 / 0,23 / 0,03** | **36** |

Was das sagt:
- **Texeldichte ist nicht der Unterschied.** form_bemalt liegt mit 0,028 zwischen G5 (0,031) und 0x23 (0,024); Texel ueber die Hoehe 81 gegen G5 78, G4 95. Ein Texel ist auf dem Schirm also nicht kleiner als bei RE2-Bossen.
- **Kontrast je Texel ist der Unterschied.** RE2: 21-35 % der Nachbartexel gleich, 10-26 % springen um 20 Y und mehr. form_bemalt: 61 % gleich, 3 % hart. Eine glatte Textur unter Gouraud-Licht liest wie ein weich schattiertes Rechnerbild, also "hoeher aufgeloest".
- **Wenige Farben je Teil stimmt fuer RE2 nicht:** RE2 nutzt je Zeile 230-256 Farben, 70-130 fuer 90 % der Texel, je Mesh genau eine Zeile (gemessen, `handwerk.json` je_mesh). form_bemalt deckt 90 % mit 39 Farben ab: zu WENIGE Toene, nicht zu viele.
- **Helligkeit**: RE2-Texturen Mittel Y 51-99, form_bemalt 36. Bei RE2 traegt die Textur das Licht (gemalte Hoehen hell, Furchen fast schwarz), die Engine legt nur wenig darueber.
- **Flaechen**: RE2 457-1427, Bosse 963-1427. form_bemalt 2208 (Abschnitt Geometrie).
- Ansicht: `gen/handwerk/tex_*.png` (3-fach). RE2 malt je Teil ein Bild: grosse Muskelformen, dunkle Furchen, helle Oberkanten, Fasern, Adern; form_bemalt ist dunkler Brei mit Rauschflecken und aufgesetzten Marken.

## 2. Was jede Variante ist

Alle Varianten: gleiche Kamera, gleiche Posen, gleiche Knochen, gleiche Lage aus `stand_final.json` wie form_bemalt. Gebaut mit `gen/vm_bau.py`, gemalt mit `gen/vm_mal.py`, Geometrie `gen/vm_form.py`, alles in `gen/vm_alle.sh`.

**Textur im RE2-Stil (gilt fuer A, B, C, G)** (`vm_mal.Maler`):
- Atlas mit Gewicht 1 fuer alle Teile (form_bemalt 0,55-1,35 je Teil): Texeldichte 0,0307 Texel je Einheit ueberall gleich (vorher p10/p90 0,016/0,037).
- Wert je Texel aus gemalter Form, alles 3D im Bindraum (Rastlage), also ohne Naht an den Schwanzgelenken: Muskelmassen (Zellen 430 Einheiten, laengs 1,8-fach gestreckt), harte dunkle Furchen nur an einem Teil der Massengrenzen mit wechselnder Breite, Woelbung je Masse, Fasern laengs der Knochenachse (quer 34 Einheiten, also rund ein Texel), Korn, grosse Hell-Dunkel-Flecken (650), wenige Adern.
- Licht durchgehend von oben (eine Richtung fuer alle Teile) aus der Normale der Ruhepose, gestoert durch die Woelbung der Massen; gemalte Umgebungsverdeckung (Naehe zu anderen Teilen), nicht zwischen Schwanzgliedern.
- Rampe Wert -> Farbe: Furche fast schwarz (Grundhaut x 0,16), Schatten, Grundhaut, Rotbraun, Rotbraun mit etwas Rosa (Glanz). Helle Faserzuege oben in der Farbe "feine helle Linien".
- KEINE Foto-Rueckprojektion, keine Bildstruktur, keine Lichtflecken aus dem Bild, kein Faktor je Knochen (form_bemalt: zw/haut.json 1,1-1,8 je Knochen, am Schwanz 1,12 / 1,19 / 1,41 / 1,28 / 1,12 / 1,24 je Glied = Helligkeitssprung je Glied).
- Zeichnung Z1-Z9, Q, G, P, X: Lage aus denselben Bildmasken wie bisher (bindend), aber gemalt: feste Flaeche (Deckkraft > 0,5, je Teil geschlossen) statt weicher Stempel, die Form der Haut scheint durch (Wert und Licht, je Marke begrenzt auf ihre Materialspanne), Rand 1 Texel dunkel nur auf der Haut, auf der Schattenseite dunkler.
- Zonen (Haende, Krallen, Zaehne, Maul, Auge) wie form_bemalt, mit demselben gemalten Licht.
- Drei CLUT-Zeilen wie RE2 (je Mesh eine Zeile): 0 Rumpf/Kopf, 1 Arme/Haende, 2 Beine/Schwanz, je bis 255 Farben (k-Mittel je Zeile).
- Helligkeit je Variante geeicht (`vm_bau.eichen`): ein Faktor fuer die ganze Haut, einer fuer den ganzen Schwanz, so dass das gefilterte Render in den Figurmasken die mittlere Helligkeit des Standbilds hat (Koerper: Bild 1 ohne Schwanz und Bild 2 im geometrischen Mittel; Schwanz: Nutzerteil tail in Bild 1). Damit unterscheiden sich die Varianten nur in Malstil und Farbton. Rest: Bild 1 will rund 11 % dunkler, Bild 2 rund 13 % heller als der gemeinsame Faktor (ein Faktor fuer beide Bilder, bewusst kein Faktor je Knochen).

**Farbe** (`vm_mal.farbsatz`):
- **A Rueckrechnung**: Materialfarben `oberflaeche_rgb` aus farbe_palette.json (Grundhaut (44,18,27), Rotbraun (103,63,84), Rosa (181,137,150), Baender (126,145,156), feine Linien (114,123,143)).
- **B Screenshot**: Farbton und Saettigung des Videos Bild 1 (`video_bild1_rgb`), Helligkeit Y der Palette (nur die Helligkeit des Videos herausgerechnet; Unschaerfe steckt nicht in den Werten, sie sind Flaechenmittel): Grundhaut (30,22,41) dunkles Blauviolett, Rotbraun (104,58,107) Magenta-Violett, Rosa (185,131,167), Baender (136,137,169) blaeulich-weiss, Linien (111,120,167). Farben ausserhalb der Palette (Hand, Horn, Zaehne, Adern) ueber die lineare Abbildung A -> B (Rest <= 10 je Kanal).
- **C Mitte**: je Material (A + B) / 2: Grundhaut (37,20,34), Rotbraun (103,61,96), Baender (131,141,163).

**E9 Schwanz (gilt fuer A, B, C, G)** (`vm_form.schwanz_e9`):
- Profil aus dem Endstand von form_bemalt (Ringe nach Glaetter und Nachpassung) ueber die ganze Kette mit einem Ausgleichsspline (ein Knoten je 750 Einheiten, die Glieder sind 592-678 lang): die Baeuche je Glied (Radien 264-337-339-275 | 275-342-275-201 | ...) sind weg, der Verlauf bleibt.
- An jedem Gelenk enden Eltern- und Kindglied mit DEMSELBEN runden Ring am Drehpunkt; dahinter Kugelkappen (30 und 62 Grad, Spitze im Abstand r). Eine Kugel um den Drehpunkt dreht sich in sich, darum fuellt sie die Aussenseite der Beuge ohne Kerbe (gemessene Gelenkwinkel Pose 1: 22, 14, 7, 14, 28 Grad; Ruhe 12, 9, 4, 8, 10). Vorher Ueberlappringe mit 0,8 r und kurze Kegelspitzen.
- Normalen: Gelenkring und Kappen radial (die Kappen zeigen sich nur aussen an der Beuge und setzen dort die Roehre fort; mit Kugelnormalen lasen sie als dunkle Zaehne, Lauf `_probe/C3e9`), Gelenkring und Nachbarring vorgedreht um die halbe mittlere Gelenkdrehung aus Ruhe und Pose 1.
- Textur: Kappen tragen die Bemalung ihres Gelenkrings (Malort auf den Gelenkring geklemmt), Muster im Bindraum laeuft ueber die Gelenke durch, ein Helligkeitsfaktor fuer den ganzen Schwanz.
- Am Schwanz keine Nachpassung je Ring mehr (sie brachte die Gliederstruktur zurueck); Breite kommt aus dem geglaetteten Endstand. Bild 1 Teil tail IoU 0,88 (vorher 0,89), Umriss-IoU 0,981 bleibt.
- Flaechen +190 (Kappen): 2398.

**G weniger Flaechen**: wie C (Textur, Farbe, Schwanz E9), dazu: keine Zwischenringe laengs (vorher alle 420 Einheiten), Krallen und Zehen 4 Ecken (6), Kopf 8 (12), Kiefer und Hals 6 (8), Schwanz eine Kugelkappe je Gelenkseite (zwei); Umrissglieder (Rumpf 12, Glieder und Haende 8, Schwanz 10) wie C, Nachpassung zw/delta.json wie A-C. **2100 Flaechen** gegen 2398 (A-C) und 2208 (form_bemalt). Glaettung allein ueber die Normalen.

Warum nicht 1300-1600 (RE2-Bossmass): Mit gehaltener Passung kam ich nicht tiefer. Laeufe in `_probe/`:

| Lauf | Ecken Rumpf/Schwanz/Glied/Hand/Kopf, Schwanzringe | Flaechen | IoU Bild 1 | Was faellt |
|---|---|---|---|---|
| G1 | 10/8/7/6/10, alter Schwanz | 1916 | 0,975 | nr_y322_links |
| G3 | 8/8/6/5/7, Ringe 450 | 1598 | 0,971 | flanke_2, rand_rechts, fr_y324, KERB_O |
| G4 | wie G3 + Nachpassung (fb_nach mit dieser Topologie) | 1598 | 0,973 | scheitel_x305, flanke_2, KERB_O, P |
| G5 | 8/8/6/7/7 + Nachpassung Regel 0,3 | 1662 | 0,976 | scheitel_x322, Kerbe E3 (Keil gedeckt 0,21) |
| G10/G11 | 10/8/7/7/7, Schwanz-Innenkreis ausgeglichen, Nachpassung bis 400 Schritte | 1840 | 0,977 | kappe_spitze, scheitel_x305, N1 |
| G12 | wie G10, jeder Ring mit dem Innenkreis der vollen Ecken | 1840 | 0,964 | 5 Kantenpunkte (ueberschiesst) |
| G13 | 10/10/8/8/8, keine Zwischenringe, Krallen 4 | 1988 | 0,979 | nur IoU |
| G14 | 12/10/8/8/8, Schwanzringe 400 | 2050 | 0,9797 | nur IoU |
| **G15 = G** | 12/10/8/8/8, Schwanzringe 300 | **2100** | **0,9802** | nichts (wie A-C) |

Die Nachpassung (fb_nach) hob dabei nur ihr weiches Mass (0,9814 -> 0,9856), das echte nicht: ihr Modell kennt den E9-Schwanz nicht. Die Form ist an zwei Bilder auf 1-3 px gepasst; jede Ecke weniger an Rumpf, Schwanz, Gliedern verschiebt Kanten um 2-10 px. Zum Ansehen, wie 1598 Flaechen wirken (Passung verfehlt, darum keine Variante): `g_1600_nur_ansicht/uebersicht_bild1.png`, `..._bild2.png` (Lauf G4 gegen C).

## 3. Messwerte (form_pruefung.py pruefe, je Variante `*/pruefung/bericht.txt`)

| | Ausgang form_bemalt | A Rueckrechnung | B Screenshot | C Mitte | G weniger Flaechen |
|---|---|---|---|---|---|
| FORM.EMD / FORM.TIM md5 | 1a61353b / a4853be4 | 2449728a / 21ee9d80 | 2449728a / 0bc2e11e | 2449728a / a6c2c188 | 1227c2d9 / d75ffce9 |
| Flaechen | 2208 | 2398 | 2398 | 2398 | **2100** |
| Umriss-IoU Bild 1 / 2 | 0,981 / 0,940 | **0,9811** / 0,9402 | 0,9811 / 0,9402 | 0,9811 / 0,9402 | **0,9802** / 0,9401 |
| Pruefpunkte Bild 1 / 2 | 31/31, 7/7 | 31/31, 7/7 | 31/31, 7/7 | 31/31, 7/7 | 31/31, 7/7 |
| Kerbe E3 (KERBE, KERB_S, KERB_O, KERB_U) | frei | frei | frei | frei | frei |
| Boden, Verdeckung | 5/7 (L1, L2), 9/9 | 5/7 (L1 3 px, L2 7 px, wie Ausgang), 9/9 | wie A | wie A | 5/7, 9/9 |
| Zeichnung | 9/9, 7/7 | 9/9, 7/7 | 9/9, 7/7 | 9/9, 7/7 | 9/9, 7/7 |
| Gegenproben N1, N2 | ok | ok | ok | ok | ok |
| Teile 8/8 (IoU tail, left_leg, right_leg, right_arm, part_of_Body; Bild 2 face, left_front, right_front) | 0,89 0,65 0,83 0,78 0,76; 0,84 0,75 0,70 | 0,88 0,66 0,82 0,78 0,77; 0,84 0,75 0,70 | wie A | wie A | 0,88 0,66 0,82 0,77 0,76; 0,84 0,74 0,70 |
| Helligkeit Haut / Schwanz (Eichung) | Faktor je Knochen 1,1-1,8 | 1,462 / 0,492 | 1,488 / 0,494 | 1,482 / 0,492 | 1,483 / 0,514 |

**Farbabstand in den Figurmasken** (`matrix.json`; CIE76 der Mittelfarben, "Feld" = mittleres dE je Pixel nach Gauss 3 px; Standbild Bild 1 Mittel RGB (52,46,73), Bild 2 (73,20,30)):

| | gefiltert Bild 1 | gefiltert Bild 2 | sauber Bild 1 | sauber Bild 2 | Mittel RGB gefiltert Bild 1 | sauber Bild 1 |
|---|---|---|---|---|---|---|
| Ausgang | **1,5** / 11,6 | 4,7 / 11,8 | 22,1 / 24,2 | 23,3 / 25,2 | (56,46,71) | (25,18,18) |
| A Rueckrechnung | 6,8 / 15,2 | 3,0 / 11,2 | 21,7 / 24,4 | 20,6 / 22,6 | (69,46,71) | (30,16,17) |
| B Screenshot | 7,8 / 15,8 | 3,5 / 13,3 | **19,6** / 22,3 | 21,1 / 23,3 | (62,47,85) | (26,17,23) |
| C Mitte | **5,5** / 14,7 | **2,2** / 11,8 | 20,6 / 23,3 | 20,8 / 22,9 | (65,47,78) | (28,17,20) |
| G (Farbe C) | 5,7 / 14,8 | 2,2 / 11,9 | 20,5 / 23,2 | 20,8 / 22,9 | (65,46,78) | (28,17,20) |

Lesart, nicht Entscheidung: Durch den Filter liegt von den neuen Varianten C am naechsten (Bild 1 und 2), A zu rot/magenta, B zu violett. Der Ausgang liegt gefiltert in Bild 1 noch naeher (1,5), weil fb_stell den Hautton je Knochen an das Video gestellt hatte (zw/hautton.json, haut.json); das geht nur mit Faktoren je Knochen, die E9 am Schwanz verbietet. Sauber (ohne Filter, so wie der Nutzer pose_bild1/2 sah) liegen alle bei dE rund 20: das Video hebt die dunkle Figur an und faerbt sie (neutrales Grau e=60 wird im Video (116,134,186), farbe.md). Sauber am naechsten ist B in Bild 1.
Der Nutzer waehlt.

**Handwerk der Texturen** (`gen/handwerk_varianten/handwerk.json`, wie Abschnitt 1):

| | Dichte p10 / Median / p90 | CLUT-Zeilen, Farben je Zeile | 90 % | Nachbarn gleich / klein / hart | Mittel Y |
|---|---|---|---|---|---|
| RE2-Bosse (G4, G5, 0x23) | 0,015-0,026 / 0,024-0,042 / 0,039-0,059 | 3-4 x 240-256 | 71-114 | 0,21-0,35 / 0,17-0,21 / 0,12-0,19 | 51-66 |
| Ausgang | 0,016 / 0,028 / 0,037 | 1 x 255 | 39 | 0,61 / 0,23 / 0,03 | 36 |
| A / B / C | 0,020 / 0,029 / 0,036 | 3 x 196-235 | 25-86 | 0,35-0,36 / 0,34-0,37 / 0,05 | 34 |
| G | 0,020 / 0,029 / 0,036 | 3 x 205-222 | 25-70 | 0,35 / 0,37 / 0,05 | 35 |

Der harte Anteil (ab 20 Y) bleibt unter RE2, weil die Figur nach der Eichung halb so hell ist wie RE2-Texturen (Mittel 34 gegen 51-99). Die Streifenabbildung je Teil laesst die Dichte trotz Gewicht 1 zwischen 0,020 und 0,036 schwanken (Breite = groesster Umfang).

## 4. Was ich selbst sehe (uebersicht_bild1.png, uebersicht_bild2.png, spiel_uebersicht.png, g_1600_nur_ansicht/)

- **Ausgang gegen A**: Der Ausgang liest sauber wie ein weich schattierter dunkler Klumpen mit aufgeklebten hellen Flecken. A liest als gemalter Koerper: Muskelmassen mit dunklen Furchen, Licht oben, Fasern; die Marken sitzen in der Form. Im Filter verwischt das (Unschaerfe 2 px, Streulicht), aber die Massen bleiben als Hell-Dunkel lesbar, wie im Screenshot die Knollen am linken Bein.
- **Farbe**: A sauber dunkel rotbraun, gefiltert rot-magenta; B sauber dunkel blauviolett, gefiltert kraeftig violett-blau (satter als der Screenshot); C sauber dunkel pflaumenfarben, gefiltert violett mit etwas Magenta, dem Screenshot am naechsten. Im echten Fenster (spiel_uebersicht.png) ist A deutlich rot, B violett, C dazwischen.
- **Schwanz (E9)**: Ausgang in Ruhe als Kette von Wuersten mit Taillen und Helligkeitsspruengen je Glied; A-G in Ruhe und in Pose 1 ein durchgehender Strang ohne Absatz im Umriss, ohne Sprung in Licht und Textur (Spalten 4 und 5 der Uebersicht Bild 1, Spalte 4 Bild 2). Im Spiel steht der Schwanz als ein glatter Strang.
- **Flaechen**: C (2398) und G (2100) sind im Blick der Screenshots nicht zu unterscheiden. Auch 1598 Flaechen (g_1600_nur_ansicht, Passung verfehlt) sehen nach dem Filter fast gleich aus, sauber etwas kantiger an Armen und Schwanzende.
- Noch offen am Bild: Die Marken haben an den Raendern Treppen der Rueckprojektion (Lage bindend, Form aus der Bildmaske); der Kopf ist frei gesetzt; die Haut ist in Bild 1 rund 11 % zu hell und in Bild 2 rund 13 % zu dunkel (ein Faktor fuer beide).

## 5. Antwort: was den Eindruck "hoeher aufgeloest, nicht aus einem Guss, kein PSX-Stil" macht

1. **Die Textur (Hauptursache).** Belegt durch Abschnitt 1 und die Zeilen Ausgang/A: Die alte Textur war glatt (61 % gleiche Nachbartexel gegen 21-35 % bei RE2, 3 % harte Spruenge gegen 10-26 %), mit nur 39 Farben fuer 90 % der Texel, Hautbild aus dem Video zurueckprojiziert und Marken als weiche Stempel. Unter Gouraud-Licht liest so etwas wie ein glattes Rechnerbild. Die Texeldichte war es NICHT (0,028 Texel je Einheit, RE2-Bosse 0,024-0,042).
2. **"Nicht aus einem Guss"**: dazu die Faktoren je Knochen (zw/haut.json 1,1-1,8, am Schwanz je Glied anders) und die Gliederform des Schwanzes (Bauch je Glied, Ueberlappringe 0,8 r), siehe E9. Beides ist in A-G weg.
3. **Die Geometrie nur am Rand**: 2398 -> 2100 (G) aendert im Blick der Screenshots nichts Sichtbares, 1598 kaum. Mit gehaltener Passung kommt die Form nicht unter rund 2100 Flaechen (Abschnitt 2, Laeufe G1-G15).
4. **Farbe**: Der Nutzer hat das SAUBERE Render (dunkel rotbraun, Bild 1 Mittel (25,18,18)) neben das Video (52,46,73, blauviolett) gelegt. Die Rueckrechnung sagt, das Violett macht die Abfilmung; durch den Filter liegt C am naechsten. Soll die Figur im Spiel so aussehen wie der Screenshot, ist das B (im Filter dann zu violett). Entscheidung beim Nutzer.

## 6. Dateien, Lauf, Regeln

- Je Variante `A_rueckrechnung/`, `B_screenshot/`, `C_mitte/`, `G_flaechen/`: FORM.EMD, FORM.TIM, form.json, bau.json (Atlas, Zeilen, Eichung), textur.png, pruefung/ (bericht.txt, Blaetter), spiel_<variante>.png, generator/ (Kopie von gen/ mit zw/*.json, BAU.txt mit dem Aufruf; fuer C bytegleich nachgebaut).
- Uebersichten: `uebersicht_bild1.png`, `uebersicht_bild2.png`, `spiel_uebersicht.png`, `matrix.json`; Nebenblatt `g_1600_nur_ansicht/`.
- Generator: `gen/vm_bau.py` (Bau, Eichung, CLUT-Zeilen), `gen/vm_mal.py` (Maler), `gen/vm_form.py` (E9, weniger Flaechen), `gen/vm_matrix.py` (Uebersicht, Farbabstand), `gen/vm_nach.py` (Nachpassung fuer G, verworfen), `gen/vm_spiel.sh`, `gen/re2_handwerk.py`, alles: `bash gen/vm_alle.sh [spiel]`. Rest wie form_bemalt (fb_*, rf_*; rf_kern.py findet FORM jetzt ueber formblatt.json).
- Laeufe und Zwischenstaende: `_probe/` (Blatt m2, Pruefberichte der G-Laeufe, Nachpassungs-Logs).
- Kein Commit. Geschrieben nur in `varianten/` und `re15_port/build/form_spiel/` (meine fuenf Laufordner danach geloescht). form_bemalt, pri/, synchro/, shared_assets/, andere Arbeitsbaeume unberuehrt; im Spiel nur eigene PIDs (form_spiel.sh).

## 7. D Fleisch (E10 und Nachtrag: Farben aus den Screenshots)

Nutzer zu A-C: "alles wie billig einfarbig bemalt", Haut mehrfarbig, Wunden fleischig; Nachtrag mit Vorrang: Farben aus den Screenshots uebernehmen, vor allem aus Bild 1 (Rueckansicht). Geometrie und E9-Schwanz wie C (FORM.EMD bytegleich 2449728a), Textur neu: `D_fleisch/` (FORM.TIM ba268185), Generator `gen/vm_bild.py`, Aufruf `D_fleisch/generator/BAU.txt`.

**Handwerk RE2-Birkin** (0x31, 0x33, 0x34, `D_fleisch/handwerk/re2_fleisch.json`): Haut mehrfarbig, Farbtonstreuung 48-73 Grad (kuehl 12-35 %, warm 25-47 %, neutral 24-63 %), grau-violetter Grund mit warmen braunen und rosa Uebergaengen. Wunden rund 20 % der Textur, 260-360 Farben, Y 55-125, gesaettigtes Rot im Kern, verzweigte rote Adern, schwarzrote Tiefen, rosa-beige ausgefranste Raender, kleine helle Glanzpunkte.

**Weg** (`vm_bild.py`):
1. Je Texel Bildpunkt und Sichtbarkeit in Bild 1 und 2 (rf_textur.Bake). Farbe aus `farbe_bild{1,2}_echt.png` (form_video.umkehr: Kurve, Streulicht, Balance), geteilt durch das Port-Licht an derselben Stelle (dasselbe Modell mit weisser Textur durch Kandidat.rendern). Dann zeigt der Port dort die zurueckgerechnete Schirmfarbe. Gegenprobe: das saubere Render trifft das zurueckgerechnete Bild im Mittel (Bild 2 innen (30,13,12) gegen (32,13,12)).
2. Gewicht: Licht (unter 0,12 zaehlt nicht), Abstand zum Umriss (Bild 1 5 px: sonst traegt der Figurrand den tuerkisen Boden aus der Chroma-Unschaerfe 6 x 3 px; Bild 2 2,5 px: dunkler Grund, sonst fallen die duennen Zuege Q und G weg), Logo aus, Bild 1 Gewicht 1, Bild 2 0,5. Kein Ausgleich des Feuerscheins in Bild 2: ein Kanalfaktor aus gemeinsam sichtbaren Texeln faerbte die Vorderseite tuerkis (Bild 2 dE 2,2 -> 7,3, Lauf D1).
3. Unsichtbar: Gegenseite im Streifen (U -> 1-U, U -> U+1/2), dann Ausfuellen je Teil (Fortsetzung laengs und rundum), Teile ohne Bild vom Gegenstueck links/rechts. 25 666 der 76 890 Texel kommen direkt aus den Bildern, 51 224 sind ergaenzt.
4. PSX-Form: Farben je Teil in Bereiche zusammengefasst (Mean-Shift im Wurzelraum, damit dunkle Haut ihre Unterschiede behaelt), Kanten geschaerft, die gemalte Form von vm_mal leise darueber (Fasern, Korn, Woelbung, Furchen schwach). Nahtspalte U = 1 je Streifen bekommt die Farbe von U = 0. 8 bpp, drei CLUT-Zeilen je 255 Farben.
5. Wunden (Lage aus den Bildmasken wie bisher, Farbton aus dem Bild): Helligkeit je Stelle in 5 Schleifen geeicht, bis das gefilterte Render dort so hell ist wie der Screenshot (die Unschaerfe des Videos nimmt kleinen hellen Stellen die Spitze; Z1 142/139, Z2 137/137, Xb 150/150, Z6a 136/134, H9 63/63). Darin Fasern, dunkelrote Tiefen (70,12,20), feuchte Glanzpunkte oben im Licht, gerissener Rand mit dunklen Kerben, entzuendeter Ring aussen. Z5 traegt die Pruefpunkte kappe_spitze/kappe_basis (Material helle Baender): Z5 liegt in der aus demselben Bildbereich zurueckgerechneten Spanne, Rand ohne Kerben, innen nur abdunkelnde Struktur (Glanz und Aufhellung verletzten die Gegenprobe N2).

**Pruefung** (`D_fleisch/pruefung/bericht.txt`): IoU Bild 1 0,9811 / Bild 2 0,9402, Pruefpunkte 31/31 und 7/7, Kerbe E3 frei, Boden/Verdeckung 5/7 (L1, L2 wie Ausgang) und 9/9, Gegenproben N1/N2 ok, 8/8 Teile wie C. Zeichnung 6/9 und 5/7: diese Pruefung misst gegen die Palettenspannen der Rueckrechnung (helle Baender, feine Linien, z9); die Marken tragen jetzt die Bildfarbe, nicht die Palette.

**Messung** (`matrix.json`, `D_fleisch/handwerk/vielfalt.json`):

| | C | D | Screenshot |
|---|---|---|---|
| Farbabstand gefiltert Bild 1, Mittel / Feld (je Pixel) | 5,5 / 14,7 | 4,6 / **8,9** | Untergrenze 0,5 / 3,0 |
| Farbabstand gefiltert Bild 2 | 2,2 / 11,8 | 6,9 / **9,5** | Untergrenze 0,3 / 3,0 |
| Farbtonstreuung Textur | 20,1 Grad | **60,5** Grad | RE2-Birkin 48-73 |
| Farbtonstreuung gefiltert in der Figur, Bild 1 | 12,8 | 18,3 | 26,5 |
| Anteil kleiner Lichter (Y ueber Umgebung + 12), Bild 1 | 0,107 | 0,101 | 0,103 |

Das Feld-Mass (Abstand je Pixel nach Gauss 3 px) misst, ob die Farbbereiche an derselben Stelle sitzen: D halbiert es in Bild 1 fast. Der hoehere Mittelwert in Bild 2 kommt nicht aus der Textur: das zurueckgerechnete Bild selbst durch den Filter liegt bei 0,3; im Port-Render fehlt das Feuer, dessen Streulicht im Video ueber die Figur faellt.

**Was ich sehe** (`uebersicht_bild1.png` Zeile D, `D_fleisch/nah_D_fleisch.png`, `D_fleisch/spiel_D_fleisch.png`, `spiel_uebersicht.png`): Gefiltert sitzt in Bild 1 alles an seinem Platz: Z1 leuchtet rosa-magenta, Beine blauviolett, Baender hell, viele kleine Lichter auf Schwanz, Beinen und Ruecken wie im Screenshot. Sauber ist die Haut fleckig mehrfarbig (grau-blau, weinrot, braun, rosa, fast schwarz), keine Flaeche einfarbig; im Spiel liest die Figur als dunkles, fleckiges Fleisch mit hellen Stellen. Die Wunden Z1 und Z5 sind sauber noch eher helle Flaechen mit roten Tiefen als offenes Fleisch (Z5 ist durch die Pruefpunkte an die helle Bandfarbe gebunden). Die Vorderseite (nur Bild 2) traegt den roten Feuerschein des Bilds. Unsichtbare Seiten sind gespiegelt und wirken dort etwas wie ein Spiegelmuster.
- Offen an D: Die Schwanzspitze ist in der Ruhepose oliv-gruenlich (ergaenzt aus dem unteren Bildrand von Bild 1, wo Boden und Logo nah sind); die Spitze X selbst ist rosa-weiss-rot wie im Bild.
- Kein Commit. Geschrieben nur in varianten/ und re15_port/build/form_spiel/ (meine Laufordner geloescht).

## 8. E Ordnung (E12: Farben aus dem Screenshot, aber geordnet und gemalt)

Nutzer zu D: "Scheint besser zu werden, aber die Textur wirkt noch irgendwie kaputt und chaotisch". `E_ordnung/` (FORM.EMD 2449728a wie C/D, FORM.TIM 9713948a), Generator `gen/vm_ordnung.py`, Aufruf `E_ordnung/generator/BAU.txt`.

**Weg** (`vm_ordnung.py`):
1. Farbe nur grossflaechig: die Projektion wie D (Schirmfarbe / Port-Licht je Texel), aber nicht je Texel uebernommen, sondern in ein 3D-Gitter in der Ruhepose gesplattet und mit Gauss 110 Einheiten geglaettet (bei Bild 1 rund 11 px). Jedes Texel liest das Feld an seiner Weltlage: die Farbe laeuft ueber Teilgrenzen, Gelenke und UV-Naehte durch, es gibt keine Spiegelnaehte mehr (nichts gespiegelt) und keine Flecken unter Feldgroesse. Wo im Umkreis nichts gemessen ist (48 % der Texel), gilt das grobe Feld (450 Einheiten). Wundtexel zaehlen nicht ins Hautfeld.
2. Feuerschein Bild 2: von Bild 2 zaehlt nur die Helligkeit, der Farbton kommt aus dem groben Feld von Bild 1.
3. Kein Gruen: Ueberschuss von G ueber max(R, B) gekappt, Helligkeit gehalten (Gruen entstand in der Rueckrechnung dunkler Stellen; Schwanzansatz war oliv).
4. Gemaltes Detail mit Richtung, gleiche Gewichte fuer alle Teile: Licht von oben, Muskelstraenge laengs (Fasern 0,20), Furchen schwach (0,26, am Schwanz 0), Woelbung, Umgebungsverdeckung, verzweigte Adern; Kontrast rund 1:6 (Lauf E2 mit staerkeren Zellfurchen las wieder als Pflaster, E1 mit 1:3 glatt).
5. Wunden: Umriss aus den Bildmasken geglaettet (Gauss 0,8 Texel, Kleinteile unter 3 Texel weg), Farbe = mittlere Bildfarbe der Wunde, Aufbau nach Abstand zum Rand: heller Rand (aufgerissene Haut), rosa Fleisch, dunkelrote Tiefe, seltene Glanzpunkte oben im Licht, entzuendeter Ring aussen. Helligkeit je Wunde und der Haut (ein Faktor 0,90) in 5 Schleifen am Screenshot geeicht. Z5: Rand in der Spanne helle Baender, kein Glanz (Pruefpunkte, N2).

**Pruefung** (`E_ordnung/pruefung/bericht.txt`): IoU Bild 1 0,9811 / Bild 2 0,9402, Pruefpunkte 31/31 und 7/7, Kerbe E3 frei, Boden/Verdeckung 5/7 (L1, L2 wie Ausgang) und 9/9, N1/N2 ok, 8/8 Teile wie C. Zeichnung 6/9 und 4/7 (Palettenspannen, Marken tragen die Bildfarbe).

**Mass fuer "chaotisch"** (`E_ordnung/chaos.json`, `.png`; alle im selben Renderer form_pruefung.rundum_dreiecke, gleiche Groesse, vier Ansichten, Ruhepose; RE2 Clip 0):

| | HF (relativ) | kleine Farbinseln (Anteil Figurpixel) |
|---|---|---|
| C | 0,046 | 0,036 |
| D | **0,070** | **0,039** |
| E | **0,041** | **0,015** |
| RE2 G2 / G3 / G4 | 0,028 / 0,025 / 0,030 | 0,029 / 0,032 / 0,024 |

E hat weniger Farbinseln als RE2 und rund die Haelfte der Hochfrequenz von D; die Hochfrequenz liegt noch rund 1,4-fach ueber RE2 (die Haut ist dunkler, das Mass ist relativ zur Helligkeit; die Faserstraenge sind texelfein).

**Farbabstand gefiltert** (`matrix.json`): Bild 1 Mittel 3,7 / Feld 10,9 (D 4,6 / 8,9, Grenze 11), Bild 2 3,1 / 11,2 (D 6,9 / 9,5). Das Feld steigt gegen D, weil die Farbe nicht mehr je Texel sitzt; das Mittel faellt, weil der Feuerschein vorn weg ist.

**Was ich sehe**: `E_ordnung/vergleich_D_E.png` (sauber 1-fach Spielgroesse und 3-fach), `E_ordnung/rundum_hell_D_E.png`, `E_ordnung/spiel_E_ordnung.png`, Zeile E in `uebersicht_bild1.png`/`_bild2.png`. Bei Spielgroesse ist D ein zerhacktes Flickwerk, E eine ruhige dunkle Figur mit Farbverlaeufen je Bereich (Ruecken violett-braun, Beine und Haende blaugrau) und klar umrissenen rosa Wunden mit dunklem Kern. Im Rundum ist D rosa-rot-gruen gefleckt, E grau-violett-braun mit Straengen laengs; keine Spiegelmuster, keine gruene Schwanzspitze, vorn kein Rotstich. Gefiltert sitzen in Bild 1 Z1 rosa, die blauvioletten Beine und die hellen Stellen wie im Screenshot, etwas weicher als bei D. Offen: die Hautfarbe wirkt in der aufgehellten Ruhepose eher einheitlich grau-violett, die Farbunterschiede der Bereiche sind leise (Feld 110 Einheiten); die Haende sind flach blaugrau.

## 9. F Sauber (E13: D ohne Pixel-Mischmasch)

Nutzer: "D. Ich weiss auch nicht warum du dort an den Stellen so ein Mischmasch bei Pixeln machen musstest an Farben. Ansonsten so gingen die Farben ja FAST in die richtige Richtung." `F_sauber/` (FORM.EMD 2449728a wie C/D/E, FORM.TIM c62f0b15), Generator `gen/vm_sauber.py`, Aufruf `F_sauber/generator/BAU.txt`. D bleibt bytegleich nachbaubar (gegengeprueft).

**Weg** (`vm_sauber.py`): Projektion je Texel wie D (Bereiche, Kraft, Lage bleiben). Fehlerbilder wie in E behoben: Bild 2 nur mit Helligkeit, Farbton aus Bild 1 (Feuerschein vorn weg), unsichtbare Texel aus dem 3D-Farbfeld statt gespiegelt (keine Spiegelnaehte), Gruen gekappt (Schwanzspitze). Dann je Teil: k-Mittel (bis 7) im Lab-Raum mit schwacher Helligkeit (0,35), zusammenhaengende Stuecke, Inseln unter 6 Texeln (bei Bild 1 rund 8 x 8 px) an den haeufigsten Nachbarbereich (1580 Inseln), Kanten mit Mehrheitsfilter 3 x 3 zweimal geglaettet, im Bereich Farbton fest und Helligkeit weich (Gauss 1,2 Texel nur im Bereich), gemalte Form leise ohne Korn. Wunden wie D (Helligkeit je Stelle geeicht, Rand, Fleisch, Tiefe, Glanz, Z5), Umrisse geglaettet, Glanzpunkte, Tiefen und Randkerben nur als Gruppen ab 2 Texeln (Einzeltexel waren der Mischmasch auf Arm und Ruecken).

**Pruefung**: IoU Bild 1 0,9811 / Bild 2 0,9402, Pruefpunkte 31/31 und 7/7, Kerbe E3 frei, Boden/Verdeckung 5/7 (L1, L2 wie Ausgang) und 9/9, N1/N2 ok, 8/8 Teile wie C. Zeichnung 4/9 und 5/7 (Palettenspannen).

**Mass** (`F_sauber/chaos.json`, selber Renderer, vier Ansichten): kleine Farbinseln D 0,039, E 0,015, **F 0,015**, RE2 G2/G3/G4 0,029 / 0,032 / 0,024. Hochfrequenz D 0,070, E 0,041, **F 0,036**, RE2 0,025-0,030. Farbabstand gefiltert: Bild 1 Mittel 4,2 / Feld **8,5** (D 4,6 / 8,9, Grenze 10), Bild 2 4,2 / 11,3 (D 6,9 / 9,5; das Feld steigt vorn, weil der Feuerschein des Videos nicht mehr in der Textur steckt).

**Was ich sehe** (`F_sauber/vergleich_D_F.png`, `F_sauber/rundum_hell_D_F.png`, `F_sauber/spiel_F_sauber.png`, Zeile F unter D in `uebersicht_bild1.png`/`_bild2.png`): Bei Spielgroesse traegt F dieselben Bereiche wie D (weinroter Ruecken mit rosa Licht, blaugraue Beine und Haende, helle Baender, rosa Wunden), ohne bunte Einzeltexel und ohne Treppen an den Bereichskanten; 3-fach sieht man ruhige Flaechen mit klaren Kanten. Vorn (nur Bild 2) ist F dunkel violett-grau statt rot. Rundum keine Spiegelmuster, Schwanzspitze rosa. Offen: Wo kein Bild die Seite zeigt, ist die Haut einheitlicher grau-violett (Feld), und die Bereichskanten sind texelgenau (stufig wie RE2), nicht weich.

## 10. G treu (E14: Zeichnung in genau den Farben des Screenshots)

Nutzer zu F: "WO kommen denn die roten Pixel her auf den weissen Fleck? Die gibt es doch im Screenshot ueberhaupt nicht...." Ursache: der pauschale Wundaufbau aus E10 (vm_bild.wunden: rote Tiefen, Glanz, Kerben in jeder hellen Stelle). `G_treu/` (FORM.EMD 2449728a, FORM.TIM 1e56a79c), Generator `gen/vm_treu.py`, Aufruf `G_treu/generator/BAU.txt`. (Nicht verwechseln mit `G_flaechen/`, der Geometrie-Variante.)

**Weg** (`vm_treu.py`): Haut wie F. Jede Zeichnung je Texel aus der Projektion DES Bilds, in dem sie liegt (Schirmfarbe / Port-Licht), unsichtbare Markentexel mit dem Mittel der sichtbaren, innerhalb der Stelle Gauss 1,0 Texel (Verlauf bleibt, Projektionsrauschen geht). Helligkeit je Stelle am Screenshot geeicht, KEIN gemalter Wundaufbau (Fleischrot, Tiefen, Glanz, Kerben entfallen ueberall; Rot steht nur noch, wo die Projektion Rot hat). Z5: nur die obere Randkante und das kurze Basisstueck um x 398 (Pruefpunkte kappe_spitze/kappe_basis) in der Spanne helle Baender; die ganze Stelle zu klemmen machte sie olivgrau, flach und im Filter zu hell (Lauf T1), der Rand rundum las als Rahmen (T2). Die Teilstuecke von G, die Bild 1 neben Z5 sieht, bekommen nicht mehr den Faktor des G-Streifens aus Bild 2 (war ein weisser Fleck links neben Z5).

**Pruefung**: IoU 0,9811 / 0,9402, Pruefpunkte 31/31 und 7/7, Kerbe E3 frei, Boden/Verdeckung 5/7 (wie Ausgang) und 9/9, N1/N2 ok, 8/8 Teile wie C.

**Mass je Stelle** (`G_treu/stellen_G_F.png`, `.json`, `.txt`; fremd = Anteil gefilterter Modellpixel mit einem Farbton, den der Screenshot an der Stelle nicht hat; rot = Lab a* > 15, Farbton -35..45 Grad):
- Bild 1, Z1-Z9 und X: fremd 0,00 ueberall ausser Z4 (0,46; F 0,43) und Z6b (0,01). Z5 0,00 (F 0,10). Rot im sauberen Modell nur in Z1 (Screenshot 1,00 rot) und Xc (0,70); in allen Stellen ohne Rot im Screenshot 0,00.
- Bild 2: Q1, Q3, Qc, H9, Gg, Gr, P fremd 0,00; Qa 0,10, Qb 0,07, Q4 0,17, Q2 0,34, H8 0,33 (Feuerrot, Farbton etwas anders getroffen).
- Z4: der weisse Bereich von Z3 darueber reicht im Render tiefer als im Bild und deckt den Polygon von Z4 (Lage, nicht Farbton erfunden).
- Farbabstand gefiltert: Bild 1 Mittel 3,7 / Feld 7,5 (F 4,2 / 8,5), Bild 2 3,4 / 11,4. Chaos: Inseln 0,015, Hochfrequenz 0,029 (RE2 0,025-0,030).

**Was ich sehe** (`G_treu/stellen_G_F.png`, `G_treu/vergleich_F_G.png`, `G_treu/spiel_G_treu.png`, Zeile "G treu" in `uebersicht_bild1.png`): Z5 auf dem rechten Arm ist sauber eine dunkle Flaeche mit hellerer Oberkante ohne jedes Rot, gefiltert ein lavendelfarbenes Dreieck, oben am hellsten. Die Baender Z2, Z6a, Z7, Z8 sind glatte helle Zuege in Bildfarbe, Z1 rosa, das Schwanzende rosa-weiss-rot wie im Bild. Offen: Z4 (Lage des Weiss), Q2/H8 im Feuerlicht von Bild 2.

## 11. H Stoff (E15: kein Cel-Shading, Textur-Look wie die Spieltexturen)

Nutzer zu G: "sieht immer noch komplett gezeichnet aus ... so cell shading maessig". `H_stoff/h1/` (FORM.TIM prozedural) und `H_stoff/h2/` (FORM.TIM mit RE2-Material), FORM.EMD 2449728a wie C-G. Generator `gen/vm_stoff.py`, Aufruf `H_stoff/generator/BAU.txt` (aus `varianten/gen`, liest `G_treu/textur.png`).

**Weg**: Farbkarte = G, je Teil weich (Gauss 1,6 Texel, keine harten Bereichsgrenzen), Zeichnungen an ihrem Ort mit weichem Rand (Deckkraft = Maske Gauss 1,0). Darueber nur Helligkeit (keine neuen Farbtoene, E14):
- **H1 prozedural**: in 4-facher Aufloesung erzeugt (Bindlage je Unter-Texel), Koernung in drei Oktaven, Poren, Knoten (Zellen 75 Einheiten), Falten laengs der Knochenachse; 4 x 4 gemittelt wie ein heruntergerechnetes Bild. Staerke 0,20.
- **H2 STIL-UEBERNAHME aus RE2**: Feinstruktur der Haut- und Fleischtexel aus den RE2-Birkin-Texturen (CDEMD0.EMS 0x31, 0x33, 0x34; rote Wunden und Schwarz ausgenommen), Hochpass Y / Gauss(Y, 3), also ohne deren Zeichnung und Form; je Teil ein Ausschnitt 24 x 24, gespiegelt gekachelt, als Helligkeit auf die Farbkarte (umgefaerbt). Staerke 1,15. Das ist Capcom-Material im neuen Modell und muss so gekennzeichnet bleiben.
- In Zeichnungen halb so stark, leises gemaltes Licht von oben, Z5-Rand und N1/N2 wie G. CLUT drei Zeilen je 255 Farben (k-Mittel).

**Pruefung**: beide IoU 0,9811 / 0,9402, Pruefpunkte 31/31 und 7/7, Kerbe E3 frei, Boden/Verdeckung 5/7 (wie Ausgang) und 9/9, 8/8 Teile wie C.

**Mass "gezeichnet"** (`H_stoff/gezeichnet.json`, Textur selbst, benutzter Bereich):

| | gleiche Nachbarn | harte Spruenge | gerade Kanten /1000 | Toene je 8x8 | Feinanteil Spektrum |
|---|---|---|---|---|---|
| G | 0,550 | 0,028 | 8 | 8 | 0,108 |
| H1 | 0,197 | 0,057 | 39 | 13 | 0,144 |
| **H2** | **0,273** | **0,112** | **40** | **16** | **0,163** |
| RE2 Birkin G2/G3/G4 | 0,30-0,35 | 0,09-0,12 | 34-50 | 17-19 | 0,14-0,19 |
| RE2 Zombie / Hund | 0,35 / 0,42 | 0,09 / 0,15 | 37 / 47 | 17 / 13 | 0,15 / 0,17 |
| RE1.5 Zombie | 0,25 | 0,09 | 45 | 22 | 0,23 |

H2 liegt in allen fuenf Massen im Bereich der Spieltexturen; H1 ist zu unruhig (gleiche Nachbarn 0,20) und hat zu wenige harte Spruenge (0,06). G liegt in allen Massen ausserhalb (flaechig = Cel-Shading).
Feinstruktur im gefilterten Render (Hochpass-Varianz relativ): Bild 1 Screenshot 0,0091, G 0,0052, H1 0,0047, H2 0,0058; Bild 2 Screenshot 0,0048, G 0,0046, H1 0,0038, H2 0,0046. In Bild 1 liegt das Video wegen seines eigenen Korns hoeher als jedes Render.
Farbabstand gefiltert Bild 1 Mittel/Feld: G 3,7/7,5, H1 4,7/8,7, H2 5,0/9,2. Je Stelle (H2, `stellen_H2_G.txt`): Z5 fremd 0,00, Z1 0,00, Z4 0,34 (G 0,46); Rot sauber nur in Z1 und am Schwanzende.

**Was ich sehe** (`H_stoff/vergleich_G_H1_H2.png`, `H_stoff/stil.png`, `H_stoff/stellen_H2_G.png`, `H_stoff/spiel_H_h1.png`, `spiel_H_h2.png`, Zeilen H1/H2 in `uebersicht_bild1.png`): G ist glatt mit flaechigen Bereichen; H1 traegt eine feine, gekoernte Haut mit Falten laengs; H2 eine faserige, organische Haut wie die RE2-Texturen, auf dem Ruecken etwas streifig (Kacheln der Birkin-Fasern). Im Stilblatt steht H2 neben Birkin G2/G4 und dem RE1.5-Zombie wie eine dunkle Gegnertextur derselben Machart; die Form ist dort kleiner, weil der lange Schwanz den Massstab bestimmt. Im Spiel lesen beide als dunkle, strukturierte Haut, die Zeichnung ist durch den weichen Rand etwas leiser als in G.

## 12. I Fuesse (E16: Beine und Fuesse nach den Nutzerteilen, Textur H2)

Nutzer: "Die Fuesse ... sind bei uns klumpige ovale Kugeln, im Screenshot sind das schon richtige saubere Fuesse und Beine", Nachtrag: "Fuesse quasi von unten, man schaut frontal auf die Unterseite". Neue Nutzerteile in `nutzer_teile/` (bein_links ergaenzt auf (198,255) 55x149, fuss_links (192,395), bein_rechts (295,284), fuss_rechts (286,403)), `lage.json` ergaenzt (alte Fassung `lage_vor_E16.json`). `I_fuesse/` (FORM.EMD, FORM.TIM), G-Zwischenstufe `I_fuesse/_g/`, Generator `gen/vm_beine.py` (Formen, Anpassung, Winkel), `gen/vm_form.py` (--beine), `gen/vm_beinblatt.py`.

**Knochen**: unveraendert (32). Der Fuss hatte schon eigene Knochen (25 links, 27 rechts, Zehen haengen daran); Oberschenkel 8/9 bleiben in Form und Pose. Neu gebaut: Unterschenkel 24/26 (ovaler Schaft, Knie, Wade, Knoechel), Fuss 25/27 (kantiger Block: Ferse, Spann, flache Sohle mit Fersen- und Ballenpolster, vorn drei kurze Zehen mit Kralle). Keine Nachpassung je Ring an diesen Teilen.

**Pose 1 (Bild 1)**: Winkel von Unterschenkel und Fuss und die Masse in zwei Stufen an die Masken angepasst (Kamera form_kamera, gefuellte Dreiecke, Powell mit Grenzen): erst das Bein (Ober- und Unterschenkel zusammen gegen bein_*, Knoechel auf die Oberkante der Fussmaske), dann der Fuss mit analytischem Start (Sohlennormale zur Kamera, Zehen im Bild nach unten) gegen fuss_*, Sohle zur Kamera Kosinus 0,76 / 0,75. Wurzelhoehe in Pose 1 wie C (sonst rutschte die Figur 24 px). Der linke Fuss liegt in Pose 1 rund 340 Einheiten unter der Bodenebene: der Hintergrund ist ein Bild, es wird nichts verdeckt, Boden L1/L2 wie im Ausgang; mit Bodenstrafe fiel fuss_links von 0,86 auf 0,65.
**Pose 2 und Ruhe**: Fuss flach (Sohle waagrecht, Gier wie bisher), Knie gebeugt (Bisektion ueber die erste Achse mit Vorzeichenwechsel), bis die Sohle auf dem Boden steht: tiefster Punkt Pose 2 links/rechts 1/1, Ruhe 0/0 Einheiten. Wurzelhoehe Pose 2 wie C.

**Pruefung** (`I_fuesse/pruefung/bericht.txt`, form_pruefung.py erweitert um die vier Teile als Verfeinerung von left_leg/right_leg: eigener Durchgang mit eigener Knochenzuordnung, alte Zahlen unveraendert, gegengeprueft an H2):
- Teile: **bein_links 0,82, fuss_links 0,85, bein_rechts 0,81, fuss_rechts 0,77** (Ziel 0,80; rechts 13 Grad Achse, Zehen 15 px zu tief war v2, jetzt Fuss kuerzer). Alte Teile: left_leg 0,87 (H2 0,66), right_leg 0,79 (0,82), tail 0,87.
- Pruefpunkte Bild 1 31/31, Bild 2 7/7, Kerbe E3 frei, Boden/Verdeckung Bild 1 5/7 (L1, L2 wie Ausgang), Bild 2 9/9.
- **Umriss-IoU Bild 1 0,943 (Ziel >= 0,98 verfehlt)**, Bild 2 0,932. Grund: Die alte Zielmaske (ziel2, aus den groben Schnitten left_leg/right_leg) zaehlt unter und neben dem rechten Fuss Figur, die die neuen Fussschnitte nicht enthalten (1011 px nur im alten Beinschnitt). Gegen ziel2 mit dieser Flaeche als unbestimmt: 0,965 (`I_fuesse/iou_verfeinert.json`, Maske `ziel_bild1_verfeinert.png`). Ein Fuss, der die alte Flaeche deckt (Abdeckung stark gewichtet), kommt auf IoU 0,967, aber fuss_rechts 0,46 und Breite 420: klumpig, genau das, was der Nutzer verwirft. Entscheidung offen: alte Zielmaske in der Fusszone an die neuen Schnitte anpassen (ziele2 neu) oder groberen Fuss.
- Gezeichnet-Mass der Textur (`I_fuesse/gezeichnet.json`): gleich 0,244, hart 0,077, gerade 44, Toene 16, Spektrum 0,135; leicht unter H2 (0,273/0,112/40/16/0,163) und knapp unter dem Bereich der Spieltexturen bei hart und Spektrum, weil auf dem Ruecken jetzt zur Haelfte H1 liegt und die Birkin-Kacheln je Feld zufaellig gedreht sind (gegen die Streifen, Orchestrator).

**Was ich sehe** (`I_fuesse/beine_bild1.png`, `beine_rundum.png`, `vergleich_H2_I.png`, `stil.png`, `spiel_I_fuesse.png`): Statt der Ovale gegliederte Beine: Oberschenkel, schlanker Unterschenkel, kantiger Fuss mit Ferse, Sohle und Zehen. In Bild 1 haengen beide Fuesse mit der Sohle zur Kamera, Zehen unten, wie die Schnitte; in Ruhe stehen die Fuesse flach. Die Sohlen tragen die Farben der Projektion (blaugrau bis tuerkis); die hellen tuerkisen Querbaender des Screenshots sind im gefilterten Bild nur schwach zu sehen (Farbkarte weich, Baender schmal). Offen: IoU-Widerspruch oben, Querbaender kraeftiger, rechter Fuss 0,77.

## 13. J Dicker (E18: Beine und Fuesse so dick wie im Screenshot, Basis I, Textur H2)

Nutzer: "Irgendwie schon ein bisschen duenner als in der Vorlage. In der Vorlage ist es schon etwas dicker alles." `J_dicker/` (FORM.EMD, FORM.TIM), G-Zwischenstufe `J_dicker/_g/`, Generator-Stand `J_dicker/generator/` (BAU.txt). Neu in `gen/`: `vm_breite.py` (Breitenmass), `vm_beine_j.py` (Anpassung J), `vm_nebeneinander.py` (Blaetter); `vm_beine.py` (DICKER, Breitenterm `zu_schmal`, Schluessel `oben` je Seite), `vm_form.py`/`vm_bau.py` (`--dicker`). I baut aus `zw/beine_I.json` weiter bytegleich (FORM.EMD und FORM.TIM der G-Stufe nachgebaut und verglichen).

**Mass** (`vm_breite.py`, `J_dicker/breite.json`): je Teil Breite quer zur Hauptachse der Nutzermaske, Station alle 2 px laengs, Modell (sichtbare Pixel des Teils wie in der Teilepruefung) durch Maske; Kennzahl Median je Teil (Ziel 0,95-1,05), das arithmetische Mittel steht daneben in der Datei, es wird an den Enden (Spitzen, Zehen, schmale Maskenzeilen) von Einzelstationen verzerrt (fuss_rechts Median 1,01, Mittel 1,18). Gemessen an I: schmaler als der Umriss waren bein_rechts 0,95 (49 % der Stationen unter 0,95), fuss_links 0,96, left_arm_front 0,94, alte grobe Schnitte left_leg 0,91 und right_leg 0,77. Rumpf und die uebrigen Teile sind schon so breit oder breiter: face_and_upper_body 1,01, part_of_Body 1,30, right_arm 1,17, right_arm_front 0,96, tail 1,08; daran nichts geaendert.

**Aenderungen**: Unterschenkel massiger (Ringfaktoren Knie 1,08 statt 1,04, Wade 1,22 statt 1,10, Knoechel 1,08 statt 1,05, Ende 0,98 statt 0,95), Gelenke bleiben abgesetzt. Neu angepasst (`vm_beine_j.py`): Start Masse aus I, Knie x 1,12, Knoechel x 1,15, Stufe Bein und Stufe Fuss wie I plus Breitenterm (Gewicht 3, straft nur zu schmal gegen 0,98). Danach je gemessen: Fussbreite / 1,15 (nach der Anpassung Modell/Maske 1,16-1,18, zu breit); links Knie rk x 0,96 und oberster Unterschenkelring 0,94 statt 1,02 (Pruefpunkt b1.nl_y342_links: die Oberkante des Unterschenkels lag 1 px weiter links als in I, Abstand -3,5 bei Grenze 3,2; Wade und Knie-Ring aenderten diese Zeile nicht, gemessen); links Ferse hl 110 und Fusshoehe H 343,4 wie I (Boden BO in Bild 2: die Fersenecke lag eine Port-Zeile tiefer, 6 px im Bodenkasten; Breite und Polster aenderten das nicht, gemessen). Arm: Unterarm und Hand links radial x 1,05 (left_arm_front).
Masse J (I): links rk 177,4 (175,7), ra 133,1 (136,9), W 247,9 (238,5), H 343,4 (343,4), L 366,7 (358,4), hl 110 (109,2); rechts rk 182,1 (150,9), ra 120,4 (93,9), W 242,3 (237,0), H 205,7 (211,6), L 262,7 (265,1), hl 95,7 (82,5).

**Ergebnis** (`J_dicker/pruefung/bericht.txt`):

| Teil | Breite I | Breite J | IoU I | IoU J |
|---|---|---|---|---|
| bein_links | 0,97 | 0,97 | 0,82 | 0,82 |
| fuss_links | 0,96 | 1,00 | 0,85 | 0,89 |
| bein_rechts | 0,95 | 1,00 | 0,81 | 0,80 |
| fuss_rechts | 1,01 | 1,01 | 0,77 | 0,76 |
| left_arm_front | 0,94 | 0,96 | 0,75 | 0,77 |
| left_leg (alt, grob) | 0,91 | 0,91 | 0,87 | 0,89 |
| right_leg (alt, grob) | 0,77 | 0,91 | 0,79 | 0,78 |

Anteil der Stationen unter 0,95: bein_rechts 49 -> 23 %, fuss_links 42 -> 19 %, fuss_rechts 27 -> 8 %. Umriss-IoU Bild 1 gegen ziel2 0,943 -> 0,948, mit grauer Fusszone (`I_fuesse/ziel_bild1_verfeinert.png`) 0,9646 -> 0,9706; Bild 2 0,932 -> 0,935. Pruefpunkte Bild 1 31/31, Bild 2 7/7, Kerbe E3 frei, Gegenproben N1/N2 ok, Boden/Verdeckung Bild 1 5/7 (L1, L2 wie Ausgang), Bild 2 9/9, alle acht alten Teile zugeordnet. left_leg/right_leg bleiben bei 0,91, weil die alten groben Schnitte die Flaeche unter und neben den Fuessen mitzaehlen, die die neuen Schnitte nicht enthalten (Abschnitt 12).

**Was ich sehe** (`J_dicker/beine_bild1.png` J oben, I unten; `beine_rundum_I_J.png`; `vergleich_I_J.png`; `spiel_I_J.png`; `stil.png`): Der rechte Unterschenkel ist deutlich kraeftiger, mit Knieknoten und voller Wade, der rechte Fuss breiter; links ist der Fuss breiter und der Unterschenkel etwas voller, das Knie links bleibt so schmal wie I (Pruefpunkt). Gelenke, kantige Sohle und Zehen bleiben, kein Rueckfall zu Ovalen. In Spielgroesse ist der Unterschied klein (wenige Pixel je Bein). Offen: fuss_rechts 0,76; links liesse sich der Unterschenkel nur gegen den Pruefpunkt b1.nl_y342_links weiter verbreitern.
