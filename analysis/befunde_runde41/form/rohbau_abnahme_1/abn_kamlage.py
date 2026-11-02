#!/usr/bin/env python3
"""Abnahme 1: Schnitte lesen und Lagen fuer die zweite Spielreihe rechnen: Figur im Bild, Abstand wie Bild 1, Blick auf die Figur wie in Bild 1. Kein RE-Wert."""
import os, sys, math, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_buehne as FB
import form_pruefung as P
import form_kamera as FK
k = P.Kandidat(os.path.join(FORM, 'rohbau_final', 'form.json'))
for raum in ('5080', '5090'):
    r = FB.Raum(raum)
    print('Raum', raum, 'Schnitte', r.n)
    for i in range(r.n):
        s = r.schnitt(i)
        p, z = np.array(s['pos'], float), np.array(s['ziel'], float)
        d = z - p
        print('  %2d pos %s ziel %s fov %d H %d Blick xz %.0f Grad, Hoehe %d, Nick %.1f' % (i, s['pos'], s['ziel'], s['fov'], s['fov'] >> 7, math.degrees(math.atan2(d[2], d[0])), -p[1], math.degrees(math.atan2(-d[1], math.hypot(d[0], d[2])))))
f = FK.lade_kamera(1)
A = np.array(f['auge']); Z = np.array(f['ziel'])
lage, gier, clip, bild = k.stand(1)
L = np.array(lage, float)
print('Bild 1 Auge', A, 'Lage', L, 'Gier', gier)
dv = L[[0, 2]] - A[[0, 2]]
print(' Abstand xz', np.hypot(*dv), 'Winkel Auge->Figur', math.degrees(math.atan2(dv[1], dv[0])))
