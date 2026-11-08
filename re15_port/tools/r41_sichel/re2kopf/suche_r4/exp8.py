from r4lib import *
import itertools
res = []
for sx, (uf, uz), d, ady, adx in itertools.product((0.85, 0.88), ((1.2, 1.0), (1.3, 0.85), (1.3, 0.9), (1.4, 0.85)), (45, 60, 75), (40, 60), (0, 10)):
    f = dict(sx=sx, unten=True, unten_waag=True, unten_f=uf, unten_z=uz, auge_dy=ady, auge_dx=adx, auge_e=(39,))
    blob, prot = bauen(f, versatz=(140 - d, 10 + d, 0))
    o, mo, M = messen(blob, prot)
    ok = not o['rot']
    print(('OK ' if ok else '   ') + 'sx %.2f uf %.1f uz %.2f d %d ady %d adx %d' % (sx, uf, uz, d, ady, adx), kurz(o), flush=True)
