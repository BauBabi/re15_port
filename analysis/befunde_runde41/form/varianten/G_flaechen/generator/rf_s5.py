#!/usr/bin/env python3
"""Stufe 5: Ausgang s5. Rumpf, Schwanz, Beine in Pose 1 festhalten, linker Arm hinter dem Bein, rechter Unterarm rollt, Finger in den Gerippe-Grenzen, Bein nicht durch den Arm. python rf_s5.py VON AUS POSE F E"""
import json, os, sys
import numpy as np
from rf_kern import HIER
import rf_plan
from rf_fit import Passung, SCHWANZ, BEIN_L, BEIN_R
import rf_auflage
von, aus = sys.argv[1], sys.argv[2]
np_, n1, n2 = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
g, t = rf_plan.plan()
print('Flaechen', sum(p.flaechen() for p in t))
pa = Passung(g, t, json.load(open(os.path.join(HIER, von))), ohne=tuple(sys.argv[6].split(',')) if len(sys.argv) > 6 else ())
ix = pa.ix
pa.pose_gew[1][:] = 1.0
for b in ['huefte', 'wirbel', 'brust'] + SCHWANZ + BEIN_L + BEIN_R:
    pa.pose_gew[1][ix[b]] = 6.0
pa.pose_gew[2][:] = .6
for b in ['huefte', 'wirbel', 'brust', 'hals', 'kopf']:
    pa.pose_gew[2][ix[b]] = 2.0
pa.anker[1] = [('hand_l', (0, 0, 0), (328, 352), 300, .06), ('unterarm_l', (0, 0, 0), (318, 298), None, .04)]
pa.gewicht.update(sicht=3.0, rolle=.6)
pa.lauf(np_, 2, 1.5, form=False, lr_w=3.0, lr_wz=2.0, zeige=100, tag='A')
pa.lauf(n1, 2, 1.5, tag='F', zeige=100)
pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
pa.lauf(n2, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=100)
json.dump(pa.stand(), open(os.path.join(HIER, aus), 'w'))
rf_auflage.blatt(pa, os.path.join(HIER, aus.replace('.json', '.png')))
print('fertig', aus)
