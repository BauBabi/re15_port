#!/usr/bin/env python3
"""spawn_zensus.py — alle Sce_em_set (SCD-Opcode 0x44) in allen RE1.5-RDTs (shared_assets/PSX/STAGE1..6),
opcode-exakt ueber re15_port/tools/scd_walk_lib.py. Gibt je Gegnertyp die Raeume (+Offset, Satz-Bytes) aus.
Satzlage wie scd_vm.c op_sce_em_set: +1 Slot, +2 Typ, +3 Verhalten/grid, ... (Byte-Beleg im Port-Kommentar
ROOM1220.RDT @0x0F2A `44 00 16 81 ...`)."""
import os, sys, glob, collections
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W
CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
typen = collections.defaultdict(list)
for st in range(1, 7):
    for p in sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT'))):
        d = open(p, 'rb').read()
        if len(d) < 0x48: continue
        try:
            reg = W.regionen(d)
        except Exception as ex:
            continue
        for (tag, idx), ops in reg.items():
            for pc, op, sz in ops:
                if op == 0x44:
                    typ = d[pc + 2]
                    typen[typ].append((os.path.basename(p)[:-4], tag, idx, pc, d[pc:pc + sz].hex(' ')))
want = [int(x, 16) for x in sys.argv[1:]] or sorted(typen)
for t in want:
    rows = typen.get(t, [])
    raeume = sorted(set(r[0] for r in rows))
    print(f'Typ 0x{t:02x}: {len(rows)} Saetze, Raeume {raeume}')
    if '-v' in os.environ.get('ZENSUS', ''):
        for r in rows: print('   ', r)
