# Leser B: Abblende in Bild 1 (n=1484) gegen den stehenden Hintergrund messen.
# Hintergrund = Mittel der Bilder 1446..1470 (Wand/Decke/Boden konstant, gemessen); Pixel ausserhalb x 260..540 und y 40..445.
import sys, json, numpy as np
from PIL import Image
S = sys.argv[1]
def f(n): return np.asarray(Image.open(f'{S}/allf/f_{n-1200:04d}.png').convert('RGB')).astype(float)
bg = np.mean([f(n) for n in range(1446, 1471)], axis=0)
m = np.zeros((480, 640), bool); m[40:445, :] = True; m[:, 260:540] = False
res = {}
for n in (1434, 1440, 1474, 1478, 1480, 1482, 1483, 1484, 1485, 1486, 1488, 1494):
    a = f(n); row = []
    for c in range(3):
        x = bg[..., c][m]; y = a[..., c][m]
        A = np.vstack([x, np.ones_like(x)]).T; (k, d), *_ = np.linalg.lstsq(A, y, rcond=None)
        r = y - (k * x + d); row.append((round(float(k), 4), round(float(d), 2), round(float(np.sqrt((r ** 2).mean())), 2)))
    # Faktor um den Schwarzwert 17: (y-17) = s (x-17)
    x = bg[m].ravel() - 17; y = a[m].ravel() - 17; s = float((x * y).sum() / (x * x).sum())
    res[n] = dict(je_kanal_k_d_rms=row, s_um_17=round(s, 4))
    print(n, row, 's(um 17)=%.4f' % s)
json.dump(res, open(f'{S}/B_blende.json', 'w'), indent=1)
