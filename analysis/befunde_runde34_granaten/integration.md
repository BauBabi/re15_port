# Runde 34 (Granaten) — INTEGRATION

Stand: 2026-09-30, Zweig `r34g/integration`, Baum `.claude/worktrees/r34g_int`, Basis master 7d4d11dd.
Bauverzeichnis: `re15_port/build_r34_int` (nur ueber `RE15_BUILD_DIR=… bash re15_port/tools/local_build.sh`).
Massgeblich: `BAUPLAN.md`, `bau_c0.md`, `bau_a.md`, `bau_b.md`, `bau_c.md`, `bau_d.md` samt Gegenpruefungen und NACHBESSERUNG.
Dieses Dossier wird fortlaufend geschrieben und committet (Sitzungslimit-Schutz).

## 0. Fortschritt (Kurzstand, wird je Schritt nachgezogen)

| Schritt | Stand |
|---|---|
| S1 Merge r34g/b-schaden | erledigt 94c7d41d — konfliktfrei, Bau OK, unit_r34_schaden + unit_r34_reaktion gruen (430 Tests) |
| S1 Merge r34g/a-granate | offen |
| S1 Merge r34g/c-plattform | offen |
| S1 Merge r34g/d-re2fx | offen |
| S2 W1..W11 | offen |
| S3 volle Suite | offen |

## 1. Merges (Schritt 1)

### 1.1 r34g/b-schaden (enthaelt r34g/c0-vertrag) — Merge-Commit 94c7d41d
* `git merge --no-ff r34g/b-schaden`: **keine Konflikte** (27 Dateien, +5102/-181; C0-Vertrag V1/V2b/V3/V5 + RE2-Assets
  `shared_assets/RE2/CORE00.ESP` 8572 B / `TEX.TIM` 132320 B kommen mit).
* Bau: `local_build.sh configure` + `build` im frischen `build_r34_int` -> `LOCAL-BUILD-OK (configure)` / `(build)`.
* Sonden der Spur: `unit_r34_schaden` Passed, `unit_r34_reaktion` Passed (`probe_r34_reaktion alle: 0 Fehler`, Zensus
  352 Laeufe / 0 Haenger, Ausgang K1 220-222 ok). `ctest -N`: 430 Tests.


## 2. Integrationswuensche (Schritt 2)

(W1..W11 je mit Beleg oder begruendet OFFEN)

## 3. Volle Suite (Schritt 3)

## 4. OFFEN
