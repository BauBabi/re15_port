# -*- coding: utf-8 -*-
"""Messung 5: Wie sah die Freistellung 07_01/07_02 in JEDER Fassung aus?

Fuer jeden Commit, der die Datei angefasst hat: Groesse, opake Punkte, Bounding-Box
und die gemessene beste Lage im Hintergrund (Nullmodell-Vergleich).
"""
import io
import subprocess
import sys

import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
H, W = BG.shape[:2]


def beste_lage(a):
    h, w = a.shape[:2]
    m = a[..., 3] >= 110
    ys, xs = np.nonzero(m)
    if len(ys) == 0 or h > H or w > W:
        return None
    best = None
    werte = []
    for oy in range(0, H - h + 1):
        for ox in range(0, W - w + 1):
            d = np.abs(BG[ys + oy, xs + ox] - a[ys, xs, :3]).sum() / float(len(ys))
            werte.append(d)
            if best is None or d < best[0]:
                best = (d, ox, oy)
    return best, float(np.median(werte)), len(ys)


FASSUNGEN = {
    "07_01": [("7b24600f", "pri/STAGE1/10D0/07_01.png"),
              ("a56cff68", "pri/10D0/07_01.png"),
              ("61607499", "pri/10D0/07_01.png")],
    "07_02": [("7b24600f", "pri/STAGE1/10D0/07_02.png"),
              ("a56cff68", "pri/10D0/07_02.png"),
              ("61607499", "pri/10D0/07_02.png")],
}

for datei, paare in sorted(FASSUNGEN.items()):
    print("=== " + datei)
    for sha, pfad in paare:
        zeile = sha + " " + pfad
        try:
            blob = subprocess.check_output(["git", "show", "%s:%s" % (sha, pfad)])
        except subprocess.CalledProcessError:
            print("  %s: nicht vorhanden" % zeile)
            continue
        im = Image.open(io.BytesIO(blob)).convert("RGBA")
        a = np.asarray(im, int)
        m = a[..., 3] >= 110
        ys, xs = np.nonzero(m)
        r = beste_lage(a)
        print("  %-70s %dx%d  opak %4d  bbox x%d..%d y%d..%d"
              % (zeile[:70], im.size[0], im.size[1], m.sum(),
                 xs.min(), xs.max(), ys.min(), ys.max()))
        if r:
            (d, ox, oy), med, n = r
            print("        beste Lage x=%d y=%d |dRGB|=%.2f (Median aller Lagen %.1f)"
                  % (ox, oy, d, med))
