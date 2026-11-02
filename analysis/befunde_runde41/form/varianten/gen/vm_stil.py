#!/usr/bin/env python3
"""Varianten, Blatt "Stil" (E15): dieselben Ansichten der Form neben RE2-Birkin und RE1.5-Zombie, selber Renderer (form_pruefung.rundum_dreiecke,
Blattlicht, Textur, Gouraud), gleiche Bildgroesse. python vm_stil.py AUS.png "Name=form.json" ... Kein RE-Wert."""
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_buehne as FB       # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
import vm_gezeichnet as VG     # noqa: E402

ANSICHTEN = ((-135, 22), (45, 22), (0, 10))
GROSS = 380


def bilder(mo, tex, kf):
    aus = []
    for az, el in ANSICHTEN:
        a = np.full((GROSS, GROSS, 3), 64, np.uint8)
        FB.rastern(P.rundum_dreiecke(mo, kf, az, el, GROSS), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        aus.append(a)
    return aus


def main():
    ziel = sys.argv[1]
    reihen = []
    for z in sys.argv[2:]:
        nm, pf = z.split('=', 1)
        k = P.Kandidat(pf)
        r = k.j.get('ruhe', {})
        reihen.append((nm + ' (Ruhepose)', bilder(k.mo, k.tex, k.kf(int(r.get('clip', 2)), int(r.get('bild', 0))))))
    mo, tim = VG.re2(0x31)
    reihen.append(('RE2 Birkin G2 (CDEMD0 0x31), Clip 0', bilder(mo, FB.Textur(tim), mo.frames[mo.clips[0][0]] & 0xFFF)))
    mo, tim = VG.re2(0x34)
    reihen.append(('RE2 Birkin G4 (CDEMD0 0x34), Clip 0', bilder(mo, FB.Textur(tim), mo.frames[mo.clips[0][0]] & 0xFFF)))
    z = EA.Modell(0x10)
    d = EA.emd_dir(z.blob)
    reihen.append(('RE1.5 Zombie (PSX/EMD/CDEMD0.EMS 0x10), Clip 0', bilder(z, FB.Textur(z.blob[d[8]:]), z.frames[z.clips[0][0]] & 0xFFF)))
    pad, kopf = 10, 24
    W = pad + len(ANSICHTEN) * (GROSS + pad)
    H = 36 + len(reihen) * (GROSS + kopf + pad)
    b = Image.new('RGB', (W, H), (18, 19, 22))
    dr = ImageDraw.Draw(b)
    dr.text((pad, 8), 'Stil: selber Renderer (Textur, Gouraud, Blattlicht), gleiche Bildgroesse, Ansichten 3/4 hinten links, 3/4 vorn rechts, Seite',
            font=EA.font(15, True), fill=(240, 200, 90))
    y = 36
    for t, bl in reihen:
        dr.text((pad, y), t, font=EA.font(15, True), fill=(214, 138, 74))
        for j, a in enumerate(bl):
            b.paste(Image.fromarray(a), (pad + j * (GROSS + pad), y + kopf))
        y += GROSS + kopf + pad
    b.save(ziel)


if __name__ == '__main__':
    main()
