#!/usr/bin/env python3
"""BAU-ABNAHME (Runde 30, Thema H): zeigen Inventar-Raster und CHECK-Foto die Rohr-Sicherung?

Vergleicht Framedumps der Inventar-Laeufe (lauf_inventar.sh, 960x720):
  * BAU      = der gebaute Stand (echte Dateien aus shared_assets, Bild/Icon im GELADENEN
               Puffer eingesetzt)
  * PROTO    = der Weg-2-Prototyp der Ermittlung (Mess-Wurzel cd_weg2 mit denselben Bytes
               auf der Platte)
  * BESTAND  = vor dem Bau

Gemessen je Feld:
  CHECK  Fotofeld  = blaues "Item data"-Feld x51..629 y171..455 (aus dem BESTAND-Bild
                     ausgelesen: blau-dominante Laeufe in Zeile/Spalte 300). Punkte, die
                     NICHT blau-dominant sind (b < r+40 oder b < g+40) = Foto.
  RASTER Icon-Feld = Inventarplatz 0, dieselbe Rechnung auf dem Bereich, in dem sich
                     BESTAND und PROTO unterscheiden (Icon-Zelle und Equip-Feld).
Dazu Punkt-fuer-Punkt BAU gegen PROTO (0 = bildgleich mit dem Prototyp).

    python bau_inventar_abnahme.py <bau_check> <proto_check> <bestand_check> \
                                   <bau_grid> <proto_grid> <bestand_grid>
Ordner relativ zu build/r30_sicherung/ oder absolut.
"""
import os
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
Z = os.path.join(REPO, "build", "r30_sicherung")


def bild(ordner, name):
    p = ordner if os.path.isabs(ordner) else os.path.join(Z, ordner)
    return np.asarray(Image.open(os.path.join(p, name)).convert("RGB")).astype(int)


def nicht_blau(a):
    return ~((a[..., 2] >= a[..., 0] + 40) & (a[..., 2] >= a[..., 1] + 40))


def feld(a, x0, x1, y0, y1):
    return a[y0:y1 + 1, x0:x1 + 1]


def main():
    if len(sys.argv) != 7:
        print(__doc__)
        return 2
    bau_c, pro_c, bes_c, bau_g, pro_g, bes_g = sys.argv[1:]
    fehler = 0

    # ---- CHECK, Bild 140 ----
    X0, X1, Y0, Y1 = 51, 629, 171, 455
    n_feld = (X1 - X0 + 1) * (Y1 - Y0 + 1)
    print("CHECK F140, Fotofeld x%d..%d y%d..%d (%d Punkte, 960x720)" % (X0, X1, Y0, Y1, n_feld))
    werte = {}
    for name, o in (("BESTAND", bes_c), ("PROTO", pro_c), ("BAU", bau_c)):
        a = bild(o, "f_000140.ppm")
        k = int(nicht_blau(feld(a, X0, X1, Y0, Y1)).sum())
        werte[name] = k
        print("  %-8s nicht blau: %6d" % (name, k))
    a = feld(bild(bau_c, "f_000140.ppm"), X0, X1, Y0, Y1)
    b = feld(bild(pro_c, "f_000140.ppm"), X0, X1, Y0, Y1)
    d = int((np.abs(a - b).max(2) > 0).sum())
    print("  BAU gegen PROTO im Fotofeld: %d abweichende Punkte" % d)
    if d != 0 or werte["BAU"] == 0:
        fehler += 1

    # ---- RASTER, Bild 50 ----
    A = bild(bes_g, "f_000050.ppm")
    P = bild(pro_g, "f_000050.ppm")
    B = bild(bau_g, "f_000050.ppm")
    dm = np.abs(A - P).max(2) > 0
    ys, xs = np.nonzero(dm)
    print("RASTER F50: BESTAND gegen PROTO %d Punkte, bbox x%d..%d y%d..%d" %
          (int(dm.sum()), xs.min(), xs.max(), ys.min(), ys.max()))
    d = int((np.abs(B - P).max(2) > 0).sum())
    d_best = int((np.abs(B - A).max(2) > 0).sum())
    print("  BAU gegen PROTO (ganzes Bild): %d abweichende Punkte" % d)
    print("  BAU gegen BESTAND (ganzes Bild): %d abweichende Punkte" % d_best)
    if d != 0 or d_best == 0:
        fehler += 1
    print("ERGEBNIS: %s" % ("BESTANDEN" if not fehler else "%d Pruefung(en) gerissen" % fehler))
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
