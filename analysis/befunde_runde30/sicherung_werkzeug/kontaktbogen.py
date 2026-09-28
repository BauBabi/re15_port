#!/usr/bin/env python3
"""Kontaktbogen aus Framedumps (PPM) eines Laufs, optional mit Ausschnitt.

    python .../kontaktbogen.py <lauf> <aus.png> <bilder,kommagetrennt> [x0,y0,x1,y1 in 320x240] [spalten]
"""
import os
import sys

from PIL import Image, ImageDraw

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
Z = os.path.join(REPO, "build", "r30_sicherung")


def main():
    lauf, aus = sys.argv[1], sys.argv[2]
    bilder = [int(x) for x in sys.argv[3].split(",")]
    aussch = [float(x) for x in sys.argv[4].split(",")] if len(sys.argv) > 4 and sys.argv[4] != "-" else None
    sp = int(sys.argv[5]) if len(sys.argv) > 5 else 4
    kacheln = []
    for f in bilder:
        p = os.path.join(Z, lauf, "f_%06d.ppm" % f)
        if not os.path.exists(p):
            continue
        im = Image.open(p).convert("RGB")
        s = im.size[0] / 320.0
        if aussch:
            im = im.crop((int(aussch[0] * s), int(aussch[1] * s), int(aussch[2] * s), int(aussch[3] * s)))
            im = im.resize((im.size[0] * 2, im.size[1] * 2), Image.NEAREST)
        else:
            im = im.resize((480, 360), Image.BILINEAR)
        ImageDraw.Draw(im).text((4, 4), "%s F%d" % (lauf, f), fill=(255, 255, 0))
        kacheln.append(im)
    if not kacheln:
        raise SystemExit("keine Bilder")
    w, h = kacheln[0].size
    z = (len(kacheln) + sp - 1) // sp
    bl = Image.new("RGB", (sp * (w + 6), z * (h + 6)), (255, 0, 255))
    for i, k in enumerate(kacheln):
        bl.paste(k, ((i % sp) * (w + 6), (i // sp) * (h + 6)))
    bl.save(os.path.join(Z, aus))
    print("geschrieben:", os.path.join(Z, aus), bl.size)


if __name__ == "__main__":
    main()
