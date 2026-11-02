#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 4: Kopf Pose 2 um 2 Grad gedreht (Spalt am Kiefer, sichtbarer Oberarm 80 auf 12 px, Wangenspitze bleibt, abn2/n_spalt2.py), linke Hand Pose 2 flach, Boden BO frei. Nur Gelenke der linken Hand und des linken Arms in Pose 2. python rf_b4.py VON AUS [--p 160]"""
import argparse
import json
import os

from rf_kern import HIER
import rf_plan
from rf_fit import Passung
import rf_auflage
from rf_a1 import dreh


def main():
    a = argparse.ArgumentParser()
    a.add_argument('von')
    a.add_argument('aus')
    a.add_argument('--p', type=int, default=160)
    a.add_argument('--kopf', type=float, default=2.0)
    a.add_argument('--band', type=float, default=8.0)
    a = a.parse_args()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, a.von)))
    if a.kopf:
        dreh(g, s, 2, 'kopf', (0, 1, 0), a.kopf)
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    ix = pa.ix
    pa.bilder = (2,)
    pa.gesperrt = {p.name for p in t}
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
    for b in ('oberarm_l', 'unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'):
        pa.pose_frei[2][ix[b]] = 1.0
    pa.gewicht.update(sicht=3.0, band=a.band, band_p=2.0)
    wl = ix['hand_l']

    def zusatz(n, R, T, w, xy, *_):
        return 3.0 * (1 - R[wl][1, 1])
    pa.zusatz = zusatz
    pa.lauf(a.p, 1, 1.0, form=False, lr_w=1.5, lr_wz=0.0, zeige=40, tag='P')
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    for n in ('1', '2'):
        z['posen'][n]['wurzel'] = s['posen'][n]['wurzel']
        z['posen'][n]['lage'] = s['posen'][n]['lage']
        z['posen'][n]['gier'] = s['posen'][n]['gier']
    z['posen']['1'] = s['posen']['1']
    json.dump(z, open(os.path.join(HIER, a.aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, a.aus.replace('.json', '.png')))
    print('fertig', a.aus)


if __name__ == '__main__':
    main()
