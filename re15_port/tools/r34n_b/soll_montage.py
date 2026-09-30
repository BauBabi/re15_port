#!/usr/bin/env python3
"""Spur B: SOLL-MONTAGE (kein Port-Lauf!) — der 11F0-Cursor, wie ihn der Port in ROOM11F0 Cut 10 zeichnet
(Framedump m2 F480, nur die exakten CLUT-Farben der Cursor-Textur ROOM11F0 @0x018DAC), versetzt auf das
Cut-4-Bild von ROOM1150 im Cursor-Zustand (m1 F240). Zeigt Start (Bildmitte) und eine Lage ueber der Kuppel,
dazu die Trefferflaeche (Engine-Huelle) als duenne Linie.
Aufruf: soll_montage.py <m2_F480.ppm> <m1_F240.ppm> <aus.png>"""
import sys
from PIL import Image, ImageDraw
CLUT = {(0, 88, 0), (64, 56, 0), (8, 248, 0), (216, 208, 0), (240, 248, 136)}
q = Image.open(sys.argv[1]).convert("RGB"); z = Image.open(sys.argv[2]).convert("RGB")
qp = q.load()
# Cursor-Kasten in 11F0 (960er): um die gemessene bbox 148.3..172.0 x 107.7..130.0 (320er) herum
x0, x1, y0, y1 = 435, 525, 315, 400
pix = [(x - x0, y - y0, qp[x, y]) for y in range(y0, y1) for x in range(x0, x1) if qp[x, y] in CLUT]
# Heisspunkt in 11F0 (Engine-Sonde): (160,119) -> 960er (480,357)
hx, hy = 480 - x0, 357 - y0
def setze(img, cx, cy):
    p = img.load()
    for dx, dy, c in pix:
        X, Y = cx - hx + dx, cy - hy + dy
        if 0 <= X < img.size[0] and 0 <= Y < img.size[1]: p[X, Y] = c
H = [(151,171),(163,161),(180,151),(206,148),(235,150),(248,156),(267,170),(265,190),(261,204),(214,210),(164,204),(151,191)]
a = z.copy(); setze(a, 480, 357)
b = z.copy(); setze(b, 209 * 3, 176 * 3)
for img in (a, b):
    d = ImageDraw.Draw(img); d.polygon([(x * 3, y * 3) for x, y in H], outline=(255, 64, 64))
out = Image.new("RGB", (960 * 2 // 2, 720 // 2 * 1), (0, 0, 0))
aa = a.resize((480, 360), Image.NEAREST); bb = b.resize((480, 360), Image.NEAREST)
out = Image.new("RGB", (960, 360)); out.paste(aa, (0, 0)); out.paste(bb, (480, 0))
d = ImageDraw.Draw(out)
d.text((4, 4), "MONTAGE Start (160,119): Druck -> 'Nothing happened.'", fill=(255, 255, 0))
d.text((484, 4), "MONTAGE ueber der Kuppel (209,176): Druck -> Klick + Kuppel auf", fill=(255, 255, 0))
out.save(sys.argv[3]); print(sys.argv[3], len(pix), "Cursor-Pixel")
