#!/usr/bin/env python3
"""ss_ents.py - Gegenpruefung Runde 34: Gegner-Array + Spieler aus DuckStation-Savestates lesen.

Je aktivem Platz (0x800acc2c, Stride 0x1f4, Wort0 Bit0): Typ +0x8, +0x4/5/6/7, HP +0x9a (s16),
+0x93, Wort +0x90, +0x78 (Box-Zeiger) + Box-Bytes, +0x7c (Versatz-Zeiger) + Versatz (3 x s16),
+0x188 (Teile-Zeiger), Dispatch 0x80072bac[Typ]. Spieler analog (0x800aca54).
Ueberspringt PATCHED-EXE-Saves (CLAUDE.md). Aufruf: python ss_ents.py [save ...]  (Default: alle)
"""
import os, sys, glob

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

ENT0, STRIDE, PL = 0x800acc2c, 0x1f4, 0x800aca54


def box(r, p):
    try:
        return [r.s16(p + 2 * i) for i in range(6)]
    except Exception:
        return None


def dump(path):
    try:
        r = re15_ss.Ram(path)
    except Exception as ex:
        print(f"## {os.path.basename(path)}: FEHLER {ex}")
        return
    cnt = r.u8(0x800aca4e)
    print(f"## {os.path.basename(path)}  aktiv(0x800aca4e)={cnt}  0x800b52c4=0x{r.u32(0x800b52c4):08x}")
    p = PL
    b = r.u32(p + 0x78); o = r.u32(p + 0x7c)
    print(f"  SPIELER +4/5/6={r.u8(p+4)}/{r.u8(p+5)}/{r.u8(p+6)} hp={r.s16(p+0x9a)} +93=0x{r.u8(p+0x93):02x}"
          f" +78=0x{b:08x} box={box(r,b) if 0x80000000<=b<0x80200000 else '-'}"
          f" +7c=0x{o:08x} ofs={box(r,o)[:3] if 0x80000000<=o<0x80200000 else '-'} +188=0x{r.u32(p+0x188):08x}")
    for i in range(34):
        e = ENT0 + i * STRIDE
        if not (r.u32(e) & 1):
            continue
        t = r.u8(e + 8)
        b = r.u32(e + 0x78); o = r.u32(e + 0x7c)
        disp = r.u32(0x80072bac + 4 * t)
        bx = box(r, b) if 0x80000000 <= b < 0x80200000 else None
        ov = box(r, o)[:3] if 0x80000000 <= o < 0x80200000 else None
        print(f"  [{i:2d}] @0x{e:08x} typ=0x{t:02x} disp=0x{disp:08x} +4/5/6/7={r.u8(e+4)}/{r.u8(e+5)}/{r.u8(e+6)}/{r.u8(e+7)}"
              f" hp={r.s16(e+0x9a)} +93=0x{r.u8(e+0x93):02x} +90=0x{r.u32(e+0x90):08x} +9=0x{r.u8(e+9):02x}"
              f" +78=0x{b:08x} box={bx} +7c=0x{o:08x} ofs={ov} +188=0x{r.u32(e+0x188):08x} yaw=0x{r.u16(e+0x6a):03x}")


def main():
    paths = sys.argv[1:] or sorted(glob.glob(os.path.join(REPO, "stage_saves", "*.sav")))
    for pth in paths:
        if "PATCHED-EXE" in os.path.basename(pth):
            continue
        if not os.path.isabs(pth):
            pth = os.path.join(REPO, pth)
        dump(pth)


if __name__ == "__main__":
    main()
