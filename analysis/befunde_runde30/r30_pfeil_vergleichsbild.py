#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_pfeil_vergleichsbild.py - Nachschliff Runde 30, Spur pfeil: Vorher/Nachher-Bild.

Schneidet aus den Framedumps der beiden Laeufe (r30_pfeil_lauf.sh lauf_vorher /
lauf_nachher) den linken Pfeilbereich x 0..60 und den rechten x 250..320, y 95..140 aus,
vergroessert 4x (naechster Nachbar) und legt sie untereinander. Rot umrandet: die
Glyphen-Pixel der Textseite (FILE25, RE2-Lage (25,30) @0x80076170-84), deren Farbe im
Abzug NICHT die der Textseite ist (= vom Pfeil verdeckt).

Aufruf: python r30_pfeil_vergleichsbild.py <ausgabe.png> <lauf>:<bild>:<seite>:<text> ...
  <seite> = t | p01..p17 ; Beispiel: build/r30_n_pfeil/lauf_vorher:372:p01:"vorher p01"
"""
import os, struct, sys
import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")


def seite(name):
    f = "FILE25_title_page.TIM" if name == "t" else "FILE25_%s_page.TIM" % name
    b = open(os.path.join(FILES, f), "rb").read()
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, 8)
    clut = np.array(struct.unpack_from("<%dH" % (cw * ch), b, 20), dtype=np.uint16)
    p = 8 + ln
    ln, x, y, w, h = struct.unpack_from("<IHHHH", b, p)
    d = np.frombuffer(b[p + 12:p + ln], dtype=np.uint8).reshape(h, w * 2)
    idx = np.empty((h, w * 4), dtype=np.uint8)
    idx[:, 0::2] = d & 15
    idx[:, 1::2] = d >> 4
    col = clut[idx]
    rgb = np.stack([col & 31, (col >> 5) & 31, (col >> 10) & 31], axis=-1).astype(np.int16)
    return col != 0, rgb


def bild(lauf, F):
    for ext in (".png", ".ppm"):
        for pfad in (os.path.join(lauf, "f%06d%s" % (F, ext)),
                     os.path.join(lauf, "mess", "f%06d%s" % (F, ext))):
            if os.path.exists(pfad):
                im = np.asarray(Image.open(pfad).convert("RGB"))
                sy, sx = im.shape[0] // 240, im.shape[1] // 320
                return im[sy // 2::sy, sx // 2::sx][:240, :320]
    raise FileNotFoundError("%s F%d" % (lauf, F))


def main():
    aus = sys.argv[1]
    faelle = [a.split(":", 3) for a in sys.argv[2:]]
    Z = 4
    zw, zh = (60 + 2 + 70) * Z, 45 * Z
    B = Image.new("RGB", (zw + 8, (zh + 16) * len(faelle) + 8), (40, 40, 40))
    dr = ImageDraw.Draw(B)
    for i, (lauf, F, name, text) in enumerate(faelle):
        s = bild(lauf, int(F))
        vis, rgb = seite(name)
        h, w = vis.shape
        ab = (s[30:30 + h, 25:25 + w].astype(np.int16) >> 3)
        verdeckt = vis & ~np.all(ab == rgb, axis=-1)
        teil = np.concatenate([s[95:140, 0:60], np.full((45, 2, 3), 80, np.uint8),
                               s[95:140, 250:320]], axis=1)
        t = Image.fromarray(teil).resize((teil.shape[1] * Z, teil.shape[0] * Z), Image.NEAREST)
        x0, y0 = 4, 4 + i * (zh + 16)
        B.paste(t, (x0, y0 + 14))
        n = int(verdeckt.sum())
        dr.text((x0, y0), "%s  (F%s, %d Glyphen-Pixel verdeckt)" % (text, F, n), fill=(255, 255, 0))
        ys, xs = np.nonzero(verdeckt)
        for yy, xx in zip(ys, xs):
            X, Y = 25 + xx, 30 + yy
            if 95 <= Y < 140 and X < 60:
                px, py = x0 + X * Z, y0 + 14 + (Y - 95) * Z
                dr.rectangle([px, py, px + Z - 1, py + Z - 1], outline=(255, 0, 0))
    B.save(aus)
    print("->", aus)


main()
