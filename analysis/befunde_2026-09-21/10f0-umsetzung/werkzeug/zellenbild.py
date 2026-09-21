"""Je Buerostuhl-Zelle: was liegt wirklich in ihrer Quader-Silhouette?

Rot = Tiefschwarz in der Silhouette (das, was die Regel als Stuhl fuehrt),
Gruen = Kunst des Nutzers, Gelb = Silhouettenrand. Ein Bild je Zelle, damit man
SIEHT, ob dort ein Stuhl steht oder die Wand.
"""
import json
import os
import struct
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import geom
import maske_aus_png
import raum
from geom import load_bg, load_rdt
from scipy import ndimage as nd

CD = "re15_port/shared_assets/PSX"
PPM = "build/bg_ppm"
OUT = "build/r22/zellen"


def main():
    os.makedirs(OUT, exist_ok=True)
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    sp = geom.sca_sperrzellen(rdt, 0) or []
    for cut in (4, 5):
        v = geom.cut_view(rdt, cam, cut)
        bg = load_bg(PPM, 0x10F0, cut)
        dk = bg.astype(int).sum(2) < 45
        kunst = np.zeros((240, 320), bool)
        for o in aus[str(cut)]["objekte"]:
            if "png" in o:
                r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
                if r is not None:
                    kunst |= r
        gebraucht = [tuple(int(x) for x in o["quader"][:2])
                     for o in aus[str(cut)]["objekte"] if "quader" in o]
        for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
            if typ != 3:
                continue
            _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
            if not tr.any():
                continue
            im = bg.copy()
            rand = tr & ~nd.binary_erosion(tr)
            im[tr & dk & ~kunst] = (255, 40, 40)
            im[tr & kunst] = (40, 255, 40)
            im[rand] = (255, 255, 0)
            ys, xs = np.nonzero(tr)
            Image.fromarray(im).resize((960, 720), Image.NEAREST).save(
                os.path.join(OUT, "c%d_z%02d.png" % (cut, ki)))
            print("cut %d zelle %2d x%6d z%6d  Silhouette %5d px y%3d..%3d  "
                  "Tiefschwarz %5d  Kunst %4d  %s"
                  % (cut, ki, zx, zz, int(tr.sum()), ys.min(), ys.max(),
                     int((tr & dk & ~kunst).sum()), int((tr & kunst).sum()),
                     "QUADER-EINTRAG" if (zx, zz) in gebraucht else "-"))


if __name__ == "__main__":
    main()
