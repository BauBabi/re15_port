"""PPM-Abzuege (Vollbild-Readback vor SDL_RenderPresent) in PNG wandeln und die Helligkeit
des Bildes melden — damit ich erkenne, ob ein Bild noch im Ueberblend-Schwarz liegt.
Aufruf: bild_umwandeln.py <verzeichnis>
"""
import glob
import os
import sys

import numpy as np
from PIL import Image

d = sys.argv[1]
for p in sorted(glob.glob(os.path.join(d, "*.ppm"))):
    im = Image.open(p).convert("RGB")
    a = np.asarray(im).astype(int)
    q = os.path.splitext(p)[0] + ".png"
    im.save(q)
    print("%-44s %dx%d  mittlere Helligkeit %.1f  schwarze Punkte %.1f %%"
          % (os.path.basename(q), im.size[0], im.size[1], a.mean(),
             100.0 * (a.sum(2) < 12).mean()))
