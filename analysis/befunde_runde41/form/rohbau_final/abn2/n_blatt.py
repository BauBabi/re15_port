#!/usr/bin/env python3
"""Tonbilder eines Tags zu einem Blatt. python n_blatt.py TAG AUS.png [spalten]"""
import glob, os, sys
import cv2, numpy as np
H = os.path.dirname(os.path.abspath(__file__))
f = sorted(glob.glob(os.path.join(H, 'netz', sys.argv[1] + '*_c*.png')))
sp = int(sys.argv[3]) if len(sys.argv) > 3 else 4
z = []
for p in f:
    a = cv2.imread(p)
    cv2.putText(a, os.path.basename(p)[:-4], (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, os.path.basename(p)[:-4], (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .5, (120, 255, 255), 1, cv2.LINE_AA)
    z.append(a)
while len(z) % sp:
    z.append(np.zeros_like(z[0]))
cv2.imwrite(sys.argv[2], np.vstack([np.hstack(z[i:i + sp]) for i in range(0, len(z), sp)]))
