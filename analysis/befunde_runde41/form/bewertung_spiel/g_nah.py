# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Nahansicht und Spielgroesse, gleicher Renderer und gleiches Licht fuer A, B, C und G4 (G4 nur als Handwerks-Eichmass, keine Formvorlage)."""
import sys
import numpy as np
from PIL import Image
from g_gelenk import *

sys.path.insert(0, WT + '/re15_port/tools')
import form_buehne as FB
import form_pruefung as PR


def modell(k):
    emd = open(K[k][0], 'rb').read()
    tim = open(K[k][1], 'rb').read()
    return FE.modell(emd, tim), FB.Textur(tim)


def kf_von(mo, clip):
    f0, _ = mo.clips[clip]
    return mo.frames[f0] & 0xFFF


def bild(mo, tex, kf, az, el, gross, gamma=None, grund=92):
    a = np.full((gross, gross, 3), grund, np.uint8)
    m = FB.rastern(PR.rundum_dreiecke(mo, kf, az, el, gross), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    if gamma:
        a[m] = np.clip(np.power(a[m] / 255.0, gamma) * 255, 0, 255).astype(np.uint8)
    return a, m


CL = {'A': 2, 'B': 2, 'C': 2, 'G4': 5}
tafeln = []
for k in ('A', 'B', 'C', 'G4'):
    mo, tex = modell(k)
    kf = kf_von(mo, CL[k])
    for t, az, el in (('3/4 vorn', 45, 18), ('Seite', 180, 2), ('3/4 hinten', -135, 22)):
        a, _ = bild(mo, tex, kf, az, el, 620, gamma=0.6)
        tafeln.append(('%s %s' % (k, t), a))
FB.montage(tafeln, 3, AUS + '/nah_alle.png', 'Nahansicht, gleicher Renderer, Licht nur fuers Blatt, Gamma 0,6. G4 nur Handwerks-Eichmass')

# Spielgroesse: Figur rund 100 px hoch wie im Fenster 320 x 240, Zwischenpixel hart, dann x3
tafeln = []
for k in ('A', 'B', 'C', 'G4'):
    mo, tex = modell(k)
    kf = kf_von(mo, CL[k])
    for t, az, el in (('vorn', 90, 14), ('3/4', 45, 14), ('Seite', 180, 14), ('hinten', -90, 14), ('3/4 hinten', -135, 14)):
        a, _ = bild(mo, tex, kf, az, el, 170, grund=60)
        b = np.array(Image.fromarray(a).resize((340, 340), Image.NEAREST))
        tafeln.append(('%s %s' % (k, t), b))
FB.montage(tafeln, 5, AUS + '/klein_alle.png', 'Spielgroesse: Figur rund 100 Bildpunkte hoch, x2 hart vergroessert, ohne Gamma')

# Textur-Ausschnitte in Originalfarben, Hautstelle aufgehellt
for k in ('A', 'B', 'C', 'G4'):
    t = FE.tim_lesen(open(K[k][1], 'rb').read())
    c = np.array(t['cluts'], dtype=np.int64)
    lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], axis=-1).astype(np.uint8)
    rgb = lut[0][t['pixel']]
    b = np.clip(np.power(rgb / 255.0, 0.6) * 255, 0, 255).astype(np.uint8)
    Image.fromarray(b).resize((b.shape[1] * 3, b.shape[0] * 3), Image.NEAREST).save(AUS + '/tex_%s.png' % k)
