# Release v0.8.19 — Dossier (laufend fortgeschrieben)

Arbeitsbaum: `.claude/worktrees/r33_integration`, Zweig `r33/integration`, Basis `8265636f`.
Auftrag: Paket v0.8.19 fuer Windows + Linux/Steam Deck + Android, Checkliste, unabhaengige
Artefakt-Pruefung (neue Port-Tuer S001 ROOM1000 / P07G, Gegenprobe ohne P07G.DO2), Archiv,
Release-Commit (kein Tag, kein Push, kein Merge).
Vorbild: `analysis/befunde_runde32/release_v0818.md` (Hauptbaum), Release-Commit c7d5a092.

NEU in v0.8.19: `re15_port/shared_assets/RE15DOOR/` (30 port-eigene Tuerarchive *.DO2, 1914932 B) muss in
Windows-, Linux-Paket und APK liegen, sha256-gleich mit dem Quellbaum.

## Ablauf
- [x] 1. Skripte lesen, Docker pruefen
- [x] 2. Windows-Cross, Linux/Deck, Android bauen
- [x] 3. make_package.sh --version v0.8.19 (Python-Shim vor PATH)
- [x] 4. Checkliste
- [x] 5. Unabhaengige Artefakt-Pruefung (Windows-Paket ausserhalb des Repos)
- [x] 6. Archiv
- [x] 7. Release-Commit

## Protokoll

### 1. Skripte + Umgebung
- Stand 8265636f; letzter Port-Code-Commit (re15_port/) cf386e32 2026-09-29 19:02:06 +0200 -> Frische-Schranke.
- Port-Aenderungen seit v0.8.18 (c7d5a092), ohne shared_assets: neu `engine/src/tuer1120_1130.c`,
  `engine/src/gen/re15_tuer_eigen.inc`, `include/re15_tuer1120.h`; geaendert u. a. `door_seq_zuordnung.c`,
  `map_hint_common.c`, `menu_common.c`, `re15_inv_screen.c`, `re15_savepoint.c`, `scd_room_setup.c`, `scd_vm.c`,
  `platform/pc/main.c`, `platform/pc/src/door_scene_pc.c`, `platform/android/app/build.gradle`.
- shared_assets: 33 neue Dateien = 30x `RE15DOOR/*.DO2` + 3x `RE2/DOOR/{DOOR0C,DOOR14,DOOR36}.DO2`.
  **Abweichung vom Auftragstext:** RE2/DOOR hat jetzt **27** *.DO2 (nicht 24) — die Checkliste misst 27 + TORSE.VBS.
- Gates fuer RE15DOOR:
  - `release/make_package.sh` (geaendert): `copy_common` kopiert `shared_assets/RE15DOOR`; `check_tree` prueft je
    Quelldatei `-s` im Paket + `cmp -s` gegen den Quellbaum, `die` bei Fehlen/Abweichung, Zeile
    "Port-Tuerarchive im Paket: N x shared_assets/RE15DOOR/*.DO2".
  - `release/build_android.sh` ist in dieser Runde **nicht** geaendert (letzter Commit fc59e06d). Die Ergaenzung
    fuer Android sitzt in `re15_port/platform/android/app/build.gradle`: `stageAssets` spiegelt
    `shared_assets/RE15DOOR`, `doFirst` bricht ab, wenn `shared_assets/RE15DOOR/P07G.DO2` im Repo fehlt. Das ist
    ein Quellbaum-Gate fuer EINE Datei, kein APK-Inhalts-Gate fuer alle 30 -> APK-Inhalt wird hier selbst gemessen.
- Docker Desktop lief bereits (27.5.1, 0 Container); Images re15-linux-build:deb11 / re15-wincross-build:deb11 vorhanden.
- Host frei: keine gcc/cc1/ninja/ctest/cmake/re15/gradle-Prozesse, CPU-Last 40 %, 171 GB frei auf C:.
- `RE15_MIN_TESTS` in re15_port/tools/local_build.sh = 428 (docker_linux_build.sh liest es als Untergrenze).
- Python-Shim `build/py3shim/{python3,python}` -> `exec /c/Python310/python` (3.10.11); mit
  `PATH="$(pwd)/build/py3shim:/c/msys64/usr/bin:$PATH"`: `command -v python3/python` = Shim, zip = /usr/bin/zip.
- Bauten seriell in einer Kette (`build/rel0819/kette.sh`): Windows-Cross -> Linux -> Android, je Log
  `build/rel0819/{win,linux,android}.log` mit eigener `EXIT=`-Zeile (kein Pipe). Start 19:19.

### 2a. Windows-Cross (bash release/build_win_cross.sh)
- Lauf 19:19 -> 19:20:03, EXIT=0, `WIN-CROSS-BUILD-OK: 4381178 Bytes`; Packen 5 s (4749 Dateien, 338 MB,
  104 Rueckfall-Links), Configure 28 s, Compile+Link 15 s, gesamt 43 s im Container (Wrapper 59 s).
- Artefakt `release/win_out/re15_pc.exe`: 4381178 B, mtime 2026-09-29 19:20:02 (juenger als Port-Commit 19:02:06),
  sha256 `30d5b67b0b03f64e6fa3ce59528e36c30fbad0a4e29387beb78a22761d6fd41b`.
- Groesse gegen v0.8.18 (4358987 B, Release): +22191 B (+0,51 %). Passt zur Quelltextaenderung: neu
  tuer1120_1130.c + gen/re15_tuer_eigen.inc (30 Archiv-Rezepte + Seitenzuordnung, `r re15_tuer_eigen`),
  door_seq_zuordnung.c (141 statt 83 Tueren mit Sequenz), Speichern/Karte (1155 Einfuegungen in 12 Quelldateien);
  Release (Debug waere ~5,8 MB) — die Optimierungs-Pruefung faehrt make_package (SDL2-Quellpfade).
- PE-Header: e_lfanew 0x80, `PE\0\0`, Magic 0x20b, Subsystem 2 (GUI), Zeitstempel 2026-09-29 17:20:02 UTC.
- Neuer Code im Build (nm, mingw64): `T re15_tuer1120_install`, `T re15_tuer1120_gesperrt`, `T re15_tuer1120_meldung`,
  `T re15_door_seq_eigen`, `T re15_door_seq_eigen_anzahl`, `r re15_tuer_eigen`.

### 2b. Linux/Deck (bash release/build_linux_deck.sh)
- Lauf 19:20:03 -> 19:30:07, EXIT=0, `LINUX-BUILD-OK`. Phasen: Packen 22 s (18713 Dateien, 555 MB,
  360 Rueckfall-Links), Auspacken 13 s, Configure 11 s, Compile+Link 22 s, ctest 530 s (seriell), gesamt 565 s.
- Suite: `100% tests passed, 0 tests failed out of 428`, `Tests: 428 gelaufen = 428 registriert (Untergrenze 428)`,
  Total Test time 530.18 s; 428x Passed in ctest_out.txt. Einzige echte SKIP-Zeile wie v0.8.18:
  integration_r30_titel_puls (Bild-Teil ohne GPU, RE15_OHNE_GPU); die uebrigen .skip-Treffer sind PASS-Texte.
- Neue Tests dieser Runde im Container gruen (Auswahl): r33_aufrufstelle, unit_r33_karte_{etage,markierung,speicher},
  unit_r33_speichern, integration_r33_speichern (62 s, echte Linux-exe), unit_r33_tuer1120_{gesperrt,frei,szene,elza},
  unit_r33_tueren_{archive,zuordnung,maschine}.
- Artefakt `release/linux_out/re15_pc`: 3834872 B (v0.8.18: 3812760 B, +22112 B — gleiche Groessenordnung wie
  Windows +22191 B), mtime 19:30:06, sha256 `abfbe7c517bf98ce4cdd31607b81a8e555f0ce23af1c22e84f89ba76f5cf0dc3`.
- `glibc_max.txt` = GLIBC_2.29; `ldd.txt` 0x "not found". nm (re15-inspect:deb11): `T re15_tuer1120_{install,
  gesperrt,meldung}`, `T re15_door_seq_eigen`, `T re15_door_seq_eigen_anzahl`, `r re15_tuer_eigen` (wie Windows).

### 2c. Android (bash release/build_android.sh --version v0.8.19)
- Lauf 19:30:07 -> 19:33:05, EXIT=0, `ANDROID-BUILD-OK`; Gradle `BUILD SUCCESSFUL in 2m 49s`, 54 Tasks executed.
- `== CMake-Cache des NDK-Baus verworfen (GLOB neu auswerten) ==` im Protokoll; neuer .cxx-Hash `4ry6gz29`.
- Neue/geaenderte Quellen je ABI uebersetzt (app/.cxx/RelWithDebInfo/4ry6gz29):
  arm64-v8a: tuer1120_1130.c.o **(neu)** 9976 B (19:31:34), door_seq_zuordnung.c.o 57312 B (inkl. gen/re15_tuer_eigen.inc),
  re15_savepoint.c.o 30024 B, door_scene_pc.c.o 74448 B; x86_64: 9208 B (19:32:29) / 58112 B / 29568 B / 70352 B.
- Symbole in libmain.so AUS DER APK (llvm-nm -D, NDK r27c 27.2.12479018): je ABI `T re15_tuer1120_install`,
  `T re15_tuer1120_gesperrt`, `T re15_tuer1120_meldung`, `T re15_door_seq_eigen`, `T re15_door_seq_eigen_anzahl`;
  1220 T-Symbole je ABI (v0.8.18: 1199, +21). arm64 libmain.so 1336832 B (v0.8.18 1315008),
  x86_64 1398200 B (v0.8.18 1375240).
- aapt: package `de.re15.port`, versionCode 81900, versionName v0.8.19, sdk 24 / target 35,
  native-code arm64-v8a + x86_64.
- Assets: re15_assets.txt 3603 Dateien / 356678277 B (30 Zeilen RE15DOOR); APK 3604 Asset-Eintraege
  (v0.8.18: 3571, +33 = 30 RE15DOOR + 3 neue RE2/DOOR).
  **RE15DOOR in der APK:** `assets/shared_assets/RE15DOOR/*.DO2` = 30 (alle Stored, P07G.DO2 55908 B);
  `RE2/DOOR/*.DO2` = 27 + `RE2/TORSE.VBS`; entpackt, `sha256sum -c` gegen re15_port/shared_assets: **58/58 OK**, rc=0.
- APK `release/re15_port_v0.8.19_android.apk`: 363212403 B (v0.8.18: 361068261 B), mtime 19:33:03,
  sha256 `514bebd55bd6b8ec2300fd45a19d3655c91f875c9d5e7667b4631c95da7373d9` (= SHA256SUMS_android.txt).

### 3. Paket (PATH="$(pwd)/build/py3shim:/c/msys64/usr/bin:$PATH" bash release/make_package.sh --version v0.8.19, ohne --only)
- Vorab mit identischem PATH (`build/rel0819/pkg_vorspann.txt`): `command -v python3` / `python` = Shim, Python 3.10.11.
- 19:34:11 -> 19:35:36, EXIT=0. Gates im Lauf: Optimierung Linux 4 / Windows 3 SDL2-Quellpfade (Release); glibc
  GLIBC_2.29; "Tuerarchive im Paket: 27 x shared_assets/RE2/DOOR/*.DO2" (beide); **"Port-Tuerarchive im Paket:
  30 x shared_assets/RE15DOOR/*.DO2" (beide)**; LF-Gate gruen; Laufzeit-Gate Windows in_pkg 26/26 + foreign_cwd 26/26
  "alle aus dem Paket"; Linux-Paket 3606 Dateien / 350M, Windows 3607 / 350M (v0.8.18: 3573 / 3574, je +33);
  Split-Kataloge 3800 / 3801 / 1 Eintraege, je 2 Volumes; x-Bit re15_pc 100644 -> 100755 gesetzt und zurueckgelesen
  (re15_pc + run.sh 100755). Frische-Gate (Binary juenger als cf386e32) bestanden (still, bricht nur ab).
- Git: "6 alte Paketdatei(en) aus dem Repo entfernt, 6 neue vorgemerkt" (alle drei Plattformen v0.8.18 -> v0.8.19;
  `git status`: 6x R + SHA256SUMS.txt/SHA256SUMS_android.txt M).
- **Python-Falle entschaerft:** nach dem Lauf KEINE Installer-Spuren: kein `release/Python/`, kein
  `release/python_install_*.log`, kein `%LOCALAPPDATA%/Programs/Python`; in `%LOCALAPPDATA%/Python` (seit 15:39 vorhanden,
  nicht aus diesem Lauf) und `WindowsApps` 0 Dateien neuer als die Marke vor dem Lauf; kein python/pymanager/msiexec-Prozess.
- Pakete (release/): android .z01 94371840 + .zip 75105247; linux .z01 94371840 + .zip 75236322;
  win64 .z01 94371840 + .zip 75344263.

### 3b. Greifen die RE15DOOR-Gates? (Negativproben)
- **make_package.sh `check_tree`** — die ECHTE Funktion aus dem Skript (per awk extrahiert, `die`/`RE2`/`RE15DOOR`/
  `SYNCHRO`/`FX_REQUIRED` wie im Skript) gegen ein Schattenpaket im Scratchpad (`rel0819_art/gate/gate_probe.sh`):
  A intakt -> "27 x RE2/DOOR", "30 x RE15DOOR", rc=0; B P07G.DO2 entfernt -> `ABBRUCH: Port-Tuerarchiv fehlt/leer im
  Paket: shared_assets/RE15DOOR/P07G.DO2`, rc=1; C P07G.DO2 0 B -> dieselbe Meldung, rc=1; D P07G.DO2 gleich gross,
  1 Byte veraendert (cmp: differ char 30001) -> `ABBRUCH: Port-Tuerarchiv im Paket weicht vom Quellbaum ab`, rc=1;
  E letzte Datei P2DS.DO2 entfernt -> fehlt/leer, rc=1.
- **Android (`app/build.gradle` stageAssets.doFirst)** — Quelldatei `re15_port/shared_assets/RE15DOOR/P07G.DO2` kurz
  nach build/rel0819 verschoben, `gradlew stageAssets`: `Task :app:stageAssets FAILED` / `Asset fehlt im Repo:
  re15_port/shared_assets/RE15DOOR/P07G.DO2`, rc=1 (12 s). Datei zurueck, sha256 vorher = nachher
  (68306346...), `git status re15_port/` leer. Log `build/rel0819/gradle_gate_neg.log`.
- **Reichweite:** das Gradle-Gate prueft nur, ob EINE Datei (P07G) im Quellbaum liegt; `build_android.sh` selbst
  wurde in dieser Runde nicht ergaenzt, seine APK-Gate-Liste enthaelt kein RE15DOOR. Ein APK-Inhalts-Gate fuer alle
  30 Archive gibt es damit nicht — hier ersetzt durch die Messung in 2c (30 Eintraege, 58/58 sha256).

### 4. Checkliste (gemessen am AUSGELIEFERTEN Artefakt: Split-Saetze mit `zip -s 0` zusammengefuehrt + entpackt, ausserhalb des Repos)
| Pruefung | Messwert | Ergebnis |
|---|---|---|
| sha256 Paket == Bau, Windows-exe | beide `30d5b67b0b03f64e6fa3ce59528e36c30fbad0a4e29387beb78a22761d6fd41b` (Paket im Scratchpad entpackt) | OK |
| sha256 Paket == Bau, Linux-Binary | beide `abfbe7c517bf98ce4cdd31607b81a8e555f0ce23af1c22e84f89ba76f5cf0dc3` (Split-Satz IM debian:11-Container zusammengefuehrt + entpackt) | OK |
| sha256 Paket == Bau, APK | beide `514bebd55bd6b8ec2300fd45a19d3655c91f875c9d5e7667b4631c95da7373d9` (APK aus dem Android-Split-Zip, Scratchpad) | OK |
| PE-Subsystem | Magic 0x20b, Subsystem 2 (Paket-exe = Bau-exe per sha256) | OK |
| Laufzeit-Gate aus dem Paket | make_package in_pkg 26/26 + foreign_cwd 26/26; zusaetzlich Kopie `r19_art.exe` im ENTPACKTEN Paket, fremdes cwd, RE15_ASSET_ROOT/CD_ROOT/RE2_ASSET_ROOT entfernt: `[selftest] RESULT ok=26 missing=0`, 26 OK-Zeilen, 0 Treffer ausserhalb des Pakets; cd-root[0] = Paket, cd-root[1] = /src/... (Container-Pfad, auf dem Host nicht vorhanden, kein C:/src) | OK |
| keine Logs im Paket | Katalog-Suche `.log/debug.log/befund/.dmp/trace/ctest/LastTest/re15_card/python`: win 0 (3801), linux 0 (3800), android 0 (1); Wurzel nur exe/2 .bat/README bzw. re15_pc/run.sh/README | OK |
| LF-Gate Linux-Paket | make_package gruen; im Container: run.sh 0 CR, Shebang `#!/usr/bin/env bash`, `file`: "Bourne-Again shell script, UTF-8 Unicode text executable" | OK |
| x-Bit entpackt (debian:11, UnZip 6.00) | Split-Satz IM Container zusammengefuehrt (zip 3.0) + entpackt: `-rwxr-xr-x re15_pc`, `-rwxr-xr-x run.sh`, Zentralverzeichnis beide `-rwxr-xr-x 3.0 unx`; Direktstart `./re15_pc --headless` rc=1, `./run.sh --headless` rc=1; **Gegenprobe** dieselbe Datei mit Modus 644: rc=126 | OK |
| GLIBC-Untergrenze | objdump -T am Paket-Binary im Container: max GLIBC_2.29 (<= 2.31); `ldd` 0x "not found" in debian:11 (nur zip/unzip/binutils/file nachinstalliert, keine Laufzeitbibliotheken) | OK |
| RE2/DOOR + TORSE.VBS + **RE15DOOR** | Windows 27+1+30, Linux 27+1+30, APK 27+1+30; `sha256sum -c` gegen re15_port/shared_assets: je **58/58 OK**; ganzer RE2-Baum (277 Dateien) in Windows + Linux bytegleich | OK |
| Groesse Windows-exe | 4381178 B gegen v0.8.18 4358987 B (+22191 B, +0,51 %); Release (3 SDL2-Pfade), Debug waere ~5,8 MB; Linux +22112 B im Gleichschritt | OK |
| Neuer Code im ausgelieferten Code | exe + Linux (nm): `re15_tuer1120_{install,gesperrt,meldung}`, `re15_door_seq_eigen{,_anzahl}` als T, `re15_tuer_eigen` als r; APK: dieselben fuenf T je ABI (+21 T-Symbole gegen v0.8.18) | OK |

Hinweis Pruefweg: das nackte debian:11 bekam `unzip` vom Security-Spiegel nicht (404 fuer unzip_6.0-26+deb11u2);
mit `unzip/bullseye` (6.0-26+deb11u1 aus main) ging es. Der erste Container-Lauf suchte den Paketordner als
`re15_port_v0.8.19_linux_steamdeck_x64` — im Zip heisst er `re15_port_v0.8.19`; Ergebnisse oben aus dem korrigierten Lauf.

### 5. Unabhaengige Artefakt-Pruefung (Windows-Paket im Scratchpad, ausserhalb des Repos)
- Quelle: `release/re15_port_v0.8.19_win64.z01/.zip` (sha256 3bfeb32c... / 55aa2b04...) nach
  `<scratchpad>/rel0819_art/win` kopiert, `zip -s 0` rc=0, `unzip` rc=0, 3607 Dateien. Lauf-exe als Kopie
  `r19_art.exe` IM Paketordner (eigener Bildname; exe-Verzeichnis-Anker bleibt das Paket). Je Lauf eigenes fremdes cwd,
  Asset-Umgebungsvariablen entfernt, RE15_ASSET_DEBUG=1. Skript `rel0819_art/art_pruefung.sh`.
- **Selbsttest** (cwd `fremd_a`): EXIT=0, `RESULT ok=26 missing=0`, 0 Treffer ausserhalb (s. Checkliste).
- **Neue Port-Tuer S001** (cwd `fremd_b`, `RE15_TUER_SEITE=S001 RE15_TUER_BOGEN=<dir> RE15_TUER_SCHNELL=1`): EXIT=0;
  Wurzeln cd[0]/shared[0]/base[0] = Paket, cd[1]/shared[1] = /src/... (existiert nicht); Log
  `[tuer-seite] S001 ROOM1000 DOOR07 V1 Spender FF Port-Archiv P07G` -> `Sequenz Archiv 2 DOOR07 Port-Archiv P07G
  Variante 1 ... (9 Skripte)` -> `Bogen-Bilder S001: Anfang Bild 20, Mitte Bild 177` -> `Bild 70 Se_on` ->
  `Sequenz fertig: 301 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0 (Bogen-Bilder geschrieben)`.
  **Beide Bilder angesehen** (960x720): Anfang = glatte, dunkle blaugruene Stahltuer ohne Lueftungsschlitze/Flecken,
  Druecker RECHTS als waagrechter Hebel auf einem senkrechten silbernen Rechteckschild; Mitte = das Blatt schwingt auf
  (Oberkante schraeg, Perspektive), Druecker heruntergedrueckt, Schild sichtbar. Entspricht Zeile a) "G1 Druecker +
  Schild: S001" im Bau-Beleg `tueren_rest_belege/bau_pilotpunkte_abc.jpg` (echte exe auf r33/tueren).
  Gemessen am Anfang-Bild: Blatt Zeile 300 x 253..651 = **399 px** (Bau-Dossier tueren_rest_bau.md 4a: 399 px),
  Druecker+Schild x 583..641 = 59 px (Dossier: Druecker 57 px); Blatt RGB-Median (23, 32, 41), Griff/Schild (73, 77, 82).
- **Gegenprobe ohne P07G.DO2** (zweite Paketkopie `win_ohne`, darin RE15DOOR 29 Dateien, cwd `fremd_c`, sonst gleich):
  EXIT=0; Log `[tuer-seite] S001 ROOM1000 DOOR07 V1 Spender FF Port-Archiv P07G` -> **`[tuer] shared_assets/RE15DOOR/
  P07G.DO2 fehlt`** -> `[tuer] Archiv 2/07 nicht lesbar -> Tuerwechsel ohne Sequenz`; keine Bogen-Bilder; Wurzeln nur
  Paketkopie + /src (kein Repo-Pfad: 0 Treffer `C:/workspace`/`worktrees` in allen drei Logs) -> **kein Rueckfall aufs Repo**,
  obwohl `re15_port/shared_assets/RE15DOOR/P07G.DO2` im Arbeitsbaum liegt.
- Belegbild: `analysis/befunde_runde33/release_belege/paket_v0819_s001_p07g.jpg` (S001 Anfang + Mitte, aus dem Paket).
- Hinweis Pruefweg: im ersten Skriptlauf fehlte fuer die Gegenprobe das Zielverzeichnis vor `cp -r` (Kopie landete eine
  Ebene zu hoch, exe nicht gefunden, EXIT=127); einzeln mit angelegtem Ziel wiederholt, Ergebnis oben aus diesem Lauf.

### 6. Archiv
- `C:/workspace/Re15Data/re15_packages_archiv/v0.8.19/` angelegt: 6 Split-Volumes + APK + SHA256SUMS.txt +
  SHA256SUMS_android.txt (gleiche Belegung wie v0.8.18). `sha256sum -c SHA256SUMS.txt`: 6x OK, rc=0;
  `sha256sum -c SHA256SUMS_android.txt`: APK OK, rc=0. SUMS im Archiv == `git show HEAD:release/...` (cmp, ohne CR).
- Unmittelbar vorherige Version `v0.8.18` dort entfernt (liegt weiter in Git, Release-Commit c7d5a092, APK im
  Android-Split); verbleibend: v0.8.7-v0.8.14, v0.8.19.

### 7. Release-Commit
- `64170c05` `release: v0.8.19 (Windows + Linux/Steam Deck + Android)` per `git commit -F`, Form wie c7d5a092:
  8 Dateien = 2x M (SHA256SUMS.txt, SHA256SUMS_android.txt) + 6x R (v0.8.18 -> v0.8.19: android/linux/win64 je
  .z01/.zip, von make_package.sh vorgemerkt). Blob-Gleichheit vor dem Commit `git rev-parse :<f>` == `git hash-object <f>`
  fuer alle 6 Volumes. APK gitignoriert (`.gitignore:63 release/*_android.apk`), liegt im Archiv.
  `git ls-files release | grep re15_port_v`: nur v0.8.19 (6).
- Danach dieses Dossier + Belegbild als eigener doc-Commit (HEAD). Der Tag gehoert auf den Release-Commit 64170c05.
  Kein Tag, kein Push, kein Merge (macht der Leiter).
- Die Windows-Suite (local_build.sh) wurde in DIESEM Lauf nicht gefahren (laut Auftrag auf 8265636f 428/428);
  gemessen ist die Linux-Suite im Container (428/428). Kein Spielcode geaendert.

## Offen / Hinweise
- **Auftragstext vs. Messung:** RE2/DOOR hat in v0.8.19 **27** Archive (neu DOOR0C, DOOR14, DOOR36), nicht 24;
  alle Pruefungen laufen gegen 27 + TORSE.VBS + 30 RE15DOOR = 58 Dateien.
- **Android-Gate-Luecke:** `release/build_android.sh` wurde in dieser Runde NICHT ergaenzt; die RE15DOOR-Ergaenzung sitzt in
  `app/build.gradle` (Spiegelung + Existenzpruefung EINER Datei P07G im Quellbaum). Ein APK-Inhalts-Gate fuer alle 30
  Port-Archive fehlt (build_android.sh prueft RE2 nur ueber CDEMD0.EMS). Fuer v0.8.19 per Messung ersetzt (30/30 Stored,
  58/58 sha256); als Nacharbeit fuer den Leiter: RE15DOOR-Zaehlung + cmp in die Gate-Schleife von build_android.sh.
- Pruefweg-Fallen: debian:11-Security-Spiegel 404 fuer unzip (-> `unzip/bullseye`); Paketordner im Linux-Zip heisst
  `re15_port_v0.8.19`; im Git-Bash-Skript `cp -r` nur in ein angelegtes Ziel.
- Aufraeumen fuer den Leiter (alles gitignoriert bzw. unversioniert, in keinem Paket): `build/rel0819/` (Bau-Logs,
  entpackte APK-libs/Assets), `build/py3shim/`, `release/{win_out,linux_out,pkg-win,pkg-linux}`, die APK in release/,
  `re15_port/platform/android/app/.cxx`. Pruefkopien im Scratchpad `rel0819_art/` (~1,5 GB).
