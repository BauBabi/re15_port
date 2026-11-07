#!/usr/bin/env python3
"""Variante J (E18): Anpassung der dickeren Beine, wie fuer die Abgabe gelaufen. python vm_beine_j.py [AUS.json]  (Vorgabe zw/beine_J_neu.json,
ueberschreibt die Abgabe zw/beine_J.json nicht). Start: Masse aus I (zw/beine_I.json), Knie x 1,12, Knoechel x 1,15; Ringfaktoren J
(vm_beine.DICKER: Knie 1,08, Wade 1,22, Knoechel 1,08); Breitenterm BREITE_GEWICHT 3 (Strafe nur fuer schmaler als die Maske);
Stufe Bein, dann Fuss (Breite x 1,12, Hoehe x 1,08 als Start, Abdeckung 0,8). Danach von Hand, je gemessen (VARIANTEN.md Abschnitt 13):
Fussbreite W / 1,15 (Modell/Maske war 1,16-1,18), links Knie rk x 0,96 und oberster Ring 0,94 statt 1,02 (Pruefpunkt b1.nl_y342_links, Kante y 342 = Oberkante Unterschenkel);
links Ferse hl 110 und Fusshoehe H 343,4 wie I (Boden BO Bild 2: die Ferse lag eine Port-Zeile tiefer, 6 px im Bodenkasten).
Kein RE-Wert."""
import json
import os
import sys

import numpy as np

import rf_kern
import vm_beine as VB


def main():
    aus = sys.argv[1] if len(sys.argv) > 1 else os.path.join('zw', 'beine_J_neu.json')
    VB.DICKER = True
    VB.BREITE_GEWICHT = 3.0
    B = json.load(open(os.path.join('zw', 'beine_I.json')))
    sz = VB.Szene(os.path.join(rf_kern.HIER, '..', 'H_stoff', 'h2', 'form.json'))
    for sd in ('l', 'r'):
        s0 = dict(B[sd]['mass'])
        s0.update(rk=s0['rk'] * 1.12, ra=s0['ra'] * 1.15)
        xb, _lb = VB.stufe_bein(sz, sd, B[sd]['winkel_unter'], s0)
        a_u = xb[:3]
        s1 = dict(s0, rk=xb[3], ra=xb[4], W=s0['W'] * 1.12, H=s0['H'] * 1.08)
        xf, _lf = VB.stufe_fuss(sz, sd, a_u, np.array(B[sd]['winkel_fuss']), s1, roi_gewicht=0.8)
        s3 = dict(s1, W=xf[3], H=xf[4], L=xf[5], hl=xf[6])
        x = np.array(list(a_u) + list(xf[:3]) + [s3['rk'], s3['ra'], s3['W'], s3['H'], s3['L'], s3['hl']])
        info, _mu, _mf = VB.bewerten(sz, sd, x, ausgabe=True)
        B[sd].update(winkel_unter=[round(float(v), 1) for v in a_u], winkel_fuss=[round(float(v), 1) for v in xf[:3]],
                     mass={k: round(float(v), 1) for k, v in s3.items()}, **info)
    for sd in ('l', 'r'):
        B[sd]['mass']['W'] = round(B[sd]['mass']['W'] / 1.15, 1)
    B['l']['mass']['rk'] = round(B['l']['mass']['rk'] * 0.96, 1)
    B['l']['mass']['oben'] = 0.94     # oberster Ring links (I und rechts 1,02)
    B['l']['mass'].update(hl=110.0, H=343.4)   # Ferse und Fusshoehe links wie I (109,2 / 343,4): Bodenkasten BO in Bild 2
    json.dump(B, open(aus, 'w'), indent=1)
    for sd in ('l', 'r'):
        print(sd, json.dumps({k: B[sd][k] for k in ('mass', 'iou_unter', 'iou_fuss', 'sohle_cos')}))


if __name__ == '__main__':
    main()
