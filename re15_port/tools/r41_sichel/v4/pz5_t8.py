"""Zahlen-Pruefer V4 r4: T8 Schwanzfarbe nach Messblatt Abschn. 7 mit den Punkten nach Wortlaut: Beginn = T1 (unterster Id-16-Pixel an
Id 7/8, 4er), Biegung = t2_mass_v4-Fenstermitte (O2), Knick = Punkt minimaler Breite (pz5_mess), Ende = unterster Schwanzpixel."""
import sys, os, json, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz5_mess import vraum, lab, de, SP
T2 = json.load(open(SP + 't2.json'))
M = json.load(open(SP + 'mess_V4_V4R4_V3.json'))
Z = dict(F15=[9.3, 11.4, 1.5], F16=[8.7, 8.4, 2.2], F17=[26.8, -0.1, 2.4], F18=[15.2, 3.2, 1.6])
for nm in ('V4', 'V4R4'):
    d = np.load(SP + 'r_%s_85.npz' % nm); V = vraum(d['img']); ids = d['ids']; LAB = lab(V)
    C = np.hypot(LAB[..., 1], LAB[..., 2]); t = (ids >= 16) & (ids <= 22); yy, xx = np.mgrid[0:480, 0:640]
    b = T2[nm]['beginn_t1']; bi = T2[nm]['mitte']; kn = M[nm]['seite']['T']['T3']['p']; ye = int(np.nonzero(t.any(1))[0].max())
    R = dict(F15=t & (np.hypot(xx - (b[0] - 12), yy - (b[1] - 5)) <= 12) & (C >= 6),
             F16=t & (np.hypot(xx - (bi[0] + 10), yy - bi[1]) <= 10) & (C >= 6),
             F17=t & (C < 6) & (yy <= bi[1]), F18=t & (yy > kn[1]) & (yy <= ye))
    out = []
    for k, m in R.items():
        l = lab(V[m].mean(0)); out.append('%s %s dE00 %.2f (%d px)' % (k, np.round(l, 1), de(l, Z[k]), m.sum()))
    print(nm, 'Beginn', b, 'Biegung', bi, 'Knick', kn, '|', '; '.join(out))
