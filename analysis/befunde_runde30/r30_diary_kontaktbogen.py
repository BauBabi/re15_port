#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_diary_kontaktbogen.py - Kontaktbogen ALLER Seiten des Prototyps FILE25 (Runde 30, E1).

Liest die geschriebenen TIM-Dateien (nicht die PNG des Satzwerkzeugs) und setzt daraus
EIN Bild fuer die Sichtpruefung:

  oben   jede Seite auf neutralem Grund, zweifach vergroessert, mit Seitenmarke
  unten  jede Seite im RE2-Schirmlayout 320x240:
           Grund schwarz            FUN_8002bda8(2,0)   @0x80071d8c-94 / @0x8006cf78-80
           Illustration bei (100,60) 128 x (256-H), ab v = H   @0x80072584-94, @0x800760b8-dc
           Textseite bei (25,30)                               @0x80076170-84
         Texel mit CLUT-Farbe 0x0000 sind durchsichtig (psx-spx).

Aufruf: python analysis/befunde_runde30/r30_diary_kontaktbogen.py [verzeichnis] [ausgabe.png]
"""
import os, struct, sys
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "r30_irons-diary-dokument")
ZIEL = sys.argv[2] if len(sys.argv) > 2 else os.path.join(OUT, "kontaktbogen.png")
DOC = 25


def tim(pfad):
    b = open(pfad, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    assert magic == 0x10
    o = 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
    clut = struct.unpack_from("<%dH" % (cw * ch), b, o + 12)
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, o)
    d = b[o + 12:o + ilen]
    if (flags & 7) == 0:
        W = iw * 4
        px = [[(d[y * iw * 2 + x // 2] >> 4) if (x & 1) else (d[y * iw * 2 + x // 2] & 15)
               for x in range(W)] for y in range(ih)]
    else:
        W = iw * 2
        px = [[d[y * W + x] for x in range(W)] for y in range(ih)]
    return W, ih, clut, px


def rgb(c):
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def main():
    namen = [("Titel", "FILE%02d_title_page.TIM" % DOC)]
    n = 1
    while os.path.exists(os.path.join(OUT, "FILE%02d_p%02d_page.TIM" % (DOC, n))):
        namen.append(("p%02d" % n, "FILE%02d_p%02d_page.TIM" % (DOC, n)))
        n += 1
    pw, ph, pclut, ppx = tim(os.path.join(OUT, "FILE%02d_title_paper.TIM" % DOC))

    spalten = 4
    zeilen = (len(namen) + spalten - 1) // spalten
    rand, marke = 6, 12
    # oben: lesbar, 2x
    tw, th = 256 * 2, 144 * 2
    breite = rand + spalten * (tw + rand)
    h_oben = 20 + zeilen * (th + marke + rand)
    # unten: Schirm 320x240, so skaliert, dass dieselbe Spaltenbreite entsteht
    sw, sh = tw, tw * 240 // 320
    h_unten = 20 + zeilen * (sh + marke + rand)
    bild = Image.new("RGB", (breite, h_oben + h_unten), (40, 40, 40))
    z = ImageDraw.Draw(bild)
    z.text((rand, 4), "FILE25 'Irons Diary' - Seiten auf neutralem Grund (aus den TIM gelesen, 2x)",
           fill=(255, 255, 255))
    z.text((rand, h_oben + 4),
           "FILE25 im RE2-Schirmlayout 320x240: Grund schwarz, Illustration (100,60), Textseite (25,30)",
           fill=(255, 255, 255))
    for i, (marke_txt, datei) in enumerate(namen):
        W, H, clut, px = tim(os.path.join(OUT, datei))
        a = Image.new("RGB", (W, H), (18, 26, 32))
        for y in range(H):
            for x in range(W):
                c = clut[px[y][x]]
                if c != 0:
                    a.putpixel((x, y), rgb(c))
        s = Image.new("RGB", (320, 240), (0, 0, 0))
        for v in range(ph - H):
            for u in range(pw):
                c = pclut[ppx[H + v][u]]
                if c != 0:
                    s.putpixel((100 + u, 60 + v), rgb(c))
        for y in range(H):
            for x in range(W):
                c = clut[px[y][x]]
                if c != 0 and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                    s.putpixel((25 + x, 30 + y), rgb(c))
        sp, ze = i % spalten, i // spalten
        x0 = rand + sp * (tw + rand)
        y0 = 20 + ze * (th + marke + rand)
        z.text((x0, y0), "%s   (%s, %dx%d)" % (marke_txt, datei, W, H), fill=(255, 220, 120))
        bild.paste(a.resize((tw, th), Image.NEAREST), (x0, y0 + marke))
        y1 = h_oben + 20 + ze * (sh + marke + rand)
        z.text((x0, y1), "%s   Leserseite %d" % (marke_txt, i), fill=(255, 220, 120))
        bild.paste(s.resize((sw, sh), Image.NEAREST), (x0, y1 + marke))
    bild.save(ZIEL)
    print("kontaktbogen: %d Seiten -> %s (%dx%d)" % (len(namen), ZIEL, bild.size[0], bild.size[1]))


main()
