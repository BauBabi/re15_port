#!/usr/bin/env python3
"""Form bemalt, Formeingriffe vor und nach dem Glaetten (E4, A1). Alles im Knochenraum des Teils, Mesh i bleibt an Knochen i.

1. a1_wurzel: Auflage A1 (rohbau_abnahme_3.md): s1 vor der Hueftmitte in den Rumpf. Ringe von schwanz1 mit t < T0 sinken mit Rampe um bis zu A1_TIEFE nach
   Knochen +Y (in Pose 1 ist das 0,935 der Senkrechten). T0 = -60: dort liegt in Pose 1 Aktor-x 13, die Hueftmitte. Gemessen in Abnahme 3 (gratprobe):
   Senken ab Aktor-x 0 um 400 kostet 6 Zielpixel, ab -200 dagegen 258. Die Rampe beginnt deshalb hinter der Hueftmitte nicht.
2. ketten: Querschnitt an den Gelenken angleichen (E4: keine aufgereihten Wuelste). Ringe des Kinds vor dem Gelenk (t <= 0) bekommen den Querschnitt des
   Elternteils an derselben Stelle, Ringe des Elternteils hinter dem Gelenk den des Kinds. In der Rastlage (Knochen achsparallel) liegen beide dann
   aufeinander, gebeugt bleibt eine Falte statt eines Knubbels.
3. schulter: Schultermasse an der Brust (E4: keine Kugel-Oberarme). Je Brustring die konvexe Huelle aus Ring und einer Kugel (Radius 400) an der Lage der
   Oberarm-Masse, nur oben und seitlich: die Brust umschliesst die Schulter, die Falte zwischen Kugel und Rumpf faellt weg, der Arm waechst aus der Schulter. Lage aus den drei Clips gemessen (Brustraum, Oberarmring t 60):
   Pose 1 (683, 87, -925), Pose 2 (723, 213, -765), Ruhe (617, 162, -923).
Frei gesetzt (kein Bildbeleg): Rampenlaenge, Wulstgroesse, Wulstbreite. Kein RE-Wert."""
import math

import numpy as np

A1_T0, A1_LANG, A1_TIEFE = -60.0, 260.0, 430.0
# Nur der Schwanz: Knie und Ellbogen sind in Pose 1 und Ruhe stark gebeugt, ein angeglichener Kindring steht dort als Keil aus der Kniekehle (g2: SCH 19 px, Knochen 26)
KETTE_VOR, KETTE_F, KETTE_ZONE = 45.0, 0.8, 260.0      # m20: 0,98 lag deckungsgleich, der Maler zeichnete Saegezaehne am Gelenk; m21 0,9 noch Zaehne
KETTEN = [('schwanz1', 'schwanz2'), ('schwanz2', 'schwanz3'), ('schwanz3', 'schwanz4'), ('schwanz4', 'schwanz5'), ('schwanz5', 'schwanz6')]
SCHULTER = dict(mitte=(680.0, 130.0, 840.0), radius=430.0, laenge=560.0, unten=0.55)


def glatt(x):
    x = np.clip(x, 0.0, 1.0)
    return x * x * (3 - 2 * x)


def a1_wurzel(p, ring, k0, k1):
    if p.name != 'schwanz1':
        return ring, k0, k1
    ring = ring.copy()
    t = (ring.mean(1) - p.o) @ p.d
    for i in range(len(ring)):
        s = A1_TIEFE * glatt((A1_T0 - t[i]) / A1_LANG)
        ring[i] += np.array([0, s, 0])
    return ring, k0 + np.array([0, A1_TIEFE, 0]), k1


OBERARM_KOPF = {0: 0.62, 1: 0.74, 2: 0.88}     # Ring -> Faktor: die Kugel am Schulterende wird kleiner, ihr Platz gehoert der Schultermasse der Brust


def oberarm(p, ring, k0, k1):
    """E4: keine Kugel-Oberarme. Die ersten drei Ringe des Oberarms (t -270, -130, 60) schrumpfen um ihre Mitte, der Arm wird zum Kegel, der aus der Schulter waechst."""
    if not p.name.startswith('oberarm'):
        return ring, k0, k1
    ring = ring.copy()
    for i, f in OBERARM_KOPF.items():
        c = ring[i].mean(0)
        ring[i] = c + (ring[i] - c) * f
    c0 = ring[0].mean(0)
    k0 = c0 + (k0 - c0) * 0.8
    return ring, k0, k1


# Auftrag 2 / rohbau_abnahme_3 Abschnitt 4: Spitzen der linken Klaue in Bild 2 je rund 7 px zu kurz (439,418) und (468,417).
# Verschiebung der Kegelspitze im Knochenraum, gerechnet mit zw/dbg_klaue.py: Spitze projiziert auf das Ziel plus 2 px in Klauenrichtung (die letzten Pixel eines Kegels zeichnet der Rasterer nicht).
KLAUE_LINKS = {'klaue_l2': (53, -17, 18), 'kralle_l': (101, -12, -108)}      # mit Bodenbedingung: Spitze in Pose 1 und Ruhe mindestens 25 ueber dem tiefsten anderen Punkt (g16 ohne: Bodenhub 4 px in Bild 1)


SCHWANZ_RUND = 0.45     # Anteil der Ecken-Abweichung vom Ringmittel, der bleibt (m22: der Kiel unten, Ecke 0 bis 432 gegen 308 seitlich, stand an jedem Gelenk als Zahn)


def schwanz_rund(p, ring, k0, k1):
    """E4: Schwanz glatt. Querschnitt runder: jede Ecke ruecken zum Ringmittel, Lage der Ringmitte bleibt. Die Nachpassung stellt den Umriss in Bild 1 wieder her."""
    if not p.name.startswith('schwanz'):
        return ring, k0, k1
    ring = ring.copy()
    for i in range(len(ring)):
        c = ring[i].mean(0)
        q = ring[i] - c
        r = np.linalg.norm(q, axis=1)
        rm = r.mean()
        ring[i] = c + q * ((rm + SCHWANZ_RUND * (r - rm)) / np.maximum(r, 1e-6))[:, None]
    return ring, k0, k1


def klaue_links(p, ring, k0, k1):
    if p.name in KLAUE_LINKS:
        k1 = k1 + np.array(KLAUE_LINKS[p.name], float)
    return ring, k0, k1


def _schnitt(ring, o, d, t):
    """Querschnitt eines Teils an Station t (linear zwischen den Ringen, an den Enden festgehalten). -> Polygon [n,3]."""
    st = (ring.mean(1) - o) @ d
    if t <= st[0]:
        return ring[0]
    if t >= st[-1]:
        return ring[-1]
    i = int(np.searchsorted(st, t)) - 1
    f = (t - st[i]) / max(st[i + 1] - st[i], 1e-6)
    return ring[i] * (1 - f) + ring[i + 1] * f


def _strahl(poly, d, c, u):
    """Schnitt des Strahls c + s*u (u senkrecht zu d) mit dem Polygon (in die Ebene senkrecht zu d projiziert). -> Punkt."""
    e1 = u
    e2 = np.cross(d, u)
    q = poly - c
    x, y = q @ e1, q @ e2
    best = None
    n = len(poly)
    for i in range(n):
        x0, y0, x1, y1 = x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]
        if (y0 > 0) == (y1 > 0) and y0 != 0 and y1 != 0:
            continue
        if y1 == y0:
            continue
        f = y0 / (y0 - y1)
        xs = x0 + f * (x1 - x0)
        if xs > 0 and (best is None or xs < best):
            best = xs
    if best is None:
        best = float(np.max(np.linalg.norm(q - (q @ d)[:, None] * d, axis=1)))
    return c + best * u


def _angleichen(ziel_ring, quelle, o_q, d_q, verschiebung):
    """Ring des Ziels (im Raum der Quelle um `verschiebung` versetzt) auf den Querschnitt der Quelle setzen."""
    P = ziel_ring + verschiebung
    c = P.mean(0)
    t = (c - o_q) @ d_q
    poly = _schnitt(quelle, o_q, d_q, t)
    aus = []
    for p_ in P:
        u = p_ - c
        u = u - (u @ d_q) * d_q
        nu = np.linalg.norm(u)
        if nu < 1e-6:
            aus.append(p_)
            continue
        u /= nu
        aus.append(_strahl(poly, d_q, c, u))
    return np.array(aus) - verschiebung


def _rmit(ring_):
    c = ring_.mean(0)
    return float(np.linalg.norm(ring_ - c, axis=1).mean())


def _skal(ring_, f):
    c = ring_.mean(0)
    return c + (ring_ - c) * f


def schwanz_profil(teile, ringe, g):
    """E4: ein durchgehender Schwanz. Alle Ringe von schwanz1 (ab t 0, die Wurzel davor bleibt A1) bis schwanz6 in Rastlage hintereinander, je Ecke Abstand zur
    Kettenachse und Ringmitte als Funktion der Laenge, mit einem Glaettungsspline (scipy) geglaettet und zurueck. Danach traegt jede Ecke den Verlauf der ganzen Kette,
    nicht mehr den Bauch ihres Glieds. m22/m23: ohne das lagen die Glieder als Wuerste hintereinander."""
    from scipy.interpolate import UnivariateSpline
    idx = {p.name: i for i, p in enumerate(teile)}
    namen_ = ['schwanz%d' % k for k in range(1, 7)]
    off = {}
    o = np.zeros(3)
    for k, nm in enumerate(namen_):
        if k > 0:
            o = o + np.array(g['knochen'][teile[idx[nm]].knochen]['versatz'], float)
        off[nm] = o.copy()
    p1 = teile[idx['schwanz1']]
    d = p1.d
    ea, eb = p1.ea, p1.eb
    eintraege = []
    for nm in namen_:
        p = teile[idx[nm]]
        R = ringe[idx[nm]][0]
        for i in range(len(R)):
            P = R[i] + off[nm]
            c = P.mean(0)
            t = float((c - p1.o) @ d)
            if nm == 'schwanz1' and float((R[i].mean(0) - p.o) @ p.d) < 0:
                continue
            q = P - c
            eintraege.append((t, nm, i, float(c @ ea), float(c @ eb), q @ ea, q @ eb))
    eintraege.sort(key=lambda e: e[0])
    ts = np.array([e[0] for e in eintraege])
    # gleiche Stationen (Ueberlappung) leicht auseinander, Spline braucht steigende x
    for k in range(1, len(ts)):
        if ts[k] <= ts[k - 1]:
            ts[k] = ts[k - 1] + 1.0
    n = len(eintraege[0][5])
    ang = np.arctan2(np.array([e[6] for e in eintraege]), np.array([e[5] for e in eintraege]))
    rad = np.hypot(np.array([e[5] for e in eintraege]), np.array([e[6] for e in eintraege]))
    ca = np.array([e[3] for e in eintraege])
    cb = np.array([e[4] for e in eintraege])
    w = np.ones(len(ts))

    def gl(y, s_):
        sp = UnivariateSpline(ts, y, w=w, k=3, s=s_ * len(ts))
        return sp(ts)
    # m25: Glaetten nahm auch die Baeuche weg, Bild 1 verlor die Schwanzbreite (IoU 0,977). Darum fuellt der Spline nur die Taillen an den Gelenken
    # (Radius = max(alt, Spline)), die Mitten wandern nur schwach.
    ca2, cb2 = gl(ca, 20.0 ** 2), gl(cb, 12.0 ** 2)
    rad2 = np.stack([np.maximum(rad[:, j], gl(rad[:, j], 22.0 ** 2)) for j in range(n)], 1)
    for k, e in enumerate(eintraege):
        t, nm, i = e[0], e[1], e[2]
        P_alt = ringe[idx[nm]][0][i] + off[nm]
        c_alt = P_alt.mean(0)
        c_neu = c_alt + (ca2[k] - ca[k]) * ea + (cb2[k] - cb[k]) * eb
        P_neu = c_neu + (rad2[k] * np.cos(ang[k]))[:, None] * ea + (rad2[k] * np.sin(ang[k]))[:, None] * eb + ((P_alt - c_alt) @ d)[:, None] * d
        ringe[idx[nm]][0][i] = P_neu - off[nm]
    return ringe


def ketten(teile, ringe, g):
    """ringe: Liste [ring, k0, k1] je Teil (wird veraendert). Rastlage: Kind = Elternraum + Versatz des Kindknochens.
    m18: Mit 'Kind = Querschnitt des Elternteils, Eltern waechst nur' blieb an jedem Gelenk eine Stufe im Umriss (Glied 3 endet mit 255, Glied 4 beginnt mit 223).
    Jetzt treffen sich beide am Gelenk im Mittel r_g beider Radien; in KETTE_ZONE Einheiten davor und danach laufen die Ringe weich darauf zu."""
    idx = {p.name: i for i, p in enumerate(teile)}
    for a, b in KETTEN:
        A, B = teile[idx[a]], teile[idx[b]]
        RA, RB = ringe[idx[a]][0].copy(), ringe[idx[b]][0].copy()
        v = np.array(g['knochen'][B.knochen]['versatz'], float)       # Kindursprung im Elternraum
        tB = (RB.mean(1) - B.o) @ B.d
        tA = (RA.mean(1) - A.o) @ A.d
        tj = (v - A.o) @ A.d
        rA = _rmit(_schnitt(RA, A.o, A.d, tj))
        rB = _rmit(_schnitt(RB, B.o, B.d, 0.0))
        rg = 0.5 * (rA + rB)
        nB, nA = RB.copy(), RA.copy()
        for i in range(len(RB)):
            if tB[i] <= 0:
                # Ring naeher ans Gelenk (KETTE_VOR): gebeugt dreht er sonst aus dem Elternteil (g11: Bild 1 Schwanzkante y 342 um 5,5 px zu weit)
                Ri = RB[i] + (max(tB[i], -KETTE_VOR) - tB[i]) * B.d
                q_ = _angleichen(Ri, RA, A.o, A.d, v)
                nB[i] = _skal(q_, rg * KETTE_F / max(_rmit(q_), 1.0))
            elif tB[i] < KETTE_ZONE:
                w = glatt(1 - tB[i] / KETTE_ZONE)
                nB[i] = _skal(RB[i], 1 + w * (rg / max(_rmit(RB[i]), 1.0) - 1))
        for i in range(len(RA)):
            if tA[i] > tj:
                q_ = _angleichen(RA[i], RB, B.o, B.d, -v)
                nA[i] = _skal(q_, rg * KETTE_F / max(_rmit(q_), 1.0))
            elif tA[i] > tj - KETTE_ZONE:
                w = glatt(1 - (tj - tA[i]) / KETTE_ZONE)
                nA[i] = _skal(RA[i], 1 + w * (rg / max(_rmit(RA[i]), 1.0) - 1))
        ringe[idx[a]][0], ringe[idx[b]][0] = nA, nB
    return ringe


def _huelle_radius(P2, c, u):
    """Konvexe Huelle der Punkte P2 [k,2]; Abstand von c entlang u bis zum Huellenrand."""
    from scipy.spatial import ConvexHull
    h = ConvexHull(P2)
    best = None
    for s_ in h.simplices:
        a_, b_ = P2[s_[0]] - c, P2[s_[1]] - c
        e = b_ - a_
        M = np.array([[u[0], -e[0]], [u[1], -e[1]]])
        if abs(np.linalg.det(M)) < 1e-9:
            continue
        t_, f_ = np.linalg.solve(M, a_)
        if t_ > 0 and -1e-6 <= f_ <= 1 + 1e-6:
            best = t_ if best is None else max(best, t_)
    return best


def schulter(name, pts, m, NR):
    """Verformung nach dem Glaetten: die Brust umschliesst oben und seitlich die Schultermasse (konvexe Huelle aus Brustring und Schulterkugel je Ring).
    pts im Brustraum (x laengs, y unten, z links). Nur obere und seitliche Ecken, laengs mit weichem Ende."""
    if name != 'brust':
        return pts
    pts = pts.copy()
    Sx, Sy, Sz = SCHULTER['mitte']
    Rs, W = SCHULTER['radius'], SCHULTER['laenge']
    for r in range(NR):
        R = pts[r * m:(r + 1) * m]
        x = float(R[:, 0].mean())
        wx = glatt(1 - abs(x - Sx) / W)
        if wx <= 0 or abs(x - Sx) >= Rs:
            continue
        rs = math.sqrt(Rs * Rs - (x - Sx) ** 2)
        c = R[:, 1:].mean(0)
        kreis = []
        for s in (1.0, -1.0):
            for k in range(24):
                w = 2 * math.pi * k / 24
                kreis.append([Sy + rs * math.cos(w), s * Sz + rs * math.sin(w)])
        P2 = np.concatenate([R[:, 1:], np.array(kreis)])
        for j in range(m):
            v = R[j, 1:] - c
            r0 = np.linalg.norm(v)
            if r0 < 1e-6:
                continue
            u = v / r0
            sek = glatt((SCHULTER['unten'] - u[0]) / 0.7)     # u[0] = y-Anteil: oben (-1) und seitlich (0) voll, ab unten aus
            if sek <= 0:
                continue
            rh = _huelle_radius(P2, c, u)
            if rh is None or rh <= r0:
                continue
            R[j, 1:] = c + u * (r0 + wx * sek * (rh - r0))
        pts[r * m:(r + 1) * m] = R
    return pts


def normalen_ketten(teile, Gs, g):
    """Schwanz glatt (E4): an jedem Gelenk der Kette bekommen Endring des Elternteils und Anfangsring des Kinds dieselben Normalen (Mittel, Rastlage).
    Gemessen vorher (zw, m18): Achsanteil der Normalen springt am Gelenk 4/5 von +0,12 auf -0,04, das las als Stufe im Licht."""
    idx = {p.name: i for i, p in enumerate(teile)}
    for a, b in KETTEN:
        A, B = Gs[idx[a]], Gs[idx[b]]
        pa, pb = teile[idx[a]], teile[idx[b]]
        v = np.array(g['knochen'][pb.knochen]['versatz'], float)
        ma, mb = A['m'], B['m']
        ka = [r for r in range(A['NR']) if A['rolle'][r] == 'k'][-1]
        kb = [r for r in range(B['NR']) if B['rolle'][r] == 'k'][0]
        Pa = A['pts'][ka * ma:(ka + 1) * ma]
        Pb = B['pts'][kb * mb:(kb + 1) * mb] + v
        Na = A['nrm'][ka * ma:(ka + 1) * ma]
        Nb = B['nrm'][kb * mb:(kb + 1) * mb]
        ca, cb = Pa.mean(0), Pb.mean(0)
        da = (Pa - ca) / np.linalg.norm(Pa - ca, axis=1, keepdims=True)
        db = (Pb - cb) / np.linalg.norm(Pb - cb, axis=1, keepdims=True)
        # je Ecke des einen Rings die Normale des anderen an derselben Winkelstelle (naechste Richtung, gewichtet mit Nachbar)
        def am(dq, dz, Nz):
            s_ = dq @ dz.T
            j = np.argsort(-s_, axis=1)[:, :2]
            w = np.take_along_axis(s_, j, 1).clip(0, None) + 1e-6
            return (Nz[j] * w[..., None]).sum(1) / w.sum(1, keepdims=True)
        neu_a = Na + am(da, db, Nb)
        neu_b = Nb + am(db, da, Na)
        A['nrm'][ka * ma:(ka + 1) * ma] = neu_a / np.linalg.norm(neu_a, axis=1, keepdims=True)
        B['nrm'][kb * mb:(kb + 1) * mb] = neu_b / np.linalg.norm(neu_b, axis=1, keepdims=True)
