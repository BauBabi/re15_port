#!/usr/bin/env python3
"""Form bemalt, Hautmass: mittlere Schirmhelligkeit der Haut im Render (Licht 'mess', ohne Filter) gegen die zurueckgerechnete Schirmfarbe (farbe_bild*_echt.png),
auf Figurpixeln ohne Zeichnung, getrennt nach Teilen. python fb_hautmass.py [form.json]. Kein RE-Wert."""
import os
import sys

import cv2
import numpy as np

from rf_kern import FORM
import form_pruefung as P
import fb_mal
import rf_kern

k = P.Kandidat(sys.argv[1] if len(sys.argv) > 1 else 'form.json')
ms, logo = rf_kern.masken()
zeich = {1: np.zeros((480, 640), bool), 2: np.zeros((480, 640), bool)}
for i, n, mk, f in fb_mal.liste():
    zeich[n] |= cv2.dilate(mk.astype(np.uint8), np.ones((7, 7), np.uint8)) > 0
for n in (1, 2):
    sauber, alb, m, kk = k.rendern(n)
    e = cv2.imread(os.path.join(FORM, 'farbe_bild%d_echt.png' % n))[:, :, ::-1].astype(np.float32)
    r = sauber.astype(np.float32)
    zeile = []
    for t in rf_kern.TEILE[n]:
        s = ms[t] & m & ~zeich[n] & ~logo
        s = cv2.erode(s.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        if s.sum() < 30:
            continue
        ye, yr = float((e[s] @ P.YK).mean()), float((r[s] @ P.YK).mean())
        sd_e, sd_r = float((e[s] @ P.YK).std()), float((r[s] @ P.YK).std())
        zeile.append('%s echt Y %.1f (sd %.1f) Render %.1f (sd %.1f) RGB echt %s Render %s' % (t, ye, sd_e, yr, sd_r, np.round(e[s].mean(0)).astype(int), np.round(r[s].mean(0)).astype(int)))
    print('Bild', n, '\n  ' + '\n  '.join(zeile))
