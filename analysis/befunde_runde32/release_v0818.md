# Release v0.8.18 — Dossier (laufend fortgeschrieben)

Arbeitsbaum: `.claude/worktrees/r32_integration`, Zweig `r32/integration`, Basis `a0b500d1`.
Auftrag: Paket v0.8.18 fuer Windows + Linux/Steam Deck + Android, Checkliste, unabhaengige
Artefakt-Pruefung (DOOR13-Mitte, Tor hell), Archiv, Release-Commit (kein Tag, kein Push, kein Merge).
Vorbild: `analysis/befunde_runde31/release_v0817.md` (+ `.pruefer.md`), Release-Commit e714d95f.

## Ablauf
- [x] 1. Skripte lesen, Docker pruefen
- [x] 2. Windows-Cross, Linux/Deck, Android bauen
- [x] 3. make_package.sh --version v0.8.18 (Python-Shim vor PATH)
- [x] 4. Checkliste
- [x] 5. Unabhaengige Artefakt-Pruefung (Windows-Paket ausserhalb des Repos)
- [x] 6. Archiv
- [x] 7. Release-Commit

## Protokoll

### 1. Skripte + Umgebung (2026-09-29 15:07)
- Stand a0b500d1; letzter Port-Code-Commit (re15_port/) 69e636a6 2026-09-29 14:52:44 -> Frische-Schranke.
- Port-Aenderungen seit v0.8.17 (eb10ceba): 26 Dateien, u. a. neu `engine/src/door_seq_zeichnen.c`,
  geaendert `engine/src/gen/tor_1170_door.inc`, `engine/src/sicherung_1150.c`, `platform/pc/src/door_scene_pc.c`.
  (hebetisch_1150.c ist NICHT neu in dieser Runde — seit v0.8.17 drin; neu ist nur door_seq_zeichnen.c.)
- Docker Desktop lief NICHT (Pipe dockerDesktopLinuxEngine fehlte) -> 15:08:18 gestartet, 15:08:38 bereit;
  27.5.1, 16 CPUs, 0 laufende Container; Images re15-linux-build:deb11 / re15-wincross-build:deb11 vorhanden.
- Host frei: keine gcc/cc1/ninja/ctest/cmake/re15/gradle-Prozesse, CPU-Last 31 %.
- `RE15_MIN_TESTS` in re15_port/tools/local_build.sh = 416 (docker_linux_build.sh liest es als Untergrenze).
- Python-Shim `build/py3shim/{python3,python}` -> `exec /c/Python310/python` (3.10.11); mit
  `PATH="$(pwd)/build/py3shim:/c/msys64/usr/bin:$PATH"`: `command -v python3/python` = Shim, zip = /usr/bin/zip.
  make_package.sh ruft python3 (verify_split, Zeile 422) und python (zip_exec_bit.py, 520/523); die Bau-Skripte
  rufen kein python.
- Bauten seriell in einer Kette (`build/rel0818/kette.sh`): Windows-Cross -> Linux -> Android, je Log
  `build/rel0818/{win,linux,android}.log` mit eigener `EXIT=`-Zeile (kein Pipe).

### 2a. Windows-Cross (bash release/build_win_cross.sh)
- Lauf 15:08:58 -> 15:09:48, EXIT=0, `WIN-CROSS-BUILD-OK: 4358987 Bytes`; Phasen: Packen 4 s (4693 Dateien,
  335 MB, 104 Rueckfall-Links), Auspacken 7 s, Configure 21 s, Compile+Link 13 s, gesamt 34 s im Container
  (Wrapper 49 s).
- Artefakt `release/win_out/re15_pc.exe`: 4358987 B, mtime 2026-09-29 15:09:47 (juenger als Port-Commit 14:52:44),
  sha256 `1faaf0fdcaf5ff6c9bc50eb3a6531932aa9b7b8b9cf0cccae87a3195786ddf11`.
- Groesse gegen v0.8.17 (4354858 B, Release): +4129 B (+0,09 %) bei einer neuen Datei door_seq_zeichnen.c
  (293 Zeilen, Dreiecksschleife aus door_scene_pc.c dorthin verschoben) und reinen Datenaenderungen im Tor-.inc
  -> plausibel, Release (Debug waere ~5,8 MB).
- PE-Header: e_lfanew 0x80, `PE\0\0`, Magic 0x20b, Subsystem 2 (GUI), Zeitstempel 2026-09-29 13:09:47 UTC.
- Neuer Code im Build (nm, mingw64): `T re15_door_divide_gt3`, `T re15_door_mesh_zeichnen`, `T re15_door_rtpt`,
  `t teil_ausgeben` (alle aus door_seq_zeichnen.c).

### 2b. Linux/Deck (bash release/build_linux_deck.sh)
- Lauf 15:09:48 -> 15:18:42, EXIT=0, `LINUX-BUILD-OK`. Phasen: Packen 16 s (18657 Dateien, 553 MB,
  359 Rueckfall-Links), Auspacken 13 s, Configure 12 s, Compile+Link 20 s, ctest 466 s (seriell), gesamt 500 s
  (Wrapper 533 s).
- Suite: `100% tests passed, 0 tests failed out of 416`, `Tests: 416 gelaufen = 416 registriert (Untergrenze 416)`,
  Total Test time 466.44 s. Fingerabdruck 416 Tests, alle Passed; einzige echte SKIP-Zeile wie v0.8.17:
  integration_r30_titel_puls (Bild-Teil ohne GPU, RE15_OHNE_GPU); die uebrigen .skip-Treffer sind PASS-Texte.
- Neue Tests dieser Runde im Container gruen: unit_r32_hebetisch_faecher, unit_r32_tor_hell,
  integration_r32_tor_hell (echte Linux-exe: `R1 Schild ... F 28.742, gemalt 28,514 -> 1.0080`),
  unit_r32_unterteilung_referenz, unit_r32_unterteilung_tor.
- Artefakt `release/linux_out/re15_pc`: 3812760 B (v0.8.17: 3808368 B, +4392 B), mtime 15:18:40,
  sha256 `5aaabaa022a90334ebfcfbb32184566facbb7bccdd036dbc38679e9f206d4b48`.
- `glibc_max.txt` = GLIBC_2.29; `ldd.txt` 0x "not found". nm: `T re15_door_divide_gt3`,
  `T re15_door_mesh_zeichnen`, `T re15_door_rtpt` (wie Windows).

### 2c. Android (bash release/build_android.sh --version v0.8.18)
- Lauf 15:18:42 -> 15:21:38, EXIT=0, `ANDROID-BUILD-OK`; Gradle `BUILD SUCCESSFUL in 2m 45s`, 54 Tasks executed.
- `== CMake-Cache des NDK-Baus verworfen (GLOB neu auswerten) ==` im Protokoll; neuer .cxx-Hash `6d1m5z70`.
- Neue/geaenderte Quellen je ABI uebersetzt (app/.cxx/RelWithDebInfo/6d1m5z70):
  arm64-v8a: door_seq_zeichnen.c.o 77232 B (15:20:06), sicherung_1150.c.o 51200 B, door_scene_pc.c.o 69408 B;
  x86_64: 56744 B (15:21:02) / 50072 B / 62728 B.
- Symbole in libmain.so AUS DER APK (llvm-nm -D, NDK r27c 27.2.12479018): je ABI `T re15_door_divide_gt3`,
  `T re15_door_mesh_zeichnen`, `T re15_door_rtpt`; 1199 T-Symbole je ABI (v0.8.17: 1196, +3 = genau diese drei);
  weiterhin 18 `re15_door_seq_*`, 6 `re15_hebetisch_*`. arm64 libmain.so 1315008 B (v0.8.17 1310640),
  x86_64 1375240 B (v0.8.17 1370168).
- aapt: package `de.re15.port`, versionCode 81800, versionName v0.8.18, sdk 24 / target 35,
  native-code arm64-v8a + x86_64.
- Assets: re15_assets.txt 3570 Dateien / 354579777 B; APK 3571 Asset-Eintraege (wie v0.8.17).
  `assets/shared_assets/RE2/DOOR/*.DO2` = 24 + `RE2/TORSE.VBS`; entpackt, sha256 gegen re15_port/shared_assets/RE2:
  25/25 OK. DOOR13.DO2 stored, 54532 B.
- APK `release/re15_port_v0.8.18_android.apk`: 361068261 B (v0.8.17: 361046809 B), mtime 15:21:36,
  sha256 `c7e4aa5b189ce108ab8f3ac3774e151046f5007205f78896aac559f1e0f0a9c7` (= SHA256SUMS_android.txt).
- Hinweis: `unzip <apk> 'lib/*'` meldete unter MSYS "filename not matched" (Musterverhalten des Werkzeugs), mit
  expliziten Namen `lib/<abi>/libmain.so` rc=0 — `unzip -Z1` listet beide libmain.so + libSDL2.so.

### 3. Paket (PATH="$(pwd)/build/py3shim:/c/msys64/usr/bin:$PATH" bash release/make_package.sh --version v0.8.18, ohne --only)
- 15:22 -> 15:24:39, EXIT=0. Gates im Lauf: Optimierung Linux 4 / Windows 3 SDL2-Quellpfade (Release); glibc
  GLIBC_2.29; "Tuerarchive im Paket: 24 x shared_assets/RE2/DOOR/*.DO2" (beide); LF-Gate gruen; Laufzeit-Gate
  Windows in_pkg 26/26 + foreign_cwd 26/26 "alle aus dem Paket"; Linux-Paket 3573 Dateien / 347M, Windows
  3574 / 348M; Split-Kataloge 3766 / 3767 / 1 Eintraege, je 2 Volumes; x-Bit re15_pc 100644 -> 100755 gesetzt und
  zurueckgelesen (re15_pc + run.sh 100755). Frische-Gate (Binary juenger als 69e636a6) bestanden.
- Git: "6 alte Paketdatei(en) aus dem Repo entfernt, 6 neue vorgemerkt" (alle drei Plattformen v0.8.17 -> v0.8.18,
  `git status`: 6x R + SHA256SUMS.txt/SHA256SUMS_android.txt M).
- **Python-Falle entschaerft:** Shim vorn im PATH (vorab mit identischem PATH `command -v python3/python` = Shim,
  Python 3.10.11). Nach dem Lauf KEINE Installer-Spuren: kein `release/Python/`, kein `release/python_install_*.log`,
  kein `%LOCALAPPDATA%/Python`, kein `%LOCALAPPDATA%/Programs/Python`, keine pymanager-Datei neuer als der Bau.
  (Die zwei Vorspannzeilen, die ich vor dem Aufruf in `build/rel0818/pkg.log` schrieb, fehlten danach im Log — Ursache
  nicht geklaert, auf das Paket ohne Einfluss; die Shim-Wirkung ist ueber die fehlenden Installer-Spuren belegt.)
- Pakete (release/): android .z01 94371840 + .zip 74030921; linux .z01 94371840 + .zip 74152050;
  win64 .z01 94371840 + .zip 74260125.

### 4. Checkliste (gemessen am AUSGELIEFERTEN Artefakt: Split-Saetze mit `zip -s 0` zusammengefuehrt + entpackt, ausserhalb des Repos)
| Pruefung | Messwert | Ergebnis |
|---|---|---|
| sha256 Paket == Bau, Windows-exe | beide `1faaf0fdcaf5ff6c9bc50eb3a6531932aa9b7b8b9cf0cccae87a3195786ddf11` | OK |
| sha256 Paket == Bau, Linux-Binary | beide `5aaabaa022a90334ebfcfbb32184566facbb7bccdd036dbc38679e9f206d4b48` (Paket im debian:11-Container entpackt) | OK |
| sha256 Paket == Bau, APK | beide `c7e4aa5b189ce108ab8f3ac3774e151046f5007205f78896aac559f1e0f0a9c7` (APK aus dem Android-Split-Zip) | OK |
| PE-Subsystem | Magic 0x20b, Subsystem 2 (Paket-exe = Bau-exe per sha256) | OK |
| Laufzeit-Gate aus dem Paket | make_package in_pkg 26/26 + foreign_cwd 26/26; zusaetzlich Kopie `r18_art.exe` im ENTPACKTEN Paket, fremdes cwd, RE15_ASSET_ROOT/CD_ROOT/RE2_ASSET_ROOT entfernt: `[selftest] RESULT ok=26 missing=0`, 0 Treffer ausserhalb (18 PSX, 5 extracted_fx, 2 RE2, 1 synchro); Wurzeln cd-root[0] = Paket, cd-root[1] = /src/... (Container-Pfad, auf dem Host nicht vorhanden) | OK |
| keine Logs im Paket | Katalog-Suche `.log/debug.log/befund/.dmp/trace/ctest/LastTest/re15_card/python`: win 0 (3767), linux 0 (3766), android 0 (1); Wurzel nur exe/2 .bat/README bzw. re15_pc/run.sh/README | OK |
| LF-Gate Linux-Paket | make_package gruen; im Container: run.sh 0 CR, Shebang `#!/usr/bin/env bash`, `file`: "Bourne-Again shell script, UTF-8 Unicode text executable" | OK |
| x-Bit entpackt (debian:11, UnZip 6.00) | Split-Satz IM Container zusammengefuehrt + entpackt: `-rwxr-xr-x re15_pc`, `-rwxr-xr-x run.sh`, Zentralverzeichnis beide `-rwxr-xr-x 3.0 unx`; Direktstart `./re15_pc --headless` rc=1, `./run.sh --headless` rc=1; **Gegenprobe** dieselbe Datei mit Modus 644: rc=126 | OK |
| GLIBC-Untergrenze | objdump -T am Paket-Binary im Container: max GLIBC_2.29 (<= 2.31); `ldd` 0x "not found" im nackten debian:11 | OK |
| RE2/DOOR/*.DO2 + TORSE.VBS | Windows 24+1, Linux 24+1, APK 24+1; sha256 gegen re15_port/shared_assets/RE2: je 25/25 OK; ganzer RE2-Baum (274 Dateien) in Windows + Linux bytegleich | OK |
| Groesse Windows-exe | 4358987 B gegen v0.8.17 4354858 B (+4129 B, +0,09 %); Release (3 SDL2-Pfade), Debug waere ~5,8 MB | OK |
| Neuer Code im ausgelieferten Code | exe + Linux (nm): `re15_door_divide_gt3`, `re15_door_mesh_zeichnen`, `re15_door_rtpt` als T; APK: dieselben drei je ABI (+3 T-Symbole gegen v0.8.17) | OK |

### 5. Unabhaengige Artefakt-Pruefung (Windows-Paket im Scratchpad, ausserhalb des Repos)
- Quelle: `release/re15_port_v0.8.18_win64.z01/.zip` (sha256 145b8a96... / 4b936f9b...) nach
  `<scratchpad>/rel0818_art/win` kopiert, `zip -s 0` rc=0, `unzip` rc=0, 3574 Dateien. Lauf-exe als Kopie
  `r18_art.exe` IM Paketordner (eigener Bildname; exe-Verzeichnis-Anker bleibt das Paket). Je Lauf eigenes fremdes cwd,
  Asset-Umgebungsvariablen entfernt, RE15_ASSET_DEBUG=1. Skript `rel0818_art/art_pruefung.sh`.
- **Selbsttest** (cwd `fremd_a`): EXIT=0, `RESULT ok=26 missing=0`, 0 Treffer ausserhalb (s. Checkliste).
- **DOOR13, Seite S017** (cwd `fremd_b`, `RE15_TUER_SEITE=S017 RE15_TUER_BOGEN=<dir> RE15_TUER_SCHNELL=1`): EXIT=0;
  Log `[tuer-seite] S017 ROOM1050 DOOR13 V0` -> `Sequenz Archiv 2 DOOR13 Variante 0 ... (9 Skripte)` ->
  `Bogen-Bilder S017: Anfang Bild 20, Mitte Bild 205` -> `Sequenz fertig: 286 Bilder + 1 Warten, 1 Se_on,
  Schliesston 1, Ton geladen, Notizen 0x0`; cd-root = Paket.
  **Mitte-Bild angesehen** (960x720, dazu Ausschnitt x420-740/y320-500 2,5x aufgehellt): die Kante zwischen Fenster und
  unterem Feld ist GERADE — Fenster-Unterkante waagrecht, Feld-Oberkante eine schraege Gerade mit nur pixelgrosser
  Welligkeit; KEIN V-Knick (Vergleich: `unterteilung_belege/s017_door13_mitte_vorher_nachher.jpg`, das Paketbild
  entspricht dem NACHHER-Stand). Knick-Mass (`tools/tueren/unterteilung_knick.py --folgen 580`) am Paketbild:
  Fenster-Unterkante (475 700 270 370, +1) max 4,09 / 95 % 1,87 / RMS 0,81 px; Feld-Oberkante (450 710 425 485, -1)
  max 3,79 / 95 % 3,14 / RMS 1,45 px (Dossier Unterteilung: vorher 27,78 bzw. 30,11 px max, nachher 3,77 / 3,90 px).
  Zusatz: `S017_mitte.ppm` aus dem Paket ist **bitgleich** (cmp) mit `build/r32_int_check/bogen/S017_mitte.ppm`
  (Kontrolllauf des Leiters auf a0b500d1).
- **Tor ROOM1170 V1** (cwd `fremd_c`, `RE15_TUER_TEST=1 RE15_TUER_SERIE=<dir> RE15_TUER_SCHNELL=1 RE15_NOAUDIO=1`):
  EXIT=0, 324 Bilder (32 a_abdunkeln, 291 b_tuer, 1 c_ende); Log `Sequenz Archiv 1 DOOR2E Variante 1 ...` ->
  `Sequenz fertig: 291 Bilder ... Ton STUMM` (NOAUDIO). **Bild 100 angesehen:** Torrahmen hellgrau, Schild hell mit
  gelb-schwarzen Warnstreifen und lesbarem Text — so hell wie das NACHHER in `tor_belege/v1_bild100_vorher_nachher.jpg`.
  Gemessen (`tools/tor/tor_helligkeit.py bild b_tuer_100.ppm tor 1 100`): gesamt 56162 Bildpunkte, gezeigt/gemalt
  F/P **1,0013**, P/F 0,999 (vorher 1,912; Dossier Tor nachher V1 Bild 100: P/F 0,999, F/P 1,0013 — identisch);
  Schild F/P 1,0079, Rohr 0,9921, Pfosten 0,9854; ueber 0x80: 480 Bildpunkte, F/T 1,12 (PSX-Modell 1,1197).
- Belegbild: `analysis/befunde_runde32/release_belege/paket_v0818_door13_mitte_tor_v1.jpg` (S017 Mitte, Ausschnitt, Tor V1
  Bild 100 — alle aus dem Paket).
- Hinweis Pruefweg: der erste Anlauf des Pruefskripts hatte `/c/msys64/usr/bin` vorn im PATH; msys64-`find`/`sha256sum`
  loesen `/tmp` anders auf als Git-Bash (C:/msys64/tmp) -> Pfade nicht gefunden, exe nicht gestartet (EXIT 127). Mit
  Windows-Pfaden (`pwd -W`) und zip per Vollpfad wiederholt; Ergebnisse oben stammen aus dem zweiten Lauf.

### 6. Archiv
- `C:/workspace/Re15Data/re15_packages_archiv/v0.8.18/` angelegt: 6 Split-Volumes + APK + SHA256SUMS.txt +
  SHA256SUMS_android.txt (gleiche Belegung wie v0.8.17). `sha256sum -c SHA256SUMS.txt`: 6x OK, rc=0;
  `sha256sum -c SHA256SUMS_android.txt`: APK OK, rc=0. SUMS im Archiv == `git show HEAD:release/...` (cmp, ohne CR).
- Unmittelbar vorherige Version `v0.8.17` dort entfernt (liegt weiter in Git, Release-Commit e714d95f);
  verbleibend: v0.8.7-v0.8.14, v0.8.18.

### 7. Release-Commit
- `c7d5a092` `release: v0.8.18 (Windows + Linux/Steam Deck + Android)` per `git commit -F`, Form wie e714d95f:
  8 Dateien = 2x M (SHA256SUMS.txt, SHA256SUMS_android.txt) + 6x R (v0.8.17 -> v0.8.18: android/linux/win64 je
  .z01/.zip, von make_package.sh vorgemerkt). Blob-Gleichheit vor dem Commit `git rev-parse :<f>` == `git hash-object <f>`
  fuer alle 6 Volumes. APK gitignoriert (`.gitignore:63 release/*_android.apk`), liegt im Archiv.
  `git ls-files release | grep re15_port_v`: nur v0.8.18 (6).
- Danach dieses Dossier + Belegbild als eigener doc-Commit (HEAD). Der Tag gehoert auf den Release-Commit c7d5a092.
  Kein Tag, kein Push, kein Merge (macht der Leiter).
- Die Windows-Suite (local_build.sh) wurde in DIESEM Lauf nicht gefahren (laut Auftrag auf a0b500d1 416/416);
  gemessen ist die Linux-Suite im Container (416/416). Kein Spielcode geaendert.

## Offen / Hinweise
- Docker Desktop lief zu Beginn nicht; gestartet (20 s bis bereit).
- Pruefweg-Falle: Git-Bash + `/c/msys64/usr/bin` vorn im PATH mischt zwei MSYS-Laufzeiten (msys64-Werkzeuge sehen `/tmp`
  als C:/msys64/tmp). Fuer eigene Pruefskripte Windows-Pfade (`pwd -W`) und msys64-zip per Vollpfad nehmen.
- `unzip <apk> 'lib/*'` unter MSYS: "filename not matched" trotz vorhandener Eintraege; explizite Namen gehen.
- Aufraeumen fuer den Leiter (alles gitignoriert bzw. unversioniert, in keinem Paket): `build/rel0818/` (Bau-Logs,
  entpackte APK-libs), `build/py3shim/`, `release/{win_out,linux_out,pkg-win,pkg-linux}`, die APK in release/,
  `re15_port/platform/android/app/.cxx`. Pruefkopien im Scratchpad `rel0818_art/`.
