# Integration Runde 41 - Lauf D Riesenform K5e: unabhaengige Gegenpruefung

Stand 2026-11-09. Baum `.claude/worktrees/r41_integration`, Zweig r41/integration, geprueft auf Kopf 37f7a8ecb
(e181dfac7 -> Merge db52dd8bf -> Fixes 48263fc5f, c3a6b2057 -> Bericht 37f7a8ecb INT_laufd_riesenform.md).
Nur gelesen und Tests gefahren; einzige Aenderung ist dieser Bericht. Logs: Scratchpad `laufd/gp/`.

Ergebnis: **bestanden, kein Befund.**

## 1. Merge-Inhalt

- Merge db52dd8bf, Eltern e181dfac7 + 2b9a57ec1, Basis 947637dae. r41/form seit der Basis: 13 Commits (f52d7a325 .. 2b9a57ec1), 173 Dateien.
- Jede der 173 Dateien verglichen (Blob in 2b9a57ec1 gegen Blob in db52dd8bf): 171 identisch. Abweichend nur zwei Dateien, die auch
  die Integrationsseite seit der Basis geaendert hatte:
  - `analysis/befunde_runde41/form/NUTZER_ENTSCHEIDE.md` (Integration: E23 und E29 eingefuegt, +27 Zeilen)
  - `re15_port/tests/unit/test_birkin_bahnsteig.c` (Integration: G5-Vorlauf M7, M3-Vorbedingung, M4 Form-1-HP 199)
  Fuer beide sind die Hunks `git diff 947637dae 2b9a57ec1` und `git diff e181dfac7 db52dd8bf` zeilengleich: die r41/form-Aenderung
  ist vollstaendig drin, die Integrationsseite ist erhalten (beide Seiten erhalten, ohne Konflikt).
- `git diff --name-only e181dfac7 HEAD` ohne die 173 r41/form-Dateien: nur `INT_laufd_riesenform.md`,
  `tests/integration/test_r41_form_wagenweg.cmake` (Fix c3a6b2057) und `tests/unit/test_r41_uv_textur.c` (Fix 48263fc5f).
  Der Merge selbst bringt keine Datei ausserhalb von r41/form. Unter engine/, platform/, include/, tools/, CMakeLists.txt aendert
  e181dfac7..HEAD nichts.
- Integrationsseite unversehrt:
  - Labor-Fix 145316fc9: `git diff 145316fc9 HEAD -- engine/src/enemy_ai_common.c`, `tests/unit/CMakeLists.txt`,
    `test_r41_labor_birkin_angriff.cmake` leer.
  - Sichel V7: `shared_assets/PSX/EMD/EM30_HANGAR.EMD` sha256 319bbd17f10ba0cd... = `release/em30_hangar.sha256`, seit Merge 91636662f
    unveraendert; unit_birkin_formen und unit_schwanz_r41 in ctest_a2 gruen.
  - Sherry-Fix: `engine/src/enemy_common.c` gleich 2c1c0eb1a (Vorfahr von HEAD).
  - `platform/pc/src/render_pc.c` gleich 4dde57452; Block "SDLs CPU-RASTERER: 32-BIT-UEBERLAUF" genau 1x, jede der Funktionen
    re15_sw_s2_produkt / re15_swtri_messen / re15_sw_teil_zeichnen / re15_sw_teilen / re15_sw_geometrie genau 1x definiert.
  - `release/make_package.sh` e181dfac7..HEAD unveraendert: Tor GATOR40A0.RBJ (b27d96891), BAHNSTEIG_FORM.EMD/.TIM in der
    RE2-Liste, EM30_HANGAR.EMD mit Pin-Vergleich gegen `release/em30_hangar.sha256`.

## 2. Asset und Pins (selbst nachgerechnet)

- sha256 im Kopf: BAHNSTEIG_FORM.EMD f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1,
  BAHNSTEIG_FORM.TIM 31a35e66428bac1e8783587f2336ea0bec7aab6e34876b0be87a144c576445b8; Git-Blobs gleich 2b9a57ec1
  (ac5e6921 / 8a2dee79), Arbeitsbaum sauber.
- Nachgerechnet wie der Test (rd32(emd) = Verzeichnis 0x47a6c, Eintrag 7 = MD1 @0x1e450, Laenge Verzeichnis - MD1):
  MD1_LEN 169500, MD1_CRC32 0x634cd4c3; TIM 136224 Byte, CRC32 0x330ea1c5. Gleich `form_j_posen.h`
  (FORM_J_MD1_LEN 169500u, FORM_J_MD1_CRC 0x634cd4c3u, FORM_J_TIM_LEN 136224u, FORM_J_TIM_CRC 0x330ea1c5u).
  Die Posen-Felder in form_j_posen.h sind gegen die Basis unveraendert (Diff nur Kopfkommentar und die drei Defines).
- TIM-Kopf: @0x00 0x10, @0x04 0x09 (8 bpp mit CLUT), CLUT-Block @0x08 0x140c, x/y/w/h (0,480,256,10) -> 10 Zeilen;
  Bildblock 0x2000c, 256 Worte x 256 Zeilen = 512x256 Pixel; Port 512 x (256 x 10) = 512x2560. Das deckt Fix 48263fc5f.

## 3. Testsummen

Gegen die Logs des Integrators (`laufd/`):
- ctest_a.log: `99% tests passed, 1 tests failed out of 1100` (rot unit_r41_uv_textur) - stimmt.
- ctest_a2.log (06:51, nach c3a6b2057 06:44): `100% tests passed, 0 tests failed out of 1100` - stimmt.
- ctest_b.log: `99% tests passed, 1 tests failed out of 115` (rot integration_r41_form_wagenweg); darin
  integration_r41_labor_birkin_angriff, unit_birkin_bahnsteig, unit_r41_form_emd gruen - stimmt.
- wagenweg_nach1-3.log rot je `F1635 Cut 12: Form sichtbar 742 < 751`; wagenweg_fix1-3.log gruen je min sichtbar 742.
- Ursachen-Isolation geprueft: `laufd/k5c/` = Blobs aus e181dfac7 (EMD 1ab56076..., TIM 7187ff09...) -> 751 gruen;
  `laufd/k5e_emd_k5c_tim/` = EMD f277916d + TIM 7187ff09 -> 742 rot. Die Zahl haengt am EMD, wie berichtet.
  Die neue Schranke 742 ist ein gemessener Port-Wert (kein RE-Wert), so im Riegelkopf und im Commit benannt.

Selbst gefahren (nach `local_build.sh build` = LOCAL-BUILD-OK, ohne Parallelbau):
- `ctest -R "^(unit_birkin_bahnsteig|unit_r41_form_emd|unit_r41_uv_textur)$"`: `100% tests passed, 0 tests failed out of 3`
- `test_r41_uv_textur gegenprobe`: rc 1, `GEGENPROBE ROT (erwartet)`; Normalzeile `Textur 512x2560 (TIM-Hoehe 256 x 10 CLUT-Zeilen)`.
- `ctest -R "^integration_r41_labor_birkin_angriff$"`: `100% tests passed, 0 tests failed out of 1` (221 s; leon 50E0 Biss F1075,
  elza 50F1 Biss F1014).
- `ctest -R "^integration_r41_form_wagenweg$"` (1x zusaetzlich): `100% tests passed, 0 tests failed out of 1`, min sichtbar 742.

## Hinweise (kein Befund)

- Die Schranke 6a leon steht jetzt genau auf dem Messwert 742 (kein Abstand). Der Lauf ist bildgleich (3x Integrator, 1x hier),
  aendert sich die Geometrie, schlaegt der Riegel an - so gewollt.
- `make_package.sh` prueft BAHNSTEIG_FORM.EMD/.TIM nur auf Vorhandensein, nicht gegen einen sha256-Pin (anders als EM30_HANGAR).
  Den Inhalt sichern die Pins in form_j_posen.h (unit_birkin_bahnsteig). Stand von vor diesem Merge, nicht Teil von Lauf D.
