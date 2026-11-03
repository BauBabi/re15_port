#!/usr/bin/env python3
"""Variante K (E20), Geometrie: Alligator-Kopf (Knochen 22: Schaedel + Oberkiefer + obere Zaehne), Unterkiefer (Knochen 23: Trog mit Maulboden + untere Zaehne),
Krallen (Hakenkrallen an den Fingern). Alles im Knochenraum: +X vorn laengs, +Y unten.
Kein RE-Wert: Form nach den Nutzer-Teilen Oberkiefer/Unterkiefer (E20) und den Handschnitten in Screenshot 2."""
import math

import numpy as np

# Atlas der neuen Seite (TIM-x 384-511, CLUT-Zeile 3). Alte Teile behalten ihre Texel und Zeilen 0-2 unveraendert.
SEITE_X = 384
ATLAS = {'kopf': (384, 0, 128, 104), 'kiefer': (384, 106, 128, 72), 'zahn': (384, 180, 24, 14), 'kralle': (410, 180, 100, 40),
         'auge': (384, 196, 24, 10), 'naht': (384, 222, 126, 32)}
ZEILE_NEU = 3


class Netz:
    """Punkte, Flaechen als Ring (3 oder 4 Indizes im Umlauf), Texel je Punkt, Normalen je Punkt."""

    def __init__(self):
        self.p, self.f, self.t, self.n = [], [], [], []

    def dazu(self, o):
        k = len(self.p)
        self.p += o.p
        self.t += o.t
        self.n += o.n
        self.f += [[k + i for i in f] for f in o.f]
        return self

    def arrays(self):
        return np.array(self.p, float).reshape(-1, 3), self.f, np.array(self.t, float).reshape(-1, 2), np.array(self.n, float).reshape(-1, 3)


def _einheit(v):
    v = np.asarray(v, float)
    return v / max(np.linalg.norm(v), 1e-9)


def _normalen(P, F, innen):
    """Glatte Normalen: Flaechennormale nach aussen gedreht (weg vom Innenpunkt je Flaeche), flaechengewichtet je Punkt."""
    N = np.zeros_like(P)
    for f, c in zip(F, innen):
        q = P[f]
        k = np.cross(q[1] - q[0], q[2] - q[0])
        if len(f) == 4:
            k = k + np.cross(q[2] - q[0], q[3] - q[0])
        if k @ (q.mean(0) - c) < 0:
            k = -k
        for i in f:
            N[i] += k
    L = np.linalg.norm(N, axis=1, keepdims=True)
    return N / np.maximum(L, 1e-9)


def rohr(zentren, ea, eb, ringe, rect, spitze0=None, spitze1=None, u_spanne=(0.0, 1.0), bogen=False):
    """Rohr ueber Ringe. zentren (S,3), ea/eb (S,3) Querachsen je Station, ringe (S,N,2) Versatz (a,b) je Ecke. Naht: Ecke 0 doppelt (Textur).
    spitze0/1: Punkt vor dem ersten / hinter dem letzten Ring (Faecher), None = Deckel um die Ringmitte.
    bogen=True (Kopfart 'kroko', Nachbesserung 2): v nach der Bogenlaenge der Ringmitten, u nach dem Umfang des mittleren Rings statt nach
    Stations-/Eckenindex - bei ungleich langen Abschnitten (kurzer Schaedel, lange Schnauze) sonst gestreckte/gestauchte Texel."""
    zentren, ea, eb, ringe = (np.asarray(x, float) for x in (zentren, ea, eb, ringe))
    S, N = ringe.shape[:2]
    x0, y0, w, h = rect
    P, T = [], []
    idx = np.zeros((S, N + 1), int)
    vs = np.arange(S) / max(S - 1, 1)
    us = np.arange(N + 1) / float(N)
    if bogen:
        d = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(zentren, axis=0), axis=1))])
        vs = d / max(d[-1], 1e-9)
        rm = ringe.mean(0)
        k = np.concatenate([[0.0], np.cumsum(np.linalg.norm(np.diff(np.vstack([rm, rm[:1]]), axis=0), axis=1))])
        us = k / max(k[-1], 1e-9)
    for s in range(S):
        for j in range(N + 1):
            a, b = ringe[s, j % N]
            idx[s, j] = len(P)
            P.append(zentren[s] + a * ea[s] + b * eb[s])
            u = u_spanne[0] + (u_spanne[1] - u_spanne[0]) * us[j]
            T.append((x0 + 0.5 + u * (w - 1), y0 + 1 + (h - 3) * vs[s]))
    F, innen = [], []
    for s in range(S - 1):
        for j in range(N):
            F.append([idx[s, j], idx[s, j + 1], idx[s + 1, j + 1], idx[s + 1, j]])
            innen.append((zentren[s] + zentren[s + 1]) / 2)
    for s, sp, vz in ((0, spitze0, -1), (S - 1, spitze1, 1)):
        ax = _einheit(zentren[min(s + 1, S - 1)] - zentren[max(s - 1, 0)])
        c = zentren[s] if sp is None else np.asarray(sp, float)
        k = len(P)
        P.append(c)
        T.append((x0 + w / 2, y0 + (0.5 if s == 0 else h - 1.5)))
        rm = zentren[s] - vz * 40.0 * ax
        for j in range(N):
            F.append([idx[s, j], idx[s, j + 1], k])
            innen.append(rm)
    P = np.array(P)
    Nn = _normalen(P, F, innen)
    o = Netz()
    o.p, o.f, o.t, o.n = [list(x) for x in P], F, [list(x) for x in T], [list(x) for x in Nn]
    return o


def zahn(basis, richtung, lang, breit, rect, quer=(0, 0, 1)):
    """Zahn als dreiseitige Pyramide (Grund steckt im Kiefer). basis = Mitte der Grundflaeche."""
    d = _einheit(richtung)
    a = _einheit(np.cross(d, quer))
    b = np.cross(d, a)
    basis = np.asarray(basis, float)
    G = [basis + breit * (math.cos(t) * a + math.sin(t) * b) for t in (0.3, 0.3 + 2 * math.pi / 3, 0.3 + 4 * math.pi / 3)]
    S = basis + lang * d
    P = np.array(G + [S])
    F = [[0, 1, 3], [1, 2, 3], [2, 0, 3]]
    x0, y0, w, h = rect
    T = [(x0 + 1, y0 + h - 1), (x0 + w / 2, y0 + h - 1), (x0 + w - 1, y0 + h - 1), (x0 + w / 2, y0 + 1)]
    Nn = _normalen(P, F, [basis + 0.3 * lang * d] * 3)
    o = Netz()
    o.p, o.f, o.t, o.n = [list(x) for x in P], F, [list(x) for x in T], [list(x) for x in Nn]
    return o


# ─────────────────────────────── Kopf
def ring_oben(w, yt, yb):
    """Querschnitt Oberkopf (z quer, y unten), 10 Ecken: 0 Gaumenmitte, 5 Scheitel. Flacher, abgerundeter Kasten (Krokodilschaedel)."""
    h = yb - yt
    halb = [(0.0, yb), (0.72 * w, yb), (w, yb - 0.22 * h), (0.97 * w, yb - 0.62 * h), (0.74 * w, yt + 0.08 * h), (0.0, yt)]
    return [(z, y) for z, y in halb] + [(-z, y) for z, y in halb[-2:0:-1]]


def ring_kiefer(w, yr, yb, tiefe):
    """Unterkiefer: U-Trog. 0 Kinnboden, 3/7 Rand aussen, 4/6 Rand innen, 5 Maulboden. Rand oben bei yr, Boden yb, Maulboden yr + tiefe."""
    h = yb - yr
    halb = [(0.0, yb), (0.7 * w, yb - 0.12 * h), (w, yr + 0.45 * h), (0.96 * w, yr), (0.72 * w, yr), (0.0, yr + tiefe)]
    return [(z, y) for z, y in halb] + [(-z, y) for z, y in halb[-2:0:-1]]


VORGABE = dict(L=2100.0, Wk=440.0, Ws=360.0, Wt=300.0, yt_k=-430.0, yt_s=-250.0, yt_t=-160.0, y_gaumen=270.0, zaehne=6,
               zahn_lang=120.0, kiefer_tief=110.0, kiefer_boden=170.0, kiefer_kuerzer=40.0, augen=1, x_hinten=-220.0)


def kopf_stationen(p):
    L, Wk, Ws, Wt = p['L'], p['Wk'], p['Ws'], p['Wt']
    yb = p['y_gaumen']
    xh = p.get('x_hinten', -220.0)
    xs = [xh, 60.0, 360.0, 700.0, 700 + 0.35 * (L - 700), 700 + 0.7 * (L - 700), L - 110.0, L]
    ws = [0.82 * Wk, Wk, 0.97 * Wk, Ws, Ws + 0.3 * (Wt - Ws), Ws + 0.75 * (Wt - Ws), Wt, 0.62 * Wt]
    yts = [p['yt_k'] * 0.85, p['yt_k'], p['yt_k'] * 0.97, p['yt_s'], p['yt_s'] + 0.4 * (p['yt_t'] - p['yt_s']),
           p['yt_s'] + 0.8 * (p['yt_t'] - p['yt_s']), p['yt_t'] - 30.0, p['yt_t'] * 0.55]
    ybs = [yb - 40, yb, yb, yb, yb, yb, yb - 10, yb - 40]
    return xs, ws, yts, ybs


def kopf(p):
    """Knochen 22: Schaedel ab x_hinten (steckt im Hals), Schnauze bis L. Zaehne oben an der Unterkante."""
    if p.get('art') == 'kroko':
        return kopf_kroko(p)
    L, Wk = p['L'], p['Wk']
    xs, ws, yts, ybs = kopf_stationen(p)
    yb = p['y_gaumen']
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([ring_oben(w, a, b) for w, a, b in zip(ws, yts, ybs)])
    o = rohr(Z, ea, eb, R, ATLAS['kopf'], spitze0=(xs[0] - 110.0, (yts[0] + ybs[0]) / 2, 0.0), spitze1=(L + 60.0, (yts[-1] + ybs[-1]) / 2 + 20, 0.0))
    if p.get('augen'):
        # Augenwuelste oben hinter der Schnauze (Krokodil): zwei flache Hoecker
        for vz in (-1, 1):
            o.dazu(zahn((520.0, p['yt_k'] * 0.9 + 60, vz * 0.55 * Wk), (0.25, -1, vz * 0.3), 150.0, 150.0, ATLAS['auge']))
    n = int(p['zaehne'])
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = 760 + t * (L - 160 - 760)
            w = np.interp(x, xs, ws)
            lang = p['zahn_lang'] * (1.25 if i in (1, n - 2) else 1.0)
            o.dazu(zahn((x, yb - 25, vz * 0.86 * w), (0.15, 1, vz * 0.12), lang, 38.0, ATLAS['zahn']))
    return o


def kiefer_stationen(p):
    L, Ws, Wt = p['L'], p['Ws'], p['Wt']
    yr = p['y_gaumen'] - 400.0 + 15.0
    yb = yr + p['kiefer_boden'] + 120
    Lj = L - 500.0 - p['kiefer_kuerzer']
    xs = [-260.0, -60.0, 200.0, 200 + 0.4 * (Lj - 200), 200 + 0.8 * (Lj - 200), Lj - 90, Lj]
    ws = [0.62 * Ws, 0.9 * Ws, 0.95 * Ws, 0.93 * (Ws + 0.4 * (Wt - Ws)), 0.93 * (Ws + 0.85 * (Wt - Ws)), 0.92 * Wt, 0.6 * Wt]
    ybs = [yb + 40, yb + 30, yb, yb - 30, yb - 60, yb - 80, yr + 60]
    tf = [40.0, p['kiefer_tief'], p['kiefer_tief'], p['kiefer_tief'], p['kiefer_tief'] * 0.8, p['kiefer_tief'] * 0.5, 20.0]
    return xs, ws, ybs, tf, yr, Lj


def kiefer(p):
    """Knochen 23 (Ursprung = Gelenk bei Kopf (500, 400)): Trog bis Kinn. Rand liegt geschlossen am Gaumen."""
    if p.get('art') == 'kroko':
        return kiefer_kroko(p)
    xs, ws, ybs, tf, yr, Lj = kiefer_stationen(p)
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([ring_kiefer(w, yr, b, t) for w, b, t in zip(ws, ybs, tf)])
    o = rohr(Z, ea, eb, R, ATLAS['kiefer'], spitze0=(-330.0, yr + 90, 0.0), spitze1=(Lj + 50.0, yr + 40, 0.0))
    n = int(p['zaehne'])
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = 260 + t * (Lj - 120 - 260)
            w = np.interp(x, xs, ws)
            lang = p['zahn_lang'] * (1.2 if i in (0, n - 1) else 0.9)
            o.dazu(zahn((x, yr + 20, vz * 0.84 * w), (0.1, -1, vz * 0.1), lang, 34.0, ATLAS['zahn']))
    return o


# ─────────────────────────────── Krallen
def hakenkralle(wurzel, richtung, unten, lang, dick, krumm, rect, u_spanne=(0.0, 1.0), N=6, S=5):
    """Hakenkralle: seitlich flach (Breite 0,6 der Hoehe), Wurzel bei `wurzel`, laeuft `richtung`, kruemmt um `krumm` Grad zur Seite `unten`."""
    d0 = _einheit(richtung)
    un = np.asarray(unten, float)
    un = _einheit(un - (un @ d0) * d0)
    zent, EA, EB, R = [], [], [], []
    p = np.asarray(wurzel, float)
    ds = lang / (S - 1)
    for s in range(S):
        t = s / (S - 1)
        ang = math.radians(krumm) * t
        d = math.cos(ang) * d0 + math.sin(ang) * un
        b = _einheit(un - (un @ d) * d)
        a = np.cross(d, b)
        r = dick * (1.0 - 0.8 * t ** 1.1)
        zent.append(p.copy())
        EA.append(a)
        EB.append(b)
        R.append([(0.6 * r * math.cos(2 * math.pi * j / N), r * math.sin(2 * math.pi * j / N)) for j in range(N)])
        p = p + ds * d
    ang = math.radians(krumm)
    spitze = zent[-1] + 0.6 * ds * (math.cos(ang) * d0 + math.sin(ang) * un)
    return rohr(zent, EA, EB, R, rect, spitze0=None, spitze1=spitze, u_spanne=u_spanne)


# ─────────────────────────────── Krokodilkopf (Nachbesserung 2, E22)
# Nutzer (E22): "Gesicht immer noch zu rundlich ... vom Oberkiefer her ... eher krokodilmaessig, hier eher hippomaessig". K3 lief auf eine
# vorn BREITERE Schnauze (Wt > Ws). Die Kopfart 'kroko' ist ein Keil: Schaedel hinten breit und hoch (Augenwuelste oben), Schnauze lang, flach,
# in der Draufsicht gerade zur Spitze verjuengt (Wt/Wk 0,30-0,50; Proportion nach RE2 EM23 Mesh 7: Spitze/groesste Breite 0,40, nur Anatomie),
# flache Oberseite ohne Kuppel, Nasenhoecker an der Spitze, Oberkiefer seitlich ueber dem Unterkiefer (Lippenrand breiter als der Kiefer),
# gezackte Zahnreihe am Lippenrand nach unten aussen, Unterkiefer V-foermig mit Zaehnen nach oben. Kein RE-Wert: Gestaltung (PORT-WAHL).
VORGABE_KROKO = dict(art='kroko', L=1800.0, Wk=360.0, taper=0.38, x_hinten=-250.0, x_auge=520.0, yt_k=-230.0, yt_s=-120.0, yt_t=-10.0,
                     y_gaumen=270.0, nase=60.0, augen=1, auge_hoch=140.0, zaehne=11, zahn_lang=150.0, kiefer_kuerzer=60.0,
                     kiefer_boden=260.0, kiefer_boden_t=120.0, kiefer_breit=0.86, kiefer_tief=110.0, uk_zaehne=10, bulbe=1.12, zahn_oben=0.75)


def ring_kroko(w, yt, yb):
    """Querschnitt Oberkiefer/Schaedel (z quer, y unten), 10 Ecken wie ring_oben (0 Gaumenmitte, 5 Scheitel): Lippenrand am breitesten,
    senkrechte Lippenwand, Flanke schraeg nach innen, flache Oberseite (Ecken 4/5/6 auf einer Hoehe) - von vorn ein schmales Trapez, keine Kuppel."""
    h = yb - yt
    halb = [(0.0, yb), (0.78 * w, yb), (w, yb - 0.10 * h), (0.93 * w, yb - 0.42 * h), (0.62 * w, yt), (0.0, yt)]
    return [(z, y) for z, y in halb] + [(-z, y) for z, y in halb[-2:0:-1]]


def kroko_stationen(p):
    """Stationen x, halbe Breite w, Oberkante yt, Unterkante yb des Oberkopfs. Schaedel x_hinten..x_auge+200 (Augenwuelste bei x_auge),
    Schnauze bis L: Breite linear von 0,92 Wk auf taper*Wk, Hoehe linear von yt_s auf yt_t, Nasenhoecker kurz vor der Spitze."""
    L, Wk, xa = p['L'], p['Wk'], p['x_auge']
    yb = p['y_gaumen']
    Wt = p['taper'] * Wk
    if p.get('schaedel') is not None:
        # Schaedeldach relativ zur Schnauze (Krokodil: Schaedel hoeher als der Schnauzenansatz; Fit K7 legte ihn sonst tiefer, _lauf/K7)
        p = dict(p, yt_k=p['yt_s'] - p['schaedel'])
    xs0 = xa + 220.0
    xh = p['x_hinten']
    sn = lambda t: xs0 + t * (L - 140.0 - xs0)
    xs = [xh, xh + 0.45 * (xa - xh), xa, xs0, sn(0.33), sn(0.66), sn(0.86), L - 140.0, L - 40.0, L]
    ws = [0.80 * Wk, 0.97 * Wk, Wk, 0.92 * Wk]
    for t in (0.33, 0.66, 0.86):
        ws.append(0.92 * Wk + t * (Wt - 0.92 * Wk))
    bu = p.get('bulbe', 1.12)
    ws += [Wt * bu, Wt * 0.85 * bu, Wt * 0.5]          # Nasenrosette (Faktor bulbe breiter), dann runde Spitze
    yts = [p['yt_k'] * 0.80, p['yt_k'], p['yt_k'] * 0.96, p['yt_s']]
    # Nasenhoecker als weiche Kuppe ueber die letzten Stationen (Anteil 0 / 0 / 0,35 / 1 / 0,75), nicht als Stufe (eine Stufe von voller
    # Hoehe an der vorletzten Station stand im Fit K6 als senkrechte Platte an der Spitze, _lauf/K6.log)
    for t, h in ((0.33, 0.0), (0.66, 0.0), (0.86, 0.35)):
        yts.append(p['yt_s'] + t * (p['yt_t'] - p['yt_s']) - h * p['nase'])
    yts += [p['yt_t'] - p['nase'], p['yt_t'] - 0.75 * p['nase'], p['yt_t'] * 0.45 - 0.3 * p['nase']]
    ybs = [yb - 60, yb, yb, yb, yb, yb, yb, yb, yb - 15, yb - 50]
    return xs, ws, yts, ybs


def kopf_kroko(p):
    if p.get('schaedel') is not None:
        p = dict(p, yt_k=p['yt_s'] - p['schaedel'])
    xs, ws, yts, ybs = kroko_stationen(p)
    L, Wk = p['L'], p['Wk']
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([ring_kroko(w, a, b) for w, a, b in zip(ws, yts, ybs)])
    o = rohr(Z, ea, eb, R, ATLAS['kopf'], spitze0=(xs[0] - 120.0, (yts[0] + ybs[0]) / 2, 0.0),
             spitze1=(L + 40.0, (yts[-1] + ybs[-1]) / 2 + 30, 0.0), bogen=True)
    if p.get('augen'):
        # Augenwuelste: flache Hoecker hinten oben auf dem Schaedel, Kante nach aussen/oben (wie beim Krokodil ueber der Ebene der Schnauze)
        for vz in (-1, 1):
            o.dazu(wulst((p['x_auge'], p['yt_k'] + 30.0, vz * 0.52 * Wk), 300.0, 150.0, p['auge_hoch'], ATLAS['auge']))
    n = int(p['zaehne'])
    x0, x1 = p['x_auge'] + 80.0, L - 120.0
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = x0 + t * (x1 - x0)
            w = np.interp(x, xs, ws)
            gross = i in (2, 3, n - 3) or (i % 3 == 0)
            lang = p['zahn_lang'] * p.get('zahn_oben', 1.0) * (1.3 if gross else 0.8)
            o.dazu(zahn((x, p['y_gaumen'] - 30, vz * 0.93 * w), (0.12, 1, vz * 0.22), lang, 40.0 if gross else 32.0, ATLAS['zahn']))
    return o


def wulst(mitte, lang, breit, hoch, rect):
    """Augenwulst: flacher Hoecker (Pyramidenstumpf aus 5 Stationen), lang in x, Spitze nach oben (-y)."""
    mitte = np.asarray(mitte, float)
    xs = [-0.5, -0.25, 0.05, 0.3, 0.5]
    hs = [0.0, 0.75, 1.0, 0.7, 0.0]
    bs = [0.35, 0.85, 1.0, 0.8, 0.3]
    Z, EA, EB, R = [], [], [], []
    for x, h, b in zip(xs, hs, bs):
        Z.append(mitte + np.array([x * lang, 0, 0]))
        EA.append([0, 0, 1.0])
        EB.append([0, 1.0, 0])
        r = []
        for j in range(6):
            a = 2 * math.pi * j / 6
            z = 0.5 * breit * b * math.cos(a)
            y = (-hoch * h * max(0.0, -math.sin(a)) + 40.0 * max(0.0, math.sin(a)))
            r.append((z, y))
        R.append(r)
    return rohr(Z, EA, EB, R, rect)


def kiefer_kroko_stationen(p):
    """Unterkiefer im Raum von Knochen 23 (Ursprung = Gelenk, Kopf (500, 400)): Rand bei yr (geschlossen 15 unter dem Gaumen), Breite
    kiefer_breit x Breite des Oberkiefers an derselben Stelle (Oberkiefer greift seitlich ueber), Boden tief hinten, flach vorn."""
    L = p['L']
    yr = p['y_gaumen'] - 400.0 + 15.0
    Lj = L - 500.0 - p['kiefer_kuerzer']
    xo, wo, _yt, _yb = kroko_stationen(p)
    breite = lambda xk: p['kiefer_breit'] * float(np.interp(xk + 500.0, xo, wo))
    xs = [-330.0, -120.0, 150.0, 150 + 0.33 * (Lj - 150), 150 + 0.66 * (Lj - 150), Lj - 220, Lj - 80, Lj]
    ws = [0.70 * breite(-120.0), breite(-120.0), breite(150.0)] + [breite(x) for x in xs[3:7]] + [0.55 * breite(Lj - 80)]
    bt = lambda t: yr + p['kiefer_boden'] + t * (p['kiefer_boden_t'] - p['kiefer_boden'])
    ybs = [yr + p['kiefer_boden'] * 0.9, bt(0.0), bt(0.1), bt(0.4), bt(0.7), bt(0.9), bt(1.0), yr + 0.5 * p['kiefer_boden_t']]
    tf = [40.0, p['kiefer_tief'], p['kiefer_tief'], p['kiefer_tief'], 0.8 * p['kiefer_tief'], 0.6 * p['kiefer_tief'], 0.4 * p['kiefer_tief'], 20.0]
    return xs, ws, ybs, tf, yr, Lj


def kiefer_kroko(p):
    xs, ws, ybs, tf, yr, Lj = kiefer_kroko_stationen(p)
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([ring_kiefer(w, yr, b, t) for w, b, t in zip(ws, ybs, tf)])
    o = rohr(Z, ea, eb, R, ATLAS['kiefer'], spitze0=(-400.0, yr + 120, 0.0), spitze1=(Lj + 40.0, yr + 30, 0.0), bogen=True)
    n = int(p['uk_zaehne'])
    for vz in (-1, 1):
        for i in range(n):
            t = (i + 0.5) / n
            x = 220 + t * (Lj - 110 - 220)
            w = np.interp(x, xs, ws)
            gross = i in (1, n - 4) or (i % 3 == 1)
            lang = p['zahn_lang'] * (1.15 if gross else 0.75)
            o.dazu(zahn((x, yr + 15, vz * 0.88 * w), (0.1, -1, vz * 0.18), lang, 36.0 if gross else 30.0, ATLAS['zahn']))
    return o
