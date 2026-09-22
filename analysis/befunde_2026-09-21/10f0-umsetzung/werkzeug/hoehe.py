"""Wie hoch sind die Buerostuehle WIRKLICH? — gemessen an der Freistellung des Nutzers.

Die Quaderhoehe -1950 stammt aus der dritten Runde (2026-09-09, auswahl.json _warum):
"die schwarzen Lehnen laufen bis h~-1950 (an den Schwarz-Saeulen aufgeloest)". Das ist
am TIEFSCHWARZ gemessen - also an genau der Groesse, die hier im Verdacht steht.

Hier wird sie stattdessen an der KUNST gemessen: die Freistellung des Nutzers IST der
Stuhl. Fuer jede Hoehe h wird der Quader der zugehoerigen Zelle projiziert und gefragt,
ab welcher Hoehe er die Freistellung vollstaendig enthaelt. Das ist die kleinste Hoehe,
die der Nutzer-Kunst nichts wegnimmt - und zugleich die groesste, die belegt ist.
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import geom
import maske_aus_png
import raum
from geom import load_bg, load_rdt

CD = "re15_port/shared_assets/PSX"
PPM = "build/bg_ppm"


def zellen(rdt):
    return geom.sca_sperrzellen(rdt, 0) or []


def main():
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    sp = zellen(rdt)
    print("Typ-3-Zellen (Buerostuhl-Kreiszellen) im RDT:")
    for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
        if typ == 3:
            print("   Zelle %2d  x%6d..%6d  z%6d..%6d" % (ki, zx, zx + zw, zz, zz + zd))
    for cut in (4, 5):
        v = geom.cut_view(rdt, cam, cut)
        bg = load_bg(PPM, 0x10F0, cut)
        dk = bg.astype(int).sum(2) < 45
        print("\n=== CUT %d ===" % cut)
        for o in aus[str(cut)]["objekte"]:
            if "png" not in o:
                continue
            r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
            if r is None:
                continue
            ys, xs = np.nonzero(r)
            # Zelle mit groesster Ueberdeckung (wie raum.py)
            best, bki = 0.0, None
            for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
                if typ != 3:
                    continue
                _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
                f = float((tr & r).sum()) / float(r.sum())
                if f > best:
                    best, bki = f, ki
            zx, zz, zw, zd, _ = sp[bki]
            # kleinste Hoehe, deren Silhouette die Kunst ganz enthaelt
            noetig = None
            for h in range(-100, -2601, -25):
                _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, h)
                if not (r & ~tr).any():
                    noetig = h
                    break
            print('  "%s"' % o["name"])
            print("     Kunst %d px, Kasten x%d..%d y%d..%d, Zelle %d (Deckung %.3f)"
                  % (int(r.sum()), xs.min(), xs.max(), ys.min(), ys.max(), bki, best))
            print("     KLEINSTE Hoehe, die die Kunst ganz enthaelt: %s" % noetig)
            for h in (noetig, -1100, -1950):
                if h is None:
                    continue
                _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, h)
                yy, xx = np.nonzero(tr)
                print("        h=%5d: Silhouette %5d px (y%d..%d), Kunst darin %d/%d, "
                      "Tiefschwarz darin %d"
                      % (h, int(tr.sum()), yy.min(), yy.max(), int((tr & r).sum()),
                         int(r.sum()), int((tr & dk & ~r).sum())))


if __name__ == "__main__":
    main()
