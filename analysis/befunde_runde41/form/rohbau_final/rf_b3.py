#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 3: Schaedel kantig (Stirn flach und breit statt Dom), danach Nachpassung nur an Beinen, Oberarm, Schwanzwurzel und Daumen. python rf_b3.py VON AUS [--f 160] [--ende 100] [--ohne-umbau]"""
import argparse
import json
import math
import os

import numpy as np

from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage


def nr(t, nm):
    return next(i for i, p in enumerate(t) if p.name == nm)


def kopf_flach(s, t):
    """Ringe 4..8, Ecken 0 unten, 5 oben: Scheitel tiefer, Schultern des Schaedels hoeher. Querschnitt wie eine Echse, breite flache Stirn mit Kanten."""
    i = nr(t, 'kopf')
    L = np.array(s['lr'][str(i)])
    f = {3: 1.05, 4: 1.03, 5: .90, 6: 1.03, 7: 1.05}
    for r in (4, 5, 6, 7, 8):
        for j, x in f.items():
            L[r, j] += math.log(x)
    s['lr'][str(i)] = L.tolist()


def main():
    a = argparse.ArgumentParser()
    a.add_argument('von')
    a.add_argument('aus')
    a.add_argument('--p', type=int, default=60)
    a.add_argument('--f', type=int, default=160)
    a.add_argument('--ende', type=int, default=100)
    a.add_argument('--ohne-umbau', action='store_true')
    a = a.parse_args()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, a.von)))
    if not a.ohne_umbau:
        kopf_flach(s, t)
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    ix = pa.ix
    frei_f = {'oberarm_r', 'oberschenkel_r', 'unterschenkel_r', 'schwanz1', 'schwanz2', 'schwanz3'}
    pa.gesperrt = {p.name for p in t} - frei_f
    glieder = {1: ('fuss_r', 'fuss_l'), 2: ('klaue_r4', 'fuss_r', 'fuss_l')}
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
        pa.pose_gew[n][:] = 8.0
        for b in glieder[n]:
            pa.pose_frei[n][ix[b]] = 1.0
            pa.pose_gew[n][ix[b]] = 1.0
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=2.5, rund=1.5, laengs=1.5)
    if a.p:
        pa.lauf(a.p, 2, 1.5, form=False, lr_w=2.0, lr_wz=0.0, zeige=30, tag='P')
    if a.f:
        pa.lauf(a.f, 2, 1.5, tag='F', zeige=80, lr_wz=0.0, lr_w=1.0, lr=.006, lr_c=2.0)
    pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
    if a.ende:
        pa.lauf(a.ende, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=50, lr_wz=0.0)
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    for n in ('1', '2'):
        z['posen'][n]['wurzel'] = s['posen'][n]['wurzel']
        z['posen'][n]['lage'] = s['posen'][n]['lage']
        z['posen'][n]['gier'] = s['posen'][n]['gier']
    json.dump(z, open(os.path.join(HIER, a.aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, a.aus.replace('.json', '.png')))
    print('fertig', a.aus)


if __name__ == '__main__':
    main()
