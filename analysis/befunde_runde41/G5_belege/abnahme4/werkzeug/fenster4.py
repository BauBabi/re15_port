# Abnahme 4: echtes Fenster (PrintWindow-Bilder der eigenen exe, printwindow4.py) gegen Framedump: fuer Szenenbild n das Fensterbild mit der
# kleinsten mittleren Differenz (160x120 Graustufen) suchen; Leiste Framedump | bestes Fensterbild.
import sys, glob, os, numpy as np
from PIL import Image, ImageDraw
ordner, fd, out = sys.argv[1], sys.argv[2], sys.argv[3]; ns = [int(x) for x in sys.argv[4].split(",")]
fs = sorted(glob.glob(os.path.join(ordner, "w_*.png")))
klein = np.stack([np.asarray(Image.open(f).convert("L").resize((160, 120), Image.BILINEAR), dtype=np.float32) for f in fs])
img = Image.new("RGB", (960, 374 * len(ns))); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    p = Image.open(fd % (1613 + n)).convert("RGB")
    pg = np.asarray(p.convert("L").resize((160, 120), Image.BILINEAR), dtype=np.float32)
    d = np.abs(klein - pg[None]).mean(axis=(1, 2)); k = int(np.argmin(d))
    print("Szenenbild %4d: bestes Fensterbild %s, mittlere Differenz %.2f (Median aller %.1f)" % (n, os.path.basename(fs[k]), d[k], np.median(d)))
    img.paste(p.resize((480, 360)), (0, r * 374 + 14)); img.paste(Image.open(fs[k]).convert("RGB").resize((480, 360)), (480, r * 374 + 14))
    dr.text((3, r * 374 + 1), "Framedump Bild %d" % n, fill=(255, 255, 0)); dr.text((483, r * 374 + 1), "Fenster (PrintWindow) %s, Diff %.1f" % (os.path.basename(fs[k]), d[k]), fill=(255, 255, 0))
img.save(out, quality=86)
