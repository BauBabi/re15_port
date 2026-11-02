#!/usr/bin/env python3
"""Kosten eines Laufs zerlegen: je Start die Einzelposten, Gewichte wie im Endplan. python kosten_zerlegen.py lauf/x.pt"""
import os, sys
import numpy as np, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402

pfad = sys.argv[1] if os.path.isabs(sys.argv[1]) else os.path.join(HIER, sys.argv[1])
ger = RG.Gerippe(FT.GER, FT.DT)
bil = [FT.Bild(1, 3, ger), FT.Bild(2, 3, ger)]
par, ids, d = FT.lade(pfad)
det = d['meta']['details']
print('Kosten', np.round(d['kosten'].numpy(), 1).tolist())
for k, v in det.items():
    print('%-34s' % k, np.round(np.array(v), 2).tolist())
with torch.no_grad():
    lo, hi = ger.lo, ger.hi
    for i in range(2):
        a = par.ang[i]
        v = (torch.relu(lo - a) + torch.relu(a - hi)) * ger.frei
        for b in range(par.B):
            w = torch.nonzero(v[b] > 0.02)
            print('Start', b, 'Pose', i + 1, 'Grenzverletzung', [(RG.NAMEN[x[0]], int(x[1]), round(float(torch.rad2deg(v[b, x[0], x[1]])), 1)) for x in w.tolist()][:8])
        print('Gier Grad', np.round(np.degrees(par.gier[i].cpu().numpy()), 0).tolist())
        print('Wurzel', np.round(par.wurzel[i].cpu().numpy() * 1000, 0).astype(int).tolist())
