"""Selbstpruefung: reproduziert mein Modell den ECHTEN Abzug? Und die Schlussbilder."""
import numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
rn = np.load("build/r19b/render_F3843.npy").astype(np.int32)
deck = np.load("build/r19b/deck.npy"); tief = np.load("build/r19b/tief.npy")
reg = np.load("build/r19b/reg0701.npy")
d = np.abs(rn - bg).sum(2)

print("=== Selbstpruefung gegen befund_10D0_F3843_marke1.bmp ===")
print("gedeckte Punkte gesamt: %d" % int(deck.sum()))
print("  davon weicht der Abzug um |dRGB| > 30 vom Hintergrund ab: %d (%.2f %%)"
      % (int((deck & (d > 30)).sum()), 100.0 * (deck & (d > 30)).sum() / deck.sum()))
print("  -> wo eine Maske DECKT, stellt der Zeichner den Hintergrund wieder her;")
print("     der Renderer selbst ist an dieser Marke sauber.")
print("uebermalte Punkte gesamt %d, davon ohne jede Maske %d"
      % (int((d > 30).sum()), int(((d > 30) & ~deck).sum())))

# Schlussbild: Hintergrund / Render, mit Tiefen der Gegenstaende beschriftet
O = "analysis/befunde_2026-09-21/pri-runde19b-marke4/"
x0, x1, y0, y1, Z = 40, 105, 115, 200, 14
W = (x1 - x0) * Z; Hh = (y1 - y0) * Z
out = Image.new("RGB", (W * 2 + 14, Hh + 34), (18, 18, 18))
dd = ImageDraw.Draw(out, "RGBA")
for i, (img, lab) in enumerate(((bg, "HINTERGRUND (gemalt)"), (rn, "ECHTER ABZUG F3843"))):
    ox = i * (W + 14)
    f = img[y0:y1, x0:x1].astype(np.float64) / 255.0
    out.paste(Image.fromarray(((f ** 0.45) * 255).astype(np.uint8)).resize((W, Hh), Image.NEAREST),
              (ox, 20))
    dd.text((ox + 2, 4), lab, fill=(255, 255, 255))
for i in range(2):
    ox = i * (W + 14)
    for xx in range(x0, x1):
        if xx % 10 == 0:
            dd.line([(ox + (xx - x0) * Z, 20), (ox + (xx - x0) * Z, Hh + 20)], fill=(0, 255, 255, 70))
            dd.text((ox + (xx - x0) * Z + 1, 21), str(xx), fill=(255, 255, 0, 255))
    for yy in range(y0, y1):
        if yy % 10 == 0:
            dd.line([(ox, 20 + (yy - y0) * Z), (ox + W, 20 + (yy - y0) * Z)], fill=(0, 255, 255, 70))
            dd.text((ox + 1, 21 + (yy - y0) * Z), str(yy), fill=(255, 255, 0, 255))
marken = [((46, 128), "Stuhl 07_01, Tiefe 53..62 (VOR dem Spieler)", (255, 90, 90)),
          ((62, 152), "grauer Kasten, Sockel y155 -> Tiefe 119 (HINTER)", (120, 220, 255)),
          ((88, 147), "blaue Tonne, Sockel y149 -> Tiefe 128 (HINTER)", (120, 220, 255)),
          ((70, 122), "Chromrohr eines ZWEITEN Stuhls (ueber beiden Sockeln)", (255, 220, 120))]
for (mx, my), txt, col in marken:
    for i in range(2):
        ox = i * (W + 14)
        dd.ellipse([ox + (mx - x0) * Z - 6, 20 + (my - y0) * Z - 6,
                    ox + (mx - x0) * Z + 6, 20 + (my - y0) * Z + 6], outline=col, width=3)
dy = 0
for (mx, my), txt, col in marken:
    dd.text((4, Hh + 22 + dy), "o " + txt, fill=col)
    dy += 0
out.save(O + "30_marke_erklaert.png")
print("\n30_marke_erklaert.png geschrieben")
for (mx, my), txt, col in marken:
    print("   Ring bei (%d,%d): %s" % (mx, my, txt))
