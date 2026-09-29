# Android-Asset-Gate - Nachbesserung Runde 1

Stand: 2026-09-29, Zweig r34a/android-gate, Baum .claude/worktrees/r34a_android.
Auftrag: Befunde der Gegenpruefer (pruefer_*_r1.md) je selbst pruefen, hoch/mittel beheben,
niedrig wo ohne Risiko; Selbsttest + Positiv-/Negativ-Kontrollen erneut fahren.

## 0. Laufprotokoll (fortlaufend)

- Dossier angelegt (erster Werkzeugaufruf).

- 21:50 Python-Schnappschuss 0 (`nachbesserung_r1_belege/py_zustand_0_vorher.txt`, 121 Zeilen,
  Werkzeug des Pruefers echtlauf) = dessen Endstand (`diff` leer).
- 21:50-21:54 alle Pruefer-Behauptungen selbst nachgemessen (Abschnitt 1) - alle bestaetigt.
- 21:55-22:20 Gate mit rohem ZIP-Leser + Selbsttest (59 Faelle), apk_pruefen.sh, build_android.sh,
  make_package.sh, python_finden.sh (Abschnitt 2); Selbsttest 59/59, Referenz rc 0, F2-F5 rc 1.
- 22:15-22:50 +11 Faelle (70), Mutanten-Probe Lauf 1 (76/84) -> +2 Faelle (72) -> Lauf 2 (78/84, 6 begruendet).
- 22:52-22:57 Gate direkt gegen Referenz + 19 Faelschungen (3.4), volle Kette --gate-only (3.5).
- 22:57-23:04 echte Android-Baeue: positiv EXIT 0, Gate-Abbruch, Gradle-Abbruch (3.6, 3.7).
- 23:04-23:11 make_package.sh echt: positiv, 6 Negativfaelle, B5 am alten Skript (3.8).
- 23:12-23:20 Linux-Docker (3.9), Python-Schnappschuss, Aufraeumen (3.10).

## 1. Nachpruefung der Befunde (vor jeder Aenderung, Gate-Stand HEAD 568e9b88)

Faelschungen mit dem Rohbyte-Werkzeug des Pruefers (`pruefer_umgehung_r1_belege/umgehung_werkzeug.py`,
/c/Python310) an Kopien von `build/r34a/ref_v0.8.19.apk` (sha256 514bebd5... = Archiv-SUMS), Ablage
`build/r34a/nb/apk/`. Belege: `nachbesserung_r1_belege/vorher_*.txt`.

| Befund | eigene Messung | Urteil |
|---|---|---|
| umgehung B1 (hoch) | F2 `\`, F3 NUL, F4 LFH-CRC, F5 LFH-Groesse: Gate rc 0 unter 3.10.11 UND 3.14.7 (`vorher_gate_HEAD.txt`); aapt2 35.0.0 (libziparchive): F2 `failed to find file.`, F3 `Invalid entry name`, F4/F5 `size/crc32 mismatch ... Inconsistent information` (`vorher_aapt2_libziparchive.txt`); F2 mit dem Debug-Schluessel neu signiert -> apksigner rc 0, Signer 432bc749... = Referenz, `build_android.sh --gate-only F2_signiert --version v0.8.19` -> EXIT=0 `ANDROID-GATES-OK` (`vorher_gate_only_HEAD.txt`) | bestaetigt |
| umgehung B2 (mittel) | apksigner verify: ref rc 0 (v2 true); K0 rc 1 `Missing META-INF/MANIFEST.MF`; F2/F4 rc 1 `CHUNKED_SHA256 digest mismatch` - dieselben APKs bestehen das Gate mit rc 0 (`vorher_apksigner.txt`) | bestaetigt |
| umgehung B3 (mittel) | Mutanten U1-U4 des Pruefers aus dem HEAD-Gate: je `SELBSTTEST-OK: 28/28`; U3 und U4 gegen F1 (CRC32-erhaltend in ENEMSE.VBS) rc 0, echtes Gate rc 1 (`vorher_mutanten_U1_U4.txt`) | bestaetigt |
| umgehung B4 (niedrig) | `ANDROID_SDK_ROOT=/c/gibt_es_nicht ... --gate-only ref --version v9.9.9` -> EXIT=0 mit `(aapt nicht gefunden - badging-Gate uebersprungen)` | bestaetigt |
| umgehung B5 (niedrig) | Code: Pruefung `make_package.sh:422-432` (`$APK_PRUEF`), Zippen `:591-596` (`$APK`, neu ermittelt), dazwischen copy_common/check_tree/check_runtime_assets beider Plattformen; ohne APK bei :423 keine Pruefung, APK bei :592 wird trotzdem gezippt. Keine Frischepruefung fuer die APK (check_binary_fresh nur :492/:524) | bestaetigt (Code) |
| echtlauf B1 (niedrig) | `bash release/python_finden.sh` -> `/c/msys64/mingw64/bin/python3 (3.14.7)`; PATH-Reihenfolge: `/c/Python310` steht an Stelle 12, `/c/msys64/mingw64/bin` an 43 - gewaehlt wird trotzdem MSYS2, weil erst ALLE `python3` und danach alle `python` kommen | bestaetigt |
| echtlauf B2 (niedrig) | `git log -1 -- engine platform include` = f2d26986 20:55:26 (build.gradle); ohne `platform/android` = cf386e32 19:02:06. PC-Bau haengt nicht an platform/android (`re15_port/CMakeLists.txt:92-101`: android/jni nur mit RE15_BUILD_ANDROID, der PC-Bau nur platform/pc) | bestaetigt |
| echtlauf B3 (niedrig) | `git check-ignore -v ...apk.ungeprueft` rc 1; `...android.apk` -> `.gitignore:63` | bestaetigt |
| echtlauf B4 (niedrig) | Code: `build_android.sh:323` `rm -f "$OUT" "$UNGEPRUEFT"` steht NACH Gradle (:311-315); make_package prueft weder versionName noch Codestand der APK | bestaetigt (Code) |
| Altlast | `HKCU\...\Uninstall\pymanager-pythoncore-3.14-64`: DisplayName Python 3.14.7, InstallDate 20260929, InstallLocation (Test-Path False), UninstallString startet den WindowsApps-PythonManager | bestaetigt |

## 2. Aenderungen (je Befund: Ursache -> Aenderung)

### 2.1 umgehung B1 (hoch): Gate liest die APK jetzt roh wie Android - `release/apk_asset_gate.py`
- **Ursache (Code, bestaetigt):** `pruefen()` las ausschliesslich ueber `zipfile`: Namen erst NACH dessen
  Normalisierung (NUL abgeschnitten, unter Windows `\` -> `/`), vom Local Header nur der Name verglichen.
  Der Geraete-Leser (android_glue.c:212 `SDL_RWFromFile` -> AAssetManager -> libziparchive) sieht die rohen
  Bytes und verlangt Local Header == Zentralverzeichnis.
- **Aenderung:** eigener ZIP-Leser, `zipfile` nur noch im Selbsttest als UNABHAENGIGER Schreiber.
  `zip_verzeichnis()`: EOCD = letzte Signatur von hinten, Kommentar reicht genau bis zum Dateiende, kein
  ZIP64/mehrteilig, Zentralverzeichnis im Bereich und ohne Rest (sonst rc 2 "APK nicht lesbar").
  `struktur_pruefen()` fuer JEDEN Eintrag (auch lib/, classes.dex): Name roh (kein NUL, gueltiges UTF-8,
  kein `\`, keine Steuerzeichen, kein `/`-Anfang/`.`/`..`/`//`), Local-Header-Signatur, Name im Local
  Header = Zentralverzeichnis (Laenge + Bytes), ohne Data-Descriptor-Bit CRC/csize/usize gleich, Daten
  ganz vor dem Zentralverzeichnis, nicht verschluesselt, Methode 0/8; Namen roh eindeutig.
  `_eintrag_lesen()`: Daten ab dem Offset aus dem LOCAL Header (wie libziparchive; die Referenz-APK hat bei
  1584 von 3616 Eintraegen LFH-Extra != CD-Extra = AGP-Ausrichtung), csize Bytes, entpackt (Deflate mit
  Ende-Pruefung), CRC32 + Laenge gegen das Zentralverzeichnis - fuer ALLE Eintraege; Assets zusaetzlich
  sha256 gegen den Quellbaum. Ergebnisse (Abschnitt 3): F2/F3/F4/F5 jetzt rc 1.

### 2.2 umgehung B2 (mittel): Signaturpruefung - neue `release/apk_pruefen.sh`
- **Ursache:** keine Signaturpruefung in der Kette; make_package.sh rief nur das Asset-Gate.
- **Aenderung:** EINE Pruefkette fuer beide Aufrufer (build_android.sh `run_gates`, make_package.sh):
  Stichproben, aapt badging (Paket, versionName, beide ABIs), `java -jar build-tools/35.0.0/lib/apksigner.jar
  verify -v --print-certs` (rc 0 UND "Verified using v2|v3 ... true"; Signer-Digest wird ausgegeben),
  Gate-Selbsttest, Gate. apksigner.jar direkt statt apksigner.bat (kein cmd.exe-Quoting).

### 2.3 umgehung B3 (mittel): Selbsttest 28 -> 72 Faelle + systematische Mutanten-Probe
- **Ursache (bestaetigt):** Fixture <= 5000 B (< BLOCK), keine CRC32-erhaltende Faelschung, kein Geister-/
  Doppelzeilen-Fall; die Mutanten-Probe des Bauers schaltete nur ganze Pruefungen ab.
- **Aenderung:** neue Faelle 29-72: gute APK mit Datei > 1 MiB (4 Dateien/1060709 B in der PSX-Zeile
  verlangt), 1 Byte hinter dem 1. MiB, CRC32-erhaltende Aenderung (GF(2)-Ausgleich, `_crc_erhaltend`),
  Geisterzeile, Doppelzeile, `1_800`, Manifest kein UTF-8 / `# Notiz` / nennt sich selbst / ohne Kopf;
  Eintragsname `\` (F2), NUL-Anhang (F3), kein UTF-8, Steuerzeichen, `..`; Local Header CRC (F4),
  Groesse (F5), Name, Signatur; verschluesselt; Methode 99; Daten ragen ins Zentralverzeichnis;
  classes.dex-Byte (nur CRC schuetzt es); Deflate mit Muell dahinter / Blocktyp 3; Verzeichniseintrag;
  abgeschnitten (F7), Bytes hinter dem EOCD, ZIP64, CD-Signatur (F8), CD-Offset, ein Eintrag zu wenig;
  gute APK mit EOCD-Signatur in Asset-Daten; neun build.gradle-Lesefehler; nach Probe-Lauf 1 zusaetzlich
  build.gradle-Kommentar `/*` ohne Ende (71) und Stored-Eintrag mit csize > usize (72, Leseschutz).
  Fixture wie AGP: nach dem Schreiben per zipfile bekommt jeder Local Header ein Ausrichtungsfeld 0xD935
  (6-9 B, CD-Extra bleibt 0) - ein Leser, der den Datenoffset aus dem Zentralverzeichnis rechnet, faellt
  schon im guten Fall durch. Faelschungen per eigenem Mini-Parser `_fx_cd` (unabhaengig vom Gate-Leser).
  Faelle laufen parallel (4 Prozesse): 72 Faelle in 4-12 s auf ruhiger Maschine (3.1).
- **Beweis:** `nachbesserung_r1_belege/mutanten_voll.py` erzeugt per `ast` JEDE Abschwaechung des
  Pruefcodes (A: jedes `befund(...)` -> `pass`, B: jedes `raise Bedienfehler/_Lesefehler` -> `pass`,
  C: jede Namensregel, D: 18 Hand-Mutanten inkl. U3/U4, Offset aus dem CD, Namen wie zipfile, EOCD von
  vorn) - Ergebnis Abschnitt 3.3.

### 2.4 umgehung B4 (niedrig): kein stilles Ueberspringen mehr
- `apk_werkzeuge_finden` bricht ab, wenn SDK, aapt, apksigner.jar oder Java fehlen (Meldung nennt den
  Weg fuer die reine Asset-Pruefung). `--gate-only` verlangt `--version` (rc 2 sonst). Im Bau werden die
  Werkzeuge VOR Gradle gesucht.

### 2.5 umgehung B5 (niedrig): make_package.sh zippt nur die gepruefte APK
- Vor den Kopierminuten: `check_binary_fresh` fuer die APK (Pfade engine, include, platform/pc,
  platform/android - der Android-Bau uebersetzt platform/pc/src/*.c, jni/CMakeLists.txt:42), dann
  `apk_pruefen` (Version = `--version`, Signatur, Assets), dann `APK_KENNUNG` = sha256/CRC32/Groesse in
  EINEM Lesedurchgang.
- Beim Zippen: APK ohne Kennung (erst spaeter abgelegt) -> Abbruch; Kennung jetzt != Kennung geprueft ->
  Abbruch; nach dem Zippen `verify_apk_im_zip`: Katalog des letzten Volumes nennt genau den einen Eintrag,
  CRC32 + Groesse = gepruefte Kennung. APK verschwunden -> Abbruch.

### 2.6 echtlauf B1 (niedrig): python_finden.sh - PATH-Reihenfolge je Ordner
- Je PATH-Ordner erst `python3`, dann `python` (leere Eintraege uebergangen). Auf dieser Maschine: `/c/Python310`
  (PATH-Stelle 12) statt MSYS2 3.14.7 (Stelle 43). Belege `python_finden_nachher.txt` T1-T9.

### 2.7 echtlauf B2 (niedrig): check_binary_fresh ohne platform/android fuer die PC-Binaries
- Standard-Pfade jetzt `engine include platform ':(exclude)re15_port/platform/android'`; die APK uebergibt
  ihre eigenen. Beleg: letzter Commit ohne android = cf386e32 19:02:06 < v0.8.19-Binaries 19:34.

### 2.8 echtlauf B3 (niedrig): .apk.ungeprueft bleibt nicht liegen
- `.gitignore` liegt ausserhalb meines Dateibereichs (nur release/*, build.gradle). Stattdessen loescht
  eine EXIT-Falle in build_android.sh die `.ungeprueft`-Kopie bei jedem Abbruch nach dem Kopieren; die
  identischen Bytes bleiben als Gradle-Ausgabe `app/build/outputs/apk/release/app-release.apk` (gitignoriert).

### 2.9 echtlauf B4 (niedrig): alte APK vor Gradle entfernen + Version/Frische in make_package
- `rm -f "$OUT" "$UNGEPRUEFT"` steht jetzt VOR Gradle; make_package prueft versionName (aapt) und Frische
  (siehe 2.5).

### 2.10 Altlast Uninstall-Eintrag: NICHT geaendert
- Registry des Nutzers liegt ausserhalb dieses Auftrags (nur release/*, build.gradle). Der Eintrag
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64` zeigt auf einen
  geloeschten Ordner; sein UninstallString STARTET den WindowsApps-PythonManager - nicht ausfuehren.
  Entfernen (Nutzerentscheidung): `reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64" /f`.

## 3. Messungen nachher

### 3.1 Selbsttest (72 Faelle)
`== SELBSTTEST-OK: 72/72 Faelle ==` unter Python 3.10.11, 3.14.7 (MSYS2) und 3.9.0 (`build/r34a/nb/selbsttest_*.log`).
Laufzeit 7,6 s (70 Faelle, ruhige Maschine) bis 47-54 s (72 Faelle, Maschine unter Last: selbst
`python -c pass` brauchte da 441-645 ms statt ~80 ms). Einzelne Faelle mit voller Gate-Ausgabe:
`nachbesserung_r1_belege/fall_zeigen.py <gate> <ordner> <nr> ...` (zeigt auch den rohen Eintragsnamen).

### 3.2 Kennung/Katalog-Sonde (make_package B5)
`kennung_sonde.sh` schneidet `apk_kennung` und `verify_apk_im_zip` per awk UNVERAENDERT aus make_package.sh
und faehrt sie gegen die Referenz-APK + das letzte Volume des Archiv-Android-Satzes v0.8.19:
Kennung sha256 `514bebd5...` (= Archiv-SUMS) / CRC32 `d16ad30a` / 363212403 B; Katalog passt -> rc 0;
falsche CRC -> rc 1 `NICHT die gepruefte Datei`; falscher Name -> rc 1 (`kennung_sonde_ergebnis.txt`).

### 3.3 Mutanten-Probe (B3): 84 Mutanten, 78 erkannt, 6 begruendet
`mutanten_voll.py release/apk_asset_gate.py <ordner> --parallel 2 --python /c/Python310/python.exe`
(Lauf 1 mit 70 Faellen `mutanten_voll_lauf1.txt`: 76/84; danach 2 Faelle ergaenzt; Lauf 2 mit 72 Faellen
`mutanten_voll_lauf2.txt`: **78/84**). Erzeugt aus dem Quelltext: A 34 x `befund(...)` -> `pass`, B 27 x
`raise` -> `pass`, C 5 Namensregeln, D 18 Hand-Mutanten. Ein Mutant gilt als erkannt, wenn sein
`--selbsttest` rc != 0 liefert; die roten Faelle stehen je Zeile im Lauf-Protokoll. Nur B__klammer_ende_Z189
wurde ueber eine Zeitgrenze erkannt (ohne den raise laeuft _klammer_ende endlos; Fall 71 -> "Lauf-Fehler
TimeoutExpired"); kein anderer Mutant-Log enthaelt "Lauf-Fehler" -> keine Erkennung durch Last.

Die Luecken der Gegenpruefung jetzt: U1 Geisterzeile = A_manifest_pruefen (Zeile ohne APK-Eintrag) -> Fall 32;
U2 Doppelzeile -> 33; U3 = D1 -> 29, 30 (und D16 "nur APK-Seite 1. Block" -> 29, 30); U4 = D2 -> 31;
U5 Verzeichnis -> 53; U6 Kommentarzeile -> 36; U7 `1_800` -> 34. U8 (64 MiB) siehe unten.
Weitere tragende: D3 Datenoffset aus dem Zentralverzeichnis -> 22 Faelle rot (auch die GUTE APK, dank
AGP-Polster); D4 Namen wie zipfile -> 39, 40; D5 Local Header nur Name -> 44, 45; D9 EOCD von vorn -> 61.

Ueberlebende (je Grund, im Code kommentiert):
| Mutant | Grund |
|---|---|
| B__ohne_kommentare "Kommentar /* ohne Ende" | unerreichbar: der Block kommt aus _klammer_ende, das jedes offene `/*` selbst meldet (Z189, erkannt) |
| B__anweisungen "Klammern passen nicht im stageAssets-Block" | unerreichbar: _klammer_ende hat den Block schon als ausgeglichen erkannt |
| B_quelldateien "Quellbaum nicht lesbar" | im Selbsttest nicht nachstellbar (unlesbarer Ordner); faellt er weg, meldet die Zusatz-Pruefung die APK-Dateien des uebersprungenen Ordners (rc 1) |
| B__eintrag_lesen "Datei endet mitten im Eintrag" | unerreichbar: struktur_pruefen verlangt daten_off + csize <= Zentralverzeichnis |
| A_manifest_pruefen "> 64 MiB" | ohne Fall: 64-MiB-Manifest je Selbsttest zu teuer; erreichbar nur mit > 1 KiB langen Pfaden (mehr als 65534 Eintraege lehnt das Gate als ZIP64 ab) |
| B_pruefen "interner Widerspruch" | Sicherheitsnetz fuer einen ZWEITEN Fehler (still uebersprungene Pruefung): in Lauf 2 erkennt es die Mutanten A_pruefen_Z706/Z713/Z724 ("fehlt"/"Groesse"/"Inhalt" geloescht) mit rc 2 statt 1 |

### 3.4 Gate direkt gegen die Referenz-APK und 19 Faelschungen (je 3.10.11 und 3.14.7)
`kontrollen_gate.sh` -> `nachher_gate_kontrollen.txt` (40 Laeufe, 22:52-22:55, je ~3,4 s). Faelschungen auf
rohen Bytes mit dem Werkzeug der Gegenpruefung (`umgehung_werkzeug.py`) bzw. `nb_faelschen.py` (nutzt dessen
Parser, nicht den des Gates); `_sig` = danach mit dem Release-Schluessel neu signiert (apksigner verify rc 0,
`signieren_faelschungen.txt`). Orakel = aapt2 35.0.0 (libziparchive, `vorher_/nachher_aapt2_libziparchive.txt`).

| APK | Gate vorher (HEAD 568e9b88; `vorher_gate_HEAD*.txt`) | Gate nachher | libziparchive (aapt2) |
|---|---|---|---|
| ref_v0.8.19 | 0 | **0** - 3603/3603 bytegleich, 3616 Eintraege lesbar | liest |
| K0 Umschrift (unsigniert) | 0 | 0 (Inhalt/Struktur richtig; Kette: apksigner, 3.5) | liest |
| F1 CRC32-erhaltend (ENEMSE.VBS), auch `_sig` | 1 | 1 `Inhalt weicht ab (sha256` | liest (falschen Inhalt) |
| F2 `\` im Namen, auch signiert | **0** | **1** `'\' im Namen` + `fehlt in der APK` | failed to find file |
| F3 NUL-Anhang | **0** | **1** `NUL-Byte im Namen` | Invalid entry name (ganze APK) |
| F4 LFH-CRC | **0** | **1** `CRC/Groessen im Local Header weichen ... ab` | Inconsistent information |
| F5 LFH-Groesse | **0** | **1** dito | Inconsistent information |
| F6 gross/klein, auch `_sig` | 1 | 1 fehlt/zusaetzlich/Manifest | failed to find file |
| F7 abgeschnitten | 2 | 2 `kein End-of-Central-Directory` | Invalid file |
| F8 CD-Signatur | 2 | 2 `Zentralverzeichnis kaputt bei Eintrag 2150` | missed a central dir sig |
| N1 Name nur im LFH anders (P1AP.DO2) | 1 (zipfile: 'File name in directory ... and header ... differ') | 1 `Name im Local Header weicht` | lfh name did not match |
| N2 Name kein UTF-8 (P24B.DO2), auch `_sig` | 1 (zipfile las den Namen als cp437: fehlt/zusaetzlich) | 1 `Eintragsname kein gueltiges UTF-8` | Invalid entry name (ganze APK) |
| N3 Verschluesselungsbit (P27S.DO2), auch `_sig` | 2 (RuntimeError aus zipfile) | 1 `verschluesselter Eintrag` | **liest** - hier ist das Gate strenger als das Geraet (AGP setzt das Bit nie) |
| N4 Methode 99 (DOOR36.DO2) | 2 (NotImplementedError aus zipfile) | 1 `Methode 99` | failed to open file |
| N5 EOCD nennt 3615 statt 3616 | **0** (zipfile sah 3615 Eintraege - weiterer Durchlass des alten Gates) | 2 `Rest oder Ueberlauf` | oeffnet, sieht aber `resources.arsc` (letzter CD-Eintrag) NICHT |

### 3.5 Volle Kette `build_android.sh --gate-only` (apk_pruefen.sh) - `nachher_kette_kontrollen.txt`
| Lauf | EXIT | Abbruch in |
|---|---|---|
| ref `--version v0.8.19` | **0** (12 s inkl. Selbsttest 72/72) | - (`ANDROID-GATES-OK`; apksigner v2 true, Signer 432bc749...) |
| ref `--version v9.9.9` | 1 | aapt: versionName |
| ref ohne `--version` | 2 | Bedienung: `--gate-only braucht --version` |
| B4: `ANDROID_SDK_ROOT=/c/gibt_es_nicht ... --version v9.9.9` | **1** (vorher 0) | APK-Pruefung: SDK-Ordner fehlt |
| K0 (unsigniert) | **1** (vorher 0 im Gate) | apksigner `DOES NOT VERIFY` |
| F4 (nach dem Signieren veraendert) | **1** (vorher 0) | apksigner |
| F2 signiert | **1** (vorher 0) | Asset-Gate: `'\' im Namen` + fehlt |
| F1 signiert | 1 | Asset-Gate: sha256 |
| F6 signiert | 1 | Asset-Gate: fehlt/zusaetzlich |
| N2 signiert (Name kein UTF-8) | 1 | aapt badging (libziparchive oeffnet die APK nicht) |
| N3 signiert (Verschluesselungsbit) | 1 | Asset-Gate |
Volle Logs `build/r34a/nb/kette/*.log`.

### 3.6 Voller Android-Bau mit der neuen Kette (positiv)
`bash release/build_android.sh --version v0.8.19 --no-toolchain` (22:57:48-23:00:29, **EXIT=0**, Log mit
Zeitstempeln `nachbesserung_r1_belege/android_voll.log`): Python **/c/Python310/python (3.10.11)** (echtlauf
B1 im echten Fluss), `APK-Werkzeuge: aapt + apksigner aus 35.0.0` VOR Gradle, stageAssets `RE2/DOOR: 27`,
`RE15DOOR: 30`, `BUILD SUCCESSFUL in 2m 25s`; Kette auf der `.ungeprueft`-Kopie: Stichproben (3604
Asset-Eintraege), aapt `versionName='v0.8.19'` + beide ABIs, apksigner `v2 ... true`, Signer 432bc749...,
Selbsttest 72/72, Gate 3616 Eintraege lesbar / 3603 Dateien bytegleich / RE2/DOOR 27/27 / RE15DOOR 30/30 /
TORSE.VBS gleich / Manifest 3603 Zeilen; dann mv + SHA256SUMS (`5ec3a0eb...`, 363212403 B), keine
`.ungeprueft` uebrig.
Laufzeit (aus den Zeitstempeln): gesamt 161 s, Gradle 146,7 s, Stichproben+aapt+apksigner 1,6 s,
Selbsttest 5,6 s, Gate 3,6 s, mv+sha256sum 1,7 s -> neue Kette 10,9 s = 6,8 % des Laufs.
APK gegen Referenz mit dem unabhaengigen Vergleich des Pruefers (`apk_vergleich.py`, zipfile, nicht der
Gate-Code): 3604/3604 assets-Eintraege sha256-gleich, Manifest `5f5acfdb...` beidseitig, alle Stored;
anders nur die vier .so (Baupfad im Binary, wie beim Pruefer) - `apk_vergleich_nachher.txt`, rc 0.

### 3.7 Echte Android-Baeue, die scheitern MUESSEN (echtlauf B3 + B4)
- **Gate scheitert** (`android_neg_lauf.sh`, 23:00:56-23:03:22): Punktdatei `RE15DOOR/.r34a_nb_probe.bin`
  (21 B) im Quellbaum + "alte" APK `release/re15_port_v0.8.19-nbneg_android.apk` (Kopie der Referenz,
  mtime 10:00). Beim Log-Eintrag `== Gradle:` (23:00:57) war die alte APK **schon entfernt**; Gradle
  `BUILD SUCCESSFUL in 2m 12s` (Manifest 3604 Dateien), Gate: `fehlt in der APK: ...probe.bin - Name beginnt
  mit '.'` + `Manifest nennt ... die APK hat keinen Eintrag` -> **EXIT=1**; danach weder `<name>.apk` noch
  `.ungeprueft` (Falle meldet `Gate-Abbruch: ... geloescht; dieselben Bytes liegen weiter unter
  .../app-release.apk`), `release/*.ungeprueft`: 0, SHA256SUMS_android.txt unveraendert, Probe entfernt,
  `git status re15_port/` leer (`android_neg_beobachtung.txt`, `android_neg.log`).
- **Gradle scheitert** (`android_gradle_fehler_lauf.sh`, genau der Fall der Gegenpruefung): alle 30
  RE15DOOR/*.DO2 kurz geparkt + alte APK `...-nbgradle_android.apk` -> `Task :app:stageAssets FAILED`,
  `Tuerarchive fehlen im Repo`, `BUILD FAILED in 13s`, **EXIT=1**; die alte APK **fehlt** danach (vorher blieb
  sie liegen und haette gezippt werden koennen). RE15DOOR zurueck: 30/30 sha256 gleich, `git status` leer.
- Nach den Baeuen: `release/SHA256SUMS_android.txt` per `git restore --source=HEAD` zurueck (cd139335...).

### 3.8 make_package.sh echt (isoliert: `mp_isoliert_nb.sh` lenkt nur git add/rm in Wegwerf-Index/-Objektspeicher)
Eingaben: PC-Binaries aus dem Archiv v0.8.19 (Split-Saetze kopiert, `sha256sum -c` 4 x OK, zusammengefuehrt,
entpackt; `re15_pc.exe` 30d5b67b..., `re15_pc` abfbe7c5... = Pruefer echtlauf) mit ihren **Original-
Zeitstempeln 19:34** (`cp -p`, KEIN touch); APK = der frische Bau aus 3.6 (5ec3a0eb...).
- **Positiv** (`mp_positiv.log`, 23:04:54-23:06:26, **EXIT=0**, `== Fertig ==`): Python 3.10.11; APK-Kette
  (Frische ok, aapt v0.8.19, apksigner v2, Selbsttest 72/72, Gate 3603/3603, 3616 lesbar); Kennung
  `5ec3a0eb... afc43f3a 363212403`; **Linux- und Windows-Binary ohne "VERALTET"** (echtlauf B2 behoben: der
  Pruefer musste dafuer noch `touch` benutzen); check_tree 27 + 30 Tuerarchive, LF-Gate, Laufzeit-Gate
  26/26 + 26/26; Zippen: linux 3800, win64 3801, android `= gepruefte Datei`, `APK im Split-Satz = gepruefte
  APK (CRC32 afc43f3a, 363212403 B)`; Git (Wegwerf-Index): 6 neue vorgemerkt, echter Index unberuehrt.
- **Negativ, vor den Kopierminuten** (`mp_negativ.sh`, `mp_negativ_ergebnis.txt`, Logs `mp_apk_*.log`):
  | Fall | EXIT | Abbruch |
  |---|---|---|
  | APK-mtime 20:00 (< f2d26986 20:55, letzter Commit an platform/android) | 1 (2 s) | `Android-APK ist VERALTET` |
  | `--version v0.8.20`, APK hat versionName v0.8.19 | 1 (2 s) | `aapt: versionName ist nicht 'v0.8.20'` |
  | K0 (unsigniert) als APK | 1 (3 s) | apksigner `DOES NOT VERIFY` |
  | F2 signiert als APK | 1 (13 s) | Asset-Gate `'\' im Namen` |
- **Negativ, WAEHREND der Kopierminuten (B5)** - APK wird getauscht, sobald das Log `Assets kopieren` zeigt:
  | Fall | EXIT | Abbruch |
  |---|---|---|
  | gegen F1 signiert getauscht (23:08:06, gueltige Signatur, gleiche Version) | 1 (62 s) | `Android-APK wurde nach der Pruefung veraendert oder ersetzt: geprueft 5ec3a0eb... afc43f3a 363212403 / jetzt 30d50681... 29b152c3 363469910 - Nichts gezippt` |
  | APK fehlte bei der Pruefung, taucht beim Kopieren auf (23:08:54) | 1 (47 s) | `ist erst NACH der Pruefung aufgetaucht - ungeprueft wird nichts gezippt` |
- **B5 am ALTEN make_package.sh gemessen** (Pruefer: "nur aus dem Code"; `mp_alt_tausch.sh`, das Skript aus
  568e9b88 kurz als release/make_package_alt.sh, Binaries per touch frisch): Pruefung der echten APK gruen,
  Tausch gegen F1 signiert um 23:10:25 -> **EXIT=0 `== Fertig ==`**, und der Katalog des gezippten
  Android-Satzes nennt CRC32 `29b152c3` / 363469910 B = die FAELSCHUNG (`verify_apk_im_zip` gegen die
  Faelschungs-Kennung rc 0, gegen die gepruefte rc 1) - `mp_alt_tausch_ergebnis.txt`. Befund bestaetigt,
  mit dem neuen Skript geschlossen (Zeile oben).
- Danach: `git restore --source=HEAD` fuer SHA256SUMS.txt + 6 Split-Volumes (`sha256sum -c` 7/7 OK gegen den
  Stand vor den Laeufen), pkg-linux/pkg-win/win_out/linux_out/APK/`release/__pycache__` geloescht,
  `git status --short release/ re15_port/` leer.

### 3.9 Linux (Docker `re15-linux-build:deb11`, Baum nur lesend unter /src) - `linux_lauf_nb.log`
Debian 11, Python 3.9.2: python_finden -> `/usr/bin/python3 (3.9.2)`; Selbsttest **72/72** (4 s); Gate ref
rc 0 (54 s - der bekannte 9p-Mount), F2 signiert rc 1, F3 rc 1, F4 rc 1, F5 rc 1, N5 rc 2. Der rohe Leser
haengt nicht mehr am Betriebssystem (unter Linux haette zipfile F2 ohnehin nicht normalisiert, F3-F5 aber
genauso durchgelassen).

### 3.10 Python-Installer-Kontrolle + Endstand
- Schnappschuss nach ALLEN Laeufen (`py_zustand_1_nach_allen_laeufen.txt`, 121 Zeilen) == Schnappschuss 0
  (`diff` leer): kein neuer Schluessel unter HKCU/HKLM `Software\Python`, kein Startmenue-Eintrag, keine
  Datei neuer als 21:45 in `%LOCALAPPDATA%\Python`, `...\WindowsApps`, Benutzer-Startmenue; kein
  python/pymanager/msiexec-Prozess. Kein Lauf hat den WindowsApps-Alias gestartet (python_finden waehlt jetzt
  C:/Python310; Selbsttests/Faelschungen/Proben mit /c/Python310, /c/Python39, /c/msys64/mingw64/bin/python3
  ausdruecklich).
- `git status --short release/ re15_port/`: leer. Geloescht (eigene Laufreste): Gradle-Ausgaben
  (`platform/android/{app/build,.gradle,build,local.properties}`, 1,5 GB), `build/r34a/nb/{apk,pakete,...}`
  (Faelschungen ~7 GB; mit `umgehung_werkzeug.py`/`nb_faelschen.py` in Sekunden wiederherstellbar).
  Liegen gelassen: `build/r34a/ref_v0.8.19.apk` (Bauer, sha256 514bebd5... unveraendert), Logs unter
  `build/r34a/nb/`.

## 4. Offen / Hinweise
- **Altlast Registry** (nicht geaendert, ausserhalb des Auftrags): `HKCU\Software\Microsoft\Windows\
  CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64` ("Python 3.14.7", Ordner existiert nicht). NICHT
  ueber "Deinstallieren" entfernen (startet den WindowsApps-PythonManager); Vorschlag an den Nutzer:
  `reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64" /f`.
- `.gitignore` (`release/*.apk.ungeprueft`) liegt ausserhalb meines Dateibereichs; die EXIT-Falle macht es
  unnoetig (3.7). Wer mag, ergaenzt es beim naechsten Release-Commit.
- Das Gate ist bei gesetztem Verschluesselungsbit STRENGER als das Geraet (libziparchive liest N3); AGP setzt
  das Bit nie, der Abbruch waere also nur bei einer Fremd-Nachbearbeitung zu erwarten.
- `check_binary_fresh` misst weiter die Datei-mtime gegen die Commit-Zeit: eine per `cp` (ohne -p) frisch
  kopierte ALTE APK gilt als frisch - dieselbe Grenze wie bei den PC-Binaries; versionName, Signatur und
  der Inhaltsabgleich greifen trotzdem.
- Laufzeit der Kette im Bau: 10,9 s von 161 s (3.6); der Selbsttest (72 Faelle, 4 parallel) braucht auf
  ruhiger Maschine 4-8 s, unter Last bis ~55 s (gemessen, 3.1).

---

# Runde 2 — Nachbesserung nach Gegenpruefung r2 (Start 2026-09-30)

Auftrag: jeden Befund B1..B9, N1 und die zwei Echtlauf-Befunde erst selbst pruefen,
dann beheben (hoch/mittel Pflicht, niedrig wo ohne Risiko), Selbsttest + Positiv-/Negativ-
Kontrollen erneut fahren, committen. Dieses Kapitel wird laufend fortgeschrieben.

Stand: angelegt, noch nichts geprueft.

## R2-0. Laufprotokoll (fortlaufend)

- 00:40 Dossier-Kapitel angelegt (erster Werkzeugaufruf); Bestand + Pruefer-Dossiers r2 gelesen.
- 00:44 Python-Schnappschuss 0 (`nachbesserung_r2_belege/py_zustand_0_vorher.txt`, 121 Zeilen) = Endstand des
  Pruefers umgehung r2 (`Compare-Object` leer), kein python/pymanager/msiexec-Prozess.
- 00:45-01:05 alle Pruefer-Behauptungen am UNVERAENDERTEN Werkzeugstand (HEAD e1640cd0 = 35d25455) selbst
  nachgemessen (R2-1). Arbeitsordner `build/r34a/nb2/` (Referenz-APK-Kopie `build/r34a/ref_v0.8.19.apk`,
  sha256 514bebd5... = Archiv-SUMS; Faelschungen mit dem Rohbyte-Werkzeug des Pruefers `r2_faelschen.py`).
- 00:58 Grundlinie der systematischen Teil-Mutanten-Probe am HEAD-Gate gestartet (R2-3.1).

## R2-1. Nachpruefung der Befunde (vor jeder Aenderung)

| Befund | eigene Messung (Belege `nachbesserung_r2_belege/vorher_*.txt`) | Urteil |
|---|---|---|
| B1 (mittel) | 5 Mutanten per `r2_mutanten.py`: je `SELBSTTEST-OK: 72/72` (15-53 s je Lauf, Maschine unter Last); jeder Mutant gegen seine Faelschung an der echten APK (mit `--repo`): M1/M9/M5/M14/M16 je **rc 0**, echtes Gate rc 1/1/2/1/1; F_M1 mit dem Debug-Schluessel signiert: apksigner Signer 432bc749... (= Referenz), Mutant M1 rc 0, echtes Gate rc 1 `Manifest-Groesse falsch ... 78243 B, APK 78244 B`. (Lauf 1 meines Skripts rief die Mutanten ohne `--repo` -> rc 2 "build.gradle fehlt" - mein Fehler, korrigiert in `vorher_b1_mutanten_gegen_faelschungen.sh`.) | bestaetigt |
| B2 (mittel) | Schattenbaum (PSX/synchro/RE2/fx Hardlinks, RE15DOOR Kopie; Kontrolle Schatten gegen Referenz rc 0): P2DS in Quelle UND APK weg -> rc 0 `RE15DOOR: Quelle 29, APK 29, sha256 gleich 29/29`; P07G in beiden 0 B -> rc 0 `30/30` | bestaetigt |
| B3 (niedrig) | echte `build_android.sh --gate-only`, Tausch gegen K0 (unsigniert) waehrend des Selbsttests: `Verified using v2 ... true`, `ANDROID-GATES-OK`, EXIT=0; die Datei unter dem Namen danach: apksigner `DOES NOT VERIFY` | bestaetigt |
| B4 (niedrig) | frischer Schluessel (keytool), K0 damit signiert: Kette EXIT=0, Signer 85ad070a... statt 432bc749...; Code build.gradle:210-218 (`if (ks && new File(ks).exists()) ... else initWith debug`) | bestaetigt |
| B5 (niedrig) | K0 ohne Ausrichtungs-Extras, `apksigner sign --alignment-preserved true`: verify rc 0; `zipalign -c -P 16 4` rc 1, 2327 BAD (libmain.so arm64 BAD - 3545, resources.arsc BAD - 1); Kette EXIT=0; Referenz zipalign rc 0 | bestaetigt |
| B6 (niedrig) | Schatten mit `re15_port/shared_assets/RE15NEU/NEU.DAT` (keine Liste), APK = Referenz -> Gate rc 0 | bestaetigt |
| B7 (niedrig) | Sonde des Pruefers = python_finden.sh bis auf die 2 Startzeilen (`diff`: 4 Zeilen); (a) PATH nur 8.3-WindowsApps + NUR_PATH -> `SONDE: WUERDE STARTEN (ohne timeout): .../WINDOW~1/python3` (und python); (d) `RE15_PYTHON=<8.3>/python3.exe`, PATH leer -> WUERDE STARTEN; (b) mit /usr/bin -> `verworfen (Link auf WindowsApps ...)`. Nichts gestartet. | bestaetigt |
| B8 (niedrig) | F_praefix (16 B, Offsets verschoben): Gate rc 0, aapt2 `Entry at offset zero has invalid LFH signature 0 ... failed` rc 1; Kopfzeile mit arabisch-indischen Ziffern: Gate rc 0 | bestaetigt |
| B9 (niedrig) | `frische_stumm_sonde.sh` (Funktion per awk unveraendert) im Mini-Repo: Ausgabe nur `vor check_binary_fresh`, EXIT=1 | bestaetigt |
| N1 (ausserhalb) | Code gelesen: android_glue.c:210 nur Groesse; dazu STAERKER als gemeldet: der Marker (:161-181) traegt FNV-1a64 der Liste - bleibt die Liste gleich (gleiche Groessen, gleiche Pfade), wird gar nichts geprueft. SDL_rwops.c:537-557 (SDL2 2.28.5 in `_deps`): relativer Pfad -> zuerst `<intern>/<pfad>` per fopen, erst dann Assets - bestaetigt; `git log`: P07G.DO2 b22088b5/1aaa98c7/d094cea1 je 55908 B (alle vor v0.8.19, v0.8.18 hatte noch kein RE15DOOR -> ausgeliefert wurde der Fall noch nicht) | bestaetigt (Code) |
| echtlauf B1 (niedrig) | `type -a python3`: zuerst `.../WindowsApps/python3`; Kopf `#!/usr/bin/env python3`, Kopf-Doku zeigt Direktaufrufe | bestaetigt (nicht ausgefuehrt) |
| echtlauf B2 (niedrig) | make_package.sh:16-23 nennt weder APK noch SDK/JDK/Python | bestaetigt |

## R2-2. Aenderungen (je Befund: Ursache -> Aenderung)

### B1 (mittel) Teil-Abschwaechungen - Selbsttest 72 -> 140 Faelle + systematische Teil-Mutanten-Probe
- **Ursache:** die Faelle trafen jede Pruefung nur von EINER Seite (Manifest nur +1/9999, EOCD nur Muell
  dahinter, Laenge nie ohne CRC-Fehler, DD-Bit nie, alle Nicht-Assets Stored); die Probe der Runde 1
  erzeugte nur Ganz-Abschaltungen (befund/raise -> pass).
- **Aenderung Gate:** M5 eigene Meldung fuer zu langen Kommentar (`rest < 0`); M14: das Data-Descriptor-Bit
  (LFH ODER CD) wird abgelehnt - AGP setzt es nie (Referenz 0 von 3616 in LFH und CD, gemessen), der
  LFH/CD-Vergleich ist damit unbedingt und die Frage "aus welchem Kopf" entfaellt.
- **Aenderung Selbsttest:** je Teil-Mutant der Gegenpruefung ein Fall, der NUR die geschwaechte Seite trifft
  (M1 Manifest 1 B kleiner; M9 Laenge +100 bei richtiger CRC fuer classes.dex/Deflate UND libmain.so/Stored;
  M5 Kommentar 1 B zu lang; M14 DD nur CD/nur LFH/beide; M16 Deflate-dex und Stored-.so gekippt - die Fixture
  schreibt Manifest/dex jetzt wie AGP komprimiert). Grenzfaelle: genau 1 B hinter dem EOCD, Kommentar genau
  1 B zu lang, Manifest genau an bzw. 1 B ueber der Grenze (Pruefhaken `RE15_GATE_MANIFEST_MAX`, der die
  64 MiB nur SENKEN kann), EOCD disk/cd_disk/n_hier, ZIP64-Locator bei normalen Zaehlern, CD endet mitten im
  Kopf. Dazu die Faelle der uebrigen Befunde (unten). Werkzeug `mutanten_teil.py` erzeugt aus dem Quelltext
  per ast JEDE Teil-Abschwaechung (Vergleichsoperator in eine Richtung / Grenze um eins, je ein Operand
  eines and/or weg, je ein Paar eines Tupelvergleichs weg, not weg, Zahl +-1) plus A/B/C der Runde 1 und
  35 Hand-Mutanten (`hand_r2.py`: die der Runde 1 und M14/M16 in der neuen Form, Verschluesselungsbit nur
  eine Seite, Paket nur Groesse, FNV nur 1. Block, Griff-Rueckfall/fuer_eigen, Basis, Wurzel, `\d`, Praefix).
  Ergebnis R2-3.

### B2 (mittel) Tuer-Soll aus den Engine-Tabellen - Gate, make_package, Selbsttest
- **Ursache (bestaetigt):** das Gate verglich nur APK gegen Quellbaum; die Soll-Liste (`re15_tuer_eigen[30]`
  mit Groesse + FNV-1a) nutzte niemand ausser der Engine und dem ctest-Probe `unit_r33_tueren_archive`, den
  die Release-Kette nicht faehrt.
- **Aenderung:** `tuer_soll()`/`tueren_pruefen()` lesen die Tabellen, nach denen `door_scene_pc.c` laedt
  (`re2_archiv_lesen` :219-251): Spalten aus den typedefs in `include/re15_door_seq.h` (C-Initialisierer-Leser
  mit C-Regeln: fehlende Felder am Zeilenende = 0 - die Runde-31-Zeilen lassen `eigen` bzw. `spender_eigen`
  weg; unbekannte Form -> rc 2). RE15DOOR: jede Zeile von `re15_tuer_eigen` als `<kennung>.DO2`, Groesse =
  datei, Sektor*0x800+Modellteil = Groesse, Tonteil <= Sektor*0x800 (dieselben Bedingungen wie :242-243),
  FNV-1a = Tabelle; RE2/DOOR: jedes Archiv, das eine Tuerzeile mit eigen=0 (:322), ein Griff-Tausch mit
  spender_eigen=0 (:347-350, erster Treffer, bei eigen=0 Rueckfall in die Port-Tabelle wie
  door_seq_zuordnung.c:165-171) oder ein Port-Archiv als Basis nennt, mit Groesse/Aufbau aus
  `gen/re2_tuer_tabelle.inc` (@0x8009a520); keine Datei ausserhalb dieser Listen; Tabellenfehler (Zeile nennt
  Port-Archiv > N, Basis passt nicht zur Zeile - die Engine gibt dann -1 -, RE2-Nummer ausserhalb) als Befund.
  Echter Baum: **RE15DOOR 30/30, RE2/DOOR 27/27** (25 aus Tuerzeilen/Spendern + DOOR0C/DOOR14 nur als Basis
  von P0CD/P14A - gemessen mit `scratchpad/tuer_mengen.py`, deckt sich mit dem Ordner exakt).
  make_package.sh prueft den Quellbaum damit VOR den Kopierminuten (`--quellbaum`) und jedes Paket danach
  (`--paket`, siehe B6); das 0-Byte-Archiv faengt jetzt auch die APK-Seite.

### B3 (niedrig) Pruefkopie statt Pfad
- **Aenderung apk_pruefen.sh:** Schritt 0 kopiert die APK EINMAL in einen privaten Temp-Ordner (Kennung
  sha256/CRC32/Groesse im selben Lesedurchgang); aapt, zipalign, apksigner, Selbsttest-Gate und Gate lesen NUR
  die Kopie; am Ende muss die Kopie unveraendert sein UND der Pfad dieselbe Kennung tragen, sonst Abbruch.
  Ergebnis `APK_GEPRUEFT_KENNUNG`/`APK_GEPRUEFT_KOPIE`. build_android.sh legt genau die gepruefte Kopie per mv
  unter dem Auslieferungsnamen ab (und vergleicht die sha256 der SUMS-Zeile); make_package.sh nimmt die Kennung
  der Kopie (nicht mehr die vom Pfad nach der Pruefung). Hin-und-zurueck-Tausch (ABA) aendert nichts: geprueft
  wurde die Kopie, und am Ende liegen dieselben Bytes unter dem Pfad. EXIT-Fallen raeumen die Kopie ab.

### B4 (niedrig) Signer festgehalten
- `release/apk_signer.sha256` (432bc749..., Referenz v0.8.19 = Debug-Schluessel der Bau-Maschine),
  Uebersteuerung `RE15_APK_SIGNER_SHA256`; apk_pruefen verlangt genau einen Signer mit diesem Zertifikat.
  build.gradle: `RE15_KEYSTORE` gesetzt, Datei fehlt -> GradleException statt stillem Debug-Schluessel.

### B5 (niedrig) Ausrichtung
- apk_pruefen Schritt 3: `zipalign -c -P 16 4` aus build-tools 35.0.0 (Referenz rc 0); bei Fehler die BAD-Zeilen.

### B6 (niedrig) dritte Liste + Wurzel
- Gate `--paket <ordner>`: der fertige PC-Paketordner gegen DIESELBE Liste wie die APK (jede Datei der Baeume
  mit Groesse + sha256, unter shared_assets/ und synchro/ nichts sonst), aus make_package.sh check_tree fuer
  beide Pakete. `wurzel_pruefen`: unter re15_port/shared_assets nur die Baeume der Liste (der
  RE15NEU-Schatten der Gegenpruefung waere jetzt rc 1).

### B7 (niedrig) python_finden.sh
- Langname per `cygpath -m -l` gegen */windowsapps/* (faengt 8.3); ein Link, dessen Ziel sich ohne readlink
  nicht pruefen laesst, wird verworfen statt gestartet; Link mit leerem Ziel ebenso. Kopf korrigiert.

### B8 (niedrig) libziparchive-Naehe
- Datei muss mit einem Local Header beginnen (rc 2 sonst); KOPF_RE nur `[0-9]`.

### B9 (niedrig) check_binary_fresh
- die drei Vorbedingungen brechen weiter ab (fail closed), jetzt mit `ABBRUCH: Frische-Gate (...)`-Meldung.

### echtlauf B1/B2 (niedrig)
- apk_asset_gate.py und zip_exec_bit.py: Kopf laeuft beim Direktaufruf als Bash (`#!/bin/bash` + `''':'`-
  Zeichenkette fuer Python) und startet den Interpreter aus python_finden.sh; `release/.gitattributes` haelt
  release/*.py auf LF (core.autocrlf=true zog zip_exec_bit.py im Arbeitsbaum schon auf CRLF - gemessen
  `git ls-files --eol`: `i/lf w/crlf`). `__doc__` explizit (zip_exec_bit.py druckt ihn als Hilfe).
- make_package.sh-Kopf nennt APK-Eingabe und alle Voraussetzungen; `--help` druckt den ganzen Kopf.

### N1 (mittel, ausserhalb) - NICHT geaendert
- Liegt in `re15_port/platform/android/jni/android_glue.c` (Geraete-Code) - ausserhalb meines Dateibereichs
  (nur release/*, build.gradle). Befund bestaetigt und verschaerft (R2-1); Vorschlag in R2-5.

## R2-5. N1 (ausserhalb meines Dateibereichs): Befund und Vorschlag

**Bestaetigt, und schaerfer als gemeldet** (Code; SDL2 2.28.5 aus `re15_port/platform/android/_deps`):
1. `android_glue.c:161-181`: der Marker `re15_assets_ok.txt` traegt FNV-1a64 des Manifests + Anzahl + Bytes.
   Das Manifest nennt nur Groesse und Pfad je Datei. Aendert ein Update nur INHALTE bei gleicher Groesse
   (P07G.DO2: b22088b5 -> 1aaa98c7 -> d094cea1, je 55908 B; fuenf Port-Archive haben 55908 B), ist das Manifest
   bytegleich -> Marker gleich -> `Assets bereits entpackt`, return: es wird **gar nichts** geprueft.
2. Selbst bei anderem Manifest entpackt :210 nur Dateien anderer Groesse - die gleich grosse, geaenderte Datei
   bleibt alt.
3. `SDL_rwops.c:537-557`: ein relativer Pfad wird ZUERST als `<intern>/<pfad>` per fopen geoeffnet, erst dann im
   Asset-System. Liegt der Anker intern (:54-55, nur ohne beschreibbaren externen Speicher), oeffnet :212 fuer eine
   Datei anderer Groesse die ALTE entpackte Datei, :213 kuerzt genau sie auf 0 -> 0 B gelesen -> `got != sz` ->
   Fehler, kein Marker, bei jedem Start erneut.
- Ausgeliefert wurde Fall 1/2 noch nicht: v0.8.18 hatte kein RE15DOOR (`git ls-tree v0.8.18`), alle drei
  P07G-Staende liegen vor v0.8.19. Er trifft das NAECHSTE Update, das eine Textur/ein Archiv gleich gross aendert.
- Das Gate kann das nicht fangen (die APK ist richtig), und aus build.gradle heraus laesst es sich nicht beheben:
  das Geraet vergleicht nur Groesse und Pfad; eine dritte Manifestspalte laese der jetzige Leser als Teil des
  Pfads (`rel = tab + 1`).
- **Vorschlag (Geraete-Code, braucht eine Entscheidung - nicht mein Bereich):** (a) writeAssetManifest schreibt je
  Zeile zusaetzlich eine Pruefsumme (`<bytes>\t<fnv64>\t<pfad>` o.ae.), android_glue.c liest sie und entpackt, wenn
  Groesse ODER Pruefsumme von einer lokal abgelegten Liste abweichen (der Marker allein reicht dann, weil sich das
  Manifest mit jedem Inhalt aendert); (b) Assets nicht ueber einen Pfad oeffnen, den SDL zuerst im internen
  Speicher sucht (z.B. Anker `<intern>/re15/` statt `<intern>/`, mit Umzug von re15_card.mcr), oder in
  `<ziel>.neu` schreiben und erst nach vollstaendigem Lesen umbenennen. Das Gate zieht (a) dann nach (Spalte
  pruefen).

## R2-3. Messungen nachher (Maschine die ganze Zeit unter Last: Granaten-Sitzung + Mutanten-Probe parallel)

### R2-3.2 Gate direkt gegen die Faelschungen der Gegenpruefung (unsigniert; `nachher_gate_*.log`)
| Faelschung | vorher (HEAD) | nachher |
|---|---|---|
| F_praefix (16 B vor dem 1. Local Header) | 0 | **2** `beginnt nicht mit einem Local Header ... 'Entry at offset zero has invalid LFH signature'` |
| F_kopf_arabisch | 0 | **1** `Manifest-Kopfzeile fehlt/unlesbar: '# re15 assets \u0663\u0666...'` |
| F_M1 / F_M9 / F_M14 / F_M16 | 1 | 1 (Manifest-Groesse / Laenge `gelesen 85672 B ... 85772 B` / `Data-Descriptor-Bit gesetzt (nur Zentralverzeichnis)` / CRC classes.dex) |
| F_M5 (Kommentar +10) | 2 | 2 `Kommentarlaenge 10 ... reicht 10 B ueber das Dateiende` |
Gegen diese Faelschungen hielt das echte Gate schon vorher; B1 betrifft die MUTANTEN - siehe R2-3.1.

### R2-3.3 Volle Kette `build_android.sh --gate-only` (neue Skripte) - `nachher_kette.txt`
| Lauf | EXIT | Abbruch in (vorher, R2-1) |
|---|---|---|
| Referenz v0.8.19 | **0** | - (Kopie 514bebd5... = Referenz, zipalign ok, Signer 432bc749..., Selbsttest 140/140, Gate 3603/3603, RE15DOOR/RE2 30/30 27/27) |
| K0 unsigniert | 1 | apksigner `Signatur ungueltig` |
| K0 fremder Schluessel (B4) | **1** (vorher 0) | `Signer-Zertifikat 85ad070a..., erwartet 432bc749...` |
| K0 unausgerichtet, signiert (B5) | **1** (vorher 0) | `zipalign: Ausrichtung falsch` |
| F_M1 signiert | 1 | Asset-Gate (Manifest-Groesse) |
| F_kopf_arabisch signiert | **1** (vorher 0) | Asset-Gate (Kopfzeile) |
| F_praefix "signiert" | 0 | kein Befund: `zipalign -f` schreibt das Archiv neu und laesst das Praefix weg - eine signierte Praefix-APK entsteht mit den SDK-Werkzeugen gar nicht; das Gate lehnt die unsignierte ab (R2-3.2) |
| B3 Tausch gegen K0 waehrend des Selbsttests | **1** (vorher 0) | `APK wurde WAEHREND der Pruefung veraendert oder ersetzt ... geprueft (Kopie vom Anfang): 514bebd5... jetzt unter dem Pfad: 2110be...` |
| B3 Gegenprobe ABA (hin zu K0, vor Schritt 6 zurueck) | 0 | korrekt: geprueft wurde die Kopie (= Referenz), und am Ende liegt dieselbe Datei unter dem Pfad |
| Schatten ohne P2DS in Quelle UND APK, signiert (B2) | **1** (vorher 0) | `Port-Tuerarchiv fehlt: re15_port/shared_assets/RE15DOOR/P2DS.DO2 (verlangt von: .../re15_tuer_eigen.inc ...)`, `RE15DOOR: Quelle 29, APK 29` |
| Schatten P07G 0 B in Quelle UND APK, signiert (B2) | **1** (vorher 0) | `... P07G.DO2 hat 0 B, Tabelle 55908 B (Tonteil 19608, Modellteil 35428 ab Sektor 10) - die Engine verwirft es` |
| Schatten mit shared_assets/RE15NEU (B6) | **1** (vorher 0) | `re15_port/shared_assets/RE15NEU liegt in keinem Asset-Baum ...` |
Keine Pruefkopie blieb in /tmp liegen (auch nicht nach den Abbruechen).

### R2-3.4 Voller Android-Bau mit der neuen Kette (positiv) - `logs/android_voll.log`
`bash release/build_android.sh --version v0.8.19 --no-toolchain` -> **EXIT=0 `ANDROID-BUILD-OK`**: Python /c/Python310
(3.10.11), `APK-Werkzeuge: aapt + zipalign + apksigner aus 35.0.0`, `erwarteter Signer: 432bc749...` VOR Gradle,
stageAssets `RE2/DOOR: 27`, `RE15DOOR: 30`, `BUILD SUCCESSFUL in 5m 43s`; Kette auf der Pruefkopie (68426c29...):
zipalign ok, Signer 432bc749..., Selbsttest 140/140, Tuer-Soll 30/30 + 27/27, Gate 3603/3603; danach
`release/re15_port_v0.8.19_android.apk` = die gepruefte Kopie (sha256 68426c29... in SHA256SUMS_android.txt), keine
.ungeprueft, keine Pruefkopie. Zeiten (unter Last): gesamt 404 s, Gradle 344 s, Kette 52,8 s (Kopie 2,8 / aapt+
Stichproben 3,8 / apksigner 2,1 / Selbsttest 31,2 / Gate 8,6 / Kennungsvergleich 3,7). Ruhige Messung: R2-3.9.

### R2-3.5 make_package.sh echt - `nachher_make_package.txt` (git-Schreibzugriffe in Wegwerf-Index, `mp_isoliert_nb2.sh`)
PC-Binaries aus dem Archiv v0.8.19 mit Original-mtime (kein touch), APK aus R2-3.4.
| Lauf | EXIT | Abbruch / Ergebnis |
|---|---|---|
| P positiv | **0** (400 s) | `APK-ASSET-GATE-QUELLBAUM-OK` vor dem Kopieren; APK-Kette (Kopie, zipalign, Signer, Selbsttest, Gate); `APK-ASSET-GATE-PAKET-OK: 3603 Dateien ... bytegleich, nichts zusaetzlich` fuer BEIDE Pakete; `APK im Split-Satz = gepruefte APK (CRC32 1da8d6dd)`; `== Fertig ==` |
| N1 APK waehrend des Selbsttests gegen die (gueltig signierte!) Referenz getauscht (B3) | **1** | `APK wurde WAEHREND der Pruefung veraendert oder ersetzt ... geprueft 68426c29... jetzt 514bebd5...` - vorher wurde die Kennung erst NACH der Pruefung genommen (mp_toctou_ergebnis.txt der Gegenpruefung: K0 wuerde gezippt) |
| N2 Quellbaum ohne RE15DOOR/P2DS.DO2 (B2) | **1** (4 s) | `Quellbaum weicht von der Asset-Liste bzw. den Tuer-Tabellen der Engine ab` + `Port-Tuerarchiv fehlt: .../P2DS.DO2` - VOR den Kopierminuten; vorher: check_tree `Port-Tuerarchive im Paket: 29` gruen |
| N3 `--zip-only --only win`, Zusatzdatei shared_assets/RE2/EXTRA.BIN im Paket (B6) | **1** | `Paket ... weicht von der Asset-Liste ab`, `[zusaetzlich im Paket] 1` |
| N4 `--zip-only --only linux`, 1 Byte in shared_assets/PSX/DATA/TEX.TIM des Pakets | **1** | `[Inhalt weicht ab (sha256)] 1` - der check_tree VOR R2 (per awk unveraendert aus e1640cd0) nimmt dasselbe Paket an: `CHECK_TREE_ALT_OK` (er verglich nur RE2/DOOR, RE15DOOR, TORSE.VBS) |
Danach P2DS/TEX.TIM zurueck (sha256 = Quelle), `git restore` der Split-Volumes und SUMS, `git status release/ re15_port/` leer.

### R2-3.6 python_finden.sh (B7) - `nachher_b7.txt`, Sonde `python_finden_SONDE_neu.sh` (= neue Datei bis auf die 2 Startzeilen)
(a) PATH nur 8.3-WindowsApps: `verworfen (Link, Ziel ohne readlink nicht pruefbar - NICHT gestartet)` fuer python3
und python; (d) `RE15_PYTHON=<8.3>/python3.exe`, PATH leer: ebenso; (b) 8.3 + /usr/bin: `verworfen (Langname
C:/Users/mjoedicke/AppData/Local/Microsoft/WindowsApps/python3.exe = WindowsApps-Alias ...)`; (e) Langname: Pfadregel;
(f) normaler PATH: `/c/Python310/python (3.10.11)`. **0 x "WUERDE STARTEN"** (vorher 3 x).

### R2-3.7 check_binary_fresh (B9) - `nachher_b9.txt`
Sonde der Gegenpruefung unveraendert gegen die neue Funktion: EXIT=1 mit `ABBRUCH: Frische-Gate (Android-APK): git log
findet keinen Commit fuer ... (kein Git-Repo, flacher Klon?) - nicht pruefbar, ob .../x.apk aktuell ist`.
