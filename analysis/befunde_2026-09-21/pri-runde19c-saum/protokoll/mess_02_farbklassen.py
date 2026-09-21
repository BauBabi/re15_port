"""Farbklasse (b-r am Hintergrund) der drei Mengen, mit DERSELBEN Definition wie in
pri-runde19b-marke4.md §3.1 (dort: "Chrom in der Freistellung", also die HELLEN Punkte).
"""
import numpy as np
from PIL import Image

bg = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB")).astype(int)
br = bg[..., 2] - bg[..., 0]
hell = bg.sum(2) >= 150
alt = np.load("build/r19c/alt.npy")
gruen = np.load("build/r19c/gruen.npy")
sn = gruen & ~alt
beh = np.zeros((240, 320), bool); beh[118:135, 41:65] = True
for name, mask in (("Freistellung ALLE", alt), ("Freistellung HELL>=150 (Chrom)", alt & hell),
                   ("Saum 187 ALLE", sn), ("Saum 187 HELL>=150", sn & hell),
                   ("Behaelter x41..64 y118..134 hell", beh & hell & ~alt & ~sn)):
    q = np.percentile(br[mask], (25, 50, 75))
    print("%-34s n=%5d  b-r Quartile %+d / %+d / %+d"
          % (name, int(mask.sum()), int(q[0]), int(q[1]), int(q[2])))
