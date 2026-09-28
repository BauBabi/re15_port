#!/usr/bin/env python3
"""Zieht die drei ORIGINAL-Bilder der Sicherung (Item 0x40) aus den ausgelieferten Dateien.

  (b) Item-Bild   ITEM/ITPS.ITP   @0x40*0x3000 = Datei 0xC0000, TIM 8bpp+CLUT, 112x72
                  (Ladeweg des Ports: engine/src/itps_common.c — CLUT @+0x14, Bild @+0x220)
  (c) Inventar-Icon DATA/ITEMALL.PIX Tile 0x40 @0x40*1200 = Datei 0x12C00, 40x30 8bpp ohne
                  Kopf, CLUT = DATA/ST_00.TIM CLUT-Zeile 0 (platform/pc/src/inv_render_pc.c:258-273)

Ausgabe: build/r30_sicherung/itps_40.png, icon_40.png (+ je eine 4x/8x-Vergroesserung) und
die Nachbarn 0x3F/0x41 als Gegenprobe der Indexregel (ITPS-Index == Item-Id).

    python analysis/befunde_runde30/sicherung_werkzeug/item_bilder.py
"""
import os
import struct
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import PSX, REPO, rgb15, tim_lesen  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")


def itps_bild(itps, item):
    base = item * 0x3000
    block = itps[base:base + 0x3000]
    w, h, idx, cluts, kopf = tim_lesen(block)
    img = Image.new("RGBA", (w, h))
    px = img.load()
    for y in range(h):
        for x in range(w):
            c = cluts[0][idx[y][x]]
            px[x, y] = (0, 0, 0, 0) if c == 0 else rgb15(c) + (255,)
    return img, kopf


def icon_bild(pix, st00, tile):
    w, h, idx, cluts, kopf = tim_lesen(st00)
    clut = cluts[0]
    roh = pix[tile * 1200:(tile + 1) * 1200]
    img = Image.new("RGBA", (40, 30))
    px = img.load()
    for y in range(30):
        for x in range(40):
            i = roh[y * 40 + x]
            c = clut[i]
            px[x, y] = (0, 0, 0, 0) if c == 0 else rgb15(c) + (255,)
    return img


def auf_grau(img, f):
    g = Image.new("RGBA", img.size, (40, 40, 48, 255))
    g.alpha_composite(img)
    return g.convert("RGB").resize((img.size[0] * f, img.size[1] * f), Image.NEAREST)


def statistik(name, img):
    px = img.load()
    w, h = img.size
    voll = [(x, y) for y in range(h) for x in range(w) if px[x, y][3]]
    if not voll:
        print("  %-10s  LEER" % name)
        return
    xs = [p[0] for p in voll]
    ys = [p[1] for p in voll]
    r = sum(px[p][0] for p in voll) / len(voll)
    g = sum(px[p][1] for p in voll) / len(voll)
    b = sum(px[p][2] for p in voll) / len(voll)
    print("  %-10s  %dx%d  deckend %d px  bbox x%d..%d y%d..%d  Mittel RGB (%.0f,%.0f,%.0f)"
          % (name, w, h, len(voll), min(xs), max(xs), min(ys), max(ys), r, g, b))


def main():
    os.makedirs(ZIEL, exist_ok=True)
    itps = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
    pix = open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read()
    st00 = open(os.path.join(PSX, "DATA", "ST_00.TIM"), "rb").read()
    print("ITPS.ITP %d B = %d Bloecke a 0x3000; ITEMALL.PIX %d B = %d Tiles a 1200"
          % (len(itps), len(itps) // 0x3000, len(pix), len(pix) // 1200))
    for item in (0x3F, 0x40, 0x41):
        img, kopf = itps_bild(itps, item)
        print("Item 0x%02X  ITPS @Datei 0x%X  TIM %s" % (item, item * 0x3000, kopf))
        statistik("ITPS", img)
        img.save(os.path.join(ZIEL, "itps_%02X.png" % item))
        auf_grau(img, 4).save(os.path.join(ZIEL, "itps_%02X_4x.png" % item))
        ic = icon_bild(pix, st00, item)
        statistik("Icon", ic)
        ic.save(os.path.join(ZIEL, "icon_%02X.png" % item))
        auf_grau(ic, 8).save(os.path.join(ZIEL, "icon_%02X_8x.png" % item))
    # Nachbarblock 0x21A0 (breites 80x30-Icon) ist NUR fuer breite Waffen belegt
    base = 0x40 * 0x3000 + 0x21A0
    rest = itps[base:base + 80 * 30]
    print("ITPS-Block 0x40 +0x21A0 (80x30-Breit-Icon): %d von %d Byte != 0"
          % (sum(1 for b in rest if b), len(rest)))


if __name__ == "__main__":
    main()
