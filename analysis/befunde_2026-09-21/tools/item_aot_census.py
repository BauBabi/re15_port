#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Census of every shipped RE1.5 Item_aot_set record: which item id, and does it
carry a visible prop model (tk_prop) or not (0xFF)?

Record layout is the port's own, byte-true-audited one
(re15_port/engine/src/scd_vm.c op_item_aot_set, LAB_80040644):
  +0 op 0x50 | +1 slot | +2 sce | +3 sat | +4 floor | +5 super
  +6 x s16 | +8 z s16 | +10 w s16 | +12 d s16
  short form (sat & 0x80 == 0, 22 bytes): +14 type u16le, +16 amount u16le,
                                          +18 taken-bit, +20 tk_prop
  long  form (sat & 0x80, 30 bytes):      +22 type, +24 amount, +26 bit, +28 prop

Ground truth to check the scan against: the port states the shipped total is
164 records, all action-gated (sat 0x31 x160 + 0x51 x4) — scd_vm.c, census
re-taken 2026-08-19 with the corrected walker.
"""
import os, struct, collections, sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else \
    r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\Re1.5\PSX"
NAMES = {}
# RE1.5 item names from PSX/BIN/DEBUG.BIN file@0x4A29.. (0x07-separated),
# id 1 = 'Combat Knife'.  Anchors: 0x15 = H.Gun Bullets (ROOM1050 @0xb9a),
# 0x31 = Fire Extinguisher (ROOM1000 @0xc26) — both cross-checked in the port.
DBG = os.path.join(ROOT, 'BIN', 'DEBUG.BIN')
if os.path.exists(DBG):
    d = open(DBG, 'rb').read()
    DECT = {0x00: ' ', 0x01: '.', 0x3A: "'", 0x3B: '-', 0x38: '/'}
    for i in range(10):
        DECT[0x0C + i] = str(i)
    for i in range(26):
        DECT[0x1D + i] = chr(ord('A') + i)
    for i in range(26):
        DECT[0x3D + i] = chr(ord('a') + i)
    off = 0x4A29
    idn = 1
    cur = []
    while off < 0x4F90 and idn < 0x80:
        b = d[off]
        if b == 0x07:
            NAMES[idn] = ''.join(cur)
            idn += 1
            cur = []
        else:
            cur.append(DECT.get(b, '<%02X>' % b))
        off += 1

rows = []
for st in sorted(os.listdir(ROOT)):
    sd = os.path.join(ROOT, st)
    if not os.path.isdir(sd) or not st.upper().startswith('STAGE'):
        continue
    for fn in sorted(os.listdir(sd)):
        if not fn.upper().endswith('.RDT'):
            continue
        p = os.path.join(sd, fn)
        d = open(p, 'rb').read()
        if len(d) < 0x60:
            continue
        nOmodel = d[2]
        offs = list(struct.unpack_from('<22I', d, 8))
        scd_lo = min(x for x in (offs[(0x40 - 8) // 4], offs[(0x44 - 8) // 4],
                                 offs[(0x48 - 8) // 4]) if x) if any(
            (offs[(0x40 - 8) // 4], offs[(0x44 - 8) // 4], offs[(0x48 - 8) // 4])) else 0
        scd_hi = offs[(0x4C - 8) // 4] or len(d)
        o = scd_lo
        while o + 30 <= min(scd_hi, len(d)):
            if d[o] == 0x50 and d[o + 3] in (0x31, 0x51, 0xB1, 0xD1):
                lf = (d[o + 3] & 0x80) != 0
                base = 22 if lf else 14
                it = d[o + base]
                hi = d[o + base + 1]
                am = d[o + base + 2]
                tkb = d[o + base + 4]
                tkp = d[o + base + 6]
                prop_ok = (tkp == 0xFF) or (tkp < nOmodel)
                if hi == 0 and 1 <= it <= 0x6A and am <= 200 and prop_ok:
                    rows.append({'room': fn[:-4], 'stage': st, 'off': o,
                                 'sat': d[o + 3], 'slot': d[o + 1], 'sce': d[o + 2],
                                 'item': it, 'amount': am, 'tk_bit': tkb,
                                 'tk_prop': tkp, 'nOmodel': nOmodel, 'long': lf})
                    o += 30 if lf else 22
                    continue
            o += 1

print('Item_aot_set records found : %d' % len(rows))
print('  sat 0x31 : %d   sat 0x51 : %d   long form : %d'
      % (sum(1 for r in rows if r['sat'] == 0x31),
         sum(1 for r in rows if r['sat'] == 0x51),
         sum(1 for r in rows if r['long'])))
print('  with tk_prop 0xFF (no world model) : %d'
      % sum(1 for r in rows if r['tk_prop'] == 0xFF))
print('  with a real tk_prop                : %d'
      % sum(1 for r in rows if r['tk_prop'] != 0xFF))
print()
byitem = collections.defaultdict(list)
for r in rows:
    byitem[r['item']].append(r)
print('%-5s %-26s %5s %7s %s' % ('id', 'name', 'count', 'noModel', 'rooms (prop)'))
for it in sorted(byitem):
    lst = byitem[it]
    nof = sum(1 for r in lst if r['tk_prop'] == 0xFF)
    where = ' '.join('%s@0x%X/p%s' % (r['room'], r['off'],
                                      'FF' if r['tk_prop'] == 0xFF else str(r['tk_prop']))
                     for r in lst[:6])
    print('0x%02X  %-26s %5d %7d  %s' % (it, NAMES.get(it, '?')[:26], len(lst), nof, where))
