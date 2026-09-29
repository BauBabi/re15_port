#!/usr/bin/env python3
"""Runde 33 / Thema K: RE2-Kartenkacheln mit gesetztem Bank-32-Bit (Satzbyte +13) in beiden
CLUT-Zeilen rendern (normal 501 / 498 gegen Variante 506 / 503), 4x vergroessert.
Raumtabelle je Blatt @0x800AAA38 (+0 Zeiger, +4 Anzahl, +5 Karten-Bit, +6 cdFileNo), Satz 16 B
(+0 u, +1 v, +2 w, +3 h, +12 Besucht-Bit Bank 9, +13 Bit in Bank 32 = 0x800D4920).
CLUT-Zeilen aus info/re2leon/COMMON/DATA/ST0.TIM, zweites TIM @0x10820 (Zeile 498 = Datei 0x10934).
Ausgabe: analysis/befunde_runde33/karte_belege/re2_b32_area<A>_idx<I>.png
"""
import struct, os, sys
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
OUT = os.path.join(REPO, 'analysis', 'befunde_runde33', 'karte_belege')
os.makedirs(OUT, exist_ok=True)
exe = open(os.path.join(REPO, 'info/re2leon/PSX.EXE'), 'rb').read()
st0 = open(os.path.join(REPO, 'info/re2leon/COMMON/DATA/ST0.TIM'), 'rb').read()
def off(a): return a - 0x80010000 + 0x800
def clut(y):
    o = 0x10934 + (y - 498) * 0x20
    return struct.unpack_from('<16H', st0, o)
def rgb(c): return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
def tim(path):
    t = open(path, 'rb').read(); o = 8
    if struct.unpack_from('<I', t, 4)[0] & 8: o += struct.unpack_from('<I', t, o)[0]
    blen, x, y, w, h = struct.unpack_from('<IHHHH', t, o)
    pix = t[o + 12:o + blen]
    return lambda u, v: (pix[v * w * 2 + u // 2] >> 4) if (u & 1) else (pix[v * w * 2 + u // 2] & 15)
for area, idx in [(3, 2), (3, 14), (5, 8), (6, 8), (2, 14), (16, 5), (17, 4)]:
    p, cnt, mb, cd, _ = struct.unpack_from('<IBBBB', exe, off(0x800AAA38 + area * 8))
    r = exe[off(p) + idx * 16: off(p) + idx * 16 + 16]
    u, v, w, h = r[0], r[1], r[2], r[3]
    px = tim(os.path.join(REPO, 'info/re2leon/COMMON/DATA/MAPS/MAPS_%03d.TIM' % (cd + 1)))
    rows = [(501, 'besucht'), (506, 'besucht+b32'), (498, 'unbesucht'), (503, 'unbesucht+b32')]
    S = 4; img = Image.new('RGB', ((w + 4) * len(rows) * S, h * S), (60, 60, 60))
    for k, (cy, _) in enumerate(rows):
        c = clut(cy)
        for yy in range(h):
            for xx in range(w):
                i = px(u + xx, v + yy)
                col = (0, 0, 0) if (i == 0 or c[i] == 0) else rgb(c[i])
                for dy in range(S):
                    for dx in range(S):
                        img.putpixel(((k * (w + 4) + xx) * S + dx, yy * S + dy), col)
    fn = os.path.join(OUT, 're2_b32_area%d_idx%d.png' % (area, idx))
    img.save(fn); print(fn, 'b32 bit', r[13], 'Reihenfolge:', [n for _, n in rows])
