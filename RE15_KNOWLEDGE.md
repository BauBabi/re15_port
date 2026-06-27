# RE1.5 Knowledge Overview

Single reference for "what we know about Resident Evil 1.5 (the unreleased RE2 PSX prototype)"
and where in source that knowledge is recorded. Status tags:

- CONFIRMED — works in code / tested / thoroughly RE'd
- PARTIAL — some parts known, others not
- UNKNOWN — open question / failed attempt / unsolved

All paths are repo-relative under `C:/workspace/git/reAi_v2/` (this repo). Note:
some provenance/Evidence paths below (e.g. `memory/…`, `scripts/…`) point at the
original `reAi` working repo and were **not** carried into reAi_v2 — treat those
as historical citations, not files present here.

---

## 1. File formats

### 1.1 RDT (Room Definition Table) — CONFIRMED
Per-room container; central asset hub. Header + 0x60-byte address table at file start.

| File offset | Field | Notes |
|-------------|-------|-------|
| 0x00 | nSprite, nCut, nOmodel, nItem, nDoor, nRoom_at, Reverb_lv, Reserved (8B) | counts |
| 0x08-0x1C | Sound banks 0+1: EDT, VH, VB offsets (6 x u32) | voice/SFX |
| 0x20 | collisionStart | |
| 0x24 | cameraStart | |
| 0x28 | zoneStart | |
| 0x2C | lightStart | |
| 0x30 | md1PointerStart | |
| 0x34 | floorStart | |
| 0x38 | blockStart | |
| 0x3C | messageStart | |
| 0x40 | mainScdStart | main room script |
| 0x44 | subScdStart | sub scripts |
| 0x48 | extraScdStart | extra scripts |
| 0x4C | effectStart | |
| 0x54 | espTimStart | |
| 0x58 | modelTimStart | |
| 0x5C | animationStart | |

Source: `src/main/java/de/re15/extractors/RDTExtractor.java:37-120`,
sub-parsers `MSGParser.java`, `BLKParser.java`, `RIDParser.java`,
`CollisionParser.java`, `FloorParser.java`, `LightParser.java`,
`SpritePriParser.java`, `ZoneRvdParser.java`, `EffectEspParser.java`,
`CameraParser.java`. Reference: `memory/scd_extension_guide.md:44-68`.

### 1.2 SCD (script bytecode) — CONFIRMED for ~90 opcodes; some 0x80-0x8E unknown
Each SCD section = `[pointer_table | bytecode]`. Pointer table = N x u16 offsets
(first entry = end-of-table marker / sub00 start). Opcodes are 1 byte + fixed
params (a few variable-size: `Obj_model_set` 0x2D = 38B base, `Aot_set` 0x2C = 20B,
`Door_aot_set_4p` 0x68 = 40B, etc.).

Full opcode table with sizes/descriptions: `src/main/java/de/re15/extractors/SCDScriptDisassembler.java:191-333`.
See "SCD scripting" section below for high-level summary.

### 1.3 BSS (MDEC video backgrounds) — CONFIRMED
Per-room file, 64KB chunks (one per camera angle). Each chunk = MDEC VLC
bitstream, 320x240. VLC ID 0x3800 in bytes 2-3.

- Parser: `src/main/java/de/re15/extractors/bss/BssExtractor.java`
- VLC decoder: `src/main/java/de/re15/extractors/bss/VlcDecoder.java`
- MDEC + IDCT + YCbCr->RGB: `src/main/java/de/re15/extractors/bss/MdecDecoder.java`
- BSS files contain VIDEO ONLY (no audio): `memory/re15_bss_streaming_reuse.md`

### 1.4 EMD / EDD / EMR / MD1 — CONFIRMED
Composite enemy/player model split into 4 components by PldExtractor/EMS extractor:

| Component | Purpose | Parser |
|-----------|---------|--------|
| EDD | Animation timing/clip metadata | `extractors/emd/AnimationParser.java` |
| EMR | Skeleton (bone hierarchy + 12-bit packed angles) | `extractors/emd/SkeletonParser.java`, `BitReader.java` |
| MD1 | Mesh (triangles + quads, 14 u32 header) | `extractors/md1/MD1File.java:163-253` |
| TIM | Texture | `extractors/md1/TimFile.java`, `extractors/emd/TimParser.java` |

- Bone rotations: 12-bit per axis (4096 = 360 degrees), see `BitReader.java:12-28`,
  `SkeletonParser.java:121-136`.
- Quaternion order: ZYX Euler. PSX->glTF Y-flip in `EmdMath.psxTripletToGltf`.
- glTF export: `extractors/emd/EmdGltfExtractor.java`.
- PLD/PLW extraction: `extractors/pld/PldExtractor.java:57-92` writes EDD/EMR/MD1/TIM.

### 1.5 PLD / PLW / EMS — CONFIRMED
- PLD = player model, PLW = player weapon, EMS = enemy model archive (containing EMDs).
- All are containers: split into EDD/EMR/MD1/TIM components.
- PldExtractor at `extractors/pld/PldExtractor.java:18-50`.
- EMS archive ordering and 2048B alignment in `RE15MasterExtractor.java:41-49`.

### 1.6 TIM / PIX / ITP — CONFIRMED (TIM/PIX), PARTIAL (ITP)
- TIM: PSX texture format, 4/8/16/24-bit with optional CLUT. Magic 0x10. Parser:
  `extractors/md1/TimFile.java`, converter: `extractors/TimToBmpConverter.java`.
- PIX: container of TIM-like sprites. Item-PIX vs Map-PIX distinct paths
  (`Step2PIXToTIMConverter.java`, `Step2bItemPIXToTIMConverter.java`,
  `Step3CorrectItemPIXToTIMConverter.java`).
- ITP: item-icon container. Parser: `extractors/ItpParser.java`.

### 1.7 ESP (effect sprites) — CONFIRMED
- 8-byte effect-ID header + EFF pointers. 0xFF = unused slot, 0xFFFFFFFF = unused ptr.
- Parser: `extractors/EffectEspParser.java:28-80`.
- ESP-to-EFF conversion finalized; do not modify per `CLAUDE.md:67-72`.

### 1.8 VAB (VH+VB) — CONFIRMED
PSX standard VAB audio bank. VH header magic `pBAV` (`SIGNATURE 0x56414270`).

| Field | Size | Notes |
|-------|------|-------|
| Header | 32B | magic, version, vab_id, total_size, ps, ts, vs, mvol, mpan |
| ProgAtr[128] | 128 x 16B = 2048B | Programs |
| ToneAtr[16] per program | 32B per tone, 16 tones max | Tones reference VAGs |
| vagOffsets[256] | 256 x u16 (size in 8B units) | sample sizes |
| VB body | (separate file) | concatenated VAGs, no per-sample header |

Parser: `src/main/java/de/re15/extractors/audio/VabFile.java`.
Converter: `extractors/audio/VabToWavConverter.java`.
PSX ADPCM decode: `extractors/audio/PsxAdpcmDecoder.java` (5 filter coefficients,
shift = 12 - (header & 0xF), flags bit2 = reset state).

### 1.9 EDH / VB (Capcom variant) — CONFIRMED
Capcom's wrapper around standard VAB. See `memory/edh_vb_format.md`:
- Variant A: 64B Capcom prefix + Sony VabHdr at 0x40 + 8B trailer (3176B total).
- Variant B: 16B prefix + VabHdr at 0x10 + 8B trailer (3128B total).
- Trailer first byte = prefix size (0x40 or 0x10).
- Capcom VabHdr.reserved0 = 0xEEEE (Sony spec says 0).
- Per-program tone count u8 max 16 (PSX VAB limit). Observed CORE00..CORE0F = 12 tones,
  ARMS00..ARMS14 = 5-8 tones.
- Extension tooling: `scripts/extend_vab.py` (parse + extend).

### 1.10 VAG — CONFIRMED
Raw 16-byte ADPCM blocks: `[shift(4)|filter(4) | flags | 14 bytes nibbles]`.
Flags: 0x01=end, 0x03=end+loop, 0x06=loop_start+loop, 0x07=end+loop+loop_start.
Filter coeffs (pos/neg over 64): (0,0)/(60,0)/(115,-52)/(98,-55)/(122,-60).
SPU decode: `out = (nib<<shift) + (K0*p1+K1*p2)>>6`, clamp i16.
Encoder: `scripts/encode_vag.py`. Decoder: `extractors/audio/PsxAdpcmDecoder.java`.

### 1.11 MSG (text) — CONFIRMED
Custom encoding `byte = ASCII - 0x24` for alphanumerics. See full table in
`memory/native_text_encoding.md:6-69`. Parser `src/main/java/de/re15/extractors/MSGParser.java`
(NOTE: hiragana/katakana table in MSGParser.java is wrong for RE1.5 English text;
0x57+ is symbols, not Japanese — see `memory/native_text_encoding.md:161-167`).
Termination: `[text...] 03 02 01`. Y/N: `[text...] 1B 03 02 F9 xx 01`.

### 1.12 BSS / SCD / RDT extraction phases — CONFIRMED
7-phase pipeline driven by `RE15MasterExtractor.extractAll()`. See
`src/main/java/de/re15/extractors/RE15MasterExtractor.java:100-200`.

### 1.13 DO2 (door archives) — CONFIRMED
Container: MD1 + TIM + 8 SCDs + VAB pair (VH/VB). Parser:
`src/main/java/de/re15/extractors/DO2Extractor.java:24-100`.

### 1.14 STR (movie) — UNKNOWN (only known via SCD opcode 0x6F `Movie_on`; format not parsed in our Java code)

### 1.15 CAPCOM.STR — PARTIAL
Located on CD at LBA 6497, ~6.1MB. Contains interleaved video Form1 + audio Form2
(stereo 37800Hz 4-bit). Subheaders: video = `00 01 0x48 0x00`, audio = `01 01 0x64 0x01`.
Used for intro movie. Source: `memory/agent_xa_capcom_compare.md:1-40`.

### 1.16 ITPS (CHECK item images) — CONFIRMED
ITPS images loaded from CD via `0x800C6878`. File index 42 (884KB). Used by CHECK
panel. See `memory/itps_loading_system.md`.

### 1.17 ADPCM — CONFIRMED (see VAG)

### 1.18 SEQ (BGM sequencer) — PARTIAL
47 `MAIN*.BGM` files in `info/Re1.5/PSX/SOUND/`. Format = SEQ + VAB (per
`memory/agent_re15_audio_paths.md:21-25`). Java SEQ-to-MIDI converter exists at
`src/main/java/de/re15/extractors/audio/SeqToMidiConverter.java` (not deeply
verified in memory).

### 1.19 BGM (per-room) — UNKNOWN structure beyond "SEQ+VAB"

---

## 2. Memory map (PSX RAM)

### 2.1 EXE & static segment — CONFIRMED
- PSX_LOAD_ADDR = 0x80010000 (`scripts/patch_save_final.py:66`)
- PSX header size = 0x800
- EXE main code: 0x80010000 - ~0x800B0000

### 2.2 Code caves (zero-filled regions in EXE) — CONFIRMED
| Cave | Address | Size | Use | Persistence |
|------|---------|------|-----|-------------|
| 0 | 0x80070340 | 236B | 56B BOX_LOAD, 180B free | Persistent EXE region |
| 2 | 0x800719D4 | 768B | 508B used, ~260B free | EXE RAM (safe) |
| 3 | 0x8007153C | 736B | 156B PATCH_LOADER + 332B TEXT_DISMISS_V2, 248B free | EXE RAM |
| 4 | 0x80072BEC | 257B | NOT PERSISTENT — runtime overwritten | DO NOT USE |

Source: `memory/MEMORY.md:71-76`, `memory/project_patch_bin.md:38-46`.

### 2.3 PATCH.BIN region — CONFIRMED
- Loaded into 0x801F0000 from CD (sector 73388, 2 sectors = 4096B).
- 0x801F0000 - 0x801F54FF = 21.7KB safe (no game writes).
- 0x801F1000+ corrupted by DEBUG.BIN room overlay (later region).
- v1.18 layout: 4096B used / 768B free.

Source: `memory/project_patch_bin.md`, `scripts/patch_save_final.py:69-83`.

### 2.4 Save buffer & data RAM — CONFIRMED
| Address | Size | Purpose |
|---------|------|---------|
| 0x80100000 | ~2MB | STAGE*.BIN data, ITEMALL.PIX (file 11) load target |
| 0x80130000+ | runtime | Room textures/models — DO NOT USE |
| 0x80150000 | dynamic | Voice CD load buffer (Phase 3 voice work) |
| 0x80160000 | save buf | Save buffer (was 0x80200000 — moved to avoid shift) |
| 0x801BD800 | 768B | Animation dispatch table (MODEL_BKP backup target) |
| 0x800B0FBC | 0x1230 | GSB game state buffer |
| 0x800ACA38 | u32 | Flag 0 (game state) |
| 0x800ACA3C | u32 | Flag 1 (story/cutscene) |
| 0x800ACA40 | u32 | Flag 2 (story events) |
| 0x800ACA44 | u32 | Flag 11 (preserved across loads) |
| 0x800ACA54 | entity | Player entity base |
| 0x800B25BC | u8 | Item box menu cursor (0=CHECK,1=MANAGE,2=EXIT,3=TAKE,4=USE) |
| 0x800B25C0..C3 | u8x4 | Inventory state machine MODE/SUB/RENDER bytes |
| 0x800B25C8 | u8 | display cursor / item slot |
| 0x800B25C9 | u8 | first-click slot for MANAGE (0xFF=none) |
| 0x800B25BD | u8 | inventory slot cursor |
| 0x800B5334 | 16B | CD channel status array |
| 0x800B5359 | u8 | save handler activation flag |
| 0x800B8520 | text | Native text FSM state struct (32B) |
| 0x800BE570 | u8 | cd_state machine counter |
| 0x800BE574 | u32 | CD_read_direct file_size param |
| 0x800BE57C | u32 | CD_read_direct LBA param |
| 0x800BF800 | 768B | MODEL_BKP — animation table backup |
| 0x800BFB00 | 192B | BOX_STORAGE — 48 item slots x 4B |
| 0x800BFBC0 | 728B | BOXFULL_CHAIN GPU primitives |
| 0x800BFE98 | 40B | PLAYER_INV_SAVE for Take mode |

Sources: `memory/save-system.md`, `memory/itembox_architecture.md:50-72`,
`memory/inventory_renderer_architecture.md:336-365`,
`memory/cd_loading_system.md:60-72`.

### 2.5 SPU RAM map — PARTIAL (game allocator behavior fully RE'd; safe region empirical)
- 0x000000-0x000FFF: Reverb work area (4KB).
- 0x001000+: Game VABs (BGM + SFX banks). STAGE1 typical: 200-300KB.
- 0x60000+: First plausibly free area (used by Phase 3d voice subsystem).
- 0x80000 = top (512KB total).
- VAB tracking globals: 0x800bbe00 (count), 0x800bbe08 (base addrs[16]),
  0x800bbda8 (sizes[16]), 0x800b5334 (state[16]). SpuMalloc head at 0x80076dcc.

Source: `memory/phase1_spu_mapping.md`.

### 2.6 SPU registers (hardware MMIO) — CONFIRMED for the registers we touched
| Address | Name | Notes |
|---------|------|-------|
| 0x1F801C00..N*0x10 | Voice N regs | VOL_L/R, PITCH, START_ADDR/8, ADSR1, ADSR2, REPEAT_ADDR |
| 0x1F801D80 | Main vol L/R | |
| 0x1F801D88 | KON | Write-only triggering |
| 0x1F801D8C | KOFF | Write-only release |
| 0x1F801D9C | ENDX | Read-only voice end flags |
| 0x1F801DAA | SPUCNT | bit 0 = CD audio enable |
| 0x1F801DAE | SPUSTAT | applied state |
| 0x1F801DB0/B2 | CD vol L/R | |

Voice 23 used by our voice subsystem. Source: `memory/agent_spu_readback_probe.md:8-37`,
`memory/phase3c_stufe3_libsnd_bypass.md:43-54`.

### 2.7 CD I/O ports — CONFIRMED (documented; we did not use raw I/O successfully)
0x1F801800-0x1F801803, INDEX-multiplexed. See `memory/xa_streaming_design.md:18-62`.

---

## 3. SCD scripting

### 3.1 Opcode table (highlights) — CONFIRMED for documented opcodes
Full table: `src/main/java/de/re15/extractors/SCDScriptDisassembler.java:191-333`.

| Opcode | Name | Size | Notes |
|--------|------|------|-------|
| 0x00 | nop | 1 | |
| 0x01 | Evt_end | 2 | |
| 0x06 | Ifel_ck | 4 | block_length follows |
| 0x07 | Else_ck | 4 | |
| 0x08 | Endif | 2 | |
| 0x09 | Sleep | 4 | |
| 0x0D | For | 6 | |
| 0x0F | While | 4 | |
| 0x21 | Ck | 4 | flag check |
| 0x22 | Set | 4 | flag set |
| 0x29 | Cut_chg | 2 | camera change |
| **0x2B** | **Message_on** | **4** | text display; **HOOKED** for voice (op2B) |
| 0x2C | Aot_set | 20 | |
| 0x2D | Obj_model_set | 38 (variable) | |
| **0x36** | **Se_on** | **12** | SFX play (bank, id, vol, pan, x, y, z) |
| 0x44 | Sce_em_set | 20 | spawn enemy |
| 0x4E/0x50 | Item_aot_set | 22 | item spawn |
| **0x59** | **Xa_on** | **4** | XA audio (originally dummy `0x8003FE90`); now PB_VOICE_HANDLER |
| 0x5F | Xa_vol | 2 | XA volume |
| 0x67 | Aot_set_4p | 28 | |
| 0x68 | Door_aot_set_4p | 40 | |
| 0x69 | Item_aot_set_4p | 30 | |
| 0x6F | Movie_on | 2 | STR file movie |
| 0x76 | Sce_item_get | 3 | give item |
| 0x80 | Se_vol | 2 | |
| 0x81-0x8E | Op81..Op8E | varies | mostly UNKNOWN; treated as padding/no-op |

### 3.2 Dispatch table — CONFIRMED
Located in PSX.EXE; entry [0x2B] at file offset `0x064D54`. Table base =
`SCD_TABLE_0x2B_FILE - 0x2B*4 = 0x064CA8`. Each entry = 4 bytes (function pointer).
Entry [0x59] at file offset `0x064E0C`. We patch entry [0x2B] to PB_OP2B_WRAPPER and
entry [0x59] to PB_VOICE_HANDLER.

Source: `scripts/patch_save_final.py:97-104`,
`memory/phase3d3_op2b_msg_voice_hook.md:39-50`.

### 3.3 Save-handler hooks — CONFIRMED
| Hook | Mechanism | Match |
|------|-----------|-------|
| `[A] SCD_HOOK` @ 0x8006EA20 | Wraps op2B/Message_on | msgIndex=0xFE AND flags=0xFFFF |
| `[D2] AOT_TYPE1_HOOK` @ 0x8007084C | Replaces type=1 entry in AOT dispatch table at 0x8007469C+4 | message index halfword = 0xFE |

Per-room change to enable save: 1 byte in RDT (msgIndex 0x01 -> 0xFE for
type=0 rooms, act 0x14 -> 0xFE for type=1). See
`memory/save-system.md:159-203`, `scripts/patch_save_final.py:155-159`.

### 3.4 Voice via op2B (Phase 3d-3) — CONFIRMED
Voice triggered by SCD opcode 0x2B (text-display) by baking `voice_id` into arg2:
`2B NN 00 00` -> `2B NN VV 00`. Native handlers ignore arg2/arg3; our wrapper reads VV.
Build-time RDT patcher `patch_rdt_voices_room1240()` scans 0x500..0x800 for matching
opcodes, looks up `voice_id` from `VOICE_LOOKUP[(room_id, msg_id)]`.

Source: `memory/phase3d3_op2b_msg_voice_hook.md:54-69`.

### 3.5 SCD growth recipe — CONFIRMED
Inserting N bytes into mainScd or sub:
1. Update SCD pointer table entries past insertion point.
2. Update Ifel_ck/For/While/Else_ck `block_length` fields that span insertion.
3. Update RDT address table (0x40 onwards) for every section >= grown SCD start.
4. For new Item_aot_set: increment nItem at RDT 0x03.

Source: `memory/scd_extension_guide.md:110-137`.

---

## 4. Audio subsystem

### 4.1 RE1.5 audio paths — CONFIRMED (decisive RE)
| Path | Files | Mechanism | Status |
|------|-------|-----------|--------|
| BGM | 47x MAIN*.BGM (SEQ+VAB) | PSY-Q libsnd -> SPU voices 0-23 | works natively |
| SFX | ARMS00-14 + CORE00-13 EDH/VB | Se_on opcode 0x36 -> SPU voices | works natively (92 uses in STAGE1) |
| Intro audio | CD-DA red book track | CdlPlay -> CD hardware DAC | works natively |
| Voice | NONE in original game | originally Xa_on (0x59) was dummy stub | WE ADDED |

**RE1.5 NEVER uses SPU CD-input** (0x1F801DAA bit 0). No CdlSetfilter in PSX.EXE
strings, no CdlSetmode bit 0x40 in execution paths. Source:
`memory/agent_re15_audio_paths.md`.

### 4.2 Voice subsystem (Phase 3d, current) — CONFIRMED working in v1.21
Architecture:
- VOICE.PAK file at end of CD catalog (sector-aligned VAG concat).
- voice_table at 0x80071A30 (Cave 2): 64 entries x 8B each = `[MSF:3][sectors:1][size:4]`.
- voice_id 0 = no-op sentinel; 1..63 = real voices.
- PB_VOICE_PLAY at 0x801F11FC (PATCH2.BIN, 428B): subroutine, voice_id in a0.
- PB_OP2B_WRAPPER at 0x801F1438 (68B): SCD dispatch[0x2B], reads arg2.
- PB_VOICE_HANDLER at 0x801F13A8 (84B): SCD dispatch[0x59] (legacy path).
- Single SPU buffer at 0x60000 (96KB into SPU RAM).
- Plays on voice 23 (KON, ADSR1=0x80FF, ADSR2=0x1FEE, pitch 0x0400).
- Skip if `loaded_voice_id == voice_id` (re-KON only).
- 6 intro voices (ROOM1240 main00..main05) ~5-7s each, ~33-44KB VAGs.

Sources: `memory/phase3d3_op2b_msg_voice_hook.md`, `memory/phase3d_voice_table_scaling.md`,
`memory/phase3c_stufe3_libsnd_bypass.md`.

Tooling: `scripts/encode_vag.py` (WAV->VAG), `scripts/extend_vab.py`.

### 4.3 PATH A (XA streaming) — UNKNOWN / FAILED
6+ iterations (C-1 through C-6 plus 10-agent analysis). Every checkable layer
verified green:
- SCD dispatch fires (DEAD sentinel reached)
- libcd commands return success
- SPU register writes confirmed via readback (SPUCNT=0xC001, CD vol=0x3FFF)
- Encoder fixed (8 separate per-unit predictor states)
- File on disc verified byte-identical to source

But **no audio output**. Hypothesis: game's libcd has CD-DA "play continuously"
state that takes priority; queued cmds accepted but never drain to hardware.
Architectural conclusion: RE1.5 prototype's hardware path may be unreachable
from outside the game's design.

Sources: `memory/xa_path_a_exhausted.md`, `memory/xa_path_a_revival_plan.md`,
`memory/agent_xa_decoder_validation.md` (decoder works; encoder fixable),
`memory/agent_psxbuild_xa_format.md` (psxbuild faithful, 2336B sectors),
`memory/agent_xa_capcom_compare.md` (CAPCOM.STR vs VOICE00.XA).

### 4.4 PATH D (SPU streaming, double-buffered) — FAILED
18 iterations. SpuRead causes blackscreen on subsequent room change regardless of
SPU address (0x40000, 0x60000, 0x70000, SpuMalloc result). State machine works,
gong test plays, but room transitions corrupt. Pivoted to Phase 3d (per-room VAB).
Sources: `memory/path_d_session_complete.md`, `memory/path_d_streaming_plan.md`.

### 4.5 PATH X (SsVabOpen + libsnd VAB injection) — FAILED
libsnd's `SsVabOpen + SsVabTransBody` causes the SAME blackscreen as raw SpuRead.
Architecture rejected by user (33KB scratch per voice, scales poorly; overrode
existing buffers). Source: `memory/phase4_option_x_failed.md`.

### 4.6 CD wrapper at 0x8004EE78 — CONFIRMED DESTRUCTIVE
Game's high-level CD command queue. Calling convention requires pre-initialized
context returned by 0x8006BBCC. cmd_word at 0x800ACA4C. **Cannot be called from
SCD opcode handlers**: even mirroring caller bytes-for-bytes, downstream font/SFX
state corrupts. 11 native callers; verdict: "it's not the cmd code, it's the timing."

Source: `memory/re_cd_wrapper_0x8004EE78.md:170-220`.

### 4.7 Direct libcd entry points — CONFIRMED
| Address | Function | Notes |
|---------|----------|-------|
| 0x80062948 | CdControl | non-blocking |
| 0x80062a80 | CdControlF | no result |
| 0x80062bac | CdControlB | **blocking** (15+ game callers) |
| 0x800628d8 | CdSync | wait status |
| 0x800628f8 | CdReady | poll readiness |
| 0x80062cf0 | CdMix | audio mixer |
| 0x80062d14 | CdGetSector | XA data extraction |
| 0x800625a0 | CdRead | low-level sector read |
| 0x800626a8 | CdReadSync | sync read |
| 0x80064600 | CdInit | subsystem init |
| 0x80062930 | CdReadyCallback | callback register |
| 0x80062918 | CdSyncCallback | callback register |
| 0x80063e0c | CD_vol | (linked, never called by game) |
| 0x80063fe4 | CD_initvol | (linked, never called by game) |

libcd globals: 0x8007E348 (service-vector struct), 0x8007E34C (queue-sync
fn ptr), 0x8007E352 (idle-state byte). Source: `memory/re15_cd_callable_inventory.md`.

### 4.8 libsnd entry points — PARTIAL (some confirmed by callsite analysis, some inferred)
| Address | Function | Status |
|---------|----------|--------|
| 0x8005ce94 | SsVabTransBody | confirmed |
| 0x8005c3e8 | SpuMalloc | confirmed |
| 0x8005cf54 | SpuSetTransferMode | confirmed |
| 0x8005cf88 | SpuSetTransferStartAddr | confirmed |
| 0x8005cfc4 | SpuRead (main->SPU) | confirmed |
| 0x800601c8 | SpuIsTransferCompleted | confirmed |
| 0x80060c34 | SsVabClose | confirmed (9 jal sites) |
| 0x80060030 | (libsnd inner — unknown) | 4 jal sites |
| 0x8006019C | (likely SsUtKeyOn) | 8 jal sites |

Source: `memory/phase1_spu_mapping.md`, `memory/edh_vb_format.md:120-131`,
`memory/phase3c_stufe3_libsnd_bypass.md:99-104`.

### 4.9 Helicopter SFX regression — UNKNOWN root mechanism, PARTIAL fix proposed
Helicopter SFX after intro silent since commit `a1e07588` (v1.4 era). Bisected:
CD_FIX patch (now at 0x80070720) clears 16 bytes of CD channel status indiscriminately
when channel 2 is active. Wipes SFX channel state mid-playback. Independent of
voice work. Source: `memory/bug_helicopter_sfx_cd_fix_regression.md`.

---

## 5. 3D pipeline

### 5.1 Models — CONFIRMED
- MD1 mesh: 12B header (length, unknown, objectCount). Per mesh: 56B header
  with 14 u32 (vertex/normal/face/UV offsets+counts for tris and quads).
  Vertex stride = 8B (x16, y16, z16, pad). Normal stride = 8B. Triangle = 12B,
  Quad = 16B. Triangle UV = 12B (u/v + clut + page). Quad UV = 16B.
- Source: `extractors/md1/MD1File.java:120-330`.

### 5.2 Skeleton — CONFIRMED
- EMR header 8B (bonesOffset, framesOffset, boneCount, keyframeSize, all u16).
- Per bone: 6B relative position (x16, y16, z16).
- Per bone-link: 4B (meshCount, childOffset).
- Per keyframe: 12B position+speed + variable-length packed angles.
- Bone rotations: **12 bits per axis** (x, y, z), packed via BitReader.
  4096 = 360 degrees. Decoded to radians via `EmdMath.deg12ToRad`.
- BFS to assign parents; multiple roots possible.
- Source: `extractors/emd/SkeletonParser.java:17-156`, `extractors/emd/BitReader.java`.

### 5.3 Animation — CONFIRMED
- EDD per clip: count + offset table at start. Each clip frame = u32 with low
  12 bits = key index, upper bits = flags.
- Source: `extractors/emd/AnimationParser.java`.

### 5.4 Texture pages — CONFIRMED
- TPAGE 12 (E100001C) at X=768, Y=256, 4-bit CLUT 0x7810: serif font (V=32-128).
- TPAGE 0 (E1000080): CONFIG.TIM button icons (CLUT 0x7800).
- During item box render, TPAGE 12 rows V=32-64 are OVERWRITTEN by UI textures.
  Only V=80-112 chars safe.
- Source: `memory/font_texpage04.md`.

### 5.5 glTF export — CONFIRMED
PSX -> glTF Y-flip (`Y' = -Y`). ZYX Euler quaternion order. Animations exported
per-clip. Source: `extractors/emd/EmdGltfExtractor.java`,
`extractors/emd/GltfExporter.java`.

### 5.6 Damage/blood texture — UNKNOWN (deferred)
Blood disappears after save+load. Three required data blocks all zero post-load:
particle slots (792B at 0x800A73B8), sprite state (152B at 0x800B25CC), entity
damage fields. Apply-function not located. Source:
`memory/bug_damage_model_after_load.md`.

### 5.7 Damage CLUT swap (Leon) — CONFIRMED
4 CLUT palettes per PLD, selected per-frame from HP status. Status thresholds
table at 0x800112B4. Selector at `0x8004ED6C`. Polygon renderer at `0x80047648`
applies `CLUT_Y = base + status*16`. Source: `memory/leon_texture_system.md`.

---

## 6. Backgrounds (BSS) / MDEC

### 6.1 BSS file — CONFIRMED
- 64KB chunks per camera angle. 320x240 frames. VLC ID = 0x3800.
- Source: `extractors/bss/BssExtractor.java:14-130`.

### 6.2 VLC depacker — CONFIRMED
Port of `depack_vlc.c`. EOB code = packCode(63, 512, 2), ESCAPE_CODE =
packCode(63, 0, 6). Predictive DC for version != 2.
Source: `extractors/bss/VlcDecoder.java`.

### 6.3 MDEC IDCT + YCbCr -> RGB — CONFIRMED
6 blocks per macroblock, 64 samples each. ZSCAN, IQ_TABLE, AAN_SCALES, fixed-point
12-bit shift, 1.402 / -0.3437 / -0.7143 / 1.772 YCbCr-to-RGB. Source:
`extractors/bss/MdecDecoder.java`.

### 6.4 BSS reuse for voice — UNKNOWN / NOT VIABLE
BSS streaming function `0x80021138` takes no parameters; uses destructive
0x8004EE78 wrapper internally; routes to data RAM at 0x80100000 (no SPU). State
machine bound to byte at 0x800ACA34 via `0x8001A9A8`.
Source: `memory/re15_bss_streaming_reuse.md`.

---

## 7. Patch / save / inventory architecture

### 7.1 PATCH.BIN architecture — CONFIRMED
- 2 sectors (4096B), at LBA 73388, root level before ZNULL.DAT.
- Loaded into 0x801F0000 by PATCH_LOADER (Cave 3, 120B at 0x8007153C).
- PATCH_LOADER uses CD_read_direct (0x80013C50) with cd_state guard (0x800BE570==0).
- Two-pass build: first build -> find ISO9660 LBA -> patch loader lui+ori -> rebuild.
- Header magic "PTCH" at 0x801F0000.
- v1.18 layout: ~3328B used, 768B free. Function table see
  `memory/itembox_architecture.md:7-25` and `memory/manage_phase_b_impl.md:34-42`.

### 7.2 Save flow — CONFIRMED
Save buffer at RAM 0x80160000. Card size 0x1680. Save offset 0x0200 from buffer base.
Two trigger paths:
- Type=0 AOT (SCD path): SCD_HOOK at 0x8006EA20 catches Message_on(0xFE, 0xFFFF).
- Type=1 AOT (dispatch path): AOT_TYPE1_HOOK at 0x8007084C replaces type=1
  entry in dispatch table.

Save chain (12 steps): EXTENDED_SNAPSHOT -> HP_SAVE -> RESTORE_CODE -> FLAGS_1_11_MERGE
-> STASH_PERROOM -> INVENTORY_STASH -> RELOAD_CHECK -> ROOM_FLAG_SKIP -> INV_INIT_WRAPPER
-> FUN314B0_WRAPPER (weapon) -> camera-hook restore -> LOAD_RESTORE_FULL.

Source: `memory/save-system.md:79-89`, `scripts/patch_save_final.py:1-46`.

### 7.3 Item box state machine — CONFIRMED
- Pass 1 dispatch table at 0x80074BE8 (3 entries: normal, item box, CHECK).
- Pass 2 dispatch table at 0x80074BF4.
- BROWSE jump table at 0x8001126C (5 entries: 0=CHECK, 1=MANAGE, 2=EXIT, 3=TAKE, 4=USE).
- CCA4 dispatch table at 0x80074C7C (state[25C1] indexes: 0=BROWSE, 1=ACTION, 2=EXIT).

Source: `memory/inventory_renderer_architecture.md`,
`memory/manage_phase_a_findings.md:15-44`.

### 7.4 MANAGE highlight (v1.18) — CONFIRMED
Native render gate at 0x8004C928-0x8004C93C (6 instructions) rewritten:
original compared masked state[25C0..C3] word to 0x00030200; new fires when
state[25C9] != 0xFF (MANAGE first-click slot active). Reuses native combine
highlight renderer (state[25CF] slot, state[25DC/25DE] coords, table at
0x80076244/6246). Zero custom prim render. Source:
`memory/manage_phase_b_impl.md:111-133`.

### 7.5 CHECK display — CONFIRMED
Native CHECK handler `0x800C6630` (DEBUG.BIN overlay), panel renderer
`0x800C6228`, ITPS image loader `0x800C6878`. Direct GPU DMA channel 2
(D2_MADR=0xBF8010A0, D2_BCR=0xBF8010A4, D2_CHCR=0xBF8010A8 with linked-list
mode 0x01000401). Two separate OTs: panel OT from 0x800C750C, text OT at fixed
0x800AA6B4.
Source: `memory/itps_loading_system.md:1-90`,
`memory/inventory_renderer_architecture.md:107-167`.

### 7.6 CD loading system — CONFIRMED
- File table at 0x8006F43C: 256 entries x 8B (size, sector_lo, sector_hi, pad).
- All 256 entries used; entry 7 (DEBUG.BIN) MUST NOT be modified.
- CD_read at 0x80013B60: `CD_read(file_index, dest_addr, mode)`.
- CD_read_direct at 0x80013C50: pre-set 0x800BE574 (size) + 0x800BE57C (LBA).
- CD_read_stage_relative at 0x80013CE8: stage-relative subfile loader.
- 6-state CD state machine at 0x80013DF0.

Key file indices: 0-5 STAGE1-6.BIN, 6 TITLE.BIN, 7 DEBUG.BIN, 8 CONFIG.TIM,
11 ITEMALL.PIX (84KB at sector 565), 42 ITPS.ITP (884KB), 43 CAPCOM.STR (6.1MB).

BIOS A-table calls (FileOpen/Read/Close) FAIL when called from SCD_FRAME_CALL
context (LIBCD conflict). Use CD_read_direct instead.

Source: `memory/cd_loading_system.md`.

### 7.7 Text rendering — CONFIRMED
- show_message at 0x80027E68: `(a0=text_id_or_pos, a1=mode, a2=group, a3=ACA40_ormask)`.
- Text state struct at 0x800B8520 (32B layout in `memory/text_rendering_architecture.md:21-41`).
- Per-frame wrapper at 0x800280B4: FSM (7 states) -> render_commit -> OT.
- Width table at 0x800C4416 (DEBUG.BIN offset 0x4416, 256 entries).
- Glyph UV formula: `V = (byte & 0xF) * 16; U = (byte >> 4) * 16 + 32`.
- Encoding: `byte = ASCII - 0x24` for alphanumerics. See `memory/native_text_encoding.md`.

---

## 8. CD-image / build constraints

### 8.1 Hardcoded LBAs — CONFIRMED
**Game freezes** if any SOUND/* file grows even by +1 sector (+512B). Tested:
extending CORE00.VB by +1 silent VAG -> freeze right after Capcom logo. RE1.5 has
hardcoded LBAs / sector counts for files after `info/Re1.5/PSX/SOUND/CORE00.VB`.
Source: `memory/hardcoded_lba_constraint.md`.

### 8.2 Allowed file mutations — CONFIRMED
- Append to PATCH.BIN (already at end of root, before ZNULL.DAT).
- Add NEW files at root cat tail (only shifts ZNULL.DAT, which is unused).

Forbidden:
- Growing any SOUND/CORE??.{EDH,VB}, ARMS??.{EDH,VB}, MAIN??.BGM.
- Growing any STAGE?.BIN, TITLE.BIN, DEBUG.BIN.
- Adding files anywhere in middle of cat.

### 8.3 Sector alignment — CONFIRMED
PSX CD sectors = 2048 bytes. EMS archives use 2048B alignment with 36-byte
directory footer (`RE15MasterExtractor.java:42`). XA files = 2336B per sector
(`memory/agent_psxbuild_xa_format.md`).

### 8.4 Build pipeline — CONFIRMED
- `scripts/patch_save_final.py` (5355 lines) — main build script.
- Two-pass build: generate PATCH.BIN -> add to catalog -> patch hooks ->
  build CD -> find PATCH.BIN sector -> patch loader -> rebuild CD.
- psxbuild = `C:\Users\mjoedicke\Downloads\psximager-2.2\psximager-2.2\src\psxbuild.exe`
  (`scripts/patch_save_final.py:63`).
- Output: `info/Re1.5_PATCHED/`.
- PostToolUse hook re-runs `patch_save_final.py` -> side-scripts that patch
  `Re1.5_PATCHED` get silently reverted; keep all build mutations inside
  `patch_save_final.py`. Source: `memory/feedback_post_tool_hook_resets_build.md`.

---

## 9. Known bugs / regressions

| Bug | Status | Memory |
|-----|--------|--------|
| Recurring pixel shifts (background) | OPEN — user repeatedly reports | global CLAUDE.md, `memory/project_todos_after_v1_18.md:54-56` |
| Helicopter SFX missing since v1.4 era | OPEN — root cause known | `memory/bug_helicopter_sfx_cd_fix_regression.md` |
| Damage/blood texture not persisting after load | DEFERRED — full RE done, fix complex | `memory/bug_damage_model_after_load.md` |
| Path A XA streaming silent | EXHAUSTED | `memory/xa_path_a_exhausted.md` |
| Path D SpuRead corrupts on room change | UNKNOWN root cause | `memory/path_d_session_complete.md` |
| Phase 4 Option X same blackscreen | LIBSND ALSO CORRUPTS | `memory/phase4_option_x_failed.md` |
| Item duplication in Take mode (since v1.6) | FIXED v1.15 (PB_COMPACT_BOX_SYNC) | `memory/bug_item_duplication.md` |
| Room 114 freeze after save/load | FIXED v1.15 (ITEMALL.PIX -> 0x80160000) | `memory/bug_room114_icon_refresh.md` |
| HP reset after load | FIXED v1.16 (HP_POLL[17] sw zero) | `memory/MEMORY.md:101-104` |
| MIPS load delay in MANAGE swap | FIXED v1.17 (nop after `lw`) | `memory/manage_phase_b_impl.md:81-95` |
| Flag address collision (saved_cursor over Cave 2 text) | FIXED v1.12 | `memory/root_cause_analysis.md` |

---

## 10. Open questions / unknown

| Question | Source |
|----------|--------|
| Why does SpuRead corrupt state until next room load? | `memory/phase4_option_x_failed.md:74-80` |
| Where exactly are the hardcoded LBAs (which table)? Theory: 0x80073A88 16-entry u16 bank-id->file-index map. | `memory/hardcoded_lba_constraint.md:50-61` |
| Recurring pixel shifts in backgrounds — root cause | Reported repeatedly by user |
| Damage-apply function (where particle slots get written) | `memory/bug_damage_model_after_load.md` |
| Combine table location (item_a + item_b -> result) | `memory/project_todos_after_v1_18.md:33-34` |
| SCD opcodes 0x81-0x8E — semantics partly unknown | `extractors/SCDScriptDisassembler.java:297-320` |
| Capcom EDH prefix table (16 entries x 4B) — semantics | `memory/edh_vb_format.md:30-34` ("IGNORED for our voice work") |
| libsnd entry points 0x80060030 (4 calls) and many others | `memory/edh_vb_format.md:124-131` |
| Where the BGM SEQ format is parsed (only converter exists, not full RE) | `extractors/audio/SeqToMidiConverter.java` |
| Room 1170 voice mapping (mp3 sources exist in `synchro/002-ROOM1170/` but not yet ported to VAG) | `memory/phase3d3_op2b_msg_voice_hook.md:135-141` |

---

## 11. Reverse-engineered EXE addresses (highlights)

### 11.1 CD subsystem
| Address | Function | Notes |
|---------|----------|-------|
| 0x80013B60 | CD_read | file_index loader |
| 0x80013C50 | CD_read_direct | LBA loader (used by PATCH_LOADER) |
| 0x80013CE8 | CD_read_stage_relative | stage subfile |
| 0x80013DF0 | CD state machine | 6 states |
| 0x80062948 | CdControl | libcd |
| 0x80062bac | CdControlB | libcd blocking |
| 0x800628d8 | CdSync | libcd sync |
| 0x80062cf0 | CdMix | libcd mixer |
| 0x80064600 | CdInit | libcd init |
| 0x8004EE78 | game CD wrapper (DESTRUCTIVE from SCD context) | 11 callers |

### 11.2 Audio (libsnd / libspu)
| Address | Function |
|---------|----------|
| 0x8005c3e8 | SpuMalloc |
| 0x8005ce94 | SsVabTransBody |
| 0x8005cf54 | SpuSetTransferMode |
| 0x8005cf88 | SpuSetTransferStartAddr |
| 0x8005cfc4 | SpuRead (main->SPU) |
| 0x800601c8 | SpuIsTransferCompleted |
| 0x80060c34 | SsVabClose |

### 11.3 Inventory / item box
| Address | Function |
|---------|----------|
| 0x80049524 | Normal Inventory Render (Pass 1[0]) |
| 0x8004C54C | Item Box Render (Pass 1[1]) |
| 0x8004D7CC | CHECK/Special Render (Pass 1[2]) |
| 0x800497Cx | Normal Inventory Logic (Pass 2[0]) |
| 0x8004CCA4 | Item Box Logic (Pass 2[1] / CCA4 dispatch) |
| 0x8004CD6C | BROWSE handler (CCA4[0]) |
| 0x8004CDCC | MANAGE handler (BROWSE[1]) — 5 instr stub |
| 0x8004CE04 | EXIT handler (BROWSE[2]) |
| 0x8004CE20 | TAKE / CHECK fallthrough (BROWSE[0,3]) |
| 0x8004CDE0 | USE handler (BROWSE[4]) |
| 0x8004CEF4 | ACTION MENU (CCA4[1]) — slide animation only |
| 0x8004D128 | Take/CHECK browse (CCA4[2]) |
| 0x8004D254 | DEPOSIT confirm site |
| 0x8004DADC | compact_inventory |
| 0x80045DF8 | inventory_init |
| 0x80049A5C | core renderer (left+right panels) |
| 0x800473F8 | USE confirmation popup (NOT CHECK) |
| 0x80046FD8 | full inventory init (796B; calls MUST be guarded) |
| 0x80047648 | inventory primitive processor (4280B) |
| 0x800708CC | ITEM_BOX_CHECK (Cave 2) |
| 0x800708F0 | ITEM_BOX_OPEN (Cave 2) |
| 0x80071920 | DEPOSIT_HOOK (Cave 2) |

### 11.4 Native CHECK / panel (DEBUG.BIN overlay 0x800C0000+)
| Address | Function |
|---------|----------|
| 0x800C6630 | CHECK handler (5 phases) |
| 0x800C6228 | ITEM DATA panel renderer |
| 0x800C6878 | ITPS image loader |
| 0x800C750C | OT entry getter |
| 0x800C00A8 | weapon-init overlay |

### 11.5 Text / message
| Address | Function |
|---------|----------|
| 0x80027E68 | show_message |
| 0x800280B4 | text_per_frame_wrapper |
| 0x80028134 | text FSM |
| 0x80028868 | render_commit |
| 0x8003EE3C | text_slot_init (CD18 handler) |
| 0x800404F4 | original Message_on handler (we tail-jump to this) |
| 0x8006EA20 | SCD_HOOK (our cave hook for save trigger) |
| 0x80019700 | native text helper for CHECK |

### 11.6 Save / load chain (our patches)
| Address | Function |
|---------|----------|
| 0x80070564 | RELOAD_CHECK |
| 0x800705C4 | RELOAD_RFI |
| 0x800706E0 | STASH_PERROOM |
| 0x80070720 | CD_FIX (helicopter regression source) |
| 0x80070818 | CARD_CLEANUP_CDFIX |
| 0x80070738 | INV_INIT_WRAPPER (weapon restore) |
| 0x80070778 | FUN314B0_WRAPPER (weapon model restore) |
| 0x80070854 | original CD_FIX location (now relocated) |
| 0x8007084C | AOT_TYPE1_HOOK |
| 0x8007153C | PATCH_LOADER |
| 0x8006EA84 | ROOM_FLAG_SKIP entry |
| 0x8003E4F4 | room_flag_init (native, we conditionally skip) |

### 11.7 Camera / room init
| Address | Function |
|---------|----------|
| 0x800316D0 | FUN_800314B0 (room_init / equip handler) |
| 0x80036B68 | weapon model loader |
| 0x80043D8C | weapon model setup |
| 0x80019E20 | prim_command_tick |
| 0x8001D5A4 | jal site we hook for FUN314B0_WRAPPER |
| 0x8001D578 | jal site we hook for INV_INIT_WRAPPER |
| 0x80039974 | room init CD read site (CD_FIX hook) |

### 11.8 GPU helpers
| Address | Function |
|---------|----------|
| 0x80021634 | GPU buffer select / mode |
| 0x800216EC | fill rectangle |
| 0x800217B0 | setup draw area |
| 0x8002178C | GPU ready check |
| 0x80029AC8 | DrawOTag |
| 0x8006B538 | addPrim |
| 0x8002BD44 | VSync |
| 0x8002C18C | swap buffers |

### 11.9 Damage / status
| Address | Function |
|---------|----------|
| 0x8004ED6C | HP -> Status (FINE/CAUTION/DANGER/POISON) |
| 0x80048A44 | per-frame RGB color modulation |
| 0x800112B4 | HP threshold table |
| 0x80046148 | CLUT table setup |

### 11.10 Game globals
| Address | Purpose |
|---------|---------|
| 0x800ACA34 | room/cutscene state-id byte |
| 0x800ACA38..40 | flag 0/1/2 |
| 0x800ACA44 | flag 11 |
| 0x800ACA4C | game CD cmd queue (16-bit halfword) |
| 0x800ACA54 | player entity base |
| 0x800AC77C | room context pointer (a1 for FUN_800314B0) |
| 0x800B5359 | save trigger flag |
| 0x8007469C | AOT dispatch table base |
| 0x80074BE8 | inventory Pass 1 dispatch |
| 0x80074BF4 | inventory Pass 2 dispatch |
| 0x80074C7C | CCA4 dispatch table |
| 0x80073A88 | possible bank-id -> file-index map (theory) |
| 0x800788a4 | CdSync callback |
| 0x800788a8 | CdReady callback |

---

## Conventions captured (cross-cutting)

- PSX is **little-endian**.
- PSX coordinate Y is inverted vs OpenGL/glTF.
- Bone rotations 12-bit packed (4096 = 360 deg), ZYX Euler.
- CD sectors = 2048 bytes (data) or 2336/2352 (XA).
- MIPS R3000: **load delay** (1 nop after `lw/lbu/lhu` if next instr uses dest register).
- MIPS delay slot: **instr after branch/jump always executes**.
- Minimum stack frame = 24B for jal (16B arg save + ra).
- PSX button convention: **Square = CONFIRM, X = CANCEL**.
- 0x801F0000 = safe RAM (PATCH.BIN), zeroed at game start only.
- 0x800C0000+ is UNSAFE — room loading overwrites it (DEBUG.BIN overlay).
- 8-bit writes to ODD SPU addresses are silently ignored. Use `lhu/sh` for SPU regs.
- Default SsVabTransBody DMA channel + ADSR1=0x80FF, ADSR2=0x1FEE, vol=0x7F, pan=0x40 are
  the working canonical values for our voice subsystem.

## Index of memory files (referenced above)
Path prefix: `memory/`

`MEMORY.md` (top index), `save-system.md`, `cd_loading_system.md`,
`itembox_architecture.md`, `inventory_renderer_architecture.md`,
`itps_loading_system.md`, `manage_phase_a_findings.md`,
`manage_phase_b_impl.md`, `project_patch_bin.md`, `project_todos_after_v1_18.md`,
`scd_extension_guide.md`, `text_rendering_architecture.md`,
`native_text_encoding.md`, `font_texpage04.md`, `leon_texture_system.md`,
`bug_damage_model_after_load.md`, `bug_helicopter_sfx_cd_fix_regression.md`,
`bug_room114_icon_refresh.md`, `bug_item_duplication.md`,
`re_cd_wrapper_0x8004EE78.md`, `re15_cd_callable_inventory.md`,
`re15_bss_streaming_reuse.md`, `agent_re15_audio_paths.md`,
`agent_se_on_vab_fallback.md`, `agent_xa_decoder_validation.md`,
`agent_psxbuild_xa_format.md`, `agent_xa_capcom_compare.md`,
`agent_xa_encoder_spec_audit.md`, `agent_spu_readback_probe.md`,
`xa_streaming_design.md`, `xa_path_a_revival_plan.md`, `xa_path_a_exhausted.md`,
`xa_hybrid_implementation_plan.md`, `re2_to_re15_voice_port_plan.md`,
`hardcoded_lba_constraint.md`, `edh_vb_format.md`,
`phase1_spu_mapping.md`, `phase3_streaming_design.md`,
`phase3b_voice_state_2026_05_01.md`, `phase3c_stufe3_libsnd_bypass.md`,
`phase3d_voice_table_scaling.md`, `phase3d3_op2b_msg_voice_hook.md`,
`phase4_option_x_failed.md`, `path_d_session_complete.md`,
`path_d_streaming_plan.md`, `voice_subsystem_plan.md`,
`voice_implementation_unified_plan.md`, `voice_per_room_vab_plan.md`,
`feedback_voice_must_stream.md`, `feedback_no_override_existing.md`,
`feedback_post_tool_hook_resets_build.md`, `versioning.md`,
`session_2026_03_29_complete.md`, `original_cd18_handler.md`.
