# AOT sce-type census — RE1.5 game-wide (2026-07-21)
(Census agent; raw-MIPS handler enumeration + opcode-exact walk of ALL 240 RDTs, 100.00% code-byte
coverage, 0 stops; script vendored as re15_port/tools/aot_sce_census.py; full JSON matrix in the
session scratchpad census.json. Verification cross-checks: sce8=32, sce7=4, sce4-via-0x2C=24 — each
matches the port's independently derived comments.)

## Dispatch (byte-true)
- Installers: Aot_set 0x2C (@0x80040534), Door_aot_set 0x3B (@0x800405bc), Item_aot_set 0x50
  (@0x80040644) — all store pc+2 into DAT_800ac9b0[pc[1]] (@0x80040580/0x80040608); record[0]=sce.
- Scan FUN_80042bac from FUN_800436a8 (3 pools: player mask1, enemies mask2 stride 0x1F4, OBJECTS
  mask4 stride 0x94). rec[1]: bit0/1/2 pool, bit4 action-vs-auto, bit5 forward-probe @620
  (0x26c @0x80042bd0), bit6 center, bit7 4-vertex polygon (FUN_80014368) else AABB (FUN_80042b64).
  rec[2]=floor (0x80 any, else == entity+0x82). Payload rec+0xC (rect) / rec+0x14 (poly).
  Latches: DAT_800b0fd0 fwd-hit slot, DAT_800b0fd2 center-hit, entity+0xB = slot. Then
  `jalr PTR_LAB_8007469c[rec[0]]` @0x80042f7c-90 — NO bounds check.
- **The table has 14 entries** ([12] @0x800746cc→0x80043500, [13] @0x800746d0→0x800435cc).
- Aot_reset 0x46 (@0x80040738) RETYPES a live record: rec[0]=pc[2] (@0x80040764), rec[1]=
  (rec[1]&0x80)+pc[3] (@0x8004076c-78), payload rec+0xC/+0xE/+0x10 = pc[4..9] (@0x80040788-a8).
- Aot_on 0x47 (@0x800407bc, jump slot 0x800745c4) FIRES the slot's handler IMMEDIATELY
  (jalr @0x8004082c).

## The 14 handlers
| sce | @ | semantics |
|---|---|---|
| 0 | 0x8004305C | NONE/inert (restores work-var latch from snapshot 0x800bbde8/eec) |
| 1 | 0x80043084 | MESSAGE: FUN_80027e68(0,0x300,msg u16+0, u16+2<<16) |
| 2 | 0x800430BC | DOOR: DAT_800ac9a8=payload, DAT_800b5359=1, 0x800aca40|=0xff000000 |
| 3 | 0x800430F0 | EVENT: FUN_8003ee3c(cond u16+0, sub u8+3) |
| 4 | 0x80043120 | FLAG_CHG: bank table @0x80074664, MSB-first bit |
| 5 | 0x8004318C | pure NOP marker (effect = the scan's work-var/entity+0xB latch) |
| 6 | 0x80043194 | EVENT-SCHEDULE: aca3c|=0x8000, 0x800b25c0=1, 25cc=u8+0 (consumer @0x8004c670) — DORMANT |
| 7 | 0x800431CC | RAMP-Y: dir u16+0, y=(dist/s16+2 +1)*s16+4 → +0x38/+0x1BA |
| 8 | 0x8004330C | WATER: entity+0x88 = u16+0 per frame |
| 9 | 0x80043328 | ITEM: latch 0x80072d3b, type→0x800b8523/0x800afbb6, rec→0x800aca30 |
| 10 | 0x80043370 | DAMAGE zone — DORMANT (spec below) |
| 11 | 0x800434B4 | DISPLACEMENT/conveyor — DORMANT (spec below) |
| 12 | 0x80043500 | STAIR-X: side by (x−x1)<w/2; entity+5=0x0B,+6=1/2, aca3c|=0x40, +0x9E=u8+2 |
| 13 | 0x800435CC | STAIR-Z: same on z; +0x9E = u8+2 | 0x80 (axis flag @0x80043688) |

## Census matrix (2223 installs: 0x2C 1033 / 0x3B 644 / 0x50 162 / 0x46-retype 384; +73 Aot_on)
| sce | total | rooms | notes |
|---|---|---|---|
| 0 | 262 | 114 | inert placeholders + 188 disables |
| 1 | 511 | 141 | 8 polygon (0xB1) |
| 2 | 639 | 200 | 607 via 0x3B + 32 retypes |
| 3 | 275 | 122 | ev 2..26 always |
| 4 | 26 | 12 | incl. 10 enemy-only (0x42) |
| 5 | 196 | 57 | **142 = object-pool auto (0x44)** box-on-zone detectors |
| 7 | 4 | 2 | ROOM5060/5061 |
| 8 | 32 | 16 | all pools (0x47) |
| 9 | 168 | 62 | 160 via 0x50, 8 retypes |
| 12/13 | 58/52 | 18/12 | stairs |
| **6/10/11** | **0** | — | **NEVER shipped → dormant census CLOSED** |

## USED-but-divergent port mechanisms (the 4 implementation leads)
1. **sce-0 fresh installs armed by the port** (74; incl. 37 sce-0 DOORS of which only 16 ever get
   retyped to 2) → **21 doors permanently INERT in the original but LIVE in the port** (e.g. 1250
   slots 0-2, 1180 slot 4, 5070 slots 1-2). op_door_aot_set/op_item_aot_set never read pc[2].
2. **Aot_reset retype payloads** (196 non-zero retypes): original overwrites sce+flags+payload
   (32 door-dest updates, 8 item re-arms rooms 1190/1191+40A0/40A1, 67 event cond/sub rewrites,
   2 flag param writes); the port's re15_aot_reassert_event keeps OLD params.
3. **Aot_on 0x47 = FIRE-NOW** (73 uses, e.g. ROOM1240 sub02 slot 0, ROOM1080 sub07-10): the
   original dispatches the handler once immediately; the port only sets active=1.
4. **sce-5 object-pool markers** (142 @0x44): original = per-frame OBJECT-presence latch into
   work_vars[1] via the object-pool scan; the port installs them as player-action
   EXAMINE_WORKVAR (+ 8 sce-5 ev=0xFF installs collide with the port's MESSAGE enum).

## Dormant specs (for the record; NOT to implement)
- sce 10 DAMAGE: gate entity[0x93]&1 → return 1; else HP(+0x9a) −= u16+2; HP<0 → state 3 death
  (+5/6/7=0) else state 2 hurt (+5=u8+0 kind); knockback: atan2_q12 from rect center, speed
  +0x8c=0xC8, FUN_800245d8(angle−rot_y). Level-triggered, any pool.
- sce 11 DISPLACEMENT: x+=u16+0, y+=u16+2, z+=u16+4 — **lhu UNSIGNED adds** (a "negative" delta
  adds ~+65535 — byte-true quirk). Per-frame while inside.
- sce 6: event-schedule zone (consumer @0x8004c670).
