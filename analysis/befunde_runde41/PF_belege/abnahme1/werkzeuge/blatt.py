"""blatt.py <lauf> <von_s> <bis_s> <schritt_s> [spalten] -> runs/<lauf>/blatt_<von>_<bis>.png (logisch 320x240 je Bild, Zeit oben)"""
import sys, os, json
from PIL import Image, ImageDraw
l, a, b, s = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4])
sp = int(sys.argv[5]) if len(sys.argv) > 5 else 5
wd = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'runs', l)
fps = json.load(open(os.path.join(wd, 'zeitachse.json')))['fps']
ts = []
t = a
while t <= b + 1e-6:
    ts.append(t); t += s
bl = Image.new('RGB', (sp * 322, ((len(ts) + sp - 1) // sp) * 254), (40, 40, 40))
for k, t in enumerate(ts):
    i = int(round(t * fps)) + 1
    p = os.path.join(wd, 'bilder', 'b_%05d.png' % i)
    if not os.path.exists(p): continue
    im = Image.open(p).convert('RGB')
    sc = im.size[0] // 320
    im = im.resize((320, 240), Image.NEAREST) if sc > 1 else im
    x, y = (k % sp) * 322, (k // sp) * 254
    bl.paste(im, (x, y + 14))
    ImageDraw.Draw(bl).text((x + 2, y + 1), 't=%.2f b_%05d' % (t, i), fill=(255, 255, 0))
out = os.path.join(wd, 'blatt_%g_%g.png' % (a, b))
bl.save(out); print(out)
