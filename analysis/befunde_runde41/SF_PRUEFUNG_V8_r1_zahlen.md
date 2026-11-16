# SF Pruefung V8 Runde 1 - Zahlen-Pruefer (2026-11-09)

Pruefgegenstand: Zweig r41/sichel_v8, HEAD f6b43670a (Commits ce3e24760, 69dfda5bf, e9b5a92d3, f6b43670a gegen 6a25db27b).
Spiel-Asset re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD sha256 fa31156c3b17dd4fe0946f3fad334e597eac91bdc9aa5168280566fd4b33a8ff
(270 500 Byte) = Pin release/em30_hangar.sha256 = scratchpad sfv8/emd/EM30_V8.EMD. Referenz V7 = git show 6a25db27b (sha256 319bbd17).

**Ergebnis: NICHT BESTANDEN.** S2 und S3 sind rot. Das deckt sich mit dem eigenen Riegel des Bauers. S1, S4, S5, S6, S7 und S8 sind
gruen, nachgemessen mit eigenem Code. Kette, Tests und Szene sind reproduziert.

## Verfahren (eigener Code, nichts aus re15_port/tools importiert)
Ordner C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr1/:
- pl.py: eigener EMD-Leser (Verzeichnis, MD1, EMR samt Elternliste, EDD, TIM-CLUT/Pixel, EMS-Blobkette fuer das Original-EM30 Typ 0x30),
  ganzzahlige Pose (RotMatrix-Formeln selbst niedergeschrieben), RID-Kamera 50A0 Cut 12 selbst aus der RDT gelesen, GTE-Projektion,
  Homographie SF_belege/kamera_50A0_12.json, **z-Puffer**-Raster mit Alphatest (der Bauer rastert nach dem Maler-Verfahren).
- linien.py: Nutzerlinien selbst aus Darstellung.png gezogen (Differenz zur Vorlage > 40, 8er-Komponenten, Farbklasse).
- form.py (S1-S4), vorlage.py (Gegencheck der Vorlage-Referenzdaten), s5s7s8.py, s7lit.py (eigenes RDT-Licht/NCCT, BSS-Hintergrund,
  Videoraum-Regel), s6.py.
- Pose PV = Clip 17 Bild 47 Gier 587 x -8246 z -21266 (Messblatt V8 Abschnitt 1).

## Nutzerlinien (eigene Extraktion)
T2 gruen: 848 px, Box x 270-374 y 178-290. T1 rot: 209 px, x 266-272 y 293-335. T verlaeuft von (372,180) bis (270,333) und ist
geglaettet 219,0 px lang. J blau: 323 px, Linie (379,189) bis (357,241), dazu ein Zeiger mit 141 px. H hellblau: 192 px, (420,190) bis
(445,206). K weiss: 202 px, (454,210) bis (463,244). Das stimmt mit Messblatt V8 Abschnitt 2 ueberein, Abweichung <= 1 px.

Gegencheck der Vorlage-Daten (vorlage.py, eigene dE-Differenz Vorlage gegen das per Homographie verzerrte BSS ROOM50A12): Die Zielkanten
m27 (Messblatt) treffen an 15 von 22 Stationen die eigene Kante auf <= 1 px. Ausreisser sind s30, s80 und s100 (dunkle Luecke) sowie die
3 T+11-Stationen. Die Oberkontur m13b x 345-440 trifft die eigene auf <= 1 px, bei x 445 auf 4 px. Die Referenzdaten sind brauchbar.

## Soll/Ist

| Punkt | Soll | V8 (eigen) | V7 (eigen, Gegenprobe) | Bewertung |
|---|---|---|---|---|
| S1 Kontur T | Mittel <= 4, Max <= 8 | **3,05 / 7,0** | 16,77 / 50 | GRUEN (V7 reisst) |
| S2a Oberkontur x 330-385 | <= 4 / <= 6 | **3,70 / 7** (Vorzeichen -3,48: Modell liegt ueber der Vorlage) | 7,30 / 13 | **ROT** (Max 7 > 6) |
| S2b Sattel x 300-420 | <= 2 | **3,7 bei x 358** | 6,6 bei x 355 | **ROT** |
| S2c Naht | <= 3 | **5,0 bei x 404** (Naht x 391) | 7,0 bei x 366 | **ROT** |
| S3a Kuppe | <= 4 | 3,0 (x 404) | 8,0 (x 406) | gruen |
| S3b Huegel H | 5-10 bei x 428-445 | **3,0 bei x 423** | 4,6 bei x 427 | **ROT** |
| S3c weitere Erhebung | keine | keine | keine | gruen |
| S3d Sattel Ruecken -> Kopf | <= 2 | **2,3 bei x 391** | 6,4 bei x 438 | **ROT** |
| S3e K | <= 3 | 3,0 | 3,0 | gruen (genau an der Schwelle) |
| S4 Breite | Wurzel >= 55; s 90-130 >= 36; monoton +-2; Ende <= 10 | 59,0; 42,6/40,0/40,3; ja; 6,0 | 44,3; 34,3/24,1/28,1; nein; 8,1 | GRUEN (V7 reisst) |
| S5 Gesicht | bytegleich V7 | Mesh 9 4280 Byte gleich, Zeile 486 gleich, 6379 Mesh-9-Texel gleich, relpos/Eltern/alle Keyframe-Winkel von Knochen 9 gleich | - | GRUEN |
| S6 Boden Keyframes | 0 von 912 | Spitze/Dorn 0, jede Schwanzecke 0 (Abstand mind. 19 bzw. 12 E.) | 70 (Spitze) / 82 (jede Ecke), bis 579 E. darunter | GRUEN (V7 reisst) |
| S6 Boden Szene | alle Takte y <= 0 | 144 Takte, tiefstes Gelenk y -66 (Gelenk 7, h32), eigener Lauf | (-108) | GRUEN |
| S7a Herkunft | Original-Texel, Original-Eintraege | Zeile 493: 62 und Zeile 494: 50 Eintraege anders als V7, alle = Original-Ader-Eintrag (a* >= 15, L* <= 38), 0 fremd, kein Original-Ader-Eintrag ausgelassen; 39 Flaechen (Mesh 13: 13, Mesh 8: 26), 6704 Texel = Original-Texel an derselben Stelle und im Nutzungsbereich derselben Original-Zeile 480/481; 2129 rote Texel, 0 davon nicht original; Texel V8 = V7 bytegleich (nichts gemalt); 26 UV-Schuebe starr (je Flaeche ein du/dv), Seite gleich, 9 Schubgruppen | - | GRUEN |
| S7b Oberarm 13 | 35-59 % | 43,5 % (Bauer 43,3) | 22,9 % | GRUEN |
| S7c Rumpf 8 | 25-80 % | lok x < 0: 29,7 %, lok z < 0 ((W)-rechts): 32,6 % | 0,0 % / 0,2 % | GRUEN (knapp ueber 25) |
| S7d Ruhe | Flecken <= Original | 18 (Bauer 19) <= Original-EM30 27 | 15 | GRUEN |
| S8 Technik | <= 16 Zeilen, <= 4096, gueltig, 23/23/21 | 15, 3840, Felder 480-494 gueltig, 23/23/21 | 13, 3328 | GRUEN |

Die eigenen Werte decken sich mit riegel_v8 bis auf Rasterdetails: S2c bei x 404 statt 406, S5 mit 6379 statt 6501 Texeln,
S7d mit 18 statt 19 Flecken.
Zusatz: Entlang der Linie J wechseln 26 % der Querfenster (+-4 px) zwischen Schwanz- und Rumpfknochen, bei V7 67 %. Die Modell-Oberkontur
liegt im Mittel 1,6 px unter der H-Linie (Spanne -5 bis +7). Die Kopf-Vorderkante liegt im Mittel 1,1 px vor der K-Linie.
S2c ueber die ganze Spanne x 330-420 statt Naht +-15: V8 5 px bei x 407, V7 33 px bei x 408. Die Praezisierung des Bauers aendert
das Urteil also nicht.

## Seiten
"Die rechte Seite" ist (W)-rechts, also z < 0 im Knochenraum 8. Herleitung: Bei x = vorn und y = unten gilt rechts = unten x vorn =
(0,0,-1). Knochen 13 hat relpos z -1136. In PV sind 3642 px von Knochen 13 sichtbar und 235 px von Knochen 10. Die Flanke (W)-rechts
ist also die zur Kamera gewandte. Die Riegel-Definition S7c (lok x < 0) misst die hintere Rumpfhaelfte, nicht die Seite. Der Bauer
hat das offengelegt. Beide Werte liegen im Band.

## Reproduktion
- v8_kette.sh, zweiter Lauf in pr1/kette: "V8-KETTE-OK sha256 fa31156c...". Alle Stufen-Summen (V4ot3 a1bc9309, V5r4 38562ced,
  V5k 99703bbd, V5f 784c8f05, V6 9e0e58ef, V7 319bbd17, V8 fa31156c) stimmen. Pin = Asset = Neubau, bytegleich.
- Bau: "ninja: no work to do", "=== LOCAL-BUILD-OK (build)" (pr1/build.txt).
- ctest -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole" --timeout 900:
  "100% tests passed, 0 tests failed out of 8" (310 s, pr1/ctest_gruppe.txt).
- ctest -j 6 --timeout 600 -R "^unit_" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)":
  "100% tests passed, 0 tests failed out of 1013" (pr1/ctest_unit.txt).
- Eigener Szenenlauf: lauf_v4.sh mit BAUM = r41_sichel_v8, EMD "-", FD 1100, Ende 1110 (pr1/szene). Ergebnis: EXIT 0, Log
  "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD", "23 meshes, 23 bones, 21 clips". state.log hat 1131 Zeilen und 0 Unterschiede zum Lauf des
  Bauers; dieser hat 0 Unterschiede zum freigegebenen V7-Lauf sfv4/r/V7_szene. Alle 144 "Schwanz B y"-Zeilen sind identisch,
  FRAMEDUMP F1100 ist bytegleich. Der Lauf des Bauers ist damit der Stand von Asset und exe.
- Labor 50E0: nur die Logs des Bauers gelesen. state.log labor_v8 gegen labor_v7: 0 Unterschiede (1251 Zeilen), Form geladen.
- Nicht nachgeprueft: die Gegenprobe "V7 als Spiel-Asset -> test_birkin_formen und test_schwanz_r41 rot". Sie wuerde verlangen, das
  Asset im Baum zu tauschen.

## Code-Diff gegen 6a25db27b
- Neue Zahlen: SW_V8_VERH 1,10, SW_V8_GLIED_MIN 500, SW_V8_GLATT_MAX 0,4363 (25 Grad). Jede ist als GESTALTUNG/PORT-WAHL kommentiert
  und mit Messblatt V8 6a, Nutzer-Wortlaut und Messwerten belegt (V8 1,00 gegen V4/V7 5,5; 538 gegen Referenz 2 466/467; Bauplan
  "je hoechstens 25 Grad"). Hinzugefuegte "@0x"-Stellen: 2, beide im Text "keine Zahl hier ist eine @0x-Konstante". Es gibt keine
  erfundene Adresse.
- Wirkungsbereich: re15_schwanz_r41_v8_kette verlangt kette_ok (23 Knochen, Eltern 8 -> 16 -> ... -> 22). Im EMD-Ordner hat nur
  EM30_HANGAR.EMD diese Kette, EM29_ROT hat 18 Knochen und das Original-EM30 16. Aufruf main.c:10378 fuer alle NPCs, aber ohne Kette
  ist der Effekt 0. Ausserhalb von Hangar/Labor ist nichts betroffen.
- Die Ergaenzung in bf_schwanz_v2 (!v8) ist wirkungslos: V8 hat Knochen-16-relpos x -667 und ist damit nicht < BF_V2_WURZEL_X -700.
  Harmlos.
- Asset gegen V7: Meshes 0-7, 9-12, 14 und 15 bytegleich; Clips/EDD gleich; Keyframes nur bei Knochen 16-22 geaendert; CLUT-Zeilen
  480-492 und alle Texel bytegleich. Hand (V7), Beine und Kopf bleiben unveraendert. Mesh 8: Ecken 7, 29, 30, 45, 47, 49, 50, 53 und
  58 sind um 13-74 E. verschoben, wie im Dossier angegeben; Flaechen gleich, **Normalen nicht nachgezogen** (Hinweis, Licht).
- Tests: Die V7-Pruefungen in test_birkin_formen.c und test_schwanz_r41.c sind durch V8-Pruefungen ersetzt, nicht ersatzlos gestrichen.

## Abweichungen vom Bauplan ausserhalb der Riegel (gelb)
1. **Umkehr in der Ruhekette.** Bauplan 6a verlangt "ohne Umkehr". In PV gibt es eine Umkehr am Gelenk 20: Die Bild-Drehwinkel der
   Kette sind -22,2 / -8,8 / -25,9 / **+8,3** / -20,4 / -15,0 Grad, und die 3D-Biegeebene kehrt am Gelenk 20 das Vorzeichen um.
   3D-Gelenkwinkel: 23,1 / 23,8 / 20,9 / 11,8 / 16,9 / 19,0.
2. **Keyframes ueber 25 Grad.** Im Asset haben 714 von 912 Keyframes ein Gelenk ueber 25 Grad und 305 eines ueber 30 Grad. Das
   Maximum je Keyframe liegt im Median bei 28,3 Grad, bei P90 34,2 Grad und hoechstens bei 95,5 Grad. In der Szene begrenzt glaetten()
   das: hoechstens 25,2 Grad, Median des Takt-Maximums 25,0. Die Kette liegt also in fast jedem Takt an der Grenze. In 134 von 144
   Takten wechselt die Biegeebene das Vorzeichen. Das ist ein Hinweis auf eine S-Form, kein Sichtbeweis; die Sicht-Pruefung soll es
   ansehen. Das passt zum Dossier, Offen 3 und 4.
3. **S7c knapp.** 29,7 bzw. 32,6 % bei einer Untergrenze von 25 %. Die Vorlage zeigt 76,3 %. Die Palette der Zeilen 493/494 ist
   gemischt: V7-Grau plus 62/50 Original-Ader-Eintraege, so im Messblatt 6c geplant. Sie ist nicht die vollstaendige Original-Zeile.

## Fazit
Rot: S2 (a Max 7 > 6, b Sattel 3,7 > 2, c Naht 5,0 > 3) und S3 (b Huegel 3,0 < 5 und bei x 423 ausserhalb x 428-445, d Sattel 2,3 > 2).
Gruen und belastbar: S1, S4, S5, S6, S7 und S8, dazu die Herkunft der Adern (keine gemalten Texel, nur Original-Texel und
Original-Eintraege), Reproduzierbarkeit, Tests und Szene.
**bestanden = nein.**
