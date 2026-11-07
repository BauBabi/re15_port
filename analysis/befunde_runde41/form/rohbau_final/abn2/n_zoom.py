#!/usr/bin/env python3
"""Echter Pruefrender gegen Original, Ausschnitt vergroessert: Original aufgehellt mit Umriss Render (gelb), ziel2 mit Render halbtransparent (rot = auf Schwarz, gruen = auf Weiss, blau = Weiss fehlt).
python abn2/n_zoom.py AUS.png bild,x0,y0,x1,y1,faktor [weitere]"""
import os
import sys

import cv2
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from rf_kern import FORM, HIER
import form_pruefung as P


def blatt(k, n, x0, y0, x1, y1, s):
    sauber, alb, m, _ = k.rendern(n)
    o = P.original(n)
    z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
    he = np.clip(255 * np.power(np.clip(o.astype(np.float32) / 255 * 2.2, 0, 1), .7), 0, 255).astype(np.uint8)
    gr = lambda a, st=cv2.INTER_CUBIC: cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=st)  # noqa: E731
    a = gr(he)[:, :, ::-1].copy()
    mg = gr(m.astype(np.uint8), cv2.INTER_NEAREST)
    ko, _x = cv2.findContours(mg, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, ko, -1, (0, 255, 255), 1)
    b = np.zeros(z.shape + (3,), np.uint8)
    b[z == 255] = (200, 200, 200)
    b[z == 128] = (90, 90, 90)
    b[(z == 255) & ~m] = (255, 80, 40)      # Weiss fehlt
    b[(z == 255) & m] = (60, 170, 60)
    b[(z == 0) & m] = (40, 40, 255)         # auf Schwarz
    b[(z == 128) & m] = (60, 110, 60)
    c = gr(b, cv2.INTER_NEAREST)
    d = gr(sauber)[:, :, ::-1].copy()
    for im in (a, c, d):
        for x in range(x0 - x0 % 10 + 10, x1, 10):
            cv2.line(im, ((x - x0) * s, 0), ((x - x0) * s, 5), (0, 255, 0), 1)
            cv2.putText(im, str(x), ((x - x0) * s + 2, 15), 0, .33, (0, 255, 0), 1, cv2.LINE_AA)
        for y in range(y0 - y0 % 10 + 10, y1, 10):
            cv2.line(im, (0, (y - y0) * s), (5, (y - y0) * s), (0, 255, 0), 1)
            cv2.putText(im, str(y), (7, (y - y0) * s + 4), 0, .33, (0, 255, 0), 1, cv2.LINE_AA)
    return np.hstack([a, c, d])


def main():
    k = P.Kandidat(os.path.join(HIER, 'form.json'))
    z = []
    for t in sys.argv[2:]:
        n, x0, y0, x1, y1, s = [int(v) for v in t.split(',')]
        z.append(blatt(k, n, x0, y0, x1, y1, s))
    w = max(a.shape[1] for a in z)
    z = [np.pad(a, ((0, 4), (0, w - a.shape[1]), (0, 0))) for a in z]
    cv2.imwrite(sys.argv[1], np.vstack(z))


if __name__ == '__main__':
    main()
