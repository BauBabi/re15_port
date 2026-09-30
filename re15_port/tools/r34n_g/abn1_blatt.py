"""Abnahme 1 Spur G2 — Belegblatt aus RE15_FRAMEDUMP-Bildern: oben zwei (oder mehr) Vollbilder
(auf 320x240 verkleinert, Schrift-Rechteck gruen umrandet), darunter ein Streifen vergroesserter
Ausschnitte x131..224 y17..48 je Bild mit Bildnummer.

    C:/Python310/python.exe re15_port/tools/r34n_g/abn1_blatt.py <aus.png> <titel> <praefix> <voll:F,F,..> <streifen:F,F,..>
"""
import sys

from PIL import Image, ImageDraw

X0, Y0, X1, Y1 = 139, 25, 216, 40


def lade(praefix, f):
    im = Image.open("%s%06d.ppm" % (praefix, f)).convert("RGB")
    return im.resize((320, 240), Image.NEAREST)


def main():
    out, titel, praefix = sys.argv[1], sys.argv[2], sys.argv[3]
    voll = [int(x) for x in sys.argv[4].split(",") if x]
    streifen = [int(x) for x in sys.argv[5].split(",") if x]
    cw, ch = (X1 - X0 + 17) * 2, (Y1 - Y0 + 17) * 2
    per_row = max(1, (len(voll) * 330) // (cw + 6))
    rows = (len(streifen) + per_row - 1) // per_row
    W = max(len(voll) * 330, per_row * (cw + 6)) + 10
    H = 20 + 255 + rows * (ch + 18) + 10
    sheet = Image.new("RGB", (W, H), (24, 24, 24))
    d = ImageDraw.Draw(sheet)
    d.text((6, 4), titel, fill=(255, 255, 255))
    for i, f in enumerate(voll):
        im = lade(praefix, f)
        dd = ImageDraw.Draw(im)
        dd.rectangle((X0 - 1, Y0 - 1, X1 + 1, Y1 + 1), outline=(0, 255, 0))
        sheet.paste(im, (6 + i * 330, 20))
        d.text((8 + i * 330, 22), "F%d" % f, fill=(255, 255, 0))
    for j, f in enumerate(streifen):
        im = lade(praefix, f).crop((X0 - 8, Y0 - 8, X1 + 9, Y1 + 9)).resize((cw, ch), Image.NEAREST)
        x = 6 + (j % per_row) * (cw + 6)
        y = 20 + 255 + (j // per_row) * (ch + 18)
        d.text((x, y), "F%d" % f, fill=(255, 255, 0))
        sheet.paste(im, (x, y + 12))
    sheet.save(out, optimize=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
