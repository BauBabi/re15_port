#!/usr/bin/env python3
"""Kandidat B: Lage und Gier in Bild 2 am fertigen Netz suchen (Koordinatenabstieg ueber Umriss-Pruefgroessen)."""
import math, sys, json
import numpy as np, cv2
from scipy.ndimage import distance_transform_edt as EDT
from b_geom import *
import b_teile as T
import b_bau as BB
import b_snap as SN
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P


def bewerte(m, Z, kante, linien):
    w = Z == 255
    s = Z == 0
    fehl = int((w & ~m).sum())
    sw = int((s & m).sum())
    r = m & ~cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    d = EDT(~r)
    k = 0.0
    for (x, y), g in kante:
        e = d[int(y), int(x)]
        k += max(0.0, e - g * 0.7) ** 2
    l = 0.0
    for (a, b) in linien:
        n = max(abs(b[0] - a[0]), abs(b[1] - a[1])) + 1
        xs = np.linspace(a[0], b[0], n).astype(int)
        ys = np.linspace(a[1], b[1], n).astype(int)
        l += (~m[ys, xs]).sum()
    return fehl + 2.5 * sw + 40.0 * k + 25.0 * l, dict(weiss_fehlt=fehl, schwarz=sw, kante=round(k, 1), linien=int(l))


def suche(teile, R, Tt, J, lage0, g0, schritte=(120, 60, 30, 15), g_schritte=(24, 12, 6, 3), w2=None, dy=0.0, gelenke=()):
    """Koordinatenabstieg ueber Lage, Gier und optional Winkel einzelner Knochen (Grad-Schritte 2, 1). Rueckgabe lage, gier[, w2]."""
    G = json.load(open(FORM + '/pruefung/grenzen.json', encoding='utf-8'))
    Z = cv2.imread(FORM + '/ziel_bild2.png', 0)
    kante = [((p[0], p[1]), G['b2.kante_%d' % (i + 1)]['grenze']) for i, p in enumerate(P.teil_b2('R')['kante_belegt'])]
    linien = [((222 + 4, 292), (298 - 4, 292)), ((232 + 4, 347), (285 - 4, 347)), ((340, 338), (364, 338)), ((408, 230), (408, 258)), ((413, 228), (413, 258))]
    lage = np.array(lage0, float)
    g = float(g0)
    w = None if w2 is None else np.array(w2, float)

    def wert(l, gg, ww=None):
        if ww is None:
            r, t = R, Tt
        else:
            r, t = BB.fk(ww, J, T.ELTERN, dy)
            u = max(((r[st['bone']] @ (st['V'] - J[st['bone']]).T).T + t[st['bone']])[:, 1].max() for st in teile)
            t = [q + [0, -u, 0] for q in t]
        s = SN.Sicht(2, K2, l, gg, r, t, J)
        return bewerte(s.maske(teile), Z, kante, linien)
    best, info = wert(lage, g, w)
    print('Start', round(best), info)
    for dl, dg in zip(schritte, g_schritte):
        besser = True
        while besser:
            besser = False
            for d in ((dl, 0), (-dl, 0), (0, dl), (0, -dl)):
                l = lage + [d[0], 0, d[1]]
                v, i = wert(l, g, w)
                if v < best:
                    best, info, lage, besser = v, i, l, True
            for d in (dg, -dg):
                v, i = wert(lage, g + d, w)
                if v < best:
                    best, info, g, besser = v, i, g + d, True
    if w is not None:
        for sch in (math.radians(3), math.radians(1.5)):
            besser = True
            while besser:
                besser = False
                for b in gelenke:
                    for ax in range(3):
                        for sg in (1, -1):
                            ww = w.copy()
                            ww[b, ax] += sg * sch
                            v, i = wert(lage, g, ww)
                            if v < best - 1e-6:
                                best, info, w, besser = v, i, ww, True
    print('Ende', round(best), info, 'Lage', np.round(lage).tolist(), 'Gier', g)
    return (lage, g) if w2 is None else (lage, g, w)
