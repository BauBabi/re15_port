# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Hoehe einer Moebel-Oberseite ueber die DECKFLAECHE (Verfahren 2).

Unabhaengig von jeder Kantendefinition (Verfahren 1 = kantenhoehe.py): fuer eine Hoehe h ist die
vorhergesagte Deckflaeche die Menge der Bildpunkte, deren Sehstrahl (echte Inverse der Engine-
Matrix, geom.py) die Ebene y = h VOR der Kamera innerhalb der SCA-Zelle trifft. Die gemalte
Deckflaeche ist die Menge der Holz-Pixel (Braeune R-B >= 30) im Auswertefenster. Gemessen wird
der Jaccard-Index beider Mengen je h (Vorbild: Memory reai-v2-quader-modell, "Deckflaeche" -
ROOM10D0 Tischplatte h=-1100 IoU 0,561, einseitiger Gipfel). Kein Schwellwert fuer h; Muetze und
Buegel liegen in beiden Mengen gleich und verschieben den Gipfel nicht.

Aufruf: python re15_port/tools/r34n_e/deckflaeche.py
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402

# (Raum, Cut, SCA-Zelle x0 x1 z0 z1, Auswertefenster x0 x1 y0 y1)
FAELLE = [
    ("1000", 0, (18800, 20650, -12450, -2750), (90, 232, 150, 239)),
    ("1000", 2, (18800, 20650, -12450, -2750), (130, 250, 195, 239)),
    ("1000", 1, (18800, 20650, -12450, -2750), (135, 215, 120, 239)),
    ("1010", 0, (-50, 2450, 1550, 5900), (140, 319, 120, 176)),
    # Marvins Schreibtisch: SCA-Zelle 2; die Platte ist hell gegen die dunklen Spinde dahinter,
    # daher HELLIGKEIT (Mittel RGB >= 60) statt Braeune; Fenster endet an der Vorderkante.
    ("1020", 6, (-10893, -9197, -17864, -15064), (122, 190, 92, 112), "hell"),
    ("1020", 3, (-10893, -9197, -17864, -15064), (122, 165, 86, 99), "hell"),
]


def main():
    for fall in FAELLE:
        raum, cut, (x0, x1, z0, z1), (a, b, c, d) = fall[:4]
        kanal = fall[4] if len(fall) > 4 else "braun"
        cams = geom.lade_kameras(raum)
        cam = cams[cut]
        A = geom.bg(raum, cut)
        holz = ((A[:, :, 0] - A[:, :, 2]) >= 30) if kanal == "braun" else (A.mean(axis=2) >= 60)
        Rinv = np.linalg.inv(cam["R"])
        wo = Rinv @ (-cam["T"])
        sys_ = np.arange(c, d + 1) + 0.5
        sxs = np.arange(a, b + 1) + 0.5
        SX, SY = np.meshgrid(sxs, sys_)
        D = np.stack([(SX - 160.0) / cam["H"], (SY - 120.0) / cam["H"], np.ones_like(SX)], -1) @ Rinv.T
        H = holz[c:d + 1, a:b + 1]
        res = []
        for h in range(-2000, -99, 5):
            s = (h - wo[1]) / D[:, :, 1]
            X = wo[0] + s * D[:, :, 0]
            Z = wo[2] + s * D[:, :, 2]
            pred = (s > 0) & (X >= x0) & (X <= x1) & (Z >= z0) & (Z <= z1)
            inter = float((pred & H).sum())
            uni = float((pred | H).sum())
            res.append((h, inter / uni if uni else 0.0, int(pred.sum()), int(H.sum())))
        best = max(res, key=lambda t: t[1])
        nb = {h: j for h, j, _, _ in res}
        andere = [t for t in res if abs(t[0] - best[0]) > 100]
        zweit = max(andere, key=lambda t: t[1]) if andere else None
        print("ROOM%s Cut %d Fenster x %d..%d y %d..%d: Holz-Pixel %d | Gipfel h = %d, Jaccard %.3f "
              "(h-50 %.3f, h+50 %.3f; bester Wert > 100 daneben: h=%s %.3f)"
              % (raum, cut, a, b, c, d, best[3], best[0], best[1], nb.get(best[0] - 50, float("nan")),
                 nb.get(best[0] + 50, float("nan")), zweit[0] if zweit else "-", zweit[1] if zweit else 0))


if __name__ == "__main__":
    main()
