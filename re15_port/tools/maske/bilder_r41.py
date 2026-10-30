"""Runde 41 Spur M: Vorher/Nachher-Bilder der Stage-3..6-Masken + berichtigte Uebersichtsbilder.

Dossier analysis/befunde_runde41/M_masken36.md. Je Cut (Leon-Datei; die Elza-Datei ist bytegleich) ein Bogen:
  links  Hintergrund (probe_bg_dump)
  Mitte  VORHER: was der Lader bis Runde 40 zeichnete - alte Sektion durch den BSS-Atlas (Engine-Entpacker), ohne die
         Rechtecke der alten Ausschluss-Tabelle (magenta)
  rechts NACHHER: was der Lader jetzt zeichnet - neue Sektion durch den eigenen Atlas, ohne die Rechtecke der neuen
         Ausschluss-Tabelle (magenta = schon vorher, gruen = neu geschlossen, rot = vorher ja / jetzt nein,
         cyan = eingeschlossene Luecke, die offen bleibt)
Dazu je Cut ein Uebersichtsbild im Format von pri/STAGE3-6/<raum>/<cc>_port.png (a8be31a7: RGBA-Ausschnitt der
gezeichneten Maskenpunkte) - die alten waren mit dem fehlerhaften Entpacker gebaut (Dossier 1.1).

Aufruf: python re15_port/tools/maske/bilder_r41.py --ppm <probe_bg_dump> --alt <Ordner mit alten MSK + altem
        masken36_ausschluss_r37.inc> --out <Ordner>
"""
import argparse
import os
import re
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import abnahme       # noqa: E402
import maskenbild    # noqa: E402
import original      # noqa: E402

REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
MASKS = os.path.join(PSX, "MASKS")
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")


def ausschluss(pfad):
    t = open(pfad).read()
    out = {}
    for m in re.finditer(r"\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+), \{ 0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, "
                         r"0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u \} \}", t):
        raum, cut = int(m.group(1), 16), int(m.group(2))
        if raum:
            bits = [int(m.group(i), 16) for i in range(4, 8)]
            out[(raum, cut)] = set(i for i in range(128) if (bits[i >> 5] >> (i & 31)) & 1)
    return out


def deckung(blob, cut, idx, aus):
    ms = maskenbild.masken(blob, cut) or []
    ms = [m for i, m in enumerate(ms) if i not in aus]
    return abnahme.deckung_und_tiefe(ms, idx)[0], len(ms)


def farbe(idx, clut, blob, cut, aus):
    """RGBA-Bild der gezeichneten Punkte (Farbe aus dem Atlas, wie der Zeichner)."""
    c = clut.astype(np.int32)
    pal = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], 1).astype(np.uint8)
    out = np.zeros((240, 320, 4), np.uint8)
    for i, (sx, sy, X, Y, w, h, dep) in enumerate(maskenbild.masken(blob, cut) or []):
        if i in aus:
            continue
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        op = sub != 0
        blk = out[y0:y1, x0:x1]
        blk[op, :3] = pal[sub[op]]
        blk[op, 3] = 255
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ppm", required=True)
    ap.add_argument("--alt", required=True)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    aus_alt = ausschluss(os.path.join(a.alt, "masken36_ausschluss_r37.inc"))
    aus_neu = ausschluss(os.path.join(GEN, "masken36_ausschluss_r37.inc"))
    os.makedirs(a.out, exist_ok=True)
    summe = dict(vorher=0, nachher=0, neu=0, weg=0, offen=0, cuts=0)
    zeilen = []
    for f in sorted(os.listdir(MASKS)):
        if not re.match(r"ROOM[3-6][0-9A-F]{2}0\.MSK$", f):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        alt = open(os.path.join(a.alt, f), "rb").read()
        neu = open(os.path.join(MASKS, f), "rb").read()
        bss = original.atlas(room)
        _, nc = struct.unpack_from("<II", neu, 4)
        for cut in range(nc):
            if not (maskenbild.masken(neu, cut) or []):
                continue
            own = maskenbild.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
            bgp = os.path.join(a.ppm, "ROOM%03X%02d.ppm" % (rid >> 4, cut))
            bg = np.asarray(Image.open(bgp).convert("RGB"), np.uint8)
            d_alt, n_alt = deckung(alt, cut, bss[cut][0], aus_alt.get((rid, cut), set()))
            d_neu, n_neu = deckung(neu, cut, own[0], aus_neu.get((rid, cut), set()))
            zu = d_neu & ~d_alt
            weg = d_alt & ~d_neu
            offen = ndimage.binary_fill_holes(d_neu) & ~d_neu
            for k, v in (("vorher", d_alt.sum()), ("nachher", d_neu.sum()), ("neu", zu.sum()), ("weg", weg.sum()),
                         ("offen", offen.sum())):
                summe[k] += int(v)
            summe["cuts"] += 1
            zeilen.append("%s C%-2d vorher %6d px (%3d Rechtecke) | nachher %6d px (%3d Rechtecke) | neu %5d | weg %d | "
                          "eingeschlossen offen %d" % (room, cut, d_alt.sum(), n_alt, d_neu.sum(), n_neu, zu.sum(),
                                                       weg.sum(), offen.sum()))
            b = bg.astype(float) / 255

            def tint(m, c, basis):
                o = basis.copy()
                o[m] = o[m] * 0.35 + np.array(c) * 0.65
                return o
            v = tint(d_alt, (1, 0.1, 0.9), b)
            n = tint(d_neu & d_alt, (1, 0.1, 0.9), b)
            n = tint(zu, (0.1, 1, 0.2), n)
            n = tint(weg, (1, 0.2, 0.1), n)
            n = tint(offen, (0, 0.9, 1), n)
            sheet = Image.new("RGB", (320 * 3 + 16, 240 + 14), (20, 20, 20))
            for k, im in enumerate((b, v, n)):
                sheet.paste(Image.fromarray((im * 255).astype(np.uint8)), (k * 328, 14))
            ImageDraw.Draw(sheet).text((2, 1), "%s C%d | VORHER %d px | NACHHER %d px: +%d gruen, -%d rot, offen %d cyan"
                                       % (room, cut, d_alt.sum(), d_neu.sum(), zu.sum(), weg.sum(), offen.sum()),
                                       fill=(255, 255, 255))
            sheet.quantize(colors=256, method=Image.MEDIANCUT).save(os.path.join(a.out, "%s_C%02d.png" % (room, cut)),
                                                                     optimize=True)
            # Uebersichtsbild wie pri/STAGEn/<raum>/<cc>_port.png (Ausschnitt der gezeichneten Punkte, RGBA)
            rgba = farbe(own[0], own[1], neu, cut, aus_neu.get((rid, cut), set()))
            ys, xs = np.nonzero(rgba[:, :, 3])
            ud = os.path.join(a.out, "uebersicht", "STAGE%d" % (rid >> 12), "%03X0" % (rid >> 4))
            os.makedirs(ud, exist_ok=True)
            if len(xs):
                Image.fromarray(rgba[ys.min():ys.max() + 1, xs.min():xs.max() + 1]).save(
                    os.path.join(ud, "%02d_port.png" % cut), optimize=True)
            else:
                # Nachbesserung 1 (Auflage Nachtrag 11): Cut OHNE gezeichneten Maskenpunkt (alle Rechtecke ausgeschlossen, z.B.
                # Sektion ohne Stehplatz = Original-NULL-Sektion) -> volles Bild 320x240, ganz durchsichtig, fuer den Leon- UND
                # den Elza-Ordner (pri/STAGEn/<raum>/<cc>_port.png gibt es fuer beide; das alte Bild zeigte nie gezeichnete Masken).
                leer = Image.fromarray(np.zeros((240, 320, 4), np.uint8))
                for v in (0, 1):
                    uv = os.path.join(a.out, "uebersicht", "STAGE%d" % (rid >> 12), "%03X%d" % (rid >> 4, v))
                    os.makedirs(uv, exist_ok=True)
                    leer.save(os.path.join(uv, "%02d_port.png" % cut), optimize=True)
    with open(os.path.join(a.out, "zahlen.txt"), "w") as fp:
        fp.write("\n".join(zeilen) + "\n")
        fp.write("SUMME %d Cuts: vorher %d px, nachher %d px, neu %d, weg %d, eingeschlossen offen %d\n" % (
            summe["cuts"], summe["vorher"], summe["nachher"], summe["neu"], summe["weg"], summe["offen"]))
    print("SUMME", summe)


if __name__ == "__main__":
    main()
