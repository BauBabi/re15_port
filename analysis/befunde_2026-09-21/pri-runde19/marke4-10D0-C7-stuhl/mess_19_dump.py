# -*- coding: utf-8 -*-
"""Messung 19: Abdeckung des Standplatz-Dumps. Sieht die Schiene den Standort
des Nutzers (418, 26497) ueberhaupt?"""
import sys

import numpy as np

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402

floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", 0x10D0)
for band, pts in sorted(floor.items()):
    a = np.array(pts)
    print("Band %d: %d Punkte, x %d..%d, z %d..%d"
          % (band, len(a), a[:, 0].min(), a[:, 0].max(), a[:, 1].min(), a[:, 1].max()))
    xs = np.unique(a[:, 0])
    zs = np.unique(a[:, 1])
    print("   Raster: dx %s  dz %s" % (np.unique(np.diff(xs))[:5], np.unique(np.diff(zs))[:5]))

a = np.array(floor[0])
for z in (26350, 26550, 26750, 27750, 27950, 28150):
    xs = np.sort(a[a[:, 1] == z][:, 0])
    print("z=%d: %d Punkte, x %s%s" % (z, len(xs), xs[:14], " ..." if len(xs) > 14 else ""))
