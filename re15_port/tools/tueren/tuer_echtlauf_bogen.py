#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuer_echtlauf_bogen.py - Ablaufbogen eines Echtlaufs (tuer_echtlauf.sh), Runde 31 Abschnitt 9.

Eine Zeile je Lauf: alter Raum (RE15_FRAMEDUMP vor dem Feuerbild) | Abdunkeln des stehenden Bildes
(RE15_TUER_SERIE a_abdunkeln_008/024) | Sequenz (b_tuer Anfang/Mitte/Ende) | Schwarz (c_ende) |
neuer Raum (RE15_FRAMEDUMP ab dem Feuerbild: Einblendung). Alle Bilder aus der echten exe.

    python re15_port/tools/tueren/tuer_echtlauf_bogen.py <ausgabe.png> <lauf-verzeichnis>:<feuerbild> ...
"""
import glob
import os
import sys

from PIL import Image, ImageDraw

W, H = 160, 120


def bild(p):
    im = Image.new("RGB", (W, H), (60, 0, 60))
    if p and os.path.exists(p):
        b = Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR)
        im.paste(b, (0, 0))
    return im


def zeile(verz, feuer):
    serie = os.path.join(verz, "serie")
    b = sorted(glob.glob(os.path.join(serie, "b_tuer_*.ppm")))
    n = len(b)
    zellen = [
        ("alt F%d" % (feuer - 6), os.path.join(verz, "f_%06d.ppm" % (feuer - 6))),
        ("alt F%d" % (feuer - 2), os.path.join(verz, "f_%06d.ppm" % (feuer - 2))),
        ("abdunkeln 8", os.path.join(serie, "a_abdunkeln_008.ppm")),
        ("abdunkeln 24", os.path.join(serie, "a_abdunkeln_024.ppm")),
        ("Tuer 20", os.path.join(serie, "b_tuer_020.ppm")),
        ("Tuer %d" % (n // 2), b[n // 2] if n else None),
        ("Tuer %d" % (n - 20), b[n - 20] if n > 20 else None),
        ("Ende 0", os.path.join(serie, "c_ende_000.ppm")),
        # neuer Raum: frame_count beginnt dort wieder bei 0 (s. tuer_echtlauf.sh)
        ("neu F0", os.path.join(verz, "f_%06d.ppm" % 0)),
        ("neu F4", os.path.join(verz, "f_%06d.ppm" % 4)),
        ("neu F8", os.path.join(verz, "f_%06d.ppm" % 8)),
        ("neu F40", os.path.join(verz, "f_%06d.ppm" % 40)),
    ]
    out = Image.new("RGB", (W * len(zellen), H + 30), (16, 16, 16))
    d = ImageDraw.Draw(out)
    d.text((4, 2), os.path.basename(verz.rstrip("/\\")), fill=(255, 230, 120))
    for i, (t, p) in enumerate(zellen):
        out.paste(bild(p), (i * W, 30))
        d.text((i * W + 4, 16), t, fill=(220, 220, 220))
    return out


def main():
    ziel = sys.argv[1]
    zeilen = []
    for arg in sys.argv[2:]:
        verz, feuer = arg.rsplit(":", 1)
        zeilen.append(zeile(verz, int(feuer)))
    bogen = Image.new("RGB", (max(z.width for z in zeilen), sum(z.height for z in zeilen)), (0, 0, 0))
    y = 0
    for z in zeilen:
        bogen.paste(z, (0, y))
        y += z.height
    bogen.save(ziel)
    print("geschrieben:", ziel)


if __name__ == "__main__":
    main()
