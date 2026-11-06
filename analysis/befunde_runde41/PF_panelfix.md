# Runde 41 — Spur PF "panelfix" (Dossier)

Baum `.claude/worktrees/r41_panelfix`, Zweig `r41/panelfix`, Basis master 789f6e681. Vorarbeit: Spur N, Dossier `N_nachtrag30.md` §1/§2.

Wortlaut Nutzer: "Der Kasten mit der Sicherung in ROOM 2010 sieht schon klasse aus, aber das Bild ist / der Kasten ist bei der Kamerasicht irgendwie komisch
gestaucht.... das ist noch falsch. Auch das Raetsel in 2030 das du gebaut hast ist Klasse! Aber Zahlen und Texte auf den Panel in Nahansicht sind kaum
lesbar/verschwommen. Und der Text am Anfang vor dem Start des Raetsels ueberlappt mit dem yes / no Auswahl."

Messweg: echtes Fenster, beschleunigter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER, ffmpeg gdigrab per hwnd (Skill re15-port-visual-verify), eigene
exe-Kopien `re15_pc_pf_*` (vorher = Bau der Basis als `re15_pc_pfvorher.exe`), SDL_ASSERT=always_ignore. Treiber und Messwerkzeuge im scratchpad `pf/`
(`lauf.py`, `schrank_mass.py`, `schaerfe.py`, `fragen.py`, nicht committet). Bilder: `PF_belege/`.

## 1. Sicherungskasten 2010: "komisch gestaucht"

**Messung vorher (Leon ROOM2010, Elza ROOM2011; Tuer auf, Texte weg, Figur nach links aus dem Bild; Cut 3):** Rueckwand und Fluegel aus dem Fensterbild
ueber die Kamera Cut 3 @0xC0 (H = 26684 >> 7 = 208) auf gleichmaessiges Weltmass entzerrt und gegen ROOM2060 Cut 6 (BG06 = fuse_box.bmp) gesucht, welcher
Massstab (sx, sy) die Kanten am besten deckt (NCC). sx/sy = 1 heisst unverzerrt.

| Fenster | Rueckwand sx/sy (NCC) | Fluegel sx/sy (NCC) |
|---|---|---|
| RE15_WINDOW_SCALE=1, Leon | 0,795 (0,30) | 0,740 (0,64) |
| RE15_WINDOW_SCALE=3, Leon | 0,795 (0,30) | 0,740 (0,64) |
| RE15_FULLSCREEN 1920x1080, Leon | 0,795 (0,29) | 0,740 (0,64) |
| RE15_WINDOW_SCALE=1, Elza | 0,795 (0,30) | 0,740 (0,64) |

Die entzerrte Rueckwand zeigt einen Knick entlang der Diagonale (Rahmen der Sicherungen schief, Roehren gegeneinander versetzt):
`PF_belege/schrank_innen_entzerrt_vorher_nachher.png` (links Port vorher | 2060).

**Nicht die Ursache (gemessen):** Fenstergroesse, 4:3 gegen 16:9, Briefkasten. `SDL_RenderSetLogicalSize(320, 240)` (render_pc.c) skaliert in beiden
Achsen gleich: im Vollbild 1920x1080 ist der Bildinhalt 1440 x 1080 = 1,3333, und die Werte oben sind in Fenster 1, Fenster 3 und Vollbild gleich.

**Ursache (zwei Teile, beide gerechnet und gemessen):**
1. Seitenverhaeltnis. `tools/r41_nachtrag30/schrank_bild.py` zog das Viereck Q_INNEN bzw. Q_TUER aus ROOM2060 Cut 6 stumpf auf die Tuerflaeche
   1300 x 3600 (B/H 0,361). In der Welt hat die Rueckwand in 2060 B/H 0,460 (Homographie Q -> Rechteck, Lochkamera H 208) bzw. 0,453 (Gegenrechnung:
   Rueckwand-Ebene z 4500, Unterkante auf dem Boden y 0 -> 1581 x 3487; Kamera Cut 6 @0x120), der sichtbare Fluegel 0,511. Der Inhalt war also um
   21 % (Rueckwand) bzw. 29 % (Fluegel) waagrecht gestaucht.
2. Affine Abbildung. Jede Flaeche war EIN Viereck. SDL_RenderGeometry bildet je Dreieck affin ab (wie die PSX-GPU); unter dem streifenden Blick von
   Cut 3 (Tiefe 3322..5317) liegt ein Texel bis 9,49 Bildpunkte neben seinem perspektivisch richtigen Ort, der Knick laeuft entlang der Diagonale v1-v2.

**Aenderung (`schrank_bild.py` -> `engine/src/gen/schrank2010_r41.inc`):**
* `entzerren()`: Viereck auf sein wahres Seitenverhaeltnis entzerren (`seitenverhaeltnis()`), dann mittig auf 1300:3600 beschneiden, in einer einzigen
  Abtastung. Gleicher Massstab in beiden Achsen; links und rechts faellt ein Rand weg (Rueckwand: 78,6 % der Breite bleiben, Fluegel 70,7 %).
* `md1_gitter()`: jede Flaeche als Gitter 4 x 8 Vierecke (Form der Wasserflaeche ROOM2080 Prop 0, deren Netz ebenfalls in Vierecke <= 1800 x 1800
  unterteilt ist). `affin_fehler()` rechnet den groessten Versatz: Rueckwand 0,40, Fluegel bei rot_y 0/512/1024 0,40/0,30/0,12 Bildpunkte (EIN Viereck:
  9,49); der Generator bricht ab, wenn er 0,5 erreicht. Gitter 1 x 1 ergibt das alte Viereck (Ecken, UV, CLUT, tpage) gleich (geprueft).
* PORT-WAHL: der mittige Ausschnitt. Die Original-RDTs haben keinen Schrank-Fluegel (Spur N §1); Cut 8/9 (Nahansicht beim Einsetzen) sind
  unveraendert die Original-Hintergruende von 2060.

**Messung nachher:**

| Fenster | Rueckwand sx/sy (NCC) | Fluegel sx/sy (NCC) |
|---|---|---|
| RE15_WINDOW_SCALE=1, Leon | 1,038 (0,73) | 1,019 (0,84) |
| RE15_WINDOW_SCALE=3, Leon | 1,038 (0,73) | 1,019 (0,84) |
| RE15_FULLSCREEN 1920x1080, Leon | 1,038 (0,73) | 1,019 (0,84) |
| RE15_WINDOW_SCALE=1, Elza | 1,038 (0,73) | 1,019 (0,84) |

Der Rest von 3,8 % liegt in der Messung (Schritt 0,02, Vorlage mit 0,453 statt 0,460). C-Werkzeug `kasten` (Riegel, gleicher Weg, 20 Einheiten je
Bildpunkt, Lage +-6 um die Mitte) auf denselben Fensterbildern: vorher Rueckwand 0,717 (NCC 0,265), Fluegel 0,740 (0,740); nachher 1,019 (0,798) und
1,019 (0,905); Gegenprobe (Port-Flaeche auf 80 % gestaucht) 0,827 / 0,808.
Bilder: `PF_belege/schrank_cut3_vorher_nachher_leon.png`, `_elza.png` (je links vorher, rechts nachher, Fenster 1, doppelt),
`schrank_innen_entzerrt_vorher_nachher.png` (je Port | 2060).

## 2. Panel 2030: Zahlen und Texte verschwommen

**Messung vorher (gdigrab, Fenster 1 und 3, Panel offen, Text 41 weg):** je Schild der Anteil der Schriftpixel (Abstand zum Grund > 20 % des Kontrasts),
die unter 60 % bleiben ("Zwischentoene", weiche Kanten):

| Schild | vorher | nachher |
|---|---|---|
| A 100 | 59,1 % | 0,0 % |
| B 50 | 57,1 % | 0,0 % |
| C 30 | 57,5 % | 0,0 % |
| 80 (Ziel an Tank A, rot) | 61,0 % | 0,0 % |
| BRIDGE | 62,6 % | 0,0 % |
| PRESSURE 80 / SET 80 | 61,0 % | 0,0 % |
| CAUTION / HIGH PRESSURE | 67,6 % | 3,2 % (Rand des Schilds) |

Fenster 1 und Fenster 3 ergeben dieselben Werte (die Vollbild-Ebene wird nearest vergroessert, kein `SDL_HINT_RENDER_SCALE_QUALITY`, SDL-Standard "0").
Bild: `PF_belege/panel_vorher_nachher_fenster3.png`.

**Ursache:** nicht die Darstellung (Vollbild-Ebene `re15_render_pc_show_fmv` 320 x 240, 1:1 in die logische Groesse), sondern die Quelle. `panel_kit.text()`
setzte Arial Bold (TrueType) mit 4,6..8,2 Bildpunkten Hoehe, 4-fach ueberabgetastet, Gauss 0,1..0,25 Bildpunkte, dann `fertig()`: INTER_AREA auf
320 x 240, Rauschen 1,9, JPEG q84 mit 4:2:0 (Farbe auf halber Aufloesung, darum die rote "80" am staerksten), Gamma 1,16. Ein 1-Pixel-Strich wird
dabei zu einem grauen Verlauf; vergroessert bleibt der Verlauf ein Verlauf.

**Aenderung (`tools/r41_nachtrag30/panel_bild.py` -> `engine/src/gen/panel2030_r41.inc`):**
* Das Bild laeuft OHNE Schrift durch dieselbe Kette (Look unveraendert), danach stempelt `beschriften()` die Beschriftung Texel fuer Texel: Glyphen der
  8x8-Schrift des Spiels aus `DATA/TEX.TIM`, Zeichen c bei u = ((c-0x20) & 0x1f) * 8, v = ((c-0x20) & 0xe0) >> 5 * 8 auf der Seite ab Texel x 256
  (FUN_80029214 @0x80029214, tpage VRAM 768 = TIM-Texel 256, CLUT (256,480); Herleitung render_pc.c `re15_render_pc_debug_text`), dieselbe Schrift in
  RE2 TEX.TIM. Je Schild eine 16er-CLUT wie auf der PSX: helle Schrift = CLUT-Zeile 0 unveraendert (mit Schlagschatten Index 8), dunkle Schrift auf
  dem hellen Feld ohne Schatten, rote Ziffern. Ganzzahlige Lage, 15 Bit, kein Weichzeichnen.
* PORT-WAHL (Gestaltung, keine Original-Werte): Laufweite Buchstaben 8 (wie FUN_80029214), Ziffern 6 (Tinte 5 breit), Leerzeichen 3; Tankplaettchen
  50,5..82,5 / 136..160 / 180..204 x 132..142 (Lampen unveraendert 85/161/205); Zielmarke 91..106 x 71..81; Schild oben "SET 80" statt
  "PRESSURE 80" (11 Zeichen = 88 Bildpunkte, das Feld hat 54); rotes Schild 122..230 statt 129..184,5 ("HIGH PRESSURE" = 98 Bildpunkte); die Ziffern
  0/50/80 im Zifferblatt entfallen (18 Bildpunkte Innenraum, eine 6x8-Ziffer haette Nabe und Teilstriche ueberdeckt), das rote Band 80..100 bleibt.
* `re15_panel2030_r41_glyphe()` (engine/src/panel2030_r41.c) gibt Glyphen und CLUTs an die Riegel.
* Dabei gefunden (gdigrab Elza, Text 41 im Panel): die jetzt scharfe, breitere Schrift des roten Schilds schien durch das Band hinter dem
  Nachrichtentext (Deckung 170/256 = 66 %), "then a tank to fill." lag sichtbar ueber "HIGH PRESSURE". Band jetzt 232/256
  (`RE15_R41N_NACHRICHT_DECKUNG`, PORT-WAHL wie das Band selbst): mittlere Helligkeit rechts des Textes (x 165..228, y 184..204) vorher 12,9,
  mit Deckung 170 29,3, mit 232 9,5. Bild `PF_belege/elza_frage_panel_vorher_nachher.png` (oben vorher, unten nachher).

**Messung nachher:** Tabelle oben (Leon, Fenster 1 und 3; Elza Fenster 1 dieselben Werte). Werkzeug `etikett` (Riegel): im gdigrab-Bild (Fenster 1,
beschleunigter Renderer) 1334 von 1334 gesetzten Texeln gleich der Spielschrift in ihrer CLUT, um 1 Bildpunkt versetzt 450 von 1334; vorher 0 von
1334. In den Framedumps F780/F900 des Riegels ebenso.

## 3. Text vor dem Raetsel ueberlappt die Ja/Nein-Auswahl

**Messung vorher (gdigrab, Leon):** Text 40 "A control panel for the bridge." / "Will you operate the panel?" auf einer Seite, die zweite Zeile laeuft in
die Auswahl: im Bild "Will you operate the **Yes**el?   No" (`PF_belege/frage40_vorher_nachher.png` links).

**RE-Beleg (PSX.EXE, ghidra1_V2.txt):** FUN_80028134 Zustand 4 zeichnet den Cursor mit `lhu a0,DAT_800b8536` @0x80028624, `addiu a0,a0,0x10` @0x80028630,
x = 0xa0 + Wahl * 0x46 (@0x8002863c..@0x80028650), und "Yes No" (DAT_800c4954) mit `addiu a0,a0,0x10` @0x80028674, x `ori a0,a0,0xae` @0x80028680.
DAT_800b8536 = 0xb4 (@0x80027f14) wird nur in FUN_80027e68 geschrieben (@0x80027f1c, @0x80027f60, @0x80027fa0, @0x80027fd8), ein Zeilenumbruch
aendert ihn nicht: die Auswahl steht IMMER auf der zweiten Zeile der Box (y 196), Cursor ab x 160, "Yes" ab 174. Das ist im Port schon so
(`engine/src/msg_select_common.c`). Zensus aller Original-RDT-Nachrichten mit 0x03 (90): 61 einzeilige Frageseiten, 29 zweizeilige mit Zeilenende
x 34..147 (vor dem Cursor), keine dreizeilige. Die Port-Frage Id 40 endete in Zeile 2 bei x 218 (Breiten DEBUG.BIN @0x4416).

**Aenderung:** Id 40 auf zwei Seiten: "A control panel for the bridge." | "Will you operate the panel?" (Form ROOM2060 msg 0 @0x1766: Erklaerung, Seite,
Frage allein); Wortlaut unveraendert. `tools/r41_ventilkette/gen_r41_ventilkette.py` prueft jetzt jede Port-Frage (`frage_frei`: hoechstens zwei
Zeilen, eine zweite Zeile endet vor x 160) und bricht sonst ab. Zensus aller Port-Fragen in `engine/src/gen/*.inc`: 16, nach der Aenderung keine
mehr auf der Auswahl.

**Messung nachher (gdigrab):** Seite 1 mit Seitenpfeil, Seite 2 "Will you operate the panel?" auf Zeile 180, "▶ Yes   No" frei auf 196
(`PF_belege/frage40_vorher_nachher.png` rechts). Die Lage ist in logischen 320 x 240 gerechnet und damit in jeder Fenstergroesse gleich.

## Tests

Neu in `unit_r41_nachtrag30` (`tests/unit/test_r41_nachtrag30.c`):
* G: jede Glyphe der Beschriftung im Panel-Bild (`re15_panel2030_r41_rgba`) Texel fuer Texel = 8x8-Spielschrift aus TEX.TIM in ihrer CLUT:
  `G Beschriftung: 42 Glyphen, 1334 von 1334 Texeln gleich der Spielschrift, um 1 Bildpunkt versetzt 450 von 1334`.
* Q: Lage jeder Port-Frage (2000/2010/2030/2050/2070/20B0) mit dem echten `re15_msg_layout` und `re15_msg_select_layout`, Breiten DEBUG.BIN @0x4416:
  `Q Ja/Nein: 11 Port-Fragen, 0 ueberdecken; Id 40 neu frei, alte Id 40 8 Glyphen auf der Auswahl` (Gegenprobe mit den alten Bytes).
* Werkzeuge `etikett <ppm>` (Beschriftung im Bild gegen die Spielschrift) und `kasten <ppm> <innen|tuer>` (Seitenverhaeltnis gegen ROOM2060 Cut 6).

Riegel in `tests/integration/test_r41_nachtrag30.cmake`:
* `_panel_*` Lauf p: F780 und F900 `etikett` muss 1334 von 1334 ergeben, versetzt < Haelfte (Leon und Elza: 1334/1334, versetzt 450);
  msgtext Id 40 jetzt `"A control panel for the bridge.Will you operate the panel?"` (Seitenwechsel), ebenso `test_r41_ventilkette_b.cmake`.
* `_schrank_*` neuer Lauf pf: Tuer auf, Texte weg, Figur nach links, F720 unter Cut 3: `kasten innen/tuer` mit NCC >= 600, sx/sy 940..1060 Promille,
  Gegenprobe (auf 80 % gestaucht) <= 880. Leon und Elza: innen NCC 782 sx/sy 1038 Gegenprobe 827, Fluegel NCC 873 sx/sy 1000 Gegenprobe 808.
* Gegenprobe des Riegels mit der exe der Basis (`re15_pc_pfvorher.exe`, Skript von Hand mit TEIL=schrank, FIGUR=leon): alle alten Laeufe gruen, Lauf pf
  rot: `innen verzerrt gegen ROOM2060 Cut 6 (NCC 242, sx/sy 736 Promille, Gegenprobe 642)`.

Zielauswahl (Bau `local_build.sh build` im Arbeitsbaum, ctest aus PowerShell mit `C:\msys64\mingw64\bin` vorn im PATH):

```
ctest --test-dir re15_port/build -R "nachtrag30|ventilkette|keyitems|schrank|panel" -j3 --timeout 3000
100% tests passed, 0 tests failed out of 49
Total Test time (real) = 7038.67 sec
```

Darunter integration_r41_nachtrag30_panel_leon/_elza (99 s / 140 s), _schrank_leon/_elza (254 s / 372 s), _laden_*, _wasser_*, _ton, _laut,
integration_r41_ventilkette{,_b,_alarm,_kette,_kette_elza,_weg,_weg_elza,_sperre}, integration_r41_keyitems_* (13), integration_r37_codepanel, alle unit-Riegel der Auswahl.

## Commits

* `fix(r41-panelfix): Schrank 2010 unverzerrt, Panel 2030 in der Spielschrift, Frage 40 frei von Ja/Nein` (Code, Generatoren, gen/*.inc, Riegel)
* `docs(r41-panelfix): Dossier PF und Belege vorher/nachher (gdigrab)`

## Fuer den Nutzer

* Schrank 2010: der Sicherungskasten hat jetzt dieselben Proportionen wie in 2060; links und rechts ist dafuer ein schmaler Rand des Kastens
  abgeschnitten (die Tuer in 2010 ist schmaler als der Kasten in 2060). Der Knick quer durch den Kasten ist weg.
* Panel 2030: die Beschriftung ist jetzt die kleine Pixelschrift des Spiels. Das Schild oben rechts heisst "BRIDGE / SET 80" (fuer
  "PRESSURE 80" ist es in dieser Schrift zu schmal), das rote Schild ist breiter, die winzigen Zahlen im runden Zeiger sind weg (das rote Feld
  80..100 und die rote 80 am Tank A zeigen das Ziel). Nachrichten im Panel stehen auf einem dunkleren Band.
* Frage am Pult: "A control panel for the bridge." erscheint jetzt allein, danach "Will you operate the panel?" mit Yes/No darunter
  (ein Tastendruck mehr, wie bei den Original-Fragen mit Erklaerung, z.B. ROOM2060).

## Nachbesserung 1 (2026-11-03)

Grundlage: Abnahme 1 (`PF_abnahme_1.md`, M1 und M2, §3a, §6). Belege `PF_belege/nachbesserung1/` (Messwerte `mass_nb1.txt`, Werkzeuge `werkzeuge/`,
dazu die Abnahme-1-Werkzeuge `PF_belege/abnahme1/werkzeuge/` geo.py, inhalt.py, logisch.py, waagrecht.py, kasten_geo.py).
Messweg wie Abnahme 1: eigene exe-Kopien, echtes Fenster 960x720, Renderer direct3d, gdigrab per hwnd der eigenen PID, kein AUTOSHOT/SOFTWARE_RENDER;
vorher = exe des Stands 422048234 (`re15_pc_nb1vorher.exe`), nachher = Arbeitsbaum.

### Nutzerurteil Punkt 3 (Lesbarkeit der Panel-Schrift)

Der Nutzer hat die Schrift der Spur PF am Vergleichsbild `PF_belege/abnahme1/panel_texte_nutzer_gegen_pf.png` (sein Fenster vor PF gegen den Stand der
Spur) beurteilt, woertlich: **"ja, ist super."** Damit ist Punkt 3 (Lesbarkeit der Beschriftung) vom Nutzer abgenommen. Die Schrift bleibt in dieser
Nachbesserung unveraendert (Glyphen, CLUTs und Lagen gleich; Riegel G: 1334 von 1334 Texeln = Spielschrift). Zugesagt waren dem Nutzer noch:
Kasten-Scherung beheben, Drehscheiben und Teilstriche sauber auf ganze Bildpunkte.

### M1: Inhalt des Kastens 2010 geschert -> Entzerrung ueber die RDT-Kamera ROOM2060 Cut 6

**Ursache (Abnahme 1 §2c/§6, hier nachgemessen):** die Handvierecke `Q_INNEN`/`Q_TUER` passten nicht zur Kamera. Rueckwand: Ecke rechts oben (122,28)
liegt nicht auf der Fluchtlinie der Waagrechten -> Waagrechte +4,2 Grad (Rechnung) / +4,5 Grad (Fenster). Fluegel: linke Kante von `Q_TUER` = Bildrand,
die Homographie nahm sie als Senkrechte -> Senkrechte 6,0 / 6,1 Grad. Dazu (neu gefunden): auch die Richtung des Fluegels aus den Handkanten
(Fluchtpunkt x 139,7) war falsch. Die vier Kanten der zwei Schilder auf dem Fluegel (BG06, Schwellenkreuzung je Spalte) treffen den Horizont der
Kamera (y 67,98) bei x 130,6 / 131,5 / 131,8 / 130,3, gemeinsamer Fluchtpunkt **x 130,80** (Rest 1,48 Bildpunkte^2 ueber 56 Kantenpunkte) ->
Fluegelrichtung 66,96 statt 64,6 Grad. Mit 130,8 entzerrt sind beide Schilder in BG06 Rechtecke (Waagrechte 0,0, Senkrechte 0,0 Grad).

**Kamera-Beleg:** ROOM2060.RDT Kopf +0x24 -> Kameratabelle @0x60, Satz 6 @0x120:
`00 00 3c 68 e6 eb ff ff 74 f5 ff ff 46 fe ff ff 89 18 00 00 42 0b 00 00 28 49 00 00 60 06 00 00` = fov 0x683c = 26684 -> H = 26684 >> 7 = 208,
Auge (-5146,-2700,-442), Ziel (6281,2882,18728). Achsen und Projektion wie `camera_common.c` (FUN_80053ca4 @0x80053ca4: Zeile 0 = (cos yaw, 0, -sin yaw),
Zeile 2 = Blickrichtung; sx = 160 + H*x/z, sy = 120 + H*y/z); der Generator rechnet dieselben Achsen in Gleitkomma.

**Aenderung (`tools/r41_nachtrag30/schrank_bild.py` -> `engine/src/gen/schrank2010_r41.inc`):**
* Keine Handvierecke mehr. Je Texel ein Weltpunkt auf der 2060-Flaeche -> Kamera Cut 6 -> Bildpunkt in BG06 (`ausschnitt()`, bikubisch). Damit sind Senkrechte
  der Welt senkrecht, Waagrechte waagrecht, gleicher Massstab in beiden Achsen.
* Rueckwand: Ebene z 4500 (achsparallel; z bestimmt nur den Massstab, die Unterkante liegt damit auf dem Boden y 0). Rechteck im entzerrten BG06
  gemessen (5 Einheiten je Bildpunkt, Helligkeitsprofile): Fuge links x -4830, Fuge rechts vor der grauen Nachbartuer x -3250, Deckeloberkante
  y -3545, Boden y 0 (Mittel der alten Handecken: -4831 / -3255 / -3554 / -68). B/H 0,446, mittig auf 1300:3600 = x -4680..-3400 (81,0 % der Breite).
* Fluegel: senkrechte Ebene durch (-4830, 0, 4500) mit der Richtung aus dem gemessenen Fluchtpunkt (`fluchtpunkt()` im Generator, Abbruch, wenn er
  ausserhalb 125..137 liegt oder eine Kante um mehr als 3 abweicht). Hoehe y -3198 (Oberkante) .. 15 (Uebergang zum Boden), Scharnierfuge s 290,
  sichtbar bis s 1538 (Bildrand, `sichtbar_bis()`); mittig auf 1300:3600 = s 1494..334 (93,0 % der sichtbaren Breite). Der freie Rand liegt
  ausserhalb von BG06 (wie bisher).
* UV wie das Netz: Texelmitte -> Anteil ueber die Flaeche je Gitterzelle zwischen den gerundeten Gitter-UV (`gitter_uv()`, `flaeche_aus_texel()`),
  SDL tex_coord = u / Texturbreite (render_pc.c). Netz, Gitter 4 x 8, Ecken, Tuer aussen: Bytes unveraendert (nur die zwei Texturen innen/Fluegel neu).
* Ausschnitt in BG06 (jetzt Projektion statt Handecken): innen (53.1,24.3) (115.2,30.1) (122.0,170.6) (71.5,183.6), Fluegel (2.6,28.7) (39.9,40.1)
  (60.2,194.2) (37.7,234.4).
* PORT-WAHL wie bisher: mittiger Ausschnitt, Rechteckkanten aus dem Bild gemessen. Kein Original-Wert betroffen (die Original-RDTs haben keinen Fluegel).

**Messung im Fenster (Leon, b_00113 = 28 s, Tuer offen, Cut 3; `mass_nb1.txt` §1):**

| Flaeche | Mass | vorher | nachher |
|---|---|---|---|
| Rueckwand | Seitenverhaeltnis V (ECC, inhalt.py) | 1,021 | 1,010 |
| Rueckwand | Achsenwinkel (ECC) | 85,1 Grad | 89,7 Grad |
| Rueckwand | Waagrechte (waagrecht.py, entzerrtes Fensterbild) | +4,5 Grad | 0,0 Grad |
| Rueckwand | Senkrechte (dto.; Vorlage 2060 allein: -1,4 = Grenze des Verfahrens) | -2,1 Grad | -0,6 Grad |
| Fluegel | Seitenverhaeltnis V (ECC, Fluchtpunkt 130,8) | 0,928 | 1,014 |
| Fluegel | Achsenwinkel (ECC) | 85,8 Grad | 89,6 Grad |
| Fluegel | Senkrechte / Waagrechte | -6,1 / 0,0 Grad | 0,0 / 0,0 Grad |

Elza (ROOM2011, eigener Lauf): nachher dieselben Werte (V 1,010 / 1,014, Waagrechte 0,0, Senkrechte -0,6 / 0,0). Kanten gegen die Projektion der
Port-Geometrie (kasten_geo.py): rechte Kante Median -1,07 vorher / -1,08 nachher, Oberkante -0,60 / +0,22 Bildpunkte (die Geometrie ist unveraendert;
die linke Kante misst in b_00113 die Figur mit, vorher und nachher gleich -15,4).
Hinweis zum Mass der Abnahme 1: fluegel.py entzerrte die 2060-Vorlage mit dem Fluchtpunkt der Handkanten (139,7). Damit ergaebe sich nachher
V 1,108; mit dem gemessenen Fluchtpunkt 130,8 sind es 1,014, und der Stand der Spur PF hatte in Wahrheit 0,928 (nicht 1,013).
Bilder: `kasten_cut3_vorher_nachher.png` (Leon vorher | Leon nachher | Elza nachher), `kasten_entzerrt_vorher_nachher_2060.png` (Rueckwand vorher |
nachher | 2060, Fluegel vorher | nachher | 2060, Hilfsgitter 200 Einheiten: vorher stehen Roehren und Schilder schief, nachher wie in 2060).

**Riegel (`kasten`, Framedump F720 Cut 3, Lauf pf; Hundertstel Grad):**

| Lauf | innen | Fluegel |
|---|---|---|
| vorher (Basis-exe, Leon) | NCC 610, V 1038, W 400, S 100 | NCC 463, V 1098, W -200, S 350 |
| nachher (Leon) | NCC 753, V 1000, W 25, S -50, Gegenprobe 5 Grad -> -475 | NCC 883, V 1018, W -25, S -50, Gegenprobe -> -550 |

### Panel 2030: Drehscheiben, Teilstriche und Zeiger auf ganzen Texeln

**Ursache:** Instrument und Drehscheiben liefen wie das uebrige Panel durch die Kette (4-fach ueberabgetastet, weiche Kanten `aa()`, INTER_AREA,
Rauschen, JPEG 4:2:0, Gamma 1,16): Teilstriche 0,44..0,72 Texel breit, Querbalken 2,3 Texel, rotes Band aus 21 Strichen 0,84 Texel -> graue Verlaeufe
(Abnahme 1 §3a). Der Zeiger wurde zur Laufzeit mit weicher Kante gezeichnet (`strecke()`, Halbbreite 0,62/0,34): bei jeder Lage andere Zwischentoene,
waehrend des Umfuellens flimmernd.

**Aenderung (PORT-WAHL wie das Panel; kein Original-Wert betroffen):**
* `tools/r41_nachtrag30/panel_bild.py`: in der Kette bleibt von den fuenf Scheiben nur der Schlagschatten. `scheiben_stempeln()` setzt sie NACH der Kette
  Texel fuer Texel (wie die Schrift): Lippe, Ring, innere Lippe, Blatt je Texel nach dem Abstand der Texelmitte (keine Teilbedeckung), Farben und Licht
  wie `kit.scheibe`, Glas nur auf dem Blatt und vor den Strichen, dann das Gamma von `fertig()`. Striche mit `linie_px()`: je Schritt entlang der
  Hauptachse genau ein Texel; Querbalken zwei Texel je Spalte in der Steigung 1:2 (63,4 statt 60 Grad, gleichmaessige Treppe); rotes Band = Texel,
  deren Mitte im Ringsektor 80..100 liegt. Deckungen wie bisher (Teilstriche Instrument 0,9, Band 0,95, Scheiben 0,6, Balkenschatten 0,32, je ganzes Texel).
  Mitten auf Texelmitten: Instrument 124,5/87,5 (frueher 124/87), Scheiben 125,5 / 163,5 / 200,5 / 237,5 bei 160,5 (frueher 125 / 163 / 200,5 / 237,5 bei 160).
  Neu im .inc: `re15_r41n_panel_stempel` (288 Strich-/Band-/Balken-Texel mit Farbe) fuer den Riegel.
* `engine/src/panel2030_r41.c`: `strecke()` -> `linie_px()` (dieselbe Regel wie im Generator), Zeiger: Schaft zwei Texel je Schritt, Spitze einer,
  Schatten = Mittellinie um (+1,+1) mit Deckung 87/256, Nabe = Texel mit Mittenabstand <= 2,3, Drehpunkt Texelmitte (`RE15_R41N_ZEIGER_X/Y` + 0,5).
  Neu: `re15_panel2030_r41_stempel()`, `re15_panel2030_r41_grund_rgba()` (Riegel).
* Schrift, Schilder, Tanks, Cursor, Band unveraendert (Etiketten-Lagen gleich, G 1334/1334).

**Messung im Fenster (Leon, `mass_nb1.txt` §3, scheiben_mass.py):**

| Mass | vorher | nachher |
|---|---|---|
| Querbalken der Drehscheiben: Zwischentoene auf der Seite ohne Schatten | 59 von 268 Texeln (22,0 %) | 0 von 272 (0,0 %) |
| Teilstriche 0..70 des Instruments: Kontrast des dunkelsten Texels zum Blatt | 0,62 (0,32..0,89) | 1,05 (alle gleich) |
| Zeiger: Zwischentoene auf der Seite ohne Schatten (6 Bilder, auch waehrend der Fahrt 100 -> 50) | 2..6 von 16..18 | 0 von 16..21 |

3x3-Bloecke einfarbig 1,0000 (nearest, wie bisher). Bilder: `panel_vorher_nachher_fenster3.png` (A 50, Cursor an Tank B; links vorher, rechts nachher),
`panel_scheiben_vorher_nachher_zoom.png` (Instrument + Scheiben, 6-fach), `zeiger_vorher_nachher.png` (links vorher, rechts nachher; je b_00109 A 100, b_00120 und b_00121 waehrend der Fahrt, b_00155 A 80; 9-fach).

### Tests

* `unit_r41_nachtrag30` neu **S**: S1 jeder Stempel-Texel steht unveraendert im Panel-Bild (279 von 279; die 9 unter dem Zeiger ausgenommen), Gegenprobe um
  (2,2) versetzt 0 von 279. S2 Zeiger in jedem Bild der Loesungsfahrt A 100 -> 50 -> 80 (55 Bilder): jeder Texel des Blatts ist Grund, Zeigerfarbe, Nabe oder
  Grund mit Schatten 87/256 -> **0 Zwischentoene**, je Bild >= 32 Zeiger-Texel. Gegenprobe: mit dem alten weichen `strecke()` rot (bis 52 Zwischentoene je Bild).
  Ausgabe: `S Scheiben: 288 Stempel-Texel, 279 von 279 gleich, um (2,2) versetzt 0 von 279; Zeiger 55 Bilder (A 80), Zwischentoene max 0, Zeiger-Texel min 32`.
* Werkzeug `kasten`: Vorlage jetzt ueber die Kamera ROOM2060 Cut 6 (statt Handviereck + B/H 0,453/0,511), dazu Scherung: Port-Flaeche mit geneigten Achsen
  (-8..+8 Grad in 0,25) gegen die Vorlage im besten Massstab -> `SCHERUNG_W`/`SCHERUNG_S`, Gegenprobe 5 Grad -> `GEGENPROBE_SCHERUNG`.
* Riegel `integration_r41_nachtrag30_schrank_*` Lauf pf: zusaetzlich |W|, |S| <= 100 (1 Grad) und Gegenprobe um 4..6 Grad versetzt; mit der exe vor der
  Nachbesserung rot (innen W 400, Fluegel NCC 463 / S 350).

**Laeufe (Bau `local_build.sh build` im Arbeitsbaum, ctest aus Git-Bash mit `C:\msys64\mingw64\bin` vorn im PATH):**

```
ctest -R "^(unit|prop|probe)_" -E "releaseskript" -j6 --timeout 900
99% tests passed, 2 tests failed out of 1053   (Total Test time 1406,67 sec)
  481 - unit_r35_android_bash_mutanten (Failed)    Mutanten von release/apk_pruefen.sh
  1105 - unit_r39_titelversion_ablauf (Timeout)    echtes release_alles.sh
```
Beide sind Release-Skripttests (das Muster "releaseskript" fing sie nicht) und fallen unter den df-Haenger der Maschine: waehrend des Laufs
standen 101 haengende `df.exe` in der Prozessliste. Keine der geaenderten Dateien liegt unter release/ oder beruehrt Titel/Android.
Darin gruen: `unit_r41_nachtrag30` (G 1334/1334, Q, neu S), `unit_r41_ventilkette`, `unit_r41_keyitems_*`, `unit_r37_codepanel_*`, `unit_r26_panel_11f0`,
`unit_r27_panel_schalterwerte`.

```
ctest -R "^integration_(r41_nachtrag30_(panel|schrank|laden)_(leon|elza)|r41_ventilkette_b)$" -j3 --timeout 3000
100% tests passed, 0 tests failed out of 7   (Total Test time 1188,69 sec)
  _panel_leon 130,1 s, _panel_elza 177,1 s, _schrank_leon 312,5 s, _schrank_elza 453,3 s, _laden_leon 227,6 s, _laden_elza 215,9 s,
  integration_r41_ventilkette_b 605,2 s
```
Riegel-Ausgaben: `_panel_leon/_elza` F780/F900 Beschriftung 1334 von 1334 (versetzt 450); `_schrank_leon` und `_schrank_elza` Lauf pf
`KASTEN innen NCC 753 SX 102 SY 102 VERHAELTNIS 1000 GEGENPROBE 804 SCHERUNG_W 25 SCHERUNG_S -50 GEGENPROBE_SCHERUNG -475`,
`KASTEN tuer NCC 883 SX 112 SY 110 VERHAELTNIS 1018 GEGENPROBE 818 SCHERUNG_W -25 SCHERUNG_S -50 GEGENPROBE_SCHERUNG -550`.
Gegenprobe des Riegels: dasselbe Werkzeug auf dem Framedump desselben Laufs mit der exe vor der Nachbesserung (`werkzeuge/lauf_pf.py`):
innen W 400 (> 100), Fluegel NCC 463 (< 600) und S 350 -> rot.

### Commits (Nachbesserung 1)

* `fix(r41-panelfix): Kasten 2010 ueber die Kamera ROOM2060 Cut 6 entzerrt, Panel-Scheiben und Zeiger auf ganzen Texeln` (bb82c0a7c)
* `docs(r41-panelfix): Dossier PF Nachbesserung 1 und Belege vorher/nachher (gdigrab)`

### Fuer den Nutzer (Nachbesserung 1)

* Schrank 2010: der Inhalt steht jetzt gerade im Kasten - Sicherungsroehren senkrecht, Rahmen und Schilder waagrecht, wie im Original-Bild in 2060.
  Die Bilder werden jetzt aus der Kamera von 2060 entzerrt statt aus von Hand gesetzten Ecken. Auf der offenen Tuer sind die zwei Schilder jetzt Rechtecke
  (vorher schiefe Parallelogramme); die Tuer zeigt etwas mehr von ihrem unteren Teil.
* Panel 2030: Schrift unveraendert. Die vier Drehscheiben, das runde Zeiger-Instrument (Teilstriche, rotes Feld 80..100) und der Zeiger sind jetzt
  scharfe Pixel ohne graue Verlaeufe; der Zeiger flimmert beim Umfuellen nicht mehr. Der Querbalken der Drehscheiben steht etwas flacher (1:2).
* Nach dem Merge den Hauptbaum neu bauen (der Nutzer spielt aus `C:\workspace\git\reAi_v2\re15_port\build`, Abnahme 1: dort lief noch der Stand
  vor PF), sonst ist weder PF noch diese Nachbesserung zu sehen.
