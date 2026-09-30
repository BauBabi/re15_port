#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - Vorher/Nachher-Blatt zweier RE15_FRAMEDUMP-Serien (alte exe ohne
Opcode 0x45 / neue exe mit), gleiche Umgebung, gleiche Bildnummern.

Je Bild eine Zeile: VORHER | NACHHER | Unterschied (geaenderte Bildpunkte magenta auf grau).
Zaehlt je Bild die geaenderten Bildpunkte (320x240-Einheiten).

  C:/Python310/python.exe re15_port/tools/r34n_g/vorher_nachher.py <praefix-alt> <praefix-neu>
      <ausgabe.png> <bild> [<bild> ...] [--titel TEXT] [--scale 3]
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
    ap.add_argument("alt")
    ap.add_argument("neu")
    ap.add_argument("out")
    ap.add_argument("bilder", nargs="+", type=int)
    ap.add_argument("--titel", default="")
    ap.add_argument("--scale", type=int, default=3)
    a = ap.parse_args()
    rows = len(a.bilder)
    img = Image.new("RGB", (3 * 320 + 20, 20 + rows * 256), (24, 24, 24))
    d = ImageDraw.Draw(img)
    d.text((4, 4), a.titel, fill=(255, 255, 255))
    for r, f in enumerate(a.bilder):
        alt = load_ppm("%s%06d.ppm" % (a.alt, f), a.scale)
        neu = load_ppm("%s%06d.ppm" % (a.neu, f), a.scale)
        diff = np.any(alt != neu, axis=2)
        grau = (neu.mean(axis=2, keepdims=True) * 0.5).astype(np.uint8).repeat(3, axis=2)
        grau[diff] = (255, 0, 255)
        y = 20 + r * 256
        img.paste(Image.fromarray(np.ascontiguousarray(alt)), (0, y))
        img.paste(Image.fromarray(np.ascontiguousarray(neu)), (330, y))
        img.paste(Image.fromarray(grau), (660, y))
        d.text((4, y + 242), "F%d VORHER (alte exe)" % f, fill=(255, 255, 0))
        d.text((334, y + 242), "F%d NACHHER" % f, fill=(255, 255, 0))
        d.text((664, y + 242), "F%d: %d Bildpunkte anders" % (f, int(diff.sum())), fill=(255, 0, 255))
        print("F%d: %d Bildpunkte anders" % (f, int(diff.sum())))
    img.save(a.out, optimize=True)
    print("geschrieben:", a.out, img.size)


if __name__ == "__main__":
    main()
