"""Die Figur an der Nutzer-Marke gegen eine Maskenvariante — Bild und Herkunft.

Die Figur kommt als ROHE Kamera-z-Karte aus der Sonde (R22_FIGDUMP, 320x240 int32,
0 = nicht gezeichnet); das Urteil ist dasselbe wie in der Sonde und im Original:
verdeckt <=> maskentiefe < (1023*vz)>>16  (re15_pri.h:104/120).

Aufruf:  python marke.py <maskenbaum> [<bildname>]
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
import geom

FIGDUMP = "build/r22/fig_f335.bin"


def figur():
    fz = np.fromfile(FIGDUMP, np.int32).reshape(240, 320)
    return fz


def eimer(vz):
    return (1023 * vz.astype(np.int64)) >> 16


def urteil(verz, cut, fz):
    deck, tief, n = variante.deckung_tiefe(verz, cut)
    gez = fz > 0
    bk = eimer(np.where(gez, fz, 1))
    verd = gez & deck & (tief < bk)
    return deck, tief, gez, verd


if __name__ == "__main__":
    verz = sys.argv[1]
    name = sys.argv[2] if len(sys.argv) > 2 else os.path.basename(verz.rstrip("/\\"))
    fz = figur()
    deck, tief, gez, verd = urteil(verz, 4, fz)
    # Herkunft: Kunst des Nutzers gegen Tiefschwarz-Zugabe
    import json
    import struct
    import maske_aus_png
    import raum
    from geom import load_bg
    auswahl = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]["4"]
    kunst = np.zeros((240, 320), bool)
    for o in auswahl["objekte"]:
        if "png" in o:
            r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
            if r is not None:
                kunst |= r
    bg = load_bg("build/bg_ppm", 0x10F0, 4)
    durchkunst = verd & kunst
    durchzugabe = verd & ~kunst
    print("%-24s gezeichnet %d  verdeckt %d (%.1f%%)  davon Kunst %d  Zugabe %d"
          % (name, int(gez.sum()), int(verd.sum()),
             100.0 * verd.sum() / max(1, gez.sum()),
             int(durchkunst.sum()), int(durchzugabe.sum())))
    ys = sorted(set(np.nonzero(gez)[0]))
    for y in ys:
        g = int(gez[y].sum())
        v = int(verd[y].sum())
        if g:
            print("   y=%3d ges=%2d verdeckt=%2d  Kunst=%2d Zugabe=%2d"
                  % (y, g, v, int(durchkunst[y].sum()), int(durchzugabe[y].sum())))
    im = bg.copy()
    im[gez & ~verd] = (110, 120, 255)
    im[durchkunst] = (255, 150, 0)
    im[durchzugabe] = (255, 30, 30)
    yy, xx = np.nonzero(gez)
    y0, y1 = max(0, yy.min() - 6), min(240, yy.max() + 6)
    x0, x1 = max(0, xx.min() - 14), min(320, xx.max() + 14)
    os.makedirs("build/r22/marke", exist_ok=True)
    Image.fromarray(im[y0:y1, x0:x1]).resize(((x1 - x0) * 8, (y1 - y0) * 8), Image.NEAREST) \
        .save("build/r22/marke/%s.png" % name)
    print("   Bild build/r22/marke/%s.png" % name)
