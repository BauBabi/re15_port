# PSX-Ziel — Stand, Bauen, offener Blocker

> **Kurzfassung:** Das PSX-Ziel **baut und linkt jetzt** (vorher gar nicht) und erzeugt ein
> bootfähiges CD-Image. Es **läuft aber noch nicht auf der Konsole**: das Programm belegt
> ~262 KB mehr RAM, als eine PlayStation hat. Was das kostet und woran es liegt, steht in §4.

## 1. Bauen

```cmd
re15_port\targets\psx\build.cmd
```

Ergebnis in `targets/psx/build/`:

| Datei | Inhalt |
|---|---|
| `re15.exe` | PS-EXE (532 KB) |
| `re15.bin` / `re15.cue` | bootfähiges CD-Image (3 MB) |
| `re15.elf` | ELF mit Symbolen (für `nm`/`size`) |

Voraussetzungen: PSn00bSDK 0.24 und der `mipsel-none-elf`-GCC. Beide werden automatisch
unter `C:\PSn00bSDK\` gesucht; abweichende Orte über `PSN00BSDK_ROOT` / `PSN00BSDK_GCC`.
`mkpsxiso` bringt das SDK selbst mit.

## 2. Warum ein eigenes Projekt statt `platform/psx/CMakeLists.txt`

Der alte Pfad ging von PSn00bSDK **0.1x** aus (flaches `$SDK/include` + `$SDK/lib`) und
suchte über `cmake/FindPSn00bSDK.cmake`. Version 0.24 hat ein anderes Layout **und** bringt
eine eigene CMake-Integration mit:

- Toolchain-Datei `lib/libpsn00b/cmake/sdk.cmake`
- Funktionen `psn00bsdk_add_executable()`, `psn00bsdk_add_cd_image()`, `psn00bsdk_target_incbin()`

Statt das Find-Modul umzubauen — und damit die laufende Basis anzufassen — benutzt
`targets/psx/CMakeLists.txt` direkt das offizielle SDK-Modell. Die Engine wird dabei **flach**
mitkompiliert (nicht über das Host-Target `re15_engine`, das für den PC konfiguriert ist).
Der alte Pfad bleibt unberührt liegen.

## 3. Was nötig war, damit es überhaupt übersetzt

### 3.1 Hosted-C-Lücken (gelöst, ohne die Basis zu ändern)

Die gemeinsamen Engine-Quellen benutzen an einigen Stellen `getenv`, `fopen`, `fprintf`,
`FILE`, `stderr` — eine PlayStation hat weder Dateisystem in der libc noch eine Umgebung,
PSn00bSDK stellt sie folgerichtig nicht bereit.

Gelöst über `compat/re15_psx_compat.h`, das im PSX-Build per `-include` **vor** jede
Übersetzungseinheit gezogen wird. Die Ersatz-Semantik ist bewusst gewählt:

| Funktion | Verhalten auf PSX | Wirkung |
|---|---|---|
| `getenv()` | immer `NULL` | alle `RE15_*`-Debug-Schalter sind aus — genau richtig |
| `fopen()` | immer `NULL` | Datei-Diagnosen nehmen ihren Fehlerzweig (alle Aufrufer prüfen auf NULL) |
| `fprintf()` | leitet auf `printf` um | Diagnosen landen auf der PSX-TTY statt zu verschwinden |
| `fclose/fflush/fread/fwrite` | Nulloperationen | — |

### 3.2 Eine Zeile in der Basis (unvermeidbar, nachweislich folgenlos)

`engine/src/game_step_common.c` deklarierte die Trig-Funktionen lokal ein zweites Mal:

```c
extern int32_t re15_sin_q12(int), re15_cos_q12(int);   /* vorher */
extern int     re15_sin_q12(int), re15_cos_q12(int);   /* jetzt  */
```

`re15_skeleton.h` (in derselben Datei eingebunden) deklariert sie als `int`. Auf dem PC ist
`int32_t` **identisch** mit `int`, deshalb fällt der Widerspruch dort nie auf. Auf MIPS ist
`int32_t` ein `long int` — ein anderer Typ, und der Compiler bricht mit
*conflicting types* ab. Es gibt keinen Compiler-Schalter dagegen.

Belegt, dass die Angleichung auf dem PC folgenlos ist: eine Übersetzungseinheit mit **beiden**
Deklarationen (`int32_t f(int);` und `int f(int);`) übersetzt mit der PC-Toolchain
fehlerfrei — die Typen sind dort dasselbe. Gegenprobe: PC-Build neu gebaut, **110/110 Tests
grün**.

### 3.3 Mitgelinkte Binärdaten

Das PSX-Backend erwartet einige Assets fest einkompiliert. Sie werden über
`psn00bsdk_target_incbin` aus dem echten Asset-Baum eingebettet:

| Symbol | Quelle | Zweck |
|---|---|---|
| `test_md1` / `test_edd` / `test_emr` | `PLD/PL00.*` | Spielermodell (Leon) |
| `pl00w01_edd` / `pl00w01_emr` | `PLD/PL00W01.*` | Waffen-Animationsbank |
| `test_bss` | `BSS/ROOM1170/BG00.BSS` | Rückfall-Hintergrund bei CD-Lesefehler |
| `test_vh` / `test_vb` | **Platzhalter (4 Byte)** | siehe unten |

**Offen:** Für die „gebündelte" SFX-Bank (`load_bundled_vab`) gibt es im Asset-Baum kein
entpacktes VH/VB-Paar — die VAB-Köpfe stecken in den `.EDH`-Containern (Kopf-Offset im
8-Byte-Trailer, so liest sie auch das PC-Backend zur Laufzeit) bzw. in `DOOR00.DO2`. Bis das
Entpacken im Build steht, wird ein 4-Byte-Platzhalter eingebettet: `re15_vab_parse` schlägt
darauf fehl, `load_bundled_vab` kehrt früh zurück, alles andere läuft. Die **echten**
Raum-Bänke (snd0/snd1) kommen ohnehin aus dem RDT von der CD.

## 4. Der offene Blocker: RAM

Die PlayStation hat 2 MB (`0x80000000`–`0x80200000`), das Programm lädt ab `0x80010000`.

```
text  337 872
data  191 100
bss 1 770 932
---------------
    2 299 904 Byte     _end = 0x8024181C
```

**262 KB über der Grenze.** Die größten statischen Puffer:

| Symbol | KB | Wo |
|---|---:|---|
| `s_enemy_arena` | 472 | `platform/psx/src/asset_psx.c` (`RE15_ENEMY_ARENA_SIZE`) |
| `s_room_rdt` | 320 | `platform/psx/src/re15_room.c` |
| `re15_psx_staging_buf` | 264 | `include/re15_cdfs.h` (`RE15_PSX_STAGING_SIZE`) |
| `s_ctx` | 136 | Audio-/Dekoder-Kontext |
| `g_enemy` | 124 | Gegner-Bank-Registry (`RE15_ENEMY_MAX`) |
| `re15_cd_staging` | 100 | `include/re15_cdfs.h` |

Beide Größen-Makros sind feste `#define` **ohne** `#ifndef`-Schutz, lassen sich also nicht
von außen per `-D` verkleinern. Das Budget zu schließen heißt, diese Puffer in der Basis zu
verkleinern (oder Regionen zu teilen) — das war für diesen Arbeitsschritt ausdrücklich
ausgeschlossen und ist der nächste Schritt, wenn das PSX-Ziel wirklich laufen soll.

Zur Einordnung: Die Kommentare an `re15_cd_staging` / `re15_psx_staging_buf` halten fest, dass
ein früherer Versuch, die beiden Staging-Puffer zusammenzulegen, den Hintergrund-Dekoder
zerstört hat — die 364 KB sind also nicht ohne Weiteres zu haben.

## 5. CD-Image: aktueller Umfang

Das erzeugte Image enthält bisher nur `SYSTEM.CNF` + `RE15.EXE`. Der volle Asset-Baum
(283 MB) passt zwar auf eine CD, hat aber **174 Verzeichnisse** — die PSX erlaubt maximal 45
(und höchstens 30 Einträge im Wurzelverzeichnis). Ursache sind die per-Raum-Unterordner unter
`BSS/`. Vor dem Ausliefern müssen die zusammengefasst werden (z. B. ein Ordner je Stage mit
`R1170_00.BSS`-artigen Namen), was auch die Pfadbildung in `bg_psx.c` berührt. Solange das
Programm ohnehin nicht in den RAM passt, ist das der zweite Schritt.
