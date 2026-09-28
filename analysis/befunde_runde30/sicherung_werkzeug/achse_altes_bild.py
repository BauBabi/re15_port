#!/usr/bin/env python3
"""Hauptachse/Laenge des Gegenstands im ausgelieferten Item-Bild 0x40 und Icon 0x40 —
mit der VOLLSTAENDIGEN Maske (alles, was nicht Hintergrundwort/-index ist, ohne die
blau-dominante Kantenglaettung). Der Vorgaenger mass mit einer Maske, der die dunklen
Flaechen fehlten (1447 statt 1972 px).

    python analysis/befunde_runde30/sicherung_werkzeug/achse_altes_bild.py
"""
import math
import os
import struct

import numpy as np

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")


def mess(name, m):
    ys, xs = np.nonzero(m)
    p = np.stack([xs, ys], 1).astype(float)
    c = p.mean(0)
    q = p - c
    w, v = np.linalg.eigh(q.T @ q / len(q))
    a = v[:, 1]
    t = q @ a
    s = q @ v[:, 0]
    grad = math.degrees(math.atan2(a[1], a[0])) % 180
    print("%-22s %4d px  Schwerpunkt (%.1f, %.1f)  Achse %.1f Grad  Laenge %.1f px (1.-99. Perzentil %.1f)  "
          "Dicke %.1f px (5.-95. Perzentil)"
          % (name, len(p), c[0], c[1], grad, t.max() - t.min() + 1,
             np.percentile(t, 99) - np.percentile(t, 1) + 1,
             np.percentile(s, 95) - np.percentile(s, 5) + 1))


d = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
b = d[0x40 * 0x3000:0x41 * 0x3000]
clut = np.array(struct.unpack_from("<256H", b, 0x14), np.uint16)
idx = np.frombuffer(b, np.uint8, 112 * 72, 0x220).reshape(72, 112)
w = clut[idx]
r, g, bl = ((w & 31) << 3).astype(int), (((w >> 5) & 31) << 3).astype(int), (((w >> 10) & 31) << 3).astype(int)
innen = np.zeros((72, 112), bool)
innen[2:-2, 2:-2] = True
kern = innen & (w != 0x1C00) & ~((bl > r + 25) & (bl > g + 25))
mess("Item-Bild 0x40 (Kern)", kern)

pix = open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read()
t = np.frombuffer(pix, np.uint8).reshape(-1, 30, 40)[0x40]
mess("Icon 0x40", ~np.isin(t, [0xE3, 0xE4, 0xE6]))
