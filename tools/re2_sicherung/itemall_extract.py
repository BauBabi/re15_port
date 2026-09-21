#!/usr/bin/env python3
"""itemall_extract.py - RE2s Inventar-Symbole aus COMMON/DATA/ITEMALL.PIX ziehen.

GEOMETRIE — 40 x 30 Pixel, 8 bpp, eine 256-Farben-CLUT. Belegt, nicht geraten:

1. Der Zeichner setzt das VRAM-Rechteck selbst (FUN_80069C40, Symbol-Upload):
       80069C74: addiu v0,zero,20   / 80069C78: sh v0,20(sp)   ; w = 20
       80069C7C: addiu v0,zero,30   / 80069C84: sh v0,22(sp)   ; h = 30
   `w` ist bei LoadImage die Breite in 16-Bit-VRAM-Worten. Bei 8 bpp sind das
   20*2 = 40 Pixel, bei 4 bpp waeren es 80.
2. Der Symbol-Cache hat 0x4B0 = 1200 Byte je Item (&DAT_8019C000 + id*0x4B0).
   40*30 = 1200 Pixel. Also GENAU 1 Byte je Pixel -> 8 bpp.
   Bei 4 bpp muessten 80*30 = 2400 Pixel in 1200 Byte passen, dann waere die
   Kachel doppelt so breit wie das Zielrechteck.
3. Der CLUT-Block ist 256 Eintraege breit (x=0..255 bei VRAM y=496). 4 bpp
   braeuchte 16 Farben; 256 ist die 8-bpp-Palette.
4. Messung: mit 4 bpp entstehen senkrechte Streifen im Abstand 1 Pixel
   (jede zweite Spalte Index 0 = transparent); mit 8 bpp verschwinden sie.

Der von einem fremden Extraktor um die Rohkacheln gelegte TIM-Kopf in
COMMON/DATA/ITEMALL/ITEMALL_*.TIM behauptet flags=0x09 (4 bpp) und w=20,
h=30 — beides zusammen ist widerspruechlich; die Kopfangabe ist falsch.
Hier wird deshalb DIREKT aus ITEMALL.PIX gelesen.

127200 Byte / 1200 Byte = 106 Kacheln.

Aufruf:
    itemall_extract.py sheet <out.png>          Kontaktbogen aller 106 Kacheln + Namen
    itemall_extract.py one <index> <out.png> [scale]
    itemall_extract.py raw <index> <out.bin>    die 1200 Rohbytes der Kachel
    itemall_extract.py pal <out.bin>            die 512 Byte CLUT (256 x u16 BGR555)
"""
import struct, sys, os
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
PIX = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ITEMALL.PIX")
TIMDIR = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ITEMALL")

W, H, BPP = 40, 30, 8
TILE = W * H                                # 1200
N_TILES = os.path.getsize(PIX) // TILE      # 106


def clut_raw(tim_index=1):
    """Die 512 CLUT-Rohbytes (256 x u16 BGR555) aus dem CLUT-Block der TIM-Datei."""
    p = os.path.join(TIMDIR, "ITEMALL_%03d.TIM" % tim_index)
    d = open(p, "rb").read()
    csz = struct.unpack_from("<I", d, 8)[0]
    return d[8 + 12: 8 + csz]


def clut(tim_index=1, bank=0):
    """256 RGBA-Farben. PSX BGR555, 0x0000 = transparent."""
    pal = clut_raw(tim_index)
    out = []
    for i in range(256):
        v = struct.unpack_from("<H", pal, i * 2)[0]
        r = (v & 0x1F) << 3
        g = ((v >> 5) & 0x1F) << 3
        b = ((v >> 10) & 0x1F) << 3
        out.append((r, g, b, 0 if v == 0 else 255))
    return out


def tile_bytes(i):
    with open(PIX, "rb") as f:
        f.seek(i * TILE)
        return f.read(TILE)


def tile_image(i, pal):
    raw = tile_bytes(i)
    im = Image.new("RGBA", (W, H))
    px = im.load()
    for y in range(H):
        for x in range(W):
            px[x, y] = pal[raw[y * W + x]]
    return im


def sheet(out, scale=3, cols=6):
    sys.path.insert(0, HERE)
    import re2_items
    pal = clut()
    cw, ch = W * scale + 8, H * scale + 16
    rows = (N_TILES + cols - 1) // cols
    img = Image.new("RGB", (cw * cols, ch * rows), (28, 28, 34))
    dr = ImageDraw.Draw(img)
    for i in range(N_TILES):
        r, c = divmod(i, cols)
        t = tile_image(i, pal).resize((W * scale, H * scale), Image.NEAREST)
        bg = Image.new("RGB", t.size, (60, 60, 70))
        bg.paste(t, (0, 0), t)
        img.paste(bg, (c * cw + 4, r * ch + 12))
        nm = re2_items.name(i)[1].strip() if i < re2_items.N_NAMES else "?"
        dr.text((c * cw + 4, r * ch + 1), "%d %s" % (i, nm[:22]), fill=(235, 235, 240))
    img.save(out)
    print("geschrieben:", out, img.size, "%d Kacheln" % N_TILES)


if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "sheet":
        sheet(sys.argv[2])
    elif mode == "one":
        i = int(sys.argv[2], 0)
        sc = int(sys.argv[4]) if len(sys.argv) > 4 else 1
        im = tile_image(i, clut())
        if sc > 1:
            im = im.resize((W * sc, H * sc), Image.NEAREST)
        im.save(sys.argv[3])
        print("geschrieben:", sys.argv[3], im.size)
    elif mode == "raw":
        i = int(sys.argv[2], 0)
        open(sys.argv[3], "wb").write(tile_bytes(i))
        print("geschrieben: %s (%d Byte, Kachel %d, Datei-Offset 0x%X)" % (
            sys.argv[3], TILE, i, i * TILE))
    elif mode == "pal":
        open(sys.argv[2], "wb").write(clut_raw())
        print("geschrieben: %s (512 Byte, 256 x u16 BGR555)" % sys.argv[2])
