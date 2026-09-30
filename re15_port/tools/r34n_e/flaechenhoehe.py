# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Hoehe einer waagerechten Flaeche aus ZWEI Hintergrundbildern.

Verfahren wie Runde 30 (r30_idw_tischhoehe.py, Irons-Tisch): jeder Bildpunkt eines Ausschnitts in
Cut A wird ueber seinen Sehstrahl (echte Inverse der Engine-Matrix, geom.py) auf die Ebene y = h
gelegt und in Cut B abgebildet. Stimmt h, zeigen beide Bilder dieselbe Stelle -> hohe
Korrelation (NCC der Helligkeit) und kleiner mittlerer Abstand (MAD). Kein Schwellwert, keine
Farbtrennung; das Mass hat einen Gipfel oder es hat keinen. Ausgegeben wird der ganze Verlauf.

Aufruf: python re15_port/tools/r34n_e/flaechenhoehe.py RAUM CUT_A CUT_B x0 x1 y0 y1 [h0 h1 schritt]
"""
import os
import sys

import numpy as np
from PIL import Image, ImageFilter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402


def bilin(A, x, y):
    x0 = int(np.floor(x))
    y0 = int(np.floor(y))
    fx = x - x0
    fy = y - y0
    if x0 < 0 or y0 < 0 or x0 + 1 >= A.shape[1] or y0 + 1 >= A.shape[0]:
        return None
    return (A[y0, x0] * (1 - fx) * (1 - fy) + A[y0, x0 + 1] * fx * (1 - fy)
            + A[y0 + 1, x0] * (1 - fx) * fy + A[y0 + 1, x0 + 1] * fx * fy)


def messe(raum, ca, cb, x0, x1, y0, y1, hs, glatt=1.0):
    cams = geom.lade_kameras(raum)
    grp = raum[:3]
    pa = os.path.join(geom.AUS, "bg", "ROOM%s%02d.ppm" % (grp, ca))
    pb = os.path.join(geom.AUS, "bg", "ROOM%s%02d.ppm" % (grp, cb))
    A = np.array(Image.open(pa).convert("RGB")).astype(float)
    B = np.array(Image.open(pb).convert("RGB").filter(ImageFilter.BoxBlur(glatt))).astype(float)
    out = []
    for h in hs:
        a = []
        b = []
        for sy in range(y0, y1):
            for sx in range(x0, x1):
                p, s = geom.auf_ebene(cams[ca], sx + 0.5, sy + 0.5, h)
                if s <= 0:
                    continue
                r = geom.projiziere(cams[cb], p)
                if r is None:
                    continue
                v = bilin(B, r[0] - 0.5, r[1] - 0.5)
                if v is None:
                    continue
                a.append(A[sy, sx])
                b.append(v)
        a = np.array(a)
        b = np.array(b)
        if len(a) < 100:
            out.append((h, len(a), float("nan"), float("nan")))
            continue
        ncc = float(np.corrcoef(a.mean(axis=1), b.mean(axis=1))[0, 1])
        mad = float(np.abs(a - b).mean())
        out.append((h, len(a), ncc, mad))
    return out


def main():
    raum, ca, cb = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    x0, x1, y0, y1 = (int(v) for v in sys.argv[4:8])
    h0, h1, st = (int(v) for v in sys.argv[8:11]) if len(sys.argv) >= 11 else (-1200, -50, 10)
    r = messe(raum, ca, cb, x0, x1, y0, y1, range(h0, h1 + 1, st))
    ok = [t for t in r if t[2] == t[2]]
    bn = max(ok, key=lambda t: t[2])
    bm = min(ok, key=lambda t: t[3])
    zweit = sorted(ok, key=lambda t: -t[2])
    print("ROOM%s Cut %d -> Cut %d, Ausschnitt x %d..%d y %d..%d: NCC-Gipfel h=%d (NCC %.3f, n=%d) | "
          "MAD-Tal h=%d (MAD %.2f)" % (raum, ca, cb, x0, x1, y0, y1, bn[0], bn[2], bn[1], bm[0], bm[3]))
    # zweiter Gipfel ausserhalb +-60 um den ersten (Eindeutigkeit)
    andere = [t for t in zweit if abs(t[0] - bn[0]) > 60]
    if andere:
        print("    naechstbester Wert ausserhalb +-60: h=%d NCC %.3f" % (andere[0][0], andere[0][2]))
    print("    NCC-Verlauf:", " ".join("%d:%.3f" % (t[0], t[2]) for t in r[::2]))


if __name__ == "__main__":
    main()
