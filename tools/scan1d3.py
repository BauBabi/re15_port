#!/usr/bin/env python3
"""Scan a raw MIPS overlay/EXE image for every `sb rt,0x1d3(rs)` and print context.

usage: scan1d3.py <file> <load_base_hex> [file_start_off_hex]
"""
import struct, sys, os

REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]

def dec(w):
    op = w >> 26
    rs = (w >> 21) & 31
    rt = (w >> 16) & 31
    imm = w & 0xffff
    names = {0x28: "sb", 0x24: "lbu", 0x20: "lb", 0x0c: "andi", 0x0d: "ori",
             0x09: "addiu", 0x0f: "lui", 0x23: "lw", 0x2b: "sw", 0x25: "lhu",
             0x29: "sh", 0x21: "lh", 0x04: "beq", 0x05: "bne"}
    if op in names:
        return (names[op], rt, rs, imm)
    return (None, 0, 0, 0)

def main():
    path = sys.argv[1]
    base = int(sys.argv[2], 16)
    want = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0x1d3
    off = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0
    data = open(path, 'rb').read()[off:]
    n = len(data) // 4
    words = struct.unpack("<%dI" % n, data[:n * 4])
    hits = []
    for i, w in enumerate(words):
        d = dec(w)
        if d[0] == "sb" and d[3] == want:
            ctx = []
            for j in range(max(0, i - 6), min(n, i + 3)):
                dj = dec(words[j])
                if dj[0]:
                    s = "%08x: %-5s %s,%s,0x%x" % (base + j * 4, dj[0], REGS[dj[1]], REGS[dj[2]], dj[3])
                else:
                    s = "%08x: (raw %08x)" % (base + j * 4, words[j])
                ctx.append(s)
            hits.append((base + i * 4, d, ctx))
    print("== %s base=%08x  stores to +0x%x: %d" % (os.path.basename(path), base, want, len(hits)))
    for a, d, ctx in hits:
        print("\n-- store @%08x : sb %s,+0x%x(%s)" % (a, REGS[d[1]], want, REGS[d[2]]))
        for c in ctx:
            print("   " + c)

main()
