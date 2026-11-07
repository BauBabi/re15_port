# -*- coding: utf-8 -*-
"""Bild 2: linker Unterarm um die eigene Achse gedreht, so dass die Schmalseite zur Kamera liegt. Die Hand bleibt, wo sie ist."""
import json, math, os, sys
import numpy as np
import figur, schau
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

E = schau.E
f = figur.bauen()
fig = figur.fertig(f)
pf = os.path.join(schau.HIER, 'posen.json')
d = json.load(open(pf, encoding='utf-8'))
p = fig['posen']['bild2']
kam = FK.lade_kamera(2)
Y = schau.ry(p['gier'] * E)
la = np.array(p['lage'], float)
iu, ih = f.ix['unterarm_l'], f.ix['hand_l']
ring = [(a * math.cos(u), 500, b * math.sin(u)) for a, b, u in [(225, 295, x) for x in np.linspace(0, 2 * math.pi, 24, endpoint=False)]]


def breite(th):
    w = [list(x) for x in p['winkel']]
    L = schau.rm(*(np.array(w[iu], float) * E)) @ schau.rm(0, th, 0)
    w[iu] = list(FE.winkel_aus_matrix(L))
    R, T = schau.fk(f.eltern, f.versatz, p['wurzel'], w)
    q = np.array([FK.projiziere(kam, Y @ (R[iu] @ np.array(c, float) + T[iu]) + la) for c in ring])
    a0 = FK.projiziere(kam, Y @ (R[iu] @ np.array((0, 0, 0.)) + T[iu]) + la)
    a1 = FK.projiziere(kam, Y @ (R[iu] @ np.array((0, 1000., 0)) + T[iu]) + la)
    e = (a1 - a0) / np.linalg.norm(a1 - a0)
    n = np.array((-e[1], e[0]))
    s = q @ n
    return float(s.max() - s.min()), w[iu]


z = [(breite(math.radians(g))[0], g) for g in range(-90, 91, 5)]
print(sorted(z)[:3], sorted(z)[-2:])
g = sorted(z)[0][1]
br, wu = breite(math.radians(g))
# Hand: alte Weltdrehung behalten
w0 = [list(x) for x in p['winkel']]
R0, T0 = schau.fk(f.eltern, f.versatz, p['wurzel'], w0)
w1 = [list(x) for x in w0]
w1[iu] = wu
R1, T1 = schau.fk(f.eltern, f.versatz, p['wurzel'], w1)
Lh = R1[iu].T @ R0[ih]
d['bild2']['winkel']['unterarm_l'] = [int(c) for c in wu]
d['bild2']['winkel']['hand_l'] = [int(c) for c in FE.winkel_aus_matrix(Lh)]
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
print('Drehung', g, 'Breite', br, d['bild2']['winkel']['unterarm_l'], d['bild2']['winkel']['hand_l'])
