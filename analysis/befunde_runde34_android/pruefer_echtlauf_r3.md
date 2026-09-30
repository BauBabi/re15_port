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

## 3. make_package.sh echt (ohne Python-Shim, ohne touch)
Vorbereitung `binaries_vorbereiten_r3.sh` (Log `build/r34a/pruefer_echtlauf_r3/binaries_vorbereiten.log`; Archiv nur
gelesen): Split-Saetze win64 + linux nach `build/r34a/pruefer_echtlauf_r3/pakete/` kopiert, `sha256sum -c` gegen die
Archiv-SUMS 4 x OK, per `zip -s 0` zusammengefuehrt, NUR die Binaries entpackt (unzip stellt die Original-mtime her), per
`cp -p` nach `release/win_out/re15_pc.exe` (`30d5b67b...`, 19:34:23) und `release/linux_out/re15_pc` (`abfbe7c5...`,
19:34:13) - KEIN touch. Letzter PC-Code-Commit (Standardpfade ohne platform/android) cf386e32 19:02:06; letzter Commit an
den APK-Pfaden 9d2337e4 01:30:03 (build.gradle R2). APK = der Bau aus Abschnitt 1 (`b7bfb699...`, 03:32).
Git-Isolation `mp_isoliert_r3.sh`: make_package.sh UNVERAENDERT, nur die git-Schreibzugriffe am Ende gehen in einen
Wegwerf-Index/-Objektspeicher (Alternates) - den echten Index benutzt der parallele Gegenpruefer zum Committen. PATH
unveraendert (normaler Git-Bash-PATH: /c/Python310 an Stelle 12, WindowsApps an 35; `type -a python3` = WindowsApps zuerst).

### 3a Normaler Lauf `bash release/make_package.sh --version v0.8.19` (03:35:10-03:37:20): **EXIT=0, `== Fertig ==`**
Log `build/r34a/pruefer_echtlauf_r3/mp_voll.log`, Kernzeilen `pruefer_echtlauf_r3_belege/mp_voll_kern.txt`, Zeiten
`mp_voll_zeiten.txt`.
- `Python: /c/Python310/python (3.10.11)` (0,42 s nach Start) - der Finder waehlt ein echtes Python.
- VOR den Kopierminuten: `--quellbaum` (0,85 s) `APK-ASSET-GATE-QUELLBAUM-OK: 3603 Dateien in 5 Baeumen, Tuer-Soll
  erfuellt` (RE15DOOR 30/30, RE2/DOOR 27/27); APK-Kette 21,6 s: Frische ohne Meldung (APK 03:32 > 01:30), Werkzeuge
  35.0.0, Pruefkopie, aapt `versionName='v0.8.19'` + beide ABIs, zipalign ok, apksigner v2 true / 1 Signer `432bc749...`,
  Selbsttest 202/202 + 58/58 (11,7 s), Gate 3603/3603 je Baum (4,8 s), `gepruefte APK: b7bfb699... 6d7117f4 363212403`.
- **PC-Binaries mit Archiv-Zeitstempel 19:34 ohne "VERALTET"**. Linux: Optimierungs-Gate (4 SDL2-Pfade), glibc 2.29,
  Kopie, check_tree: **`Tuerarchive im Paket: 27 x shared_assets/RE2/DOOR/*.DO2`** (Schleife mit dem neuen cmp je Datei),
  `30 x RE15DOOR`, TORSE.VBS-cmp ohne Abbruch, dann `--paket` `APK-ASSET-GATE-PAKET-OK: 3603 Dateien ... bytegleich, nichts
  zusaetzlich` (6,6 s), LF-Gate, `OK: 349M, 3606 Dateien`. Windows: dieselben check_tree-Zeilen (27 + 30), `--paket` OK
  (6,8 s), Laufzeit-Gate `in_pkg 26/26` + `foreign_cwd 26/26` aus dem Paket, `3607 Dateien`.
- Zippen ueber `"$PY"`: linux `Volumes: 2, Katalog 3800`, x-Bit `re15_pc 100644 -> 100755` (zurueckgelesen 100755),
  win64 `3801`, android `(APK 347M, = gepruefte Datei)`, `Katalog 1`, **`APK im Split-Satz = gepruefte APK (CRC32 6d7117f4,
  363212403 B)`**; SHA256SUMS.txt (`sha256sum -c` 6/6 OK, Beleg `SHA256SUMS_nach_make_package.txt`); Git im Wegwerf-Index:
  `0 alte ... entfernt, 6 neue vorgemerkt`. **Echter Index danach leer** (`git diff --cached --stat` ohne Ausgabe); der
  Wegwerf-Index hat genau die 6 Volumes.
- Laufzeit: gesamt 129 s; neu in dieser Runde davon --quellbaum 0,85 s + APK-Kette 21,6 s + 2 x --paket 13,4 s.
- **Paketinhalt gegen die Archiv-Pakete** (`paket_vergleich_r3.py`, zusammengefuehrt, Name/Groesse/CRC32/Unix-Modus;
  `paket_vergleich_r3_ergebnis.txt`): win64 3801/3801 und linux 3800/3800 **identisch** (Modi re15_pc/run.sh 100755);
  android 1 Eintrag, nur die APK anders (Neubau), sha256 des Eintrags im neuen Satz = `b7bfb699...` = die gepruefte APK.
- **Python-Schnappschuss 2** (03:37:47, `-seit 03:20`) == Schnappschuss 0 (`Compare-Object` leer): kein Unterschluessel
  unter `HKCU\Software\Python\PythonCore`, kein Startmenue-Ordner `Python 3.1x` (Benutzer), keine neue Datei in
  `%LOCALAPPDATA%\Python`, `...\Programs\Python`, `...\WindowsApps`, Startmenue; kein Installer-Prozess.
- Rezept-PATH aus der Memory (`PATH="/c/msys64/usr/bin:$PATH"`): `python_finden.sh` waehlt auch dann `/c/Python310/python`
  (in msys64/usr/bin liegt kein Python). `grep` ueber release/*.sh und release/docker/*: kein direkter
  `python3`/`python`/`py`-Aufruf mehr ausser ueber den Finder.

### 3b Die neuen cmp-Gates im echten Fluss (`mp_neg_cmp_r3.sh`, `--zip-only` auf die Paketordner aus 3a, isoliert)
Je Fall EIN Byte (Groesse gleich) in einer Paketdatei gekippt, andere Dateien als in r2 (dort DOOR04/DOOR36); danach aus
dem Quellbaum zurueck (cmp 0). Logs `mp_neg_N1_linux_door13.log`, `mp_neg_N2_win_torse.log`, Uebersicht `mp_neg_cmp.out`.
| Fall | EXIT | Abbruch (woertlich) | vorher |
|---|---|---|---|
| N1 `--only linux`, pkg-linux RE2/DOOR/DOOR13.DO2 @1500 | **1** (33,5 s) | `ABBRUCH: RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR13.DO2` | Quellbaum-OK, APK-Kette OK; 0 x `== Zippen` |
| N2 `--only win`, pkg-win RE2/TORSE.VBS @9000 | **1** (33,4 s) | `ABBRUCH: RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS` | Quellbaum-OK, APK-Kette OK; 0 x `== Zippen` |
(Die cmp-Gates brechen VOR dem `--paket`-Gate ab; dass `--paket` einen gekippten Byte in einer Paketdatei ebenfalls
faengt, zeigt unter Linux L11b in Abschnitt 4 - unter Windows im echten Fluss hier nicht eigens gemessen.)

### 3c Stolperdraht: ruft der ENDSTAND von make_package.sh Python am Finder vorbei? (`stolperdraht_r3.sh`)
Der Bauer hat seinen Stolperdraht (R2-3.9) nur mit `build_android.sh --gate-only`, dem Direktaufruf des Gates und
zip_exec_bit.py gefahren - NICHT mit make_package.sh, das in R2 neue Python-Aufrufe bekam (`--quellbaum`, `--paket` x 2,
`apk_kopie_mit_kennung`, `apk_kennung` x 3, `verify_apk_im_zip`). Deshalb ein voller make_package-Lauf (03:40:19-03:42:36,
isoliert) mit PATH = `<draht>:/usr/bin:/mingw64/bin:<WindowsApps>:<Rest>`; im Draht `python3`/`python`/`py`, die jeden
Aufruf samt Elternprozess protokollieren und mit rc 97 scheitern (kein Shim - sie lassen nichts gelingen). `type -a
python3`: Draht, dann WindowsApps. Ergebnis (`stolperdraht_lauf.out`, `stolperdraht.log`, `mp_stolper.log`):
- Finder: Draht-`python3` und -`python` `verworfen (Pruefung rc=97 ...)`, dann `WindowsApps/python3` und `WindowsApps/python`
  **`verworfen (WindowsApps-Alias, NICHT gestartet)`**, gewaehlt `/c/Python310/python (3.10.11)`.
- **EXIT=0 `== Fertig ==`** (Quellbaum-OK, Selbsttest 202/202, beide PAKET-OK, x-Bit ok, Kataloge 3800/3801/1, `APK im
  Split-Satz = gepruefte APK`).
- **Drahtprotokoll: genau 2 Eintraege**, beide `timeout 30 <draht>/python{3,} -c import sys, zipfile, hashlib; ...` = die
  Probe des Finders. **Kein Umgehungsaufruf** im ganzen Endstand-Lauf.
- Python-Schnappschuss 3 (03:42:50) == Schnappschuss 0.

### 3d Aufraeumen release/ (03:43)
`git restore --source=HEAD` fuer SHA256SUMS.txt + 6 Split-Volumes; `pkg-linux`, `pkg-win`, `win_out`, `linux_out` und die
APK geloescht (Kopie `build/r34a/pruefer_echtlauf_r3/neu_v0.8.19.apk` fuer Abschnitt 4). Danach 8/8 versionierte
release/-Dateien sha256 = Stand vor den Laeufen (`release_vorher.sha256` == `release_nachher.sha256`), `git status --short
release/ re15_port/` leer, `git status --ignored release/` ohne Reste, echter Index leer.

## 4. Linux (Docker)
(folgt)

## Befunde
(folgt)

## Urteil
(folgt)
