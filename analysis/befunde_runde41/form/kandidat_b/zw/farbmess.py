"""Mittelfarbe dunkler Flaechen: Original gegen gefilterten Render. Aufruf: farbmess.py ORDNER"""
import sys, json
sys.path.insert(0, '.')
from b_geom import *
import numpy as np, cv2
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
import form_video as FV
o = sys.argv[1]
k = P.Kandidat(o + '/kandidat.json')
Zonen = {1: dict(rechts=(352, 250, 378, 300), oben=(300, 196, 336, 214), links=(255, 225, 295, 262), pfeiler=(388, 305, 410, 350), saeule=(236, 305, 280, 335), kugel=(214, 352, 240, 372), untermasse=(300, 372, 340, 392)),
         2: dict(innen=(290, 225, 340, 280), oben=(300, 200, 345, 222), unten=(245, 300, 300, 335))}
for n in (1, 2):
    sauber, alb, m, lk = k.rendern(n)
    gef = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
    orig = P.original(n)
    for nm, (x0, y0, x1, y1) in Zonen[n].items():
        a = orig[y0:y1, x0:x1].reshape(-1, 3).mean(0)
        b = gef[y0:y1, x0:x1].reshape(-1, 3).mean(0)
        c = sauber[y0:y1, x0:x1].reshape(-1, 3).mean(0)
        d = alb[y0:y1, x0:x1].reshape(-1, 3).mean(0)
        print(n, '%-10s Video %s  Filter %s  Schirm %s  Textur %s' % (nm, np.round(a).astype(int), np.round(b).astype(int), np.round(c).astype(int), np.round(d).astype(int)))
