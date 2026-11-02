#!/usr/bin/env python3
"""Varianten J (E18): fertige Blaetter nebeneinander (links nach rechts) mit Kopfzeile je Bild.
python vm_nebeneinander.py AUS.png "Titel=bild.png" ... [--unter]  (--unter: untereinander). Kein RE-Wert."""
import sys

from PIL import Image, ImageDraw


def main():
    unter = '--unter' in sys.argv
    args = [a for a in sys.argv[2:] if a != '--unter']
    bilder = [(a.split('=', 1)[0], Image.open(a.split('=', 1)[1]).convert('RGB')) for a in args]
    kopf, pad = 26, 10
    if unter:
        w = max(b.width for _, b in bilder) + 2 * pad
        h = sum(b.height + kopf + pad for _, b in bilder) + pad
    else:
        w = sum(b.width + pad for _, b in bilder) + pad
        h = max(b.height for _, b in bilder) + kopf + 2 * pad
    out = Image.new('RGB', (w, h), (24, 24, 24))
    d = ImageDraw.Draw(out)
    x = y = pad
    for t, b in bilder:
        d.text((x + 4, y + 6), t, fill=(235, 235, 235))
        out.paste(b, (x, y + kopf))
        if unter:
            y += b.height + kopf + pad
        else:
            x += b.width + pad
    out.save(sys.argv[1])


if __name__ == '__main__':
    main()
