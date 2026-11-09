"""Arbeitsbild Buckel-Ausschnitt x3: Maler | Tiefenpuffer | Klassen. Plus Zahlen je Kopf-Dreieck im Ausschnitt."""
import cv2
from collections import Counter
from s_basis import *  # noqa: F401,F403

d = dict(np.load(AUS + '/r_K5e_f1_2.npz'))
tm = d['tm']
k, tris, Mg, kam = szene(ASSET, 2)
X0, X1, Y0, Y1 = 100, 460, 60, 320
FAR = {10: (245, 130, 48), 21: (128, 128, 0), 22: (60, 180, 75), 23: (0, 130, 200)}
ma = np.where(d['maler_i'] >= 0, tm[np.maximum(d['maler_i'], 0)], -1)
mb = np.where(d['puffer_i'] >= 0, tm[np.maximum(d['puffer_i'], 0)], -1)


def fb(m):
    out = np.full(m.shape + (3,), 255, np.uint8); out[m >= 0] = 170
    for t, f in FAR.items():
        out[m == t] = f
    return out


A, B = fb(ma), fb(mb)
C = np.full(A.shape, 255, np.uint8); C[ma >= 0] = 215
C[(ma == 22) & (mb == 22)] = (60, 180, 75)          # Kopf auch geometrisch vorn
C[(ma == 22) & np.isin(mb, (10, 21))] = (220, 0, 0)  # Kopf gemalt, Brust/Hals liegt vorn -> Ordnungsfehler
C[np.isin(ma, (10, 21)) & (mb == 22)] = (0, 0, 220)  # Brust/Hals gemalt, Kopf liegt vorn
z = [cv2.resize(q[Y0:Y1, X0:X1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST) for q in (A, B, C)]
cv2.imwrite(AUS + '/sicht_zoom.png', cv2.cvtColor(np.hstack(z), cv2.COLOR_RGB2BGR))
# Zahlen
sl = (slice(Y0, Y1), slice(X0, X1))
print('Ausschnitt x %d-%d y %d-%d' % (X0, X1, Y0, Y1))
for nm, msk in (('Kopf gemalt & Kopf vorn', (ma == 22) & (mb == 22)), ('Kopf gemalt & Brust/Hals vorn', (ma == 22) & np.isin(mb, (10, 21))),
                ('Brust/Hals gemalt & Kopf vorn', np.isin(ma, (10, 21)) & (mb == 22))):
    print(nm, int(msk[sl].sum()))
