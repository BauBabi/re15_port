# Release v0.8.17 — Dossier (laufend fortgeschrieben)

Arbeitsbaum: `.claude/worktrees/r31_integration`, Zweig `r31/integration`, Basis `a3368ef7`.
Auftrag: Paket v0.8.17 fuer Windows + Linux/Steam Deck + Android, Checkliste, Archiv, Release-Commit
(kein Tag, kein Push, kein Merge).

## Ablauf
- [x] 1. Skripte lesen, Docker pruefen
- [x] 2. Linux/Deck, Windows-Cross, Android bauen
- [x] 3. make_package.sh --version v0.8.17
- [x] 4. Checkliste
- [x] 5. Archiv
- [x] 6. Release-Commit

## Protokoll

### 1. Skripte + Umgebung (2026-09-29, gemessen)
- `release/build_linux_deck.sh` -> Image `re15-linux-build:deb11`, Quellbaum als Kopie, im Container
  `docker_linux_build.sh`: Release-Bau + ctest seriell (`--timeout 600`), Gates: Summenzeile vorhanden,
  gelaufen == registriert, >= `RE15_MIN_TESTS` (local_build.sh: 411). Ausgabe `release/linux_out/re15_pc`
  (+ `ldd.txt`, `glibc_max.txt`, `diag/`). Rotes ctest -> kein `re15_pc` in linux_out.
- `release/build_win_cross.sh` -> Image `re15-wincross-build:deb11`, `docker_win_build.sh`, Release,
  ohne Tests, nur Ziel re15_pc. Ausgabe `release/win_out/re15_pc.exe` (vorher geloescht).
- `release/build_android.sh --version v0.8.17` -> nativ (SDK/NDK r27c), `rm -rf app/.cxx` vor Gradle,
  Ausgabe `release/re15_port_v0.8.17_android.apk` + `release/SHA256SUMS_android.txt`. Gates: libs je ABI,
  Asset-Stichproben, aapt badging.
- `release/make_package.sh` baut NICHT, kopiert aus linux_out/win_out; Gates: BSS-Pfad, Frische
  (Binary juenger als letzter Port-Commit), Optimierung (<=20 SDL2-Pfade), glibc <= 2.31, Baum
  (inkl. TORSE.VBS + jede RE2/DOOR/*.DO2), LF, Laufzeit-Gate (nur Windows-exe auf diesem Host),
  x-Bit (zip_exec_bit.py), Split-Katalog; zippt APK mit; tauscht alte Pakete in git.
- Docker: 27.5.1 Docker Desktop laeuft, 16 CPUs; keine laufenden Container; Host-CPU-Last 13 %,
  keine gcc/ninja/ctest/re15-Prozesse -> Maschine frei.
- Letzter Port-Code-Commit: 93160292 (2026-09-29 10:24:17) -> Frische-Gate-Schranke.
- `re15_port/shared_assets/RE2/DOOR`: 24 Dateien.
- Reihenfolge seriell (Last-Regel): Windows-Cross -> Linux -> Android.

### 2a. Windows-Cross (bash release/build_win_cross.sh)
- Lauf 10:38:52 -> 10:40:04, EXIT=0, `WIN-CROSS-BUILD-OK: 4354858 Bytes`; Phasen: Packen 6 s (4681 Dateien,
  335 MB, 103 Rueckfall-Links), Auspacken 8 s, Configure 30 s, Compile+Link 16 s, gesamt 48 s im Container.
- Artefakt `release/win_out/re15_pc.exe`: 4354858 B, mtime 2026-09-29 10:40:02 (juenger als Port-Commit 10:24:17),
  sha256 `8227f8d91da796751dadfbe7b2f4de131cf6548f83b3af75640aa0b1746b7e7a`.
- Groesse gegen v0.8.16 (re15_pc.exe im Archiv-Katalog 4318308 B): +36550 B (+0,85 %) bei +2132/-179 Zeilen
  Port-Code (23 Dateien) inkl. zwei Tabellen-.inc -> plausibel, Release (Debug-Baum waere ~5,8 MB).
- PE-Header der gebauten exe: Magic 0x20b, Subsystem 2 (GUI), Zeitstempel 2026-09-29 08:40:01 UTC.
- Neuer Code im Build: `RE15_HEBETISCH_LOG` (hebetisch_1150.c) 1x, `DOOR/DOOR%02X.DO2` 1x,
  `passt nicht zur Tabelle @0x8009a520` 1x (door_scene_pc.c).

### 2b. Linux/Deck (bash release/build_linux_deck.sh)
- Lauf 10:40:12 -> 10:49:40, EXIT=0, `LINUX-BUILD-OK`. Phasen: Packen 27 s (18645 Dateien, 552 MB,
  357 Rueckfall-Links), Auspacken 12 s, Configure 16 s, Compile+Link 26 s, ctest 467 s (seriell), gesamt 519 s.
- Suite: `100% tests passed, 0 tests failed out of 411`, `Tests: 411 gelaufen = 411 registriert (Untergrenze 411)`,
  Total Test time 466.66 s. Fingerabdruck 411 Zeilen; einzige echte SKIP-Zeile: integration_r30_titel_puls
  (Bild-Teil ohne GPU, RE15_OHNE_GPU, wie v0.8.16) — die uebrigen .skip-Treffer sind PASS-Texte mit "skip/fehlt".
- Artefakt `release/linux_out/re15_pc`: 3808368 B (v0.8.16: 3769488 B, +38880 B), mtime 10:49:37,
  sha256 `54bb5b8c5a9c23c7113ff342eeec1ef06bcd015cdb86e17cbb0eb8f7142e7657`.
- `glibc_max.txt` = GLIBC_2.29; `ldd.txt` 0x "not found". Neue Code-Strings je 1x enthalten (wie Windows).

### 2c. Android (bash release/build_android.sh --version v0.8.17)
- Lauf 10:50:05 -> 10:53:49, EXIT=0, `ANDROID-BUILD-OK`; Gradle `BUILD SUCCESSFUL in 3m 34s`.
- `== CMake-Cache des NDK-Baus verworfen (GLOB neu auswerten) ==` im Protokoll; danach frische Tasks
  `configureCMakeRelWithDebInfo[arm64-v8a]` / `[x86_64]` + `buildCMake...[SDL2,main]` je ABI.
- Neue Quellen je ABI uebersetzt (app/.cxx/RelWithDebInfo/340i1f2z, 10:51-10:53):
  arm64-v8a: door_seq_zuordnung.c.o 34032 B, hebetisch_1150.c.o 15952 B, door_scene_pc.c.o 87672 B;
  x86_64: 33504 B / 16040 B / 77416 B.
- Symbole in libmain.so AUS DER APK (llvm-nm -D, NDK r27c): je ABI `re15_door_seq_zuordnen_re2`,
  `re15_door_seq_re2_archiv`, `re15_door_seq_anfrage_fuer_seite`, `re15_door_seq_griff_tausch`,
  `re15_hebetisch_raum_scan`, `re15_hebetisch_ruht_oben`, `re15_hebetisch_protokoll` u. a. als `T` definiert
  (arm64 libmain.so 1310640 B, x86_64 1370168 B; je 1196 T-Symbole). Strings `DOOR/DOOR%02X.DO2` und
  `RE15_HEBETISCH_LOG` je 1x.
- aapt: package `de.re15.port`, versionCode 81700, versionName v0.8.17, native-code arm64-v8a + x86_64.
- Assets: re15_assets.txt 3570 Dateien / 354579777 B; APK 3571 Asset-Eintraege.
  `assets/shared_assets/RE2/DOOR/*.DO2` = 24 (alle `Stored`), `RE2/TORSE.VBS` = 1; entpackt und gegen
  `re15_port/shared_assets/RE2` per sha256 geprueft: 25/25 OK (24 DO2 + TORSE.VBS); Dateiliste des ganzen
  RE2-Baums identisch.
- APK `release/re15_port_v0.8.17_android.apk`: 361046809 B (v0.8.16: 359397277 B), mtime 10:53:46,
  sha256 `7d8ae4b4a4943fb6fee6994c525a69a9bafcddc2fb2a88c61d43f425e575d640` (= SHA256SUMS_android.txt).

### 3. Paket (PATH="/c/msys64/usr/bin:$PATH" bash release/make_package.sh --version v0.8.17, ohne --only)
- EXIT=0, Ende 10:56:38. Gates im Lauf: Optimierung Linux 4 / Windows 3 SDL2-Quellpfade (Release);
  glibc GLIBC_2.29; "Tuerarchive im Paket: 24 x shared_assets/RE2/DOOR/*.DO2" (beide); LF-Gate gruen;
  Laufzeit-Gate Windows in_pkg 26/26 + foreign_cwd 26/26 "alle aus dem Paket"; Linux-Paket 3573 Dateien / 347M,
  Windows 3574 / 348M; Split-Kataloge 3766 / 3767 / 1 Eintraege, je 2 Volumes; x-Bit re15_pc 100644 -> 100755
  gesetzt und zurueckgelesen (re15_pc + run.sh 100755).
- Git: "6 alte Paketdatei(en) aus dem Repo entfernt, 6 neue vorgemerkt" (alle drei Plattformen v0.8.16 -> v0.8.17).
- Nebenwirkung auf dem Host: der erste `python`-Aufruf (zip_exec_bit.py) startete den Windows-Python-Install-
  Manager, der sich auf 26.3 aktualisierte und "Python 3.14.7" installierte (Meldung im Paketprotokoll). Auf das
  Paket ohne Einfluss (x-Bit gesetzt + zurueckgelesen, Container-Pruefung unten), aber eine unangekuendigte
  Host-Aenderung.
- Pakete (release/): android .z01 94371840 + .zip 74020013; linux .z01 94371840 + .zip 74149444;
  win64 .z01 94371840 + .zip 74256964.

### 4. Checkliste (gemessen am AUSGELIEFERTEN Artefakt: Split-Satz mit `zip -s 0` zusammengefuehrt + entpackt)
| Pruefung | Messwert | Ergebnis |
|---|---|---|
| sha256 Paket == Bau, Windows-exe | beide `8227f8d91da796751dadfbe7b2f4de131cf6548f83b3af75640aa0b1746b7e7a` | OK |
| sha256 Paket == Bau, Linux-Binary | beide `54bb5b8c5a9c23c7113ff342eeec1ef06bcd015cdb86e17cbb0eb8f7142e7657` | OK |
| sha256 Paket == Bau, APK | beide `7d8ae4b4a4943fb6fee6994c525a69a9bafcddc2fb2a88c61d43f425e575d640` | OK |
| PE-Subsystem (exe aus dem Paket) | Magic 0x20b, Subsystem 2 | OK |
| Laufzeit-Gate aus dem Paket | make_package in_pkg 26/26 + foreign_cwd 26/26; zusaetzlich exe aus dem ENTPACKTEN Zip, fremdes cwd, Umgebung geleert: `RESULT ok=26 missing=0`, 0 Treffer ausserhalb (18 PSX, 5 extracted_fx, 2 RE2, 1 synchro), rc=0 | OK |
| keine Logs im Paket | Katalog-Suche `.log/debug.log/befund/.dmp/trace/ctest/LastTest`: win 0, linux 0, android 0; Wurzel nur exe/.bat/README bzw. re15_pc/run.sh/README | OK |
| LF-Gate Linux-Paket | make_package gruen; entpackt: run.sh 0 CR, Shebang `#!/usr/bin/env bash`; im Container nochmals 0 CR | OK |
| x-Bit entpackt (debian:11, UnZip 6.00) | Split-Satz im Container zusammengefuehrt + entpackt: `-rwxr-xr-x re15_pc`, `-rwxr-xr-x run.sh`; Direktstart `./re15_pc --headless` rc=1 (keine Anzeige), nicht 126 | OK |
| GLIBC-Untergrenze | objdump -T am Paket-Binary: max GLIBC_2.29 (<= 2.31); ldd im Bau-Container 0 "not found" | OK |
| RE2/DOOR/*.DO2 + TORSE.VBS | Windows 24+1, Linux 24+1, APK 24+1; sha256 gegen re15_port/shared_assets/RE2: je 25/25 OK; ganzer RE2-Baum (274 Dateien) in allen drei bytegleich | OK |
| Groesse Windows-exe | 4354858 B gegen v0.8.16 4318308 B (+36550 B, +0,85 %); Release (3 SDL2-Pfade), Debug waere ~5,8 MB | OK |
| Neue Quellen im ausgelieferten Code | exe + Linux: Strings aus hebetisch_1150.c/door_scene_pc.c je 1x; APK: Symbole re15_door_seq_*/re15_hebetisch_* je ABI | OK |

### 5. Archiv
- `C:/workspace/Re15Data/re15_packages_archiv/v0.8.17/` angelegt: 6 Split-Volumes + APK + SHA256SUMS.txt +
  SHA256SUMS_android.txt (gleiche Belegung wie v0.8.16). `sha256sum -c SHA256SUMS.txt`: 6x OK, rc=0;
  `sha256sum -c SHA256SUMS_android.txt`: APK OK, rc=0.
- Unmittelbar vorherige Version `v0.8.16` dort entfernt; verbleibend: v0.8.7-v0.8.14, v0.8.17.

### 6. Release-Commit
- Zuerst dieses Dossier als eigener Commit (doc), danach der Release-Commit als HEAD von r31/integration:
  `release: v0.8.17 (Windows + Linux/Steam Deck + Android)` per `git commit -F`, Inhalt wie 2073ba13:
  6 Paketdateien (v0.8.16 -> v0.8.17 umbenannt/ersetzt, von make_package.sh vorgemerkt), SHA256SUMS.txt,
  SHA256SUMS_android.txt. Kein Tag, kein Push, kein Merge (macht der Leiter).
- Die Windows-Suite (local_build.sh) wurde in DIESEM Lauf nicht gefahren; gemessen ist die Linux-Suite im
  Container (411/411). Kein Spielcode geaendert.

## Offen / Hinweise
- Python-Install-Manager hat sich waehrend make_package.sh selbst aktualisiert und Python 3.14.7 installiert
  (Host-Nebenwirkung des `python`-Aufrufs fuer zip_exec_bit.py).
- Erste Entpackprobe der APK mit Muster `assets/shared_assets/RE2/*` lieferte nur die oberste Ebene (unzip-
  Musterverhalten, kein Paketfehler); mit `.../RE2/DOOR/*` explizit: 24 DO2 vorhanden, `unzip -Z1` zaehlt 24.
- Der gemeinsam genutzte Scratchpad-Ordner verlor waehrend der Pruefung ein Unterverzeichnis (`fremd/`, parallel
  aufgeraeumt?); Selbsttest in eindeutigem Ordner wiederholt, Ergebnis oben.
