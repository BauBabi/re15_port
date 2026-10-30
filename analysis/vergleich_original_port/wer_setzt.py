#!/usr/bin/env python3
"""wer_setzt.py BIN OFFSET WERT : findet `li r,WERT` gefolgt (<=6 Befehle) von `sb r,OFFSET(x)`."""
import struct, sys, os
p, off, val = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0)
d = open(p, 'rb').read(); hdr = 0x800 if p.endswith('.EXE') else 0
base = 0x80010000 if hdr else 0x80100000
w = struct.unpack_from('<%dI' % ((len(d) - hdr) // 4), d, hdr)
for i, x in enumerate(w):
    op = x >> 26
    if op in (0x09, 0x0d) and (x >> 21) & 31 == 0 and (x & 0xffff) == val:
        r = (x >> 16) & 31
        for j in range(i + 1, min(i + 7, len(w))):
            y = w[j]
            if y >> 26 == 0x28 and (y >> 16) & 31 == r and (y & 0xffff) == off:
                print('0x%08x li %d -> sb @0x%08x' % (base + i * 4, val, base + j * 4)); break
            if (y >> 16) & 31 == r and y >> 26 in (0x09, 0x0d, 0x0f, 0x23, 0x24): break
