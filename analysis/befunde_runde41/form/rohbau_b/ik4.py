# -*- coding: utf-8 -*-
"""Pose 1: Boden in L1 und L2 ist frei, also steht die linke Hand nicht dort. Linker Arm angehoben, hinter Rumpf und rechtem Bein. Clip Ruhe: alle Viere am Boden (Stand vor dem Anheben)."""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL

f = figur.bauen()
fig = figur.fertig(f)
g = figur.lade_gerippe()
pf = os.path.join(schau.HIER, 'posen.json')
d = json.load(open(pf, encoding='utf-8'))
# Ruhe: Stand vor dem Anheben, Kopf etwas hoeher, Schwanz flacher
ruhe = d.setdefault('ruhe', {}).setdefault('winkel', {})
for k in ('oberarm_l', 'unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4', 'oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'):
    ruhe[k] = [int(c) for c in fig['posen']['bild1']['winkel'][f.ix[k]]]
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
fig['posen']['bild1']['wurzel'] = list(g['posen']['bild1']['wurzel'])
a = Aufgabe(f, fig, 'bild1', 1)
fr = ['oberarm_l', 'unterarm_l', 'hand_l']
GO = ((-70, 130), (-100, 100), (-165, 165))
GU = ((-12, 12), (-20, 20), (-155, 2))
r = a.loese(fr, [('unterarm_l', (0, 0, 0), (316, 285), 1), ('hand_l', (0, 0, 0), (322, 296), 1), ('hand_l', (500, 0, 0), (324, 290), .5)],
            hoehe=[('hand_l', (0, 0, 0), 1250, .5), ('unterarm_l', (0, 0, 0), 1500, .3)], reg=.01, grenzen={fr[0]: GO, fr[1]: GU})
print(r, [a.winkel[f.ix[n]] for n in fr])
speichern(a, fr)
# Finger eingeschlagen, damit nichts aus dem Rumpf ragt
for k, w in (('klaue_l1', (0, 0, 680)), ('klaue_l2', (0, 0, 680)), ('klaue_l3', (0, 0, 680)), ('klaue_l4', (0, -600, 500))):
    a.winkel[f.ix[k]] = list(w)
speichern(a, ['klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'])
