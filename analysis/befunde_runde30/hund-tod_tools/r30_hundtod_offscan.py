#!/usr/bin/env python3
"""r30_hundtod_offscan.py - alle Speicherzugriffe mit einem festen Offset (z.B. 342 = +0x156 hp)
in einer Binaerdatei, getrennt nach Lesen/Schreiben. Aufruf:
  python r30_hundtod_offscan.py <offset-dez> <datei> <ladeadresse-hex> [kopf-bytes-hex]
Nur Lesen."""
import struct, sys, os
NAMES = {0x23:"lw",0x25:"lhu",0x24:"lbu",0x21:"lh",0x20:"lb",0x2b:"sw",0x29:"sh",0x28:"sb"}
REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
def main():
    off = int(sys.argv[1]); path = sys.argv[2]; base = int(sys.argv[3], 16)
    off0 = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0
    data = open(path, "rb").read()
    n = (len(data) - off0) // 4
    words = struct.unpack_from("<%dI" % n, data, off0)
    r = w_ = 0
    for i, w in enumerate(words):
        op = w >> 26
        if op not in NAMES: continue
        imm = w & 0xffff
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if simm != off: continue
        rs = (w >> 21) & 31; rt = (w >> 16) & 31
        if rs == 29: continue            # Stapel
        kind = "SCHREIBT" if op in (0x2b, 0x29, 0x28) else "liest"
        if kind == "SCHREIBT": w_ += 1
        else: r += 1
        print("0x%08x %-4s %s,%d(%s)   %s" % (base + i * 4, NAMES[op], REGS[rt], off, REGS[rs], kind))
    print("# %s Offset %d: %d Leser, %d Schreiber" % (os.path.basename(path), off, r, w_))
main()
