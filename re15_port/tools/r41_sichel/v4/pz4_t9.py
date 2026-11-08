"""Zahlen-Pruefer V4 r4: Schwanzfarbe entlang der eigenen Mittellinie (t2_mass_v4.linie) bei Bogenlaenge 0,05..0,95 (7 Stellen), 5x5-Mittel
in der Maske; Original Vorlage 2 roh in Leser-Maske A, Port Videoraum in Ids 16-22 (Pose 85). Unabhaengig von den festen Achspunkten
des Bauers (riegel_v4.achse_ziele, die zugleich Eichregion sind)."""
import os, sys, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz4_mess import vraum, lab, de, orig, SP
import t2_mass_v4 as T2M
def mask(f):
    d = np.load(f); return np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
def punkte(V, t):
    ys, xs = np.nonzero(t); P, s = T2M.linie(t, (int(xs.max()), int(ys[np.argmax(xs)])))
    res = []
    for f in np.linspace(0.05, 0.95, 7):
        j = int(np.argmin(np.abs(s - f * s[-1]))); x, y = int(round(P[j][0])), int(round(P[j][1]))
        m = np.zeros(t.shape, bool); m[y - 2:y + 3, x - 2:x + 3] = True; m &= t
        res.append(((x, y), lab(V[m].mean(0))))
    return res
o = punkte(orig(2), mask(T2M.MASKEN[0]))
print('Original', [(p, np.round(l, 1).tolist()) for p, l in o])
for nm in ('V4', 'V4R3'):
    d = np.load(SP + 'r_%s_85.npz' % nm); t = (d['ids'] >= 16) & (d['ids'] <= 22)
    p = punkte(vraum(d['img']), t)
    dd = [de(a[1], b[1]) for a, b in zip(p, o)]
    print(nm, 'dE00 je Stelle', np.round(dd, 1).tolist(), 'Mittel %.2f' % np.mean(dd), 'L*/a* Mittel %.1f/%.1f (Original %.1f/%.1f)' % (
        np.mean([x[1][0] for x in p]), np.mean([x[1][1] for x in p]), np.mean([x[1][0] for x in o]), np.mean([x[1][1] for x in o])))
