# Pruefung Farbe Versuch 2, Runde 1: Zahlen-Pruefer (2026-11-07)

Gegenstand: Variante `farbe/K5c_farbe_v2/` (FORM.TIM sha256 eca20f1d..., FORM.EMD sha256 a71f8d08...), Commits aee1a1b71, 1984269ca, e38c7996c auf r41/form.
Massstab: MESSBLATT_FARBE_V1.md (A1-A12, K1-K10, N1-N2, B1-B9) + MESSBLATT_FARBE_V2.md (K1v2/K2v2/K4b/B2 float/B9b/B6 je Feld/B10-B17, Konstanten Abschnitt 4).
Nicht committet (Auftrag).

## 0. Ergebnis: NICHT BESTANDEN

1. **7 Riegel ROT, von der eigenen Nachmessung alle 7 bestaetigt**: K5 (geclippt), N1 (Seite), B2 (3/4 vorn), B6 Koerper, B6 Feld Kiefer, B12 (Kopf-Canny), B17 (Render).
2. **Konstanten-Vorschrift V2 4 verletzt**: Fuer K_GRAPH 8 und SPIEGEL_ABSTAND 2,0 fehlt der Unabhaengigkeitsnachweis. Der Bauer sagt das selbst (FARBE.md V2.4), und die Laeufe bestaetigen es: Keiner der Laeufe kg4/kg16/sp1/sp4 kommt auf 45/45, und Entscheidungen kippen. Nach V2 4 sind beide Werte damit "nicht zulaessig".
3. **Ziele Z1/Z2 zahlenmaessig weitgehend erreicht**, und zwar gegen die eigene Implementierung:
   - Binnenzeichnung (Z1): B10, B11, B13, B14, B15 und B16 gruen.
   - Bild-1-Rot (Z2): K1v2, K2v2, K4, K4b, B9 und B9b gruen.
   - Alle 28 Halteriegel aus r3 sind gruen geblieben, mit Ausnahme von N1 und B6. Beide waren in r3 gruen und sind jetzt knapp rot.
   - B2 ist weiter rot.

## 1. Eigene Messung (Werkzeug, Unabhaengigkeit)

Der eigene Code liegt in `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe2/pruef_v2r1/code/`:

| Skript | Inhalt |
|---|---|
| `eigen.py` | Atlas und Render, B1/B2/B9/B10-B17 und N1/N2 |
| `kette.py` | Bild-1/2-Kette mit 3 Saaten (15, 12345, 7) und Rundlauf |
| `atlas_v1.py` | A1-A12, B4-B7 und B6 je Feld |

Die Ergebnisse stehen als `eigen_*.json`, `kette_*.json` und `atlas_v1_v2.json` im Ordner `pruef_v2r1/`.

Eigenstaendig implementiert sind:
- Lab: skimage `rgb2lab`, nicht OpenCV.
- Top-Hat: scipy `grey_closing`.
- 8-Komponenten und Linien-PCA: scipy `label`.
- Strukturtensor: scipy `sobel` und `gaussian_filter`.
- k-Mittel: scipy `kmeans2`, mit eigener Single-Linkage-Kette.
- HF_fft und HF_rest: eigene FFT bzw. normierte Faltung.
- B6: Wiederholungsbloecke als vektorisierte Matrix.
- Rot-, Hell- und Clip-Klassen der Kette.

Uebernommen sind nur:
- Lader und Texelsammlung: `p_lade.sammeln` und `p_messen.atlas`.
- Renderer `render.blick`.
- Kette: `Kandidat.rendern`, `video_filter` und `rueck`.
- Zonenmasken.

Den TIM-Decoder habe ich selbst gegengelesen: Indexbild bytegleich, CLUT-Abweichung 0 (8 bpp, 256 x 4 CLUT, 512 x 256).

## 2. Riegel: Soll | Bauer | eigene Messung

| Riegel | Soll | Bauer | eigen | Bewertung |
|---|---|---|---|---|
| A1 Chroma Koerper | 9,5-21 | 14,4 | 14,40 | GRUEN |
| A2 sigma_ab je Teil | >= 7,0 | 9,2 | 9,20 | GRUEN |
| A3 Fleischrot | 8-40 % | 20,5 | 20,89 | GRUEN |
| A4 Farbton | 15-50 Grad | 27,0 | 27,3 | GRUEN |
| A5 Neutralweiss | <= 0,20 % | 0,0 | 0,0 | GRUEN |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,04-0,26 | 0,091/0,250/0,045/0,068 | 0,091/0,2498/0,0445/0,068 | GRUEN (Kopf 0,010 unter der Grenze) |
| A7 HF_rest | <= 0,19 | max 0,118 | max 0,118 | GRUEN |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >=20 / >=20 | 6,8/29,0/40,5 | 6,81/29,05/40,49 | GRUEN |
| A9 Auge L*/C* | <= 30 | 13,1/18,0 | 13,16/18,02 | GRUEN |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 | nicht neu gemessen: UV bytegleich K5c | GRUEN |
| A11 Schwanzsprung | <= 4,0 | 0,5 | 0,52 (Glieder 14,1/14,1/14,2/14,6/14,0) | GRUEN |
| A12 Farbkorn | <= 5,7 | 4,62 | 4,62 | GRUEN |
| Rundlauf C* / ab (V2-O6) | V2 7,85 / 8,03 | 7,8 / 8,0 | Saat 15/12345/7: 7,84/7,86/7,82 und 8,05/8,04/8,06 | **bestaetigt** |
| K1v2 Bild-1-Kette C* | 6,67-9,42 | 6,7 | 6,73 / 6,73 / 6,71 | GRUEN, Abstand zur Untergrenze nur 0,04-0,06 |
| K2v2 ab-Streuung | >= 6,83 | 8,2 | 8,21 / 8,24 / 8,22 | GRUEN |
| K3 dE je Zone | <= 6 | max 5,1 | max 5,18 / 5,11 / 5,03 (Hinterbein r (W)) | GRUEN (eingeeicht, siehe 6.3) |
| K4 Rotklasse Figur | 3,15-7,87 % | 6,5 | 6,57 / 6,44 / 6,63 | GRUEN |
| K4b Rotklasse Ruecken/Flanke | 7,1-17,9 % | 11,5 | 11,46 / 11,42 / 11,39 | GRUEN |
| **K5** Hell Ruecken / geclippt Figur | <= 1,5 x Original | 9,3 / 0,8 (Grenze 14,9 / 0,75) | Hell 9,35/9,26/9,31 (Grenze 15,0); geclippt 0,85/0,83/0,81 gegen Original 0,54 (Grenze 0,81) | **ROT** (2 von 3 Saaten ueber der Grenze, Saat 7 genau auf ihr; gegen die Messblatt-Grenze 0,75 alle drei) |
| K6 Bild 2 dE / C* | <= 6 / >= 21,8 | 2,2 / 23,1 | 2,19 / 23,16 | GRUEN |
| K7 Unterkiefer dE / a* | <= 6 / >= 21,2 | 2,6 / 22,4 | 2,64 / 22,40 | GRUEN |
| K8 Oberkiefer dE | <= 6 | 4,0 | 3,98 / 3,86 / 4,12 | GRUEN |
| K9 Klauen a* r/l | >= 18,1 / 16,6 | 26,3 / 28,0 | 26,31 / 27,98 | GRUEN |
| K10 Augen L* max | <= 30 | 19,4 / 15,7 | 19,5/15,7; 18,7/12,9; 15,8/16,6 | GRUEN |
| **N1** C* je Winkel | >= 0,75 x EM30 | 10,7/9,3/12,6 gegen EM30 10,0/12,5/10,5 | 10,66/**9,25**/12,54 gegen EM30 9,94/12,49/10,42 (Grenze Seite 9,37) | **ROT** (Seite) |
| N2 L*>85 | <= 0,10 % | 0/0/0 | 0/0/0 | GRUEN |
| B1 erosionsfest | >= 32,1 % | 36,4 | 37,4 | GRUEN |
| **B2** groesste Rotflaeche 3/4 vorn / Seite | >= 3,2 % | 3,11 / 3,67 | **3,18** / 3,78 (Lab skimage) | **ROT** (3/4 vorn, unabhaengig von der Lab-Implementierung) |
| B3 Blau-Klasse | <= 0,10 % | 0/0/0 | 0/0/0 | GRUEN |
| B4 Glanzrest p90 | <= 58 | 50,4 | 50,5 | GRUEN |
| B5 Kopffeld Zahnklasse | <= 2,0 % | 0,03 | 0,03 | GRUEN |
| **B6** Wiederholung Koerper | <= 0,28 % | 0,29 | **0,295** | **ROT** |
| **B6** Feld Kiefer | <= 0,28 % | 1,83 | **1,67** | **ROT** |
| B6 Feld Krallen | <= 0,28 % | 0,0 | 0,0 | GRUEN |
| B7 Rot-Farbton-Streuung | >= 5,8 Grad | 10,4 | 10,41 | GRUEN |
| B8 Bild-2-Zahnflecken | 13,6-33,9 % | 15,9 | 16,0 / 17,5 / 17,4 | GRUEN |
| B9 Umrissband Clip 2 | <= 11,3 % | 8,1 | 8,20 | GRUEN |
| B9b Umrissband Clip 0 | <= 11,3 % | 5,6 | 5,70 (r3: 10,01) | GRUEN |
| B10 dunkle Linien Fleisch | 8,1-23,2 % | 8,94 | 8,67 (46 Linien) | GRUEN, Abstand 0,57 |
| B11 dunkle Linien Haut | 3,3-9,9 % | 6,22 | 6,09 | GRUEN |
| **B12** Kopf Linien / Canny | >= 4,6 % / >= 25,2 | 20,07 / 24,5 | 20,07 / **24,47** | **ROT** (Canny) |
| B13 Canny Fleisch | 25,1-36,2 | 32,85 | 32,74 | GRUEN |
| B14 L*-Streuung Render / Atlas | >= 8,2 / >= 9,3 | 10,1 / 12,1 | 10,12 / 12,3 | GRUEN |
| B15 a*-Streuung Render | >= 4,25 | 6,2 | 6,19 | GRUEN |
| B16 Rotpixel in Flaechen mit >= 2 Stufen | >= 50,7 % | 81,0 | 81,3 (31 Flaechen) | GRUEN |
| **B17** Kohaerenz Render / Atlas | >= 0,60 / >= 0,576 | 0,599 / 0,586 | **0,5990** (0,586/0,622/0,589, Kachel 521 px) / 0,587 | **ROT** (Render) |

Summe eigen: 38/45 GRUEN, identisch mit der Summe des Bauers. Die eigenen Werte liegen in 0,0-0,3 der Werte des Bauers, B6 Kiefer in 0,16, und kein Riegel kippt zwischen den beiden Implementierungen.

## 3. Geometrie, Clips, Asset

- **sha256 a71f8d08...** ist fuer sechs Dateien gleich: `K5c/FORM.EMD`, `farbe/K5c_farbe_v2/FORM.EMD`, `farbe/K5C_FARBE_V2.EMD`, `scratchpad/formfarbe2/emd/K5C_FARBE_V2.EMD`, `farbe/K5C_FARBE.EMD` und `farbe/K5c_farbe_r3/FORM.EMD`. Geometrie, Clips und UV sind damit bytegleich.
- **sha256 eca20f1d...** ist fuer die drei TIM-Kopien gleich.
- **Nachbau unabhaengig wiederholt** mit `python kf_mal4.py bauen` und frischem Cache (`KF4_CACHE=pruef_v2r1/kf4cache`). TIM und EMD sind laut cmp bytegleich, der Generator ist also reproduzierbar.
- **Spiel-Asset unveraendert**:
  - `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` hat sha256 16eb6ea8..., `.TIM` hat ec280802....
  - Letzter Commit an beiden ist 67408cfcb (NB2).
  - `git diff 73d953f7f..HEAD -- re15_port` und `git diff 7e9b314c7..HEAD -- re15_port` sind leer, der Baum ist sauber.
- Hinweis: Der Kandidat hat 3 Clips (K5c-Variante), das Spiel-Asset 28. Laut `spiel_lauf.log` laedt der Spiellauf den Kandidaten mit "3 Clips -> Slot". Das war in r3 genauso, es ist kein V2-Defekt.

## 4. Tests

- **Lauf**: `ctest --test-dir re15_port/build -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600` (PATH mit msys64 zuerst).
- **Ergebnis**: 11/11 gruen in 95,05 s, nichts wiederholt. Log: `pruef_v2r1/ctest.log`.
- **Build**: Ich habe nicht neu gebaut. `ninja -n` meldet "no work to do", und re15_port ist seit 7e9b314c7 unveraendert.
- **Reichweite**: Die Tests pruefen nur das unveraenderte Spiel-Asset, sie sagen ueber die Variante nichts aus.

## 5. Gegenproben

- **r3 mit eigener Messung**:
  - Alle V2-Riegel sind rot: K1v2 6,07/6,11/6,06, K2v2 6,24/6,30/6,25, K4b 2,30/2,37/2,25, B2 3/4 vorn 3,01, B9 17,77, B10 3,57, B11 3,04, B12 1,28/8,75, B13 18,35.
  - B14 ist rot ueber den Render-Wert 5,44.
  - B15 3,13, B16 0,0, B17 0,560/0,560.
  - B9b Clip 0 liegt bei 10,01 und ist damit gruen (Bezugsprobe laut V2 3.2).
- **Familie mit eigener Messung**:
  - G4 ist bei B10-B17 komplett gruen: 9,51 / 3,89 / 46,45+29,61 / 29,44 / 11,04+11,2 / 7,90 / 81,6 / 0,616+0,619.
  - EM30 ist ebenfalls komplett gruen: 12,51 / 4,49 / 5,39+29,68 / 29,69 / 9,60+10,9 / 5,02 / 61,1 / 0,6205+0,577.
  - Achtung: Mit eigener Implementierung liegt der EM30-Einzelwinkel 3/4 vorn bei 0,598 und damit unter der B17-Grenze 0,60. Die Grenze liegt im Implementierungsrauschen (siehe 6.5).
- **K5c heute**: Die Aussage "alle Farbriegel ROT" stimmt nur, weil `riegel_farbe.py` die V2-Riegel aus der K5c-heute-Pruefung ausnimmt (`V2_NEU`). Tatsaechlich sind dort B11, B13, B14, B15, B16, B9 und B9b gruen. Das deckt sich mit dem Messblatt V2 (K4 heute 5/8). Die Gegenprobe gegen K5c heute traegt fuer diese Riegel also nichts. Den Satz in der Auftrags- bzw. Bauer-Meldung muss man deshalb einschraenken.

## 6. Konstanten (Diff 73d953f7f..HEAD -- generator/)

### 6.1 Konstanten mit nachvollziehbarer Quelle (geprueft)

| Konstante | Wert | Pruefung |
|---|---|---|
| K_NAH | 755 | pi x 15,5^2 = 754,8 |
| ZAHNFLEISCH_A | 19,30 | V2 4 |
| JE1000_KOPF | 1,22 | `N2_messung.json` EM30_kopf je1000 = 1,22 |
| KOPF_BREITE / KOPF_KONTRAST | 2,1 / 16,2 | ebd. 2,08 / 16,2 |
| SIGMA_WINKEL | 0,580 | sqrt(-ln 0,51 / 2) = 0,580 |
| FLEISCH_ANTEIL | 0,239 | V1 G4 |
| G_START | 2,22 | 1,48 x 1,5, V2 3.2 |
| K1_RUNDLAUF | 7,85 | eigen 7,82-7,86 |
| Eich-Abbruch | 2 x 0,125 | `kf_eich4.json` kettenstreuung |
| Linien-/Faltenwerte | Familienmitte | V2 1 |
| L_STUFEN | | Pflichtgrenzen 20 und 45 enthalten |

### 6.2 Konstanten mit Mangel

1. **K_GRAPH 8 und SPIEGEL_ABSTAND 2,0: Nachweis nicht erbracht** (V2 4: "Sonst nicht zulaessig").
   - Gelesen in `scratchpad/formfarbe2/riegel_ind_*.json`: kg4 39/45 (ROT: A6, K5, K1v2, N1, B6 Kiefer, B17), kg16 40/45 (N1, B6, B8, B6 Kiefer, B17), sp1 40/45 (K5, B6, B6 Kiefer, B10, B17), sp4 42/45 (N1, B6, B6 Kiefer).
   - Mit 2,0/8 sind es 38/45.
   - B2 3/4 vorn springt zwischen 3,11 und 5,88 und B12 zwischen 24,5 und 28,5. Diese beiden Riegel haengen also staerker am Rechenparameter als an der Malentscheidung.
   - **Schluss**: Das ist ein Nachweisfehler und fuer sich ein Grund fuer "nicht bestanden".
2. **Von kf_mal3 geerbte Konstanten ohne Zeile in der V2.3-Tabelle, aber neu verwendet**:
   - `SIG_GROB` 2,5 hat als Quelle nur `kf_mal.py:40-42`. V2 nutzt den Wert neu fuer die Glaettung des Muskelachsen-Doppelwinkels und fuer die Wundkante (kf_mal4.py:484, 566).
   - Die Mehrheitsschwelle 0,5 der Wundkante (kf_mal4.py:566) ist ebenfalls neu verwendet.
   - Unveraendert uebernommen und weiter wirksam sind L_MAX_HAUT 58, C_REST 5,5, EINSTREU_A2 12,8, B_MIN -6, C_BLAU 8 und H_MIN/H_MAX. Diese haben Quellen in kf_mal3, das ist in Ordnung.
   - V2 4 verlangt aber eine Quelle fuer jede neue Konstante des V2-Baus. SIG_GROB in neuer Funktion fehlt in der Tabelle.
3. **FLEISCH_KOERNUNG 1,0 ist eine Auswahl aus zulaessigen Werten, keine Ableitung**.
   - V2 4 verlangt "so gesetzt, dass B14-Atlas 10,9-14,5". Die Laeufe cf1/1,5/2,5 erreichen das alle (12,1/12,8/13,7).
   - Gewaehlt wurde 1,0 mit der Begruendung "= Hautkoernung, E8 aus einem Guss". Der Nutzer-Wortlaut E8 betrifft den Gesamteindruck und keinen Zahlenwert.
   - **Schluss**: Das ist zulaessig als dokumentierte Wahl innerhalb des Fensters, eine Quelle im Sinn von V2 4 ist es nicht ganz.
4. **Linienabstand 2 Texel ist aus der Messdefinition abgeleitet** (Halbbreite der 5x5-Schliessung des Riegels B10), nicht aus der Familie. Damit wird gegen das Messwerkzeug gebaut. Das ist kein Toleranzfehler, nur ein Hinweis.
5. **Koernungslaenge 13,5** stammt aus der Linienlaenge der Familie (Adern) und wird auf die Koernung uebertragen, also ein anderes Mass. **ZIEL_ROT_TON_SD 9,4**: Erreicht sind 10,4. Dass die Halbierung nicht auf dem Ziel endet, ist unbegruendet. Beides ist nur ein Hinweis.
6. **Textfehler im Kopf von kf_mal4.py**:
   - Punkt 5 sagt "laengs und quer zur Kopfachse", der Code setzt aber `KOPF_QUER = False`.
   - Punkt 7 nennt "Eichfaktor g ... 1,48", der Code setzt aber `G_START` = 2,22.
   - Das Messblatt V2 4 verlangt, Textfehler zu berichtigen.

### 6.3 Eingeeichte Riegel (V2 3.2, letzter Absatz)

- **Versaetze**: Sie sind unveraendert aus r3 uebernommen (`kf_eich4.json` 'runden' = [], die Eichlaeufe e1/e2 wurden verworfen). Ueber 6 dE liegen sie in 8 Zonen, am groessten bei Hinterbein r (W) mit 56,2 dE (b* -51,4) und beim Unterkiefer mit 54,0 dE.
- **Begruendung im Dossier**: "Kette/Kappen ausgleichen; b* der Beine gesperrt". Formal ist sie vorhanden, sachlich duenn.
- **Kappe**: Nachbau-Log und `bau.json` melden "Blau-Klasse auf C* 8: 30 916". Das sind 36 % der 85 948 genutzten Texel, falls die Zaehlung genutzte Texel meint. Ein grosser Teil der b*-Versaetze wirkt damit als Kappe und nicht als Farbe.
- **K3/K6/K7/K8**: Sie sind bei allen drei Saaten gruen (eigen 15/12345/7). Weil die Versaetze nicht neu geeicht wurden, belegen K3-K8 diesmal mehr als nur die Eichung. Die Spanne bis zur K3-Grenze betraegt nur 0,8-1,0 dE.

## 7. Tafeln

- **03_familie**: fair. Gleicher Renderer, Licht 0,55 + 0,45 N.L, Umkugel 0,46, roh, Zahlen in der Beschriftung. Die Beschriftungs-Werte der V2-Kacheln (L* 18,4/15,9/17,6, C* 10,7/9,3/12,6) stimmen mit der eigenen Messung ueberein (C* 10,66/9,25/12,54).
- **01/02**: fair. Original, K4 heute und V2 laufen unter gleichem Disc-Licht x k, Zeile 2 durch denselben Videofilter, kein Gamma.
- **07**: fair.
  - Alle Bilder stammen aus dem echten Fenster derselben exe-Kopie, laut `spiel_lauf.log` 10/10 mit rc=0.
  - Die V2-Bilder unterscheiden sich von spiel_r3 (sha256 5814d088 gegen 81dc885b, 5babc86b gegen 4ce23bc5).
- **08_binnenzeichnung**: Licht und Rohbehandlung sind gleich, die **Ausschnitte zeigen aber nicht dieselbe Anatomie**.
  - Die Regel "Anteil des Figurrahmens" trifft bei "Schulter" bei r3/V2 den Kopf mit Kiefer, bei G4 die Klauen und bei EM30 den Bauch.
  - Bei "Flanke" trifft sie bei G4 das Schulterauge.
  - Den Schulter-Fall raeumt der Bauer selbst ein (V2.7).
  - Als Beleg fuer Z1 vergleicht die Tafel damit ungleiche Koerperteile. Das ist keine Manipulation, aber nur bedingt aussagekraeftig.
- **Darstellungsregel V1 7.5**: Bilder gehen erst zum Nutzer, wenn alle Riegel gruen sind. Das ist nicht erfuellt (38/45), der Bauer haelt die Regel aber selbst ein ("keine Abnahmevorlage").

## 8. Weitere Befunde

1. **Knappe Gruenwerte**: K1v2 (Abstand 0,04-0,06 bei Kettenstreuung 0,125), B10 (0,57) und A6 Kopf (0,010; HF_MAX 0,258 wurde gezielt an die Grenze gelegt).
   - Zusammen mit den sechs roten Riegeln, die um 0,001-0,7 verfehlen, liegt das Ergebnis im Laufrauschen. Das zeigen die Laeufe kg/sp.
   - Das Laufprotokoll nennt ueber 22 Abstimmlaeufe (v0-v22, exp_*, o_*, ny-*) gegen dieselben Riegel. Der Abstand zur Grenze ist deshalb kein Robustheitsmass.
2. **Die B17-Grenze 0,60 liegt im Implementierungsrauschen**: EM30 3/4 vorn kommt bei mir auf 0,598, beim Bauer auf 0,601. V2 hat 0,5990 (beide Implementierungen). Der Riegel ist trotzdem ROT, die Regel kennt keine Ausnahme. Ein Hinweis fuer das Messblatt: B17 ist ein schwacher Riegel (V2 6.3).
3. **Lab-Grenzempfindlichkeit an r3**: Der Atlas-Rotanteil von r3 betraegt mit skimage-Lab 23,0 %, mit OpenCV-Lab 20,3 %, bei einer maximalen Lab-Differenz von 0,25. r3 hat also viele Texel genau auf der Rotklassen-Grenze. V2 ist stabil (20,9 gegen 20,5). Fuer V2 aendert das nichts.
4. **Commits**:
   - Die Co-Authored-By-Zeile lautet "Claude Opus 5.5", der Auftrag verlangt "Claude Fable 5.1". Das ist ein Formfehler.
   - 1984269ca enthaelt Zahlen aus Lauf v20, z. B. "K1v2 6,8, B16 66,4". Der Bauer weist das in V2-O5 und in e38c7996c aus.

## 9. Was zum Bestehen fehlt (gemessen, Abstand zur Grenze)

| # | Riegel | Abstand / Bedarf |
|---|---|---|
| 1 | K5 geclippt | 0,05; r3 lag bei 0,35 und war gruen. Die Ursache liegt vermutlich bei der neuen hellen Ruecken-Wunde (L* 45-55) oder der Hautbuntheit g 2,22, das ist nicht gemessen. |
| 2 | N1 Seite | 0,12 C* |
| 3 | B2 3/4 vorn | 0,02-0,09 % = 3-14 Bildpunkte |
| 4 | B6 Koerper | 0,01 |
| 5 | B6 Kiefer | 1,4-1,55; braucht ein gemaltes Maulinneres statt der gestreckten G4-Kopie |
| 6 | B12 Kopf-Canny | 0,7, gegen HF_fft Kopf 0,250/0,26 (Zielkonflikt V2-O3) |
| 7 | B17 Render | 0,001 |
| 8 | Unabhaengigkeitsnachweis K_GRAPH/SPIEGEL_ABSTAND | entweder der Nachweis oder Werte, bei denen 4/16 und 1,0/4,0 keine Riegelentscheidung kippen |
| 9 | SIG_GROB in neuer Verwendung | Quelle in der Konstantentabelle nachtragen |
| 10 | Textfehler | im Kopf von kf_mal4.py berichtigen |
