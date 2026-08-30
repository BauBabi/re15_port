# Item-Box build sheet — RE2 mechanics spec + RE1.5-hybrid mapping
(RE agent 2026-07-21; raw-MIPS-verified against ghidra_re2_Leon.txt; decompiles cross-checked.
All RE2 addresses = RE2-Leon retail EXE; RE1.5 addresses = RE1.5 PSX.EXE.)

## 0. CORRECTION of the campaign's function list (load-bearing)
- **FUN_80068f9c is NOT the box** — it is the Ada/Sherry sub-scenario inventory swap (param 0xe/0xf
  selects preset page 0/1 from .data table DAT_800a93d0; backs the 11 live slots into Bk_item_work
  @0x800ce5e4; forces capacity DAT_800d46ac := 8 because SUB-CHARACTERS have 8 slots — that is the
  "count-forced-8", not a box window; restore copies back 10 slots + re-derives the personal item
  {0x2f lighter | 0x30 lockpick} by param&1 @0x800691c0).
- FUN_80069668 = inventory FREE-SLOT scanner (param 0: first id==0 else 0xffffffff; param 1: count;
  bound = DAT_800d46ac). FUN_80069714 = inventory COMPACTION (shift-down from first empty, adjusts
  equip slot DAT_800d5bf8 + DAT_800d69f4; special case first-empty==9). Both are inventory utilities
  the box CALLS.
- **DAT_800d4a68 IS the box storage** (the earlier "box-screen state" claim was wrong).

## 1. Box storage
- **DAT_800d4a68, 64 entries × 4B = 0x100** (0x800d4a68..0x800d4b67). Entry = ITEM_WORK
  {u8 Id, u8 Num, u8 Size, u8 dummy} (BioRdt global.h:783-788; binary access byte-wise +0/+1/+2).
- Capacity 64 proven twice: every box index masked `andi 0x3f` (@0x80070440 transfer, @0x8006ff4c/
  0x8007005c/0x80070204 FSM, @0x8006fa5c init, draws FUN_800710dc/FUN_80072c6c); next byte
  0x800d4b68 = File_tag_bk[24] (save-keyboard glyph grid, FUN_800724b4).
- Sits after the 11-slot inventory DAT_800d4a3c (+0x2c) inside SAVE_DATA @0x800d4830 (0x36c bytes;
  box at +0x238; member addresses cross-checked: Scenario_flg 0x800d4834 ✓ @0x80068aec,
  Item_flg 0x800d48bc ✓ side-pack flag 0x3f @0x80068ad0).
- Empty slot = Id 0 (draw substitutes name id 0x64 "empty"). Box starts empty (new-game zero region).
- Inventory capacity DAT_800d46ac: 8 default, 10 with side pack (flag 0x3f → `sb 0xa` @0x80068ae4);
  slot 10 = personal item, auto-refreshed (flag 0xa2 → lighter 0x2f @0x80068b00).

## 2. Box screen + FSM (raw MIPS)
### 2.1 Task + dispatch
- Subscreen open: `DAT_800d5c00 := screen_id; DAT_800df348 := 1; DAT_800cfbd8 |= 0x8000`.
  **Box = screen id 1**, written at ONE site @0x80051dd0-0x80051e08. Task LAB_800689bc (installed
  from 0x80025b90): loads UI, re-derives capacity/personal slot, zeroes FSM struct 0x800d5bf0..f7
  @0x80068be8-fc + cursor DAT_800d5bfc/fd, snapshots equip (5bf9:=slot, 5bfb:=id @0x80068c38-44),
  **compaction FUN_80069714 @0x80068c60**, loop `jalr [PTR_LAB_800a936c + 5c00*12 + 5bf0*4]`
  @0x80068c70-9c. Screen table @0x800a936c: [1] = box = {init LAB_8006f900, run LAB_8006fb64,
  close LAB_80068cd4} (@0x800a9378/7c/80).
### 2.2 Init LAB_8006f900
- Loads CD file 0xa9 "ITEM_ALL" (string @0x80011d34) to 0x8019c000 (icon bank, 0x4b0/tile; 2nd-cell
  art of wide weapons at tile id+0x56). Phase:=1; panel DAT_800d5bf1 := 3; scroll DAT_800d5c14:=0 +
  pixel DAT_800d5c15:=0; prim build FUN_80070e58 (panel base 5c40=7, 5c42=0xe); background variant
  DAT_800d5c04 = from the box AOT halfword +0xe (@0x80051c58); preloads icon cache slot 7 :=
  box[scroll], rows box[scroll+0..4] → cache 10/13/16/19/22; fade-in FUN_8002c1a0(0x200,-0x1800,7,1).
  Inventory cursor starts 0.
### 2.3 Run LAB_8006fb64 + panels @0x800a9bb4
- [0] exit start (fade-out), [1] exit wait (→ phase 2 close), [2] EXIT row (repeat-DOWN 0x4000 →
  cursor:=1 panel:=3 SE 0x404; action &0x3000 → panel:=0 SE 0x406), [3] main FSM LAB_8006fc58.
### 2.4 Main FSM — sub-state DAT_800d5bf2 (0..5, jump table @0x80011d44)
- Pad: movement = DAT_800ce304 (0x1000 up/0x2000 right/0x4000 down/0x8000 left, 0x4/0x8 shoulder);
  actions = DAT_800ce310 edge word (0x1000 confirm-role, 0x2000 cancel-role, 0x4000 third). Gated
  on DAT_800cfb74 < 0 (no message).
- **State 0 inventory grid**: linear cursor over capacity; right +1 (skip 2 if Size==1 head), left −1
  (skip tail; blocked at 0 and 0xa), down +2 (personal 0xa → 1), up −2 (cursor<2 or ==0xa → EXIT
  row); confirm → state 1 (box list) SE 0x406; cancel → panel 0 exit SE 0x405; third (gated 5c06==1)
  → panel 1 instant exit.
- **State 1 box list**: selection FIXED at middle row, LIST scrolls: selected = (scroll+2)&0x3f.
  Repeat-up → state 2 (preload box[(scroll−1)&0x3f] into cache 7); repeat-down → state 3 (preload
  scroll+5); 0x4 → scroll−5, 0x8 → scroll+5 instant (commit tail @0x8007005c). Cancel → state 0;
  confirm → state 4 (swap).
- **States 2/3 scroll anim**: 6 frames ±3px (row pitch 0x14=20px → 2px snap QUIRK); 5c06:=0 during
  (blocks the third-action exit); incoming row name at offset +0x9/+0x81; at ≥6 held direction →
  auto-repeat commit ±1 SE 0x214, else state 1.
- **State 4 swap**: state:=0, call FUN_800703b8 (may set state 5 on reject).
- **State 5 message wait**: until DAT_800e873c bit 0x80 clears → state 1.
- **Common tail**: name caption at (0x10,0xaf) FUN_800693d0(…,6,inv[cursor].id); pulsing cursor
  frame FUN_80073350(Size) (wide when Size≠0).
### 2.5 Draws
- FUN_800710dc: 16 bg tiles (UNK_800a9c38/0x800a9c28), EXIT highlight (0x80 when panel==2 else 0x40,
  pos DAT_800a9c98, CLUT (0x100,0x1f0)), button hints, 5 box rows pitch 0x14 + pixel offset, row
  highlight wide(0x30)/bright when Size≠0 else narrow(0x18)/dim, names via FUN_800693d0 (0x64 if
  empty), wrap separator when (scroll+2)&0x3f < 5 || == 0x3f at y=(0x3f−((scroll−4)&0x3f))*0x14+7,
  cursor pulse DAT_800d4cd0 ping-pong ±2.
- FUN_80072c6c: qty digits (skip Size==2 tails), equip qty, box-mode 5 rows + incoming; digit color
  from property table DAT_800a9e1c stride 8 (+0 = MAX STACK, +2 = color code, sign → no-count).

## 3. Transfer engine FUN_800703b8 — ONE operation: SWAP inv[cursor] ⇄ box[(scroll+2)&0x3f]
(no deposit/withdraw commands — the "0x0e/0x0f" belonged to FUN_80068f9c, §0)
- **Ammo pre-pass** (box id ∈ 0x14..0x1f @0x80070490-94, matching inv id): cap = DAT_800a9e1c[id*8]
  @0x800704c4; (a) inv qty == cap → **quantities SWAP** (inv:=boxqty, box:=cap) @0x800704e0-f4
  QUIRK; (b) sum>cap → inv:=cap, box:=sum−cap; (c) sum≤cap → inv+=box, box zeroed. Empty cursor
  slot: scan ALL inventory for same ammo id, merge only if the WHOLE box stack fits @0x80070554-98.
- **case 0 (1c⇄1c/empty)**: unequip if cursor==equip; destination = min(cursor, first-free)
  (FUN_80069668(0)); swap; compact after.
- **case 1 (box holds 2-cell, Size==3)**: THE ONLY REJECT: empties=FUN_80069668(1); reject if
  empties==0 || (empties==1 && cursor empty) → FUN_8002fe38(0xaf0010,0xe400,8,0) + state 5
  @0x800707dc-ec. Else unequip-if-needed, clear cursor, compact, front-shift-by-2 FUN_800698b4(1)
  (@0x80070830; equip index += 2), weapon into slots 0+1 (Size 1/2), old item into box.
- **case 2 (inv 2-cell out)**: clear BOTH cells, **compaction TWICE** @0x800709bc+0x80070a24/a38,
  weapon into box with **Size:=3** (`li v0,3; sb → DAT_800d4a6a[sel*4]` @0x80070ad0-dc raw), box
  item into first-free.
- **case 3 (2c⇄2c)**: in-place; box weapon → both inv cells, inv weapon → box Size:=3
  (@0x80070c30-3c/@0x80070d98).
- Equipped deposit = SILENT auto-unequip in every branch (equip:=0x80, weapon id:=0).
- **No box-full reject exists** (64 fixed slots, 1:1 swap).

## 4. Close + persistence
- Close LAB_80068cd4: FUN_80069eec(1); rooms 0x32/0x4b/0x43 force-unequip (stash DAT_800d69f4/f5);
  if box screen: weapon-model reload FUN_80069210+FUN_80041060+FUN_800408c8 @0x80068d44-70; equip-
  changed → weapon halfword DAT_800cfd06 rebuilt (model byte DAT_800d4c3c[equip*4] + FUN_80059c74);
  5c00:=0; ammo-HUD flag from DAT_800d4c3d[equip*4].
- Persistence: box inside SAVE_DATA (+0x238); EXE has NO serializer touching it (23+1 xrefs = box
  screen only); card code = COMMON/BIN/MEM_CARD.BIN (in-repo 0x5d7c B; ink consume @0x1188-11e4,
  inventory memcpy 0x2c @0x1180, header @0x3838-394c, load scatter @0x3b20-3bac; bulk SAVE_DATA
  gather not pinned — OPEN, non-blocking: the hybrid persists via the port's save block).

## 5. Trigger: AOT sce-type 0x0a → lid anim → screen 1
- Dispatcher FUN_80051088 jalr [PTR_LAB_800a73c4 + sce*4] @0x80051400-1c; entry [10] @0x800a73ec =
  LAB_80051c20 = the box handler. Arm: AOT +0xc → lid entity id (DAT_800d423b), +0xe → bg variant
  (DAT_800d5c04); continuation LAB_80051c64: phase 0 freeze + SE 0x215xxxx; phase 1 lid rotate
  +0x78 accelerating (start 1, +3/f) until ≥0x190 @0x80051d30-8c; phase 2 decelerate → 5c00:=1,
  df348:=1, cfbd8|=0x8000; phase 3 post-close re-close (not fully walked).

## 6. RE1.5-HYBRID mapping (DESIGN — flagged per part) + DECISIONS
- **Mechanics [RE2-ported]**: swap model §3 verbatim incl. ALL quirks; FSM states 0-5 + panels;
  compaction-on-open; ammo pre-pass with RE1.5's own caps (prop table @0x80074da8 +0 — already
  ported); 2-cell mapping: RE1.5 kind byte (settled in shots/inv_wave2_spec.md fact "(2) Slot +2
  byte — DEFINITIVE semantics") ↔ RE2 Size; boxed-wide = Size 3 analog.
- **Storage [RE1.5-dormant] — DECISION (a): 32-slot box** in the dormant arrays @0x800b1444/1484/
  14a4/14c4 as 4 logical pages × 8 (mask 0x1f; the 0x20 un-zeroed gap 0x800b1464-1484 means the
  arrays are NOT contiguous — the port models them as 4 discrete pages, exactly the dormant shape).
  Flagged: arrays' original purpose undecidable; use is RE1.5-plausible, not proven.
- **Persistence**: authoritative = re15_savedata_t v4 box field (the port's RE2-ported save system);
  the dormant-array shape is the in-RAM model.
- **UI [design, non-canonical flagged] — DECISION (ii)**: 4 pages × 8 icon cells in the RE1.5
  status-screen style (reuse the ITEM-LIST grid geometry/chrome/icon pipeline), L1/R1 = page flip
  (≙ RE2's ±5 keys), RE1.5 SEs bank 4 {move 4, cancel 5, confirm 6}; name caption via
  re15_item_name; reject = the RE1.5 cant-use message infra.
- **Trigger [invented, precedent = save phone]**: the 16 safe-room box AOTs open the box (default-on
  like the functional save); RE2's lid pre-anim skipped (no RE1.5 asset — open with freeze+fade like
  the save/door patterns). The shipped message behavior stays available under a test/dev flag and
  the existing ctest is split accordingly.

## 7. Quirks catalog (must-preserve)
1. Ammo qty-SWAP when inventory stack full. 2. Ammo always merges box→inventory even on deposit
intent. 3. Empty-cursor ammo withdraw redirects into any fitting existing stack, whole-stack-only.
4. Withdraw lands at min(cursor, first-free). 5. 2-cell withdraw → slots 0+1 via front-shift (equip
+2); reject only when empties==0 || (==1 && cursor empty). 6. Boxed 2-cell = ONE slot, Size 3, wide
bright row. 7. Deposit-2-cell compacts TWICE. 8. Equipped deposit silently unequips. 9. Compaction
at screen open. 10. Selection fixed at scroll+2, 64-ring wrap (hybrid: 32-ring per §6), separator at
the ring boundary. 11. Scroll anim 6×3px vs 20px rows (2px snap); held auto-repeat; page keys ±5
(hybrid: page flip). 12. Personal slot 0xa nav special cases. 13. Empty rows = name id 0x64 +
prop-colored zero qty. 14. Cursor starts 0; equip snapshot drives close-time weapon-model reload.

## Open questions (non-blocking)
1. Physical buttons behind RE2 action bits (post-remap; port maps ROLES onto the RE1.5 virtual word
   @0x80073dbc — confirm=SQUARE etc.). 2. RE2 new-game zero-clear instruction for SAVE_DATA.
3. MEM_CARD.BIN bulk gather + overlay base. 4. Panel 0/1 FUN_8002c350 no-fade semantics.
5. (settled — see inv_wave2_spec.md) 6. Lid phase 3.
