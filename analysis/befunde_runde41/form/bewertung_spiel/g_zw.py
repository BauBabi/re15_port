# -*- coding: utf-8 -*-
"""Schreibt je Kandidat ein EMD mit den Biegeposen als Clips (0 gehen, 1 hieb, 2 biss, 3 drohen, 4 seitenhieb), nur fuer die Spielprobe."""
import shutil
from g_biege import *

os.makedirs(AUS + '/zw', exist_ok=True)
for k in ('A', 'B', 'C'):
    form = lade(k)
    R, T = pose(form, 2)
    neu = neu_form(form, R, T, posen(k))
    open(AUS + '/zw/biege_%s.EMD' % k, 'wb').write(FE.schreibe_emd(neu, pruefen=False))
    shutil.copyfile(K[k][1], AUS + '/zw/biege_%s.TIM' % k)
    print(k, [c['name'] for c in neu['clips']])
