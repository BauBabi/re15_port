"""Einzelstelle: Linien aus Bild 2 auf die Kuppel legen und in Bild 1 ansehen, je Gier. Rumpf starr."""
import sys, json
sys.path.insert(0, '.')
from b_geom import *
import b_mesh as BM
import b_teile as T
import numpy as np, cv2
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
fit = BM.lade('zw/fit2.pt')
F = BM.Feld(fit, {'rumpf_m'}, 70.0)
c = np.array(T.KUPPEL_MITTE, float)
l2 = np.array(fit['lage2'])
ORIG1 = P.original(1)


def treffer_b2(x, y, g, lage):
    e, d = FK.strahl(K2, np.array([x, y], float))
    t = np.linspace(2500, 9000, 900)
    w = e + t[:, None] * d
    p = (Y(g).T @ (w - lage).T).T       # Modellraum, Rumpf ohne Eigendrehung
    s = F(p.astype(np.float32))
    i = int(np.argmax(s < 0))
    return p[i] if s[i] < 0 else None


st = P.stellen()
Q = [s for s in st if s['bild'] == 2 and s['id'] in ('Q1', 'Q2', 'Q3', 'Q4', 'Q5', 'P')]
farbe = {'Q1': (0, 255, 255), 'Q2': (255, 128, 0), 'Q3': (0, 255, 0), 'Q4': (255, 0, 255), 'Q5': (0, 128, 255), 'P': (255, 255, 255)}
tafeln = []
for g in (1196, 1025, 850, 2276, 2500):
    lage = l2 if g < 2000 else np.array([2361.0, 0, -23418.0])
    a = cv2.cvtColor(ORIG1, cv2.COLOR_RGB2BGR)
    a = np.clip(a.astype(np.float32) * 1.3, 0, 255).astype(np.uint8)
    z = []
    Yb = (ORIG1.astype(np.float32) @ P.YK)
    for s in Q:
        m = np.zeros((480, 640), np.uint8)
        for q in s['polygone']:
            cv2.fillPoly(m, [np.round(np.array(q, float)).astype(np.int32)], 1)
        ys, xs = np.nonzero(m)
        ys, xs = ys[::5], xs[::5]
        tr = 0
        Yv = []
        for x, y in zip(xs, ys):
            p = treffer_b2(x, y, g, lage)
            if p is None:
                continue
            w = welt(p)
            b = FK.projiziere(K1, w)
            # sichtbar in Bild 1, wenn Normale zur Kamera zeigt
            n = (p - c) / np.linalg.norm(p - c)
            au = lokal(K1['auge'])
            if n @ (au - p) <= 0:
                continue
            tr += 1
            X, Y_ = int(b[0]), int(b[1])
            if 0 <= X < 640 and 0 <= Y_ < 480:
                cv2.circle(a, (X, Y_), 1, farbe[s['id']], -1)
                Yv.append(Yb[Y_, X])
        z.append('%s %d/%d Y%.0f' % (s['id'], tr, len(xs), np.mean(Yv) if Yv else -1))
    print('Gier', g, ' | '.join(z))
    cv2.putText(a, 'G%d' % g, (190, 186), cv2.FONT_HERSHEY_SIMPLEX, .5, (255, 255, 255), 1)
    tafeln.append(a[180:330, 180:450])
m = np.vstack([np.hstack(tafeln[:3]), np.hstack(tafeln[3:] + [np.zeros_like(tafeln[0])])])
cv2.imwrite('zw/kreuz.png', m)
print(m.shape)
