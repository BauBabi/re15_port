#!/usr/bin/env python3
"""Stichprobe (Fortsetzungs-Agent): das grosse Item-Bild. RE1.5 ITEM/ITPS.ITP Block 0x40/0x41
gegen ALLE Bloecke von RE2 COMMON/DATA/ITPS.ITP (Blocklaenge beidseitig 0x3000).

Verglichen werden die FARBEN (CLUT aufgeloest), nicht die Indexbytes — zwei Bloecke koennen
dasselbe Bild mit verschieden sortierter Palette tragen.

    python analysis/befunde_runde30/sicherung_werkzeug/re2_itps_gleichheit.py
"""
import os
import struct

import numpy as np
from PIL import Image

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
A = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "ITEM", "ITPS.ITP"), "rb").read()
B = open(os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ITPS.ITP"), "rb").read()
ZIEL = os.path.join(REPO, "build", "r30_sicherung")
S = 0x3000


def bild(d, k):
    b = d[k * S:(k + 1) * S]
    if struct.unpack_from("<I", b, 0)[0] != 0x10:
        return None
    clut = np.array(struct.unpack_from("<256H", b, 0x14), np.uint16)
    idx = np.frombuffer(b, np.uint8, 112 * 72, 0x220).reshape(72, 112)
    return clut[idx] & 0x7FFF, idx, b


print("RE1.5 ITPS.ITP %d B = %d Bloecke | RE2 ITPS.ITP %d B = %d Bloecke"
      % (len(A), len(A) // S, len(B), len(B) // S))
for t in (0x3F, 0x40, 0x41):
    a = bild(A, t)
    best = None
    for k in range(len(B) // S):
        b = bild(B, k)
        if b is None:
            continue
        g = int((a[0] == b[0]).sum())
        gi = int((a[1] == b[1]).sum())
        if best is None or g > best[0]:
            best = (g, k, gi, a[2] == b[2])
    print("RE1.5 Block 0x%02X @Datei 0x%05X: bestes RE2-Bild = Block %d (0x%02X) @Datei 0x%06X, "
          "%d von %d Farbworten gleich, %d Indexbytes gleich, Block bytegleich: %s"
          % (t, t * S, best[1], best[1], best[1] * S, best[0], 112 * 72, best[2], best[3]))
    if t == 0x40:
        b = bild(B, best[1])
        w = b[0]
        rgb = np.stack([(w & 31) << 3, ((w >> 5) & 31) << 3, ((w >> 10) & 31) << 3], -1).astype(np.uint8)
        Image.fromarray(rgb).resize((448, 288), Image.NEAREST).save(
            os.path.join(ZIEL, "re2_itps_%02X_4x.png" % best[1]))
