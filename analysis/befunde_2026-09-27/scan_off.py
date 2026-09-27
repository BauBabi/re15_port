#!/usr/bin/env python3
"""Vollscan: alle Load/Store-Instruktionen mit einem gesuchten Offset.
   usage: scan_off.py <bin> <rambase-hex> <skip-hex> <off1,off2,...> [sb,sh,sw,lb,lbu,lh,lhu,lw]
   RE2-EXE:  scan_off.py info/re2leon/PSX.EXE 0x80010000 0x800 340
   Overlay:  scan_off.py info/re2leon/COMMON/BIN/EMZ0.BIN 0x80100000 0 340
"""
import sys, struct

OPS = {0x28: 'sb', 0x29: 'sh', 0x2b: 'sw',
       0x20: 'lb', 0x24: 'lbu', 0x21: 'lh', 0x25: 'lhu', 0x23: 'lw'}
R = ['zero', 'at', 'v0', 'v1', 'a0', 'a1', 'a2', 'a3',
     't0', 't1', 't2', 't3', 't4', 't5', 't6', 't7',
     's0', 's1', 's2', 's3', 's4', 's5', 's6', 's7',
     't8', 't9', 'k0', 'k1', 'gp', 'sp', 's8', 'ra']


def main():
    path = sys.argv[1]
    base = int(sys.argv[2], 16)
    skip = int(sys.argv[3], 16)
    want = set(int(x, 0) for x in sys.argv[4].split(','))
    which = set(sys.argv[5].split(',')) if len(sys.argv) > 5 else set(OPS.values())
    data = open(path, 'rb').read()[skip:]
    n = 0
    for i in range(0, len(data) - 3, 4):
        w = struct.unpack_from('<I', data, i)[0]
        op = w >> 26
        if op not in OPS or OPS[op] not in which:
            continue
        imm = w & 0xffff
        if imm >= 0x8000:
            imm -= 0x10000
        if imm in want:
            print("%08x: %-4s %s,%d(%s)" % (base + i, OPS[op], R[(w >> 16) & 31],
                                            imm, R[(w >> 21) & 31]))
            n += 1
    print("-- %d Treffer" % n)


main()
