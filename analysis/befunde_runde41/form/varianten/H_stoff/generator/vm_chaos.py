#!/usr/bin/env python3
"""Varianten, Mass fuer "chaotisch" im sauberen Render, alle Modelle im SELBEN Renderer (form_pruefung.rundum_dreiecke, Blattlicht, gleiche Groesse
auf dem Schirm, Hoehe der Figur rund 300 px wie Bild 1), vier Ansichten. python vm_chaos.py AUS.json "Name=form.json" ... [--re2]. Kein RE-Wert.

 - HF: Hochfrequenzenergie der Helligkeit in der Figur (2 px vom Rand), mittleres (Y - Gauss(Y, 1,0))^2 geteilt durch mittleres Y^2 (relativ, damit
   dunkle und helle Texturen vergleichbar sind).
 - Inseln: Anteil der Figurpixel in kleinen isolierten Farbinseln: Pixel, deren Farbe vom Median der 5x5-Umgebung um mehr als dE 10 (Lab) abweicht,
   zusammenhaengend; Inseln bis 6 px zaehlen (bei Spielgroesse wirkt das als Rauschen).
RE2-Birkin aus CDEMD0.EMS (G2 0x31, G3 0x33, G4 0x34), Clip 0 Bild 0, nur zum Vergleich des Handwerks."""
import json
import os
import re
import sys

import cv2
import numpy as np
from skimage.color import rgb2lab

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_buehne as FB       # noqa: E402
import form_emd as FE          # noqa: E402

ANSICHTEN = ((-135, 22), (45, 22), (180, 10), (0, 10))     # 3/4 hinten links, 3/4 vorn rechts, Seiten
GROSS = 330


def re2_modell(kind):
    import birkin_bahnsteig as BB
    W = os.path.normpath(os.path.join(TOOLS, '..', '..'))
    inc = open(os.path.join(W, 're15_port/engine/src/gen/re2_ems_toc.inc'), encoding='utf-8', errors='replace').read()
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
    ems = open(os.path.join(W, 're15_port/shared_assets/RE2/CDEMD0.EMS'), 'rb').read()

    def rec(r):
        i = ((kind - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
    BB.KIND = kind
    emd, tim = rec(3), rec(2)
    mo = BB.modell(emd, tim)
    return mo, FB.Textur(tim), mo.frames[mo.clips[0][0]] & 0xFFF


def bilder(mo, tex, kf):
    aus = []
    for az, el in ANSICHTEN:
        a = np.full((GROSS, GROSS, 3), 0, np.uint8)
        m = FB.rastern(P.rundum_dreiecke(mo, kf, az, el, GROSS), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
        aus.append((a, m))
    return aus


def masse(a, m):
    inn = cv2.erode(m.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    Y = a.astype(np.float32) @ np.array([.299, .587, .114], np.float32)
    hf = float(((Y - cv2.GaussianBlur(Y, (0, 0), 1.0)) ** 2)[inn].mean() / max((Y[inn] ** 2).mean(), 1e-6))
    lab = rgb2lab(a.astype(np.float32) / 255.0)
    med = np.stack([cv2.medianBlur(np.ascontiguousarray(lab[..., c]).astype(np.float32), 5) for c in range(3)], -1)
    aus_ = (np.linalg.norm(lab - med, axis=2) > 10) & inn
    n, lab_, st, _ = cv2.connectedComponentsWithStats(aus_.astype(np.uint8), connectivity=8)
    klein = 0
    for k in range(1, n):
        if st[k, cv2.CC_STAT_AREA] <= 6:
            klein += st[k, cv2.CC_STAT_AREA]
    return hf, klein / max(inn.sum(), 1)


def main():
    ziel = sys.argv[1]
    R = {}
    zeilen = [z for z in sys.argv[2:] if z != '--re2']
    for z in zeilen:
        nm, pf = z.split('=', 1)
        k = P.Kandidat(pf)
        r = k.j.get('ruhe', {})
        R[nm] = bilder(k.mo, k.tex, k.kf(int(r.get('clip', 2)), int(r.get('bild', 0))))
    if '--re2' in sys.argv:
        for nm, kind in (('RE2 G2 (0x31)', 0x31), ('RE2 G3 (0x33)', 0x33), ('RE2 G4 (0x34)', 0x34)):
            R[nm] = bilder(*re2_modell(kind))
    out = {}
    for nm, bl in R.items():
        ms = [masse(a, m) for a, m in bl]
        out[nm] = dict(hf=round(float(np.mean([x[0] for x in ms])), 4), inseln=round(float(np.mean([x[1] for x in ms])), 4))
        print('%-18s HF %.4f  kleine Inseln %.4f' % (nm, out[nm]['hf'], out[nm]['inseln']))
    json.dump(out, open(ziel, 'w', encoding='utf-8'), indent=1)
    reihe = [np.hstack([a for a, _ in bl]) for bl in R.values()]
    cv2.imwrite(ziel.replace('.json', '.png'), np.vstack(reihe)[:, :, ::-1])


if __name__ == '__main__':
    main()
