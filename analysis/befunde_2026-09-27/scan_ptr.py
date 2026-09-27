#!/usr/bin/env python3
"""Vollscan: alle 32-Bit-Worte, die auf eine Zieladresse zeigen (Funktionszeiger-Tabellen).
   usage: scan_ptr.py <ziel-hex> [datei] [rambase-hex] [skip-hex]
   RE2-EXE: scan_ptr.py 0x80042c64 info/re2leon/PSX.EXE 0x80010000 0x800
"""
import struct
import sys

tgt = int(sys.argv[1], 16)
path = sys.argv[2] if len(sys.argv) > 2 else "info/re2leon/PSX.EXE"
base = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0x80010000
skip = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0x800
d = open(path, 'rb').read()[skip:]
n = 0
for i in range(0, len(d) - 3, 4):
    if struct.unpack_from('<I', d, i)[0] == tgt:
        print("%08x: -> %08x" % (base + i, tgt))
        n += 1
print("-- %d Treffer" % n)
