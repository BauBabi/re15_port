#!/usr/bin/env python3
"""Nacharbeit Abnahme 1, letzte Stufe: nur Fingergelenke der linken Hand in Pose 2, Form und alles andere stehen. Aussenfinger links stand 13 px neben dem Soll (461,392). python rf_a2.py VON AUS"""
import json
import os
import sys

from rf_kern import HIER
import rf_plan
import rf_fit
from rf_fit import Passung
import rf_auflage

von, aus = sys.argv[1], sys.argv[2]
g, t = rf_plan.plan()
s = json.load(open(os.path.join(HIER, von)))
pa = Passung(g, t, s)
pa.basis_jetzt()
pa.bilder = (2,)
ix = pa.ix
pa.pose_gew[2][:] = 100.0
for f in ('klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'):
    pa.pose_gew[2][ix[f]] = .2
rf_fit.SPITZEN[2]['klaue_l3'] = ((461, 392), .15)
pa.gewicht.update(sicht=3.0, band=2.0)
pa.gesperrt = {p.name for p in t}
pa.lauf(120, 1, 1.0, form=False, lr_w=2.0, lr_wz=0.0, zeige=60, tag='L')
z = pa.stand()
z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
# Pose 1 und Wurzeln bleiben wie im Stand
z['posen']['1'] = s['posen']['1']
z['posen']['2']['wurzel'] = s['posen']['2']['wurzel']
z['posen']['2']['lage'] = s['posen']['2']['lage']
z['posen']['2']['gier'] = s['posen']['2']['gier']
json.dump(z, open(os.path.join(HIER, aus), 'w'))
print('fertig', aus)
