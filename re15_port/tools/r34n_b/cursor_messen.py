#!/usr/bin/env python3
"""Spur B: misst den 11F0-Welt-Cursor in Framedumps (960x720 = Skala 3 oder 320x240).
Cursor-Pixel = die Palettenfarben der Cursor-Textur ROOM11F0.RDT TIM @0x018DAC (hellgruen
Winkel, gelbes Kreuz). Ausgabe je Bild: Anzahl, bbox, Mittelpunkt (in 320er-Koordinaten),
dazu die haeufigsten Cursor-Farben (fuer die Frage "beleuchtet oder neutral?").
Aufruf: cursor_messen.py <ordner> [--farben]"""
import sys, os, glob, collections
from PIL import Image
d = sys.argv[1]
farben = "--farben" in sys.argv
for f in sorted(glob.glob(os.path.join(d, "f_*.ppm"))):
    im = Image.open(f).convert("RGB")
    W, H = im.size; s = W / 320.0
    px = im.load()
    pts = []; hist = collections.Counter()
    for y in range(H):
        for x in range(W):
            r, g, b = px[x, y]
            gruen = g >= 150 and r <= 100 and b <= 60
            gelb = r >= 150 and g >= 150 and b <= 80
            if gruen or gelb:
                pts.append((x, y)); hist[(r, g, b)] += 1
    name = os.path.basename(f)[:-4]
    if not pts:
        print(f"{name}: kein Cursor-Pixel"); continue
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
    print(f"{name}: n={len(pts)} bbox320 x {min(xs)/s:.1f}..{max(xs)/s:.1f} y {min(ys)/s:.1f}..{max(ys)/s:.1f}"
          f"  Mitte ({(min(xs)+max(xs))/2/s:.1f},{(min(ys)+max(ys))/2/s:.1f})")
    if farben:
        print("   Farben:", hist.most_common(6))
