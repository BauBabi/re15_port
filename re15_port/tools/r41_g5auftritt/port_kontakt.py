#!/usr/bin/env python3
"""port_kontakt.py - Runde 41 G5-Auftritt: Kontaktblatt der Port-Szene aus einer gdigrab-Aufnahme des echten Fensters.

Aufruf: port_kontakt.py <videobilder-ordner v_####.png> <videobild der Armierung> <videobild Schnitt 17> <titel> <ausgabe.png>
Die Spielbilder je Videobild ergeben sich aus den zwei gemessenen Schnittkanten (Bild 0 und Bild 437): Videobild = v0 + Bild * (v437 - v0) / 437.
Dieselben Schluesselbilder wie re2_kontakt.py.
"""
import os
import sys

from PIL import Image, ImageDraw

BILDER = [
    (0, "Armierung, Schnitt 16 = RE2 Schnitt 9"), (12, "[T1] Arm 0 tritt aus"), (45, "Arme 0/2/3 draussen"),
    (91, "[T2] Hereinziehen beginnt"), (180, "[T2] Koerper kommt herein"), (271, "[T3] aufrichten"), (400, "[T5] Speere"),
    (437, "Schnitt 17 = RE2 Schnitt 10"), (560, "[T6] kriecht +3946"), (700, "[T7/T8] Clip 2"),
    (807, "Cut 12 (RE2 5)"), (950, "[T10] Wedeln"), (1126, "[T14] -> Kampf"), (1162, "Ende, Spieler frei"),
]


def main():
    ordner, v0, v437, titel, aus = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4], sys.argv[5]
    schritt = (v437 - v0) / 437.0
    kacheln = []
    for b, text in BILDER:
        n = int(round(v0 + b * schritt)) + (1 if b in (0, 437) else 0)   # eine Kachel hinter die Schnittkante
        p = os.path.join(ordner, "v_%04d.png" % n)
        if not os.path.exists(p):
            continue
        im = Image.open(p).convert("RGB").resize((640, 480))
        d = ImageDraw.Draw(im)
        d.rectangle([0, 0, 640, 22], fill=(0, 0, 0))
        d.text((4, 5), "Port Bild %d (Video %d): %s" % (b, n, text), fill=(255, 255, 0))
        kacheln.append(im)
    spalten = 4
    zeilen = (len(kacheln) + spalten - 1) // spalten
    blatt = Image.new("RGB", (640 * spalten, 480 * zeilen + 40), (0, 0, 0))
    for k, im in enumerate(kacheln):
        blatt.paste(im, ((k % spalten) * 640, (k // spalten) * 480))
    ImageDraw.Draw(blatt).text((6, 480 * zeilen + 8), titel, fill=(255, 255, 255))
    blatt.save(aus)
    print("geschrieben:", aus, blatt.size)


if __name__ == "__main__":
    main()
