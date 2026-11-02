# -*- coding: utf-8 -*-
"""Ruhepose (Clip 2): alle Viere am Boden, gerader Rumpf, Kopf waagerecht vorgestreckt, Schwanz liegt hinten auf. Rechte Glieder aus Pose 1, linke gespiegelt, Fuesse und Haende flach am Boden.
Frei gesetzt, kein Bild zeigt sie."""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

E = schau.E
pf = os.path.join(schau.HIER, 'posen.json')
d = json.load(open(pf, encoding='utf-8'))
d['ruhe'] = {'winkel': {}}
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
f = figur.bauen()
fig = figur.fertig(f)
p1 = fig['posen']['bild1']
w = {n: list(p1['winkel'][f.ix[n]]) for n in f.namen}
r = {}
g = lambda deg: int(round(deg * 4096 / 360.))
# Rumpf gerade, leicht ansteigend zur Schulter
r['huefte'] = [0, 0, 0]
r['wirbel'] = [0, 0, g(2)]
r['brust'] = [0, 0, g(-14)]
r['hals'] = [0, 0, g(14)]
r['kopf'] = [0, 0, g(8)]
r['kiefer'] = [0, 0, g(4)]
# Schwanz: von der Wurzel abwaerts auf den Boden, ohne seitliche Biegung
for i, a_ in zip(range(1, 7), (-30, -15, -5, 10, 15, 15)):
    r['schwanz%d' % i] = [0, 0, g(a_)]
d = json.load(open(pf, encoding='utf-8'))
d['ruhe'] = {'winkel': r}
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)

f = figur.bauen()
fig = figur.fertig(f)
a = Aufgabe(f, fig, 'ruhe', 1)
a.wurzel = [0, -1650, 0]
a.lage = np.zeros(3)
a.Y = np.eye(3)


def flach(b, richtung=(1, 0), neigung=4.0):
    i = f.ix[b]
    dv = np.array((richtung[0], 0.0, richtung[1]), float)
    dv /= np.linalg.norm(dv)
    n = math.radians(neigung)
    X = dv * math.cos(n) + np.array((0, 1, 0.)) * math.sin(n)
    Yq = np.array((0, 1, 0.)) - X * X[1]
    Yq /= np.linalg.norm(Yq)
    Z = np.cross(X, Yq)
    R, T = schau.fk(f.eltern, f.versatz, a.wurzel, a.winkel)
    Rw = np.stack([X, Yq, Z], 1)
    a.winkel[i] = list(FE.winkel_aus_matrix(R[f.eltern[i]].T @ Rw))


class Welt:
    """Ziel im Aktorraum statt im Bild: projiziert auf (x, z) durch eine Hilfskamera ist unnoetig, hier direkte kleinste Quadrate."""


from scipy.optimize import least_squares


def glied(ob, un, ziel_knie, ziel_ende, ende_knochen, knie_knochen):
    ids = [f.ix[ob], f.ix[un]]
    x0 = np.array([a.winkel[ids[0]] + a.winkel[ids[1]]], float).ravel() * E

    def setz(x):
        w = [list(v) for v in a.winkel]
        w[ids[0]] = list(x[:3] / E)
        w[ids[1]] = list(x[3:] / E)
        return w

    def res(x):
        w_ = setz(x)
        R, T = schau.fk(f.eltern, f.versatz, a.wurzel, w_)
        e = T[f.ix[ende_knochen]] - np.array(ziel_ende, float)
        k = T[f.ix[knie_knochen]] - np.array(ziel_knie, float)
        return np.r_[e / 100.0, k / 400.0, (x - x0) * .02]
    s = least_squares(res, x0, diff_step=1e-3, max_nfev=200)
    for j, i in enumerate(ids):
        a.winkel[i] = [int(round(c)) for c in s.x[j * 3:j * 3 + 3] / E]


# Beine: Knie vorn-aussen, Knoechel unter der Huefte, Hoehe 300
glied('oberschenkel_r', 'unterschenkel_r', (250, -760, -900), (-360, -215, -800), 'fuss_r', 'unterschenkel_r')
glied('oberschenkel_l', 'unterschenkel_l', (250, -760, 900), (-360, -215, 800), 'fuss_l', 'unterschenkel_l')
flach('fuss_r', (1, -.12), 2)
flach('fuss_l', (1, .12), 2)
# Arme: Ellbogen hinten-aussen, Handgelenk vorn, Hoehe 360
glied('oberarm_r', 'unterarm_r', (1500, -1250, -1250), (1900, -360, -1100), 'hand_r', 'unterarm_r')
glied('oberarm_l', 'unterarm_l', (1500, -1250, 1250), (1900, -360, 1100), 'hand_l', 'unterarm_l')
flach('hand_r', (1, -.2), 10)
flach('hand_l', (1, .2), 10)
for k in ('klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_l1', 'klaue_l2', 'klaue_l3'):
    a.winkel[f.ix[k]] = [0, 0, g(6)]
a.winkel[f.ix['klaue_r4']] = [0, 0, g(3)]
a.winkel[f.ix['klaue_l4']] = [0, 0, g(3)]
namen = ['oberschenkel_r', 'unterschenkel_r', 'fuss_r', 'oberschenkel_l', 'unterschenkel_l', 'fuss_l', 'oberarm_r', 'unterarm_r', 'hand_r', 'oberarm_l', 'unterarm_l', 'hand_l', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
speichern(a, namen)
d = json.load(open(pf, encoding='utf-8'))
d['ruhe']['wurzel'] = [0, -1650, 0]
json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
