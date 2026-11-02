#!/usr/bin/env python3
"""Kandidat B: Naht auf der Kuppelfront. Pfad aus dem Stapel Z1-Z4 (Bildpunkte auf die Kuppel), Rinne im Kuppelfeld, Lippen und Querwuelste als eigene Teile.
Deutung B: Stapel = senkrechtes Maul, frei gesetzt sind Lippenform, Rinnentiefe und die Rueckseite."""
import math, os, sys
import numpy as np, torch
from b_geom import *
import b_teile as T
import b_mesh as BM
from b_fit import GERAET, ablage_torch, abst_kanten, glatt_min

# Mittellinie im Bild 1: Spindel Z1, Band Z2, Kern Z3, Band Z4, Auslauf bis Bauchkante (FORMBLATT 2.2, 2.3)
PFAD_BILD = [(336, 216), (331, 224), (325, 232), (318, 243), (313, 255), (316, 266), (321, 279), (325, 292), (322, 304), (323, 317)]
BREITE = [34, 70, 118, 118, 70, 46, 42, 42, 46, 36]        # halbe Oeffnung in Einheiten, frei, Spindelbreite Z1 300-370 belegt
LIPPE_R = [52, 62, 74, 74, 62, 54, 52, 52, 54, 46]         # Lippenradius, frei


def treffer(F, x, y):
    e, d = ray_lokal(x, y)
    t = np.linspace(2500, 9000, 1600)
    p = e + t[:, None] * d
    s = F(p.astype(np.float32))
    i = int(np.argmax(s < 0))
    return p[i] if s[i] < 0 else None


def normale(F, p, h=25.0):
    q = np.array([p + [h, 0, 0], p - [h, 0, 0], p + [0, h, 0], p - [0, h, 0], p + [0, 0, h], p - [0, 0, h]], np.float32)
    s = F(q)
    n = np.array([s[0] - s[1], s[2] - s[3], s[4] - s[5]], float)
    return n / np.linalg.norm(n)


def naht(fit):
    F = BM.Feld(fit, {'rumpf_m'}, 30.0)
    P_ = np.array([treffer(F, x, y) for x, y in PFAD_BILD], float)
    # leicht glaetten
    Q = P_.copy()
    for _ in range(2):
        Q[1:-1] = 0.25 * Q[:-2] + 0.5 * Q[1:-1] + 0.25 * Q[2:]
    n = np.array([normale(F, p) for p in Q])
    t = np.gradient(Q, axis=0)
    t /= np.linalg.norm(t, axis=1, keepdims=True)
    b = np.cross(n, t)
    b /= np.linalg.norm(b, axis=1, keepdims=True)
    # b so ausrichten, dass +b nach rechts im Bild zeigt
    if (bild(Q[4] + 100 * b[4])[0] - bild(Q[4])[0]) < 0:
        b = -b
    return dict(P=Q, n=n, t=t, b=b, w=np.array(BREITE, float), r=np.array(LIPPE_R, float))


def treffer_rippe(F, a, c, hub, ra, rc):
    p, q = treffer(F, *a), treffer(F, *c)
    return p + normale(F, p) * hub, q + normale(F, q) * hub, ra, rc


def ergaenze(fit):
    """Fit um Lippen und Querwuelste erweitern; Rueckgabe neuer Fit, Rinnenfunktion fuer die Kuppel."""
    N = naht(fit)
    F = BM.Feld(fit, {'rumpf_m'}, 30.0)
    namen = list(fit['namen'])
    pos = [p for p in fit['pos']]
    rad = [r for r in fit['rad']]
    kanten = list(fit['kanten'])
    n = len(N['P'])
    for bn, sg in (('maul_l', -1.0), ('maul_r', 1.0)):
        prev = None
        for i in range(n):
            p = N['P'][i] + sg * N['b'][i] * (N['w'][i] + N['r'][i] * 0.55) + N['n'][i] * (N['r'][i] * 0.25)
            nm = '%s_%d' % (bn, i)
            namen.append(nm)
            pos.append(p)
            rad.append(N['r'][i])
            if prev:
                kanten.append((bn, prev, nm))
            prev = nm
    for bn, nm, a, c, hub, ra, rc in (('maul_l', 'z2', (303, 258), (336, 275), 28.0, 40.0, 46.0),
                                      ('maul_r', 'z3', (314, 289), (338, 295), 34.0, 62.0, 58.0),
                                      ('maul_r', 'z4', (303, 306), (336, 302), 30.0, 38.0, 42.0)):
        p, q, ra, rc = treffer_rippe(F, a, c, hub, ra, rc)
        namen += [nm + 'a', nm + 'b']
        pos += [p, q]
        rad += [ra, rc]
        kanten.append((bn, nm + 'a', nm + 'b'))
    neu = dict(fit)
    neu.update(namen=namen, pos=np.array(pos), rad=np.array(rad), kanten=kanten)
    J = np.array(fit['J'], float)
    m = N['P'].mean(0) - N['n'].mean(0) * 120
    J[T.IDX['maul_l']] = m - N['b'].mean(0) * 120
    J[T.IDX['maul_r']] = m + N['b'].mean(0) * 120
    neu['J'] = J
    neu['naht'] = N
    # Rinne
    A = ablage_torch(N['P'] - N['n'] * (N['w'][:, None] * 0.75))
    Ra = ablage_torch(N['w'] * 1.15)

    def rinne(q, d):
        a, b = A[:-1], A[1:]
        ra, rb = Ra[:-1], Ra[1:]
        g = abst_kanten(q, a, b, ra, rb).min(1).values
        k = 34.0
        return k * torch.log(torch.exp(d / k) + torch.exp(-g / k))    # glattes Maximum aus Kuppel und minus Rinne
    return neu, rinne
