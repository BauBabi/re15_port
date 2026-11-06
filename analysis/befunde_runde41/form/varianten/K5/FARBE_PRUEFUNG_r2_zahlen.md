# Pruefung K5c Farbe Runde 2, Zahlen-Pruefer (2026-11-06)

Geprueft wurden die Commits acdde09a4, 158d04e45, 17977866a und be2ad8190 auf r41/form (HEAD be2ad8190).
Die Variante `farbe/K5c_farbe/` ist identisch mit `farbe/K5C_FARBE.*` und mit Scratchpad `formfarbe/emd/K5C_FARBE.*`.

Eigener Code (nicht committet) liegt in `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/pruef_r2/code/`:
- `eigen.py`: eigener EMD- und TIM-Leser.
- `geo.py`: Geometrievergleich.
- `atlas.py`: Riegel A1-A12.
- `kette.py`: Riegel K1-K10.
- `nb.py`: N1/N2 und B1-B3 mit eigener Statistik, dazu die Familie.
- `b45.py`: B4/B5.
- `blau.py`, `g4q.py`, `zusatz.py`: Zusatzmessungen.

Was ich vom Messblatt uebernommen habe und was eigen ist:
- Renderer fuer N/B und fuer die Kette: Definition des Messblatts (`render.blick`, `form_pruefung.Kandidat.rendern`, `form_video`).
- Statistik: eigener Code (float-Lab), nicht `riegel_farbe.py` oder `p_riegel.py`.

## Urteil: NICHT BESTANDEN

Mit eigenem Code bestaetigt sind fuenf Riegel ROT: **K1, K2, N1 (Seite und 3/4 hinten), B1, B2**. Die uebrigen 24 Riegel sind GRUEN.
Ausserdem stehen Konstanten ohne Quelle im Code, die das Ergebnis beeinflussen (Abschnitt 5).

## 1. Riegel nachgemessen

| Riegel | Soll | eigen neu | Bauer | eigen K5c heute | Bewertung |
|---|---|---|---|---|---|
| A1 C\* Koerper | 9,5-21 | 11,84 | 11,8 | 4,88 | GRUEN |
| A2 sigma_ab je Teil (26 Teile) | >= 7,0 | 13,10 (min 9,04) | 13,1 | 3,94 | GRUEN |
| A3 Fleischrot | 8-40 % | 22,43 % | 22,4 | 0,51 | GRUEN |
| A4 Rot-Farbton | 15-50 Grad | 26,1 | 26 | -9,9 | GRUEN; die Streuung betraegt aber nur 4,6 Grad (6.2) |
| A5 Neutralweiss | <= 0,20 % | 0,00 % | 0,00 | 0,80 | GRUEN |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,04-0,26 | 0,091/0,151/0,047/0,064 | 0,091/0,153/0,047/0,064 | Kiefer 0,462, Kralle 0,322 | GRUEN |
| A7 HF_rest | <= 0,19 | max 0,065 | 0,065 | 0,437 | GRUEN |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >= 20 / >= 20 % | 6,13/95,74/39,23 | 6,1/95,7/39,2 | 77,1/0/0 | GRUEN; Rot liegt mit 95,7 % weit ueber der Familie (21-36 %), siehe 6.3 |
| A9 Auge L\*/C\* | <= 30 / <= 30 | 13,3/17,9 | 13,1/18,0 | 56,3/53,5 | GRUEN |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 | gleich | gleich | GRUEN (UV bytegleich) |
| A11 Schwanzsprung | <= 4,0 | 1,46 (Glieder 11,5/12,7/14,1/15,5/16,7, Spitze 19,6) | 1,4 | 3,35 | GRUEN |
| A12 Farbkorn | <= 5,7 | 4,53 | 4,54 | 2,62 | GRUEN |
| **K1 Bild 1 C\*** | 7,87-11,10 (0,85-1,2 x 9,25) | **6,51** | 6,5 | 6,31 | **ROT** |
| **K2 Bild 1 ab-Streuung** | >= 8,355 (0,85 x 9,83) | **7,99** | 8,0 | 6,35 | **ROT** |
| K3 Bild 1 dE je Zone | <= 6 | max 5,37 (Hinterbein r (W)) | 5,4 | 8,24 | GRUEN |
| K4 Bild 1 Rotklasse | 3,15-7,87 % | 6,19 % | 6,2 | 2,49 | GRUEN |
| K5 Ruecken hell / geclippt | <= 14,9 / <= 0,80 % | 4,75 / 0,36 % | 4,7/0,4 | 21,9/1,02 | GRUEN |
| K6 Bild 2 dE / C\* | <= 6 / >= 21,75 | 1,49 / 23,84 | 1,5/23,8 | 9,86/16,7 | GRUEN |
| K7 Unterkiefer dE / a\* | <= 6 / >= 21,16 | 0,06 / 24,94 | 0,0/24,9 | 12,3/13,7 | GRUEN (Eichziel, siehe 6.4) |
| K8 Oberkiefer dE | <= 6 | 1,41 | 1,4 | 12,1 | GRUEN |
| K9 Klauen a\* r/l (W) | >= 18,08 / 16,64 | 26,1/28,3 | 26,1/28,3 | 13,8/22,8 | GRUEN |
| K10 Augen L\* max | <= 30 | 14,3/15,9 (bei r 8 px: 19,8/19,1) | 14,3/15,9 | 51,5/36,2 | GRUEN |
| **N1 neutral C\*** | >= 0,75 x EM30 (9,91/12,47/10,40, also 7,43/9,35/7,80) | 10,16 / **8,66 / 6,82** | 10,2/8,8/6,9 | 4,58/4,88/3,97 | **ROT** (Seite, 3/4 hinten) |
| N2 neutral L\*>85 | <= 0,10 % | 0/0/0 | 0/0/0 | 0,81/1,29/0,11 | GRUEN |
| **B1 Rot erosionsfest** | >= 32,1 % | **29,1 %** (je Winkel 30,1/33,8/13,4) | 28,0 | 21,6 | **ROT** |
| **B2 groesste Rotflaeche / Figur** | >= 3,2 % (3/4 vorn, Seite) | 3,70 / **2,43 %** (3/4 hinten 0,96 %) | 3,65/2,45 | 1,21/0,57 | **ROT** |
| B3 Blau-Klasse | <= 0,10 % | 0,00/0,00/0,00 | 0,00 | 3,97/5,03/2,41 | GRUEN |
| B4 Glanzrest p90 L\* (493 Keime) | <= 58 | 48,0 (p50 37,9, max 63,7; Ring 2-4 Texel L\* 30,2) | 47,8 | 100 | GRUEN |
| B5 Kopffeld Zahnklasse | <= 2,0 % | 1,02 % (133/12 992) | 1,02 | 15,06 | GRUEN |

**Robustheit:** Mit Saat 12345 ergibt die Kette dieselbe Einstufung (K1 6,51, K2 7,96, K10 15,8/13,7).

**Familie, eigene Statistik** (B-Grenzen nachgerechnet):

| Modell | B1 | B2 |
|---|---|---|
| EM30 | 32,6 % | 3,57-4,77 % |
| G4 | 41,6 % | 3,30-12,35 % |
| G5 | 52,2 % | 12,3-14,2 % |

Die Grenzen 32,1 und 3,2 stimmen.

**Gegenproben, eigene Messung:**
- K5c heute: ROT bei A1-A9, K1-K10, N1, N2 und B1-B5. GRUEN sind nur A10-A12, also die Halteriegel.
- Runde 1: B1 20,1, B2 3,23/1,65, B3 11,6-16,2 %, B4 80,2, B5 9,0 %. Alle B-Riegel sind dort ROT.
- Der Riegel misst also etwas.

**Ausweg O1 des Bauers geprueft:**
- Original Bild 1 mit b\* auf -6 gekappt ergibt C\* 7,99 und ab-Streuung 8,57. Die Bauer-Zahl 7,99 ist damit bestaetigt; die Blau-Klasse umfasst 29,9 %.
- Auch gegen diese gelockerte Grenze (0,85 x 7,99 = 6,79) bleibt K1 mit 6,51 **ROT**. Nur K2 waere gruen (Grenze 7,28).
- Die Luecke sind die Beine. Hinterbeine und Vorderglied haben in der Kette C\* 3,7-4,8 und ab-Streuung 3,8-5,7.
- Zum Vergleich das Original: ohne Blau C\* 4,7-6,2, mit Blau C\* 5,9-10,4 und ab-Streuung 6,2-8,6.

## 2. Geometrie, Clips, Spiel-Asset

**FORM.EMD**
- sha256 a71f8d08... ist gleich fuer K5c/FORM.EMD, farbe/K5c_farbe/FORM.EMD, farbe/K5C_FARBE.EMD und die EMD im Scratchpad.
- Eigener Parser: 32 Meshes, 3828 Primitive, 6386 Dreiecke in beiden Fassungen.
- Vertex-Diff 0,0; Primitiv- und UV-Bytes ohne Unterschied.
- EMD-Abschnitte 0-7 (Skelett, Clips, Modell) sind bytegleich.

**FORM.TIM**
- sha256 426819be... ist in allen drei Kopien gleich.
- 8 bpp, 512 x 256, CLUT 4 x 256 bei (0,480).
- 85 948 genutzte Texel. Keiner nutzt mehr als eine CLUT-Zeile, keiner liegt auf Index 0.
- Ungenutzte Texel stehen unveraendert auf 0; keine STP-Bits gesetzt.

**Nachbau**
- `kf_mal2.py bauen` mit kf_eich2.json, Ausgabe in den Scratchpad.
- FORM.TIM und FORM.EMD sind **byte-gleich** (cmp).
- Masken: glanz 881, gaumen 2324, ader 4449, fleisch 17 210.

**Spiel-Asset** `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`
- Blob 1fba8a34 / 4e7db099, gleich in 19a9b2b17, HEAD und Arbeitsbaum.
- `git diff 19a9b2b17..HEAD -- re15_port` ist leer.

Hinweis zum Bestand (nicht diese Runde): Die Variante hat 3 Clips, das Spiel-Asset 28. spiel.log meldet "3 Clips", spiel_heute.log "28 Clips".

## 3. Tests

- Bau: `bash re15_port/tools/local_build.sh build` ergibt LOCAL-BUILD-OK.
- `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`: **11/11 bestanden**, 95,2 s, nichts wiederholt.
- Die Tests pruefen den unveraenderten Bestand, nicht die neue Textur.

## 4. Quellen der G4-Rechtecke (eigene Messung am G4-Atlas)

**Haut:** Fuenf Rechtecke, alle 100 % gedeckt. Die Rotanteile stimmen mit den Bauer-Zahlen 6/9/7/0/9,5 % ueberein.

| Rechteck | Rot |
|---|---|
| (130,96,20,62) | 6,2 % |
| (190,96,20,60) | 8,7 % |
| (314,12,32,30) | 6,8 % |
| (269,4,24,32) | 0,0 % |
| (29,152,24,32) | 9,5 % |

**Fleisch:** Zwei Rechtecke, beide 100 % gedeckt; die Werte stimmen.

| Rechteck | Rot | Lab |
|---|---|---|
| (181,224,20,30) | 83,8 % | (33,4/24,6/11,4) |
| (179,229,24,24) | 88,5 % | (37,2/25,6/11,6) |

- Die beiden Fleischquellen ueberlappen sich fast vollstaendig (x 179-202, y 224-253). Alles Fleisch (17 210 Texel) stammt also aus einem G4-Fleck von etwa 24 x 30 Texeln.
- Maul ist zu 83,8 % gedeckt, Krallen zu 81,7 %.
- Die Suche, die diese Rechtecke gefunden hat, liegt nicht im Repo. Das Dossier sagt nur "Suche im G4-Atlas".

## 5. Konstanten im Generator-Diff (3215ec1da..HEAD -- generator/)

**Mit Quelle.** Geprueft gegen Messblatt, FARBE_PRUEFUNG_r1_sicht.md und Familienmessung:
- SIG_GROB, G_AB, GLANZ_L/C, HOF_L 58, GAUMEN_L/C
- ADER_L 23/27 (Sicht r1 P1), ADER_CL 1,50
- FLEISCH_ZIEL (Sicht r1 P5), FLEISCH_ANTEIL 0,239, FLEISCH_UMRING 1,2
- G4_HAUT2 mit der 10-%-Grenze, G4_FLEISCH, G4_DUNKELROT
- EINSTREU_A2 12,8
- H_MIN/H_WARM/H_MAX, C_VIO, C_REST, B_MIN
- ZAHN_GRUND, ZIEL_AUGE
- die Grenzen von B1-B5

**PORT-WAHL:**
- FLEISCH_SCHWELLE 0,5: Mehrheit nach E14.
- SIG_AB_FEIN 0,7: E13 begruendet, dass geglaettet wird; der Wert 0,7 kommt aber aus den Laeufen v0-v5.

**Ohne Quelle, im Ergebnis wirksam:**
1. `kf_mal.ausschnitt` (von kf_mal2 fuer Maul und Krallen benutzt) fuellt Luecken mit `grob(P, m, 2.0)` und `for _ in range(4)`.
   - So gefuellt werden 16,2 % der Maul-Quelltexel und 18,3 % der Krallen-Quelltexel.
   - Weder sigma 2,0 noch die 4 Durchlaeufe sind belegt.
2. `kf_mal2.kachel_reihe`: `n_st = h // max(P.shape[0] - blend, 1) + 2`.
   - Das "+2" legt die Anzahl der Stuecke fest.
   - Damit bestimmt es die Zeilenversaetze `(j * Hoehe) // n_st` jedes Hautfelds.
3. Kiefer und Krallen: `kop = -(-ww // (Quellbreite - blend)) + 1`. Das "+1" legt die Kopienzahl fest und damit den Zeilenversatz.
4. `kf_mal.palette`: k-Mittel mit 50 Iterationen, eps 0,1 und 2 Versuchen.
   - Das sind Verfahrensparameter; sie wirken auf die CLUT.
   - Belegt sind sie nicht, aber unkritisch.
5. DAEMPF 0,8 gilt laut Code als "Weg, nicht Ziel". Die Eichung ist aber nicht konvergiert (Restfehler dE max 8,0).
   - Sie lief ueber 67 Runden aus Laeufen mit wechselndem Code.
   - Die Endversaetze haengen damit vom Laufverlauf ab.
   - Der Bau aus kf_eich2.json ist bytegleich wiederholbar, die Eichung selbst nicht.

**Altlasten aus Runde 1:** Die Konstanten ohne Quelle in kf_mal.py stehen weiter im Diff (`clip(g,0,2.5)`, L\* 70/0,25, `7*i`, `11*j`, `9*j`, `auf_hoehe(5)`, D_MAX).
kf_mal2 ruft sie nicht auf; fuer Runde 2 wirken sie nicht.

## 6. Sachbefunde jenseits der Riegel

1. **Kacheln wiederholen sich.**
   - 3,1 % der 6x6-Bloecke im Koerper haben einen fast gleichen Doppelgaenger (RMS < 1,5 L\*, mehr als 12 Texel entfernt).
   - EM30 0,3 %, K5c heute 0,4 %: das ist etwa das Zehnfache der Familie.
   - Ursache: ein G4-Fleck fuer alles Fleisch und fuenf kleine Hautquellen fuer 46 Felder. O5 des Bauers ist damit nicht "klein".
2. **Rot ist ein einziger Ton.**
   - Rottexel-Farbton p10-p90 21,4-30,5 Grad, Streuung 4,6 Grad. EM30: 9,5-36,0 Grad, Streuung 13,8.
   - 15,4 % aller Koerpertexel liegen bei C\* > 14,5 im Farbton 24-28 Grad (EM30 4,8 %).
   - Ursache ist die harte Drehung auf H_WARM 25.
   - A4 ist gruen, aber nur als Mittelwert. Das widerspricht E10 ("mehr als eine Farbe").
3. **Das Maul ist einfarbig rot.**
   - Kiefer-Atlas: Rot 95,7 % (Familie 21-36 %), Lab (33,1/35,5/22,3), C\* 42.
   - Kette Bild 2, Unterkiefer: 93 % Rotklasse (Original 77 %), Hellklasse 0,2 % (Original 22,6 % Zahnflecken), ab-Streuung 8,5 (Original 21,2).
   - Die Zaehne sind in Bild 2 nicht zu sehen.
4. **Die K-Riegel sind Eichziele.**
   - Die Zonenversaetze sind direkt auf die Pruefer-Kette geregelt. Unterkiefer: L\* +82,4, dE 0,06.
   - K3 und K6-K10 sind deshalb kein unabhaengiger Beleg.
   - Unabhaengig sind A, N und B. Davon sind N1, B1 und B2 ROT.
5. **Die Rueckansicht hat kaum Fleisch.**
   - 3/4 hinten: B1 13,4 %, groesste Rotflaeche 0,96 % (EM30 24,9 / 3,02 %, G4 53,2 / 11,2 %).
   - Im neutralen Render sehr dunkel: L\* 14,5-16,6 (EM30 24,4-28,9, G4 17,0-21,5).
6. **Rest im Kopffeld.**
   - x 500-511 (1212 Texel) hat Lab (41,6/7,2/7,9), der uebrige Kopf (21,8/7,3/1,0).
   - Das ist der aufgefuellte Gaumenstreifen: heller als der Kopf, aber unter der Zahnklasse.
   - Sichtbar ist er in Bild 1/2 nur mit 0/3 Bildpunkten.
7. **Z1-Fleck.**
   - In Tafel 01 ist der Rueckenfleck in beiden Zeilen scharlachrot. Im Original ist er rosa-violett (O3 des Bauers).
   - Bestaetigt: Rosa/Magenta-Kandidaten im Koerper 0,00 %.

## 7. Tafeln (Messblatt 7)

**Kein Gamma.** Alle Kacheln laufen ueber `render.blick(..., None)`, jede Kachel ist mit "roh" beschriftet.

**Einzelne Tafeln:**
- 01/02: Original, K4 und neu mit demselben Disc-Licht x k; der Faktor k steht in der Beschriftung.
- 03: alle Modelle mit demselben Renderer und demselben Licht, je Kachel L\*/C\*/L\*>85/px.
- 05: EM30 als Vergleich.
- 06: Ausschnitt x 200-360, y 290-420.

**07:**
- Die 10 Spielkacheln stimmen mit `farbe/spiel/*.png` ueberein (mittlere Abweichung <= 0,04), sind also roh.
- Die K4-Spalte ist vorhanden, das Skript ist versioniert (kf_tafeln.py 07).
- spiel.log und spiel_heute.log belegen je 5/5 rc=0.
- Die Spielbilder sind von 21:09-21:11, die TIM von 21:08:36. Sie zeigen also die Endtextur.

**Ergebnis:** Die Darstellungsregel ist eingehalten. Nach Regel 7.5 duerfen die Tafeln trotzdem nicht zum Nutzer, solange 5 Riegel ROT sind.
