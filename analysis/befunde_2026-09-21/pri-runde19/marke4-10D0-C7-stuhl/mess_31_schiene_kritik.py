# -*- coding: utf-8 -*-
"""Messung 31: Warum steigt HINTfrei von 2 auf 36, obwohl der Fix richtig ist?

abnahme.standlinie nimmt je BILDSPALTE die NAECHSTE Standlinie ueber alle Objekte.
Traegt eine Spalte Punkte von zwei Gegenstaenden in verschiedener Entfernung, dann
vergleicht die Schiene den Standplatz mit der Linie des NAHEN Gegenstands, verlangt
aber Verdeckung auch durch die Punkte des FERNEN. Ein Standplatz ZWISCHEN beiden
wird damit zwangslaeufig als "HINTER, aber frei" gemeldet - obwohl genau das richtig ist.

Hier: fuer jeden neu gemeldeten Standplatz der Vergleich mit der Standlinie des
KASTENS selbst.
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402
import geometrie                                # noqa: E402

rdt, _ = geom.load_rdt(os.path.join("re15_port", "shared_assets", "PSX"), "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)

# Standlinie des Kastens: tiefster Punkt seiner Silhouette, aufrecht -> ein Standpunkt
a = np.asarray(Image.open("pri/STAGE1/10D0/07_03.png").convert("RGBA"), int)
K = np.zeros((240, 320), bool)
K[126:126 + a.shape[0], 46:46 + a.shape[1]] = a[..., 3] >= 110
ys, xs = np.nonzero(K)
cx = float(xs.mean()) + 0.5
sy = float(ys.max()) + 0.5
vz_kasten = geom.vz_at_floor(R, t, H, cx, sy, 0)
print("Kasten: Standpunkt Bild(%.1f, %.1f) -> Kamera-z %.0f (Tiefe %d), Bucketbreite %.4f"
      % (cx, sy, vz_kasten, int(1023 * vz_kasten / 65536.0), geometrie.BUCKET))
# Standlinie des Stuhls
b = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
S = np.zeros((240, 320), bool)
S[123:123 + b.shape[0], 0:b.shape[1]] = b[..., 3] >= 110
sys_, sxs = np.nonzero(S)
vz_stuhl = geom.vz_at_floor(R, t, H, float(sxs.mean()) + 0.5, float(sys_.max()) + 0.5, 0)
print("Stuhl : Kamera-z %.0f (Tiefe %d)" % (vz_stuhl, int(1023 * vz_stuhl / 65536.0)))

REST = [(-1500, 24950, 0.00), (-900, 25550, 0.78), (-900, 25750, 0.76), (-700, 25550, 0.73),
        (-700, 25750, 0.65), (-500, 25750, 0.60), (-300, 25750, 0.86), (500, 26350, 0.00),
        (500, 26550, 0.13), (500, 26750, 0.52), (500, 26950, 0.67), (500, 27150, 0.74)]
print()
print("Standplatz        | Kamera-z | vs Stuhl 3985 | vs Kasten %.0f | Urteil" % vz_kasten)
for (wx, wz, q) in REST:
    pf = abnahme.proj(R, t, H, wx, 0, wz)
    if pf is None:
        continue
    vz = pf[2]
    lage_s = "HINTER" if vz > vz_stuhl + geometrie.BUCKET else ("VOR" if vz < vz_stuhl - geometrie.BUCKET else "gleich")
    lage_k = "HINTER" if vz > vz_kasten + geometrie.BUCKET else ("VOR" if vz < vz_kasten - geometrie.BUCKET else "gleich")
    print("(%6d,%6d) q=%.2f | %7.0f | %-6s | %-6s | %s"
          % (wx, wz, q, vz, lage_s, lage_k,
             "Kasten DARF nicht verdecken" if lage_k == "VOR" else "Kasten soll verdecken"))
