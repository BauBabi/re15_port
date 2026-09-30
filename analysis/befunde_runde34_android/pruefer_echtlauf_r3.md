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

## 1. Voller Android-Bau (03:29:06-03:32:46, EXIT=0)
Aufruf im Hintergrund, Rueckgabe per `lauf_mit_zeit.sh` abgefangen (keine Pipe), Log
`build/r34a/pruefer_echtlauf_r3/android_voll.log` (1412 Zeilen), Auszug `pruefer_echtlauf_r3_belege/android_voll_auszug.log`
(Kopf + alles ab Gradle-Ende, inkl. aller 202 Selbsttest-Zeilen), Zeiten `android_voll_zeiten.txt`:
`bash release/build_android.sh --version v0.8.19 --no-toolchain` -> **EXIT=0**, `== ANDROID-BUILD-OK ==`.
`--no-toolchain` reichte; voller NDK-Bau (kein app/.cxx, kein app/build vorher).
- VOR Gradle: `Python: /c/Python310/python (3.10.11)` (0,44 s nach Start), `APK-Werkzeuge: aapt + zipalign + apksigner aus
  35.0.0, Java .../jdk-17.0.15.6-hotspot/bin/java`, `erwarteter Signer: 432bc7497b520324... (release/apk_signer.sha256)`,
  `CMake-Cache des NDK-Baus verworfen`.
- Gradle: stageAssets mit dem neuen doFirst `RE2/DOOR: 27 x *.DO2`, `RE15DOOR: 30 x *.DO2`; writeAssetManifest;
  `BUILD SUCCESSFUL in 3m 8s`.
- Kette `apk_pruefen` im echten Fluss auf `release/re15_port_v0.8.19_android.apk.ungeprueft`:
  Pruefkopie `/tmp/re15_apk_pruefen.4AMbL7/...` (Kennung `b7bfb699... 6d7117f4 363212403`); Stichproben (unzip 3604
  Asset-Eintraege); aapt `versionCode='81900' versionName='v0.8.19'`, `native-code: 'arm64-v8a' 'x86_64'`, targetSdk 35;
  `zipalign -c -P 16 4: Ausrichtung ok`; apksigner `v2 ... true`, `Number of signers: 1`, Signer `432bc749...`;
  **Selbsttest: `Innere Proben: 58/58`, 202 Faelle je `[ok]` (0 Zeilen ohne `[ok]`), `SELBSTTEST-OK: 202/202`**;
  **volle Pruefung mit Zaehlung je Baum**: Baumliste = build.gradle stageAssets (5 Baeume); Tuer-Soll RE15DOOR 30/30
  (Groesse, Aufbau, FNV-1a) + RE2/DOOR 27/27; `ZIP-Struktur: 3616 Eintraege roh gelesen, 3616 fuer Android lesbar`;
  PSX 3193/3193/3193 (300522652 B), extracted_fx 13/13/13, RE2 277/277/277, RE15DOOR 30/30/30, synchro 90/90/90,
  SUMME 3603/3603/3603 (356678277 B); `RE2/DOOR 27/27`, `RE15DOOR 30/30`, `TORSE.VBS 19176 B ... gleich`, Manifest 3603
  Zeilen; `APK-ASSET-GATE-OK`, `APK-PRUEFUNG-OK`; danach mv der Pruefkopie + SHA256SUMS, `ANDROID-BUILD-OK`.
- **Laufzeit der neuen Gates** (Zeitstempel je Zeile; Maschine unter Last der Granaten-Sitzung):
  | Abschnitt | s |
  |---|---|
  | Lauf gesamt | 220,19 |
  | Gradle | 188,42 |
  | Kette apk_pruefen gesamt | **25,46 (11,6 %)** |
  | - Pruefkopie + Kennung | 0,97 |
  | - Stichproben / aapt + zipalign / apksigner | 0,52 / 0,37 / 1,21 |
  | - **Selbsttest (202 Faelle + 58 Proben)** | **12,66** (Gate meldet selbst 12,4 s) |
  | - **Gate gegen den Quellbaum** | **6,89** (Gate meldet 6,2 s) |
  | - Kennung Kopie + Pfad (2 x sha256) | 2,84 |
  | neue Gates Selbsttest + Gate | **19,55 (8,9 %)** |
  | mv + SHA256SUMS | 3,99 |
  Gegen R2 (72 Faelle: Selbsttest 6,9 s, Kette 12,5 s) hat sich die Kette etwa verdoppelt (202 Faelle, Pruefkopie,
  zweite Kennung) - bei 188 s Gradle weiterhin klein.
- Nachher: `release/re15_port_v0.8.19_android.apk` 363212403 B, sha256 **`b7bfb699...`** == Gradle-Ausgabe
  `app/build/outputs/apk/release/app-release.apk` (die Kette veraendert die Bytes nicht); keine `.ungeprueft`, keine
  Pruefkopie unter /tmp; APK gitignoriert (`.gitignore:63`). `SHA256SUMS_android.txt` (Beleg
  `SHA256SUMS_android_nach_bau.txt`) per `git restore --source=HEAD` zurueck (`cd139335...`), `git status --short
  release/ re15_port/` leer. Kopie der APK fuer 2./4.: `build/r34a/pruefer_echtlauf_r3/neu_v0.8.19.apk`.
- Python-Schnappschuss 1 (03:34:21) == Schnappschuss 0 (`Compare-Object` leer).

## 2. Gebaute APK gegen die Referenz-APK (03:33-03:34)
Eigenes, vom Gate UNABHAENGIGES Skript `pruefer_echtlauf_r3_belege/apk_vergleich_r3.py` (eigener Rohleser fuer
Zentralverzeichnis + Local Header, zlib, eigener Baum-Lauf; importiert/ruft das Gate nicht; /c/Python310). Referenz =
Kopie des Archivs (`build/r34a/pruefer_echtlauf_r3/ref_v0.8.19.apk`, sha256 `514bebd5...` = Archiv-SUMS).
Ergebnis `apk_vergleich_r3_ergebnis.txt`, **rc 0**, 6,1 s:
- 3616/3616 Eintraege, Namen eindeutig; `assets/` 3604/3604, **Reihenfolge gleich**, nur-Referenz 0, nur-neu 0.
- Alle Eintraege entpackt + sha256: unter `assets/` **0 Abweichungen** (Methode, CRC, csize, usize, sha256, Flags);
  Stored 3604/3604 beidseitig; neu: 0 Stored-Assets nicht 4-B-ausgerichtet, 0 `.so` nicht 16-KiB-ausgerichtet.
- Manifest bytegleich (`5f5acfdb...`), Kopf `# re15 assets 3603 356678277`, 3603 Zeilen, jede trifft einen Eintrag
  gleicher Groesse.
- Quellbaum unabhaengig vom Gate: 3603 Dateien (PSX 3193, RE15DOOR 30, RE2 277, extracted_fx 13, synchro 90), alle
  sha256-gleich in der neuen APK, fehlt 0, anders 0, APK-Asset ohne Quelle 0; RE2/DOOR 27/27, RE15DOOR 30/30,
  TORSE.VBS gleich.
- Ausserhalb `assets/`: 8/12 gleich, anders nur die vier `.so` (`apk_so_unterschied.txt`): eingebauter Baupfad
  (`worktrees/r33_integration` -> `worktrees/r34a_android`, libSDL2 32/16 B kleiner) und bei libmain `__DATE__`
  (`Sep 29 2026` -> `Sep 30 2026`). Kein Unterschied in Signatur-Eintraegen (v2-only, kein META-INF-Signaturblock).
- Kontrolle des eigenen Werkzeugs (nicht blind): Kopie der neuen APK mit 1 geaendertem Byte in RE15DOOR/P2DS.DO2
  (per zipfile neu geschrieben, CRC konsistent) -> rc 1, `assets/ mit Abweichung: 1 ... P2DS.DO2 ['crc','sha']`,
  `RE15DOOR: 30 Quelle, 29 gleich`. Sanity: Referenz gegen sich selbst + Quellbaum -> rc 0, gleiche Zaehlung.
- Folgerung: der Bau MIT den neuen Gates nimmt der APK nichts weg und aendert kein Asset (Unterschied nur im Code der
  vier `.so` durch Baupfad/Datum; Versionsfelder gleich v0.8.19/81900).

## 3. make_package.sh echt
(folgt)

## 4. Linux (Docker)
(folgt)

## Befunde
(folgt)

## Urteil
(folgt)
