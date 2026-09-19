# Wo genau liegen die Fehltreffer der Massstab-4-Freistellungen? Rand oder Inneres?
# Und wie gross ist der Farbabstand ueberhaupt?
import os, sys
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
from scipy import ndimage
from geom import load_bg

FAELLE = [
    ("ROOM1000", 1, "pri/STAGE1/1000/01_01.png", 188, 54, 4),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_02.png", 181, 207, 4),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_01.png", 139, 91, 4),
    ("ROOM11F0", 1, "pri/STAGE1/11F0/01.png", 99, 161, 4),
    # Gegenprobe: eine 1x-Freistellung, die zu 100 % trifft
    ("ROOM11F0", 6, "pri/STAGE1/11F0/06_02.png", 177, 60, 1),
    ("ROOM10E0", 7, "pri/STAGE1/10E0/07_01.png", 110, 97, 1),
]
SCHWELLE = 110

for (room, cut, png, X, Y, s) in FAELLE:
    rid = int(room[4:], 16)
    bg = load_bg("build/bg_ppm", rid, cut).astype(np.float64)
    im = Image.open(png).convert("RGBA")
    if s > 1:
        im = im.resize((im.width // s, im.height // s), Image.BOX)
    a = np.asarray(im)
    m = a[:, :, 3] > SCHWELLE
    h, w = m.shape
    x0, y0 = max(0, X), max(0, Y)
    x1, y1 = min(320, X + w), min(240, Y + h)
    mm = m[y0 - Y:y1 - Y, x0 - X:x1 - X]
    tt = a[y0 - Y:y1 - Y, x0 - X:x1 - X, :3].astype(np.float64)
    bb = bg[y0:y1, x0:x1]
    d = np.abs(bb - tt).max(2)
    # Abstand jedes Maskenpunkts zum Rand der Silhouette
    dist = ndimage.distance_transform_edt(mm)
    schlecht = mm & (d >= 26)
    n = int(mm.sum()); ns = int(schlecht.sum())
    print("%s C%d %-12s s=%d: %d Punkte, %d Fehltreffer (%.1f %%), mittlerer Farbabstand %.1f"
          % (room, cut, os.path.basename(png), s, n, ns, 100.0 * ns / max(1, n), float(d[mm].mean())))
    if ns:
        print("      Fehltreffer am Rand (Abstand 1): %.0f %%, Abstand >=2: %.0f %%; "
              "alle Punkte: Rand %.0f %%"
              % (100.0 * (dist[schlecht] <= 1).mean(), 100.0 * (dist[schlecht] >= 2).mean(),
                 100.0 * (dist[mm] <= 1).mean()))
        print("      Farbabstand der Fehltreffer: Median %.0f, p90 %.0f, Max %.0f"
              % (np.median(d[schlecht]), np.percentile(d[schlecht], 90), d[schlecht].max()))
