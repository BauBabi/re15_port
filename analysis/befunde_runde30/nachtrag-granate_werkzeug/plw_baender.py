# -*- coding: utf-8 -*-
"""Nachtrag K: welche Flaechen des Hand+Granate-Netzes (PL00W09.PLW dir[2]) lesen das
WAFFEN-Band der Textur und welche das HAND-Band?

Beleg fuer die Baender: main.c (Kommentar WEAPON TEXTURE COMPOSITE, byte-true FUN_80036b68
@0x80036c08): jedes PL00W** dir[2] hat auf page 0x81 zwei UV-Baender - Hand v108-157 (Haut
aus PL00.TIM) und Waffe u72-126/v224-251 (= das 56x32-Bild aus dir[3], vom Port bei
Slot-0-Texel (200,480) einkopiert).
"""
import os, sys, struct, collections
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import PSX, md1_lesen  # noqa

name = sys.argv[1] if len(sys.argv) > 1 else 'PL00W09.PLW'
d = open(os.path.join(PSX, 'PLD', name), 'rb').read()
off, n = struct.unpack_from('<II', d, 0)
dr = struct.unpack_from('<4I', d, off)
m = md1_lesen(d[dr[2]:dr[3]])['meshes'][0]
k = m['kopf']
print('%s dir[2] @0x%X: tv@%d==qv@%d: %s   tn@%d==qn@%d: %s' % (name, dr[2], k[0], k[7], k[0] == k[7], k[2], k[9], k[2] == k[9]))
vl = m['tv']
band = collections.Counter()
for art, fl, uvl in (('T', m['tris'], m['tuv']), ('Q', m['quads'], m['quv'])):
    for i, (fc, uv) in enumerate(zip(fl, uvl)):
        if art == 'T':
            us = (uv[0], uv[3], uv[6]); vs = (uv[1], uv[4], uv[7])
        else:
            us = (uv[0], uv[3], uv[6], uv[9]); vs = (uv[1], uv[4], uv[7], uv[10])
        b = 'WAFFE' if min(vs) >= 200 else ('HAND' if max(vs) < 200 else 'GEMISCHT')
        band[b] += 1
        pts = [vl[p] for p in fc]
        print('  %s%02d %-8s u%3d..%3d v%3d..%3d  Punkte %s' % (
            art, i, b, min(us), max(us), min(vs), max(vs), ' '.join('(%d,%d,%d)' % p for p in pts)))
print(dict(band))
# Punkte nach Band
waffe = set(); hand = set()
for art, fl, uvl in (('T', m['tris'], m['tuv']), ('Q', m['quads'], m['quv'])):
    for fc, uv in zip(fl, uvl):
        vs = (uv[1], uv[4], uv[7]) + ((uv[10],) if art == 'Q' else ())
        (waffe if min(vs) >= 200 else hand).update(fc)
print('Punkte nur Waffe: %d, nur Hand: %d, beide: %d' % (len(waffe - hand), len(hand - waffe), len(waffe & hand)))
w = [vl[p] for p in sorted(waffe)]
print('Waffen-bbox x%d..%d y%d..%d z%d..%d' % (min(p[0] for p in w), max(p[0] for p in w), min(p[1] for p in w), max(p[1] for p in w), min(p[2] for p in w), max(p[2] for p in w)))
