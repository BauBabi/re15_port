# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Kamera-Geometrie fuer beliebige Raeume (Vorbild r30_idw_geom.py).

Die Sichtmatrizen werden NICHT nachgebaut, sondern aus der Ausgabe der Engine-Sonde gelesen
(probe_r34n_e_dokumente kamera RAUM > build/r34n_e/kamera_RAUM.txt): dieselben Q12-Werte, mit
denen der Port zeichnet (re15_camera_build_view = FUN_80053ca4 @0x80053ca4, H = fov >> 7
@0x80021e70 / @0x80046128, Bildmitte (160,120) @0x800460f4-fc).

⛔ Die Engine-Matrix ist wegen SquareRoot0 NICHT exakt orthonormal - fuer den Sehstrahl wird die
ECHTE Inverse benutzt (Runde 30 irons-diary-welt 2.4: R^T lag 110 Einheiten daneben).
"""
import os
import re

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
AUS = os.path.join(REPO, "build", "r34n_e")


def lade_kameras(raum):
    pfad = os.path.join(AUS, "kamera_%s.txt" % raum)
    cams = {}
    for z in open(pfad):
        m = re.match(r"VIEW (\d+) R= (.*?) T= (.*?) H= (\d+)", z)
        if m:
            c = int(m.group(1))
            Ri = np.array([int(v) for v in m.group(2).split()]).reshape(3, 3)
            cams.setdefault(c, {}).update(dict(R=Ri / 4096.0, Ri=Ri,
                                               T=np.array([int(v) for v in m.group(3).split()], float),
                                               H=float(m.group(4))))
        m = re.match(r"CUT (\d+) .*pos=(-?\d+) (-?\d+) (-?\d+) tgt=(-?\d+) (-?\d+) (-?\d+) pri=0x([0-9A-F]+)", z)
        if m:
            c = int(m.group(1))
            cams.setdefault(c, {})
            cams[c]["pos"] = np.array([int(m.group(i)) for i in (2, 3, 4)], float)
            cams[c]["tgt"] = np.array([int(m.group(i)) for i in (5, 6, 7)], float)
            cams[c]["pri"] = int(m.group(8), 16)
    return cams


def projiziere(cam, p):
    """Weltpunkt -> (sx, sy, vz); ganzzahlig wie die Sonde (>>12 je Zeile)."""
    p = np.round(np.asarray(p, float))
    v = np.floor((cam["Ri"] @ p) / 4096.0) + cam["T"]
    if v[2] <= 0:
        return None
    return 160.0 + cam["H"] * v[0] / v[2], 120.0 + cam["H"] * v[1] / v[2], v[2]


def strahl(cam, sx, sy):
    Rinv = np.linalg.inv(cam["R"])
    d = np.array([(sx - 160.0) / cam["H"], (sy - 120.0) / cam["H"], 1.0])
    return Rinv @ (-cam["T"]), Rinv @ d


def auf_ebene(cam, sx, sy, y):
    wo, wd = strahl(cam, sx, sy)
    s = (y - wo[1]) / wd[1]
    return wo + s * wd, s


def trianguliere(strahlen):
    """Kleinste-Quadrate-Schnittpunkt mehrerer Strahlen (o, d); dazu die Restabstaende."""
    A = np.zeros((3, 3))
    b = np.zeros(3)
    for o, d in strahlen:
        d = d / np.linalg.norm(d)
        P = np.eye(3) - np.outer(d, d)
        A += P
        b += P @ o
    x = np.linalg.solve(A, b)
    rest = []
    for o, d in strahlen:
        d = d / np.linalg.norm(d)
        rest.append(float(np.linalg.norm((np.eye(3) - np.outer(d, d)) @ (x - o))))
    return x, rest


def bg(raum, cut):
    from PIL import Image
    grp = raum[:3]
    return np.array(Image.open(os.path.join(AUS, "bg", "ROOM%s%02d.ppm" % (grp, cut))).convert("RGB")).astype(float)
