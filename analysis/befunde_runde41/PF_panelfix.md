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
