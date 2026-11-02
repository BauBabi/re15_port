import sys, glob, os, numpy as np
from PIL import Image
# Welche Port-Aufnahmebilder zeigen Cut 12? Kantenkorrelation der linken Wand (ohne Figuren) gegen ein Videobild.
gdpat, vid = sys.argv[1], sys.argv[2]
def grad(a):
    gx = np.zeros_like(a); gy = np.zeros_like(a)
    gx[:, 1:-1] = a[:, 2:] - a[:, :-2]; gy[1:-1, :] = a[2:, :] - a[:-2, :]
    return np.hypot(gx, gy)
v = Image.open(vid).convert('L').resize((320, 240), Image.BILINEAR)
p = Image.new('L', (320, 240)); p.paste(v, (-1, -3))
GV = grad(np.asarray(p, dtype=np.float32))
mask = np.zeros((240, 320), bool); mask[30:170, 0:60] = True; mask[30:170, 270:320] = True
a = GV[mask]; a = a - a.mean()
res = []
for f in sorted(glob.glob(gdpat)):
    g = grad(np.asarray(Image.open(f).convert('L').resize((320, 240)), dtype=np.float32))[mask]
    g = g - g.mean()
    c = float((a * g).sum() / np.sqrt((a * a).sum() * (g * g).sum() + 1e-9))
    res.append((os.path.basename(f), c))
an = [r for r in res if r[1] > 0.45]
print('Cut-12-Bilder:', len(an), 'erstes', an[0] if an else None, 'letztes', an[-1] if an else None)
for r in res:
    if r[1] > 0.45: pass
prev = False
for i, r in enumerate(res):
    on = r[1] > 0.45
    if on != prev: print('Wechsel bei', r[0], round(r[1], 3))
    prev = on
