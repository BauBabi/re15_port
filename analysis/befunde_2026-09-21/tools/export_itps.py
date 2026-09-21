#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Export one ITPS.ITP tile (item id) as raw .tim + .png (1x and 6x)."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import timlib
from PIL import Image

SRC, ID, OUT, BASE = sys.argv[1], int(sys.argv[2], 16), sys.argv[3], sys.argv[4]
STRIDE = 0x3000
d = open(SRC, 'rb').read()
off = ID * STRIDE
t = timlib.decode(d, off)
os.makedirs(OUT, exist_ok=True)
open(os.path.join(OUT, BASE + '.tim'), 'wb').write(d[off:t['end']])
img = timlib.to_image(t, 0)
img.save(os.path.join(OUT, BASE + '.png'))
img.resize((img.width * 6, img.height * 6), Image.NEAREST).save(
    os.path.join(OUT, BASE + '_6x.png'))
print('%s: id 0x%02X  file@0x%06X  %dx%d %dbpp  tim %d B'
      % (BASE, ID, off, t['w'], t['h'], t['bpp'], t['end'] - off))
