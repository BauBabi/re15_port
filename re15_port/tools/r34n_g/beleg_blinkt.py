#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - Belegbild aus einer RE15_FRAMEDUMP-Serie der echten exe.

Oben: zwei ganze Bilder (AN = Buchstaben-Masken gezeichnet, AUS = Rotlicht), auf 320x240 verkleinert.
Unten: das Schrift-Rechteck (x130..225 y18..47) je Bild einer Folge, mit Bildnummer, damit der Takt
(20 Bilder je Zustand) im Bild abzulesen ist.

  C:/Python310/python.exe re15_port/tools/r34n_g/beleg_blinkt.py <dump-praefix> <an-bild> <aus-bild>
      <von> <bis> <schritt> <ausgabe.png> [--titel TEXT] [--scale 3]
"""
import argparse

import numpy as np
from PIL import Image, ImageDraw


def load_ppm(path, k):
    data = open(path, "rb").read()
    parts = data.split(b"\n", 3)
    w, h = [int(v) for v in parts[1].split()]
    a = np.frombuffer(parts[3][:w * h * 3], dtype=np.uint8).reshape(h, w, 3)
    return a[k // 2::k, k // 2::k][:240, :320]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("an", type=int)
    ap.add_argument("aus", type=int)
    ap.add_argument("von", type=int)
    ap.add_argument("bis", type=int)
    ap.add_argument("schritt", type=int)
    ap.add_argument("out")
    ap.add_argument("--titel", default="")
    ap.add_argument("--scale", type=int, default=3)
    a = ap.parse_args()
    k = a.scale
    an = load_ppm("%s%06d.ppm" % (a.dump, a.an), k)
    aus = load_ppm("%s%06d.ppm" % (a.dump, a.aus), k)
    x0, y0, x1, y1 = 130, 18, 226, 48
    frames = list(range(a.von, a.bis + 1, a.schritt))
    cw, ch = (x1 - x0) * 2, (y1 - y0) * 2
    cols = 8
    rows = (len(frames) + cols - 1) // cols
    W = max(640 + 10, cols * (cw + 4))
    H = 20 + 240 + 10 + rows * (ch + 14) + 4
    img = Image.new("RGB", (W, H), (24, 24, 24))
    d = ImageDraw.Draw(img)
    d.text((4, 4), a.titel, fill=(255, 255, 255))
    img.paste(Image.fromarray(np.ascontiguousarray(an)), (0, 20))
    img.paste(Image.fromarray(np.ascontiguousarray(aus)), (330, 20))
    d.rectangle((x0 - 1, 20 + y0 - 1, x1, 20 + y1), outline=(0, 255, 0))
    d.rectangle((330 + x0 - 1, 20 + y0 - 1, 330 + x1, 20 + y1), outline=(0, 255, 0))
    d.text((4, 22), "F%d AN (Masken)" % a.an, fill=(0, 255, 0))
    d.text((334, 22), "F%d AUS (Rotlicht)" % a.aus, fill=(0, 255, 0))
    for i, f in enumerate(frames):
        fr = load_ppm("%s%06d.ppm" % (a.dump, f), k)
        crop = Image.fromarray(np.ascontiguousarray(fr[y0:y1, x0:x1])).resize((cw, ch), Image.NEAREST)
        cx = (i % cols) * (cw + 4)
        cy = 20 + 240 + 10 + (i // cols) * (ch + 14)
        img.paste(crop, (cx, cy + 12))
        d.text((cx + 2, cy), "F%d" % f, fill=(255, 255, 0))
    img.save(a.out, optimize=True)
    print("geschrieben:", a.out, img.size)


if __name__ == "__main__":
    main()
