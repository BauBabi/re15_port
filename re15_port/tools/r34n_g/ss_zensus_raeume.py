#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - welcher DuckStation-Stand liegt in welchem Raum? Nur LESEN.

Stage ueber den Code-Fingerabdruck CRC32(RAM 0x80100000..+0x8000) gegen die ersten 0x8000 Byte von
info/Re1.5/PSX/BIN/STAGE{1..6}.BIN (Overlay ohne Kopf, geladen @0x80100000), Raum DAT_800b0fe2, Cut
DAT_800b0fe4 (work_vars[0x0A]), EXE-Sauberkeit @0x80026e4c == 0x03e00008.

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_zensus_raeume.py <ordner> [...]
"""
import glob
import os
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402


def main():
    fps = {}
    for n in range(1, 7):
        p = os.path.join(ROOT, "info", "Re1.5", "PSX", "BIN", "STAGE%d.BIN" % n)
        fps[zlib.crc32(open(p, "rb").read()[:0x8000]) & 0xffffffff] = n
    files = []
    for d in sys.argv[1:]:
        for pat in ("*.sav", "*.bak"):
            files += glob.glob(os.path.join(d, "**", pat), recursive=True)
    for f in sorted(set(files)):
        try:
            r = re15_ss.Ram(f)
        except Exception as e:  # noqa: BLE001
            print("UNLESBAR %s: %s" % (f, e))
            continue
        stub = r.u32(0x80026e4c)
        fp = zlib.crc32(r.bytes(0x80100000, 0x8000)) & 0xffffffff
        st = fps.get(fp, 0)
        room = r.u16(0x800b0fe2)
        cut = r.u16(0x800b0fe4)
        print("%-60s %s stage=%s raum=0x%02X -> ROOM%d%02X? cut=%d" % (
            os.path.relpath(f, ROOT) if f.startswith(ROOT) else f,
            "sauber  " if stub == 0x03e00008 else "GEPATCHT", st or "?", room, st, room, cut))
    return 0


if __name__ == "__main__":
    sys.exit(main())
