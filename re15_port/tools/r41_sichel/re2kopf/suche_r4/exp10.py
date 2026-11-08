from r4lib import *
import itertools
for sx, uf, uz, d, ady in itertools.product((0.85,), (1.2, 1.3), (0.85, 0.9, 1.0), (60, 75), (50, 60)):
    f = dict(sx=sx, unten=True, unten_waag=True, unten_f=uf, unten_z=uz, unten_seite='rechts', auge_dy=ady, auge_e=(39,))
    blob, prot = bauen(f, versatz=(140 - d, 10 + d, 0))
    o, mo, M = messen(blob, prot)
    ok = not o['rot']
    print(('OK ' if ok else '   ') + 'uf %.1f uz %.2f d %d ady %d' % (uf, uz, d, ady), kurz(o).replace('kb85', '\n        kb85'), flush=True)
