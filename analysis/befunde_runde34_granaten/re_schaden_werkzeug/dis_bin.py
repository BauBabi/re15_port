#!/usr/bin/env python3
"""dis_bin.py - disassembliert einen beliebigen Binaerbereich mit freier Ladeadresse.

Runde 34 / re_schaden_resolver. Grund: re15_disasm.py bildet 0x800c0000 (DEBUG.BIN,
resident @0x800C0000 via FUN_80013b60(7,&0x800c0000,0)) auf die PSX.EXE ab und bricht
dort ab. Dieses Skript nutzt denselben Decoder (dis_one aus re15_disasm.py), rechnet den
Datei-Offset aber aus einer explizit genannten Ladeadresse: off = addr - base.

Aufruf:
  python dis_bin.py <datei> <base_hex> <addr_hex> [n]
  python dis_bin.py info/Re1.5/PSX/BIN/DEBUG.BIN 0x800c0000 0x800c4780 16
"""
import os, sys, struct

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R  # noqa: E402


def main():
    if len(sys.argv) < 4:
        print(__doc__)
        sys.exit(1)
    path = sys.argv[1]
    if not os.path.isabs(path):
        path = os.path.join(REPO, path)
    base = int(sys.argv[2], 16)
    addr = int(sys.argv[3], 16)
    n = int(sys.argv[4]) if len(sys.argv) > 4 else 32
    data = open(path, "rb").read()
    print(f"; {path} base 0x{base:08x} @0x{addr:08x} ({n} instr)")
    for i in range(n):
        a = addr + 4 * i
        off = a - base
        if off < 0 or off + 4 > len(data):
            print(f"  {a:08x}: <ausserhalb der Datei>")
            break
        w = struct.unpack_from("<I", data, off)[0]
        s, call = R.dis_one(w, a)
        print(f"  {a:08x}: {w:08x}  {s:34s} {call or ''}")


if __name__ == "__main__":
    main()
