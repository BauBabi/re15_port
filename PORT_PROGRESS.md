# RE1.5 Port Progress — Honest, Evidence-Based Assessment

**Scope:** the `re15_port/` C engine (PC via SDL2 + PSX via PSn00bSDK) measured against the **full original scope** of the RE1.5 (RE2 prototype) game. Percentages are *done-vs-full-original-scope*, calibrated by independent code inspection — **not** marketing. A wired faithful-line stand-in is deliberately **not** counted as 100%; deferred byte-true parts are flagged everywhere.

Generated from 8 cluster investigations + adversarial calibration passes (each cited port file was re-opened and cross-checked against RE source / ghidra / knowledge docs).

---

## 1. OVERALL

### Weighted completion: **≈ 45 %**

The overall figure is the weight-weighted average of the eight calibrated clusters:

| Cluster | Calibrated % | Weight | % × weight |
|---|---:|---:|---:|
| Rendering & VRAM | 63 | 9 | 567 |
| Animation, Player Control & Camera | 58 | 8 | 464 |
| Enemy AI, Combat & Damage | 44 | 8 | 352 |
| Audio | 56 | 6 | 336 |
| Script VM, Rooms & Game Flow | 64 | 9 | 576 |
| Inventory, Items & Menus | 12 | 6 | 72 |
| System, UI/Text & Persistence | 16 | 6 | 96 |
| System-level Infrastructure | 23 | 6 | 138 |
| **Σ** | | **58** | **2601** |

**Weighted average = 2601 / 58 = 44.84 % ≈ 45 %.**

### State of the port (one paragraph)

The port is a **byte-true-oriented, in-room gameplay slice** — not yet a whole game. Its *engine core* is genuinely strong and, on the PSX target, hardware-accurate: the SCD script VM, the RDT room-container parser, the AOT trigger + room-transition machine, the skeletal-animation pose math, the GTE/math foundation, the MDEC/BSS background decoder, TIM/CLUT handling, per-vertex NCCT lighting, and the two-sided ROOM1140 combat loop are all present, wired, and covered by 34 passing ctests, with cited ghidra/STAGE1.BIN/PSX.EXE addresses. Where the port is thin is exactly the *shell around* the game: there is **no title/boot flow, no memory-card save/load, no FMV playback, no options screen, no game-over screen, no fade-between-rooms, and essentially no inventory/menu/HUD UI**. Even inside gameplay, large original *mechanisms* are deferred stand-ins rather than byte-true ports — the player command FSM is collapsed to ad-hoc branches, the per-frame camera animator is written but not wired (framing is static-per-cut), enemy forward-walk root motion is an explicit no-op (zombies fight standing still), and the weapon cone is a radial-from-player approximation. **Two framings matter (see §5): room-scoped, the targeted ROOM1140/1170/1240 content is far more complete than 45 %; whole-game, the front-end + back-end + non-targeted rooms pull the honest average down to ~45 %.**

---

## 2. PER-CLUSTER TABLE

| Cluster | Calibrated % | Weight | One-line status |
|---|---:|---:|---|
| **Script VM, Rooms & Game Flow** | **64 %** | 9 | Byte-true VM core + RDT + AOT + transition machine; but no game-shell (title/save/game-over) and fade is cinematic-intro-only. |
| **Rendering & VRAM** | **63 %** | 9 | Real hardware-GTE/OT/MDEC pipeline on PSX + faithful SDL re-creation on PC; ESP sprite draw is a PC-only stand-in, PSX has none. |
| **Animation, Player Control & Camera** | **58 %** | 8 | Skeleton/stairs/dest-walker/view-matrix byte-true; command-FSM collapsed to if/else, camera animator written-but-unwired (static framing). |
| **Audio** | **56 %** | 6 | Real BGM synth + VAB/ADPCM SE on PC; Se_on plays a bundled test VAB, PSX SE path is a no-op stub, voice/rotor are stand-ins. |
| **Enemy AI, Combat & Damage** | **44 %** | 8 | Strongest gameplay area: end-to-end ROOM1140 two-sided combat green — but one enemy, one room, standing still; presentation deferred. |
| **System-level Infrastructure** | **23 %** | 6 | Excellent math/scheduler/CD foundation, but no boot flow / title / FMV / save / options / vibration. |
| **System, UI/Text & Persistence** | **16 %** | 6 | Byte-true text/typewriter engine is real; the entire UI shell (title→menus→save→game-over→options→HUD) is absent. |
| **Inventory, Items & Menus** | **12 %** | 6 | Minimal 11-slot backend + byte-true weapon-equip commit; the whole inventory/status/map/file UI is unimplemented. |

---

## 3. PER-AREA DETAIL

### 3.1 Script VM, Rooms & Game Flow — **64 %** (weight 9)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| SCD VM — control flow (0x00–0x1A) | ported | 92 | `scd_vm.c` `scd_vm_tick` 4-return-code model (CONTINUE/YIELD/FRAME_RET/IF_FALSE), byte-true block-stack + loop-counter, all If/Else/Switch/For/While/Do handlers (FUN_8003f0a0 + @0x800744a8 table). | `op_goto` unwind empirical (not fully disasm-proven); Break's +0x4 flag byte omitted; PSX thread-pool range (24→14) deferred (#24). |
| SCD VM — data/state opcodes | ported | 85 | Flags Ck/Set (MSB-first per-zone FUN_8004ef90/efe4), work-vars (DAT_800b0fd0), Pos/Dir/Member (direct id table), Speed/Add_speed; cross-check fixed 6 real bugs. | 0x47/0x50/0x53 are RE2-misattributions the port deliberately does **not** run byte-true (honestly flagged); 0x2d fabricates collision; 0x38 sets no SCA AABB; Member calc 0x55/0x56 size-only. |
| SCD VM — subsystem/effect/audio opcodes | partial | **58** | Real ports for Se_on/Xa_on/Message_on/Cut_chg/Cut_auto/Aot_set/Door/Item_aot/Obj_model/Sce_em_set; **0x3A Sce_espr_on is fully wired end-to-end** (spawn→byte-true FUN_80019e20 timer→projected/textured PC draw @30Hz). `s_opcode_sizes[256]` gives every op byte-true advance (no desync). | ~15 other effect-sprite/ESP/entity-spawn ops are byte-true-**disassembled but size-only stubs** (`{pc+=N;return 1;}`): 0x4C kill, 0x36/0x40/0x41 draw, 0x5C shake, 0x5A weapon_chg, 0x5B/0x5D/0x5E. Sce_em_set HP nominal. |
| RDT room container load | ported | 90 | `rdt_common.c` parses all 12 sections (header counters, RID/cut, RVD zones, main/sub SCD tables, SCA, LIGHT, MESSAGE, FLOOR, RBJ, props, audio banks); 240 rooms load through it; 3 parse ctests. | Per-room RVD record layout verified for ROOM1170 only; `main_scd_size` a debug heuristic; MDEC BG VRAM dest hardcoded (a BG gap, not RDT). |
| AOT triggers (install + per-frame scan/fire) | ported | 80 | `aot_common.c` single-action-per-press latch (FUN_80042bac), door forward-reach 563u±900, band-gate, cam_from filter, point-in-quad (FUN_80014368), cut_auto gate; RVD→AOT install; 3 tests. | Only RECT shapes (no CIRCLE — no RE1.5 user found); canonical SCD-Se_on not wired to AOT event; some faithful-line reconstructions (power-gate redirect) approximate prototype behaviour. |
| Room-to-room transitions | ported | 78 | `room_common.c` 15-step apply-pending (load RDT, reset render/props, actor+enemy reset, cinematic, cut table, player spawn, camera, BG stream, lights/messages, band, BGM); byte-true dest-id formula; 2 tests. | **No DO2 door-open animation / door-transition screen** (format has a roundtrip test but no runtime door-swing); **no fade** between rooms — transitions are an instant teleport + cut. |
| Cinematic / cutscene overlays | partial | 70 | `scd_room_setup.c` same-room re-entry (work_vars[10]); `re15_to_re2.c` Plc_dest clip/speed resolver (cites FUN_80041BE4 per-mode tables); letterbox+player-mode cinematic FSM; RBJ parsed+loaded; cut-freeze montage (0x29/0x2a). | Neck damped-spring FSM approximated; walker 3-state partial; cut-timing residual drift (#6); only ROOM1170 is a voiced/fully-tuned cinematic, others structural. |
| Top-level game-state machine (@0x8001c958) | partial | 30 | `game_step_common.c` = the STATE-1 (gameplay) per-frame step with inline menu/stair/dead/grabbed/normal branches; inline pause; death FSM core. | **NO title, new-game, save/load, or game-over shell**; the DAT_800b5359 states 1–5 dispatcher is not reproduced — the port hard-boots into gameplay. **Biggest game-flow gap.** |
| Fade / letterbox / screen transitions | stub | 20 | Cinematic letterbox bars (24px byte-true) driven by `letterbox_countdown`. | Unified fade FSM (FUN_80021a0c ±0x10/frame, DAT_800b5568) **not integrated**; no fade on room-transition/death/menu — every transition is an instant cut. (Note: a one-shot cinematic-intro fade-in *is* wired on both targets — see §3.7.) |

### 3.2 Rendering & VRAM — **63 %** (weight 9)

> **Structural fact:** two backends. `platform/psx/` is a **real** hardware pipeline (GTE `gte_rtpt/nclip/avsz/ncct`, double-buffered ordering table, POLY_GT3/GT4, hardware MDEC, POLY_F4 fade). `platform/pc/` is an **SDL2 re-creation** (software float projection, `SDL_RenderGeometry` painter's z-sort, software IDCT/NCCT). Only the PSX target is byte-true for mesh/OT/fade; PC is faithful-line.

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| MDEC/BSS video backgrounds | ported | 85 | `bss_vlc.c` full VLC tables + DC predictors; `bss_mdec.c` AAN Loeffler 2D IDCT + BT.601 YUV→RGB (cited truncated Q12 consts); PC RGBA cache blit + PSX hardware DecDCT path. (Cr/Cb-swap bias since dropped — standard MDEC layout.) | VRAM dest hardcoded (not read from RDT cam +8/+10); no-rounding descale color bias; no full-pipeline ctest (only chunk-parse). |
| 3D model mesh rendering | partial | 60 | PSX byte-true (`mesh_psx.c` gte_ldv3→rtpt→nclip→avsz→OT bucket→POLY_GT3/GT4); PC faithful (PROJECT_VERT float, backface cross-product, near-clip, per-tri z-sort). Shared MD1/EMD/EDD/EMR parsers. | PC uses float not GTE; **no depth buffer** (painter's per-tri avg z-sort loses per-pixel OTZ); affine texturing only; bone-0 root + RotMatrix barrel-roll open; no projection/backface/sort ctest. |
| Per-vertex lighting (NCCT) | ported | 80 | `light_common.c` 3-stage integer re-derivation of gte_ncct 0x0118043F (LLM.V, BK, CODE·IR>>12), all-integer isqrt; PSX hardware ncct; wired per mesh vertex. | No-normal flat-128 fallback is canonical-but-dead; falloff constants RE'd from RE2 arch ref (not RE1.5 GTE capture); no shade-vs-reference ctest. |
| Effect sprites (ESP: hit/blood/muzzle) | partial | **42** | Byte-true **parser + pool + AABB-cull dispatch + FX particle timer**; wired to spawn blood on damage; `op_sce_espr_on` spawns, `op_sce_espr_control` a real predicate; `test_esp_parse.c` against real ROOM1140.RDT. | **On-screen draw is faithful-line PC-only** (fixed 24px UV cell, ~world-600 billboard, pivots deferred; exact coord-cell mapping TBD via live capture); **PSX has NO ESP render**; **muzzle-flash INFRA-BLOCKED** (FUN_80045024 unported). |
| TIM / texture + CLUT | ported | 90 | `tim_common.c` all bpp (4/8/16/24) + CLUT block; PC 24-slot pool, multi-CLUT stacked via per-tri v-offset, rgb555→argb8888; PSX hardware LoadImage/OpenTIM; TEX.TIM font page. | 24bpp path lightly exercised (assets are 4/8/16); per-tri CLUT emulated via UV offset not true VRAM indexing; index0-opaque is a documented heuristic; no standalone ctest. |
| Priority / z-sort / masking (sprite.pri) | partial | **57** | `pri_common.c` byte-true section parse (0xFFFFFFFF terminator, high-nibble×8 dims, signed truncating dst add); PC **overdraw is wired** (mask patches interleaved with tri batches in one camera-Z order — the "disabled" comments are stale); PSX OT bucket native resolve. | PC is a painter's approximation of the PSX OT (no per-pixel OTZ); exact pri.depth field + insert order partial; region-quad cull replaces per-primitive occlusion. |
| Display list / double-buffer / fade / letterbox | ported | 75 | PSX byte-true dual-OT double buffer + VSync(2) 30Hz + subtractive POLY_F4 fade (0xF0, setRGB0(8,16,48)) + 24px letterbox; PC framebuffer + SDL alpha fade + letterbox + subtractive shadow blob. | Unified fade_level PolyF4 PSX-port integration pending; PC fade is SDL alpha (not unified with PSX subtractive quad); no game-over/death-camera fade; PC path is not a real OT. |

### 3.3 Animation, Player Control & Camera — **58 %** (weight 8)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| Skeletal animation (bone pose math) | ported | 80 | `skeleton_common.c` YXZ Euler RotMatrix, GTE-faithful Q12 single-shift accumulate, parent BFS chain, 12-bit signed angle unpacker @bone×36 (FUN_8001f664), FRAC crossfade + root lerp, Plc_neck head-look bone 8. | **Bone-0 root decode KNOWN-WRONG/Tier-4** (+ live file contradiction: emd_common says kf translation zeroed, skeleton_common reads+applies it); neck damped-spring FSM only referenced not decoded; per-bone yaw/pitch clamp omitted. |
| Player control (tank + command FSM + aim/fire) | partial | **52** | `player_common.c` byte-true WALK75/RUN200/BACK60 + turn rates; move along visual facing; **idle-fidget FSM byte-exact port** of switchD_8003206c with HP-gated injured clips; aim RAISE clip 17→READY 18. | **Top-level command FSM (FUN_80031c44 → PTR_LAB_80073f90, 8 modes + 3 sub-tables) NOT structurally ported** — collapsed to ad-hoc if/else; aim-elevation pitch (+0x6a), muzzle, fire discharge sub-state 5, weapon-inventory selection all deferred; back-walk = forward clip reversed (real clip un-extracted). |
| Actor locomotion / root motion (dest-seek walker) | partial | 68 | `actor_locomotion.c` byte-true 3-state walker (ATAN256 heading FUN_8001a6d4, slew 0x15e/0x30/0x48, arrival radii 100/300, mode-9 turn-in-place, completion flag); keyframe-speed readers present. | **`re15_actor_apply_root_motion` is an explicit NO-OP** (lines 355–362); **enemy forward-walk root motion (FUN_8001AD68/AE38, +0x5=5/6) deferred** (readers exist but unwired); m0 +0x5=6 trigger an unresolved variant puzzle; no pathfinding. |
| Camera (view build + cut-select + per-frame animator) | partial | **52** | `camera_common.c` byte-true RE2 FUN_80076cb0 view-matrix (forward-normalize, V=Rx·Ry, t=−M·pos, fov>>7); RVD cut-select + region-quad cull wired. | **Per-frame camera animator (soft-track + orbit/dolly + H-clamp) written but ZERO callers** (grep-verified; main.c "REVERTED all animator hacks") — framing is **static-per-cut**, the every-frame soft-track is inert; RVD layout verified ROOM1170 only. |
| Stairs (ascend/descend, FK foot-lock) | ported | 82 | `stair_common.c` byte-exact descend/ascend (clip cursor, FK foot-lock FUN_800390e0 body-dragged-by-support-foot bones 4/7, tread bob, +0x708 per-band Y, band loop, finalize); byte-true entry (800u probe + band gate + type-12/13 AOT pair). | Finalize forward-eject is a deterministic **reconstruction** (runs resolver on the stationary finalize tick, frame-order differs slightly); foot-lock byte-confirmed for ROOM1170 stairs only. |

### 3.4 Enemy AI, Combat & Damage — **44 %** (weight 8)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| STAGE1 zombie AI dispatch + FSM | ported | **68** | `enemy_ai_common.c` byte-true main-state dispatch @0x8011f7b4 [0]init/[1]active/[2]hurt/[3]death → CORPSE state 7; wired via `re15_enemy_ai_run_all` (type-gated 0x10/0x11/0x16); all ctests pass. | Sub-mode table @0x8011f80c [1–4],[9–15] deferred (only 0/5/6/7/8 handled); [4] idle deferred; hurt/death anim **durations gate nothing** (faithful-line); one live family out of ~130 overlay fns. |
| Enemy locomotion / movement AI | partial | **40** | Dist-gated wake (@0x801018f8, disasm+savestate-verified), engage-decide (FUN_80102058), turn-to-face (FUN_80102dc8 rot_y += arc_test); arc_test/dist byte-true. | **Forward-walk / approach is a literal empty function** — zombies engage/turn/grab **in place**, never stride toward the player; `re15_emd_get_keyframe_speed` exists but unwired; no pathing. |
| Zombie attack: grab/bite (+ dormant lunge) | ported | 68 | `re15_enemy_ai_live_grab` (FUN_80102548 9-sub-step: IMPACT −10, BITE −5, exit→engage; grab clips, release clip 17, SEs 4/7); lunge windup + begin/tick present. | Grab bite loop is faithful-line one −5/cycle; grab clips set but no grab **animation** plays; **lunge-arm DORMANT byte-true** (DAT_800aca3c&1 never set, savestate-proven) stubbed; attack-point a faithful-line bone pick. |
| Hitbox geometry + attack resolver | partial | 60 | `re15_damage.c` FUN_8002b5d0 circular path byte-true (instruction-mapped); FUN_80012d60 resolver loop; per-type hitbox structs (0x10/0x11/0x16=400/1440 savestate; player=450/1530 EXE). | **Angular-SECTOR hitbox path deferred** (routed through circular); resolver GATE B (terminal-state skip) omitted/inert; structs for only 4–5 types; enemy attack-point faithful-line. |
| Player weapon fire + damage/crit tables | partial | **52** | `re15_player_weapon_fire` (FUN_80011f50 core: auto-aim nearest front zombie, per-weapon dmg/reach rows byte-dumped from PSX.EXE, crit w7/w8<3000→hp=−1, HURT/DEATH); wired R1+Square; test parts 11/12/19. | Cone is a **radial-from-player + arc_test(0x400) stand-in**, not the byte-true aim-point/camera line-of-fire; knife (FUN_80012574) deferred; non-zombie dmg rows unported; hit-direction clip, aim/fire command FSM @0x80035810, inventory, muzzle deferred. |
| Player hit-reaction, grabbed-lock & death FSM | partial | 55 | `re15_player_take_damage` (guard, hp−=table, bleed roll, hurt/death states), `is_grabbed`/`is_dead`/`death_tick` (0x78=120 timer); wired death+grabbed branches skip player_tick (pin like stair branch); test parts 8/9. | Grabbed-lock is a faithful-line re-derived latch (not the original cmd-5 latch + LAB_80036834 pose/XZ-pin + struggle-escape); **death sequence is timer-core only** — colour fade, death camera, "eaten" anim FSM, **GAME-OVER screen all deferred** (no fade/game-over infra); bleed RNG non-reproducible stand-in. |
| Enemy model load (EMS + EMD/skel/anim/TIM) | ported | 85 | `re15_ems.c` byte-true EMS index; `enemy_common.c` registry + RBJ rebind; `pc_ensure_enemy` real load from CDEMD0.EMS → md1/skel/anim/TIM + texture upload; `test_ems` passes. | Loaded enemy **animation only partly consumed** by AI (clips selected but durations don't gate transitions; enemy root motion not applied); PSX side hand-inlined (parity risk); per-equip reload boot/cross-room only. |

### 3.5 Audio — **56 %** (weight 6)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| BGM — SsSeq synth (MAIN/SUB, ADSR, note2pitch, reverb, room table) | ported | 80 | `audio_pc.c` full software synth: real SPU ADSR decode of adsr1/adsr2, byte-extracted `s_pitch_lut`, byte-exact STUDIO_B reverb (PSX.EXE 0x80077274), byte-true SS_BGMTBL[106]/SS_STAGE_OFF[6]; PSX mirror drives hardware SPU voices. | Not the actual libsnd driver — an RE2-shaped standard-MIDI synth reproducing the audible result; voice-alloc/priority approximated; only STAGE1/ROOM1170 exercised. |
| SE/SFX — VAB/ADPCM + footsteps + room SE + weapon ARMS + combat SEs | partial | **57** | `vab_common.c` byte-true VAB parse + VAG ADPCM decode + footstep EDT→prog/tone→VAG; PC footstep/room/weapon SE loaders; combat SEs wired (grab4/release7/death5-8, gunshot idx8); damage-side now fires equipped weapon (default 1) — earlier "weapon 2" bug closed. | **Se_on plays a BUNDLED TEST VAB** (test.vh/test.vb; PSX=DOOR00) not the room snd banks; bank0 resident + bank4 CORE never loaded; per-tone vol/pan faithful-line (fixed vol=100); per-weapon reload deferred; **PSX room_se/weapon_se/prime_weapon are pure no-op stubs.** |
| XA VOICE — dialogue stream | partial | **42** | PC RE2-shaped facade + PSX real async CD VAG streaming (VOICE##.VAG → reserved SPU ch). | Largely a **stand-in for INVENTED content** — AI-TTS "synchro" WAVs (RE1.5 shipped no English XA), keyed off Message_on not Xa_on, ROOM1170-hardcoded, mono; op_xa_on a counter no-op; PSX file-id↔message mapping unverified. |
| Rotor ambience (helicopter positional SE) | partial | **48** | `rotor_common.c` byte-true pan/distance/azimuth math (nested SquareRoot0, ATAN256 LUT, SE_PAN_ATTEN[256], &0x7f mask), wired to seq_ctl/rotor_update. | **Rotor VAG source UNCONFIRMED and OFF BY DEFAULT** on PC (silent unless RE15_ROTOR_WAV set); PSX runs SUB_15 BGM which may not be the real bank; heli-EM keyoff faithful-line. Math-complete, content-unproven. |

### 3.6 System-level Infrastructure — **23 %** (weight 6)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| GTE / fixed-point math library | ported | **82** | Real hardware GTE on PSX (gte_ldv3/rtpt/rtps/nclip/avsz/ncct/stsxy3, InitGeom); matched float RNDI projection on PC; shared Q12 trig + 12-bit angle unpacker + ATAN256 atan2. | Bone-0 root-translation keyframe decode KNOWN-WRONG (Tier-4); angular-sector hitbox needs BIOS ratan2 the port lacks (only catan). |
| Task scheduler / frame pacing | ported | 70 | Shared `re15_game_step` ordered update; PC SDL frame budget (30fps, RE15_FPS), PSX VSync(2) 30Hz; SCD cooperative thread-pool scheduler (the game's real task system) ported. | Frame-count gates tuned for 60fps and rescaled at 30 via FRAME_AT_60() rather than frame-rate-independent; no OS-level scheduler modeled (not needed). |
| Localization / text-encoding + font | partial | **65** | Byte-true dialog FSM (typewriter FUN_80028134) + renderer (FUN_80028868 glyph emission + pen-width); TEX.TIM 4bpp font; `.msg` subtitle text; `byte = ASCII − 0x24` mapping. | Single-language only; full in-game examine/message-text integration partial (subtitle path is the wired one). |
| CD streaming / on-demand asset loader | partial | 55 | PSX real streaming (CdSearchFile + CdRead + separated MDEC src/dest staging, voice-flush arbitration); shared room loader; EMS index + per-room slicing; PC plain fopen/fread. | **No 256-entry file-id table** (loads by ISO name not CD_read(index)); no 6-state CD FSM; no async double-buffered background streaming (synchronous). Adequate for room-scoped target, not the full engine loader. |
| Heap / memory manager | partial | 50 | PSX fixed-region staging (0x42000 decode + 0x19000 CD-src, kept separate; bump VRAM allocators reset on room enter); PC host malloc/free. | No general allocator/InitHeap; byte-exact RAM map (save buffer address, PATCH region, per-room offsets) not reproduced. Architecturally faithful for a fixed-region engine, not byte-true placement. |
| Controller input / analog / vibration | partial | 50 | Digital pad works (keyboard→PSX bits with edge detection; PSX reads real pad; shared layout). | **No vibration/rumble anywhere** (PadSetAct absent); **no analog stick**; no button-remap/config; PC keyboard only (no SDL gamepad). |
| Debug tooling / DEBUG.BIN dev menu | partial | 30 | Port ships its own dev tooling (RE15_START_ROOM boot-any-room, [/] room browser, Tab motion-lock + clip stepping, --headless JSON, autoshot, RE15_FPS, debug HUD). | Original in-game DEBUG.BIN menu (stage/room JUMP selector, utility menu, native CHECK panel) **not ported** — deliberate non-goal; own PC affordances substituted. |
| Boot sequence + top-level game-flow | stub | 8 | Init order (render/input/cdfs/assets/mesh/bg/audio/scd_vm) then the loop; death path exists (0x78 timer). | **No game-flow enum/dispatcher** — boots straight to hardcoded room; no Capcom logo, title, NEW/CONTINUE, or game-over→title loop; death dead-ends (no game-over screen). |
| Memory-card save / load | stub | 5 | Save-relevant state exists byte-true in RAM (g_game.flags, HP, inventory). | **ZERO serialization** — no memory-card I/O, save format, save FSM, typewriter/ink-ribbon, item-box persistence, or continue (RE2 save FSM cataloged "future rebuild"). |
| Title screen / attract / demo (TITLE.BIN) | none | 0 | — | TITLE.BIN never loaded; no menu/attract state. |
| FMV / STR movie playback (Movie_on 0x6F) | none | 0 | CAPCOM.STR shipped in assets. | Opcode 0x6F is a bare length-table no-op (RE2-import block); no STR demuxer, no streaming MDEC video, no XA sync — opening movie can't play. |
| Options / Config screen | none | 0 | — | No options/config UI, no settings persistence, no button remap. |

### 3.7 System, UI/Text & Persistence — **16 %** (weight 6)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| Fonts / in-game text (message boxes, .msg font, subtitles, YES/NO) | partial | **58** | `msg_common.c` byte-true `.msg` parser + glyph→ASCII + typewriter FSM (FUN_80028134 states 0–6, 0x02/0x04/0x05 codes, fast-forward) + YES/NO choice; `render_pc.c` real TEX.TIM 4bpp atlas (per-glyph advance from RAM 0x800c4416, per-attr CLUT). | Only cinematic subtitles + ROOM1140 examine/choice wired; **item-description/inventory text not driven**; framed message-BOX chrome absent; PSX truncates `.msg` to 32. |
| Input (input_pc.c + pad) | partial | 60 | Keyboard→PSX pad bits with pressed/released edges (PSn00bSDK layout); consumed by shared FSM, aim/fire, dialog, menu; PSX reads real pad. | Hardcoded binds (no remap); no analog; no vibration; no SDL gamepad; L2/R2 repurposed for dev. |
| Fades / screen transitions | partial | **30** | One-shot **cinematic-intro fade-in wired on BOTH targets** (PC alpha ramp 0→0xF0 @+0x10/frame; PSX POLY_F4 via re15_render_set_fade), gated to first cut; byte-true 24px letterbox. | Not the unified FUN_80021a0c/DAT_800b5568 fade FSM; **no door/room-transition/death fade**; covers exactly one transition. |
| Inventory / weapon-select menu | partial | **28** | `menu_common.c` byte-true EQUIP commit order (set DAT_800aca5d @0x80046688 then re-prime ARMS @0x800466c4); START-edge open + inline pause; debug-text render + '>' cursor; `test_weapon_select`. | Owned list is a hardcoded 3-entry TEST list (real FUN_800c00a8 derivation in un-RE'd DEBUG.BIN); no 2-col grid nav, no ITEMALL icons, no examine/combine/use; debug-text not the grid UI. |
| GAME-OVER screen | stub | 12 | Input-freeze + byte-true 120-frame death timer (grabbed-death state 7 @0x8003694c). | Game-over **presentation** — colour fade, death camera, eaten-anim FSM, GAME OVER graphic, continue/retry — all deferred (blocked on missing fade/UI infra). |
| DEBUG / JUMP menu | stub | 8 | PC dev [/] room browser + 'I' inventory toggle. | No FUN_8001443c byte-true stage/room JUMP selector, layout, categories, or debug-flag toggles. |
| HUD (health ECG, ammo, item) | stub | 8 | HP tracked (=100, drives injured-idle + death); pickup subtitle line. | **No ECG/heartbeat, no health colour states, no ammo counter, no status ring** — HP is a value driving animation only, not a HUD. |
| BOOT / TITLE screen | none | 0 | — | Boots directly into a room; no title/menu/press-start; TMOJI.TIM not loaded. |
| SAVE / LOAD (memory card, typewriter, libcard) | none | 0 | — | No memory-card, save format, typewriter save-room, ink-ribbon, item-box, or load. RE2 save FSM cataloged only. |
| PAUSE screen | none | 0 | — | No pause/status screen (weapon menu pauses via game_step but there is no map/file/full-status view). |
| Config / options screen | none | 0 | — | No controller remap, sound/brightness/vibration settings, or screen-position adjust. |

### 3.8 Inventory, Items & Menus — **12 %** (weight 6)

| Area | Status | % | What's done | What's open |
|---|---|---:|---|---|
| Inventory backend (g_inv 11 slots, grant/stack) | partial | **40** | `inventory_common.c` byte-true 4-byte {id,qty,flags,pad}×11 layout; grant = pass-1 stack same-id + pass-2 first-free slot; wired from AOT pickup. | **No decrement/remove/use** despite the name/header advertising it (only `re15_inv_init` + `re15_inv_grant` exist); no compaction (FUN_8004dc4c unported), no stack-limit, no size/flags handling, no drop; grant is self-described demo-grade; never populated from a save. |
| Weapon-select menu (Phase 1: open/equip) | partial | 55 | `menu_common.c` byte-true open (raw START edge) + EQUIP commit order (set-weapon then re-prime SE bank); playable + unit-tested (open/toggle/nav-clamp/equip/cancel). | Owned list a fixed {1,2,3} test list (real FUN_800c00a8 in un-RE'd DEBUG.BIN); clamped nav not 2-col parity grid; no same-item guard, no 0x80 sentinel; debug-text not grid+icons. |
| Item pickup (AOT ITEM grant) | partial | 50 | `aot_common.c` grant + one-shot deactivate; `op_item_aot_set` byte-true rect parse (LE, PC-advance 30/22 [AUDIT #5]). | +18 pickup_action / +20 flags ignored (no examine-prompt / event branch); pickup SFX intentionally removed; grant ignores classification — auto-grant items only. |
| Full inventory / status screen (grid, icons, CHECK/USE/COMBINE/EXAMINE) | none | 3 | — (only debug-text weapon list + reference comments). | Entirely unimplemented: no grid render, cursor FSM, ITEMALL icon atlas, sub-action logic, item-box, or quantity digits (all Phase 2 = deferred). |
| Item definitions / attribute table (id→weapon/ammo/key) | none | 0 | — (grant treats id as opaque type/amount). | Not started **and not reverse-engineered** — the source table lives in DEBUG.BIN @0x800C0000 (all-?? in the EXE dump); needs a DEBUG.BIN disasm/savestate dump. Blocks owned-weapon derivation, classification, and combine/use rules. |
| Status / health display (HUD) | none | 5 | HP tracked internally (drives injured-idle + death). | Zero visual health/status — no EKG waveform, FINE/CAUTION/DANGER indicator, or HUD element. |
| MAP screen | none | 0 | — | Not started (no renderer, data loader, or menu tab). |
| FILE / document screen | none | 0 | — | Not started (no document collection, text-reader UI, or menu tab). |

---

## 4. WHAT'S MOST DONE vs WHAT'S MOST OPEN

### Strengths (most done, roughly in order)

1. **SCD script VM control-flow + data/flag/work-var core (92 % / 85 %)** — genuinely byte-true dispatcher, block-stack/loop model, all structured opcodes; a cross-check fixed 6 real bugs. The backbone that runs every room.
2. **RDT room-container parser (90 %)** — all 12 sections, 240 rooms load through it.
3. **TIM/texture + CLUT (90 %)** — all bpp + multi-CLUT, PSX hardware + PC pool.
4. **Enemy model load from CDEMD0.EMS (85 %)** and **stairs traversal + FK foot-lock (82 %)** — byte-exact and wired.
5. **GTE/math foundation (82 %)** — real hardware GTE on PSX, matched float on PC.
6. **Skeletal-animation pose math (80 %)**, **NCCT lighting (80 %)**, **BGM synth (80 %)**, **AOT triggers (80 %)** — solid, cited, wired.
7. **MDEC/BSS backgrounds (85 %)** and the **PSX double-buffered OT + POLY_F4 fade + 24px letterbox (75 %)**.
8. **The two-sided ROOM1140 combat loop** — end-to-end and green (34 ctests): spawn→wake→engage→turn→GRAB(−10/−5)→player pinned→HP<0→death, plus R1+Square fire→auto-aim→byte-true per-weapon damage→zombie HURT/DEATH/CORPSE. This is the port's showcase.

### Biggest gaps (most open, prioritized by weight × missing)

1. **No top-level game shell (game-flow 30 %, boot 8 %, title 0 %, save/load 0/5 %, game-over 12 %, options 0 %)** — the port hard-boots into a gameplay room; there is no title, new-game/continue, memory-card save, game-over screen, or options. This is the single largest whole-game gap and spans two of the highest-weight-adjacent clusters. The port is a *room runner*, not a full game.
2. **Fade / screen transitions are cinematic-intro-only (20–30 %)** — no fade between rooms/on death/on menu; every transition is an instant cut. This one missing primitive blocks a faithful death sequence, menu open, and door transition.
3. **The entire inventory/menu/HUD UI (cluster 12 %)** — no item grid, icons, sub-actions, map, file, or on-screen health/ammo; item-definition table not even reverse-engineered (locked in DEBUG.BIN).
4. **Enemy forward-walk / approach locomotion is an explicit NO-OP** — zombies fight standing still; no pathing. Central to a zombie's spatial behaviour and entirely absent.
5. **Combat presentation is deferred** — anim durations gate no state transitions, the weapon cone is a radial stand-in (not aim-point/camera line-of-fire), the aim/fire command FSM + muzzle-flash + death camera are all deferred; combat is byte-true for *one enemy, one room, standing still*.
6. **FMV / STR movie playback (0 %)** — CAPCOM.STR shipped, opcode 0x6F is a no-op, no demuxer.
7. **DO2 door-open animations (0 %)** — transitions are instant teleports, no door-swing cutscene.
8. **ESP effect-sprite draw + muzzle-flash (42 %)** — parse/pool/dispatch byte-true, but on-screen draw is a PC-only faithful-line billboard, PSX has none, muzzle is infra-blocked.
9. **Command FSM + per-frame camera animator not structurally ported** — player control is ad-hoc if/else; camera framing is static-per-cut (animator written but unwired).
10. **PSX-side audio SE/weapon path is a no-op stub** and **Se_on plays a bundled test VAB** — scripted room SFX are placeholder-sourced.

---

## 5. How to read this

**This is a byte-true reverse-engineering effort deliberately focused on specific rooms** — the ROOM1140 STAGE1 briefing combat and the ROOM1170/1240 cinematics — **not** a breadth-first whole-game port. That makes two framings diverge, and both are true:

- **Room-scoped completeness (the target content):** for ROOM1140/1170/1240 the implemented *mechanisms* are far more complete and much closer to byte-true than 45 % — the combat loop, cinematic playback, background decode, animation, lighting, and script VM for those rooms are wired and tested. If you only measured the targeted rooms, the number would be substantially higher.
- **Whole-game completeness (the number in §1):** measured against the *full* original game — every room, every enemy, the front-end (logo/title/movie/menus), the back-end (save/continue/game-over), options, HUD, and all deferred byte-true sub-mechanisms — the honest weighted average is **≈ 45 %**. The gap between the two framings is exactly the shell around the game plus the non-targeted rooms and enemies.

Every percentage here is *done-vs-full-original-scope* and was calibrated by re-opening the cited port files and cross-checking against RE source / ghidra / knowledge docs; wired faithful-line stand-ins are **not** counted as complete, and each one's deferred byte-true part is named. Where the source assessments were optimistic (e.g. "ESP opcodes are stubs" when 0x3A actually spawns and draws; "overdraw disabled" when the render-side interleave is wired; "fade not integrated" when a one-shot cinematic fade *is* wired) the calibration corrected them — mostly in the pessimistic direction, so a few percents nudged up. Where they were generous (rotor silent-by-default, voice binding invented, camera animator unwired, inventory decrement never written) the calibration trimmed them down. **Bottom line: an excellent byte-true engine core and a proven single-room combat slice, wrapped in almost no game shell — an in-room gameplay demo at ~45 % of the whole game.**
