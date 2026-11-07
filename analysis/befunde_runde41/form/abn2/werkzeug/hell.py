# Mittlere Helligkeit und Anteil fast schwarzer Bildpunkte je FRAMEDUMP-Bild (eigenes Werkzeug der Abnahme 2).
#   python hell.py <laufdir> [schwelle_mittel]
import sys, os, glob
import numpy as np
from PIL import Image
d = sys.argv[1]; sw = float(sys.argv[2]) if len(sys.argv) > 2 else 30.0
for p in sorted(glob.glob(os.path.join(d, 'fd', 'f*.ppm'))):
    a = np.asarray(Image.open(p).convert('L'), dtype=np.float32)
    m = a.mean(); dunkel = (a < 24).mean() * 100
    f = int(os.path.basename(p)[1:7])
    mark = '  <-- DUNKEL' if m < sw else ''
    print('F%d mittel=%.1f dunkel<24=%.1f%%%s' % (f, m, dunkel, mark))
