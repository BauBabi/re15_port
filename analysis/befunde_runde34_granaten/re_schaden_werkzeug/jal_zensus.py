#!/usr/bin/env python3
"""jal_zensus.py - sucht alle `jal <ziel>`-Instruktionen UND das Ziel als Datenwort (Funktionszeiger)
in PSX.EXE (t_addr aus Header @0x18, Text ab Datei 0x800), DEBUG.BIN (@0x800C0000) und den
Overlays STAGE1..6/TITLE (@0x80100000, ohne Kopf).
Runde 34 / re_schaden_resolver.  Aufruf: python jal_zensus.py 0x80012d60 0x80011f50 0x800128a0
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
BINS = [("DEBUG.BIN", 0x800c0000)] + [(f"STAGE{i}.BIN", 0x80100000) for i in range(1, 7)] + [("TITLE.BIN", 0x80100000)]

def scan(buf, base, first, target, name):
    enc = (3 << 26) | ((target >> 2) & 0x3ffffff)
    for off in range(first, len(buf) - 3, 4):
        w = struct.unpack_from("<I", buf, off)[0]
        a = base + off - first
        if w == enc:
            print(f"  {name:10s} jal  @0x{a:08x}")
        elif w == target:
            print(f"  {name:10s} wort @0x{a:08x}")

for t in [int(x, 16) for x in sys.argv[1:]]:
    print(f"Ziel 0x{t:08x}")
    exe = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
    taddr = struct.unpack_from("<I", exe, 0x18)[0]
    scan(exe, taddr, 0x800, t, "PSX.EXE")
    for f, base in BINS:
        b = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", f), "rb").read()
        scan(b, base, 0, t, f)
