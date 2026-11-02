# -*- coding: utf-8 -*-
"""Bild 1: dunkler Keil zwischen left_leg und tail (218-255,345-445) traegt Zeichnung Z7, ist also Figur, aber in keinem Nutzer-Teil. Linker Fuss zeigt nach innen und fuellt ihn. python ik8.py [x z]"""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

f = figur.bauen()
fig = figur.fertig(f)
a = Aufgabe(f, fig, 'bild1', 1)
dx, dz = float(sys.argv[1]) if len(sys.argv) > 1 else 1.0, float(sys.argv[2]) if len(sys.argv) > 2 else -.75
i = f.ix['fuss_l']
dv = np.array((dx, 0.0, dz), float)
dv /= np.linalg.norm(dv)
X = dv
Yq = np.array((0, 1, 0.))
Z = np.cross(X, Yq)
Rw = np.stack([X, Yq, Z], 1)
R, T = schau.fk(f.eltern, f.versatz, a.wurzel, a.winkel)
a.winkel[i] = list(FE.winkel_aus_matrix(R[f.eltern[i]].T @ a.Y.T @ Rw))
speichern(a, ['fuss_l'])
print(a.winkel[i])
