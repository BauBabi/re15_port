# Nachbesserung 4 G5A: RE2-Haeuter FUN_800197f4 nachgebaut (Tabellen im RAM @0x801058b8/0x80105940/0x801059b8 = Datei-Link 0x801128b8.. -0xD000)
# Pruefung: aus den RAM-Teilmatrizen (+0x48 R, +0x5c t) des zuletzt gezeichneten Arms den Vertexpuffer von Mesh 0 nachrechnen.
import sys
from re2_ram import SS, SN
TAB = (0x801128b8, 0x80112940, 0x801129b8)   # RAM (EM37 bei 0x8010D000) = Datei 0x58b8/0x5940/0x59b8
def tabelle(s, a):
    return dict(A=s.u8(a), B=s.u8(a + 1), n=s.u16(a + 2), locA=s.u32(a + 0xc), locB=s.u32(a + 0x24), idx=s.u32(a + 0x34))
def teil(s, p, k):
    q = p + 172 * k
    return [s.s16(q + 0x48 + 2 * j) for j in range(9)], (s.s32(q + 0x5c), s.s32(q + 0x60), s.s32(q + 0x64))
def rt(R, v):   # gte rtv0 (ohne TR), >>12
    return [(R[3*r] * v[0] + R[3*r+1] * v[1] + R[3*r+2] * v[2]) >> 12 for r in range(3)]
def haut(s, parts, tR=None):
    out = {}
    for a in TAB:
        T = tabelle(s, a)
        RA, tA = tR[T["A"]] if tR else teil(s, parts, T["A"]); RB, tB = tR[T["B"]] if tR else teil(s, parts, T["B"])
        for j in range(T["n"]):
            vA = (s.s16(T["locA"] + 8*j), s.s16(T["locA"] + 8*j + 2), s.s16(T["locA"] + 8*j + 4))
            vB = (s.s16(T["locB"] + 8*j), s.s16(T["locB"] + 8*j + 2), s.s16(T["locB"] + 8*j + 4))
            wA = [c + t for c, t in zip(rt(RA, vA), tA)]; wB = [c + t for c, t in zip(rt(RB, vB), tB)]
            i = s.u16(T["idx"] + 4*j); w = s.u16(T["idx"] + 4*j + 2)
            out[i] = wA if w == 0x1000 else (wB if w == 0 else [(w * x + (0x1000 - w) * y) >> 12 for x, y in zip(wA, wB)])
    return out
if __name__ == "__main__":
    n = int(sys.argv[1]); slot = int(sys.argv[2])
    s = SS(SN % n)
    for a in TAB: print({k: (hex(v) if v > 0xffff else v) for k, v in tabelle(s, a).items()})
    e = s.u32(0x800cfe1c + 4 * slot); p = s.u32(e + 0x198); vb = s.u32(s.u32(p + 8))
    out = haut(s, p)
    d = [max(abs(out[i][c] - s.s16(vb + 8*i + 2*c)) for c in range(3)) for i in sorted(out)]
    print("s%02d slot %d: %d Vertices gehaeutet, max Abweichung zum RAM-Puffer %d, Mittel %.2f" % (n, slot, len(out), max(d), sum(d) / len(d)))
