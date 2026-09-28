#!/usr/bin/env python3
"""r30_karte3010_xref.py - Xref-Sucher fuer PS-X-EXEs (RE2 Leon / RE1.5).

  jal  <ziel>      alle 'jal ziel' / 'j ziel' im Textsegment
  word <wert>      alle 32-Bit-Datenworte == wert (Zeiger-/Sprungtabellen)
  ref  <adresse>   alle Zugriffe auf eine absolute Adresse:
                   lui rX,hi  ... (innerhalb 12 Instruktionen) op off(rX)
                   mit (hi<<16)+simm == adresse. Meldet Instruktion + L/S.

Binaerdatei: --exe re2 (info/re2leon/PSX.EXE, Default) | re15 (info/Re1.5/PSX.EXE)
             | <Pfad> ; --base/--hdr fuer Rohdateien (Overlay: --base 0x80100000 --hdr 0)
"""
import struct, sys, os, argparse
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
R = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
MEM = {0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2b:"sw",
       0x09:"addiu",0x0d:"ori"}

def load(args):
    p = args.exe
    if p == "re2": p = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
    elif p == "re15": p = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
    d = open(p, "rb").read()
    if args.base is not None:
        base = args.base; hdr = args.hdr
    else:
        base = struct.unpack_from("<I", d, 0x18)[0]; hdr = 0x800
    return d, base, hdr, p

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=["jal", "word", "ref"])
    ap.add_argument("val", type=lambda s: int(s, 0))
    ap.add_argument("--exe", default="re2")
    ap.add_argument("--base", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--hdr", type=lambda s: int(s, 0), default=0)
    a = ap.parse_args()
    d, base, hdr, p = load(a)
    n = (len(d) - hdr) // 4
    print("; %s base=0x%08X hdr=0x%X words=%d" % (p, base, hdr, n))
    W = struct.unpack_from("<%dI" % n, d, hdr)
    if a.mode == "jal":
        for i, w in enumerate(W):
            op = w >> 26
            if op in (2, 3):
                addr = base + i * 4
                tgt = ((addr + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
                if tgt == a.val:
                    print("  0x%08X  %s 0x%08X   (Datei 0x%X)" % (addr, "jal" if op == 3 else "j", tgt, hdr + i * 4))
    elif a.mode == "word":
        for i, w in enumerate(W):
            if w == a.val:
                print("  0x%08X  .word 0x%08X   (Datei 0x%X)" % (base + i * 4, w, hdr + i * 4))
    else:
        tgt = a.val
        for i, w in enumerate(W):
            if (w >> 26) != 0x0F:
                continue
            reg = (w >> 16) & 31
            hi = (w & 0xFFFF) << 16
            for j in range(1, 14):
                if i + j >= n: break
                x = W[i + j]
                op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
                imm = x & 0xFFFF; simm = imm - 0x10000 if imm & 0x8000 else imm
                if op in MEM and rs == reg:
                    val = (hi + (imm if op == 0x0d else simm)) & 0xFFFFFFFF
                    if val == tgt:
                        print("  0x%08X  lui %s / 0x%08X %s %s,%d(%s)" % (
                            base + i * 4, R[reg], base + (i + j) * 4, MEM[op], R[rt], simm, R[rs]))
                # Register ueberschrieben -> Kette endet
                if op == 0x0F and ((x >> 16) & 31) == reg: break
                if op in (0x09, 0x0d, 0x0c, 0x0a, 0x0b, 0x20, 0x21, 0x23, 0x24, 0x25) and rt == reg and not (op in MEM and rs == reg and ((hi + simm) & 0xFFFFFFFF) == tgt and op in (0x09,)):
                    break
                if op == 0 and ((x >> 11) & 31) == reg and (x & 63) not in (8, 9, 0x18, 0x19, 0x1a, 0x1b):
                    break
if __name__ == "__main__":
    main()
