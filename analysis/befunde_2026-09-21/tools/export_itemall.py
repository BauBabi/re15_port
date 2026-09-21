#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Export one ITEMALL.PIX tile: raw 1200 index bytes + an index-grayscale PNG.

Tile layout from the port's own byte-true decoder
re15_port/engine/src/item_icon_common.c: 72 tiles, 40x30, 1 byte per pixel,
tile index = item id (weapons proven 1:1), pixel = CLUT[tile[v*40+u]].
The per-item CLUT is NOT in the file (it is framebuffer-derived and the port
has only captured clut_idx 2 and 6, ids 0x00..0x18) — so the PNG here shows the
palette INDICES, not the final colours.
"""
import os, sys
from PIL import Image

SRC, ID, OUT, BASE = sys.argv[1], int(sys.argv[2], 16), sys.argv[3], sys.argv[4]
W, H = 40, 30
d = open(SRC, 'rb').read()
off = ID * W * H
tile = d[off:off + W * H]
os.makedirs(OUT, exist_ok=True)
open(os.path.join(OUT, BASE + '.raw'), 'wb').write(tile)
img = Image.new('L', (W, H))
img.putdata(list(tile))
img.save(os.path.join(OUT, BASE + '_indices.png'))
img.resize((W * 6, H * 6), Image.NEAREST).save(os.path.join(OUT, BASE + '_indices_6x.png'))
print('%s: id 0x%02X file@0x%06X %dx%d, %d distinct indices'
      % (BASE, ID, off, W, H, len(set(tile))))
