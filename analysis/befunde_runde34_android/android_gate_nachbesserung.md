# Android-Asset-Gate - Nachbesserung Runde 1

Stand: 2026-09-29, Zweig r34a/android-gate, Baum .claude/worktrees/r34a_android.
Auftrag: Befunde der Gegenpruefer (pruefer_*_r1.md) je selbst pruefen, hoch/mittel beheben,
niedrig wo ohne Risiko; Selbsttest + Positiv-/Negativ-Kontrollen erneut fahren.

## 0. Laufprotokoll (fortlaufend)

- Dossier angelegt (erster Werkzeugaufruf).

- 21:50 Python-Schnappschuss 0 (`nachbesserung_r1_belege/py_zustand_0_vorher.txt`, 121 Zeilen,
  Werkzeug des Pruefers echtlauf) = dessen Endstand (`diff` leer).
- 21:50-21:54 alle Pruefer-Behauptungen selbst nachgemessen (Abschnitt 1) - alle bestaetigt.

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

### 2.3 umgehung B3 (mittel): Selbsttest 28 -> 70 Faelle + systematische Mutanten-Probe
- **Ursache (bestaetigt):** Fixture <= 5000 B (< BLOCK), keine CRC32-erhaltende Faelschung, kein Geister-/
  Doppelzeilen-Fall; die Mutanten-Probe des Bauers schaltete nur ganze Pruefungen ab.
- **Aenderung:** neue Faelle 29-70: gute APK mit Datei > 1 MiB (4 Dateien/1060709 B in der PSX-Zeile
  verlangt), 1 Byte hinter dem 1. MiB, CRC32-erhaltende Aenderung (GF(2)-Ausgleich, `_crc_erhaltend`),
  Geisterzeile, Doppelzeile, `1_800`, Manifest kein UTF-8 / `# Notiz` / nennt sich selbst / ohne Kopf;
  Eintragsname `\` (F2), NUL-Anhang (F3), kein UTF-8, Steuerzeichen, `..`; Local Header CRC (F4),
  Groesse (F5), Name, Signatur; verschluesselt; Methode 99; Daten ragen ins Zentralverzeichnis;
  classes.dex-Byte (nur CRC schuetzt es); Deflate mit Muell dahinter / Blocktyp 3; Verzeichniseintrag;
  abgeschnitten (F7), Bytes hinter dem EOCD, ZIP64, CD-Signatur (F8), CD-Offset, ein Eintrag zu wenig;
  gute APK mit EOCD-Signatur in Asset-Daten; neun build.gradle-Lesefehler.
  Fixture wie AGP: nach dem Schreiben per zipfile bekommt jeder Local Header ein Ausrichtungsfeld 0xD935
  (6-9 B, CD-Extra bleibt 0) - ein Leser, der den Datenoffset aus dem Zentralverzeichnis rechnet, faellt
  schon im guten Fall durch. Faelschungen per eigenem Mini-Parser `_fx_cd` (unabhaengig vom Gate-Leser).
  Faelle laufen parallel (4 Prozesse): 70 Faelle in 7-12 s.
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
