#!/usr/bin/env python3
"""Spielbilder als Tafel: Zeile je Clip, Spalte je Gier, Ausschnitt um die Figur, Gamma 0,5 (Lesehilfe)."""
import os
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); S = os.path.join(H, 'spiel')
x0, y0, x1, y1 = 110, 90, 530, 460
CL = {0: 'Pose1', 1: 'Pose2', 2: 'Ruhe'}
for art, gam in (('roh', 1.0), ('hell', 0.5)):
    zeilen = []
    for c in (0, 1, 2):
        z = []
        for g in (0, 1024, 2048, 3072):
            p = os.path.join(S, 'c%d_g%d_s11.png' % (c, g))
            im = cv2.imread(p) if os.path.exists(p) else np.zeros((480, 640, 3), np.uint8)
            im = (255 * (im / 255.0) ** gam).astype(np.uint8)[y0:y1, x0:x1].copy()
            cv2.putText(im, '%s gier %d' % (CL[c], g), (6, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
            z.append(im)
        zeilen.append(np.hstack(z))
    cv2.imwrite(os.path.join(H, 'bild', 'spiel_dreh_%s.png' % art), np.vstack(zeilen))
print('ok')
