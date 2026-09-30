#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht) - wertet eine Savestate-Serie (ds_sign_series.py) aus.

Je Savestate:
  * Vcount (libetc-VBlank-Zaehler, VSync(-1) liefert lw 0x800787dc @0x80062004)
  * Raum/Cut (DAT_800b0fe2 / DAT_800b0fe4, FUN_80021bbc)
  * Schrift "MAGAZINE CLUB" in BEIDEN Bildspeichern (VRAM y=0 und y=240, RECT @0x80072f2c)
    und in der RAM-Hintergrundkopie 0x80198000 (StoreImage @0x80021e44): mittlere Helligkeit
    + Anzahl "weisser" Schriftpunkte im Schild-Rechteck des Cuts.
  * optional: PNG-Ausschnitt je Bild (--png <dir>)

  C:/Python310/python.exe re15_port/tools/r34n_g/ds_sign_eval.py <dir-mit-cap_*.sav> [--png dir]
"""
import argparse, glob, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

# Schild-Rechtecke je Cut, gemessen an der BSS-Dekodierung (probe_r34n_g_bss, Blau-Maske
# b>150, b>r+60, b>g+40): Cut 2 x285..312 y50..63, Cut 3 x292..319 y6..21, Cut 10 x262..287 y0..10.
SIGN = {2: (283, 48, 315, 66), 3: (290, 4, 320, 24), 10: (260, 0, 290, 12)}


def rgb(v):
    return ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3)


def stats(get, box):
    x0, y0, x1, y1 = box
    n = 0; s = 0; white = 0; blue = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = rgb(get(x, y))
            s += r + g + b; n += 1
            if r > 150 and g > 150 and b > 150:
                white += 1
            if b > 150 and b > r + 60:
                blue += 1
    return s / (3.0 * n), white, blue


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dir")
    ap.add_argument("--png", default="")
    a = ap.parse_args()
    fs = sorted(glob.glob(os.path.join(a.dir, "cap_*.sav")))
    prev = None
    for f in fs:
        r = re15_ss.Ram(f)
        vc = r.u32(0x800787dc)
        room = r.u16(0x800b0fe2); cut = r.u16(0x800b0fe4)
        box = SIGN.get(cut)
        line = "%s Vcount=%8d dV=%5s room=%02x cut=%2d aca34=%d" % (
            os.path.basename(f), vc, "" if prev is None else str(vc - prev), room, cut,
            r.u8(0x800aca34))
        prev = vc
        if box:
            ram = r.bytes(0x80198000, 320 * 240 * 2)
            ramget = lambda x, y: ram[(y * 320 + x) * 2] | (ram[(y * 320 + x) * 2 + 1] << 8)
            for yb in (0, 240):
                m, w, b = stats(lambda x, y: r.vpix(x, yb + y), box)
                line += " | fb%d L=%5.1f w=%3d b=%3d" % (yb, m, w, b)
            m, w, b = stats(ramget, box)
            line += " | ram L=%5.1f w=%3d b=%3d" % (m, w, b)
        print(line)
        if a.png:
            from PIL import Image
            os.makedirs(a.png, exist_ok=True)
            im = Image.new("RGB", (640, 240))
            p = im.load()
            for yb, xo in ((0, 0), (240, 320)):
                for y in range(240):
                    for x in range(320):
                        p[xo + x, y] = rgb(r.vpix(x, yb + y))
            im.save(os.path.join(a.png, os.path.basename(f).replace(".sav", ".png")))


if __name__ == "__main__":
    main()
