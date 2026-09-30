#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht) - DuckStation-Savestate: VRAM byte-true (GPU-VRAM-Tag) als PNG
plus Kopf-Globals (Kamera-Cut DAT_800b532e, Raumzeiger DAT_800ac778).

Benutzt re15_ss.py (Skill re15-savestate-ghidra) mit true_base=True
(memory reai-v2-savestate-vram-base: NIE ram_base+0x200000).

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_vram.py <sav> [<out.png>]
"""
import os, sys, zlib
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

def main():
    sav = sys.argv[1]
    r = re15_ss.Ram(sav)
    fp = zlib.crc32(r.bytes(0x80100000, 0x8000)) & 0xffffffff
    print("%-28s fp=0x%08x cut=%d vram_base=%s" % (
        os.path.basename(sav), fp, r.u16(0x800b532e),
        hex(r.vram_base) if r.vram_base is not None else None))
    if len(sys.argv) > 2:
        re15_ss.vram_png(r, sys.argv[2], true_base=True)

if __name__ == "__main__":
    main()
