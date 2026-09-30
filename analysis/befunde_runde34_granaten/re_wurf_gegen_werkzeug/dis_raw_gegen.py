#!/usr/bin/env python3
"""Gegenpruefung R34: disassembliert eine rohe RAM-Datei (aus ss_dump_gegen.py) mit dis_one aus re15_disasm.py.
Aufruf: dis_raw_gegen.py <bin> <basisaddr> <startaddr> <n>"""
import sys, os, struct
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', '.claude', 'skills', 're15-psx-disasm', 'scripts'))
import re15_disasm as D
d = open(sys.argv[1], 'rb').read(); base = int(sys.argv[2], 16); a = int(sys.argv[3], 16); n = int(sys.argv[4])
for k in range(n):
    x = a + 4 * k; w = struct.unpack_from('<I', d, x - base)[0]
    print('  %08x: %08x  %s' % (x, w, D.dis_one(w, x)))
