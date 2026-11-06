# Nachbesserung 4 G5A: Leiste RE2 echt | Port vorher (Kopf 8c45dbd14, Framedump Abnahme 3) | Leon nachher | Elza nachher.
# Port-Bild n = Spielbild F1613 + n (RE15_FRAMEDUMP). Optional Ausschnitt "x0,y0,x1,y1" in Anteilen des Bilds (Zoom).
import sys, os
from PIL import Image, ImageDraw
out, ns = sys.argv[1], [int(x) for x in sys.argv[2].split(",")]
aus = [float(v) for v in sys.argv[3].split(",")] if len(sys.argv) > 3 else None
A3 = sys.argv[4] if len(sys.argv) > 4 else "../g5abn3/lauf"
Q = [("RE2 echt", lambda n: "re2bilder/re2_%+05d.png" % n), ("Port vorher 8c45dbd14 Leon", lambda n: "%s/fd_leon/fd/f%06d.ppm" % (A3, 1613 + n)),
     ("Port nachher Leon 5090", lambda n: "lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)), ("Port nachher Elza 5091", lambda n: "lauf/fd_elza/fd/f%06d.ppm" % (1613 + n))]
W, H = 320, 240
img = Image.new("RGB", (W * 4, (H + 14) * len(ns)), (0, 0, 0)); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    for c, (nm, f) in enumerate(Q):
        p = f(n)
        if os.path.exists(p):
            b = Image.open(p).convert("RGB")
            if aus: b = b.crop((int(aus[0] * b.width), int(aus[1] * b.height), int(aus[2] * b.width), int(aus[3] * b.height)))
            img.paste(b.resize((W, H), Image.BILINEAR), (c * W, r * (H + 14) + 14))
        else: dr.text((c * W + 4, r * (H + 14) + 40), "fehlt", fill=(255, 0, 0))
        dr.text((c * W + 4, r * (H + 14) + 1), "%s  Bild %d" % (nm, n), fill=(255, 255, 0))
img.save(out, quality=86); print(out)
