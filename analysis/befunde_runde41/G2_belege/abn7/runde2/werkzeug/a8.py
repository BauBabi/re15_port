# Abnahme 8 (eigenes Werkzeug, unabhaengig von gtrlog des Bauers): wertet state.log + climb.log (zeilengleich) eines exe-Laufs aus.
# Letzter Besuch in ROOM2090/2091 (F beginnt je Raum bei 1). Treffer = hp-Flanke (hp sinkt) im state.log.
# Je Treffer: Abstand zum vorigen Treffer, Gator-Clip/Bild, gleicher Gator-Clip-Lauf?, Reaktion (pst/ps1, Spieler-Clips), Dauer bis pst wieder 1,
# Weg (Summe der Bildschritte) und Luftlinie, Sturz (y/Band aus climb.log). Dazu RE1.5-Niederwurf-Bilder (Spieler-Clip 12..16 mit pst 2).
# Aufruf: python a8.py <laufordner> [ -v ]
import sys, re, math, os
d = sys.argv[1]; V = '-v' in sys.argv
st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
cl = open(os.path.join(d, 'climb.log'), encoding='latin-1').read().splitlines()
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+)')
rg = re.compile(r'\[\d+ t=(\d+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=\S+ mo=(\d+) af=(\d+) stun=(\d+) d=\S+ @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
rc = re.compile(r'^T(\d+) room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\) rot=(-?\d+) band=(-?\d+) sub=(-?\d+)')
fr = []
for i, z in enumerate(st):
    m = rs.match(z)
    if not m: continue
    g = rg.search(z)
    c = rc.match(cl[i]) if i < len(cl) else None
    fr.append(dict(i=i, F=int(m.group(1)), x=int(m.group(2)), z=int(m.group(3)), rot=int(m.group(4)), hp=int(m.group(5)), pst=int(m.group(6)),
                   ps1=int(m.group(7)), ps2=int(m.group(8)), mo=int(m.group(9)),
                   gmo=int(g.group(6)) if g else -1, gaf=int(g.group(7)) if g else -1, gstun=int(g.group(8)) if g else -1,
                   gss1=int(g.group(3)) if g else -1, gx=int(g.group(9)) if g else 0, gz=int(g.group(10)) if g else 0,
                   room=c.group(2) if c else '?', y=int(c.group(4)) if c else 0, band=int(c.group(7)) if c else 0, csub=int(c.group(8)) if c else 0))
# Besuche: F faellt zurueck -> neuer Besuch
vis = []; cur = []
for f in fr:
    if cur and f['F'] <= cur[-1]['F']: vis.append(cur); cur = []
    cur.append(f)
if cur: vis.append(cur)
lv = None
for v in vis:
    if v and v[len(v)//2]['room'] in ('2090', '2091'): lv = v
if lv is None: print('kein Besuch 2090/2091'); sys.exit(1)
room = lv[len(lv)//2]['room']
# Treffer
hits = []
for k in range(1, len(lv)):
    a, b = lv[k-1], lv[k]
    if b['hp'] < a['hp']: hits.append(k)
# Gator-Clip-Laeufe: neuer Lauf, wenn mo wechselt oder af nicht steigt
lauf = [0]*len(lv); lid = 0
for k in range(1, len(lv)):
    a, b = lv[k-1], lv[k]
    if b['gmo'] != a['gmo'] or b['gaf'] < a['gaf']: lid += 1
    lauf[k] = lid
res = []; prevF = None; prevl = None
for k in hits:
    h = lv[k]; F = h['F']
    dF = None if prevF is None else F - prevF
    same = (prevl is not None and lauf[k] == prevl and h['gmo'] == lv[hits[hits.index(k)-1]]['gmo'])
    # Reaktion: ab k, solange pst==2 (Treffer-Klasse); Clipfolge der Spielerclips
    e = k; clips = []; weg = 0.0
    while e + 1 < len(lv) and lv[e+1]['pst'] == 2 and lv[e+1]['hp'] >= 0:
        e += 1
        if not clips or clips[-1] != lv[e]['mo']: clips.append(lv[e]['mo'])
        weg += math.hypot(lv[e]['x'] - lv[e-1]['x'], lv[e]['z'] - lv[e-1]['z'])
        if e + 1 < len(lv) and lv[e+1]['hp'] < lv[e]['hp']: break
    dauer = lv[e]['F'] - F + 1 if e > k else 0
    frei = lv[e+1]['F'] if e + 1 < len(lv) else None
    luft = math.hypot(lv[e]['x'] - lv[k-1]['x'], lv[e]['z'] - lv[k-1]['z'])
    ys = [lv[j]['y'] for j in range(k-1, min(e+2, len(lv)))]
    bands = [lv[j]['band'] for j in range(k-1, min(e+2, len(lv)))]
    sturz = (ys[0] <= -1700 and ys[-1] == 0)
    res.append(dict(F=F, dF=dF, same=same, gmo=h['gmo'], gaf=h['gaf'], gss1=h['gss1'], hp0=lv[k-1]['hp'], hp1=h['hp'], ps1=h['ps1'], dauer=dauer, frei=frei,
                    clips=clips, weg=int(weg), luft=int(luft), y0=ys[0], y1=ys[-1], sturz=sturz, band0=bands[0], band1=bands[-1],
                    pos0=(lv[k-1]['x'], lv[k-1]['z']), pos1=(lv[e]['x'], lv[e]['z'])))
    prevF = F; prevl = lauf[k]
kd = sum(1 for f in lv if f['pst'] == 2 and 12 <= f['mo'] <= 16)
gtr_start = 0
gb = os.path.join(d, 'gator_boss.log')
n_tot = 0
if os.path.exists(gb):
    for z in open(gb, encoding='latin-1'):
        if z.startswith('GTR F') and (' START ' in z): gtr_start += 1
        if z.startswith('GTR F') and (' TOT ' in z): n_tot += 1
dmin = min([r['dF'] for r in res if r['dF'] is not None], default=None)
unter70 = [r for r in res if r['dF'] is not None and r['dF'] < 70]
same = [r for r in res if r['same']]
s2 = [r for r in res if r['ps1'] == 2]; s3 = [r for r in res if r['ps1'] == 3]
d2 = sorted(set(r['dauer'] for r in s2)); d3 = sorted(set(r['dauer'] for r in s3))
print('%s: Raum %s, letzter Besuch F%d..F%d (%d Bilder); Treffer (hp-Flanken) %d [GTR START %d, TOT %d]; Sub 2 %d (Dauern %s), Sub 3 %d (Dauern %s); '
      'kleinster Abstand %s; Abstand < 70: %d; Zweitbiss im selben Gator-Clip-Lauf: %d; RE1.5-Niederwurf-Bilder (pst 2, Clip 12..16) %d; Stuerze %d; '
      'Weg max %d, Luftlinie max %d; Spieler-hp min %d' % (
      os.path.basename(os.path.normpath(d)), room, lv[0]['F'], lv[-1]['F'], len(lv), len(res), gtr_start, n_tot, len(s2), d2, len(s3), d3, dmin, len(unter70), len(same), kd,
      sum(1 for r in res if r['sturz']), max([r['weg'] for r in res], default=0), max([r['luft'] for r in res], default=0), min(f['hp'] for f in lv)))
for r in unter70[:10]: print('  ABSTAND<70: F%d nach %d Bildern (Gator mo %d af %d, selber Lauf %s) hp %d->%d' % (r['F'], r['dF'], r['gmo'], r['gaf'], r['same'], r['hp0'], r['hp1']))
if V:
    for r in res:
        print('  F%-5d dF %-5s G mo%-2d af%-3d ss1 %d  hp %3d->%-4d Sub %d Dauer %-3d frei F%-5s Clips %-14s Weg %-5d Luft %-5d y %d->%d Band %d->%d %s->%s%s' % (
              r['F'], r['dF'], r['gmo'], r['gaf'], r['gss1'], r['hp0'], r['hp1'], r['ps1'], r['dauer'], r['frei'], '/'.join(map(str, r['clips'])), r['weg'], r['luft'],
              r['y0'], r['y1'], r['band0'], r['band1'], r['pos0'], r['pos1'], ' STURZ' if r['sturz'] else ''))
