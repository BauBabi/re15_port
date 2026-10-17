# Runde 38 Spur V — releaseskript: UNABHAENGIGE ABNAHME 1

Baum `.claude/worktrees/r38_releaseskript` (Zweig `r38/releaseskript`, HEAD 99925242, Basis e2cc7673).
Pruefer: Abnahme-Agent 1, 2026-10-16 (ab ~23:30). Gegenstand: Nachbesserung 1 (Mangel M1 der Abnahme 0,
`V_abnahme_0.md`) und Regressionen. Alles hier ist SELBST gemessen; Behauptungen des Bau-Agenten gelten erst nach
eigener Messung.

## Stand (laufend, neueste oben)

* 2026-10-17 04:56 FORTSETZUNG nach dem Sitzungslimit (gleicher Pruefer-Auftrag, neue Sitzung): Bau erneut
  `LOCAL-BUILD-OK (configure)` + `LOCAL-BUILD-OK (build)` (`ninja: no work to do.`), Maschine jetzt OHNE Fremdlast
  (keine ctest/cmake/ninja/re15_pc-Prozesse); serieller ctest-Lauf B3 gestartet. Der ctest-Lauf der ersten Sitzung
  (23:33-23:45) ist NICHT verwertbar: `fehler_umfeld` endete nach 7 gruenen Pruefungen ohne eine `FEHL`-Zeile mit
  `***Failed 119.89 sec` (23:44:05), `cmd` startete und ctest endete mit `CTEST_RC=1` - beides in der Minute des
  Sitzungsabbruchs, ohne Zeitlimit und ohne Pruef-Ausgabe (Ursache dort nicht mehr feststellbar); deshalb hier
  neu und vollstaendig gemessen (B3).
* 23:40 B1 Bau gruen, B2 echter Trockenlauf gruen, eigene Gegenprobe G-S1 (Rezept A9) gruen; weitere laufen.
* 23:35 Lektuere AUFTRAG/VERTRAG/Dossier/Abnahme 0, Diff 190a26a0..99925242 (Nachbesserung) gelesen; Bau gestartet.

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
> Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

Teilpunkte wie Abnahme 0: V1 "einfache Moeglichkeit - ueber Skript oder sowas", V2 "die Ports fuer alle Systeme zu
bauen", V3 "neue Versionsnummer - minor", V4 "die alte ins Archiv zu bringen". Massstab Spur V (VERTRAG.md §2):
"Trockenlauf + Versionslogik-Tests; den echten Release faehrt der Orchestrator mit dem Skript."

## 1. Diff und Vertrag

`git diff master...HEAD --name-only` (Basis e2cc7673; der Zwei-Punkt-Diff `git diff master` zeigt zusaetzlich
geloeschte Orchestrator-Dateien - das sind nur Commits, die master nach der Basis bekam): 18 Dateien, nur `release/`
(README.md, .gitattributes, .gitignore, docker_bereit.sh, release_alles.cmd, release_alles.sh, release_alles_lib.sh),
`re15_port/tests/unit/probes/r38_releaseskript.cmake`, `re15_port/tests/unit/r38_releaseskript/**`, Dossier und
Abnahme 0. Nachbesserung 1 (`git diff 190a26a0 HEAD --stat`): release_alles.sh (+222/-), README, .cmd (+2 Zeilen),
docker_bereit.sh (+3 Kommentarzeilen), Test (+92), Sonde (Kommentar), Attrappe (Kommentar), Dossier.
Code-Diff der Nachbesserung vollstaendig gelesen, dazu release_alles.sh ganz (861 Zeilen) und die Attrappen.
* TABU unberuehrt: `git diff master...HEAD --name-only -- release/RELEASE_NOTES.md re15_port/platform/android
  re15_port/shared_assets` = 0. `release/` gehoert laut VERTRAG §1.4 ausschliesslich Spur V.
* Keine CMakeLists geaendert (Sonde ueber GLOB tests/unit/CMakeLists.txt:4060). Keine Bank-9-Bits, Nachrichten-Ids,
  Slots, Ereignisse (kein Spiel-Code).
* Eingaben anderer Release-Tests unberuehrt: `git diff master...HEAD --name-only -- release/apk_pruefen.sh
  release/python_finden.sh release/apk_asset_gate.py release/gate_urteil.py release/make_package.sh
  release/build_android.sh release/build_win_cross.sh release/build_linux_deck.sh release/docker_win_build.sh
  release/docker_linux_build.sh re15_port/tests/unit/r35_android re15_port/tests/unit/CMakeLists.txt
  re15_port/CMakeLists.txt` = 0 Dateien; `grep -rlE "release_alles|docker_bereit" re15_port/tests re15_port/tools
  re15_port/CMakeLists.txt` ausser r38_releaseskript = keine.

## 2. Messungen

### B1 Bau (23:33)
`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
to do.`, `=== LOCAL-BUILD-OK (build)`. `ctest -N`: `Total Tests: 795`, darunter #768-#774
`unit_r38_releaseskript_{version,bausteine,trocken,ablauf,fehler_bau,fehler_umfeld,cmd}` (Testzahl wie Abnahme 0).

### B2 Echter Trockenlauf im Spurbaum (23:37, HEAD 99925242)
`bash release/release_alles.sh --dry-run` -> `RC=0` in 37 s: `Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`,
`[ok] Tag v0.8.26 ist frei`, KEINE Zeile "der Push fehlt" (richtig: v0.8.25 = 2077bb8e ist in origin/master -
eigene Gegenprobe `git merge-base --is-ancestor "v0.8.25^{commit}" refs/remotes/origin/master` -> rc 0,
origin/master = 6210d98f, `remote.origin.fetch = +refs/heads/*:refs/remotes/origin/*`), Docker `[--] ... startet
Docker Desktop`, `[ok] Vorversion v0.8.25 fehlt im Archiv - Schritt 'alt' legt .../v0.8.25 an` (H1 der Abnahme 0
besteht weiter: `/c/workspace/Re15Data` fehlt, vorher und nachher `No such file or directory`), sechs Schritte,
`TROCKENLAUF-OK v0.8.26 (1 Hinweis(e) [!!])` (der Hinweis = Fremdlast `cmake 5, ctest 6, re15_pc 1`).
Schreibt nichts: `git status --porcelain --untracked-files=all --ignored -- release` vorher = nachher (sha256
bdbb0f8a..., 9 Zeilen), das Archiv wurde nicht angelegt.

### B3 Tests der Spur, seriell (2026-10-17 04:56:37-05:01:21; beim Start keine Fremdprozesse, ab 04:56:55 lief ein ctest der Spur tagebuch mit)

`ctest --test-dir "<baum>/re15_port/build" -V -R "r38_releaseskript" --timeout 1200 --output-on-failure`
(Ausgabe `<scratchpad>/abn1v_ctest.txt`), woertlich:
```
1/7 Test #768: unit_r38_releaseskript_version .........   Passed   19.65 sec
2/7 Test #769: unit_r38_releaseskript_bausteine .......   Passed    2.65 sec
3/7 Test #770: unit_r38_releaseskript_trocken .........   Passed   22.72 sec
4/7 Test #771: unit_r38_releaseskript_ablauf ..........   Passed   46.33 sec
5/7 Test #772: unit_r38_releaseskript_fehler_bau ......   Passed   57.29 sec
6/7 Test #773: unit_r38_releaseskript_fehler_umfeld ...   Passed   99.45 sec
7/7 Test #774: unit_r38_releaseskript_cmd .............   Passed   35.58 sec
100% tests passed, 0 tests failed out of 7
Total Test time (real) = 283.70 sec
```
Pruefzahlen aus der -V-Ausgabe: version 39/39, bausteine 18/18, trocken 32/32, **ablauf 71/71**, fehler_bau 25/25,
**fehler_umfeld 30/30**, **cmd 21/21** - genau die Zahlen des Bau-Agenten; 236 `ok`-Zeilen, 0 `FEHL`. Darunter die
neuen M1-Riegel (`M1 gemeldete Zeile: rc 0 (ist 0)`, `M1 Wahl [3] ohne --version: rc 0 (ist 0)`, `--kein-push: Meldung
nennt den spaeteren Push ...`, `Doppelklick 3 nach verlorenem Push: rc 0 (ist 0), nichts gebaut`, `... origin hat master
+ Tag v0.8.26, kein neuer Commit, kein v0.8.27`). Ohne Last 284 s statt 577 s (Bau-Agent) bzw. 811 s (Abnahme 0, A11).

### B4 Echter Trockenlauf im Spurbaum, zweite Sitzung (05:02, HEAD 18c29e6d = Code 99925242 + Teilbericht)

`bash release/release_alles.sh --dry-run` -> `RC=0 DAUER=9s` (docker info kehrte jetzt sofort mit rc 1 zurueck):
`Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`, `[ok] Tag v0.8.26 ist frei`, KEINE Zeile "der Push fehlt",
`[ok] Vorversion v0.8.25 fehlt im Archiv - Schritt 'alt' legt C:/workspace/Re15Data/re15_packages_archiv/v0.8.25 an`,
sechs Schritte, `TROCKENLAUF-OK v0.8.26 (1 Hinweis(e) [!!])` (der Hinweis = Fremdlast `cmake 1, ctest 1, re15_pc 1`
anderer Abnahmen). Schreibt nichts: `git status --porcelain --untracked-files=all --ignored -- release` vorher =
nachher (sha256 30bb3210..., 11 Zeilen; gegenueber B2 zwei Zeilen mehr = das gitignorierte Protokoll
`release/logs/v0.8.26_20261016-234330/` des Teillaufs B5). Gegenprobe Hauptbaum: `remote.origin.fetch =
+refs/heads/*:refs/remotes/origin/*`, origin/master = 6210d98f, `git merge-base --is-ancestor "v0.8.25^{commit}"
refs/remotes/origin/master` -> rc 0 (also richtig kein Hinweis).

### B5 V4 mit dem Stand der Nachbesserung: Schritt alt mit ECHTEN Daten in ein Wegwerf-Archiv

Erste Sitzung 23:43:30 (Ausgabe `<scratchpad>/abn1_alt_echt.txt`): `bash release/release_alles.sh --bis alt --archiv
<scratchpad>/archiv_real` (Archiv vorher leer) -> rc 0 in 51 s, `release/ v0.8.25: 6 Volumes, sha256sum -c 6/6 OK`,
`Vorversion v0.8.25 ins Archiv gebracht (...): SHA256SUMS.txt 6/6 OK`, und die neue Weiter-Zeile traegt die Option des
Aufrufs: `Teillauf bis 'alt' fertig. Weiter mit: bash release/release_alles.sh --version v0.8.26 --ab windows --archiv
<scratchpad>/archiv_real`, `RELEASE-ALLES-TEILLAUF-OK v0.8.26 (alt)`. Zweite Sitzung (05:03) unabhaengig nachgeprueft:
`sha256sum -c SHA256SUMS.txt` im Archivordner 6x `OK` (rc 0); `git show v0.8.25:release/SHA256SUMS.txt` (ohne CR) =
Archiv-SUMS (0 CR); `cmp` aller 6 Volumes gegen release/: 6x gleich.

### B6 Linux-Gegenprobe im Release-Bau-Image (05:03-05:04) - schliesst die offene Linux-Angabe

Docker Desktop war gestoppt (`docker-desktop Stopped`); `docker desktop start --timeout 240` -> `start_rc=0 nach 12 s`,
`docker info` -> `27.5.1`. Lauf wie im Release-Container (`<scratchpad>/linux_im_container.sh` aus Abnahme 0:
`/src/release/*` als Rueckfall-Links auf den read-only gemounteten Spurbaum, Registrierung ueber die ECHTE Sonde
`probes/r38_releaseskript.cmake`): `docker run --rm -v <baum>:/host:ro -v <scratchpad>:/abn:ro re15-linux-build:deb11
bash /abn/linux_im_container.sh` -> `docker_run_rc=0`; Umgebung `git version 2.30.2, cmake version 3.28.6, zip: fehlt,
unzip: fehlt, ps: fehlt`; woertlich:
```
1/7 Test #1: unit_r38_releaseskript_version .........   Passed    0.47 sec
2/7 Test #2: unit_r38_releaseskript_bausteine .......   Passed    0.08 sec
3/7 Test #3: unit_r38_releaseskript_trocken .........   Passed    0.69 sec
4/7 Test #4: unit_r38_releaseskript_ablauf ..........   Passed    1.57 sec
5/7 Test #5: unit_r38_releaseskript_fehler_bau ......   Passed    2.07 sec
6/7 Test #6: unit_r38_releaseskript_fehler_umfeld ...   Passed    3.30 sec
7/7 Test #7: unit_r38_releaseskript_cmd .............***Skipped   0.01 sec
100% tests passed, 0 tests failed out of 7
== r38_releaseskript version: 38/38 Pruefungen gruen ==   (Pruefung gegen die echten Repo-Tags entfaellt sichtbar)
== r38_releaseskript ablauf: 71/71 Pruefungen gruen ==
== r38_releaseskript fehler_umfeld: 30/30 Pruefungen gruen ==
SKIP: release_alles.cmd braucht cmd.exe (Windows) - hier Linux
```
(bausteine 18/18, trocken 32/32, fehler_bau 25/25.) Die neuen Wege (`ls-remote`, `merge-base --is-ancestor`,
`printf %q`, Riegel mit `update-ref`/`tag -d`) laufen also auch unter git 2.30.2 / Debian 11 gruen; die Testzahl ist
dort 7 (6 + 1 sichtbar SKIP) wie unter Windows. Danach `docker ps` leer, `docker desktop stop`, `docker info` rc 1,
`docker-desktop Stopped`, kein Docker-Prozess mehr (= Ausgangszustand).

### G Eigene Gegenproben zu M1 (eigenes Skript, nicht die Test-Fixture der Spur)

Skript `<scratchpad>/abn1_pruef.sh` (nicht im Repo): eigenes Wegwerf-Repo je Szenario mit dem ECHTEN
`release_alles.sh` + `release_alles_lib.sh` + `release_alles.cmd` aus dem Spurbaum, nur die Schritt-Skripte als
Attrappen der Spur; Tags v0.8.24 (annotiert) + v0.8.25 (leichtgewichtig, wie im echten Repo), bare origin.
Abweichend von der Spur-Fixture: `core.autocrlf` NICHT ueberschrieben (System-gitconfig `true` wie im echten Repo,
`git config --show-origin` -> `C:/Program Files/Git/etc/gitconfig true`) und das Archiv per Umgebung `RE15_ARCHIV`
statt `--archiv` (so wie ein Lauf gegen das Standard-Archiv: keine Option, die die Zeile mitnehmen muesste).

**G-S1 Rezept A9 der Abnahme 0, Wiederaufnahme-Zeile WOERTLICH (23:36-23:38):** ganzer Lauf ohne `--tag` rc 0;
Notes `# v0.8.26` oben; `git remote set-url origin <nicht vorhanden>`; `--version v0.8.26 --ab abschluss --tag`
-> rc 1, lokal Release-Commit (`release: v0.8.26 ...`) + annotierter Tag auf HEAD, origin ohne Tag. Meldung woertlich:
```
!!! ABBRUCH in Schritt 'abschluss': git push endete mit Rueckgabe 128
    Commit cebebcf und Tag v0.8.26 stehen lokal - nur der Push fehlt.
    Nach der Behebung weiter mit: bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag
    (pusht nur, Commit und Tag bleiben; Doppelklick: Wahl [3])
    oder von Hand: git push origin master v0.8.26
RELEASE-ALLES-ABBRUCH (abschluss)
```
Trockenlauf danach rc 0: `[!!] v0.8.26: Release-Commit cebebcf und Tag stehen lokal, origin/master steht auf
1269019 - der Push fehlt.` / `Nur pushen: bash release/release_alles.sh --ab abschluss --tag (Doppelklick: Wahl [3])`
/ `oder von Hand: git push origin master v0.8.26`. origin repariert, die gemeldete Zeile per `eval` WOERTLICH im Repo:
**rc 0** (Abnahme 0: rc 2 "Downgrade"), `Version: v0.8.26  (Tag v0.8.26 steht schon auf HEAD = Release-Commit: ...)`,
`Release-Commit steht schon: cebebcf ... - wird nicht neu angelegt`, `Tag v0.8.26 steht schon auf dem Release-Commit -
wird nicht neu angelegt`, `origin bestaetigt (git ls-remote): master = cebebcf, Tag v0.8.26 = ce4a3f9f0513`,
`RELEASE-ALLES-OK v0.8.26`. Gegen origin selbst geprueft (`git -C origin.git rev-parse`): master = Release-Commit,
Tag = dasselbe Tag-Objekt; HEAD unveraendert (kein zweiter Commit), kein v0.8.27, kein Bau-Aufruf. Trockenlauf danach:
kein Push-Hinweis, `Version: v0.8.27`.

**G-S1b Aufruf mit `--archiv` (Zeile muss es tragen):** Abbruch-Zeile woertlich `bash release/release_alles.sh
--version v0.8.26 --ab abschluss --tag --archiv /tmp/claude/.../s1b/archiv --min-frei-gb 1`; mit `RE15_ARCHIV` auf
einen LEEREN Ordner ausgefuehrt -> rc 0, origin master = Release-Commit, Tag gepusht.

**G-S2 Wahl [2] (`--tag` ab Beginn) mit kaputtem Push, dann ECHTER Doppelklick Wahl 3 (23:38-23:39):**
`release_alles.sh --tag` (Notes vorher) -> rc 1 im Schritt abschluss, dieselbe Meldung (`Commit 25e8e20 und Tag v0.8.26
stehen lokal - nur der Push fehlt.` ...). origin repariert, `printf '3\n' | cmd.exe /c <repo>\release\release_alles.cmd`
(ohne Argumente = Doppelklick, `RE15_ARCHIV` = Archiv des Laufs): **rc 0**; der Trockenlauf des Doppelklicks zeigt
`Version: v0.8.27` und darunter `[!!] v0.8.26: Release-Commit 25e8e20 und Tag stehen lokal ... - der Push fehlt.` /
`Nur pushen: ... (Doppelklick: Wahl [3])`; nach `Auswahl [1-4]: 3`: `Version: v0.8.26  (Tag v0.8.26 steht schon auf
HEAD ...)`, `origin bestaetigt (git ls-remote): master = 25e8e20, Tag v0.8.26 = 9f89208ab1f7`, `RELEASE-ALLES-OK
v0.8.26`. Attrappen-Protokoll ohne Bau-Aufruf (nichts gebaut), origin master + Tag-Objekt = lokal, kein v0.8.27.

**G-S3 Teil-Push - origin nimmt master, lehnt den Tag ab (`update`-Hook im bare origin: `refs/tags/*` -> exit 1;
`git push` ist ohne `--atomic` nicht atomar) (23:39-23:41):** `--version v0.8.26 --ab abschluss --tag` -> rc 1,
`! [remote rejected] v0.8.26 -> v0.8.26 (hook declined)`, `!!! ABBRUCH in Schritt 'abschluss': git push endete mit
Rueckgabe 1`, `Commit d937d8c und Tag v0.8.26 stehen lokal - nur der Push fehlt.`, Wiederaufnahme-Zeile, `oder von Hand:
git push origin master v0.8.26`; origin master stand danach schon auf dem Release-Commit (d937d8c8), origin ohne Tag.
Trockenlauf danach: rc 0, `Version: v0.8.27`, **kein** `[!!] ... der Push fehlt` (der Hinweis prueft nur, ob der
Tag-Commit in `refs/remotes/origin/master` liegt - hier liegt er dort, nur der Tag fehlt) -> Hinweis H-a. Hook entfernt,
`--ab abschluss --tag` (= Wahl [3]) -> **rc 0**, origin Tag = dasselbe Tag-Objekt, origin master = Release-Commit.
Die Wiederaufnahme selbst fuehrt also auch hier zum Ziel.

**G-S4 Push scheitert, danach ein weiterer Commit (Doku, wie die Orchestratoren laufend committen):** Trockenlauf rc 0,
`[!!] v0.8.26: der Commit des Tags (67b9701) fehlt in origin/master (9096b18) - der Push fehlt.` / `von Hand: git push
origin master v0.8.26`. Wahl [3] (`--ab abschluss --tag`) danach: rc 1 in den Vorbedingungen (`Version: v0.8.27`,
`[XX] --tag: release/RELEASE_NOTES.md beginnt nicht mit '# v0.8.27 ...'`) - bricht ab, pusht nichts Falsches, legt keinen
v0.8.27-Tag an. Die Zeile des Hinweises (`git push origin master v0.8.26`) -> origin traegt den Tag; Trockenlauf danach
ohne Hinweis. (Der Text der Wahl [3] im Doppelklick "Stehen Commit und Tag schon (der Push scheiterte), wird nur noch
gepusht" gilt in diesem Fall nicht; der unmittelbar davor gezeigte Trockenlauf nennt aber den richtigen Weg -> H-b.)

**G-S5 Release-Commit steht, Tag fehlt** (`--kein-push`, danach `git tag -d v0.8.26` - wie eine gescheiterte
Tag-Anlage): Wahl [3] (`--ab abschluss --tag`) -> rc 0, `Version: v0.8.26  (hoechster Tag v0.8.25 + 1)`,
`Release-Commit steht schon: 56328ac release: v0.8.26 ... - wird nicht neu angelegt`, Tag neu (annotiert) auf HEAD,
`origin bestaetigt (git ls-remote): master = 56328ac, Tag v0.8.26 = 18a3ee3fa66f`, kein zweiter Commit.

**G-S6 `--kein-push`, danach die gemeldete Zeile "Pushen spaeter" WOERTLICH:** Zeile `bash release/release_alles.sh
--version v0.8.26 --ab abschluss --tag` (ohne `--kein-push`) -> rc 0, origin master = Release-Commit, origin-Tag =
dasselbe Tag-Objekt.

**G-S8 Regression Normalfall** (ganzer Lauf `--tag`, origin in Ordnung): rc 0, Bau-Reihenfolge `windows android linux
paket`, Archiv v0.8.25 + v0.8.26, Release-Commit nur Paketdateien + Notes, `origin bestaetigt (git ls-remote): master =
98e111e, Tag v0.8.26 = 68c91ff218e1`, `RELEASE-ALLES-OK v0.8.26`.

Summe der eigenen Gegenproben der ersten Sitzung (`abn1_pruef.sh`, Ausgabe `abn1_pruef_out.txt`, 23:36-23:44):
`== Abnahme 1 Gegenproben: 52/52 gruen ==`.

Zweite Sitzung, zwei weitere Faelle (`<scratchpad>/abn1v_pruef2.sh`, gleiche Fixture wie oben, Ausgabe
`abn1v_pruef2_out.txt`, 05:01:14-05:01:57, `10/10` Pruefungen wie erwartet):

**G-S4b Push scheitert, danach ein weiterer Commit, dann die GEMELDETE Zeile woertlich:** Abbruchmeldung wie G-S1
(`... weiter mit: bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag` + `oder von Hand: git push origin
master v0.8.26`); origin repariert; `doc: nach dem Release` committet; die gemeldete Zeile -> **rc 2**:
`--version v0.8.26 ist nicht groesser als der hoechste Tag v0.8.26 - die APK waere ein Downgrade
(build_android.sh:195-199)` - derselbe Wortlaut wie M1 in Abnahme 0, nur jetzt erst nach einem Zwischen-Commit
(HEAD ist nicht mehr der Release-Commit -> kein Nachholen). Nichts veraendert (HEAD, origin master, kein Tag v0.8.27,
origin ohne Tag v0.8.26). Die zweite Zeile derselben Meldung (`git push origin master v0.8.26`) -> origin master = HEAD,
Tag = dasselbe Tag-Objekt. Zusammen mit G-S4: in diesem Fall fuehren Zeile 1 (rc 2) und Wahl [3] (rc 1, `[XX]` Notes
v0.8.27) NICHT zum Ziel, die Zeile "von Hand" und der Trockenlauf-Hinweis schon -> Hinweis H-b.

**G-S7 Push von v0.8.26 scheitert, der Hinweis wird uebergangen, v0.8.27 ganz mit `--tag` gebaut:** Trockenlauf vor
dem Lauf: `[!!] v0.8.26: Release-Commit cd9cd8c und Tag stehen lokal, origin/master steht auf debddb8 - der Push
fehlt.`; der ganze Lauf v0.8.27 `--tag` -> rc 0, `origin bestaetigt (git ls-remote): master = 5b67a80, Tag v0.8.27 =
3f6e01fe6d63`, `RELEASE-ALLES-OK v0.8.27`. Danach: origin master 5b67a80 (enthaelt den v0.8.26-Release-Commit),
**origin ohne Tag v0.8.26**, der naechste Trockenlauf (`Version: v0.8.28`) zeigt KEINEN Hinweis mehr. Der
Release-Commit ist also gesichert, nur der Tag v0.8.26 fehlt auf origin dauerhaft und still -> Hinweis H-a (wie G-S3).

### G-R Messen die neuen Riegel? Gegenprobe am alten Skript (23:40-23:42)

Neuer Test (`test_r38_releaseskript.sh` von HEAD, Kopie im Scratchpad mit den Attrappen) gegen ein Repo-Abbild, dessen
`release/release_alles.sh` + `_lib` + `.cmd` aus 190a26a0 stammen (= Stand der Abnahme 0; `git diff 190a26a0 98652210
-- release/` leer; `grep -c NACHHOLEN` = 0): `bash test_neu.sh <alt_repo> <wd> ablauf` -> **RC=1, `58/71 Pruefungen
gruen`**, rot genau die M1-Riegel: `M1 Meldung: nur der Push fehlt`, `M1 Meldung: Push von Hand`, `M1 Trockenlauf
danach: [!!] der Push fehlt, Wahl [3]`, `M1 Trockenlauf nach weiterem Commit: ...`, **`M1 gemeldete Zeile: rc 0 (ist
2)`** (= A9: "Downgrade"), `... RELEASE-ALLES-OK v0.8.26`, `... origin master = Release-Commit`, `... origin traegt Tag
v0.8.26`, `... Push von origin bestaetigt`, `M1 Wahl [3] ohne --version: rc 0 (ist 1)`, `M1 Wahl [3]: Version v0.8.26`,
`M1 Wahl [3]: origin master + Tag ...`, `M1 Wiederholung ...: rc 0 (ist 1)` (13 rot; das Dossier nennt 12 von 70 - die
13. ist der spaeter in 10fe5afa ergaenzte p3b-Riegel). Am neuen Skript dieselben 71/71 gruen (ctest unten). Die Riegel
messen also den Mangel und nicht nur sich selbst.
