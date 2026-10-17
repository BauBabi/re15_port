# kontaktbogen.py <ausgabe.png> <spalten> <titel> <bild.ppm>[=beschriftung] ...
# Setzt RE15_FRAMEDUMP-Bilder (PPM, Fenster-Skalierung 1 = 320x240) zu einem Kontaktbogen zusammen; jede Zelle
# traegt ihre Beschriftung (Standard: Bildnummer aus dem Dateinamen).
import os, re, sys
from PIL import Image, ImageDraw

aus, spalten, titel = sys.argv[1], int(sys.argv[2]), sys.argv[3]
eintraege = []
for a in sys.argv[4:]:
    pfad, _, txt = a.partition("=")
    if not txt:
        m = re.search(r"(\d+)\.ppm$", pfad)
        txt = "F%d" % int(m.group(1)) if m else os.path.basename(pfad)
    eintraege.append((pfad, txt))
bilder = [(Image.open(p).convert("RGB"), t) for p, t in eintraege]
w, h = bilder[0][0].size
zeilen = (len(bilder) + spalten - 1) // spalten
kopf = 18
bogen = Image.new("RGB", (spalten * w, kopf + zeilen * (h + 14)), (0, 0, 0))
d = ImageDraw.Draw(bogen)
d.text((4, 3), titel, fill=(255, 255, 255))
for i, (im, t) in enumerate(bilder):
    x, y = (i % spalten) * w, kopf + (i // spalten) * (h + 14)
    d.text((x + 4, y + 1), t, fill=(120, 255, 120))
    bogen.paste(im, (x, y + 14))
bogen.save(aus)
print(aus, bogen.size)
