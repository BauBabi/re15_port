#!/usr/bin/env python3
"""Rohbau final, Stellschleife der Marken: Farbe je Marke so, dass die Schirmfarbe im Render (Licht `mess`) der zurueckgerechneten Schirmfarbe des Videos entspricht.
Grund: L0 steht in Bild 1 fast senkrecht, Oberseiten bekommen bis Faktor 1, Flaechen zur Kamera rund 0,4. Palette = Textur bei mittlerem Faktor. python rf_stell.py STAND.json [Runden]"""
import json
import os
import sys

import cv2
import numpy as np

import rf_bau
import rf_marken
import rf_plan
import rf_textur
from rf_kern import HIER
import form_pruefung as P


def messen(k, liste):
    aus = {}
    bild = {n: k.rendern(n) for n in (1, 2)}
    ec = {n: rf_textur.echt(n) for n in (1, 2)}
    for i, n, m, f in liste:
        s = m > 0
        s &= bild[n][2]
        if s.sum() < 6:
            aus[i] = None
            continue
        e_soll = ec[n][s].mean(0)
        e_ist = bild[n][0][s].astype(np.float32).mean(0)
        aus[i] = (float(e_soll @ P.YK), float(e_ist @ P.YK), int(s.sum()))
    return aus


def main():
    stand = json.load(open(os.path.join(HIER, sys.argv[1])))
    runden = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    g, teile = rf_plan.plan()
    pf = os.path.join(HIER, 'zw', 'marken.json')
    ks = json.load(open(pf)) if os.path.exists(pf) else {}
    li = rf_marken.liste()
    for r in range(runden):
        form, lage, dichte, L, C = rf_bau.baue_form(g, teile, stand)
        rgb, bk = rf_textur.backen(form, g, teile, lage, stand, L=L, C=C)
        rf_bau.schreibe(form, rgb, HIER, g, stand)
        k = P.Kandidat(os.path.join(HIER, 'form.json'))
        z = messen(k, li)
        print('Runde', r, ' '.join('%s %.0f/%.0f' % (i, v[0], v[1]) for i, v in z.items() if v))
        for i, v in z.items():
            if v is None or i in ('Gg',):
                continue
            q = (v[0] + 2.0) / max(v[1], 2.0)
            # Grenzen je Marke: Spanne der Palette (Pruefung Material) nur dort verlassen, wo das Licht von Bild 1 es erzwingt
            hi = dict(Z2=1.4, Q4=1.4, Q1=1.5, Q2=1.25, Q3=1.5, H8=1.5, H9=1.5, Z6a=1.25).get(i, 1.6)      # Z6a: ueber 1,25 faellt sie aus dem Material helle_baender (Abgabe 17:18: 1,54, Material 0,00; vor Abnahme 2: 1,22, Material 1,00)
            lo = dict(Z3=.95, Z4=1.06, Z6a=1.06, Z5=1.03).get(i, .3)      # Abnahme 2: Z4 liegt auf dem Arm jetzt heller, darunter faellt sie aus dem Material. Z5: bei 0,91 Material 0,00 (Lauf s7_final), bei 1,03 0,40
            ks[i] = float(np.clip(ks.get(i, 1.0) * q ** .8, lo, hi))
        json.dump(ks, open(pf, 'w'), indent=0)
    print({k_: round(v, 2) for k_, v in ks.items()})


if __name__ == '__main__':
    main()
