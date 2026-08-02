# RE1.5 Byte‑True Port — Master Implementation Roadmap

> **AKTUELLE KAMPAGNE (2026-08-02):** [ROADMAP_ROOMCHAIN.md](ROADMAP_ROOMCHAIN.md) — Raumkette
> 1120/1130/1140/1150/1170/1240 byte-true (Rolltor-Sound, Krähen-KI 1170, Zombie-Hit 1140,
> systematischer Sweep, Adress-Coverage). Neue Sessions starten DORT.

*Synthesis of the six area plans into one ordered, dependency‑driven execution sequence. Every task keeps its original area ID (prefixed `A1…A6` for traceability) so we can work it piece by piece.*

---

## 1. Strategy — depth vs breadth, and what unlocks what

There are two currencies of work:

- **DEPTH** = byte‑true polish/verification of the existing 43‑file / ~22 k‑line engine (SCD VM, RDT, render, collision, camera, skeleton, lighting, AOT, audio). It is already 85–95 % correct; the ROOM1140 combat loop is the strongest slice.
- **BREADTH** = net‑new *playable* content — the front‑end shell, the other ~74 STAGE1 rooms, STAGE2‑6, the PSX hardware target.

The engine is good but the artifact **is not yet a game**: it boots hard into one room, has no shell, and only ~6–8 of ~80 STAGE1 rooms and *zero* STAGE2‑6 rooms are parity‑verified. So the leverage order is:

1. **Build an automated byte‑true ORACLE first.** Today parity is proven by hand (eyeballing stderr vs `./original`). Before pouring in breadth, we need a machine that emits a canonical per‑frame state schema from **both** the PC port and PSX savestates and diffs them to a first‑divergence frame+field. This converts every downstream task from "sieht richtig aus" into a deterministic pass/fail, and it is the single highest‑leverage investment in the whole plan.
2. **Stand up the front‑end shell.** A shared engine‑side mode machine that de‑hardcodes `main.c`. This is foundational because it makes the thing *launchable as a game* and makes **every room reachable** (title → new game → char‑select → playable), which every content phase depends on.
3. **Complete STAGE1 end‑to‑end.** One fully‑playable stage proves the *entire* pipeline (room‑load, HUD, inventory, locks/keys/flags, per‑room SCD/cutscenes, enemies, map/file/save) — the reference methodology that STAGE2‑6 then mass‑applies.
4. **Mass‑produce breadth (STAGE2‑6) behind the oracle**, protected by a regression corpus so depth can't silently regress.
5. **Polish engine residuals continuously** — the combat‑critical ones (R1) ride with STAGE1; the SCD‑opcode ones (R2) ride with the room sweeps; the rest form a dedicated polish pass.
6. **Bring up the PSX hardware target as a mostly‑independent parallel track**, gated only on the *shared* engine being stable and on `_end < 0x80200000`.

**Dependency backbone (what unlocks what):**

```
A6·I0 (build all)  ─┬─► A6·I2 (parity oracle) ─► every "acc:" gate below
                    └─► A3·F0 (room substrate) ─► all room verification (STAGE1 + 2-6)
A1·FE-0 (mode machine, de-hardcode main.c) ─► every room reachable ─► STAGE1 ─► STAGE2-6
STAGE1 complete (proves methodology) ─────────► STAGE2-6 mass-apply
A6·I3/I4 (corpus) ─► safety net ─► fast breadth
A5·B0 (PSX links) ─► A6·I1 PSX-CI + RAM gate ─► A5·P1..P5 hardware bring-up ─► A6·I6 cross-target parity
```

**Parallel tracks.** After Phase 0, three tracks can run concurrently by different agents: **(T‑Game)** front‑end → STAGE1 → STAGE2‑6 (the critical path); **(T‑Engine)** R3/R4/R6 depth polish; **(T‑PSX)** A5·B0 link → P1…P5 hardware bring‑up. They re‑converge at cross‑target parity.

**Verification discipline (applies to every task).** Prove the *mechanism* against the bytes, never the result. Presentation (fades/overlays/FMV) verifies via ffmpeg gdigrab screen‑capture vs `./original` (autoshot BMPs miss blends). State‑carrying flows verify via DuckStation savestate RAM diff. No behavior/format constant lands without a cited disasm address or file byte‑offset.

---

## 2. Legend

- **Effort:** `S` < 1 session · `M` ~1 session · `L` multi‑session.
- **Kind:** `DEPTH` (byte‑true polish of existing) · `BREADTH` (new playable content).
- **Dep:** blocking predecessor task IDs. **✔** = acceptance (how we prove it byte‑true).
- Area tags: `A1` front‑end · `A2` STAGE1 · `A3` STAGE2‑6 · `A4` engine residuals · `A5` PSX · `A6` infra/tooling.

---

## PHASE 0 — Foundation: build everything + the automated parity oracle
*Nothing byte‑true can be scaled until (a) every artifact builds from one command and (b) an automated state‑DIFF oracle exists. This phase is the keystone; do it before any content work. Pulls A6·I0, A6·I2, A3·F0, and the cheap A4·R0 triage.*

### 0A — Unblock the toolchain (A6·I0)
- **A6·I0‑4** (S, BREADTH) — **START HERE.** One build+test wrapper (kill stale `re15_pc.exe` lock → configure `RE15_BUILD_PC=ON RE15_BUILD_TESTS=ON` → ninja → `ctest --timeout 30` → print pass count). Encodes the exe‑lock + default‑OFF‑tests gotchas. Dep: —. ✔ clean tree → "79/79 passed"; re‑run with exe alive still builds.
- **A6·I0‑1** (S, BREADTH) — Restore Java extractor Gradle (`build.gradle`+`settings.gradle`, shadow, `mainClass de.re15.Main`). Src: worktree build.gradle. Dep: —. ✔ `./gradlew build && test` green; `run` executes RE15MasterExtractor phase 1.
- **A6·I0‑2** (S, BREADTH) — RE tools into CMake (`tools/overlay_mapper/CMakeLists.txt`, extend `tools/CMakeLists.txt`, flip `RE15_BUILD_TOOLS` default ON for PC). Dep: —. ✔ `overlay_mapper`+`room_graph`+`extract_doors` build; overlay_mapper emits per‑stage jump‑table JSON.
- **A6·I0‑3** (M, BREADTH) — Pin toolchain (mingw64 GCC 15.2/Ninja/CMake versions) + Python harness venv (`requirements.txt`: vgamepad, zstandard, torch, numpy, pillow) + a `doctor` bootstrap. Dep: —. ✔ fresh venv imports all harness scripts; doctor reports OK for gcc/ninja/cmake/python/vgamepad/DuckStation/Ghidra.
- **A6·I0‑5** (M, BREADTH) — Configure PSX cross‑build (`psx_toolchain.cmake` + `PSN00BSDK_PATH`), report `_end` via `mipsel-none-elf-size`. *(The real link fix is A5·B0 in the PSX track; this establishes the CI gate.)* Dep: I0‑3. ✔ `-DRE15_BUILD_PSX=ON` produces an ELF; `_end < 0x80200000` with ≥64 KB stack margin. **→ hand to T‑PSX (A5·B0).**

### 0B — The parity oracle spine (A6·I2) — *the keystone deliverable*
- **A6·I2‑1** (M, DEPTH) — Design the canonical machine‑readable per‑frame state schema (JSON‑lines, versioned): frame idx, player block (x,z,rot,hp,pst/ps1/ps2,motion), per‑actor block (slot,type,x,z,rot,ss1/ss2,grid,ai_flags,dist,hp,hitbox), room globals (cut, fade, flags, room) + declared per‑field tolerances. Src: RE15_STATE_LOG fmt (Commit 1cbcc4f5); re15_enemy_state.py field map (array 0x800acc2c stride 0x1f4, pos +0x34/+0x3c, dist +0x1d0, hp +0x9a; player 0x800aca88; cut DAT_800b532e; fade DAT_800b5568; flags DAT_800aca3c). Dep: —. ✔ schema doc + validator; sample port frame + savestate frame both validate.
- **A6·I2‑2** (M, DEPTH) — Port side: emit the schema behind `RE15_STATE_LOG_JSON=1`, one object/tick, deterministic at 30 fps under `RE15_INPUT_SCRIPT`. Dep: I2‑1. ✔ `RE15_START_ROOM=1140 RE15_INPUT_SCRIPT='R0.15,U2.5' RE15_STATE_LOG_JSON=1` byte‑identical across two runs + schema‑valid.
- **A6·I2‑3** (M, DEPTH) — PSX side: savestate→schema exporter wrapping `re15_ss.py`/`re15_enemy_state.py`/`ppos.py`, plus a multi‑save `orig_timeline.py` timeline builder. Dep: I2‑1. ✔ on `mzd_stage1_briefing.sav` → schema‑valid frame whose 5‑zombie types/positions/dist match the human reader.
- **A6·I2‑4** (L, DEPTH) — The automated per‑room state‑DIFF comparator: two schema timelines → aligned → per‑field compare with I2‑1 tolerances → first‑divergence report (frame, slot, field, port vs psx). Dep: I2‑2, I2‑3. ✔ known‑good ROOM1140 = 0 divergences; injecting the old turn‑48 (vs byte‑true 96) bug flags the rot field at the first pivot frame.
- **A6·I2‑5** (M, DEPTH) — Wire `parity_net` as the timing‑alignment front‑end (content‑based frame matches → monotone frame map) to defeat the DuckStation variable‑fps trap; cos‑score is triage, not proof. Dep: I2‑4. ✔ variable‑fps PSX capture + 30 fps port series → `--align` map; unmatched frames surface as CHECK‑DIVERGENCE, not false state‑diffs.

### 0C — Cross‑stage room substrate (A3·F0)
- **A3·F0‑1** (M, BREADTH) — Per‑room SCD **opcode census** for all 80 STAGE2‑6 variant‑0 rooms via re15‑room‑probe; diff the reached‑opcode union against the 68 implemented handlers (runtime is arbiter; offline walker desyncs). Dep: —. ✔ per‑stage opcode matrix; every reached opcode maps to a handler or files a gap ticket.
- **A3·F0‑2** (M, BREADTH) — BSS/MDEC decode sweep over all 80 STAGE2‑6 backgrounds (catch VLC/MDEC edges STAGE1 never hit). Dep: —. ✔ each decodes clean + matches PSX VRAM dump (crop x=440).
- **A3·F0‑3** (S, BREADTH) — Fix enemy→EMS model mapping for STAGE2‑6 (extend DEFAULT_EMS_ORDER / `re15_ems_index_for_type`). Dep: —. ✔ every registered STAGE2‑6 type binds an EMD; only `0x23` Alligator is a documented graceful gap.
- **A3·F0‑4** (M, DEPTH) — Per‑stage overlay data‑table residency audit (HP @0x8011xxxx, hitbox, clips; the STAGE5 `0x30` handler is a +0x814 relocated copy of STAGE3's). Ensure a stage‑agnostic tick reads the correct per‑stage bytes. Dep: F0‑3. ✔ each ported enemy's HP/hitbox/clip/damage‑window constant traces to a cited offset in the *correct* BIN; diff tool confirms relocated copies are operand‑only.
- **A3·F0‑5** (M, BREADTH) — Extract per‑room door/AOT transition tables for all STAGE2‑6 rooms → connectivity graph (dest_room/dest_stage/spawn/facing). Dep: F0‑1. ✔ walking a door in probe/PC loads the correct next room+facing, matching PSX debug‑menu adjacency (dest_room=0 IS ROOM_x00).
- **A3·F0‑6** (M, BREADTH) — Bring up the live‑parity harness for STAGE2‑6: document DuckStation JUMP offsets (`room>>4`, HEX) per stage, confirm probe loads all variant‑0 rooms, capture ≥1 baseline savestate per stage. Dep: —. ✔ a (stage,room)→JUMP table for all 80 rooms + one captured save per STAGE2‑6.

### 0D — OPEN‑list triage (A4·R0) — *cheap, prevents wasted RE*
- **A4·R0‑1** (S, DEPTH) — Re‑verify every coverage‑map 🟡/❌ row + every deferred‑backlog TIER item against current source; tag each of the 43 as `{OPEN‑RE | VERIFY‑CLOSE | FAITHFUL}`. Already confirmed stale (→VERIFY‑CLOSE, not RE): bone‑0 root decode (skeleton_common.c:229‑277), neck head‑look (:311‑374), letterbox 24 px + fade 0xF0 (fade_common.c:72‑86), espr_on owner‑rot (Commit ee01665b). Dep: —. ✔ every item carries file:line + FUN cite + a tag; true OPEN‑RE count fixed (expect ~20‑25, not 43).
- **A4·R0‑2** (S, DEPTH) — Classify each OPEN‑RE item by harness: (a) re15‑room‑probe ctest, (b) savestate byte‑diff (enemy 0x800acc2c/player 0x800aca88), (c) needs green PSX build. Dep: R0‑1. ✔ each item has a named harness + provoke recipe; PSX‑only items marked blocked‑on‑PSX‑build.

---

## PHASE 1 — Front‑End Shell & Game‑Flow

> **STATUS 2026-07-20: PHASE 1 ABGESCHLOSSEN** bis auf zwei Rest-Punkte: ⬜ FE‑1.5 (Title-Attract/Idle) und ⬜ FE‑3.2 (SCD-Opcode 0x6F Movie_on — der Boot-FMV läuft über FE‑3.3, nur der Room-SCD-Trigger fehlt). Alles andere ✅ byte-true + verifiziert (Titel/Char-Select/FMV/Save+Load/Save-Points/Pause/Game-Over/Options; Save-Point-Härtung 852c0528…b0025f9d). **NÄCHSTER TASK: S1‑3 INVENTAR** (Nutzer-Direktive — Ist-Stand: menu_common.c ist ein 95-Zeilen-Equip-Grid, kein byte-true Status-Screen).
*De‑hardcode `main.c` into a shared engine‑side mode machine so the game becomes launchable‑from‑title → new‑game → playable, then layer FMV, memory‑card save, and the in‑game mode transitions. This unlocks reachability of every room. **Area 1 (FE‑x.y) is canonical; it absorbs Area 2's S1‑0 (FE‑1…FE‑6) — see reconciliation table §Merges.***

### FE‑0 — Top‑level mode machine (must land first; zero INGAME regression)
- ✅ **A1·FE‑0.1** (M, DEPTH) — RE the RE1.5 main‑loop mode gate + mode global + per‑mode dispatch. Anchors: main body @0x8001cce0; in‑game→attract handoff @0x80015810‑38 (writes DAT_800aca38, stops audio); mode gate @0x8001d1e8 (DAT_800bxxxx mode word); START‑pause poll @0x8001cd68 (DAT_800ac762). Cross‑ref RE2 sysmode dispatcher. Dep: —. ✔ mode enum + dispatch addresses in the catalog, each citing a disasm line; the DAT_800bxxxx word + its transition writes traced; RE2 cross‑checked.
- ✅ **A1·FE‑0.2** (M, BREADTH) — Create `re15_gameflow.c/.h`: mode enum `{BOOT,WARNING,FMV,TITLE,CHARSELECT,OPTIONS,LOADGAME,INGAME,PAUSE,INVENTORY,GAMEOVER}` + `re15_gameflow_step()` shared by PSX+PC; INGAME initially calls the existing shared game step. Dep: FE‑0.1. ✔ builds all targets; ctest 79/79; headless INGAME smoke byte‑identical STATE_LOG to pre‑refactor (frame diff 0).
- ✅ **A1·FE‑0.3 ≡ A2·RL‑1** (L, BREADTH) — **[MERGED]** Lift the ~3978‑line boot‑into‑room path (RDT/ESP/audio/prop/model load + per‑frame loop) into `enter_ingame(room_id, spawn)`; default boot lands in TITLE; keep `RE15_START_ROOM=<hex>` as a debug fast‑path into INGAME. Also de‑ROOM1170‑biases the loader (Area 2's RL‑1 goal). Src: main.c boot L506‑1300 & cross‑room reload L1987‑2112 (dup'd/ROOM1170‑biased); re15_room_load/apply_pending. Dep: FE‑0.2. ✔ default exe opens on TITLE; `RE15_START_ROOM=1140` boots straight in with identical state‑log; `--headless` still emits room JSON; booting any of the 40 Leon rooms shows zero ROOM1170 residue (state‑log + autoshot on 8 rooms).

### FE‑1 — Boot → Title
- ✅ **A1·FE‑1.1** (S, BREADTH) — Cold‑boot init as BOOT mode (ResetGraph/SetVideoMode NTSC 320×240/InitGeom/pad/SPU/SsInit/VRAM font+CLUT); PC maps to SDL init, PSX links a real boot module. Src: EXE `_start` prologue before 0x8001cce0. Dep: FE‑0.2. ✔ boot path documented; PC reaches WARNING/TITLE; PSX boot module reaches the mode loop (TTY).
- ✅ **A1·FE‑1.2** (M, BREADTH) — WARNING/FMV pre‑title chain. Measured: the Capcom logo **is** MOVIE/CAPCOM.STR (no separate DATA logo TIM). Render any warning still with a fixed frame timer; else chain BOOT→(CAPCOM.STR via FE‑3)→TITLE. Dep: FE‑0.2. ✔ gdigrab capture: same screen order + per‑screen dwell (±1 frame); if no separate warning exists, document with asset/disasm evidence (don't fabricate).
- ✅ **A1·FE‑1.3** (S, BREADTH) — TITLE render: draw DATA/TITLEU.TIM (already parsed by the death tail) + blinking PRESS START in the byte‑true game font (`re15_render_msg_text`/TEX.TIM). Dep: FE‑1.1. ✔ blink cadence matches capture; START advances.
- ✅ **A1·FE‑1.4** (M, BREADTH) — Title menu FSM (NEW GAME / CONTINUE / OPTION): RE cursor layout/wrap/move+confirm SE/targets via callers of the new‑vs‑load discriminator DAT_800d4a2c + FUN_80026b7c/FUN_80034a04; cross‑ref RE2. NEW→CHARSELECT, CONTINUE→LOADGAME, OPTION→OPTIONS. Dep: FE‑1.3. ✔ cursor/wrap/SE match capture; routing correct; new‑vs‑load flag matches DAT_800d4a2c (savestate‑checked). *(Absorbs A2·FE‑2.)*
- ⬜ **A1·FE‑1.5** (S, BREADTH) — Title attract/idle timeout: RE whether the prototype idles into attract/loops FMV after N s; implement or document absence. Dep: FE‑1.4. ✔ idle behavior + timing byte‑true or documented‑absent with evidence.

### FE‑2 — New game + character select → playable
- ✅ **A1·FE‑2.1 ≡ A2·FE‑4** (S, BREADTH) — **[MERGED]** Port new‑game record init FUN_80034a04 → `*DAT_800ce550` defaults: coords (0x17f4,0,0x6db5), rot 0x400, stage/room/floor/char/HP + the byte‑true briefing loadout (knife 0x01, Browning HP 0x03+15, 50 H.GUN BULLETS 0x15+50). Build the shared save‑record struct here (with FE‑4.1). Dep: FE‑1.4. ✔ record byte‑identical to FUN_80034a04 (each field cited); original post‑new‑game savestate diffs to 0; assert g_inv slots 0/1/2.
- ✅ **A1·FE‑2.2 ≡ A2·FE‑5** (M, BREADTH) — **[MERGED]** NEW GAME → first‑room entry: port the record‑apply half of FUN_80026b7c (coords→DAT_800cfc30/34/38, rot→DAT_800cfc6e, floor→DAT_800cfdba, idx=record[+4]%9→FUN_8004a3c0), then `enter_ingame()`. Player spawns at record coords (not a door landing). Chains into the ROOM1240 pre‑intro montage (sub[2] Cut_chg+Message_on, Commit 2b7f77e6) → first playable room via the auto‑advance door. Dep: FE‑2.1, FE‑0.3. ✔ boots into the correct first room at exact record spawn (pos/rot/floor byte‑match player @0x800aca88); montage plays then hands control with no hang.
- ✅ **A1·FE‑2.3 ≡ A2·FE‑3** (M, BREADTH) — **[MERGED]** Character‑select (Leon / Elza): two‑portrait cursor FSM, confirm writes the char global (drives weapon‑bank base DAT_800741e8[char] + PLD load + ROOMxxx0/xxx1 file suffix). Determine which PLD is Elza (PL01‑PL04; PL00=Leon confirmed) by model/skeleton inspection + char‑id table. Dep: FE‑2.2. ✔ both selectable; Elza loads the correct PLD + sets char global so the room spawns her model+weapon bank; suffix‑1 resolves *1 RDTs; cursor/SE match capture.

### FE‑3 — FMV / opening movie (CAPCOM.STR, opcode 0x6F)
- ✅ **A1·FE‑3.1** (L, BREADTH) — RE the Sony STR container + build a decoder reusing bss_mdec.c + bss_vlc.c. CAPCOM.STR = 2336‑byte Form1 video (subheader 00 01 48 00) interleaved with Form2 XA (01 01 64 01, stereo 37800 Hz 4‑bit). Parse chain‑frame headers, assemble each frame's bitstream, MDEC→RGB, demux XA. Src: KNOWLEDGE §1.14/§1.15 (LBA 6497); MDEC‑VLC id 0x3800. Dep: FE‑0.2. ✔ decodes to the correct RGB sequence (dump PNGs, compare first/mid/last vs DuckStation playback) + XA demuxes to correct PCM.
- ⬜ **A1·FE‑3.2** (M, BREADTH) — Wire opcode 0x6F Movie_on (currently operand‑length only, scd_vm.c:178): RE the handler at SCD dispatch 0x800744a8 (STR‑select operand, freeze/letterbox, resume/wait), add an FMV mode entry so a room's Movie_on suspends → plays → resumes. Dep: FE‑3.1. ✔ a Movie_on room plays the STR then returns control at the byte‑true resume state; operand decode matches disasm; post‑playback state = savestate.
- ✅ **A1·FE‑3.3** (S, BREADTH) — Play CAPCOM.STR in boot/attract as the opening logo, START‑skippable, BOOT→FMV→TITLE (and title‑idle→FMV per FE‑1.5). Dep: FE‑3.1, FE‑1.2. ✔ plays at the correct point with A/V sync; START skips; gdigrab order/timing matches original.

### FE‑4 — Memory‑card SAVE / LOAD (record format + I/O FSM)
- ✅ **A1·FE‑4.1** (M, BREADTH) — RE + document the full save record at DAT_800ce550: known (FUN_80026b7c) coords@0/1/2 (s16), rot@3, stage/idx@+4 (%9), char@+5, room‑hi@+9, floor@+0xb (×‑0x708); extend to inventory 11 slots, item box, event/status flags, HP, play‑time, ink by diffing original saves + the writer. Shared serializer/deserializer with FE‑2.1. Dep: FE‑2.1. ✔ struct documented field‑by‑field with cited offsets; serialize→deserialize round‑trips byte‑identical; a decoded original card save reproduces its room/pos/inventory.
- ✅ **A1·FE‑4.2** (L, BREADTH) — Port the memory‑card I/O FSM: FUN_80032150 (op initiator — slot select, I/O bufs, op‑size 0x10/0x18, activate via bit0x200 DAT_800cfb74) + FUN_80032340 (per‑frame driver over PTR_LAB_800a23ac[DAT_800d7680] until state<2, double‑buffer DAT_800ce5e0^1). RE the 3 unknown state handlers [0..2]. PC = file (block‑format); PSX = real MemCard. Dep: FE‑4.1. ✔ writes then reads back byte‑identical; FSM step sequence + double‑buffer swap match PTR_LAB_800a23ac semantics (traced vs FUN_80032340).
- ✅ **A1·FE‑4.3 ≡ A2·SAVE‑1(load)** (M, BREADTH) — **[MERGED]** CONTINUE/LOAD: port the load half of FUN_80026b7c (FUN_80077360 record scan of DAT_800d4a2c → restore DAT_800ce550 → apply via FE‑2.2 → FUN_8004a3c0). Wire title CONTINUE + a load‑slot select screen; enter INGAME at saved state. Dep: FE‑4.2, FE‑2.2. ✔ loads to the exact saved room/pos/rot/floor/inventory (savestate diff 0); load‑slot UI cursor/SE match original.
- ✅ **A1·FE‑4.4 ≡ A2·SAVE‑1(save)** (M, BREADTH) — **[MERGED]** In‑game save point (typewriter/ink ribbon): RE the save AOT/event trigger, ink‑count gate + decrement, save‑slot UI, write call (FE‑4.2). SAVE sub‑mode from INGAME. Dep: FE‑4.2. ✔ interacting opens save UI, decrements ink byte‑true, writes a card save that FE‑4.3 reloads exactly; UI matches original.

### FE‑5 — In‑game mode transitions
- ✅ **A1·FE‑5.1** (S, DEPTH) — Promote the inline START‑pause (game_step_common.c:173, poll @0x8001cd68 reads DAT_800ac762) to a real PAUSE mode (world frozen, overlay), START→INGAME; preserve the byte‑true edge‑poll. Dep: FE‑0.2. ✔ engine halt identical to current inline (state‑log frame diff 0 while paused); resumes on the byte‑true poll; overlay matches original.
- ✅ **A1·FE‑5.2** (M, DEPTH) — INVENTORY as a mode: the status/inventory overlay (menu_common.c) becomes a mode that freezes the world like the original status screen (enter/exit gated as FUN_8006b358). Dep: FE‑0.2. ✔ opens as a mode with world frozen; all existing inventory ctests still pass; closes to INGAME; enter/exit gate byte‑true.
- ✅ **A1·FE‑5.3 ≡ A2·FE‑6** (M, DEPTH) — **[MERGED]** GAME‑OVER→TITLE→CONTINUE (proper): replace the main.c:1841‑1891 `s_in_title` hack with the byte‑true death FSM (g_death_*, sub‑6 ctr 0x6d → g_gameover_active) driving INGAME→GAMEOVER→TITLE (@0x80015810‑38 module leave + SsSeqStop audio kill; gate @0x8001d1e8); CONTINUE reloads the **last save** (FE‑4.3), not the current room. Dep: FE‑4.3, FE‑0.2. ✔ death plays the byte‑true YOU‑DIED chain → TITLE; CONTINUE reloads the last card save; audio‑stop + module handoff match @0x80015810‑38; gdigrab vs original; no leftover death music/g_gameover.

### FE‑6 — Options
- ✅ **A1·FE‑6.1** (M, BREADTH) — OPTIONS mode: RE screen‑position/brightness, sound stereo/mono, controller config. The config→logical remap is already RE'd (dialog audit: engine reads DAT_800ac768/76c), so controller‑config writes those; wire other settings to the values the engine samples; persist to the record/config block. Dep: FE‑1.4. ✔ each setting honored live (remap changes which physical button drives DAT_800ac768/76c); persists across boot; cursor/SE match original.

---

## PHASE 2 — STAGE1 Fully Playable (the reference vertical)
*Take STAGE1 from "6–8 rooms exercised" to "cold boot → RPD station → stage exit, byte‑true." This proves the whole pipeline; STAGE2‑6 then mass‑applies it. Combat‑critical engine residuals (A4·R1) and the two stubbed SCD opcodes (A4·R2‑1/R2‑2) ride in this phase because a STAGE1 player hits them. Ordered by the critical path a player walks.*

### S1‑1 — Universal room load + door‑graph traversal (foundational)
- ✅ **A2·RL‑2** (M, BREADTH) — Build the STAGE1 door‑graph (per‑room door slot → dest stage/room/spawn xyz/yaw/cut) by parsing every RDT's Door_aot_set (dest bytes pc[22]/pc[23]); regenerate/commit the `door_graph.py` generator. Dep: FE‑0.3(RL‑1). ✔ every door resolves to an existing room id; every non‑boot room has ≥1 inbound door matching its re15_room_spawns[] entry. **DONE (ead96906):** scripts/door_graph.py recreated (byte‑true walker + Door_aot_set 0x3B dest fields); build/door_graph.json = 202 rooms / 640 real doors; (A) 640/640 resolve (+4 data‑region false positives flagged), STAGE1 262 doors all resolve; (B) 184 exact + 2 yaw‑override (1140/41) + 16 FLR/manual stage‑entry + 38 boot/one‑way. RESULT PASS.
- ✅ **A2·RL‑3** (M, DEPTH) — Room‑probe sweep: load+tick (main00+sub00) all 40 Leon RDTs N frames, assert no parse/SCD crash; log spawns + camera count. Dep: RL‑1. ✔ a ctest loads+ticks all 40 with 0 crashes + a per‑room spawn/camera manifest. **DONE (264fa98d):** integration_room_probe_sweep loads each real STAGE1 RDT (re15_rdt_parse) + engine load path (scd_vm_init/register/room_reenter = main00+sub00) + 30 ticks (scd_vm_tick + enemy AI + anim); 39 real rooms 0 crashes + per‑room cut/spawn manifest; ROOM1270/71 = 4‑byte empty CD stub (skipped, not failed). 89/89 ctest.
- ✅ **A2·RL‑4** (L, BREADTH) — Per‑room model‑bank resolution: load the correct enemy EMD + NPC PLW + player PLD per room from the RDT/spawn table (drop the fixed test/elliot incbin bank). Dep: RL‑1. ✔ maggot EM in 11C0, dog in 1190/1230, crow in 10C0, spider in 1090 (autoshot spot‑check). **DONE (f0f28829):** the generic per‑type loader (pc_enemy_load, EM<type> from CDEMD0.EMS) already resolves per room; added an EAGER preload of the spawned roster BEFORE re15_game_step so the byte‑true bank is resident from the spawn frame (was lazy‑on‑render, AFTER the AI → NULL bank fallback to s_irons_clip_len). Spot‑check LIVE‑verified: EM27@11C0, EM20@1190&1230, EM21@10C0, EM26@1090, NPC EM40/42/4B@11B0. 89/89 ctest. ⬜ FOLLOW‑UPS (not in ✔): drop the ELLIOT.MD1 0x47 hardcode (→ generic EM47, needs ROOM1170 visual check); spider EM26 parses to 1 mesh (model‑data, not resolution).
- ✅ **A2·RL‑5** (M, DEPTH) — RVD camera‑cut coverage per room: drive BG cut switching from each room's RVD table (RDT+0x28 via FUN_80014230, gate DAT_800aca3c&0x100) across all rooms. Dep: RL‑1. ✔ cuts switch at byte‑true RVD boundaries; spot‑check 5 rooms vs PSX capture. **DONE (11e89989):** the RVD scan is byte‑true implemented (FUN_80014230 in aot_common.c, live‑verified ROOM1170/1190). Added integration_rvd_coverage: (A) all 1084 RVD zones across 39 rooms reference valid cut indices; (B) the real scan (re15_aot_scan) runs on every room's real zones (0 crashes, fires 716/1084 centroid probes). The exact overlap PICK is proven by unit_rvd_scan_order (ROOM1190 dog cam_from=3). 90/90 ctest.
- ✅ **A2·RL‑6** (M, DEPTH) — Door‑traversal integration: script a walk ROOM1140 → ≥3 connected rooms via real doors, confirm spawn pos/cut/band each crossing (band gate FUN_8002bd44 @0x8002bf38). Dep: RL‑2, RL‑4. ✔ RE15_INPUT_SCRIPT crosses ≥3 boundaries; arrival pos/yaw/cut = door‑graph destination. **DONE (ee261beb):** integration_door_traversal walks ROOM1140 → 1130 → 1120 → 1060 → 10C0 → 10D0 = 5 real Door_aot_set crossings, 6 distinct rooms; each fired byte‑true (forward‑620 reach FUN_80042bac + band gate + SQUARE press) and each arrival (room_id + spawn xyz + cut) == the door's door_params (= the RL‑2 graph). Engine‑only ctest (deterministic, more robust than a blind RE15_INPUT_SCRIPT walk). 91/91 ctest. **→ S1‑1 COMPLETE (RL‑2…RL‑6).**

### S1‑2 — HUD / Condition / Status
- **A2·HUD‑1** (S, DEPTH) — Byte‑true condition classification (Fine/Caution/Danger + poison) from hp + status word; single source for status text + ECG. Src: idle‑FSM thresholds @0x8003206c (<0x32 caution, <0x1e danger); DAT_800acaec bit0x2 = poison. Dep: —. ✔ flips at hp 49→caution, 29→danger; poison shows on bit0x2.
- **A2·HUD‑2** (M, DEPTH) — RE the ECG waveform generator; replace the admitted faithful‑line trace (main.c L96/L1413) with the byte‑true per‑condition waveform (RE via mzd_inv_open.sav framebuffer). Dep: HUD‑1. ✔ ECG pixel‑matches PSX status screen for Fine/Caution/Danger (framebuffer diff).
- **A2·HUD‑3** (S, DEPTH) — Equipped‑weapon icon + ammo (magazine FUN_8004ea6c / reserve FUN_8004eb70) on the status screen. Dep: HUD‑1. ✔ icon + mag/reserve match g_inv after equip/fire/reload.
- **A2·HUD‑4** (S, BREADTH) — ID‑card / portrait panel (ITEM/STPIC_*.TIM) per the inventory ground truth (mzd_inv_open.sav). Dep: HUD‑1. ✔ correct portrait renders for the selected character.

### S1‑3 — Inventory / item interactions completion

> **✅ ABGESCHLOSSEN (2026-07-21, Kampagne Waves 0-6 + Fix-Pass, Commits 971c1825…fb721b5c).** Der komplette byte-true Status-Screen: ID-Card/ECG/ARMS/Item-Grid (W1, 99.9961%), FSM/Tabs/Grid-Nav (W2), Command-Stage/Classifier/Equip-Anims/Heal-ohne-Prompt (W3, inkl. Green=+25-Fix), CHECK/Examine (W4), EXCHANGE/Combine inkl. KRAUT-MIXING (W5 — der alte „kein Combine"-Audit war falsch), MAP-Tab (100.0000% pixel-perfekt), FILE-Tab+Reader (100.0000%), PSX-Ground-Truth-Cross-Checks aller States (W6) + Fix-Pass (CHECK 100.0000%; Pad-Remap @0x80073dbc: Confirm=SQUARE byte-true). INV-1 ✅ (Command-Modell settled) · INV-2 ✅ (voller Mix/Reload/Crafting-Graph) · INV-3 Menü-Seite ✅ (Key im Menü = byte-true „You can't use it here."; Tür-Unlock → S1-4 PROG) · INV-4 ✅ · INV-5 ✅ als shipped-Verhalten (Box = Message-only, ctest-gepinnt; funktionale RE2-Box wäre separate Nutzer-Entscheidung). S1-2: HUD-1 ✅ (Screen 80/20 @0x800112b4) · HUD-2 ECG ✅ byte-true · HUD-3 ARMS-Panel ✅ · HUD-4 Portrait ✅.
- **A2·INV‑1 ≡ A4·R6‑1** (M, DEPTH) — **[MERGED]** RE the menu‑confirm classification for non‑weapon items to settle COMBINE‑vs‑no‑op (SQUARE on ammo/key currently no‑op, menu_common.c:91). Strong prior it IS a byte‑true no‑op (RE1.5 = direct id‑classifier, no COMBINE submenu). RE the FSM caller @0x80046670 (Equip DAT_800aca5d) + FUN_8004a0cc; implement COMBINE or byte‑true‑confirm the no‑op and fix the comment. Dep: —. ✔ behavior + comment match the bytes (implemented, or no‑op with citation).
- **A2·INV‑2** (M, BREADTH) — Item COMBINE: medicine mixing (Green 0x24 + Red 0x25 → G.R MIX 0x27, etc.) via FUN_8004a0cc + in‑menu ammo reload (FUN_8004e054/FUN_8004ebdc). Dep: INV‑1. ✔ combines byte‑true; menu‑reload merges box→magazine.
- **A2·INV‑3** (M, BREADTH) — Key‑item USE: using a key/tool triggers its door‑unlock/event (consume or flag per item; id≥0x22 classifier @0x80049124). Dep: INV‑1, PROG‑1. ✔ correct key sets the unlock flag + consumes/keeps byte‑true; the door opens.
- **A2·INV‑4** (M, BREADTH) — Item EXAMINE: rotate the per‑item ITPS.ITP 112×72 model (id×0x3000, reuse the pickup‑modal path) + show its description text. Dep: —. ✔ shows the rotating ITPS picture + byte‑true description string.
- **A2·INV‑5** (L, BREADTH) — Item box: deposit/withdraw (FUN_80068f9c, 0x0e/0x0f) with the 8‑visible cap in box mode + post‑removal compaction (FUN_80069714). Dep: INV‑1. ✔ round‑trips through a persistent box; visible count clamps to 8; compacts on removal.

### S1‑4 — Progression: door locks, keys, event flags, puzzles
- **A2·PROG‑1** (M, BREADTH) — Add the lock/key field to `re15_aot_door_params_t` (currently none) + the Door_aot_set lock operands + the lock check + "It's locked / you need X" message (via FUN_8002bd44 + msg_common). Dep: RL‑2. ✔ a locked door refuses entry + shows its lock message until the flag/key is set, then opens.
- **A2·PROG‑2** (M, DEPTH) — Wire the byte‑true flag store to gates: indexed CHECK/SET ops 0x58 (LAB_8003fd54) / 0x59 (LAB_8003fe90) + FUN_8004ef90 set / FUN_8004efe4 check, through DAT_800b0fd0[pc[2]]>>5 zone scratch. Dep: —. ✔ setting flag X via 0x59 opens the X‑gated door/event next scan; spot‑check 3 flags from information74.txt live.
- **A2·PROG‑3** (M, BREADTH) — Derive the STAGE1 key→door map from every RDT's Door_aot_set lock ids + info74 room notes. Dep: PROG‑1. ✔ a documented table; each entry verified by using the key in‑engine and passing the door.
- **A2·PROG‑4** (L, BREADTH) — Per‑room puzzle flags: red jewels ROOM10B0 (0x12/0x13), ladder button ROOM1100 (0x94), Kendo ROOM1010 (0x03 examined / 0x1b attacked), Brad ROOM1030 (0x11/0x12). Src: info74 + STAGE1 overlay SCD. Dep: PROG‑2, RL‑1. ✔ each flag transitions at the byte‑true trigger and gates the dependent room/door live.
- **A2·PROG‑5** (M, DEPTH) — Visited/one‑way latches: ROOM1010 visited (0x1a), ROOM1000 camera‑5 zombie‑change (0x0e), single‑fire event latches. Dep: PROG‑2. ✔ revisiting reflects the visited flag (no double‑trigger); the camera‑5 zombie swap fires exactly once.

### S1‑5 — Per‑room SCD events & cutscenes (data‑driven sweep)
- **A4·R2‑1** (M, DEPTH) — **[hoisted from Engine phase]** Kage_set (shadow), currently op_unknown no‑op (scd_vm.c:307/331). RE off jump table 0x800744a8; implement scripted shadow‑blob placement. Dep: R0. ✔ a Kage_set room places the shadow at disasm coords; probe shows the entity created.
- **A4·R2‑2** (M, DEPTH) — **[hoisted]** Cut_be_set, currently op_unknown no‑op (scd_vm.c:308/331). RE + implement the camera cut‑behind set. Dep: R0. ✔ a Cut_be_set room switches the active cut per disasm; cut sequence matches a savestate.
- **A2·SCD‑1** (L, DEPTH) — SCD coverage sweep across all 40 Leon rooms (main00 + every sub), catalog every opcode reached, flag op_unknown / walker‑desync (runtime is arbiter). Dep: RL‑1. ✔ per‑room opcode report; every op_unknown RE'd+mapped or proven a data‑region desync.
- **A2·SCD‑2** (L, DEPTH) — Fix each per‑room SCD divergence the sweep surfaces (van‑fire 1090, Marvin's‑desk 1020, conference 1060/10A0, shooting‑range 1190). Src: STAGE1 overlay FUN_* @0x80072bac. Dep: SCD‑1. ✔ each fires byte‑true vs a DuckStation capture (re15‑parity‑verify).
- **A2·SCD‑3** (L, DEPTH) — Cutscene coverage beyond Irons (Marvin desk 1020, Sherry 11B0, Ben/Ada) end‑to‑end via RBJ/ROOM%04X.RBJ + RE15_FORCE_EVENT. Dep: SCD‑1, RL‑4. ✔ each plays to completion + returns control (the Irons standard).
- **A2·SCD‑4** (M, DEPTH) — Per‑room BGM + SE_ON banks: each room loads its byte‑true track (op 0x54) + resolves SE banks (Se_on 0x36 → FUN_80045024). Dep: RL‑1. ✔ correct BGM SEQ on load + SE banks resolve to real samples (headless probe + audio spot‑check).

### S1‑6 — Enemies in real rooms + item pickups (with combat residuals A4·R1)
- **A4·R1‑1** (M, DEPTH) — Melee hit‑test ORIGIN = blade‑tip world point, not player‑center. re15_damage.c reads bone‑11 (394/411/454) but game_step_common.c:406 still measures from center on the SCD path — reconcile all melee paths to `*(0x800acbdc)+0x7b8`. Dep: R0. ✔ knife hit‑point world‑xyz == PSX blade‑tip transform (integer‑exact) for a fixed savestate frame; probe knife swing lands the same hit vs the center baseline.
- **A4·R1‑2** (M, DEPTH) — Gore/blood spawns at the model bone, not the actor root. RE done: bone_idx = LAB_8011f784[type], matrix = entity+0x188+idx*0xac, trans@+64 (zombie 0x10/0x11/0x16=bone14). Implement via re15_skel_compute_pose QUERY + re15_skel_bone_to_world. Dep: R0, R1‑1. ✔ blood spawn xyz == FUN_80106edc(bone14) integer‑exact; blade/gore share LAB_8011f784; visible blood from the bite bone.
- **A4·R1‑3** (M, DEPTH) — Sce_em_set (0x44) packed‑pose spawn: pc[18]!=0 → spawn in state 4 with the packed pose (pc[18]/[19]/[14]) for pre‑posed/scripted enemies. Src: @0x800425a4‑e8. Dep: R0. ✔ a pc[18]!=0 room spawns state=4 + packed pose == disasm; probe F0 dump matches a savestate.
- **A4·R1‑4** (M, DEPTH) — Live idle‑state[4] dispatch: FUN_8010919c is undispatched (enemy_ai_common.c:2537). Wire case state 4 → PTR_FUN_801217a0[4]. CAUTION: preserve RNG draw‑order (idle re‑roll must not shift combat RNG). Dep: R0. ✔ idle sub‑behavior == disasm; RNG stream after N idle frames matches a savestate; combat wake/grab gates unchanged.
- **A4·R1‑5** (M, DEPTH) — Maggot wave‑2 crawl slew + root‑motion speed: slew = `(rand8()&0x1f)+6` @0x80117d1c (not flat 0x20); crawl speed from move‑helper @0x8011bf50 (not flat advance(40)). Batch (RNG‑stream risk). Dep: R1‑4. ✔ per‑frame slew + advance == disasm on a walk_probe; HEAVY‑BITE uses the 0x8011bf50 stride; spawn positions unchanged vs mzd_stage1_maggot.sav.
- **A4·R1‑6** (L, DEPTH) — Zombie‑girl wave‑2 mode‑dispatch (+0x9&0xf @0x80120230) + lunge‑arm timers (enemy_ai_common.c:4170/4239 resume‑the‑brain). Requires a STAGE1 zombie‑girl provoke (Root 0x8010a8c8). Dep: R1‑4. ✔ sub‑modes fire == disasm; savestate diff of a provoked zombie‑girl matches per‑mode transitions; standard combat gates still byte‑true.
- **A2·ENE‑1** (M, DEPTH) — Per‑room enemy spawn/placement/count audit for all 40 rooms (Sce_em_set) vs PSX savestate. Dep: RL‑1, RL‑4. ✔ each room's types/positions/rotations match a DuckStation capture (the 3‑verified‑room standard).
- **A2·ENE‑2** (L, DEPTH) — *(= A4·R1‑5/R1‑6 for STAGE1 types)* Finish deferred wave‑2 behaviors (maggot slew/root‑motion, spider hit‑arm @0x80019d50, zombie‑girl mode @0x80120230, NPC idle @0x8010919c). Dep: ENE‑1. ✔ each byte‑true + live‑verified vs a PSX RAM read.
- **A2·ENE‑3** (M, DEPTH) — World item pickups (Item_aot_set 0x50): grant the right id/qty (pc[14]/pc[16] LE) + set taken_bit DAT_800b1078 so items don't respawn. Dep: RL‑1, PROG‑2. ✔ every pickup grants byte‑true (assert g_inv) + doesn't re‑appear on re‑entry.
- **A2·ENE‑4** (M, DEPTH) — Examine AOTs / readable props (EXAMINE_WORKVAR AOT + msg block, aot_common.c L427‑441; FUN_80028868 text). Dep: RL‑1. ✔ examining a flagged prop shows byte‑true text; spot‑check Marvin's desk (1020) + a note.

### S1‑7 — Map & File screens
- **A2·MAP‑1** (L, BREADTH) — Map screen: load+render STAGE1 floor maps (MAP01‑0A.PIX) with current‑floor highlight + player marker (RE the draw fn via RE2 map layer; PIX→TIM decode). Dep: HUD‑1. ✔ correct floor (B1/F3/B2), toggles from tabs, marker on the current room.
- **A2·FILE‑1** (M, BREADTH) — File/document reader: files list + paged text (reuse FUN_80028868 + msg_common paging). Dep: —. ✔ a collected STAGE1 file opens + pages byte‑true; appears in the list once obtained.

### S1‑8 — End‑to‑end STAGE1 parity + regression net
- **A2·E2E‑1** (M, DEPTH) — Author the STAGE1 critical‑path script (boot→intro→briefing→escape→hub→branches→stage exit) as a deterministic RE15_INPUT_SCRIPT routed by the door graph (walk 75 / turn 48‑96 / run CROSS). Dep: S1‑1…S1‑6. ✔ runs headless new‑game→exit, no soft‑lock, visits the expected room sequence.
- **A2·E2E‑2** (L, DEPTH) — PSX parity capture of the same path + state diff (pos/cut/flags/inventory) at each room boundary (re15‑room‑capture + parity‑verify + savestate‑ghidra; variable‑fps caveat). Dep: E2E‑1. ✔ at each boundary the port state‑log matches the PSX savestate within tolerance.
- **A2·E2E‑3** (M, DEPTH) — Per‑milestone‑room regression ctest (spawn + key SCD event + outbound door). Dep: E2E‑1. ✔ new cases pass + full suite green; a deliberate spawn/flag break is caught.

---

## PHASE 3 — Engine Byte‑True Residuals (depth polish; can run as a parallel track T‑Engine)
*The remaining DEPTH backlog after R0 triage (Phase 0) and the combat‑critical R1 + SCD‑stub R2‑1/R2‑2 (folded into Phase 2). Everything here polishes existing systems. R5 (VAB SPU) is PSX‑coupled → lives in the PSX phase. Order within is impact‑then‑tractability.*

### R2 — SCD VM opcode completion (remainder)
- **A4·R2‑3** (M, DEPTH) — Ifel_ck chained AND/OR eval (FUN_80053f50): verify the opcode‑native predicate chain handles chained AND/OR beyond single predicates; implement the combinator if any room uses it. Dep: R0. ✔ a mixed‑AND+OR Ifel takes the same branch as disasm for all operand combos; unit test over the 7 comparators + chain.
- **A4·R2‑4** (M, DEPTH) — Reconcile opcodes 0x80‑0x8E (~8 stubbed length‑1). The RE1.5 jump table 0x800744a8 ends at 0x5E → each is either a port pseudo‑op or should route to op_unknown; cite a real FUN or prove dead (Se_vol 0x80 already real). Dep: R0. ✔ each 0x80‑0x8E has a FUN cite or an explicit 'not in RE1.5 dispatch, dead' note.
- **A4·R2‑5** (S, DEPTH) — Member_calc (0x55) + fade_config (0x56) byte‑true verify (0x56 = fade CONFIG @0x80042a58→FUN_800217b0, NOT RE2's non‑existent Member_calc2). Dep: R0. ✔ op_member_calc writes per disasm; op_fade_config == FUN_800217b0; comments corrected.
- **A4·R2‑6** (M, DEPTH) — Full opcode‑table reconcile audit (coverage 0x18/0x2B mismatch + walker desync). Cross‑check every 0x00‑0x5E {length,handler} vs the jump table so the offline walker stops desyncing (0x2D=fix 34 B trap). Dep: R2‑4. ✔ offline walker decodes every STAGE1 room with zero desync vs the runtime op‑count log; table matches entry‑for‑entry.

### R3 — Presentation residuals
- **A4·R3‑1** (M, DEPTH) — MDEC BG VRAM dest from the RDT cam record (+8/+10 via FUN_8002bdf4), not hardcoded 640,256 (bg_psx.c:63‑64). Dep: R0. ✔ BG dest == RDT‑derived per cut; correct BG per cut on the PSX build (**blocked‑on‑PSX‑build**).
- **A4·R3‑2** (L, DEPTH) — ESP routineA selectors 12‑19 (muzzle/shell/room chains): decode 13/14/15/17/18/19 (currently noop) off the 48‑entry table @0x80071d40 / FUN_80019e20 @0x80019e84. Dep: R0. ✔ each runs the disasm‑cited action; a gunshot+muzzle sequence matches the routine progression in a savestate.
- **A4·R3‑3** (M, DEPTH) — ESP CLUT fade (routine 10: tpage|=row[0x16], clut+=row[0x1e]<<6, @0x800176b0) + droplet ballistic params (FUN_800174e4) currently faithful‑line. Dep: R3‑2. ✔ CLUT fades through the disasm stages; droplet velocity/lifetime from row bytes == disasm.
- **A4·R3‑4** (M, DEPTH) — Neck damped‑spring secondary motion FUN_80024c30/FUN_80024e40 (separate from the byte‑true Plc_neck head‑look): decode + determine which actors run it; implement or prove unused. Dep: R0. ✔ a spring‑driven bone matches a savestate per frame, OR proven zero live callers in STAGE1‑6 (dead) + documented.

### R4 — Skeleton / player‑anim residuals
- **A4·R4‑1** (S, DEPTH) — Verify‑and‑close bone‑0 keyframe root decode (coverage lists "known wrong"; skeleton_common.c:229‑277 shows it RE'd as FUN_8001f3bc gpf12/gpl12). Dep: R0. ✔ root translation per frame == FUN_8001f3bc (feet grounded ~‑166); delete the stale coverage row.
- **A4·R4‑2** (M, DEPTH) — RBJ un‑overlay (Plc_ret LAB_80041f88): verify/implement the player RBJ overlay‑restore back to base pose (FSM FUN_80031c44). Dep: R4‑1. ✔ after an RBJ clip completes, pose returns to base == disasm; ROOM1170 cinematic RBJ transitions match the savestate.
- **A4·R4‑3** (S, DEPTH) — Plc_neck neck‑data init fields (skeleton_common.c:327 omitted): restore the full init so head‑look is correct outside room1150's in‑range shortcut. Dep: R4‑1. ✔ an out‑of‑range initial target settles without snap == disasm.

### R6 — Menu / init residuals + final faithful‑line sweep
- **A4·R6‑2** (M, DEPTH) — 0x47‑INIT invuln trap (enemy_ai_common.c:72, +0x9a=0xffff): enumerate which types take the generic INIT (RE15_AI_STATE_INIT) before writing hp=‑1 (real NPC invuln is already @0x80101918). Dep: R0. ✔ the INIT‑path type set is enumerated; 0x47 fields written only if they don't leak into combat types (no combat zombie becomes invulnerable in probe).
- **A4·R6‑3** (S, DEPTH) — Verify the per‑thread work‑slot leak fix (U6: member_set2/add_aspeed/plc_rot) under a concurrent multi‑thread Work_set scene (ROOM1170 never exercised it). Dep: R2‑6. ✔ no work‑slot leak; slot state == a savestate at frame N.
- **A4·R6‑4** (M, DEPTH) — Final adversarial faithful‑line sweep of the ~35 legit approximations (spider web‑spit wobble uninit, player‑model‑facing fx points, VAB PC‑mixer >>1): for EACH, prove byte‑true impossible (uninit RAM / PSX‑only) with a cite, or promote it. Dep: R1‑6, R3‑3, R5‑3. ✔ every remaining faithful‑line carries an explicit 'not determinable because X (cite)' note or is converted.

---

## PHASE 4 — Regression Gate + Room/Enemy Corpus (scale verification before mass breadth)
*Turn the proven single‑room methodology into an automated safety net so STAGE2‑6 breadth can't silently regress STAGE1/depth. Depends on the Phase 0 oracle + STAGE1 methodology.*

### I1 — CI, golden‑extraction, coverage scorecard
- **A6·I1‑1** (M, BREADTH) — Stand up CI (self‑hosted/scripted, Windows/mingw/DuckStation): matrix {PC Debug, PC Release, PSX ELF}; steps configure/build/ctest/`_end` check; fail on any ctest failure or `_end` overflow. Dep: I0‑4, I0‑5, A5·B0. ✔ a PR run shows 3 green targets + 79/79 + a printed `_end`; a failing test turns it red.
- **A6·I1‑2** (S, BREADTH) — Opt‑in pre‑commit/build‑verify hook (kill lock → build → ctest) that blocks breaking commits (respect commit‑only‑when‑asked). Dep: I0‑4. ✔ a failing test rejects the commit with its name; green suite proceeds.
- **A6·I1‑3** (M, DEPTH) — Golden‑output regression for the extractor + core parsers (checked‑in hashes/blobs for an RDT parse, EMD split, TIM→BMP, ESP→EFF); assert both Java + C reproduce them. Dep: I0‑1, I0‑4. ✔ a ctest + a gradle test hash‑match; mutating a converter byte flips it red.
- **A6·I1‑4** (M, BREADTH) — Auto coverage scorecard: parse RE15_FUN_CATALOG + COVERAGE, cross‑ref FUN_/DAT_ cited in engine comments, emit %‑by‑subsystem (verified/partial/unknown) in CI. Dep: I0‑4. ✔ regenerates deterministically; a new FUN citation bumps the subsystem's implemented count.

### I3 — Room parity corpus (scale to the 240 RDTs)
- **A6·I3‑1** (L, DEPTH) — Per‑room manifest DB (one JSON entry/RDT: stage, entry spawn+yaw, cut count, door/AOT count, enemy roster from Sce_em_set, curated golden tank‑path); seed from RDT+SCD parse, verify vs boot + savestate. Dep: I0‑2. ✔ all 240 rooms listed non‑empty; ROOM1140 lists the 5‑zombie roster (0x16/0x10/0x10/0x11/0x11) + spawn (‑7600,‑17600,rot ‑96).
- **A6·I3‑2** (M, DEPTH) — Batch room‑probe ctest matrix: every RDT load+tick N frames, assert no crash + snapshot spawn set vs I3‑1 golden. Dep: I3‑1. ✔ one ctest iterates all rooms; loadable rooms pass; a spawn drift fails with the offending slot printed.
- **A6·I3‑3** (M, DEPTH) — Codify the per‑room byte‑true CHECKLIST (spawn pos+yaw, cut count + RVD anchor rule, door/AOT + dest_room, SCD thread set, walls, RDT tint, BG cut) as doc + a partial runner that fills each from port + savestate readers, flagging manual‑only items. Dep: I3‑2. ✔ the runner auto‑fills ROOM1140 + marks confirmed items; a fresh‑room template exists.
- **A6·I3‑4** (L, DEPTH) — PSX golden‑savestate corpus via re15_quickload.py (HEX JUMP) → one clean‑load save/room → export to schema (I2‑3) → index in the manifest. Dep: I2‑3, I3‑1. ✔ stage_saves/corpus/ with ≥1 verified non‑black save per captured room; each exports a frame matching the port's I3‑2 snapshot.
- **A6·I3‑5** (M, DEPTH) — Room‑parity regression job: for rooms with both a golden save + golden path, run the port deterministically, capture the JSON timeline, diff via I2‑4; report per‑room pass/fail into the scorecard. Dep: I2‑4, I3‑4. ✔ green for ROOM1140 (0 state‑word divergences through grab‑death) + a per‑room parity column.

### I4 — Per‑enemy verification (the 50 types)
- **A6·I4‑1** (L, DEPTH) — Generalize the savestate‑provoke pipeline: parameterize re15_ss_patch.py + re15_provoke.py to force‑spawn ANY of the 50 types into a base save and drive each AI state, incl. types with no loadable room (enemy‑pos +0x34/+0x3c; player‑pos 0x800aca88 is CACHE; skip‑tick +0x9&0x20 clear). Dep: I2‑3. ✔ force‑spawns a no‑JUMP‑room type, provokes attack, re15_enemy_state.py confirms +0x4/+0x5/+0x9; the save exports a schema frame.
- **A6·I4‑2** (M, DEPTH) — Per‑enemy byte‑true CHECKLIST (hitbox r/h, wake gate, per‑state attack constant + damage, death→corpse, EMD load, root‑motion) filled from the roster AI docs. Dep: I4‑1. ✔ filled checklists for zombie/maggot/spider/dog with every constant carrying a disasm address; a blank template for the next type.
- **A6·I4‑3** (L, DEPTH) — Per‑(type,state) golden savestate corpus + a port‑side enemy‑probe ctest matrix (re15_enemy_ai_run_all in an isolated actor, grid_id=0x86/x=z=30000) asserting byte‑true fields. Dep: I4‑1, I4‑2. ✔ a golden save per reached (type,state); the matrix passes + flags a perturbed constant.
- **A6·I4‑4** (M, DEPTH) — Unit‑test generator: from a captured enemy savestate emit a compilable `test_<enemy>_ai.c` asserting byte‑true fields. Dep: I4‑3. ✔ points at a save → produces a test that compiles/runs/passes; regeneration is deterministic.

### I5 — Docs + living register + lint
- **A6·I5‑1** (M, BREADTH) — Consolidate the knowledge base into a navigable `docs/` (index KNOWLEDGE, CATALOG/COVERAGE, AI/audit MDs, skills) + an Artifact overview for the human. Dep: I1‑4. ✔ docs/ links every top‑level MD; the scorecard embedded; no dead cross‑refs.
- **A6·I5‑2** (S, DEPTH) — Wire living registers to the corpora: when a room/enemy scenario now exercises an UNTESTED/deferred code path, the parity job flags it 'now runtime‑exercisable' (the audit found ~81/219 markers lying). Dep: I3‑5, I4‑3. ✔ a report lists now‑covered entries; flags them for promotion.
- **A6·I5‑3** (M, DEPTH) — 'No unsourced constant' lint over engine/src: flag any behavior/format magic number in a changed line lacking a nearby FUN_/DAT_/RDT/byte‑offset cite (RE‑Disziplin). Dep: I1‑1. ✔ passes on current cited code; adding an uncited threshold makes CI flag the line+value.

---

## PHASE 5 — STAGE2‑6 Room & Content Parity (the big breadth push, behind the corpus)
*Mass‑apply the proven STAGE1 methodology to 80 distinct rooms across 5 stages (+ their *1 variants). Mostly BREADTH per‑room parity verification via the corpus + a few DEEP boss code‑ports. Each stage phase = room‑by‑room parity + its bosses + its events; F0/F0‑4 (Phase 0) is the substrate.*

### S2 — STAGE2 (16 rooms; Adult Spider 0x25, Alligator 0x23, FX 0x24, stub 0x22)
- **A3·S2‑1** (L, BREADTH) — Room‑by‑room SCD/BG/AOT parity ROOM2000‑20F0 (probe tick + capture‑diff spawn/camera/AOT/BG/initial‑SCD vs PSX). Dep: F0‑1/2/5/6. ✔ each of 16 rooms byte‑identical spawns/cuts/AOT/BG vs the savestate frame.
- **A3·S2‑2** (M, DEPTH) — Alligator 0x23 AI live‑verify (root 0x8010c448): HP=300 (@0x801175dc), grab‑eat latch (DAT_800aca58=2→eaten=3 at hp<0 @0x8010d27c/288, reach 800), hitbox {2200,720} @0x80118b98, ×2.5 skeleton scale. Dep: S2‑1, F0‑6. ✔ live RAM shows the latch sequence + constants byte‑match; AI verified independent of the missing model.
- **A3·S2‑3** (L, BREADTH) — Alligator 0x23 **MODEL data‑recovery (DATA‑BLOCKED)**: em023 EMD proven absent from CDEMD0/1.EMS — RE the disc structure / STAGE2 container / RDT model section, or prove omission. Dep: —. ✔ em023 loads+renders, OR a documented disc‑structure proof it's absent (gap closed either way).
- **A3·S2‑4** (M, DEPTH) — Adult Spider 0x25 full‑AI live‑verify (root 0x801109e4, tables @0x80118e24/44/64): grab‑devour via player cmd‑2 FSM (keep‑alive @0x8011259c) + provoke the ceiling‑drop (grid 2‑0x1f, y=‑10800) 9‑phase ambush. Dep: S2‑1, F0‑6. ✔ spawn byte‑identical, grab‑devour timing matches a provoke save, ceiling‑drop exercised.
- **A3·S2‑5** (S, DEPTH) — FX‑emitter 0x24 (billboard drift + wrap <‑25000→20000, self‑disabled collision @0x8010efe4) + stub 0x22 (all‑leaves jr‑ra, 0‑guard inert). Dep: S2‑1. ✔ emitter trajectory matches PSX; stub spawns inert, no crash.
- **A3·S2‑6** (M, BREADTH) — STAGE2 events/cutscenes/puzzles: per‑room SCD (message/flag 0x58/0x59/camera/key‑gating) reach the same branch as PSX. Dep: S2‑1. ✔ each script reaches the same message/flag/camera outcome under the same trigger.

### S3 — STAGE3 (16 rooms; Cockroach 0x29, G‑Birkin 0x30, Ada 0x41) — biggest boss port
- **A3·S3‑1** (L, BREADTH) — Room‑by‑room parity ROOM3000‑30F0 (S2‑1 method). Dep: F0‑1/2/5/6. ✔ 16 rooms byte‑identical vs savestate.
- **A3·S3‑2** (M, DEPTH) — Cockroach 0x29 Wave‑2 polish (root 0x80110b00): scurry speed, subs 7/8/9, ‑500 overwhelm @0x80112af8. Dep: S3‑1. ✔ reproduce a provoke save; per‑frame diff clean.
- **A3·S3‑3** (L, DEPTH) — **G‑Birkin 0x30 Wave‑2b CODE PORT (biggest boss task)**: port ACTIVE‑brain subs 5 (grab‑hold/throw), 6, 7, 8, 10 (charge combo clip 1→0xf→0x13) + per‑sub damage windows + death handler (root 0x80116230, ACTIVE 0x80116d38, tables @0x8011eea4/eeb8/eef0, death 0x8011a3f0). Dep: S3‑1, F0‑4. ✔ each sub's clips/damage cite a STAGE3.BIN offset; re‑RE the subs‑6‑8 cluster; live provoke reproduces each attack.
- **A3·S3‑4** (M, DEPTH) — G‑Birkin ROOM3070 full‑encounter live‑verify (emergence sub9→hub sub1→BITE/LUNGE/CLAW/grab/charge→death→corpse), HP=300 @0x80116910, hurt‑box {r1000,h1440} @0x8011ee64. Dep: S3‑3, F0‑6. ✔ FSM matches PSX live RAM at each transition; playable to death + corpse settle.
- **A3·S3‑5** (M, DEPTH) — STAGE3 boss‑render FUN_80100000 (bone‑attach draw, LAB_801178a8 bone table @+0x188 stride 0xac). Dep: S3‑1, F0‑3. ✔ Birkin renders as the correct composite posed on its skeleton, matching VRAM capture.
- **A3·S3‑6** (M, BREADTH) — Ada NPC 0x41 behavior + STAGE3 events/cutscenes (escort/dialogue, key‑item gating; shared NPC lib 0x80050be8). Dep: S3‑1. ✔ Ada + each event script reach PSX‑identical outcomes.

### S4 — STAGE4 (16 rooms; Tyrant 0x2b, Ivy 0x2d, Cockroach 0x29)
- **A3·S4‑1** (L, BREADTH) — Room‑by‑room parity ROOM4000‑40F0. Dep: F0‑1/2/5/6. ✔ 16 rooms byte‑identical.
- **A3·S4‑2** (M, DEPTH) — Tyrant 0x2b live‑verify + polish (root 0x801118d0): sub4 ‑10 @0x80112898, sub14 ‑10 @0x80113ff8, grab‑pin cmd5→6 @0x80112b34, emerge state4 (grid 0x40/41/43), HP‑pool {86..126} @0x80118b00, hitbox {800,1710} @0x8011a094. Dep: S4‑1, F0‑6. ✔ live provoke shows both attacks + grab‑pin + emerge + death; constants cite offsets.
- **A3·S4‑3** (M, DEPTH) — Ivy 0x2d live‑verify + polish (root 0x801168c4): grab→instant‑kill (DAT_800aca58=7 @0x80116858), nav‑chase (0x800509e4/15000), HP=100 @0x80116954, hitbox {450,1530} @0x8011a2c8, weapon‑immune (null damage row). Dep: S4‑1, F0‑6. ✔ instant‑kill grab + nav + immunity byte‑match.
- **A3·S4‑4** (M, BREADTH) — STAGE4 boss‑render (FUN_80100000) + events/puzzles (lab sequence, item gating). Dep: S4‑1. ✔ Tyrant/Ivy render correctly; each event matches PSX.

### S5 — STAGE5 (24 rooms — largest; Tyrant 0x2b, Birkin 0x30/0x36)
- **A3·S5‑1** (L, BREADTH) — Room‑by‑room parity ROOM5000‑5170 (24 rooms, the largest breadth block). Dep: F0‑1/2/5/6. ✔ 24 rooms byte‑identical.
- **A3·S5‑2** (S, DEPTH) — Verify the STAGE5 Birkin relocated handler (0x80116a44 = STAGE3's +0x814): confirm re15_birkin_ai_tick correct for the copy (HP‑300 @0x80117120) + S3‑3 subs apply. Dep: S3‑3, F0‑4. ✔ diff confirms behavior‑identical to STAGE3; live provoke matches.
- **A3·S5‑3** (S, DEPTH) — Verify STAGE5 Tyrant (shares root 0x801118d0) provokes identically; catch any HP‑pool/grid variance. Dep: S4‑2, F0‑4. ✔ matches STAGE4‑verified behavior; any stage delta cited.
- **A3·S5‑4** (L, BREADTH) — STAGE5 events/puzzles/cutscenes (densest content): decode+verify every per‑room SCD + boss‑render. Dep: S5‑1. ✔ every script reaches the PSX branch; cutscene camera/flag transitions match.

### S6 — STAGE6 (8 rooms; finale, NPC 0x40, event‑heavy)
- **A3·S6‑1** (M, BREADTH) — Room‑by‑room parity ROOM6000‑6070. Dep: F0‑1/2/5/6. ✔ 8 rooms byte‑identical.
- **A3·S6‑2** (M, BREADTH) — STAGE6 finale event/cutscene scripting (escape/ending choreography, 33 overlay funcs, ending trigger/flag path). Dep: S6‑1. ✔ finale plays through to the ending state matching PSX event/flag progression frame‑for‑frame.

### S7 — Cross‑stage integration
- **A3·S7‑1** (L, BREADTH) — Cross‑stage transitions: doors whose dest is another stage swap the overlay's stage tables + reload resident assets (RE2 streaming, staging buffer 0x42000) via the S7 asset manager. Dep: F0‑4, F0‑5, S2‑1…S6‑1. ✔ walking a cross‑stage door loads the next stage's room+overlay+assets, correct spawn/facing, no residency overflow; matches PSX boundary behavior.
- **A3·S7‑2 ≡ A1·FE‑3** (M, BREADTH) — **[MERGED with front‑end FMV]** movie player for CAPCOM.STR + any transition MDEC (reuse the decoder). Dep: FE‑3.1. ✔ decodes/plays at the correct point; no A/V desync.
- **A3·S7‑3** (L, BREADTH) — Second scenario variant (ROOMxxx1) across STAGE2‑6: confirm variant semantics (player‑0/Leon vs player‑1/Elza) from RDT bytes, then parity each *1 room. Dep: S2‑1…S6‑1. ✔ semantics confirmed from RDT; each *1 room byte‑true (spawns/camera/events).
- **A3·S7‑4** (L, BREADTH) — Full STAGE2→6 playthrough integration test (both scenarios) via the door graph, hitting every spawn/event/boss, as a CI regression. Dep: S2‑6, S3‑6, S4‑4, S5‑4, S6‑2, S7‑1, S7‑3. ✔ a scripted traversal walks the whole arc with each room/boss/event matching its per‑stage baseline.

---

## PHASE 6 — PSX Hardware Target + Streaming Asset Layer (parallel track T‑PSX)
*The PSX target was written but never built/run here. This track can start right after Phase 0's toolchain install (A6·I0‑5). It's mostly DEPTH (get the existing shared engine to link, boot, fit RAM, and stream on hardware). Gated continuously on `_end < 0x80200000` + ≥64 KB stack margin, and every hardware claim on DuckStation TTY/framebuffer evidence.*

### P0 — Toolchain + linkable PS‑EXE
- **A5·B0‑1** (S, DEPTH) — Install PSn00bSDK (mipsel‑none‑elf‑gcc + libs + CMake package) + mkpsxiso; set `PSN00BSDK_PATH`; confirm DuckStation drivable. Dep: —. ✔ `mipsel-none-elf-gcc --version` + `mkpsxiso --version` run; PATH has ldscripts + CMake package; psxtest.sh launches DuckStation.
- **A5·B0‑2** (M, DEPTH) — Replace the custom FindPSn00bSDK + raw `add_executable`+`objcopy -O binary` with `psn00bsdk_add_executable` (crt0 + MIPS‑I linker script + elf2x). Dep: B0‑1. ✔ links `re15_psx.elf` + emits RE15.EXE whose first 8 bytes are "PS‑X EXE"; `size` reports text/data/bss + `_end`.
- **A5·B0‑3** (S, DEPTH) — Restore `psn00bsdk_target_incbin` for still‑resident assets (test_tim/md1/edd/emr, pl00w01_*, test_vh/vb, test_bss) so externs resolve. Dep: B0‑2. ✔ zero undefined symbols; each blob byte‑matches its shared_assets source (`cmp`); build produces RE15.EXE.

### P1 — Bootable ISO + first frame
- **A5·B1‑1** (M, DEPTH) — Populate iso.xml with the boot‑room CD tree (exact 8.3‑uppercase `;1` paths): ROOM1170.RDT, PL00.TIM, ROOM1170.RBJ, ELLIOT.{MD1,EMR,TIM}, EM21.EMD, MAIN32/SUB15.{VH,VB,SEQ}, BSS/ROOM1170/BG%02d.BSS, VOICE%02d.VAG. Dep: B0‑3. ✔ mkpsxiso builds RE15.bin/cue; TTY shows every boot‑room CdSearchFile returns size>0.
- **A5·B1‑2** (S, DEPTH) — Wire the mkpsxiso ISO target + license/region (LICENSEA.DAT), confirm SYSTEM.CNF boots cdrom:\RE15.EXE;1. Dep: B1‑1. ✔ `--target re15_iso` yields RE15.cue passing the BIOS boot/license check.
- **A5·B1‑3** (M, DEPTH) — First hardware boot smoke via psxtest.sh + TTY logging: confirm init order (GPU 320×240 double‑buffer, pad, CdInit, InitGeom, SPU), first frame, no exception. Dep: B1‑2. ✔ log shows frame_count advancing, no exception; framebuffer shows the intro not a hang.
- **A5·B1‑4** (M, DEPTH) — Bring up the full ROOM1170 intro on console (MDEC BG DecDCTvlc + A2 DrawSync/Reset fix, Leon GTE render, TEX.TIM subtitles, sub11→sub00→sub02 handoff, letterbox+fade). Dep: B1‑3. ✔ DuckStation capture matches the PC build at cut boundaries; `_end < 0x80200000` with ≥64 KB margin.

### P2 — RAM budget crisis
- **A5·RAM‑1** (S, DEPTH) — Raise s_room_rdt (0x50000=320 KB, silently rejects ROOM1190≈330 KB); measure true max (~369 KB → 0x5C000). Dep: B1‑4. ✔ the largest room's RDT loads on hardware; `_end` under budget with margin.
- **A5·RAM‑2** (M, DEPTH) — Stream the player model (ASSET_LAYER Stage 3): replace test_md1/edd/emr + pl00w01 incbin with a CD stream of PL00.{MD1,EDD,EMR}+W01 into a resident work area (−~150 KB). Dep: RAM‑1. ✔ incbin removed; `_end` drops ~150 KB; Leon mesh + idle/walk/run identical to the incbin build.
- **A5·RAM‑3** (M, DEPTH) — Safe staging consolidation (Stage 1 redo, −~100 KB) honoring src≠dst (the MDEC/VLC path reads src from cd_staging, writes dst to psx_staging — keep them separate; only share buffers across non‑overlapping loads). Dep: RAM‑1. ✔ −100 KB `_end` with BG cut‑change + enemy‑load + async‑voice all still correct (no "camera stuck / models misplaced").
- **A5·RAM‑4** (S, DEPTH) — Author `re15_resmap.h`: one header declaring every resident work area + byte budget (code, staging, RDT, player, NPC, enemy bank, fonts/shadow/sprite, SPU) mirroring RE2. Dep: RAM‑1/2/3. ✔ static sum < 0x80200000 − stack reserve; all PSX loaders size from this header; `_end` tracks the map.

### P3 — SPU audio path (+ A4·R5 folded in here)
- **A5·SPU‑1** (M, DEPTH) — Implement `re15_audio_room_se` (snd1 combat/room‑SE bank, TODO stub; mirror the footstep path, play_sample_from by se_id; player‑hit SE 10 via FUN_800453d0). Dep: B1‑4. ✔ a player hit / room SE keys the correct VAG at the right vol/pitch; A/B identical to audio_pc.c.
- **A5·SPU‑2** (M, DEPTH) — Implement `re15_audio_weapon_se` + `re15_audio_prime_weapon` (mirror FUN_80043d8c: CD‑load ARMS<w>.EDH/.VB on equip; EDT tone‑layering — handgun gunshot = 2 stacked VAGs, ARMS record 0 = 00 00 13 30). Dep: SPU‑1. ✔ equipping primes ARMS00; firing plays the 2‑VAG layered gunshot; matches audio_pc.c.
- **A5·SPU‑3** (S, DEPTH) — Implement `re15_audio_core_se` (CORE00 devour bank4 @0x801fbd00). Dep: SPU‑1. ✔ the zombie‑devour SE fires during the ROOM1140 grab‑kill, matching PC.
- **A5·SPU‑4 ≡ A4·R5‑2** (S, DEPTH) — Hardware‑verify U8 (VAB tone ADSR from VH +0x10/+0x12). Dep: B1‑3, SPU‑1. ✔ SPU_CH_ADSR readback == the tone's adsr1/adsr2; audible envelope (no flat click); no SE hits the 0x00FF/0x0000 dummy.
- **A5·SPU‑5 ≡ A4·R5‑1** (M, DEPTH) — Wire+verify U9 (note2pitch for SFX): RE Se_on→note + bank→program so SPU_CH_FREQ = re15_vab_note2pitch (BGM already uses it); remove the 22050 override where a real note exists. Dep: SPU‑1, SPU‑4. ✔ SEs play at the tone's true pitch (no octave error); footstep pitch matches PSX by spectrogram/ear.
- **A5·SPU‑6** (M, DEPTH) — Apply MIDI pitch‑bend 0xE0 (parsed‑not‑applied, audio_psx.c:468): scale active‑voice SPU_CH_FREQ via tone pbmin/pbmax (SpuVmPBVoice @0x80057ea8/@0x80057e44). Dep: SPU‑4. ✔ a bend‑using SEQ bends correctly vs original recording.
- **A4·R5‑3** (S, DEPTH) — Reconcile the 22050 Hz footstep/voice base: prove byte‑true (per‑bank base via FUN_80045a64 + EDT tone) or PC‑mixer artifact (44100/2). Dep: SPU‑5. ✔ documented as byte‑true (FUN cite) or PC‑only approximation with reason; no unexplained magic 22050.

### P4 — RE2‑style streaming asset layer (re15_res)
- **A5·RES‑1** (M, DEPTH) — Introduce the re15_res unified loader skeleton (Stage 2): `re15_res_load/stage` wrapping re15_cdfs (PSX) + file IO (PC); repoint existing CD callers. Pure refactor. Dep: B1‑4. ✔ all boot‑room CD reads route through re15_res; hardware behavior byte‑identical (TTY load log unchanged); PC uses the identical interface file‑backed.
- **A5·RES‑2** (M, DEPTH) — Move EM##.EMD load out of the RENDER loop (main.c:613‑615, loads on first draw) into the Sce_em_set SPAWN event (RE2: FUN_80022300 from LAB_800420a0). Dep: RES‑1. ✔ the model is resident before its first rendered frame (no 1‑frame Leon placeholder); no blocking CD read during render.
- **A5·RES‑3** (L, BREADTH) — Raise the enemy MODEL bank to RE2's 2 resident types (Stage 6) using RAM‑2/RAM‑3 headroom + per‑room VRAM budgeting in re15_vram (fix "still‑Leon" tex‑alloc failures in non‑1170 rooms). Dep: RAM‑2, RAM‑3, RES‑2. ✔ a 2‑enemy room (ROOM1140) loads BOTH EM types with correct textures; TTY [enemy] OK, not "arena full"/"VRAM tex FAILED".
- **A5·RES‑4** (M, DEPTH) — Finalize voice/BGM streaming for all STAGE1 rooms (Stage 7): validate SS_BGMTBL slot loads + VOICE%02d.VAG per room + graceful missing‑bank fallback (silence, not the previous track). Dep: RES‑1, SPU‑5. ✔ each reachable STAGE1 room plays its correct BGM + ≥1 voice line from CD; no drive‑collision glitch at a cut coinciding with a voice line.
- **A5·RES‑5** (L, BREADTH) — Generalize re15_res so EVERY room's RDT/EMD/TIM/BSS/VAB flows through it with NO per‑room special cases (Stage 8): remove hardcoded manifests (asset_psx.c:489‑491 ELLIOT@0x1170; :547‑549 IRONS@0x1150) → data‑driven residency from RDT bindings. Dep: RES‑1/2/3/4. ✔ loading any STAGE1 room pulls its full asset set purely from data; zero room‑id literals in the loader.

### P5 — Dual‑target byte‑true parity
- **A5·PAR‑1** (M, DEPTH) — PSX↔PC parity harness: same room + same input script on both, diff g_actors/cut/band/flags (interpreter is shared → any divergence isolates to a per‑port file). Dep: B1‑4. ✔ identical logged actor/camera/collision state for the intro + a scripted walk; every diff root‑caused to a named per‑port file.
- **A5·PAR‑2** (M, DEPTH) — GTE / fixed‑point render parity: verify the PSX GTE path (mesh_psx + skeleton_trig_psx, 20.12) produces the SAME projected screen coords as the byte‑true PC pixel‑shift model, so both match the original. Dep: PAR‑1. ✔ a per‑vertex projected‑coord dump for a fixed pose matches PSX GTE vs the PC software projector within documented GTE rounding.
- **A5·PAR‑3** (L, DEPTH) — PSX↔original savestate parity: capture DuckStation savestates of the REAL RE1.5 (helipad intro + ROOM1140 engage/grab) and diff RAM (g_actors/flags/enemy state/HP) at matched frames — the definitive PSX arbiter. Dep: PAR‑1, RES‑3. ✔ the port's RAM matches the original at matched frames for both scenes; divergences fixed or documented with exact bytes.

---

## PHASE 7 — Cross‑Target Parity & Final Integration (re‑converge the tracks)
*Prove the same shared engine yields the same state on PC and the port's own PSX ISO, and lock it in CI. Depends on the oracle (Phase 0), the PSX bring‑up (Phase 6), and the corpus (Phase 4).*

- **A6·I6‑1** (M, BREADTH) — Produce the port's PSX ISO in CI + archive as an artifact, with the `_end`/RAM gate enforced per build. Dep: I0‑5, I1‑1, A5·B1‑2. ✔ CI attaches re15_reborn.iso + prints `_end` under the limit for every PSX build.
- **A6·I6‑2** (L, BREADTH) — Automate booting the port's OWN ISO in DuckStation headless with TTY logging; assert boot health ([enemy]/[boot]/[rdt] printfs, no exceptions) — reuse the re15‑room‑capture driver pointed at the port ISO. Dep: I6‑1, A5·B1‑4. ✔ a scripted run boots the port ISO to a room + asserts the expected TTY lines with no CPU exception; a broken asset load is caught from the log.
- **A6·I6‑3 ≡ A5·PAR‑1** (L, DEPTH) — Prove PC↔PSX cross‑target state parity: same scripted room+input on the PC build (JSON state‑log) and the PSX‑ISO build (TTY schema frames), diff via I2‑4. Dep: I6‑2, I2‑4, A5·PAR‑1. ✔ a scripted ROOM1140 run yields state‑log JSON from PC + PSX ISO that diff to zero on state words/spawn; a backend‑specific divergence is surfaced with frame+field.

---

## Merges & reconciliation (explicit — where two areas describe the same work)

| Canonical task | Absorbs | What was reconciled |
|---|---|---|
| **A1·FE‑0.3** | A2·RL‑1 | De‑hardcode main.c → `enter_ingame()` = "generic per‑room loader" |
| **A1·FE‑1.4** | A2·FE‑2 | TITLE menu FSM (NEW/CONTINUE/OPTION) |
| **A1·FE‑2.1** | A2·FE‑4 | New‑game record init FUN_80034a04 + briefing loadout |
| **A1·FE‑2.2** | A2·FE‑5 | New‑game → first room + ROOM1240 pre‑intro handoff |
| **A1·FE‑2.3** | A2·FE‑3 | Character select → PLD + scenario suffix |
| **A1·FE‑3.x** | A3·S7‑2 | CAPCOM.STR decoder + Movie_on (one FMV implementation) |
| **A1·FE‑4.3/4.4** | A2·SAVE‑1 | Memory‑card load/save + typewriter AOT |
| **A1·FE‑5.3** | A2·FE‑6 | Death→title→continue as a real mode |
| **A2·INV‑1** | A4·R6‑1 | Menu‑confirm COMBINE‑vs‑no‑op RE (one investigation) |
| **A2·ENE‑2** | A4·R1‑5/R1‑6 | STAGE1 wave‑2 enemy behaviors |
| **A5·SPU‑4/5** | A4·R5‑1/R5‑2 | VAB ADSR/note2pitch (PSX‑verified) |
| **A6·I6‑3** | A5·PAR‑1 | PC↔PSX cross‑target state diff |

Area 3's **F0** (Phase 0) is the shared substrate for BOTH the STAGE1 door/room work (Area 2 RL) and the STAGE2‑6 sweeps — do it once, in the foundation.

---

## Recommended FIRST task to start now

**Start with `A6·I0‑4` — the single build+test wrapper (Effort S).** Then immediately proceed to the parity‑oracle spine `A6·I2‑1 → I2‑2 → I2‑4`.

**Why this order:**

1. **I0‑4 is the one‑hour precondition to everything.** Right now each session re‑learns the exe‑lock and default‑OFF‑tests gotchas, and there's no single "green baseline" command. One wrapper (kill lock → configure PC+TESTS → ninja → `ctest --timeout 30` → print `79/79`) gives every subsequent task — and every agent on every track — a reliable, reproducible foundation. It is cheap, unblocks all others, and is the literal precondition for CI (Phase 4) and for emitting the state log.

2. **The A6·I2 oracle is the true keystone — it changes the economics of the entire roadmap.** Today "byte‑true" is proven by eyeballing ad‑hoc stderr against `./original`, which is slow, manual, and — per the parity‑oracle memory and the door‑transition lesson — error‑prone (autoshot misses fades; "sieht richtig aus" is not proof). The moment I2‑1 (canonical schema) + I2‑2 (port emits it deterministically) + I2‑4 (automated first‑divergence diff) exist, **every single "✔ acceptance" gate in this document becomes a deterministic pass/fail** instead of a manual judgement. That is the difference between verifying 6 rooms by hand and verifying 240 rooms in CI. Building it first means all downstream breadth (STAGE1 completion, STAGE2‑6, PSX parity) is provable from day one and can't silently regress the hard‑won depth.

3. **It respects the dependency backbone.** I0‑4 and I2 depend on nothing, block nearly everything, and are Small/Medium. Front‑end and STAGE1 work (the exciting playable milestones) become far safer and faster once the oracle exists — so investing the first ~1–2 sessions here pays back across the whole plan.

Concretely, the first session's deliverable: `re15_pc.exe` builds green via one command (I0‑4), and `RE15_START_ROOM=1140 RE15_INPUT_SCRIPT='R0.15,U2.5' RE15_STATE_LOG_JSON=1` emits a schema‑valid, run‑to‑run‑identical JSON timeline (I2‑1/I2‑2) — the first brick of the automated oracle that gates the rest of the roadmap.


---

# APPENDIX — Full per-area task breakdown (176 tasks)

*Every task with byte-true source, effort (S/M/L), dependencies, acceptance, and DEPTH/BREADTH.*


## AREA: FRONT-END SHELL + GAME-FLOW (boot → title → character-select → new game/continue → memory-card save/load → FMV → in-game mode machine {title/game/pause/inventory/game-over})

> **STATUS 2026-07-20: Dieses Area-Detail ist umgesetzt** (kanonische Marker oben in PHASE 1: alles ✅ außer FE‑1.5/FE‑3.2). Der Absatz darunter beschreibt den VOR-Zustand („The port has NO front-end") und bleibt als historische Analyse stehen.

The port has NO front-end: PC main.c (3978 lines) boots hard into a room (default 0x1240 / RE15_START_ROOM), and the only "flow" is a death→title hack (main.c:1841-1891, s_in_title) that reloads the *current* room on SQUARE. There is no top-level mode machine, no title, no character select, no new-game/continue, no memory-card save (only RE2-referenced catalog stubs), and opcode 0x6F Movie_on is a no-op (scd_vm.c:178 operand-length only). BUT the pieces to build on exist and are byte-true: the death/game-over FSM (game_state.c g_death_*, sub-6 ctr 0x6d, FUN_8003694c/FUN_80021880), an inline START-pause poll (@0x8001cd68 reads DAT_800ac762), an inventory overlay (menu_common.c / FUN_8006b358), title+youdied TIMs (DATA/TITLEU.TIM, YOUDIED.TIM), the CAPCOM.STR opening FMV (MOVIE/CAPCOM.STR, 6.1MB Form1 video + Form2 XA), a working MDEC+VLC decoder (bss_mdec.c/bss_vlc.c) reusable for STR, five player PLDs (PL00=Leon, PL01-PL04 candidate Elza), and the RE2-referenced save chain (FUN_80032150 initiator, FUN_80032340 FSM over PTR_LAB_800a23ac[state], FUN_80026b7c load/resume, FUN_80034a04 new-game record DAT_800ce550 with default coords 0x17f4/0/0x6db5/0x400). Strategy: since RE1.5 is a prototype whose shell is thin, RE2 retail (info/re2leon, ghidra_re2_Leon.txt) is the architectural reference (as the save section already is). Ordered so the game becomes launchable-from-title→new-game→playable FIRST (Phases FE-0..FE-2), then FMV, then card save/load, then the in-game mode transitions, then options. The top-level mode machine must be shared engine-side (new re15_gameflow.c) so PSX and PC share it. Byte-true verification for presentation uses ffmpeg gdigrab screen-capture vs ./original (per the door-transition lesson that autoshot BMPs miss fades/overlays); state-carrying flows verify against DuckStation savestate RAM (DAT_800ce550 record, mode gate @0x8001d1e8).


### FE-0: Top-level game-mode machine (foundation — must land before anything else)
*Extract the boot-into-room monolith into a shared engine-side mode FSM so title/game/pause/inventory/game-over become real modes; INGAME must reproduce today's behavior byte-for-byte (zero regression).*

- ✅ **[FE-0.1 · M · DEPTH]** RE the RE1.5 top-level main-loop mode gate and identify the mode global + per-mode dispatch. Anchors already located: main body @0x8001cce0; the in-game→attract handoff @0x80015810-38 (writes DAT_800aca38, stops audio); the main-loop mode gate @0x8001d1e8 (lui v1,0x800b — reads a DAT_800bxxxx mode word); START-pause poll @0x8001cd68 (reads pad DAT_800ac762). Cross-reference RE2 retail's clean sysmode dispatcher in ghidra_re2_Leon.txt / info/re2leon as the architectural template.
  - *Quelle:* ghidra1_V2.txt:94301(main),81645-81655(@0x80015810 attract),94636(@0x8001d1e8 gate),94337(@0x8001cd68 pause); game_state.c:43 note; RE2 ref ghidra_re2_Leon.txt
  - *Dep:* none  ·  *Akzeptanz:* Documented mode enum + dispatch addresses added to RE15_FUN_CATALOG.md, each entry citing the disasm line; the DAT_800bxxxx mode word identified and its transition writes traced; RE2 sysmode cross-checked.
- ✅ **[FE-0.2 · M · BREADTH]** Create engine-side re15_gameflow.c/.h with a mode enum {BOOT, WARNING, FMV, TITLE, CHARSELECT, OPTIONS, LOADGAME, INGAME, PAUSE, INVENTORY, GAMEOVER} and re15_gameflow_step() that dispatches per-mode, shared by PSX+PC (mirrors game_step_common.c). INGAME initially just calls the existing shared game step so behavior is identical.
  - *Quelle:* Mode set from FE-0.1 (DAT_800bxxxx @0x8001d1e8); reuse existing re15_game_step.h / game_step_common.c contract
  - *Dep:* FE-0.1  ·  *Akzeptanz:* Builds all targets; ctest 79/79 stays green; a headless smoke in INGAME mode produces byte-identical STATE_LOG to pre-refactor (frame-by-frame diff = 0).
- ✅ **[FE-0.3 · L · BREADTH]** Refactor PC platform/pc/main.c: lift the ~3978-line boot-into-room sequence (RDT load, ESP/audio/prop/model load, per-frame loop) into an enter_ingame(room_id, spawn) subroutine callable by the mode machine on NEW GAME / LOAD / door. Default boot now lands in TITLE; keep RE15_START_ROOM=<hex> as a debug fast-path that jumps straight to INGAME (preserving every current parity/room-probe harness).
  - *Quelle:* Current main.c boot path (L506-~1300); RE15_START_ROOM handling main.c:583-604
  - *Dep:* FE-0.2  ·  *Akzeptanz:* Default `re15_pc.exe` opens on TITLE (not a room); `RE15_START_ROOM=1140` still boots straight into ROOM1140 with identical state-log; `--headless` smoke still emits room JSON.

### FE-1: Boot → Title (make it launchable-from-title first)
*Cold boot init, CAPCOM logo FMV / warning, and a working PRESS-START title with a NEW GAME / CONTINUE / OPTION menu FSM.*

- ✅ **[FE-1.1 · S · BREADTH]** RE + implement the cold-boot init sequence (ResetGraph/SetGraphDebug, SetVideoMode NTSC 320x240, InitGeom, pad init, SPU/SsInit, VRAM font/CLUT upload) as the BOOT mode. On PC this maps to the existing SDL init; on PSX it must link a real boot module. Establishes the entry that hands off to WARNING/FMV.
  - *Quelle:* RE1.5 EXE _start/main prologue preceding 0x8001cce0 (ghidra1_V2.txt ~94200-94300); RE2 boot ref info/re2leon/PSX.EXE
  - *Dep:* FE-0.2  ·  *Akzeptanz:* Boot path documented in catalog; PC boots to WARNING/TITLE cleanly; PSX target compiles a boot module that reaches the mode loop (TTY confirms).
- ✅ **[FE-1.2 · M · BREADTH]** CAPCOM logo + warning/disclaimer as the WARNING/FMV pre-title chain. NOTE (measured): there is no separate CAPCOM/warning TIM in DATA — the Capcom logo IS MOVIE/CAPCOM.STR (the opening FMV), so this task renders any warning still (if one exists in DATA) with a fixed frame timer and otherwise chains BOOT→(CAPCOM.STR via FE-3)→TITLE. Verify whether RE1.5 shows a text warning screen before title vs going logo→title.
  - *Quelle:* attract chain note game_state.c:43; DATA/ TIM enumeration (only TITLEU/TITLEJ/YOUDIED present); CAPCOM.STR MOVIE/ 6.1MB
  - *Dep:* FE-0.2  ·  *Akzeptanz:* gdigrab capture of boot vs ./original: same screen order and per-screen dwell time (±1 frame); if RE1.5 has no separate warning still, that is documented with the disasm/asset evidence rather than fabricated.
- ✅ **[FE-1.3 · S · BREADTH]** TITLE render — draw DATA/TITLEU.TIM (already parsed by the death-tail via re15_render_pc_show_title) as the TITLE mode background plus a blinking PRESS START prompt in the byte-true game font (re15_render_msg_text / TEX.TIM). START advances to the menu.
  - *Quelle:* DATA/TITLEU.TIM; existing render_pc show_title (main.c:1838-1885); font FUN_80028868
  - *Dep:* FE-1.1  ·  *Akzeptanz:* Title still + blinking prompt display; START press (pad_pressed START bit) advances; blink cadence matches original capture.
- ✅ **[FE-1.4 · M · BREADTH]** Title menu FSM (NEW GAME / CONTINUE / OPTION) — RE the RE1.5 title menu cursor layout, wrap, move/confirm SE, and selection targets. Anchor via the callers of the new-vs-load discriminator DAT_800d4a2c and FUN_80026b7c/FUN_80034a04; cross-ref RE2 title menu. NEW GAME→CHARSELECT, CONTINUE→LOADGAME, OPTION→OPTIONS.
  - *Quelle:* DAT_800d4a2c (new-vs-load, RE2_Quellcode_V2/FUN_80026b7c.c:17); FUN_80034a04.c; RE2 title menu ghidra_re2_Leon.txt
  - *Dep:* FE-1.3  ·  *Akzeptanz:* Cursor positions/wrap/SE match original capture; each item routes to the correct mode; the new-vs-load flag set on selection matches DAT_800d4a2c semantics (savestate-checked).
- **[FE-1.5 · S · BREADTH]** Attract/idle timeout on TITLE — RE whether the RE1.5 prototype title idles into an attract/demo or loops the FMV after N seconds, and implement the timeout transition (or document its absence).
  - *Quelle:* RE1.5 title handler idle timer (from FE-1.4 RE); RE2 attract ref
  - *Dep:* FE-1.4  ·  *Akzeptanz:* Idle behavior matches original (loops to FMV/attract or stays), timing byte-true or documented-as-absent with evidence.

### FE-2: NEW GAME + character select → playable
*NEW GAME initializes the byte-true save record and drops the player into the correct first room at the correct spawn; character select (Leon/Elza) layered on. After this phase the game is title→new-game→playable.*

- ✅ **[FE-2.1 · S · BREADTH]** Port the new-game record init FUN_80034a04 — populate *DAT_800ce550 with defaults: coords (0x17f4, 0, 0x6db5), rot 0x400, plus stage/room/floor/char/HP/inventory defaults. Build the byte-true save-record struct here (shared with FE-4.1).
  - *Quelle:* RE2_Quellcode_V2/FUN_80034a04.c (coords 0x17f4/0/0x6db5/0x400); DAT_800ce550 record; RE15_FUN_CATALOG.md:199
  - *Dep:* FE-1.4  ·  *Akzeptanz:* New-game record byte-identical to FUN_80034a04 output (each field cited); a savestate of the original's post-new-game record diffs to 0 against the port's record.
- ✅ **[FE-2.2 · M · BREADTH]** NEW GAME → first-room entry — port the record-apply half of FUN_80026b7c: copy record coords→player pos (DAT_800cfc30/34/38), rot→DAT_800cfc6e, floor→DAT_800cfdba, resolve stage/room (idx = record[+4]%9 → FUN_8004a3c0 room load), then enter INGAME via enter_ingame(). Player spawns at record coords, not a door-landing spot.
  - *Quelle:* RE2_Quellcode_V2/FUN_80026b7c.c:32-53 (record→live-state copy, FUN_8004a3c0 room load); FE-0.3 enter_ingame
  - *Dep:* FE-2.1,FE-0.3  ·  *Akzeptanz:* NEW GAME boots into the correct RE1.5 first room at the exact record spawn (pos/rot/floor) — verified against the original's first-frame savestate (player pos @0x800aca88, rot) = byte match.
- ✅ **[FE-2.3 · M · BREADTH]** Character-select screen (Leon / Elza). RE the select FSM: two portraits/cursor, confirm writes the character global (drives the weapon-bank base DAT_800741e8[char] and the PLD load). Determine which PLD is Elza (candidates PL01-PL04; PL00=Leon confirmed) by model/skeleton inspection + RE2/1.5 char-id table. Wire NEW GAME→CHARSELECT→FUN_80034a04(char).
  - *Quelle:* char weapon-bank DAT_800741e8[char] (main.c:772); PLD set PL00-PL04 (shared_assets/PSX/PLD); RE1.5 char-id table (RE via ghidra1)
  - *Dep:* FE-2.2  ·  *Akzeptanz:* Both characters selectable; confirming Elza loads the correct PLD and sets the char global so the first room spawns the Elza model with her weapon bank; cursor/SE match original capture.

### FE-3: FMV / opening movie (CAPCOM.STR, opcode 0x6F Movie_on)
*Decode Sony STR video and wire the opening movie + the Movie_on opcode so room-scripted and boot FMVs play.*

- ✅ **[FE-3.1 · L · BREADTH]** RE the STR container + build a decoder reusing the existing MDEC (bss_mdec.c) + VLC (bss_vlc.c). CAPCOM.STR is 2336-byte-sector Form1 video (subheader 00 01 48 00) interleaved with Form2 XA audio (01 01 64 01, stereo 37800Hz 4-bit). Parse the sector chain-frame headers (frame#, sector-in-frame, demuxed BS size), assemble each frame's bitstream, MDEC-decode to RGB, and demux the XA audio stream.
  - *Quelle:* RE15_KNOWLEDGE.md §1.14/§1.15 (STR/CAPCOM.STR, LBA 6497, subheaders); bss_mdec.c/bss_vlc.c (MDEC-VLC id 0x3800); memory/agent_xa_capcom_compare.md
  - *Dep:* FE-0.2  ·  *Akzeptanz:* CAPCOM.STR decodes to the correct RGB frame sequence (dump PNGs, compare first/mid/last frame to real DuckStation playback of CAPCOM.STR) and the XA audio demuxes to correct PCM.
- **[FE-3.2 · M · BREADTH]** Wire opcode 0x6F Movie_on in the SCD VM (currently only its operand length is known, scd_vm.c:178). RE the RE1.5 Movie_on handler: which operand selects the STR, the freeze/letterbox during playback, and the resume/wait semantics. Add an FMV mode entry so a room SCD Movie_on suspends the game loop, plays the STR (FE-3.1), then resumes.
  - *Quelle:* scd_vm.c:178 (0x6F operand-len); RE1.5 Movie_on handler @ SCD dispatch 0x800744a8 (RE via re15-psx-disasm); RE15_KNOWLEDGE.md:290
  - *Dep:* FE-3.1  ·  *Akzeptanz:* A room whose SCD issues Movie_on plays the STR then returns control at the byte-true resume state; operand decode matches the disasm; game state after playback = savestate.
- ✅ **[FE-3.3 · S · BREADTH]** Play CAPCOM.STR in the boot/attract flow as the opening logo/FMV, START-skippable, chaining BOOT→FMV→TITLE (and title-idle→FMV per FE-1.5).
  - *Quelle:* attract chain (FE-0.1 @0x80015810 handoff); CAPCOM.STR MOVIE/
  - *Dep:* FE-3.1,FE-1.2  ·  *Akzeptanz:* Opening FMV plays at the correct point with correct A/V sync, START skips it to TITLE; gdigrab capture order/timing matches original.

### FE-4: Memory-card SAVE / LOAD + save-data format
*Byte-true save record, the memory-card I/O FSM, CONTINUE/LOAD from title, and the in-game save point.*

- ✅ **[FE-4.1 · M · BREADTH]** RE + document the full save-data record at DAT_800ce550: known fields from FUN_80026b7c are coords@0/1/2 (s16), rot@3, stage/idx@+4 (%9), char@+5, room-hi@+9, floor@+0xb (×-0x708). Extend to the whole block (inventory 11 slots, item box, event/status flags, HP, play-time, ink count) by diffing original save blocks + the record writer. Produce a serializer/deserializer shared with FE-2.1.
  - *Quelle:* RE2_Quellcode_V2/FUN_80026b7c.c:32-48 (field reads); FUN_80034a04.c (defaults); inventory DAT_800d4a3c / box DAT_800d4a68 (catalog:201-206)
  - *Dep:* FE-2.1  ·  *Akzeptanz:* Save-record struct documented field-by-field with byte offsets cited; serialize→deserialize round-trips byte-identical, and a decoded original memory-card save reproduces that save's room/pos/inventory.
- ✅ **[FE-4.2 · L · BREADTH]** Port the memory-card I/O FSM: FUN_80032150 (op initiator — select card/CD/RAM slot, set I/O bufs, op-size 0x10/0x18, activate via bit0x200 DAT_800cfb74) + FUN_80032340 (per-frame driver: dispatch PTR_LAB_800a23ac[DAT_800d7680] until state<2, double-buffer via DAT_800ce5e0^1). RE the 3 unknown state handlers PTR_LAB_800a23ac[0..2] (coverage line 80 gap). On PC back it with a file (block-format), on PSX with real MemCard calls.
  - *Quelle:* RE2_Quellcode_V2/FUN_80032150.c, FUN_80032340.c (PTR_LAB_800a23ac[state], DAT_800d7680); catalog:196-197; coverage:80
  - *Dep:* FE-4.1  ·  *Akzeptanz:* Port writes a save (block-format) and reads it back byte-identical; the FSM step sequence + double-buffer swap matches PTR_LAB_800a23ac semantics (traced against FUN_80032340).
- ✅ **[FE-4.3 · M · BREADTH]** CONTINUE / LOAD flow — port the load half of FUN_80026b7c (FUN_80077360 record scan of DAT_800d4a2c, restore DAT_800ce550, apply record via the FE-2.2 path, FUN_8004a3c0 room load). Wire title CONTINUE + a load-slot select screen; enter INGAME at the saved state.
  - *Quelle:* RE2_Quellcode_V2/FUN_80026b7c.c:17-53 (FUN_80077360 scan, record restore, FUN_8004a3c0)
  - *Dep:* FE-4.2,FE-2.2  ·  *Akzeptanz:* A saved game loads to the exact saved room/pos/rot/floor/inventory (savestate diff = 0); load-slot UI cursor/SE match original.
- ✅ **[FE-4.4 · M · BREADTH]** In-game save point (typewriter/ink ribbon). RE the save trigger (AOT/event → save screen), the ink-ribbon-count gate + decrement, the save-slot select UI, and the write call (FE-4.2). Enter a SAVE sub-mode from INGAME.
  - *Quelle:* RE1.5 save AOT/event handler (RE via aot_common dispatch + ghidra1); RE2 save-room typewriter ref ghidra_re2_Leon.txt; ink count in record (FE-4.1)
  - *Dep:* FE-4.2  ·  *Akzeptanz:* Interacting with a save point opens the save UI, decrements ink byte-true, and writes a card save that FE-4.3 reloads exactly; UI matches original capture.

### FE-5: In-game mode transitions (pause / inventory / game-over→title→continue)
*Promote the existing inline pause + inventory + death-tail hack into proper modes wired through the FE-0 machine, byte-true.*

- ✅ **[FE-5.1 · S · DEPTH]** PAUSE mode — the START-pause is currently a partial inline in game_step_common.c:173 (poll @0x8001cd68 reads DAT_800ac762). Promote it to a real PAUSE mode: START from INGAME → PAUSE (world frozen, pause overlay), START/confirm → INGAME. Preserve the byte-true edge-poll.
  - *Quelle:* @0x8001cd68 START poll (ghidra1_V2.txt:94337, DAT_800ac762); existing inline game_step_common.c:173-180
  - *Dep:* FE-0.2  ·  *Akzeptanz:* START pauses (engine halt identical to current inline behavior — state-log frame diff 0 while paused), resumes on the byte-true poll; overlay matches original.
- ✅ **[FE-5.2 · M · DEPTH]** INVENTORY as a mode — the status/inventory overlay exists (menu_common.c, item_icon/modal) but runs inline. Make it an INVENTORY mode that freezes the world like the original status screen (enter/exit gated as FUN_8006b358 / the status open), so open/close is a clean mode transition rather than an overlay flag.
  - *Quelle:* menu_common.c (existing); FUN_8006b358 inventory runtime (catalog:202); status-screen open trigger
  - *Dep:* FE-0.2  ·  *Akzeptanz:* Inventory opens as a mode with the world frozen, all existing inventory features (grid, equip, heal-use, item-get modal) still pass their ctests, closes back to INGAME; enter/exit gate byte-true.
- ✅ **[FE-5.3 · M · DEPTH]** GAME-OVER → TITLE → CONTINUE (proper). Replace the main.c:1841-1891 s_in_title hack with a real transition: the byte-true death FSM (game_state.c g_death_*, sub-6 ctr 0x6d → g_gameover_active) drives INGAME→GAMEOVER→TITLE (@0x80015810-38 leaves the in-game module, main gate @0x8001d1e8), and CONTINUE reloads from the LAST SAVE (FE-4.3) instead of the current room.
  - *Quelle:* game_state.c g_death_*/s_go_ctr(FUN_8003694c, sub-6 0x6d); @0x80015810-38 (ghidra1_V2.txt:81645-81655); @0x8001d1e8 gate (94636)
  - *Dep:* FE-4.3,FE-0.2  ·  *Akzeptanz:* Death plays the byte-true YOU-DIED chain, returns to TITLE, and CONTINUE reloads the last card save (not the current room); the audio-stop + module handoff match @0x80015810-38; gdigrab capture vs original.

### FE-6: Options screen
*RE and implement the RE1.5 options (screen adjust, sound mode, controller config) reachable from TITLE and persisted.*

- ✅ **[FE-6.1 · M · BREADTH]** OPTIONS mode — RE the RE1.5 options screen: screen-position/brightness, sound stereo/mono, and controller config. The config→logical-button remap is already reverse-engineered (dialog audit: the engine reads logical remap output words DAT_800ac768/76c), so the controller-config task writes those; wire the other settings to the values the engine actually samples. Persist to the save record / a config block.
  - *Quelle:* config remap DAT_800ac768/76c (reai-v2-msg-dialog-audit); RE1.5 options handler (RE via ghidra1, title-menu OPTION target from FE-1.4); RE2 options ref
  - *Dep:* FE-1.4  ·  *Akzeptanz:* Options screen adjusts each setting and the engine honors it (e.g. remap changes which physical button drives DAT_800ac768/76c, verified live); settings persist across boot; cursor/SE match original.

## AREA: STAGE1 FULLY PLAYABLE — from cold boot through the RPD police station (all ~80 STAGE1 RDTs / 40 Leon rooms), ordered by the critical path a player walks

Measured state: all 80 STAGE1 RDTs exist and are in re15_room_ids[]; re15_room_spawns[] already carries per-room inbound-door spawns; cross-room DOOR transition FSM works (g_room_change → re15_room_apply_pending + byte-true fade). BUT the exe hard-boots into ROOM1240 (no title/new-game/char-select), main.c is heavily ROOM1170/1240/1140/1150-hardcoded, inventory has equip+heal-use but NO combine/key-use/examine/item-box, the ECG is an admitted faithful-line trace, there is NO MAP/FILE/SAVE screen, door locks are unmodeled (re15_aot_door_params_t has no lock/key field), the flag store (game_state.c FUN_8004ef90/efe4) is barely wired to progression, and only 4 rooms' SCD/cutscenes are verified (1240 montage, 1140 combat, 1150 Irons; 1090/1020/11B0/etc unverified). Enemy AI for all 50 types is byte-true but only 3 rooms' spawns are live-parity-checked. The plan is 9 phases: (0) front-end shell, (1) universal room-load + door-graph traversal [foundational — de-hardcode main.c], (2) HUD/condition/status, (3) inventory completion, (4) locks/keys/flags/puzzles, (5) per-room SCD/cutscene sweep, (6) enemies+pickups per room, (7) map/file/save screens, (8) end-to-end STAGE1 parity + regression net. DEPTH = byte-true polish of existing systems; BREADTH = new playable content. Scope note: PSX target build and the streaming asset-layer rewrite are cross-cutting but belong to their own areas — here they surface only where a PC room-load needs de-hardcoding (RL-1/RL-4). Primary byte-true sources: STAGE1 overlay @0x80100000 (RE_15_Quellcode_Overlays/STAGE1), entity dispatch @0x80072bac, EXE menu/save/box fns (FUN_80034a04/80032150/80068f9c), the attract/title gate @0x8001d1e8, information74.txt room+flag table, and the re15-room-probe / re15-parity-verify harnesses.


### S1-0 — Front-End Shell (cold boot → title → new game → character select → intro handoff)
*The exe boots to a TITLE screen instead of hard-loading a room; a player can start NEW GAME, pick a character (scenario suffix), be seeded a byte-true save record, and be handed into the STAGE1 pre-intro. The death→title→continue loop closes as a real state. RE15_START_ROOM stays as a debug bypass.*

- ✅ **[FE-1 · M · BREADTH]** Add a top-level front-end state machine (BOOT → disclaimer/attract → TITLE) that owns the main loop before any room loads; today main.c L584 hard-sets boot_room=0x1240 and jumps straight into gameplay. Draw TITLEU.TIM (already loaded for the death path, main.c L1838-1840).
  - *Quelle:* Attract/title chain @0x80015810-0x80015838 + main-loop gate @0x8001d1e8 (cited in main.c L1862); TITLEU.TIM (BIOHAZARD-2 logo, 320x240 16bpp). FUN_8003694c death-FSM already reaches title.
  - *Dep:* none  ·  *Akzeptanz:* Launching re15_pc.exe with NO RE15_START_ROOM shows the title screen first (autoshot); setting RE15_START_ROOM still bypasses to a room for debug.
- ✅ **[FE-2 · S · BREADTH]** TITLE menu: NEW GAME / LOAD GAME / OPTION cursor + selection, using the .msg game font glyphs (not the debug font).
  - *Quelle:* Title-menu FSM behind the @0x8001d1e8 gate; option glyph codes are the .msg charset (main.c L1374-1381 already decodes Yes/No option glyphs); FUN_80028868 TEX.TIM font renderer.
  - *Dep:* FE-1  ·  *Akzeptanz:* Cursor moves between the 3 options with the byte-true face-button convention; NEW GAME dispatches FE-4; LOAD dispatches SAVE-1.
- ✅ **[FE-3 · M · BREADTH]** Character select → set the scenario suffix (Leon=0 / Elliot=1, the ROOMxxx0 vs ROOMxxx1 file convention) and select the player PLD, replacing the hardcoded test.*/elliot.* incbin choice.
  - *Quelle:* Char-select in the boot chain; room-file suffix convention (ROOM115_/PL00 vs PL01, information74.txt); PLD/ dir (PL00=Leon).
  - *Dep:* FE-2  ·  *Akzeptanz:* Selecting Leon boots the *0 rooms with PL00; a suffix-1 selection resolves *1 RDTs and PL01. Verified by the loaded RDT path + player model.
- ✅ **[FE-4 · S · DEPTH]** New-game save-record init: seed start room/position/heading + the byte-true briefing loadout (knife + Browning HP 15 + 50 H.GUN BULLETS).
  - *Quelle:* FUN_80034a04 (new-game save-record init, defaults coords 0x17f4/0/0x6db5/0x400 — verify vs STAGE1 start); re15_inv_load_briefing (inventory_common.c L87, from mzd_stage1_briefing.sav).
  - *Dep:* FE-3  ·  *Akzeptanz:* NEW GAME seeds the byte-true start position + inventory (assert g_inv slots 0/1/2 = 0x01/0x03+15/0x15+50) with no debug env vars.
- ✅ **[FE-5 · S · BREADTH]** Intro handoff: title/new-game → the existing ROOM1240 pre-intro montage → first playable room. Wire the montage as the real new-game entry, not a debug boot.
  - *Quelle:* ROOM1240 sub[2] Cut_chg(0..8)+Message_on montage (reai-v2-room-fixes #2, committed 2b7f77e6); the intro-handoff auto-door path already in aot_common.c L297-321.
  - *Dep:* FE-4  ·  *Akzeptanz:* NEW GAME plays the montage frame-set then hands control into the first playable room via the auto-advance door (no hang).
- ✅ **[FE-6 · S · DEPTH]** Formalize death→title→continue as a front-end state (fold the ad-hoc s_in_title block main.c L1841-1879 into the FE state machine), including the SsSeqStop audio kill on entry.
  - *Quelle:* FUN_8003694c HP<0 fade FSM + attract gate @0x80015810; re15_audio_seq_ctl(slot,2)=SsSeqStop (reai-v2-deferred-backlog Batch 3, Commit a53361a1).
  - *Dep:* FE-1  ·  *Akzeptanz:* YOU DIED → TITLE → NEW GAME reloads a clean run with no leftover death music or g_gameover state.

### S1-1 — Universal room load + door-graph traversal (all 80 RDTs load; walk room→room)
*Any STAGE1 room loads its own BG/camera-cuts/RDT/SCD/models/lighting from data (main.c de-hardcoded off ROOM1170), and the player can walk the real STAGE1 door graph between all 40 Leon rooms. This is the foundational breadth work everything downstream depends on.*

- ✅ **[RL-1 · L · DEPTH]** De-hardcode the room-load path: extract the ROOM1170-specific boot (pc_load_room_prop_set, msg block, RBJ cinematic, spawn) into a single generic per-room loader keyed off the loaded RDT, so boot and cross-room reload share one code path.
  - *Quelle:* re15_room_load (room_pc.c) + re15_room_apply_pending (room_common.c); main.c boot L584-1094 and cross-room reload L1987-2112 (currently duplicated/ROOM1170-biased).
  - *Dep:* none  ·  *Akzeptanz:* ✅ DONE (bf6f3ccb) — 12 rooms each boot ROOM<r>.RDT + ROOM<r>.RBJ + their own spawn; the 2 residues (ROOM1170 hardcoded spawn + forced intro flags 3/193 & 4/195) are now keyed off the room id / gated on the intro path (boot 0x1170/0x1240). ROOM1140 boot reads Ck(4,195) got=0 (clean); ROOM1170 keeps its intro (flags got=1 + Cut_chg(7)).
- ✅ **[RL-2 · M · BREADTH]** Build the STAGE1 door-graph table (per-room: door slot → dest stage/room/spawn xyz/yaw/cut) by parsing every RDT's Door_aot_set records; regenerate/commit the generator that already produced re15_room_spawns.h.
  - *Quelle:* Door_aot_set dest bytes pc[22]/pc[23] (re15_aot_door_params_t dest_stage/dest_room, re15_aot.h L111-114); the door_graph.py referenced in re15_room_spawns.h header.
  - *Dep:* RL-1  ·  *Akzeptanz:* A generated STAGE1 adjacency table where every door resolves to an existing room id and every non-boot room has ≥1 inbound door matching its re15_room_spawns[] entry.
- ✅ **[RL-3 · M · DEPTH]** Room-probe sweep: load + tick (main00 + sub00) all 40 Leon RDTs N frames and assert no parse/SCD crash; log spawns + camera count per room.
  - *Quelle:* re15-room-probe skill (loads ROOM####.RDT into the C engine, runs SCD, ticks N frames, inspects g_actors+globals); rdt_common.c section parser.
  - *Dep:* RL-1  ·  *Akzeptanz:* A ctest that loads+ticks all 40 rooms with 0 crashes and produces a per-room spawn/camera manifest.
- ✅ **[RL-4 · L · BREADTH]** Per-room model-bank resolution: load the correct enemy EMD + NPC PLW + player PLD per room from the RDT/spawn table instead of the fixed test/elliot incbin bank.
  - *Quelle:* RE2 lazy model-stream model (ASSET_LAYER_REWRITE §2.1-2.3); Sce_em_set type → EMD file (EMD/ dir); reai-v2-stage1-enemy-rooms room→type map.
  - *Dep:* RL-1  ·  *Akzeptanz:* Each enemy room shows the correct model: maggot EM in ROOM11C0, dog in 1190/1230, crow in 10C0, spider in 1090 (autoshot spot-check).
- ✅ **[RL-5 · M · DEPTH]** Camera-cut (RVD) coverage per room: drive BG cut switching from each room's RVD table across all rooms, not just the tuned ones.
  - *Quelle:* RVD table @RDT+0x28 via FUN_80014230, gate DAT_800aca3c&0x100 (RE15_FUN_CATALOG 0x80042bac note); camera_common.c.
  - *Dep:* RL-1  ·  *Akzeptanz:* Walking a room switches BG cuts at the byte-true RVD boundaries; spot-check 5 rooms against a PSX capture (re15-parity-verify).
- ✅ **[RL-6 · M · DEPTH]** Door-traversal integration: script a walk from ROOM1140 through ≥3 connected rooms via real doors, confirming spawn pos/cut/band each crossing.
  - *Quelle:* Door graph RL-2; band gate FUN_8002bd44 @0x8002bf38 (aot_common.c L398-410); reai-v2-parity-oracle movement model.
  - *Dep:* RL-2,RL-4  ·  *Akzeptanz:* An RE15_INPUT_SCRIPT crosses ≥3 room boundaries; the arrival pos/yaw/cut at each equals the door-graph destination.

### S1-2 — HUD / Condition / Status screen (health, ECG, ammo, equipped weapon, ID card)
*A byte-true status/condition screen: the Fine/Caution/Danger/Poison classification, the real ECG waveform (retire the faithful-line trace), the equipped-weapon icon + ammo readout, and the character ID-card panel.*

- ✅ **[HUD-1 · S · DEPTH]** Byte-true condition classification (Fine/Caution/Danger, + poison state) from hp and the status word; wire it as the single source the status text + ECG read.
  - *Quelle:* Idle-FSM condition thresholds @0x8003206c (<0x32 caution, <0x1e danger — cited main.c L1414); DAT_800acaec player status word bit0x2 = bleed/poison (RE15_FUN_CATALOG).
  - *Dep:* none  ·  *Akzeptanz:* Condition text flips exactly at hp 49→caution and 29→danger; poison shows when bit0x2 set.
- ✅ **[HUD-2 · M · DEPTH]** RE the ECG waveform generator and replace the admitted faithful-line trace (re15_pc_ecg, main.c L96/L1413) with the byte-true per-condition waveform.
  - *Quelle:* Status-screen draw fn near @0x8003206c; RE the waveform table via the mzd_inv_open.sav framebuffer (re15-savestate-ghidra) since it is not in ghidra1_V2's covered range yet.
  - *Dep:* HUD-1  ·  *Akzeptanz:* The ECG trace pixel-matches the PSX status screen for Fine/Caution/Danger (framebuffer diff vs savestate).
- ✅ **[HUD-3 · S · DEPTH]** Equipped-weapon icon + ammo (magazine/reserve) readout on the status screen.
  - *Quelle:* item_icon_common.c icon table; equipped slot DAT_800b25c8 (re15_inv_equipped_slot); FUN_8004ea6c magazine + FUN_8004eb70 reserve (inventory_common.c L130-182).
  - *Dep:* HUD-1  ·  *Akzeptanz:* The equipped weapon's icon + mag/reserve counts on the status screen match g_inv after equip/fire/reload.
- ✅ **[HUD-4 · S · BREADTH]** ID-card / character portrait panel (STPIC_*.TIM) laid out per the inventory-screen ground truth.
  - *Quelle:* ITEM/STPIC_00..NN.TIM; layout from stage_saves/mzd_inv_open.sav (main.c L1393-1397 references LEFT = ID card + CONDITION/ECG).
  - *Dep:* HUD-1  ·  *Akzeptanz:* The correct portrait renders for the selected character in the status panel.

### S1-3 — Inventory / item interactions completion (combine, key-use, examine, item box)
*Finish the inventory FSM beyond equip+heal-use: settle whether RE1.5 has COMBINE, add medicine-mix/menu-reload combine, key-item USE, item EXAMINE (ITPS model + description), and the item box.*

- ✅ **[INV-1 · M · DEPTH]** RE the menu-confirm classification for non-weapon items to settle COMBINE-vs-no-op (currently SQUARE on ammo/key is a no-op, menu_common.c L91). Either implement COMBINE or byte-true-confirm the no-op and fix the comment.
  - *Quelle:* Menu FSM confirm handler @0x80046670 (sets DAT_800aca5d equip); FUN_8004a0cc combine; reai-v2-deferred-backlog menu-combine note (decompile garbled → multi-level disasm).
  - *Dep:* none  ·  *Akzeptanz:* Disasm proves the RE1.5 menu action model; behavior + comment match the bytes (implemented, or confirmed no-op with citation).
- ✅ **[INV-2 · M · BREADTH]** Item COMBINE: medicine mixing (Green+Green/Green+Red/etc → G.G / G.R MIX ids) and in-menu ammo reload.
  - *Quelle:* FUN_8004a0cc combine + FUN_8004e054/FUN_8004ebdc reload (cited inventory_common.c L26); medicine-mix result ids 0x24-0x2e (item name table inventory_common.c L224-227).
  - *Dep:* INV-1  ·  *Akzeptanz:* Combining Green(0x24)+Red(0x25) yields G.R MEDICINE MIX (0x27) byte-true; menu-reload merges ammo box→magazine.
- ✅ **[INV-3 · M · BREADTH]** Key-item USE: using a key/tool item triggers its door-unlock/event (consume or flag per item). *(Menu key-use = byte-true "You can't use it here."; the actual STAGE1 door-unlock is the ROOM card-reader keypad, now completable end-to-end — PROG-3.)*
  - *Quelle:* Key-use branch of the menu FSM (id≥0x22 classifier @0x80049124); door lock check (PROG-1); flag store FUN_8004ef90.
  - *Dep:* INV-1,PROG-1  ·  *Akzeptanz:* Using the correct key on its door sets the unlock flag + consumes/keeps the item byte-true, and the door then opens.
- ✅ **[INV-4 · M · BREADTH]** Item EXAMINE: rotate the per-item ITPS.ITP 112x72 model and show its description text (reuse the item-modal picture path).
  - *Quelle:* itps_common.c (id×0x3000 ITPS.ITP picture, already used by the pickup modal); examine/description text block; item_prompt_common.c glyph replay.
  - *Dep:* none  ·  *Akzeptanz:* Examining an inventory item shows its ITPS picture rotating + the byte-true description string.
- ✅ **[INV-5 · L · BREADTH]** Item box: deposit/withdraw with the 8-visible cap in box mode and post-removal compaction.
  - *Quelle:* FUN_80068f9c box enter/exit (0x0e deposit/0x0f withdraw, count forced 8); FUN_80069714 compaction; FUN_80069668 free-slot; DAT_800d4a3c 11 slots (RE15_FUN_CATALOG save/item-box section).
  - *Dep:* INV-1  ·  *Akzeptanz:* Deposit/withdraw round-trips items through a persistent box; the visible count clamps to 8 in box mode and compacts on removal.

### S1-4 — Progression: door locks, key items, event flags, puzzles
*The room graph is gated exactly as STAGE1 gates it — locked doors + their keys, one-way/visited flags, and the STAGE1 puzzle flags (red jewels, ladder button, Kendo/Brad events) — driven by the byte-true flag store.*

- ✅ **[PROG-1 · M · BREADTH]** ~~Add the door lock/key field to re15_aot_door_params_t + the Door_aot_set lock operands + the lock check and the "It's locked / you need X" message.~~ **BYTE-TRUE FINDING (audit wf_8d83d72a, 2026-07-25): the premise is RE2-only — a STAGE1 no-op.** Across all 262 STAGE1 `Door_aot_set` (0x3B) records pc[25..31] (the RE2 door_type/lock/locked/key operands) are **ALL ZERO**, and the RE1.5 door sce-handler @0x800430bc transitions **unconditionally** (reads no lock byte). So there is NOTHING to add to `re15_aot_door_params_t`. The ACTUAL STAGE1 lock is a **flag-gated sce1↔sce2 AOT-install SWAP** in the room SCD: inside `If(Ck(flag))…Else…`, the SAME slot at the IDENTICAL reach-rect installs either a **sce=1 MESSAGE AOT** (locked → shows the room's "It's locked."-type text via handler @0x80043084 → show_message) OR a **sce=2 real Door_aot_set** (unlocked → transition @0x800430bc). STAGE1 ships **412 such sce=1 message AOTs across 55 rooms** (e.g. ROOM1170 slot5 "It's locked from the other side.", ROOM1030 ×12). This is **already implemented byte-true** in the port (op_ck polarity `scd_vm.c:1555`; op_aot_set sce=1 → `RE15_AOT_TYPE_MESSAGE` → `re15_scd_show_message`; sce=2 → DOOR). **PINNED by `integration_door_lock`** (test_door_lock.c): a MESSAGE AOT fires the message FSM + does NOT queue a room change on an action press; the same reach as a DOOR AOT DOES transition. → the flag→door wiring itself is PROG-2.
  - *Quelle:* audit wf_8d83d72a (262 STAGE1 Door_aot_set bytes pc[25..31]==0); door interact = AOT action-scan `aot_common.c` (NOT FUN_8002bd44 = box-push); sce dispatch @0x8007469c ([1]=MESSAGE @0x80043084, [2]=DOOR @0x800430bc); msg_common message path.
  - *Dep:* RL-2  ·  *Akzeptanz:* ✅ a sce=1 "locked" door refuses entry + shows its message; a sce=2 "open" door transitions — verified deterministically by `integration_door_lock` (92/92 ctest).
- ✅ **[PROG-2 · M · DEPTH]** Wire the byte-true flag store to door/event gates: the indexed flag CHECK/SET ops (0x58/0x59) and the FUN_8004ef90/efe4 set/check, resolving through the DAT_800b0fd0 zone scratch. **DONE + disasm-verified (2026-07-25):** the store `re15_game_flag_get/set` is byte-identical to **FUN_8004efe4** (check: `0x80000000>>(idx&0x1f) & *(word)`) / **FUN_8004ef90** (set/OR) — decompiles read + matched. The gate ops are all cited: **op_ck (0x21, LAB_8003fcf4)** cond=`(flag!=0)^(expected==0)` → CONTINUE/IF_FALSE; **op_set (0x22, LAB_8003fdd0)** pc[3]=1 OR/0 CLEAR/7 TOGGLE; **op_flag_set2 (0x59, LAB_8003fe90)** idx=`work_vars[pc[2]]` (DAT_800b0fd0 lhu @0x8003feb8), bank=pc[1], MSB-first; **op_flag_ck2 (0x58, LAB_8003fd54)**. Pinned by **`unit_flag_gate`** (test_flag_gate.c): 0x59 (idx=work_vars) + 0x22 SET/CLEAR/TOGGLE round-trip through the store with no >31-idx bit-bleed, and `If(Ck(flag)) { Set witness } EndIf` fires the guarded op **IFF** the flag is set → the exact plumbing behind the PROG-1 door-lock sce1↔sce2 swap.
  - *Quelle:* LAB_8003fd54 (0x58) / LAB_8003fe90 (0x59) indexed flag ops; FUN_8004ef90 set / FUN_8004efe4 check (game_state.c re15_game_flag_get/set); DAT_800b0fd0[pc[2]]>>5 zone scratch (RE15_FUN_CATALOG #19); op_ck LAB_8003fcf4 / op_set LAB_8003fdd0.
  - *Dep:* none  ·  *Akzeptanz:* ✅ setting flag X (via 0x59/0x22) makes the X-gated `If(Ck)` event fire on the next run; clearing it suppresses the fire — verified deterministically by `unit_flag_gate` (93/93 ctest). *(The 3 named puzzle flags → their doors are PROG-4, which builds on this plumbing.)*
> **⚠ RE2-RETAIL PREMISE (audit 2026-07-25) — PROG-3/4 flag ids are NOT RE1.5.** A byte-true VM census
> (`integration_flag_census` + `RE15_FLAG_CENSUS=1`, documented in **`STAGE1_FLAG_MAP.md`**) shows the roadmap's
> named puzzles (red jewels, ladder button, Kendo) are **retail RE2 content**, and the specific ids are wrong:
> ROOM1030's real "Brad" gate is `Ck(z5,20/32/33/34)`, **not** `0x11/0x12`; info74 (the cited source) is the TCRF
> RE2-proto page and contains none of these flags. The *actual* STAGE1 flag layout is now censused: **z5** = per-room
> event/working flags, **z3** = cross-room story progression, **z4** = enemy-status/defeated latches (`z4/243` shared
> across ~10 rooms), **z12** = scenario gate (`z12/31`), plus the **`z1/27` + `z2/7`** global entry latches. PROG-3/4
> must be derived from that map. The plumbing that drives every gate is byte-true + verified (PROG-2).

- ✅ **[PROG-3 · M · BREADTH]** STAGE1 key→door map — **DONE + byte-true (audit wf_c78a98ec, 2026-07-25).** The lock is the PROG-1 sce1↔sce2 swap; the **key = a zone-9 possession flag** the pickup modal sets (`item_modal_common.c:241` `re15_game_flag_set(9,taken_bit,1)`), the card-reader sub checks it with `Ck(9,bit,1)` then keypad→`Set(3,idx,1)` unlocks the door. Table (in `STAGE1_FLAG_MAP.md`): ROOM10D0 **Blue**(id0x38) `Ck(9,52)`→`Set(3,50)`; ROOM11E0 **Yellow**(0x39) `Ck(9,138)`→`Set(3,139)`; ROOM1230 **Red**(0x37) `Ck(9,136)`→`Set(3,137)`; ROOM1100 Minidisc→`Set(4,232)`+z3/103. **The REAL gap (found + fixed): the readers are `sub20` (event 20); `RE15_RDT_MAX_SUB_SCD` was 16 → the sub table was truncated and `Evt_exec(20)`/AOT-event 20 returned −1, so no reader ever ran.** Raised **16→32** (max STAGE1-6 sub count = 28; original reads `first_off/2` dynamically). `op_ck`/`op_set` were already byte-true (PROG-2); `0x5E` is an unrelated stub. Pinned by `integration_keydoor`. **Keypad completed (d598ffc1):** the card-reader keypad is a dial-combination lock (cursor-on-grid); RE'd byte-true by hand-disassembling sub01.scd, and the two missing engine pieces implemented — the `op_member_cmp` prop-member path (`re15_prop_get_member`, member 15=`member_0b`=notch) + the notch mechanic (`re15_aot_object_notch` @0x80042f5c + per-frame `re15_object_notch_update` from `re15_game_step`). Pinned by `integration_keypad` (cursor→notch→confirm→4 digits→win→door). STAGE1 keycard doors completable end-to-end.
  - *Quelle:* disasm SCD `room{10D0,11E0,1230}/scd/{main00,sub20}` + raw `.scd`; `item_modal_common.c:241`; `rdt_common.c:203`; STAGE1_FLAG_MAP.md.
  - *Dep:* PROG-1  ·  *Akzeptanz:* ✅ key→door table documented; each keycard door flips MESSAGE→DOOR on its unlock flag and its reader sub is now reachable/dispatchable (`integration_keydoor`, 95/95 ctest).
- ✅ **[PROG-4 · L · BREADTH]** Per-room gate live-verification — **DONE.** **RE2-retail ids debunked** (ROOM1030 = `Ck(z5,20/32/33/34)`, not `0x11/0x12`; real map in `STAGE1_FLAG_MAP.md`). Live "set the flag → the gated door opens" proven on the REAL room RDTs: the 3 keycard doors (ROOM10D0/11E0/1230) each install their gated slot as **MESSAGE (locked)** while the unlock flag is clear and as **DOOR (open)** once it is set (`integration_keydoor`); the flag store/gate plumbing that drives it is byte-true (PROG-2).
  - *Quelle:* STAGE1_FLAG_MAP.md (real RE1.5 gates); disassembled per-room `main00`; runtime census.
  - *Dep:* PROG-2,RL-1  ·  *Akzeptanz:* ✅ each keycard gate's slot flips MESSAGE↔DOOR at the byte-true flag transition, live on real room data (`integration_keydoor`).
- ✅ **[PROG-5 · M · DEPTH]** ~~Visited/one-way flags (RE2-retail ids 0x1a/0x0e)~~ → **DONE via census (2026-07-25):** the real RE1.5 visited/one-way latches are the global **`Set(z1,27)` + `Set(z2,7)`** entry latches (set by ROOM1240/11A0/1170/11C0/11B0/1090… `main00` on entry) and the **`z3/139`** cross-room intro latch (ROOM1240 `Set` → ROOM11E0 `Ck`). Live-pinned by `integration_flag_census` (after ROOM1240 init `flag(3,139)==1`) — the writer lands in the same store the readers query, so a re-entered room reflects its latch (no double-trigger). Single-fire enforced by the byte-true `Set`/`Ck` (PROG-2).
  - *Quelle:* STAGE1_FLAG_MAP.md (censused `z1/27`,`z2/7`,`z3/139` latches); FUN_8004ef90 set / FUN_8004efe4 check.
  - *Dep:* PROG-2  ·  *Akzeptanz:* ✅ the intro latch persists in the store across a room re-enter (pinned by `integration_flag_census`, 94/94 ctest); byte-true `Set` idempotence prevents a first-visit event double-trigger.

### S1-5 — Per-room SCD events & cutscenes (data-driven sweep across the ~36 unverified rooms)
*Every STAGE1 room's main/sub SCD runs its real events, BGM, and cutscenes byte-true — not just the 4 already-verified rooms (1240/1140/1150/1170). Data-driven sweep first, then fix each divergence.*

- **[SCD-1 · L · DEPTH]** SCD coverage sweep: for each of the 40 Leon rooms, tick main00 + every sub and catalog every opcode reached, flagging op_unknown / walker-desync regions.
  - *Quelle:* re15-room-probe + scd-disassembly skills; scd_vm.c op table; reai-v2-scd-pad-predicates note that the offline walker desyncs in data regions (runtime is the arbiter).
  - *Dep:* RL-1  ·  *Akzeptanz:* A per-room opcode-coverage report; every op_unknown across STAGE1 is either RE'd + mapped or proven a data-region desync.
- **[SCD-2 · L · DEPTH]** Fix each per-room SCD event divergence the sweep surfaces (e.g. van-fire ROOM1090, Marvin's-desk ROOM1020, conference rooms 1060/10A0, shooting-range 1190).
  - *Quelle:* STAGE1 overlay per-room FUN_* (RE_15_Quellcode_Overlays/STAGE1/*.c, dispatched @0x80072bac); the specific divergence + a PSX capture.
  - *Dep:* SCD-1  ·  *Akzeptanz:* Each flagged room's event fires byte-true vs a DuckStation capture of the same room+action (re15-parity-verify).
- **[SCD-3 · L · DEPTH]** Cutscene coverage: enumerate STAGE1 cutscenes and verify the ones beyond Irons (Marvin desk ROOM1020, Sherry ROOM11B0, any Ben/Ada scenes) end-to-end.
  - *Quelle:* Per-room RBJ cinematic (RBJ/ROOM%04X.RBJ, main.c L870); sub_scd event triggers via RE15_FORCE_EVENT; reai-v2-room-fixes #5 Irons verification recipe.
  - *Dep:* SCD-1,RL-4  ·  *Akzeptanz:* Each STAGE1 cutscene plays to completion and returns control to the player (same criterion the Irons cutscene passed).
- **[SCD-4 · M · DEPTH]** Per-room BGM + SE_ON banks: each room loads and plays its byte-true music track and resolves its SE banks.
  - *Quelle:* Sce_bgm_control op 0x54 (reai-v2-gameplay-divergence-audit); Se_on 0x36 bank routing FUN_80045024 (reai-v2-stage1-gameplay-gaps se-on-bank fix); per-room SEQ/VAB.
  - *Dep:* RL-1  ·  *Akzeptanz:* Each STAGE1 room starts its correct BGM SEQ on load and its SE banks resolve to real samples (headless bank probe + audio spot-check).

### S1-6 — Enemies in real rooms + item pickups / examines
*Every STAGE1 room's actual enemy placement/count/wave logic is live-parity-correct (beyond the 3 already-checked rooms), the deferred wave-2 behaviors are finished, and every world item pickup + examine works.*

- **[ENE-1 · M · DEPTH]** Per-room enemy spawn/placement/count audit for all 40 Leon rooms (Sce_em_set), matching each against a PSX savestate.
  - *Quelle:* RDT Sce_em_set records; re15-room-probe spawn scan; reai-v2-stage1-enemy-rooms room→type map (1090 spider, 10C0 crow, 1190/1230 dog, 11C0 maggot, 11B0 NPCs).
  - *Dep:* RL-1,RL-4  ·  *Akzeptanz:* Each room's spawned types/positions/rotations match a DuckStation capture (same byte-true standard the 3 verified rooms met).
- **[ENE-2 · L · DEPTH]** Finish the deferred wave-2 enemy behaviors: Maggot heavy-bite/crawl-slew per-frame RNG + clip-root-motion speed, Spider hit-code arming, Zombie-Girl mode-dispatch, NPC wave-2 idle-state[4].
  - *Quelle:* reai-v2-deferred-backlog TIER-2: maggot @0x80117d1c slew / @0x8011bf50 root-motion; spider hit-arm @0x80019d50; zombie-girl mode @0x80120230; NPC idle @0x8010919c.
  - *Dep:* ENE-1  ·  *Akzeptanz:* Each deferred wave-2 behavior is byte-true + live-verified in its room against a PSX RAM read (per reai-v2-parity-oracle).
- **[ENE-3 · M · DEPTH]** World item pickups per room (Item_aot_set 0x50): grant the right id/qty and set the taken-flag so items don't respawn.
  - *Quelle:* Item_aot_set pc[14]/pc[16] LE decode + taken_bit DAT_800b1078 (reai-v2-stage1-gameplay-gaps Item_aot_set fix, Commit 6f9bf45a; re15_aot.h item_params taken_bit).
  - *Dep:* RL-1,PROG-2  ·  *Akzeptanz:* Every STAGE1 pickup grants byte-true (assert g_inv) and, once taken, does not re-appear on room re-entry.
- **[ENE-4 · M · DEPTH]** Examine AOTs / readable props per room (examine text, desk items, notes) via the EXAMINE_WORKVAR AOT path.
  - *Quelle:* EXAMINE_WORKVAR AOT + msg block (aot_common.c L427-441 examine reach); FUN_80028868 text renderer.
  - *Dep:* RL-1  ·  *Akzeptanz:* Examining a flagged prop shows its byte-true message; spot-check Marvin's desk (ROOM1020) and a note prop.

### S1-7 — Map & File (documents) screens + typewriter save
*The remaining menu screens: the floor MAP (MAP*.PIX), the FILE/document reader, and the typewriter SAVE/LOAD (which also backs LOAD GAME from the title).*

- **[MAP-1 · L · BREADTH]** Map screen: load + render the STAGE1 floor maps (MAP01-0A.PIX) with the current-floor highlight and a player marker.
  - *Quelle:* DATA/MAP01..0A.PIX (present in shared_assets); RE the map draw fn (RE2 map layer / RE2_Quellcode_Overlays); PIX→TIM decode (asset-pipeline skill).
  - *Dep:* HUD-1  ·  *Akzeptanz:* The map screen shows the correct STAGE1 floor map (B1/F3/B2) and toggles from the inventory tabs; player marker on the current room.
- **[FILE-1 · M · BREADTH]** File/document reader: a files list + paged document text, reusing the game-font text renderer.
  - *Quelle:* File/document text blocks (RDT/DATA msg blocks); FUN_80028868 TEX.TIM font renderer; msg_common paging.
  - *Dep:* none  ·  *Akzeptanz:* A collected STAGE1 file opens and pages its byte-true text; the file appears in the files list once obtained.
- **[SAVE-1 · L · BREADTH]** Typewriter save/load: a save-room AOT that writes a save slot (record layout) and LOAD GAME from the title that restores room/pos/inventory/flags.
  - *Quelle:* FUN_80032150 MC/CD op initiator (slot/IO/op-size/activate FSM); FUN_80034a04 save-record init; the save-room AOT + ink-ribbon gate.
  - *Dep:* FE-4  ·  *Akzeptanz:* Saving at a save room writes a slot; LOAD GAME from the title restores that exact room, position, inventory, and flag state.

### S1-8 — End-to-end STAGE1 parity + regression net
*Walk the entire STAGE1 critical path start→stage-exit, prove parity against the PSX original at each milestone, and lock it with regression tests so breadth work downstream can't silently regress STAGE1.*

- **[E2E-1 · M · DEPTH]** Author the STAGE1 critical-path script (boot → intro → briefing → escape → hub → branches → stage exit) as a deterministic RE15_INPUT_SCRIPT, using the door graph to route it.
  - *Quelle:* Door graph RL-2 + information74.txt room order; reai-v2-parity-oracle byte-true movement model (walk 75 / turn 48-96 / run CROSS).
  - *Dep:* S1-1..S1-6  ·  *Akzeptanz:* The script runs headless from new-game to the STAGE1 exit with no soft-lock and visits the expected room sequence.
- **[E2E-2 · L · DEPTH]** PSX parity capture of the same path in DuckStation + a state diff (pos/cut/flags/inventory) at each room boundary.
  - *Quelle:* re15-room-capture (drives DuckStation) + re15-parity-verify + re15-savestate-ghidra (RAM reads); the variable-fps direct-load caveat (reai-v2-parity-oracle).
  - *Dep:* E2E-1  ·  *Akzeptanz:* At each room boundary the port's state-log (position/cut/flag-zone/inventory) matches the PSX savestate within the byte-true tolerance.
- **[E2E-3 · M · DEPTH]** Add a regression ctest per milestone room (spawn + key SCD event + outbound door) so STAGE1 stays green as later stages are built.
  - *Quelle:* Existing ctest harness (79/79 green, reai-v2-build-recipe); re15-room-probe deterministic room tick.
  - *Dep:* E2E-1  ·  *Akzeptanz:* New per-room ctest cases pass and the full suite stays green; a deliberate spawn/flag break is caught by the suite.

## AREA: STAGE2-6 ROOM / CONTENT PARITY (80 distinct rooms across 5 stages + variants; currently load+AI-sandbox only, zero verified room parity)

Measured scope: STAGE2=16, STAGE3=16, STAGE4=16, STAGE5=24, STAGE6=8 distinct rooms (variant-0/Leon path) = 80 rooms, each with a second *1 variant → 160 RDT. Today the port loads any ROOMxxxx.RDT, ticks its SCD through the shared 68-handler VM, and routes all 50 enemy types, but NONE of STAGE2-6 has been live-parity-verified against PSX (only STAGE1 zombie/maggot/spider are). The work is therefore MOSTLY breadth-verification (per-room SCD/BG/AOT/event parity via the room-probe + room-capture harnesses) plus a handful of deep boss ports. Key structural facts that shape the plan: (1) rooms are data-driven — the SCD VM interprets each room's script, so "room parity" = proving the VM handles every opcode/event each stage uses, that BSS/MDEC decodes every background, that door-AOT tables build a traversable graph, and that spawns/camera/events match a PSX savestate frame-for-frame. (2) Enemy AI is already ported for every registered type; the remaining CODE work is boss polish — Birkin 0x30/0x36 Wave-2b (grab-hold/throw/charge subs of the ~13KB brain, root 0x80116230), Tyrant 0x2b and Ivy 0x2d live-verify+polish. These are code-only-deferred (registered, real brains) and need savestate-provoke to verify. (3) Alligator 0x23 is the one DATA-BLOCKED item: AI is byte-true (root 0x8010c448) but em023.EMD is absent from CDEMD0/1.EMS — model recovery needs disc-structure RE. (4) FMV is trivial (only CAPCOM.STR exists; RE1.5 doors are fade-only, no inter-room FMV). Ordered: Phase 0 lays cross-cutting foundation (opcode census, BG sweep, model mapping, transition graph, harness bring-up), then one phase per stage S2→S6 (each: room-by-room parity + its bosses + its events), then S7 cross-stage integration (transitions, FMV, second scenario variant, full playthrough). Bosses flagged code-deferred vs data-blocked below.


### Phase 0 — ROOM-PARITY FOUNDATION (cross-cutting; unblocks every stage)
*Stand up the infrastructure and verification harness so any STAGE2-6 room loads, ticks, renders, connects, and can be diffed byte-true against a PSX savestate. Do this FIRST — it is the reusable substrate every per-stage phase consumes.*

- **[F0-1 · M · BREADTH]** Run a per-room SCD opcode census for all 80 STAGE2-6 variant-0 rooms via re15-room-probe: load each ROOMxxxx.RDT, run main00+sub00, log every opcode actually dispatched, and diff the union against the 68 implemented op_ handlers in scd_vm.c to find any opcode STAGE2-6 uses that STAGE1 never exercised. (The offline SCD walker desyncs in data regions — runtime is the arbiter, per the scd-pad-predicates memory.)
  - *Quelle:* scd_vm.c op table (68 handlers) + RE15_SCD_OPCODES_REFERENCE.md (90 opcodes 0x00-0x8E) + .claude/skills/re15-room-probe/scripts; per-room SCD in STAGE{2..6}/ROOMxxxx.RDT
  - *Dep:* none  ·  *Akzeptanz:* A per-stage opcode-usage matrix; every opcode any STAGE2-6 room dispatches maps to an implemented handler (or a filed gap ticket). No unimplemented opcode reached at runtime.
- **[F0-2 · M · BREADTH]** BSS/MDEC background decode sweep across all 80 STAGE2-6 BSS packs (STAGE2=16, 3=16, 4=16, 5=24, 6=8): decode + render each and catch any VLC/MDEC edge (frame count, CLUT bank, tile geometry, mask/priority) that the STAGE1 backgrounds never hit.
  - *Quelle:* bss_mdec.c + bss_vlc.c + RE15_KNOWLEDGE §1.3 BSS (MDEC-VLC-ID 0x3800); the 80 STAGE{2..6}/*.BSS files
  - *Dep:* none  ·  *Akzeptanz:* Each room's background decodes without garbage/error and matches the PSX frame captured via re15-room-capture VRAM dump (VRAM crop x=440 per the parity_net memory).
- **[F0-3 · S · BREADTH]** Fix the enemy→EMS model-index mapping for STAGE2-6: extend the 25-entry DEFAULT_EMS_ORDER / re15_ems_index_for_type so every registered STAGE2-6 enemy binds an EMD; confirm all resolve except the known Alligator 0x23 data-gap.
  - *Quelle:* reai-v2-enemy-roster-complete memory (order skips 0x22/0x23; 29 EMS blobs, 25 mapped) + RE15MasterExtractor.java:45 + re15_ems.c + main.c:434-445 loader
  - *Dep:* none  ·  *Akzeptanz:* room-probe load of each STAGE2-6 enemy room logs a bound model for every registered type; the only 'model not found' is 0x23 (documented data-gap), graceful.
- **[F0-4 · M · DEPTH]** Per-stage overlay data-table residency audit: the ported enemy/boss AI reads stage-specific tables (HP @0x8011xxxx, hitbox, clip tables, the FUN_80100000 boss bone-render). Verify the port embeds the CORRECT per-stage bytes — STAGE2.BIN vs STAGE3.BIN differ, and the 0x30 handler is a +0x814-relocated copy in STAGE5. Ensure a stage-agnostic tick reads the right constants per stage.
  - *Quelle:* STAGE{2..6}.BIN pointer/data tables (file_off = addr − 0x80100000, no header) + roster memory relocated-copy finding (STAGE3 @0x8011690c = STAGE5 @0x80117120)
  - *Dep:* F0-3  ·  *Akzeptanz:* Each ported STAGE2-6 enemy's HP/hitbox/clip/damage-window constants trace to a cited offset in the correct stage's BIN; a diff tool confirms relocated copies are operand-only.
- **[F0-5 · M · BREADTH]** Extract per-room door/AOT transition tables for all STAGE2-6 rooms and build the room-connectivity graph (dest_room / dest_stage / spawn point / entry facing), so stages are walk-through traversable and cross-stage doors resolve.
  - *Quelle:* aot_common.c door handler + RDT AOT section (RE15_KNOWLEDGE §1.1) + Aot_set 0x2C / Door_aot_set SCD opcodes; dest_room=0 IS ROOM_x00 (subsystem-audit memory)
  - *Dep:* F0-1  ·  *Akzeptanz:* A connectivity map per stage; walking a door in room-probe/PC loads the correct next room + spawn/facing, matching the PSX debug-menu room adjacency.
- **[F0-6 · M · BREADTH]** Bring up the live-parity harness for STAGE2-6: establish the DuckStation debug-menu JUMP offsets (JUMP=room>>4, HEX) for each stage so any room can be savestate-provoked, confirm re15-room-probe loads all variant-0 rooms, and capture a baseline savestate for ≥1 room per stage.
  - *Quelle:* re15-room-capture (re15_quickload.py --left/--right N, HEX) + re15-room-probe + reai-v2-parity-oracle (same-room+same-path arbiter) + reai-v2-re-pitfalls (Debug-JUMP HEX)
  - *Dep:* none  ·  *Akzeptanz:* A documented (stage,room)→JUMP table covering all 80 rooms; a captured savestate exists for a representative room in each of STAGE2-6.

### Phase S2 — STAGE2 PARITY (16 rooms; Adult Spider 0x25, Alligator boss 0x23, FX-emitter 0x24, stub 0x22)
*Bring the first non-STAGE1 stage to byte-true room parity: every room's spawns/camera/BG/AOT/events match PSX, the Adult Spider full brain is live-verified, and the Alligator encounter is resolved (AI verified; model data-recovery attempted).*

- **[S2-1 · L · BREADTH]** Room-by-room SCD/BG/AOT parity pass ROOM2000-20F0 (variant 0, 16 rooms): for each room run room-probe tick + capture-diff spawn set, camera cuts, AOT layout, BG frame, and initial SCD branch against a PSX savestate.
  - *Quelle:* STAGE2/ROOM2xxx.RDT + STAGE2_overlay.c (9593 lines) + reai-v2-stage2-enemies room map (2020/21 zombie, 2030/31 boss, 2040 dog, 2050/60/A0 spider)
  - *Dep:* F0-1,F0-2,F0-5,F0-6  ·  *Akzeptanz:* Each of 16 rooms: byte-identical spawn list (type/pos/rot), camera cut set, AOT triggers, and BG vs the PSX savestate frame.
- **[S2-2 · M · DEPTH]** Alligator 0x23 AI live-verify: savestate-provoke the encounter and confirm HP=300 (row @0x801175dc), the grab-and-eat latch (DAT_800aca58=2 → eaten=3 at hp<0 @0x8010d27c/288, reach 800), hitbox {2200,720} @0x80118b98, and the ×2.5 skeleton scale.
  - *Quelle:* root 0x8010c448 (STAGE2.BIN) + re15_alligator_ai_tick + roster memory
  - *Dep:* S2-1,F0-6  ·  *Akzeptanz:* Live savestate RAM shows the grab-eat latch sequence + HP/hitbox constants byte-match the port; AI verified byte-true independent of the missing model.
- **[S2-3 · L · BREADTH]** Alligator 0x23 MODEL data-recovery (DATA-BLOCKED): locate em023 EMD — proven absent from CDEMD0/1.EMS — by RE'ing the disc structure / STAGE2 container / RDT-embedded model section; or conclusively prove it is omitted from the prototype disc.
  - *Quelle:* reai-v2-enemy-roster-complete data-gap note (all 29 EMS blobs parsed, 4 tail = dupes, no quadruped signature) + RE15MasterExtractor + RDT model section §1.1 + Java extractor domain
  - *Dep:* none  ·  *Akzeptanz:* em023 mesh/skeleton loads and renders in the port, OR a documented disc-structure proof that the model is absent from the available archives (data-gap closed either way).
- **[S2-4 · M · DEPTH]** Adult Spider 0x25 full-AI live-verify + deferred variants: confirm the dual-table brain (grid-DECIDE @0x80118e44 + substep-ACT @0x80118e64), grab-devour via player cmd-2 FSM (keep-alive clamp @0x8011259c), and provoke the deferred ceiling-drop (grid 2-0x1f, y=-10800) + 9-phase ambush choreography.
  - *Quelle:* root 0x801109e4, state table @0x80118e24 (STAGE2.BIN) + reai-v2-stage2-enemies (grab A @0x8011254c / B @0x801129cc, DAT_800aca58=2)
  - *Dep:* S2-1,F0-6  ·  *Akzeptanz:* Live spawn byte-identical (already confirmed), grab-devour timing matches a PSX provoke save, and the ceiling-drop variant is exercised in a grid 2-0x1f room.
- **[S2-5 · S · DEPTH]** Confirm FX-emitter 0x24 (billboard particle X-drift + wrap <-25000→20000, self-disabled collision @0x8010efe4) and stub 0x22 (all-leaves jr-ra, spawns inert via the 0-guard) render/behave byte-true.
  - *Quelle:* roots 0x8010ee9c (FX) / 0x8010c080 (stub) + FUN_8001a50c 0-handler guard (@0x8001a568) + roster memory
  - *Dep:* S2-1  ·  *Akzeptanz:* Emitter drift/wrap trajectory matches PSX; stub type spawns present-but-inert with no crash (0-guard parity).
- **[S2-6 · M · BREADTH]** Decode and verify STAGE2 scenario events/cutscenes/puzzles: per-room SCD event scripts (message boxes, flag sets, camera events, any key-item door gating) run to the same branch as PSX.
  - *Quelle:* STAGE2_overlay.c event helpers + per-room SCD (scd-disassembly skill) + SCD flag ops 0x58/0x59 (subsystem-audit memory)
  - *Dep:* S2-1  ·  *Akzeptanz:* Each room's event script reaches the same message/flag/camera outcome as the PSX savestate under the same trigger sequence.

### Phase S3 — STAGE3 PARITY (16 rooms; Cockroach 0x29, G-Birkin boss 0x30, Ada NPC 0x41)
*Reach byte-true parity for STAGE3 and land the biggest boss code-port: the ~13KB G-Birkin brain's remaining attack subs (Wave-2b), verified end-to-end in the ROOM3070 fight.*

- **[S3-1 · L · BREADTH]** Room-by-room SCD/BG/AOT parity pass ROOM3000-30F0 (16 rooms), same method as S2-1.
  - *Quelle:* STAGE3/ROOM3xxx.RDT + STAGE3_overlay.c (12767 lines); ROOM3040=cockroach, ROOM3070=Birkin boss
  - *Dep:* F0-1,F0-2,F0-5,F0-6  ·  *Akzeptanz:* Each of 16 rooms byte-identical spawns/camera/AOT/BG vs PSX savestate.
- **[S3-2 · M · DEPTH]** Cockroach 0x29 Wave-2 polish: exact scurry speed, reorient/lunge subs 7/8/9, and the -500 overwhelm attack @0x80112af8 (currently deferred faithful-line).
  - *Quelle:* root 0x80110b00, two-table brain @0x8011101c (A @0x8011ecc4 + B @0x8011ed04), HP row @0x8011d6e8, bite/heavy clips @0x80111f60/@0x801122ac
  - *Dep:* S3-1  ·  *Akzeptanz:* Scurry speed + subs 7/8/9 + overwhelm reproduce a PSX provoke save; per-frame behavior diff clean.
- **[S3-3 · L · DEPTH]** G-Birkin 0x30 boss Wave-2b CODE PORT (biggest boss task): port the remaining ACTIVE-brain subs of the ~13KB brain — sub 5 (grab-hold/throw), sub 6, sub 7, sub 8, sub 10 (charge combo clip 1→0xf→0x13 multi-hit) — plus exact per-sub damage windows and the exact death handler. (Wave-1 + subs 0-2/3-4/9-11 already ported.)
  - *Quelle:* root 0x80116230, ACTIVE brain 0x80116d38, tables grid @0x8011eea4 / decide-A @0x8011eeb8 / act-B @0x8011eef0, death state 0x8011a3f0; re15_birkin_ai_tick + reai-v2-stage2-enemies wave-2b list
  - *Dep:* S3-1,F0-4  ·  *Akzeptanz:* Each ported sub's clips/damage windows cite a STAGE3.BIN offset; re-RE the subs-6-8 cluster that overflowed the earlier agent cap; live provoke reproduces each attack.
- **[S3-4 · M · DEPTH]** G-Birkin ROOM3070 full-encounter live-verify: savestate-provoke and verify the whole state machine end-to-end (emergence sub9 → hub sub1 → BITE/LUNGE/CLAW/grab/charge → death → corpse), HP=300 @0x80116910, hurt-box {r1000,h1440} @0x8011ee64.
  - *Quelle:* reai-v2-stage2-enemies Birkin notes + savestate-provoke via re15-room-capture (STAGE3 JUMP)
  - *Dep:* S3-3,F0-6  ·  *Akzeptanz:* Full fight FSM matches PSX live RAM at each transition; the fight is playable to the boss's death and corpse settle.
- **[S3-5 · M · DEPTH]** STAGE3 boss-render setup FUN_80100000: the per-stage multi-part boss 3D draw (func_0x80019700 bone-matrix draws attaching model parts to skeleton bones @+0x188 stride 0xac). Confirm the port renders the Birkin composite model.
  - *Quelle:* STAGE3 FUN_80100000 (bone-attach draw loop, LAB_801178a8 bone-index table) + skeleton_common.c re15_skel_bone_to_world
  - *Dep:* S3-1,F0-3  ·  *Akzeptanz:* Birkin renders as the correct composite model posed on its skeleton (bone indices from the stage table), matching the PSX VRAM capture.
- **[S3-6 · M · BREADTH]** Ada NPC 0x41 behavior + STAGE3 scenario events/cutscenes (Ada escort/dialogue, key-item puzzle gating).
  - *Quelle:* STAGE3_overlay.c 0x41 handler + shared NPC lib (reai-v2-npc-ai state[4] 0x80050be8) + per-room SCD events
  - *Dep:* S3-1  ·  *Akzeptanz:* Ada NPC idle/scripted behavior and each STAGE3 event script reach PSX-identical outcomes.

### Phase S4 — STAGE4 PARITY (16 rooms; Tyrant 0x2b, Ivy 0x2d, Cockroach 0x29)
*Byte-true STAGE4 rooms plus live-verify + polish of the two humanoid bosses (Tyrant, Ivy) whose brains are ported but never runtime-verified.*

- **[S4-1 · L · BREADTH]** Room-by-room SCD/BG/AOT parity pass ROOM4000-40F0 (16 rooms), same method as S2-1.
  - *Quelle:* STAGE4/ROOM4xxx.RDT + STAGE4_overlay.c (10454 lines)
  - *Dep:* F0-1,F0-2,F0-5,F0-6  ·  *Akzeptanz:* Each of 16 rooms byte-identical spawns/camera/AOT/BG vs PSX savestate.
- **[S4-2 · M · DEPTH]** Tyrant 0x2b live-verify + polish: provoke and confirm the 2 attacks (sub4 -10 @0x80112898, sub14 -10 @0x80113ff8), grab-pin cmd5→6 @0x80112b34, emerge state4 (grid 0x40/41/43), hurt/death, HP-pool {86..126} @0x80118b00, hitbox {800,1710} @0x8011a094.
  - *Quelle:* root 0x801118d0 (STAGE4/5.BIN) + re15_tyrant_ai_tick + roster memory
  - *Dep:* S4-1,F0-6  ·  *Akzeptanz:* Live provoke save shows both attacks + grab-pin + emerge + death matching the port; all constants cite offsets.
- **[S4-3 · M · DEPTH]** Ivy 0x2d live-verify + polish: confirm the grab→instant-kill (DAT_800aca58=7 @0x80116858, no chip-hp), nav-chase humanoid (shared EXE walker + pathfind 0x800509e4/15000), HP=100 @0x80116954, hitbox {450,1530} @0x8011a2c8, and weapon-immunity (all-null damage row).
  - *Quelle:* root 0x801168c4 + re15_ivy_ai_tick + roster memory (Ivy weapon-immune like RE2 final form)
  - *Dep:* S4-1,F0-6  ·  *Akzeptanz:* Live provoke shows the instant-kill grab + nav path + weapon-immunity byte-match; hitbox/HP constants cited.
- **[S4-4 · M · BREADTH]** STAGE4 boss-render setup (FUN_80100000) + scenario events/puzzles (lab sequence, item gating).
  - *Quelle:* STAGE4 FUN_80100000 + STAGE4_overlay.c event scripts + per-room SCD
  - *Dep:* S4-1  ·  *Akzeptanz:* Tyrant/Ivy render correctly; each STAGE4 event script matches PSX outcome.

### Phase S5 — STAGE5 PARITY (24 rooms — largest stage; Tyrant 0x2b, Birkin 0x30/0x36)
*Byte-true parity for the largest stage (24 rooms → most event/puzzle content) and confirm the relocated Birkin/Tyrant handlers behave identically to their STAGE3/4 counterparts.*

- **[S5-1 · L · BREADTH]** Room-by-room SCD/BG/AOT parity pass ROOM5000-5170 (24 rooms), same method as S2-1 — the largest per-stage breadth block.
  - *Quelle:* STAGE5/ROOM5xxx.RDT + STAGE5_overlay.c (13174 lines)
  - *Dep:* F0-1,F0-2,F0-5,F0-6  ·  *Akzeptanz:* Each of 24 rooms byte-identical spawns/camera/AOT/BG vs PSX savestate.
- **[S5-2 · S · DEPTH]** Verify the STAGE5 Birkin 0x30/0x36 relocated handler (0x80116a44 = STAGE3's 0x80116230 +0x814): confirm the stage-agnostic re15_birkin_ai_tick is correct for the relocated copy (HP-300 write @0x80117120) and that any Wave-2b subs ported in S3-3 apply here too.
  - *Quelle:* roster memory relocated-copy proof (177/17872 bytes differ, all reloc operands) + STAGE5.BIN reg
  - *Dep:* S3-3,F0-4  ·  *Akzeptanz:* Diff confirms behavior-identical to STAGE3 Birkin; live provoke in the STAGE5 boss room matches.
- **[S5-3 · S · DEPTH]** Verify the STAGE5 Tyrant 0x2b instance (shares root 0x801118d0 with STAGE4) provokes identically; catch any STAGE5-specific HP-pool/grid-emerge variance.
  - *Quelle:* root 0x801118d0 + STAGE5.BIN reg + re15_tyrant_ai_tick
  - *Dep:* S4-2,F0-4  ·  *Akzeptanz:* STAGE5 Tyrant live provoke matches STAGE4-verified behavior; any stage-data delta cited.
- **[S5-4 · L · BREADTH]** STAGE5 scenario events/puzzles/cutscenes (24 rooms → the densest event content of the game): decode + verify every per-room SCD event script and boss-render setup.
  - *Quelle:* STAGE5_overlay.c event helpers + FUN_80100000 + per-room SCD (scd-disassembly)
  - *Dep:* S5-1  ·  *Akzeptanz:* Every STAGE5 event script reaches the PSX-identical branch; all cutscene camera/flag transitions match.

### Phase S6 — STAGE6 PARITY (8 rooms; finale, NPC 0x40 only, event/cutscene-heavy)
*Byte-true the endgame stage — no unique enemies (only NPC 0x40 registered), so the parity work is almost entirely the finale event/cutscene scripting and room presentation.*

- **[S6-1 · M · BREADTH]** Room-by-room SCD/BG/AOT parity pass ROOM6000-6070 (8 rooms), same method as S2-1.
  - *Quelle:* STAGE6/ROOM6xxx.RDT + STAGE6_overlay.c (682 lines, 33 funcs)
  - *Dep:* F0-1,F0-2,F0-5,F0-6  ·  *Akzeptanz:* Each of 8 rooms byte-identical spawns/camera/AOT/BG vs PSX savestate.
- **[S6-2 · M · BREADTH]** STAGE6 finale event/cutscene scripting: port + verify the endgame sequence (escape/ending choreography) driven by the 33 STAGE6 overlay functions + room SCD; confirm the ending trigger/flag path.
  - *Quelle:* STAGE6_overlay.c (FUN_80100000..FUN_801015ac) + STAGE6/ROOM6xxx.RDT SCD + NPC 0x40 handler
  - *Dep:* S6-1  ·  *Akzeptanz:* The finale sequence plays through to the ending state matching the PSX event/flag progression frame-for-frame.

### Phase S7 — CROSS-STAGE INTEGRATION (transitions, FMV, second scenario, full playthrough)
*Stitch the five verified stages into a continuous playable arc: cross-stage transitions with overlay/asset reload, movie playback, the second character/scenario variant, and an end-to-end STAGE2→6 integration test.*

- **[S7-1 · L · BREADTH]** Wire cross-stage transitions: doors whose dest is another stage must swap the overlay's stage-specific data tables and reload resident assets (RE2-style streaming) between stages, using the S7 asset-layer manager.
  - *Quelle:* aot door dest_stage handling (aot_common.c) + F0-5 transition graph + ASSET_LAYER_REWRITE.md (re15_res unified loader, shared staging buffer 0x42000)
  - *Dep:* F0-4,F0-5,S2-1,S3-1,S4-1,S5-1,S6-1  ·  *Akzeptanz:* Walking a cross-stage door loads the next stage's room + overlay data + assets with correct spawn/facing, no residency overflow; matches PSX stage-boundary behavior.
- **[S7-2 · M · BREADTH]** FMV/STR playback: wire a movie player for CAPCOM.STR (the only STR present) and any stage-transition MDEC stream, reusing the MDEC decoder. (Scope is small — RE1.5 doors are fade-only, no inter-room FMV.)
  - *Quelle:* MOVIE/CAPCOM.STR (6.1MB) + bss_mdec.c (MDEC decode reuse) + reai-v2-door-transition (doors are fade-only, silent)
  - *Dep:* none  ·  *Akzeptanz:* CAPCOM.STR decodes and plays at the correct point; any transition MDEC renders; no audio/video desync.
- **[S7-3 · L · BREADTH]** Second scenario variant (ROOMxxx1): port + verify the *1 room variants (second character path) across STAGE2-6; first confirm the variant semantics (player-0/Leon vs player-1/Elza) since RE15_KNOWLEDGE does not document it explicitly.
  - *Quelle:* STAGE{2..6}/ROOMxxx1.RDT (80 variant files) + RDT header player field (§1.1) + RE2 two-scenario naming convention (info/re2leon)
  - *Dep:* S2-1,S3-1,S4-1,S5-1,S6-1  ·  *Akzeptanz:* Variant semantics confirmed from RDT bytes; each *1 room reaches byte-true parity (spawns/camera/events) for the second scenario the same way *0 did.
- **[S7-4 · L · BREADTH]** Full STAGE2→6 playthrough integration test: traverse each stage end-to-end via the door graph, hitting every spawn set, event, and boss in sequence, as a repeatable regression run alongside the existing ctest suite.
  - *Quelle:* F0-5 connectivity graph + all per-stage acceptance from S2-S6 + reai-v2-parity-oracle (same-room+same-path)
  - *Dep:* S2-6,S3-6,S4-4,S5-4,S6-2,S7-1,S7-3  ·  *Akzeptanz:* A scripted traversal walks the whole STAGE2-6 arc (both scenarios) with each room/boss/event matching its per-stage byte-true baseline; added as a CI regression.

## AREA: ENGINE BYTE-TRUE RESIDUALS — closing the ~43 OPEN backlog + the ~35 faithful-lines to byte-true (all DEPTH: polish of the existing 43-file / ~22k-line engine, not new content)

The engine core (SCD VM, RDT, render, collision, camera, skeleton, lighting, AOT, PC-audio) is 85-95% byte-true; the ROOM1140 combat loop is the strongest-verified slice. The residual work is NOT one flat list — the deferred-freshness audit (wf_40eb900f) proved only ~43 of 219 markers are real OPEN backlog, ~half of the rest are lying comments already resolved. So the plan FRONT-LOADS a triage/reconciliation phase (R0) to kill stale claims cheaply, then works the genuine residuals ordered by (a) impact on the live gameplay loop and (b) tractability. Cross-checking the coverage map against the actual source revealed several map "unknowns" are ALREADY byte-true (bone-0 root decode FUN_8001f3bc done 2026-06-15; neck head-look FUN_80037358 done; letterbox 24px + fade 0xF0 curve FUN_80021a0c done; espr_on owner-rotation FUN_80041864 done; gore-bone LAB_8011f784 RE'd, impl pending) — those become verify-and-close tasks, not RE tasks. The real remaining RE effort is: (R1) 6 core-loop AI/combat residuals that a player WILL hit (melee blade-tip origin, gore-fx bone spawn, Sce_em_set packed-pose spawn, live idle-state[4], zombie-girl + maggot wave-2 dispatch); (R2) SCD opcode completion (Kage_set + Cut_be_set are stubbed to no-op via op_unknown; Ifel_ck AND/OR chain, 0x80-0x8E reconcile, full opcode-table audit); (R3) presentation (MDEC BG VRAM dest FUN_8002bdf4 hardcoded 640,256; ESP routines 12-19 muzzle/room chains + CLUT fade + droplet params); (R4) skeleton spring FUN_80024e40 + RBJ un-overlay Plc_ret; (R5) VAB SPU pitch/ADSR (U8/U9 — coupled to the PSX target build, another area); (R6) menu-combine confirm + the 0x47-INIT invuln-risk + a final faithful-line sweep. Each task cites a FUN address / file:line / RDT field and a byte-true acceptance test — most verifiable deterministically via the re15-room-probe ctest or a DuckStation savestate diff, a few (MDEC BG, VAB SPU) only on a PSX build. Verify the mechanism against the bytes, never "sieht richtig aus".


### R0 — Triage & OPEN-list reconciliation (do FIRST — cheap, prevents wasted RE on already-resolved items)
*Produce ONE authoritative, code-anchored OPEN list from the 43 backlog items + the coverage-map 🟡/❌ rows, distinguishing genuinely-open RE from already-byte-true (stale comment) from legit faithful-line. The deferred audit already showed ~half the markers lie; the coverage map is stale on ≥5 items.*

- **[R0-1 · S · DEPTH]** Re-verify every coverage-map 🟡/❌ row and every reai-v2-deferred-backlog TIER item against the CURRENT source. Confirmed-stale-so-far (demote to verify-and-close, NOT RE): bone-0 root decode (skeleton_common.c:229-277 gpf12/gpl12), neck head-look (skeleton_common.c:311-374), letterbox 24px + fade 0xF0 curve (fade_common.c:72-86), espr_on owner-rotation (scd_vm espr, Commit ee01665b). Tag each of the 43 as {OPEN-RE | VERIFY-CLOSE | FAITHFUL}.
  - *Quelle:* RE15_FUN_COVERAGE.md §🟡/❌ vs skeleton_common.c:229/311, fade_common.c:72, scd_vm.c espr; reai-v2-deferred-backlog TIER-1/2 lists
  - *Dep:* none  ·  *Akzeptanz:* Every item carries a file:line + FUN cite + one of the 3 tags; coverage map + backlog memory updated so no lying rows remain; count of true OPEN-RE items is fixed (expect ~20-25, not 43).
- **[R0-2 · S · DEPTH]** Classify each OPEN-RE item by its verification harness: (a) deterministic re15-room-probe ctest, (b) DuckStation savestate byte-diff (enemy array 0x800acc2c stride 0x1f4 / player 0x800aca88), (c) requires a green PSX build (audio SPU, MDEC BG). This fixes the ordering and flags the PSX-build-coupled items early.
  - *Quelle:* re15-room-probe + re15-savestate-ghidra skills; ASSET_LAYER_REWRITE.md (PSX build state)
  - *Dep:* R0-1  ·  *Akzeptanz:* Each OPEN-RE item has a named harness + a concrete provoke recipe (room number + input script or savestate); PSX-only items explicitly marked blocked-on-PSX-build.

### R1 — Core-loop AI/combat residuals (highest player-visible impact; ~13 of 43 touch the live loop)
*Close every residual a player hits during normal STAGE1 combat: melee geometry, gore spawn point, scripted enemy spawns, and the idle/wave-2 enemy sub-behaviors that currently resume-the-brain instead of running the real dispatch.*

- **[R1-1 · M · DEPTH]** Melee hit-test ORIGIN = blade-tip world point, not player-center. re15_damage.c already reads the bone-11 blade point (re15_damage.c:394/411/454) but game_step_common.c:406 still notes the port measures from player-center on the SCD-driven path — reconcile so ALL melee paths pass *(0x800acbdc)+0x7b8.
  - *Quelle:* re15_damage.c:394-454 (blade point @0x800353a4-c8, DAT_800acbdc+0x7b8); game_step_common.c:406
  - *Dep:* R0  ·  *Akzeptanz:* Knife swing hit-point world-xyz == the PSX blade-tip transform for a fixed savestate frame (integer-exact); re15-room-probe knife swing in a zombie room lands the same hit vs player-center baseline.
- **[R1-2 · M · DEPTH]** Gore/blood spawns at the model_inst BONE, not the actor root (feet). RE is DONE: bone_idx=LAB_8011f784[type] (byte table), matrix=entity+0x188+idx*0xac, translation@+64; zombie 0x10/0x11/0x16=bone14, zombie-girl/writhers=bone8. Implement by posing the enemy (re15_skel_compute_pose QUERY mode) + re15_skel_bone_to_world(poses[LAB_8011f784[type]].trans,...) — the infra already exists in re15_enemy_update_attack_point. Fixes the attack-point stand-in too if it shares the table.
  - *Quelle:* FUN_80106edc @0x80106ffc-0x80107030, LAB_8011f784 (byte table); re15_damage.c:541/563/579/588/837/839
  - *Dep:* R0,R1-1  ·  *Akzeptanz:* For a savestate zombie pose, blood spawn xyz == FUN_80106edc(bone14) integer-exact; blade/gore share LAB_8011f784 verified; visible blood emits from the bite bone not the feet.
- **[R1-3 · M · DEPTH]** Sce_em_set (0x44) packed-pose spawn: pc[18]!=0 → spawn the enemy directly in state 4 with the packed pose from pc[18]/pc[19]/pc[14] (pre-posed corpses/scripted enemies), instead of the current default state. Needs a small handler restructure (latent per backlog).
  - *Quelle:* scd_vm op_sce_em_set spec @0x800425a4-e8 (documented in Batch-2 backlog)
  - *Dep:* R0  ·  *Akzeptanz:* A room whose Sce_em_set carries pc[18]!=0 spawns state=4 + the packed pose bytes == disasm; re15-room-probe F0 dump matches a savestate spawn of that room.
- **[R1-4 · M · DEPTH]** Live idle-state[4] dispatch: FUN_8010919c is undispatched (enemy_ai_common.c:2537) — the zombie idle sub-behavior (RNG-driven fidget/turn while dormant). Wire case RE15_AI_STATE 4 → the real sub-behavior via PTR_FUN_801217a0[4]. CAUTION per backlog: verify the RNG stream draw-order is preserved (idle re-roll must not shift combat RNG).
  - *Quelle:* FUN_8010919c @0x8010919c; dispatch *PTR_FUN_801217a0[e->state] (enemy_ai_common.c:137)
  - *Dep:* R0  ·  *Akzeptanz:* Idle zombie runs the state-4 sub-behavior == disasm; RNG stream after N idle frames matches a live savestate; combat wake/grab gates (line-23 memory) unchanged.
- **[R1-5 · M · DEPTH]** Maggot wave-2 crawl clip/slew + crawl SPEED root-motion. Clip is DETERMINISTIC (+0x7 blind→clip4 / LOS→clip5) but the slew is per-frame rng ((rand8()&0x1f)+6, NOT flat 0x20) @0x80117d1c; crawl speed must come from move-helper 0x8011bf50 (clip root-motion), not the flat re15_dog_advance(40). Do as a BATCH (RNG-stream/draw-order risk flagged in backlog).
  - *Quelle:* @0x80117d1c (slew), @0x8011bf50 (crawl move-helper); enemy_ai_common.c:3807/3856/3858/3906
  - *Dep:* R1-4  ·  *Akzeptanz:* Maggot per-frame crawl slew + advance distance == disasm on a direct-load walk_probe; HEAVY-BITE lunge uses 0x8011bf50 stride; RNG stream draw-order-safe (spawn positions unchanged vs mzd_stage1_maggot.sav).
- **[R1-6 · L · DEPTH]** Zombie-girl wave-2 mode-dispatch (+0x9&0xf @0x80120230) + lunge-arm timer choreography; currently enemy_ai_common.c:4170/4239 resume-the-brain. Requires a STAGE1 zombie-girl provoke (Root 0x8010a8c8 — reuses standard zombie combat, new only INIT+nav). Decode the mode table + arm timers.
  - *Quelle:* @0x80120230 (zombie-girl mode table); enemy_ai_common.c:4156-4239; Root 0x8010a8c8
  - *Dep:* R1-4  ·  *Akzeptanz:* Zombie-girl sub-modes (attack/turn) fire == disasm; savestate diff of a provoked zombie-girl matches per-mode transitions; standard combat gates still byte-true.

### R2 — SCD VM opcode completion (the last undecoded/stubbed opcodes + a full table audit)
*Every SCD opcode 0x00-0x5E has a byte-true handler, no opcode silently no-ops that the original acts on, and the offline walker no longer desyncs. Two opcodes (Kage_set shadow, Cut_be_set) are currently routed to op_unknown = no-op and must be RE'd.*

- **[R2-1 · M · DEPTH]** Kage_set (shadow) — currently unregistered → op_unknown no-op (scd_vm.c:307/331). RE the handler off the jump table 0x800744a8 and implement the scripted shadow-blob placement (distinct from the auto shadow-blob under actors).
  - *Quelle:* jump table 0x800744a8 Kage_set entry; RE2 Kage_set ref (RE2_Quellcode_Overlays); shadow_blob_psx.h
  - *Dep:* R0  ·  *Akzeptanz:* A room that issues Kage_set places the shadow at the disasm-derived coords; re15-room-probe shows the shadow entity created (was silently dropped).
- **[R2-2 · M · DEPTH]** Cut_be_set — currently unregistered → op_unknown no-op (scd_vm.c:308/331). RE + implement the camera cut-behind / cut-region-behind set.
  - *Quelle:* jump table 0x800744a8 Cut_be_set entry; RE2 Cut_be_set ref; camera_common.c cut model
  - *Dep:* R0  ·  *Akzeptanz:* A room issuing Cut_be_set switches the active cut per disasm; camera cut sequence matches a savestate at the trigger frame.
- **[R2-3 · M · DEPTH]** Ifel_ck chained AND/OR condition eval (FUN_80053f50). Verify the current opcode-native predicate chain (op_if enters body, first opcode IS the predicate — U4) correctly handles CHAINED AND/OR beyond single predicates; implement the chain combinator if any room exercises it.
  - *Quelle:* FUN_80053f50 (chained predicate); dispatch 0x800744a8; scd_vm.c op_ck/op_cmp (U4)
  - *Dep:* R0  ·  *Akzeptanz:* A multi-predicate Ifel with mixed AND+OR takes the same branch as disasm for all operand combinations; unit test over the 7 comparators + chain.
- **[R2-4 · M · DEPTH]** Reconcile opcodes 0x80-0x8E (~8 stubbed length-1 entries, scd_vm.c:183-185). The RE1.5 jump table 0x800744a8 ENDS at 0x5E — so 0x80-0x8E are either port-added pseudo-ops or should route to op_unknown. For each, either cite a real handler FUN or prove it is dead and document (Se_vol 0x80 is already real).
  - *Quelle:* scd_vm.c:183-185, jump table 0x800744a8 (ends @0x5E, 95 entries); Se_vol 0x80 @0x8003f... 
  - *Dep:* R0  ·  *Akzeptanz:* Each 0x80-0x8E entry has either a FUN cite or an explicit 'not in RE1.5 dispatch, dead' note; no opcode in 0x00-0x5E is mishandled.
- **[R2-5 · S · DEPTH]** Member_calc (0x55) + fade_config (0x56) byte-true verify (both registered but confirm semantics vs RE2's Member_calc2 which does NOT exist in RE1.5). 0x56 = fade CONFIG @0x80042a58→FUN_800217b0, not Member_calc2.
  - *Quelle:* scd_vm.c:322/325; 0x56 @0x80042a58→FUN_800217b0
  - *Dep:* R0  ·  *Akzeptanz:* op_member_calc writes the member per disasm; op_fade_config sets the fade channel config == FUN_800217b0; comments corrected.
- **[R2-6 · M · DEPTH]** Full opcode-table reconcile audit (the coverage-map 0x18/0x2B mismatch note + offline-walker desync). Cross-check every 0x00-0x5E {length, handler} against jump table 0x800744a8 so the offline SCD walker stops desyncing in data regions (0x2D=fix 34B trap noted in memory).
  - *Quelle:* jump table 0x800744a8; RE15_FUN_COVERAGE ❌ '0x18 mismatch'; scd-disassembly skill opcode table
  - *Dep:* R2-4  ·  *Akzeptanz:* Offline walker decodes every STAGE1 room SCD with zero desync vs the runtime opcode-count log; length/handler table matches the jump table entry-for-entry.

### R3 — Presentation residuals (MDEC BG dest, ESP effect chains, CLUT fade)
*Byte-true the background upload destination and the remaining ESP effect routines/params. MDEC BG is PSX-target (needs the PSX build); ESP is shared and testable on PC.*

- **[R3-1 · M · DEPTH]** MDEC BG VRAM dest from the RDT camera record, not hardcoded (640,256). bg_psx.c hardcodes BG_VRAM_CACHE_X/Y = 640/256; FUN_8002bdf4 derives the dest (and the per-cut source) from the RDT cam fields +8/+10. Wire the cam-record read into the BG upload.
  - *Quelle:* FUN_8002bdf4 (RE2_Quellcode/FUN_8002bdf4.c ref); bg_psx.c:63-64 (hardcode); RDT cam record +8/+10
  - *Dep:* R0  ·  *Akzeptanz:* BG VRAM dest == RDT cam-derived coords for each cut of a multi-cut room; on the PSX build the correct BG shows per cut (BLOCKED-ON-PSX-BUILD).
- **[R3-2 · L · DEPTH]** ESP routineA selectors 12-19 (muzzle/shell/room chains). Currently 0/3/4/5/8/9/11/16 + routineB-12 (floor bounce) are implemented; 13/14/15/17/18/19 act as noop (re15_esp.c:376). Decode the remaining selectors off the 48-entry table @0x80071d40 / dispatch FUN_80019e20 @0x80019e84.
  - *Quelle:* FUN_80019e20 @0x80019e84-9c; routine table @0x80071d40; re15_esp.c:370-460
  - *Dep:* R0  ·  *Akzeptanz:* Each of routines 13-19 runs the disasm-cited action (muzzle flash / room-bank spawn / etc.); a gunshot+muzzle sequence matches the routine progression in a savestate.
- **[R3-3 · M · DEPTH]** ESP CLUT fade stages (routine 10: tpage|=row[0x16], clut+=row[0x1e]<<6, render-side CLUT fade @0x800176b0) + the droplet ballistic params currently faithful-line (accel/lifetime via FUN_800174e4). Byte-true the CLUT fade and the droplet seed/accel from the row bytes.
  - *Quelle:* @0x800176b0 (routine 10 CLUT fade); FUN_800174e4 (droplet physics/anim); re15_esp.c:452
  - *Dep:* R3-2  ·  *Akzeptanz:* Blood/gore CLUT fades through the disasm-cited stages; droplet velocity/lifetime derived from row bytes == disasm (no more uninit/faithful seed where the byte exists).
- **[R3-4 · M · DEPTH]** Neck damped-spring secondary motion FUN_80024c30/FUN_80024e40 (the SEPARATE spring chain, not the Plc_neck head-look which is byte-true). Decode it and determine which actors run it; implement or prove unused. anim_bone_follow_3d.
  - *Quelle:* FUN_80024e40 / FUN_80024c30 (anim_bone_follow_3d.c); skeleton_common.c:312 note
  - *Dep:* R0  ·  *Akzeptanz:* Either the spring is implemented and a spring-driven bone matches a savestate per frame, or it is proven to have zero live callers in STAGE1-6 (dead) and documented.

### R4 — Skeleton / player-anim residuals (verify-close + RBJ overlay)
*Close the skeleton items the coverage map still lists as unknown/partial but that the source shows resolved, and finish the one real gap (RBJ un-overlay).*

- **[R4-1 · S · DEPTH]** Verify-and-close bone-0 keyframe root decode (coverage map lists it 'known wrong' — but skeleton_common.c:229-277 shows it RE'd 2026-06-15 as FUN_8001f3bc gpf12/gpl12 root lerp, eating/lying enemies grounded). Confirm byte-true vs disasm and delete the stale coverage-map row.
  - *Quelle:* FUN_8001f3bc (RE_15_Quellcode_V2/FUN_8001f3bc.c:28-57); skeleton_common.c:229-277
  - *Dep:* R0  ·  *Akzeptanz:* Root translation per frame for an eating-zombie clip == FUN_8001f3bc gpf12/gpl12 (feet grounded ~-166); coverage map row removed.
- **[R4-2 · M · DEPTH]** RBJ un-overlay (Plc_ret). The player RBJ overlay-animation restore path — LAB_80041f88 Plc_ret un-overlays the RBJ clip back to the base pose. Verify the current player-mode FSM handles it; implement the un-overlay if missing.
  - *Quelle:* Plc_ret LAB_80041f88 (player-mode FSM dispatch FUN_80031c44); RBJ (room1170_* / PL00 RBJ)
  - *Dep:* R4-1  ·  *Akzeptanz:* After an RBJ overlay clip completes, the player pose returns to base == disasm; ROOM1170 cinematic RBJ transitions match the savestate.
- **[R4-3 · S · DEPTH]** Plc_neck neck-data init fields currently omitted (skeleton_common.c:327). Restore the full neck-data init so head-look is correct outside room1150's in-range case (currently 'stays within range' shortcut).
  - *Quelle:* Plc_neck neck-data init (skeleton_common.c:327 note); FUN_80037358
  - *Dep:* R4-1  ·  *Akzeptanz:* Head-look with an out-of-range initial target settles correctly (no snap) == disasm; a cutscene with a wider neck target matches the savestate slew.

### R5 — VAB audio byte-true (U8/U9 — coupled to the PSX target build in another area)
*Byte-true SPU pitch + ADSR for SFX. The tone-driven pitch/ADSR is ALREADY wired in audio_psx.c play_sample; the residual is the Se_on→note+bank plumbing and the 22050 fallback, and it can only be VERIFIED on a green PSX build (the PC path uses the SDL mixer with legit approximations).*

- **[R5-1 · M · DEPTH]** VAB pitch: plumb Se_on→note+bank→program so SFX pitch uses re15_vab_note2pitch(note,center,shift) instead of the 22050 fallback. play_sample already calls note2pitch(tn->center_note,...) when a tone is found (audio_psx.c:191-194); the gap is the incoming Se_on note + the per-bank base the LUT doesn't encode (U9).
  - *Quelle:* vab_common.c note2pitch + 192-u16 LUT; FUN_80045630 (EDT floor→bank/tone map); audio_psx.c:186-194; U9
  - *Dep:* R0,PSX-target-build(other area)  ·  *Akzeptanz:* On a PSX/DuckStation build, a room SE plays at the VAB tone's center pitch (not flat 22050); a gunshot's two stacked tones pitch per the EDT map.
- **[R5-2 · S · DEPTH]** VAB ADSR: verify the tone envelope path (tn->adsr1/adsr2 from tone+0x10/+0x12, audio_psx.c:198) always resolves the owning tone so SFX never fall back to the dummy 0x00FF/0x0000 (U8). Fix the tone-lookup (vag_index+1 match) if any SE misses.
  - *Quelle:* tone+0x10/+0x12 (re15_vab_tone_t); audio_psx.c:173-199; U8
  - *Dep:* R5-1  ·  *Akzeptanz:* On a PSX build, SFX have proper attack/decay/release matching the VAB tone envelope, not flat full-volume; no SE hits the dummy fallback.
- **[R5-3 · S · DEPTH]** Reconcile the 22050 Hz footstep/voice base: prove whether 22050 is byte-true (a per-bank base) or a PC-mixer artifact (its 2× downsample of 44100). Decide the SPU-path base from FUN_80045a64 (cam-dist atten) + the EDT tone.
  - *Quelle:* FUN_80045a64 (cam-dist attenuation); audio_psx.c:222-226/304; audio_pc.c mixer >>1
  - *Dep:* R5-1  ·  *Akzeptanz:* Footstep + voice SPU rate documented as either byte-true (with FUN cite) or a PC-only approximation (faithful-line, with the reason 44100/2 note); no unexplained magic 22050.

### R6 — Menu / inventory / init residuals + final faithful-line sweep
*Close the menu-combine question, de-risk the 0x47 INIT invuln trap, verify the per-thread work-slot leak fix, and do a final adversarial pass over the ~35 legit faithful-lines so each is either promoted to byte-true or proven impossible.*

- **[R6-1 · M · DEPTH]** Menu confirm on ammo/key (menu_common.c:91 no-op). STRONG prior it IS byte-true no-op (RE1.5 = direct id-classifier, NO COMBINE submenu — reai-v2-item-use-heal). RE the menu-FSM caller @0x80046670 (Equip set DAT_800aca5d) + FUN_8004a0cc to CONFIRM, then either correct the comment (if no-op) or implement reload/combine.
  - *Quelle:* FUN_800460b8 / confirm @0x80046670 / FUN_8004a0cc (combine); menu_common.c:91; reai-v2-item-use-heal
  - *Dep:* R0  ·  *Akzeptanz:* Menu confirm on ammo/key behavior == disasm (proven no-op OR reload implemented); the menu-FSM classification path is cited, not assumed.
- **[R6-2 · M · DEPTH]** 0x47-INIT-fields caution (enemy_ai_common.c:72, +0x9a=0xffff etc.). Before writing any INIT field, resolve which types take re15_enemy_ai_init (generic case RE15_AI_STATE_INIT) — writing hp=0xffff (-1) here risks making ALL init-ing types invulnerable. Confirm 0x47-Elliot vs the shared NPC-lib (real invuln already @0x80101918).
  - *Quelle:* re15_enemy_ai_init (enemy_ai_common.c:72/137); real NPC invuln @0x80101918 (:5273); PTR_FUN_801217a0
  - *Dep:* R0  ·  *Akzeptanz:* The set of types entering the generic INIT path is enumerated; 0x47 INIT fields written only if they don't leak into combat types (no combat zombie becomes invulnerable in re15-room-probe).
- **[R6-3 · S · DEPTH]** Verify the per-thread work-slot leak fix (U6: op_member_set2 / add_aspeed / plc_rot) under a concurrent multi-thread Work_set scene (the ROOM1170 intro never exercised it).
  - *Quelle:* scd_vm.c U6 (UNTESTED_IMPLEMENTATIONS.md); per-thread work slots
  - *Dep:* R2-6  ·  *Akzeptanz:* A multi-thread scene using Member_set2/Add_aspeed/Plc_rot under concurrent Work_set shows no work-slot leak; slot state == a savestate at frame N.
- **[R6-4 · M · DEPTH]** Final adversarial faithful-line sweep of the ~35 legit approximations (spider web-spit wobble uninit; player-model-facing points in several fx; VAB PC-mixer >>1). For EACH: either prove byte-true is impossible (uninit RAM / PSX-only) with a cite, or promote it. No approximation stands without a documented reason.
  - *Quelle:* reai-v2-deferred-backlog FAITHFUL-LINE list; reai-v2-spider-ai; coverage-map 🟡 audio
  - *Dep:* R1-6,R3-3,R5-3  ·  *Akzeptanz:* Every remaining faithful-line marker carries an explicit 'byte-true not determinable because X (cite)' note or is converted; the marker count reflects only genuine impossibilities.

## AREA: PSX Target + Streaming Asset Layer (PSn00bSDK hardware target + RE2-style CD streaming)

The PSX target was written but NEVER built or run here, and it is not in a linkable state: the custom cmake/FindPSn00bSDK.cmake does not provide psn00bsdk_add_executable/psn00bsdk_target_incbin, platform/psx/CMakeLists.txt uses a raw add_executable + `objcopy -O binary` (no crt0, no SDK linker script, no valid PS-EXE header), and the resident-asset externs (test_tim/md1/edd/emr, pl00w01_*, test_vh/vb, test_bss) have no incbin → undefined symbols at link. iso.xml bundles only SYSTEM.CNF+RE15.EXE so no CD asset the code reads is on the disc. PSn00bSDK and mkpsxiso are not installed on this machine. The plan is ordered strictly per the ASSET_LAYER_REWRITE mandate: (P0) make the target LINK into a valid PS-EXE; (P1) put assets on a bootable ISO and boot the room1170 intro in DuckStation; (P2) fix the RAM budget crisis (369KB RDT buffer, stream the player-model incbin, safe staging consolidation honoring src≠dst, one re15_resmap.h); (P3) finish the SPU audio path — the 4 SPU-load stubs, hardware-verify U8 ADSR, wire+verify U9 note2pitch for SFX, apply pitch-bend; (P4) build the RE2-style re15_res unified streaming layer (Stages 2/6/7/8), move enemy loads to the Sce_em_set spawn event, raise the enemy bank to RE2's 2 slots; (P5) dual-target PSX==PC==original byte-true parity. Every PSX RAM change is gated on `mipsel-none-elf-size` `_end` < 0x80200000 with ≥64KB stack margin, and every hardware claim is gated on DuckStation TTY/framebuffer evidence (the author cannot run the console build blind). DEPTH = byte-true polish/verify of existing systems on the real target; BREADTH = new streaming capacity/rooms.


### P0 — Toolchain + Linkable PS-EXE (the target must first compile, link, and produce a bootable executable)
*Turn the currently-unbuildable PSX target into one that links into a valid PSn00bSDK PS-EXE (crt0 + linker script + elf2x), with all resident-asset symbols resolved.*

- **[B0-1 · S · DEPTH]** Install and verify the PSX toolchain on this machine: PSn00bSDK (mipsel-none-elf-gcc + libs + its CMake package) and mkpsxiso; set PSN00BSDK_PATH; confirm DuckStation is drivable. Today `find_package(PSn00bSDK)` cannot resolve (empty PSN00BSDK_PATH) and mkpsxiso is not on PATH.
  - *Quelle:* cmake/psx_toolchain.cmake:27-32 (expects mipsel-none-elf-*); platform/psx/CMakeLists.txt:52 (find_program mkpsxiso); CLAUDE.md PSX build section
  - *Dep:* none  ·  *Akzeptanz:* `mipsel-none-elf-gcc --version` runs; PSN00BSDK_PATH points at a tree with lib/ ldscripts + a CMake package; `mkpsxiso --version` runs; DuckStation launches via psx_dev/re15_reborn/psxtest.sh
- **[B0-2 · M · DEPTH]** Replace the custom cmake/FindPSn00bSDK.cmake + raw add_executable(re15_psx) + manual `objcopy -O binary` with the SDK's canonical CMake package (psn00bsdk_add_executable), so the target links against crt0 + the PSn00bSDK MIPS-I linker script and is converted to a real PS-EXE via elf2x. The current path yields no _start/entry and a headerless raw binary that cannot boot.
  - *Quelle:* platform/psx/CMakeLists.txt:12-13,42-47 (add_executable + objcopy -O binary); cmake/FindPSn00bSDK.cmake (no psn00bsdk_add_executable/target_incbin defined); PSn00bSDKConfig.cmake (SDK-provided)
  - *Dep:* B0-1  ·  *Akzeptanz:* `cmake -DRE15_BUILD_PSX=ON … && cmake --build` links re15_psx.elf and emits RE15.EXE whose first 8 bytes are the ASCII magic "PS-X EXE"; `mipsel-none-elf-size` reports text/data/bss and an _end symbol
- **[B0-3 · S · DEPTH]** Restore the psn00bsdk_target_incbin declarations for the still-resident assets whose externs are currently undefined at link: test_tim/test_md1/test_edd/test_emr (PL00), pl00w01_edd/pl00w01_emr (W01 walk track), test_vh/test_vb (DOOR00 SFX VAB), test_bss (intro BG). Source bytes come from shared_assets/PSX. (These are the assets ASSET_LAYER later streams; incbin them now just to link, then reclaim in P2.)
  - *Quelle:* asset_psx.c:25-32,50-51 + audio_psx.c:41-43 + bg_psx.c:49-51 (externs); comments "psn00bsdk_target_incbin in CMakeLists.txt" (asset_psx.c:145, audio_psx.c:39, bg_psx.c:49)
  - *Dep:* B0-2  ·  *Akzeptanz:* link reports zero undefined symbols; each incbin'd blob byte-matches its shared_assets source (`cmp`); build produces RE15.EXE

### P1 — Bootable ISO + first frame (assets on the disc; boot the intro on real hardware)
*Produce a bootable RE15.bin/cue whose CD tree contains the assets the code reads, and bring up the room1170 helipad intro end-to-end in DuckStation.*

- **[B1-1 · M · DEPTH]** Populate iso.xml (today a TODO stub bundling only SYSTEM.CNF+RE15.EXE) with the CD asset tree the boot path reads, matching the EXACT 8.3-uppercase `;1` paths the code builds. Boot-room subset first: ROOM1170.RDT, \PLD\PL00.TIM, \RBJ\ROOM1170.RBJ, \PLD\ELLIOT.{MD1,EMR,TIM}, \EMD\EM21.EMD, \SOUND\MAIN32/SUB15.{VH,VB,SEQ}, \BSS\ROOM1170\BG%02d.BSS, \VOICE\VOICE%02d.VAG. Source from shared_assets/PSX.
  - *Quelle:* iso.xml:31-37 (stub); re15_room.c:33 (ROOM name); asset_psx.c:468/498-517/575 + audio_psx.c:275/598/605/612 + bg_psx.c:190 (exact CdSearchFile path formats)
  - *Dep:* B0-3  ·  *Akzeptanz:* mkpsxiso builds RE15.bin/cue; DuckStation TTY shows every boot-room re15_cd_load_file / CdSearchFile returns size>0 (not -1)
- **[B1-2 · S · DEPTH]** Wire the mkpsxiso ISO build target + license + region. platform/psx/CMakeLists.txt:52-66 gates re15_iso on find_program(mkpsxiso); iso.xml references LICENSEA.DAT (provide it or disable the license); confirm SYSTEM.CNF boots cdrom:\RE15.EXE;1 (already matches).
  - *Quelle:* platform/psx/CMakeLists.txt:52-66; iso.xml:20-21; platform/psx/SYSTEM.CNF
  - *Dep:* B1-1  ·  *Akzeptanz:* `cmake --build --target re15_iso` yields RE15.cue that passes the BIOS boot/license check in DuckStation (reaches the EXE entry, not a black/region error)
- **[B1-3 · M · DEPTH]** First hardware boot smoke via psxtest.sh + DuckStation TTY logging (TTYLogging=true + LogToFile per ASSET_LAYER §4 diag loop). Confirm the init order in main.c runs: GPU 320×240 double-buffer, pad, CdInit, GTE (InitGeom), SPU — first frame presented, no CPU/COP exception.
  - *Quelle:* main.c:99-119 (init order); ASSET_LAYER_REWRITE.md:108-110 (TTY diag loop); psx_dev/re15_reborn/psxtest.sh
  - *Dep:* B1-2  ·  *Akzeptanz:* duckstation.log shows g_engine.frame_count advancing with no exception; a captured framebuffer shows the intro (black + narrator) rather than a hang/crash
- **[B1-4 · M · DEPTH]** Bring up the full ROOM1170 intro chain on the console: MDEC BG decode (bg_psx DecDCTvlc; the A2 DrawSync/Reset fix), Leon skeletal render (mesh_psx GTE path), TEX.TIM narrator subtitles, sub11→sub00→sub02 cinematic handoff, letterbox + fade-in. main.c already wires all of this for PSX; the task is to verify it actually runs on hardware (it has only ever run on PC).
  - *Quelle:* main.c:124-366 (documented A1/A2/A6 fixes + pre-intro→helipad handoff); reai-v2-room-fixes memory (PC-verified intro baseline)
  - *Dep:* B1-3  ·  *Akzeptanz:* DuckStation capture of the helipad intro matches the PC build at the cut boundaries; `mipsel-none-elf-size` shows _end < 0x80200000 with ≥64KB stack margin

### P2 — RAM budget crisis (fit the biggest room + reclaim the fat incbin globals)
*Eliminate the 2MB overflow risk: size the RDT buffer to the true max, stream the player-model incbin, safely reclaim staging RAM, and account the whole budget in one header.*

- **[RAM-1 · S · DEPTH]** Raise s_room_rdt (re15_room.c:22, currently 0x50000=320KB) to hold the largest RDT. ROOM1190≈330KB is silently rejected today (re15_rdt_parse fails, room never loads). Measure the true max RDT across shared_assets/PSX and size to it (~369KB per the RE2 memory → 0x5C000).
  - *Quelle:* re15_room.c:22,33-35; re_enemy_memory_architecture memory ("s_room_rdt 0x50000 — TOO SMALL: ROOM1190=330KB rejected"; biggest RDT 369KB)
  - *Dep:* B1-4  ·  *Akzeptanz:* the measured-largest room's RDT loads (re15_room_load==0, re15_rdt_parse ok) on hardware; buffer ≥ that size; _end still under budget with stack margin
- **[RAM-2 · M · DEPTH]** Stream the player model (ASSET_LAYER Stage 3): replace the test_md1/edd/emr + pl00w01_edd/emr incbin (from B0-3) with a CD stream of \PLD\PL00.{MD1,EDD,EMR} and the W01 walk track into a resident work area at boot; drop the incbin (−~150KB). PL00.TIM already streams (asset_psx.c:575).
  - *Quelle:* ASSET_LAYER_REWRITE.md:83-84 (Stage 3, −150KB); asset_psx.c:624-666 (current incbin parse of test_md1/edd/emr + pl00w01)
  - *Dep:* RAM-1  ·  *Akzeptanz:* test_* / pl00w01_* incbin removed; _end drops ~150KB; DuckStation shows Leon's mesh + idle/walk/run animation identical to the incbin build (frame compare)
- **[RAM-3 · M · DEPTH]** Safe staging consolidation (ASSET_LAYER Stage 1 redo, −~100KB): reclaim re15_cd_staging (100KB) for loads that do NOT time-overlap another decode, strictly honoring the src≠dst invariant. The first attempt was reverted because the BG VLC decoder reads src from cd_staging and writes dst to psx_staging; merging them overwrote the decode source + spilled into camera/actor globals. Keep src≠dst for the MDEC/VLC path; only share buffers across non-overlapping loads.
  - *Quelle:* ASSET_LAYER_REWRITE.md:73-79,104-107 (Stage-1 revert + src≠dst rule; RE2 keeps 0x80190000 src vs 0x80198000 dst); re15_cdfs.c:9-11 (buffer-separation comment)
  - *Dep:* RAM-1  ·  *Akzeptanz:* −100KB _end with BG cut-change + enemy-load + async-voice all still correct on hardware — no "camera stuck / models misplaced" regression in a cut-heavy room
- **[RAM-4 · S · DEPTH]** Author re15_resmap.h: one header declaring every resident work area and its byte budget (code/.text, shared staging, room RDT, player model, NPC model, enemy bank, fonts/shadow/sprite, SPU) mirroring RE2's fixed work-area layout, so the 2MB is accounted in ONE place and overflow is caught by a compile-time/measure check, not a boot-hang.
  - *Quelle:* ASSET_LAYER_REWRITE.md:41-58 (§2.2 RAM-map table); re_enemy_memory_architecture (no-heap 2MB reality, _end<0x80200000 rule)
  - *Dep:* RAM-1,RAM-2,RAM-3  ·  *Akzeptanz:* a static sum of the map header's regions < 0x80200000 − stack reserve; all PSX loaders size their buffers from this header; _end tracks the map within tolerance

### P3 — SPU audio path (the 4 stubs + ADSR/pitch verify on hardware)
*Complete and hardware-verify the RE2-architecture SPU audio: the four SPU-load SE stubs, U8 ADSR, U9 note2pitch for SFX, and MIDI pitch-bend.*

- **[SPU-1 · M · DEPTH]** Implement re15_audio_room_se (audio_psx.c:233, snd1 combat/room-SE bank — currently a TODO stub). Mirror the footstep path: load the room's snd1 VH/VB (RDT-sliced like snd0, or CD) into a reserved SPU region, then play_sample_from by se_id. The player-hit SE 10 (re15_damage type<2) and general Se_on room SEs are silent on PSX until this exists.
  - *Quelle:* audio_psx.c:229-236 (stub cites FUN_800453d0); audio_pc.c (byte-true impl to match); reai-v2-deferred-backlog Batch 1 (player-hit SE 10 via FUN_800453d0)
  - *Dep:* B1-4  ·  *Akzeptanz:* on hardware a player hit / room SE keys the correct VAG at the right vol/pitch; A/B identical to the PC audio_pc.c backend
- **[SPU-2 · M · DEPTH]** Implement re15_audio_weapon_se + re15_audio_prime_weapon (audio_psx.c:241/247 stubs): mirror FUN_80043d8c to CD-load ARMS<weapon>.EDH/.VB into a reserved SPU region on equip, then play se_id (gunshot=8). Respect EDT tone-layering — the handgun gunshot keys 2 stacked VAGs (ARMS record 0 = 00 00 13 30 → tone1+tone2).
  - *Quelle:* audio_psx.c:238-250 (stubs cite FUN_80043d8c / FUN_80045024 bank1); re15_vab.h:105-114 (arms-record VAG layering); reai-v2-gameplay-divergence-audit (gunshot=2 VAGs)
  - *Dep:* SPU-1  ·  *Akzeptanz:* equipping the handgun primes ARMS00; firing plays the 2-VAG layered gunshot on hardware; matches PC audio_pc.c
- **[SPU-3 · S · DEPTH]** Implement re15_audio_core_se (audio_psx.c:824, CORE00 devour-SE bank4). Load CORE00.EDH/.VB to a reserved SPU region and play se_id, mirroring the other bank loaders.
  - *Quelle:* audio_psx.c:822-827 (stub cites FUN_80045024 bank4 + CORE00 table @0x801fbd00)
  - *Dep:* SPU-1  ·  *Akzeptanz:* the zombie-devour SE fires on hardware during the ROOM1140 grab-kill (reai-v2-foundation-combat), matching the PC backend
- **[SPU-4 · S · DEPTH]** Hardware-verify U8 (VAB tone ADSR). play_sample_from + ssx_note_on already source adsr1/adsr2 from the VH tone bytes +0x10/+0x12 (vab_common.c:143-144; audio_psx.c:198-199,422-423); confirm on the SPU that voices get real attack/decay/sustain/release rather than the 0x00FF/0x0000 fallback. Code-complete — this closes the loop the console never ran.
  - *Quelle:* UNTESTED_IMPLEMENTATIONS.md U8; vab_common.c:143-144; re15_vab.h:56-57 (+0x10/+0x12)
  - *Dep:* B1-3,SPU-1  ·  *Akzeptanz:* SPU_CH_ADSR register readback == the tone's adsr1/adsr2 bytes; audible envelope (no flat-full-volume click) on a keyed SE and a BGM note
- **[SPU-5 · M · DEPTH]** Wire + verify U9 (note2pitch for the SFX path). Today Se_on carries a VAG index (not a MIDI note) and footstep forces 22050 Hz (audio_psx.c:191-194,224-227). RE the Se_on→note + bank→program plumbing so SPU_CH_FREQ = re15_vab_note2pitch(note,center,shift); BGM already uses this in ssx_note_on (:411). Extend it to SE and remove the 22050 override where a real note exists.
  - *Quelle:* UNTESTED_IMPLEMENTATIONS.md U9; audio_psx.c:191-194,411; vab_common.c:235-256 + LUT (DAT_80077520)
  - *Dep:* SPU-1,SPU-4  ·  *Akzeptanz:* SEs play at the tone's true pitch (no octave error); footstep pitch matches the PSX original by spectrogram/ear
- **[SPU-6 · M · DEPTH]** Apply MIDI pitch-bend (0xE0), currently parsed-not-applied (audio_psx.c:468 `parsed, not applied (TODO)`). Scale SPU_CH_FREQ of the active voices using the tone pbmin/pbmax range (SpuVmPBVoice @0x80057ea8 / @0x80057e44). BGM SEQs that bend otherwise play flat.
  - *Quelle:* audio_psx.c:468 (TODO); re15_vab.h:54-55 (pbmin/pbmax, cites FUN @0x80057ea8/0x80057e44)
  - *Dep:* SPU-4  ·  *Akzeptanz:* a bend-using SEQ (identified via SS_BGMTBL) bends correctly on hardware vs the PC mixer / original recording

### P4 — RE2-style streaming asset layer (re15_res) — ASSET_LAYER_REWRITE Stages 2/6/7/8
*Collapse the scattered per-buffer loaders into one unified CD/file streaming manager with lazy event-driven loads, then raise the resident-enemy capacity toward RE2's 2 slots and generalize to all rooms.*

- **[RES-1 · M · DEPTH]** Introduce the re15_res unified loader skeleton (Stage 2): re15_res_load(name,dst,cap) + re15_res_stage(name,*out_size) wrapping re15_cdfs on PSX and file IO on PC (same interface). Repoint the existing CD callers (asset_psx, audio_psx ssx_load_track/voice, bg_psx, enemy loader, re15_room) at it. Pure refactor, behavior-identical, no incbin removed by this task.
  - *Quelle:* ASSET_LAYER_REWRITE.md:29-40 (§2.1 API), :85 (Stage 2); current scattered loaders (asset_psx.c, audio_psx.c:589-618, bg_psx.c, re15_cdfs.c)
  - *Dep:* B1-4  ·  *Akzeptanz:* all boot-room CD reads route through re15_res; hardware behavior byte-identical to pre-refactor (TTY load log unchanged); PC uses the identical interface file-backed
- **[RES-2 · M · DEPTH]** Move the enemy EM##.EMD load out of the RENDER loop (main.c:613-615, which loads on first DRAW) into the Sce_em_set SPAWN event (op_sce_em_set), matching RE2 (loads at spawn via FUN_80022300 from LAB_800420a0, not at draw). Add a per-port hook so the SCD spawn opcode calls re15_enemy_load. Removes the blocking ~262KB CD read from the render path.
  - *Quelle:* ASSET_LAYER_REWRITE.md:62-64 (load at Sce_em_set); re_enemy_memory_architecture (RE2 Sce_em_set → EM{type} first spawn); main.c:608-615; scd_vm op_sce_em_set
  - *Dep:* RES-1  ·  *Akzeptanz:* the enemy model is resident before its first rendered frame (no 1-frame Leon placeholder), and no blocking CD read occurs during render (perf counter g_re15_pf_rbuild stable across a spawn)
- **[RES-3 · L · BREADTH]** Raise the enemy MODEL bank to RE2's 2 resident types (Stage 6) using the RAM freed by RAM-2/RAM-3, and fix the "still-Leon" load failures via per-room VRAM budgeting in re15_vram (prime suspect: VRAM-tex exhaustion in non-1170 rooms). Today only N=1 model fits and non-room1170 rooms fail the tex alloc.
  - *Quelle:* ASSET_LAYER_REWRITE.md:88-90 (Stage 6); re_enemy_memory_architecture (RE2 = 2 slots × 0xD000, LRU DAT_800d031c); asset_psx.c:219-323 + vram_psx.c:18-24 (8 fixed slots)
  - *Dep:* RAM-2,RAM-3,RES-2  ·  *Akzeptanz:* a 2-enemy room (e.g. ROOM1140 zombies) loads BOTH distinct EM types on hardware with correct textures; TTY [enemy] logs OK, not "arena full"/"VRAM tex alloc FAILED"
- **[RES-4 · M · DEPTH]** Finalize the voice/BGM streaming facade for all STAGE1 rooms (Stage 7). The async CD-XA-equivalent voice path (voice_play/voice_poll + the voice_flush drive barrier) and SsSeq BGM (SS_BGMTBL → MAIN/SUB slot) are room1170-proven; validate the SS_BGMTBL slot loads + VOICE%02d.VAG for each STAGE1 room and the graceful missing-bank fallback (silence, not the previous room's track).
  - *Quelle:* ASSET_LAYER_REWRITE.md:91 (Stage 7); audio_psx.c:547-618 (SS_BGMTBL + ssx_load_track), :271-326 (async voice + flush barrier)
  - *Dep:* RES-1,SPU-5  ·  *Akzeptanz:* each reachable STAGE1 room plays its correct BGM (MAIN/SUB) + at least one voice line from CD on hardware; no drive-collision glitch at a cut that coincides with a voice line
- **[RES-5 · L · BREADTH]** Generalize re15_res so EVERY room's RDT/EMD/TIM/BSS/VAB flows through it with NO per-room special cases (Stage 8): remove the hardcoded room manifests (asset_psx.c:489-491 s_room_npc ELLIOT@0x1170; :547-549 s_room_rbj_enemy IRONS@0x1150) in favor of data-driven residency (RDT-declared NPC/rbj bindings).
  - *Quelle:* ASSET_LAYER_REWRITE.md:92-93 (Stage 8, globalization goal); asset_psx.c:489-565 (per-room-id special cases)
  - *Dep:* RES-1,RES-2,RES-3,RES-4  ·  *Akzeptanz:* loading any STAGE1 room on hardware pulls its full asset set purely from data (RDT slices + CD by name) with zero room-id literals remaining in the loader

### P5 — Dual-target byte-true parity (PSX == PC == original)
*Prove the two backends of the shared engine agree and both match the real hardware, so a PSX divergence is a per-port arch bug, not a gameplay drift.*

- **[PAR-1 · M · DEPTH]** Stand up a PSX↔PC parity harness: load the same room and drive the same input script on both targets and diff the resulting state (g_actors, camera cut, collision band, flags). The interpreter is shared (game_step_common.c), so any divergence isolates to a per-port file (render/input/audio/asset or the SCD-tick cadence).
  - *Quelle:* reai-v2-parity-oracle memory; re15-parity-verify skill; game_step_common.c (shared step); main.c:490-502 (identical gctx build)
  - *Dep:* B1-4  ·  *Akzeptanz:* PSX and PC produce identical logged actor/camera/collision state for the intro + a scripted walk path; every diff root-caused to a named per-port source file
- **[PAR-2 · M · DEPTH]** GTE / fixed-point render parity. The PC pixel-shift model (trig-LUT DAT_800794c4, per-product RotMatrix truncation, integer LookAt) is already byte-true to the PSX; verify the PSX GTE path (mesh_psx + skeleton_trig_psx, 20.12 fixed-point) produces the SAME projected screen coordinates so both targets match the original, not merely each other.
  - *Quelle:* reai-v2-pixel-shift memory (3 coupled LSB sources); skeleton_trig_psx.c; platform/psx/src/mesh_psx.c; CLAUDE.md (20.12 Q12, yaw 0-4095)
  - *Dep:* PAR-1  ·  *Akzeptanz:* a per-vertex projected-coord dump for a fixed test pose matches between the PSX GTE and the PC software projector within the documented GTE rounding
- **[PAR-3 · L · DEPTH]** PSX↔original savestate parity: capture DuckStation savestates of the REAL RE1.5 (re15-room-capture) for the helipad intro and a ROOM1140 zombie engage/grab, and diff RAM (g_actors, flags, enemy state, HP) against the port at matched frames — the definitive byte-true arbiter for the PSX target.
  - *Quelle:* reai-v2-parity-oracle; re15-savestate-ghidra + re15-room-capture skills; stage_saves/; reai-v2-foundation-combat (ROOM1140 loop)
  - *Dep:* PAR-1,RES-3  ·  *Akzeptanz:* the port's RAM state matches the original savestate at matched frames for BOTH the helipad intro and a ROOM1140 zombie wake→engage→grab, divergences either fixed or documented with the exact bytes

## AREA: INFRASTRUCTURE, TOOLING, TEST + VERIFICATION METHODOLOGY

This area builds the scaffolding that keeps the port byte-true as BREADTH work (new rooms/stages/enemies) lands. Today the verification power is real but manual and gated: the Java reference extractor is unbuildable (root gradle files missing), the RE tools rot outside CMake (RE15_BUILD_TOOLS/TESTS default OFF), there is no CI, the PSX target has never been built here, and — most importantly — the parity harness has no automated per-room state-DIFF: the PC port emits ad-hoc stderr (PL(x,z,rot,hp) + per-enemy ss1/grid/mo/af/dist + pst/ps1/ps2) and the savestate readers (re15_enemy_state.py/ppos.py/re15_ss.py) emit human text, while parity_net only aligns frames by image content. The plan is ordered so each layer unblocks the next: (I0) unblock and reproducibly build EVERYTHING incl. the never-built PSX ELF; (I1) put it under a regression gate with CI, golden-extraction tests, and an auto coverage scorecard; (I2) build the core deliverable — a canonical machine-readable per-frame state schema emitted by BOTH sides plus the automated state-DIFF comparator (parity_net becomes its timing-alignment front-end, defeating the DuckStation variable-fps trap); (I3) scale that to a room-parity corpus with a per-room checklist and PSX golden savestates over the 240 RDTs; (I4) generalize the savestate-provoke repacker into a per-enemy harness + checklist + golden corpus so the 50 enemy types that don't spawn in loadable rooms are still regression-verifiable, auto-emitting unit tests; (I5) consolidate docs + wire the living UNTESTED/deferred registers and a no-unsourced-constant lint into CI; (I6) close the PSX gap by booting the port's OWN ISO headless in DuckStation and proving PC↔PSX cross-target state parity. DEPTH tasks enforce/prove byte-trueness of existing systems; BREADTH tasks are net-new capability that enables the breadth roadmap.


### I0 — Unblock the toolchain (reproducibly build EVERYTHING, incl. the never-built PSX ELF)
*Every artifact the byte-true methodology depends on (Java reference extractor, RE tools, PC tests, PSX ELF) builds from one documented, pinned environment with a single command — nothing rots behind a default-OFF flag.*

- **[I0-1 · S · BREADTH]** Restore the Java asset-extractor Gradle build: recover root build.gradle + settings.gradle (java+application+shadow plugin, mainClass de.re15.Main), wire the 7-phase RE15MasterExtractor entrypoint, confirm ./gradlew build|run|test works. The extractor is the byte-true REFERENCE pipeline (TIM/PIX/ESP finalized) the port is validated against.
  - *Quelle:* .claude/worktrees/agent-a795e02c/build.gradle + settings.gradle (rootProject.name='re1.5-ai-test'); gradlew wrapper + gradle/ present at root; de.re15.Main -> de.re15.extractors.RE15MasterExtractor (CLAUDE.md Java section)
  - *Dep:* none  ·  *Akzeptanz:* ./gradlew build && ./gradlew test green from a clean checkout; ./gradlew run executes RE15MasterExtractor phase 1; shadow jar produced.
- **[I0-2 · S · BREADTH]** Bring the RE tools into the CMake build so they stop rotting: add tools/overlay_mapper/CMakeLists.txt (targets overlay_mapper + generate_mapping), extend tools/CMakeLists.txt (currently only add_subdirectory(room_graph)) to include it and the standalone python tools (gen_trig_lut.py, vram_png_to_tim.py) as documented custom targets; flip RE15_BUILD_TOOLS default ON for the PC preset and fix any stale-API breakage.
  - *Quelle:* re15_port/tools/overlay_mapper/{overlay_mapper.c,generate_mapping.c} (no CMakeLists); re15_port/tools/CMakeLists.txt (only room_graph); re15_port/CMakeLists.txt:99-103 RE15_BUILD_TOOLS option; re15-room-probe SKILL gotcha 'tools/ default OFF -> verrottet'
  - *Dep:* none  ·  *Akzeptanz:* cmake -DRE15_BUILD_TOOLS=ON configures; ninja builds overlay_mapper + room_graph + extract_doors without errors; overlay_mapper emits its per-stage JSON jump-table map from STAGE{1..6}_overlay.c.
- **[I0-3 · M · BREADTH]** Pin + document a reproducible toolchain and a Python harness venv: capture exact mingw64 GCC/Ninja/CMake versions and a requirements.txt for the parity scripts (vgamepad, zstandard, torch, numpy, pillow) so every verification script runs on a fresh machine. Add a bootstrap script that creates the venv and checks ViGEmBus/DuckStation/Ghidra paths.
  - *Quelle:* CLAUDE.md Build section (mingw64 GCC 15.2 @C:\msys64\mingw64\bin, Ninja, CMake>=3.21); harness imports across .claude/skills/*/scripts/*.py (vgamepad, zstandard, torch); re15-savestate-ghidra analyzeHeadless path
  - *Dep:* none  ·  *Akzeptanz:* python -m pip install -r requirements.txt in a fresh venv imports all harness scripts; a bootstrap 'doctor' command reports OK for gcc/ninja/cmake/python/vgamepad/DuckStation/Ghidra.
- **[I0-4 · S · BREADTH]** Create a single build+test wrapper (Make/PS + bash) that: kills a stale re15_pc.exe lock, configures PC with RE15_BUILD_PC=ON + RE15_BUILD_TESTS=ON, builds via Ninja, runs ctest --timeout 30, and prints the pass count. Encodes the known exe-lock and default-OFF-tests gotchas so no session re-learns them.
  - *Quelle:* re15_port/CMakeLists.txt:100,109-111 (RE15_BUILD_TESTS OFF, enable_testing gated on PC+TESTS); re15-room-probe SKILL ('taskkill //F //IM re15_pc.exe before build; GLOB picks up new tests'); reai-v2-build-recipe memory (26/26 -> now 79/79)
  - *Dep:* none  ·  *Akzeptanz:* one command from clean tree yields '79/79 tests passed' (or current N/N); re-running after leaving re15_pc.exe alive still builds (lock auto-cleared).
- **[I0-5 · M · BREADTH]** Establish the PSX cross-build target end-to-end for the FIRST time here: configure with cmake/psx_toolchain.cmake + FindPSn00bSDK.cmake + PSN00BSDK_PATH, build re15_reborn.elf, and report _end via mipsel-none-elf-size. This is the prerequisite for ALL PSX-side verification (I6) and enforces the 2 MB RAM budget invariant continuously instead of after-the-fact.
  - *Quelle:* re15_port/cmake/psx_toolchain.cmake + FindPSn00bSDK.cmake; ASSET_LAYER_REWRITE.md §4 (mipsel-none-elf-size, _end < 0x80200000, >=64KB stack margin, no heap); CLAUDE.md PSX target flags
  - *Dep:* I0-3  ·  *Akzeptanz:* cmake -DRE15_BUILD_PSX=ON produces re15_reborn.elf; mipsel-none-elf-size reports _end < 0x80200000 with the documented stack margin; ISO builds (mkpsxiso) if PSn00bSDK present.

### I1 — Regression gate: CI, golden-extraction tests, coverage scorecard
*Nothing byte-true silently regresses: every commit builds PC(debug+release)+PSX, runs the full ctest suite, byte-compares reference extractions against goldens, and reports an auto coverage scorecard of RE'd-vs-implemented FUN addresses.*

- **[I1-1 · M · BREADTH]** Stand up CI (GitHub Actions self-hosted or a scripted local CI runner given the Windows/mingw/DuckStation constraints): matrix = {PC Debug, PC Release, PSX ELF}; steps = configure, build, ctest, mipsel-none-elf-size _end check. Fail the build on any ctest failure or _end overflow.
  - *Quelle:* re15_port/CMakeLists.txt (RE15_BUILD_PC/PSX/TESTS options); ASSET_LAYER_REWRITE.md §4 invariant '_end<0x80200000'; absence of any .github/workflows at repo root (confirmed)
  - *Dep:* I0-4,I0-5  ·  *Akzeptanz:* a CI run on a PR shows 3 green targets + '79/79 ctest' + a printed _end value; a deliberately-failing test turns the run red.
- **[I1-2 · S · BREADTH]** Add a pre-commit / build-verify hook that runs the I0-4 wrapper (kill exe lock -> build -> ctest) and blocks commits that break the suite; document it so it is opt-in per the repo's commit-only-when-asked rule.
  - *Quelle:* reai-v2-build-recipe memory; re15-room-probe SKILL exe-lock gotcha; project git discipline in CLAUDE.md
  - *Dep:* I0-4  ·  *Akzeptanz:* committing with a failing test is rejected with the failing test name; committing with green suite proceeds.
- **[I1-3 · M · DEPTH]** Golden-output regression for the reference extractor + core parsers: check in byte-hashes (or small golden blobs) for a representative RDT parse, an EMD split, a TIM->BMP, and an ESP->EFF, then assert the Java extractor AND the C parsers reproduce them. Locks the finalized converters (TIM/PIX/ESP) against silent drift.
  - *Quelle:* CLAUDE.md 'Fertige Konverter (nicht ändern) TIM->BMP, PIX->TIM->BMP, ESP->EFF'; asset-pipeline skill (parser classes); re15_port parsers rdt_common.c/emd_common.c/tim_common.c/re15_esp.c
  - *Dep:* I0-1,I0-4  ·  *Akzeptanz:* a ctest + a gradle test each hash-match the checked-in golden; mutating a converter byte flips the test red.
- **[I1-4 · M · BREADTH]** Auto coverage scorecard: a script that parses RE15_FUN_CATALOG.md (addresses+subsystem+confidence) and RE15_FUN_COVERAGE.md, cross-refs which FUN_/DAT_ are cited in engine/src comments, and emits a %-by-subsystem table (verified/partial/unknown). Run it in CI so the 'what's left' number is always current.
  - *Quelle:* RE15_FUN_CATALOG.md (curated address catalog); RE15_FUN_COVERAGE.md (known/partial/unknown scorecard); ghidra-mapping skill
  - *Dep:* I0-4  ·  *Akzeptanz:* scorecard regenerates deterministically in CI; adding a new FUN citation in code bumps the matching subsystem's implemented count.

### I2 — The parity state-log spine (canonical schema + automated state-DIFF)
*Replace manual eyeballing with a machine comparator: define ONE per-frame state schema emitted by both the PC port and the savestate readers, then a diff tool that aligns two timelines and reports the exact first-divergence frame+field — the automated oracle every new room/enemy is checked against.*

- **[I2-1 · M · DEPTH]** Design + document a canonical machine-readable per-frame state schema (JSON-lines, versioned): frame index, player block (x,z,rot,hp,state pst/ps1/ps2,motion), per-actor block (slot,type,x,z,rot,ss1/ss2,grid,ai_flags,dist,hp,hitbox), and room globals (camera cut, fade level, display flags, active room). Include declared per-field comparison tolerances (exact for state words/flags; timing-independent for positions).
  - *Quelle:* main.c RE15_STATE_LOG format 'PL(x,z,rot,hp)'+per-enemy 'ss1/grid/mo/af/dist'+'pst/ps1/ps2' (Commit 1cbcc4f5); re15_enemy_state.py field map (enemy array 0x800acc2c stride 0x1f4, pos +0x34/+0x3c, dist +0x1d0, hp +0x9a; player 0x800aca54/88/90, cut DAT_800b532e, fade DAT_800b5568, flags DAT_800aca3c)
  - *Dep:* none  ·  *Akzeptanz:* schema doc + a JSON-schema validator; a sample port frame and a sample savestate frame both validate against it.
- **[I2-2 · M · DEPTH]** Port side: extend RE15_STATE_LOG to emit the I2-1 JSON schema (behind RE15_STATE_LOG_JSON=1 to keep the human text), one object per game tick, deterministic at 30 fps under RE15_INPUT_SCRIPT.
  - *Quelle:* re15_port/platform/pc/main.c RE15_STATE_LOG + RE15_INPUT_SCRIPT (deterministic 30fps, 0.5s=15f) per re15-parity-verify SKILL §2
  - *Dep:* I2-1  ·  *Akzeptanz:* RE15_START_ROOM=1140 RE15_INPUT_SCRIPT='R0.15,U2.5' RE15_STATE_LOG_JSON=1 yields byte-identical JSON across two runs (determinism) and validates against the schema.
- **[I2-3 · M · DEPTH]** PSX side: a savestate->schema exporter that wraps the existing readers (re15_ss.py Ram class + re15_enemy_state.py + ppos.py) to emit one schema frame per savestate, and a multi-savestate 'timeline' builder (orig_timeline.py) that stitches a sequence of waypoint saves into a schema JSON-lines stream.
  - *Quelle:* .claude/skills/re15-savestate-ghidra/scripts/{re15_ss.py,re15_enemy_state.py} + re15-parity-verify/scripts/{ppos.py,orig_timeline.py}; RAM-base signature method (PSX.EXE[0x800:] search, base=pos-0x10000)
  - *Dep:* I2-1  ·  *Akzeptanz:* running the exporter on stage_saves/mzd_stage1_briefing.sav produces a schema-valid frame whose 5-zombie types/positions/dist match re15_enemy_state.py's human output.
- **[I2-4 · L · DEPTH]** Build the automated per-room state-DIFF comparator: input two schema timelines (port + PSX), align them (frame index or via I2-5), compare each field with I2-1 tolerances, and emit a report of first-divergence (frame, slot, field, port-val vs psx-val). This is the missing automated oracle the parity-oracle memory describes doing by hand.
  - *Quelle:* reai-v2-parity-oracle memory ('gleicher Raum+Bewegung MUSS 100% identisch; Divergenz=Port-Bug'); re15-parity-verify SKILL §3 (timing-independent invariants: spawn, dz/dx direction, wall x=-5118, state words)
  - *Dep:* I2-2,I2-3  ·  *Akzeptanz:* on a known-good ROOM1140 run it reports zero divergences on state words/spawn; injecting the old turn-in-place-48 bug (vs byte-true 96) makes it flag the rot field at the first pivot frame.
- **[I2-5 · M · DEPTH]** Wire parity_net as the timing-alignment front-end for I2-4: use its content-based frame matches (PSX capture <-> port series) to produce the frame correspondence that defeats the DuckStation variable-fps trap, then hand aligned pairs to the state diff. Keep the discipline note that cos-score is triage, not byte-true proof.
  - *Quelle:* .claude/skills/re15-parity-verify/parity_net/parity_net.py (SupCon matcher, match --queries --gallery -> out/*.csv); parity_net/README (verdicts MATCH>=0.70 / CHECK-DIVERGENCE); re15-parity-verify SKILL §4 (variable-fps pitfall)
  - *Dep:* I2-4  ·  *Akzeptanz:* a variable-fps PSX capture + a 30fps port series produce a monotone frame map (--align) that lets I2-4 diff aligned frames; frames with no counterpart are surfaced as CHECK-DIVERGENCE not false state-diffs.

### I3 — Room parity corpus + per-room byte-true checklist (scale verification to the 240 RDTs)
*Turn the single-room ROOM1140 methodology into a repeatable, batched corpus so every one of ~240 rooms can be spawn-verified in CI and, where a golden save exists, full-timeline-diffed — the safety net for the BREADTH room work.*

- **[I3-1 · L · DEPTH]** Generate a per-room manifest DB (JSON, one entry per RDT): stage, expected entry spawn+yaw, camera-cut count, door/AOT count, enemy roster (from Sce_em_set), and a curated golden tank-path. Seed spawn/roster from the RDT+SCD parse, then verify against port boot + (where available) a savestate.
  - *Quelle:* rdt-analysis skill (RDT sections: header/cut/collision/camera/SCD/models); scd-disassembly skill (doors/Sce_em_set); 240 ROOM*.RDT under re15_port/shared_assets/PSX (STAGE1=80); re15-room-probe spawn method
  - *Dep:* I0-2  ·  *Akzeptanz:* manifest lists all 240 rooms with non-empty stage/cut-count/door-count; ROOM1140 entry lists the 5-zombie roster (0x16/0x10/0x10/0x11/0x11) and spawn (-7600,-17600,rot -96) matching the known-good values.
- **[I3-2 · M · DEPTH]** Batch room-probe ctest matrix: for every RDT, load it, run main00+sub00, tick N frames, assert no crash + snapshot the actor spawn set, comparing against the I3-1 manifest golden. Isolates per-room load/SCD/spawn regressions deterministically without DuckStation.
  - *Quelle:* re15-room-probe SKILL (test_room1140_spawn.c template + scd_room_reenter minimal sequence); re15_port/tests/unit/test_room1140_spawn.c + tests/unit/CMakeLists.txt asset-path injection
  - *Dep:* I3-1  ·  *Akzeptanz:* one ctest iterates all rooms; all currently-loadable rooms pass spawn-snapshot; a room whose spawn set drifts from the manifest fails with the offending slot printed.
- **[I3-3 · M · DEPTH]** Codify the per-room byte-true verification CHECKLIST (spawn pos+yaw, camera-cut count + RVD zone install/anchor rule, door/AOT count + dest_room, SCD thread set, collision walls, RDT light tint, BG cut) as a doc AND a partially-automated runner that fills each item from the port + savestate readers, flagging manual-only items.
  - *Quelle:* re15-parity-verify SKILL §7 checklist; RE15_KNOWLEDGE.md §1.1 RDT / §1.2 SCD / §1.3 BSS; RE15_FUN_COVERAGE.md RVD anchor rule (FUN_80014230/80014324 first-per-cam_from-group skip); reai-v2-gameplay-divergence-audit (RVD auto-scan, dest_room=0)
  - *Dep:* I3-2  ·  *Akzeptanz:* running the checklist runner on ROOM1140 auto-fills spawn/cuts/doors/zones/lighting from data and marks the byte-true-confirmed items; a checklist template exists for a fresh room.
- **[I3-4 · L · DEPTH]** Build the PSX golden-savestate corpus: use re15_quickload.py (HEX JUMP --left/--right) to capture a clean-load savestate per reachable room, export each to a schema frame (I2-3), and index them in the manifest. This is the reference set the room-parity job diffs against.
  - *Quelle:* .claude/skills/re15-room-capture/scripts/re15_quickload.py (HEX JUMP; ROOM1140=0x114 --left 16); re15_capture.py black-frame guard (--postload, cross-stage>=16s); stage_saves/ existing captures
  - *Dep:* I2-3,I3-1  ·  *Akzeptanz:* a growing stage_saves/corpus/ with >=1 verified (non-black, gameplay-state) save per captured room; each exports a schema frame whose spawn matches the port's I3-2 snapshot.
- **[I3-5 · M · DEPTH]** Room-parity regression job: for each room that has both a golden save (I3-4) and a golden path (I3-1), run the port deterministically, capture the JSON timeline, and diff via I2-4 (parity_net-aligned where a capture series exists). Report per-room pass/fail into the coverage scorecard.
  - *Quelle:* reai-v2-parity-oracle (same room+path must be 100% identical); re15-parity-verify SKILL §4 (drive the PORT deterministically for reliable combat/anim; verified paths R0.15,U2.5)
  - *Dep:* I2-4,I3-4  ·  *Akzeptanz:* the job runs green for ROOM1140 (0 state-word divergences through the grab-death sequence) and produces a per-room parity column in the scorecard.

### I4 — Per-enemy verification + generalized savestate-provoke pipeline (the 50 types)
*Make every enemy type regression-verifiable even when it never spawns in a loadable room, by turning the one-off provoke/repack scripts into a per-enemy harness with a checklist, a golden state corpus, and auto-generated unit tests.*

- **[I4-1 · L · DEPTH]** Generalize the savestate-provoke pipeline into a per-enemy harness: parameterize re15_ss_patch.py (RAM patch+repack) + re15_provoke.py to force-spawn ANY of the 50 types into a base save and drive it into each AI state (idle/wake/engage/attack/hurt/death), including types with no loadable room. Handle the enemy-pos +0x34/+0x3c and skip-tick +0x9&0x20 caveats.
  - *Quelle:* reai-v2-savestate-repacker memory (re15_ss_patch.py; enemy-pos +0x34/+0x3c; player-pos 0x800aca88 is CACHE; skip-tick +0x9&0x20 clear); re15-savestate-ghidra scripts/{re15_provoke.py,re15_flag_sweep.py}; reai-v2-enemy-roster-complete (all 50 types)
  - *Dep:* I2-3  ·  *Akzeptanz:* a command force-spawns e.g. a type that has no clean JUMP room, provokes it to attack, and re15_enemy_state.py confirms the expected +0x4/+0x5/+0x9 state; the produced save exports a schema frame.
- **[I4-2 · M · DEPTH]** Author the per-enemy byte-true CHECKLIST (spawn hitbox radius/height, wake distance gate, per-state attack constant + damage value, death->corpse transition, model EM##.EMD load, root-motion) as a template filled from the roster AI docs, so each new enemy is verified against the same rubric.
  - *Quelle:* RE15_MAGGOT_AI.md/RE15_DOG_AI.md/RE15_CROW_AI.md/RE15_SPIDER_AI.md etc.; re15-enemy-ai-re skill (playbook); re15_enemy_state.py FSM field map (+0x4 main/+0x5 anim-phase/+0x9 sub/hitbox *(ent+0x78)); reai-v2-parity-oracle enemy gates (wake<3000@0x801048bc, grab<1200@0x80102128, attack<2000@0x801020c8)
  - *Dep:* I4-1  ·  *Akzeptanz:* a filled checklist for zombie/maggot/spider/dog with every constant carrying a disasm address; a blank template exists for the next type.
- **[I4-3 · L · DEPTH]** Capture a per-(type,state) golden savestate corpus via I4-1 and add a port-side enemy-probe ctest matrix (driving re15_enemy_ai_run_all in an isolated actor as in test_room1140_combat.c) that asserts the port reproduces each captured state's byte-true fields; enforce the per-scenario actor-isolation gotcha.
  - *Quelle:* re15-room-probe SKILL (test_room1140_combat.c 7-part template + state-contamination isolation recipe grid_id=0x86/x=z=30000); reai-v2-foundation-combat
  - *Dep:* I4-1,I4-2  ·  *Akzeptanz:* a golden save exists per active (type,state) reached; the enemy-probe ctest matrix passes for the covered types and flags a field mismatch when a constant is perturbed.
- **[I4-4 · M · DEPTH]** Build a unit-test generator: from a captured enemy savestate (via the I2-3 exporter) emit a compilable tests/unit/test_<enemy>_ai.c that asserts the byte-true fields (hitbox dims, state words, damage), so newly-RE'd enemies get a locked-in regression test with minimal hand-coding.
  - *Quelle:* re15_port/tests/unit/test_*_ai.c (existing per-enemy tests: dog/crow/maggot/spider/birkin/ivy/alligator...); re15_enemy_state.py field extraction
  - *Dep:* I4-3  ·  *Akzeptanz:* pointing the generator at a captured save produces a test that compiles, runs under ctest, and passes against the current port; regenerating from the same save is deterministic.

### I5 — Docs consolidation + living-register + no-unsourced-constant lint
*Keep the byte-true knowledge base navigable and self-policing: consolidate the scattered MD into docs/, wire the living UNTESTED/deferred registers to the new corpora, and lint the engine so no behavior constant lacks a cited address.*

- **[I5-1 · M · BREADTH]** Consolidate the knowledge base into a navigable docs/ set (CLAUDE.md notes docs/ was dropped): index RE15_KNOWLEDGE.md (formats), RE15_FUN_CATALOG/COVERAGE, per-subsystem AI/audit MDs, and the skill methodology into one cross-linked structure; render an Artifact overview for the human to browse the plan/coverage.
  - *Quelle:* CLAUDE.md ('das alte DOCUMENTATION.md/docs/ wurde nicht übernommen'); RE15_KNOWLEDGE.md §1.1-1.19; RE15_FUN_CATALOG.md/RE15_FUN_COVERAGE.md
  - *Dep:* I1-4  ·  *Akzeptanz:* docs/ index links every top-level MD; the coverage scorecard (I1-4) is embedded/linked; no dead cross-references.
- **[I5-2 · S · DEPTH]** Wire the living registers to the corpora: when a room (I3) or enemy (I4) scenario now exercises a code path listed in UNTESTED_IMPLEMENTATIONS.md or reai-v2-deferred-backlog, the parity job flags it as 'now runtime-exercisable' so the register can be updated instead of drifting stale (the deferred-backlog audit found ~81/219 markers were lying comments).
  - *Quelle:* UNTESTED_IMPLEMENTATIONS.md (U6-U9 open register); reai-v2-deferred-backlog memory (43 OPEN of 219 markers; ~40 STALE)
  - *Dep:* I3-5,I4-3  ·  *Akzeptanz:* a report lists which UNTESTED/deferred entries are now covered by a corpus scenario; running it on U-entries whose room is now in the corpus flags them for promotion.
- **[I5-3 · M · DEPTH]** Add a 'no unsourced constant' lint over engine/src: flag any behavior/format magic number (clip index, offset, threshold, distance, flag) in a changed line that lacks a nearby FUN_/DAT_/RDT/byte-offset citation, per the hard RE-Disziplin rule. Run in CI on the diff to keep new code byte-true-sourced.
  - *Quelle:* CLAUDE.md RE-Disziplin ('Jede verhaltens-/format-relevante Konstante MUSS eine Disassembly-Adresse oder einen Datei-Byte-Offset zitieren'); existing engine citation style (e.g. enemy_ai_common.c @0x801048bc comments)
  - *Dep:* I1-1  ·  *Akzeptanz:* the lint passes on current cited code; adding an uncited threshold constant to a source line makes CI flag it with the line and value.

### I6 — Close the PSX gap: boot the port's OWN ISO headless + PC↔PSX cross-target parity
*Extend the parity oracle to the PSX target the port has never been run on: boot the port's own ISO in DuckStation headless, assert via TTY/state, and prove the same engine yields the same state-log on PC and PSX for a scripted room.*

- **[I6-1 · M · BREADTH]** Produce the port's PSX ISO in CI and archive it as a build artifact, with the _end/RAM-budget gate from I0-5 enforced per build (RE2-model streaming budget from ASSET_LAYER_REWRITE feeds this).
  - *Quelle:* re15_port/cmake/psx_toolchain.cmake; ASSET_LAYER_REWRITE.md §4 (per-stage build all-3 + _end report; no heap); mkpsxiso in PSn00bSDK
  - *Dep:* I0-5,I1-1  ·  *Akzeptanz:* CI attaches re15_reborn.iso as an artifact and prints the _end value under the 0x80200000 limit for every PSX build.
- **[I6-2 · L · BREADTH]** Automate booting the port's OWN ISO in DuckStation headless with TTY logging on, and assert boot health from the log ([enemy]/[boot]/[rdt] printfs, no exceptions) — reusing the re15-room-capture DuckStation driver but pointed at the port ISO instead of the MZD original.
  - *Quelle:* re15-room-capture SKILL (DuckStation -batch, settings TTYLogging=true + LogToFile, duckstation.log read); ASSET_LAYER_REWRITE.md §4 diagnosis loop ([enemy] TTY diagnostics)
  - *Dep:* I6-1  ·  *Akzeptanz:* a scripted run boots the port ISO to a room and the harness asserts the expected [rdt]/[enemy] TTY lines with no CPU exception; a broken asset load is caught from the log.
- **[I6-3 · L · DEPTH]** Prove PC↔PSX cross-target state parity: run the SAME scripted room+input on the PC build (JSON state-log) and the PSX-ISO build (TTY-emitted schema frames), and diff via I2-4. Same engine on two backends must produce the same state-log — the cross-target byte-true guarantee.
  - *Quelle:* ASSET_LAYER_REWRITE.md §4 ('PC is the parity reference, PSX is CD-backed, same interface'); reai-v2-parity-oracle (100% identical behavior); I2-1 schema shared by both targets
  - *Dep:* I6-2,I2-4  ·  *Akzeptanz:* a scripted ROOM1140 run yields state-log JSON from PC and from the PSX ISO that diff to zero on state words/spawn via I2-4; a backend-specific divergence is surfaced with frame+field.