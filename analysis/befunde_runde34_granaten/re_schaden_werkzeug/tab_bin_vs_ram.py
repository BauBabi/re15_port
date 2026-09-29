#!/usr/bin/env python3
"""tab_bin_vs_ram.py - vergleicht eine Overlay-Tabelle (STAGEn.BIN, roh @0x80100000) Wort fuer Wort
mit dem Live-RAM eines sauberen Savestates. Runde 34 / re_schaden_resolver.
Aufruf: python tab_bin_vs_ram.py <sav> <STAGEn.BIN> <addr_hex> <n_worte>
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa
sav, binname, addr, n = sys.argv[1], sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4])
if "PATCHED-EXE" in sav:
    sys.exit("gepatchter Savestate - nicht verwenden (CLAUDE.md)")
ram = re15_ss.Ram(sav)
b = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", binname), "rb").read()
same = 0
for i in range(n):
    a = addr + 4 * i
    wb = struct.unpack_from("<I", b, a - 0x80100000)[0]
    wr = ram.u32(a)
    same += (wb == wr)
    if wb != wr:
        print(f"  DIFF @0x{a:08x}: BIN 0x{wb:08x}  RAM 0x{wr:08x}")
print(f"{os.path.basename(sav)} {binname} @0x{addr:08x} x{n}: {same}/{n} gleich")
