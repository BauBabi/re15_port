from r4lib import *
import itertools
for sx, d, ady, adx in itertools.product((0.85,), (60, 75, 90), (50, 60, 70), (0, 20, 40)):
    f = dict(sx=sx, auge_dy=ady, auge_dx=adx, auge_e=(39,))
    blob, prot = bauen(f, versatz=(140 - d, 10 + d, 0))
    o, mo, M = messen(blob, prot)
    ok = not o['rot']
    m = min(o['S2'][0] - 0.03, 0.82 - o['S2'][1], o['S2'][1] - 0.76, o['K4'][0] - 0.035, 0.065 - o['K4'][0], o['K4'][1] - 0.07, 0.12 - o['K4'][1], (o['KS2'][1] - o['KS2'][0]) / 100)
    print(('OK ' if ok else '   ') + 'd %d ady %d adx %d marge %.4f' % (d, ady, adx, m), 'ueber %d S2 %s K4 %s K10 %s KS2 %.1f/%.1f rot %s' % (o['ueber'], np.round(o['S2'], 3).tolist(), np.round(o['K4'], 3).tolist(), np.round(o['K10'], 3).tolist(), o['KS2'][0], o['KS2'][1], [r[5:20] for r in o['rot']]), flush=True)
