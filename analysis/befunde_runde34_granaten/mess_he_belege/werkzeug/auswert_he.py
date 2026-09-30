# auswert_he.py <laufdir> [--voll]
# mess_he (Runde 34): Zeitlinie + Pruefpunkte eines Handgranaten-Laufs aus den Mess-Logs der echten exe.
#   state.log (RE15_STATE_LOG), wf.log (RE15_WAFFEN_LOG), gr.log (RE15_GRANATE_LOG), fx.log (RE15_FX_LOG), debug.log.
# Nur Lesen, kein Verhalten. Bildnummern = Spielbilder NACH dem Debug-Sprung (Zaehler-Reset beim Raumwechsel).
import re, sys, os, math
from collections import defaultdict, OrderedDict

d = sys.argv[1]
voll = '--voll' in sys.argv
def lies(n):
    p = os.path.join(d, n)
    return open(p, errors='replace').read().splitlines() if os.path.exists(p) else []

# ---------- state.log nach dem Sprung (erster Rueckgang des Bildzaehlers) ----------
st = lies('state.log')
post = []; vor = -1; nach = False
for l in st:
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if not nach and f < vor: nach = True
    vor = f
    if nach: post.append((f, l))
Z = OrderedDict()
for f, l in post:
    if f not in Z: Z[f] = l
RXG = re.compile(r'\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
def gegner(l):
    o = {}
    for m in RXG.finditer(l or ''):
        o[int(m.group(1))] = dict(t=m.group(2), st=int(m.group(3)), ss1=int(m.group(4)), ss2=int(m.group(5)),
                                  ss3=int(m.group(6)), g=m.group(7), mo=int(m.group(8)), af=int(m.group(9)),
                                  d=int(m.group(11)), x=int(m.group(12)), z=int(m.group(13)), r=int(m.group(14)),
                                  hp=int(m.group(15)))
    return o
def spieler(l):
    m = re.search(r'PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(-?\d+) ps1=(-?\d+) ps2=(-?\d+) mo=(-?\d+) ac=(-?\d+) fx=(-?\d+) mg=(-?\d+)', l or '')
    if not m: return None
    k = ['x','z','rot','hp','pst','ps1','ps2','mo','ac','fx','mg']
    o = {k[i]: int(m.group(i+1)) for i in range(len(k))}
    g = re.search(r' gr=(\d+)', l); o['gr'] = int(g.group(1)) if g else -1
    return o

# ---------- Abzug A ----------
A = None; alt = None
for f, l in Z.items():
    p = spieler(l)
    if p is None: continue
    if alt is not None and p['mg'] < alt:
        A = f; break
    alt = p['mg']

# ---------- wf.log nach dem Sprung ----------
wf = lies('wf.log'); wpost = []; vor = -1; nach = False; cur = None
for l in wf:
    m = re.match(r'F(\d+) ', l)
    if m:
        f = int(m.group(1))
        if not nach and f < vor: nach = True
        vor = f; cur = f
        if nach: wpost.append((f, l))
    elif nach:
        wpost.append((cur, l))
wl = {}
for f, l in wpost:
    if l.startswith('F') and f not in wl: wl[f] = l
def wfeld(f, k):
    m = re.search(r' %s=(-?\d+)' % k, wl.get(f, ''))
    return int(m.group(1)) if m else None
ses = [(f, l.strip()) for f, l in wpost if 'SE  esp code=' in l or 'SE  re2fx code=' in l]
spawns_wf = [(f, l.strip()) for f, l in wpost if 'SPAWN id=' in l]

# ---------- gr.log ----------
gr = lies('gr.log')
tf = {}
for l in gr:
    m = re.match(r'T=(\d+) F=(\d+) slot=', l)
    if m and int(m.group(1)) not in tf: tf[int(m.group(1))] = int(m.group(2))
S = None; spawn_l = None
ev = []
for l in gr:
    m = re.match(r'F=(\d+) SPAWN granate', l)
    if m and S is None: S = int(m.group(1)); spawn_l = l
    m = re.match(r'T=(\d+) EV (.*)$', l)
    if m: ev.append((int(m.group(1)), m.group(2)))
# EV-Ticks ohne Platzzeile (nach "frei"): T -> F ueber den letzten bekannten Tick + Differenz
def t2f(t):
    if t in tf: return tf[t]
    ks = [k for k in tf if k < t]
    if not ks: return None
    k = max(ks); return tf[k] + (t - k)
kontakte = [(t2f(t), e) for t, e in ev if e.startswith('se code=010a')]
L = next((f for f, e in kontakte if e.endswith('liegen')), None)
X = next((t2f(t) for t, e in ev if e.startswith('resolver')), None)
P = None
for t, e in ev:
    m = re.match(r'resolver art=(\d+) P=\((-?\d+),(-?\d+),(-?\d+)\)', e)
    if m: P = tuple(int(m.group(i)) for i in (2,3,4)); break
frei = next((t2f(t) for t, e in ev if e.startswith('frei')), None)
kinder = [(t2f(t), e) for t, e in ev if e.startswith('kind')]
# Flugsaetze je Bild (Platzzeilen des Granatenplatzes)
satz = OrderedDict(); fl = OrderedDict(); zaehl = OrderedDict(); wp = OrderedDict()
for l in gr:
    m = re.match(r'T=(\d+) F=(\d+) slot=(\d+) art=(\d+) A=(\d+) B=(\d+) fl=(\w+) zuender=(\d+) zaehler=(\d+) wpos=\((-?\d+),(-?\d+),(-?\d+)\).* satz=(-?\d+)', l)
    if m:
        f = int(m.group(2)); satz[f] = int(m.group(13)); fl[f] = m.group(7); zaehl[f] = int(m.group(9))
        wp[f] = (int(m.group(10)), int(m.group(11)), int(m.group(12)))

# ---------- debug.log ----------
dbg = lies('debug.log')
licht = [l for l in dbg if l.startswith('[licht]')]
exit_at = [l for l in dbg if 'EXIT_AT' in l]

# ---------- fx.log (Zeichnen) ----------
fxd = defaultdict(list)
for l in lies('fx.log'):
    m = re.match(r'id=(\d+) sub=(\d+) eidx=(-?\d+) frame=(\d+) .* slot=(\d+) q=\d+ wpos=\((-?\d+),(-?\d+),(-?\d+)\) A=(\d+) B=(\d+) zuender=(\d+) zaehler=(\d+) fl=(\w+) art=(\d+) F=(\d+)', l)
    if m:
        fxd[int(m.group(15))].append((int(m.group(1)), int(m.group(2)), int(m.group(4)), m.group(13), int(m.group(14))))

print('=== Lauf', os.path.basename(d.rstrip('/')))
env = {l.split('=',1)[0]: l.split('=',1)[1] for l in lies('env.txt') if '=' in l}
for k in ('RE15_AI_FLAVOR','RE15_PLAYER_POS','RE15_INPUT_SCRIPT','RE15_INPUT_SCRIPT_START','RE15_EXIT_AT','RE15_FRAMEDUMP'):
    if k in env: print('  %s=%s' % (k, env[k]))
print('  lauf_rc:', ' '.join(lies('lauf_rc.txt')))
print('  EXIT_AT-Zeile:', exit_at[-1] if exit_at else 'FEHLT')
print('A (Abzug, mg faellt) =', A)
if A is not None:
    c1 = wfeld(A+1, 'clip'); fc1 = wfeld(A+1, 'fc'); fr1 = wfeld(A+1, 'frame')
    print('  Wurfclip (wf F%d): clip=%s fc=%s frame=%s' % (A+1, c1, fc1, fr1))
    # Clipbild im Spawnbild
    if S is not None:
        print('  wf F%d (Spawnbild): clip=%s frame=%s' % (S, wfeld(S,'clip'), wfeld(S,'frame')))
    # Ende des Wurfclips
    ende = None
    for f in range(A+1, A+60):
        if wfeld(f,'clip') is not None and wfeld(f,'clip') != c1: ende = f; break
    print('  Wurfclip laeuft bis wf F%s (dann clip=%s)' % ((ende-1) if ende else '?', wfeld(ende,'clip') if ende else '?'))
    za = gegner(Z.get(A)); zv = gegner(Z.get(A-1))
    verl = [(s, zv[s]['hp'], za[s]['hp']) for s in za if s in zv and za[s]['hp'] < zv[s]['hp']]
    print('  Schaden im Abzugsbild:', verl if verl else 'KEINER', '| Gegner in A:', ', '.join('%d:t%s d=%d hp=%d' % (s, g['t'], g['d'], g['hp']) for s, g in sorted(za.items())))
print('S (Spawn) =', S, '-> S-A =', (S - A) if (S is not None and A is not None) else None, '|', spawn_l)
print('Kontakte (F, rel. S):', [(f, f - S if S else None, e.split()[1]) for f, e in kontakte])
print('L =', L, '(L-S = %s)' % ((L - S) if (L and S) else None), '| X =', X, '(X-L = %s, X-S = %s, X-A = %s)' % ((X-L) if (X and L) else None, (X-S) if (X and S) else None, (X-A) if (X and A) else None))
print('P =', P, '| frei =', frei, '(frei-X = %s)' % ((frei - X) if (frei and X) else None))
print('Kinder:', [(f, (f - X) if X else None, e.split()[1]) for f, e in kinder])
print('Ereignisse im Explosionstick:', [e for t, e in ev if X and t2f(t) == X])
# Flug-Saetze S..L
if S is not None and L is not None:
    seq = [satz.get(f) for f in range(S, L+1)]
    print('Flug-Saetze S..L:', seq)
    ok = all(23 <= s <= 34 for s in seq if s is not None)
    zyk = [f for f in range(S, L) if satz.get(f) == 23]
    print('  Satz 23 in Bildern:', zyk, '(Abstaende', [b - a for a, b in zip(zyk, zyk[1:])], ') Bereich 23..34:', ok)
    print('  Flags im Flug:', sorted(set(fl[f] for f in range(S, L) if f in fl)), '| ab L:', sorted(set(fl[f] for f in fl if f >= L)))
    print('  Liegestelle wpos(L) =', wp.get(L))
print('SEs (wf.log, Bild, rel. S):')
for f, s in ses:
    print('   F%s (S+%s) %s' % (f, (f - S) if (S is not None and f is not None) else '?', s))
print('Licht-Latch-Zeilen:', licht if licht else 'KEINE')
if X is not None:
    for f in (X-1, X, X+1, X+2):
        l = Z.get(f)
        if not l: continue
        p = spieler(l)
        print(' F%d PL=(%d,%d) rot=%d hp=%d mo=%d gr=%d dP=%s' % (f, p['x'], p['z'], p['rot'], p['hp'], p['mo'], p['gr'],
              int(math.hypot(p['x'] - P[0], p['z'] - P[2])) if P else None))
        for s, g in sorted(gegner(l).items()):
            print('    [%d] t=%s st=%d ss1=%d ss2=%d ss3=%d mo=%d af=%d g=%s hp=%d @(%d,%d) dP=%d' % (
                s, g['t'], g['st'], g['ss1'], g['ss2'], g['ss3'], g['mo'], g['af'], g['g'], g['hp'], g['x'], g['z'],
                int(math.hypot(g['x'] - P[0], g['z'] - P[2])) if P else -1))
    gx = gegner(Z.get(X)); gv = gegner(Z.get(X-1))
    hit = [s for s in gx if s in gv and gx[s]['hp'] < gv[s]['hp']]
    print('Getroffen im Bild X:', hit)
    fs = sorted(Z)
    for s in hit:
        leiche = next((f for f in fs if f > X and gegner(Z[f]).get(s, {}).get('st') == 7), None)
        # Clipfolge nach X
        folge = []; last = None
        for f in fs:
            if f < X: continue
            g = gegner(Z[f]).get(s)
            if not g: continue
            k = (g['st'], g['ss1'], g['ss2'], g['mo'])
            if k != last: folge.append((f, 'st%d ss1=%d ss2=%d mo=%d hp=%d' % (g['st'], g['ss1'], g['ss2'], g['mo'], g['hp']))); last = k
        print('  Slot %d: HP %d -> %d, Leiche (st 7) ab %s; Folge:' % (s, gv[s]['hp'], gx[s]['hp'], leiche))
        for f, t in folge[:14]: print('     F%d %s' % (f, t))
    # Spieler ueber den Lauf
    fs = sorted(Z); ph = None; pm = None
    print('Spieler-HP/Clip-Wechsel ab X-2:')
    for f in fs:
        if f < X - 2: continue
        p = spieler(Z[f])
        if p is None: continue
        if (p['hp'], p['mo'], p['pst']) != (ph, pm, None):
            if p['hp'] != ph or p['mo'] != pm:
                print('   F%d hp=%d pst=%d ps1=%d mo=%d gr=%d @(%d,%d)' % (f, p['hp'], p['pst'], p['ps1'], p['mo'], p['gr'], p['x'], p['z']))
            ph, pm = p['hp'], p['mo']
# FX-Zeichnen: Feuerball / Rauch / Granate
if X is not None:
    def spanne(pred):
        fr = sorted(f for f in fxd if any(pred(e) for e in fxd[f]))
        return (fr[0], fr[-1], len(fr)) if fr else None
    print('FX gezeichnet: Feuerball id3/25:', [(f, [e[2] for e in fxd[f] if e[0]==3 and e[1]==25]) for f in sorted(fxd) if any(e[0]==3 and e[1]==25 for e in fxd[f])][:3], '...')
    fb = sorted(f for f in fxd if any(e[0]==3 and e[1]==25 for e in fxd[f]))
    rb = sorted(f for f in fxd if any(e[0]==3 and e[1]==11 for e in fxd[f]))
    gb = sorted(f for f in fxd if any(e[0]==4 and e[1]==13 for e in fxd[f]))
    print('  Feuerball-Bilder: %s..%s (%d Bilder, rel X %s..%s), max gleichzeitig %d' % (fb[0] if fb else None, fb[-1] if fb else None, len(fb),
          (fb[0]-X) if fb else None, (fb[-1]-X) if fb else None, max((sum(1 for e in fxd[f] if e[0]==3 and e[1]==25) for f in fb), default=0)))
    print('  Rauch-Bilder: %s..%s (%d, rel X %s..%s)' % (rb[0] if rb else None, rb[-1] if rb else None, len(rb), (rb[0]-X) if rb else None, (rb[-1]-X) if rb else None))
    print('  Granaten-Sprite gezeichnet: %s..%s (%d Bilder), im Bild X: %s' % (gb[0] if gb else None, gb[-1] if gb else None, len(gb), X in gb))
    andere = defaultdict(list)
    for f in sorted(fxd):
        if f < X: continue
        for e in fxd[f]:
            if (e[0], e[1]) not in ((3,25),(3,11),(4,13)): andere[(e[0],e[1])].append(f)
    for k, v in andere.items(): print('  weitere FX id%d/%d: F%d..F%d (%d Zeilen)' % (k[0], k[1], v[0], v[-1], len(v)))
print('SPAWN-Zeilen wf.log:', [(f, s) for f, s in spawns_wf][:12])
