"""Bildleiste 6030-Ausgang (Slot 2) vor / nach dem Zugstart (Nachbesserung 2)."""
import sys
from PIL import Image, ImageDraw

vor, nach, ziel = sys.argv[1], sys.argv[2], sys.argv[3]
W, H = 480, 360


def lab(d, n, txt):
    im = Image.open('%s/bild_%03d.png' % (d, n)).convert('RGB').resize((W, H))
    dr = ImageDraw.Draw(im)
    dr.rectangle((0, 0, W, 18), fill=(0, 0, 0))
    dr.text((5, 4), txt, fill=(255, 220, 80))
    return im


zeilen = [
    [(vor, 136, 'VOR Zugstart: Wagen 6030 Cut 6, Leon am Ausgang (Slot 2)'),
     (vor, 142, 'VOR: Aktion -> Tuer (main00 @0x00C72) oeffnet'),
     (vor, 151, 'VOR: Tuerfahrt (Tuer 42)'),
     (vor, 172, 'VOR: angekommen ROOM6000 (1693,-10054)')],
    [(nach, 136, 'NACH Zugstart (9,133)=1: gleiche Stelle'),
     (nach, 142, 'NACH: Aktion -> msg 20 (sub06 @0x01098)'),
     (nach, 151, 'NACH: "You can\'t get off the train right now."'),
     (nach, 172, 'NACH: Leon bleibt im Wagen')],
]
blatt = Image.new('RGB', (4 * W, 2 * H), (20, 20, 20))
for r, z in enumerate(zeilen):
    for c, (d, n, t) in enumerate(z):
        blatt.paste(lab(d, n, 'Bild %d  %s' % (n, t)), (c * W, r * H))
blatt.save(ziel, quality=86)
print(ziel, blatt.size)
