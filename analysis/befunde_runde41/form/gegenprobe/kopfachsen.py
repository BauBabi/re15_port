import numpy as np
from fk import *
g = lade(__import__('sys').argv[1] if len(__import__('sys').argv) > 1 else None)
ix = {k['name']: k['index'] for k in g['knochen']}
for nr in (1, 2):
    kam = FK.lade_kamera(nr)
    R, T = fk(g, 'bild%d' % nr)
    print('=== Bild', nr)
    Rc = kam['R']
    for n in ('huefte', 'wirbel', 'brust', 'hals', 'kopf'):
        i = ix[n]
        ex, ey, ez = R[i][:, 0], R[i][:, 1], R[i][:, 2]
        up = -ey
        # Roll: Winkel des "oben" des Knochens gegen Bild-Oben, gemessen in der Bildebene (Blickraum)
        v = Rc @ up
        roll = math.degrees(math.atan2(v[0], -v[1]))
        # Welt-Roll: Winkel von up gegen Welt-Oben senkrecht zur Knochenachse
        w = np.array([0, -1, 0.]); w = w - ex * (w @ ex); w /= np.linalg.norm(w)
        wr = math.degrees(math.atan2(np.cross(w, up) @ ex, w @ up))
        print('%-7s x (vorn) Blickraum %s  up Blickraum %s  Roll im Bild %6.1f  Roll zur Welt um Achse %6.1f  links Welt %s' % (n, np.round(Rc @ ex, 2), np.round(v, 2), roll, wr, np.round(ez, 2)))
