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

## 3. make_package.sh echt (ohne Shim, ohne touch)

Vorbereitung (`binaries_vorbereiten.sh`, Log `binaries_vorbereiten.log`; Archiv nur gelesen): Split-Saetze win64 +
linux nach `build/r34a/pruefer_echtlauf_r2/pakete/` kopiert, `sha256sum -c` gegen die Archiv-SUMS 4 x OK, per
`zip -s 0` zusammengefuehrt, NUR die Binaries entpackt (unzip stellt die Original-mtime her) und per `cp -p` nach
`release/win_out/re15_pc.exe` (sha256 `30d5b67b...`, 19:34:23) und `release/linux_out/re15_pc` (`abfbe7c5...`,
19:34:13) gelegt - KEIN touch. Letzter PC-Code-Commit (Standardpfade ohne platform/android) = cf386e32 19:02:06;
letzter Commit an den APK-Pfaden = f2d26986 20:55:26. APK = der Bau aus Abschnitt 1 (`9be7e512...`, 23:24).
Laeufe ueber `mp_isoliert.sh`: make_package.sh UNVERAENDERT, nur die git-Schreibzugriffe am Ende in Wegwerf-Index
+ Wegwerf-Objektspeicher (`GIT_INDEX_FILE`/`GIT_OBJECT_DIRECTORY`, Alternates) - der echte Index dieses Baums wird
auch vom Gegenpruefer Umgehung R2 zum Committen benutzt.

### 3a Normaler Lauf `bash release/make_package.sh --version v0.8.19` (23:26:27-23:28:01, 94 s): **EXIT=0, `== Fertig ==`**
Log `mp_voll.log`. Normaler Git-Bash-PATH (kein Shim, zip nicht im PATH -> make_package traegt /c/msys64/usr/bin nach).
- `Python: /c/Python310/python (3.10.11)` (0,25 s nach Start).
- APK-Kette vor den Kopierminuten (11,4 s): Frische ok (kein VERALTET), `APK-Werkzeuge ... 35.0.0`, Stichproben
  3604, aapt `versionName='v0.8.19'` + beide ABIs, apksigner v2 true (Signer `432bc749...`), Selbsttest 72/72
  (5,9 s), Gate 3603/3603 bytegleich, `RE2/DOOR 27/27`, `RE15DOOR 30/30`, `TORSE.VBS gleich` (3,3 s);
  `gepruefte APK (sha256 crc32 Bytes): 9be7e512... 99a4f42a 363212403`.
- **PC-Binaries mit Archiv-Zeitstempel 19:34 ohne "VERALTET"** (r1-B2 im echten Fluss behoben; r1 brauchte touch).
- Linux: Optimierungs-Gate (4 SDL2-Pfade), glibc 2.29, Kopie, check_tree `Tuerarchive im Paket: 27 x
  shared_assets/RE2/DOOR/*.DO2` (Schleife mit dem neuen cmp je Datei, kein Abbruch) + `30 x RE15DOOR`, TORSE.VBS-cmp
  ohne Abbruch, LF-Gate, `OK: 349M, 3606 Dateien`. Windows: dieselben check_tree-Zeilen, Laufzeit-Gate `in_pkg 26/26`
  + `foreign_cwd 26/26` aus dem Paket, `3607 Dateien`.
- Zippen ueber `"$PY"`: linux `Volumes: 2, Katalog 3800`, x-Bit `re15_pc 100644 -> 100755`, `run.sh 100755`
  (zurueckgelesen); win64 `3801`; android `(APK 347M, = gepruefte Datei)`, `Volumes: 2, Katalog 1`,
  `APK im Split-Satz = gepruefte APK (CRC32 99a4f42a, 363212403 B)`; SHA256SUMS.txt; Git (Wegwerf-Index):
  `0 alte ... entfernt, 6 neue vorgemerkt`. Echter Index danach: nichts vorgemerkt (`git diff --cached` leer).
- **Paketinhalt gegen die Archiv-Pakete** (`paket_vergleich_r2.py`, zusammengefuehrt; Name/Groesse/CRC/Unix-Modus;
  `paket_vergleich_r2_ergebnis.txt`): win64 3801/3801 und linux 3800/3800 **identisch**; android 1 Eintrag, nur die
  APK anders (Neubau), sha256 des Eintrags im neuen Satz = `9be7e512...` = genau die gepruefte APK.
- Python-Schnappschuss 2 (23:28:49, mit `-seit 23:21:00`) == Schnappschuss 0: kein neuer Registry-Schluessel, kein
  Startmenue-Eintrag, keine neue Datei in `%LOCALAPPDATA%\Python`, `...\Programs\Python`, `...\WindowsApps`,
  Startmenue (Benutzer + ProgramData); kein python/pymanager/msiexec-Prozess.

### 3b Die neuen cmp-Gates im echten Fluss (`mp_neg_cmp.sh`, `--zip-only` auf die Paketordner aus 3a)
Je Fall EIN Byte (Offset 100, Groesse gleich) in einer Paketdatei gekippt; danach aus dem Quellbaum zurueck (cmp gleich):
| Fall | EXIT | Abbruch (woertlich) |
|---|---|---|
| N1 pkg-linux RE2/DOOR/DOOR04.DO2 | 1 | `RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR04.DO2` |
| N2 pkg-linux RE2/TORSE.VBS | 1 | `RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS` |
| N3 pkg-win RE2/DOOR/DOOR36.DO2 (letzte Datei der Schleife; Linux-Paket intakt -> Linux check_tree 27+30 ok) | 1 | `... RE2/DOOR/DOOR36.DO2` im Windows-Teil |
| N4 pkg-win RE2/TORSE.VBS | 1 | `... TORSE.VBS` im Windows-Teil |
In allen vier lief vorher die APK-Kette gruen (12-24 s) und es wurde nichts gezippt (Abbruch vor dem Zippen).

### 3c Stolperdraht: ruft irgendein Teil python3/python/py AM FINDER VORBEI? (`stolperdraht_python3.sh`, `stolperdraht.log`)
Im Normalfluss steht /c/Python310 (PATH-Stelle 12) VOR WindowsApps (35) - der Alias wird dort gar nicht erst
Kandidat, ein Umgehungsaufruf von `python3` traefe aber zuerst den WindowsApps-Alias (`type -a python3`: WindowsApps
zuerst). Deshalb zwei zusaetzliche Laeufe mit PATH = `<stolperdraht>:/usr/bin:/mingw64/bin:<WindowsApps>:<Rest>`:
im Stolperdraht-Ordner liegen `python3`, `python`, `py` - Skripte, die jeden Aufruf mit Argumenten + Elternprozess
protokollieren und mit rc 97 scheitern (KEIN Shim: sie lassen nichts gelingen). Jeder Aufruf ausser den Probe-
Aufrufen des Finders waere ein Umgehungsaufruf - und traefe ohne den Draht den WindowsApps-Alias.
- `build_android.sh --gate-only release/...apk --version v0.8.19` (`gateonly_stolper.log`): Finder verwirft
  Draht-python3/python (`Pruefung rc=97`), dann `WindowsApps/python3` und `WindowsApps/python` **(WindowsApps-Alias,
  NICHT gestartet)**, waehlt `/c/Python310/python (3.10.11)`; Kette gruen, **EXIT=0** `ANDROID-GATES-OK` (13,5 s).
- `make_package.sh --version v0.8.19` voll (`mp_stolper.log`, 23:31:44-23:33:33): dieselben vier Verwerfungen, dann
  APK-Kette, beide Pakete (27 + 30 Tuerarchive), Zippen (Katalog 3800/3801/1, x-Bit, `APK im Split-Satz = gepruefte
  APK`), **EXIT=0 `== Fertig ==`**.
- Draht-Protokoll ueber BEIDE Laeufe: genau **4 Eintraege**, alle vier `timeout 30 <draht>/python{3,} -c import sys,
  zipfile, hashlib; ...` = die Probe des Finders. **Kein einziger Umgehungsaufruf** (verify_split, zip_exec_bit.py
  setzen/pruefen, apk_kennung x2, verify_apk_im_zip, Selbsttest, Gate - alle ueber `"$PY"`).
- Eigener Fehlgriff, offen gelegt: der ERSTE Versuch setzte WindowsApps VOR /usr/bin; dadurch loeste mein eigener
  `bash`-Aufruf des Laufwerkzeugs auf `WindowsApps/bash.exe` (WSL-Alias) auf -> `WSL ... execvpe(/bin/bash) failed`,
  sonst nichts (kein Skript lief an; WSL-Prozesse danach alle von 15:08 = Docker Desktop). Kein Python-Bezug.
- Python-Schnappschuss 3 (23:33:45, `-seit 23:21:00`) == Schnappschuss 0.

### 3d Aufraeumen release/ (23:34)
Neue SHA256SUMS.txt vorher gegengeprueft (`sha256sum -c` 6/6 OK). Dann `git restore --source=HEAD` fuer SHA256SUMS.txt
+ 6 Split-Volumes (danach 7/7 sha256 = Stand vor den Laeufen, `release_vorher.sha256`), `pkg-linux`, `pkg-win`,
`win_out`, `linux_out`, APK geloescht (Kopie fuer Schritt 4 unter `build/r34a/pruefer_echtlauf_r2/neu_v0.8.19.apk`);
`git status --short release/ re15_port/` leer, `git status --ignored release/` ohne Reste.
