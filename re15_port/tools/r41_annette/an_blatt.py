#!/usr/bin/env python3
"""Runde 41 Spur AN: Bildleiste aus einem an_lauf.sh-Lauf (echtes Fenster) und Vergleich mit 80 % #145.

    python an_blatt.py <laufdir> <ziel.jpg> <bild:beschriftung> [...]      Bildleiste, 3 Spalten
    python an_blatt.py --vergleich <port.png> <foto145.png> <ziel.jpg>     Port neben #145
"""
import sys
from PIL import Image, ImageDraw


def beschriftet(im, text):
    im = im.convert('RGB').resize((640, 480))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 640, 22), fill=(0, 0, 0))
    d.text((6, 5), text, fill=(255, 220, 80))
    return im


def leiste(laufdir, ziel, punkte):
    bilder = []
    for p in punkte:
        n, txt = p.split(':', 1)
        bilder.append(beschriftet(Image.open('%s/bild_%03d.png' % (laufdir, int(n))), 'Bild %s  %s' % (n, txt)))
    sp = 3
    zeilen = (len(bilder) + sp - 1) // sp
    blatt = Image.new('RGB', (640 * sp, 480 * zeilen), (20, 20, 20))
    for i, b in enumerate(bilder):
        blatt.paste(b, ((i % sp) * 640, (i // sp) * 480))
    blatt.save(ziel, quality=88)


def vergleich(port, foto, ziel):
    a = beschriftet(Image.open(foto), '80 % Video A Foto #145 (ROOM5090 Cut 11)')
    b = beschriftet(Image.open(port), 'Port Cut 11, Pose Bild 2 waehrend des Satzes')
    blatt = Image.new('RGB', (1280, 480))
    blatt.paste(a, (0, 0)); blatt.paste(b, (640, 0))
    blatt.save(ziel, quality=88)


if __name__ == '__main__':
    if sys.argv[1] == '--vergleich':
        vergleich(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        leiste(sys.argv[1], sys.argv[2], sys.argv[3:])
