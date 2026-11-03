"""Bildleiste aus RE15_FRAMEDUMP-Bildern (f<NNNNNN>.ppm) mit Bildnummer, Schnitt und Untertitel-Id aus dem state.log.
Nur lesen. Aufruf: blatt2.py <laufdir> <ausgabe> <spalten> <breite> <bilder...>   (bild = Zahl oder von-bis/schritt)
Hinweis: das state.log zaehlt je Raum ab 1; genommen wird die LETZTE Zeile mit dieser Bildnummer (= der spaeteste Raum)."""
import sys, os, re
from PIL import Image, ImageDraw
d, aus, sp, br = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
liste = []
for a in sys.argv[5:]:
    m = re.match(r'(\d+)-(\d+)/(\d+)$', a)
    if m:
        liste += list(range(int(m.group(1)), int(m.group(2)) + 1, int(m.group(3))))
    else:
        liste.append(int(a))
info = {}
sl = os.path.join(d, 'state.log')
if os.path.exists(sl):
    for z in open(sl, errors='replace'):
        m = re.match(r'F(\d+) .*?cam=(\d+) msg\(a=(\d) fsm=\d+ id=(\d+)', z)
        if m:
            info[int(m.group(1))] = (int(m.group(2)), int(m.group(3)), int(m.group(4)))
bilder = []
for f in liste:
    p = os.path.join(d, 'f%06d.ppm' % f)
    if os.path.exists(p):
        bilder.append((f, Image.open(p).convert('RGB')))
w0, h0 = bilder[0][1].size
hh = int(h0 * br / w0)
zeilen = (len(bilder) + sp - 1) // sp
blatt = Image.new('RGB', (sp * br, zeilen * (hh + 16)), (255, 255, 255))
dr = ImageDraw.Draw(blatt)
for i, (f, im) in enumerate(bilder):
    x, y = (i % sp) * br, (i // sp) * (hh + 16)
    blatt.paste(im.resize((br, hh)), (x, y + 16))
    c = info.get(f)
    t = 'F%d' % f + ('  Cut %d' % c[0] if c else '') + ('  msg %d' % c[2] if c and c[1] else '')
    dr.text((x + 3, y + 2), t, fill=(0, 0, 0))
blatt.save(aus, quality=88)
print(aus, len(bilder), 'Bilder')
