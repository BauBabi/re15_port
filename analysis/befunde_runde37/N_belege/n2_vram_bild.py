#!/usr/bin/env python3
"""Runde 37 Spur N — Bildschirm des ORIGINALS aus einem DuckStation-Endstand (VRAM, wahre Basis "GPU-VRAM").

Liest den ganzen VRAM (1024x512, 16 bpp 1555) und schneidet die beiden Anzeigepuffer 320x240 bei (0,0) und
(0,240) aus (RE1.5 doppelt gepuffert); gibt beide nebeneinander + den ganzen VRAM aus.
Aufruf: n2_vram_bild.py <sav> <ausgabe-praefix>
"""
import sys, os
from PIL import Image
BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.insert(0, os.path.join(BAUM, '.claude/skills/re15-savestate-ghidra/scripts'))
import re15_ss

r = re15_ss.Ram(sys.argv[1])
o = r.vram_base if r.vram_base is not None else r.base + 0x200000
b = r.blob
img = Image.new('RGB', (1024, 512))
px = img.load()
for y in range(512):
    row = o + y * 2048
    for x in range(1024):
        v = b[row + 2 * x] | (b[row + 2 * x + 1] << 8)
        px[x, y] = ((v & 0x1f) << 3, ((v >> 5) & 0x1f) << 3, ((v >> 10) & 0x1f) << 3)
img.save(sys.argv[2] + '_vram.png')
a = img.crop((0, 0, 320, 240))
c = img.crop((0, 240, 320, 480))
both = Image.new('RGB', (650, 240))
both.paste(a, (0, 0))
both.paste(c, (330, 0))
both.save(sys.argv[2] + '_puffer.png')
print('ok', sys.argv[2])
