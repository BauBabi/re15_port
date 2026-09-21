# -*- coding: utf-8 -*-
"""Messung 18: Steht an der gemessenen Stelle wirklich ein Gegenstand?

(a) die soliden SCA-Zellen des Raums um (1870, 27980),
(b) ist der Punkt begehbar (Engine-Dump der Sonde)? Begehbar => kein Gegenstand.
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
print("solide Typ-1-Zellen (Band 0), die x1000..3000 / z27000..29000 beruehren:")
for (x, z, w, d) in geom.sca_wandzellen(rdt, 0):
    if x < 3000 and x + w > 1000 and z < 29000 and z + d > 27000:
        print("   x%6d..%6d  z%6d..%6d  (%dx%d)" % (x, x + w, z, z + d, w, d))
print()
print("ALLE soliden Zellen mit z27000..29500:")
for (x, z, w, d) in geom.sca_wandzellen(rdt, 0):
    if z < 29500 and z + d > 27000:
        print("   x%6d..%6d  z%6d..%6d" % (x, x + w, z, z + d))

floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", 0x10D0)
pts = np.array(floor.get(0, []))
print()
print("begehbare Punkte in Band 0: %d" % len(pts))
for (px, pz) in ((1870, 27980), (418, 26497), (1870, 27400), (1870, 28400)):
    if len(pts) == 0:
        continue
    dd = np.abs(pts[:, 0] - px) + np.abs(pts[:, 1] - pz)
    i = dd.argmin()
    print("   (%6d,%6d): naechster begehbarer Punkt (%6d,%6d), Manhattan %d"
          % (px, pz, pts[i, 0], pts[i, 1], dd[i]))
