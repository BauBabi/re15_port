"""Runde 41 SF NB2: Lage des gehaltenen Spider-Man (Mitte der Hintergrund-Differenz im Feld oben links, Spielbild PSX
x 40-200, y 22-100; oberhalb y 100 = ueber Birkins Schultern) im Fenster gegen Video 4.  python spiderman.py <ordner>@<p0> ..."""
import os, sys
import numpy as np
import cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import umriss as U   # noqa: E402
BG = np.asarray(Image.open(os.path.join(U.REPO, 're15_port', 'build', 'form_bg', 'ROOM50A12.ppm')).convert('RGB'), float)


def mitte(a, thr):
    d = np.abs(a - BG).max(2)
    f = np.zeros_like(d, bool); f[22:100, 40:200] = d[22:100, 40:200] > thr
    ys, xs = np.where(f)
    return (int(xs.mean()), int(ys.mean()), len(xs)) if len(xs) > 30 else None


def video(h):
    v = Image.open(U.V % (1350 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    vp = Image.new('RGB', (320, 240)); vp.paste(v, (-1, -3))
    return np.asarray(vp, float)


laeufe = [(a.split('@')[0], int(a.split('@')[1])) for a in sys.argv[1:]]
for h in range(16, 72, 6):
    z = ['h%2d Video %s' % (h, mitte(cv2.GaussianBlur(video(h), (0, 0), 1.0), 45))]
    for d, p0 in laeufe:
        a = np.asarray(Image.open(os.path.join(d, 'gd_%04d.png' % (p0 + h))).convert('RGB').resize((320, 240)), float)
        z.append('%s %s' % (os.path.basename(d), mitte(a, 30)))
    print(' | '.join(z))
