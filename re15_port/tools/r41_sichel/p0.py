"""Runde 41 Spur SF: Bezugsbild p0 (Aufnahmebild von Videobild h0) einer szene_lauf.sh-Aufnahme.

    python re15_port/tools/r41_sichel/p0.py <laufordner>
"""
import sys, glob, os, numpy as np
from PIL import Image
# p0 = Aufnahmebild des Videobilds h0: erstes Cut-12-Bild (Kantenkorrelation linke/rechte Wand gegen Video n1470),
# dann erste Bewegung im Spider-Man-Feld (x 0..120, y 60..220) = Absprung h2 -> p0 = dieses Bild - 2 (Verfahren Spur H p0find.sh).
d = sys.argv[1]
V = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
def grad(a):
    gx = np.zeros_like(a); gy = np.zeros_like(a)
    gx[:, 1:-1] = a[:, 2:] - a[:, :-2]; gy[1:-1, :] = a[2:, :] - a[:-2, :]
    return np.hypot(gx, gy)
v = Image.open(V % 1470).convert('L').resize((320, 240), Image.BILINEAR)
p = Image.new('L', (320, 240)); p.paste(v, (-1, -3))
GV = grad(np.asarray(p, dtype=np.float32))
mask = np.zeros((240, 320), bool); mask[30:170, 0:60] = True; mask[30:170, 270:320] = True
a = GV[mask]; a = a - a.mean()
fs = sorted(glob.glob(d + '/gd_*.png'))
st = None
for f in fs:
    g = grad(np.asarray(Image.open(f).convert('L'), dtype=np.float32))[mask]; g = g - g.mean()
    c = float((a * g).sum() / np.sqrt((a * a).sum() * (g * g).sum() + 1e-9))
    if c > 0.45: st = int(os.path.basename(f)[3:7]); break
prev = None
for i in range(st, st + 30):
    b = np.asarray(Image.open(d + '/gd_%04d.png' % i).convert('L'), dtype=np.float32)
    if prev is not None and np.abs(b - prev)[60:220, 0:120].mean() > 5:
        print('cut12', st, 'p0', i - 2); break
    prev = b
