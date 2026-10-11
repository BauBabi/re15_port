# spur.py <gy.log> <von> <bis> <schritt> [raum]: je Bild Leon + Gorillas (Lage, Sub, d)
import re, sys, math
G = re.compile(r"\[(\d+) t=27 st=(\d+)/(\d+)/(\d+)/(\d+) [^\]]*?mo=(\d+) af=(\d+) @\((-?\d+),(-?\d+),(-?\d+)\)[^\]]*?rot=(\d+)")
H = re.compile(r"^F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+)")
fn, a, b, st = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]); raum = sys.argv[5] if len(sys.argv) > 5 else "11C0"
for z in open(fn, errors="replace"):
    m = H.match(z)
    if not m or m.group(2) != raum: continue
    f = int(m.group(1))
    if f < a or f > b or (f - a) % st: continue
    px, pz = int(m.group(3)), int(m.group(5))
    out = "F%d Leon (%d,%d)" % (f, px, pz)
    for g in G.finditer(z):
        s = int(g.group(1)); x, zz = int(g.group(8)), int(g.group(10))
        out += " | S%d %s/%s/%s/%s mo%s @(%d,%d) r%s d%d" % (s, g.group(2), g.group(3), g.group(4), g.group(5), g.group(6), x, zz, g.group(11), math.hypot(x - px, zz - pz))
    print(out)
