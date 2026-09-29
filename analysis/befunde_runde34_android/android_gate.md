# Runde 34a — Volle Android-Asset-Prüfung (Dossier)

Zweig `r34a/android-gate`, Arbeitsbaum `.claude/worktrees/r34a_android` (Basis master a358fd5d +
Auftrag c1c91713). Auftrag: `analysis/befunde_runde34_android/AUFTRAG.md` — offener Punkt aus
v0.8.19: „Der Android-Bau prüft bisher nur eine der 30 neuen Türdateien automatisch."

Status: **fertig** (Werkzeuge + Tests; vollen Android-Bau zusätzlich selbst gefahren, Abschnitt 3.7).
Kein Spielcode angefasst (nur `release/*`, `app/build.gradle`, dieses Dossier).

## 0. Laufprotokoll

- 20:3x Dossier angelegt, Bestand gelesen (Abschnitt 1), Python-Schnappschuss (3.0).
- 20:40 `release/apk_asset_gate.py` + Selbsttest 28/28; Referenz-APK rc=0 (3.1, 3.2).
- 20:45 Mutanten-Probe des Selbsttests (3.3); `release/python_finden.sh` + Tests (3.8).
- 20:50 `build_android.sh` (Gates als Funktion, `--gate-only`), `build.gradle`, `make_package.sh`.
- 20:53-21:00 Gradle-Laeufe stageAssets (3.6), Negativ-Kontrollen an APK-Kopien (3.4/3.5),
  check_tree-/verify_split-Sonden (3.9).
- 21:02 voller Android-Bau mit den neuen Gates (3.7).

## 1. Bestand (gelesen, nicht vermutet)

- `re15_port/platform/android/app/build.gradle:99-122` stageAssets (Sync) spiegelt fuenf Baeume:
  `shared_assets/{PSX,extracted_fx,RE2,RE15DOOR}` (Basis portRoot) und `synchro` mit
  `include "STAGE*/**"` (Basis repoRoot). doFirst prueft nur Stichproben, fuer die Tuerarchive
  genau EINE Datei (`shared_assets/RE15DOOR/P07G.DO2`), RE2/DOOR gar nicht. Kopfkommentar :12-15
  nannte noch "vier Asset-Baeume" (RE15DOOR fehlte dort).
- `:124-145` writeAssetManifest: Kopfzeile `# re15 assets <n> <summe>\n`, dann
  `"<len>\t<relativePath.pathString>"` je Datei, als Zeichenketten sortiert, mit `\n` verbunden,
  abschliessendes `\n` (`manifest.text = ...`, :141).
- Leser auf dem Geraet `re15_port/platform/android/jni/android_glue.c:146-256`:
  liest `re15_assets.txt` per SDL_RWFromFile (hoechstens 64 MiB, :80), Kopf per
  `sscanf(man, "# re15 assets %ld %lld")` am PUFFERANFANG (:163), Zeilen an `\n` getrennt (:197),
  angehaengte `\r` abgeschnitten (:200), leere und `#`-Zeilen uebersprungen (:201), Zeile ohne Tab
  STILL uebersprungen (:203), Groesse per `atoll` (:205), Pfad = Rest nach dem ersten Tab, gelesen
  als `assets/<pfad>` (SDL_RWFromFile(rel)), kopiert nach `<anker>/<pfad>`; Fehler, wenn die
  gelesene Bytezahl != Manifest-Groesse (:225) -> Marker wird nie geschrieben.
  Folgen fuer das Gate: fehlende Manifestzeile = Datei wird NIE entpackt; falsche Groesse =
  Entpack-Fehler bei jedem Start; BOM vor der Kopfzeile = Kopf unlesbar (n=0).
- `release/build_android.sh:211-237` (alt) Gates: 10 Stichproben (`unzip -Z1`), Zahl der
  `assets/`-Eintraege, WARNUNG bei `Defl:N` (nur unter `assets/shared_assets/`), aapt badging
  (Paketname + beide ABIs; die im Kopf behauptete VERSIONS-Pruefung fehlte im Code). Die APK lag
  schon VOR den Gates unter `release/<name>.apk`.
- `release/make_package.sh` (alt): check_tree :199-219 RE2/DOOR nur `-s` (kein cmp), RE15DOOR mit
  cmp, TORSE.VBS nur `-s`; Python-Aufrufe `python3 -` :438 (verify_split) und
  `python ".../zip_exec_bit.py"` :536/:539 (der Auftrag nannte :413/:511). v0.8.19 lief dafuer mit
  einem Shim `build/py3shim` im PATH (analysis/befunde_runde33/release_v0819.md Abschnitt 3).
- Git-Bash dieser Maschine (gemessen, `type -a -P`): `python3` -> 1. WindowsApps-Alias,
  2. `/c/msys64/mingw64/bin/python3`; `python` -> `/c/Python310/python`, `/c/Python39/python`,
  WindowsApps-Alias, `/c/msys64/mingw64/bin/python`. Die WindowsApps-Eintraege sind App-Aliase
  des "PythonSoftwareFoundation.PythonManager" (MSYS zeigt sie als Link nach
  `/c/Program Files/WindowsApps/PythonSoftwareFoundation.PythonManager_26.3.240.0_x64__.../python3.exe`).
- `/c/msys64/mingw64/bin/python3.exe` ist **Python 3.14.7 aus MSYS2** (pacman-DB
  `mingw-w64-x86_64-python-3.14.7-1`, INSTALLDATE 1787580399 = 2026-08-24 16:06:39, REASON 1 =
  Abhaengigkeit von `mingw-w64-x86_64-gdb-17.2-1`, also die MSYS2-Neueinrichtung aus CLAUDE.md) —
  ein echtes Python, NICHT der Installer-Unfall aus v0.8.17. `python_finden.sh` waehlt es auf dieser
  Maschine (erster echter `python3` im PATH); `RE15_PYTHON=/c/Python310/python.exe` pinnt 3.10.
- Referenz-APK v0.8.19 (Kopie `build/r34a/ref_v0.8.19.apk`, sha256 514bebd5... = Archiv-SUMS):
  3616 Eintraege, davon **3604 unter assets/** (3603 Dateien + re15_assets.txt), alle Stored,
  keine Verzeichniseintraege, keine Duplikate; Manifest 163901 B, LF, kein BOM,
  Kopf `# re15 assets 3603 356678277`. Hinweis: die Auftragszahl "3571" ist der Stand v0.8.18
  (release_v0819.md:86-87: +33 = 30 RE15DOOR + 3 RE2/DOOR).
- Quellbaum Arbeitsbaum: PSX 3193 / extracted_fx 13 / RE2 277 / RE15DOOR 30 / synchro 289
  (davon STAGE1 78 + STAGE2 12 im Muster, `unused/` + README.md draussen). Keine Links, keine
  Punkt-/Unterstrich-/Nicht-ASCII-Namen in den eingeschlossenen Teilen (Gradle-Standardausschluesse
  bzw. das AAPT-Ignoriermuster greifen also heute nicht; das Gate nennt sie trotzdem, falls eine
  solche Datei auftaucht).
- Unabhaengige Quelle fuer RE15DOOR (Groesse + FNV-1a je Archiv): `engine/src/gen/re15_tuer_eigen.inc`,
  geprueft von der Suite (`tests/unit/probes/r33_tueren.cmake`, Test "archive"). Das Gate prueft
  APK gegen Quellbaum; Quellbaum gegen Tabelle bleibt Sache der Suite.

## 2. Werkzeuge (was sich geaendert hat)

### 2.1 `release/apk_asset_gate.py` (neu, reines Python >= 3.8, keine Fremdpakete)
- Baumliste `BAEUME` steht EINMAL im Skript und wird gegen den stageAssets-Block der build.gradle
  geprueft: Block per Klammer-Suche (Zeichenketten + Kommentare ausgenommen), je
  `from(new File(<portRoot|repoRoot>, "<ordner>")) { into ...; include ... }` ein Tupel;
  zugelassen sonst nur `into(assetStage)`, `preserve { include "re15_assets.txt" }`,
  `description`, `duplicatesStrategy`, `doFirst`, `doLast`. Jede andere Anweisung (z.B. ein
  `exclude`) oder eine andere Baumliste -> Rueckgabe 2 mit "nur in build.gradle / nur im Gate".
- (a) jede Quelldatei jedes Baums liegt als `assets/<ziel>/<pfad>` in der APK;
  (b) Groesse, dann sha256 (Inhalt ueber zipfile; zipfile prueft die CRC mit — Bitfehler =
  "beschaedigt (CRC)"); (c) nichts sonst unter `assets/` ausser `re15_assets.txt`, keine
  Doppel- oder Verzeichniseintraege; (d) Manifest exakt wie der Geraete-Leser gelesen
  (`\n`-Trennung, `\r` abschneiden, leere/`#`-Zeilen ueberspringen) plus Kopfzeile = Anzahl/Summe,
  jede Zeile trifft genau einen APK-Eintrag gleicher Groesse, jede Asset-Datei steht im Manifest,
  keine Zeile ohne Tab, keine Nicht-Zahl-Groesse, kein `..`/absoluter Pfad, kein BOM, UTF-8,
  <= 64 MiB; (e) Zaehlung je Baum + RE2/DOOR, RE15DOOR, TORSE.VBS.
  Pflichtinhalt: kein Baum fehlt/leer, RE2/DOOR und RE15DOOR mit *.DO2, TORSE.VBS nicht leer.
- (f) Rueckgabe 0/1/2; 2 auch fuer JEDE unerwartete Ausnahme (fail closed); Befunde je Art
  bis `--max-zeilen` (25), dann "... und N weitere".
- (g) `--selbsttest`: 28 Faelle im Temp-Ordner (Mini-Repo mit build.gradle-Muster inkl.
  Kommentar-/Zeichenketten-Fallen, Mini-APK, Manifest im Schreiberformat): 3 gute Varianten
  (Original, Manifest mit CRLF, ein Eintrag Deflate -> WARNUNG) muessen 0 liefern, 18 Faelschungen
  1, 7 Bedien-/Konfigurationsfehler 2 — jeweils MIT der treffenden Meldung (Teilstring-Pruefung),
  jeder Fall als eigener Prozess ueber den echten Einstieg `main()`.

### 2.2 `release/python_finden.sh` (neu, per `source`)
- Kandidaten: `$RE15_PYTHON` (dann nur dieser), sonst alle `python3`, dann alle `python` im PATH
  (`type -a -P`), unter Windows zusaetzlich `/c/Python3*/python.exe` und
  `"/c/Program Files"/Python3*/python.exe` (abschaltbar: `RE15_PYTHON_NUR_PATH=1`).
- Pfad (oder Link-Ziel) unter `*/WindowsApps/*` -> verworfen **ohne Start**; fehlende/0-Byte-Datei
  -> verworfen ohne Start; sonst `-c "import sys, zipfile, hashlib; ..."` mit stdin=/dev/null und
  `timeout 30` -> muss >= 3.8 melden. Setzt `PY`, `PY_VERSION`; nichts gefunden -> Meldung, rc 1.
- Nur Bash-Bordmittel (ausser Kandidat, optional timeout/readlink) — laeuft auch mit einem PATH,
  der nur den WindowsApps-Ordner enthaelt; `${VAR:-}` durchgehend (Aufrufer haben `set -u`).

### 2.3 `release/build_android.sh`
- Gates als Funktion `run_gates <apk>`: bisherige Stichproben + Deflate-WARNUNG + aapt
  (jetzt auch `versionName='<Version>'`), danach `"$PY" apk_asset_gate.py --selbsttest` und
  `"$PY" apk_asset_gate.py --repo <repo> <apk>`; Rueckgabe je Aufruf getrennt abgefangen
  (`rc=0; ... || rc=$?`), 1 -> "weicht vom Quellbaum ab", sonst "keine Aussage moeglich".
- `--gate-only <apk> [--version v]`: nur `run_gates`, kein JDK/Toolchain/Gradle, schreibt nichts.
- Python wird VOR dem Bau gesucht (`source python_finden.sh || die`), Toolchain-Konstanten nach
  oben gezogen (aapt braucht BUILD_TOOLS_PKG auch ohne Bau), Pfade fuer Windows-Programme per
  `cygpath -m` (`nativ_pfad`), zwei set-e-Fallen im alten Gate-Code entschaerft (`ls | head` ohne
  `|| true`, `echo | grep | sed` ohne Treffer beendeten das Skript still).
- **Neu: Gates vor dem Auslieferungsnamen.** Die APK wird als `<name>.apk.ungeprueft` kopiert,
  geprueft und erst dann per `mv` zu `<name>.apk` (+ SHA256SUMS). Bis v0.8.19 lag sie schon vor
  den Gates dort; ein Gate-Abbruch haette sie liegen lassen und make_package.sh haette genau diese
  abgelehnte APK gezippt.
- Kopfkommentar (ERGEBNIS, AUFRUF, UMGEBUNG, GATES) nachgezogen; `--help` druckt bis
  `set -euo pipefail` statt fester Zeilennummern.

### 2.4 `re15_port/platform/android/app/build.gradle`
- doFirst: statt `RE15DOOR/P07G.DO2` als Einzeldatei zaehlen jetzt `RE2/DOOR` und `RE15DOOR`
  ihre `*.DO2` (0 -> GradleException, sonst `logger.lifecycle` mit der Zahl). Der Rest bleibt;
  JEDE Datei prueft das APK-Gate. Kopfkommentar: fuenf Baeume + Hinweis auf die zweite Liste im Gate.

### 2.5 `release/make_package.sh`
- `source python_finden.sh` (nur bei `DO_ZIP=1`, vor dem Kopieren); `verify_split` ruft
  `"$PY" -`, zip_exec_bit.py beide Male `"$PY"`. RE2/DOOR je Datei und TORSE.VBS zusaetzlich per
  `cmp -s` gegen den Quellbaum; TORSE.VBS gehoert jetzt zur Eingangspruefung der Quelle.

## 3. Messwerte

### 3.0 Python-Schnappschuss VOR allen Laeufen (20:34)
`werkzeug/py_snapshot.ps1`: HKCU/HKLM `Software\Python` rekursiv, Startmenue `Python*`
(Benutzer + ProgramData), `%LOCALAPPDATA%\Python`, `%LOCALAPPDATA%\Programs\Python` (27 Zeilen).
Stand: HKCU `PythonCore` ohne Unterschluessel; HKLM 3.9/3.10/3.12; Startmenue nur ProgramData
3.9/3.10/3.12; `%LOCALAPPDATA%\Python\_cache` (15:39, vor dieser Sitzung); kein Programs\Python.
`werkzeug/py_neu_seit.ps1`: Dateien/Ordner neuer als 20:30 unter `%LOCALAPPDATA%\Python`,
`...\Microsoft\WindowsApps`, `...\Programs\Python`, Benutzer-Startmenue.

### 3.1 (a) Gate gegen die Referenz-APK (Kopie) und den Arbeitsbaum -> rc 0
```
Baum (unter assets/)          Quelle     APK   gleich Bytes gleich
shared_assets/PSX               3193    3193     3193    300522652
shared_assets/extracted_fx        13      13       13      1079875
shared_assets/RE2                277     277      277     25482039
shared_assets/RE15DOOR            30      30       30      1914932
synchro                           90      90       90     27678779
SUMME                           3603    3603     3603    356678277
RE2/DOOR:  Quelle 27, APK 27, sha256 gleich 27/27
RE15DOOR:  Quelle 30, APK 30, sha256 gleich 30/30
TORSE.VBS: Quelle 19176 B, APK 19176 B, sha256 gleich
Manifest:  3603 Zeilen / 356678277 Bytes
== APK-ASSET-GATE-OK: 3603 Dateien in 5 Baeumen bytegleich, Manifest stimmt ==
```
Laufzeit: 4.8 s (Python 3.10.11, erster Lauf), 3.2 s (mingw64 3.14.7), 3.6-3.7 s in `--gate-only`.
Deckt sich mit release_v0819.md:86 (3603 Dateien / 356678277 B). Auch die Textdateien
(extracted_fx/README.md, effect0_clut.txt, RE2/FILES/toc.csv; core.autocrlf=true) sind gleich.

### 3.2 (b) `--selbsttest` -> rc 0
`== SELBSTTEST-OK: 28/28 Faelle ==` unter Python 3.10.11 (10.5 s, kalt), 3.9.0 (5.1 s),
mingw64 3.14.7 (3.9 s; in `--gate-only` 3.8-4.4 s).
Faelle: 01 gute APK, 02 Manifest CRLF, 03 Eintrag Deflate (0 + WARNUNG); 04 RE15DOOR-Eintrag
entfernt, 05 RE2/DOOR 1 Byte, 06 Zusatzeintrag, 07 synchro/unused in der APK, 08 Manifest falsche
Groesse, 09 Manifest fehlende Zeile, 10 Datei nur im Quellbaum, 11 APK-Eintrag kuerzer, 12 Byte im
APK-Datenstrom gekippt (CRC), 13 doppelter Eintrag, 14 Kopfzeile Anzahl falsch, 15 BOM, 16 Manifest
fehlt, 17 Zeile ohne Tab, 18 Pfad mit `..`, 19 RE15DOOR leer, 20 RE15DOOR-Ordner fehlt, 21 TORSE.VBS
fehlt (alle 1); 22 kein ZIP, 23 APK fehlt, 24 build.gradle zusaetzlicher Baum, 25 Baum RE15DOOR
fehlt, 26 include geaendert, 27 unbekannte Anweisung (`exclude`), 28 build.gradle fehlt (alle 2).

### 3.3 Bestaetigt sich der Selbsttest selbst? Mutanten-Probe (`werkzeug/mutanten.py`)
Acht Kopien des Gates mit je EINER abgeschalteten Pruefung; der Selbsttest muss jede erkennen:
| Mutant | Selbsttest | rote Faelle |
|---|---|---|
| M1 sha256-Vergleich aus | rc 1 | 05 |
| M2 Zusatz-Pruefung (c) aus | rc 1 | 06, 07 |
| M3 "fehlt im Manifest" aus | rc 1 | 09, 17 |
| M4 `\r` nicht abschneiden | rc 1 | 02 |
| M5 build.gradle-Abgleich aus | rc 1 | 24, 25, 26, 27 |
| M6 Abweichung meldet rc 0 | rc 1 | 04-21 (18) |
| M7 CRC-Fehler als gleich | rc 1 | 12 |
| M8 Pflichtinhalt aus | rc 1 | 19 |

### 3.4 (c) Negativ-Kontrollen an Kopien der Referenz-APK (`werkzeug/faelschen.py`, zipfile)
Jede Faelschung aendert genau eine Stelle (vom Skript gezaehlt); K0 = unveraenderte Umschrift
(beweist, dass das Umschreiben selbst nichts ausloest).
| Faelschung | Gate rc | Befunde (woertlich gekuerzt) |
|---|---|---|
| K0 Identitaet | 0 | OK, 3603 bytegleich |
| N1 RE15DOOR/P07G.DO2 entfernt | 1 | `fehlt in der APK: assets/shared_assets/RE15DOOR/P07G.DO2`; `Manifest nennt ... (55908 B), die APK hat keinen Eintrag`; RE15DOOR 29/30 |
| N2 RE2/DOOR/DOOR04.DO2 1 Byte (gleiche Groesse) | 1 | `Inhalt weicht ab (sha256, gleiche Groesse 69560 B): assets/shared_assets/RE2/DOOR/DOOR04.DO2`; RE2/DOOR 26/27 |
| N3 PSX/SOUND/SUB_00.BGM entfernt | 1 | `fehlt in der APK: ...SUB_00.BGM`; Manifest-Zeile ohne APK-Eintrag (100804 B) |
| N4 Zusatz-Asset PSX/ZUSATZ.BIN | 1 | `zusaetzlich in der APK ...ZUSATZ.BIN`; `fehlt im Manifest: shared_assets/PSX/ZUSATZ.BIN` |
| N5 Manifestzeile P2DS.DO2 +1 B | 1 | `Manifest-Groesse falsch: ... Manifest 78245 B, APK 78244 B`; Kopfzeile 356678277 != 356678278 |
| N6 Manifestzeile synchro/STAGE1/room1170/main00.wav fehlt | 1 | `fehlt im Manifest: synchro/STAGE1/room1170/main00.wav`; Kopfzeile 3603 != 3602 |
Laufzeit je Lauf 3.5-4.3 s.

### 3.5 (d) Dasselbe ueber `bash release/build_android.sh --gate-only <apk> --version v0.8.19`
- Original-Kopie: rc 0 (`== ANDROID-GATES-OK (--gate-only) ==`, 11 s gesamt), K0: rc 0.
- N1-N6: je rc 1, `ABBRUCH: APK-Asset-Gate: die APK weicht vom Quellbaum ab`. In ALLEN sechs Faellen
  bestanden die alten Gates (Stichproben, aapt inkl. versionName) — erst die volle Pruefung schlug an.
- Weitere: `--version v0.0.1` -> rc 1 `aapt: versionName ist nicht 'v0.0.1'`; ohne `--version`
  -> rc 0 mit `(versionName nicht geprueft ...)`; APK fehlt -> rc 1; `--gate-only` ohne Pfad ->
  rc 2; Textdatei als APK -> rc 1 `APK nicht lesbar (unzip -Z1)`.
- `release/SHA256SUMS_android.txt` vorher = nachher (sha256sum -c OK), `git status release/` zeigte
  nur die eigenen Quelltextaenderungen.

### 3.6 build.gradle
- Groovy 3.0.22 (die Groovy-Version von Gradle 8.11.1, `FileSystemCompiler`, Stub fuer
  GradleException): alte Fassung rc 0 (48 Klassen), neue rc 0 (50: zwei Closures mehr), absichtlich
  kaputte Variante (`if (n == 0` ohne Klammer) rc 1 "Unexpected input".
- `gradlew -Pre15Version=v0.8.19 stageAssets writeAssetManifest` (31 s): `stageAssets:
  re15_port/shared_assets/RE2/DOOR: 27 x *.DO2`, `... RE15DOOR: 30 x *.DO2`, `re15_assets.txt:
  3603 Dateien, 356678277 Bytes`; das frisch geschriebene Manifest ist BYTEGLEICH mit dem der
  Referenz-APK (sha256 5f5acfdb...).
- Negativ: alle 30 RE15DOOR/*.DO2 kurz nach build/r34a geparkt -> `Task :app:stageAssets FAILED`,
  `Tuerarchive fehlen im Repo: re15_port/shared_assets/RE15DOOR/*.DO2 (0 Dateien)`, rc 1 (14 s);
  zurueck, `sha256sum -c` 30/30, `git status re15_port/` sauber.
- Baumlisten-Abgleich an ECHTEN Fassungen: build.gradle vor Runde 33 (4ccb96e6, ohne RE15DOOR) ->
  rc 2 `nur im Gate: portRoot/shared_assets/RE15DOOR`; aktuelle Fassung + Zeile
  `shared_assets/NEU` -> rc 2 `nur in build.gradle: portRoot/shared_assets/NEU`.

### 3.7 Voller Android-Bau mit den neuen Gates
_folgt (Lauf gestartet 21:02:11)_

### 3.8 (f) python_finden.sh
| Lauf | Ergebnis |
|---|---|
| T1 normaler Git-Bash-PATH | WindowsApps-`python3` verworfen (nicht gestartet), `/c/msys64/mingw64/bin/python3 (3.14.7)`, rc 0 |
| T2 PATH NUR WindowsApps + `RE15_PYTHON_NUR_PATH=1`, per `source` unter `set -euo pipefail` | beide Aliase verworfen, `FEHLER: kein echtes Python >= 3.8 ...`, source rc 1 (sauberer Abbruch) |
| T3 PATH NUR WindowsApps, Rueckfall an | Aliase verworfen, `/c/Python310/python.exe (3.10.11)`, rc 0 |
| T4 `RE15_PYTHON=<WindowsApps>/python3.exe` | verworfen, KEIN Rueckfall, rc 1 |
| T5 `RE15_PYTHON=/c/Python39/python.exe` | 3.9.0, rc 0 |
| T6 `RE15_PYTHON` zeigt ins Leere | verworfen (fehlt), rc 1 |
| V6 `build_android.sh --gate-only` mit PATH=/usr/bin + WindowsApps, NUR_PATH=1 | `ABBRUCH: kein echtes Python >= 3.8`, rc 1 — vor jedem Gate |
Schnappschuss nach T1-T6, nach allen Gate-/Sonden-Laeufen: **unveraendert** (27 Zeilen, `diff` leer);
0 Dateien neuer als 20:30 in den Python-/WindowsApps-/Startmenue-Ordnern; kein
python/pymanager/msiexec-Prozess. (Endstand siehe 3.10.)

### 3.9 make_package.sh
- `bash -n` OK.
- check_tree — die ECHTE Funktion (per awk aus dem Skript, `werkzeug/check_tree_sonde.sh`) gegen
  ein Schattenpaket (4 FX, 3 RE2-Dateien, 27 RE2/DOOR, 30 RE15DOOR, synchro STAGE1+2), neu gegen alt
  (a358fd5d):
  | Fall | neu | alt |
  |---|---|---|
  | A intakt | rc 0 (27 x RE2/DOOR, 30 x RE15DOOR) | rc 0 |
  | B DOOR04.DO2 1 Byte (gleiche Groesse) | rc 1 `RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR04.DO2` | **rc 0** (Luecke) |
  | C TORSE.VBS 1 Byte | rc 1 `RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS` | **rc 0** (Luecke) |
  | D wieder intakt | rc 0 | — |
- verify_split + zip_exec_bit.py ueber `"$PY"` (mingw64 3.14.7; `werkzeug/split_probe.sh`) an Kopien
  der v0.8.19-Split-Saetze: android (1) rc 0, linux (3606; Katalog 3800) rc 0, linux (9999) rc 1,
  android ohne .z01 rc 1 `fehlende Volumes: [1]`; `zip_exec_bit pruefen` linux rc 0 (re15_pc +
  run.sh 100755), android re15_pc rc 1. Kopien danach sha256-gleich mit der Archiv-SUMS (4/4).
- Den vollen make_package-Lauf fahren die Pruefer.

### 3.10 Endstand
_folgt_

## 4. Offen / Hinweise fuer die Pruefer

- Den vollen `make_package.sh`-Lauf (mit Zippen) habe ich nicht gefahren; verify_split,
  zip_exec_bit.py und check_tree sind einzeln gegen echte Artefakte belegt (3.9).
- `release/*.py` fallen nicht unter `*.sh text eol=lf` (.gitattributes, nicht in meinem
  Dateibereich): unter core.autocrlf=true liegen sie im Windows-Arbeitsbaum mit CRLF (wie schon
  zip_exec_bit.py). Harmlos, weil beide immer als `"$PY" datei.py` gestartet werden.
- Auf dieser Maschine waehlt python_finden.sh das MSYS2-Python 3.14.7 (erster echter `python3` im
  PATH, siehe Abschnitt 1). Wer 3.10 will: `RE15_PYTHON=/c/Python310/python.exe`.
- Weitere `python3`-Aufrufe in `release/`: keine (grep ueber release/*.sh, release/docker/*.sh).
