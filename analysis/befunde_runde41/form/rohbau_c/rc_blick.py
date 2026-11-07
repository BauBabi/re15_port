#!/usr/bin/env python3
"""Rohbau C, grosser Rundblick: python rc_blick.py form.json aus.png [clip]. Graue Haut, Licht von oben links, damit die Form lesbar wird."""
import sys

import cv2
import numpy as np

from rc_kern import TOOLS  # noqa: F401  (setzt den Suchpfad)
import form_pruefung as P
import form_buehne as FB


def blatt(j, aus, clip=2, ansichten=None, gross=640, spalten=3):
    k = P.Kandidat(j)
    weiss = FB.Textur(k.tim)
    weiss.lut = weiss.lut.copy()
    weiss.lut[..., :3] = 200
    kf = k.kf(clip, 0)
    A = ansichten or [('rechts', 0, 4), ('links', 180, 4), ('vorn', 90, 8), ('hinten', -90, 8), ('3/4 vorn rechts', 40, 22), ('3/4 vorn links', 140, 22)]
    z = []
    for t, az, el in A:
        a = np.full((gross, gross, 3), 60, np.uint8)
        FB.rastern(P.rundum_dreiecke(k.mo, kf, az, el, gross), weiss, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        cv2.putText(a, t, (8, 20), 0, .55, (255, 255, 255), 1, cv2.LINE_AA)
        z.append(a)
    zeilen = [np.hstack(z[i:i + spalten]) for i in range(0, len(z), spalten)]
    cv2.imwrite(aus, np.vstack(zeilen)[:, :, ::-1])


if __name__ == '__main__':
    blatt(sys.argv[1], sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 2)
