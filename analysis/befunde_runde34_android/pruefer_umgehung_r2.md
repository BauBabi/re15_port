# Gegenpruefung Runde 2 — Linse UMGEHUNG/ROBUSTHEIT (Android-Asset-Gate)

Stand: 2026-09-30 00:30, abgeschlossen. Gegenpruefer, aendert KEINE Werkzeuge.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Pruefgegenstand: Werkzeugstand 35d25455 (release/apk_asset_gate.py, apk_pruefen.sh, build_android.sh,
make_package.sh, python_finden.sh, app/build.gradle; danach nur Doku-Commits). Belege: `pruefer_umgehung_r2_belege/`.
Arbeitsordner (nicht committet): build/r34a/pruefer_umgehung_r2/. Parallel im Baum: Pruefer "echtlauf r2"
(Android-Bau + make_package.sh in release/) - release/ und platform/android/ habe ich NICHT angefasst;
Kettenlaeufe gegen einen Schattenbaum laufen ueber unveraenderte KOPIEN der Skripte.

## Urteil: NICHT HALTBAR

Das Gate haelt JEDE im Auftrag genannte Faelschungsart (16-Fall-Batterie, 1.9). Es kommen aber durch die
GESAMTE Kette (`build_android.sh --gate-only`, EXIT=0 `ANDROID-GATES-OK`): eine APK mit 29 statt 30
Tuerarchiven (B2), eine nach apksigner-Pruefung getauschte unsignierte Datei (B3), eine fremd signierte
(B4) und eine nicht ausgerichtete, auf Android 11+ nicht installierbare APK (B5). Und der Selbsttest faengt
ein absichtlich kaputtes Gate nicht, sobald es nur TEILWEISE kaputt ist: 5 von 5 Teil-Mutanten bestehen
72/72, einer davon laesst eine signierte, auf dem Geraet nicht entpackbare APK durch die ganze Kette (B1).

## 1. Befunde (Kurzfassung; Einzelheiten Abschnitt 2)

| Nr | Schwere | Befund |
|---|---|---|
| B1 | mittel | Selbsttest faengt TEILWEISE Abschwaechungen nicht: 5/5 Mutanten `SELBSTTEST-OK 72/72`; M1 (Manifest-Groesse nur in EINER Richtung) -> ganze Kette EXIT=0 mit signierter Faelschung |
| B2 | mittel | Konsistent unvollstaendiger Quellbaum: 29/30 RE15DOOR (oder 0-Byte-Archiv) -> ganze Kette EXIT=0; die Soll-Liste (gen/re15_tuer_eigen.inc, 30 x Groesse + FNV-1a) nutzt niemand |
| B3 | niedrig | TOCTOU: Signatur/Version an Datei A, Assets + Kennung an Datei B (Fenster = Selbsttest, 6-55 s) -> unsigniertes K0 wird "gepruefte APK" und waere gezippt |
| B4 | niedrig | Signer nicht festgelegt: fremder Schluessel -> Kette EXIT=0 (Update-Installation unmoeglich, Deinstallation loescht die Spielstaende) |
| B5 | niedrig | Ausrichtung ungeprueft: signiert (v2 true), `zipalign -c` FAILED (resources.arsc BAD, 4 x .so BAD) -> Kette EXIT=0 |
| B6 | niedrig | Drift-Schutz nur build.gradle <-> Gate; die dritte Liste (make_package.sh copy_common) prueft niemand |
| B7 | niedrig | python_finden.sh: WindowsApps ueber 8.3-Pfad + kein readlink im PATH -> der Alias WUERDE gestartet |
| B8 | niedrig | Python-Gate != libziparchive in zwei Punkten (Praefix vor Offset 0; KOPF_RE `\d`) - Kette faengt das erste, das zweite ist nur Anzeige |
| B9 | niedrig | check_binary_fresh: `[[ -n "$src_t" ]] \|\| return` gibt 1 -> set -e bricht make_package.sh STUMM ab (fail closed, ohne Meldung) |
| N1 | (ausserhalb) | Geraete-Entpacker android_glue.c:210 vergleicht nur die Groesse; SDL_RWFromFile liest relative Pfade zuerst aus dem internen Speicher - Codebefund, nicht gemessen, nicht Urteilsgrundlage |

## 2. Einzelheiten

### B1 (mittel) Selbsttest faengt TEILWEISE Abschwaechungen nicht
Werkzeug `r2_mutanten.py` (je genau EINE Textstelle, Kopien unter build/.../mutanten/), Selbsttest je Kopie
(`selbsttests.sh` -> `selbsttests_uebersicht.txt`), dann je Mutant eine Faelschung an der echten APK
(`r2_faelschen.py`, `mutanten_gegen_faelschungen.txt`):

| Mutant (eine Zeile) | Selbsttest | Faelschung (echte APK) | echtes Gate | Mutant | signierbar? |
|---|---|---|---|---|---|
| M1 :641 `apk_dateien[pfad] != eintraege[pfad]` -> `<` | OK 72/72 | P2DS-Manifestzeile 78243 statt 78244, Kopf nachgezogen | 1 | **0** | ja (verify rc 0) |
| M9 :557 `crc != e.crc or n != e.usize` -> nur CRC | OK 72/72 | classes.dex usize +100 (LFH+CD) | 1 | **0** | nein (apksigner `Malformed ZIP entry`) |
| M5 :410 EOCD `!=` -> `<` | OK 72/72 | EOCD-Kommentarlaenge +10 | 2 | **0** | nein (Neuschreiben) |
| M14 :481 DD-Bit aus dem CD statt aus dem LFH | OK 72/72 | P07G: DD-Bit nur im CD, LFH-CRC ^1 | 1 | **0** | nein (`Malformed ZIP entry`) |
| M16 :734 uebrige Eintraege nur Stored | OK 72/72 | classes.dex (Deflate) Byte gekippt | 1 | **0** | nein (`Malformed ZIP entry`) |

Gegenprobe mit dem Beispiel des Auftrags: M0 (sha256-Vergleich :723 auskommentiert) -> `SELBSTTEST-FEHLER:
3 von 72` (Faelle 05, 30, 31) - TOTAL-Abschaltungen faengt der Selbsttest, TEIL-Abschwaechungen nicht.
**Ganze Kette mit M1** (Schatten-release/apk_asset_gate.py := M1, F_M1 signiert): Selbsttest des Mutanten
72/72, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK`, **EXIT=0** (`kette_M1_mutant.log`); dieselbe APK mit dem
echten Gate: `Manifest-Groesse falsch ... 78243 B, APK 78244 B` EXIT=1 (`kette_M1_echt.log`). Auf dem Geraet
liest der Entpacker die ganze Datei (78244 B) und meldet `got != sz` (android_glue.c:225) bei JEDEM Start.
Ursache: keine Selbsttest-Faelle fuer "Manifest-Groesse KLEINER als APK" (nur +1 / 9999), "entpackte Laenge !=
usize bei richtiger CRC", "Kommentarlaenge zu gross", "DD-Bit nur im CD", "komprimierter Nicht-Asset-Eintrag"
(alle `_NICHT_ASSETS` der Fixture sind Stored: `zf.writestr(name, b)`, apk_asset_gate.py:1070-1071).
Die Mutanten-Probe des Bauers (78/84) mutiert nur ganze Pruefungen (befund/raise -> pass, Namensregeln, Hand-
Mutanten); gerichtete Vergleiche (`!=` -> `<`) und Teilbedingungen erzeugt sie nicht.

### B2 (mittel) Konsistent unvollstaendiger Quellbaum -> ganze Kette gruen
Schattenbaum (`build/.../schatten`, PSX+synchro Hardlinks, RE15DOOR/RE2/extracted_fx echte Kopien, build.gradle +
Skripte unveraenderte Kopien; Kontrolle: Schatten-Gate gegen ref rc 0, `schatten_gate_ref`):
- RE15DOOR/P2DS.DO2 im Schatten entfernt + APK ohne den Eintrag (Manifestzeile weg, Kopf `3602 356600033`),
  mit dem Debug-Schluessel signiert: `schatten/release/build_android.sh --gate-only F_ohne_P2DS_sig.apk
  --version v0.8.19` -> **EXIT=0**, `RE15DOOR:  Quelle 29, APK 29, sha256 gleich 29/29`, `ANDROID-GATES-OK`
  (`kette_schatten_ohne_P2DS.log`).
- 0-Byte-Variante: Schatten-P07G.DO2 0 B + APK-Eintrag 0 B -> Gate rc 0, `RE15DOOR 30/30`
  (`schatten_gate_P07G_leer.log`). Dagegen make_package check_tree (unveraenderte Funktion per awk,
  `check_tree_sonde_r2.sh`): `ABBRUCH: Port-Tuerarchiv fehlt/leer im Paket: shared_assets/RE15DOOR/P07G.DO2`
  (`pc_check_tree_P07G_leer.log`) - PC prueft `-s`, das Android-Gate nicht. Bei 29 Archiven ist auch check_tree
  gruen (`pc_check_tree_ohne_P2DS.log`: `Port-Tuerarchive im Paket: 29`).
- Die Engine erwartet 30: `re15_tuer_eigen[30]` (engine/src/gen/re15_tuer_eigen.inc:7, je Archiv Groesse +
  FNV-1a); door_scene_pc.c:242-243 verwirft ein fehlendes/0-Byte/falsches Archiv -> an diesen Tueren still der
  RE1.5-Uebergang. Verankert ist "30" nirgends: Gradle-doFirst verlangt >= 1 *.DO2 (build.gradle:124-126),
  PFLICHT_ORDNER ebenso (apk_asset_gate.py:368-372), check_tree ist relativ zur Quelle.
- Genau das war der offene Punkt ("alle 30 von Hand nachgemessen"). Abhilfe-Richtung: RE15DOOR gegen
  gen/re15_tuer_eigen.inc (Name, Groesse, FNV-1a) und RE2/DOOR gegen die Groessentabelle pruefen - im Gate
  UND in check_tree.

### B3 (niedrig) TOCTOU zwischen den Schritten von apk_pruefen
apk_pruefen liest die APK fuenfmal ueber den PFAD (unzip, aapt, apksigner, Gate); zwischen apksigner (Schritt 3)
und Gate (4.2) liegt der Selbsttest (6-55 s). make_package.sh nimmt die Kennung ERST danach (:464-465).
- `toctou_lauf.sh`: echte `release/build_android.sh --gate-only toctou.apk --version v0.8.19`; sobald das Log
  `Volle Asset-Pruefung 1/2` zeigt, `mv` K0 (unsigniert, Assets gleich) auf den Pfad -> `Verified using v2 ...
  true`, `ANDROID-GATES-OK`, **EXIT=0**; die Datei unter dem Namen: `DOES NOT VERIFY / Missing META-INF/MANIFEST.MF`,
  sha256 = K0 (`toctou_ergebnis.txt`).
- `mp_toctou_sonde.sh`: make_package.sh-APK-Block :439-468 per awk UNVERAENDERT in einer Sandbox (nur die
  Frische-Pruefung als Stub, siehe B9), Tausch um 23:56:18 waehrend des Selbsttests -> `gepruefte APK (sha256
  crc32 Bytes): 2110beff... 1af90309 363208307` = K0, Zipzeit-Vergleich (:662-663) `= gepruefte Datei -> wuerde
  gezippt` (`mp_toctou_ergebnis.txt`).
Gleiche Klasse wie R1-B5 (dort das Kopierfenster, jetzt geschlossen). Abhilfe-Richtung: Kennung VOR Schritt 1
nehmen und nach Schritt 4 vergleichen, oder alle Schritte auf eine private Kopie.

### B4 (niedrig) Signer nicht festgelegt
`keytool -genkeypair` (fremd.jks), K0 damit signiert -> echte Kette EXIT=0, `Signer #1 certificate SHA-256
digest: 88e69cf2...` (Referenz 432bc749...) (`kette_fremder_schluessel.log`). apk_pruefen.sh:118-120 gibt den
Digest nur aus. Folge: `adb install -r` ueber v0.8.19 scheitert (Signatur-Konflikt), Deinstallieren loescht
Android/data/de.re15.port/files (re15_card.mcr, android_glue.c:9-10). build.gradle:211/217 faellt bei einem
RE15_KEYSTORE-Pfad, der nicht existiert, STILL auf den Debug-Schluessel zurueck. Abhilfe-Richtung: erwarteten
Signer-Digest (432bc749...) unter release/ festhalten und vergleichen.

### B5 (niedrig) Ausrichtung ungeprueft
K0 ohne die Ausrichtungs-Extras der Local Header (`--ohne-lfh-extra`), signiert mit `apksigner sign
--alignment-preserved true` -> verify rc 0 (v2 true); `zipalign -c -v -P 16 4` -> **Verification FAILED**:
`resources.arsc (BAD - 1)`, `lib/arm64-v8a/libmain.so (BAD - 3545)` u.a., 2327 BAD (`ausrichtung.txt`); Gate rc 0,
Kette **EXIT=0** (`kette_unausgerichtet.log`). targetSdk 35: Android 11+ installiert keine APK mit nicht
4-Byte-ausgerichteter resources.arsc; useLegacyPackaging=false (build.gradle:235) verlangt seitenausgerichtete .so.
Referenz: `zipalign -c -P 16 4` rc 0. Abhilfe-Richtung: `zipalign -c -P 16 4` (build-tools 35.0.0) in apk_pruefen.

### B6 (niedrig) Drift-Schutz deckt die dritte Liste nicht
Schatten mit neuem Baum `re15_port/shared_assets/RE15NEU/NEU.DAT` (nicht in build.gradle), APK = Referenz ->
Gate rc 0 (`drift_dritte_liste_ergebnis.txt`). Das Gate liest nur build.gradle (GRADLE_REL); copy_common
(make_package.sh:470-495) erwaehnt es nur in Meldungstexten (:296). Ein neuer Baum nur fuer PC (wie RE15DOOR in
Runde 33, wenn build.gradle vergessen worden waere) fehlte in der APK, und alle Gates waeren gruen.

### B7 (niedrig) python_finden.sh startet den Alias ueber den 8.3-Pfad, wenn readlink fehlt
Nur Dateiattribute gelesen, NICHTS gestartet: `/c/Users/MJOEDI~1/AppData/Local/MICROS~1/WINDOW~1/python3`:
-f j, -L j, -s j (folgt dem Link), Pfadregel `*/windowsapps/*` greift NICHT. Instrumentierte Kopie
(`python_finden_SONDE.sh` = Original bis auf die zwei Startzeilen :92/:94 -> `echo WUERDE STARTEN`),
`python_finden_83_sonde.txt`:
- (a) PATH = nur 8.3-WindowsApps (kein readlink/timeout erreichbar), NUR_PATH=1 -> `SONDE: WUERDE STARTEN (ohne
  timeout): .../WINDOW~1/python3` (und `python`)
- (c) dasselbe ohne NUR_PATH -> der Alias kommt VOR /c/Python310 dran
- (d) `RE15_PYTHON=<8.3>/python3.exe`, PATH leer -> WUERDE STARTEN
- (b)/(e) mit /usr/bin (readlink da) -> `verworfen (Link auf WindowsApps ... NICHT gestartet)`.
Ursache :79 `if [[ -L "$kand" ]] && type -P readlink ...` - ohne readlink wird die Linkregel uebersprungen statt
den Kandidaten zu verwerfen. Der Kopf (:32-33) verspricht "laeuft auch mit einem PATH, der NUR den
WindowsApps-Ordner enthaelt" - stimmt nur fuer den Langnamen. Abhilfe-Richtung: `-L` ohne readlink = verwerfen.
Echte Umgebung unauffaellig: Langname -> `verworfen (WindowsApps-Alias, NICHT gestartet)`, Kette ABBRUCH vor
jedem Gate; kein Python im PATH -> /c/Python310 (`python_finden_kette.txt`).

### B8 (niedrig) Python-Gate bildet libziparchive nicht vollstaendig nach
- F_praefix (16 Null-Bytes vor dem ersten Local Header, alle Offsets verschoben): Gate **rc 0** `ZIP-Struktur ...
  wie Android sie liest`; aapt2/aapt 35.0.0: `Zip: Entry at offset zero has invalid LFH signature 0 ... failed
  opening zip: Invalid file.` (`libziparchive_praefix.txt`). In der Kette faengt es aapt badging (und apksigner).
- F_kopf_arabisch (Kopfzeile `# re15 assets ٣٦٠٣ ٣٥٦٦٧٨٢٧٧`, signiert): Kette **EXIT=0**
  (`kette_kopf_arabisch.log`) - KOPF_RE :105 nutzt `\d` (Unicode-Ziffern), das Groessenfeld :609 `[0-9]+`;
  das Geraet (sscanf `%ld`, android_glue.c:163) liest 0/0 -> nur Fortschrittsanzeige und Marker.

### B9 (niedrig) check_binary_fresh bricht unter set -e stumm ab
`frische_stumm_sonde.sh` (Funktion per awk unveraendert, Mini-Repo ohne passenden Commit): `vor
check_binary_fresh`, dann **EXIT=1 ohne Meldung** (`frische_stumm.txt`). Ursache make_package.sh:134/136/138
`... || return` = Status des fehlgeschlagenen Tests (1) -> set -e. Gemeint ist "ueberspringen". Fail closed,
aber ohne ABBRUCH-Text (so auch in meiner ersten Sandbox-Sonde, 23:44-23:51, geschehen). Kein Durchlass.

### N1 (ausserhalb des Gates, Codebefund, nicht gemessen) Geraete-Entpacker
- android_glue.c:210 `if (file_size(dst) != sz)` - nach einem Update wird eine Datei gleicher Groesse NIE neu
  entpackt. Tuerarchive aendern sich genau so: P07G.DO2 dreimal mit 55908 B (b22088b5 -> 1aaa98c7 -> d094cea1);
  fuenf Port-Archive haben 55908 B. Eine Textur-Korrektur erreicht aktualisierte Geraete nicht (APK richtig).
- SDL_RWFromFile (_deps/SDL2-2.28.5/src/file/SDL_rwops.c:536-551, Asset-System erst :566) oeffnet relative Pfade ZUERST unter
  SDL_AndroidGetInternalStoragePath(). Ist der Anker intern (kein beschreibbarer externer Speicher,
  android_glue.c:54-55), ist das die Zieldatei selbst: bei geaenderter Groesse liest android_glue.c:212 die alte
  Datei, die :213 `fopen(dst, "wb")` gerade gekuerzt hat -> 0 B -> Fehler bei jedem Start.
Beides liegt ausserhalb des Auftrags (nur Release-Werkzeuge) und des Gates; das Gate belegt nur die APK.

## 1.9 Was HAELT (gemessen)
- **Batterie der Auftragsfaelle** (`batterie.sh` -> `batterie_ergebnis.txt`, eigene Varianten, alle soll = ist):
  STR 6 MB letztes Byte (rc 1 sha256), BSS mittleres Byte (1), Ordner kleingeschrieben mit stimmigem Manifest (1),
  doppelter Eintrag mit richtigem ERSTEM (1 `Duplicate entry`), Verzeichnis `assets/shared_assets/` (1), `\` nur im
  CD (1), Manifest CRLF / Leerzeilen (0 = wie das Geraet), Geisterzeile (1), Tab im Pfad (1), `assets/synchro/
  README.md` (1), abgeschnitten / 0 Byte (2), Schatten: extracted_fx fehlt (1), RE15DOOR nur leerer Unterordner
  (1), build.gradle mit zusaetzlichem Baum (2). Dazu echte Gate-Laeufe F_M1/F_M9/F_M14/F_M16 rc 1, F_M5 rc 2.
- **--gate-only-Randfaelle** (`gate_only_randfaelle.txt`): fehlt/Ordner/Textdatei -> 1, `''`/ohne Pfad/ohne
  --version/leere Version -> 2, falsche Version -> 1 (`aapt: versionName`).
- **make_package.sh**: `bash -n` ok (alle vier Skripte); keine blanken python/python3-Aufrufe mehr - `"$PY"` an
  :442 (apk_kennung), :507 (verify_split), :527 (verify_apk_im_zip), :635/:638 (zip_exec_bit.py); cmp greift
  (`pc_check_tree_cmp.txt`): TORSE.VBS, DOOR36.DO2, DOOR04.DO2 im Paket letztes Byte ^1 -> je rc 1 mit der
  richtigen Meldung, Positivkontrolle CHECK_TREE_OK (27 + 30).
- **Verschlucken**: in build_android.sh/apk_pruefen.sh stehen `|| true` nur an Anzeigezeilen (apk_pruefen.sh:100,
  105, 115, 120; danach explizite grep/die), alle Gate-Rueckgaben `rc=0; ... || rc=$?` + explizites die, kein
  Pipe/Subshell um eine Entscheidung. Einziger Fehlerweg ohne Meldung: B9 (fail closed). build_android.sh:193
  (`java -version | head -1 | grep -q` unter pipefail) kann hoechstens FALSCH ROT werden, nicht gruen.
- **Python-Falle**: Schnappschuss vorher == nachher (121 Zeilen, `Compare-Object` leer,
  `py_zustand_0_vorher.txt`/`py_zustand_1_nachher.txt`); laufender python-Prozess um 00:22 = C:\Python310 der
  Granaten-Sitzung (build/r34g_a/mutation.py), nicht meiner. Alle meine Laeufe mit /c/Python310/python oder
  python_finden (-> /c/Python310). **Eigener Fehlgriff:** ein `env PATH=<WindowsApps>:/usr/bin bash ...` loeste
  `bash` ueber den neuen PATH auf -> WindowsApps\bash.exe (WSL-Starter) lief kurz an und brach ab (`WSL ...
  execvpe(/bin/bash) failed`); kein Python, keine Installation (Schnappschuss gleich, keine neuen WSL-Prozesse).
  Danach nur noch `/usr/bin/bash` absolut.

## 3. Werkzeuge / Belege
- `r2_faelschen.py` roher ZIP-Leser/-Schreiber (Eingriffe: ohne, daten, manifest-weg/-groesse/-kopf-arabisch/
  -zeile-plus/-crlf/-leerzeilen, usize-plus, cd-dd, lfh-crc-kippen, daten-kippen, kommentar-plus, praefix,
  umbenennen, cd-backslash, doppelt, verzeichnis, ohne-lfh-extra); Kontrolle K0 rc 0.
- `r2_mutanten.py`, `selbsttests.sh`, `batterie.sh`, `toctou_lauf.sh`, `mp_toctou_sonde.sh`,
  `check_tree_sonde_r2.sh` (Kopie der Sonde des Bauers), `frische_stumm_sonde.sh`, `py_zustand.ps1`.
- Referenz: Kopie der Archiv-APK v0.8.19, sha256 514bebd5... = Archiv-SUMS (Archiv nur gelesen).

## 9. Laufprotokoll
- 23:32 Dossier; Bestand gelesen; Python-Schnappschuss 0 (121 Zeilen).
- 23:34 ref kopiert, K0 + erste Faelschungen, echtes Gate je Faelschung.
- 23:37-23:38 Selbsttest echtes Gate + 5 Mutanten; Mutanten gegen Faelschungen. 23:39 Signieren.
- 23:40-23:43 Schattenbaum; Kette ohne P2DS (EXIT=0); 0-Byte-P07G; check_tree 0 B / 29.
- 23:43 Kette M1-Mutant/echt; fremder Schluessel; TOCTOU --gate-only.
- 23:44-23:51 erste make_package-Sonde stumm abgebrochen (-> B9); 23:56 Sonde mit Stub: Kennung = K0.
- 00:04-00:17 Batterie (16 Faelle); 00:17 bash -n/grep; 00:18 --gate-only-Randfaelle, python_finden 8.3-Sonde,
  Ketten-Faelle Alias/kein Python (+ WSL-Fehlgriff); 00:20 Ausrichtung; 00:21 Drift; 00:23 Schnappschuss 1;
  00:24 cmp-Negativfaelle; 00:26 M0 (Auftragsbeispiel) gefangen.
- 00:33 Python-Schnappschuss 2 (`py_zustand_2_ende.txt`) == Schnappschuss 0 (121 Zeilen, gleich).
- Endstand: `git status --short -- re15_port synchro release` leer; Schatten/Paket/Sandbox/APK-Kopien geloescht (7,2 GB -> 1,2 MB)
  (Hardlinks - Originale unberuehrt), Logs unter build/r34a/pruefer_umgehung_r2/logs/ liegen gelassen.
