#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Contact sheet of EVERY distinct MD1 mesh found on the RE2-Leon disc."""
import os, sys, json, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1lib
from PIL import Image, ImageDraw, ImageFont

WORK = sys.argv[1]
OUTD = sys.argv[2]
os.makedirs(OUTD, exist_ok=True)
scan = json.load(open(os.path.join(WORK, 'scan.json')))
occ = collections.defaultdict(list)
for r in scan:
    occ[r['md5']].append(r)

items = []
for h, lst in occ.items():
    p = os.path.join(WORK, 'md1', h + '.md1')
    md1 = md1lib.parse(open(p, 'rb').read())
    if md1 is None:
        continue
    polys = md1lib.faces_world(md1)
    bb = md1lib.bbox(polys)
    if not bb:
        continue
    dx, dy, dz = bb[1] - bb[0], bb[3] - bb[2], bb[5] - bb[4]
    items.append({'md5': h, 'path': p, 'polys': polys, 'dx': dx, 'dy': dy,
                  'dz': dz, 'n': len(lst), 'where': lst, 'nmesh': md1['nmesh']})


def slender(it):
    d = sorted([it['dx'], it['dy'], it['dz']])
    if d[0] <= 0:
        return 1e9
    return d[2] / float(d[0])


# order: upright, roughly cylindrical, hand-to-body sized first — i.e. the
# geometric signature of a fire extinguisher (tall, round cross-section)
def rank(it):
    dx, dy, dz = it['dx'], it['dy'], it['dz']
    if min(dx, dz) <= 0 or dy <= 0:
        return (2, 0)
    round_ness = abs(dx - dz) / float(max(dx, dz))     # 0 = circular footprint
    upright = dy / float(max(dx, dz))                  # >1 = taller than wide
    size_ok = 1 if 200 <= dy <= 1200 and 60 <= max(dx, dz) <= 400 else 0
    return (0 if size_ok else 1, round_ness - upright * 0.05)


items.sort(key=rank)

TW, TH = 148, 148
LBL = 30
COLS, ROWS = 6, 6
PER = COLS * ROWS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 12)
except Exception:
    F = ImageFont.load_default()
print('distinct MD1 meshes: %d' % len(items))
npage = (len(items) + PER - 1) // PER
for pg in range(npage):
    W = COLS * (TW * 2 + 8)
    H = ROWS * (TH + LBL)
    sheet = Image.new('RGB', (W, H), (8, 8, 10))
    dr = ImageDraw.Draw(sheet)
    for k in range(PER):
        gi = pg * PER + k
        if gi >= len(items):
            break
        it = items[gi]
        cx = (k % COLS) * (TW * 2 + 8)
        cy = (k // COLS) * (TH + LBL)
        sheet.paste(md1lib.render_z(it['polys'], TW, TH, 'front'), (cx, cy + LBL))
        sheet.paste(md1lib.render_z(it['polys'], TW, TH, 'side'), (cx + TW + 4, cy + LBL))
        w0 = it['where'][0]
        nm = os.path.basename(w0['file'])
        dr.text((cx + 2, cy + 1), '#%03d %s@0x%X x%d' % (gi, nm[:18], w0['off'], it['n']),
                fill=(230, 230, 120), font=F)
        dr.text((cx + 2, cy + 14), 'dx%d dy%d dz%d m%d f%d' % (it['dx'], it['dy'], it['dz'],
                it['nmesh'], w0['nfaces']), fill=(150, 200, 230), font=F)
    fn = os.path.join(OUTD, 'all_page%02d.png' % pg)
    sheet.save(fn)
    print('wrote', fn)

with open(os.path.join(OUTD, 'index_all.txt'), 'w') as f:
    for gi, it in enumerate(items):
        f.write('#%03d md5=%s dx=%d dy=%d dz=%d nmesh=%d n=%d\n' % (
            gi, it['md5'], it['dx'], it['dy'], it['dz'], it['nmesh'], it['n']))
        for w in it['where'][:8]:
            f.write('       %s @0x%X size=%d faces=%d\n' % (w['file'], w['off'], w['size'], w['nfaces']))
print('wrote index_all.txt')
