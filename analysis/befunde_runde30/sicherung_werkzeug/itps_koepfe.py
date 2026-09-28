#!/usr/bin/env python3
"""Die eingebetteten Rechtecke (crect/prect) ALLER ITPS-Bloecke, gruppiert.

Der CHECK-Foto-Upload des Ports (platform/pc/src/inv_render_pc.c:365-400, Original
0x800c6878 -> 0x800c0258) uebernimmt einen Block nur, wenn crect == (0,489) und
prect == (832,256) 56x72 ist. Welche Bloecke erfuellen das, welche nicht?

    python analysis/befunde_runde30/sicherung_werkzeug/itps_koepfe.py
"""
import os
import struct

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
for name, pfad in (("RE1.5 ITEM/ITPS.ITP", os.path.join(REPO, "re15_port", "shared_assets", "PSX", "ITEM", "ITPS.ITP")),
                   ("RE2 COMMON/DATA/ITPS.ITP", os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ITPS.ITP"))):
    d = open(pfad, "rb").read()
    gr = {}
    for k in range(len(d) // 0x3000):
        b = d[k * 0x3000:(k + 1) * 0x3000]
        if struct.unpack_from("<I", b, 0)[0] != 0x10:
            gr.setdefault("kein TIM", []).append(k)
            continue
        clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, 8)
        plen, px, py, pw, ph = struct.unpack_from("<IHHHH", b, 8 + clen)
        gr.setdefault("crect (%d,%d) %dx%d | prect (%d,%d) %dhw x %d" % (cx, cy, cw, ch, px, py, pw, ph), []).append(k)
    print(name, "-", len(d) // 0x3000, "Bloecke")
    for g, ks in sorted(gr.items(), key=lambda a: -len(a[1])):
        print("   %-48s %2d Bloecke: %s" % (g, len(ks), " ".join("%02X" % k for k in ks)))
b = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "ITEM", "ITPS.ITP"), "rb").read()[0x40 * 0x3000:0x41 * 0x3000]
print("Block 0x40 Bildkopf @Block+0x214 = Datei 0x%X: %s" % (0x40 * 0x3000 + 0x214, b[0x214:0x220].hex(" ")))
print("Block 0x40 Breit-Icon @+0x21A0 (80x30): %d von 2400 Byte != 0" % sum(1 for x in b[0x21A0:0x21A0 + 2400] if x))
