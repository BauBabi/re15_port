"""Runde 41 Spur SF Nachbesserung 1: Bildfolge Video 4 | Port mit gleicher Bildnummer h (Hangar-Szene).

    python re15_port/tools/r41_sichel/vgl.py <laufordner> <p0> <h-liste a,b,c|a-b:s> <aus.png> [spalten] [--port2 <ordner> <p0>]

Video: SF_belege/roh/alle/f%04d.png (alle Bilder von Video 4 480p), Bild n = 1350 + h (Schnitt in den Hangar bei n1350).
Eichung Video -> Spielbild psx = video/2 + (-1,-3) (Spur H, tools/r41_hangar/kalib.py). Port: gd_%04d.png einer
30-Bilder/s-Aufnahme (szene_lauf.sh), p0 = Aufnahmebild von h0 (p0.py). Mit --port2 eine dritte Spalte (z.B. vorher).
"""
import os, sys
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
V = os.path.join(HIER, '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
a = sys.argv[1:]
p2 = None
if '--port2' in a:
    i = a.index('--port2'); p2 = (a[i + 1], int(a[i + 2]), a[i + 3] if len(a) > i + 3 else 'vorher'); a = a[:i]
pdir, p0, liste, out = a[0], int(a[1]), a[2], a[3]
cols = int(a[4]) if len(a) > 4 else 3
hs = []
for t in liste.split(','):
    if '-' in t[1:]:
        r, s = (t.split(':') + ['1'])[:2]; lo, hi = r.split('-'); hs += list(range(int(lo), int(hi) + 1, int(s)))
    else:
        hs.append(int(t))
def port(d, p, h):
    f = os.path.join(d, 'gd_%04d.png' % (p + h))
    return Image.open(f).convert('RGB').resize((320, 240)) if os.path.exists(f) else Image.new('RGB', (320, 240))
tiles = []
n = 3 if p2 else 2
for h in hs:
    v = Image.open(V % (1350 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    vp = Image.new('RGB', (320, 240)); vp.paste(v, (-1, -3))
    t = Image.new('RGB', (324 * n, 252), (20, 20, 20)); t.paste(vp, (0, 12))
    d = ImageDraw.Draw(t)
    d.text((2, 0), 'Video h%d (n%d)' % (h, 1350 + h), fill=(255, 255, 0))
    if p2:
        t.paste(port(p2[0], p2[1], h), (324, 12)); d.text((326, 0), 'Port %s h%d' % (p2[2], h), fill=(255, 128, 128))
    t.paste(port(pdir, p0, h), (324 * (n - 1), 12)); d.text((324 * (n - 1) + 2, 0), 'Port h%d (gd_%04d)' % (h, p0 + h), fill=(0, 255, 255))
    tiles.append(t)
w, hh = tiles[0].size; rows = (len(tiles) + cols - 1) // cols
S = Image.new('RGB', (w * cols, hh * rows), (0, 0, 0))
for k, t in enumerate(tiles): S.paste(t, ((k % cols) * w, (k // cols) * hh))
S.save(out); print(len(tiles), S.size, out)
