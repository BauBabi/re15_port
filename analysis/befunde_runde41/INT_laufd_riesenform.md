# Integration Runde 41 - Lauf D: Riesenform K5e (r41/form, Einbau E32)

Stand 2026-11-09. Baum `.claude/worktrees/r41_integration`, Zweig r41/integration.

## Merge

- Kopf vorher: e181dfac7 (master b27d96891 + Sherry-Fix + Lauf C Sichel V7 + Labor-Birkin-Fix 145316fc9 mit Bericht/Pruefung).
- Gemergt: r41/form Kopf 2b9a57ec1 (13 Commits seit Lauf A2, Basis 947637dae).
- Merge-Commit: db52dd8bf "merge(r41): Riesenform K5e (r41/form, Einbau E32 bestanden)", **konfliktfrei**.
- 173 Dateien, davon in re15_port nur vier: BAHNSTEIG_FORM.EMD (293516 Byte), BAHNSTEIG_FORM.TIM (139296 -> 136224 Byte),
  tests/unit/form_j_posen.h, tests/unit/test_birkin_bahnsteig.c (Kommentar). Der Rest ist Analyse unter analysis/befunde_runde41/form/.
- Nach dem Merge gemessen:
  - sha256 BAHNSTEIG_FORM.EMD f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1
  - sha256 BAHNSTEIG_FORM.TIM 31a35e66428bac1e8783587f2336ea0bec7aab6e34876b0be87a144c576445b8
  - form_j_posen.h: FORM_J_MD1_CRC 0x634cd4c3u, FORM_J_TIM_LEN 136224u, FORM_J_TIM_CRC 0x330ea1c5u
  - `git diff r41/form HEAD` auf Asset und form_j_posen.h: leer (beide Dateien von r41/form).

## Bau

`RE15_BUILD_DIR=<baum>/re15_port/build bash re15_port/tools/local_build.sh configure|build`:
`=== LOCAL-BUILD-OK (configure)`, `=== LOCAL-BUILD-OK (build)`. Nach jedem Fix erneut `=== LOCAL-BUILD-OK (build)`.

## Fixes (Querbezug Integration x r41/form)

Beide Riegel lebten nur in der Integration (aus master bzw. Integrationsarbeit) und pinnten Werte, die an K5c gemessen waren.
r41/form kannte sie nicht und hat sie darum nicht mitgezogen.

### 48263fc5f unit_r41_uv_textur

Rot nach dem Merge: `FAIL: BAHNSTEIG_FORM: Textur 512x2560 10 Zeilen statt K5c 512x4096 16 Zeilen`.
Beleg aus den TIM-Bytes des neuen Assets: @0x04 = 0x09 (8 bpp mit CLUT), @0x08 CLUT-Block 0x140c = 12 + 256 x 10 x 2,
@0x10 CLUT-Breite 0x0100, @0x12 CLUT-Hoehe 0x000a -> 10 Zeilen, Port-Textur 512 x (256 x 10) = 512x2560.
Pin auf 512x2560 / 10 Zeilen umgestellt; UV-Pruefung unveraendert (Texel-x 44..436, v 0..254, ausserhalb 0).
Nachgefahren: 2x gruen; eingebaute Gegenprobe "rot wie verlangt"; `test_r41_uv_textur gegenprobe` -> "GEGENPROBE ROT (erwartet)".

### c3a6b2057 integration_r41_form_wagenweg (Teil leon)

Rot in Suite (b) und in drei Einzellaeufen bildgleich: `F1635 Cut 12: Form sichtbar 742 < 751`.
Die Schranke 751 ist laut Kopf des Riegels der an K5c gemessene Wert (Stand bf5001400), kein RE-Wert.
Ursache per RE15_RE2_ASSET_ROOT isoliert (Riegel unveraendert, ctest -V):

| Asset                       | min sichtbar | Form max verdeckt | max fast schwarz | Ergebnis |
|-----------------------------|-------------:|------------------:|-----------------:|----------|
| K5c-Paar (aus e181dfac7)    | 751          | 639               | 600              | gruen    |
| K5e-EMD + K5c-TIM           | 742          | 631               | 600              | rot      |
| K5e_f1-Paar (Spiel-Asset)   | 742          | 631               | 448              | rot      |

Die Sichtbarkeit haengt am EMD (Geometrie K5d/K5e, Krallen ganz eingezogen), nicht an der Textur; render_pc.c
re15_slotdeckung zaehlt nur deckende Dreiecke je Slot. Schranke 6a fuer leon auf den gemessenen K5e-Wert 742 gesetzt,
Kopfkommentar ergaenzt; 6c leon (631 <= 639) und alle anderen Teile unveraendert (in Suite (b) gruen).
Nachgefahren: 3 Einzellaeufe gruen, je min sichtbar 742.

## Tests

(a) `ctest -j 6 --timeout 600 -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake|unit_r39_titelversion_ablauf$)"`

- direkt nach dem Merge: `99% tests passed, 1 tests failed out of 1100` (rot: unit_r41_uv_textur, s. Fix 48263fc5f)
- nach beiden Fixes (Kopf c3a6b2057): `100% tests passed, 0 tests failed out of 1100` (296 s)

(b) `ctest -j 2 --timeout 1800 -R "form|bahnsteig|birkin|labor|sichel|r41_birkin|g5_tod|g5_westen|uv_textur|npcglieder|annette|6030|r41_gatorfix|r41_re2gator|waffenschaden"`

- Lauf nach Fix 48263fc5f: `99% tests passed, 1 tests failed out of 115` (5918 s; rot: integration_r41_form_wagenweg,
  s. Fix c3a6b2057, danach 3x einzeln gruen). Alle uebrigen 114 gruen, darunter unit_r41_uv_textur.

(c) Software-Renderer, eigener Lauf 5090 Schnitt 11 mit Riesenform (re15_port/tools/form_spiel.sh, eigene exe-Kopie
re15_pc_form_54825_064505, Pose 941,-23776,3874,12,0,11, RE15_SOFTWARE_RENDER=1):
`[render] SDL-Renderer software ...`, `[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11`,
`t601 slot2 typ 0x30 st 1/9 pos (941,0,-23776) gier 3874 clip 12 bild 0`, Fensterbild per gdigrab (Versuch 1), Spiel lief
bis F450+ und wurde erst vom Skript (eigene PID) beendet, rc=0, keine Absturz-/Ausnahmezeile. unit_r41_uv_textur gruen.

(d) Debug-Log echter Renderer (direct3d), RE15_TIM_LOG=1:

| Raum | Pose                          | Form-Zeile                                                   | TIM-Zeile Slot 11                                  |
|------|-------------------------------|--------------------------------------------------------------|----------------------------------------------------|
| 5080 | -20711,-15984,756,0,11,1      | [enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11 | [tim] slot 11: 512x2560 8bpp cluts=10 - 0.0% transparent, 8.2% opak-schwarz |
| 5090 | 941,-23776,3874,12,0,11       | [enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11 | [tim] slot 11: 512x2560 8bpp cluts=10 - 0.0% transparent, 8.2% opak-schwarz |

Keine [tim]-Fehlzeile (alle [tim]-Zeilen sind slot-/loaded-Zeilen), kein "not loaded", "WARNUNG" oder "unbrauchbar".

## Ergebnis

Gruen. Kopf nach Lauf D (vor diesem Bericht): c3a6b2057. Kein push, kein master-Merge.
Logs: Scratchpad laufd/ (configure.log, build.log, ctest_a.log, ctest_a2.log, ctest_b.log, wagenweg_*.log,
softrender_lauf.log, lauf_r5080.log, lauf_r5090.log, debug_*.log, Bilder soft_r5090_s11.png, lauf_r5080.png, lauf_r5090.png).
