#!/usr/bin/env python3
"""Abnahme 1: Machbarkeit nur im Speicher, FORM.EMD bleibt unberuehrt. Kopf in Pose 1 tiefer: schliesst das Achselfenster? Dazu Rueckenstufe und Fingermasse. Kein RE-Wert."""
import os, sys, math
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose
from abn_kamton import welt, bildpunkte
LUM = np.array([.299, .587, .114])
k = P.Kandidat(K)
n = netz(k)
aus = []
def sag(t=''):
    print(t); aus.append(t)

# Fenster wie in abn_fenster: Original dunkel, Render hell, in der Zone
o = P.original(1).astype(np.float32)
Y = cv2.GaussianBlur(o @ LUM, (0, 0), 1.2)
gef = cv2.cvtColor(cv2.imread(os.path.join(HIER, 'bild', 'gefiltert_bild1.png')), cv2.COLOR_BGR2RGB).astype(np.float32)
Yg = cv2.GaussianBlur(gef @ LUM, (0, 0), 1.2)
zone = np.zeros(Y.shape, bool); zone[280:326, 336:386] = True
fenster = (Y < 70) & (Yg > 130) & zone
hell = (Y > 110) & zone                 # hell im Original, dort darf nichts stehen
sag('Fenster %d px, helles Original in der Zone %d px' % (fenster.sum(), hell.sum()))
lage, gier, clip, bild = k.stand(1)
kam = P.kamera(1)
V, F, B = pose(k, n, clip, bild)
R, T = k.mo.pose(k.kf(clip, bild))
J = np.array(T[21], float)
def maske(Vn, nur=None):
    W = welt(Vn, lage, gier)
    px, z = bildpunkte(W, kam)
    M = np.zeros((480, 640), np.uint8)
    for f in (F if nur is None else F[np.isin(B, nur)]):
        cv2.fillConvexPoly(M, np.round(px[f]).astype(np.int32), 1)
    return M > 0
m0 = maske(V)
sag('Pose 1 unveraendert: Fenster gedeckt %.2f, helles Original gedeckt %.2f' % ((m0 & fenster).sum() / fenster.sum(), (m0 & hell).sum() / hell.sum()))
idx = np.unique(F[np.isin(B, (21, 22, 23))])
for d in (15, 25, 35, 45, 55, 65):
    a = math.radians(-d)                # Rotation um die Querachse Z im Aktorraum; Kopf zeigt nach +X, Y unten: Vorzeichen so, dass +X nach +Y geht
    c, s = math.cos(a), math.sin(a)
    for vz in (1,):
        Rz = np.array([[c, s, 0], [-s, c, 0], [0, 0, 1]])
        Vn = V.copy()
        Vn[idx] = (V[idx] - J) @ Rz.T + J
        hoehe = -Vn[idx][:, 1].max()
        m = maske(Vn)
        sag('Hals %2d Grad tiefer: Fenster gedeckt %.2f, helles Original gedeckt %.2f, Kopf tiefster Punkt Hoehe %.0f' % (d, (m & fenster).sum() / fenster.sum(), (m & hell).sum() / hell.sum(), hoehe))
        if d in (35, 45):
            bg = cv2.imread(os.path.join(HIER, 'bild', 'sauber_bild1.png'))
            x = o[..., ::-1].astype(np.uint8).copy()
            ko, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            x2 = cv2.resize(x[170:400, 300:450], None, fx=4, fy=4, interpolation=cv2.INTER_CUBIC)
            ko2 = [((c_.reshape(-1, 2) - [300, 170]) * 4).astype(np.int32) for c_ in ko]
            cv2.polylines(x2, ko2, True, (255, 255, 255), 1, cv2.LINE_AA)
            cv2.imwrite(os.path.join(HIER, 'bild', 'probe_kopf_%d.png' % d), x2)

# Rueckenstufe: obere Kontur im Seitenblick je Pose, groesster Sprung auf kurzem Weg
for clip_, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    V2, F2, B2 = pose(k, n, clip_)
    q = V2[:, [0, 1]]
    x0, x1 = q[:, 0].min(), q[:, 0].max()
    sp = np.arange(int(x0), int(x1), 40)
    top = np.full(len(sp), np.nan)
    t = q[F2]
    for i, xx in enumerate(sp):
        best = None
        lo = t[:, :, 0].min(1) <= xx
        hi = t[:, :, 0].max(1) >= xx
        for tri in t[lo & hi]:
            for a, b in ((0, 1), (1, 2), (2, 0)):
                xa, ya = tri[a]; xb, yb = tri[b]
                if (xa - xx) * (xb - xx) <= 0 and xa != xb:
                    y = ya + (yb - ya) * (xx - xa) / (xb - xa)
                    best = y if best is None else min(best, y)
        if best is not None:
            top[i] = -best
    sprung = np.abs(top[3:] - top[:-3])
    w = np.where(sprung > 0)[0]
    i = int(np.nanargmax(sprung))
    sag('%s Seitenkontur oben: groesster Sprung %.0f Einheiten auf 120 bei x %d (Hoehe %.0f auf %.0f)' % (nm, sprung[i], sp[i], top[i], top[i + 3]))

# Finger: Laenge und groesste Breite je Mesh in Bindlage
for b, t in ((19, 'Finger r 19'), (20, 'Finger r 20'), (29, 'Finger r 29 aussen'), (31, 'Daumen r 31'), (14, 'Finger l 14'), (15, 'Finger l 15'), (28, 'Finger l 28'), (30, 'Daumen l 30')):
    v = n[b][0]
    c = v - v.mean(0)
    u, s, vt = np.linalg.svd(c, full_matrices=False)
    p = c @ vt.T
    sag('%-20s Laenge %5.0f  Breite %5.0f  Dicke %5.0f' % (t, np.ptp(p[:, 0]), np.ptp(p[:, 1]), np.ptp(p[:, 2])))
with open(os.path.join(HIER, 'mess', 'probe.txt'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(aus))

# Rueckenstufe nur ueber dem Rumpf: Knochen 0, 7, 10 allein, Seitenblick, obere Kontur in Schritten von 40 Einheiten
aus2 = []
for clip_, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    V2, F2, B2 = pose(k, n, clip_)
    m = np.isin(B2, (0, 7, 10))
    q = V2[:, [0, 1]]
    t = q[F2[m]]
    x0, x1 = t[:, :, 0].min(), t[:, :, 0].max()
    sp = np.arange(int(x0) + 40, int(x1) - 40, 40)
    top = []
    for xx in sp:
        ys = []
        for tri in t[(t[:, :, 0].min(1) <= xx) & (t[:, :, 0].max(1) >= xx)]:
            for a, b in ((0, 1), (1, 2), (2, 0)):
                xa, ya = tri[a]; xb, yb = tri[b]
                if (xa - xx) * (xb - xx) <= 0 and xa != xb:
                    ys.append(ya + (yb - ya) * (xx - xa) / (xb - xa))
        top.append(-min(ys))
    top = np.array(top)
    d = np.diff(top)
    i = int(np.argmax(np.abs(d)))
    # flaches Dach: laengste Strecke mit Steigung unter 10 %
    fl = np.abs(d) < 4
    best = cur = 0; ende = 0
    for j, f in enumerate(fl):
        cur = cur + 1 if f else 0
        if cur > best:
            best, ende = cur, j
    t_ = '%s Rumpfdach: Spanne %d..%d, Hoehe %d..%d, groesster Sprung %.0f auf 40 bei x %d, flaches Dach %d Einheiten ab x %d auf Hoehe %.0f' % (nm, sp[0], sp[-1], top.min(), top.max(), abs(d[i]), sp[i], best * 40, sp[ende - best + 1], top[ende])
    print(t_); aus2.append(t_)
with open(os.path.join(HIER, 'mess', 'probe.txt'), 'a', encoding='utf-8') as f:
    f.write('\n' + '\n'.join(aus2))
