# Die neu gefundenen Lagen ansehen: Freistellung bei Massstab 1 ueber dem Hintergrund
# des Cuts, in dem sie einen EINDEUTIGEN Gipfel hat.
import os, sys
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
import maske_aus_png
from geom import load_bg

NEU = [
    ("ROOM1000", 0, [("pri/STAGE1/1000/01_01.png", 0, 47), ("pri/STAGE1/1000/01_02.png", 99, 155),
                     ("pri/STAGE1/1000/01_03.png", 152, 159), ("pri/STAGE1/1000/01_04.png", 280, 112)]),
    ("ROOM1000", 2, [("pri/STAGE1/1000/03_01.png", 0, 62), ("pri/STAGE1/1000/03_02.png", 147, 206),
                     ("pri/STAGE1/1000/03_03.png", 185, 204), ("pri/STAGE1/1000/03_04.png", 259, 64)]),
    ("ROOM11F0", 0, [("pri/STAGE1/11F0/01.png", 191, 88)]),
]
FARBEN = [(255, 40, 40), (40, 255, 80), (60, 140, 255), (255, 220, 40)]
for (room, cut, liste) in NEU:
    rid = int(room[4:], 16)
    bg = load_bg("build/bg_ppm", rid, cut)
    ov = bg.astype(float)
    for i, (png, x, y) in enumerate(liste):
        r = maske_aus_png.setze(png, x, y, 1, alpha_schwelle=110)
        ys, xs = np.nonzero(r)
        print("%s C%d %-14s x=%d y=%d  %5d Punkte  x%d..%d y%d..%d"
              % (room, cut, os.path.basename(png), x, y, r.sum(), xs.min(), xs.max(), ys.min(), ys.max()))
        c = np.array(FARBEN[i % 4], float)
        ov[r] = ov[r] * 0.35 + c * 0.65
    Z = 3
    p = "build/p3/neulage_%s_C%d.png" % (room, cut)
    Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST).save(p)
    print("  ", p)
