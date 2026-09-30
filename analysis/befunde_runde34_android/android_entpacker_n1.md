# N1 — Geraete-Entpacker update-sicher (Runde 34, Spur android_entpacker_n1)

Stand: 2026-09-30, Zweig r34a/android-gate, Arbeitsbaum .claude/worktrees/r34a_android.
Auftrag (Nutzer woertlich): "Gehe parallel den noch offenen Punkt an." — offener Punkt v0.8.19:
Android-Bau prueft nicht, ob alle Tuerdateien in der APK landen; hier Teilpunkt N1 (Geraete-Entpacker).

Dieses Dossier wird laufend fortgeschrieben (Sitzungslimit-Regel).

## 0. Laufprotokoll

- [start] Dossier angelegt.
- [sitzung 2] Fortsetzung ab 1.3 (1.1/1.2 aus Sitzung 1 uebernommen, nicht neu gemessen). Entwurf
  `jni/asset_abgleich.h` wird geprueft und fertiggestellt.

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
| `release/apk_asset_gate.py` | `manifest_lesen` (= Regeln von `re15_abgleich_lesen`, auf Bytes), `manifest_pruefen` (Summe je Zeile == sha256 der APK-Daten), v1 abgelehnt, Selbsttest +21 Faelle, +55 innere Proben |
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
   | gueltig | nur in der alten Liste | - | Datei loeschen (Regeln wie jede Zeile: nie ausserhalb) |
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
  sha256 ueber `pfad\tgroesse\tsha` aller Zeilen gleich).

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

## 4. Gates / Suite

(folgt)

## 5. Offen

(folgt)
