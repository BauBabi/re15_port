# Bildleiste aus RE15_FRAMEDUMP-Serie (fd_NNNNNN.ppm) mit Beschriftung; 3 Spalten. Nur lesen/schreiben im Ziel.
import sys, os
from PIL import Image, ImageDraw
laufdir, ziel = sys.argv[1], sys.argv[2]
punkte = sys.argv[3:]
bilder = []
for p in punkte:
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
blatt.save(ziel, quality=88)
print('geschrieben', ziel, blatt.size)
