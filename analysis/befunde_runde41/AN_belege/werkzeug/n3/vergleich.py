"""Vergleichsblatt RE2 | Port: vergleich.py <re2-dir> <port-laufdir> <aus.jpg> "re2zeit|portbild|Text" ...
RE2-Bild = <re2-dir>/re2_<zeit>.png (aus re2c_szene_320.mp4, Clipzeit = Videozeit - 1,00 s), Port = <laufdir>/f<NNNNNN>.ppm."""
import sys, os
from PIL import Image, ImageDraw, ImageFont
r2, lauf, aus = sys.argv[1:4]
zeilen = [a.split('|') for a in sys.argv[4:]]
W, H, K = 480, 360, 24
try:
    font = ImageFont.truetype('arial.ttf', 15)
except Exception:
    font = ImageFont.load_default()
b = Image.new('RGB', (2 * W + 8, len(zeilen) * (H + K)), (0, 0, 0))
d = ImageDraw.Draw(b)
for i, (t, f, txt) in enumerate(zeilen):
    y = i * (H + K)
    for x, p, kopf in ((0, os.path.join(r2, 're2_%s.png' % t), 'RE2 %s s' % t),
                       (W + 8, os.path.join(lauf, 'f%06d.ppm' % int(f)), 'Port F%s' % f)):
        if os.path.exists(p):
            b.paste(Image.open(p).convert('RGB').resize((W, H), Image.BILINEAR), (x, y + K))
        d.text((x + 4, y + 4), '%s  %s' % (kopf, txt), fill=(255, 220, 120), font=font)
b.save(aus, quality=88)
print(aus, b.size)
