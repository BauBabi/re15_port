"""Was verliert / behaelt eine Variante gegenueber der Auslieferung?

Blau  = Maskentexel, die beide haben
Rot   = Texel, die NUR die Auslieferung hat (faellt weg)
Gruen = Texel, die NUR die Variante hat (kommt dazu)

Aufruf: python vergleichsbild.py <variantenbaum> <name>
"""
import os
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
sys.path.insert(0, HIER)
os.chdir(WURZEL)

import variante
from geom import load_bg

if __name__ == "__main__":
    verz, name = sys.argv[1], sys.argv[2]
    os.makedirs("build/r22/vergleich", exist_ok=True)
    for cut in (4, 5):
        a, _, _ = variante.deckung_tiefe("build/r22/v0_auslieferung", cut)
        b, _, _ = variante.deckung_tiefe(verz, cut)
        bg = load_bg("build/bg_ppm", 0x10F0, cut)
        im = bg.copy()
        im[a & b] = (70, 110, 255)
        im[a & ~b] = (255, 40, 40)
        im[b & ~a] = (40, 255, 40)
        Image.fromarray(im).resize((960, 720), Image.NEAREST).save(
            "build/r22/vergleich/%s_c%d.png" % (name, cut))
        print("cut %d: beide %5d | nur Auslieferung %5d | nur %s %5d -> %s"
              % (cut, int((a & b).sum()), int((a & ~b).sum()), name, int((b & ~a).sum()),
                 "build/r22/vergleich/%s_c%d.png" % (name, cut)))
