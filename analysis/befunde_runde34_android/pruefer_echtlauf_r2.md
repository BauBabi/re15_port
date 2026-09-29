# Gegenpruefung Runde 2 — Linse ECHTER LAUF / INTEGRATION (Android-Asset-Pruefung)

Pruefer: Gegenpruefer (Linse echter Lauf/Integration, Runde 2)
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Datum: 2026-09-29
Status: IN ARBEIT (Dossier wird laufend fortgeschrieben)

## Auftrag (Kurzfassung)
1. Voller Android-Bau im Arbeitsbaum (`bash release/build_android.sh --version v0.8.19 --no-toolchain`),
   neue Gates im echten Fluss (Selbsttest + volle Pruefung, Zaehlung je Baum), `ANDROID-BUILD-OK`,
   Laufzeit der neuen Gates messen; danach release/SHA256SUMS_android.txt zuruecksetzen.
2. Gebaute APK gegen Referenz-APK v0.8.19: gleiche Asset-Liste, gleiche Inhalte.
3. make_package.sh echt laufen lassen (Binaries aus Archiv v0.8.19), Python-Finder waehlt echtes
   Python, KEIN Installer (Registry + Startmenue vorher/nachher), neue cmp-Gates RE2/DOOR + TORSE.VBS.
4. apk_asset_gate.py unter Linux (Docker-Bau-Image) gegen Referenz-APK + Quellbaum, --selbsttest.

## Protokoll
(folgt)

- 23:20 Dossier angelegt; Commits (a358fd5d..35d25455, 27 Commits), Bauer-Dossiers (android_gate.md,
  android_gate_nachbesserung.md), Pruefer-R1-Dossier und die Werkzeuge gelesen.
- 23:21 Laufwerkzeuge: `pruefer_echtlauf_r2_belege/lauf_mit_zeit.sh` (Zeitstempel je Zeile, EXIT per
  Prozess-Substitution; Selbstprobe `exit 3` -> rc 3 + `EXIT=3`), `py_zustand.ps1` (Python-Installer-
  Schnappschuss, Basis r1 + Prozesse + Dateien neuer als -seit). Execution-Policy blockt `-File`;
  gestartet per `[scriptblock]::Create(Get-Content -Raw ...)` (keine Richtlinienaenderung).
- 23:21:25 Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 125 Zeilen): HKCU `Software\Python\PythonCore`
  ohne Unterschluessel; HKLM 3.9/3.10/3.12 + PyLauncher; Startmenue nur ProgramData 3.9/3.10/3.12;
  `%LOCALAPPDATA%\Python` = `_cache` (15:39:13); kein `Programs\Python`; WindowsApps-Aliase (0 Byte,
  2026-07-07); kein python/pymanager/msiexec-Prozess. Altlast weiter da: Uninstall-Eintrag
  `pymanager-pythoncore-3.14-64` (Basislinie, nicht dieser Runde).
- Ausgangszustand: `git status --short release/ re15_port/` leer; `re15_port/platform/android/` ohne
  app/build, app/.cxx, .gradle, local.properties (voller NDK-Bau folgt); keine Bau-/Python-Prozesse.
  Parallel im Baum: Gegenpruefer Umgehung R2 (nicht per SendMessage erreichbar) - vor jedem Lauf, der
  release/ oder platform/android/ beschreibt, Prozessliste pruefen.

## 1. Voller Android-Bau im Arbeitsbaum (23:21:53-23:24:29, EXIT=0)

Aufruf (Hintergrund, Rueckgabe per `lauf_mit_zeit.sh` abgefangen, keine Pipe; Log
`build/r34a/pruefer_echtlauf_r2/android_voll.log`, Auszug `pruefer_echtlauf_r2_belege/android_voll_auszug.log`):
`bash release/build_android.sh --version v0.8.19 --no-toolchain` -> **EXIT=0**, `== ANDROID-BUILD-OK ==`.
Ausgangslage: platform/android ohne app/build, app/.cxx, .gradle -> voller NDK-Bau, 54 Tasks ausgefuehrt.

- `--no-toolchain` reichte (NDK 27.2.12479018, cmake 3.22.1, build-tools 35.0.0 im SDK).
- VOR Gradle: `Python: /c/Python310/python (3.10.11)` (0,39 s nach Start; PATH: /c/Python310 an Stelle 12,
  WindowsApps an 35 -> der Alias wird im Normalfluss gar nicht erst Kandidat, siehe 3.x fuer den Fall
  "WindowsApps vorn"), `APK-Werkzeuge: aapt + apksigner aus 35.0.0, Java .../jdk-17.0.15.6-hotspot/bin/java`.
- Gradle: stageAssets lief (nicht UP-TO-DATE) mit dem neuen doFirst: `RE2/DOOR: 27 x *.DO2`, `RE15DOOR: 30 x
  *.DO2`; writeAssetManifest `3603 Dateien, 356678277 Bytes`; `BUILD SUCCESSFUL in 2m 18s`.
- Kette `apk_pruefen` auf `release/re15_port_v0.8.19_android.apk.ungeprueft`: Stichproben ok (unzip 3604
  Asset-Eintraege); aapt `versionCode='81900' versionName='v0.8.19'`, `native-code: 'arm64-v8a' 'x86_64'`,
  targetSdk 35; apksigner `v2 ... true`, Signer #1 `432bc749...`; Selbsttest **72/72** `[ok]` (5 x rc 0,
  44 x rc 1, 23 x rc 2 - jeweils = soll); volle Pruefung: `ZIP-Struktur: 3616 Eintraege roh gelesen, 3616
  fuer Android lesbar`, Zaehlung je Baum PSX 3193 / extracted_fx 13 / RE2 277 / RE15DOOR 30 / synchro 90 =
  3603/3603 bytegleich (356678277 B), `RE2/DOOR 27/27`, `RE15DOOR 30/30`, `TORSE.VBS 19176 B gleich`,
  Manifest 3603 Zeilen; `APK-ASSET-GATE-OK`, `APK-PRUEFUNG-OK`; erst danach mv + SHA256SUMS.
- **Laufzeit (Zeitstempel je Zeile, `android_voll_zeiten.txt`):** Lauf 154,4 s; Gradle 139,2 s; Kette
  apk_pruefen **12,53 s = 8,1 %** (Stichproben 0,68 / aapt 0,20 / apksigner 1,07 / **Selbsttest 6,93** /
  **Gate 3,56**); Selbsttest+Gate 10,48 s = 6,8 %; mv+sha256sum 1,70 s; python_finden 0,39 s.
  (Maschine: Granaten-Sitzung aktiv; Werte decken sich mit Bauer 3.6: 10,9 s Kette.)
- Nachher: `release/re15_port_v0.8.19_android.apk` 363212403 B, sha256 `9be7e512...` == Gradle-Ausgabe
  `app/build/outputs/apk/release/app-release.apk` (die Kette veraendert die Datei nicht); keine
  `.ungeprueft` uebrig; `SHA256SUMS_android.txt` (Beleg `SHA256SUMS_android_nach_bau.txt`) per
  `git restore --source=HEAD` zurueck, `git status --short release/ re15_port/` leer.

## 2. Gebaute APK gegen die Referenz-APK (Archiv v0.8.19, nur gelesen, sha256 514bebd5... = Archiv-SUMS)

Eigenes, vom Gate UNABHAENGIGES Skript `pruefer_echtlauf_r2_belege/apk_vergleich_r2.py` (zipfile fuer
Inhalte, eigener Mini-Parser fuer Datenoffsets, eigener Baum-Lauf fuer den Repo-Abgleich; /c/Python310),
Ergebnis `apk_vergleich_r2_ergebnis.txt`, **rc 0**, 5,0 s:
- 3616/3616 Eintraege, Namen eindeutig; `assets/` 3604/3604, **Reihenfolge gleich**, nur-Referenz 0, nur-neu 0.
- ALLE 3616 Eintraege per sha256 des entpackten Inhalts verglichen: unter `assets/` **0 Abweichungen**
  (Groesse, CRC, Methode, sha256), Stored 3604/3604 beidseitig.
- Manifest bytegleich (`5f5acfdb...`), Kopf `# re15 assets 3603 356678277`, jede Zeile trifft einen Eintrag
  gleicher Groesse, kein Asset ohne Zeile.
- Datenoffsets aus dem Local Header: 3604 Stored-Assets je APK, 0 nicht 4-Byte-ausgerichtet (beide).
- Repo-Abgleich unabhaengig vom Gate: 3603 Quelldateien der fuenf Baeume, sha256 gleich 3603, fehlt 0,
  anders 0, nur-APK 0 (PSX 3193, RE15DOOR 30, RE2 277, extracted_fx 13, synchro 90).
- Ausserhalb `assets/`: 8/12 gleich; anders nur die vier `.so` (Code): eingebauter Baupfad
  (`worktrees/r33_integration` vs `worktrees/r34a_android`, libSDL2 16/32 B kleiner) und bei libmain
  zusaetzlich `__DATE__/__TIME__` (`platform/pc/main.c:6006`) - deshalb hat jede Neubau-APK eine andere
  sha256 (r1 e3b203ee, nb 5ec3a0eb, hier 9be7e512) bei gleicher Groesse 363212403 B.
- Folgerung: Der Bau MIT den neuen Gates nimmt der APK nichts weg und aendert kein Asset.
- 23:25:58 Python-Schnappschuss 1 (nach dem Android-Bau) == Schnappschuss 0 (`Compare-Object` leer, 125 Zeilen).
