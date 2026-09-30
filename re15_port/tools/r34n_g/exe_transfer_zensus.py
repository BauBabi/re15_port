#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht, Stufe BAU, Auflage 6) - Vollzensus der GPU-Bildtransfers.

Sucht in info/Re1.5/PSX.EXE (text ab Datei 0x800, t_addr/t_size aus dem PS-X-EXE-Kopf @0x18/@0x1c)
und in allen info/Re1.5/PSX/BIN/*.BIN (Overlays, ohne Kopf) das jal-Wort 0x0C000000 | (ziel>>2)
der vier libgpu-Bildtransfers (Namen aus dem Binary, G15 D: "LoadImage" @0x80011b9c usw.) und der
Hintergrund-Huellen, dazu jeden Store mit Versatz 0x536c (DAT_800b536c = Upload-Auslass,
lbu @0x8002151c / bne @0x80021524).

  C:/Python310/python.exe re15_port/tools/r34n_g/exe_transfer_zensus.py [info/Re1.5]
"""
import struct, glob, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "info", "Re1.5")
exe = open(os.path.join(ROOT, "PSX.EXE"), "rb").read()
t_addr = struct.unpack_from("<I", exe, 0x18)[0]
t_size = struct.unpack_from("<I", exe, 0x1c)[0]
print("PSX.EXE t_addr=%08x t_size=%x" % (t_addr, t_size))
text = exe[0x800:0x800 + t_size]
def words(buf):
    for i in range(0, len(buf) - 3, 4):
        yield i, struct.unpack_from("<I", buf, i)[0]
ZIELE = {"LoadImage": 0x80068c88, "StoreImage": 0x80068cec, "MoveImage": 0x80068d50, "ClearImage": 0x80068bf4,
         "FUN_80043870(BG-Upload)": 0x80043870, "FUN_80021bbc(Cut-Wechsel)": 0x80021bbc, "FUN_80021634(Modus)": 0x80021634}
def jal(t): return 0x0C000000 | ((t >> 2) & 0x03FFFFFF)
bins = sorted(glob.glob(os.path.join(ROOT, "PSX", "BIN", "*.BIN")))
for name, t in ZIELE.items():
    w = jal(t)
    hits = [t_addr + i for i, x in words(text) if x == w]
    print("%-26s jal=%08x EXE %2d: %s" % (name, w, len(hits), " ".join("%08x" % h for h in hits)))
    for b in bins:
        data = open(b, "rb").read()
        n = sum(1 for i, x in words(data) if x == w)
        if n: print("    %s: %d" % (os.path.basename(b), n))
    tot = sum(sum(1 for i, x in words(open(b, "rb").read()) if x == w) for b in bins if os.path.basename(b).startswith("STAGE"))
    print("    STAGE1..6.BIN gesamt: %d" % tot)
# Stores mit imm 0x536c (sb/sh/sw) in EXE und BINs, jeweils mit vorangehendem lui 0x800b auf das Basisregister
def stores(buf, base):
    out = []
    for i, x in words(buf):
        op = x >> 26
        if op in (0x28, 0x29, 0x2b) and (x & 0xffff) == 0x536c:
            rs = (x >> 21) & 31
            # rueckwaerts bis 8 Befehle nach lui rs,0x800b suchen
            ok = False
            for k in range(1, 9):
                if i - 4 * k < 0: break
                y = struct.unpack_from("<I", buf, i - 4 * k)[0]
                if (y >> 26) == 0x0f and ((y >> 16) & 31) == rs:
                    ok = ((y & 0xffff) == 0x800b); break
            out.append((base + i, {0x28: "sb", 0x29: "sh", 0x2b: "sw"}[op], ok))
    return out
print("Stores imm 0x536c EXE:", [("%08x" % a, m, "lui800b" if ok else "?") for a, m, ok in stores(text, t_addr)])
for b in bins:
    s = stores(open(b, "rb").read(), 0x80100000)
    if s: print("Stores imm 0x536c", os.path.basename(b), [("%08x" % a, m, ok) for a, m, ok in s])
