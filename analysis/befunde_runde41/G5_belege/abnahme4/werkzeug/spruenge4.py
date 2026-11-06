# Abnahme 4: Bild-zu-Bild-Differenz aller Framedump-Bilder (160x120 Graustufen, mittlere |Differenz|), Ausreisser > 7 auflisten
import sys, glob, numpy as np
from PIL import Image
fs = sorted(glob.glob(sys.argv[1] + "/f*.ppm"))
alt = None; d = []
for f in fs:
    a = np.asarray(Image.open(f).convert("L").resize((160, 120), Image.BILINEAR), dtype=np.float32)
    if alt is not None: d.append((int(f[-10:-4]), float(np.abs(a - alt).mean())))
    alt = a
v = np.array([x for _, x in d])
print("Bilder %d, Median %.2f, 99%%-Quantil %.2f" % (len(d), np.median(v), np.quantile(v, 0.99)))
for F, x in d:
    if x > 7: print("  F%d (Szenenbild %d): %.1f" % (F, F - 1613, x))
