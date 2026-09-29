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

(laufend fortgeschrieben)
- 21:1x Dossier angelegt, Commits/Bauer-Dossier/Werkzeuge gelesen, Python-Schnappschuss 0.
- 21:16:57-21:20:50 voller Android-Bau EXIT=0 ANDROID-BUILD-OK (Abschnitt 1); 21:22 APK-Vergleich rc 0 (Abschnitt 2).
- 21:23-21:30 make_package.sh: 3a-3e (Abschnitt 3), release/ danach auf HEAD, Artefakte weg.

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

## Befunde

## Urteil
