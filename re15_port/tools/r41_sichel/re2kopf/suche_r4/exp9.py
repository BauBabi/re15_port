from r4lib import *
import itertools
for sx, d, ady, adx in itertools.product((0.75, 0.8), (0, 30, 60), (30, 50, 70), (0,)):
    f = dict(sx=sx, auge_dy=ady, auge_dx=adx, auge_e=(39,))
    blob, prot = bauen(f, versatz=(140 - d, 10 + d, 0))
    o, mo, M = messen(blob, prot)
    ok = not o['rot']
    print(('OK ' if ok else '   ') + 'sx %.2f d %d ady %d adx %d' % (sx, d, ady, adx), kurz(o).replace('kb85', '\n        kb85'), flush=True)
