#!/usr/bin/env python3
"""Linke Klaue Bild 2: Nutzermaske, Render-Maske, Original, 8-fach um (400-480, 360-430)."""
import os, sys
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P, form_buehne as FB
k = P.Kandidat(os.path.join(F, 'rohbau_final', 'form.json'))
lage, gier, clip, bild = k.stand(2); kam = P.kamera(2)
w = FB.Textur(k.tim); w.lut = w.lut.copy(); w.lut[..., :3] = 255
r = FB.rastern(FB.dreiecke(k.mo, k.kf(clip, bild), lage, gier, kam, None), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
m = cv2.imread(os.path.join(F, 'nutzer_teile', 'maske_left_arm_front.png'), cv2.IMREAD_GRAYSCALE) > 127
o = cv2.imread(os.path.join(F, 'bild2.png'))
x0, y0, x1, y1 = 395, 340, 485, 432
for (cx, cy) in ((439, 418), (468, 417)):
    d = 99
    ys, xs = np.nonzero(r)
    d = np.min(np.hypot(xs - cx, ys - cy))
    print('Spitze (%d,%d): naechstes Render-Pixel %.1f px, Maske dort %s' % (cx, cy, d, m[cy, cx]))
b = o.copy() // 2
b[r & m] = (200, 200, 200); b[r & ~m] = (0, 0, 255); b[m & ~r] = (0, 200, 0)
cv2.imwrite(os.path.join(H, 'bild', 'klaue_links_b2.png'), np.hstack([cv2.resize(o[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST), cv2.resize(b[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)]))
