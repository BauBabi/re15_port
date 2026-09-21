"""Das Bild fuer die EINE offene Rueckfrage: die 12 Punkte, die im Bild wirken und die
ich nicht zuordnen kann. Links der Hintergrund, rechts dasselbe mit der Freistellung
des Nutzers (rot) und diesen 12 Punkten (gelb)."""
import numpy as np
from PIL import Image, ImageDraw

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy")
O = "analysis/befunde_2026-09-21/pri-runde19b-marke4/"
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]

OFFEN = [(55, 132), (56, 132), (57, 132), (57, 133),
         (61, 142), (61, 143), (61, 144), (61, 145),
         (60, 148), (60, 149), (60, 150), (60, 151)]
m = np.zeros_like(reg)
for x, y in OFFEN:
    m[y, x] = True

x0, x1, y0, y1, Z = 50, 70, 126, 158, 34
W = (x1 - x0) * Z; Hh = (y1 - y0) * Z
out = Image.new("RGB", (W * 2 + 14, Hh + 22), (18, 18, 18))
d = ImageDraw.Draw(out, "RGBA")
for i in range(2):
    ox = i * (W + 14)
    f = bg[y0:y1, x0:x1].astype(np.float64) / 255.0
    out.paste(Image.fromarray(((f ** 0.42) * 255).astype(np.uint8)).resize((W, Hh), Image.NEAREST),
              (ox, 20))
    d.text((ox + 2, 5), "HINTERGRUND" if i == 0
           else "rot = Ihre Freistellung 07_01, gelb = die 12 offenen Punkte", fill=(255, 255, 255))
    if i == 1:
        for yy in range(y0, y1):
            for xx in range(x0, x1):
                if reg[yy, xx]:
                    d.rectangle([ox + (xx - x0) * Z, 20 + (yy - y0) * Z,
                                 ox + (xx - x0) * Z + Z - 1, 20 + (yy - y0) * Z + Z - 1],
                                outline=(255, 40, 40, 230), width=4)
                if m[yy, xx]:
                    d.rectangle([ox + (xx - x0) * Z + 3, 20 + (yy - y0) * Z + 3,
                                 ox + (xx - x0) * Z + Z - 4, 20 + (yy - y0) * Z + Z - 4],
                                outline=(255, 230, 0, 255), width=4)
    for xx in range(x0, x1):
        d.line([(ox + (xx - x0) * Z, 20), (ox + (xx - x0) * Z, Hh + 20)], fill=(0, 255, 255, 60))
        if xx % 5 == 0:
            d.line([(ox + (xx - x0) * Z, 20), (ox + (xx - x0) * Z, Hh + 20)], fill=(0, 255, 255, 160))
            d.text((ox + (xx - x0) * Z + 2, 22), str(xx), fill=(255, 255, 0, 255))
    for yy in range(y0, y1):
        d.line([(ox, 20 + (yy - y0) * Z), (ox + W, 20 + (yy - y0) * Z)], fill=(0, 255, 255, 60))
        if yy % 5 == 0:
            d.line([(ox, 20 + (yy - y0) * Z), (ox + W, 20 + (yy - y0) * Z)], fill=(0, 255, 255, 160))
            d.text((ox + 2, 22 + (yy - y0) * Z), str(yy), fill=(255, 255, 0, 255))
out.save(O + "31_offene_12_punkte.png")
print("geschrieben:", O + "31_offene_12_punkte.png")
print("\nDie 12 Punkte (x, y): Helligkeit r+g+b, b-r")
for x, y in OFFEN:
    print("   (%2d,%3d)  Hell %3d  b-r %+3d" % (x, y, L[y, x], BR[y, x]))
print("\nVergleich: dieselbe Struktur INNERHALB Ihrer Freistellung")
for x, y in [(61, 137), (61, 138), (62, 138), (61, 139), (62, 139), (61, 140), (62, 140),
             (61, 141), (62, 141)]:
    print("   (%2d,%3d)  Hell %3d  b-r %+3d   (in der Freistellung)" % (x, y, L[y, x], BR[y, x]))
