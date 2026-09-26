#!/usr/bin/env python3
"""Scan a raw MIPS image for every `sh rt,<off>(rs)` and print preceding context.

usage: scanhalf.py <file> <load_base_hex> <offset_hex>
"""
import struct, sys, os

REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]

def dec(w):
    op = w >> 26
    rs = (w >> 21) & 31
    rt = (w >> 16) & 31
    imm = w & 0xffff
    names = {0x29: "sh", 0x25: "lhu", 0x21: "lh", 0x0c: "andi", 0x0d: "ori",
             0x09: "addiu", 0x0f: "lui", 0x28: "sb", 0x24: "lbu"}
    return (names.get(op), rt, rs, imm)

def main():
    path = sys.argv[1]
    base = int(sys.argv[2], 16)
    want = int(sys.argv[3], 16)
    data = open(path, 'rb').read()
    n = len(data) // 4
    words = struct.unpack("<%dI" % n, data[:n * 4])
    cnt = 0
    for i in range(n):
        a = dec(words[i])
        if a[0] == "sh" and a[3] == want:
            cnt += 1
            print("-- sh %s,+0x%x(%s) @%08x" % (REGS[a[1]], want, REGS[a[2]], base + i * 4))
            for j in range(max(0, i - 5), i + 1):
                b = dec(words[j])
                if b[0]:
                    print("    %08x: %-5s %s,%s,0x%x" % (base + j * 4, b[0], REGS[b[1]], REGS[b[2]], b[3]))
                else:
                    print("    %08x: (raw %08x)" % (base + j * 4, words[j]))
    print("== %s base=%08x  sh to +0x%x: %d" % (os.path.basename(path), base, want, cnt))

main()
