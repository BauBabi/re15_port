# -*- coding: utf-8 -*-
"""Tonumfang und Binnenkontrast im gerenderten Bild (richtige CLUT je Flaeche, gleiches Licht), Ausschnitte der Fehlstellen, Glied-Querschnitte."""
import sys
import numpy as np
from PIL import Image
from scipy.ndimage import uniform_filter
from g_gelenk import *
from g_biege import BAU

sys.path.insert(0, WT + '/re15_port/tools')
import form_buehne as FB
import form_pruefung as PR


def modell(k):
    emd = open(K[k][0], 'rb').read()
    tim = open(K[k][1], 'rb').read()
    return FE.modell(emd, tim), FB.Textur(tim)


CL = {'A': 2, 'B': 2, 'C': 2, 'G4': 5}
for k in ('A', 'B', 'C', 'G4'):
    mo, tex = modell(k)
    f0, _ = mo.clips[CL[k]]
    kf = mo.frames[f0] & 0xFFF
    ys, lokal = [], []
    for az, el in ((45, 14), (135, 14), (225, 14), (315, 14)):
        a = np.full((340, 340, 3), 60, np.uint8)
        m = FB.rastern(PR.rundum_dreiecke(mo, kf, az, el, 340), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        y = a.astype(float) @ np.array([.299, .587, .114])
        ys.append(y[m])
        mw = uniform_filter(y, 9)
        sd = np.sqrt(np.maximum(uniform_filter(y * y, 9) - mw * mw, 0))
        lokal.append(sd[m])
    y = np.concatenate(ys)
    l = np.concatenate(lokal)
    print(k, 'Y p10/50/90/99 %s  lokaler Kontrast (Std 9 x 9) Median %.1f  Anteil Y>60: %.3f' % (np.round(np.percentile(y, [10, 50, 90, 99]), 0), float(np.median(l)), float((y > 60).mean())))

print('--- Querschnitt der Beine mitte Glied (Ruhe): Ausdehnung in X und Z je Schicht um die Gliedmitte, Einheiten')
for k in ('A', 'B', 'C'):
    form = lade(k)
    R, T = pose(form, 2)
    W = welt(form, R, T)
    for nm, b in (('FR unter', BAU[k]['FR'][1]), ('FL unter', BAU[k]['FL'][1]), ('NR unter', BAU[k]['NR'][1]), ('NL unter', BAU[k]['NL'][1])):
        v = W[b]
        y0, y1 = v[:, 1].min(), v[:, 1].max()
        m = (v[:, 1] > y0 + .35 * (y1 - y0)) & (v[:, 1] < y0 + .65 * (y1 - y0))
        q = v[m]
        print(k, nm, BAU[k]['namen'][b], 'dx %d dz %d, Hoehe %d' % (q[:, 0].ptp() if hasattr(q[:, 0], 'ptp') else np.ptp(q[:, 0]), np.ptp(q[:, 2]), y1 - y0))

n = Image.open(AUS + '/nah_alle.png').convert('RGB')
W_, H_ = n.size
print('nah_alle', n.size)
s = 3
for name, box in (('A_hinten', (1262, 30, 1880, 650)), ('B_vorn', (8, 677, 628, 1297)), ('C_vorn', (8, 1325, 628, 1945))):
    n.crop(box).resize(((box[2] - box[0]) * 2, (box[3] - box[1]) * 2), Image.NEAREST).save(AUS + '/aus_%s.png' % name)
