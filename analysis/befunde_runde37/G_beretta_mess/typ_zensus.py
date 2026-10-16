#!/usr/bin/env python3
# Runde 37 Spur G (Nachbesserung 1) — Zensus der Gegner-Typen ueber alle ausgelieferten RDTs.
# Walker = re15_port/tools/aot_sce_census.py (opcode-genau, Sektionsende-Fixes 2026-08-19); hier nur ein
# anderer Sammler: Sce_em_set (0x44, 20 B), Typ = pc[2] (scd_vm.c op_sce_em_set `type = t->pc[2]`).
# Aufruf: python typ_zensus.py <baum>
import os, sys, glob
from collections import defaultdict
baum = sys.argv[1]
sys.path.insert(0, os.path.join(baum, 're15_port', 'tools'))
import aot_sce_census as A
recs = []
def collect(data, pc, op, room, script, installs):
    if op == 0x44:
        recs.append((room, script, pc, data[pc+1], data[pc+2], data[pc+3]))
A.collect = collect
root = os.path.join(baum, 're15_port', 'shared_assets', 'PSX')
for f in sorted(glob.glob(os.path.join(root, 'STAGE*', 'ROOM*.RDT'))):
    room = os.path.basename(f)[4:-4]
    data = open(f, 'rb').read()
    if len(data) < 0x48: continue
    main_s, sub_s = A.u32(data, 0x40), A.u32(data, 0x44)
    stats = dict(walked=0, total=0, residue=0, stops=[])
    for (sec, tag) in ((main_s, 'main'), (sub_s, 'sub')):
        if sec == 0 or sec >= len(data): continue
        send = A.rdt_section_end(data, sec)
        for (o, e, idx) in A.section_regions(data, sec, send):
            A.walk(data, sec + o, sec + e, room, '%s%02d' % (tag, idx), [], stats)
by = defaultdict(set); n = defaultdict(int)
for (room, sc, pc, slot, typ, beh) in recs:
    by[typ].add(room); n[typ] += 1
print('Sce_em_set-Saetze gesamt:', len(recs))
for t in sorted(by):
    print('Typ 0x%02x: %3d Saetze in %2d Raeumen: %s' % (t, n[t], len(by[t]), ' '.join(sorted(by[t]))))
