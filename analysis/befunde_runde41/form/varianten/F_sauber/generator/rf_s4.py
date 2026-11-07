#!/usr/bin/env python3
"""Stufe 4: rechter Arm in Pose 1 um die eigene Achse drehen: Bild 1 sieht dann Aussen- und Rueckseite, Bild 2 Innen- und Vorderseite. Dann traegt der gerippte Streifen G nur die Seite, die Bild 1 nicht sieht. python rf_s4.py VON AUS GRAD"""
import json, math, os, sys
import numpy as np
from rf_kern import HIER, E, namen
import rf_plan


def euler_mat(w):
    a = np.array(w, float) * E
    cx, sx, cy, sy, cz, sz = np.cos(a[0]), np.sin(a[0]), np.cos(a[1]), np.sin(a[1]), np.cos(a[2]), np.sin(a[2])
    Rx = np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]])
    Ry = np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
    Rz = np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])
    return Rx @ Ry @ Rz


def mat_euler(R):
    ay = math.asin(np.clip(R[0, 2], -1, 1))
    ax = math.atan2(-R[1, 2], R[2, 2])
    az = math.atan2(-R[0, 1], R[0, 0])
    return [ax / E, ay / E, az / E]


von, aus, grad = sys.argv[1], sys.argv[2], float(sys.argv[3])
g, T = rf_plan.plan()
ix = namen(g)
s = json.load(open(os.path.join(HIER, von)))
w = s['posen']['1']['winkel']
b = ix['oberarm_r']
t = math.radians(grad)
c, sn = math.cos(t), math.sin(t)
Ry = np.array([[c, 0, sn], [0, 1, 0], [-sn, 0, c]])
m = euler_mat(w[b]) @ Ry
w[b] = mat_euler(m)
# Hand gegendrehen, damit sie auf dem Boden bleibt
h = ix['unterarm_r']
m2 = Ry.T @ euler_mat(w[ix['hand_r']])
print('oberarm_r', np.round(np.array(w[b]) * 360 / 4096, 1))
json.dump(s, open(os.path.join(HIER, aus), 'w'))
