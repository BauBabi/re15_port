# auswert.py <laufdir> : Explosion (X, P) aus gr.log, Gegner-Lage/HP in den Zeilen X-1/X/X+1 (state.log nach dem Sprung)
import re, sys, math, os
d = sys.argv[1]
gr = open(os.path.join(d, 'gr.log')).read().splitlines() if os.path.exists(os.path.join(d,'gr.log')) else []
tf = {}
for l in gr:
    m = re.match(r'T=(\d+) F=(\d+) ', l)
    if m: tf[int(m.group(1))] = int(m.group(2))
X = P = L = S = None
for l in gr:
    m = re.match(r'T=(\d+) EV resolver art=(\d+) P=\((-?\d+),(-?\d+),(-?\d+)\)', l)
    if m and X is None:
        X = tf.get(int(m.group(1))); P = tuple(int(m.group(i)) for i in (3,4,5)); art = int(m.group(2))
    m = re.match(r'T=(\d+) EV se code=\w+ pos=.* liegen', l)
    if m and L is None: L = tf.get(int(m.group(1)))
    m = re.match(r'F=(\d+) SPAWN granate art=(\d+)', l)
    if m and S is None: S = int(m.group(1))
lines = open(os.path.join(d, 'state.log')).read().splitlines()
# nach dem Sprung: zweite Folge ab F1
post = []; seen = False
for i, l in enumerate(lines):
    if l.startswith('F250 '): seen = True; continue
    if seen: post.append(l)
byf = {}
for l in post:
    m = re.match(r'F(\d+) ', l)
    if m and int(m.group(1)) not in byf: byf[int(m.group(1))] = l
def gegner(l):
    out = {}
    for m in re.finditer(r'\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)', l):
        out[int(m.group(1))] = dict(t=m.group(2), st=int(m.group(3)), ss1=int(m.group(4)), mo=int(m.group(8)), x=int(m.group(12)), z=int(m.group(13)), hp=int(m.group(15)))
    return out
def pl(l):
    m = re.search(r'PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)', l); return tuple(int(m.group(i)) for i in (1,2,3,4)) if m else None
print('Spawn S=%s Liegen L=%s Explosion X=%s P=%s' % (S, L, X, P))
if X is None: sys.exit(0)
for f in (X-1, X, X+1):
    l = byf.get(f)
    if not l: continue
    g = gegner(l)
    print(' F%d PL=%s' % (f, pl(l)))
    for k, v in sorted(g.items()):
        dist = math.hypot(v['x']-P[0], v['z']-P[2])
        print('   [%d] t=%s st=%d ss1=%d mo=%d hp=%d @(%d,%d) d(P)=%d' % (k, v['t'], v['st'], v['ss1'], v['mo'], v['hp'], v['x'], v['z'], dist))
