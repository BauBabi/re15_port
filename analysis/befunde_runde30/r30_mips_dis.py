#!/usr/bin/env python3
"""r30_mips_dis.py - MIPS-Disassembler fuer BELIEBIGE Binaerdatei mit BELIEBIGER Ladeadresse.
re2_disasm.py kennt nur EXE (0x8001xxxx) und Overlays @0x80100000; RE2s MEM_CARD.BIN laedt
@0x801c0000 ("BASLUS" @0x801c0008), RE1.5s DEBUG.BIN @0x800c0000.
Aufruf:  r30_mips_dis.py <datei> <ladeadresse-hex> <adresse-hex> [n] [--find-addr 0x800d4b68]
  --find-addr X : sucht alle lui/addiu|ori|load|store-Paare, die die Adresse X bilden
"""
import sys, os, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re2_disasm as D

def main():
    a = sys.argv[1:]
    path, base = a[0], int(a[1], 16)
    data = open(path, "rb").read()
    hdr = 0
    if data[:8] == b"PS-X EXE":
        hdr = 0x800; base = struct.unpack_from("<I", data, 0x18)[0]
    if "--find-addr" in a:
        tgt = int(a[a.index("--find-addr") + 1], 16)
        lo_r = int(a[a.index("--range") + 1], 16) if "--range" in a else 1
        n = (len(data) - hdr) // 4
        hi = {}
        for i in range(n):
            w = struct.unpack_from("<I", data, hdr + i * 4)[0]
            op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
            simm = imm - 0x10000 if imm & 0x8000 else imm
            adr = base + i * 4
            if op == 0xf: hi[rt] = (imm << 16, adr); continue
            if op in (9, 0xd, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2b) and rs in hi:
                v = (hi[rs][0] + (simm if op != 0xd else imm)) & 0xffffffff
                if tgt <= v < tgt + lo_r and adr - hi[rs][1] < 0x40:
                    s, _ = D.dis_one(w, adr)
                    print("  %08x: %-30s -> 0x%08x (lui @%08x)" % (adr, s, v, hi[rs][1]))
            if op in (9, 0xd) and rt in hi and rt != rs: hi.pop(rt, None)
        return
    adr = int(a[2], 16); n = int(a[3]) if len(a) > 3 else 48
    hi = {}
    print("; %s  Ladeadresse 0x%08x  @0x%08x (%d Instr.)" % (path, base, adr, n))
    for i in range(n):
        off = hdr + adr - base + i * 4
        w = struct.unpack_from("<I", data, off)[0]
        s, call = D.dis_one(w, adr + i * 4)
        op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
        simm = imm - 0x10000 if imm & 0x8000 else imm
        ann = ""
        if op == 0xf: hi[rt] = imm << 16
        elif op in (0x23, 0x24, 0x25, 0x21, 0x20, 0x28, 0x29, 0x2b, 9) and rs in hi:
            g = (hi[rs] + simm) & 0xffffffff
            if 0x80000000 <= g < 0x80200000: ann = "0x%08x" % g
        elif op == 0xd and rs in hi:
            g = (hi[rs] | imm) & 0xffffffff
            if 0x80000000 <= g < 0x80200000: ann = "0x%08x" % g
        print("  %08x: %08x  %-34s %s" % (adr + i * 4, w, s, ann))
main()
