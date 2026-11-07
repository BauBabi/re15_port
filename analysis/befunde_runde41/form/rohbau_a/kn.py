#!/usr/bin/env python3
"""Knochen und Posen fuer Rohbau A. Grundlage gerippe.json (Laengen, Posen). Nachstellungen stehen in NACHSTELLUNG, Dossier nennt den Grund.
Zwei Knochen dazu: klaue_l4 (30), klaue_r4 (31), Daumenkralle. Bild 2 verlangt mehr Lappen an der rechten Hand als drei Finger hergeben."""
import json, math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)

E = 2 * math.pi / 4096


def rx(a):
    s, c = math.sin(a), math.cos(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def ry(a):
    s, c = math.sin(a), math.cos(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rz(a):
    s, c = math.sin(a), math.cos(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def gerippe():
    return json.load(open(os.path.join(FORM, 'gerippe.json'), encoding='utf-8'))


NAMEN_NEU = ('klaue_l4', 'klaue_r4')
# Fingerwurzeln weiter auseinander: Gerippe 153, Handbreite 508. Drei Finger mit 185 Radius standen auf einer Breite von 300 und liessen
# die Handkante leer stehen. Winkel bleiben, das Ende der Klaue wandert um rund 170 nach aussen, die Rechnung fit.py zieht es zurueck.
FINGER_Z = 300.0
DAUMEN_V = (240.0, 40.0, 400.0)      # auf dem Handruecken zur Innenseite, rechte Hand +Z, linke -Z (gespiegelt)


def knochen():
    g = gerippe()
    ks = []
    for k in g['knochen']:
        v = list(k['versatz'])
        if k['klasse'] == 'klaue' and v[2] != 0:
            v[2] = math.copysign(FINGER_Z, v[2])
        ks.append(dict(name=k['name'], eltern=k['eltern'], versatz=v, laenge=k['laenge'], klasse=k['klasse'], seite=k['seite']))
    hl = next(k['index'] for k in g['knochen'] if k['name'] == 'hand_l')
    hr = next(k['index'] for k in g['knochen'] if k['name'] == 'hand_r')
    ks.append(dict(name='klaue_l4', eltern=hl, versatz=[DAUMEN_V[0], DAUMEN_V[1], -DAUMEN_V[2]], laenge=560, klasse='klaue', seite='links'))
    ks.append(dict(name='klaue_r4', eltern=hr, versatz=[DAUMEN_V[0], DAUMEN_V[1], DAUMEN_V[2]], laenge=560, klasse='klaue', seite='rechts'))
    return ks


INDEX = {k['name']: i for i, k in enumerate(knochen())}


def posen_roh():
    g = gerippe()
    p = {}
    for n in ('bild1', 'bild2'):
        q = g['posen'][n]
        w = [list(x) for x in q['winkel']] + [[0, 300, 0], [0, -300, 0]]    # Daumen: Startwert, fit.py stellt nach
        p[n] = dict(lage=list(q['lage']), gier=q['gier'], wurzel=list(q['wurzel']), winkel=w)
    # Nachstellung Bild 2: rechter Unterarm um seine Achse gedreht, flache Seite zur Bildebene. Gemessen: Blick im Knochenraum (-0,58 0,07 -0,81), der Streifen ist 20-28 px schmal.
    # Handgelenk bleibt, die Hand dreht gegen, ihre Lage im Raum aendert sich nicht.
    a = math.atan2(0.58, 0.81)
    w = p['bild2']['winkel']
    u, h = INDEX['unterarm_r'], INDEX['hand_r']
    Mu = rx(w[u][0] * E) @ ry(w[u][1] * E) @ rz(w[u][2] * E) @ ry(a)
    Mh = ry(-a) @ rx(w[h][0] * E) @ ry(w[h][1] * E) @ rz(w[h][2] * E)
    import form_emd as FE
    w[u] = [int(x) for x in FE.winkel_aus_matrix(Mu)]
    w[h] = [int(x) for x in FE.winkel_aus_matrix(Mh)]
    return p


def lade_posen(pfad=None):
    """Posen mit Nachstellung aus fit.py, falls vorhanden."""
    p = posen_roh()
    q = pfad or os.path.join(HIER, 'posen.json')
    if os.path.exists(q):
        j = json.load(open(q, encoding='utf-8'))
        for n in j:
            p[n] = j[n]
    return p


def fk(ks, pose):
    """R, T je Knochen im Aktorraum und in Welt. welt = Y(gier)(R v + T) + lage."""
    n = len(ks)
    Rl, Tl = [None] * n, [None] * n
    for i, k in enumerate(ks):
        a = pose['winkel'][i]
        M = rx(a[0] * E) @ ry(a[1] * E) @ rz(a[2] * E)
        e = k['eltern']
        if e < 0:
            Rl[i], Tl[i] = M, np.array(pose['wurzel'], float)
        else:
            Rl[i] = Rl[e] @ M
            Tl[i] = Rl[e] @ np.array(k['versatz'], float) + Tl[e]
    Y = ry(pose.get('gier', 0) * E)
    la = np.array(pose.get('lage', (0, 0, 0)), float)
    return np.array(Rl), np.array(Tl), np.array([Y @ r for r in Rl]), np.array([Y @ t + la for t in Tl])


TEILE = {'tail': ['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6'],
         'left_leg': ['oberschenkel_l', 'unterschenkel_l', 'fuss_l'],
         'right_leg': ['oberschenkel_r', 'unterschenkel_r', 'fuss_r'],
         'right_arm': ['oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'],
         'part_of_Body': ['huefte', 'wirbel', 'brust'],
         'face_and_upper_body': ['huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer', 'oberarm_l', 'oberarm_r', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r'],
         'left_arm_front': ['unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'],
         'right_arm_front': ['unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4']}
BILD_TEILE = {1: ('tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'), 2: ('face_and_upper_body', 'left_arm_front', 'right_arm_front')}
