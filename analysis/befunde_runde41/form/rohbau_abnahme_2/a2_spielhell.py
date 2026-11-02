#!/usr/bin/env python3
"""Abnahme 2: Spielbilder aufgehellt und vergroessert, nur zum Lesen der Form im echten Spiel."""
import cv2, os
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
def hell(a, g=4.0, e=.6):
    return (255 * np.power(np.clip(a.astype(np.float32) / 255 * g, 0, 1), e)).astype(np.uint8)
Z = [('p1_g1024', (0, 90, 480, 340)), ('p1_g3072', (120, 90, 640, 340)), ('ruhe_g3072', (120, 90, 640, 340)), ('ruhe_g1024', (0, 90, 480, 340)), ('p1_g0', (160, 110, 480, 460)), ('ruhe_g0', (160, 100, 520, 460)), ('p2_dreh3', (120, 90, 640, 340)), ('p2_dreh1', (0, 90, 480, 340))]
t = []
for n, (x0, y0, x1, y1) in Z:
    a = cv2.imread(os.path.join(HIER, 'spiel', n + '.png'))
    c = hell(a[y0:y1, x0:x1])
    c = cv2.resize(c, (640, 420), interpolation=cv2.INTER_NEAREST)
    cv2.putText(c, n, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(c, n, (6, 18), cv2.FONT_HERSHEY_SIMPLEX, .55, (120, 255, 255), 1, cv2.LINE_AA)
    t.append(c)
cv2.imwrite(os.path.join(HIER, 'spiel', 'hell_tafel.png'), np.vstack([np.hstack(t[:4 // 2 * 1 + 0 + 2]) if False else np.hstack(t[0:2]), np.hstack(t[2:4]), np.hstack(t[4:6]), np.hstack(t[6:8])]))
