#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - Masken-Record-Tabelle eines DuckStation-Stands lesen. Nur LESEN.

Je Stand: EXE-Sauberkeit (@0x80026e4c), ob der Code der beteiligten Funktionen im RAM bytegleich zu
info/Re1.5/PSX.EXE ist (FUN_800392d4..FUN_800396a8 Ende, Opcode-0x45-Handler 0x800428d4, SCD-Laeufer
0x8003f038, Cut-Apply-Kopf 0x80021bbc, Masken-Aufruf im Hauptloop 0x8001ce3c..0x8001ce5c), Raum
DAT_800b0fe2, Cut DAT_800b0fe4, Flags (Bank-Zeiger @0x80074664, Wort = bit>>5, Maske 0x80000000>>(bit&31)),
RDT[0] (Zahl, FUN_800392d4 @0x80039358) und RDT[7], Records DAT_800b2584 (Byte0/Byte1).
Optional: beide Bildpuffer (VRAM y=0 / y=240, 320x240) als PNG.

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_masken_records.py <stand.sav> [--flag 4,9 --flag 5,0]
      [--png <praefix>]
"""
import argparse
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

CODE = [(0x800392d4, 0x800396f8), (0x800428d4, 0x8004290c), (0x8003f038, 0x8003f0a0),
        (0x80021bbc, 0x80021c30), (0x8001ce3c, 0x8001ce5c)]
BANK = {0: 0x800aca38, 1: 0x800aca3c, 2: 0x800aca40, 3: 0x800b0ff8, 4: 0x800b1018, 5: 0x800b1028,
        6: 0x800b1030, 7: 0x800b1038, 8: 0x800b1058, 9: 0x800b1078, 10: 0x800b1098}


def exe_bytes(a, n):
    d = open(os.path.join(ROOT, "info", "Re1.5", "PSX.EXE"), "rb").read()
    t = struct.unpack_from("<I", d, 0x18)[0]
    o = 0x800 + a - t
    return d[o:o + n]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sav")
    ap.add_argument("--flag", action="append", default=[])
    ap.add_argument("--png", default=None)
    a = ap.parse_args()
    r = re15_ss.Ram(a.sav)
    stub = r.u32(0x80026e4c)
    print("Stand %s: EXE %s (@0x80026e4c = %08x)" % (os.path.basename(a.sav),
          "sauber" if stub == 0x03e00008 else "GEPATCHT", stub))
    for lo, hi in CODE:
        same = r.bytes(lo, hi - lo) == exe_bytes(lo, hi - lo)
        print("  Code %08x..%08x im RAM %s PSX.EXE" % (lo, hi, "==" if same else "!="))
    print("  Raum 0x%02X, Cut (wv0A) %d, alter Cut (wv0C) %d" % (r.u16(0x800b0fe2), r.u16(0x800b0fe4),
          r.u16(0x800b0fe8)))
    for f in a.flag:
        b, bit = [int(x, 0) for x in f.split(",")]
        w = r.u32(BANK[b] + 4 * (bit >> 5))
        print("  Flag (%d,%d) = %d" % (b, bit, 1 if w & (0x80000000 >> (bit & 31)) else 0))
    rdt = r.u32(0x800ac778)
    zahl, n7 = r.u8(rdt), r.u8(rdt + 7)
    tab = r.u32(0x800b2584)
    print("  RDT @%08x: RDT[0] (Zahl) = %d, RDT[7] = %d; Records @%08x" % (rdt, zahl, n7, tab))
    an = 0
    zeilen = []
    for i in range(max(zahl, 1) if zahl else 0):
        b0, b1, tiefe = r.u8(tab + 4 * i), r.u8(tab + 4 * i + 1), r.s16(tab + 4 * i + 2)
        an += b0 & 1
        zeilen.append("%d:%d/g%d" % (i, b0, b1))
    print("  Records i < Zahl (i:Byte0/gGruppe+1): " + " ".join(zeilen))
    print("  -> gezeichnet (Byte0 & 1): %d von %d" % (an, zahl))
    if a.png:
        for y0, name in ((0, "puffer0"), (240, "puffer1")):
            rgb = bytearray(320 * 240 * 3)
            for y in range(240):
                for x in range(320):
                    px = r.vpix(x, y0 + y)
                    i = (y * 320 + x) * 3
                    rgb[i], rgb[i + 1], rgb[i + 2] = (px & 31) << 3, ((px >> 5) & 31) << 3, ((px >> 10) & 31) << 3
            re15_ss.write_png("%s_%s.png" % (a.png, name), 320, 240, bytes(rgb))
            print("  Bild %s_%s.png" % (a.png, name))
    return 0


if __name__ == "__main__":
    sys.exit(main())
