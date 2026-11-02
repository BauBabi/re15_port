#!/usr/bin/env python3
"""Teile -> Netze: Ecken lokal am Knochen, Flaechen als Ring (von aussen im Uhrzeigersinn wie form_emd.netz), Ringposition je Ecke fuer die UV-Karte."""
import numpy as np
from ringe import hohl_quads


def netz_von(t, q=None):
    """-> dict(name, knochen, V[m,3], F, ring[m] (Index oder -1 Spitze), ecke[m], teil). Spiegelung: z -> -z, Ringrichtung umgekehrt."""
    P = t.punkte(q)
    nr, n = P.shape[:2]
    V = P.reshape(-1, 3)
    idx = lambda i, j: i * n + (j % n)                     # noqa: E731
    F, art = [], []
    for i in range(nr - 1):
        for j in range(n):
            # von aussen gesehen im Uhrzeigersinn; Rahmen: 0 Grad oben, 90 Grad rechts, Achse nach vorn. Probe unten orientiert notfalls um.
            F.append((idx(i, j), idx(i, j + 1), idx(i + 1, j + 1), idx(i + 1, j)))
            art.append(('q', i, j))
    tips = t.tip()
    V = list(V)
    # Reihenfolge der Zusatzpunkte: erst Spitzen, dann Kappenmitten (fit.py kennt nur Ringe und Spitzen)
    ia = ie = None
    if t.anf == 'spitz':
        ia = len(V)
        V.append(tips[0])
    if t.ende == 'spitz':
        ie = len(V)
        V.append(tips[1])
    if t.anf == 'spitz':
        for j in range(n):
            F.append((ia, idx(0, j + 1), idx(0, j)))
            art.append(('sa', 0, j))
    else:
        c = P[0].mean(0) + t.ea * 0.6 * float(np.linalg.norm(P[0] - P[0].mean(0), axis=1).mean())
        m0 = len(V)
        V.append(c)
        for f in hohl_quads(n, [idx(0, j) for j in range(n)], m0, umkehr=True):
            F.append(tuple(f))
            art.append(('k', 0, 0))
    if t.ende == 'spitz':
        for j in range(n):
            F.append((idx(nr - 1, j), idx(nr - 1, j + 1), ie))
            art.append(('se', nr - 1, j))
    else:
        c = P[-1].mean(0) - t.ea * 0.6 * float(np.linalg.norm(P[-1] - P[-1].mean(0), axis=1).mean())
        m1 = len(V)
        V.append(c)
        for f in hohl_quads(n, [idx(nr - 1, j) for j in range(n)], m1):
            F.append(tuple(f))
            art.append(('k', nr - 1, 0))
    V = np.array(V, float)
    if t.spiegel:
        V = V * np.array([1, 1, -1.0])
        F = [tuple(reversed(f)) for f in F]
    return dict(name=t.name, knochen=t.knochen, V=V, F=F, art=art, nr=nr, n=n, teil=t, spiegel=t.spiegel, gedreht=bool(t.spiegel))


def aussen_pruefen(m):
    """Flaechennormale muss weg von der Ringmitte zeigen. Rueckgabe: Anteil falsch, und Flaechen umgedreht falls noetig."""
    V, F = m['V'], m['F']
    t = m['teil']
    c = t.M.copy()
    if t.spiegel:
        c = c * np.array([1, 1, -1.0])
    falsch = 0
    gesamt = 0
    for f, (a, i, j) in zip(F, m['art']):
        if a != 'q':
            continue
        p = V[list(f)]
        n = np.cross(p[1] - p[0], p[2] - p[0])
        d = p.mean(0) - c[i]
        gesamt += 1
        if n @ d < 0:
            falsch += 1
    return falsch / max(1, gesamt)


def netze(teile, korr=None):
    aus = []
    for t in teile:
        q = None if korr is None else korr.get(t.name)
        if q is not None and np.shape(q) != t.R.shape:
            q = None
        m = netz_von(t, None if q is None else np.array(q))
        f = aussen_pruefen(m)
        if f > 0.5:
            m['F'] = [tuple(reversed(x)) for x in m['F']]
            m['gedreht'] = not m['gedreht']
        aus.append(m)
    return aus
