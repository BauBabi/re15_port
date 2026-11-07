#!/usr/bin/env python3
"""UV-Atlas: je Teil ein Streifen, Umfang U, Laenge V, 384x256, 8 bpp. Enden 'flach' zeigen auf einen dunklen Fleck. textur() ruft mal.py."""
import math
import numpy as np

TEX_W, TEX_H = 384, 256
FLECK = (2, 2, 5, 5)        # Kappen, immer verdeckt


def groessen(m):
    t = m['teil']
    P = t.punkte().reshape(m['nr'], m['n'], 3)
    um = np.linalg.norm(P - np.roll(P, -1, axis=1), axis=2).sum(1)
    dl = np.linalg.norm(P[1:] - P[:-1], axis=2).mean(1)
    return float(np.percentile(um, 80)), float(dl.sum()), dl


def packen(eintraege, d):
    """Skyline, breite Streifen zuerst. eintraege: Liste (name, um, lg, dichte). Rueckgabe dict oder None."""
    W, H = TEX_W, TEX_H
    rand = 1
    sky = np.full(W, 8, int)
    pos = {}
    for nm, um, lg, dichte in sorted(eintraege, key=lambda e: -e[2]):
        w = max(8, int(math.ceil(um * dichte / d)))
        h = max(6, int(math.ceil(lg * dichte / d)))
        best = None
        for x in range(0, W - w - 2 * rand + 1):
            y = int(sky[x:x + w + 2 * rand].max())
            if best is None or y < best[0]:
                best = (y, x)
        if best is None or best[0] + h + 2 * rand > H:
            return None
        y, x = best
        pos[nm] = (x + rand, y + rand, w, h)
        sky[x:x + w + 2 * rand] = y + h + 2 * rand
    return pos


def karte(N):
    """-> karte[name] = Liste je Flaeche: (x,y) je Ecke; streifen[name] = dict(x,y,w,h, v[i], n, nr)."""
    ein = []
    gr = {}
    for m in N:
        um, lg, dl = groessen(m)
        gr[m['name']] = (um, lg, dl)
        ein.append((m['name'], um, lg, m['teil'].dichte))
    lo, hi = 2.0, 80.0
    best = None
    for _ in range(26):
        d = 0.5 * (lo + hi)
        p = packen(ein, d)
        if p is None:
            lo = d
        else:
            hi = d
            best = p
    pos = best
    karte = {}
    streifen = {}
    for m in N:
        x0, y0, w, h = pos[m['name']]
        um, lg, dl = gr[m['name']]
        n, nr = m['n'], m['nr']
        vv = np.concatenate([[0.0], np.cumsum(dl)]) / max(lg, 1e-6)
        streifen[m['name']] = dict(x=x0, y=y0, w=w, h=h, v=vv.tolist(), n=n, nr=nr, d=hi)
        li = []

        def uv_ecke(i, j):
            return (x0 + w * j / n, y0 + (h - 1) * vv[i])
        for f, (a, i, j) in zip(m['F'], m['art']):
            if a == 'q':
                ecken = [(i, j), (i, j + 1), (i + 1, j + 1), (i + 1, j)]
                e = [uv_ecke(*q) for q in ecken]
            elif a == 'se':
                e = [uv_ecke(i, j), uv_ecke(i, j + 1), (x0 + w * (j + 0.5) / n, y0 + h - 1)]
            elif a == 'sa':
                e = [(x0 + w * (j + 0.5) / n, y0), uv_ecke(i, j + 1), uv_ecke(i, j)]
            else:
                e = [(FLECK[0], FLECK[1]), (FLECK[2], FLECK[1]), (FLECK[2], FLECK[3]), (FLECK[0], FLECK[3])][:len(f)]
            # netz.py dreht Flaechen um (Spiegel, Aussenprobe): UV muss mit
            if m.get('gedreht'):
                e = e[::-1]
            li.append(e)
        karte[m['name']] = [[(int(round(min(x, TEX_W - 1))), int(round(min(y, TEX_H - 1)))) for x, y in e] for e in li]
    return karte, streifen


def textur(N, ks, form, posen, streifen, karte, ohne_zeichnung=False):
    import mal
    return mal.textur(N, ks, form, posen, streifen, karte, ohne_zeichnung)
