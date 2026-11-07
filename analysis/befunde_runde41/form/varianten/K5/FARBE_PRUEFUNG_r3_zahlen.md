# Pruefung K5c Farbe Runde 3, Zahlen-Pruefer (2026-11-06)

Geprueft: Commits 0dbf690d2, 3b2d51773, d656d660b, 7e9b314c7 auf r41/form (HEAD 7e9b314c7, Arbeitsbaum sauber).
Eigener Code (nicht committet): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/pruef_r3/code/`
(eigen.py EMD/TIM-Leser, geo.py, atlas.py A1-A12, kette.py K1-K10, nb.py N1/N2/B1-B3/B9, b45.py B4/B5, b67.py B6/B7, b2q.py, kiefer.py, e14.py, g4q.py).
Statistik eigen (float-Lab, eigene Texelsammlung); Renderer und Kette nach Messblatt-Definition (`render.blick`, `form_pruefung.Kandidat.rendern`, `form_video`).
`riegel_farbe.py` / `p_riegel.py` wurden nicht ausgefuehrt.

## Urteil: NICHT BESTANDEN

Mit eigenem Code bestaetigt ROT: **K1, K2, K4, B2, B9** (5 von 33, wie der Bauer). Dazu fehlen Quellen fuer mehrere ergebnisrelevante
Konstanten (Abschnitt 5), und vier Ketten-Riegel sind durch die Eichung direkt eingepasst (Abschnitt 6).

## 1. Riegel nachgemessen

| Riegel | Soll | eigen neu | Bauer | eigen K5c heute | Bewertung |
|---|---|---|---|---|---|
| A1 C* Koerper | 9,5-21 | 13,90 | 13,9 | 4,88 | GRUEN |
| A2 sigma_ab je Teil (26 Teile) | >= 7,0 | 7,61 (min 1,72) | 7,6 | 3,94 | GRUEN, knapp |
| A3 Fleischrot | 8-40 % | 20,30 | 20,3 | 0,51 | GRUEN |
| A4 Rot-Farbton | 15-50 Grad | 22,2 (p10/50/90 0,1/24,5/30,6) | 22 | -9,9 | GRUEN; 26 % der Rottexel unter 15 Grad |
| A5 Neutralweiss Koerper | <= 0,20 % | 0,00 | 0,00 | 0,80 | GRUEN |
| A6 HF_fft K/Ko/Ki/Kr | 0,04-0,26 | 0,090/0,168/0,073/0,065 | 0,090/0,167/0,073/0,065 | Kiefer 0,462 | GRUEN |
| A7 HF_rest | <= 0,19 | max 0,082 | 0,082 | 0,437 | GRUEN |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20/>=20/>=20 % | 6,91/25,33/39,61 | 6,9/25,3/39,6 | 77,1/0/0 | GRUEN |
| A9 Auge L*/C* | <= 30/<= 30 | 13,3/17,9 | 13,1/18,0 | 56,3/53,5 | GRUEN |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 | gleich | gleich | GRUEN (UV bytegleich) |
| A11 Schwanzsprung 1-5 | <= 4,0 | 0,14 (Glieder 14,3/14,3/14,4/14,3/14,4, Spitze 29,3) | 0,1 | 3,35 | GRUEN, per Konstruktion (6.2) |
| A12 Farbkorn | <= 5,7 | 2,48 | 2,48 | 2,62 | GRUEN |
| **K1 Bild 1 C*** | 7,87-11,10 | **6,09** (Saat 12345: 6,12) | 6,1 | 6,31 | **ROT** |
| **K2 Bild 1 ab-Streuung** | >= 8,36 | **6,23** (6,29) | 6,2 | 6,35 | **ROT** |
| K3 Bild 1 dE je Zone | <= 6 | max 4,89 (Hinterbein r (W)) | 4,9 | 8,24 | GRUEN (eingeeicht) |
| **K4 Bild 1 Rotklasse** | 3,15-7,87 % | **2,75** (2,71) | 2,8 | 2,49 | **ROT** |
| K5 Ruecken hell / geclippt | <= 14,9 / <= 0,80 % | 11,73 / 0,35 | 11,7/0,3 | 21,9/1,02 | GRUEN |
| K6 Bild 2 dE / C* | <= 6 / >= 21,75 | 1,09 / 24,66 | 1,0/24,7 | 9,86/16,7 | GRUEN (eingeeicht) |
| K7 Unterkiefer dE / a* | <= 6 / >= 21,16 | 1,57 / 23,33 | 1,6/23,3 | 12,3/13,7 | GRUEN (eingeeicht, 6.1) |
| K8 Oberkiefer dE | <= 6 | 0,34 | 0,3 | 12,1 | GRUEN (eingeeicht) |
| K9 Klauen a* r/l (W) | >= 18,08/16,64 | 25,70/28,84 | 25,7/28,8 | 13,8/22,8 | GRUEN |
| K10 Augen L* max | <= 30 | 13,99/15,43 (r 8 px: 18,5/16,0) | 14,0/15,4 | 51,5/36,2 | GRUEN |
| N1 neutral C* | >= 7,43/9,35/7,80 (0,75 x EM30 9,91/12,47/10,40) | 10,28/9,64/10,11 | 10,3/9,7/10,1 | 4,58/4,88/3,97 | GRUEN (Seite knapp) |
| N2 neutral L*>85 | <= 0,10 % | 0/0/0 | 0/0/0 | 0,81/1,29/0,11 | GRUEN |
| B1 Rot erosionsfest | >= 32,1 % | 36,7 | 35,9 | 21,6 | GRUEN |
| **B2 groesste Rotflaeche 3/4 vorn / Seite** | >= 3,2 % | **2,88** / 4,74 | 2,97/5,96 | 1,21/0,57 | **ROT** |
| B3 Blau-Klasse | <= 0,10 % | 0,00/0,00/0,00 | 0,03/0,02/0,00 | 3,97/5,03/2,41 | GRUEN |
| B4 Glanzrest p90 (493 Keime) | <= 58 | 44,7 (max 60,9) | 44,3 | 100 | GRUEN |
| B5 Kopffeld Zahnklasse | <= 2,0 % | 0,00 (0/12 992) | 0,0 | 15,06 | GRUEN |
| B6 Wiederholung | <= 0,28 % | 0,04 (2/4497; bei RMS < 2,0: 1,07) | 0,04 | 0,36 | GRUEN |
| B7 Rot-Farbton-Streuung | >= 5,8 Grad | 11,4 (zirkulaer 11,4) | 11,3 | 19,9 | GRUEN |
| B8 Bild-2-Zahnflecken | 13,6-33,9 % | 14,1 (Saat 12345: 14,2) | 14,1 | 41,1 | GRUEN, knapp |
| **B9 Rot im Umrissband** | <= 11,3 % | **17,52** (je Winkel 17,5/20,7/14,1) | 17,8 | 10,37 | **ROT** |

- B2 Seite haengt an der Lab-Quantisierung: float-Lab 820 px (4,74 %), uint8-Lab (fl.lab) 1031 px (5,96 %), weil zwei Flecken verschmelzen. 3/4 vorn ist in beiden Rechnungen ROT (2,88 / 2,97).
- B9 haengt an der Pose: Clip 0 (Bild-1-Pose) ergibt 10,0 %, Clip 2 (Ruhe, Riegeldefinition) 17,5 %.
- Familie mit eigenem Code: B9 EM30 11,13 / G4 7,69 / G5 7,23 %; B1 EM30 32,6 %; N1 EM30 9,91/12,47/10,40. Die Grenzen stimmen.
- Ketten-Ursache K1/K2/K4 (eigene Zonenwerte): Ruecken/Flanke Rotklasse 2,3 % gegen Original 11,9 %, C* 6,5 gegen 10,5; Hinterbeine C* 3,5/5,3 gegen 8,1/10,4; Vorderglied C* 4,7 gegen 5,9.

## 2. Geometrie, Clips, Spiel-Asset

- FORM.EMD sha256 a71f8d08... gleich fuer K5c/FORM.EMD, farbe/K5c_farbe/FORM.EMD, farbe/K5C_FARBE.EMD, Scratchpad emd/K5C_FARBE.EMD.
- Eigener Parser: 32 Meshes, 3828 Primitive, 6386 Dreiecke in beiden. Vertex-Diff 0,0, Primitiv-/UV-Bytes 0 Unterschiede. EMD-Abschnitte 0-7 (Skelett, Clips, Modell) bytegleich.
- FORM.TIM sha256 7c719e12... in allen drei Kopien. 8 bpp 512 x 256, CLUT 4 x 256 bei (0,480); 85 948 genutzte Texel, keiner mit mehr als einer CLUT-Zeile, keiner auf Index 0, ungenutzte Texel 0, keine STP-Bits.
- Nachbau `python kf_mal3.py bauen <scratchpad>/pruef_r3/nachbau`: FORM.TIM und FORM.EMD **byte-gleich** (cmp).
- Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`: Blob 1fba8a34 / 4e7db099 in 19a9b2b17 und HEAD gleich; `git diff 19a9b2b17..HEAD -- re15_port` leer.

## 3. Tests

- `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK.
- `ctest --test-dir re15_port/build -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`: **11/11 bestanden**, 95,0 s, nichts wiederholt.
- Die Tests pruefen das unveraenderte Spiel-Asset, nicht die neue Textur. Spiellauf `farbe/spiel_lauf.log`: 10/10 rc=0, gdigrab Versuch 1; Kandidat geladen ("3 Clips"), Vergleich "28 Clips".

## 4. Gegenproben (eigener Code)

- K5c heute: ROT bei A1-A9, K1-K10, N1, N2, B1-B5, B6 (eigen 0,36 %, Bauer 0,24 %), B8 (41,1 %). GRUEN nur A10-A12, B7 (19,9), B9 (10,4).
  K5c heute ist im Riegel klar rot. B7 und B9 trennen K5c heute nicht; sie trennen Runde 2.
- Runde 2: B6 3,06 %, B7 4,6 Grad, B9 23,2 % = ROT (B8 0,2 % aus Pruefung r2). Runde 1: B1 20,1 %, B2 3,23/1,65 %, B3 11,6-16,2 % = ROT.

## 5. Konstanten im Generator-Diff (3215ec1da..HEAD -- generator/; neu in Runde 3: kf_mal3.py, riegel_farbe.py B6-B9, kf_tafeln.py Kopfzeile)

**Mit Quelle, nachgeprueft:**
- EINSTREU_B2 3,4 = p_zonen.json K5c Bild 2 Albedo b* -1,5 -> Kette b* 1,9.
- L_MAX_FLEISCH 70,6 = EM30-Haut p95 (Messblatt 1); L_MAX_HAUT 58 (Messblatt 2).
- RAMPE_FLEISCH: fuenf Stufen = Messblatt-1-Paletten G4/EM30; Familienwunde (51,5/26,5/18,0) = Mittel (50/26/13) und (53/27/23).
- FLEISCH_BAUCH_L 34,65; L_STREU_ZIEL 14,9 (G4); G_AB 1,48 (9,3/6,3); C_BLAU 8 (B3-Klasse); Schwelle L* < 20 (Messblatt-2-Klasse).
- B6/B7/B9-Grenzen: Familie mit eigenem Code bestaetigt.

**Ohne Quelle oder mit falscher Quelle (Nachweis fehlt):**
1. Als "Verfahrensparameter" bezeichnet, weder gemessen noch PORT-WAHL mit Nutzerentscheid:
   - K_NAH/K_FERN 64/256, K_GRAPH 8, SPIEGEL_ABSTAND 2,0.
   - Faltenuebergang "1 Standardabweichung" (`np.clip(hz,0,1)`).
   - Koernung im Fleisch auf +-1 sigma begrenzt (kf_mal3.py ~471).
   - Eichung: Saettigungsregel 0,1, Abbruch 1 dE, Fenster 0,95-1,05.
   Die Fleisch-Koernung (+-1 sigma) wurde laut Kommentar nach Riegel-Ergebnissen gewaehlt (Laeufe v7/v8: B1, B6).
2. L_STUFEN (0,10,15,20,25,30,40,60,100): Der Kommentar nennt die Messblatt-2-Klassen (20 und 45), aber 45 fehlt in der Liste.
3. Zahnfleisch-Gewicht 21,5 "G4-Maul a* p90, gemessen": Gemessen wurde am lueckengefuellten Ausschnitt (`kf_mal.ausschnitt`). An den genutzten G4-Texeln von (10,44,60,40) ist p90 = **19,30** (Zahn 1,7 / Rot 8,7 / Dunkel 47,6 % statt 1,5/10,5/46,1).
4. Mehrheitsschwelle 0,5 fuer die Hautseite warm/kuehl wird mit E14 begruendet. E14 regelt nur, wo Fleischrot erscheint, nicht die Wahl der Hautseite.
5. Weiter offen aus r2 (O6): `kf_mal.ausschnitt` (sigma 2, 4 Durchlaeufe) und Kopienzahl `+1` fuer Kiefer und Krallen.
6. Textfehler im Code:
   - Kommentar in Schritt 9 nennt "-55..80 Grad", gerechnet wird mit H_MIN -125.
   - bau.json 'hautseiten' sagt "k-Mittel Bild 1", gerechnet wird ueber Bild 1 und Bild 2 (O9).

## 6. Weitere Befunde (nicht Riegel, aber pruefrelevant)

1. **Eingeeichte Riegel:** Die Zonenversaetze sind Original minus Kette, also genau K3/K6/K7/K8. Deren dE 0,2-1,6 belegt deshalb nur die Eichung, nicht die Malweise.
   - Die Versaetze sind gross: Unterkiefer +43,3 L*, Hinterbein r (W) -17,0 L* und b* -51,4 (gesperrt), Hinterbein l (W) b* -31,6.
   - Folge im Atlas: Das sichtbare Unterkieferband (1132 Texel) hat L* 67,2 (p90 87,1). Davon sind 14,6 % L* > 85 und 3,5 % neutralweiss (L* > 85, C* < 10).
   - Vergleich: Screenshot-Band T L* 50,9, Familie Kiefer L* 34-39. Kein Riegel erfasst das, A5 misst nur den Koerper.
2. **A11 per Konstruktion:** Schritt 8 setzt die Glied-L* 1-5 auf ihr gemeinsames Mittel (vorher 14,3/6,8/10,1/18,0/21,7). Die Spitze springt um +14,9 L*.
   Gelenknaehte im Render sind nicht gemessen.
3. **Eichung nicht sicher konvergiert:**
   - Schritte der Runden 9-14: 1,20 / 4,96 / 1,77 / 4,96 / 1,10 / 0,71. Abgebrochen wurde beim ersten Wert unter 1.
   - Rest-dE max 7,9 (Klaue l (W), kein Riegel).
   - K1 lag in keiner der 15 Runden ueber 6,9.
4. **E14** ("Fleischrot nur, wo der Screenshot Rot zeigt", Mass nahe 0). Gemessen an Bildpunkten, die auf sichtbare Texel abbilden (Bild 2 mit Einstreuung -12,8/-3,4):
   - Vorderglied r (W), die Z5-Stelle aus E14: Atlas-Rot 2,6 %, Screenshot 0,0 % (K5c heute 0,3 %).
   - Ruecken/Flanke: Atlas-Rot an 6,6 % der Bildpunkte, wo der Screenshot a* < 6 zeigt.
   - Kopf/Rumpf 9,2 %, Oberkiefer 9,5 %, Klaue r (W) 13,6 %.
   - Ursache: Dunkelrote Falten (G4-Dunkelrot, 9,6 % der Haut) und die Fleischrampe liegen auch auf gesehenen Flaechen.

## 7. Tafeln (Darstellungsregel Messblatt 7)

- Kein einseitiges Gamma. kf_tafeln.py rechnet kein Gamma; jede Kachel ist "roh" beschriftet.
- 01/02: Disc-Licht x k gleich fuer K4 heute und neu; Kopfzeile sagt, dass Zeile 1 heller ist als das Spielfenster.
- 03: gleicher Renderer und gleiches Licht. Die Beschriftung der neuen Kachel (L* 18,2/15,9/16,4, C* 10,3/9,7/10,1, L*>85 0 %) stimmt mit meiner eigenen Messung ueberein.
- Maengel:
  - Die Kopfzeile von 01 ist am rechten Rand abgeschnitten ("... Sicht-P").
  - Vergleichsmodell in 01-03 ist "K4 heute", nicht die abgelehnte K5c-Textur. Nach K5.md gleicher Koerper, aber nicht dasselbe Bild, das der Nutzer abgelehnt hat.
  - Messblatt 7.5: Die Tafeln duerfen nicht zum Nutzer, solange Riegel ROT sind (der Bauer sagt das selbst).
