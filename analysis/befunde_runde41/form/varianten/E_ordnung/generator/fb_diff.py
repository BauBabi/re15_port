#!/usr/bin/env python3
"""Form bemalt, Umrissvergleich: Render-Maske je Bild fuer zwei Kandidaten, gewonnen (rot), verloren (blau), Ziel weiss/schwarz. Je Knochen: gewonnene Pixel auf Schwarz.
python fb_diff.py A/form.json B/form.json --aus zw/diff.png. Kein RE-Wert."""
import argparse
import os

import cv2
import numpy as np

from rf_kern import FORM
import form_pruefung as P
import form_buehne as FB


def masken(k, n):
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    w = FB.Textur(k.tim)
    w.lut = w.lut.copy()
    w.lut[..., :3] = 255
    m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
    # Kennbild: Knochen je Pixel (Textur durch Knochenfarbe ersetzen geht nicht, darum je Knochen einzeln)
    kenn = np.full((480, 640), -1, np.int32)
    return m


def je_knochen(k, n, gewinn):
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    w = FB.Textur(k.tim)
    w.lut = w.lut.copy()
    w.lut[..., :3] = 255
    tr = FB.dreiecke(k.mo, kf, lage, gier, kam, None)
    return tr


a = argparse.ArgumentParser()
a.add_argument('a')
a.add_argument('b')
a.add_argument('--aus', default='zw/diff.png')
a = a.parse_args()
KA, KB = P.Kandidat(a.a), P.Kandidat(a.b)
zeilen = []
for n, box in ((1, (185, 170, 445, 480)), (2, (135, 135, 480, 480))):
    z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
    ma, mb = masken(KA, n), masken(KB, n)
    b = np.zeros((480, 640, 3), np.uint8)
    b[z == 255] = (70, 70, 70)
    b[ma & mb] = (150, 150, 150)
    b[mb & ~ma] = (0, 0, 255)
    b[ma & ~mb] = (255, 120, 0)
    x0, y0, x1, y1 = box
    zeilen.append(cv2.resize(b[y0:y1, x0:x1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST))
    gew = mb & ~ma
    ver = ma & ~mb
    print('Bild %d: gewonnen %d (auf Schwarz %d, auf Weiss %d), verloren %d (auf Weiss %d)' % (n, gew.sum(), (gew & (z == 0)).sum(), (gew & (z == 255)).sum(), ver.sum(), (ver & (z == 255)).sum()))
    for nm, mm in (('gewonnen auf Schwarz', gew & (z == 0)), ('verloren auf Weiss', ver & (z == 255))):
        cnt, lab, st, _ = cv2.connectedComponentsWithStats(mm.astype(np.uint8), connectivity=8)
        fl = sorted([(st[i, 4], st[i, 0], st[i, 1], st[i, 0] + st[i, 2], st[i, 1] + st[i, 3]) for i in range(1, cnt)], reverse=True)[:6]
        print('   ', nm, fl)
h = max(x.shape[0] for x in zeilen)
cv2.imwrite(a.aus, np.hstack([np.pad(x, ((0, h - x.shape[0]), (0, 10), (0, 0))) for x in zeilen]))
print(a.aus)


def knochenbilder(k, n):
    """Maske je Knochen (jeder allein gerendert)."""
    import copy
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    w = FB.Textur(k.tim)
    w.lut = w.lut.copy()
    w.lut[..., :3] = 255
    orig = k.mo.meshes
    aus = {}
    for b in range(len(orig)):
        ms = []
        for i, m in enumerate(orig):
            if i == b:
                ms.append(m)
            else:
                m2 = copy.copy(m)
                m2.tf, m2.qf = [], []
                ms.append(m2)
        k.mo.meshes = ms
        aus[b] = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
    k.mo.meshes = orig
    return aus


if os.environ.get('FB_KNOCHEN'):
    NAMEN = [x['name'] for x in __import__('json').load(open(os.path.join(os.path.dirname(os.path.abspath(a.b)), 'gerippe_final.json')))['knochen']] if os.path.exists(os.path.join(os.path.dirname(os.path.abspath(a.b)), 'gerippe_final.json')) else None
    for n in (1, 2):
        z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
        gew = masken(KB, n) & ~masken(KA, n) & (z == 0)
        kb = knochenbilder(KB, n)
        print('Bild %d gewonnen auf Schwarz je Knochen:' % n, {b: int((m & gew).sum()) for b, m in kb.items() if (m & gew).sum()})
