# -*- coding: utf-8 -*-
"""Wie fein ginge es ueberhaupt? Kachelzahl und Atlasflaeche je EINHEITLICHER Kante."""
import os, sys
import numpy as np
sys.path.insert(0, "re15_port/tools/maske")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom
from gewinn import regionen, KANTEN, KAP, BUDGET

for cut, doppel in ((4, {7, 8, 9}), (5, {4, 5, 6})):
    regs, masks = regionen("ROOM10F0", cut)
    kosten = [geom.gitter_kosten(r, KANTEN) for (r, k, n) in regs]
    behalten = [i for i in range(len(regs)) if i not in doppel]
    print("=== ROOM10F0 C%d" % cut)
    print("  %5s %10s %10s | %10s %10s" % ("Kante", "n (alle)", "Atlas", "n (ohne D)", "Atlas"))
    for k in KANTEN:
        na = sum(kosten[i].get(k, (0, 0))[0] for i in range(len(regs)))
        fa = sum(kosten[i].get(k, (0, 0))[1] for i in range(len(regs)))
        nb = sum(kosten[i].get(k, (0, 0))[0] for i in behalten)
        fb = sum(kosten[i].get(k, (0, 0))[1] for i in behalten)
        mk = ""
        if nb <= BUDGET and fb <= KAP:
            mk = "  <- passt ohne Doppel"
        if na <= BUDGET and fa <= KAP:
            mk += "  (passt auch mit)"
        print("  %5d %10d %10d | %10d %10d%s" % (k, na, fa, nb, fb, mk))
