"""Bildleiste aus RE15_FRAMEDUMP-Bildern: blatt.py <laufdir> <aus.jpg> <spalten> F:Text [F:Text ...] (F = Spielbild, ppm f<NNNNNN>)."""
import sys, os
from PIL import Image, ImageDraw, ImageFont
lauf, aus, sp = sys.argv[1], sys.argv[2], int(sys.argv[3])
felder = [a.split(':', 1) for a in sys.argv[4:]]
W, H, K = 640, 480, 22
try:
    font = ImageFont.truetype('arial.ttf', 15)
except Exception:
    font = ImageFont.load_default()
zeilen = (len(felder) + sp - 1) // sp
blatt = Image.new('RGB', (sp * W, zeilen * (H + K)), (0, 0, 0))
d = ImageDraw.Draw(blatt)
for i, (f, t) in enumerate(felder):
    p = os.path.join(lauf, 'f%06d.ppm' % int(f))
    x, y = (i % sp) * W, (i // sp) * (H + K)
    if os.path.exists(p):
        blatt.paste(Image.open(p).convert('RGB').resize((W, H), Image.NEAREST), (x, y + K))
    d.text((x + 4, y + 3), '%d F%s %s' % (i + 1, f, t), fill=(255, 220, 120), font=font)
blatt.save(aus, quality=88)
print(aus, blatt.size)
