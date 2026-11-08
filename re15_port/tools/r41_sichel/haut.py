"""Runde 41 Spur SF N1.8: Hautmass Birkin im echten Fenster gegen Video 4.

    python re15_port/tools/r41_sichel/haut.py <h,h,...> <laufordner>@<p0> [...]
"""
import os, sys, numpy as np
from PIL import Image
# Hautmass Birkin (Rumpf), Video 4 gegen Port-Aufnahme bei gleichem h. Kasten PSX x 150-240, y 95-150 (Rumpf/Bauch,
# unter dem Spider-Man, ueber der Hose); Haut = r > g + 8, r > b + 4 (Hintergrund grau/blau/weiss, Hose blau faellt raus).
V = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
def vid(h):
    v = Image.open(V % (1350 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    p = Image.new('RGB', (320, 240)); p.paste(v, (-1, -3)); return np.asarray(p, dtype=float)
def port(d, p0, h): return np.asarray(Image.open('%s/gd_%04d.png' % (d, p0 + h)).convert('RGB'), dtype=float)
def mass(a, box=(150, 95, 240, 150)):
    x0, y0, x1, y1 = box
    c = a[y0:y1, x0:x1].reshape(-1, 3); r, g, b = c[:, 0], c[:, 1], c[:, 2]
    haut = (r > g + 8) & (r > b + 4)
    c = c[haut]; r, g, b = c[:, 0], c[:, 1], c[:, 2]
    rot = r > 1.5 * g
    L = 0.299 * r + 0.587 * g + 0.114 * b
    return dict(n=len(c), L=L.mean(), rot=rot.mean(), med=np.median(c, 0).round().astype(int).tolist(),
                med_rot=np.median(c[rot], 0).round().astype(int).tolist() if rot.any() else None,
                med_hell=np.median(c[~rot], 0).round().astype(int).tolist() if (~rot).any() else None)
hs = [int(x) for x in sys.argv[1].split(',')]
quellen = [('video', vid)] + [(a.split('@')[0][-8:], (lambda d, p: (lambda h: port(d, p, h)))(a.split('@')[0], int(a.split('@')[1]))) for a in sys.argv[2:]]
for nm, f in quellen:
    ms = [mass(f(h)) for h in hs]
    print('%-10s n %5d  L %5.1f  rot %.3f  med %s  med_rot %s  med_hell %s' % (nm, np.mean([m['n'] for m in ms]),
          np.mean([m['L'] for m in ms]), np.mean([m['rot'] for m in ms]), ms[len(ms)//2]['med'], ms[len(ms)//2]['med_rot'], ms[len(ms)//2]['med_hell']))
