# RE1.5 Byte-True Inventory/Status Screen — Synthesized Implementation Plan

Synthesis of reports A (ground truth), B (screen FSM), C (assets), D (semantics), E (item box), F (port audit). Every claim carries its `@0x…` + binary; anything without one is marked **UNVERIFIED**. Address-space rule applied throughout: RE1.5-EXE globals live `0x800a0000–0x800c0000`; all `0x800d…`/`FUN_80068f9c`-class citations are RE2-Leon (confirmed by report E) and appear only in §4.

---

## 1. GROUND-TRUTH SCREEN SPEC

**Acceptance target:** `c:\workspace\git\reAi_v2\shots\inv_original.png` (320x240; decoded from `stage_saves/mzd_inv_open.sav`; real framebuffer at VRAM (0,0)/(0,240) double-buffered, appears in the VRAM PNG at x=443 due to a linear 886-byte dump shift — report A edge scan + report C shift derivation; this supersedes the old parity_net x=440 note). Acceptance = pixel-compare of the port's frame against this PNG in the identical state: 3 items (knife q0 / Browning HP q15 / H.gun bullets q50 = slots `@0x800b10ac`, RE1.5-EXE), equipped slot 0 (`DAT_800b25c8=0`), tab cursor on FILE (`DAT_800b25bc=3`), ECG sweep `DAT_800b2600=0x62`.

**Layout registers** (init `FUN_800460b8 @0x800460b8` + `LAB_80049524 @0x80049524`, RE1.5-EXE): ID card `(25f0,25f2)=(14,26)`; CONDITION/ECG `(25e4,25e6)=(13,82)`; ARMS `(25d8,25da)=(126,26)`; ITEM LIST `(25e0,25e2)=(215,26)`; tab cluster `(25e8,25ea)=(126,166)`; action cluster staged `(25ec,25ee)=(142,264)` (slides to ~166).

| Panel / element | Measured px (report A) | Mechanism + citation (binary) | Asset |
|---|---|---|---|
| Chassis chrome (frame, screws, rails) | full 320x240 border | prim group 0, geometry `@0x80075108` (26 SPRTs), master panel table `@0x80074a8c` (stride 0xc: {clut_idx→`DAT_800b2610[]`, count, tmpl, prim buf}), builder `FUN_800467a8 @0x800467a8`, animator `FUN_80047648 @0x80047648` (RE1.5-EXE) | ST_00.TIM atlas, clut 0x7a10 |
| ID panel | (13,21)-(125,93); cream card (56,26)-(121,88) | group 4 header (14,26) 112x8 uv(16,16); card art = STPIC TIM at its header VRAM coords (640,406)/CLUT(0,485) — byte-confirmed in savestate VRAM (reports A+C) | `ITEM/STPIC_00..0F.TIM` (16x, 8bpp 116x103; CD file ids 0x2c–0x3b, table `@0x8006f43c` RE1.5-EXE). Leon = STPIC_00 (==01==02==0C) |
| CONDITION window | (14,94)-(121,160); ECG monitor (26,117)-(111,151); green "Fine" (61,129)-(90,151) | ECG: 64 LineF2 buffer `FUN_80046f68 @0x80046f68` → renderer `FUN_80048a44 @0x80048a44`; waveform ptrs `@0x80076214[cond]`, colors `@0x8007622c[cond*4]` (Fine = 0x20,0xff,0x20), sweep `DAT_800b2600` 0x20..0x80 +1/frame, 32 trail lines −8·i fade, pulse LED `DAT_800b2602`; condition value `FUN_8004ed6c` (all RE1.5-EXE) | "Fine/Caution/Danger/Poison" words in ST_00 atlas (report C atlas dump) |
| ARMS CONTROL | (127,27)-(212,159); equip box (130,58)-(209,87); Standard-Arms box (149,114)-(195,143); green arrow (196,124)-(204,135) | group 3, geometry `@0x80075240` (15 rows, 88 wide); Standard-Arms box = **slot 10 of the cell table** `@0x80076274` entry 10 = (−66,88)+base(215,26)=(149,114) — exact pixel match (report A); equip-box gun prim: rgb=`DAT_800b25cd`, y += slide `DAT_800b2608` (draw chain `FUN_80049a5c @0x80049a5c`, RE1.5-EXE) | big weapon art from `DATA/ST_00.TIM` (file id 0x20 → VRAM(640,256), CLUT(0,484) — CLUT byte-confirmed) |
| ITEM LIST | (216,27)-(301,215); navy interior (219,40)-(301,212), 3 diagonal shades RGB(8,0,88)/(8,0,80)/(8,0,72) | group 1 @x=215; cells = table `@0x80076274` (s16 x,y stride 4): x∈{4,44}, y∈{32..152 pitch 30} + base (215,26) → **cells at x 219/259, y 58..178**, 40x30 (RE1.5-EXE) | icons: 40x30 8bpp, VRAM cache at (640,256)+ 3-col grid via `FUN_800492b8 @0x800492b8` (mode1 = 80x30 wide weapons); uv table `@0x80076244`; source at grant = ITPS block +0x21a0 (≡ ITEMALL.PIX tile[id], identity — report C byte-compare) |
| Qty digits | '0'(223,78) green, '15'(264,78) **cyan**, '50'(223,110) green | list-entry drawer `FUN_80048f28 @0x80048f28` (contains the `sltiu id,0x22` gate `@0x80049124`); **color rule UNRESOLVED** (slot +2 bytes are 0 in RAM — not the stored flag; open question §5) | TEX.TIM font / ST_00 digit glyphs |
| Button bar (tabs) | grid (72,166)-(213,213); 32x16 buttons at cols 94/126/158, rows 166/182/198; L1 socket (73,174), R1 (193,174) | button table `@0x80075390` (stride 0xc {dx,dy,w,h,u,v}) + base `(DAT_800b25e8/ea)=(126,166)`; layout top=ITEM, mid=MAP/logo/FILE, bottom=EXIT; cursor highlight group 5 from same table, dim-clut swap when `DAT_800b25ca==1` (RE1.5-EXE, all pixel-verified report A) | ST_00 atlas |
| Action cluster (command stage) | staged offscreen y=264 in target image | group 7; slides in on item confirm; prim0 label swap: id<0x15 → uv(0x10,0x90) EQUIP, else uv(0x68,0xc0) USE, cursor==equipped → uv(0x10,0xa0) (`FUN_80047648`, RE1.5-EXE) | ST_00 atlas |
| Portrait sprite (group 10) | — | **MUST-VERIFY**: report B lists "g10 PORTRAIT (13,90) 112x72 uv(128,0) clut 0x7a90" anchored to the ECG base (25e4,25e6), but report A measures the STPIC card art inside the ID panel at y 21–93. Which prim actually samples the STPIC VRAM block, and its true anchor, must be re-read from the live prim buffer before drawing | STPIC |
| Textures/CLUTs | — | 6 DR_MODEs (tpages 0x9a=8bpp(640,256), 0x1b=4bpp(704,256), 0x17=4bpp(448,256)) + 8 CLUTs `GetClut(0x100,0x1e8..0x1ef)` = VRAM(256,488..495), ids 0x7a10..0x7bd0 in `DAT_800b2610` — all set in `FUN_800460b8`, RAM-confirmed (reports A+B) | uploads: TEX.TIM (id 0x21)→(704,256), ST_00 (id 0x20)→(640,256)/CLUT(0,484) via packer `FUN_8004ee78 @0x8004ee78` `@0x800212ac-f8` (RE1.5-EXE) |
| OT / layering | — | OT = `0x800aa6d8 + parity(DAT_800aca34)*0x4000`; **earliest AddPrim = topmost**; draw order per `FUN_80049a5c` (report B) — map onto the PC renderer per skill re15-pc-render-order | — |

Full per-sprite element table (positions + uv for every chrome/panel/button prim) is in report B's "Draw-element table" fact — use it verbatim as the wave-1 build sheet; it came from the live prim buffers `@0x8019b000+` with templates `@0x80075108-0x800755f8` (RE1.5-EXE file image → extractable as data, no guessing needed).

---

## 2. GAP LIST (ordered by severity)

| # | Gap | Sev | Original mechanism (@0x, binary) | Port status (file:line) | Needed work |
|---|---|---|---|---|---|
| 1 | Entire screen render is a faithful-line approximation (invented panels/colors, debug font, no chrome) | CRIT | prim system: `@0x80074a8c` table + `FUN_800467a8`/`FUN_80047648`/`FUN_80049a5c` + geometry `@0x80075108/@0x80075240/@0x80075390/@0x80076274/@0x80076244` (RE1.5-EXE) | `platform/pc/main.c:2792-2881` | Wave 1: data-driven prim rebuild from the EXE tables + ST_00/TEX atlas |
| 2 | No STPIC portrait/ID-card art anywhere | CRIT | STPIC_xx → VRAM(640,406)/CLUT(0,485); ids 0x2c–0x3b (`@0x8006f43c`, RE1.5-EXE) | zero STPIC references (grep, report F) | Wave 1 decode + draw; selection index = open question |
| 3 | ECG is invented ("one beat per 40px") | CRIT | `FUN_80048a44` + tables `@0x80076214/@0x8007622c` + sweep `DAT_800b2600` (RE1.5-EXE, fully RE'd) | `main.c:93-109, 2813` | Wave 1: port renderer + waveform tables verbatim |
| 4 | Button semantics diverge: port SQUARE=equip / CROSS=close; original CROSS(virtual 0x4000 `DAT_800ac76c`)=confirm→**4-way command stage**, SQUARE(0x8000)=cancel | HIGH | command stage state 4 `@0x8004a458-a54c`, dispatch `@0x8004a73c`: [0]=USE→5, [1]=CHECK→9, [2]=exit-to-tabs→8, [3]=EXCHANGE→7 (RE1.5-EXE, reports B+D agree) | `menu_common.c:44-94` | Wave 3 FSM rewrite |
| 5 | No tab cluster function (ITEM/MAP/FILE/EXIT static text) | HIGH | run FSM `LAB_8004974c @0x8004974c` sub-state `DAT_800b25c1`: D-pad raw edge `DAT_800ac762` bits 0x1000/8000/4000/2000 → tab 0..3; L1/R1 raw 0x4/0x8 shortcuts; confirm 0x4000 (RE1.5-EXE) | tabs dead (`main.c:2818-2823`) | Wave 2 |
| 6 | No screen task FSM / phases / slide animations | HIGH | task `LAB_8004603c @0x8004603c`, phase table `@0x80074bdc`, ITEM-mode 10-state FSM `FUN_8004a0cc @0x8004a0cc` on `DAT_800b25c2`, slides ±0xe/frame (RE1.5-EXE) | menu is a flat open/closed bool | Wave 2 |
| 7 | Grid cell x = 217/257, byte-true = 219/259 | HIGH | `@0x80076274` + base (215,26) (RE1.5-EXE; A pixel-verified) | `main.c:2839-2844` vs `re15_menu.h:14` | Wave 1 fix (2px) |
| 8 | No equip cell (0xA) in nav; no equip/unequip/swap animations | HIGH | grid nav `0x800c62a0` (DEBUG.BIN module, savestate-RAM disasm): cursor jump to 0xA; anim FSMs: equip 12-step `@0x80010f24`, unequip 12-step `@0x80010f54`, swap 11-step inline `@0x8004b07c` (RE1.5-EXE) | nav has no 0xA; equip is instant | Waves 2 (nav) + 3 (anims; step bodies need disasm — §5) |
| 9 | No EXAMINE/CHECK | HIGH | command [1] → state 9 = `FUN_800c6630` (DEBUG.BIN): ITPS photo spin −207→+13 (+22/f), description via `FUN_80027e68(…,0x8400,id,…)` from bank `@0x800c50de` (glyph = code+0x24) | absent | Wave 4 |
| 10 | No COMBINE | HIGH | command [3] → state 7 `FUN_8004b33c @0x8004b33c`; 2nd cursor `FUN_80048904 @0x80048904` (`DAT_800b25be`); matcher `FUN_8004e900 @0x8004e900` over prop table `@0x80074da8` (4B pairs {partner,result,action,pic}); executor `FUN_8004e054 @0x8004e054` actions 1–6; full mix graph dumped `@0x80074c88-0x80074da7`; result anim 17-step `@0x80010ff4`; MIXITEM.PIX (file 0x19) (all RE1.5-EXE) | explicit no-op `menu_common.c:91` | Wave 5 |
| 11 | No "You cant use it here." path (ammo/keys/Red on USE) | MED | classifier `@0x8004aa64` → c3=6 → `LAB_8004b250 @0x8004b250`: msg `FUN_80027e68(0x00a80018,0x8400,0,0)`, no Yes/No (RE1.5-EXE) | ammo/key = silent no-op | Wave 3 |
| 12 | Icon palettes framebuffer-guessed, clut holes 0/4/7; hardcoded transparency set | MED | real CLUTs shipped in assets: ITPS per-block CLUT (0,489); ST_00 CLUTs (0,484+); STPIC (0,485) (report C) | `item_icon_common.c:28-51` | Wave 1: source palettes from TIMs |
| 13 | Icon tile map `0x15→37` vs proven identity mapping | MED | ITPS block N `+0x21a0` ≡ ITEMALL tile N for all non-weapons **incl. 0x15** (byte-compare, report C) | `item_icon_common.c:37` | Wave 1 after one re-verify (contradiction #6, §4) |
| 14 | `@0x80074a8c` misread as per-item clut table | MED | it is the 12B-stride **UI-panel** descriptor table (report C correction) | `item_icon_common.c` comment/use | Wave 1 refactor |
| 15 | No menu SEs | MED | `FUN_80045024 @0x80045024` bank 4: move=4, cancel=5, confirm=6, equip-cell=0x0a (call sites in report D) | none | Waves 2–3 |
| 16 | No open/close fades | MED | open: `FUN_800217b0(0x200,0x1800,7,0)` `@0x8001c9f8+`; screen fade-in `(0x200,-0x1800,7,0)` in `LAB_80049524` (RE1.5-EXE) | hard toggle | Wave 2 |
| 17 | Qty digits drawn in debug font at wrong sizes/colors | MED | 16x32 / 8x32 digit SPRTs (report B element table); color rule green-vs-cyan **UNRESOLVED** (§5) | `main.c:2853-2854` | Wave 1 geometry; color blocked on RE |
| 18 | Item box | LOW | shipped MZD behavior = **message-only AOT**: ROOM1150 sub07 msg 0x03 / ROOM30B0 sub02 msg 0x00 "Itembox is not available in this preview" (RDT bytes, report E). NO RE1.5 engine box exists (single array `@0x800b10ac`; AOT sce table `@0x8007469c` has no box type) | generic event-AOT path exists (`scd_vm.c:2072-2142`) | Wave 6: parity-verify the message fires; functional box = RE2 port per roadmap INV-5, separate campaign |
| 19 | Classifier comment mislabels (`0x8004b250` called "Yes/No", `0x80049124` called classifier) | LOW | real classifier `@0x8004aa64`; `0x8004b250` = plain message (report D) | `item_use_common.c`, `inventory_common.c` comments | Wave 3 comment fixes |
| 20 | Debug features (SELECT item spawner, held-0x10 refill) | LOW | spawner in `FUN_8004a0cc` (ITEMALL file 0x0b, clamp 0x47); refill `@0x8004974c` head via prop cap `@0x80074da8` | absent | optional, Wave 6 |

---

## 3. WAVE PLAN

Precondition for every wave (per STOP-GATE): constants land in code with their `@0x…`; acceptance is measured, not eyeballed.

**Wave 0 — table extraction (same session as wave 1 start).** Write a small extractor (build-time or checked-in `.inc`) that reads directly from `PSX.EXE` (file_off = 0x800+(addr−0x80010000)): panel master `@0x80074a8c`, geometries `@0x80075108`/`@0x80075240`/templates `@0x80075108-0x800755f8`, buttons `@0x80075390`, cells `@0x80076274`, icon uv `@0x80076244`, ECG waves `@0x80076214`→6 tables + colors `@0x8007622c`, prop table `@0x80074da8` + pair lists `@0x80074c88-0x80074da7`. Decode CLUTs from ST_00.TIM/STPIC/ITPS TIM blocks. **Acceptance:** extracted tables byte-match the report A/B dumps (cells (4,32)…(44,152)+( −66,88); buttons 32x16 at 94/126/158 x 166/182/198).

**Wave 1 — the visual screen (what the user sees).** Replace `main.c:2792-2881` with a data-driven composer: chrome group 0, ITEM LIST g1 (cells at **219/259**), ARMS g3 (Standard-Arms = cell-table entry 10 at (149,114)), ID g4 + STPIC_00 card, tab cluster g6 + L1/R1 sockets, cursor groups g5/g8/g9, ECG renderer `FUN_80048a44` port with real waveform/color tables, item icons from the identity map with real ITPS/ST_00 CLUTs, qty digits at the byte-true SPRT geometry. Respect OT order of `FUN_80049a5c` (earliest=topmost) via re15-pc-render-order. Also fix gaps 7/12/13/14.
**Acceptance:** reproduce the savestate state (3 items, equip slot 0, tab=FILE, `DAT_800b2600=0x62`, `DAT_800b25ca=1`) and pixel-compare against `shots/inv_original.png`. Target: exact match on all panel geometry/asset pixels; known-open pixels (qty digit colors, portrait prim anchor, exact ECG trace phase) listed explicitly, not hand-waved — resolve their §5 questions before declaring wave 1 done.

**Wave 2 — screen FSM, navigation, open/close.** Port the task shape: phases `@0x80074bdc` (init `FUN_800460b8` semantics / run / close), tab-select sub-state (`DAT_800b25c1`) with D-pad raw-edge mapping + L1/R1 shortcuts + confirm/cancel on `DAT_800ac76c` 0x4000/0x8000, ITEM-mode slide FSM (`DAT_800b25c2` states 0/1/2, ±0xe/frame on 25ea), grid nav per `0x800c62a0` (held pad, ±1/±2, equip-cell 0xA with saved-cursor restore — global per contradiction #4), open/close fades (gap 16), SEs (gap 15), START-close from grid. MAP/FILE tabs: enter + immediately-back stubs clearly marked UNVERIFIED-screen (MAP/FILE are separate screens `@0x80074c0c`/`FUN_800c6ca0`, out of campaign scope).
**Acceptance:** input-script test driving tab nav + grid nav; cursor globals sequence matches the RE'd transitions (state assertions in a ctest via re15-room-probe style harness); slide positions per-frame == ±0xe arithmetic; visual spot-check of cursor frames vs savestate.

**Wave 3 — command stage + USE semantics + equip anims.** 4-way command cluster (state 3/4: slide-in of g7, d6 select on raw edges, dispatch `@0x8004a73c`), classifier `@0x8004aa64` (weapons <0x15 → equip/unequip/swap; heals (id−0x22)<0xe && id≠0x25 → existing heal FSM; else c3=6 message `@0x8004b250`), equip/unequip/swap animation FSMs (12/12/11 steps; **requires the §5 step-table disasm first**), `DAT_800b25cd`/`2608`/`25dc/25de` visual registers, equip commit on close (`DAT_800aca5d @0x80046688` — already ported, rewire). Fix comment mislabels (gap 19).
**Acceptance:** scripted flows (equip from grid, unequip via cell 0xA, swap, heal, ammo→"You cant use it here.") assert the exact state-byte sequences; anim = per-frame register values equal the step tables; message text bytes == bank entry 0.

**Wave 4 — CHECK/examine.** `FUN_800c6630` 5-sub-state port: panel slide-out (−36/frame), ITPS photo spin −207→13 (+22/frame, 10 frames), description via the msg system (mode 0x8400, bank `@0x800c50de`, glyph=code+0x24 — extract bank from DEBUG.BIN/savestate dump `scratchpad/debugbin_ram.bin` into a shipped-asset-derived table), confirm/cancel → spin-out → back to command stage d6=1.
**Acceptance:** frame-counted spin positions; decoded description strings byte-match the bank for ids 0x00/0x15/0x30; pixel check of photo at (13,26).

**Wave 5 — EXCHANGE/combine.** Second cursor `FUN_80048904`, matcher `FUN_8004e900` (kind-byte normalization, pair walk), executor `FUN_8004e054` actions 1–6 (mix graph, reload-merge cap clamp from prop[+0], GL transform, self-stack, detonator/shells), compaction `FUN_8004dadc`, MIXITEM.PIX result icon ((pic−1)*0x4b0), 17-step result anim `@0x80010ff4` (needs §5 disasm).
**Acceptance:** ctest table-driven over the full dumped pair graph (G+R→0x27 etc., caps, unequal-qty splits); on-screen result icon/anim frame-counted.

**Wave 6 — closure.** (a) Item box shipped behavior: parity-verify ROOM1150 sub07 / ROOM30B0 sub02 (Cut_chg + message + Cut_old) end-to-end via re15-parity-verify — engine box is **out of scope** (RE2-only, §4). (b) ECG condition-change wipes (`DAT_800b25d4` effects) + Caution/Danger/Poison states live-verified. (c) Optional debug spawner/refill. (d) Resolve any §5 leftovers; final full-screen pixel-compare across ≥3 states (idle-FILE, ITEM grid open, command stage open — the latter needs a fresh savestate capture, §5 Q10).

---

## 4. CONTRADICTIONS → MUST-VERIFY (do not treat as fact)

1. **Slot byte +2 semantics:** B calls it "digit-count" (`@DAT_800b10ae`, ammo-counter x pos), C calls it icon "mode" (1=double-wide), D calls it "kind" (2=2-cell weapon tail). All three read the same byte. Re-disasm the three read sites (`LAB_80049524` ammo-x, `FUN_800492b8` mode, `FUN_8004e900` normalize) before modeling it. Savestate has 0 for all three items (A), so live data doesn't discriminate.
2. **`DAT_800b0fbc` value:** A read 0x0a from the savestate (=10 grid cells, matches cell table); C claims "16-slot cache, DAT_800b0fbc=0x10". Direct RAM read (A) wins provisionally; C's 0x10 is UNVERIFIED — re-read `FUN_8004dc4c`'s bound constant.
3. **Saved-cursor global for equip cell:** B says `DAT_800b25d7`; D says `DAT_800b25bf` — but 25bf is the phase byte (A: value 1 while open), so D is almost certainly a typo. Verify in the `0x800c62a0` disasm.
4. **Grid nav pad word:** B (and D's own pad-fact) = HELD `DAT_800ac760`; D's grid fact says edge `DAT_800ac762`. Verify; held is the working assumption.
5. **Command [2] (Down) semantics:** D "close whole screen" vs B "state 8 → slide-out → state 2 → back to tab select". Adopt B's more granular reading; verify state-2 exit target live.
6. **Icon tile map:** C's byte-proof identity (incl. 0x15) vs port/F's framebuffer-derived `0x15→37`. One decisive re-check (render tile 0x15 vs the savestate bullets sprite), then adopt identity.
7. **Portrait prim (group 10):** B's "(13,90) 112x72 uv(128,0)" vs A's ID-card measurement at y 21–93 — anchor/uv/role of g10 unclear; re-read the live prim buffer before drawing the STPIC.
8. **ITEM LIST panel x:** A measured 216, B prim table says 215 + A's own cell math uses 215 — 215 is the register value (`DAT_800b25e0=0xd7`); A's 216 is a 1px edge-measure artifact. Use 215.
9. **RE2-address quarantine (facts, per report E):** `FUN_80068f9c`, `FUN_80069714`, `FUN_80069668`, `DAT_800d4a3c`, `DAT_800d46ac`, `DAT_800d4a68`, `FUN_800703b8` = **RE2-Leon only** (0 hits in ghidra1_V2.txt). Roadmap INV-5's box citations are RE2. Also retired RE1.5 candidates: `0x80046670` (no role found), "FUN_80049124" (not a function — an instruction inside `FUN_80048f28`), dead prototype block `@0x8004a578-a648` (zero xrefs), dead grid LEFT-jump `@0x800c6384` (skipped by `beq zero,zero @0x800c637c`).
10. **ITEMALL runtime role:** B implies it's the gameplay icon source; C shows gameplay uses the ITPS block icon at grant and ITEMALL only for the debug picker. Since the bytes are identical, the port may read ITEMALL — but document the true mechanism, don't claim ITEMALL is the runtime path.

---

## 5. OPEN RE QUESTIONS (blockers, by wave)

**Blocks wave 1:**
1. Qty digit color rule (green vs cyan '15') — slot flags are 0; RE the `FUN_80048f28` draw path / a3 param (prop byte `@0x80074db0+id*12`?) . Until resolved, digits are the one declared non-matching region.
2. Portrait prim anchor/uv (contradiction #7) + STPIC index selection (u16 table `@0x80073ad8` has no static xref; only Leon→STPIC_00 proven). Wave 1 can hardcode STPIC_00 for Leon with the gap documented.
3. ST_00 runtime VRAM anomaly: big gun at real y~242 vs upload y=256; ST_00 rows mostly don't byte-match VRAM — find the second upload/copy before trusting ST_00 uv's blindly (B's live prim uv dump is the safety net).
4. CLUT-per-prim assignment beyond the master-table clut_idx (which of `DAT_800b2610[0..7]` each icon SPRT carries; ITPS (0,489) vs ST_00 (0,484) for grid icons).

**Blocks wave 2/3:**
5. Slot +2 byte semantics (contradiction #1). 6. Saved-cursor global (contradiction #3). 7. Grid R1 handler `@0x800c6560` (`25bf++`?) purpose. 8. Who sets d6=4 (cancel-wait entry `@0x8004a648`). 9. Full step-body disasm of the equip/unequip/swap/combine anim tables (`@0x80010f24`/`@0x80010f54`/inline `@0x8004b07c`/`@0x80010ff4`) — only entry/terminal steps read so far.

**Blocks wave 3/4:**
10. On-screen layout of the command stage (need a live capture in master state 4 — new savestate via re15-room-capture). 11. `FUN_80027e68` a1 bit 0x8000 consumption in the msg VM. 12. Pickup-grant icon pointer: where the +0x21a0 advance happens between the ITPS load and `FUN_8004dc4c` (`*(0x800ac77c)` passed = block start).

**Blocks wave 5:** 13. Combine pair-data anomalies (GLOCK pair {15,04,02} → result id 4; duplicate {04,04,03}) — confirm list-pointer mapping before shipping the table. 14. MIXITEM.PIX 14-tile content mapping (unverified art).

**Non-blocking:** CD file id 0x25 identity; unreachable screen-index-2 triplet (`LAB_8004d7cc` — dead code); RE15_MODE_PAUSE enum (no distinct pause found in RE1.5 — START = status screen; UNVERIFIED as designed-dead); in-game HUD ammo echo (S1-2, separate from this screen — original ARMS panel round-count readout existence itself unresolved, F open question).

**Key implementation files:** `re15_port/engine/src/{menu_common.c, inventory_common.c, item_use_common.c, item_icon_common.c, itps_common.c, item_modal_common.c}`, `re15_port/include/{re15_inventory.h, re15_menu.h}`, `re15_port/platform/pc/main.c` (L2792-2881 render block), `re15_port/platform/pc/src/render_pc.c`; ground truth `stage_saves/mzd_inv_open.sav`, acceptance image `shots/inv_original.png`, DEBUG.BIN RAM dump `scratchpad/debugbin_ram.bin`.