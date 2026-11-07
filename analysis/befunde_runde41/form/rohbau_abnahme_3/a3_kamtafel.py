#!/usr/bin/env python3
"""Ton aus der Screenshot-Kamera ueber die Homographie ins Bild, neben das Original (Gamma 0,5), Nutzerumrisse auf allen."""
import os, json
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H); NT = os.path.join(F, 'nutzer_teile')
lage = json.load(open(os.path.join(NT, 'lage.json')))
FARBE = {'tail': (80, 230, 120), 'left_leg': (40, 140, 255), 'right_leg': (255, 220, 60), 'right_arm': (255, 60, 255), 'part_of_Body': (150, 170, 255),
         'face_and_upper_body': (60, 230, 255), 'left_arm_front': (60, 255, 60), 'right_arm_front': (255, 120, 40)}
AUS = {1: (175, 165, 450, 485), 2: (120, 130, 490, 485)}
for n in (1, 2):
    kam = json.load(open(os.path.join(F, 'kamera_bild%d.json' % n)))
    Bm = np.array(kam['abbildung_port_nach_bild']['B'], float) @ np.diag([0.5, 0.5, 1.0])
    # Pixelmitte: Port-Pixel (u+0.5) -> grob; fuer eine Lesehilfe genuegt das
    orig = cv2.imread(os.path.join(F, 'bild%d.png' % n))
    hell = (255 * (orig / 255.0) ** 0.5).astype(np.uint8)
    tafeln = [hell]
    for art in ('kamton', 'kamfar'):
        r = cv2.imread(os.path.join(H, 'bild', '%s_b%d.png' % (art, n)), cv2.IMREAD_UNCHANGED)
        w = cv2.warpPerspective(r, Bm, (640, 480), flags=cv2.INTER_LINEAR)
        al = w[..., 3:4] / 255.0
        bg = (hell * 0.35).astype(float)
        tafeln.append((w[..., :3] * al + bg * (1 - al)).astype(np.uint8))
    x0, y0, x1, y1 = AUS[n]; k = 2
    reihe = []
    for t in tafeln:
        t = cv2.resize(t[y0:y1, x0:x1], None, fx=k, fy=k, interpolation=cv2.INTER_NEAREST)
        for nm, L in lage.items():
            if L['bild'] != n: continue
            m = (cv2.imread(os.path.join(NT, 'maske_%s.png' % nm), cv2.IMREAD_GRAYSCALE) > 127).astype(np.uint8)[y0:y1, x0:x1]
            m = cv2.resize(m, None, fx=k, fy=k, interpolation=cv2.INTER_NEAREST)
            cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(t, cs, -1, FARBE[nm], 2)
        reihe.append(t)
    tafel = np.hstack(reihe)
    for i, txt in enumerate(('Original Gamma 0,5', 'Ton aus Kamera Bild %d' % n, 'Ton je Teil gefaerbt')):
        cv2.putText(tafel, txt, (i * reihe[0].shape[1] + 8, 24), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
    cv2.imwrite(os.path.join(H, 'bild', 'kamtafel_bild%d.png' % n), tafel)
print('ok')
