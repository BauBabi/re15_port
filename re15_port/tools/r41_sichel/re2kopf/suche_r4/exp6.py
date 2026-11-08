from r4lib import *
import itertools
for f in (dict(sx=0.85, unten=False), dict(sx=0.85, unten=True), dict(sx=0.85, unten=True, unten_z=0.85)):
    for d, ady in ((60, 60), (90, 40)):
        g = dict(f, auge_dy=ady, auge_e=(39,))
        blob, prot = bauen(g, versatz=(140 - d, 10 + d, 0))
        o, mo, M = messen(blob, prot)
        print(f, 'd %d ady %d' % (d, ady), kurz(o), flush=True)
