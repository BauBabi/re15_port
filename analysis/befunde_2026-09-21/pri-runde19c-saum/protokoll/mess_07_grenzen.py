"""Grenzen: Rechteckzahl gegen die Engine-Schranke RE15_PRI_MAX_MASKS_PER_CUT = 105
(re15_port/include/re15_pri.h:50, belegt aus ROOM3000/3001 Cut 3, hdr byte[7] = 0x69)
und Atlasflaeche gegen das 256x256-Blatt (tools/maske/atlas.py:24-25). Beide Staende
aus den GESCHRIEBENEN Dateien gelesen, nicht aus dem Bauprotokoll.
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import maskenbild                                           # noqa: E402

CD = "re15_port/shared_assets/PSX"
ROOM, CUT = "ROOM10D0", 7
for nm, d in (("VORHER ", os.path.join(CD, "MASKS")), ("NACHHER", "build/r19c/neu_MASKS")):
    blob = open(os.path.join(d, "%s.MSK" % ROOM), "rb").read()
    ms = maskenbild.masken(blob, CUT) or []
    tim = open(os.path.join(d, "%s_PRI%02d.TIM" % (ROOM, CUT)), "rb").read()
    idx = maskenbild.lies_tim_bytes(tim)[0]
    flaeche = sum(m[4] * m[5] for m in ms)
    bel = np.zeros(idx.shape, bool)
    for (sx, sy, X, Y, w, h, dep) in ms:
        bel[sy:sy + h, sx:sx + w] = True
    print("%s Rechtecke %3d von 105 (%.0f %% der Schranke) | Atlasblatt %dx%d, "
          "belegte Flaeche %d von %d (%.1f %%), Summe der Rechteckflaechen %d | TIM %d Bytes"
          % (nm, len(ms), 100.0 * len(ms) / 105, idx.shape[1], idx.shape[0],
             int(bel.sum()), idx.size, 100.0 * bel.sum() / idx.size, flaeche, len(tim)))
    tiefen = sorted(set(m[6] for m in ms))
    print("         Tiefenstufen %d (%d..%d)" % (len(tiefen), tiefen[0], tiefen[-1]))
