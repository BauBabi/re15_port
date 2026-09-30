# spur.py <laufdir> [slots] [step] : Gegner-Spur nach dem Sprung (Lage, Zustand, Clip, HP, Abstand zu Leon)
import re, sys, math
d = sys.argv[1]; want = set(int(s) for s in sys.argv[2].split(',')) if len(sys.argv) > 2 and sys.argv[2] else None
step = int(sys.argv[3]) if len(sys.argv) > 3 else 10
lines = open(d + '/state.log').read().splitlines()
post = []; prev = -1; nach = False
for l in lines:
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if not nach and prev > f: nach = True
    prev = f
    if nach: post.append((f, l))
seen = set()
for f, l in post:
    if f in seen: continue
    seen.add(f)
    if f % step and f != 1: continue
    pl = re.search(r'PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)', l)
    out = 'F%-4d PL(%s,%s r%s hp%s st%s)' % (f, pl.group(1), pl.group(2), pl.group(3), pl.group(4), pl.group(5))
    for m in re.finditer(r'\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)', l):
        s = int(m.group(1))
        if want and s not in want: continue
        dx = int(m.group(12)) - int(pl.group(1)); dz = int(m.group(13)) - int(pl.group(2))
        out += ' | %d:t%s st%s/%s/%s/%s mo%s af%s (%s,%s) D%d hp%s' % (s, m.group(2), m.group(3), m.group(4), m.group(5), m.group(6), m.group(8), m.group(9), m.group(12), m.group(13), math.hypot(dx, dz), m.group(15))
    print(out)
