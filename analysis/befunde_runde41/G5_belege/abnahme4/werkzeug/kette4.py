# Abnahme 4: Teil-Kette der Arme aus dem RE2-RAM: Gelenkabstaende |p_i - p_(i-1)| (Teil-Lage +0x5c/+0x60/+0x64 der Teilstruktur, 172 Byte je Teil,
# Teile ab *(Arm+0x198)) gegen lokale X-Versaetze (+0x2c je Teil) * Streckung/4096 - prueft die gestreckte Kette (FUN_80027434 Flag 0x800).
import sys, os, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2ram4 import Ram, SN
for n in [int(a) for a in sys.argv[1:]]:
    r = Ram(SN % n)
    print("s%02d" % n)
    for i in range(1, 5):
        e = r.u32(0x800cfe1c + 4 * i); t0 = r.u32(e + 0x198)
        S = r.s16(t0 + 0x8c)
        p = [(r.s32(t0 + k * 172 + 0x5c), r.s32(t0 + k * 172 + 0x60), r.s32(t0 + k * 172 + 0x64)) for k in range(4)]
        fl = [r.u32(t0 + k * 172) & 0xffff for k in range(4)]
        lx = [r.s16(t0 + k * 172 + 0x2c) for k in range(4)]
        sk = [(r.s16(t0 + k * 172 + 0x8c)) for k in range(4)]
        d = [math.dist(p[k], p[k - 1]) for k in range(1, 4)]
        soll = [abs(lx[k]) * S / 4096 for k in range(1, 4)]
        print("  Arm %d S %5d Flags %s Streck je Teil %s lokal x %s | |p_i-p_i-1| %s  (lokal*S/4096 %s) |p3-p0| %.0f" % (i - 1, S, ["%04x" % f for f in fl], sk, lx,
              ["%.0f" % x for x in d], ["%.0f" % x for x in soll], math.dist(p[3], p[0])))
