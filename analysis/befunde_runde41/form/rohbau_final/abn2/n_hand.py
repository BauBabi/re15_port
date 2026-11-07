#!/usr/bin/env python3
"""Hand: Masse je Ring, Kamera Bild 1 und 2, Tiefe Handgelenk, Blickrichtung im Aktorraum. python abn2/n_hand.py [STAND]"""
import json, os, sys, math
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import numpy as np
import torch
from rf_kern import *
import rf_plan, rf_bau
from rf_fit import Passung
g, T = rf_plan.plan()
sn = sys.argv[1] if len(sys.argv) > 1 else 'stand_final.json'
st = json.load(open(os.path.join(HIER, sn)))
np.set_printoptions(linewidth=200, suppress=True, precision=1)
L, C = rf_bau.wirksam(T, st['lr'], st['c'])
i = next(i for i, p in enumerate(T) if p.name == 'hand_r')
p = T[i]
ring, k0, k1 = rf_bau.teil_ecken(p, L[i], C[i])
print('hand_r je Ring: t, y-Spanne, z-Spanne, Mitte')
for r in range(p.T):
    q = ring[r]
    print('  t %6.0f  y %6.0f..%6.0f (%5.0f)  z %6.0f..%6.0f (%5.0f)' % (p.t[r], q[:, 1].min(), q[:, 1].max(), np.ptp(q[:, 1]), q[:, 2].min(), q[:, 2].max(), np.ptp(q[:, 2])))
pa = Passung(g, T, st)
ix = pa.ix
for n in (1, 2):
    R, Tt = pa.kette_pose(n)
    k = pa.kam[n]
    au = torch.tensor(k.k['auge'], dtype=torch.float32, device=pa.dev)
    print('Bild', n, 'Auge', k.k['auge'])
    for b in ('oberarm_r', 'unterarm_r', 'hand_r', 'hand_l'):
        t_ = Tt[ix[b]]
        xy = k(t_[None])[0].detach().cpu().numpy()
        print('  %-11s Bild (%.0f,%.0f) Hoehe %.0f Tiefe %.0f Welt %s' % (b, xy[0], xy[1], -float(t_[1]), float(k.tiefe(t_[None])[0]), t_.detach().cpu().numpy()))
    Rh = R[ix['hand_r']].detach().cpu().numpy()
    print('  hand_r Achsen Welt X', Rh[:, 0], 'Y', Rh[:, 1], 'Z', Rh[:, 2])
    print('  Handnormale (Y) gegen unten %.1f Grad' % math.degrees(math.acos(abs(Rh[1, 1]))))
    bl = (Tt[ix['hand_r']] - au).detach().cpu().numpy()
    bl /= np.linalg.norm(bl)
    print('  Blickrichtung Welt', bl, 'el %.1f' % math.degrees(math.asin(bl[1])))
