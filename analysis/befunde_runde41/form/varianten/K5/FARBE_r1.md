# K5c Farbe, Runde 1 von 3 (Bauer, 2026-11-06)

Auftrag: Orchestrator, Spur Form. Die Riesenform K5c (vom Nutzer gewaehlt: "eher c") neu texturieren nach
`MESSBLATT_FARBE_V1.md`. Geometrie, Clips, UV und Dreiecke bleiben gleich: `FORM.EMD` ist byte-gleich mit `varianten/K5/K5c/FORM.EMD`
(cmp), 3828 Flaechen. Neu ist nur `FORM.TIM` (Texelindizes und 4 CLUT-Zeilen, gleiches Atlas-Layout).
Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL, NUTZER_ENTSCHEIDE E5/E19/E20). Jede Zahl im Generator traegt ihre Quelle (Messblatt-Zeile,
Familienmessung, Bildmessung) oder PORT-WAHL mit Lauf-Beleg. Seiten: (W) = aus Sicht des Wesens.

## 0. Ergebnis

| | K5c heute | K5c neu (Runde 1) |
|---|---|---|
| Riegel MESSBLATT_FARBE_V1 Abschnitt 6 (24 Zeilen, `generator/riegel_farbe.py`) | 3/24 GRUEN (nur A10-A12) | **24/24 GRUEN, Exit 0** |
| Koerper C\* / Fleischrot / Rot-Farbton | 4,9 / 0,5 % / -10 Grad | 12,9 / 15,1 % / 19 Grad |
| Neutralweiss Koerper (A5) / neutraler Render L\* > 85 (N2) | 0,79 % / 0,83-1,30 % | 0,00 % / 0,00 % |
| Kiefer Zahn / Rot / Dunkel (A8) | 77,1 / 0 / 0 % | 2,7 / 33,8 / 44,5 % |
| Augen Atlas L\*/C\* (A9), Kette E24 L\* max (K10) | 56,1/53,7, 51,5/36,2 | 13,1/18,0, 13,5/15,9 |
| Bild-1-Kette C\* / ab-Streuung (Original 9,3 / 9,8) | 6,3 / 6,3 | 8,6 / 9,9 |
| Bild-2-Kette Figur dE / C\* (Original C\* 25,6) | 9,9 / 16,7 | 2,7 / 22,7 |
| Neutraler Render C\* je Winkel (EM30 10,0 / 12,5 / 10,5) | 4,6 / 5,0 / 4,0 | 10,7 / 9,8 / 10,7 |

Ergebnis-Dateien: `farbe/K5C_FARBE.EMD` + `farbe/K5C_FARBE.TIM` (Paar wie RE2/BAHNSTEIG_FORM.EMD/.TIM), dieselben Dateien als
Variantenordner `farbe/K5c_farbe/` (FORM.EMD, FORM.TIM, form.json, bau.json mit Herkunft je Atlasfeld). Kopie im Scratchpad
`formfarbe/emd/K5C_FARBE.EMD/.TIM`. Spiel-Asset und Pins nicht angefasst.

## 1. Ursache der Ablehnung in Zahlen (aus dem Messblatt, von `riegel_farbe.py --gegenprobe ../K5c` bestaetigt)

"Pokemon" = grau + Magenta-Augen + Creme-Maul + Weissflecken, dazu die Tafel-Aufhellung:
Koerper C\* 4,9 gegen Familie 9,3-19,8; Fleischrot 0,5 % (Magenta, -10 Grad) gegen 11,5-38 % bei 25-48 Grad; je Teil fast eine Farbe
(sigma_ab 4,0 gegen 7,4-10,7); Kiefer 77 % Creme, 0 % Rot, 0 % dunkel, Rauschflaeche HF 0,461; Krallen HF 0,324; Augen (207,95,161) C\* 54;
0,79 % silberweisse Koerpertexel (rueckprojizierte Glanzpunkte); Tafeln nur fuer K5c mit Gamma 0,5/0,6. Die Gegenprobe ergibt dieselben
Zahlen wie das Messblatt (3/24, N1 4,6/5,0/4,0, N2 0,83/1,30/0,11).

## 2. Vorgehen

1. **Sichtkarte** (`generator/kf_sicht.py`, Ergebnis `farbe/kf_sicht_k5c.npz`): K5c wird in Bild 1 und Bild 2 genau wie in p_riegel gerendert
   (form_pruefung.Kandidat, Licht 'mess'), aber mit drei Kodier-TIMs (Index = Texelkoordinate, 15-Bit-CLUT eindeutig). Aus dem Albedo-Render
   kommt je Bildpunkt der Texel. Gegenprobe: 98 % der Bildpunkte treffen genutzte Atlastexel, 94 % mit Albedo = Atlasfarbe (+-8).
   Sichtbar: Bild 1 9807 Texel (Schwanz 1855, Ruecken/Flanke 2130, Hinterbeine 1307/1331, Vorderglied r (W) 1511), Bild 2 13711 Texel
   (Oberkiefer 1785, Kopf/Rumpf 5484, Unterkiefer 1681, Klauen 1210/1695). Das Unterkieferband ist im Kieferfeld x 30-89, y 21-70 (11 % des Felds).
2. **Bau** (`generator/kf_mal.py bauen`), je Texel in Lab:
   - L\* = K5c-Grobbild (Gauss 2,5 Texel, normiert im Atlasfeld) + 0,5 x K5c-Feinstruktur + RE2-G4-Malform + Zonenwert.
     Die G4-Malform kommt aus zehn voll gedeckten G4-Hautfeldern (Abschnitt 3), nebeneinander mit 4 Texeln Ueberblendung, nicht gespiegelt.
   - a\*, b\* = 1,48 x K5c-Grobbild (Messblatt 2: Bild-1-Kette C\* 9,3 / 6,3) + g_zone x G4-Malform (Gauss 0,7) + Zonenwert;
     verdeckte Texel bekommen zusaetzlich den G4-Grundton (Rot-/Fleischflaechen der G4-Haut, Messblatt 1 "G4 = Vorbild") und tragen den
     Zonen-Ausgleich nur mit ihrer Naehe zur Sicht.
   - Fleischstellen Bild 1 (E11, E14): Bildpunkte der Pruefer-Rotklasse im Original Bild 1 auf ihre Texel (930 Texel mit Gewicht > 0,1),
     Farbe = Mittel dieser Originalpunkte T Lab (36,7, 19,6, -3,3), Staerke 0,632 (an K4 geeicht). Bild 2 bekommt keine Fleischstellen
     (Rotklasse dort teils Feuerschein, Messblatt 2).
   - Farbwechsel innerhalb der Zone (E10/E11): a\*b\*-Abweichung der Original-Bildpunkte vom Zonenmittel, geglaettet (1,5 Texel), nur Farbe,
     nie Helligkeit (keine Glanzpunkte).
   - Farbfenster der Familie: Farbton 10-80 Grad frei; -55..10 Grad mit C\* > 14,5 auf 10 Grad gedreht (Magenta -> Fleischrosa);
     -125..-55 Grad hoechstens C\* 14,5 (Violett-/Blaugrau der Grundhaut); sonst (Tuerkis, Gruen, Gelbgruen) hoechstens C\* 5,5.
     Grenzen gemessen: Familie G4/G5/Hund p1-p99 der Texel mit C\* > 10; Bild-1-Grundhaut und Original-Bild-1-Saettigung (p75 / p50).
   - E9: mittlere Helligkeit der Schwanzglieder 1-5 auf die Ausgleichsgerade (vorher 13,4 / 8,9 / 11,6 / 12,2 / 11,3 -> Sprung 0,5).
   - L\* > 70 gestaucht (EM30-Haut p95 70,6) -> 0 % Neutralweiss.
   - 8-bpp: je CLUT-Zeile 255 Farben (k-Mittel in Lab, Saat fest), 5-Bit-CLUT wie das Spiel.
3. **Eichung an der Kette** (`kf_mal.py eichen`, Verlauf `generator/kf_eich.json`): je Nutzerzone Lab-Versatz d += 0,8 x (Original - Kette),
   G4-Aderstaerke g nach der Rotklasse, Fleischstaerke nach K4, Augen nach K10; Begrenzung |d| <= 45/20/20 gegen Aufschaukeln.
   Endwerte (L\*, a\*, b\*; g): Schwanz (-3,4, -6,6, -2,4; 0,00), Ruecken/Flanke (-11,0, -8,4, -1,2; 0,09), Hinterbein l (W) (-0,3, -11,5, -13,0; 2,5),
   Hinterbein r (W) (-2,9, -8,5, -11,8; 0,33), Vorderglied r (W) (-2,4, -5,7, -3,6; 0), Oberkiefer (-5,0, 11,4, 1,8; 0,35),
   Kopf/Rumpf (-5,0, 1,1, 0,6; 1,15), Unterkiefer (41,0, 16,3, 16,6; 0,09), Klaue r (W) (1,0, 1,7, 0; 0,12), Klaue l (W) (10,8, -6,2, 0; 2,5).
   Klauen-b\* wird nicht geeicht (Messblatt K9: Feuerschein).
4. **Deterministisch**: `kf_mal.py bauen` aus `kf_eich.json` ergibt byte-gleich dasselbe FORM.TIM (cmp).

### Laufprotokoll (Scratchpad `formfarbe/v0` ... `v13`)

| Lauf | Aenderung | Riegel / Befund |
|---|---|---|
| v0 | erster Bau, keine Eichung | 10/22 (A+K); A12 Farbkorn 6,95 ROT; K10 links 36,2 (helle Kopfhaut im Augenkreis) |
| v1 | G4-Farbe Gauss 0,7, Augenumfeld r 6 px dunkel | A12 3,92, K10 gruen; A1/A3 zu wenig |
| v2 | Eichung 4 Runden | 17/22; A3 7,4 %, A8 Dunkel 12 %, K1/K2/K4 knapp |
| v3-v5 | Kiefer-L nur am Band, Fleischstellen Bild 1, Farbwechsel aus dem Original, E9 | 20/22; A3 5,6 %, A12 6,06 bei Gauss 0,5 |
| v6 | verdeckte Texel mit G4-Grundton, Naehe zur Sicht neu (v6a: 83 % galten als gesehen) | A3 15-17 % |
| v7 | Eichung | **24/24** - aber neutraler Render: Regenbogen (Gruen, Tuerkis, Gelb, Blau), gespiegelte "Rorschach"-Adern -> verworfen |
| v8-v10 | Ueberblendung statt Spiegelung, Farbfenster, Klauen-b\* nicht geeicht | Bild sauberer; K1 6,8-7,7 ROT (Fenster zu eng) |
| v11-v12 | K5c-Grobfarbe x 1,48, Fenster aus Bildmessung (C 14,5 / 5,5), Blau bis -125 Grad (Beine: Spiellicht nimmt Blau weg, gemessen), Begrenzung d | 23/24 (K4 7,9 % > 7,8) |
| v13 | Fleischstaerke an K4 geeicht | **24/24, Gegenprobe K5c 3/24, Exit 0** |

## 3. Herkunft der Atlasfelder (`farbe/K5c_farbe/bau.json` "herkunft", Feld fuer Feld)

| Feld (Atlas, Zeile) | Herkunft |
|---|---|
| Koerper Seiten 0-2 (46 Felder aus `J_dicker/bau.json`: huefte, wirbel, brust, hals, Schwanz 1-6, Beine, Fuesse, Zehen, Arme, Haende, Klauen, Krallen) | K5c-Grobbild + 0,5 x K5c-Fein + RE2-G4-Malform aus den Hautfeldern (130,96,20,62), (160,98,20,62), (190,96,20,60), (136,172,20,76), (132,26,20,34), (232,22,16,60), (314,12,32,30), (20,158,48,24), (22,110,40,24), (10,202,24,34) (G4-Atlas wie `p_lade.re2(0x34)`, Startfeld je Atlasfeld verschieden) + Zonenwert + Fleisch/Farbwechsel aus dem Original |
| Kopf (384,0,128,104), Zeile 3 | wie Koerper (Malform G4), Zonen Oberkiefer/Kopf-Rumpf; Augenumfeld 218 Texel dunkel |
| Kiefer (384,106,128,72) | RE2-G4-Maul (10,44,60,40) (Zahnreihen, Zahnfleisch, dunkles Maulinneres), 90 Grad gedreht (Zahnreihen laengs), 5 x versetzt um den Umfang, G4-Texelmassstab; Helligkeits-Eichung nur am sichtbaren Band |
| Zaehne (384,180,24,14) | gemalt: Bild-2-Zahnflecken Lab (79,6, 1,4, 9,5), Verlauf 100 -> 60 % L\* zum Zahngrund |
| Krallen (410,180,100,40) | RE2-G4-Krallen (231,200,20,50), 7 x versetzt, G4-Texelmassstab, Zonenwert Klauen |
| Augen (384,196,24,10) | gemalt: E24 Lab (13,4, 15,8, 6,9) (Mittel der Original-Augenpunkte, Messblatt 3) |

Die zehn G4-Hautfelder sind Rechtecke ohne ungenutzte Texel (Suche im G4-Atlas nach den groessten voll gedeckten Rechtecken); Kiefer und Krallen
sind zu 84 / 82 % gedeckt, die Luecken aus dem Umfeld ergaenzt (normierte Faltung).

## 4. Riegel Soll / Ist (`python generator/riegel_farbe.py farbe/K5c_farbe --gegenprobe K5c`, Ergebnis `farbe/riegel_K5c_farbe.json`)

| # | Soll | K5c neu | K5c heute |
|---|---|---|---|
| A1 Koerper C\* | 9,5-21 | 12,9 GRUEN | 4,9 |
| A2 sigma_ab je Teil | >= 7,0 | 11,9 GRUEN | 4,0 |
| A3 Fleischrot | 8-40 % | 15,1 % GRUEN | 0,5 % |
| A4 Rot-Farbton | 15-50 Grad | 19 GRUEN | -10 |
| A5 Neutralweiss | <= 0,20 % | 0,00 % GRUEN | 0,79 % |
| A6 HF_fft je Feld | 0,04-0,26 | Koerper 0,164, Kopf 0,109, Kiefer 0,068, Kralle 0,063 GRUEN | Kiefer 0,461, Kralle 0,324 |
| A7 HF_rest | <= 0,19 | max 0,092 GRUEN | 0,436 |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >= 20 / >= 20 % | 2,7 / 33,8 / 44,5 GRUEN | 77,1 / 0 / 0 |
| A9 Auge L\*/C\* | <= 30 / <= 30 | 13,1 / 18,0 GRUEN | 56,1 / 53,7 |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 GRUEN (unveraendert, UV gleich) | 0,64-2,33 |
| A11 Schwanzsprung | <= 4,0 | 0,5 GRUEN | 3,4 |
| A12 Farbkorn | <= 5,7 | 4,40 GRUEN | 2,63 |
| K1 Bild 1 C\* | 7,9-11,2 | 8,6 GRUEN | 6,3 |
| K2 Bild 1 ab-Streuung | >= 8,3 | 9,9 GRUEN | 6,3 |
| K3 Bild 1 dE je Zone | <= 6 | max 1,3 GRUEN | 8,3 |
| K4 Bild 1 Rotklasse | 3,1-7,8 % | 6,9 % GRUEN | 2,5 % |
| K5 Bild 1 Hell Ruecken / geclippt | <= 14,9 / <= 0,75 % | 6,8 / 0,1 GRUEN | 21,9 / 1,0 |
| K6 Bild 2 Figur dE / C\* | <= 6 / >= 21,8 | 2,7 / 22,7 GRUEN | 9,9 / 16,7 |
| K7 Bild 2 Unterkiefer dE / a\* | <= 6 / >= 21,2 | 0,2 / 24,8 GRUEN | 12,4 / 13,7 |
| K8 Bild 2 Oberkiefer dE | <= 6 | 0,6 GRUEN | 12,0 |
| K9 Bild 2 Klauen a\* r/l | >= 18,1 / 16,7 | 24,1 / 22,5 GRUEN | 13,8 / 22,8 |
| K10 Bild 2 Augen L\* max | <= 30 | 13,5 / 15,9 GRUEN | 51,5 / 36,2 |
| N1 neutral C\* je Winkel | >= 7,5 / 9,4 / 7,9 | 10,7 / 9,8 / 10,7 GRUEN | 4,6 / 5,0 / 4,0 |
| N2 neutral L\* > 85 | <= 0,10 % | 0 / 0 / 0 GRUEN | 0,83 / 1,30 / 0,11 |

Gegenprobe: an K5c heute sind alle Farbriegel ROT, gruen nur die Halteriegel A10-A12 (wie im Messblatt). Der Riegel misst also.

## 5. Tafeln (`farbe/`, `generator/kf_tafeln.py`, Darstellungsregel Messblatt 7: roh, kein Gamma auf keiner Kachel)

| Tafel | Inhalt |
|---|---|
| `01_szene_bild1.png` | Bild 1 Original - K4 heute - K5c neu; Zeile 1 Spiellicht ROOM5080 roh, Zeile 2 durch form_video (Videofilter) |
| `02_szene_bild2.png` | dasselbe fuer ROOM5090 Cut 11 |
| `03_familie.png` | EM30_HANGAR - RE2-G5 - RE2-G4 - K5c neu - K4 heute, gleicher Renderer, weisses Licht 0,55 + 0,45 N.L, Umkugel 0,46, 3 Winkel, L\*/C\*/L\*>85 je Kachel |
| `04_atlas.png` | EM30 - K5c neu - K5c heute, 2 Bildpunkte je Texel, je 8 Palettenfelder mit Lab und Anteil |
| `05_rundum_neu.png` | K5c neu Pose Bild 1 (Clip 0) und Ruhe (Clip 2), 6 Winkel, Licht wie 03, EM30 als Vergleichskachel |
| `06_kopf_nah.png` | Bild-2-Ausschnitt x 200-360, y 290-420 (Original, T, K5c neu Kette, K5c heute Kette); Kopf+Kiefer nah neu / heute / RE2-G4-Kopf |
| `07_spiel.png` | echtes Spiel, eigene exe-Kopie, Kandidat ueber RE15_RE2_ASSET_ROOT (`generator/kf_spiel.sh` -> `re15_port/tools/form_spiel.sh`), gdigrab, kein AUTOSHOT/SOFTWARE_RENDER; Einzelbilder in `farbe/spiel/` |

## 6. Tests

`ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3` im Arbeitsbaum (Build `local_build.sh build`, LOCAL-BUILD-OK): 11/11 gruen
(unit_birkin_bahnsteig, unit_birkin_formen, unit_r41_form_emd, integration_r41_form_wagenweg, integration_r35_werfer_form,
integration_r34_plattform_takt, integration_r34_plattform_esp_eintritt, unit_r34_plattform, unit_r34_plattform_ton, unit_r35_fenster_satzform,
unit_r39_funk10f1_form). Das Spiel-Asset ist unveraendert, die Tests pruefen also den Bestand; der Kandidat laeuft nur in 07.

## 7. Offen

| # | Punkt | Stand |
|---|---|---|
| O1 | Nutzer-Urteil am Bild (Tafeln 01-07) | Riegel gruen ist keine Abnahme. Erst zweiter Pruefer, dann Nutzer (Messblatt 7.5) |
| O2 | Helligkeit: K5c neu im neutralen Licht L\* 13,8-16,1, G4 17,0-21,6, EM30 24,5-29,0 (03). Die Kette Bild 1 verlangt das Dunkle (K3 dE <= 1,3); ob der Nutzer "passt zum Rest" auf die Helligkeit bezieht, ist offen | Gestaltungsfrage |
| O3 | Sichtbare Teilnaehte der G4-Kacheln in grossen Feldern (Kopf 128 x 104, Huefte 158 x 52: senkrechte Streifen im Atlas, im Render schwach) | Runde 2: groessere zusammenhaengende G4-Quellen oder Synthese |
| O4 | Gaumenstreifen im Kopffeld (K5c-Grobbild hell, Kopf x 384-392 und 500-511) wirkt im Atlas cremefarben; im Render nur bei offenem Maul | Runde 2 pruefen |
| O5 | Rosa Fleck auf dem Ruecken (Z1 aus Bild 1) und weisser Fleck rechter Arm (Z5, E1/E14) bleiben bewusst; im neutralen Licht deutlich | Nutzer-Urteil |
| O6 | "von der Struktur" (Messblatt 8 Offen 1): Texturstruktur hier neu; Kopfform K5c nicht veraendert | Nutzer |
| O7 | E19 (Form und Textur fest) gegen Neufaerbung: hier nur Variante, kein Einbau | Einbau erst nach Nutzer-Abnahme |
| O8 | Die Kette ist an zwei Standbildern geeicht; 74 % der Koerpertexel sind in keinem Bild sichtbar und tragen G4-Grundton (Messblatt 1 Vorbild), nicht Screenshot-Farbe | Messgrenze |
