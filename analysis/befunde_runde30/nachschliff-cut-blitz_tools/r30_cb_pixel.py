#!/usr/bin/env python3
"""Runde 30 Nachschliff, Spur cut-blitz: Pixelvergleich zweier Framedumps (PPM P6, 960x720).

  r30_cb_pixel.py <a.ppm> <b.ppm> [--quad]

Gibt die Zahl der abweichenden Pixel, deren Huelle in 320x240-Lage (x/3, y/3; die Konvention
des Gegenpruefers: Huelle x 148,5..213,2 / y 108,5..145,2) und das mittlere RGB der
abweichenden Pixel in b aus. Keine Schwelle: jedes Byte zaehlt.
"""
import sys


def lies(p):
    with open(p, "rb") as f:
        d = f.read()
    # P6\n<w> <h>\n255\n
    teile = d.split(b"\n", 3)
    assert teile[0] == b"P6", p
    w, h = map(int, teile[1].split())
    return w, h, teile[3]


def vergleich(a, b):
    wa, ha, da = lies(a)
    wb, hb, db = lies(b)
    assert (wa, ha) == (wb, hb)
    n = 0
    x0 = y0 = 10 ** 9
    x1 = y1 = -1
    sr = sg = sb = 0
    for i in range(wa * ha):
        o = i * 3
        if da[o:o + 3] != db[o:o + 3]:
            n += 1
            x, y = i % wa, i // wa
            x0 = min(x0, x); x1 = max(x1, x)
            y0 = min(y0, y); y1 = max(y1, y)
            sr += db[o]; sg += db[o + 1]; sb += db[o + 2]
    return n, (x0, x1, y0, y1), (sr, sg, sb)


def main():
    a, b = sys.argv[1], sys.argv[2]
    n, (x0, x1, y0, y1), (sr, sg, sb) = vergleich(a, b)
    if n == 0:
        print("abweichend=0")
        return
    # 960x720 -> 320x240: Pixelmitte (x+0.5)/3
    print("abweichend=%d huelle320 x %.1f..%.1f y %.1f..%.1f rgb_b=(%d,%d,%d)" % (
        n, (x0 + 0.5) / 3, (x1 + 0.5) / 3, (y0 + 0.5) / 3, (y1 + 0.5) / 3,
        sr // n, sg // n, sb // n))


if __name__ == "__main__":
    main()
