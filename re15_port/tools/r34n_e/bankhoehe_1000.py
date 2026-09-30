# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Hoehe der Bank-Oberseite in ROOM1000 (Nutzerbild elliot.bmp = Cut 0).

Nur Cut 0 sieht die Marke (Sonde `projekt`: alle anderen Cuts ausserhalb/hinter der Kamera) -
eine Flaechenkorrelation zweier Cuts wie beim Irons-Tisch (Runde 30) geht hier nicht. Stattdessen
ZWEI unabhaengige Bedingungen an die GEMALTEN Kanten derselben Platte:

  (1) FERNKANTE: die hintere Kante der Sitzflaeche liegt in der Welt auf EINER z-Linie (die Bank
      steht achsparallel: SCA-Zelle 7 x[18800..20650] z[-12450..-2750], Sonde `sca`). Ihre
      Bildpunkte, auf die Ebene y = h zurueckgelegt, haben nur beim richtigen h dieselbe z.
  (2) RECHTE KANTE: die rechte Laengskante liegt auf EINER x-Linie. Dieselbe Bedingung in x.

Kantenpunkte = staerkster Sprung der "Braeune" R-B (Holz gegen grauen Boden), je Spalte bzw.
Zeile; die Grenze liegt zwischen zwei Pixeln (stetige Koordinate = Index der unteren/rechten
Zeile). Ausreisser (Kleiderbuegel, Mütze) werden ueber den Median-Abstand verworfen und gezaehlt.
Kein Schwellwert fuer h: fuer jedes h im Raster die Streuung (Std) der z bzw. x - das Minimum ist
die Hoehe, zusammen mit der Kruemmung um das Minimum als Guete.

Aufruf: python re15_port/tools/r34n_e/bankhoehe_1000.py
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402


def kanten(A):
    br = A[:, :, 0] - A[:, :, 2]
    fern = []
    for x in range(150, 206):
        col = br[140:180, x]
        g = np.diff(col)
        i = int(np.argmax(g))
        if g[i] >= 30:
            fern.append((x + 0.5, 140 + i + 1.0))
    rechts = []
    for y in range(162, 236):
        row = br[y, 185:225]
        g = np.diff(row)
        i = int(np.argmin(g))
        if g[i] <= -20:
            rechts.append((185 + i + 1.0, y + 0.5))
    return fern, rechts


def ohne_ausreisser(pts, achse):
    """Kantenpunkte gegen eine Gerade (kleinste Quadrate) pruefen, > 2 px Abstand verwerfen."""
    p = np.array(pts)
    a, b = (1, 0) if achse == "zeile" else (0, 1)   # Fernkante: y = f(x); rechte Kante: x = f(y)
    k, d = np.polyfit(p[:, b], p[:, a], 1)
    r = np.abs(p[:, a] - (k * p[:, b] + d))
    gut = p[r <= 2.0]
    return gut, len(p) - len(gut), (k, d)


def streuung(cam, pts, h, welche):
    w = [geom.auf_ebene(cam, sx, sy, h)[0] for sx, sy in pts]
    v = np.array([p[2] if welche == "z" else p[0] for p in w])
    return float(v.std()), float(v.mean())


def main():
    cams = geom.lade_kameras("1000")
    A = geom.bg("1000", 0)
    fern, rechts = kanten(A)
    fern, nf, lf = ohne_ausreisser(fern, "zeile")
    rechts, nr, lr = ohne_ausreisser(rechts, "spalte")
    print("Fernkante: %d Punkte (verworfen %d), Bildgerade y = %.4f x + %.2f" % (len(fern), nf, lf[0], lf[1]))
    print("rechte Kante: %d Punkte (verworfen %d), Bildgerade x = %.4f y + %.2f" % (len(rechts), nr, lr[0], lr[1]))
    hs = list(range(-900, -99, 10))
    for name, pts, welche in (("(1) Fernkante -> gleiche z", fern, "z"), ("(2) rechte Kante -> gleiche x", rechts, "x")):
        werte = [(h,) + streuung(cams[0], pts, h, welche) for h in hs]
        best = min(werte, key=lambda t: t[1])
        print("%s: Minimum der Streuung bei h = %d (Std %.1f, Mittel %s = %.0f)" % (name, best[0], best[1], welche, best[2]))
        print("    Verlauf:", " ".join("%d:%.0f" % (h, s) for h, s, _ in werte[::5]))
        # Guete: wie scharf ist das Minimum? Std bei h +- 100
        nb = {h: s for h, s, _ in werte}
        print("    Std bei h-100 / h / h+100: %.1f / %.1f / %.1f" % (nb.get(best[0] - 100, float('nan')), best[1],
                                                                  nb.get(best[0] + 100, float('nan'))))


if __name__ == "__main__":
    main()
