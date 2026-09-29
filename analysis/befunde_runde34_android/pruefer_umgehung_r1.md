# Gegenpruefung Runde 1 — Linse UMGEHUNG/ROBUSTHEIT (Android-Asset-Gate)

Stand: 2026-09-29, laufend fortgeschrieben.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate, geprueft d8a3ad09)
Pruefgegenstand: Commits a358fd5d..HEAD des Bauers (Dossier analysis/befunde_runde34_android/android_gate.md)
Rolle: Gegenpruefer — ich aendere KEINE Werkzeuge. Faelschungen nur unter build/r34a/pruefer_umgehung_r1/.
Belege: analysis/befunde_runde34_android/pruefer_umgehung_r1_belege/ (Werkzeug `umgehung_werkzeug.py`
arbeitet auf ROHEN ZIP-Bytes, nicht ueber zipfile - sonst erbte es genau die Normalisierung, die es aufdeckt).

Hinweis Parallelbetrieb: der Pruefer "echtlauf" faehrt im selben Baum make_package.sh/Android-Bau
(release/*.zip, *.z01, SHA256SUMS.txt und release/re15_port_v0.8.19_android.apk um 21:20-21:26 von IHM
geschrieben, siehe build/r34a/pruefer_echtlauf_r1/mp_3c_voll.log). Ich fasse release/ nicht an.

## 0. Vorgehen
1. Commits + Bauer-Dossier + Gate-Code gelesen (apk_asset_gate.py, build_android.sh, python_finden.sh,
   make_package.sh-Diff, build.gradle-Diff, Geraete-Leser android_glue.c:146-256).
2. Faelschungen an Kopien der Referenz-APK (roh gepatcht) + Mini-Faelle; jede gegen das Gate, rc selbst abgefangen.
3. Gegenprobe mit einem UNABHAENGIGEN Leser: Androids libziparchive (steckt im Host-aapt2/aapt 35.0.0)
   und apksigner 35.0.0.
4. Fehler-Verschlucken im Shell-Code, make_package.sh, Selbsttest-Mutanten, Python-Falle.

## 1. Befunde (Kurzfassung, Einzelheiten Abschnitt 2 ff.)

### B1 (hoch) Drei Faelschungen kommen durch die GESAMTE Gate-Kette (rc 0), obwohl Android den Eintrag nicht liest
Ziel jeweils `assets/shared_assets/RE15DOOR/P07G.DO2` — eine der 30 Tuerdateien, um die es im offenen Punkt geht
(keine Stichprobe der alten Gates). Rohe 1-4-Byte-Patches an einer Kopie der Referenz-APK:
| Fall | Aenderung | apk_asset_gate.py (3.10.11 und 3.14.7) | build_android.sh --gate-only --version v0.8.19 | libziparchive (aapt2 35.0.0) |
|---|---|---|---|---|
| F2 | Name in LFH+CD `/` -> `\` (gleiche Laenge) | **rc 0** GATE-OK 3603/3603, RE15DOOR 30/30 | **rc 0** ANDROID-GATES-OK | `error: failed to find file.` |
| F4 | CRC-Feld NUR im Local File Header ^1 (CD richtig) | **rc 0** | **rc 0** | `Zip: size/crc32 mismatch ... failed opening zip: Inconsistent information.` |
| F5 | unkompr. Groesse NUR im LFH +1 | **rc 0** | **rc 0** | `Zip: size/crc32 mismatch ... Inconsistent information.` |
| F3 | Name := Name + `\0abc` (neu geschrieben) | **rc 0** | rc 1 (nur das ALTE aapt-Gate: libziparchive oeffnet die APK gar nicht) | `Zip: invalid file name at entry 3205 ... Invalid entry name.` |
Ursache (Code): `zipfile.ZipInfo` schneidet Namen am ersten NUL ab und ersetzt unter Windows `\` durch `/`
(3.10: os.sep; mingw-3.14 hat os.sep='/', aber os.altsep='\\' -> ebenfalls ersetzt), und `ZipFile.open`
vergleicht vom Local Header nur den NAMEN, nicht CRC/Groessen. Das Gate liest die APK ausschliesslich ueber
zipfile (apk_asset_gate.py:465 ff., :357-365) und vergleicht Namen erst NACH dieser Normalisierung.
Auf dem Geraet (android_glue.c:212 SDL_RWFromFile -> AAssetManager -> libziparchive) scheitert genau diese
Datei -> Entpack-Fehler bei jedem Start, Marker nie geschrieben. F3 faengt in build_android.sh nur das alte
aapt-Gate; make_package.sh ruft vor dem Zippen NUR apk_asset_gate.py (make_package.sh:423-432) -> dort kommt
auch F3 durch.
Nebenbei sichtbar: fuer F2 meldet die alte Stichprobenzeile "3603 Asset-Eintraege" statt 3604 (unzip zeigt den
rohen `\`-Namen), prueft den Wert aber nicht (build_android.sh:149-150).
**Installierbar:** F2 laesst sich mit DEMSELBEN Schluessel wie der Release-Bau neu signieren
(`apksigner sign --ks ~/.android/debug.keystore`, build.gradle:205-226) -> `apksigner verify` rc 0, Signer-SHA-256
432bc749... = Referenz (also sogar als Update ueber die Original-APK installierbar), aapt2 weiter `failed to find
file`, `build_android.sh --gate-only F2_signiert --version v0.8.19` -> **rc 0 ANDROID-GATES-OK**
(`apksigner_und_F2_signiert.txt`). F4 dagegen weist apksigner ab (`Malformed ZIP entry`) - F4/F5 blieben
unsigniert (B2).
Einordnung: ein normaler AGP-Bau erzeugt weder `\`/NUL-Namen noch LFH/CD-Widersprueche; es sind gezielte
Byte-Faelschungen (bzw. Folgen eines fehlerhaften Nachbearbeitungswerkzeugs). Nach dem Massstab des Auftrags
("haltbar nur, wenn keine Faelschung durchkommt"; `\` war ausdruecklich genannt) reicht das fuer "nicht haltbar".
Abhilfe-Richtung (nicht gebaut, ich aendere keine Werkzeuge): Namen roh vergleichen (`ZipInfo.orig_filename`,
`\`/NUL ablehnen) und je Eintrag den Local Header gegen das CD pruefen (CRC, Groessen, Methode, Name) - oder
den Eintrag zusaetzlich ueber einen libziparchive-Leser oeffnen; dazu `apksigner verify` (B2).
Belege: `pruefer_umgehung_r1_belege/F_gate_ergebnisse.txt`, `F_gateonly_ergebnisse.txt`,
`aapt2_libziparchive_gegenprobe.txt`.

### B2 (mittel) Keine Signaturpruefung: unsignierte/manipulierte APK -> Gate-Kette rc 0
apksigner 35.0.0 `verify`: Referenz rc 0; K0 (reines Neuschreiben, Signing Block weg) `DOES NOT VERIFY ERROR:
Missing META-INF/MANIFEST.MF`; F2/F4 `APK Signature Scheme v2 signer #1: APK integrity check failed.
CHUNKED_SHA256 digest mismatch`. Alle drei bestehen apk_asset_gate.py mit rc 0 (F2/F4 auch die volle
--gate-only-Kette). Auch die K0-"Identitaet" des Bauers (3.4, zipfile-Umschrift) ist unsigniert und lief rc 0.
Eine solche APK ist nicht installierbar (targetSdk 35 verlangt v2+). make_package.sh zippt sie trotzdem.

### B3 (mittel) Der Selbsttest faengt vier tragende Abschwaechungen NICHT (28/28 trotz kaputtem Gate)
Mutanten (`pruefer_umgehung_r1_belege/mutanten_umgehung.py`, je EINE Pruefung abgeschwaecht, Kopien unter
build/r34a/pruefer_umgehung_r1/mutanten/), jeweils `<mutant> --selbsttest` -> **SELBSTTEST-OK 28/28**, und
jeweils eine Faelschung, die das ECHTE Gate ablehnt, der Mutant aber mit rc 0 annimmt:
| Mutant | Selbsttest | Faelschung | echtes Gate | Mutant |
|---|---|---|---|---|
| U1 "Manifest-Zeile ohne APK-Eintrag" still | 28/28 OK | A1 Geisterzeile `5\tshared_assets/RE15DOOR/GEIST.DO2`, Kopf angepasst | rc 1 | **rc 0** |
| U2 doppelte Manifest-Zeile: spaetere gewinnt still | 28/28 OK | A2 P2DS.DO2 zweimal, erste mit 9999 B (Geraet: Entpack-Fehler, android_glue.c:225) | rc 1 | **rc 0** |
| U3 Inhalt nur ueber den ersten 1-MiB-Block | 28/28 OK | F1 (echte APK, ENEMSE.VBS 6,7 MB, Aenderung bei L-40) | rc 1 | **rc 0** |
| U4 CRC32 statt sha256 (APK-Eintrag weiter gelesen) | 28/28 OK | F1 (CRC32 bleibt 52dc25a2) | rc 1 | **rc 0** |
| U7 Groessenfeld nicht auf Ziffern geprueft | 28/28 OK | A6 `1_800` (int() 1800, atoll() 1) | rc 1 | rc 0 (konstruiert) |
Nicht tragend (andere Pruefung faengt es, gemessen): U5 Verzeichniseintrag (A3: rc 1 ueber "zusaetzlich"),
U6 Kommentarzeile (A4 versteckte Zeile: rc 1 ueber "fehlt im Manifest"), U8 64-MiB-Grenze (unrealistisch).
Ursache: die Fixture-Dateien sind hoechstens 5000 B (< BLOCK = 1 MiB, apk_asset_gate.py:90, :633-648), keine
Faelschung erhaelt die CRC32, und es gibt keinen Fall "Manifest-Zeile fuer eine Datei, die weder in APK noch
Quelle liegt" und keinen Fall "doppelte Manifest-Zeile". Die Mutanten-Probe des Bauers (M1-M8) deckte nur
Totalabschaltungen ab. Folge in make_package.sh (APK-Block per awk ausgeschnitten, HERE mit U3 als
apk_asset_gate.py): Selbsttest 28/28, F1 -> `APK-BLOCK-OK (make_package.sh wuerde diese APK zippen)`; mit dem
echten Gate: `ABBRUCH: Android-APK passt nicht zum Quellbaum (apk_asset_gate.py rc=1)`.
Belege: `mutanten_und_minifaelle.txt`, `make_package_apk_block_sonde.sh`, Abschnitt 4.

### B4 (niedrig) --gate-only mit --version, aber ohne aapt: Versionspruefung still uebersprungen, rc 0
`ANDROID_SDK_ROOT=/c/gibt_es_nicht bash release/build_android.sh --gate-only <ref> --version v9.9.9`
-> `(aapt nicht gefunden - badging-Gate uebersprungen)` ... `== ANDROID-GATES-OK (--gate-only) ==`, **rc 0**,
obwohl die APK versionName 'v0.8.19' hat (mit SDK: rc 1 `aapt: versionName ist nicht 'v9.9.9'`). Der Kopf
(build_android.sh:40) verspricht "Version wird nur mit --version geprueft"; ohne aapt fallen auch Paketname
und ABIs weg (:157-173). Im vollen Bau ist das SDK Pflicht (:212), aapt kommt aus build-tools (AGP braucht
es nicht - ein SDK ohne build-tools/aapt ist moeglich).

### B5 (niedrig) make_package.sh: Pruefzeitpunkt und Zipzeitpunkt der APK fallen auseinander
Gepruefte APK = `release/<name>_android.apk` zum Zeitpunkt make_package.sh:423-432, gezippt wird die Datei, die
bei :591-596 dort liegt; dazwischen liegen copy_common + Laufzeit-Gates beider Plattformen (Minuten). Eine
in dieser Zeit ersetzte/neu abgelegte APK (z.B. von Hand kopiert) wird ungeprueft gezippt; lag bei :423 keine
APK, entfaellt die Pruefung ganz. (Nur Codebefund; make_package.sh selbst nicht gefahren, der Pruefer
"echtlauf" nutzt release/ im selben Baum.) Ebenfalls nur Assets: eine APK mit veraltetem libmain.so (Code
nach dem APK-Bau geaendert, gleiche Version) besteht die Pruefung - fuer die PC-Binaries gibt es dafuer
check_binary_fresh (:119-139), fuer die APK nichts.

(weitere Befunde folgen unten, Abschnitt 1 wird fortgeschrieben)

### Was HAELT (gemessen)
- Gleiche Groesse/anderer Inhalt in einer GROSSEN Datei, sogar CRC32-erhaltend (F1, VAB 6,7 MB): rc 1.
- Gross/klein (F6), abgeschnittene APK (F7, rc 2), zerstoerter CD-Eintrag (F8, rc 2), Verschluesselungsbit
  (A15, rc 2), Methode 99 (A16, rc 2), Verzeichniseintrag (A3), Doppelzeile/Geisterzeile/versteckte Zeile/
  Tab/Leerzeichen/NUL im Manifest (A1-A4, A9-A11: rc 1), Leerzeilen und `\r\r\n` (A7/A8: rc 0 = wie das Geraet),
  RE15DOOR nur mit leerem Unterordner (A17: rc 1).
- Drift-Schutz: ein Baum ueber `tasks.named("stageAssets")` AUSSERHALB des register-Blocks umgeht zwar den
  build.gradle-Abgleich (kein rc 2), der Inhalt faellt aber als "zusaetzlich in der APK" auf (A19 rc 1);
  liefert der Baum nichts, bleibt rc 0 (A20, richtig). Keine Faelschung ueber diesen Weg.
- python_finden.sh: nur WindowsApps im PATH + NUR_PATH=1 -> rc 1 ohne Start, build_android.sh --gate-only
  bricht VOR jedem Gate ab; kein Python im PATH -> Rueckfall /c/Python310 (3.10.11). Alias in 8.3-Schreibweise
  (`C:\Users\MJOEDI~1\...\WINDOW~1\python3.exe`): Pfadregel greift nicht, die Linkregel (readlink -f ->
  `/c/Program Files/WindowsApps/PythonSoftwareFoundation.PythonManager_.../python3.exe`) verwirft ihn ohne
  Start (`guard_probe.sh`, ohne einen Kandidaten zu starten). Anmerkung: die "0-Byte"-Regel greift unter MSYS
  fuer Aliase NICHT (`-s` folgt dem Link; Kopfkommentar python_finden.sh:23 ist dort ungenau) - harmlos, weil
  die Linkregel vorher greift.
- --gate-only: Pfad fehlt/Ordner -> rc 1, leer/ohne Pfad -> rc 2, `--version` ohne Wert -> rc 1 (set -u).
- make_package.sh check_tree (eigene Schattenkopie, echte Funktion per awk): TORSE.VBS letztes Byte -> rc 1,
  DOOR36.DO2 letztes Byte -> rc 1, DOOR36.DO2 fehlt -> rc 1; alte Fassung a358fd5d bei TORSE -> rc 0 (Luecke
  geschlossen). `bash -n` fuer make_package.sh, build_android.sh, python_finden.sh: OK. python_finden an beiden
  Stellen (verify_split `"$PY" -`, zip_exec_bit.py 2x `"$PY"`) - sonst kein python3/python-Aufruf.
- Shell-Fehlerwege build_android.sh: alle neuen Gate-Aufrufe `rc=0; ... || rc=$?` + explizites die, run_gates
  nie in einem Bedingungskontext (set -e wirkt), kein Pipe/Subshell um die Rueckgaben. `|| true` nur an
  Anzeige-/Suchstellen (:149 Zaehlung, :158 aapt-Suche, :160-162 badging-Anzeige; danach explizite greps).
  Einziger weicher Weg: aapt fehlt -> ueberspringen (B4). `source python_finden.sh || die` schaltet set -e im
  Skript ab - python_finden arbeitet mit expliziten Rueckgaben, geprueft ok.

## 2. Faelschungen an der Referenz-APK (Kopien unter build/r34a/pruefer_umgehung_r1/apk/)

Werkzeug: `pruefer_umgehung_r1_belege/umgehung_werkzeug.py forge <ref> <ziel> <art> <eintrag>`
(`inspect`: 3616 Eintraege, APK Signing Block 4096 B vor dem CD, keine Data Descriptors, Methoden 0/8,
LFH/CD konsistent, keine `\`/NUL-Namen).

| Fall | Art | 3.10.11 | 3.14.7 | Meldung |
|---|---|---|---|---|
| K0 | Neuschreiben ohne Aenderung (Kontrolle des Werkzeugs) | 0 | 0 | GATE-OK (Werkzeug transparent) |
| F1 | ENEMSE.VBS (VAB, 6694912 B): 1 Nutzbyte bei L-40 + 4 Ausgleichsbytes, **CRC32 gleich** (52dc25a2), Groesse gleich, Header unveraendert | 1 | 1 | `Inhalt weicht ab (sha256, gleiche Groesse 6694912 B): assets/shared_assets/RE2/ENEMSE.VBS` — sha256 greift auch hinter dem 1. MiB-Block und wo CRC blind ist |
| F2 | `\` statt `/` | **0** | **0** | GATE-OK (B1) |
| F3 | NUL-Anhang | **0** | **0** | GATE-OK (B1) |
| F4 | LFH-CRC | **0** | **0** | GATE-OK (B1) |
| F5 | LFH-Groesse | **0** | **0** | GATE-OK (B1) |
| F6 | P07G.DO2 -> p07g.do2 (Gross/klein) | 1 | 1 | fehlt / zusaetzlich / Manifest-Zeile ohne APK-Eintrag / fehlt im Manifest |
| F7 | letzte 100 B abgeschnitten | 2 | 2 | `APK nicht lesbar ... (File is not a zip file)` — fail closed |
| F8 | CD-Eintrag TEX.TIM Signatur zerstoert | 2 | 2 | `APK nicht lesbar ... (Bad magic number for central directory)` — fail closed |

Gegenprobe mit libziparchive (aapt2 35.0.0, `aapt2 dump xmltree --file <eintrag> <apk>`; bei einer DO2 heisst
"failed to parse file as binary XML" = Eintrag gefunden UND gelesen): ref/K0 gelesen (55908 B); F2 `failed to
find file`; F3 `Zip: invalid file name at entry 3205 ... Invalid entry name` (ganze APK nicht zu oeffnen);
F4/F5 `Zip: size/crc32 mismatch ... Inconsistent information`. Der Geraete-Leser geht denselben Weg:
android_glue.c:212 `SDL_RWFromFile(rel)` -> SDL_android.c:1945 `AAssetManager_open` -> libziparchive.
apksigner 35.0.0 verify: ref rc 0; K0 `Missing META-INF/MANIFEST.MF`; F2/F4 `CHUNKED_SHA256 digest mismatch`.
zipfile-Quelltext: 3.10 `C:\Python310\lib\zipfile.py:351` (NUL), `:358` (os.sep -> '/'), `:1559` (einziger
LFH-Vergleich: Name); 3.14.7 `mingw64/lib/python3.14/zipfile/__init__.py:410/:417/:419` (os.altsep='\\'
-> '/'), `:1770`. Unter Linux (os.altsep None) wuerde F2 gefangen - aus dem Quelltext, nicht gemessen;
F3-F5 gingen dort ebenso durch.

## 3. Mini-Faelle (`minifaelle_umgehung.py`, Fixture `_Fall` des Gates, echtes Gate + Mutanten je Prozess)
| Fall | echtes Gate | Bemerkung |
|---|---|---|
| A1 Geisterzeile (weder APK noch Quelle) | 1 | U1: 0 |
| A2 Doppelzeile, erste falsche Groesse | 1 | U2: 0 |
| A3 Verzeichniseintrag | 1 | U5: 1 (redundant) |
| A4 Zeile auskommentiert | 1 | U6: 1 (redundant) |
| A5 harmlose `# Notiz` | 1 | strenger als das Geraet (ok) |
| A6 Groesse `1_800` | 1 | U7: 0 |
| A7 Leerzeilen / A8 `\r\r\n` | 0 / 0 | wie das Geraet (harmlos) |
| A9 Tab im Pfad / A10 Leerzeichen am Ende / A11 NUL-Zeile | 1 / 1 / 1 | |
| A12 `\` im Namen / A13 NUL im Namen / A14 LFH-CRC | **0 / 0 / 0** | Mini-Nachbau von B1 - passte in den Selbsttest |
| A15 Verschluesselungsbit / A16 Methode 99 | 2 / 2 | fail closed |
| A17 RE15DOOR nur leerer Unterordner | 1 | |
| A18 `synchro/STAGEX.txt` nur in der Quelle | 1 | Gate erwartet Dateien auf oberster Ebene, die auf `STAGE*/**` passen (wie Gradle); Abweichung waere in beide Richtungen rc 1 |
| A19/A20 Baum per tasks.named ausserhalb | 1 / 0 | siehe "Was haelt" |

## 4. Mutanten (`mutanten_umgehung.py`) - siehe B3; Selbsttest-Logs build/r34a/pruefer_umgehung_r1/logs/selbsttest_U*.log

## 9. Laufprotokoll
- 21:18 Dossier angelegt; Bestand/Code gelesen.
- 21:22 Python-Schnappschuss vorher (`py_snapshot_vorher.txt`, 27 Zeilen = Stand des Bauers).
- 21:24 F1-F8 + K0 erzeugt; 21:25 Gate je Fall unter 3.10.11 und 3.14.7.
- 21:26 aapt2/aapt-Gegenprobe (libziparchive); 21:27 build_android.sh --gate-only F2-F5; apksigner.
- 21:29 Mutanten U1-U8 + Selbsttest je Mutant (alle 28/28); 21:31 Mini-Faelle A1-A20; U3/U4 gegen F1.
- 21:33 make_package.sh-APK-Block (awk) mit echtem Gate/U3; build_android.sh-Randfaelle V1-V7.
- 21:35 python_finden: nur Alias / kein Python / guard_probe (8.3-Pfad, ohne Start); Zwischen-Schnappschuss.
- 21:37 check_tree-cmp an eigener Schattenkopie (neu vs. a358fd5d).
- 21:40 F2/F4 mit dem Release-Schluessel neu signiert; F2_signiert durch --gate-only (rc 0).
- 21:42 End-Schnappschuss, Aufraeumen.

## 10. Endstand (21:43)
- Python-Schnappschuss vorher/zwischen/nachher identisch (27 Zeilen, `py_snapshot_*.txt`); 0 Dateien/Ordner
  neuer als 21:18 in `%LOCALAPPDATA%\Python`, `...\WindowsApps`, `...\Programs\Python`, Benutzer-Startmenue;
  0 python/pymanager/msiexec-Prozesse. Kein Aufruf des WindowsApps-Alias (python_finden verwarf ihn in jedem Lauf).
- Aufgeraeumt: alle Faelschungs-APKs (3,7 GB), Schattenkopie, Mini-Repos, Hardlinks unter
  build/r34a/pruefer_umgehung_r1/; liegen gelassen: logs/, mutanten/, make_package_alt.sh (klein). Jede
  Faelschung ist mit `umgehung_werkzeug.py forge` in Sekunden wiederherstellbar.
- build/r34a/ref_v0.8.19.apk unveraendert (sha256 514bebd5... = Archiv-SUMS). Archiv unter C:/workspace/Re15Data
  nur gelesen (SUMS). `git status --short release/`: leer (die zeitweisen Paketdateien waren vom Pruefer echtlauf).
- Werkzeuge (release/*, build.gradle) NICHT geaendert.

## 11. Urteil: NICHT HALTBAR
Die Asset-Pruefung selbst ist gruendlich (sha256 ueber alles, auch CRC-erhaltende Aenderungen in grossen
Dateien; Manifest wie der Geraete-Leser; fail closed bei kaputter APK/fehlendem Python; kein verschluckter
Rueckgabewert in build_android.sh). Aber sie prueft die APK so, wie Pythons zipfile sie sieht, nicht so, wie
Android sie liest: `\`-Namen, NUL-Namen und LFH/CD-Widersprueche kommen durch (B1), F2 sogar korrekt signiert
und installierbar durch die GESAMTE Kette. Der Selbsttest bestaetigt vier tragende Abschwaechungen nicht (B3).
Nebenbefunde B2 (keine Signaturpruefung), B4 (Version ohne aapt still ungeprueft), B5 (Zeitfenster
Pruefen/Zippen in make_package.sh).
