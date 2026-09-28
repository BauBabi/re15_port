#!/usr/bin/env python3
"""r30_port_rows.py - Runde 30 / Thema D.
Dasselbe Zeilenmass wie r30_title_vram_rows.py, aber auf den gdigrab-Bildern des PORTS
(rohes rgb24, 320x240 je Bild): mittlere Helligkeit / Maximum / Zahl heller Pixel der
drei Menuezeilen.  Gibt Minimum- und Maximum-Bild der aktiven Zeile aus und schreibt
beide als PNG.
"""
import sys, os, numpy as np
from PIL import Image
ROWS = [("NEW GAME", 0x85), ("LOAD GAME", 0x99), ("OPTION", 0xad)]
def stats(img, y0):
    reg = img[y0:y0 + 17, 0x20:0x120].astype(np.int32)
    # wie die VRAM-Messung auf 5 Bit je Kanal quantisieren (PSX-Bildpuffer = RGB555)
    reg = (reg >> 3) << 3
    l = (reg[..., 0] * 299 + reg[..., 1] * 587 + reg[..., 2] * 114) // 1000
    return l.mean(), int(l.max()), int((l >= 128).sum())
def main():
    raw = np.fromfile(sys.argv[1], dtype=np.uint8)
    n = raw.size // (320 * 240 * 3)
    fr = raw[:n * 320 * 240 * 3].reshape(n, 240, 320, 3)
    act = [stats(f, 0x85)[0] for f in fr]
    imin, imax = int(np.argmin(act)), int(np.argmax(act))
    print("Bilder: %d" % n)
    for tag, i in (("Puls-MINIMUM", imin), ("Puls-MAXIMUM", imax)):
        print("%-13s Bild %-4d | %s" % (tag, i, "  ".join(
            "%s %6.2f/%3d/%5d" % ((nm,) + stats(fr[i], y0)) for nm, y0 in ROWS)))
        if len(sys.argv) > 2:
            Image.fromarray(fr[i]).save(os.path.join(sys.argv[2], "port_%s.png" % tag.replace("-", "_").lower()))
if __name__ == "__main__":
    main()
