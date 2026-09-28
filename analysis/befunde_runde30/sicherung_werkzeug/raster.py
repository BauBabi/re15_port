#!/usr/bin/env python3
"""Kleiner Software-Rasterer mit Z-PUFFER (nur Analyse, NICHT der Zeichenweg des Ports).

Der Port sortiert Dreiecke nach mittlerer Tiefe (Maler-Verfahren / OT); dieser Rasterer
rechnet dagegen die GEOMETRISCH WAHRE Verdeckung je Pixel. Er beantwortet damit die
Frage "liegt der Gegenstand frei oder steckt er in/unter etwas?" — die Frage "was
zeichnet der Port?" beantwortet ausschliesslich der Framedump aus dem Spiel.
"""
import numpy as np


class Bild:
    def __init__(self, w, h, hintergrund=(40, 40, 48)):
        self.w, self.h = w, h
        self.rgb = np.zeros((h, w, 3), np.uint8)
        self.rgb[:] = hintergrund
        self.z = np.full((h, w), np.inf)
        self.wer = np.full((h, w), -1, np.int32)

    def dreieck(self, p, uv, tex, kennung, hell=1.0, farbe=None):
        """p = 3x(sx, sy, tiefe) ; uv = 3x(u, v) ; tex = (H, W, 4)-Feld RGBA oder None."""
        (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = p
        fl = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0)
        if abs(fl) < 1e-9:
            return
        xa = max(int(np.floor(min(x0, x1, x2))), 0)
        xb = min(int(np.ceil(max(x0, x1, x2))), self.w - 1)
        ya = max(int(np.floor(min(y0, y1, y2))), 0)
        yb = min(int(np.ceil(max(y0, y1, y2))), self.h - 1)
        if xa > xb or ya > yb:
            return
        ys, xs = np.mgrid[ya:yb + 1, xa:xb + 1]
        px, py = xs + 0.5, ys + 0.5
        w0 = ((x1 - px) * (y2 - py) - (x2 - px) * (y1 - py)) / fl
        w1 = ((x2 - px) * (y0 - py) - (x0 - px) * (y2 - py)) / fl
        w2 = 1.0 - w0 - w1
        drin = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        if not drin.any():
            return
        z = w0 * z0 + w1 * z1 + w2 * z2
        if tex is not None:
            u = np.clip((w0 * uv[0][0] + w1 * uv[1][0] + w2 * uv[2][0]).astype(int), 0, tex.shape[1] - 1)
            v = np.clip((w0 * uv[0][1] + w1 * uv[1][1] + w2 * uv[2][1]).astype(int), 0, tex.shape[0] - 1)
            t = tex[v, u]
            drin &= t[..., 3] > 0
            c = np.clip(t[..., :3].astype(float) * hell, 0, 255).astype(np.uint8)
        else:
            c = np.zeros(z.shape + (3,), np.uint8)
            c[:] = farbe
        sub_z = self.z[ya:yb + 1, xa:xb + 1]
        vorn = drin & (z < sub_z)
        sub_z[vorn] = z[vorn]
        self.rgb[ya:yb + 1, xa:xb + 1][vorn] = c[vorn]
        self.wer[ya:yb + 1, xa:xb + 1][vorn] = kennung


def tim_als_rgba(tim_tuple):
    from r30_lib import rgb15
    w, h, idx, cluts, kopf = tim_tuple
    a = np.zeros((h, w, 4), np.uint8)
    cl = cluts[0]
    for y in range(h):
        for x in range(w):
            c = cl[idx[y][x]]
            if c:
                a[y, x] = rgb15(c) + (255,)
    return a


def flaechen(mesh):
    """-> Liste (punkte[3], uv[3]) — Quads in zwei Dreiecke (0,1,2)+(1,3,2)? NEIN:
    der Port teilt (0,1,3)+(0,3,2) (main.c, Quad-Zweig). Hier genauso."""
    aus = []
    for k, (a, b, c) in enumerate(mesh["tris"]):
        u = mesh["tuv"][k]
        aus.append(((mesh["tv"][a], mesh["tv"][b], mesh["tv"][c]),
                    ((u[0], u[1]), (u[3], u[4]), (u[6], u[7])), (u[5] & 15) * 128))
    for k, (a, b, c, d) in enumerate(mesh["quads"]):
        u = mesh["quv"][k]
        p = [mesh["qv"][i] for i in (a, b, c, d)]
        t = [(u[0], u[1]), (u[3], u[4]), (u[6], u[7]), (u[9], u[10])]
        po = (u[5] & 15) * 128
        aus.append(((p[0], p[1], p[3]), (t[0], t[1], t[3]), po))
        aus.append(((p[0], p[3], p[2]), (t[0], t[3], t[2]), po))
    return aus
