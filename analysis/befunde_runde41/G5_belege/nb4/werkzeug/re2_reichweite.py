# Nachbesserung 4 G5A: RE2 gezeichnete Mesh-0-Vertices eines Arms aus dem RAM: Haeuter (re2_haut, +-1 gegen den RAM-Puffer) mit den
# RAM-Teilmatrizen, dann FUN_80019CD0-Streckung W0*S*W0^T*(v - t0) + t0 (S = Streckung +0x8c, 4096, 4096). Reichweite = max |v - t0|.
import sys, math
from re2_ram import SS, SN
from re2_haut import haut, teil
def gezeichnet(s, slot):
    e = s.u32(0x800cfe1c + 4 * slot); p = s.u32(e + 0x198); sc = s.s16(p + 0x8c)
    W, t0 = teil(s, p, 0); V = haut(s, p); out = []
    for i in sorted(V):
        d = [V[i][c] - t0[c] for c in range(3)]
        l = [(W[0 + c] * d[0] + W[3 + c] * d[1] + W[6 + c] * d[2]) >> 12 for c in range(3)]
        l[0] = (l[0] * sc) >> 12
        out.append([((W[3*r] * l[0] + W[3*r+1] * l[1] + W[3*r+2] * l[2]) >> 12) + t0[r] for r in range(3)])
    return sc, t0, out, s.u8(e + 0x218)
for n in [int(a) for a in sys.argv[1:]]:
    s = SS(SN % n)
    for slot in (1, 2, 3, 4):
        sc, t0, out, k = gezeichnet(s, slot)
        r = max(math.sqrt(sum((v[c] - t0[c]) ** 2 for c in range(3))) for v in out)
        print("s%02d Arm %d Streckung %4d t0 %s Reichweite %.0f" % (n, k, sc, t0, r))
