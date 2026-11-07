#!/usr/bin/env python3
"""Nahbilder zu Tafeln: je Praefix eine Reihe."""
import os, glob, sys
from PIL import Image, ImageDraw
H = os.path.dirname(os.path.abspath(__file__)); N = os.path.join(H, 'bild', 'nah')
for pre in ('wurzel', 'handr', 'fuss', 'kopf'):
    fs = sorted(glob.glob(os.path.join(N, pre + '_*.png')))
    if not fs: continue
    W, Hh = 400, 300; sp = 5
    rows = (len(fs) + sp - 1) // sp
    bl = Image.new('RGB', (W * sp, Hh * rows), (20, 20, 20)); d = ImageDraw.Draw(bl)
    for i, f in enumerate(fs):
        im = Image.open(f).convert('RGB').resize((W, Hh)); x, y = (i % sp) * W, (i // sp) * Hh
        bl.paste(im, (x, y)); d.text((x + 5, y + 4), os.path.basename(f)[:-4], fill=(255, 255, 0))
    bl.save(os.path.join(H, 'bild', 'nah_%s.png' % pre))
print('ok')
