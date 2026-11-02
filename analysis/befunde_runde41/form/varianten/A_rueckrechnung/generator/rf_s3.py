#!/usr/bin/env python3
"""Stufe 3: neuer Kopf und groessere rechte Hand aus dem Plan, Lappen an den Beinen, weiter Passung. python rf_s3.py VON AUS POSE F E [ohne,liste]"""
import json, os, sys
from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage
von, aus = sys.argv[1], sys.argv[2]
np_, n1, n2 = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
ohne = sys.argv[6].split(',') if len(sys.argv) > 6 else ()
g, t = rf_plan.plan()
print('Flaechen', sum(p.flaechen() for p in t))
pa = Passung(g, t, json.load(open(os.path.join(HIER, von))), ohne=ohne)
if np_:
    pa.pose_gew[1][:] = .5
    pa.pose_gew[2][:] = .5
    pa.lauf(np_, 2, 1.5, form=False, lr_w=3.0, lr_wz=3.0, zeige=100, tag='A')
pa.pose_gew[1][:] = 1
pa.pose_gew[2][:] = 1
if n1:
    pa.lauf(n1, 2, 1.5, tag='F', zeige=100)
pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
if n2:
    pa.lauf(n2, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=100)
json.dump(pa.stand(), open(os.path.join(HIER, aus), 'w'))
rf_auflage.blatt(pa, os.path.join(HIER, aus.replace('.json', '.png')))
print('fertig', aus)
