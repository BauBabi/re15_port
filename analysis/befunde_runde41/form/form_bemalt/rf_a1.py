#!/usr/bin/env python3
"""Nacharbeit Abnahme 1, Stufe 1. Eingriffe in den Stand, dann kleine Passung mit der Grundlage auf dem Stand.
M1 Hals Pose 1 tiefer, M3 Brust gewoelbt, M4 Aussenfinger neu aus dem Plan, M2/M5 ueber Zonen. python rf_a1.py VON AUS [HALS_GRAD] [POSE F E] [ohne]"""
import json
import math
import os
import sys

import numpy as np

from rf_kern import E, HIER, namen
import rf_plan
from rf_fit import Passung, SCHWANZ, BEIN_L, BEIN_R
import rf_auflage


def euler_mat(w):
    a = np.array(w, float) * E
    cx, sx, cy, sy, cz, sz = np.cos(a[0]), np.sin(a[0]), np.cos(a[1]), np.sin(a[1]), np.cos(a[2]), np.sin(a[2])
    Rx = np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]])
    Ry = np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
    Rz = np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])
    return Rx @ Ry @ Rz


def mat_euler(R):
    ay = math.asin(np.clip(R[0, 2], -1, 1))
    ax = math.atan2(-R[1, 2], R[2, 2])
    az = math.atan2(-R[0, 1], R[0, 0])
    return [ax / E, ay / E, az / E]


def kette_R(g, w):
    R = [None] * len(g['knochen'])
    for i, k in enumerate(g['knochen']):
        M = euler_mat(w[i])
        R[i] = M if k['eltern'] < 0 else R[k['eltern']] @ M
    return R


def dreh(g, s, n, name, achse, grad):
    """Knochen um eine Achse im Aktorraum drehen, Kinder folgen. Vorzeichen Z: + bringt +X nach +Y (Kopf nach unten)."""
    ix = namen(g)
    q = s['posen'][str(n)]
    w = q['winkel']
    i = ix[name]
    e = g['knochen'][i]['eltern']
    R = kette_R(g, w)
    t = math.radians(grad)
    c, sn = math.cos(t), math.sin(t)
    a = np.array(achse, float)
    a /= np.linalg.norm(a)
    K = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    D = np.eye(3) * c + sn * K + (1 - c) * np.outer(a, a)
    Rp = R[e] if e >= 0 else np.eye(3)
    M = Rp.T @ D @ Rp @ euler_mat(w[i])
    w[i] = mat_euler(M)


def dach(L, alt, neu, gw):
    """log-Radien einer Ringfolge: Rueckenecken je Ring so aendern, dass der Radius an Ecke 5 von alt auf neu geht, Nachbarecken anteilig."""
    L = np.array(L, float)
    for r in range(L.shape[0]):
        L[r] += np.array(gw) * math.log(neu[r] / alt[r])
    return L.tolist()


def ruecken(s, t):
    """M3: Rueckenlinie von Brust und Wirbel als Kugelkappen um das Gelenk Wirbel/Brust (Radius rund 950), siehe rf_plan.BRUST_KAPPE. Werte: Hoehe ueber der Achse je Ring."""
    for nm, hoehe, gw in (('brust', [600, 680, 880, 985, 955, 900, 815, 650, 373], [0, 0, .03, .2, .65, 1, .65, .2, .03, 0]),
                          ('wirbel', [629, 690, 683, 735, 840, 924, 930, 870, 640], [0, 0, .1, .5, .9, 1, .9, .5, .1, 0])):
        i = next(i for i, p in enumerate(t) if p.name == nm)
        L = np.array(s['lr'][str(i)])
        ca = np.array(s['c'][str(i)])[:, 0]
        s['lr'][str(i)] = dach(L, np.exp(L[:, 5]), np.array(hoehe, float) + ca, gw)


def pranke(s, t):
    """M4: Handflaeche breiter und als Kuppel (Video: dunkelrote Kuppel, Oberkante y 385-395), Rest der Handmaske liegt jetzt in der Hand, nicht im Finger. Links folgt ueber die Bindung."""
    i = next(i for i, p in enumerate(t) if p.name == 'hand_r')
    L = np.array(s['lr'][str(i)])
    #            Ring:    0     1     2     3     4     5
    quer = np.array([1.03, 1.08, 1.14, 1.16, 1.16, 1.12])
    ruecken_ = np.array([1.0, 1.15, 1.45, 1.70, 1.60, 1.3])
    for r in range(L.shape[0]):
        k = [1.0, 1.1, quer[r], .5 * (quer[r] + ruecken_[r]), ruecken_[r], .5 * (quer[r] + ruecken_[r]), quer[r], 1.1]
        L[r] += np.log(k)
    s['lr'][str(i)] = L.tolist()


def umziehen(s, t):
    """Stand vor der fuenften Kralle: Teile-Nummern verschieben sich um die neuen Teile."""
    alt = [i for i, p in enumerate(t) if not p.name.startswith('kralle')]
    lr, c = {}, {}
    for a, n in enumerate(alt):
        if str(a) in s['lr']:
            lr[str(n)] = s['lr'][str(a)]
            c[str(n)] = s['c'][str(a)]
    s['lr'], s['c'] = lr, c
    s['umzug'] = 1


def kante7(s, t):
    """Bild 2 Kante 3 (370,171): Brust, Ring 7, obere Flanke etwas voller. Einmal anwenden."""
    i = next(i for i, p in enumerate(t) if p.name == 'brust')
    L = np.array(s['lr'][str(i)])
    for j, f in ((3, 1.03), (4, 1.07), (6, 1.07), (7, 1.03)):
        L[7, j] += math.log(f)
    s['lr'][str(i)] = L.tolist()


def main():
    von, aus = sys.argv[1], sys.argv[2]
    hals = float(sys.argv[3]) if len(sys.argv) > 3 else 30.0
    np_, n1, n2 = [int(x) for x in sys.argv[4:7]] if len(sys.argv) > 6 else (150, 200, 150)
    ohne = tuple(sys.argv[7].split(',')) if len(sys.argv) > 7 else ()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, von)))
    if s.get('umzug') is None:
        umziehen(s, t)
    if hals:
        dreh(g, s, 1, 'hals', (0, 0, 1), hals)
    if s.get('m3') is None:
        ruecken(s, t)
        pranke(s, t)
        s['m3'] = 1
    if s.get('k7') is None:
        kante7(s, t)
        s['k7'] = 1
    pa = Passung(g, t, s, ohne=ohne)
    pa.basis_jetzt()
    ix = pa.ix
    # Nacharbeit, kein Neubau: Form nur an Hand, Fingern, Armen und Schwanzwurzel frei, Gelenke nur an Armen, Hals und Schwanzwurzel
    frei = {'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'kralle_r', 'oberarm_r', 'unterarm_r', 'schwanz1', 'schwanz2', 'schwanz4', 'schwanz5', 'oberschenkel_r'}
    pa.gesperrt = {p.name for p in t} - frei
    pa.pose_gew[1][:] = 8.0
    pa.pose_gew[2][:] = 8.0
    for b in ('oberarm_r', 'unterarm_r', 'hand_r', 'oberarm_l', 'unterarm_l', 'hand_l', 'hals', 'kopf', 'kiefer', 'schwanz1', 'schwanz2', 'schwanz3'):
        pa.pose_gew[1][ix[b]] = 1.0
    for b in ('unterarm_r', 'hand_r', 'unterarm_l', 'hand_l'):
        pa.pose_gew[2][ix[b]] = 1.0
    for b in ('klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'):
        pa.pose_gew[1][ix[b]] = .3
        pa.pose_gew[2][ix[b]] = .3
    pa.pose_gew[1][ix['hals']] = .3
    pa.pose_gew[1][ix['kopf']] = .3
    pa.mitte_gew = {'klaue_r%d' % i: 8.0 for i in (1, 2, 3, 4)}
    pa.mitte_gew['kralle_r'] = 8.0
    pa.entwurf.update({i: 40.0 for i in pa.netz.meister if t[i].name.startswith('kl') or t[i].name.startswith('kr')})
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=2.0)
    pa.anker[1] = [('hand_l', (0, 0, 0), (328, 352), 300, .06), ('unterarm_l', (0, 0, 0), (318, 298), None, .04), ('unterarm_r', (0, 0, 0), (403, 296), None, .05), ('klaue_r3', (0, 0, 0), (414, 366), None, .05), ('klaue_r2', (0, 0, 0), (410, 352), None, .05), ('klaue_r1', (0, 0, 0), (398, 332), None, .03),
                   ('klaue_l3', (0, 0, 0), (316, 351), None, .06), ('klaue_l4', (0, 0, 0), (338, 354), None, .03), ('klaue_r4', (0, 0, 0), (392, 342), None, .04)]
    pa.anker[2] = [('hand_r', (0, 0, 0), (272, 380), None, .1)]
    if np_:
        pa.lauf(np_, 2, 1.5, form=False, lr_w=3.0, lr_wz=0.0, zeige=100, tag='A')
    if n1:
        pa.lauf(n1, 2, 1.5, tag='F', zeige=100, lr_wz=0.0)
    pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
    if n2:
        pa.lauf(n2, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=100, lr_wz=0.0)
    z = pa.stand()
    z['m3'] = 1
    z['umzug'] = 1
    z['k7'] = 1
    json.dump(z, open(os.path.join(HIER, aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, aus.replace('.json', '.png')))
    print('fertig', aus)


if __name__ == '__main__':
    main()
