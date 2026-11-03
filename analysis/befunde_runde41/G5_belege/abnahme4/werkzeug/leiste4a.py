# Abnahme 4: Leiste RE2 echt | Port vorher (Framedump Abnahme 3, Kopf 15b2fd419) | Port Leon NB4 | Port Elza NB4; Port-Bild n = F1613 + n
import sys, os
from PIL import Image, ImageDraw
out = sys.argv[1]; ns = [int(x) for x in sys.argv[2].split(",")]
box = tuple(int(v) for v in sys.argv[3].split(",")) if len(sys.argv) > 3 else None   # Ausschnitt x0,y0,x1,y1 im 640x480-Raster
A3 = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn3/lauf"
Q = [("RE2 echt", lambda n: "re2bilder/re2_%+05d.png" % n), ("Port vorher (Abn. 3)", lambda n: A3 + "/fd_leon/fd/f%06d.ppm" % (1613 + n)),
     ("Port Leon NB4", lambda n: "lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)), ("Port Elza NB4", lambda n: "lauf/fd_elza/fd/f%06d.ppm" % (1613 + n))]
W, H = 320, 240
img = Image.new("RGB", (W * len(Q), (H + 14) * len(ns)), (0, 0, 0)); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    for c, (nm, f) in enumerate(Q):
        p = f(n)
        if os.path.exists(p):
            im = Image.open(p).convert("RGB").resize((640, 480), Image.BILINEAR)
            if box: im = im.crop(box)
            img.paste(im.resize((W, H), Image.BILINEAR), (c * W, r * (H + 14) + 14))
        dr.text((c * W + 4, r * (H + 14) + 1), "%s  Bild %d" % (nm, n), fill=(255, 255, 0))
img.save(out, quality=86); print(out)
