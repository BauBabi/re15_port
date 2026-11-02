#!/usr/bin/env python3
"""Abnahme 2: Kontaktblatt aus den Tonbildern. python a2_kontakt.py TAG CLIP EL SPALTEN NAME"""
import cv2, glob, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
tag, clip, el, sp, name = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), sys.argv[5]
fs = sorted(glob.glob(os.path.join(HIER, 'netz', '%s_c%s_az*_el%s.png' % (tag, clip, el))))
z = []
for f in fs:
    a = cv2.imread(f)
    t = os.path.basename(f)[:-4]
    cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (120, 255, 255), 1, cv2.LINE_AA)
    z.append(a)
while len(z) % sp:
    z.append(np.zeros_like(z[0]))
zeilen = [np.hstack(z[i:i + sp]) for i in range(0, len(z), sp)]
cv2.imwrite(os.path.join(HIER, 'bild', name), np.vstack(zeilen))
print(name, len(fs))
