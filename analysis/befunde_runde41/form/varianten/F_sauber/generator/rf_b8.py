#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 8: Bild 2, dunkle Masse der rechten Hand (rf_fit.BALLEN_B2). Nur Ringradien und Ringmitten von Pranke, Kralle und Fingern rechts, Lage gesperrt.
Bild 1 bleibt im Verlust (Pranke von hinten, Baender um den Fuss des Arms). python rf_b8.py VON AUS [--f 200] [--ende 80]"""
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
    a.add_argument('--f', type=int, default=200)
    a.add_argument('--ende', type=int, default=80)
    a.add_argument('--bp', type=float, default=3.0)
    a.add_argument('--frei', default='hand_r,kralle_r,klaue_r1')
    a.add_argument('--lr_c', type=float, default=0.0)
    a = a.parse_args()
    g, t = rf_plan.plan()
    s = json.load(open(os.path.join(HIER, a.von)))
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    frei = set(a.frei.split(','))
    pa.gesperrt = {p.name for p in t} - frei
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=4.0, band_p=a.bp, rund=1.0, laengs=1.0)
    if a.f:
        pa.lauf(a.f, 2, 1.5, tag='F', zeige=50, lr_wz=0.0, lr_w=0.0, lr=.006, lr_c=a.lr_c)
    pa.gewicht.update(gestalt=0.02, mitte=0.06, naht=0.8)
    if a.ende:
        pa.lauf(a.ende, 1, 0.9, lr=0.004, lr_c=a.lr_c, lr_w=0.0, tag='e', zeige=40, lr_wz=0.0)
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    z['posen'] = s['posen']
    json.dump(z, open(os.path.join(HIER, a.aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, a.aus.replace('.json', '.png')))
    print('fertig', a.aus)


if __name__ == '__main__':
    main()
