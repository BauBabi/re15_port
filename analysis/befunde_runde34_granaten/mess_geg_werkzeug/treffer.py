# treffer.py <laufdir> [explosionsbild] : Auswertung eines Granaten-/FORCE_EXPLOSION-Laufs.
#  X = Explosionsbild (gr.log: erste Zeile "EV resolver" -> T -> F; sonst debug.log "[harness]
#  RE15_FORCE_EXPLOSION F<n>"; oder Argument). Zustandslog nach dem Sprung (Bildzaehler faellt zurueck).
#  Je Gegner: Zeile X-1 / X / X+1 (HP, st/ss1/ss2/ss3, Clip mo, Bild af), dann die Folge der Zustands-
#  wechsel bis Laufende, erstes Bild mit st 7 (Leiche). Dazu: Spieler-HP, EXIT_AT erreicht (kein Haenger).
import re, sys, os, math
d = sys.argv[1]
X = int(sys.argv[2]) if len(sys.argv) > 2 else None
P = None; eing = None; art = None
gr = os.path.join(d, 'gr.log')
if X is None and os.path.exists(gr):
    tf = {}
    L = open(gr).read().splitlines()
    for l in L:
        m = re.match(r'T=(\d+) F=(\d+) ', l)
        if m and int(m.group(1)) not in tf: tf[int(m.group(1))] = int(m.group(2))
    for l in L:
        m = re.match(r'T=(\d+) EV resolver art=(\d+) P=\((-?\d+),(-?\d+),(-?\d+)\) r=500 eingriffe=(\d+)', l)
        if m:
            X = tf.get(int(m.group(1))); art = int(m.group(2)); P = tuple(int(m.group(i)) for i in (3, 4, 5)); eing = int(m.group(6)); break
dbg = open(os.path.join(d, 'debug.log'), errors='replace').read()
if X is None:
    m = re.search(r'RE15_FORCE_EXPLOSION F(\d+) art=(\d+) slot=(\d+) t=(\w+) P=\((-?\d+),(-?\d+),(-?\d+)\) Treffer=(-?\d+)', dbg)
    if m: X = int(m.group(1)); art = int(m.group(2)); P = tuple(int(m.group(i)) for i in (5, 6, 7)); eing = int(m.group(8))
exit_ok = '[flow] EXIT_AT' in dbg
lines = open(os.path.join(d, 'state.log')).read().splitlines()
post = {}; prev = -1; nach = False
for l in lines:
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if not nach and prev > f: nach = True
    prev = f
    if nach and f not in post: post[f] = l
RX = re.compile(r'\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
def geg(l):
    o = {}
    for m in RX.finditer(l):
        o[int(m.group(1))] = dict(t=m.group(2), st=int(m.group(3)), ss1=int(m.group(4)), ss2=int(m.group(5)), ss3=int(m.group(6)), mo=int(m.group(8)), af=int(m.group(9)), x=int(m.group(12)), z=int(m.group(13)), hp=int(m.group(15)))
    return o
def pl(l):
    m = re.search(r'PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+)', l)
    return tuple(int(m.group(i)) for i in range(1, 9)) if m else None
fmax = max(post) if post else 0
print('Lauf %s: X=%s Art=%s P=%s eingriffe=%s EXIT_AT=%s Bilder bis %d' % (os.path.basename(d), X, art, P, eing, 'ja' if exit_ok else 'NEIN', fmax))
if X is None or X not in post: sys.exit(0)
gv, gx, gn = geg(post.get(X - 1, '')), geg(post[X]), geg(post.get(X + 1, ''))
for f in (X - 1, X, X + 1):
    print('  F%d Spieler %s' % (f, pl(post.get(f, ''))))
for s in sorted(gx):
    v, x, n = gv.get(s), gx[s], gn.get(s)
    dist = math.hypot(x['x'] - P[0], x['z'] - P[2]) if P else -1
    hit = v is not None and x['hp'] < v['hp']
    fmt = lambda e: 'hp%d st%d/%d/%d/%d mo%d af%d' % (e['hp'], e['st'], e['ss1'], e['ss2'], e['ss3'], e['mo'], e['af']) if e else '-'
    print('  [%d] t=%s d(P)=%d %s | X-1 %s | X %s | X+1 %s' % (s, x['t'], dist, 'TREFFER' if hit else 'kein Treffer', fmt(v), fmt(x), fmt(n)))
    if hit:
        last = None; seq = []
        leiche = None
        for f in range(X, fmax + 1):
            e = geg(post.get(f, '')).get(s)
            if not e: seq.append('F%d weg' % f); break
            k = (e['st'], e['ss1'], e['ss2'], e['ss3'], e['mo'])
            if k != last:
                seq.append('F%d st%d/%d/%d/%d mo%d hp%d' % (f, e['st'], e['ss1'], e['ss2'], e['ss3'], e['mo'], e['hp']))
                last = k
            if e['st'] == 7 and leiche is None: leiche = f
        print('     Folge: ' + ', '.join(seq[:18]) + (' ...' if len(seq) > 18 else ''))
        e = geg(post.get(fmax, '')).get(s)
        print('     Leiche (st 7) ab: %s; Ende F%d: %s' % (leiche, fmax, fmt(e)))
