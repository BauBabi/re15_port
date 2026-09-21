# -*- coding: utf-8 -*-
"""Erzeugt die Referenz-Silhouetten fuer den Riegel
re15_port/tests/unit/probes/r19_marke4-10D0-C7-stuhl.cmake:

  r19_marke4_kasten.pbm  - der blaugraue Kasten am Tischende (273 Punkte), aus dem
                           HINTERGRUND gemessen (Regel s. bau_huelle.py)
  r19_marke4_nutzer.pbm  - die Freistellungen des Nutzers 07_01 + 07_02 an ihrer
                           gemessenen Lage (3663 Punkte). Schuetzt die Handarbeit:
                           kein spaeterer "Fix" darf sie wegschneiden.

P4, 320x240, MSB zuerst - gleiches Format wie abnahme.pbm_schreiben, damit der
C-Riegel den vorhandenen Leser benutzen kann.
"""
import numpy as np
from PIL import Image

ZIEL = "re15_port/tests/unit/probes"


def schreib(pfad, m):
    bits = np.packbits(np.asarray(m, bool), axis=1)
    with open(pfad, "wb") as f:
        f.write(b"P4\n320 240\n")
        f.write(bits.tobytes())
    print("%s: %d Punkte" % (pfad, int(m.sum())))


def setze(png, x, y):
    a = np.asarray(Image.open(png).convert("RGBA"), int)
    r = np.zeros((240, 320), bool)
    r[y:y + a.shape[0], x:x + a.shape[1]] = a[..., 3] > 110
    return r


schreib(ZIEL + "/r19_marke4_kasten.pbm", setze("pri/STAGE1/10D0/07_03.png", 46, 126))
schreib(ZIEL + "/r19_marke4_nutzer.pbm",
        setze("pri/STAGE1/10D0/07_01.png", 0, 123) | setze("pri/STAGE1/10D0/07_02.png", 0, 146))
