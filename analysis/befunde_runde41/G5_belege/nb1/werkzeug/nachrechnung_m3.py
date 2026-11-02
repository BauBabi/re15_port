"""M3 Nachrechnung: RE2 FUN_80015e7c (a3=0) + FUN_8002959c + FUN_800152c8 mit den EM36-Keyframes aus dem RE2-RAM
gegen die G5-Lage +0x38 der Savestates; daneben die Port-Regel (Anker) alt und neu."""
import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
s = SS(SN % 5)
EDD, EMR = 0x80125d94, 0x80127304          # G5 +0x17c / +0x108 (EMD-Abschnitte 1/2 im RAM)
FOFF, ESZ = s.u16(EMR + 2), (s.u32(EMR + 4) >> 18) * 4   # @0x80015edc lhu 2(a1), @0x80015ec8-d0 lw 4(a1) >>18
def spur(clip):
    w = s.u32(EDD + 4 * clip); n, off = w & 0xffff, w >> 16
    return [s.s16(EMR + FOFF + ESZ * (s.u32(EDD + off + 4 * f) & 0xfff) + 6) for f in range(n)]

def re2(x0, clip, ticks):
    """je Takt: e7c liest Bild +0x14D (vor dem Vorschub), Bild 0 -> Vorwert 0; 959c zaehlt +0x14D hoch; 52c8 addiert."""
    sp = spur(clip); prev = 0; x = x0; fr = 0
    for _ in range(ticks):
        if fr == 0: prev = 0                      # @0x80015f04-1c
        d = sp[fr] - prev; prev = sp[fr]          # @0x80015fcc / sh 0x144 @0x80015fd8
        fr = (fr + 1) % len(sp)                   # @0x80029b28-48
        x += d                                    # @0x80015314-20
    return x, fr, prev

def port_alt(x0, clip, ticks):
    sp = spur(clip); prev = sp[0]; x = x0; fr = 0  # Anker: prev = speed[aktuelles Bild 0]
    for _ in range(ticks):
        d = sp[fr] - prev; prev = sp[fr]; fr = (fr + 1) % len(sp); x += d
    return x

print("Clip 1 Spur: Bild0 %d, Bild38 %d, Bild179 %d" % (spur(1)[0], spur(1)[38], spur(1)[179]))
print("Clip 4 Spur: Bild0 %d, Bild179 %d" % (spur(4)[0], spur(4)[179]))
print("\n[T2] Clip 1 ab -9000 (Takte = +0x14D im Savestate):")
for sn in (5, 6, 7, 8):
    t = SS(SN % sn); g5 = t.u32(0x800cfe1c); n = t.u8(g5 + 0x14d)
    x, fr, prev = re2(-9000, 1, n)
    print("  s%02d +0x14D %3d  RAM x %6d  +0x20C %6d | RE2-Modell x %6d prev %6d | Port alt %6d" % (
        sn, n, t.s32(g5 + 0x38), t.s16(g5 + 0x20c), x, prev, port_alt(-9000, 1, n)))
xe, _, _ = re2(-9000, 1, 180)
print("  Clipende (180 Takte): RE2-Modell %d (RAM s09..s12 -2786) | Port alt %d (Port-Spur Bild 270 -1986)" % (xe, port_alt(-9000, 1, 180)))
print("\n[T6] Clip 4 ab -2786:")
for sn in (13, 14, 15, 16):
    t = SS(SN % sn); g5 = t.u32(0x800cfe1c); n = t.u8(g5 + 0x14d)
    x, fr, prev = re2(-2786, 4, n)
    print("  s%02d +0x14D %3d  RAM x %6d  +0x20C %6d | RE2-Modell x %6d prev %6d" % (sn, n, t.s32(g5 + 0x38), t.s16(g5 + 0x20c), x, prev))
x6, _, _ = re2(-2786, 4, 180)
print("  Clipende: RE2-Modell %d (RAM s17..s26 1160)" % x6)
