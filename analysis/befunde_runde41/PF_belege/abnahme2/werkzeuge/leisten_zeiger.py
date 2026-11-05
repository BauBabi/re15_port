import numpy as np
from PIL import Image
import bilder
import leisten as L
up = lambda a, f: np.repeat(np.repeat(a, f, 0), f, 1)
def fahrt(r, n=9, schritt=2):
    ev = bilder.ereignisse(r, ('[panel2030]',))
    t0 = next(t for t, s in ev if 'Tank B: umgefuellt' in s)
    out = []
    for i, t, b in bilder.bilder(r):
        if t >= t0 - 0.05 and len(out) < n * schritt:
            out.append((t, bilder.logisch(b)[70:106, 107:143].copy()))
    # nur Bilder mit Aenderung (Spiel-Bild), dann jedes schritt-te
    uniq = [out[0]]
    for t, a in out[1:]:
        if not np.array_equal(a, uniq[-1][1]): uniq.append((t, a))
    return uniq[:n]
reihen = []
for f in ('leon', 'elza'):
    for s in ('vor', 'nach'):
        fr = fahrt('p_%s_%s' % (s, f))
        reihen.append(L.leiste([up(a, 6) for t, a in fr], ['%s NB1 %d' % (s, k) for k in range(len(fr))],
                               '%s %s NB1: Zeiger waehrend A -> B (A 100 -> 50), aufeinanderfolgende Spielbilder, 6-fach' % (f.capitalize(), s)))
L.stapel(reihen).save(L.OUT + 'zeiger_fahrt_vor_nach.png')
print('ok')
