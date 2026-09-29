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
