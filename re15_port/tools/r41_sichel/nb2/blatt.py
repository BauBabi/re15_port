"""Runde 41 SF NB2: Vergleichsblatt Video 4 | Port vorher | Port nachher (echtes Fenster, gdigrab 30 Bilder/s), gleiche
Bildnummer h, Ausschnitt im Spielbild (PSX 320x240) vergroessert.
    python blatt.py <aus.jpg> <h-liste> <x0,y0,x1,y1> <zoom> <vorher_ordner>@<p0> <nachher_ordner>@<p0> [spalten] [--video-n N0]
Video: SF_belege/roh/alle/f%04d.png, Bild n = n0 + h (Hangar n0 = 1350), Eichung psx = video/2 + (-1,-3) (Spur H kalib.py).
"""
import os, sys
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
V = os.path.join(HIER, '..', '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
a = sys.argv[1:]
n0 = 1350
if '--video-n' in a:
    i = a.index('--video-n'); n0 = int(a[i + 1]); a = a[:i] + a[i + 2:]
aus, liste, box, z = a[0], a[1], [int(v) for v in a[2].split(',')], int(a[3])
laeufe = [(s.split('@')[0], int(s.split('@')[1])) for s in a[4:6]]
spalten = int(a[6]) if len(a) > 6 else 2
hs = []
for t in liste.split(','):
    if '-' in t[1:]:
        r, s = (t.split(':') + ['1'])[:2]; lo, hi = r.split('-'); hs += list(range(int(lo), int(hi) + 1, int(s)))
    else:
        hs.append(int(t))
x0, y0, x1, y1 = box
W, H = (x1 - x0) * z, (y1 - y0) * z


def kachel(im, titel, farbe):
    t = Image.new('RGB', (W, H + 14), (16, 16, 16))
    t.paste(im.crop((x0, y0, x1, y1)).resize((W, H), Image.BICUBIC), (0, 14))
    ImageDraw.Draw(t).text((2, 1), titel, fill=farbe)
    return t


zeilen = []
for h in hs:
    v = Image.open(V % (n0 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    vp = Image.new('RGB', (320, 240)); vp.paste(v, (-1, -3))
    ks = [kachel(vp, 'Video 4 n%d h%d' % (n0 + h, h), (255, 255, 0))]
    for (d, p0), name, f in zip(laeufe, ('vorher 3799d7e94', 'nachher'), ((255, 140, 140), (0, 255, 255))):
        g = os.path.join(d, 'gd_%04d.png' % (p0 + h))
        im = Image.open(g).convert('RGB').resize((320, 240)) if os.path.exists(g) else Image.new('RGB', (320, 240))
        ks.append(kachel(im, 'Port %s h%d' % (name, h), f))
    z_ = Image.new('RGB', (3 * W + 8, H + 14), (40, 40, 40))
    for i, k in enumerate(ks): z_.paste(k, (i * (W + 4), 0))
    zeilen.append(z_)
w, hh = zeilen[0].size
rows = (len(zeilen) + spalten - 1) // spalten
S = Image.new('RGB', (w * spalten + 10 * (spalten - 1), hh * rows + 4 * rows), (0, 0, 0))
for k, t in enumerate(zeilen): S.paste(t, ((k % spalten) * (w + 10), (k // spalten) * (hh + 4)))
S.save(aus, quality=92); print(aus, S.size)
