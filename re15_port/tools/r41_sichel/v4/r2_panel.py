"""SF V4 Runde 2: Vergleichstafel Original | V4 (b2-Render, beide Vorlage-Kameras), Ausschnitt x 200-560, y 120-470."""
import sys, os
import numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
emd, aus = sys.argv[1], sys.argv[2]
mo = b2.modell(emd); P = G.posen()
rows = []
for b_, k in ((1, '135'), (2, '85')):
    o = G.vorlage(b_).astype(np.uint8)
    img, ids = G.zeichne(mo, P[k])
    rows.append(np.hstack([o[120:470, 200:560], img[120:470, 200:560]]))
cv2.imwrite(aus, cv2.cvtColor(np.vstack(rows), cv2.COLOR_RGB2BGR))
