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

## 4. Linux-Lauf

## Befunde

## Urteil
