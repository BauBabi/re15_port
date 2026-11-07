# Abnahme 5: Leiste RE2 echt | Port Leon | Port Elza (eigene Laeufe fdp_leon/fdp_elza, RE15_FRAMEDUMP nativ 320x240).
# Ausrichtung "schnitt": k Bilder nach dem ersten Bild mit sichtbarem Schnitt 16/9 (RE2 Videobild 10 + 2k ab 61,30 s; Port F3551 + k).
# Ausrichtung "szene": Szenenbild b (Armierung = 0): RE2 Videobild 10 + 2(58 + b); Port F3605 + b.
# Aufruf: leiste5.py ausgabe schnitt|szene k-Liste x0,y0,x1,y1 skala
import numpy as np, sys
from PIL import Image, ImageDraw
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
v = np.load(B + "/re2/re2_6130_rgb.npy")
out, art = sys.argv[1], sys.argv[2]
ks = [int(a) for a in sys.argv[3].split(",")]
x0, y0, x1, y1 = [int(a) for a in sys.argv[4].split(",")]; sk = float(sys.argv[5])
W, H = int((x1 - x0) * sk), int((y1 - y0) * sk)
img = Image.new("RGB", (3 * (W + 4), len(ks) * (H + 14)), (0, 0, 40)); dr = ImageDraw.Draw(img)
for r, k in enumerate(ks):
    if art == "schnitt": vi, pf, txt = 10 + 2 * k, 3551 + k, "Schnitt+%d" % k
    else: vi, pf, txt = 10 + 2 * (58 + k), 3605 + k, "Bild %d" % k
    t = 61.300 + vi / 59.8173
    tiles = [Image.fromarray(v[vi])]
    for la in ("fdp_leon", "fdp_elza"):
        tiles.append(Image.open("%s/lauf/%s/fd/f%06d.ppm" % (B, la, pf)).convert("RGB"))
    y = r * (H + 14)
    dr.text((2, y), "%s: RE2 echt %.3f s | Port Leon 5090 F%d | Port Elza 5091 F%d" % (txt, t, pf, pf), fill=(255, 255, 0))
    for c, im in enumerate(tiles):
        img.paste(im.crop((x0, y0, x1, y1)).resize((W, H), Image.NEAREST), (c * (W + 4), y + 14))
img.save(out, quality=92); print(out, img.size)
