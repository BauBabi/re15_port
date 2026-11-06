# NB6: Lampe nativ (320x240) - RE2-VRAM (Savestate s04, Puffer y0 = Lampe x -6000; Puffer y240 = x -7000) gegen Port (RE15_WINDOW_SCALE=1).
import numpy as np, sys
from PIL import Image
sys.path.insert(0, ".")
from vram5 import vram, rgb
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb6/lauf"   # NB6-Abschluss 2026-11-06: Laeufe lauf6.sh (fd_leon, fd_elza, nativ, fenster)
v = vram(4)
paare = [("x -6000", rgb(v[0:240, 0:320]).astype(int), sys.argv[1], 1581)]
for name, a, lauf, f in paare:
    p = np.asarray(Image.open("%s/%s/fd/f%06d.ppm" % (L, lauf, f)).convert("RGB")).astype(int)
    print("== Lampe %s: RE2-VRAM s04 | Port %s F%d (Grauwert, Zeilen 118..124, x 76..91)" % (name, lauf, f))
    for y in range(118, 125):
        print("  %3d RE2  " % y + " ".join("%3d" % int(a[y, x].mean()) for x in range(76, 92)))
        print("      Port " + " ".join("%3d" % int(p[y, x].mean()) for x in range(76, 92)))
    d = np.abs(a[110:135, 60:125].astype(int) - p[110:135, 60:125].astype(int)).max(axis=2)
    print("  Feld x 60..124 / y 110..134: max Abweichung %d, Mittel %.2f, Pixel > 16: %d von %d" % (d.max(), d.mean(), (d > 16).sum(), d.size))
