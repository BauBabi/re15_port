# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Lage der "Armory Notice" auf dem Verhoertisch ROOM1010 (Cut 0).

Befund (Sonde `projekt`, RDT ROOM1010.RDT sub00 @0x00930): genau unter der roten Nutzer-Marke
(interrogation.bmp, x 208..231 y 166..187, Mitte (220,177)) steht im Spiel das ORIGINAL-Item
First Aid Spray (Item_aot_set Slot 2 @0x00996, Id 0x22; ROOM1011: Id 0x39) als Modell obj 0 bei
(200,-1600,5500), rot_y 3084, bbox x -79..79 y -489..0 z -90..90 (Sonde `modelle`) - Bildpunkt
des Fusses (227,9;177,7). Das Nutzerbild ist der reine Hintergrund (mittlere Abweichung 3,8 gegen
Cut 0), der Nutzer hat die Dose also nicht gesehen. "Ort in etwa markiert" -> das Blatt kommt
NEBEN die Dose, so nah an die Marke wie moeglich.

REGEL (Port-Wahl, keine Original-Adresse; jede Groesse gemessen):
  * Tischplatte y = -1600 (Original: beide Tisch-Items stehen bei y = -1600, RDT @0x00930/@0x00952)
  * Tisch = SCA-Zelle 5 x[-50..2450] z[1550..5900] (Sonde `sca 1010`), Blatt ganz darauf (Rand 30)
  * Blatt = mesh04 Grundriss x -144..142, z -225..211 (bbox, Sonde/md1), Drehung rot_y
  * Abstand Blatt-Grundriss <-> Dosen-Grundriss (Kreis Radius 120 = Umkreis der bbox 79/90) > 0
  * Ziel: groesster sichtbarer Anteil des Blatts IN der roten Marke (Bildflaeche Cut 0), wobei die
    Dose (naeher an der Kamera -> verdeckt) ihre Silhouette vom Blatt abzieht.
Gerastert wird mit der Engine-Matrix (geom.py) 4x ueberabgetastet, Kandidaten auf einem 20er-Raster.

Aufruf: python re15_port/tools/r34n_e/lage_1010.py
"""
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402

SS = 4
TISCH = (-50, 2450, 1550, 5900)
Y = -1600
DOSE = (200, 5500)
DOSE_R = 120
MARKE = (208, 166, 231, 187)
BLATT = (-144, 142, -225, 211)


def welt(pos, ry, mx, mz):
    a = ry / 4096.0 * 2 * math.pi
    c, s = math.cos(a), math.sin(a)
    return (pos[0] + c * mx + s * mz, pos[1] - s * mx + c * mz)


def poly_maske(cam, pts3):
    img = Image.new("L", (320 * SS, 240 * SS), 0)
    q = []
    for p in pts3:
        r = geom.projiziere(cam, p)
        if r is None:
            return None, None
        q.append((r[0] * SS, r[1] * SS))
    ImageDraw.Draw(img).polygon(q, fill=255)
    vz = np.mean([geom.projiziere(cam, p)[2] for p in pts3])
    return np.array(img) > 0, vz


def main():
    cam = geom.lade_kameras("1010")[0]
    marke = np.zeros((240 * SS, 320 * SS), bool)
    marke[MARKE[1] * SS:(MARKE[3] + 1) * SS, MARKE[0] * SS:(MARKE[2] + 1) * SS] = True
    # Dosen-Silhouette: Zylinder als 16-Eck, unten y=-1600, oben y=-2089
    ring = [(DOSE[0] + 85 * math.cos(k * math.pi / 8), DOSE[1] + 85 * math.sin(k * math.pi / 8)) for k in range(16)]
    dose = np.zeros_like(marke)
    for i in range(16):
        a, b = ring[i], ring[(i + 1) % 16]
        m, _ = poly_maske(cam, [(a[0], Y, a[1]), (b[0], Y, b[1]), (b[0], Y - 489, b[1]), (a[0], Y - 489, a[1])])
        dose |= m
    top, _ = poly_maske(cam, [(x, Y - 489, z) for x, z in ring])
    dose |= top
    best = []
    drehungen = [int(v) for v in os.environ.get("R34N_E_DREHUNG", "").split(",") if v] or list(range(0, 4096, 256))
    for ry in drehungen:
        for x in range(-50, 900, 20):
            for z in range(5000, 5900, 20):
                ecken = [welt((x, z), ry, mx, mz) for mx in BLATT[:2] for mz in BLATT[2:]]
                if any(not (TISCH[0] + 30 <= ex <= TISCH[1] - 30 and TISCH[2] + 30 <= ez <= TISCH[3] - 30)
                       for ex, ez in ecken):
                    continue
                # Abstand Rechteck (gedreht) <-> Dosenkreis: in Modellkoordinaten des Blatts
                a = ry / 4096.0 * 2 * math.pi
                c, s = math.cos(a), math.sin(a)
                dx, dz = DOSE[0] - x, DOSE[1] - z
                mx = c * dx - s * dz
                mz = s * dx + c * dz
                qx = min(max(mx, BLATT[0]), BLATT[1])
                qz = min(max(mz, BLATT[2]), BLATT[3])
                if math.hypot(mx - qx, mz - qz) <= DOSE_R:
                    continue
                pts = [(ex, Y, ez) for ex, ez in (ecken[0], ecken[1], ecken[3], ecken[2])]
                m, vz = poly_maske(cam, pts)
                if m is None:
                    continue
                sichtbar = m & ~dose
                treffer = (sichtbar & marke).sum()
                best.append((treffer / float(marke.sum()), sichtbar.sum() / float(max(1, m.sum())), x, z, ry,
                             int(m.sum() / SS / SS)))
    best.sort(reverse=True)
    print("Kandidaten: %d gueltige Lagen (auf dem Tisch, Dose nicht beruehrt)" % len(best))
    print("  Anteil der Marke gedeckt | sichtbarer Anteil des Blatts | x z rot_y | Blattflaeche (px)")
    for b in best[:12]:
        print("  %.3f | %.3f | %5d %5d %4d | %d" % b)


if __name__ == "__main__":
    main()
