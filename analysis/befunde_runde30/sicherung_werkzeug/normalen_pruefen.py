#!/usr/bin/env python3
"""Zeigen die Normalen des Sicherungs-MD1 nach AUSSEN? Und wie ist es bei einem Original-Prop?

Sicherung: Laengsachse = X, Querschnitt (Y,Z) um (0,0). Aussen heisst: Normale . (0,y,z) > 0
fuer Mantelpunkte. Gegenprobe am ausgelieferten ROOM1150 Prop 3 (Item-Box, ein Quader):
Normale . (Punkt - Schwerpunkt) > 0.

    python analysis/befunde_runde30/sicherung_werkzeug/normalen_pruefen.py
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import inc_bytes, props, rdt_laden  # noqa: E402


def lesen(d):
    (tv, tvc, tn, tnc, tf, tfc, tu,
     qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, 12)
    V = np.array([struct.unpack_from("<3h", d, 12 + qv + i * 8) for i in range(qvc)], float)
    N = np.array([struct.unpack_from("<3h", d, 12 + qn + i * 8) for i in range(qnc)], float)
    F = [struct.unpack_from("<8H", d, 12 + qf + i * 16) for i in range(qfc)]
    return V, N, F


def bericht(name, d, achse_x):
    V, N, F = lesen(d)
    c = V.mean(0)
    aus = ein = null = 0
    for f in F:
        for k in range(4):
            n = N[f[2 * k]]
            p = V[f[2 * k + 1]] - c
            if achse_x:
                p = np.array([0.0, p[1], p[2]])
                n = np.array([0.0, n[1], n[2]])
            s = float(n @ p)
            if s > 1e-6:
                aus += 1
            elif s < -1e-6:
                ein += 1
            else:
                null += 1
    ln = np.linalg.norm(N, axis=1)
    print("%-34s %3d Vierecke, Eck-Normalen: nach AUSSEN %d | nach INNEN %d | quer/0 %d ; "
          "Normalenlaenge %d..%d" % (name, len(F), aus, ein, null, ln.min(), ln.max()))


rdt = rdt_laden("ROOM1150.RDT")
pr = props(rdt)
bericht("ROOM1150 Prop 3 (Item-Box)", pr[3]["md1"], False)
bericht("ROOM1150 Prop 1 (Deckel)", pr[1]["md1"], False)
bericht("Sicherung gen/sicherung_prop.inc", inc_bytes("re15_sicherung_md1"), True)
