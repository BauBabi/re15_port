# Pruefung Farbe Versuch 2, Runde 2: Zahlen-Pruefer (2026-11-07)

Gegenstand: `farbe/K5c_farbe_v2/` (FORM.TIM sha256 085bb4335fa6..., FORM.EMD sha256 a71f8d08...), Commits d8ea91597, 8f26798d8, 69597b13e, eb7f6eefa, 02aed15f7 auf r41/form.
Massstab: MESSBLATT_FARBE_V1.md (A1-A12, K1-K10, N1-N2, B1-B9) und MESSBLATT_FARBE_V2.md (K1v2/K2v2/K4b, B2 float, B9b, B6 je Feld, B10-B17, Konstanten Abschnitt 4).
Nicht committet (Auftrag).

## 0. Ergebnis: NICHT BESTANDEN

1. **2 Riegel ROT, beide von der eigenen Nachmessung bestaetigt:**
   - B12 (Kopf): Canny 4,11 (Soll >= 25,2), Linien 0,12 % (Soll >= 4,6 %).
   - B17 (Kohaerenz): Render-Mittel 0,543 (Soll >= 0,60), Atlas 0,563 (Soll >= 0,576). Beide Teile sind rot.
   - V2 6.3 erlaubt einen Sichtentscheid nur, wenn B17 *allein* rot ist. B12 ist zusaetzlich rot und weit entfernt (Canny 16 % der Grenze).
2. **Unabhaengigkeitsnachweis nach V2 4 fehlt weiter fuer K_GRAPH 8 und SPIEGEL_ABSTAND 2,0.**
   - kg4 kippt K4, K1v2 und K4b. kg16 kippt B8 und B10. sp4 kippt N1 und B2. Nur sp1 ist ohne Kippen.
   - Der Bauer raeumt das selbst ein (V2r2.5, O2). Nach V2 4 sind beide Werte damit "nicht zulaessig".
3. **Konstanten**: KORN_SIG 1,0 hat weder eine Quelle noch einen Nachweis. KORN_SD 2,30 stammt aus einer neuen Bauer-Messung, die keine Zeile in V2 1 und keine in V1 hat. Der Wert selbst ist nachgemessen und stimmt (G4 2,302).
4. **Neuer Befund ausserhalb der Riegel: Farbton des Bild-1-Rots verfehlt das Original (E14).**
   - Original-Rotklasse am Ruecken: h -11,8 Grad, 81 % mit b* < 0, also magenta.
   - V2: h +23,5 Grad am Ruecken und +38,1 Grad an der Figur, 0 % mit b* < 0, also orange-rot.
   - Abstand der Rotklassen-Mittel: dE 12,8 (Ruecken) bzw. 18,0 (Figur). K4 heute trifft den Farbton (-13,1 Grad).
   - Ursache ist die Vorgabe "Farbton 34 Grad" im Messblatt V2 3.2 selbst. Abschnitt 8.
5. **Gehalten**: Alle 43 uebrigen Riegel sind gruen und von mir nachgemessen. Z2 ist zahlenmaessig erreicht (K1v2, K2v2, K4, K4b, B2, B9, B9b). Geometrie und Clips sind bytegleich, der Nachbau ist bytegleich, Tests 11/11.

## 1. Eigene Messung

Der Code liegt in `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe2/pruef_v2r2/code/`:
- `eigen.py`, `kette.py`, `atlas_v1.py` (aus der Pruefung v2 r1, unveraendert)
- neu: `rotton.py` (Farbton der Bild-1-Rotklasse) und `korn.py` (Feinkorn, Quelle KORN_SD)

Die Ergebnisse stehen im selben Ordner `pruef_v2r2/`: `eigen_v2.json`, `kette_v2.log`, `atlas_v1_v2.json`, `rotton_v2.json`, `korn.json`, `n2_riegel_v2.log`, `ctest.log`, `nachbau/`.

Eigene Implementierung:
- Lab ueber skimage
- Top-Hat, Komponenten, PCA, Strukturtensor und k-Mittel ueber scipy
- FFT, HF und B6-Bloecke eigen

Uebernommen sind nur Lader und Rasterung, der Renderer `render.blick`, die Kette (`Kandidat.rendern`, `video_filter`, `rueck`) und die Zonenmasken.

TIM gegengelesen: 8 bpp, 256 x 4 CLUT, 512 x 256. Das Indexbild ist gleich, die CLUT-Abweichung 0.

## 2. Riegel: Soll | Bauer | eigen

| Riegel | Soll | Bauer | eigen | Bewertung |
|---|---|---|---|---|
| A1 Chroma Koerper | 9,5-21 | 15,4 | 15,39 | GRUEN |
| A2 sigma_ab je Teil | >= 7,0 | 9,3 | 9,37 | GRUEN |
| A3 Fleischrot | 8-40 % | 22,6 | 22,62 | GRUEN |
| A4 Farbton | 15-50 Grad | 27,0 | 27,2 | GRUEN |
| A5 Neutralweiss | <= 0,20 % | 0,0 | 0,0 | GRUEN |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,04-0,26 | 0,139/0,069/0,065/0,065 | 0,139/0,069/0,065/0,065 | GRUEN |
| A7 HF_rest | <= 0,19 | max 0,092 | max 0,092 | GRUEN |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >= 20 / >= 20 | 6,1/28,4/40,1 | 6,06/28,44/40,13 | GRUEN |
| A9 Auge L*/C* | <= 30 | 13,1/18,0 | 13,16/18,02 | GRUEN |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 | UV bytegleich K5c (EMD sha gleich) | GRUEN |
| A11 Schwanzsprung | <= 4,0 | 0,6 | 0,64 | GRUEN |
| A12 Farbkorn | <= 5,7 | 4,53 | 4,54 | GRUEN |
| Rundlauf C*/ab | V2 7,85/8,03 | 7,8/8,0 | 7,84/7,86/7,82 und 8,05/8,04/8,06 | bestaetigt |
| K1v2 C* | 6,67-9,42 | 7,0/7,0/6,9 | 6,96/6,94/6,91 (Saat 15/12345/7) | GRUEN (Abstand 0,24-0,29 = ca. 2 x Kettenstreuung 0,125) |
| K2v2 ab | >= 6,83 | 8,4 | 8,43/8,44/8,38 | GRUEN |
| K3 dE je Zone | <= 6 | max 4,4 | max 4,44/4,55/4,56 (Hinterbein r (W)) | GRUEN (eingeeicht, 6.2 Nr. 6) |
| K4 Rotklasse Figur | 3,15-7,87 % | 5,4 | 5,39/5,35/5,41 (Original 5,26) | GRUEN |
| K4b Rotklasse Ruecken/Flanke | 7,1-17,9 % | 8,8 | 8,80/8,97/8,79 (Original 11,92) | GRUEN |
| K5 Hell Ruecken / geclippt | <= 15,0 / <= 0,75 | 10,7/0,3 | 10,77/10,55/10,79 und 0,27 | GRUEN |
| K6 dE / C* | <= 6 / >= 21,8 | 1,1/23,2 | 1,13/23,21 | GRUEN |
| K7 dE / a* | <= 6 / >= 21,2 | 1,9/23,3 | 1,94/23,27 | GRUEN |
| K8 dE | <= 6 | 3,5 | 3,49/3,37/3,59 | GRUEN |
| K9 Klauen a* r/l | >= 18,1/16,6 | 26,6/27,9 | 26,58/27,92 | GRUEN |
| K10 Augen L*max | <= 30 | 14,6/15,9 | 14,6/15,9; 17,2/13,2; 16,8/15,0 | GRUEN |
| N1 C* je Winkel | >= 0,75 x EM30 (Seite 9,37) | 11,0/9,5/13,4 | 11,00/9,50/13,35 | GRUEN (Seite Abstand 0,13; kippt bei sp4) |
| N2 L*>85 | <= 0,10 % | 0/0,01/0 | 0/0,006/0 | GRUEN |
| B1 erosionsfest | >= 32,1 % | 41,9 | 42,0 | GRUEN |
| B2 groesste Rotflaeche 3/4 vorn / Seite | >= 3,2 % | 4,90/3,95 | 5,23/4,07 | GRUEN (eigen ohne Farbtonfenster, daher hoeher) |
| B3 Blau-Klasse | <= 0,10 % | 0/0/0 | 0/0/0 | GRUEN |
| B4 Glanzrest p90 | <= 58 | 43,7 | 43,8 | GRUEN |
| B5 Kopffeld Zahnklasse | <= 2 % | 0,0 | 0,0 | GRUEN |
| B6 Koerper / Kiefer / Krallen | <= 0,28 % | 0,14/0,0/0,0 | 0,137/0,0/0,0 | GRUEN |
| B7 Rot-Farbtonstreuung | >= 5,8 Grad | 10,3 | 10,30 | GRUEN |
| B8 Unterkiefer-Hellklasse | 13,6-33,9 % | 17,7 | 17,9/17,8/18,1 | GRUEN |
| B9 Umrissband Clip 2 | <= 11,3 % | 8,7 | 8,69 | GRUEN |
| B9b Umrissband Clip 0 | <= 11,3 % | 5,0 | nicht neu gemessen (Bauer 5,0) | GRUEN laut Bauer |
| B10 Linien Fleisch | 8,1-23,2 % | 8,81 | 8,81 (63 Linien) | GRUEN (Abstand 0,71; kippt bei kg16) |
| B11 Linien Haut | 3,3-9,9 % | 5,09 | 5,15 | GRUEN |
| **B12 Kopf Canny / Linien** | >= 25,2 / >= 4,6 % | 4,08/0,12 | **4,11/0,12** | **ROT** |
| B13 Canny Fleisch | 25,1-36,2 | 30,87 | 30,81 | GRUEN |
| B14 L*-Streuung Render/Atlas | >= 8,2 / >= 9,3 | 9,4/11,8 | 9,48/11,80 | GRUEN |
| B15 a*-Streuung Render | >= 4,25 | 6,5 | 6,54 | GRUEN |
| B16 Rotpixel in Flaechen mit >= 2 Stufen | >= 50,7 % | 80,7 | 81,8 (32 Flaechen) | GRUEN |
| **B17 Kohaerenz Render-Mittel / Atlas** | >= 0,60 / >= 0,576 | 0,543/0,562 | **0,5427 (0,547/0,551/0,530, Kachel 521 px) / 0,563** | **ROT** |

- Summe eigen: 43/45, gleich der Summe des Bauers. Die Abweichung zum Bauer liegt bei 0,0-0,3 (B2 0,33), kein Riegel kippt zwischen den Implementierungen.
- `n2_riegel.py` (Werkzeug des Pruefers) zeigt am selben Ordner B10-B17 mit 6/8, ROT sind B12 und B17 (`pruef_v2r2/n2_riegel_v2.log`).

## 3. Geometrie, Clips, Asset, Nachbau

- sha256 a71f8d08... ist gleich fuer `K5c/FORM.EMD`, `farbe/K5c_farbe_v2/FORM.EMD`, `farbe/K5C_FARBE_V2.EMD`, `scratchpad/formfarbe2/emd/K5C_FARBE_V2.EMD`, `farbe/K5c_farbe_r3/FORM.EMD` und `farbe/K5c_farbe_v2r1/FORM.EMD`. Geometrie, Clips und UV sind bytegleich.
- sha256 085bb433... ist gleich fuer die drei TIM-Kopien (Variante, farbe/, Scratchpad emd/).
- **Nachbau unabhaengig**: `KF4_CACHE=pruef_v2r2/kf4cache python kf_mal4.py bauen pruef_v2r2/nachbau` mit HEAD-Generator.
  - TIM und EMD sind laut cmp bytegleich.
  - Wichtig, weil kf_mal4.py (mtime 02:58:39) nach dem Bau `fin` (02:57:42) noch geaendert wurde. Die Aenderung wirkt nicht auf das Ergebnis.
- **Spiel-Asset**: `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` 16eb6ea8..., `.TIM` ec280802..., letzter Commit 67408cfcb. `git diff 7e9b314c7..HEAD -- re15_port` ist leer.

## 4. Tests

- Lauf: `ctest --test-dir re15_port/build -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`
- Ergebnis: **11/11 gruen** in 95,12 s, nichts wiederholt (`pruef_v2r2/ctest.log`). `ninja -n` meldet "no work to do".
- Die Tests pruefen nur das unveraenderte Spiel-Asset, ueber die Variante sagen sie nichts.

## 5. Gegenproben

Code und Eingaben sind gegen v2 r1 unveraendert (r3-TIM sha 7c719e12). Die Werte stammen deshalb aus `pruef_v2r1/eigen_*.json`.

- **r3**: B10 3,57, B11 3,04, B12 8,75/1,28, B13 18,35, B14-Render 5,44, B15 3,13, B16 0,0, B17 0,560/0,560 und B9 17,77 sind alle ROT, dazu K1v2/K2v2/K4b. B9b 10,0 ist gruen, das ist die Bezugsprobe nach V2 3.2.
- **G4**: B10-B17 komplett GRUEN (9,51 / 3,89 / 29,61+46,45 / 29,44 / 11,04+11,21 / 7,90 / 81,6 / 0,616+0,619).
- **EM30**: komplett GRUEN (12,51 / 4,49 / 29,68+5,39 / 29,69 / 9,60+10,87 / 5,02 / 61,1 / 0,6205+0,577).
- **K5c heute**: riegel_farbe.py meldet "Farbriegel, die dort NICHT rot sind: keine". Tatsaechlich sind dort A10-A12, B6, B7, B9, B9b, B11 und B13-B16 GRUEN. Damit trennt die Gegenprobe "K5c heute" bei den V2-Riegeln B11 und B13-B16 nichts (Hinweis wie in v2 r1, unveraendert).

## 6. Konstanten (Diff d8ea91597..HEAD -- generator/)

### 6.1 Geprueft und in Ordnung

| Konstante | Wert | Pruefung |
|---|---|---|
| c_f (Fleisch-Koernung) | 0,0 | Ergebnis der Halbierung (bau.json `halbierung_faktor` 0,0, `sdL_pool_ohne` 11,2 = Ziel 11,2). Keine Wahl mehr. |
| HELLKERN_ANTEIL | 0,50 nach Abstand | G4-Leitwert V2 1 (f) |
| Mehrheitsschwelle 0,5 | - | Definition (Mehrheit), zulaessig |
| Eichabbruch | 2 x 0,125 | kf_eich4.json `runden`: schritt_max 4,2 -> 1,1 -> 0,4 -> 0,10 < 0,25, g 2,22 |
| Farbton-Drehung | k 15,26 | sd 9,41 gegen Ziel 9,4 (bau.json). Das CLUT-Atlas-B7 liegt bei 10,30. |
| Textfehler aus v2 r1 | - | KOPF_QUER faellt weg, G_START ist beschrieben: berichtigt |

### 6.2 Mangel

1. **K_GRAPH 8 / SPIEGEL_ABSTAND 2,0: Nachweis nicht erbracht.**
   - Die Laeufe `farbe/unabh_v2r2/` habe ich gelesen: kg4 40/45 (K4 3,1, K1v2 6,6, K4b 3,8), kg16 41/45 (B8 10,0, B10 7,0), sp1 43/45 ohne Kippen, sp4 41/45 (N1 Seite 9,2, B2 Seite 3,18).
   - `kf_kgraph_mass.py` (`N2_kgraph.json`) beschreibt den Graphen (k 8 deckt 80 % der Atlas-8-Nachbarschaft), ist aber keine Quelle fuer den Wert 8.
   - Nach V2 4 ist damit keiner der beiden Werte zulaessig, das ist ein Nachweisfehler.
2. **KORN_SIG 1,0**: Das ist die Skala, mit der der Bauer seine Quelle (kf_korn_mass.py) selbst definiert hat, also ein Zirkel. Es gibt weder eine Messblatt-Zeile noch einen Unabhaengigkeitsnachweis.
3. **KORN_SD 2,30**:
   - Eigene Nachmessung (`korn.json`, scipy-Gauss je Mesh): G4 2,302, EM30 3,542. Der Wert stimmt.
   - V2 4 letzte Zeile verlangt aber "Zeile aus Abschnitt 1 dieses Blatts oder V1-Zeile". N2_korn.json ist eine neue Messung ausserhalb des Messblatts. Formal fehlt die Quelle, sachlich ist der Wert belegt.
   - Ergebnis im Atlas: Koerper-Haut 3,06 (in der Familie). Kopf 1,62 liegt **unter** dem Familienminimum 2,30 (r3-Kopf 2,31). Das passt zu B12 ROT.
4. **SIG_GROB 2,5** steht jetzt in der Tabelle. Die Quelle ist "PORT-WAHL nach E12" plus eine Ablesung im G4-Feld (130,96,20,62) ohne Zahl in V2 1. V2 4 schliesst PORT-WAHL fuer Malkonstanten aus, damit ist der Wert nur teilweise belegt.
5. **RUECKEN_ZIEL 0,18**:
   - Gewaehlt ist die Obergrenze des V2-Fensters 15-18 % nach Laeufen (f3/f7). Das ist eine Wahl im Fenster und keine Ableitung.
   - Realisiert sind 0,181 (bau.json "Anteil 0.181"), also knapp ueber 18 %.
   - Der Kommentar nennt "bei 18 % 9,8 % (Lauf f7)", der Endstand liegt bei K4b 8,8. Das ist eine veraltete Zahl (Textfehler).
6. **Zonenversaetze (V2 3.2)**:
   - a*/b* bleiben auf dem r3-Stand (`--nur-l`).
   - 8 Zonen liegen ueber 6 dE, am groessten Hinterbein r (W) mit 58,1 dE (b* -51,4) und Unterkiefer mit 52,9 dE (L* +41,9).
   - Die Begruendung im Dossier ist knapp ("Kette/Blau-Kappe ausgleichen"). Die Blau-Kappe trifft laut Nachbau-Log 30 073 Texel.
   - K3, K6, K7 und K8 sind mit 3 Saaten gruen, belegen aber vor allem die Eichung.

## 7. Tafeln

| Tafel | Befund |
|---|---|
| 03_familie | **fair**: gleicher Renderer und gleiches Licht, roh. V2-Beschriftung C* 11,0/9,5/13,4 deckt sich mit der eigenen Messung 11,00/9,50/13,35. |
| 01/02 | **fair**: Original, K4 heute und V2 unter gleichem Disc-Licht x k, Zeile 2 durch denselben Filter, kein Gamma. |
| 07 | **fair**: eigene exe-Kopie, `spiel_lauf.log` 10/10 rc=0. Die V2-Bilder unterscheiden sich von v2r1 (z. B. spiel_ruhe_r5080 e3ee5feb gegen 5814d088). Die r1-Panne (heute 5090 Pose = Ruhe, gleiche sha 2609f0ed) ist behoben. |
| 08 | **fair**: Der Ausschnitt ist je Modell auf die Anatomie gesetzt (Flanke/Schulter/Ruecken). Das ist eine Sichtwahl, laut Dossier keine Malgroesse; der r1-Mangel ist behoben. |

V1 7.5 ist nicht erfuellt (43/45), die Tafeln sind damit keine Abnahmevorlage. Der Bauer haelt das selbst ein.

## 8. Neuer Befund: Farbton des Bild-1-Rots (E14)

Gemessen mit `rotton.py`, Kette Saat 15, Rotklasse a* >= 12, C* >= 15, L* >= 20 wie K4:

| | Original | V2 | K4 heute |
|---|---|---|---|
| Ruecken/Flanke Rot-Lab | (38,1 / 19,6 / -4,1), h -11,8 Grad, b*<0 81 % | (33,1 / 16,9 / 7,4), h +23,5 Grad, b*<0 0 % | (50,3 / 18,2 / -4,2), h -13,1 Grad |
| Figur Rot-Lab | (36,7 / 19,6 / -3,3), h -9,6 Grad | (29,1 / 16,2 / 12,7), h +38,1 Grad | (44,5 / 17,9 / -2,5), h -8,0 Grad |

- **Menge**: Die Rotmenge stimmt (K4, K4b).
- **Farbton**: Der Farbton liegt 35-48 Grad daneben, dE der Mittel 12,8 bzw. 18,0. Im Spielbild 5080 (Tafel 07) ist das als orange-roter statt rosa-violetter Ruecken sichtbar.
- **Verstoss gegen E14**: E14 verlangt "Fleischrot nur wo der Screenshot Rot/Rosa zeigt", also Farbton im Modell, den der Screenshot dort nicht hat, nahe 0.
- **Ursache im Messblatt**: V2 3.2 gibt als Textur-Ziel "Farbton 34 Grad" (Familienwunde) vor. Das Original zeigt am Ruecken -12 Grad.
- **Riegel**: Kein Riegel misst das. K3 vergleicht nur das Zonenmittel und bleibt mit 3,0 gruen.
- **Einordnung**: Das ist kein Toleranzverstoss, aber eine Luecke im Messblatt, die der Nutzer sehen wird.

## 9. Was zum Bestehen fehlt

| # | Punkt | Abstand / Bedarf |
|---|---|---|
| 1 | B12 Kopf | Canny 4,1 -> 25,2, Linien 0,12 -> 4,6 %. Laut V2r2.4 nur mit Fell, Rinde oder Zebra erreichbar. Entweder Messblatt-Entscheid (B12 gegen Sicht P2/Bild 2) oder ein anderer Kopf. |
| 2 | B17 | Render 0,543 -> 0,60, Atlas 0,563 -> 0,576. Nach V2 6.3 nur dann Sichtentscheid, wenn B17 allein rot ist. |
| 3 | K_GRAPH / SPIEGEL_ABSTAND | Nachweis V2 4 (kg4/kg16/sp4 kippen) |
| 4 | KORN_SIG, KORN_SD | Quelle im Messblatt (Zeile in V2 1) oder Nachweis |
| 5 | Hinweise | E14-Farbton Bild-1-Rot (8.); RUECKEN_ZIEL-Kommentar; Gegenprobe K5c heute; Commit-Trailer "Claude Opus 5.5" statt "Claude Fable 5.1" in allen 5 Commits (Formfehler, vom Bauer als O4 offen gefuehrt) |
