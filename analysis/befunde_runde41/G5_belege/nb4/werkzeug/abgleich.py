# Nachbesserung 4 G5A: Arme Port (Messschiene RE15_G5A_SPUR) gegen RE2-RAM s01..s26. Zuordnung Savestate -> Szenenbild ueber den
# G5-Zustand: Zeitphasen (ph 1/4/5/10/14) ueber (ph, +0x158), Clipphasen (ph 2/6/8/12) ueber (ph, +0x14D = anim_frame).
import sys, re
from re2_ram import SS, SN
spur = sys.argv[1]
P = {}
for z in open(spur):
    a = z.split(); b = int(a[0])
    gp = int(a[9]); af = int(a[7])
    arme = re.findall(r"\| a(\d) s(\d+) p(\d+) p0", z)
    m = re.search(r"\|\| g5t (-?\d+)(.*)", z)
    gt = int(m.group(1)); ms = re.findall(r"m(\d) (-?\d+) (-?\d+) (-?\d+) (-?\d+)", m.group(2))
    sc = [int(a[15 + 4 * i]) for i in range(4)]
    P[b] = dict(gp=gp, af=af, gt=gt, arme={int(i): (int(s), int(p)) for i, s, p in arme}, m={int(i): tuple(map(int, r)) for i, *r in ms}, sc=sc)
def s12(v): v &= 0xfff; return v - 4096 if v >= 2048 else v
ok = nok = 0
for n in range(3, 27):
    s = SS(SN % n); g5 = s.u32(0x800cfe1c); w = s.u32(g5 + 4); ph = (w >> 16) & 0xff; t = s.s16(g5 + 0x158); bild = s.u8(g5 + 0x14d)
    if ph in (2, 6, 8, 12): kand = [b for b in P if P[b]["gp"] == ph and P[b]["af"] == bild]
    else: kand = [b for b in P if P[b]["gp"] == ph and P[b]["gt"] == t]
    if not kand: print("s%02d G5 ph %d t %d bild %d: kein Port-Bild" % (n, ph, t, bild)); continue
    b = kand[0]; q = P[b]; zeile = []; gleich = True
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4 * i); p = s.u32(e + 0x198); a = s.u32(e + 4); k = s.u8(e + 0x218)
        r = dict(sub=(a >> 8) & 0xff, ph=(a >> 16) & 0xff, sc=s.s16(p + 0x8c), t=s.s16(e + 0x158), rx=s12(s.s16(e + 0x74)), ry=s12(s.s16(e + 0x76)), rz=s12(s.s16(e + 0x78)))
        su, pp = q["arme"][k]; mt, mx, my, mz = q["m"][k]
        pz = dict(sub=su, ph=pp, sc=q["sc"][k], t=mt, rx=s12(mx), ry=s12(my), rz=s12(mz))
        diff = [f for f in ("sub", "ph", "sc", "t", "rx", "ry", "rz") if r[f] != pz[f]]
        if diff: gleich = False
        zeile.append("A%d %s" % (k, "=" if not diff else ",".join("%s %d/%d" % (f, r[f], pz[f]) for f in diff)))
    ok += gleich; nok += not gleich
    print("s%02d -> Bild %4d (G5 ph %2d t %3d bild %3d)%s: %s" % (n, b, ph, t, bild, "" if len(kand) == 1 else " [%d Kand.]" % len(kand), " | ".join(zeile)))
print("gleich %d / %d (Feld RE2/Port)" % (ok, ok + nok))
