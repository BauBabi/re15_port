"""Exportiert die fertige Maske eines Raums als PNG — genau das, was der Port zeichnet.

Ausgabe je Cut in masken_vorschau/<RAUM>/:
  <RAUM>_cutNN.png       320x240 RGBA — nur die Maskenpixel, alles andere DURCHSICHTIG.
                         Das ist woertlich die Freistellung, die im Spiel ueber die
                         Figur gemalt wird.
  <RAUM>_cutNN_x3.png    dieselbe Freistellung dreifach vergroessert (passt zum
                         Massstab eines 960x720-Bildschirmfotos) — zum Nachbessern
                         mit dem Lasso und Zurueckgeben.
  <RAUM>_cutNN_kontext.png  die Freistellung ueber dem Hintergrund, zum Beurteilen.

Aufruf:  python re15_port/tools/maske/vorschau.py ROOM1140
"""
import argparse
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg, load_rdt
import maskenbild as MB

CD = "re15_port/shared_assets/PSX"
MASKS = os.path.join(CD, "MASKS")


def freistellung(room, cut):
    """-> (rgba 240x320x4 uint8, anzahl_rechtecke) oder (None, 0)."""
    p = os.path.join(MASKS, "%s.MSK" % room)
    if not os.path.exists(p):
        return None, 0
    ms = MB.masken(open(p, "rb").read(), cut)
    if not ms:
        return None, 0
    t = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
    if not t:
        return None, 0
    idx, clut = t
    c = clut.astype(np.uint32)
    pal = np.stack([((c & 0x1F) << 3), (((c >> 5) & 0x1F) << 3), (((c >> 10) & 0x1F) << 3)], 1).astype(np.uint8)
    out = np.zeros((240, 320, 4), np.uint8)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if sub.shape != (y1 - y0, x1 - x0):
            continue
        op = sub != 0
        ziel = out[y0:y1, x0:x1]
        ziel[..., :3] = np.where(op[..., None], pal[sub], ziel[..., :3])
        ziel[..., 3] = np.where(op, 255, ziel[..., 3])
    return out, len(ms)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--out", default="masken_vorschau")
    a = ap.parse_args()
    room = a.room.upper()
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(CD, room)
    ziel = os.path.join(a.out, room)
    os.makedirs(ziel, exist_ok=True)
    cuts = [a.cut] if a.cut is not None else range(rdt[1] if rdt else 16)
    n = 0
    for cut in cuts:
        rgba, nr = freistellung(room, cut)
        if rgba is None:
            continue
        im = Image.fromarray(rgba, "RGBA")
        im.save(os.path.join(ziel, "%s_cut%02d.png" % (room, cut)))
        im.resize((960, 720), Image.NEAREST).save(
            os.path.join(ziel, "%s_cut%02d_x3.png" % (room, cut)))
        bg = load_bg("build/bg_ppm", rid, cut)
        if bg is not None:
            hg = Image.fromarray(bg.astype(np.uint8), "RGB").convert("RGBA")
            hg.alpha_composite(im)
            hg.resize((960, 720), Image.NEAREST).save(
                os.path.join(ziel, "%s_cut%02d_kontext.png" % (room, cut)))
        deckung = float((rgba[..., 3] > 0).mean())
        print("  %s Cut %2d: %5.1f %% des Bildes, %3d Rechtecke" % (room, cut, 100 * deckung, nr))
        n += 1
    print("%d Cuts nach %s/ geschrieben" % (n, ziel))


if __name__ == "__main__":
    main()
