# Gegenpruefung Runde 2 — Linse ECHTER LAUF / INTEGRATION (Android-Asset-Pruefung)

Pruefer: Gegenpruefer (Linse echter Lauf/Integration, Runde 2)
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Datum: 2026-09-29
Status: **fertig - Urteil: haltbar** (Befunde B1-B2 niedrig, siehe Ende). Pruefgegenstand: Bauer-Stand
35d25455 (a358fd5d..35d25455); dieses Dossier aendert keine Werkzeuge.

Kurz: voller Android-Bau EXIT=0 `ANDROID-BUILD-OK` (Kette 12,5 s = 8,1 % des Laufs; Selbsttest 72/72, Gate
3603/3603 mit Zaehlung je Baum); APK = Referenz unter assets/ (unabhaengig verglichen, auch gegen den Quellbaum);
make_package.sh ohne Shim und ohne touch EXIT=0, Python = C:/Python310, kein Installer (Schnappschuesse 0-4
gleich), cmp-Gates RE2/DOOR + TORSE.VBS brechen im echten Fluss ab (Linux- und Windows-Paket), Stolperdraht:
0 Aufrufe am Finder vorbei; unter Linux Selbsttest 72/72 + Gate rc 0/1/2 wie erwartet, volle Kette ohne SDK
geschlossen rc 1.

## Auftrag (Kurzfassung)
1. Voller Android-Bau im Arbeitsbaum (`bash release/build_android.sh --version v0.8.19 --no-toolchain`),
   neue Gates im echten Fluss (Selbsttest + volle Pruefung, Zaehlung je Baum), `ANDROID-BUILD-OK`,
   Laufzeit der neuen Gates messen; danach release/SHA256SUMS_android.txt zuruecksetzen.
2. Gebaute APK gegen Referenz-APK v0.8.19: gleiche Asset-Liste, gleiche Inhalte.
3. make_package.sh echt laufen lassen (Binaries aus Archiv v0.8.19), Python-Finder waehlt echtes
   Python, KEIN Installer (Registry + Startmenue vorher/nachher), neue cmp-Gates RE2/DOOR + TORSE.VBS.
4. apk_asset_gate.py unter Linux (Docker-Bau-Image) gegen Referenz-APK + Quellbaum, --selbsttest.

## Protokoll

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

## 4. Linux (Docker Desktop 27.5.1 lief; Baum und Archiv NUR LESEND eingehaengt, Faelschungen nur im Container)

Skripte + Logs: `linux_lauf_r2.sh` / `linux_lauf_r2.log` (Image `re15-linux-build:deb11`: Debian 11 bullseye,
Python 3.9.2, bash 5.1.4, `OSTYPE=linux-gnu`, kein unzip/cygpath/java/aapt) und `linux_lauf_r2_deck.sh` /
`linux_lauf_r2_deck.log` (Image `re15-deck:latest`: Steam Runtime 3 sniper, Python 3.9.2, unzip, kein SDK).
Gate-Datei im Mount: 0 CR (LF), Modus 777 (9p).
| Lauf | Ergebnis |
|---|---|
| L1 `bash release/python_finden.sh` | `Python: /usr/bin/python3 (3.9.2)`, rc 0 |
| L2 `python3 release/apk_asset_gate.py --selbsttest` | **72/72** `[ok]`, rc 0, 3,6 s (Temp unter /tmp; /src ist ro - der Selbsttest schreibt nichts in den Baum) |
| L3 Referenz-APK gegen `--repo /src` (9p) | **rc 0** `APK-ASSET-GATE-OK`, Zaehlung wie unter Windows (PSX 3193, fx 13, RE2 277, RE15DOOR 30, synchro 90; RE2/DOOR 27/27, RE15DOOR 30/30, TORSE.VBS gleich; 3616 Eintraege roh lesbar), 43,4 s |
| L4 die in Abschnitt 1 gebaute APK gegen `/src` | rc 0, dieselbe Zaehlung, 50,9 s |
| L5 Negativ: Kopie der Referenz, 1 Byte in den Daten von RE15DOOR/P07G.DO2 | **rc 1** `APK-Eintrag beschaedigt (CRC/Entpacken): ...P07G.DO2: gelesen 55908 B, CRC a88df3b1 - Zentralverzeichnis 55908 B, CRC 2c23d999`, RE15DOOR 29/30 |
| L6 Negativ: Kopie ohne RE2/TORSE.VBS (Manifest unveraendert) | **rc 1** `fehlt in der APK: assets/shared_assets/RE2/TORSE.VBS` + `Manifest nennt ... die APK hat keinen Eintrag`, `TORSE.VBS: ... APK FEHLT` |
| L7 `--repo /tmp` | rc 2 `build.gradle fehlt: /tmp/re15_port/platform/android/app/build.gradle` |
| L8 `source python_finden.sh` unter `set -euo pipefail`, Gate ueber `"$PY"` | `PY=/usr/bin/python3`, gate-rc 0 |
| L9 Direktaufruf `/src/release/apk_asset_gate.py --selbsttest` (Shebang `#!/usr/bin/env python3`) | 72/72, rc 0 (unter Linux unkritisch; zu Windows siehe Befund B2) |
| L10 Quellbaum-Kopie auf dem Container-Dateisystem (346 MB, Kopie 63,7 s), Gate gegen die Referenz | rc 0, **8,0 s** statt 43-54 s -> die Linux-Laufzeiten ueber /src sind der 9p-Mount, nicht das Gate |
| D1 `bash release/build_android.sh --gate-only <ref> --version v0.8.19` (deck, ohne Android-SDK) | **rc 1** `ABBRUCH: APK-Pruefung: Android-SDK-Ordner fehlt: /root/Android/Sdk ... Nur die Assets: "$PY" release/apk_asset_gate.py --repo . <apk>` (r1: hier noch rc 0 mit "aapt uebersprungen" - jetzt geschlossen) |
| D2 dasselbe ohne `--version` | rc 2 `--gate-only braucht --version` |
| D3 Gate direkt im deck-Image | rc 0, 54,1 s |
Folgerung: `apk_asset_gate.py` (Selbsttest + Pruefung) und `python_finden.sh` sind unter Linux portabel und liefern
dieselben Zahlen; die volle Kette (`apk_pruefen.sh`: aapt/apksigner/Java) braucht ein Linux-Android-SDK, das keines der
re15-Images hat - dort bricht sie geschlossen ab (D1), statt still zu ueberspringen.
Nebenbei: `re15-deck:latest` hat ENTRYPOINT `/usr/local/bin/re15-build`; mein erster Aufruf ohne `--entrypoint bash`
startete den Deck-Bau, der am ro-Mount scheiterte (`CMake Error: Unable to (re)create ... pkgRedirects`) - nichts
geschrieben (`git status` leer, kein `targets/steamdeck/build`).

## 5. Der urspruengliche offene Punkt in der ECHTEN Kette (`kette_neg_tuer.sh`, `kette_neg_tuer.out`, `kette_neg_p2ds.log`)
Kopie der in Abschnitt 1 gebauten APK OHNE `assets/shared_assets/RE15DOOR/P2DS.DO2` (bewusst nicht P07G.DO2, die
fruehere einzige Stichprobe), `zipalign -p 4`, mit dem Debug-Schluessel neu signiert (`apksigner verify`:
`Verifies`, v2 true) - also eine APK, die wie ein normaler Bau aussieht. `build_android.sh --gate-only <apk>
--version v0.8.19`: Stichproben ok (3603 Asset-Eintraege), aapt `versionName='v0.8.19'`, apksigner v2 true,
Selbsttest 72/72 - alles, was bis v0.8.19 geprueft wurde, ist gruen - dann das Gate: `RE15DOOR: Quelle 30, APK 29,
sha256 gleich 29/30`, `fehlt in der APK: assets/shared_assets/RE15DOOR/P2DS.DO2`, `Manifest nennt
shared_assets/RE15DOOR/P2DS.DO2 (78244 B), die APK hat keinen Eintrag` -> `ABBRUCH: APK-Asset-Gate: die APK weicht
vom Quellbaum ab`, **EXIT=1**. (Erster Versuch scheiterte an meinem Laufwerkzeug: `/c/msys64/usr/bin` vorn im PATH
machte `bash` zur MSYS2-bash, die `LOG` nicht sah - Skript danach mit absolutem zip-Pfad, Lauf wie oben.)
Grenze (nicht meine Linse, zur Einordnung): das gilt bei VOLLSTAENDIGEM Quellbaum - das Gate vergleicht APK gegen
Quellbaum, nicht gegen die Soll-Zahl 30. Fehlt dieselbe Datei auch im Quellbaum, laeuft die Kette gruen
(Gegenpruefer Umgehung R2, Abschnitt 1.2: `RE15DOOR: Quelle 29, APK 29`); Quellbaum gegen die Engine-Tabelle
`re15_tuer_eigen.inc` prueft laut Bauer-Dossier 1 die Suite (`tests/unit/probes/r33_tueren.cmake`).

## 6. Aufraeumen + Endstand (23:45)
- Eigene Laufreste geloescht: `build/r34a/pruefer_echtlauf_r2/{git_iso (489 MB), pakete (1,5 GB), neu_v0.8.19.apk,
  kette_neg (1,1 GB)}` und die Gradle-Ausgaben des eigenen Baus `re15_port/platform/android/{app/build (1,5 GB),
  app/.cxx, .gradle, build, local.properties}` (vorher Prozessliste: kein Bau im Baum; Gegenpruefer Umgehung R2
  arbeitete in einer eigenen Sandbox unter build/r34a/pruefer_umgehung_r2). Liegen gelassen: die Logs unter
  `build/r34a/pruefer_echtlauf_r2/` (~300 KB), `re15_port/platform/android/_deps` (vom Bauer, schon vorher da).
- `git status --short release/ re15_port/` leer; `git status --ignored` unter release/ ohne Reste; release/-Dateien
  7/7 sha256 = Stand vor den Laeufen; keine `%TEMP%\apk_gate_selbsttest_*` (0 nach 9 Windows-Selbsttests).
- Python-Schnappschuss 4 (23:45:59, `-seit 23:21:00`) == Schnappschuss 0 (125 Zeilen, `Compare-Object` leer).
  Die Altlast (Uninstall-Eintrag `pymanager-pythoncore-3.14-64`) steht unveraendert in allen fuenf Schnappschuessen.

## Befunde

- **B1 (niedrig) - Direktaufruf des Gates unter Git-Bash landet beim WindowsApps-Alias.** `release/apk_asset_gate.py`
  beginnt mit `#!/usr/bin/env python3`, ist unter Git-Bash wegen des Shebangs ausfuehrbar, und sein Kopf zeigt
  Direktaufrufe (`apk_asset_gate.py:61-62`: `apk_asset_gate.py [--repo <repo>] <apk>`, `apk_asset_gate.py
  --selbsttest`). Ein direkter Start loest `python3` ueber den PATH auf; auf dieser Maschine ist der erste Treffer
  `type -P python3` = `/c/Users/mjoedicke/AppData/Local/Microsoft/WindowsApps/python3` - die v0.8.17-Installer-Klasse.
  Beleg ohne Risiko: mit dem Stolperdraht vorn im PATH landet `./release/apk_asset_gate.py --selbsttest` beim Draht
  (`stolperdraht_direkt.log`: `ARGS=[./release/apk_asset_gate.py --selbsttest]`, rc 97) - ohne Draht waere es der
  Alias. Die automatischen Aufrufer sind NICHT betroffen (alle ueber `"$PY"`; 3c: 0 Umgehungsaufrufe in Bau-Kette
  und make_package). Das Muster gab es schon (`release/zip_exec_bit.py`, gleicher Shebang, vor a358fd5d); neu ist
  ein zweites, dessen Kopf zum Direktaufruf einlaedt. Vorschlag: im Kopf nur `source release/python_finden.sh` +
  `"$PY" release/apk_asset_gate.py ...` zeigen bzw. "unter Git-Bash nie direkt starten" vermerken.
- **B2 (niedrig) - Kopf von make_package.sh nennt die neuen Voraussetzungen nicht.** Liegt
  `release/<name>_android.apk`, braucht make_package.sh jetzt Android-SDK (aapt + lib/apksigner.jar aus
  build-tools), ein JDK und Python >= 3.8 und bricht sonst ab (fail closed; Meldung klar, vgl. D1 `APK-Pruefung:
  Android-SDK-Ordner fehlt ... Nur die Assets: ...`). Der Kopf (`make_package.sh:16-23`, "Aufruf"/"Eingaben") nennt
  weder die APK als Eingabe noch diese Voraussetzungen; beschrieben ist es nur im Code (`:413-468`). Auf der
  Bau-Maschine ist alles da (3a gruen) - Dokumentationsluecke, kein Defekt.

Hinweise (kein Befund dieser Linse):
- Jede Neubau-APK hat eine andere sha256 bei gleicher Groesse (libmain.so: `__DATE__/__TIME__`,
  `platform/pc/main.c:6006`, + eingebauter Baupfad): "APK = Referenz" laesst sich nur eintragsweise belegen (so
  hier, Abschnitt 2), nicht per Hash.
- Im Normalfluss kommt der WindowsApps-Zweig des Finders nicht dran (/c/Python310 PATH-Stelle 12 < WindowsApps 35);
  belegt ist er im vollen Fluss nur durch 3c (WindowsApps vor Python gesetzt).
- Code-Lesart, von mir NICHT gemessen (Zeitfenster-Linse; der Gegenpruefer Umgehung R2 sondiert genau das gerade in
  seiner Sandbox): make_package.sh nimmt `APK_KENNUNG` erst NACH `apk_pruefen` (`:464` pruefen, `:465` Kennung). Die
  Kennung haelt also die Datei fest, die nach den ~12 s Pruefung dort liegt - der Zipzeit-Vergleich (`:662`) sichert
  damit das Kopierfenster, nicht das Pruefzeitfenster. In meinen echten Laeufen kein Effekt (niemand tauscht).

Widerlegt / bestaetigt ohne Befund:
- Der echte Android-Bau laeuft mit der neuen Kette gruen und nimmt der APK nichts weg (1, 2); die Kette veraendert
  die Datei nicht (sha256 Gradle-Ausgabe == Auslieferung).
- r1-B1 (Finder waehlte MSYS2 3.14.7) ist im echten Fluss behoben: `/c/Python310/python (3.10.11)` in Bau, make_package,
  --gate-only. r1-B2 (build.gradle-Commit machte PC-Binaries VERALTET): behoben, 3a ohne touch gruen. r1-B3
  (`.ungeprueft` bleibt liegen): im Erfolgsfall nichts uebrig. r1-B4 (make_package prueft nur Assets): make_package
  prueft jetzt Frische, versionName, Signatur und Assets der APK (3a-Log).
- Kein Python-Installer in irgendeinem Lauf (Schnappschuesse 0-4 gleich, kein Prozess, keine neue Datei).
- Paketinhalt win64/linux identisch mit v0.8.19; die APK im Android-Satz ist bytegleich die gepruefte.

## Urteil

**haltbar.** Der echte Android-Bau lief mit den neuen Gates gruen (EXIT=0, `ANDROID-BUILD-OK`; Selbsttest 72/72 in
6,9 s, volle Pruefung 3603/3603 mit Zaehlung je Baum in 3,6 s, Kette gesamt 12,5 s = 8,1 %); die gebaute APK hat
dieselben 3604 Asset-Eintraege bytegleich (und in derselben Reihenfolge) wie die Referenz-APK und stimmt unabhaengig
geprueft mit dem Quellbaum; make_package.sh lief ohne Shim, ohne touch und ohne Installer durch (Python C:/Python310,
Pakete identisch mit v0.8.19, APK im Satz = gepruefte APK), die neuen cmp-Gates brechen im echten Fluss fuer beide
Pakete ab, kein Skriptteil ruft Python am Finder vorbei; unter Linux laufen Selbsttest und Gate portabel (rc 0/1/2
wie erwartet, gleiche Zahlen), die volle Kette schliesst ohne SDK ab. Die echte Kette faengt genau den offenen Punkt
(eine fehlende RE15DOOR-Datei in einer gueltig signierten APK, bei vollstaendigem Quellbaum - Abschnitt 5). B1/B2
sind niedrig und blockieren die Uebernahme nicht.
