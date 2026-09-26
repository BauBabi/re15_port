#!/usr/bin/env python3
"""Roh-Scan: alle MIPS-Worte der PSX.EXE mit einem gegebenen 16-Bit-Immediate.

Aufruf: python scan_imm.py <imm-hex> [datei]
Gibt RAM-Adresse + Opcode-Feld aus. RAM = 0x80010000 + (offset - 0x800).
"""
import struct, sys

imm = int(sys.argv[1], 16) & 0xFFFF
path = sys.argv[2] if len(sys.argv) > 2 else "info/Re1.5/PSX.EXE"
data = open(path, "rb").read()
base = 0x80010000
OPN = {0x08: "addi", 0x09: "addiu", 0x0C: "andi", 0x0D: "ori", 0x0F: "lui",
       0x20: "lb", 0x21: "lh", 0x23: "lw", 0x24: "lbu", 0x25: "lhu",
       0x28: "sb", 0x29: "sh", 0x2B: "sw"}
for off in range(0x800, len(data) - 3, 4):
    w = struct.unpack_from("<I", data, off)[0]
    if (w & 0xFFFF) != imm:
        continue
    op = w >> 26
    if op not in OPN:
        continue
    rs = (w >> 21) & 31
    rt = (w >> 16) & 31
    print("%08x: %-6s rt=%d rs=%d  word=%08x" % (base + off - 0x800, OPN[op], rt, rs, w))
