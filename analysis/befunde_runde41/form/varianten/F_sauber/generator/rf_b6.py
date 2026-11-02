#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 6: E3 Kerbe im rechten Arm. Keil (rf_fit.KEIL_E3) frei, Masse darueber und darunter gedeckt. Nur Ringradien und Ringmitten von Oberarm und
Unterarm rechts, Lage aller Knochen gesperrt. python rf_b6.py VON AUS [--f 220] [--ende 80] [--band 4]"""
import argparse
import json
import os

from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage


def main():
    a = argparse.ArgumentParser()
    a.add_argument('von')
    a.add_argument('aus')
    a.add_argument('--f', type=int, default=220)
    a.add_argument('--ende', type=int, default=80)
    a.add_argument('--band', type=float, default=4.0)
    a.add_argument('--rund', type=float, default=.6)
    a = a.parse_args()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, a.von)))
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    pa.gesperrt = {p.name for p in t} - {'oberarm_r', 'unterarm_r'}
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=a.band, rund=a.rund, laengs=a.rund)
    if a.f:
        pa.lauf(a.f, 2, 1.5, tag='F', zeige=55, lr_wz=0.0, lr_w=0.0, lr=.006, lr_c=2.0)
    pa.gewicht.update(gestalt=0.02, mitte=0.06, naht=0.8)
    if a.ende:
        pa.lauf(a.ende, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.0, tag='e', zeige=40, lr_wz=0.0)
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    z['posen'] = s['posen']
    json.dump(z, open(os.path.join(HIER, a.aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, a.aus.replace('.json', '.png')))
    print('fertig', a.aus)


if __name__ == '__main__':
    main()
