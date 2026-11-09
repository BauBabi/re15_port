"""Arbeitsbild: Teile-Falschfarben Maler-Ordnung | Tiefenpuffer | Unterschied (nur zum Hinsehen)."""
import cv2
from s_basis import *  # noqa: F401,F403

d = dict(np.load(AUS + '/r_K5e_f1_2.npz'))
tm = d['tm']
FAR = {10: (245, 130, 48), 21: (128, 128, 0), 22: (60, 180, 75), 23: (0, 130, 200)}


def bild(idx):
    m = np.where(idx >= 0, tm[np.maximum(idx, 0)], -1)
    out = np.full(m.shape + (3,), 255, np.uint8)
    out[m >= 0] = 170
    for t, f in FAR.items():
        out[m == t] = f
    return out, m


a, ma = bild(d['maler_i'])
b, mb = bild(d['puffer_i'])
c = np.full(a.shape, 255, np.uint8)
c[ma >= 0] = 210
x = (d['maler_i'] != d['puffer_i']) & (d['maler_i'] >= 0)
c[x] = (200, 0, 0)
y = x & (ma == 22) & (mb == 10)
c[y] = (60, 180, 75)
y2 = x & (ma == 10) & (mb == 21)
c[y2] = (0, 0, 200)
z = np.hstack([a, b, c])
cv2.imwrite(AUS + '/sicht_maler_puffer.png', cv2.cvtColor(z, cv2.COLOR_RGB2BGR))
print(z.shape)
