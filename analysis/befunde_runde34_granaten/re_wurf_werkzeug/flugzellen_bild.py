#!/usr/bin/env python3
"""Runde 34 / Granate: die 12 Flug-Zellen (CORE00.ESP Effekt 4, Anim-Saetze 23..34) aus einem
4bpp-TIM-Schnitt von DATA/TEX.TIM rendern (x8), einmal mit der Granaten-CLUT 0x7B11 (sub 0x0D,
(sub>>3)*0x40 = +0x40 auf 0x7AD1, FUN_80019700 @0x8001987c-88) und einmal mit der Huelsen-CLUT
0x7AD1 (sub < 8) zum Vergleich. Zellen (u,v) aus esp_effekt.py; Kante 16 (Satz-Byte 3).

Aufruf: python flugzellen_bild.py <tim_7b11> <tim_7ad1> <out.png>
"""
import struct, sys
from PIL import Image

CELLS = [(0,112),(16,112),(32,112),(64,112),(80,112),(96,88),(96,104),(96,88),(80,112),
         (48,112),(32,112),(16,112)]   # Anim 23..34 (koord 16,17,18,20,21,22,23,22,21,19,18,17)

def load(path):
    d = open(path, "rb").read()
    flag = struct.unpack_from("<I", d, 4)[0]
    off = 8
    clut = None
    if flag & 8:
        n, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, off)
        clut = [struct.unpack_from("<H", d, off + 12 + 2 * i)[0] for i in range(cw * ch)]
        off += n
    n, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, off)
    pix = d[off + 12: off + 12 + iw * ih * 2]
    W = iw * 4
    idx = []
    for y in range(ih):
        row = []
        for x in range(iw * 2):
            byte = pix[y * iw * 2 + x]
            row += [byte & 15, byte >> 4]
        idx.append(row)
    return clut, idx, W, ih

def rgba(c):
    if c == 0:
        return (0, 0, 0, 0)
    r = (c & 31) << 3; g = ((c >> 5) & 31) << 3; b = ((c >> 10) & 31) << 3
    return (r, g, b, 255)

def strip(clut, idx, scale=8):
    img = Image.new("RGBA", (16 * len(CELLS) + 4 * (len(CELLS) - 1), 16), (40, 40, 40, 255))
    for k, (u, v) in enumerate(CELLS):
        for y in range(16):
            for x in range(16):
                img.putpixel((k * 20 + x, y), rgba(clut[idx[v + y][u + x]]))
    return img.resize((img.width * scale, img.height * scale), Image.NEAREST)

def main():
    c1, i1, _, _ = load(sys.argv[1])
    c2, i2, _, _ = load(sys.argv[2])
    a = strip(c1, i1); b = strip(c2, i2)
    out = Image.new("RGBA", (a.width, a.height * 2 + 16), (0, 0, 0, 255))
    out.paste(a, (0, 0)); out.paste(b, (0, a.height + 16))
    out.save(sys.argv[3])
    print("geschrieben", sys.argv[3], out.size, "(oben CLUT 0x7B11 = Granate, unten 0x7AD1 = Huelse)")

if __name__ == "__main__":
    main()
