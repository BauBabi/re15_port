from r4lib import *
import itertools
t = time.time()
for sx, ratio in ((1.0, 1.0), (0.9, 1.0), (0.8, 1.0), (0.8, 1.05), (0.7, 1.0)):
    f = dict(sx=sx, ratio=ratio)
    blob, prot = bauen(f)
    o, mo, M = messen(blob, prot)
    print('sx %.2f ratio %.2f' % (sx, ratio), kurz(o), 'bewegt', prot['form']['bewegt'], '%.0fs' % (time.time()-t), flush=True)
