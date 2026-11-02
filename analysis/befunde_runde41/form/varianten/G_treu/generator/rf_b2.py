#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 2: Kopf und Hals (M5), Hinterende (M2), Oberarm aus dem Kopf (M5), Bild-1-Reste.
Umbau im Stand (nur einmal, ohne --ohne-umbau), danach Passung nur fuer Beine und Arm Pose 2. python rf_b2.py VON AUS [--p 200] [--f 200] [--ende 100] [--ohne-umbau]"""
import argparse
import json
import math
import os

import numpy as np
import torch

from rf_kern import HIER, innen_tiefe
import rf_plan
import rf_fit
from rf_fit import Passung
import rf_auflage


def nr(t, nm):
    return next(i for i, p in enumerate(t) if p.name == nm)


def ringe_weg(s, i, reihen):
    L = np.array(s['lr'][str(i)])
    c = np.array(s['c'][str(i)])
    s['lr'][str(i)] = np.delete(L, reihen, 0).tolist()
    s['c'][str(i)] = np.delete(c, reihen, 0).tolist()


def kopf_voll(s, t):
    """Stirn laeuft vom Kamm gerade zur Nase, Rueckseite als Dom. Der Kamm bleibt an den Seiten, oben verschwindet die Krempe. Ringe 0..9, Ecken 0 unten, 5 oben."""
    i = nr(t, 'kopf')
    L = np.array(s['lr'][str(i)])
    w = np.array([0, .15, .45, .8, 1, 1, 1, .8, .45, .15])
    for r, f in enumerate((1.10, 1.12, 1.03, 1.0, 1.22, 1.24, 1.32, 1.35, 1.30, 1.25)):
        L[r] += w * math.log(f)
    s['lr'][str(i)] = L.tolist()


def hals_stark(s, t):
    i = nr(t, 'hals')
    L = np.array(s['lr'][str(i)])
    for r, f in enumerate((1.0, 1.05, 1.12)):
        L[r] += math.log(f)
    s['lr'][str(i)] = L.tolist()


def bein(s, t):
    """Knoechel schlanker als das Knie, Ferse in den Fuss."""
    i = nr(t, 'unterschenkel_r')
    L = np.array(s['lr'][str(i)])
    L[4] += np.log([.80, .80, .78, .80, .78, .80, .78, .80])
    L[5] += np.log([.66, .68, .68, .68, .64, .68, .68, .68])
    s['lr'][str(i)] = L.tolist()
    i = nr(t, 'fuss_r')
    L = np.array(s['lr'][str(i)])
    L[0] += math.log(1.3)
    L[1] += math.log(1.18)
    L[2] += math.log(1.04)
    s['lr'][str(i)] = L.tolist()


def main():
    a = argparse.ArgumentParser()
    a.add_argument('von')
    a.add_argument('aus')
    a.add_argument('--p', type=int, default=200)
    a.add_argument('--f', type=int, default=200)
    a.add_argument('--ende', type=int, default=100)
    a.add_argument('--ohne-umbau', action='store_true')
    a = a.parse_args()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, a.von)))
    if not a.ohne_umbau:
        ringe_weg(s, nr(t, 'hals'), [1, 2])
        ringe_weg(s, nr(t, 'schwanz5'), [2])
        kopf_voll(s, t)
        hals_stark(s, t)
        bein(s, t)
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    ix = pa.ix
    frei_f = {'unterschenkel_r', 'fuss_r', 'oberarm_r'}
    pa.gesperrt = {p.name for p in t} - frei_f
    glieder = {1: ('fuss_r', 'fuss_l', 'unterschenkel_r', 'unterschenkel_l'), 2: ('oberarm_r', 'unterarm_r', 'hand_r', 'fuss_r', 'fuss_l', 'unterschenkel_r', 'unterschenkel_l')}
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
        pa.pose_gew[n][:] = 8.0
        for b in glieder[n]:
            pa.pose_frei[n][ix[b]] = 1.0
            pa.pose_gew[n][ix[b]] = 1.0
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=2.0, rund=1.5, laengs=1.5)
    pa.anker[2] = [('hand_r', (0, 0, 0), (275, 378), None, .6), ('unterarm_r', (0, 0, 0), (322, 305), None, .05)]
    i_k = [nr(t, x) for x in ('kopf', 'kiefer')]
    ob = next(sel for nm, sel in pa.innen if nm == 'oberarm_r')

    def zusatz(n, R, T, w, xy, L_, C_):
        if n != 2:
            return 0.0
        q_ = w[ob]
        dm = None
        for i in i_k:
            de = innen_tiefe(q_, t[i], pa.netz.pl[i], L_[i], C_[i], R, T)
            dm = de if dm is None else torch.maximum(dm, de)
        p_ = torch.relu(dm + 20.0) / 60.0
        return 2 * (p_ * p_).mean()
    pa.zusatz = zusatz
    if a.p:
        pa.lauf(a.p, 2, 1.5, form=False, lr_w=2.5, lr_wz=0.0, zeige=60, tag='P')
    if a.f:
        pa.lauf(a.f, 2, 1.5, tag='F', zeige=60, lr_wz=0.0, lr_w=1.5)
    pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
    if a.ende:
        pa.lauf(a.ende, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=60, lr_wz=0.0)
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
