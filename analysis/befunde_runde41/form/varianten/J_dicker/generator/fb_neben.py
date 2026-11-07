#!/usr/bin/env python3
"""Form bemalt (aus rohbau_abnahme_3/a3_neben.py): Original und gefiltertes Render (aus dem eigenen Pruefungslauf) nebeneinander, normal und aufgehellt, mit Nutzerumrissen."""
import os, json
import numpy as np, cv2
import sys
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H); NT = os.path.join(F, 'nutzer_teile')
TAG = sys.argv[1]; ZIEL = sys.argv[2] if len(sys.argv) > 2 else H
lage = json.load(open(os.path.join(NT, 'lage.json')))
FARBE = {'tail': (80, 230, 120), 'left_leg': (40, 140, 255), 'right_leg': (255, 220, 60), 'right_arm': (255, 60, 255), 'part_of_Body': (150, 170, 255),
         'face_and_upper_body': (60, 230, 255), 'left_arm_front': (60, 255, 60), 'right_arm_front': (255, 120, 40)}
AUS = {1: (175, 165, 450, 485), 2: (120, 130, 490, 485)}
for n in (1, 2):
    bl = cv2.imread(os.path.join(H, 'lauf', TAG, 'blatt_bild%d.png' % n))
    orig = bl[34:514, 12:652]; filt = bl[34:514, 1316:1956]
    assert orig.shape == (480, 640, 3) and filt.shape == (480, 640, 3)
    # Probe: Original-Kachel gegen bild1.png
    o2 = cv2.imread(os.path.join(F, 'bild%d.png' % n))
    print('Bild', n, 'Kachel gegen Original MAE', float(np.abs(orig.astype(int) - o2.astype(int)).mean()))
    x0, y0, x1, y1 = AUS[n]; k = 3
    def hell(a):
        return (255 * (a / 255.0) ** 0.5).astype(np.uint8)
    def umriss(a):
        a = a.copy()
        for nm, L in lage.items():
            if L['bild'] != n:
                continue
            m = cv2.imread(os.path.join(NT, 'maske_%s.png' % nm), cv2.IMREAD_GRAYSCALE)
            m = (m > 127).astype(np.uint8)[y0:y1, x0:x1]
            m = cv2.resize(m, None, fx=k, fy=k, interpolation=cv2.INTER_NEAREST)
            cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(a, cs, -1, FARBE[nm], 2)
        return a
    def gross(a):
        return cv2.resize(a[y0:y1, x0:x1], None, fx=k, fy=k, interpolation=cv2.INTER_NEAREST)
    reihe1 = np.hstack([gross(orig), gross(filt)])
    reihe2 = np.hstack([umriss(gross(hell(orig))), umriss(gross(hell(filt)))])
    tafel = np.vstack([reihe1, reihe2])
    for (tx, ty, t) in ((10, 30, 'Original'), (reihe1.shape[1] // 2 + 10, 30, 'Render durch video_filter'),
                        (10, reihe1.shape[0] + 30, 'Original, Gamma 0,5, Nutzerumrisse'), (reihe1.shape[1] // 2 + 10, reihe1.shape[0] + 30, 'Render gefiltert, Gamma 0,5, Nutzerumrisse')):
        cv2.putText(tafel, t, (tx, ty), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 255), 2)
    cv2.imwrite(os.path.join(ZIEL, 'neben_bild%d.png' % n), tafel)
    # kleine Fassung im Originalmassstab 2x, ganze Bilder
    ganz = np.hstack([cv2.resize(orig, None, fx=1, fy=1), filt])
    pass
print('ok')
