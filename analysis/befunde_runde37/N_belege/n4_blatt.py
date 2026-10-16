#!/usr/bin/env python3
"""Runde 37 Spur N — Bildblatt aus RE15_FRAMEDUMP-Serien (PPM) eines oder mehrerer Laeufe.

Aufruf: n4_blatt.py <ausgabe.png> <spalten> <titel1>=<ordner1>[:<bild-liste>] [<titel2>=<ordner2>...]
  bild-liste = Komma-Liste der Bildnummern (sonst alle f_*.ppm). Je Lauf eine Zeile, Beschriftung je Bild.
"""
import sys, os, re
from PIL import Image, ImageDraw

aus = sys.argv[1]
spalten = int(sys.argv[2])
zeilen = []
for arg in sys.argv[3:]:
    titel, rest = arg.split('=', 1)
    if ':' in rest[2:]:
        i = rest.rfind(':')
        ordner, liste = rest[:i], [int(x) for x in rest[i + 1:].split(',') if x]
    else:
        ordner, liste = rest, None
    dateien = sorted(f for f in os.listdir(ordner) if re.match(r'f_\d+\.ppm$', f))
    if liste is not None:
        dateien = [f for f in dateien if int(f[2:-4]) in liste]
    zeilen.append((titel, ordner, dateien[:spalten]))
W, H = 320, 240
blatt = Image.new('RGB', (spalten * W, len(zeilen) * (H + 14)), (20, 20, 20))
d = ImageDraw.Draw(blatt)
for zi, (titel, ordner, dateien) in enumerate(zeilen):
    for si, f in enumerate(dateien):
        im = Image.open(os.path.join(ordner, f)).convert('RGB')
        if im.size != (W, H):
            im = im.resize((W, H))
        blatt.paste(im, (si * W, zi * (H + 14) + 14))
        d.text((si * W + 3, zi * (H + 14) + 1), '%s B%d' % (titel, int(f[2:-4])), fill=(255, 255, 0))
blatt.save(aus)
print('ok', aus, blatt.size)
