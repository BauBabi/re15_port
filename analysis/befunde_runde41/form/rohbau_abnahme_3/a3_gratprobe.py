#!/usr/bin/env python3
"""Abnahme 3, Probe nur im Speicher: s1-Ecken vor Modell-x CUT um SENK Einheiten nach unten (in den Rumpf), Bild 1 Umriss gegen ziel2 und tail-Umriss.
Aendert keine Datei. Kein RE-Wert. Aufruf: python a3_gratprobe.py CUT SENK"""
import os, sys
import numpy as np, cv2
H = os.path.dirname(os.path.abspath(__file__)); F = os.path.dirname(H)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P, form_buehne as FB
CUT = int(sys.argv[1]); SENK = int(sys.argv[2])
k = P.Kandidat(os.path.join(F, 'rohbau_final', 'form.json'))
z = cv2.imread(os.path.join(F, 'ziel2_bild1.png'), cv2.IMREAD_GRAYSCALE)
mt = cv2.imread(os.path.join(F, 'nutzer_teile', 'maske_tail.png'), cv2.IMREAD_GRAYSCALE) > 127
lage, gier, clip, bild = k.stand(1); kam = P.kamera(1); kf = k.kf(clip, bild)
w = FB.Textur(k.tim); w.lut = w.lut.copy(); w.lut[..., :3] = 255
def maske():
    return FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None), w, np.zeros((480, 640, 3), np.uint8), kam.abbildung)
vor = maske()
R, T = k.mo.pose(kf)
Rf = R[1].astype(float) / 4096.0; Tf = T[1].astype(float)
m = k.mo.meshes[1]
n_mod = 0
for liste in (m.tv, m.qv):
    for i, e in enumerate(liste):
        v = np.array(e[:3], float); p = Rf @ v + Tf
        if p[0] > CUT:
            p[1] += SENK; n_mod += 1
            v2 = np.linalg.solve(Rf, p - Tf)
            liste[i] = type(e)([int(round(v2[0])), int(round(v2[1])), int(round(v2[2]))] + list(e[3:]))
nach = maske()
def kenn(a):
    iou = (a & (z == 255)).sum() / max(((a | (z == 255)) & ((z == 255) | (z == 0))).sum(), 1)
    return 'IoU %.4f, Weiss gedeckt %.4f, Schwarz getroffen %d, tail-Umriss gedeckt %.4f' % (iou, (a & (z == 255)).sum() / (z == 255).sum(), (a & (z == 0)).sum(), (a & mt).sum() / mt.sum())
out = ['CUT %d SENK %d, veraenderte Ecken %d' % (CUT, SENK, n_mod), 'vorher  ' + kenn(vor), 'Probe   ' + kenn(nach)]
weg = vor & ~nach
ys, xs = np.nonzero(weg)
out.append('verlorene Pixel %d, davon Ziel Figur %d, im tail-Umriss %d%s' % (weg.sum(), (weg & (z == 255)).sum(), (weg & mt).sum(), (', Lage x %d-%d y %d-%d' % (xs.min(), xs.max(), ys.min(), ys.max())) if len(ys) else ''))
for x in (290, 300, 305, 311, 316, 322, 330, 340):
    a = np.nonzero(vor[:, x])[0]; b = np.nonzero(nach[:, x])[0]; c = np.nonzero(z[:, x] == 255)[0]
    out.append('  x %d: Oberkante vorher %s, Probe %s, Ziel %s' % (x, a.min() if len(a) else '-', b.min() if len(b) else '-', c.min() if len(c) else '-'))
b = np.zeros((480, 640, 3), np.uint8); b[nach] = (110, 110, 110); b[weg] = (0, 0, 255); b[(z == 255) & ~nach] = (0, 200, 0)
cv2.imwrite(os.path.join(H, 'bild', 'gratprobe_%d_%d.png' % (CUT, SENK)), cv2.resize(b[150:330, 220:400], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST))
txt = '\n'.join(out); print(txt)
open(os.path.join(H, 'mess', 'gratprobe.txt'), 'a').write(txt + '\n\n')
