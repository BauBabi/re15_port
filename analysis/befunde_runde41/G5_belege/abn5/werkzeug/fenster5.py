# Abnahme 5: echtes Fenster (PrintWindow der eigenen exe-Kopie) gegen den Framedump DESSELBEN Laufs - je Fensterbild das aehnlichste
# Framedump-Bild (320x240 grau, mittlere |d|), Ausgabe Zuordnung + Leiste Fenster | Framedump.
import numpy as np, sys, os, glob
from PIL import Image, ImageDraw
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
l = sys.argv[1]; out = sys.argv[2]
fds = sorted(glob.glob(B + "/lauf/%s/fd/f*.ppm" % l))
def g(p): return np.asarray(Image.open(p).convert("L").resize((320, 240), Image.BILINEAR)).astype(np.int16)
F = {int(os.path.basename(p)[1:7]): g(p) for p in fds}
pws = sorted(glob.glob(B + "/lauf/%s/pw/w_*.png" % l))
paare = []
for p in pws:
    w = g(p); best = min(F, key=lambda f: np.abs(F[f] - w).mean())
    d = np.abs(F[best] - w).mean(); paare.append((p, best, d))
    print('%s -> F%d  mittl |d| %.2f' % (os.path.basename(p), best, d))
print('Fenster-Bilder %d, Framedump %d (%s), Median mittl |d| %.2f, Max %.2f' % (len(pws), len(fds), Image.open(fds[0]).size, np.median([x[2] for x in paare]), max(x[2] for x in paare)))
sel = paare[::max(1, len(paare) // 8)][:8]
img = Image.new("RGB", (2 * 324, len(sel) * 254), (0, 0, 40)); dr = ImageDraw.Draw(img)
for r, (p, f, d) in enumerate(sel):
    img.paste(Image.open(p).convert("RGB").resize((320, 240), Image.BILINEAR), (0, r * 254 + 14))
    img.paste(Image.open(B + "/lauf/%s/fd/f%06d.ppm" % (l, f)).convert("RGB").resize((320, 240), Image.BILINEAR), (324, r * 254 + 14))
    dr.text((2, r * 254), "echtes Fenster %s | Framedump F%d (|d| %.2f)" % (os.path.basename(p), f, d), fill=(255, 255, 0))
img.save(out); print(out)
