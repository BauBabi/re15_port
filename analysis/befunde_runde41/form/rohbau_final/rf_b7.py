#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 7: E3 Kerbe von Hand nachgestellt. Einzelne Ecken des rechten Oberarms (Ring, Ecke) auf eine Sollkante in Bild 1 (Pose 1) gesetzt:
nur der Radius dieser Ecke, Newton ueber die Projektion. Kein Passen, keine andere Ecke.
python rf_b7.py VON AUS [ring:ecke:x ...]  Vorgabe: 4:2:423.5 5:2:426.5"""
import json
import math
import os
import sys

import torch

from rf_kern import HIER
import rf_plan
from rf_fit import Passung


def lage(pa, i, r, j):
    with torch.no_grad():
        v, L_, C_ = pa.netz.ecken(pa.lr, pa.c)
        R, T = pa.kette_pose(1)
        xy = pa.kam[1](pa.netz.welt(v, R, T))
    p = pa.teile[i]
    return xy[pa.netz.offs[i] + r * p.n + j].cpu().numpy()


def main():
    von, aus = sys.argv[1], sys.argv[2]
    ziele = [tuple(float(x) for x in a.split(':')) for a in sys.argv[3:]] or [(4, 2, 423.5), (5, 2, 426.5)]
    g, t = rf_plan.plan()
    s = json.load(open(os.path.join(HIER, von)))
    pa = Passung(g, t, s)
    i = pa.idx['oberarm_r']
    for r, j, x in ziele:
        r, j = int(r), int(j)
        q0 = lage(pa, i, r, j)
        for _ in range(6):
            q = lage(pa, i, r, j)
            h = 0.02
            pa.lr[i].data[r, j] += h
            dq = (lage(pa, i, r, j)[0] - q[0]) / h
            pa.lr[i].data[r, j] -= h
            if abs(dq) < 1e-6:
                break
            pa.lr[i].data[r, j] += float((x - q[0]) / dq)
        q = lage(pa, i, r, j)
        print('Ring %d Ecke %d: (%.1f,%.1f) -> (%.1f,%.1f), Radius x%.3f' % (r, j, q0[0], q0[1], q[0], q[1], math.exp(float(pa.lr[i][r, j]) - s['lr'][str(i)][r][j])))
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    z['posen'] = s['posen']
    json.dump(z, open(os.path.join(HIER, aus), 'w'))
    print('fertig', aus)


if __name__ == '__main__':
    main()
