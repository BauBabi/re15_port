# Vorpruefung OHNE Spiel: an jeder ECHTEN Ankunftslage (EIN-Zeilen = Door_aot_set-Spawn) die Engine-Rechnung
# (probe PUNKT) gegen die GEMALTE Flaeche der eigenen Kachel (Original-Kunst) halten.
import os, sys, subprocess, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kunst
S = os.path.dirname(os.path.abspath(__file__))
PROBE = os.path.join(kunst.WT, r're15_port\build\tests\unit\probe_r42_karte_bestand.exe')
best = sys.argv[1] if len(sys.argv) > 1 else os.path.join(S, 'bestand.txt')
kopf = sys.argv[2] if len(sys.argv) > 2 else None
probe = sys.argv[3] if len(sys.argv) > 3 else PROBE
raeume = set(); ein = collections.defaultdict(list)
for l in open(best):
    t = l.split()
    if not t: continue
    if t[0] == 'ROOM': raeume.add(t[1])
    if t[0] == 'EIN':
        d, v, k, x, y, z, yaw = t[1], t[2], int(t[3]), int(t[4]), int(t[5]), int(t[6]), int(t[7])
        ein[d].append((x, z, yaw, v, k, y))
pts = []
for r in sorted(raeume):
    uniq = []
    for p in ein.get(r, []):
        if all(abs(p[0] - u[0]) + abs(p[1] - u[1]) > 600 for u in uniq): uniq.append(p)
    for i, p in enumerate(uniq): pts.append(('%s_t%d' % (r, i), r, p))
pf = os.path.join(S, 'vor_punkte.txt')
open(pf, 'w').write(''.join('%s %s %d %d\n' % (t, r, p[0], p[1]) for t, r, p in pts))
env = dict(os.environ, RE15_BESTAND_PUNKTE=pf); env['PATH'] = 'C:/msys64/mingw64/bin;' + env['PATH']
out = subprocess.run([probe], env=env, capture_output=True, text=True).stdout
erw = {}
for l in out.splitlines():
    p = l.split()
    if p and p[0] == 'PUNKT': erw[p[1]] = (int(p[5]), int(p[6]), int(p[7]), int(p[8]))
Z = kunst.zonen(kopf)
res = []
for t, r, p in pts:
    e = erw.get(t)
    rid = int(r, 16)
    if not e or e[0] < 0: res.append((t, r, p, e, 'KEINE ZONE', None)); continue
    pg, mx, my, zid = e
    if pg == 13 or mx < 0: res.append((t, r, p, e, 'KEIN BLATT/PUNKT', None)); continue
    eig = [(z['page'], z['rect']) for z in Z if z['room'] == rid and z['page'] == pg]
    if any(rc == 255 for _, rc in eig) and all(rc == 255 for _, rc in eig):
        res.append((t, r, p, e, 'SCHEMA', None)); continue
    menge = set()
    for pp, rc in eig:
        if rc != 255: menge |= kunst.flaeche(pp, rc)
    d = kunst.abstand(menge, mx, my)
    urt = 'auf' if d == 0 else ('rand%d' % d if d is not None and d <= 2 else 'NEBEN')
    # wessen Kunst liegt dort?
    fremd = [(z['room'], z['rect']) for z in Z if z['page'] == pg and z['rect'] != 255 and (mx, my) in kunst.flaeche(pg, z['rect'])
             and z['room'] != rid]
    res.append((t, r, p, e, urt, sorted(set('%04X/r%d' % f for f in fremd))))
open(os.path.join(S, 'vorpruef_detail.txt'), 'w').write(''.join('%s %s %d %d %s %d %d %d %s %s' % (t, r, p[0], p[1], p[3], e[0] if e else -1, e[1] if e else 0, e[2] if e else 0, u.replace(' ', '_'), ','.join(f or [])) + chr(10) for t, r, p, e, u, f in res))
c = collections.Counter(x[4] if not x[4].startswith('rand') else 'rand' for x in res)
print('Lagen', len(res), dict(c))
for t, r, p, e, u, f in res:
    if u not in ('auf',) and not u.startswith('rand'):
        print(' ', t, 'lage', p[:2], 'yaw', p[2], 'aus', p[3], 'erw', e, u, f or '')
