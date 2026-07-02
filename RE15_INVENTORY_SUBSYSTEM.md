# RE1.5 Inventory / Weapon-Select Subsystem — byte-true reference + phased port plan

From the `inventory-weapon-select-re` + `inventory-data-tables-re` workflows (adversarial verify).
Tags: **[BT]** byte-true (disasm/asset/savestate proven), **[FL]** faithful-line interim, **[UNVERIFIED]**.
Constants cite `ghidra1_V2.txt` / a decompile file / a file byte-offset / a savestate.

> **CORRECTIONS (2026-07-02, inventory-data-tables-re workflow, 13 confirmed / 7 corrected):**
> - **Item CLASSIFICATION is an id-RANGE, not a type byte** [BT]: WEAPON = `id < 0x15` (0x00..0x14,
>   `sltiu id,0x15` @0x80047d54; the ARMS-head table @0x8007492c is non-zero exactly for 0..0x14).
>   AMMO = 0x15..0x21. KEY/heal = id ≥ 0x22 (max_stack 1). Port: `re15_item_is_weapon/ammo/key`.
> - **Briefing items** [BT]: id 1 = **COMBAT KNIFE** (the *equipped* weapon, DAT_800aca5d==1), id 3 =
>   **BROWNING HP** handgun (15-round clip), id 0x15 = **H. GUN BULLETS** (ammo). (Earlier "id 1 = handgun"
>   was WRONG.) The equipped briefing weapon is the KNIFE, not a gun.
> - **The item-attribute table IS located**: EXE `DAT_80074da8` (stride 12: [0]=max_stack, [4]=recipe_ptr,
>   [8]=icon-CLUT selector, [9]=recipe_count) — NOT "unlocated in DEBUG.BIN @0x800c0000" (that note is superseded).
> - **Icons = multi-sub-sprite COMPOSITE** via descriptor table `DAT_80074a8c` (stride 12: {clut_idx,
>   sprite_count, template_ptr, vram_dest}, ids 0x00..0x18), each sub-sprite a 12-byte template {tx,ty,w,h,
>   U@8,V@10}. ITEMALL tpage 0x22a = VRAM(640,0) 4-bit. NOT a single id→cell tile.
> - **Full item catalog 0x00..0x21** (names from DAT_800c4a28 blob) is embedded in the port (`re15_item_name`).

Companion to `RE15_COMBAT_SE_SUBSYSTEM.md`. Port status in memory `reai-v2-foundation-combat`.

---

## 1. Open / close + state

- **Open button** [BT]: PSX **START** (`PADstart = 1<<11 = 0x0800`, `LIBETC.H:29`). State-1 poll
  `@0x8001cd64` reads the raw newly-pressed edge `DAT_800ac762 & 0x800`. **Not SELECT** (0x100 opens
  the debug menu `FUN_8001443c`). The port abstracts this as `pad_pressed & RE15_PAD_BIT_START` (0x0008,
  its own bit convention); PC binds **I** → START (`input_pc.c`).
- **Pad edges** [BT, verdict-corrected]: `FUN_80030444` (`ghidra1_V2.txt:124172`) computes TWO edges —
  the RAW edge → `DAT_800ac75c` (low16 `DAT_800ac762`), and the PROCESSED/bit-remapped edge (via
  `PTR_DAT_80073e1c`) → `DAT_800ac76c` (u32). Held pad low16 → `DAT_800ac760`. START-open reads the raw
  edge `DAT_800ac762 & 0x800`; menu-confirm reads the processed edge `DAT_800ac76c & 0x4000`. The port's
  `pad_pressed` (edge) / `pad_current` (held) model this correctly.
- **Main state machine** [BT]: entry `0x8001c958`, dispatched by game-state **byte `DAT_800b5359`** via
  table `PTR_LAB_8001069c`, index = state−1, range 1..5 (state 1 = gameplay). Open = set
  `DAT_800aca3c |= 0x8000` (`@0x8001cd64`, gated by `DAT_800acae7 == 0` "menu allowed") → fade
  (`DAT_800b5359 = 2`) → install the menu task handler `FUN_80029a98(1, 0x8002dde4)` (`@0x8001cb00`).
  Close = states 3/4/5 (fade-back + restore `DAT_800aca40`), fade-gated by `FUN_8002178c`.
- **PAUSE** [FL, verdict-OVERTURNED]: the finder's "PsyQ cooperative-thread switch" pause was
  **refuted** — the scheduler (`FUN_800298b0`/`ChangeTh`) exists and is byte-true, but its attribution
  to the inventory is unproven (the inventory task lives in overlay space absent from the EXE dump). The
  **port implements pause as an inline game_step gate** (skip player_tick+collision+fire while the menu
  flag is set), mirroring the verified stair/dead/grabbed skip pattern — functionally equivalent for the
  single-threaded PC port.

## 2. Weapon-equip + item model

- **Equipped-weapon global** [BT]: `DAT_800aca5d` (byte, range **0..21**). Indexes both the reach table
  `0x8006e5a0` (u32[22]) and the zombie-damage table `0x8006e0d0` (`+weapon*4 +type*0x58`, u16). Weapon 0
  = unarmed (0dmg), **weapon 1 = handgun (6dmg/1100reach, ARMS01)**, weapon 2 = 24/1000.
- **Room-init default** [BT getter; UNVERIFIED one branch]: `FUN_800314b0` sets
  `DAT_800aca5d = FUN_800c00a8()`. `FUN_800c00a8` is `??` in the EXE (DEBUG.BIN overlay @0x800C0000);
  disassembled from savestate RAM it is a **per-character static table read** `table[DAT_800aca5c]`
  @`0x800c00d4` (`01×15, 00` → char 0..14 → weapon 1, char 15 → 0), **NOT inventory-derived** (the port
  doc that said "from the inventory's equipped slot" was WRONG — fixed). The `==0xf → 0` fallback
  (`FUN_800314b0:43`) is [UNVERIFIED]: Ghidra emitted `extraout_v1` (a post-`FUN_800c00a8` register), and
  the callee is undisassemblable, so whether the 0xf gate is on the char-id or a `FUN_800c00a8` return
  cannot be proven. Immaterial: char 0 → 1, every pristine STAGE1 save reads 1.
- **Live inventory** [BT, verdict-corrected]: `DAT_800b10ac` (53 xrefs), 4-byte stride `{id, qty,
  flags/mode, pad}` (`@0x8004ded4 sll v0,v0,0x2`), written by `FUN_8004dc4c`/`FUN_8004a1f0`/`FUN_8004e214`.
  Layout = RE2 `ITEM_WORK {u8 Id;Num;Size;dummy} × 11` (`BioRdt-master/global.h:783,1716`). ⚠ The port's
  old `DAT_800d4a3c` / `FUN_8006947c` citations were WRONG (`DAT_800d4a3c` = uninitialised 0xFF;
  `FUN_8006947c` = sys.c GPU code) — corrected; the 11-slot count + layout are right.
- **Equip action** [BT]: in the menu handler `FUN_800460b8`, equip-commit `@0x80046654`:
  `v1 = DAT_800b25c8` (cursor); if cursor==0x80 → weapon 1, else `v0 = DAT_800b10ac[cursor*4]` (the slot
  id); `sb v0 → DAT_800aca5d` (`@0x80046688`, raw id, no translation); then re-prime the ARMS bank
  `FUN_80043d8c(DAT_800aca5d,...)` (`@0x800466c4`). Same-item no-op guard `@0x8004661c` (skip if id ==
  `DAT_800b25ce`). Dynamic proof: `equip_test.sav` {cursor=1, slot1.id=3, DAT_800aca5d=3}. → In the equip
  path **item-id == weapon-id** [BT-for-this-path]; the general item→weapon map (room-init) is the
  undisassemblable `FUN_800c00a8` [FL].
- **ARMS bank load** [BT]: `FUN_80043d8c(weapon,...)` — head id `DAT_8007492c[w*2]` (0x77+2w), body id
  `DAT_80074958[w*2]` (0x78+2w), via numeric CD resource `FUN_80013b60` → `SsVabOpenHeadSticky`/
  `SsVabTransBody`. No `ARMS%02X` string; the disc `SOUND/ARMS*.EDH/.VB` ARE those numeric resources.
  (See RE15_COMBAT_SE_SUBSYSTEM.md.)

## 3. Screen (Phase 2 reference — mostly byte-true, some verdict-overturned)

- **Dispatch** [BT]: `FUN_800460b8` runs a draw pass (table `0x80074be8[3]` = `0x80049524` normal /
  `0x8004c54c` item-box / `0x8004d7cc` check) + a logic pass (table `0x80074bf4[3]` = `0x8004974c` /
  `0x8004cca4` / `0x8004d904`), indexed by sub-state `DAT_800b25c0`. Core renderer `FUN_80049a5c`.
- **Grid** [BT]: cell offsets `DAT_80076274` (u16 dx,dy) first 10 = (4,32)(44,32)(4,62)…(44,152) = **2
  cols {X 4,44}, 5 rows {Y 32,62,92,122,152}**, col pitch 40, row pitch 30. Panel origin
  `DAT_800b25e0=215, DAT_800b25e2=26`.
- **Cursor** [BT]: grid cursor `DAT_800b25bd`, display cursor `DAT_800b25c8` (**0x80 = no-selection**).
  Nav `FUN_800487b0` from held pad, PSYQ PADL* bits: **Right(0x2000) +1 if even & <count−1; Left(0x8000)
  −1 if odd & >0; Down(0x4000) +2 if not last row; Up(0x1000) −2 if ≥2**, NO wrap (2-wide column-major).
  ⚠ The port's d-pad bits differ (UP 0x10…) → map to the ±1-parity/±2 semantics, not the raw bits.
- **Icons** [asset BT; render OVERTURNED]: `ITEMALL.PIX` = 86400 B raw 8-bit indexed. ⚠ The finder's
  "id × 1200 linear tiling" was **refuted** — the renderer selects icon VRAM u/v from lookup tables
  (`DAT_80076244`/`0x80076274` family), id bounded `sltiu 0x22` (0..33). Phase-2 icons need the VRAM
  atlas u/v tables, NOT a linear file offset.
- **Sub-actions** [BT cursor; mechanism OVERTURNED]: top-menu cursor `DAT_800b25bc` (0 CHECK/1 MANAGE/2
  EXIT/3 TAKE/4 USE). ⚠ `FUN_8004974c` confirm does NOT use the `0x8001126c` BROWSE table (that's a
  different fn `FUN_8004cdc4`); it uses an if/else chain + table `0x80074c0c`. Quantity digits [BT]:
  `FUN_80048f28` reciprocal-multiply (`/100` 0x51eb851f, `/10` 0xcccccccd), glyph = value×8.

## 4. Port plan

### PHASE 1 — minimal playable/testable weapon-select — ✅ DONE (Phase 8.20)

- **`re15_menu.h` + `menu_common.c`** (engine LOGIC, headless-testable): `re15_menu_toggle` (START edge),
  `re15_menu_tick` (UP/DOWN cursor, SQUARE=equip+close, CROSS=cancel; START owned by toggle),
  `re15_menu_is_open` + getters (count/cursor/entry_name/entry_weapon). EQUIP calls
  `re15_player_set_equipped_weapon(w)` + `re15_audio_prime_weapon(w)` — the byte-true equip order
  (`@0x80046688` + `@0x800466c4`). **[FL]** menu owned-set = a fixed test list {1 HANDGUN, 2, 3} (kept
  for multi-weapon testing; the byte-true briefing owns only the handgun as a weapon).
- **Byte-true initial inventory** (Phase 8.20b): `re15_inv_load_briefing()` (inventory_common.c) populates
  `g_inv` with the savestate-confirmed briefing loadout (`DAT_800b10ac`: item 0x01 qty 0 = HANDGUN, item
  0x03 qty 15, item 0x15 qty 50); wired at boot (main.c, after scd_vm_init). `re15_item_is_weapon(id)`
  byte-true-recognizes the handgun (id 1). Test (0) in test_weapon_select. NOTE: per-room persistence
  across `room_unload -> scd_vm_init` (which clears g_inv) is a separate pre-existing concern.
- **Item CLASSIFICATION status** (the Phase-2b bottleneck): the equip-commit @0x80046688 stores the raw
  slot id (no is-weapon gate THERE), so the weapon/ammo/key classification lives in an item-attribute
  table. Located in the DEBUG.BIN **item module @0x800c0000** (resident in RAM, capstone-disassemblable):
  the char-default table @0x800c00d4 (`01×15,00`) is decoded (FUN_800c00a8), a second indexed table
  @0x800c5ea0 was found, but the item-id→type table is not yet pinned (needs a dedicated disasm of the
  item add/use/equip functions + the inventory sub-action FSM FUN_8004cdc4). Ways already exhausted:
  savestate inventory bytes, EXE equip-path disasm, RE2-Leon, RE15Editor, the info/ refs, module capstone scan.
- **`game_step_common.c`**: menu branch prepended to the stair/dead/grabbed chain — START toggles; while
  open, tick + keep the RVD cam scan + clear the action edge + SKIP player_tick/collision/fire (inline
  pause, §1). 1170-safe (only entered when the menu opens).
- **`input_pc.c`**: `I` → `RE15_PAD_START` (0x0008). **`audio_pc.c`**: `re15_audio_prime_weapon` (public
  re-prime = `load_weapon_se_vab_pc`); PSX + test stubs. **`main.c`**: overlay render (panel + text list +
  `>` cursor; **[FL]** presentation).
- **Test** `test_weapon_select.c` (unit_weapon_select): open snaps to equipped, START toggle-only, nav
  clamps, SQUARE equips+closes, CROSS cancels. **34/34 ctest.**

### PHASE 2 — byte-true grid + icons + sub-actions (deferred)

Port the real FSM (`FUN_800460b8` draw/logic passes §3.1), the byte-true grid `DAT_80076274` (§3),
cursor `DAT_800b25bd`/`DAT_800b25c8` with the ±1-parity/±2 nav + 0x80 sentinel, quantity render. Icons:
load `ITEMALL.PIX` + derive id→cell u/v from the game's VRAM atlas tables (NOT id×1200 — overturned;
needs an `ITEMALL` VRAM savestate dump). Owned-weapons: scan `g_inv` for weapon-class ids. Sub-actions
via `FUN_8004cdc4`/`0x8001126c`. `FUN_800c00a8` (inventory→weapon default) needs a **DEBUG.BIN @0x800C0000
disassembly** (re15-psx-disasm) — deferred; Phase 1/2 use the dynamic default 1 + the direct
equip-commit formula, both byte-true.
