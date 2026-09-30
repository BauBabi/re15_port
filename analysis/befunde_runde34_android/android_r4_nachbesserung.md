# Runde 34 Android - Stufe 4 Nachbesserung (Runde 1)

Angelegt 2026-09-30 10:40. Auftrag: Gegenpruefer-Befunde pruefer_*_r4_1.md beheben (H1 hoch; H5, H3, H2 mittel; niedrige nach Moeglichkeit).


## 0. Stand / Plan

Gegenstand: Gegenpruefer-Befunde `pruefer_umgehung_r4_1.md` (H1 hoch; H5, H3, H2 mittel; V1/V2, U1-U4 niedrig) und
`pruefer_echtlauf_r4_1.md` (E1 niedrig = U2). Arbeitsordner (nicht versioniert): `build/r34a/nb/`. Ausgangsstand HEAD
`79ebd085` (+ Dossier-Commit `e777d796`). Python nur `/c/Python310/python` bzw. `release/python_finden.sh`.

- [x] 1 Befunde selbst nachmessen (H1, H2, H5 schnell; H3 per make_package in eigener Sandbox)
- [x] 2 Bauplan
- [ ] 3 H1 + H2: Gate-Urteil aus Rueckgabe UND Ausgabe, Gate-Pin, keine Umgebungsvariable mehr
- [ ] 4 H3: SHA256SUMS.txt + git add aus einer Positivliste, fremde Versionsdateien -> Abbruch
- [ ] 5 H5 (+U3, V1/V2): Pfade nur druckbares ASCII, Segment <= 251 B - Gradle, Gate, Geraete-Leser, Tests
- [ ] 6 niedrig: U1, U2/E1 (Waisen), U4 (fail closed) im Entpacker
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
