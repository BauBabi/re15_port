# Gegenpruefung Runde 3 — Linse ECHTER LAUF / INTEGRATION (Android-Asset-Pruefung)

Pruefer: Gegenpruefer R3 (Echtlauf/Integration). Baum: `C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android`, Zweig `r34a/android-gate`.
Auftrag: den vollen Android-Bau mit den neuen Gates echt laufen lassen, die gebaute APK gegen die Referenz-APK v0.8.19
vergleichen, `make_package.sh` echt laufen lassen (Python-Finder, kein Installer, neue cmp-Gates), das Gate-Skript unter
Linux (Docker) pruefen. Ich aendere KEINE Werkzeuge.

Status: IN ARBEIT (laufend fortgeschrieben).

## 0. Ausgangslage (03:24-03:29)
- Pruefgegenstand: Bauer-Stand **eff38fc1** (a358fd5d..eff38fc1; Nachbesserung R2 = Gate v6, Selbsttest 202 Faelle
  + 58 innere Proben, Kette `apk_pruefen.sh` mit Pruefkopie/zipalign/Signer-Pin, make_package `--quellbaum`/`--paket`).
  Gelesen: AUFTRAG.md, android_gate_nachbesserung.md (R1+R2), pruefer_echtlauf_r2.md, die fuenf Memory-Regeln,
  build_android.sh, apk_pruefen.sh, python_finden.sh, make_package.sh, Kopf von apk_asset_gate.py.
- Parallel im selben Baum: Gegenpruefer Umgehung R3 (Dossier `pruefer_umgehung_r3.md`, Sandbox `build/r34a/pruefer_r3/`,
  nicht per SendMessage erreichbar - ListAgents zeigt nur die Hauptsitzungen). Vor jedem Lauf, der release/ oder
  platform/android/ beschreibt, Prozessliste geprueft (kein java/gradle/build_android/make_package). Maschine unter
  Last: Granaten-Sitzung (mehrere `local_build.sh test/build` in r34g_*-Baeumen, DuckStation-Aufnahmen).
- Mein Arbeitsordner: `build/r34a/pruefer_echtlauf_r3/` (Logs, Kopien; nicht versioniert). Belege:
  `pruefer_echtlauf_r3_belege/`.
- `git status --short release/ re15_port/` leer; `release/` 8 versionierte Dateien, sha256 festgehalten
  (`build/r34a/pruefer_echtlauf_r3/release_vorher.sha256`; SUMS `9432f742...`, SUMS_android `cd139335...`).
  `re15_port/platform/android/` ohne app/build, app/.cxx, .gradle, local.properties -> voller NDK-Bau.
  SDK: NDK 27.2.12479018, cmake 3.22.1, build-tools 35.0.0 vorhanden; `_deps/SDL2-2.28.5` vorhanden.
- Referenz: Archiv `C:/workspace/Re15Data/re15_packages_archiv/v0.8.19/re15_port_v0.8.19_android.apk` (363212403 B,
  SUMS `514bebd5...`), nur gelesen.
- **Hauptbaum (nur gelesen) gegen die neue Wurzel-/Baumregel des Gates:** `re15_port/shared_assets/` enthaelt dort
  genau PSX, RE15DOOR, RE2, extracted_fx; `git status --short --ignored -- re15_port/shared_assets synchro` zeigt nur
  `synchro/unused/...` (nicht in der Liste). Ein Release aus dem Hauptbaum stolpert also nicht an Zusatzordnern
  oder Thumbs.db/desktop.ini (AGP-ignoreAssetsPattern).
- **Python-Schnappschuss 0** (03:28:12, `py_zustand_0_vorher.txt`, 125 Zeilen, eigenes Skript `py_zustand_r3.ps1`
  = r2-Skript, Prozesse nur bei Installer-Verdacht): `HKEY_CURRENT_USER\Software\Python\PythonCore` ohne
  Unterschluessel; Startmenue nur ProgramData 3.9/3.10/3.12; WindowsApps-Aliase 0 Byte (2026-07-07);
  `%LOCALAPPDATA%\Python` = `_cache` (15:39:13); kein `NEU_SEIT 03:20`; kein Installer-Prozess. Altlast
  Uninstall-Eintrag `pymanager-pythoncore-3.14-64` (Basislinie, nicht diese Runde).
- Laufwerkzeug `pruefer_echtlauf_r3_belege/lauf_mit_zeit.sh` (Zeitstempel je Zeile, EXIT per Prozess-Substitution;
  Selbstprobe `exit 3` -> rc 3 + `EXIT=3`).

## 1. Voller Android-Bau
(folgt)

## 2. Gebaute APK gegen Referenz-APK
(folgt)

## 3. make_package.sh echt
(folgt)

## 4. Linux (Docker)
(folgt)

## Befunde
(folgt)

## Urteil
(folgt)
