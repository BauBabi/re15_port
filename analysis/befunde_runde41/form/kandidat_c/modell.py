# -*- coding: utf-8 -*-
"""Kandidat C: Geometrie-Grundbausteine. Stand 1 = Pose 1 ist der Bindezustand, alle Ecken im Aktorraum von Pose 1."""
from netz import *

L1 = (-18514, 0, -18726); G1 = 2145       # FORMBLATT 3.1, Ende N vorn
L2 = (2625, 0, -23625); G2 = 1166         # FORMBLATT 3.2 Sicht Flanke, Achse a = 77,5 Grad (innerhalb 60-100), Ende N vorn. Lage aus der Probe, Gier so, dass Rumpf rund wird
S1 = Stand(1, L1, G1); S2 = Stand(2, L2, G2)

FB1 = {t['id']: t for t in FB['teile_bild1']}
FB2 = {t['id']: t for t in FB['teile_bild2']}


def poly(pts):
    m = np.zeros((480, 640), np.uint8)
    cv2.fillPoly(m, [np.round(np.array(pts, float)).astype(np.int32)], 1)
    return m.astype(bool)


def weit(m, r):
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * r + 1, 2 * r + 1))
    return cv2.dilate(m.astype(np.uint8), k).astype(bool)


def schlauch(st, n_ring, phi0=0.0, kappe=(0.5, 0.5)):
    """st = Liste dict(c=3D, a=Halbbreite quer, b=Halbbreite tief, e1=Richtung von a). Ringe senkrecht zur Achse, Enden mit Polpunkt.
    Rueckgabe V, F, ringe (Eckenindizes je Ring), (pol0, pol1)."""
    C = np.array([s['c'] for s in st], float)
    n = len(C)
    Tg = np.zeros_like(C)
    Tg[1:-1] = C[2:] - C[:-2]
    Tg[0] = C[1] - C[0]
    Tg[-1] = C[-1] - C[-2]
    Tg /= np.linalg.norm(Tg, axis=1, keepdims=True)
    V = []
    ringe = []
    for k, s in enumerate(st):
        e1 = np.array(s['e1'], float)
        e1 = e1 - (e1 @ Tg[k]) * Tg[k]
        e1 /= np.linalg.norm(e1)
        e2 = np.cross(Tg[k], e1)
        ex = s.get('ex', 2.0)
        r = []
        for i in range(n_ring):
            ph = phi0 + 2 * math.pi * i / n_ring
            c, si = math.cos(ph), math.sin(ph)
            cc = math.copysign(abs(c) ** (2 / ex), c)
            ss = math.copysign(abs(si) ** (2 / ex), si)
            r.append(len(V))
            V.append(C[k] + e1 * s['a'] * cc + e2 * s['b'] * ss)
        ringe.append(r)
    p0 = len(V)
    V.append(C[0] - Tg[0] * kappe[0] * min(st[0]['a'], st[0]['b']))
    p1 = len(V)
    V.append(C[-1] + Tg[-1] * kappe[1] * min(st[-1]['a'], st[-1]['b']))
    F = []
    for i in range(n_ring):
        j = (i + 1) % n_ring
        F.append((p0, ringe[0][j], ringe[0][i]))
        F.append((p1, ringe[-1][i], ringe[-1][j]))
        for k in range(n - 1):
            F.append((ringe[k][i], ringe[k][j], ringe[k + 1][j], ringe[k + 1][i]))
    return np.array(V, float), F, ringe, (p0, p1)


def stationen_bild(pts, tiefen, maske=None, breiten=None, k=1.0, s=S1, weite_max=80, ex=2.0):
    """pts = Achse im Bild, tiefen je Station, Breite aus der Maske quer zur Achse oder aus breiten (px).
    Mitte = Mitte der Randpunkte auf der Tiefe, a = halber Abstand, b = k * a."""
    pts = np.array(pts, float)
    aus = []
    n = len(pts)
    for i in range(n):
        t = pts[min(i + 1, n - 1)] - pts[max(i - 1, 0)]
        t /= np.linalg.norm(t)
        q = np.array([t[1], -t[0]])
        if q[0] < 0:
            q = -q
        p = pts[i]
        if breiten is not None and breiten[i] is not None:
            w = breiten[i]
            if isinstance(w, (tuple, list)):
                a0, a1 = p - q * w[0], p + q * w[1]
            else:
                a0, a1 = p - q * w, p + q * w
        else:
            grenzen = []
            for sgn in (-1, 1):
                r = 0.0
                while r < weite_max:
                    x = p + q * sgn * (r + 1)
                    if not maske[int(round(x[1])), int(round(x[0]))]:
                        break
                    r += 1
                grenzen.append(p + q * sgn * r)
            a0, a1 = grenzen
            if np.linalg.norm(a1 - a0) < 3.0:
                a0, a1 = p - q * 1.5, p + q * 1.5
        P = s.auf_tiefe(np.array([a0, a1]), tiefen[i])
        c = 0.5 * (P[0] + P[1])
        a = 0.5 * np.linalg.norm(P[1] - P[0])
        kk = k[i] if isinstance(k, (list, tuple)) else k
        aus.append(dict(c=c, a=max(a, 8.0), b=max(a, 8.0) * kk, e1=(P[1] - P[0]) / (2 * a + 1e-6), ex=ex))
    return aus
