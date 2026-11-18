# Einbau K5e_f1 (E32) - Zahlen-Pruefer Runde 1

Stand: r41/form HEAD 2b9a57ec1 (Commits 2c98abb54, 21c4bc685, 62bd0001b, 0097848a0, 2b9a57ec1). Nicht committet.
Alles mit eigenem Code gemessen. Die Riegel des Bauers wurden nicht nachgestartet.
Skripte und Rohausgaben liegen unter
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5e_einbau/zp/

- Eigener EMD/TIM-Leser: py/zlese.py (8 EMD-Sektionen, MD1 mit 64 Objekten, TIM 8 bpp mit CLUT, CRC32 zlib).
- Vergleich: py/p13.py -> p13.txt; F1: py/pf1.py, pf1d.py -> pf1.txt, pf1d.txt.
- Eigener Rasterer: py/prender.py, pring.py, pfeld.py -> render*.txt/json.
- Szene: py/pszene.py -> szene440.txt.

Ergebnis: **BESTANDEN**. Es gibt keinen roten Punkt, aber vier Hinweise (H1-H4).

## 1 Asset = K5e_f1; gegen K5e nur F1-Zeile

| Pruefung | Soll | Ist (Beleg) |
|---|---|---|
| Asset-EMD = ruhe/K5e_f1 | byte-gleich | f277916d7d6518b5 = f277916d7d6518b5, Bytevergleich gleich (p13.txt) |
| Asset-TIM = ruhe/K5e_f1 | byte-gleich | 31a35e66428bac1e = 31a35e66428bac1e (p13.txt) |
| EMD gegen K5e | gleich | Asset-EMD == ruhe/K5e/BAHNSTEIG_FORM.EMD: True |
| CLUT-Zeilen gegen K5e | nur Zeile 8 anders | Zeilen 0-7 und 9: je 0 anders; Zeile 8: 24 Eintraege anders (p13.txt) |
| Kern-Eintraege | bytegleich K5c-Zeile 5 | Kernmaske selbst mitgeschnitten (eigener Haken auf binary_fill_holes in kf_g4.bau, py/pkern.py; Nachbau-TIM sha 512828067ea7f048 = farbe/K5c_farbe_v3/bau.json). Kern 122, Rand 70, Feldhaut 881 Texel. 56 Kern-Eintraege, davon 0 geaendert, alle bytegleich K5c-Zeile 5 (pf1d.txt) |
| Geaenderte Eintraege | nur Nicht-Kern | Die F1-Flaechen lesen 80 Eintraege, davon 24 nicht Kern; diese Menge ist genau die geaenderte Menge (True). |
| Aufteilung der 24 | 16 F11-Haut, 8 nur Rand | 16 sind wortgleich K5e-Zeile 3. Die 8 Eintraege 67, 75, 78, 84, 86, 232, 245, 246 liegen auf 12 Randtexeln, 0 Kern- und 0 Feldhauttexeln. |
| Fleisch-Randtexel | - | 40, davon 28 auf Kern-Eintraegen (bleiben K5c), 12 folgen der Haut |
| Texel (Pixel) | gleich | TIM-Pixel Asset == K5e: True; Asset == K5c: True |
| UV/Flaechen/Page | gleich | UV, Indizes und Page gegen K5c: 0 Unterschiede in 64 Objekten |
| 0x0000 (durchsichtig) | nicht neu | Zeile 8 Asset: kein 0x0000. K5c [255]=0x8000 wird a0c8 (= K5e-Zeile 3 [255]). |

Albedo L* mit eigenem Lab (sRGB, D65), K5c / K5e / K5e_f1:

- Kern: 48,57 / 48,57 / 48,57
- Rand: 19,98 / 19,98 / 32,91
- Feldhaut: 6,69 / 6,69 / 27,76

Der Bauer nennt 48,49 / 19,91 -> 32,85 / 6,65 -> 27,69. Der Unterschied kommt von der Lab-Umrechnung.

## 2 F1-Ring selbst gerendert

### 2a Weisses Licht, eigener Ortho-Rasterer

Aufbau: Ruhelage (alle Winkel 0, Knochen aus EMR-relpos), Blick entlang der F1-Normale. Es gibt einen Kennungspuffer (Flaeche, Texel).

- Ring = Pixel, deren Texel in der Randmaske liegt.
- Angrenzende Haut = Pixel der Mesh-0-Flaechen ausser F1 im Abstand <= 3 bzw. 6 px.

render_ring.txt, Werte bei 3 px:

| Variante | Licht | dL Ring | dL Rand+Feldhaut | dL Kern |
|---|---|---|---|---|
| K5c | Albedo | +10,06 | -1,69 | +38,28 |
| K5e | Albedo | **-11,71 ROT** | -23,47 | +16,50 |
| K5e_f1 (Asset) | Albedo | **+0,92 GRUEN** | -3,66 | +16,50 |
| K5c | Lambert 0,3+0,7 | +10,77 | -0,66 | +39,02 |
| K5e | Lambert | **-8,90 ROT** | -20,33 | +19,36 |
| K5e_f1 (Asset) | Lambert | **+3,48 GRUEN** | -1,05 | +19,36 |

Mit 6 px Abstand ergeben sich dieselben Vorzeichen (Asset Ring +1,03 / +3,80; K5e -11,60 / -8,57).
Die eigene Tafel render_tafel.png zeigt: Bei K5e ist das ganze F1-Feld um den rosa Kern schwarz, bei K5e_f1 geht es in die Beckenhaut ueber.

### 2b Szene Bild 1 im Spiel

Aufbau: 5080 Schnitt 1, Pose -20711,-15984,756,0,11,1, RE15_FRAMEDUMP Bild 440, eigene Laeufe ueber form_spiel.sh mit eigener exe-Kopie.

Klassenmasken kommen aus Farbschluessel-TIMs, die ich selbst gebaut habe (py/pkey.py):

- Kern rot, Rand gruen, Feldhaut blau, nur Zeile 8 und nur im F1-Feld. Keine andere Flaeche liest das Feld (0 Treffer).
- Figur magenta.

| Wert | Ergebnis |
|---|---|
| Figur | 35 520 px |
| F1 | 2324 px (Kern 662, Rand 345, Feldhaut 1317) |
| Pixel Asset gegen K5e | 1526 geaendert, 0 ausserhalb F1; Kernpixel gleich |
| K5e, Nachbar <= 3 px (n = 782, L* 7,49) | Ring -2,83 / Feldhaut -5,93 / Rand+Feldhaut -5,28 |
| K5e_f1, Nachbar <= 3 px | Ring **+0,28 GRUEN** / Feldhaut -1,61 / Rand+Feldhaut -1,22 |
| K5e / K5e_f1, Nachbar <= 6 px | Ring -2,90 / +0,20 |

## 3 Gegen K5c (einbau_k5c = Asset von git ed31d11d8, 1ab56076 / 7187ff09)

| Pruefung | Ist (p13.txt) |
|---|---|
| Sektionen 0-6 (EDD Clips, EMR Knochen) | Sektionen 0-6 alle gleich: 12 / 3312 / 120636 / 4 / 4 / 4 / 4 Byte. Offsets gleich. |
| MD1 | Laenge 169 500 gleich, Kopf (113332, 0, 64) gleich. Die 4850 Differenzbytes sind 496 Eckbytes und 4354 CLUT-Wortbytes, sonst 0. |
| Ecken | Nur die Meshes 19, 20, 28 und 29 sind anders, je 26 von 26 Ecken. Die Normalen sind gleich. |
| TIM-Pixel | gleich (512 x 256, Kopf (0, 0, 256, 256)) |
| CLUT | 10 Zeilen x 256. Im Port 512 x 2560, also 2560 <= 4096. |
| CLUT-Indizes | Genutzt werden die Zeilen 0-9 (alle < 10), x-Teil 0, Pages 0x80-0x83, u max 436 < 512, v max 254 < 256. |
| Spiel-Log | "[tim] slot 11: 512x2560 8bpp cluts=10" (5080) bzw. "slot 12" (5090) |

## 4 Reproduktion (eigener Lauf, Ausgabe im Scratch)

Befehle:

- kf_k5e.py ruhe/K5e_f1/k5e_f1_par.json
- kf_k5e.py ruhe/K5e/k5e_par.json

Beide Laeufe haben rc 0. Alle 6 Ausgaben sind sha256-gleich der Variante:

| Datei | K5e_f1 | K5e |
|---|---|---|
| FORM.EMD | 0a4894f2 | 0a4894f2 |
| FORM.TIM | 31a35e66 | 0a700ba5 |
| form.json | 110bd2f1 | 45eca32e |
| BAHNSTEIG_FORM.EMD | f277916d | f277916d |
| BAHNSTEIG_FORM.TIM | 31a35e66 | 0a700ba5 |
| bau.json | 7d678218 | fab74ac1 |

## 5 Pins

Selbst mit zlib-CRC32 nachgerechnet:

- MD1: 169 500 Byte, 0x634cd4c3 = form_j_posen.h:10-11.
- TIM: 136 224 Byte, 0x330ea1c5 = form_j_posen.h:12-13.
- K5c zum Vergleich: MD1 0x2456b5cc, TIM 139 296 Byte.

Eigenes git grep in re15_port und release:

- K5c-Werte (2456b5cc, 9234618d, 139296): nur noch test_birkin_bahnsteig.c:116, ein Kommentar ("vorher ...").
- 1ab56076, 7187ff09, f277916d, 31a35e66: 0 Treffer.
- BAHNSTEIG_FORM: Dateinamen in Engine, Tests und Tools. release/make_package.sh:294 steht in einer Dateiliste, ohne Hash.

Gegenprobe selbst gefahren (ctest_gegenprobe_k5c.log): Die K5c-Dateien kamen kurz nach shared_assets/RE2.

- unit_birkin_bahnsteig wird ROT:
  - "FAIL: MD1 weicht von J ab (169500 Byte)"
  - "FAIL: TIM weicht von J ab (139296 Byte)"
  - "83% tests passed, 1 tests failed out of 6"
- unit_birkin_szenen, n2_griff_start, birkin_opfer, r41_form_emd und birkin_formen bleiben gruen. Sie pinnen keine Bytes.
- Danach habe ich die Dateien mit git checkout zurueckgeholt: f277916d / 31a35e66, git status ohne Aenderung an Assets.

## 6 Bau und Tests (selbst)

- local_build.sh configure: "=== LOCAL-BUILD-OK (configure)"
- local_build.sh build: "ninja: no work to do." / "=== LOCAL-BUILD-OK (build)"
- ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3:
  - "100% tests passed, 0 tests failed out of 11"
  - "Total Test time (real) =  95.33 sec"
- ctest -R "n2_griff_start|birkin_szenen|r41_birkin_pistole|birkin_opfer" -j 2 --timeout 1800:
  - "100% tests passed, 0 tests failed out of 4"
  - "Total Test time (real) = 242.02 sec"
  - integration_r41_birkin_pistole 241,97 s

## 7 Eigener Spiellauf ohne Umleitung

Lauf: 5090 Schnitt 11, Pose 941,-23776,3874,12,0,11, RE15_FRAMEDUMP 360-440/20.

- rc 0, "Verfahren: gdigrab (Versuch 1)".
- Log: "[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 12" und "[tim] slot 12: 512x2560 8bpp cluts=10".
- 0 Zeilen FAILED/not loaded, 0 Zeilen fehlt/defekt/error/warn.
- Bild 440 sha 6f9bf5a9d3455518 = framedump_vergleich.txt des Bauers.
- 5080 Bild 1 Asset, Bild 440: b381ae60f728fd62 (ebenfalls gleich).

## Hinweise (nicht rot)

- **H1 Gegenprobe in Bild 1:** In der Szene Bild 1 liegt der reine Ring-Wert von K5e bei -2,83, also knapp ueber der Schwelle -3 und damit nicht rot. Der Bauer misst dort -2,49.
  - Grund: Die Szene ist dunkel (Nachbar L* 7,5), das staucht den Kontrast.
  - Der Defekt zeigt sich dort im ganzen Nicht-Kern-Feld: K5e -5,28 gegen K5e_f1 -1,22.
  - Unter weissem Licht ist K5e eindeutig rot (-11,71 / -8,90).
  - Das Asset ist in allen Ansichten gruen.
- **H2 Feldhaut im reinen Albedo:** Die Feldhaut liegt gegen die Mesh-0-Haut bei -4,52 (3 px: -4,63). Das ist nicht der Ring.
  - K5c, also der gelobte Stand E30, hat dort -4,03.
  - Mit Lambert-Licht sind es -1,69, im Spiel -1,61.
  - Die 16 Feldhaut-Eintraege sind wortgleich K5e-Zeile 3, also dieselben Farben wie die Koerperhaut.
- **H3 Fleischrand:** 12 Fleisch-Randtexel bekommen Hautfarbe; 4 davon (Eintraege 78 und 245) liegen bei L* 60-64. Das ist die dokumentierte Grenze der Regel aus FARBE.md EB.1.
- **H4 Asset-Tausch bei der Gegenprobe:** Die K5c-Dateien lagen etwa 2 s in shared_assets. Parallel hat der Sicht-Pruefer EINBAU_K5E_PRUEFUNG_r1_sicht.md angelegt. Falls er in genau diesem Fenster einen Spiellauf ohne Umleitung gestartet hat, sollte er dessen Log auf "cluts=10" pruefen.
