# Runde 34 (Granaten) — INTEGRATION

Stand: 2026-09-30, Zweig `r34g/integration`, Baum `.claude/worktrees/r34g_int`, Basis master 7d4d11dd.
Bauverzeichnis: `re15_port/build_r34_int` (nur ueber `RE15_BUILD_DIR=… bash re15_port/tools/local_build.sh`).
Massgeblich: `BAUPLAN.md`, `bau_c0.md`, `bau_a.md`, `bau_b.md`, `bau_c.md`, `bau_d.md` samt Gegenpruefungen und NACHBESSERUNG.
Dieses Dossier wird fortlaufend geschrieben und committet (Sitzungslimit-Schutz).

## 0. Fortschritt (Kurzstand, wird je Schritt nachgezogen)

| Schritt | Stand |
|---|---|
| S1 Merge r34g/b-schaden | erledigt 94c7d41d — konfliktfrei, Bau OK, unit_r34_schaden + unit_r34_reaktion gruen (430 Tests) |
| S1 Merge r34g/a-granate | erledigt c4ebd472 — konfliktfrei, Bau OK, unit_r34_wurf + B-Sonden + ESP-/Waffen-Pins gruen (431 Tests) |
| S1 Merge r34g/c-plattform | erledigt 7c202b7a — konfliktfrei, Bau OK, C-Sonden (2 unit + 2 exe) + A/B-Sonden gruen (435 Tests) |
| S1 Merge r34g/d-re2fx | erledigt 3086da7e — konfliktfrei, Bau OK, alle 12 r34-Sonden gruen (440 Tests) |
| S2 W1..W11 | offen |
| S3 volle Suite | offen |

## 1. Merges (Schritt 1)

### 1.1 r34g/b-schaden (enthaelt r34g/c0-vertrag) — Merge-Commit 94c7d41d
* `git merge --no-ff r34g/b-schaden`: **keine Konflikte** (27 Dateien, +5102/-181; C0-Vertrag V1/V2b/V3/V5 + RE2-Assets
  `shared_assets/RE2/CORE00.ESP` 8572 B / `TEX.TIM` 132320 B kommen mit).
* Bau: `local_build.sh configure` + `build` im frischen `build_r34_int` -> `LOCAL-BUILD-OK (configure)` / `(build)`.
* Sonden der Spur: `unit_r34_schaden` Passed, `unit_r34_reaktion` Passed (`probe_r34_reaktion alle: 0 Fehler`, Zensus
  352 Laeufe / 0 Haenger, Ausgang K1 220-222 ok). `ctest -N`: 430 Tests.

### 1.2 r34g/a-granate — Merge-Commit c4ebd472
* `git merge --no-ff r34g/a-granate`: **keine Konflikte** (9 Dateien, +2841/-58: re15_esp.c, re15_esp.h, game_step_common.c,
  player_common.c, menu_common.c, probe_r34_wurf).
* Bau: `configure` (neue Sonde probes/r34_wurf.cmake) + `build` -> `LOCAL-BUILD-OK`.
* Sonden: `unit_r34_wurf` Passed (`probe_r34_wurf: ALLE PRUEFUNGEN GRUEN`; MITTE gesund SE-Folge 0x010a0601..0x010a0001 +
  Liegen, Explosion Bild 109, Zeitlinien HOCH/TIEF/vergiftet wie BAUPLAN 1.1), dazu erneut `unit_r34_schaden`,
  `unit_r34_reaktion` und die ESP-/Waffen-Bestandspins `probe_abzug_takt`, `unit_r17_waffen_loop_pin`, `unit_r26_mg_blut`,
  `unit_r30_granate`, `r30b_muendungshoehe`, `unit_espr_11e0`, `unit_aim*` — 11/11 gruen. `ctest -N`: 431.
### 1.3 r34g/c-plattform — Merge-Commit 7c202b7a
* `git merge --no-ff r34g/c-plattform`: **keine Konflikte** (main.c, audio_pc.c, render_pc.c, room_pc.c, neue
  fx_plattform_pc.c/.h, Sonden probe_r34_plattform*, Werkzeuge bau_c_werkzeug/ + bau_c_gegen_werkzeug/).
* Bau: `configure` + `build` -> `LOCAL-BUILD-OK`. Die drei Warnungen `-Wdiscarded-qualifiers` (main.c:8510/9769/10133)
  sind Altbestand (Gore-Pfad), nicht durch den Merge entstanden.
* Sonden: `unit_r34_plattform`, `unit_r34_plattform_ton`, `integration_r34_plattform_takt` (23.0 s),
  `integration_r34_plattform_esp_eintritt` (9.7 s), dazu `unit_r34_schaden`, `unit_r34_reaktion`, `unit_r34_wurf` —
  7/7 gruen. `ctest -N`: 435.
### 1.4 r34g/d-re2fx — Merge-Commit 3086da7e
* `git merge --no-ff r34g/d-re2fx`: **keine Konflikte** (re2_fx.c, re2_fx.h, re2fx_pc.c/.h, fuenf Sonden, zwei
  Werkzeuge unter `tools/`).
* Bau: `configure` + `build` -> `LOCAL-BUILD-OK`; keine neuen Warnungen.
* Sonden: `unit_r34_re2fx`, `_knochen`, `_bild` (1.1 s), `_raum`, `_pc` + alle frueheren r34-Sonden (A/B/C inkl. der
  zwei exe-Pins) — **12/12 gruen**. `ctest -N`: **440** (= 428 + B 2 + A 1 + C 4 + D 5).
* Befund zum Merge insgesamt: die Spuren hielten den Dateibesitz aus BAUPLAN §3.0 ein; git fand in keinem der vier
  Merges eine Ueberschneidung. Kein Verhalten ging verloren (keine Konfliktaufloesung noetig).

## 2. Integrationswuensche (Schritt 2)

(W1..W11 je mit Beleg oder begruendet OFFEN)

## 3. Volle Suite (Schritt 3)

## 4. OFFEN
