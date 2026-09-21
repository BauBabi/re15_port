#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Contact sheet of every DISTINCT RE2 room-prop geometry (front + side view)."""
import os, sys, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1lib
from PIL import Image, ImageDraw, ImageFont

WORK = sys.argv[1]
OUTD = sys.argv[2]
os.makedirs(OUTD, exist_ok=True)
rows = json.load(open(os.path.join(WORK, 'props.json')))
ok = [r for r in rows if r['ok']]

# one representative per distinct md5, sorted by "tall & thin"-ness first so
# extinguisher-shaped things land on page 1
seen = {}
for r in ok:
    seen.setdefault(r['md5'], []).append(r)
reps = []
for h, lst in seen.items():
    lst.sort(key=lambda r: (r['room'], r['idx']))
    reps.append((h, lst))


def slender(r):
    d = sorted([r['dx'], r['dy'], r['dz']])
    if d[0] == 0:
        return 0
    return d[2] / float(max(1, d[0]))


reps.sort(key=lambda hl: (-slender(hl[1][0])))

TW, TH = 150, 150
LBL = 30
COLS, ROWS = 6, 6
PER = COLS * ROWS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 12)
except Exception:
    F = ImageFont.load_default()

print('distinct geometries: %d' % len(reps))
npage = (len(reps) + PER - 1) // PER
for pg in range(npage):
    W = COLS * (TW * 2 + 8)
    H = ROWS * (TH + LBL)
    sheet = Image.new('RGB', (W, H), (8, 8, 10))
    dr = ImageDraw.Draw(sheet)
    for k in range(PER):
        gi = pg * PER + k
        if gi >= len(reps):
            break
        h, lst = reps[gi]
        r = lst[0]
        p = os.path.join(WORK, 'md1', '%s_m%02d.md1' % (r['room'], r['idx']))
        md1 = md1lib.parse(open(p, 'rb').read())
        polys = md1lib.faces_world(md1)
        a = md1lib.render(polys, TW, TH, 'front')
        b = md1lib.render(polys, TW, TH, 'side')
        cx = (k % COLS) * (TW * 2 + 8)
        cy = (k // COLS) * (TH + LBL)
        sheet.paste(a, (cx, cy + LBL))
        sheet.paste(b, (cx + TW + 4, cy + LBL))
        lab1 = '#%03d %s m%02d  x%d' % (gi, r['room'], r['idx'], len(lst))
        lab2 = 'dx%d dy%d dz%d f%d' % (r['dx'], r['dy'], r['dz'], r['nfaces'])
        dr.text((cx + 2, cy + 1), lab1, fill=(230, 230, 120), font=F)
        dr.text((cx + 2, cy + 14), lab2, fill=(150, 200, 230), font=F)
    fn = os.path.join(OUTD, 'props_page%02d.png' % pg)
    sheet.save(fn)
    print('wrote', fn)

# also write the mapping
with open(os.path.join(OUTD, 'index.txt'), 'w') as f:
    for gi, (h, lst) in enumerate(reps):
        r = lst[0]
        f.write('#%03d md5=%s dx=%d dy=%d dz=%d faces=%d n=%d  %s\n' % (
            gi, h, r['dx'], r['dy'], r['dz'], r['nfaces'], len(lst),
            ' '.join('%s/m%02d' % (x['room'], x['idx']) for x in lst)))
print('wrote index.txt')
