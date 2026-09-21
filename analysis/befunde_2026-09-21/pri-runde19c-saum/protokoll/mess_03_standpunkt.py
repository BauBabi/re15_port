"""Warum verschiebt der Saum die Tiefe des Stuhls um EINEN Eimer (53..62 -> 52..61)?
Der aufrecht-Zweig nimmt EINEN Standpunkt: Schwerpunkt der BELEGTEN SPALTEN (cx = mean(xs)
+ 0,5, geometrie.py:125) und tiefste Silhouettenzeile (geometrie.py:127). Beides hier
gemessen, vorher gegen nachher, samt Weltpunkt und Kamera-z.
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom                                                  # noqa: E402
import geometrie                                             # noqa: E402

CD = "re15_port/shared_assets/PSX"
ROOM, CUT = "ROOM10D0", 7
rdt, _ = geom.load_rdt(CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)
print("Kamerasatz Cut %d: H=%d, R[4]=%d R[7]=%d" % (CUT, H, R[4], R[7]))

alt = np.load("build/r19c/alt.npy")
neu = np.load("build/r19c/neu.npy")
for name, reg in (("VORHER 07_01", alt), ("NACHHER 07_01_mit_saum", neu)):
    xs = [int(x) for x in np.nonzero(reg.any(0))[0]]
    cx = float(np.mean(xs)) + 0.5
    yb = float(int(np.nonzero(reg.any(1))[0].max()))
    P = geometrie.welt_am_boden(R, t, H, cx, min(yb, 239.0) + 0.5, 0)
    vz = geometrie.vz_at_floor(R, t, H, cx, min(yb, 239.0) + 0.5, 0)
    ys = np.arange(240)
    prof, ok = geometrie.profil_spalte(R, t, H, P[0], P[1], ys)
    eimer = np.floor(1023.0 * prof / 65536.0)
    zeilen = np.nonzero(reg.any(1))[0]
    print("%-24s Spalten %d (x%d..%d), cx=%.1f, tiefste Zeile %d"
          % (name, len(xs), min(xs), max(xs), cx, yb))
    print("   Standpunkt Welt (%d, %d), Kamera-z %d, Eimer %d"
          % (P[0], P[1], vz, int(1023 * vz / 65536)))
    print("   Zeilen %d..%d -> Eimer %d..%d"
          % (zeilen.min(), zeilen.max(), int(eimer[zeilen.min()]), int(eimer[zeilen.max()])))

xa = set(np.nonzero(alt.any(0))[0].tolist())
xn = set(np.nonzero(neu.any(0))[0].tolist())
print("neue Spalten durch den Saum: %s ; weggefallen: %s" % (sorted(xn - xa), sorted(xa - xn)))
ya = int(np.nonzero(alt.any(1))[0].max()); yn = int(np.nonzero(neu.any(1))[0].max())
print("tiefste Zeile: vorher %d, nachher %d" % (ya, yn))
