# RE1.5 Combat-SE Subsystem — byte-true reference

Produced by the 60-agent `combat-se-subsystem-re` workflow (2026-07-02): 5 parallel finders,
adversarial verification of every claim (42 confirmed / 12 corrected), synthesis. Every constant
cites `ghidra1_V2.txt` (PSX.EXE, RAM base 0x80010000), a STAGE1.BIN overlay address
(@0x80100000, file_off = addr−0x80100000, **no** 0x800 header), or a file byte-offset.

Companion to `RE15_KNOWLEDGE.md` (§1.7 ESP / VAB) and `RE15_SCD_OPCODES_REFERENCE.md`.
Port status is tracked in the memory `reai-v2-foundation-combat`.

---

## 1. SE-play core — `FUN_80045024(uint a0, pos)`

The single general SE trigger. `a0` packs bank/index/flags:

- `bank      = a0 >> 0x18`            — `srl v1,a0,0x18` @0x80045028
- `se_index  = (a0 >> 0x10) & 0xff`   — `srl v0,a0,0x10` @0x80045078 + `andi 0xff` @0x8004507c
- `flags     = a0 & 0xff`             — `andi a0,a0,0xff` @0x80045080; **!=0 ⇒ positional** (`FUN_80045a64(pos)`), ==0 ⇒ non-positional (full low byte, not just bit0)

### 6-way bank selector (`sltiu v0,v1,0x6` @0x80045094, jumptable @0x80010e70)

| bank | source | address | index bound |
|---|---|---|---|
| 0 | `&DAT_801fdd00` (resident) | `ori a0,a0,0xdd00` @0x800450cc | ≤0x20 |
| 1 | `&DAT_801fcd00` (weapon / **ARMS**) | `ori a0,a0,0xcd00` @0x800450e0 | ≤0x20 |
| 2 | → falls into case 5 | `j caseD_5` @0x800450f0 | ≤0x20 |
| 3 | `*(DAT_800ac778+0x14)` (RDT **snd1**, room/combat SE) | `lw a0,0x14(v0)` @0x80045110 | ≤0x18 |
| 4 | `&DAT_801fbd00` (**CORE**) | `ori a0,a0,0xbd00` @0x8004512c | ≤0x20 |
| 5 | `*(DAT_800ac778+8)` (RDT **snd0**, footsteps) | `lw a0,0x8(v0)` @0x80045138 | — |

**Bank-loaded guard**: signed byte `DAT_800b21ec[bank]` (`lb a1` @0x80045064, `beq a1,-1` @0x8004506c ⇒ return). −1 = not loaded; set to the `SsVabOpenHeadSticky` handle on open, reset to −1 on close. Sentinel-init'd by `FUN_80043a34` @0x80043a94-aac.

### SE record — 4 bytes at `bank_base + se_index*4` (`sll v0,s4,2` @0x80045140)

| byte | meaning | disasm |
|---|---|---|
| byte0 | bit7 = **VAB-ID override** (byte0&0x7f replaces the vabId arg, **not** program) | `andi 0x80` @0x80045150; `andi s8,v1,0x7f` @0x8004515c |
| byte1 | program = byte1 & 0x7f | `andi s7,v1,0x7f` @0x80045180 |
| byte2 | tone = byte2>>4; low nibble = channel/group | `srl s2,v0,4` @0x80045168; `andi s5,a0,0xf` @0x800451a0 |
| byte3 | voice/key = byte3&0x1f (≥0x10 ⇒ stop/off via FUN_80045a18); repeat = byte3>>5 (layered-SE loop LAB_80045320) | `andi s0,v0,0x1f` @0x8004517c; `srl s3,v1,5` @0x8004516c |

This is **byte-identical** to the record the port's `re15_footstep_vag` decodes for snd0/snd1
(byte1&0x7f=prog, byte2>>4=tone).

### Tone / VAG addressing

Per-bank VH base = word array `DAT_800b2598[bank]` (`lw a2,0x2598(at)` @0x80045090). Then:

```
tone_table_base = DAT_800b2598[bank] + (byte1&0x7f)*0x200 + 0x820   (@0x80045184-8c)
tone_entry      = tone_table_base + tone*0x20                       (@0x80045190-94)
```

0x820 = 0x20 header + 128×16 program table; 0x200 = 16 tones×32B. **Byte-identical to
`re15_vab_parse`** (HEADER_SIZE 32, PROGRAM_COUNT 128, TONES_PER_PROGRAM 16, TONE_ENTRY_SIZE 32).
VAG index = `tone_entry[+0x16]` (1-based) — resolved downstream by SsUtKeyOnV, not inside FUN_80045024.

`SsUtKeyOnV(voice, vabId, prog, tone, note=tone[+6], fine=tone[+5], voll, volr)` @0x8004522c.
voll/volr: non-positional ⇒ both = `tone[+2]` (vol byte); positional ⇒ `FUN_80045a64(pos)` panned L/R into DAT_800b2824/2826.

### Wrapper `FUN_800453d0(se_index)` = bank3/snd1 only

`if (*DAT_800ac784 & 0x2000) se_index += 0xc` (@0x8004540c); reject ≥0x19 (@0x80045420);
guard DAT_800b21ef==−1; record from `*(DAT_800ac778+0x14)+se_index*4`; **positional arg =
`DAT_800ac784 + 0x34`** (@0x800454ec — the decompile's `+0xd` is a pointer-typing artifact).
**Ported** as `re15_audio_room_se` (audio_pc.c).

---

## 2. Bank sources

| bank | loader | source | called |
|---|---|---|---|
| 0 resident | `FUN_800170e0` | in-RAM blob: memcpy `*DAT_801a1004` (VH) + SsVabTransBody `*DAT_801a1008` (VB) → `&DAT_801fdd00`. No CD I/O. Not a named SOUND/ file. | boot |
| 1 weapon | `FUN_80043d8c(weapon,vb_dst)` | equipped weapon's **ARMS** bank via CD file-id `FUN_80013b60` | `FUN_800314b0` @0x80031708 = `FUN_80043d8c(DAT_800aca5d, DAT_800ac77c)` at room-init |
| 2/5 snd0/snd1 | RDT-resident | `*(DAT_800ac778+8)` / `+0x14` | RDT load |
| 4 CORE | `FUN_800440c4(idx)` | **CORE** bank via CD file-id, SsVabOpenHeadSticky bank 4 size 0x38840 → `&DAT_801fbd00` | `FUN_800314b0` @0x800316e8 = `FUN_800440c4(DAT_800aca5c)` |

SPU RAM buffers @0x1000 spacing: bank4=0x801fbd00, bank1=0x801fcd00, bank0=0x801fdd00.

### File-id tables (`FUN_80013b60(id,dst,mode)` reads by numeric CD id; 8-byte/entry LBA table @0x8006f43c)

- bank1 VH ids @0x8007492c: weapon N → `0x77 + 2N` (0x77..0x9f)
- bank1 VB ids @0x80074958: weapon N → `0x78 + 2N` (0x78..0xa0)
- bank4 CORE EDH ids @0x80073a88: N → `0xa1 + 2N`
- bank4 CORE VBD ids @0x80073ab0: N → `0xa2 + 2N`

> ⚠ **UNPROVEN**: that id 0x77 == `ARMS00.EDH`. The EXE stores LBAs, not filenames; there is no
> CD-directory→name table in the repo. `ARMS{NN}`/`CORE{NN}` ordering is a strong inference
> (SOUND/ holds exactly 21 ARMS + 20 CORE EDH/VB pairs matching the id counts) but must be
> confirmed via the CD ISO directory or a runtime LBA match (DuckStation).

### EDH / VB format (byte-verified against real files)

```
.EDH = [EDT prefix: N × 4-byte SE records @0] + [Sony VAB VH "pBAV" (70 42 41 56) @pBAV_off] + [8-byte trailer]
```

- **trailer u32 @ file[size−8] = pBAV_off** (= the EDT-prefix size / VH offset); file[size−4]=0.
  The game locates the VH via `bank_base + *(u32*)(bank_base + loaded_size − 8)`.
- **pBAV_off is per-file, NOT constant** — hardcoding one offset misparses the other family.
  Verified: ARMS00=0x10 (4 records), ARMS01=0x28 (10), ARMS02=0x20 (8), ARMS03=0x40 (16),
  ARMS0E=0x40 (16), CORE00=0x40 (16).
- VH inside EDH = byte-identical to RDT snd0/snd1 → `re15_vab_parse(edh+pBAV_off, edh_sz−pBAV_off, &vab)`.
  ARMS02: ver 7, bank 0, prog1/tone6/vag5.
- **.VB = raw concatenated ADPCM body** (no per-VAG headers); sizes from the VH VAG-size-table (u16 words<<3).
  Sum == VB for ARMS01/02/07/09/0E/CORE00 (ARMS02: 34528 == ARMS02.VB). ⚠ ARMS00 has 6448B trailing pad
  (4 VAGs sum 29040, VB 35488) — the size-table→VB byte mapping is exact; "VB == VH fsize" is NOT universal.

**Assets already in `re15_port/shared_assets/PSX/SOUND`** (82 files, MD5-identical to `info/Re1.5/PSX/SOUND`). No copy step.

---

## 3. The gunshot

The **only** decoded muzzle SE-play: `FUN_80045024(0x01080001, &DAT_800aca88)` = **bank1 (ARMS),
SE index 8, positional** (`lui a0,0x108` + `ori a0,a0,0x1`). Two call sites, both `a1=&DAT_800aca88`:

1. `FUN_80011f50` (hit resolver) @0x800123d4 — gated `if ((param_1 & 0xff) == 1)` (`bne s0,1` @0x800123c4).
2. `FUN_80035538` (aim/fire sub-state, table 0x80074164 idx 4, dispatched by DAT_800aca5a) @0x800355f8 — in the DAT_800aca5b==0 fire-start branch.

`FUN_80011f50`'s `param_1` **is the equipped weapon id** `DAT_800aca5d` (all callers `lbu a0,DAT_800aca5d` before the jal).
Weapon-class dispatch @0x80074030 (idx DAT_800aca5d*4): weapons 0/1/2 → LAB_80034e70; 3..11,13,15,16,17,18 → LAB_80032e9c; 12,14,19 → LAB_80034014.

> ⚠ **BLOCKED — the pistol's actual shot SE index is UNVERIFIED.** Player pistol = **weapon 2**,
> which does NOT hit the `param_1==1` branch. Index 8 is out of ARMS02's 8-record range (idx 8 =
> the pBAV header = garbage), and case-1's only bound is ≤0x20 (no catch). The per-shot SE for
> weapon 2 is not in the decompiles. **Do NOT hardcode 8.** Resolve dynamically (DuckStation
> savestate: fire the pistol, watch the se_id passed to FUN_80035538 / FUN_80045024). This is the
> single gate on shipping the player gunshot byte-true.

Per-weapon-class fire slots (all bank1, positional, a1=&DAT_800aca88) in the un-ported aim/fire FSMs:
idx 1 (jals @0x8003338c/0x80034488/0x80034a0c), idx 3 (@0x80033ed0/0x80034e54), idx 5 (@0x8003537c), idx 8 (the two above).

Related fire-path SEs (not the muzzle): `FUN_80045024(0x02070000,...)` = **snd0** idx7 non-positional (FUN_80035538 case 5, when DAT_800acae9==1). Heavy-weapon **CORE** (bank4) fire SEs idx 0/1/2/3 (action FSM @0x800741a8).

---

## 4. Hit + zombie SEs (all `func_0x800453d0` on snd1 unless noted)

| SE id | meaning | site | port status |
|---|---|---|---|
| 4 | grab/lunge **start** | FUN_80102548 case 0 @0x8010268c; FUN_80102bd8 state-0 @0x80102c8c | **PORTED** (grab, 784240e7) |
| 3 | **bite-connect** | FUN_80102bd8 @0x80102cb0 (anim-frame +0x95==0x28; 3→15 remap if flag 0x2000) | lunge dormant for ROOM1140 |
| 7 | grab **release** | FUN_80102548 case 4 @0x80102920 + @0x80102960 (twice) | **PORTED** (grab, 784240e7) |
| 5 / 8 | **death groan** | FUN_80107cb0 @0x80107e00 (+0x95==7, se=(rng&1)?5:8) | **PORTED** (5e10a0ec) |
| 10 | bullet-**impact** / low-dmg grunt | FUN_80012d60 @0x80012e70 + @0x80012f74 (gated (param3&0xff)<2, after HP subtract) | resolver path, not the grab's direct HP drain → no live ROOM1140 trigger |
| 6 | **special pose-collapse** stagger impact | FUN_80106a38 @0x80106b0c (gated (rng&7)==0); FUN_801068a0 @0x8010695c | dormant for ROOM1140 |

> **SE 6 is NOT "the SE when a bullet lands"** (finder claim refuted). `FUN_80105a8c` (hurt dispatch
> @0x8011f7b4[2]) routes on entity+9&0x80: **normal hit** (clear) → stagger table @0x8011fb90 →
> handler 0x80105b7c, **no SE6**. FUN_801068a0/FUN_80106a38 (SE6) are the +9&0x80 special-collapse/
> grab-stun sub-branch (split by +5∈{0x12,0x13}), **dormant for ROOM1140**.

**Gore spawn plays NO SE**: `func_0x80019700` (effect-sprite pool @0x800a73b8, 0x84-byte records) is a
leaf — zero jal in its body (0x80019700-0x800199cc). The grab's bite (case 3) spawns blood via
`func_0x80019700(0x1500,...)` (effect-0) but the SE and gore are separate calls.

Scripted first-zombie death cutscene `FUN_8010a6f8` (driven by DAT_800aca5a): `0x04010001` (CORE idx1)
init, `0x02070001` (**snd0** idx7) at frame '#'(0x23), `0x04030001` (CORE idx3) at frame '7'(0x37).

---

## 5. Port plan

### PORTABLE NOW (byte-true buildable)

- **snd1 zombie combat SEs** — DONE: grab-start 4 + release 7 (784240e7), death groan 5/8 (5e10a0ec),
  all via the existing `re15_audio_room_se` + `re15_footstep_vag`.
- **Resident weapon-SE bank (bank1)** loader — buildable (assets present, format known):
  - `load_weapon_se_vab_pc(weapon)`: read `SOUND/ARMS%02X.EDH`; `pBAV_off = read_u32_le(edh+edh_sz-8)`;
    `re15_vab_parse(edh+pBAV_off, edh_sz-pBAV_off, &s_weap_vab)`; decode VAGs from `ARMS%02X.VB`;
    keep `s_weap_edt = edh` (EDT prefix @0, size = pBAV_off).
  - `re15_audio_weapon_se(se_id)`: guard se_id∈[0,0x20]; `vag = re15_footstep_vag(s_weap_edt, &s_weap_vab, se_id)`;
    reuse the `re15_audio_room_se` mixer block.
  - Prime at room-load (parity with FUN_80043d8c from FUN_800314b0) with the equipped weapon; reload on change.

### BLOCKED

1. **Which SE index the pistol (weapon 2) fires** — unproven; needs a DuckStation dynamic check (§3). The gate on the gunshot.
2. **id 0x77 → ARMS00 filename** — inferred, not byte-proven; confirm via CD ISO dir / runtime LBA.
3. **The aim/fire command FSM** (DAT_800aca59 tables @0x80073fb0/ff0; sub-switch DAT_800aca5a @0x80010b68;
   trigger latch DAT_800acae9; weapon-class dispatch @0x80074030) — not ported. Needed to fire the SE at the exact frame.
4. **bank0 resident SE blob** (DAT_801a1000) — boot-resident region, not a named SOUND/ file.
5. **bank4 CORE loader** (FUN_800440c4 → &DAT_801fbd00) — for heavy-weapon + scripted-death CORE SEs.
   (CORE00.**ESP** in the port is a different visual-fx subsystem — does not satisfy this.)
6. **byte0 VAB-ID override + byte3 extra-tone layering + SsUtKeyOnV note/fine/pan** — beyond the current
   faithful-line mixer (same deferral the snd1 path already makes).
