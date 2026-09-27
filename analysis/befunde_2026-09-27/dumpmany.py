#!/usr/bin/env python3
"""dumpmany.py <overlay.BIN> <n> <addr-hex> [addr-hex ...]
Disassembliert n Instruktionen ab jeder Adresse (roh @0x80100000)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import struct
from scan_sh import dis, BASE  # gleiche Decoder-Tabelle

def main():
    path, n = sys.argv[1], int(sys.argv[2])
    d = open(path, "rb").read()
    tot = len(d) // 4
    w = struct.unpack("<%dI" % tot, d[:tot*4])
    for a_s in sys.argv[3:]:
        a = int(a_s, 16)
        i = (a - BASE) // 4
        print(f"=== 0x{a:08x} ===")
        for j in range(i, min(tot, i+n)):
            pc = BASE + j*4
            print(f"  {pc:08x}: {w[j]:08x}  {dis(w[j], pc)}")

main()
