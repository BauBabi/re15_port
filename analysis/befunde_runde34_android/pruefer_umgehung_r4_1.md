# Gegenpruefung Runde 1 — Linse UMGEHUNG/ROBUSTHEIT (Stufe-4-Kette + Geraete-Entpacker N1)

Pruefer: Gegenpruefer UMGEHUNG/ROBUSTHEIT, Runde 1 (Sitzung 2026-09-30).
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate).
Gegenstand: Commits `git log --oneline be8b60f3..HEAD` — Stufe-4-Kette (B1-B4, README;
Dossier android_gate_r4_kette.md) und Geraete-Entpacker N1 (Dossier android_entpacker_n1.md).
Regel: Ich aendere KEINE Werkzeuge/Quellen. Mutanten/Zusatzfaelle nur in Kopien unter build/r34a/pruefer_u1/.

Ziel: ein falsches Ergebnis erzwingen (Umgehung) oder einen verschluckten Fehler finden (Robustheit).

## 0. Stand / Fortschritt

- [ ] 0.1 Commit-Liste und Stand aufnehmen
- [ ] 1 Kette: veralteter Android-Satz (nur PC, abgelehnte APK, APK geloescht, --ohne-android, --only)
- [ ] 2 Kette: sha256-Kennung der APK im Split-Satz
- [ ] 3 Kette: make_package-PC-Pfad mit kaputtem Gate (Mutant) -> Abbruch?
- [ ] 4 Selbsttest: eigene neue Ein-Zeilen-Mutanten (auch v2-Manifest-Teil)
- [ ] 5 Format v2: Faelschungen
- [ ] 6 Entpacker: Code + PC-Unit-Test + Zusatzfaelle (Pfade, Abbruch/Neustart, JNI-Fehlerpfade, SHA-256)
- [ ] 7 Urteil


## 0.1 Ausgangslage (gelesen, nicht gemessen)

- HEAD `6b8e8cc8` (Zweig r34a/android-gate). Gegenstand `be8b60f3..HEAD`: Kette 42d72ec1 (B2), 3b677ea0 (B1/B3/B4,
  README), 7f269d04, 80559d2f (B1 nachgeschaerft), cb6617fd (Temp-Pfade), 93f85469 (Dossier); N1 eda2360b ..
  6b8e8cc8 (asset_abgleich.{h,c}, android_glue.c, build.gradle v2, Gate v2 248 Faelle + 116 innere Proben,
  Unit-Test 272 Pruefungen). Parallel arbeitet der Pruefer ECHTER LAUF im selben Baum (pruefer_echtlauf_r4_1.md) -
  ich committe nur meine Pfade, Emulator nur nach `adb devices`/qemu-Pruefung.
- Gelesen: beide Dossiers ganz, asset_abgleich.{h,c} ganz, android_glue.c ganz, Unit-Test ganz, build.gradle
  (stageAssets/writeAssetManifest), make_package.sh (Diff be8b60f3..HEAD + Zip-/Git-Abschnitt), apk_pruefen.sh
  (Schritte 0-6), apk_asset_gate.py (Kopf, Konstanten, `_pfad_fehler`, `manifest_lesen`, `manifest_pruefen`,
  Selbsttest-Harness `_fall_laufen`/`_fall_bewerten`/`selbsttest`, `main`), main.c um die Bootstrap-Aufrufe,
  Mutanten-Werkzeug der R2-Kampagne (`mutanten_teil.py`: "Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/main)").

## 0.2 Hypothesen aus dem Lesen (werden unten gemessen)

- H1 (B4): Das Urteil jedes Gate-Aufrufs ist NUR sein Exit-Code (make_package.sh `(( rc_selbst == 0 ))`, `case "$rc"
  in 0)`; apk_pruefen.sh `(( rc == 0 ))`). Den Exit-Code erzeugt derselbe Code, der geprueft werden soll. Eine
  leere/abgeschnittene Gate-Datei (Memory reai-v2-patchskript-truncate: das ist schon einmal passiert) oder ein
  Ein-Zeilen-Mutant im Ausgangspfad (`sys.exit(main())` -> `main()`, `RC_ABWEICHUNG = 0`, `return rc if ...` ->
  `return RC_GLEICH`) liefert fuer Selbsttest UND jede Pruefung rc 0 - niemand verlangt die OK-Zeilen.
  Die Mutanten-Kampagnen haben main/Selbsttest ausdruecklich ausgenommen.
- H2: build_android.sh uebernimmt `APK_GATE_DATEI` aus der Umgebung (apk_pruefen.sh `${APK_GATE_DATEI:-...}`), ohne
  den Pfad anzuzeigen - ein fremdes/altes Gate prueft dann still.
- H3 (B1): B1 haengt am exakten Namen `${NAME}_android.z*`. SHA256SUMS.txt und git add nehmen weiter JEDE Datei
  `${NAME}_*.z*` (z.B. `${NAME}_ANDROID.zip`, `${NAME}_android_alt.z01`, `${NAME}_android.apk.zip`) ungeprueft mit,
  auch mit `--ohne-android`.
- H4: neue Gate-Mutanten im v2-Teil (`manifest_lesen`/`_pfad_fehler` standen in keiner systematischen Kampagne,
  nur G1-G22 von Hand): `if not z:` -> `if not z.strip():` (Zeile nur aus Leerraum), Steuerzeichen-Grenze
  `c < 0x1f`.
- H5 (Format v2): Faelschungen gegen C-Leser und Gate gemeinsam; dazu Unicode-Gross/klein (Kelvin-Zeichen U+212A,
  Ae/ae) und NFC/NFD - die Dublettenregel faltet nur ASCII, der App-Speicher (ext4/f2fs casefold) faltet Unicode.
- H6 (Entpacker): Unicode-Faltung auf dem Geraet (H5), Reste die nie entfernt werden (weg nur mit gueltiger alter
  Liste; `.neu` nur fuer Pfade der neuen Liste), `unlink(pf_liste)` ohne Rueckgabepruefung, nach Fehlern laeuft das
  Spiel nach 3 s Meldung mit altem/halbem Baum weiter (main.c:3075 hat keine Rueckgabe), SHA-256 gegen Python
  (Zufallslaengen/Stueckelungen, Datei > 4 GiB).

## 0.3 Laufprotokoll (fortlaufend)

- ⛔ Eigener Regelverstoss, sofort geprueft: beim Anlegen von 0.1 lief EINMAL `python3 --version` (nur Versionsabfrage).
  `type -a python3` zeigt danach: erster Treffer ist der WindowsApps-Alias
  (`AppData/Local/Microsoft/WindowsApps/python3` -> PythonManager 26.3), er gab "Python 3.10.11" aus (bestehende
  Installation). Schnappschuss mit dem R4-Werkzeug `py_zustand.ps1` direkt danach
  (`build/r34a/pruefer_u1/py_zustand_nach_python3.txt`, 121 Zeilen) == R4-Endstand `py_zustand_2_ende.txt`
  (`diff` rc 0): kein neuer PythonCore-Schluessel, kein Startmenue-Eintrag, `LocalAppData\Python` unveraendert
  (neuester Eintrag `_cache/last_welcome.txt` 2026-09-29T15:39:13), kein pymanager/msiexec-Prozess. Ab hier nur
  `/c/Python310/python`.

## 1. H1 - der Selbsttest (B4) urteilt nur ueber den Exit-Code des geprueften Codes

Werkzeug `u1_mutanten.py` (Kopien unter `build/r34a/pruefer_u1/mut/`, je Mutant `diff` = genau 1 geaenderte Zeile;
G0/G4 sind Kuerzungen), `u1_selbsttests.sh` (Selbsttest direkt, Rueckgabe selbst abgefangen). Beleg
`selbsttests_h1h4.txt`:

| Gate | Selbsttest EXIT | gedruckte Aussage |
|---|---|---|
| ECHT (sha256 8f84f136...) | 0 (25 s) | `SELBSTTEST-OK: 248/248` |
| G0 leere Datei (0 B) | **0** (0 s) | nichts - 0 Byte Ausgabe |
| G4 abgeschnitten vor dem `main()`-Block (Anweisungsgrenze, 184185 B) | **0** (0 s) | nichts |
| G1 `sys.exit(main())` -> `main()` | **0** (30 s) | `SELBSTTEST-FEHLER: 234 von 248 Faellen falsch` |
| G2 `RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2` -> `0, 0, 2` | **0** (40 s) | `SELBSTTEST-FEHLER: 165 von 248` |
| G3 `return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER` -> `return RC_GLEICH` | **0** (50 s) | `SELBSTTEST-FEHLER: 165 von 248` |

make_package.sh (`(( rc_selbst == 0 ))`) und apk_pruefen.sh (`(( rc == 0 ))`) werten NUR diese Exit-Codes aus. Dieselben
Gates liefern auch fuer `--quellbaum`, `--paket` und die APK-Pruefung rc 0 (G0/G4: es laeuft gar nichts; G1-G3: jede
Abweichung endet mit 0). Die ganze Kette mit diesen Gates: Abschnitt 2.
Warum die Kampagnen das nicht sahen: `mutanten_teil.py` (R2) "Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/
main)", G1-G22 (N1) nur `manifest_lesen`/`manifest_pruefen`/`_pfad_fehler`, MU1-MU8 (R3) Pruefcode.

## 3. SHA-256 von asset_abgleich.c (H6f) - haltbar

`u1_sha_zusatz.c` uebersetzt die AUSGELIEFERTE `asset_abgleich.c` unveraendert mit (gcc 16.2 msys64, `-Wall -Wextra
-Wpedantic -Wshadow -Wconversion`, ohne Warnung); `u1_sha_vergleich.py` rechnet dasselbe mit Python hashlib
(Beleg `sha_zusatz.txt`):
- 300 Zufallsnachrichten (Laengen 2..1046990 B, 136 unter 300 B, gezielt um 55/56/63/64 je Block, 94 >= 64 KiB), je
  Nachricht zufaellige Stueckelung 0..4097 B inkl. leerer Aufrufe: **0 falsch**;
- Strom 2^29-1 / 2^29 / 2^29+1 B (Bitlaenge ueber 2^32) und **4,5 GiB** (Bytezaehler ueber 2^32): gleich;
- `re15_sha256_datei` auf `RE2/CDEMD0.EMS` (11124736 B) und der Referenz-APK (363212403 B, = Archiv-sha 514bebd5...): gleich.
Kein Befund.

## 4. Format v2: eigene Faelschungen (H5) und neue Gate-Mutanten (H4)

`u1_liste_korpus.c` (asset_abgleich.c unveraendert) + `u1_korpus.py` (Gate `manifest_lesen` als Modul geladen) auf
DENSELBEN 57 Byte-Listen (Beleg `korpus_ergebnis.txt`): Summe falsch/leer/fehlt (v1-Zeile)/Grossbuchstaben (ganz und
einer)/Leerzeichen vorn+hinten/NBSP/Tab doppelt/Vollbreit/'g'; Zeilenenden CR allein, LF CR, CRLF, CR CR LF, VT, FF,
NEL, U+2028; Zeilen nur aus Leerzeichen/Tab/CR/NBSP; Kopf v1 + Zeilen v2, Kopf v2 + Zeilen v1, zwei Kopfzeilen, v1-Kopf
als 2. Zeile, Nullen vorn, `\r\r`, UTF-16; Pfade doppelt, ASCII-Gross/klein, 0x1f, C1-NEL, Leerzeichen/Punkt am Ende,
`.neu` in der Mitte/als Ordner/`.NEU`/Vollbreit, 512/513 B mit 2-Byte-Zeichen am Ende, Datei + gleichnamiger Ordner;
Groesse mit Nullen/Leerzeichen/Unterstrich; BOM, NUL.
- **Gate == Geraete-Leser auf allen 57 Listen** (0 Abweichungen). Jede Faelschung aus dem Auftrag (Summe falsch ->
  formal gueltig, den Rest faengt der APK-Abgleich `manifest_pruefen`; Summe fehlt/gross/Leerraum, Zeilenenden, v1/v2
  gemischt, Dubletten) wird von BEIDEN gleich behandelt.
- **Luecke der Dublettenregel (H5):** die 5 Paare, die sich nur in UNICODE-Gross/klein bzw. -Normalform unterscheiden
  (Kelvin-Zeichen U+212A gegen `K` und gegen `k`, `Ä`/`ä`, NFC-`é`/NFD-`e`+U+0301, `ß`/`ss`), nehmen Geraete-Leser UND
  Gate an - die Regel faltet nur ASCII (`asset_abgleich.c` `ascii_klein`, Gate `_ASCII_KLEIN`), begruendet wird sie mit
  dem case-insensitiven App-Speicher. Ob der Speicher diese Paare zusammenlegt: Emulator, Abschnitt 6.
- **Neue Ein-Zeilen-Mutanten im v2-Teil ueberleben den Selbsttest** (`selbsttests_h1h4.txt`):
  | Mutant | Selbsttest | weicht vom Geraet ab bei (Korpus) |
  |---|---|---|
  | V1 `manifest_lesen`: `if not z:` -> `if not z.strip():` | **SELBSTTEST-OK 248/248**, EXIT 0 | `zeile_nur_leerzeichen`, `zeile_nur_tab`: Gate nimmt an, Geraet verwirft die GANZE Liste (entpackt nichts) |
  | V2 `_pfad_fehler`: `c < 0x20` -> `c < 0x1f` | **SELBSTTEST-OK 248/248**, EXIT 0 | `pfad_steuer_1f`: Gate nimmt an, Geraet verwirft |
  Weder `_MANIFEST_PROBEN` noch die 248 Faelle enthalten eine Zeile nur aus Leerraum oder das Byte 0x1f im Pfad
  (der C-Unit-Test hat 0x1f, das Gate nicht). Ganze Kette mit V1 und einer signierten Faelschung: Abschnitt 2.

## 2. Die ganze Kette mit kaputtem Gate, fremdem Gate und Mutant V1

Eigene Sandbox `build/r34a/pruefer_u1/sb` (R4-Werkzeug `r4_sandbox_anlegen.sh`: Release-Skripte = Kopien des
Arbeitsbaums, per `cmp` geprueft; Quellbaum per Hardlink; eigenes git-Repo, Commit 2026-09-29 12:00). Nur
`<sandbox>/release/apk_asset_gate.py` wird getauscht (danach zurueck, `cmp` = Arbeitsbaum). Werkzeug `u1_kette.sh`,
Beleg `kette_A.txt` (gekuerzt; volle Logs `build/r34a/pruefer_u1/logs/kette/`).
APKs (Kopien unter `build/r34a/pruefer_u1/apk/`, release/ nur gelesen): N2 = `release/re15_port_v0.8.20-n1f_android.apk`
(N1-Endstand, Liste v2, c393a18e...); FW_sig = N2 + Listenzeile nur aus drei Leerzeichen am Ende (`r3_faelschen.py
--manifest-anhang`, `r3_signieren.sh`: zipalign + DERSELBE Debug-Schluessel, verify rc 0, Signer 432bc749...);
FD_sig = N2 + `RE15DOOR/P07G.DO2` mit 1 gekipptem Byte UND passender sha256 in der Liste (das Geraet entpackt das
klaglos: Groesse und Summe stimmen) - `faelschen_FD.txt`; FK_sig = N2 + Kelvin-Paar (Abschnitt 6) - `faelschen_FK.txt`.

### 2.1 build_android.sh --gate-only (= apk_pruefen.sh Schritte 0-6)

| Lauf | Gate | APK | EXIT | Ausgabe |
|---|---|---|---|---|
| A0 | echt | N2 | 0 | Selbsttest 248/248, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK` (Kontrolle) |
| A1 | echt | ref v0.8.19 (Liste v1) | 1 | `Manifest im alten Format v1 ... das Geraet lehnt es ab und entpackt NICHTS` (Kontrolle) |
| A6 | echt | FW_sig | 1 | Kontrolle: Leerraumzeile erkannt |
| **A2** | **G0 (0 Byte)** | ref v0.8.19 | **0** (9 s) | kein Selbsttest-Text, kein Gate-Text, dann `== ANDROID-GATES-OK (--gate-only)` |
| **A3** | **G1 (`main()` ohne `sys.exit`)** | ref v0.8.19 | **0** (43 s) | im selben Log `== SELBSTTEST-FEHLER: 234 von 248 Faellen falsch - das Gate ist NICHT verlaesslich ==` und danach `== ANDROID-GATES-OK` |
| **A7** | **V1 (`if not z.strip():`)** | FW_sig | **0** (33 s) | `SELBSTTEST-OK: 248/248`, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK` - das Geraet verwirft genau diese Liste (Korpus `zeile_nur_leerzeichen`: `Zeile 4: Groesse (1-18 Ziffern) + Tab erwartet`) und entpackt NICHTS |
| **A4** | echt in release/, aber Umgebung `APK_GATE_DATEI=<G0>` | ref v0.8.19 | **0** | wie A2 - build_android.sh uebernimmt die Variable (H2) |
| **A5** | echt in release/, Umgebung `APK_GATE_DATEI=<Gate be8b60f3>` | ref v0.8.19 | **0** | `SELBSTTEST-OK: 202/202`, `APK-ASSET-GATE-OK ... Manifest stimmt`, `ANDROID-GATES-OK` - das alte Gate nimmt die v1-Liste an; welches Gate lief, verraet nur die Fallzahl 202 statt 248 |

Keine Pruefkopie blieb liegen (`re15_apk_pruefen.*`: 0).

Nachtrag (`u1_kette_fd.sh`, Beleg `kette_A2.txt`) - die Faelschung, die auf dem Geraet wirklich schadet (FD_sig: falsches
Tuerarchiv, Liste passend, das Geraet entpackt es ohne Fehler):
| Lauf | Gate | APK | EXIT | Ausgabe |
|---|---|---|---|---|
| A8 | echt | FD_sig | 1 | `Inhalt weicht ab (sha256, gleiche Groesse 55908 B): assets/shared_assets/RE15DOOR/P07G.DO2 Quelle 68306346.., APK 0f75724b..` (Kontrolle) |
| **A9** | **G0 (0 Byte)** | FD_sig | **0** (13 s) | `ANDROID-GATES-OK` |
| **A10** | **G2 (`RC_ABWEICHUNG = 0`)** | FD_sig | **0** (48 s) | im Log `SELBSTTEST-FEHLER: 165 von 248`, dann `Inhalt weicht ab ... P07G.DO2` + `APK-ASSET-GATE-ABWEICHUNG: 1 Befunde` - und trotzdem `ANDROID-GATES-OK` |
| A11 | echt | FK_sig (Kelvin-Paar) | 1 | NUR `zusaetzlich in der APK (kein Asset-Baum liefert ihn)` fuer beide Dateien; die Liste selbst (3605 Zeilen, Kelvin-Paar) ohne Befund - siehe Abschnitt 6 |

**Befund H1 (hoch):** Der Selbsttest vor dem PC-Pfad (B4) und die ganze APK-Kette verlassen sich allein auf Exit-Codes,
die der geprufte Code selbst erzeugt. Eine 0-Byte-Gate-Datei (die Falle aus Memory reai-v2-patchskript-truncate: ein
Patch-Skript kuerzte 2026-09-07 zweimal Quelldateien auf 0 B, es fiel zwei Releases lang nicht auf; das Gate hat 6 Zeilen
mit Nicht-ASCII) oder eine Ein-Zeilen-Aenderung im Ausgangspfad laesst ein falsches Tuerarchiv, eine v1-Liste usw. mit
`ANDROID-GATES-OK` durch - auch dann, wenn dasselbe Log `SELBSTTEST-FEHLER` bzw. den Befund zeigt. Abhilfe-Richtung:
zusaetzlich zur 0 die Schlusszeile verlangen (`SELBSTTEST-OK: n/n` mit n >= bekannter Mindestzahl, `APK-ASSET-GATE-OK`,
`QUELLBAUM-OK`, `PAKET-OK`) und/oder die sha256 des Gates gegen einen versionierten Wert pruefen.

**Befund H2 (mittel):** `build_android.sh` uebernimmt `APK_GATE_DATEI` aus der Umgebung (apk_pruefen.sh Schritt 5:
`${APK_GATE_DATEI:-...}`; nur make_package.sh setzt es selbst). Weder build_android.sh noch apk_pruefen.sh zeigen den
Pfad oder die sha256 des benutzten Gates an. A4/A5: mit einem leeren bzw. dem alten Gate (be8b60f3) ist die v0.8.19-APK
mit Liste v1 `ANDROID-GATES-OK`. Die Variable ist nirgends als Bedienschalter dokumentiert (README nennt sie nicht).

**Befund V1/V2 (niedrig):** zwei Ein-Zeilen-Mutanten im v2-Teil bestehen den Selbsttest 248/248; V1 laesst mit einer
signierten Faelschung (Leerraumzeile) die ganze Kette gruen durch (A7), die das Geraet verwirft (nichts entpackt).

### 2.2 make_package.sh (PC-Pfad, B4) und fremd benannte Android-Dateien (B1)

Sandbox wie 2.1, `--version v0.8.19 --only linux`, Linux-Binary v0.8.19 aus dem Archiv (`u1_binaries.sh`: Split-Satz
`sha256sum -c` OK, mtime 2026-09-29 19:34, kein touch), keine APK. Beleg `kette_B.txt`.
| Lauf | Gate | Lage | EXIT | Ergebnis |
|---|---|---|---|---|
| B0 | echt | `--no-zip` (Paket anlegen) | 0 | Selbsttest 248/248, `APK-ASSET-GATE-PAKET-OK` |
| - | - | 1 Byte in der PAKET-Kopie von `PSX/DATA/TEX.TIM` (Linkzahl 1) | - | Paket 2daac755..., Quelle 7f10f1af... |
| B1 | echt | `--zip-only` | 1 | `Inhalt weicht ab: shared_assets/PSX/DATA/TEX.TIM` -> ABBRUCH (Kontrolle) |
| **B2** | **G0 (0 Byte)** | `--zip-only` | **0** (50 s) | nach "Selbsttest der privaten Kopie" kein Wort, `== Fertig ==`, `SHA256SUMS.txt` mit beiden Linux-Volumes, git-vorgemerkt (A x2); im ausgelieferten Satz (zip -s 0 + unzip -p) TEX.TIM = **2daac755...** (die veraenderte) |
| **B3** | **G1** | `--zip-only` | **0** (49 s) | im Log `SELBSTTEST-FEHLER: 234 von 248` und `Inhalt weicht ab: shared_assets/PSX/DATA/TEX.TIM ... APK-ASSET-GATE-PAKET-ABWEICHUNG`, dann `== Fertig ==`, SUMS + git add |
| - | echt | TEX.TIM zurueck; in release/ (Sandbox) `re15_port_v0.8.19_ANDROID.zip` und `re15_port_v0.8.19_android.apk.zip` (je 193 B: ein Zip mit einer "APK" aus Text) | - | - |
| **B4** | echt | ohne APK, `--zip-only` | **0** (55 s) | kein B1-Abbruch; `SHA256SUMS.txt`: `*re15_port_v0.8.19_ANDROID.zip *re15_port_v0.8.19_android.apk.zip` + die Linux-Volumes; git: **A** fuer beide |
| **B5** | echt | ohne APK, `--zip-only --ohne-android` | **0** (60 s) | Meldung `--ohne-android: keine APK-Pruefung, kein Android-Satz (keiner dieser Version vorhanden)` - und trotzdem beide Dateien in `SHA256SUMS.txt` und git-vorgemerkt |

**Befund H1 bestaetigt auf dem PC-Pfad:** B4 (Selbsttest der privaten Kopie) haelt weder ein leeres noch ein im
Ausgangspfad kaputtes Gate auf; das veraenderte Paket wird gezippt, in SHA256SUMS.txt geschrieben und vorgemerkt.
**Befund H3 (mittel):** B1 schuetzt nur den kanonischen Namen `${NAME}_android.z*` (Glob, gross/klein-genau).
SHA256SUMS.txt und git add nehmen weiterhin JEDE Datei `${NAME}_*.z*` ohne Pruefung mit - auch bei `--ohne-android`,
das ausdruecklich "kein Android-Satz" meldet. Die R4-Nachschaerfung "SHA256SUMS nur aus den in DIESEM Lauf erzeugten
Volumes" gilt nur fuer den kanonischen Android-Namen; fuer alles andere mit dem Versions-Praefix nicht. Abhilfe-Richtung:
SUMS/git add aus einer Positivliste der in diesem Lauf gezippten Saetze (plus bewusst die der anderen Plattform) statt
aus dem Glob.

### 2.3 Kelvin-Paar im Quellbaum (H5) - das ECHTE Gate laesst es durch

NTFS (Bau-Maschine) gemessen (Python, Wegwerf-Ordner): `K.bin`/`U+212A.bin`, `k.bin`/`U+212A.bin`, NFC-`é`/NFD-`é`,
`straße`/`strasse` liegen als ZWEI Dateien nebeneinander; nur `Ä`/`ä` und `A`/`a` faltet NTFS zusammen. Ein
Quellbaum auf der Bau-Maschine kann solche Paare also enthalten.
`u1_kette_fk.sh` (Beleg `kette_A12.txt`): beide Dateien (5 B, "AAAAA"/"BBBBB") als eigene Dateien in den
Sandbox-Quellbaum `shared_assets/PSX/` (Original unberuehrt, danach entfernt), FK_sig durch
`build_android.sh --gate-only` mit dem ECHTEN Gate: **EXIT 0** - `shared_assets/PSX 3195 3195 3195`, `Manifest: 3605
Zeilen ... gegen die APK geprueft`, `APK-ASSET-GATE-OK: 3605 Dateien ... Manifest stimmt`, `ANDROID-GATES-OK`.
Was das Geraet daraus macht: Abschnitt 6 (Emulator).

## 5. Geraete-Entpacker: Code gegen die Fragen des Auftrags (android_glue.c HEAD, asset_abgleich.c)

- **Ausserhalb des Speicherordners schreiben/loeschen:** nicht gefunden. Jede Zeile (neue UND "zuletzt entpackt"-Liste,
  also auch jedes `weg`-unlink) laeuft durch `re15_abgleich_pfad_ok`: kein fuehrendes `/`, kein leeres/`.`/`..`-Segment,
  kein `\`, keine Steuerzeichen, <= 512 B, mindestens ein `/` (Korpus + Unit-Test). Puffer: `pfad_bauen` = snprintf
  mit Laengenpruefung in `PATH_MAX` (4096) - `s_root` (<= 511) + `/` + 512 + `.neu` passt immer; `mkdirs_for` kopiert
  per snprintf in `PATH_MAX`. Symlinks: `open(<ziel>.neu, O_CREAT|O_TRUNC)` ohne `O_NOFOLLOW`, aber unmittelbar davor
  `unlink(<ziel>.neu)`; Zwischenordner folgen Symlinks - anlegen kann sie im externen Speicher (FUSE) niemand ohne root,
  im internen nur die App selbst (Messung ln -s: Abschnitt 6). Kein Befund.
- **Abbruch + Neustart:** Liste "zuletzt entpackt" wird VOR jeder Aenderung geloescht (`:415-417`), `.neu` + `fsync` +
  `rename` beim Schreiben (`liste_schreiben`), eine halbe `.neu`-Liste wird im naechsten aendernden Lauf entfernt (`:416`);
  ohne Liste prueft der naechste Start jede gleich grosse Datei per SHA-256, `.neu`-Reste je Listenpfad weg (`:455`).
  Eine halbe Liste kann nicht als gueltig gelesen werden (Kopfzeile nennt Anzahl + Bytes). Kein Befund zur Konsistenz.
  Randbefunde (niedrig):
  - U1 `unlink(pf_liste)` (`:415`) ohne Rueckgabepruefung: die tragende Invariante "ein aendernder Lauf hat die alte Liste
    zuerst entfernt" wird nicht erzwungen (scheitert das unlink, laeuft der Lauf weiter; mit einem spaeteren Fehler bliebe
    die ALTE Liste stehen). Auf dem Geraet nicht provozierbar gefunden - ein Verzeichnis gleichen Namens laesst auch das
    spaetere `rename` scheitern (laut, jeder Start PRUEFEN).
  - U2 Reste, die nie mehr entfernt werden: geloescht wird `weg` nur mit gueltiger alter Liste (`re15_abgleich_planen`
    alt != NULL). Nach einem Abbruch, von v0.8.19 kommend oder nach einem Abbruch VOR der weg-Schleife bleiben Dateien, die
    die neue Version nicht mehr hat, fuer immer liegen; `.neu`-Reste nur fuer Pfade der NEUEN Liste werden entfernt. Die
    Engine zaehlt keine Ordner auf (grep opendir/readdir/FindFirstFile: 0) - nur Platz. README "Dateien, die nicht mehr in
    der Liste stehen, werden geloescht" gilt nur fuer das Update mit gueltiger alter Liste.
  - U3 Segmentlaenge: die Liste erlaubt Segmente bis 512 B; ext4/f2fs erlauben 255 B je Name, `<name>.neu` braucht 4 mehr.
    Ein Segment von 252-255 B liesse sich nie entpacken (laut: Fehler bei jedem Start), > 255 B ebenso; Gate und Gradle
    lassen es durch (Messung Abschnitt 6).
- **Fehlerpfade (JNI ohne AssetManager, Liste fehlt/ungueltig/v1, Datei-Fehler):** Anzeige ja - `fehler_zeigen` bzw. die
  Fehlerzeile laufen 90 x 33 ms = ~3 s (`:321`, `:517`), dazu logcat + debug.log. **Fail closed nein:**
  `re15_android_bootstrap_assets()` hat keine Rueckgabe, main.c:3075 laeuft danach weiter (`re15_pc_asset_roots_report`
  meldet ohne Baum nur `WARNUNG: keine Asset-Wurzel gefunden - es wird nichts laden`). Bei einem Update startet das Spiel
  nach 3 s mit dem ALTEN Baum (bzw. bei Datei-Fehlern mit einem gemischten) - also genau das Symptom von N1a (alte Dateien),
  nur mit einem 3-s-Hinweis. Im gegateten Bau erreicht man diese Pfade nur ueber ein kaputtes Gate (H1/V1) oder ein volles
  Geraet. Befund U4 (niedrig; vorbestehend seit v0.8.19, durch N1 um die neuen Ablehnungen erweitert).
- **SHA-256:** haltbar (Abschnitt 3).
- **Geaenderte Datei wird NICHT neu entpackt:** der schnelle Weg prueft bewusst nur Groessen (dokumentiert); alle anderen
  Wege vergleichen Summen. Einzige gefundene Luecke: zwei Listenzeilen, die der App-Speicher zu EINER Datei faltet, ohne
  dass die ASCII-Dublettenregel greift (H5) - Messung Abschnitt 6.

### 2.4 B1-Varianten mit echter APK (`u1_kette_b1.sh`, Beleg `kette_B1var.txt`)

APK = Kopie des frischen v0.8.19-Baus des Pruefers ECHTER LAUF (`release/re15_port_v0.8.19_android.apk`, 09:54, Liste v2,
a340a325...; nur gelesen), Sandbox wie oben, `--zip-only --only linux`:
| Lauf | Lage | EXIT | Ergebnis |
|---|---|---|---|
| P1 | APK da | 0 (205 s) | APK-Kette 248/248, `APK im Split-Satz = gepruefte APK (entpackt: sha256 a340a325... = Pruefkopie)`, `SHA256SUMS.txt geschrieben (4 Volumes, Android-Satz aus diesem Lauf)`, 4 vorgemerkt; danach Release-Commit in der Sandbox |
| P2 | APK nach `APK-PRUEFUNG-OK` geloescht (Waechter, 10:27:09) | **1** (115 s) | `ABBRUCH: Android-APK verschwand zwischen Pruefung und Zippen`; SHA256SUMS.txt unveraendert (cf9a75ba...), Index leer - haelt |
| P3 | ohne APK, Satz versioniert | **1** (26 s) | B1-Abbruch vor den Kopierminuten; SUMS unveraendert, Index leer - haelt |
Zusammen mit R4 L1-L6/E1-E3 (nur PC, abgelehnte APK, `--ohne-android`, `--only win`) haelt B1 fuer den KANONISCHEN
Namen in allen Varianten; die Luecke ist der Name (H3, 2.2).

## 6. Emulator: der App-Speicher faltet Unicode - das Kelvin-Paar endet als EINE Datei, der schnelle Weg merkt es nie

`u1_emu.sh` (Beleg `emu.txt`, logcat `emu_fk_erststart.logcat`, `emu_fk_neustart.logcat`). Vorher `adb devices` leer und
kein qemu-Prozess (der Emulator des Pruefers ECHTER LAUF war beendet). Nutzer-AVD `Medium_Phone_API_36`, headless,
Android 16 / API 36 / x86_64, `/storage/emulated` = FUSE. `de.re15.port` war nicht installiert.

**1. Faltung (`u1_fold.sh`, in `/sdcard/Download` UND im App-Speicher `Android/data/de.re15.port/files`, gleiches Ergebnis):**
| Paar | Geraet |
|---|---|
| `A.bin` / `a.bin` | gefaltet (bekannt, N1 1.3) |
| `x.bin` / `y.bin` (Kontrolle) | getrennt |
| `K.bin` / `U+212A.bin` (Kelvin) | **gefaltet** (1 Eintrag) |
| `Ä.bin` / `ä.bin` | **gefaltet** |
| NFC `é.bin` / NFD `e`+U+0301`.bin` | **gefaltet** |
| `straße.bin` / `strasse.bin` | **gefaltet** |
| Name 255 B / 256 B / 252 B + `.neu` | angelegt / **zu lang** / **zu lang** (U3 bestaetigt) |
| `ln -s` im Speicher | abgelehnt (kein Symlink moeglich) |
(Im gemeinsamen Speicher lasen die zweiten Namen direkt danach teils noch den alten Inhalt - Cache; im App-Speicher
konsistent. Massgeblich ist die Zahl der Eintraege: 1.)

**2. FK_sig frisch installiert (Liste: `K.bin` = "AAAAA" sha 11770b3e..., `U+212A.bin` = "BBBBB" sha 0fad32b1...):**
- `Abgleich (ohne Liste): 3605 Dateien ... pruefen 3605` -> `Summe weicht ab -> neu: shared_assets/PSX/K.bin` ->
  `Entpacken fertig (ohne Liste): 3605 geprueft, 3605 kopiert ..., 0 Fehler`, Liste "zuletzt entpackt" geschrieben.
- Auf dem Geraet: in `shared_assets/PSX` EIN Eintrag `K.bin` (5 B); `cat K.bin` = `cat U+212A.bin` = **BBBBB**, sha256
  beider Namen 0fad32b1... - die Liste verspricht fuer `K.bin` 11770b3e... ("AAAAA").
- Neustart: `Assets aktuell (schneller Weg): Liste = zuletzt entpackt, 3605 Dateien, Groessen geprueft, 293 ms` -
  `K.bin` bleibt dauerhaft "BBBBB". **Kein Fehler, keine Meldung, falscher Inhalt.**
- Danach deinstalliert (`Android/data` ohne re15), Emulator beendet (`adb emu kill`, kein qemu mehr).

**Befund H5 (mittel):** Die Dublettenregel der Liste v2 (asset_abgleich.c `ascii_klein`, Gate `_ASCII_KLEIN`) faltet
nur ASCII, der App-Speicher faltet Unicode-Gross/klein, Normalform und sogar `ß`/`ss`. Ein Paar, das NTFS als zwei Dateien
haelt (gemessen: Kelvin/K, NFC/NFD, ß/ss), besteht Gradle, das ECHTE Gate (2.3: `ANDROID-GATES-OK`) und den
Geraete-Leser - und liefert auf dem Geraet still den falschen Inhalt, den der schnelle Weg nie mehr korrigiert (gleiche
Groesse). Heute ohne Folge: alle 3603 Pfade sind ASCII (Gradle-Kommentar "die Pfade sind ASCII", aber nirgends erzwungen).
Abhilfe-Richtung: Pfade auf druckbares ASCII (0x21-0x7e plus Leerzeichen) beschraenken - in Gradle, Gate und
`re15_abgleich_pfad_ok` - statt Unicode nachzufalten; dazu Segmentlaenge <= 251 B (U3).
