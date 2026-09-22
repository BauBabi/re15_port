# -*- coding: utf-8 -*-
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bilder import lade

for cut, doppel in ((4, {7, 8, 9}), (5, {4, 5, 6})):
    masks, idx, clut, bl, regs = lade("ROOM10F0", cut)
    ges = sum(m[5] * m[6] for m in masks)
    dop = 0
    dop_opak = 0
    for o, (s, e, k, ax, ay) in enumerate(bl):
        if o not in doppel:
            continue
        for m in masks[s:e]:
            dop += m[5] * m[6]
            dop_opak += int((idx[m[1]:m[1] + m[6], m[0]:m[0] + m[5]] != 0).sum())
    belegt = int((idx != 0).sum())
    print("C%d: Kachelflaeche im Blatt gesamt %d von 65536 (%.1f %%); "
          "davon Doppel %d (%.1f %% des Blattes); opake Doppelpunkte %d"
          % (cut, ges, 100.0 * ges / 65536, dop, 100.0 * dop / 65536, dop_opak))
    print("    opake Punkte im Blatt: %d" % belegt)
