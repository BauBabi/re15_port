"""abstand6.py - NB6: Abstand Spieler <-> 0x4D je Bild im letzten Raumabschnitt eines state.log; Minimum ab Bild N.
  python abstand6.py <state.log> [ab_bild]"""
import sys, re, math
pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
an = re.compile(r"\[(\d+) t=4d [^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
ab = int(sys.argv[2]) if len(sys.argv) > 2 else 0
segs, cur, last = [], [], -1
for l in open(sys.argv[1], errors="replace"):
    if not l.startswith("F"): continue
    f = int(l.split(" ", 1)[0][1:])
    if f < last: segs.append(cur); cur = []
    last = f
    p, a = pl.search(l), an.search(l)
    if p and a:
        cur.append((f, int(p.group(1)), int(p.group(2)), int(a.group(2)), int(a.group(3))))
segs.append(cur)
v = [x for x in segs[-1] if x[0] >= ab]
d = [(math.hypot(x[1] - x[3], x[2] - x[4]), x) for x in v]
print("letzter Abschnitt: %d Bilder mit 0x4D, ab F%d: %d" % (len(segs[-1]), ab, len(v)))
for f0 in (1, 2, 3, 5, 10, 50, 100, 102, 150, 200, 300, 400):
    for dd, x in d:
        if x[0] == f0: print("  F%d Spieler (%d,%d) 0x4D (%d,%d) Abstand %.0f" % (x[0], x[1], x[2], x[3], x[4], dd))
m = min(d)
print("Minimum ab F%d: %.0f bei F%d, Maximum %.0f" % (ab, m[0], m[1][0], max(d)[0]))
