import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, f_rast as FR, r3_gemein as R3
from scipy import ndimage
sp = R3.szene_posen()
posen = [('PV', V.PV), ('P2', R3.P2)] + [('F%d' % k, sp[k]) for k in sorted(sp)[::3]]
ref = V.modell(open(sys.argv[1], 'rb').read())
R = {n: FR.zeichne(ref, p, None, licht=False)['bone'][:480] >= 0 for n, p in posen}
for q in sys.argv[2:]:
    mo = V.modell(open(q, 'rb').read())
    tot = loch = 0; wo = []
    for n, p in posen:
        b = FR.zeichne(mo, p, None, licht=False)['bone'][:480] >= 0
        a = R[n]
        inner = (a & ~b) & ndimage.binary_erosion(a, iterations=2)
        neu_l = ~b & ndimage.binary_closing(b, iterations=2) & ~(~a & ndimage.binary_closing(a, iterations=2))
        tot += int((a ^ b).sum()); loch += int(inner.sum())
        if inner.sum(): wo.append((n, int(inner.sum())))
    print(q, 'Ansichten', len(posen), 'Umriss-Pixel anders', tot, 'Loch-Pixel (innen weg)', loch, wo[:12])
