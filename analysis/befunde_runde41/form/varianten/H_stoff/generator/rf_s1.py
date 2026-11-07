#!/usr/bin/env python3
"""Stufe 1: Pose 2 neu setzen. Becken tief, Rumpf aufgerichtet, Anker auf die Hauptgelenke, dann Pose-Passung."""
import json, os, sys
import torch
from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage
g, t = rf_plan.plan()
pa = Passung(g, t)
pa.pose_gew[1][:] = 1.0
pa.pose_gew[2][:] = 0.1
A2 = [('huefte', (-450, 0, 0), (256, 290), None, .08), ('brust', (0, 0, 0), (314, 218), None, .08), ('kopf', (470, 0, 0), (325, 245), None, .05), ('unterarm_r', (0, 0, 0), (322, 318), None, .05), ('hand_r', (0, 0, 0), (262, 395), 330, .08),
      ('unterarm_l', (0, 0, 0), (386, 283), None, .02), ('hand_l', (0, 0, 0), (411, 360), 300, .06), ('unterschenkel_r', (0, 0, 0), (262, 330), None, .03), ('fuss_r', (0, 0, 0), (303, 345), 260, .03),
      ('unterschenkel_l', (0, 0, 0), (340, 300), None, .02), ('fuss_l', (0, 0, 0), (352, 335), 260, .03)]
pa.anker[2] = A2
pa.gewicht.update(spill=.3, cover=.3, sicht=.1, gesicht=.3)
pa.lauf(int(sys.argv[1]) if len(sys.argv) > 1 else 300, 3, 2.5, form=False, lr_w=6.0, lr_wz=12.0, zeige=100, tag='A')
pa.gewicht.update(spill=1, cover=1, sicht=1, gesicht=1)
for i in range(len(A2)):
    A2[i] = A2[i][:4] + (A2[i][4] * .25,)
pa.lauf(200, 2, 1.5, form=False, lr_w=3.0, lr_wz=6.0, zeige=100, tag='B')
json.dump(pa.stand(), open(os.path.join(HIER, 'zw/s1.json'), 'w'))
rf_auflage.blatt(pa, os.path.join(HIER, 'zw/a1.png'))
