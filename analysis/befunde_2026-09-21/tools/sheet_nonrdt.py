#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Contact sheet of every distinct MD1 that is NOT an RDT room prop
(door archives, enemy EMDs, player PLD/PLW, EMS overlays, ...)."""
import os, sys, json, collections, math
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


def is_rdt(f):
    u = f.upper()
    return u.startswith('PL0/RDT/') and (u.endswith('.RDT') or '/OBJ/' in u)


items = []
for h, lst in occ.items():
    if all(is_rdt(r['file']) for r in lst):
        continue
    p = os.path.join(WORK, 'md1', h + '.md1')
    md1 = md1lib.parse(open(p, 'rb').read())
    if md1 is None:
        continue
    polys = md1lib.faces_world(md1)
    bb = md1lib.bbox(polys)
    if not bb:
        continue
    items.append({'md5': h, 'polys': polys, 'nmesh': md1['nmesh'],
                  'dx': bb[1] - bb[0], 'dy': bb[3] - bb[2], 'dz': bb[5] - bb[4],
                  'where': lst})
items.sort(key=lambda it: (it['where'][0]['file'], it['where'][0]['off']))
TW, TH = 148, 148
LBL = 30
COLS, ROWS = 6, 6
PER = COLS * ROWS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 12)
except Exception:
    F = ImageFont.load_default()
print('distinct non-RDT MD1 meshes: %d' % len(items))
byf = collections.Counter()
for it in items:
    byf[it['where'][0]['file'].split('/')[1] if '/' in it['where'][0]['file'] else '?'] += 1
print(dict(byf))
npage = (len(items) + PER - 1) // PER
for pg in range(npage):
    sheet = Image.new('RGB', (COLS * (TW * 2 + 8), ROWS * (TH + LBL)), (8, 8, 10))
    dr = ImageDraw.Draw(sheet)
    for k in range(PER):
        gi = pg * PER + k
        if gi >= len(items):
            break
        it = items[gi]
        cx = (k % COLS) * (TW * 2 + 8)
        cy = (k // COLS) * (TH + LBL)
        sheet.paste(md1lib.render_z(it['polys'], TW, TH, 'front'), (cx, cy + LBL))
        sheet.paste(md1lib.render_z(it['polys'], TW, TH, 'front',
                                    yaw=math.radians(40), pitch=math.radians(-20)),
                    (cx + TW + 4, cy + LBL))
        w = it['where'][0]
        dr.text((cx + 2, cy + 1), '#%03d %s' % (gi, w['file'].split('/')[-1][:24]),
                fill=(230, 230, 120), font=F)
        dr.text((cx + 2, cy + 14), '@0x%X dx%d dy%d dz%d m%d' % (
            w['off'], it['dx'], it['dy'], it['dz'], it['nmesh']),
            fill=(150, 200, 230), font=F)
    fn = os.path.join(OUTD, 'nonrdt_page%02d.png' % pg)
    sheet.save(fn)
    print('wrote', fn)
with open(os.path.join(OUTD, 'index_nonrdt.txt'), 'w') as f:
    for gi, it in enumerate(items):
        f.write('#%03d dx=%d dy=%d dz=%d nmesh=%d\n' % (gi, it['dx'], it['dy'], it['dz'], it['nmesh']))
        for w in it['where'][:6]:
            f.write('      %s @0x%X size=%d faces=%d\n' % (w['file'], w['off'], w['size'], w['nfaces']))
