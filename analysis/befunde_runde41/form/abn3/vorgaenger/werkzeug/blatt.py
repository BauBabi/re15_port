# Abnahme 3: Kontaktblatt aus einer Framedump-Serie mit Beschriftung (Bild, Cut aus state.log, fast schwarz in Promille).
#   python blatt.py <laufdir> <von> <bis> <schritt> <ausgabe.png> [spalten] [kachelbreite]
import sys, os, re
import numpy as np
from PIL import Image, ImageDraw
d, von, bis, sch, aus = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
sp = int(sys.argv[6]) if len(sys.argv) > 6 else 6
kb = int(sys.argv[7]) if len(sys.argv) > 7 else 256
kh = kb * 3 // 4
cam = {}
form = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'^F(\d+) pad=.*? cam=(\d+)', line)
    if m:
        cam[int(m.group(1))] = int(m.group(2))
        e = re.search(r't=30 st=(\d+) [^]]*mo=(\d+) [^]]*@\((-?\d+),(-?\d+),r-?\d+\)\] hp=(-?\d+)', line)
        if e: form[int(m.group(1))] = (int(e.group(2)), int(e.group(3)))
bilder = [f for f in range(von, bis + 1, sch) if os.path.exists(os.path.join(d, 'fd', 'f%06d.ppm' % f))]
zl = (len(bilder) + sp - 1) // sp
blatt = Image.new('RGB', (sp * kb, zl * (kh + 14)), (40, 40, 40))
dr = ImageDraw.Draw(blatt)
for i, f in enumerate(bilder):
    im = Image.open(os.path.join(d, 'fd', 'f%06d.ppm' % f)).convert('RGB')
    l = np.asarray(im.convert('L'), dtype=np.int32)
    dn = int((l < 24).sum() * 1000 // l.size)
    t = im.resize((kb, kh), Image.BILINEAR)
    x, y = (i % sp) * kb, (i // sp) * (kh + 14)
    blatt.paste(t, (x, y + 14))
    fo = form.get(f)
    txt = 'F%d c%s d%d%s' % (f, cam.get(f, '?'), dn, (' m%d x%d' % fo) if fo else '')
    dr.text((x + 3, y + 1), txt, fill=(255, 255, 0) if dn <= 600 else (255, 80, 80))
blatt.save(aus)
print(aus, len(bilder), 'Bilder')
