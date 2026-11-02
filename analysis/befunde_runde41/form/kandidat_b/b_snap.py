#!/usr/bin/env python3
"""Kandidat B: Umrissecken der Quad-Netze auf die Sollkante ziehen, beide Bilder, starre Teile. Verschiebung wird ueber das Netz geglaettet."""
import math, os, sys, json
import numpy as np, cv2
from scipy.ndimage import distance_transform_edt as EDT
from b_geom import *
import b_teile as T
import b_bau as BB
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P

RASTER_VERSATZ = np.array([-1.2, -1.3])     # Mittel aus Abschneiden im Port + Phase (form_buehne), Video-Pixel


def stuecke(quad):
    """Teile wie im Bauordner: kuppel + k??. Rueckgabe Liste dict(name, bone, V, F, nb)."""
    aus = []
    namen = ['kuppel'] + ['k%02d' % i for i in range(len(T.KNOCHEN))]
    for nm in namen:
        p = os.path.join(quad, nm + '.obj')
        if not os.path.exists(p):
            continue
        v, vt, f = BB.lese_obj(p)
        F_ = [[a for a, _ in e] for e in f]
        nb = [set() for _ in range(len(v))]
        for e in F_:
            for i in range(len(e)):
                a, b = e[i], e[(i + 1) % len(e)]
                nb[a].add(b)
                nb[b].add(a)
        n, um = BB.vertexnormalen(v, f)
        aus.append(dict(name=nm, bone=T.IDX['rumpf_m'] if nm == 'kuppel' else int(nm[1:]), V=v.copy(), V0=v.copy(), F=F_, nb=[np.array(sorted(s), int) for s in nb], um=um))
    return aus


class Sicht:
    def __init__(self, n, kam, lage, gier, R, Tt, J):
        self.n, self.kam, self.lage, self.gier, self.R, self.T, self.J = n, kam, np.array(lage, float), gier, R, Tt, J
        self.Yg = Y(gier)
        self.f = np.array([kam['brennweite_bild_px']['fx'], kam['brennweite_bild_px']['fy']], float)

    def welt(self, st):
        b = st['bone']
        return ((self.R[b] @ (st['V'] - self.J[b]).T).T + self.T[b]) @ self.Yg.T + self.lage

    def proj(self, w):
        return FK.projiziere(self.kam, w) + RASTER_VERSATZ

    def maske(self, teile):
        m = np.zeros((480, 640), np.uint8)
        for st in teile:
            q = np.round(self.proj(self.welt(st))).astype(np.int32)
            for e in st['F']:
                if len(e) == 3:
                    cv2.fillConvexPoly(m, q[e], 1)
                else:
                    cv2.fillConvexPoly(m, q[[e[0], e[1], e[2]]], 1)
                    cv2.fillConvexPoly(m, q[[e[0], e[2], e[3]]], 1)
        return m.astype(bool)

    def konturecken(self, st):
        """Ecken mit vorderen und hinteren Nachbarflaechen."""
        w = self.welt(st)
        a = np.asarray(self.kam['auge'], float)
        vo = np.zeros(len(w), bool)
        hi = np.zeros(len(w), bool)
        for e in st['F']:
            k = np.cross(w[e[1]] - w[e[0]], w[e[2]] - w[e[0]])
            if st['um']:
                k = -k
            s = k @ (a - w[e[0]])
            for i in e:
                if s > 0:
                    vo[i] = True
                else:
                    hi[i] = True
        return vo & hi, w


def sdf(m):
    """+ ausserhalb, - innerhalb, Pixel."""
    return EDT(~m) - EDT(m)


def gradient(s):
    g = cv2.GaussianBlur(s.astype(np.float32), (0, 0), 1.2)
    return cv2.Sobel(g, cv2.CV_32F, 1, 0, ksize=3) / 8.0, cv2.Sobel(g, cv2.CV_32F, 0, 1, ksize=3) / 8.0


def bild1_ziel():
    fig = cv2.imread(FORM + '/pruefung/figur_bild1.png', 0) > 0
    z = cv2.imread(FORM + '/ziel_bild1.png', 0)
    offen = np.zeros((480, 640), bool)
    offen[401:, :290] = True
    offen[415:, 290:] = True
    offen[:225, 250:277] = True
    offen[278:303, 412:437] = True
    return fig, z, offen


def bild2_ziel():
    fig = cv2.imread(FORM + '/pruefung/figur_bild2.png', 0) > 0
    z = cv2.imread(FORM + '/ziel_bild2.png', 0)
    return fig, z


def schritt(sichten, teile, ziele, rate=0.6, sigma=95.0, max_px=9.0, deckel=190.0):
    """Weiche Verschiebung: Umrissecken geben eine Bildebenen-Verschiebung vor, das ganze Teil folgt per Gauss-Gewicht ueber den 3D-Abstand. Kein Dorn, Deckel je Ecke."""
    bericht = {}
    for s in sichten:
        m = s.maske(teile)
        rand = m & ~cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
        dr = EDT(~rand)
        Z = ziele[s.n]
        fehl = []
        for ti, st in enumerate(teile):
            kon, w = s.konturecken(st)
            q = s.proj(w)
            ix = np.clip(np.round(q[:, 0]).astype(int), 1, 638)
            iy = np.clip(np.round(q[:, 1]).astype(int), 1, 478)
            tiefe = FK.tiefe(s.kam, w)
            idx = []
            d = []
            for i in np.nonzero(kon & (dr[iy, ix] <= 1.5))[0]:
                dx, dy = Z(ix[i], iy[i])
                if dx is None:
                    continue
                idx.append(i)
                d.append((dx, dy))
                fehl.append(math.hypot(dx, dy))
            if not idx:
                continue
            idx = np.array(idx)
            d = np.clip(np.array(d, float), -max_px, max_px)
            r, u = s.kam['R'][0], s.kam['R'][1]
            dw = (d[:, 0, None] * r[None] * (tiefe[idx] / s.f[0])[:, None]) + (d[:, 1, None] * u[None] * (tiefe[idx] / s.f[1])[:, None])
            b = st['bone']
            dl = (dw @ s.Yg) @ s.R[b]
            P0 = st['V'][idx]
            d2 = ((st['V'][:, None, :] - P0[None]) ** 2).sum(-1)
            G = np.exp(-d2 / (2 * sigma ** 2))
            nen = G.sum(1)
            feld = (G @ dl) / np.maximum(nen, 1e-6)[:, None]
            feld *= np.minimum(nen, 1.0)[:, None]               # weit weg vom Umriss bleibt alles stehen
            st['V'] += rate * feld
            weg = st['V'] - st['V0']
            lg = np.linalg.norm(weg, axis=1, keepdims=True)
            st['V'] = st['V0'] + weg * np.minimum(1.0, deckel / np.maximum(lg, 1e-6))
        bericht[s.n] = (len(fehl), float(np.mean(fehl)) if fehl else 0.0, float(np.max(fehl)) if fehl else 0.0)
    return bericht


def baue_ziele(tiefenbonus=None):
    fig1, z1, offen1 = bild1_ziel()
    s1 = sdf(fig1)
    gx1, gy1 = gradient(s1)

    schwarz1 = z1 == 0
    s_b1 = sdf(schwarz1)
    gxb1, gyb1 = gradient(s_b1)

    def Z1(x, y):
        if schwarz1[y, x]:                  # Vorrang: sicher nicht Figur, hinaus bis ueber den grauen Saum
            e = -s_b1[y, x] + 2.0
            return e * gxb1[y, x], e * gyb1[y, x]
        if offen1[y, x]:
            return None, None
        e = -(s1[y, x] + 1.0)               # 1 px nach innen: der Pruefwert schliesst Stichloecher (geschlossen, 3 x 3), das Raster deckt angeschnittene Pixel
        return e * gx1[y, x], e * gy1[y, x]
    fig2, z2 = bild2_ziel()
    weiss = z2 == 255
    schwarz = z2 == 0
    s_w = sdf(weiss)
    s_b = sdf(schwarz)
    gxw, gyw = gradient(s_w)
    gxb, gyb = gradient(s_b)
    s2 = sdf(fig2)
    gx2, gy2 = gradient(s2)
    R = P.teil_b2('R')
    belegt = np.zeros((480, 640), np.uint8)
    cv2.polylines(belegt, [np.round(np.array(R['kante_belegt'], float)).astype(np.int32)], False, 1, 1)
    nah = EDT(belegt == 0) < 16

    def Z2(x, y):
        if nah[y, x] or (405 <= x <= 416 and 220 <= y <= 268):
            e = -s2[y, x]
            return e * gx2[y, x], e * gy2[y, x]
        if weiss[y, x]:                                 # Kontur liegt in Weiss: nach aussen bis zur Weissgrenze
            e = -s_w[y, x] + 1.5
            return e * gxw[y, x], e * gyw[y, x]
        if schwarz[y, x]:                               # Kontur liegt in Schwarz: heraus
            e = -s_b[y, x] + 1.5
            return e * gxb[y, x], e * gyb[y, x]
        return None, None
    return {1: Z1, 2: Z2}
