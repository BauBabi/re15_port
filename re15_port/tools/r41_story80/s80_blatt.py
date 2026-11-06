#!/usr/bin/env python3
"""Runde 41 Spur ST80-T: Bildleiste aus einem s80_lauf.sh-Lauf (echtes Fenster, gdigrab). Muster tools/r41_annette/an_blatt.py.

    python s80_blatt.py <laufdir> <ziel.jpg> <bild:beschriftung> [...]      3 Spalten, je 640x480 (Fenster 1:1)

<bild> = Nummer der Datei bild_<bild>.png (ffmpeg %03d, ab 1000 vierstellig).
"""
import os
import sys
from PIL import Image, ImageDraw


def beschriftet(im, text):
    im = im.convert('RGB').resize((640, 480))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 640, 18), fill=(0, 0, 0))
    d.text((6, 3), text, fill=(255, 220, 80))
    return im


def datei(laufdir, n):
    for name in ('bild_%03d.png' % n, 'bild_%d.png' % n):
        p = os.path.join(laufdir, name)
        if os.path.exists(p):
            return p
    raise SystemExit('fehlt: Bild %d in %s' % (n, laufdir))


def leiste(laufdir, ziel, punkte):
    bilder = []
    for p in punkte:
        n, txt = p.split(':', 1)
        bilder.append(beschriftet(Image.open(datei(laufdir, int(n))), 'Bild %s  %s' % (n, txt)))
    sp = 3
    zeilen = (len(bilder) + sp - 1) // sp
    blatt = Image.new('RGB', (640 * sp, 480 * zeilen), (20, 20, 20))
    for i, b in enumerate(bilder):
        blatt.paste(b, ((i % sp) * 640, (i // sp) * 480))
    blatt.save(ziel, quality=90)
    print('geschrieben', ziel, blatt.size)


if __name__ == '__main__':
    leiste(sys.argv[1], sys.argv[2], sys.argv[3:])
