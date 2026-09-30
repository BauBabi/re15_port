# Gegenpruefung Runde 1 — Linse ECHTER LAUF / INTEGRATION

Pruefer: Gegenpruefer (echter Lauf), Runde 1
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Beginn: 2026-09-29

Auftrag (Kurzfassung): Die vom Bauer eingebaute Android-Asset-Pruefung im echten Ablauf pruefen:
1. Voller Android-Bau (build_android.sh --version v0.8.19 --no-toolchain) — Gates im echten Fluss, ANDROID-BUILD-OK, Laufzeit.
2. Gebaute APK gegen Referenz-APK v0.8.19 (Asset-Liste + Inhalte gleich).
3. make_package.sh echt laufen lassen (Python-Finder, kein Installer, neue cmp-Gates RE2/DOOR + TORSE.VBS).
4. apk_asset_gate.py unter Linux (Docker-Bau-Image), inkl. --selbsttest.

Belege: analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/

## Stand

**Fertig — Urteil: haltbar** (Befunde B1-B4 niedrig, siehe unten).
- 21:1x Dossier angelegt, Commits/Bauer-Dossier/Werkzeuge gelesen, Python-Schnappschuss 0.
- 21:16:57-21:20:50 voller Android-Bau EXIT=0 ANDROID-BUILD-OK (Abschnitt 1); 21:22 APK-Vergleich rc 0 (Abschnitt 2).
- 21:23-21:30 make_package.sh: 3a-3e (Abschnitt 3), release/ danach auf HEAD, Artefakte weg.
- 21:33-21:37 Linux-Laeufe in Docker (Abschnitt 4).
- 21:40 Aufraeumen: Gradle-Reste des eigenen Baus (`platform/android/{app/build,app/.cxx,.gradle,build,local.properties}`,
  ~1,6 GB) und der eigene Arbeitsordner `build/r34a/pruefer_echtlauf_r1/` (zusammengefuehrte Pakete, Wegwerf-Objekt-
  speicher 482 MB, Negativ-APK) geloescht; Ordner der anderen (Bauer, Pruefer Umgehung) unberuehrt.
  `git status --short release/ re15_port/` leer. Python-Schnappschuss 4 (Ende) == 0.

## 0. Ausgangslage

- Pruefgegenstand: `git log --oneline a358fd5d..HEAD` = c1c91713 (Auftrag) .. d8a3ad09 (feat: volle
  Android-Asset-Pruefung). Geaendert: `release/apk_asset_gate.py` (neu, 971 Z.), `release/python_finden.sh`
  (neu), `release/build_android.sh`, `release/make_package.sh`, `re15_port/platform/android/app/build.gradle`.
- Parallel im selben Baum: Gegenpruefer "Umgehung" (Dossier pruefer_umgehung_r1.md, Faelschungen unter
  build/r34a/pruefer_umgehung_r1/). Beim Start liefen keine Bau-Prozesse im Baum (Prozessliste 21:15:
  nur die Granaten-Sitzung mit C:\Python310\python.exe lauf.py, anderer Baum).
- Seit dem Tag v0.8.19 (64170c05) aendert sich unter `re15_port/engine re15_port/platform re15_port/include`
  NUR `re15_port/platform/android/app/build.gradle` (`git diff --stat v0.8.19..HEAD`) -> die PC-Binaries
  aus dem v0.8.19-Archiv entsprechen dem heutigen PC-Code.
- Werkzeuge des Pruefers (nur Belege, keine Aenderung an release/*):
  `pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh` (Zeitstempel je Ausgabezeile per `$EPOCHREALTIME`,
  Rueckgabe per Prozess-Substitution statt Pipe, haengt `EXIT=<rc>` an; Selbstprobe `exit 3` -> rc 3,
  `EXIT=3`), `pruefer_echtlauf_r1_belege/py_zustand.ps1` (Python-Installer-Schnappschuss: HKCU/HKLM
  `Software\Python` rekursiv MIT Werten, HKCU-Uninstall-Eintraege "Python", Startmenue beider Wurzeln
  rekursiv, `%LOCALAPPDATA%\Python` + `\Programs\Python` rekursiv (Anzahl + neuester Eintrag),
  WindowsApps-Aliase, `release/Python` + `python_install*`-Logs im Baum — startet nichts).

### 0.1 Python-Schnappschuss 0 (vor allen Laeufen, 21:14) — `py_zustand_0_vorher.txt`, 121 Zeilen
- HKCU `Software\Python\PythonCore` ohne Unterschluessel; HKLM 3.9/3.10/3.12; Startmenue nur ProgramData
  3.9/3.10/3.12; `%LOCALAPPDATA%\Python` = nur `_cache\last_welcome.txt` (15:39:13); kein `Programs\Python`;
  kein `release/Python` im Baum.
- **Altlast (vor dieser Runde, nicht vom Bauer):** HKCU
  `Software\Microsoft\Windows\CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64` existiert noch:
  DisplayName "Python 3.14.7", InstallDate 20260929, InstallLocation
  `C:\workspace\git\reAi_v2\.claude\worktrees\r31_integration\release\Python\pythoncore-3.14-64` (Ordner
  existiert NICHT mehr), UninstallString ruft den WindowsApps-PythonManager. Das ist der Rest des
  v0.8.17-Unfalls: die Entfernung 2026-09-29 (Memory runde31) nahm PythonCore\3.14, Startmenue und Ordner
  weg, aber nicht diesen Uninstall-Eintrag -> unter "Installierte Apps" steht weiter "Python 3.14.7".
  Fuer die Vorher/Nachher-Vergleiche unten ist er Teil der Basislinie.

## 1. Voller Android-Bau

Aufruf (im Baum, Hintergrund, Rueckgabe per `lauf_mit_zeit.sh` selbst abgefangen, keine Pipe):
`LOG=build/r34a/pruefer_echtlauf_r1/android_voll.log bash .../lauf_mit_zeit.sh bash release/build_android.sh --version v0.8.19 --no-toolchain`
-> **EXIT=0**, `== ANDROID-BUILD-OK: .../release/re15_port_v0.8.19_android.apk ==` (21:16:57-21:20:50).
Auszug: `pruefer_echtlauf_r1_belege/android_voll_auszug.log` (Kopf + gesamter Gate-Teil, Zeitstempel je Zeile).

- `--no-toolchain` reichte (NDK 27.2.12479018, cmake 3.22.1, build-tools 35.0.0 lagen im SDK).
- Python-Wahl im echten Fluss VOR dem Bau: `Python-Kandidat verworfen (WindowsApps-Alias, NICHT gestartet):
  .../WindowsApps/python3`, dann `Python: /c/msys64/mingw64/bin/python3 (3.14.7)` (siehe Befund B1).
- Gradle: app/.cxx verworfen, `stageAssets` lief (nicht UP-TO-DATE, app/build war leer) und meldete
  `stageAssets: re15_port/shared_assets/RE2/DOOR: 27 x *.DO2` / `... RE15DOOR: 30 x *.DO2` (neuer doFirst im
  echten Lauf), `BUILD SUCCESSFUL in 3m 40s`, 54 Tasks.
- Gates auf `release/re15_port_v0.8.19_android.apk.ungeprueft`: Stichproben ok, `Inhalt: 3604 Asset-Eintraege`,
  aapt `versionCode='81900' versionName='v0.8.19'`, `native-code: 'arm64-v8a' 'x86_64'`;
  `== Volle Asset-Pruefung 1/2: Selbsttest ...` -> 28 x `[ok]` (3 x rc 0, 18 x rc 1, 7 x rc 2),
  `== SELBSTTEST-OK: 28/28 Faelle ==`; `== Volle Asset-Pruefung 2/2 ==` -> `Baumliste: 5 Baeume = build.gradle
  stageAssets`, Zaehlung je Baum:

  ```
  Baum (unter assets/)          Quelle     APK   gleich Bytes gleich
  shared_assets/PSX               3193    3193     3193    300522652
  shared_assets/extracted_fx        13      13       13      1079875
  shared_assets/RE2                277     277      277     25482039
  shared_assets/RE15DOOR            30      30       30      1914932
  synchro                           90      90       90     27678779
  SUMME                           3603    3603     3603    356678277
  RE2/DOOR:  Quelle 27, APK 27, sha256 gleich 27/27
  RE15DOOR:  Quelle 30, APK 30, sha256 gleich 30/30
  TORSE.VBS: Quelle 19176 B, APK 19176 B, sha256 gleich
  Manifest:  3603 Zeilen / 356678277 Bytes (Kopfzeile + Zeilen gegen die APK geprueft)
  == APK-ASSET-GATE-OK: 3603 Dateien in 5 Baeumen bytegleich, Manifest stimmt ==
  ```
  erst danach `mv` + SHA256SUMS (`e3b203ee...`, 363212403 B), `ANDROID-BUILD-OK`.
- **Laufzeit (aus den Zeitstempeln, `android_voll_zeiten.txt`):** Gradle 222,5 s; alte Gates (unzip/aapt)
  0,98 s; **Selbsttest 3,88 s; volle Pruefung 3,43 s; neue Gates zusammen 7,31 s** (= 3,1 % des Laufs von
  232,5 s); mv + sha256sum 1,48 s. (Maschine dabei nicht leer: Granaten-Sitzung lief parallel.)
- Die Gates veraendern die APK nicht: sha256 `app/build/outputs/apk/release/app-release.apk` ==
  `release/re15_port_v0.8.19_android.apk` == `e3b203ee76b795e5...`; keine `.ungeprueft` liegen geblieben.
- Aufraeumen: `release/SHA256SUMS_android.txt` per `git restore --source=HEAD` zurueck (danach sha256
  `cd139335...` = HEAD), `git status --short release/` leer. Die APK (gitignoriert, `release/*_android.apk`)
  bleibt fuer Schritt 3 liegen und wird danach geloescht.
- Python-Schnappschuss 1 (nach dem Bau) == Schnappschuss 0 (`diff` leer, `py_zustand_1_nach_android.txt`).

## 2. APK gegen Referenz-APK

Unabhaengiges Vergleichsskript `pruefer_echtlauf_r1_belege/apk_vergleich.py` (nutzt NICHT den Gate-Code;
`/c/Python310/python.exe` ausdruecklich), Referenz = Archiv-APK (nur gelesen), Ergebnis
`apk_vergleich_ergebnis.txt`, rc 0:
- Eintraege 3616 / 3616, Namen identisch; `assets/` 3604 / 3604, nur-Referenz 0, nur-neu 0.
- Alle 3604 `assets/`-Eintraege per sha256 des entpackten Inhalts verglichen (356842178 B inkl. Manifest):
  **0 Abweichungen** (Groesse, CRC, Methode, sha256); Stored 3604/3604 in beiden.
- Manifest sha256 beide `5f5acfdb062a8ee0...` (bytegleich).
- Ausserhalb `assets/`: 8 von 12 Eintraegen gleich (AndroidManifest.xml usw. — gleiche Version v0.8.19);
  anders sind NUR die vier nativen Bibliotheken (`lib/{arm64-v8a,x86_64}/{libmain,libSDL2}.so`). Ursache
  belegt (`apk_lib_unterschied.txt`): eingebauter Baupfad — Referenz enthaelt
  `C:/workspace/git/reAi_v2/.claude/worktrees/r33_integration/re15_...`, die neue APK
  `.../worktrees/r34a_android/re15_...` (3 Zeichen kuerzer -> libSDL2 16/32 B kleiner, libmain gleich gross mit
  verschobenen Bytes). Code-Seite, erwartet.
- Folgerung: Der neue Bauweg (build.gradle-Aenderung + neue Gates) nimmt der APK nichts weg und aendert kein
  Asset; die APK-Groesse ist sogar gleich (363212403 B).

## 3. make_package.sh echt

Vorbereitung (Archiv nur gelesen): Split-Saetze win64 + linux aus dem v0.8.19-Archiv nach
`build/r34a/pruefer_echtlauf_r1/pakete/` kopiert (`sha256sum -c` gegen die Archiv-SUMS: 4 x OK), per
`zip -s 0` zusammengefuehrt, daraus NUR die Binaries nach `release/win_out/re15_pc.exe` (sha256 `30d5b67b...`)
und `release/linux_out/re15_pc` (`abfbe7c5...`) entpackt. Die frisch gebaute APK aus Schritt 1 lag als
`release/re15_port_v0.8.19_android.apk` (`e3b203ee...`). KEIN Python-Shim, normaler Git-Bash-PATH.
Laeufe ueber `pruefer_echtlauf_r1_belege/mp_isoliert.sh`: make_package.sh unveraendert im Baum, nur die
git-SCHREIBzugriffe (git add der neuen Volumes) in Wegwerf-Index + Wegwerf-Objektspeicher gelenkt
(`GIT_INDEX_FILE`/`GIT_OBJECT_DIRECTORY`, alte Objekte ueber `GIT_ALTERNATE_OBJECT_DIRECTORIES` lesbar) —
sonst waeren ~480 MB Paket-Blobs als Muell im gemeinsamen `.git/objects` des Hauptbaums gelandet.
Probe: im Wegwerf-Index liefern `git ls-files release/re15_port_v0*` (6 Volumes) und `git log -1 -- re15_port/engine
re15_port/platform re15_port/include` (`f2d26986 20:55:26`) dasselbe wie im echten Index.
Logs: `pruefer_echtlauf_r1_belege/mp_3*.log`.

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| 3a | APK-Kopie ohne `assets/shared_assets/RE15DOOR/P07G.DO2` (`zip -d`, Manifest unveraendert) | **EXIT=1** nach 10 s: Python-Wahl, Selbsttest 28/28, dann `RE15DOOR: Quelle 30, APK 29`, `fehlt in der APK: assets/shared_assets/RE15DOOR/P07G.DO2`, `Manifest nennt ... (55908 B), die APK hat keinen Eintrag`, `ABBRUCH: Android-APK passt nicht zum Quellbaum (apk_asset_gate.py rc=1)` — VOR dem Kopieren; kein pkg-*, `git status release/` leer |
| 3b | echte APK, Binaries mit Archiv-Zeitstempel (19:34) | **EXIT=1**: Python-Wahl + APK-Pruefung gruen (`APK-ASSET-GATE-OK`), dann `ABBRUCH: Linux-Binary ist VERALTET ... stammt von 2026-09-29 19:34:13, der letzte Port-Code-Commit von 2026-09-29 20:55:26` (= f2d26986, die build.gradle-Aenderung dieses Zweigs; Befund B2) |
| 3c | wie 3b, Binaries per `touch` auf 21:24:59 (Bytes unveraendert, sha256 wie oben; PC-Code seit v0.8.19 unveraendert, Abschnitt 0) | **EXIT=0, `== Fertig ==`** (21:25:07-21:26:46, 99 s) |
| 3d-1 | nach 3c: 1 Byte in `release/pkg-linux/.../RE2/DOOR/DOOR04.DO2` gekippt (Groesse gleich), `--zip-only` (verwendet den Paketordner ohne neu zu kopieren) | **EXIT=1**: `ABBRUCH: RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR04.DO2` (neues cmp-Gate im echten Fluss) |
| 3d-2 | DOOR04 zurueck (cmp gleich), 1 Byte in `.../RE2/TORSE.VBS` gekippt, `--zip-only` | **EXIT=1**: `ABBRUCH: RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS`; danach zurueck (cmp gleich) |
| 3e-1 | `PATH=/usr/bin:<WindowsApps>`, `RE15_PYTHON_NUR_PATH=1` | **EXIT=1** nach 0,2 s: beide Aliase `verworfen (WindowsApps-Alias, NICHT gestartet)`, `ABBRUCH: kein echtes Python >= 3.8` — vor jedem anderen Schritt |
| 3e-2 | `PATH=/usr/bin:<WindowsApps>` (Rueckfall an) + APK aus 3a | **EXIT=1**: Aliase verworfen, `Python: /c/Python310/python.exe (3.10.11)`, Selbsttest 28/28 unter 3.10, APK-Pruefung findet P07G.DO2 -> Abbruch wie 3a |

Lauf 3c im Einzelnen (`mp_3c_voll.log`):
- `Python-Kandidat verworfen (WindowsApps-Alias, NICHT gestartet): .../WindowsApps/python3` ->
  `Python: /c/msys64/mingw64/bin/python3 (3.14.7)` (NICHT C:/Python310, Befund B1).
- `== Android-APK: Selbsttest + volle Asset-Pruefung gegen den Quellbaum ==`: Selbsttest 28/28 (4,7 s),
  volle Pruefung 3603/3603, RE2/DOOR 27/27, RE15DOOR 30/30, TORSE.VBS gleich (4,0 s) -> 9,4 s vor den Kopierminuten.
- Linux: Optimierungs-Gate (4 SDL2-Pfade), glibc 2.29, Kopie, check_tree `Tuerarchive im Paket: 27 x
  shared_assets/RE2/DOOR/*.DO2` (Schleife mit dem neuen cmp je Datei, kein Abbruch) + `Port-Tuerarchive im Paket:
  30 x`, TORSE.VBS-cmp ohne Abbruch, LF-Gate ok, Laufzeit-Gate uebersprungen (ELF auf Windows-Host, wie bisher).
- Windows: dieselben check_tree-Zeilen, Laufzeit-Gate `in_pkg: 26/26` + `foreign_cwd: 26/26`, alle aus dem Paket.
- Zippen: `verify_split` ueber `"$PY"`: linux `Volumes: 2 ... Katalog: 3800`, win64 `3801`, android `1`;
  `zip_exec_bit.py setzen/pruefen` ueber `"$PY"`: `re15_pc 100644 -> 100755`, `x-Bit ok: re15_pc 100755`,
  `run.sh 100755`. SHA256SUMS.txt geschrieben, `Git: ... 0 alte Paketdatei(en) entfernt, 6 neue vorgemerkt`
  (im Wegwerf-Index; der echte Index blieb unberuehrt: dort nur ` M` ungestaged).
- **Paketinhalt gegen die Archiv-Pakete v0.8.19** (zusammengefuehrt, `paket_vergleich.py`: Name, Groesse, CRC,
  Unix-Modus; Zeitstempel ausgenommen; `paket_vergleich_ergebnis.txt`): win64 3801/3801 und linux 3800/3800
  Eintraege **identisch** (0 nur-alt, 0 nur-neu, 0 anders); android: 1 Eintrag, nur die APK anders (neu gebaut,
  Abschnitt 2).
- Python-Schnappschuss 2 (nach 3c) und 3 (nach 3a-3e) == Schnappschuss 0 (`diff` leer): kein Installer, kein
  neuer Registry-Schluessel unter HKCU/HKLM `Software\Python`, kein neuer Startmenue-Eintrag, `%LOCALAPPDATA%\Python`
  unveraendert (2 Eintraege, neuester 15:39:13), kein `release/Python`, kein `python_install*`-Log.

Aufraeumen: `git restore --source=HEAD -- release/SHA256SUMS.txt release/re15_port_v0.8.19_*.{z01,zip}`
(danach `sha256sum -c` 6/6 OK, identisch mit der Archiv-SUMS; SHA256SUMS.txt sha256 `9432f742...` = Stand vor den
Laeufen), `release/pkg-linux`, `pkg-win`, `win_out`, `linux_out` und die APK geloescht; `git status --short release/`
leer.

## 4. Linux-Lauf

Docker Desktop lief (Server 27.5.1). Baum und Archiv NUR LESEND eingehaengt
(`-v <baum>:/src:ro -v <archiv v0.8.19>:/archiv:ro`), Faelschungen nur im Container unter /tmp.
Skripte + Logs: `pruefer_echtlauf_r1_belege/linux_lauf{,2}.{sh,log}` (gestartet als
`/src/build/r34a/pruefer_echtlauf_r1/linux_lauf*.sh`).

`re15-linux-build:deb11` (Debian 11.11, Python 3.9.2, bash 5.1.4; kein unzip, kein cygpath; `OSTYPE=linux-gnu`):
| Lauf | Ergebnis |
|---|---|
| Referenz-APK im Archiv | sha256 `514bebd5...` = Archiv-SUMS |
| L1 `bash release/python_finden.sh` | `Python: /usr/bin/python3 (3.9.2)`, rc 0 |
| L2 `python3 release/apk_asset_gate.py --selbsttest` | `SELBSTTEST-OK: 28/28`, rc 0, 6,3 s |
| L3 `python3 release/apk_asset_gate.py --repo /src /archiv/...apk` | `APK-ASSET-GATE-OK: 3603 Dateien in 5 Baeumen bytegleich`, Zaehlung wie unter Windows (PSX 3193, fx 13, RE2 277, RE15DOOR 30, synchro 90; RE2/DOOR 27/27, RE15DOOR 30/30, TORSE.VBS gleich), rc 0, 57 s |
| L4 Negativ: im Container gebaute Kopie ohne `RE15DOOR/P2DS.DO2` | rc 1, `fehlt in der APK: assets/shared_assets/RE15DOOR/P2DS.DO2` + Manifestzeile ohne Eintrag |
| L5 Negativ: `--repo /tmp` | rc 2, `build.gradle fehlt: /tmp/re15_port/platform/android/app/build.gradle` |
| L6 `source python_finden.sh` unter `set -euo pipefail`, Gate ueber `"$PY"` | `PY=/usr/bin/python3`, `APK-ASSET-GATE-OK`, rc 0 |
| L7 `build_android.sh --gate-only` | nicht moeglich: `unzip` fehlt im Image (Voraussetzung der ALTEN Stichproben-Gates, `unzip -Z1`) -> L7 in einem Image mit unzip |

`re15-deck:latest` (Steam Runtime 3 "sniper", Python 3.9.2, unzip vorhanden):
| Lauf | Ergebnis |
|---|---|
| L7 `bash release/build_android.sh --gate-only /archiv/...apk --version v0.8.19` | `Python: /usr/bin/python3 (3.9.2)`, `(Android-SDK-Ordner fehlt ... ohne aapt)`, Stichproben + `3604 Asset-Eintraege`, `(aapt nicht gefunden - badging-Gate uebersprungen)`, Selbsttest 28/28, volle Pruefung OK, `== ANDROID-GATES-OK (--gate-only) ==`, rc 0, 36 s |
| L7n dasselbe mit der Kopie ohne P2DS.DO2 | Selbsttest 28/28, dann `fehlt in der APK: ...P2DS.DO2`, `ABBRUCH: APK-Asset-Gate: die APK weicht vom Quellbaum ab`, rc 1 |

Folgerung: Gate, Selbsttest, python_finden.sh und der Gate-Teil von build_android.sh laufen unter Linux
unveraendert (Pfade ohne cygpath, `nativ_pfad` = printf). Die Laufzeiten 31-57 s sind der bekannte WSL-9p-Mount
(nativ unter Windows 3,2-4,1 s fuer dieselbe Pruefung), kein Gate-Effekt. `apk_asset_gate.py` ist im Git 100644
und im Blob LF; die Aufrufer starten es immer als `"$PY" apk_asset_gate.py` (kein direkter Aufruf noetig).

## Befunde

- **B1 (niedrig) — python_finden.sh waehlt auf dieser Maschine NICHT C:/Python310, sondern das MSYS2-Python
  3.14.7.** Beleg: `android_voll_auszug.log` Z.3 und `mp_3c_voll.log`: `Python: /c/msys64/mingw64/bin/python3
  (3.14.7)`. Grund: Kandidaten-Reihenfolge = zuerst jedes `python3` im PATH (WindowsApps verworfen, dann
  `/c/msys64/mingw64/bin/python3`), die `/c/Python3*`-Ordner erst als Rueckfall. Das ist ein echtes Python (pacman,
  Abhaengigkeit von gdb, laut Bauer-Dossier seit 2026-08-24), KEIN Installer (Schnappschuesse 0-3 gleich), und alle
  Laeufe waren gruen. Aber: (a) die Erwartung im Auftrag nennt C:/Python310; (b) die Meldung "3.14.7" sieht aus wie
  der v0.8.17-Unfall ("Paketbau installierte Python 3.14") und kann bei jedem Paketbau Rueckfragen ausloesen;
  (c) die Version wandert mit jedem `pacman -Syu`. Der Rueckfall auf `/c/Python310/python.exe (3.10.11)` funktioniert
  (3e-2). Wer 3.10 fest will: `RE15_PYTHON=/c/Python310/python.exe` oder unter Windows die `/c/Python3*`-Kandidaten
  vor die PATH-Kandidaten stellen.
- **B2 (niedrig) — Der build.gradle-Commit dieses Zweigs macht die vorhandenen PC-Binaries fuer make_package
  "VERALTET".** Beleg `mp_3b_frische.log`: `ABBRUCH: Linux-Binary ist VERALTET ... stammt von 2026-09-29 19:34:13,
  der letzte Port-Code-Commit von 2026-09-29 20:55:26` (= f2d26986 `build.gradle stageAssets`). check_binary_fresh
  zaehlt `re15_port/platform` komplett, also auch `platform/android/app/build.gradle`, obwohl der PC-Code seit v0.8.19
  unveraendert ist (`git diff --stat v0.8.19..HEAD -- re15_port/engine re15_port/platform re15_port/include` = nur
  build.gradle). Nach dem Merge verlangt jeder Paketlauf frisch gebaute Windows-/Linux-Binaries (beim naechsten
  Release ohnehin Pflicht, beim reinen Nachpacken ein unnoetiger Neubau). Kein Defekt der neuen Pruefung.
- **B3 (niedrig) — `release/<name>.apk.ungeprueft` ist nicht gitignoriert.** Beleg: `git check-ignore -v
  release/re15_port_v0.8.19_android.apk.ungeprueft` -> rc 1 (`.gitignore:63` deckt nur `release/*_android.apk`).
  Nach einem roten Gate bleibt dort eine ~363-MB-Datei als "untracked" liegen (Bauer-Dossier 3.7: "nur
  .apk.ungeprueft"), bis der naechste build_android.sh-Lauf sie loescht; ein `git add release/` wuerde eine Datei
  ueber GitHubs 100-MB-Grenze vormerken. make_package zippt sie nicht (sucht exakt `<name>_android.apk`).
- **B4 (niedrig, schon vorher so) — Das APK-Gate in make_package prueft nur Assets, nicht, ob die APK zum
  aktuellen CODE passt.** Beleg: make_package.sh:422-432 ruft nur `apk_asset_gate.py`; fuer die APK gibt es kein
  Gegenstueck zu check_binary_fresh und keine versionName-Pruefung. build_android.sh:323 loescht die vorige
  `<name>.apk` erst NACH erfolgreichem Gradle-Lauf; bricht Gradle ab (:312), bleibt die alte APK unter dem
  Auslieferungsnamen liegen (wie in a358fd5d, :209). Eine solche alte APK mit unveraenderten Assets besteht die neue
  Pruefung und wird gezippt — z.B. besteht die Archiv-APK v0.8.19 (gebaut vor f2d26986) die Pruefung gegen den
  heutigen Baum (L3/L7, Bauer 3.1). Der Kommentar make_package.sh:418-419 nennt genau diesen Fall ("die APK waere dann
  veraltet und wuerde trotzdem gezippt"), abgedeckt ist davon nur die Asset-Haelfte.
- **Altlast (niedrig, nicht diese Aenderung)** — verwaister HKCU-Uninstall-Eintrag `pymanager-pythoncore-3.14-64`
  ("Python 3.14.7", InstallLocation im geloeschten `r31_integration/release/Python`), siehe 0.1. Die Entfernung vom
  2026-09-29 hat ihn uebersehen.

Widerlegt / bestaetigt ohne Befund:
- Die neuen Gates laufen im echten Android-Bau und brechen im Fehlerfall wirklich ab (hier nicht wiederholt, Bauer
  3.7; im echten make_package-Fluss von mir 3a/3e-2 belegt).
- Die neuen cmp-Gates in check_tree greifen im echten Fluss (3d-1 DOOR04.DO2, 3d-2 TORSE.VBS).
- Kein Python-Installer in irgendeinem Lauf: Schnappschuesse 0/1/2/3 identisch (121 Zeilen inkl. Registry-Werten).
- Keine Selbsttest-Temp-Ordner liegen geblieben (`%TEMP%\apk_gate_selbsttest_*`: 0 nach 6 Windows-Selbsttests).
- Die Gates veraendern die APK nicht (sha256 Gradle-Ausgabe == ausgelieferte APK).

## Urteil

**haltbar.** Der echte Android-Bau lief mit den neuen Gates gruen (EXIT=0, ANDROID-BUILD-OK; Selbsttest 28/28 in
3,9 s, volle Pruefung 3603/3603 mit Zaehlung je Baum in 3,4 s), die gebaute APK hat dieselben 3604 Asset-Eintraege
bytegleich wie die Referenz-APK, make_package.sh lief ohne Shim und ohne Installer durch (Paketinhalt win64/linux
identisch mit v0.8.19) und bricht in allen vier echten Negativfaellen ab, und Gate + Selbsttest + python_finden +
`build_android.sh --gate-only` funktionieren unter Linux. B1-B4 sind niedrig und blockieren die Uebernahme nicht.
