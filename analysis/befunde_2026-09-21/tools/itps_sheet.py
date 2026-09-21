#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Render all 72 RE1.5 ITPS.ITP item pictures (item id 0x00..0x47).

Stride and tile layout from the port's own byte-true decoder
re15_port/engine/src/itps_common.c (ITPS_STRIDE 0x3000, ITPS_TILES 72,
'the modal's load callback LAB_8001e404 streams the TIM at CD LBA 0x1740 +
id x 6 sectors = byte offset id x 0x3000').
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import timlib
from PIL import Image, ImageDraw, ImageFont

SRC = sys.argv[1]
OUT = sys.argv[2]
NAMEFILE = sys.argv[3] if len(sys.argv) > 3 else None
d = open(SRC, 'rb').read()
STRIDE = 0x3000
N = len(d) // STRIDE
names = {}
if NAMEFILE:
    for line in open(NAMEFILE, encoding='utf-8'):
        if '=' in line:
            k, v = line.split('=', 1)
            names[int(k, 16)] = v.strip()
tiles = []
for i in range(N):
    t = timlib.decode(d, i * STRIDE)
    if t is None:
        tiles.append((i, None))
        continue
    tiles.append((i, timlib.to_image(t, 0)))
TW = max((im.width for _, im in tiles if im), default=64)
TH = max((im.height for _, im in tiles if im), default=64)
SC = 1
LBL = 14
COLS = 8
ROWS = (N + COLS - 1) // COLS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 11)
except Exception:
    F = ImageFont.load_default()
sheet = Image.new('RGB', (COLS * TW * SC, ROWS * (TH * SC + LBL)), (10, 10, 14))
dr = ImageDraw.Draw(sheet)
for k, (i, im) in enumerate(tiles):
    x = (k % COLS) * TW * SC
    y = (k // COLS) * (TH * SC + LBL)
    if im:
        sheet.paste(im.resize((im.width * SC, im.height * SC), Image.NEAREST), (x, y + LBL))
    lab = '0x%02X %s' % (i, names.get(i, ''))
    dr.text((x + 2, y + 1), lab[:26], fill=(235, 235, 130), font=F)
sheet.save(OUT)
print('wrote %s  (%d tiles, %dx%d each)' % (OUT, N, TW, TH))
