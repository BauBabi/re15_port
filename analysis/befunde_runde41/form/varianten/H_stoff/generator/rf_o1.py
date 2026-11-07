#!/usr/bin/env python3
"""Nacharbeit Abnahme 1, Blatt fuer die Nutzerfrage O1: rechts neben dem Arm in Bild 1. Links Original, Mitte Umriss alter Stand, rechts Umriss neuer Stand, beide auf dem aufgehellten Original.
python rf_o1.py -> o1_blatt.png. Vergleicht FORM.EMD im Ordner mit stand_vor_abn1/."""
import os

import cv2
import numpy as np

from rf_kern import HIER
import form_pruefung as P


def maske(pfad):
    k = P.Kandidat(pfad)
    return k.rendern(1)[2]


def main():
    x0, y0, x1, y1 = 350, 222, 450, 382
    s = 5
    o = P.original(1)[y0:y1, x0:x1].astype(np.float32)
    hell = (255 * np.power(np.clip(o / 255 * 1.2, 0, 1), .6)).astype(np.uint8)
    b = cv2.resize(hell, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)

    def tafel(m, t, farbe):
        a = b.copy()
        if m is not None:
            q = cv2.resize(m[y0:y1, x0:x1].astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
            k, _ = cv2.findContours(q, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.polylines(a, k, True, farbe, 1, cv2.LINE_AA)
        for xx, yy in ((405, 255), (431, 300)):
            pass
        cv2.rectangle(a, ((405 - x0) * s, (255 - y0) * s), ((431 - x0) * s, (300 - y0) * s), (255, 200, 0), 1)
        cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (255, 255, 120), 1, cv2.LINE_AA)
        return a
    alt = maske(os.path.join(HIER, 'stand_vor_abn1', 'form.json'))
    neu = maske(os.path.join(HIER, 'form.json'))
    bild = np.hstack([tafel(None, 'Original aufgehellt, Kasten = O1', (255, 255, 255)), tafel(alt, 'Umriss vor Abnahme 1', (255, 255, 255)), tafel(neu, 'Umriss jetzt', (255, 255, 255))])
    cv2.imwrite(os.path.join(HIER, 'o1_blatt.png'), bild[:, :, ::-1])
    print('o1_blatt.png')


if __name__ == '__main__':
    main()
