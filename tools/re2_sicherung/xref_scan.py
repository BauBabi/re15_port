#!/usr/bin/env python3
"""xref_scan.py - alle Zugriffe auf die RE2-Item-Eigenschaftstabelle finden.

Sucht in der RE2-PSX.EXE nach Load-/Store-Instruktionen, deren
(lui-Basis + signed-Immediate) in einen vorgegebenen Adressbereich fallen.
So werden auch die "lui at,0x800b / addu at,at,v0 / lbu v0,-25059(at)"-Muster
gefunden, bei denen die Tabellenadresse gar nicht als Wort im Code steht.
"""
import struct, sys, os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
D = open(EXE, "rb").read()
TADDR = struct.unpack_from("<I", D, 0x18)[0]
TSIZE = struct.unpack_from("<I", D, 0x1c)[0]

LOADS = {0x20: "lb", 0x21: "lh", 0x23: "lw", 0x24: "lbu", 0x25: "lhu",
         0x28: "sb", 0x29: "sh", 0x2B: "sw"}
REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]


def scan_range(lo, hi):
    """Alle mem-Ops, deren lui-Basis + simm in [lo,hi) liegt."""
    lui = {}                      # reg -> (upper<<16, addr)
    hits = []
    n = TSIZE // 4
    for k in range(n):
        a = TADDR + k * 4
        w = struct.unpack_from("<I", D, 0x800 + k * 4)[0]
        op = w >> 26
        rs = (w >> 21) & 31
        rt = (w >> 16) & 31
        imm = w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 0x0F:                          # lui rt, imm
            lui[rt] = (imm << 16, a)
            continue
        if op == 0x09 and rs in lui:            # addiu rt, rs, imm (Basis fortschreiben)
            lui[rt] = (lui[rs][0] + simm, a)
            continue
        if op in LOADS and rs in lui:
            tgt = (lui[rs][0] + simm) & 0xFFFFFFFF
            if lo <= tgt < hi:
                hits.append((a, LOADS[op], REGS[rt], simm, REGS[rs], tgt, lui[rs][1]))
        if op == 0x00 and (w & 0x3F) in (0x20, 0x21):   # add/addu rd,rs,rt
            rd = (w >> 11) & 31
            if rs in lui and rt not in lui:
                lui[rd] = lui[rs]
            elif rt in lui and rs not in lui:
                lui[rd] = lui[rt]
    return hits


def scan_imm(op, imm):
    """Alle Instruktionen mit Opcode op und exaktem Immediate imm."""
    out = []
    for k in range(TSIZE // 4):
        w = struct.unpack_from("<I", D, 0x800 + k * 4)[0]
        if (w >> 26) == op and (w & 0xFFFF) == imm:
            out.append((TADDR + k * 4, w))
    return out


if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "range":
        lo = int(sys.argv[2], 16); hi = int(sys.argv[3], 16)
        for a, m, rt, simm, rs, tgt, luia in scan_range(lo, hi):
            print("0x%08X  %-4s %-4s %6d(%s)   -> 0x%08X   (lui @0x%08X)" % (
                a, m, rt, simm, rs, tgt, luia))
    elif mode == "imm":
        op = int(sys.argv[2], 16); imm = int(sys.argv[3], 16)
        for a, w in scan_imm(op, imm):
            print("0x%08X  %08x" % (a, w))
