# Pruefung K5e, Runde 1: Zahlen-Pruefer (2026-11-09)

Geprueft wurde Zweig r41/form, Kopf ed31d11d8 (Bauer-Commits f817fb41a, a95cf39e5, ed31d11d8), Variante `ruhe/K5e/`.
Alles wurde mit eigenem Code gemessen; `riegel_k5e.py` wurde nicht gestartet.

- **Eigener EMD/TIM-Leser:** `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5e/pruefer_r1/emdlese.py`
  - Er liest das RE2-Verzeichnis, MD1 (Tri-UV 12 Byte, Quad-UV 16 Byte) und TIM 8 bpp.
  - Lab nach CIE-Formel D65, die 15-Bit-Farbe als c5 x 255/31.
- **Eigene Masse:**
  - `pr_tech.py`: Technik und Herkunft
  - `pr_pal.py` und `pr_z3.py`: Paletten
  - `pr_ruhe.py`: Ruhe-Masse
  - `pr_wunsch.py`: Wuensche und Helligkeit
  - `pr_f1.py`: F1-Leuchtstelle
- **Renderer:** Ich habe ihn selbst aufgerufen, nicht nachgebaut.
  - Weisses Licht: `rl.weiss_id`, also `render.blick` der Familien-Tafel 07, 4 Winkel.
  - Szenenkette: `v4f.szene_id` bzw. `rl.szene_k`.
  - Videofilter: `v4f.video` mit den Saaten 15, 12345 und 7.
- **Ergebnisse** stehen im selben Ordner: tech.json, pal.json, ruhe.json, wunsch.json, ctest.log und lauf2/.

**Ergebnis: kein roter Punkt.** Es gibt drei gelbe Hinweise; sie stehen in Abschnitt 5.

## 1. Regel (Herkunft, ein Satz je Material)

| Punkt | Soll | Ist (eigene Messung) | Bewertung |
|---|---|---|---|
| Texel gegen das Spiel-Asset K5c (TIM 7187ff09) | 0 neu | **0** von 512 x 256 Texeln abweichend (zur Gegenprobe: K5d hat 41 510) | gruen |
| Seiten 0-2 gegen das G4-TIM (CDEMD0.EMS @0x65e800, 384 x 256, 3 Zeilen) | gleich | **0** Texel abweichend | gruen |
| UV, Seite und Eckreihenfolge je Flaeche gegen K5c | 3828/3828 | **3828/3828** gleich; nur das CLUT-Wort weicht ab, in 2581 Flaechen (K5d: nur 2784 UV gleich, also 1044 neu) | gruen |
| Bytegleiche Zeilen | Zeile 0 = G4-Zeile 0, Zeile 8 = K5c-Zeile 5, Zeile 9 = K5c-Zeile 7 | alle drei **ja** (dazu K5c-Zeile 0/1 = G4-Zeile 0/1) | gruen |
| Haut, Zeilen 1/2/3 aus den G4-Zeilen 0/1/2: ein Satz | gleiche Hebung, ein Ton, ein C\* | Hebung je Zeile (Median L\*'-L\*) **21,20 / 21,52 / 21,56**, global 21,49. Rest-dE76 gegen das globale Modell: p95 1,52 / 2,57 / 1,94, max 1,61 / 2,93 / 2,55, texelgewichtet 0,72 / 0,78 / 0,79 (Rundung auf 15 Bit). Farbton je Zeile -34,6 / -33,5 / -34,1 | gruen |
| Gelenk, Zeilen 4/5/6 | Haut + 9,1 | Hebung **30,27 / 30,61 / 30,41** (21,4 + 9,1 = 30,5). Rest-dE76 max 1,57 / 1,97 / 1,64. Farbton -35,1 / -35,2 / -35,2, C\* 5,8-6,2 | gruen |
| Fleisch, Zeile 7 | Haut + 6,74, Ton -8 | Hebung **28,44** (Soll 28,15). Farbton -10,1 (Haut-Eintraege) bzw. -8,4 (Median); Rest-dE76 max 2,68 | gruen |
| Farbtonspanne je Zeile (genutzte Eintraege, C\* >= 3) | <= 15 Grad | Haut 0,5 / 11,9 / 11,7; Gelenk 0,1 / 0,3 / 0,3; Fleisch 7,0; Adern 0,0-0,2 | gruen |
| Rangfolge je Klasse | 0 Vertauschungen | Haut 0, hell 0 und Ader 0 in allen 7 Zeilen. Die Klassengrenze ist dabei wie in kf_g4 berechnet (OpenCV-Lab, Ader = C\* > 15 und Farbton in (-40, 70)). Ein Grenzfall: In Zeile 3, Index 170, liegt C\* bei genau 15,0. In meiner Lab-Formel zaehlt er als Ader, in OpenCV-Lab als Haut; dann ergeben sich formal 31 Vertauschungen. Das kommt allein von der Klassengrenze und ist kein Defekt. | gruen |
| Adern dunkler als die Haut | Ader-L\* < Haut | Ader-max gegen Haut-p10/p50: Z1 17,8 < 22,9/26,6; Z2 25,3 < 30,1/33,7; Z3 21,6 < 26,4/30,1; Z5 31,5 < 36,0/42,8; Z6 27,9 < 36,0/39,5; Z7 29,9 < 35,5/42,3. a\* der Adern liegt 4,7-9,6 unter dem Haut-Median, die Adern sind also nicht roeter. Die Ader-Abbildung ist linear (Restfehler 0,0) und hat ein C\* | gruen |

## 2. Ruhe-Masse (eigene Definitionen, Massstab aus G4 und EM30-Haut)

**Eigene Definitionen:**
- **Figurmaske:** um 2 px erodiert (Video: 3 px); EM30 ohne die Meshes 0-4 und 6.
- **S5:** 24 Sektoren, gezaehlt werden Pixel mit C\* >= 6 und Sektoren mit >= 5 %.
- **Spanne:** kleinster Bogen, der 90 % enthaelt.
- **Block:** C\*-gewichtete Kreisstreuung sqrt(-2 ln R) aus 16-px-Bloecken.
- **Farbfremd:** 4er-Komponenten >= 6 px, mehr als 45 Grad vom C\*-gewichteten Hauptton entfernt.
  Diese Definition ist einfacher als im Messblatt; der Massstab ist deshalb neu aus G4 und EM30 bestimmt.

**Weisses Licht** (Mittel der 4 Winkel; S5 gepoolt):

| Mass | G4 | EM30 | Schwelle (eigen) | K5e | K5c | K5d |
|---|---|---|---|---|---|---|
| S5 gepoolt | 4 | 4 | <= 5 | **1** | 6 ROT | 6 ROT |
| Spanne 90 % | 68,9 | 60,3 | <= 83,9 (max + 15) | **3,2** | 88,5 ROT | 88,7 ROT |
| Block-Farbton | 13,31 | 8,51 | <= 16,6 (1,25 x max) | **4,53** | 26,5 ROT | 24,3 ROT |
| farbfremd je 10 000 px | 12,24 | 11,68 | <= 15,3 | **0,44** | 18,19 ROT | 17,79 ROT |
| farbfremde Flaeche | 1,96 % | 2,09 % | <= 2,61 % | **0,07 %** | 11,5 % ROT | 9,9 % ROT |
| C\*-Std (Info) | 13,56 | 7,64 | | 3,20 | 8,98 | 8,22 |
| L\* (Info) | 19,1 | 30,4 | | 24,7 | 8,8 | 14,4 |

Gegen die Bauer-Zahlen: R1 1, R3 3,4 und R4 4,5 stimmen mit meinen 1 / 3,2 / 4,53 ueberein.

**Szenenkette ohne Filter, ganze Figur** (K5e in Klammern ohne Zeilen 0/8/9):

| Mass | G4 | EM30 | Schwelle | K5e (ohne Sonder) | K5c | K5d |
|---|---|---|---|---|---|---|
| Bild 1 S5 | 4 | 4 | <= 5 | 3 (3) | 6 ROT | 6 ROT |
| Bild 1 Spanne | 44,5 | 42,5 | <= 59,5 | 43,5 (43,7) | 69,9 ROT | 82,3 ROT |
| Bild 1 Block | 9,86 | 7,07 | <= 12,3 | 9,61 (9,87) | 17,15 ROT | 18,88 ROT |
| Bild 2 Block | 8,67 | 8,32 | <= 10,8 | 7,07 (5,30) | 20,96 ROT | 17,76 ROT |
| Bild 2 S5 / Spanne | 4 / 46,0 | 4 / 40,4 | <= 5 / <= 61,0 | 4 / 54,6 (2 / 9,0) | 7 / 81,4 ROT | 3 / 56,4 |

**Mit Videofilter (nur Info, der Filter glaettet):**
- Bild 1 Block: K5e 4,94; G4 13,36; EM30 7,56.
- Bild 2 Block: K5e 10,61; G4 6,66; EM30 5,32; K5c 11,72; K5d 11,66.

**Bewertung:** K5e liegt in allen bindenden Zeilen innerhalb des Massstabs. K5c und K5d liegen in allen Zeilen unter weissem Licht ausserhalb, dazu in Bild 1 S5, Spanne und Block sowie in Bild 2 Block. **Gruen.**

## 3. Wuensche (Albedo = Texel im UV-Polygon ueber die Zeile der Flaeche; eigener Code)

| Wunsch | Soll | K5e | K5c | K5d | Bewertung |
|---|---|---|---|---|---|
| Gelenke heller (Albedo) | dL\* >= 6 | Gelenk-Median 39,46 gegen Nachbarhaut 30,14, also **+9,32**; C\* 6,0 | 0,0 | +10,15 | gruen |
| Gelenke heller (gerendert, weisses Licht, Ring 6 px auf denselben Meshes) | jeder Winkel >= 5 | **5,2 / 7,4 / 7,64 / 6,31** (min 5,2, Mittel 6,64) | -0,37..1,71 | 5,08..12,64 | gruen (knapp, 3/4 vorn 5,2) |
| Schwanzspitze fleischrot (Mesh 6) | Anteil a\* >= 12 & C\* >= 15 mindestens 50 %, dL\* gegen Meshes 1-5 >= +5 | **85,4 %**, **+8,64**, Lab 42,3/16,3/-4,1 | 6,8 %, 0 | 21,2 %, +9,6 | gruen |
| Buckel-Fleisch zusammenhaengend | 12 Flaechen, 1 Komponente (eigene Union-Find ueber gemeinsame Ecken), Ton -8 +- 15, Anteil >= 40 %, dL >= 3 | 12, **1**, Zeile 7, Ton **-14,0**, **57,1 %**, **+5,40** | Ton -33,2, dL -1,3 | Ton -19,1, 92,5 %, +11,3 | gruen |
| Lippe + Band ohne Rot, ohne Leuchten | Rot (a\* > 15 & C\* > 15) 0 %, L\*-p95 <= Haut | **0,0 %**, 37,12 <= 43,9, Zeilen 1/2 (Haut) | 54,4 %, 58,2 > 35,0 | 3,7 %, 43,1 > 39,2 | gruen |
| Unterkiefer aussen (Mesh 23 ohne Zahn/Maulinneres, 79 Flaechen) | grau, ohne Rot, ohne Leuchten | Zeilen 1/2, Rot **0,0 %**, L\*-p95 33,66, Lab-Median 26,6/10,2/-7,1 | Rot 54,7 %, p95 58,2 | Rot 15,4 %, p95 54,1 | gruen |
| Krallen = Hand (13/18) | dE76 <= 6, Rot 0 % | **0,00**, **0 %** (gleiche Zeile 2) | 30,6 / 47,2 % | 3,3 / 0 % | gruen |
| Helligkeit Bild 2 (Pixelmenge V4F 3.1, 42 350 px) | Video >= Vorlage | Vorlage 15,39 (meine Lab-Formel; Bauer 15,37); K5e **15,51** (K5c 10,93, K5d 15,50); ohne Filter 8,64 >= 7,33 | | | gruen (Abstand 0,12 L\*) |
| F1 bleibt | Zeile = K5c-5, UV gleich | Zeile 8 bytegleich K5c-5, 6 Flaechen lesen Zeile 8, UV 3828/3828 gleich | | | gruen |

## 4. Technik

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| CLUT-Zeilen / Port-Texturhoehe | <= 16 / <= 4096 (Messblatt T1; Plan 10 / 2560) | **10 / 2560** (K5c und K5d 16 / 4096) | gruen. Den Auftragstext "16 Zeilen, Texturhoehe 4096" lese ich als Obergrenze; K5e ist der Plan aus Messblatt 3.3. |
| CLUT-Indizes gueltig | jede Flaeche liest eine Zeile 0..9, clut_x passend | Zeilen min 0, max 9; clut_x 0 ueberall; Flaechen je Zeile 99/324/1934/579/6/360/314/92/6/114 = 3828 | gruen |
| Kein Wort 0x0000 neu oder verloren | 0 | 0 neu und 0 verloren in allen 10 Zeilen; genutzte Texel mit Wort 0: **0** | gruen |
| Geometrie = K5d | Ecken, Normalen und Flaechen gleich | **64/64** Bloecke (32 Meshes x Tri/Quad) gleich K5d; gegen K5c 56/64 (die 8 Krallenbloecke weichen ab) | gruen |
| Eckindizes = K5c | 3828 | 3828/3828 | gruen |
| EDD/EMR/Morph/Paare 2-3 = Spiel-Asset K5c | byte-gleich | Sektionen 0-6 **byte-gleich**; 28 Clips, 32 Meshes | gruen |
| Spiel-Asset unveraendert | 1ab56076 / 7187ff09 | 1ab56076 / 7187ff09 nachgerechnet; git status sauber | gruen |
| Zweiter Generatorlauf | byte-gleich | `kf_k5e.py k5e_par.json` -> scratchpad/k5e/pruefer_r1/lauf2: FORM.EMD, FORM.TIM, BAHNSTEIG_FORM.EMD/.TIM, form.json und bau.json alle **gleich** (f277916d / 0a700ba5 / 0a4894f2) | gruen |
| ctest (selbst, umgeleitet) | Summenzeile | RE15_RE2_ASSET_ROOT=ruhe/K5e, ctest -R "form, bahnsteig, birkin_form, r41_form" -j 3: **"100% tests passed, 0 tests failed out of 11"**, 95,27 s, rc 0 (Log scratchpad/k5e/pruefer_r1/ctest.log). Kein Code nach a4108c761 geaendert, der Bau war deshalb nicht noetig. Die Variable wird in asset_root_pc.c:304 gelesen. | gruen |

## 5. Hinweise (gelb, kein Rot)

1. **Bild 1 ist 2,4-mal so hell wie die Vorlage.**
   - Video: K5e 43,40 gegen Vorlage 18,33; ohne Filter 16,57 gegen 7,62.
   - Zum Vergleich: K5c 16,44, K5d 27,69 (zu K5d sagte der Nutzer "ja, besser").
   - Das ist Messblatt Offen 1 und in FARBE.md R.5 Punkt 1 gemeldet; in der Bauer-Zusammenfassung an den Orchestrator fehlt es.
   - Der Nutzerwunsch lautet "ein wenig heller ... zumindest die Helligkeit der Vergleichsbilder". Bild 2 trifft die Vorlage genau, Bild 1 liegt weit darueber.
   - Entscheid Orchestrator oder Nutzer.
2. **Die F1-Leuchtstelle wirkt jetzt wie ein dunkler Fleck mit hellem Kern.**
   - Weisses Licht, "Seite", 455 px: Mittel **-8,9 L\*** gegen den Ring (K5c +9,4, K5d +7,7); p90 +23,9.
   - 3/4 vorn: -16,8 (14 px).
   - Das ist im neuen, helleren Umfeld genau der vom Orchestrator befuerchtete "aufgemalte Fleck". Laut Auftrag wird das nur gemeldet; FARBE.md R.5 Punkt 2 meldet es ebenfalls.
3. **Farbton und Buntheit nach der Rundung:**
   - Die Haut-Eintraege liegen bei **-34,4 bis -34,6 Grad**, C\* 12,3-12,7, gemessen in meiner Formel und in OpenCV-Lab gleich. Der Parameter ist -26,3 / 11,68.
   - Gelenk: -35,2 Grad, C\* 5,9 statt -26,3 / 4,8.
   - Die Verschiebung ist in allen Zeilen eines Materials gleich, kommt also aus der 15-Bit-Rundung und nicht aus einer Eichung je Zeile. Die Regel ist eingehalten; die Angabe "h -26,3" in k5e_par.json ist aber der Sollwert, nicht das Ergebnis.
   - Dazu: Unter weissem Licht ist K5e weit ruhiger als die Originale.

     | Mass | K5e | G4 | EM30 |
     |---|---|---|---|
     | S5 | 1 | 4 | 4 |
     | Spanne | 3,2 Grad | 68,9 Grad | 60,3 Grad |
     | C\*-Std | 3,2 | 13,6 | 7,6 |

   - Der Massstab ist nur nach oben begrenzt, das ist also gruen. Ob die Haut dadurch flach wirkt, zeigt erst das Bild fuer den Nutzer.
