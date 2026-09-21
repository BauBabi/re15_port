"""Schlussbild: was an Marke F3843 wirklich zu sehen ist, Gegenstand fuer Gegenstand."""
import numpy as np
from PIL import Image, ImageDraw

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
rn = np.load("build/r19b/render_F3843.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy")
O = "analysis/befunde_2026-09-21/pri-runde19b-marke4/"

x0, x1, y0, y1, Z = 40, 105, 115, 200, 14
W = (x1 - x0) * Z; Hh = (y1 - y0) * Z
LEG = 92
out = Image.new("RGB", (W * 2 + 14, Hh + 22 + LEG), (18, 18, 18))
d = ImageDraw.Draw(out, "RGBA")
for i, (img, lab) in enumerate(((bg, "HINTERGRUND (gemalt)"),
                                (rn, "ECHTER ABZUG F3843 (Vollbild-Readback)"))):
    ox = i * (W + 14)
    f = img[y0:y1, x0:x1].astype(np.float64) / 255.0
    out.paste(Image.fromarray(((f ** 0.45) * 255).astype(np.uint8)).resize((W, Hh), Image.NEAREST),
              (ox, 20))
    d.text((ox + 2, 5), lab, fill=(255, 255, 255))
    for xx in range(x0, x1):
        if xx % 10 == 0:
            d.line([(ox + (xx - x0) * Z, 20), (ox + (xx - x0) * Z, Hh + 20)], fill=(0, 255, 255, 70))
            d.text((ox + (xx - x0) * Z + 1, 21), str(xx), fill=(255, 255, 0, 255))
    for yy in range(y0, y1):
        if yy % 10 == 0:
            d.line([(ox, 20 + (yy - y0) * Z), (ox + W, 20 + (yy - y0) * Z)], fill=(0, 255, 255, 70))
            d.text((ox + 1, 21 + (yy - y0) * Z), str(yy), fill=(255, 255, 0, 255))

marken = [
    ("1", (44, 140), (255, 90, 90),
     "1  Klappstuhl 07_01 (Nutzer-Freistellung): Tiefe 53..62 = VOR dem Spieler (Fuss 94)."
     "  2677/2677 Punkte gedeckt, im Abzug bleibt sein Rohr stehen."),
    ("2", (62, 145), (120, 220, 255),
     "2  heller Behaelter dahinter: Sockel Bildzeile 155/156 -> Kamera-z 7663 -> Tiefe 119."
     "  HINTER dem Spieler (6049) -> zu Recht uebermalt, er traegt keine Maske."),
    ("3", (88, 140), (120, 220, 255),
     "3  blaue Tonne: Sockel Bildzeile 149 -> Kamera-z 8211 -> Tiefe 128. Ebenfalls HINTER."),
    ("4", (72, 132), (255, 220, 120),
     "4  Chromrohr eines ZWEITEN Stuhls (x68..76 y120..140, 22 helle Punkte): liegt ganz"
     "  oberhalb beider Sockelzeilen, also noch ferner. Kein Teil von 07_01."),
]
for nr, (mx, my), col, _ in marken:
    for i in range(2):
        ox = i * (W + 14)
        cx, cy = ox + (mx - x0) * Z + Z // 2, 20 + (my - y0) * Z + Z // 2
        d.ellipse([cx - 9, cy - 9, cx + 9, cy + 9], outline=col, width=3)
        d.text((cx - 3, cy - 6), nr, fill=col)
yy = Hh + 26
for nr, _, col, txt in marken:
    d.text((6, yy), txt, fill=col)
    yy += 22
out.save(O + "30_marke_erklaert.png")
print("geschrieben:", O + "30_marke_erklaert.png")
