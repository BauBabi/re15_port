# Runde 38 Spur V — releaseskript (Dossier)

Baum `.claude/worktrees/r38_releaseskript` (Zweig `r38/releaseskript`, Basis e2cc7673). Angelegt 2026-10-16 16:28 (Uhr).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
> Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

Lesart des Orchestrators (AUFTRAG.md, "Lesarten"): "minor" = wie alle bisherigen Releases die LETZTE Stelle hochzaehlen
(0.8.25 -> 0.8.26, 0.8.99 -> 0.9.0 wie v0.7.99 -> v0.8.0); das Skript nimmt optional eine explizite Version.

## Stand

(laufend, neueste Eintraege oben)

* 2026-10-16 23:40: NACHBESSERUNG 1 FERTIG: gezielt `100% tests passed, 0 tests failed out of 7` (-R r38_releaseskript),
  echter Trockenlauf rc 0 ohne falschen Hinweis. Abschnitt 8.
* 2026-10-16 23:25: Umsetzung M1 committet (88dda648, 10fe5afa), Bau `LOCAL-BUILD-OK (build)`; gezielte Tests laufen
  (Maschine stark ausgelastet: 105 bash, 13 cmake, 6 ctest, 15 Test-exes anderer Spuren).
* 2026-10-16 23:00: NACHBESSERUNG 1 begonnen (Abnahme 0: Mangel M1, gescheiterter Push im Schritt abschluss) -
  Abschnitt 8.
* 2026-10-16 18:17: FERTIG. Endlauf auf 98652210: `100% tests passed, 0 tests failed out of 7` (-R r38_releaseskript),
  Release-Modul `100% tests passed, 0 tests failed out of 2` (r35_android_pruefkette|bash_mutanten).
* 2026-10-16 17:54: gezielte Tests gruen (Windows 7/7, Linux-Bau-Image 6/6 + cmd sichtbar SKIP), echte Messungen
  N1 (Trockenlauf), N2 (Schritt alt gegen das echte Archiv), N5 (Docker-Start 19 s + echter Windows-Cross-Bau);
  Docker Desktop danach wieder gestoppt. Abschnitte 3-7 geschrieben.
* 2026-10-16 17:10-17:45: Umsetzung (release_alles.sh, _lib, docker_bereit.sh, .cmd, README), Tests mit Attrappen;
  Funde beim Testen: geerbtes MSYS_NO_PATHCONV (cmd-Weg), zip/unzip fehlen im Linux-Bau-Image, Testzahl Windows/Linux.
* 2026-10-16 16:55: Messung vorher + Ist-Ablauf belegt (Abschnitte 1/2), Entwurf (Abschnitt 3) folgt.
* 2026-10-16 16:28: Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE Runde 38, UEBERGABE Runde 37 §4,
  Memory-Notizen zur Release-Kette gelesen.

## 1. Messung vorher (2026-10-16, ~16:30-16:55, gemessen, nicht vermutet)

Es gibt KEIN Skript, das die Kette faehrt. Ein Release sind heute vier Einzelskripte + Handarbeit (Archiv, Commit, Tag,
Push); die Version wird an vier Stellen von Hand getippt (build_android, make_package, Archivordner, Tag).

| # | Messung | Ergebnis |
|---|---|---|
| M1 | `grep -n '^VERSION=' release/make_package.sh` | Zeile 69 `VERSION="v0.1.2"` - ein Aufruf ohne `--version` schnuert Pakete namens v0.1.2 |
| M2 | `release/build_android.sh:193` Standard = `git describe --tags --always` | heute `v0.8.25-6-gfed84ccd` (Spurbaum) bzw. `v0.8.25-7-g98d35560` (Hauptbaum) - APK/versionName mit Suffix statt neuer Nummer |
| M3 | `timeout 30 docker info` | rc=1, `open //./pipe/dockerDesktopLinuxEngine: The system cannot find the file specified` - Docker Desktop laeuft nicht; build_win_cross.sh:36-38 bricht dann mit "Weder docker noch podman nutzbar." ab |
| M4 | Tags (`git tag --list 'v*' --sort=v:refname`) | hoechster v0.8.25; Uebertraege v0.3.99 -> v0.4.0 und v0.7.99 -> v0.8.0, aber v0.8.9 -> v0.8.10 (kein Uebertrag bei 9); fruehe Ausreisser v0.2.9->v0.3.0, v0.4.9->v0.5.0, v0.5.9->v0.6.0, v0.6.9->v0.7.0; dazu Nicht-Versions-Tags `v0.1`, `v0.2`, `backup-before-pull-ed20f6d3`, `itembox-hybrid-v1`; v0.8.24/v0.8.25 leichtgewichtig, v0.8.20/v0.7.99 annotiert |
| M5 | Archiv `C:/workspace/Re15Data/re15_packages_archiv` (PowerShell) | 9 Ordner: v0.8.7 .. v0.8.14 (je 820/821 MB) + v0.8.25 (894 MB); je 6 Volumes + APK + SHA256SUMS.txt + SHA256SUMS_android.txt (v0.8.12-14 zusaetzlich RELEASE_NOTES.md) |
| M6 | `Get-PSDrive C` | frei 131 578 617 856 B (122,5 GiB) |
| M7 | Hauptbaum `release/` | v0.8.25-Volumes (6, getrackt) + APK v0.8.25 (385 208 598 B) UND die APK v0.8.24 (379 951 560 B, 2026-10-12) - alte APKs raeumt niemand weg (gitignoriert `.gitignore:63`, make_package.sh entfernt nur `re15_port_v0*.z*`, Zeilen 1018-1030) |
| M8 | Hauptbaum `git status --porcelain --untracked-files=all -- re15_port` | 0 Eintraege (sauber); `-- synchro`: 33 unversionierte Eintraege, darunter `synchro/STAGE2/room2001/` - make_package.sh copy_common (Zeilen 672-685) packt synchro/STAGE* aus dem ARBEITSBAUM, unversionierte Aufnahmen landen also im Paket, aber nicht im Release-Commit |
| M9 | Bau-Ausgaben gitignoriert? (`git check-ignore -v`) | re15_port/build*, android app/build, app/.cxx, _deps, .gradle, local.properties, release/{win_out,linux_out,lbuild,pkg-win,pkg-linux,wxbuild} alle ignoriert -> ein Bau macht re15_port/ nicht "schmutzig" |
| M10 | Platzbedarf je Lauf (Hauptbaum nach v0.8.25, PowerShell) | pkg-win 364 MB + pkg-linux 364 MB + android app/build 1530 MB + app/.cxx 74 MB + 6 Volumes ~535 MB + APK 385 MB (+ .ungeprueft- und Pruefkopie je 385 MB) + Archiv neu 894 MB (+ alt 894 MB, falls es fehlt) = ~6,2 GB auf C:, dazu die Docker-VM |

Folgen fuer den Nutzer heute: Version an vier Stellen von Hand, zwei Standardwerte falsch (M1, M2), Docker muss
von Hand gestartet werden (M3), Fehlschlaege einzelner Schritte nur ueber Rueckgabewert + Datum erkennbar
(Memory reai-v2-releasebau-pipe-schluckt-fehler: v0.8.15 meldete ein gepipter Linux-Bau exit 0 trotz rotem ctest).

## 2. Belege (Ist-Ablauf, Skripte, Zeilen)

Nicht Spielverhalten, sondern Bau-Umgebung: es gibt keine Original-Adresse. Belegt wird jede Regel/Konstante mit
Datei:Zeile oder Messung (Muster der vorhandenen Skripte: "Port-Wahl, keine Original-Adresse (Bau-Umgebung)").

| Regel / Wert | Beleg |
|---|---|
| Uebertrag bei 99 (Z=99 -> Y+1.0) | `re15_port/platform/android/app/build.gradle:43-49`: versionCode = Major*1000000 + Minor*10000 + Patch*100 - Patch 100 ergaebe denselben Code wie Minor+1 (0.8.100 -> 90000 = 0.9.0), Minor 100 denselben wie Major+1; dazu die Tags v0.3.99 -> v0.4.0, v0.7.99 -> v0.8.0 (M4); AUFTRAG.md "Lesarten" |
| neue Version muss GROESSER sein als jede vorhandene | `release/build_android.sh:195-199`: kleinerer versionCode -> "adb install -r lehnt die APK als DOWNGRADE ab" |
| x.y.z Pflicht | `release/build_android.sh:199` (sonst versionCode 1) |
| Ablauf/Reihenfolge der Einzelschritte | Runde 37 UEBERGABE §4 (Zeilen 127-133) + Eintrag 2026-10-14 10:40; Runde 32 `release_v0818.md` (Protokoll aller Schritte) |
| Erfolgsmarken der Einzelskripte | `docker_win_build.sh:103` "WIN-CROSS-BUILD-OK", `docker_linux_build.sh:142` "LINUX-BUILD-OK" + `:131` "Tests: N gelaufen = N registriert", `build_android.sh:355` "ANDROID-BUILD-OK", `make_package.sh:1051` "== Fertig ==" |
| Artefakte | `release/win_out/re15_pc.exe` (docker_win_build.sh:102), `release/linux_out/re15_pc` (docker_linux_build.sh:133), `release/re15_port_<v>_android.apk` + `SHA256SUMS_android.txt` (build_android.sh:294, 348), Volumes + `SHA256SUMS.txt` (make_package.sh 839-976) |
| make_package baut nicht, tauscht aber die Volumes im git-Index | make_package.sh:979-1049 (`git rm --cached` alt, `git add` Positivliste neu; KEIN Commit) |
| SHA256SUMS*.txt werden NICHT vorgemerkt | make_package.sh:1040-1045 (nur Volumes) - Release-Commit 2077bb8e enthielt sie trotzdem (8 Dateien: 6 R + 2 M) -> der Commit-Schritt muss sie selbst adden |
| Docker-Start dauert ~20 s | Runde 32 `release_v0818.md:24` ("15:08:18 gestartet, 15:08:38 bereit") |
| zip nur aus /c/msys64/usr/bin, HINTEN anhaengen | make_package.sh:22-28 (vorn verliert MSYS2-bash LOCALAPPDATA/ANDROID_SDK_ROOT) |
| Archiv-Inhalt je Version | M5 + Runde 32 `release_v0818.md` Abschnitt 6 (6 Volumes + APK + beide SUMS, `sha256sum -c` 6x + 1x OK) |

### 2.1 "die alte ins Archiv bringen" gegen "alte Archivversion entfernen"

Wortlaut 2026-10-16: "... und die alte ins Archiv zu bringen"; VERTRAG.md Spur V: "packt und bringt die alte Version ins
Archiv". Der bisherige Ablauf (seit HANDOVER_2026-08-25 Zeile 175, Runde 32 release_v0818.md Abschnitt 6, Runde 37 §4)
legt dagegen die NEUE Version ins Archiv und LOESCHT dort die vorige ("liegt weiter in Git"). Mit dem Loeschen waere die
alte Version nach dem Lauf gerade NICHT im Archiv - das widerspricht dem neuen Wortlaut. Aeltere Nutzer-Vorgaben dazu:
make_package.sh:980-981 (2026-08-22) "alte Packages vom Repo loeschen" spricht ausdruecklich vom REPO; Memory
reai-v2-immer-paketieren (2026-08-24) "auch im archiv, auch pushen und das letzte davor entfernen" nennt das Entfernen
im selben Zug wie das Pushen, meint also eher ebenfalls das Repo - eindeutig ist dieser Satz nicht, das Loeschen im
Archiv ist die Auslegung, die die Agenten seit HANDOVER_2026-08-25 daraus gemacht haben. Der neue Wortlaut ist
eindeutig und juenger. Umsetzung daher:
1. die alte Version (die Volumes in release/ vor dem Lauf) kommt ins Archiv bzw. wird dort geprueft (sha256sum -c), bevor
   irgendetwas gebaut wird;
2. die neue Version kommt zusaetzlich ins Archiv (stehende Anweisung "auch im archiv");
3. aus release/ verschwindet die alte Version (Volumes: make_package.sh; APK: nach geprueftem Archiv) - im Archiv bleibt sie;
4. im Archiv wird NICHTS geloescht. Entscheidungsfrage an den Nutzer in Abschnitt 7.

## 3. Umsetzung

Neue Dateien (alle unter `release/`, das nur Spur V aendern darf; kein Eingriff in die vier Einzelskripte):

| Datei | Zweck |
|---|---|
| `release/release_alles.sh` | EIN Aufruf: Version, Vorbedingungen, Schritte alt -> windows -> android -> linux -> paket -> archiv (-> abschluss mit `--tag`), je Schritt Protokoll + klarer Abbruch + Wiederaufnahme-Zeile |
| `release/release_alles_lib.sh` | Bausteine (von Skript und Tests geladen): Versionslogik, Volume-Namen, SHA256SUMS ohne `\r`, Frische, Archiv anlegen/pruefen |
| `release/docker_bereit.sh` | Docker pruefen/starten: `docker desktop start --timeout` (CLI v0.1.4 auf dieser Maschine), sonst `Docker Desktop.exe` per `Start-Process`; `--pruefen` fuer den Trockenlauf (0 bereit / 1 startbar / 3 nichts zu starten) |
| `release/release_alles.cmd` | Doppelklick: Git-Bash suchen (Program Files, Registry GitForWindows, `where git`; nie System32\bash.exe), Trockenlauf, Wahl [1] bauen [2] bauen + Commit/Tag/Push [3] nur Commit/Tag/Push [4] nichts; mit Argumenten 1:1 durchgereicht, Rueckgabe durchgereicht |
| `release/README.md` | Kurzanleitung (auch im Kopf von release_alles.sh und `--help`) |
| `release/.gitattributes` | `*.cmd text eol=crlf` (cmd.exe und Sprungmarken in LF-Dateien) |
| `release/.gitignore` | `logs/` (Protokolle + Sperre) |

Regeln/Konstanten mit Beleg (Bau-Umgebung, keine Original-Adresse):

| Wert | Beleg / Art |
|---|---|
| Folgeversion Z+1, bei Z = 99 -> Y+1.0; Y = 99 und Z = 99 -> keine automatische (dann `--version`) | build.gradle:43-49 (versionCode X*1000000 + Y*10000 + Z*100), Tags v0.3.99 -> v0.4.0 / v0.7.99 -> v0.8.0; "minor" (Wortlaut) schliesst den Sprung in die erste Stelle aus |
| X <= 999, Y/Z <= 99, keine fuehrenden Nullen | versionCode < 2^31; zwei Schreibweisen einer Version vermeiden (Tag, Paketname, versionName) |
| explizite Version > hoechster Tag | build_android.sh:195-199 (Downgrade-Sperre auf dem Geraet) |
| hoechster Tag = numerisches Maximum der Tags `vX.Y.Z` | M4 (Fremd-Tags v0.1, v0.2, backup-..., itembox-...; v0.8.10 > v0.8.9) |
| Erfolgsmarken je Schritt | docker_win_build.sh:103, docker_linux_build.sh:131/142, build_android.sh:355, make_package.sh:1051 |
| Frische: Artefakt nicht aelter als der Schrittbeginn, vorher geloescht | Memory reai-v2-releasebau-pipe-schluckt-fehler (v0.8.15: altes linux_out-Binary) |
| Rueckgabe = PIPESTATUS[0] des Schritts, nicht die von tee | dieselbe Memory ('| tail' meldete exit 0) |
| alle DREI Plattformen muessen nach `paket` als Satz da sein | make_package.sh:931 liefert ohne APK still nur PC-Saetze (Zitat in Nachbesserung 1 von :929 berichtigt, Abnahme H3) |
| `stand_pruefen`: re15_port/ sauber und `HEAD:re15_port` unveraendert vor/nach jedem Bau-Schritt | sonst stammen die Binaries aus verschiedenen Staenden |
| Release-Commit nur mit Paketdateien (Volumes, 2 SUMS, Notes) | Form von 2077bb8e / e8b3958f (8 Dateien); SUMS werden von make_package NICHT vorgemerkt (:1040-1045) |
| `--tag` verweigert, wenn nach dem Paketbau Paket-Inhalt committet wurde | Regel wie make_package.sh check_binary_fresh (:160-191), erweitert um Assets/synchro/pkg_files |
| Archivordner der neuen Version existiert + Neubau geplant -> Abbruch VOR dem Bauen | neue Zips haben andere Bytes (Zeitstempel); im Archiv wird nichts geloescht |
| SHA256SUMS ohne `\r` lesen | gemessen: frischer Checkout w/crlf, `sha256sum -c` -> `'a.bin'$'\r': No such file` |
| Mindestplatz 10 GiB | PORT-WAHL aus M10 (~6,2 GB je Lauf + Docker-VM) |
| Docker-Wartegrenze 300 s, Takt 5 s | PORT-WAHL; gemessener Start 20 s (release_v0818.md:24) |
| Reihenfolge windows -> android -> linux | PORT-WAHL: kurze Baue zuerst, der 3-h-Lauf zuletzt (keine Abhaengigkeit zwischen den Bauten) |
| annotierter Tag | PORT-WAHL (v0.8.20/v0.7.99 annotiert, v0.8.24/25 leichtgewichtig; annotiert traegt Datum + Autor) |
| `unset MSYS_NO_PATHCONV MSYS2_ARG_CONV_EXCL` am Anfang | gemessen im cmd-Test: geerbt -> "kein git-Repo: /tmp/claude/..." |

Optionen: `--version`, `--dry-run`, `--tag`, `--kein-push`, `--ab`, `--bis`, `--linux-jobs` (RE15_CTEST_JOBS), `--archiv`
(oder Umgebung RE15_ARCHIV), `--min-frei-gb`. Sperre `release/logs/.release_alles.sperre` (PID; verwaiste wird
uebernommen, fremde lebende bricht ab und bleibt stehen).

## 4. Messung nachher

Ein echter Release-Lauf ist NICHT Teil der Spur (dauert Stunden; den faehrt der Orchestrator fuer v0.8.26 mit dem
Skript). Gemessen wurde am echten Repo/Archiv, was ohne Release geht, und die ganze Kette mit Attrappen der vier
Schritt-Skripte (Abschnitt 5).

| # | Messung (echtes Repo, Spurbaum, 2026-10-16) | Ergebnis |
|---|---|---|
| N1 | `bash release/release_alles.sh --dry-run` (17:18, HEAD 37304753) | rc 0, `Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`, alle Vorbedingungen [ok] (Tag frei, re15_port/ sauber, Werkzeug sauber, Platz 98 GiB, zip aus /c/msys64/usr/bin, Python /c/Python310/python 3.10.11, Vorversion v0.8.25 liegt im Archiv), Docker `[--] ... startet Docker Desktop ('docker desktop start')`, 6 Schritte mit Befehlen, `TROCKENLAUF-OK v0.8.26`; nichts geschrieben (kein release/logs) |
| N2 | `bash release/release_alles.sh --bis alt` (17:19, echtes Archiv) | rc 0 in 24,5 s: `release/ v0.8.25: 6 Volumes, sha256sum -c 6/6 OK` (die SUMS-Datei ist in diesem Checkout CRLF - gelesen ohne \r), `Vorversion v0.8.25 war schon im Archiv, geprueft: SHA256SUMS.txt 6/6 OK; SHA256SUMS_android.txt 1/1 OK`, `RELEASE-ALLES-TEILLAUF-OK v0.8.26 (alt)`; Protokoll release/logs/v0.8.26_20261016-171913/ (gitignoriert, `git status` zeigt nur das Dossier), Sperre wieder weg; das Archiv nur gelesen |
| N3 | Vorher M1/M2 (falsche Standard-Versionen) | eine Quelle: `--version $NEU` an build_android.sh UND make_package.sh (Aufrufzeilen im Protokoll, Test `ablauf`) |
| N4 | Vorher M3 (Docker aus) | `docker_bereit.sh --pruefen` -> rc 1 "ein echter Lauf startet Docker Desktop ('docker desktop start') und wartet bis 300 s" |
| N5 | `bash release/release_alles.sh --ab windows --bis windows` (17:27:25-17:29:48, Docker Desktop war AUS) | rc 0 in 2 min 23 s: Vorbedingung `Container-Engine bereit nach 19 s: docker 27.5.1` (gestartet per `docker desktop start`; Runde 32 maß 20 s), danach das ECHTE build_win_cross.sh: Image 2 s, Quellbaum packen 25 s (5397 Dateien, 347 MB), auspacken 22 s, Configure 24 s, Compile+Link 25 s, `WIN-CROSS-BUILD-OK: 5295813 Bytes`, Wrapper 107 s; Schritt-Pruefung `windows OK in 0h01m48s: .../release/win_out/re15_pc.exe 5295813 B, 2026-10-16 17:29:46` (Marke + frisches Artefakt); `RELEASE-ALLES-TEILLAUF-OK v0.8.26 (windows)`. Gegenprobe Groesse: die v0.8.25-exe im Hauptbaum (release/win_out, 2026-10-14 07:24) hat ebenfalls 5295813 B - seit v0.8.25 kein Port-Code geaendert. Docker Desktop danach wieder gestoppt (`docker desktop stop`, `docker info` rc 1 = Ausgangszustand) |
| N6 | die Tests der Spur im Linux-Bau-Image (`docker run re15-linux-build:deb11`, git 2.30.2, Baum read-only als /src) | erster Lauf ROT: `[XX] zip fehlt`, `[XX] unzip fehlt` - das Bau-Image hat beides nicht (linux_deps.sh PKGS), also brach jeder Attrappen-Lauf in den Vorbedingungen ab. Die Vorbedingung ist fuer einen echten Lauf richtig (make_package braucht zip); der Test ersetzt jetzt FEHLENDE zip/unzip durch Platzhalter ausserhalb des Repos (unter Windows bleibt der echte Weg /c/msys64/usr/bin/zip im Test). Zweiter Lauf: version 38/38 (die Zusatzpruefung gegen die echten Repo-Tags entfaellt dort sichtbar: der Arbeitsbaum-.git-Verweis zeigt auf C:/...), bausteine 18/18, trocken 32/32, ablauf 47/47, fehler_bau 25/25, fehler_umfeld 29/29, zusammen 21 s. Dieser Fund haette sonst den Linux-Release-Bau v0.8.26 rot gemacht (Memory reai-v2-linux-test-portabilitaet) |
| N7 | Testzahl Windows = Linux | `unit_r38_releaseskript_cmd` ist ueberall registriert und meldet ausserhalb von Windows sichtbar SKIP (Rueckgabe 77, SKIP_RETURN_CODE): sonst haette Windows einen Test mehr, und docker_linux_build.sh:125-130 (gelaufen >= RE15_MIN_TESTS) wuerde rot, sobald die Untergrenze nach der Windows-Suite angehoben wird |

## 5. Tests

Registriert in `re15_port/tests/unit/probes/r38_releaseskript.cmake` (GLOB), Skript
`re15_port/tests/unit/r38_releaseskript/test_r38_releaseskript.sh <repo> <arbeit> <teil>`. Die Tests fahren das ECHTE
`release_alles.sh` + `release_alles_lib.sh` (+ das echte `.cmd`) in einem Wegwerf-Repo (git init, Tags v0.1/v0.2/
v0.8.9/v0.8.24/v0.8.25/backup-..., bare origin, eigenes Archiv, Vorversion v0.8.25 mit CRLF-SUMS wie in einem frischen
Checkout); nur die vier Schritt-Skripte + docker_bereit.sh + python_finden.sh sind Attrappen
(`r38_releaseskript/attrappen/`, steuerbar ueber FAKE_FAIL/FAKE_KEIN/FAKE_ALT/FAKE_OHNE_MARKE/FAKE_OHNE_ANDROID/
FAKE_ALTE_BLEIBEN/FAKE_DOCKER). Je Nutzer-Teilpunkt mindestens ein messender Riegel:

| Test | misst (Auszug) | Nutzer-Teilpunkt |
|---|---|---|
| unit_r38_releaseskript_version | Folgeversion (v0.8.25->26, v0.8.9->10, v0.7.99->v0.8.0, v0.3.99->v0.4.0, v0.99.99 -> keine), Gueltigkeit (0.8.26 -> v0.8.26; v0.08.1/v0.8.100/v0.100.0/rc1 ungueltig), numerischer Vergleich, hoechster Tag mit Fremd-Tags und CRLF, versionCode streng steigend ueber 300 Folgeversionen + Gegenprobe v0.8.100 = v0.9.0 = 90000, echte Repo-Tags gegen `sort -V`, Volume-Namen | "neue Versionsnummer - minor" |
| unit_r38_releaseskript_bausteine | sha256sum -c scheitert an CRLF (Gegenprobe), ra_sums_pruefen liest es; Archiv erst `.unvollstaendig`, dann fertig; vorhandener Ordner bleibt; veraenderte Quelle -> kein fertiger Ordner; veraendertes Volume wird gefunden; leere SUMS nicht gruen; altes/leeres/fehlendes Artefakt faellt durch | "die alte ins Archiv" (Pruefbarkeit) |
| unit_r38_releaseskript_trocken | --dry-run: Version, alle 6 Schritte mit Befehlen, schreibt NICHTS (Pruefsumme ueber Baum+Index+Tags+Archiv vorher = nachher), ruft keinen Schritt (nur `docker --pruefen`); --tag ohne Notes [XX]; --version 0.9.0 / v0.8.25 / v0.8.100; --ab paket ohne Bau-Ergebnisse [XX]; --ab windows --bis windows; --bis vor --ab; Docker nicht startbar; schmutziges re15_port/; Tags bis v0.7.99 -> v0.8.0; v0.99.99 -> rc 2 | "einfache Moeglichkeit" (vorher ansehen) |
| unit_r38_releaseskript_ablauf | ganzer Lauf: Reihenfolge windows, android --version v0.8.26, linux (3 Jobs), paket --version v0.8.26; Archiv v0.8.25 (die ALTE, 6 Volumes + APK + 2 SUMS, sha256sum -c, SUMS ohne CR) und v0.8.26 (3 Plattformen, bytegleich); release/ nur v0.8.26, alte APK weg; kein Commit/Tag ohne --tag; Index 12 Aenderungen; Protokolle 00-06; dann --ab abschluss --tag: Commit `release: v0.8.26 (...)` mit 15 Pfaden, annotierter Tag, Push (origin master + Tag); danach Trockenlauf v0.8.27 | alle vier Teilpunkte |
| unit_r38_releaseskript_fehler_bau | Linux rot (rc 7 durch die tee-Pipe) -> Abbruch, paket/archiv laufen nicht, stderr im Protokoll, Wiederaufnahme --ab linux baut nur linux + paket; Windows rot; altes Artefakt (2020); rc 0 ohne APK; ohne Marke; Paket ohne Android-Satz; alte Volumes geblieben; Paket rot | "fuer alle Systeme bauen" (kein falsches Gruen) |
| unit_r38_releaseskript_fehler_umfeld | Archiv v0.8.26 schon da -> Abbruch VOR dem Bauen, Ordner unangetastet; --ab archiv gleich = ok / anders = Abbruch; kaputtes Altarchiv -> Abbruch vor dem Bauen; anderes v0.8.25 im Archiv; schmutziger Baum/Werkzeug/unversionierte .c; Sperre lebend/verwaist; --tag ohne Notes/nicht master; Docker; Platz; Aufruffehler; --bis android; --tag nach Code-Commit verweigert; --kein-push | "die alte ins Archiv" (nichts geht verloren) |
| unit_r38_releaseskript_cmd | (Windows; sonst sichtbar SKIP 77) CRLF; `cmd /c release_alles.cmd --dry-run` rc 0; Rueckgabe durchgereicht (2); Doppelklick-Weg ueber die Pipe: Wahl 4 -> nichts gebaut; Wahl 1 -> ganzer Lauf; Wahl 2 danach -> Abbruch vor dem Bauen mit Hinweis; Wahl 3 -> Commit + Tag + Push | "einfache Moeglichkeit - Skript oder sowas" |

Ergebnisse:

* Windows (Spurbaum, `local_build.sh configure` + `build` = LOCAL-BUILD-OK, dann gezielt), Endstand 98652210, woertlich:
  `ctest --test-dir re15_port/build -V -R "r38_releaseskript" --timeout 1200 --output-on-failure` ->
  `100% tests passed, 0 tests failed out of 7` (`Total Test time (real) = 593.63 sec`), je Teil aus der -V-Ausgabe:
  version 39/39 (129 s), bausteine 18/18 (5 s), trocken 32/32 (46 s), ablauf 47/47 (31 s), fehler_bau 25/25 (112 s),
  fehler_umfeld 29/29 (207 s), cmd 19/19 (64 s) - Maschine dabei von anderen Spuren ausgelastet (Hinweis der
  Vorbedingung: cmake 9, ctest 8). Vorlauf auf 97b946db ebenso 7/7 (598.61 s).
* Linux (Bau-Image `re15-linux-build:deb11`, git 2.30.2, cmake 3.28.6), Registrierung wie im Release-Container mit
  einem Mini-Projekt aus `probes/r38_releaseskript.cmake`: `ctest -R r38_releaseskript` ->
  `100% tests passed, 0 tests failed out of 7`, `7 - unit_r38_releaseskript_cmd (Skipped)` (sichtbar, gleiche
  Testzahl); direkt je Teil: version 38/38, bausteine 18/18, trocken 32/32, ablauf 47/47, fehler_bau 25/25,
  fehler_umfeld 29/29, cmd `SKIP: release_alles.cmd braucht cmd.exe (Windows) - hier Linux` (rc 77).
* Release-Modul (die vorhandenen Tests, die release/ benutzen - der Diff fuegt dort nur Dateien und je eine Zeile in
  .gitattributes/.gitignore hinzu), woertlich:
  `ctest --test-dir re15_port/build -R "r35_android_pruefkette|r35_android_bash_mutanten" --timeout 1500 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 2` (pruefkette 167.52 s, bash_mutanten 634.80 s).

## 6. OFFEN

* Kein echter Gesamtlauf durch die Spur (Auftrag: den faehrt der Orchestrator fuer v0.8.26). Echt gemessen sind
  Trockenlauf (N1), Schritt alt gegen das echte Archiv (N2) und Docker-Start + Windows-Bau (N5); android/linux/paket
  laufen in den Tests als Attrappen, ihre Schnittstellen (Aufruf, Marke, Artefakt) sind mit Datei:Zeile belegt.
* Abweichung vom Ablauf der Runde 37, mit Absicht (Abschnitt 2.1): im Archiv wird die vorige Version NICHT mehr
  geloescht; Reihenfolge windows -> android -> linux statt windows -> linux -> android (kurze Baue zuerst).
* docker_bereit.sh: echt gelaufen ist der Weg `docker desktop start` (zweimal, 19 s / 24 s). Der Rueckfall fuer eine
  Docker-Desktop-Version ohne diese CLI (`Start-Process "Docker Desktop.exe"`) ist auf dieser Maschine nicht
  erreichbar und nur gelesen, nicht gelaufen.
* Fuer den Orchestrator beim Release v0.8.26 (Hauptbaum, master): erst `bash release/release_alles.sh --dry-run`;
  der Trockenlauf meldet dort die 33 unversionierten Eintraege unter synchro/ (darunter `synchro/STAGE2/room2001/`,
  kommt ins Paket, nicht in den Commit) als [!!]. Die volle Suite des Integrationsbaums enthaelt die 7 neuen Tests
  (Windows 7 gelaufen, Linux 6 + 1 SKIP) - RE15_MIN_TESTS entsprechend anheben.

## 7. Fuer den Nutzer

* **Bedienung**: Doppelklick auf `release\release_alles.cmd`. Es zeigt erst, was passieren wuerde (Version, Pruefungen,
  Schritte), dann die Wahl [1] bauen, packen, archivieren / [2] dazu Commit + Tag + Push / [3] nur Commit + Tag + Push
  / [4] nichts. Aus Git-Bash: `bash release/release_alles.sh` (`--help`, Anleitung `release/README.md`).
* **Versionsnummer**: automatisch die naechste (v0.8.25 -> v0.8.26, v0.8.99 -> v0.9.0); eine eigene mit `--version`.
* **Archiv**: die alte Version kommt ins Archiv (bzw. wird dort geprueft), die neue ebenfalls; geloescht wird im Archiv
  nichts. Das Archiv waechst damit um ~0,9 GB je Version (heute 9 Versionen v0.8.7-v0.8.14 + v0.8.25, zusammen 7456 MB; frei 97 GiB).
* **Entscheidungsfrage**: Bisher wurde bei jedem Release die vorige Version im Archiv geloescht (v0.8.15-v0.8.24 sind
  deshalb nicht mehr dort; v0.8.7-v0.8.14 und v0.8.25 schon). Soll das Archiv kuenftig doch aufgeraeumt werden (z.B. nur
  die letzten N Versionen behalten)? Bis zu Ihrer Antwort bleibt alles liegen.
* **Hinweis**: im Hauptbaum liegt noch `release/re15_port_v0.8.24_android.apk` (380 MB, gitignoriert); v0.8.24 ist
  nicht mehr im Archiv. Das Skript entfernt nur APKs, die bytegleich im Archiv liegen - diese bleibt liegen.
* Neue Assets fuer das Paket-Gate: keine. Sprachdateien: keine.

## 8. Nachbesserung 1 (nach Abnahme 0, `V_abnahme_0.md`)

Abnahme 0: bestanden = NEIN, einziger Mangel **M1** (V1 "einfache Moeglichkeit" teilweise): scheitert im Schritt
`abschluss` der Push, nachdem Release-Commit und annotierter Tag lokal schon stehen (release_alles.sh:683-688), fuehrt
die gemeldete Wiederaufnahme nicht weiter. V2/V3/V4 erfuellt - ihre Riegel bleiben gruen.

### 8.1 M1 - Ursache, Messung vorher, Beleg, Aenderung, Messung nachher

**Messung vorher** (22:49-22:59, Spurbaum 877f421a, Skript unveraendert = Stand 98652210; Riegel
`ablauf_push_scheitert` im Test `ablauf`, Rezept der Abnahme A9: Wegwerf-Repo wie `fx_bauen`, Tags v0.8.24/v0.8.25,
bare origin; ganzer Lauf; Notes `# v0.8.26` oben; origin per `set-url` auf einen nicht vorhandenen Pfad;
`--version v0.8.26 --ab abschluss --tag`; origin zurueck; die gemeldete Zeile WOERTLICH im Repo ausgefuehrt):
`bash test_r38_releaseskript.sh . <scratch> ablauf` -> rc 1, `58/70 Pruefungen gruen` (9 min 48 s; Fremdlast laut
Vorbedingungszeile `cmake 12, ctest 7, re15_pc 1`). Woertlich aus den Ausgaben:

| Lauf | Ergebnis (unveraendertes Skript) |
|---|---|
| p2 `--version v0.8.26 --ab abschluss --tag`, origin kaputt | rc 1; `Commit bc40d50, Tag v0.8.26`; `fatal: '.../gibtsnicht.git' does not appear to be a git repository`; `!!! ABBRUCH in Schritt 'abschluss': git push endete mit Rueckgabe 128`; `Nach der Behebung weiter mit: bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag` - kein Wort davon, dass Commit + Tag schon stehen, kein Push von Hand |
| p3 Trockenlauf danach (zeigt der Doppelklick zuerst) | `Version: v0.8.27  (hoechster Tag v0.8.26 + 1)`, `TROCKENLAUF-OK v0.8.27` - kein Hinweis, dass v0.8.26 nicht gepusht ist |
| p4 die gemeldete Zeile woertlich, origin repariert | **rc 2** `--version v0.8.26 ist nicht groesser als der hoechste Tag v0.8.26 - die APK waere ein Downgrade (build_android.sh:195-199)`; origin ohne Commit und ohne Tag |
| p5 `--ab abschluss --tag` (= Doppelklick-Wahl [3], release_alles.cmd:70) | rc 1, `Version: v0.8.27`, `[XX] --tag: release/RELEASE_NOTES.md beginnt nicht mit '# v0.8.27 ...' (oben steht: '# v0.8.26 - Testfassung')` |

12 neue Pruefungen rot (Meldung, Trockenlauf-Hinweis, gemeldete Zeile -> gepusht, Wahl [3] -> gepusht, Wiederholung),
die 58 alten gruen. M1 ist damit reproduziert.

**Ursache (Beleg Datei:Zeile, Stand 98652210):** Die Version wird VOR dem Plan festgelegt und kennt nur zwei Wege:
mit `--version` muss sie groesser als der hoechste Tag sein (release_alles.sh:198-201), ohne `--version` ist sie
hoechster Tag + 1 (:205). Nach Commit + Tag (:683-684) ist der hoechste Tag die Release-Version selbst - beide Wege
fuehren an ihr vorbei. `s_abschluss` legt Commit und Tag ausserdem immer neu an (:662-684; ein leerer Index endet in
`die "nichts vorgemerkt"` :673), ist also nicht wiederholbar. Die Abbruchmeldung (`die`, :152-154) setzt nur
`--version $NEU --ab $AKTUELL --tag` zusammen - ohne die Optionen des Aufrufs (`--archiv`!) und ohne den Push von Hand.

**Aenderung** (nur `release/` + Test; Bau-Umgebung, keine Original-Adresse - Belege Datei:Zeile wie Abschnitt 3):

| Stelle | vorher | jetzt |
|---|---|---|
| Version (release_alles.sh, Abschnitt `# --- Version`) | nur "--version > hoechster Tag" oder "hoechster Tag + 1" | zusaetzlich **Nachholen**: nur bei `--ab abschluss --tag` (= Doppelklick-Wahl [3], release_alles.cmd `--ab abschluss --tag`), wenn der hoechste Tag auf HEAD zeigt, HEAD dessen Release-Commit ist (Betreff `release: vX.Y.Z ...`, Form 2077bb8e/e8b3958f) und release/ genau dessen Volumes traegt -> diese Version; `--version` darf dann gleich dem Tag sein. Sonst unveraendert (Downgrade-Sperre build_android.sh:195-199 bleibt fuer jeden Bau) |
| Vorbedingung Tag | Tag da -> `[XX]` | Nachholen: `[ok] Tag vX.Y.Z steht auf HEAD = Release-Commit ... nur der Push wird nachgeholt`; Notes werden im Release-Commit geprueft (`git show HEAD:release/RELEASE_NOTES.md`), die Datei darf schon die naechste Version beginnen |
| Trockenlauf-Hinweis (neu) | - | `[!!] vX.Y.Z: Release-Commit ... und Tag stehen lokal, origin/master steht auf ... - der Push fehlt. Nur pushen: ... --ab abschluss --tag (Doppelklick: Wahl [3]) / oder von Hand: git push origin master vX.Y.Z` - lokal gemessen ueber `refs/remotes/origin/master` (git setzt es beim Push; `remote.origin.fetch = +refs/heads/*:refs/remotes/origin/*` im Repo) |
| `s_abschluss` | legt Commit + Tag immer neu an; leerer Index -> `die "nichts vorgemerkt"` | **wiederholbar**: ist HEAD schon der Release-Commit, wird er nicht neu angelegt (vorher Pruefung: Volumes/SUMS in release/ = HEAD, sonst Abbruch); steht der Tag schon, muss er auf HEAD zeigen und bleibt; dann `git push origin master vX.Y.Z` |
| Erfolgsmarke Push (neu) | Rueckgabe 0 von git push | zusaetzlich `git ls-remote origin refs/heads/master refs/tags/vX.Y.Z`: origin SELBST traegt master = HEAD und denselben Tag (Tag-Objekt); Zeile `origin bestaetigt (git ls-remote): ...` - gleiche Regel wie Marke + Artefakt der Bau-Schritte |
| Abbruchmeldung (`die`) | `Nach der Behebung weiter mit: ... --version V --ab S [--tag]` | dieselbe Zeile **mit den Optionen des Aufrufs** (`--archiv`, `--min-frei-gb`, `--linux-jobs`, `--kein-push`; `printf %q`) - ohne `--archiv` pruefte die Zeile ein anderes Archiv; beim Push zusaetzlich `Commit X und Tag V stehen lokal - nur der Push fehlt.` / `(pusht nur, Commit und Tag bleiben; Doppelklick: Wahl [3])` / `oder von Hand: git push origin master V` |
| `--kein-push` | `(nicht gepusht: --kein-push)` | dazu `Pushen spaeter: bash release/release_alles.sh --version V --ab abschluss --tag ... (Doppelklick: Wahl [3])` + Push von Hand |
| Teillauf-/Noch-offen-Zeile | ohne Optionen | mit den Optionen des Aufrufs (`weiter_zeile`) |
| release_alles.cmd | [3] "nur Release-Commit + Tag + Push" | dazu `Stehen Commit und Tag schon (der Push scheiterte), wird nur noch gepusht.` (CRLF, ASCII: 78/78 Zeilen CRLF) |
| README, Kopf/`--help` | - | Push-Abbruch + Nachholen beschrieben |

Nebenbei (Hinweise der Abnahme, keine Maengel): H3 Zitat `make_package.sh:929` -> `:931` (release_alles.sh, Kommentar in
`s_paket`; selbst nachgelesen: :931 `(kein Android-Paket: ... fehlt`); H2 Zeitgrenzen 60 s / 30 s in docker_bereit.sh
als PORT-WAHL gekennzeichnet (Kommentar, kein Verhalten geaendert). Das Zitat :929 stand ausserdem in Abschnitt 3 und im
Kommentar der Attrappe attrappe_gemeinsam.sh - beide berichtigt (nur Kommentar/Text).

Trockenlauf-Hinweis, zweite Stufe (nach den ersten Messungen ergaenzt): kommt nach dem nicht gepushten Release noch ein
Commit (die Orchestratoren committen laufend Doku), steht der Release-Commit nicht mehr auf HEAD - dann pusht Wahl [3]
nicht allein, der Tag bliebe still ungepusht. Der Hinweis prueft deshalb allgemein, ob der Commit des hoechsten Tags in
`refs/remotes/origin/master` enthalten ist (`git merge-base --is-ancestor`, Rueckgabe 1 = nicht enthalten; Fehler ->
kein Hinweis): auf HEAD -> `Nur pushen: ... --ab abschluss --tag (Doppelklick: Wahl [3])`, sonst
`... fehlt in origin/master ... von Hand: git push origin master vX.Y.Z`. Im Hauptbaum heute (gemessen):
`remote.origin.fetch = +refs/heads/*:refs/remotes/origin/*`, origin/master = 6210d98f, v0.8.25 (2077bb8e) ist darin
enthalten -> kein Hinweis (richtig).

### 8.2 Messung nachher

**Zwischenlauf** (23:06-23:14, Skript-Stand 88dda648 = ohne die zweite Hinweis-Stufe; Test als Kopie im Scratchpad,
`bash test_r38_releaseskript.sh . <scratch> ablauf`): rc 0, `== r38_releaseskript ablauf: 70/70 Pruefungen gruen ==`
(7 min 43 s unter Fremdlast). Dieselben Laeufe wie vorher, woertlich aus den Ausgaben:

| Lauf | vorher (98652210) | nachher (88dda648) |
|---|---|---|
| p2 Push scheitert | rc 1, nur `--version v0.8.26 --ab abschluss --tag` | rc 1; `Commit ce5867a und Tag v0.8.26 stehen lokal - nur der Push fehlt.` / `Nach der Behebung weiter mit: bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag --archiv <A> --min-frei-gb 1` / `(pusht nur, Commit und Tag bleiben; Doppelklick: Wahl [3])` / `oder von Hand: git push origin master v0.8.26` |
| p3 Trockenlauf danach | `TROCKENLAUF-OK v0.8.27`, kein Hinweis | `[!!] v0.8.26: Release-Commit ce5867a und Tag stehen lokal, origin/master steht auf f348fef - der Push fehlt.` + `Nur pushen: ... --ab abschluss --tag ... (Doppelklick: Wahl [3])` |
| p4 gemeldete Zeile WOERTLICH (im Repo, RE15_ARCHIV auf einen leeren Ordner) | **rc 2** "nicht groesser als der hoechste Tag v0.8.26 - ... Downgrade" | **rc 0**: `Version: v0.8.26  (Tag v0.8.26 steht schon auf HEAD = Release-Commit: ...)`, `Release-Commit steht schon: ce5867a ... - wird nicht neu angelegt`, `Tag v0.8.26 steht schon auf dem Release-Commit - wird nicht neu angelegt`, `origin bestaetigt (git ls-remote): master = ce5867a, Tag v0.8.26 = 0c72087e9481`, `RELEASE-ALLES-OK v0.8.26`; origin master = Release-Commit, origin-Tag = dasselbe Tag-Objekt, kein zweiter Commit, kein v0.8.27 |
| p5 `--ab abschluss --tag` ohne --version (= Wahl [3]) nach zurueckgesetztem origin | rc 1, v0.8.27, `[XX]` Notes | rc 0, v0.8.26, gepusht, von origin bestaetigt |
| p6 dasselbe, alles schon gepusht | rc 1 | rc 0, `Everything up-to-date`, `origin bestaetigt`, HEAD + Tags unveraendert |

**Endlauf** (Stand 10fe5afa + Dossier, Bau `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`,
`ninja: no work to do.` nach dem Neu-Configure der geaenderten Sonde), gezielt und seriell (Abnahme H6: unter Last kein
-j), woertlich:
`ctest --test-dir "<baum>/re15_port/build" -V -R "r38_releaseskript" --timeout 1200 --output-on-failure` ->
`100% tests passed, 0 tests failed out of 7`, `Total Test time (real) = 576.93 sec`; je Teil aus der -V-Ausgabe:
version 39/39 (38 s), bausteine 18/18 (4 s), trocken 32/32 (70 s), **ablauf 71/71** (110 s; vorher 47 - neu: 24
Pruefungen M1), fehler_bau 25/25 (131 s), **fehler_umfeld 30/30** (173 s; +1 `--kein-push`-Meldung), **cmd 21/21**
(49 s; +2 Doppelklick-Wahl 3 nach verlorenem Push). Testzahl unveraendert 7 (RE15_MIN_TESTS: weiter +7).

Echter Trockenlauf im Spurbaum mit dem neuen Skript (23:28, read-only): `bash release/release_alles.sh --dry-run` ->
rc 0 in 13 s, `Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`, `[ok] Tag v0.8.26 ist frei`, KEIN Push-Hinweis
(v0.8.25 liegt in origin/master - richtig), `TROCKENLAUF-OK v0.8.26`; `git status --porcelain --untracked-files=all
--ignored -- release` vorher = nachher.

Andere Release-Tests: `grep -rnE "release_alles|docker_bereit|release/README|release_alles\.cmd"` ueber
re15_port/tests (ausser r38_releaseskript), re15_port/CMakeLists.txt, re15_port/tools -> 0 Treffer; die Tests
r35_android_* lesen nur apk_pruefen.sh/python_finden.sh/apk_asset_gate.py/gate_urteil.py (Abnahme A11), die dieser
Diff nicht beruehrt - deshalb nicht erneut gefahren.

### 8.2a Tests (Nachbesserung 1, neue Pruefungen)

| Test | neue Pruefungen |
|---|---|
| unit_r38_releaseskript_ablauf | `ablauf_push_scheitert` (Rezept Abnahme A9): Push scheitert -> rc 1, Commit + annotierter Tag lokal, origin ohne Tag, Meldung "nur der Push fehlt" + Push von Hand + Wiederaufnahme-Zeile; Trockenlauf danach `[!!] ... der Push fehlt ... Wahl [3]`; nach einem weiteren Commit `... fehlt in origin/master ... von Hand`; die gemeldete Zeile WOERTLICH im Repo (RE15_ARCHIV auf einen leeren Ordner) -> rc 0, `RELEASE-ALLES-OK v0.8.26`, kein zweiter Commit, Tag-Objekt unveraendert, origin master = Release-Commit, origin-Tag = dasselbe Tag-Objekt, kein v0.8.27, `origin bestaetigt`; Wahl-[3]-Aufruf ohne --version -> v0.8.26, gepusht; Wiederholung bei fertigem Push -> rc 0, nichts veraendert; im ersten Lauf zusaetzlich "kein Push-Hinweis, wenn origin alles hat" |
| unit_r38_releaseskript_fehler_umfeld | `--kein-push`: Meldung nennt `Pushen spaeter: ... --ab abschluss --tag --archiv ...` (ohne --kein-push) und den Push von Hand |
| unit_r38_releaseskript_cmd | echter Doppelklick (cmd.exe, Wahl 3) nach verlorenem Push: rc 0, nichts gebaut, origin traegt master + Tag v0.8.26, kein neuer Commit, kein v0.8.27 |

Gegenprobe am unveraenderten Skript (8.1, "Messung vorher"): dieselben Riegel 12 rot (58/70).

### 8.3 OFFEN (Nachbesserung 1)

* Die neuen Pruefungen liefen nur unter Windows (Git-Bash 5.2.26). Eine Linux-Gegenprobe im Bau-Image war nicht
  moeglich: `timeout 20 docker info` -> rc 124 (Docker Desktop laeuft nicht; Abnahme A10: Start scheiterte am
  WSL-Bootstrap) - die Spur startet Docker nicht neben der Last der anderen Spuren. Die neuen Wege brauchen nur
  `git ls-remote`/`merge-base --is-ancestor`/`update-ref`/`tag -d` und `printf %q` (git 2.30.2 / bash 5.1 im Image
  `re15-linux-build:deb11` laut Dossier N6); der Release-Linux-Lauf faehrt die 7 Tests (cmd: SKIP) ohnehin.
* Der Push-Weg ist nur gegen ein lokales bare origin gemessen (Test), nicht gegen GitHub: ein echter Push gehoert zum
  Release-Lauf des Orchestrators. `git ls-remote origin` nutzt dieselbe Anmeldung wie `git push`.
* Hinweis H1 der Abnahme selbst nachgemessen (2026-10-16 ~23:30, PowerShell `Test-Path`): `C:\workspace\Re15Data` und
  `C:\workspace\Re15Data\re15_packages_archiv` fehlen weiterhin (`C:\workspace` enthaelt nur `Gemeinde`, `git`,
  `keepass`). Ein Release-Lauf legt das Archiv am Standardpfad neu an und bringt v0.8.25 aus release/ hinein (Abnahme
  A2); v0.8.7-v0.8.14 kann das Skript nicht wiederherstellen. VOR dem Release v0.8.26 den Nutzer fragen bzw.
  `--archiv`/`RE15_ARCHIV` auf den neuen Ort setzen.
* H5 der Abnahme (Kosmetik) unveraendert: im zweistufigen Ablauf ([1], spaeter [3]) nennt die Commit-Nachricht die
  Lauf-Id des Abschluss-Laufs.

### 8.4 Fuer den Nutzer (Nachbesserung 1)

* **Wenn beim Veroeffentlichen der Push scheitert** (Netz, Anmeldung): Release-Commit und Tag stehen dann schon auf dem
  Rechner, die Meldung sagt das ("... stehen lokal - nur der Push fehlt."). Nach der Behebung einfach noch einmal
  Doppelklick und Wahl [3] - oder die Zeile aus der Meldung. Es wird nur noch gepusht: kein zweiter Commit, keine neue
  Versionsnummer. Von Hand geht auch `git push origin master vX.Y.Z`. Solange der Push fehlt, zeigt jeder Trockenlauf
  (also auch der Doppelklick) `[!!] ... der Push fehlt`.
