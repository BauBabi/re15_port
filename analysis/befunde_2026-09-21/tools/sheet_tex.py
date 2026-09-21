#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Textured contact sheet of every distinct RE2 room-prop geometry.

Each tile: front view | 3/4 view, textured with the prop's own TIM out of the
same RDT (page/clut taken from the MD1 UV records, sampled exactly as the PSX
GPU addresses VRAM).
"""
import os, sys, json, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1tex, timlib
from PIL import Image, ImageDraw, ImageFont

RDTDIR = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon\PL0\RDT"
WORK = sys.argv[1]
OUTD = sys.argv[2]
ONLY = sys.argv[3:] if len(sys.argv) > 3 else None
os.makedirs(OUTD, exist_ok=True)
rows = json.load(open(os.path.join(WORK, 'props.json')))
ok = [r for r in rows if r['ok']]
seen = {}
for r in ok:
    seen.setdefault(r['md5'], []).append(r)
reps = []
for h, lst in seen.items():
    lst.sort(key=lambda r: (r['room'], r['idx']))
    reps.append(lst)


def rank(lst):
    r = lst[0]
    dx, dy, dz = r['dx'], r['dy'], r['dz']
    if min(dx, dz) <= 0 or dy <= 0:
        return (2, 0)
    rnd = abs(dx - dz) / float(max(dx, dz))
    size_ok = 1 if 200 <= dy <= 1200 and 50 <= max(dx, dz) <= 420 else 0
    return (0 if size_ok else 1, rnd - (dy / float(max(dx, dz))) * 0.05)


reps.sort(key=rank)
if ONLY:
    reps = [l for l in reps if ('%s/m%02d' % (l[0]['room'], l[0]['idx'])) in ONLY]

cache = {}


def rdt(room):
    if room not in cache:
        cache[room] = open(os.path.join(RDTDIR, room + '.RDT'), 'rb').read()
    return cache[room]


TW, TH = 152, 152
LBL = 30
COLS, ROWS = 6, 6
PER = COLS * ROWS
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 12)
except Exception:
    F = ImageFont.load_default()
print('tiles: %d' % len(reps))
npage = (len(reps) + PER - 1) // PER
for pg in range(npage):
    sheet = Image.new('RGB', (COLS * (TW * 2 + 8), ROWS * (TH + LBL)), (8, 8, 10))
    dr = ImageDraw.Draw(sheet)
    for k in range(PER):
        gi = pg * PER + k
        if gi >= len(reps):
            break
        lst = reps[gi]
        r = lst[0]
        d = rdt(r['room'])
        blob = d[r['md1_off']: r['md1_off'] + r['md1_len']]
        faces = md1tex.parse_uv(blob)
        tim = timlib.decode(d, r['tim_off']) if r['tim_off'] else None
        cx = (k % COLS) * (TW * 2 + 8)
        cy = (k // COLS) * (TH + LBL)
        sheet.paste(md1tex.render_tex(faces, tim, TW, TH, 'front'), (cx, cy + LBL))
        sheet.paste(md1tex.render_tex(faces, tim, TW, TH, 'front',
                                      yaw=math.radians(35), pitch=math.radians(-18)),
                    (cx + TW + 4, cy + LBL))
        dr.text((cx + 2, cy + 1), '#%03d %s m%02d x%d' % (gi, r['room'], r['idx'], len(lst)),
                fill=(230, 230, 120), font=F)
        dr.text((cx + 2, cy + 14), 'dx%d dy%d dz%d %s' % (
            r['dx'], r['dy'], r['dz'],
            ('tim%dbpp %dx%d' % (tim['bpp'], tim['w'], tim['h'])) if tim else 'noTIM'),
            fill=(150, 200, 230), font=F)
    fn = os.path.join(OUTD, 'tex_page%02d.png' % pg)
    sheet.save(fn)
    print('wrote', fn)
