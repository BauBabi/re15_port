# Prüfung K5g3, Runde 2: Zahlen-Prüfer

Stand 2026-11-14. Geprüft wurden die Commits 037e0aeea, 9928ec91a und f48d37df1 auf dem Zweig r41/form. Asset: BAHNSTEIG_FORM.EMD 482bd63f…, BAHNSTEIG_FORM.TIM = FORM.TIM ce9a1dc9…, FORM.EMD 9eab983e…; die sha256 sind nachgerechnet.

Alles ist mit eigenem Code gemessen. Die Skripte liegen unter
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/pz4/`:

| Skript | Aufgabe |
|---|---|
| geo_check.py | Geometrie |
| uvchk.py | UV gegen die Geometrie-Stufe plus Kopie-Versatz |
| v3a-v3h.py, kind.py, strict.py, loch.py | Flecken, Kopien, Spalt-Kinder, Index 0 |
| struktur5.py, struktur6.py | Helligkeitsstruktur in den Flecken gegen die G4-Originalfarbe |
| auge1.py, auge2.py | Augen im Texel- und im Modellraum |
| farbe.py, kralle.py, hell.py, f1.py, hue.py | Farbe der Zonen |
| lauf.sh, serie4.sh | eigene Spielläufe |
| mass.py, mass2.py, ruhe_eigen.py, auge_spiel.py, auge_hell.py, braun.py, rot.py | Messungen im Spiel |

Die Belegbilder liegen in `.../pz4/belege/`, die Spielläufe in `.../pz4/laeufe/`.

## Ergebnis: NICHT BESTANDEN (1 roter Punkt)

**ROT:** Die bräunliche Musterung (Zonen 16 und 19) ist eine flache Farbinsel. V3 verlangt: „Innerhalb eines Flecks bleibt die Helligkeitsstruktur der Original-Texel … keine flache Farbinsel“ (Abschnitt 3.2).

Alle Befunde der Runde 1 sind behoben:
- Die weiße Musterung ist jetzt fast unbunt.
- Der weiße Strich auf dem Buckel ist weg (Index 0 wird 0-mal gelesen).
- Die Augen sind im Spiel sichtbar, an beiden Nutzerpunkten.
- Das Braun ist nicht mehr beige.

Grün sind außerdem:
- Geometrie G1/G2/G5
- G3 mit eigenem Kampflauf
- V3 Lage und Exklusivität
- V4, V5, O5, V6
- Zielband Z1-Z9 und Ruhe
- Generator byte-gleich
- ctest

## 1 Geometrie (G1, G2, G5): GRÜN

Gemessen mit geo_check.py und uvchk.py.

| Prüfung | Ergebnis |
|---|---|
| Sektionen 0-6 (Morph, EDD/EMR aller Bänke, Posen) | byte-gleich K5e_f1, K5c und KAND. Die Posen sind die von K5c. Eine krallen_posen.json gibt es im generator/-Ordner nicht |
| Netze 10, 16, 21, 22: Ecken, Normalen, Flächen-Ecken | gleich KAND.EMD (B4). Die sha256 von KAND f78e5983… und TABELLE 99a4f142… sind nachgerechnet |
| Netze 19, 20, 28, 29 | Ecken und Normalen gleich K5c (einbau_k5c 1ab56076…), Flächen-Ecken gleich K5e_f1 |
| Netz 31 | gleich K5e_f1, und K5c = K5e_f1 |
| übrige 24 Netze | gleich K5e_f1 |
| Asset gegen die Geometrie-Stufe geo/BAHNSTEIG_FORM.EMD (db9404a1…) | alle 32 Netze geometrisch gleich |
| UV aller 13 173 Flächen gegen geo/ (uvfein) + Kopie-Versatz der Mutter (bau.json kopien) | 11 549 gleich, 1 613 genau um den Versatz verschoben. 11 weichen ab, das sind genau die 11 Augen-Dreiecke aus bau.json augen_dekal (22\|0\|287, 288, 4071, 4082, 4084, 4085, 4148, 4151, 4180, 4181, 4183) |
| Spiel-Asset shared_assets/RE2 | EMD f277916d…, TIM 31a35e66… = K5e_f1, unverändert (G5) |
| Geänderte Pfade e225e7fb5..f48d37df1 | 37 Dateien, alle unter analysis/…/K5/. Nichts in Version_80%/, pri/, synchro/, shared_assets/ oder re15_port/ |

## 2 Dreiecks-Liste (G3): GRÜN

- **Code:** Seit Runde 1 gibt es keine C-Änderung (git diff da6560372..HEAD -- re15_port ist leer).
  - render_pc.c:226 setzt TEXTRI_QUEUE_MAX auf 16384.
  - Am Makro hängen s_textri_* (Zeilen 462-465), s_sb_* (493-496), s_textri_flush_buf[*3] (509) und order[] (1209).
  - Verworfene Dreiecke werden gezählt (470, 1378-1387). Es gibt die Abfrage re15_render_pc_textri_verworfen/_hoechststand (569/570) und die Zeile `[tri-liste] ENDE` (574).
  - Speicher: 196 B je Eintrag × 16384 = 3,06 MiB (vorher 1,53 MiB), dazu 64 KiB Stapel für order[]. Das ist die Zählung der Runde 1; der Code ist unverändert.
- **Eigene Spielläufe** (eigene exe-Kopie, Modell K5g3 Runde 2 über RE15_RE2_ASSET_ROOT, ohne SOFTWARE_RENDER):

| Lauf | Aufbau | Schüsse (mg) | hoechststand | verworfen |
|---|---|---|---|---|
| kampf_5090 | 5090, freier Kampf, Pistole | 144 → 57 = 87 Schüsse, Form HP -1 | 7415 (F522) | **0** |
| s11_5090 | 5090 Schnitt 11, Form Clip 12 | 144 → 100 = 44 Schüsse | 7718 (F829) | **0** |
| v5080s | 5080 von vorn, Clip 12 | 144 → 100 = 44 Schüsse | 7184 (F941) | **0** |
| 20 Bild-Läufe | z5080, v5080, b2_5090, griff_5090, Marker | – | höchstens 6813 | **0** |

`UEBERLAUF` kommt in keinem debug.log vor. Kein Absturz.

- **ctest selbst:** `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3` mit RE15_RE2_ASSET_ROOT=k5g/K5g3 ergibt „100% tests passed, 0 tests failed out of 12“ in 276,9 s. Darin integration_r41_form_dreiecke 276,86 s, Passed. Protokoll: `pz4/ctest.log`. `local_build.sh build` meldet „ninja: no work to do“.

## 3 Flecken (E3, V3)

Die Texel-Abdeckung jeder Fläche ist großzügig gerechnet (cover.py). Index 0 ist streng geprüft, abgetastet 16×16 im Inneren der Dreiecke.

Material-Einträge sind die Einträge, die von der Basiszeile abweichen. Basiszeilen: 10→3, 11→5, 12→5, 13→6. Zeile 14 ist die neue Zeile „auge“ und wird getrennt geprüft.

| Prüfung | Ergebnis |
|---|---|
| Kopien (150): reine Verschiebung / Zielplatz frei / Ziel-Texel geteilt | 150 / ja, 0 unverschobene Flächen lesen ein geändertes Pixel / 0 |
| Index-0-Zugriffe (ungeschriebener Kopie-Texel) | **0** großzügig, **0** streng (Runde 1: 26) |
| Spalt-Kinder: Kinder verschobener Mütter mit Texeln außerhalb des Kopie-Rechtecks | 0 von 1544 |
| Spalt-Kinder, die Fleck-Material lesen | 17 985 im Fleck der Mutter, 69 Zone-27-Texel knapp außerhalb der Mutter-UV (dieselbe Zone), **0 fremd** |
| Nicht-Träger, die eine Fleck-Zeile lesen | nur die 8 Loch-Flächen 22\|0\|28-35 und ihre 8 Kinder 22\|0\|234-241. Sie lesen die Einträge 70-103 der Zeile 12; kein echter Fleck-Träger liest diese Einträge. Farbe = Haut: L\* Median 23,2, a\* 11,3, b\* 4,6, gleich haut1. Kein Fleck-Material: E3 „Rest des Augenfelds wird Haut“ |
| Gegen traeger_texel.json (je Fläche): erwartet 11417, Material 11322, **zu viel 0** (ohne Loch-Haut) | Es fehlen 218: 125 Zone 2 + 4 Zone 3 F1-Kern-Vorrang (V6), 61 Zone 27 + 1 Zone 29 außerhalb der UV-Fläche, 4 zufällig gleiches Wort, 23 Zone 29 (die Augen laufen jetzt über eigene Dreiecke, siehe unten) |
| Mundfleisch-Zeilen 0 und 1 | gelesen nur von den 27 Flächen der Zone 30 (23 + 4). 0x8000 kommt 7-mal vor, nur in Zone 30. 0x0000 und 0x7FFF auf Nicht-Zahn-Flächen: 0 |
| Zahn-Zeile 2 | nur die 114 Flächen der Zone 41. Gelesene Farben gleich K5e_f1 (L\* 74,2, C\* 5,3); nur die 228 Zugriffe auf 0x8000 sind durch ein nicht transparentes Schwarz ersetzt (Anteil schwarz 1,60 % gleich) |
| Texel-Konflikt 14/15 | nach „kleinere Zone“ an 15; Zone 14 trägt nur 1 Texel-Flächen-Paar. Das ist berichtet (V3.2) |

### Augen (E3 Zone 29, O4): GRÜN

Gemessen mit auge1.py und auge2.py.

- **Texel:** Die Zeile 14 („auge“) lesen nur die 11 Augen-Dreiecke. Helle Einträge (L\* > 40) sind 2, 3, 5, 6, 10, 11 und 12.
- **Lage im Modellraum:** Jeder helle Texel wurde baryzentrisch in den Knochenraum von Teil 22 gerechnet und sein Abstand zur Augenmitte aus generator/k5g3_augen_mitten.json bestimmt. Die Mitten habe ich über das Dreieck und die bary-Werte nachgerechnet; sie sind gleich.

| Auge | Anteil heller Proben mit Abstand ≤ Radius | P95 Abstand/Radius | Max Abstand/Radius | Anteil dunkler Proben innerhalb des Radius |
|---|---|---|---|---|
| rechts | 92,5 % | 1,04 | 1,76 (Texelraster) | 1,5 % |
| links | 95,2 % | 1,00 | 1,11 | 0,6 % |

  Augen-Material liegt also nur auf den beiden Augenscheiben, und diese sind fast vollständig hell.
- **Mütter:** Die Mutter von 288 ist 22\|0\|82, die von 4082 ist 22\|1\|112. Genau diese Flächen liegen laut zonen_v2 unter den Nutzerpunkten.
- **Im Spiel**, Marker-Läufe (Augen-Einträge grün):

| Ansicht | Auge | Pixel | Lage | L\* Median | Ring daneben | Faktor | K5e_f1 an derselben Stelle |
|---|---|---|---|---|---|---|---|
| 5080 von vorn | 1 | 9 | (259,236) | 15,5 | 5,1 | 3,1 | 3,1 / 7,7 |
| 5080 von vorn | 2 | 15 | (332,240) | 17,3 | 2,8 | 6,1 | 7,1 / 5,7 |
| 5090 Bild-2-Pose | 1 | 5 | (223,267) | 19,4 | 5,1 | 3,8 | |
| 5090 Bild-2-Pose | 2 | 9 | (281,268) | 17,4 | 3,1 | 5,6 | |

  - In der Bild-2-Pose sind die Nutzerpunkte (217,274) und (280,275). Die Spielkamera ist nicht die Videokamera.
  - In Runde 1 war im Spiel nur ein Auge mit Faktor etwa 1,9 zu sehen.
  - Bild: `belege/augen_v5080.png` (K5e_f1 | K5g3 | Marker).
- Die alten G4-Augenflächen sind jetzt Haut (Zeile 7, Rang-Tausch, L\* Median 9,5). In K5e_f1 waren es 5,1 mit 9,7 % Schwarz.

### ROT: Bräunliche Musterung (Zonen 16 und 19) als flache Insel

Eigene Messung mit struktur5.py und struktur6.py. Die G4-Originalfarbe kommt aus CDEMD0.EMS (G4-CLUT der Seite) am Original-Index.

**Achtung:** Die K5c-Zeile 2 im einbau_k5c ist NICHT die G4-Zeile 2 (254 Einträge verschieden). Wer gegen K5c misst, misst falsch.

| Zone | Seite | Texel | G4: cv / Stufen | K5g3: cv / Stufen | Spearman |
|---|---|---|---|---|---|
| 9, 11, 12, 27, 24 | 1 | 135-9540 | 0,33-0,58 | 0,24-0,45, 9-38 Stufen | 0,90-0,99 |
| 14, 15, 18 | 2 | 15-16 | 0,25-0,54 | 0,23, 8-9 Stufen | 0,95-0,98 |
| 3 | 2 | 35 | 0,54 | 0,22, 10 Stufen | 0,97 |
| 4 | 2 | 24 | 0,30 | 0,10, 5 Stufen | 0,82 |
| **16** | 2 | 94 | **0,42 / 23 Stufen** | **0,06 / 2 Stufen** | 0,87 |
| **19** | 2 | 16 | **0,28 / 7 Stufen** | **0,06 / 2 Stufen** | 0,90 |

Die Rangfolge bleibt überall erhalten. Die Zonen 16 und 19 haben aber nur noch **zwei** Farbwerte, Zeile 13 Index 9 (L\* 32) und Index 20 (L\* 36).

- **Ursache:** Die Braun-Rampe hat nur 3 G4-Einträge (bau.json braun.eintraege 1:148, 2:123, 2:170). Seit Runde 2 sind alle Einträge über P90 der Haut abgeschnitten. Der Rang-Tausch auf diese Rampe ergibt 2 Stufen.
- **Im Spiel**, 5080 Bild-1-Lage (Marker z5080_braun, 103 Pixel):

| Lesart | K5g3 | K5e_f1 |
|---|---|---|
| Q (Albedo) cv | **0,062** | 0,234 |
| P (Spiellicht) cv | 0,149 | 0,333 |
| verschiedene Farben | 37 | 48 |

  Im Bild ist der linke Fleck eine glatte dunkle Fläche ohne Maserung. Bild: `belege/braun_z5080.png` (K5e_f1 | K5g3 | Marker).
- **Behebung (Vorschlag):** Braun als EINE Transformation auf die G4-Helligkeit der Zonen-Texel: L' = f × L_G4 (Knie wie Haut), Farbton und Buntheit aus den braunen G4-Einträgen (Ton 70°, C\*/L\* der Quelle). So bleibt die Struktur der 23 bzw. 7 Original-Stufen erhalten.
- **Riegel (Vorschlag):** In jeder Fleck-Zone gilt cv(K5g3) ≥ 0,5 × cv(G4) und mindestens 5 Stufen (bei ≥ 15 Texeln), gemessen gegen die G4-CLUT aus der EMS. Gegenprobe: der heutige Stand (Zone 16: 0,06 gegen 0,21).

## 4 V4 Hals, Lippe, Kiefer, Mund: GRÜN

Albedo der gelesenen Texel (farbe.py):

| Bereich | K5g3 | K5e_f1 |
|---|---|---|
| Zone 45 Kopf-Unterseite | L\* 26,9, a\* 11,1, C\* 12,0, 0× 0x8000, nur Zeile 7 | L\* 37,4, a\* 46,7, C\* 49,9, 11× 0x8000 |
| Zone 42 Lippe | L\* 23,3, a\* 11,1, schwarz 0,20 % | |
| Zone 31 Unterkiefer | L\* 23,3, a\* 11,1, schwarz 0,37 % | |
| Zone 40 Augenhöhle | L\* 26,9, a\* 11,1 | |

Die Werte für Lippe, Unterkiefer und Augenhöhle liegen im Band der Haut (L\* 23,3, a\* 11,1).

**Im Spiel**, eigener Lauf griff_5090 (Clip 27 Bild 20, Hals-Unterseite): Die roten Streifen von K5e_f1 sind weg. Rot ist nur das Maul-Innere im Spalt (Zone 30, erlaubt). Bild: `belege/griff_hals_zoom.png` (K5e_f1 | K5g3).

**Hinweis (gelb, wie Runde 1, beim Bauer weiter offen):** Die Zone 44 (Hals-Unterseite) trägt auf 5 Flächen Zone-27-Fleisch. Das sind 2722 Zugriffe auf Zeile 12. Im Griff-Bild ist das kein roter Streifen. Der Vorrang von Zone 27 ist noch nicht vom Orchestrator bestätigt.

## 5 V5 Krallen, O5 Hände, V6 F1: GRÜN

| Punkt | K5g3 | Bewertung |
|---|---|---|
| V5 Krallen links / rechts | L\* 28,0 / 28,0, a\* 14,3, C\* 15,4; Hand L\* 26,9, a\* 11,1, C\* 12,0. 0× 0x7FFF, 0× 0x8000 | grün |
| O5 Hände 13 und 18 | L\* 26,9, 0× 0x7FFF, nur Zeile 8 | grün |
| V6 F1 | 25,8 % der Zugriffe byte-gleich K5c (Kern). Neben dem Kern 11 von 105 Rand-Texeln mit L\* < 8; Haut 4,25 %; K5c 45,8 % | grün, kein schwarzer Ring |
| Zone 12 rotes Fleisch | alle 38 Wörter aus der F1-Quelle (K5c-Zeile 5) | grün |

Hinweis zu V5 (gelb): Die Kralle trifft „nicht reinweiß“ nur dem Wortlaut nach. 2,5 % der Krallen-Zugriffe lesen 0x631F (L\* 84,5, rosa-weiß). Bei den Händen sind es 0,2-0,4 %.

## 6 V1, V2, O3 im Spiel: GRÜN, Z9 knapp

Eigene gdigrab-Läufe (lauf.sh):
- eigene exe-Kopie, Fenster nur über die eigene PID
- WINDOW_SCALE 2, 640×480
- kein AUTOSHOT, kein SOFTWARE_RENDER
- Normal- und Schlüssel-Lauf je Variante

Szene: 5080 Bild-1-Lage, Pose -20711,-15984,756,0,11, FORCE_CUT 1. Gemessen mit mass.py und mass2.py.

| | K5g3 r2 | K5e_f1 | Band der Originale |
|---|---|---|---|
| Z1 P L\* | 7,79 | 9,45 | 5,57-18,80 grün |
| Z2 P L-cv | 0,708 | 0,409 | 0,481-1,431 grün |
| Z3 P lokal_rel | 0,363 | 0,168 | 0,243-0,656 grün |
| Z4 P C\* | 4,84 | 4,33 | 4,41-7,59 grün |
| Z5 P C-std | 3,19 | 1,76 | 2,39-7,45 grün |
| Z6 Q L-cv | 0,517 | 0,238 | 0,369-1,020 grün |
| Z7 Q lokal_rel | 0,288 | 0,115 | 0,210-0,528 grün |
| Z8 Q C\* | 13,73 | 11,51 | 12,09-15,05 grün |
| Z9 Q C-std | **6,571 (8-Bit-Q) / 6,518 (float-Q)** | 3,98 | 6,53-8,88: **knapp** |

Die Werte stimmen mit dem Bauer überein (Z9 6,57).

- **Gegenprobe:** K5e_f1 ist rot in Z2-Z9.
- **Z9 (gelb):** Nach der Lesart des Messblatts (8-Bit-Q) liegt der Wert 0,04 über der Grenze, ohne Rundung 0,01 darunter.
- **Weißes Licht:** Das Spiel kennt keinen Schalter dafür (RE15_FORCE_LICHT und RE15_KEIN_RAUMLICHT sind keiner). Darum steht Q als Albedo-Lesart. Gegen das W-Band liegen L-cv 0,517, lokal 0,288, C-std 6,57 und C\* 13,73, alles im Band.
- **Hautton (V2):**

| Lesart | K5g3 | K5e_f1 | K5f |
|---|---|---|---|
| Spiel P | +19,7° | -26,8° | |
| Spiel Q | +20,9° | -29,8° | |
| Albedo | +22,4° | -34,4° | -27,0° |

  Im Band 14-27° und wärmer als K5e_f1 und K5f: grün.
- **Ruhe, zweiseitig** (eigene Umsetzung nach Messblatt RUHE, Q-Bild):

| | K5g3 | K5e_f1 | Grenze |
|---|---|---|---|
| S5 | 1 | 3 | ≤ 5 |
| F_fremd | 2,2 / 0,19 % | 0 | ≤ 7,4 / 0,64 % |
| Spanne90 | 1,3° | 26,5° | ≤ 76,8 |
| Block | 5,0° | 8,4° | ≤ 16,3 |

  Grün. „Nicht flacher“ ist über Z2/Z3/Z5-Z9 belegt.
- **V1, multiplikativ:** Nachgerechnet gegen die G4-CLUT der EMS:

| Zeile | Fit | Spearman |
|---|---|---|
| 4 | L' = -0,90 + 4,822·L | 0,998 |
| 5 | L' = -0,08 + 1,866·L | 0,999 |
| 6 | L' = 1,53 + 2,491·L | 0,999 |

  Die Achsenabschnitte sind klein, die Haut ist also multiplikativ.

## 7 Gelbe Punkte (nicht rot)

1. **V1, drei Seitenfaktoren der Haut** (4,8 / 1,87 / 2,49): Das war Gelb 1 in Runde 1 und ist beim Orchestrator weiter offen.
2. **Fast weiß auf der Haut, nur formal behoben:** Die Haut liest kein 0x7FFF mehr, aber 0x7FFE (L\* 99,3, 35 Zugriffe), 0x7BDF (97,4, 148) und 0x77BF (95,3, 128). Dazu kommen 1 % der Rumpf-Zugriffe mit L\* > 80. Im Spiel (z5080) haben 107 Q-Pixel L\* > 90 (0,30 %); in Runde 1 waren es 104, bei K5e_f1 sind es 0.
3. **Farbtonspanne der Haut** 1,3° (Q) / 2,4° (P). Die Originale liegen bei P-Spanne Gorilla 54 und Tyrant 42, K5e_f1 bei 29,4, Plan v7 bei 19,9. Das Messblatt kennt keine Untergrenze. Ob die Haut „plastisch“ wirkt, muss der Sicht-Prüfer beurteilen.
4. **Zonen 3 und 4 auf Seite 2:** cv 0,22 bzw. 0,10 gegen G4 0,54 bzw. 0,30. Die Struktur ist dort gestaucht, aber mit 10 bzw. 5 Stufen nicht flach.
5. **Z9 knapp** (Abschnitt 6). **Zone 44 trägt Zone-27-Fleisch** (Abschnitt 4).
6. **Weiße Musterung im Spiel:** Die Musterung (C\* 0) erscheint im Spiel als graue Fläche auf rotbrauner Haut, siehe `belege/braun_z5080.png` oben rechts. Das folgt W6; der Eindruck („aufgemalt“?) ist Sache des Sicht-Prüfers.

## 8 Technik: GRÜN

- **Generator zweimal selbst gelaufen** (`python generator/kf_k5g3.py bauen <pz4/gen1>` und `<pz4/gen2>`, je 33 s): Alle 11 Dateien sind gleich untereinander **und** gleich der Abgabe, nämlich BAHNSTEIG_FORM.EMD 482bd63f…, .TIM ce9a1dc9…, FORM.EMD 9eab983e…, FORM.TIM, form.json ba2147b3…, bau.json 0ced82c3… und pruef/*. Im Worktree hat der Generator nichts verändert.
- **T1:** 15 CLUT-Zeilen, Port-Höhe 3840 ≤ 4096. Alle Flächen lesen die Zeilen 0-14.
- **Commits:** auf r41/form, Deutsch, die letzte Zeile ist „Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>“, keine Backticks, kein Push (es gibt keinen Remote-Zweig).
- **Spielläufe:** nur eigene PIDs beendet, gdigrab nur auf das eigene Fenster (HWND über die eigene PID).

## 9 Was der Bauer tun muss

1. **Zonen 16 und 19:** Die Braun-Transformation muss die G4-Helligkeitsstruktur behalten (Abschnitt 3, Vorschlag mit Riegel). Danach das Spielbild z5080 mit dem Braun-Marker wiederholen: Q-cv nicht unter etwa 0,5 × K5e_f1 (0,23). Die übrigen B1-Werte (L\*, Ton) müssen bleiben.
2. **Optional:**
   - Z9 mit Reserve
   - fast weiße Haut-Wörter (Gelb 2) nicht nur am Wort 0x7FFF begrenzen, sondern z. B. über das Knie mit einer Obergrenze
   - Entscheide des Orchestrators zu Gelb 1 und zu Zone 44/27
