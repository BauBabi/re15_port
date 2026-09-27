#!/usr/bin/env python3
"""Roh-Scan: alle MIPS-Load/Store/addiu-Worte mit Immediate in [lo,hi].
   usage: scan_immrange.py <lo-hex> <hi-hex> [datei]
"""
import struct
import sys

lo = int(sys.argv[1], 16) & 0xFFFF
hi = int(sys.argv[2], 16) & 0xFFFF
path = sys.argv[3] if len(sys.argv) > 3 else "info/re2leon/PSX.EXE"
data = open(path, "rb").read()
base = 0x80010000
OPN = {0x08: "addi", 0x09: "addiu", 0x0C: "andi", 0x0D: "ori",
       0x20: "lb", 0x21: "lh", 0x23: "lw", 0x24: "lbu", 0x25: "lhu",
       0x28: "sb", 0x29: "sh", 0x2B: "sw"}
R = ['zero', 'at', 'v0', 'v1', 'a0', 'a1', 'a2', 'a3',
     't0', 't1', 't2', 't3', 't4', 't5', 't6', 't7',
     's0', 's1', 's2', 's3', 's4', 's5', 's6', 's7',
     't8', 't9', 'k0', 'k1', 'gp', 'sp', 's8', 'ra']
for off in range(0x800, len(data) - 3, 4):
    w = struct.unpack_from("<I", data, off)[0]
    imm = w & 0xFFFF
    if imm < lo or imm > hi:
        continue
    op = w >> 26
    if op not in OPN:
        continue
    print("%08x: %-5s %s,0x%04x(%s)" % (base + off - 0x800, OPN[op],
                                        R[(w >> 16) & 31], imm, R[(w >> 21) & 31]))
