# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: GEGENPROBE des Lichtfaktors am ECHTEN Renderer.

Frage: ist das Licht-Modell des Kontrollabzugs (EIN Faktor je Raum = Vertexfarbe der Normalen -Y aus
der Sonde `licht` / 128, ROOM1010: (41,39,39)/128 = 0,32) das, was die echte exe zeigt?
Probe: das ORIGINAL-Item First Aid Spray (ROOM1010 obj 0, RDT @0x00930 (200,-1600,5500) rot_y 3084)
steht in Cut 0 genau unter der Nutzermarke. Die echte exe zeigt es im Nullbild (raum_lauf.sh,
Framedump 960x720 -> BOX auf 320x240). Der Abzug zeichnet dasselbe Prop mit kontrollabzug.py.
Verglichen wird das Mittel-RGB ueber die Pixel, die im ABZUG vom Hintergrund abweichen (Maske aus dem
Abzug, auf beide Bilder angewandt; Rand 1 px abgetragen gegen Kantenversatz).

Aufruf: python re15_port/tools/r34n_e/licht_gegenprobe.py <framedump.ppm> [cut]
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402
import kontrollabzug as K  # noqa: E402


def main():
    echt_pfad = sys.argv[1]
    cut = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    cams = geom.lade_kameras("1010")
    ep, et = K.rdt_prop("1010", 0)
    bg = Image.fromarray(geom.bg("1010", cut).astype(np.uint8))
    SS = K.SS
    ergebnis = {}
    for name, licht in (("abzug_0,32", (41 / 128, 39 / 128, 39 / 128)), ("abzug_1,00", (1.0, 1.0, 1.0))):
        gross = np.array(bg.resize((320 * SS, 240 * SS), Image.NEAREST)).astype(float)
        zbuf = np.full(gross.shape[:2], 1e18)
        K.modell_zeichnen(gross, zbuf, cams[cut], (200, -1600, 5500), 3084, ep, et, np.array(licht))
        maske_g = zbuf < 1e18
        klein = np.array(Image.fromarray(gross.astype(np.uint8)).resize((320, 240), Image.BOX)).astype(float)
        m = np.array(Image.fromarray((maske_g * 255).astype(np.uint8)).resize((320, 240), Image.BOX)) >= 255
        ergebnis[name] = (klein, m)
    klein, m = ergebnis["abzug_0,32"]
    # Rand abtragen
    mm = m.copy()
    mm[1:, :] &= m[:-1, :]
    mm[:-1, :] &= m[1:, :]
    mm[:, 1:] &= m[:, :-1]
    mm[:, :-1] &= m[:, 1:]
    echt = np.array(Image.open(echt_pfad).convert("RGB").resize((320, 240), Image.BOX)).astype(float)
    ys, xs = np.nonzero(mm)
    print("Spray-Maske (Abzug, Rand 1 px ab): %d Pixel, x %d..%d y %d..%d" % (len(xs), xs.min(), xs.max(), ys.min(), ys.max()))
    for name, (bild, _) in ergebnis.items():
        mu = bild[mm].mean(axis=0)
        print("  %-12s Mittel-RGB (%.1f, %.1f, %.1f)  Y %.1f" % (name, mu[0], mu[1], mu[2], 0.299 * mu[0] + 0.587 * mu[1] + 0.114 * mu[2]))
    mu = echt[mm].mean(axis=0)
    print("  %-12s Mittel-RGB (%.1f, %.1f, %.1f)  Y %.1f" % ("ECHTE exe", mu[0], mu[1], mu[2], 0.299 * mu[0] + 0.587 * mu[1] + 0.114 * mu[2]))
    bgk = np.array(bg).astype(float)
    mu = bgk[mm].mean(axis=0)
    print("  %-12s Mittel-RGB (%.1f, %.1f, %.1f)  Y %.1f" % ("Hintergrund", mu[0], mu[1], mu[2], 0.299 * mu[0] + 0.587 * mu[1] + 0.114 * mu[2]))


if __name__ == "__main__":
    main()
