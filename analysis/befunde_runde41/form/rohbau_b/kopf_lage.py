# -*- coding: utf-8 -*-
"""Kopfrand gegen Zug Q: Rand-Mitte (326,245), Eckbuckel auf (290,172) und (372,201), Schnauze zeigt gut 15 Grad neben die Kamera (schwacher Fleck 305-335,265-290 ist das Maul)."""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK

pf = 'posen.json'
d = json.load(open(pf, encoding='utf-8'))
g = figur.lade_gerippe()
nam = [k['name'] for k in g['knochen']]
for k in ('hals', 'kopf', 'kiefer'):
    d['bild2']['winkel'][k] = list(g['posen']['bild2']['winkel'][nam.index(k)])
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
f = figur.bauen()
fig = figur.fertig(f)
t = [x for x in f.teile if x.name == 'kopf'][0]
r = t.ringe[3]
kam = FK.lade_kamera(2)
p = fig['posen']['bild2']


def lage(a=None):
    w = a.winkel if a else p['winkel']
    R, T = schau.fk(f.eltern, f.versatz, p['wurzel'], w)
    Y = schau.ry(p['gier'] * schau.E)
    la = np.array(p['lage'], float)
    b = f.ix['kopf']
    return FK.projiziere(kam, (r @ R[b].T + T[b]) @ Y.T + la), FK.projiziere(kam, Y @ (R[b] @ t.pol_e + T[b]) + la)


q, sp = lage()
print('vorher Mitte', q.mean(0).round(1), 'Ecken', q[1].round(), q[9].round(), 'Spitze', sp.round())
a = Aufgabe(f, fig, 'bild2', 2)
c = tuple(r.mean(0))
e1, e9 = tuple(r[1]), tuple(r[9])
links, rechts = (e1, e9) if q[1][0] < q[9][0] else (e9, e1)
res = a.loese(['hals', 'kopf'], [('kopf', c, (326, 245), 2), ('kopf', links, (290, 176), .5), ('kopf', rechts, (372, 201), .5), ('kopf', tuple(t.pol_e), (316, 262), 1.5)], reg=.03, achsen={'hals': (0, 1, 1), 'kopf': (1, 1, 1)},
              grenzen={'kopf': ((-30, 30), (-60, 60), (-75, 75)), 'hals': ((-30, 30), (-65, 65), (-75, 75))})
print(res, [a.winkel[f.ix[n]] for n in ('hals', 'kopf')])
speichern(a, ['hals', 'kopf'])
q, sp = lage(a)
print('nachher Mitte', q.mean(0).round(1), 'Breite', q[:, 0].max() - q[:, 0].min(), 'Hoehe', q[:, 1].max() - q[:, 1].min(), 'Ecken', q[1].round(), q[9].round(), 'Spitze', sp.round())
