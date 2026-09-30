#!/usr/bin/env python3
"""ss_gegner_hp.py - listet je sauberem Savestate die aktiven Gegner (Array 0x800acc2c, Stride 0x1f4,
Zahl u8 @0x800aca4e): Typ +0x8, Zustand +0x4/+0x5/+0x6, HP s16 +0x9a, +0x93, Wort +0x90, +0x9 und die
Hitbox (+0x78 -> u16 +6/+8/+10, +0x7c -> s16 x,y,z). Runde 34 / re_schaden_resolver."""
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
    except Exception:
        continue
    cnt = ram.u8(0x800aca4e)
    if cnt == 0:
        continue
    out = []
    a = 0x800acc2c
    seen = 0
    for slot in range(20):
        e = a + slot * 0x1f4
        if ram.u32(e) & 1:
            seen += 1
            p78 = ram.u32(e + 0x78); p7c = ram.u32(e + 0x7c)
            hb = (ram.u16(p78 + 6), ram.u16(p78 + 8), ram.u16(p78 + 10)) if 0x80000000 <= p78 < 0x80200000 else None
            of = (ram.s16(p7c), ram.s16(p7c + 2), ram.s16(p7c + 4)) if 0x80000000 <= p7c < 0x80200000 else None
            out.append(f"[{slot}] typ=0x{ram.u8(e+8):02x} st={ram.u8(e+4)}/{ram.u8(e+5)}/{ram.u8(e+6)}/{ram.u8(e+7)} "
                       f"hp={ram.s16(e+0x9a)} +93=0x{ram.u8(e+0x93):02x} +90=0x{ram.u32(e+0x90):08x} +9=0x{ram.u8(e+9):02x} "
                       f"hb(r1,h,r2)={hb} off={of}")
        if seen >= cnt:
            break
    print(f"{os.path.basename(s)} n={cnt}")
    for o in out:
        print("   " + o)
