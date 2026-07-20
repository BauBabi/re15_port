# Wave-5 build sheet — EXCHANGE/combine pair graph
(Contradiction-settlement agent, 2026-07-20. VERDICT: herb/medicine mixing EXISTS, fully wired
and reachable; the old wf_159c5115 "no combine" audit is WRONG on all three claims. Spot-checked
in-session: pair list @0x80074d54 bytes `25 27 01 01 24 28 01 02 26 29 01 03 ...` + prop row 0x24
{pair_ptr=0x80074D54, count=6} re-read from PSX.EXE. The port's embedded re15_inv_ui_tables.c
bytes match the EXE 1:1.)

## Reachability chain (raw MIPS, cited)
1. Command stage → state 7: inventory FSM switch(0x800b25c2) @0x8004a360 via jump table
   @0x8004a714 (10 states). State 4 @0x8004a458 = grid: held-pad 0x800ac762 bits
   0x2000/0x8000/0x4000/0x1000 set command byte 0x800b25d6 = 3/1/2/0; press-edge 0x800ac76c
   bit 0x4000 dispatches jr @0x8004a73c[cmd]; entry [3] = 0x8004a570: `ori v0,zero,0x7` →
   0x800b25c2=7. Secondary path @0x8004a578-0x8004a5f4 gates only on item_id != 0
   (@0x8004a5a4). NO item-class gate — herbs pass.
2. State 7 @0x8004a6ac: jal 0x8004b33c → sub-dispatch 0x800b25c3 via @0x80074c44:
   [0]=0x8004b37c (select), [1]=0x8004b408 (result anim, sltiu 0x11 = 17-step jump table
   @0x80010ff4).
3. Second cursor FUN_80048904 (0x800b25be): pure 2-col grid movement, bounds only
   (right +1 even col @0x80048924-6c, left −1 @0x8004897c, down +2 @0x800489c0, up −2
   @0x80048a08). ZERO item restriction — two herbs selectable.
4. Confirm (0x800ac76c bit 0x4000 @0x8004b3a4): jal 0x8004e900 (matcher) →
   jal 0x8004e054(action); action≠0 → sub-state 1 anim; action==0 (no pair) → state 6 exit.

## Matcher FUN_8004e900 @0x8004e900
Kind-byte normalization: if inv entry byte+2 (0x800b10ae+slot*4) == 2 (second cell of a 2-cell
weapon), use slot−1 (@0x8004e910-38). Rejects same-slot and empty partner. Indexes prop table
0x80074da8 + idA*12 ({cap u32@+0, pair_ptr@+4, kind u8@+8, pair_count u8@+9}; matcher reads
+4/+9 @0x8004e9cc-e8), linear-scans 4-byte pairs {partner,result,action,pic} for partner==idB
(@0x8004ea40-5c); on match writes result→0x800b25d5, pic→0x800b260c, RETURNS action. No class
check — herbs match.

## Executor FUN_8004e054, jump table @0x80011298 [0..6]
- 0 = no-op (0x8004e318).
- 1 @0x8004e1ac = MIX-MERGE: id←result, count=cntA+cntB into the LOWER slot; MIXITEM tile
  (pic−1)*1200 from the 0x19-file buffer uploaded via 0x800492b8 (@0x8004e224-44; file load
  FUN_80013b60(0x19,…) @0x8004e16c-74 when pic≠0); other slot cleared + compact 0x8004947c.
- 2 @0x8004e320 = RELOAD into A, cap clamp from prop[idA].cap (@0x8004e338-44): full →
  id[A]=result, B deleted; partial → count[A]=cap, count[B]=rest.
- 3 @0x8004e42c = mirror of 2 (merge into B, cap from prop[idB]) — ammo-selected-first.
- 4 @0x8004e538 = swap+transform (B←{result,cntA,kindA}, A←old B) — DORMANT: only pairs are
  GL 0x1a→0x10 / 0x1b→0x11 orphans @0x80074cb8/bc; owners 0x0f-0x11 have pair_count=0.
- 5 @0x8004e664 = self-stack (delegates to 2/3 tail by slot order).
- 6 @0x8004e680 = CRAFTING: equal counts → result replaces A, B deleted; unequal → result
  count=min in lower slot, leftover keeps source id (@0x8004e764-8d0).

## Herb mix graph (action 1; 0x24 GREEN / 0x25 RED / 0x26 BLUE)
| item | pair list @ | pairs {partner → result, pic} |
|---|---|---|
| 0x24 GREEN | 0x80074d54 (6) | +25→27 p1; +24→28 p2; +26→29 p3; +27→2a p7; +28→2b p5; +29→2c p8 |
| 0x25 RED | 0x80074d6c (4) | +24→27 p1; +27→2d p7; +28→2a p4; +29→2e p8 |
| 0x26 BLUE | 0x80074d7c (3) | +24→29 p3; +27→2e p8; +28→2c p4 |
| 0x27 G.R | 0x80074d88 (3) | +25→2d p7; +24→2a p4; +26→2e p8 |
| 0x28 G.G | 0x80074d94 (3) | +25→2a p4; +24→2b p5; +26→2c p6 |
| 0x29 G.B | 0x80074da0 (2) | +25→2e p8; +24→2c p6 |

Weapon/ammo/crafting rows: caps 15/6/7/12/100/250; detonator+capsule→grenades 0x09-0x0b;
empty shells+capsule→rounds 0x19-0x1b. ORPHAN pairs @0x80074cb0-cc8 (MC51 reload, GL
reload/transform, grenade self-reload) exist in data but unreachable (owner pair_count=0).
BYTE-TRUE data quirks to port as-is: GLOCK 18 reload result byte = 0x04 SIG P228
(@0x80074c99); asymmetric pic bytes for the same result (0x24+0x27→0x2a pic7 vs
0x27+0x24→0x2a pic4).

## MIXITEM.PIX
16800 bytes = exactly 14×1200 (40×30 8bpp) = pic ids 1-14. Decoded with ST_00.TIM CLUT row 0:
tiles 1-8 = medicine mix bottles, 9-11 = hand/acid/incendiary grenades, 12-14 =
explosive/acid/incendiary GL-round pouches.

## Why the old audit was wrong (lesson)
- "0x27-0x2e never produced": produced by executor action 1 via DATA bytes, never code
  immediates → constant-scans of code miss it.
- "No 2-item select": EXCHANGE (command 3) → state 7 with a free-roaming second cursor.
- "Only weapon+ammo reload": reload is merely actions 2/3/5 of the same six-action engine.
