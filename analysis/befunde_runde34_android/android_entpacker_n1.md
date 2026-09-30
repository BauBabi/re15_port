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

(folgt)

## 3. Nachweis im Emulator

(folgt)

## 4. Gates / Suite

(folgt)

## 5. Offen

(folgt)
