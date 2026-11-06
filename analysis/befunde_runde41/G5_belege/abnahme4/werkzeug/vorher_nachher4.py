# Abnahme 4: wo aendert NB4 das Bild? Framedump Abnahme 3 (Kopf 15b2fd419) gegen NB4 (6cce9fcf2), Leon, je Spielbild mittlere |Differenz|
# (160x120 Graustufen) und Anteil Pixel mit |d| > 30; zusammenhaengende Bereiche mit Anteil > 0,2 % ausgeben.
import sys, numpy as np
from PIL import Image
A, B = sys.argv[1], sys.argv[2]
def g(p): return np.asarray(Image.open(p).convert("L").resize((160, 120), Image.BILINEAR), dtype=np.float32)
werte = []
for F in range(1540, 2901):
    a, b = g(A % F), g(B % F); d = np.abs(a - b)
    werte.append((F - 1613, float(d.mean()), float((d > 30).mean() * 100)))
gleich = sum(1 for _, m, p in werte if m == 0)
print("Bilder %d, pixelgleich %d" % (len(werte), gleich))
bereich = None
for n, m, p in werte + [(99999, 0, 0)]:
    if p > 0.2:
        if bereich is None: bereich = [n, n, p]
        else: bereich[1] = n; bereich[2] = max(bereich[2], p)
    elif bereich is not None:
        print("  Szenenbild %d..%d: max %.1f %% Pixel |d|>30" % tuple(bereich)); bereich = None
