"""Enger Ausschnitt um die Stelle, an der der Saum den Spieler beruehrt: Bildspalten
55..75 / Zeilen 123..200 (320x240-Koordinaten), 18-fach, VORHER | NACHHER | Hintergrund.
Der Hintergrund als dritte Spalte zeigt, was dort GEMALT ist — daran entscheidet sich, ob
die Maske das Richtige wiederherstellt.
"""
import os

import numpy as np
from PIL import Image

ZIEL = "analysis/befunde_2026-09-21/pri-runde19c-saum"
v = np.asarray(Image.open("build/r19c/abzug_vorher/bild000260.png").convert("RGB")).astype(np.uint8)
n = np.asarray(Image.open("build/r19c/abzug_nachher/bild000260.png").convert("RGB")).astype(np.uint8)
bg = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB")).astype(np.uint8)
bg3 = np.kron(bg, np.ones((3, 3, 1), np.uint8))

x0, x1, y0, y1 = 50, 80, 120, 205
F = 12


def crop(a):
    s = a[y0 * 3:y1 * 3, x0 * 3:x1 * 3]
    return Image.fromarray(s).resize(((x1 - x0) * 3 * F, (y1 - y0) * 3 * F), Image.NEAREST)


b = (x1 - x0) * 3 * F
h = (y1 - y0) * 3 * F
bl = Image.new("RGB", (b * 3 + 24, h), (20, 20, 20))
for i, a in enumerate((v, n, bg3)):
    bl.paste(crop(a), (i * (b + 12), 0))
bl.save(os.path.join(ZIEL, "52_kante_vorher_nachher_bg.png"))
print("geschrieben: 52_kante_vorher_nachher_bg.png  (VORHER | NACHHER | HINTERGRUND, %d-fach)" % (3 * F))

# Zahlen zur Kante: Helligkeit entlang der Spalten 59..63 in den Zeilen 130..160
print("\nHelligkeit (r+g+b) an den Saumpunkten der rechten Kante, Mitte des 3x3-Blocks:")
print("  Zeile | Spalte  vorher / nachher / Hintergrund")
saum = np.load("build/r19c/neu.npy") & ~np.load("build/r19c/alt.npy")
for y in range(138, 156):
    zeile = []
    for x in range(58, 65):
        if not saum[y, x]:
            continue
        py, px = y * 3 + 1, x * 3 + 1
        zeile.append("x%d %3d/%3d/%3d" % (x, int(v[py, px].sum()), int(n[py, px].sum()),
                                          int(bg[y, x].sum())))
    if zeile:
        print("   y%3d | %s" % (y, "   ".join(zeile)))
