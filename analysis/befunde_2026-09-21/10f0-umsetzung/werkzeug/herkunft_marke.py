"""Welche ZELLE deckt jeden faelschlich verdeckten Figurpunkt an der Marke?

Die Tiefschwarz-Regel etikettiert jeden dunklen Bildpunkt mit EINER Stuhl-Zelle
(raum.py:346 bzw. :358). Diese Auswertung traegt das Etikett an die Marke heran und
zeigt, woher die Verdeckung stammt: aus der Zelle des Stuhls, der wirklich vor Leon
steht, oder aus einer ganz anderen.
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
sys.path.insert(0, HIER)
os.chdir(WURZEL)

import geom
import maske_aus_png
import raum
import variante
from geom import load_bg, load_rdt
from scipy import ndimage as nd

CD = "re15_port/shared_assets/PSX"
CUT = 4


def etiketten(cut):
    """kunst_label / dunkel_label wie raum.py sie baut."""
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"][str(cut)]
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    sp = geom.sca_sperrzellen(rdt, 0) or []
    v = geom.cut_view(rdt, cam, cut)
    bg = load_bg("build/bg_ppm", 0x10F0, cut)
    sil = {}
    for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
        if typ == 3:
            sil[ki] = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
    kunst = np.full((240, 320), -1, int)
    for o in aus["objekte"]:
        if "png" not in o:
            continue
        r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
        if r is None:
            continue
        lab, n = nd.label(r)
        for k in range(1, n + 1):
            comp = lab == k
            if comp.sum() < 20:
                continue
            bf, bki = 0.0, None
            for ki, (vzq, trq) in sil.items():
                f = float((trq & comp).sum()) / float(comp.sum())
                if f > bf:
                    bf, bki = f, ki
            if bki is not None:
                kunst[comp] = bki
    dkl = np.full((240, 320), -1, int)
    dk = bg.astype(int).sum(2) < 45
    naechste = np.full((240, 320), np.inf)
    for ki in sorted(set(int(x) for x in kunst[kunst >= 0])):
        dist = nd.distance_transform_edt(~(kunst == ki))
        m = dk & sil[ki][1] & (dist <= 7) & (dist < naechste)
        naechste[m] = dist[m]
        dkl[m] = ki
    regel = np.where(dkl >= 0, 1, 0)
    rest = dk & (dkl < 0)
    naechstes = np.full((240, 320), np.inf)
    for ki, (vzq, trq) in sil.items():
        m = rest & trq & (vzq < naechstes)
        naechstes[m] = vzq[m]
        dkl[m] = ki
        regel[m] = 2
    return kunst, dkl, regel, sp


def main():
    kunst, dkl, regel, sp = etiketten(CUT)
    fz = np.fromfile("build/r22/fig_f335.bin", np.int32).reshape(240, 320)
    gez = fz > 0
    bk = (1023 * np.where(gez, fz, 1).astype(np.int64)) >> 16
    d0, t0, _ = variante.deckung_tiefe("build/r22/v0_auslieferung", CUT)
    verd = gez & d0 & (t0 < bk)
    zug = verd & (kunst < 0)
    print("MARKE: gezeichnet %d, verdeckt %d, davon ohne Nutzer-Kunst %d"
          % (int(gez.sum()), int(verd.sum()), int(zug.sum())))
    print("\nZugabe nach ETIKETT:")
    for ki in sorted(set(int(x) for x in dkl[zug])):
        m = zug & (dkl == ki)
        zx, zz, zw, zd, _ = sp[ki]
        ys, xs = np.nonzero(m)
        hat_kunst = (kunst == ki).any()
        print("   Zelle %2d (x%6d z%6d, %s)  %3d Punkte, Zeilen %d..%d, "
              "Regel1 %d / Regel2 %d"
              % (ki, zx, zz, "MIT Nutzer-Kunst" if hat_kunst else "ohne Kunst",
                 int(m.sum()), ys.min(), ys.max(),
                 int((m & (regel == 1)).sum()), int((m & (regel == 2)).sum())))
    print("\nZugabe nach ZEILE (ges = Figurpunkte der Zeile):")
    for y in range(240):
        if zug[y].any():
            zellen = sorted(set(int(x) for x in dkl[y][zug[y]]))
            print("   y=%3d  Zugabe %2d  Zellen %s" % (y, int(zug[y].sum()), zellen))
    # Ueber der Kunst?
    print("\nLiegt die Zugabe UEBER der Kunst ihrer eigenen Zelle?")
    for ki in sorted(set(int(x) for x in dkl[zug])):
        if not (kunst == ki).any():
            continue
        oben = int(np.nonzero(kunst == ki)[0].min())
        m = zug & (dkl == ki)
        ys = np.nonzero(m)[0]
        print("   Zelle %2d: Kunst beginnt y=%d, Zugabe-Zeilen %d..%d, davon ueber der "
              "Kunst %d von %d" % (ki, oben, ys.min(), ys.max(), int((ys < oben).sum()),
                                   int(m.sum())))


if __name__ == "__main__":
    main()
