#!/usr/bin/env python3
"""Stufe 2: linker Arm Pose 1 hinter das rechte Bein, Pose 2 linke Finger, dann Form und Pose zusammen."""
import json, os, sys
from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage
von, aus = sys.argv[1], sys.argv[2]
n1, n2 = int(sys.argv[3]), int(sys.argv[4])
g, t = rf_plan.plan()
pa = Passung(g, t, json.load(open(os.path.join(HIER, von))))
A1 = [('hand_l', (0, 0, 0), (326, 352), 300, .08), ('unterarm_l', (0, 0, 0), (316, 298), None, .05)]
pa.anker[1] = A1
pa.pose_gew[2][:] = .4
pa.lauf(150, 2, 1.5, form=False, lr_w=4.0, lr_wz=4.0, zeige=75, tag='A')
for i in range(len(A1)):
    A1[i] = A1[i][:4] + (A1[i][4] * .3,)
pa.lauf(n1, 2, 1.5, tag='F', zeige=100)
pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
pa.lauf(n2, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=100)
json.dump(pa.stand(), open(os.path.join(HIER, aus), 'w'))
rf_auflage.blatt(pa, os.path.join(HIER, aus.replace('.json', '.png')))
print('fertig', aus)
