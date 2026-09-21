#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Large textured multi-view render of named RE1.5 props + their TIM atlas."""
import os, sys, json, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import md1tex, timlib
from PIL import Image, ImageDraw, ImageFont

ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\Re1.5\PSX"
WORK = sys.argv[1]
OUT = sys.argv[2]
want = sys.argv[3:]
rows = json.load(open(os.path.join(WORK, 'props.json')))
idx = {}
for r in rows:
    idx['%s/m%02d' % (r['room'], r['idx'])] = r
S = 220
LBL = 26
try:
    F = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 14)
except Exception:
    F = ImageFont.load_default()
VIEWS = [(0, 0), (35, -18), (90, 0), (20, -75)]
sheet = Image.new('RGB', (S * (len(VIEWS) + 1) + 20, (S + LBL) * len(want)), (8, 8, 10))
dr = ImageDraw.Draw(sheet)
for i, key in enumerate(want):
    r = idx[key]
    d = open(os.path.join(ROOT, r['stage'], r['room'] + '.RDT'), 'rb').read()
    blob = d[r['md1_off']: r['md1_off'] + r['md1_len']]
    faces = md1tex.parse_uv(blob)
    tim = timlib.decode(d, r['tim_off']) if r['tim_off'] else None
    y = i * (S + LBL)
    for j, (ya, pi) in enumerate(VIEWS):
        sheet.paste(md1tex.render_tex(faces, tim, S, S, 'front',
                                     yaw=math.radians(ya), pitch=math.radians(pi)),
                    (j * S, y + LBL))
    if tim and tim['w']:
        img = timlib.to_image(tim, 0)
        img = img.resize((S, min(S, int(S * tim['h'] / tim['w']))), Image.NEAREST)
        sheet.paste(img, (len(VIEWS) * S + 10, y + LBL))
    dr.text((3, y + 4), '%s  dx=%d dy=%d dz=%d faces=%d  %s  [0 | 35 | 90 | top | TIM]'
            % (key, r['dx'], r['dy'], r['dz'], r['nfaces'],
               ('TIM %dbpp %dx%d @0x%X' % (tim['bpp'], tim['w'], tim['h'], r['tim_off'])) if tim else 'no TIM'),
            fill=(235, 235, 140), font=F)
sheet.save(OUT)
print('wrote', OUT)
