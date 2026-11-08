from r4lib import *
import itertools
for sx, d, (adx, ady) in itertools.product((0.9, 0.85), (30, 60, 90), ((0, 0), (0, 40), (10, 40), (0, 60))):
    f = dict(sx=sx, ratio=1.0, auge_dx=adx, auge_dy=ady, auge_e=(39,))
    v = (140 - d, 10 + d, 0)
    blob, prot = bauen(f, versatz=v)
    o, mo, M = messen(blob, prot)
    print('sx %.2f d %d adx %d ady %d' % (sx, d, adx, ady), kurz(o), flush=True)
