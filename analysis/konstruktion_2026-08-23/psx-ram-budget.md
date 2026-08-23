# PSX-RAM-Budget + CD/SPU-Plan

# PSX-HARDWARE-TARGET — RAM-Budget, CD-Layout, SPU, Streaming (Analyse + Plan)

## 1) MECHANISMUS — wie das ORIGINAL mit 2 MB auskommt (alle Adressen zitiert)

### 1.1 Gemessener Ist-Stand des Ports (aus `targets/psx/build/re15.elf`, `mipsel-none-elf-size`/`nm`, Build 2026-08-05)

```
text 337 872   data 191 100   bss 1 770 932   → _end = 0x8024181C
Überhang über 0x80200000 (2 MB retail): 0x4181C = 268 316 B (~262 KB)
```
Linker erzwingt das Limit nicht: PSn00bSDK `exe.ld` definiert `APP_RAM ORIGIN 0x80010000 LENGTH 0x7f0000` (8 MB Dev-Konsole) — darum „linkt, läuft nicht".

### 1.2 Original-RAM-Ökonomie (RE1.5 PSX.EXE, Decompiles `RE_15_Quellcode_V2/`)

Die Original-Engine hält fast NICHTS statisch resident; alles Große läuft durch **feste Regionen + einen Bump-Cursor**:

**(a) Stage-Overlay + RDT + Raum-Runtime = EIN Bereich ab 0x80100000, Deckel 0x80190000:**
- Stage-Wechsel: `FUN_80039a30`: `DAT_800bbeac = FUN_800299a4(2, stage+1); DAT_800ac780 = &DAT_80100000 + DAT_800bbeac;` — d.h. STAGE\<N\>.BIN lädt @0x80100000 (Dateigrößen: STAGE1=137 648 B … STAGE6=10 324 B), und die **RDT-Basis = 0x80100000 + Overlay-Größe** (dynamisch!).
- Raum-Laden `FUN_800396fc`: `DAT_800ac778 = DAT_800ac780; FUN_80013b60(file_id_tab[PTR_DAT_8007438c[stage] + room*2], DAT_800ac778, 1, …)` — die **ganze RDT wird an die RDT-Basis gelesen**, danach werden die Header-Offsets +0x08..+0x60 in-place zu Pointern relativiert (`*piVar1 += DAT_800ac778`-Schleife ebd.).
- Danach wird `DAT_800ac77c = *(int*)(DAT_800ac778 + 0x10)` gesetzt = **Bump-Cursor hinter den residenten RDT-Teil**; alle Raum-Runtime-Allokationen bumpen ihn:
  - Sprite-Arbeitsbereich: `FUN_80039270`: `DAT_800ac77c += n*4 + n*0x20 + n*0x20` (n = RDT-Byte +7).
  - SHITAI.TIM: `FUN_80039c2c`: `FUN_80013b60(0x29, DAT_800ac77c, 1, "SHITAI")` → VRAM-Upload `FUN_80022150(2,…,0xf,8)` → `DAT_800ac77c += size` (CD→Bump→VRAM, RAM bleibt nur belegt was resident sein muss).
  - SFX-VB-Body wird **durch den Cursor NUR DURCHGESCHLEUST**: `FUN_800440c4`: EDH-Kopf → `&DAT_801fbd00` (4-KB-Slot oben), `SsVabOpenHeadSticky(head,4,0x38840)`, Body `FUN_80013b60(tab@0x80073ab0[id], DAT_800ac77c, 1, "CORE VBD")` + `SsVabTransBody` → SPU; RAM-Kopie wird nicht behalten.
- Harter Deckel dieses Bereichs: **0x80190000**, denn dort liest jeder Raum die Hintergründe (siehe b). Effektives Raum-Budget = 0x90000 (589 824 B) minus Overlay: STAGE1 ≈ 442 KB, STAGE6 ≈ 566 KB. Größte RDT auf der Disc: ROOM6030.RDT = 368 996 B; Median 167 132 B (240 RDTs gemessen).

**(b) Hintergrund-Streaming, feste Puffer 0x80190000/0x80198000/0x80199e00:**
- `FUN_8001d600` (Raum-Betreten): `DAT_800be574 = size…; DAT_800be57c = LBA(file@DAT_8006f440[idx]) + cut*0x20; FUN_80013c50(&DAT_80190000, 2, &DAT_800106b0)` — Direkt-LBA-Read der Raum-BSS, **Cut-Slot-Stride = 0x20 Sektoren = 64 KB** (bestätigt durch Disc: `STAGE1/ROOM117.BSS` = 851 968 B = 13×65 536).
- Cut-Wechsel `FUN_80021bbc`: erneut `FUN_80013c50(&DAT_80190000,0,…)`, dann `FUN_80053a8c(&DAT_80072f2c, &DAT_80190000, &DAT_80199e00, &DAT_80198000, 0)` + `StoreImage(&DAT_80072f2c, &DAT_80198000)` — MDEC-Kette Quelle @0x80190000, Arbeits-/VLC-Bereich @0x80199e00, Slice-/Pixel-Puffer @0x80198000 (0x80199e00−0x80198000 = **0x1E00 = 7 680 B = 16 px × 240 px × 16 bpp** — exakt der Port-`s_slice_buf` mit 1920 Words, `bg_psx.c:86`).
- 0x80198000 ist zugleich generischer TIM-Scratch: `FUN_8001613c`: `FUN_80013b60(0x24, &DAT_80198000, 0, "YOUDIED.TIM")` → `FUN_8004ee78(&DAT_80198000)`.

**(c) Player/Waffe: feste residente Regionen, von CD geladen (nichts einkompiliert):**
- PLD: `FUN_800314b0`: `FUN_80013b60(*(u16*)(&DAT_80073f70 + char_id*2), &DAT_801bd814, 1, …)` — Spielermodell resident @0x801bd814 (deckt sich mit KNOWLEDGE §2.4 „0x801BD800 Animation dispatch table").
- PLW: `FUN_80036b68`: `FUN_80013b60(base_tab@0x800741e8[char]*2 + weapon, &DAT_801d7700, 0)` + `OpenTIM/LoadImage` der dir[3]-TIM — Waffenbank resident @0x801d7700.

**(d) Audio: nur Köpfe resident, Bodies im SPU; komplette SPU-Karte zitierbar:**
Alle `SsVabOpenHeadSticky`-Stellen der EXE (VAB-id → SPU-Adresse):

| id | SPU-Adresse | Loader | Inhalt |
|---|---|---|---|
| 0 | 0x1020 | `FUN_800170e0` | System-Bank |
| 1 | 0x7330 | `FUN_80043d8c` | Waffen/ARMS |
| 2 | 0x2f3d0 | `FUN_80043eac` | Raum-snd-Bank 0 (aus `FUN_800396fc`) |
| 3 | 0xffd0 | `FUN_80043fb0` | Raum-snd-Bank 1 (aus `FUN_800396fc`) |
| 4 | 0x38840 | `FUN_800440c4` | CORE-SFX („CORE EDH/VBD", Datei-Tabellen @0x80073a88/@0x80073ab0) |
| 5 | 0x42fc0 | `FUN_80044564` | BGM MAIN-VB (VH-Staging @&DAT_801f5500) |
| 6 | 0x42fc0 + DAT_800bbdec | `FUN_80044774` | BGM SUB-VB, direkt hinter MAIN; VH/SEQ resident @&DAT_801eed00 (`SsSeqOpen(&DAT_801eed00 + …)`) |

RAM-seitig bleiben nur: EDH/VH-Köpfe in 4-KB-Slots oben (&DAT_801fbd00, &DAT_801fcd00 via `FUN_80013b60(tab@0x8007492c[i], &DAT_801fcd00,…)`), BGM-VH+SEQ @0x801eed00, Sound-Globals @0x801ff404 (33 Xrefs). Voice ist im Original CD-XA (SCD-Op 0x5E, Memory `reai-v2-scd-stub-audit`), belegt also **gar kein** SPU/RAM-Streaming-Budget.

**(e) Gegner-Modelle:** Die Disc hat KEINE EM\*.EMD-Einzeldateien — nur `EMD/CDEMD0.EMS` (4 732 928 B) + `CDEMD1.EMS` (self-describing Blob-Kette, sektor-aligned, `re15_ems.h`-Kopf = byte-true Port des Java-Splitters). Der Original-Ladepfad (welche FUN liest welchen Blob wohin) ist NICHT gepinnt — siehe OFFEN.

## 2) PORT-STAND

### 2.1 Build-System
- **Aktiv:** `re15_port/targets/psx/` — eigenständiges CMake-Projekt gegen PSn00bSDK 0.24 (`psn00bsdk_add_executable`, `psn00bsdk_add_cd_image`), Engine flach mitkompiliert, Compat-Shim `compat/re15_psx_compat.h` per `-include` (getenv→NULL, fopen→NULL, fprintf→printf). Build: `targets\psx\build.cmd`. Artefakte vorhanden: `build/re15.exe` (532 480 B), `re15.bin/.cue` (3 MB, nur SYSTEM.CNF+EXE+1024 Dummy-Sektoren laut `iso.xml`), `re15.elf` (Symbole), **`re15.map` existiert** (79 KB).
- **Alt/tot:** `platform/psx/CMakeLists.txt` + `cmake/FindPSn00bSDK.cmake`/`psx_toolchain.cmake` (0.1x-Layout, findet 0.24 nicht; README §2) — liegen lassen.

### 2.2 BSS-Top-20 (aus `nm --size-sort`, exakt)

| # | Symbol | Bytes | Definition |
|---|---|---:|---|
| 1 | `s_enemy_arena` | 483 328 (0x76000) | `platform/psx/src/asset_psx.c:164-165` |
| 2 | `s_room_rdt` | 327 680 (0x50000) | `platform/psx/src/re15_room.c:22` |
| 3 | `re15_psx_staging_buf` | 270 336 (0x42000) | `include/re15_cdfs.h:24` / `bg_psx.c:85` |
| 4 | `s_ctx` (Render: 2×(64-KB-Paketpuffer+4-KB-OT+Envs)) | 139 440 (0x220b0) | `platform/psx/src/render.c:60-70` |
| 5 | `g_enemy` (4 Bänke × 31 680) | 126 720 (0x1ef00) | `include/re15_enemy.h:34,74` |
| 6 | `re15_cd_staging` | 102 400 (0x19000) | `include/re15_cdfs.h:36` / `re15_cdfs.c:11` |
| 7 | `s_voice_buf` | 51 200 (0xC800) | `audio_psx.c:262-264` |
| 8/9 | `s_bgm_main`/`s_bgm_sub` (ssx_seq_t, je ~31 KB davon eingebettete `re15_vab_t`) | 2×32 216 | `audio_psx.c:365-366` |
| 10/11 | `s_sfx_vab`/`s_foot_vab` (`re15_vab_t`, dominiert von `tones[2048]`×14 B=28 672) | 2×31 016 | `audio_psx.c:129,136`; Struct `include/re15_vab.h` |
| 12 | `g_scd` | 14 112 | engine |
| 13 | `re15_obj_md1[6]` | 10 896 | `asset_psx.c` |
| 14 | `s_esp_fx` | 10 368 | engine |
| 15 | `g_actors` | 8 384 | engine |
| 16/17 | `vh_copy.0` / `s_main_seq` | je 8 192 | `audio_psx.c:595,369` |
| 18 | `s_slice_buf` | 7 680 | `bg_psx.c:86` |
| 19 | 4× `re15_*_anim` (`re15_emd_animation_t`, davon `frames[1664]`=6 656) | 4×7 180 | `include/re15_emd.h:49,67` |
| 20 | `g_aot` 5 644; `s_msg_raw/_text` 2×4 096; `_isr_stack` 4 096 | — | — |

.data-Blobs (incbin, `targets/psx/CMakeLists.txt:73-95`): `test_bss` 65 536, `test_emr` 57 136, `test_md1` 28 916, `pl00w01_emr` 17 768, `test_edd` 3 160 (+`pl00w01_edd`) ≈ 173 KB des .data-Segments.

### 2.3 Zwei bei der Analyse gefundene LATENTE DEFEKTE (unabhängig vom RAM)
1. **PSX-Gegnerlader kann nie laden:** `asset_psx.c:234` baut `"\\EMD\\EM%02X.EMD;1"` — diese Dateien existieren weder auf der Original-Disc noch in `shared_assets/PSX/EMD/` (dort nur CDEMD0/1.EMS + SHITAI). Der PC-Port liest korrekt per `re15_ems_get_entry` aus der EMS (pc/main.c); der PSX-Pfad liefe in den Negative-Cache → alle Gegner dauerhaft als Leon.
2. **`s_room_rdt` (0x50000=327 680) ist kleiner als die größten RDTs:** ROOM6030.RDT=368 996, ROOM6031=363 876, ROOM2001=360 660 — STAGE2/5/6-Räume würden am `maxbytes`-Guard von `re15_cd_load_file` scheitern.
3. (Bereits im Audit: `platform/psx/main.c:757` Letterbox-24 stale ggü. FUN_80021a0c-Zähler.)

## 3) IMPLEMENTIERUNGS-PLAN

### 3.1 RAM-Abbau (Ziel: _end ≤ ~0x801F0000, d.h. −334 KB inkl. 64 KB Stack-Marge; die Boot-Kollision aus `re15_enemy.h:27-33` zeigt, dass Marge Pflicht ist)

| Schritt | Maßnahme | Ersparnis | Beleg der Machbarkeit | Aufwand |
|---|---|---:|---|---|
| A | **Raum-Arena vereinigen**: `s_room_rdt`+`s_enemy_arena` (811 008 B) → EINE Arena `RE15_ROOM_ARENA 0x90000` (589 824 B): RDT an den Anfang (fixt zugleich Defekt 2.3-2), Gegner/RBJ/Elliot bumpen dahinter — exakt das Original-Modell `DAT_800ac77c` | **−221 184** (Option 0x80000: −286 720, s. Verifikation) | Original: Overlay+RDT+Bump passen in 0x80100000..0x80190000 (`FUN_80039a30`/`FUN_800396fc`/`FUN_8001d600`); Port braucht den Overlay-Anteil (10-137 KB) nicht | M — `re15_room.c` (Arena statt eigenem Puffer), `asset_psx.c` (`re15_arena_put` auf Arena-Rest hinter RDT, Reset bei Raumwechsel behält RDT) |
| B | **Gegner-Staging in die Arena**: EMD/EMS-Blob in den freien Arena-Tail lesen statt in `re15_psx_staging_buf`; danach Staging auf VLC-Bedarf schrumpfen: 0x42000→0x28000 (163 840; „BG VLC needs only ~155 KB", `bg_psx.c:83`) | **−106 496** | Original-BSS-Kette braucht nur 0x80190000..0x801a0000 (64 KB) für Quelle+Slice; unsere VLC-Zwischenform ist größer, 160 KB reichen laut eigenem Messkommentar | S (nach A) |
| C | **`s_voice_buf` streichen**: Voice-CD-Read → `re15_cd_staging` (102 400 ≥ 51 200), SpuWrite von dort (transient wie Original-VBD durch den Bump-Cursor, `FUN_800440c4`) | **−51 200** | `re15_audio_voice_flush`-Serialisierung existiert schon (`audio_psx.c:319-326`) | S |
| D | **`test_bss`-Fallback (incbin) streichen** — Original hat keinen In-EXE-Hintergrund; Fehlerpfad = Retry/Schwarz | **−65 536** (.data) | CMakeLists:73-74 + `bg_psx.c` Fallback-Zweig | S |
| E | **`re15_vab_t` kompaktieren**: `tones[2048]` (28 672 B) auf reale Maximal-Programmzahl dimensionieren; VORHER messen: numProgs aller RDT-snd-VHs + ARMS\*.EDH + MAIN\*.BGM-VHs (Skript über `shared_assets`; VH-Format §1.9). Guard `#ifndef RE15_VAB_PROGRAM_COUNT` einführen (README §4 moniert fehlende Guards), PSX-Build setzt per `-D` | bis **−86 016** (4 Instanzen: s_sfx, s_foot, 2× in ssx_seq_t) | Original hält nur den ROHEN VH (≤6,7 KB, `vh_copy`-Kommentar `audio_psx.c:595`) resident | M |
| F | (Später, optional) `re15_emd_animation_t.frames[1664]` → Pointer in die residenten EDD-Bytes | ~−133 000 (20 Instanzen à 6 656) | EDD liegt in Arena/PLD ohnehin resident | L (engine-weit, PC-Retest 224 ctests) |

A+B+C+D = **−444 416 B** → _end ≈ 0x801D5000, ~172 KB unter der 2-MB-Decke (Stack + Boot-Transienten gedeckt). E/F sind Reserve für STAGE2-6.

### 3.2 CD-Layout (löst das 174-Verzeichnis-Problem vollständig)
Die **Original-Disc hat nur 16 Verzeichnisse** (Root + BIN/DATA/DOOR/EMD/ITEM/MOVIE/PLD/SOUND/STAGE1-6/VOICE, `info/Re1.5/PSX/`) — ISO-konform (≤45 Dirs, ≤30 Root-Einträge). Plan: `iso.xml` referenziert `shared_assets/PSX` 1:1 in Original-Struktur; Port-Pfade angleichen:
1. `re15_room.c:32`: `\RDT\ROOM%04X.RDT` → `\STAGE%d\ROOM%04X.RDT` (d = room_id>>12).
2. `bg_psx.c:190`: `\BSS\ROOM%04X\BG%02d.BSS` (Extraktions-Artefakt, 174 Dirs) → `\STAGE%d\ROOM%03X.BSS` + **Cut-Offset-Read**: neue API `re15_cd_load_at(path, sector_off, sectors, buf)` (CdSearchFile → `fp.pos`+off → CdlSetloc → CdRead); Cut-Slot = 0x20 Sektoren (**@FUN_8001d600 `DAT_800be57c = LBA + cut*0x20`**, Disc-verifiziert ROOM117.BSS=13×64 KB).
3. `asset_psx.c` Gegnerlader: `\EMD\EM%02X.EMD` → `\EMD\CDEMD0.EMS` + `re15_ems_index_for_type`/`re15_ems_get_entry` (Offset+Länge liegen vor; Blobs sind sektor-aligned → derselbe `re15_cd_load_at`) — fixt Defekt 2.3-1.
4. `targets/psx/CMakeLists.txt`: `test_vh/test_vb`-Platzhalter ersetzen durch CD-Stream der ARMS/CORE-EDH (Kopf-Offset im 8-Byte-Trailer, Mechanismus komplett RE'd — Memory `reai-v2-bgm-container`; Loader-Vorbild `FUN_800440c4`).

### 3.3 Streaming-Architektur (was WANN von CD kommt)
- Raum-Betreten: RDT (Median 167 KB) → Arena-Kopf; snd-Bank-VBs → per `re15_cd_staging`-Chunks (50 Sektoren, Schleife existiert `audio_psx.c:83-98`) → SPU; SHITAI/Props aus RDT → VRAM.
- Erster Spawn eines Typs: EMS-Blob (~150-260 KB) → Arena-Tail, TIM → VRAM, TIM-Bytes verworfen (Pfad existiert, `asset_psx.c re15_enemy_load`).
- Cut-Wechsel: 1 BSS-Cut (≤64 KB komprimiert) → `re15_cd_staging` → VLC in Staging-Buf → MDEC-Slices (existiert).
- BGM-Wechsel: BGM-Container (VH resident ≤8 KB, SEQ resident ≤8 KB, VB transient→SPU) — existiert (`ssx_load_track`).
- Voice: .XA/VAG via `re15_cd_staging` (3.1-C); Original nutzt CD-XA (Arch-Divergenz dokumentieren).

### 3.4 SPU-Upload-Plan
Bump-Allocator (`audio_psx.c:54 RE15_SPU_BASE 0x1010` + Rewind-Mark Z.671) ersetzen durch die **feste Original-Karte** aus §1.2(d): System@0x1020, ARMS@0x7330, Raum-Bank1@0xffd0, Raum-Bank0@0x2f3d0, CORE@0x38840, BGM@0x42fc0 (SUB direkt dahinter). Damit sind die Bank-Budgets byte-true begrenzt und Raum-/BGM-Wechsel fragmentierungsfrei. Port-Voice-Region (kein Original-Pendant) ans obere Ende hinter BGM legen, Größe nach Messung des größten BGM-VB.

### 3.5 Reihenfolge + Verifikation (KEINE Builds in dieser Session — Plan)
1. Mess-Skripte (Scratchpad): (a) pro Raum RDT-Größe + Σ EMS-Blob-Größen der per `Sce_em_set` gespawnten Typen (SCD-Scan + EMS-TOC) → belegt Arena-Größe 0x80000 vs 0x90000; (b) max numProgs/VH über alle Bänke → belegt E.
2. A→B→C→D umsetzen, `nm`-Gegenprobe `_end ≤ 0x801F0000`.
3. 3.2 (Pfade+iso.xml), dann DuckStation-Boot (`psx_dev/re15_reborn/psxtest.sh`, ≥40 s, TTY-Log: `[enemy] … OK`-Zeilen, kein `psxspu timeout`).
4. Raumketten-Smoke ROOM1170→1240→1140 auf Emulator; Vergleich der TTY-Ausgaben mit PC-Headless.

## 4) OFFEN (ehrlich)
1. **Original-Ladepfad der Gegner-Modelle** (welche FUN liest CDEMD0.EMS, an welche Adresse) ist NICHT gepinnt — in `FUN_800396fc`-Kette nicht enthalten; vermutlich lazy im Overlay/`Sce_em_set`-Umfeld. Nächster RE-Schritt: Datei-Index von CDEMD0.EMS in der LBA-Tabelle @0x8006f440 bestimmen, dessen Xrefs disassemblieren (`re15_disasm.py scan`), plus Savestate-Diff des Bereichs hinter der RDT in einem Zombie-Raum.
2. **`FUN_80053a8c` (MDEC-Decoder) nicht decompiliert** — Rollen der drei Puffer 0x80190000/0x80198000/0x80199e00 sind aus dem Callsite-Kontext + Slice-Größe 0x1E00 belegt, die exakten Grenzen des 64-KB-Fensters nicht. Nächster Schritt: `re15_disasm.py dis 0x80053a8c`.
3. **Arena-Worst-Case pro Raum ungemessen** (Schritt 3.5-1a offen) — bis dahin gilt konservativ 0x90000 (byte-true = Original-Fenstergröße).
4. **`s_ctx` (139 KB) nicht angetastet**: RE2s persistente Paket-Pools (`FUN_8002da80/8002ddf0`, Katalog) wären das Vorbild für kleinere Doppelpuffer; nicht quantifiziert.
5. Original-Stack-Spitze/Heap-Nutzung oberhalb 0x801ff404 nicht vermessen (33 Xrefs auf DAT_801ff404 nur gezählt, Struktur nicht decodiert).
6. DOOR-.DO2-Streaming (Tür-Transitionen) im PSX-Target ungeprüft; VOICE-XA vs. SPU-Voice ist eine dokumentierte, aber nicht auf Original-Disasm rückgeführte Divergenz (SCD 0x5E).

Relevante Dateien: `C:\workspace\git\reAi_v2\re15_port\targets\psx\{README.md,CMakeLists.txt,iso.xml,build.cmd}`, `C:\workspace\git\reAi_v2\re15_port\platform\psx\src\{asset_psx.c,re15_room.c,bg_psx.c,audio_psx.c,re15_cdfs.c,render.c}`, `C:\workspace\git\reAi_v2\re15_port\include\{re15_cdfs.h,re15_enemy.h,re15_emd.h,re15_vab.h,re15_ems.h}`, `C:\workspace\git\reAi_v2\RE_15_Quellcode_V2\{FUN_800396fc.c,FUN_80039a30.c,FUN_8001d600.c,FUN_80021bbc.c,FUN_800440c4.c,FUN_80044774.c,FUN_80044564.c,FUN_80036b68.c,FUN_800314b0.c,FUN_80039c2c.c,FUN_80039270.c}`, Karte `C:\workspace\git\reAi_v2\re15_port\targets\psx\build\re15.map`.
