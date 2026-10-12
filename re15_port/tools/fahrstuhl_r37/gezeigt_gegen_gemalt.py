#!/usr/bin/env python3
"""gezeigt_gegen_gemalt.py - Runde 37 Spur C: gezeigte Fahrstuhltuer (Bogen-Bild der Sequenz, Bild "anfang") gegen
das gemalte Fluegelfeld (gemalt_fluegel.py). Messwerkzeug, keine Spiellogik.

Abbildung Zeile <-> Fluegelhoehe: Tuerkamera Auge x = 10000 (tor_sequenz_bauen.KAMERA, Runde 31: Bild-x = 160 +
290 * z / (10000 - x)), Vorderseite der Fluegel x = 2000 + DICKE_HALB = 2100 -> Bild-y = 120 + 290 * y / 7900;
Fluegel y -2800 (oben) .. 3800 (unten) = obj-Lage y 3800 (DOOR27 Skript 1) + Netz -6600..0.
Aufruf: python gezeigt_gegen_gemalt.py <anfang.ppm> <feld.png> [x0 x1]
"""
import sys

import numpy as np
from PIL import Image

LUM = np.array([0.299, 0.587, 0.114])


def main():
    bild = np.asarray(Image.open(sys.argv[1]).convert("RGB"), np.float64)
    feld = np.asarray(Image.open(sys.argv[2]).convert("RGB"), np.float64)
    x0, x1 = (int(sys.argv[3]), int(sys.argv[4])) if len(sys.argv) > 4 else (96, 224)
    fuge = (x0 + x1) // 2
    spalten = [x for x in range(x0, x1) if abs(x - fuge) > 3]
    zeilen = []
    for r in range(4, 120, 12):                       # Feldzeile r (0..127) = Fluegel v 2r
        y_welt = -2800 + (2 * r + 1) / 256.0 * 6600
        yb = int(round(120 + 290 * y_welt / 7900.0))
        if not 0 <= yb < bild.shape[0]:
            continue
        gez = np.median(bild[yb, spalten], 0)
        gem = np.median(feld[r], 0)
        zeilen.append((r, yb, gez, gem))
    print("Feldzeile Bildzeile  gezeigt(R,G,B)        gemalt(R,G,B)        L gez/gem")
    for r, yb, a, b in zeilen:
        print("%9d %10d  (%5.1f,%5.1f,%5.1f)  (%5.1f,%5.1f,%5.1f)  %5.1f / %5.1f" % (r, yb, *a, *b, a @ LUM, b @ LUM))
    tuer = bild[16:236, spalten].reshape(-1, 3)
    tuer = tuer[(tuer @ LUM) > 1.5]
    print("Median Tuerflaeche gezeigt", np.median(tuer, 0).round(1), " gemalt", np.median(feld.reshape(-1, 3), 0).round(1))
    d = np.array([abs(a @ LUM - b @ LUM) for _, _, a, b in zeilen])
    print("|L gezeigt - L gemalt| je Zeile: max %.1f, Mittel %.1f" % (d.max(), d.mean()))


if __name__ == "__main__":
    main()
