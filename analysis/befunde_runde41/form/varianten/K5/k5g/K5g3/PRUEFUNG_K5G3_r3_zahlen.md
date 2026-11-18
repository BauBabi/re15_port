# Prüfung K5g3, Runde 3: Zahlen-Prüfer

Stand 2026-11-14. Geprüft: Commits 6427e517a und 968001097 auf r41/form. Asset BAHNSTEIG_FORM.EMD 1981f97e…, BAHNSTEIG_FORM.TIM = FORM.TIM c00823b3…, FORM.EMD 26e310fe… (sha256 nachgerechnet, gleich den Angaben des Bauers).

Alles mit eigenem Code gemessen. Gegenprobe ist die Abgabe der Runde 2 aus git (f48d37df1: EMD 482bd63f…, TIM ce9a1dc9…), ausgepackt nach
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/pz5/r2/.
Skripte, Läufe und Belege: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/pz5/
(Skripte r2r3.py, r2r3b.py, zstat.py, z27.py, braun_g6.py, g8.py, g9.py, v4.py, fehlt.py, timdiff.py, emddiff.py, zband.py, braun5.py, hellspiel.py, serie5.sh, lauf5.sh; dazu die Prüfer-Skripte der Runde 2 neu gelaufen: geo_check.py, v3a-v3d.py, struktur5.py, kralle.py, hell.py, f1.py, loch.py, kind.py, auge1.py, auge2.py, weiss.py, farbe.py, hue.py, mass.py, mass2.py, ruhe_eigen.py). Die Riegel des Bauers habe ich nicht benutzt.

## Ergebnis: BESTANDEN (kein roter Punkt), G9 offen beim Orchestrator

- G6 Braun: grün. Struktur der G4-Texel erhalten, im Spiel nachgemessen.
- G7 Hals-Unterseite: grün. Zone 44 liest kein Fleck-Material mehr; im Spielbild keine roten Streifen.
- G8 Deckel L\* 85: grün. 0 von 1 174 981 Proben über 85 (Runde 2: 4867).
- G9 Seitenfaktoren: Bedingung **nicht** erfüllt. Der Bauer hat das, wie G9 verlangt, gemeldet und nicht gebaut. Ich habe es selbst nachgerechnet (Abschnitt 4). Entscheidung beim Orchestrator; nach dem Wortlaut von G9 („melden statt bauen“) kein Rot für diese Runde.
- G10 Rest unverändert: ja, mit einer nicht gemeldeten kleinen Nebenwirkung von G7 auf Zone 27 (gelb, Abschnitt 5).
- Geometrie G1/G2/G5, G3, V3, V4, V5, V6, Zielband, Ruhe, Hautton, Generator byte-gleich, ctest: grün.

## 1 Geometrie (G1, G2, G5): GRÜN

geo_check.py:
- Sektionen 0-6 (Morph, EDD/EMR, Posen) byte-gleich K5e_f1, K5c und KAND. Die Posen sind die von K5c.
- Netze 10, 16, 21, 22 gleich KAND.EMD (B4, f78e5983…, TABELLE 99a4f142… nachgerechnet).
- Netze 19, 20, 28, 29: Ecken/Normalen gleich K5c (1ab56076…), Flächen-Ecken gleich K5e_f1. Netz 31 gleich (K5c = K5e_f1).
- Übrige Netze gleich K5e_f1. Alle 32 Netze gleich der Geometrie-Stufe geo/ (db9404a1…). Abweichungen: keine.
- Gegen Runde 2 (emddiff.py): 0 Geometrie-Abweichungen. Nur Sektion 7 (MD1) unterscheidet sich, und zwar nur in den Textur-Bytes von 1472 Flächen.
  - Das sind mehr als „5 Flächen“. Ursache: Die 145 Kopien (statt 150) liegen teils an anderen freien Plätzen, also neue UV/Seite/CLUT-Zeile.
  - v3b.py: alle 145 Kopien sind reine Verschiebungen. 0 unverschobene Flächen lesen geänderte Texel. 0 fremde Flächen lesen Ziel-Texel einer Kopie. 0 Kopien-Paare überlappen.
  - Wirkung im Bild ist allein über die gelesenen Farben gemessen (Abschnitt 5).
- Spiel-Asset re15_port/shared_assets/RE2: EMD f277916d…, TIM 31a35e66… = K5e_f1 (G5).
- Geänderte Pfade f48d37df1..HEAD: 41 Dateien, alle unter analysis/…/K5/. Nichts in Version_80%/, pri/, synchro/, shared_assets/ oder re15_port/.

## 2 Dreiecks-Liste (G3): GRÜN

- **Code:** render_pc.c:226 setzt TEXTRI_QUEUE_MAX auf 16384.
  - s_textri_* (462-465), s_sb_* (493-496), s_textri_flush_buf (509) und order[] (1209) hängen am Makro.
  - Zähler und Abfrage: 470, 569/570, 1378-1387. Die exe enthält die Zeilen „[tri-liste] ENDE …“ und „UEBERLAUF“.
  - local_build.sh build (RE15_BUILD_DIR = Worktree): „ninja: no work to do“, „LOCAL-BUILD-OK (build)“.
  - Speicher wie Runde 1/2: 196 B je Eintrag × 16384 = 3,06 MiB, dazu 64 KiB Stapel für order[].
- **Eigene Spielläufe** mit Asset Runde 3 (eigene exe-Kopie, RE15_RE2_ASSET_ROOT, ohne SOFTWARE_RENDER):

| Lauf | Schüsse (mg) | hoechststand | verworfen |
|---|---|---|---|
| kampf_5090 (freier Kampf, Form HP -1) | 144 → 57 = 87 | 7415 (F522) | **0** |
| s11_5090 (Schnitt 11, Form Clip 12) | 144 → 100 = 44 | 7718 (F829) | **0** |
| v5080s (5080 vorn, Clip 12) | 144 → 100 = 44 | 7184 (F941) | **0** |
| 10 Bild-Läufe 5080/5090 | – | höchstens 7318 | **0** |

  „UEBERLAUF“ kommt in keinem debug.log vor, kein Absturz.

## 3 Die Entscheide G6, G7, G8

### G6 Braune Musterung (Zonen 16/19): GRÜN

braun_g6.py: je Zonen-Texel die G4-Farbe aus der EMS-CLUT (g4clut.npy) am K5e_f1-Index, dazu die gelesene K5g3-Farbe.

| Zone | Paare | G4 cv / Stufen | K5g3 cv / Stufen | Spearman | Fit L\*(K5g3) | Ton | C\*/L\* |
|---|---|---|---|---|---|---|---|
| 16 | 94 | 0,424 / 23 | **0,414 / 14** | 0,996 | 0,79 + 1,875·L_G4 | 71,1° | 0,433 |
| 19 | 16 | 0,275 / 7 | **0,262 / 5** | 0,997 | 1,51 + 1,821·L_G4 | 70,3° | 0,486 |

- **Multiplikativ:** L_K5g3/L_G4 liegt im Median bei 1,91 bzw. 1,90 (P10-P90 1,87-2,01). Das ist b·f_s = 0,7576 × 2,549 = 1,93. Der Achsenabschnitt ist klein, also EINE Transformation.
- **Riegel** (cv ≥ 0,5·cv(G4), ab 15 Texeln ≥ 5 Stufen): 16 grün (0,414 ≥ 0,212; 14 ≥ 5), 19 grün (0,262 ≥ 0,138; 5 Stufen, genau an der Grenze).
- **Gegenprobe Runde 2** (z27.py): Zone 16 und 19 cv je 0,059 mit 2 Stufen, ROT.
- **Exklusiv:** Die Braun-Einträge (Zeile 13, 23 Einträge) liest keine fremde Fläche (v3c: Material außerhalb nur die bekannten Loch-Flächen in Zeile 12).
- **Im Spiel** (5080 Bild-1-Lage, Marker-Lauf mit Braun-Einträgen in Grün, 103 Pixel; braun5.py):

| | Q cv | Q L\* Median | Q Ton | Q Farben | P cv |
|---|---|---|---|---|---|
| Runde 3 | **0,412** | 25,3 | 69,8 | 50 | 0,507 |
| Runde 2 | 0,062 | 32,1 | 73,3 | 37 | 0,149 |
| K5e_f1 | 0,234 | 33,5 | -34,7 | 48 | 0,333 |

  Beleg: pz5/belege/braun_z5080.png. Der Fleck zeigt Maserung statt einer glatten Fläche.

### G7 Hals-Unterseite (Zone 44): GRÜN

v4.py und farbe.py, Abtastung aller Flächen inkl. Spalt-Kinder (Mutter über geo/spalt_uv_tabelle.json):

| Zone | Runde 2 | Runde 3 |
|---|---|---|
| 44 | Zeilen {12: 26 364, 5: 67 626} | nur Zeile 5 (Haut), **0** Fleck-Material-Proben; L\* 23,2, a\* 11,3, C\* 12,2 = Haut |
| 45, 42, 31 | – | nur Hautzeilen 7/8, 0 Fleck-Material |

- Das Material fehlt genau auf den 5 Flächen 21|0|0, 21|1|0, 21|1|8, 21|1|56, 21|1|64: 969 Texel-Flächen-Paare (fehlt.py; 39+106+72+186+566).
- Schwarz: 0 Proben mit 0x0000/0x8000 außer 6 Proben in Zone 30 (Maul innen, erlaubt).
- Zeilen 0/1 (Mundfleisch) lesen nur Kinder der 27 Flächen von Zone 30. Zeile 2 lesen nur Flächen der Zone 41.
- **Im Spiel** (griff_5090, Clip 27 Bild 20, gdigrab): Bei K5e_f1 ist die Hals-Unterseite rot gestreift. Bei Runde 3 ist sie dunkelgrau ohne rote Streifen; rot ist nur der Maulspalt (Zone 30). Beleg: pz5/belege/griff_hals_zoom.png.

### G8 Fast weiße Haut: GRÜN

g8.py, alle 13 173 Flächen, 1 174 981 Proben. Ausgenommen sind Zeilen 0/1/2/14 und die F1-Kern-Worte K5c.

| | Proben mit L\* > 85 |
|---|---|
| Runde 3 | **0** |
| Gegenprobe Runde 2 | 4867 (0x739F 2604, 0x7BDF 553, 0x77BF 451 …), ROT |

- Kein 0x7FFF außerhalb der Ausnahmen. weiss.py: alle Haut-, Tausch- und Fleckzeilen haben L\*max 85.
- Ersatzwort ist 0x631F (L\* 84,5) bzw. 0x631E.
- **Im Spiel**, Pixel mit Q L\* > 90:

| Ansicht | Runde 3 | Runde 2 | K5e_f1 |
|---|---|---|---|
| z5080 | **0** | 107 | 0 |
| v5080 | 38 | 101 | 36 |

  Mit Q L\* > 85 sind es in z5080 13 Pixel (Runde 2: 138).

## 4 G9 Seitenfaktoren: Bedingung NICHT erfüllt, gemeldet, nicht gebaut

g9.py rechnet auf dem G4-Atlas (Seiten 0-2 von K5e_f1) mit der G4-CLUT aus der EMS.

| Statistik | Seite 0 | Seite 1 | Seite 2 | g_s (Seite 1 / Seite s) |
|---|---|---|---|---|
| Grundhelligkeit der Seiten: Median L\* aller nicht transparenten Texel (je 32 768) | 8,53 | 12,19 | 12,19 | 1,43 / 1 / 1,00 |
| Gewichtet mit den K5-Hautflächen (Zeilen 4/5/6) | 4,91 | 12,19 | 9,16 | 2,48 / 1 / 1,33 |

- **Abgabe:** g_s = 2,44 / 1 / 1,37, also f_s = 4,54 / 1,86 / 2,55 (bau.json). Das entspricht der gewichteten Zeile, nicht der Grundhelligkeit der Seiten.
- **Bauer** (FARBE.md K5g3R3.2): Mit G4-eigener Gewichtung kommt er auf 1,33 / 1 / 1,05. Meine ungewichtete Seiten-Grundhelligkeit liegt dazwischen. Beide Lesarten zeigen dasselbe: Die Grundhelligkeit erklärt nur einen Teil der Faktoren auf Seite 0 und 2.
- **Woher der Rest kommt:** Die K5c-UV legt Hüfte und Rücken auf dunkle Stellen der Seite 0. Die Faktoren sind aus Original-Texeln abgeleitet (kein Video, keine Zielwerte je Region), wirken aber wie ein Ausgleich für Hüfte und Rücken.
- **Bewertung:** Die Bedingung aus G9 ist nicht erfüllt, der Bauer hat richtig gemeldet. Ohne die Faktoren käme der Sattel zurück; das hat der Bauer nachgerechnet (Hüfte/Rücken L\* 12,7 unter Haut-P25 16,5), ich nicht selbst. **Entscheid beim Orchestrator.** Nach G9 ist das kein roter Punkt dieser Runde.

## 5 G10: Rest gegenüber Runde 2

### CLUT (timdiff.py, Klassifizierung je Eintrag)

| Zeilen | geänderte Einträge | Bewertung |
|---|---|---|
| 0, 2, 14 | 0 (gleich) | – |
| 1, 3-10, 12 | nur Deckel (r2 L\* > 85 → r3 ≤ 85) | G8 |
| 13 | 138 Deckel + 34 andere (gelesen 154-mal) | G6 Braun |
| 11 | 12 Deckel + **84 andere (gelesen 1093-mal)** | gelb, siehe unten |

### Pixel

38 806 Bytes anders auf den Seiten 0, 1 und 3. Das sind die verlegten Kopien (v3b, siehe Abschnitt 1).

### Gelesene Farben je Fläche (r2r3.py/r2r3b.py, 1 189 422 Proben)

1044 Flächen lesen eine andere Farbe:

| Ursache | Proben | Flächen |
|---|---|---|
| G7 | 7820 | 174 |
| G8-Deckel | 4854 | 424 |
| G6 | 99 | 6 |
| Rest | 4046 | 451 |

Der Rest liegt fast nur auf Trägern der Zone 27 (Buckel-Fleisch), dazu Einzelproben in den Zonen 3, 9, 11, 14/15, 24, 29.

**Gelb 1, nicht gemeldete Nebenwirkung von G7 auf Zone 27:**
- Der Rang-Tausch der Fleisch-Rampe rechnet mit weniger Texeln (8571 statt 9540). Darum bekommen 19 von 112 G4-Quellen ein anderes Wort, meist ΔL\* ≤ 5,3 (z27-Abfrage). Betroffen sind 4,8 % der Proben auf Zone-27-Flächen.
- Die Zonenwerte bleiben gleich (z27.py, Material-Texel):

| Zone 27 | Runde 2 | Runde 3 |
|---|---|---|
| L\* Median | 35,9 | 35,9 |
| P10 / P90 | 18,3 / 60,9 | 18,3 / 60,9 |
| C\* | 24,8 | 24,8 |
| Ton | 24° | 24° |
| cv | 0,439 | 0,434 |
| Stufen | 38 | 35 |
| Spearman zur G4-Struktur | 0,990 | 0,990 |

  Damit liegt der Buckel weiter im Eichungsband L\* 33-39, C\* 22-26. Im Dossier steht das nicht („Zeilen wie Runde 2“).

**Gelb 2, Deckel trifft Fleck-Material:**
- In 12 Texel-Flächen-Paaren der Zonen 9, 11 und 3 sind Material-Eintrag und Grund-Eintrag beide auf 0x631F gedeckelt. Sie sind dadurch nicht mehr unterscheidbar (fehlt.py: Zeile 11 Index 144-151, Zeile 13 Index 65).
- Die Farbe wäre ohnehin das Deckelwort. Das ist kein sichtbarer Fehler, aber „fehlende Fleck-Texel 0“ (V3.1 des Bauers) gilt nur in seiner Zählweise.

## 6 V3 und E3 (Flecken, Augen): GRÜN

v3a-v3d neu gerechnet, gegen K5g2/traeger_texel.json je Fläche:

| | Anzahl | Erklärung |
|---|---|---|
| erwartet | 11 417 | – |
| Material, richtig | 10 220 | – |
| zu viel | 0 | ohne die 123 Loch-Haut-Zugriffe der 8 Flächen 22\|0\|28-35; deren Zeile-12-Einträge 70-103 liest kein echter Träger, Farbe = Haut |
| fehlt | 1197 | davon 969 G7 (so verlangt); 125 Zone 2 + 8 Zone 3 F1-Kern-Vorrang/gleiche Worte; 61 Zone 27 + 1 Zone 29 außerhalb der UV-Fläche (wie Runde 2); 24 Zone 29 über Augen-Dekal; 12 Deckel-Gleichheit (Gelb 2) |

- **Spalt-Kinder** (kind.py): 15 660 im Fleck der Mutter, 69 Zone-27-Texel knapp außerhalb der Mutter-UV, 0 fremd.
- **Index 0** (strict/loch): 0 Zugriffe.
- **Augen:**
  - Zeile 14 lesen nur die 11 Augen-Dreiecke.
  - Helle Proben innerhalb des Radius: 92,5 % / 95,2 %. Dunkle Proben innerhalb des Radius: 1,5 % / 0,6 %.
  - Mütter 22|0|82 und 22|1|112, also die Flächen unter den Nutzerpunkten (auge1/auge2).
  - Unverändert gegen Runde 2.
- **Zone 45:** nur Zeile 7, L\* 26,9, a\* 11,1, C\* 12,0, kein 0x8000. K5e_f1 hatte dort a\* 46,7.

## 7 V5, O5, V6: GRÜN

| Punkt | Messung (kralle.py, f1.py, farbe.py) |
|---|---|
| Krallen links / rechts | L\* 27,7, a\* 14,3, C\* 15,5, max L\* 84,5; 0× 0x7FFF, 0× 0x8000 |
| Hände 13 / 18 | L\* 26,9, nur Zeile 8, max 84,5 |
| F1 | 25,8 % der Zugriffe byte-gleich K5c-Kern; von 105 Rand-Texeln 11 mit L\* < 8; kein schwarzer Ring |

- **Gelb 3:** 2,3-2,5 % der Krallen-Proben lesen jetzt das Deckelwort 0x631F (rosa-weiß, L\* 84,5, C\* 22,3). Vorher waren es fast unbunte Wörter mit L\* 86-99 und C\* 3-9.
  - Auf der ganzen Haut sind 0,4 % der Proben betroffen.
  - Zone 45 mit a\* > 20 steigt von 3,5 % auf 6,1 %, weil 0x631F a\* 20,8 hat.
  - Hell bleibt hell, ist aber rosa statt grau-weiß. Ob das im Spiel als rosa Sprenkel auffällt, beurteilt der Sicht-Prüfer.

## 8 V1, V2, O3 im Spiel: GRÜN (Z9 knapp wie Runde 2)

Eigene gdigrab-Läufe (serie5.sh):
- eigene exe-Kopie, Fenster-HWND über die eigene PID, nur eigene PIDs beendet
- 640×480, kein AUTOSHOT, kein SOFTWARE_RENDER
- Normal- und Schlüssel-Lauf (alle CLUT-Einträge weiß = weißes Licht/Albedo-Bezug)

K5e_f1 und K5f stammen aus den Läufen der Runde 2 mit derselben exe und Szene. Szene z5080 (Bild-1-Lage), die drei Bilder sind identisch. zband.py/mass2.py:

| | Runde 3 | Runde 2 | K5e_f1 | Band |
|---|---|---|---|---|
| Z1 P L\* | 7,77 | 7,79 | 9,45 | 5,57-18,80 |
| Z2 P L-cv | 0,703 | 0,708 | 0,409 ROT | 0,481-1,431 |
| Z3 P lokal | 0,361 | 0,363 | 0,168 ROT | 0,243-0,656 |
| Z4 P C\* | 4,86 | 4,84 | 4,33 ROT | 4,41-7,59 |
| Z5 P C-std | 3,20 | 3,19 | 1,76 ROT | 2,39-7,45 |
| Z6 Q L-cv | 0,514 | 0,517 | 0,238 ROT | 0,369-1,020 |
| Z7 Q lokal | 0,286 | 0,288 | 0,115 ROT | 0,210-0,528 |
| Z8 Q C\* | 13,79 | 13,73 | 11,51 ROT | 12,09-15,05 |
| Z9 Q C-std | **6,578 (8-Bit-Q) / 6,524 (Float)** | 6,571 / 6,518 | 3,98 ROT | 6,53-8,88 |

- **Gelb 4, Z9:** Nach der Lesart des Messblatts (8-Bit-Q) liegt der Wert 0,05 über der Grenze, ohne Rundung 0,006 darunter. Das ist unverändert gegen Runde 2.
- **Ansicht v5080 (Info):** alles im Band außer Z8 16,35. Das war in Runde 2 genauso (16,43). Das Band ist für z5080 definiert.
- **5090-Szenen:** Feuer flackert, Normal- und Schlüsselbild weichen in 52 000 Pixeln voneinander ab. Für Q-Maße sind sie nicht brauchbar und nur für Bildausschnitte benutzt.
- **Hautton (V2):**

| Lesart | Runde 3 | K5e_f1 | K5f |
|---|---|---|---|
| P | +19,7° | -26,9° | |
| Q | +20,9° | -29,8° | |
| Albedo (hue.py) | +22,4° | -34,4° | -27,0° |

  Im Band 14-27°, wärmer als K5e_f1 und K5f.
- **Ruhe, zweiseitig** (ruhe_eigen.py, Q):

| | Runde 3 | K5e_f1 |
|---|---|---|
| S5 | 1 | 3 |
| F_fremd | 1,6 / 0,18 % | 0 |
| Spanne90 | 1,3° | 26,5° |
| Block | 5,0° | 8,4° |

  Grün. „Nicht flacher“ ist über Z2/Z3/Z5-Z9 belegt.
- **V1, multiplikativ:** Die Hautzeilen sind gegen Runde 2 nur im Deckel geändert, die Fits der Runde 2 gelten weiter (Achsenabschnitte -0,9 / -0,08 / 1,5). Braun ist multiplikativ (Abschnitt 3). Zu den Seitenfaktoren siehe G9.

## 9 Technik: GRÜN

- **Generator zweimal selbst gelaufen** (python generator/kf_k5g3.py bauen …/pz5/gen1 und gen2):
  - Alle 11 Dateien sind untereinander gleich und gleich der Abgabe: EMD 1981f97e…, TIM c00823b3…, FORM.EMD 26e310fe…, bau.json 2013c977…, form.json ba2147b3…, pruef/* (5b424955…, c220e46b…).
  - git status im Worktree vor und nach dem Lauf gleich.
- **ctest selbst** (PowerShell, ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3, RE15_RE2_ASSET_ROOT = k5g/K5g3): „100% tests passed, 0 tests failed out of 12“ in 360,97 s. Darin integration_r41_form_dreiecke Passed 360,90 s und integration_r41_form_wagenweg Passed 126,51 s. Protokoll pz5/ctest.log.
- **T1:** 15 CLUT-Zeilen, Port-Höhe 3840.
- **Commits:** auf r41/form, Deutsch, keine Backticks, letzte Zeile Co-Authored-By wie verlangt, kein Push.
- **R8** (Geometrie) bleibt rot wie verabredet (G11). Nicht geprüft und nicht angefasst.

## 10 Offen / Gelb (kein Rot)

1. **G9:** Die Seitenfaktoren sind nicht allein aus der Grundhelligkeit der Seiten abgeleitet: 1,43/1,00 (Seiten) bzw. 1,33/1,05 (G4-gewichtet) gegen 2,44/1,37. Entscheid beim Orchestrator.
2. **Zone 27:** Nebenwirkung von G7, 19 von 112 G4-Quellen haben ein anderes Wort, die Zonenwerte sind gleich. Im Dossier nicht gemeldet.
3. **Deckel-Kollision:** 12 Fleck-Texel der Zonen 3, 9, 11 sind nicht mehr von der Haut unterscheidbar (beide 0x631F).
4. **Deckelwort 0x631F ist rosa-weiß** (C\* 22,3 statt C\* 3-9 der früheren hellsten Wörter). Betroffen: 0,4 % der Proben, Krallen 2,4 %. Das beurteilt der Sicht-Prüfer.
5. **Z9 knapp** (6,578 bei Grenze 6,53; Float 6,524).
6. **G6 Zone 19:** genau 5 Stufen bei einer Grenze von 5.
