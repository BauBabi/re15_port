# Runde 38 Spur V — releaseskript (Dossier, laufend)

Baum `.claude/worktrees/r38_releaseskript` (Zweig `r38/releaseskript`, Basis e2cc7673). Angelegt 2026-10-16 16:28 (Uhr).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
> Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

Lesart des Orchestrators (AUFTRAG.md, "Lesarten"): "minor" = wie alle bisherigen Releases die LETZTE Stelle hochzaehlen
(0.8.25 -> 0.8.26, 0.8.99 -> 0.9.0 wie v0.7.99 -> v0.8.0); das Skript nimmt optional eine explizite Version.

## Stand

(laufend, neueste Eintraege oben)

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
make_package.sh:980-981 (2026-08-22) "alte Packages vom Repo loeschen" und Memory reai-v2-immer-paketieren (2026-08-24)
"auch im archiv, auch pushen und das letzte davor entfernen" - beide sprechen vom REPO (Pakete im Repo), nicht vom
Archiv; das Loeschen im Archiv ist eine spaetere Auslegung der Agenten. Umsetzung daher:
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
| alle DREI Plattformen muessen nach `paket` als Satz da sein | make_package.sh:929 liefert ohne APK still nur PC-Saetze |
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

Ergebnisse: siehe Abschnitt 4 (N6, Linux-Container) und die ctest-Summenzeile unten.

## 6. OFFEN

* Kein echter Gesamtlauf durch die Spur (Auftrag: den faehrt der Orchestrator fuer v0.8.26). Echt gemessen sind
  Trockenlauf (N1), Schritt alt gegen das echte Archiv (N2) und Docker-Start + Windows-Bau (N5); android/linux/paket
  laufen in den Tests als Attrappen, ihre Schnittstellen (Aufruf, Marke, Artefakt) sind mit Datei:Zeile belegt.
* Abweichung vom Ablauf der Runde 37, mit Absicht (Abschnitt 2.1): im Archiv wird die vorige Version NICHT mehr
  geloescht; Reihenfolge windows -> android -> linux statt windows -> linux -> android (kurze Baue zuerst).

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
