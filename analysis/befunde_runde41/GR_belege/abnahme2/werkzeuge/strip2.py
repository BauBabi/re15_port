"""strip2.py out.jpg scale "label|video|x,y,w,h|f1:txt1,f2:txt2,..." ... - Zeilen aus Einzelbildern (sequentiell dekodiert)."""
import sys, cv2
from PIL import Image, ImageDraw
out = sys.argv[1]; sc = float(sys.argv[2]); rows = []
for spec in sys.argv[3:]:
    lab, vid, crop, fl = spec.split('|')
    x, y, w, h = [int(v) for v in crop.split(',')]
    items = []
    for t in fl.split(','):
        fr, _, txt = t.partition(':'); items.append((int(fr), txt))
    want = {f for f, _ in items}; got = {}
    cap = cv2.VideoCapture(vid); i = 0; last = max(want)
    while i <= last:
        ok, f = cap.read()
        if not ok: break
        if i in want: got[i] = f[y:y+h, x:x+w]
        i += 1
    tiles = []
    for fr, txt in items:
        f = got.get(fr)
        im = Image.fromarray(cv2.cvtColor(f, cv2.COLOR_BGR2RGB)) if f is not None else Image.new('RGB', (w, h))
        im = im.resize((int(w * sc), int(h * sc)))
        d = ImageDraw.Draw(im); d.rectangle([0, 0, im.width - 1, 24], fill=(0, 0, 0))
        d.text((3, 1), f"{lab} #{fr}", fill=(255, 255, 0)); d.text((3, 12), txt, fill=(255, 255, 255))
        tiles.append(im)
    rows.append(tiles)
W = max(sum(t.width for t in r) for r in rows); H = sum(max(t.height for t in r) for r in rows)
S = Image.new('RGB', (W, H), (30, 30, 30)); yy = 0
for r in rows:
    xx = 0
    for t in r: S.paste(t, (xx, yy)); xx += t.width
    yy += max(t.height for t in r)
S.save(out, quality=88); print(out, S.size)
