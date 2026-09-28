#!/usr/bin/env python3
"""RE2-Raumtexte (ENG = RDT-Offsetindex 14 @Datei 0x40; @0x800301dc `lw v0,64(v0)`)."""
import struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_re2_text import decode
def msgs(path):
    d = open(path, 'rb').read()
    offs = struct.unpack_from('<23I', d, 8)
    base = offs[14]
    if base == 0: return []
    first = struct.unpack_from('<H', d, base)[0]
    n = first // 2
    out = []
    for i in range(n):
        o = struct.unpack_from('<H', d, base + i*2)[0]
        s, e = decode(d, base + o)
        out.append((i, base + o, s))
    return out
if __name__ == '__main__':
    for room in sys.argv[1:]:
        p = 'info/re2leon/PL0/RDT/ROOM%s.RDT' % room
        for i, o, s in msgs(p):
            print('ROOM%s msg[%2d] @Datei 0x%05X: %s' % (room, i, o, s))
