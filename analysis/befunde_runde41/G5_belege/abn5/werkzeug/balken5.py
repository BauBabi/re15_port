# Abnahme 5: Hoehe der schwarzen Baender oben/unten je Bild (Zeilenmittel Spalten 40..300 <= Schwelle), RE2 echt gegen Port
import numpy as np, sys
from PIL import Image
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
v = np.load(B + "/re2/re2_6130_rgb.npy").astype(np.float32).mean(axis=3)
def baender(g, s=6.0):
    zm = g[:, 40:300].mean(axis=1)
    oben = 0
    while oben < 240 and zm[oben] <= s: oben += 1
    unten = 0
    while unten < 240 and zm[239 - unten] <= s: unten += 1
    return oben, unten
def port(l, f): return np.asarray(Image.open(B + "/lauf/%s/fd/f%06d.ppm" % (l, f)).convert("L")).astype(np.float32)
print(' k  RE2(oben,unten)  Leon  Elza')
for k in range(-1, 18):
    r = baender(v[10 + 2 * k]); a = baender(port('fdp_leon', 3551 + k)); b = baender(port('fdp_elza', 3551 + k))
    print('%2d  %-14s %-14s %-14s' % (k, r, a, b))
