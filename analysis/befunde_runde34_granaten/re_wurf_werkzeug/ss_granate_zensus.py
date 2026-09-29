#!/usr/bin/env python3
"""Runde 34 / Granate: Zensus ueber alle SAUBEREN DuckStation-Savestates (stage_saves/, ohne die
PATCHED-EXE_*.sav): ausgeruestete Waffe (u8 0x800aca5d), Spielerstatus (u16 0x800acaec),
Inventar-Menge der Id 9, und ob im ESP-Pool 0x800a73b8 (96 x 0x84) ein Platz mit
Kategorie 4 / sub 0x0D (+0x70/+0x71) oder Routine 29/30/31 (+0x00/+0x02) lebt.
Zusaetzlich: Lichtsatz-Latch 0x800b5358. Reines Lesewerkzeug (re15_ss.Ram).

Aufruf: python ss_granate_zensus.py [stage_saves]
"""
import os, sys, importlib.util
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
spec = importlib.util.spec_from_file_location(
    "ss", os.path.join(REPO, ".claude/skills/re15-savestate-ghidra/scripts/re15_ss.py"))
ss = importlib.util.module_from_spec(spec); spec.loader.exec_module(ss)

POOL = 0x800a73b8
INV = 0x800b10ac      # Inventar-Satz [id, menge, ?, ?] x N (FUN_8004eae4 @0x8004eafc/34)

def main():
    d = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "stage_saves")
    files = sorted(f for f in os.listdir(d) if f.endswith(".sav") and not f.startswith("PATCHED-EXE"))
    n9 = 0
    for f in files:
        try:
            r = ss.Ram(os.path.join(d, f))
        except Exception as e:
            print("%-44s FEHLER %s" % (f, e)); continue
        w = r.u8(0x800aca5d); st = r.u16(0x800acaec); latch = r.u8(0x800b5358)
        inv9 = [r.u8(INV + 4 * i + 1) for i in range(10) if r.u8(INV + 4 * i) == 9]
        gran = []
        for i in range(96):
            s = POOL + 0x84 * i
            fl = r.u8(s + 0x6c)
            if not fl:
                continue
            a, b = r.u16(s), r.u16(s + 2)
            cat, sub = r.u8(s + 0x70), r.u8(s + 0x71)
            if (cat == 4 and sub == 0x0d) or a in (29, 30, 31) or b in (29, 30, 31):
                gran.append((i, fl, a, b, cat, sub))
        if w == 9 or inv9 or gran:
            n9 += 1
        print("%-44s waffe=%2d acaec=%04x latch=%d inv9=%s granaten-slots=%s"
              % (f, w, st, latch, inv9 or "-", gran or "-"))
    print("Savestates:", len(files), " mit Granate (Waffe/Inventar/Slot):", n9)

if __name__ == "__main__":
    main()
