#!/usr/bin/env python3
"""Runde 37 Spur N — Sce_em_set-Zensus (Opcode 0x44, 20 B) ueber alle 240 RDTs.

Handler @0x800420a0: pc[1] & 0x7f = Slot (s4 @0x800420f8), pc[2] = Typ (`lbu v0,0(s2)` @0x800422b4 ->
`sb v0,8(s0)` @0x800422bc), pc[6] = +0x97 (`lbu v1,4(s2)` @0x80042254 -> `sb v1,151(s0)` @0x8004225c).
Walker = re15_port/tools/scd_dump_room.py (opcode-exakt). Raum-Id-Endziffer 1 = Elza, 0 = Leon.
"""
import os, sys, struct, re, subprocess, collections
BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
PSX = os.path.join(BAUM, 're15_port/shared_assets/PSX')
DUMP = os.path.join(BAUM, 're15_port/tools/scd_dump_room.py')
rx = re.compile(r'0x([0-9A-F]+) \(\+[0-9A-F]+\) Sce_em_set\s+((?:[0-9a-f]{2} ){19}[0-9a-f]{2})')
rows = []
for st in range(1, 7):
    d = os.path.join(PSX, 'STAGE%d' % st)
    for fn in sorted(os.listdir(d)):
        if not (fn.startswith('ROOM') and fn.endswith('.RDT')): continue
        out = subprocess.run([sys.executable, DUMP, os.path.join(d, fn)], capture_output=True, text=True).stdout
        seen = set()
        for m in rx.finditer(out):
            off = int(m.group(1), 16)
            if off in seen: continue
            seen.add(off)
            b = bytes(int(x, 16) for x in m.group(2).split())
            rows.append((fn[4:8], off, b[1] & 0x7f, b[2], b[6], b))
by = collections.defaultdict(list)
for r in rows: by[r[3]].append(r)
for t in sorted(by):
    el = [r for r in by[t] if r[0][-1] == '1']; le = [r for r in by[t] if r[0][-1] != '1']
    print('Typ 0x%02X: %d Saetze (Leon %d, Elza %d)' % (t, len(by[t]), len(le), len(el)))
    print('   Elza: ' + ', '.join('%s@0x%05X s%d +97=%d' % (r[0], r[1], r[2], r[4]) for r in el))
    print('   Leon: ' + ', '.join('%s@0x%05X s%d +97=%d' % (r[0], r[1], r[2], r[4]) for r in le))
