# -*- coding: utf-8 -*-
"""Kandidat C: Netze aus Umrissen. Kugelnetz, Schlauch, Silhouetten-Anpassung."""
from kern import *
from scipy.ndimage import map_coordinates, gaussian_filter


def kugelnetz(mitte, halb, n_lang, n_breit, phi0=0.0):
    """Laengen-Breiten-Netz um die Y-Achse. Rueckgabe V, F (Vierecke und Dreiecke, Ring um die Flaeche), Index (ring, lang) -> Ecke."""
    V = []
    idx = {}
    V.append([mitte[0], mitte[1] - halb[1], mitte[2]])            # Pol oben (Y klein)
    for j in range(1, n_breit):
        th = math.pi * j / n_breit
        for i in range(n_lang):
            ph = phi0 + 2 * math.pi * i / n_lang
            idx[j, i] = len(V)
            V.append([mitte[0] + halb[0] * math.sin(th) * math.cos(ph), mitte[1] - halb[1] * math.cos(th), mitte[2] + halb[2] * math.sin(th) * math.sin(ph)])
    pol_u = len(V)
    V.append([mitte[0], mitte[1] + halb[1], mitte[2]])
    F = []
    for i in range(n_lang):
        k = (i + 1) % n_lang
        F.append((0, idx[1, k], idx[1, i]))
        F.append((pol_u, idx[n_breit - 1, i], idx[n_breit - 1, k]))
        for j in range(1, n_breit - 1):
            F.append((idx[j, i], idx[j, k], idx[j + 1, k], idx[j + 1, i]))
    return np.array(V, float), F, idx


def flaechen_dreiecke(F):
    T = []
    for f in F:
        for i in range(1, len(f) - 1):
            T.append((f[0], f[i], f[i + 1]))
    return np.array(T, int)


def eckennormalen(V, T, innen_mal=1.0):
    """Normalen aus Dreiecken T, Ausrichtung ueber Mittelpunkt: nach aussen."""
    a, b, c = V[T[:, 0]], V[T[:, 1]], V[T[:, 2]]
    n = np.cross(b - a, c - a)
    N = np.zeros_like(V)
    for k in range(3):
        np.add.at(N, T[:, k], n)
    N /= (np.linalg.norm(N, axis=1, keepdims=True) + 1e-9)
    mitte = V.mean(0)
    if ((V - mitte) * N).sum() < 0:
        N = -N
    return N


def sdf(maske):
    m = maske.astype(np.uint8)
    d_a = cv2.distanceTransform(1 - m, cv2.DIST_L2, 5)
    d_i = cv2.distanceTransform(m, cv2.DIST_L2, 5)
    return d_a - d_i


def lese_bl(f, p):
    return map_coordinates(f, [p[:, 1], p[:, 0]], order=1, mode='nearest')


def nachbarn(F, n):
    N = [set() for _ in range(n)]
    for f in F:
        for i in range(len(f)):
            a, b = f[i], f[(i + 1) % len(f)]
            N[a].add(b); N[b].add(a)
    return [np.array(sorted(s), int) for s in N]


def laplace(D, NB, schritt=0.5, n=1):
    for _ in range(n):
        M = np.array([D[q].mean(0) if len(q) else D[i] for i, q in enumerate(NB)])
        D = D + schritt * (M - D)
    return D


def silhouette_anpassen(V, F, ziele, iter_n=300, rate=0.5, glatt=1, glatt_formerhalt=0.15, rand=0.32, frei=None, ringe=None, k_max=4):
    """Netz V an Zielumrisse anpassen. ziele = Liste (Stand, SDF, gewicht, unbekannt-Maske oder None).
    Ecken ausserhalb des Ziels werden immer gezogen, Randecken (Normale quer zum Strahl) auch nach aussen. frei = Maske bewegter Ecken."""
    T = flaechen_dreiecke(F)
    NB = nachbarn(F, len(V))
    V = V.copy()
    for it in range(iter_n):
        N = eckennormalen(V, T)
        D = np.zeros_like(V)
        W = np.zeros(len(V))
        for zl in ziele:
            s, d, g, unbek = zl[:4]
            nur_rein = len(zl) > 4 and zl[4] == 'rein'
            p = s.bild(V)
            auge = s.auge_aktor()
            r = V - auge
            tief = np.linalg.norm(r, axis=1)
            r /= tief[:, None]
            c = np.abs((N * r).sum(1))
            f = 0.5 * (s.k['brennweite_bild_px']['fx'] + s.k['brennweite_bild_px']['fy'])
            px = f / tief
            dd = lese_bl(d, p)
            rimw = np.exp(-(c / rand) ** 2)
            aussen = dd > 0
            cur = raster(V, F, s)
            dc = cv2.distanceTransform(cur, cv2.DIST_L2, 3)
            amrand = lese_bl(dc, p) < 3.5                    # nur Ecken am aktuellen Umriss duerfen nach aussen
            w = np.where(aussen, np.maximum(rimw, 0.5), 0.0 if nur_rein else rimw * amrand) * g
            if unbek is not None:
                w = w * (lese_bl(unbek.astype(np.float32), p) < 0.5)
            D += (w * (-dd / px))[:, None] * N
            W += w
        D = D / (W[:, None] + 1e-3)
        D = laplace(D, NB, 0.6, glatt)
        if frei is not None:
            D[~frei] = 0
        lg = np.linalg.norm(D, axis=1, keepdims=True)
        D = D * np.minimum(1.0, 60.0 / (lg + 1e-6))
        V += rate * D
        if glatt_formerhalt > 0:
            M = np.array([V[q].mean(0) for q in NB])
            E = M - V
            if frei is not None:
                E[~frei] = 0
            V += glatt_formerhalt * 0.1 * E
        if ringe is not None and it % 2 == 0:
            V = ringe_glaetten(V, ringe, k_max, 0.5)
    return V


def strahl_ellipsoid(c0, d, m, h):
    """Groesste Entfernung von c0 in Richtung d (Einheitsvektoren n x 3) bis zur Ellipsoidflaeche, 0 wenn kein Treffer."""
    o = (c0 - np.asarray(m, float)) / np.asarray(h, float)
    q = d / np.asarray(h, float)
    a = (q * q).sum(1)
    b = 2 * (o[None, :] * q).sum(1)
    c = (o * o).sum() - 1
    D = b * b - 4 * a * c
    r = np.zeros(len(d))
    ok = D > 0
    r[ok] = (-b[ok] + np.sqrt(D[ok])) / (2 * a[ok])
    return np.maximum(r, 0)


def ringe_glaetten(V, ringe, k_max=4, beta=0.5):
    """Jeden Ring als Fourier-Reihe mit wenigen Gliedern darstellen: harte Grate aus der Huelle verschwinden, die Tangenten bleiben fast liegen."""
    V = V.copy()
    for r in ringe:
        r = np.asarray(r, int)
        z = V[r, 0] + 1j * V[r, 2]
        c = np.fft.fft(z)
        n = len(r)
        f = np.fft.fftfreq(n, 1.0 / n)
        c[np.abs(f) > k_max] = 0
        z2 = np.fft.ifft(c)
        y = np.fft.rfft(V[r, 1])
        y[k_max + 1:] = 0
        y2 = np.fft.irfft(y, n)
        V[r, 0] = (1 - beta) * V[r, 0] + beta * z2.real
        V[r, 2] = (1 - beta) * V[r, 2] + beta * z2.imag
        V[r, 1] = (1 - beta) * V[r, 1] + beta * y2
    return V


def laengs_glaetten(V, ringe, beta=0.35, n=3):
    """Ringe gegeneinander glaetten (Spalten ueber die Hoehe), Enden fest."""
    V = V.copy()
    R = np.array(ringe)
    for _ in range(n):
        A = V[R]
        B = A.copy()
        B[1:-1] = (1 - beta) * A[1:-1] + beta * 0.5 * (A[:-2] + A[2:])
        V[R] = B
    return V
