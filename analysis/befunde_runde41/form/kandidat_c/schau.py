# -*- coding: utf-8 -*-
"""Schnellansicht: Netze aus mehreren Richtungen, schattiert, Maler-Sortierung."""
from kern import *


def ansicht(teile, az, el, gross=420, rad=None, mitte=None):
    """teile = Liste (V, F, farbe). az wie form_pruefung ANSICHTEN, el nach oben."""
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    alle = np.vstack([t[0] for t in teile])
    if mitte is None:
        mitte = (alle.max(0) + alle.min(0)) / 2
    if rad is None:
        rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.47 / rad
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    tris = []
    for V, F, fb in teile:
        q = (V - mitte) @ A.T
        for f in F:
            p = q[list(f)]
            n = np.cross(V[f[1]] - V[f[0]], V[f[2]] - V[f[0]])
            ln = np.linalg.norm(n)
            if ln < 1e-6:
                continue
            n = n / ln
            tris.append((p[:, 2].mean(), p, n, fb))
    tris.sort(key=lambda t: -t[0])
    img = np.full((gross, gross, 3), 70, np.uint8)
    for z, p, n, fb in tris:
        h = 0.35 + 0.65 * abs(float(n @ L))
        c = tuple(int(min(255, x * h)) for x in fb)
        pts = np.round(np.c_[gross / 2 + p[:, 0] * s, gross / 2 + p[:, 1] * s] * 8).astype(np.int32)
        cv2.fillConvexPoly(img, pts, c, lineType=cv2.LINE_AA, shift=3)
    return img


def blatt(teile, pfad, gross=420):
    alle = np.vstack([t[0] for t in teile])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    kacheln = []
    for az, el, t in ((90, 0, 'vorn +X'), (-90, 0, 'hinten -X'), (180, 0, 'Seite +Z'), (0, 0, 'Seite -Z'), (45, 22, '3/4'), (135, 22, '3/4'), (-45, 22, '3/4'), (0, 89.9, 'oben')):
        k = ansicht(teile, az, el, gross, rad, mitte)
        cv2.putText(k, t + ' az%d' % az, (6, 16), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 0), 1, cv2.LINE_AA)
        kacheln.append(k)
    z = [np.hstack(kacheln[i:i + 4]) for i in range(0, 8, 4)]
    cv2.imwrite(pfad, np.vstack(z)[:, :, ::-1])
