# -*- coding: utf-8 -*-
"""Haende flach auf den Boden, Klauen auf die Spitzen der Nutzer-Umrisse. Handlage aus Weltrichtung gesetzt, Finger mit Grenzen nachgestellt."""
import numpy as np, json, os, math, sys
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

f = figur.bauen()
fig = figur.fertig(f)
g = figur.lade_gerippe()
dd = json.load(open('posen.json', encoding='utf-8')) if os.path.exists('posen.json') else {}
for nm in ('bild1', 'bild2'):
    fig['posen'][nm]['wurzel'] = list(dd.get(nm, {}).get('wurzel', g['posen'][nm]['wurzel']))


def tip(name):
    art = (figur.KLAUE_L if '_l' in name else figur.KLAUE_R)[name]
    v, w, lg = figur.FINGER[art]
    sg = -1 if '_l' in name else 1
    w = math.radians(w * sg)
    e = lg / 800.
    return (800 * e * math.cos(w), 120, 800 * e * math.sin(w))


def kn(name):
    t = tip(name)
    return (t[0] * .2, 190, t[2] * .2)


GK = ((-16, 16), (-30, 30), (-30, 60))
GA = ((-16, 16), (-80, 50), (-85, 60))


def boden(h, fl=5):
    z = []
    for n in ('1', '2', '3', '4'):
        k = 'klaue_%s%s' % (h, n)
        z += [(k, tip(k), fl), (k, kn(k), fl)]
    b = 'hand_' + h
    z += [(b, (620, 110, 400), 15), (b, (620, 110, -400), 15), (b, (300, 160, 0), 15)]
    return z


G = lambda h: {'klaue_%s1' % h: GK, 'klaue_%s2' % h: GK, 'klaue_%s3' % h: GK, 'klaue_%s4' % h: GA}


def hand_flach(a, b, richtung, neigung=8.0, roll=0.0):
    """Hand-X zeigt in Welt nach richtung (x, z), neigung Grad abwaerts, Hand-Y etwa Welt unten. Setzt die Winkel der Hand."""
    i = f.ix[b]
    d = np.array((richtung[0], 0.0, richtung[1]), float)
    d /= np.linalg.norm(d)
    n = math.radians(neigung)
    X = d * math.cos(n) + np.array((0, 1, 0.)) * math.sin(n)
    Yq = np.array((0, 1, 0.)) - X * X[1]
    Yq /= np.linalg.norm(Yq)
    Z = np.cross(X, Yq)
    r = math.radians(roll)
    Yr = Yq * math.cos(r) + Z * math.sin(r)
    Zr = np.cross(X, Yr)
    Rw = np.stack([X, Yr, Zr], 1)
    R, T = schau.fk(f.eltern, f.versatz, a.wurzel, a.winkel)
    L = R[f.eltern[i]].T @ a.Y.T @ Rw
    a.winkel[i] = list(FE.winkel_aus_matrix(L))


# Bild 2 rechte Hand: Richtung aus Handgelenk und Fingerspitzen im Raum (Dossier)
a = Aufgabe(f, fig, 'bild2', 2)
hand_flach(a, 'hand_r', (640, -780), 10)
frei = ['klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4']
r = a.loese(frei, [('klaue_r1', tip('klaue_r1'), (276, 443), 1), ('klaue_r2', tip('klaue_r2'), (236, 476), .6), ('klaue_r3', tip('klaue_r3'), (197, 475), 1), ('klaue_r4', tip('klaue_r4'), (150, 352), 1)], boden=boden('r'), reg=.01, grenzen=G('r'))
print(r)
print([a.winkel[f.ix[n]] for n in ['hand_r'] + frei])
speichern(a, ['hand_r'] + frei)
# Bild 2 linke Hand
a = Aufgabe(f, fig, 'bild2', 2)
hand_flach(a, 'hand_l', (1257, -170), 10)
frei = ['klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
GL = G('l')
GL['klaue_l4'] = ((-16, 16), (-130, 30), (-85, 60))
r = a.loese(frei, [('klaue_l3', tip('klaue_l3'), (410, 392), 1), ('klaue_l2', tip('klaue_l2'), (437, 417), 1), ('klaue_l1', tip('klaue_l1'), (467, 416), 1), ('klaue_l4', tip('klaue_l4'), (432, 362), 1)], boden=boden('l'), reg=.01, grenzen=GL)
print(r)
print([a.winkel[f.ix[n]] for n in ['hand_l'] + frei])
speichern(a, ['hand_l'] + frei)
# Bild 1 rechter Arm und Hand
a = Aufgabe(f, fig, 'bild1', 1)
frei = ['oberarm_r', 'unterarm_r']
r = a.loese(frei, [('unterarm_r', (0, 0, 0), (400, 294), 1), ('hand_r', (0, 0, 0), (399, 357), 1), ('oberarm_r', (0, 0, 0), (392, 254), .5)], reg=.01)
print(r)
hand_flach(a, 'hand_r', (1, .10), 8)
frei2 = ['klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4']
r = a.loese(frei2, [('klaue_r1', tip('klaue_r1'), (383, 364), .6), ('klaue_r4', tip('klaue_r4'), (424, 364), .6)], boden=boden('r'), reg=.02, grenzen=G('r'))
print(r)
print([a.winkel[f.ix[n]] for n in frei + ['hand_r'] + frei2])
speichern(a, frei + ['hand_r'] + frei2)
# Bild 1 linke Hand: flach, Finger gerade
a = Aufgabe(f, fig, 'bild1', 1)
hand_flach(a, 'hand_l', (1, .10), 8)
frei = ['klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
r = a.loese(frei, [], boden=boden('l'), reg=.03, grenzen=G('l'))
print([a.winkel[f.ix[n]] for n in ['hand_l'] + frei])
speichern(a, ['hand_l'] + frei)
