#!/usr/bin/env python3
"""Kandidat B: Textur malen. Grund prozedural aus der 3D-Lage je Texel, Zeichnung aus den gemessenen Sollflaechen der Vorlagen."""
import json, math, os, sys
import numpy as np, cv2
from b_geom import *
import b_teile as T
import b_bau as BB
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P

MAT = P.formblatt()['zeichnung']['materialien']
FARBE = {k: np.array(v['oberflaeche_rgb'], float) for k, v in MAT.items()}
FARBE['z9'] = np.array([85, 80, 60], float)
FARBE['untere_streifen'] = np.array([205, 185, 200], float)     # innerhalb der Spanne, Obergrenze ist nicht die Farbe
FARBE['weisskerne'] = np.array([214, 222, 226], float)          # nur Mindesthelligkeit belegt, Wert frei


class Sicht:
    """Eine Vorlage: Kamera + Pose. Liefert Tiefenkarte und Texel -> Bild."""

    def __init__(self, n, kam, R, Tt, J, lage, gier):
        self.n, self.kam, self.R, self.T, self.J = n, kam, R, Tt, J
        self.lage, self.gier = np.array(lage, float), gier
        self.Yg = Y(gier)

    def welt(self, p, b):
        return ((self.R[b] @ (p - self.J[b]).T).T + self.T[b]) @ self.Yg.T + self.lage

    def tiefenkarte(self, netze, H=480, B=640):
        z = np.full((H, B), 1e9, np.float32)
        for nz in netze:
            w = self.welt(nz['v'], nz['bone'])
            xy = FK.projiziere(self.kam, w)
            d = FK.tiefe(self.kam, w)
            for e in nz['f']:
                idx = [a for a, _ in e]
                for j in range(1, len(idx) - 1):
                    g = BB.tri_raster(xy[idx[0]], xy[idx[j]], xy[idx[j + 1]], (H, B))
                    if g is None:
                        continue
                    xs, ys, w0, w1, w2 = g
                    t = w0 * d[idx[0]] + w1 * d[idx[j]] + w2 * d[idx[j + 1]]
                    q = t < z[ys, xs]
                    z[ys[q], xs[q]] = t[q]
        self.z = z
        return z

    def texel(self, pos, nrm, bone):
        """Fuer belegte Texel: Bild-x, Bild-y, sichtbar."""
        H, B = bone.shape
        xy = np.full((H, B, 2), np.nan, np.float32)
        sicht = np.zeros((H, B), bool)
        auge = np.asarray(self.kam['auge'], float)
        for b in np.unique(bone):
            if b < 0:
                continue
            m = bone == b
            p = pos[m].astype(float)
            n = nrm[m].astype(float)
            w = self.welt(p, b)
            q = FK.projiziere(self.kam, w)
            d = FK.tiefe(self.kam, w)
            nw = (self.R[b] @ n.T).T @ self.Yg.T
            zu = ((auge - w) * nw).sum(1) > 0
            x = np.clip(np.round(q[:, 0]).astype(int), 0, 639)
            y = np.clip(np.round(q[:, 1]).astype(int), 0, 479)
            sichtbar = zu & (d <= self.z[y, x] + 90)
            xy[m] = q
            sicht[m] = sichtbar
        return xy, sicht


def rauschen3(p, zelle, saat=0):
    """Wert-Rauschen 3D, glatt, -1..1."""
    q = p / zelle + saat * 17.13
    i = np.floor(q).astype(np.int64)
    f = q - i
    f = f * f * (3 - 2 * f)

    def h(a, b, c):
        x = (i[..., 0] + a) * 73856093 ^ (i[..., 1] + b) * 19349663 ^ (i[..., 2] + c) * 83492791 ^ saat * 1000003
        x = (x ^ (x >> 13)) * 1274126177
        return ((x ^ (x >> 16)) & 0xFFFF) / 65535.0 * 2 - 1
    v = 0
    for a in (0, 1):
        for b in (0, 1):
            for c in (0, 1):
                w = (f[..., 0] if a else 1 - f[..., 0]) * (f[..., 1] if b else 1 - f[..., 1]) * (f[..., 2] if c else 1 - f[..., 2])
                v = v + w * h(a, b, c)
    return v


def zellen3(p, zelle, saat=0):
    """Worley: Abstand zum naechsten und zweitnaechsten Punkt, in Zellen."""
    q = p / zelle
    i = np.floor(q).astype(np.int64)
    d1 = np.full(p.shape[:-1], 9.0)
    d2 = np.full(p.shape[:-1], 9.0)
    for a in (-1, 0, 1):
        for b in (-1, 0, 1):
            for c in (-1, 0, 1):
                g = i + np.array([a, b, c])
                x = g[..., 0] * 73856093 ^ g[..., 1] * 19349663 ^ g[..., 2] * 83492791 ^ (saat * 2654435761)
                x = (x ^ (x >> 13)) * 1274126177
                jx = ((x >> 3) & 1023) / 1023.0
                jy = ((x >> 13) & 1023) / 1023.0
                jz = ((x >> 23) & 1023) / 1023.0
                d = np.sqrt(((g + np.stack([jx, jy, jz], -1)) - q) ** 2).sum(-1) if False else np.sqrt((((g + np.stack([jx, jy, jz], -1)) - q) ** 2).sum(-1))
                n = d < d1
                d2 = np.where(n, d1, np.minimum(d2, d))
                d1 = np.where(n, d, d1)
    return d1, d2


def haut(pos, nrm, belegt):
    """Grundhaut: dunkel, Falten, Knoten, Adern. Alle Farben aus der Palette (grundhaut bis rotbraun)."""
    H, B = belegt.shape
    p = pos[belegt].astype(np.float64)
    g = FARBE['grundhaut_dunkel']
    m = FARBE['rotbraun_mittelton']
    t = 0.5 + 0.5 * rauschen3(p, 420.0, 1)
    # Falten: Rippen im verzerrten Feld
    w = rauschen3(p, 260.0, 2) * 130
    fa = np.abs(np.sin((p[:, 1] + w * 1.4 + 0.5 * p[:, 0]) / 62.0))
    falte = np.clip(1 - fa * 7, 0, 1)
    # Knoten
    d1, d2 = zellen3(p, 130.0, 3)
    knoten = np.clip(1 - d1 / 0.38, 0, 1)
    rand = np.clip(1 - (d2 - d1) / 0.10, 0, 1)
    # Adern: Nullstelle eines Rauschfeldes
    a = np.abs(rauschen3(p, 300.0, 4) + 0.4 * rauschen3(p, 110.0, 5))
    ader = np.clip(1 - a / 0.035, 0, 1)
    s = 0.10 + 0.20 * t + 0.34 * knoten ** 2 + 0.28 * ader
    s = s * (1 - 0.65 * falte) * (1 - 0.4 * rand)
    s = np.clip(s, 0, 1)
    f = g[None] * 0.72 + (m[None] - g[None] * 0.72) * s[:, None]
    aus = np.zeros((H, B, 3), np.float32)
    aus[belegt] = f
    return aus


def stellen_malen(rgb, sichten, nrm_mask=None):
    """Sollflaechen in die Textur, je Vorlage mit Sichtpruefung."""
    for s in P.stellen():
        v = sichten[s['bild']]
        k = np.zeros((480, 640), np.uint8)
        for q in s['polygone']:
            cv2.fillPoly(k, [np.round(np.array(q, float)).astype(np.int32)], 1)
        xy, si = v['xy'], v['sicht']
        ok = si & np.isfinite(xy[..., 0])
        x = np.clip(np.round(xy[..., 0]).astype(int), 0, 639)
        y = np.clip(np.round(xy[..., 1]).astype(int), 0, 479)
        tr = ok & (k[y, x] > 0)
        rgb[tr] = FARBE[s['material']]
    return rgb


def quantisieren(rgb, belegt):
    r = np.clip(rgb, 0, 255).astype(np.uint8)
    return BB.FE.tim_aus_rgb(r, None)
