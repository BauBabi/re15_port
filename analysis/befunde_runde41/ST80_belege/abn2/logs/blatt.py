# ST80 Abnahme 2: Bildleiste aus einer RE15_FRAMEDUMP-Serie (fd_NNNNNN.ppm), beschriftet, 3 Spalten; dazu _klein.
#   python blatt.py <laufordner> <ziel.jpg> "<bild>:<text>" ...
import sys, os
from PIL import Image, ImageDraw
laufdir, ziel = sys.argv[1], sys.argv[2]
bilder = []
for p in sys.argv[3:]:
    n, txt = p.split(':', 1)
    im = Image.open(os.path.join(laufdir, 'fd_%06d.ppm' % int(n))).convert('RGB').resize((640, 480))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 640, 18), fill=(0, 0, 0))
    d.text((6, 3), 'F%s  %s' % (n, txt), fill=(255, 220, 80))
    bilder.append(im)
sp = 3
zeilen = (len(bilder) + sp - 1) // sp
blatt = Image.new('RGB', (640 * sp, 480 * zeilen), (20, 20, 20))
for i, b in enumerate(bilder):
    blatt.paste(b, ((i % sp) * 640, (i // sp) * 480))
blatt.save(ziel, quality=85)
klein = blatt.resize((blatt.width // 2, blatt.height // 2))
klein.save(ziel.replace('.jpg', '_klein.jpg'), quality=85)
print('geschrieben', ziel, blatt.size, os.path.getsize(ziel), 'B')
