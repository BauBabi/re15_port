#!/usr/bin/env python3
"""Kandidat B: letzte Pixel. Liest den echten Pruefbericht und zieht an den Stellen, die fehlen, die Umrissecken gezielt nach."""
import json, math, os, sys
import numpy as np, cv2
from scipy.ndimage import distance_transform_edt as EDT
from b_geom import *
import b_teile as T
import b_snap as SN
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P


def ziehe(teile, s, punkt, d_img, sigma=70.0, radius=12.0, nur=None):
    """Umrissecken nahe punkt (Bild) um d_img verschieben, weich ueber das Teil."""
    q0 = np.array(punkt, float)
    bester = None
    for st in teile:
        if nur and st['name'] not in nur:
            continue
        kon, w = s.konturecken(st)
        q = s.proj(w)
        d = np.linalg.norm(q - q0, axis=1)
        wahl = np.nonzero(kon & (d < radius))[0]
        if not len(wahl):
            continue
        if bester is None or d[wahl].min() < bester[0]:
            bester = (d[wahl].min(), st, wahl, w)
    if bester is None:
        return False
    _, st, wahl, w = bester
    tiefe = FK.tiefe(s.kam, w[wahl])
    r, u = s.kam['R'][0], s.kam['R'][1]
    dw = (d_img[0] * r[None] * (tiefe / s.f[0])[:, None]) + (d_img[1] * u[None] * (tiefe / s.f[1])[:, None])
    dl = (dw @ s.Yg) @ s.R[st['bone']]
    P0 = st['V'][wahl]
    d2 = ((st['V'][:, None, :] - P0[None]) ** 2).sum(-1)
    G = np.exp(-d2 / (2 * sigma ** 2))
    nen = G.sum(1)
    feld = (G @ dl) / np.maximum(nen, 1e-6)[:, None] * np.minimum(nen, 1.0)[:, None]
    st['V'] += feld
    return True


def runde(teile, sichten, aus, masken=None):
    """Einmal alle Fehlstellen aus bericht.json bedienen. Rueckgabe: Zahl der Stellen."""
    R = json.load(open(os.path.join(aus, 'bericht.json'), encoding='utf-8'))
    n_ = 0
    for n in (1, 2):
        B = R['bild%d' % n]
        s = sichten[n]
        for x in B['punkte']:
            if not x['zaehlt'] or x['ok'] or x['abstand'] is None:
                continue
            a, d = x['art'], x['abstand']
            sx, sy = x['soll']
            if a.startswith('mat'):
                continue
            nur = ['kuppel'] if n == 2 else None
            if 'zeile' in a:
                gr = x['grenze']
                ist = x['ist']
                ziehe(teile, s, (ist, sy), (-d * 1.1 + (0.6 if d > 0 else -0.6), 0.0), nur=nur)
            elif 'spalte' in a:
                ist = x['ist']
                ziehe(teile, s, (sx, ist), (0.0, -d * 1.1 + (0.6 if d > 0 else -0.6)), nur=nur)
            else:
                m = s.maske(teile)
                r = m & ~cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
                ys, xs = np.nonzero(r)
                i = int(np.argmin((xs - sx) ** 2 + (ys - sy) ** 2))
                c = np.array([xs[i], ys[i]], float)
                v = np.array([sx, sy], float) - c
                ziehe(teile, s, c, v * 1.15, nur=nur)
            n_ += 1
        for x in B['flaechen']:
            if not x['zaehlt'] or x['ok']:
                continue
            if 'treffer' in x and n == 1:
                z = cv2.imread(FORM + '/ziel_bild1.png', 0) == 0
                inn = EDT(z)
                sg = cv2.GaussianBlur(inn.astype(np.float32), (0, 0), 1.5)
                gx = cv2.Sobel(sg, cv2.CV_32F, 1, 0, ksize=3) / 8.0
                gy = cv2.Sobel(sg, cv2.CV_32F, 0, 1, ksize=3) / 8.0
                zone = P.poly_maske(P.zone_b1(x['id']))
                ka = P.Kandidat(os.path.join(aus, 'kandidat.json'))
                _s, _a, m_, _k = ka.rendern(1)
                hit = m_ & zone & z
                kk, lab, st_, _c = cv2.connectedComponentsWithStats(hit.astype(np.uint8), connectivity=8)
                liste = [[int(st_[i, 4]), int(st_[i, 0]), int(st_[i, 1]), int(st_[i, 0] + st_[i, 2] - 1), int(st_[i, 1] + st_[i, 3] - 1)] for i in range(1, kk)]
                for px, x0, y0, x1, y1 in liste:
                    c = np.array([(x0 + x1) / 2.0, (y0 + y1) / 2.0])
                    xi, yi = int(c[0]), int(c[1])
                    g = np.array([gx[yi, xi], gy[yi, xi]], float)
                    g /= max(np.linalg.norm(g), 1e-3)
                    tief = min(float(inn[y0:y1 + 1, x0:x1 + 1].max()), 3.0)
                    if 303 <= c[0] <= 318 and 345 <= c[1] <= 356:
                        # Spitze von L2 sitzt in der Taille: Oberkante C3 absenken, die Kante bei y=346 bleibt stehen
                        for t_ in ('k18', 'k19'):
                            ziehe(teile, s, c + (1.0, 3.0), (0.5, 3.5), sigma=34.0, radius=22.0, nur=[t_])
                    else:
                        ziehe(teile, s, c, -g * (tief + 1.5), sigma=45.0, radius=10.0)
                    n_ += 1
                continue
            if x['id'] == 'V6':
                m = s.maske(teile)
                r = m & ~cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
                ys, xs = np.nonzero(r)
                i = int(np.argmin((xs - 411) ** 2 + (ys - 206) ** 2))
                c = np.array([xs[i], ys[i]], float)
                ziehe(teile, s, c, (np.array([411.0, 206.0]) - c) * 1.2 + (1.5, -1.0), nur=['kuppel'])
                n_ += 1
    return n_
