# -*- coding: utf-8 -*-
"""Wo scheint es in Ruhe durch? Karten fuer den schlechtesten Blick je Blickhoehe; Spielbilder als Tafel, falls vorhanden."""
import sys, glob
import numpy as np
import cv2
from g_gelenk import *

for k, ruhe in (('A', 2), ('C', 2)):
    form = lade(k)
    n = len(form['knochen'])
    R, T = pose(form, ruhe)
    W = welt(form, R, T)
    tris = [dreiecke(m) for m in form['meshes'][:n]]
    for e in (0, 20, 40, 65):
        s, b, sk = durchblick(form, W, tris, els=(e,))
        print(k, 'el', e, 'Summe', s, 'schlechtester', b[0], b[1])
        if b[2] is None:
            continue
        g = np.zeros(b[2].shape + (3,), np.uint8)
        g[b[3] > 0] = (150, 150, 150)
        g[b[2] > 0] = (0, 0, 255)
        cv2.imwrite(AUS + '/lochruhe_%s_%d.png' % (k, e), g)
