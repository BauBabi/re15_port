#!/usr/bin/env python3
"""r30_hundtod_absscan.py - alle absoluten Zugriffe (lui+load/store/addiu) auf einen
Adressbereich in einer Binaerdatei. Aufruf:
  python r30_hundtod_absscan.py <von-hex> <bis-hex> <datei> <ladeadresse-hex> [kopf-bytes-hex]
Nur Lesen."""
import struct, sys, os
NAMES = {0x23:"lw",0x25:"lhu",0x24:"lbu",0x21:"lh",0x20:"lb",0x2b:"sw",0x29:"sh",0x28:"sb",0x09:"addiu"}
def main():
    lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
    path = sys.argv[3]; base = int(sys.argv[4], 16)
    off0 = int(sys.argv[5], 16) if len(sys.argv) > 5 else 0
    data = open(path, "rb").read()
    n = (len(data) - off0) // 4
    words = struct.unpack_from("<%dI" % n, data, off0)
    hits = 0
    for i, w in enumerate(words):
        op = w >> 26
        if op not in NAMES: continue
        rs = (w >> 21) & 31
        imm = w & 0xffff
        simm = imm - 0x10000 if imm & 0x8000 else imm
        for j in range(i - 1, max(i - 10, -1), -1):
            wj = words[j]
            if (wj >> 26) == 0x0f and ((wj >> 16) & 31) == rs:
                addr = (((wj & 0xffff) << 16) + simm) & 0xffffffff
                if lo <= addr <= hi:
                    hits += 1
                    print("0x%08x %-5s -> 0x%08x" % (base + i * 4, NAMES[op], addr))
                break
    print("# Treffer %d in %s fuer 0x%08x..0x%08x" % (hits, os.path.basename(path), lo, hi))
main()
