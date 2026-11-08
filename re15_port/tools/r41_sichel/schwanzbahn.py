"""Runde 41 Spur SF N1.9: Schwanzbahn Video 4 gegen Port (Hangar h14..h70).

    python re15_port/tools/r41_sichel/schwanzbahn.py <laufordner>      # Lauf von szene_lauf.sh (debug.log mit RE15_HANGAR_LOG)

Video: rote Flecken unterhalb der Huefte (r > 80, r > 1,35 g, r > 1,3 b; Bild 640x480 -> PSX = /2 + (-1,-3); Wandschild
x 151-169 ausgenommen) je Bild, Kasten des groessten Flecks. Port: Schwanzspitze aus der Messzeile
"[hangar-takt] hN Schwanzspitze (x,y,z)" (Stand der Feder-Kette), projiziert durch die RID-Kamera ROOM50A0 Schnitt 12.
"""
import os, re, sys
import numpy as np
from PIL import Image
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..'))
import birkin_buehne as BB      # noqa: E402
V = os.path.join(HIER, '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
r = BB.Raum('50A0'); s = r.schnitt(12); kam = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)


def proj(p):
    v = ((kam.V @ np.array(p, dtype=np.int64)) >> 12) + kam.t
    x, y = BB._gte(np.array([v[0]]), np.array([v[1]]), np.array([v[2]]), kam.h)
    return int(x[0]), int(y[0])


def video(h):
    a = np.asarray(Image.open(V % (1350 + h)).convert('RGB'), dtype=int)
    rr, g, b = a[..., 0], a[..., 1], a[..., 2]
    m = (rr > 80) & (rr > 1.35 * g) & (rr > 1.3 * b)
    m[:250, :] = False; m[:, :100] = False; m[:, 600:] = False; m[270:292, 150:172] = False
    lab, n = ndimage.label(ndimage.binary_dilation(m, iterations=2))
    best = None
    for i in range(1, n + 1):
        ys, xs = np.where((lab == i) & m)
        if len(xs) >= 60 and (best is None or len(xs) > best[0]):
            best = (len(xs), xs.min() // 2 - 1, xs.max() // 2 - 1, ys.min() // 2 - 3, ys.max() // 2 - 3)
    return best


spitze = {}
log = open(os.path.join(sys.argv[1], 'debug.log'), encoding='latin-1').read()
for mm in re.finditer(r'\[hangar-takt\] h(-?\d+) Schwanzspitze \((-?\d+),(-?\d+),(-?\d+)\)', log):
    spitze[int(mm.group(1))] = proj(tuple(int(mm.group(k)) for k in (2, 3, 4)))
print(' h  | Video roter Fleck (PSX x0-x1, y0-y1)  | Port Schwanzspitze (PSX)')
for h in range(14, 72, 2):
    v = video(h)
    vs = 'x%3d-%3d y%3d-%3d' % v[1:] if v else '-'
    print('h%3d | %-36s | %s' % (h, vs, spitze.get(h, '-')))
