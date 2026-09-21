#!/usr/bin/env python3
"""re15_icon.py - RE1.5-Inventarsymbole aus info/Re1.5/PSX/DATA/ITEMALL.PIX ziehen.

Geometrie aus dem Port (re15_port/engine/src/item_icon_common.c:15-17, dort
gegen den Framebuffer des Savestates mzd_inv_open.sav abgeglichen):
    ITEMALL_TILE_BYTES 1200  = 40 x 30 x 1 Byte     ITEMALL_TILES 72
    86400 Byte = 72 * 1200   (Dateigroesse stimmt exakt)
Der Port bildet Item-Id -> Kachel bisher als `tile == id` ab (Ausnahme 0x15 -> 37).

Die CLUT liegt NICHT in der PIX-Datei. Der Port hat bisher nur zwei
framebuffer-gemessene Paletten (clut_idx 2 und 6, ids 0x00..0x18). Fuer Item
0x31 gibt es im Port noch keine. Deshalb wird hier zusaetzlich die Palette des
RE2-Symbolsatzes als PLATZHALTER angeboten (--re2pal) und die Kachel roh als
Indexbild (--gray) ausgegeben, damit die Form zweifelsfrei sichtbar ist, ohne
eine Farbpalette zu behaupten, die nicht gemessen ist.

Aufruf:
    re15_icon.py <tile-index> <out-praefix>
"""
import sys, os, struct
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
PIX15 = os.path.join(REPO, "info", "Re1.5", "PSX", "DATA", "ITEMALL.PIX")
W, H, TILE = 40, 30, 1200


def tile(i):
    with open(PIX15, "rb") as f:
        f.seek(i * TILE)
        return f.read(TILE)


if __name__ == "__main__":
    i = int(sys.argv[1], 0)
    pre = sys.argv[2]
    raw = tile(i)
    open(pre + ".bin", "wb").write(raw)

    # 1) Rohes Indexbild (Graustufe = Palettenindex) — keine Farbbehauptung.
    g = Image.new("L", (W, H))
    g.putdata(list(raw))
    g.resize((W * 6, H * 6), Image.NEAREST).save(pre + "_index.png")

    # 2) Mit der RE2-Symbolpalette als PLATZHALTER eingefaerbt.
    sys.path.insert(0, HERE)
    import itemall_extract as IX
    pal = IX.clut()
    im = Image.new("RGBA", (W, H))
    px = im.load()
    for y in range(H):
        for x in range(W):
            px[x, y] = pal[raw[y * W + x]]
    im.resize((W * 6, H * 6), Image.NEAREST).save(pre + "_re2pal.png")
    used = sorted(set(raw))
    print("Kachel %d: Datei-Offset 0x%X, %d Byte, %d verschiedene Indizes %s"
          % (i, i * TILE, TILE, len(used), used[:24]))
    print("geschrieben: %s.bin  %s_index.png  %s_re2pal.png" % (pre, pre, pre))
