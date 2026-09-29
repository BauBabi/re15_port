# Runde 34a — Volle Android-Asset-Prüfung (Dossier)

Zweig `r34a/android-gate`, Arbeitsbaum `.claude/worktrees/r34a_android`.
Auftrag: `analysis/befunde_runde34_android/AUFTRAG.md` (offener Punkt aus v0.8.19:
„Der Android-Bau prüft bisher nur eine der 30 neuen Türdateien automatisch").

Status: **in Arbeit** (wird laufend fortgeschrieben).

## 0. Laufprotokoll

- 2026-09-29 20:3x: Dossier angelegt (erster Werkzeugaufruf). Bestand gelesen (Abschnitt 1),
  Python-Schnappschuss vor allen Laeufen genommen (Abschnitt 3.0).

## 1. Bestand (gelesen, nicht vermutet)

- `re15_port/platform/android/app/build.gradle:99-122` stageAssets (Sync) spiegelt fuenf Baeume:
  `shared_assets/{PSX,extracted_fx,RE2,RE15DOOR}` (Basis portRoot) und `synchro` mit
  `include "STAGE*/**"` (Basis repoRoot). doFirst prueft nur Stichproben, fuer die Tuerarchive
  genau EINE Datei (`shared_assets/RE15DOOR/P07G.DO2`), RE2/DOOR gar nicht. Kopfkommentar :12-15
  nennt noch "vier Asset-Baeume" (RE15DOOR fehlt dort).
- `:124-145` writeAssetManifest: Kopfzeile `# re15 assets <n> <summe>\n`, dann
  `"<len>\t<relativePath.pathString>"` je Datei, als Zeichenketten sortiert, mit `\n` verbunden,
  abschliessendes `\n` (`manifest.text = ...`, :141).
- Leser auf dem Geraet `re15_port/platform/android/jni/android_glue.c:146-256`:
  liest `re15_assets.txt` per SDL_RWFromFile (hoechstens 64 MiB, :80), Kopf per
  `sscanf(man, "# re15 assets %ld %lld")` am PUFFERANFANG (:163), Zeilen an `\n` getrennt,
  angehaengte `\r` abgeschnitten (:200), leere und `#`-Zeilen uebersprungen (:201), Zeile ohne Tab
  STILL uebersprungen (:203), Groesse per `atoll` (:205), Pfad = Rest nach dem ersten Tab, gelesen
  als `assets/<pfad>` (SDL_RWFromFile(rel)), kopiert nach `<anker>/<pfad>`; Fehler, wenn die
  gelesene Bytezahl != Manifest-Groesse (:225) -> Marker wird nie geschrieben.
  Folgen fuer das Gate: fehlende Manifestzeile = Datei wird NIE entpackt; falsche Groesse =
  Entpack-Fehler bei jedem Start; BOM vor der Kopfzeile = Kopf unlesbar (n=0).
- `release/build_android.sh:211-237` Gates: 10 Stichproben (`unzip -Z1`), Zahl der
  `assets/`-Eintraege, WARNUNG bei `Defl:N` (nur unter `assets/shared_assets/`), aapt badging
  (Paketname + beide ABIs; die im Kopf behauptete VERSIONS-Pruefung fehlt im Code).
- `release/make_package.sh`: check_tree :199-219 RE2/DOOR nur `-s` (kein cmp), RE15DOOR mit cmp,
  TORSE.VBS nur `-s`; Python-Aufrufe `python3 -` :438 (verify_split) und
  `python ".../zip_exec_bit.py"` :536/:539. v0.8.19 brauchte dafuer einen Shim
  `build/py3shim` im PATH (analysis/befunde_runde33/release_v0819.md Abschnitt 3).
- Git-Bash dieser Maschine (gemessen, `type -a -P`): `python3` -> 1. WindowsApps-Alias,
  2. `/c/msys64/mingw64/bin/python3`; `python` -> `/c/Python310/python`, `/c/Python39/python`,
  WindowsApps-Alias, `/c/msys64/mingw64/bin/python`. Die WindowsApps-Eintraege sind App-Aliase
  des "PythonSoftwareFoundation.PythonManager" (py/python/python3/pymanager.exe).
- Referenz-APK v0.8.19 (Kopie `build/r34a/ref_v0.8.19.apk`, sha256 514bebd5... = Archiv-SUMS):
  3616 Eintraege, davon **3604 unter assets/** (3603 Dateien + re15_assets.txt), alle Stored,
  keine Verzeichniseintraege, keine Duplikate; Manifest 163901 B, LF, kein BOM,
  Kopf `# re15 assets 3603 356678277`. Hinweis: die Auftragszahl "3571" ist der Stand v0.8.18
  (release_v0819.md:86-87: +33 = 30 RE15DOOR + 3 RE2/DOOR).
- Quellbaum Arbeitsbaum: PSX 3193 / extracted_fx 13 / RE2 277 / RE15DOOR 30 / synchro 289
  (davon STAGE1 78 + STAGE2 12 im Muster, `unused/` + README.md draussen). Keine Links, keine
  Punkt-/Unterstrich-/Nicht-ASCII-Namen in den eingeschlossenen Teilen.
- Unabhaengige Quelle fuer RE15DOOR (Groesse + FNV-1a je Archiv): `engine/src/gen/re15_tuer_eigen.inc`,
  geprueft von der Suite (`tests/unit/probes/r33_tueren.cmake`, Test "archive"). Das Gate prueft
  deshalb APK gegen Quellbaum; Quellbaum gegen Tabelle bleibt Sache der Suite.

## 2. Werkzeuge

_folgt_

## 3. Messwerte

_folgt_

## 4. Offen

_folgt_

## 3.0 Python-Schnappschuss VOR allen Laeufen (20:34)

`py_snapshot.ps1` (Scratchpad): HKCU/HKLM `Software\Python` rekursiv, Startmenue `Python*`
(Benutzer + ProgramData), `%LOCALAPPDATA%\Python`, `%LOCALAPPDATA%\Programs\Python`.
Stand: HKCU `PythonCore` ohne Unterschluessel; HKLM 3.9/3.10/3.12; Startmenue nur ProgramData
3.9/3.10/3.12; `%LOCALAPPDATA%\Python\_cache` (15:39, vor dieser Sitzung); kein Programs\Python.
