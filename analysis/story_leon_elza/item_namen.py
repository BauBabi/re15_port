#!/usr/bin/env python3
"""item_namen.py - Itemnamen 0x00..0x65 aus DEBUG.BIN (Tabelle @0x800c495c, Blob @0x800c4a28,
Digraphen @0x800c44b8; Leser FUN_80028840 / Stromdekoder FUN_80013160 - Herleitung
re15_port/tools/gen_inv_name_bank.py). Glyphen = scd_walk_lib.glyph."""
import os, sys, json
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L
d = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
B = 0x800C0000
def name(i):
    o = L.u16(d, 0x800C495C - B + 2 * i)
    p = 0x800C4A28 - B + o
    out = []
    stack = []
    while True:
        c = d[p]; p += 1
        if c == 0x07: break
        if 0x60 <= c <= 0xB8:
            q = 0x800C44B8 - B + (c - 0xA0) * 2
            for cc in (d[q], d[q + 1]):
                g = L.glyph(cc); out.append(g if g else "?%02x" % cc)
            continue
        g = L.glyph(c); out.append(g if g else "?%02x" % c)
    return "".join(out)
names = {i: name(i) for i in range(0x66)}
if __name__ == "__main__":
    json.dump({"%02X" % k: v for k, v in names.items()}, open(os.path.join(HERE, "item_namen.json"), "w"), indent=0)
    for k, v in names.items(): print("%02X %s" % (k, v))
