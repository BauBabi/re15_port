# Runde 39 Spur T "titelversion" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r39_titelversion`, Zweig `r39/titelversion`, HEAD `abcc5d17`, Basis `832b68bd`.
Pruefer: unabhaengige Abnahme 0 (2026-10-19). Alles unten selbst gemessen; Behauptungen des Bau-Agenten nur
uebernommen, wo ausdruecklich "nachgemessen" steht.

Nutzer-Punkt (AUFTRAG.md, Nachtrag 14, woertlich): "Eine letzte Ergänzung - bei dieser und zukünftig neuen Versionen
passe mir bitte die Version im Titelscreen an. Entferne das bestehende. Ich glaube dafür gibt es auch schon ein Skript
iregndwo."

Zerlegt in pruefbare Teile:
- T1 "Entferne das bestehende" (alte Beschriftung "Sample" / "Ver.2025.01.25" weg, sonst nichts veraendert)
- T2 "passe ... die Version im Titelscreen an" (die Version steht im Titelbild, an der Stelle der alten Zeile)
- T3 "bei dieser und zukuenftig neuen Versionen" (automatisch je Release: release_alles.sh -> Windows/Linux/Android)
- T4 "dafuer gibt es auch schon ein Skript irgendwo" (Suche + ehrliche Antwort)

## 0. Lesen / Diff (Stand der Pruefung)
- `git log --oneline master..HEAD`: 10 Commits b518d183..abcc5d17 (Dossier, Messung vorher, Titelbild, Weitergabe,
  Riegel, Dossier, Release-Riegel-Timeout, Android/Linux-Belege, messen.py, feat).
- `git diff master --stat` zeigt zusaetzlich AUFTRAG.md/UEBERGABE.md je -9 Zeilen: das ist master-Fortschritt nach der
  Basis 832b68bd (Nachtraege 15, J-Integration), KEINE Aenderung der Spur. `git diff master...HEAD --name-only` (Spur
  allein): 26 Dateien, siehe Abschnitt 7.2. master lief waehrend der Pruefung weiter (J integriert, 118 Commits seit
  der Basis); `git merge-tree --write-tree master HEAD` blieb konfliktfrei (7.2).

Stand: Pruefung abgeschlossen (Ergebnis Abschnitt 11).

## 1. Bau
`cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build`:
`=== LOCAL-BUILD-OK (configure)`, Configure-Zeile `-- RE15_VERSION (Titelbild): 'v0.8.26 dev' (git describe (letzter Tag) +
dev)`, Bau `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (exe vom Bau-Agenten 10:32 ist aktuell zum Quellstand
abcc5d17). `strings re15_pc.exe` enthaelt `v0.8.26 dev` und das Protokollformat `[titel_r39] ...`.

## 2. Datei-Belege selbst nachgemessen (RE-Gate: Byte-Offsets statt @0x)
Dieser Punkt hat kein Original-Verhalten (Beschriftung ist Teil der Bilddatei). Die Belege sind Datei-Byte-Offsets;
selbst geprueft (eigenes Skript `scratchpad/t_abn0/maske.py`, NICHT messen.py des Bau-Agenten):
- `sha256 re15_port/shared_assets/PSX/DATA/TITLEU.TIM` = `1acd3339ff6b...445c3` = `info/Re1.5/PSX/DATA/TITLEU.TIM`
  (Auslieferungsstand, kein Mod). `TITLEJ.TIM` = `b0d6733e...` wie im Dossier.
- Kopf `xxd -l 20`: `1000 0000 0200 0000 0c58 0200 c002 0000 4001 f000` = Magic 0x10, Flag 2 (16 bpp), 320 x 240.
- Zitat "V" Zeile 25 ab Datei-Byte 0x4020 (= 0x14 + 2*(25*320+198), nachgerechnet): `xxd -s 0x4020 -l 20` =
  `630c 3967 3967 3967 ef3d ef3d 3967 3967 3967 630c` -> Grau 3 25 25 25 15 15 25 25 25 3. Stimmt.
- Glyphen-Herkunft im .inc: "V" `Datei-Byte 0x3DA0` = 0x14 + 2*(24*320+198) (nachgerechnet), "e" `0x3DB2` =
  0x14 + 2*(24*320+207) (nachgerechnet). Ergaenzte Glyphen 3 4 6 7 8 9 d v sind als `ERGAENZT ... (PORT-WAHL, HD44780 A00)`
  gekennzeichnet.
- Komponenten im Kasten x188..295 y4..35 (8-Nachbarschaft, eigene Rechnung): Kastenrand 0 Nicht-Schwarz-Pixel;
  10 Komponenten, davon 8 grau mit Maximum 25 (S, l, a, m, "p"+"2025.01.25", e, V, "er.") und 2 Kunst-Reste
  (x204 y21: 1 Pixel, Max 1; x191..196 y22..25: 8 Pixel, Max 2). Maske (grau, Max >= 20) = **929 Pixel, bbox x198..283
  y11..33, Zeilen <=23: 357, >=24: 572**; alle STP 0. **Ring 608 Pixel, alle 0x8000.** -> Dossier 1.1 bestaetigt.
- Graue Pixel ausserhalb des Kastens (x150..319, y0..59): 124, alle in x150..193 y20..59, Maximum 8 = Hintergrund-Ader
  links; keine Beschriftung ausserhalb des Kastens.
- Nur `s_boot_title` (= `DATA/TITLEU.TIM`, platform/pc/main.c:3452) laeuft durch `re15_render_pc_show_title`
  (Aufrufe main.c:3540/3652/3717); andere 320x240-Bilder (C_BACK2.TIM, YOUDIED.TIM) haben eigene Wege -> der Haken
  trifft nur das Titelbild.

## 3. T1 "Entferne das bestehende" + T2 "Version im Titelscreen" — gemessen an der exe
Lauf (eigene Messkopie `build/platform/pc/re15_pc_tabn0_<id>.exe` neben der exe, danach geloescht; eigener Laufordner
`scratchpad/t_abn0/lauf_lokal_1792399863`; keine RE15_*-Variable im Aufrufer, `env | grep -c ^RE15_` = 0):
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=titel.bmp
RE15_TITLE_SHOT_AF=600 RE15_INV_SHOT=ende34.bmp timeout 180 re15_pc_tabn0_<id>.exe` -> rc 0 nach 8 s.
- debug.log: `[titel_r39] alte Beschriftung entfernt: 929 Pixel; neu "Ver.0.8.26 dev" (RE15_VERSION "v0.8.26 dev")
  Spalten 200..283`
- Auswertung `scratchpad/t_abn0/abzug.py` (eigenes Skript) Abzug gegen rohes TITLEU.TIM (RGB555 << 3):
  - ausserhalb des Kastens x188..295 y4..35 und ausserhalb des Menues (x80..240 y130..225): **0 Abweichungen**
    (Logo, TM, Bildrand, Hintergrund unveraendert); im Menuebereich 2217 (NEW GAME/LOAD GAME/OPTION, pulsierend = erwartet)
  - Zeile "Sample" (y11..23) im Kasten: **0 nicht-schwarze Pixel** ausser dem Kunst-Staub (204,21)=(8,8,8) und
    x191..196 y22..25 (8 Pixel) - die beide unveraendert bleiben (gehoeren zur Hintergrund-Ader, nicht zur Schrift)
  - Kasten gesamt: 508 nicht-schwarze Pixel, bbox x191..283 y21..33, alle grau, Maximum 200 (= 25 << 3)
  - ASCII der Zeilen 24..33 (x195..289) liest "Ver.0.8.26 dev":
```
25       .@@@++@@@.                   -@@@-     -@@@-     .@@@+   .@@=        @+
27       .@-  +%   %%%= -%.%%-      -@. %@+   -@.  @+    +.  +%-@+.      :%%-@+ %%%= -%.  #+
29       .@-=##  .@#++#%-@%  #=     -@*%-%+   :#####-      =%= -@####-  -@. #@*@#++#%-@.  @+
31       .@@+    .@+-----@.    +@%  -@=--%++@%-@=--%++@% =@+----@=--%+  -@=--@*@+---- :@+@=
32       .@=      .@@@@%-@.    +@%   =@@@= +@% =@@@= +@%.@@@@@% =@@@=    =@@@@+.@@@@%  .@-
```
  - Sichtpruefung der Vergroesserung (x176..303 y0..47, 6x; oben roh, unten Abzug) `scratchpad/t_abn0/zoom_lokal.png`:
    oben "Sample" / "Ver.2025.01.25", unten nur "Ver.0.8.26 dev" in derselben Pixelschrift, rechtsbuendig an der alten
    Kante, TM und Logo gleich.
- **Urteil T1: erfuellt.** **Urteil T2 (lokaler Bau): erfuellt** - die lokale exe zeigt ehrlich "Ver.0.8.26 dev"
  (Orchestrator-Vorgabe der Spur: lokaler Bau = letzter Tag + " dev").

## 4. T4 "Ich glaube dafuer gibt es auch schon ein Skript irgendwo" — selbst gesucht
Behauptung des Bau-Agenten (Dossier 1.3 und 7 "Fuer den Nutzer"): "Es gab keins - weder in diesem Repo noch im alten
reAi-Ordner ... Die Version kam bisher nur in die Paket-Dateinamen und in die APK, nicht ins Spiel."
**Das ist widerlegt.** Es GAB ein solches Skript - im Vorprojekt, und die Belege liegen in DIESEM Repo:
1. `analysis/save_system_patched_build.md:59` (getrackt): ISO-Vergleich der gepatchten Disc des
   Vorprojekts `re15_save_final` gegen `info/Re1.5/PSX`: `| /PSX/DATA/TITLEU.TIM | 1101 | 153620 | 2118 B — kosmetisch
   (Titelgrafik), nicht Save |`; ebenso Zeile 1098 "Zusaetzlich ist DATA/TITLEU.TIM geaendert (kosmetisch)".
   Erzeugt von `c:/workspace/git/reAi/scripts/patch_save_final.py` (`save_system_patched_build.md:276`; `CLAUDE.md` Abschnitt "Zwei
   Datenquellen"; `RE15_KNOWLEDGE.md` 8.4: "`scripts/patch_save_final.py` (5355 lines) — main build script ...
   keep all build mutations inside patch_save_final.py").
2. Selbst gemessen (nur lesend, `scratchpad/t_abn0/patched_title.py`): `TITLEU.TIM` aus
   `info/Re1.5/re15_save_final.bin` (Hauptbaum, ungetrackt) ab LBA 1101 (2352-B-Rohsektoren, Nutzdaten +24),
   sha256 `4d00f0d7...829b`, gleicher Kopf; **2118 Bytes = 1059 Pixel verschieden, bbox x178..283 y11..34** - genau
   der Kasten der alten Beschriftung. Dort ist "Sample" / "Ver.2025.01.25" auf `0x8000` gesetzt und eine neue Zeile
   y18..24 in einer 5x7-Pixelschrift (Grau 20) gestempelt. Sie lautet **"V1.18 2026.04.24"** (Vergroesserung
   `scratchpad/t_abn0/zoom_vorprojekt.png`, oben Auslieferung, unten Vorprojekt-Disc; ASCII:
```
18  #   #  #        #    ###       ###   ###   ###    ##       ###     #       ###     #
19  #   # ##       ##   #   #     #   # #   # #   #  #        #   #   ##      #   #   ##
20  #   #  #        #   #   #         # #  ##     # #         #  ##  # #          #  # #
21   # #   #        #    ###        ##  # # #   ##  ####      # # # #  #        ##  #  #
22   # #   #        #   #   #      #    ##  #  #    #   #     ##  # #####      #    #####
23    #    #   ##   #   #   #     #     #   # #     #   # ##  #   #    #  ##  #        #
24    #   ###  ##  ###   ###      #####  ###  #####  ###  ##   ###     #  ##  #####    #
```
   ).
3. Notiz des Vorprojekts (Claude-Memory des Nutzers, `C:/Users/mjoedicke/.claude/projects/C--workspace-git-reAi/memory/
   versioning.md`, nur gelesen): "v1.1 (finalisiert, 2026-03-07) ... - Title screen shows "v1.1  2026.03.07"", "v1.2 ...
   - Title screen shows "v1.1  2026.03.16"", Regeln: "Script: `scripts/patch_save_final.py`", "Output:
   `info/Re1.5_PATCHED/re15_save_final.bin`"; `feedback_java_reference.md:8`: "Bei JEDEM Minor oder Major Version-Update:
   1. `scripts/patch_save_final.py` aktualisieren (Patches + Version string)"; `manage_phase_b_impl.md:78`:
   "Version string: "v1.17 2026.03.30"".
4. Das Skript selbst existiert nicht mehr: `C:/workspace/git/reAi` enthaelt nur `psx_dev/re15_reborn_pc/build/`
   (ls -laR); in reAi_v2 kein Commit mit `*patch_save_final*` (`git log --all -- '*patch_save_final*'` leer);
   `find /c/workspace/git -maxdepth 4 -name "patch_save_final*"` leer. Die Java-Referenz
   `src/main/java/de/re15/patches/PatchReference.java` (v1.21, getrackt) enthaelt das Titel-Stempeln nicht (grep TITLEU/
   title/version: nur Patch-Versionen).
- **Urteil T4: nicht_erfuellt.** Der Nutzer hatte recht ("es gibt schon ein Skript"); Spur T sagt ihm das Gegenteil.
  Die Umsetzung selbst ist davon nicht beruehrt (das Skript ist weg und war ein Disc-/Asset-Patch, den der Spur-Auftrag
  ausschliesst: "KEIN Asset-Patch - TITLEU.TIM bleibt bytegleich"). Zu korrigieren ist die Auskunft (Mangel M1).
  Hinweis: auch die Orchestrator-Messung in AUFTRAG.md Nachtrag 14 ("Ein vorhandenes Skript dafuer gibt es weder hier
  noch im alten reAi-Repo") ging von derselben falschen Annahme aus - der Bau-Agent hat sie uebernommen, statt den
  Repo-Belegen (Punkt 1) nachzugehen.

## 5. Gezielte Tests (keine volle Suite im Spurbaum)
`ctest --test-dir re15_port/build -R "r39_titelversion|r38_releaseskript|r35_android|r34a_asset_abgleich|r30_android|r30_titel|boot_bg_pin|dark_start_pin" --timeout 1200 --output-on-failure -j4`
(eigene 10 Riegel + Titel-Haken r30_titel_puls/r30_titel_blinken/boot_bg_pin/dark_start_pin + alle Release-/Android-
Riegel, weil der Diff release/*.sh und build.gradle beruehrt):
**100% tests passed, 0 tests failed out of 30** (Total Test time 514 s; u. a. integration_r39_titelversion_titel 12 s,
unit_r39_titelversion_ablauf 31 s, unit_r39_titelversion_huelle 10 s, unit_r35_android_bash_mutanten 514 s,
integration_r30_titel_puls 28 s). Kein roter Fenster-Haken, kein Nachfahren noetig.
Weitere Pruefungen:
- `python3 re15_port/tools/r39_titelversion/glyphen_bauen.py --pruefen` -> `OK: ...titelversion_r39_glyphen.inc entspricht
  dem Generator` (rc 0; gen/ nicht von Hand geaendert).
- Riegel messen wirklich: `glyphen` vergleicht die Ausschnitte bytegleich mit TITLEU.TIM; `maske` prueft 929/bbox/
  357+572/Ring 608 x 0x8000 und Kunstfaelle; `anwenden` prueft ausserhalb Maske+Kacheln bytegleich und TITLEU.TIM auf
  der Platte unveraendert; Integration: echter Abzug, ausserhalb des Kastens gegen das ROHE TITLEU.TIM (unabhaengig vom
  neuen Code), Zeile "Sample" schwarz, Protokollzeile mit 929 und der CMake-Version. Die Bildgleichheit "Abzug ==
  Erwartung" nutzt dieselbe Funktion wie die exe (teilweise selbstbestaetigend) - die Form der Glyphen sichern aber die
  Unit-Riegel gegen die Datei ab.
- Registrierung nur ueber `tests/unit/probes/r39_titelversion.cmake` (GLOB `tests/unit/CMakeLists.txt:4060`), keine
  Test-CMakeLists geaendert.

## 6. T3 "bei dieser und zukuenftig neuen Versionen" — Release-Kette
### 6.1 Code gelesen
- `release/release_alles.sh` Schritte windows/linux: `env RE15_VERSION="$NEU" bash build_win_cross.sh` bzw.
  `... build_linux_deck.sh`; android unveraendert `build_android.sh --version $NEU`.
- `build_win_cross.sh`/`build_linux_deck.sh`: `--version V` oder Umgebung, sonst Host-`git describe --tags --abbrev=0
  --match 'v[0-9]*'` + " dev"; `ENV_WEITER+=RE15_VERSION` -> `release/docker/kopie_lauf.sh:60-61`
  (`envargs+=( -e "$n=${!n}" )`) -> `docker run -e RE15_VERSION=...`.
- Container: `release/docker_win_build.sh:93` und `docker_linux_build.sh:70` rufen bei JEDEM Lauf `cmake -S ... -B ...`
  (Quellbaum frisch per tar, Bauordner wxbuild/lbuild in der NIE-Liste) -> `cmake/re15_version_r39.cmake` liest
  `$ENV{RE15_VERSION}` bei jedem Release neu (kein klebriger Cache-Wert).
- Android: `release/build_android.sh:193` (ohne --version `git describe --tags --always`), `:311` `rm -rf app/.cxx`,
  `:316` `-Pre15Version="$VERSION"` -> `app/build.gradle:220` `arguments "-DRE15_VERSION=" + re15Version`; Gradle baut die
  Wurzel-CMakeLists (`build.gradle:228` path portRoot/CMakeLists.txt) -> engine/CMakeLists.txt -> Modul.
### 6.2 Trockenlauf (selbst gefahren)
`timeout 300 bash release/release_alles.sh --dry-run` im Spurbaum -> rc 0, `TROCKENLAUF-OK v0.8.27 ... nichts gebaut,
nichts geschrieben` (git status danach nur dieser Bericht, kein release/logs):
```
Version: v0.8.27  (hoechster Tag v0.8.26 + 1)
  2/6 windows   RE15_VERSION=v0.8.27 bash release/build_win_cross.sh -> release/win_out/re15_pc.exe ...
  3/6 android   bash release/build_android.sh --version v0.8.27 -> release/re15_port_v0.8.27_android.apk ...
  4/6 linux     RE15_VERSION=v0.8.27 bash release/build_linux_deck.sh -> release/linux_out/re15_pc ...
```
-> Anzeige im Release "Ver.0.8.27" (Riegel text: "v0.8.27" -> "Ver.0.8.27").
### 6.3 Echter Windows-Release-Weg (selbst gefahren, nicht nur Attrappe)
`bash release/build_win_cross.sh --version v9.9.9` im Spurbaum (Docker Desktop 27.5.1, Image re15-wincross-build:deb11)
-> rc 0 nach 49 s; Protokoll `scratchpad/t_abn0/wincross.log`:
`== RE15_VERSION (Titelbild): 'v9.9.9' ==` (Huelle), im Container `-- RE15_VERSION (Titelbild): 'v9.9.9' (Umgebung
RE15_VERSION)`, `[171/490] Building C object engine/CMakeFiles/re15_engine.dir/src/titelversion_r39.c.obj` (0 Warnungen
zu titelversion_r39.c), `WIN-CROSS-BUILD-OK: 5616046 Bytes`; `release/win_out/re15_pc.exe` frisch (11:00:04), `strings`
enthaelt `v9.9.9`. Diese Release-exe als Messkopie neben die exe gelegt (Asset-Wurzel per exe-Ordner,
`[asset] exe-dir: .../re15_port/build/platform/pc`), gleicher Titelabzug wie Abschnitt 3 (Laufordner
`scratchpad/t_abn0/lauf_xwin_1792400443`), danach Kopie und `release/win_out` geloescht:
- debug.log `[titel_r39] alte Beschriftung entfernt: 929 Pixel; neu "Ver.9.9.9" (RE15_VERSION "v9.9.9") Spalten 229..283`
- gegen rohes TITLEU.TIM: ausserhalb Kasten + Menue **0 Abweichungen**; Zeile "Sample" leer; Kasten 325 graue Pixel,
  bbox x191..283 y21..33 (Kunst-Staub unveraendert); ASCII:
```
25                                   .@@@++@@@.                   -@@@-     -@@@-     -@@@-
28                                    .@- -#=  +#++*=-@*++*-     :####@+   :####@+   :####@+
31                                    .@@+    .@+-----@.    +@%   .-=@= +@% .-=@= +@% .-=@=
```
- Sicht `scratchpad/t_abn0/zoom_xwin_v999.png`: unten nur "Ver.9.9.9", rechtsbuendig an der alten Kante.
=> Der Windows-Release-Weg (build_win_cross.sh --version / Umgebung -> docker -e -> CMake -> exe -> Titelbild) ist
am Artefakt belegt. release_alles.sh ruft ihn mit `RE15_VERSION=$NEU` auf (6.1, 6.2, Riegel ablauf).
### 6.4 Linux / Android
- Linux: dieselbe Weitergabe (`ENV_WEITER`, `docker_linux_build.sh:70` Configure je Lauf); Riegel huelle startet das
  ECHTE build_linux_deck.sh mit docker-Attrappe und findet `-e RE15_VERSION=...`; ein voller build_linux_deck.sh-Lauf
  (volle Suite im Container, Stunden) wurde hier nicht gefahren - der Bau-Agent belegt einen Container-Configure mit
  `-e RE15_VERSION=v9.9.9` (Dossier 4/4.2). Wegen identischer Mechanik zum gemessenen Windows-Weg ausreichend.
- Android: Wurzel-CMakeLists `add_subdirectory(engine)` immer (`CMakeLists.txt:66`, Modul wird geladen);
  `platform/android/jni/CMakeLists.txt:45-63` libmain = re15_engine + platform/pc/src/*.c + platform/pc/main.c ->
  derselbe Titel-Code (render_pc.c re15_render_pc_show_title). Weitergabe 6.1. Kein Geraete-Abzug (wie beim Bau-Agenten
  offen); Kompilier-Beleg des Bau-Agenten (libmain.so beider ABIs mit v9.9.9) nicht nachgefahren.
- **Urteil T3: erfuellt** (Windows am Artefakt gemessen, Linux/Android ueber dieselbe Mechanik + Riegel + Code).

## 7. Gates
### 7.1 RE-Gate
- Kein Original-Verhalten betroffen (die Beschriftung ist Bildinhalt, kein Code; Port-Darstellung vorher 1:1 aus der
  Datei, render_pc.c alte Zeile `rgb555_to_argb8888(tim->pixels[i])`). Daher keine @0x-Adressen; jede Konstante traegt
  einen Datei-Beleg (Byte-Offset / gemessene Pixelzahl) oder ist als NUTZER-VORGABE / PORT-WAHL markiert
  (14 Marken im Diff; Kopf `include/re15_titelversion_r39.h`, `cmake/re15_version_r39.cmake`, Glyphen ERGAENZT).
- Stichproben (statt Disassembly, weil Byte-Offsets zitiert sind) selbst mit xxd gelesen:
  1. "V" y25 @0x4020 = `630c 3967 3967 3967 ef3d ef3d 3967 3967 3967 630c` (3 25 25 25 15 15 25 25 25 3) = Dossier 2 und
     .inc-Zeile y25 von k_r39t_g_V. Stimmt.
  2. "V" y24 @0x3DA0 = `0080 630c 630c 630c 2104 2104 630c 630c 630c 0080` (0 3 3 3 1 1 3 3 3 0) = .inc y24. Stimmt.
  3. "e" y27 @0x3DB2+3*0x280 = 0x4532: `0080 2104 d65a d65a d65a 8c31 0080` (0 1 22 22 22 12 0) = .inc y27. Stimmt.
  4. (zusaetzlich) "5" y25 @0x3E3E+0x280 = 0x40BE: `4a29 3967 x5 ef3d` (10 25 25 25 25 25 15) = .inc y25. Stimmt.
  5. TITLEJ.TIM im Kasten: 14 Nicht-Schwarz-Pixel (Dossier 2: 14). Stimmt.
- Suche im Diff (`git diff master...HEAD -- re15_port release`) nach deferred/tunable/interim/for now/faithful/
  plausibel/TODO/FIXME/approx: **0 Treffer**; kein neuer `getenv`-Schalter im Engine-/Plattform-Code.
- Erklaert der Fix den Befund? Ja: die Beschriftung ist Pixelinhalt der Datei (929 Pixel), der Haken ersetzt genau diese
  Pixel durch den gemessenen Hintergrund 0x8000 und zeichnet die Version; Abzug vorher/nachher ausserhalb des Kastens
  bytegleich zur Datei.
### 7.2 Vertrag
- Bank-9-Bits, Nachrichten-Ids, Ereignisse: keine belegt (grep im Diff leer) - passt (Spur T hat keine Zuteilung).
- Pfade (`git diff master...HEAD --name-only`, 26 Dateien): eigene Dateien (titelversion_r39.c/.h/.inc, cmake-Modul,
  Riegel, Werkzeuge, Dossier, T_belege). Gemeinsame Dateien nur als kleine Haken mit Kommentar "Runde 39 Spur T":
  `engine/CMakeLists.txt` 1 Zeile, `platform/pc/src/render_pc.c` 1 include + 3 Zeilen. `release/` (release_alles.sh,
  build_win_cross.sh, build_linux_deck.sh) und `platform/android/app/build.gradle` (3 Zeilen) - eigentlich TABU (VERTRAG
  1.3), hier aber vom Orchestrator ausdruecklich erlaubt: `orchestrator/r39.js:132` "AUSNAHME VERTRAG 1.3: diese Spur
  darf in release/ NUR die Versions-Weitergabe aendern (release_alles.sh, build_win_cross.sh, build_linux_deck.sh,
  build_android.sh, Android-Gradle-Argumente) - nichts an Paket, Gates, Archiv; RELEASE_NOTES.md bleibt tabu"; UEBERGABE
  08:50 ebenso. Diff-Inhalt geprueft: nur Versions-Weitergabe (Option --version, Umgebung, ENV_WEITER, Plan-/Kopftext,
  distrobox-Durchreichung, Gradle-Argument); make_package.sh, docker_*_build.sh, Gates, Archiv, RELEASE_NOTES.md,
  shared_assets/PSX unveraendert; keine Test-CMakeLists geaendert (Registrierung ueber probes/, GLOB
  tests/unit/CMakeLists.txt:4060).
- Riegel je Punkt: T1 (maske, anwenden, Integration "Sample weg"), T2 (glyphen, text, layout, Integration Protokoll +
  Bild), T3 (version, cmake, huelle, ablauf). T4 hat keinen Riegel (Auskunft).
- Parallel-/Linux-fest: Integration mit Zufalls-Laufordner + exe-Kopie `re15_pc_r39t_<id>`, RE15_* geloescht
  (`test_r39_titelversion_titel.cmake`), Skript-Riegel loeschen geerbte RE15_* (`test_r39_titelversion_release.sh:19`,
  `test_r39_titelversion_cmake.cmake:16-17`); `cmake_minimum_required` im -P-Skript; keine libm-Funktionen.
- Integrationsfaehigkeit: `git merge-tree --write-tree master HEAD` rc 0 und gegen `r39/integration` rc 0; keine
  Datei der Spur wird von master/r39-integration seit der Basis veraendert (comm leer).

## 8. Zusatzlaeufe (Nebenwirkungen)
- Release-Fall "volle Suite im Linux-Container mit gesetzter Umgebung" nachgestellt:
  `RE15_VERSION=v0.8.27 ctest --test-dir re15_port/build -R "r39_titelversion|r38_releaseskript" --timeout 1200 -j4`
  -> **100% tests passed, 0 tests failed out of 17** (die Riegel loeschen geerbte RE15_* selbst; ausser den Spur-Dateien
  liest niemand RE15_VERSION - `git grep RE15_VERSION` ausserhalb: nur Haken/Weitergabe; die exe hat kein
  getenv("RE15_VERSION")).
- Tod -> Titel (in-process, Titeltextur einmal je Prozess) und Titel -> Laden:
  `ctest -R "relatch_pin|save_counter_pin|weste_load_pin" --timeout 600` -> **100% tests passed, 0 tests failed out of 3**.
- Andere sichtbare Versionsangaben: Fenstertitel "RE1.5 Rebuilt — PC" (render_pc.c:644, ohne Version);
  `RE15_REBORN_VERSION_STRING` (re15_engine.h:24) wird nirgends angezeigt. Nichts Veraltetes uebrig.
- Beide Szenarien: das Titelbild liegt vor der Figurenwahl (Leon/Elza gemeinsam); kein Szenario-Code beruehrt.

## 9. Urteile
| Teil | Urteil | Beleg |
|---|---|---|
| T1 "Entferne das bestehende" | **erfuellt** | Abschnitt 3: Zeile "Sample" leer, "Ver.2025.01.25" weg (929 Pixel -> 0x8000, Ring 608/608), ausserhalb Kasten+Menue 0 Abweichungen zur Datei; TITLEU.TIM bytegleich |
| T2 "Version im Titelscreen" | **erfuellt** | Abschnitt 3 (lokal "Ver.0.8.26 dev", ASCII + Bild), 6.3 (Release-exe "Ver.9.9.9"), Pixelschrift der alten Zeile, rechtsbuendig an der alten Kante |
| T3 "bei dieser und zukuenftigen Versionen" | **erfuellt** | 6.1 Code, 6.2 Trockenlauf (v0.8.27 an windows/linux/android), 6.3 echter Container-Bau -> Titelbild "Ver.9.9.9"; Riegel huelle/ablauf/cmake/version gruen, auch mit geerbter Umgebung |
| T4 "dafuer gibt es schon ein Skript irgendwo" | **nicht_erfuellt** | Abschnitt 4: das Vorprojekt-Skript `reAi/scripts/patch_save_final.py` hat je Version "vX.Y Datum" ins Titelbild geschrieben (Disc re15_save_final: "V1.18 2026.04.24", TITLEU.TIM 1059 Pixel; Belege im Repo + Vorprojekt-Notiz versioning.md). Die Spur sagt dem Nutzer "Es gab keins" |

Gates: RE-Gate haelt (Datei-Belege selbst nachgelesen, keine Rate-Signale), Vertrag haelt (release/ + build.gradle nur
im erlaubten Umfang der Orchestrator-Ausnahme), Riegel messen, 30 + 17 + 3 gezielte Tests gruen, Windows-Release-Weg
am Artefakt belegt.

## 10. Maengel
**M1 (T4, Auskunft an den Nutzer sachlich falsch).** `T_titelversion.md` Abschnitt 1.3 ("Ergebnis: es gab kein Skript
dafuer") und Abschnitt 7 ("Es gab keins - weder in diesem Repo noch im alten reAi-Ordner ... Die Version kam bisher nur
in die Paket-Dateinamen und in die APK, nicht ins Spiel") sind widerlegt. Nachpruefbar:
- `analysis/save_system_patched_build.md:59` und `:1098` (TITLEU.TIM in der gepatchten Disc geaendert, 2118 B);
  `RE15_KNOWLEDGE.md` 8.4 (`scripts/patch_save_final.py` = "main build script"); `CLAUDE.md` (Save-Patch aus
  `reAi/scripts/patch_save_final.py`).
- `info/Re1.5/re15_save_final.bin` (Hauptbaum, nur lesen) LBA 1101, 2352-B-Sektoren, Nutzdaten +24, 153620 B:
  1059 Pixel bbox x178..283 y11..34 anders als die Auslieferung; dort steht "V1.18 2026.04.24" statt
  "Sample / Ver.2025.01.25" (Skript `scratchpad/t_abn0/patched_title.py`, Bild `zoom_vorprojekt.png`).
- `C:/Users/mjoedicke/.claude/projects/C--workspace-git-reAi/memory/versioning.md`: "Title screen shows "v1.1
  2026.03.07"", "Script: `scripts/patch_save_final.py`".
Abhilfe (nur Dokument, kein Code): Dossier 1.3 und 7 berichtigen - dem Nutzer sagen: Ja, im Vorprojekt gab es das
(patch_save_final.py stempelte bei jeder Version "v<Version> <Datum>" direkt in die Titelbild-Datei der gepatchten Disc,
zuletzt "V1.18 2026.04.24"); das Skript selbst ist nicht mehr vorhanden (C:/workspace/git/reAi enthaelt nur noch
psx_dev/re15_reborn_pc/build/), nur sein Ergebnis in `info/Re1.5/re15_save_final.bin`; warum nicht wiederverwendet
(Datei-Patch einer gepatchten Disc, Spur-Auftrag verlangt TITLEU.TIM bytegleich; Skript weg) und dass der Port dasselbe
jetzt beim Laden und automatisch je Release macht. Dabei die Form nennen: Vorprojekt "V1.18 2026.04.24" (Version +
Datum, eigene 5x7-Schrift, eine Zeile), jetzt "Ver.0.8.27" in der Schrift der alten Zeile.
(Hinweis an den Orchestrator: dieselbe falsche Annahme steht in AUFTRAG.md Nachtrag 14, "Ein vorhandenes Skript dafuer
gibt es weder hier noch im alten reAi-Repo" - dort ebenfalls berichtigen.)

Hinweise ohne Mangel-Rang:
- H1: Ein lokaler Bau genau auf dem Release-Tag zeigt "Ver.0.8.27 dev" (Regel "letzter Tag + dev" laut Spur-Auftrag);
  das Release-Paket zeigt "Ver.0.8.27".
- H2: Android ohne Geraete-/Emulator-Abzug (Code-Identitaet + Kompilier-Beleg des Bau-Agenten); Linux nicht als voller
  build_linux_deck.sh-Lauf gefahren (gleiche Mechanik wie der gemessene Windows-Weg).
- H3: RE15_MIN_TESTS (+10) und RELEASE_NOTES.md bei der Integration (gemeinsame bzw. tabu Dateien).

## 11. Ergebnis
**Nicht bestanden** - einziger Mangel M1 (falsche Auskunft zum vorhandenen Skript; reine Dossier-Berichtigung). Die
Umsetzung selbst (alte Beschriftung weg, Version im Titelbild, automatisch je Release) ist am gebauten Stand und an einer
echten Release-exe gemessen und in Ordnung.
