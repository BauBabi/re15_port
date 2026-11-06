# Nachbesserung 4 G5A: C-Tabellen fuer den Riegel aus dem RE2-RAM (Savestates s03..s26 der Abnahme 1), keine Handabschrift.
import math
from re2_ram import SS, SN
from re2_reichweite import gezeichnet
BILD = {3: 31, 4: 80, 5: 129, 6: 165, 7: 215, 8: 264, 9: 313, 10: 363, 11: 412, 12: 441, 13: 479, 14: 529, 15: 578, 16: 628,
        17: 677, 18: 727, 19: 776, 20: 812, 21: 854, 26: 1101}
OHNE_T = {21, 26}           # RE2-Sender 0x80104E9C schreibt nur +4 (Port-Kommando nullt den Zaehler); sub10/sub6 lesen ihn nicht
def s12(v): v &= 0xfff; return v - 4096 if v >= 2048 else v
print("static const struct { int s, bild, g5ph, g5t, g5bild; struct { int sub, ph, sc, t, rx, ry, rz; } a[4]; } k_re2_arme[] = {")
for n, b in BILD.items():
    s = SS(SN % n); g5 = s.u32(0x800cfe1c); w = s.u32(g5 + 4)
    arme = [None] * 4
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4 * i); p = s.u32(e + 0x198); a = s.u32(e + 4)
        arme[s.u8(e + 0x218)] = "{ %d, %d, %d, %d, %d, %d, %d }" % ((a >> 8) & 0xff, (a >> 16) & 0xff, s.s16(p + 0x8c), -1 if n in OHNE_T else s.s16(e + 0x158),
                                                                 s12(s.s16(e + 0x74)), s12(s.s16(e + 0x76)), s12(s.s16(e + 0x78)))
    print("    { %2d, %4d, %2d, %3d, %3d, { %s } }," % (n, b, (w >> 16) & 0xff, s.s16(g5 + 0x158), s.u8(g5 + 0x14d), ", ".join(arme)))
print("};")
print("static const struct { int s, bild; int r[4]; } k_re2_reichweite[] = {   /* Streckung 0 in diesem und dem Vorbild */")
for n in (5, 9, 10, 17, 18, 19, 20):
    s = SS(SN % n); r = [-1] * 4
    for slot in (1, 2, 3, 4):
        sc, t0, out, k = gezeichnet(s, slot)
        if sc == 0: r[k] = int(round(max(math.sqrt(sum((v[c] - t0[c]) ** 2 for c in range(3))) for v in out)))
    print("    { %2d, %4d, { %s } }," % (n, BILD[n], ", ".join(str(x) for x in r)))
print("};")
