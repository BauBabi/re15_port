"""leisten_nb3.py - Nachbesserung G5 3: Vergleichsleisten RE2 echt | Port Leon 5090 | Port Elza 5091 je Szenenbild und Farbmessung der Masse.

Aufruf: python leisten_nb3.py <re2_bilder> <leon_bilder> <elza_bilder> <ziel_ordner>
Bilddateien: re2_%04d.png / port_%04d.png mit Index Szenenbild + 100 (wie abnahme2/werkzeug/port_bilder.py, re2_zeit.py).
Farbe der Masse wie Abnahme G5 2 (2.): Mittelwert R/G/B der Flaeche x 40..140, y 160..230 im 320x240-Bild.
"""
import sys, os
import numpy as np
from PIL import Image, ImageDraw

re2, leon, elza, ziel = sys.argv[1:5]
quellen = [("RE2 echt", os.path.join(re2, "re2_%04d.png")), ("Port Leon 5090", os.path.join(leon, "port_%04d.png")),
           ("Port Elza 5091", os.path.join(elza, "port_%04d.png"))]
W, H = 320, 240


def leiste(name, ns):
    img = Image.new("RGB", (W * len(quellen), (H + 16) * len(ns)), (0, 0, 0))
    dr = ImageDraw.Draw(img)
    for r, n in enumerate(ns):
        for c, (nm, pat) in enumerate(quellen):
            p = pat % (n + 100)
            if os.path.exists(p):
                im = Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR)
                img.paste(im, (c * W, r * (H + 16) + 16))
            dr.text((c * W + 4, r * (H + 16) + 2), "%s  Bild %d" % (nm, n), fill=(255, 255, 0))
    out = os.path.join(ziel, name)
    img.save(out, quality=88)
    print(out)


leiste("vergleich_nb3_1_bild165_313_479_677.jpg", [165, 313, 479, 677])
leiste("vergleich_nb3_2_schnitt16_bild180_240_360.jpg", [180, 240, 360])
leiste("vergleich_nb3_3_schnitt17_bild450_472_520_600.jpg", [450, 472, 520, 600])

with open(os.path.join(ziel, "messung_farbe_masse.txt"), "w") as f:
    f.write("Farbe der Masse: Mittel R/G/B ueber x 40..140, y 160..230 (320x240), Abnahme G5 2 Abschnitt 2\n")
    for n in (180, 240, 313):
        zeile = "Bild %d:" % n
        for nm, pat in quellen:
            p = pat % (n + 100)
            if not os.path.exists(p):
                zeile += " %s -" % nm
                continue
            a = np.asarray(Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR), dtype=float)[160:231, 40:141]
            zeile += " | %s %d/%d/%d" % (nm, round(a[..., 0].mean()), round(a[..., 1].mean()), round(a[..., 2].mean()))
        f.write(zeile + "\n")
        print(zeile)
