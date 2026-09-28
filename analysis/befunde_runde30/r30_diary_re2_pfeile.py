#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_diary_re2_pfeile.py - RE2s Blaetter-Pfeile und die Ende-Marke mit der RICHTIGEN CLUT-Zeile.

Der erste Durchgang las die CLUT-Zeilen von COMMON/DATA/ST0.TIM (zweites TIM @Datei 0x10820)
an ihrer DATEI-Lage (256,480) und fand fuer die Pfeil-Indizes 5/6/7 nur 0x0000. Der Lader
verschiebt den CLUT-Block aber:

  80068588  addiu v0,zero,2587      ; 0x0a1b: Seite 0x1b, CLUT-Zeile 0x0a
  80068590  sh    v0,0(s0)          ; -> 0x800cfbf0
  8006858c  jal   0x80076a40        ; TIM-Lader, a0 = 0x801a8820 (= 0x80198000 + 0x10820)
  80076b00  lbu   v0,-1039(v0)      ; 0x800cfbf1 = 0x0a
  80076b08  addiu v0,v0,480
  80076b0c  sh    v0,2(v1)          ; CLUT-Rechteck y = 490
  80076a6c  sll   v0,v1,6 ; 80076a80 addiu v0,v0,-1024   ; Bild x = 0x1b*64 - 1024 = 704 (y = 256)

Datei-Zeile k liegt also im VRAM auf y = 490 + k:
  CLUT (256,490) = Datei-Zeile 0  -> Ende-Marke "EXIT"   (@0x800725cc-d0)
  CLUT (256,492) = Datei-Zeile 2  -> Pfeile              (@0x80072628-2c)
Texturseite der drei Sprites: DR_MODE 0x800d6c20, SetDrawMode(..., tpage = 27) @0x800687bc-e4
= 4bpp, (704,256) - dieselbe Lage, auf die der Lader das zweite TIM legt.

Aufruf: python analysis/befunde_runde30/r30_diary_re2_pfeile.py [ausgabeverzeichnis]
"""
import os, struct, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "r30_irons-diary-dokument")
ST0 = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ST0.TIM")
LADE_ZEILE = 10          # 0x0a aus 0x0a1b @0x80068588


def main():
    b = open(ST0, "rb").read()
    o = 0x10820
    magic, flags = struct.unpack_from("<II", b, o)
    assert magic == 0x10 and flags == 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o + 8)
    clut = struct.unpack_from("<%dH" % (cw * ch), b, o + 20)
    p = o + 8 + clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, p)
    d = b[p + 12:p + ilen]

    def px(u, v):
        x = d[v * iw * 2 + u // 2]
        return (x >> 4) if (u & 1) else (x & 15)

    def farbe(vram_y, idx):
        k = vram_y - 480 - LADE_ZEILE
        return clut[k * cw + idx]

    aus = []
    aus.append("ST0.TIM zweites TIM @Datei 0x%x: CLUT-Block Datei-Lage (%d,%d) %dx%d, Bild %dx%d (4bpp)"
               % (o, cx, cy, cw, ch, iw * 4, ih))
    aus.append("Lader: CLUT-Zeile 0x%02x -> VRAM y = %d..%d ; Bild -> (704,256)"
               % (LADE_ZEILE, 480 + LADE_ZEILE, 480 + LADE_ZEILE + ch - 1))
    teile = (("Pfeil links", 28, 12, 12, 13, 492), ("Pfeil rechts", 42, 12, 12, 13, 492),
             ("Ende-Marke", 56, 12, 42, 14, 490))
    bild = Image.new("RGB", (8 + sum(t[3] * 8 + 8 for t in teile), 14 * 8 + 16), (0, 0, 0))
    x0 = 8
    for name, u0, v0, w, h, cy_v in teile:
        benutzt = sorted(set(px(u, v) for v in range(v0, v0 + h) for u in range(u0, u0 + w)))
        aus.append("%-12s u=%d v=%d %dx%d CLUT (256,%d) = Datei-Zeile %d" %
                   (name, u0, v0, w, h, cy_v, cy_v - 480 - LADE_ZEILE))
        for i in benutzt:
            c = farbe(cy_v, i)
            aus.append("      Index %2d -> 0x%04x  (r,g,b = %d,%d,%d von 31)%s"
                       % (i, c, c & 31, (c >> 5) & 31, (c >> 10) & 31,
                          "  durchsichtig" if c == 0 else ("  STP" if c & 0x8000 else "")))
        for v in range(h):
            for u in range(w):
                c = farbe(cy_v, px(u0 + u, v0 + v))
                if c:
                    f = ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
                    for yy in range(8):
                        for xx in range(8):
                            bild.putpixel((x0 + u * 8 + xx, 8 + v * 8 + yy), f)
        x0 += w * 8 + 8
    bild.save(os.path.join(OUT, "re2_pfeile_und_endemarke.png"))
    txt = "\n".join(aus) + "\n"
    open(os.path.join(OUT, "re2_pfeile_und_endemarke.txt"), "w", encoding="utf-8").write(txt)
    sys.stdout.write(txt)


main()
