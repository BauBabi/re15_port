"""Messung 1: Gibt ROOM1050_PRI01.PBM die Freistellung des Nutzers BITGENAU wieder?
Kein Modell, kein Raten: PBM lesen, PNG mit der Auswahl-Lage setzen, XOR zaehlen.
Zusaetzlich: welche Alphaschwelle traefe die PBM genau?"""
import os
import sys
import numpy as np
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
PBM = os.path.join(ROOT, "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.PBM")
PNG = os.path.join(ROOT, "pri/STAGE1/1050/01.png")


def lies_pbm(p):
    b = open(p, "rb").read()
    assert b[:3] == b"P4\n", b[:8]
    i = b.index(b"\n", 3) + 1
    kopf = b[3:i].strip().split()
    w, h = int(kopf[0]), int(kopf[1])
    bits = np.unpackbits(np.frombuffer(b[i:], np.uint8).reshape(h, -1), axis=1)[:, :w]
    return bits.astype(bool)


soll = lies_pbm(PBM)
print("PBM: %d Punkte, Kasten x%d..%d y%d..%d"
      % (soll.sum(), np.nonzero(soll.any(0))[0].min(), np.nonzero(soll.any(0))[0].max(),
         np.nonzero(soll.any(1))[0].min(), np.nonzero(soll.any(1))[0].max()))

im = Image.open(PNG).convert("RGBA")
a = np.asarray(im)
print("PNG: %dx%d" % (im.width, im.height))
X, Y = 179, 139

for schw in (0, 1, 63, 110, 127, 200, 254):
    m = a[:, :, 3] > schw
    r = np.zeros((240, 320), bool)
    h, w = m.shape
    r[Y:Y + h, X:X + w] = m
    fehlt = int((r & ~soll).sum())
    zuviel = int((soll & ~r).sum())
    print("  Alpha > %3d : %5d Punkte | PNG-ohne-PBM %4d | PBM-ohne-PNG %4d"
          % (schw, m.sum(), fehlt, zuviel))
