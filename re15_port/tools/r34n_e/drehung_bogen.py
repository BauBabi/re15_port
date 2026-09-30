# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Drehungs-Bogen der vier Dokumente an ihren ENDWERTEN (E_dokumente.md 5.2).

Je Dokument und Haupt-Cut die Drehungen 0/1024/2048/3072 (bzw. fuer Dok 4 die gewaehlte 3840 dazu),
OHNE Licht (Faktor 1), damit Schild, Schliesse und Blattkante erkennbar sind - Grundlage des offenen
Punkts 8.1 (Drehung = PORT-WAHL, optische Abnahme beim Nutzer). Zeichnung wie kontrollabzug.py
(Engine-Matrix, Prop-Drehung wie platform/pc/main.c pc_prop_rot_q12).

Aufruf: python re15_port/tools/r34n_e/drehung_bogen.py [ausgabe.png]
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402
import kontrollabzug as K  # noqa: E402

FAELLE = [("Dok1", "1050", "mesh00_0541704e", 9, (16474, -360, -6592), (0, 1024, 2048, 3072)),
          ("Dok1", "1050", "mesh00_0541704e", 3, (16474, -360, -6592), (0, 1024, 2048, 3072)),
          ("Dok2", "1000", "mesh03_cf9f316d", 0, (19226, -398, -11723), (0, 1024, 2048, 3072)),
          ("Dok3", "1020", "mesh01_ae2d0a30", 6, (-9975, -1410, -16428), (0, 1024, 2048, 3072)),
          ("Dok4", "1010", "mesh04_cab7b32d", 0, (450, -1600, 5600), (3840, 0, 1024, 2048))]


def main():
    aus = sys.argv[1] if len(sys.argv) > 1 else os.path.join(geom.AUS, "drehung_bogen.png")
    SS = K.SS
    reihen = []
    for name, raum, modell, cut, pos, drehungen in FAELLE:
        cams = geom.lade_kameras(raum)
        polys = K.md1_laden(open(os.path.join(K.WELT, modell + ".md1"), "rb").read())
        tim = K.tim_laden(open(os.path.join(K.WELT, modell + ".tim"), "rb").read())
        bg = Image.fromarray(geom.bg(raum, cut).astype(np.uint8))
        reihe = []
        for ry in drehungen:
            gross = np.array(bg.resize((320 * SS, 240 * SS), Image.NEAREST)).astype(float)
            zbuf = np.full(gross.shape[:2], 1e18)
            bb, n = K.modell_zeichnen(gross, zbuf, cams[cut], pos, ry, polys, tim, np.array((1.0, 1.0, 1.0)))
            img = Image.fromarray(gross.astype(np.uint8))
            cx, cy = (bb[0] + bb[1]) / 2, (bb[2] + bb[3]) / 2
            t = img.crop((int((cx - 25) * SS), int((cy - 19) * SS), int((cx + 25) * SS), int((cy + 19) * SS)))
            t = t.resize((200, 152), Image.BOX)
            ImageDraw.Draw(t).text((3, 2), "%s ROOM%s C%d ry %d" % (name, raum, cut, ry), fill=(255, 255, 0))
            reihe.append(t)
        reihen.append(reihe)
    bogen = Image.new("RGB", (4 * 200, len(reihen) * 152), (0, 0, 0))
    for r, reihe in enumerate(reihen):
        for c, t in enumerate(reihe):
            bogen.paste(t, (c * 200, r * 152))
    bogen.save(aus)
    print(aus)


if __name__ == "__main__":
    main()
