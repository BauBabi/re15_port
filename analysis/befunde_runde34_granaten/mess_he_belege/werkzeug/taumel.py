# taumel.py <laufdir> <out.png> <F0> <F1> [halb=18] [zoom=4] : Ausschnitt um das Granaten-Sprite je Bild (Lage aus fx.log)
import re, sys, os
from PIL import Image, ImageDraw
d, out, f0, f1 = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
h = int(sys.argv[5]) if len(sys.argv) > 5 else 18
z = int(sys.argv[6]) if len(sys.argv) > 6 else 4
pos = {}; cur = None
for l in open(os.path.join(d, 'fx.log')):
    m = re.match(r'id=4 sub=13 eidx=-?\d+ frame=(\d+) .* F=(\d+)$', l.rstrip())
    if m: cur = (int(m.group(1)), int(m.group(2))); continue
    m = re.match(r'\s+-> sx=(-?\d+) sy=(-?\d+)', l)
    if m and cur: pos[cur[1]] = (cur[0], int(m.group(1)), int(m.group(2))); cur = None
ims = []; farben = []
for f in range(f0, f1 + 1):
    p = os.path.join(d, 'f_%06d.ppm' % f)
    if f not in pos or not os.path.exists(p): continue
    satz, sx, sy = pos[f]; cx, cy = sx * 3, sy * 3
    im = Image.open(p).convert('RGB').crop((cx - h, cy - h, cx + h, cy + h)).resize((2 * h * z, 2 * h * z), Image.NEAREST)
    ImageDraw.Draw(im).text((2, 2), 'F%d s%d' % (f, satz), fill=(255, 255, 0))
    ims.append(im)
cols = min(12, len(ims)); rows = (len(ims) + cols - 1) // cols
W, H = ims[0].size
s = Image.new('RGB', (cols * W, rows * H))
for i, im in enumerate(ims): s.paste(im, ((i % cols) * W, (i // cols) * H))
s.save(out); print(out, s.size, len(ims))
