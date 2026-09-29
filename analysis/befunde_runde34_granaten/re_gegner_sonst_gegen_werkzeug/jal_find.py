#!/usr/bin/env python3
"""Gegenpruefung R34: jede `jal <ziel>` in PSX.EXE + STAGE1..6 (RE1.5) auflisten. jal_find.py <ziel_hex>"""
import struct, sys, os
REPO = r"C:\workspace\git\reAi_v2"
tgt = int(sys.argv[1], 16)
bins = [("PSX.EXE", os.path.join(REPO, "info/Re1.5/PSX.EXE"), None)]
for n in ["STAGE1","STAGE2","STAGE3","STAGE4","STAGE5","STAGE6","DEBUG","TITLE"]:
    p = os.path.join(REPO, "info/Re1.5/PSX/BIN/%s.BIN" % n)
    if os.path.exists(p): bins.append((n, p, 0x80100000))
for name, path, load in bins:
    d = open(path, "rb").read()
    if load is None:
        base = struct.unpack_from("<I", d, 0x18)[0]; start = 0x800
    else:
        base = load; start = 0
    for i in range((len(d) - start)//4):
        w = struct.unpack_from("<I", d, start + i*4)[0]
        a = base + i*4
        if (w >> 26) == 3 and (((a + 4) & 0xf0000000) | ((w & 0x3ffffff) << 2)) == tgt:
            print("%-7s %08x" % (name, a))
