from r4lib import *
import itertools
for uf, uz in ((1.2, 1.0), (1.2, 0.9), (1.3, 0.85)):
    f = dict(sx=0.85, unten=True, unten_waag=True, unten_f=uf, unten_z=uz)
    for d, ady in ((60, 60),):
        g = dict(f, auge_dy=ady, auge_e=(39,))
        blob, prot = bauen(g, versatz=(140 - d, 10 + d, 0))
        o, mo, M = messen(blob, prot)
        print(f, 'd %d ady %d' % (d, ady), kurz(o), 'bew', prot['form']['bewegt'], flush=True)
