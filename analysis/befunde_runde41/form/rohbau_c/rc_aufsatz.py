#!/usr/bin/env python3
"""Rohbau C, Aufsaetze: Zaehne und Zehen als spitze, kantige Kegel, je Knochen fest, aufgesetzt auf die fertigen Ringe. Keine Passung, alles [f]."""
import numpy as np

from rc_kern import Teil, ringecken


def _basis(d):
    d = np.asarray(d, float)
    d = d / np.linalg.norm(d)
    h = np.array([0, 1.0, 0]) if abs(d[1]) < .8 else np.array([1.0, 0, 0])
    a = h - (h @ d) * d
    a /= np.linalg.norm(a)
    return d, a, np.cross(d, a)


def kegel(name, kn, p, d, r, lang, n=4, feld='zahn', wurzel=None):
    """Spitzer Kegel, Boden offen (steckt im Koerper). wurzel = (Abstand, Faktor) fuer einen zweiten Ring."""
    dd, a, b = _basis(d)
    if wurzel:
        t = [0.0, wurzel[0]]
        rr = np.array([[r] * n, [r * wurzel[1]] * n], float)
    else:
        t = [0.0]
        rr = np.array([[r] * n], float)
    return Teil(name, kn, p, dd, a, b, t, np.zeros((len(t), 2)), rr, spitze0=0.0, spitze1=lang, feld=feld, offen0=True)


def ringpunkt(p, lr, c, k, j):
    """Ecke j auf Ring k eines Teils, Knochenraum."""
    r = np.exp(np.asarray(lr)[k, j] + p.relief[k, j])
    cs, sn = ringecken(p.n)
    m = p.o + p.t[k] * p.d + c[k][0] * p.ea + c[k][1] * p.eb
    return m + r * cs[j] * p.ea + r * sn[j] * p.eb


def mit_aufsaetzen(g, teile, stand, hauer=None):
    idx = {}
    for i, p in enumerate(teile):
        idx.setdefault(p.name, i)
    neu = list(teile)
    lr, c = list(stand['lr']), list(stand['c'])

    def dazu(t):
        neu.append(t)
        lr.append(np.log(np.maximum(t.r, 1.0)).tolist())
        c.append(t.c.tolist())
    # Oberkiefer: wenige, grobe Zaehne an der Kante der Schnauze, vorn zwei Fangzaehne. Unterkiefer: zwei Hauer, dazwischen Reihe.
    ko = idx['kopf']
    p = teile[ko]
    for k, jl, l, r in ((9, (1, 9), 250, 95), (8, (1, 9), 400, 120), (7, (1, 9), 240, 95), (6, (1, 9), 210, 90), (9, (0,), 180, 80)):
        for j in jl:
            q = ringpunkt(p, stand['lr'][ko], stand['c'][ko], k, j)
            dazu(kegel('zahn_o', p.knochen, q + np.array([0, -40, 0]), (0.1, 1.0, 0.10 * (1 if j <= 4 else -1)), r, l, n=3))
    ki = idx['kiefer']
    p = teile[ki]
    for k, jl, l, r in ((3, (3, 5), 340, 110), (2, (3, 5), 210, 90), (4, (3, 5), 200, 85), (1, (3, 5), 190, 85)):
        for j in jl:
            q = ringpunkt(p, stand['lr'][ki], stand['c'][ki], k, j)
            dazu(kegel('zahn_u', p.knochen, q + np.array([0, 40, 0]), (0.05, -1.0, 0.1 * (-1 if j <= 4 else 1)), r, l, n=3))
    # Hauer am Oberkiefer: links reicht die Spitze in Pose 2 auf Stelle P (366,304), dort sitzt im Bild ein heller Kern. Rechts der Spiegel.
    if hauer is not None:
        ko = idx['kopf']
        p = teile[ko]
        wurzel_l = ringpunkt(p, stand['lr'][ko], stand['c'][ko], 6, 2)
        wurzel_r = wurzel_l * np.array([1, 1, -1.0])
        spitze_l = hauer(p.knochen, wurzel_l)
        d = spitze_l - wurzel_l
        lang = float(np.linalg.norm(d))
        for nm, w, dd in (('hauer_l', wurzel_l, d), ('hauer_r', wurzel_r, d * np.array([1, 1, -1.0]))):
            dazu(kegel(nm, p.knochen, w - dd / lang * 60, dd, 150, lang * .5 + 60, n=5, feld=None, wurzel=(lang * .5, .72)))
    # Fusszehen: drei je Fuss am Ende des Fusses
    for s, vz in (('r', 1), ('l', -1)):
        f = idx['fuss_' + s]
        p = teile[f]
        m = np.array(p.o + p.t[-1] * p.d, float)
        rr = np.exp(np.array(stand['lr'][f][-1]) + p.relief[-1])
        b = float(np.max(rr))
        for i, (z, dz, l) in enumerate(((-0.66, -.20, 300), (0.0, 0.0, 360), (0.66, .20, 300))):
            q = m + p.eb * z * b * 0.9 + p.d * (-60) + p.ea * 40
            dazu(kegel('zeh', p.knochen, q, p.d + p.eb * dz + p.ea * .10, 125, l, n=5, feld='zeh', wurzel=(l * .35, .84)))
    return neu, dict(stand, lr=lr, c=c)
