#!/usr/bin/env python3
"""Lesehilfe: Video 4-fach (Gamma) je Bild mit allen Nutzer-Umrissen und Mittellinien, Raster alle 20 px."""
import json, os
import numpy as np, cv2
F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
T = os.path.join(F, 'nutzer_teile')
A = os.path.join(F, 'gerippe', 'lauf')
mass = json.load(open(os.path.join(A, 'teile_mass.json'), encoding='utf-8'))
FARBE = {'tail': (80, 255, 80), 'left_leg': (0, 160, 255), 'right_leg': (255, 230, 0), 'right_arm': (255, 60, 255),
         'part_of_Body': (160, 160, 255), 'face_and_upper_body': (0, 255, 255), 'left_arm_front': (60, 255, 60),
         'right_arm_front': (255, 120, 40)}
def blatt(nr, x0, y0, x1, y1, s, g, aus, kanal=None):
    b = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
    if kanal is not None:
        b = np.repeat(b[..., kanal:kanal+1], 3, axis=2)
    b = b[y0:y1, x0:x1] ** g
    b = cv2.resize(b, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
    b = (np.clip(b, 0, 1) * 255).astype(np.uint8)
    for x in range((x0 // 20 + 1) * 20, x1, 20):
        cv2.line(b, ((x - x0) * s, 0), ((x - x0) * s, 8 if x % 100 else 18), (255, 255, 255), 1)
        if x % 40 == 0:
            cv2.putText(b, str(x), ((x - x0) * s + 2, 26), cv2.FONT_HERSHEY_SIMPLEX, .38, (255, 255, 255), 1, cv2.LINE_AA)
    for y in range((y0 // 20 + 1) * 20, y1, 20):
        cv2.line(b, (0, (y - y0) * s), (8, (y - y0) * s), (255, 255, 255), 1)
        if y % 40 == 0:
            cv2.putText(b, str(y), (10, (y - y0) * s + 4), cv2.FONT_HERSHEY_SIMPLEX, .38, (255, 255, 255), 1, cv2.LINE_AA)
    for n, m in mass.items():
        if m['bild'] != nr:
            continue
        k = cv2.imread(os.path.join(T, 'maske_%s.png' % n), cv2.IMREAD_UNCHANGED)
        k = (k[..., -1] if k.ndim == 3 else k)
        c, _ = cv2.findContours((k > 127).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for q in c:
            q = ((q.reshape(-1, 2) - [x0, y0]) * s + s // 2).astype(np.int32)
            cv2.polylines(b, [q], True, FARBE[n], 1, cv2.LINE_AA)
        ml = np.array(m['mittellinie'])[:, :2]
        q = ((ml - [x0, y0]) * s + s // 2).astype(np.int32)
        cv2.polylines(b, [q], False, FARBE[n], 1, cv2.LINE_AA)
    cv2.imwrite(os.path.join(A, aus), b)
blatt(1, 185, 170, 440, 480, 3, 0.55, 'b1_gamma_x3.png')
blatt(2, 135, 130, 480, 480, 3, 0.55, 'b2_gamma_x3.png')
blatt(2, 135, 130, 480, 480, 3, 0.5, 'b2_blau_x3.png', 0)
