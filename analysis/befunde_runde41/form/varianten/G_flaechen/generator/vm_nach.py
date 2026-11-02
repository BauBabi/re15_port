#!/usr/bin/env python3
"""Varianten, Nachpassung fuer die Geometrie mit weniger Flaechen: fb_nach.Nach mit den Ecken und Ringen von vm_form 'wenig' (gleiche Regel, gleiche Ziele,
Start = zw/delta.json von form_bemalt). python vm_nach.py --form ../G/form.json --aus zw/delta_wenig.json [--schritte 250]. Kein RE-Wert."""
import argparse
import json
import os

import fb_glatt
import fb_nach
import vm_form
from rf_kern import HIER

if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--form', required=True)
    a.add_argument('--aus', default=os.path.join(HIER, 'zw', 'delta_wenig.json'))
    a.add_argument('--start', default=os.path.join(HIER, 'zw', 'delta.json'))
    a.add_argument('--schritte', type=int, default=250)
    a.add_argument('--reg', type=float, default=0.5)
    a.add_argument('--lr', type=float, default=0.004)
    a = a.parse_args()
    fb_glatt.ECKEN_ART.update(vm_form.WENIG_ECKEN_ART)
    fb_glatt.ECKEN_NAME.update(vm_form.WENIG_ECKEN_NAME)
    for k in fb_glatt.SCHRITT:
        if k not in ('finger', 'kegel'):
            fb_glatt.SCHRITT[k] = vm_form.WENIG_SCHRITT
    fb_nach.REG = a.reg
    n = fb_nach.Nach(json.load(open(os.path.join(HIER, 'stand_final.json'))), a.form, a.start)
    n.lauf(a.schritte, lr=a.lr)
    n.speichern(a.aus)
