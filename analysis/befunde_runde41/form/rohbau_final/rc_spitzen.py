#!/usr/bin/env python3
"""Rohbau C: Bildlage der Klauenspitzen je Pose gegen die Spitzen des Nutzer-Umrisses. python rc_spitzen.py stand.json"""
import json
import os
import sys

import numpy as np

from rc_kern import HIER, FK, namen
import form_emd as FE
import rc_bau
import rc_plan
import rc_textur
from rc_fit import SPITZEN


def tips(pfad):
    g, t = rc_plan.plan()
    s = json.load(open(pfad))
    form, lage, d, ta = rc_bau.baue_form(g, t, s)
    aus = {}
    for n in (1, 2):
        p = g['posen']['bild%d' % n]
        Rw, Tw = rc_textur.pose_matrizen(form, n - 1, p['gier'], p['lage'])
        kam = FK.lade_kamera(n)
        for i, q in enumerate(ta):
            if q.name.startswith('klaue'):
                ring, k0, k1 = rc_bau.teil_ecken(q, s['lr'][i], s['c'][i])
                w = Rw[q.knochen] @ k1 + Tw[q.knochen]
                xy = FK.projiziere(kam, w)
                aus[(n, q.name)] = (xy, -w[1])
    return aus


if __name__ == '__main__':
    a = tips(os.path.join(HIER, sys.argv[1]))
    for (n, nm), (xy, h) in sorted(a.items()):
        z = SPITZEN[n].get(nm)
        print('Bild %d %-9s (%4.0f,%4.0f) Hoehe %5.0f %s' % (n, nm, xy[0], xy[1], h, '' if z is None else 'Soll %s Abstand %.1f' % (z, np.hypot(xy[0] - z[0], xy[1] - z[1]))))
