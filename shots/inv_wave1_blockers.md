# Wave-1 blocker resolutions (RE workflow wf_b906bb86, live-verified vs mzd_inv_open.sav)

---
## Q1: Item-quantity digit COLOR rule in the RE1.5 inventory list drawer FUN_80048f28: where does the green-vs-cyan RGB of the qty digit prims come from (a3 param? prop table @0x80074DA8? equipped-slot compa

RESOLVED: True

### Answer
RESOLVED. The digit prims carry NO per-item RGB at all — their RGB bytes stay at the template's neutral (128,128,128) and are never written by FUN_80048f28. The color is carried entirely by the per-prim CLUT, and the CLUT index is the per-item property byte at prop-table +8:

RULE: digit_clut = DAT_800b2610[2 + prop8], where prop8 = u8 @ (0x80074da8 + item_id*12 + 8). The caller FUN_80049a5c loads this byte (address 0x80074db0 = 0x80074da8+8, indexed by id*12) and passes it as a2 to FUN_80048f28; FUN_80048f28 stores DAT_800b2610[2+a2] (loaded from 0x800b2614 + a2*2; 0x800b2614 = DAT_800b2610 element [2]) into the SPRT clut field (prim+14). The a3 parameter is only grid (0) vs ARMS-equip-panel (1) position mode; the equipped-slot compare plays no role in color.

prop8 is a 4-value color class → CLUT (all clut ids from the live DAT_800b2610 array = {0x7a10,0x7a50,0x7a90,0x7ad0,0x7b10,0x7b50,0x7b90,0x7bd0} = VRAM x=256, y=488..495; glyph body = CLUT entry 1, decoded from savestate VRAM):
- class 0 → clut 0x7a90, entry1 = (0,128,0) GREEN — ids 0x00-0x02 (knives), 0x15-0x17 (ammo), 0x19, 0x1c-0x1e
- class 1 → clut 0x7ad0, entry1 = (160,144,0) YELLOW — ids 0x0a, 0x10, 0x1a, 0x1f, 0x21
- class 2 → clut 0x7b10, entry1 = (176,80,0) ORANGE — ids 0x0b, 0x0e, 0x11, 0x18, 0x1b, 0x20
- class 3 → clut 0x7b50, entry1 = (0,112,184) CYAN — ids 0x03-0x09, 0x0c, 0x0d, 0x0f, 0x12-0x14 (magazine weapons)

Full prop8 sweep for ids 0x00-0x21 (PSX.EXE bytes @0x80074db0+id*12): 00:0 01:0 02:0 03:3 04:3 05:3 06:3 07:3 08:3 09:3 0a:1 0b:2 0c:3 0d:3 0e:2 0f:3 10:1 11:2 12:3 13:3 14:3 15:0 16:0 17:0 18:2 19:0 1a:1 1b:2 1c:0 1d:0 1e:0 1f:1 20:2 21:1.

Digit glyph geometry (also byte-confirmed): SPRT code 0x66 (SPRT|ABE), w=h=8, u = digit*8, v = 0xf0 (240), x advance +6/digit, leading-zero suppression over 3 digits (hundreds/tens/ones via 0x51eb851f and 0xcccccccd magic-divides).

CROSS-CHECK against all three ground-truth colors (savestate slots @0x800b10ac = 01 00 / 03 0f / 15 32):
- knife id 0x01, prop8=0 → clut 0x7a90 GREEN. Live prim2 @0x80199800: xy=(222,78) uv=(0,240) clut=0x7a90 rgb=(128,128,128). MATCHES '0' green at (223,78).
- Browning HP id 0x03, prop8=3 → clut 0x7b50 CYAN. Live prims 4/5: xy=(262,78)/(268,78) uv=(8,240)/(40,240) clut=0x7b50. MATCHES '15' cyan at (264,78).
- H.gun bullets id 0x15, prop8=0 → clut 0x7a90 GREEN. Live prims 7/8: xy=(222,108)/(228,108) uv=(40,240)/(0,240) clut=0x7a90. MATCHES '50' green at (223,110).
Equipped-marker hypothesis refuted as predicted: equipped knife (slot 0) is green, non-equipped Browning is cyan; the equipped item's digits are simply drawn a second time (second call site, a3=1) with the SAME color rule at the ARMS-panel coords (DAT_800b25dc/de).

Port implication for wave 1: give each digit SPRT neutral rgb 128,128,128 and select its palette as DAT_800b2610[2 + prop8(id)]; ship the prop+8 column (it is inside the already-embedded blob [0x80074A8C,0x800762A0) in re15_inv_ui_tables.c — 0x80074da8+8 falls in range) and the four CLUT rows from ST_00.TIM's CLUT block at VRAM (256,490/491/492/493).

### Mechanism
Caller FUN_80049a5c grid loop: @0x80049ae0-aec lbu id from 0x800b10ac+slot*4; @0x80049af4-afc id*12 via sll v0,v1,1 / addu v0,v0,v1 / sll v0,v0,2; @0x80049b00-b0c lui at,0x8007 / addiu at,at,19888 (=0x80074db0 = prop table 0x80074da8 + 8) / addu at,at,v0 / lbu a2,0(at); @0x80049b10 jal 0x80048f28 (a0=prim ptr, a1=slot, a2=prop8, a3=0). Second call site for the equipped slot: @0x80049b34 lbu a1,(0x800b25c8); same a2 = lbu @0x80074db0+id*12 @0x80049b6c; @0x80049b70 jal 0x80048f28 with a3=1 (@0x80049b74 ori a3,zero,0x1). Inside FUN_80048f28: @0x80048f6c sb a2,24(sp); per digit: @0x80049214-20 ori v0,zero,0xf0 / sb v0,-1(s0) → prim+13 = v=0xf0; @0x80049224-28 sll v1,v1,3 / sb v1,-2(s0) → prim+12 = u=digit*8; @0x8004922c-244 lbu v0,24(sp) / sll v0,v0,1 / lui at,0x800b / addiu at,at,9748 (=0x800b2614 = DAT_800b2610[2]) / addu at,at,v0 / lhu v0,0(at); @0x8004924c sh v0,0(s0) → prim+14 = clut = DAT_800b2610[2+prop8]. No sb/sh to prim+4..+6 (r,g,b) anywhere in the function — RGB stays the builder template's (0x80,0x80,0x80), live-confirmed in prim buffers 0x80199800/0x80199814. x/y: grid mode @0x80049138-180 cell table 0x80076274+slot*4 + base DAT_800b25e0/e2, y+20; equip mode @0x80049184-1e0 uses DAT_800b25c8/25dc/25de (wide-weapon x-40 when slot+2 byte==2 @0x800491a8-c0); digit advance s2+=6 @0x80049230; AddPrim jal 0x8006b538 @0x8004925c with OT 0x800adca4 + parity(0x800aca34)<<12.

### Citations
- @0x80049b00-0x80049b10 (FUN_80049a5c, RE1.5-EXE): lbu a2 from 0x80074db0+id*12 = prop table @0x80074da8 offset +8, then jal FUN_80048f28 (grid call, a3=0)
- @0x80049b34-0x80049b74 (FUN_80049a5c): second call for equipped slot DAT_800b25c8 with a3=1 — same color byte, refutes equipped-marker hypothesis
- @0x8004922c-0x8004924c (FUN_80048f28): clut = lhu @(0x800b2614 + a2*2) = DAT_800b2610[2+a2], stored to SPRT prim+14; @0x80049214-0x80049228: v=0xf0, u=digit*8; no rgb write in the whole function (dis 0x80048f28..0x800492b4)
- @0x80049124 sltiu v0,v0,0x22 id gate inside FUN_80048f28 (function identity anchor)
- PSX.EXE file bytes @0x80074db0+id*12 (file_off 0x800+addr-0x80010000): prop8 sweep ids 0x00-0x21 = 0,0,0,3,3,3,3,3,3,3,1,2,3,3,2,3,1,2,3,3,3,0,0,0,2,0,1,2,0,0,0,1,2,1
- stage_saves/mzd_inv_open.sav RAM @0x800b10ac = 01 00 00 00 / 03 0f 00 00 / 15 32 00 00 (knife q0, Browning q15, bullets q50); @0x800b2610 = 10 7a 50 7a 90 7a d0 7a 10 7b 50 7b 90 7b d0 7b
- stage_saves/mzd_inv_open.sav live prim buffers @0x80199800/@0x80199814: digit SPRTs rgb=(128,128,128) code=0x66 wh=8x8; knife '0' (222,78) clut 0x7a90; Browning '1','5' (262/268,78) clut 0x7b50; bullets '5','0' (222/228,108) clut 0x7a90
- stage_saves/mzd_inv_open.sav VRAM (886-byte shift): CLUT 0x7a90=(256,490) entry1=(0,128,0) green; CLUT 0x7b50=(256,493) entry1=(0,112,184) cyan; CLUT 0x7ad0=(256,491) entry1=(160,144,0) yellow; CLUT 0x7b10=(256,492) entry1=(176,80,0) orange
- ghidra1_V2.txt:162057 XREF list: FUN_80048f28 called from FUN_80049a5c:80049b10/80049b70 + 0x8004c8a8 + 0x8004da00

---
## Q2: QUESTION 2: portrait prim (group 10) anchor/uv + STPIC selection — which prim(s) sample the STPIC VRAM block (640,406)/CLUT(0,485), their true screen anchor, and how the STPIC index is selected (resol

RESOLVED: True

### Answer
QUESTION 2 RESOLVED — all three parts, live-verified against mzd_inv_open.sav (prims parsed + textures re-rendered pixel-true).

(a) WHICH PRIMS SAMPLE THE STPIC BLOCK (640,406)/CLUT(0,485): NOT group 10. Two consumers, both in the FUN_80046a1c-built arena @0x80198000 (not in the 12-group master-table arenas):
1. THE ID-CARD SPRT — one sprite, double-buffered pair live @0x80198230/@0x80198244: code 0x66 (semi-trans SPRT), xy=(14,26), w=128 h=63, uv=(0,0x96)=(0,150), clut=0x7940. clut 0x7940 = GetClut(0,0x1e5) = VRAM CLUT (0,485) = exactly the STPIC CLUT; under the final DR_MODE @0x800b2620 (tpage 0x9a = 8bpp page (640,256)) uv(0,150) samples VRAM (640,406)-(703,468) = STPIC pixel rows 0-62. Re-rendering these texels with that CLUT reproduces the Leon POLICE card byte-identically to the framebuffer at (14,26)-(141,88) (scratchpad q2b_card_prim.png vs q2_fb_card.png). Build site: FUN_80046a1c (called with 0x80198000 from LAB_80049524), decompile block writing code=0x66, u=0/v=0x96, w=0x80/h=0x3f, clut=GetClut(0,0x1e5) (RE_15_Quellcode_V2/FUN_80046a1c.c lines 171-198).
2. THE FULL-SCREEN BACKGROUND GRID — 48 SPRTs (8 cols x 6 rows, 40x40, screen (0,12)-(320,252)) @0x80198258..0x801989c4, uv=(0,0xd5)=(0,213), same clut 0x7940 → samples VRAM (640,469)-(659,508) = STPIC rows 63-102: the navy screen-background tile ships INSIDE the STPIC file's lower rows (FUN_80046a1c.c lines 200-238, the 6x8 do-loop writing v=0xd5).

(b) TRUE SCREEN ANCHOR: the card sprite is re-anchored EVERY frame in the draw chain FUN_80049a5c tail to exactly (DAT_800b25f0, DAT_800b25f2) = (14,26) — the ID-CARD base registers, zero offset: @0x8004a018 lui v0,0x800b; @0x8004a01c lhu v0,0x25f0(v0) [DAT_800b25f0]; @0x8004a020 addiu s1,s1,40; @0x8004a024 sh v0,8(s1) [prim.x]; @0x8004a028-0x8004a034 lhu DAT_800b25f2 → sh v0,10(s1) [prim.y]; @0x8004a044 jal 0x8006b538 (AddPrim). The build-time template xy=(0,8) is dead after the first frame. So the card art = screen (14,26)-(141,88); STPIC columns >=116 sample past the 58-hw block (transparent/garbage right edge, covered by the ARMS rail chrome).
GROUP 10 CONTRADICTION RESOLVED: g10 (master table @0x80074a8c row 10: clut_idx=2, count=1, tmpl @0x800752f4, primbuf 0x8019ec00; live prim (13,90) 112x72 uv(128,0) clut 0x7a90) is NOT a portrait — under the 4bpp DR_MODE tpage 0x1b (704,256) its uv(128,0) samples VRAM hw (736,256)-(763,327) = TEX.TIM art, and the re-render is the "E.C.G." MONITOR BEZEL (title + instruction text + screen frame), pixel-matching the framebuffer at (13,90)-(124,161) (q2b_g10_4bpp.png vs q2_fb_g10.png). Its anchor is the CONDITION/ECG base (DAT_800b25e4/e6)=(13,82) + template offset (0,8) — report B's coordinates were right, its "PORTRAIT" label wrong. Both reports were correct about pixels; the roles were swapped.

(c) STPIC INDEX SELECTION — found (the "no static xref" gap): the selector is a DEBUG.BIN-module function @0x800C01C4 (disassembled from savestate RAM; not in the EXE image, hence no Ghidra xref):
  @0x800c01cc lui v1,0x800b; @0x800c01d0 lbu v1,-13732(v1) → u8 @0x800ACA5C = CHARACTER/COSTUME INDEX (0..15);
  @0x800c01d4 lui v0,0x8007; @0x800c01d8 addiu v0,v0,0x3ad8 → 0x80073AD8; @0x800c01dc sll v1,1; @0x800c01e4 lhu a0,0(v0+v1) → file_id = u16table[char] (table = identity {0x2c..0x3b} = STPIC_00..0F, EXE file off 0x642d8);
  @0x800c01e8-01f4 a1=0x801D7700, jal FUN_80013b60(file_id, 0x801D7700, 0) — CD-load the STPIC TIM to RAM;
  @0x800c01f8-0200 jal 0x800c0258(0x801d7700) — TIM upload helper: OpenTIM @0x8006bbbc / ReadTIM @0x8006bbcc (catalog-confirmed), then LoadImage @0x80068c88 + DrawSync @0x80068a60 for the CLUT rect and the pixel rect AT THE TIM-HEADER COORDS → CLUT lands at (0,485), pixels at (640,406). (Same helper reused by the CHECK-photo path @0x800c68fc.)
CALLER (single site, whole EXE + module scanned): FUN_800314b0, the per-character player init — jal 0x800c01c4 @0x800316d0. The same function proves @0x800ACA5C is the character id: it indexes the player-archive CD-file table @0x80073F70 (u16 {0x3c..0x4b}, 16 per-character archives) with the same byte, sets the default weapon DAT_800aca5d = FUN_800c00a8() (byte table @0x800c00d4: chars 0-14 → weapon 1 = knife, char 15 → 0), and sets hp=100 @0x80031710-18. So: STPIC index == character/costume id, selected ONCE at player init (not per menu-open, not per condition); the card is permanently resident in VRAM. Savestate: u8@0x800aca5c = 0 → file 0x2c = STPIC_00 (Leon) — matches the proven Leon==STPIC_00.

PORT IMPLICATIONS (wave 1): (1) draw the ID card as ONE 128x63 sprite at (DAT_800b25f0/f2)=(14,26) from STPIC rows 0-62 with the STPIC's own CLUT — not 112x72, not at y 90; (2) g10 = ECG bezel from the TEX.TIM 4bpp page (u=128 → hw 736,256), clut 0x7a90=DAT_800b2610[2], anchored to the ECG base; (3) the navy screen background = STPIC rows 63-102 tiled 8x6 at 40x40 with the STPIC CLUT; (4) STPIC file = 0x2c + character byte (Leon build: hardcode index 0 with the @0x800c01c4 citation).

### Mechanism
Card draw: FUN_80046a1c (@0x80046a1c) builds one double-buffered semi-trans SPRT (code 0x66) with u=0,v=0x96,w=0x80,h=0x3f,clut=GetClut(0,0x1e5); FUN_80049a5c re-anchors it per frame @0x8004a018-0x8004a034 (lhu DAT_800b25f0 -> sh x; lhu DAT_800b25f2 -> sh y) and AddPrims it under DR_MODE @0x800b2620 (tpage 0x9a, 8bpp (640,256)), so it samples VRAM (640,406)-(703,468) = STPIC rows 0-62 with CLUT (0,485). STPIC selection/upload: FUN_800314b0 @0x800316d0 calls DEBUG.BIN fn @0x800C01C4: lbu char @0x800ACA5C; lhu file_id = [0x80073AD8 + char*2] (identity {0x2c..0x3b}); FUN_80013b60(file_id, 0x801D7700, 0); helper @0x800c0258 (OpenTIM 0x8006bbbc / ReadTIM 0x8006bbcc / LoadImage 0x80068c88 / DrawSync 0x80068a60) uploads CLUT+pixels at the TIM-header coords (0,485)/(640,406). Group 10 (master table @0x80074a8c row 10, tmpl @0x800752f4) is the ECG bezel: SPRT (13,90)=(25e4,25e6)+(0,8), 112x72, uv(128,0), clut 0x7a90, sampled from the 4bpp tpage 0x1b (704,256) = TEX.TIM.

### Citations
- FUN_80046a1c @0x80046a1c (RE_15_Quellcode_V2/FUN_80046a1c.c:140-238) — builds the card SPRT pair: code 0x66, uv=(0,0x96), w=0x80 h=0x3f, clut=GetClut(0,0x1e5)=0x7940=(0,485), plus the 8x6 v=0xd5 background grid
- live prim buffer @0x80198230/@0x80198244 (mzd_inv_open.sav RAM): xy=(14,26) 128x63 uv=(0,150) clut=0x7940 — pixel-render == framebuffer card (14,26)-(141,88)
- FUN_80049a5c tail @0x8004a018-0x8004a044: lhu DAT_800b25f0 -> sh prim+8; lhu DAT_800b25f2 -> sh prim+10; jal 0x8006b538 (per-frame card anchor = ID-card base regs, zero offset)
- master table @0x80074a8c row 10 (PSX.EXE file 0x6528C+0x78): clut_idx=2 count=1 tmpl=0x800752f4 primbuf=0x8019ec00; live g10 prim (13,90) 112x72 uv(128,0) clut 0x7a90 = ECG bezel from 4bpp tpage 0x1b (704,256) — pixel-verified, NOT the portrait
- STPIC selector @0x800C01C4 (DEBUG.BIN, savestate-RAM disasm): @0x800c01d0 lbu 0x800ACA5C (character id); @0x800c01d4-01e4 lhu [0x80073AD8 + id*2]; @0x800c01f0 jal FUN_80013b60(file_id, 0x801D7700, 0); @0x800c01fc jal 0x800c0258
- TIM upload helper @0x800C0258 (DEBUG.BIN): OpenTIM @0x8006bbbc / ReadTIM @0x8006bbcc + LoadImage @0x80068c88 + DrawSync @0x80068a60 at TIM-header coords -> CLUT (0,485), pixels (640,406)
- u16 table @0x80073AD8 (PSX.EXE file off 0x642d8) = {0x2c..0x3b} identity -> STPIC_00..0F CD file ids (file table @0x8006f43c)
- caller FUN_800314b0 @0x800316d0 jal 0x800c01c4 (sole call site, EXE+module scan); same fn: player archive table @0x80073F70={0x3c..0x4b}[0x800aca5c], default weapon FUN_800c00a8/@0x800c00d4, hp=100 @0x80031710
- savestate ground truth: u8@0x800aca5c=0 -> STPIC_00 (Leon); DAT_800b2610 cluts 0x7a10..0x7bd0; renders scratchpad q2b_card_prim.png / q2b_g10_4bpp.png / q2_fb_card.png / q2_fb_g10.png

---
## Q3: QUESTION 3 — ST_00 runtime VRAM anomaly: determine EXACTLY the runtime VRAM layout at (640,256)-(704,512) (savestate VRAM + upload code) and produce the authoritative uv->content map wave 1 must use (

RESOLVED: True

### Answer
RESOLVED. There is NO second upload, NO MoveImage relocation of ST_00 art, and NO transformed upload. The "anomaly" dissolves into three byte-proven facts:

(1) ST_00.TIM IS uploaded verbatim to (640,256): the packer FUN_8004ee78 can only produce y=0 or y=256 (`sltiu v0,cursor,0x10; xori v0,1; sll v0,8` @0x8004eec8-eed4) and x=cursor*64hw-1024 (@0x8004eea8-eeb8); the chain @0x800212dc-f8 loads file 0x20 and sets cursor halfword 0x041a -> pixels (640,256), CLUT row0 -> (0,484).

(2) The reason "many ST_00 rows don't byte-match VRAM" is that ST_00's top 120 rows are the FACTORY PRE-LOAD of the live icon-cache grid. The whole rect (640,256)-(700,436) is a 3-col x 6-row grid of 40x30 8bpp cells (cell k: x_hw=640+(k%3)*20, y=256+(k/3)*30 — FUN_800492b8 @0x80049318-6c). Cells 0-9 are DYNAMIC (rewritten with the inventory icons / blank tiles); cells 10 and 11 are RESERVED STATIC ART shipped inside ST_00: cell 10 = the diagonal Standard-Arms knife (ST_00 rows 90-119, bytes 40-79 — byte-identical 1200/1200 in the savestate), cell 11 = the blank navy tile (indices e3/e4/e6) which is the hardcoded MoveImage SOURCE rect (0x2a8,0x15a,0x14,0x1e)=(680,346) 20hw x 30 in FUN_8004947c @0x80049494-b4, used by compaction FUN_8004dadc (@0x8004dbec) to blank vacated cells. The capacity byte DAT_800b0fbc=0x0a (live) bounds every alloc/shift loop (`sltu` @0x8004db10), so cells 10/11 are never allocated — they keep their ST_00 art by construction. Live savestate verdict per cell (all 1200/1200 byte compares): slots 0/1/2 = ITEMALL tiles 0x01/0x03/0x15; slots 3-9 and 11 = ITEMALL tile 0 (blank navy — identical to ITPS blk0 icon per the proven identity); slot 10 = ST_00 nominal knife; cells 12-14 (y 376-405) untouched = ST_00 rows 120-149 full-row matches.

(3) The "big gun at real y~242" was a MISMEASURE: rows y 240-255 at x 640-704 are uniform byte 0x02 (measured); the handgun art there is the icon-cache tile 0x03 starting exactly at (660,256). No weapon art exists above y=256.

AUTHORITATIVE RUNTIME VRAM MAP, hw 640-768, y 256-512 (= tpage 0x9a, u=byte 0-255, v=y-256):
- v 0-119, u 0-119 — ICON CACHE grid cells 0-11 (uv per slot from table @0x80076244, 12 entries {u16 u,u16 v} = ((k%3)*40,(k/3)*30), dumped: 00/28/50 x 00/1e/3c/5a):
  - cells 0-9 (v0-89 all cols + v90-119 u0-39): DYNAMIC — icon of inventory[k].id (ITPS blk[id]+0x21a0 == ITEMALL tile[id]); empty slot = blank tile (== tile 0).
  - cell 10 (u40-79, v90-119): STATIC diagonal-knife art (Standard-Arms box display) = ST_00 rows 90-119.
  - cell 11 (u80-119, v90-119): STATIC blank navy tile = compaction MoveImage source.
- v 120-149, u 0-119 — STATIC ST_00 atlas rows 120-149 (crest/header art), the only surviving "trust-ST_00-uv" band.
- v 150-252, u 0-115 — STPIC_xx ID card (per character; Leon=STPIC_00), uploaded at its TIM-header coords (640,406) 58hw x 103, overwriting ST_00's shipped placeholder card (ST_00 rows 150-252 no longer exist in VRAM).
- v 253-255 — ST_00 rows 253-255 (uniform 0). ST_00 rows 256-277 (also uniform 0) target y>=512; the room-texture noise at (640,0-127) is intact, so the transfer was clipped, not wrapped — zero observable effect either way.
- u 120-127 (all v) and u 116-127 in the STPIC band — orphaned ST_00 margin bytes (mostly 0), sampled by nothing.
- u 128-255 (hw 704-768) — overlap with TEX.TIM: file 0x21 uploaded at cursor 0x001b -> (704,256), pixel block 320hw x 256, byte-verified 256/256 rows. Valid only via the 4bpp tpage 0x1b; 8bpp reads there are garbage.

CLUTs (bonus, closes part of plan §5-Q4): (0,484) = ST_00 CLUT row 0 = the palette for ALL 8bpp grid content incl. icons/knife/blank (grid cell sprites use GetClut(0,0x1e4) = id 0x7900); (0,485) = STPIC's own CLUT (byte-match; it OVERWRITES ST_00 CLUT row 1); (0,489) is ALL-ZERO live — the ITPS per-block CLUT is NOT resident on the status screen (only during pickup/CHECK), so grid icons must NOT be paletted from ITPS CLUTs; (256,480)-(288,503) = TEX.TIM's own 32x24 CLUT block at its header coords — the 8 UI clut ids 0x7a10-0x7bd0 in DAT_800b2610 decode to (256,488)-(256,495) = TEX.TIM CLUT rows 8-15 (chrome/4bpp sprites).

WAVE-1 RULE derived from this: trust ST_00.TIM uv's ONLY for v 120-149 (+ the zero margins v253-255/u120-127); v 0-119 must be COMPOSED at runtime from the icon-cache model (inventory ids -> tiles, empties -> blank tile, cell 10 = ST_00 knife art, cell 11 = blank source); v 150-252 comes from STPIC_xx; u>=128 belongs to TEX.TIM. The port should treat ST_00 rows 0-119 as initial cache contents only (or simply initialize cells 10/11 from ST_00 and rebuild 0-9), never as a static atlas.

### Mechanism
Packer FUN_8004ee78: `lbu v1,0(s1)` [s1=0x800aca4c cursor] -> `sll a0,v1,6; sltiu v1,v1,0x10; bne...; addiu a0,a0,-1024; sh a0,0(prect)` (@0x8004eea0-eebc: x = cursor*64hw, minus 1024 iff cursor>=0x10) then `sltiu v0,cursor,0x10; xori v0,0x1; sll v0,v0,8; sh v0,2(prect)` (@0x8004eec8-eed4: y = 256 iff cursor>=0x10 else 0 — no other y possible) -> LoadImage(prect,paddr) (jal 0x80068c88 @0x8004eee0); cursor += (w+63)>>6 (@0x8004eef8-ef18); CLUT: `crect->y = 0x1e0 + DAT_800aca4d` then aca4d += clut_h (@0x8004ef2c-ef6c). Upload chain: @0x800212ac-d8 load file 0x21 (TEX.TIM) + `ori v0,zero,0x1b; sh v0,0(0x800aca4c)` -> (704,256), CLUT y 480; @0x800212dc-f8 load file 0x20 (ST_00.TIM, FUN_80013b60) + `ori v0,zero,0x41a; sh` -> pixels (640,256), CLUT y 484. Icon-cache upload FUN_800492b8 @0x80049318-6c: x=((slot%3)*40-128)/2+704 &~3 = 640+(slot%3)*20hw, y=(slot/3)*30+256, w=0x14 hw (mode1: 0x28), h=0x1e. Staging copy FUN_8004947c @0x80049494-b4: literal source RECT(0x2a8,0x15a,0x14,0x1e) = grid cell 11 (680,346), MoveImage (jal 0x80068d50 @0x80049504) to the same grid formula (@0x800494d0-4fc). Compaction FUN_8004dadc: FUN_8004df2c finds first free slot (@0x8004daec), loop bounded by `lbu v0,4028(0x800b)`=DAT_800b0fbc capacity 0x0a (`sltu s0,v0` @0x8004db04-14) shifting cell images down via jal 0x80049390 (@0x8004db30) + slot-table bytes 0x800b10ac/ad/ae (@0x8004db38-dba8) + equipped-slot fixup DAT_800b25c8 (@0x8004dbac-dbc8), then refills the vacated last cell from the static blank cell 11 via jal 0x8004947c (@0x8004dbec) and zeroes its slot entry (@0x8004dc0c-2c). Because capacity=10, grid cells 10/11 are never written by the cache — their content is ST_00.TIM's shipped art at those grid positions. STPIC_xx is uploaded at its TIM-header dest (640,406)/CLUT(0,485) (header-coord load, not the packer), overwriting ST_00 rows 150-252.

### Citations
- FUN_8004ee78 packer x/y: @0x8004eea0-eebc (x=cursor<<6, -1024 if >=0x10), @0x8004eec8-eed4 (y=(cursor>=0x10)<<8: only 0 or 256), @0x8004eef8-ef18 (cursor+=ceil(w/64)), @0x8004ef2c-ef6c (CLUT y=0x1e0+DAT_800aca4d) — RE1.5 PSX.EXE disasm
- Upload chain: @0x800212ac-d8 TEX.TIM id 0x21, cursor 0x001b -> (704,256); @0x800212dc-f8 ST_00.TIM id 0x20, cursor halfword 0x041a -> (640,256)/CLUT y484 — disasm
- Icon-cache upload FUN_800492b8 @0x80049318-6c: x=640+(slot%3)*20hw, y=256+(slot/3)*30, w 0x14/0x28 hw, h 0x1e
- Staging MoveImage FUN_8004947c @0x80049494-b4: literal rect (0x2a8,0x15a,0x14,0x1e)=(680,346)=grid cell 11; dest formula @0x800494d0-4fc; MoveImage jal 0x80068d50 @0x80049504
- Compaction FUN_8004dadc: capacity gate lbu DAT_800b0fbc + sltu @0x8004db04-14; shift jal 0x80049390 @0x8004db30; slot table 0x800b10ac/ad/ae moves @0x8004db38-dba8; equipped fixup DAT_800b25c8 @0x8004dbac-dbc8; blank refill jal 0x8004947c @0x8004dbec; slot zero @0x8004dc0c-2c
- uv table @0x80076244 dumped (PSX.EXE file 0x66a44): 12x{u16 u,u16 v} = 00/28/50 x 00/1e/3c/5a; cell table @0x80076274: (4,32)..(44,152), entry10=(-66,88), entry11=(120,60)
- Savestate mzd_inv_open.sav VRAM (base = RAM-dump offset 0x200000+886, pinned by unique hit of ST_00 CLUT row0 at (0,484)): cells 0/1/2 @ (640/660/680,256) == ITEMALL tiles 0x01/0x03/0x15 1200/1200; cells 3-9,11 == ITEMALL tile 0 1200/1200; cell 10 @ (660,346) == ST_00 rows 90-119 bytes 40-79 1200/1200; (640,376-405) == ST_00 rows 120-149 full 128B rows; (640,406)-(697,508) == STPIC_00 pixel block, (0,485) == STPIC CLUT; (704,256)+ == TEX.TIM 320hw x 256, 256/256 rows; (256,480-503) == TEX.TIM CLUT block (rows 8-15 = clut ids 0x7a10-0x7bd0 of DAT_800b2610); (0,489) all-zero (no ITPS CLUT resident); y240-255 x640-704 uniform 0x02 (refutes 'gun at y~242'); DAT_800b0fbc=0x0a; inventory 0x800b10ac = 01 00 / 03 0f / 15 32
- Scripts + artifacts: scratchpad q3_map2.py/q3_map3.py/q3_cells.py/q3_src.py/q3_tex.py/q3_cluts.py, renders q3_st00_full.png / q3_col640_full.png (C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\b2650b2c-85d7-477c-8d7d-967f39582e74\scratchpad)
- Assets: re15_port/shared_assets/PSX/DATA/ST_00.TIM (pix 64hw x 278, CLUT 256x2 hdr dest (0,480)), DATA/TEX.TIM (pix 320hw x 256 + CLUT block hdr (256,480,32,24)), DATA/ITEMALL.PIX, ITEM/STPIC_00.TIM (pix hdr dest (640,406), CLUT (0,485)), ITEM/ITPS.ITP

---
## Q4: QUESTION 4: the CLUT-per-prim assignment of the RE1.5 status/inventory screen — dump the clut id of every SPRT in groups 0,1,3,4,5,6 from the live savestate prim buffers, map which prims use which of 

RESOLVED: True

### Answer
DEFINITIVE PER-ELEMENT CLUT TABLE (dumped from the LIVE prim buffers of stage_saves/mzd_inv_open.sav; full dump saved to scratchpad q4_dump.txt; SPRT layout tag+0 rgb+4 code+7 xy+8 uv+0xc clut+0xe wh+0x10 per FUN_800467a8, each element = 0x28 B = 2 double-buffered SPRTs; A/B halves identical unless noted).

A. DAT_800b2610[8] live values (set FUN_800460b8 via GetClut(0x100,0x1e8..0x1ef)): idx0=0x7a10(256,488) 1=0x7a50(256,489) 2=0x7a90(256,490) 3=0x7ad0(256,491) 4=0x7b10(256,492) 5=0x7b50(256,493) 6=0x7b90(256,494) 7=0x7bd0(256,495).

B. Master-table groups (buffer @0x8019b000+g*0x600; every SPRT in a group carries DAT_800b2610[table clut_idx] uniformly, exceptions listed):
- g0 chrome, 26 SPRTs @0x8019b000: ALL clut 0x7a10 (idx0).
- g1 ITEM-LIST panel, 15 @0x8019b600: ALL 0x7a90 (idx2).
- g2, 2 @0x8019bc00: p0 'Fine' word (49,140) 0x7a90; p1 pulse LED (25,108) 0x7b90 (idx6 — animator FUN_80047648 cond-based swap; rgb=pulse 28/32).
- g3 ARMS, 9 @0x8019c200: ALL 0x7a90.
- g4 ID header (14,26)112x8, 1 @0x8019c800: 0x7a90.
- g5 cursor highlights, 2 @0x8019ce00: 0x7b90 (idx6; FUN_80047648 dim-swap picks DAT_800b261a=0x7b50 / 261c=0x7b90 when DAT_800b25ca==1 — live shows 0x7b90).
- g6 tab cluster, 13 @0x8019d400: ALL 0x7bd0 (idx7).
- g7 action cluster, 13 @0x8019da00: ALL 0x7bd0.
- g8 big 16x32/8x32 digit sprites (219,58) uv(72/88/96,48), 8 @0x8019e000: ALL 0x7b90 (idx6).
- g9 16x16 digits, 2 @0x8019e600: 0x7b90.
- g10 (13,90)112x72 uv(128,0), 1 @0x8019ec00: 0x7a90 (NOT the STPIC card prim — see D).
- g11 4 screws 16x16 uv(112,40..88) @0x8019f200: 0x7b10 (idx4).
These groups draw on the 4bpp TEX.TIM pixel page (DR_MODE @0x800b2638/2644 = tpage 0x1b (704,256), tag 0x02199d14 chains the glyph buffer; live words e100001b).

C. ITEM-LIST icon SPRTs (cell buffer @0x80198000, FUN_80046a1c, code 0x66 semi-trans): 10 grid cells (219/259, 58..178) uv 3-col grid + c10 Standard-Arms gun (150,114 rgb 62=DAT_800b25cd) + c11 equip-box gun (150,58) + c12 (126,26) + c13 80x30 wide (131,58) — ALL clut 0x7900 = VRAM(0,484) = ST_00.TIM CLUT ROW 0 (GetClut(0,0x1e4) @FUN_80046a1c.c:14). They sample the 8bpp icon cache page (DR_MODE @0x800b2620/262c tpage 0x9a (640,256), live e100009a). The ITPS per-block CLUT (0,489) is NOT used — live VRAM row (0,489) is ALL ZEROS with the screen open; grid icons byte-render with ST_00 row 0.

D. ID-card + background (same buffer, after the cells): card art SPRT (14,26) 128x63 uv(0,150) [= 8bpp (640,406) = the STPIC pixel block] + 48 background sprites 40x40 uv(0,213) tiling the screen — ALL clut 0x7940 = VRAM(0,485) = STPIC_00 CLUT (GetClut(0,0x1e5) @FUN_80046a1c.c:171). Live row (0,485) == STPIC_00.TIM CLUT 256/256 hw (it OVERWRITES ST_00 CLUT row 1, which matches only 1/256). This is the real card prim (plan contradiction #7: g10 is a TEX-page prim, not the STPIC sampler).

E. Qty-digit SPRTs (glyph buffer @0x80199800, 33 8x8 SPRTs, FUN_8004694c default clut DAT_800b2614=0x7a90): per-frame FUN_80048f28 writes clut = DAT_800b2610[2 + arg3] where arg3 = item-property byte @0x80074db0+id*12 (byte +8 of prop row @0x80074da8): 0->0x7a90 (green), 1->0x7ad0, 2->0x7b10, 3->0x7b50 (cyan). Prop byte8 per id: 0x00-0x02=0, 0x03-0x09=3, 0x0a=1, 0x0b=2, 0x0c-0x0d=3, 0x0e=2, 0x0f=3, 0x10=1, 0x11=2, 0x12-0x14=3, 0x15-0x17=0, 0x18=2, 0x19=0, 0x1a=1, 0x1b=2, 0x1c-0x1e=0, 0x1f=1, 0x20=2, 0x21=1; ids>=0x22 draw no digits (sltiu 0x22 gate @0x80049124). LIVE: knife '0' t02 (222,78) uv(0,240) 0x7a90; Browning '15' t04/t05 (262/268,78) uv(8,240)/(40,240) 0x7b50 = CYAN; bullets '50' t07/t08 (222/228,108) 0x7a90; equip-panel '0' t32 (153,78) 0x7a90. Digit glyph uv=(digit*8, 240) 8x8 on the 4bpp TEX page. THIS ALSO RESOLVES plan §5 Q1 (green-vs-cyan): the color is the TEX CLUT row picked by the item's prop byte8 (ammo-class code), not a slot flag.

F. COLOR-DATA SOURCE of the 8 CLUTs: they are TEX.TIM's own CLUT block. TEX.TIM (DATA/TEX.TIM, CD file id 0x21) header: CLUT rect (256,480) 32x24 (payload @file+0x14, 64 B/row). Byte-compare: file CLUT rows 0..23 == live VRAM (256,480..503) row-for-row. So DAT_800b2610[0..7] = ids (256,488..495) = TEX.TIM CLUT rows 8..15 = file bytes [0x214..0x413]. Upload site: @0x800212ac 'ori a0,0x21' + jal 0x800116f4 (loads TEX.TIM to 0x80100000), @0x800212d0-d8 'ori v0,0x1b; sh v0,0(0x800aca4c)' (cursor aca4c=0x1b pixels->(704,256), aca4d=0) then jal FUN_8004ee78 which LoadImages crect at y=0x1e0+DAT_800aca4d=480 KEEPING the TIM header x=256. ST_00 CLUT: @0x800212dc-f8 loads id 0x20 then cursor 0x041a (aca4c=0x1a->pixels (640,256), aca4d=4->CLUT y=484, x=0) -> ST_00 rows at (0,484),(0,485); row (0,484) byte-matches ST_00.TIM file CLUT row 0 (payload @+0x14) 256/256. STPIC CLUT is later uploaded at its own header rect (0,485) (upload call site still untraced statically — open question from report C — but the live VRAM content proves it), replacing ST_00 row 1.

Wave-1 rule set: master-group SPRTs use TEX CLUT row 8+clut_idx (16-color 4bpp cluts, x=256); qty digits use TEX row 10/11/12/13 by prop-byte8; grid/equip icons use ST_00 CLUT row 0 (0,484); card art + screen background use STPIC CLUT (0,485); nothing on this screen uses the ITPS CLUT (0,489).

### Mechanism
Group prims: FUN_800467a8 @0x800467a8 builds each master-table group and stores SPRT->clut = DAT_800b2610[*(u16*)(0x80074a8c+group*0xc)] (decompile line '*puVar3 = (&DAT_800b2610)[*(ushort *)(&DAT_80074a8c + iVar2)]'), where DAT_800b2610[i] = GetClut(0x100,0x1e8+i) set in FUN_800460b8 @0x800460b8; FUN_80047648 @0x80047648 swaps cluts per-frame only for g2 prim0 (DAT_800b2610[idx+cond]) and g5 (DAT_800b261a/261c when DAT_800b25ca==1). Icon cells: FUN_80046a1c stores clut=GetClut(0,0x1e4)=0x7900 into all 10 grid + equip/Standard-Arms sprites, and clut=GetClut(0,0x1e5)=0x7940 into the 128x63 card sprite + 48 40x40 background sprites. Qty digits: FUN_80048f28 @0x8004922c-4c: lbu v0,24(sp) (3rd arg = prop byte @0x80074db0+id*12); at=0x800b2614+v0*2; lhu v0,0(at); sh v0,0(s0) => digit SPRT clut = DAT_800b2610[2+prop_byte8]. CLUT pixel data: FUN_8004ee78 LoadImages each TIM's CLUT block at y=0x1e0+DAT_800aca4d keeping the header x; TEX.TIM (id 0x21, crect (256,480) 32x24) uploaded @0x800212d0-d8 with cursor 0x001b (aca4d=0) => rows (256,480..503); its rows 8..15 are the 8 menu CLUTs. ST_00.TIM (id 0x20, crect (0,480) 256x2) uploaded @0x800212f0-f8 with cursor 0x041a (aca4d=4) => (0,484..485); STPIC_00.TIM CLUT later lands at its header rect (0,485) overwriting ST_00 row 1. ITPS block CLUT dest (0,489) is never uploaded on this screen (VRAM zeros).

### Citations
- FUN_800467a8 @0x800467a8 (RE_15_Quellcode_V2/FUN_800467a8.c): SPRT clut = DAT_800b2610[u16 @0x80074a8c+group*0xc]; prim entry 0x28B = 2 SPRTs (clut at +0xe/+0x22)
- master table @0x80074a8c live: g0{idx0,26,@0x80075108,0x8019b000} g1{idx2,15,@0x80075240,0x8019b600} g2{idx2,2} g3{idx2,9} g4{idx2,1} g5{idx6,2} g6{idx7,13,@0x80075390,0x8019d400} g7{idx7,13} g8{idx6,8} g9{idx6,2} g10{idx2,1} g11{idx4,4}
- DAT_800b2610[8] live RAM @0x800b2610-261e = 0x7a10,0x7a50,0x7a90,0x7ad0,0x7b10,0x7b50,0x7b90,0x7bd0 (set FUN_800460b8 GetClut(0x100,0x1e8..0x1ef))
- live prim dumps: g0 all 0x7a10 @0x8019b000-0x8019b40f; g1 all 0x7a90 @0x8019b600+; g6/g7 all 0x7bd0 @0x8019d400/0x8019da00; g8/g9 0x7b90; g11 0x7b10 (scratchpad q4_dump.txt, mzd_inv_open.sav)
- icon cells: FUN_80046a1c.c:14 GetClut(0,0x1e4)=0x7900; live @0x80198000-0x8019822f all clut 0x7900 (0,484)
- card+bg: FUN_80046a1c.c:171 GetClut(0,0x1e5)=0x7940; live @0x80198230 (14,26)128x63 uv(0,150) + 48x 40x40 uv(0,213) @0x80198258-0x801989d7 all 0x7940 (0,485)
- digit clut chooser: FUN_80048f28 @0x8004922c-4c 'lbu v0,24(sp); sll v0,1; lhu v0,0x800b2614(v0); sh v0,0(s0)' = clut DAT_800b2610[2+arg3]; digit uv @0x800491f4-0x80049228 (v=0xf0, u=digit*8); id gate sltiu 0x22 @0x80049124
- prop byte8 @0x80074db0+id*12 (row base @0x80074da8): id1=0, id3=3, id0x15=0 (full 0x00-0x21 map read live)
- live digit SPRTs @0x80199850 (222,78) 0x7a90; @0x801998a0/c8 (262/268,78) 0x7b50 CYAN; @0x80199918/40 (222,108) 0x7a90; @0x80199d00 (153,78) 0x7a90
- TEX.TIM header: CLUT rect (256,480) 32x24, payload @file+0x14; file rows 0..23 byte== VRAM (256,480..503) => 8 menu CLUTs = TEX.TIM CLUT rows 8..15 = file [0x214..0x413]
- upload: @0x800212ac ori a0,0x21 (TEX.TIM); @0x800212d0-d8 ori v0,0x1b + sh ->0x800aca4c; @0x800212dc-f8 id 0x20 + cursor 0x041a (ST_00 CLUT y=0x1e0+4=484); FUN_8004ee78 (RE_15_Quellcode_V2) crect->y=DAT_800aca4d+0x1e0, x from TIM header
- VRAM row compares (savestate, linear shift +443px): (0,484)==ST_00 CLUT row0 256/256; (0,485)==STPIC_00 CLUT 256/256 (vs ST_00 row1 1/256); (0,489) ITPS CLUT dest = ALL ZEROS (unused)
- DR_MODE live words: @0x800b2620/262c=e100009a tpage (640,256) 8bpp (tag 0x021989c4 -> cell buffer); @0x800b2638/2644=e100001b (704,256) 4bpp TEX page (tag 0x02199d14 -> glyph buffer); @0x800b2650/265c=e1000017
