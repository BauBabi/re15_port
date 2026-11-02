#!/usr/bin/env python3
"""Kandidat B: UV-Inseln nach Bitmaske in den Atlas legen (konkave Inseln greifen ineinander). Dichte per Bisektion."""
import math
import numpy as np, cv2
from scipy.signal import fftconvolve
import b_bau as BB


def inseln_roh(netze, dichte_je=None):
    roh = []
    for ni, nz in enumerate(netze):
        v, vt, f = nz['v'], nz['vt'], nz['f']
        for fl in BB.inseln(nz):
            ids = sorted({c for i in fl for _, c in f[i]})
            uv = vt[ids]
            a3 = sum(BB.flaeche3d(v, f[i]) for i in fl)
            a2 = sum(BB.flaeche2d([vt[c] for _, c in f[i]]) for i in fl)
            mass = math.sqrt(a3 / max(a2, 1e-12))
            pts = (uv - uv.mean(0)) * mass
            best = None
            hull = cv2.convexHull(pts.astype(np.float32))[:, 0, :]
            for w in np.arange(0, 90, 7.5):
                c, s = math.cos(math.radians(w)), math.sin(math.radians(w))
                r = hull @ np.array([[c, s], [-s, c]]).T
                ext = r.max(0) - r.min(0)
                if best is None or ext[0] * ext[1] < best[0]:
                    best = (ext[0] * ext[1], w)
            c, s = math.cos(math.radians(best[1])), math.sin(math.radians(best[1]))
            r = pts @ np.array([[c, s], [-s, c]]).T
            r -= r.min(0)
            idx = {c_: i for i, c_ in enumerate(ids)}
            polys = [[idx[c_] for _, c_ in f[i]] for i in fl]
            roh.append(dict(netz=ni, flaechen=fl, ids=ids, p=r, polys=polys, ext=r.max(0), a3=a3, gew=(dichte_je or {}).get(nz['name'], 1.0)))
    return roh


def maske(q, s, dreh, rand):
    p = q['p'] * s
    if dreh:
        p = p[:, ::-1]
    p = p + rand + 1
    b = int(math.ceil(p[:, 0].max())) + rand + 2
    h = int(math.ceil(p[:, 1].max())) + rand + 2
    m = np.zeros((h, b), np.uint8)
    pp = np.round(p).astype(np.int32)
    for f in q['polys']:
        cv2.fillPoly(m, [pp[f]], 1)
    if rand > 0:
        m = cv2.dilate(m, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * rand + 1, 2 * rand + 1)))
    return m


def lege(roh, d, B, H, rand):
    A = np.zeros((H, B), np.float32)
    aus = {}
    for i in sorted(range(len(roh)), key=lambda k: -roh[k]['a3'] * roh[k]['gew'] ** 2):
        q = roh[i]
        s = d * q['gew']
        best = None
        for dreh in (0, 1):
            m = maske(q, s, dreh, rand)
            h, b = m.shape
            if h > H or b > B:
                continue
            if h * b > 900:
                c = fftconvolve(A, m[::-1, ::-1].astype(np.float32), mode='valid')
            else:
                c = cv2.matchTemplate(A, m.astype(np.float32), cv2.TM_CCORR)
            ok = c < 0.5
            if not ok.any():
                continue
            ys, xs = np.nonzero(ok)
            k = np.argmin(ys * 3 + xs)              # zeilenweise von oben, kurz nach links
            wert = int(ys[k] * 3 + xs[k])
            if best is None or wert < best[0]:
                best = (wert, int(xs[k]), int(ys[k]), dreh, m)
        if best is None:
            return None
        _, x, y, dreh, m = best
        A[y:y + m.shape[0], x:x + m.shape[1]] += m
        aus[i] = (x, y, dreh)
    return aus


def packe(netze, B=384, H=256, rand=1, dichte_je=None, lo=0.015, hi=0.07, schritte=8):
    roh = inseln_roh(netze, dichte_je)
    beste = None
    for _ in range(schritte):
        m = (lo + hi) / 2
        lage = lege(roh, m, B, H, rand)
        if lage is None:
            hi = m
        else:
            lo = m
            beste = (m, lage)
    d, lage = beste
    tex = [[None] * len(nz['f']) for nz in netze]
    for i, q in enumerate(roh):
        x0, y0, dreh = lage[i]
        s = d * q['gew']
        pp = q['p'] * s
        if dreh:
            pp = pp[:, ::-1]
        pp = pp + rand + 1
        t = {c: (x0 + a, y0 + b) for c, (a, b) in zip(q['ids'], pp)}
        nz = netze[q['netz']]
        for fi in q['flaechen']:
            tex[q['netz']][fi] = [(int(round(t[c][0])), int(round(t[c][1]))) for _, c in nz['f'][fi]]
    return tex, d
