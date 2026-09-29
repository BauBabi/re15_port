#!/usr/bin/env python3
"""Runde 33 / Thema K: wer liest/schreibt in RE1.5 das Flag (Bank,Bit) in ALLEN 240 RDTs?
RE1.5 Ck 0x21 {op,bank,bit,wert} / Set 0x22 {op,bank,bit,op} (Laengen aus scd_walk_lib = scd_vm.c).
Aufruf: r33_re15_flag_zensus.py <bank> <bit> [...]"""
import sys, os, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as L
pairs = [(int(sys.argv[i]), int(sys.argv[i + 1])) for i in range(1, len(sys.argv), 2)]
for f in sorted(glob.glob(os.path.join(REPO, 're15_port/shared_assets/PSX/STAGE*/ROOM*.RDT'))):
    d = open(f, 'rb').read()
    if len(d) < 0x100: continue
    room = os.path.basename(f)[4:8]
    for (tag, idx), ops in sorted(L.regionen(d).items()):
        for pc, op, sz in ops:
            if op in (0x21, 0x22) and (d[pc + 1], d[pc + 2]) in pairs:
                print('ROOM%s %s%02d @0x%05X %s(%d,%d,%d)  [%s]' % (room, tag, idx, pc,
                      'Ck ' if op == 0x21 else 'Set', d[pc + 1], d[pc + 2], d[pc + 3], d[pc:pc + sz].hex(' ')))
