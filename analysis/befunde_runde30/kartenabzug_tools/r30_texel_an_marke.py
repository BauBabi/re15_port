#!/usr/bin/env python
"""Runde 30 karten-marken (Fortsetzung): WELCHEN Palettenindex traegt die Kartenkunst an
den drei Nutzer-Marken, und kommen die Indizes 12/13/14 in den Raum-Rechtecken vor?
Quellen: info/Re1.5/PSX.EXE (Paar-Tabelle @0x80076840 {count, ptr} je Seite, Eintrag 12 B
{x,y,w,h,u,v}), re15_port/shared_assets/PSX/DATA/MAPxx.PIX (headerlos 256x256 4bpp,
unteres Nibble = linker Texel; Datei-Offset eines Texels = v*128 + u//2).
Aufruf: python r30_texel_an_marke.py"""
import struct
from collections import Counter
EXE = open('info/Re1.5/PSX.EXE', 'rb').read()
def fo(a): return a - 0x80010000 + 0x800

def rects(pg):
    cnt, ptr = struct.unpack_from('<II', EXE, fo(0x80076840 + 8 * pg))
    return [(ptr + 12 * i,) + struct.unpack_from('<hhhhhh', EXE, fo(ptr + 12 * i)) for i in range(cnt)]

def blatt(pg):
    d = open('re15_port/shared_assets/PSX/DATA/MAP%02X.PIX' % (pg + 1), 'rb').read()
    return d

def texel(d, u, v):
    b = d[(v & 255) * 128 + ((u & 255) >> 1)]
    return (b >> 4) if (u & 1) else (b & 15)

def feld(pg, x0, x1, y0, y1, name):
    d = blatt(pg)
    rs = rects(pg)
    print("\n== %s: Seite %d (MAP%02X.PIX, %d B), Schirm x %d..%d y %d..%d" % (name, pg, pg + 1, len(d), x0, x1, y0, y1))
    for i, (adr, x, y, w, h, u, v) in enumerate(rs):
        c = Counter()
        erste = None
        for sy in range(y0, y1 + 1):
            for sx in range(x0, x1 + 1):
                if x <= sx < x + w and y <= sy < y + h:
                    tu, tv = u + sx - x, v + sy - y
                    c[texel(d, tu, tv)] += 1
                    if erste is None:
                        erste = (sx, sy, tu, tv, (tv & 255) * 128 + ((tu & 255) >> 1))
        if c:
            print("   rect %2d @0x%08X (%d,%d) %dx%d uv(%d,%d): Index-Histogramm %s" %
                  (i, adr, x, y, w, h, u, v, dict(sorted(c.items()))))
            print("        erster Punkt Schirm (%d,%d) -> Texel (%d,%d) @Datei 0x%04X" % erste)
    n = sum(1 for (adr, x, y, w, h, u, v) in rs
            for sy in range(y0, y1 + 1) for sx in range(x0, x1 + 1)
            if x <= sx < x + w and y <= sy < y + h)
    if not n:
        print("   KEIN Rechteck der Seite ueberdeckt dieses Feld")

def zensus():
    print("\n== ZENSUS: Indizes 12/13/14 in den RAUM-Rechtecken aller 13 Seiten")
    tot = Counter()
    for pg in range(13):
        d = blatt(pg)
        c = Counter()
        for (adr, x, y, w, h, u, v) in rects(pg):
            for dy in range(h):
                for dx in range(w):
                    c[texel(d, u + dx, v + dy)] += 1
        ganz = Counter()
        for vv in range(256):
            for uu in range(256):
                ganz[texel(d, uu, vv)] += 1
        print("   Seite %2d: in Rechtecken  1:%5d 4:%5d 12:%3d 13:%3d 14:%3d | ganzes Blatt 12:%3d 13:%3d 14:%3d"
              % (pg, c[1], c[4], c[12], c[13], c[14], ganz[12], ganz[13], ganz[14]))
        tot.update({k: c[k] for k in (12, 13, 14)})
    print("   SUMME in Rechtecken: 12:%d 13:%d 14:%d" % (tot[12], tot[13], tot[14]))

feld(5, 148, 182, 155, 155, "ROOF untere Kante (Nutzer: 320er x 148..182,7 y 155)")
feld(5, 148, 182, 154, 154, "ROOF Zeile darueber")
feld(5, 148, 182, 156, 156, "ROOF Zeile darunter")
feld(2, 208, 220, 90, 120, "1F blaue Kachel (Nutzer: x 208..220,7 y 90..120,7)")
feld(2, 207, 221, 89, 121, "1F blaue Kachel mit Rand")
feld(3, 188, 188, 178, 182, "2F schwebende Marke (188, 178..182) - ACHTUNG: fuer Blatt 3 benutzt der Port s_map_rectfix statt dieser EXE-Tabelle (Sonde H: rectfix[2], Index 0)")
zensus()
