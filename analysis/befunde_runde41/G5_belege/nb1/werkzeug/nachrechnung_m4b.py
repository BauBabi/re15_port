"""M4b: posierte Wurzel-Y je Szenenbild. Port heute = Pose aus (motion, anim_frame) NACH dem KI-Takt (main.c Zeichner);
RE2 = Pose aus +0x14D VOR dem Vorschub (FUN_8002959c @0x800295f8 sw a2,0x178 / FUN_80029614 posiert, dann +0x14D++ @0x80029b28-48),
[T3] faellt in [T4] durch (0x8010136c..8c ohne Sprung, 0x80101390 = Tabelle[4])."""
import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
s = SS(SN % 5)
EDD, EMR = 0x80125d94, 0x80127304
def kfy(clip, f):
    w = s.u32(EDD + 4 * clip); n, off = w & 0xffff, w >> 16
    k = s.u32(EDD + off + 4 * (f % n)) & 0xfff
    return k, s.s16(EMR + 0x20 + 24 * k + 2)
print("RE2-RAM: posiertes Bild (+0x178) gegen +0x14D und Teil-0-Wurzel-Y:")
for n in (5, 6, 8, 9, 14, 18):
    t = SS(SN % n); g5 = t.u32(0x800cfe1c); c = t.u8(g5 + 0x14c); f = t.u8(g5 + 0x14d)
    off = t.u32(EDD + 4 * c) >> 16
    posed = (t.u32(g5 + 0x178) - (EDD + off)) // 4
    print("  s%02d Clip %d +0x14D %3d  +0x178 -> Bild %3d  Teil0 y %d  (kf(+0x14D-1).y %d, kf(+0x14D).y %d)" % (
        n, c, f, posed, t.s32(t.u32(g5 + 0x198) + 0x30), kfy(c, f - 1)[1], kfy(c, f)[1]))
P = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb1_b/spur_5090.txt"
rows = {int(l.split()[0]): l.split() for l in open(P)}
print("\nPort-Spur 5090 (test_r41_g5auftritt, RE15_G5A_SPUR), Pose heute vs. RE2-Regel:")
for b in range(266, 275):
    r = rows[b]; m, af, ph = int(r[6]), int(r[7]), int(r[9])
    k, y = kfy(m, af)
    # RE2-Regel: Takt b<=270 posiert Clip 1 Bild (b-91); Takt 271 = [T3]+[T4] posiert Clip 3 Bild 0, danach Clip 3 Bild b-271
    if b <= 270: rc, rf = 1, b - 91
    else: rc, rf = 3, b - 271
    k2, y2 = kfy(rc, rf)
    print("  Bild %d ph %d  Port: Clip %d Bild %3d kf %3d Wurzel-y %d   | RE2: Clip %d Bild %3d kf %3d Wurzel-y %d%s" % (
        b, ph, m, af, k, y, rc, rf, k2, y2, "   <-- Sprung %+d" % (y - y2) if y != y2 else ""))
print("\nzweite Stelle [T6]->[T7] (Schnitt 17):")
for b in range(649, 654):
    r = rows[b]; m, af, ph = int(r[6]), int(r[7]), int(r[9])
    k, y = kfy(m, af)
    print("  Bild %d ph %d  Port: Clip %d Bild %3d Wurzel-y %d" % (b, ph, m, af, y))
