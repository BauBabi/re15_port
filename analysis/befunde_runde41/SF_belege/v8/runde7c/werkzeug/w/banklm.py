import sys, numpy as np
sys.path.insert(0, 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/bank')
import os
cwd = os.getcwd()
import p5_gemein as G, marker_p5 as MK, voll_p5 as VP
from PIL import Image
R = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/laeufe/'
out = {}
for n in ('KC6', 'R3B'):
    M = VP.ppm(R + 'bLM_%s/fd/f000276.ppm' % n); P = VP.ppm(R + '%s/fd/f000276.ppm' % n)
    k = MK.klassen(M); fig = np.abs(M - P).sum(-1) <= VP.FIGUR_DIFF
    k[fig] = -1
    b = VP.klein(k); b[b == 3] = 0
    r, Gm = G.L.luecke_aus(b, b == 8)
    print(n, r['kerbe'] + r['spalt'], r['stuecke'], 'Figur-Ausfall (|LM-plain|<=40 und LM nicht Hintergrund):', int((fig & (MK.klassen(M) >= 0)).sum()))
    out[n] = (b, Gm, fig)
d = out['KC6'][0] != out['R3B'][0]
ys, xs = np.nonzero(d); print('Klassen-Unterschiede (640x480):', len(ys), list(zip(xs[:20], ys[:20])))
for x, y in list(zip(xs, ys))[:15]:
    print((x, y), 'KC6', out['KC6'][0][y, x], 'R3B', out['R3B'][0][y, x])
