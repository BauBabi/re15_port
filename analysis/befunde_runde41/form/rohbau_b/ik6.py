# -*- coding: utf-8 -*-
"""Linke Hand Bild 2: Richtung nach dem Keil der Maske, aeusserer Finger quer in die Handflaeche geschlagen (Bild 2 und Pose 1, dort ist die Hand gehoben).
Rechter Fuss Bild 2: Zehen hinter den Unterarm."""
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
wurzel = d['bild2'].get('wurzel', fig['posen']['bild2']['wurzel'])
fig['posen']['bild2']['wurzel'] = list(wurzel)
a = Aufgabe(f, fig, 'bild2', 2)


def flach(a, b, richtung, neigung=0.0):
    i = f.ix[b]
    dv = np.array((richtung[0], 0.0, richtung[1]), float)
    dv /= np.linalg.norm(dv)
    n = math.radians(neigung)
    X = dv * math.cos(n) + np.array((0, 1, 0.)) * math.sin(n)
    Yq = np.array((0, 1, 0.)) - X * X[1]
    Yq /= np.linalg.norm(Yq)
    Z = np.cross(X, Yq)
    Rw = np.stack([X, Yq, Z], 1)
    R, T = schau.fk(f.eltern, f.versatz, a.wurzel, a.winkel)
    a.winkel[i] = list(FE.winkel_aus_matrix(R[f.eltern[i]].T @ a.Y.T @ Rw))


dirx, dirz = float(sys.argv[1]) if len(sys.argv) > 1 else 1200., float(sys.argv[2]) if len(sys.argv) > 2 else 150.
flach(a, 'hand_l', (dirx, dirz), 10)
a.winkel[f.ix['klaue_l4']] = [0, 1730, 0]
flach(a, 'fuss_r', (-.25, 1.0))
frei = ['klaue_l1', 'klaue_l2', 'klaue_l3']
speichern(a, ['hand_l', 'klaue_l4', 'fuss_r'])
d = json.load(open(pf, encoding='utf-8'))
d['bild1']['winkel']['klaue_l4'] = [0, 1730, 0]
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
