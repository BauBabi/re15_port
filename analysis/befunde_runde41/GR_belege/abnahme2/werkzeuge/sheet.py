"""sheet.py <video> <out.jpg> <t0> <t1> <step> [cols] [scale] [crop=x,y,w,h]  - SEQUENTIELL dekodiert (kein Seek)."""
import sys, cv2
from PIL import Image, ImageDraw
v, out, t0, t1, st = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])
cols = int(sys.argv[6]) if len(sys.argv) > 6 else 6
sc = float(sys.argv[7]) if len(sys.argv) > 7 else 0.5
crop = None
for a in sys.argv[8:]:
    if a.startswith('crop='): crop = [int(x) for x in a[5:].split(',')]
fps = 30.0; want = []; t = t0
while t <= t1 + 1e-6: want.append(int(round(t * fps))); t += st
ws = set(want); got = {}; cap = cv2.VideoCapture(v); i = 0
while i <= max(want):
    ok, fr = cap.read()
    if not ok: break
    if i in ws: got[i] = fr
    i += 1
tiles = []
for i in want:
    fr = got.get(i)
    if fr is None: break
    if crop: x, y, w, h = crop; fr = fr[y:y+h, x:x+w]
    im = Image.fromarray(cv2.cvtColor(fr, cv2.COLOR_BGR2RGB)); im = im.resize((int(im.width * sc), int(im.height * sc)))
    d = ImageDraw.Draw(im); d.rectangle([0, 0, 110, 13], fill=(0, 0, 0)); d.text((2, 1), f"{i/fps:.2f}s #{i}", fill=(255, 255, 0))
    tiles.append(im)
W, H = tiles[0].size; rows = (len(tiles) + cols - 1) // cols
S = Image.new('RGB', (W * cols, H * rows), (40, 40, 40))
for k, im in enumerate(tiles): S.paste(im, ((k % cols) * W, (k // cols) * H))
S.save(out, quality=88); print(out, len(tiles), 'Bilder', S.size, 'decodiert', i)
