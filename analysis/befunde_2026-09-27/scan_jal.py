#!/usr/bin/env python3
"""Roh-Scan: alle `jal <ziel>` in PSX.EXE (oder einem Overlay).

Aufruf: python scan_jal.py <ziel-hex> [datei] [ladeadresse-hex]
"""
import struct, sys

tgt = int(sys.argv[1], 16)
path = sys.argv[2] if len(sys.argv) > 2 else "info/Re1.5/PSX.EXE"
if len(sys.argv) > 3:
    base = int(sys.argv[3], 16); skip = 0
else:
    base = 0x80010000; skip = 0x800
data = open(path, "rb").read()
want = 0x0C000000 | ((tgt & 0x0FFFFFFF) >> 2)
for off in range(skip, len(data) - 3, 4):
    w = struct.unpack_from("<I", data, off)[0]
    if w == want:
        print("%08x: jal %08x" % (base + off - skip, tgt))
