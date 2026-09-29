#!/usr/bin/env python3
"""Runde 34 (Granaten) - Kontaktbogen aus RE15_FRAMEDUMP-PPMs (echte exe, beschleunigter Renderer).

Aufruf: bogen.py <marke> <ausgabe.png> <bild>[:<text>] [<bild>[:<text>] ...] [--spalten N] [--breite W]
  <marke>  Unterordner von build/r34g_baseline/ mit f_NNNNNN.ppm
Jedes Bild wird auf W Pixel Breite verkleinert (Default 480 = halbe 960er-Aufnahme), beschriftet mit
"F<bild> <text>" und in N Spalten (Default 4) gekachelt. Nur lesend auf die PPMs.
Zusaetzlich: --lupe x0,y0,x1,y1 schneidet vor dem Verkleinern einen Ausschnitt (Pixel der 960x720-Aufnahme).
"""
import os
import sys

from PIL import Image, ImageDraw

BASE = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'build', 'r34g_baseline')


def main():
    args = sys.argv[1:]
    spalten, breite, lupe = 4, 480, None
    if '--spalten' in args:
        i = args.index('--spalten'); spalten = int(args[i + 1]); del args[i:i + 2]
    if '--breite' in args:
        i = args.index('--breite'); breite = int(args[i + 1]); del args[i:i + 2]
    if '--lupe' in args:
        i = args.index('--lupe'); lupe = tuple(int(v) for v in args[i + 1].split(',')); del args[i:i + 2]
    marke, out = args[0], args[1]
    teile = []
    for spec in args[2:]:
        f, _, txt = spec.partition(':')
        p = os.path.join(BASE, marke, f'f_{int(f):06d}.ppm')
        if not os.path.exists(p):
            print('fehlt', p)
            continue
        im = Image.open(p).convert('RGB')
        if lupe:
            im = im.crop(lupe)
        h = int(im.height * breite / im.width)
        im = im.resize((breite, h), Image.NEAREST if lupe else Image.BILINEAR)
        d = ImageDraw.Draw(im)
        label = f'F{int(f)} {txt}'.strip()
        d.rectangle([0, 0, 8 + 7 * len(label), 16], fill=(0, 0, 0))
        d.text((4, 2), label, fill=(255, 255, 0))
        teile.append(im)
    if not teile:
        sys.exit('keine Bilder')
    w, h = teile[0].size
    zeilen = (len(teile) + spalten - 1) // spalten
    bogen = Image.new('RGB', (w * min(spalten, len(teile)), h * zeilen), (40, 40, 40))
    for k, im in enumerate(teile):
        bogen.paste(im, ((k % spalten) * w, (k // spalten) * h))
    bogen.save(out, optimize=True)
    print(f'{out}: {len(teile)} Bilder, {bogen.size}')


if __name__ == '__main__':
    main()
