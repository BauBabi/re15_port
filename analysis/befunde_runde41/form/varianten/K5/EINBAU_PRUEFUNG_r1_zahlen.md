# Einbau v0.8.30 (E30) — Zahlen-Pruefung Runde 1

Pruefer: Zahlen. Stand: Zweig `r41/form`, HEAD `ce2c3a32d`, Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form`
(Baum sauber bis auf das fremde, unversionierte `varianten/K5/einbau/sicht/` des Sicht-Pruefers und meinen unversionierten
Belegordner `varianten/K5/einbau/pruefer_zahlen_r1/`). Gegenstand: Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`
= Riesenform K5c V3 Runde 5 auf 16 CLUT-Zeilen (Nutzerentscheid E30, 2026-11-07 ~22:20, Frage 1 -> A, Frage 2 -> Leuchtstelle bleibt).

Alle Zahlen stammen aus EIGENEM Code (`pruefer_zahlen_r1/pruef_einbau.py`, `pruef_p2.py`: eigener TIM-/EMD-/MD1-/EMR-/EDD-Leser,
eigener Software-Renderer), nicht aus `form_emd.py` und nicht aus den Riegeln des Bauers. Formatquellen im Kopf der Skripte
(tim_common.c:40-80, re2_ems.c:93-170, md1_common.c, emd_common.c:136-200, render_pc.c:2560-2569 und 2773-2775, main.c:2288).

**Ergebnis: BESTANDEN — kein roter Punkt.** Drei Info-Punkte (I1-I3) unten, keiner ist ein Mangel des Assets.

## Dateien und Hashes (sha256, selbst gerechnet)

| Datei | sha256 | Byte |
|---|---|---|
| `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (Arbeitsdatei = HEAD-Blob, `git cat-file -p`) | `1ab56076b20ef022c45a4517d944c0829ff93120124b6c7bb3601c7373736041` | 293 516 |
| `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.TIM` (Arbeitsdatei = HEAD-Blob) | `7187ff097f42427dc40fc77264f43cc5860f6b28de70fb1549c626ef92af75e3` | 139 296 |
| `analysis/.../stufe3/asset/BAHNSTEIG_FORM.EMD` / `.TIM` | identisch 1ab56076... / 7187ff09... | |
| `analysis/.../stufe3/basis_k5c/FORM.EMD` / `FORM.TIM` | `a2e87fb84e73c7d7bd173ea11d56482df731175c1e12af26cebd48063eb09adf` / 7187ff09... (= Asset-TIM) | 170 424 / 139 296 |
| `analysis/.../varianten/K5/farbe/K5c_farbe_v3/FORM.EMD` / `FORM.TIM` (17 Zeilen, = `K5C_FARBE_V3.EMD/.TIM`) | `090e0b80e214ae455788a383da6dd8f1b44ec6b26f771b281780a24953c5cfb0` / `512828067ea7f0480763110b4820ae311bc5cd7423796d58274b07ef776e7e6d` | 170 424 / 143 648 |
| `analysis/.../varianten/K4/BAHNSTEIG_FORM.EMD` / `.TIM` (K4-Sicherung, wie K4.md) | `16eb6ea89a40df179ef08cb538b12d7d038183f01b88b47baf05c97ebf8d8bd0` / `ec280802dd785e9238535ba90830f7e6b8a690cc4aa3609fc91f1638b3df2072` | 291 816 / 362 528 |

## (1) Asset-TIM: Zeilen und UV-CLUT-Indizes — GRUEN

- TIM-Kopf (eigener Leser nach tim_common.c): 8 bpp, CLUT-Block x 0 / y 480 / w 256 / h **16** = 4096 Eintraege; Pixelblock 512 x 256.
  Port-Formel render_pc.c:2560-2569: `n_cluts = 4096 / 256 = 16`, `tex_h = 256 * 16 = 4096 <= 4096` -> GRUEN.
  Gegenprobe Variante (17 Zeilen): `tex_h = 4352 > 4096` -> waere ROT (deckt sich mit dem Riegel des Bauers).
  render_pc.c:2636-2650: `SDL_CreateTexture` ohne Fehlerzweig/Log (gelesen: kein else, kein fprintf) — die Grenze ist also wirklich stumm.
  Belege der 4096er Grenze im Port: re2fx_pc.c:35-45 (Kommentar N5), fx_plattform_pc.h:106-110 (16 Paletten -> 256 x 4096), gator_fx_r41_pc.c:79 (`G41FX_ATLAS_MAX_H 4096`).
- 32 Meshes, **3828 Flaechen** (Dreiecke + Vierecke, alle Meshes). Zeile je Flaeche = `((clut >> 6) & 0x1ff) - 480` (render_pc.c:2773-2775);
  CLUT-x `(clut & 0x3f) * 16 == 0` bei allen 3828. **0 ungueltige Zeilen**, alle in 0..15:
  `0:155 1:252 2:770 3:696 4:62 5:6 6:25 7:114 8:45 9:187 10:152 11:204 12:131 13:229 14:232 15:568` (Summe 3828).
  Meshes je Zeile: 0:[22,23] 1:[14,15,19,20,28-31] 2:[1-6] 3:[8,9,24-27] 4:[0,7] 5:[0] 6:[22,23] 7:[22,23] 8:[0,7] 9:[0,7] 10:[0,7] 11:[16,17,18] 12:[10] 13:[10,21] 14:[22,23] 15:[11,12,13,16,17].
- Variante (17 Zeilen), gleiche Zaehlung: `0:155 1:252 2:0 3:770 4:696 5:62 6:6 7:25 8:114 9:45 10:187 11:152 12:204 13:131 14:229 15:232 16:568` — Zeile 2 wird von **keiner** Flaeche gelesen. Die Zahlen des Bauers sind damit unabhaengig bestaetigt.

## (2) Bytegleichheit Asset gegen 17-Zeilen-Variante — GRUEN

RE2-EMD-Layout (re2_ems.c:93-107): `dir_off` = u32 @0 = Dateiende - 32 (Asset 293 484, Variante 170 392), 8 Eintraege; MD1 = dir[7] bis dir_off.

- **MD1** (169 500 Byte in Asset, Basis und Variante): Asset **bytegleich basis_k5c**. Asset gegen Variante: 5003 Byte verschieden,
  **0 davon ausserhalb der clut-Felder** (Tri-UV +2, Quad-UV +2); 3421 von 3828 clut-Worten geaendert (die Flaechen der Zeilen 3..16),
  407 unveraendert (Zeilen 0/1). Geometrie, Normalen, UV, TPage: bytegleich.
- **EMR-Struktur** (dir[2] ab +4 bis kf_off 360: Knochenzahl 32, kf_size 156, rel-Positionen, Knochentabelle + Kinderlisten):
  Asset bytegleich Variante und Basis. Gegen K4: nur Knochen 23 rel-Pos (784,420,0) statt (500,400,0), Knochentabelle gleich (wie vom Bauer angegeben).
- **Clips**: Asset 28 Clips, 799 Bilder (`18,1,60,60,50,89,1,1,30,63,126,57,56,40,1x12,35,100`), 771 Keyframes, Pool-Rest 0, hoechster Bild-Index 770 < 771 (alle gueltig).
  Clip-Laengen identisch K4; EDD gegen K4: alle Flag-Bits (>>12, Treffer/Ton) je Bild identisch, nur 286 Keyframe-Indizes anders (andere Posen).
  Die Variante traegt 3 Posen-Clips (1 Bild je Clip), kein 28-Clip-Satz — die Abweichung „32/32/28 statt 32/32/3" des Bauers ist korrekt:
  test_birkin_bahnsteig.c:112 verlangt 28 Clips, test_r41_birkin_pistole.cmake:70 die Logzeile „28 Clips".
  Eigene Posen-Dekodierung (12-Bit-Winkel ab Byte 12, 36 Bit je Knochen; Wurzel Byte 0..5):
  - Variante Clip 0 (Bild 1) == Asset Clip 0 Bild 11: Wurzel (-9,-1887,-22) und alle 32 Knochen gleich; einziger Byte-Unterschied Geschwindigkeitsvektor Byte 6..11 (0,0,0) -> (3959,0,0) (Lauf-Clip).
  - Variante Clip 1 (Bild 2) == Asset Clip 12 Bild 0 **bytegleich** (auch Clip 10 Bild 45).
  - Variante Clip 2 gegen Asset Ruhe (Clip 2 Bild 0): Wurzel gleich (0,-1828,0); Knochen 22 (0,0,150) -> (0,0,91), Knochen 23 (0,0,60) -> (0,0,0); alle anderen gleich. Das ist die dokumentierte „Ruhe von J mit Kiefer zu" (form_j_posen.h Zeile 22/23) — siehe I1.
- **TIM**: Pixel (131 072 Texel) bytegleich, Masse gleich, CLUT x/y gleich. Abbildung Variantenzeile -> Assetzeile eindeutig:
  `0->0, 1->1, 3->2, 4->3, ..., 16->15`; Variantenzeile 2 ohne Ziel (0 Flaechen). **Keine Zusammenlegung**: jede Assetzeile ist ihrer
  Quellzeile **bytegleich, max dE76 0,0 und 0 Eintraege ungleich** (alle 16 Paare, `pruefer_zahlen_r1/zeilen_de.json`). Zur Einordnung: die
  entfernte Zeile 2 haette zu jeder anderen Zeile max dE76 >= 45,3 (gegen Zeile 7) — eine Zusammenlegung waere sichtbar gewesen, war aber nicht noetig.
- **Render-Diff** (eigener orthographischer Renderer, Bind-Pose aus EMR-rel-Positionen, Texelstapel wie render_pc.c:2572-2578, U-Basis `(page & 0xF) * 128` nach main.c:2288, Zeilenwahl wie render_pc.c:2773-2775, nearest Texel, z-Puffer, 640x480):
  - S1 Seite: 94 406 belegte Pixel, **Pixel-Diff 0, max dE76 0,0**.
  - S2 schraeg oben: 80 142 belegte Pixel, **Pixel-Diff 0, max dE76 0,0**.
  - Empfindlichkeit (Asset mit vertauschten Zeilen 0<->5): S1 1309 px / dE76 74,2; S2 2527 px / dE76 87,1 -> der Diff sieht Zeilenfehler.
  Tafeln (roh, Variante | Asset | Gegenprobe): `pruefer_zahlen_r1/render_S1_seite_tafel.png`, `render_S2_schraeg_oben_tafel.png`.

## (3) Pins, Hash-Fundstellen, Paket-Tor, K4-Reste — GRUEN

- `re15_port/tests/unit/form_j_posen.h`: `FORM_J_MD1_LEN 169500`, `FORM_J_MD1_CRC 0x2456b5cc`, `FORM_J_TIM_LEN 139296`, `FORM_J_TIM_CRC 0x9234618d`
  — selbst nachgerechnet (zlib crc32 ueber dir[7]..dir_off bzw. ganze TIM, gleiches Polynom wie `crc32_lauf` test_birkin_bahnsteig.c:76-84): **0x2456b5cc / 0x9234618d, gleich**.
  Posen RUHE/BILD1/BILD2 des Headers == Asset Clip 2 Bild 0 / Clip 0 Bild 11 / Clip 12 Bild 0 (Wurzel + 32 Winkel mod 4096): alle drei gleich.
- `git grep` ueber den Baum (ohne png): die neuen Hashes 1ab56076.../7187ff09... stehen vollstaendig in `FARBE.md:1164-1165` (und 7187ff09... in :1132);
  die alten 16eb6ea8.../ec280802... nur in Berichten (`STUFE3_abnahme_2.md`, `abn2/pins.txt`, `K4.md`, `FARBE_PRUEFUNG_*`), die alten Pins 167644/0xc0b6c82a/362528/0x76775364 nur in `STUFE3_abnahme_2.md`, `abn2/pins.txt`, `FARBE.md:1171`.
  `release/SHA256SUMS.txt`, `SHA256SUMS_android.txt`, `*.sha256`, `pri_hashes.py`: kein BAHNSTEIG-Eintrag (git grep ohne Treffer).
- `release/make_package.sh:294`: Tor fuehrt `BAHNSTEIG_FORM.EMD BAHNSTEIG_FORM.TIM` (vorhanden, nicht leer, `cmp` gegen Quellbaum) — kein Hash-Pin, nichts nachzuziehen.
- `re15_port/shared_assets/RE2/`: nur `BAHNSTEIG_FORM.EMD` und `.TIM`, keine G4-/K4-Dateien. `stufe3/asset` = Spiel-Asset. K4 unter `varianten/K4/` mit den in K4.md genannten Hashes (nachgerechnet).
- Engine-Code seit 67408cfcb (git diff --stat auf re15_port): nur `birkin_bahnsteig.c`, `re15_birkin_formen.h`, Tests, `form_j_posen.h` und die beiden Assets — in den Einbau-Commits f813205ca..ce2c3a32d kein Engine-Code (Bauer-Angabe stimmt; re15_pc.exe wurde beim Bau nicht neu gelinkt).

## (4) Bau und Tests selbst — GRUEN

- `RE15_BUILD_DIR=.../r41_form/re15_port/build bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `... build` -> `[1/2] test_birkin_bahnsteig.c.obj`, `[2/2] test_birkin_bahnsteig.exe`, `=== LOCAL-BUILD-OK (build)` (`pruefer_zahlen_r1/build_pruefer.log`).
- `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3`: **100% tests passed, 0 tests failed out of 11**, Total Test time (real) = 74.87 sec (`ctest_form_pruefer.log`).
- `ctest -R "n2_griff_start|birkin_szenen|r41_birkin_pistole|birkin_opfer" -j 3`: **100% tests passed, 0 tests failed out of 4**, Total 242.12 sec (`ctest_asset_pruefer.log`).

## (5) Spiellauf — GRUEN

- Bauer-Logs gelesen (`einbau/spiel_lauf.log`, Laufverzeichnisse `build/form_spiel/2268_215621, 2393_215702, 2545_215733, 2648_215758`): 4 x rc 0, gdigrab,
  `[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11/12`, Lagezeilen `typ 0x30 ... clip 0 bild 11 / clip 2 bild 0 / clip 12 bild 0 ... kein_bild 0`;
  stderr nur `[lautstaerke] re15_options.cfg fehlt`; keine FAILED-/not-loaded-/WARNUNG-/fehlt-/defekt-Zeile. Die Zeile `[tim] slot 11: 512x4096 8bpp cluts=16 — 0.0% transparent, 22.6% opak-schwarz`
  steht in den Laeufen `2810_215844` und `5271_221133` (RE15_TIM_LOG=1).
- Eigener Lauf (`bash re15_port/tools/form_spiel.sh --raum 5080 --pose -20711,-15984,756,0,11,1 --env RE15_TIM_LOG=1`, OHNE --emd/--tim, Laufverzeichnis `build/form_spiel/7028_221551`, `re2/` leer = keine RE15_RE2_ASSET_ROOT-Umleitung,
  eigene exe-Kopie, nur eigene PID, danach kein Prozess mehr): rc 0, `Verfahren: gdigrab (Versuch 1)`,
  `[tim] slot 11: 512x4096 8bpp cluts=16 — 0.0% transparent, 22.6% opak-schwarz`, `[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11`,
  `t601 slot1 typ 0x30 st 1/9 pos (-20711,0,-15984) gier 756 clip 0 bild 11 grid 0x33 kein_bild 0`. Bild `pruefer_zahlen_r1/pruefer_spiel_5080_bild1.png` (640x480, roh):
  **pixelgleich** mit dem Bauer-Bild `einbau/spiel/einbau_pose1_nachstellung_r5080.png` (0 von 307 200 Pixeln verschieden) — Form texturiert, dunkel violett/braunrot wie E30 A.

## Info (kein roter Punkt)

- **I1 Ruhepose**: die dritte Pose der Variante (Kopf 22 z 150, Kiefer 23 z 60) ist nicht im Asset; die Ruhe im Spiel ist „Ruhe von J mit Kiefer zu" (22 z 91, 23 z 0), wie in form_j_posen.h und vom Bauer angegeben („Ruhe unveraendert"). Bild 1/Bild 2 der Variante sind 1:1 im Asset.
- **I2 Bauer-Riegel E-Z3 Szene 07 ROT** (`riegel_einbau_spiel07.json`: 5080 2 px / dE76 2,71; 5090 nicht messbar): vergleicht die 17-Zeilen-Variante ueber Asset-Root-Umleitung mit der 16-Zeilen-Basis im Spiel. Betrifft nicht das Asset (die 17-Zeilen-Datei kommt nicht ins Spiel); Ursache vom Bauer nicht nachgewiesen, offline (eigener Renderer, beide Dateien) ist der Diff 0.
- **I3 Stolperdraht**: `22.6% opak-schwarz, 0.0% transparent` liegt unter der WARNUNG-Schwelle von 25 % (render_pc.c:2615 `n_black * 4 >= n_pixels`); keine Warnung, aber wenig Luft. Keine Aenderung noetig.

## Belege (unversioniert)

`C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/einbau/pruefer_zahlen_r1/`:
`pruef_einbau.py`, `pruef_p2.py`, `pruef_einbau.json`, `pruef_p2.json`, `zeilen_de.json`, `render_S1_seite_{asset,variante,tafel}.png`, `render_S2_schraeg_oben_{asset,variante,tafel}.png`,
`pruefer_spiel_5080_bild1.png`, `pruefer_spiel_5080.log`, `build_pruefer.log`, `ctest_form_pruefer.log`, `ctest_asset_pruefer.log`.
