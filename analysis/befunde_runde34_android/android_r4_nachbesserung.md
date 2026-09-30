# Runde 34 Android - Stufe 4 Nachbesserung (Runde 1)

Angelegt 2026-09-30 10:40. Auftrag: Gegenpruefer-Befunde pruefer_*_r4_1.md beheben (H1 hoch; H5, H3, H2 mittel; niedrige nach Moeglichkeit).


## 0. Stand / Plan

Gegenstand: Gegenpruefer-Befunde `pruefer_umgehung_r4_1.md` (H1 hoch; H5, H3, H2 mittel; V1/V2, U1-U4 niedrig) und
`pruefer_echtlauf_r4_1.md` (E1 niedrig = U2). Arbeitsordner (nicht versioniert): `build/r34a/nb/`. Ausgangsstand HEAD
`79ebd085` (+ Dossier-Commit `e777d796`). Python nur `/c/Python310/python` bzw. `release/python_finden.sh`.

- [x] 1 Befunde selbst nachmessen (H1, H2, H5 schnell; H3 per make_package in eigener Sandbox)
- [x] 2 Bauplan
- [x] 3 H1 + H2: Gate-Urteil aus Rueckgabe UND Ausgabe, Gate-Pin, keine Umgebungsvariable mehr
- [x] 4 H3: SHA256SUMS.txt + git add aus einer Positivliste, fremde Versionsdateien -> Abbruch
- [x] 5 H5 (+U3, V1/V2): Pfade nur druckbares ASCII, Segment <= 251 B - Gradle, Gate, Geraete-Leser, Tests
- [x] 6 niedrig: U1, U2/E1 (Waisen), U4 (fail closed) im Entpacker
- [ ] 7 Nachweise: Selbsttest, Mutanten/Kette gegen die neuen Skripte, PC-Suite, Android-Bau, Emulator
- [ ] 8 Endstand

## 1. Befunde selbst nachgemessen (Stand HEAD, vor jeder Aenderung)

Werkzeug `build/r34a/nb/nb_repro.sh` (Mutanten mit dem Pruefer-Werkzeug `u1_mutanten.py`, Sandbox mit
`r4_sandbox_anlegen.sh` unter `build/r34a/nb/sb`; release/ und der Index des Arbeitsbaums unberuehrt). Logs
`build/r34a/nb/logs/repro/`, Zusammenfassung `build/r34a/nb/logs/repro_teil1.txt`.

| Befund | Messung | Ergebnis |
|---|---|---|
| H1 | Selbsttest G0 (0-Byte-Gate) | **EXIT 0**, 0 B Ausgabe |
| H1 | Selbsttest G1 (`sys.exit(main())` -> `main()`) | **EXIT 0**, letzte Zeile `== SELBSTTEST-FEHLER: 234 von 248 Faellen falsch` |
| H1 | Sandbox `build_android.sh --gate-only` Referenz-APK v0.8.19 (Liste v1) mit G0 als Gate | **EXIT 0**, `APK-PRUEFUNG-OK`, `ANDROID-GATES-OK` (6 s) |
| H2 | dasselbe mit ECHTEM Gate in release/, Umgebung `APK_GATE_DATEI=<G0>` | **EXIT 0**, `ANDROID-GATES-OK` |
| H5 | Gate `manifest_lesen` auf Listen mit je einem Paar Kelvin/K, NFC/NFD, ss/sz, Ae/ae | alle vier **angenommen** (0 Fehler) |

Code bestaetigt die Ursache: make_package.sh wertet nur `rc_selbst`/`rc_quelle`/`rc` aus (`(( rc_selbst == 0 ))`,
`case "$rc" in 0)`), apk_pruefen.sh nur `(( rc == 0 ))` bzw. `case`; `APK_GATE_DATEI` wird in apk_pruefen.sh Schritt 5 per
`${APK_GATE_DATEI:-...}` aus der Umgebung genommen. H3: siehe 1.1.

### 1.1 H3 in der Sandbox (make_package.sh echt, Stand HEAD)

`nb_repro.sh B` (Log `build/r34a/nb/logs/repro_teil2.txt`): Linux-Binary v0.8.19 aus dem Archiv (Split-Satz
`sha256sum -c` OK, kein touch), `--version v0.8.19 --only linux`, erst `--no-zip`, dann in release/ der Sandbox
`re15_port_v0.8.19_ANDROID.zip` und `re15_port_v0.8.19_android.apk.zip` (je 193 B, Zip mit Text-"APK"):

| Lauf | EXIT | SHA256SUMS.txt | git-Index der Sandbox |
|---|---|---|---|
| B4 `--zip-only` | **0** (56 s) | `*..._ANDROID.zip *..._android.apk.zip` + 2 Linux-Volumes | **A** fuer beide Fremddateien |
| B5 `--zip-only --ohne-android` | **0** (58 s), meldet "kein Android-Satz (keiner dieser Version vorhanden)" | dieselben 4 | **A** fuer beide |

Ursache (Code): `for f in "${NAME}"_*.z*` nimmt jede Datei mit dem Versions-Praefix; ausgenommen wird nur der
kanonische Name `"${NAME}_android".z*` (gross/klein-genau). Alle vier Befunde (H1, H2, H3, H5) sind damit bestaetigt.

## 2. Bauplan

- **H1 (Gate-Urteil)**: Die Aufrufer glauben nie mehr nur der Rueckgabe. Neue Funktionen in `release/apk_pruefen.sh`
  (von build_android.sh und make_package.sh geladen):
  - `gate_festhalten`: das Gate wird in einen privaten Ordner kopiert, und die sha256 der KOPIE muss dem versionierten
    Pin `release/apk_asset_gate.sha256` gleichen (wie `apk_signer.sha256`). Eine 0-Byte-, abgeschnittene, mutierte
    oder alte Gate-Datei laeuft gar nicht erst. Anzeige: Pfad + volle sha256.
  - `gate_laufen <modus>`: Ausgabe in eine Datei, danach gezeigt; Urteil = Rueckgabe UND Ausgabe (`gate_urteil`,
    unabhaengiger Python-Code im Aufrufer): letzte Zeile genau die Schlusszeile des Modus, sonst keine Urteilszeile,
    Rueckgabe passt zur Schlusszeile (OK <-> 0, FEHLER/ABWEICHUNG <-> 1, alles andere = 2). Selbsttest zusaetzlich:
    `SELBSTTEST-OK: n/n` mit n >= Mindestzahl, JEDE Fallzeile vorhanden (1..n genau einmal), `[ok]` und `rc = soll`,
    `Innere Proben: m/m` mit m >= Mindestzahl. APK/Paket/Quellbaum: Zaehlzeilen muessen zur Schlusszeile passen
    (Quelle = APK = gleich = Manifest-Zeilen; APK-Eintraege = unzip-Zaehlung - 1; Tuer-Soll g/g).
  - Damit ist auch ein neu gepinntes, aber kaputtes Gate (zweite Schicht) nicht mehr gruen.
- **H2**: `APK_GATE_DATEI` wird nicht mehr aus der Umgebung gelesen (beim Laden von apk_pruefen.sh verworfen, mit
  Hinweis). make_package.sh uebergibt seine Kopie ueber die interne Variable `APK_GATE_KOPIE`, die apk_pruefen.sh beim
  Laden leert; sie wird vor jedem Lauf erneut gegen den Pin geprueft.
- **H3**: SHA256SUMS.txt und `git add` aus einer Positivliste: die in DIESEM Lauf gezippten Saetze (Namen kanonisch),
  bewusst der kanonische Satz der anderen PC-Plattform derselben Version (nach `verify_split`, als "frueherer Lauf"
  gemeldet) und der Android-Satz nur mit `ANDROID_GEZIPPT`. Jede andere Datei `<NAME>_*.z*` bricht ab - vor den
  Kopierminuten und noch einmal vor dem Schreiben der SUMS (auch mit `--ohne-android`).
- **H5 (+U3, V1/V2)**: Pfade der Liste v2 nur druckbares ASCII 0x20-0x7e, jedes Segment 1-251 B (Geraet: Name <= 255 B,
  `.neu` haengt 4 an) - in Gradle `writeAssetManifest` (dazu ASCII-Gross/klein-Dubletten), Gate `_pfad_fehler`
  (+ Quellbaum-Pruefung jeder Datei, damit auch `--quellbaum`/`--paket` frueh abbrechen), Geraete-Leser
  `re15_abgleich_pfad_ok`. Dann ist die ASCII-Faltung der Dublettenregel vollstaendig. Selbsttest-Faelle + innere
  Proben fuer Kelvin/NFD/sz/0x80/0x7e/Segment 251/252, Zeile nur Leerzeichen/Tab (V1), Pfad 0x1f (V2); die
  Selbsttest-Fixture verliert ihren UTF-8-Namen `gruen.tim`.
- **niedrig, im Entpacker (android_glue.c + asset_abgleich.c)**:
  - U1: scheitert `unlink(<zuletzt entpackt>)` (ausser ENOENT) -> kein Lauf, Fehler.
  - U2/E1: ohne gueltige "zuletzt entpackt"-Liste (Erstinstallation, Uebergang v0.8.19, abgebrochener Lauf) werden
    `shared_assets/` und `synchro/` im Speicherordner durchgegangen: jede Datei, die nicht in der neuen Liste steht
    (auch `.neu`-Reste), wird VOR dem Entpacken geloescht, leere Ordner ebenso. Reiner POSIX-Teil in
    `asset_abgleich.c` (`re15_abgleich_waisen`), damit der PC-Unit-Test ihn auf einem echten Ordner prueft. Die Engine
    schreibt in beide Baeume nichts (grep aller fopen-Schreibpfade: nur Logs/Dumps im Arbeitsverzeichnis).
  - U4: jeder Fehlerpfad haelt die Meldung stehen, bis die App geschlossen wird, und beendet dann den Prozess - main()
    laeuft nicht mit altem/gemischtem Baum weiter (main.c ist PC-Code und bleibt unberuehrt).
- Nachweise: Gate-Selbsttest, Pruefer-Mutanten G0-G4/V1/V2 und die Kette A/B in der Sandbox gegen die NEUEN Skripte
  (einmal mit echtem Pin, einmal mit auf den Mutanten umgepinntem Gate = zweite Schicht), PC-Suite, Android-Bau,
  Emulator (frisch, Uebergang v0.8.19, Abbruch + Update mit gestrichener Datei, Kelvin-APK -> abgelehnt + Meldung
  bleibt stehen).

## 3. H1 + H2 behoben (Commit `587e6cdf`)

`release/apk_pruefen.sh` (Kopf "ASSET-GATE: FESTHALTEN UND URTEIL"):
- `gate_pin_pruefen` / `gate_festhalten`: das Gate laeuft nur als private Kopie, deren sha256 in
  `release/apk_asset_gate.sha256` steht (neu, Format wie `apk_signer.sha256`, Kommentar sagt, wie man ihn nach einer
  gewollten Aenderung neu setzt). `gate_laufen` prueft den Pin vor JEDEM Lauf.
- `gate_laufen <modus>` schreibt die Ausgabe in eine Datei (kein Pipe), zeigt sie danach, und `gate_urteil`
  (eigener Python-Code im Aufrufer, liest nur die Ausgabe) entscheidet: letzte Zeile = Schlusszeile des Modus, genau
  eine Urteilszeile im Lauf, keine `ABBRUCH`/`Traceback`/`[FEHLER]`-Zeile, Rueckgabe passend (OK <-> 0,
  FEHLER/ABWEICHUNG <-> 1, sonst 2 = keine Aussage). Selbsttest: `n/n Faelle` mit n >= 258, jede Fallzeile 1..n genau
  einmal, `[ok]` und `rc = soll`, `Innere Proben m/m` mit m >= 132 (Mindestzahlen `GATE_SELBSTTEST_MIN_*`). APK:
  SUMME Quelle = APK = gleich = Manifestzeilen = Schlusszeile, Manifest-Bytes = Bytes gleich, RE2/DOOR + RE15DOOR
  q = a = g > 0, TORSE.VBS gleich, Tuer-Soll g/g, und die APK-Zahl = `unzip -Z1`-Zaehlung der assets/-Eintraege - 1.
  Quellbaum/Paket: Baumzeilen summieren zur Schlusszeile (Paket: je Baum Quelle = gleich).
- H2: `APK_GATE_DATEI` wird beim Laden von apk_pruefen.sh verworfen (Hinweis auf stderr); make_package.sh setzt nach
  dem Laden `APK_GATE_KOPIE` (die festgehaltene, selbstgetestete Kopie), apk_pruefen.sh leert sie beim Laden.
- make_package.sh: Selbsttest, `--quellbaum` und `--paket` laufen nur noch ueber `gate_laufen`.

Nachweis `build/r34a/nb/nb_urteil_test.sh` (Log `build/r34a/nb/logs/urteil_test.txt`, **FEHLER=0**):
- echter Selbsttest-Log -> 0; derselbe Log mit Rueckgabe 1/2 -> 2; leer (G0) / nur Leerzeilen / ohne Schlusszeile /
  FEHLER-Schluss mit Rueckgabe 0 (G1-G3) -> 2 (mit Rueckgabe 1 -> 1); `257/258`, `202/202` (altes Gate), Fallzeile
  17 fehlt, Fallzeile 2 doppelt, eine Fallzeile `rc=0 (soll 1)`, eine `[FEHLER]`-Zeile, `Innere Proben 131/132` bzw.
  `116/116`, Traceback nach der Schlusszeile, ABBRUCH-Zeile, zwei Urteilszeilen, Selbsttest-Log als APK-Urteil -> je 2.
- Pin: echte Kopie angenommen; G0 (0 B), G1 (`main()` ohne `sys.exit`) und das Gate von vor R4-1 abgelehnt.
- zweite Schicht: G1 und G0 mit auf sie UMGEPINNTER Pin-Datei durch `gate_laufen selbsttest` -> Urteil 2.

## 4. H3 behoben (Commit `587e6cdf`)

make_package.sh: `fremde_versionsdateien`/`fremde_abbruch` - jede Datei `<NAME>_*.z*`, deren Rest nicht genau
`(linux_steamdeck_x64|win64|android).(zip|zNN)` ist, bricht ab (mit `--zip`: vor den Kopierminuten und noch einmal vor
dem Schreiben der SUMS; auch mit `--ohne-android`). SHA256SUMS.txt und `git add` nur aus der Positivliste
(`AUSLIEFERN` = PC-Saetze dieses Laufs + kanonischer Satz der anderen PC-Plattform nach `verify_split`, gemeldet als
"frueherer Lauf, nicht neu geprueft"; `ANDROID_VOLUMES` nur mit `ANDROID_GEZIPPT`); ein scheiterndes `git add` bricht
ab. Mit `--no-zip` wird nichts mehr vorgemerkt. Nachweis in der Sandbox: Abschnitt 7.

## 5. H5 (+U3, V1/V2) behoben (Commits `e7b7584f`, `ef5fa0fe`)

Regel der Liste v2 an ALLEN drei Stellen gleich: Pfad nur druckbares ASCII 0x20-0x7e ohne `\`, jedes Segment 1-251 B
(Geraet: Name <= 255 B, der Entpacker schreibt `<name>.neu`), sonst wie bisher. Damit ist die ASCII-Faltung der
Dublettenregel vollstaendig (Unicode-Paare kommen gar nicht erst in die Liste).
- Gate `_pfad_fehler` ("Nicht-ASCII-Byte ...", "Segment mit n Bytes > 251"); neu `quellpfade_pruefen`: jeder Pfad des
  Quellbaums nach derselben Regel + ASCII-Gross/klein-Dubletten, also melden auch `--quellbaum`/`--paket` ein
  Kelvin-Paar (vor den Kopierminuten).
- Geraete-Leser `re15_abgleich_pfad_ok` (`c < 0x20 || c > 0x7e || c == '\'`, Segment <= `RE15_ABGLEICH_SEGMENT_MAX`),
  `utf8_ok` entfaellt (folgt aus ASCII).
- Gradle `writeAssetManifest`: bricht bei Nicht-ASCII, Segment leer/./../> 251 und ASCII-Gross/klein-Dubletten ab.
- Quellbaum heute: 3603 Pfade, 0 ausserhalb 0x20-0x7e, laengster Pfad 53 B, laengstes Segment 27 B, kein Leerzeichen
  (gemessen) - die Regel aendert am Bestand nichts.
- Selbsttest 258/258 (+10 Faelle: Kelvin-Paar in APK / `--quellbaum` / `--paket`, 0x80, `~`, Segment 251/252, Zeile nur
  Leerzeichen/Tab (V1), 0x1f (V2)) und 132 innere Proben (+16, u.a. `quellpfade_pruefen` mit Gross/klein-Dublette, die
  sich auf NTFS nicht als Datei anlegen laesst). Die Fixture verliert ihren UTF-8-Asset-Namen; den rohen UTF-8-Namen in
  der APK prueft jetzt ein Nicht-Asset-Eintrag `res/raw/gr<u-Umlaut>n.bin`. Fall 207 (NBSP am Pfadende) erwartet jetzt
  die Ablehnung an der Pfadregel (+ "fehlt im Manifest", das ein strip()-Mutant verlieren wuerde).
- Mutanten (Werkzeug `build/r34a/nb/nb_mutanten.py`, Logs `build/r34a/nb/logs/mut2_*.log`): Pruefer-V1
  (`if not z.strip()`), Pruefer-V2 (`c < 0x1f`), `c == 0x7e`, ASCII-Grenze `> 0x80`, ASCII-Pruefung aus, Segment
  `> 252`, Segment aus, `SEGMENT_MAX = 255`, Quellpfad-Regel aus, Quell-Dublette aus, `quellpfade_pruefen` nicht
  gerufen: **alle 11 -> SELBSTTEST-FEHLER** (Rueckgabe 1).
- PC-Unit-Test `test_r34a_asset_abgleich`: **324 Pruefungen, 0 Fehler** (vorher 272) - ASCII-Grenzen 0x1f/0x20/0x7e/
  0x7f/0x80/0xff, Kelvin/NFD/U+00DF/4-Byte, Kelvin-Paar als Liste, Segment 251/252 (Datei und Ordner), Pfad 512 B aus
  Segmenten 9/251/250, Zeile nur Leerzeichen/Tab (ungueltig) bzw. nur `\r` (leer). Mit `-std=c11 -Wall -Wextra
  -Wpedantic -Wshadow -Wconversion` ohne Warnung.

## 6. Niedrige Befunde im Entpacker (Commit `ef5fa0fe`)

- **U2/E1 Waisen**: neu `re15_abgleich_waisen` (asset_abgleich.c, POSIX `opendir/lstat/unlink/rmdir`, Namen erst ganz
  einlesen, dann loeschen; Symlinks nie gefolgt; Baumnamen nur einfache Ordnernamen). android_glue.c ruft es, wenn es
  KEINE gueltige "zuletzt entpackt"-Liste gibt (Modi "ohne Liste" und "Uebergang v0.8.19" - genau die Faelle, in denen
  bis dahin niemand wusste, was frueher entpackt wurde), fuer `shared_assets` und `synchro`, VOR dem Entpacken. Die
  Engine schreibt in beide Baeume nichts (alle `fopen(..."w"/"a")` in engine/ und platform/pc/: nur Logs/Dumps im
  Arbeitsverzeichnis bzw. per Umgebungsvariable). Nicht loeschbare Waise = Warnung (die Engine oeffnet sie nie), kein
  Abbruch. Unit-Test auf einem echten Ordner: gelistete Dateien bleiben, Waise, `.neu`-Rest, Waise in eigenem Ordner
  und leere Ordner weg, Wurzel-Dateien (Spielstand) und Ordner ausserhalb der Baeume unberuehrt, zweiter Lauf 0,
  schlechte Baumnamen (`""`, `.`, `..`, `a/b`, `../x`) je Fehler ohne Loeschung, ohne Liste nichts.
- **U4 fail closed**: `fehler_halten` ersetzt die 3-s-Anzeige in allen Fehlerpfaden (kein AssetManager, Liste fehlt/
  ungueltig/v1, kein Speicher, alte Liste nicht loeschbar, Dateifehler beim Entpacken): die Meldung bleibt, bis die App
  geschlossen wird (`SDL_QUIT`/`SDL_APP_TERMINATING` -> `exit(1)`), main() laeuft nicht weiter (main.c unberuehrt).
- **U1**: `unlink(re15_assets_entpackt.txt)` wird geprueft; scheitert es mit etwas anderem als `ENOENT`, laeuft kein Lauf.
- **U3**: mit H5 (Segment <= 251 B).
- Abschlusszeile des Entpackers nennt zusaetzlich `n Waisen entfernt (m nicht loeschbar)`.
- README (platform/android): Gate-Pin, Positivliste, ASCII-Regel, Waisen, fail closed.

## 7. Nachweise

### 7.1 Voller Android-Bau mit der neuen Kette

`bash release/build_android.sh --version v0.8.20-nb1 --no-toolchain` (Log `build/r34a/nb/logs/android_nb1.log`):
**EXIT 0, `ANDROID-BUILD-OK`**. Gradle `BUILD SUCCESSFUL in 2m 56s` (beide ABIs neu, `re15_assets.txt (v2): 3603
Dateien` - die Gradle-Regel laesst den Bestand durch), aapt `versionName='v0.8.20-nb1'`, Signer `432bc749...`,
`Gate: private Kopie ... sha256 8a0e3f15... = festgehalten`, `Gate-Urteil (selbsttest, Rueckgabe 0): SELBSTTEST-OK
258/258, jede Fallzeile [ok] mit rc = soll, innere Proben 132/132`, `Gate-Urteil (apk, Rueckgabe 0):
APK-ASSET-GATE-OK: 3603 Dateien, Quelle = APK = unzip-Zaehlung - 1 = gleich = Manifestzeilen, RE2/DOOR + RE15DOOR +
TORSE.VBS gleich` (RE2/DOOR 27/27, RE15DOOR 30/30). APK **NB1** = `d6921014c580c4e8...`, 363483287 B (nach
`build/r34a/nb/apk/NB1.apk` verschoben; `release/SHA256SUMS_android.txt` per `git restore` auf HEAD). Keine
Compiler-Warnung aus android_glue.c oder asset_abgleich.c (die 12 Warnungen im Log stammen aus Engine-Kopfdateien).
Faelschungen daraus (`nb_faelschen.sh`, Werkzeuge der Pruefer, zipalign + derselbe Debug-Schluessel, verify rc 0):
FW_sig (Leerraumzeile), FD_sig (P07G.DO2 1 Byte gekippt + passende sha256), FK_sig (Kelvin-Paar), R_sig (ohne
RE2/CDEMD0.EMS, Kopf nachgerechnet).
