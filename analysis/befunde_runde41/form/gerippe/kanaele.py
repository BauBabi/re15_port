#!/usr/bin/env python3
"""Lesehilfe: Ausschnitt vergroessert, vier Tafeln (Farbe, R, G, B), Nutzer-Umrisse, Raster. python kanaele.py bild x0 y0 x1 y1 s aus.png"""
import os, sys
import numpy as np, cv2

F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
T = os.path.join(F, 'nutzer_teile')
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}
FARBE = {'tail': (80, 255, 80), 'left_leg': (0, 160, 255), 'right_leg': (255, 230, 0), 'right_arm': (255, 60, 255),
         'part_of_Body': (160, 160, 255), 'face_and_upper_body': (0, 255, 255), 'left_arm_front': (60, 255, 60), 'right_arm_front': (255, 120, 40)}


def blatt(nr, x0, y0, x1, y1, s, pfad, gain=1.5, gam=0.65):
    b = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
    tafeln = []
    for kan in (None, 2, 1, 0):
        c = b if kan is None else np.repeat(b[..., kan:kan + 1], 3, axis=2)
        c = np.clip(c[y0:y1, x0:x1] * gain, 0, 1) ** gam
        c = cv2.resize(c, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
        c = (np.clip(c, 0, 1) * 255).astype(np.uint8)
        for n in TEILE[nr]:
            m = cv2.imread(os.path.join(T, 'maske_%s.png' % n), cv2.IMREAD_UNCHANGED)
            m = (m[..., -1] if m.ndim == 3 else m) > 127
            k, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            for q in k:
                q = ((q.reshape(-1, 2) - [x0, y0]) * s + s // 2).astype(np.int32)
                cv2.polylines(c, [q], True, FARBE[n], 1, cv2.LINE_AA)
        for x in range((x0 // 20 + 1) * 20, x1, 20):
            cv2.line(c, ((x - x0) * s, 0), ((x - x0) * s, 8), (255, 255, 255), 1)
            cv2.putText(c, str(x), ((x - x0) * s + 2, 20), 0, .4, (255, 255, 0), 1, cv2.LINE_AA)
        for y in range((y0 // 20 + 1) * 20, y1, 20):
            cv2.line(c, (0, (y - y0) * s), (8, (y - y0) * s), (255, 255, 255), 1)
            cv2.putText(c, str(y), (10, (y - y0) * s + 4), 0, .4, (255, 255, 0), 1, cv2.LINE_AA)
        tafeln.append(c)
    cv2.imwrite(pfad, np.vstack([np.hstack(tafeln[:2]), np.hstack(tafeln[2:])]))


if __name__ == '__main__':
    a = sys.argv
    blatt(int(a[1]), int(a[2]), int(a[3]), int(a[4]), int(a[5]), int(a[6]), a[7])
