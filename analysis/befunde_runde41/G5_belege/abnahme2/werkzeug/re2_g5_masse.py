# Abnahme 2: RE2-RAM (Savestates der Abnahme 1, Lauf 2) - G5 Slot 0: Lage, Richtung, Routine, Clip/Bild, Teil-Winkel,
# Teil-Weltlagen, Morph-Gewichte (part2+0x88 -> Record, u16 @+12/+16/+20/+24; g5-morph.md Tab. Record), Bounding-Box Mesh 2
import sys, struct
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/analysis/befunde_runde41/G5_belege/nb1/werkzeug")
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
for n in [int(a) for a in sys.argv[1:]]:
    s = SS(SN % n)
    g5 = s.u32(0x800cfe1c)
    pl = s.u32(0x800cfe14)
    parts = s.u32(g5 + 0x198)
    print("== s%02d Schnitt %d Leon x %d | G5 (%d,%d,%d) rot(%d,%d,%d) Wort %08x t158 %d clip %d bild %d 178 %d 14e %d" % (
        n, s.u16(0x800d4820), s.s32(pl + 0x38), s.s32(g5 + 0x38), s.s32(g5 + 0x3c), s.s32(g5 + 0x40), s.s16(g5 + 0x74), s.s16(g5 + 0x76),
        s.s16(g5 + 0x78), s.u32(g5 + 4), s.s16(g5 + 0x158), s.u8(g5 + 0x14c), s.u8(g5 + 0x14d), s.u8(g5 + 0x178), s.u8(g5 + 0x14e)))
    for p in range(3):
        b = parts + p * 172
        print("   p%d ang(%d,%d,%d) welt(%d,%d,%d) flags %04x" % (p, s.s16(b + 104), s.s16(b + 106), s.s16(b + 108),
              s.s32(b + 92), s.s32(b + 96), s.s32(b + 100), s.u16(b + 0x7c) if False else 0))
    ctrl = s.u32(parts + 2 * 172 + 136)
    w = [s.s16(ctrl + 12 + 4 * i) for i in range(4)]
    print("   Morph-Record @%08x Gewichte %s" % (ctrl, w))
