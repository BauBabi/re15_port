# N1 — Geraete-Entpacker update-sicher (Runde 34, Spur android_entpacker_n1)

Stand: 2026-09-30, Zweig r34a/android-gate, Arbeitsbaum .claude/worktrees/r34a_android.
Auftrag (Nutzer woertlich): "Gehe parallel den noch offenen Punkt an." — offener Punkt v0.8.19:
Android-Bau prueft nicht, ob alle Tuerdateien in der APK landen; hier Teilpunkt N1 (Geraete-Entpacker).

Dieses Dossier wird laufend fortgeschrieben (Sitzungslimit-Regel).

## 0. Laufprotokoll

- [start] Dossier angelegt.
- [sitzung 2] Fortsetzung ab 1.3 (1.1/1.2 aus Sitzung 1 uebernommen, nicht neu gemessen). Entwurf
  `jni/asset_abgleich.h` geprueft und fertiggestellt (Pfadlaenge 512, UTF-8, Gross/klein-Dubletten ergaenzt).
- [1.3] N1a im Emulator nachgestellt (Update A -> B, `main03.wav` bleibt alt). Nutzer-AVD zu voll fuer ein
  Update -> eigene AVD-Kopie mit 12 GB Datenbereich.
- [2] `asset_abgleich.c`, Glue, CMake, Gradle v2, Gate v2 (+22 Faelle, +55 innere Proben), Unit-Test 272/0,
  C-Mutanten 14/14, Gate-Mutanten 22/22 (G22 erst nach Fall 248), Differenz-Test 0 Abweichungen.
- [3] Emulator (i)-(v) + N1b + Zusatzwege (geloeschte Datei, geaendert+weg, Abbruchzustand) mit N0/N1/N0d.
- [2.5] Eigenpruefung: Loeschen nach dem Entpacken falsch (case-insensitiver Speicher) - mit N0 auf dem Geraet
  gemessen, behoben (026632a4), Endstand N2 im Emulator komplett nachgelaufen (3.8).
- [4] build_android.sh (N2) gruen, --gate-only N2 gruen, v0.8.19 erwartungsgemaess abgelehnt (v1),
  local_build.sh all 429/429. Emulator beendet, Test-AVD-Daten geloescht.

## 1. Messen / Belegen

### 1.1 Quelle: SDL 2.28.5 oeffnet relative Pfade zuerst im internen Speicher (Befund N1b)

SDL-Quellbaum `re15_port/platform/android/_deps/SDL2-2.28.5/` (Tarball sha256 `332cb37d...66e4` = Pin in
`app/build.gradle:30`).

`src/file/SDL_rwops.c`, `SDL_RWFromFile()` :526-575 (Android-Zweig):
```
533  #if defined(__ANDROID__)
534  #ifdef HAVE_STDIO_H
535      /* Try to open the file on the filesystem first */
536      if (*file == '/') {
537          FILE *fp = fopen(file, mode);            ...
541      } else {
542          /* Try opening it from internal storage if it's a relative path */
549              SDL_snprintf(path, PATH_MAX, "%s/%s",
550                           SDL_AndroidGetInternalStoragePath(), file);
551              fp = fopen(path, mode);
553              if (fp) {
554                  return SDL_RWFromFP(fp, 1);
557      }
558  #endif /* HAVE_STDIO_H */
560      /* Try to open the file from the asset system */
566      if (Android_JNI_FileOpen(rwops, file, mode) < 0) {
```
- `HAVE_STDIO_H`: `include/SDL_config.h.cmake:69` `#cmakedefine HAVE_STDIO_H 1` (CMake-Bau wie hier);
  `include/SDL_config_android.h:46` `#define HAVE_STDIO_H 1`. (Im erzeugten Config des NDK-Baus nachgesehen: 1.2.)
- `SDL_AndroidGetInternalStoragePath()` = `context.getFilesDir().getCanonicalPath()`
  (`src/core/android/SDL_android.c:2396-2447`), z.B. `/data/user/0/de.re15.port/files`.
- `SDL_AndroidGetExternalStorageState()` (`SDL_android.c:2450ff`): WRITE nur bei
  `Environment.getExternalStorageState() == "mounted"`.
- Erst danach das APK-Asset: `Android_JNI_FileOpen` -> `AAssetManager_open(asset_manager, fileName,
  AASSET_MODE_UNKNOWN)` (`SDL_android.c:1931-1953`); den AAssetManager holt SDL per
  `context.getAssets()` + `NewGlobalRef` + `AAssetManager_fromJava` (`SDL_android.c:1882-1919`).

Port (Stand HEAD = v0.8.19, `android_glue.c` seit 4ccb96e6 unveraendert):
- `:53-56` Anker `s_root` = extern, wenn `SDL_ANDROID_EXTERNAL_STORAGE_WRITE`, sonst
  `SDL_AndroidGetInternalStoragePath()`.
- `:77` `read_apk_asset("re15_assets.txt")` und `:212` `SDL_RWFromFile(rel, "rb")` mit RELATIVEM `rel`,
  `:209` `dst = s_root/rel`, `:213` `fopen(dst, "wb")`.
- Liegt `s_root` intern, ist `<intern>/<rel>` == `dst`: `SDL_RWFromFile(rel)` oeffnet (wenn `dst` existiert)
  die ZIELDATEI als Quelle, `fopen(dst,"wb")` leert sie, `SDL_RWread` liefert 0 -> `got 0 != sz` -> Fehler,
  und `dst` ist jetzt 0 Byte gross. Beim naechsten Start dasselbe (`file_size(dst)=0 != sz`) -> Entpackfehler
  bei JEDEM Start. Ausloeser: jedes Update, das eine Datei in der Groesse aendert (sonst greift N1a).

### 1.2 Befund N1a (gleich grosse Aenderung bleibt alt) - Code

- `:161` Marker-Hash = FNV-1a 64 ueber die Bytes von `re15_assets.txt` (`"<bytes>\t<pfad>"` je Zeile, Kopfzeile
  `# re15 assets <n> <total>`, `build.gradle:141-151`) - kein Inhalt, nur Groessen und Namen.
- `:175-179` Marker gleich -> `return` ohne jede Pruefung.
- `:210` sonst je Datei `if (file_size(dst) != sz)` - gleich grosse Datei wird nie kopiert.
- Folge: aendert ein Update eine Datei bei gleicher Groesse (P07G.DO2 in Runde 33 dreimal bei 55908 B), bleibt
  die Liste bytegleich, der Marker passt, und die ALTE Datei bleibt auf dem Geraet.

### 1.3 Nachstellen im Emulator (Stand VOR dem Umbau)

Emulator: `emulator -avd Medium_Phone_API_36 -no-window -no-audio -gpu swiftshader_indirect -no-snapshot`
(Log `build/r34a/n1/logs/emulator_s2.log`), Android 16 (API 36), x86_64, Image `google_apis_playstore`
(`ro.build.type=user`, `ro.debuggable=0` -> kein `adb root`; `run-as` nur mit debuggable APK). `/data` 5,8 G,
1,5 G frei. Gemessen nebenbei:
- die Shell darf `/sdcard/Android/data/<paket>` lesen (`ls`, `sha256sum`, `adb pull` gehen ohne run-as);
- der externe App-Speicher ist **case-insensitiv**: `touch /sdcard/Download/cs_test/Aa; ls .../aa` -> rc 0,
  gefunden wird `Aa`. Folge fuer Format v2: zwei Pfade, die sich nur in Gross/klein unterscheiden, waeren auf
  dem Geraet EINE Datei, und `X.NEU` kollidiert mit der Zwischendatei von `X` (Regel im Kopf von
  `asset_abgleich.h`).

APK A = `release/re15_port_v0.8.20-n1a_android.apk` (Sitzung 1, `build_A_alt.log`: build_android.sh EXIT 0,
Gates gruen, sha256 `65301b5f...e7db`), `android_glue.c` = HEAD-Stand (seit 4ccb96e6 unveraendert).
`aapt dump badging`: `de.re15.port`, versionCode 82000, versionName v0.8.20-n1a,
launchable-activity `de.re15.port.RE15Activity`.

1. `adb install -r` A (6,5 s), `am start -W -n de.re15.port/.RE15Activity`, logcat (`s13_A_erststart.logcat`):
   ```
   06:18:52.915 [android] Speicherordner (exe-Anker + cwd): /storage/emulated/0/Android/data/de.re15.port/files (extern=1)
   06:18:53.269 [android] Entpacke 3603 Dateien (356678277 Bytes) nach /storage/emulated/0/Android/data/de.re15.port/files
   06:19:17.533 [android] Entpacken fertig: 3603 geprueft, 3603 kopiert, 0 Fehler
   ```
   Vollstaendiges Entpacken alter Stand: **24,3 s** (Emulator). Danach Capcom-Logo (`s13_A_nach_entpacken.png`).
   Marker `re15_assets_ok.txt` = `af45d06a3555f997 3603 356678277`.
   Testdatei `synchro/STAGE1/room1090/main03.wav` (109306 B) auf dem Geraet: sha256 `0fac8b2a...96ba` = Quelle.
2. APK B: dieselbe Datei im Quellbaum VORUEBERGEHEND geaendert (`build/r34a/n1/byte_kippen.py`: Byte 109206
   0x00 -> 0x01, gleiche Groesse, sha256 neu `41edb173...d1ef`; Original gesichert unter
   `build/r34a/n1/orig/main03.wav`, git-Blob `18a44b01`), `build_android.sh --version v0.8.20-n1b` (Log
   `build_B_alt.log`: EXIT 0, 5 min 32 s, Selbsttest 226/226, Gate gruen; APK sha256 `ac122a5e...d64c`).
   Danach ZURUECKGESETZT: `git checkout -- synchro/STAGE1/room1090/main03.wav release/SHA256SUMS_android.txt`,
   `git status --short synchro/ release/` leer, `cmp` gegen die Sicherung rc 0, `git hash-object` = `18a44b01`
   (= Index). A gegen B (Python zipfile): `assets/re15_assets.txt` in beiden 163901 B, **bytegleich**
   (Kopfzeile `# re15 assets 3603 356678277`); `main03.wav` weicht nur in Byte 109206 ab, alle anderen
   Asset-Eintraege haben dieselbe CRC.
3. Update A -> B scheiterte auf dem Nutzer-AVD: `INSTALL_FAILED_INSUFFICIENT_STORAGE: Failed to override
   installation location` (851 MB frei, `pm trim-caches 8G` brachte nur 26 MB). Den Nutzer-AVD NICHT
   geloescht (darauf liegen fremde Apps `com.example.goodsstore`, `com.example.goodscounter`), sondern:
   `adb uninstall de.re15.port` (vorher nicht installiert; danach wieder 4,1 G belegt / 1,5 G frei wie zu
   Beginn), Emulator beendet, und eine **Kopie derselben AVD-Konfiguration** mit eigenem Datenbereich unter
   `build/r34a/n1/avd/` (`ANDROID_AVD_HOME`), Name `Medium_Phone_API_36_r34a`: config.ini bytegleich bis auf
   AvdId/Anzeigename, `disk.dataPartition.size = 12884901888` (statt 6442450944) und Kaltstart; dasselbe
   System-Image `google_apis_playstore/x86_64` (Android 16). Start mit `-wipe-data` (erste Inbetriebnahme),
   `/data` 12 G, 11 G frei. Alle folgenden Emulator-Laeufe dort (Log `emulator_r34a.log`, Helfer
   `build/r34a/n1/lauf.sh`).
4. Auf dem Test-AVD wiederholt (`s13b_A_erst.logcat`, `s13b_B_update.logcat`):
   ```
   A frisch:  06:41:11.765 [android] Entpacke 3603 Dateien (356678277 Bytes) nach .../files
              06:41:39.414 [android] Entpacken fertig: 3603 geprueft, 3603 kopiert, 0 Fehler      (27,6 s)
              Marker af45d06a3555f997 3603 356678277; main03.wav auf dem Geraet sha256 0fac8b2a...96ba
   Update B:  dumpsys: versionName=v0.8.20-n1b, lastUpdateTime=06:42:55
              06:43:02.018 [android] Assets aktuell (af45d06a3555f997 3603 356678277)
              debug.log: [android] Assets bereits entpackt (3603 Dateien, 356678277 Bytes) unter .../files
              main03.wav auf dem Geraet (adb pull): sha256 0fac8b2a...96ba = ALT, cmp gegen das Original rc 0
   ```
   **Befund N1a nachgestellt:** B traegt `main03.wav` mit sha256 `41edb173...d1ef`, das Geraet behaelt nach
   dem Update die alte Datei - der Marker (FNV-1a ueber die bytegleiche v1-Liste) passt, `android_glue.c:175`
   kehrt ohne jede Pruefung zurueck.
5. Befund N1b (interner Speicher) laesst sich auf diesem Image nicht als Ganzes nachstellen: der
   Speicherordner liegt extern (`extern=1`, `SDL_AndroidGetExternalStorageState()` = mounted), und ohne
   `adb root`/`run-as` (Release-APK, user-Image) laesst sich unter `/data/user/0/de.re15.port/files` nichts
   anlegen. Beleg bleibt die Quelle (1.1). Der Umbau schliesst ihn konstruktiv aus (Abschnitt 2); Nachweis
   am neuen Stand in Abschnitt 3.

## 2. Bau (PORT-WAHL, kein Originalverhalten)

Die PSX las von CD, der PC-Port liest den Paketordner; das Entpacken aus der APK gibt es nur auf Android. Es
gibt daher keine `@0x...`-Adresse - jede Entscheidung unten ist eine Port-Wahl mit Begruendung und Messung.

### 2.1 Dateien

| Datei | Inhalt |
|---|---|
| `re15_port/platform/android/jni/asset_abgleich.{h,c}` | reines C99 (kein SDL/Android): SHA-256 nach FIPS 180-4, Liste v2 lesen (`re15_abgleich_lesen`), Pfadregel, Abgleich gegen "zuletzt entpackt" (`re15_abgleich_planen`), Entscheidung je Datei (`re15_abgleich_tun`), `re15_sha256_datei` |
| `re15_port/platform/android/jni/android_glue.c` | `re15_android_bootstrap_assets()` neu: Quelle AAssetManager, `.neu` + `rename()`, Liste "zuletzt entpackt", schneller Weg, Uebergang |
| `re15_port/platform/android/jni/CMakeLists.txt` | `asset_abgleich.c` in `libmain.so` (libandroid war schon gelinkt) |
| `re15_port/platform/android/app/build.gradle` | `writeAssetManifest` schreibt Format v2 (sha256 je Datei, nach Pfad sortiert, UTF-8), Grundregeln der Pfade -> Bauabbruch |
| `release/apk_asset_gate.py` | `manifest_lesen` (= Regeln von `re15_abgleich_lesen`, auf Bytes), `manifest_pruefen` (Summe je Zeile == sha256 der APK-Daten), v1 abgelehnt, Selbsttest 226 -> 248 Faelle (+21 fuer v2, +1 aus der Mutanten-Probe), innere Proben 61 -> 116 |
| `re15_port/tests/unit/test_r34a_asset_abgleich.c`, `probes/r34a_android.cmake` | PC-Unit-Test, uebersetzt `asset_abgleich.c` direkt (`unit_r34a_asset_abgleich`) |

### 2.2 Format v2 (Kopf von `asset_abgleich.h`)

```
# re15 assets v2 <anzahl> <bytes>
<bytes>\t<sha256, 64 x 0-9a-f>\t<pfad>          je Datei, Schreiber sortiert nach Pfad
```
Zeilen nur an `\n`, angehaengte `\r` weg, Leerzeilen uebersprungen; Kopfzeile genau `# re15 assets v2 ` + 1-18
Ziffern + ` ` + 1-18 Ziffern; Datenzeile 1-18 Ziffern, Tab, 64 x `[0-9a-f]` (Grossbuchstaben NICHT), Tab, Pfad.
Pfad: 1-512 Bytes, relativ, mindestens ein `/`, kein `\`, keine Steuerzeichen (< 0x20, 0x7f), kein leeres/`.`/
`..`-Segment, gueltiges UTF-8, endet nicht auf `.neu` (ASCII, Gross/klein egal). Weitere `#`-Zeilen, doppelte
Pfade (auch nur in ASCII-Gross/klein verschieden - der App-Speicher ist case-insensitiv, 1.3), NUL, keine
Datei, Kopfzeile passt nicht, > 64 MiB -> die GANZE Liste ist ungueltig. Die v1-Kopfzeile wird erkannt und
mit eigener Rueckgabe abgelehnt (`RE15_ABGLEICH_ALTES_FORMAT`, Schirm "ASSET-LISTE IM ALTEN FORMAT").
Warum fail closed statt "ungueltige Zeile ueberspringen" (v1): eine uebersprungene Zeile ist eine Datei, die
nie entpackt wird - das Spiel liefe still mit einem Loch im Asset-Baum; so steht die Meldung auf dem Schirm.

### 2.3 Entpacker (`android_glue.c`)

1. **Quelle nur der AAssetManager** (N1b): `SDL_AndroidGetActivity()` -> `getAssets()` per JNI -> globale
   Referenz -> `AAssetManager_fromJava`; Liste und Dateien per `AAssetManager_open(..., AASSET_MODE_STREAMING)`.
   `AAssetManager_open` liest nur aus den Asset-Pfaden der APK (derselbe Weg, den SDL erst NACH dem
   Dateisystem-Versuch nimmt: `Android_JNI_FileOpen`, SDL_android.c:1931-1953) - ein Dateisystem-Pfad, der die
   Zieldatei selbst treffen koennte, kommt nicht mehr vor.
2. **Ziel `<ziel>.neu`**, beim Schreiben gehasht; nur wenn Groesse UND sha256 der Liste entsprechen `rename()`
   an den Platz, sonst `.neu` loeschen und Fehler. Die Liste "zuletzt entpackt" behauptet damit nie eine Summe,
   die die Datei nicht hat (die Summe ist beim Schreiben gemessen, nicht aus der Liste abgeschrieben).
3. **"zuletzt entpackt"** = bytegenaue Kopie von `assets/re15_assets.txt` als `<speicher>/re15_assets_entpackt.txt`,
   geschrieben erst nach vollstaendigem Erfolg: `sync()`, dann `.neu` + `fsync` + `rename` + `fsync` des Ordners.
   Ein Lauf, der etwas aendern kann, loescht diese Kopie ZUERST und macht das Loeschen per `fsync` des Ordners
   haltbar. Folge: nach JEDEM Abbruch (Prozess beendet oder Strom weg) gibt es keine alte Liste, und der naechste
   Start prueft jede gleich grosse Datei per SHA-256 (inhaltlich richtig, egal was halb geschrieben wurde) -
   darum kein `fsync` je Datei noetig.
4. **Start**: Liste bytegleich der zuletzt entpackten -> **schneller Weg**: nur `stat()` je Datei (Groesse);
   stimmt eine nicht (Datei geloescht), werden genau diese neu entpackt. Sonst Abgleich
   (`re15_abgleich_planen` + `re15_abgleich_tun`):

   | alte Liste | Eintrag | Datei im Speicher | Tun |
   |---|---|---|---|
   | gueltig | gleiche Groesse UND Summe (BEHALTEN) | Groesse stimmt | nichts |
   | gueltig | BEHALTEN | fehlt / andere Groesse | entpacken |
   | gueltig | Groesse oder Summe anders (GEAENDERT) | egal | entpacken (Befund N1a) |
   | gueltig | nicht in der alten Liste (NEU) | egal | entpacken |
   | gueltig | nur in der alten Liste | - | Datei loeschen, VOR dem Entpacken (Regeln wie jede Zeile: nie ausserhalb; Reihenfolge siehe 2.5) |
   | keine/unlesbar (PRUEFEN) | - | gleiche Groesse | sha256 der Datei gegen die Liste; anders -> entpacken |
   | keine/unlesbar (PRUEFEN) | - | fehlt / andere Groesse | entpacken |

   "keine" = Erstinstallation, **v0.8.19-Geraet** (nur `re15_assets_ok.txt`, keine v2-Liste), abgebrochener Lauf.
   Nach Erfolg wird der alte Marker `re15_assets_ok.txt` geloescht. `.neu`-Reste eines Abbruchs werden in jedem
   Lauf, der etwas aendern kann, je Datei entfernt.
5. **Uebergang v0.8.19**: einmal jede gleich grosse Datei per SHA-256 pruefen statt alles neu zu schreiben -
   Pruefen ist eine echte Teilmenge des Entpackens (Entpacken = APK lesen + schreiben + hashen), also nie
   teurer; gemessen in Abschnitt 3 (iv). Fortschrittsbalken bleibt ("ASSETS WERDEN EINMALIG GEPRUEFT").
6. Protokoll: je Datei nur im Update (`entpacke <pfad> (geaendert|neu|Groesse falsch/fehlt)`, `Summe weicht ab
   -> neu: <pfad>`, `entfernt ...`), bei der Erstinstallation nur die Summe. Abschlusszeile
   `[android] Entpacken fertig (<modus>): ... kopiert (B, ms), ... per SHA-256 geprueft (B, ms, abweichend),
   ... entfernt, ... .neu-Reste, ... Fehler, <ms>` bzw. `[android] Assets aktuell (schneller Weg): ...`.

### 2.4 Pruefung des C-Teils (PC)

- `unit_r34a_asset_abgleich`: 272 Pruefungen, 0 Fehler (`gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow
  -Wconversion`, ohne Warnung). SHA-256: FIPS-180-2-Vektoren (`""`, `abc`, 448/896 Bit, 10^6 x `a` in 9
  Stueckelungen), Kette ueber die Laengen 0..300 (Python hashlib: `9ab015b3...653b`), Datei; Liste: gut/CRLF/
  Leerzeilen/ohne Schluss-`\n`/UTF-8, v1 -> ALTES_FORMAT, 18 Kopf-, 40 Zeilen-/Pfadfaelle, 512/513 Bytes, NUL,
  Ueberlauf, 64 MiB/+1; Abgleich BEHALTEN/GEAENDERT (gleiche Groesse, andere Summe)/NEU/weg/PRUEFEN; `tun`-Tabelle.
- **Mutanten-Probe** (`build/r34a/n1/pc/mutanten.py`): 14 Abschwaechungen von `asset_abgleich.c` - darunter M3
  "BEHALTEN nur nach Groesse" = genau Befund N1a - machen den Test alle ROT (0 von 14 nicht gefangen).
- **Differenz-Test Geraet <-> Gate** (`build/r34a/n1/diff/`): derselbe Byte-Korpus durch `asset_abgleich.c`
  (PC-uebersetzt) und `manifest_lesen` des Gates - 20257 Listen (die 55 inneren Proben + 20000 Zufalls-Mutanten
  einer gueltigen Liste: Bytes ersetzen/einfuegen/loeschen aus einem Alphabet mit `\n \r \t \0 # / . \ `, Ziffern,
  Hex-Buchstaben gross/klein, `0x7f`, UTF-8-Start-/Folgebytes; Zeilen verdoppeln/loeschen, Gross/klein tauschen):
  4769 x beide gueltig, 15488 x beide ungueltig, **0 Abweichungen** (bei gueltig auch dieselben Eintraege:
  sha256 ueber `pfad\tgroesse\tsha` aller Zeilen gleich). Zweiter Lauf mit der ECHTEN v2-Liste von N0 (3603
  Zeilen) + 200 Mutanten davon (vorher lag dort noch die v1-Liste aus dem B-Bau): 20257 Listen, 4808 / 15449,
  **0 Abweichungen**.
- **Gate-Selbsttest**: 248/248 Faelle, innere Proben 116/116 (`gate_selbsttest_2.log`); die Faelle 203-226 (R4,
  MU1-MU8) auf v2 umgestellt: Tab/VT/FF am Pfadende sind jetzt Steuerzeichen im Pfad ("unzulaessiger Pfad
  (Steuerzeichen)" statt "Manifest nennt ..."), fremde Ziffern "ist keine Zahl (1-18 Ziffern 0-9)", Zeile 1 mit
  Leerzeichen davor "Manifest-Zeile 1: erwartet ...", Trenner-Faelle mit der sha256-Spalte.
- **Gate-Mutanten-Probe** (`build/r34a/n1/gate_mutanten.py`, Selbsttest im Schnellmodus je Mutant): 22
  Abschwaechungen von `manifest_lesen`/`manifest_pruefen`/`_pfad_fehler` (Summe gross erlaubt, Summenvergleich mit
  der APK weg, v1 nicht erkannt, `.neu` weg/nur klein, Gross/klein-Dubletten, 513 Bytes, NUL, leere Liste, 19
  Ziffern in Groesse bzw. Kopf, `rstrip()`, `splitlines()`, UTF-8-Regel weg, Steuerzeichen erst ab 0x09, DEL,
  Ueberlauf, ohne `/`, `..`, Kommentarzeile, Kopf-Abgleich weg, APK-Summe fuer Eintraege ohne Quelle): Lauf 1
  (`gate_mutanten_1.log`) 21 ROT, **G22 blieb GRUEN** (die sha256 von APK-Eintraegen ohne Quelle wurde nicht
  erfasst - Urteil unveraendert, weil solche Eintraege immer "zusaetzlich in der APK" ausloesen, aber die Meldung
  "Manifest-Pruefsumme falsch" fehlte) -> Fall 248 "Zusatzeintrag ohne Quelle, Manifest-Summe falsch" ergaenzt ->
  G22 ROT. Endstand 0 von 22 nicht gefangen.

### 2.5 Eigenpruefung: Loeschen erst NACH dem Entpacken war falsch (behoben)

Beim Durchsehen des Glue nach den ersten Emulator-Laeufen gefunden: die erste Fassung loeschte die Pfade "nur in
der alten Liste" NACH der Entpack-Schleife. Der App-Speicher ist case-insensitiv (gemessen, 1.3). Aendert eine
Version einen Pfad nur in der Gross/Kleinschreibung (alt `a/X.BIN`, neu `a/x.bin`), so schreibt die Schleife
zuerst `a/x.bin` (ueber den Eintrag `a/X.BIN`), und das anschliessende `unlink("a/X.BIN")` trifft genau diese
frische Datei - sie fehlt bis zum naechsten Start (dann holt der Groessen-Nachlauf sie zurueck). Die Liste v2
verbietet solche Paare nur INNERHALB einer Liste, nicht zwischen alter und neuer. Behoben: erst loeschen, dann
entpacken - auf einem case-insensitiven Speicher faellt die alte Schreibweise weg und die neue wird frisch
geschrieben, auf einem case-sensitiven (interner Speicher) ebenso ohne Rest. Die APKs N0/N1 der Laeufe 3.1-3.6
stammen von VOR diesem Fix; Nachlauf mit dem Endstand in 3.8.

## 3. Nachweis im Emulator

Test-AVD `Medium_Phone_API_36_r34a` (1.3 Punkt 3). APKs (alle `build_android.sh`, Gates gruen):
- **N0** = neuer Stand, `--version v0.8.20-n1c` (Log `build_N0.log`);
- **N1** = N0 + dieselbe gleich grosse Aenderung an `synchro/STAGE1/room1090/main03.wav` wie bei B
  (voruebergehend, danach zurueckgesetzt), `--version v0.8.20-n1d`;
- Referenz v0.8.19 = `build/r34a/ref_v0.8.19.apk` (Kopie aus dem Archiv, sha256 wie im Archiv).
Pruefwerkzeug `build/r34a/n1/geraet_pruefen.py <apk>`: sha256 JEDER Datei unter `shared_assets/` + `synchro/`
auf dem Geraet gegen die Liste v2 der APK, dazu Zusatzdateien, `.neu`-Reste und die oberste Ebene.

N0: `build_N0.log` EXIT 0 (8 min 42 s), `re15_assets.txt (v2): 3603 Dateien, 356678277 Bytes, sha256 je Datei`,
Gate-Selbsttest 248/248, `APK-ASSET-GATE-OK ... Manifest stimmt`; APK sha256 `027099ed...8555`, 363479831 B
(267428 B mehr als A: die Liste traegt jetzt 3603 x 65 Zeichen sha256 + Tab; Liste 398099 B statt 163901 B).
Bauwarnungen: nur die bekannten `-Wcomment` aus `re15_emd.h`/`re15_itps.h`, keine aus den neuen Quellen.

### 3.1 (i) Frische Installation

`adb uninstall de.re15.port` (externer Ordner weg: `ls /sdcard/Android/data | grep -c re15` = 0), `lauf.sh
s3i_N0_frisch <N0>`:
```
07:02:00.518 [android] Abgleich (ohne Liste): 3603 Dateien (356678277 Bytes) - behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
07:02:18.660 [android] Entpacken fertig (ohne Liste): 3603 geprueft, 3603 kopiert (356678277 B, 15482 ms),
             0 per SHA-256 geprueft (0 B, 0 ms, 0 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 18172 ms
```
18,2 s gesamt (15,5 s Kopieren inkl. Hashen) - der alte Entpacker brauchte auf demselben AVD 27,6 s (1.3 Punkt 4).
`geraet_pruefen.py` (`s3i_geraet.txt`): 3603 Dateien, fehlen 0, sha256 falsch 0, zusaetzlich 0, `.neu`-Reste 0,
`re15_assets_entpackt.txt` sha256 `95770eb5...15dd` = Liste der APK, kein `re15_assets_ok.txt` ->
**GERAET-KONSISTENT**. Bild (`s3i_bild4.png`, angesehen): Titelbild "BIOHAZARD 2 Sample Ver.2025.01.25",
NEW GAME / LOAD GAME / OPTION (vorher lag Androids Einmal-Hinweis "Viewing full screen" darueber -
Systemdialog des frischen AVD, per `settings put secure immersive_mode_confirmations confirmed` abgestellt).

### 3.2 (iii) Neustart und Update ohne Aenderung -> schneller Weg

```
Neustart (force-stop + start, s3iii_N0_neustart):
  07:03:43.907 [android] Assets aktuell (schneller Weg): Liste = zuletzt entpackt, 3603 Dateien, Groessen geprueft, 120 ms
dieselbe APK als Update (adb install -r N0, s3iii_N0_gleiche_apk):
  07:04:07.099 [android] Assets aktuell (schneller Weg): Liste = zuletzt entpackt, 3603 Dateien, Groessen geprueft, 238 ms
```
(Das Update mit einer ANDEREN APK ohne Asset-Aenderung - N1 -> N1 - steht unter 3.3.)

### 3.3 Weitere Wege des Entpackers auf dem Geraet (N0, Speicherordner per adb-Shell veraendert)

Die Shell darf im Speicherordner schreiben (`touch`/`rm`/`dd`/`adb push` gehen; Datei gehoert dann `shell`,
die App liest/loescht sie trotzdem - gemessen unten).

- **(vi) eine entpackte Datei geloescht** (`rm synchro/STAGE1/room1090/main04.wav`, 298780 B), Neustart
  (`s3vi_datei_geloescht`):
  ```
  [android] Liste = zuletzt entpackt, aber 1 Dateien fehlen/falsche Groesse -> neu entpacken
  [android] Abgleich (Groessen-Nachlauf): ... behalten 3603, geaendert 0, neu 0, pruefen 0, weg 0
  [android] entpacke synchro/STAGE1/room1090/main04.wav (Groesse falsch/fehlt)
  [android] Entpacken fertig (Groessen-Nachlauf): 3603 geprueft, 1 kopiert (298780 B, 15 ms), ... 0 Fehler, 937 ms
  ```
  danach GERAET-KONSISTENT.
- **(vii) "zuletzt entpackt" nennt einen anderen Stand** (`build/r34a/n1/liste_manipulieren.py`: in der Liste auf
  dem Geraet `main05.wav` mit anderer sha256 bei gleicher Groesse, dazu eine Zeile `synchro/STAGE1/room1090/geist.bin`
  (5 B) + die Datei selbst; Kopfzeile 3604/356678282 nachgerechnet, nach `manifest_lesen` gueltig), Neustart
  (`s3vii_liste_anders`):
  ```
  [android] Abgleich (Update): 3603 Dateien (356678277 Bytes) - behalten 3602, geaendert 1, neu 0, pruefen 0, weg 1
  [android] entpacke synchro/STAGE1/room1090/main05.wav (geaendert)
  [android] entfernt (nicht mehr in der Liste): synchro/STAGE1/room1090/geist.bin
  [android] Entpacken fertig (Update): 3603 geprueft, 1 kopiert (164290 B, 7 ms), ..., 1 entfernt, 0 .neu-Reste, 0 Fehler, 1167 ms
  ```
  `main05.wav` neu geschrieben (Zeit 07:05, sha256 wieder `d905e992...`), `geist.bin` weg, Liste wieder = APK
  (`95770eb5...`), GERAET-KONSISTENT. Das ist derselbe Weg wie (ii), nur ohne neue APK.
- **(viii) Zustand nach einem Abbruch** (Liste geloescht, `.neu`-Rest `shared_assets/PSX/STAGE1/ROOM1240.RDT.neu`
  angelegt, `main06.wav` per `dd conv=notrunc` an Stelle 1000 veraendert: gleiche Groesse 240824 B, sha256
  `f84f76dd...` -> `e0bf118d...`), Neustart (`s3viii_ohne_liste_rest_kaputt`):
  ```
  [android] Abgleich (ohne Liste): ... behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
  [android] Summe weicht ab -> neu: synchro/STAGE1/room1090/main06.wav
  [android] Entpacken fertig (ohne Liste): 3603 geprueft, 1 kopiert (240824 B, 9 ms), 3603 per SHA-256 geprueft
            (356678277 B, 3130 ms, 1 abweichend), 0 entfernt, 1 .neu-Reste, 0 Fehler, 4441 ms
  ```
  GERAET-KONSISTENT (kein `.neu`, `main06.wav` wieder richtig).

(Der Vergleich 4,4 s gegen 18,2 s taugt NICHT als Aufwandsvergleich: (viii) lief mit warmem Cache - die Dateien
waren kurz vorher von `geraet_pruefen.py` gelesen worden -, (i) lief waehrend des N1-Baus. Fairer Vergleich
unter 3.5.)

### 3.4 (ii) Update mit einer gleich grossen Inhaltsaenderung (N0 -> N1)

N0 und N1 unterscheiden sich in der Liste in GENAU einer Zeile (`difflib` ueber `assets/re15_assets.txt`):
```
-109306	0fac8b2a951bbd5cbd6fe592ee5aff7d4cdab24c3fb142ba04936bc19ec196ba	synchro/STAGE1/room1090/main03.wav
+109306	41edb1737570110db4576bc091fec03ec4c8c718e99276a154f3ec902ec0d1ef	synchro/STAGE1/room1090/main03.wav
```
(N1-Bau `build_N1.log` EXIT 0, Selbsttest 248/248, Gate gruen; danach `main03.wav` und `SHA256SUMS_android.txt`
per `git checkout` zurueckgesetzt, `git status` leer, `cmp` gegen die Sicherung rc 0, Blob `18a44b01`.)
Vorher auf dem Geraet: `main03.wav` sha256 `0fac8b2a...`. `lauf.sh s3ii_N1_update <N1>`:
```
dumpsys: versionName=v0.8.20-n1d, lastUpdateTime=07:06:51
07:06:54.957 [android] Abgleich (Update): 3603 Dateien (356678277 Bytes) - behalten 3602, geaendert 1, neu 0, pruefen 0, weg 0
07:06:56.176 [android] entpacke synchro/STAGE1/room1090/main03.wav (geaendert)
07:06:56.242 [android] Entpacken fertig (Update): 3603 geprueft, 1 kopiert (109306 B, 6 ms), 0 per SHA-256 geprueft
             (0 B, 0 ms, 0 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 1325 ms
```
`adb pull` `main03.wav`: sha256 **`41edb173...d1ef` = neuer Inhalt**; `cmp -l` gegen das Original: genau Byte 109207
(1-basiert) `1` statt `0`. `geraet_pruefen.py <N1>`: GERAET-KONSISTENT, `re15_assets_entpackt.txt` = Liste von N1
(`3a54758c...5b62`). **Befund N1a behoben** (Gegenprobe zu 1.3 Punkt 4, dort blieb die Datei alt).

### 3.5 (iv) Uebergang von v0.8.19 (Referenz-APK -> N1)

`adb uninstall`, `lauf.sh s3iv_ref0819_erst build/r34a/ref_v0.8.19.apk` (versionCode 81900, alter Entpacker):
```
07:07:39.691 [android] Entpacke 3603 Dateien (356678277 Bytes) nach .../files
07:07:49.923 [android] Entpacken fertig: 3603 geprueft, 3603 kopiert, 0 Fehler
oberste Ebene: debug.log re15_assets_ok.txt shared_assets synchro; Marker af45d06a3555f997 3603 356678277 (= A)
main03.wav sha256 0fac8b2a... (v0.8.19-Inhalt)
```
Dann N1 als Update (`s3iv_uebergang_N1`):
```
dumpsys: versionName=v0.8.20-n1d
07:08:17.215 [android] Abgleich (Uebergang v0.8.19): 3603 Dateien (356678277 Bytes) - behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
07:08:31.947 [android] Summe weicht ab -> neu: synchro/STAGE1/room1090/main03.wav
07:08:32.384 [android] Entpacken fertig (Uebergang v0.8.19): 3603 geprueft, 1 kopiert (109306 B, 27 ms), 3603 per SHA-256
             geprueft (356678277 B, 11944 ms, 1 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 15216 ms
oberste Ebene danach: debug.log re15_assets_entpackt.txt shared_assets synchro   (re15_assets_ok.txt geloescht)
```
GERAET-KONSISTENT gegen N1. Bild danach (`s3iv_bild.png`, angesehen): Titelbild wie in (i) - das Spiel laeuft.
Genau die eine gleich grosse Aenderung wurde gefunden, die der alte Entpacker uebersah.

**Aufwand, fair gemessen** (N1 installiert, abwechselnd direkt hintereinander, `s3iv_zeit_*`):

| Runde | Pruefen (Liste weg -> alle 3603 per SHA-256) | Entpacken (Assets + Liste weg -> alle 3603) |
|---|---|---|
| 1 | 8116 ms (Hashen 6702 ms; Dateien kurz vorher gelesen) | 16485 ms (Kopieren 13586 ms) |
| 2 | 14508 ms (Hashen 11998 ms; Dateien frisch geschrieben) | 12110 ms (Kopieren 9714 ms) |
| Uebergang oben | 15216 ms (Hashen 11944 ms, kalt) | - |
| (viii) oben | 4441 ms (Hashen 3130 ms, warm) | - |

Der Emulator zeigt **keinen klaren Zeitvorteil** fuer eine Seite: die Streuung (Host-Last durch parallele Bau-
Spuren, Cache-Zustand) ist groesser als der Unterschied. Das Lesen der entpackten Dateien geht durch FUSE
(`/storage/emulated`), das Entpacken liest die APK direkt - darum ist Pruefen hier nicht billiger, obwohl es eine
echte Teilmenge der Arbeit ist (Entpacken = APK lesen + schreiben + dieselbe SHA-256). **Gewaehlt: Pruefen**, weil
es bei gleichem gemessenem Zeitaufwand nichts schreibt (keine 356 MB Schreiblast auf den Flash des Geraets, keine
`.neu`-Zwischendateien) und nur abweichende Dateien neu entpackt; die Fortschrittsanzeige laeuft in beiden Faellen
("ASSETS WERDEN EINMALIG GEPRUEFT"). Echtes Geraet: nicht gemessen (keins angeschlossen).

### 3.6 (v) Abbruch waehrend des Entpackens (`am force-stop`) und Neustart

Jeweils `adb uninstall`, N0 frisch installiert, App gestartet, waehrend des Entpackens beendet, danach
`lauf.sh` (Neustart) und `geraet_pruefen.py`:

| Lauf | Abbruch | Zustand danach | Neustart | Ergebnis |
|---|---|---|---|---|
| v-a (`abbruch.sh`, Schwelle 1500 Dateien) | `am force-stop` nach 14 s | 1624 Dateien, keine Liste, kein `.neu` (zwischen zwei Dateien getroffen), Log endet nach `Abgleich (ohne Liste)` | `Entpacken fertig (ohne Liste): 3603 geprueft, 1979 kopiert (249711725 B, 10870 ms), 1624 per SHA-256 geprueft (106966552 B, 1431 ms, 0 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 15677 ms` | KONSISTENT |
| v-b (`abbruch2.sh`, Ausloeser `CDEMD0.EMS.neu` per adb) | `.neu` gesehen, Stopp kam per adb-Rundlauf zu spaet | 3235 Dateien, 11-MB-Datei fertig umbenannt, kein `.neu` | `368 kopiert, 3235 per SHA-256 geprueft (314211360 B, 6571 ms, 0 abweichend), 0 .neu-Reste, 0 Fehler, 9938 ms` | KONSISTENT |
| v-c (`abbruch3.sh`, Ausloeser AUF dem Geraet: Shell-Schleife + `cmd activity force-stop`) | mitten in `shared_assets/RE2/CDEMD0.EMS` (11124736 B) | `CDEMD0.EMS.neu` **6291456 B** (halb), `CDEMD0.EMS` **existiert nicht** (nie umbenannt), 3226 Eintraege | `378 kopiert (54157661 B, 1792 ms), 3225 per SHA-256 geprueft (302520616 B, 7352 ms, 0 abweichend), 0 entfernt, **1 .neu-Reste**, 0 Fehler, 11293 ms` | KONSISTENT, kein `.neu` |

In keinem Abbruch lag eine halb geschriebene Datei unter ihrem echten Namen, und nie eine Liste "zuletzt
entpackt" (sie entsteht erst nach vollem Erfolg). Nach v-c laeuft das Spiel (`s3v_bild.png`, angesehen:
Hinweisbild "This game contains scenes of explicit violence and gore." vor dem Intro).
(Der Ausloeser von v-c legte vorab `mkdir -p <files>` an; die Ordner gehoeren danach trotzdem der App
`u0_a216`, der Lauf ist davon unberuehrt.)

### 3.7 Befund N1b (interner Speicher)

Ganz nachstellen laesst er sich hier nicht (1.3 Punkt 5: Speicherordner extern, kein `adb root`). Belegt ist er
ausgeschlossen:
- **statisch**: im alten Glue waren `:77` (`read_apk_asset` -> Liste) und `:212` (je Datei) die einzigen
  `SDL_RWFromFile`-Aufrufe des Ports (`git show eda2360b:.../android_glue.c | grep -n SDL_RWFromFile`); nach dem
  Umbau gibt es in `re15_port/engine`, `platform/pc`, `platform/android/jni`, `include` KEINEN Aufruf von
  `SDL_RWFromFile`/`SDL_RWFromFP`/`SDL_LoadFile` mehr (grep rc 1). Die Quelle ist nur noch `AAssetManager_open`
  auf den Asset-Pfad der APK (SDL selbst nimmt diesen Weg erst nach dem Dateisystem-Versuch, 1.1). Dazu schreibt
  der Entpacker nie in die Zieldatei selbst, sondern in `<ziel>.neu` - selbst eine (hypothetische) Quelle gleich
  dem Ziel wuerde nicht mehr vor dem Lesen geleert.
- **dynamisch** (debuggable Bau N0d = `build_android.sh --debug --version v0.8.20-n1e`, EXIT 0, Selbsttest 248/248,
  Gate gruen; `pkgFlags=[ DEBUGGABLE ... ]`; Stand vor dem Fix 2.5, fuer diese Frage ohne Belang),
  `build/r34a/n1/intern_pflanzen.sh`: frisch installiert und VOR dem ersten Start per `run-as` im internen
  Speicher (`/data/user/0/de.re15.port/files` = `getFilesDir()` = `SDL_AndroidGetInternalStoragePath()`, genau
  dort suchte `SDL_RWFromFile(<relativ>)` zuerst) angelegt:
  `files/re15_assets.txt` = falsche Liste `# re15 assets v2 1 5` / `synchro/gepflanzt.bin` (sha256 `1d4d9200...`)
  und `files/synchro/STAGE1/room1090/main03.wav` = 109306 Null-Bytes (gleiche Groesse, sha256 `0a3bdf0d...`).
  Start (`s3b_intern_gepflanzt`):
  ```
  [android] Abgleich (ohne Liste): 3603 Dateien (356678277 Bytes) - ... pruefen 3603, weg 0      <- Liste der APK, nicht die gepflanzte
  [android] Entpacken fertig (ohne Liste): 3603 geprueft, 3603 kopiert (356678277 B, 14496 ms), ... 0 Fehler, 17072 ms
  extern main03.wav: sha256 0fac8b2a...96ba (= APK), nicht 0a3bdf0d... (gepflanzt)
  intern danach: beide gepflanzten Dateien unveraendert (gleiche sha256) - weder gelesen noch geleert
  ```
  GERAET-KONSISTENT. Der alte Entpacker haette an dieser Stelle die gepflanzte Liste gelesen (`:77`, SDL-Quelle
  1.1); das ist nicht dynamisch nachgestellt (dafuer waere ein debuggable Bau des ALTEN Stands noetig gewesen).

### 3.8 Befund 2.5 auf dem Geraet und Nachlauf mit dem Endstand (N2)

**Vorher (N0, Stand vor dem Fix)**, `build/r34a/n1/liste_gross.py`: in der Liste "zuletzt entpackt" auf dem
Geraet `synchro/STAGE1/room1090/main05.wav` -> `MAIN05.WAV` (Groesse/Summe gleich, Liste gueltig), Neustart
(`s25_gross_vorher_N0`):
```
[android] Abgleich (Update): ... behalten 3602, geaendert 0, neu 1, pruefen 0, weg 1
[android] entpacke synchro/STAGE1/room1090/main05.wav (neu)
[android] entfernt (nicht mehr in der Liste): synchro/STAGE1/room1090/MAIN05.WAV
[android] Entpacken fertig (Update): 3603 geprueft, 1 kopiert (164290 B, 5 ms), ..., 1 entfernt, 0 .neu-Reste, 0 Fehler, 1000 ms
ls room1090: main00..main04, main06 - main05.wav FEHLT;  geraet_pruefen: fehlt 1 -> GERAET-ABWEICHUNG
```
Der Lauf meldet 0 Fehler und schreibt die Liste - und die frisch entpackte Datei ist weg. Befund 2.5 gemessen.

**Endstand N2** = `build_android.sh --version v0.8.20-n1f` nach dem Fix (Commit 026632a4), `build_N2.log` EXIT 0
(4 min 38 s), Selbsttest 248/248, Gate gruen, APK sha256 `c393a18e...c4e6`; Liste bytegleich der von N0:
| Lauf | Log | Ergebnis |
|---|---|---|
| frische Installation | `s38a_N2_frisch` | `3603 kopiert (356678277 B, 12930 ms) ... 0 Fehler, 15271 ms`, KONSISTENT |
| Gross/klein wie oben | `s38b_N2_gross` | `entfernt ... MAIN05.WAV` (07:35:34.245) VOR `entpacke ... main05.wav (neu)` (07:35:35.239), `1 kopiert, 1 entfernt, 0 Fehler`, **KONSISTENT** |
| geaendert + weg (liste_manipulieren) | `s38c_N2_liste_anders` | `entfernt ... geist.bin`, `entpacke ... main05.wav (geaendert)`, KONSISTENT |
| Neustart | `s38d_N2_neustart` | `Assets aktuell (schneller Weg) ... 268 ms`; Bild `s38_bild.png` (angesehen): Titelbild |
| Uebergang v0.8.19 -> N2 | `s38e_uebergang_N2` | `Abgleich (Uebergang v0.8.19)`, `3603 per SHA-256 geprueft (356678277 B, 9298 ms, 0 abweichend)`, 0 kopiert, KONSISTENT (N2 hat die Assets von v0.8.19) |
| Abbruch (`abbruch.sh`, 1500) + Neustart | `s3v_abbruch.logcat`, `s38f_N2_neustart_nach_abbruch` | Abbruch traf diesmal mitten in eine Datei: `BG05.BSS.neu` 65536 B; Neustart `2004 kopiert, 1599 per SHA-256 geprueft (0 abweichend), 1 .neu-Reste, 0 Fehler`, KONSISTENT |

(ii) mit echten APKs lief mit N0 -> N1 (3.4); der Fix beruehrt den Weg "geaendert -> entpacken" nicht, und
derselbe Weg ist mit N2 ueber die manipulierte Liste (Zeile 3) belegt.

## 4. Gates / Suite

| Pruefung | Ergebnis | Log |
|---|---|---|
| `build_android.sh --version v0.8.20-n1f` (Endstand N2) | EXIT 0, `ANDROID-BUILD-OK`, `re15_assets.txt (v2)` 3603 Dateien, Selbsttest 248/248, `APK-ASSET-GATE-OK ... Manifest stimmt`; APK sha256 `c393a18e...c4e6`, 363479879 B | `build_N2.log` |
| `build_android.sh --gate-only <N2> --version v0.8.20-n1f` (mit dem Gate-Endstand 85c8189e) | EXIT 0: aapt/badging, `zipalign -c -P 16 4` ok, Signer `432bc749...` = `apk_signer.sha256`, Selbsttest 248/248, `ANDROID-GATES-OK (--gate-only)` | `gateonly_N2.log`, `gateonly_N2_final.log` |
| `build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19` | **EXIT 1 - erwartet**: alle 3603 Dateien in 5 Baeumen bytegleich zum Quellbaum, Tuer-Soll erfuellt, einziger Befund `[Manifest] Manifest im alten Format v1 ('# re15 assets 3603 356678277', bis v0.8.19: ohne sha256) - das Geraet lehnt es ab und entpackt NICHTS`. Eine v0.8.19-APK kann nach diesem Umbau kein Paket mehr bestehen; das naechste Paket braucht eine neu gebaute APK (Liste v2). | `gateonly_ref0819.log`, `gate_ref0819_direkt.log` |
| `re15_port/tools/local_build.sh all` | `=== LOCAL-BUILD-OK (all) — Tests 429/429` (voller Neubau 1618 Schritte, 20 min unter Last), darin `unit_r34a_asset_abgleich ... Passed 1.57 sec`; `RE15_MIN_TESTS` 428 -> 429 | `pc_suite.log` |
| Gate-Selbsttest einzeln | 248/248, innere Proben 116/116 | `gate_selbsttest_3.log` |
| Gate-Mutanten | 22/22 gefangen (2.4) | `gate_mutanten_1.log` + G22-Nachlauf |
| C-Mutanten | 14/14 gefangen (2.4) | `build/r34a/n1/pc/mutanten.py` |
| Differenz-Test Geraet <-> Gate | 2 x 20257 Listen, 0 Abweichungen (2.4) | `build/r34a/n1/diff/` |

Nach jedem Lauf, der `release/` beschreibt: nur `release/SHA256SUMS_android.txt` per `git checkout` zurueckgesetzt
(`git status --short release/` danach leer bzw. nur die eigene Gate-Aenderung); keine APK committet.
APKs dieser Runde (ungetrackt, `release/`): n1a (alter Stand, A), n1b (alter Stand + Aenderung, B), n1c (N0),
n1d (N1), n1e (N0d debuggable), n1f (N2 = Endstand); zusammen 2,1 GB. Test-AVD: Datenabbilder nach dem letzten
Lauf geloescht (6,7 GB), `build/r34a/n1/avd/*.ini` + `config.ini` bleiben (mit `-wipe-data` neu aufsetzbar).
Emulator beendet (`adb emu kill`, keine qemu-/emulator-Prozesse mehr). Nutzer-AVD `Medium_Phone_API_36`:
`de.re15.port` wieder deinstalliert, 4,1 G belegt wie vorher, sonst unveraendert.

## 5. Offen

- **Echtes Geraet nicht gemessen** (keins angeschlossen): Laufzeiten nur Emulator; ob Pruefen auf einem Telefon
  schneller ist als Entpacken (dort ist Schreiben meist teurer als Lesen), ist nicht belegt.
- **N1b nicht als Ganzes nachgestellt** (interner Speicherordner laesst sich auf dem user-Image nicht erzwingen);
  statisch ausgeschlossen und mit gepflanzten Dateien am neuen Stand geprueft (3.7). Der alte Stand wurde fuer N1b
  nicht dynamisch gezeigt (braeuchte einen debuggable Bau des alten Glue).
- **Stufe-4-Aenderungen der Kette** (make_package.sh / apk_pruefen.sh, B1-B4) sind weiterhin nicht gegengeprueft;
  dieser Auftrag hat sie nicht angefasst. Folge des Formatwechsels fuer die Kette: make_package.sh laeuft ueber
  dasselbe Gate - eine APK mit Liste v1 (jede bis v0.8.19) wird dort jetzt abgelehnt.
- **Nicht abgedeckt, bewusst**: eine im Speicher gleich gross veraenderte Datei bei unveraenderter Liste erkennt
  der schnelle Weg nicht (nur `stat`); ein Pfad, der case-insensitiv Ordner eines anderen Pfades ist
  (`a/b` und `A/B/c`), scheitert erst beim Entpacken laut (Fehlerbild) statt schon beim Lesen der Liste - in einem
  Quellbaum auf NTFS kann es das nicht geben.
- Update mit Aenderungen kostet auf dem Emulator ~1,0-1,5 s, davon der groesste Teil `stat` + `unlink(<ziel>.neu)`
  je Datei ueber FUSE (3603 x); bewusst so gelassen (Aufraeumen von `.neu`-Resten in jedem aendernden Lauf).
- Gegenpruefung dieses Umbaus (N1) steht aus.
