#!/usr/bin/env python3
"""Zusammenhang, Loecher, Gelenkkugel, Biegeprobe, Stand.

Loch = Pixel, das mit allen Flaechen bedeckt ist, mit Rueckseiten-Wegwurf (wie im Spiel) aber nicht: dort sieht man durch die Figur.
Zerfall = Silhouette aus einer Richtung zerfaellt in mehrere Stuecke: Teile haengen nicht zusammen.
"""
import json, math, os, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
import geo2 as G2
import torch
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')

# 14 Richtungen: 6 Achsen, 8 Ecken
RICHT = [(1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1)] + [(a, b, c) for a in (1, -1) for b in (1, -1) for c in (1, -1)]
GROSS = 320


def pose_frei(f, ci, extra=None):
    """Wie FE.pose_aus_bild in Gleitkomma, extra = {knochen: 3x3 lokal zusaetzlich}"""
    kn = f['knochen']
    b = f['clips'][ci]['bilder'][0]
    k = f['pool'][b['kf']]
    import emd_ansichtsblatt as EA
    R, T = [None] * len(kn), [None] * len(kn)
    for i, x in enumerate(kn):
        loc = EA.mat_euler(*k['winkel'][i]).astype(float) / 4096.0
        if extra and i in extra:
            loc = extra[i] @ loc
        p = x['eltern']
        if p < 0:
            R[i], T[i] = loc, np.array(k['wurzel'], float)
        else:
            R[i] = R[p] @ loc
            T[i] = R[p] @ np.array(x['versatz'], float) + T[p]
    return R, T


class Netz:
    def __init__(self, f):
        self.f = f
        self.v, self.t = [], []
        for b in range(len(f['knochen'])):
            v, n, t, nt = G.mesh_arrays(f, b)
            self.v.append(v)
            self.t.append(t)

    def alles(self, R, T, teile=None):
        V, Fa, Bi = [], [], []
        o = 0
        for b in (teile if teile is not None else range(len(self.v))):
            if not len(self.t[b]):
                continue
            V.append(self.v[b] @ R[b].T + T[b])
            Fa.append(self.t[b] + o)
            Bi.append(np.full(len(self.t[b]), b))
            o += len(self.v[b])
        return np.vstack(V), np.vstack(Fa), np.concatenate(Bi)


def achsen(d):
    d = np.array(d, float)
    d /= np.linalg.norm(d)
    h = np.array([0, 1.0, 0]) if abs(d[1]) < 0.9 else np.array([1.0, 0, 0])
    r = np.cross(h, d); r /= np.linalg.norm(r)
    u = np.cross(d, r)
    return r, u, d


def bilder(V, Fa, d, m, s):
    """Bedeckung mit und ohne Rueckseiten. s = Pixel je Einheit."""
    r, u, dd = achsen(d)
    Q = (V - m)
    x = (Q @ r) * s + GROSS / 2
    y = (Q @ u) * s + GROSS / 2
    P = np.stack([x, y], 1)
    a, b, c = P[Fa[:, 0]], P[Fa[:, 1]], P[Fa[:, 2]]
    ar = (b[:, 0] - a[:, 0]) * (c[:, 1] - a[:, 1]) - (c[:, 0] - a[:, 0]) * (b[:, 1] - a[:, 1])
    # Aussenseite bestimmen: Windung gegen Aussennormale, Vorzeichen fest aus dem Format
    n = np.cross(V[Fa[:, 1]] - V[Fa[:, 0]], V[Fa[:, 2]] - V[Fa[:, 0]])
    vorn = (n @ dd) > 0          # cross zeigt gegen die Aussennormale, also zeigt die Aussenseite zur Kamera, wenn cross mit Blickrichtung d laeuft (Blick entlang +d)
    pi = np.round(P * 16).astype(np.int32)
    al = np.zeros((GROSS, GROSS), np.uint8)
    vo = np.zeros((GROSS, GROSS), np.uint8)
    for i in range(len(Fa)):
        p = pi[Fa[i]]
        cv2.fillConvexPoly(al, p, 1, lineType=cv2.LINE_8, shift=4)
        if vorn[i]:
            cv2.fillConvexPoly(vo, p, 1, lineType=cv2.LINE_8, shift=4)
    return al.astype(bool), vo.astype(bool)


def lochzahl(al, vo):
    k = np.ones((3, 3), np.uint8)
    vz = cv2.morphologyEx(vo.astype(np.uint8), cv2.MORPH_CLOSE, k).astype(bool)      # Risse zwischen Dreiecken zaehlen nicht
    loch = al & ~vz
    return int(loch.sum()), loch


def stuecke(al):
    n, lab, st, _ = cv2.connectedComponentsWithStats(al.astype(np.uint8), connectivity=8)
    gr = sorted(st[1:, 4].tolist(), reverse=True)
    return gr


def rundblick(N, R, T, s, m=None, ziel=None):
    V, Fa, Bi = N.alles(R, T)
    if m is None:
        m = (V.max(0) + V.min(0)) / 2
    gl, zer, jed = 0, [], []
    bl = []
    for d in RICHT:
        al, vo = bilder(V, Fa, d, m, s)
        c, lo = lochzahl(al, vo)
        gl += c
        q = stuecke(al)
        jed.append(c)
        if len(q) > 1:
            zer.append((d, q[:4]))
        bl.append((al, vo, lo))
    return gl, zer, jed, bl


def tafel(bl, ziel):
    z = []
    for al, vo, lo in bl:
        a = np.full((GROSS, GROSS, 3), 60, np.uint8)
        a[al] = (120, 120, 120)
        a[vo] = (200, 170, 180)
        a[lo] = (255, 0, 0)
        z.append(a)
    r1 = np.hstack(z[:7]); r2 = np.hstack(z[7:14])
    cv2.imwrite(ziel, cv2.cvtColor(np.vstack([r1, r2]), cv2.COLOR_RGB2BGR))


def rot(ax, g):
    a = np.array(ax, float); a /= np.linalg.norm(a)
    c, s = math.cos(math.radians(g)), math.sin(math.radians(g))
    K = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    return np.eye(3) + s * K + (1 - c) * K @ K


def teilbaum(kn, b):
    s = {b}
    for i in range(b + 1, len(kn)):
        if kn[i]['eltern'] in s:
            s.add(i)
    return s


def gelenkkugeln(f, k, ci=0):
    N = Netz(f)
    R, T = pose_frei(f, ci)
    kn = f['knochen']
    aus = []
    for b in range(1, len(kn)):
        p = T[b]                        # Drehpunkt im Aktorraum
        tb = teilbaum(kn, b)
        Vc, Fc, _ = N.alles(R, T, [b])
        rest = [i for i in range(len(kn)) if i not in tb]
        Vr, Fr, _ = N.alles(R, T, rest)
        wc = float(G2.windung(p[None], Vc, Fc)[0])
        wr = float(G2.windung(p[None], Vr, Fr)[0])
        pc, _ = G2.flaechen_proben(Vc, Fc, 1 / 900.0)
        pr, _ = G2.flaechen_proben(Vr, Fr, 1 / 900.0)
        dc = float(np.linalg.norm(pc - p, axis=1).min())
        dr = float(np.linalg.norm(pr - p, axis=1).min())
        rk = dc if abs(wc) > .5 else -dc
        rr = dr if abs(wr) > .5 else -dr
        aus.append((b, round(rk), round(rr)))
    return aus


def biegen(f, k, winkel=(30,), tafeln=False):
    N = Netz(f)
    kn = f['knochen']
    R0, T0 = pose_frei(f, 2)
    V, _, _ = N.alles(R0, T0)
    m = (V.max(0) + V.min(0)) / 2
    s = GROSS * 0.46 / float(np.linalg.norm(V - m, axis=1).max())
    g0, z0, _, _ = rundblick(N, R0, T0, s, m)
    aus = []
    for b in range(1, len(kn)):
        sch = 0
        zer = 0
        for w in winkel:
            for ax in ((1, 0, 0), (0, 0, 1)):
                for sg in (1, -1):
                    R, T = pose_frei(f, 2, {b: rot(ax, sg * w)})
                    g, z, _, _ = rundblick(N, R, T, s, m)
                    sch = max(sch, g - g0)
                    zer = max(zer, len(z))
        aus.append((b, sch, zer))
    return g0, len(z0), s, aus


def stand(f, k):
    N = Netz(f)
    aus = {}
    for ci in (0, 1, 2):
        R, T = pose_frei(f, ci)
        V, Fa, Bi = N.alles(R, T)
        lo, hi = V.min(0), V.max(0)
        ga = np.linspace(lo[0], hi[0], 34); gb = np.linspace(lo[1], hi[1], 30); gc = np.linspace(lo[2], hi[2], 30)
        Q = np.stack(np.meshgrid(ga, gb, gc, indexing='ij'), -1).reshape(-1, 3)
        w = G2.windung(Q, V, Fa, block=2048)
        q = Q[np.abs(w) > .5]
        com = q.mean(0)
        vol = len(q) * (ga[1] - ga[0]) * (gb[1] - gb[0]) * (gc[1] - gc[0])
        # Bodenpunkte
        boden = V[-V[:, 1] <= 30]
        kont = []
        for nm, bs in G.BEINE[k].items():
            kont.append((nm, round(float(-np.vstack([V[Bi == b] for b in bs])[:, 1].max()))) if False else nm) if False else None
        hull = cv2.convexHull(boden[:, [0, 2]].astype(np.float32)) if len(boden) >= 3 else None
        d = cv2.pointPolygonTest(hull, (float(com[0]), float(com[2])), True) if hull is not None else None
        fl = float(cv2.contourArea(hull)) if hull is not None else 0.0
        aus[ci] = dict(volumen=round(vol / 1e9, 1), schwerpunkt=[round(float(x)) for x in com], boden_ecken=len(boden), standflaeche=round(fl / 1e6, 2), rand_abstand=None if d is None else round(d))
    return aus


if __name__ == '__main__':
    was = sys.argv[1] if len(sys.argv) > 1 else 'grund'
    ks = sys.argv[2] if len(sys.argv) > 2 else 'abc'
    for k in ks:
        f, _ = G.lade(k)
        nm = G.NAMEN[k]
        print('\n=========', k.upper())
        if was == 'grund':
            N = Netz(f)
            V0, _, _ = N.alles(*pose_frei(f, 0))
            m0 = (V0.max(0) + V0.min(0)) / 2
            s = GROSS * 0.46 / float(np.linalg.norm(V0 - m0, axis=1).max())
            for ci in (0, 1, 2):
                R, T = pose_frei(f, ci)
                g, z, jed, bl = rundblick(N, R, T, s)
                print('Clip', ci, 'Lochpixel Summe 14 Richtungen', g, 'je Richtung', jed, 'Zerfall', z[:6], ' (Masstab %.3f px/E)' % s)
                tafel(bl, os.path.join(ZW, 'loch_%s_clip%d.png' % (k, ci)))
            # Eltern-Kind-Abstand je Pose
            print('Eltern-Kind-Abstand (kleinster Abstand der Oberflaechen, 0 = beruehrt; - = Drehpunkt im Kind) je Clip:')
            kn = f['knochen']
            for ci in (0, 1, 2):
                R, T = pose_frei(f, ci)
                z = []
                for b in range(1, len(kn)):
                    Vc, Fc, _ = N.alles(R, T, [b])
                    Vp, Fp, _ = N.alles(R, T, [kn[b]['eltern']])
                    pc, _ = G2.flaechen_proben(Vc, Fc, 1 / 1200.0)
                    pp, _ = G2.flaechen_proben(Vp, Fp, 1 / 1200.0)
                    d = float(G2.naechster(pc, pp).min())
                    z.append((nm[b], round(d)))
                schl = [x for x in z if x[1] > 45]
                print(' Clip', ci, 'groesster Abstand', max(x[1] for x in z), 'ueber 45:', schl)
            print('Stand:', stand(f, k))
            print('Gelenkkugeln (Knochen, Radius in Kind, Radius in Rest; negativ = Drehpunkt ausserhalb):')
            kg = gelenkkugeln(f, k)
            print(' ', [(nm[a], b, c) for a, b, c in kg])
            print(' Gelenke Drehpunkt ausserhalb Kind:', [nm[a] for a, b, c in kg if b < 0], ' ausserhalb Rest:', [nm[a] for a, b, c in kg if c < 0])
        if was == 'biegen':
            g0, z0, s, a = biegen(f, k)
            print('Ausgang Clip 2: Lochpixel', g0, 'Zerfall', z0, 'Massstab %.3f' % s)
            for b, sch, zer in a:
                print(' %2d %-11s +-30 Grad: neue Lochpixel %6d  zerfallene Richtungen %2d' % (b, nm[b], sch, zer))
            print(' Summe neue Lochpixel', sum(x[1] for x in a), 'Gelenke mit ueber 100:', sum(x[1] > 100 for x in a), 'Gelenke mit Zerfall:', sum(x[2] > 0 for x in a))
