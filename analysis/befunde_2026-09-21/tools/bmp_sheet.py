#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Contact sheet from a directory of images (item icon atlas pages)."""
import os, sys
from PIL import Image, ImageDraw, ImageFont
SRC = sys.argv[1]
OUT = sys.argv[2]
SC = int(sys.argv[3]) if len(sys.argv) > 3 else 2
names = sorted(f for f in os.listdir(SRC) if f.lower().endswith(('.bmp', '.png')))
imgs = []
for n in names:
    try:
        imgs.append((n, Image.open(os.path.join(SRC, n)).convert('RGB')))
    except Exception as e:
        print('skip', n, e)
if not imgs:
    print('nothing')
    raise SystemExit
TW = max(i.width for _, i in imgs) * SC
TH = max(i.height for _, i in imgs) * SC
LBL = 14
COLS = max(1, min(10, int(2000 / max(1, TW))))
ROWS = (len(imgs) + COLS - 1) // COLS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 11)
except Exception:
    F = ImageFont.load_default()
sheet = Image.new('RGB', (COLS * TW, ROWS * (TH + LBL)), (10, 10, 14))
dr = ImageDraw.Draw(sheet)
for k, (n, im) in enumerate(imgs):
    x = (k % COLS) * TW
    y = (k // COLS) * (TH + LBL)
    im2 = im.resize((im.width * SC, im.height * SC), Image.NEAREST)
    sheet.paste(im2, (x, y + LBL))
    dr.text((x + 2, y + 1), n[:28], fill=(230, 230, 130), font=F)
sheet.save(OUT)
print('wrote %s (%d images, %dx%d tiles)' % (OUT, len(imgs), TW, TH))
