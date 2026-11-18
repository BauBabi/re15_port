# Stufe 3: Auswertung aller Abzuege. Erwartung = Engine (probe_r42_karte_bestand, RE15_BESTAND_PUNKTE)
# an der TATSAECHLICHEN Lage (letzter [walk]-Eintrag bis Bild 96 im debug.log des Laufs).
import os, re, sys, subprocess, collections, json
from PIL import Image
S = os.path.dirname(os.path.abspath(__file__))
PROBE = r'C:\workspace\git\reAi_v2\.claude\worktrees\r42_karte\re15_port\build\tests\unit\probe_r42_karte_bestand.exe'
ring = [(1,0),(2,0),(3,0),(0,1),(4,1),(0,2),(4,2),(0,3),(4,3),(1,4),(2,4),(3,4)]
innen = [(1,1),(2,1),(3,1),(1,2),(3,2),(1,3),(2,3),(3,3)]
def finde(p, W, H):
    hits = []
    for y in range(0, H - 5):
        for x in range(0, W - 5):
            c = p[x + 1, y]
            if c[0] < 120 or c[2] > 60: continue
            if all(p[x+dx, y+dy] == c for dx, dy in ring) and all(p[x+dx, y+dy] != c for dx, dy in innen):
                hits.append((x + 3, y + 3))
    return hits
def klasse(c):
    r, g, b = c
    if 40 <= r <= 56 and g <= 12 and 40 <= b <= 72: return 'rot'
    if r == 16 and g == 56 and b in (40, 56): return 'besucht'
    if c == (176, 176, 176): return 'wand'
    return None
sweep = {}
for pg in range(13):
    im = Image.open(os.path.join(S, 'sweep', 'blatt%02d.bmp' % pg)).convert('RGB'); sweep[pg] = im.load()
zeilen = [l.split() for l in open(os.path.join(S, 'kampagne.txt')) if l.strip()]
lage = {}
for t in zeilen:
    d = os.path.join(S, 'schuss', t[0]); pos = None
    if os.path.exists(os.path.join(d, 'debug.log')):
        for ln in open(os.path.join(d, 'debug.log'), errors='replace'):
            m = re.search(r'\[walk\] F(\d+) .*pl pos=\((-?\d+),(-?\d+),(-?\d+)\)', ln)
            if m and int(m.group(1)) <= 96: pos = (int(m.group(2)), int(m.group(4)))
    lage[t[0]] = pos or (int(t[2]), int(t[3]))
pf = os.path.join(S, 'punkte.txt')
open(pf, 'w').write(''.join('%s %s %d %d\n' % (t[0], t[1], lage[t[0]][0], lage[t[0]][1]) for t in zeilen))
env = dict(os.environ, RE15_BESTAND_PUNKTE=pf); env['PATH'] = 'C:/msys64/mingw64/bin;' + env['PATH']
out = subprocess.run([PROBE], env=env, capture_output=True, text=True).stdout
erw = {}
for l in out.splitlines():
    p = l.split()
    if p and p[0] == 'PUNKT': erw[p[1]] = (int(p[5]), int(p[6]), int(p[7]), int(p[8]), int(p[9]))
erg = []
for t in zeilen:
    tag, rid = t[0], t[1]
    f = os.path.join(S, 'schuss', tag, 'karte.bmp')
    e = erw.get(tag)
    r = {'tag': tag, 'rid': rid, 'lage': lage[tag], 'erw': e}
    if not os.path.exists(f): r['urteil'] = 'KEIN ABZUG'; erg.append(r); continue
    im = Image.open(f).convert('RGB'); p = im.load(); W, H = im.size
    h = finde(p, W, H)
    # Blatt erkennen: Anteil der Wandpixel des Abzugs, die auf dem Sweep-Blatt ebenfalls Wand sind
    wand = [(x, y) for y in range(0, H, 1) for x in range(0, W) if p[x, y] == (176, 176, 176)]
    best = None
    if wand:
        sc = {pg: sum(1 for x, y in wand if sweep[pg][x, y] == (176, 176, 176)) / len(wand) for pg in sweep}
        best = max(sc, key=sc.get); r['blatt_score'] = round(sc[best], 3)
    r['blatt'] = best
    kl = collections.Counter(klasse(p[x, y]) for y in range(H) for x in range(W))
    r['rot_px'] = kl['rot']; r['besucht_px'] = kl['besucht']
    r['marker'] = h[0] if h else None
    if not h: r['urteil'] = 'KEIN MARKER'
    elif not e or e[0] < 0: r['urteil'] = 'KEINE ERWARTUNG'
    else:
        dd = min(max(abs(a - e[1]), abs(b - e[2])) for a, b in h)
        r['abstand'] = dd
        if e[0] != 13 and best is not None and best != e[0]: r['urteil'] = 'FALSCHES BLATT %s statt %d' % (best, e[0])
        elif dd > 1: r['urteil'] = 'DANEBEN %d px' % dd
        elif kl['rot'] == 0: r['urteil'] = 'KEIN ROT'
        else: r['urteil'] = 'ok'
    erg.append(r)
json.dump(erg, open(os.path.join(S, 'auswertung.json'), 'w'), indent=0)
z = collections.Counter(r['urteil'].split(' ')[0] if not r['urteil'].startswith('KEIN') else r['urteil'] for r in erg)
print('Laeufe', len(erg), dict(z))
for r in erg:
    if r['urteil'] != 'ok': print('  ', r['tag'], r['urteil'], r.get('lage'), r.get('erw'), r.get('marker'), 'blatt', r.get('blatt'), r.get('blatt_score'))
