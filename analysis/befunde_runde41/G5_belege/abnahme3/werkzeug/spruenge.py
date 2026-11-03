# Abnahme 3: Bild-zu-Bild-Differenz ueber alle Framedump-Bilder (pixelgleich je Spielbild), Ausreisser ausserhalb der Schnittkanten
import numpy as np, sys, os
from PIL import Image
d = sys.argv[1]; F0, F1 = int(sys.argv[2]), int(sys.argv[3])
prev = None; out = []
for F in range(F0, F1 + 1):
    p = os.path.join(d, "f%06d.ppm" % F)
    if not os.path.exists(p): continue
    a = np.asarray(Image.open(p).convert("L").resize((160, 120), Image.BILINEAR), dtype=np.int16)
    if prev is not None: out.append((F, float(np.abs(a - prev).mean())))
    prev = a
v = np.array([x[1] for x in out]); med = np.median(v)
print("Bilder %d, Median %.2f, 99%%-Quantil %.2f" % (len(v), med, np.quantile(v, 0.99)))
for F, x in sorted(out, key=lambda t: -t[1])[:14]: print("  F%d (Bild %d): %.2f" % (F, F - 1613, x))
np.save(os.path.join(d, "..", "diffs.npy"), np.array(out))
