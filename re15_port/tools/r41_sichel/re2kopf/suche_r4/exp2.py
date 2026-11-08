from r4lib import *
t = time.time()
for sx in (1.0, 0.85):
    for dv in ((0, 0, 0), (-20, 0, 0), (-40, 0, 0), (0, 20, 0), (0, 0, 20), (0, 0, -20)):
        v = (140 + dv[0], 10 + dv[1], 0 + dv[2])
        blob, prot = bauen(dict(sx=sx, ratio=1.0), versatz=v)
        o, mo, M = messen(blob, prot)
        print('sx %.2f versatz %s' % (sx, v), kurz(o), flush=True)
