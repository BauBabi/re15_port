#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 5: Kopfende des Unterschenkels als Kuppe. Ring 0 weiter, Faecher bleibt in rf_plan. Oberes Ende des Oberschenkels bleibt schmal: Eine Keule dort kostet in Bild 1 part_of_Body 0,80 auf 0,76 (Lauf kn3). Keine Passung: liegt in Pose 1 im Oberschenkel, gebeugt am Knie. python rf_b5.py VON AUS"""
import json
import math
import os
import sys

import numpy as np

from rf_kern import HIER
import rf_plan

g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, sys.argv[1])))
i = next(i for i, p in enumerate(t) if p.name == 'unterschenkel_r')
L = np.array(s['lr'][str(i)])
L[0] += math.log(1.8)
s['lr'][str(i)] = L.tolist()
json.dump(s, open(os.path.join(HIER, sys.argv[2]), 'w'))
print('fertig', sys.argv[2])
