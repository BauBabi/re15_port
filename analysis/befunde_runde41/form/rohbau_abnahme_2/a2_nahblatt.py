#!/usr/bin/env python3
"""Abnahme 2: je Tag ein Nahblatt nebeneinander."""
import cv2, glob, os
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
for tag in ('nfuss2', 'nfuss0', 'nfuss1', 'nkopf2', 'nkopf1', 'nhand0', 'nrueck0', 'nrueck1', 'nrueck2'):
    fs = sorted(glob.glob(os.path.join(HIER, 'netz', tag + '_c*_az*.png')))
    z = []
    for f in fs:
        a = cv2.imread(f)
        t = os.path.basename(f)[:-4]
        cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (120, 255, 255), 1, cv2.LINE_AA)
        z.append(a)
    if z:
        cv2.imwrite(os.path.join(HIER, 'bild', tag + '.png'), np.hstack(z))
        print(tag, len(z))
