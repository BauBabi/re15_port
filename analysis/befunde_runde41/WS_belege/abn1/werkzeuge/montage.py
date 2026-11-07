# Bildbeleg: Framedump-Serie eines Laufs als beschriftete Tafel (Bild-Nr. + HP/Zustand der Ziel-Slots aus state.log).
# Aufruf: montage.py <laufdir> <slots,komma> <bild1,bild2,...> <ausgabe.jpg> <titel> [spalten]
import re, sys, os
from PIL import Image, ImageDraw, ImageFont
d, slots, bilder, out, titel = sys.argv[1], sys.argv[2].split(','), [int(x) for x in sys.argv[3].split(',')], sys.argv[4], sys.argv[5]
sp = int(sys.argv[6]) if len(sys.argv) > 6 else 3
# letztes Segment des Zustandslogs (Raum nach dem Sprung, ohne Folge-Raeume) = Segment mit dem groessten Bild der Liste
segs = []; prev = -1
for l in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'F(\d+) ', l)
    if not m: continue
    f = int(m.group(1))
    if f < prev or not segs: segs.append({})
    prev = f; segs[-1][f] = l
seg = segs[1] if len(segs) > 1 else segs[0]
try: font = ImageFont.truetype('arial.ttf', 13)
except Exception: font = ImageFont.load_default()
zeilen = (len(bilder) + sp - 1) // sp
import textwrap
tz = textwrap.wrap(titel, width=max(40, sp * 43))
kopf = 6 + 16 * len(tz)
W = Image.new('RGB', (sp * 320, zeilen * 240 + kopf), (20, 20, 20))
dr = ImageDraw.Draw(W)
for k, t in enumerate(tz): dr.text((6, 3 + 16 * k), t, fill=(255, 255, 255), font=font)
for i, b in enumerate(bilder):
    p = os.path.join(d, 'fd_%06d.ppm' % b)
    x, y = (i % sp) * 320, kopf + (i // sp) * 240
    if os.path.exists(p): W.paste(Image.open(p).convert('RGB'), (x, y))
    l = seg.get(b, '')
    txt = 'F%d' % b
    for s in slots:
        m = re.search(r'\[' + s + r' t=([0-9a-f]+) st=(\d+) [^\]]*\] hp=(-?\d+)', l)
        if m: txt += '  s%s:%s hp%s Z%s' % (s, m.group(1), m.group(3), m.group(2))
    dr.rectangle([x, y, x + 319, y + 16], fill=(0, 0, 0))
    dr.text((x + 3, y + 1), txt, fill=(255, 255, 0), font=font)
W.save(out, quality=88)
print(out, W.size)
