#!/usr/bin/env python3
"""Rohbau final, Rundblick im Ton: Zeichenweg der Buehne, eine Normale je Ecke. python rf_ton.py  -> rundblick_pose1.png, rundblick_pose2.png, rundblick_ruhe.png"""
import os

import cv2
import numpy as np

from rf_kern import HIER
import form_buehne as FB
import form_emd as FE
import form_pruefung as P

SICHT = (('rechts', 0, 6), ('links', 180, 6), ('vorn', 90, 8), ('hinten', -90, 16), ('3/4 vorn rechts', 50, 20), ('3/4 hinten links', -130, 24))


def main():
    k = P.Kandidat(os.path.join(HIER, 'form.json'))
    weiss = FB.Textur(k.tim)
    weiss.lut = weiss.lut.copy()
    weiss.lut[..., :3] = 205
    for nm, clip in (('pose1', 0), ('pose2', 1), ('ruhe', 2)):
        kf = k.kf(clip, 0)
        z = []
        for t, az, el in SICHT:
            a = np.full((640, 640, 3), 60, np.uint8)
            FB.rastern(P.rundum_dreiecke(k.mo, kf, az, el, 640), weiss, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
            cv2.putText(a, t, (8, 20), 0, .55, (255, 255, 255), 1, cv2.LINE_AA)
            z.append(a)
        cv2.imwrite(os.path.join(HIER, 'rundblick_%s.png' % nm), np.vstack([np.hstack(z[:3]), np.hstack(z[3:])])[:, :, ::-1])


if __name__ == '__main__':
    main()
