# Runde 41 Spur H: Eichung Video -> PSX-Bild. Aufruf: kalib.py <videobild 640x480> <port-bild cut 12>; sucht Massstab k und
# Versatz (dx,dy) mit der besten Kantenkorrelation an den Waenden (Ergebnis Hangar: k 2.00, dx -1, dy -3).
import sys, numpy as np
from PIL import Image
vid = Image.open(sys.argv[1]).convert('L')            # 640x480 Video
port = Image.open(sys.argv[2]).convert('L').resize((320, 240), Image.BILINEAR)
P = np.asarray(port, dtype=np.float32)
def grad(a):
    gx = np.zeros_like(a); gy = np.zeros_like(a)
    gx[:, 1:-1] = a[:, 2:] - a[:, :-2]; gy[1:-1, :] = a[2:, :] - a[:-2, :]
    return np.hypot(gx, gy)
GP = grad(P)
mask = np.zeros_like(P, dtype=bool)
mask[30:215, 0:110] = True      # linke Wand mit Schild (ohne Figuren-Mitte)
mask[30:215, 260:320] = True    # rechte Wand
best = None
V = np.asarray(vid, dtype=np.float32)
for k in np.arange(1.80, 2.21, 0.02):
    w, h = int(round(640 / k)), int(round(480 / k))
    Vs = np.asarray(vid.resize((w, h), Image.BILINEAR), dtype=np.float32)
    GV = grad(Vs)
    for dx in range(-24, 25, 1):
        for dy in range(-24, 25, 1):
            # Port-Pixel (x,y) <- Video-Pixel (x - dx, y - dy) im skalierten Bild
            ys, xs = np.nonzero(mask)
            vx, vy = xs - dx, ys - dy
            ok = (vx >= 0) & (vx < w) & (vy >= 0) & (vy < h)
            if ok.sum() < 2000: continue
            a = GP[ys[ok], xs[ok]]; b = GV[vy[ok], vx[ok]]
            a = a - a.mean(); b = b - b.mean()
            c = float((a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum() + 1e-9))
            if best is None or c > best[0]: best = (c, k, dx, dy)
print('best corr %.3f  k %.2f  dx %d dy %d' % best)
