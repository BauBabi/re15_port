# Kontaktbogen: je Bild eine Zeile [A | B | Diff-Maske], optional Ausschnitt + Skalierung.
# python sheet2.py <dirA> <dirB> <out.png> <frames: 360,361,... oder 360-371> [--crop x0,y0,x1,y1] [--scale s] [--labels A,B]
import sys, os
import numpy as np
from PIL import Image, ImageDraw
def frames(s):
    out = []
    for part in s.split(','):
        if '-' in part:
            a, b = part.split('-'); out += list(range(int(a), int(b) + 1))
        else:
            out.append(int(part))
    return out
a, b, outp, fs = sys.argv[1:5]
args = sys.argv[5:]
crop = None; scale = 1.0; labels = ['master v0.8.19', 'Integration ef1c6f94']
if '--crop' in args:
    crop = tuple(int(v) for v in args[args.index('--crop') + 1].split(','))
if '--scale' in args:
    scale = float(args[args.index('--scale') + 1])
if '--labels' in args:
    labels = args[args.index('--labels') + 1].split(',')
rows = []
for n in frames(fs):
    pa = os.path.join(a, 'f_%06d.ppm' % n); pb = os.path.join(b, 'f_%06d.ppm' % n)
    if not (os.path.exists(pa) and os.path.exists(pb)):
        continue
    A = Image.open(pa).convert('RGB'); B = Image.open(pb).convert('RGB')
    if crop:
        A = A.crop(crop); B = B.crop(crop)
    an = np.asarray(A).astype(np.int16); bn = np.asarray(B).astype(np.int16)
    d = (np.abs(an - bn).max(axis=2) > 0)
    D = Image.fromarray((d * 255).astype(np.uint8)).convert('RGB')
    if scale != 1.0:
        sz = (max(1, int(A.width * scale)), max(1, int(A.height * scale)))
        A = A.resize(sz, Image.NEAREST); B = B.resize(sz, Image.NEAREST); D = D.resize(sz, Image.NEAREST)
    w, h = A.width, A.height
    row = Image.new('RGB', (w * 3 + 8, h + 14), (40, 40, 40))
    row.paste(A, (0, 14)); row.paste(B, (w + 4, 14)); row.paste(D, (2 * w + 8, 14))
    dr = ImageDraw.Draw(row)
    dr.text((2, 1), 'F%d %s' % (n, labels[0]), fill=(255, 255, 0))
    dr.text((w + 6, 1), labels[1], fill=(255, 255, 0))
    dr.text((2 * w + 10, 1), 'Diff (%d px)' % int(d.sum()), fill=(255, 255, 0))
    rows.append(row)
if not rows:
    print('keine Bilder'); sys.exit(1)
W = max(r.width for r in rows); H = sum(r.height + 2 for r in rows)
S = Image.new('RGB', (W, H), (0, 0, 0)); y = 0
for r in rows:
    S.paste(r, (0, y)); y += r.height + 2
S.save(outp)
print(outp, S.size)
