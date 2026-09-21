# -*- coding: utf-8 -*-
"""Messung 20: ABDECKUNG der Abnahme-Schiene fuer Marke 4.

Sieht die Standplatz-Schiene den Standort (418, 26497) des Nutzers?
Und welche soliden Zellen enthalten ihn?
"""
import os
import sys

import numpy as np

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
PX, PZ = 418, 26497
print("solide Zellen, die (%d, %d) enthalten:" % (PX, PZ))
tr = False
for (x, z, w, d) in geom.sca_wandzellen(rdt, 0):
    if x <= PX < x + w and z <= PZ < z + d:
        print("   x%6d..%6d  z%6d..%6d" % (x, x + w, z, z + d))
        tr = True
if not tr:
    print("   keine")

floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", 0x10D0)
a = np.array(floor[0])
fen = a[(np.abs(a[:, 0] - PX) <= 600) & (np.abs(a[:, 1] - PZ) <= 600)]
print()
print("begehbare Dump-Punkte im Umkreis 600 um den Standort: %d" % len(fen))
print(sorted(map(tuple, fen)))
print()
# Liegt der Standort in einem 200er-Raster-Loch?
for z in (26350, 26550):
    xs = np.sort(a[a[:, 1] == z][:, 0])
    nah = xs[(xs > -1500) & (xs < 2500)]
    print("z=%d, x-1500..2500 begehbar: %s" % (z, nah))
