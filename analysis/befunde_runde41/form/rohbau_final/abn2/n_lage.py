#!/usr/bin/env python3
"""Nacharbeit Abnahme 2: Lage der Mangelstellen. Achsen, Faechermitten, Gelenke je Pose."""
import json, os, sys, math
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import numpy as np
import torch
from rf_kern import *
import rf_plan, rf_bau
from rf_blick import netz_von, pose_welt
g, T = rf_plan.plan()
ix = namen(g)
st = json.load(open(os.path.join(HIER, 'stand_final.json')))
L, C = rf_bau.wirksam(T, st['lr'], st['c'])
np.set_printoptions(linewidth=200, suppress=True, precision=1)
for nm in ('schwanz1', 'unterschenkel_r', 'hals', 'kopf', 'kiefer', 'hand_r'):
    i = next(i for i, p in enumerate(T) if p.name == nm)
    p = T[i]
    ring, k0, k1 = rf_bau.teil_ecken(p, L[i], C[i])
    print(nm, 'bone', p.knochen, 'o', p.o, 'd', p.d, 'ea', p.ea, 'eb', p.eb, 'bind', p.bind, 'sp0', p.spitze0, 'sp1', p.spitze1)
    print('   t', p.t)
    print('   mitte', ring.mean(1)[[0, -1]], 'k0', k0, 'k1', k1)
    print('   r', np.exp(L[i]).round(0).tolist() if p.T < 8 else np.exp(L[i])[[0,1,-2,-1]].round(0).tolist())
ruhe = json.load(open(os.path.join(HIER, 'zw', 'ruhe.json')))
gr = lambda a: np.round(np.array(a) * 360 / 4096, 1).tolist()
for nm, q in (('P1', st['posen']['1']), ('P2', st['posen']['2']), ('Ruhe', ruhe)):
    print(nm, 'wurzel', np.round(q['wurzel']), 'lage', q.get('lage'), 'gier', q.get('gier'))
    for b in ('huefte','wirbel','brust','hals','kopf','kiefer','schwanz1','oberschenkel_r','unterschenkel_r','fuss_r','oberschenkel_l','unterschenkel_l','fuss_l','oberarm_r','unterarm_r','hand_r','oberarm_l','unterarm_l','hand_l'):
        print('   %-16s %s' % (b, gr(q['winkel'][ix[b]])))
for k in g['knochen']:
    print(k['index'], k['name'], k['eltern'], np.round(k['versatz'], 0), k.get('klasse'), k.get('querachse_a'))
