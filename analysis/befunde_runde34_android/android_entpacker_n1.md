# N1 — Geraete-Entpacker update-sicher (Runde 34, Spur android_entpacker_n1)

Stand: 2026-09-30, Zweig r34a/android-gate, Arbeitsbaum .claude/worktrees/r34a_android.
Auftrag (Nutzer woertlich): "Gehe parallel den noch offenen Punkt an." — offener Punkt v0.8.19:
Android-Bau prueft nicht, ob alle Tuerdateien in der APK landen; hier Teilpunkt N1 (Geraete-Entpacker).

Dieses Dossier wird laufend fortgeschrieben (Sitzungslimit-Regel).

## 0. Laufprotokoll

- [start] Dossier angelegt.

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

(folgt: 1.3 Nachstellen im Emulator)

## 2. Bau (PORT-WAHL, kein Originalverhalten)

(folgt)

## 3. Nachweis im Emulator

(folgt)

## 4. Gates / Suite

(folgt)

## 5. Offen

(folgt)
