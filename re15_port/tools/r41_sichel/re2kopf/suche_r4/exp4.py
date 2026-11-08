from r4lib import *
import itertools
for sx, d, (adx, ady) in itertools.product((1.0, 0.9), (40, 70), ((0, 30), (0, 50), (15, 40))):
    f = dict(sx=sx, ratio=1.0, auge_dx=adx, auge_dy=ady)
    v = (140 - d, 10 + d, 0)
    blob, prot = bauen(f, versatz=v)
    o, mo, M = messen(blob, prot)
    print('sx %.2f d %d adx %d ady %d' % (sx, d, adx, ady), kurz(o), flush=True)
