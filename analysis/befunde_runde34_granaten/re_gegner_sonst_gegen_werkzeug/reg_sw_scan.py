#!/usr/bin/env python3
"""Gegenpruefung R34: sucht in PSX.EXE + allen STAGE*.BIN jede `sw` (op 0x2b), `sh` (0x29), `sb` (0x28),
deren effektive Adresse (einfache Registerverfolgung lui/addiu/ori im Fenster von 12 Instruktionen)
in [lo, hi) liegt. Aufruf: reg_sw_scan.py <lo_hex> <hi_hex>"""
import struct, sys, os
REPO = r"C:\workspace\git\reAi_v2"
lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
bins = [("PSX.EXE", os.path.join(REPO, "info/Re1.5/PSX.EXE"), None)]
for n in ["STAGE1","STAGE2","STAGE3","STAGE4","STAGE5","STAGE6","DEBUG","TITLE"]:
    p = os.path.join(REPO, "info/Re1.5/PSX/BIN/%s.BIN" % n)
    if os.path.exists(p): bins.append((n, p, 0x80100000))
def s16(x): return x - 0x10000 if x & 0x8000 else x
for name, path, load in bins:
    d = open(path, "rb").read()
    if load is None:
        taddr = struct.unpack_from("<I", d, 0x18)[0]; start = 0x800; base = taddr
    else:
        start = 0; base = load
    n = (len(d) - start) // 4
    regs = {}; age = {}
    for i in range(n):
        w = struct.unpack_from("<I", d, start + i*4)[0]
        a = base + i*4
        op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
        # decay
        for r in list(age):
            age[r] += 1
            if age[r] > 12: regs.pop(r, None); age.pop(r, None)
        if op == 0x0f:  # lui
            regs[rt] = imm << 16; age[rt] = 0; continue
        if op == 0x09 and rs in regs:  # addiu
            regs[rt] = (regs[rs] + s16(imm)) & 0xffffffff; age[rt] = 0; continue
        if op == 0x0d and rs in regs:  # ori
            regs[rt] = regs[rs] | imm; age[rt] = 0; continue
        if op in (0x2b, 0x29, 0x28) and rs in regs:
            ea = (regs[rs] + s16(imm)) & 0xffffffff
            if lo <= ea < hi:
                k = {0x2b: "sw", 0x29: "sh", 0x28: "sb"}[op]
                print("%-7s %08x: %s r%d -> 0x%08x" % (name, a, k, rt, ea))
        # any other write to rt kills tracking (rough)
        if op in (0x08,0x0a,0x0b,0x0c,0x0e,0x20,0x21,0x23,0x24,0x25) and rt in regs and op != 0x09:
            regs.pop(rt, None); age.pop(rt, None)
        if op == 0 and ((w >> 11) & 31) in regs:
            rd = (w >> 11) & 31; regs.pop(rd, None); age.pop(rd, None)
