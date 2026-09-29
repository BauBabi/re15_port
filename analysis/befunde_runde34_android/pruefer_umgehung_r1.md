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
Belege: `pruefer_umgehung_r1_belege/F_gate_ergebnisse.txt`, `F_gateonly_ergebnisse.txt`,
`aapt2_libziparchive_gegenprobe.txt`.

### B2 (mittel) Keine Signaturpruefung: unsignierte/manipulierte APK -> Gate-Kette rc 0
apksigner 35.0.0 `verify`: Referenz rc 0; K0 (reines Neuschreiben, Signing Block weg) `DOES NOT VERIFY ERROR:
Missing META-INF/MANIFEST.MF`; F2/F4 `APK Signature Scheme v2 signer #1: APK integrity check failed.
CHUNKED_SHA256 digest mismatch`. Alle drei bestehen apk_asset_gate.py mit rc 0 (F2/F4 auch die volle
--gate-only-Kette). Auch die K0-"Identitaet" des Bauers (3.4, zipfile-Umschrift) ist unsigniert und lief rc 0.
Eine solche APK ist nicht installierbar (targetSdk 35 verlangt v2+). make_package.sh zippt sie trotzdem.

(weitere Befunde folgen unten, Abschnitt 1 wird fortgeschrieben)

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

## 9. Laufprotokoll
- 21:18 Dossier angelegt; Bestand/Code gelesen.
- 21:22 Python-Schnappschuss vorher (`py_snapshot_vorher.txt`, 27 Zeilen = Stand des Bauers).
- 21:24 F1-F8 + K0 erzeugt; 21:25 Gate je Fall unter 3.10.11 und 3.14.7.
- 21:26 aapt2/aapt-Gegenprobe (libziparchive); 21:27 build_android.sh --gate-only F2-F5; apksigner.
