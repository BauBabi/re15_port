# Abnahme 3: Leiste RE2 echt | Port Leon (Framedump) | Port Elza (Framedump), 320x240 je Feld; Port-Bild n = Spielbild F1613+n
import sys, os
from PIL import Image, ImageDraw
out = sys.argv[1]; ns = [int(x) for x in sys.argv[2].split(",")]
Q = [("RE2 echt", lambda n: "re2bilder/re2_%+05d.png" % n), ("Port Leon 5090", lambda n: "lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)),
     ("Port Elza 5091", lambda n: "lauf/fd_elza/fd/f%06d.ppm" % (1613 + n))]
W, H = 320, 240
img = Image.new("RGB", (W * 3, (H + 14) * len(ns)), (0, 0, 0)); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    for c, (nm, f) in enumerate(Q):
        p = f(n)
        if os.path.exists(p): img.paste(Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR), (c * W, r * (H + 14) + 14))
        dr.text((c * W + 4, r * (H + 14) + 1), "%s  Bild %d" % (nm, n), fill=(255, 255, 0))
img.save(out, quality=86); print(out)
