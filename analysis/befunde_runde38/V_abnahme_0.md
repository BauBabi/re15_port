# Runde 38 Spur V — releaseskript: UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r38_releaseskript` (Zweig `r38/releaseskript`, HEAD bec9462d, Basis e2cc7673).
Pruefer: Abnahme-Agent 0, 2026-10-16 (ab ~21:00). Alles hier ist SELBST gemessen; Behauptungen des Bau-Agenten
gelten erst nach eigener Messung.

## Stand (laufend, neueste oben)

* 22:40 ABSCHLUSS: bestanden = NEIN (Mangel M1, V1 teilweise). Seriell 7/7 r38 gruen (A11).
* 22:35 Serieller ctest-Lauf fertig (A11): r38 7/7 Passed (811 s), r35 pruefkette Passed, r35 bash_mutanten
  Timeout unter Last (nicht der Spur zuzurechnen, A11).
* 22:10 Docker-Desktop-Start fuer die Linux-Gegenprobe: `docker desktop start --timeout 300` -> nach 339 s
  `Failed to start Docker Desktop ... still starting: context deadline exceeded` (A10); Docker danach gestoppt.
* 21:50 Eigener Fehlerpfad-Versuch Push (A9) -> Mangel M1.
* 21:45 fehler_umfeld einzeln gruen (A5), Doppelklick-Weg gemessen (A6).
* 21:30 ctest -j3: 6/7, fehler_umfeld Timeout (A4).
* 21:20 Trockenlauf selbst gemessen (A1); FUND: das echte Archiv `C:\workspace\Re15Data` existiert nicht mehr (§3).
* 21:05 Lektuere AUFTRAG/VERTRAG/Dossier, Code-Diff vollstaendig gelesen, Bau `local_build.sh configure` + `build`
  = `LOCAL-BUILD-OK (configure)`, `LOCAL-BUILD-OK (build)` (ninja: no work to do), `ctest -N` = 795 Tests
  (788 + 7 neue unit_r38_releaseskript_*).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Zeile 77)

> Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
> Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

Teilpunkte fuer das Urteil:
* V1 "einfache Moeglichkeit - ueber Skript oder sowas"
* V2 "die Ports fuer alle Systeme zu bauen"
* V3 "der neuen Version eine neue Versionsnummer - minor"
* V4 "die alte ins Archiv zu bringen"

Abnahme-Massstab Spur V (VERTRAG.md §2): "Trockenlauf + Versionslogik-Tests; den echten Release faehrt der
Orchestrator mit dem Skript." Lesart "minor" (AUFTRAG.md "Lesarten"): letzte Stelle hochzaehlen, 0.8.99 -> 0.9.0.

## 1. Diff (git diff master...HEAD --name-only, 17 Dateien)

Nur `release/` (README.md, .gitattributes, .gitignore, docker_bereit.sh, release_alles.cmd, release_alles.sh,
release_alles_lib.sh), `re15_port/tests/unit/probes/r38_releaseskript.cmake` (Sonden-GLOB, keine CMakeLists),
`re15_port/tests/unit/r38_releaseskript/**` (Test + Attrappen) und das Dossier. Kein Spiel-Code, keine
`platform/android/`, keine `shared_assets/PSX/`, kein `release/RELEASE_NOTES.md`. `release/` gehoert laut
VERTRAG.md §1.4 ausschliesslich Spur V. (`git diff master --stat` zeigt zusaetzlich geloeschte Orchestrator-Dateien:
das sind nur Commits, die master NACH der Basis e2cc7673 bekam - der Drei-Punkt-Diff hat sie nicht.)
Code-Diff vollstaendig gelesen: release_alles.sh (722 Z.), release_alles_lib.sh (230), docker_bereit.sh (105),
release_alles.cmd (76), README.md, Test (642) + 7 Attrappen.

## 2. Messungen

### A1 Trockenlauf im Spurbaum gegen den Standard-Archivpfad (21:13, HEAD bec9462d)

`bash release/release_alles.sh --dry-run` -> RC=0 in 161 s (Maschine ausgelastet: cmake 11, ctest 9 laut eigener
Vorbedingungszeile), `Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`, Plan `alt windows android linux paket archiv`,
alle Vorbedingungen [ok]/[--], Docker `[--] ... ein echter Lauf startet Docker Desktop ('docker desktop start')`,
sechs Schritte mit Befehl (android/paket mit `--version v0.8.26`), Schlusszeile
`TROCKENLAUF-OK v0.8.26 (1 Hinweis(e) [!!]) - nichts gebaut, nichts geschrieben`.
Schreibt nichts: `git status --porcelain --untracked-files=all --ignored -- release` vorher = nachher; der
Archivpfad wurde nicht angelegt (`Test-Path C:\workspace\Re15Data` danach weiter False).
ABER: `[ok] Vorversion v0.8.25 fehlt im Archiv - Schritt 'alt' legt C:/workspace/Re15Data/re15_packages_archiv/v0.8.25 an`
- siehe §3.

### A2 Schritt "alt" mit ECHTEN Daten in ein leeres Wegwerf-Archiv (21:25, V4)

`bash release/release_alles.sh --bis alt --archiv <scratchpad>/archiv_real` -> RC=0 (139 s gesamt, Schritt 27 s):
`release/ v0.8.25: 6 Volumes, sha256sum -c 6/6 OK` (SUMS im Spurbaum CRLF, `cat -A` zeigt `^M$` - ohne \r gelesen),
`Hinweis: release/re15_port_v0.8.25_android.apk fehlt ... archiviert werden die Volumes` (die APK ist gitignoriert
und liegt nur im Hauptbaum), `Vorversion v0.8.25 ins Archiv gebracht (...): SHA256SUMS.txt 6/6 OK`,
`RELEASE-ALLES-TEILLAUF-OK v0.8.26 (alt)`. Unabhaengig nachgeprueft:
* Archivordner `v0.8.25/` = 6 Volumes (94371840/89702462/94371840/89417242/94371840/89521169 B) + SHA256SUMS.txt,
  kein `.unvollstaendig`;
* `sha256sum -c SHA256SUMS.txt` im Archivordner (eigener Aufruf): 6x `OK`;
* `cmp` jedes Volumes gegen release/: 6x bytegleich;
* `git show v0.8.25:release/SHA256SUMS.txt | diff - <archiv>/SHA256SUMS.txt` -> leer: die archivierten Summen sind
  bytegleich die des Release-Commits 2077bb8e ("release: v0.8.25 Zwischenversion ..."), 0 CR.

### A3 Versionslogik gegen die echten Tags (V3), eigener Aufruf der Lib

268 Tags, davon 264 der Form vX.Y.Z. `git tag --list 'v*' | ra_hoechste_version` = v0.8.25; unabhaengig per
numerischer Sortierung (awk %05d je Stelle) ebenfalls v0.8.25. Folgeversionen: v0.8.25 -> v0.8.26, v0.8.99 -> v0.9.0,
v0.9.99 -> v0.10.0, v0.7.99 -> v0.8.0, v0.8.9 -> v0.8.10, v0.99.98 -> v0.99.99, v0.99.99 und v1.99.99 -> keine
(rc 1, Skript verlangt dann `--version`). Uebergaenge in der echten Tag-Geschichte (Y-Wechsel): v0.2.9->v0.3.0,
v0.3.99->v0.4.0, v0.4.9->v0.5.0, v0.5.9->v0.6.0, v0.6.9->v0.7.0, v0.7.99->v0.8.0 - seit v0.8.9->v0.8.10 gilt der
Uebertrag erst bei 99; das Skript folgt dem juengsten Brauch und build.gradle:43-49 (selbst gelesen: Zeilen 43-49
`computeVersionCode` = X*1000000 + Y*10000 + Z*100).
Die Version erreicht jedes Artefakt, das eine traegt: APK (versionName/versionCode ueber `build_android.sh --version`,
build.gradle:203-204), Paketnamen + README.txt (`make_package.sh --version`, :97/:689), Archivordner, Tag. Die PC-exe
traegt keine Release-Version (einzige Konstante `RE15_REBORN_VERSION_STRING "v0.1.0 Phase 4.1"`, re15_engine.h:24,
nirgends benutzt).
Aufruffehler (eigener Aufruf): `--ab gibtsnicht` rc 2; `--version v0.8.25` rc 2 "nicht groesser als der hoechste Tag
v0.8.25 - die APK waere ein Downgrade (build_android.sh:195-199)"; `--version v0.8.100` rc 2; `--help` 48 Zeilen
Kurzanleitung.

### A4 Tests der Spur, selbst gefahren (Windows, Spurbaum, unter Fremdlast, -j3)

`ctest --test-dir re15_port/build -R r38_releaseskript -j3 --timeout 1200 --output-on-failure` (21:10-21:30;
gleichzeitig liefen laut Vorbedingungszeile cmake 9-11 / ctest 8-9 anderer Agenten):
version 39/39 (160 s), bausteine 18/18 (27 s), trocken 32/32 (384 s), ablauf 47/47 (192 s), fehler_bau 25/25 (572 s),
cmd 19/19 (364 s) - **fehler_umfeld: Timeout nach 1200.84 s** bei 27 von 29 Pruefungen (die beiden `--kein-push`-
Pruefungen fehlten). Ergebnis woertlich: `86% tests passed, 1 tests failed out of 7`. Der Bau-Agent mass 207 s fuer
denselben Test; hier ~6x langsamer (alle Teile 4-6x langsamer als im Dossier).

### A5 fehler_umfeld einzeln nachgefahren (21:33-21:45)

`ctest --test-dir re15_port/build -R unit_r38_releaseskript_fehler_umfeld -V --timeout 1200` ->
`== r38_releaseskript fehler_umfeld: 29/29 Pruefungen gruen ==`, `100% tests passed, 0 tests failed out of 1`,
`Total Test time (real) = 747.60 sec` (Fremdlast waehrenddessen: bash 59, cmake 10, ctest 7 Prozesse). Funktional
gruen; die Laufzeit liegt unter dieser Last aber nur 1,6x unter der Grenze 1200 s (Dossier: 207 s; seriell in A11:
198 s) -> Hinweis H6.

### A6 Doppelklick-Weg im Spurbaum (21:35, V1)

`printf '4\n' | cmd.exe /c <baum>\release\release_alles.cmd` (ohne Argumente = Doppelklick) -> RC=0 in 125 s:
`Git-Bash: C:\Program Files\Git\bin\bash.exe`, danach der komplette Trockenlauf (`TROCKENLAUF-OK v0.8.26`), dann
die Wahl `[1] Release bauen, packen, archivieren ... [2] wie 1, am Ende Release-Commit + Tag + Push ... [3] nur
Release-Commit + Tag + Push ... [4] nichts tun`, `Auswahl [1-4]: 4`, `Druecken Sie eine beliebige Taste`. Kein neuer
Protokollordner, nichts gebaut. `release_alles.cmd` im Arbeitsbaum CRLF (`git ls-files --eol`:
`i/lf w/crlf attr/text eol=crlf`), reines ASCII (Pruefung je Zeile). Die Wege [1]/[2]/[3] faehrt der Test
`unit_r38_releaseskript_cmd` (19/19, A4) in einem Wegwerf-Repo mit Attrappen.

### A7 Echte Schnittstellen der vier Einzelskripte (selbst gelesen, nicht gelaufen)

| Erwartung in release_alles.sh | Quelle selbst gelesen |
|---|---|
| Marke `WIN-CROSS-BUILD-OK`, Artefakt `release/win_out/re15_pc.exe` | docker_win_build.sh:101-103 (`cp ... release/win_out/re15_pc.exe`, `echo "WIN-CROSS-BUILD-OK: ..."`) |
| Marke `ANDROID-BUILD-OK`, APK `release/re15_port_<V>_android.apk`, `SHA256SUMS_android.txt` | build_android.sh:200 `NAME=re15_port_${VERSION}_android`, :294 `OUT=$HERE/${NAME}.apk`, :295 `.ungeprueft`, :348 SUMS, :355 Marke |
| Marke `LINUX-BUILD-OK` + Zaehlzeile `Tests: N gelaufen = N registriert` | docker_linux_build.sh:131 und :142 (echo auf stdout, kommt ueber `docker run` ohne -t beim Aufrufer an - derselbe Weg wie Windows) |
| Marke `== Fertig ==`, Volumes aller Plattformen, alte Volumes weg | make_package.sh:1051; :986-1036 (Austausch je gepackter Plattform, Android nur mit ANDROID_GEZIPPT); :931 "(kein Android-Paket: ... fehlt" (Dossier zitiert :929) |
| Version ohne `--version` falsch | make_package.sh:69 `VERSION="v0.1.2"`; build_android.sh:193 `git describe --tags --always` |
| versionCode | build.gradle:43-49 `computeVersionCode` X*1000000+Y*10000+Z*100 |
| Docker noetig | build_win_cross.sh:35-38, build_linux_deck.sh:59-66 |

Echter Windows-Schritt des Bau-Agenten (N5) am Artefakt nachgeprueft: `release/logs/v0.8.26_20261016-172759/
01_windows.log` Zeile 1016 `WIN-CROSS-BUILD-OK: 5295813 Bytes`, Zeile 1019 `== Wrapper gesamt: 107 s (rc=0)`;
`release/win_out/re15_pc.exe` 5295813 B, mtime 17:29:46, sha256 96cd1e7305cd9515... = die Zeile im 00_ablauf.log.
Android, Linux und Paket liefen in der Spur NICHT echt (VERTRAG §2: den echten Release faehrt der Orchestrator).

### A8 "alle Systeme" (V2)

Ausgeliefert wurden bisher genau drei Saetze (win64, linux_steamdeck_x64, android; RA_PLATTFORMEN, make_package.sh).
arm64 steckt in der APK (build.gradle:206 `abiFilters "arm64-v8a", "x86_64"`), der PSX-Zielbau wurde nie gebaut
(Memory reai-v2-psx-build-gap). Das Skript ruft alle drei Bauten und verlangt nach `paket` alle drei
`re15_port_<V>_<plattform>.zip` (s_paket, release_alles.sh:583-586), sonst Abbruch "nicht alle Systeme gepackt".

### A9 Eigener Fehlerpfad-Versuch: Push scheitert im Schritt abschluss (21:50)

Eigenes Skript (nicht im Repo, `<scratchpad>/pruef_push.sh`): Wegwerf-Repo mit dem ECHTEN release_alles.sh + Lib,
nur die Schritt-Attrappen der Spur; Tag v0.8.25, bare origin.
1. Ganzer Lauf ohne --tag: rc 0, `RELEASE-ALLES-OK v0.8.26`.
2. Notes `# v0.8.26 ...` oben, origin auf einen nicht vorhandenen Pfad, `--version v0.8.26 --ab abschluss --tag`:
   rc 1, `Commit b3f45ba, Tag v0.8.26`, `!!! ABBRUCH in Schritt 'abschluss': git push endete mit Rueckgabe 128`,
   `Nach der Behebung weiter mit: bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag`.
   Lokal stehen Release-Commit und annotierter Tag.
3. origin repariert, GENAU die vorgeschlagene Zeile: **rc 2** `--version v0.8.26 ist nicht groesser als der hoechste
   Tag v0.8.26 - die APK waere ein Downgrade (build_android.sh:195-199)`; origin weiter ohne Commit und ohne Tag.
   Ohne `--version` (bzw. Doppelklick-Wahl [3]) waehlt das Skript v0.8.27 und bricht an den Notes ab
   (`[XX] --tag: release/RELEASE_NOTES.md beginnt nicht mit '# v0.8.27 ...'`).
-> Nach einem gescheiterten Push fuehrt die angebotene Wiederaufnahme nicht weiter; nur ein Push von Hand
(`git push origin master v0.8.26`) hilft, und das sagt die Meldung nicht. -> Mangel M1.

### A10 Docker-Start auf dieser Maschine (22:00-22:10)

`docker desktop start --timeout 300` (Docker Desktop war aus): nach 339 s `Failed to start Docker Desktop` / `Docker
Desktop is still starting: context deadline exceeded`, `docker info` danach `open //./pipe/dockerDesktopLinuxEngine:
The system cannot find the file specified`; `wsl -l -v`: `docker-desktop Stopped`. Der Bau-Agent mass 19 s / 24 s.
Fuer das Skript heisst das: ein echter Lauf bricht in diesem Zustand korrekt VOR dem Bauen ab ("Docker nicht
bereit", release_alles.sh:288-289) - fail-closed, kein falsches Gruen. Nachpollen (`docker info` alle 10 s, 300 s):
`nicht bereit nach 300 s`. Backend-Protokoll (`%LOCALAPPDATA%\Docker\log\host\com.docker.backend.exe.log`,
20:10:55Z): `running engine: waiting for the VM setup to be ready: bootstrapping in the main distro: listing WSL
distros: running WSL command wsl.exe ...` - WSL-Bootstrap scheitert (Ursache offen: Last, WSL, oder die Sandbox
dieser Sitzung). Danach `docker desktop stop` + Rest-Prozess PID 20556 per PID beendet: 0 Docker-Prozesse =
Ausgangszustand. Die Linux-Gegenprobe der 7 Tests im Bau-Image (`<scratchpad>/linux_im_container.sh`, Registrierung
ueber die echte Sonde, release/* als Rueckfall-Links wie rueckfall_links.sh) konnte deshalb NICHT laufen - die
Linux-Angabe des Bau-Agenten (6/6 + SKIP) bleibt von mir ungemessen (nur gelesen: git steht in linux_deps.sh:54 PKGS,
zip/unzip nicht; der Test ersetzt fehlende zip/unzip, test_r38_releaseskript.sh:94-107; cmd endet ausserhalb Windows
mit 77, probes/r38_releaseskript.cmake:44 SKIP_RETURN_CODE 77).

### A11 Serieller Lauf wie in der Integration (21:46-22:35, ohne -j, Fremdlast anderer Agenten)

`ctest --test-dir re15_port/build -R "r38_releaseskript|r35_android_pruefkette|r35_android_bash_mutanten"
--timeout 1500 --output-on-failure`, woertlich:
```
1/9 Test #479: unit_r35_android_pruefkette ............   Passed  613.68 sec
2/9 Test #480: unit_r35_android_bash_mutanten .........***Timeout 1501.37 sec
3/9 Test #768: unit_r38_releaseskript_version .........   Passed  132.83 sec
4/9 Test #769: unit_r38_releaseskript_bausteine .......   Passed   41.17 sec
5/9 Test #770: unit_r38_releaseskript_trocken .........   Passed  231.77 sec
6/9 Test #771: unit_r38_releaseskript_ablauf ..........   Passed   34.89 sec
7/9 Test #772: unit_r38_releaseskript_fehler_bau ......   Passed  101.97 sec
8/9 Test #773: unit_r38_releaseskript_fehler_umfeld ...   Passed  197.55 sec
9/9 Test #774: unit_r38_releaseskript_cmd .............   Passed   66.72 sec
89% tests passed, 1 tests failed out of 9
```
Die 7 Tests der Spur: seriell 7/7 gruen, zusammen 811 s (Dossier 593 s). Pruefzahlen je Teil (aus dem -j3-Lauf A4
und A5, gleiche Skripte): version 39/39, bausteine 18/18, trocken 32/32, ablauf 47/47, fehler_bau 25/25, fehler_umfeld
29/29, cmd 19/19 - genau die Zahlen des Dossiers.
`unit_r35_android_bash_mutanten` lief in die Grenze 1500 s; schon der UNVERAENDERTE Kontrolllauf meldet
`Kontrolle (unveraendert): Rueckgabe 124 Zeitlimit` - Last. Der Test liest nur release/apk_pruefen.sh,
release/python_finden.sh und seinen eigenen Ordner (test_r35_android_bash_mutanten.sh:14-21); keine dieser Dateien ist
im Diff (`git diff master...HEAD --name-only -- release/apk_pruefen.sh release/python_finden.sh
release/apk_asset_gate.py release/gate_urteil.py re15_port/tests/unit/r35_android` = 0 Dateien). Nicht der Spur
zuzurechnen; der Bau-Agent mass ihn gruen (634.80 s).

## 3. FUND: das echte Archiv existiert nicht mehr

* `Test-Path C:\workspace\Re15Data` = False, `C:\workspace\Re15Data\re15_packages_archiv` = False;
  `cmd /c dir C:\workspace /a` zeigt nur `Gemeinde`, `git`, `keepass`, das Verzeichnis `C:\workspace` selbst
  zuletzt geaendert **16.10.2026 18:26**. Auf C:\ und D:\ (oberste Ebene) kein `Re15*`-Ordner.
* Der Bau-Agent hat um 17:19 noch gegen dieses Archiv gemessen (Protokoll
  `release/logs/v0.8.26_20261016-171913/00_ablauf.log`: `Vorversion v0.8.25 war schon im Archiv, geprueft:
  SHA256SUMS.txt 6/6 OK; SHA256SUMS_android.txt 1/1 OK`); Dossier M5 (16:30-16:55): 9 Versionen, 7456 MB.
* Zwischen 17:19 und 18:26 (bzw. spaetestens 21:13) ist `C:\workspace\Re15Data` verschwunden - samt allem, was
  dort sonst lag (Agenten-Transkripte nennen z.B. `C:/workspace/Re15Data/extracted_re2_items`).

### 3.1 Kann die Spur das ausgeloest haben? Nein (gemessen, soweit pruefbar)

* Jede Loesch-/Verschiebe-Stelle im Spur-Code (grep `rm|mv|rmdir|Remove-Item`): release_alles.sh:411/417 (Sperre
  `release/logs/.release_alles.sperre`), :527/:538/:555 (Artefakte in release/), :630 (alte APK in release/, nur wenn
  bytegleich im Archiv), release_alles_lib.sh:213 `rm -rf "$wurzel/.$version.unvollstaendig"` (nur der Zwischenordner)
  und :228 `mv` Zwischenordner -> Versionsordner; Tests :57/:229 nur unter dem ctest-Arbeitsordner; Attrappen nur im
  Wegwerf-Repo. Kein Pfad, der das Archiv-Wurzelverzeichnis oder `Re15Data` loescht.
* Alle Tests uebergeben `--archiv` bzw. `RE15_ARCHIV` auf einen Wegwerf-Ordner (test_r38_releaseskript.sh:112, :589,
  :594, :600-624).
* Transkripte der Sitzung c41eae99 (458 Agenten-Dateien) nach Befehlen mit `Re15Data`/`re15_packages_archiv`
  durchsucht: nur lesende Befehle (Get-ChildItem/Test-Path/ls), kein rm/Remove-Item/Move-Item. Papierkorb leer.
* Ursache damit offen (Nutzer? anderes Programm?). FOLGE fuer den Release v0.8.26: der Trockenlauf meldet
  `Vorversion v0.8.25 fehlt im Archiv - Schritt 'alt' legt ... an`; der Schritt alt legt das Archiv neu an und bringt
  v0.8.25 aus release/ hinein (A2 zeigt genau das mit echten Daten) - die frueheren v0.8.7-v0.8.14 kann das Skript
  nicht wiederherstellen. Der Orchestrator/Nutzer muss das wissen, BEVOR er den Release faehrt.

## 4. Gates

### 4.1 RE-Gate / Belege
Der Diff enthaelt KEIN Spielverhalten (nur Bau-Umgebung) - es gibt keine @0x-Adresse zu zitieren oder zu
disassemblieren; die Spur belegt jede Regel mit Datei:Zeile oder Messung bzw. kennzeichnet PORT-WAHL. Stichproben
SELBST nachgelesen (an Stelle der Disassembly): build.gradle:43-49 (versionCode-Formel) stimmt;
build_android.sh:193/:195-199/:355 stimmen; docker_win_build.sh:103, docker_linux_build.sh:125-130/:131/:142 stimmen;
make_package.sh:69/:22-28/:160 (check_binary_fresh)/:986-1049 (Volumentausch)/:1051 stimmen; build_win_cross.sh:35-38,
build_linux_deck.sh:59-66 stimmen; analysis/befunde_runde32/release_v0818.md:24 ("15:08:18 gestartet, 15:08:38
bereit") stimmt. Ungenau: Dossier und release_alles.sh:581 zitieren make_package.sh:929 fuer "kein Android-Paket" -
die Zeile ist :931 (Hinweis H3).
PORT-WAHL gekennzeichnet: MIN_FREI_GB=10 (release_alles.sh:72-73), Docker-Wartegrenze 300 s / Takt 5 s
(docker_bereit.sh:22-24), Reihenfolge windows->android->linux (Kopf :46, Dossier §3), annotierter Tag (Dossier §3).
Ohne Kennzeichnung: die CLI-Zeitgrenzen `timeout 60 docker info` / `timeout 30 docker desktop version`
(docker_bereit.sh:41/:58) - Bau-Umgebung, ohne Wirkung auf Spiel oder Paket (Hinweis H2).
Rate-Woerter im Diff (`deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME|vorlaeufig|geschaetzt`): 0 Treffer.
Erklaert die Loesung den gemessenen Ausgangsbefund (Dossier §1)? Ja: Dossier-M1/M2 (falsche Standard-Versionen) ->
eine Quelle `--version $NEU` fuer build_android + make_package (Test ablauf prueft die Aufrufzeilen, A3 die
Rechnung); Dossier-M3 (Docker aus) -> docker_bereit.sh
(A1/A10: pruefen bzw. starten, sonst Abbruch vor dem Bauen); Pipe schluckt Fehler -> PIPESTATUS[0] (Test fehler_bau
rc 7 durch tee); altes Artefakt -> Frische gegen Schrittbeginn (Test fehler_bau 2020-Zeitstempel); still fehlender
Android-Satz -> Pflicht aller drei Saetze (Test fehler_bau).

### 4.2 Vertrag
* Pfade: nur `release/` (gehoert laut VERTRAG §1.4 ausschliesslich Spur V), die Sonde `probes/r38_releaseskript.cmake`
  (GLOB tests/unit/CMakeLists.txt:4060, keine CMakeLists geaendert), Testordner, Dossier. TABU-Dateien
  (`release/RELEASE_NOTES.md`, `platform/android/`, `shared_assets/PSX/*`) unberuehrt.
* Keine Bank-9-Bits, Nachrichten-Ids, Slots, Ereignisse.
* Nebenwirkungen: `release/.gitignore` `logs/` trifft keine versionierte Datei (`git ls-files release | grep /logs/`
  = 0); `release/.gitattributes` `*.cmd text eol=crlf` gilt nur unter release/ (einzige .cmd dort:
  release_alles.cmd; `re15_port/targets/psx/build.cmd` liegt ausserhalb). Suite +7 Tests (ctest -N 795):
  RE15_MIN_TESTS (local_build.sh:368, Standard 788) muss in der Integration angehoben werden (Dossier §6 nennt es).
  Die vorhandenen Release-Tests r35_android_pruefkette/bash_mutanten pruefen nur apk_pruefen.sh/apk_asset_gate.py/
  gate_urteil.py (test_r35_android_*.sh:14-20/:40-223) - vom Diff unberuehrt; pruefkette selbst gefahren: Passed
  613.68 s.

### 4.3 Abweichung vom Orchestrator-Hinweis "alte Archivversion entfernen"
r38.js:131 (Hinweis an den Bau-Agenten) und UEBERGABE §4 ("alte Version aus dem Archiv") verlangen das Loeschen der
Vorversion im Archiv; die Spur loescht dort nichts (Dossier §2.1). Gegen den WORTLAUT ("die alte ins Archiv zu
bringen") ist die Spur richtig: nach dem Lauf liegt die alte Version im Archiv (A2); mit dem Loeschen laege sie gerade
NICHT dort. Die Frage, ob das Archiv kuenftig auf N Versionen gekuerzt wird, ist eine echte Nutzer-Entscheidung
(Memory reai-v2-immer-paketieren 2026-08-24 "das letzte davor entfernen" ist mehrdeutig) - kein Mangel der Spur.

## 5. Urteile je Teilpunkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg (Kurz) |
|---|---|---|
| V1 "einfache Moeglichkeit - ueber Skript oder sowas" | teilweise | Normalweg erfuellt: Doppelklick `release\release_alles.cmd` (A6: Git-Bash gefunden, Trockenlauf, Wahl [1]-[4], Rueckgabe durchgereicht), ein Aufruf `bash release/release_alles.sh` (A1), `--help` + README; Wege [1]/[2]/[3] im Test cmd 19/19. ABER: scheitert der Push (Wahl [2]/[3], `--tag`; ~0,5 GB neue Volumes je Release - der fehleranfaelligste Schritt), fuehrt weder die angebotene Wiederaufnahme-Zeile noch Wahl [3] weiter (A9, Mangel M1): rc 2 "Downgrade" bzw. Sprung auf v0.8.27 |
| V2 "die Ports fuer alle Systeme zu bauen" | erfuellt (Massstab VERTRAG §2) | Kette windows -> android -> linux -> paket mit Rueckgabe + Marke + frischem Artefakt je Schritt, Pflicht aller drei Saetze (A7/A8, Tests ablauf 47/47, fehler_bau 25/25); echter Windows-Schritt am Artefakt nachgeprueft (A7); Android/Linux/Paket echt erst im Release-Lauf des Orchestrators |
| V3 "neue Versionsnummer - minor" | erfuellt | hoechster Tag v0.8.25 -> v0.8.26, Uebertrag bei 99 (A3, eigene Rechnung gegen 264 echte Tags), eine Quelle `--version $NEU` fuer APK + Pakete (Test ablauf), Grenzen/Downgrade-Sperre (A3), Test version 39/39 |
| V4 "die alte ins Archiv zu bringen" | erfuellt | Schritt alt mit echten v0.8.25-Volumes in ein leeres Archiv: 6/6 bytegleich, SUMS = Release-Commit 2077bb8e (A2); vorhandene Vorversion wird nur geprueft (Protokoll 17:19 des Bau-Agenten); im Archiv wird nichts geloescht (§3.1 grep), alte APK verlaesst release/ nur bytegleich archiviert (Test ablauf) |

## 6. Maengel (nummeriert, nachpruefbar)

**M1 - Gescheiterter Push im Schritt abschluss: die angebotene Wiederaufnahme fuehrt nicht weiter (muss).**
* Ort: release_alles.sh:683-688 (Commit + annotierter Tag, DANN `lauf "git push" ...`); bei Push-Fehler meldet die()
  (:153) `Nach der Behebung weiter mit: bash release/release_alles.sh --version vX.Y.Z --ab abschluss --tag`. Diese
  Zeile scheitert danach zwangslaeufig: der lokale Tag vX.Y.Z existiert schon, also bricht :198-201 mit rc 2 ab
  ("nicht groesser als der hoechste Tag ... Downgrade"); ohne `--version` (auch Doppelklick-Wahl [3]) rechnet :205
  die Folgeversion vX.Y.(Z+1) und :259-260 bricht an den Notes ab. Einen "nur noch pushen"-Weg gibt es nicht, und die
  Meldung nennt `git push origin master vX.Y.Z` nicht.
* Messung: A9 (rc 1 beim Push, `Commit b3f45ba, Tag v0.8.26` lokal; die vorgeschlagene Zeile -> rc 2; origin danach
  weiter ohne Commit/Tag).
* Nachpruefbar ohne meinen Scratchpad: Wegwerf-Repo wie test_r38_releaseskript.sh `fx_bauen` (Tag v0.8.25, bare
  origin), `release_alles.sh --archiv <A> --min-frei-gb 1` (rc 0), Notes `# v0.8.26 ...` oben,
  `git remote set-url origin <nicht vorhanden>`, `release_alles.sh --archiv <A> --min-frei-gb 1 --version v0.8.26
  --ab abschluss --tag` (rc 1 + Hinweiszeile), origin zuruecksetzen, Hinweiszeile ausfuehren -> heute rc 2.
* Soll: nach einem Push-Fehler (Commit + Tag stehen lokal) fuehrt der gemeldete naechste Schritt zum Ziel - z.B. die
  Meldung nennt den Push von Hand, oder `--ab abschluss --tag` (und Wahl [3]) erkennt "HEAD = Release-Commit mit Tag
  vX.Y.Z, origin hat ihn nicht" und pusht nur. Ein Riegel (fehler_umfeld oder ablauf) faehrt genau diesen Fall bis zum
  gepushten Tag. Betrifft V1 (Doppelklick-Wahl [2]/[3]); Bauen/Version/Archiv sind nicht betroffen.

## 7. Hinweise (keine Maengel der Spur; fuer Orchestrator/Nutzer)

* **H1 (wichtig, vor dem Release v0.8.26):** `C:\workspace\Re15Data` (mit `re15_packages_archiv`, 9 Versionen
  7456 MB laut Dossier M5, und weiteren Daten) existiert seit spaetestens 21:13 nicht mehr (§3); die Spur kann es
  nicht ausgeloest haben (§3.1). Ein Release-Lauf legt das Archiv neu an und bringt v0.8.25 aus release/ hinein (A2);
  v0.8.7-v0.8.14 sind ohne den Nutzer nicht wiederherstellbar. Den Nutzer fragen, ob das Absicht war (verschoben?),
  bevor das Skript den Standardpfad neu anlegt; sonst `--archiv`/`RE15_ARCHIV` auf den neuen Ort.
* H2: `docker info` haengt auf dieser Maschine bei gestopptem Docker Desktop bis zur Grenze 60 s
  (docker_bereit.sh:41; gemessen `docker info rc=124`); Trockenlauf/Doppelklick brauchen deshalb 125-161 s, ohne
  Fortschrittszeile waehrend des Wartens. Die Grenze ist nicht als PORT-WAHL gekennzeichnet (Bau-Umgebung).
* H3: "make_package.sh:929" (Dossier, release_alles.sh:581) meint :931.
* H4: Docker Desktop startete aus dieser Abnahme-Sitzung nicht (A10, WSL-Bootstrap-Fehler) - die Linux-Angabe
  "6/6 + cmd SKIP im Bau-Image" ist von mir NICHT gemessen. Der Release braucht Docker; das Skript prueft das und bricht
  sonst vor dem Bauen ab. Vor dem Release v0.8.26 `docker info` pruefen.
* H5: Zweistufiger Ablauf ([1], dann [3] bzw. `--ab abschluss --tag`): die Release-Commit-Nachricht nennt die Lauf-Id
  des ABSCHLUSS-Laufs als "Gebaut und gepackt mit ... (Lauf ...)", die Bau-Ergebnisse fehlen (gemessen im A9-Repo:
  `Gebaut und gepackt mit release/release_alles.sh (Lauf v0.8.26_20261016-215057)` = der abschluss-Lauf). Kosmetik.
* H6: Laufzeit der Tests unter Last: seriell 811 s fuer alle 7 (A11, fehler_umfeld 198 s); einzeln unter starker
  Fremdlast fehler_umfeld 748 s (A5); mit -j3 unter Fremdlast Timeout bei 1200 s (A4). Die Dossier-Aussage "1200 s
  laesst genug Luft" gilt seriell, nicht parallel unter Last - fuer die serielle Integrationssuite ohne Agenten reicht
  es. Gezielte Laeufe in belasteten Baeumen ohne -j fahren. Die Windows-Suite waechst um ~10-14 min.
* H7: RE15_MIN_TESTS (local_build.sh, Standard 788) in der Integration auf die neue Zahl anheben (+7).
* H8: Abweichung vom Orchestrator-Hinweis "alte Archivversion entfernen" ist wortlautgetreu (§4.3); die
  Entscheidungsfrage des Dossiers (Archiv kuerzen?) an den Nutzer weitergeben.

## 8. Ergebnis

**bestanden: NEIN.** V2, V3, V4 erfuellt (Massstab VERTRAG §2: Trockenlauf + Versionslogik-Tests, dazu echte
Daten im Schritt alt, echte Versionsrechnung gegen die Repo-Tags, seriell 7/7 Tests gruen); V1 teilweise wegen M1.
Gates: RE-Gate (Datei:Zeile statt Adressen, Stichproben stimmen), Vertrag (nur release/ + Sonde + Tests) und
Nebenwirkungen halten. Nachbesserung: nur M1 (klein); H1 vor dem Release klaeren.
