# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`reAi_v2` ist der bewusst aufgeräumte Neu-Aufbau des Resident-Evil-1.5-Projekts (unveröffentlichter RE2-PSX-Prototyp). Vorgänger: `C:/workspace/git/reAi` (war zu groß/unübersichtlich). Das Projekt hat zwei Teile:

1. **`re15_port/` — der 1:1-Port (PRIMÄRER FOKUS).** Eine gemeinsame C-Engine für PC (SDL2/OpenGL) und PSX (PSn00bSDK), gebaut über ein vereinheitlichtes CMake-System. Vollständig spezifiziert unter `.kiro/specs/re15-port-setup/` (`requirements.md`, `design.md`, `tasks.md`).
2. **`src/main/java/de/re15/` — der Java-Asset-Extraktor.** Extrahiert und konvertiert PSX-Original-Assets in moderne Formate; dient als Referenz-/Werkzeug-Pipeline für den Port. Little-Endian (PSX-Architektur), PSX-Koordinatensystem (Y-Achse invertiert ggü. OpenGL).

## RE-Disziplin (hart)

### ⛔ STOP-GATE — das hier ZUERST, bei JEDER Verhaltens-/Render-/Timing-/State-Änderung

Diese Regel existiert, weil ich genau hier WIEDERHOLT versagt habe (2026-07: Card-Fade `fa+=24` = geratene Framezahl; „blau→schwarz" ohne den Übergangs-Mechanismus zu RE'en; ein Message-Clear-„Fix", der den Spieler-Freeze gegen ein Save-Menü-Flackern tauschte, weil die Examine/`msg_block`-Interaktion NICHT zu Ende disassembliert war). Der Nutzer weist — zu Recht — JEDE Änderung ohne Disasm-Zitat zurück und ist massiv genervt vom Raten. „Nicht mehr blau" ≠ byte-true.

**Auslöser:** ein Report „X ist falsch / nicht wie im Original / immer noch nicht korrekt", ODER jede Konstante/Rate/Dauer/Farbe/Frame-Zahl/Reihenfolge, die Rendering, Timing oder Game-State betrifft.

**Pflicht-Reihenfolge — NICHT abkürzen, egal wie „klar" das Symptom scheint:**
1. **MESSEN/REPRODUZIEREN**, was tatsächlich passiert (Log/Screenshot/Savestate-RAM). NICHT modellieren, NICHT vermuten — wenn mein Kopf-Modell sagt „kann nicht sein", ist das Modell falsch, also reproduzieren.
2. **Die verantwortliche ORIGINAL-Funktion disassemblieren** (`re15_disasm.py` / `ghidra1_V2.txt` / `RE_15_Quellcode_*` / RE2-Vergleich `ghidra_re2_Leon.txt`). Die **Adresse + die exakten Instruktionen/Bytes im Chat POSTEN, BEVOR** ich eine Zeile Fix-Code vorschlage.
3. **Erst dann** implementieren — JEDE Konstante trägt im Code-Kommentar UND in der Commit-Message ihre `@0x…`-Adresse (oder Datei-Byte-Offset). Konstante ohne `@0x…` = Rate-Defekt = kommt nicht rein.
4. **Verifizieren** durch erneutes Messen gegen die Disasm-Werte (nicht gegen „sieht besser aus").

**Mechanismus nicht gefunden = NICHT fertig.** Weiter-RE'en (andere Xrefs, Caller/Callee, RE2, Savestate-RAM). NIE die Lücke mit einer Zahl nach Gefühl oder einem als byte-true verkauften Platzhalter füllen.

**Guess-Tells — schreibe/denke ich das, SOFORT STOP + RE:** „plausibel", „sieht richtig aus", „nicht mehr blau/kein X mehr", runde Zahl nach Gefühl (6/11/32 Frames), „interim/for now/tunable", „faithful-line" als Ersatz für einen Beleg, „das Symptom ist weg".

**Selbst-Check vor JEDEM Edit einer Verhaltens-Konstante:** „Kann ich JETZT die `@0x…`-Adresse zeigen?" Nein → nicht schreiben. **Nutzer-Hebel:** für jede Konstante das `@0x…`-Zitat verlangen; fehlt es → zurückweisen. (Siehe Memory `reai-v2-re-gate`.)

---

Beim byte-true Reverse Engineering gelten diese Regeln verbindlich und überschreiben jedes Default-Verhalten:

- Jede verhaltens-/format-relevante Konstante (Clip-Index, Offset, Threshold, Distanz, Flag) MUSS eine Disassembly-Adresse oder einen Datei-Byte-Offset zitieren. Kein Beleg → kommt nicht in den Code, wird **nicht** als "interim/tunable/approximiert" verkauft.
- **Niemals raten und niemals aufgeben.** Läuft der erste RE-Weg in die Sackgasse → NICHT abbrechen, sondern den Weg wechseln und weitermachen, bis es gefunden ist:
  - Statisch: andere Einstiegspunkte, Dispatch-Table-/String-/Daten-Xrefs, Caller/Callee verfolgen, dieselbe Logik in verwandten Binaries vergleichen (RE2 Retail, BioClone/lib_bio/BioModels unter `info/`).
  - Dynamisch: DuckStation-Savestate-RAM-Dumps + `psx_dev/re15_reborn/psxtest.sh` + Runtime-Logging — live beobachten, was der Wert (z.B. `entity[0x94]`) tatsächlich wird.
  - Datengetrieben: EMD/EDD/Format-Bytes direkt parsen, Clips per Render-Katalog identifizieren, Asset-Reihenfolge differenziell vergleichen.
  - Agenten parallel auf die Wege ansetzen, wenn der Suchraum groß ist.
- Den Nutzer nur als **letztes Mittel** fragen — nachdem die Wege ausgeschöpft sind — und dann mit "Ich habe X/Y/Z versucht, der nächste Weg ist W". Standard = weitermachen.
- "Sieht richtig aus" (Positionen/Screenshots == `./original`) ist **kein** Beweis für Clips/Formate/Mechanismus. Den Mechanismus belegen, nicht das Ergebnis.
- Vor "fertig/verifiziert/byte-true": Kann ich für JEDE Behauptung die exakten Bytes/Instruktionen zeigen? Wenn nein → nicht fertig, weiterarbeiten (keine falsche Fertigmeldung, kein Abbruch).

### RE-Quellen — zuerst HIER nachschauen, niemals raten

**PRIMÄR — Overlay-Decompilate (Spiel-/Raum-/AI-Logik, geladen @`0x80100000`):** Hier ZUERST nachschauen.
- `RE_15_Quellcode_Overlays/` — die dekompilierten RE1.5-Stage-Overlays: `STAGE{1..6}_overlay.c` (ganzes Overlay je Stage) sowie `STAGE{N}/FUN_*.c` und `STAGE{N}_full/FUN_*.c` (einzelne Overlay-Funktionen, `0x8010xxxx`). Erste Anlaufstelle für Raum-/Gegner-/Event-Logik.
- `RE2_Quellcode_Overlays/STAGE{1..7}/` — RE2-Retail-Overlays als Architektur-Referenz, wenn RE1.5 unklar ist.
- Entity-Dispatch-Tabelle in der EXE @`0x80072bac` mappt Typ→Overlay-Handler. STAGE1 = Briefing-Room (room1140-Zombies).

**FALLBACK — EXE-Decompilate (Engine-Infrastruktur, `~0x8001xxxx–0x800Fxxxx`):** nur wenn die gesuchte Logik NICHT in den Overlays liegt.
- `ghidra1_V2.txt` (~92 MB) — vollständiger Ghidra-Dump der RE1.5 `PSX.EXE` (Disasm + Daten).
- `RE_15_Quellcode_V2/FUN_<addr>.c` — 1258 einzeln decompilierte EXE-Funktionen (z.B. `FUN_80012d60.c` = Spieler-Damage-Entry); `RE2_Quellcode_V2/` analog für RE2.
- `ghidra_re2_Leon.txt` (~88 MB), `info/re2leon/` — RE2-Retail-Leon, nahezu identische Engine.
- `RE15_FUN_CATALOG.md` (Repo-Root) — kuratierter Katalog code-verifizierter `FUN_80xxxxxx`-Adressen mit Subsystem/Zweck/Evidence/Confidence. Vor manuellem RE IMMER hier prüfen (Skill: `ghidra-mapping`).

**FEHLT etwas (auch in den Overlays nicht enthalten) → selbstständig ermitteln, niemals raten:**
- Ghidra headless auf `info/Re1.5/PSX/BIN/STAGE{1..6}.BIN` (+ `DEBUG.BIN`/`TITLE.BIN`; 0x800-Byte-Header, Code lädt @`0x80100000`), PSX-Loader + PsyQ-Signaturen. (Der Decompile-Dump-Helfer lag im alten `reAi`-Repo unter `scripts/` und kam nicht mit — ggf. neu anlegen.)
- `stage_saves/` + DuckStation-Savestate-RAM-Dumps — live beobachten, was ein Wert (z.B. `entity[0x94]`) tatsächlich wird.

⛔ **Zwei Datenquellen im Repo sind NICHT der Auslieferungsstand — wer daraus misst, misst einen Mod:**
- `stage_saves/PATCHED-EXE_HASH-881C08B8082E53B6_*.sav` (6 Stück) laufen auf einer **gepatchten** EXE.
  Prüfbar: `@0x80026e4c` ist dort `24 c2 01 08` (`j 0x80070890`) statt des Auslieferungs-Stubs
  `08 00 e0 03` (`jr ra` / `addu v0,zero,zero`). Die übrigen ~78 Savestates sind sauber.
- `info/Re1.5/re15_save_final.bin` (+ `.cue`) ist das **gepatchte Disc-Image** des Vorprojekts
  (`t_size=0xb1000` statt `0xaf000`; der Save-Patch aus `reAi/scripts/patch_save_final.py`).
  Der Auslieferungsstand ist `info/Re1.5/PSX.EXE` und der Baum darunter (`info/Re1.5/PSX/`).
  Hintergrund: `analysis/save_system_patched_build.md`. Der Auslieferungsstand kann **nicht speichern**
  — 21 RDTs enthalten wörtlich „Save is not available in this preview".

**Format-Referenz (statt Formate zu raten):**
- `RE15_KNOWLEDGE.md` — alle Dateiformate (§1.1 RDT, §1.2 SCD, §1.3 BSS, §1.4 EMD, §1.5 PLD, §1.6 TIM, §1.7 ESP, §1.8 VAB, … bis §1.19). Das ist die Format-Referenz in reAi_v2. (Das alte `DOCUMENTATION.md`/`docs/` wurde nicht übernommen.)
- `info/Resident_Evil_und_Playstation_Information/`: `psx-spx.github.io-master` (kanonische PSX-Hardware-Spec), `BioClone Redux*`/`bioclone-remake-main`/`BioModels-master` (RE-Engine-RE-Referenz), `RE15Data`/`RE15Editor`/`re1.5-specific` (RE1.5-spezifisch), `information*.txt` (Format-Notizen), `PSYQ_SDK`/`PSn00bSDK-*` (SDK-Header + `psyq*.gdt`), `3dgraph.pdf` (GTE/3D).

**Dynamisch (wenn statisch nicht reicht):**
- `psx_dev/re15_reborn/psxtest.sh` — DuckStation-Selbsttest (≥40 s, graceful close) + Log lesen.
- DuckStation-Savestate-RAM-Dumps (DUCCS/zstd; RAM-Base via `PSX.EXE[0x800:0x840]`-Suche) — live lesen, was ein Wert (z.B. `entity[0x94]`) tatsächlich wird.

## Build & Run

### C-Port (`re15_port/`) — primär, verifiziert 2026-08-24 (224/224 Tests grün, lokal + Docker)

Toolchain auf dieser Maschine: **mingw64 GCC 15.2** (`C:\msys64\mingw64\bin`) + **Ninja** + **CMake ≥3.21**. Kein MSVC/Clang.

#### ⛔ Aus Git-Bash IMMER über `local_build.sh` bauen

```bash
bash re15_port/tools/local_build.sh            # configure + build + test (Default: all)
bash re15_port/tools/local_build.sh configure  # | build | test | clean | env
```
Verifiziert 2026-08-24: **224/224 Tests grün**, ~128 s, Abschlusszeile `=== LOCAL-BUILD-OK (all) — Tests 224/224`.

**Warum ein blankes `cmake --build` aus Git-Bash scheitert** (gemessen, nicht vermutet):
Git-Bash injiziert `/mingw64/bin` = `C:/Program Files/Git/mingw64/bin` selbst an PATH-Position 2,
`C:/msys64/mingw64/bin` steht erst an ~40. `cc1.exe` liegt **nicht** in `mingw64/bin` (sondern in
`lib/gcc/x86_64-w64-mingw32/15.2.0/`) und findet seine DLLs deshalb nur über den PATH — anders als
`ld`/`as`/`gdb`, die durch ihr eigenes Verzeichnis geschützt sind. Gits
`libwinpthread-1.dll` (2024-07-29) exportiert **`clock_gettime64` nicht**, GCC 15.2 `cc1` importiert
es → `0xC0000139 STATUS_ENTRYPOINT_NOT_FOUND`. Symptom: `gcc` beendet mit **exit 1 und 0 Byte stderr**,
also ein **stiller** Tod ohne Fehlermeldung. `local_build.sh` setzt einen minimalen PATH mit msys64
zuerst und misst cc1 im Preflight. (Historie: Das wurde am 2026-08-23 als „Host-Compiler defekt →
nur noch Docker" fehlgedeutet; Reboot und pacman-Reinstall halfen nicht, weil die Toolchain nie
kaputt war. Siehe `HANDOVER_2026-08-23.md` §0a5.)

⚠️ `option(RE15_BUILD_TESTS … OFF)` ist der Default (`re15_port/CMakeLists.txt:100`). Ohne
`-DRE15_BUILD_TESTS=ON` meldet ctest „No tests were found!!!" und beendet mit **EXIT 0** — falsches
Grün. `local_build.sh` setzt die Option und bricht ab, wenn ctest keine Summenzeile liefert.

**PowerShell ist NICHT betroffen** (gemessen: dessen PATH enthält `C:\Program Files\Git\cmd`, aber
nicht `…\Git\mingw64\bin`). Dort gehen die rohen Befehle direkt:

```powershell
# Konfigurieren (PC-Target). RE15_ASSETS_PATH ist optional:
# Default = re15_port/shared_assets/PSX (in-repo, dort liegen ALLE Assets).
cmake -S re15_port -B re15_port/build -G Ninja -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON
cmake --build re15_port/build
ctest --test-dir re15_port/build --timeout 30
```

```bash
# Headless-Smoke (lädt Startraum ROOM1240, gibt JSON-Status auf stdout)
re15_port/build/platform/pc/re15_pc.exe --headless
```

- **Alle Spiel-Assets** liegen in `re15_port/shared_assets/PSX/` (vollständiger CD-Baum, alle 6 Stages + BIN/DATA/DOOR/EMD/ITEM/MOVIE/PLD/SOUND/VOICE). Der Port liest **ausschließlich** von dort (CMake-Default für RE15_ASSETS_PATH). Original-Quelle: `info/Re1.5/PSX`. Extraktions-Intermediates gehören nach `build/extracted/`, nicht in `shared_assets/`.
- PSX-Target: `-DRE15_BUILD_PSX=ON -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake -DPSN00BSDK_PATH=<sdk>` (PSn00bSDK nötig).
- CMake nutzt GLOB für `engine/src/*.c` und `platform/*/src/*.c` → neue Quelldateien werden beim nächsten Configure automatisch erfasst.

### Java-Extraktor (`src/`)

⚠️ `build.gradle`/`settings.gradle` fehlen derzeit im Root — `./gradlew build|run|test` läuft erst, wenn diese wiederhergestellt sind. Einstieg: `de.re15.Main` → `de.re15.extractors.RE15MasterExtractor` (7 sequentielle Phasen). Hex-Inspektion: `xxd <file>`.

## Skills (Domänenwissen)

Die Domänen-Skills liegen doppelt:
- `.kiro/skills/` — Originale (Kiro-IDE-Format).
- `.claude/skills/<name>/SKILL.md` — für Claude Code als `/slash`-Skill nutzbar: `rdt-analysis`, `scd-disassembly`, `ghidra-mapping`, `build-workflow`, `asset-pipeline`.

## Directory Structure

- `re15_port/` — der C-Port (engine/, include/, platform/{pc,psx}/, tools/, tests/, cmake/, shared_assets/, CMakeLists.txt)
- `src/main/java/de/re15/` — Java-Asset-Extraktor
- `info/Re1.5/` — RE1.5-Original-CD-Daten (STAGE1-6, BIN, DATA, DOOR, EMD, ITEM, MOVIE, PLD, SOUND, VOICE)
- `info/Resident_Evil_und_Playstation_Information/` — Referenzdoku & verwandte Projekte (BioClone, BioModels, psx-spx, SDKs, …)
- `RE_15_Quellcode_Overlays/` (**primär**: Stage-Overlays, Spiel-/Raum-/AI-Logik) und `RE_15_Quellcode_V2/` (**Fallback**: EXE-Engine-Funktionen) — decompilierte RE1.5-Quellen; `RE2_Quellcode_Overlays/` / `RE2_Quellcode_V2/` analog für RE2-Retail
- `ghidra*_V2.txt`, `ghidra_*_Leon.txt` — Ghidra-Dumps (RE1.5 EXE, RE2-Leon)
- `voice/`, `synchro/` — Audio-Rohmaterial; `stage_saves/` — Savestate-Snapshots (für Vergleichsmodus)
- `RE15_KNOWLEDGE.md`, `RE15_FUN_CATALOG.md`, `RE15_FUN_COVERAGE.md`, `UNTESTED_IMPLEMENTATIONS.md`, `ASSET_LAYER_REWRITE.md` — Wissensbasis (Repo-Root)

## Java-Extraktor — Architektur (`src/`)

### Extraktions-Phasen (RE15MasterExtractor)
1. **Copy Source Files** — nicht-prozessierte Dateien in die Output-Struktur kopieren
2. **Room Data (RDT, BSS)** — RDT-Raum-Container, BSS-MDEC-Video-Hintergründe
3. **Scripts (SCD)** — SCD-Bytecode zu C-Code disassemblieren
4. **Model Containers (PLD, PLW, EMS)** — in EDD, EMR, MD1, TIM splitten
5. **Textures (TIM, PIX, ITP)** — zu BMP konvertieren
6. **3D Models** — MD1 zu OBJ, EMD/PLD zu glTF
7. **Audio & Effects (ESP, VAB)** — ESP zu EFF, VAB zu WAV

### Key Extractors
- `RDTExtractor` — Room Definition Table (zentraler Raum-Container)
- `BssExtractor` — Hintergrund-Videoframes (VLC + MDEC)
- `SCDScriptDisassembler` — Script-Bytecode zu C-Code
- `PldExtractor` / `PldBlenderExporter` — Spielermodelle
- `EmdGltfExtractor` — Gegnermodelle mit Skelett/Animationen zu glTF
- `DO2Extractor` — Tür-Archive
- `SoundExtractor` / `VabToWavConverter` — PSX-ADPCM-Audio

### Fertige Konverter (nicht ändern)
TIM→BMP, PIX→TIM→BMP, ESP→EFF sind finalisiert und korrekt.

## Technical Notes

- Bone-Rotationen in EMR sind 12-bit gepackt (4096 = 360°)
- PSX→glTF-Koordinaten: `Y' = -Y` (Y invertiert)
- Quaternion-Rotationsreihenfolge: ZYX Euler
- CD-Sektor-Alignment: 2048 Bytes
- MDEC-VLC-ID: 0x3800
- Fixkomma im Port: 20.12 (Q12), `4096 = 1.0`; Yaw 0–4095 = 360°
