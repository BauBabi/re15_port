#!/usr/bin/env python3
"""Form bemalt, Glaetter (E4): aus den Ringen des Rohbaus (rf_plan + Stand) eine glatte Flaeche je Teil. Mesh i bleibt an Knochen i, kein Skinning.
Rundum: Catmull-Rom geschlossen, n -> m Ecken, Querschnittsflaeche je Ring erhalten (Umriss bleibt im Mittel stehen).
Laengs: PCHIP je Spalte ueber die Sehnenlaenge (kein Ueberschwingen, also keine neuen Wuelste), Zwischenringe alle SCHRITT Einheiten.
Enden: Gelenkenden als flache Kuppel, die im Nachbarn steckt, Normalen dort radial (kein dunkler Ring an der Fuge). Freie Spitzen als Kegel mit Vorring.
Kein RE-Wert: neues Modell nach zwei Videostandbildern."""
import math

import numpy as np
from scipy.interpolate import PchipInterpolator

# Ecken rundum je Ausgangszahl, Ringabstand laengs (Einheiten). Frei gesetzt: so fein, dass der Umriss im Spiel rund liest, Flaechen unter rund 4000.
# Rahmen der Pruefung: rund 1900 Flaechen (pruefung.md). Darum keine allgemeine Verfeinerung: Rumpf und Kopf 12 statt 10 Ecken, Schwanz 10 statt 8 (E4 glatt), sonst wie der Rohbau.
# Glatt wird die Flaeche ueber die Normalen je Ecke und den Verlauf der Ringe, nicht ueber mehr Flaechen. Zwischenringe nur ueber grosse Luecken.
ECKEN_ART = dict(rumpf=12, schwanz=10, glied=8, hand=8, finger=6, kegel=4)
ECKEN_NAME = dict(kopf=12, kiefer=8, hals=8)
ECKEN = {10: 12, 8: 8, 6: 6, 4: 4}
SCHRITT = dict(rumpf=420.0, schwanz=420.0, glied=420.0, hand=420.0, finger=9999.0, kopf=420.0, kegel=9999.0)
# Kuppeln mit zwei Ringen nur, wo das Ende zu sehen ist (Ferse, Fuss, Hand vorn); sonst ein Ring, das Ende steckt im Nachbarn
KUPPEL2 = {('unterschenkel_r', 1), ('unterschenkel_l', 1), ('fuss_r', 0), ('fuss_l', 0), ('fuss_r', 1), ('fuss_l', 1), ('hand_r', 1), ('hand_l', 1)}
# Spitze Enden (Kegel): Name beginnt mit ... oder genau. Sonst Kuppel.
SPITZ1 = ('klaue', 'kralle', 'zeh', 'fang', 'wange', 'schwanz6', 'kopf', 'kiefer')
SPITZ_VOR = {'klaue_l2': (0.5, 0.62), 'kralle_l': (0.5, 0.62)}
SPITZ0 = ('schwanz1', 'klaue', 'kralle', 'zeh', 'fang', 'wange')


def art(name):
    if name in ('huefte', 'wirbel', 'brust'):
        return 'rumpf'
    if name.startswith('schwanz'):
        return 'schwanz'
    if name.startswith(('klaue', 'kralle')):
        return 'finger'
    if name.startswith(('zeh', 'fang', 'wange')):
        return 'kegel'
    if name.startswith('hand') or name.startswith('fuss'):
        return 'hand'
    if name in ('kopf', 'kiefer', 'hals'):
        return 'kopf'
    return 'glied'


def _cr(p0, p1, p2, p3, u):
    u2, u3 = u * u, u * u * u
    return 0.5 * (2 * p1 + (-p0 + p2) * u + (2 * p0 - 5 * p1 + 4 * p2 - p3) * u2 + (-p0 + 3 * p1 - 3 * p2 + p3) * u3)


def rund(P, m):
    """Geschlossener Catmull-Rom durch die n Ecken, m gleichmaessige Stellen. Ecke 0 bleibt Ecke 0 (Phase), Laufrichtung bleibt."""
    n = len(P)
    out = []
    for k in range(m):
        s = k * n / m
        i = int(math.floor(s + 1e-9)) % n
        u = s - math.floor(s + 1e-9)
        out.append(_cr(P[(i - 1) % n], P[i], P[(i + 1) % n], P[(i + 2) % n], u))
    return np.array(out)


def flaeche(P, c):
    s = np.zeros(3)
    for i in range(len(P)):
        s += np.cross(P[i] - c, P[(i + 1) % len(P)] - c)
    return 0.5 * np.linalg.norm(s)


def ring_rund(P, m):
    c = P.mean(0)
    Q = rund(P, m)
    a0, a1 = flaeche(P, c), flaeche(Q, c)
    if a1 > 1e-6:
        Q = c + (Q - c) * (0.5 + 0.5 * math.sqrt(a0 / a1))      # halb flaechentreu: Ecken leicht nach innen, Kantenmitten leicht nach aussen
    return Q


def laengs(R, schritt):
    """R[T,m,3] -> Ringe mit Zwischenringen. PCHIP je Spalte ueber die Sehnenlaenge. Rueckgabe Ringe, Herkunft (Ring i, Anteil f)."""
    T, m, _ = R.shape
    if T < 2:
        return R.copy(), [(0, 0.0)]
    lang = np.linalg.norm(np.diff(R, axis=0), axis=2)      # [T-1, m]
    k = [max(0, int(math.ceil(float(lang[i].max()) / schritt)) - 1) for i in range(T - 1)]
    stellen = []
    for i in range(T - 1):
        for j in range(k[i] + 1):
            stellen.append((i, j / (k[i] + 1)))
    stellen.append((T - 1, 0.0))
    aus = np.zeros((len(stellen), m, 3))
    for c in range(m):
        X = R[:, c, :]
        s = np.concatenate([[0], np.cumsum(np.maximum(np.linalg.norm(np.diff(X, axis=0), axis=1), 1e-3))])
        f = PchipInterpolator(s, X, axis=0) if T >= 3 else None
        for q, (i, a) in enumerate(stellen):
            if a == 0.0:
                aus[q, c] = X[i]
            else:
                x = s[i] + a * (s[i + 1] - s[i])
                aus[q, c] = f(x) if f is not None else X[i] * (1 - a) + X[i + 1] * a
    return aus, stellen


def glatt_geometrie(p, ring, k0, k1):
    """ring[T,n,3], k0, k1 Kappenspitzen (Knochenraum). -> dict(pts, m, NR, n, T, rolle, anteil, herk, quads, tris, a0, a1). Noch ohne Normalen."""
    T, n, _ = ring.shape
    a = art(p.name)
    m = ECKEN_NAME.get(p.name, ECKEN_ART.get(a, ECKEN.get(n, n)))
    R = np.stack([ring_rund(ring[i], m) for i in range(T)])
    R, herk = laengs(R, SCHRITT[a])
    spitz0 = p.name.startswith(SPITZ0)
    spitz1 = p.name.startswith(SPITZ1)

    def ende(Rend, apex, spitz, seite):
        c = Rend.mean(0)
        if spitz:
            # Kegel mit Vorring: leicht gewoelbt, Spitze bleibt. Linke Klaue (verlaengert, Auftrag 2): voller, damit die Spitze im Bild ankommt.
            # Rahmen rund 1900 Flaechen: Vorring nur, wo er traegt (verlaengerte Klaue, Schwanzspitze, Schnauze, Kinn); kleine Kegel ohne.
            if p.name not in SPITZ_VOR and p.name not in ('schwanz6', 'kopf', 'kiefer'):
                return []
            fa, fr = SPITZ_VOR.get(p.name, (0.32, 0.74))
            q = c + fa * (apex - c)
            return [(q + fr * (Rend - c), fa, True)]
        ls = []
        for th in ((math.radians(52),) if (p.name, seite) in KUPPEL2 else ()):
            q = c + math.sin(th) * (apex - c)
            ls.append((q + math.cos(th) * (Rend - c), math.sin(th), False))
        return ls
    e0 = ende(R[0], np.asarray(k0, float), spitz0, 0)
    e1 = ende(R[-1], np.asarray(k1, float), spitz1, 1)
    alle = [x[0] for x in reversed(e0)] + list(R) + [x[0] for x in e1]
    rolle = ['e0'] * len(e0) + ['k'] * len(R) + ['e1'] * len(e1)
    anteil = [x[1] for x in reversed(e0)] + [0.0] * len(R) + [x[1] for x in e1]
    pts = np.concatenate([np.stack(alle).reshape(-1, 3), np.asarray(k0, float)[None], np.asarray(k1, float)[None]])
    NR = len(alle)
    a0, a1 = NR * m, NR * m + 1
    idx = lambda r, j: r * m + (j % m)  # noqa: E731
    quads, tris = [], []
    for r in range(NR - 1):
        for j in range(m):
            quads.append(((r, j), (r, j + 1), (r + 1, j + 1), (r + 1, j)))
    for j in range(m):
        tris.append(('a0', (0, j + 1), (0, j)))
        tris.append(('a1', (NR - 1, j), (NR - 1, j + 1)))
    # Windung: Probe an einem mittleren Ring. Port: cross(b-a, c-a) zeigt GEGEN die Normale (form_emd._falsch_gewunden), also umgekehrt zur Aussenrichtung
    if quads:
        r_ = NR // 2
        q = quads[r_ * m] if r_ < NR - 1 else quads[(r_ - 1) * m]
        P_ = [pts[idx(*v)] for v in q]
        fn = np.cross(P_[1] - P_[0], P_[2] - P_[0])
        aussen = float(fn @ (P_[0] - (pts[q[0][0] * m:(q[0][0] + 1) * m].mean(0)))) > 0
    else:
        # ein Ring (kleiner Kegel ohne Vorring): Probe an der Spitze a1 gegen den Schwerpunkt
        t_ = tris[1]
        P_ = [pts[a1] if v == 'a1' else pts[a0] if v == 'a0' else pts[idx(*v)] for v in t_]
        fn = np.cross(P_[1] - P_[0], P_[2] - P_[0])
        aussen = float(fn @ (np.mean(P_, 0) - pts.mean(0))) > 0
    if aussen:
        quads = [tuple(reversed(x)) for x in quads]
        tris = [tuple(reversed(x)) for x in tris]

    def vid(v):
        if v == 'a0':
            return a0
        if v == 'a1':
            return a1
        return idx(*v)
    Q = [[vid(v) for v in x] for x in quads]
    D = [[vid(v) for v in x] for x in tris]
    return dict(pts=pts, m=m, n=n, T=T, NR=NR, rolle=rolle, anteil=anteil, herk=herk, quads=Q, tris=D, a0=a0, a1=a1, spitz0=spitz0, spitz1=spitz1, d=np.asarray(p.d, float))


def delta_karte(G):
    """Lineare Abbildung der Ausgangs-Gitterwerte [T, n] auf die Glattecken [NR*m]: laengs nach Herkunft, rundum nach Winkel. -> Matrix [NR*m, T*n]."""
    T, n, m, NR = G['T'], G['n'], G['m'], G['NR']
    A = np.zeros((NR * m, T * n))
    ks = [r for r in range(NR) if G['rolle'][r] == 'k']
    zeile = {}
    for r, (i, f) in zip(ks, G['herk']):
        zeile[r] = [(i, 1 - f)] + ([(i + 1, f)] if f > 0 else [])
    for r in range(NR):
        if G['rolle'][r] == 'e0':
            zeile[r] = [(0, 1.0)]
        elif G['rolle'][r] == 'e1':
            zeile[r] = [(T - 1, 1.0)]
    for r in range(NR):
        for k in range(m):
            s = k * n / m
            j0 = int(math.floor(s + 1e-9)) % n
            g = s - math.floor(s + 1e-9)
            for i, w in zeile[r]:
                A[r * m + k, i * n + j0] += w * (1 - g)
                if g > 0:
                    A[r * m + k, i * n + (j0 + 1) % n] += w * g
    return A


def delta_anwenden(G, delta):
    """delta[T, n] log-Faktor: Glattecken radial um ihre Ringmitte skalieren (Kuppelringe mit dem Endring)."""
    if delta is None:
        return G['pts']
    m, NR = G['m'], G['NR']
    f = np.exp(delta_karte(G) @ np.asarray(delta, float).reshape(-1))
    pts = G['pts'].copy()
    R = pts[:NR * m].reshape(NR, m, 3)
    c = R.mean(1, keepdims=True)
    R = c + (R - c) * f.reshape(NR, m, 1)
    pts[:NR * m] = R.reshape(-1, 3)
    return pts


def glatt_normalen(G, pts):
    """Normalen je Ecke: Flaechensumme, fuer die Ringe nur Seitenflaechen; Gelenkenden radial mit wenig Achsanteil."""
    m, NR, rolle, anteil, a0, a1 = G['m'], G['NR'], G['rolle'], G['anteil'], G['a0'], G['a1']
    spitz0, spitz1, d = G['spitz0'], G['spitz1'], G['d']
    Q, D = G['quads'], G['tris']
    nrm = np.zeros_like(pts)
    for f in Q:
        a_ = pts[f[0]]
        s = np.cross(pts[f[1]] - a_, pts[f[2]] - a_) + np.cross(pts[f[2]] - a_, pts[f[3]] - a_)
        for v in f:
            nrm[v] += s
    for f in D:
        s = np.cross(pts[f[1]] - pts[f[0]], pts[f[2]] - pts[f[0]])
        for v in f:
            if v in (a0, a1):
                nrm[v] += s
            else:
                r = v // m
                if rolle[r] != 'k' and not ((rolle[r] == 'e0' and spitz0) or (rolle[r] == 'e1' and spitz1)):
                    continue
                nrm[v] += s
    nrm = -nrm      # Port-Windung: cross(b-a, c-a) zeigt nach innen, die Normale nach aussen
    nrm /= np.maximum(np.linalg.norm(nrm, axis=1, keepdims=True), 1e-9)
    i_k0 = rolle.index('k')
    i_k1 = len(rolle) - 1 - rolle[::-1].index('k')
    for seite, spitz, ik, rs in ((0, spitz0, i_k0, [r for r in range(NR) if rolle[r] == 'e0']), (1, spitz1, i_k1, [r for r in range(NR) if rolle[r] == 'e1'])):
        if spitz:
            continue
        ri = ik + (1 if seite == 0 else -1)
        if 0 <= ri < NR and rolle[ri] == 'k':
            sn = nrm[ri * m:(ri + 1) * m].copy()
        else:
            sn = nrm[ik * m:(ik + 1) * m].copy()
        ax = -d if seite == 0 else d
        nrm[ik * m:(ik + 1) * m] = sn
        for r in rs:
            w = anteil[r] * 0.55
            v = sn * (1 - w) + ax[None] * w
            nrm[r * m:(r + 1) * m] = v / np.linalg.norm(v, axis=1, keepdims=True)
        apx = a0 if seite == 0 else a1
        nrm[apx] = ax
    return nrm


def glatt_teil(p, ring, k0, k1, verform=None, delta=None):
    G = glatt_geometrie(p, ring, k0, k1)
    pts = delta_anwenden(G, delta)
    if verform is not None:
        pts = verform(p.name, pts, G['m'], G['NR'])
    G['pts'] = pts
    G['nrm'] = glatt_normalen(G, pts)
    G['ring_pts'] = pts[:G['NR'] * G['m']].reshape(G['NR'], G['m'], 3)
    return G
