# Vorher (aaea5f8c2) gegen nachher (dieser Stand), gleiche Lage, nur der aktuelle Raum besucht. Leon STAGE2-6, je Raum ein Abzug.
import os, sys, json, collections
from PIL import Image, ImageDraw, ImageFont
S = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, S)
ring = [(1,0),(2,0),(3,0),(0,1),(4,1),(0,2),(4,2),(0,3),(4,3),(1,4),(2,4),(3,4)]
innen = [(1,1),(2,1),(3,1),(1,2),(3,2),(1,3),(2,3),(3,3)]
def finde(p, W, H):
    for y in range(0, H - 5):
        for x in range(0, W - 5):
            c = p[x + 1, y]
            if c[0] < 120 or c[2] > 60: continue
            if all(p[x+dx, y+dy] == c for dx, dy in ring) and all(p[x+dx, y+dy] != c for dx, dy in innen):
                return (x + 3, y + 3)
def rot(c): return 40 <= c[0] <= 56 and c[1] <= 12 and 40 <= c[2] <= 72
def mess(f):
    if not os.path.exists(f): return None
    im = Image.open(f).convert('RGB'); p = im.load(); W, H = im.size
    m = finde(p, W, H)
    nrot = sum(1 for y in range(H) for x in range(W) if rot(p[x, y]))
    um = None
    if m:   # Rand 2 px um den 5x5-Marker: liegt der Punkt auf dem roten Raum?
        pts = [(m[0] - 3 + dx, m[1] - 3 + dy) for dx in range(-2, 7) for dy in range(-2, 7) if not (0 <= dx <= 4 and 0 <= dy <= 4)]
        um = round(sum(1 for x, y in pts if rot(p[x, y])) / len(pts), 2)
    return dict(marker=m, rot=nrot, um=um)
zeilen = [l.split() for l in open(os.path.join(S, 'kampagne_vgl.txt')) if l.strip()]
erg = []
for t in zeilen:
    a = mess(os.path.join(S, 'schuss_basis', t[0], 'karte.bmp')); b = mess(os.path.join(S, 'schuss_nachher', t[0], 'karte.bmp'))
    erg.append(dict(tag=t[0], rid=t[1], page=int(t[4]), vorher=a, nachher=b))
json.dump(erg, open(os.path.join(S, 'vergleich.json'), 'w'), indent=0)
z = collections.Counter()
for e in erg:
    a, b = e['vorher'], e['nachher']
    if not a or not b: z['ohne Abzug'] += 1; continue
    va = 'punkt' if a['marker'] else 'KEIN PUNKT'; vb = 'punkt' if b['marker'] else 'KEIN PUNKT'
    ra = a['rot'] > 0; rb = b['rot'] > 0
    aufa = a['um'] is not None and a['um'] >= 0.3; aufb = b['um'] is not None and b['um'] >= 0.3
    z['vorher Punkt auf rotem Raum'] += aufa; z['nachher Punkt auf rotem Raum'] += aufb
    z['vorher Raum rot'] += ra; z['nachher Raum rot'] += rb
    z['vorher kein Punkt'] += not a['marker']; z['nachher kein Punkt'] += not b['marker']
    z['Punkt verschoben'] += (a['marker'] != b['marker'])
    print('%s vorher %-10s rot=%-5d um=%-5s | nachher %-10s rot=%-5d um=%-5s %s' % (e['rid'], a['marker'], a['rot'], a['um'], b['marker'], b['rot'], b['um'],
          'BESSER' if (aufb and not aufa) else ('SCHLECHTER' if (aufa and not aufb) else '')))
print(dict(z), 'Raeume', len(erg))
