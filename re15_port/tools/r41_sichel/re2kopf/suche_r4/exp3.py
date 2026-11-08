from r4lib import *
import itertools
for sx, dome, d, adx in itertools.product((1.0, 0.9), (True,), (0, 60, 100), (0, 30)):
    f = dict(sx=sx, ratio=1.0, dome=dome, auge_dx=adx)
    v = (140 - d, 10 + d, 0)
    blob, prot = bauen(f, versatz=v)
    o, mo, M = messen(blob, prot)
    print('sx %.2f dome %s d %d adx %d' % (sx, dome, d, adx), kurz(o), flush=True)
