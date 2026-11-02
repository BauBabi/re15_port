#!/usr/bin/env python3
"""Probe nur im Speicher: dieselbe Senkung (Knochen-lokal, aus Pose 1 bestimmt) gegen Bild 2. Kein RE-Wert, aendert keine Datei."""
import os, sys
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P, form_buehne as FB
CUT = int(sys.argv[1]); SENK = int(sys.argv[2])
k = P.Kandidat(os.path.join(F, 'rohbau_final', 'form.json'))
w = FB.Textur(k.tim); w.lut = w.lut.copy(); w.lut[..., :3] = 255
def maske(n):
    lage, gier, clip, bild = k.stand(n); kam = P.kamera(n)
    return FB.rastern(FB.dreiecke(k.mo, k.kf(clip, bild), lage, gier, kam, None), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
vor = maske(2)
R, T = k.mo.pose(k.kf(*k.stand(1)[2:]))
Rf = R[1].astype(float) / 4096.0; Tf = T[1].astype(float)
m = k.mo.meshes[1]
for liste in (m.tv, m.qv):
    for i, e in enumerate(liste):
        p = Rf @ np.array(e[:3], float) + Tf
        if p[0] > CUT:
            p[1] += SENK; v2 = np.linalg.solve(Rf, p - Tf)
            liste[i] = type(e)([int(round(x)) for x in v2] + list(e[3:]))
nach = maske(2)
z = cv2.imread(os.path.join(F, 'ziel2_bild2.png'), cv2.IMREAD_GRAYSCALE)
print('Bild 2: Pixel vorher %d, Probe %d, verschieden %d, davon Ziel Figur %d' % (vor.sum(), nach.sum(), (vor ^ nach).sum(), ((vor ^ nach) & (z == 255)).sum()))
