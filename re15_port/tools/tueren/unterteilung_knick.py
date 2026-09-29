#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""unterteilung_knick.py - Knick-Mass einer Bildkante (Runde 32, analysis/befunde_runde32/tueren_unterteilung.md).

Eine im Tuermodell GERADE Kante (Unterkante Fenster, Oberkante Feld, Griffleiste) bleibt unter einer
Zentralprojektion gerade. Affine Texturierung je grossem Dreieck knickt sie an der Dreiecksdiagonale
(V-Form). Mass: je Spalte x im Fenster [x0,x1] die Zeile des staerksten Helligkeitssprungs in [y0,y1]
(Richtung +1 = oben dunkel -> unten hell, -1 umgekehrt; +2/-2 = senkrechte Kante: je Zeile y in
[y0,y1] die Spalte des Sprungs in [x0,x1], +2 = links dunkel -> rechts hell), Ausgleichsgerade, Abweichung der Kantenpunkte
(max und RMS, in Bildpixeln des Abzugs). Ueberlagerung als PNG zum Ansehen.

    python unterteilung_knick.py <bild.ppm> x0 x1 y0 y1 richtung [ueberlagerung.png] [--folgen xs]

--folgen xs: die Kante wird ab Spalte xs (Suche in [y0,y1]) nach links und rechts VERFOLGT (je
Nachbarspalte nur +-2 Zeilen um die vorige) - so springt die Messung nicht auf eine zweite Kante
im Fenster (Feld-Oberkante unter der Fenster-Unterkante bei DOOR13).
"""
import sys
from PIL import Image, ImageDraw


def _sprung(px, x, y, richtung):
    a = sum(px[xx, y - 2] + px[xx, y - 1] for xx in (x - 1, x, x + 1))
    b = sum(px[xx, y + 1] + px[xx, y + 2] for xx in (x - 1, x, x + 1))
    return (b - a) * richtung


def folgen(bild, x0, x1, y0, y1, richtung, xs):
    im = Image.open(bild).convert("L")
    px = im.load()
    def best(x, lo, hi):
        lo, hi = max(lo, 2), min(hi, im.height - 3)
        return max(((_sprung(px, x, y, richtung), y) for y in range(lo, hi + 1)))
    g, ys = best(xs, y0, y1)
    pts = {xs: ys}
    for schritt in (-1, 1):
        y = ys
        x = xs + schritt
        while x0 <= x <= x1:
            g, y = best(x, y - 2, y + 2)
            if g <= 0:
                break
            pts[x] = y
            x += schritt
    return im, sorted(pts.items())


def kante(bild, x0, x1, y0, y1, richtung):
    im = Image.open(bild).convert("L")
    if abs(richtung) == 2:   # senkrechte Kante: Bild transponieren, Punkte zurueckdrehen
        im2 = im.transpose(Image.TRANSPOSE)
        tmp = "_knick_t.png"
        im2.save(tmp)
        _, pts = kante(tmp, y0, y1, x0, x1, richtung // 2)
        return im, [(y, x) for (x, y) in pts]
    px = im.load()
    pts = []
    for x in range(x0, x1 + 1):
        best, by = None, None
        for y in range(max(y0, 2), min(y1, im.height - 3) + 1):
            # 3 Spalten gemittelt, Sprung ueber 2 Zeilen
            a = sum(px[xx, y - 2] + px[xx, y - 1] for xx in (x - 1, x, x + 1))
            b = sum(px[xx, y + 1] + px[xx, y + 2] for xx in (x - 1, x, x + 1))
            g = (b - a) * richtung
            if best is None or g > best:
                best, by = g, y
        if best is not None and best > 0:
            pts.append((x, by))
    return im, pts


def gerade(pts):
    n = len(pts)
    sx = sum(p[0] for p in pts); sy = sum(p[1] for p in pts)
    sxx = sum(p[0] * p[0] for p in pts); sxy = sum(p[0] * p[1] for p in pts)
    a = (n * sxy - sx * sy) / (n * sxx - sx * sx)
    b = (sy - a * sx) / n
    res = [p[1] - (a * p[0] + b) for p in pts]
    return a, b, res


def mass(bild, x0, x1, y0, y1, richtung, ueber=None, xs=None):
    if xs is not None:
        im, pts = folgen(bild, x0, x1, y0, y1, richtung, xs)
    else:
        im, pts = kante(bild, x0, x1, y0, y1, richtung)
    senk = abs(richtung) == 2
    a, b, res = gerade([(y, x) for (x, y) in pts] if senk else pts)
    mx = max(abs(r) for r in res)
    rms = (sum(r * r for r in res) / len(res)) ** 0.5
    srt = sorted(abs(r) for r in res)
    p95 = srt[min(len(srt) - 1, int(0.95 * len(srt)))]
    if ueber:
        o = Image.open(bild).convert("RGB")
        d = ImageDraw.Draw(o)
        d.rectangle((x0, y0, x1, y1), outline=(0, 90, 255))
        if senk:
            d.line((a * y0 + b, y0, a * y1 + b, y1), fill=(0, 255, 0))
        else:
            d.line((x0, a * x0 + b, x1, a * x1 + b), fill=(0, 255, 0))
        for (x, y) in pts:
            o.putpixel((x, y), (255, 40, 40))
        o.save(ueber)
    return {"punkte": len(pts), "max": mx, "p95": p95, "rms": rms, "steigung": a}


if __name__ == "__main__":
    arg = sys.argv[1:]
    xs = None
    if "--folgen" in arg:
        i = arg.index("--folgen")
        xs = int(arg[i + 1])
        del arg[i:i + 2]
    b = arg[0]
    x0, x1, y0, y1, r = (int(v) for v in arg[1:6])
    m = mass(b, x0, x1, y0, y1, r, arg[6] if len(arg) > 6 else None, xs)
    print("%s  Punkte %d  Abweichung max %.2f px  95%% %.2f px  RMS %.2f px  Steigung %.4f" %
          (b, m["punkte"], m["max"], m["p95"], m["rms"], m["steigung"]))
