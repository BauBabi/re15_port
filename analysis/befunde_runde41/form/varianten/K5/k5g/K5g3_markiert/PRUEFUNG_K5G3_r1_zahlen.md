# Prüfung K5g3, Runde 1: Zahlen-Prüfer

Stand 2026-11-14. Geprüft wurden die Commits 8dddcbd56, da6560372 und 05d884816 (davor e35810d6d, c75f7b06e, d36d6b932) auf dem Zweig r41/form. Alles ist mit eigenem Code gemessen. Die Skripte liegen unter
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/pz3/`:

| Skript | Aufgabe |
|---|---|
| emdp.py | EMD/MD1-Leser |
| timp.py | TIM-Leser |
| cover.py / strict.py | Texel-Abdeckung je Fläche |
| geo_check.py | Geometrie |
| v3a-v3h.py | Flecken |
| farbe.py / weiss.py / f1.py / hue.py | Farbe |
| lauf.sh | eigener Spiellauf mit gdigrab |
| mass.py / mass2.py | Zielband P/Q |

Die Belegbilder liegen in `.../scratchpad/k5g2/pz3/belege/`.

## Ergebnis: NICHT BESTANDEN (2 rote Punkte)

Rot sind:
1. **Die weiße Musterung ist nicht weiß.** Die Zonen 14, 15, 18 und 24 sind genauso bunt wie das Fleisch (V3).
2. **Auf dem Buckel liegt ein sichtbarer heller Strich.** Er entsteht aus 26 reinweißen Texeln (0x7FFF) an der Fläche 10|1|76, einer Zone-27-Fläche. Ursache ist die Kopie der Fleck-Texel (V3/V1).

Alles andere ist grün oder steht als Hinweis da: Geometrie G1/G2, Dreiecks-Liste G3, die Lage der Flecken, V4, V5, O5, V6, das Zielband Z1-Z9, die Ruhe, der Generator und ctest.

## 1 Geometrie (G1, G2, G5): GRÜN

Gemessen mit emdp.py und geo_check.py.

| Prüfung | Ergebnis |
|---|---|
| Sektionen 0-6 (Morph, EDD/EMR aller Bänke; Posen) | byte-gleich K5e_f1 **und** K5c; Posen = K5c, keine Posen-Datei |
| Netze 10, 16, 21, 22 (Ecken, Normalen, Flächen-Indizes) | gleich KAND.EMD (B4). Die sha256 von KAND f78e5983… und TABELLE 99a4f142… sind nachgerechnet. Zählung: 10 = 804 Ecken / 1483 Dreiecke, 16 = 406 / 640 + 80 Vierecke, 21 = 1688 / 2981, 22 = 2768 / 5073 |
| Netze 19, 20, 28, 29 | Ecken und Normalen gleich K5c, Flächen-Indizes gleich K5e_f1. Die Ecken weichen wie gewollt von K5e_f1 ab |
| Netz 31 | gleich K5e_f1 und gleich K5c |
| übrige 24 Netze | Geometrie gleich K5e_f1 |
| Asset gegen Geometrie-Stufe geo/ (eadfac31) | alle 32 Netze geometrisch gleich |
| Spiel-Asset shared_assets/RE2 | EMD f277916d…, TIM 31a35e66… = K5e_f1, unverändert (G5) |

Dazu ein Hinweis: Die Geometrie-Stufe hat einen eigenen Riegel R8, der rot ist (geo/riegel_k5g3.txt, „10 von 11 grün“). Er misst die Umriss-IoU in Bild 2 mit 0,612 statt mindestens 0,652 und Streifen zwischen Krallen und Kopf in Clip 3 und 9. Beides kommt von den K5c-Krallen in der Haltung von Bild 2. Über diese Haltung entscheidet der Nutzer gerade (G2). Das ist bekannt und in GEOMETRIE.md §8 benannt. Die Textur-Abgabe hat es nicht verursacht.

## 2 Dreiecks-Liste (G3): GRÜN

- **Code:** render_pc.c:226 setzt TEXTRI_QUEUE_MAX auf 16384. Alle Felder hängen am Makro: s_textri_queue, _depth, _slot und _blend, s_sb_queue, _depth, _slot und _blend, s_textri_flush_buf[*3] und order[] in end_frame. Es gibt keine fest eingetragene 8192 mehr. Beide Einreih-Stellen (Zeilen 2805 und 2909) zählen verworfene Dreiecke. Ausgaben: die Zeile `[tri-liste] ENDE` (main.c:3069), die Abfrage `re15_render_pc_textri_verworfen/_hoechststand/_grenze`, und `UEBERLAUF` im debug.log.
- **Speicher, nachgerechnet:** Ein Eintrag braucht 60 + 4 + 1 + 1 = 66 B. Mit Liste, Standbild-Kopie, Flush-Puffer (60 B) und order (4 B) sind es 196 B. 196 B × 16384 = 3 211 264 B ≈ 3,06 MiB (vorher 1,53 MiB). order[] belegt jetzt 64 KiB Stapel in end_frame (vorher 32 KiB).
- **Eigene Spielläufe:** eigene exe-Kopie, Modell K5g3 **mit Textur** über RE15_RE2_ASSET_ROOT, Läufe unter `laeufe/`.

| Lauf | Aufbau | hoechststand | verworfen |
|---|---|---|---|
| kampf2 | 5090, freier Kampf, Pistole | 7415 (F522) | **0** |
| s11_5090_k5g3 | 5090 Schnitt 11, Clip 12 läuft | 7718 (F829) | **0** |
| v5080s_k5g3 | 5080 von vorn, Clip 12 | 7184 (F941) | **0** |
| 37 Bild-Läufe | Bild-1-Lage, Bild-2-Pose, Griff, von vorn, gedreht, 15 Clip-Läufe | höchstens 7318 | **0** |

Was in den Kampfläufen passiert ist:
- kampf2: Magazin 144 → 57, also 87 Schüsse. Die Form fällt auf HP -1, die Kamera steht in 501 Zeilen auf Schnitt 11.
- s11 und v5080s: Magazin 144 → 100, also 44 Schüsse.

Kein Absturz.
- **ctest selbst:** `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3` mit RE15_RE2_ASSET_ROOT=K5g3 ergibt „100% tests passed, 0 tests failed out of 12“ in 277,7 s. Darin läuft integration_r41_form_dreiecke in 277,7 s (Passed). Protokoll: `pz3/ctest.log`.

## 3 Flecken texelgenau (E3, V3)

Die Prüfung ist eigenständig. Die Texel-Abdeckung jeder Fläche ist mit cover.py gerechnet, großzügig: ein Texel zählt, wenn sein geschlossenes Quadrat das geschlossene Dreieck schneidet.

Material-Einträge lassen sich ohne Hilfe des Bauers bestimmen. Die Zeilen 10-14 haben je eine Basiszeile, nämlich 3, 5, 5, 6 und 7; sie ist die Zeile mit den meisten gleichen Einträgen. Material-Einträge sind die Einträge, die von der Basiszeile abweichen: 23, 152, 117, 42 und 4 Stück.

| Prüfung | Ergebnis |
|---|---|
| Material außerhalb der Fleck-Zonen (Vereinigung zonen_v2: Trägerfläche × Zonen-Texel) | **0** |
| Nicht-Trägerflächen, die eine Fleck-Zeile lesen | **0** (Prüf-Stand), **0** (Asset mit Spalt-Kindern) |
| Kopien (147 Flächen): reine Verschiebung / Zielplatz frei / Ziel-Texel geteilt | 147 / ja (keine unverschobene Fläche liest ein geändertes Pixel) / 0 |
| gegen traeger_texel.json (je Fläche): erwartet 11417, Material 11224, **zu viel 0** | Es fehlen 193. Davon sind 129 F1-Kern-Vorrang (Zone 2: 125, Zone 3: 4; bewusst, V6). 62 liegen außerhalb der UV-Fläche ihrer Fläche (Zone 27: 61, Zone 29: 1), sie werden dort nie gelesen. 2 sind ein zufällig gleiches Wort (0x7bdf) |
| Vereinigung traeger_texel = zonen_v2.json (Texel und Trägerflächen) | gleich, alle 14 Fleck-Zonen |
| Spalt-Kinder (10257): Zeile und Seite der Mutter, UV = KAND + Verschiebung der Mutter | 0 falsch. Kein Kind liest außerhalb des Kopie-Rechtecks seiner Mutter |
| Materialarten getrennt | 338 Material-Einträge, keiner gehört zu zwei Arten |
| Augen-Einträge (Zeile 12: 5/7/10/11, Zeile 14: 1/3/4/5) | nur Zone 29 liest sie (23 Paare auf 22\|0\|82, 22\|0\|83, 22\|1\|112, 22\|1\|113, 22\|1\|152). Andere Zugriffe: **0** |
| Mundfleisch-Zeilen 0 und 1 | gelesen nur von den 27 Flächen der Zone 30; Zahn-Zeile 2 nur von den 114 Flächen der Zone 41 |

### ROT 1: Die weiße Musterung ist nicht „fast unbunt“

V3 verlangt: „weiße Musterung = helle, fast unbunte Gelenk-Einträge“. Die Albedo der Material-Texel liegt so (farbe.py):

| Zone | Material | L\* Median | C\* Median | Farbton |
|---|---|---|---|---|
| 14 | weiß | 47,4 | **22,0** | 22,5° |
| 15 | weiß | 43,7 | **22,0** | 22,5° |
| 18 | weiß | 51,7 | **25,2** | 22,5° |
| 24 | weiß | 39,4 | **18,5** | 22,2° |
| Haut (zum Vergleich) | | 23,3 | 12,0 | 22,1° |
| 27 Fleisch (zum Vergleich) | | 36,2 | 25,0 | 24,2° |

Die „weiße“ Musterung ist also bunter als die Haut und so bunt wie das Fleisch. Sie wirkt wie ein helleres Rosa, nicht wie Weiß.

- **Gegenprobe:** An denselben Texeln hat das G4-Original (K5c-Zeilen 0/1 = G4-CLUT, gegen CDEMD0.EMS@0x65e800 geprüft) L\* 10,9 und C\* 0,0. K5e_f1 hat dort L\* 39,4 und C\* 5,8.
- **Ursache:** kf_k5g3_tex.py:356-362 transformiert die weißen Einträge mit `k_haut` (s = 0,5225, also C' = 0,52·L'). In k5g3_par.json steht als Begründung: „Transformation = Haut (k klein = fast unbunt)“. Das stimmte nur in K5g2, wo k aus der Eichung C\* 2-6 kam. Mit dem s aus O3 trägt die Annahme nicht mehr.
- **Riegel:** Kein Riegel prüft die Buntheit der weißen Zonen.

Weitere Wörter teilen sich weiß, Muster und Fleisch, zum Beispiel 0x3192, 0x210c und 0x2950.

### ROT 2: Weißer Strich auf dem Buckel (Fläche 10\|1\|76, Zone 27)

Die Fläche 10\|1\|76 (Seite 1 → 3) wurde als Rechteck 11×77 nach (467,119) kopiert. Geschrieben wurde aber nur die Maske des Bauers: fillPoly in der Eckreihenfolge 0,1,3,2.

Das Spiel zeichnet das Viereck dagegen als die Dreiecke (0,1,2) und (1,3,2). B4 hat es in die Kinder 10\|0\|342, 343 und 344 geteilt. Diese Dreiecke decken einen Streifen entlang der inneren Diagonale ab, in dem die Kopie leer geblieben ist (Index 0). Index 0 ist in **allen** 15 Zeilen von K5g3 jetzt 0x7FFF; in K5e_f1 war er grau, z. B. Zeile 2 = 0x356d.

| Messung | Ergebnis |
|---|---|
| Texel-Abtastung 16×16, nur das Innere der Kinder-Dreiecke | **26** Zugriffe auf Index 0 = reinweiß |
| großzügige Abdeckung | 33 Zugriffe |
| andere Flächen im ganzen Asset | 0 |
| K5e_f1 | kein Zugriff auf Index 0 |

Bild: `belege/kopie_10_1_76_index0.png` (weiß = leere Kopie, grün = Kinder-Dreiecke).

**Im Spiel nachgewiesen** mit einer Marker-Variante (Index 0 in allen Zeilen grün; nur diese Fläche liest ihn):

| Ansicht | grüne Pixel |
|---|---|
| 5080, Gier 3200, Clips 2/3/9/11/27 abgespielt, 60 Bilder | 0-13 je Bild, zusammen 458 |
| Gier 2095 | 7 |
| Gier 756, Bild-2-Pose, Griff | 0 |

Ruhiges Bild, 5080, Gier 3200, Clip 2 Bild 0: 10 Pixel mit L\* 37,4-39,9 (RGB um 93,91,91), die Umgebung hat L\* 9,1. Das ist ein heller, grauweißer Strich auf dem rotbraunen Buckel. K5e_f1 hat ihn nicht. Bild: `belege/weisslinie_zoom.png` (K5e_f1 | K5g3 | Marker).

Die Riegel V3.1 und V3.1b messen mit derselben fillPoly-Maske und finden ihn darum nicht.

**Behebung (Vorschlag):** Die Kopie über die beiden Render-Dreiecke (0,1,2) und (1,3,2) maskieren, oder das ganze Quell-Rechteck kopieren. Danach Index-0-Zugriffe im Asset = 0 verlangen; Gegenprobe ist der heutige Stand.

## 4 V4 Hals, Lippe, Kiefer, Mund: GRÜN, mit Hinweis

Albedo der gelesenen Texel (farbe.py):

| Bereich | K5g3 | K5e_f1 |
|---|---|---|
| Zone 45 | L\* 26,9, a\* 11,1, C\* 12,0, keine 0x8000 | L\* 37,4, a\* 46,7, C\* 49,9, 11× 0x8000 |
| Zone 42 Lippe | L\* 23,3, a\* 11,1 | |
| Zone 31 Unterkiefer | L\* 23,3, a\* 11,1; a\*-P95 17,6 (Haut 23,9) | |
| Zone 44 und Hals 21 | Median = Haut, Anteil L\*<5 0,18-0,37 % | |

0x8000 kommt im ganzen Asset 7× vor, nur in Zone 30 (Maul innen; dort erlaubt). 0x0000 kommt nicht vor.

Im Spiel, eigener gdigrab-Lauf (griff_5090, Clip 27 Bild 20, Hals-Unterseite): Die roten Streifen von K5e_f1 sind weg. Bild: `belege/griff_hals_zoom.png` (K5e_f1 | K5g3 | Fleckpixel grün).

**Hinweis (gelb):** 5 Flächen der Zone 44 (Hals-Unterseite) sind Trägerflächen der Zone 27 und tragen dort Fleisch, 969 Texel-Zugriffe. Riegel V4.1 nimmt Fleck-Material ausdrücklich aus.

Im Griff-Bild ist der Fleck ein dunkelbrauner Bereich von 1481 px (a\* 7,1 gegen 3,2 daneben, L\* 10,1). Das ist kein roter Streifen. Es ist aber Fleisch auf der Hals-Unterseite, die V4 „nie rot“ nennt; das ist Zone 27 aus der abgenommenen Grundlage. Der Orchestrator sollte bestätigen, dass Zone 27 dort Vorrang hat.

## 5 V5 Krallen, O5 Hände, S1, V6 F1

| Punkt | K5g3 | Bewertung |
|---|---|---|
| V5 Krallen links 14/15/28/30 und rechts 19/20/29/31 | L\* 28,0, a\* 14,3, C\* 15,4; Hand L\* 26,9, a\* 11,1, C\* 12,0. Abstand etwa 3,5 dE. Kein 0x7FFF, kein 0x8000 | grün. Die Krallen sind um a\* +3 wärmer als die Hand, kein Rot |
| O5 Hände 13 und 18 | L\* 26,9 (Haut P25-P75 im Riegel 14,7-31,6), 0× 0x7FFF; nur Zeile 8 (tausch1) | grün |
| S1 Hüfte/Rücken 0/7 | L\* 23,3 = Haut-Median, kein dunkler Sattel | grün; zu V1 siehe Abschnitt 7 |
| V6 F1 (6 Flächen) | 25,8 % der Zugriffe byte-gleich K5c (Kern); Rest (Zone-2/3-Material und Rand) L\* 14,7 Median. Rand-Texel direkt am Kern: 11 von 105 mit L\* < 8; sonst Haut 4,25 % | grün. Kein schwarzer Ring. K5c hatte 45,8 % mit L\* < 8 |

## 6 V1, V2 und O3 im Spiel: GRÜN, Z9 knapp

Eigene gdigrab-Läufe (lauf.sh: eigene exe-Kopie, Fenster nur über die eigene PID, WINDOW_SCALE 2, 640×480, kein AUTOSHOT, kein SOFTWARE_RENDER). Je Variante gibt es ein Paar aus Normal- und Schlüssel-Lauf. Den Schlüssel-TIM erzeugt mkkey.py: CLUT ≠ 0 → 0x7FFF.

Messung mit eigenem Code (mass.py). Szene: 5080 Bild-1-Lage, Pose -20711,-15984,756,0,11, FORCE_CUT 1.

| | K5g3 | K5e_f1 | Band der Originale |
|---|---|---|---|
| Z1 P L\* | 7,80 | 9,45 | 5,57-18,80 grün |
| Z2 P L-cv | 0,708 | 0,409 | 0,481-1,431 grün |
| Z3 P lokal_rel | 0,361 | 0,168 | 0,243-0,656 grün |
| Z4 P C\* | 4,91 | 4,33 | 4,41-7,59 grün |
| Z5 P C-std | 3,19 | 1,76 | 2,39-7,45 grün |
| Z6 Q L-cv | 0,516 (8-Bit-Q: 0,520) | 0,237 | 0,369-1,020 grün |
| Z7 Q lokal_rel | 0,287 | 0,115 | 0,210-0,528 grün |
| Z8 Q C\* | 13,97 (8-Bit-Q: 13,93) | 11,51 | 12,09-15,05 grün |
| Z9 Q C-std | **6,484 (float-Q) / 6,541 (8-Bit-Q wie spiel_fam2.py)** | 3,96 | 6,53-8,88: **knapp** |

**Z9 (gelb):** Nach der Messblatt-Lesart (Q auf 8 Bit gerundet) liegt der Wert 0,011 über der Untergrenze, ohne Rundung 0,046 darunter. Der Bauer meldet 6,54. Das Band ist erreicht, aber ohne Reserve.

**Farbton (V2):**

| Lesart | K5g3 | K5e_f1 | K5f |
|---|---|---|---|
| Spiel, P | +19,8° | -26,8° | -21,7° |
| Spiel, Q | +21,1° | -29,8° | -24,1° |
| Albedo der Hautflächen | +22,4° | -34,4° | -27,0° |

K5c liegt in der Albedo bei -8,1°. Wärmer als K5e_f1 und K5f, im Band 14-27°: grün.

**Ruhe:** Gemessen mit der Referenz-Umsetzung ruhe_mass.masse auf meinem Q-Bild, nur zum Abgleich. Ein weißes Licht gibt es im Spiel nicht, darum steht hier Q als Albedo-Lesart.

| | K5g3 | K5e_f1 | Grenze |
|---|---|---|---|
| S5 | 1 | 3 | ≤ 5 |
| F_fremd | 0,6 / 0,09 % | | ≤ 7,4 / 0,64 % |
| Spanne90 | 1,3° | 26,5° | ≤ 76,8 |
| Block | 4,8° | 8,4° | ≤ 16,3 |

Grün. Zweiseitig: nicht flacher (Z2/Z3/Z6/Z7 im Band, K5e_f1 fällt darunter) und nicht bunter (Z4/Z8 im Band).

5090 ist nur zur Info. Das Feuer liegt in der Maske; Bild 2 hat P C\* 23,6 gegen 10,8 bei K5e_f1.

## 7 Gelbe Punkte (nicht rot, Entscheid oder Sicht-Prüfer)

1. **V1 „je Material EINE Transformation“:** Die Haut hat drei multiplikative Faktoren, je G4-Seite einen: f_s = 4,54 / 1,86 / 2,55. Selbst nachgerechnet: L' = -0,68 + 4,646·L, -0,44 + 1,871·L und 1,30 + 2,476·L; Rangfolge Spearman ≥ 0,998. Begründet ist das mit O3 (kein Sattel). Es weicht aber vom Wortlaut von V1 ab, und der Hüft-/Rückenbereich ist gegenüber dem Rumpf 2,44-fach aufgehellt. Der Orchestrator sollte das entscheiden.
2. **Bräunliche Musterung, Zonen 16 und 19:** L\* 72,6 / 67,2 bei C\* 21,7 / 13,7 und Farbton 72-74°, also etwa dreimal so hell wie die Haut (L\* 23,3). Das ist eher hell-beige als braun. Der Grund ist der Faktor 4,54 bzw. 2,55 auf hellen G4-Einträgen. Bitte am Bild beurteilen.
3. **Augen (O4):** Im Spiel (Bild-2-Pose 5090, Marker auf den Augen-Einträgen) ist nur **ein** Auge sichtbar, 13 px. Es hat L\* max 11,2 und Median 9,7, der Ring daneben (Augenhöhle) 5,1. K5e_f1 hat dort max 8,2. Das Auge ist sichtbar heller, aber schwach. Riegel A1 (Bühnen-Render) ist grün. Bild: `belege/augen_zoom.png`.
4. **Reinweiß auf der Haut:** 76 Zugriffe auf 0x7FFF auf Hüfte/Rücken (Zeile 4, Index 221/231, Knie nach Faktor 4,54) und 1 auf Netz 25. Im Spiel, Bild-1-Lage, haben 104 Pixel Q-L\* > 90 (0,29 %); K5e_f1 hat 0. V5/O5 betreffen nur Hände und Krallen, dort ist es 0. W4/Ruhe sind grün.
5. **Grundlage gegen B4:** 1607 Fleisch-Paare der Zone 27 liegen auf 14 Mutterflächen, die B4 entfernt hat (Kopfkugel und Netz 10). 43 Zugriffe von Kindern der Fläche 10\|1\|76 lesen Zone-27-Material knapp außerhalb der Mutter-UV; das ist dieselbe Zone. 3283 von 10257 Kindern lesen Texel außerhalb der UV-Fläche ihrer Mutter (B4, baryzentrisch gerundet). Das ist nur bei kopierten Müttern relevant und führt dort zu ROT 2.
6. **Geometrie-Riegel R8 rot** (Abschnitt 1): wegen der K5c-Krallen, Posen-Entscheid offen.

## 8 Technik: GRÜN

- **Generator zweimal selbst gelaufen:** `python generator/kf_k5g3.py bauen <pz3/gen1>` und `<pz3/gen2>`, 80 s. Alle Dateien sind gleich untereinander **und** gleich der Abgabe: BAHNSTEIG_FORM.EMD 37407afd…, .TIM 2f7b2444…, FORM.EMD 62fa1e67…, FORM.TIM, form.json ba2147b3…, bau.json b82f4fa7…, pruef/* (ce6766a0…, 6ee4ca46…).
- Die Import-Kette des Generators ist vollständig versioniert: kf_k5g3, _tex, _geo, kf_k5g2_tex, _geo, kf_k5e, kf_g4, die B4-Tabelle, die Krallen-Tabelle, traeger_texel.json und k5g3_par.json.
- **T1:** 15 CLUT-Zeilen, Port-Höhe 15 × 256 = 3840 ≤ 4096. **T5:** Alle 13173 Flächen lesen die Zeilen 0-14. Die höchste Spalte u + 128 × Seite ist 510, also unter 512.
- **Commits:** auf r41/form, Deutsch, je mit „Co-Authored-By: Claude Opus 5.5“, keine Backticks. Kein Push.

## 9 Was der Bauer tun muss

1. Weiße Musterung (Zonen 14, 15, 18, 24) fast unbunt machen: eigene, kleine Buntheit für „weiss“ statt s der Haut. Riegel: C\* der Weiß-Texel deutlich unter der Haut (z. B. ≤ 0,5 × Haut-C\*), L\* nicht unter den Originalen. Gegenprobe ist der heutige Stand.
2. Kopie-Maske = Render-Dreiecke (0,1,2) + (1,3,2), oder ganzes Rechteck kopieren. Riegel: 0 Zugriffe auf ungeschriebene Kopie-Texel (Index 0) über alle Asset-Flächen, abgetastet im Inneren der Dreiecke. Gegenprobe ist der heutige Stand (26). Danach das Spielbild Gier 3200 wiederholen.
3. Optional: Z9 mit etwas Reserve, die braunen Zonen 16 und 19 und Zone 44 gegen Zone 27 vom Orchestrator bestätigen lassen.
