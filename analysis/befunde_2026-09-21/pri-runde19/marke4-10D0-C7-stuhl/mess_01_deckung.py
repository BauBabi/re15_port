# -*- coding: utf-8 -*-
"""Messung 1: ueberlebt die Nutzer-Freistellung die Pipeline bis in die .PBM?

Soll  = Alpha-Punkte der Nutzer-PNGs an ihrer eingetragenen Lage.
Deckung = die geschriebene ROOM10D0_PRI07.PBM (P4, 1-Bit = gedeckt).
"""
import numpy as np
from PIL import Image

COV = ~np.asarray(Image.open("re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM"))
OBJ = (("07_01.png", (0, 123), 110, "Stuhl (Nutzer)"),
       ("07_02.png", (0, 146), 110, "Tischplattenecke (Nutzer)"))

soll = np.zeros((240, 320), bool)
for nm, (px, py), thr in [(o[0], o[1], o[2]) for o in OBJ]:
    a = np.asarray(Image.open("pri/STAGE1/10D0/" + nm).convert("RGBA"))
    h, w = a.shape[:2]
    s = np.zeros((240, 320), bool)
    s[py:py + h, px:px + w] = a[..., 3] >= thr
    fehlt = s & ~COV
    print("%-12s Soll %4d  gedeckt %4d  FEHLT %4d (%.1f%%)"
          % (nm, s.sum(), (s & COV).sum(), fehlt.sum(),
             100.0 * fehlt.sum() / max(1, s.sum())))
    if fehlt.sum():
        ys, xs = np.nonzero(fehlt)
        print("             fehlend x %d..%d  y %d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
    soll |= s

print("Nutzer-Soll gesamt %d, Deckung der PBM %d" % (soll.sum(), COV.sum()))
print("Deckung OHNE Nutzer-Soll (= Quader-Anteil) %d" % (COV & ~soll).sum())
