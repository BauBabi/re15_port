#!/usr/bin/env python3
"""PPM -> PNG (optional verkleinert auf 320x240 mit NEAREST, optional Kontaktbogen).
Aufruf: ppm2png.py <ordner> [--klein] [--bogen out.png --spalten N --nur f_000230,f_000236,...]"""
import sys, os, glob
from PIL import Image
d = sys.argv[1]
klein = "--klein" in sys.argv
files = sorted(glob.glob(os.path.join(d, "f_*.ppm")))
if "--nur" in sys.argv:
    want = sys.argv[sys.argv.index("--nur") + 1].split(",")
    files = [f for f in files if os.path.basename(f)[:-4] in want]
if "--bogen" in sys.argv:
    out = sys.argv[sys.argv.index("--bogen") + 1]
    sp = int(sys.argv[sys.argv.index("--spalten") + 1]) if "--spalten" in sys.argv else 4
    ims = [Image.open(f).convert("RGB") for f in files]
    ims = [im.resize((320, 240), Image.NEAREST) if klein else im for im in ims]
    w, h = ims[0].size
    rows = (len(ims) + sp - 1) // sp
    b = Image.new("RGB", (w * sp, h * rows), (0, 0, 0))
    from PIL import ImageDraw
    dr = ImageDraw.Draw(b)
    for i, (f, im) in enumerate(zip(files, ims)):
        x, y = (i % sp) * w, (i // sp) * h
        b.paste(im, (x, y)); dr.text((x + 3, y + 2), os.path.basename(f)[:-4], fill=(255, 255, 0))
    b.save(out); print(out, b.size)
else:
    for f in files:
        im = Image.open(f).convert("RGB")
        if klein: im = im.resize((320, 240), Image.NEAREST)
        im.save(f[:-4] + ".png")
    print(len(files), "png")
