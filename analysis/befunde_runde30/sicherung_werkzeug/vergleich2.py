#!/usr/bin/env python3
"""Messbare Unterschiede der vier Darstellungen (Fortsetzungs-Agent, vollstaendige Masken).

  (a) Welt-Modell            gen/sicherung_prop.inc  (Geometrie + Textur-Nutzflaeche 32x128)
  (b) Item-Bild              ITEM/ITPS.ITP Block 0x40
  (c) Inventar-Icon          DATA/ITEMALL.PIX Tile 0x40
  (e) Weg-2-Prototyp         build/r30_sicherung/weg2z_itps_block_40.bin / weg2z_icon_tile_40.bin
  (d) ROOM1050 Cut 8         steht in vergleich.txt des Vorgaengers (Differenz Cut 8 - Cut 7)

    python analysis/befunde_runde30/sicherung_werkzeug/vergleich2.py
"""
import math
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import REPO, inc_bytes, tim_lesen, rgb15  # noqa: E402

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
Z = os.path.join(REPO, "build", "r30_sicherung")


def mess(name, rgb, m):
    ys, xs = np.nonzero(m)
    p = np.stack([xs, ys], 1).astype(float)
    q = p - p.mean(0)
    w, v = np.linalg.eigh(q.T @ q / len(q))
    a = v[:, 1]
    t, s = q @ a, q @ v[:, 0]
    L = np.percentile(t, 99) - np.percentile(t, 1) + 1
    D = np.percentile(s, 95) - np.percentile(s, 5) + 1
    c = rgb[m].astype(float)
    lum = c.mean(1)
    print("%-28s %5d px  Laenge %5.1f  Dicke %5.1f  Schlankheit %4.1f : 1  Achse %5.1f Grad | "
          "Mittel RGB (%3.0f,%3.0f,%3.0f)  hell(>=150) %4.1f %%  dunkel(<60) %4.1f %%"
          % (name, int(m.sum()), L, D, L / D, math.degrees(math.atan2(a[1], a[0])) % 180,
             c[:, 0].mean(), c[:, 1].mean(), c[:, 2].mean(),
             100.0 * (lum >= 150).mean(), 100.0 * (lum < 60).mean()))


def itps(b):
    clut = np.array(struct.unpack_from("<256H", b, 0x14), np.uint16)
    idx = np.frombuffer(b, np.uint8, 112 * 72, 0x220).reshape(72, 112)
    w = clut[idx] & 0x7FFF
    rgb = np.stack([(w & 31) << 3, ((w >> 5) & 31) << 3, ((w >> 10) & 31) << 3], -1)
    innen = np.zeros((72, 112), bool)
    innen[2:-2, 2:-2] = True
    r, g, bl = rgb[..., 0].astype(int), rgb[..., 1].astype(int), rgb[..., 2].astype(int)
    m = innen & (w != 0x1C00) & ~((bl > r + 25) & (bl > g + 25))
    return rgb, m


def icon(t):
    st00 = open(os.path.join(PSX, "DATA", "ST_00.TIM"), "rb").read()
    cl = np.array(struct.unpack_from("<256H", st00, 20), np.uint16)
    rgb = np.stack([(cl & 31) << 3, ((cl >> 5) & 31) << 3, ((cl >> 10) & 31) << 3], -1)[t]
    return rgb, ~np.isin(t, [0xE3, 0xE4, 0xE6])


d = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
mess("(b) Item-Bild 0x40", *itps(d[0x40 * 0x3000:0x41 * 0x3000]))
mess("(e) Weg-2 Item-Bild", *itps(open(os.path.join(Z, "weg2z_itps_block_40.bin"), "rb").read()))
pix = np.frombuffer(open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read(), np.uint8).reshape(-1, 30, 40)
mess("(c) Icon 0x40", *icon(pix[0x40]))
mess("(e) Weg-2 Icon", *icon(np.frombuffer(open(os.path.join(Z, "weg2z_icon_tile_40.bin"), "rb").read(),
                                           np.uint8).reshape(30, 40)))
w, h, idx, cluts, kopf = tim_lesen(inc_bytes("re15_sicherung_tim"))
t = np.array([[rgb15(cluts[0][idx[y][x]]) for x in range(32)] for y in range(128)], float)
lum = t.mean(2)
print("(a) Welt-Modell Textur 32x128 (Nutzflaeche oben links der 128x256-Seite): Mittel RGB (%.0f,%.0f,%.0f) "
      "hell(>=150) %.1f %%  dunkel(<60) %.1f %%" % (t[..., 0].mean(), t[..., 1].mean(), t[..., 2].mean(),
                                                   100 * (lum >= 150).mean(), 100 * (lum < 60).mean()))
md = inc_bytes("re15_sicherung_md1")
(tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", md, 12)
V = np.array([struct.unpack_from("<3h", md, 12 + qv + i * 8) for i in range(qvc)])
print("(a) Welt-Modell Geometrie: x[%d..%d] y[%d..%d] z[%d..%d] -> Laenge %d, Durchmesser %d, Schlankheit %.1f : 1"
      % (V[:, 0].min(), V[:, 0].max(), V[:, 1].min(), V[:, 1].max(), V[:, 2].min(), V[:, 2].max(),
         V[:, 0].max() - V[:, 0].min(), V[:, 1].max() - V[:, 1].min(),
         (V[:, 0].max() - V[:, 0].min()) / float(V[:, 1].max() - V[:, 1].min())))
