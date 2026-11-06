# Abnahme 2: Vergleichsleiste RE2 echt | Port Leon | Port Elza je Szenenbild (320x240 je Feld), Beschriftung
import sys, os
from PIL import Image, ImageDraw
out = sys.argv[1]; ns = [int(x) for x in sys.argv[2].split(",")]
quellen = [("RE2 echt", "re2/bilder/re2_%04d.png"), ("Port Leon 5090", "lauf/leon_steh/bilder/port_%04d.png"),
           ("Port Elza 5091", "lauf/elza_steh/bilder/port_%04d.png")]
W, H = 320, 240
img = Image.new("RGB", (W * len(quellen), (H + 16) * len(ns)), (0, 0, 0))
dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    for c, (nm, pat) in enumerate(quellen):
        p = pat % (n + 100)
        if os.path.exists(p):
            im = Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR)
            img.paste(im, (c * W, r * (H + 16) + 16))
        dr.text((c * W + 4, r * (H + 16) + 2), "%s  Bild %d" % (nm, n), fill=(255, 255, 0))
img.save(out, quality=88)
print(out)
