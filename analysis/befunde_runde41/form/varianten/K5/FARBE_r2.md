# K5c Farbe, Runde 2 von 3 (Bauer, 2026-11-06)

Auftrag: Orchestrator, Spur Form. Die Riesenform K5c ("eher c") neu texturieren nach `MESSBLATT_FARBE_V1.md` und jeden Befund der Pruefung
Runde 1 (`FARBE_PRUEFUNG_r1_sicht.md`, `FARBE_PRUEFUNG_r1_zahlen.md`) beheben. Runde 1 steht in `FARBE_r1.md`, ihre Variante in `farbe/K5c_farbe_r1/`.
Geometrie, Clips, UV, Dreiecke unveraendert: `farbe/K5c_farbe/FORM.EMD` = `farbe/K5C_FARBE.EMD` = `K5c/FORM.EMD` (cmp, sha256 a71f8d08...), 3828 Flaechen.
Neu ist nur `FORM.TIM` (Texelindizes und 4 CLUT-Zeilen, gleiches Atlas-Layout). Kein RE-Wert: neues Modell (PORT-WAHL, NUTZER_ENTSCHEIDE E5/E19/E20).
Seiten: (W) = aus Sicht des Wesens. Lab = CIE-Lab D65 (OpenCV float).

## 0. Ergebnis

**Riegel 24/29 GRUEN, Exit 1.** ROT: K1, K2, N1, B1, B2 (Abschnitt 4). Die Gegenproben sind sauber: K5c heute 3/29 (nur die Halteriegel A10-A12),
Runde 1 bei allen fuenf Befund-Riegeln B1-B5 ROT. Die Riegel messen also etwas.

| | K5c heute | Runde 1 | **Runde 2** |
|---|---|---|---|
| Riegel (Messblatt 24 + Befund 5) | 3/29 | 24/24 + B 0/5 | **24/29** |
| Neutralweiss A5 / Glanzrest B4 (p90 L\* ehem. Keime, <= 58) | 0,79 % / 100 | 0,00 % / 80,1 (gestaucht) | **0,00 % / 47,8 (entfernt)** |
| Blau-Klasse im neutralen Render B3 (<= 0,10 %) | 2,4-5,0 % | 11,4-16,0 % | **0,00 %** |
| Rot erosionsfest B1 (>= 32,1 %) / groesste Rotflaeche B2 (>= 3,2 %) | 21,5 / 1,19-0,61 | 20,0 / 3,26-1,70 | **28,0 / 3,65-2,45** |
| Kopf-Gaumenklasse B5 (<= 2 %) | 15,1 % | 9,0 % | **1,0 %** |
| Rot-Farbton A4 (15-50 Grad) | -10 | 19 (Median 16) | **26** |
| Koerper C\* / Fleischrot (A1 / A3) | 4,9 / 0,5 % | 12,9 / 15,1 % | 11,8 / 22,4 % |
| Bild-1-Kette C\* / ab-Streuung (K1 >= 7,9 / K2 >= 8,36) | 6,3 / 6,3 | 8,6 / 9,9 | **6,5 / 8,0 ROT** |
| Neutraler Render C\* (N1 >= 7,5 / 9,4 / 7,9) | 4,6 / 5,0 / 4,0 | 10,7 / 9,8 / 10,7 | **10,2 / 8,8 / 6,9 ROT** |

Ergebnis-Dateien: `farbe/K5C_FARBE.EMD` + `farbe/K5C_FARBE.TIM`, Variantenordner `farbe/K5c_farbe/` (FORM.EMD, FORM.TIM, form.json, bau.json mit
Herkunft je Atlasfeld, masken.npz). Kopie im Scratchpad `formfarbe/emd/K5C_FARBE.EMD/.TIM`. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*`
und Pins unveraendert.

## 1. Ursache der Ablehnung in Zahlen (Messblatt, Gegenprobe K5c heute)

"Pokemon" = grau + Magenta-Augen + Creme-Maul + Weissflecken + Tafel-Aufhellung: Koerper C\* 4,9 gegen Familie 9,3-19,8; Fleischrot 0,5 % bei -10 Grad
(Familie 11,5-38 % bei 25-48 Grad); sigma_ab je Teil 4,0 gegen 7,4-10,7; Kiefer 77 % Creme, HF 0,461; Augen (207,95,161) C\* 54; 0,79 % silberweisse
Koerpertexel; Gamma 0,5/0,6 nur auf K5c. `riegel_farbe.py --gegenprobe K5c` ergibt dieselben Zahlen (Abschnitt 4, rechte Spalte).

Runde 1 hatte das behoben, aber zwei neue "Pokemon"-Merkmale erzeugt (Sicht-Pruefer r1): **P1** alles Rot als helles Adernetz auf dunklem Grund
(Erosionsrest 14-24 %, groesste Rotflaeche 286-483 px, Adern heller als die Haut), **P2** stahlblaugraue Beine (Blau-Klasse 10,6-14,7 %). Dazu
(Zahlen-Pruefer r1) Glanzflecken nur gestaucht (ehemalige Weisstexel Lab (72,9/7,9/1,5)), beiger Gaumenstreifen x 500-511, Konstanten ohne Quelle.

## 2. Vorgehen (`generator/kf_mal2.py`, Runde 1 `kf_mal.py` bleibt fuer den Nachbau unveraendert)

Je Texel (Lab), Reihenfolge wie im Code:

1. **Glanz und Gaumen entfernen** (Zahlen-Pruefer 6.1/6.6, Sicht-Pruefer P4): Keim = Neutralweiss nach A5 (L\* > 85, C\* < 10), Hof = angrenzende
   Texel mit L\* > 58 (hellstes Original-Band, Messblatt 2 "T L\* 51-58"); Gaumen = Zahnklasse im Kopffeld (L\* > 60, C\* < 35, Messblatt 3) mit
   demselben Hof. 881 + 2324 Texel werden im Ausgangsbild K5c **harmonisch aus der Umgebung gefuellt** (Laplace je Atlasfeld, kein Radius).
   Keine Stauchung oberhalb L\* 70 mehr.
2. **Grundton** = K5c-Grobbild (Gauss 2,5 Texel, normiert im Feld; Quelle kf_mal.py:40-42) mit a\*b\* x 1,48 (Messblatt 2).
3. **Haut-Form** aus RE2-G4: Hautquellen mit Aderanteil <= 10 % (`G4_HAUT2`, Abschnitt 3), Helligkeits- und Farbform = Quelle minus Grobbild, aber
   aus dem G4-Bild **ohne seine Adern** (Adern harmonisch gefuellt), sonst trug die Malform den roten Aderhof als Sprenkel (Lauf r2-v0). Die G4-Adern
   selbst: L\* = 0,85 x Haut (EM30-Adern 23 gegen Haut 27, Sicht-Pruefer P1), C\* <= 1,50 x L\* (G4-Dunkelrot (18,9/25,6/12,2), Messblatt 1),
   am Ende nach allen Versaetzen erneut begrenzt. 4449 Texel = 5,9 % der Haut.
4. **Fleischflaechen** (P1): Anteil roter Original-Bildpunkte je Texel (Pruefer-Rotklasse, Bild 2 nach Abzug der Einstreuung a\* 12,8 = Kette 15,5
   minus Albedo 2,7, Messblatt 2), im Feld harmonisch fortgesetzt (E11), Gauss 2,5 (E12/E13), **Mehrheitsschwelle 0,5** (E14). Material: G4-Fleisch
   (181,224,20,30) und (179,229,24,24), in L\* und C\* auf das Mittel der Familienwunden skaliert (Sicht-Pruefer P5: EM30 (50/26/13), G4 (53/27/23)
   -> (51,5/26,5/18,0)), Farbton der Quelle. **Verdeckte Unterseite**: die 23,9 % der Hauttexel mit der staerksten Normalen nach unten (Ruhepose
   Clip 2, `kf_geo.py`; 23,9 % = G4-Fleischrot, Messblatt 1), nur nie gesehene Texel (E14), Kopffeld ausgenommen, Uebergang ueber 1,2 x 23,9 %
   (Umring 19-20 %, P5), in der dunklen G4-Rotstufe (18,9/25,6/12,2). Fleisch gesamt 17 210 Texel (23,0 % der Haut).
5. **Kiefer, Krallen** wie Runde 1 (G4-Maul (10,44,60,40) 90 Grad gedreht, G4-Krallen (231,200,20,50)); Kopienzahl = Umfang / Quellbreite
   (5 bzw. 8), Zeilenversatz gleichverteilt (j x Hoehe / Kopien).
6. **Zonenversatz** (Eichung gegen die Kette, Messblatt 2): sichtbare Texel nach Bildpunkten je Zone, verdeckte harmonisch im Feld.
   **Zone = Bauteil**: die Bildzone Unterkiefer wirkt nur auf das Kieferfeld, alle anderen nie darauf. Runde 1 traf ueber die Unterkiefer-Zone
   (L\* +38,7) hand_r (217 px), Kopf (502 px) und Unterschenkel (107 px): Ursache der hellen Pfote (Sicht-Pruefer P3) und eines Teils der Glanzreste.
7. **Farbwechsel aus dem Original** (E10/E11, K2): je sichtbarem Hauttexel a\*b\* minus Zonenmittel der Original-Bildpunkte, ohne die roten Bildpunkte
   (die tragen die Fleischflaeche), harmonisch fortgesetzt, Gauss 0,7 (E13).
8. Zaehne (Bild-2-Zahnflecken (79,6/1,4/9,5), Verlauf auf 45/80 = 0,56 zum Grund), Augen (E24 (13,4/15,8/6,9)), Augenumfeld r 6 px (218 Texel),
   E9-Schwanzausgleich (Gerade durch die Glied-Mittel 1-5).
9. **Farbfenster**: Farbton -55..25 Grad bei C\* > 14,5 auf **25 Grad** gedreht (EM30-Rot, Messblatt 1; Runde 1: 10); ausserhalb -55..80 Grad
   C\* <= 5,5 (Runde 1, Quellen dort); **b\* >= -6** (Blau-Klasse P2; Runde 1 liess -125..-55 Grad bis C\* 14,5 offen). Gemessen am Original Bild 1:
   29,9 % der Figurpixel liegen in der Blau-Klasse; mit b\* auf -6 gekappt bleibt C\* 7,99, mit Chroma-Skalierung 7,40 (deshalb Kappung).
10. **CLUT**: k-Mittel je Zeile (255 Farben, Saat 7), 5-Bit-Stufen **gerundet** statt abgeschnitten (Zahlen-Pruefer r1 A8 +2,5), danach Blau-Regel an der
    Spielfarbe (Dekodierung x 255 // 31): 20 Stufen gesenkt, sonst kippen dunkle Farben durch eine Blaustufe in die Blau-Klasse.

**Eichung** (`kf_mal2.py eichen`, Verlauf `generator/kf_eich2.json`, 67 Runden ueber alle Laeufe): nur noch Zonenversatz L\*/a\*/b\* (10 Zonen) und
Augenhelligkeit (Ziel Original L\* max 22,1). Klauen: L\* und b\* nicht geeicht (Feuerschein, Messblatt K9), a\* nur nach oben (K9 ist Untergrenze).
b\*-Versatz begrenzt, bis der Texel mit dem groessten b\* der Zone an b\* -6 liegt (darueber wirkungslos; Lauf r2-e18 ohne Grenze: d_b -147).
Keine Aderstaerke g, keine Fleischschwelle, keine Farbkraft in der Eichung. Endwerte (L\*, a\*, b\*): Schwanz (-2,6/-2,2/-11,5), Ruecken/Flanke
(-6,6/-4,1/-16,8), Hinterbein l (W) (0,4/2,3/-17,1), Hinterbein r (W) (-3,0/-1,0/-24,3), Vorderglied r (W) (-2,2/-6,2/-11,2), Oberkiefer (-5,0/10,8/-15,2),
Kopf/Rumpf (-1,8/1,1/-2,1), Unterkiefer (82,4/31,2/19,0), Klauen 0. Restfehler der letzten Runde dE max 8,0 (Klaue r (W), b\* Feuerschein, kein Riegel).
Verfahrensparameter: Schrittweite 0,8, Saat 7. `kf_mal2.py bauen` aus `kf_eich2.json` ist byte-gleich wiederholbar (cmp).

**Konstanten ohne Quelle aus Runde 1 (Zahlen-Pruefer 4), Verbleib:** `np.clip(g, 0, 2.5)` und g selbst entfallen; `clip(fleisch_k, 0.2, 1.5)`,
`max(4.0, auge)` und `deck > 0.2`, `deck/0.5`, `cov/0.5`, `gd/0.05` entfallen (harmonische Fortsetzung, Mehrheitsschwelle); Stauchung L\* > 70 (0,25)
entfaellt (Glanz entfernt); Kachelversaetze `7*i`, `11*j`, `9*j`, `auf_hoehe(5)` ersetzt durch gleichverteilte Versaetze aus der Quellhoehe; Ueberblendung
= 2 x SIG_GROB (5); D_MAX ersetzt durch die Fenstergrenze B_MIN; Augen-Reserve 26 ersetzt durch das Original 22,1; K_FEIN, G_L, SIG_ERG, SIG_NAEHE,
K_VERDECKT und naehe^3 entfallen. Verbleibende PORT-WAHL mit Nutzerentscheid: SIG_AB_FEIN 0,7 (E13), Mehrheitsschwelle 0,5 (E14).

### Laufprotokoll (Scratchpad `formfarbe/r2/v0 ... e21`)

| Lauf | Aenderung | Befund |
|---|---|---|
| v0 | erster Bau: Glanz entfernt, Adern dunkel, Fleisch nur Screenshot | Blau 0,8-1,6 %, Rot nur Sprenkel (Erosion 6-20 %), Gaumen-Rest x 384-392 |
| v1-v2 | Gaumen-Hof, G4-Malform ohne Adern, Hautquellen <= 10 % Adern, Unterseite 23,9 % | Blau 0 %, ruhiger; Rot zerfaellt im Render weiter |
| e1-e2 | Eichung; Farbton der Haut >= -14 Grad (Projektion / C\* <= 6) | K1 6,2, K2 6,8, N1 6,9-9,1 ROT -> verworfen |
| e3 | Zone = Bauteil (Unterkiefer), b\* >= -6 | Pfote nicht mehr hell; 23/29 |
| e4-e7 | Farbwechsel aus dem Original; Farbkraft-Eichung auf K1 bzw. K4 | K1 8,7 nur mit K4 9,9-10,2 % (Fleisch verdraengt); Ziel K4 entsaettigt auf 0,74 -> verworfen |
| e8-e12 | Klauen L\*/b\* fest, Kieferkante weich, G4-Rose, weiche Unterseite | 25/29; Unterseite als rote Saeume an Umrisskanten |
| e13 | ohne Unterseite | A1 8,8, A3 8,4 knapp, B1 28 -> verworfen (A1 ROT) |
| e14-e17 | Unterseite dunkelrot (G4-Stufe), Kopf ausgenommen, Versaetze aus Massen | 25/29 (K1, N1, B1, B2) |
| e18 | Rose raus, Fleisch auf Familienwunde skaliert (Z1 in Tafel 01 knallrot statt rosa) | 24/29 (K2 8,1) |
| e19-e21 | b\*-Eichung begrenzt (Zonenmittel, dann Zonenmaximum) | **24/29, Endstand** |

## 3. Herkunft der Atlasfelder (`farbe/K5c_farbe/bau.json` "herkunft")

| Feld (Atlas) | Herkunft |
|---|---|
| Koerper Seiten 0-2 (46 Felder aus `J_dicker/bau.json`) und Kopf (384,0,128,104) | K5c-Grobbild ohne Glanz/Gaumen (Gauss 2,5) mit a\*b\* x 1,48 + RE2-G4-Hautform aus (130,96,20,62), (190,96,20,60), (314,12,32,30), (269,4,24,32), (29,152,24,32) (G4-Atlas wie `p_lade.re2(0x34)`; Aderanteil 6 / 9 / 7 / 0 / 9,5 %), Startquelle = Feldnummer; G4-Adern dunkel; Fleisch wo Bild 1/2 mehrheitlich Rot zeigen aus G4 (181,224,20,30) / (179,229,24,24) skaliert auf (51,5/26,5/18,0); verdeckte Unterseite G4-Fleisch in der Stufe (18,9/25,6/12,2); Zonenversatz; Farbwechsel aus dem Original |
| Kiefer (384,106,128,72) | RE2-G4-Maul (10,44,60,40), 90 Grad gedreht, 5 Kopien um den Umfang, Zeilenversatz j x 40 / 5, Ueberblendung 5 Texel; L\*-Versatz nur am sichtbaren Band, Kante Gauss 2,5 |
| Zaehne (384,180,24,14) | gemalt: Bild-2-Zahnflecken Lab (79,6/1,4/9,5), L\* zum Grund x 0,56 |
| Krallen (410,180,100,40) | RE2-G4-Krallen (231,200,20,50), 8 Kopien, Zeilenversatz j x 50 / 8 |
| Augen (384,196,24,10) + 218 Texel Augenumfeld | gemalt: E24 (13,4/15,8/6,9) |

Die G4-Rechtecke sind voll gedeckt (Suche nach gedeckten Rechtecken mit kleinstem bzw. groesstem Rotanteil im G4-Atlas, Schulterauge x 270-321,
y 160-245 ausgenommen); Maul und Krallen zu 84 / 82 % gedeckt, Luecken aus dem Umfeld (kf_mal.ausschnitt).

## 4. Riegel Soll / Ist (`python generator/riegel_farbe.py farbe/K5c_farbe --gegenprobe K5c --gegenprobe-r1 farbe/K5c_farbe_r1`, `farbe/riegel_K5c_farbe.json/.txt`)

| # | Soll | Runde 2 | K5c heute | Runde 1 |
|---|---|---|---|---|
| A1 Koerper C\* | 9,5-21 | 11,8 GRUEN | 4,9 | 12,9 |
| A2 sigma_ab je Teil | >= 7,0 | 13,1 GRUEN | 4,0 | 11,9 |
| A3 Fleischrot | 8-40 % | 22,4 GRUEN | 0,5 | 15,1 |
| A4 Rot-Farbton | 15-50 Grad | 26 GRUEN | -10 | 19 |
| A5 Neutralweiss | <= 0,20 % | 0,00 GRUEN | 0,79 | 0,00 |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,04-0,26 | 0,091/0,153/0,047/0,064 GRUEN | Kiefer 0,461 | 0,164/0,109/0,068/0,063 |
| A7 HF_rest | <= 0,19 | max 0,065 GRUEN | 0,436 | 0,092 |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >= 20 / >= 20 % | 6,1 / 95,7 / 39,2 GRUEN | 77,1/0/0 | 2,7/33,8/44,5 |
| A9 Auge L\*/C\* | <= 30 / <= 30 | 13,1 / 18,0 GRUEN | 56,1/53,7 | 13,1/18,0 |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 GRUEN | gleich | gleich |
| A11 Schwanzsprung | <= 4,0 | 1,4 GRUEN | 3,4 | 0,5 |
| A12 Farbkorn | <= 5,7 | 4,54 GRUEN | 2,63 | 4,40 |
| K1 Bild 1 C\* | 7,9-11,1 | **6,5 ROT** | 6,3 | 8,6 |
| K2 Bild 1 ab-Streuung | >= 8,36 | **8,0 ROT** | 6,3 | 9,9 |
| K3 Bild 1 dE je Zone | <= 6 | max 5,4 (Hinterbein r (W)) GRUEN | 8,3 | 1,3 |
| K4 Bild 1 Rotklasse | 3,1-7,8 % | 6,2 GRUEN | 2,5 | 6,9 |
| K5 Bild 1 Hell Ruecken / geclippt | <= 14,9 / <= 0,8 % | 4,7 / 0,4 GRUEN | 21,9/1,0 | 6,8/0,1 |
| K6 Bild 2 Figur dE / C\* | <= 6 / >= 21,8 | 1,5 / 23,8 GRUEN | 9,9/16,7 | 2,7/22,7 |
| K7 Bild 2 Unterkiefer dE / a\* | <= 6 / >= 21,2 | 0,0 / 24,9 GRUEN | 12,4/13,7 | 0,2/24,8 |
| K8 Bild 2 Oberkiefer dE | <= 6 | 1,4 GRUEN | 12,0 | 0,6 |
| K9 Bild 2 Klauen a\* r/l | >= 18,1 / 16,7 | 26,1 / 28,3 GRUEN | 13,8/22,8 | 24,1/22,5 |
| K10 Bild 2 Augen L\* max | <= 30 | 14,3 / 15,9 GRUEN | 51,5/36,2 | 13,5/15,9 |
| N1 neutral C\* (EM30 10,0/12,5/10,5) | >= 0,75 x EM30 | 10,2 / **8,8 / 6,9 ROT** | 4,6/5,0/4,0 | 10,7/9,8/10,7 |
| N2 neutral L\* > 85 | <= 0,10 % | 0 / 0 / 0 GRUEN | 0,83/1,30/0,11 | 0/0/0 |
| B1 Rot erosionsfest (3 Winkel zus.) | >= 32,1 % (EM30; G4 41,4, G5 52,4) | **28,0 ROT** | 21,5 | 20,0 |
| B2 groesste Rotflaeche / Figur (3/4 vorn, Seite) | >= 3,2 % (G4 Seite; EM30 3,59) | 3,65 / **2,45 ROT** | 1,19/0,61 | 3,26/1,70 |
| B3 Blau-Klasse je Winkel | <= 0,10 % (G4/G5 0,0) | 0,00 GRUEN | 3,9/5,0/2,4 | 11,4/15,5/16,0 |
| B4 Glanzrest p90 L\* ehem. Keime | <= 58 (hellstes Band) | 47,8 GRUEN | 100 | 80,1 |
| B5 Kopffeld Gaumen-/Zahnklasse | <= 2,0 % | 1,02 GRUEN | 15,1 | 9,0 |

B1-B5 sind neu (Befunde r1). Grenzen aus der Familie mit demselben Code (`riegel_farbe.py --familie`); B1/B2 relativ zur Figur statt der 34 % / 1000 px
des Sicht-Pruefers, weil K5c in derselben Umkugel nur etwa halb so viele Pixel deckt wie G4 (Fairness-Befund des Sicht-Pruefers) und EM30 (gelobter
Stand derselben Kreatur) die Familienuntergrenze stellt.

**Warum K1/K2/N1 rot bleiben (gemessen, kein Platzhalter):** Die Bild-1-Farbkraft des Originals (C\* 9,25) besteht zu 29,9 % aus Bildpunkten der
Blau-Klasse. Ohne Blau (B3, Sicht-Pruefer P2 "Abweichung, die der Nutzer wieder ablehnen wuerde") traegt das Original selbst nur noch C\* 7,99
(gekappt) bzw. 7,40 (skaliert), also an der K1-Grenze 7,9. Runde 1 erreichte K1 8,6 mit blauen Beinen und rotem Adernetz. Die fehlende Farbkraft
aus warmen Toenen zu holen, kollidiert mit K4 (Laeufe e5/e6: K1 8,7 bei K4 9,9-10,2 % > 7,8). N1 (neutraler Render, Seite und 3/4 hinten) ist dieselbe
Lage: die Rueckansicht entspricht Bild 1. **K1/K3-Ziel enthaelt das Video-Blau, B3 verbietet es** - das ist eine Entscheidung (Abschnitt 7, O1).
B1/B2: Rot liegt jetzt als Flaechen (Schulter/Ruecken Z1, Kiefer, Krallen, Schwanzspitze; 3/4 vorn 3,65 % gruen), in der Seitenansicht ist die
groesste Flaeche 2,45 %; mehr sichtbares Fleisch auf Ruecken/Flanke verbietet Bild 1 (K4, E14).

## 5. Tafeln (`farbe/`, `generator/kf_tafeln.py`, Messblatt 7: roh, kein Gamma auf keiner Kachel)

| Tafel | Inhalt |
|---|---|
| `01_szene_bild1.png` | Bild 1 Original - K4 heute - K5c neu (Runde 2); Zeile 1 Disc-Licht ROOM5080 Cut 2 x Kanalfaktor k (1,06/0,95/0,82, `pruefung/licht.json`, beide Modelle gleich; Befund r1 "k verschwiegen"), Zeile 2 durch form_video |
| `02_szene_bild2.png` | dasselbe fuer ROOM5090 Cut 11 (k 1,20/1,12/1,00) |
| `03_familie.png` | EM30_HANGAR - RE2-G5 - RE2-G4 - K5c neu - K4 heute, gleicher Renderer, Licht 0,55 + 0,45 N.L, Umkugel 0,46, 3 Winkel; L\*/C\*/L\*>85 und Figurpixel je Kachel |
| `04_atlas.png` | EM30 - K5c neu - K5c heute, 2 Bildpunkte je Texel, je 8 Palettenfelder mit Lab und Anteil |
| `05_rundum_neu.png` | K5c neu Pose Bild 1 (Clip 0) und Ruhe (Clip 2), 6 Winkel, Licht wie 03, EM30 als Vergleichskachel |
| `06_kopf_nah.png` | Bild-2-Ausschnitt x 200-360, y 290-420 3-fach (Original, T, neu Kette, heute Kette); Kopf+Kiefer neu / heute / RE2-G4-Kopf |
| `07_spiel.png` | echtes Spiel (eigene exe-Kopie, `generator/kf_spiel.sh --mit-heute` -> `re15_port/tools/form_spiel.sh`, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER): je Zeile Original-Standbild - K4 heute im Spiel - K5c neu im Spiel; 5 Szenen (5080 Nachstellung, Lage Bild 1, Ruhe; 5090 Pose Bild 2, Ruhe). Einzelbilder `farbe/spiel/`, Logs `spiel.log` (neu, 5/5 rc=0) und `spiel_heute.log` (K4, 5/5 rc=0). Zusammengesetzt von `kf_tafeln.py ... 07` (versioniert, Befund r1). Runde-1-Spielbilder in `farbe/spiel_r1/` |

## 6. Tests

Bau `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK. `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`: **11/11 gruen**,
nichts wiederholt, 95 s (unit_birkin_bahnsteig, unit_birkin_formen, unit_r41_form_emd, integration_r41_form_wagenweg, integration_r35_werfer_form,
integration_r34_plattform_takt, integration_r34_plattform_esp_eintritt, unit_r34_plattform, unit_r34_plattform_ton, unit_r35_fenster_satzform,
unit_r39_funk10f1_form).
Das Spiel-Asset ist unveraendert; die Tests pruefen den Bestand, der Kandidat laeuft nur in 07.

## 7. Offen

| # | Punkt | Stand |
|---|---|---|
| O1 | **Blau gegen Bild-1-Farbkraft**: K1/K2/N1 verlangen die Bild-1-Farbkraft, die zu 30 % Video-Blau ist; B3/P2 verbieten blaue Haut. Entweder Blau-Stich an den Beinen zulassen (K1/N1 erreichbar, P2 kehrt zurueck) oder K1/N1 auf das Original ohne Blau-Klasse beziehen (C\* 7,99 -> Grenze 6,8) | Entscheidung Orchestrator/Nutzer |
| O2 | Helligkeit: neutral L\* 14,6-16,3 (G4 17,0-21,6, EM30 24,5-29,0). Bild 1 verlangt das Dunkle (K3) | unveraendert seit Runde 1 |
| O3 | Z1 (Rueckenfleck) ist jetzt warmes Familienrot (25 Grad); im Video wirkt er rosa-violett (Original T (54/17/-3), -10 Grad). Messblatt 1 nennt das K5c-Magenta einen Fehler, E14 verlangt die Screenshot-Farbe | Nutzer-Urteil an Tafel 01 |
| O4 | Verdeckte Unterseite (23,9 %) ist eine Familien-Annahme (G4), kein Screenshot-Beleg (74 % der Texel unsichtbar); dunkelrot, damit sie an Umrisskanten nicht leuchtet | Nutzer-Urteil an 03/05 |
| O5 | Kachelnaehte der G4-Hautquellen in grossen Feldern (O3 r1) bleiben schwach sichtbar; Versaetze jetzt gleichverteilt | klein |
| O6 | K-Riegel sind weiter Eichziele (Zonenversatz); unabhaengig sind A, N und B. Gegenprobe mit anderem Rausch-Saatwert nicht wiederholt | zweiter Pruefer |
| O7 | "von der Struktur" (Messblatt 8): nur Texturstruktur bearbeitet, Kopfform K5c unveraendert | Nutzer |
