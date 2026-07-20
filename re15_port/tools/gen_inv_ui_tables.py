#!/usr/bin/env python3
# gen_inv_ui_tables.py - regenerate engine/src/re15_inv_ui_tables.c: the VERBATIM RE1.5
# inventory/status-screen UI data region from PSX.EXE (Wave 0 of the S1-3 campaign,
# RE campaign wf_b4a239f2 / plan shots/inv_plan.md). Run from the repo root:
#   python re15_port/tools/gen_inv_ui_tables.py
#
# Pattern: like gen_trig_lut.py, the EXE data is embedded VERBATIM (s_gait_blob pattern,
# CLAUDE.md RE-Disziplin) so no table is re-typed by hand. One contiguous blob covers the
# whole status-screen data region; consumers index it via the RE15_INV_* offset macros in
# re15_inv_ui.h, each carrying its @0x address.
#
# Region [0x80074A8C, 0x800762A0) in RE1.5 PSX.EXE (all RE1.5-EXE, campaign-verified +
# spot-checked against the live bytes 2026-07-20):
#   @0x80074A8C  panel MASTER table (stride 0xC: {clut_idx u16, prim_count u16,
#                template_ptr u32, prim_buf_ptr u32}) - screen prim groups (builder
#                FUN_800467a8, animator FUN_80047648, draw chain FUN_80049a5c)
#   @0x80074BDC  screen task phase table (task LAB_8004603c)
#   @0x80074C88  combine PAIR lists (4B {partner,result,action,pic}) .. 0x80074DA7
#   @0x80074DA8  per-item PROP table (stride 12; combine matcher FUN_8004e900)
#   @0x80075108  prim geometry TEMPLATES (chrome 26 SPRTs) .. @0x800755F8 (groups)
#   @0x80075240  ARMS group geometry (15 rows)
#   @0x80075390  button table (stride 0xC {dx,dy,w,h,u,v}; 32x16 buttons)
#   @0x80075774  ECG waveform tables (targets of the pointer table @0x80076214:
#                0x80075774/592C/5AE4/5CEC/5EA4/605C, deltas 0x1B8/0x1B8/0x208/
#                0x1B8/0x1B8; the last table ends EXACTLY at the pointer table
#                0x8007605C+0x1B8 == 0x80076214 -> the region is self-closing)
#   @0x80076214  ECG waveform POINTER table (6 u32, per condition)
#   @0x8007622C  ECG trace colors (4 conditions x 4B; Fine = 20 ff 20)
#   @0x80076244  item-icon UV table
#   @0x80076274  item grid CELL table (11 x s16 pair: 10 cells x{4,44} y 32..152
#                pitch 30 + entry 10 = (-66,88) = the Standard-Arms box)
#
# SECOND region [0x800762A0, 0x80076C00) - the MAP-tab data (MAP wave, FUN_8004c058
# family; all bounds from raw disasm this wave). Contiguous sub-tables:
#   @0x800762A0  the 14 room-rect LISTS (stride-12 entries {u16 x,y,w,h; u8 u @+8;
#                u8 v @+10} per FUN_80046fd8 @0x8004731c-60, walk stride
#                @0x800473bc/@0x800473c8). List 13 @0x80076834 (count 1) ends EXACTLY
#                at the pair table 0x80076840 (self-closing).
#   @0x80076840  per-map-page room-rect PAIR table, 14 x {u16 count, u16 pad, u32 ptr}
#                (reader FUN_80046fd8 @0x80047048-70: lhu 0x80076840+page*8 = count,
#                lw 0x80076844+page*8 = list ptr)
#   @0x800768B0  marker scale rows, stride 8 {u16 x_off, y_off, x_scale, z_scale}
#                indexed by DAT_800b260d = the global room-slot index 0..105
#                (marker math FUN_800473f8 @0x800474e8/@0x80047518/@0x80047444/
#                @0x8004747c; builder FUN_80046fd8 @0x80047000-0c/@0x8004707c-88).
#                106 rows: max 260d = stage-6 base 98 (@0x8004c044 addiu +98) + room
#                bound 8 (@0x8004bf7c sltiu 0x8) - 1 = 105; the bytes at 0x80076C00
#                are ASCII text of the NEXT data blob (region end proven by content).
import struct, sys, os

EXE   = 'info/Re1.5/PSX.EXE'
BASE  = 0x80074A8C           # panel master table (first byte of the region)
END   = 0x800762A0           # EXACT end: grid cell table 0x80076274 + 11*4 = 0x2C bytes
FOFF  = 0x800 + (BASE - 0x80010000)
SIZE  = END - BASE
BASE2 = 0x800762A0           # first rect list (== region-1 END: the tables are adjacent)
END2  = 0x80076C00           # EXACT end: scale row 105 @0x800768B0 + 106*8
FOFF2 = 0x800 + (BASE2 - 0x80010000)
SIZE2 = END2 - BASE2
OUT   = 're15_port/engine/src/re15_inv_ui_tables.c'

with open(EXE, 'rb') as f:
    f.seek(FOFF)
    blob = f.read(SIZE)
    f.seek(FOFF2)
    blob2 = f.read(SIZE2)
assert len(blob) == SIZE, 'short read'
assert len(blob2) == SIZE2, 'short read (region 2)'

def u16(addr):
    o = addr - BASE
    return struct.unpack_from('<H', blob, o)[0]
def s16v(addr):
    o = addr - BASE
    return struct.unpack_from('<h', blob, o)[0]
def u32(addr):
    o = addr - BASE
    return struct.unpack_from('<I', blob, o)[0]
def bts(addr, n):
    o = addr - BASE
    return blob[o:o+n]
def m16(addr):
    o = addr - BASE2
    return struct.unpack_from('<H', blob2, o)[0]
def m32(addr):
    o = addr - BASE2
    return struct.unpack_from('<I', blob2, o)[0]

# ---- self-checks: the byte-exact values spot-checked in-session (2026-07-20) ----
# cell table @0x80076274: (4,32),(44,32),...,(44,152), entry 10 = (-66,88)
cells = [(s16v(0x80076274 + i*4), s16v(0x80076274 + i*4 + 2)) for i in range(11)]
assert cells[0] == (4, 32) and cells[1] == (44, 32), cells[:2]
assert cells[9] == (44, 152) and cells[10] == (-66, 88), cells[9:]
# ECG colors @0x8007622C: Fine = 20 ff 20
assert bts(0x8007622C, 3) == b'\x20\xff\x20', bts(0x8007622C, 4).hex()
# button table @0x80075390 entry 0 = {0,0,32,16,16,128}; entry 1 dx = -32
assert [u16(0x80075390 + i*2) for i in range(6)] == [0, 0, 32, 16, 16, 128]
assert s16v(0x8007539C) == -32
# panel master row 0 @0x80074A8C: clut 0, count 26 (chrome), tmpl @0x80075108, prim @0x8019B000
assert u16(0x80074A8C) == 0 and u16(0x80074A8E) == 26
assert u32(0x80074A90) == 0x80075108 and u32(0x80074A94) == 0x8019B000
# ECG pointer table @0x80076214: targets 0x80075774..0x8007605C, all inside the blob;
# the last waveform ends EXACTLY at the pointer table (self-closing region).
ecg_ptrs = [u32(0x80076214 + i*4) for i in range(6)]
assert ecg_ptrs == [0x80075774, 0x8007592C, 0x80075AE4, 0x80075CEC, 0x80075EA4, 0x8007605C], \
    [hex(p) for p in ecg_ptrs]
assert all(BASE <= p < END for p in ecg_ptrs), 'ECG waveform target outside blob'
assert ecg_ptrs[5] + 0x1B8 == 0x80076214, 'waveform region no longer self-closing'
print('inv-ui blob self-check OK (%d bytes)' % SIZE, file=sys.stderr)

# ---- region-2 self-checks (raw values dumped in-session, MAP wave 2026-07-21) ----
# pair table @0x80076840: 14 x {u16 count, pad, u32 ptr} (reader @0x80047048-70)
map_cnt = [m16(0x80076840 + i*8) for i in range(14)]
map_ptr = [m32(0x80076844 + i*8) for i in range(14)]
assert map_cnt == [7, 10, 11, 10, 7, 2, 11, 14, 15, 15, 4, 4, 8, 1], map_cnt
assert map_ptr == [0x800762A0, 0x800762F4, 0x8007636C, 0x800763F0, 0x80076468,
                   0x800764BC, 0x800764D4, 0x80076558, 0x8007660C, 0x800766C0,
                   0x80076774, 0x800767A4, 0x800767D4, 0x80076834], \
    [hex(p) for p in map_ptr]
# every list (stride-12 entries) lies inside region 2; the lists run contiguously and
# list 13 ends exactly at the pair table (self-closing)
assert all(BASE2 <= p and p + c*12 <= 0x80076840 for c, p in zip(map_cnt, map_ptr)), \
    'rect list outside region 2'
assert map_ptr[13] + map_cnt[13]*12 == 0x80076840, 'lists no longer close into pair table'
# lists are ascending and non-overlapping (one shipped 12-byte gap after list 7:
# 0x80076558 + 14*12 = 0x80076600, list 8 starts 0x8007660C)
assert all(map_ptr[i] + map_cnt[i]*12 <= map_ptr[i+1] for i in range(13)), \
    'rect lists overlap'
# marker scale rows @0x800768B0 stride 8: row 0 = {0,0,1,1}, row 2 = {100,136,2287,2287},
# row 101 (stage-6) = {243,137,2161,2304}; page-4 list entry 0 = {127,137,16,16,168,40}
assert [m16(0x800768B0 + i*2) for i in range(4)] == [0, 0, 1, 1]
assert [m16(0x800768C0 + i*2) for i in range(4)] == [100, 136, 2287, 2287]
assert [m16(0x800768B0 + 101*8 + i*2) for i in range(4)] == [243, 137, 2161, 2304]
assert [m16(0x80076468 + i*2) for i in range(6)] == [127, 137, 16, 16, 168, 40]
print('inv-map blob self-check OK (%d bytes)' % SIZE2, file=sys.stderr)

with open(OUT, 'w', newline='\n') as f:
    f.write('/* re15_inv_ui_tables.c - VERBATIM RE1.5 status/inventory-screen UI data region\n'
            ' * [0x%08X, 0x%08X) of PSX.EXE (%d bytes).\n'
            ' * AUTO-GENERATED by re15_port/tools/gen_inv_ui_tables.py - do not hand-edit.\n'
            ' * Table map + citations: re15_port/include/re15_inv_ui.h and the generator header.\n'
            ' * RE: campaign wf_b4a239f2 (shots/inv_plan.md), spot-checked 2026-07-20. */\n'
            '#include "re15_inv_ui.h"\n\n' % (BASE, END, SIZE))
    f.write('const unsigned char re15_inv_ui_blob[RE15_INV_UI_BLOB_SIZE] = {\n')
    for i in range(0, SIZE, 16):
        row = ','.join('0x%02x' % b for b in blob[i:i+16])
        f.write('    ' + row + ',\n')
    f.write('};\n')
    f.write('\n/* MAP-tab data region [0x%08X, 0x%08X) (%d bytes): 14 room-rect lists\n'
            ' * @0x800762A0 + per-page pair table @0x80076840 + 106 marker scale rows\n'
            ' * @0x800768B0 (all verbatim; see the generator header for citations). */\n'
            % (BASE2, END2, SIZE2))
    f.write('const unsigned char re15_inv_map_blob[RE15_INV_MAP_BLOB_SIZE] = {\n')
    for i in range(0, SIZE2, 16):
        row = ','.join('0x%02x' % b for b in blob2[i:i+16])
        f.write('    ' + row + ',\n')
    f.write('};\n')
print('wrote %s' % OUT, file=sys.stderr)
