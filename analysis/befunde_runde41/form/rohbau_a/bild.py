#!/usr/bin/env python3
"""Tonblatt mit Korrektur und Posen. python bild.py"""
import os, sys
import bau
from kn import knochen, lade_posen, HIER
import entwurf, ruhe, ton
import netz as NZ
ks = knochen()
N = NZ.netze(entwurf.alle(), bau.korr_laden())
P = lade_posen()
P['ruhe'] = ruhe.pose(ks, P)
ton.blatt(N, (('Pose 1', P['bild1']), ('Pose 2', P['bild2']), ('Ruhe', P['ruhe'])), ks, os.path.join(HIER, 'zw', 'ton.png'))
ton.umriss(N, ks, P, os.path.join(HIER, 'zw', 'ton_umriss.png'))
print('Flaechen', sum(len(m['F']) for m in N))
