"""E3: Ringe von oberarm_r und unterarm_r in Bild 1 (Pose 1): je Ring Stelle t, Mitte im Bild, rechteste Ecke im Bild (Ecke j)."""
import json, os, sys
import numpy as np, torch
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER
import rf_plan
from rf_fit import Passung
g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, sys.argv[1])))
pa = Passung(g, t, s)
with torch.no_grad():
    v, L_, C_ = pa.netz.ecken(pa.lr, pa.c)
    R, T = pa.kette_pose(1)
    w = pa.netz.welt(v, R, T)
    xy = pa.kam[1](w).cpu().numpy()
for nm in ('oberarm_r', 'unterarm_r'):
    i = pa.idx[nm]
    p = t[i]
    o = pa.netz.offs[i]
    print(nm, 'n', p.n, 'T', p.T)
    for r in range(p.T):
        q = xy[o + r * p.n: o + (r + 1) * p.n]
        j = int(q[:, 0].argmax())
        print('  Ring %d t %5.0f  Mitte (%.0f,%.0f)  rechts Ecke %d (%.1f,%.1f)  Radien %s' % (r, p.t[r], q[:, 0].mean(), q[:, 1].mean(), j, q[j, 0], q[j, 1], np.round(np.exp(L_[i][r].detach().cpu().numpy())).astype(int).tolist()))
        print('     Ecken x', ' '.join('%5.1f,%5.1f' % (a, b) for a, b in q))
    print('  Spitzen', xy[o + p.T * p.n], xy[o + p.T * p.n + 1])
