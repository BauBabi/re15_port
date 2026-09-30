#!/usr/bin/env python3
"""ss_spieler_hitbox.py - liest aus sauberen DuckStation-Savestates die Spieler-Hitbox-Zeiger
+0x78 (0x800acacc) / +0x7c (0x800acad0), den Offsetvektor @0x800b2354 (s16 x,y,z), die
Hitbox-Struktur @0x80073e94, Spielerlage +0x34/+0x38/+0x3c, +0x93 und HP.
Runde 34 / re_schaden_resolver. Nur saubere Saves (PATCHED-EXE_* ausgeschlossen, CLAUDE.md).
"""
import os, sys, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa

saves = sys.argv[1:] or sorted(glob.glob(os.path.join(REPO, "stage_saves", "*.sav")))
for s in saves:
    if "PATCHED-EXE" in s:
        continue
    try:
        ram = re15_ss.Ram(s)
    except Exception as ex:  # noqa
        print(os.path.basename(s), "FEHLER", ex)
        continue
    p78 = ram.u32(0x800acacc); p7c = ram.u32(0x800acad0)
    off = (ram.s16(0x800b2354), ram.s16(0x800b2356), ram.s16(0x800b2358))
    hb = ram.bytes(0x80073e94, 12).hex()
    pos = (ram.s32(0x800aca88), ram.s32(0x800aca8c), ram.s32(0x800aca90))
    print(f"{os.path.basename(s):32s} +78=0x{p78:08x} +7c=0x{p7c:08x} off={off} hb@73e94={hb} "
          f"pos={pos} +93=0x{ram.u8(0x800acae7):02x} hp={ram.s16(0x800acaee)} mode={ram.u8(0x800aca58)} st={ram.u8(0x800aca59)}")
