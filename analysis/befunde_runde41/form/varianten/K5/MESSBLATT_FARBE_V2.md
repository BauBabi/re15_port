# Messblatt Farbe und Struktur V2: Nachtrag "Binnenzeichnung" (Riesenform K5c, Pruefer, 2026-11-06)

Nur Nachtrag. **MESSBLATT_FARBE_V1.md gilt weiter** (A1-A12, K1-K10, N1-N2, B1-B9, Darstellungsregel 7). Hier geaendert wird nur, was ausdruecklich als
"ersetzt V1" markiert ist (K1/K2-Bezug, B2-Rechenweg, B9-Gegenprobe). Anlass: Versuch 2 (der letzte, Nutzer 2026-11-06), Pruefung r3 (Sicht P1 "Fleisch und Haut
sind Farbflaechen ohne gemalte Binnenzeichnung"; Zahlen ROT K1, K2, K4, B2, B9; Konstanten ohne Quelle).

Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL, NUTZER_ENTSCHEIDE E5/E19/E20). Jede Grenze unten traegt ihre Quelle: gemessenes Spielmodell (Datei, Zone, Zahl),
gemessene Screenshot-Zone oder V1-Zeile. Seiten (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, OpenCV float (RGB/255), wie V1.

Werkzeug (versioniert, `analysis/befunde_runde41/form/varianten/K5/farbe_lesung/N2_skripte/`):
- `n2_mass.py` - Masse (a)-(f) an Atlas und Render, Definitionen im Kopf der Datei; schreibt `farbe_lesung/N2_messung.json` und die Belegbilder `N2_atlas_*.png`, `N2_render_seite_*.png`.
- `n2_riegel.py <Variantenordner>` - **Riegel B10-B17**, GRUEN/ROT, Exit 1 bei ROT; `--familie` = EM30, G4, G5, r3, K4 (`N2_riegel_familie.json`).
- `n2_linienbild.py` - Belegbilder `N2_linien_*.png` (gezaehlte dunkle Linien cyan, Rotflaechen-Umriss gelb).
- `n2_empfindlich.py` - Empfindlichkeit der Riegel gegen die Messkonstanten (`N2_empfindlich.json`).
- `n2_bild1.py`, `n2_bild1_v7.py`, `n2_bild1_v8.py` - Ursache Bild-1-Rot, Rundlauf, Ersatzversuche (`N2_bild1*.json`).

Modelle wie V1 Abschnitt 1: EM30 = `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`, Zone Haut (ohne Meshes 0, 1, 3, 4, 6 = Jeans/Schuhe, ohne Kopf 9); RE2-G4 = `re15_port/shared_assets/RE2/CDEMD0.EMS`
Typ 0x34, Koerper ohne Kopf 26, Krallen, Schulterauge; RE2-G5 = Typ 0x36 ohne Maul/Augen; **r3** = `varianten/K5/farbe/K5c_farbe/FORM.TIM` (sha256 7c719e12..., = `farbe/K5C_FARBE.TIM`),
**K4 heute** = `varianten/K5/K4_heute/FORM.TIM`; K5c-Zonen: Koerper Atlas x < 384, Kopf (384,0,128,104) (`K_alligator/generator/k_form.py:11`). Render: `B_skripte/render.blick`
(Licht 0,55 + 0,45 max(0, N.L), weiss, Umkugel 0,46, 380 px, 3 Winkel 3/4 vorn / Seite / 3/4 hinten, roh; Familie Clip 0, K5c Clip 2 Ruhe) = Licht von Tafel 03.

## 1. Binnenzeichnung der Familie (Zahlen)

Messdefinitionen (`n2_mass.py`): Atlas = jeder genutzte Texel einmal; gezaehlt nur, wo die 3x3-Nachbarschaft ganz genutzt ist und zum selben Mesh gehoert (keine Insel-/Meshnaehte);
fuer Gradienten werden ungenutzte Texel mit dem naechsten genutzten gefuellt. Rotklasse = V1-A3 (a\* > 15, C\* > 20, Farbton -40..70 Grad). Rotflaeche = 8-Komponente >= 20 Texel.
Fleischzone = Rotklasse + 2 Texel Rand; Hautzone = Rest der Zone.
- (a) Canny auf L\* (Sobel 3, L2), Schwellen 61/122 = Stufe 6/12 L\* (Sobel-3-Antwort 4 d; 6/12 = Schrittklassen Sicht-Pruefer r3 2.1), Kantentexel je 100 Zonentexel; "lang" = Kantentexel
  in Komponenten >= 6 Texel. Zusatz **dunkle Linien** (Adern/Falten): Black-Top-Hat 5x5 auf L\* >= 6, Komponente >= 6 Texel, Laenge 4 sqrt(l1) >= 6, Elongation sqrt(l1/l2) >= 3.
- (b) Feldkohaerenz R = |Sum m e^(2i theta)| / Sum m an den Kantentexeln je Atlasfeld (Mesh-Komponente >= 200 Texel), kantengewichtet; lokal = Strukturtensor (Gauss 1,5 Texel),
  (l1-l2)/(l1+l2) an den Kanten; Hauptrichtung der Kanten gegen die Laengsachse des Felds (PCA, nur Felder mit Achsverhaeltnis >= 1,5).
- (c) Aussenrand 2 Texel: dL_rand = L\*(Flaeche) - L\*(Ring); "dunkler Rand" bei dL_rand >= 6. (d) k-Mittel k = 6 je Rotflaeche, Zentren mit dE76 < 10 verkettet (V1 8, Leser-B-Kette),
  gezaehlt Gruppen > 10 %. (e) L\*/a\*-Streuung in den Rotflaechen, gepoolt (Abzug Flaechenmittel). (f) Hell-Kern: Kern = Erosion 2 Texel, L\*(Kern) - L\*(Innenrand) >= 6.
- Render: dieselben Masse auf den Bildpunkten (gueltig = Erosion 3x3 der Figur); fuer die Kohaerenz **flaechengleich** (Kachel so gross, dass die Figur im Mittel 30 000 px je Winkel deckt:
  EM30 408 px, G4 344, G5 425, K5c 521), weil das Mass mit der Bildgroesse steigt (Gegenprobe: r3 bei 260/380/540 px 0,48/0,51/0,56).

| Mass (Koerper bzw. Haut) | EM30 Haut | RE2-G4 | RE2-G5 | **Familie** | **r3** | K4 heute |
|---|---|---|---|---|---|---|
| Rotanteil Atlas % | 18,0 | 23,9 | 38,0 | 18-38 | 20,3 | 0,5 |
| (a) Canny-Dichte Zone / Fleisch / Haut | 28,0 / 29,6 / 26,2 | 20,9 / 29,6 / 12,7 | 27,4 / 30,2 / 17,9 | 20,9-28,0 / **29,6-30,2** / 12,7-26,2 | 22,3 / **19,1** / 23,8 | 20,5 / 33,1 / 20,2 |
| (a) dasselbe, Schwellen 4/8 L\* | 32,5 / 33,9 / 31,1 | 26,9 / 33,7 / 20,6 | 30,2 / 32,6 / 22,3 | - / 32,6-33,9 / - | 30,6 / 29,3 / 31,2 | 30,1 / 35,1 / 30,0 |
| (a) lange Kanten (>= 6 Texel) % | 93,0 | 95,6 | 95,9 | 93-96 | 94,6 | 93,4 |
| (a) **dunkle Linien % Fleisch / Haut** | 12,65 / 5,35 | 9,50 / 3,89 | 19,32 / 8,29 | **9,5-19,3 / 3,9-8,3** | **3,71 / 2,99** | 1,32 / 5,30 |
| Linien je 1000 Texel Fleisch / Haut | 1,52 / 2,37 | 3,01 / 2,22 | 3,45 / 3,50 | 1,5-3,5 / 2,2-3,5 | 1,84 / 1,43 | 1,55 / 1,45 |
| Linienbreite Fleisch / Haut (Texel, Median) | 1,62 / 1,36 | 1,50 / 1,34 | 1,79 / 1,53 | 1,5-1,8 / 1,3-1,5 | 1,31 / 1,45 | 0,97 / 1,68 |
| Linienlaenge Fleisch Median / p90 (Texel) | 12,6 / 26,5 | 10,8 / 21,2 | 15,6 / 42,9 | 11-16 / 21-43 | 8,4 / 24,4 | 8,7 / 8,8 |
| Linienkontrast (Top-Hat L\*) Fleisch / Haut | 15,2 / 10,6 | 11,2 / 9,2 | 12,1 / 11,5 | 11-15 / 9-12 | 9,0 / 10,0 | 9,0 / 9,9 |
| (b) Feldkohaerenz R (kantengewichtet) | 0,214 | 0,370 | 0,513 | 0,21-0,51 | 0,229 | 0,201 |
| (b) lokale Kohaerenz Atlas | 0,576 | 0,618 | 0,753 | **0,576-0,753** | **0,561** | 0,554 |
| (b) lokale Kohaerenz Render flaechengleich (3/4 vorn, Seite, 3/4 hinten) | 0,601/0,623/0,640 | 0,618/0,628/0,603 | 0,674/0,690/0,677 | **0,601-0,690** | **0,569/0,554/0,559** | 0,543/0,551/0,550 |
| (b) Kanten zur Feldlaengsachse: Median Grad / Anteil < 30 Grad | 31,1 / 72 % | 3,7 / 98 % | 45,7 / 59 % | 4-46 / 59-98 % | 9,2 / 82 % | 26,6 / 69 % |
| (c) dL_rand Median; Flaechen mit dunklem Rand (Anzahl) | -0,2; 10,5 % | 11,8; 73,7 % | 5,2; 42,5 % | -0,2..11,8; 10-74 % | -1,8; 25,0 % | 9,2; 80 % |
| (c) Render: Flaechen mit dunklem Rand | 8,6 % | 51,4 % | 9,8 % | 9-51 % | 28,6 % | 50 % |
| (d) Atlas: Rottexel in Flaechen mit >= 2 Stufen | 54,3 % | 88,5 % | 96,7 % | 54-97 % | 85,7 % | 100 % |
| (d) **Render: Rotpixel in Flaechen mit >= 2 Stufen** | 59,6 % | 81,1 % | 66,0 % | **60-81 %** | **2,1 %** | 94,7 % |
| (e) Atlas L\*- / a\*-Streuung gepoolt | 10,9 / 5,4 | 11,2 / 11,4 | 14,5 / 5,7 | **10,9-14,5** / 5,4-11,4 | **8,6** / 5,8 | 11,4 / 9,9 |
| (e) **Render L\*- / a\*-Streuung gepoolt** | 9,6 / 5,0 | 11,0 / 7,9 | 11,2 / 8,7 | **9,6-11,2 / 5,0-8,7** | **5,4 / 3,1** | 9,1 / 10,0 |
| (e) Render Rotklasse gesamt L\* sd / a\* sd je Winkel | 11,7-13,0 / 6,1-6,3 | 12,8-13,6 / 6,9-11,3 | 13,2-15,4 / 6,8-15,0 | 11,7-15,4 / 6,1-15,0 | 8,4-9,3 / 2,9-3,7 | - |
| (f) Hell-Kern (Flaechen mit Kern), Atlas / Render | 0 % (5) / 0 % | 50 % (18) / 4,8 % | 50 % (10) / 11,8 % | 0-50 % / 0-12 % | 46 % (13) / 0 % | - / 0 % |
| Kopf: Canny-Dichte / dunkle Linien % | 29,6 / 5,39 | 30,1 / 46,45 | - | 29,6-30,1 / 5,4-46 | **8,85 / 1,28** | 4,79 / 2,16 |

Lesart (gemessen, nicht gesehen):
1. **Die Familie zeichnet Linien in das Fleisch, r3 nicht**: dunkle Linien im Fleisch 9,5-19,3 % (r3 3,7 %), Kontrast 11-15 L\* (r3 9,0), Laenge Median 11-16 Texel (r3 8,4);
   Canny-Dichte im Fleisch 29,6-30,2 (r3 19,1). In der Haut liegt r3 bei der Canny-Dichte in der Familie (23,8), die Kanten dort sind aber Wolkenraender: lokale Kohaerenz
   0,561 unter 0,576 (Atlas) bzw. 0,554-0,569 unter 0,601 (Render, flaechengleich). Belege: `farbe_lesung/N2_linien_G4.png`, `N2_linien_EM30.png`, `N2_linien_r3.png`
   (cyan = gezaehlte Linien), `N2_atlas_*.png` (Mitte Kanten weiss, rechts Richtungsfeld: Farbton = Kantenrichtung, Helligkeit = Kohaerenz).
2. **Im Render ist das r3-Rot einstufig und flach**: Rotpixel in mehrstufigen Flaechen 2,1 % (Familie 60-81 %), L\*-Streuung 5,4 (9,6-11,2), a\*-Streuung 3,1 (5,0-8,7). Im Atlas
   ist r3 dagegen mehrstufig (85,7 %) - die Stufen sind weiche Verlaeufe, die unter Gouraud-Licht ineinanderlaufen; die Familie hat harte Farbspruenge, die das Licht ueberstehen
   (Lesart; gemessen ist nur Atlas 85,7 % gegen Render 2,1 %). Dimmen allein ist es nicht: bei Licht 0,775 bleiben 75 % der r3-Rottexel rot, EM30 69 %, G4 82 %, G5 57 %.
3. **Kopf ohne Zeichnung**: Canny-Dichte 8,85 gegen 29,6-30,1, dunkle Linien 1,3 % gegen 5,4-46 % (Sicht-Pruefer r3 P11 bestaetigt).
4. **Trennt nicht** (kein Riegel): Feldkohaerenz R (r3 0,229 in 0,21-0,51), Richtung zur Feldlaengsachse (r3 laeuft schon laengs, 82 % < 30 Grad), lange Kanten,
   Wundrand (c) und Hell-Kern (f): EM30 malt Rot dunkler als die rosa Haut (dunkler Rand 10,5 %, Hell-Kern 0 %), G4/G5 umgekehrt (74/43 %, 50/50 %). Diese beiden sind daher
   **Leitwerte aus G4** (V1: G4 = Vorbild der Riesenform), keine Riegel.
5. Empfindlichkeit (`N2_empfindlich.json`): Fleisch-Linien bei Top-Hat-Schwelle 4/6/8 L\* Familie 10,3-15,6 / 9,5-19,3 / 5,9-17,8, r3 4,8 / 3,7 / 1,7 (robust); Haut-Linien
   bei 8 L\*: Familie 2,2-5,3, r3 3,3 (dort **nicht** getrennt); Kopf-Linien bei 4 L\*: EM30 1,9 < r3 4,5 (nicht robust, deshalb B12 mit Canny-Dichte); Canny Fleisch bei 4/8:
   r3 0,9-fach Familie (schwach). Render-Rotmasse bei Mindestflaeche 10/20/40: Familie sdL 9,5-11,3, sda 4,9-8,8, Stufen 58-83 %; r3 5,3-5,5 / 3,1 / 1,3-2,7 (robust).
   Die Messkonstanten sind deshalb fest (Schwelle 6, Mindestflaeche 20); B11 und B13 sind die schwachen Riegel.

## 2. Riegel B10-B17 (neu)

Messung: `python analysis/befunde_runde41/form/varianten/K5/farbe_lesung/N2_skripte/n2_riegel.py <Variantenordner>`. Toleranz wie V1 6 (K1/K2: unten 0,85-fach, oben 1,2-fach der
Familienspanne); B17 ohne Faktor (normiertes Mass, Luecke Familie/r3 nur 0,03-0,05). Gegenprobe `--familie` gelaufen: **EM30 8/8, G4 8/8, G5 7/7 (kein Kopffeld) GRUEN; r3 0/8 GRUEN;
K4 heute 5/8** (B10, B12, B17 ROT).

| # | Mass | Vorgabe (Familienspanne) | Toleranz / Grenze | Messmethode Atlas | Messmethode Render (gleiches Licht wie Tafel 03) | EM30 / G4 / G5 | **r3** |
|---|---|---|---|---|---|---|---|
| B10 | dunkle Linien im Fleisch % | 9,5-19,3 | 8,1-23,2 | Top-Hat 5x5 >= 6 L\*, Linie >= 6 Texel lang, Elongation >= 3, je gueltige Fleischtexel | kein Riegel: Geometriefalten zaehlen mit (Familie 2,6-11,2 %, r3 3,6-5,6 %) | 12,65 / 9,50 / 19,32 GRUEN | **3,71 ROT** |
| B11 | dunkle Linien in der Haut % | 3,9-8,3 | 3,3-9,9 | wie B10, Hautzone | kein Riegel (wie B10) | 5,35 / 3,89 / 8,29 GRUEN | **2,99 ROT** (schwach, 1.5) |
| B12 | Kopf: Canny-Dichte und dunkle Linien | 29,6-30,1 / 5,4-46 | Canny >= 25,2 und Linien >= 4,6 % | Kopffeld (K5c (384,0,128,104), EM30 Mesh 9, G4 Mesh 26) | Sichtkontrolle Tafel 06 | 29,6+5,39 / 30,1+46,45 / - GRUEN | **8,85 / 1,28 ROT** |
| B13 | Canny-Dichte im Fleisch (6/12) | 29,6-30,2 | 25,1-36,2 | (a), Fleischzone | - | 29,63 / 29,55 / 30,17 GRUEN | **19,05 ROT** (schwach, 1.5) |
| B14 | L\*-Streuung in den Rotflaechen, gepoolt | Render 9,6-11,2; Atlas 10,9-14,5 | Render >= 8,2 und Atlas >= 9,3 | (e) Atlas | (e) Render, 3 Winkel zusammen | 9,6+10,9 / 11,0+11,2 / 11,2+14,5 GRUEN | **5,4 + 8,6 ROT** |
| B15 | a\*-Streuung in den Rotflaechen, gepoolt | 5,0-8,7 | Render >= 4,25 | Atlas nur Kontrolle (trennt nicht: r3 5,8) | (e) Render | 5,0 / 7,9 / 8,7 GRUEN | **3,1 ROT** |
| B16 | Rotpixel in Rotflaechen mit >= 2 Rotstufen | 59,6-81,1 % | Render >= 50,7 % | Atlas nur Kontrolle (trennt nicht: r3 85,7 %) | (d) Render | 59,6 / 81,1 / 66,0 GRUEN | **2,1 ROT** |
| B17 | lokale Kohaerenz der Kanten (Linie statt Wolke) | Render 0,616-0,680 (Mittel), Einzelwinkel ab 0,601; Atlas 0,576-0,753 | Render-Mittel >= 0,60 und Atlas >= 0,576 | (b) lokal, kantengewichtet je Feld | (b) Render flaechengleich 30 000 px, Mittel 3 Winkel | 0,621+0,576 / 0,616+0,618 / 0,680+0,753 GRUEN | **0,561 + 0,561 ROT** |

Zusatzbedingung "kein Zellnetz, kein Rauschen" (keine eigene Zahl noetig): ein Netz verbundener Linien ist eine Komponente ohne Elongation und faellt aus B10/B11 heraus; Rauschen
sperren weiter V1-A6/A7 (HF je Feld), A12 (Farbkorn), B6 (Wiederholung). Die Obergrenzen von B10/B11/B13 (1,2 x Familienmaximum) sperren Ueberzeichnung.

## 3. Die fuenf roten Riegel aus r3: Ursache und Zielwerte

### 3.1 Bild 1, Ruecken/Flanke: warum 2,3 % statt 11,9 % Rot (`n2_bild1.py`, `N2_bild1.json`)

| Messung | Original | r3 | Befund |
|---|---|---|---|
| Rotklasse a\* >= 12, C\* >= 15, L\* >= 20 | 11,9 % | 2,3 % | - |
| Schwellen verschoben (a\* 10/14; C\* 13/17; L\* 18/22; ohne L\*) | 12,9/10,9; 13,6/9,9; 13,0/11,1; 15,7 | 2,3/2,2; 4,2/0,7; 2,6/1,9; 3,0 | Abstand bleibt 3-14-fach: **nicht die Schwelle** |
| uint8- statt float-Lab | 12,4 % | 2,3 % | **nicht die Quantisierung** (gilt auch fuer die Figur: 5,56/2,86 statt 5,25/2,75) |
| farbig, aber L\* < 20 / hell (L\* >= 20), aber unbunt | 3,8 / 32,5 % | 0,7 / 44,4 % | nicht "zu dunkel": r3 ist im Hellen **unbunt** |
| a\* p10/50/90; C\* p10/50/90 | -2,4/4,4/17,5; 3,5/9,2/19,6 | 0,1/4,0/11,1; 2,8/5,5/11,9 | r3 fehlt das obere Zehntel an a\*/C\* |
| Lab der Rotpixel; L\* p10/50/90 | (38,1, 19,6, -4,1); 22,6/36,7/54,7 | (28,1, 16,0, -3,7); 21,0/26,9/37,0 | Original-Rot ist **hell** (Median L\* 37) |
| r3-Albedo im Ruecken: Rotklasse / L\* p10/50/90 | - | 8,7 % / 5,7/17,2/41,5; Kette 2,3 % | die Kette laesst von r3s dunklem Albedo-Rot ein Viertel uebrig |
| **Rundlauf**: Original-T x Licht -> `video_filter` -> Umkehr | Ruecken 11,9 -> 10,0 %; Figur Rot 5,25 -> 4,02 %, C\* 9,25 -> 7,85, ab 9,83 -> 8,03 | - | die Kette selbst kostet 16 % Rot, 15 % C\*, 18 % ab-Streuung |

Licht-Rueckrechnung: die Lichtfaktoren (`farbe_palette.json` Bild 1 (0,487, 0,432, 0,374)) wirken auf Original und Kandidat gleich; die Neubeleuchtungsprobe (Lichtkarte aus denselben Dreiecken
mit weisser Textur, `n2_bild1_v7.lichtkarte`) gibt das Spielbild bis +-2 Stufen wieder (9,6 % der Bildpunkte). **Ursache von K4 ist die Textur** (zu wenig und zu dunkles Rot an
den in Bild 1 sichtbaren Ruecken-Texeln), nicht Schwelle, Quantisierung oder Lichtfaktor. **Ursache von K1/K2 ist zum Teil die Kette**: das Original selbst kaeme nach einem
Durchlauf auf C\* 7,85 (V1-Grenze 7,87) und ab 8,03 (V1-Grenze 8,36).

Ersatzversuche (`n2_bild1_v7.py` V6/V7, `n2_bild1_v8.py` V8; Albedo im 2D-Bild ersetzt, exakt neu beleuchtet, durch die echte Kette):

| Versuch | Ruecken-Rot | Figur Rot | Figur C\* | Figur ab |
|---|---|---|---|---|
| V6: 10 / 15 / 20 % der Ruecken-Pixel (hoechstes Albedo-a\*) -> Zielrot L\* 35 (a\* 26,5, b\* 18,0) | 2,4 / 5,4 / 12,0 | 2,76 / 3,50 / 5,10 | 6,3-6,7 | 6,4-7,3 |
| V6: dasselbe mit L\* 45 | 5,4 / **12,7** / 19,0 | 3,48 / **5,27** / 6,81 | 6,3-6,8 | 6,5-7,2 |
| V6: dasselbe mit L\* 55 | 6,7 / 12,4 / 17,9 | 3,81 / 5,20 / 6,54 | 6,4-6,8 | 6,6-7,1 |
| V7: V6 (L\* 45, 15 %) + Hautbuntheit warm x 1,5 / x 2,0 | 18,8 / 22,9 | 7,92 / 9,59 | 7,60 / 8,74 | 8,04 / 9,36 |
| V7: V6 + a\*b\* x 1,5 (auch kuehl) | 19,1 | 8,04 | 8,63 | 8,44 (Albedo-Blauklasse 45 %) |
| **V8: V6 + a\*b\* x 1,5, gekappt a\* <= 11 und kuehl C\* <= 8 (B3)** | **13,5** | **5,21** | **7,36** | **7,16** |
| V8 mit x 2 / x 3 / x 4 | 12,4 / 11,2 / 10,9 | 4,90 / 4,07 / 3,98 | 7,33 / 7,10 / 7,10 | 7,15 / 6,91 / 6,91 |

Ergebnis: K4 ist mit hellem Fleischrot im Ruecken erreichbar (V6 L\* 45-55, 15 %). K1/K2 in V1-Form sind unter K4 und B3 **nicht** erreichbar: warme Buntheit treibt die Rotklasse
ueber 7,87 %, bevor C\* 7,87 erreicht ist (V7); die fehlende ab-Streuung des Originals sitzt in den Beinen (Hinterbein r (W) T b\* -7,9, C\* 10,4) und waere Blau-Klasse,
die B3 verbietet; gekappt bleibt das Maximum bei C\* 7,36 / ab 7,16 (V8). Der V1-Bezug verlangt eine Textur, die bunter ist als der Screenshot selbst nach einem Durchlauf.

### 3.2 Zielwerte (ersetzt V1 bei K1/K2; K4, B2, B9 bleiben mit Zusatz)

Regel: Bezug ist das Original; nur wo der Rundlauf des Originals selbst die V1-Grenze verfehlt, ist der Bezug der Rundlauf (Original-T x Licht -> derselbe Filter -> Umkehr).

| Riegel | V1 | **V2 (gilt)** | Quelle | r3 | Textur-Ziel (Bauer) |
|---|---|---|---|---|---|
| K1 Bild-1-Kette C\* Figur | 0,85-1,2 x 9,25 = 7,87-11,10 | **0,85-1,2 x Rundlauf 7,85 = 6,67-9,42** | 3.1 Rundlauf | 6,09 ROT | Haut-Buntheit wie V8: Albedo-C\* der in Bild 1 sichtbaren Haut ~1,5 x r3 (7,6 -> 9,5), a\* <= 11 ausserhalb der Wunden, kuehle Toene C\* <= 8 (V8: 7,36) |
| K2 Bild-1-Kette ab-Streuung Figur | >= 0,85 x 9,83 = 8,36 | **>= 0,85 x Rundlauf 8,03 = 6,83** | 3.1 Rundlauf | 6,23 ROT | wie K1 (V8: 7,16); die Streuung kommt aus warm/kuehl nebeneinander, nicht aus mehr Rot |
| K4 Bild-1-Kette Rotklasse Figur | 0,6-1,5 x 5,25 = 3,15-7,87 % | unveraendert (Rundlauf 4,02 liegt darin) | V1 6 | 2,75 ROT | V6: 5,2-5,3 % |
| **K4b (neu)** Bild-1-Kette Rotklasse Ruecken/Flanke | - | **0,6-1,5 x 11,9 = 7,1-17,9 %** | 3.1, Rundlauf 10,0 liegt darin | 2,3 ROT | **15-18 % der in Bild 1 sichtbaren Ruecken/Flanke-Pixel helles Fleischrot L\* 45-55** (Farbton 34 Grad, C\* 32, Familienwunde); dunkles Rot (L\* <= 35) braucht >= 20 % und zaehlt kaum |
| B2 groesste Rotflaeche / Figur, 3/4 vorn und Seite | >= 3,2 % | >= 3,2 %, **Lab float** (V1-Kopf), nicht `fl.lab` (uint8) | Familie float: EM30 3,57/4,77, G4 12,35/3,30, G5 14,22/12,33 (uint8 G4 Seite 3,21) | 3/4 vorn 2,88 ROT (Seite 4,74) | eine zusammenhaengende Wunde >= 3,2 % der Figur in 3/4 vorn (>= 480 von 14 976 px), d. h. +11 % gegen r3; dunkler Wundrand in G4-Dunkelrot (18,9/25,6/12,2) liegt selbst in der Rotklasse und verbindet |
| B9 Rot im Umrissband | <= 11,3 %, Clip 2 | <= 11,3 % an Clip 2 **und** Gegenprobe Clip 0 <= 11,3 % | V1/Familie EM30 11,1 / G4 7,7 / G5 7,2 | 17,5 % (je Winkel 17,5/20,7/14,1), Clip 0 10,0 ROT | Fleisch und Wunden mindestens 2 Texel innerhalb der Umrisskanten der Ruhepose; Ursache r3 laut Sicht r3 P15: Schwanzspitze, Krallen, Bauchkante (verdeckte Rumpf-Unterseite ny > 0) |

Eingeeichte Riegel (Zahlen-Pruefer r3 6.1): K3, K6, K7, K8 belegen nur die Eichung, solange die Zonenversaetze Original minus Kette sind. Wer eicht, zeigt dieselben Riegel zusaetzlich
mit einer zweiten Saat des Videofilters (`saat` != 15) und gibt die Versatzbetraege an; ein Versatz > 6 dE (V1-K3-Grenze) auf einer Zone ist im Dossier zu begruenden.

## 4. Konstanten (Pruefung r3 Abschnitt 5): Quelle vorgeschrieben

| Konstante (kf_mal3.py) | r3-Stand | V2-Vorschrift |
|---|---|---|
| K_NAH / K_FERN 64 / 256 (Hohlraum-Mass, k-NN) | "Verfahrensparameter" | Quelle Familienmessung: Radius der Faltenform = halber Linienabstand der Familie (Fleisch: Flaeche je Linienlaenge 1000/(n x Laenge) = EM30 52, G4 31, G5 19 Texel -> Radius 9-26 Texel, k = pi r^2 = 250-2100). K_FERN = pi (Gliedhalbbreite in Texeln)^2, Gliedhalbbreite aus der K5c-Geometrie je Mesh gemessen und im Dossier gelistet. |
| K_GRAPH 8 | "Verfahrensparameter" | Rechenparameter: zulaessig mit Unabhaengigkeitsnachweis - Lauf mit 4 und 16; keine Riegelentscheidung kippt, kein Riegelwert aendert sich um mehr als seinen Abstand zur Grenze. Sonst nicht zulaessig. |
| SPIEGEL_ABSTAND 2,0 | "Verfahrensparameter" | wie K_GRAPH (Laeufe 1,0 und 4,0). |
| Faltenuebergang 1 Standardabweichung (`np.clip(hz, 0, 1)`) | ohne Quelle | entfaellt als Flaechenfaerbung: Falten werden Linien (Abschnitt 5). Falls eine Flaeche bleibt: Anteil Gewicht >= 0,5 = G4-Dunkelrot-Anteil 7,6 % (V1 1, G4-Palette), r3 hatte 9,6 %. |
| Fleisch-Koernung +-1 sigma | nach Riegeln gewaehlt | Quelle = Familienmessung: die Begrenzung wird so gesetzt, dass Atlas-L\*-Streuung in den Rotflaechen 10,9-14,5 (B14) erreicht wird; nicht mehr frei. |
| Eichung: Saettigungsregel 0,1; Abbruch 1 dE; Fenster 0,95-1,05 | ohne Quelle | Abbruchschwelle = gemessene Kettenstreuung ueber >= 3 Saaten (Zahlen-Pruefer r3: Saat 15/12345 K1 6,09/6,12) x 2; Fenster entfaellt (Ziel ist das V2-K1-Fenster 6,67-9,42); Regel 0,1 nur mit Unabhaengigkeitsnachweis wie K_GRAPH. |
| L_STUFEN (0,10,15,20,25,30,40,60,100) | 45 fehlt | Pflichtgrenzen 20 und 45 (V1 2 Klassen). Uebrige Grenzen = Dezile der Original-T-Helligkeit (Bild 1 + 2, Nicht-Rot-Figurpunkte), jede Stufe >= 30 Bildpunkte (wie im Code). |
| Zahnfleisch-Gewicht 21,5 ("G4-Maul a\* p90") | am lueckengefuellten Ausschnitt | **19,30** = a\* p90 der 2011 genutzten G4-Texel in (10,44,60,40) (nachgemessen; Zahn 1,7 / Rot 8,7 / Dunkel 47,6 %). Messungen nie an gefuellten Texeln. |
| Hautseiten-Mehrheitsschwelle 0,5 | mit E14 begruendet (E14 regelt nur das Rot) | Quelle = Messung: Schwelle so, dass der warme Anteil der Haut-Texel dem warmen Anteil der Nicht-Rot-Figurpunkte in Bild 1 + 2 (k-Mittel-Gewicht) entspricht; Wert im Dossier. Alternativ PORT-WAHL nur mit Nutzer-Wortlaut (liegt nicht vor). |
| `kf_mal.ausschnitt` sigma 2, 4 Durchlaeufe | offen aus r2 | Bau: zulaessig, wenn die Riegel des Felds (A6-A8, B6) an den genutzten Texeln gruen bleiben; Messungen und Eichwerte nie an gefuellten Texeln. |
| Kopienzahl +1 (Kiefer, Krallen) | offen aus r2 | B6 (Wiederholung <= 0,28 %) zusaetzlich je Feld Kiefer und Krallen; sonst Unabhaengigkeitsnachweis wie K_GRAPH. |
| Textfehler (Schritt 9 "-55..80 Grad" statt H_MIN -125; bau.json "k-Mittel Bild 1") | - | berichtigen. |
| Neue Konstanten des V2-Baus (Abschnitt 5) | - | jede mit Zeile aus Abschnitt 1 dieses Blatts (Familienspalte) oder V1-Zeile; Saat fest und genannt. |

Nutzer-Wortlaut fuer PORT-WAHL liegt nur fuer Form/Modell vor (E5/E19/E20, "eher c"); fuer keine der obigen Malkonstanten. Deshalb ist PORT-WAHL hier keine Option.

## 5. Vorschlag fuer den Bau (Versuch 2)

Grundsatz: auf die Koerperoberflaeche gemalt (Texelgraph `kf_mal3.oberflaeche`, 3D-Nachbarschaft), Dichte/Breite/Laenge/Kontrast aus Abschnitt 1, Leitbild RE2-G4 (V1); keine Foto-
oder Atlas-Kacheln. Alles, was r3 erreicht hat (28 gruene V1-Riegel: Farbfamilie, Glanz 0 %, Mehrtoenigkeit, Texeldichte, Kiefer dreiteilig, Augen dunkel, Zonen, Schwanz aus einem Stueck,
Tafel-Fairness), bleibt Halteriegel; die Binnenzeichnung wird auf r3 gelegt, nicht neu angefangen.

1. **Muskelachse je Teil**: 3D-Laengsachse je Mesh (PCA der Ruheposen-Eckpunkte bzw. Knochenrichtung), je Texel in UV ueber die Jacobi-Matrix des Dreiecks abgebildet. Die Familie legt
   ihre Kanten laengs der Felder (G4 98 % < 30 Grad zur Feldachse, EM30 72 %, G5 59 %).
2. **Adern/Muskelstraenge im Fleisch**: Polylinien auf der Oberflaeche ab Saatpunkten (Poisson-Scheibe in 3D, Abstand = Linienabstand 19-52 Texel, Ziel G4 31), Richtung = Muskelachse,
   Abweichung so, dass die Feldkohaerenz R in 0,21-0,51 bleibt; Laenge Median 11-16 Texel (p90 21-43), Breite 1,5-1,8 Texel, Kontrast (Top-Hat) 11-15 L\* dunkler als die Umgebung,
   Farbe G4-Dunkelrot (18,9/25,6/12,2). Ziel B10 9,5-19,3 %, B13, B17.
3. **Wunden mit Stufen**: in der vorhandenen Fleischkarte je Wunde 2-4 diskrete Rotstufen aus RAMPE_FLEISCH (G4/EM30-Palette, V1 1) als Baender nach geodaetischem Abstand zum
   Wundrand, Uebergang <= 1 Texel (harte Spruenge, keine Verlaeufe - sonst laufen die Stufen unter Gouraud ineinander, 1.2): Rand 1-2 Texel G4-Dunkelrot, innen (32,8/43,7/25,8),
   Kern (51,5/26,5/18,0) bzw. (64,3/24,7/16,3). Leitwerte G4: dunkler Aussenrand 74 % der Flaechen, Hell-Kern 50 % (keine Riegel). Ziel B14-B16.
4. **Hautfalten**: in der Haut als Linien, wo das Hohlraum-Mass ein Maximum quer zur Gliedachse hat (Beugen, Achsel, Beinansatz), Breite 1,3-1,5 Texel, Kontrast 9-12 L\*, Ton =
   Hautton derselben Seite, dunkler. Ziel B11 3,9-8,3 %, B17. Die r3-Hautkoernung (Fingerabdruck-Wirbel, `N2_linien_r3.png`) bleibt nur, solange A6/A7/A12 gruen sind.
5. **Kopf**: wenige Platten-/Stirn-/Schnauzenlinien und Augenhoehlenraender (G4-Kopf), Breite ~2 Texel, Kontrast ~16 L\* (EM30-Kopf 2,1 / 16,2); Ziel B12 (Canny >= 25,2, Linien >= 4,6 %).
6. **Bild-1-Ruecken** (Z2): an der Z1-Stelle 15-18 % der in Bild 1 sichtbaren Ruecken/Flanke-Texel helles Fleisch L\* 45-55 als zusammenhaengende Wunde mit den Stufen aus 3;
   Hautbuntheit der sichtbaren Haut ~1,5 x r3 mit den Kappen aus V8. Ziel K1 6,67-9,42, K2 >= 6,83, K4 3,15-7,87 %, K4b 7,1-17,9 %.
7. **B2/B9**: groesste Wunde an Flanke/Bauch so legen, dass sie in 3/4 vorn >= 3,2 % der Figur deckt; Fleisch/Wunden >= 2 Texel innerhalb der Umrisskanten (Bauchkante, Schwanzspitze,
   Krallen), Gegenprobe Clip 0.
8. Pruefreihenfolge: `riegel_farbe.py` (V1 + B1-B9, mit den V2-Aenderungen an K1/K2/K4b/B2/B9) und `n2_riegel.py` (B10-B17) am selben Ordner; Tafeln erst danach (V1 7.5).

## 6. Offen

1. **K1/K2-Bezugswechsel** ist ein Pruefer-Entscheid aus Messung (3.1); `riegel_farbe.py`/`p_riegel.py` rechnen noch den V1-Bezug. Der Bauer stellt um (Rundlauf in `kette_riegel`)
   und weist beide Werte aus; ein zweiter Pruefer rechnet den Rundlauf nach.
2. Ob die kuehle Buntheit der Hinterbeine in Bild 1 (T b\* -3,6/-7,9) Licht oder Haut ist, entscheidet ein einzelnes Bild nicht; B3 haelt die Textur unbunt-kuehl.
3. B11 und B13 sind schwach (Empfindlichkeit 1.5); B17 hat nur 0,03-0,05 Abstand. Gilt einer davon allein als Grund fuer ROT, entscheidet die Sichtpruefung (Tafel 03/04/06).
4. Wundrand (c) und Hell-Kern (f) trennen die Familie nicht (EM30 gegen G4/G5); Leitwerte aus G4, keine Riegel.
5. Die Ersatzversuche V6-V8 aendern das 2D-Bild, nicht die Textur; ob die 3D-Textur dieselben Kettenwerte erreicht, zeigt erst der Bau.
6. Nutzerfrage 6 (Sicht r3 P8: rosa Stelle unten links in Bild 1 = Fuss, Schwanzspitze oder Schatten) bleibt offen; sie betrifft B9 (Schwanzspitze im Umrissband).
