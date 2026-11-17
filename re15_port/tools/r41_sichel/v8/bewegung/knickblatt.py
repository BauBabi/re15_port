"""Knick-Blatt: sichtbare Gelenke (beide Nachbarglieder mit Querschnitt) mit mehr als 25 Grad Knick im Lauf, je h (jedes 2. Bild
h16-h110), Ausschnitt +-36 Framedump-Punkte um das Gelenk (= +-24 Videopunkte), 3x vergroessert, mit dem gleichen Ausschnitt vorher.

    python knickblatt.py <vorher> <nachher> <ausgabe.png>
"""
import sys, os, math
import numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import messen as Ms
import b2
vl, nl, aus = sys.argv[1], sys.argv[2], sys.argv[3]
PN = Ms.pose_log(os.path.join(nl, 'pose.log'))
R0 = 36
kacheln = []
SCHRITT = int(sys.argv[4]) if len(sys.argv) > 4 else 2
for h in range(16, 111, SCHRITT):
    F = h + 243
    if F not in PN:
        continue
    p = PN[F]
    R, T = p['R'].astype(float), p['T'].astype(float)
    nn = np.linalg.norm(R, axis=1)
    sicht = [nn[b][0] > 1 and nn[b][2] > 1 for b in range(23)]
    pose = dict(clip=p['clip'], bild=p['bild'], gier=p['gier'], x=p['x'], y=p['y'], z=p['z'])
    for j in range(1, 7):
        b0, b1 = 15 + j, 16 + j
        if not (sicht[b0] and sicht[b1]):
            continue
        a0 = R[b0][:, 1] / nn[b0][1]; a1 = R[b1][:, 1] / nn[b1][1]
        w = math.degrees(math.acos(max(-1.0, min(1.0, float(a0 @ a1)))))
        if w <= 25.0:
            continue
        q = b2.projiziere(pose, T[b1][None])[0]
        X, Y = int(round((q[0] - 1.97) * 1.5)), int(round((q[1] - 1.3) * 3.0 / 2.079))
        reihe = []
        for name, lauf in (('vorher', vl), ('nachher', nl)):
            im = cv2.imread(os.path.join(lauf, 'fd', 'f%06d.ppm' % F))
            c = np.zeros((2 * R0, 2 * R0, 3), np.uint8)
            if im is not None:
                x0, y0 = max(X - R0, 0), max(Y - R0, 0)
                s = im[y0:Y + R0, x0:X + R0]
                c[y0 - (Y - R0):y0 - (Y - R0) + s.shape[0], x0 - (X - R0):x0 - (X - R0) + s.shape[1]] = s
            c = cv2.resize(c, None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
            cv2.putText(c, '%s h%d Gelenk %d %.0f Grad' % (name, h, j, w), (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 0), 3)
            cv2.putText(c, '%s h%d Gelenk %d %.0f Grad' % (name, h, j, w), (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 255, 255), 1)
            reihe.append(c)
        kacheln.append(np.hstack([reihe[0], np.full((2 * R0 * 3, 4, 3), 60, np.uint8), reihe[1]]))
print(len(kacheln), 'Gelenke > 25 Grad sichtbar')
if len(sys.argv) > 5:
    kacheln = kacheln[:int(sys.argv[5])]
sp = 3
leer = np.zeros_like(kacheln[0])
while len(kacheln) % sp:
    kacheln.append(leer)
reihen = [np.hstack([np.hstack([k, np.full((k.shape[0], 10, 3), 20, np.uint8)]) for k in kacheln[i:i + sp]]) for i in range(0, len(kacheln), sp)]
im = np.vstack(reihen)
kopf = np.zeros((32, im.shape[1], 3), np.uint8)
cv2.putText(kopf, 'Messbild - kein Ergebnis | sichtbare Gelenke > 25 Grad, nachher (rechts) und gleicher Ausschnitt vorher (links), Framedump 3x', (8, 22),
            cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 255, 255), 1)
cv2.imwrite(aus, np.vstack([kopf, im]))
print(aus, im.shape)
