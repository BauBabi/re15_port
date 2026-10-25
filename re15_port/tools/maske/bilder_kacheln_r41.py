"""Runde 41 Spur M, Nachbesserung 3: Vorher/Nachher-Boegen und Uebersichtsbilder der durch kacheln_r41.py geaenderten Cuts.

Dossier analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 3". Je geaendertem Cut (Leon-Datei; Elza bytegleich):
  Bogen M_bilder/NB3/NB3_bogen_ROOMxxxx_Cnn.png: Hintergrund | VORHER (Stand vor NB3: MSK + eigener Atlas + Ausschluss-Tabelle)
  gezeichnete Punkte magenta | NACHHER magenta = schon vorher, gruen = neu gezeichnet, rot = vorher ja / jetzt nein, darunter die
  Teilrechtecke (Umriss, Farbe nach Tiefe) - und das Uebersichtsbild M_bilder/uebersicht/STAGEn/<raum>/<cc>_port.png (RGBA der
  gezeichneten Punkte, Format wie pri/STAGE3-6/<raum>/<cc>_port.png, a8be31a7; wie bilder_r41.py).

Aufruf: python re15_port/tools/maske/bilder_kacheln_r41.py --ppm <probe_bg_dump> --alt <Ordner MSK/TIM vorher> --alt-aus <inc vorher>
        --out <analysis/befunde_runde41/M_bilder> [ROOMxxxx ...]
"""
import argparse
import os
import re
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import bilder_r41 as BR    # noqa: E402
import maskenbild          # noqa: E402

MASKS = BR.MASKS
GEN = BR.GEN


def deckung(msk_dir, room, cut, aus):
    blob = open(os.path.join(msk_dir, room + ".MSK"), "rb").read()
    tim = maskenbild.lies_tim(os.path.join(msk_dir, "%s_PRI%02d.TIM" % (room, cut)))
    ms = maskenbild.masken(blob, cut) or []
    op = tim[0] != 0
    d = np.zeros((240, 320), bool)
    for i, (sx, sy, X, Y, w, h, dep) in enumerate(ms):
        if i in aus:
            continue
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 > x0 and y1 > y0:
            d[y0:y1, x0:x1] |= op[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
    return d, blob, tim, ms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--ppm", required=True)
    ap.add_argument("--alt", required=True)
    ap.add_argument("--alt-aus", required=True)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    aus_alt = BR.ausschluss(a.alt_aus)
    aus_neu = BR.ausschluss(os.path.join(GEN, "masken36_ausschluss_r37.inc"))
    os.makedirs(os.path.join(a.out, "NB3"), exist_ok=True)
    summe = dict(vorher=0, nachher=0, neu=0, weg=0, cuts=0)
    zeilen = []
    for f in sorted(os.listdir(MASKS)):
        if not re.match(r"ROOM[3-6][0-9A-F]{2}0\.MSK$", f):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if a.raeume and room not in a.raeume:
            continue
        alt_blob = open(os.path.join(a.alt, f), "rb").read()
        neu_blob = open(os.path.join(MASKS, f), "rb").read()
        nc = int.from_bytes(neu_blob[8:12], "little")
        for cut in range(nc):
            if not (maskenbild.masken(neu_blob, cut) or []):
                continue
            tn = os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut))
            ta = os.path.join(a.alt, "%s_PRI%02d.TIM" % (room, cut))
            if not os.path.exists(tn) or not os.path.exists(ta):
                continue
            d_alt, _, _, _ = deckung(a.alt, room, cut, aus_alt.get((rid, cut), set()))
            d_neu, _, tim, ms = deckung(MASKS, room, cut, aus_neu.get((rid, cut), set()))
            sec_alt = maskenbild.masken(alt_blob, cut) or []
            if (d_alt == d_neu).all() and [m[6] for m in sec_alt] == [m[6] for m in ms] and len(sec_alt) == len(ms) \
                    and open(ta, "rb").read() == open(tn, "rb").read():
                continue                                       # Cut unveraendert
            zu, weg = d_neu & ~d_alt, d_alt & ~d_neu
            summe["vorher"] += int(d_alt.sum()); summe["nachher"] += int(d_neu.sum())
            summe["neu"] += int(zu.sum()); summe["weg"] += int(weg.sum()); summe["cuts"] += 1
            zeilen.append("%s C%-2d vorher %6d px (%3d Rechtecke) | nachher %6d px (%3d Rechtecke) | neu %5d | weg %d" % (
                room, cut, d_alt.sum(), len(sec_alt), d_neu.sum(), len(ms), zu.sum(), weg.sum()))
            bg = np.asarray(Image.open(os.path.join(a.ppm, "ROOM%03X%02d.ppm" % (rid >> 4, cut))).convert("RGB"), np.uint8)
            b = bg.astype(float) / 255

            def tint(m, c, basis):
                o = basis.copy()
                o[m] = o[m] * 0.35 + np.array(c) * 0.65
                return o
            v = tint(d_alt, (1, 0.1, 0.9), b)
            n = tint(d_neu & d_alt, (1, 0.1, 0.9), b)
            n = tint(zu, (0.1, 1, 0.2), n)
            n = tint(weg, (1, 0.2, 0.1), n)
            sheet = Image.new("RGB", (320 * 3 + 16, 240 + 16), (20, 20, 20))
            for k, im in enumerate((b, v, n)):
                sheet.paste(Image.fromarray((im * 255).astype(np.uint8)), (k * 328, 14))
            # Teilrechtecke: Umriss, Farbe nach Tiefe (blau nah .. rot fern)
            unten = Image.fromarray((b * 255 * 0.6).astype(np.uint8)).resize((640, 480), Image.NEAREST)
            du = ImageDraw.Draw(unten)
            aus = aus_neu.get((rid, cut), set())
            deps = [m[6] for i, m in enumerate(ms) if i not in aus] or [0]
            lo, hi = min(deps), max(deps) + 1e-9
            for i, (sx, sy, X, Y, w, h, dep) in enumerate(ms):
                if i in aus:
                    continue
                q = (dep - lo) / (hi - lo)
                du.rectangle([X * 2, Y * 2, (X + w) * 2 - 1, (Y + h) * 2 - 1], outline=(int(255 * q), 80, int(255 * (1 - q))))
            sheet2 = Image.new("RGB", (max(sheet.width, 640), sheet.height + 480), (20, 20, 20))
            sheet2.paste(sheet, (0, 0))
            sheet2.paste(unten, (0, sheet.height))
            ImageDraw.Draw(sheet2).text((2, 1), "%s C%d | VORHER (NB2) %d px | NACHHER (NB3) %d px: +%d gruen, -%d rot | unten: gezeichnete "
                                        "Rechtecke, Farbe = Tiefe %d (blau) .. %d (rot)" % (room, cut, d_alt.sum(), d_neu.sum(), zu.sum(),
                                                                                           weg.sum(), lo, int(hi)), fill=(255, 255, 255))
            sheet2.quantize(colors=256, method=Image.MEDIANCUT).save(os.path.join(a.out, "NB3", "NB3_bogen_%s_C%02d.png" % (room, cut)),
                                                                     optimize=True)
            rgba = BR.farbe(tim[0], tim[1], neu_blob, cut, aus_neu.get((rid, cut), set()))
            ys, xs = np.nonzero(rgba[:, :, 3])
            for var in (0, 1):
                ud = os.path.join(a.out, "uebersicht", "STAGE%d" % (rid >> 12), "%03X%d" % (rid >> 4, var))
                if var == 1 and not os.path.isdir(ud):
                    continue                                   # Elza-Ordner nur, wo es ihn schon gibt (bytegleiche MSK)
                os.makedirs(ud, exist_ok=True)
                if len(xs):
                    Image.fromarray(rgba[ys.min():ys.max() + 1, xs.min():xs.max() + 1]).save(os.path.join(ud, "%02d_port.png" % cut),
                                                                                           optimize=True)
                else:
                    Image.fromarray(np.zeros((240, 320, 4), np.uint8)).save(os.path.join(ud, "%02d_port.png" % cut), optimize=True)
    with open(os.path.join(a.out, "NB3", "NB3_zahlen.txt"), "w") as fp:
        fp.write("\n".join(zeilen) + "\n")
        fp.write("SUMME %d geaenderte Cuts: vorher %d px, nachher %d px, neu %d, weg %d\n" % (
            summe["cuts"], summe["vorher"], summe["nachher"], summe["neu"], summe["weg"]))
    print("SUMME", summe)


if __name__ == "__main__":
    main()
