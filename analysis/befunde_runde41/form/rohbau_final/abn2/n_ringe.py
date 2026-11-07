#!/usr/bin/env python3
"""Ringradien und Mitten je Teil aus dem Stand. python abn2/n_ringe.py STAND teil,teil"""
import json
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import HIER
import rf_bau
import rf_plan

g, T = rf_plan.plan()
st = json.load(open(os.path.join(HIER, sys.argv[1])))
L, C = rf_bau.wirksam(T, st['lr'], st['c'])
np.set_printoptions(linewidth=200, suppress=True, precision=0)
print('Flaechen', sum(p.flaechen() for p in T))
for nm in sys.argv[2].split(','):
    i = next(i for i, p in enumerate(T) if p.name == nm)
    p = T[i]
    print(nm, 'bind', p.bind, 'n', p.n, 'T', p.T, 'sp0', p.spitze0, 'sp1', p.spitze1, 'Flaechen', p.flaechen())
    for r in range(p.T):
        print('  t %6.0f  c %s  r %s' % (p.t[r], np.round(C[i][r]), np.round(np.exp(L[i][r] + p.relief[r]))))
