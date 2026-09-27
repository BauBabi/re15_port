#!/usr/bin/env python3
"""Zeigt zu jedem Store auf einen Offset die 4 Instruktionen davor (nur die
   Bit-Operationen), um Band-Schreiber (ori 0x2000/0x4000/0x8000) zu finden.
   usage: ctx_stores.py <bin> <rambase-hex> <skip-hex> <offset>
"""
import sys, struct

R = ['zero', 'at', 'v0', 'v1', 'a0', 'a1', 'a2', 'a3',
     't0', 't1', 't2', 't3', 't4', 't5', 't6', 't7',
     's0', 's1', 's2', 's3', 's4', 's5', 's6', 's7',
     't8', 't9', 'k0', 'k1', 'gp', 'sp', 's8', 'ra']
IOPS = {0x0c: 'andi', 0x0d: 'ori', 0x0e: 'xori', 0x09: 'addiu', 0x0f: 'lui'}


def main():
    path, base, skip, off = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), int(sys.argv[4], 0)
    d = open(path, 'rb').read()[skip:]
    for i in range(0, len(d) - 3, 4):
        w = struct.unpack_from('<I', d, i)[0]
        if (w >> 26) != 0x29 or (w & 0xffff) != off:
            continue
        ctx = []
        for k in range(max(0, i - 24), i, 4):
            v = struct.unpack_from('<I', d, k)[0]
            o = v >> 26
            if o in IOPS:
                ctx.append("%s %s,%s,0x%x" % (IOPS[o], R[(v >> 16) & 31], R[(v >> 21) & 31], v & 0xffff))
        print("%08x  sh %s,%d(%s)   <- %s" % (base + i, R[(w >> 16) & 31], off, R[(w >> 21) & 31],
                                              " | ".join(ctx)))


main()
