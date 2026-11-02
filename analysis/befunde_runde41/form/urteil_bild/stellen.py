#!/usr/bin/env python3
"""Zahlen zu dem, was im Bild auffiel. Kein Urteil, nur Beleg: Zeichnung Y, Feuer gedeckt, Luecken, Rand der Hand. Kein RE-Wert."""
import json
import os
import sys

import cv2
import numpy as np

H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
from einsatz import P


def zeichnung():
    print('--- Zeichnung Y Video/Filter je Stelle')
    d = {r: json.load(open(os.path.join(H, 'mess', 'P_' + r, 'bericht.json'), encoding='utf-8')) for r in 'ABC'}
    for n in (1, 2):
        ids = [z['id'] for z in d['A']['bild%d' % n]['zeichnung']]
        print('Bild', n)
        for i in ids:
            z = []
            for r in 'ABC':
                q = [x for x in d[r]['bild%d' % n]['zeichnung'] if x['id'] == i][0]
                v = q.get('y_video', q.get('Y_video'))
                g = q.get('y_render', q.get('y_filter', q.get('Y_render')))
                z.append('%s %s/%s %s' % (r, v, g, 'ok' if q.get('ok') else 'NEIN'))
            print('  %-4s' % i, '  '.join(z))
    print('--- Flaechen und Punkte, die fehlen')
    for r in 'ABC':
        for n in (1, 2):
            b = d[r]['bild%d' % n]
            p = [x['id'] for x in b['punkte'] if x.get('zaehlt', True) and not x['ok']]
            f = ['%s(%s)' % (x['id'], x.get('treffer', x.get('anteil', ''))) for x in b['flaechen'] if x.get('zaehlt', True) and not x['ok']]
            print(r, 'Bild', n, 'Punkte', p, 'Flaechen', f, 'NCC', b['ncc_weiss'], 'MAE', b['mae_weiss'])


def feuer():
    o = P.original(2).astype(np.float32)
    y = o @ P.YK
    hell = cv2.GaussianBlur(y, (0, 0), 1) >= 170
    print('--- Feuer im Original (Y>=170), vom Modell ueberdeckt')
    for name, (x0, y0, x1, y1) in dict(links=(150, 325, 248, 395), rechts=(415, 225, 475, 395)).items():
        z = []
        w = np.zeros(y.shape, bool)
        w[y0:y1, x0:x1] = True
        w &= hell
        for r in 'ABC':
            m = cv2.imread(os.path.join(H, 'maske_%s_bild2.png' % r), 0) > 0
            z.append('%s %d von %d (%.0f %%)' % (r, (m & w).sum(), w.sum(), 100 * (m & w).sum() / w.sum()))
        print(name, '  '.join(z))


def luecke():
    print('--- Helle Luecke Bild 1 zwischen Schwanz und rechtem Bein: im Original Y>=150 in (280-320,310-385), Abgleich mit Render')
    y = cv2.GaussianBlur(P.original(1).astype(np.float32) @ P.YK, (0, 0), 1)
    w = np.zeros(y.shape, bool)
    w[310:385, 280:320] = True
    h = w & (y >= 150)
    print('Original hell', h.sum(), 'y', np.nonzero(h)[0].min(), np.nonzero(h)[0].max())
    for r in 'ABC':
        m = cv2.imread(os.path.join(H, 'maske_%s_bild1.png' % r), 0) > 0
        fr = w & ~m
        print(r, 'Luecke im Render', fr.sum(), 'davon im Original hell', (fr & h).sum(), 'Luecke auf Dunkel', (fr & ~h).sum(), 'Original hell verdeckt', (h & m).sum(), 'y', np.nonzero(fr)[0].min(), np.nonzero(fr)[0].max())


def dicken():
    print('--- Breite right_arm Bild 1 je Zeile, Render')
    for r in 'ABC':
        m = cv2.imread(os.path.join(H, 'maske_%s_bild1.png' % r), 0) > 0
        z = []
        for yy in (310, 330, 350, 360, 365, 370):
            xs = np.nonzero(m[yy, 365:445])[0]
            z.append('y%d %d-%d' % (yy, xs.min() + 365, xs.max() + 365) if xs.size else 'y%d -' % yy)
        print(r, '  '.join(z))
    mt = cv2.imread(os.path.join(H, '..', 'nutzer_teile', 'maske_right_arm.png'), 0) > 128
    z = []
    for yy in (310, 330, 350, 360, 365, 370):
        xs = np.nonzero(mt[yy, :])[0]
        z.append('y%d %d-%d' % (yy, xs.min(), xs.max()) if xs.size else 'y%d -' % yy)
    print('Nutzer', '  '.join(z))


if __name__ == '__main__':
    zeichnung()
    feuer()
    luecke()
    dicken()
