"""Nachbesserung 1: Bildleisten vorher/nachher aus Laeufen im echten Fenster (s3_n1_spiel.sh, abnahme1/werkzeug/lauf.sh).

    python s3_n1_leiste.py AUS.png "TITEL" "ZEILE|LAUFDIR|i1,i2,...[|beschriftung1;beschriftung2;...]" ...

LAUFDIR enthaelt bild/b_NNNN.png (gdigrab). Je Zeile die angegebenen Bilder nebeneinander (320 x 240), Zeilentitel links darueber.
Kein Messwert, nur Zusammenstellung."""
import os, sys
from PIL import Image, ImageDraw, ImageFont


def font(n):
    try:
        return ImageFont.truetype('C:/Windows/Fonts/segoeui.ttf', n)
    except OSError:
        return ImageFont.load_default()


def main():
    aus, titel, zeilen = sys.argv[1], sys.argv[2], sys.argv[3:]
    bw, bh = 320, 240
    reihen = []
    for z in zeilen:
        teile = z.split('|')
        name, d, idx = teile[0], teile[1], [int(x) for x in teile[2].split(',') if x]
        lab = teile[3].split(';') if len(teile) > 3 else []
        ims = []
        for k, i in enumerate(idx):
            p = os.path.join(d, 'bild', 'b_%04d.png' % i)
            im = Image.open(p).convert('RGB').resize((bw, bh)) if os.path.exists(p) else Image.new('RGB', (bw, bh), (60, 0, 0))
            ims.append((im, lab[k] if k < len(lab) else 'Bild %d' % i))
        reihen.append((name, ims))
    sp = max(len(r[1]) for r in reihen)
    H = 30 + sum(22 + bh + 18 for _ in reihen)
    out = Image.new('RGB', (sp * (bw + 4) + 4, H), (20, 20, 24))
    dr = ImageDraw.Draw(out)
    dr.text((6, 4), titel, fill=(230, 230, 230), font=font(16))
    y = 30
    for name, ims in reihen:
        dr.text((6, y), name, fill=(140, 190, 240), font=font(14))
        y += 22
        for k, (im, lab) in enumerate(ims):
            x = 4 + k * (bw + 4)
            out.paste(im, (x, y))
            dr.text((x + 3, y + bh + 1), lab, fill=(240, 200, 120), font=font(12))
        y += bh + 18
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    out.save(aus)
    print(aus, out.size)


if __name__ == '__main__':
    main()
