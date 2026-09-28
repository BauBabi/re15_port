"""Kontaktbogen aus RE15_FRAMEDUMP-Bildern (f_NNNNNN.ppm, 960x720).

    python bogen.py <ordner> <ausgabe.png> <bild> [<bild> ...]

Jedes Bild wird auf 320x240 verkleinert und mit seiner Bildnummer beschriftet.
"""
import os
import sys

from PIL import Image, ImageDraw


def main():
    ordner, aus = sys.argv[1], sys.argv[2]
    bilder = [int(b) for b in sys.argv[3:]]
    spalten = 4
    zeilen = (len(bilder) + spalten - 1) // spalten
    bogen = Image.new("RGB", (spalten * 320, zeilen * 240), (40, 40, 40))
    zeichner = ImageDraw.Draw(bogen)
    for i, b in enumerate(bilder):
        pfad = os.path.join(ordner, "f_%06d.ppm" % b)
        x, y = (i % spalten) * 320, (i // spalten) * 240
        if os.path.exists(pfad):
            bogen.paste(Image.open(pfad).convert("RGB").resize((320, 240)), (x, y))
        else:
            zeichner.text((x + 10, y + 110), "fehlt", fill=(255, 0, 0))
        zeichner.rectangle([x, y, x + 60, y + 14], fill=(0, 0, 0))
        zeichner.text((x + 3, y + 2), "F%d" % b, fill=(255, 255, 0))
    bogen.save(aus)
    print(aus, len(bilder), "Bilder")


if __name__ == "__main__":
    main()
