# -*- coding: utf-8 -*-
"""Pose 2 nach dem Senken der Huefte: Schwanz aufs Boden-Niveau, Beine in die Hocke, Fuesse flach."""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

f = figur.bauen()
fig = figur.fertig(f)
pf = os.path.join(schau.HIER, 'posen.json')
d = json.load(open(pf, encoding='utf-8'))
fig['posen']['bild2']['wurzel'] = list(d['bild2']['wurzel'])
a = Aufgabe(f, fig, 'bild2', 2)
# Schwanz: Abstand zum Boden wie Dicke
sw = ['schwanz%d' % i for i in range(1, 7)]
L = dict(zip(sw, [592, 598, 625, 662, 678, 663]))
bo = [(s, (-L[s], 0, 0), r) for s, r in zip(sw, (420, 400, 330, 260, 240, 170))] + [('schwanz6', (-663 * 1.5, 0, 0), 30)]
G = {s: ((-12, 12), (-40, 40), (-40, 40)) for s in sw}
hh = [(s, (-L[s], 0, 0), h, .25) for s, h in zip(sw, (1250, 1000, 780, 560, 340, 200))]
r = a.loese(sw, [], boden=bo, hoehe=hh, reg=.02, grenzen=G)
print([a.winkel[f.ix[n]] for n in sw])
speichern(a, sw)

KNIE = {'r': (222, 322), 'l': (316, 322)}
GELENK = {'r': (262, 352), 'l': (352, 338)}
GO = ((-40, 50), (-60, 60), (-170, 80))
GU = ((-12, 12), (-20, 20), (-2, 165))


def fuss_flach(a, b, richtung):
    i = f.ix[b]
    dv = np.array((richtung[0], 0.0, richtung[1]), float)
    dv /= np.linalg.norm(dv)
    X = dv
    Yq = np.array((0, 1, 0.))
    Z = np.cross(X, Yq)
    Rw = np.stack([X, Yq, Z], 1)
    R, T = schau.fk(f.eltern, f.versatz, a.wurzel, a.winkel)
    Ll = R[f.eltern[i]].T @ a.Y.T @ Rw
    a.winkel[i] = list(FE.winkel_aus_matrix(Ll))


for s in ('r', 'l'):
    fr = ['oberschenkel_' + s, 'unterschenkel_' + s]
    r = a.loese(fr, [('unterschenkel_' + s, (0, 0, 0), KNIE[s], 1), ('fuss_' + s, (0, 0, 0), GELENK[s], 1)],
                hoehe=[('fuss_' + s, (0, 0, 0), 300, 1.0)], reg=.01, grenzen={fr[0]: GO, fr[1]: GU})
    print(r, [a.winkel[f.ix[n]] for n in fr])
    fuss_flach(a, 'fuss_' + s, (.96, .27) if s == 'r' else (.3, -.95))
    speichern(a, fr + ['fuss_' + s])
