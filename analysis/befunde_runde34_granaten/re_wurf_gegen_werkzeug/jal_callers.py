#!/usr/bin/env python3
"""Gegenpruefung R34: findet alle `jal <ziel>` in PSX.EXE und STAGE*.BIN (+ optional RAM-Dump).
Aufruf: jal_callers.py 0x8001f278 [ram.bin]"""
import struct, sys, os, glob
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
tgt = int(sys.argv[1], 16); w = 0x0c000000 | ((tgt >> 2) & 0x3ffffff)
def scan(name, base, data):
    for i in range(0, len(data) - 3, 4):
        if struct.unpack_from('<I', data, i)[0] == w: print('%-12s jal %08x @%08x' % (name, tgt, base + i))
d = open(os.path.join(REPO, 'info/Re1.5/PSX.EXE'), 'rb').read(); t_addr, t_size = struct.unpack_from('<II', d, 0x18)
scan('PSX.EXE', t_addr, d[0x800:0x800 + t_size])
for p in sorted(glob.glob(os.path.join(REPO, 'info/Re1.5/PSX/BIN/*.BIN'))): scan(os.path.basename(p), 0x80100000, open(p, 'rb').read())
if len(sys.argv) > 2:
    r = open(sys.argv[2], 'rb').read(); scan('RAM>EXE', 0x80000000 + 0xbf000, r[0xbf000:0x100000])
