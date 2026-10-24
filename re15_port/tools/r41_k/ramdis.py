#!/usr/bin/env python3
"""Runde 41 Spur K: MIPS-Disassembler fuer RAM-Bereiche AUSSERHALB der EXE (z.B. 0x800c0000+, zur Laufzeit geladen)
aus einem DuckStation-Savestate (re15_ss.Ram). Decoder = re15_disasm.dis_one (Skill re15-psx-disasm).
    python ramdis.py <savestate> <addr> <n> [bytes]
"""
import sys, os, importlib.util, struct
HIER = os.path.dirname(os.path.abspath(__file__))
BAUM = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(BAUM, '.claude', 'skills', 're15-savestate-ghidra', 'scripts'))
import re15_ss
spec = importlib.util.spec_from_file_location('dis', os.path.join(BAUM, '.claude', 'skills', 're15-psx-disasm', 'scripts', 're15_disasm.py'))
dis = importlib.util.module_from_spec(spec); spec.loader.exec_module(dis)
r = re15_ss.Ram(sys.argv[1]); a0 = int(sys.argv[2], 0); n = int(sys.argv[3], 0)
if len(sys.argv) > 4:
    b = r.bytes(a0, n)
    for i in range(0, n, 16): print('  %08x: %s' % (a0 + i, b[i:i+16].hex(' ')))
    sys.exit(0)
for i in range(n):
    a = a0 + 4 * i
    w = struct.unpack('<I', r.bytes(a, 4))[0]
    s, c = dis.dis_one(w, a)
    print('  %08x: %08x  %-32s %s' % (a, w, s, c or ''))
